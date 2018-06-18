/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_fastPathQOSCOSCosQueueCfg.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to cos-object.xml
*
* @create  8 January 2008
*
* @author  Madan Mohan Goud.K
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_fastPathQOSCOSCosQueueCfg_obj.h"
#include "usmdb_qos_cos_api.h"
#include "cos_exports.h"
#include "l7_cos_api.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueGlobalShapingRate
*
* @purpose Get 'CosQueueGlobalShapingRate'
 *@description  [CosQueueGlobalShapingRate] egress shaping rate forspecified
* interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueGlobalShapingRate (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueGlobalShapingRateValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
	owa.l7rc =
    usmDbQosCosQueueIntfShapingRateGet (L7_UNIT_CURRENT,
                                        L7_ALL_INTERFACES,
                                        &objCosQueueGlobalShapingRateValue);
	
   if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objCosQueueGlobalShapingRateValue,
                     sizeof (objCosQueueGlobalShapingRateValue));

  /* return the object value: CosQueueGlobalShapingRate */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCosQueueGlobalShapingRateValue,
                           sizeof (objCosQueueGlobalShapingRateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueGlobalShapingRate
*
* @purpose Set 'CosQueueGlobalShapingRate'
 *@description  [CosQueueGlobalShapingRate] egress shaping rate forspecified
* interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueGlobalShapingRate (void *wap, void *bufp)
{
 fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueGlobalShapingRateValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CosQueueGlobalShapingRate */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objCosQueueGlobalShapingRateValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCosQueueGlobalShapingRateValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc =
    usmDbQosCosQueueIntfShapingRateSet (L7_UNIT_CURRENT,
                                        L7_ALL_INTERFACES,
                                        objCosQueueGlobalShapingRateValue);
	
   if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueGlobalRestoreDefaults
*
* @purpose Set 'CosQueueGlobalRestoreDefaults'
 *@description  [CosQueueGlobalRestoreDefaults] Restore Default shaping rate   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueGlobalRestoreDefaults (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueGlobalRestoreDefaultsValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CosQueueGlobalRestoreDefaults */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objCosQueueGlobalRestoreDefaultsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCosQueueGlobalRestoreDefaultsValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  if(objCosQueueGlobalRestoreDefaultsValue == L7_TRUE)
  {
    owa.l7rc = usmDbQosCosQueueIntfShapingRateSet(L7_UNIT_CURRENT, L7_ALL_INTERFACES,FD_QOS_COS_QCFG_INTF_SHAPING_RATE);
    if (owa.l7rc != L7_SUCCESS)
    {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }

    if ((usmDbFeaturePresentCheck(L7_UNIT_CURRENT, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_WRED_SUPPORT_FEATURE_ID) == L7_TRUE) &&
            (usmDbFeaturePresentCheck(L7_UNIT_CURRENT, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_WRED_DECAY_EXP_SYSTEM_ONLY_FEATURE_ID) != L7_TRUE))
    {
        owa.l7rc = usmDbQosCosQueueWredDecayExponentSet(L7_UNIT_CURRENT, L7_ALL_INTERFACES,FD_QOS_COS_QCFG_WRED_DECAY_EXP);

        if (owa.l7rc != L7_SUCCESS)
        {
           owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
           FPOBJ_TRACE_EXIT (bufp, owa);
           return owa.rc;
        }
    }
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtTypeGlobal
*
* @purpose Get 'CosQueueMgmtTypeGlobal'
 *@description  [CosQueueMgmtTypeGlobal] Queue depth management technique used
* for queues on this interface. This is only used if device supports
* independent settings per-queue.Queue Management Type can only be
* one of the following: 1)taildrop.Default value is taildrop.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtTypeGlobal (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueMgmtTypeGlobalValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbQosCosQueueMgmtTypePerIntfGet (L7_UNIT_CURRENT,
                                        L7_ALL_INTERFACES,
                                        &objCosQueueMgmtTypeGlobalValue);
   if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objCosQueueMgmtTypeGlobalValue,
                     sizeof (objCosQueueMgmtTypeGlobalValue));

  /* return the object value: CosQueueMgmtTypeGlobal */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCosQueueMgmtTypeGlobalValue,
                           sizeof (objCosQueueMgmtTypeGlobalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtTypeGlobal
*
* @purpose Set 'CosQueueMgmtTypeGlobal'
 *@description  [CosQueueMgmtTypeGlobal] Queue depth management technique used
* for queues on this interface. This is only used if device supports
* independent settings per-queue.Queue Management Type can only be
* one of the following: 1)taildrop.Default value is taildrop.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtTypeGlobal (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueMgmtTypeGlobalValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CosQueueMgmtTypeGlobal */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objCosQueueMgmtTypeGlobalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCosQueueMgmtTypeGlobalValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
	owa.l7rc =
    usmDbQosCosQueueMgmtTypePerIntfSet (L7_UNIT_CURRENT,
                                        L7_ALL_INTERFACES,
                                        objCosQueueMgmtTypeGlobalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSCOSCosQueueCfg_GlobalCosQueueWredDecayExponent
*
* @purpose Get 'GlobalCosQueueWredDecayExponent'
 *@description  [GlobalCosQueueWredDecayExponent] The management technique used
* for all queues on the specified interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosQueueCfg_GlobalCosQueueWredDecayExponent (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGlobalCosQueueWredDecayExponentValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
	owa.l7rc =
    usmDbQosCosQueueWredDecayExponentGet (L7_UNIT_CURRENT,
                                          L7_ALL_INTERFACES,
                                          &objGlobalCosQueueWredDecayExponentValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objGlobalCosQueueWredDecayExponentValue,
                     sizeof (objGlobalCosQueueWredDecayExponentValue));

  /* return the object value: GlobalCosQueueWredDecayExponent */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGlobalCosQueueWredDecayExponentValue,
                           sizeof (objGlobalCosQueueWredDecayExponentValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosQueueCfg_GlobalCosQueueWredDecayExponent
*
* @purpose Set 'GlobalCosQueueWredDecayExponent'
 *@description  [GlobalCosQueueWredDecayExponent] The management technique used
* for all queues on the specified interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosQueueCfg_GlobalCosQueueWredDecayExponent (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGlobalCosQueueWredDecayExponentValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GlobalCosQueueWredDecayExponent */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objGlobalCosQueueWredDecayExponentValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGlobalCosQueueWredDecayExponentValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc =
    usmDbQosCosQueueWredDecayExponentSet (L7_UNIT_CURRENT,
                                          L7_ALL_INTERFACES,
                                          objGlobalCosQueueWredDecayExponentValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}





/*******************************************************************************
* @function fpObjGet_fastPathQOSCOSCosQueueCfg_GlobalCosQueueSchedulerType
*
* @purpose Get 'GlobalCosQueueSchedulerType'
 *@description  [GlobalCosQueueSchedulerType] Specifies the type of scheduling
* used for this queue.Scheduler Type can only be one of the
* following: 1)strict 2)weighted   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosQueueCfg_GlobalCosQueueSchedulerType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGlobalCosQueueSchedulerTypeValue;
	
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyCosQueueIndexValue;

  L7_qosCosQueueSchedTypeList_t schedTypeList;
  L7_uint32 listIndex;
  L7_uint32 temp;


  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CosQueueIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                          (xLibU8_t *) & keyCosQueueIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIndexValue, kwa.len);

  /* get the value from application */

  owa.l7rc =
    usmDbQosCosQueueSchedulerTypeListGet (L7_UNIT_CURRENT,
                                      L7_ALL_INTERFACES,
                                      &schedTypeList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  listIndex = keyCosQueueIndexValue - L7_QOS_COS_QUEUE_ID_MIN;
  temp = schedTypeList.schedType[listIndex];
   objGlobalCosQueueSchedulerTypeValue = temp;
	
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objGlobalCosQueueSchedulerTypeValue,
                     sizeof (objGlobalCosQueueSchedulerTypeValue));

  /* return the object value: GlobalCosQueueSchedulerType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGlobalCosQueueSchedulerTypeValue,
                           sizeof (objGlobalCosQueueSchedulerTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosQueueCfg_GlobalCosQueueSchedulerType
*
* @purpose Set 'GlobalCosQueueSchedulerType'
 *@description  [GlobalCosQueueSchedulerType] Specifies the type of scheduling
* used for this queue.Scheduler Type can only be one of the
* following: 1)strict 2)weighted   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosQueueCfg_GlobalCosQueueSchedulerType (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGlobalCosQueueSchedulerTypeValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GlobalCosQueueSchedulerType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objGlobalCosQueueSchedulerTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGlobalCosQueueSchedulerTypeValue, owa.len);

  /* retrieve key: CosQueueIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                          (xLibU8_t *) & keyCosQueueIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbQosCosQueueSchedulerTypeSet (L7_UNIT_CURRENT,
                                      L7_ALL_INTERFACES,
                                      keyCosQueueIndexValue,
                                      objGlobalCosQueueSchedulerTypeValue);
   if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSCOSCosQueueCfg_GlobalCosQueueMgmtType
*
* @purpose Get 'GlobalCosQueueMgmtType'
 *@description  [GlobalCosQueueMgmtType] Queue depth management technique used
* for queues on this interface. This is only used if device supports
* independent settings per-queue   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosQueueCfg_GlobalCosQueueMgmtType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueMgmtTypeValue;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIndexValue;
 
  FPOBJ_TRACE_ENTER (bufp);

  L7_qosCosQueueMgmtTypeList_t MgmtTypeList;

  /* retrieve key: CosQueueIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                           (xLibU8_t *) & keyCosQueueIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIndexValue, kwa2.len);
 
  /* get the value from application */
   L7_uint32 listIndex = keyCosQueueIndexValue - L7_QOS_COS_QUEUE_ID_MIN;
  owa.l7rc =
    usmDbQosCosQueueMgmtTypeListGet (L7_UNIT_CURRENT, L7_ALL_INTERFACES,
                                     &MgmtTypeList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objCosQueueMgmtTypeValue = MgmtTypeList.mgmtType[listIndex];
  /* return the object value: CosQueueMgmtType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCosQueueMgmtTypeValue,
                           sizeof (objCosQueueMgmtTypeValue)); 
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosQueueCfg_GlobalCosQueueMgmtType
*
* @purpose Set 'GlobalCosQueueMgmtType'
 *@description  [GlobalCosQueueMgmtType] Queue depth management technique used
* for queues on this interface. This is only used if device supports
* independent settings per-queue   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosQueueCfg_GlobalCosQueueMgmtType (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objGlobalCosQueueMgmtTypeValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GlobalCosQueueMgmtType */
  owa.len = sizeof(objGlobalCosQueueMgmtTypeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objGlobalCosQueueMgmtTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGlobalCosQueueMgmtTypeValue, owa.len);

  /* retrieve key: CosQueueIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                           (xLibU8_t *) & keyCosQueueIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIndexValue, kwa2.len);
 
  /* set the value in application */
  owa.l7rc =
    usmDbQosCosQueueMgmtTypeSet (L7_UNIT_CURRENT,
                                        L7_ALL_INTERFACES, keyCosQueueIndexValue, 
                                        objGlobalCosQueueMgmtTypeValue);
	
   if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosQueueCfg_GlobalCosQueueDefaultsRestore
*
* @purpose Set 'GlobalCosQueueDefaultsRestore'
 *@description  [GlobalCosQueueDefaultsRestore] Causes the default values to be
* restored for all COS queue objects defined for this interface    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosQueueCfg_GlobalCosQueueDefaultsRestore (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objGlobalCosQueueDefaultsRestoreValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GlobalCosQueueDefaultsRestore */
  owa.len = sizeof (objGlobalCosQueueDefaultsRestoreValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objGlobalCosQueueDefaultsRestoreValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGlobalCosQueueDefaultsRestoreValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc =
    usmDbQosCosQueueDefaultsRestore (L7_UNIT_CURRENT, L7_ALL_INTERFACES);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSCOSCosQueueCfg_GlobalCosQueueMinBandwidth
*
* @purpose Get 'GlobalCosQueueMinBandwidth'
 *@description  [GlobalCosQueueMinBandwidth] Specifies the minimum guaranteed
* bandwidth allotted to this queue. Valid Range is (0 to 100)in
* increments of 5.The value 0 means no guaranteed minimum. Sum of
* individual Minimum Bandwidth values for all queues in the selected
* interface cannot exceed defin   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosQueueCfg_GlobalCosQueueMinBandwidth (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objGlobalCosQueueMinBandwidthValue;
 
  L7_qosCosQueueBwList_t minbwList;
  L7_uint32 listIndex;
  
  xLibU32_t keyCosQueueIndexValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CosQueueIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                          (xLibU8_t *) & keyCosQueueIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIndexValue, kwa.len);

  /* get the value from application */

  listIndex = keyCosQueueIndexValue;
  /* get the value from application */
  owa.l7rc =
    usmDbQosCosQueueMinBandwidthListGet(L7_UNIT_CURRENT,
                                        L7_ALL_INTERFACES,
                                        &minbwList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objGlobalCosQueueMinBandwidthValue = minbwList.bandwidth[listIndex];
	
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objGlobalCosQueueMinBandwidthValue,
                     sizeof (objGlobalCosQueueMinBandwidthValue));

  /* return the object value: GlobalCosQueueMinBandwidth */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGlobalCosQueueMinBandwidthValue,
                           sizeof (objGlobalCosQueueMinBandwidthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosQueueCfg_GlobalCosQueueMinBandwidth
*
* @purpose Set 'GlobalCosQueueMinBandwidth'
 *@description  [GlobalCosQueueMinBandwidth] Specifies the minimum guaranteed
* bandwidth allotted to this queue. Valid Range is (0 to 100)in
* increments of 5.The value 0 means no guaranteed minimum. Sum of
* individual Minimum Bandwidth values for all queues in the selected
* interface cannot exceed defin   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosQueueCfg_GlobalCosQueueMinBandwidth (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objGlobalCosQueueMinBandwidthValue;

  xLibU32_t keyCosQueueIndexValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  L7_uint32 listIndex;
  L7_uint32 total_bw=0;
  L7_uint32 i;
  L7_qosCosQueueBwList_t minbwList;


  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GlobalCosQueueMinBandwidth */
  owa.len = sizeof(objGlobalCosQueueMinBandwidthValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objGlobalCosQueueMinBandwidthValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGlobalCosQueueMinBandwidthValue, owa.len);

  /* retrieve key: CosQueueIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                          (xLibU8_t *) & keyCosQueueIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIndexValue, kwa.len);

   /* set the value in application */
  if((objGlobalCosQueueMinBandwidthValue != L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MIN) && (objGlobalCosQueueMinBandwidthValue != L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MAX) &&
           ((objGlobalCosQueueMinBandwidthValue % L7_QOS_COS_QUEUE_BANDWIDTH_STEP_SIZE) != 0))
  {
    owa.rc = XLIBRC_IMPROPER_VALUE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  (void)usmDbQosCosQueueMinBandwidthListGet(L7_UNIT_CURRENT,L7_ALL_INTERFACES,&minbwList);
  listIndex = keyCosQueueIndexValue - L7_QOS_COS_QUEUE_ID_MIN;
  minbwList.bandwidth[listIndex] = objGlobalCosQueueMinBandwidthValue;
  for(i=L7_QOS_COS_QUEUE_ID_MIN;i<=L7_QOS_COS_QUEUE_ID_MAX;i++)
     total_bw = total_bw + minbwList.bandwidth[i - L7_QOS_COS_QUEUE_ID_MIN];
  if (total_bw > L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MAX)
  {
    owa.rc = XLIBRC_MIN_BWIDTH_EXCEEDS;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.l7rc =
    usmDbQosCosQueueMinBandwidthSet (L7_UNIT_CURRENT, L7_ALL_INTERFACES,
                                     keyCosQueueIndexValue,
                                     objGlobalCosQueueMinBandwidthValue);
 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSCOSCosQueueCfg_GlobalCosQueueMaxBandwidth
*
* @purpose Get 'GlobalCosQueueMaxBandwidth'
 *@description  [GlobalCosQueueMaxBandwidth] Specifies the maximum allowed
* bandwidth allotted to this queue.    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosQueueCfg_GlobalCosQueueMaxBandwidth (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objGlobalCosQueueMaxBandwidthValue;

  xLibU32_t keyCosQueueIndexValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  L7_qosCosQueueBwList_t maxbwList;
  L7_uint32 listIndex;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CosQueueIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                          (xLibU8_t *) & keyCosQueueIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIndexValue, kwa.len);

  /* get the value from application */

  listIndex = keyCosQueueIndexValue - L7_QOS_COS_QUEUE_ID_MIN;
  owa.l7rc = usmDbQosCosQueueMaxBandwidthListGet(L7_UNIT_CURRENT,
                                                 L7_ALL_INTERFACES,
                                                 &maxbwList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
	
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
    objGlobalCosQueueMaxBandwidthValue = maxbwList.bandwidth[listIndex];


  FPOBJ_TRACE_VALUE (bufp, &objGlobalCosQueueMaxBandwidthValue,
                     sizeof (objGlobalCosQueueMaxBandwidthValue));

  /* return the object value: GlobalCosQueueMaxBandwidth */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGlobalCosQueueMaxBandwidthValue,
                           sizeof (objGlobalCosQueueMaxBandwidthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosQueueCfg_GlobalCosQueueMaxBandwidth
*
* @purpose Set 'GlobalCosQueueMaxBandwidth'
 *@description  [GlobalCosQueueMaxBandwidth] Specifies the maximum allowed
* bandwidth allotted to this queue.    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosQueueCfg_GlobalCosQueueMaxBandwidth (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objGlobalCosQueueMaxBandwidthValue;

  xLibU32_t keyCosQueueIndexValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GlobalCosQueueMaxBandwidth */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objGlobalCosQueueMaxBandwidthValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGlobalCosQueueMaxBandwidthValue, owa.len);

  /* retrieve key: CosQueueIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                          (xLibU8_t *) & keyCosQueueIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbQosCosQueueMaxBandwidthSet (L7_UNIT_CURRENT, L7_ALL_INTERFACES,
                                     keyCosQueueIndexValue,
                                     objGlobalCosQueueMaxBandwidthValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_fastPathQOSCOSCosQueueCfg_GlobalCosQueueMinBandwidthAllocated
*
* @purpose Get 'GlobalCosQueueMinBandwidthAllocated'
 *@description  [GlobalCosQueueMinBandwidthAllocated] minimum bandwidth allocated   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosQueueCfg_GlobalCosQueueMinBandwidthAllocated (void *wap,
                                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objGlobalCosQueueMinBandwidthAllocatedValue;

  L7_qosCosQueueBwList_t qosCosQueueBwList; 
  L7_uint32 queueId;
  L7_uint32 total_bw=0;  


  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */

  owa.l7rc = usmDbQosCosQueueMinBandwidthListGet(L7_UNIT_CURRENT, L7_ALL_INTERFACES, &qosCosQueueBwList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
	
  for(queueId=L7_QOS_COS_QUEUE_ID_MIN; queueId<=L7_QOS_COS_QUEUE_ID_MAX; queueId++)
  {
      total_bw = total_bw + qosCosQueueBwList.bandwidth[queueId - L7_QOS_COS_QUEUE_ID_MIN];
  } 
  objGlobalCosQueueMinBandwidthAllocatedValue = total_bw;


  FPOBJ_TRACE_VALUE (bufp, &objGlobalCosQueueMinBandwidthAllocatedValue,
                     sizeof (objGlobalCosQueueMinBandwidthAllocatedValue));

  /* return the object value: GlobalCosQueueMinBandwidthAllocated */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGlobalCosQueueMinBandwidthAllocatedValue,
                           sizeof (objGlobalCosQueueMinBandwidthAllocatedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}







/*******************************************************************************
* @function fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueDropPrecIndex
*
* @purpose Get 'CosQueueDropPrecIndex'
*
* @description The COS queue drop precedence level 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueDropPrecIndex (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueDropPrecIndexValue;
  xLibU32_t nextObjCosQueueDropPrecIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CosQueueDropPrecIndex */
  owa.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueDropPrecIndex,
                   (xLibU8_t *) & objCosQueueDropPrecIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjCosQueueDropPrecIndexValue=L7_QOS_COS_DROP_PREC_LEVEL_MIN;
    owa.l7rc = usmDbQosCosQueueDropPrecIndexGet(L7_UNIT_CURRENT,nextObjCosQueueDropPrecIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objCosQueueDropPrecIndexValue, owa.len);
    owa.l7rc =
      usmDbQosCosQueueDropPrecIndexGetNext (L7_UNIT_CURRENT,
                                            objCosQueueDropPrecIndexValue,
                                            &nextObjCosQueueDropPrecIndexValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjCosQueueDropPrecIndexValue, owa.len);

  /* return the object value: CosQueueDropPrecIndex */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & nextObjCosQueueDropPrecIndexValue,
                    sizeof (objCosQueueDropPrecIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtTailDropThreshold
*
* @purpose Get 'CosQueueMgmtTailDropThreshold'
*
* @description Tail drop queue threshold above which all packets are dropped 
*              for the current drop precedence level.The value specifies the 
*              threshold based on a fraction of the overall device queue size 
*              in terms of sixteenths (0/16, 1/16, 2/16…, 16/16). Since device 
*              impl 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtTailDropThreshold (void
                                                                           *wap,
                                                                           void
                                                                           *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIntfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueDropPrecIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueMgmtTailDropThresholdValue;
  L7_qosCosDropParmsList_t dropParmsList;
  L7_uint32 listIndex;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CosQueueIntfIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIntfIndex,
                   (xLibU8_t *) & keyCosQueueIntfIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIntfIndexValue, kwa1.len);

  /* retrieve key: CosQueueIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                           (xLibU8_t *) & keyCosQueueIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIndexValue, kwa2.len);

  /* retrieve key: CosQueueDropPrecIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueDropPrecIndex,
                   (xLibU8_t *) & keyCosQueueDropPrecIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueDropPrecIndexValue, kwa3.len);

  /* get the value from application */
  listIndex = keyCosQueueDropPrecIndexValue - L7_QOS_COS_DROP_PREC_LEVEL_MIN;
  owa.l7rc = usmDbQosCosQueueDropParmsListGet(L7_UNIT_CURRENT, keyCosQueueIntfIndexValue, &dropParmsList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objCosQueueMgmtTailDropThresholdValue = dropParmsList.queue[keyCosQueueIndexValue].tailDropMaxThreshold[listIndex];
  /* return the object value: CosQueueMgmtTailDropThreshold */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objCosQueueMgmtTailDropThresholdValue,
                    sizeof (objCosQueueMgmtTailDropThresholdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtTailDropThreshold
*
* @purpose Set 'CosQueueMgmtTailDropThreshold'
*
* @description Tail drop queue threshold above which all packets are dropped 
*              for the current drop precedence level.The value specifies the 
*              threshold based on a fraction of the overall device queue size 
*              in terms of sixteenths (0/16, 1/16, 2/16…, 16/16). Since device 
*              impl 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtTailDropThreshold (void
                                                                           *wap,
                                                                           void
                                                                           *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueMgmtTailDropThresholdValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIntfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueDropPrecIndexValue;
  L7_qosCosDropParmsList_t dList;
  L7_uint32 listIndex;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CosQueueMgmtTailDropThreshold */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCosQueueMgmtTailDropThresholdValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCosQueueMgmtTailDropThresholdValue, owa.len);

  /* retrieve key: CosQueueIntfIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIntfIndex,
                   (xLibU8_t *) & keyCosQueueIntfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIntfIndexValue, kwa1.len);

  /* retrieve key: CosQueueIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                           (xLibU8_t *) & keyCosQueueIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIndexValue, kwa2.len);

  /* retrieve key: CosQueueDropPrecIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueDropPrecIndex,
                   (xLibU8_t *) & keyCosQueueDropPrecIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueDropPrecIndexValue, kwa3.len);

  /* set the value in application */
  listIndex = keyCosQueueDropPrecIndexValue - L7_QOS_COS_DROP_PREC_LEVEL_MIN;
  owa.l7rc = usmDbQosCosQueueDropParmsListGet(L7_UNIT_CURRENT, keyCosQueueIntfIndexValue, &dList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  dList.queue[keyCosQueueIndexValue].tailDropMaxThreshold[listIndex] = objCosQueueMgmtTailDropThresholdValue;
  owa.l7rc = usmDbQosCosQueueDropParmsListSet(L7_UNIT_CURRENT, keyCosQueueIntfIndexValue, &dList);
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
* @function fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtWredMinThreshold
*
* @purpose Get 'CosQueueMgmtWredMinThreshold'
*
* @description Weighted RED minimum queue threshold, below which no packets are 
*              dropped for the current drop precedence level 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtWredMinThreshold (void
                                                                          *wap,
                                                                          void
                                                                          *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIntfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueDropPrecIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueMgmtWredMinThresholdValue;
  L7_qosCosDropParmsList_t dropParmsList;
  L7_uint32 listIndex;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CosQueueIntfIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIntfIndex,
                   (xLibU8_t *) & keyCosQueueIntfIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIntfIndexValue, kwa1.len);

  /* retrieve key: CosQueueIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                           (xLibU8_t *) & keyCosQueueIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIndexValue, kwa2.len);

  /* retrieve key: CosQueueDropPrecIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueDropPrecIndex,
                   (xLibU8_t *) & keyCosQueueDropPrecIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueDropPrecIndexValue, kwa3.len);

  /* get the value from application */
  listIndex = keyCosQueueDropPrecIndexValue - L7_QOS_COS_DROP_PREC_LEVEL_MIN;
  owa.l7rc = usmDbQosCosQueueDropParmsListGet(L7_UNIT_CURRENT, keyCosQueueIntfIndexValue, &dropParmsList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objCosQueueMgmtWredMinThresholdValue = dropParmsList.queue[keyCosQueueIndexValue].minThreshold[listIndex];
  /* return the object value: CosQueueMgmtWredMinThreshold */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objCosQueueMgmtWredMinThresholdValue,
                    sizeof (objCosQueueMgmtWredMinThresholdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtWredMinThreshold
*
* @purpose Set 'CosQueueMgmtWredMinThreshold'
*
* @description Weighted RED minimum queue threshold, below which no packets are 
*              dropped for the current drop precedence level 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtWredMinThreshold (void
                                                                          *wap,
                                                                          void
                                                                          *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueMgmtWredMinThresholdValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIntfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueDropPrecIndexValue;
  L7_qosCosDropParmsList_t dList;
  L7_uint32 listIndex;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CosQueueMgmtWredMinThreshold */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCosQueueMgmtWredMinThresholdValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCosQueueMgmtWredMinThresholdValue, owa.len);

  /* retrieve key: CosQueueIntfIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIntfIndex,
                   (xLibU8_t *) & keyCosQueueIntfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIntfIndexValue, kwa1.len);

  /* retrieve key: CosQueueIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                           (xLibU8_t *) & keyCosQueueIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIndexValue, kwa2.len);

  /* retrieve key: CosQueueDropPrecIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueDropPrecIndex,
                   (xLibU8_t *) & keyCosQueueDropPrecIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueDropPrecIndexValue, kwa3.len);

  /* set the value in application */
  listIndex = keyCosQueueDropPrecIndexValue - L7_QOS_COS_DROP_PREC_LEVEL_MIN;
  owa.l7rc = usmDbQosCosQueueDropParmsListGet(L7_UNIT_CURRENT, keyCosQueueIntfIndexValue, &dList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  dList.queue[keyCosQueueIndexValue].minThreshold[listIndex] = objCosQueueMgmtWredMinThresholdValue;
  owa.l7rc = usmDbQosCosQueueDropParmsListSet(L7_UNIT_CURRENT, keyCosQueueIntfIndexValue, &dList);
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
* @function fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtWredMaxThreshold
*
* @purpose Get 'CosQueueMgmtWredMaxThreshold'
*
* @description Weighted RED maximum queue threshold, above which all packets 
*              are dropped for the current drop precedence level. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtWredMaxThreshold (void
                                                                          *wap,
                                                                          void
                                                                          *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIntfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueDropPrecIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueMgmtWredMaxThresholdValue;
  L7_qosCosDropParmsList_t dropParmsList;
  L7_uint32 listIndex;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CosQueueIntfIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIntfIndex,
                   (xLibU8_t *) & keyCosQueueIntfIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIntfIndexValue, kwa1.len);

  /* retrieve key: CosQueueIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                           (xLibU8_t *) & keyCosQueueIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIndexValue, kwa2.len);

  /* retrieve key: CosQueueDropPrecIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueDropPrecIndex,
                   (xLibU8_t *) & keyCosQueueDropPrecIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueDropPrecIndexValue, kwa3.len);

  /* get the value from application */
  listIndex = keyCosQueueDropPrecIndexValue - L7_QOS_COS_DROP_PREC_LEVEL_MIN;
  owa.l7rc = usmDbQosCosQueueDropParmsListGet(L7_UNIT_CURRENT, keyCosQueueIntfIndexValue, &dropParmsList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objCosQueueMgmtWredMaxThresholdValue = dropParmsList.queue[keyCosQueueIndexValue].wredMaxThreshold[listIndex];
  /* return the object value: CosQueueMgmtWredMaxThreshold */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objCosQueueMgmtWredMaxThresholdValue,
                    sizeof (objCosQueueMgmtWredMaxThresholdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtWredMaxThreshold
*
* @purpose Set 'CosQueueMgmtWredMaxThreshold'
*
* @description Weighted RED maximum queue threshold, above which all packets 
*              are dropped for the current drop precedence level. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtWredMaxThreshold (void
                                                                          *wap,
                                                                          void
                                                                          *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueMgmtWredMaxThresholdValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIntfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueDropPrecIndexValue;
  L7_qosCosDropParmsList_t dList;
  L7_uint32 listIndex;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CosQueueMgmtWredMaxThreshold */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCosQueueMgmtWredMaxThresholdValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCosQueueMgmtWredMaxThresholdValue, owa.len);

  /* retrieve key: CosQueueIntfIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIntfIndex,
                   (xLibU8_t *) & keyCosQueueIntfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIntfIndexValue, kwa1.len);

  /* retrieve key: CosQueueIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                           (xLibU8_t *) & keyCosQueueIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIndexValue, kwa2.len);

  /* retrieve key: CosQueueDropPrecIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueDropPrecIndex,
                   (xLibU8_t *) & keyCosQueueDropPrecIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueDropPrecIndexValue, kwa3.len);

  /* set the value in application */
  listIndex = keyCosQueueDropPrecIndexValue - L7_QOS_COS_DROP_PREC_LEVEL_MIN;
  owa.l7rc = usmDbQosCosQueueDropParmsListGet(L7_UNIT_CURRENT, keyCosQueueIntfIndexValue, &dList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  dList.queue[keyCosQueueIndexValue].wredMaxThreshold[listIndex] = objCosQueueMgmtWredMaxThresholdValue;
  owa.l7rc = usmDbQosCosQueueDropParmsListSet(L7_UNIT_CURRENT, keyCosQueueIntfIndexValue, &dList);
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
* @function fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtWredDropProbScale
*
* @purpose Get 'CosQueueMgmtWredDropProbScale'
*
* @description A scaling factor used for the WRED calculation to determine the 
*              packet drop probability for the current drop precedence level. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtWredDropProbScale (void
                                                                           *wap,
                                                                           void
                                                                           *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIntfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueDropPrecIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueMgmtWredDropProbScaleValue;
  L7_qosCosDropParmsList_t dropParmsList;
  L7_uint32 listIndex;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CosQueueIntfIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIntfIndex,
                   (xLibU8_t *) & keyCosQueueIntfIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIntfIndexValue, kwa1.len);

  /* retrieve key: CosQueueIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                           (xLibU8_t *) & keyCosQueueIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIndexValue, kwa2.len);

  /* retrieve key: CosQueueDropPrecIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueDropPrecIndex,
                   (xLibU8_t *) & keyCosQueueDropPrecIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueDropPrecIndexValue, kwa3.len);

  /* get the value from application */
  listIndex = keyCosQueueDropPrecIndexValue - L7_QOS_COS_DROP_PREC_LEVEL_MIN;
  owa.l7rc = usmDbQosCosQueueDropParmsListGet(L7_UNIT_CURRENT, keyCosQueueIntfIndexValue, &dropParmsList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objCosQueueMgmtWredDropProbScaleValue = dropParmsList.queue[keyCosQueueIndexValue].dropProb[listIndex];
  /* return the object value: CosQueueMgmtWredDropProbScale */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objCosQueueMgmtWredDropProbScaleValue,
                    sizeof (objCosQueueMgmtWredDropProbScaleValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtWredDropProbScale
*
* @purpose Set 'CosQueueMgmtWredDropProbScale'
*
* @description A scaling factor used for the WRED calculation to determine the 
*              packet drop probability for the current drop precedence level. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtWredDropProbScale (void
                                                                           *wap,
                                                                           void
                                                                           *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueMgmtWredDropProbScaleValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIntfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueDropPrecIndexValue;
  L7_qosCosDropParmsList_t dList;
  L7_uint32 listIndex;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CosQueueMgmtWredDropProbScale */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCosQueueMgmtWredDropProbScaleValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCosQueueMgmtWredDropProbScaleValue, owa.len);

  /* retrieve key: CosQueueIntfIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIntfIndex,
                   (xLibU8_t *) & keyCosQueueIntfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIntfIndexValue, kwa1.len);

  /* retrieve key: CosQueueIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                           (xLibU8_t *) & keyCosQueueIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIndexValue, kwa2.len);

  /* retrieve key: CosQueueDropPrecIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueDropPrecIndex,
                   (xLibU8_t *) & keyCosQueueDropPrecIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueDropPrecIndexValue, kwa3.len);

  /* set the value in application */
  listIndex = keyCosQueueDropPrecIndexValue - L7_QOS_COS_DROP_PREC_LEVEL_MIN;
  owa.l7rc = usmDbQosCosQueueDropParmsListGet(L7_UNIT_CURRENT, keyCosQueueIntfIndexValue, &dList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  dList.queue[keyCosQueueIndexValue].dropProb[listIndex] = objCosQueueMgmtWredDropProbScaleValue;
  owa.l7rc = usmDbQosCosQueueDropParmsListSet(L7_UNIT_CURRENT, keyCosQueueIntfIndexValue, &dList);
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
* @function fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueNumQueuesPerPort
*
* @purpose Get 'CosQueueNumQueuesPerPort'
*
* @description number of configurable queues per port 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueNumQueuesPerPort (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueNumQueuesPerPortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbQosCosQueueNumQueuesPerPortGet (L7_UNIT_CURRENT,
                                         &objCosQueueNumQueuesPerPortValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCosQueueNumQueuesPerPortValue,
                     sizeof (objCosQueueNumQueuesPerPortValue));

  /* return the object value: CosQueueNumQueuesPerPort */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objCosQueueNumQueuesPerPortValue,
                    sizeof (objCosQueueNumQueuesPerPortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueNumDropPrecedenceLevels
*
* @purpose Get 'CosQueueNumDropPrecedenceLevels'
*
* @description number of drop precedence levels supported per queue 
*              
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueNumDropPrecedenceLevels (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueNumDropPrecedenceLevelsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbQosCosQueueNumDropPrecLevelsGet (L7_UNIT_CURRENT,
                                          &objCosQueueNumDropPrecedenceLevelsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCosQueueNumDropPrecedenceLevelsValue,
                     sizeof (objCosQueueNumDropPrecedenceLevelsValue));

  /* return the object value: CosQueueNumDropPrecedenceLevels */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objCosQueueNumDropPrecedenceLevelsValue,
                    sizeof (objCosQueueNumDropPrecedenceLevelsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueIntfShapingRate
*
* @purpose Get 'CosQueueIntfShapingRate'
*
* @description egress shaping rate forspecified interface 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueIntfShapingRate (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIntfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueIntfShapingRateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CosQueueIntfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIntfIndex,
                          (xLibU8_t *) & keyCosQueueIntfIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIntfIndexValue, kwa.len);
  
  /* get the value from application */
  owa.l7rc =
    usmDbQosCosQueueIntfShapingRateGet (L7_UNIT_CURRENT,
                                        keyCosQueueIntfIndexValue,
                                        &objCosQueueIntfShapingRateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  /* return the object value: CosQueueIntfShapingRate */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCosQueueIntfShapingRateValue,
                           sizeof (objCosQueueIntfShapingRateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueIntfShapingRate
*
* @purpose Set 'CosQueueIntfShapingRate'
*
* @description egress shaping rate forspecified interface 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueIntfShapingRate (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueIntfShapingRateValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIntfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CosQueueIntfShapingRate */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCosQueueIntfShapingRateValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCosQueueIntfShapingRateValue, owa.len);

  /* retrieve key: CosQueueIntfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIntfIndex,
                          (xLibU8_t *) & keyCosQueueIntfIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIntfIndexValue, kwa.len);

  if (L7_QOS_COS_INTF_SHAPING_RATE_UNITS == L7_RATE_UNIT_KBPS){ 
  
  
  /* set the value in application */
  if ((objCosQueueIntfShapingRateValue < L7_QOS_COS_INTF_SHAPING_RATE_KBPS_MIN) &&
      (objCosQueueIntfShapingRateValue > L7_QOS_COS_INTF_SHAPING_RATE_KBPS_MAX) )
     /* && ((objCosQueueIntfShapingRateValue % L7_QOS_COS_QUEUE_BANDWIDTH_STEP_SIZE) != 0)) */
  {
    owa.rc = XLIBRC_IMPROPER_VALUE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  }
  else
  {
      /* check proposed value */
      if ((objCosQueueIntfShapingRateValue < L7_QOS_COS_INTF_SHAPING_RATE_MIN) ||
          (objCosQueueIntfShapingRateValue > L7_QOS_COS_INTF_SHAPING_RATE_MAX))
      {
        owa.rc = XLIBRC_FAILURE;
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
  
     /* check percentage as an integral step size amount between min, max */
     if ((objCosQueueIntfShapingRateValue != L7_QOS_COS_INTF_SHAPING_RATE_MIN) &&
         (objCosQueueIntfShapingRateValue != L7_QOS_COS_INTF_SHAPING_RATE_MAX) &&
         ((objCosQueueIntfShapingRateValue % L7_QOS_COS_QUEUE_BANDWIDTH_STEP_SIZE) != 0))
      {
        owa.rc = XLIBRC_FAILURE;
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }  
  }

  owa.l7rc =
    usmDbQosCosQueueIntfShapingRateSet (L7_UNIT_CURRENT,
                                        keyCosQueueIntfIndexValue,
                                        objCosQueueIntfShapingRateValue);
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
* @function fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtTypeIntf
*
* @purpose Get 'CosQueueMgmtTypeIntf'
*
* @description Queue depth management technique used for queues on this interface. 
*              This is only used if device supports independent settings 
*              per-queue.Queue Management Type can only be one of the following: 
*              1)taildrop.Default value is taildrop. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtTypeIntf (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIntfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueMgmtTypeIntfValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CosQueueIntfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIntfIndex,
                          (xLibU8_t *) & keyCosQueueIntfIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIntfIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbQosCosQueueMgmtTypePerIntfGet (L7_UNIT_CURRENT,
                                        keyCosQueueIntfIndexValue,
                                        &objCosQueueMgmtTypeIntfValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: CosQueueMgmtTypeIntf */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCosQueueMgmtTypeIntfValue,
                           sizeof (objCosQueueMgmtTypeIntfValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtTypeIntf
*
* @purpose Set 'CosQueueMgmtTypeIntf'
*
* @description Queue depth management technique used for queues on this interface. 
*              This is only used if device supports independent settings 
*              per-queue.Queue Management Type can only be one of the following: 
*              1)taildrop.Default value is taildrop. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtTypeIntf (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueMgmtTypeIntfValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIntfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CosQueueMgmtTypeIntf */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCosQueueMgmtTypeIntfValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCosQueueMgmtTypeIntfValue, owa.len);

  /* retrieve key: CosQueueIntfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIntfIndex,
                          (xLibU8_t *) & keyCosQueueIntfIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIntfIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbQosCosQueueMgmtTypePerIntfSet (L7_UNIT_CURRENT,
                                        keyCosQueueIntfIndexValue,
                                        objCosQueueMgmtTypeIntfValue);
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
* @function fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueWredDecayExponent
*
* @purpose Get 'CosQueueWredDecayExponent'
*
* @description The management technique used for all queues on the specified 
*              interface 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueWredDecayExponent (void
                                                                       *wap,
                                                                       void
                                                                       *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIntfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueWredDecayExponentValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CosQueueIntfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIntfIndex,
                          (xLibU8_t *) & keyCosQueueIntfIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIntfIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbQosCosQueueWredDecayExponentGet (L7_UNIT_CURRENT,
                                          keyCosQueueIntfIndexValue,
                                          &objCosQueueWredDecayExponentValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: CosQueueWredDecayExponent */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objCosQueueWredDecayExponentValue,
                    sizeof (objCosQueueWredDecayExponentValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueWredDecayExponent
*
* @purpose Set 'CosQueueWredDecayExponent'
*
* @description The management technique used for all queues on the specified 
*              interface 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueWredDecayExponent (void
                                                                       *wap,
                                                                       void
                                                                       *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueWredDecayExponentValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIntfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CosQueueWredDecayExponent */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCosQueueWredDecayExponentValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCosQueueWredDecayExponentValue, owa.len);

  /* retrieve key: CosQueueIntfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIntfIndex,
                          (xLibU8_t *) & keyCosQueueIntfIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIntfIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbQosCosQueueWredDecayExponentSet (L7_UNIT_CURRENT,
                                          keyCosQueueIntfIndexValue,
                                          objCosQueueWredDecayExponentValue);
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
* @function fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueDefaultsRestore
*
* @purpose Get 'CosQueueDefaultsRestore'
*
* @description Causes the default values to be restored for all COS queue objects 
*              defined for this interface 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueDefaultsRestore (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIntfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueDefaultsRestoreValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CosQueueIntfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIntfIndex,
                          (xLibU8_t *) & keyCosQueueIntfIndexValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIntfIndexValue, kwa.len);

  /* get the value from application */
#if 0
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keyCosQueueIntfIndexValue,
                              &objCosQueueDefaultsRestoreValue);
#endif
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: CosQueueDefaultsRestore */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCosQueueDefaultsRestoreValue,
                           sizeof (objCosQueueDefaultsRestoreValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueDefaultsRestore
*
* @purpose Set 'CosQueueDefaultsRestore'
*
* @description Causes the default values to be restored for all COS queue objects 
*              defined for this interface 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueDefaultsRestore (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueDefaultsRestoreValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIntfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CosQueueDefaultsRestore */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCosQueueDefaultsRestoreValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCosQueueDefaultsRestoreValue, owa.len);

  /* retrieve key: CosQueueIntfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIntfIndex,
                          (xLibU8_t *) & keyCosQueueIntfIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIntfIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbQosCosQueueDefaultsRestore (L7_UNIT_CURRENT, keyCosQueueIntfIndexValue);
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
* @function fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueIntfIndex
*
* @purpose Get 'CosQueueIntfIndex'
*
* @description Specifies all CoS Queue configurable interfaces. 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueIntfIndex (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueIntfIndexValue;
  xLibU32_t nextObjCosQueueIntfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CosQueueIntfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIntfIndex,
                          (xLibU8_t *) & objCosQueueIntfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbValidIntIfNumFirstGet(&nextObjCosQueueIntfIndexValue);
    if (usmDbQosCosQueueIntfIndexGet(L7_NULL, nextObjCosQueueIntfIndexValue) != L7_SUCCESS)
    {
      owa.l7rc = L7_FAILURE;
    }
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objCosQueueIntfIndexValue, owa.len);
    owa.l7rc =
      usmDbQosCosQueueIntfIndexGetNext (L7_UNIT_CURRENT,
                                        objCosQueueIntfIndexValue,
                                        &nextObjCosQueueIntfIndexValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjCosQueueIntfIndexValue, owa.len);

  /* return the object value: CosQueueIntfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjCosQueueIntfIndexValue,
                           sizeof (objCosQueueIntfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueIndex
*
* @purpose Get 'CosQueueIndex'
*
* @description The COS queue index, numbered 0 to (n-1), where n is the total 
*              number of configurable interface queues for the device as indicated 
*              by agentCosQueueNumQueuesPerPort.In general, a higher 
*              numbered queue index is used to support higher priority traffic, 
*              alth 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueIndex (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueIndexValue;
  xLibU32_t nextObjCosQueueIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CosQueueIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                          (xLibU8_t *) & objCosQueueIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjCosQueueIndexValue=L7_QOS_COS_QUEUE_ID_MIN;
    owa.l7rc = usmDbQosCosQueueIdIndexGet(L7_UNIT_CURRENT,nextObjCosQueueIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objCosQueueIndexValue, owa.len);
    owa.l7rc = usmDbQosCosQueueIdIndexGetNext (L7_UNIT_CURRENT, objCosQueueIndexValue,
                                               &nextObjCosQueueIndexValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjCosQueueIndexValue, owa.len);

  /* return the object value: CosQueueIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjCosQueueIndexValue,
                           sizeof (objCosQueueIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueSchedulerType
*
* @purpose Get 'CosQueueSchedulerType'
*
* @description Specifies the type of scheduling used for this queue.Scheduler 
*              Type can only be one of the following: 1)strict 2)weighted 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueSchedulerType (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIntfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  L7_qosCosQueueSchedTypeList_t schedTypeList;
  xLibU32_t objCosQueueSchedulerTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CosQueueIntfIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIntfIndex,
                   (xLibU8_t *) & keyCosQueueIntfIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIntfIndexValue, kwa1.len);

  /* retrieve key: CosQueueIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                           (xLibU8_t *) & keyCosQueueIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIndexValue, kwa2.len);

  /* get the value from application */
  L7_uint32 listIndex = keyCosQueueIndexValue - L7_QOS_COS_QUEUE_ID_MIN;
  L7_uint32 temp;


  owa.l7rc =
    usmDbQosCosQueueSchedulerTypeListGet (L7_UNIT_CURRENT,
                                      keyCosQueueIntfIndexValue,
                                      &schedTypeList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  temp = schedTypeList.schedType[listIndex];
  /*objCosQueueSchedulerTypeValue = schedTypeList.schedType[listIndex];*/
   objCosQueueSchedulerTypeValue = temp;
  /* return the object value: CosQueueSchedulerType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCosQueueSchedulerTypeValue,
                           sizeof (objCosQueueSchedulerTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueSchedulerType
*
* @purpose Set 'CosQueueSchedulerType'
*
* @description Specifies the type of scheduling used for this queue.Scheduler 
*              Type can only be one of the following: 1)strict 2)weighted 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueSchedulerType (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueSchedulerTypeValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIntfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CosQueueSchedulerType */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCosQueueSchedulerTypeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCosQueueSchedulerTypeValue, owa.len);

  /* retrieve key: CosQueueIntfIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIntfIndex,
                   (xLibU8_t *) & keyCosQueueIntfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIntfIndexValue, kwa1.len);

  /* retrieve key: CosQueueIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                           (xLibU8_t *) & keyCosQueueIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIndexValue, kwa2.len);
  /* set the value in application */
  owa.l7rc =
    usmDbQosCosQueueSchedulerTypeSet (L7_UNIT_CURRENT,
                                      keyCosQueueIntfIndexValue,
                                      keyCosQueueIndexValue,
                                      objCosQueueSchedulerTypeValue);
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
* @function fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueMinBandwidth
*
* @purpose Get 'CosQueueMinBandwidth'
*
* @description Specifies the minimum guaranteed bandwidth allotted to this queue. 
*              Valid Range is (0 to 100)in increments of 5.The value 0 
*              means no guaranteed minimum. Sum of individual Minimum Bandwidth 
*              values for all queues in the selected interface cannot exceed 
*              defin 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueMinBandwidth (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIntfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  L7_qosCosQueueBwList_t minbwList;
  xLibU32_t objCosQueueMinBandwidthValue;
  L7_uint32 listIndex;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CosQueueIntfIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIntfIndex,
                   (xLibU8_t *) & keyCosQueueIntfIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIntfIndexValue, kwa1.len);

  /* retrieve key: CosQueueIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                           (xLibU8_t *) & keyCosQueueIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIndexValue, kwa2.len);
   listIndex = L7_QOS_COS_QUEUE_ID_MIN;
   if(keyCosQueueIndexValue >= L7_QOS_COS_QUEUE_ID_MIN && keyCosQueueIndexValue <= L7_QOS_COS_QUEUE_ID_MAX)
   {
     listIndex = keyCosQueueIndexValue;
   }
  /* get the value from application */
  owa.l7rc =
    usmDbQosCosQueueMinBandwidthListGet(L7_UNIT_CURRENT,
                                        keyCosQueueIntfIndexValue,
                                        &minbwList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objCosQueueMinBandwidthValue = minbwList.bandwidth[listIndex];
  /* return the object value: CosQueueMinBandwidth */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCosQueueMinBandwidthValue,
                           sizeof (objCosQueueMinBandwidthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueMinBandwidth
*
* @purpose Set 'CosQueueMinBandwidth'
*
* @description Specifies the minimum guaranteed bandwidth allotted to this queue. 
*              Valid Range is (0 to 100)in increments of 5.The value 0 
*              means no guaranteed minimum. Sum of individual Minimum Bandwidth 
*              values for all queues in the selected interface cannot exceed 
*              defin 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueMinBandwidth (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueMinBandwidthValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIntfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIndexValue;
  L7_uint32 listIndex;
  L7_uint32 total_bw=0;
  L7_uint32 i;
  L7_qosCosQueueBwList_t minbwList;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CosQueueMinBandwidth */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCosQueueMinBandwidthValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCosQueueMinBandwidthValue, owa.len);

  /* retrieve key: CosQueueIntfIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIntfIndex,
                   (xLibU8_t *) & keyCosQueueIntfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIntfIndexValue, kwa1.len);

  /* retrieve key: CosQueueIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                           (xLibU8_t *) & keyCosQueueIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIndexValue, kwa2.len);

  /* set the value in application */
  if((objCosQueueMinBandwidthValue != L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MIN) && (objCosQueueMinBandwidthValue != L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MAX) &&
           ((objCosQueueMinBandwidthValue % L7_QOS_COS_QUEUE_BANDWIDTH_STEP_SIZE) != 0))
        {
          owa.rc = XLIBRC_IMPROPER_VALUE;    /* TODO: Change if required */
          FPOBJ_TRACE_EXIT (bufp, owa);
          return owa.rc;
        }
        (void)usmDbQosCosQueueMinBandwidthListGet(L7_UNIT_CURRENT,keyCosQueueIntfIndexValue,&minbwList);
        listIndex = keyCosQueueIndexValue - L7_QOS_COS_QUEUE_ID_MIN;
        minbwList.bandwidth[listIndex] = objCosQueueMinBandwidthValue;
        for(i=L7_QOS_COS_QUEUE_ID_MIN;i<=L7_QOS_COS_QUEUE_ID_MAX;i++)
           total_bw = total_bw + minbwList.bandwidth[i - L7_QOS_COS_QUEUE_ID_MIN];
        if (total_bw > L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MAX)
        {
          owa.rc = XLIBRC_MIN_BWIDTH_EXCEEDS;    /* TODO: Change if required */
          FPOBJ_TRACE_EXIT (bufp, owa);
          return owa.rc;
        }

  owa.l7rc =
    usmDbQosCosQueueMinBandwidthSet (L7_UNIT_CURRENT, keyCosQueueIntfIndexValue,
                                     keyCosQueueIndexValue,
                                     objCosQueueMinBandwidthValue);
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
* @function fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueMaxBandwidth
*
* @purpose Get 'CosQueueMaxBandwidth'
*
* @description Specifies the maximum allowed bandwidth allotted to this queue. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueMaxBandwidth (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIntfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  L7_qosCosQueueBwList_t maxbwList;
  xLibU32_t objCosQueueMaxBandwidthValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CosQueueIntfIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIntfIndex,
                   (xLibU8_t *) & keyCosQueueIntfIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIntfIndexValue, kwa1.len);

  /* retrieve key: CosQueueIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                           (xLibU8_t *) & keyCosQueueIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIndexValue, kwa2.len);

  /* get the value from application */
   L7_uint32 listIndex = keyCosQueueIndexValue - L7_QOS_COS_QUEUE_ID_MIN;
  owa.l7rc = usmDbQosCosQueueMaxBandwidthListGet(L7_UNIT_CURRENT,
                                                 keyCosQueueIntfIndexValue,
                                                 &maxbwList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objCosQueueMaxBandwidthValue = maxbwList.bandwidth[listIndex];

  /* return the object value: CosQueueMaxBandwidth */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCosQueueMaxBandwidthValue,
                           sizeof (objCosQueueMaxBandwidthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueMaxBandwidth
*
* @purpose Set 'CosQueueMaxBandwidth'
*
* @description Specifies the maximum allowed bandwidth allotted to this queue. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueMaxBandwidth (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueMaxBandwidthValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIntfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CosQueueMaxBandwidth */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCosQueueMaxBandwidthValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCosQueueMaxBandwidthValue, owa.len);

  /* retrieve key: CosQueueIntfIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIntfIndex,
                   (xLibU8_t *) & keyCosQueueIntfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIntfIndexValue, kwa1.len);

  /* retrieve key: CosQueueIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                           (xLibU8_t *) & keyCosQueueIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc =
    usmDbQosCosQueueMaxBandwidthSet (L7_UNIT_CURRENT, keyCosQueueIntfIndexValue,
                                     keyCosQueueIndexValue,
                                     objCosQueueMaxBandwidthValue);
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
* @function fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtType
*
* @purpose Get 'CosQueueMgmtType'
*
* @description Queue depth management technique used for queues on this interface. 
*              This is only used if device supports independent settings 
*              per-queue 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtType (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIntfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  L7_qosCosQueueMgmtTypeList_t MgmtTypeList;
  xLibU32_t objCosQueueMgmtTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CosQueueIntfIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIntfIndex,
                   (xLibU8_t *) & keyCosQueueIntfIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIntfIndexValue, kwa1.len);

  /* retrieve key: CosQueueIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                           (xLibU8_t *) & keyCosQueueIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIndexValue, kwa2.len);

  /* get the value from application */
   L7_uint32 listIndex = keyCosQueueIndexValue - L7_QOS_COS_QUEUE_ID_MIN;
  owa.l7rc =
    usmDbQosCosQueueMgmtTypeListGet (L7_UNIT_CURRENT, keyCosQueueIntfIndexValue,
                                     &MgmtTypeList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objCosQueueMgmtTypeValue = MgmtTypeList.mgmtType[listIndex]; 
  /* return the object value: CosQueueMgmtType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCosQueueMgmtTypeValue,
                           sizeof (objCosQueueMgmtTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtType
*
* @purpose Set 'CosQueueMgmtType'
*
* @description Queue depth management technique used for queues on this interface. 
*              This is only used if device supports independent settings 
*              per-queue 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtType (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueMgmtTypeValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIntfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CosQueueMgmtType */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCosQueueMgmtTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCosQueueMgmtTypeValue, owa.len);

  /* retrieve key: CosQueueIntfIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIntfIndex,
                   (xLibU8_t *) & keyCosQueueIntfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIntfIndexValue, kwa1.len);

  /* retrieve key: CosQueueIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                           (xLibU8_t *) & keyCosQueueIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc =
    usmDbQosCosQueueMgmtTypeSet (L7_UNIT_CURRENT, keyCosQueueIntfIndexValue,
                                 keyCosQueueIndexValue,
                                 objCosQueueMgmtTypeValue);
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
* @function fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueIntfRestoreDefaults
*
* @purpose Get 'Restore Defaults'
*
* @description [CosQueueIntfRestoreDefaults]: Restore egress shaping rate
*              for specified interface to default value
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueIntfRestoreDefaults (void
                                                                         *wap,
                                                                         void
                                                                         *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIntfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueIntfRestoreDefaultsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CosQueueIntfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIntfIndex,
                          (xLibU8_t *) & keyCosQueueIntfIndexValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIntfIndexValue, kwa.len);
#if 0 /*To be Implemented only for SNMP */
  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keyCosQueueIntfIndexValue,
                              &objCosQueueIntfRestoreDefaultsValue);
#endif
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: CosQueueIntfRestoreDefaults */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objCosQueueIntfRestoreDefaultsValue,
                    sizeof (objCosQueueIntfRestoreDefaultsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueIntfRestoreDefaults
*
* @purpose Set 'Restore Defaults'
*
* @description [CosQueueIntfRestoreDefaults]: Restore egress shaping rate
*              for specified interface to default value
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueIntfRestoreDefaults (void
                                                                         *wap,
                                                                         void
                                                                         *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueIntfRestoreDefaultsValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIntfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CosQueueIntfRestoreDefaults */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCosQueueIntfRestoreDefaultsValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCosQueueIntfRestoreDefaultsValue, owa.len);

  /* retrieve key: CosQueueIntfIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIntfIndex,
                          (xLibU8_t *) & keyCosQueueIntfIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIntfIndexValue, kwa.len);
  
  if(objCosQueueIntfRestoreDefaultsValue)
  {
    owa.l7rc = usmDbQosCosQueueIntfShapingRateSet(L7_UNIT_CURRENT, keyCosQueueIntfIndexValue,FD_QOS_COS_QCFG_INTF_SHAPING_RATE);
  }

  /* set the value in application */
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(objCosQueueIntfRestoreDefaultsValue)
  {
    if ((usmDbFeaturePresentCheck(L7_UNIT_CURRENT, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_WRED_SUPPORT_FEATURE_ID) == L7_TRUE) &&
            (usmDbFeaturePresentCheck(L7_UNIT_CURRENT, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_WRED_DECAY_EXP_SYSTEM_ONLY_FEATURE_ID) != L7_TRUE))
    {
        owa.l7rc = usmDbQosCosQueueWredDecayExponentSet(L7_UNIT_CURRENT, keyCosQueueIntfIndexValue,FD_QOS_COS_QCFG_WRED_DECAY_EXP);

        if (owa.l7rc != L7_SUCCESS)
        {
           owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
           FPOBJ_TRACE_EXIT (bufp, owa);
           return owa.rc;
        }
    }
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueMinBandwidthAllocated
*
* @purpose Get 'Mininum Bandwidth Allocated'
*
* @description [CosQueueMinBandwidthAllocated]: Specifies the minimum bandwidth
*              allotted to all queues currently. Valid Range is (0 to
*              100)in increments of 5.The value 0 means no guaranteed minimum.
*              Sum of individual Minimum Bandwidth values for all queues
*              in the selected interface cannot exceed 100
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueMinBandwidthAllocated (void
                                                                           *wap,
                                                                           void
                                                                           *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIntfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueMinBandwidthAllocatedValue;
  L7_qosCosQueueBwList_t qosCosQueueBwList; 
  L7_uint32 queueId;
  L7_uint32 total_bw=0;  
  FPOBJ_TRACE_ENTER (bufp);
  
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIntfIndex,
                   (xLibU8_t *) & keyCosQueueIntfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }

  
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIntfIndexValue, kwa1.len);

  /* get the value from application */
  owa.l7rc = usmDbQosCosQueueMinBandwidthListGet(L7_UNIT_CURRENT, keyCosQueueIntfIndexValue, &qosCosQueueBwList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  for(queueId=L7_QOS_COS_QUEUE_ID_MIN; queueId<=L7_QOS_COS_QUEUE_ID_MAX; queueId++)
  {
      total_bw = total_bw + qosCosQueueBwList.bandwidth[queueId - L7_QOS_COS_QUEUE_ID_MIN];
  } 
  objCosQueueMinBandwidthAllocatedValue = total_bw;
  /* return the object value: CosQueueMinBandwidthAllocated */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objCosQueueMinBandwidthAllocatedValue,
                    sizeof (objCosQueueMinBandwidthAllocatedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtTailDropThreshold
*
* @purpose Get 'CosQueueMgmtTailDropThreshold'
*
* @description Tail drop queue threshold above which all packets are dropped 
*              for the current drop precedence level.The value specifies the 
*              threshold based on a fraction of the overall device queue size 
*              in terms of sixteenths (0/16, 1/16, 2/16â, 16/16). Since device 
*              impl 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosQueueCfg_GlobalCosQueueMgmtTailDropThreshold (void
                                                                           *wap,
                                                                           void
                                                                           *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueDropPrecIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueMgmtTailDropThresholdValue;
  L7_qosCosDropParmsList_t dropParmsList;
  L7_uint32 listIndex;
  FPOBJ_TRACE_ENTER (bufp);
  

  /* retrieve key: CosQueueIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                           (xLibU8_t *) & keyCosQueueIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIndexValue, kwa2.len);

  /* retrieve key: CosQueueDropPrecIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueDropPrecIndex,
                   (xLibU8_t *) & keyCosQueueDropPrecIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueDropPrecIndexValue, kwa3.len);

  /* get the value from application */
  listIndex = keyCosQueueDropPrecIndexValue - L7_QOS_COS_DROP_PREC_LEVEL_MIN;
  owa.l7rc = usmDbQosCosQueueDropParmsListGet(L7_UNIT_CURRENT, L7_ALL_INTERFACES, &dropParmsList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objCosQueueMgmtTailDropThresholdValue = dropParmsList.queue[keyCosQueueIndexValue].tailDropMaxThreshold[listIndex];
  /* return the object value: CosQueueMgmtTailDropThreshold */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objCosQueueMgmtTailDropThresholdValue,
                    sizeof (objCosQueueMgmtTailDropThresholdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtTailDropThreshold
*
* @purpose Set 'CosQueueMgmtTailDropThreshold'
*
* @description Tail drop queue threshold above which all packets are dropped 
*              for the current drop precedence level.The value specifies the 
*              threshold based on a fraction of the overall device queue size 
*              in terms of sixteenths (0/16, 1/16, 2/16â, 16/16). Since device 
*              impl 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosQueueCfg_GlobalCosQueueMgmtTailDropThreshold (void
                                                                           *wap,
                                                                           void
                                                                           *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueMgmtTailDropThresholdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueDropPrecIndexValue;
  L7_qosCosDropParmsList_t dList;
  L7_uint32 listIndex;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CosQueueMgmtTailDropThreshold */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCosQueueMgmtTailDropThresholdValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCosQueueMgmtTailDropThresholdValue, owa.len);


  /* retrieve key: CosQueueIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                           (xLibU8_t *) & keyCosQueueIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIndexValue, kwa2.len);

  /* retrieve key: CosQueueDropPrecIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueDropPrecIndex,
                   (xLibU8_t *) & keyCosQueueDropPrecIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueDropPrecIndexValue, kwa3.len);

  listIndex = keyCosQueueDropPrecIndexValue - L7_QOS_COS_DROP_PREC_LEVEL_MIN;
  owa.l7rc = usmDbQosCosQueueDropParmsListGet(L7_UNIT_CURRENT, L7_ALL_INTERFACES, &dList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  dList.queue[keyCosQueueIndexValue].tailDropMaxThreshold[listIndex] = objCosQueueMgmtTailDropThresholdValue;
  owa.l7rc = usmDbQosCosQueueDropParmsListSet(L7_UNIT_CURRENT, L7_ALL_INTERFACES, &dList);
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
* @function fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtWredMinThreshold
*
* @purpose Get 'CosQueueMgmtWredMinThreshold'
*
* @description Weighted RED minimum queue threshold, below which no packets are 
*              dropped for the current drop precedence level 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosQueueCfg_GlobalCosQueueMgmtWredMinThreshold (void
                                                                          *wap,
                                                                          void
                                                                          *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueDropPrecIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueMgmtWredMinThresholdValue;
  L7_qosCosDropParmsList_t dropParmsList;
  L7_uint32 listIndex;
  FPOBJ_TRACE_ENTER (bufp);


  /* retrieve key: CosQueueIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                           (xLibU8_t *) & keyCosQueueIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIndexValue, kwa2.len);

  /* retrieve key: CosQueueDropPrecIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueDropPrecIndex,
                   (xLibU8_t *) & keyCosQueueDropPrecIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueDropPrecIndexValue, kwa3.len);

  /* get the value from application */
  listIndex = keyCosQueueDropPrecIndexValue - L7_QOS_COS_DROP_PREC_LEVEL_MIN;
  owa.l7rc = usmDbQosCosQueueDropParmsListGet(L7_UNIT_CURRENT, L7_ALL_INTERFACES, &dropParmsList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objCosQueueMgmtWredMinThresholdValue = dropParmsList.queue[keyCosQueueIndexValue].minThreshold[listIndex];
  /* return the object value: CosQueueMgmtWredMinThreshold */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objCosQueueMgmtWredMinThresholdValue,
                    sizeof (objCosQueueMgmtWredMinThresholdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtWredMinThreshold
*
* @purpose Set 'CosQueueMgmtWredMinThreshold'
*
* @description Weighted RED minimum queue threshold, below which no packets are 
*              dropped for the current drop precedence level 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosQueueCfg_GlobalCosQueueMgmtWredMinThreshold (void
                                                                          *wap,
                                                                          void
                                                                          *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueMgmtWredMinThresholdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueDropPrecIndexValue;
  L7_qosCosDropParmsList_t dList;
  L7_uint32 listIndex;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CosQueueMgmtWredMinThreshold */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCosQueueMgmtWredMinThresholdValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCosQueueMgmtWredMinThresholdValue, owa.len);


  /* retrieve key: CosQueueIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                           (xLibU8_t *) & keyCosQueueIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIndexValue, kwa2.len);

  /* retrieve key: CosQueueDropPrecIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueDropPrecIndex,
                   (xLibU8_t *) & keyCosQueueDropPrecIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueDropPrecIndexValue, kwa3.len);

  /* set the value in application */
  listIndex = keyCosQueueDropPrecIndexValue - L7_QOS_COS_DROP_PREC_LEVEL_MIN;
  owa.l7rc = usmDbQosCosQueueDropParmsListGet(L7_UNIT_CURRENT, L7_ALL_INTERFACES, &dList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  dList.queue[keyCosQueueIndexValue].minThreshold[listIndex] = objCosQueueMgmtWredMinThresholdValue;
  owa.l7rc = usmDbQosCosQueueDropParmsListSet(L7_UNIT_CURRENT, L7_ALL_INTERFACES, &dList);
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
* @function fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtWredMaxThreshold
*
* @purpose Get 'CosQueueMgmtWredMaxThreshold'
*
* @description Weighted RED maximum queue threshold, above which all packets 
*              are dropped for the current drop precedence level. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosQueueCfg_GlobalCosQueueMgmtWredMaxThreshold (void
                                                                          *wap,
                                                                          void
                                                                          *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueDropPrecIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueMgmtWredMaxThresholdValue;
  L7_qosCosDropParmsList_t dropParmsList;
  L7_uint32 listIndex;
  FPOBJ_TRACE_ENTER (bufp);


  /* retrieve key: CosQueueIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                           (xLibU8_t *) & keyCosQueueIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIndexValue, kwa2.len);

  /* retrieve key: CosQueueDropPrecIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueDropPrecIndex,
                   (xLibU8_t *) & keyCosQueueDropPrecIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueDropPrecIndexValue, kwa3.len);

  /* get the value from application */
  listIndex = keyCosQueueDropPrecIndexValue - L7_QOS_COS_DROP_PREC_LEVEL_MIN;
  owa.l7rc = usmDbQosCosQueueDropParmsListGet(L7_UNIT_CURRENT, L7_ALL_INTERFACES, &dropParmsList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objCosQueueMgmtWredMaxThresholdValue = dropParmsList.queue[keyCosQueueIndexValue].wredMaxThreshold[listIndex];

  /* return the object value: CosQueueMgmtWredMaxThreshold */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objCosQueueMgmtWredMaxThresholdValue,
                    sizeof (objCosQueueMgmtWredMaxThresholdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtWredMaxThreshold
*
* @purpose Set 'CosQueueMgmtWredMaxThreshold'
*
* @description Weighted RED maximum queue threshold, above which all packets 
*              are dropped for the current drop precedence level. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosQueueCfg_GlobalCosQueueMgmtWredMaxThreshold (void
                                                                          *wap,
                                                                          void
                                                                          *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueMgmtWredMaxThresholdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueDropPrecIndexValue;
  L7_qosCosDropParmsList_t dList;
  L7_uint32 listIndex;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CosQueueMgmtWredMaxThreshold */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCosQueueMgmtWredMaxThresholdValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCosQueueMgmtWredMaxThresholdValue, owa.len);


  /* retrieve key: CosQueueIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                           (xLibU8_t *) & keyCosQueueIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIndexValue, kwa2.len);

  /* retrieve key: CosQueueDropPrecIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueDropPrecIndex,
                   (xLibU8_t *) & keyCosQueueDropPrecIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueDropPrecIndexValue, kwa3.len);

  /* set the value in application */
  listIndex = keyCosQueueDropPrecIndexValue - L7_QOS_COS_DROP_PREC_LEVEL_MIN;
  owa.l7rc = usmDbQosCosQueueDropParmsListGet(L7_UNIT_CURRENT, L7_ALL_INTERFACES, &dList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  dList.queue[keyCosQueueIndexValue].wredMaxThreshold[listIndex] = objCosQueueMgmtWredMaxThresholdValue;
  owa.l7rc = usmDbQosCosQueueDropParmsListSet(L7_UNIT_CURRENT, L7_ALL_INTERFACES, &dList);
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
* @function fpObjGet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtWredDropProbScale
*
* @purpose Get 'CosQueueMgmtWredDropProbScale'
*
* @description A scaling factor used for the WRED calculation to determine the 
*              packet drop probability for the current drop precedence level. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosQueueCfg_GlobalCosQueueMgmtWredDropProbScale (void
                                                                           *wap,
                                                                           void
                                                                           *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueDropPrecIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueMgmtWredDropProbScaleValue;
  L7_qosCosDropParmsList_t dropParmsList;
  L7_uint32 listIndex;
  FPOBJ_TRACE_ENTER (bufp);


  /* retrieve key: CosQueueIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                           (xLibU8_t *) & keyCosQueueIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIndexValue, kwa2.len);

  /* retrieve key: CosQueueDropPrecIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueDropPrecIndex,
                   (xLibU8_t *) & keyCosQueueDropPrecIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueDropPrecIndexValue, kwa3.len);

  /* get the value from application */
  listIndex = keyCosQueueDropPrecIndexValue - L7_QOS_COS_DROP_PREC_LEVEL_MIN;
  owa.l7rc = usmDbQosCosQueueDropParmsListGet(L7_UNIT_CURRENT, L7_ALL_INTERFACES, &dropParmsList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objCosQueueMgmtWredDropProbScaleValue = dropParmsList.queue[keyCosQueueIndexValue].dropProb[listIndex];
  /* return the object value: CosQueueMgmtWredDropProbScale */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objCosQueueMgmtWredDropProbScaleValue,
                    sizeof (objCosQueueMgmtWredDropProbScaleValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtWredDropProbScale
*
* @purpose Set 'CosQueueMgmtWredDropProbScale'
*
* @description A scaling factor used for the WRED calculation to determine the 
*              packet drop probability for the current drop precedence level. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosQueueCfg_GlobalCosQueueMgmtWredDropProbScale (void
                                                                           *wap,
                                                                           void
                                                                           *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosQueueMgmtWredDropProbScaleValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIndexValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueDropPrecIndexValue;
  L7_qosCosDropParmsList_t dList;
  L7_uint32 listIndex;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CosQueueMgmtWredDropProbScale */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCosQueueMgmtWredDropProbScaleValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCosQueueMgmtWredDropProbScaleValue, owa.len);


  /* retrieve key: CosQueueIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                           (xLibU8_t *) & keyCosQueueIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIndexValue, kwa2.len);

  /* retrieve key: CosQueueDropPrecIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueDropPrecIndex,
                   (xLibU8_t *) & keyCosQueueDropPrecIndexValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueDropPrecIndexValue, kwa3.len);

  /* set the value in application */
  listIndex = keyCosQueueDropPrecIndexValue - L7_QOS_COS_DROP_PREC_LEVEL_MIN;
  owa.l7rc = usmDbQosCosQueueDropParmsListGet(L7_UNIT_CURRENT, L7_ALL_INTERFACES, &dList);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  dList.queue[keyCosQueueIndexValue].dropProb[listIndex] = objCosQueueMgmtWredDropProbScaleValue;
  owa.l7rc = usmDbQosCosQueueDropParmsListSet(L7_UNIT_CURRENT, L7_ALL_INTERFACES, &dList);
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
* @function fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtWredDropProbScale
*
* @purpose Set 'CosQueueMgmtWredDropProbScale'
*
* @description A scaling factor used for the WRED calculation to determine the 
*              packet drop probability for the current drop precedence level. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosQueueCfg_GlobalCosQueueDropPrecRestoreDefaults(void
                                                                           *wap,
                                                                           void
                                                                           *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  L7_RC_t rc =L7_FAILURE;

  FPOBJ_TRACE_ENTER (bufp);

  
  /* retrieve key: CosQueueIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                           (xLibU8_t *) & keyCosQueueIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIndexValue, kwa2.len);

  rc = usmDbQosCosQueueDropDefaultsRestore(L7_UNIT_CURRENT, L7_ALL_INTERFACES, keyCosQueueIndexValue);
  if (rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueMgmtWredDropProbScale
*
* @purpose Set 'CosQueueMgmtWredDropProbScale'
*
* @description A scaling factor used for the WRED calculation to determine the 
*              packet drop probability for the current drop precedence level. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosQueueCfg_CosQueueIntfDropPrecRestoreDefaults(void
                                                                           *wap,
                                                                           void
                                                                           *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIntfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosQueueIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  L7_RC_t rc = L7_FAILURE;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CosQueueIntfIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIntfIndex,
                   (xLibU8_t *) & keyCosQueueIntfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIntfIndexValue, kwa1.len);
  /* retrieve key: CosQueueIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosQueueCfg_CosQueueIndex,
                           (xLibU8_t *) & keyCosQueueIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosQueueIndexValue, kwa2.len);

  rc = usmDbQosCosQueueDropDefaultsRestore(L7_UNIT_CURRENT, keyCosQueueIntfIndexValue, keyCosQueueIndexValue);
  if (rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



