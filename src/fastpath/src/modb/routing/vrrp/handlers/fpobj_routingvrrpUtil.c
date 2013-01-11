/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingvrrpUtil.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to vrrpOper-object.xml
*
* @create  19 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingvrrpUtil_obj.h"
#include "usmdb_mib_vrrp_api.h"

/*******************************************************************************
* @function fpObjGet_routingvrrpUtil_VMacAddress
*
* @purpose Get 'VMacAddress'
*
* @description [VMacAddress]: To get the Vmac address associated with an IP
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpUtil_VMacAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objVMacAddressValue;
  FPOBJ_TRACE_ENTER (bufp);
  xLibU32_t ipaddress;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));


  kwa.rc = xLibFilterGet (wap, XOBJ_routingvrrpUtil_IpAddress,
                          (xLibU8_t *) & ipaddress, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &ipaddress, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbVrrpVMacGet (L7_UNIT_CURRENT, ipaddress, objVMacAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objVMacAddressValue, strlen (objVMacAddressValue));

  /* return the object value: VMacAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objVMacAddressValue,
                           strlen (objVMacAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingvrrpUtil_IpAddress
*
* @purpose Get 'IpAddress'
*
* @description [IpAddress]:  To get the ip address associated with the VMAC.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpUtil_IpAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIpAddressValue;
  xLibStr256_t objVMacAddressValue;
  FPOBJ_TRACE_ENTER (bufp);
  
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  kwa.rc = xLibFilterGet (wap, XOBJ_routingvrrpUtil_VMacAddress,
                          (xLibU8_t *) & objVMacAddressValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objVMacAddressValue, kwa.len);
  /* get the value from application */
  owa.l7rc = usmDbVrrpIpGet (L7_UNIT_CURRENT, objVMacAddressValue, &objIpAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIpAddressValue, sizeof (objIpAddressValue));

  /* return the object value: IpAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIpAddressValue,
                           sizeof (objIpAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
