/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseDnsResCounter.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to dns-object.xml
*
* @create  4 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseDnsResCounter_obj.h"
#include "usmdb_dns_client_api.h"

/*******************************************************************************
* @function fpObjGet_baseDnsResCounter_dnsResCounterByRcodeCode
*
* @purpose Get 'dnsResCounterByRcodeCode'
*
* @description The index to this table. The Response Codes that have already 
*              been defined are found in RFC-1035. 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDnsResCounter_dnsResCounterByRcodeCode (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdnsResCounterByRcodeCodeValue;
  xLibU32_t nextObjdnsResCounterByRcodeCodeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dnsResCounterByRcodeCode */
  owa.rc = xLibFilterGet (wap, XOBJ_baseDnsResCounter_dnsResCounterByRcodeCode,
                          (xLibU8_t *) & objdnsResCounterByRcodeCodeValue,
                          &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbDNSClientCounterByRcodeFirstGet (
		                    &nextObjdnsResCounterByRcodeCodeValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objdnsResCounterByRcodeCodeValue, owa.len);
    if (usmDbDNSClientCounterByRcodeGet(objdnsResCounterByRcodeCodeValue) == L7_SUCCESS)
    {  
       owa.l7rc =
         usmDbDNSClientCounterByRcodeNextGet (&objdnsResCounterByRcodeCodeValue);
       nextObjdnsResCounterByRcodeCodeValue = objdnsResCounterByRcodeCodeValue;
    }
    else
    {
      owa.l7rc = L7_FAILURE;
    }
		
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdnsResCounterByRcodeCodeValue, owa.len);

  /* return the object value: dnsResCounterByRcodeCode */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdnsResCounterByRcodeCodeValue,
                    sizeof (objdnsResCounterByRcodeCodeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDnsResCounter_dnsResCounterByRcodeResponses
*
* @purpose Get 'dnsResCounterByRcodeResponses'
*
* @description Number of responses the resolver has received for the response 
*              code value which identifies this row of the table. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDnsResCounter_dnsResCounterByRcodeResponses (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydnsResCounterByRcodeCodeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdnsResCounterByRcodeResponsesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dnsResCounterByRcodeCode */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDnsResCounter_dnsResCounterByRcodeCode,
                          (xLibU8_t *) & keydnsResCounterByRcodeCodeValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydnsResCounterByRcodeCodeValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDNSClientCounterByRcodeResponsesGet (
                                              keydnsResCounterByRcodeCodeValue,
                                              &objdnsResCounterByRcodeResponsesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dnsResCounterByRcodeResponses */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objdnsResCounterByRcodeResponsesValue,
                    sizeof (objdnsResCounterByRcodeResponsesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDnsResCounter_dnsResCounterByOpcodeCode
*
* @purpose Get 'dnsResCounterByOpcodeCode'
*
* @description The index to this table. The OpCodes that have already been defined 
*              are found in RFC-1035. 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDnsResCounter_dnsResCounterByOpcodeCode (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdnsResCounterByOpcodeCodeValue;
  xLibU32_t nextObjdnsResCounterByOpcodeCodeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dnsResCounterByOpcodeCode */
  owa.rc = xLibFilterGet (wap, XOBJ_baseDnsResCounter_dnsResCounterByOpcodeCode,
                          (xLibU8_t *) & objdnsResCounterByOpcodeCodeValue,
                          &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjdnsResCounterByOpcodeCodeValue = DNS_OPCODE_STANDARD_QUERY;	
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objdnsResCounterByOpcodeCodeValue, owa.len);
    owa.l7rc = L7_FAILURE;
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdnsResCounterByOpcodeCodeValue, owa.len);

  /* return the object value: dnsResCounterByOpcodeCode */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdnsResCounterByOpcodeCodeValue,
                    sizeof (objdnsResCounterByOpcodeCodeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDnsResCounter_dnsResCounterByOpcodeQueries
*
* @purpose Get 'dnsResCounterByOpcodeQueries'
*
* @description Total number of queries that have sent out by the resolver since 
*              initialization for the OpCode which is the index to this row 
*              of the table. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDnsResCounter_dnsResCounterByOpcodeQueries (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydnsResCounterByOpcodeCodeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdnsResCounterByOpcodeQueriesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dnsResCounterByOpcodeCode */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDnsResCounter_dnsResCounterByOpcodeCode,
                          (xLibU8_t *) & keydnsResCounterByOpcodeCodeValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydnsResCounterByOpcodeCodeValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDNSClientCounterByOpcodeQueriesGet (
                                             keydnsResCounterByOpcodeCodeValue,
                                             &objdnsResCounterByOpcodeQueriesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dnsResCounterByOpcodeQueries */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objdnsResCounterByOpcodeQueriesValue,
                    sizeof (objdnsResCounterByOpcodeQueriesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDnsResCounter_dnsResCounterByOpcodeResponses
*
* @purpose Get 'dnsResCounterByOpcodeResponses'
*
* @description Total number of responses that have been received by the resolver 
*              since initialization for the OpCode which is the index to 
*              this row of the table. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDnsResCounter_dnsResCounterByOpcodeResponses (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydnsResCounterByOpcodeCodeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdnsResCounterByOpcodeResponsesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dnsResCounterByOpcodeCode */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDnsResCounter_dnsResCounterByOpcodeCode,
                          (xLibU8_t *) & keydnsResCounterByOpcodeCodeValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydnsResCounterByOpcodeCodeValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDNSClientCounterByOpcodeResponsesGet (
                                               keydnsResCounterByOpcodeCodeValue,
                                               &objdnsResCounterByOpcodeResponsesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dnsResCounterByOpcodeResponses */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objdnsResCounterByOpcodeResponsesValue,
                    sizeof (objdnsResCounterByOpcodeResponsesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDnsResCounter_UnparseResps
*
* @purpose Get 'UnparseResps'
*
* @description Number of responses received which were unparseable. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDnsResCounter_UnparseResps (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnparseRespsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDNSClientCounterUnparsedResponsesGet (&objUnparseRespsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUnparseRespsValue,
                     sizeof (objUnparseRespsValue));

  /* return the object value: UnparseResps */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnparseRespsValue,
                           sizeof (objUnparseRespsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDnsResCounter_RecdResponses
*
* @purpose Get 'RecdResponses'
*
* @description Number of responses received to all queries. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDnsResCounter_RecdResponses (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRecdResponsesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDNSClientCounterReceivedResponsesGet (&objRecdResponsesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRecdResponsesValue,
                     sizeof (objRecdResponsesValue));

  /* return the object value: RecdResponses */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRecdResponsesValue,
                           sizeof (objRecdResponsesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDnsResCounter_Martians
*
* @purpose Get 'Martians'
*
* @description Number of responses received which were received from servers 
*              that the resolver does not think it asked. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDnsResCounter_Martians (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMartiansValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDNSClientCounterMartiansGet (&objMartiansValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMartiansValue, sizeof (objMartiansValue));

  /* return the object value: Martians */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMartiansValue,
                           sizeof (objMartiansValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDnsResCounter_NonAuthNoDataResps
*
* @purpose Get 'NonAuthNoDataResps'
*
* @description Number of requests made by the resolver for which a non-authoritative 
*              answer - no such data response (empty answer) was received. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDnsResCounter_NonAuthNoDataResps (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNonAuthNoDataRespsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDNSClientCounterNonAuthNoDataResponsesGet (&objNonAuthNoDataRespsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNonAuthNoDataRespsValue,
                     sizeof (objNonAuthNoDataRespsValue));

  /* return the object value: NonAuthNoDataResps */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNonAuthNoDataRespsValue,
                           sizeof (objNonAuthNoDataRespsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDnsResCounter_NonAuthDataResps
*
* @purpose Get 'NonAuthDataResps'
*
* @description Number of requests made by the resolver for which a non-authoritative 
*              answer (cached data) was received. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDnsResCounter_NonAuthDataResps (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNonAuthDataRespsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDNSClientCounterNonAuthDataResponsesGet (&objNonAuthDataRespsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNonAuthDataRespsValue,
                     sizeof (objNonAuthDataRespsValue));

  /* return the object value: NonAuthDataResps */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNonAuthDataRespsValue,
                           sizeof (objNonAuthDataRespsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDnsResCounter_Fallbacks
*
* @purpose Get 'Fallbacks'
*
* @description Number of times the resolver had to fall back to its seat belt 
*              information. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDnsResCounter_Fallbacks (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objFallbacksValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDNSClientCounterFallbacksGet (&objFallbacksValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objFallbacksValue, sizeof (objFallbacksValue));

  /* return the object value: Fallbacks */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objFallbacksValue,
                           sizeof (objFallbacksValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
