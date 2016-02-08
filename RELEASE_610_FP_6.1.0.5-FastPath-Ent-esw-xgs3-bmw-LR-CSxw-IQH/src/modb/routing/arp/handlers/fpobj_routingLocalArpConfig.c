/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingLocalArpConfig.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to arp-object.xml
*
* @create  16 February 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingLocalArpConfig_obj.h"
#include "usmdb_util_api.h"
#include "usmdb_iputil_api.h"
#include "usmdb_1213_api.h"
#include "usmdb_ip_api.h"

static L7_RC_t usmDbIpArpEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *intIfNum)
{
  L7_int32 temp_val;
  /* loop through available external interface numbers */
  while (usmDbValidIntIfNumNext(*intIfNum, intIfNum) == L7_SUCCESS)
  {
    /* check to see if this is a valid routing interface */
    if ( (usmDbValidateRtrIntf(UnitIndex, *intIfNum) == L7_SUCCESS) &&
         (usmDbVisibleInterfaceCheck(UnitIndex, *intIfNum, &temp_val) == L7_SUCCESS) )
    return L7_SUCCESS;

  }

  return L7_FAILURE;
}


/*******************************************************************************
* @function fpObjGet_routingLocalArpConfig_interface
*
* @purpose Get 'interface'
*
* @description [interface]: Interfaces for ARP Configuration 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingLocalArpConfig_interface (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objinterfaceValue;
  xLibU32_t nextObjinterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: interface */
  owa.rc = xLibFilterGet (wap, XOBJ_routingLocalArpConfig_interface,
                          (xLibU8_t *) & objinterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjinterfaceValue = 0;
    owa.l7rc = usmDbIpArpEntryNextGet(L7_UNIT_CURRENT, &nextObjinterfaceValue);
  }
  else
  {
    nextObjinterfaceValue = objinterfaceValue;
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objinterfaceValue, owa.len);
    owa.l7rc = usmDbIpArpEntryNextGet(L7_UNIT_CURRENT, &nextObjinterfaceValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjinterfaceValue, owa.len);

  /* return the object value: interface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjinterfaceValue,
                           sizeof (objinterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingLocalArpConfig_intflProxyArpMode
*
* @purpose Get 'intflProxyArpMode'
*
* @description [intflProxyArpMode]: The Local Proxy Arp Mode for the interface.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingLocalArpConfig_intflProxyArpMode (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objintflProxyArpModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingLocalArpConfig_interface,
                          (xLibU8_t *) & keyinterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinterfaceValue, kwa.len);

  owa.l7rc = usmDbValidateRtrIntf(L7_UNIT_CURRENT, keyinterfaceValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_INVALID_RTRINTERFACE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbLocalProxyArpGet (L7_UNIT_CURRENT, keyinterfaceValue,
                                    &objintflProxyArpModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: intflProxyArpMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objintflProxyArpModeValue,
                           sizeof (objintflProxyArpModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingLocalArpConfig_intflProxyArpMode
*
* @purpose Set 'intflProxyArpMode'
*
* @description [intflProxyArpMode]: The Local Proxy Arp Mode for the interface.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingLocalArpConfig_intflProxyArpMode (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objintflProxyArpModeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: intflProxyArpMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objintflProxyArpModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objintflProxyArpModeValue, owa.len);

  /* retrieve key: interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingLocalArpConfig_interface,
                          (xLibU8_t *) & keyinterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinterfaceValue, kwa.len);

  owa.l7rc = usmDbValidateRtrIntf(L7_UNIT_CURRENT, keyinterfaceValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_INVALID_RTRINTERFACE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbLocalProxyArpSet (L7_UNIT_CURRENT, keyinterfaceValue,
                                    objintflProxyArpModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
