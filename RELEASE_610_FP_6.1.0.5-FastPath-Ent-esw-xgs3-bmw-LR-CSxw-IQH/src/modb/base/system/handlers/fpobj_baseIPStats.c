/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseIPStats.c
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
#include "_xe_baseIPStats_obj.h"
#include "usmdb_1213_api.h"

/*******************************************************************************
* @function fpObjGet_baseIPStats_ipInAddrErrors
*
* @purpose Get 'ipInAddrErrors'
*
* @description Ip Address errors 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIPStats_ipInAddrErrors (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipInAddrErrorsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpInAddrErrorsGet (L7_UNIT_CURRENT, &objipInAddrErrorsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipInAddrErrorsValue,
                     sizeof (objipInAddrErrorsValue));

  /* return the object value: ipInAddrErrors */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipInAddrErrorsValue,
                           sizeof (objipInAddrErrorsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseIPStats_ipFragCreates
*
* @purpose Get 'ipFragCreates'
*
* @description IP Flag Creates 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIPStats_ipFragCreates (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipFragCreatesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpFragCreatesGet (L7_UNIT_CURRENT, &objipFragCreatesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipFragCreatesValue,
                     sizeof (objipFragCreatesValue));

  /* return the object value: ipFragCreates */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipFragCreatesValue,
                           sizeof (objipFragCreatesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseIPStats_ipInDiscards
*
* @purpose Get 'ipInDiscards'
*
* @description IP in discards 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIPStats_ipInDiscards (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipInDiscardsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpInDiscards (L7_UNIT_CURRENT, &objipInDiscardsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipInDiscardsValue,
                     sizeof (objipInDiscardsValue));

  /* return the object value: ipInDiscards */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipInDiscardsValue,
                           sizeof (objipInDiscardsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseIPStats_ipInReceives
*
* @purpose Get 'ipInReceives'
*
* @description IP In receives 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIPStats_ipInReceives (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipInReceivesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpInReceivesGet (L7_UNIT_CURRENT, &objipInReceivesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipInReceivesValue,
                     sizeof (objipInReceivesValue));

  /* return the object value: ipInReceives */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipInReceivesValue,
                           sizeof (objipInReceivesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseIPStats_ipFragOKs
*
* @purpose Get 'ipFragOKs'
*
* @description Ip Frags OK 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIPStats_ipFragOKs (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipFragOKsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpFragOKsGet (L7_UNIT_CURRENT, &objipFragOKsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipFragOKsValue, sizeof (objipFragOKsValue));

  /* return the object value: ipFragOKs */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipFragOKsValue,
                           sizeof (objipFragOKsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseIPStats_ipInDelivers
*
* @purpose Get 'ipInDelivers'
*
* @description IP in Delivers 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIPStats_ipInDelivers (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipInDeliversValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpInDelivers (L7_UNIT_CURRENT, &objipInDeliversValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipInDeliversValue,
                     sizeof (objipInDeliversValue));

  /* return the object value: ipInDelivers */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipInDeliversValue,
                           sizeof (objipInDeliversValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseIPStats_ipReasmFails
*
* @purpose Get 'ipReasmFails'
*
* @description IP reasms failed 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIPStats_ipReasmFails (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipReasmFailsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpReasmFailsGet (L7_UNIT_CURRENT, &objipReasmFailsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipReasmFailsValue,
                     sizeof (objipReasmFailsValue));

  /* return the object value: ipReasmFails */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipReasmFailsValue,
                           sizeof (objipReasmFailsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseIPStats_ipRoutingDiscards
*
* @purpose Get 'ipRoutingDiscards'
*
* @description IP routing discards 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIPStats_ipRoutingDiscards (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipRoutingDiscardsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbIpRoutingDiscardsGet (L7_UNIT_CURRENT, &objipRoutingDiscardsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipRoutingDiscardsValue,
                     sizeof (objipRoutingDiscardsValue));

  /* return the object value: ipRoutingDiscards */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipRoutingDiscardsValue,
                           sizeof (objipRoutingDiscardsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseIPStats_ipReasmReqds
*
* @purpose Get 'ipReasmReqds'
*
* @description Ip Reasm reqds 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIPStats_ipReasmReqds (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipReasmReqdsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpReasmReqdsGet (L7_UNIT_CURRENT, &objipReasmReqdsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipReasmReqdsValue,
                     sizeof (objipReasmReqdsValue));

  /* return the object value: ipReasmReqds */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipReasmReqdsValue,
                           sizeof (objipReasmReqdsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseIPStats_ipFragFails
*
* @purpose Get 'ipFragFails'
*
* @description Ip Flag fails 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIPStats_ipFragFails (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipFragFailsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpFragFailsGet (L7_UNIT_CURRENT, &objipFragFailsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipFragFailsValue, sizeof (objipFragFailsValue));

  /* return the object value: ipFragFails */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipFragFailsValue,
                           sizeof (objipFragFailsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseIPStats_ipOutRequests
*
* @purpose Get 'ipOutRequests'
*
* @description Ip Out requests 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIPStats_ipOutRequests (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipOutRequestsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpOutRequests (L7_UNIT_CURRENT, &objipOutRequestsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipOutRequestsValue,
                     sizeof (objipOutRequestsValue));

  /* return the object value: ipOutRequests */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipOutRequestsValue,
                           sizeof (objipOutRequestsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseIPStats_ipOutNoRoutes
*
* @purpose Get 'ipOutNoRoutes'
*
* @description Ip out no of routes 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIPStats_ipOutNoRoutes (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipOutNoRoutesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpNoRoutesGet (L7_UNIT_CURRENT, &objipOutNoRoutesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipOutNoRoutesValue,
                     sizeof (objipOutNoRoutesValue));

  /* return the object value: ipOutNoRoutes */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipOutNoRoutesValue,
                           sizeof (objipOutNoRoutesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseIPStats_ipReasmTimeout
*
* @purpose Get 'ipReasmTimeout'
*
* @description Ip Reasm time out 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIPStats_ipReasmTimeout (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipReasmTimeoutValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpReasmTimeoutGet (L7_UNIT_CURRENT, &objipReasmTimeoutValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipReasmTimeoutValue,
                     sizeof (objipReasmTimeoutValue));

  /* return the object value: ipReasmTimeout */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipReasmTimeoutValue,
                           sizeof (objipReasmTimeoutValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseIPStats_ipInHdrErrors
*
* @purpose Get 'ipInHdrErrors'
*
* @description Ip Header errors 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIPStats_ipInHdrErrors (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipInHdrErrorsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpInHdrErrorsGet (L7_UNIT_CURRENT, &objipInHdrErrorsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipInHdrErrorsValue,
                     sizeof (objipInHdrErrorsValue));

  /* return the object value: ipInHdrErrors */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipInHdrErrorsValue,
                           sizeof (objipInHdrErrorsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseIPStats_ipForwDatagrams
*
* @purpose Get 'ipForwDatagrams'
*
* @description Ip flow datagrams 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIPStats_ipForwDatagrams (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipForwDatagramsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbIpForwDatagramsGet (L7_UNIT_CURRENT, &objipForwDatagramsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipForwDatagramsValue,
                     sizeof (objipForwDatagramsValue));

  /* return the object value: ipForwDatagrams */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipForwDatagramsValue,
                           sizeof (objipForwDatagramsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseIPStats_ipOutDiscards
*
* @purpose Get 'ipOutDiscards'
*
* @description Ip out discards 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIPStats_ipOutDiscards (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipOutDiscardsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpOutDiscardsGet (L7_UNIT_CURRENT, &objipOutDiscardsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipOutDiscardsValue,
                     sizeof (objipOutDiscardsValue));

  /* return the object value: ipOutDiscards */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipOutDiscardsValue,
                           sizeof (objipOutDiscardsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseIPStats_ipReasmOKs
*
* @purpose Get 'ipReasmOKs'
*
* @description IP reasm OK 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIPStats_ipReasmOKs (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipReasmOKsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpReasmOKsGet (L7_UNIT_CURRENT, &objipReasmOKsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipReasmOKsValue, sizeof (objipReasmOKsValue));

  /* return the object value: ipReasmOKs */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipReasmOKsValue,
                           sizeof (objipReasmOKsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseIPStats_ipDefaultTTL
*
* @purpose Get 'ipDefaultTTL'
*
* @description Default time to live 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIPStats_ipDefaultTTL (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipDefaultTTLValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpDefaultTTLGet (L7_UNIT_CURRENT, &objipDefaultTTLValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipDefaultTTLValue,
                     sizeof (objipDefaultTTLValue));

  /* return the object value: ipDefaultTTL */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipDefaultTTLValue,
                           sizeof (objipDefaultTTLValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseIPStats_ipDefaultTTL
*
* @purpose Set 'ipDefaultTTL'
*
* @description Default time to live 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseIPStats_ipDefaultTTL (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipDefaultTTLValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipDefaultTTL */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objipDefaultTTLValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipDefaultTTLValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbIpDefaultTTLSet (L7_UNIT_CURRENT, objipDefaultTTLValue);
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
* @function fpObjGet_baseIPStats_ipInUnknownProtos
*
* @purpose Get 'ipInUnknownProtos'
*
* @description IP unknown protos 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIPStats_ipInUnknownProtos (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipInUnknownProtosValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbIpInUnknownProtosGet (L7_UNIT_CURRENT, &objipInUnknownProtosValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipInUnknownProtosValue,
                     sizeof (objipInUnknownProtosValue));

  /* return the object value: ipInUnknownProtos */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipInUnknownProtosValue,
                           sizeof (objipInUnknownProtosValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
