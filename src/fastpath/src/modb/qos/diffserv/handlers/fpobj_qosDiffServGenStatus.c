/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_qosDiffServGenStatus.c
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
#include "_xe_qosDiffServGenStatus_obj.h"
#include "usmdb_mib_diffserv_private_api.h"


/*******************************************************************************
* @function fpObjGet_qosDiffServGenStatus_PolicyAttrTableSize
*
* @purpose Get 'PolicyAttrTableSize'
*
* @description [PolicyAttrTableSize]:  Current size of the Policy Attribute
*              Table 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGenStatus_PolicyAttrTableSize (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPolicyAttrTableSizeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServGenPolicyAttrTableSizeGet (L7_UNIT_CURRENT,
                                            &objPolicyAttrTableSizeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPolicyAttrTableSizeValue,
                     sizeof (objPolicyAttrTableSizeValue));

  /* return the object value: PolicyAttrTableSize */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPolicyAttrTableSizeValue,
                           sizeof (objPolicyAttrTableSizeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServGenStatus_PolicyTableMax
*
* @purpose Get 'PolicyTableMax'
*
* @description [PolicyTableMax]:  Max size of the Policy Table 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGenStatus_PolicyTableMax (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPolicyTableMaxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServGenPolicyTableMaxGet (L7_UNIT_CURRENT,
                                       &objPolicyTableMaxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPolicyTableMaxValue,
                     sizeof (objPolicyTableMaxValue));

  /* return the object value: PolicyTableMax */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPolicyTableMaxValue,
                           sizeof (objPolicyTableMaxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServGenStatus_DiffServPolicyIndexNextFree
*
* @purpose Get 'DiffServPolicyIndexNextFree'
*
* @description [DiffServPolicyIndexNextFree]: This object contains an unused
*              value for agentDiffServPolicyIndex, or a zero to indicate
*              that none exists. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGenStatus_DiffServPolicyIndexNextFree (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDiffServPolicyIndexNextFreeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyIndexNext (L7_UNIT_CURRENT,
                                  &objDiffServPolicyIndexNextFreeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(objDiffServPolicyIndexNextFreeValue == 0 )
  {
    owa.rc = XLIBRC_COMMON_TABLE_FULL;    /* ADD TABLE FULL ERROR CODE*/
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDiffServPolicyIndexNextFreeValue,
                     sizeof (objDiffServPolicyIndexNextFreeValue));

  /* return the object value: DiffServPolicyIndexNextFree */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objDiffServPolicyIndexNextFreeValue,
                    sizeof (objDiffServPolicyIndexNextFreeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServGenStatus_PolicyAttrTableMax
*
* @purpose Get 'PolicyAttrTableMax'
*
* @description [PolicyAttrTableMax]:  Max size of the Policy Attribute Table
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGenStatus_PolicyAttrTableMax (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPolicyAttrTableMaxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServGenPolicyAttrTableMaxGet (L7_UNIT_CURRENT,
                                           &objPolicyAttrTableMaxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPolicyAttrTableMaxValue,
                     sizeof (objPolicyAttrTableMaxValue));

  /* return the object value: PolicyAttrTableMax */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPolicyAttrTableMaxValue,
                           sizeof (objPolicyAttrTableMaxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServGenStatus_ServiceTableMax
*
* @purpose Get 'ServiceTableMax'
*
* @description [ServiceTableMax]:  Max size of the Service Table 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGenStatus_ServiceTableMax (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objServiceTableMaxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServGenServiceTableMaxGet (L7_UNIT_CURRENT,
                                        &objServiceTableMaxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objServiceTableMaxValue,
                     sizeof (objServiceTableMaxValue));

  /* return the object value: ServiceTableMax */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objServiceTableMaxValue,
                           sizeof (objServiceTableMaxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServGenStatus_PolicyInstTableMax
*
* @purpose Get 'PolicyInstTableMax'
*
* @description [PolicyInstTableMax]:  Max size of the Policy-Class Instance
*              Table 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGenStatus_PolicyInstTableMax (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPolicyInstTableMaxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServGenPolicyInstTableMaxGet (L7_UNIT_CURRENT,
                                           &objPolicyInstTableMaxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPolicyInstTableMaxValue,
                     sizeof (objPolicyInstTableMaxValue));

  /* return the object value: PolicyInstTableMax */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPolicyInstTableMaxValue,
                           sizeof (objPolicyInstTableMaxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServGenStatus_AdminMode
*
* @purpose Get 'AdminMode'
*
* @description [AdminMode]:  DiffServ Admin Mode 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGenStatus_AdminMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbDiffServGenAdminModeGet (L7_UNIT_CURRENT, &objAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAdminModeValue, sizeof (objAdminModeValue));

  /* return the object value: AdminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAdminModeValue,
                           sizeof (objAdminModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_qosDiffServGenStatus_AdminMode
*
* @purpose Set 'AdminMode'
*
* @description [AdminMode]:  DiffServ Admin Mode 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_qosDiffServGenStatus_AdminMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AdminMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAdminModeValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbDiffServGenAdminModeSet (L7_UNIT_CURRENT, objAdminModeValue);
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
* @function fpObjGet_qosDiffServGenStatus_DiffServClassIndexNextFree
*
* @purpose Get 'DiffServClassIndexNextFree'
*
* @description [DiffServClassIndexNextFree]: This object contains an unused
*              value for classIndex, or a zero to indicate that none exists.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGenStatus_DiffServClassIndexNextFree (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDiffServClassIndexNextFreeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClassIndexNext (L7_UNIT_CURRENT,
                                 &objDiffServClassIndexNextFreeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (objDiffServClassIndexNextFreeValue == 0)
  {
    owa.rc = XLIBRC_COMMON_TABLE_FULL;    /* ADD TABLE FULL ERROR CODE */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_VALUE (bufp, &objDiffServClassIndexNextFreeValue,
                     sizeof (objDiffServClassIndexNextFreeValue));

  /* return the object value: DiffServClassIndexNextFree */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objDiffServClassIndexNextFreeValue,
                    sizeof (objDiffServClassIndexNextFreeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServGenStatus_ClassTableMax
*
* @purpose Get 'ClassTableMax'
*
* @description [ClassTableMax]:  Max size of the Class Table 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGenStatus_ClassTableMax (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objClassTableMaxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServGenClassTableMaxGet (L7_UNIT_CURRENT, &objClassTableMaxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objClassTableMaxValue,
                     sizeof (objClassTableMaxValue));

  /* return the object value: ClassTableMax */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objClassTableMaxValue,
                           sizeof (objClassTableMaxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServGenStatus_ClassTableSize
*
* @purpose Get 'ClassTableSize'
*
* @description [ClassTableSize]:  Current size of the Class Table 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGenStatus_ClassTableSize (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objClassTableSizeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServGenClassTableSizeGet (L7_UNIT_CURRENT,
                                       &objClassTableSizeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objClassTableSizeValue,
                     sizeof (objClassTableSizeValue));

  /* return the object value: ClassTableSize */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objClassTableSizeValue,
                           sizeof (objClassTableSizeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServGenStatus_ClassRuleTableSize
*
* @purpose Get 'ClassRuleTableSize'
*
* @description [ClassRuleTableSize]:  Current size of Class Rule Table 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGenStatus_ClassRuleTableSize (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objClassRuleTableSizeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServGenClassRuleTableSizeGet (L7_UNIT_CURRENT,
                                           &objClassRuleTableSizeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objClassRuleTableSizeValue,
                     sizeof (objClassRuleTableSizeValue));

  /* return the object value: ClassRuleTableSize */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objClassRuleTableSizeValue,
                           sizeof (objClassRuleTableSizeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServGenStatus_ClassRuleTableMax
*
* @purpose Get 'ClassRuleTableMax'
*
* @description [ClassRuleTableMax]:  Max size of the Class Rule Table. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGenStatus_ClassRuleTableMax (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objClassRuleTableMaxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServGenClassRuleTableMaxGet (L7_UNIT_CURRENT,
                                          &objClassRuleTableMaxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objClassRuleTableMaxValue,
                     sizeof (objClassRuleTableMaxValue));

  /* return the object value: ClassRuleTableMax */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objClassRuleTableMaxValue,
                           sizeof (objClassRuleTableMaxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServGenStatus_ServiceTableSize
*
* @purpose Get 'ServiceTableSize'
*
* @description [ServiceTableSize]:  Current size of the Service Table 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGenStatus_ServiceTableSize (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objServiceTableSizeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServGenServiceTableSizeGet (L7_UNIT_CURRENT,
                                         &objServiceTableSizeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objServiceTableSizeValue,
                     sizeof (objServiceTableSizeValue));

  /* return the object value: ServiceTableSize */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objServiceTableSizeValue,
                           sizeof (objServiceTableSizeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServGenStatus_PolicyInstTableSize
*
* @purpose Get 'PolicyInstTableSize'
*
* @description [PolicyInstTableSize]:  Current size of the Policy-Class Instance
*              Table 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGenStatus_PolicyInstTableSize (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPolicyInstTableSizeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServGenPolicyInstTableSizeGet (L7_UNIT_CURRENT,
                                            &objPolicyInstTableSizeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPolicyInstTableSizeValue,
                     sizeof (objPolicyInstTableSizeValue));

  /* return the object value: PolicyInstTableSize */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPolicyInstTableSizeValue,
                           sizeof (objPolicyInstTableSizeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServGenStatus_PolicyTableSize
*
* @purpose Get 'PolicyTableSize'
*
* @description [PolicyTableSize]:  Current Size of the Policy Table. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGenStatus_PolicyTableSize (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPolicyTableSizeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServGenPolicyTableSizeGet (L7_UNIT_CURRENT,
                                        &objPolicyTableSizeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPolicyTableSizeValue,
                     sizeof (objPolicyTableSizeValue));

  /* return the object value: PolicyTableSize */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPolicyTableSizeValue,
                           sizeof (objPolicyTableSizeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_qosDiffServGenStatus_ClassTableSizeAndMax
*
* @purpose Get 'ClassTableSizeAndMax'
*
* @description [ClassTableSizeAndMax]:  Current Size / Max Size of the Class Table 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGenStatus_ClassTableSizeAndMax (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objClassTableSizeAndMaxValue;
  xLibU32_t tableSize, tableMax;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = 
    usmDbDiffServGenClassTableSizeGet (L7_UNIT_CURRENT, &tableSize);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &tableSize, sizeof (tableSize));

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServGenClassTableMaxGet (L7_UNIT_CURRENT, &tableMax);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &tableMax, sizeof (tableMax));

  /* format the string */
  osapiSnprintf(objClassTableSizeAndMaxValue, sizeof(objClassTableSizeAndMaxValue), 
                "%u / %u", tableSize, tableMax);

  /* return the object value: ClassTableSize */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objClassTableSizeAndMaxValue,
                           sizeof (objClassTableSizeAndMaxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_qosDiffServGenStatus_ClassRuleTableSizeAndMax
*
* @purpose Get 'ClassRuleTableSizeAndMax'
*
* @description [ClassRuleTableSizeAndMax]:  Current Size / Max Size of the Class Rule Table 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGenStatus_ClassRuleTableSizeAndMax (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objClassRuleTableSizeAndMaxValue;
  xLibU32_t tableSize, tableMax;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = 
    usmDbDiffServGenClassRuleTableSizeGet (L7_UNIT_CURRENT, &tableSize);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &tableSize, sizeof (tableSize));

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServGenClassRuleTableMaxGet (L7_UNIT_CURRENT, &tableMax);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &tableMax, sizeof (tableMax));

  /* format the string */
  osapiSnprintf(objClassRuleTableSizeAndMaxValue, sizeof(objClassRuleTableSizeAndMaxValue), 
                "%u / %u", tableSize, tableMax);

  /* return the object value: ClassRuleTableSize */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objClassRuleTableSizeAndMaxValue,
                           sizeof (objClassRuleTableSizeAndMaxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_qosDiffServGenStatus_PolicyTableSizeAndMax
*
* @purpose Get 'PolicyTableSizeAndMax'
*
* @description [PolicyTableSizeAndMax]:  Current Size / Max Size of the Policy Table 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGenStatus_PolicyTableSizeAndMax (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objPolicyTableSizeAndMaxValue;
  xLibU32_t tableSize, tableMax;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = 
    usmDbDiffServGenPolicyTableSizeGet (L7_UNIT_CURRENT, &tableSize);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &tableSize, sizeof (tableSize));

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServGenPolicyTableMaxGet (L7_UNIT_CURRENT, &tableMax);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &tableMax, sizeof (tableMax));

  /* format the string */
  osapiSnprintf(objPolicyTableSizeAndMaxValue, sizeof(objPolicyTableSizeAndMaxValue), 
                "%u / %u", tableSize, tableMax);

  /* return the object value: PolicyTableSize */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPolicyTableSizeAndMaxValue,
                           sizeof (objPolicyTableSizeAndMaxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_qosDiffServGenStatus_PolicyInstTableSizeAndMax
*
* @purpose Get 'PolicyInstTableSizeAndMax'
*
* @description [PolicyInstTableSizeAndMax]:  Current Size / Max Size of the Policy Instance Table 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGenStatus_PolicyInstTableSizeAndMax (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objPolicyInstTableSizeAndMaxValue;
  xLibU32_t tableSize, tableMax;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = 
    usmDbDiffServGenPolicyInstTableSizeGet (L7_UNIT_CURRENT, &tableSize);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &tableSize, sizeof (tableSize));

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServGenPolicyInstTableMaxGet (L7_UNIT_CURRENT, &tableMax);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &tableMax, sizeof (tableMax));

  /* format the string */
  osapiSnprintf(objPolicyInstTableSizeAndMaxValue, sizeof(objPolicyInstTableSizeAndMaxValue), 
                "%u / %u", tableSize, tableMax);

  /* return the object value: PolicyInstTableSize */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPolicyInstTableSizeAndMaxValue,
                           sizeof (objPolicyInstTableSizeAndMaxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_qosDiffServGenStatus_PolicyAttrTableSizeAndMax
*
* @purpose Get 'PolicyAttrTableSizeAndMax'
*
* @description [PolicyAttrTableSizeAndMax]:  Current Size / Max Size of the Policy Attribute Table 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGenStatus_PolicyAttrTableSizeAndMax (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objPolicyAttrTableSizeAndMaxValue;
  xLibU32_t tableSize, tableMax;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = 
    usmDbDiffServGenPolicyAttrTableSizeGet (L7_UNIT_CURRENT, &tableSize);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &tableSize, sizeof (tableSize));

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServGenPolicyAttrTableMaxGet (L7_UNIT_CURRENT, &tableMax);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &tableMax, sizeof (tableMax));

  /* format the string */
  osapiSnprintf(objPolicyAttrTableSizeAndMaxValue, sizeof(objPolicyAttrTableSizeAndMaxValue), 
                "%u / %u", tableSize, tableMax);

  /* return the object value: PolicyAttrTableSize */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPolicyAttrTableSizeAndMaxValue,
                           sizeof (objPolicyAttrTableSizeAndMaxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_qosDiffServGenStatus_ServiceTableSizeAndMax
*
* @purpose Get 'ServiceTableSizeAndMax'
*
* @description [ServiceTableSizeAndMax]:  Current Size / Max Size of the Service Table 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGenStatus_ServiceTableSizeAndMax (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objServiceTableSizeAndMaxValue;
  xLibU32_t tableSize, tableMax;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = 
    usmDbDiffServGenServiceTableSizeGet (L7_UNIT_CURRENT, &tableSize);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &tableSize, sizeof (tableSize));

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServGenServiceTableMaxGet (L7_UNIT_CURRENT, &tableMax);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &tableMax, sizeof (tableMax));

  /* format the string */
  osapiSnprintf(objServiceTableSizeAndMaxValue, sizeof(objServiceTableSizeAndMaxValue), 
                "%u / %u", tableSize, tableMax);

  /* return the object value: ServiceTableSize */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objServiceTableSizeAndMaxValue,
                           sizeof (objServiceTableSizeAndMaxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

