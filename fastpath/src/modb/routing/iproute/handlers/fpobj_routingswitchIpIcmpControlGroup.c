
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_routingswitchIpIcmpControlGroup.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to routing-object.xml
*
* @create  02 June 2008, Monday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_routingswitchIpIcmpControlGroup_obj.h"
#include "usmdb_ip_api.h"

/*******************************************************************************
* @function fpObjGet_routingswitchIpIcmpControlGroup_switchIpIcmpEchoReplyMode
*
* @purpose Get 'switchIpIcmpEchoReplyMode'
 *@description  [switchIpIcmpEchoReplyMode] Sets the administrative Echo Reply
* Mode.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchIpIcmpControlGroup_switchIpIcmpEchoReplyMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objswitchIpIcmpEchoReplyModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpMapRtrICMPEchoReplyModeGet (&objswitchIpIcmpEchoReplyModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objswitchIpIcmpEchoReplyModeValue,
                     sizeof (objswitchIpIcmpEchoReplyModeValue));

  /* return the object value: switchIpIcmpEchoReplyMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objswitchIpIcmpEchoReplyModeValue,
                           sizeof (objswitchIpIcmpEchoReplyModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingswitchIpIcmpControlGroup_switchIpIcmpEchoReplyMode
*
* @purpose Set 'switchIpIcmpEchoReplyMode'
 *@description  [switchIpIcmpEchoReplyMode] Sets the administrative Echo Reply
* Mode.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingswitchIpIcmpControlGroup_switchIpIcmpEchoReplyMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objswitchIpIcmpEchoReplyModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: switchIpIcmpEchoReplyMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objswitchIpIcmpEchoReplyModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objswitchIpIcmpEchoReplyModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbIpMapRtrICMPEchoReplyModeSet (objswitchIpIcmpEchoReplyModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingswitchIpIcmpControlGroup_switchIpIcmpRedirectsMode
*
* @purpose Get 'switchIpIcmpRedirectsMode'
 *@description  [switchIpIcmpRedirectsMode] Sets the administrative Mode for
* sending Redirects.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchIpIcmpControlGroup_switchIpIcmpRedirectsMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objswitchIpIcmpRedirectsModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpMapRtrICMPRedirectsModeGet (&objswitchIpIcmpRedirectsModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objswitchIpIcmpRedirectsModeValue,
                     sizeof (objswitchIpIcmpRedirectsModeValue));

  /* return the object value: switchIpIcmpRedirectsMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objswitchIpIcmpRedirectsModeValue,
                           sizeof (objswitchIpIcmpRedirectsModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingswitchIpIcmpControlGroup_switchIpIcmpRedirectsMode
*
* @purpose Set 'switchIpIcmpRedirectsMode'
 *@description  [switchIpIcmpRedirectsMode] Sets the administrative Mode for
* sending Redirects.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingswitchIpIcmpControlGroup_switchIpIcmpRedirectsMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objswitchIpIcmpRedirectsModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: switchIpIcmpRedirectsMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objswitchIpIcmpRedirectsModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objswitchIpIcmpRedirectsModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbIpMapRtrICMPRedirectsModeSet (objswitchIpIcmpRedirectsModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingswitchIpIcmpControlGroup_switchIpIcmpRateLimitInterval
*
* @purpose Get 'switchIpIcmpRateLimitInterval'
 *@description  [switchIpIcmpRateLimitInterval] Specifies the time interval
* between tokens being placed in the bucket for ICMP Ratelimit.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchIpIcmpControlGroup_switchIpIcmpRateLimitInterval (void *wap,
                                                                                 void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objswitchIpIcmpRateLimitIntervalValue;
  L7_uint32 burstsize;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpMapRtrICMPRatelimitGet (&burstsize, &objswitchIpIcmpRateLimitIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objswitchIpIcmpRateLimitIntervalValue,
                     sizeof (objswitchIpIcmpRateLimitIntervalValue));

  /* return the object value: switchIpIcmpRateLimitInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objswitchIpIcmpRateLimitIntervalValue,
                           sizeof (objswitchIpIcmpRateLimitIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingswitchIpIcmpControlGroup_switchIpIcmpRateLimitInterval
*
* @purpose Set 'switchIpIcmpRateLimitInterval'
 *@description  [switchIpIcmpRateLimitInterval] Specifies the time interval
* between tokens being placed in the bucket for ICMP Ratelimit.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingswitchIpIcmpControlGroup_switchIpIcmpRateLimitInterval (void *wap,
                                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objswitchIpIcmpRateLimitIntervalValue;
  xLibU32_t tempswitchIpIcmpRateLimitIntervalValue;
  L7_uint32 burstsize;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: switchIpIcmpRateLimitInterval */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objswitchIpIcmpRateLimitIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objswitchIpIcmpRateLimitIntervalValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  if (usmDbIpMapRtrICMPRatelimitGet (&burstsize, &tempswitchIpIcmpRateLimitIntervalValue) == L7_SUCCESS)
  {
     if (tempswitchIpIcmpRateLimitIntervalValue != objswitchIpIcmpRateLimitIntervalValue)
     {
       owa.l7rc = usmDbIpMapRtrICMPRatelimitSet (burstsize, objswitchIpIcmpRateLimitIntervalValue);
     }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingswitchIpIcmpControlGroup_switchIpIcmpRateLimitBurstSize
*
* @purpose Get 'switchIpIcmpRateLimitBurstSize'
 *@description  [switchIpIcmpRateLimitBurstSize] Specifies the number of tokens
* to be placed after timeout.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchIpIcmpControlGroup_switchIpIcmpRateLimitBurstSize (void *wap,
                                                                                  void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objswitchIpIcmpRateLimitBurstSizeValue;
  L7_uint32 interval;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpMapRtrICMPRatelimitGet (&objswitchIpIcmpRateLimitBurstSizeValue, &interval);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objswitchIpIcmpRateLimitBurstSizeValue,
                     sizeof (objswitchIpIcmpRateLimitBurstSizeValue));

  /* return the object value: switchIpIcmpRateLimitBurstSize */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objswitchIpIcmpRateLimitBurstSizeValue,
                           sizeof (objswitchIpIcmpRateLimitBurstSizeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingswitchIpIcmpControlGroup_switchIpIcmpRateLimitBurstSize
*
* @purpose Set 'switchIpIcmpRateLimitBurstSize'
 *@description  [switchIpIcmpRateLimitBurstSize] Specifies the number of tokens
* to be placed after timeout.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingswitchIpIcmpControlGroup_switchIpIcmpRateLimitBurstSize (void *wap,
                                                                                  void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objswitchIpIcmpRateLimitBurstSizeValue;
  xLibU32_t tempswitchIpIcmpRateLimitBurstSizeValue;
  L7_uint32 interval;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: switchIpIcmpRateLimitBurstSize */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objswitchIpIcmpRateLimitBurstSizeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objswitchIpIcmpRateLimitBurstSizeValue, owa.len);

  /* set the value in application */
  if (usmDbIpMapRtrICMPRatelimitGet (&tempswitchIpIcmpRateLimitBurstSizeValue, &interval) == L7_SUCCESS)
  {
     if (tempswitchIpIcmpRateLimitBurstSizeValue != objswitchIpIcmpRateLimitBurstSizeValue)
     {
       owa.l7rc = usmDbIpMapRtrICMPRatelimitSet (objswitchIpIcmpRateLimitBurstSizeValue, interval);
     }
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
