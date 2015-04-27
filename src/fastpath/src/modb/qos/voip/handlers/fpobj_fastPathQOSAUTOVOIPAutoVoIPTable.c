
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_fastPathQOSAUTOVOIPAutoVoIPTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to fastPathQOSAUTOVOIP-object.xml
*
* @create  03 June 2008, Tuesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_fastPathQOSAUTOVOIPAutoVoIPTable_obj.h"
#include "usmdb_qos_voip_api.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_fastPathQOSAUTOVOIPAutoVoIPTable_GlobalAutoVoIPMode
*
* @purpose Get 'GlobalAutoVoIPMode'
 *@description  [GlobalAutoVoIPMode] Enables / disables AutoVoIP profile on the
* switch.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSAUTOVOIPAutoVoIPTable_GlobalAutoVoIPMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objGlobalAutoVoIPModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbQosVoIPIntfModeGet (L7_UNIT_CURRENT, L7_ALL_INTERFACES, &objGlobalAutoVoIPModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objGlobalAutoVoIPModeValue, sizeof (objGlobalAutoVoIPModeValue));

  /* return the object value: GlobalAutoVoIPMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGlobalAutoVoIPModeValue,
                           sizeof (objGlobalAutoVoIPModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_fastPathQOSAUTOVOIPAutoVoIPTable_GlobalAutoVoIPMode
*
* @purpose Set 'GlobalAutoVoIPMode'
 *@description  [GlobalAutoVoIPMode] Enables / disables AutoVoIP profile on the
* switch.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSAUTOVOIPAutoVoIPTable_GlobalAutoVoIPMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objGlobalAutoVoIPModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GlobalAutoVoIPMode */
  owa.len = sizeof (objGlobalAutoVoIPModeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objGlobalAutoVoIPModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGlobalAutoVoIPModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbQosVoIPIntfModeSet (L7_UNIT_CURRENT, L7_ALL_INTERFACES, objGlobalAutoVoIPModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSAUTOVOIPAutoVoIPTable_GlobalAutoVoIPTrafficClass
*
* @purpose Get 'AutoVoIPTrafficClass'
 *@description  [AutoVoIPTrafficClass] Traffic Class to which all VoIP traffic
* is mapped to.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSAUTOVOIPAutoVoIPTable_GlobalAutoVoIPTrafficClass (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGlobalAutoVoIPTrafficClassValue;
  FPOBJ_TRACE_ENTER (bufp);


  /* get the value from application */
  owa.l7rc = usmDbQosVoIPIntfCosQueueGet (L7_UNIT_CURRENT, L7_ALL_INTERFACES,
                              &objGlobalAutoVoIPTrafficClassValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AutoVoIPTrafficClass */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGlobalAutoVoIPTrafficClassValue,
                           sizeof (objGlobalAutoVoIPTrafficClassValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_fastPathQOSAUTOVOIPAutoVoIPTable_AutoVoIPIntfIndex
*
* @purpose Get 'AutoVoIPIntfIndex'
 *@description  [AutoVoIPIntfIndex] Specifies all Auto VoIP configurable
* interfaces.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSAUTOVOIPAutoVoIPTable_AutoVoIPIntfIndex (void *wap, void *bufp)
{
  xLibU32_t objAutoVoIPIntfIndexValue;
  xLibU32_t nextObjAutoVoIPIntfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (nextObjAutoVoIPIntfIndexValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AutoVoIPIntfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSAUTOVOIPAutoVoIPTable_AutoVoIPIntfIndex,
                          (xLibU8_t *) & objAutoVoIPIntfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbQoSVoIPValidIntfFirstGet(&nextObjAutoVoIPIntfIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objAutoVoIPIntfIndexValue, owa.len);
    owa.l7rc = usmDbQoSVoIPValidIntfNextGet(objAutoVoIPIntfIndexValue,&nextObjAutoVoIPIntfIndexValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjAutoVoIPIntfIndexValue, owa.len);

  /* return the object value: AutoVoIPIntfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjAutoVoIPIntfIndexValue,
                           sizeof (nextObjAutoVoIPIntfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSAUTOVOIPAutoVoIPTable_AutoVoIPMode
*
* @purpose Get 'AutoVoIPMode'
 *@description  [AutoVoIPMode] Enables / disables AutoVoIP profile on an
* interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSAUTOVOIPAutoVoIPTable_AutoVoIPMode (void *wap, void *bufp)
{

  xLibU32_t keyAutoVoIPIntfIndexValue;
  fpObjWa_t kwaAutoVoIPIntfIndex = FPOBJ_INIT_WA (sizeof (keyAutoVoIPIntfIndexValue));
  
  xLibU32_t objAutoVoIPModeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objAutoVoIPModeValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AutoVoIPIntfIndex */
  kwaAutoVoIPIntfIndex.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSAUTOVOIPAutoVoIPTable_AutoVoIPIntfIndex,
                   (xLibU8_t *) & keyAutoVoIPIntfIndexValue, &kwaAutoVoIPIntfIndex.len);
  if (kwaAutoVoIPIntfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaAutoVoIPIntfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaAutoVoIPIntfIndex);
    return kwaAutoVoIPIntfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAutoVoIPIntfIndexValue, kwaAutoVoIPIntfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbQosVoIPIntfModeGet (L7_UNIT_CURRENT, keyAutoVoIPIntfIndexValue, &objAutoVoIPModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AutoVoIPMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAutoVoIPModeValue,
                           sizeof (objAutoVoIPModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_fastPathQOSAUTOVOIPAutoVoIPTable_AutoVoIPMode
*
* @purpose Set 'AutoVoIPMode'
 *@description  [AutoVoIPMode] Enables / disables AutoVoIP profile on an
* interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSAUTOVOIPAutoVoIPTable_AutoVoIPMode (void *wap, void *bufp)
{

  xLibU32_t objAutoVoIPModeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objAutoVoIPModeValue));
  
  xLibU32_t keyAutoVoIPIntfIndexValue;
  fpObjWa_t kwaAutoVoIPIntfIndex = FPOBJ_INIT_WA (sizeof (keyAutoVoIPIntfIndexValue));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AutoVoIPMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAutoVoIPModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAutoVoIPModeValue, owa.len);

  /* retrieve key: AutoVoIPIntfIndex */
  kwaAutoVoIPIntfIndex.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSAUTOVOIPAutoVoIPTable_AutoVoIPIntfIndex,
                   (xLibU8_t *) & keyAutoVoIPIntfIndexValue, &kwaAutoVoIPIntfIndex.len);
  if (kwaAutoVoIPIntfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaAutoVoIPIntfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaAutoVoIPIntfIndex);
    return kwaAutoVoIPIntfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAutoVoIPIntfIndexValue, kwaAutoVoIPIntfIndex.len);

  /* set the value in application */
  owa.l7rc = usmDbQosVoIPIntfModeSet (L7_UNIT_CURRENT, keyAutoVoIPIntfIndexValue, objAutoVoIPModeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSAUTOVOIPAutoVoIPTable_AutoVoIPTrafficClass
*
* @purpose Get 'AutoVoIPTrafficClass'
 *@description  [AutoVoIPTrafficClass] Traffic Class to which all VoIP traffic
* is mapped to.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSAUTOVOIPAutoVoIPTable_AutoVoIPTrafficClass (void *wap, void *bufp)
{

  xLibU32_t keyAutoVoIPIntfIndexValue;
  fpObjWa_t kwaAutoVoIPIntfIndex = FPOBJ_INIT_WA (sizeof (keyAutoVoIPIntfIndexValue));
  
  xLibU32_t objAutoVoIPTrafficClassValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objAutoVoIPTrafficClassValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AutoVoIPIntfIndex */
  kwaAutoVoIPIntfIndex.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSAUTOVOIPAutoVoIPTable_AutoVoIPIntfIndex,
                   (xLibU8_t *) & keyAutoVoIPIntfIndexValue, &kwaAutoVoIPIntfIndex.len);
  if (kwaAutoVoIPIntfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaAutoVoIPIntfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaAutoVoIPIntfIndex);
    return kwaAutoVoIPIntfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAutoVoIPIntfIndexValue, kwaAutoVoIPIntfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbQosVoIPIntfCosQueueGet (L7_UNIT_CURRENT, keyAutoVoIPIntfIndexValue,
                              &objAutoVoIPTrafficClassValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AutoVoIPTrafficClass */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAutoVoIPTrafficClassValue,
                           sizeof (objAutoVoIPTrafficClassValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
