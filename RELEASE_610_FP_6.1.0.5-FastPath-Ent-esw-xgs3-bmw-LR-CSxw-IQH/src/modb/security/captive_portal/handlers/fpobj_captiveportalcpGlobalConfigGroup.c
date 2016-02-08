
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_captiveportalcpGlobalConfigGroup.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to captiveportal-object.xml
*
* @create  06 July 2008, Sunday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_captiveportalcpGlobalConfigGroup_obj.h"

#include "usmdb_cpdm_api.h"
#include "usmdb_cpdm_connstatus_api.h"
#include "sslt_exports.h"
#include "usmdb_sslt_api.h"

/*******************************************************************************
* @function fpObjGet_captiveportalcpGlobalConfigGroup_cpAdminMode
*
* @purpose Get 'cpAdminMode'
 *@description  [cpAdminMode] Enable or disable the captive portal feature on
* the system.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpGlobalConfigGroup_cpAdminMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpAdminModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbCpdmGlobalModeGet (&objcpAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpAdminModeValue, sizeof (objcpAdminModeValue));

  /* return the object value: cpAdminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpAdminModeValue, sizeof (objcpAdminModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpGlobalConfigGroup_cpAdminMode
*
* @purpose Set 'cpAdminMode'
 *@description  [cpAdminMode] Enable or disable the captive portal feature on
* the system.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpGlobalConfigGroup_cpAdminMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpAdminModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpAdminMode */
  owa.len = sizeof (objcpAdminModeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcpAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpAdminModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbCpdmGlobalModeSet (objcpAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_captiveportalcpGlobalConfigGroup_cpAdditionalHttpPort
*
* @purpose Get 'cpAdditionalHttpPort'
 *@description  [cpAdditionalHttpPort] The additional captive portal
* authentication server port for HTTP web connection. The HTTP port default is
* 0 which denotes no specific port and the default port (80) is
* used.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpGlobalConfigGroup_cpAdditionalHttpPort (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpAdditionalHttpPortValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbCpdmGlobalHTTPPortGet (&objcpAdditionalHttpPortValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpAdditionalHttpPortValue, sizeof (objcpAdditionalHttpPortValue));

  /* return the object value: cpAdditionalHttpPort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpAdditionalHttpPortValue,
                           sizeof (objcpAdditionalHttpPortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpGlobalConfigGroup_cpAdditionalHttpPort
*
* @purpose Set 'cpAdditionalHttpPort'
 *@description  [cpAdditionalHttpPort] The additional captive portal
* authentication server port for HTTP web connection. The HTTP port default is
* 0 which denotes no specific port and the default port (80) is
* used.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpGlobalConfigGroup_cpAdditionalHttpPort (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpAdditionalHttpPortValue;

#ifdef L7_MGMT_SECURITY_PACKAGE
  xLibU32_t port;
  xLibU32_t defaultSecurePort;
  xLibU32_t additionalSecurePort;
  xLibU32_t adminSecurePort;
#endif

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpAdditionalHttpPort */
  owa.len = sizeof (objcpAdditionalHttpPortValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcpAdditionalHttpPortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpAdditionalHttpPortValue, owa.len);

#ifdef L7_MGMT_SECURITY_PACKAGE
  /* get the all configured ports from application */
  if (L7_SUCCESS != usmDbCpdmGlobalHTTPPortGet (&port))
  {
    port = CP_HTTP_PORT_MIN;              
  }
  if (L7_SUCCESS != usmDbCpdmGlobalHTTPSecurePort1Get(&defaultSecurePort))
  {
    defaultSecurePort = L7_SSLT_SECURE_PORT;
  }
  if (L7_SUCCESS != usmDbCpdmGlobalHTTPSecurePort2Get(&additionalSecurePort))
  {
    additionalSecurePort = FD_CP_HTTP_AUX_SECURE_PORT2;
  }
  if (L7_SUCCESS != usmDbssltSecurePortGet(L7_UNIT_CURRENT,&adminSecurePort))
  {
    adminSecurePort = FD_SSLT_SECURE_PORT;
  }

  /* Check for the validity of the port usage */
  if ((objcpAdditionalHttpPortValue != 0) &&
      (objcpAdditionalHttpPortValue != port) &&
      ((objcpAdditionalHttpPortValue == CP_STANDARD_HTTP_PORT)
      || (objcpAdditionalHttpPortValue == defaultSecurePort)
      || (objcpAdditionalHttpPortValue == additionalSecurePort)
      || (objcpAdditionalHttpPortValue == adminSecurePort)
      ))
  {
    owa.rc = XLIBRC_CP_FAILED_TO_ADD_HTTP_PORT;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;              
  }

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbCpdmGlobalHTTPPortSet (objcpAdditionalHttpPortValue);
#else
  owa.l7rc = L7_SUCCESS;
#endif

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_captiveportalcpGlobalConfigGroup_cpAdditionalSecureHttpPort
*
* @purpose Get 'cpAdditionalSecureHttpPort'
 *@description  [cpAdditionalSecureHttpPort] The additional captive portal
* authentication server port for Secure HTTP web connection. The Secure HTTP port default is
* 0 which denotes no specific port and the default port (443) is
* used.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpGlobalConfigGroup_cpAdditionalSecureHttpPort (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpAdditionalSecureHttpPortValue;

  FPOBJ_TRACE_ENTER (bufp);
#ifdef L7_MGMT_SECURITY_PACKAGE
  /* get the value from application */
  owa.l7rc = usmDbCpdmGlobalHTTPSecurePort2Get (&objcpAdditionalSecureHttpPortValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
#else
  objcpAdditionalSecureHttpPortValue = 0;
#endif
  FPOBJ_TRACE_VALUE (bufp, &objcpAdditionalSecureHttpPortValue, sizeof (objcpAdditionalSecureHttpPortValue));

  /* return the object value: cpAdditionalHttpPort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpAdditionalSecureHttpPortValue,
                           sizeof (objcpAdditionalSecureHttpPortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpGlobalConfigGroup_cpAdditionalSecureHttpPort
*
* @purpose Set 'cpAdditionalSecureHttpPort'
 *@description  [cpAdditionalSecureHttpPort] The additional captive portal
* authentication server port for Secure HTTP web connection. The Secure HTTP port default is
* 0 which denotes no specific port and the default port (443) is
* used.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpGlobalConfigGroup_cpAdditionalSecureHttpPort (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpAdditionalSecureHttpPortValue;

#ifdef L7_MGMT_SECURITY_PACKAGE
  xLibU32_t port;
  xLibU32_t defaultSecurePort;
  xLibU32_t additionalSecurePort;
  xLibU32_t adminSecurePort;
#endif

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpAdditionalSecureHttpPort */
  owa.len = sizeof (objcpAdditionalSecureHttpPortValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcpAdditionalSecureHttpPortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpAdditionalSecureHttpPortValue, owa.len);

#ifdef L7_MGMT_SECURITY_PACKAGE
  /* get the all configured ports from application */
  if (L7_SUCCESS != usmDbCpdmGlobalHTTPPortGet (&port))
  {
    port = CP_HTTP_PORT_MIN;              
  }
  if (L7_SUCCESS != usmDbCpdmGlobalHTTPSecurePort1Get(&defaultSecurePort))
  {
    defaultSecurePort = L7_SSLT_SECURE_PORT;
  }
  if (L7_SUCCESS != usmDbCpdmGlobalHTTPSecurePort2Get(&additionalSecurePort))
  {
    additionalSecurePort = FD_CP_HTTP_AUX_SECURE_PORT2;
  }
  if (L7_SUCCESS != usmDbssltSecurePortGet(L7_UNIT_CURRENT,&adminSecurePort))
  {
    adminSecurePort = FD_SSLT_SECURE_PORT;
  }

  /* Check for the validity of the port usage */
  if ((objcpAdditionalSecureHttpPortValue != 0) &&
      (objcpAdditionalSecureHttpPortValue != additionalSecurePort) &&
      ((objcpAdditionalSecureHttpPortValue == CP_STANDARD_HTTP_PORT)
      || (objcpAdditionalSecureHttpPortValue == defaultSecurePort)
      || (objcpAdditionalSecureHttpPortValue == port)
      || (objcpAdditionalSecureHttpPortValue == adminSecurePort)
      ))
  {
    owa.rc = XLIBRC_CP_FAILED_TO_ADD_HTTPS_PORT;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;              
  }

  /* set the value in application */
  owa.l7rc = usmDbCpdmGlobalHTTPSecurePort2Set (objcpAdditionalSecureHttpPortValue);
#else
   owa.l7rc = L7_SUCCESS;
#endif
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_captiveportalcpGlobalConfigGroup_cpPeerStatsReportingInterval
*
* @purpose Get 'cpPeerStatsReportingInterval'
 *@description  [cpPeerStatsReportingInterval] The time interval between
* statistics reports to the cluster controller.A value of 0 indicates
* reporting is disabled.The value is represented in seconds.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpGlobalConfigGroup_cpPeerStatsReportingInterval (void *wap,
                                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpPeerStatsReportingIntervalValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* Need to check about this clustering support */
#if 0
  if (usmDbCpdmClusterSupportGet() == L7_SUCCESS)
  {
    /* get the value from application */
    owa.l7rc = usmDbCpdmGlobalStatusPeerSwStatsReportIntervalGet (&objcpPeerStatsReportingIntervalValue);
  }
#endif

  owa.l7rc = usmDbCpdmGlobalStatusPeerSwStatsReportIntervalGet (&objcpPeerStatsReportingIntervalValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpPeerStatsReportingIntervalValue,
                     sizeof (objcpPeerStatsReportingIntervalValue));

  /* return the object value: cpPeerStatsReportingInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpPeerStatsReportingIntervalValue,
                           sizeof (objcpPeerStatsReportingIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpGlobalConfigGroup_cpPeerStatsReportingInterval
*
* @purpose Set 'cpPeerStatsReportingInterval'
 *@description  [cpPeerStatsReportingInterval] The time interval between
* statistics reports to the cluster controller.A value of 0 indicates
* reporting is disabled.The value is represented in seconds.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpGlobalConfigGroup_cpPeerStatsReportingInterval (void *wap,
                                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpPeerStatsReportingIntervalValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpPeerStatsReportingInterval */
  owa.len = sizeof (objcpPeerStatsReportingIntervalValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcpPeerStatsReportingIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpPeerStatsReportingIntervalValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbCpdmGlobalStatusPeerSwStatsReportIntervalSet (objcpPeerStatsReportingIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_captiveportalcpGlobalConfigGroup_cpAuthTimeout
*
* @purpose Get 'cpAuthTimeout'
 *@description  [cpAuthTimeout] The length of time to maintain an authentication
* session from a client.This time is measured from when a client
* initiates an authentication request.The value is represented in
* seconds.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpGlobalConfigGroup_cpAuthTimeout (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpAuthTimeoutValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbCpdmGlobalStatusSessionTimeoutGet (&objcpAuthTimeoutValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpAuthTimeoutValue, sizeof (objcpAuthTimeoutValue));

  /* return the object value: cpAuthTimeout */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpAuthTimeoutValue,
                           sizeof (objcpAuthTimeoutValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpGlobalConfigGroup_cpAuthTimeout
*
* @purpose Set 'cpAuthTimeout'
 *@description  [cpAuthTimeout] The length of time to maintain an authentication
* session from a client.This time is measured from when a client
* initiates an authentication request.The value is represented in
* seconds.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpGlobalConfigGroup_cpAuthTimeout (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpAuthTimeoutValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpAuthTimeout */
  owa.len = sizeof (objcpAuthTimeoutValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcpAuthTimeoutValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpAuthTimeoutValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbCpdmGlobalStatusSessionTimeoutSet (objcpAuthTimeoutValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
