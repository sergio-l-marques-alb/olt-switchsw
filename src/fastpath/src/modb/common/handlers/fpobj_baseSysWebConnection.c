/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseSysWebConnection.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to sntp-object.xml
*
* @create  6 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseSysWebConnection_obj.h"
#include "usmdb_sim_api.h"

/*******************************************************************************
* @function fpObjGet_baseSysWebConnection_sysWebMgmtMode
*
* @purpose Get 'sysWebMgmtMode'
*
* @description Enable/Disable Web Managemnt mode of the switch 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSysWebConnection_sysWebMgmtMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysWebMgmtModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbSwDevCtrlWebMgmtModeGet (L7_UNIT_CURRENT, &objsysWebMgmtModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysWebMgmtModeValue,
                     sizeof (objsysWebMgmtModeValue));

  /* return the object value: sysWebMgmtMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objsysWebMgmtModeValue,
                           sizeof (objsysWebMgmtModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSysWebConnection_sysWebMgmtMode
*
* @purpose Set 'sysWebMgmtMode'
*
* @description Enable/Disable Web Managemnt mode of the switch 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysWebConnection_sysWebMgmtMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysWebMgmtModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysWebMgmtMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objsysWebMgmtModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysWebMgmtModeValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbSwDevCtrlWebMgmtModeSet (L7_UNIT_CURRENT, objsysWebMgmtModeValue);
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
* @function fpObjGet_baseSysWebConnection_sysWebJavaMode
*
* @purpose Get 'sysWebJavaMode'
*
* @description Enable/Disable Web java mode of the switch 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSysWebConnection_sysWebJavaMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysWebJavaModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbWebJavaModeGet (L7_UNIT_CURRENT, &objsysWebJavaModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysWebJavaModeValue,
                     sizeof (objsysWebJavaModeValue));

  /* return the object value: sysWebJavaMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objsysWebJavaModeValue,
                           sizeof (objsysWebJavaModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSysWebConnection_sysWebJavaMode
*
* @purpose Set 'sysWebJavaMode'
*
* @description Enable/Disable Web java mode of the switch 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysWebConnection_sysWebJavaMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysWebJavaModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysWebJavaMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objsysWebJavaModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysWebJavaModeValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbWebJavaModeSet (L7_UNIT_CURRENT, objsysWebJavaModeValue);
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
* @function fpObjGet_baseSysWebConnection_sysCliWebHttpNumSessions
*
* @purpose Get 'sysCliWebHttpNumSessions'
*
* @description maximum number of web sessions 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSysWebConnection_sysCliWebHttpNumSessions (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysCliWebHttpNumSessionsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbCliWebHttpNumSessionsGet ( &objsysCliWebHttpNumSessionsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysCliWebHttpNumSessionsValue,
                     sizeof (objsysCliWebHttpNumSessionsValue));

  /* return the object value: sysCliWebHttpNumSessions */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objsysCliWebHttpNumSessionsValue,
                    sizeof (objsysCliWebHttpNumSessionsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSysWebConnection_sysCliWebHttpNumSessions
*
* @purpose Set 'sysCliWebHttpNumSessions'
*
* @description maximum number of web sessions 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysWebConnection_sysCliWebHttpNumSessions (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysCliWebHttpNumSessionsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysCliWebHttpNumSessions */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objsysCliWebHttpNumSessionsValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysCliWebHttpNumSessionsValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbCliWebHttpNumSessionsSet (objsysCliWebHttpNumSessionsValue);
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
* @function fpObjGet_baseSysWebConnection_sysCliWebHttpSessionHardTimeOut
*
* @purpose Get 'sysCliWebHttpSessionHardTimeOut'
*
* @description http session hard timeout (in hours) 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSysWebConnection_sysCliWebHttpSessionHardTimeOut (void
                                                                        *wap,
                                                                        void
                                                                        *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysCliWebHttpSessionHardTimeOutValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbCliWebHttpSessionHardTimeOutGet (&objsysCliWebHttpSessionHardTimeOutValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysCliWebHttpSessionHardTimeOutValue,
                     sizeof (objsysCliWebHttpSessionHardTimeOutValue));

  /* return the object value: sysCliWebHttpSessionHardTimeOut */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objsysCliWebHttpSessionHardTimeOutValue,
                    sizeof (objsysCliWebHttpSessionHardTimeOutValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSysWebConnection_sysCliWebHttpSessionHardTimeOut
*
* @purpose Set 'sysCliWebHttpSessionHardTimeOut'
*
* @description http session hard timeout (in hours) 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysWebConnection_sysCliWebHttpSessionHardTimeOut (void
                                                                        *wap,
                                                                        void
                                                                        *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysCliWebHttpSessionHardTimeOutValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysCliWebHttpSessionHardTimeOut */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) &
                           objsysCliWebHttpSessionHardTimeOutValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysCliWebHttpSessionHardTimeOutValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbCliWebHttpSessionHardTimeOutSet (objsysCliWebHttpSessionHardTimeOutValue);
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
* @function fpObjGet_baseSysWebConnection_sysCliWebHttpSessionSoftTimeOut
*
* @purpose Get 'sysCliWebHttpSessionSoftTimeOut'
*
* @description http session soft timeout (iin minutes) 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSysWebConnection_sysCliWebHttpSessionSoftTimeOut (void
                                                                        *wap,
                                                                        void
                                                                        *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysCliWebHttpSessionSoftTimeOutValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbCliWebHttpSessionSoftTimeOutGet (&objsysCliWebHttpSessionSoftTimeOutValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysCliWebHttpSessionSoftTimeOutValue,
                     sizeof (objsysCliWebHttpSessionSoftTimeOutValue));

  /* return the object value: sysCliWebHttpSessionSoftTimeOut */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objsysCliWebHttpSessionSoftTimeOutValue,
                    sizeof (objsysCliWebHttpSessionSoftTimeOutValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseSysWebConnection_sysCliWebHttpSessionSoftTimeOut
*
* @purpose Set 'sysCliWebHttpSessionSoftTimeOut'
*
* @description http session soft timeout (iin minutes) 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysWebConnection_sysCliWebHttpSessionSoftTimeOut (void
                                                                        *wap,
                                                                        void
                                                                        *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysCliWebHttpSessionSoftTimeOutValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysCliWebHttpSessionSoftTimeOut */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) &
                           objsysCliWebHttpSessionSoftTimeOutValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysCliWebHttpSessionSoftTimeOutValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbCliWebHttpSessionSoftTimeOutSet (objsysCliWebHttpSessionSoftTimeOutValue);
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
* @function fpObjGet_baseSysWebConnection_sysWebHTTPPortNumber
*
* @purpose Get 'sysWebHTTPPortNumber'
 *@description  [sysWebHTTPPortNumber] <HTML>Get Port-Number of Access for HTTP   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSysWebConnection_sysWebHTTPPortNumber (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objsysWebHTTPPortNumberValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbSwDevCtrlWebMgmtPortNumGet(L7_UNIT_CURRENT, &objsysWebHTTPPortNumberValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objsysWebHTTPPortNumberValue, sizeof (objsysWebHTTPPortNumberValue));

  /* return the object value: sysWebHTTPPortNumber */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objsysWebHTTPPortNumberValue,
                           sizeof (objsysWebHTTPPortNumberValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSysWebConnection_sysWebHTTPPortNumber
*
* @purpose Set 'sysWebHTTPPortNumber'
 *@description  [sysWebHTTPPortNumber] <HTML>Get Port-Number of Access for HTTP   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysWebConnection_sysWebHTTPPortNumber (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objsysWebHTTPPortNumberValue;
  xLibU32_t objsysWebHTTPPortFlagValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysWebHTTPPortNumber */
  owa.len = sizeof (objsysWebHTTPPortNumberValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objsysWebHTTPPortNumberValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysWebHTTPPortNumberValue, owa.len);

  /* retrieve key: sysWebHTTPPortNumber */
  owa.len = sizeof (objsysWebHTTPPortFlagValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSysWebConnection_sysWebHTTPPortFlag,
                          (xLibU8_t *) & objsysWebHTTPPortFlagValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbSwDevCtrlWebMgmtPortNumSet (L7_UNIT_CURRENT, objsysWebHTTPPortNumberValue,
                                         objsysWebHTTPPortFlagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseSysWebConnection_sysWebHTTPPortFlag
*
* @purpose Set 'sysWebHTTPPortFlag'
 *@description  [sysWebHTTPPortFlag] <HTML>Set the flag port-number for HTTP
* Access   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysWebConnection_sysWebHTTPPortFlag (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objsysWebHTTPPortFlagValue;

  xLibU32_t keysysWebHTTPPortNumberValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysWebHTTPPortFlag */
  owa.len = sizeof (objsysWebHTTPPortFlagValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objsysWebHTTPPortFlagValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysWebHTTPPortFlagValue, owa.len);

  /* retrieve key: sysWebHTTPPortNumber */
  owa.len = sizeof (keysysWebHTTPPortNumberValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSysWebConnection_sysWebHTTPPortNumber,
                          (xLibU8_t *) & keysysWebHTTPPortNumberValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keysysWebHTTPPortNumberValue, owa.len);

  return XLIBRC_SUCCESS;
}
