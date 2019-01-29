/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseentPhysicalContains.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to system-object.xml
*
* @create  11 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseentPhysicalContains_obj.h"
#include "usmdb_edb_api.h"


/*******************************************************************************
* @function fpObjGet_baseentPhysicalContains_entPhysicalIndex
*
* @purpose Get 'entPhysicalIndex'
*
* @description [entPhysicalIndex] The index for this entry.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseentPhysicalContains_entPhysicalIndex (void *wap, void *bufp)
{

  xLibU32_t objentPhysicalIndexValue;
  xLibU32_t nextObjentPhysicalIndexValue;
  xLibU32_t objentPhysicalChildIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: entPhysicalIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseentPhysicalContains_entPhysicalIndex,
                          (xLibU8_t *) & objentPhysicalIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objentPhysicalIndexValue = objentPhysicalChildIndexValue = 0;
    owa.l7rc =
      usmDbEdbPhysicalContainsEntryNextGet (
                                            &objentPhysicalIndexValue,
                                            &objentPhysicalChildIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objentPhysicalIndexValue, owa.len);
    nextObjentPhysicalIndexValue = objentPhysicalIndexValue;
    objentPhysicalChildIndexValue = 0;
    do
    {
      owa.l7rc =
        usmDbEdbPhysicalContainsEntryNextGet (
                                              &nextObjentPhysicalIndexValue,
                                              &objentPhysicalChildIndexValue);
    }
    while ((objentPhysicalIndexValue == nextObjentPhysicalIndexValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjentPhysicalIndexValue, owa.len);

  /* return the object value: entPhysicalIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjentPhysicalIndexValue,
                           sizeof (objentPhysicalIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseentPhysicalContains_entPhysicalChildIndex
*
* @purpose Get 'entPhysicalChildIndex'
*
* @description [entPhysicalChildIndex] The value of entPhysicalIndex for the contained physical entity.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseentPhysicalContains_entPhysicalChildIndex (void *wap, void *bufp)
{

  xLibU32_t objentPhysicalIndexValue;
  xLibU32_t nextObjentPhysicalIndexValue;
  xLibU32_t objentPhysicalChildIndexValue;
  xLibU32_t nextObjentPhysicalChildIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: entPhysicalIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseentPhysicalContains_entPhysicalIndex,
                          (xLibU8_t *) & objentPhysicalIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objentPhysicalIndexValue, owa.len);

  /* retrieve key: entPhysicalChildIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseentPhysicalContains_entPhysicalChildIndex,
                          (xLibU8_t *) & objentPhysicalChildIndexValue, &owa.len);
  nextObjentPhysicalIndexValue = objentPhysicalIndexValue;
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objentPhysicalChildIndexValue = 0;
    owa.l7rc =
      usmDbEdbPhysicalContainsEntryNextGet (
                                            &nextObjentPhysicalIndexValue,
                                            &objentPhysicalChildIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objentPhysicalChildIndexValue, owa.len);

    owa.l7rc =
      usmDbEdbPhysicalContainsEntryNextGet (
                                            &nextObjentPhysicalIndexValue,
                                            &objentPhysicalChildIndexValue);

  }

  if ((objentPhysicalIndexValue != nextObjentPhysicalIndexValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 
  nextObjentPhysicalChildIndexValue = objentPhysicalChildIndexValue;
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjentPhysicalChildIndexValue, owa.len);

  /* return the object value: entPhysicalChildIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjentPhysicalChildIndexValue,
                           sizeof (objentPhysicalChildIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
#if 0
/*******************************************************************************
* @function fpObjGet_baseentPhysicalContains_entPhysicalIndex
*
* @purpose Get 'entPhysicalIndex'
*
* @description [entPhysicalIndex]: The index for this entry. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseentPhysicalContains_entPhysicalIndex (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objentPhysicalIndexValue;
  xLibU32_t nextObjentPhysicalIndexValue;
  xLibU32_t tempObjentChildIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: entPhysicalIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseentPhysicalContains_entPhysicalIndex,
                          (xLibU8_t *) & objentPhysicalIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
     owa.rc = xLibFilterGet (wap, XOBJ_baseentPhysicalContains_entPhysicalChildIndex,
                          (xLibU8_t *) & tempObjentChildIndexValue, &owa.len);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objentPhysicalIndexValue = 0;
    nextObjentPhysicalIndexValue = 0;
    tempObjentChildIndexValue = 0;
  }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objentPhysicalIndexValue, owa.len);
    owa.l7rc =
      usmDbEdbPhysicalContainsEntryNextGet (
                                            &objentPhysicalIndexValue,
                                            &tempObjentChildIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  nextObjentPhysicalIndexValue = objentPhysicalIndexValue
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjentPhysicalIndexValue, owa.len);

  /* return the object value: entPhysicalIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjentPhysicalIndexValue,
                           sizeof (objentPhysicalIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseentPhysicalContains_entPhysicalChildIndex
*
* @purpose Get 'entPhysicalChildIndex'
*
* @description [entPhysicalChildIndex]: The value of entPhysicalIndex for
*              the contained physical entity. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseentPhysicalContains_entPhysicalChildIndex (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objentPhysicalChildIndexValue;
  xLibU32_t nextObjentPhysicalChildIndexValue;
  xLibU32_t tempObjentPhysicalIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: entPhysicalChildIndex */
  owa.rc =
    xLibFilterGet (wap, XOBJ_baseentPhysicalContains_entPhysicalChildIndex,
                   (xLibU8_t *) & objentPhysicalChildIndexValue, &owa.len);
  if (owa.rc == XLIBRC_SUCCESS)
  {
    owa.rc =
    xLibFilterGet (wap, XOBJ_baseentPhysicalContains_entPhysicalIndex ,
                   (xLibU8_t *) & tempObjentPhysicalIndexValue, &owa.len);
  }
  else
  {
        tempObjentPhysicalIndexValue = 0;
	 objentPhysicalChildIndexValue = 0;
	 nextObjentPhysicalChildIndexValue = 0;
  }  
  
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objentPhysicalChildIndexValue, owa.len);
    owa.l7rc =
      usmDbEdbPhysicalContainsEntryNextGet (
                                            &tempObjentPhysicalIndexValue,
                                            &objentPhysicalChildIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  nextObjentPhysicalChildIndexValue = objentPhysicalChildIndexValue;
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjentPhysicalChildIndexValue, owa.len);

  /* return the object value: entPhysicalChildIndex */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & nextObjentPhysicalChildIndexValue,
                    sizeof (objentPhysicalChildIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
#endif

