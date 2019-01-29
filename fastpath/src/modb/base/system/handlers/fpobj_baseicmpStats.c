/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseicmpStats.c
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
#include "_xe_baseicmpStats_obj.h"
#include "usmdb_1213_api.h"

/*******************************************************************************
* @function fpObjGet_baseicmpStats_icmpInEchoReps
*
* @purpose Get 'icmpInEchoReps'
*
* @description The number of ICMP Echo Reply messages received. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseicmpStats_icmpInEchoReps (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objicmpInEchoRepsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIcmpInEchoRepsGet (L7_UNIT_CURRENT, &objicmpInEchoRepsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objicmpInEchoRepsValue,
                     sizeof (objicmpInEchoRepsValue));

  /* return the object value: icmpInEchoReps */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objicmpInEchoRepsValue,
                           sizeof (objicmpInEchoRepsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseicmpStats_icmpOutParmProbs
*
* @purpose Get 'icmpOutParmProbs'
*
* @description The number of ICMP Parameter Problem messages sent. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseicmpStats_icmpOutParmProbs (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objicmpOutParmProbsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbIcmpOutParmProbsGet (L7_UNIT_CURRENT, &objicmpOutParmProbsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objicmpOutParmProbsValue,
                     sizeof (objicmpOutParmProbsValue));

  /* return the object value: icmpOutParmProbs */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objicmpOutParmProbsValue,
                           sizeof (objicmpOutParmProbsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseicmpStats_icmpOutEchos
*
* @purpose Get 'icmpOutEchos'
*
* @description The number of ICMP Echo (request) messages sent. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseicmpStats_icmpOutEchos (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objicmpOutEchosValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIcmpOutEchosGet (L7_UNIT_CURRENT, &objicmpOutEchosValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objicmpOutEchosValue,
                     sizeof (objicmpOutEchosValue));

  /* return the object value: icmpOutEchos */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objicmpOutEchosValue,
                           sizeof (objicmpOutEchosValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseicmpStats_icmpOutRedirects
*
* @purpose Get 'icmpOutRedirects'
*
* @description The number of ICMP Redirect messages sent. For a host, this object 
*              will always be zero, since hosts do not send redirects. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseicmpStats_icmpOutRedirects (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objicmpOutRedirectsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbIcmpOutRedirectsGet (L7_UNIT_CURRENT, &objicmpOutRedirectsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objicmpOutRedirectsValue,
                     sizeof (objicmpOutRedirectsValue));

  /* return the object value: icmpOutRedirects */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objicmpOutRedirectsValue,
                           sizeof (objicmpOutRedirectsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseicmpStats_icmpOutTimestampReps
*
* @purpose Get 'icmpOutTimestampReps'
*
* @description The number of ICMP Timestamp Reply messages sent. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseicmpStats_icmpOutTimestampReps (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objicmpOutTimestampRepsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbIcmpOutTimestampRepsGet (L7_UNIT_CURRENT,
                                  &objicmpOutTimestampRepsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objicmpOutTimestampRepsValue,
                     sizeof (objicmpOutTimestampRepsValue));

  /* return the object value: icmpOutTimestampReps */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objicmpOutTimestampRepsValue,
                           sizeof (objicmpOutTimestampRepsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseicmpStats_icmpInErrors
*
* @purpose Get 'icmpInErrors'
*
* @description The number of ICMP messages which the entity received but determined 
*              as having ICMP-specific errors (bad ICMP checksums, bad 
*              length, etc.). 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseicmpStats_icmpInErrors (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objicmpInErrorsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIcmpInErrorsGet (L7_UNIT_CURRENT, &objicmpInErrorsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objicmpInErrorsValue,
                     sizeof (objicmpInErrorsValue));

  /* return the object value: icmpInErrors */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objicmpInErrorsValue,
                           sizeof (objicmpInErrorsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseicmpStats_icmpInRedirects
*
* @purpose Get 'icmpInRedirects'
*
* @description The number of ICMP Redirect messages received. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseicmpStats_icmpInRedirects (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objicmpInRedirectsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbIcmpInRedirectsGet (L7_UNIT_CURRENT, &objicmpInRedirectsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objicmpInRedirectsValue,
                     sizeof (objicmpInRedirectsValue));

  /* return the object value: icmpInRedirects */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objicmpInRedirectsValue,
                           sizeof (objicmpInRedirectsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseicmpStats_icmpInParmProbs
*
* @purpose Get 'icmpInParmProbs'
*
* @description The number of ICMP Parameter Problem messages received. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseicmpStats_icmpInParmProbs (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objicmpInParmProbsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbIcmpInParmProbsGet (L7_UNIT_CURRENT, &objicmpInParmProbsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objicmpInParmProbsValue,
                     sizeof (objicmpInParmProbsValue));

  /* return the object value: icmpInParmProbs */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objicmpInParmProbsValue,
                           sizeof (objicmpInParmProbsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseicmpStats_icmpOutErrors
*
* @purpose Get 'icmpOutErrors'
*
* @description The number of ICMP messages which this entity did not send due 
*              to problems discovered within ICMP such as a lack of buffers. 
*              This value should not include errors discovered outside the 
*              ICMP layer such as the inability of IP to route the resultant 
*              datagram. In some implementations there may be no types of 
*              error which contribute to this counter's value. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseicmpStats_icmpOutErrors (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objicmpOutErrorsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIcmpOutErrorsGet (L7_UNIT_CURRENT, &objicmpOutErrorsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objicmpOutErrorsValue,
                     sizeof (objicmpOutErrorsValue));

  /* return the object value: icmpOutErrors */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objicmpOutErrorsValue,
                           sizeof (objicmpOutErrorsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseicmpStats_icmpOutDestUnreachs
*
* @purpose Get 'icmpOutDestUnreachs'
*
* @description The number of ICMP Destination Unreachable messages sent. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseicmpStats_icmpOutDestUnreachs (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objicmpOutDestUnreachsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbIcmpOutDestUnreachsGet (L7_UNIT_CURRENT, &objicmpOutDestUnreachsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objicmpOutDestUnreachsValue,
                     sizeof (objicmpOutDestUnreachsValue));

  /* return the object value: icmpOutDestUnreachs */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objicmpOutDestUnreachsValue,
                           sizeof (objicmpOutDestUnreachsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseicmpStats_icmpOutTimestamps
*
* @purpose Get 'icmpOutTimestamps'
*
* @description The number of ICMP Timestamp (request) messages sent. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseicmpStats_icmpOutTimestamps (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objicmpOutTimestampsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbIcmpOutTimestampsGet (L7_UNIT_CURRENT, &objicmpOutTimestampsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objicmpOutTimestampsValue,
                     sizeof (objicmpOutTimestampsValue));

  /* return the object value: icmpOutTimestamps */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objicmpOutTimestampsValue,
                           sizeof (objicmpOutTimestampsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseicmpStats_icmpOutAddrMasks
*
* @purpose Get 'icmpOutAddrMasks'
*
* @description The number of ICMP Address Mask Request messages sent. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseicmpStats_icmpOutAddrMasks (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objicmpOutAddrMasksValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbIcmpOutAddrMasksGet (L7_UNIT_CURRENT, &objicmpOutAddrMasksValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objicmpOutAddrMasksValue,
                     sizeof (objicmpOutAddrMasksValue));

  /* return the object value: icmpOutAddrMasks */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objicmpOutAddrMasksValue,
                           sizeof (objicmpOutAddrMasksValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseicmpStats_icmpInAddrMasks
*
* @purpose Get 'icmpInAddrMasks'
*
* @description The number of ICMP Address Mask Request messages received. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseicmpStats_icmpInAddrMasks (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objicmpInAddrMasksValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbIcmpInAddrMasksGet (L7_UNIT_CURRENT, &objicmpInAddrMasksValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objicmpInAddrMasksValue,
                     sizeof (objicmpInAddrMasksValue));

  /* return the object value: icmpInAddrMasks */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objicmpInAddrMasksValue,
                           sizeof (objicmpInAddrMasksValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseicmpStats_icmpOutMsgs
*
* @purpose Get 'icmpOutMsgs'
*
* @description The total number of ICMP messages which this entity attempted 
*              to send. Note that this counter includes all those counted by 
*              icmpOutErrors. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseicmpStats_icmpOutMsgs (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objicmpOutMsgsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIcmpOutMsgsGet (L7_UNIT_CURRENT, &objicmpOutMsgsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objicmpOutMsgsValue, sizeof (objicmpOutMsgsValue));

  /* return the object value: icmpOutMsgs */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objicmpOutMsgsValue,
                           sizeof (objicmpOutMsgsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseicmpStats_icmpInEchos
*
* @purpose Get 'icmpInEchos'
*
* @description The number of ICMP Echo (request) messages received. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseicmpStats_icmpInEchos (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objicmpInEchosValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIcmpInEchosGet (L7_UNIT_CURRENT, &objicmpInEchosValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objicmpInEchosValue, sizeof (objicmpInEchosValue));

  /* return the object value: icmpInEchos */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objicmpInEchosValue,
                           sizeof (objicmpInEchosValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseicmpStats_icmpInMsgs
*
* @purpose Get 'icmpInMsgs'
*
* @description The total number of ICMP messages which the entity received. Note 
*              that this counter includes all those counted by icmpInErrors. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseicmpStats_icmpInMsgs (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objicmpInMsgsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIcmpInMsgsGet (L7_UNIT_CURRENT, &objicmpInMsgsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objicmpInMsgsValue, sizeof (objicmpInMsgsValue));

  /* return the object value: icmpInMsgs */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objicmpInMsgsValue,
                           sizeof (objicmpInMsgsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseicmpStats_icmpInTimeExcds
*
* @purpose Get 'icmpInTimeExcds'
*
* @description The number of ICMP Time Exceeded messages received. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseicmpStats_icmpInTimeExcds (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objicmpInTimeExcdsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbIcmpInTimeExcdsGet (L7_UNIT_CURRENT, &objicmpInTimeExcdsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objicmpInTimeExcdsValue,
                     sizeof (objicmpInTimeExcdsValue));

  /* return the object value: icmpInTimeExcds */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objicmpInTimeExcdsValue,
                           sizeof (objicmpInTimeExcdsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseicmpStats_icmpInTimestampReps
*
* @purpose Get 'icmpInTimestampReps'
*
* @description The number of ICMP Timestamp Reply messages received. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseicmpStats_icmpInTimestampReps (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objicmpInTimestampRepsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbIcmpInTimestampRepsGet (L7_UNIT_CURRENT, &objicmpInTimestampRepsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objicmpInTimestampRepsValue,
                     sizeof (objicmpInTimestampRepsValue));

  /* return the object value: icmpInTimestampReps */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objicmpInTimestampRepsValue,
                           sizeof (objicmpInTimestampRepsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseicmpStats_icmpOutSrcQuenchs
*
* @purpose Get 'icmpOutSrcQuenchs'
*
* @description The number of ICMP Source Quench messages sent. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseicmpStats_icmpOutSrcQuenchs (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objicmpOutSrcQuenchsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbIcmpOutSrcQuenchsGet (L7_UNIT_CURRENT, &objicmpOutSrcQuenchsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objicmpOutSrcQuenchsValue,
                     sizeof (objicmpOutSrcQuenchsValue));

  /* return the object value: icmpOutSrcQuenchs */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objicmpOutSrcQuenchsValue,
                           sizeof (objicmpOutSrcQuenchsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseicmpStats_icmpInTimestamps
*
* @purpose Get 'icmpInTimestamps'
*
* @description The number of ICMP Timestamp (request) messages received. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseicmpStats_icmpInTimestamps (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objicmpInTimestampsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbIcmpInTimestampsGet (L7_UNIT_CURRENT, &objicmpInTimestampsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objicmpInTimestampsValue,
                     sizeof (objicmpInTimestampsValue));

  /* return the object value: icmpInTimestamps */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objicmpInTimestampsValue,
                           sizeof (objicmpInTimestampsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseicmpStats_icmpInDestUnreachs
*
* @purpose Get 'icmpInDestUnreachs'
*
* @description The number of ICMP Destination Unreachable messages received. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseicmpStats_icmpInDestUnreachs (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objicmpInDestUnreachsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbIcmpInDestUnreachsGet (L7_UNIT_CURRENT, &objicmpInDestUnreachsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objicmpInDestUnreachsValue,
                     sizeof (objicmpInDestUnreachsValue));

  /* return the object value: icmpInDestUnreachs */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objicmpInDestUnreachsValue,
                           sizeof (objicmpInDestUnreachsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseicmpStats_icmpInAddrMaskReps
*
* @purpose Get 'icmpInAddrMaskReps'
*
* @description The number of ICMP Address Mask Reply messages received. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseicmpStats_icmpInAddrMaskReps (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objicmpInAddrMaskRepsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbIcmpInAddrMaskRepsGet (L7_UNIT_CURRENT, &objicmpInAddrMaskRepsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objicmpInAddrMaskRepsValue,
                     sizeof (objicmpInAddrMaskRepsValue));

  /* return the object value: icmpInAddrMaskReps */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objicmpInAddrMaskRepsValue,
                           sizeof (objicmpInAddrMaskRepsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseicmpStats_icmpInSrcQuenchs
*
* @purpose Get 'icmpInSrcQuenchs'
*
* @description The number of ICMP Source Quench messages received. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseicmpStats_icmpInSrcQuenchs (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objicmpInSrcQuenchsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbIcmpInSrcQuenchsGet (L7_UNIT_CURRENT, &objicmpInSrcQuenchsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objicmpInSrcQuenchsValue,
                     sizeof (objicmpInSrcQuenchsValue));

  /* return the object value: icmpInSrcQuenchs */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objicmpInSrcQuenchsValue,
                           sizeof (objicmpInSrcQuenchsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseicmpStats_icmpOutEchoReps
*
* @purpose Get 'icmpOutEchoReps'
*
* @description The number of ICMP Echo Reply messages sent. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseicmpStats_icmpOutEchoReps (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objicmpOutEchoRepsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbIcmpOutEchoRepsGet (L7_UNIT_CURRENT, &objicmpOutEchoRepsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objicmpOutEchoRepsValue,
                     sizeof (objicmpOutEchoRepsValue));

  /* return the object value: icmpOutEchoReps */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objicmpOutEchoRepsValue,
                           sizeof (objicmpOutEchoRepsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseicmpStats_icmpOutAddrMaskReps
*
* @purpose Get 'icmpOutAddrMaskReps'
*
* @description The number of ICMP Address Mask Reply messages sent. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseicmpStats_icmpOutAddrMaskReps (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objicmpOutAddrMaskRepsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbIcmpOutAddrMaskRepsGet (L7_UNIT_CURRENT, &objicmpOutAddrMaskRepsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objicmpOutAddrMaskRepsValue,
                     sizeof (objicmpOutAddrMaskRepsValue));

  /* return the object value: icmpOutAddrMaskReps */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objicmpOutAddrMaskRepsValue,
                           sizeof (objicmpOutAddrMaskRepsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseicmpStats_icmpOutTimeExcds
*
* @purpose Get 'icmpOutTimeExcds'
*
* @description The number of ICMP Time Exceeded messages sent. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseicmpStats_icmpOutTimeExcds (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objicmpOutTimeExcdsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbIcmpOutTimeExcdsGet (L7_UNIT_CURRENT, &objicmpOutTimeExcdsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objicmpOutTimeExcdsValue,
                     sizeof (objicmpOutTimeExcdsValue));

  /* return the object value: icmpOutTimeExcds */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objicmpOutTimeExcdsValue,
                           sizeof (objicmpOutTimeExcdsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
