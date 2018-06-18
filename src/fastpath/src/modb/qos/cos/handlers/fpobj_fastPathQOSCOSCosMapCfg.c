/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_fastPathQOSCOSCosMapCfg.c
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
#include "_xe_fastPathQOSCOSCosMapCfg_obj.h"
#include "usmdb_qos_cos_api.h"
#include "cos_exports.h"
#include "usmdb_util_api.h"


/*******************************************************************************
* @function fpObjGet_fastPathQOSCOSCosMapCfg_GlobalCosMapIpDscpValue
*
* @purpose Get 'GlobalCosMapIpDscpValue'
 *@description  [GlobalCosMapIpDscpValue] The IP DSCP value contained in the
* received frame.    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosMapCfg_GlobalCosMapIpDscpValue (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objGlobalCosMapIpDscpValueValue;
  xLibU32_t nextObjGlobalCosMapIpDscpValueValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: GlobalCosMapIpDscpValue */
  owa.len = sizeof (objGlobalCosMapIpDscpValueValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosMapCfg_GlobalCosMapIpDscpValue,
                          (xLibU8_t *) & objGlobalCosMapIpDscpValueValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
	nextObjGlobalCosMapIpDscpValueValue = 0;
    owa.l7rc = usmDbQosCosMapIpDscpIndexGet (L7_UNIT_CURRENT, nextObjGlobalCosMapIpDscpValueValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objGlobalCosMapIpDscpValueValue, owa.len);
    owa.l7rc = usmDbQosCosMapIpDscpIndexGetNext (L7_UNIT_CURRENT, objGlobalCosMapIpDscpValueValue,
                                    &nextObjGlobalCosMapIpDscpValueValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjGlobalCosMapIpDscpValueValue, owa.len);

  /* return the object value: GlobalCosMapIpDscpValue */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjGlobalCosMapIpDscpValueValue,
                           sizeof (nextObjGlobalCosMapIpDscpValueValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSCOSCosMapCfg_GlobalCosMapIpDscpTrafficClass
*
* @purpose Get 'GlobalCosMapIpDscpTrafficClass'
 *@description  [GlobalCosMapIpDscpTrafficClass] Traffic class priority queue
* the received frame is mapped to.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosMapCfg_GlobalCosMapIpDscpTrafficClass (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objGlobalCosMapIpDscpTrafficClassValue;

  xLibU32_t keyCosMapIpDscpValueValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CosMapIpDscpValue */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosMapCfg_GlobalCosMapIpDscpValue,
                           (xLibU8_t *) & keyCosMapIpDscpValueValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosMapIpDscpValueValue, kwa2.len);


  /* get the value from application */
  owa.l7rc =
    usmDbQosCosMapIpDscpTrafficClassGet (L7_UNIT_CURRENT,
                                         L7_ALL_INTERFACES,
                                         keyCosMapIpDscpValueValue,
                                         &objGlobalCosMapIpDscpTrafficClassValue);
	
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objGlobalCosMapIpDscpTrafficClassValue,
                     sizeof (objGlobalCosMapIpDscpTrafficClassValue));

  /* return the object value: GlobalCosMapIpDscpTrafficClass */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGlobalCosMapIpDscpTrafficClassValue,
                           sizeof (objGlobalCosMapIpDscpTrafficClassValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_fastPathQOSCOSCosMapCfg_GlobalCosMapIpDscpTrafficClass
*
* @purpose List 'GlobalCosMapIpDscpTrafficClass'
 *@description  [GlobalCosMapIpDscpTrafficClass] Traffic class priority queue
* the received frame is mapped to.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_fastPathQOSCOSCosMapCfg_GlobalCosMapIpDscpTrafficClass (void *wap, void *bufp)
{
   return fpObjAppGetKey_U32Number(wap, bufp,XOBJ_fastPathQOSCOSCosMapCfg_GlobalCosMapIpDscpTrafficClass,
      L7_QOS_COS_QUEUE_ID_MIN, L7_QOS_COS_QUEUE_ID_MAX, 1);

}

/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosMapCfg_GlobalCosMapIpDscpTrafficClass
*
* @purpose Set 'GlobalCosMapIpDscpTrafficClass'
 *@description  [GlobalCosMapIpDscpTrafficClass] Traffic class priority queue
* the received frame is mapped to.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosMapCfg_GlobalCosMapIpDscpTrafficClass (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objGlobalCosMapIpDscpTrafficClassValue;

	  xLibU32_t keyCosMapIpDscpValueValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));


  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GlobalCosMapIpDscpTrafficClass */
  owa.len = sizeof (objGlobalCosMapIpDscpTrafficClassValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objGlobalCosMapIpDscpTrafficClassValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGlobalCosMapIpDscpTrafficClassValue, owa.len);

  /* retrieve key: CosMapIpDscpValue */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosMapCfg_GlobalCosMapIpDscpValue,
                           (xLibU8_t *) & keyCosMapIpDscpValueValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosMapIpDscpValueValue, kwa2.len);


  /* get the value from application */
  owa.l7rc =
    usmDbQosCosMapIpDscpTrafficClassSet (L7_UNIT_CURRENT,
                                         L7_ALL_INTERFACES,
                                         keyCosMapIpDscpValueValue,
                                         objGlobalCosMapIpDscpTrafficClassValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_QOS_COS_TRAFFICCLASS_DSCPVAL;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSCOSCosMapCfg_CosMapGlobalTrustMode
*
* @purpose Get 'CosMapGlobalTrustMode'
 *@description  [CosMapGlobalTrustMode] Specifies whether or not to trust a
* particular packet marking at ingress. Interface Trust Mode can only
* be one of the following 1)untrusted 2)trust dot1p 3)trust ip-dscp
* Default value is trust dot1p   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosMapCfg_CosMapGlobalTrustMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objCosMapGlobalTrustModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbQosCosMapTrustModeGet (L7_UNIT_CURRENT,
                                L7_ALL_INTERFACES,
                                &objCosMapGlobalTrustModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objCosMapGlobalTrustModeValue, sizeof (objCosMapGlobalTrustModeValue));

  /* return the object value: CosMapGlobalTrustMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCosMapGlobalTrustModeValue,
                           sizeof (objCosMapGlobalTrustModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosMapCfg_CosMapGlobalTrustMode
*
* @purpose Set 'CosMapGlobalTrustMode'
 *@description  [CosMapGlobalTrustMode] Specifies whether or not to trust a
* particular packet marking at ingress. Interface Trust Mode can only
* be one of the following 1)untrusted 2)trust dot1p 3)trust ip-dscp
* Default value is trust dot1p   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosMapCfg_CosMapGlobalTrustMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objCosMapGlobalTrustModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CosMapGlobalTrustMode */
  owa.len = sizeof (objCosMapGlobalTrustModeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objCosMapGlobalTrustModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCosMapGlobalTrustModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc =
    usmDbQosCosMapTrustModeSet (L7_UNIT_CURRENT,
                                L7_ALL_INTERFACES,
                                objCosMapGlobalTrustModeValue);
	
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSCOSCosMapCfg_GlobalCosMapUntrustedTrafficClass
*
* @purpose Get 'GlobalCosMapUntrustedTrafficClass'
 *@description  [GlobalCosMapUntrustedTrafficClass] The traffic class (i.e.
* hardware queue) to which all untrusted traffic is assigned.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosMapCfg_GlobalCosMapUntrustedTrafficClass (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objGlobalCosMapUntrustedTrafficClassValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbQosCosMapUntrustedPortDefaultTrafficClassGet (L7_UNIT_CURRENT,
                                                       L7_ALL_INTERFACES,
                                                       &objGlobalCosMapUntrustedTrafficClassValue);
	
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objGlobalCosMapUntrustedTrafficClassValue,
                     sizeof (objGlobalCosMapUntrustedTrafficClassValue));

  /* return the object value: GlobalCosMapUntrustedTrafficClass */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGlobalCosMapUntrustedTrafficClassValue,
                           sizeof (objGlobalCosMapUntrustedTrafficClassValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosMapCfg_CosMapGlobalTrustRestoreDefaults
*
* @purpose Set 'CosMapGlobalTrustRestoreDefaults'
 *@description  [CosMapGlobalTrustRestoreDefaults] Restore Trust Mode for
* specified interface to default   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosMapCfg_CosMapGlobalTrustRestoreDefaults (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objCosMapGlobalTrustRestoreDefaultsValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CosMapGlobalTrustRestoreDefaults */
  owa.len = sizeof (objCosMapGlobalTrustRestoreDefaultsValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objCosMapGlobalTrustRestoreDefaultsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCosMapGlobalTrustRestoreDefaultsValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
   owa.l7rc = usmDbQosCosMapTrustModeSet(L7_UNIT_CURRENT, L7_ALL_INTERFACES, FD_QOS_COS_MAP_INTF_TRUST_MODE);
	
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosMapCfg_CosMapGlobalIpDscpRestoreDefaults
*
* @purpose Set 'CosMapGlobalIpDscpRestoreDefaults'
 *@description  [CosMapGlobalIpDscpRestoreDefaults] Restore IP DSCP Queue
* Mapping to defaults for a specified interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosMapCfg_CosMapGlobalIpDscpRestoreDefaults (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objCosMapGlobalIpDscpRestoreDefaultsValue;

	L7_uint32 dscp=0;
    L7_uint32 list_cnt=0;
    L7_uint32 queueID;
    L7_RC_t rc;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CosMapGlobalIpDscpRestoreDefaults */
  owa.len = sizeof (objCosMapGlobalIpDscpRestoreDefaultsValue);
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCosMapGlobalIpDscpRestoreDefaultsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCosMapGlobalIpDscpRestoreDefaultsValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */

    rc = usmDbQosCosMapIpDscpIndexGet(L7_UNIT_CURRENT, dscp);
	  if(rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_QOS_COS_RESTORE_DSCP_FAILED;
	    FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

    while((rc == L7_SUCCESS) && (list_cnt < L7_QOS_COS_MAP_NUM_IPDSCP))
    {
      usmDbQosCosMapIpDscpDefaultTrafficClassGet(L7_UNIT_CURRENT, L7_ALL_INTERFACES, dscp, &queueID);
      rc = usmDbQosCosMapIpDscpTrafficClassSet(L7_UNIT_CURRENT, L7_ALL_INTERFACES, dscp, queueID);
      if (rc != L7_SUCCESS)
      {
         owa.l7rc = L7_FAILURE;
         break;
      }
      rc = usmDbQosCosMapIpDscpIndexGetNext(L7_UNIT_CURRENT, dscp, &dscp);
      list_cnt++;
   }

	
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_QOS_COS_RESTORE_DSCP_FAILED;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosMapCfg_CosMapglobalIpPrecRestoreDefaults
*
* @purpose Set 'CosMapglobalIpPrecRestoreDefaults'
 *@description  [CosMapglobalIpPrecRestoreDefaults] Restore IP Precedence Queue
* Mapping to defaults for a specified interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosMapCfg_CosMapglobalIpPrecRestoreDefaults (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objCosMapglobalIpPrecRestoreDefaultsValue;

	  L7_uint32 prec=0;
    L7_uint32 list_cnt=0;
    L7_uint32 queueID;
    L7_RC_t rc;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CosMapglobalIpPrecRestoreDefaults */
  owa.len = sizeof (objCosMapglobalIpPrecRestoreDefaultsValue);
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCosMapglobalIpPrecRestoreDefaultsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCosMapglobalIpPrecRestoreDefaultsValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */

	owa.l7rc = L7_SUCCESS;

    rc = usmDbQosCosMapIpPrecIndexGet(L7_UNIT_CURRENT, prec);

    if(rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;
	  FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
		
    while((rc == L7_SUCCESS) && (list_cnt < L7_QOS_COS_MAP_NUM_IPPREC))
    {
      usmDbQosCosMapIpPrecDefaultTrafficClassGet(L7_UNIT_CURRENT, L7_ALL_INTERFACES, prec, &queueID);
      rc = usmDbQosCosMapIpPrecTrafficClassSet(L7_UNIT_CURRENT, L7_ALL_INTERFACES, prec, queueID);
      if (rc != L7_SUCCESS)
      {
         owa.l7rc = L7_FAILURE;
         break;
      }
      rc = usmDbQosCosMapIpPrecIndexGetNext(L7_UNIT_CURRENT, prec, &prec);
      list_cnt++;

   }
		
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}




/*******************************************************************************
* @function fpObjGet_fastPathQOSCOSCosMapCfg_CosMapIpPrecIntfIndex
*
* @purpose Get 'CosMapIpPrecIntfIndex'
*
* @description Specifies all CoS configurable interfaces. 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosMapCfg_CosMapIpPrecIntfIndex (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosMapIpPrecIntfIndexValue;
  xLibU32_t nextObjCosMapIpPrecIntfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CosMapIpPrecIntfIndex */
  owa.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosMapCfg_CosMapIpPrecIntfIndex,
                   (xLibU8_t *) & objCosMapIpPrecIntfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbValidIntIfNumFirstGet(&nextObjCosMapIpPrecIntfIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objCosMapIpPrecIntfIndexValue, owa.len);
    owa.l7rc =
      usmDbQosCosMapIpPrecIntfIndexGetNext (L7_UNIT_CURRENT,
                                            objCosMapIpPrecIntfIndexValue,
                                            &nextObjCosMapIpPrecIntfIndexValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjCosMapIpPrecIntfIndexValue, owa.len);

  /* return the object value: CosMapIpPrecIntfIndex */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & nextObjCosMapIpPrecIntfIndexValue,
                    sizeof (objCosMapIpPrecIntfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSCOSCosMapCfg_CosMapIpPrecValue
*
* @purpose Get 'CosMapIpPrecValue'
*
* @description IP Precedence value 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosMapCfg_CosMapIpPrecValue (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosMapIpPrecValueValue;
  xLibU32_t nextObjCosMapIpPrecValueValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CosMapIpPrecValue */
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosMapCfg_CosMapIpPrecValue,
                          (xLibU8_t *) & objCosMapIpPrecValueValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjCosMapIpPrecValueValue=0;
    owa.l7rc = usmDbQosCosMapIpPrecIndexGet(L7_UNIT_CURRENT,nextObjCosMapIpPrecValueValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objCosMapIpPrecValueValue, owa.len);
    owa.l7rc =
      usmDbQosCosMapIpPrecIndexGetNext (L7_UNIT_CURRENT,
                                        objCosMapIpPrecValueValue,
                                        &nextObjCosMapIpPrecValueValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjCosMapIpPrecValueValue, owa.len);

  /* return the object value: CosMapIpPrecValue */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjCosMapIpPrecValueValue,
                           sizeof (objCosMapIpPrecValueValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSCOSCosMapCfg_CosMapIpPrecTrafficClass
*
* @purpose Get 'CosMapIpPrecTrafficClass'
*
* @description traffic class (queue) for this IP precedence 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosMapCfg_CosMapIpPrecTrafficClass (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosMapIpPrecIntfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosMapIpPrecValueValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosMapIpPrecTrafficClassValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CosMapIpPrecIntfIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosMapCfg_CosMapIpPrecIntfIndex,
                   (xLibU8_t *) & keyCosMapIpPrecIntfIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosMapIpPrecIntfIndexValue, kwa1.len);

  /* retrieve key: CosMapIpPrecValue */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosMapCfg_CosMapIpPrecValue,
                           (xLibU8_t *) & keyCosMapIpPrecValueValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosMapIpPrecValueValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbQosCosMapIpPrecTrafficClassGet (L7_UNIT_CURRENT,
                                         keyCosMapIpPrecIntfIndexValue,
                                         keyCosMapIpPrecValueValue,
                                         &objCosMapIpPrecTrafficClassValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: CosMapIpPrecTrafficClass */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objCosMapIpPrecTrafficClassValue,
                    sizeof (objCosMapIpPrecTrafficClassValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosMapCfg_CosMapIpPrecTrafficClass
*
* @purpose Set 'CosMapIpPrecTrafficClass'
*
* @description traffic class (queue) for this IP precedence 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosMapCfg_CosMapIpPrecTrafficClass (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosMapIpPrecTrafficClassValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosMapIpPrecIntfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosMapIpPrecValueValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CosMapIpPrecTrafficClass */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCosMapIpPrecTrafficClassValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCosMapIpPrecTrafficClassValue, owa.len);

  /* retrieve key: CosMapIpPrecIntfIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosMapCfg_CosMapIpPrecIntfIndex,
                   (xLibU8_t *) & keyCosMapIpPrecIntfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosMapIpPrecIntfIndexValue, kwa1.len);

  /* retrieve key: CosMapIpPrecValue */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosMapCfg_CosMapIpPrecValue,
                           (xLibU8_t *) & keyCosMapIpPrecValueValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosMapIpPrecValueValue, kwa2.len);

  /* set the value in application */
  owa.l7rc =
    usmDbQosCosMapIpPrecTrafficClassSet (L7_UNIT_CURRENT,
                                         keyCosMapIpPrecIntfIndexValue,
                                         keyCosMapIpPrecValueValue,
                                         objCosMapIpPrecTrafficClassValue);
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
* @function fpObjGet_fastPathQOSCOSCosMapCfg_CosMapIpDscpIntfIndex
*
* @purpose Get 'CosMapIpDscpIntfIndex'
*
* @description Specify which queue value to map the corresponding IP DSCP value. 
*              
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosMapCfg_CosMapIpDscpIntfIndex (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosMapIpDscpIntfIndexValue;
  xLibU32_t nextObjCosMapIpDscpIntfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CosMapIpDscpIntfIndex */
  owa.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosMapCfg_CosMapIpDscpIntfIndex,
                   (xLibU8_t *) & objCosMapIpDscpIntfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbValidIntIfNumFirstGet(&nextObjCosMapIpDscpIntfIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objCosMapIpDscpIntfIndexValue, owa.len);
    owa.l7rc =
      usmDbQosCosMapIpDscpIntfIndexGetNext (L7_UNIT_CURRENT,
                                            objCosMapIpDscpIntfIndexValue,
                                            &nextObjCosMapIpDscpIntfIndexValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjCosMapIpDscpIntfIndexValue, owa.len);

  /* return the object value: CosMapIpDscpIntfIndex */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & nextObjCosMapIpDscpIntfIndexValue,
                    sizeof (objCosMapIpDscpIntfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSCOSCosMapCfg_CosMapIpDscpValue
*
* @purpose Get 'CosMapIpDscpValue'
*
* @description The IP DSCP value contained in the received frame. 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosMapCfg_CosMapIpDscpValue (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosMapIpDscpValueValue;
  xLibU32_t nextObjCosMapIpDscpValueValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CosMapIpDscpValue */
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosMapCfg_CosMapIpDscpValue,
                          (xLibU8_t *) & objCosMapIpDscpValueValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjCosMapIpDscpValueValue=0;
    owa.l7rc = usmDbQosCosMapIpDscpIndexGet(L7_UNIT_CURRENT,nextObjCosMapIpDscpValueValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objCosMapIpDscpValueValue, owa.len);
    owa.l7rc = usmDbQosCosMapIpDscpIndexGetNext(L7_UNIT_CURRENT,objCosMapIpDscpValueValue,
                                                &nextObjCosMapIpDscpValueValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjCosMapIpDscpValueValue, owa.len);

  /* return the object value: CosMapIpDscpValue */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjCosMapIpDscpValueValue,
                           sizeof (objCosMapIpDscpValueValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSCOSCosMapCfg_CosMapIpDscpTrafficClass
*
* @purpose Get 'CosMapIpDscpTrafficClass'
*
* @description Traffic class priority queue the received frame is mapped to. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosMapCfg_CosMapIpDscpTrafficClass (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosMapIpDscpIntfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosMapIpDscpValueValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosMapIpDscpTrafficClassValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CosMapIpDscpIntfIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosMapCfg_CosMapIpDscpIntfIndex,
                   (xLibU8_t *) & keyCosMapIpDscpIntfIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosMapIpDscpIntfIndexValue, kwa1.len);

  /* retrieve key: CosMapIpDscpValue */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosMapCfg_CosMapIpDscpValue,
                           (xLibU8_t *) & keyCosMapIpDscpValueValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosMapIpDscpValueValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbQosCosMapIpDscpTrafficClassGet (L7_UNIT_CURRENT,
                                         keyCosMapIpDscpIntfIndexValue,
                                         keyCosMapIpDscpValueValue,
                                         &objCosMapIpDscpTrafficClassValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: CosMapIpDscpTrafficClass */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objCosMapIpDscpTrafficClassValue,
                    sizeof (objCosMapIpDscpTrafficClassValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosMapCfg_CosMapIpDscpTrafficClass
*
* @purpose Set 'CosMapIpDscpTrafficClass'
*
* @description Traffic class priority queue the received frame is mapped to. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosMapCfg_CosMapIpDscpTrafficClass (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosMapIpDscpTrafficClassValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosMapIpDscpIntfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosMapIpDscpValueValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CosMapIpDscpTrafficClass */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCosMapIpDscpTrafficClassValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCosMapIpDscpTrafficClassValue, owa.len);

  /* retrieve key: CosMapIpDscpIntfIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosMapCfg_CosMapIpDscpIntfIndex,
                   (xLibU8_t *) & keyCosMapIpDscpIntfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosMapIpDscpIntfIndexValue, kwa1.len);

  /* retrieve key: CosMapIpDscpValue */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosMapCfg_CosMapIpDscpValue,
                           (xLibU8_t *) & keyCosMapIpDscpValueValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosMapIpDscpValueValue, kwa2.len);

  /* set the value in application */
  owa.l7rc =
    usmDbQosCosMapIpDscpTrafficClassSet (L7_UNIT_CURRENT,
                                         keyCosMapIpDscpIntfIndexValue,
                                         keyCosMapIpDscpValueValue,
                                         objCosMapIpDscpTrafficClassValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_QOS_COS_TRAFFICCLASS_DSCPVAL;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSCOSCosMapCfg_CosMapIntfTrustIntfIndex
*
* @purpose Get 'CosMapIntfTrustIntfIndex'
*
* @description A non-zero value indicates the ifIndex for the corresponding interface 
*              entry in the ifTable. A value of zero represents global 
*              configuration, which in turn causes all interface entries 
*              to be updated for a set operation, or reflects the most recent 
*              globa 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosMapCfg_CosMapIntfTrustIntfIndex (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosMapIntfTrustIntfIndexValue;
  xLibU32_t nextObjCosMapIntfTrustIntfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CosMapIntfTrustIntfIndex */
  owa.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosMapCfg_CosMapIntfTrustIntfIndex,
                   (xLibU8_t *) & objCosMapIntfTrustIntfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbValidIntIfNumFirstGet(&nextObjCosMapIntfTrustIntfIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objCosMapIntfTrustIntfIndexValue, owa.len);
    owa.l7rc =
      usmDbQosCosMapTrustModeIntfIndexGetNext (L7_UNIT_CURRENT,
                                               objCosMapIntfTrustIntfIndexValue,
                                               &nextObjCosMapIntfTrustIntfIndexValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjCosMapIntfTrustIntfIndexValue, owa.len);

  /* return the object value: CosMapIntfTrustIntfIndex */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & nextObjCosMapIntfTrustIntfIndexValue,
                    sizeof (objCosMapIntfTrustIntfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_fastPathQOSCOSCosMapCfg_CosMapIntfTrustMode
*
* @purpose Get 'CosMapIntfTrustMode'
*
* @description Specifies whether or not to trust a particular packet marking 
*              at ingress. Interface Trust Mode can only be one of the following 
*              1)untrusted 2)trust dot1p 3)trust ip-dscp Default value 
*              is trust dot1p 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosMapCfg_CosMapIntfTrustMode (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosMapIntfTrustIntfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosMapIntfTrustModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CosMapIntfTrustIntfIndex */
  kwa.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosMapCfg_CosMapIntfTrustIntfIndex,
                   (xLibU8_t *) & keyCosMapIntfTrustIntfIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosMapIntfTrustIntfIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbQosCosMapTrustModeGet (L7_UNIT_CURRENT,
                                keyCosMapIntfTrustIntfIndexValue,
                                &objCosMapIntfTrustModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: CosMapIntfTrustMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCosMapIntfTrustModeValue,
                           sizeof (objCosMapIntfTrustModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosMapCfg_CosMapIntfTrustMode
*
* @purpose Set 'CosMapIntfTrustMode'
*
* @description Specifies whether or not to trust a particular packet marking 
*              at ingress. Interface Trust Mode can only be one of the following 
*              1)untrusted 2)trust dot1p 3)trust ip-dscp Default value 
*              is trust dot1p 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosMapCfg_CosMapIntfTrustMode (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosMapIntfTrustModeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosMapIntfTrustIntfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CosMapIntfTrustMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCosMapIntfTrustModeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCosMapIntfTrustModeValue, owa.len);

  /* retrieve key: CosMapIntfTrustIntfIndex */
  kwa.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosMapCfg_CosMapIntfTrustIntfIndex,
                   (xLibU8_t *) & keyCosMapIntfTrustIntfIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosMapIntfTrustIntfIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbQosCosMapTrustModeSet (L7_UNIT_CURRENT,
                                keyCosMapIntfTrustIntfIndexValue,
                                objCosMapIntfTrustModeValue);
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
* @function fpObjGet_fastPathQOSCOSCosMapCfg_CosMapUntrustedTrafficClass
*
* @purpose Get 'CosMapUntrustedTrafficClass'
*
* @description The traffic class (i.e. hardware queue) to which all untrusted 
*              traffic is assigned. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosMapCfg_CosMapUntrustedTrafficClass (void
                                                                       *wap,
                                                                       void
                                                                       *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosMapIntfTrustIntfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosMapUntrustedTrafficClassValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CosMapIntfTrustIntfIndex */
  kwa.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosMapCfg_CosMapIntfTrustIntfIndex,
                   (xLibU8_t *) & keyCosMapIntfTrustIntfIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosMapIntfTrustIntfIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbQosCosMapUntrustedPortDefaultTrafficClassGet (L7_UNIT_CURRENT,
                                                       keyCosMapIntfTrustIntfIndexValue,
                                                       &objCosMapUntrustedTrafficClassValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: CosMapUntrustedTrafficClass */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objCosMapUntrustedTrafficClassValue,
                    sizeof (objCosMapUntrustedTrafficClassValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjList_fastPathQOSCOSCosMapCfg_CosMapIpPrecTrafficClass
*
* @purpose Get 'CosMapIpPrecTrafficClass'
*
* @description [CosMapIpPrecTrafficClass]: traffic class (queue) for this
*              IP precedence
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_fastPathQOSCOSCosMapCfg_CosMapIpPrecTrafficClass (void *wap,
                                                                     void *bufp)
{
  return fpObjAppGetKey_U32Number(wap, bufp,XOBJ_fastPathQOSCOSCosMapCfg_CosMapIpPrecTrafficClass,
      L7_QOS_COS_QUEUE_ID_MIN, L7_QOS_COS_QUEUE_ID_MAX, 1);
}
/*******************************************************************************
* @function fpObjList_fastPathQOSCOSCosMapCfg_CosMapIpDscpTrafficClass
*
* @purpose Get 'CosMapIpDscpTrafficClass'
*
* @description [CosMapIpDscpTrafficClass]: Traffic class priority queue the
*              received frame is mapped to.
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_fastPathQOSCOSCosMapCfg_CosMapIpDscpTrafficClass (void *wap,
                                                                     void *bufp)
{
   return fpObjAppGetKey_U32Number(wap, bufp,XOBJ_fastPathQOSCOSCosMapCfg_CosMapIpDscpTrafficClass,
      L7_QOS_COS_QUEUE_ID_MIN, L7_QOS_COS_QUEUE_ID_MAX, 1);
}

/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosMapCfg_CosMapIntfTrustRestoreDefaults
*
* @purpose Set 'CosMapIntfTrustRestoreDefaults'
*
* @description [CosMapIntfTrustRestoreDefaults] Restore Trust Mode for specified interface to default
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosMapCfg_CosMapIntfTrustRestoreDefaults (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosMapIntfTrustRestoreDefaultsValue;

  fpObjWa_t kwaCosMapIntfTrustIntfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosMapIntfTrustIntfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CosMapIntfTrustRestoreDefaults */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objCosMapIntfTrustRestoreDefaultsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCosMapIntfTrustRestoreDefaultsValue, owa.len);

  /* retrieve key: CosMapIntfTrustIntfIndex */
  kwaCosMapIntfTrustIntfIndex.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosMapCfg_CosMapIntfTrustIntfIndex,
                   (xLibU8_t *) & keyCosMapIntfTrustIntfIndexValue,
                   &kwaCosMapIntfTrustIntfIndex.len);
  if (kwaCosMapIntfTrustIntfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaCosMapIntfTrustIntfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaCosMapIntfTrustIntfIndex);
    return kwaCosMapIntfTrustIntfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosMapIntfTrustIntfIndexValue,
                           kwaCosMapIntfTrustIntfIndex.len);

  /* set the value in application */
  if(objCosMapIntfTrustRestoreDefaultsValue)
  {
   owa.l7rc = usmDbQosCosMapTrustModeSet(L7_UNIT_CURRENT, keyCosMapIntfTrustIntfIndexValue, FD_QOS_COS_MAP_INTF_TRUST_MODE);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosMapCfg_CosMapIntfIpDscpRestoreDefaults
*
* @purpose Set 'CosMapIntfIpDscpRestoreDefaults'
*
* @description [CosMapIntfIpDscpRestoreDefaults] Restore IP DSCP Queue Mapping to defaults for a specified interface
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosMapCfg_CosMapIntfIpDscpRestoreDefaults (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosMapIntfIpDscpRestoreDefaultsValue;

  fpObjWa_t kwaCosMapIpDscpIntfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosMapIpDscpIntfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CosMapIntfIpDscpRestoreDefaults */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objCosMapIntfIpDscpRestoreDefaultsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCosMapIntfIpDscpRestoreDefaultsValue, owa.len);

  /* retrieve key: CosMapIpDscpIntfIndex */
  kwaCosMapIpDscpIntfIndex.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosMapCfg_CosMapIpDscpIntfIndex,
                   (xLibU8_t *) & keyCosMapIpDscpIntfIndexValue, &kwaCosMapIpDscpIntfIndex.len);
  if (kwaCosMapIpDscpIntfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaCosMapIpDscpIntfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaCosMapIpDscpIntfIndex);
    return kwaCosMapIpDscpIntfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosMapIpDscpIntfIndexValue, kwaCosMapIpDscpIntfIndex.len);

  /* set the value in application */
  if(objCosMapIntfIpDscpRestoreDefaultsValue)
  {
    L7_uint32 dscp=0;
    L7_uint32 list_cnt=0;
    L7_uint32 queueID;
    L7_RC_t rc;
    rc = usmDbQosCosMapIpDscpIndexGet(L7_UNIT_CURRENT, dscp);

    while((rc == L7_SUCCESS) && (list_cnt < L7_QOS_COS_MAP_NUM_IPDSCP))
    {
      usmDbQosCosMapIpDscpDefaultTrafficClassGet(L7_UNIT_CURRENT, keyCosMapIpDscpIntfIndexValue, dscp, &queueID);
      rc = usmDbQosCosMapIpDscpTrafficClassSet(L7_UNIT_CURRENT, keyCosMapIpDscpIntfIndexValue, dscp, queueID);
      if (rc != L7_SUCCESS)
      {
         owa.rc = XLIBRC_FAILURE;
         break;
      }
      rc = usmDbQosCosMapIpDscpIndexGetNext(L7_UNIT_CURRENT, dscp, &dscp);
      list_cnt++;
   }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_QOS_COS_RESTORE_DSCP_FAILED;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosMapCfg_CosMapIntfIpPrecRestoreDefaults
*
* @purpose Set 'CosMapIntfIpPrecRestoreDefaults'
*
* @description [CosMapIntfIpPrecRestoreDefaults] Restore IP Precedence Queue Mapping to defaults for a specified interface
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosMapCfg_CosMapIntfIpPrecRestoreDefaults (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosMapIntfIpPrecRestoreDefaultsValue;

  fpObjWa_t kwaCosMapIpPrecIntfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyCosMapIpPrecIntfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CosMapIntfIpPrecRestoreDefaults */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objCosMapIntfIpPrecRestoreDefaultsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCosMapIntfIpPrecRestoreDefaultsValue, owa.len);

  /* retrieve key: CosMapIpPrecIntfIndex */
  kwaCosMapIpPrecIntfIndex.rc =
    xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosMapCfg_CosMapIpPrecIntfIndex,
                   (xLibU8_t *) & keyCosMapIpPrecIntfIndexValue, &kwaCosMapIpPrecIntfIndex.len);
  if (kwaCosMapIpPrecIntfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaCosMapIpPrecIntfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaCosMapIpPrecIntfIndex);
    return kwaCosMapIpPrecIntfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosMapIpPrecIntfIndexValue, kwaCosMapIpPrecIntfIndex.len);

  /* set the value in application */
  if(objCosMapIntfIpPrecRestoreDefaultsValue)
  {
    L7_uint32 prec=0;
    L7_uint32 list_cnt=0;
    L7_uint32 queueID;
    L7_RC_t rc;
    rc = usmDbQosCosMapIpPrecIndexGet(L7_UNIT_CURRENT, prec);

    while((rc == L7_SUCCESS) && (list_cnt < L7_QOS_COS_MAP_NUM_IPPREC))
    {
      usmDbQosCosMapIpPrecDefaultTrafficClassGet(L7_UNIT_CURRENT, keyCosMapIpPrecIntfIndexValue, prec, &queueID);
      rc = usmDbQosCosMapIpPrecTrafficClassSet(L7_UNIT_CURRENT, keyCosMapIpPrecIntfIndexValue, prec, queueID);
      if (rc != L7_SUCCESS)
      {
         owa.rc = XLIBRC_FAILURE;
         break;
      }
      rc = usmDbQosCosMapIpPrecIndexGetNext(L7_UNIT_CURRENT, prec, &prec);
      list_cnt++;
   }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* fpObjGet_fastPathQOSCOSCosMapCfg_GlobalCosMapIpPrecValue
*
* @purpose Get 'GlobalCosMapIpPrecValue'
 *@description  [GlobalCosMapIpPrecValue] The IP DSCP value contained in the
* received frame.    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosMapCfg_GlobalCosMapIpPrecValue (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objGlobalCosMapIpPrecValueValue;
  xLibU32_t nextObjGlobalCosMapIpPrecValueValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: GlobalCosMapIpPrecValue */
  owa.len = sizeof (objGlobalCosMapIpPrecValueValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosMapCfg_GlobalCosMapIpPrecValue,
                          (xLibU8_t *) & objGlobalCosMapIpPrecValueValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
  nextObjGlobalCosMapIpPrecValueValue = 0;
    owa.l7rc = usmDbQosCosMapIpPrecIndexGet (L7_UNIT_CURRENT, nextObjGlobalCosMapIpPrecValueValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objGlobalCosMapIpPrecValueValue, owa.len);
    owa.l7rc = usmDbQosCosMapIpPrecIndexGetNext (L7_UNIT_CURRENT, objGlobalCosMapIpPrecValueValue,
                                    &nextObjGlobalCosMapIpPrecValueValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjGlobalCosMapIpPrecValueValue, owa.len);

  /* return the object value: GlobalCosMapIpPrecValue */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjGlobalCosMapIpPrecValueValue,
                           sizeof (nextObjGlobalCosMapIpPrecValueValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSCOSCosMapCfg_GlobalCosMapIpPrecTrafficClass
*
* @purpose Get 'GlobalCosMapIpPrecTrafficClass'
 *@description  [GlobalCosMapIpPrecTrafficClass] Traffic class priority queue
* the received frame is mapped to.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSCOSCosMapCfg_GlobalCosMapIpPrecTrafficClass (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objGlobalCosMapIpPrecTrafficClassValue;

  xLibU32_t keyCosMapIpPrecValueValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: CosMapIpPrecValue */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosMapCfg_GlobalCosMapIpPrecValue,
                           (xLibU8_t *) & keyCosMapIpPrecValueValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosMapIpPrecValueValue, kwa2.len);


  /* get the value from application */
  owa.l7rc =
    usmDbQosCosMapIpPrecTrafficClassGet (L7_UNIT_CURRENT,
                                         L7_ALL_INTERFACES,
                                         keyCosMapIpPrecValueValue,
                                         &objGlobalCosMapIpPrecTrafficClassValue);
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objGlobalCosMapIpPrecTrafficClassValue,
                     sizeof (objGlobalCosMapIpPrecTrafficClassValue));

  /* return the object value: GlobalCosMapIpPrecTrafficClass */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGlobalCosMapIpPrecTrafficClassValue,
                           sizeof (objGlobalCosMapIpPrecTrafficClassValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_fastPathQOSCOSCosMapCfg_GlobalCosMapIpPrecTrafficClass
*
* @purpose List 'GlobalCosMapIpPrecTrafficClass'
 *@description  [GlobalCosMapIpPrecTrafficClass] Traffic class priority queue
* the received frame is mapped to.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_fastPathQOSCOSCosMapCfg_GlobalCosMapIpPrecTrafficClass (void *wap, void *bufp)
{
   return fpObjAppGetKey_U32Number(wap, bufp,XOBJ_fastPathQOSCOSCosMapCfg_GlobalCosMapIpPrecTrafficClass,
      L7_QOS_COS_QUEUE_ID_MIN, L7_QOS_COS_QUEUE_ID_MAX, 1);

}

/*******************************************************************************
* @function fpObjSet_fastPathQOSCOSCosMapCfg_GlobalCosMapIpPrecTrafficClass
*
* @purpose Set 'GlobalCosMapIpPrecTrafficClass'
 *@description  [GlobalCosMapIpPrecTrafficClass] Traffic class priority queue
* the received frame is mapped to.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSCOSCosMapCfg_GlobalCosMapIpPrecTrafficClass (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objGlobalCosMapIpPrecTrafficClassValue;

   xLibU32_t keyCosMapIpPrecValueValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));


  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GlobalCosMapIpPrecTrafficClass */
  owa.len = sizeof (objGlobalCosMapIpPrecTrafficClassValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objGlobalCosMapIpPrecTrafficClassValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGlobalCosMapIpPrecTrafficClassValue, owa.len);

  /* retrieve key: CosMapIpPrecValue */
  kwa2.rc = xLibFilterGet (wap, XOBJ_fastPathQOSCOSCosMapCfg_GlobalCosMapIpPrecValue,
                           (xLibU8_t *) & keyCosMapIpPrecValueValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyCosMapIpPrecValueValue, kwa2.len);


  /* get the value from application */
  owa.l7rc =
    usmDbQosCosMapIpPrecTrafficClassSet (L7_UNIT_CURRENT,
                                         L7_ALL_INTERFACES,
                                         keyCosMapIpPrecValueValue,
                                         objGlobalCosMapIpPrecTrafficClassValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_QOS_COS_TRAFFICCLASS_DSCPVAL;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


