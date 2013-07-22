/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_SwitchingDot1qConstraint.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to dot1q-object.xml
*
* @create  9 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_SwitchingDot1qConstraint_obj.h"
#include "usmdb_mib_vlan_api.h"

#if 0
/*******************************************************************************
* @function fpObjGet_SwitchingDot1qConstraint_Vlan_Set
*
* @purpose Get 'Vlan + Set +'
*
* @description [Vlan]: The index of the row in dot1qVlanCurrentTable for the
*              VLAN constrained by this entry. 
*              [Set]: The identity of the constraint set to which dot1qConstraintVlan
*              belongs. These values may be chosen by the management
*              station. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1qConstraint_Vlan_Set (void *wap, void *bufp[],
                                                     xLibU16_t keyCount)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owaVlan = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVlanValue, nextObjVlanValue;
  fpObjWa_t owaSet = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSetValue, nextObjSetValue;
  void *outVlan = (void *) bufp[--keyCount];
  void *outSet = (void *) bufp[--keyCount];
  FPOBJ_TRACE_ENTER (outVlan);
  FPOBJ_TRACE_ENTER (outSet);

  /* retrieve key: Vlan */
  owaVlan.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1qConstraint_Vlan,
                              (xLibU8_t *) & objVlanValue, &owaVlan.len);
  if (owaVlan.rc == XLIBRC_SUCCESS)
  {
    /* retrieve key: Set */
    owaSet.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1qConstraint_Set,
                               (xLibU8_t *) & objSetValue, &owaSet.len);
  }
  else
  {
     objVlanValue = 0;
     nextObjVlanValue = 0; 
     objSetValue = 0;
     nextObjSetValue = 0;
  }

  FPOBJ_TRACE_CURRENT_KEY (outVlan, &objVlanValue, owaVlan.len);
  FPOBJ_TRACE_CURRENT_KEY (outSet, &objSetValue, owaSet.len);

  owa.rc =
    usmDbDot1qLearningConstraintsEntryNextGet(L7_UNIT_CURRENT, &objVlanValue, &objSetValue);

  nextObjVlanValue = objVlanValue;
  nextObjSetValue = objSetValue;

  if (owa.rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (outVlan, owaVlan);
    FPOBJ_TRACE_EXIT (outSet, owaSet);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (outVlan, &nextObjVlanValue, owaVlan.len);
  FPOBJ_TRACE_CURRENT_KEY (outSet, &nextObjSetValue, owaSet.len);

  /* return the object value: Vlan */
  xLibBufDataSet (outVlan,
                  (xLibU8_t *) & nextObjVlanValue, sizeof (nextObjVlanValue));

  /* return the object value: Set */
  xLibBufDataSet (outSet,
                  (xLibU8_t *) & nextObjSetValue, sizeof (nextObjSetValue));
  FPOBJ_TRACE_EXIT (outVlan, owaVlan);
  FPOBJ_TRACE_EXIT (outSet, owaSet);
  return XLIBRC_SUCCESS;
}
#endif

