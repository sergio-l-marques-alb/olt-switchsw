
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_captiveportalcpCaptivePortalGlobalStatusGroup.c
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
#include "_xe_captiveportalcpCaptivePortalGlobalStatusGroup_obj.h"

#include "usmdb_cpdm_api.h"
#include "usmdb_cpdm_user_api.h"

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalGlobalStatusGroup_cpCaptivePortalOperStatus
*
* @purpose Get 'cpCaptivePortalOperStatus'
 *@description  [cpCaptivePortalOperStatus] Indication of the current
* operational state of the captive portal feature.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalGlobalStatusGroup_cpCaptivePortalOperStatus (void
                                                                                           *wap,
                                                                                           void
                                                                                           *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalOperStatusValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbCpdmGlobalStatusModeGet (&objcpCaptivePortalOperStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalOperStatusValue,
                     sizeof (objcpCaptivePortalOperStatusValue));

  /* return the object value: cpCaptivePortalOperStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalOperStatusValue,
                           sizeof (objcpCaptivePortalOperStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalGlobalStatusGroup_cpCaptivePortalOperDisabledReason
*
* @purpose Get 'cpCaptivePortalOperDisabledReason'
 *@description  [cpCaptivePortalOperDisabledReason] The reason for the captive
* portal feature being operationally disabled.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalGlobalStatusGroup_cpCaptivePortalOperDisabledReason (void *wap,
                                                                                          void
                                                                                          *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalOperDisabledReasonValue;
  xLibU32_t objcpCaptivePortalOperStatusValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbCpdmGlobalStatusModeGet (&objcpCaptivePortalOperStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if ((objcpCaptivePortalOperStatusValue == L7_CP_MODE_DISABLED) || 
                  (objcpCaptivePortalOperStatusValue == L7_CP_MODE_DISABLE_PENDING))
  {
    /* get the value from application */
    owa.l7rc = usmDbCpdmGlobalCPDisableReasonGet (&objcpCaptivePortalOperDisabledReasonValue);
    if (owa.l7rc != L7_SUCCESS)
    {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }
  }
  else
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalOperDisabledReasonValue,
                     sizeof (objcpCaptivePortalOperDisabledReasonValue));

  /* return the object value: cpCaptivePortalOperDisabledReason */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalOperDisabledReasonValue,
                           sizeof (objcpCaptivePortalOperDisabledReasonValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalGlobalStatusGroup_cpCaptivePortalIpv4Address
*
* @purpose Get 'cpCaptivePortalIpv4Address'
 *@description  [cpCaptivePortalIpv4Address] The IP address of this captive
* portal.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalGlobalStatusGroup_cpCaptivePortalIpv4Address (void
                                                                                            *wap,
                                                                                            void
                                                                                            *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibIpV4_t objcpCaptivePortalIpv4AddressValue;

  xLibU32_t objcpAdminModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  if ((usmDbCpdmGlobalModeGet(&objcpAdminModeValue) == L7_SUCCESS) && (objcpAdminModeValue == L7_ENABLE))
  {
    /* get the value from application */
    owa.l7rc = usmDbCpdmGlobalCPIPAddressGet (&objcpCaptivePortalIpv4AddressValue);
  }
  else
    objcpCaptivePortalIpv4AddressValue = 0;

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalIpv4AddressValue,
                     sizeof (objcpCaptivePortalIpv4AddressValue));

  /* return the object value: cpCaptivePortalIpv4Address */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalIpv4AddressValue,
                           sizeof (objcpCaptivePortalIpv4AddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalGlobalStatusGroup_cpCaptivePortalInstanceMaxCount
*
* @purpose Get 'cpCaptivePortalInstanceMaxCount'
 *@description  [cpCaptivePortalInstanceMaxCount] The maximum number of captive
* portal instances supported by the system.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalGlobalStatusGroup_cpCaptivePortalInstanceMaxCount (void *wap,
                                                                                        void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalInstanceMaxCountValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbCpdmGlobalSupportedCPInstancesGet (&objcpCaptivePortalInstanceMaxCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalInstanceMaxCountValue,
                     sizeof (objcpCaptivePortalInstanceMaxCountValue));

  /* return the object value: cpCaptivePortalInstanceMaxCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalInstanceMaxCountValue,
                           sizeof (objcpCaptivePortalInstanceMaxCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalGlobalStatusGroup_cpCaptivePortalInstanceConfiguredCount
*
* @purpose Get 'cpCaptivePortalInstanceConfiguredCount'
 *@description  [cpCaptivePortalInstanceConfiguredCount] The number of captive
* portal instances currently configured in the system.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalGlobalStatusGroup_cpCaptivePortalInstanceConfiguredCount (void
                                                                                               *wap,
                                                                                               void
                                                                                               *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalInstanceConfiguredCountValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbCpdmGlobalConfiguredCPInstancesGet (&objcpCaptivePortalInstanceConfiguredCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalInstanceConfiguredCountValue,
                     sizeof (objcpCaptivePortalInstanceConfiguredCountValue));

  /* return the object value: cpCaptivePortalInstanceConfiguredCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalInstanceConfiguredCountValue,
                           sizeof (objcpCaptivePortalInstanceConfiguredCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalGlobalStatusGroup_cpCaptivePortalInstanceActiveCount
*
* @purpose Get 'cpCaptivePortalInstanceActiveCount'
 *@description  [cpCaptivePortalInstanceActiveCount] The number of captive
* portal instances currently active in the system.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalGlobalStatusGroup_cpCaptivePortalInstanceActiveCount (void
                                                                                           *wap,
                                                                                           void
                                                                                           *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalInstanceActiveCountValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbCpdmGlobalActiveCPInstancesGet (&objcpCaptivePortalInstanceActiveCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalInstanceActiveCountValue,
                     sizeof (objcpCaptivePortalInstanceActiveCountValue));

  /* return the object value: cpCaptivePortalInstanceActiveCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalInstanceActiveCountValue,
                           sizeof (objcpCaptivePortalInstanceActiveCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalGlobalStatusGroup_cpCaptivePortalAuthenUserMaxCount
*
* @purpose Get 'cpCaptivePortalAuthenUserMaxCount'
 *@description  [cpCaptivePortalAuthenUserMaxCount] The maximum number of
* authenticated users that the system can support.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalGlobalStatusGroup_cpCaptivePortalAuthenUserMaxCount (void *wap,
                                                                                          void
                                                                                          *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalAuthenUserMaxCountValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbCpdmGlobalSupportedUsersGet (&objcpCaptivePortalAuthenUserMaxCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalAuthenUserMaxCountValue,
                     sizeof (objcpCaptivePortalAuthenUserMaxCountValue));

  /* return the object value: cpCaptivePortalAuthenUserMaxCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalAuthenUserMaxCountValue,
                           sizeof (objcpCaptivePortalAuthenUserMaxCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalGlobalStatusGroup_cpCaptivePortalLocalUserMaxCount
*
* @purpose Get 'cpCaptivePortalLocalUserMaxCount'
 *@description  [cpCaptivePortalLocalUserMaxCount] The maximum number of local
* users that the system can support.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalGlobalStatusGroup_cpCaptivePortalLocalUserMaxCount (void *wap,
                                                                                         void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalLocalUserMaxCountValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbCpdmGlobalLocalUsersGet (&objcpCaptivePortalLocalUserMaxCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalLocalUserMaxCountValue,
                     sizeof (objcpCaptivePortalLocalUserMaxCountValue));

  /* return the object value: cpCaptivePortalLocalUserMaxCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalLocalUserMaxCountValue,
                           sizeof (objcpCaptivePortalLocalUserMaxCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalGlobalStatusGroup_cpCaptivePortalConfiguredLocalUserCount
*
* @purpose Get 'cpCaptivePortalConfiguredLocalUserCount'
 *@description  [cpCaptivePortalConfiguredLocalUserCount] The number of
* local users configured.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalGlobalStatusGroup_cpCaptivePortalConfiguredLocalUserCount (void *wap,
                                                                                          void
                                                                                          *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalConfiguredLocalUserCountValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbCpdmUserEntryCountGet (&objcpCaptivePortalConfiguredLocalUserCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalConfiguredLocalUserCountValue,
                     sizeof (objcpCaptivePortalConfiguredLocalUserCountValue));

  /* return the object value: cpCaptivePortalConfiguredLocalUserCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalConfiguredLocalUserCountValue,
                           sizeof (objcpCaptivePortalConfiguredLocalUserCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalGlobalStatusGroup_cpCaptivePortalAuthenUserCurrentCount
*
* @purpose Get 'cpCaptivePortalAuthenUserCurrentCount'
 *@description  [cpCaptivePortalAuthenUserCurrentCount] The number of currently
* authenticated users across all captive portal instances.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalGlobalStatusGroup_cpCaptivePortalAuthenUserCurrentCount (void
                                                                                              *wap,
                                                                                              void
                                                                                              *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalAuthenUserCurrentCountValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbCpdmGlobalAuthenticatedUsersGet (&objcpCaptivePortalAuthenUserCurrentCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalAuthenUserCurrentCountValue,
                     sizeof (objcpCaptivePortalAuthenUserCurrentCountValue));

  /* return the object value: cpCaptivePortalAuthenUserCurrentCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalAuthenUserCurrentCountValue,
                           sizeof (objcpCaptivePortalAuthenUserCurrentCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
