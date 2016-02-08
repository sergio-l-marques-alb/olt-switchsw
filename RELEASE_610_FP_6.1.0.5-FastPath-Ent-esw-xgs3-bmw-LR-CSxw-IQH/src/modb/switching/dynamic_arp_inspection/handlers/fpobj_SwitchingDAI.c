/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_SwitchingDAI.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to DynamicArpInspection-object.xml
*
* @create  3 March 2008
*
* @author  Radha K
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_SwitchingDAI_obj.h"
#include "usmdb_dai_api.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingDAI_SRCMACVerify
*
* @purpose Get 'SRCMACVerify'
*
* @description [SRCMACVerify]: Dynamic ARP Inspection to verify Source MAC
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDAI_SRCMACVerify (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSRCMACVerifyValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbDaiVerifySMacGet ( &objSRCMACVerifyValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSRCMACVerifyValue,
                     sizeof (objSRCMACVerifyValue));

  /* return the object value: SRCMACVerify */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSRCMACVerifyValue,
                           sizeof (objSRCMACVerifyValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingDAI_SRCMACVerify
*
* @purpose Set 'SRCMACVerify'
*
* @description [SRCMACVerify]: Dynamic ARP Inspection to verify Source MAC
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDAI_SRCMACVerify (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSRCMACVerifyValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SRCMACVerify */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSRCMACVerifyValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSRCMACVerifyValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbDaiVerifySMacSet ( objSRCMACVerifyValue);
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
* @function fpObjGet_SwitchingDAI_DSTMACVerify
*
* @purpose Get 'DSTMACVerify'
*
* @description [DSTMACVerify]: Dynamic ARP Inspection to verify Destination
*              MAC 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDAI_DSTMACVerify (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDSTMACVerifyValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbDaiVerifyDMacGet ( &objDSTMACVerifyValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDSTMACVerifyValue,
                     sizeof (objDSTMACVerifyValue));

  /* return the object value: DSTMACVerify */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDSTMACVerifyValue,
                           sizeof (objDSTMACVerifyValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingDAI_DSTMACVerify
*
* @purpose Set 'DSTMACVerify'
*
* @description [DSTMACVerify]: Dynamic ARP Inspection to verify Destination
*              MAC 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDAI_DSTMACVerify (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDSTMACVerifyValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DSTMACVerify */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDSTMACVerifyValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDSTMACVerifyValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbDaiVerifyDMacSet ( objDSTMACVerifyValue);
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
* @function fpObjGet_SwitchingDAI_IPVerify
*
* @purpose Get 'IPVerify'
*
* @description [IPVerify]: Dynamic ARP Inspection to verify IP 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDAI_IPVerify (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIPVerifyValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbDaiVerifyIPGet ( &objIPVerifyValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIPVerifyValue, sizeof (objIPVerifyValue));

  /* return the object value: IPVerify */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIPVerifyValue,
                           sizeof (objIPVerifyValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingDAI_IPVerify
*
* @purpose Set 'IPVerify'
*
* @description [IPVerify]: Dynamic ARP Inspection to verify IP 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDAI_IPVerify (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIPVerifyValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: IPVerify */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objIPVerifyValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIPVerifyValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbDaiVerifyIPSet ( objIPVerifyValue);
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
* @function fpObjGet_SwitchingDAI_Interface
*
* @purpose Get 'Interface'
*
* @description [Interface]: Specifies all configurable interfaces for Dynamic
*              ARP Inspection 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDAI_Interface (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objInterfaceValue=0;
  xLibU32_t nextObjInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDAI_Interface,
                          (xLibU8_t *) & objInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbIntIfNumTypeFirstGet (L7_UNIT_CURRENT,USM_PHYSICAL_INTF | USM_LAG_INTF,0,
                                       &nextObjInterfaceValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);
    owa.l7rc = usmDbIntIfNumTypeNextGet (L7_UNIT_CURRENT,USM_PHYSICAL_INTF | USM_LAG_INTF,0,objInterfaceValue,
                                       &nextObjInterfaceValue);
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjInterfaceValue, owa.len);

  /* return the object value: Interface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjInterfaceValue,
                           sizeof (objInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

#if 0
/*******************************************************************************
* @function fpObjGet_SwitchingDAI_IsValidDAIInterface
*
* @purpose Get 'IsValidDAIInterface'
*
* @description [IsValidDAIInterface]: Specifies if the given interface is
*              a Valid DAI Interface 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDAI_IsValidDAIInterface (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsValidDAIInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDAI_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDaiIsValidIntf (keyInterfaceValue);
  if (owa.l7rc != L7_TRUE)
  {
    objIsValidDAIInterfaceValue = L7_FALSE;
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  objIsValidDAIInterfaceValue = L7_TRUE;
  /* return the object value: IsValidDAIInterface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsValidDAIInterfaceValue,
                           sizeof (objIsValidDAIInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
#endif

/*******************************************************************************
* @function fpObjGet_SwitchingDAI_IntfTrust
*
* @purpose Get 'IntfTrust'
*
* @description [IntfTrust]: Trust setting for DAI on the interface 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDAI_IntfTrust (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIntfTrustValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDAI_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDaiIntfTrustGet (keyInterfaceValue,
                                   &objIntfTrustValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IntfTrust */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIntfTrustValue,
                           sizeof (objIntfTrustValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingDAI_IntfTrust
*
* @purpose Set 'IntfTrust'
*
* @description [IntfTrust]: Trust setting for DAI on the interface 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDAI_IntfTrust (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIntfTrustValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: IntfTrust */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objIntfTrustValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIntfTrustValue, owa.len);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDAI_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbDaiIntfTrustSet (keyInterfaceValue,
                                   objIntfTrustValue);
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
* @function fpObjGet_SwitchingDAI_DAIEnable
*
* @purpose Get 'DAIEnable'
*
* @description [DAIEnable]: Enable DAI on this Interface 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDAI_DAIEnable (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDAIEnableValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDAI_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDaiPortEnabledGet (keyInterfaceValue,
                                     &objDAIEnableValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DAIEnable */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDAIEnableValue,
                           sizeof (objDAIEnableValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingDAI_IntfRateLimit
*
* @purpose Get 'IntfRateLimit'
*
* @description [IntfRateLimit]: The rate limit value of incoming ARP packets
*              on untrusted interfaces 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDAI_IntfRateLimit (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIntfRateLimitValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDAI_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDaiIntfRateLimitGet (keyInterfaceValue,
                                       &objIntfRateLimitValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IntfRateLimit */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIntfRateLimitValue,
                           sizeof (objIntfRateLimitValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingDAI_IntfRateLimit
*
* @purpose Set 'IntfRateLimit'
*
* @description [IntfRateLimit]: The rate limit value of incoming ARP packets
*              on untrusted interfaces 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDAI_IntfRateLimit (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIntfRateLimitValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: IntfRateLimit */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objIntfRateLimitValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIntfRateLimitValue, owa.len);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDAI_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbDaiIntfRateLimitSet (keyInterfaceValue,
                                       objIntfRateLimitValue);
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
* @function fpObjGet_SwitchingDAI_NoLimit
*
* @purpose Get 'NoLimit'
*
* @description [NoLimit]: The rate limit value of incoming ARP packets
*              on untrusted interfaces
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDAI_NoLimit (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibS32_t));
  xLibS32_t objNoLimitValue;
  FPOBJ_TRACE_ENTER (bufp);

/* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDAI_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

 /* get the value from application */
  owa.l7rc = usmDbDaiIntfRateLimitGet (keyInterfaceValue,
                                       &objNoLimitValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

/* return the object value: IntfRateLimit */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNoLimitValue,
                           sizeof (objNoLimitValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingDAI_NoLimit
*
* @purpose Set 'NoLimit'
*
* @description [NoLimit]: The rate limit value of incoming ARP packets
*              on untrusted interfaces
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDAI_NoLimit (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibS32_t));
  xLibS32_t objNoLimitValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

   /* retrieve object: IntfRateLimit */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objNoLimitValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNoLimitValue, owa.len);

   /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDAI_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbDaiIntfRateLimitSet (keyInterfaceValue,
                                      objNoLimitValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

#ifdef RADHA
/*******************************************************************************
* @function fpObjGet_SwitchingDAI_IntfRateLimitEnable
*
* @purpose Get 'IntfRateLimitEnable'
*
* @description [IntfRateLimitEnable]: The enable/disable rate limit of incoming
*              ARP packets on untrusted interfaces 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDAI_IntfRateLimitEnable (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIntfRateLimitEnableValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDAI_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDaiIntfRateLimitGet (keyInterfaceValue,
                                       &objIntfRateLimitEnableValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IntfRateLimitEnable */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIntfRateLimitEnableValue,
                           sizeof (objIntfRateLimitEnableValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingDAI_IntfRateLimitEnable
*
* @purpose Set 'IntfRateLimitEnable'
*
* @description [IntfRateLimitEnable]: The enable/disable rate limit of incoming
*              ARP packets on untrusted interfaces 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDAI_IntfRateLimitEnable (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIntfRateLimitEnableValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: IntfRateLimitEnable */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objIntfRateLimitEnableValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIntfRateLimitEnableValue, owa.len);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDAI_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbDaiIntfRateLimitSet(keyInterfaceValue,
                                      objIntfRateLimitEnableValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
#endif
/*******************************************************************************
* @function fpObjGet_SwitchingDAI_IntfBurstInterval
*
* @purpose Get 'IntfBurstInterval'
*
* @description [IntfBurstInterval]: The burst interval value of incoming ARP
*              packets on untrusted interfaces 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDAI_IntfBurstInterval (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIntfBurstIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDAI_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDaiIntfBurstIntervalGet (keyInterfaceValue,
                                           &objIntfBurstIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IntfBurstInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIntfBurstIntervalValue,
                           sizeof (objIntfBurstIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingDAI_IntfBurstInterval
*
* @purpose Set 'IntfBurstInterval'
*
* @description [IntfBurstInterval]: The burst interval value of incoming ARP
*              packets on untrusted interfaces 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDAI_IntfBurstInterval (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIntfBurstIntervalValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: IntfBurstInterval */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objIntfBurstIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIntfBurstIntervalValue, owa.len);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDAI_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbDaiIntfBurstIntervalSet (keyInterfaceValue,
                                           objIntfBurstIntervalValue);
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
* @function fpObjGet_SwitchingDAI_VlanIndex
*
* @purpose Get 'VlanIndex'
*
* @description [VlanIndex]: The VLAN-ID or other identifier refering to this
*              VLAN. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDAI_VlanIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVlanIndexValue;
  xLibU32_t nextObjVlanIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDAI_VlanIndex,
                          (xLibU8_t *) & objVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objVlanIndexValue = 0;
    owa.l7rc = usmDbDaiVlanNextGet (objVlanIndexValue,
                                    &nextObjVlanIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objVlanIndexValue, owa.len);
    owa.l7rc = usmDbDaiVlanNextGet (objVlanIndexValue,
                                    &nextObjVlanIndexValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjVlanIndexValue, owa.len);

  /* return the object value: VlanIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjVlanIndexValue,
                           sizeof (objVlanIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingDAI_VLANEnable
*
* @purpose Get 'VLANEnable'
*
* @description [VLANEnable]: Dynamic ARP Inspection is enabled or disabled
*              on a VLAN 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDAI_VLANEnable (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVLANEnableValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDAI_VlanIndex,
                          (xLibU8_t *) & keyVlanIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDaiVlanEnableGet (keyVlanIndexValue,
                                    &objVLANEnableValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: VLANEnable */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVLANEnableValue,
                           sizeof (objVLANEnableValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingDAI_VLANEnable
*
* @purpose Set 'VLANEnable'
*
* @description [VLANEnable]: Dynamic ARP Inspection is enabled or disabled
*              on a VLAN 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDAI_VLANEnable (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVLANEnableValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: VLANEnable */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objVLANEnableValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objVLANEnableValue, owa.len);

  /* retrieve key: VlanIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDAI_VlanIndex,
                          (xLibU8_t *) & keyVlanIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbDaiVlanEnableSet (keyVlanIndexValue,
                                    objVLANEnableValue);
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
* @function fpObjGet_SwitchingDAI_VlanLoggingEnable
*
* @purpose Get 'VlanLoggingEnable'
*
* @description [VlanLoggingEnable]:  DAI logging is enabled or disabled on
*              a VLAN 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDAI_VlanLoggingEnable (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVlanLoggingEnableValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDAI_VlanIndex,
                          (xLibU8_t *) & keyVlanIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDaiVlanLoggingEnableGet (keyVlanIndexValue,
                                           &objVlanLoggingEnableValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: VlanLoggingEnable */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVlanLoggingEnableValue,
                           sizeof (objVlanLoggingEnableValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingDAI_VlanLoggingEnable
*
* @purpose Set 'VlanLoggingEnable'
*
* @description [VlanLoggingEnable]:  DAI logging is enabled or disabled on
*              a VLAN 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDAI_VlanLoggingEnable (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVlanLoggingEnableValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: VlanLoggingEnable */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objVlanLoggingEnableValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objVlanLoggingEnableValue, owa.len);

  /* retrieve key: VlanIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDAI_VlanIndex,
                          (xLibU8_t *) & keyVlanIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbDaiVlanLoggingEnableSet (keyVlanIndexValue,
                                           objVlanLoggingEnableValue);
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
* @function fpObjGet_SwitchingDAI_VlanArpAcl
*
* @purpose Get 'VlanArpAcl'
*
* @description [VlanArpAcl]: ARP ACL Name for a vlan 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDAI_VlanArpAcl (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objVlanArpAclValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDAI_VlanIndex,
                          (xLibU8_t *) & keyVlanIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDaiVlanArpAclGet (keyVlanIndexValue,
                                    objVlanArpAclValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: VlanArpAcl */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objVlanArpAclValue,
                           strlen (objVlanArpAclValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingDAI_VlanArpAcl
*
* @purpose Set 'VlanArpAcl'
*
* @description [VlanArpAcl]: ARP ACL Name for a vlan 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDAI_VlanArpAcl (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objVlanArpAclValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: VlanArpAcl */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objVlanArpAclValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objVlanArpAclValue, owa.len);

  /* retrieve key: VlanIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDAI_VlanIndex,
                          (xLibU8_t *) & keyVlanIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbDaiVlanArpAclSet (keyVlanIndexValue,
                                    objVlanArpAclValue);
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
* @function fpObjGet_SwitchingDAI_VlanArpAclStaticFlag
*
* @purpose Get 'VlanArpAclStaticFlag'
*
* @description [VlanArpAclStaticFlag]: the ARP ACL static flag for a vlan
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDAI_VlanArpAclStaticFlag (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVlanArpAclStaticFlagValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDAI_VlanIndex,
                          (xLibU8_t *) & keyVlanIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDaiVlanArpAclStaticFlagGet (keyVlanIndexValue,
                                     &objVlanArpAclStaticFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: VlanArpAclStaticFlag */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVlanArpAclStaticFlagValue,
                           sizeof (objVlanArpAclStaticFlagValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingDAI_VlanArpAclStaticFlag
*
* @purpose Set 'VlanArpAclStaticFlag'
*
* @description [VlanArpAclStaticFlag]: the ARP ACL static flag for a vlan
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDAI_VlanArpAclStaticFlag (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVlanArpAclStaticFlagValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: VlanArpAclStaticFlag */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objVlanArpAclStaticFlagValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objVlanArpAclStaticFlagValue, owa.len);

  /* retrieve key: VlanIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDAI_VlanIndex,
                          (xLibU8_t *) & keyVlanIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDaiVlanArpAclStaticFlagSet (keyVlanIndexValue,
                                     objVlanArpAclStaticFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

#if 0
/*******************************************************************************
* @function fpObjSet_SwitchingDAI_VLANRangeEnable
*
* @purpose Set 'VLANRangeEnable'
*
* @description [VLANRangeEnable]: Dynamic ARP Inspection is enabled or disabled
*              on a VLAN 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDAI_VLANRangeEnable (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVLANRangeEnableValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: VlanIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingDAI_VlanIndex,
                          (xLibU8_t *) & keyVlanIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanIndexValue, kwa.len);

  /* retrieve object: VLANRangeEnable */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objVLANRangeEnableValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objVLANRangeEnableValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDaiVlanEnableRangeSet (keyVlanIndexValue,
                                keyVlanIndexValue,
                                objVLANRangeEnableValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
#endif

/*******************************************************************************
* @function fpObjSet_SwitchingDAI_VlanStatsClear
*
* @purpose Set 'VlanStatsClear'
*
* @description [VlanStatsClear]: Clear DAI statistics on all vlans 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDAI_VlanStatsClear (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVlanStatsClearValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: VlanStatsClear */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objVlanStatsClearValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objVlanStatsClearValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbDaiVlanStatsClear ();
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