/*******************************************************************************
* @function fpObjGet_SwitchingDot1qConstraint_Vlan
*
* @purpose Get 'Vlan'
*
* @description [Vlan] The index of the row in dot1qVlanCurrentTable for the VLAN constrained by this entry.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1qConstraint_Vlan (void *wap, void *bufp)
{

  xLibU32_t objVlanValue;
  xLibU32_t nextObjVlanValue;
  xLibU32_t objSetValue;
  xLibU32_t nextObjSetValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Vlan */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1qConstraint_Vlan,
                          (xLibU8_t *) & objVlanValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objVlanValue = objSetValue = 0;
    owa.l7rc = L7_FAILURE; /* TODO: check */
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objVlanValue, owa.len);
    objSetValue = 0;
    nextObjVlanValue = objVlanValue;
    do
    {
      owa.l7rc = usmDbDot1qLearningConstraintsEntryNextGet (L7_UNIT_CURRENT,
                                                            &nextObjVlanValue,
                                                            &nextObjSetValue);
    }
    while ((objVlanValue == nextObjVlanValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjVlanValue, owa.len);

  /* return the object value: Vlan */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjVlanValue, sizeof (objVlanValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDot1qConstraint_Set
*
* @purpose Get 'Set'
*
* @description [Set] The identity of the constraint set to which dot1qConstraintVlan belongs. These values may be chosen by the management station.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1qConstraint_Set (void *wap, void *bufp)
{

  xLibU32_t objVlanValue;
  xLibU32_t nextObjVlanValue;
  xLibU32_t objSetValue;
  xLibU32_t nextObjSetValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Vlan */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1qConstraint_Vlan,
                          (xLibU8_t *) & objVlanValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objVlanValue, owa.len);

  /* retrieve key: Set */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1qConstraint_Set,
                          (xLibU8_t *) & objSetValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objSetValue = 0;
    owa.l7rc = L7_FAILURE; /* TODO: check */
    nextObjVlanValue = 0;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objSetValue, owa.len);

    nextObjVlanValue = objVlanValue;
    nextObjSetValue = objSetValue; 
    owa.l7rc = usmDbDot1qLearningConstraintsEntryNextGet (L7_UNIT_CURRENT,
                                                          &nextObjVlanValue, 
                                                          &nextObjSetValue);
  }

  if ((objVlanValue != nextObjVlanValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjSetValue, owa.len);

  /* return the object value: Set */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjSetValue, sizeof (objSetValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDot1qConstraint_SetDefault
*
* @purpose Get 'SetDefault'
*
* @description [SetDefault]: The identity of the constraint set to which a
*              VLAN belongs, if there is not an explicit entry for that VLAN
*              in dot1qLearningConstraintsTable 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1qConstraint_SetDefault (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSetDefaultValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1qConstraintSetDefaultGet (L7_UNIT_CURRENT, &objSetDefaultValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSetDefaultValue, sizeof (objSetDefaultValue));

  /* return the object value: SetDefault */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSetDefaultValue,
                           sizeof (objSetDefaultValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingDot1qConstraint_SetDefault
*
* @purpose Set 'SetDefault'
*
* @description [SetDefault]: The identity of the constraint set to which a
*              VLAN belongs, if there is not an explicit entry for that VLAN
*              in dot1qLearningConstraintsTable 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDot1qConstraint_SetDefault (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSetDefaultValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SetDefault */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSetDefaultValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSetDefaultValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot1qConstraintSetDefaultSet (L7_UNIT_CURRENT, objSetDefaultValue);
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
* @function fpObjGet_SwitchingDot1qConstraint_TypeDefault
*
* @purpose Get 'TypeDefault'
*
* @description [TypeDefault]: The type of constraint set to which a VLAN belongs,
*              if there is not an explicit entry for that VLAN in
*              dot1qLearningConstraintsTable. The types are as defined for
*              dot1qConstraintType. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1qConstraint_TypeDefault (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTypeDefaultValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1qConstraintTypeDefaultGet (L7_UNIT_CURRENT, &objTypeDefaultValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTypeDefaultValue, sizeof (objTypeDefaultValue));

  /* return the object value: TypeDefault */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTypeDefaultValue,
                           sizeof (objTypeDefaultValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingDot1qConstraint_TypeDefault
*
* @purpose Set 'TypeDefault'
*
* @description [TypeDefault]: The type of constraint set to which a VLAN belongs,
*              if there is not an explicit entry for that VLAN in
*              dot1qLearningConstraintsTable. The types are as defined for
*              dot1qConstraintType. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDot1qConstraint_TypeDefault (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTypeDefaultValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TypeDefault */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTypeDefaultValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTypeDefaultValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot1qConstraintTypeDefaultSet (L7_UNIT_CURRENT, objTypeDefaultValue);
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
* @function fpObjGet_SwitchingDot1qConstraint_Status
*
* @purpose Get 'Status'
*
* @description [Status]: The status of this entry. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1qConstraint_Status (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySetValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Vlan */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1qConstraint_Vlan,
                           (xLibU8_t *) & keyVlanValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanValue, kwa1.len);

  /* retrieve key: Set */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1qConstraint_Set,
                           (xLibU8_t *) & keySetValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySetValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1qConstraintStatusGet (L7_UNIT_CURRENT, keyVlanValue,
                                            keySetValue, &objStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Status */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStatusValue,
                           sizeof (objStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingDot1qConstraint_Status
*
* @purpose Set 'Status'
*
* @description [Status]: The status of this entry. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDot1qConstraint_Status (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySetValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Status */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStatusValue, owa.len);

  /* retrieve key: Vlan */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1qConstraint_Vlan,
                           (xLibU8_t *) & keyVlanValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanValue, kwa1.len);

  /* retrieve key: Set */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1qConstraint_Set,
                           (xLibU8_t *) & keySetValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySetValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbDot1qConstraintStatusSet (L7_UNIT_CURRENT, keyVlanValue,
                                            keySetValue, objStatusValue);
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
* @function fpObjGet_SwitchingDot1qConstraint_Type
*
* @purpose Get 'Type'
*
* @description [Type]: The type of constraint this entry defines. independent(1)
*              - the VLAN, dot1qConstraintVlan, uses an independent
*              filtering database from all other VLANs in the same set, defined
*              by dot1qConstraintSet. shared(2) - the VLAN, dot1qConstraintVlan,
*              shares the same filtering database as all other
*              VLANs in the same set, defined by dot1qConstraintSet. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1qConstraint_Type (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySetValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Vlan */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1qConstraint_Vlan,
                           (xLibU8_t *) & keyVlanValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanValue, kwa1.len);

  /* retrieve key: Set */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1qConstraint_Set,
                           (xLibU8_t *) & keySetValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySetValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1qConstraintTypeGet (L7_UNIT_CURRENT, keyVlanValue,
                                          keySetValue, &objTypeValue);
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


/*******************************************************************************
* @function fpObjSet_SwitchingDot1qConstraint_Type
*
* @purpose Set 'Type'
*
* @description [Type]: The type of constraint this entry defines. independent(1)
*              - the VLAN, dot1qConstraintVlan, uses an independent
*              filtering database from all other VLANs in the same set, defined
*              by dot1qConstraintSet. shared(2) - the VLAN, dot1qConstraintVlan,
*              shares the same filtering database as all other
*              VLANs in the same set, defined by dot1qConstraintSet. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDot1qConstraint_Type (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTypeValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySetValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Type */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTypeValue, owa.len);

  /* retrieve key: Vlan */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1qConstraint_Vlan,
                           (xLibU8_t *) & keyVlanValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanValue, kwa1.len);

  /* retrieve key: Set */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1qConstraint_Set,
                           (xLibU8_t *) & keySetValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySetValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbDot1qConstraintTypeSet (L7_UNIT_CURRENT, keyVlanValue,
                                          keySetValue, objTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
