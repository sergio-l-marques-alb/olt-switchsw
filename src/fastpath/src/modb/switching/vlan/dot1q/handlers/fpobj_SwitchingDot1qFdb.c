/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_SwitchingDot1qFdb.c
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
#include "_xe_SwitchingDot1qFdb_obj.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_mib_bridge_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingDot1qFdb_Id
*
* @purpose Get 'Id'
*
* @description [Id]: The identity of this Filtering Database 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1qFdb_Id (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIdValue;
  xLibU32_t nextObjIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Id */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1qFdb_Id,
                          (xLibU8_t *) & objIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objIdValue = 0;
    owa.l7rc = usmDbFdbIdNextGet(L7_UNIT_CURRENT, objIdValue, &nextObjIdValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIdValue, owa.len);
    objIdValue = objIdValue +1;
    owa.l7rc = usmDbFdbIdNextGet (L7_UNIT_CURRENT, objIdValue, &nextObjIdValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIdValue, owa.len);

  /* return the object value: Id */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIdValue,
                           sizeof (objIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingDot1qFdb_DynamicCount
*
* @purpose Get 'DynamicCount'
*
* @description [DynamicCount]: The current number of dynamic entries in this
*              Filtering Database. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1qFdb_DynamicCount (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDynamicCountValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Id */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1qFdb_Id,
                          (xLibU8_t *) & keyIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1qFdbDynamicCountGet (L7_UNIT_CURRENT, keyIdValue,
                                           &objDynamicCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DynamicCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDynamicCountValue,
                           sizeof (objDynamicCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
