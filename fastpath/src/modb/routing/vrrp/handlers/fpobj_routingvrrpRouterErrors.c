/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingvrrpRouterErrors.c
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
#include "_xe_routingvrrpRouterErrors_obj.h"
#include "usmdb_mib_vrrp_api.h"

/*******************************************************************************
* @function fpObjGet_routingvrrpRouterErrors_VrIdErrors
*
* @purpose Get 'VrIdErrors'
*
* @description [VrIdErrors]: The total number of VRRP packets received with
*              an invalid VRID for this virtual router. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpRouterErrors_VrIdErrors (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVrIdErrorsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbVrrpRouterVridErrorGet (L7_UNIT_CURRENT, &objVrIdErrorsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objVrIdErrorsValue, sizeof (objVrIdErrorsValue));

  /* return the object value: VrIdErrors */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVrIdErrorsValue,
                           sizeof (objVrIdErrorsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingvrrpRouterErrors_VersionErrors
*
* @purpose Get 'VersionErrors'
*
* @description [VersionErrors]: The total number of VRRP packets received
*              with an unknown or unsupported version number. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpRouterErrors_VersionErrors (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVersionErrorsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbVrrpRouterVerErrorGet (L7_UNIT_CURRENT, &objVersionErrorsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objVersionErrorsValue,
                     sizeof (objVersionErrorsValue));

  /* return the object value: VersionErrors */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVersionErrorsValue,
                           sizeof (objVersionErrorsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingvrrpRouterErrors_ChecksumErrors
*
* @purpose Get 'ChecksumErrors'
*
* @description [ChecksumErrors]: The total number of VRRP packets received
*              with an invalid VRRP checksum value. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingvrrpRouterErrors_ChecksumErrors (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objChecksumErrorsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbVrrpRouterChecksumErrorGet (L7_UNIT_CURRENT, &objChecksumErrorsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objChecksumErrorsValue,
                     sizeof (objChecksumErrorsValue));

  /* return the object value: ChecksumErrors */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objChecksumErrorsValue,
                           sizeof (objChecksumErrorsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
