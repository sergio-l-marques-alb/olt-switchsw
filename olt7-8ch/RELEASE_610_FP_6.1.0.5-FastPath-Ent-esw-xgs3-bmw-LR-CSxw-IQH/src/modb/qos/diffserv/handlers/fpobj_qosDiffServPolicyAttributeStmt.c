/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_qosDiffServPolicyAttributeStmt.c
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
#include "_xe_qosDiffServPolicyAttributeStmt_obj.h"
#include "usmdb_mib_diffserv_private_api.h"
#include "usmdb_util_api.h" 
#include "usmdb_mib_diffserv_api.h"

xLibRC_t xLibFilterSet (void *arg, xLibId_t oid, xLibU16_t type, xLibU8_t * val, xLibU16_t 
len);


xLibRC_t ObjUtilDiffServPolicyClsDefStrToIndexValGet(xLibS8_t *str, xLibU32_t *val)
{
  xLibU32_t i = 0;
  xLibU8_t *matchTypeList[] = {"None","Assign Queue","Drop","Mark Cos","Mark CoS As Secondary CoS","Mark IP DSCP",
                               "Mark IP Precedence","Mirror","Simple","Single Rate","Two Rate","Redirect"};

  /* Convert the string to Enum value */
  for (i=0; i<=11; i++)
  {
    if ((osapiStrncmp(matchTypeList[i], str, strlen(str))) == 0)
    {
      *val = i;
      return XLIBRC_SUCCESS;
    }
  }  
  
  /* (i==12) ...End-of-Table */
  return XLIBRC_FAILURE;
}


xLibRC_t ObjUtilDiffServPolicyClassEnumValToStrGet(xLibU32_t val, xLibU8_t *str)
{
  xLibU32_t i = 0;
  xLibU8_t *matchTypeList[] = {"None","Assign Queue","Drop","Mark Cos","Mark CoS As Secondary CoS","Mark IP DSCP",
                               "Mark IP Precedence","Mirror","Simple","Single Rate","Two Rate","Redirect"};          
  xLibU32_t AttrTypeEnumList[] = {L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE,
                                  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE,     
                                  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP,
                                  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL,
                                  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2,
                                  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL,
                                  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL,
                                  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR,
                                  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE,
                                  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE,
                                  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE,
                                 L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT};
 
  memset(str, 0x0, sizeof(str));

  /* Convert Enum value to String */
  for (i=0; i<=11; i++)
  {
    if (AttrTypeEnumList[i] == val)
    {
      osapiStrncpy(str, matchTypeList[i], strlen(matchTypeList[i]));
      return XLIBRC_SUCCESS;
    }
  }
  
  /* (i==12) ...End-of-Table */
  return XLIBRC_FAILURE;
}


xLibBool_t resolveconflict(xLibU32_t objEntryTypeValue, xLibU32_t policyId, 
                               xLibU32_t policyInstId, xLibBool_t attribCfg)
{
  xLibU32_t FeatureList[]     =   {0,                
                                   L7_DIFFSERV_POLICY_ATTR_ASSIGN_QUEUE_FEATURE_ID,
                                   0,
                                   L7_DIFFSERV_POLICY_ATTR_DROP_FEATURE_ID,        
                                   0,
                                   L7_DIFFSERV_POLICY_ATTR_MARK_COSVAL_FEATURE_ID,         
                                   L7_DIFFSERV_POLICY_ATTR_MARK_COS2VAL_FEATURE_ID,         
                                   L7_DIFFSERV_POLICY_ATTR_MARK_COSASCOS2_FEATURE_ID,
                                   L7_DIFFSERV_POLICY_ATTR_MARK_IPDSCPVAL_FEATURE_ID,      
                                   L7_DIFFSERV_POLICY_ATTR_MARK_IPPRECEDENCEVAL_FEATURE_ID,
                                   L7_DIFFSERV_POLICY_ATTR_MIRROR_FEATURE_ID,              
                                   L7_DIFFSERV_POLICY_ATTR_POLICE_SIMPLE_FEATURE_ID,       
                                   L7_DIFFSERV_POLICY_ATTR_POLICE_SINGLERATE_FEATURE_ID,   
                                   L7_DIFFSERV_POLICY_ATTR_POLICE_TWORATE_FEATURE_ID,      
                                   0,
                                   L7_DIFFSERV_POLICY_ATTR_REDIRECT_FEATURE_ID};
  xLibU32_t nextPolicyId =0, nextPolicyInstId =0,nextPolicyAttrId =0;
  xLibU32_t entryType =0, prevPolicyAttrId = 0;

  if(objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE)
  {
    return XLIB_FALSE;
  }
 
  if(usmDbFeaturePresentCheck(L7_UNIT_CURRENT,L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,FeatureList[objEntryTypeValue]) != L7_TRUE)
  {
    return XLIB_FALSE;
  }
  else if (attribCfg == XLIB_FALSE)
  {
    return XLIB_TRUE;
  }  
 
  while (usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT, policyId, policyInstId,
                                        prevPolicyAttrId, &nextPolicyId, 
                                        &nextPolicyInstId,&nextPolicyAttrId) == L7_SUCCESS &&
                                        nextPolicyId == policyId &&
                                        nextPolicyInstId == policyInstId)
  {
    if (usmDbDiffServPolicyAttrStmtEntryTypeGet(L7_UNIT_CURRENT, nextPolicyId, nextPolicyInstId,
                                            nextPolicyAttrId, &entryType) != L7_SUCCESS)
    {
      return XLIB_FALSE;   
    }
    prevPolicyAttrId = nextPolicyAttrId;

    switch(entryType)
    {
     /* Assign Queue*/
     case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE:
     {
       if (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP)
       { 
         return XLIB_FALSE;   
       }
       break;
     }      
     /* Mark Cos*/  
     case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL:
     {
       if((objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE) || 
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE))
       {
         return XLIB_FALSE;
       }
       break;
     }    
     /* Mirror */
     case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR:
     {
       if((objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT))
       {
         return XLIB_FALSE;
       }
       break;
     }
     /* Drop */  
     case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
     {
       if((objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL) ||                          
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE) || 
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT))
       {
         return XLIB_FALSE;
       }
       break;
     }
     /* Mark Cos2 */     
     case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2:
     {
       if((objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE) || 
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP))
       {
         return XLIB_FALSE;
       }
       break;
     }
     /* Mark IP DSCP */     
     case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL:
     {
       if((objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE) || 
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE))
       {
         return L7_FALSE;
       }
       break;
     }
     /* Mark IP Precedence */     
     case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL:
     {
       if((objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE) || 
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE))
       {
         return XLIB_FALSE;
       }
       break;
     }
     /* Simple */     
     case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
     {
       if((objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE) || 
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE))
       {
         return XLIB_FALSE;
       }
       break;
     }     
     /* Single Rate */     
     case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
     {
       if((objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE) || 
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE))
       {
         return XLIB_FALSE;
       }
       break;
     }
     /* Single Rate */     
     case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
     {
       if((objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE) || 
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE))
       {
         return XLIB_FALSE;
       }
       break;
     }
     /* Redirect */
     case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT:
     {
       if((objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP) ||
          (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR))
       {
         return XLIB_FALSE;
       }
       break;
     }   
     default:
     {
       return XLIB_FALSE;  
     }
    }/*end-of-Switch-Case*/ 
  }/*end-of-While*/

  return XLIB_TRUE;
}/*end-of-resolveconflict*/



/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyAttributeStmt_PolicyIndex
*
* @purpose Get 'PolicyIndex'
*
* @description [PolicyIndex] The identifier for DiffServ Policy table entry.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_PolicyIndex (void *wap, void *bufp)
{

  xLibU32_t objPolicyIndexValue;
  xLibU32_t nextObjPolicyIndexValue;
  xLibU32_t objPolicyInstIndexValue;
  xLibU32_t nextObjPolicyInstIndexValue;
  xLibU32_t objPolicyAttrIndexValue;
  xLibU32_t nextObjPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                          (xLibU8_t *) & objPolicyIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objPolicyIndexValue = objPolicyInstIndexValue = objPolicyAttrIndexValue = 0;
    owa.l7rc = usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT,
                                              objPolicyIndexValue,
                                              objPolicyInstIndexValue,
                                              objPolicyAttrIndexValue, 
                                              &nextObjPolicyIndexValue,
                                              &nextObjPolicyInstIndexValue, 
                                              &nextObjPolicyAttrIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objPolicyIndexValue, owa.len);
    objPolicyInstIndexValue = objPolicyAttrIndexValue = 0;
    nextObjPolicyInstIndexValue = nextObjPolicyAttrIndexValue = 0;
    nextObjPolicyIndexValue = objPolicyIndexValue;
    do
    {
      objPolicyInstIndexValue = nextObjPolicyInstIndexValue;
      objPolicyAttrIndexValue = nextObjPolicyAttrIndexValue;
      owa.l7rc = usmDbDiffServPolicyAttrGetNext (L7_UNIT_CURRENT,
                                                 objPolicyIndexValue,
                                                 objPolicyInstIndexValue,
                                                 objPolicyAttrIndexValue, 
                                                 &nextObjPolicyIndexValue,
                                                 &nextObjPolicyInstIndexValue, 
                                                 &nextObjPolicyAttrIndexValue);
    }
    while ((objPolicyIndexValue == nextObjPolicyIndexValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjPolicyIndexValue, owa.len);

  /* return the object value: PolicyIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjPolicyIndexValue,
                           sizeof (objPolicyIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyAttributeStmt_PolicyInstIndex
*
* @purpose Get 'PolicyInstIndex'
*
* @description [PolicyInstIndex] The identifier for policy-class Instance table entry within a policy.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_PolicyInstIndex (void *wap, void *bufp)
{

  xLibU32_t objPolicyIndexValue;
  xLibU32_t nextObjPolicyIndexValue;
  xLibU32_t objPolicyInstIndexValue;
  xLibU32_t nextObjPolicyInstIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                          (xLibU8_t *) & objPolicyIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objPolicyIndexValue, owa.len);

  /* retrieve key: PolicyInstIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                          (xLibU8_t *) & objPolicyInstIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objPolicyInstIndexValue = 0;
    owa.l7rc = usmDbDiffServPolicyInstGetNext (L7_UNIT_CURRENT,
                                               objPolicyIndexValue,
                                               objPolicyInstIndexValue,
                                               &nextObjPolicyIndexValue,
                                               &nextObjPolicyInstIndexValue 
                                               );
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objPolicyInstIndexValue, owa.len);
    nextObjPolicyIndexValue = objPolicyIndexValue;
    nextObjPolicyInstIndexValue = objPolicyInstIndexValue;
    
    do
    {
      owa.l7rc = usmDbDiffServPolicyInstGetNext (L7_UNIT_CURRENT,
                                                 objPolicyIndexValue,
                                                 objPolicyInstIndexValue,
                                                 &nextObjPolicyIndexValue,
                                                 &nextObjPolicyInstIndexValue
                                                 );
    }
    while ((objPolicyIndexValue == nextObjPolicyIndexValue)
           && (objPolicyInstIndexValue == nextObjPolicyInstIndexValue) && (owa.l7rc == L7_SUCCESS));
  }
  
  if ((objPolicyIndexValue != nextObjPolicyIndexValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjPolicyInstIndexValue, owa.len);

  /* return the object value: PolicyInstIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjPolicyInstIndexValue,
                           sizeof (objPolicyInstIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyAttributeStmt_PolicyAttrIndex
*
* @purpose Get 'PolicyAttrIndex'
*
* @description [PolicyAttrIndex] The identifier for policy-attribute entry 
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_PolicyAttrIndex (void *wap, void *bufp)
{

  xLibU32_t objPolicyIndexValue;
  xLibU32_t nextObjPolicyIndexValue;
  xLibU32_t objPolicyInstIndexValue;
  xLibU32_t nextObjPolicyInstIndexValue;
  xLibU32_t objPolicyAttrIndexValue;
  xLibU32_t nextObjPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                          (xLibU8_t *) & objPolicyIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objPolicyIndexValue, owa.len);

  /* retrieve key: PolicyInstIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                          (xLibU8_t *) & objPolicyInstIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objPolicyInstIndexValue, owa.len);

  /* retrieve key: PolicyAttrIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                          (xLibU8_t *) & objPolicyAttrIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objPolicyAttrIndexValue = 0;
    owa.l7rc = usmDbDiffServPolicyAttrGetNext (L7_UNIT_CURRENT,
                                               objPolicyIndexValue,
                                               objPolicyInstIndexValue,
                                               objPolicyAttrIndexValue, 
                                               &nextObjPolicyIndexValue,
                                               &nextObjPolicyInstIndexValue, 
                                               &nextObjPolicyAttrIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objPolicyAttrIndexValue, owa.len);

    owa.l7rc = usmDbDiffServPolicyAttrGetNext (L7_UNIT_CURRENT,
                                               objPolicyIndexValue,
                                               objPolicyInstIndexValue,
                                               objPolicyAttrIndexValue, 
                                               &nextObjPolicyIndexValue,
                                               &nextObjPolicyInstIndexValue, 
                                               &nextObjPolicyAttrIndexValue);

  }

  if ((objPolicyIndexValue != nextObjPolicyIndexValue)
      || (objPolicyInstIndexValue != nextObjPolicyInstIndexValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjPolicyAttrIndexValue, owa.len);

  /* return the object value: PolicyAttrIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjPolicyAttrIndexValue,
                           sizeof (objPolicyAttrIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyAttributeStmt_EntryType1
*
* @purpose Get 'EntryType1'
*
* @description [EntryType1]: Policy attribute statement entry type -- this
*              entry type is essential to determine which of the individual
*              object(s) is defined for this policy attribute statement.
*              This object must be created before any other policy 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_EntryType1 (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue = 0;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue = 0;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue = 0;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objEntryTypeValue = 0;
  xLibStr256_t objMatchEntryTypeListStringVal;

  FPOBJ_TRACE_ENTER (bufp);

  memset(objMatchEntryTypeListStringVal, 0x0, sizeof(objMatchEntryTypeListStringVal));

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtEntryTypeGet (L7_UNIT_CURRENT,
                                             keyPolicyIndexValue,
                                             keyPolicyInstIndexValue,
                                             keyPolicyAttrIndexValue,
                                             &objEntryTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* Convert Enum value to String. */
  if (ObjUtilDiffServPolicyClassEnumValToStrGet(objEntryTypeValue, (xLibU8_t *)objMatchEntryTypeListStringVal) != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  /* return the object value: EntryType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objMatchEntryTypeListStringVal,
                           strlen(objMatchEntryTypeListStringVal));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_qosDiffServPolicyAttributeStmt_EntryType1
*
* @purpose Set 'EntryType1'
*
* @description [EntryType1]: Policy attribute statement entry type -- this
*              entry type is essential to determine which of the individual
*              object(s) is defined for this policy attribute statement.
*              This object must be created before any other policy 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_qosDiffServPolicyAttributeStmt_EntryType1 (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objEntryTypeValue = 0;
  xLibU32_t prevEntryTypeValue = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue = 0;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue = 0;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue = 0;
  xLibStr256_t objEntryTypeStr;
  xLibU32_t AttrTypeEnumLst[] = {L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE,
                                 L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE,
                                 L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP,
                                 L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL,
                                 L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2,
                                 L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL,
                                 L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL,
                                 L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR,
                                 L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE,
                                 L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE,
                                 L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE,
                                 L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT};

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: EntryType */
  memset(objEntryTypeStr, 0x00, sizeof(objEntryTypeStr));
  owa.len = sizeof(objEntryTypeStr);
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_EntryType1,
                           (xLibU8_t *) objEntryTypeStr, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objEntryTypeStr, owa.len);
  /* Get the Array index of the string from the List of Strings. */
  if (ObjUtilDiffServPolicyClsDefStrToIndexValGet(objEntryTypeStr, &objEntryTypeValue) != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  /* Convert Index value to corresponding Enum value. */
  objEntryTypeValue = AttrTypeEnumLst[objEntryTypeValue];         

  FPOBJ_TRACE_VALUE (bufp, &objEntryTypeValue, sizeof(objEntryTypeValue));

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);
  owa.l7rc = 
  usmDbDiffServPolicyAttrStmtEntryTypeGet(L7_UNIT_CURRENT,keyPolicyIndexValue,keyPolicyInstIndexValue,
  keyPolicyAttrIndexValue,&prevEntryTypeValue);
  if(prevEntryTypeValue == objEntryTypeValue)
  {
    owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  /* set the value in application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtEntryTypeSet (L7_UNIT_CURRENT,
                                             keyPolicyIndexValue,
                                             keyPolicyInstIndexValue,
                                             keyPolicyAttrIndexValue,
                                             objEntryTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServPolicyAttrDelete(L7_UNIT_CURRENT, keyPolicyIndexValue, keyPolicyInstIndexValue, keyPolicyAttrIndexValue);
    owa.rc = XLIBRC_DIFFSERV_POLICY_ATTRIBUTE_ENTRYTYPE_CONFLICT;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjList_qosDiffServPolicyAttributeStmt_EntryType1
*
* @purpose Get 'EntryType1'
*
* @description [EntryType]: Policy attribute statement entry type -- this
*              entry type is essential to determine which of the individual
*              object(s) is defined for this policy attribute statement.
*              This object must be created before any other policy 
*
* @return
*******************************************************************************/
 xLibRC_t fpObjList_qosDiffServPolicyAttributeStmt_EntryType1 (void *wap,
                                                             void *bufp)
 {
   fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
   xLibU32_t keyPolicyIndexValue =0;
   fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
   xLibU32_t keyPolicyInstIndexValue =0;
   /*fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));*/
   xLibU32_t keyPolicyAttrIndexValue =0;
   fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
   xLibStr256_t objEntryTypeStr;
   xLibU32_t objEntryTypeValue =0, nextPolicyId =0, nextPolicyInstId =0, nextPolicyAttrId =0;
   xLibBool_t ret =XLIB_FALSE, attribCfg =XLIB_FALSE;
   xLibU32_t AttrTypeEnumLst[] = {L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE,
                                  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE,
                                  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP,
                                  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL,
                                  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2,
                                  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL,
                                  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL,
                                  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR,
                                  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE,
                                  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE,
                                  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE,
                                  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT};
    
   FPOBJ_TRACE_ENTER (bufp);
 
   /* retrieve key: PolicyIndex */
   kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                            (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
   if (kwa1.rc != XLIBRC_SUCCESS)
   {
     kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, kwa1);
     return kwa1.rc;
   }
   FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);
 
   /* retrieve key: PolicyInstIndex */
   kwa2.rc =
     xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                    (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
   if (kwa2.rc != XLIBRC_SUCCESS)
   {
     kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, kwa2);
     return kwa2.rc;
   }
   FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

   /* This is used to check if there are no policy attributes 
    * configured for the particular policy. */
   if (usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT, keyPolicyIndexValue, keyPolicyInstIndexValue,
                                      keyPolicyAttrIndexValue, &nextPolicyId, &nextPolicyInstId,
                                      &nextPolicyAttrId) == L7_SUCCESS &&
       nextPolicyId == keyPolicyIndexValue &&
       nextPolicyInstId == keyPolicyInstIndexValue)
   {
     attribCfg = XLIB_TRUE;
   }
   else /* No Attributes Configured. */
   {
     attribCfg = XLIB_FALSE;
   }

 
   memset(objEntryTypeStr, 0x00, sizeof(objEntryTypeStr));
   owa.len = sizeof(objEntryTypeStr);
   owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_EntryType1,
                            (xLibU8_t *) objEntryTypeStr, &owa.len);
   if (owa.rc != XLIBRC_SUCCESS)
   { 
     /* If no policy attributes configured the current policy and if the blank option in the 
      * selector-Filtor is found then in this case populate " " initially. */
     if (attribCfg == XLIB_FALSE)
     {
       /* No Attributes Configured. */
       osapiStrncpy(objEntryTypeStr, "None",strlen("None"));
       FPOBJ_TRACE_EXIT (bufp, owa);
     }
   }

   if ((attribCfg==XLIB_TRUE) || (owa.rc==XLIBRC_SUCCESS))
   {
       /* Get the Array index of the string from the List of Strings. */
       if (owa.rc != XLIBRC_SUCCESS)
       {
         objEntryTypeValue = 0;
       }
       else if (ObjUtilDiffServPolicyClsDefStrToIndexValGet(objEntryTypeStr, &objEntryTypeValue) != XLIBRC_SUCCESS)
       {
         owa.rc = XLIBRC_ENDOF_TABLE ;
         FPOBJ_TRACE_EXIT (bufp, owa);
         return owa.rc;
       }

       while (ret == XLIB_FALSE)
       {
         /* Increment the Array Index. */
         objEntryTypeValue++;
         
         /* Check for Max Index reached. */
         if (objEntryTypeValue >= 12)
         {
           owa.rc = XLIBRC_ENDOF_TABLE ; /* L7_TRUE; */
           FPOBJ_TRACE_EXIT (bufp, owa);
           return owa.rc;                /*  break; */
         }
         
         /* Below we pass the Enum value of the corresponding Array Index */
         ret = resolveconflict(AttrTypeEnumLst[objEntryTypeValue],keyPolicyIndexValue,
                               keyPolicyInstIndexValue, attribCfg);
       }/*End--of-While-Loop*/

       if (ret == XLIB_TRUE)
       {
         /* Convert Index value to corresponding Enum value. */
         objEntryTypeValue = AttrTypeEnumLst[objEntryTypeValue];
         /* Fetch the corresponding String. */
         memset(objEntryTypeStr, 0x00, sizeof(objEntryTypeStr));
         if (ObjUtilDiffServPolicyClassEnumValToStrGet(objEntryTypeValue, objEntryTypeStr) != XLIBRC_SUCCESS)
         {
           owa.rc = XLIBRC_ENDOF_TABLE;
           FPOBJ_TRACE_EXIT (bufp, owa);
           return owa.rc;
         }     
       }
   }   
   /* return the object value: EntryType */
   owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objEntryTypeStr,
                            strlen (objEntryTypeStr));
   FPOBJ_TRACE_EXIT (bufp, owa);
   return owa.rc;
 }


