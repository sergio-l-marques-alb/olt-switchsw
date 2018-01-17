/*******************************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 ********************************************************************************
 *
 * @filename fpobj_qosDiffServClassRule.c
 *
 * @purpose
 *
 * @component object handlers
 *
 * @comments  Refer to diffserv-object.xml
 *
 * @create  1 February 2008
 *
 * @author  Rama Sasthri, Kristipati
 * @end
 *
 ********************************************************************************/
#include "fpobj_util.h"
#include "_xe_qosDiffServClassRule_obj.h"
#include "usmdb_mib_diffserv_private_api.h"
#include "diffserv_exports.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"
#include "osapi_support.h"

L7_RC_t diffservConvertEtherTypeKeyIdToString(L7_uint32 etypeKeyId, L7_char8 * strKeyword, L7_uint32 strKeywordSize);
L7_RC_t convertDSCPValToString(L7_uint32 dscpVal, L7_char8* dscpString);
L7_RC_t diffservConvertL4PortValToString(L7_uint32 val, L7_char8 * dscpString, L7_int32 dscpStringLenMax);
xLibRC_t xLibFilterSet (void *arg, xLibId_t oid, xLibU16_t type, xLibU8_t * val, xLibU16_t 
    len);

/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_ClassIndex
 *
 * @purpose Get 'ClassIndex'
 *
 * @description [ClassIndex] The identifier for DiffServ Class entry 
 *
 * @notes  
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_ClassIndex (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIndexValue;
  xLibU32_t nextObjIndexValue;
  L7_RC_t rc = L7_FAILURE;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Index */
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & objIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objIndexValue = 0;
    nextObjIndexValue = 0;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objIndexValue, owa.len);

  rc = L7_FAILURE;	
  while (usmDbDiffServClassGetNext(L7_UNIT_CURRENT, 
        objIndexValue, 
        &nextObjIndexValue) == L7_SUCCESS)
  {
    if (usmDbDiffServClassGet(L7_UNIT_CURRENT, nextObjIndexValue) == L7_SUCCESS)
    {
      rc = L7_SUCCESS;
      break;
    }
  }

  if(rc != L7_SUCCESS)
  {
    owa.l7rc = L7_FAILURE;
  }
  else
  {
    owa.l7rc = L7_SUCCESS;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIndexValue, owa.len);

  /* return the object value: Index */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIndexValue,
      sizeof (objIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_ClassRuleIndex
 *
 * @purpose Get 'ClassRuleIndex'
 *
 * @description [ClassRuleIndex] The identifier for DiffServ Class Rule Table entry within a class
 *
 * @notes  
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_ClassRuleIndex (void *wap, void *bufp)
{

  xLibU32_t objClassIndexValue;
  xLibU32_t nextObjClassIndexValue;
  xLibU32_t objClassRuleIndexValue;
  xLibU32_t nextObjClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & objClassIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objClassIndexValue, owa.len);

  /* retrieve key: ClassRuleIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & objClassRuleIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objClassRuleIndexValue = 0;
    owa.l7rc = usmDbDiffServClassRuleGetNext(L7_UNIT_CURRENT,
        objClassIndexValue,
        objClassRuleIndexValue, 
        &nextObjClassIndexValue,
        &nextObjClassRuleIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objClassRuleIndexValue, owa.len);

    owa.l7rc = usmDbDiffServClassRuleGetNext(L7_UNIT_CURRENT,
        objClassIndexValue,
        objClassRuleIndexValue, 
        &nextObjClassIndexValue,
        &nextObjClassRuleIndexValue);

  }

  if ((objClassIndexValue != nextObjClassIndexValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjClassRuleIndexValue, owa.len);

  /* return the object value: ClassRuleIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjClassRuleIndexValue,
      sizeof (objClassRuleIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchEntryType
 *
 * @purpose Get 'MatchEntryType'
 *
 * @description [MatchEntryType]: Class Rule match entry type, it determines
 *              which one of the individual match conditions is defined for
 *              the present class rule. This object must be created before
 *              any other match object in this row. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchEntryType (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchEntryTypeValue;
  FPOBJ_TRACE_ENTER (bufp);


  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchEntryTypeGet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        &objMatchEntryTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchEntryType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMatchEntryTypeValue,
      sizeof (objMatchEntryTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchEntryType
 *
 * @purpose Set 'MatchEntryType'
 *
 * @description [MatchEntryType]: Class Rule match entry type, it determines
 *              which one of the individual match conditions is defined for
 *              the present class rule. This object must be created before
 *              any other match object in this row. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchEntryType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchEntryTypeValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MatchEntryType */
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) & objMatchEntryTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMatchEntryTypeValue, owa.len);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchEntryTypeSet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        objMatchEntryTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    /* Delete the existing row if failed to set the value */
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc =  XLIBRC_CLASS_RULE_NOT_CREATED;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;

  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchCos
 *
 * @purpose Get 'MatchCos'
 *
 * @description [MatchCos]: Three-bit user priority field in the 802.1Q tag
 *              header of a tagged Ethernet frame used as a class-match parameter
 *              - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to cos(1). For frames containing a doubl 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchCos (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchCosValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchCosGet (L7_UNIT_CURRENT, keyClassIndexValue,
        keyClassRuleIndexValue,
        &objMatchCosValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchCos */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMatchCosValue,
      sizeof (objMatchCosValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/******************************************************************************
 *
 * @function fpObjList_qosDiffServClassRule_MatchCos
 *
 * @purpose List 'MatchCos'
 *@description  [MatchCos] Three-bit user priority field in the 802.1Q tag
 * header of a tagged Ethernet frame used as a class-match parameter -
 * only valid if the agentDiffServClassRuleMatchEntryType is set to
 * cos(1). For frames containing a double
 * @notes
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjList_qosDiffServClassRule_MatchCos (void *wap, void *bufp)
{
  return fpObjAppGetKey_U32Number(wap, bufp,XOBJ_qosDiffServClassRule_MatchCos,
      0, 7, 1);
}

/******************************************************************************
 *
 * @function fpObjList_qosDiffServClassRule_MatchCos2
 *
 * @purpose List 'MatchCos2'
 *@description  [MatchCos2] Three-bit user priority field in the second/inner
 * 802.1Q tag header of a double VLAN tagged Ethernet frame used as a
 * class-match parameter - only valid if the
 * agentDiffServClassRuleMatchEntryType is set to cos2(15).
 * @notes
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjList_qosDiffServClassRule_MatchCos2 (void *wap, void *bufp)
{
  return fpObjAppGetKey_U32Number(wap, bufp,XOBJ_qosDiffServClassRule_MatchCos2,
      0, 7, 1);
}

/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchCos
 *
 * @purpose Set 'MatchCos'
 *
 * @description [MatchCos]: Three-bit user priority field in the 802.1Q tag
 *              header of a tagged Ethernet frame used as a class-match parameter
 *              - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to cos(1). For frames containing a doubl 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchCos (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchCosValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MatchCos */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMatchCosValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMatchCosValue, owa.len);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbDiffServClassRuleMatchEntryTypeGet(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue, &entryType);
  if(owa.l7rc != L7_SUCCESS || entryType != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue,keyClassRuleIndexValue);
    owa.rc =  XLIBRC_CLASS_RULE_NOT_CREATED;   
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (usmDbFeaturePresentCheck(L7_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_EXCLUDE_FEATURE_ID) != L7_TRUE)
  {
  owa.l7rc = usmDbDiffServClassRuleMatchExcludeFlagSet(L7_UNIT_CURRENT, keyClassIndexValue,
      keyClassRuleIndexValue, L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  }
  /* set the value in application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchCosSet (L7_UNIT_CURRENT, keyClassIndexValue,
        keyClassRuleIndexValue,
        objMatchCosValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc =  XLIBRC_CLASS_RULE_NOT_CREATED; 
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchDstIpAddr
 *
 * @purpose Get 'MatchDstIpAddr'
 *
 * @description [MatchDstIpAddr]: Destination IP address match value for the
 *              class - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to dstip(2). 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchDstIpAddr (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchDstIpAddrValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchDstIpAddrGet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        &objMatchDstIpAddrValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchDstIpAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMatchDstIpAddrValue,
      sizeof (objMatchDstIpAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchDstIpAddr
 *
 * @purpose Set 'MatchDstIpAddr'
 *
 * @description [MatchDstIpAddr]: Destination IP address match value for the
 *              class - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to dstip(2). 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchDstIpAddr (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchDstIpAddrValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t kwaIpMask = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t tempIpMask;
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MatchDstIpAddr */
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) & objMatchDstIpAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMatchDstIpAddrValue, owa.len);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);


  /* Get the mask value */
  kwaIpMask.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_MatchDstIpMask,
      (xLibU8_t *) & tempIpMask, &kwaIpMask.len);
  if (kwaIpMask.rc != XLIBRC_SUCCESS)
  {
    kwaIpMask.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIpMask);
    return kwaIpMask.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbDiffServClassRuleMatchEntryTypeGet(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue, &entryType);
  if(owa.l7rc != L7_SUCCESS || entryType != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIP)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue,keyClassRuleIndexValue);
    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (usmDbFeaturePresentCheck(L7_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_EXCLUDE_FEATURE_ID) != L7_TRUE)
  {
  owa.l7rc = usmDbDiffServClassRuleMatchExcludeFlagSet(L7_UNIT_CURRENT, keyClassIndexValue,
      keyClassRuleIndexValue, L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
    }
  }

  owa.l7rc =
    usmDbDiffServClassRuleMatchDstIpAddrSet (L7_UNIT_CURRENT, keyClassIndexValue,
        keyClassRuleIndexValue,
        objMatchDstIpAddrValue);
  if(owa.l7rc == L7_SUCCESS )
  {
    owa.l7rc =
      usmDbDiffServClassRuleMatchDstIpMaskSet (L7_UNIT_CURRENT, keyClassIndexValue,
          keyClassRuleIndexValue,
          tempIpMask);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchDstIpMask
 *
 * @purpose Get 'MatchDstIpMask'
 *
 * @description [MatchDstIpMask]: Destination IP address mask match value for
 *              the class - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to dstip(2). This mask value identifies the
 *              portion of agentDiffServClassRuleMatchDstIpAddr that is
 *              
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchDstIpMask (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchDstIpMaskValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchDstIpMaskGet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        &objMatchDstIpMaskValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchDstIpMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMatchDstIpMaskValue,
      sizeof (objMatchDstIpMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchDstIpMask
 *
 * @purpose Set 'MatchDstIpMask'
 *
 * @description [MatchDstIpMask]: Destination IP address mask match value for
 *              the class - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to dstip(2). This mask value identifies the
 *              portion of agentDiffServClassRuleMatchDstIpAddr that is
 *              
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchDstIpMask (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchDstIpMaskValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MatchDstIpMask */
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) & objMatchDstIpMaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMatchDstIpMaskValue, owa.len);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = L7_SUCCESS;
  owa.rc = XLIBRC_SUCCESS;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchDstL4PortStart
 *
 * @purpose Get 'MatchDstL4PortStart'
 *
 * @description [MatchDstL4PortStart]: Destination layer 4 port range start
 *              match value for the class - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to dstl4port(3). 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchDstL4PortStart (void *wap,
    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchDstL4PortStartValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchDstL4PortStartGet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        &objMatchDstL4PortStartValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchDstL4PortStart */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMatchDstL4PortStartValue,
      sizeof (objMatchDstL4PortStartValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchDstL4PortStart
 *
 * @purpose Set 'MatchDstL4PortStart'
 *
 * @description [MatchDstL4PortStart]: Destination layer 4 port range start
 *              match value for the class - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to dstl4port(3). 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchDstL4PortStart (void *wap,
    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchDstL4PortStartValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MatchDstL4PortStart */
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) & objMatchDstL4PortStartValue,
      &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMatchDstL4PortStartValue, owa.len);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  owa.l7rc = L7_SUCCESS;
  owa.rc = XLIBRC_SUCCESS;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchDstL4PortEnd
 *
 * @purpose Get 'MatchDstL4PortEnd'
 *
 * @description [MatchDstL4PortEnd]: Destination layer 4 port range end match
 *              value for the class - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to dstl4port(3). 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchDstL4PortEnd (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchDstL4PortEndValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchDstL4PortEndGet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        &objMatchDstL4PortEndValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchDstL4PortEnd */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMatchDstL4PortEndValue,
      sizeof (objMatchDstL4PortEndValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchDstL4PortEnd
 *
 * @purpose Set 'MatchDstL4PortEnd'
 *
 * @description [MatchDstL4PortEnd]: Destination layer 4 port range end match
 *              value for the class - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to dstl4port(3). 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchDstL4PortEnd (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchDstL4PortEndValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t kwaPort = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchDstL4PortStartValue;
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MatchDstL4PortEnd */
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) & objMatchDstL4PortEndValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMatchDstL4PortEndValue, owa.len);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  kwaPort.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServClassRule_MatchDstL4PortStart ,
        (xLibU8_t *) & objMatchDstL4PortStartValue, &kwaPort.len);
  if ( objMatchDstL4PortEndValue < objMatchDstL4PortStartValue)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    kwaPort.rc = XLIBRC_DIFFSERV_ENDPORT_NOT_VALID;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwaPort);
    return kwaPort.rc;
  }

  owa.l7rc = usmDbDiffServClassRuleMatchEntryTypeGet(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue, &entryType);
  if(owa.l7rc != L7_SUCCESS || entryType != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTL4PORT)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (usmDbFeaturePresentCheck(L7_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_EXCLUDE_FEATURE_ID) != L7_TRUE)
  {
    owa.l7rc = usmDbDiffServClassRuleMatchExcludeFlagSet(L7_UNIT_CURRENT, keyClassIndexValue,
        keyClassRuleIndexValue, L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  }

  /* set the value in application */

  owa.l7rc =
    usmDbDiffServClassRuleMatchDstL4PortStartSet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        objMatchDstL4PortStartValue);

  if (owa.l7rc == L7_SUCCESS)
  {

    owa.l7rc =
      usmDbDiffServClassRuleMatchDstL4PortEndSet (L7_UNIT_CURRENT,
          keyClassIndexValue,
          keyClassRuleIndexValue,
          objMatchDstL4PortEndValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchDstMacAddr
 *
 * @purpose Get 'MatchDstMacAddr'
 *
 * @description [MatchDstMacAddr]: Destination MAC address match value for
 *              the class - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to dstmac(4). 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchDstMacAddr (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t objMatchDstMacAddrValue;
  xLibU32_t stringLengthValue = sizeof(objMatchDstMacAddrValue);
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchDstMacAddrGet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        objMatchDstMacAddrValue,
        &stringLengthValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchDstMacAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objMatchDstMacAddrValue,
      sizeof (objMatchDstMacAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchDstMacAddr
 *
 * @purpose Set 'MatchDstMacAddr'
 *
 * @description [MatchDstMacAddr]: Destination MAC address match value for
 *              the class - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to dstmac(4). 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchDstMacAddr (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t objMatchDstMacAddrValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t kwaMacMask = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t tempMacMask;
	L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType;

  FPOBJ_TRACE_ENTER (bufp);


  /* retrieve object: MatchDstMacAddr */
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) objMatchDstMacAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objMatchDstMacAddrValue, owa.len);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* retrieve key: Mac Mask */
  kwaMacMask.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_MatchDstMacMask,
      (xLibU8_t *)  tempMacMask, &kwaMacMask.len);
  if (kwaMacMask.rc != XLIBRC_SUCCESS)
  {
    kwaMacMask.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaMacMask);
    return kwaMacMask.rc;
  }
  owa.l7rc = usmDbDiffServClassRuleMatchEntryTypeGet(L7_UNIT_CURRENT,keyClassIndexValue, keyClassRuleIndexValue, &entryType);
  if(owa.l7rc != L7_SUCCESS || entryType != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTMAC)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  if (usmDbFeaturePresentCheck(L7_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_EXCLUDE_FEATURE_ID) != L7_TRUE)
  {
    owa.l7rc = usmDbDiffServClassRuleMatchExcludeFlagSet(L7_UNIT_CURRENT, keyClassIndexValue,
        keyClassRuleIndexValue, L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  }

  owa.l7rc =
    usmDbDiffServClassRuleMatchDstMacAddrSet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        objMatchDstMacAddrValue);
  if (owa.l7rc == L7_SUCCESS)
  {

    owa.l7rc =
      usmDbDiffServClassRuleMatchDstMacMaskSet (L7_UNIT_CURRENT,
          keyClassIndexValue,
          keyClassRuleIndexValue,
          tempMacMask);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchDstMacMask
 *
 * @purpose Get 'MatchDstMacMask'
 *
 * @description [MatchDstMacMask]: Destination MAC address mask match value
 *              for the class - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to dstmac(4). This mask value identifies
 *              the portion of agentDiffServClassRuleMatchDstMacAddr that
 *              is 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchDstMacMask (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t objMatchDstMacMaskValue;
  xLibU32_t stringLengthValue = sizeof(objMatchDstMacMaskValue);
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchDstMacMaskGet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        objMatchDstMacMaskValue,
        &stringLengthValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchDstMacMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objMatchDstMacMaskValue,
      strlen (objMatchDstMacMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchDstMacMask
 *
 * @purpose Set 'MatchDstMacMask'
 *
 * @description [MatchDstMacMask]: Destination MAC address mask match value
 *              for the class - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to dstmac(4). This mask value identifies
 *              the portion of agentDiffServClassRuleMatchDstMacAddr that
 *              is 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchDstMacMask (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t  objMatchDstMacMaskValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MatchDstMacMask */
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) objMatchDstMacMaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objMatchDstMacMaskValue, owa.len);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = L7_SUCCESS;
  owa.rc = XLIBRC_SUCCESS;

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchEvery
 *
 * @purpose Get 'MatchEvery'
 *
 * @description [MatchEvery]: Flag to indicate that the class rule is defined
 *              to match on every packet, regardless of content. - only
 *              valid if the agentDiffServClassRuleMatchEntryType is set to
 *              every(5). 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchEvery (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchEveryValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchEveryFlagGet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        &objMatchEveryValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchEvery */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMatchEveryValue,
      sizeof (objMatchEveryValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchEvery
 *
 * @purpose Set 'MatchEvery'
 *
 * @description [MatchEvery]: Flag to indicate that the class rule is defined
 *              to match on every packet, regardless of content. - only
 *              valid if the agentDiffServClassRuleMatchEntryType is set to
 *              every(5). 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchEvery (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t entryType;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

/* get the value from application */
  owa.l7rc = usmDbDiffServClassRuleMatchEntryTypeGet(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue, &entryType);
  if(owa.l7rc != L7_SUCCESS || entryType !=  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_EVERY)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue,keyClassRuleIndexValue);
    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (usmDbFeaturePresentCheck(L7_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_EXCLUDE_FEATURE_ID) != L7_TRUE)
  {
  owa.l7rc = usmDbDiffServClassRuleMatchExcludeFlagSet(L7_UNIT_CURRENT, keyClassIndexValue,
      keyClassRuleIndexValue, L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc =XLIBRC_CLASS_RULE_NOT_CREATED;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  }
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchIpDscp
 *
 * @purpose Get 'MatchIpDscp'
 *
 * @description [MatchIpDscp]: IP DiffServ Code Point (DSCP) match value for
 *              the class - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to ipdscp(6). The DSCP is defined as the high-order
 *              six bits of the Service Type octet in the 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchIpDscp (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchIpDscpValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchIpDscpGet (L7_UNIT_CURRENT, keyClassIndexValue,
        keyClassRuleIndexValue,
        &objMatchIpDscpValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchIpDscp */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMatchIpDscpValue,
      sizeof (objMatchIpDscpValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/******************************************************************************
 *
 * @function fpObjList_qosDiffServClassRule_MatchIpDscp
 *
 * @purpose List 'MatchIpDscp'
 *@description  [MatchIpDscp] IP DiffServ Code Point (DSCP) match value for the
 * class - only valid if the agentDiffServClassRuleMatchEntryType is
 * set to ipdscp(6). The DSCP is defined as the high-order six bits
 * of the Service Type octet in the
 * @notes
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjList_qosDiffServClassRule_MatchIpDscp (void *wap, void *bufp)
{
  return fpObjAppGetKey_U32Number(wap, bufp,XOBJ_qosDiffServClassRule_MatchIpDscp,
      0, 63, 1);
}


/******************************************************************************
 *
 * @function fpObjList_qosDiffServClassRule_MatchIpPrecedence
 *
 * @purpose List 'MatchIpPrecedence'
 *@description  [MatchIpPrecedence] IP Precedence match value for the class -
 * only valid if the agentDiffServClassRuleMatchEntryType is set to
 * ipprecedence(7). The Precedence bits are defined as the high-order
 * three bits of the Service Type octed in the IPv4 header.
 * @notes
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjList_qosDiffServClassRule_MatchIpPrecedence (void *wap, void *
    bufp)
{
  return fpObjAppGetKey_U32Number(wap, bufp,XOBJ_qosDiffServClassRule_MatchIpPrecedence,
      0, 7, 1);
}

/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchIpDscp
 *
 * @purpose Set 'MatchIpDscp'
 *
 * @description [MatchIpDscp]: IP DiffServ Code Point (DSCP) match value for
 *              the class - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to ipdscp(6). The DSCP is defined as the high-order
 *              six bits of the Service Type octet in the 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchIpDscp (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchIpDscpValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
	L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType;
	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MatchIpDscp */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMatchIpDscpValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMatchIpDscpValue, owa.len);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbDiffServClassRuleMatchEntryTypeGet(L7_UNIT_CURRENT, 
      keyClassIndexValue, keyClassRuleIndexValue, 
      &entryType);
  if(owa.l7rc != L7_SUCCESS || entryType != 
      L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, 
        keyClassRuleIndexValue);
    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (usmDbFeaturePresentCheck(L7_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_EXCLUDE_FEATURE_ID) != L7_TRUE)
  {
  owa.l7rc = usmDbDiffServClassRuleMatchExcludeFlagSet(L7_UNIT_CURRENT, keyClassIndexValue,
      keyClassRuleIndexValue, L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
    }
  }



  owa.l7rc =
    usmDbDiffServClassRuleMatchIpDscpSet (L7_UNIT_CURRENT, keyClassIndexValue,
        keyClassRuleIndexValue,
        objMatchIpDscpValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchIpPrecedence
 *
 * @purpose Get 'MatchIpPrecedence'
 *
 * @description [MatchIpPrecedence]: IP Precedence match value for the class
 *              - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to ipprecedence(7). The Precedence bits are defined
 *              as the high-order three bits of the Service Type octed in the
 *              IPv4 header. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchIpPrecedence (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchIpPrecedenceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchIpPrecedenceGet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        &objMatchIpPrecedenceValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchIpPrecedence */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMatchIpPrecedenceValue,
      sizeof (objMatchIpPrecedenceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchIpPrecedence
 *
 * @purpose Set 'MatchIpPrecedence'
 *
 * @description [MatchIpPrecedence]: IP Precedence match value for the class
 *              - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to ipprecedence(7). The Precedence bits are defined
 *              as the high-order three bits of the Service Type octed in the
 *              IPv4 header. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchIpPrecedence (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchIpPrecedenceValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
	L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType;
	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MatchIpPrecedence */
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) & objMatchIpPrecedenceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMatchIpPrecedenceValue, owa.len);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbDiffServClassRuleMatchEntryTypeGet(L7_UNIT_CURRENT, 
      keyClassIndexValue, keyClassRuleIndexValue, 
      &entryType);
  if(owa.l7rc != L7_SUCCESS || entryType != 
      L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, 
        keyClassRuleIndexValue);
    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (usmDbFeaturePresentCheck(L7_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_EXCLUDE_FEATURE_ID) != L7_TRUE)
  {
  owa.l7rc = usmDbDiffServClassRuleMatchExcludeFlagSet(L7_UNIT_CURRENT, keyClassIndexValue,
      keyClassRuleIndexValue, L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
    }
  }

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchIpPrecedenceSet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        objMatchIpPrecedenceValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchIpTosBits
 *
 * @purpose Get 'MatchIpTosBits'
 *
 * @description [MatchIpTosBits]: IP TOS bits match value for the class - only
 *              valid if the agentDiffServClassRuleMatchEntryType is set
 *              to iptos(8). The TOS bits are defined as all eight bits of
 *              the Service Type octet in the IPv4 header. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchIpTosBits (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objMatchIpTosBitsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchIpTosBitsGet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        objMatchIpTosBitsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchIpTosBits */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objMatchIpTosBitsValue,
      strlen (objMatchIpTosBitsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_qosDiffServClassRule_MatchIpTosBits
*
* @purpose Set 'MatchIpTosBits'
 *@description  [MatchIpTosBits] IP TOS bits match value for the class - only
* valid if the agentDiffServClassRuleMatchEntryType is set to
* iptos(8). The TOS bits are defined as all eight bits of the Service Type
* octet in the IPv4 header.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchIpTosBits (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objMatchIpTosBitsValue;

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyMatchIpTosMaskValue;


  xLibU32_t keyClassIndexValue;
  xLibU32_t keyClassRuleIndexValue;
  
  xLibU8_t tosBitsValue[5];  
  xLibU8_t strTosBits[5];
  xLibU8_t tosMaskValue[5];
  xLibU8_t strTosMask[5];

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MatchIpTosBits */
  owa.len = sizeof (objMatchIpTosBitsValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objMatchIpTosBitsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objMatchIpTosBitsValue, owa.len);

  memset (tosBitsValue, 0, sizeof(tosBitsValue));  
  osapiSnprintf(tosBitsValue, sizeof(tosBitsValue), objMatchIpTosBitsValue);
  if (usmDbConvertTwoDigitHex(tosBitsValue, strTosBits) == L7_FALSE)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* retrieve key: MatchIpTosMask */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_MatchIpTosMask,
                           (xLibU8_t *) keyMatchIpTosMaskValue,
                           &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyMatchIpTosMaskValue, kwa1.len);

  memset (tosMaskValue, 0, sizeof(tosMaskValue));
  osapiSnprintf(tosMaskValue, sizeof(tosMaskValue), keyMatchIpTosMaskValue);
  if (usmDbConvertTwoDigitHex(tosMaskValue, strTosMask) == L7_FALSE)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* retrieve key: ClassIndex */
  owa.len = sizeof (keyClassIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
                          (xLibU8_t *) & keyClassIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, owa.len);

  /* retrieve key: ClassRuleIndex */
  owa.len = sizeof (keyClassRuleIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
                          (xLibU8_t *) & keyClassRuleIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbDiffServClassRuleMatchEntryTypeGet(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue, &entryType);
  if(owa.l7rc != L7_SUCCESS || entryType != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (usmDbFeaturePresentCheck(L7_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_EXCLUDE_FEATURE_ID) != L7_TRUE)
  {
  owa.l7rc = usmDbDiffServClassRuleMatchExcludeFlagSet(L7_UNIT_CURRENT, keyClassIndexValue,
      keyClassRuleIndexValue, L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
    }
  }

  owa.l7rc = usmDbDiffServClassRuleMatchIpTosBitsSet (L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue, (xLibU8_t)(*strTosBits));

  if (owa.l7rc == L7_SUCCESS)
  {
    owa.l7rc = usmDbDiffServClassRuleMatchIpTosMaskSet (L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue, (xLibU8_t)(*strTosMask));
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
    
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchIpTosMask
 *
 * @purpose Get 'MatchIpTosMask'
 *
 * @description [MatchIpTosMask]: IP TOS bits mask match value for the class
 *              - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to iptos(8). This mask value identifies the portion
 *              of agentDiffServClassRuleMatchIpTosBits that is compared against
 *              a packet 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchIpTosMask (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objMatchIpTosMaskValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchIpTosMaskGet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        objMatchIpTosMaskValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchIpTosMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objMatchIpTosMaskValue,
      strlen (objMatchIpTosMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchIpTosMask
 *
 * @purpose Set 'MatchIpTosMask'
 *
 * @description [MatchIpTosMask]: IP TOS bits mask match value for the class
 *              - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to iptos(8). This mask value identifies the portion
 *              of agentDiffServClassRuleMatchIpTosBits that is compared against
 *              a packet 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchIpTosMask (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objMatchIpTosMaskValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MatchIpTosMask */

  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objMatchIpTosMaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objMatchIpTosMaskValue, owa.len);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = L7_SUCCESS;
  owa.rc = XLIBRC_SUCCESS;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchProtocolNum
 *
 * @purpose Get 'MatchProtocolNum'
 *
 * @description [MatchProtocolNum]: Protocol number match value for the class
 *              - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to protocol(9). 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchProtocolNum (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchProtocolNumValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchProtocolNumGet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        &objMatchProtocolNumValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchProtocolNum */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMatchProtocolNumValue,
      sizeof (objMatchProtocolNumValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/******************************************************************************
 *
 * @function fpObjList_qosDiffServClassRule_MatchProtocolNum
 *
 * @purpose List 'MatchProtocolNum'
 *@description  [MatchProtocolNum] Protocol number match value for the class -
 * only valid if the agentDiffServClassRuleMatchEntryType is set to
 * protocol(9).
 * @notes
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjList_qosDiffServClassRule_MatchProtocolNum (void *wap, void *
    bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objMatchProtocolNumValue;
  xLibU32_t nextObjMatchProtocolNumValue;

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (objMatchProtocolNumValue);
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_MatchProtocolNum,
      (xLibU8_t *) & objMatchProtocolNumValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbGetFirstUnknown (L7_UNIT_CURRENT, &
        nextObjMatchProtocolNumValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objMatchProtocolNumValue, owa.len);
    owa.l7rc =
      usmDbGetNextUnknown (L7_UNIT_CURRENT, objMatchProtocolNumValue,
          &nextObjMatchProtocolNumValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjMatchProtocolNumValue, owa.len);

  /* return the object value: MatchProtocolNum */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjMatchProtocolNumValue,
      sizeof (nextObjMatchProtocolNumValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchProtocolNum
 *
 * @purpose Set 'MatchProtocolNum'
 *
 * @description [MatchProtocolNum]: Protocol number match value for the class
 *              - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to protocol(9). 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchProtocolNum (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchProtocolNumValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
	L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType;
	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MatchProtocolNum */
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) & objMatchProtocolNumValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMatchProtocolNumValue, owa.len);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbDiffServClassRuleMatchEntryTypeGet(L7_UNIT_CURRENT, 
      keyClassIndexValue, keyClassRuleIndexValue, 
      &entryType);
  if(owa.l7rc != L7_SUCCESS || entryType != 
      L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_PROTOCOL)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, 
        keyClassRuleIndexValue);
    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (usmDbFeaturePresentCheck(L7_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_EXCLUDE_FEATURE_ID) != L7_TRUE)
  {
  owa.l7rc = usmDbDiffServClassRuleMatchExcludeFlagSet(L7_UNIT_CURRENT, keyClassIndexValue,
      keyClassRuleIndexValue, L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
    }
  }

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchProtocolNumSet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        objMatchProtocolNumValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchRefClassIndex
 *
 * @purpose Get 'MatchRefClassIndex'
 *
 * @description [MatchRefClassIndex]: Index of the corresponding referenced
 *              class - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to refclass(10). The set of match conditions defined
 *              for the reference class are directly inherited by the
 *              current class. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchRefClassIndex (void *wap,
    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchRefClassIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchRefClassIndexGet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        &objMatchRefClassIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchRefClassIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMatchRefClassIndexValue,
      sizeof (objMatchRefClassIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchRefClassIndex
 *
 * @purpose Get 'MatchRefClassIndex'
 *
 * @description [MatchRefClassIndex]: Index of the corresponding referenced
 *              class - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to refclass(10). The set of match conditions defined
 *              for the reference class are directly inherited by the
 *              current class. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchRefClassName (void *wap,
    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  xLibU32_t objMatchRefClassIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t refClassName;
  xLibU32_t strLen;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);
  owa.l7rc = usmDbDiffServClassToRefClass(L7_UNIT_CURRENT,keyClassIndexValue,&objMatchRefClassIndexValue);
  memset(refClassName,0x0,sizeof(refClassName));
  /* get the value from application */
  if (owa.l7rc != L7_SUCCESS)
  {
    osapiStrncpy(refClassName,"",sizeof(refClassName));
  }
  else
  {
    strLen = sizeof(refClassName);
    owa.l7rc = usmDbDiffServClassNameGet(L7_UNIT_CURRENT, objMatchRefClassIndexValue, refClassName, 
        &strLen);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  owa.rc = XLIBRC_SUCCESS;
  /* return the object value: MatchRefClassIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)  refClassName,
      sizeof (refClassName));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
 * @function fpObjList_qosDiffServClassRule_MatchRefClassName
 *
 * @purpose Get 'MatchRefClassName'
 *
 * @description [MatchRefClassName]: Index of the corresponding referenced
 *              class - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to refclass(10). The set of match conditions defined
 *              for the reference class are directly inherited by the
 *              current class. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjList_qosDiffServClassRule_MatchRefClassName (void *wap,
    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t referenceName;
  xLibU32_t objMatchRefClassIndexValue;
  xLibU32_t nextObjIndexValue;
  xLibU32_t strLen;

  FPOBJ_TRACE_ENTER (bufp);

  memset(referenceName,0x0,sizeof(referenceName));
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) &keyClassIndexValue, &kwa1.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    keyClassIndexValue = 0;
  }

  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_MatchRefClassName,
      (xLibU8_t *) referenceName, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    objMatchRefClassIndexValue = 0;
  }
  else
  {
    owa.l7rc = usmDbDiffServClassNameToIndex(L7_UNIT_CURRENT,referenceName,&objMatchRefClassIndexValue);
    if(owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }

  memset(referenceName,0x0,sizeof(referenceName));
  owa.l7rc = L7_FAILURE;

  while (usmDbDiffServClassGetNext(L7_UNIT_CURRENT, 
        objMatchRefClassIndexValue, 
        &nextObjIndexValue) == L7_SUCCESS)
  {
    if (usmDbDiffServClassGet(L7_UNIT_CURRENT, nextObjIndexValue) == L7_SUCCESS)
    {
      strLen = sizeof(referenceName);
      if(keyClassIndexValue == nextObjIndexValue)
      {

        objMatchRefClassIndexValue = nextObjIndexValue;
        continue;
      }
      owa.l7rc = usmDbDiffServClassNameGet(L7_UNIT_CURRENT, nextObjIndexValue, referenceName, 
          &strLen);
    }
    break;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchRefClassIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) referenceName,
      strlen(referenceName));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchRefClassIndex
 *
 * @purpose Set 'MatchRefClassIndex'
 *
 * @description [MatchRefClassIndex]: Index of the corresponding referenced
 *              class - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to refclass(10). The set of match conditions defined
 *              for the reference class are directly inherited by the
 *              current class. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchRefClassIndex (void *wap,
    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objMatchRefClassName;
  xLibU32_t    objMatchRefClassIndexValue ;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
	L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType;
	
  FPOBJ_TRACE_ENTER (bufp);

  memset(objMatchRefClassName,0x0,sizeof(objMatchRefClassName)); 
  /* retrieve object: MatchRefClassIndex */
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) & objMatchRefClassName, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMatchRefClassName, owa.len);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  owa.l7rc = usmDbDiffServClassRuleMatchEntryTypeGet(L7_UNIT_CURRENT, 
      keyClassIndexValue, keyClassRuleIndexValue, 
      &entryType);
  if(owa.l7rc != L7_SUCCESS || entryType != 
      L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, 
        keyClassRuleIndexValue);
    owa.rc = XLIBRC_DIFFSERV_REFER_CLASS_ERROR;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (usmDbFeaturePresentCheck(L7_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_EXCLUDE_FEATURE_ID) != L7_TRUE)
  {
  owa.l7rc = usmDbDiffServClassRuleMatchExcludeFlagSet(L7_UNIT_CURRENT, keyClassIndexValue,
      keyClassRuleIndexValue, L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_DIFFSERV_REFER_CLASS_ERROR;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  }

  /* make sure refereence classId specified is already existing */
  owa.l7rc = 
    usmDbDiffServClassNameToIndex(L7_UNIT_CURRENT,objMatchRefClassName,&objMatchRefClassIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_DIFFSERV_CLASS_NOT_EXISTS;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchRefClassIndexSet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        objMatchRefClassIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_DIFFSERV_REFER_CLASS_ERROR;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchRefClassName
 *
 * @purpose Set 'MatchRefClassIndex'
 *
 * @description [MatchRefClassIndex]: Index of the corresponding referenced
 *              class - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to refclass(10). The set of match conditions defined
 *              for the reference class are directly inherited by the
 *              current class. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchRefClassName (void *wap,
    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t referenceName;
  xLibU32_t objMatchRefClassIndexValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
	L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType;
	
  FPOBJ_TRACE_ENTER (bufp);

  memset(referenceName,0x0,sizeof(referenceName)); 
  /* retrieve object: MatchRefClassIndex */
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) referenceName, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMatchRefClassIndexValue, owa.len);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);
  owa.l7rc = usmDbDiffServClassNameToIndex(L7_UNIT_CURRENT, referenceName, 
      &objMatchRefClassIndexValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  owa.l7rc = usmDbDiffServClassRuleMatchEntryTypeGet(L7_UNIT_CURRENT,keyClassIndexValue, keyClassRuleIndexValue, &entryType);
if(owa.l7rc != L7_SUCCESS || entryType != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS)
{
  usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue,keyClassRuleIndexValue);
  owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

  if (usmDbFeaturePresentCheck(L7_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_EXCLUDE_FEATURE_ID) != L7_TRUE)
  {
  owa.l7rc = usmDbDiffServClassRuleMatchExcludeFlagSet(L7_UNIT_CURRENT, keyClassIndexValue,
      keyClassRuleIndexValue, L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  }


  /* make sure refereence classId specified is already existing */
  owa.l7rc = usmDbDiffServClassGet(L7_UNIT_CURRENT, objMatchRefClassIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_DIFFSERV_CLASS_NOT_EXISTS;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchRefClassIndexSet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        objMatchRefClassIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_DIFFSERV_REFER_CLASS_ERROR;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchSrcIpAddr
 *
 * @purpose Get 'MatchSrcIpAddr'
 *
 * @description [MatchSrcIpAddr]: Source IP address match value for the class
 *              - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to srcip(11). 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchSrcIpAddr (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchSrcIpAddrValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchSrcIpAddrGet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        &objMatchSrcIpAddrValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchSrcIpAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMatchSrcIpAddrValue,
      sizeof (objMatchSrcIpAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchSrcIpAddr
 *
 * @purpose Set 'MatchSrcIpAddr'
 *
 * @description [MatchSrcIpAddr]: Source IP address match value for the class
 *              - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to srcip(11). 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchSrcIpAddr (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchSrcIpAddrValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;

  fpObjWa_t kwaSrcIpMask = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t tempSrcIpMaskValue;
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MatchSrcIpAddr */
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) & objMatchSrcIpAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMatchSrcIpAddrValue, owa.len);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* retrieve key: MatchSrcIpMask */
  kwaSrcIpMask.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_MatchSrcIpMask,
      (xLibU8_t *) & tempSrcIpMaskValue, &kwaSrcIpMask.len);
  if (kwaSrcIpMask.rc != XLIBRC_SUCCESS)
  {
    kwaSrcIpMask.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaSrcIpMask);
    return kwaSrcIpMask.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbDiffServClassRuleMatchEntryTypeGet(L7_UNIT_CURRENT, 
      keyClassIndexValue, keyClassRuleIndexValue, 
      &entryType);
  if(owa.l7rc != L7_SUCCESS || entryType != 
      L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIP)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, 
        keyClassRuleIndexValue);
    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (usmDbFeaturePresentCheck(L7_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_EXCLUDE_FEATURE_ID) != L7_TRUE)
  {
  owa.l7rc = usmDbDiffServClassRuleMatchExcludeFlagSet(L7_UNIT_CURRENT, keyClassIndexValue,
      keyClassRuleIndexValue, L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
    }
  }



  owa.l7rc =
    usmDbDiffServClassRuleMatchSrcIpAddrSet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        objMatchSrcIpAddrValue);
  if (owa.l7rc == L7_SUCCESS)
  {

    owa.l7rc =
      usmDbDiffServClassRuleMatchSrcIpMaskSet (L7_UNIT_CURRENT,
          keyClassIndexValue,
          keyClassRuleIndexValue,
          tempSrcIpMaskValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchSrcIpMask
 *
 * @purpose Get 'MatchSrcIpMask'
 *
 * @description [MatchSrcIpMask]: Source IP address mask match value for the
 *              class - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to srcip(11). This mask value identifies the portion
 *              of agentDiffServClassRuleMatchSrcIpAddr that is compared
 *              against a 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchSrcIpMask (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchSrcIpMaskValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchSrcIpMaskGet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        &objMatchSrcIpMaskValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchSrcIpMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMatchSrcIpMaskValue,
      sizeof (objMatchSrcIpMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchSrcIpMask
 *
 * @purpose Set 'MatchSrcIpMask'
 *
 * @description [MatchSrcIpMask]: Source IP address mask match value for the
 *              class - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to srcip(11). This mask value identifies the portion
 *              of agentDiffServClassRuleMatchSrcIpAddr that is compared
 *              against a 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchSrcIpMask (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchSrcIpMaskValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MatchSrcIpMask */
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) & objMatchSrcIpMaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMatchSrcIpMaskValue, owa.len);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = L7_SUCCESS;
  owa.rc = XLIBRC_SUCCESS; 

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchSrcL4PortStart
 *
 * @purpose Get 'MatchSrcL4PortStart'
 *
 * @description [MatchSrcL4PortStart]: Source layer 4 port range start match
 *              value for the class - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to srcl4port(12). 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchSrcL4PortStart (void *wap,
    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchSrcL4PortStartValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchSrcL4PortStartGet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        &objMatchSrcL4PortStartValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchSrcL4PortStart */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMatchSrcL4PortStartValue,
      sizeof (objMatchSrcL4PortStartValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchSrcL4PortStart
 *
 * @purpose Set 'MatchSrcL4PortStart'
 *
 * @description [MatchSrcL4PortStart]: Source layer 4 port range start match
 *              value for the class - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to srcl4port(12). 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchSrcL4PortStart (void *wap,
    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchSrcL4PortStartValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  xLibU32_t objMatchSrcL4PortEndValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MatchSrcL4PortStart */
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) & objMatchSrcL4PortStartValue,
      &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMatchSrcL4PortStartValue, owa.len);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  owa.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServClassRule_MatchSrcL4PortEnd,
        (xLibU8_t *) & objMatchSrcL4PortEndValue, &owa.len);
  if ( objMatchSrcL4PortEndValue < objMatchSrcL4PortStartValue)
  {
    owa.rc = XLIBRC_DIFFSERV_ENDPORT_NOT_VALID;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */

  owa.l7rc = L7_SUCCESS;
  owa.rc = XLIBRC_SUCCESS; 

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchSrcL4PortEnd
 *
 * @purpose Get 'MatchSrcL4PortEnd'
 *
 * @description [MatchSrcL4PortEnd]: Source layer 4 port range end match value
 *              for the class - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to srcl4port(12). 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchSrcL4PortEnd (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchSrcL4PortEndValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchSrcL4PortEndGet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        &objMatchSrcL4PortEndValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchSrcL4PortEnd */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMatchSrcL4PortEndValue,
      sizeof (objMatchSrcL4PortEndValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchSrcL4PortEnd
 *
 * @purpose Set 'MatchSrcL4PortEnd'
 *
 * @description [MatchSrcL4PortEnd]: Source layer 4 port range end match value
 *              for the class - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to srcl4port(12). 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchSrcL4PortEnd (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchSrcL4PortEndValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t kwaTempStartVal = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objMatchDstL4PortStartValue;
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MatchSrcL4PortEnd */
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) & objMatchSrcL4PortEndValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMatchSrcL4PortEndValue, owa.len);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  kwaTempStartVal.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServClassRule_MatchSrcL4PortStart,
        (xLibU8_t *) & objMatchDstL4PortStartValue, &kwaTempStartVal.len);
  if ( objMatchSrcL4PortEndValue < objMatchDstL4PortStartValue)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    kwaTempStartVal.rc = XLIBRC_DIFFSERV_ENDPORT_NOT_VALID;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwaTempStartVal);
    return kwaTempStartVal.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbDiffServClassRuleMatchEntryTypeGet(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue, 
      &entryType);
  if(owa.l7rc != L7_SUCCESS || entryType != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCL4PORT)
  {
      usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
      owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    
  }

  if (usmDbFeaturePresentCheck(L7_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_EXCLUDE_FEATURE_ID) != L7_TRUE)
  {
  owa.l7rc = usmDbDiffServClassRuleMatchExcludeFlagSet(L7_UNIT_CURRENT, keyClassIndexValue,
      keyClassRuleIndexValue, L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
    }
  }

  owa.l7rc =
    usmDbDiffServClassRuleMatchSrcL4PortStartSet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        objMatchDstL4PortStartValue);
  if (owa.l7rc == L7_SUCCESS)
  {

    owa.l7rc =
      usmDbDiffServClassRuleMatchSrcL4PortEndSet (L7_UNIT_CURRENT,
          keyClassIndexValue,
          keyClassRuleIndexValue,
          objMatchSrcL4PortEndValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchSrcMacAddr
 *
 * @purpose Get 'MatchSrcMacAddr'
 *
 * @description [MatchSrcMacAddr]: Source MAC address match value for the class
 *              - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to srcmac(13). 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchSrcMacAddr (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t objMatchSrcMacAddrValue;
  xLibU32_t stringLengthValue = sizeof(objMatchSrcMacAddrValue);
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchSrcMacAddrGet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        objMatchSrcMacAddrValue,
        &stringLengthValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchSrcMacAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objMatchSrcMacAddrValue,
      sizeof (objMatchSrcMacAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchSrcMacAddr
 *
 * @purpose Set 'MatchSrcMacAddr'
 *
 * @description [MatchSrcMacAddr]: Source MAC address match value for the class
 *              - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to srcmac(13). 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchSrcMacAddr (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t objMatchSrcMacAddrValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;

  fpObjWa_t kwaTempMacMaskValue = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t objMatchSrcMacMaskValue;
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MatchSrcMacAddr */
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) objMatchSrcMacAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objMatchSrcMacAddrValue, owa.len);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* retrieve key: SrcMacMask */
  kwaTempMacMaskValue.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_MatchSrcMacMask,
      (xLibU8_t *) & objMatchSrcMacMaskValue, &kwaTempMacMaskValue.len);
  if (kwaTempMacMaskValue.rc != XLIBRC_SUCCESS)
  {
    kwaTempMacMaskValue.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaTempMacMaskValue);
    return kwaTempMacMaskValue.rc;
  }
  /* set the value in application */
  owa.l7rc = usmDbDiffServClassRuleMatchEntryTypeGet(L7_UNIT_CURRENT, 
      keyClassIndexValue, keyClassRuleIndexValue, 
      &entryType);
  if(owa.l7rc != L7_SUCCESS || entryType != 
      L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCMAC)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, 
        keyClassRuleIndexValue);
    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (usmDbFeaturePresentCheck(L7_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_EXCLUDE_FEATURE_ID) != L7_TRUE)
  {
  owa.l7rc = usmDbDiffServClassRuleMatchExcludeFlagSet(L7_UNIT_CURRENT, keyClassIndexValue,
      keyClassRuleIndexValue, L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
    }
  }

  owa.l7rc =
    usmDbDiffServClassRuleMatchSrcMacAddrSet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        objMatchSrcMacAddrValue);
  if (owa.l7rc == L7_SUCCESS)
  {

    owa.l7rc =
      usmDbDiffServClassRuleMatchSrcMacMaskSet (L7_UNIT_CURRENT,
          keyClassIndexValue,
          keyClassRuleIndexValue,
          objMatchSrcMacMaskValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchSrcMacMask
 *
 * @purpose Get 'MatchSrcMacMask'
 *
 * @description [MatchSrcMacMask]: Source MAC address mask match value for
 *              the class - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to srcmac(13). This mask value identifies the
 *              portion of agentDiffServClassRuleMatchSrcMacAddr that is
 *              compared agains 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchSrcMacMask (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t objMatchSrcMacMaskValue;
  xLibU32_t stringLengthValue = sizeof(objMatchSrcMacMaskValue);
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchSrcMacMaskGet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        objMatchSrcMacMaskValue,
        &stringLengthValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchSrcMacMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objMatchSrcMacMaskValue,
      strlen (objMatchSrcMacMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchSrcMacMask
 *
 * @purpose Set 'MatchSrcMacMask'
 *
 * @description [MatchSrcMacMask]: Source MAC address mask match value for
 *              the class - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to srcmac(13). This mask value identifies the
 *              portion of agentDiffServClassRuleMatchSrcMacAddr that is
 *              compared agains 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchSrcMacMask (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t objMatchSrcMacMaskValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MatchSrcMacMask */
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) objMatchSrcMacMaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objMatchSrcMacMaskValue, owa.len);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = L7_SUCCESS;
  owa.rc = XLIBRC_SUCCESS;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchVlanId
 *
 * @purpose Get 'MatchVlanId'
 *
 * @description [MatchVlanId]: VLAN ID match value for the class - only valid
 *              if the agentDiffServClassRuleMatchEntryType is set to vlan(14).
 *              This object is obsoleted by agentDiffServClassRuleMatchVlanIdStart
 *              and agentDiffServClassRuleMatchVlanIdEnd. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchVlanId (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchVlanIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  owa.l7rc = L7_FAILURE;

  /* get the value from application */
  /*owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, &objMatchVlanIdValue);*/
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMatchVlanIdValue, sizeof (objMatchVlanIdValue));

  /* return the object value: MatchVlanId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMatchVlanIdValue,
      sizeof (objMatchVlanIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchVlanId
 *
 * @purpose Set 'MatchVlanId'
 *
 * @description [MatchVlanId]: VLAN ID match value for the class - only valid
 *              if the agentDiffServClassRuleMatchEntryType is set to vlan(14).
 *              This object is obsoleted by agentDiffServClassRuleMatchVlanIdStart
 *              and agentDiffServClassRuleMatchVlanIdEnd. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchVlanId (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  /*xLibU32_t objMatchVlanIdValue; */
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MatchVlanId */
  /*  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMatchVlanIdValue, &owa.len);
      if (owa.rc != XLIBRC_SUCCESS)
      {
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
      }
      FPOBJ_TRACE_VALUE (bufp, &objMatchVlanIdValue, owa.len); */

  /* set the value in application */
  owa.l7rc = L7_FAILURE;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchExcludeFlag
 *
 * @purpose Get 'MatchExcludeFlag'
 *
 * @description [MatchExcludeFlag]: Class Rule exclude flag - when set to true(1),
 *              the match condition identified by agentDiffServClassRuleMatchEntryType
 *              is negated. This means that for a given
 *              field, every value is considered to be a match EXCEPT 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchExcludeFlag (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchExcludeFlagValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {   
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchExcludeFlagGet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        &objMatchExcludeFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchExcludeFlag */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMatchExcludeFlagValue,
      sizeof (objMatchExcludeFlagValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchExcludeFlag
 *
 * @purpose Set 'MatchExcludeFlag'
 *
 * @description [MatchExcludeFlag]: Class Rule exclude flag - when set to true(1),
 *              the match condition identified by agentDiffServClassRuleMatchEntryType
 *              is negated. This means that for a given
 *              field, every value is considered to be a match EXCEPT 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchExcludeFlag (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchExcludeFlagValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* retrieve object: MatchExcludeFlag */
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) & objMatchExcludeFlagValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);

    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMatchExcludeFlagValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchExcludeFlagSet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        objMatchExcludeFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {   
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);

    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_StorageType
 *
 * @purpose Get 'StorageType'
 *
 * @description [StorageType]: Storage-type for this conceptual row 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_StorageType (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStorageTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleStorageTypeGet (L7_UNIT_CURRENT, keyClassIndexValue,
        keyClassRuleIndexValue,
        &objStorageTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: StorageType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStorageTypeValue,
      sizeof (objStorageTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_StorageType
 *
 * @purpose Set 'StorageType'
 *
 * @description [StorageType]: Storage-type for this conceptual row 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_StorageType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStorageTypeValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* retrieve object: StorageType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStorageTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);

    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStorageTypeValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServClassRuleStorageTypeSet (L7_UNIT_CURRENT, keyClassIndexValue,
        keyClassRuleIndexValue,
        objStorageTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);

    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_RowStatus
 *
 * @purpose Get 'RowStatus'
 *
 * @description [RowStatus]: The status of this conceptual row. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_RowStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleRowStatusGet (L7_UNIT_CURRENT, keyClassIndexValue,
        keyClassRuleIndexValue,
        &objRowStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRowStatusValue,
      sizeof (objRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_RowStatus
 *
 * @purpose Set 'RowStatus'
 *
 * @description [RowStatus]: The status of this conceptual row. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_RowStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;


  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RowStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRowStatusValue, owa.len);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    keyClassRuleIndexValue = 0;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* call the usmdb only for add and delete */
  if (objRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    owa.l7rc  = usmDbDiffServClassRuleIndexNext(L7_UNIT_CURRENT, keyClassIndexValue, &keyClassRuleIndexValue);
    if (owa.l7rc != L7_SUCCESS) 
    {
      if ((owa.l7rc == L7_ERROR) || (keyClassRuleIndexValue == 0))
      {
        /*Error: Class Rule table is full*/
        owa.rc = XLIBRC_CLASS_RULE_TABLE_FULL; 
      }
      else  
      {
        owa.rc = XLIBRC_FAILURE;  
      }
     
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

    owa.l7rc = usmDbDiffServClassRuleCreate(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue, L7_TRUE);

    if(owa.l7rc != L7_SUCCESS)
    {

      owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;  
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    xLibFilterSet(wap,XOBJ_qosDiffServClassRule_ClassRuleIndex,0,(xLibU8_t 
          *) &keyClassRuleIndexValue,sizeof(keyClassRuleIndexValue));

    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (objRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Delete the existing row */
    owa.l7rc =
      usmDbDiffServClassRuleRowStatusSet (L7_UNIT_CURRENT, keyClassIndexValue,
          keyClassRuleIndexValue,
          objRowStatusValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  return XLIBRC_SUCCESS;
}


/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchCos2
 *
 * @purpose Get 'MatchCos2'
 *
 * @description [MatchCos2]: Three-bit user priority field in the second/inner
 *              802.1Q tag header of a double VLAN tagged Ethernet frame
 *              used as a class-match parameter - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to cos2(15). 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchCos2 (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchCos2Value;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchCos2Get (L7_UNIT_CURRENT, keyClassIndexValue,
        keyClassRuleIndexValue,
        &objMatchCos2Value);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchCos2 */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMatchCos2Value,
      sizeof (objMatchCos2Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchCos2
 *
 * @purpose Set 'MatchCos2'
 *
 * @description [MatchCos2]: Three-bit user priority field in the second/inner
 *              802.1Q tag header of a double VLAN tagged Ethernet frame
 *              used as a class-match parameter - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to cos2(15). 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchCos2 (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchCos2Value;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MatchCos2 */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMatchCos2Value, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMatchCos2Value, owa.len);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  owa.l7rc = usmDbDiffServClassRuleMatchEntryTypeGet(L7_UNIT_CURRENT, 
      keyClassIndexValue, keyClassRuleIndexValue, 
      &entryType);
  if(owa.l7rc != L7_SUCCESS || entryType != 
      L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS2)
  {
    if (owa.l7rc != L7_SUCCESS)
    {
      usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, 
          keyClassRuleIndexValue);
      owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }

  if (usmDbFeaturePresentCheck(L7_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_EXCLUDE_FEATURE_ID) != L7_TRUE)
  {
  owa.l7rc = usmDbDiffServClassRuleMatchExcludeFlagSet(L7_UNIT_CURRENT, keyClassIndexValue,
      keyClassRuleIndexValue, L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
    }
  }

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchCos2Set (L7_UNIT_CURRENT, keyClassIndexValue,
        keyClassRuleIndexValue,
        objMatchCos2Value);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchEtypeKey
 *
 * @purpose Get 'MatchEtypeKey'
 *
 * @description [MatchEtypeKey]: Ethertype keyword match value for the class
 *              - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to etype(16). A keyword of custom(1) requires that the
 *              agentDiffServClassRuleMatchEtypeValue object also be set.
 *              
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchEtypeKey (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchEtypeKeyValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchEtypeKeyGet (L7_UNIT_CURRENT, keyClassIndexValue,
        keyClassRuleIndexValue,
        &objMatchEtypeKeyValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchEtypeKey */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMatchEtypeKeyValue,
      sizeof (objMatchEtypeKeyValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchEtypeKey
 *
 * @purpose Set 'MatchEtypeKey'
 *
 * @description [MatchEtypeKey]: Ethertype keyword match value for the class
 *              - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to etype(16). A keyword of custom(1) requires that the
 *              agentDiffServClassRuleMatchEtypeValue object also be set.
 *              
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchEtypeKey (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchEtypeKeyValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MatchEtypeKey */
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) & objMatchEtypeKeyValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMatchEtypeKeyValue, owa.len);

  /* When MatchEtypeKey is custom then just return success as we are handling this in
      qosDiffServClassRule_MatchEtypeValue set handler */ 
  if (L7_QOS_ETYPE_KEYID_CUSTOM == objMatchEtypeKeyValue)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);
  owa.l7rc = usmDbDiffServClassRuleMatchEntryTypeGet(L7_UNIT_CURRENT, 
      keyClassIndexValue,keyClassRuleIndexValue,&entryType);

  if(owa.l7rc != L7_SUCCESS || entryType != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_ETYPE)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (usmDbFeaturePresentCheck(L7_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_EXCLUDE_FEATURE_ID) != L7_TRUE)
  {
  owa.l7rc = usmDbDiffServClassRuleMatchExcludeFlagSet(L7_UNIT_CURRENT, keyClassIndexValue,
      keyClassRuleIndexValue, L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
    }
  }

  owa.l7rc = usmDbDiffServClassRuleMatchEtypeKeySet(L7_UNIT_CURRENT,keyClassIndexValue,keyClassRuleIndexValue
      ,objMatchEtypeKeyValue);
  if(owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  /* set the value in application */

  owa.rc = XLIBRC_SUCCESS;

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchEtypeValue
 *
 * @purpose Get 'MatchEtypeValue'
 *
 * @description [MatchEtypeValue]: Ethertype match value for the class - only
 *              valid if the agentDiffServClassRuleMatchEntryType is set
 *              to etype(16) and the agentDiffServClassRuleMatchEtypeKey is
 *              set to custom(1). The allowed value for this object is 0x0600
 *              to 0 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchEtypeValue (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchEtypeValueValue;
  xLibU32_t objMatchEtypeKeyValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  owa.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServClassRule_MatchEtypeKey,
        (xLibU8_t *) & objMatchEtypeKeyValue, &owa.len);
  if (objMatchEtypeKeyValue != L7_QOS_ETYPE_KEYID_CUSTOM)
  {
    owa.rc = XLIBRC_DIFFSERV_KEYID_NOTVALID;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchEtypeValueGet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        &objMatchEtypeValueValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchEtypeValue */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMatchEtypeValueValue,
      sizeof (objMatchEtypeValueValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchEtypeValue
 *
 * @purpose Set 'MatchEtypeValue'
 *
 * @description [MatchEtypeValue]: Ethertype match value for the class - only
 *              valid if the agentDiffServClassRuleMatchEntryType is set
 *              to etype(16) and the agentDiffServClassRuleMatchEtypeKey is
 *              set to custom(1). The allowed value for this object is 0x0600
 *              to 0 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchEtypeValue (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibU32_t objMatchEtypeValueValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  xLibStr256_t etherTypeValue;
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType;

  fpObjWa_t kwaEtypeKey = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchEtypeKeyValue;

  FPOBJ_TRACE_ENTER (bufp);
  memset(etherTypeValue,0x0,sizeof(etherTypeValue));

  /* retrieve object: MatchEtypeValue */
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) & etherTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  (void)usmDbConvertXstrtoi(etherTypeValue, &objMatchEtypeValueValue);
  FPOBJ_TRACE_VALUE (bufp, &objMatchEtypeValueValue, owa.len);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve row status object value */
  kwaEtypeKey.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServClassRule_MatchEtypeKey,
        (xLibU8_t *) & objMatchEtypeKeyValue, &kwaEtypeKey.len);
  if (objMatchEtypeKeyValue != L7_QOS_ETYPE_KEYID_CUSTOM)
  {
    kwaEtypeKey.rc = XLIBRC_DIFFSERV_KEYID_NOTVALID;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwaEtypeKey);
    return kwaEtypeKey.rc;
  }

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  owa.l7rc = usmDbDiffServClassRuleMatchEntryTypeGet(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue, &entryType);
  if(owa.l7rc != L7_SUCCESS || entryType != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_ETYPE)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (usmDbFeaturePresentCheck(L7_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_EXCLUDE_FEATURE_ID) != L7_TRUE)
  {
    owa.l7rc = usmDbDiffServClassRuleMatchExcludeFlagSet(L7_UNIT_CURRENT, keyClassIndexValue,
        keyClassRuleIndexValue, L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);
    if (owa.l7rc != L7_SUCCESS)
    {
      usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);

      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchEtypeKeySet (L7_UNIT_CURRENT, keyClassIndexValue,
        keyClassRuleIndexValue,
        objMatchEtypeKeyValue);
  if(owa.l7rc == L7_SUCCESS)
  {
  owa.l7rc  = usmDbDiffServClassRuleMatchEtypeValueSet(L7_UNIT_CURRENT, keyClassIndexValue,
        keyClassRuleIndexValue,
        objMatchEtypeValueValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/******************************************************************************
 *
 * @function fpObjGet_qosDiffServClassRule_MatchEntryTypeValue
 *
 * @purpose Get 'MatchEntryTypeValue'
 *@description  [MatchEntryTypeValue] Class Rule match entry Value
 * @notes
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchEntryTypeValue (void *wap, void *
    bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objMatchEntryTypeValueValue;
  xLibU32_t objMatchEntryTypeValueValue2;

  xLibU32_t keyClassIndexValue;
  xLibU32_t keyClassRuleIndexValue;
  xLibU32_t entryType,val,val1,val2;
  xLibStr256_t strValue,strValue2,tempBuf;
  xLibU8_t macAddr[L7_MAC_ADDR_LEN];
  xLibU8_t macMask[L7_MAC_ADDR_LEN];
  xLibU8_t className[L7_USMDB_MIB_DIFFSERV_CLASS_NAME_MAX+1];
  xLibU32_t macLen = sizeof(macAddr);
  xLibU32_t classNameLen = sizeof(className);
  L7_QOS_ETYPE_KEYID_t etype;
  xLibU32_t  flowLabel;
  L7_in6_addr_t objIPv6Address; 
  xLibU8_t tosBits,tosMask;
 
  FPOBJ_TRACE_ENTER (bufp);
  memset(strValue,0x0,sizeof(strValue));
  memset(strValue2,0x0,sizeof(strValue2));
  memset(tempBuf,0x0,sizeof(tempBuf));

  /* retrieve key: ClassIndex */
  owa.len = sizeof (keyClassIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, owa.len);

  /* retrieve key: ClassRuleIndex */
  owa.len = sizeof (keyClassRuleIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, owa.len);
  
  /* get the value from application */

  owa.l7rc = 
    usmDbDiffServClassRuleMatchEntryTypeGet(L7_UNIT_CURRENT,keyClassIndexValue,keyClassRuleIndexValue,&entryType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.rc = XLIBRC_SUCCESS; 
  switch(entryType)
  {
    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS:
      owa.l7rc = usmDbDiffServClassRuleMatchCosGet(L7_UNIT_CURRENT,keyClassIndexValue,keyClassRuleIndexValue,&objMatchEntryTypeValueValue);	
      if(owa.l7rc != L7_SUCCESS)
      {
        break;
      }	

      memset(strValue,0x0,sizeof(strValue));
      sprintf(strValue," %d ",objMatchEntryTypeValueValue);

      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS2:
      owa.l7rc = usmDbDiffServClassRuleMatchCos2Get(L7_UNIT_CURRENT,keyClassIndexValue,keyClassRuleIndexValue,&objMatchEntryTypeValueValue);	
      if(owa.l7rc != L7_SUCCESS)
      {
        break;
      }	
      memset(strValue,0x0,sizeof(strValue));
      sprintf(strValue," %d ",objMatchEntryTypeValueValue);

      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIP:
      if ((usmDbDiffServClassRuleMatchDstIpAddrGet(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue,
              &objMatchEntryTypeValueValue) == L7_SUCCESS) &&
          (usmDbDiffServClassRuleMatchDstIpMaskGet(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue,
                                                   &objMatchEntryTypeValueValue2) == L7_SUCCESS))
      {
        memset(strValue, 0x0,sizeof(strValue));
        memset(strValue2, 0x0,sizeof(strValue2));
        memset(tempBuf, 0x0,sizeof(tempBuf));
        if ((usmDbInetNtoa(objMatchEntryTypeValueValue, strValue) == L7_SUCCESS) &&
            (usmDbInetNtoa(objMatchEntryTypeValueValue2,strValue2 ) == L7_SUCCESS))
        {
          sprintf(tempBuf, "%s (%s)", strValue, strValue2);
          memset(strValue, 0x0,sizeof(strValue));
          memcpy(strValue,tempBuf,strlen(tempBuf));

        }
      }
      else
      {
        owa.l7rc = L7_FAILURE;
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIPV6:
  
      if ((usmDbDiffServClassRuleMatchDstIpv6AddrGet(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue,
              &objIPv6Address) == L7_SUCCESS) &&
          (usmDbDiffServClassRuleMatchDstIpv6PrefLenGet(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue,
                                                   &objMatchEntryTypeValueValue2) == L7_SUCCESS))
      {
        memset(strValue, 0x0,sizeof(strValue));
        memset(strValue2, 0x0,sizeof(strValue2));
        memset(tempBuf, 0x0,sizeof(tempBuf));
        osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)&objIPv6Address, strValue, sizeof(strValue));
        sprintf(tempBuf, "%s/%d", strValue, objMatchEntryTypeValueValue2);
        memset(strValue, 0x0,sizeof(strValue));
        memcpy(strValue,tempBuf,strlen(tempBuf));
      }
      else
      {
        owa.l7rc = L7_FAILURE;
      }
      break;


    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIPV6:
  
      if ((usmDbDiffServClassRuleMatchSrcIpv6AddrGet(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue,
              &objIPv6Address) == L7_SUCCESS) &&
          (usmDbDiffServClassRuleMatchSrcIpv6PrefLenGet(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue,
                                                   &objMatchEntryTypeValueValue2) == L7_SUCCESS))
      {
        memset(strValue, 0x0,sizeof(strValue));
        memset(strValue2, 0x0,sizeof(strValue2));
        memset(tempBuf, 0x0,sizeof(tempBuf));
        osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)&objIPv6Address, strValue, sizeof(strValue));
        sprintf(tempBuf, "%s/%d", strValue, objMatchEntryTypeValueValue2);
        memset(strValue, 0x0,sizeof(strValue));
        memcpy(strValue,tempBuf,strlen(tempBuf));
      }
      else
      {
        owa.l7rc = L7_FAILURE;
      }
      break;


    


 
    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTL4PORT:
      owa.l7rc = usmDbDiffServClassRuleMatchDstL4PortStartGet(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue,
          &val1);
      if(owa.l7rc != L7_SUCCESS)
      {

        break;
      }
      owa.l7rc = usmDbDiffServClassRuleMatchDstL4PortEndGet(L7_UNIT_CURRENT, keyClassIndexValue, 
          keyClassRuleIndexValue,&val2);
      if(owa.l7rc != L7_SUCCESS)
      {

        break;
      }
      memset(tempBuf,0x0, sizeof(tempBuf));
      memset(strValue, 0x0,sizeof(strValue));
      memset(strValue2,0x0, sizeof(strValue2));

      diffservConvertL4PortValToString(val1, strValue, sizeof(strValue));
      if(val1 == val2)
      {
          osapiSnprintf(tempBuf, sizeof(tempBuf), strValue);
      }
      else
      {
          diffservConvertL4PortValToString(val2, strValue2, sizeof(strValue2));
          osapiSnprintf(tempBuf, sizeof(tempBuf),  "%s - %s", strValue, strValue2);
          sprintf(strValue, "%s", tempBuf);
      }

      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTMAC:
      owa.l7rc = usmDbDiffServClassRuleMatchDstMacAddrGet(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue,
          (L7_uchar8 *) macAddr, &
          macLen); 
      if(owa.l7rc != L7_SUCCESS)
      {
        break;
      }
      owa.l7rc =  usmDbDiffServClassRuleMatchDstMacMaskGet(L7_UNIT_CURRENT, keyClassIndexValue, 
          keyClassRuleIndexValue,
          (L7_uchar8 *) macMask, &
          macLen);
      if(owa.l7rc != L7_SUCCESS)
      {
        break;
      }
      memset(strValue, 0x0,sizeof(strValue));
      sprintf(strValue, "%02X:%02X:%02X:%02X:%02X:%02X (%02X:%02X:%02X:%02X:%02X:%02X)",
          macAddr[0], macAddr[1], macAddr[2], macAddr[3], 
          macAddr[4], macAddr[5],
          macMask[0], macMask[1], macMask[2], macMask[3], 
          macMask[4], macMask[5]);


      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_ETYPE:
      if (usmDbDiffServClassRuleMatchEtypeKeyGet(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue,
            &etype) == L7_SUCCESS)
      {
        
        memset(strValue,0x0,sizeof(strValue));
        sprintf(strValue," %d ",etype);
        if (etype == L7_QOS_ETYPE_KEYID_CUSTOM)
        {
          if (usmDbDiffServClassRuleMatchEtypeValueGet(L7_UNIT_CURRENT, keyClassIndexValue,keyClassRuleIndexValue,
                                                       &val) == L7_SUCCESS)
          {
            osapiSnprintf(strValue, sizeof(strValue),  "0x%x", (L7_ushort16)val);
          }
        }
        else
        {
          diffservConvertEtherTypeKeyIdToString(etype, strValue, sizeof(strValue));
        }
      }
      else
      {
        owa.l7rc = L7_FAILURE;
      }
      break;



    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_FLOWLBLV6:
      if (usmDbDiffServClassRuleMatchIp6FlowLabelGet(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue,
            &flowLabel) == L7_SUCCESS)
      {
        memset(strValue,0x0,sizeof(strValue));
        sprintf(strValue," %d ",flowLabel);
      }
      else
      {
        owa.l7rc = L7_FAILURE;
      }
      break;


    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_EVERY:
      memset(strValue,0x0,sizeof(strValue));
      memcpy(strValue,"Any",sizeof("Any"));
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP:
      owa.l7rc = usmDbDiffServClassRuleMatchIpDscpGet(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue,
          &val);
      if(owa.l7rc != L7_SUCCESS)
      {
        break;
      }
      memset(strValue,0x0,sizeof(strValue));
      convertDSCPValToString(val,strValue);
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE:
      owa.l7rc = usmDbDiffServClassRuleMatchIpPrecedenceGet(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue,
          &val);
      if(owa.l7rc != L7_SUCCESS)
      {
        break;
      }
      memset(strValue,0x0,sizeof(strValue));
      sprintf(strValue," %d ",val);

      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS:
      val = val2 = 0;     
      owa.l7rc = usmDbDiffServClassRuleMatchIpTosBitsGet(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue,
          &tosBits);
      if(owa.l7rc != L7_SUCCESS)
      {
        break;
      }
      owa.l7rc = usmDbDiffServClassRuleMatchIpTosMaskGet(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue,
          &tosMask);
      if(owa.l7rc != L7_SUCCESS)
      {
        break;
      }
      osapiSnprintf(strValue,sizeof(strValue),"%x/%x",tosBits,tosMask);
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_PROTOCOL:

      owa.l7rc = usmDbDiffServClassRuleMatchProtocolNumGet(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue,
          &val);
      if(owa.l7rc != L7_SUCCESS)
      {
        break;
      }
      memset(strValue,0x0,sizeof(strValue));
      switch (val)
      {
        case L7_USMDB_MIB_DIFFSERV_LAYER_4_PROTOCOL_ICMP:
          osapiSnprintf(strValue, sizeof(strValue),  "%u(%s)", val, "icmp");
          break;
        case L7_USMDB_MIB_DIFFSERV_LAYER_4_PROTOCOL_IGMP:
          osapiSnprintf(strValue, sizeof(strValue),  "%u(%s)", val, "igmp");
          break;
        case L7_USMDB_MIB_DIFFSERV_LAYER_4_PROTOCOL_IP:
          osapiSnprintf(strValue, sizeof(strValue),  "%u(%s)", val, "ip");
          break;
        case L7_USMDB_MIB_DIFFSERV_LAYER_4_PROTOCOL_TCP:
          osapiSnprintf(strValue, sizeof(strValue),  "%u(%s)", val, "tcp");
          break;
        case L7_USMDB_MIB_DIFFSERV_LAYER_4_PROTOCOL_UDP:
          osapiSnprintf(strValue, sizeof(strValue),  "%u(%s)", val, "udp");
          break;
        default:
          osapiSnprintf(strValue, sizeof(strValue),  "%u", val);
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS:
      memset(className,0x0,sizeof(className));
      memset(strValue,0x0,sizeof(strValue));
      classNameLen = 0;
      owa.l7rc = usmDbDiffServClassRuleMatchRefClassIndexGet(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue,
          &val);
      if(owa.l7rc != L7_SUCCESS)
      {
        break;
      }
      classNameLen = sizeof(className);
      if (usmDbDiffServClassNameGet(L7_UNIT_CURRENT, val,
            className, &classNameLen) == 
          L7_SUCCESS)
      {
        sprintf(strValue, "%s", className);
      }
      else
      {
        sprintf(strValue, "%s", "-- Removed --");
      }

      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIP:
      owa.l7rc = usmDbDiffServClassRuleMatchSrcIpAddrGet(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue,
          &val);
      if(owa.l7rc != L7_SUCCESS)
      {
        break;
      }
      owa.l7rc = usmDbDiffServClassRuleMatchSrcIpMaskGet(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue,
          &val2);
      if(owa.l7rc != L7_SUCCESS)
      {
        break;
      }
      memset(strValue, 0x0,sizeof(strValue));
      memset(strValue2, 0x0,sizeof(strValue2));
      memset(tempBuf,0x0, sizeof(tempBuf));
      if ((usmDbInetNtoa(val, strValue) == L7_SUCCESS) &&
          (usmDbInetNtoa(val2, strValue2) == L7_SUCCESS))
      {
        sprintf(tempBuf, "%s (%s)", strValue, strValue2);
        memset(strValue,0x0,sizeof(strValue));
        sprintf(strValue, "%s", tempBuf);
      }

      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCL4PORT:
      owa.l7rc = usmDbDiffServClassRuleMatchSrcL4PortStartGet(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue,
          &val1);
      if(owa.l7rc != L7_SUCCESS)
      {
        break;
      }
      owa.l7rc = usmDbDiffServClassRuleMatchSrcL4PortEndGet(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue,
          &val2);
      if(owa.l7rc != L7_SUCCESS)
      {
        break;
      }

      memset (tempBuf, 0x0, sizeof(tempBuf));
      memset(strValue,0x0,sizeof(strValue));
      memset(strValue2,0x0, sizeof(strValue2));

      diffservConvertL4PortValToString(val1, strValue, sizeof(strValue));
      if(val1 == val2)
      {
          osapiSnprintf(tempBuf, sizeof(tempBuf), strValue);
      }
      else
      {
          diffservConvertL4PortValToString(val2, strValue2, sizeof(strValue2));
          osapiSnprintf(tempBuf, sizeof(tempBuf),  "%s - %s", strValue, strValue2);
          sprintf(strValue, "%s", tempBuf);
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCMAC:
      memset(strValue, 0x0,sizeof(strValue));
      memset(strValue2, 0x0,sizeof(strValue2));
      memset(macAddr, 0x0,sizeof(macAddr));
      memset(macMask, 0x0,sizeof(macMask));
      owa.l7rc = usmDbDiffServClassRuleMatchSrcMacAddrGet(L7_UNIT_CURRENT, keyClassIndexValue, 
          keyClassRuleIndexValue,
          (L7_uchar8 *) macAddr, &
          macLen);
      if(owa.l7rc != L7_SUCCESS)
      {
        break;
      }
      owa.l7rc = usmDbDiffServClassRuleMatchSrcMacMaskGet(L7_UNIT_CURRENT, keyClassIndexValue, 
          keyClassRuleIndexValue,
          (L7_uchar8 *) macMask, &
          macLen); 
      if(owa.l7rc != L7_SUCCESS)
      {
        break;
      }
      memset(tempBuf,0x0, sizeof(tempBuf));
      sprintf(tempBuf, "%02X:%02X:%02X:%02X:%02X:%02X (%02X:%02X:%02X:%02X:%02X:%02X)",
          macAddr[0], macAddr[1], macAddr[2], macAddr[3], 
          macAddr[4], macAddr[5],
          macMask[0], macMask[1], macMask[2], macMask[3], 
          macMask[4], macMask[5]);
      sprintf(strValue, "%s", tempBuf);



      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID:
      memset(strValue,0x0,sizeof(strValue));
      owa.l7rc = usmDbDiffServClassRuleMatchVlanIdStartGet(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue,
          &val1);
      if(owa.l7rc != L7_SUCCESS)
      {
        break;
      }
      owa.l7rc = usmDbDiffServClassRuleMatchVlanIdEndGet(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue,
          &val2);
      if(owa.l7rc != L7_SUCCESS)
      {
        break;
      }
      if(val1 == val2)
      {
        sprintf(strValue," %d ",val1);
      }
      else
      {
        sprintf(strValue," %d - %d ",val1,val2);
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID2:
      memset(strValue,0x0,sizeof(strValue));
      owa.l7rc = usmDbDiffServClassRuleMatchVlanId2StartGet(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue,
          &val1);
      if(owa.l7rc != L7_SUCCESS)
      {
        break;
      }
      owa.l7rc = usmDbDiffServClassRuleMatchVlanId2EndGet(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue,
          &val2);
      if(owa.l7rc != L7_SUCCESS)
      {
        break;
      }
      if(val1 == val2)
      {
        sprintf(strValue," %d ",val1);
      }
      else
      {
        sprintf(strValue," %d - %d ",val1,val2);
      }
      break;


  }
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, strValue, strlen (
        strValue));

  /* return the object value: MatchEntryTypeValue */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) strValue,
      strlen (strValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchVlanIdStart
 *
 * @purpose Get 'MatchVlanIdStart'
 *
 * @description [MatchVlanIdStart]: VLAN ID range start match value for the
 *              class - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to vlanid(17). The VLAN ID field is defined as
 *              the 12-bit VLAN identifier in the 802.1Q tag head 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchVlanIdStart (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchVlanIdStartValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchVlanIdStartGet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        &objMatchVlanIdStartValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchVlanIdStart */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMatchVlanIdStartValue,
      sizeof (objMatchVlanIdStartValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchVlanIdStart
 *
 * @purpose Set 'MatchVlanIdStart'
 *
 * @description [MatchVlanIdStart]: VLAN ID range start match value for the
 *              class - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to vlanid(17). The VLAN ID field is defined as
 *              the 12-bit VLAN identifier in the 802.1Q tag head 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchVlanIdStart (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchVlanIdStartValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  xLibU32_t objMatchVlanIdEndValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MatchVlanIdStart */
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) & objMatchVlanIdStartValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMatchVlanIdStartValue, owa.len);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* retrieve row status object value */
  owa.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServClassRule_MatchVlanIdEnd,
        (xLibU8_t *) & objMatchVlanIdEndValue, &owa.len);
  if (objMatchVlanIdEndValue < objMatchVlanIdStartValue)
  {
    owa.rc = XLIBRC_DIFFSERV_ENDVLANID_NOT_VALID;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.l7rc = L7_SUCCESS;
  owa.rc = XLIBRC_SUCCESS;

  /* set the value in application */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchVlanIdEnd
 *
 * @purpose Get 'MatchVlanIdEnd'
 *
 * @description [MatchVlanIdEnd]: VLAN ID range end match value for the class
 *              - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to vlanid(17). The VLAN ID field is defined as the
 *              12-bit VLAN identifier in the 802.1Q tag heade 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchVlanIdEnd (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchVlanIdEndValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchVlanIdEndGet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        &objMatchVlanIdEndValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchVlanIdEnd */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMatchVlanIdEndValue,
      sizeof (objMatchVlanIdEndValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchVlanIdEnd
 *
 * @purpose Set 'MatchVlanIdEnd'
 *
 * @description [MatchVlanIdEnd]: VLAN ID range end match value for the class
 *              - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to vlanid(17). The VLAN ID field is defined as the
 *              12-bit VLAN identifier in the 802.1Q tag heade 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchVlanIdEnd (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchVlanIdEndValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t kwaStartVlanID = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchVlanIdStartValue=1;
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MatchVlanIdEnd */
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) & objMatchVlanIdEndValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMatchVlanIdEndValue, owa.len);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* retrieve row status object value */
  kwaStartVlanID.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServClassRule_MatchVlanIdStart,
        (xLibU8_t *) & objMatchVlanIdStartValue, &kwaStartVlanID.len);
  if (objMatchVlanIdEndValue < objMatchVlanIdStartValue)
  {
    kwaStartVlanID.rc = XLIBRC_DIFFSERV_ENDVLANID_NOT_VALID;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwaStartVlanID);
    return kwaStartVlanID.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbDiffServClassRuleMatchEntryTypeGet(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue, &entryType);
  if(owa.l7rc != L7_SUCCESS || entryType != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue,keyClassRuleIndexValue);
    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;  
  }

  if (usmDbFeaturePresentCheck(L7_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_EXCLUDE_FEATURE_ID) != L7_TRUE)
  {
  owa.l7rc = usmDbDiffServClassRuleMatchExcludeFlagSet(L7_UNIT_CURRENT, keyClassIndexValue,
      keyClassRuleIndexValue, L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue,keyClassRuleIndexValue);
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
    }
  }

  owa.l7rc =
    usmDbDiffServClassRuleMatchVlanIdStartSet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        objMatchVlanIdStartValue);
  if (owa.l7rc == L7_SUCCESS)
  {
    owa.l7rc =
      usmDbDiffServClassRuleMatchVlanIdEndSet (L7_UNIT_CURRENT,
          keyClassIndexValue,
          keyClassRuleIndexValue,
          objMatchVlanIdEndValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchVlanId2Start
 *
 * @purpose Get 'MatchVlanId2Start'
 *
 * @description [MatchVlanId2Start]: Secondary VLAN ID range start match value
 *              for the class - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to vlanid2(18). The Secondary VLAN ID
 *              field is defined as the 12-bit VLAN identifier 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchVlanId2Start (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchVlanId2StartValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchVlanId2StartGet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        &objMatchVlanId2StartValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchVlanId2Start */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMatchVlanId2StartValue,
      sizeof (objMatchVlanId2StartValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchVlanId2Start
 *
 * @purpose Set 'MatchVlanId2Start'
 *
 * @description [MatchVlanId2Start]: Secondary VLAN ID range start match value
 *              for the class - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to vlanid2(18). The Secondary VLAN ID
 *              field is defined as the 12-bit VLAN identifier 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchVlanId2Start (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchVlanId2StartValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  xLibU32_t objMatchVlanId2EndValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MatchVlanId2Start */
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) & objMatchVlanId2StartValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMatchVlanId2StartValue, owa.len);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  owa.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServClassRule_MatchVlanId2End,
        (xLibU8_t *) & objMatchVlanId2EndValue, &owa.len);
  if (objMatchVlanId2EndValue < objMatchVlanId2StartValue)
  {
    owa.rc = XLIBRC_DIFFSERV_ENDVLANID_NOT_VALID;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */

  owa.l7rc = L7_SUCCESS;
  owa.rc = XLIBRC_SUCCESS;

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchVlanId2End
 *
 * @purpose Get 'MatchVlanId2End'
 *
 * @description [MatchVlanId2End]: Secondary VLAN ID range end match value
 *              for the class - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to vlanid2(18). The Secondary VLAN ID field
 *              is defined as the 12-bit VLAN identifier in 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchVlanId2End (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchVlanId2EndValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchVlanId2EndGet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        &objMatchVlanId2EndValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchVlanId2End */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMatchVlanId2EndValue,
      sizeof (objMatchVlanId2EndValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchVlanId2End
 *
 * @purpose Set 'MatchVlanId2End'
 *
 * @description [MatchVlanId2End]: Secondary VLAN ID range end match value
 *              for the class - only valid if the agentDiffServClassRuleMatchEntryType
 *              is set to vlanid2(18). The Secondary VLAN ID field
 *              is defined as the 12-bit VLAN identifier in 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchVlanId2End (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchVlanId2EndValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t kwaStartVlanID = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objMatchVlanId2StartValue;
	L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType;
	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MatchVlanId2End */
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) & objMatchVlanId2EndValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMatchVlanId2EndValue, owa.len);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  kwaStartVlanID.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServClassRule_MatchVlanId2Start,
        (xLibU8_t *) & objMatchVlanId2StartValue, &kwaStartVlanID.len);
  if (objMatchVlanId2EndValue < objMatchVlanId2StartValue)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);

    kwaStartVlanID.rc = XLIBRC_DIFFSERV_ENDVLANID_NOT_VALID;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwaStartVlanID);
    return kwaStartVlanID.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbDiffServClassRuleMatchEntryTypeGet(L7_UNIT_CURRENT,keyClassIndexValue, keyClassRuleIndexValue, 
      &entryType);
  if(owa.l7rc != L7_SUCCESS || entryType != 
      L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID2)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, 
        keyClassRuleIndexValue);
    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (usmDbFeaturePresentCheck(L7_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_EXCLUDE_FEATURE_ID) != L7_TRUE)
  {
  owa.l7rc = usmDbDiffServClassRuleMatchExcludeFlagSet(L7_UNIT_CURRENT, keyClassIndexValue,
      keyClassRuleIndexValue, L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
    }
  }

  owa.l7rc =
    usmDbDiffServClassRuleMatchVlanId2StartSet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        objMatchVlanId2StartValue);
  if (owa.l7rc == L7_SUCCESS)
  {
    owa.l7rc =
      usmDbDiffServClassRuleMatchVlanId2EndSet (L7_UNIT_CURRENT,
          keyClassIndexValue,
          keyClassRuleIndexValue,
          objMatchVlanId2EndValue);
  }


  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchFlowLabel
 *
 * @purpose Get 'MatchFlowLabel'
 *
 * @description [MatchFlowLabel]: Flow label is 20-bit number that is unique
 *              to an IPv6 packet, used by end stations to signify quality-of-service
 *              handling in routers. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchFlowLabel (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchFlowLabelValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchIp6FlowLabelGet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        &objMatchFlowLabelValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchFlowLabel */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMatchFlowLabelValue,
      sizeof (objMatchFlowLabelValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchFlowLabel
 *
 * @purpose Set 'MatchFlowLabel'
 *
 * @description [MatchFlowLabel]: Flow label is 20-bit number that is unique
 *              to an IPv6 packet, used by end stations to signify quality-of-service
 *              handling in routers. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchFlowLabel (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchFlowLabelValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t  entryType;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MatchFlowLabel */
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) & objMatchFlowLabelValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMatchFlowLabelValue, owa.len);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* set the value in application */

  owa.l7rc = usmDbDiffServClassRuleMatchEntryTypeGet(L7_UNIT_CURRENT,keyClassIndexValue, keyClassRuleIndexValue, 
      &entryType);
  if(owa.l7rc != L7_SUCCESS || entryType != 
      L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_FLOWLBLV6)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, 
        keyClassRuleIndexValue);
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (usmDbFeaturePresentCheck(L7_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_EXCLUDE_FEATURE_ID) != L7_TRUE)
  {
  owa.l7rc = usmDbDiffServClassRuleMatchExcludeFlagSet(L7_UNIT_CURRENT, keyClassIndexValue,
      keyClassRuleIndexValue, L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue,
        keyClassRuleIndexValue);
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
    }
  }

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchIp6FlowLabelSet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        objMatchFlowLabelValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchDstIpv6Prefix
 *
 * @purpose Get 'MatchDstIpv6Prefix'
 *
 * @description [MatchDstIpv6Prefix]: The Ipv6 Prefix Address configured on
 *              the Service Port. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchDstIpv6Prefix (void *wap,
    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_in6_addr_t));
  L7_in6_addr_t objMatchDstIpv6PrefixValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchDstIpv6AddrGet (L7_UNIT_CURRENT,
    keyClassIndexValue,
    keyClassRuleIndexValue,
    &objMatchDstIpv6PrefixValue); 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchDstIpv6Prefix */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objMatchDstIpv6PrefixValue,
      sizeof (objMatchDstIpv6PrefixValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchDstIpv6Prefix
 *
 * @purpose Set 'MatchDstIpv6Prefix'
 *
 * @description [MatchDstIpv6Prefix]: The Ipv6 Prefix Address configured on
 *              the Service Port. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchDstIpv6Prefix (void *wap,
    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_in6_addr_t));
  L7_in6_addr_t objMatchDstIpv6PrefixValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MatchDstIpv6Prefix */
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) &objMatchDstIpv6PrefixValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMatchDstIpv6PrefixValue, owa.len);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);


  /* set the value in application */

  owa.l7rc = usmDbDiffServClassRuleMatchEntryTypeGet(L7_UNIT_CURRENT,keyClassIndexValue, keyClassRuleIndexValue, 
      &entryType);
  if(owa.l7rc != L7_SUCCESS || entryType != 
      L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIPV6)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, 
        keyClassRuleIndexValue);
    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (usmDbFeaturePresentCheck(L7_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_EXCLUDE_FEATURE_ID) != L7_TRUE)
  {
  owa.l7rc = usmDbDiffServClassRuleMatchExcludeFlagSet(L7_UNIT_CURRENT, keyClassIndexValue,
      keyClassRuleIndexValue, L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, 
        keyClassRuleIndexValue);
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
    }
  }


  /* set the value in application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchDstIpv6AddrSet (L7_UNIT_CURRENT,
    keyClassIndexValue,
    keyClassRuleIndexValue,
    &objMatchDstIpv6PrefixValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, 
        keyClassRuleIndexValue);
    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchSrcIpv6Prefix
 *
 * @purpose Get 'MatchSrcIpv6Prefix'
 *
 * @description [MatchSrcIpv6Prefix]: The Ipv6 Prefix Address configured on
 *              the Service Port. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchSrcIpv6Prefix (void *wap,
    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_in6_addr_t));
  L7_in6_addr_t objMatchSrcIpv6PrefixValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
   owa.l7rc =
     usmDbDiffServClassRuleMatchSrcIpv6AddrGet (L7_UNIT_CURRENT,
     keyClassIndexValue,
     keyClassRuleIndexValue,
     &objMatchSrcIpv6PrefixValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchSrcIpv6Prefix */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objMatchSrcIpv6PrefixValue,
      sizeof (objMatchSrcIpv6PrefixValue));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchSrcIpv6Prefix
 *
 * @purpose Set 'MatchSrcIpv6Prefix'
 *
 * @description [MatchSrcIpv6Prefix]: The Ipv6 Prefix Address configured on
 *              the Service Port. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchSrcIpv6Prefix (void *wap,
    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_in6_addr_t));
  L7_in6_addr_t objMatchSrcIpv6PrefixValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t  entryType;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MatchSrcIpv6Prefix */
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) &objMatchSrcIpv6PrefixValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMatchSrcIpv6PrefixValue, owa.len);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);


  owa.l7rc = usmDbDiffServClassRuleMatchEntryTypeGet(L7_UNIT_CURRENT,keyClassIndexValue, keyClassRuleIndexValue, 
      &entryType);
  if(owa.l7rc != L7_SUCCESS || entryType != 
      L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIPV6)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, 
        keyClassRuleIndexValue);
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (usmDbFeaturePresentCheck(L7_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_EXCLUDE_FEATURE_ID) != L7_TRUE)
  {
  owa.l7rc = usmDbDiffServClassRuleMatchExcludeFlagSet(L7_UNIT_CURRENT, keyClassIndexValue,
      keyClassRuleIndexValue, L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, 
        keyClassRuleIndexValue);
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
    }
  }

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchSrcIpv6AddrSet (L7_UNIT_CURRENT,
    keyClassIndexValue,
    keyClassRuleIndexValue,
    &objMatchSrcIpv6PrefixValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, 
        keyClassRuleIndexValue);
    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchDstIpv6PrefixLength
 *
 * @purpose Get 'MatchDstIpv6PrefixLength'
 *
 * @description [MatchDstIpv6PrefixLength]: The Prefix Length. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchDstIpv6PrefixLength (void *wap,
    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchDstIpv6PrefixLengthValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchDstIpv6PrefLenGet (L7_UNIT_CURRENT,
    keyClassIndexValue,
    keyClassRuleIndexValue,
    &objMatchDstIpv6PrefixLengthValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchDstIpv6PrefixLength */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objMatchDstIpv6PrefixLengthValue,
        sizeof (objMatchDstIpv6PrefixLengthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchDstIpv6PrefixLength
 *
 * @purpose Set 'MatchDstIpv6PrefixLength'
 *
 * @description [MatchDstIpv6PrefixLength]: The Prefix Length. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchDstIpv6PrefixLength (void *wap,
    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchDstIpv6PrefixLengthValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MatchDstIpv6PrefixLength */
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) & objMatchDstIpv6PrefixLengthValue,
      &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMatchDstIpv6PrefixLengthValue, owa.len);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* set the value in application */

  owa.l7rc =
    usmDbDiffServClassRuleMatchDstIpv6PrefLenSet (L7_UNIT_CURRENT,
    keyClassIndexValue,
    keyClassRuleIndexValue,
    objMatchDstIpv6PrefixLengthValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, 
        keyClassRuleIndexValue);
    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchSrcIpv6PrefixLength
 *
 * @purpose Get 'MatchSrcIpv6PrefixLength'
 *
 * @description [MatchSrcIpv6PrefixLength]: The Prefix Length. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchSrcIpv6PrefixLength (void *wap,
    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchSrcIpv6PrefixLengthValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchSrcIpv6PrefLenGet (L7_UNIT_CURRENT,
    keyClassIndexValue,
    keyClassRuleIndexValue,
    &objMatchSrcIpv6PrefixLengthValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MatchSrcIpv6PrefixLength */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objMatchSrcIpv6PrefixLengthValue,
        sizeof (objMatchSrcIpv6PrefixLengthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchSrcIpv6PrefixLength
 *
 * @purpose Set 'MatchSrcIpv6PrefixLength'
 *
 * @description [MatchSrcIpv6PrefixLength]: The Prefix Length. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchSrcIpv6PrefixLength (void *wap,
    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchSrcIpv6PrefixLengthValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MatchSrcIpv6PrefixLength */
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) & objMatchSrcIpv6PrefixLengthValue,
      &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMatchSrcIpv6PrefixLengthValue, owa.len);

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc =
     usmDbDiffServClassRuleMatchSrcIpv6PrefLenSet (L7_UNIT_CURRENT,
     keyClassIndexValue,
     keyClassRuleIndexValue,
     objMatchSrcIpv6PrefixLengthValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, 
        keyClassRuleIndexValue);
    owa.rc = XLIBRC_CLASS_RULE_NOT_CREATED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/******************************************************************************
 *
 * @function fpObjGet_qosDiffServClassRule_Name
 *
 * @purpose Get 'Name'
 *
 * @description [Name]: Name of the DiffServ traffic class 
 *
 * @return

 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_Name (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objNameValue;
  xLibU32_t keyLengthValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);
  keyLengthValue = sizeof(objNameValue);
  /* get the value from application */
  owa.l7rc = usmDbDiffServClassNameGet (L7_UNIT_CURRENT, keyIndexValue,
      objNameValue,&keyLengthValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Name */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objNameValue,
      strlen (objNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/******************************************************************************
 *
 * @function fpObjGet_qosDiffServClassRule_ClassTypeProto
 *
 * @purpose Get 'ClassNameType'
 *
 * @description [Name]: Name of the DiffServ traffic class 
 *
 * @return

 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_ClassTypeProto (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue;
  L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_t   objTypeValue;
  L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_t objProtoType;
  xLibStr256_t protoString;
  xLibStr256_t typeString;
  xLibStr256_t returnString;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDiffServClassL3ProtoGet(L7_UNIT_CURRENT, keyIndexValue,
      &objProtoType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  owa.l7rc = usmDbDiffServClassTypeGet(L7_UNIT_CURRENT, 
      keyIndexValue,&objTypeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  memset(protoString,0x0,sizeof(protoString));
  memset(typeString,0x0,sizeof(typeString));
  memset(returnString,0x0,sizeof(returnString));
  switch(objTypeValue)
  {
    case L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_NONE:
      osapiStrncpy(typeString,"none",sizeof(typeString));
      break;

    case  L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ALL:
      osapiStrncpy(typeString,"All",sizeof(typeString));
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ANY:
      osapiStrncpy(typeString,"Any",sizeof(typeString));
      break;
    case L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL:
      osapiStrncpy(typeString,"Acl",sizeof(typeString));
      break;
    case L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_TOTAL:
      osapiStrncpy(typeString,"Total",sizeof(typeString));				 
      break;		 
    default:
      owa.rc = XLIBRC_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;

  }

  switch(objProtoType)
  {
    case L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_NONE:
      osapiStrncpy(protoString,"None",sizeof(protoString));
      break;
    case L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV4:
      osapiStrncpy(protoString,"IPV4",sizeof(protoString));
      break;
    case L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV6:
      osapiStrncpy(protoString,"IPV6",sizeof(protoString));
      break;		
    default:
      owa.rc = XLIBRC_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;		
  }
  sprintf(returnString," %s(%s) ",typeString,protoString);

  /* return the object value: Name */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) returnString,
      strlen (returnString));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/******************************************************************************
 *
 * @function fpObjSet_qosDiffServClassRule_Name
 *
 * @purpose Set 'Name'
 *
 * @description [Name]: Name of the DiffServ traffic class 
 *
 * @return

 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_Name (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objNameValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue=0;

  fpObjWa_t kwaRowStatus = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRowStatusValue = 0;
	
  xLibU32_t newClassId;
  FPOBJ_TRACE_ENTER (bufp);

  /*retrun success if its a row status is create_and Go */
  kwaRowStatus.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRowStatus,
	(xLibU8_t *) &keyRowStatusValue, &kwaRowStatus.len);
	if (kwaRowStatus.rc == XLIBRC_SUCCESS)
	{
		FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRowStatusValue, kwaRowStatus.len);

		if ( (keyRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO) ||
		  (keyRowStatusValue == L7_ROW_STATUS_CREATE_AND_WAIT) || (keyRowStatusValue == L7_ROW_STATUS_DESTROY ))
		{
		 return XLIBRC_SUCCESS;
		}
	}
	else
	{
	  memset(objNameValue,0x0,sizeof(objNameValue)); 
	  /* retrieve object: Name */
	  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objNameValue, &owa.len);
	  if (owa.rc != XLIBRC_SUCCESS)
	  {
	    FPOBJ_TRACE_EXIT (bufp, owa);
	    return owa.rc;
	  }
	  FPOBJ_TRACE_VALUE (bufp, objNameValue, owa.len);

	  /* retrieve key: Index */
	  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
	      (xLibU8_t *) & keyIndexValue, &kwa.len);
	  if (kwa.rc != XLIBRC_SUCCESS)
	  {
	    kwa.rc = XLIBRC_FILTER_MISSING;
	    FPOBJ_TRACE_EXIT (bufp, kwa);
	    return kwa.rc;
	  }
	  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);

	  if( strlen(objNameValue) > L7_USMDB_MIB_DIFFSERV_CLASS_NAME_MAX)
	  {
	    owa.rc = XLIBRC_DIFFSERV_POLICYNAME_TOOLONG;    /* TODO: Change if 
	                                                       required */
	    FPOBJ_TRACE_EXIT (bufp, owa);
	    return owa.rc;
	  }

	  /* Verify if the new class name already exists or not */
	  if (usmDbDiffServClassNameToIndex(L7_UNIT_CURRENT, objNameValue, &newClassId
	        ) == L7_SUCCESS)
	  {
	    if(newClassId != keyIndexValue)
	    {
	      owa.rc = XLIBRC_CLASS_NAME_EXISTS;    /* TODO: Change if required */
	      FPOBJ_TRACE_EXIT (bufp, owa);
	      return owa.rc;
	    }
	  }
	}

  /* set the value in application */
  owa.l7rc = usmDbDiffServClassNameSet (L7_UNIT_CURRENT, keyIndexValue,
      objNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassDelete(L7_UNIT_CURRENT, keyIndexValue);

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/******************************************************************************
 *
 * @function fpObjGet_qosDiffServClassRule_Type
 *
 * @purpose Get 'Type'
 *
 * @description [Type]: The class type indicates how the individual class match
 *              criteria are evaluated. For type all, all of the individual
 *              match conditions must be true for a packet to be considered
 *              a member of the class. For types any and acl, 
 *
 * @return

 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_Type (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDiffServClassTypeGet (L7_UNIT_CURRENT, keyIndexValue,
      &objTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Type */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTypeValue,
      sizeof (objTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/******************************************************************************
 *
 * @function fpObjSet_qosDiffServClassRule_Type
 *
 * @purpose Set 'Type'
 *
 * @description [Type]: The class type indicates how the individual class match
 *              criteria are evaluated. For type all, all of the individual
 *              match conditions must be true for a packet to be considered
 *              a member of the class. For types any and acl, 
 *
 * @return

 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_Type (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTypeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  fpObjWa_t kwaRowStatus = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRowStatusValue = 0;
	
  kwaRowStatus.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRowStatus,
	(xLibU8_t *) &keyRowStatusValue, &kwaRowStatus.len);
  if (kwaRowStatus.rc != XLIBRC_SUCCESS)
  {
	kwaRowStatus.rc = XLIBRC_FILTER_MISSING;
	FPOBJ_TRACE_EXIT (bufp, kwaRowStatus);
	return kwaRowStatus.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRowStatusValue, kwaRowStatus.len);

  if ( (keyRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO) ||
      (keyRowStatusValue == L7_ROW_STATUS_CREATE_AND_WAIT) )
  {
     return XLIBRC_SUCCESS;
  }

  /* retrieve object: Type */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTypeValue, owa.len);

  /* retrieve key: Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbDiffServClassTypeSet (L7_UNIT_CURRENT, keyIndexValue,
      objTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassDelete(L7_UNIT_CURRENT, keyIndexValue);

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/******************************************************************************
 *
 * @function fpObjGet_qosDiffServClassRule_AclNum
 *
 * @purpose Get 'AclNum'
 *
 * @description [AclNum]: The access list number used to define the match 
 conditions
 *              for the DiffServ class. This attribute is only valid
 *              if the value of agentDiffServClassType is set to acl(3).
 *              Match conditions are copied from the ACL definiti 
 *
 * @return

 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_AclNum (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAclNumValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDiffServClassAclNumGet (L7_UNIT_CURRENT, keyIndexValue,
      &objAclNumValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AclNum */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAclNumValue,
      sizeof (objAclNumValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/******************************************************************************
 *
 * @function fpObjSet_qosDiffServClassRule_AclNum
 *
 * @purpose Set 'AclNum'
 *
 * @description [AclNum]: The access list number used to define the match 
 conditions
 *              for the DiffServ class. This attribute is only valid
 *              if the value of agentDiffServClassType is set to acl(3).
 *              Match conditions are copied from the ACL definiti 
 *
 * @return

 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_AclNum (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAclNumValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue;
  L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_t classType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AclNum */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAclNumValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAclNumValue, owa.len);

  /* retrieve key: Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);

  if ( (usmDbDiffServClassTypeGet(L7_UNIT_CURRENT, keyIndexValue, &classType) 
        != L7_SUCCESS) ||
      (classType != L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL) )
  {
    owa.rc = XLIBRC_DIFFSERV_CLASSTYPE_NOT_ACL;    /* TODO: Change if 
                                                      required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbDiffServClassAclNumSet (L7_UNIT_CURRENT, keyIndexValue,
      objAclNumValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/******************************************************************************
 *
 * @function fpObjGet_qosDiffServClassRule_RuleIndexNextFree
 *
 * @purpose Get 'RuleIndexNextFree'
 *
 * @description [RuleIndexNextFree]: This object contains an unused value for
 *              classRuleIndex, or a zero to indicate that none exists.
 *              This index is maintained on a per-class basis. 
 *
 * @return

 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_RuleIndexNextFree (void *wap, void *
    bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRuleIndexNextFreeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDiffServClassRuleIndexNext (L7_UNIT_CURRENT, keyIndexValue,
      &objRuleIndexNextFreeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (objRuleIndexNextFreeValue == 0)
  {
    owa.rc = XLIBRC_COMMON_TABLE_FULL;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RuleIndexNextFree */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRuleIndexNextFreeValue,
      sizeof (objRuleIndexNextFreeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/******************************************************************************
 *
 * @function fpObjGet_qosDiffServClassRule_ClassStorageType
 *
 * @purpose Get 'ClassStorageType'
 *
 * @description [ClassStorageType]: Storage-type for this conceptual row 
 *
 * @return

 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_ClassStorageType (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objClassStorageTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDiffServClassStorageTypeGet (L7_UNIT_CURRENT, 
      keyIndexValue,
      &objClassStorageTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ClassStorageType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objClassStorageTypeValue,
      sizeof (objClassStorageTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/******************************************************************************
 *
 * @function fpObjSet_qosDiffServClassRule_ClassStorageType
 *
 * @purpose Set 'ClassStorageType'
 *
 * @description [ClassStorageType]: Storage-type for this conceptual row 
 *
 * @return

 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_ClassStorageType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objClassStorageTypeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ClassStorageType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objClassStorageTypeValue, &owa
      .len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objClassStorageTypeValue, owa.len);

  /* retrieve key: Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbDiffServClassStorageTypeSet (L7_UNIT_CURRENT, 
      keyIndexValue,
      objClassStorageTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/******************************************************************************
 *
 * @function fpObjGet_qosDiffServClassRule_ClassClassRowStatus
 *
 * @purpose Get 'ClassClassRowStatus'
 *
 * @description [ClassClassRowStatus]: The status of this conceptual row. 
 Entries can
 *              not be deleted until all rows in agentDiffServClassRuleTable
 *              with corresponding values of agentDiffServClassIndex have
 *              been deleted. 
 *
 * @return

 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_ClassRowStatus (void *wap, void *
    bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objClassClassRowStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDiffServClassRowStatusGet (L7_UNIT_CURRENT, 
      keyIndexValue,
      &objClassClassRowStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ClassClassRowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objClassClassRowStatusValue,
      sizeof (objClassClassRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/******************************************************************************
 *
 * @function fpObjSet_qosDiffServClassRule_ClassRowStatus
 *
 * @purpose Set 'ClassClassRowStatus'
 *
 * @description [ClassClassRowStatus]: The status of this conceptual row. 
 Entries can
 *              not be deleted until all rows in agentDiffServClassRuleTable
 *              with corresponding values of agentDiffServClassIndex have
 *              been deleted. 
 *
 * @return

 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_ClassRowStatus (void *wap, void *
    bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objClassRowStatusValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue = 0;

  fpObjWa_t kwaClassName = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t kwaClassNameValue;

  fpObjWa_t kwaClassType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassTypeValue = 0;

  fpObjWa_t kwaClassProtoType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassProtoTypeValue = 0;

  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ClassRowStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objClassRowStatusValue, &owa.
      len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objClassRowStatusValue, owa.len);
  /* retrieve key: Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    keyIndexValue = 0; 
  } 
  /* call the usmdb only for add and delete */
  if ( (objClassRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO) ||
      (objClassRowStatusValue == L7_ROW_STATUS_CREATE_AND_WAIT) )
  {
    owa.l7rc = usmDbDiffServClassIndexNext(L7_UNIT_CURRENT,&keyIndexValue);
    if(owa.l7rc !=L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    /* Create a row */
    if (usmDbDiffServClassGet(L7_UNIT_CURRENT, keyIndexValue) != L7_SUCCESS)
    {
      /* creation is only allowed when setting row status to createAndGo or 
         createAndWait */
      owa.l7rc = usmDbDiffServClassCreate(L7_UNIT_CURRENT, keyIndexValue,
            ((objClassRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO) ? 
             L7_TRUE : L7_FALSE));
      if(owa.l7rc != L7_SUCCESS)
      {
        if(owa.l7rc == L7_REQUEST_DENIED)
        {
          owa.rc = XLIBRC_DIFFSERV_CHANGE_REQ_DENY_APP;
        }
        else
        {
          owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
        }
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }

	  /* get the class name and class type value */
		kwaClassName.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_Name,
		(xLibU8_t *) kwaClassNameValue, &kwaClassName.len);
		if (kwaClassName.rc != XLIBRC_SUCCESS)
		{
			kwaClassName.rc = XLIBRC_FILTER_MISSING;
			FPOBJ_TRACE_EXIT (bufp, kwaClassName);
			return kwaClassName.rc;
		}
		FPOBJ_TRACE_CURRENT_KEY (bufp, kwaClassNameValue, kwaClassName.len);

		/* get the class name and class type value */
		kwaClassType.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_Type,
		(xLibU8_t *) & keyClassTypeValue, &kwaClassType.len);
		if (kwaClassType.rc != XLIBRC_SUCCESS)
		{
			keyClassTypeValue = L7_DIFFSERV_CLASS_TYPE_ALL;
		}
		FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassTypeValue, kwaClassType.len);

		/* get the class protocol type value */
		kwaClassProtoType.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ProtoType,
		(xLibU8_t *) & keyClassProtoTypeValue, &kwaClassProtoType.len);
		if (kwaClassProtoType.rc != XLIBRC_SUCCESS)
		{
			keyClassProtoTypeValue = L7_DIFFSERV_CLASS_L3PROTOCOL_IPV4;
		}
		FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassProtoTypeValue, kwaClassProtoType.len);

		/*set the class name and class type as without them the class row cannot be active */
		owa.l7rc = usmDbDiffServClassNameSet(L7_UNIT_CURRENT,
		                                                     keyIndexValue, kwaClassNameValue);

		if (owa.l7rc != L7_SUCCESS)
      {
		usmDbDiffServClassDelete(L7_UNIT_CURRENT, keyIndexValue);
		owa.rc = XLIBRC_CLASS_NAME_EXISTS;  /* TODO: Change if required */
		FPOBJ_TRACE_EXIT (bufp, owa);
		return owa.rc;
      }
      owa.l7rc = usmDbDiffServClassTypeSet(L7_UNIT_CURRENT,
				                  keyIndexValue,keyClassTypeValue);
      if(owa.l7rc  != L7_SUCCESS)
      {
        usmDbDiffServClassDelete(L7_UNIT_CURRENT, keyIndexValue);
		owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
		FPOBJ_TRACE_EXIT (bufp, owa);
		return owa.rc;
      }

      owa.l7rc = usmDbDiffServClassL3ProtoSet(L7_UNIT_CURRENT,
				                   keyIndexValue,keyClassProtoTypeValue);
      if(owa.l7rc  != L7_SUCCESS)
      {
        usmDbDiffServClassDelete(L7_UNIT_CURRENT, keyIndexValue);
		owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
		FPOBJ_TRACE_EXIT (bufp, owa);
		return owa.rc;
      }

	   /*now check to make sure row is active */
		owa.l7rc = usmDbDiffServClassRowStatusGet(L7_UNIT_CURRENT,keyIndexValue, &status);
		if ( (owa.l7rc != L7_SUCCESS ) || (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE) )
		{
	        usmDbDiffServClassDelete(L7_UNIT_CURRENT, keyIndexValue);
			owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
			FPOBJ_TRACE_EXIT (bufp, owa);
			return owa.rc;
		}
		else   /* row status is success. need to push this value into the filter*/
		{
          owa.rc = xLibFilterSet(wap,XOBJ_qosDiffServClassRule_ClassIndex,
						               0,(xLibU8_t *) &keyIndexValue,sizeof(keyIndexValue));
		}
		if(owa.rc  != XLIBRC_SUCCESS)
		{
			usmDbDiffServClassDelete(L7_UNIT_CURRENT, keyIndexValue);
			owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
			FPOBJ_TRACE_EXIT (bufp, owa);
			return owa.rc;
		}
    }
    else
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (objClassRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* retrieve key: Index */
    kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
        (xLibU8_t *) & keyIndexValue, &kwa.len);
    if (kwa.rc != XLIBRC_SUCCESS)
    {
      kwa.rc = XLIBRC_FILTER_MISSING;
      FPOBJ_TRACE_EXIT (bufp, kwa);
      return kwa.rc;
    } 
    /* Delete the existing row */
    owa.l7rc = usmDbDiffServClassDelete(L7_UNIT_CURRENT, keyIndexValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      if (owa.l7rc == L7_REQUEST_DENIED)
      {
        owa.rc = XLIBRC_DIFFSERV_CHANGE_REQ_DENY_APP_USING_CLASS;
      }
      else
      {
        owa.rc = XLIBRC_DIFFSERV_CLASS_DELETE_FAILED;
      }
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  return XLIBRC_SUCCESS;
}


/******************************************************************************
 *
 * @function fpObjGet_qosDiffServClassRule_AclType
 *
 * @purpose Get 'AclType'
 *
 * @description [AclType]: The access list type used to define the match 
 conditions
 *              for the DiffServ class. This attribute is only valid
 *              if the value of agentDiffServClassType is set to acl(3).
 *              When this attribute is set to ip(1), 
 *
 * @return

 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_AclType (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAclTypeValue;  
  L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_t classType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);


  if ( (usmDbDiffServClassTypeGet(L7_UNIT_CURRENT, keyIndexValue, &classType) 
        != L7_SUCCESS) ||
      (classType != L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL) )
  {
    owa.rc = XLIBRC_DIFFSERV_CLASSTYPE_NOT_ACL;    /* TODO: Change if 
                                                      required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbDiffServClassAclTypeGet (L7_UNIT_CURRENT, keyIndexValue,
      &objAclTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AclType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAclTypeValue,
      sizeof (objAclTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/******************************************************************************
 *
 * @function fpObjSet_qosDiffServClassRule_AclType
 *
 * @purpose Set 'AclType'
 *
 * @description [AclType]: The access list type used to define the match 
 conditions
 *              for the DiffServ class. This attribute is only valid
 *              if the value of agentDiffServClassType is set to acl(3).
 *              When this attribute is set to ip(1), 
 *
 * @return

 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_AclType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAclTypeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue;
  L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_t classType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AclType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAclTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAclTypeValue, owa.len);


  /* retrieve key: Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);

  if ( (usmDbDiffServClassTypeGet(L7_UNIT_CURRENT, keyIndexValue, &classType) 
        != L7_SUCCESS) ||
      (classType != L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL) )
  {
    owa.rc = XLIBRC_DIFFSERV_CLASSTYPE_NOT_ACL;    /* TODO: Change if 
                                                      required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbDiffServClassAclTypeSet (L7_UNIT_CURRENT, keyIndexValue,
      objAclTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/******************************************************************************
 *
 * @function fpObjGet_qosDiffServClassRule_ProtoType
 *
 * @purpose Get 'ProtoType'
 *
 * @description [ProtoType]: The diffserv class protocol type used to indicate
 *              how to interpret any layer 3. This attribute is only valid
 *              if the value of agentDiffServClassProtoType is set to all(1)
 *              or any(2). 
 *
 * @return

 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_ProtoType (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objProtoTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDiffServClassL3ProtoGet (L7_UNIT_CURRENT, keyIndexValue,
      &objProtoTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ProtoType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objProtoTypeValue,
      sizeof (objProtoTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/******************************************************************************
 *
 * @function fpObjSet_qosDiffServClassRule_ProtoType
 *
 * @purpose Set 'ProtoType'
 *
 * @description [ProtoType]: The diffserv class protocol type used to indicate
 *              how to interpret any layer 3. This attribute is only valid
 *              if the value of agentDiffServClassProtoType is set to all(1)
 *              or any(2). 
 *
 * @return

 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_ProtoType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objProtoTypeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  fpObjWa_t kwaRowStatus = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRowStatusValue = 0;
	
  kwaRowStatus.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRowStatus,
	(xLibU8_t *) &keyRowStatusValue, &kwaRowStatus.len);
  if (kwaRowStatus.rc != XLIBRC_SUCCESS)
  {
	kwaRowStatus.rc = XLIBRC_FILTER_MISSING;
	FPOBJ_TRACE_EXIT (bufp, kwaRowStatus);
	return kwaRowStatus.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRowStatusValue, kwaRowStatus.len);

  if ( (keyRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO) ||
      (keyRowStatusValue == L7_ROW_STATUS_CREATE_AND_WAIT) )
  {
     return XLIBRC_SUCCESS;
  }
	

  /* retrieve object: ProtoType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objProtoTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objProtoTypeValue, owa.len);



  /* retrieve key: Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {

    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbDiffServClassL3ProtoSet (L7_UNIT_CURRENT, keyIndexValue,
      objProtoTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServClassDelete(L7_UNIT_CURRENT, keyIndexValue);

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/******************************************************************************
 *
 * @function fpObjGet_qosDiffServClassRule_isReferenceClass
 *
 * @purpose Get 'ProtoType'
 *
 * @description [isReferenceClass]:To find a class has reference class
 *              
 *              
 *              
 *
 * @return

 *******************************************************************************/

xLibRC_t fpObjGet_qosDiffServClassRule_isReferenceClass (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objisRefClass,tempClassId,tempClassRuleId;
  FPOBJ_TRACE_ENTER (bufp);

   /* retrieve key: Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);

  if ((usmDbDiffServClassToRefClass(L7_UNIT_CURRENT, keyIndexValue, 
                                   &tempClassId ) == L7_SUCCESS ) &&
    (usmDbDiffServClassRuleMatchRefClassRuleIndexFind(L7_UNIT_CURRENT, 
                        keyIndexValue, &tempClassRuleId )==L7_SUCCESS))
  {
    objisRefClass = L7_DIFFSERV_TRUTH_VALUE_TRUE;
  }
  else
  {
    objisRefClass = L7_DIFFSERV_TRUTH_VALUE_FALSE;
  }
  /* return the object value: ProtoType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objisRefClass,
      sizeof (objisRefClass));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/******************************************************************************
 *
 * @function fpObjSet_qosDiffServClassRule_RemoveReferenceClass
 *
 * @purpose 
 *
 * @description [RemoveReferenceClass]:To remove reference class form 
 *               class
 *              
 *              
 *
 * @return

 *******************************************************************************/

xLibRC_t fpObjSet_qosDiffServClassRule_RemoveReferenceClass (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue;
  xLibU32_t classRuleId,tempClassId;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);

  if (usmDbDiffServClassToRefClass(L7_UNIT_CURRENT, keyIndexValue, 
      &tempClassId ) != L7_SUCCESS )
  {
    owa.rc = XLIBRC_DIFFSERV_REFCLASS_NOTEXIST;   
                 
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
   
  if (usmDbDiffServClassRuleMatchRefClassRuleIndexFind(L7_UNIT_CURRENT, 
                    keyIndexValue, &classRuleId) != L7_SUCCESS)
  {
    owa.rc =XLIBRC_DIFFSERV_REFCLASS_NOTEXIST;

    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  if (usmDbDiffServClassRuleMatchRefClassIndexRemove(
            L7_UNIT_CURRENT, keyIndexValue, classRuleId) != L7_SUCCESS)
  {

    owa.rc =XLIBRC_DIFFSERV_DELETE_FAILED;

    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc; 
}
  
/*********************************************************************
* @purpose  used for diffserv, Converts a Layer 4 Port Keyword value
*           to the associated keyword, if one is available.
*           Otherwise, a string of the input dscpVal is returned.
*
* @param    dscpVal            value to convert
* @param    dscpString         string to return
* @param    dscpStringLenMax   Maximum number of bytes to copy
*
* @returns rc
*
* @notes none
*
* @end
*********************************************************************/

L7_RC_t diffservConvertL4PortValToString(L7_uint32 val, L7_char8 * dscpString, L7_int32 dscpStringLenMax)
{
  switch(val)
  {
  case L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_DOMAIN:
    osapiSnprintf (dscpString, dscpStringLenMax,  "%u(%s)", val, "domain");
    break;
  case L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_ECHO:
    osapiSnprintf (dscpString, dscpStringLenMax,  "%u(%s)", val, "echo");
    break;
  case L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_FTP:
    osapiSnprintf (dscpString, dscpStringLenMax,  "%u(%s)", val, "ftp");
    break;
  case L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_FTPDATA:
    osapiSnprintf (dscpString, dscpStringLenMax,  "%u(%s)", val, "ftpdata");
    break;
  case L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_HTTP:
    osapiSnprintf (dscpString, dscpStringLenMax,  "%u(%s/%s)", val, "http","www");
    break;
  case L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_SMTP:
    osapiSnprintf (dscpString, dscpStringLenMax,  "%u(%s)", val, "smtp");
    break;
  case L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_SNMP:
    osapiSnprintf (dscpString, dscpStringLenMax,  "%u(%s)", val, "snmp");
    break;
  case L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_TELNET:
    osapiSnprintf (dscpString, dscpStringLenMax,  "%u(%s)", val, "telnet");
    break;
  case L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_TFTP:
    osapiSnprintf (dscpString, dscpStringLenMax,  "%u(%s)", val, "tftp");
    break;
  default:
    osapiSnprintf (dscpString, dscpStringLenMax,  "%u", val);
    break;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts a DSCP value to its associated keyword, if one
*           is available.  Otherwise, a string of the numeric dscpVal
*           input value is returned.
*
* @param    dscpVal         value to convert
* @param    dscpString      string to return
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Primarily used for QOS Diffserv.
*
* @end
*********************************************************************/
L7_RC_t convertDSCPValToString(L7_uint32 dscpVal, L7_char8* dscpString)
{
    L7_uchar8 *pFmtStr = L7_NULLPTR;

    switch(dscpVal)
    {
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF11:
        pFmtStr = (L7_uchar8 *)"%u(af11)";
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF12:
        pFmtStr = (L7_uchar8 *)"%u(af12)";
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF13:
        pFmtStr = (L7_uchar8 *)"%u(af13)";
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF21:
        pFmtStr = (L7_uchar8 *)"%u(af21)";
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF22:
        pFmtStr = (L7_uchar8 *)"%u(af22)";
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF23:
        pFmtStr = (L7_uchar8 *)"%u(af23)";
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF31:
        pFmtStr = (L7_uchar8 *)"%u(af31)";
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF32:
        pFmtStr = (L7_uchar8 *)"%u(af32)";
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF33:
        pFmtStr = (L7_uchar8 *)"%u(af33)";
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF41:
        pFmtStr = (L7_uchar8 *)"%u(af41)";
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF42:
        pFmtStr = (L7_uchar8 *)"%u(af42)";
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF43:
        pFmtStr = (L7_uchar8 *)"%u(af43)";
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_BE:
        pFmtStr = (L7_uchar8 *)"%u(be/cs0)";
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS1:
        pFmtStr = (L7_uchar8 *)"%u(cs1)";
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS2:
        pFmtStr = (L7_uchar8 *)"%u(cs2)";
        break;
     case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS3:
        pFmtStr = (L7_uchar8 *)"%u(cs3)";
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS4:
        pFmtStr = (L7_uchar8 *)"%u(cs4)";
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS5:
        pFmtStr = (L7_uchar8 *)"%u(cs5)";
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS6:
        pFmtStr = (L7_uchar8 *)"%u(cs6)";
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS7:
        pFmtStr = (L7_uchar8 *)"%u(cs7)";
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_EF:
        pFmtStr = (L7_uchar8 *)"%u(ef)";
        break;
    default:
        pFmtStr = (L7_uchar8 *)"%u";
        break;
    }

    sprintf ((char *)dscpString, (char *)pFmtStr, dscpVal);
    return L7_SUCCESS;
}
 
/*********************************************************************
* @purpose  used for diffserv, Converts a Ethertype keyword ID to its
*           associated keyword string
*
* @param    etypeKeyId      keyword id to return
* @param    strKeyword      string to parse
 * @param    strKeywordSize  size of output buffer
*
* @returns rc
*
* @notes none
*
* @end
*********************************************************************/

L7_RC_t diffservConvertEtherTypeKeyIdToString(L7_uint32 etypeKeyId, L7_char8 * strKeyword, L7_uint32 strKeywordSize)
{
  L7_char8 * pStr = L7_NULLPTR;

  switch (etypeKeyId)
  {
    case L7_QOS_ETYPE_KEYID_APPLETALK:
      pStr = L7_QOS_ETYPE_STR_APPLETALK;
      break;

    case L7_QOS_ETYPE_KEYID_ARP:
      pStr = L7_QOS_ETYPE_STR_ARP;
      break;

    case L7_QOS_ETYPE_KEYID_IBMSNA:
      pStr = L7_QOS_ETYPE_STR_IBMSNA;
      break;

    case L7_QOS_ETYPE_KEYID_IPV4:
      pStr = L7_QOS_ETYPE_STR_IPV4;
      break;

    case L7_QOS_ETYPE_KEYID_IPV6:
      pStr = L7_QOS_ETYPE_STR_IPV6;
      break;

    case L7_QOS_ETYPE_KEYID_IPX:
      pStr = L7_QOS_ETYPE_STR_IPX;
      break;

    case L7_QOS_ETYPE_KEYID_MPLSMCAST:
      pStr = L7_QOS_ETYPE_STR_MPLSMCAST;
      break;

    case L7_QOS_ETYPE_KEYID_MPLSUCAST:
      pStr = L7_QOS_ETYPE_STR_MPLSUCAST;
      break;

    case L7_QOS_ETYPE_KEYID_NETBIOS:
      pStr = L7_QOS_ETYPE_STR_NETBIOS;
      break;

    case L7_QOS_ETYPE_KEYID_NOVELL:
      pStr = L7_QOS_ETYPE_STR_NOVELL;
      break;

    case L7_QOS_ETYPE_KEYID_PPPOE:
      pStr = L7_QOS_ETYPE_STR_PPPOE;
      break;
    case L7_QOS_ETYPE_KEYID_RARP:
      pStr = L7_QOS_ETYPE_STR_RARP;
      break;

    default:
      return L7_FAILURE;
      /*PASSTHRU*/

  } /* endswitch */

  osapiStrncpySafe(strKeyword, pStr, strKeywordSize);

  return L7_SUCCESS;
}
 
/*******************************************************************************
 * @function fpObjList_qosDiffServClassRule_MatchEntryTypeList
 *
 * @purpose Populate the list 'MatchEntryTypeList'
 *
 * @description [MatchEntryTypeList]: Class Rule match entry type, it determines
 *              which one of the individual match conditions is defined for
 *              the present class rule. This object must be created before
 *              any other match object in this row. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjList_qosDiffServClassRule_MatchEntryTypeList(void *wap, void *bufp) 
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t i=0, keyIndexValue, maxListIndex=0;
  xLibU32_t objProtoTypeValue,objisRefClass,tempClassId,tempClassRuleId;
  xLibStr256_t  objMatchEntryTypeListValue;
  xLibStr256_t  nextMatchEntryTypeListValue;
  xLibU8_t *v4selectorList[]= {" ","Class of Service","Secondary Class of Service","Destination IP Address",
                               "Destination L4 Port","Destination MAC Address","Ethertype","Any","IP DSCP",
                               "IP Precedence","IP TOS","Protocol","Reference Class","Source IP Address",
                               "Source L4 Port","Source MAC Address","VLAN","Secondary VLANID"};
  xLibU8_t *v6selectorList[]= {" ","Destination IPv6 Address","Destination L4 Port","Any","Flow Label",
                               "IP DSCP","Protocol","Reference Class","Source IPv6 Address","Source L4 Port"};
  xLibU8_t **list = L7_NULLPTR;
  xLibU32_t v4FeatureList[] = {0,                                              L7_DIFFSERV_CLASS_MATCH_COS_FEATURE_ID,  
                               L7_DIFFSERV_CLASS_MATCH_COS2_FEATURE_ID,        L7_DIFFSERV_CLASS_MATCH_DSTIP_FEATURE_ID,  
                               L7_DIFFSERV_CLASS_MATCH_DSTL4PORT_FEATURE_ID,   L7_DIFFSERV_CLASS_MATCH_DSTMAC_FEATURE_ID, 
                               L7_DIFFSERV_CLASS_MATCH_ETYPE_FEATURE_ID,       L7_DIFFSERV_CLASS_MATCH_EVERY_FEATURE_ID,
                               L7_DIFFSERV_CLASS_MATCH_IPDSCP_FEATURE_ID,      L7_DIFFSERV_CLASS_MATCH_IPPRECEDENCE_FEATURE_ID,
                               L7_DIFFSERV_CLASS_MATCH_IPTOS_FEATURE_ID,       L7_DIFFSERV_CLASS_MATCH_PROTOCOL_FEATURE_ID,    
                               L7_DIFFSERV_CLASS_MATCH_REFCLASS_FEATURE_ID,    L7_DIFFSERV_CLASS_MATCH_SRCIP_FEATURE_ID,
                               L7_DIFFSERV_CLASS_MATCH_SRCL4PORT_FEATURE_ID,   L7_DIFFSERV_CLASS_MATCH_SRCMAC_FEATURE_ID,  
                               L7_DIFFSERV_CLASS_MATCH_VLANID_FEATURE_ID,      L7_DIFFSERV_CLASS_MATCH_VLANID2_FEATURE_ID };
  xLibU32_t *featList = L7_NULLPTR;
  xLibU32_t v6FeatureList[] = {0,                                                 L7_DIFFSERV_CLASS_MATCH_IPV6_DSTIP_FEATURE_ID,  
                               L7_DIFFSERV_CLASS_MATCH_IPV6_DSTL4PORT_FEATURE_ID, L7_DIFFSERV_CLASS_MATCH_EVERY_FEATURE_ID,  
                               L7_DIFFSERV_CLASS_MATCH_IPV6_FLOWLBL_FEATURE_ID,   L7_DIFFSERV_CLASS_MATCH_IPV6_IPDSCP_FEATURE_ID,  
                               L7_DIFFSERV_CLASS_MATCH_IPV6_PROTOCOL_FEATURE_ID,  L7_DIFFSERV_CLASS_MATCH_REFCLASS_FEATURE_ID,
                               L7_DIFFSERV_CLASS_MATCH_IPV6_SRCIP_FEATURE_ID,     L7_DIFFSERV_CLASS_MATCH_IPV6_SRCL4PORT_FEATURE_ID };
                                 


  list = v4selectorList; 
  featList = v4FeatureList;
  memset(objMatchEntryTypeListValue, 0x0, sizeof(objMatchEntryTypeListValue));
  memset(nextMatchEntryTypeListValue, 0x0, sizeof(nextMatchEntryTypeListValue));
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ProtoType */
  kwa.len = sizeof(objProtoTypeValue);
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ProtoType,
                           (xLibU8_t *) & objProtoTypeValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objProtoTypeValue, kwa.len);
  if (objProtoTypeValue == L7_DIFFSERV_CLASS_L3PROTOCOL_IPV4)
  {
    list = v4selectorList;
    featList = v4FeatureList;
    maxListIndex = 17;
  }
  else /* L7_DIFFSERV_CLASS_L3PROTOCOL_IPV6 */
  {
    list = v6selectorList;
    featList = v6FeatureList;
    maxListIndex = 9;   
  }

  /* retrieve key: Index */
  owa.len = sizeof (keyIndexValue);
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);

  if ((usmDbDiffServClassToRefClass(L7_UNIT_CURRENT, keyIndexValue,
                                   &tempClassId ) == L7_SUCCESS ) &&
    (usmDbDiffServClassRuleMatchRefClassRuleIndexFind(L7_UNIT_CURRENT,
                        keyIndexValue, &tempClassRuleId )==L7_SUCCESS))
  {
    objisRefClass = L7_DIFFSERV_TRUTH_VALUE_TRUE;
  }
  else
  {
    objisRefClass = L7_DIFFSERV_TRUTH_VALUE_FALSE;
  }

  /* retrieve List element */
  owa.len = sizeof (objMatchEntryTypeListValue);
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_MatchEntryTypeList,
                          (xLibU8_t *) &objMatchEntryTypeListValue, &owa.len);                          
 
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    osapiStrncpy(nextMatchEntryTypeListValue, list[0], sizeof(list[0]));
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objMatchEntryTypeListValue, owa.len);
    
    owa.l7rc = L7_FAILURE;  
    for (i=0; i<=maxListIndex; i++)
    {
      if ((osapiStrncmp(list[i], objMatchEntryTypeListValue, strlen(list[i]))) ==0) 
      {
        /* Loop to pick the next supported Feature to be added into the List. */
        do
        {
          if (i == maxListIndex)
          {  
            break;
          }
       
          if(usmDbFeaturePresentCheck(L7_UNIT_CURRENT,L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,featList[i+1]) != L7_TRUE)
          {
            i = i+1;
            continue;
          }
          else
          {
            if (((osapiStrncmp(list[i+1], "Reference Class", strlen(list[i+1]))) == 0)
                                    && (objisRefClass == L7_DIFFSERV_TRUTH_VALUE_TRUE))
            {
              i = i+1;
              continue;
            }
            else
            {
              strcpy(nextMatchEntryTypeListValue, list[i+1]);
              owa.l7rc = L7_SUCCESS;
              break;
            }
          }
        }while(1);
        
        if ((i == maxListIndex) || (owa.l7rc == L7_SUCCESS))
        {
          break;
        }
      }/*end-of-Main-IF-Block*/       
    }/*end-of-For-Loop*/
  }  
    
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextMatchEntryTypeListValue, owa.len);
 
  /* return the object value: UserName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextMatchEntryTypeListValue, strlen (nextMatchEntryTypeListValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
 
/*******************************************************************************
 * @function fpObjGet_qosDiffServClassRule_MatchEntryTypeList
 *
 * @purpose Get 'MatchEntryTypeList'
 *
 * @description [MatchEntryTypeList]: Class Rule match entry type, it determines
 *              which one of the individual match conditions is defined for
 *              the present class rule. This object must be created before
 *              any other match object in this row. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServClassRule_MatchEntryTypeList (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchEntryTypeListValue;
  xLibStr256_t objMatchEntryTypeListStringVal;
  xLibU8_t *matchTypeList[]= {" ","Class of Service","Secondary Class of Service","Destination IP Address",
                              "Destination IPv6 Address","Destination L4 Port",
                              "Destination MAC Address","Ethertype","Any","Flow Label","IP DSCP","IP Precedence",
                              "IP TOS","Protocol","Reference Class","Source IP Address","Source IPv6 Address",
                              "Source L4 Port","Source MAC Address","VLAN","Secondary VLANID"};

  FPOBJ_TRACE_ENTER (bufp);

  memset(objMatchEntryTypeListStringVal, 0x0, sizeof(objMatchEntryTypeListStringVal));

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchEntryTypeGet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        &objMatchEntryTypeListValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* Convert Enum value to String */
  osapiStrncpy(objMatchEntryTypeListStringVal, matchTypeList[objMatchEntryTypeListValue],sizeof(objMatchEntryTypeListStringVal));

  /* return the object value: MatchEntryType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMatchEntryTypeListStringVal,
      sizeof (objMatchEntryTypeListStringVal));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
 * @function fpObjSet_qosDiffServClassRule_MatchEntryTypeList
 *
 * @purpose Set 'MatchEntryTypeList'
 *
 * @description [MatchEntryTypeList]: Class Rule match entry type, it determines
 *              which one of the individual match conditions is defined for
 *              the present class rule. This object must be created before
 *              any other match object in this row. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServClassRule_MatchEntryTypeList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMatchEntryTypeListValue = 0;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassIndexValue = 0;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyClassRuleIndexValue = 0;
  xLibU32_t i=0;                         
  xLibStr256_t objMatchEntryTypeListStringVal;
  xLibU8_t *matchTypeList[]= {" ","Class of Service","Secondary Class of Service","Destination IP Address",
                              "Destination IPv6 Address","Destination L4 Port",
                              "Destination MAC Address","Ethertype","Any","Flow Label","IP DSCP","IP Precedence",
                              "IP TOS","Protocol","Reference Class","Source IP Address","Source IPv6 Address",
                              "Source L4 Port","Source MAC Address","VLAN","Secondary VLANID"};
 

  FPOBJ_TRACE_ENTER (bufp);

  memset(objMatchEntryTypeListStringVal, 0x0, sizeof(objMatchEntryTypeListStringVal));

  /* retrieve object: MatchEntryType */
  owa.len = sizeof(objMatchEntryTypeListStringVal);
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) & objMatchEntryTypeListStringVal, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMatchEntryTypeListStringVal, owa.len);
  /* Convert the string to Enum value */
  for (i=0; i<=20; i++)
  {
    if ((osapiStrncmp(matchTypeList[i], objMatchEntryTypeListStringVal, strlen(matchTypeList[i]))) ==0)
    {
      objMatchEntryTypeListValue = i;
      break;
    }
  }  
  if (i==21)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    owa.rc = XLIBRC_INVALID_STRING;
    return owa.rc;
  }

  /* retrieve key: ClassIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassIndex,
      (xLibU8_t *) & keyClassIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassIndexValue, kwa1.len);

  /* retrieve key: ClassRuleIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServClassRule_ClassRuleIndex,
      (xLibU8_t *) & keyClassRuleIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyClassRuleIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServClassRuleMatchEntryTypeSet (L7_UNIT_CURRENT,
        keyClassIndexValue,
        keyClassRuleIndexValue,
        objMatchEntryTypeListValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    /* Delete the existing row if failed to set the value */
    usmDbDiffServClassRuleDelete(L7_UNIT_CURRENT, keyClassIndexValue, keyClassRuleIndexValue);
    owa.rc =  XLIBRC_CLASS_RULE_NOT_CREATED;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;

  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

