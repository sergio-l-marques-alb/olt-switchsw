/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseIpRoute.c
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
#include "_xe_baseIpRoute_obj.h"
#include "usmdb_1213_api.h"

/*******************************************************************************
* @function fpObjGet_baseIpRoute_ipRouteDest
*
* @purpose Get 'ipRouteDest'
*
* @description Determines if a route entry exists for the specified route exists 
*              
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIpRoute_ipRouteDest (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRouteDestValue;
  xLibU32_t nextObjipRouteDestValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipRouteDest */
  owa.rc = xLibFilterGet (wap, XOBJ_baseIpRoute_ipRouteDest,
                          (xLibU8_t *) & objipRouteDestValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjipRouteDestValue = 0;
    owa.l7rc = usmDbIpRouteEntryGet (L7_UNIT_CURRENT, nextObjipRouteDestValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objipRouteDestValue, owa.len);
    owa.l7rc = usmDbIpRouteEntryGetNext (L7_UNIT_CURRENT, &objipRouteDestValue);
    nextObjipRouteDestValue = objipRouteDestValue;
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjipRouteDestValue, owa.len);

  /* return the object value: ipRouteDest */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjipRouteDestValue,
                           sizeof (objipRouteDestValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseIpRoute_ipRouteIfIndex
*
* @purpose Get 'ipRouteIfIndex'
*
* @description the index value which uniquely identifies the local interface 
*              through which the next hop of this route should be reached 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIpRoute_ipRouteIfIndex (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipRouteDestValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRouteIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipRouteDest */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseIpRoute_ipRouteDest,
                          (xLibU8_t *) & keyipRouteDestValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipRouteDestValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIpRouteIfIndexGet (L7_UNIT_CURRENT, keyipRouteDestValue,
                                     &objipRouteIfIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipRouteIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipRouteIfIndexValue,
                           sizeof (objipRouteIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseIpRoute_ipRouteIfIndex
*
* @purpose Set 'ipRouteIfIndex'
*
* @description the index value which uniquely identifies the local interface 
*              through which the next hop of this route should be reached 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseIpRoute_ipRouteIfIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRouteIfIndexValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipRouteDestValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipRouteIfIndex */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objipRouteIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipRouteIfIndexValue, owa.len);

  /* retrieve key: ipRouteDest */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseIpRoute_ipRouteDest,
                          (xLibU8_t *) & keyipRouteDestValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipRouteDestValue, kwa.len);

  /* set the value in application */
  /*owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keyipRouteDestValue,
                              objipRouteIfIndexValue);*/
  owa.l7rc = L7_FAILURE;
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
* @function fpObjGet_baseIpRoute_ipRouteMetric1
*
* @purpose Get 'ipRouteMetric1'
*
* @description he primary routing metric for this route 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIpRoute_ipRouteMetric1 (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipRouteDestValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRouteMetric1Value;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipRouteDest */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseIpRoute_ipRouteDest,
                          (xLibU8_t *) & keyipRouteDestValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipRouteDestValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIpRouteMetric1Get (L7_UNIT_CURRENT, keyipRouteDestValue,
                                     &objipRouteMetric1Value);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipRouteMetric1 */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipRouteMetric1Value,
                           sizeof (objipRouteMetric1Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseIpRoute_ipRouteMetric1
*
* @purpose Set 'ipRouteMetric1'
*
* @description he primary routing metric for this route 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseIpRoute_ipRouteMetric1 (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRouteMetric1Value;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipRouteDestValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipRouteMetric1 */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objipRouteMetric1Value, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipRouteMetric1Value, owa.len);

  /* retrieve key: ipRouteDest */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseIpRoute_ipRouteDest,
                          (xLibU8_t *) & keyipRouteDestValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipRouteDestValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbIpRouteMetric1Set (L7_UNIT_CURRENT, keyipRouteDestValue,
                                     objipRouteMetric1Value);
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
* @function fpObjGet_baseIpRoute_ipRouteMetric2
*
* @purpose Get 'ipRouteMetric2'
*
* @description the alternate routing metric for this route 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIpRoute_ipRouteMetric2 (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipRouteDestValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRouteMetric2Value;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipRouteDest */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseIpRoute_ipRouteDest,
                          (xLibU8_t *) & keyipRouteDestValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipRouteDestValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIpRouteMetric2Get (L7_UNIT_CURRENT, keyipRouteDestValue,
                                     &objipRouteMetric2Value);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipRouteMetric2 */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipRouteMetric2Value,
                           sizeof (objipRouteMetric2Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseIpRoute_ipRouteMetric2
*
* @purpose Set 'ipRouteMetric2'
*
* @description the alternate routing metric for this route 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseIpRoute_ipRouteMetric2 (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRouteMetric2Value;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipRouteDestValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipRouteMetric2 */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objipRouteMetric2Value, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipRouteMetric2Value, owa.len);

  /* retrieve key: ipRouteDest */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseIpRoute_ipRouteDest,
                          (xLibU8_t *) & keyipRouteDestValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipRouteDestValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbIpRouteMetric2Set (L7_UNIT_CURRENT, keyipRouteDestValue,
                                     objipRouteMetric2Value);
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
* @function fpObjGet_baseIpRoute_ipRouteMetric3
*
* @purpose Get 'ipRouteMetric3'
*
* @description the alternate routing metric for this route 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIpRoute_ipRouteMetric3 (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipRouteDestValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRouteMetric3Value;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipRouteDest */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseIpRoute_ipRouteDest,
                          (xLibU8_t *) & keyipRouteDestValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipRouteDestValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIpRouteMetric3Get (L7_UNIT_CURRENT, keyipRouteDestValue,
                                     &objipRouteMetric3Value);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipRouteMetric3 */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipRouteMetric3Value,
                           sizeof (objipRouteMetric3Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseIpRoute_ipRouteMetric3
*
* @purpose Set 'ipRouteMetric3'
*
* @description the alternate routing metric for this route 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseIpRoute_ipRouteMetric3 (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRouteMetric3Value;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipRouteDestValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipRouteMetric3 */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objipRouteMetric3Value, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipRouteMetric3Value, owa.len);

  /* retrieve key: ipRouteDest */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseIpRoute_ipRouteDest,
                          (xLibU8_t *) & keyipRouteDestValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipRouteDestValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbIpRouteMetric3Set (L7_UNIT_CURRENT, keyipRouteDestValue,
                                     objipRouteMetric3Value);
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
* @function fpObjGet_baseIpRoute_ipRouteMetric4
*
* @purpose Get 'ipRouteMetric4'
*
* @description the alternate routing metric for this route 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIpRoute_ipRouteMetric4 (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipRouteDestValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRouteMetric4Value;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipRouteDest */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseIpRoute_ipRouteDest,
                          (xLibU8_t *) & keyipRouteDestValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipRouteDestValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIpRouteMetric4Get (L7_UNIT_CURRENT, keyipRouteDestValue,
                                     &objipRouteMetric4Value);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipRouteMetric4 */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipRouteMetric4Value,
                           sizeof (objipRouteMetric4Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseIpRoute_ipRouteMetric4
*
* @purpose Set 'ipRouteMetric4'
*
* @description the alternate routing metric for this route 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseIpRoute_ipRouteMetric4 (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRouteMetric4Value;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipRouteDestValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipRouteMetric4 */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objipRouteMetric4Value, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipRouteMetric4Value, owa.len);

  /* retrieve key: ipRouteDest */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseIpRoute_ipRouteDest,
                          (xLibU8_t *) & keyipRouteDestValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipRouteDestValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbIpRouteMetric4Set (L7_UNIT_CURRENT, keyipRouteDestValue,
                                     &objipRouteMetric4Value);
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
* @function fpObjGet_baseIpRoute_ipRouteNextHop
*
* @purpose Get 'ipRouteNextHop'
*
* @description Next Hop address 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIpRoute_ipRouteNextHop (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipRouteDestValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRouteNextHopValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipRouteDest */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseIpRoute_ipRouteDest,
                          (xLibU8_t *) & keyipRouteDestValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipRouteDestValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIpRouteNextHopGet (L7_UNIT_CURRENT, keyipRouteDestValue,
                                     &objipRouteNextHopValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipRouteNextHop */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipRouteNextHopValue,
                           sizeof (objipRouteNextHopValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseIpRoute_ipRouteNextHop
*
* @purpose Set 'ipRouteNextHop'
*
* @description Next Hop address 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseIpRoute_ipRouteNextHop (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRouteNextHopValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipRouteDestValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipRouteNextHop */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objipRouteNextHopValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipRouteNextHopValue, owa.len);

  /* retrieve key: ipRouteDest */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseIpRoute_ipRouteDest,
                          (xLibU8_t *) & keyipRouteDestValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipRouteDestValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbIpRouteNextHopSet (L7_UNIT_CURRENT, keyipRouteDestValue,
                                     objipRouteNextHopValue);
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
* @function fpObjGet_baseIpRoute_ipRouteType
*
* @purpose Get 'ipRouteType'
*
* @description Route Type 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIpRoute_ipRouteType (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipRouteDestValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRouteTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipRouteDest */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseIpRoute_ipRouteDest,
                          (xLibU8_t *) & keyipRouteDestValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipRouteDestValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIpRouteTypeGet (L7_UNIT_CURRENT, keyipRouteDestValue,
                                  &objipRouteTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipRouteType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipRouteTypeValue,
                           sizeof (objipRouteTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseIpRoute_ipRouteType
*
* @purpose Set 'ipRouteType'
*
* @description Route Type 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseIpRoute_ipRouteType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRouteTypeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipRouteDestValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipRouteType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objipRouteTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipRouteTypeValue, owa.len);

  /* retrieve key: ipRouteDest */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseIpRoute_ipRouteDest,
                          (xLibU8_t *) & keyipRouteDestValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipRouteDestValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbIpRouteTypeSet (L7_UNIT_CURRENT, keyipRouteDestValue,
                                  objipRouteTypeValue);
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
* @function fpObjGet_baseIpRoute_ipRouteProto
*
* @purpose Get 'ipRouteProto'
*
* @description Route Proto 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIpRoute_ipRouteProto (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipRouteDestValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRouteProtoValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipRouteDest */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseIpRoute_ipRouteDest,
                          (xLibU8_t *) & keyipRouteDestValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipRouteDestValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIpRouteProtoGet (L7_UNIT_CURRENT, keyipRouteDestValue,
                                   &objipRouteProtoValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipRouteProto */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipRouteProtoValue,
                           sizeof (objipRouteProtoValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseIpRoute_ipRouteAge
*
* @purpose Get 'ipRouteAge'
*
* @description Route Age 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIpRoute_ipRouteAge (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipRouteDestValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRouteAgeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipRouteDest */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseIpRoute_ipRouteDest,
                          (xLibU8_t *) & keyipRouteDestValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipRouteDestValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIpRouteAgeGet (L7_UNIT_CURRENT, keyipRouteDestValue,
                                 &objipRouteAgeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipRouteAge */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipRouteAgeValue,
                           sizeof (objipRouteAgeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseIpRoute_ipRouteAge
*
* @purpose Set 'ipRouteAge'
*
* @description Route Age 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseIpRoute_ipRouteAge (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRouteAgeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipRouteDestValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipRouteAge */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objipRouteAgeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipRouteAgeValue, owa.len);

  /* retrieve key: ipRouteDest */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseIpRoute_ipRouteDest,
                          (xLibU8_t *) & keyipRouteDestValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipRouteDestValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbIpRouteAgeSet (L7_UNIT_CURRENT, keyipRouteDestValue,
                                 objipRouteAgeValue);
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
* @function fpObjGet_baseIpRoute_ipRouteMask
*
* @purpose Get 'ipRouteMask'
*
* @description Route Mask 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIpRoute_ipRouteMask (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipRouteDestValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRouteMaskValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipRouteDest */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseIpRoute_ipRouteDest,
                          (xLibU8_t *) & keyipRouteDestValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipRouteDestValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIpRouteMaskGet (L7_UNIT_CURRENT, keyipRouteDestValue,
                                  &objipRouteMaskValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipRouteMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipRouteMaskValue,
                           sizeof (objipRouteMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseIpRoute_ipRouteMask
*
* @purpose Set 'ipRouteMask'
*
* @description Route Mask 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseIpRoute_ipRouteMask (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRouteMaskValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipRouteDestValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipRouteMask */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objipRouteMaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipRouteMaskValue, owa.len);

  /* retrieve key: ipRouteDest */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseIpRoute_ipRouteDest,
                          (xLibU8_t *) & keyipRouteDestValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipRouteDestValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbIpRouteMaskSet (L7_UNIT_CURRENT, keyipRouteDestValue,
                                  objipRouteMaskValue);
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
* @function fpObjGet_baseIpRoute_ipRouteMetric5
*
* @purpose Get 'ipRouteMetric5'
*
* @description the alternate routing metric for this route 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIpRoute_ipRouteMetric5 (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipRouteDestValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRouteMetric5Value;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipRouteDest */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseIpRoute_ipRouteDest,
                          (xLibU8_t *) & keyipRouteDestValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipRouteDestValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIpRouteMetric5Get (L7_UNIT_CURRENT, keyipRouteDestValue,
                                     &objipRouteMetric5Value);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipRouteMetric5 */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipRouteMetric5Value,
                           sizeof (objipRouteMetric5Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseIpRoute_ipRouteMetric5
*
* @purpose Set 'ipRouteMetric5'
*
* @description the alternate routing metric for this route 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseIpRoute_ipRouteMetric5 (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRouteMetric5Value;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipRouteDestValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipRouteMetric5 */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objipRouteMetric5Value, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipRouteMetric5Value, owa.len);

  /* retrieve key: ipRouteDest */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseIpRoute_ipRouteDest,
                          (xLibU8_t *) & keyipRouteDestValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipRouteDestValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbIpRouteMetric5Set (L7_UNIT_CURRENT, keyipRouteDestValue,
                                     &objipRouteMetric5Value);
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
* @function fpObjGet_baseIpRoute_ipRouteInfo
*
* @purpose Get 'ipRouteInfo'
*
* @description Route Info 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIpRoute_ipRouteInfo (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipRouteDestValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objipRouteInfoValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipRouteDest */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseIpRoute_ipRouteDest,
                          (xLibU8_t *) & keyipRouteDestValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipRouteDestValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIpRouteInfoGet (L7_UNIT_CURRENT, keyipRouteDestValue,
                                  objipRouteInfoValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipRouteInfo */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objipRouteInfoValue,
                           strlen (objipRouteInfoValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
