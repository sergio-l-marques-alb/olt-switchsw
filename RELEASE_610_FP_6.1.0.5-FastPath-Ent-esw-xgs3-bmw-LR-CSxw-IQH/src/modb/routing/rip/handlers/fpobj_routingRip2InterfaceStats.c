/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingRip2InterfaceStats.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to Rip2Interface-object.xml
*
* @create  22 February 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingRip2InterfaceStats_obj.h"
#include "usmdb_mib_ripv2_api.h"
#include "usmdb_iputil_api.h"
#include "usmdb_1213_api.h"

/*******************************************************************************
* @function fpObjGet_routingRip2InterfaceStats_Address
*
* @purpose Get 'Address'
*
* @description [Address]: The IP Address of this system on the indicated subnet.
*              For unnumbered interfaces, the value 0.0.0.N, where
*              the least significant 24 bits (N) is the ifIndex for the IP
*              Interface in network byte order. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingRip2InterfaceStats_Address (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAddressValue;
  xLibU32_t nextObjAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Address */
  owa.rc = xLibFilterGet (wap, XOBJ_routingRip2InterfaceStats_Address,
                          (xLibU8_t *) & objAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjAddressValue = 0;
    owa.l7rc = usmDbRip2IfStatEntryNext(L7_UNIT_CURRENT, &nextObjAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objAddressValue, owa.len);
    nextObjAddressValue = objAddressValue;
    owa.l7rc = usmDbRip2IfStatEntryNext(L7_UNIT_CURRENT,
                                    &nextObjAddressValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjAddressValue, owa.len);

  /* return the object value: Address */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjAddressValue,
                           sizeof (objAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingRip2InterfaceStats_RcvBadPackets
*
* @purpose Get 'RcvBadPackets'
*
* @description [RcvBadPackets]: The number of RIP response packets received
*              by the RIP process which were subsequently discarded for
*              any reason (e.g. a version 0 packet, or an unknown command
*              type). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingRip2InterfaceStats_RcvBadPackets (void *wap,
                                                           void *bufp)
{
  xLibU32_t keyAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRcvBadPacketsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Address */
  owa.rc = xLibFilterGet (wap, XOBJ_routingRip2InterfaceStats_Address,
                          (xLibU8_t *) & keyAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressValue, owa.len);
#if 0
  if (usmDbRip2IfIPAddrToIntIf(L7_UNIT_CURRENT, keyAddressValue, &interface) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
#endif
  /* get the value from application */
  owa.l7rc = usmDbRip2IfStatRcvBadPacketsGet (L7_UNIT_CURRENT, keyAddressValue,
                                              &objRcvBadPacketsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RcvBadPackets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRcvBadPacketsValue,
                           sizeof (objRcvBadPacketsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingRip2InterfaceStats_RcvBadRoutes
*
* @purpose Get 'RcvBadRoutes'
*
* @description [RcvBadRoutes]: The number of routes, in valid RIP packets,
*              which were ignored for any reason (e.g. unknown address family,
*              or invalid metric). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingRip2InterfaceStats_RcvBadRoutes (void *wap, void *bufp)
{
  xLibU32_t keyAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRcvBadRoutesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Address */
  owa.rc = xLibFilterGet (wap, XOBJ_routingRip2InterfaceStats_Address,
                          (xLibU8_t *) & keyAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressValue, owa.len);
#if 0
  if (usmDbRip2IfIPAddrToIntIf(L7_UNIT_CURRENT, keyAddressValue, &interface) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
#endif
  /* get the value from application */
  owa.l7rc = usmDbRip2IfStatRcvBadRoutesGet (L7_UNIT_CURRENT, keyAddressValue,
                                             &objRcvBadRoutesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RcvBadRoutes */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRcvBadRoutesValue,
                           sizeof (objRcvBadRoutesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingRip2InterfaceStats_SentUpdates
*
* @purpose Get 'SentUpdates'
*
* @description [SentUpdates]: The number of triggered RIP updates actually
*              sent on this interface. This explicitly does NOT include full
*              updates sent containing new information. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingRip2InterfaceStats_SentUpdates (void *wap, void *bufp)
{
  xLibU32_t keyAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSentUpdatesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Address */
  owa.rc = xLibFilterGet (wap, XOBJ_routingRip2InterfaceStats_Address,
                          (xLibU8_t *) & keyAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressValue, owa.len);
#if 0
  if (usmDbRip2IfIPAddrToIntIf(L7_UNIT_CURRENT, keyAddressValue, &interface) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
#endif
  /* get the value from application */
  owa.l7rc = usmDbRip2IfStatSentUpdatesGet (L7_UNIT_CURRENT, keyAddressValue,
                                            &objSentUpdatesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SentUpdates */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSentUpdatesValue,
                           sizeof (objSentUpdatesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingRip2InterfaceStats_Status
*
* @purpose Get 'Status'
*
* @description [Status]: Writing invalid has the effect of deleting this interface.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingRip2InterfaceStats_Status (void *wap, void *bufp)
{
  xLibU32_t keyAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Address */
  owa.rc = xLibFilterGet (wap, XOBJ_routingRip2InterfaceStats_Address,
                          (xLibU8_t *) & keyAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressValue, owa.len);
#if 0
  if (usmDbRip2IfIPAddrToIntIf(L7_UNIT_CURRENT, keyAddressValue, &interface) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
#endif
  /* get the value from application */
  owa.l7rc = usmDbRip2IfStatStatusGet (L7_UNIT_CURRENT, keyAddressValue,
                                       &objStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Status */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStatusValue,
                           sizeof (objStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingRip2InterfaceStats_Status
*
* @purpose Set 'Status'
*
* @description [Status]: Writing invalid has the effect of deleting this interface.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingRip2InterfaceStats_Status (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Status */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStatusValue, owa.len);

  /* retrieve key: Address */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingRip2InterfaceStats_Address,
                          (xLibU8_t *) & keyAddressValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressValue, kwa.len);
#if 0
  if (usmDbRip2IfIPAddrToIntIf(L7_UNIT_CURRENT, keyAddressValue, &interface) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
#endif
  if(usmDbValidateRtrIntf(L7_UNIT_CURRENT, keyAddressValue) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbRip2IfStatStatusSet (L7_UNIT_CURRENT, keyAddressValue,
                                       objStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
