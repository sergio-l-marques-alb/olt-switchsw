/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseIPAddrInfo.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to sntp-object.xml
*
* @create  14 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseIPAddrInfo_obj.h"
#include "usmdb_1213_api.h"

/*******************************************************************************
* @function fpObjGet_baseIPAddrInfo_ipAdEntAddr
*
* @purpose Get 'ipAdEntAddr'
*
* @description IP Address 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIPAddrInfo_ipAdEntAddr (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipAdEntAddrValue;
  xLibU32_t nextObjipAdEntAddrValue;
  xLibU32_t tempIntfValue = 0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipAdEntAddr */
  owa.rc = xLibFilterGet (wap, XOBJ_baseIPAddrInfo_ipAdEntAddr,
                          (xLibU8_t *) & objipAdEntAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjipAdEntAddrValue = 0;	
    owa.l7rc = usmDbIpAdEntAddrGet (L7_UNIT_CURRENT, nextObjipAdEntAddrValue,tempIntfValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objipAdEntAddrValue, owa.len);
	
    owa.l7rc = usmDbIpAdEntAddrGetNext (L7_UNIT_CURRENT, &objipAdEntAddrValue,
                                        &tempIntfValue);
    nextObjipAdEntAddrValue = objipAdEntAddrValue;
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjipAdEntAddrValue, owa.len);

  /* return the object value: ipAdEntAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjipAdEntAddrValue,
                           sizeof (objipAdEntAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseIPAddrInfo_ipAdEntIfIndex
*
* @purpose Get 'ipAdEntIfIndex'
*
* @description IPAdress table index 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIPAddrInfo_ipAdEntIfIndex (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipAdEntAddrValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipAdEntIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipAdEntAddr */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseIPAddrInfo_ipAdEntAddr,
                          (xLibU8_t *) & keyipAdEntAddrValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipAdEntAddrValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIpEntIfIndexGet (L7_UNIT_CURRENT, keyipAdEntAddrValue,
                                   &objipAdEntIfIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipAdEntIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipAdEntIfIndexValue,
                           sizeof (objipAdEntIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseIPAddrInfo_ipAdEntNetMask
*
* @purpose Get 'ipAdEntNetMask'
*
* @description Ip Mask 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIPAddrInfo_ipAdEntNetMask (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipAdEntAddrValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipAdEntNetMaskValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipAdEntAddr */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseIPAddrInfo_ipAdEntAddr,
                          (xLibU8_t *) & keyipAdEntAddrValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipAdEntAddrValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIpAdEntNetMaskGet (L7_UNIT_CURRENT, keyipAdEntAddrValue,
                                     &objipAdEntNetMaskValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipAdEntNetMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipAdEntNetMaskValue,
                           sizeof (objipAdEntNetMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseIPAddrInfo_ipAdEntBcastAddr
*
* @purpose Get 'ipAdEntBcastAddr'
*
* @description IP Broadcast address 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIPAddrInfo_ipAdEntBcastAddr (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipAdEntAddrValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipAdEntBcastAddrValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipAdEntAddr */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseIPAddrInfo_ipAdEntAddr,
                          (xLibU8_t *) & keyipAdEntAddrValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipAdEntAddrValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIpAdEntBCastGet (L7_UNIT_CURRENT, keyipAdEntAddrValue,
                                   &objipAdEntBcastAddrValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipAdEntBcastAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipAdEntBcastAddrValue,
                           sizeof (objipAdEntBcastAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseIPAddrInfo_ipAdEntReasmMaxSize
*
* @purpose Get 'ipAdEntReasmMaxSize'
*
* @description Ip Address Reasm Max Size 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIPAddrInfo_ipAdEntReasmMaxSize (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipAdEntAddrValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipAdEntReasmMaxSizeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipAdEntAddr */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseIPAddrInfo_ipAdEntAddr,
                          (xLibU8_t *) & keyipAdEntAddrValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipAdEntAddrValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIpAdEntReasmMaxSizeGet (L7_UNIT_CURRENT, keyipAdEntAddrValue,
                                          &objipAdEntReasmMaxSizeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipAdEntReasmMaxSize */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipAdEntReasmMaxSizeValue,
                           sizeof (objipAdEntReasmMaxSizeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