/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyAttributeStmt_EntryType
*
* @purpose Get 'EntryType'
*
* @description [EntryType]: Policy attribute statement entry type -- this
*              entry type is essential to determine which of the individual
*              object(s) is defined for this policy attribute statement.
*              This object must be created before any other policy 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_EntryType (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objEntryTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtEntryTypeGet (L7_UNIT_CURRENT,
                                             keyPolicyIndexValue,
                                             keyPolicyInstIndexValue,
                                             keyPolicyAttrIndexValue,
                                             &objEntryTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: EntryType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objEntryTypeValue,
                           sizeof (objEntryTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_qosDiffServPolicyAttributeStmt_EntryType
*
* @purpose Set 'EntryType'
*
* @description [EntryType]: Policy attribute statement entry type -- this
*              entry type is essential to determine which of the individual
*              object(s) is defined for this policy attribute statement.
*              This object must be created before any other policy 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_qosDiffServPolicyAttributeStmt_EntryType (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objEntryTypeValue;
	xLibU32_t prevEntryTypeValue = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: EntryType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objEntryTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objEntryTypeValue, owa.len);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);
  owa.l7rc = 
  usmDbDiffServPolicyAttrStmtEntryTypeGet(L7_UNIT_CURRENT,keyPolicyIndexValue,keyPolicyInstIndexValue,
  keyPolicyAttrIndexValue,&prevEntryTypeValue);
	if(prevEntryTypeValue == objEntryTypeValue)
	{
	  owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
	}
  /* set the value in application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtEntryTypeSet (L7_UNIT_CURRENT,
                                             keyPolicyIndexValue,
                                             keyPolicyInstIndexValue,
                                             keyPolicyAttrIndexValue,
                                             objEntryTypeValue);

  if (objEntryTypeValue == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2)
  {
    owa.l7rc =
        usmDbDiffServPolicyAttrStmtMarkCosAsCos2Set(L7_UNIT_CURRENT,
                                                 keyPolicyIndexValue, 
                                                 keyPolicyInstIndexValue, 
                                                 keyPolicyAttrIndexValue,
                                                 L7_TRUE);
  }                                   

  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServPolicyAttrDelete(L7_UNIT_CURRENT, keyPolicyIndexValue, keyPolicyInstIndexValue, keyPolicyAttrIndexValue);
    owa.rc = XLIBRC_DIFFSERV_POLICY_ATTRIBUTE_ENTRYTYPE_CONFLICT;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyAttributeStmt_MarkCosVal
*
* @purpose Get 'MarkCosVal'
*
* @description [MarkCosVal]: Three-bit user priority field value in the 802.1Q
*              tag header of a tagged Ethernet frame, marked as part
*              of the inbound policy for a class instance. For frames containing
*              a double VLAN tag, this field is located in the 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_MarkCosVal (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMarkCosValValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtMarkCosValGet (L7_UNIT_CURRENT,
                                              keyPolicyIndexValue,
                                              keyPolicyInstIndexValue,
                                              keyPolicyAttrIndexValue,
                                              &objMarkCosValValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MarkCosVal */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMarkCosValValue,
                           sizeof (objMarkCosValValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_qosDiffServPolicyAttributeStmt_MarkCosVal
*
* @purpose Set 'MarkCosVal'
*
* @description [MarkCosVal]: Three-bit user priority field value in the 802.1Q
*              tag header of a tagged Ethernet frame, marked as part
*              of the inbound policy for a class instance. For frames containing
*              a double VLAN tag, this field is located in the 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_qosDiffServPolicyAttributeStmt_MarkCosVal (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMarkCosValValue;
  xLibU32_t prevCosValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType;
  L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_t policyType = L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_NONE;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MarkCosVal */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMarkCosValValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMarkCosValValue, owa.len);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the policy type */
  if (usmDbDiffServPolicyTypeGet(L7_UNIT_CURRENT, keyPolicyIndexValue, &policyType) != L7_SUCCESS)
  {
    usmDbDiffServPolicyAttrDelete(L7_UNIT_CURRENT, keyPolicyIndexValue, keyPolicyInstIndexValue, keyPolicyAttrIndexValue);
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

/*check if the entryKey set is valid for thie entry type */
  if(usmDbDiffServPolicyAttrStmtEntryTypeGet(L7_UNIT_CURRENT,keyPolicyIndexValue,keyPolicyInstIndexValue,
        keyPolicyAttrIndexValue,&entryType)!=L7_SUCCESS || entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL)
  {

    usmDbDiffServPolicyAttrDelete(L7_UNIT_CURRENT, keyPolicyIndexValue, keyPolicyInstIndexValue, keyPolicyAttrIndexValue);
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;

  }
  owa.l7rc = 
    usmDbDiffServPolicyAttrStmtMarkCosValGet(L7_UNIT_CURRENT,keyPolicyIndexValue,keyPolicyInstIndexValue,
        keyPolicyAttrIndexValue,&prevCosValue);

  if(objMarkCosValValue == prevCosValue)
  {
    owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  /* set the value in application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtMarkCosValSet (L7_UNIT_CURRENT,
                                              keyPolicyIndexValue,
                                              keyPolicyInstIndexValue,
                                              keyPolicyAttrIndexValue,
                                              objMarkCosValValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServPolicyAttrDelete(L7_UNIT_CURRENT, keyPolicyIndexValue, keyPolicyInstIndexValue, keyPolicyAttrIndexValue);
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyAttributeStmt_MarkIpDscpVal
*
* @purpose Get 'MarkIpDscpVal'
*
* @description [MarkIpDscpVal]: Specified IP DSCP value to mark in all inbound
*              packets belonging to the class-instance. This attribute
*              is only valid if the value of agentDiffServPolicyAttrStmtEntryType
*              is set to markIpDscpVal(4). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_MarkIpDscpVal (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMarkIpDscpValValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtMarkIpDscpValGet (L7_UNIT_CURRENT,
                                                 keyPolicyIndexValue,
                                                 keyPolicyInstIndexValue,
                                                 keyPolicyAttrIndexValue,
                                                 &objMarkIpDscpValValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MarkIpDscpVal */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMarkIpDscpValValue,
                           sizeof (objMarkIpDscpValValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_qosDiffServPolicyAttributeStmt_MarkIpDscpVal
*
* @purpose Set 'MarkIpDscpVal'
*
* @description [MarkIpDscpVal]: Specified IP DSCP value to mark in all inbound
*              packets belonging to the class-instance. This attribute
*              is only valid if the value of agentDiffServPolicyAttrStmtEntryType
*              is set to markIpDscpVal(4). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_qosDiffServPolicyAttributeStmt_MarkIpDscpVal (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMarkIpDscpValValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType; 
  xLibU32_t prevDscpValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MarkIpDscpVal */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objMarkIpDscpValValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMarkIpDscpValValue, owa.len);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /*check if the entryKey set is valid for thie entry type */
  if(usmDbDiffServPolicyAttrStmtEntryTypeGet(L7_UNIT_CURRENT,keyPolicyIndexValue,keyPolicyInstIndexValue,	
        keyPolicyAttrIndexValue,&entryType)!=L7_SUCCESS || entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL)
  { 
    usmDbDiffServPolicyAttrDelete(L7_UNIT_CURRENT, keyPolicyIndexValue, keyPolicyInstIndexValue, keyPolicyAttrIndexValue);      
    owa.rc = XLIBRC_FAILURE;    
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;  
  }
  owa.l7rc = usmDbDiffServPolicyAttrStmtMarkIpDscpValGet(L7_UNIT_CURRENT,keyPolicyIndexValue,
      keyPolicyInstIndexValue,  keyPolicyAttrIndexValue,&prevDscpValue);  

  if(objMarkIpDscpValValue == prevDscpValue)
  {     
    owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */    
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  } 
  /* set the value in application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtMarkIpDscpValSet (L7_UNIT_CURRENT,
                                                 keyPolicyIndexValue,
                                                 keyPolicyInstIndexValue,
                                                 keyPolicyAttrIndexValue,
                                                 objMarkIpDscpValValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    usmDbDiffServPolicyAttrDelete(L7_UNIT_CURRENT, keyPolicyIndexValue, keyPolicyInstIndexValue, keyPolicyAttrIndexValue);
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyAttributeStmt_MarkIpPrecedenceVal
*
* @purpose Get 'MarkIpPrecedenceVal'
*
* @description [MarkIpPrecedenceVal]: Specified IP Precedence value to mark
*              in all inbound packets belonging to the class-instance. This
*              attribute is only valid if the value of agentDiffServPolicyAttrStmtEntryType
*              is set to markIpPrecedenceVal(5). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_MarkIpPrecedenceVal (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMarkIpPrecedenceValValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtMarkIpPrecedenceValGet (L7_UNIT_CURRENT,
                                                       keyPolicyIndexValue,
                                                       keyPolicyInstIndexValue,
                                                       keyPolicyAttrIndexValue,
                                                       &objMarkIpPrecedenceValValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MarkIpPrecedenceVal */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMarkIpPrecedenceValValue,
                           sizeof (objMarkIpPrecedenceValValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_qosDiffServPolicyAttributeStmt_MarkIpPrecedenceVal
*
* @purpose Set 'MarkIpPrecedenceVal'
*
* @description [MarkIpPrecedenceVal]: Specified IP Precedence value to mark
*              in all inbound packets belonging to the class-instance. This
*              attribute is only valid if the value of agentDiffServPolicyAttrStmtEntryType
*              is set to markIpPrecedenceVal(5). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_qosDiffServPolicyAttributeStmt_MarkIpPrecedenceVal (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMarkIpPrecedenceValValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType;
  xLibU32_t prevPrecValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MarkIpPrecedenceVal */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objMarkIpPrecedenceValValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMarkIpPrecedenceValValue, owa.len);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /*check if the entryKey set is valid for thie entry type */
  if(usmDbDiffServPolicyAttrStmtEntryTypeGet(L7_UNIT_CURRENT,keyPolicyIndexValue,keyPolicyInstIndexValue,	keyPolicyAttrIndexValue,&entryType)!=L7_SUCCESS
      || entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL)
  { 

    usmDbDiffServPolicyAttrDelete(L7_UNIT_CURRENT,keyPolicyIndexValue, keyPolicyInstIndexValue, keyPolicyAttrIndexValue);      
    owa.rc = XLIBRC_FAILURE;  
    FPOBJ_TRACE_EXIT (bufp, owa);   
    return owa.rc ;

  }
  owa.l7rc = usmDbDiffServPolicyAttrStmtMarkIpPrecedenceValGet(L7_UNIT_CURRENT,keyPolicyIndexValue,
      keyPolicyInstIndexValue,  keyPolicyAttrIndexValue,&prevPrecValue);	

  if(objMarkIpPrecedenceValValue== prevPrecValue)
  {     
    owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */    
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }	
  /* set the value in application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtMarkIpPrecedenceValSet (L7_UNIT_CURRENT,
                                                       keyPolicyIndexValue,
                                                       keyPolicyInstIndexValue,
                                                       keyPolicyAttrIndexValue,
                                                       objMarkIpPrecedenceValValue);
  if (owa.l7rc != L7_SUCCESS)
  {
  usmDbDiffServPolicyAttrDelete(L7_UNIT_CURRENT, keyPolicyIndexValue, keyPolicyInstIndexValue, keyPolicyAttrIndexValue);
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyAttributeStmt_PoliceConformAct
*
* @purpose Get 'PoliceConformAct'
*
* @description [PoliceConformAct]: Policing conform action attribute statement
*              value -- determines the action taken on conforming traffic
*              for the policing style (simple,singlerate,tworate) currently
*              configured for the specified class instance within the
*              policy. The d 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_PoliceConformAct (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceConformActValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceConformActGet (L7_UNIT_CURRENT,
                                                    keyPolicyIndexValue,
                                                    keyPolicyInstIndexValue,
                                                    keyPolicyAttrIndexValue,
                                                    &objPoliceConformActValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PoliceConformAct */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPoliceConformActValue,
                           sizeof (objPoliceConformActValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_qosDiffServPolicyAttributeStmt_PoliceConformAct
*
* @purpose Set 'PoliceConformAct'
*
* @description [PoliceConformAct]: Policing conform action attribute statement
*              value -- determines the action taken on conforming traffic
*              for the policing style (simple,singlerate,tworate) currently
*              configured for the specified class instance within the
*              policy. The d 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_qosDiffServPolicyAttributeStmt_PoliceConformAct (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceConformActValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PoliceConformAct */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPoliceConformActValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPoliceConformActValue, owa.len);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceConformActSet (L7_UNIT_CURRENT,
                                                    keyPolicyIndexValue,
                                                    keyPolicyInstIndexValue,
                                                    keyPolicyAttrIndexValue,
                                                    objPoliceConformActValue);
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
* @function fpObjGet_qosDiffServPolicyAttributeStmt_PoliceConformVal
*
* @purpose Get 'PoliceConformVal'
*
* @description [PoliceConformVal]: Policing conform value attribute statement
*              -- used to mark conforming packets when the conform action
*              is one of the following: markdscp(2) - mark IP DSCP field
*              markprec(3) - mark IP Precedence field markcos(5) 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_PoliceConformVal (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceConformValValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceConformValGet (L7_UNIT_CURRENT,
                                                    keyPolicyIndexValue,
                                                    keyPolicyInstIndexValue,
                                                    keyPolicyAttrIndexValue,
                                                    &objPoliceConformValValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PoliceConformVal */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPoliceConformValValue,
                           sizeof (objPoliceConformValValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_qosDiffServPolicyAttributeStmt_PoliceConformVal
*
* @purpose Set 'PoliceConformVal'
*
* @description [PoliceConformVal]: Policing conform value attribute statement
*              -- used to mark conforming packets when the conform action
*              is one of the following: markdscp(2) - mark IP DSCP field
*              markprec(3) - mark IP Precedence field markcos(5) 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_qosDiffServPolicyAttributeStmt_PoliceConformVal (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceConformValValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PoliceConformVal */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPoliceConformValValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPoliceConformValValue, owa.len);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceConformValSet (L7_UNIT_CURRENT,
                                                    keyPolicyIndexValue,
                                                    keyPolicyInstIndexValue,
                                                    keyPolicyAttrIndexValue,
                                                    objPoliceConformValValue);
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
* @function fpObjGet_qosDiffServPolicyAttributeStmt_PoliceExceedAct
*
* @purpose Get 'PoliceExceedAct'
*
* @description [PoliceExceedAct]: Policing exceed action attribute statement
*              value -- determines the action taken on excess traffic for
*              the policing style (singlerate, tworate) currently configured
*              for the specified class instance within the 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_PoliceExceedAct (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceExceedActValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceExceedActGet (L7_UNIT_CURRENT,
                                                   keyPolicyIndexValue,
                                                   keyPolicyInstIndexValue,
                                                   keyPolicyAttrIndexValue,
                                                   &objPoliceExceedActValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PoliceExceedAct */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPoliceExceedActValue,
                           sizeof (objPoliceExceedActValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_qosDiffServPolicyAttributeStmt_PoliceExceedAct
*
* @purpose Set 'PoliceExceedAct'
*
* @description [PoliceExceedAct]: Policing exceed action attribute statement
*              value -- determines the action taken on excess traffic for
*              the policing style (singlerate, tworate) currently configured
*              for the specified class instance within the 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_qosDiffServPolicyAttributeStmt_PoliceExceedAct (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceExceedActValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PoliceExceedAct */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPoliceExceedActValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPoliceExceedActValue, owa.len);


  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceExceedActSet (L7_UNIT_CURRENT,
                                                   keyPolicyIndexValue,
                                                   keyPolicyInstIndexValue,
                                                   keyPolicyAttrIndexValue,
                                                   objPoliceExceedActValue);
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
* @function fpObjGet_qosDiffServPolicyAttributeStmt_PoliceExceedVal
*
* @purpose Get 'PoliceExceedVal'
*
* @description [PoliceExceedVal]: Policing exceed value attribute statement
*              -- used to mark excess packets when the exceed action is
*              one of the following: markdscp(2) - mark IP DSCP field markprec(3)
*              - mark IP Precedence field markcos(5) - mar 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_PoliceExceedVal (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceExceedValValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceExceedValGet (L7_UNIT_CURRENT,
                                                   keyPolicyIndexValue,
                                                   keyPolicyInstIndexValue,
                                                   keyPolicyAttrIndexValue,
                                                   &objPoliceExceedValValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PoliceExceedVal */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPoliceExceedValValue,
                           sizeof (objPoliceExceedValValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_qosDiffServPolicyAttributeStmt_PoliceExceedVal
*
* @purpose Set 'PoliceExceedVal'
*
* @description [PoliceExceedVal]: Policing exceed value attribute statement
*              -- used to mark excess packets when the exceed action is
*              one of the following: markdscp(2) - mark IP DSCP field markprec(3)
*              - mark IP Precedence field markcos(5) - mar 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_qosDiffServPolicyAttributeStmt_PoliceExceedVal (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceExceedValValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PoliceExceedVal */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPoliceExceedValValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPoliceExceedValValue, owa.len);


  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceExceedValSet (L7_UNIT_CURRENT,
                                                   keyPolicyIndexValue,
                                                   keyPolicyInstIndexValue,
                                                   keyPolicyAttrIndexValue,
                                                   objPoliceExceedValValue);
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
* @function fpObjGet_qosDiffServPolicyAttributeStmt_PoliceNonconformAct
*
* @purpose Get 'PoliceNonconformAct'
*
* @description [PoliceNonconformAct]: Policing non-conform action attribute
*              statement value -- determines the action taken on nonconforming
*              traffic for the policing style (simple, singlerate,tworate)
*              currently configured for the specified class inst 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_PoliceNonconformAct (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceNonconformActValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceNonconformActGet (L7_UNIT_CURRENT,
                                                       keyPolicyIndexValue,
                                                       keyPolicyInstIndexValue,
                                                       keyPolicyAttrIndexValue,
                                                       &objPoliceNonconformActValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PoliceNonconformAct */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPoliceNonconformActValue,
                           sizeof (objPoliceNonconformActValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_qosDiffServPolicyAttributeStmt_PoliceNonconformAct
*
* @purpose Set 'PoliceNonconformAct'
*
* @description [PoliceNonconformAct]: Policing non-conform action attribute
*              statement value -- determines the action taken on nonconforming
*              traffic for the policing style (simple, singlerate,tworate)
*              currently configured for the specified class inst 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_qosDiffServPolicyAttributeStmt_PoliceNonconformAct (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceNonconformActValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PoliceNonconformAct */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPoliceNonconformActValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPoliceNonconformActValue, owa.len);


  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceNonconformActSet (L7_UNIT_CURRENT,
                                                       keyPolicyIndexValue,
                                                       keyPolicyInstIndexValue,
                                                       keyPolicyAttrIndexValue,
                                                       objPoliceNonconformActValue);
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
* @function fpObjGet_qosDiffServPolicyAttributeStmt_PoliceNonconformVal
*
* @purpose Get 'PoliceNonconformVal'
*
* @description [PoliceNonconformVal]: Policing non-conform value attribute
*              statement -- used to mark nonconforming packets when the nonconform
*              action is one of the following: markdscp(2) - mark
*              IP DSCP field markprec(3) - mark IP Precedence field m 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_PoliceNonconformVal (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceNonconformValValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceNonconformValGet (L7_UNIT_CURRENT,
                                                       keyPolicyIndexValue,
                                                       keyPolicyInstIndexValue,
                                                       keyPolicyAttrIndexValue,
                                                       &objPoliceNonconformValValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PoliceNonconformVal */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPoliceNonconformValValue,
                           sizeof (objPoliceNonconformValValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_qosDiffServPolicyAttributeStmt_PoliceNonconformVal
*
* @purpose Set 'PoliceNonconformVal'
*
* @description [PoliceNonconformVal]: Policing non-conform value attribute
*              statement -- used to mark nonconforming packets when the nonconform
*              action is one of the following: markdscp(2) - mark
*              IP DSCP field markprec(3) - mark IP Precedence field m 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_qosDiffServPolicyAttributeStmt_PoliceNonconformVal (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceNonconformValValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PoliceNonconformVal */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPoliceNonconformValValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPoliceNonconformValValue, owa.len);


  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceNonconformValSet (L7_UNIT_CURRENT,
                                                       keyPolicyIndexValue,
                                                       keyPolicyInstIndexValue,
                                                       keyPolicyAttrIndexValue,
                                                       objPoliceNonconformValValue);
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
* @function fpObjGet_qosDiffServPolicyAttributeStmt_PoliceSimpleCrate
*
* @purpose Get 'PoliceSimpleCrate'
*
* @description [PoliceSimpleCrate]: Simple policing committed rate attribute
*              statement value, specified in kbps. This attribute is only
*              valid if the value of agentDiffServPolicyAttrStmtEntryType
*              is set to policeSimple(6). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_PoliceSimpleCrate (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceSimpleCrateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceSimpleCrateGet (L7_UNIT_CURRENT,
                                                     keyPolicyIndexValue,
                                                     keyPolicyInstIndexValue,
                                                     keyPolicyAttrIndexValue,
                                                     &objPoliceSimpleCrateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PoliceSimpleCrate */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPoliceSimpleCrateValue,
                           sizeof (objPoliceSimpleCrateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_qosDiffServPolicyAttributeStmt_PoliceSimpleCrate
*
* @purpose Set 'PoliceSimpleCrate'
*
* @description [PoliceSimpleCrate]: Simple policing committed rate attribute
*              statement value, specified in kbps. This attribute is only
*              valid if the value of agentDiffServPolicyAttrStmtEntryType
*              is set to policeSimple(6). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_qosDiffServPolicyAttributeStmt_PoliceSimpleCrate (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceSimpleCrateValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PoliceSimpleCrate */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPoliceSimpleCrateValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPoliceSimpleCrateValue, owa.len);

  /* retrieve row status object value */
  

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceSimpleCrateSet (L7_UNIT_CURRENT,
                                                     keyPolicyIndexValue,
                                                     keyPolicyInstIndexValue,
                                                     keyPolicyAttrIndexValue,
                                                     objPoliceSimpleCrateValue);
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
* @function fpObjGet_qosDiffServPolicyAttributeStmt_PoliceSimpleCburst
*
* @purpose Get 'PoliceSimpleCburst'
*
* @description [PoliceSimpleCburst]: Simple policing committed burst size
*              attribute statement value, specified in Kbytes. This attribute
*              is only valid if the value of agentDiffServPolicyAttrStmtEntryType
*              is set to policeSimple(6). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_PoliceSimpleCburst (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceSimpleCburstValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceSimpleCburstGet (L7_UNIT_CURRENT,
                                                      keyPolicyIndexValue,
                                                      keyPolicyInstIndexValue,
                                                      keyPolicyAttrIndexValue,
                                                      &objPoliceSimpleCburstValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PoliceSimpleCburst */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPoliceSimpleCburstValue,
                           sizeof (objPoliceSimpleCburstValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_qosDiffServPolicyAttributeStmt_PoliceSimpleCburst
*
* @purpose Set 'PoliceSimpleCburst'
*
* @description [PoliceSimpleCburst]: Simple policing committed burst size
*              attribute statement value, specified in Kbytes. This attribute
*              is only valid if the value of agentDiffServPolicyAttrStmtEntryType
*              is set to policeSimple(6). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_qosDiffServPolicyAttributeStmt_PoliceSimpleCburst (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceSimpleCburstValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PoliceSimpleCburst */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPoliceSimpleCburstValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPoliceSimpleCburstValue, owa.len);


  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceSimpleCburstSet (L7_UNIT_CURRENT,
                                                      keyPolicyIndexValue,
                                                      keyPolicyInstIndexValue,
                                                      keyPolicyAttrIndexValue,
                                                      objPoliceSimpleCburstValue);
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
* @function fpObjGet_qosDiffServPolicyAttributeStmt_PoliceSinglerateCrate
*
* @purpose Get 'PoliceSinglerateCrate'
*
* @description [PoliceSinglerateCrate]: Single-rate policing committed rate
*              attribute statement value, specified in kbps. This attribute
*              is only valid if the value of agentDiffServPolicyAttrStmtEntryType
*              is set to policeSinglerate(7). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_PoliceSinglerateCrate (void
                                                                        *wap,
                                                                        void
                                                                        *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceSinglerateCrateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceSinglerateCrateGet (L7_UNIT_CURRENT,
                                                         keyPolicyIndexValue,
                                                         keyPolicyInstIndexValue,
                                                         keyPolicyAttrIndexValue,
                                                         &objPoliceSinglerateCrateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PoliceSinglerateCrate */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPoliceSinglerateCrateValue,
                           sizeof (objPoliceSinglerateCrateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_qosDiffServPolicyAttributeStmt_PoliceSinglerateCrate
*
* @purpose Set 'PoliceSinglerateCrate'
*
* @description [PoliceSinglerateCrate]: Single-rate policing committed rate
*              attribute statement value, specified in kbps. This attribute
*              is only valid if the value of agentDiffServPolicyAttrStmtEntryType
*              is set to policeSinglerate(7). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_qosDiffServPolicyAttributeStmt_PoliceSinglerateCrate (void
                                                                        *wap,
                                                                        void
                                                                        *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceSinglerateCrateValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PoliceSinglerateCrate */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPoliceSinglerateCrateValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPoliceSinglerateCrateValue, owa.len);


  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceSinglerateCrateSet (L7_UNIT_CURRENT,
                                                         keyPolicyIndexValue,
                                                         keyPolicyInstIndexValue,
                                                         keyPolicyAttrIndexValue,
                                                         objPoliceSinglerateCrateValue);
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
* @function fpObjGet_qosDiffServPolicyAttributeStmt_PoliceSinglerateCburst
*
* @purpose Get 'PoliceSinglerateCburst'
*
* @description [PoliceSinglerateCburst]: Single-rate policing committed burst
*              size attribute statement value, specified in Kbytes. This
*              attribute is only valid if the value of agentDiffServPolicyAttrStmtEntryType
*              is set to policeSinglerate(7). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_PoliceSinglerateCburst (void
                                                                         *wap,
                                                                         void
                                                                         *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceSinglerateCburstValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceSinglerateCburstGet (L7_UNIT_CURRENT,
                                                          keyPolicyIndexValue,
                                                          keyPolicyInstIndexValue,
                                                          keyPolicyAttrIndexValue,
                                                          &objPoliceSinglerateCburstValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PoliceSinglerateCburst */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPoliceSinglerateCburstValue,
                           sizeof (objPoliceSinglerateCburstValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_qosDiffServPolicyAttributeStmt_PoliceSinglerateCburst
*
* @purpose Set 'PoliceSinglerateCburst'
*
* @description [PoliceSinglerateCburst]: Single-rate policing committed burst
*              size attribute statement value, specified in Kbytes. This
*              attribute is only valid if the value of agentDiffServPolicyAttrStmtEntryType
*              is set to policeSinglerate(7). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_qosDiffServPolicyAttributeStmt_PoliceSinglerateCburst (void
                                                                         *wap,
                                                                         void
                                                                         *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceSinglerateCburstValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PoliceSinglerateCburst */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPoliceSinglerateCburstValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPoliceSinglerateCburstValue, owa.len);


  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceSinglerateCburstSet (L7_UNIT_CURRENT,
                                                          keyPolicyIndexValue,
                                                          keyPolicyInstIndexValue,
                                                          keyPolicyAttrIndexValue,
                                                          objPoliceSinglerateCburstValue);
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
* @function fpObjGet_qosDiffServPolicyAttributeStmt_PoliceSinglerateEburst
*
* @purpose Get 'PoliceSinglerateEburst'
*
* @description [PoliceSinglerateEburst]: Single-rate policing excess burst
*              size attribute statement value, specified in Kbytes. This
*              attribute is only valid if the value of agentDiffServPolicyAttrStmtEntryType
*              is set to policeSinglerate(7). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_PoliceSinglerateEburst (void
                                                                         *wap,
                                                                         void
                                                                         *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceSinglerateEburstValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceSinglerateEburstGet (L7_UNIT_CURRENT,
                                                          keyPolicyIndexValue,
                                                          keyPolicyInstIndexValue,
                                                          keyPolicyAttrIndexValue,
                                                          &objPoliceSinglerateEburstValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PoliceSinglerateEburst */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPoliceSinglerateEburstValue,
                           sizeof (objPoliceSinglerateEburstValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_qosDiffServPolicyAttributeStmt_PoliceSinglerateEburst
*
* @purpose Set 'PoliceSinglerateEburst'
*
* @description [PoliceSinglerateEburst]: Single-rate policing excess burst
*              size attribute statement value, specified in Kbytes. This
*              attribute is only valid if the value of agentDiffServPolicyAttrStmtEntryType
*              is set to policeSinglerate(7). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_qosDiffServPolicyAttributeStmt_PoliceSinglerateEburst (void
                                                                         *wap,
                                                                         void
                                                                         *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceSinglerateEburstValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PoliceSinglerateEburst */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPoliceSinglerateEburstValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPoliceSinglerateEburstValue, owa.len);


  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceSinglerateEburstSet (L7_UNIT_CURRENT,
                                                          keyPolicyIndexValue,
                                                          keyPolicyInstIndexValue,
                                                          keyPolicyAttrIndexValue,
                                                          objPoliceSinglerateEburstValue);
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
* @function fpObjGet_qosDiffServPolicyAttributeStmt_PoliceTworateCrate
*
* @purpose Get 'PoliceTworateCrate'
*
* @description [PoliceTworateCrate]: Two-rate policing committed rate attribute
*              statement value, specified in kbps. This attribute is
*              only valid if the value of agentDiffServPolicyAttrStmtEntryType
*              is set to policeTworate(8). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_PoliceTworateCrate (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceTworateCrateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceTworateCrateGet (L7_UNIT_CURRENT,
                                                      keyPolicyIndexValue,
                                                      keyPolicyInstIndexValue,
                                                      keyPolicyAttrIndexValue,
                                                      &objPoliceTworateCrateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PoliceTworateCrate */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPoliceTworateCrateValue,
                           sizeof (objPoliceTworateCrateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_qosDiffServPolicyAttributeStmt_PoliceTworateCrate
*
* @purpose Set 'PoliceTworateCrate'
*
* @description [PoliceTworateCrate]: Two-rate policing committed rate attribute
*              statement value, specified in kbps. This attribute is
*              only valid if the value of agentDiffServPolicyAttrStmtEntryType
*              is set to policeTworate(8). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_qosDiffServPolicyAttributeStmt_PoliceTworateCrate (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceTworateCrateValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PoliceTworateCrate */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPoliceTworateCrateValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPoliceTworateCrateValue, owa.len);


  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceTworateCrateSet (L7_UNIT_CURRENT,
                                                      keyPolicyIndexValue,
                                                      keyPolicyInstIndexValue,
                                                      keyPolicyAttrIndexValue,
                                                      objPoliceTworateCrateValue);
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
* @function fpObjGet_qosDiffServPolicyAttributeStmt_PoliceTworateCburst
*
* @purpose Get 'PoliceTworateCburst'
*
* @description [PoliceTworateCburst]: Two-rate policing committed burst size
*              attribute statement value, specified in Kbytes. This attribute
*              is only valid if the value of agentDiffServPolicyAttrStmtEntryType
*              is set to policeTworate(8). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_PoliceTworateCburst (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceTworateCburstValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceTworateCburstGet (L7_UNIT_CURRENT,
                                                       keyPolicyIndexValue,
                                                       keyPolicyInstIndexValue,
                                                       keyPolicyAttrIndexValue,
                                                       &objPoliceTworateCburstValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PoliceTworateCburst */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPoliceTworateCburstValue,
                           sizeof (objPoliceTworateCburstValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_qosDiffServPolicyAttributeStmt_PoliceTworateCburst
*
* @purpose Set 'PoliceTworateCburst'
*
* @description [PoliceTworateCburst]: Two-rate policing committed burst size
*              attribute statement value, specified in Kbytes. This attribute
*              is only valid if the value of agentDiffServPolicyAttrStmtEntryType
*              is set to policeTworate(8). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_qosDiffServPolicyAttributeStmt_PoliceTworateCburst (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceTworateCburstValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PoliceTworateCburst */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPoliceTworateCburstValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPoliceTworateCburstValue, owa.len);


  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceTworateCburstSet (L7_UNIT_CURRENT,
                                                       keyPolicyIndexValue,
                                                       keyPolicyInstIndexValue,
                                                       keyPolicyAttrIndexValue,
                                                       objPoliceTworateCburstValue);
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
* @function fpObjGet_qosDiffServPolicyAttributeStmt_PoliceTworatePrate
*
* @purpose Get 'PoliceTworatePrate'
*
* @description [PoliceTworatePrate]: Two-rate policing peak rate attribute
*              statement value, specified in kbps. This attribute is only
*              valid if the value of agentDiffServPolicyAttrStmtEntryType
*              is set to policeTworate(8). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_PoliceTworatePrate (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceTworatePrateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceTworatePrateGet (L7_UNIT_CURRENT,
                                                      keyPolicyIndexValue,
                                                      keyPolicyInstIndexValue,
                                                      keyPolicyAttrIndexValue,
                                                      &objPoliceTworatePrateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PoliceTworatePrate */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPoliceTworatePrateValue,
                           sizeof (objPoliceTworatePrateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_qosDiffServPolicyAttributeStmt_PoliceTworatePrate
*
* @purpose Set 'PoliceTworatePrate'
*
* @description [PoliceTworatePrate]: Two-rate policing peak rate attribute
*              statement value, specified in kbps. This attribute is only
*              valid if the value of agentDiffServPolicyAttrStmtEntryType
*              is set to policeTworate(8). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_qosDiffServPolicyAttributeStmt_PoliceTworatePrate (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceTworatePrateValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PoliceTworatePrate */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPoliceTworatePrateValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPoliceTworatePrateValue, owa.len);


  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceTworatePrateSet (L7_UNIT_CURRENT,
                                                      keyPolicyIndexValue,
                                                      keyPolicyInstIndexValue,
                                                      keyPolicyAttrIndexValue,
                                                      objPoliceTworatePrateValue);
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
* @function fpObjGet_qosDiffServPolicyAttributeStmt_PoliceTworatePburst
*
* @purpose Get 'PoliceTworatePburst'
*
* @description [PoliceTworatePburst]: Two-rate policing peak burst size attribute
*              statement value, specified in Kbytes. This attribute
*              is only valid if the value of agentDiffServPolicyAttrStmtEntryType
*              is set to policeTworate(8). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_PoliceTworatePburst (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceTworatePburstValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceTworatePburstGet (L7_UNIT_CURRENT,
                                                       keyPolicyIndexValue,
                                                       keyPolicyInstIndexValue,
                                                       keyPolicyAttrIndexValue,
                                                       &objPoliceTworatePburstValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PoliceTworatePburst */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPoliceTworatePburstValue,
                           sizeof (objPoliceTworatePburstValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_qosDiffServPolicyAttributeStmt_PoliceTworatePburst
*
* @purpose Set 'PoliceTworatePburst'
*
* @description [PoliceTworatePburst]: Two-rate policing peak burst size attribute
*              statement value, specified in Kbytes. This attribute
*              is only valid if the value of agentDiffServPolicyAttrStmtEntryType
*              is set to policeTworate(8). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_qosDiffServPolicyAttributeStmt_PoliceTworatePburst (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceTworatePburstValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PoliceTworatePburst */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPoliceTworatePburstValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPoliceTworatePburstValue, owa.len);


  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceTworatePburstSet (L7_UNIT_CURRENT,
                                                       keyPolicyIndexValue,
                                                       keyPolicyInstIndexValue,
                                                       keyPolicyAttrIndexValue,
                                                       objPoliceTworatePburstValue);
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
* @function fpObjGet_qosDiffServPolicyAttributeStmt_DiffServPolicyAttrStorageType
*
* @purpose Get 'DiffServPolicyAttrStorageType'
*
* @description [DiffServPolicyAttrStorageType]: Storage-type for this conceptual
*              row. 
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_qosDiffServPolicyAttributeStmt_DiffServPolicyAttrStorageType (void
                                                                       *wap,
                                                                       void
                                                                       *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDiffServPolicyAttrStorageTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStorageTypeGet (L7_UNIT_CURRENT, keyPolicyIndexValue,
                                           keyPolicyInstIndexValue,
                                           keyPolicyAttrIndexValue,
                                           &objDiffServPolicyAttrStorageTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DiffServPolicyAttrStorageType */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objDiffServPolicyAttrStorageTypeValue,
                    sizeof (objDiffServPolicyAttrStorageTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_qosDiffServPolicyAttributeStmt_DiffServPolicyAttrStorageType
*
* @purpose Set 'DiffServPolicyAttrStorageType'
*
* @description [DiffServPolicyAttrStorageType]: Storage-type for this conceptual
*              row. 
*
* @return
*******************************************************************************/
xLibRC_t
fpObjSet_qosDiffServPolicyAttributeStmt_DiffServPolicyAttrStorageType (void
                                                                       *wap,
                                                                       void
                                                                       *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDiffServPolicyAttrStorageTypeValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DiffServPolicyAttrStorageType */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDiffServPolicyAttrStorageTypeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDiffServPolicyAttrStorageTypeValue, owa.len);


  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStorageTypeSet (L7_UNIT_CURRENT, keyPolicyIndexValue,
                                           keyPolicyInstIndexValue,
                                           keyPolicyAttrIndexValue,
                                           objDiffServPolicyAttrStorageTypeValue);
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
* @function fpObjGet_qosDiffServPolicyAttributeStmt_DiffServPolicyAttrRowStatus
*
* @purpose Get 'DiffServPolicyAttrRowStatus'
*
* @description [DiffServPolicyAttrRowStatus]: The status of this conceptual
*              row. 
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_qosDiffServPolicyAttributeStmt_DiffServPolicyAttrRowStatus (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDiffServPolicyAttrRowStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrRowStatusGet (L7_UNIT_CURRENT, keyPolicyIndexValue,
                                         keyPolicyInstIndexValue,
                                         keyPolicyAttrIndexValue,
                                         &objDiffServPolicyAttrRowStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DiffServPolicyAttrRowStatus */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objDiffServPolicyAttrRowStatusValue,
                    sizeof (objDiffServPolicyAttrRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_qosDiffServPolicyAttributeStmt_DiffServPolicyAttrRowStatus
*
* @purpose Set 'DiffServPolicyAttrRowStatus'
*
* @description [DiffServPolicyAttrRowStatus]: The status of this conceptual
*              row. 
*
* @return
*******************************************************************************/
xLibRC_t
fpObjSet_qosDiffServPolicyAttributeStmt_DiffServPolicyAttrRowStatus (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDiffServPolicyAttrRowStatusValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  xLibU32_t tempPolicyAttrIndxValue;

  xLibU32_t tempPolicyAttrIndexValue;
  xLibU32_t entryType;
  xLibU32_t found = 0;
	xLibU32_t maxIndex = 0;
	
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t EntryType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DiffServPolicyAttrRowStatus */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDiffServPolicyAttrRowStatusValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDiffServPolicyAttrRowStatusValue, owa.len);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
   keyPolicyAttrIndexValue = 0;
  }

  owa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_EntryType,(xLibU8_t *) & entryType, &owa1.len); 
  if (owa1.rc != XLIBRC_SUCCESS)  
  {
    owa1.rc = XLIBRC_FILTER_MISSING;    
    FPOBJ_TRACE_EXIT (bufp, owa1);
    return owa1.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* call the usmdb only for add and delete */
  if (objDiffServPolicyAttrRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
       /*Get the next free attributrindex */
      if (usmDbDiffServPolicyAttrIndexNext(L7_UNIT_CURRENT, keyPolicyIndexValue, keyPolicyInstIndexValue,
                                           &tempPolicyAttrIndexValue ) != L7_SUCCESS)
      {
  	  owa.rc = XLIBRC_FAILURE;  /* TODO: Add error string */
         FPOBJ_TRACE_EXIT (bufp, owa);
         return owa.rc;
      }
    if(tempPolicyAttrIndexValue == 0)/*That means table is full*/
    {
      owa.l7rc = usmDbDiffServPolicyAttrIndexMaxGet(L7_UNIT_CURRENT,&maxIndex);
			if(owa.l7rc != L7_SUCCESS)
				{
				   owa.rc = XLIBRC_FAILURE;  /* TODO: Add error string */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
				}
			
    }
		else
		{
		  maxIndex = tempPolicyAttrIndexValue;
		}
    /* THIS IS A HACK */
    for(tempPolicyAttrIndxValue = 1; tempPolicyAttrIndxValue < maxIndex;tempPolicyAttrIndxValue++)
    {
      usmDbDiffServPolicyAttrStmtEntryTypeGet(L7_UNIT_CURRENT, keyPolicyIndexValue, keyPolicyInstIndexValue, tempPolicyAttrIndxValue,  
          &EntryType);
      if(entryType == EntryType)
      {
        found = 1;
        break;
      }
    }

    if(found == 0)
    {
      /*You have this type of entry already,so return that index so that the user 
        will be able to modify this entry*/

	    if(tempPolicyAttrIndexValue == 0)
	    {
	      owa.l7rc = L7_FAILURE;
	      owa.rc = XLIBRC_FAILURE;  /* TODO: Add error string */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
	    }
 
      /* create the row entry in the policy attribute table */
      if (usmDbDiffServPolicyAttrCreate(L7_UNIT_CURRENT, keyPolicyIndexValue, keyPolicyInstIndexValue,
                                           tempPolicyAttrIndexValue, L7_TRUE) != L7_SUCCESS)
      {
        owa.l7rc = L7_FAILURE;
        owa.rc = XLIBRC_FAILURE;  /* TODO: Add error string */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
	    }
    }
    else
    {
      tempPolicyAttrIndexValue = tempPolicyAttrIndxValue;
    }

  	/* Successful creation of the row,push the attribute index to the filter */
    xLibFilterSet(wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex, 0,
               (xLibU8_t *) &tempPolicyAttrIndexValue, sizeof(tempPolicyAttrIndexValue));

    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (objDiffServPolicyAttrRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    kwa3.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) &keyPolicyAttrIndexValue, &kwa3.len);
    if (kwa3.rc != XLIBRC_SUCCESS)
    {
     kwa3.rc = XLIBRC_FILTER_MISSING;
     FPOBJ_TRACE_EXIT (bufp, kwa3);
     return kwa3.rc;
    }
    /* Delete the existing row */
    owa.l7rc = usmDbDiffServPolicyAttrDelete(L7_UNIT_CURRENT, keyPolicyIndexValue,
                                           keyPolicyInstIndexValue,
                                           keyPolicyAttrIndexValue);
    
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
* @function fpObjGet_qosDiffServPolicyAttributeStmt_AssignQueueId
*
* @purpose Get 'AssignQueueId'
*
* @description [AssignQueueId]: Queue identifier to which all inbound packets
*              belonging to this class-instance are directed. This attribute
*              is only valid if the value of agentDiffServPolicyAttrStmtEntryType
*              is set to assignQueue(12). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_AssignQueueId (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAssignQueueIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtAssignQueueIdGet (L7_UNIT_CURRENT,
                                                 keyPolicyIndexValue,
                                                 keyPolicyInstIndexValue,
                                                 keyPolicyAttrIndexValue,
                                                 &objAssignQueueIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AssignQueueId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAssignQueueIdValue,
                           sizeof (objAssignQueueIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_qosDiffServPolicyAttributeStmt_AssignQueueId
*
* @purpose Set 'AssignQueueId'
*
* @description [AssignQueueId]: Queue identifier to which all inbound packets
*              belonging to this class-instance are directed. This attribute
*              is only valid if the value of agentDiffServPolicyAttrStmtEntryType
*              is set to assignQueue(12). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_qosDiffServPolicyAttributeStmt_AssignQueueId (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAssignQueueIdValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType;
  xLibU32_t prevAssignQueueValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AssignQueueId */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objAssignQueueIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAssignQueueIdValue, owa.len);



  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /*check if the entryKey set is valid for thie entry type */
  if(usmDbDiffServPolicyAttrStmtEntryTypeGet(L7_UNIT_CURRENT,
        keyPolicyIndexValue,keyPolicyInstIndexValue,
        keyPolicyAttrIndexValue,&entryType)!=L7_SUCCESS || entryType != 
      L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE)
  {
    usmDbDiffServPolicyAttrDelete(L7_UNIT_CURRENT, keyPolicyIndexValue, 
        keyPolicyInstIndexValue, keyPolicyAttrIndexValue);
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  owa.l7rc = usmDbDiffServPolicyAttrStmtAssignQueueIdGet(L7_UNIT_CURRENT,
      keyPolicyIndexValue,
      keyPolicyInstIndexValue,  keyPolicyAttrIndexValue,&prevAssignQueueValue);

  if(objAssignQueueIdValue == prevAssignQueueValue)
  {
    owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtAssignQueueIdSet (L7_UNIT_CURRENT,
                                                 keyPolicyIndexValue,
                                                 keyPolicyInstIndexValue,
                                                 keyPolicyAttrIndexValue,
                                                 objAssignQueueIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
  usmDbDiffServPolicyAttrDelete(L7_UNIT_CURRENT, keyPolicyIndexValue, keyPolicyInstIndexValue, keyPolicyAttrIndexValue);
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyAttributeStmt_Drop
*
* @purpose Get 'Drop'
*
* @description [Drop]: Flag to indicate that all inbound packets belonging
*              to this class-instance are dropped at ingress. This attribute
*              is only valid if the agentDiffServPolicyAttrStmtEntryType
*              is set to drop(13). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_Drop (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDropValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtDropFlagGet (L7_UNIT_CURRENT,
                                            keyPolicyIndexValue,
                                            keyPolicyInstIndexValue,
                                            keyPolicyAttrIndexValue,
                                            &objDropValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Drop */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDropValue,
                           sizeof (objDropValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyAttributeStmt_MarkCos2Val
*
* @purpose Get 'MarkCos2Val'
*
* @description [MarkCos2Val]: Three-bit user priority field value in the second/inner
*              802.1Q tag header of a double VLAN tagged Ethernet
*              frame, marked as part of the inbound policy for a class
*              instance. This attribute is only valid if the value of 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_MarkCos2Val (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMarkCos2ValValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtMarkCos2ValGet (L7_UNIT_CURRENT,
                                               keyPolicyIndexValue,
                                               keyPolicyInstIndexValue,
                                               keyPolicyAttrIndexValue,
                                               &objMarkCos2ValValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MarkCos2Val */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMarkCos2ValValue,
                           sizeof (objMarkCos2ValValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_qosDiffServPolicyAttributeStmt_MarkCos2Val
*
* @purpose Set 'MarkCos2Val'
*
* @description [MarkCos2Val]: Three-bit user priority field value in the second/inner
*              802.1Q tag header of a double VLAN tagged Ethernet
*              frame, marked as part of the inbound policy for a class
*              instance. This attribute is only valid if the value of 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_qosDiffServPolicyAttributeStmt_MarkCos2Val (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMarkCos2ValValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MarkCos2Val */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMarkCos2ValValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMarkCos2ValValue, owa.len);


  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtMarkCos2ValSet (L7_UNIT_CURRENT,
                                               keyPolicyIndexValue,
                                               keyPolicyInstIndexValue,
                                               keyPolicyAttrIndexValue,
                                               objMarkCos2ValValue);
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
* @function fpObjGet_qosDiffServPolicyAttributeStmt_PoliceColorConformIndex
*
* @purpose Get 'PoliceColorConformIndex'
*
* @description [PoliceColorConformIndex]: Index of the DiffServ Class Table
*              row whose class definition is used to specify the policing
*              color conform mode and value. This identifies incoming traffic
*              categorized as 'green' packets in the network. This attribute
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_PoliceColorConformIndex (void
                                                                          *wap,
                                                                          void
                                                                          *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceColorConformIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceColorConformIndexGet (L7_UNIT_CURRENT,
                                                           keyPolicyIndexValue,
                                                           keyPolicyInstIndexValue,
                                                           keyPolicyAttrIndexValue,
                                                           &objPoliceColorConformIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PoliceColorConformIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPoliceColorConformIndexValue,
                           sizeof (objPoliceColorConformIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyAttributeStmt_PoliceColorConformMode
*
* @purpose Get 'PoliceColorConformMode'
*
* @description [PoliceColorConformMode]: Policing color conform mode -- indicates
*              the color awareness mode of conforming traffic for
*              the policing style (simple, singlerate, or tworate) currently
*              configured for the specified class instance within the policy.
*              The default va 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_PoliceColorConformMode (void
                                                                         *wap,
                                                                         void
                                                                         *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceColorConformModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceColorConformModeGet (L7_UNIT_CURRENT,
                                                          keyPolicyIndexValue,
                                                          keyPolicyInstIndexValue,
                                                          keyPolicyAttrIndexValue,
                                                          &objPoliceColorConformModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PoliceColorConformMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPoliceColorConformModeValue,
                           sizeof (objPoliceColorConformModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/******************************************************************************
*
* @function fpObjSet_qosDiffServPolicyAttributeStmt_PoliceColorConformIndex
*
* @purpose Set 'PoliceColorConformIndex'
*
* @description [PoliceColorConformIndex]: Index of the DiffServ Class Table
*              row whose class definition is used to specify the policing
*              color conform mode and value. This identifies incoming traffic
*              categorized as 'green' packets in the network. This attribute
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_qosDiffServPolicyAttributeStmt_PoliceColorConformIndex (void
                                                                          *wap,
                                                                          void
                                                                          *
bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceColorConformIndexValue;
  L7_uint32 temp_val;

  FPOBJ_TRACE_ENTER (bufp);

    /* retrieve object: PoliceColorConformIndex */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & 
objPoliceColorConformIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPoliceColorConformIndexValue, owa.len);


  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, 
XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */


  if (objPoliceColorConformIndexValue != 0)
  {
    owa.l7rc = usmDbDiffServPolicyAttrStmtPoliceColorAwareClassValidate(
L7_UNIT_CURRENT,
                                                                               
         keyPolicyIndexValue,
                                                                               
         keyPolicyInstIndexValue,
                                                                               
         objPoliceColorConformIndexValue);
  }

  if (owa.l7rc == L7_SUCCESS)
  {
    /* make sure conform color and exceed color are not the same */
    if (usmDbDiffServPolicyAttrStmtPoliceColorExceedIndexGet(L7_UNIT_CURRENT,
                                                                               
         keyPolicyIndexValue,
                                                                               
         keyPolicyInstIndexValue,
                                                                               
         keyPolicyAttrIndexValue,
                                                                               
         &temp_val) == L7_SUCCESS)
    {
      if (usmDbDiffServPolicyAttrStmtPoliceColorAwareClassesAreEquivalent(
L7_UNIT_CURRENT,
                                                                               
      objPoliceColorConformIndexValue, 
                                                                               
      temp_val) == L7_TRUE)
        owa.l7rc = L7_FAILURE;
    }
  }

  if (owa.l7rc == L7_SUCCESS)
  {
    owa.l7rc = usmDbDiffServPolicyAttrStmtPoliceColorConformIndexSet(
L7_UNIT_CURRENT,
                                                                               
         keyPolicyIndexValue,
                                                                               
         keyPolicyInstIndexValue,
                                                                               
         keyPolicyAttrIndexValue,
                                                                               
         objPoliceColorConformIndexValue);
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
* @function fpObjGet_qosDiffServPolicyAttributeStmt_PoliceColorConformVal
*
* @purpose Get 'PoliceColorConformVal'
*
* @description [PoliceColorConformVal]: Policing color conform value -- indicates
*              the color awareness packet mark value of conforming
*              traffic for the policing style (simple, singlerate, or tworate)
*              currently configured for the specified class instance
*              within the policy. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_PoliceColorConformVal (void
                                                                        *wap,
                                                                        void
                                                                        *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceColorConformValValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceColorConformValGet (L7_UNIT_CURRENT,
                                                         keyPolicyIndexValue,
                                                         keyPolicyInstIndexValue,
                                                         keyPolicyAttrIndexValue,
                                                         &objPoliceColorConformValValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PoliceColorConformVal */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPoliceColorConformValValue,
                           sizeof (objPoliceColorConformValValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyAttributeStmt_PoliceColorExceedIndex
*
* @purpose Get 'PoliceColorExceedIndex'
*
* @description [PoliceColorExceedIndex]: Index of the DiffServ Class Table
*              row whose class definition is used to specify the policing
*              color exceed mode and value. This identifies incoming traffic
*              categorized as 'yellow' packets in the network. This attribute
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_PoliceColorExceedIndex (void
                                                                         *wap,
                                                                         void
                                                                         *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceColorExceedIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceColorExceedIndexGet (L7_UNIT_CURRENT,
                                                          keyPolicyIndexValue,
                                                          keyPolicyInstIndexValue,
                                                          keyPolicyAttrIndexValue,
                                                          &objPoliceColorExceedIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PoliceColorExceedIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPoliceColorExceedIndexValue,
                           sizeof (objPoliceColorExceedIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_qosDiffServPolicyAttributeStmt_PoliceColorExceedIndex
*
* @purpose Set 'PoliceColorExceedIndex'
*
* @description [PoliceColorExceedIndex]: Index of the DiffServ Class Table
*              row whose class definition is used to specify the policing
*              color exceed mode and value. This identifies incoming traffic
*              categorized as 'yellow' packets in the network. This attribute
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_qosDiffServPolicyAttributeStmt_PoliceColorExceedIndex (void
                                                                         *wap,
                                                                         void
                                                                         *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceColorExceedIndexValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PoliceColorExceedIndex */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPoliceColorExceedIndexValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPoliceColorExceedIndexValue, owa.len);


  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceColorExceedIndexSet (L7_UNIT_CURRENT,
                                                          keyPolicyIndexValue,
                                                          keyPolicyInstIndexValue,
                                                          keyPolicyAttrIndexValue,
                                                          objPoliceColorExceedIndexValue);
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
* @function fpObjGet_qosDiffServPolicyAttributeStmt_PoliceColorExceedMode
*
* @purpose Get 'PoliceColorExceedMode'
*
* @description [PoliceColorExceedMode]: Policing color exceed mode -- indicates
*              the color awareness mode of exceeding traffic for the
*              policing style (singlerate or tworate) currently configured
*              for the specified class instance within the policy. The default
*              value is blind 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_PoliceColorExceedMode (void
                                                                        *wap,
                                                                        void
                                                                        *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceColorExceedModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceColorExceedModeGet (L7_UNIT_CURRENT,
                                                         keyPolicyIndexValue,
                                                         keyPolicyInstIndexValue,
                                                         keyPolicyAttrIndexValue,
                                                         &objPoliceColorExceedModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PoliceColorExceedMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPoliceColorExceedModeValue,
                           sizeof (objPoliceColorExceedModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyAttributeStmt_PoliceColorExceedVal
*
* @purpose Get 'PoliceColorExceedVal'
*
* @description [PoliceColorExceedVal]: Policing color exceed value -- indicates
*              the color awareness packet mark value of exceeding traffic
*              for the policing style (singlerate or tworate) currently
*              configured for the specified class instance within the
*              policy. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_PoliceColorExceedVal (void
                                                                       *wap,
                                                                       void
                                                                       *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPoliceColorExceedValValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceColorExceedValGet (L7_UNIT_CURRENT,
                                                        keyPolicyIndexValue,
                                                        keyPolicyInstIndexValue,
                                                        keyPolicyAttrIndexValue,
                                                        &objPoliceColorExceedValValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PoliceColorExceedVal */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPoliceColorExceedValValue,
                           sizeof (objPoliceColorExceedValValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyAttributeStmt_RedirectIntf
*
* @purpose Get 'RedirectIntf'
*
* @description [RedirectIntf]: External interface number to which all inbound
*              packets belonging to this class-instance are redirected.
*              This attribute is only valid if the value of agentDiffServPolicyAttrStmtEntryType
*              is set to redirect(15). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_RedirectIntf (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRedirectIntfValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtRedirectIntfGet (L7_UNIT_CURRENT,
                                                keyPolicyIndexValue,
                                                keyPolicyInstIndexValue,
                                                keyPolicyAttrIndexValue,
                                                &objRedirectIntfValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RedirectIntf */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRedirectIntfValue,
                           sizeof (objRedirectIntfValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_qosDiffServPolicyAttributeStmt_RedirectIntf
*
* @purpose Set 'RedirectIntf'
*
* @description [RedirectIntf]: External interface number to which all inbound
*              packets belonging to this class-instance are redirected.
*              This attribute is only valid if the value of agentDiffServPolicyAttrStmtEntryType
*              is set to redirect(15). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_qosDiffServPolicyAttributeStmt_RedirectIntf (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRedirectIntfValue;
  xLibU32_t prevRedirectIntfValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RedirectIntf */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRedirectIntfValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRedirectIntfValue, owa.len);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);
  /*check if the entryKey set is valid for thie entry type */
  if(usmDbDiffServPolicyAttrStmtEntryTypeGet(L7_UNIT_CURRENT,
        keyPolicyIndexValue,keyPolicyInstIndexValue,
        keyPolicyAttrIndexValue,&entryType)!=L7_SUCCESS || entryType != 
      L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT)
  {

    usmDbDiffServPolicyAttrDelete(L7_UNIT_CURRENT, keyPolicyIndexValue, 
        keyPolicyInstIndexValue, keyPolicyAttrIndexValue);
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;

  }
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtRedirectIntfGet(L7_UNIT_CURRENT,
        keyPolicyIndexValue,keyPolicyInstIndexValue,
        keyPolicyAttrIndexValue,&prevRedirectIntfValue);

  if(objRedirectIntfValue == prevRedirectIntfValue)
  {
    owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtRedirectIntfSet (L7_UNIT_CURRENT,
                                                keyPolicyIndexValue,
                                                keyPolicyInstIndexValue,
                                                keyPolicyAttrIndexValue,
                                                objRedirectIntfValue);
  if (owa.l7rc != L7_SUCCESS)
  {
  usmDbDiffServPolicyAttrDelete(L7_UNIT_CURRENT, keyPolicyIndexValue, keyPolicyInstIndexValue, keyPolicyAttrIndexValue);
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

xLibRC_t fpObjList_qosDiffServPolicyAttributeStmt_MirrorIntf (void *wap,void *bufp)
{   
fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t)); 
xLibU32_t 
objPortValue; 
xLibU32_t 
nextObjPortValue;	
FPOBJ_TRACE_ENTER (bufp);  /* 
retrieve key: PortMirrorSessionID */ 
	owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_MirrorIntf,(xLibU8_t *) &objPortValue, &owa.len);
if (owa.rc != XLIBRC_SUCCESS)
	{   
	FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
	owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF, 0, &nextObjPortValue); 
	}  
else  
{
FPOBJ_TRACE_CURRENT_KEY (bufp, &objPortValue, owa.len);	

owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF, 0 , objPortValue,&nextObjPortValue);
}  
if (owa.l7rc != L7_SUCCESS)  
{    
  owa.rc = XLIBRC_ENDOF_TABLE;  
  FPOBJ_TRACE_EXIT (bufp, owa);  
  return owa.rc; 
}	 

if(usmDbQosDiffServIsValidIntf(L7_UNIT_CURRENT,nextObjPortValue)!= L7_TRUE)
 {    
owa.rc = XLIBRC_ENDOF_TABLE;  
FPOBJ_TRACE_EXIT (bufp, owa);  
return owa.rc; 
}	 

FPOBJ_TRACE_NEW_KEY (bufp, &nextObjPortValue, owa.len);
/* return the object value: PortMirrorSessionID */  
owa.rc =    xLibBufDataSet (bufp, (xLibU8_t *) & nextObjPortValue,sizeof (nextObjPortValue));  
FPOBJ_TRACE_EXIT (bufp, owa); 
return owa.rc; 
}    

xLibRC_t fpObjList_qosDiffServPolicyAttributeStmt_RedirectIntf (void *wap,
                                                             void *bufp)
{   
fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t)); 
xLibU32_t 
objPortValue; 
xLibU32_t 
nextObjPortValue;	
FPOBJ_TRACE_ENTER (bufp);  /* 
retrieve key: PortMirrorSessionID */ 
	owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_RedirectIntf,(xLibU8_t *) &objPortValue, &owa.len);
if (owa.rc != XLIBRC_SUCCESS)
	{   
	FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
	owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF, 0, &nextObjPortValue);  
	}  
else  
{
FPOBJ_TRACE_CURRENT_KEY (bufp, &objPortValue, owa.len);	

owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF, 0 , objPortValue,&nextObjPortValue); 
}  
if (owa.l7rc != L7_SUCCESS)  
{    
owa.rc = XLIBRC_ENDOF_TABLE;  
FPOBJ_TRACE_EXIT (bufp, owa);  
return owa.rc; 
}	 

if(usmDbQosDiffServIsValidIntf(L7_UNIT_CURRENT,nextObjPortValue)!= L7_TRUE)
{   
  owa.rc = XLIBRC_ENDOF_TABLE;  
  FPOBJ_TRACE_EXIT (bufp, owa);  
  return owa.rc; 
}	 

FPOBJ_TRACE_NEW_KEY (bufp, &nextObjPortValue, owa.
len);
/* return the object value: PortMirrorSessionID */  
owa.rc =    xLibBufDataSet (bufp, (xLibU8_t *) & nextObjPortValue,sizeof (nextObjPortValue));  
FPOBJ_TRACE_EXIT (bufp, owa); 
return owa.rc; 
}    
/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyAttributeStmt_MirrorIntf
*
* @purpose Get 'MirrorIntf'
*
* @description [MirrorIntf]: External interface number to which all inbound
*              packets belonging to this class-instance are mirrored. This
*              attribute is only valid if the value of agentDiffServPolicyAttrStmtEntryType
*              is set to mirror(16). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_MirrorIntf (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMirrorIntfValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtMirrorIntfGet (L7_UNIT_CURRENT,
                                              keyPolicyIndexValue,
                                              keyPolicyInstIndexValue,
                                              keyPolicyAttrIndexValue,
                                              &objMirrorIntfValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MirrorIntf */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMirrorIntfValue,
                           sizeof (objMirrorIntfValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_qosDiffServPolicyAttributeStmt_MirrorIntf
*
* @purpose Set 'MirrorIntf'
*
* @description [MirrorIntf]: External interface number to which all inbound
*              packets belonging to this class-instance are mirrored. This
*              attribute is only valid if the value of agentDiffServPolicyAttrStmtEntryType
*              is set to mirror(16). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_qosDiffServPolicyAttributeStmt_MirrorIntf (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMirrorIntfValue;
  xLibU32_t prevMirrorIntfValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MirrorIntf */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMirrorIntfValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMirrorIntfValue, owa.len);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /*check if the entryKey set is valid for thie entry type */
  if(usmDbDiffServPolicyAttrStmtEntryTypeGet(L7_UNIT_CURRENT,
        keyPolicyIndexValue,keyPolicyInstIndexValue,
        keyPolicyAttrIndexValue,&entryType)!=L7_SUCCESS || entryType != 
      L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR)
  {

    usmDbDiffServPolicyAttrDelete(L7_UNIT_CURRENT, keyPolicyIndexValue, 
        keyPolicyInstIndexValue, keyPolicyAttrIndexValue);
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;

  }
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtMirrorIntfGet(L7_UNIT_CURRENT,
        keyPolicyIndexValue,keyPolicyInstIndexValue,
        keyPolicyAttrIndexValue,&prevMirrorIntfValue);

  if(objMirrorIntfValue == prevMirrorIntfValue)
  {
    owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }



  /* set the value in application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtMirrorIntfSet (L7_UNIT_CURRENT,
                                              keyPolicyIndexValue,
                                              keyPolicyInstIndexValue,
                                              keyPolicyAttrIndexValue,
                                              objMirrorIntfValue);
  if (owa.l7rc != L7_SUCCESS)
  {
  usmDbDiffServPolicyAttrDelete(L7_UNIT_CURRENT, keyPolicyIndexValue, keyPolicyInstIndexValue, keyPolicyAttrIndexValue);
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/******************************************************************************
 *
 * @function fpObjGet_qosDiffServPolicyAttributeStmt_AssignQueueIndex
 *
 * @purpose Get 'AssignQueueIndex'
 *
 * @description [MirrorIntf]: Get the attribute index of Assign Queue Match.
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_AssignQueueIndex (void *wap,
    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAssignQueueIndex;
  L7_RC_t rc;
  xLibBool_t entryFound = XLIB_FALSE;


  xLibU32_t tempAttrPolicyIndex, tempAttrPolicyInstIndex, policyAttrIndex;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, 
      XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
      (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
        (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);


  /* get the value from application */


  tempAttrPolicyIndex = keyPolicyIndexValue;
  tempAttrPolicyInstIndex = keyPolicyInstIndexValue;

  rc = usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT, keyPolicyIndexValue,
      keyPolicyInstIndexValue, 0,
      &tempAttrPolicyIndex, &
      tempAttrPolicyInstIndex,
      &policyAttrIndex);


  while((tempAttrPolicyIndex == keyPolicyIndexValue) && (
        tempAttrPolicyInstIndex == keyPolicyInstIndexValue) && (rc == L7_SUCCESS) )
  {
    if(usmDbDiffServPolicyAttrStmtEntryTypeGet(L7_UNIT_CURRENT, 
          tempAttrPolicyIndex,
          tempAttrPolicyInstIndex,
          policyAttrIndex, &entryType) 
        == L7_SUCCESS)
    {
      if (entryType ==  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE)
      {
        entryFound = XLIB_TRUE;
        objAssignQueueIndex = policyAttrIndex;
        owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAssignQueueIndex,
            sizeof (objAssignQueueIndex));
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    }
    rc = usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT, keyPolicyIndexValue,
        keyPolicyInstIndexValue, 
        policyAttrIndex,
        &tempAttrPolicyIndex, &
        tempAttrPolicyInstIndex,
        &policyAttrIndex); 
  }


  owa.l7rc = usmDbDiffServPolicyAttrIndexNext(L7_UNIT_CURRENT, 
      keyPolicyIndexValue, keyPolicyInstIndexValue,
      &objAssignQueueIndex );

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAssignQueueIndex,
      sizeof (objAssignQueueIndex));
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
  }
  return owa.rc;
}


/******************************************************************************
 *
 * @function fpObjGet_qosDiffServPolicyAttributeStmt_MarkCosIndex
 *
 * @purpose Get 'MarkCosIndex'
 *
 * @description [MirrorIntf]: Get the attribute index of Mark Cos Match.
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_MarkCosIndex (void *wap,
    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMarkCosIndex;
  L7_RC_t rc;
  xLibBool_t entryFound = XLIB_FALSE;


  xLibU32_t tempAttrPolicyIndex, tempAttrPolicyInstIndex, policyAttrIndex;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, 
      XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
      (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
        (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);


  /* get the value from application */


  tempAttrPolicyIndex = keyPolicyIndexValue;
  tempAttrPolicyInstIndex = keyPolicyInstIndexValue;

  rc = usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT, keyPolicyIndexValue,
      keyPolicyInstIndexValue, 0,
      &tempAttrPolicyIndex, &
      tempAttrPolicyInstIndex,
      &policyAttrIndex);


  while((tempAttrPolicyIndex == keyPolicyIndexValue) && (
        tempAttrPolicyInstIndex == keyPolicyInstIndexValue) && (rc == L7_SUCCESS) )
  {
    if(usmDbDiffServPolicyAttrStmtEntryTypeGet(L7_UNIT_CURRENT, 
          tempAttrPolicyIndex,
          tempAttrPolicyInstIndex,
          policyAttrIndex, &entryType) 
        == L7_SUCCESS)
    {
      if ((entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL) ||
               (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2))
      {
        entryFound = XLIB_TRUE;
        objMarkCosIndex = policyAttrIndex;
        owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMarkCosIndex,
            sizeof (objMarkCosIndex));
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    }
    rc = usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT, keyPolicyIndexValue,
        keyPolicyInstIndexValue, 
        policyAttrIndex,
        &tempAttrPolicyIndex, &
        tempAttrPolicyInstIndex,
        &policyAttrIndex); 
  }


  owa.l7rc = usmDbDiffServPolicyAttrIndexNext(L7_UNIT_CURRENT, 
      keyPolicyIndexValue, keyPolicyInstIndexValue,
      &objMarkCosIndex );

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMarkCosIndex,
      sizeof (objMarkCosIndex));
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
  }
  return owa.rc;
}

/******************************************************************************
 *
 * @function fpObjGet_qosDiffServPolicyAttributeStmt_IPDSCPIndex
 *
 * @purpose Get 'IPDSCPIndex'
 *
 * @description [MirrorIntf]: Get the attribute index of IPDSCPIndex Match.
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_IPDSCPIndex (void *wap,
    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIPDSCPIndex;
  L7_RC_t rc;
  xLibBool_t entryFound = XLIB_FALSE;


  xLibU32_t tempAttrPolicyIndex, tempAttrPolicyInstIndex, policyAttrIndex;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, 
      XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
      (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
        (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);


  /* get the value from application */


  tempAttrPolicyIndex = keyPolicyIndexValue;
  tempAttrPolicyInstIndex = keyPolicyInstIndexValue;

  rc = usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT, keyPolicyIndexValue,
      keyPolicyInstIndexValue, 0,
      &tempAttrPolicyIndex, &
      tempAttrPolicyInstIndex,
      &policyAttrIndex);


  while((tempAttrPolicyIndex == keyPolicyIndexValue) && (
        tempAttrPolicyInstIndex == keyPolicyInstIndexValue) && (rc == L7_SUCCESS) )
  {
    if(usmDbDiffServPolicyAttrStmtEntryTypeGet(L7_UNIT_CURRENT, 
          tempAttrPolicyIndex,
          tempAttrPolicyInstIndex,
          policyAttrIndex, &entryType) 
        == L7_SUCCESS)
    {
      if (entryType ==  
          L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL)
      {
        entryFound = XLIB_TRUE;
        objIPDSCPIndex = policyAttrIndex;
        owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIPDSCPIndex,
            sizeof (objIPDSCPIndex));
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    }
    rc = usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT, keyPolicyIndexValue,
        keyPolicyInstIndexValue, 
        policyAttrIndex,
        &tempAttrPolicyIndex, &
        tempAttrPolicyInstIndex,
        &policyAttrIndex); 
  }


  owa.l7rc = usmDbDiffServPolicyAttrIndexNext(L7_UNIT_CURRENT, 
      keyPolicyIndexValue, keyPolicyInstIndexValue,
      &objIPDSCPIndex );

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIPDSCPIndex,
      sizeof (objIPDSCPIndex));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/******************************************************************************
 *
 * @function fpObjGet_qosDiffServPolicyAttributeStmt_IPPrecedenceIndex
 *
 * @purpose Get 'IPPrecedenceIndex'
 *
 * @description [MirrorIntf]: Get the attribute index of IPPrecedenceIndex 
 Match.
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_IPPrecedenceIndex (void *wap,
    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIPPrecedenceIndex;
  L7_RC_t rc;
  xLibBool_t entryFound = XLIB_FALSE;


  xLibU32_t tempAttrPolicyIndex, tempAttrPolicyInstIndex, policyAttrIndex;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, 
      XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
      (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
        (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);


  /* get the value from application */


  tempAttrPolicyIndex = keyPolicyIndexValue;
  tempAttrPolicyInstIndex = keyPolicyInstIndexValue;

  rc = usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT, keyPolicyIndexValue,
      keyPolicyInstIndexValue, 0,
      &tempAttrPolicyIndex, &
      tempAttrPolicyInstIndex,
      &policyAttrIndex);


  while((tempAttrPolicyIndex == keyPolicyIndexValue) && (
        tempAttrPolicyInstIndex == keyPolicyInstIndexValue) && (rc == L7_SUCCESS) )
  {
    if(usmDbDiffServPolicyAttrStmtEntryTypeGet(L7_UNIT_CURRENT, 
          tempAttrPolicyIndex,
          tempAttrPolicyInstIndex,
          policyAttrIndex, &entryType) 
        == L7_SUCCESS)
    {
      if (entryType ==  
          L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL)
      {
        entryFound = XLIB_TRUE;
        objIPPrecedenceIndex = policyAttrIndex;
        owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIPPrecedenceIndex,
            sizeof (objIPPrecedenceIndex));
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    }
    rc = usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT, keyPolicyIndexValue,
        keyPolicyInstIndexValue, 
        policyAttrIndex,
        &tempAttrPolicyIndex, &
        tempAttrPolicyInstIndex,
        &policyAttrIndex); 
  }


  owa.l7rc = usmDbDiffServPolicyAttrIndexNext(L7_UNIT_CURRENT, 
      keyPolicyIndexValue, keyPolicyInstIndexValue,
      &objIPPrecedenceIndex );

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIPPrecedenceIndex,
      sizeof (objIPPrecedenceIndex));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/******************************************************************************
 *
 * @function fpObjGet_qosDiffServPolicyAttributeStmt_DropIndex
 *
 * @purpose Get 'DropIndex'
 *
 * @description [MirrorIntf]: Get the attribute index of drop index Match.
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_DropIndex (void *wap,
    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDropIndex;
  L7_RC_t rc;
  xLibBool_t entryFound = XLIB_FALSE;


  xLibU32_t tempAttrPolicyIndex, tempAttrPolicyInstIndex, policyAttrIndex;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, 
      XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
      (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
        (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);


  /* get the value from application */


  tempAttrPolicyIndex = keyPolicyIndexValue;
  tempAttrPolicyInstIndex = keyPolicyInstIndexValue;

  rc = usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT, keyPolicyIndexValue,
      keyPolicyInstIndexValue, 0,
      &tempAttrPolicyIndex, &
      tempAttrPolicyInstIndex,
      &policyAttrIndex);


  while((tempAttrPolicyIndex == keyPolicyIndexValue) && (
        tempAttrPolicyInstIndex == keyPolicyInstIndexValue) && (rc == L7_SUCCESS) )
  {
    if(usmDbDiffServPolicyAttrStmtEntryTypeGet(L7_UNIT_CURRENT, 
          tempAttrPolicyIndex,
          tempAttrPolicyInstIndex,
          policyAttrIndex, &entryType) 
        == L7_SUCCESS)
    {
      if (entryType ==  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP)
      {
        entryFound = XLIB_TRUE;
        objDropIndex = policyAttrIndex;
        owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDropIndex,
            sizeof (objDropIndex));
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    }
    rc = usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT, keyPolicyIndexValue,
        keyPolicyInstIndexValue, 
        policyAttrIndex,
        &tempAttrPolicyIndex, &
        tempAttrPolicyInstIndex,
        &policyAttrIndex); 
  }


  owa.l7rc = usmDbDiffServPolicyAttrIndexNext(L7_UNIT_CURRENT, 
      keyPolicyIndexValue, keyPolicyInstIndexValue,
      &objDropIndex );

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDropIndex,
      sizeof (objDropIndex));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/******************************************************************************
 *
 * @function fpObjGet_qosDiffServPolicyAttributeStmt_MirrorIntfIndex
 *
 * @purpose Get 'MirrorIntfIndex'
 *
 * @description [MirrorIntf]: Get the attribute index of MirrorIntfIndexMatch.
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_MirrorIntfIndex (void *wap,
    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMirrorIntfIndex;
  L7_RC_t rc;
  xLibBool_t entryFound = XLIB_FALSE;


  xLibU32_t tempAttrPolicyIndex, tempAttrPolicyInstIndex, policyAttrIndex;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, 
      XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
      (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
        (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);


  /* get the value from application */


  tempAttrPolicyIndex = keyPolicyIndexValue;
  tempAttrPolicyInstIndex = keyPolicyInstIndexValue;

  rc = usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT, keyPolicyIndexValue,
      keyPolicyInstIndexValue, 0,
      &tempAttrPolicyIndex, &
      tempAttrPolicyInstIndex,
      &policyAttrIndex);


  while((tempAttrPolicyIndex == keyPolicyIndexValue) && (
        tempAttrPolicyInstIndex == keyPolicyInstIndexValue) && (rc == L7_SUCCESS) )
  {
    if(usmDbDiffServPolicyAttrStmtEntryTypeGet(L7_UNIT_CURRENT, 
          tempAttrPolicyIndex,
          tempAttrPolicyInstIndex,
          policyAttrIndex, &entryType) 
        == L7_SUCCESS)
    {
      if (entryType ==  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR)
      {
        entryFound = XLIB_TRUE;
        objMirrorIntfIndex = policyAttrIndex;
        owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMirrorIntfIndex,
            sizeof (objMirrorIntfIndex));
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    }
    rc = usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT, keyPolicyIndexValue,
        keyPolicyInstIndexValue, 
        policyAttrIndex,
        &tempAttrPolicyIndex, &
        tempAttrPolicyInstIndex,
        &policyAttrIndex); 
  }


  owa.l7rc = usmDbDiffServPolicyAttrIndexNext(L7_UNIT_CURRENT, 
      keyPolicyIndexValue, keyPolicyInstIndexValue,
      &objMirrorIntfIndex );

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMirrorIntfIndex,
      sizeof (objMirrorIntfIndex));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/******************************************************************************
 *
 * @function fpObjGet_qosDiffServPolicyAttributeStmt_RedirectIntfIndex
 *
 * @purpose Get 'RedirectIntfIndex'
 *
 * @description [MirrorIntf]: Get the attribute index of RedirectIntfIndex 
 match.
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_RedirectIntfIndex (void *wap,
    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRedirectIntfIndex;
  L7_RC_t rc;
  xLibBool_t entryFound = XLIB_FALSE;


  xLibU32_t tempAttrPolicyIndex, tempAttrPolicyInstIndex, policyAttrIndex;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, 
      XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
      (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
        (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);


  /* get the value from application */


  tempAttrPolicyIndex = keyPolicyIndexValue;
  tempAttrPolicyInstIndex = keyPolicyInstIndexValue;

  rc = usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT, keyPolicyIndexValue,
      keyPolicyInstIndexValue, 0,
      &tempAttrPolicyIndex, &
      tempAttrPolicyInstIndex,
      &policyAttrIndex);


  while((tempAttrPolicyIndex == keyPolicyIndexValue) && (
        tempAttrPolicyInstIndex == keyPolicyInstIndexValue) && (rc == L7_SUCCESS) )
  {
    if(usmDbDiffServPolicyAttrStmtEntryTypeGet(L7_UNIT_CURRENT, 
          tempAttrPolicyIndex,
          tempAttrPolicyInstIndex,
          policyAttrIndex, &entryType) 
        == L7_SUCCESS)
    {
      if (entryType ==  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT)
      {
        entryFound = XLIB_TRUE;
        objRedirectIntfIndex = policyAttrIndex;
        owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRedirectIntfIndex,
            sizeof (objRedirectIntfIndex));
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    }
    rc = usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT, keyPolicyIndexValue,
        keyPolicyInstIndexValue, 
        policyAttrIndex,
        &tempAttrPolicyIndex, &
        tempAttrPolicyInstIndex,
        &policyAttrIndex); 
  }


  owa.l7rc = usmDbDiffServPolicyAttrIndexNext(L7_UNIT_CURRENT, 
      keyPolicyIndexValue, keyPolicyInstIndexValue,
      &objRedirectIntfIndex );

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRedirectIntfIndex,
      sizeof (objRedirectIntfIndex));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/******************************************************************************
 *
 * @function fpObjGet_qosDiffServPolicyAttributeStmt_SimplePoliceIndex
 *
 * @purpose Get 'SimplePoliceIndex'
 *
 * @description [MirrorIntf]: Get the attribute index of SimplePoliceIndex 
 match.
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_SimplePoliceIndex (void *wap,
    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSimplePoliceIndex;
  L7_RC_t rc;
  xLibBool_t entryFound = XLIB_FALSE;


  xLibU32_t tempAttrPolicyIndex, tempAttrPolicyInstIndex, policyAttrIndex;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, 
      XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
      (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
        (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);


  /* get the value from application */


  tempAttrPolicyIndex = keyPolicyIndexValue;
  tempAttrPolicyInstIndex = keyPolicyInstIndexValue;

  rc = usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT, keyPolicyIndexValue,
      keyPolicyInstIndexValue, 0,
      &tempAttrPolicyIndex, &
      tempAttrPolicyInstIndex,
      &policyAttrIndex);


  while((tempAttrPolicyIndex == keyPolicyIndexValue) && (
        tempAttrPolicyInstIndex == keyPolicyInstIndexValue) && (rc == L7_SUCCESS) )
  {
    if(usmDbDiffServPolicyAttrStmtEntryTypeGet(L7_UNIT_CURRENT, 
          tempAttrPolicyIndex,
          tempAttrPolicyInstIndex,
          policyAttrIndex, &entryType) 
        == L7_SUCCESS)
    {
      if (entryType ==  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE)
      {
        entryFound = XLIB_TRUE;
        objSimplePoliceIndex = policyAttrIndex;
        owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSimplePoliceIndex,
            sizeof (objSimplePoliceIndex));
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    }
    rc = usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT, keyPolicyIndexValue,
        keyPolicyInstIndexValue, 
        policyAttrIndex,
        &tempAttrPolicyIndex, &
        tempAttrPolicyInstIndex,
        &policyAttrIndex); 
  }


  owa.l7rc = usmDbDiffServPolicyAttrIndexNext(L7_UNIT_CURRENT, 
      keyPolicyIndexValue, keyPolicyInstIndexValue,
      &objSimplePoliceIndex );

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSimplePoliceIndex,
      sizeof (objSimplePoliceIndex));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}




/******************************************************************************
 *
 * @function fpObjGet_qosDiffServPolicyAttributeStmt_IsAssignQueuePresent
 *
 * @purpose Get 'IsAssignQueuePresent'
 *
 * @description [MirrorIntf]: To check if the AssignQueue match criteria 
 *   is configured for the given policy and class combination.
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_IsAssignQueuePresent (void *
    wap,
    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsAssignQueuePresent;
  L7_RC_t rc;
  xLibBool_t entryFound = XLIB_FALSE;


  xLibU32_t tempAttrPolicyIndex, tempAttrPolicyInstIndex, policyAttrIndex;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, 
      XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
      (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
        (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);


  /* get the value from application */


  tempAttrPolicyIndex = keyPolicyIndexValue;
  tempAttrPolicyInstIndex = keyPolicyInstIndexValue;

  rc = usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT, keyPolicyIndexValue,
      keyPolicyInstIndexValue, 0,
      &tempAttrPolicyIndex, &
      tempAttrPolicyInstIndex,
      &policyAttrIndex);


  while((tempAttrPolicyIndex == keyPolicyIndexValue) && (
        tempAttrPolicyInstIndex == keyPolicyInstIndexValue) && (rc == L7_SUCCESS) )
  {
    if(usmDbDiffServPolicyAttrStmtEntryTypeGet(L7_UNIT_CURRENT, 
          tempAttrPolicyIndex,
          tempAttrPolicyInstIndex,
          policyAttrIndex, &entryType) 
        == L7_SUCCESS)
    {
      if (entryType ==  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE)
      {
        entryFound = XLIB_TRUE;
        objIsAssignQueuePresent = L7_TRUE;
        owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsAssignQueuePresent,
            sizeof (objIsAssignQueuePresent));
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    }
    rc = usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT, keyPolicyIndexValue,
        keyPolicyInstIndexValue, 
        policyAttrIndex,
        &tempAttrPolicyIndex, &
        tempAttrPolicyInstIndex,
        &policyAttrIndex); 
  }


  objIsAssignQueuePresent = L7_FALSE;

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsAssignQueuePresent,
      sizeof (objIsAssignQueuePresent));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/******************************************************************************
 *
 * @function fpObjGet_qosDiffServPolicyAttributeStmt_IsMarkCosPresent
 *
 * @purpose Get 'IsMarkCosPresent'
 *
 * @description [MirrorIntf]: To check if the Mark Cos match criteria is 
 configured 
 *    for the given policy and class combination.
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_IsMarkCosPresent(void *wap,
    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsMarkCosPresent;
  L7_RC_t rc;
  xLibBool_t entryFound = XLIB_FALSE;


  xLibU32_t tempAttrPolicyIndex, tempAttrPolicyInstIndex, policyAttrIndex;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, 
      XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
      (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
        (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);


  /* get the value from application */


  tempAttrPolicyIndex = keyPolicyIndexValue;
  tempAttrPolicyInstIndex = keyPolicyInstIndexValue;

  rc = usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT, keyPolicyIndexValue,
      keyPolicyInstIndexValue, 0,
      &tempAttrPolicyIndex, &
      tempAttrPolicyInstIndex,
      &policyAttrIndex);


  while((tempAttrPolicyIndex == keyPolicyIndexValue) && (
        tempAttrPolicyInstIndex == keyPolicyInstIndexValue) && (rc == L7_SUCCESS) )
  {
    if(usmDbDiffServPolicyAttrStmtEntryTypeGet(L7_UNIT_CURRENT, 
          tempAttrPolicyIndex,
          tempAttrPolicyInstIndex,
          policyAttrIndex, &entryType) 
        == L7_SUCCESS)
    {
      if (entryType ==  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL)
      {
        entryFound = XLIB_TRUE;
        objIsMarkCosPresent = L7_TRUE;
        owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsMarkCosPresent,
            sizeof (objIsMarkCosPresent));
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    }
    rc = usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT, keyPolicyIndexValue,
        keyPolicyInstIndexValue, 
        policyAttrIndex,
        &tempAttrPolicyIndex, &
        tempAttrPolicyInstIndex,
        &policyAttrIndex); 
  }


  objIsMarkCosPresent = L7_FALSE;

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsMarkCosPresent,
      sizeof (objIsMarkCosPresent));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/******************************************************************************
 *
 * @function fpObjGet_qosDiffServPolicyAttributeStmt_IsIPDSCPPresent
 *
 * @purpose Get 'IsIPDSCPPresent'
 *
 * @description [MirrorIntf]: To check if the IsIPDSCPPresent match criteria 
 is configured 
 *    for the given policy and class combination.
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_IsIPDSCPPresent(void *wap,
    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsIPDSCPPresent;
  L7_RC_t rc;
  xLibBool_t entryFound = XLIB_FALSE;


  xLibU32_t tempAttrPolicyIndex, tempAttrPolicyInstIndex, policyAttrIndex;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, 
      XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
      (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
        (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);


  /* get the value from application */


  tempAttrPolicyIndex = keyPolicyIndexValue;
  tempAttrPolicyInstIndex = keyPolicyInstIndexValue;

  rc = usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT, keyPolicyIndexValue,
      keyPolicyInstIndexValue, 0,
      &tempAttrPolicyIndex, &
      tempAttrPolicyInstIndex,
      &policyAttrIndex);


  while((tempAttrPolicyIndex == keyPolicyIndexValue) && (
        tempAttrPolicyInstIndex == keyPolicyInstIndexValue) && (rc == L7_SUCCESS) )
  {
    if(usmDbDiffServPolicyAttrStmtEntryTypeGet(L7_UNIT_CURRENT, 
          tempAttrPolicyIndex,
          tempAttrPolicyInstIndex,
          policyAttrIndex, &entryType) 
        == L7_SUCCESS)
    {
      if (entryType ==  
          L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL)
      {
        entryFound = XLIB_TRUE;
        objIsIPDSCPPresent = L7_TRUE;
        owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsIPDSCPPresent,
            sizeof (objIsIPDSCPPresent));
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    }
    rc = usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT, keyPolicyIndexValue,
        keyPolicyInstIndexValue, 
        policyAttrIndex,
        &tempAttrPolicyIndex, &
        tempAttrPolicyInstIndex,
        &policyAttrIndex); 
  }


  objIsIPDSCPPresent = L7_FALSE;

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsIPDSCPPresent,
      sizeof (objIsIPDSCPPresent));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/******************************************************************************
 *
 * @function fpObjGet_qosDiffServPolicyAttributeStmt_IsIPPrecedencePresent
 *
 * @purpose Get 'IsIPPrecedencePresent'
 *
 * @description [MirrorIntf]: To check if the IsIPPrecedencePresent match 
 criteria is configured 
 *    for the given policy and class combination.
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_IsIPPrecedencePresent(void *
    wap,
    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsIPPrecedencePresent;
  L7_RC_t rc;
  xLibBool_t entryFound = XLIB_FALSE;


  xLibU32_t tempAttrPolicyIndex, tempAttrPolicyInstIndex, policyAttrIndex;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, 
      XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
      (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
        (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);


  /* get the value from application */


  tempAttrPolicyIndex = keyPolicyIndexValue;
  tempAttrPolicyInstIndex = keyPolicyInstIndexValue;

  rc = usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT, keyPolicyIndexValue,
      keyPolicyInstIndexValue, 0,
      &tempAttrPolicyIndex, &
      tempAttrPolicyInstIndex,
      &policyAttrIndex);


  while((tempAttrPolicyIndex == keyPolicyIndexValue) && (
        tempAttrPolicyInstIndex == keyPolicyInstIndexValue) && (rc == L7_SUCCESS) )
  {
    if(usmDbDiffServPolicyAttrStmtEntryTypeGet(L7_UNIT_CURRENT, 
          tempAttrPolicyIndex,
          tempAttrPolicyInstIndex,
          policyAttrIndex, &entryType) 
        == L7_SUCCESS)
    {
      if (entryType ==  
          L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL)
      {
        entryFound = XLIB_TRUE;
        objIsIPPrecedencePresent = L7_TRUE;
        owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsIPPrecedencePresent,
            sizeof (objIsIPPrecedencePresent));
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    }
    rc = usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT, keyPolicyIndexValue,
        keyPolicyInstIndexValue, 
        policyAttrIndex,
        &tempAttrPolicyIndex, &
        tempAttrPolicyInstIndex,
        &policyAttrIndex); 
  }


  objIsIPPrecedencePresent = L7_FALSE;

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsIPPrecedencePresent,
      sizeof (objIsIPPrecedencePresent));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/******************************************************************************
 *
 * @function fpObjGet_qosDiffServPolicyAttributeStmt_IsDropPresent
 *
 * @purpose Get 'IsDropPresent'
 *
 * @description [MirrorIntf]: To check if the IsDropPresent match criteria is 
 configured 
 *    for the given policy and class combination.
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_IsDropPresent(void *wap,
    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsDropPresent;
  L7_RC_t rc;
  xLibBool_t entryFound = XLIB_FALSE;


  xLibU32_t tempAttrPolicyIndex, tempAttrPolicyInstIndex, policyAttrIndex;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, 
      XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
      (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
        (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);


  /* get the value from application */


  tempAttrPolicyIndex = keyPolicyIndexValue;
  tempAttrPolicyInstIndex = keyPolicyInstIndexValue;

  rc = usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT, keyPolicyIndexValue,
      keyPolicyInstIndexValue, 0,
      &tempAttrPolicyIndex, &
      tempAttrPolicyInstIndex,
      &policyAttrIndex);


  while((tempAttrPolicyIndex == keyPolicyIndexValue) && (
        tempAttrPolicyInstIndex == keyPolicyInstIndexValue) && (rc == L7_SUCCESS) )
  {
    if(usmDbDiffServPolicyAttrStmtEntryTypeGet(L7_UNIT_CURRENT, 
          tempAttrPolicyIndex,
          tempAttrPolicyInstIndex,
          policyAttrIndex, &entryType) 
        == L7_SUCCESS)
    {
      if (entryType ==  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP)
      {
        entryFound = XLIB_TRUE;
        objIsDropPresent = L7_TRUE;
        owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsDropPresent,
            sizeof (objIsDropPresent));
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    }
    rc = usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT, keyPolicyIndexValue,
        keyPolicyInstIndexValue, 
        policyAttrIndex,
        &tempAttrPolicyIndex, &
        tempAttrPolicyInstIndex,
        &policyAttrIndex); 
  }


  objIsDropPresent = L7_FALSE;

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsDropPresent,
      sizeof (objIsDropPresent));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/******************************************************************************
 *
 * @function fpObjGet_qosDiffServPolicyAttributeStmt_IsMirrorIntfPresent
 *
 * @purpose Get 'IsMirrorIntfPresent'
 *
 * @description [MirrorIntf]: To check if the IsMirrorIntfPresent match 
 criteria is configured 
 *    for the given policy and class combination.
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_IsMirrorIntfPresent(void *wap,
    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsMirrorIntfPresent;
  L7_RC_t rc;
  xLibBool_t entryFound = XLIB_FALSE;


  xLibU32_t tempAttrPolicyIndex, tempAttrPolicyInstIndex, policyAttrIndex;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, 
      XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
      (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
        (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);


  /* get the value from application */


  tempAttrPolicyIndex = keyPolicyIndexValue;
  tempAttrPolicyInstIndex = keyPolicyInstIndexValue;

  rc = usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT, keyPolicyIndexValue,
      keyPolicyInstIndexValue, 0,
      &tempAttrPolicyIndex, &
      tempAttrPolicyInstIndex,
      &policyAttrIndex);


  while((tempAttrPolicyIndex == keyPolicyIndexValue) && (
        tempAttrPolicyInstIndex == keyPolicyInstIndexValue) && (rc == L7_SUCCESS) )
  {
    if(usmDbDiffServPolicyAttrStmtEntryTypeGet(L7_UNIT_CURRENT, 
          tempAttrPolicyIndex,
          tempAttrPolicyInstIndex,
          policyAttrIndex, &entryType) 
        == L7_SUCCESS)
    {
      if (entryType ==  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR)
      {
        entryFound = XLIB_TRUE;
        objIsMirrorIntfPresent = L7_TRUE;
        owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsMirrorIntfPresent,
            sizeof (objIsMirrorIntfPresent));
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    }
    rc = usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT, keyPolicyIndexValue,
        keyPolicyInstIndexValue, 
        policyAttrIndex,
        &tempAttrPolicyIndex, &
        tempAttrPolicyInstIndex,
        &policyAttrIndex); 
  }


  objIsMirrorIntfPresent = L7_FALSE;

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsMirrorIntfPresent,
      sizeof (objIsMirrorIntfPresent));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/******************************************************************************
 *
 * @function fpObjGet_qosDiffServPolicyAttributeStmt_IsRedirectIntfPresent
 *
 * @purpose Get 'IsRedirectIntfPresent'
 *
 * @description [MirrorIntf]: To check if the IsRedirectIntfPresent match 
 criteria is configured 
 *    for the given policy and class combination.
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_IsRedirectIntfPresent(void *
    wap,
    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsRedirectIntfPresent;
  L7_RC_t rc;
  xLibBool_t entryFound = XLIB_FALSE;


  xLibU32_t tempAttrPolicyIndex, tempAttrPolicyInstIndex, policyAttrIndex;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, 
      XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
      (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
        (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);


  /* get the value from application */


  tempAttrPolicyIndex = keyPolicyIndexValue;
  tempAttrPolicyInstIndex = keyPolicyInstIndexValue;

  rc = usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT, keyPolicyIndexValue,
      keyPolicyInstIndexValue, 0,
      &tempAttrPolicyIndex, &
      tempAttrPolicyInstIndex,
      &policyAttrIndex);


  while((tempAttrPolicyIndex == keyPolicyIndexValue) && (
        tempAttrPolicyInstIndex == keyPolicyInstIndexValue) && (rc == L7_SUCCESS) )
  {
    if(usmDbDiffServPolicyAttrStmtEntryTypeGet(L7_UNIT_CURRENT, 
          tempAttrPolicyIndex,
          tempAttrPolicyInstIndex,
          policyAttrIndex, &entryType) 
        == L7_SUCCESS)
    {
      if (entryType ==  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT)
      {
        entryFound = XLIB_TRUE;
        objIsRedirectIntfPresent = L7_TRUE;
        owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsRedirectIntfPresent,
            sizeof (objIsRedirectIntfPresent));
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    }
    rc = usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT, keyPolicyIndexValue,
        keyPolicyInstIndexValue, 
        policyAttrIndex,
        &tempAttrPolicyIndex, &
        tempAttrPolicyInstIndex,
        &policyAttrIndex); 
  }


  objIsRedirectIntfPresent = L7_FALSE;

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsRedirectIntfPresent,
      sizeof (objIsRedirectIntfPresent));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/******************************************************************************
 *
 * @function fpObjGet_qosDiffServPolicyAttributeStmt_IsSimplePolicePresent
 *
 * @purpose Get 'IsSimplePolicePresent'
 *
 * @description [MirrorIntf]: To check if the IsSimplePolicePresent match 
 criteria is configured 
 *    for the given policy and class combination.
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_IsSimplePolicePresent(void *
    wap,
    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsSimplePolicePresent;
  L7_RC_t rc;
  xLibBool_t entryFound = XLIB_FALSE;


  xLibU32_t tempAttrPolicyIndex, tempAttrPolicyInstIndex, policyAttrIndex;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, 
      XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
      (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
        (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);


  /* get the value from application */


  tempAttrPolicyIndex = keyPolicyIndexValue;
  tempAttrPolicyInstIndex = keyPolicyInstIndexValue;

  rc = usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT, keyPolicyIndexValue,
      keyPolicyInstIndexValue, 0,
      &tempAttrPolicyIndex, &
      tempAttrPolicyInstIndex,
      &policyAttrIndex);


  while((tempAttrPolicyIndex == keyPolicyIndexValue) && (
        tempAttrPolicyInstIndex == keyPolicyInstIndexValue) && (rc == L7_SUCCESS) )
  {
    if(usmDbDiffServPolicyAttrStmtEntryTypeGet(L7_UNIT_CURRENT, 
          tempAttrPolicyIndex,
          tempAttrPolicyInstIndex,
          policyAttrIndex, &entryType) 
        == L7_SUCCESS)
    {
      if (entryType ==  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE)
      {
        entryFound = XLIB_TRUE;
        objIsSimplePolicePresent = L7_TRUE;
        owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsSimplePolicePresent,
            sizeof (objIsSimplePolicePresent));
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    }
    rc = usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT, keyPolicyIndexValue,
        keyPolicyInstIndexValue, 
        policyAttrIndex,
        &tempAttrPolicyIndex, &
        tempAttrPolicyInstIndex,
        &policyAttrIndex); 
  }


  objIsSimplePolicePresent = L7_FALSE;

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsSimplePolicePresent,
      sizeof (objIsSimplePolicePresent));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
 * @function fpObjGet_qosDiffServPolicyAttributeStmt_EntryType
 *
 * @purpose Get 'EntryType'
 *
 * @description [EntryType]: Policy attribute statement entry type -- this
 *              entry type is essential to determine which of the individual
 *              object(s) is defined for this policy attribute statement.
 *              This object must be created before any other policy 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_EntryDetails (void *wap,
    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  L7_uchar8 objEntryDetailsValue[512];
	xLibStr256_t tempEntryDetailsValue;
	xLibU32_t objEntryTypeValue;
	xLibU32_t val,u,s,p;
  L7_uint32 tempColorConformIndexValue;
  L7_uint32 objPoliceSimpleCrateValue;
  L7_uint32 objPoliceSimpleCburstValue;
  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_t objPoliceConformActValue;
  L7_uint32 objPoliceConformValValue;
  L7_uint32 objPoliceColorConformValValue;
  xLibU32_t objPoliceColorConformModeValue;
 
  xLibU32_t classIndexValueNext;
  L7_uint32   len = 0;
  L7_BOOL lookForNextAdd = L7_TRUE;

  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_t  objPoliceNonconformActValue;
  L7_uint32 objPoliceNonconformValValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
      (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
        (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
        (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrStmtEntryTypeGet (L7_UNIT_CURRENT,
        keyPolicyIndexValue,
        keyPolicyInstIndexValue,
        keyPolicyAttrIndexValue,
        &objEntryTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
	memset(objEntryDetailsValue,0x0,sizeof(objEntryDetailsValue));
	memset(tempEntryDetailsValue,0x0,sizeof(tempEntryDetailsValue));
	
  switch(objEntryTypeValue)
  {
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE:
			if (usmDbDiffServPolicyAttrStmtAssignQueueIdGet(L7_UNIT_CURRENT,keyPolicyIndexValue, keyPolicyInstIndexValue, 
keyPolicyAttrIndexValue,&val) == L7_SUCCESS)
    {
      osapiSnprintf(objEntryDetailsValue,sizeof(objEntryDetailsValue)," %s:%d ","Assign Queue",val );
		}
		else
		{
		  owa.rc = XLIBRC_FAILURE;
		}	

			break;
	  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_BANDWIDTH:
			break;
		case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
			osapiSnprintf(objEntryDetailsValue, sizeof(objEntryDetailsValue), " %s ","Packets Discarded");
			break;
		case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_EXPEDITE:
			break;
		case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL:
		     	
        if( usmDbDiffServPolicyAttrStmtMarkCosValGet (L7_UNIT_CURRENT,
                                              keyPolicyIndexValue,
                                              keyPolicyInstIndexValue,
                                              keyPolicyAttrIndexValue,
                                              &val) == L7_SUCCESS)
        {
          osapiSnprintf(objEntryDetailsValue,sizeof(objEntryDetailsValue)," %s:%d ", "CoS Value", val);
        }
        else
        {
          owa.rc = XLIBRC_FAILURE;           
        }
				break;
		case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2:
		     	
        if( usmDbDiffServPolicyAttrStmtMarkCosAsCos2Get(L7_UNIT_CURRENT,
                                              keyPolicyIndexValue,
                                              keyPolicyInstIndexValue,
                                              keyPolicyAttrIndexValue,
                                              &val) == L7_SUCCESS)
        {
          if(val == L7_TRUE)
            osapiSnprintf(objEntryDetailsValue,sizeof(objEntryDetailsValue)," %s", "CoS As Secondary CoS", val);
        }
        else
        {
          owa.rc = XLIBRC_FAILURE;           
        }
				break;
		case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL:
			break;
		case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL:
		 if (usmDbDiffServPolicyAttrStmtMarkIpDscpValGet(L7_UNIT_CURRENT,keyPolicyIndexValue, keyPolicyInstIndexValue, 
keyPolicyAttrIndexValue,&val) == L7_SUCCESS)
    {
      osapiSnprintf(objEntryDetailsValue,sizeof(objEntryDetailsValue)," %s:%d ","IP Dscp",val);
		}
		 else
		{
		  owa.rc = XLIBRC_FAILURE;
		}
			break;
		case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL:
			if (usmDbDiffServPolicyAttrStmtMarkIpPrecedenceValGet(L7_UNIT_CURRENT,keyPolicyIndexValue, keyPolicyInstIndexValue, 
keyPolicyAttrIndexValue,&val) == L7_SUCCESS)
    {
      osapiSnprintf(objEntryDetailsValue,sizeof(objEntryDetailsValue)," %s:%d ","IP Precedence",val);
		}
			else
		{
		  owa.rc = XLIBRC_FAILURE;
		}
		  break;
		case  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR:
			if (usmDbDiffServPolicyAttrStmtMirrorIntfGet(L7_UNIT_CURRENT,keyPolicyIndexValue, keyPolicyInstIndexValue, 
keyPolicyAttrIndexValue,&val) == L7_SUCCESS)
    {
    if(usmDbUnitSlotPortGet(val, &u, &s, &p) == L7_SUCCESS)
      {
        osapiSnprintf(objEntryDetailsValue,sizeof(objEntryDetailsValue)," %s:%d/%d/%d ","Mirror Interface",u,s,p);
    	}
		}
			else
		{
		  owa.rc = XLIBRC_FAILURE;
		}
			break;
		case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
      
      if(usmDbDiffServPolicyAttrStmtPoliceColorConformIndexGet (L7_UNIT_CURRENT,
        keyPolicyIndexValue,
        keyPolicyInstIndexValue,
        keyPolicyAttrIndexValue,
        &tempColorConformIndexValue) != L7_SUCCESS)
      {
        osapiStrncpy(objEntryDetailsValue, "Color Conform Mode: Color Blind; ",strlen("Color Conform Mode: Color Blind; ")); 
      }
      else
      {
        if ( tempColorConformIndexValue == 0 ) 
        {
          osapiStrncpy(objEntryDetailsValue, "Color Conform Mode: Color Blind; ",strlen("Color Conform Mode: Color Blind; ")); 
        }
        else
        {
          osapiStrncpy(objEntryDetailsValue, "Color Conform Mode: Color Aware ",strlen("Color Conform Mode: Color Aware "));          
          owa.l7rc = usmDbDiffServPolicyAttrStmtPoliceColorConformModeGet (L7_UNIT_CURRENT,
                                                  keyPolicyIndexValue,
                                                  keyPolicyInstIndexValue,
                                                  keyPolicyAttrIndexValue,
                                                  &objPoliceColorConformModeValue);
          if(owa.l7rc == L7_SUCCESS)
          {
            switch( objPoliceColorConformModeValue )
            {

                 case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS:
                       osapiStrncat(objEntryDetailsValue, "COS ",strlen("COS "));
                       break;
                  case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS2:   
                       osapiStrncat(objEntryDetailsValue, "COS2 ",strlen("COS2 "));
                       break;
                  case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP:
                       osapiStrncat(objEntryDetailsValue, "IP DSCP ",strlen("IP DSCP "));
                       break;
                  case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC:                 
                       osapiStrncat(objEntryDetailsValue, "IP Prec ",strlen("IP Prec "));
                       break;
                  default:
                       break;
            }
          }
          owa.l7rc =  usmDbDiffServPolicyAttrStmtPoliceColorConformValGet (L7_UNIT_CURRENT,
                                                         keyPolicyIndexValue,
                                                         keyPolicyInstIndexValue,
                                                         keyPolicyAttrIndexValue,
                                                         &objPoliceColorConformValValue);          
          if(owa.l7rc == L7_SUCCESS)       
          {
             memset(tempEntryDetailsValue, 0x00, sizeof(tempEntryDetailsValue));
             osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue), "%d ", objPoliceColorConformValValue);
             osapiStrncat(objEntryDetailsValue, tempEntryDetailsValue,strlen(tempEntryDetailsValue));
             memset(tempEntryDetailsValue, 0x00, sizeof(tempEntryDetailsValue));
          }

          classIndexValueNext = 0;
          owa.l7rc =  usmDbDiffServPolicyAttrStmtPoliceColorConformIndexGet (L7_UNIT_CURRENT,
                                   keyPolicyIndexValue,
                                   keyPolicyInstIndexValue,
                                   keyPolicyAttrIndexValue,
                                   &tempColorConformIndexValue);

   	      while (lookForNextAdd == L7_TRUE)
	        {   
	          if (usmDbDiffServClassGetNext(L7_UNIT_CURRENT, classIndexValueNext, 
	                                          &classIndexValueNext) == L7_SUCCESS)
	          {
	            if((usmDbDiffServPolicyAttrStmtPoliceColorAwareClassValidate(L7_UNIT_CURRENT, 
	                                    keyPolicyIndexValue,
	                                    keyPolicyInstIndexValue,
	                                    classIndexValueNext)  == L7_SUCCESS )&& (tempColorConformIndexValue == classIndexValueNext))
	            {
	              len = sizeof(tempEntryDetailsValue);
	              usmDbDiffServClassNameGet(L7_UNIT_CURRENT, classIndexValueNext, 
	                        tempEntryDetailsValue, &len);
                osapiStrncat( objEntryDetailsValue, tempEntryDetailsValue,strlen(tempEntryDetailsValue));
                osapiStrncat( objEntryDetailsValue, "; ",strlen("; "));
	              memset(tempEntryDetailsValue, 0, sizeof(tempEntryDetailsValue));
                break;
	            }
	          }
	          else
	          {
	            lookForNextAdd = L7_FALSE;
	          }
	        }
        }
      }
     
      if( usmDbDiffServPolicyAttrStmtPoliceSimpleCrateGet (L7_UNIT_CURRENT,
                                                     keyPolicyIndexValue,
                                                     keyPolicyInstIndexValue,
                                                     keyPolicyAttrIndexValue,
                                                     &objPoliceSimpleCrateValue)== L7_SUCCESS)
      {
        osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue), "%s:%d; ", "Committed Rate (Kbps)",objPoliceSimpleCrateValue);
        strcat(objEntryDetailsValue, tempEntryDetailsValue); 
        memset(tempEntryDetailsValue, 0x00, sizeof(tempEntryDetailsValue));
      }           
      if( usmDbDiffServPolicyAttrStmtPoliceSimpleCburstGet (L7_UNIT_CURRENT,
                                                      keyPolicyIndexValue,
                                                      keyPolicyInstIndexValue,
                                                      keyPolicyAttrIndexValue,
                                                      &objPoliceSimpleCburstValue) == L7_SUCCESS)
      {
        osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue), "%s:%d; ","Committed Burst Size (KB)",objPoliceSimpleCburstValue);
        strcat(objEntryDetailsValue, tempEntryDetailsValue); 
        memset(tempEntryDetailsValue, 0x00, sizeof(tempEntryDetailsValue));
      }

      if(usmDbDiffServPolicyAttrStmtPoliceConformActGet (L7_UNIT_CURRENT,
                                                    keyPolicyIndexValue,
                                                    keyPolicyInstIndexValue,
                                                    keyPolicyAttrIndexValue,
                                                    &objPoliceConformActValue) == L7_SUCCESS)
      {   
        strcat( objEntryDetailsValue, "Conform Action:");
        if (usmDbDiffServPolicyAttrStmtPoliceConformValGet (L7_UNIT_CURRENT,
                                                    keyPolicyIndexValue,
                                                    keyPolicyInstIndexValue,
                                                    keyPolicyAttrIndexValue,
                                                    &objPoliceConformValValue)== L7_SUCCESS ||
             objPoliceConformActValue == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2)
        {
          switch(objPoliceConformActValue)
          {
            case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP:
              osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s; ", "Drop");     
              break;
           
            case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS:
              osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s %d; ", "Mark CoS", objPoliceConformValValue);     
              break;

            case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2:
              osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s;", "Mark CoS As Secondary CoS");     
              break;

            case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2:
              osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s %d; ", "Mark CoS2", objPoliceConformValValue);     
            break;
          
          case  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP:
            osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s %d; ", "DSCP", objPoliceConformValValue);     
            break;
          
          case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC:
            osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s %d; ", "Precedence", objPoliceConformValValue);     
            break;
          
          case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND:
            osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s; ", "Send");     
            break;

          case  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_TOTAL:
            break;

          case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_NONE:
          default:
            osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s; ", "None"); 
            break;
        }
        strcat(objEntryDetailsValue, tempEntryDetailsValue); 
        memset(tempEntryDetailsValue, 0x00, sizeof(tempEntryDetailsValue));
      } 
      } 
          
       if(usmDbDiffServPolicyAttrStmtPoliceNonconformActGet (L7_UNIT_CURRENT,
                                                       keyPolicyIndexValue,
                                                       keyPolicyInstIndexValue,
                                                       keyPolicyAttrIndexValue,
                                                       &objPoliceNonconformActValue)== L7_SUCCESS)
 
      {   
        strcat( objEntryDetailsValue, "Violate Action:");
        if (usmDbDiffServPolicyAttrStmtPoliceNonconformValGet (L7_UNIT_CURRENT,
                                                       keyPolicyIndexValue,
                                                       keyPolicyInstIndexValue,
                                                       keyPolicyAttrIndexValue,
                                                       &objPoliceNonconformValValue)== L7_SUCCESS ||
             objPoliceNonconformActValue == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2)
        {
          switch(objPoliceNonconformActValue)
          {
            case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP:
              osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s; ", "Drop");     
              break;
           
            case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS:
              osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s %d; ", "Mark CoS", objPoliceNonconformValValue);     
              break;

            case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2:
              osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s;", "Mark CoS as Secondary CoS");     
              break;

            case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2:
              osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s %d; ", "Mark CoS2", objPoliceNonconformValValue);     
              break;
            
            case  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP:
              osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s %d; ", "DSCP", objPoliceNonconformValValue);     
              break;
            
            case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC:
              osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s %d; ", "Precedence", objPoliceNonconformValValue);     
              break;
            
            case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND:
              osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s; ", "Send");     
              break;

            case  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_TOTAL:
              break;

            case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_NONE:
            default:
              osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s; ", "None"); 
              break;
          }
          strcat(objEntryDetailsValue, tempEntryDetailsValue); 
          memset(tempEntryDetailsValue, 0x00, sizeof(tempEntryDetailsValue));
        }
      } 
			break;
    
		case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
			break;
		case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
			if(usmDbDiffServPolicyAttrStmtPoliceColorConformIndexGet (L7_UNIT_CURRENT,
						keyPolicyIndexValue,
						keyPolicyInstIndexValue,
						keyPolicyAttrIndexValue,
						&tempColorConformIndexValue) != L7_SUCCESS)
			{
				osapiStrncpy(objEntryDetailsValue, "Color Conform Mode: Color Blind; ",strlen("Color Conform Mode: Color Blind; "));
			}
			else
			{
				if ( tempColorConformIndexValue == 0 )
				{
					osapiStrncpy(objEntryDetailsValue, "Color Conform Mode: Color Blind; ",strlen("Color Conform Mode: Color Blind; "));
				}
				else
				{
					osapiStrncpy(objEntryDetailsValue, "Color Conform Mode: Color Aware ",strlen("Color Conform Mode: Color Aware "));
					owa.l7rc = usmDbDiffServPolicyAttrStmtPoliceColorConformModeGet (L7_UNIT_CURRENT,
							keyPolicyIndexValue,
							keyPolicyInstIndexValue,
							keyPolicyAttrIndexValue,
							&objPoliceColorConformModeValue);
					if(owa.l7rc == L7_SUCCESS)
					{
						switch( objPoliceColorConformModeValue )
						{

							case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS:
								osapiStrncat(objEntryDetailsValue, "COS ",strlen("COS "));
								break;
							case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS2:
								osapiStrncat(objEntryDetailsValue, "COS2 ",strlen("COS2 "));
								break;
							case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP:
								osapiStrncat(objEntryDetailsValue, "IP DSCP ",strlen("IP DSCP "));
								break;
							case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC:
								osapiStrncat(objEntryDetailsValue, "IP Prec ",strlen("IP Prec "));
								break;
							default:
								break;
						}
					}
					owa.l7rc =  usmDbDiffServPolicyAttrStmtPoliceColorConformValGet (L7_UNIT_CURRENT,
							keyPolicyIndexValue,
							keyPolicyInstIndexValue,
							keyPolicyAttrIndexValue,
							&objPoliceColorConformValValue);
					if(owa.l7rc == L7_SUCCESS)
					{
						memset(tempEntryDetailsValue, 0x00, sizeof(tempEntryDetailsValue));
						osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue), "%d ", objPoliceColorConformValValue);
						osapiStrncat(objEntryDetailsValue, tempEntryDetailsValue,strlen(tempEntryDetailsValue));
						memset(tempEntryDetailsValue, 0x00, sizeof(tempEntryDetailsValue));
					}
					classIndexValueNext = 0;
					owa.l7rc =  usmDbDiffServPolicyAttrStmtPoliceColorConformIndexGet (L7_UNIT_CURRENT,
							keyPolicyIndexValue,
							keyPolicyInstIndexValue,
							keyPolicyAttrIndexValue,
							&tempColorConformIndexValue);

					while (lookForNextAdd == L7_TRUE)
					{
						if (usmDbDiffServClassGetNext(L7_UNIT_CURRENT, classIndexValueNext,
									&classIndexValueNext) == L7_SUCCESS)
						{
							if((usmDbDiffServPolicyAttrStmtPoliceColorAwareClassValidate(L7_UNIT_CURRENT,
											keyPolicyIndexValue,
											keyPolicyInstIndexValue,
											classIndexValueNext)  == L7_SUCCESS )&& (tempColorConformIndexValue == classIndexValueNext))
							{
								len = sizeof(tempEntryDetailsValue);
								if (usmDbDiffServClassNameGet(L7_UNIT_CURRENT, classIndexValueNext,
										tempEntryDetailsValue, &len) == L7_SUCCESS)
                            {
    								osapiStrncat( objEntryDetailsValue, tempEntryDetailsValue,strlen(tempEntryDetailsValue));
    								osapiStrncat( objEntryDetailsValue, "; ",strlen("; "));
								}
								memset(tempEntryDetailsValue, 0, sizeof(tempEntryDetailsValue));
								break;
							}
						}
						else
						{
							lookForNextAdd = L7_FALSE;
						}
					}
				}
			}
                         if(usmDbDiffServPolicyAttrStmtPoliceColorExceedIndexGet (L7_UNIT_CURRENT,
						 keyPolicyIndexValue,
						 keyPolicyInstIndexValue,
						 keyPolicyAttrIndexValue,
						 &tempColorConformIndexValue) != L7_SUCCESS)
			 {
				 osapiStrncat(objEntryDetailsValue, "Color Exceed Mode: Unspecified; ",strlen("Color Exceed Mode: Unspecified; "));
			 }
			 else
			 {
				 if ( tempColorConformIndexValue == 0 )
				 {
					 osapiStrncat(objEntryDetailsValue, "Color Exceed Mode: Color Blind; ",strlen("Color Exceed Mode: Color Blind; "));
				 }
				 else
				 {
					 osapiStrncat(objEntryDetailsValue, "Color Exceed Mode: Color Aware ",strlen("Color Exceed Mode: Color Aware "));
					 owa.l7rc = usmDbDiffServPolicyAttrStmtPoliceColorExceedModeGet (L7_UNIT_CURRENT,
							 keyPolicyIndexValue,
							 keyPolicyInstIndexValue,
							 keyPolicyAttrIndexValue,
							 &objPoliceColorConformModeValue);
					 if(owa.l7rc == L7_SUCCESS)
					 {
						 switch( objPoliceColorConformModeValue )
						 {

							 case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS:
								 osapiStrncat(objEntryDetailsValue, "COS ",strlen("COS "));
								 break;
							 case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS2:
								 osapiStrncat(objEntryDetailsValue, "COS2 ",strlen("COS2 "));
								 break;
							 case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP:
								 osapiStrncat(objEntryDetailsValue, "IP DSCP ",strlen("IP DSCP "));
								 break;
							 case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC:
								 osapiStrncat(objEntryDetailsValue, "IP Prec ",strlen("IP Prec "));
								 break;
							 default:
								 break;
						 }
					 }
					 owa.l7rc =  usmDbDiffServPolicyAttrStmtPoliceColorExceedValGet(L7_UNIT_CURRENT,
							 keyPolicyIndexValue,
							 keyPolicyInstIndexValue,
							 keyPolicyAttrIndexValue,
							 &objPoliceColorConformValValue);
					 if(owa.l7rc == L7_SUCCESS)
					 {
						 memset(tempEntryDetailsValue, 0x00, sizeof(tempEntryDetailsValue));
						 osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue), "%d ", objPoliceColorConformValValue);
						 osapiStrncat(objEntryDetailsValue, tempEntryDetailsValue,strlen(tempEntryDetailsValue));
						 memset(tempEntryDetailsValue, 0x00, sizeof(tempEntryDetailsValue));
					 }

					 classIndexValueNext = 0;
					 owa.l7rc =  usmDbDiffServPolicyAttrStmtPoliceColorExceedIndexGet(L7_UNIT_CURRENT,
							 keyPolicyIndexValue,
							   keyPolicyInstIndexValue,
							   keyPolicyAttrIndexValue,
							   &tempColorConformIndexValue);

					   while (lookForNextAdd == L7_TRUE)
					   {
						   if (usmDbDiffServClassGetNext(L7_UNIT_CURRENT, classIndexValueNext,
									   &classIndexValueNext) == L7_SUCCESS)
						   {
							   if((usmDbDiffServPolicyAttrStmtPoliceColorAwareClassValidate(L7_UNIT_CURRENT,
											   keyPolicyIndexValue,
											   keyPolicyInstIndexValue,
											   classIndexValueNext)  == L7_SUCCESS )&& (tempColorConformIndexValue == classIndexValueNext))
							   {
								   len = sizeof(tempEntryDetailsValue);
								   if (usmDbDiffServClassNameGet(L7_UNIT_CURRENT, classIndexValueNext,
										   tempEntryDetailsValue, &len) == L7_SUCCESS)
                               {										   
    								   osapiStrncat( objEntryDetailsValue, tempEntryDetailsValue,strlen(tempEntryDetailsValue));
    								   osapiStrncat( objEntryDetailsValue, "; ",strlen("; "));
								   }
								   memset(tempEntryDetailsValue, 0, sizeof(tempEntryDetailsValue));
								   break;
							   }
						   }
						   else
						   {
							   lookForNextAdd = L7_FALSE;
						   }
					   }
				 }
			 }

			if( usmDbDiffServPolicyAttrStmtPoliceTworateCrateGet(L7_UNIT_CURRENT,
						keyPolicyIndexValue,
						keyPolicyInstIndexValue,
						keyPolicyAttrIndexValue,
						&objPoliceSimpleCrateValue)== L7_SUCCESS)
			{
				osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue), "%s:%d; ", "Committed Rate (Kbps)",objPoliceSimpleCrateValue);
				strcat(objEntryDetailsValue, tempEntryDetailsValue);
				memset(tempEntryDetailsValue, 0x00, sizeof(tempEntryDetailsValue));
			}
			if( usmDbDiffServPolicyAttrStmtPoliceTworateCburstGet (L7_UNIT_CURRENT,
						keyPolicyIndexValue,
						keyPolicyInstIndexValue,
						keyPolicyAttrIndexValue,
						&objPoliceSimpleCburstValue) == L7_SUCCESS)
			{
				osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue), "%s:%d; ","Committed Burst Size (KB)",objPoliceSimpleCburstValue);
				strcat(objEntryDetailsValue, tempEntryDetailsValue);
				memset(tempEntryDetailsValue, 0x00, sizeof(tempEntryDetailsValue));
			}
                         if( usmDbDiffServPolicyAttrStmtPoliceTworatePrateGet(L7_UNIT_CURRENT,
                                                keyPolicyIndexValue,
                                                keyPolicyInstIndexValue,
                                                keyPolicyAttrIndexValue,
                                                &objPoliceSimpleCrateValue)== L7_SUCCESS)
                        {
                                osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue), "%s:%d; ", "Peak Rate (Kbps)",objPoliceSimpleCrateValue);
                                strcat(objEntryDetailsValue, tempEntryDetailsValue);
                                memset(tempEntryDetailsValue, 0x00, sizeof(tempEntryDetailsValue));
                        }
                        if( usmDbDiffServPolicyAttrStmtPoliceTworatePburstGet (L7_UNIT_CURRENT,
                                                keyPolicyIndexValue,
                                                keyPolicyInstIndexValue,
                                                keyPolicyAttrIndexValue,
                                                &objPoliceSimpleCburstValue) == L7_SUCCESS)
                        {
                                osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue), "%s:%d; ","Peak Burst Size (KB)",objPoliceSimpleCburstValue);
                                strcat(objEntryDetailsValue, tempEntryDetailsValue);
                                memset(tempEntryDetailsValue, 0x00, sizeof(tempEntryDetailsValue));
                        }


			if(usmDbDiffServPolicyAttrStmtPoliceConformActGet (L7_UNIT_CURRENT,
						keyPolicyIndexValue,
						keyPolicyInstIndexValue,
						keyPolicyAttrIndexValue,
						&objPoliceConformActValue) == L7_SUCCESS)
			{
				strcat( objEntryDetailsValue, "Conform Action:");
				if (usmDbDiffServPolicyAttrStmtPoliceConformValGet (L7_UNIT_CURRENT,
						keyPolicyIndexValue,
						keyPolicyInstIndexValue,
						keyPolicyAttrIndexValue,
						&objPoliceConformValValue) == L7_SUCCESS ||
                   objPoliceConformActValue == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2)
              {
    				switch(objPoliceConformActValue)
    				{
    					case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP:
    						osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s; ", "Drop");
    						break;

    					case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS:
    						osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s %d; ", "Mark CoS", objPoliceConformValValue);
    						break;

    					case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2:
    						osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s %d; ", "Mark CoS2", objPoliceConformValValue);
    						break;

                      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2:
                        osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s;", "Mark CoS As Secondary CoS");     
                        break;

    					case  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP:
    						osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s %d; ", "DSCP", objPoliceConformValValue);
    						break;

    					case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC:
    						osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s %d; ", "Precedence", objPoliceConformValValue);
    						break;

    					case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND:
    						osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s; ", "Send");
    						break;

    					case  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_TOTAL:
    						break;

    					case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_NONE:
    					default:
    						osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s; ", "None");
    						break;
    				}
    				strcat(objEntryDetailsValue, tempEntryDetailsValue);
    				memset(tempEntryDetailsValue, 0x00, sizeof(tempEntryDetailsValue));
				}
			}
                         if(usmDbDiffServPolicyAttrStmtPoliceExceedActGet (L7_UNIT_CURRENT,
                                                keyPolicyIndexValue,
                                                keyPolicyInstIndexValue,
                                                keyPolicyAttrIndexValue,
                                                &objPoliceConformActValue) == L7_SUCCESS)
                        {
                                strcat( objEntryDetailsValue, "Exceed Action:");
                                if (usmDbDiffServPolicyAttrStmtPoliceExceedValGet (L7_UNIT_CURRENT,
                                                keyPolicyIndexValue,
                                                keyPolicyInstIndexValue,
                                                keyPolicyAttrIndexValue,
                                                &objPoliceConformValValue) == L7_SUCCESS ||
                                     objPoliceConformActValue == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2)
                                {  
                                  switch(objPoliceConformActValue)
                                  {
                                    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP:
                                            osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s; ", "Drop");
                                            break;

                                    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS:
                                            osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s %d; ", "Mark CoS", objPoliceConformValValue);
                                            break;

                                    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2:
                                            osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s %d; ", "Mark CoS2", objPoliceConformValValue);
                                            break;

                                    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2:
                                            osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s;", "Mark CoS As Secondary CoS");     
                                            break;

                                    case  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP:
                                            osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s %d; ", "DSCP", objPoliceConformValValue);
                                            break;

                                    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC:
                                            osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s %d; ", "Precedence", objPoliceConformValValue);
                                            break;

                                    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND:
                                            osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s; ", "Send");
                                            break;

                                    case  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_TOTAL:
                                            break;

                                    case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_NONE:
                                    default:
                                            osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s; ", "None");
                                            break;
                            }
                            strcat(objEntryDetailsValue, tempEntryDetailsValue);
                            memset(tempEntryDetailsValue, 0x00, sizeof(tempEntryDetailsValue));
                          }
                        }

          
       if(usmDbDiffServPolicyAttrStmtPoliceNonconformActGet (L7_UNIT_CURRENT,
                                                       keyPolicyIndexValue,
                                                       keyPolicyInstIndexValue,
                                                       keyPolicyAttrIndexValue,
                                                       &objPoliceNonconformActValue) == L7_SUCCESS)
 
      {   
        strcat( objEntryDetailsValue, "Violate Action:");
				if (usmDbDiffServPolicyAttrStmtPoliceNonconformValGet (L7_UNIT_CURRENT,
                                                       keyPolicyIndexValue,
                                                       keyPolicyInstIndexValue,
                                                       keyPolicyAttrIndexValue,
						&objPoliceNonconformValValue) == L7_SUCCESS ||
                   objPoliceNonconformActValue == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2)
        {
    				switch(objPoliceNonconformActValue)
    				{
    					case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP:
    						osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s; ", "Drop");
    						break;

    					case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS:
    						osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s %d; ", "Mark CoS", objPoliceNonconformValValue);
    						break;

    					case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2:
    						osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s %d; ", "Mark CoS2", objPoliceNonconformValValue);
    						break;

                      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2:
                        osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s;", "Mark CoS As Secondary CoS");     
            break;
          
          case  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP:
            osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s %d; ", "DSCP", objPoliceNonconformValValue);     
            break;
          
          case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC:
            osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s %d; ", "Precedence", objPoliceNonconformValValue);     
            break;
          
          case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND:
            osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s; ", "Send");     
            break;

          case  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_TOTAL:
            break;

          case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_NONE:
          default:
            osapiSnprintf( tempEntryDetailsValue, sizeof(tempEntryDetailsValue),"%s; ", "None"); 
            break;
        }
        strcat(objEntryDetailsValue, tempEntryDetailsValue); 
        memset(tempEntryDetailsValue, 0x00, sizeof(tempEntryDetailsValue));
				}
			}
			break;
		case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_RANDOMDROP:
			break;
		case  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT:
			if (usmDbDiffServPolicyAttrStmtRedirectIntfGet(L7_UNIT_CURRENT,keyPolicyIndexValue, keyPolicyInstIndexValue, 
keyPolicyAttrIndexValue,&val) == L7_SUCCESS)
    {
    if(usmDbUnitSlotPortGet(val, &u, &s, &p) == L7_SUCCESS)
      {
        osapiSnprintf(objEntryDetailsValue,sizeof(objEntryDetailsValue)," %s:%d/%d/%d ","Redirect Interface",u,s,p);
    	}
		}
			else
		{
		  owa.rc = XLIBRC_FAILURE;
		}
			break;
		case  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_SHAPE_AVERAGE:
			break;
		case  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_SHAPE_PEAK:
			break;
		default:
			break;
												
  }
	if(owa.rc == XLIBRC_SUCCESS)
	{
	
  /* return the object value: EntryType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objEntryDetailsValue,
      sizeof (objEntryDetailsValue));
	}
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/******************************************************************************
*
* @function fpObjSet_qosDiffServPolicyAttributeStmt_PoliceColorConformIndex
*
* @purpose Set 'PoliceColorConformIndex'
*
* @description [PoliceColorConformIndex]: Index of the DiffServ Class Table
*              row whose class definition is used to specify the policing
*              color conform mode and value. This identifies incoming traffic
*              categorized as 'green' packets in the network. This attribute
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_qosDiffServPolicyAttributeStmt_ClassColorType (
                                                                   void *wap, 
void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objClassColorType;

  FPOBJ_TRACE_ENTER (bufp);

    /* retrieve object: PoliceColorConformIndex */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objClassColorType, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objClassColorType, owa.len);


  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, 
XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
                   (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
                   (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* get the value from application */


  if (objClassColorType  == L7_XUI_DIFFSERV_COLOR_BLIND)
  {
    owa.l7rc = usmDbDiffServPolicyAttrStmtPoliceColorConformIndexSet(
L7_UNIT_CURRENT,
                                                                               
         keyPolicyIndexValue,
                                                                               
         keyPolicyInstIndexValue,
                                                                               
         keyPolicyAttrIndexValue,
                                                                               
         0);
  }
  else
  {
    owa.l7rc = L7_SUCCESS;
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

xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_ClassColorType(void *wap, 
void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objClassColorType ;
	
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;

 fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;

		fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;

   xLibU32_t tempColorConformIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

   /* retrieve key: PolicyIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                          (xLibU8_t *) & keyPolicyIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa.len);

     /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
        (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

	    /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
        (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

	

	owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceColorConformIndexGet (L7_UNIT_CURRENT,
        keyPolicyIndexValue,
        keyPolicyInstIndexValue,
        keyPolicyAttrIndexValue,
        &tempColorConformIndexValue);
  if ( (owa.l7rc != L7_SUCCESS) ||(tempColorConformIndexValue == 0 ) )
  {
     objClassColorType = L7_XUI_DIFFSERV_COLOR_BLIND;
	  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objClassColorType,
	 	                                       sizeof(objClassColorType) );
   	  FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }
  
   objClassColorType = L7_XUI_DIFFSERV_COLOR_AWARE;
   owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objClassColorType,
	 	                                       sizeof(objClassColorType) );
   FPOBJ_TRACE_EXIT (bufp, owa);
   return owa.rc;
}



xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_IsColorAwareClassPresent(
void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsColorAwareClassPresent  ;
	
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;

 fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;


   xLibU32_t classIndexValueNext;

	L7_BOOL lookForNextAdd = L7_TRUE;
	
  FPOBJ_TRACE_ENTER (bufp);

   /* retrieve key: PolicyIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                          (xLibU8_t *) & keyPolicyIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa.len);

     /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
        (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);
	
  classIndexValueNext = 0;

  while (lookForNextAdd == L7_TRUE)
  {   
	  if (usmDbDiffServClassGetNext(L7_UNIT_CURRENT, classIndexValueNext, 
			                                                           &
classIndexValueNext) == L7_SUCCESS)
	  {
	      if(usmDbDiffServPolicyAttrStmtPoliceColorAwareClassValidate(
L7_UNIT_CURRENT, 
					                                      keyPolicyIndexValue,
					                                      keyPolicyInstIndexValue,
					                                      classIndexValueNext)  == 
L7_SUCCESS )
	      {
	          objIsColorAwareClassPresent = L7_TRUE;
	          
			   owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objIsColorAwareClassPresent,
				 	                                       sizeof(
objIsColorAwareClassPresent) );
  			  FPOBJ_TRACE_EXIT (bufp, owa);
		     return owa.rc;
	      }
  	 }
	 else
    {
      lookForNextAdd = L7_FALSE;
    }
  }
	
   objIsColorAwareClassPresent = L7_FALSE;
	          
   owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objIsColorAwareClassPresent,
	 	                                       sizeof(objIsColorAwareClassPresent) 
);
   FPOBJ_TRACE_EXIT (bufp, owa);
   return owa.rc;

}


xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_ColorAwareClassList(void *wap
, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t nextColorAwareClassList  ;
	
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;

 fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;

	fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;




   xLibU32_t classIndexValueNext;
	xLibU32_t tempColorConformIndexValue;
   L7_uint32   len = 0;

	L7_BOOL lookForNextAdd = L7_TRUE;
	
  FPOBJ_TRACE_ENTER (bufp);

   /* retrieve key: PolicyIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                          (xLibU8_t *) & keyPolicyIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa.len);

     /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
        (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

    /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
        (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);
	
  classIndexValueNext = 0;

    owa.l7rc =
    usmDbDiffServPolicyAttrStmtPoliceColorConformIndexGet (L7_UNIT_CURRENT,
        keyPolicyIndexValue,
        keyPolicyInstIndexValue,
        keyPolicyAttrIndexValue,
        &tempColorConformIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {

		while (lookForNextAdd == L7_TRUE)
		  {   
			  if (usmDbDiffServClassGetNext(L7_UNIT_CURRENT, classIndexValueNext, 
					                                                &classIndexValueNext) == L7_SUCCESS)
			  {
			      if(usmDbDiffServPolicyAttrStmtPoliceColorAwareClassValidate(
L7_UNIT_CURRENT, 
							                                      keyPolicyIndexValue,
							                                      keyPolicyInstIndexValue,
							                                      classIndexValueNext)  == L7_SUCCESS )
			      {
			          len = sizeof(nextColorAwareClassList);
						memset(nextColorAwareClassList, 0, sizeof(nextColorAwareClassList));
			          usmDbDiffServClassNameGet(L7_UNIT_CURRENT, classIndexValueNext, 
									                    nextColorAwareClassList, &len);
					   owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextColorAwareClassList,len);
		  			  FPOBJ_TRACE_EXIT (bufp, owa);
				     return owa.rc;
			      }
		  	 }
			 else
		    {
		      lookForNextAdd = L7_FALSE;
		    }
		  }
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */	
		  FPOBJ_TRACE_EXIT (bufp, owa);
		  return owa.rc;

  }


  while (lookForNextAdd == L7_TRUE)
  {   
	  if (usmDbDiffServClassGetNext(L7_UNIT_CURRENT, classIndexValueNext, 
			                                                           &classIndexValueNext) == L7_SUCCESS)
	  {
	      if( (usmDbDiffServPolicyAttrStmtPoliceColorAwareClassValidate(
L7_UNIT_CURRENT, 
					                                      keyPolicyIndexValue,
					                                      keyPolicyInstIndexValue,
					                                      classIndexValueNext)  == L7_SUCCESS ) &&
				 (tempColorConformIndexValue == classIndexValueNext) )
	      {
	          len = sizeof(nextColorAwareClassList);
				memset(nextColorAwareClassList, 0, sizeof(nextColorAwareClassList));
	          usmDbDiffServClassNameGet(L7_UNIT_CURRENT, classIndexValueNext, 
							                    nextColorAwareClassList, &len);
			   owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextColorAwareClassList,len);
  			  FPOBJ_TRACE_EXIT (bufp, owa);
		     return owa.rc;
	      }
  	 }
	 else
    {
      lookForNextAdd = L7_FALSE;
    }
  }
	owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

xLibRC_t fpObjSet_qosDiffServPolicyAttributeStmt_ColorAwareClassList(void *wap
, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objColorAwareClassList;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;

  L7_uint32   classIndexValueTemp;

	fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

   /* retrieve key: PolicyIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                          (xLibU8_t *) & keyPolicyIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa.len);

   /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
        (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
        (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  /* retrieve object: AddToMemberClassList */
  memset(objColorAwareClassList, 0x00, sizeof(objColorAwareClassList));
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *)  objColorAwareClassList, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objColorAwareClassList, owa.len);

  owa.l7rc = usmDbDiffServClassNameToIndex(L7_UNIT_CURRENT, 
                                       objColorAwareClassList,
                                      &classIndexValueTemp);
  if (owa.l7rc != L7_SUCCESS)
   {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
   }


	owa.l7rc = usmDbDiffServPolicyAttrStmtPoliceColorConformIndexSet(
L7_UNIT_CURRENT,
keyPolicyIndexValue, 
keyPolicyInstIndexValue, 
keyPolicyAttrIndexValue,
                                                                0);
   if(owa.l7rc == L7_SUCCESS)
   {
	    /* set the conform color class index in the policing policy attr,
         * which will cause the corresponding color mode and value to be set
         * (we just set the index to 0, so no need to set it to 0 again)
         */
          owa.l7rc = usmDbDiffServPolicyAttrStmtPoliceColorConformIndexSet(
L7_UNIT_CURRENT,
keyPolicyIndexValue, 
keyPolicyInstIndexValue, 
keyPolicyAttrIndexValue,
classIndexValueTemp);
   	}


  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


xLibRC_t fpObjList_qosDiffServPolicyAttributeStmt_ColorAwareClassList (void *wap, 
void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objColorAwareClassList;
  xLibStr256_t nextColorAwareClassList ;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;

   fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;

   xLibU32_t classIndexValueNext;
   L7_uint32   len = 0;

	xLibBool_t isFirstTime = XLIB_FALSE;
	xLibBool_t entryFound = XLIB_FALSE;
	L7_BOOL lookForNextAdd = L7_TRUE;

  /* retrieve key: PolicyIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                          (xLibU8_t *) & keyPolicyIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa.len);

    /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
        (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);
	


  memset(objColorAwareClassList, 0x00, sizeof(objColorAwareClassList ));

  owa.rc = xLibFilterGet (wap, 
XOBJ_qosDiffServPolicyAttributeStmt_ColorAwareClassList ,
                        (xLibU8_t *) objColorAwareClassList, &owa.len );
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(objColorAwareClassList, 0x00, sizeof(objColorAwareClassList));
	isFirstTime = XLIB_TRUE;
  }

  /* Get the next valid class name */
 /*1 Convert name to index. 2. get Next valid index. 3. convert classindex to 
class name. push to fileter
	*/

  if(isFirstTime == XLIB_TRUE )
 {
	classIndexValueNext = 0;
  }
  else
  {
     /*get the index of the class retrieved by the filterget */
	 owa.l7rc = usmDbDiffServClassNameToIndex(L7_UNIT_CURRENT, 
                                       objColorAwareClassList,
                                      &classIndexValueNext);
	  if (owa.l7rc != L7_SUCCESS)
     {
        owa.rc = XLIBRC_ENDOF_TABLE;
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
     }
   
  }

  while (lookForNextAdd == L7_TRUE)
  {   
	  if (usmDbDiffServClassGetNext(L7_UNIT_CURRENT, classIndexValueNext, 
			                                         &classIndexValueNext) == L7_SUCCESS)
	  {
	      if(usmDbDiffServPolicyAttrStmtPoliceColorAwareClassValidate(
L7_UNIT_CURRENT, 
					                                      keyPolicyIndexValue,
					                                      keyPolicyInstIndexValue,
					                                      classIndexValueNext)  == L7_SUCCESS )
	      {
	         len = sizeof(nextColorAwareClassList);
	         memset(nextColorAwareClassList, 0x00, sizeof(nextColorAwareClassList));
	         usmDbDiffServClassNameGet(L7_UNIT_CURRENT, classIndexValueNext, 
					 	       nextColorAwareClassList, &len);
             entryFound = XLIB_TRUE;
			  break;
	      }
  	 }
	 else
    {
      lookForNextAdd = L7_FALSE;
    }
  }
	
  if(entryFound != XLIB_TRUE )
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextColorAwareClassList, 
		                                      strlen(nextColorAwareClassList));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/******************************************************************************
*
* @function fpObjGet_qosDiffServPolicyAttributeStmt_ColorExceedClassList
*
* @purpose Set 'ColorExceedClassList'
*
*
*
* @return
*******************************************************************************/


xLibRC_t fpObjGet_qosDiffServPolicyAttributeStmt_ColorExceedClassList(void *wap
, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t nextColorExceedClassList  ;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;

  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;

  xLibU32_t classIndexValueNext;
  xLibU32_t tempColorExceedIndexValue;
  L7_uint32   len = 0;

  L7_BOOL lookForNextAdd = L7_TRUE;

  FPOBJ_TRACE_ENTER (bufp);

   /* retrieve key: PolicyIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                          (xLibU8_t *) & keyPolicyIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa.len);

     /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
        (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }

   FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

    /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
        (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  classIndexValueNext = 0;

  owa.l7rc =
	  usmDbDiffServPolicyAttrStmtPoliceColorExceedIndexGet (L7_UNIT_CURRENT,
			  keyPolicyIndexValue,
			  keyPolicyInstIndexValue,
			  keyPolicyAttrIndexValue,
			  &tempColorExceedIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {

	  while (lookForNextAdd == L7_TRUE)
	  {
		  if (usmDbDiffServClassGetNext(L7_UNIT_CURRENT, classIndexValueNext,
					  &classIndexValueNext) == L7_SUCCESS)
		  {
			  if(usmDbDiffServPolicyAttrStmtPoliceColorAwareClassValidate(
						  L7_UNIT_CURRENT,
						  keyPolicyIndexValue,
						  keyPolicyInstIndexValue,
						  classIndexValueNext)  == L7_SUCCESS )
			  {
				  len = sizeof(nextColorExceedClassList);
				  memset(nextColorExceedClassList, 0, sizeof(nextColorExceedClassList));
				  usmDbDiffServClassNameGet(L7_UNIT_CURRENT, classIndexValueNext,
						  nextColorExceedClassList, &len);
				  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextColorExceedClassList,len);
				  FPOBJ_TRACE_EXIT (bufp, owa);
				  return owa.rc;
			  }
		  }
		  else
		  {
			  lookForNextAdd = L7_FALSE;
		  }
	  }
	  owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
	  FPOBJ_TRACE_EXIT (bufp, owa);
	  return owa.rc;

  }


   while (lookForNextAdd == L7_TRUE)
   {
          if (usmDbDiffServClassGetNext(L7_UNIT_CURRENT, classIndexValueNext,
                                                                                   &classIndexValueNext) == L7_SUCCESS)
          {
              if( (usmDbDiffServPolicyAttrStmtPoliceColorAwareClassValidate(
L7_UNIT_CURRENT,
                                                                              keyPolicyIndexValue,
                                                                              keyPolicyInstIndexValue,
                                                                              classIndexValueNext)  == L7_SUCCESS ) &&
                                 (tempColorExceedIndexValue == classIndexValueNext) )
              {
                  len = sizeof(nextColorExceedClassList);
                                memset(nextColorExceedClassList, 0, sizeof(nextColorExceedClassList));
                  usmDbDiffServClassNameGet(L7_UNIT_CURRENT, classIndexValueNext,
                                                                            nextColorExceedClassList, &len);
                  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextColorExceedClassList,len);
                  FPOBJ_TRACE_EXIT (bufp, owa);
                  return owa.rc;
              }
         }
         else
         {
           lookForNextAdd = L7_FALSE;
         }
   }
  owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;


} 
/******************************************************************************
*
* @function fpObjList_qosDiffServPolicyAttributeStmt_ColorExceedClassList
*
* @purpose Set 'ColorExceedClassList'
*
*
*
* @return
*******************************************************************************/


xLibRC_t fpObjList_qosDiffServPolicyAttributeStmt_ColorExceedClassList (void *wap,
void *bufp)
{
	fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
	xLibStr256_t objColorExceedClassList;
	xLibStr256_t nextColorExceedClassList ;

	fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
	xLibU32_t keyPolicyIndexValue;

	fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
	xLibU32_t keyPolicyInstIndexValue;

	xLibU32_t classIndexValueNext;
	L7_uint32   len = 0;

	xLibBool_t isFirstTime = XLIB_FALSE;
	xLibBool_t entryFound = XLIB_FALSE;
	L7_BOOL lookForNextAdd = L7_TRUE;

	/* retrieve key: PolicyIndex */
	kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
			(xLibU8_t *) & keyPolicyIndexValue, &kwa.len);
	if (kwa.rc != XLIBRC_SUCCESS)
	{
		kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
		FPOBJ_TRACE_EXIT (bufp, kwa);
		return kwa.rc;
	}
	FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa.len);

	/* retrieve key: PolicyInstIndex */
	kwa2.rc =
		xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
				(xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
	if (kwa2.l7rc != L7_SUCCESS)
	{
		kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
		FPOBJ_TRACE_EXIT (bufp, kwa2);
		return kwa2.rc;
	}
	FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);



	memset(objColorExceedClassList, 0x00, sizeof(objColorExceedClassList ));

	owa.rc = xLibFilterGet (wap,
			XOBJ_qosDiffServPolicyAttributeStmt_ColorExceedClassList ,
			(xLibU8_t *) objColorExceedClassList, &owa.len );
	if (owa.rc != XLIBRC_SUCCESS)
	{
		FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
		memset(objColorExceedClassList, 0x00, sizeof(objColorExceedClassList));
		isFirstTime = XLIB_TRUE;
	}
        if(isFirstTime == XLIB_TRUE )
        {
	  classIndexValueNext = 0;
        }
	else
	{
		/*get the index of the class retrieved by the filterget */
		owa.l7rc = usmDbDiffServClassNameToIndex(L7_UNIT_CURRENT,
				objColorExceedClassList,
				&classIndexValueNext);
		if (owa.l7rc != L7_SUCCESS)
		{
			owa.rc = XLIBRC_ENDOF_TABLE;
			FPOBJ_TRACE_EXIT (bufp, owa);
			return owa.rc;
		}

	}

	while (lookForNextAdd == L7_TRUE)
	{
           if (usmDbDiffServClassGetNext(L7_UNIT_CURRENT, classIndexValueNext,
					&classIndexValueNext) == L7_SUCCESS)
           {
	      if(usmDbDiffServPolicyAttrStmtPoliceColorAwareClassValidate(
						L7_UNIT_CURRENT,
						keyPolicyIndexValue,
						keyPolicyInstIndexValue,
						classIndexValueNext)  == L7_SUCCESS )
              {
        	len = sizeof(nextColorExceedClassList);
		memset(nextColorExceedClassList, 0x00, sizeof(nextColorExceedClassList));
		usmDbDiffServClassNameGet(L7_UNIT_CURRENT, classIndexValueNext,
				nextColorExceedClassList, &len);
		entryFound = XLIB_TRUE;
		break;
	      }
	   }
	   else
	   {
		   lookForNextAdd = L7_FALSE;
	   }
	}

	if(entryFound != XLIB_TRUE )
	{
          owa.rc = XLIBRC_ENDOF_TABLE;
	  FPOBJ_TRACE_EXIT (bufp, owa);
	  return owa.rc;
	}

	owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextColorExceedClassList,
			strlen(nextColorExceedClassList));
	FPOBJ_TRACE_EXIT (bufp, owa);
	return owa.rc;
}

/******************************************************************************
*
* @function fpObjSet_qosDiffServPolicyAttributeStmt_ColorExceedClassList
*
* @purpose Set 'ColorExceedClassList'
*
*
*
* @return
*******************************************************************************/

xLibRC_t fpObjSet_qosDiffServPolicyAttributeStmt_ColorExceedClassList(void *wap
, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objColorExceedClassList;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;

  L7_uint32   classIndexValueTemp;

        fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyAttrIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

   /* retrieve key: PolicyIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyIndex,
                          (xLibU8_t *) & keyPolicyIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa.len);

   /* retrieve key: PolicyInstIndex */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyInstIndex,
        (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* retrieve key: PolicyAttrIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyAttributeStmt_PolicyAttrIndex,
        (xLibU8_t *) & keyPolicyAttrIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyAttrIndexValue, kwa3.len);

  memset(objColorExceedClassList, 0x00, sizeof(objColorExceedClassList));
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *)  objColorExceedClassList, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
   {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objColorExceedClassList, owa.len);

  owa.l7rc = usmDbDiffServClassNameToIndex(L7_UNIT_CURRENT,
                                       objColorExceedClassList,
                                      &classIndexValueTemp);
  if (owa.l7rc != L7_SUCCESS)
  {
     owa.rc = XLIBRC_ENDOF_TABLE;
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
  }


  owa.l7rc = usmDbDiffServPolicyAttrStmtPoliceColorExceedIndexSet(L7_UNIT_CURRENT,
			keyPolicyIndexValue,
			keyPolicyInstIndexValue,
			keyPolicyAttrIndexValue,
                                                                0);
  if(owa.l7rc == L7_SUCCESS)
  {
         /* set the conform color class index in the policing policy attr,
         * which will cause the corresponding color mode and value to be set
         * (we just set the index to 0, so no need to set it to 0 again)
         */
      owa.l7rc = usmDbDiffServPolicyAttrStmtPoliceColorExceedIndexSet(
			  L7_UNIT_CURRENT,
			  keyPolicyIndexValue,
			  keyPolicyInstIndexValue,
			  keyPolicyAttrIndexValue,
			  classIndexValueTemp);
  }


  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}



