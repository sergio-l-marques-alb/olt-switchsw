
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_captiveportalcpCaptivePortalInstanceStatusTable.c
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
#include "_xe_captiveportalcpCaptivePortalInstanceStatusTable_obj.h"

#include "usmdb_cpdm_api.h"
#include "usmdb_cpdm_connstatus_api.h"

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalInstanceStatusTable_cpCaptivePortalInstanceId
*
* @purpose Get 'cpCaptivePortalInstanceId'
 *@description  [cpCaptivePortalInstanceId] The identifier associated with this
* instance of captive portal.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalInstanceStatusTable_cpCaptivePortalInstanceId (void
                                                                                             *wap,
                                                                                             void
                                                                                             *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalInstanceIdValue;
  xLibU32_t nextObjcpCaptivePortalInstanceIdValue;

  xLibU16_t cpInstanceIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (objcpCaptivePortalInstanceIdValue);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalInstanceStatusTable_cpCaptivePortalInstanceId,
                   (xLibU8_t *) & objcpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objcpCaptivePortalInstanceIdValue = 0;
    owa.l7rc = usmDbCpdmCPConfigNextGet (objcpCaptivePortalInstanceIdValue,
                                    &cpInstanceIdValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objcpCaptivePortalInstanceIdValue, owa.len);
    owa.l7rc = usmDbCpdmCPConfigNextGet (objcpCaptivePortalInstanceIdValue,
                                    &cpInstanceIdValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  nextObjcpCaptivePortalInstanceIdValue = (xLibU32_t)cpInstanceIdValue;

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjcpCaptivePortalInstanceIdValue, owa.len);

  /* return the object value: cpCaptivePortalInstanceId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjcpCaptivePortalInstanceIdValue,
                           sizeof (nextObjcpCaptivePortalInstanceIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalInstanceStatusTable_cpCaptivePortalInstanceOperStatus
*
* @purpose Get 'cpCaptivePortalInstanceOperStatus'
 *@description  [cpCaptivePortalInstanceOperStatus] Indication of the current
* operational state of the captive portal instance.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalInstanceStatusTable_cpCaptivePortalInstanceOperStatus (void
                                                                                            *wap,
                                                                                            void
                                                                                            *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalInstanceOperStatusValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalInstanceStatusTable_cpCaptivePortalInstanceId,
                   (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigOperStatusGet (keycpCaptivePortalInstanceIdValue,
                              &objcpCaptivePortalInstanceOperStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalInstanceOperStatusValue,
                     sizeof (objcpCaptivePortalInstanceOperStatusValue));

  /* return the object value: cpCaptivePortalInstanceOperStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalInstanceOperStatusValue,
                           sizeof (objcpCaptivePortalInstanceOperStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalInstanceStatusTable_cpCaptivePortalInstanceOperDisabledReason
*
* @purpose Get 'cpCaptivePortalInstanceOperDisabledReason'
 *@description  [cpCaptivePortalInstanceOperDisabledReason] The reason the
* captive portal instance is not currently operational.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalInstanceStatusTable_cpCaptivePortalInstanceOperDisabledReason
(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalInstanceOperDisabledReasonValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t objcpCaptivePortalInstanceOperStatusValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalInstanceStatusTable_cpCaptivePortalInstanceId,
                   (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  if ((usmDbCpdmCPConfigOperStatusGet (keycpCaptivePortalInstanceIdValue,
                              &objcpCaptivePortalInstanceOperStatusValue) == L7_SUCCESS)
                  && (objcpCaptivePortalInstanceOperStatusValue == L7_CP_INST_OPER_STATUS_DISABLED))
  {
    /* get the value from application */
    owa.l7rc = usmDbCpdmCPConfigDisableReasonGet (keycpCaptivePortalInstanceIdValue,
                              &objcpCaptivePortalInstanceOperDisabledReasonValue);
  }
  else
    owa.l7rc = L7_ERROR;

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalInstanceOperDisabledReasonValue,
                     sizeof (objcpCaptivePortalInstanceOperDisabledReasonValue));

  /* return the object value: cpCaptivePortalInstanceOperDisabledReason */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalInstanceOperDisabledReasonValue,
                           sizeof (objcpCaptivePortalInstanceOperDisabledReasonValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalInstanceStatusTable_cpCaptivePortalInstanceBlockStatus
*
* @purpose Get 'cpCaptivePortalInstanceBlockStatus'
 *@description  [cpCaptivePortalInstanceBlockStatus] Block or unblock the
* captive portal instance for authentications.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalInstanceStatusTable_cpCaptivePortalInstanceBlockStatus (void
                                                                                             *wap,
                                                                                             void
                                                                                             *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalInstanceBlockStatusValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalInstanceStatusTable_cpCaptivePortalInstanceId,
                   (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigBlockedStatusGet (keycpCaptivePortalInstanceIdValue,
                              &objcpCaptivePortalInstanceBlockStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalInstanceBlockStatusValue,
                     sizeof (objcpCaptivePortalInstanceBlockStatusValue));

  /* return the object value: cpCaptivePortalInstanceBlockStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalInstanceBlockStatusValue,
                           sizeof (objcpCaptivePortalInstanceBlockStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpCaptivePortalInstanceStatusTable_cpCaptivePortalInstanceBlockStatus
*
* @purpose Set 'cpCaptivePortalInstanceBlockStatus'
 *@description  [cpCaptivePortalInstanceBlockStatus] Block or unblock the
* captive portal instance for authentications.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjSet_captiveportalcpCaptivePortalInstanceStatusTable_cpCaptivePortalInstanceBlockStatus (void
                                                                                             *wap,
                                                                                             void
                                                                                             *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalInstanceBlockStatusValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalInstanceBlockStatus */
  owa.len = sizeof (objcpCaptivePortalInstanceBlockStatusValue);
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objcpCaptivePortalInstanceBlockStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalInstanceBlockStatusValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalInstanceStatusTable_cpCaptivePortalInstanceId,
                   (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigBlockedStatusSet (keycpCaptivePortalInstanceIdValue,
                              objcpCaptivePortalInstanceBlockStatusValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalInstanceStatusTable_cpCaptivePortalInstanceAuthUserCount
*
* @purpose Get 'cpCaptivePortalInstanceAuthUserCount'
 *@description  [cpCaptivePortalInstanceAuthUserCount] The number of currently
* authenticated users for this captive portal.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalInstanceStatusTable_cpCaptivePortalInstanceAuthUserCount (void
                                                                                               *wap,
                                                                                               void
                                                                                               *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalInstanceAuthUserCountValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalInstanceStatusTable_cpCaptivePortalInstanceId,
                   (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigAuthenticatedUsersGet (keycpCaptivePortalInstanceIdValue,
                              &objcpCaptivePortalInstanceAuthUserCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalInstanceAuthUserCountValue,
                     sizeof (objcpCaptivePortalInstanceAuthUserCountValue));

  /* return the object value: cpCaptivePortalInstanceAuthUserCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalInstanceAuthUserCountValue,
                           sizeof (objcpCaptivePortalInstanceAuthUserCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpCaptivePortalInstanceStatusTable_cpCaptivePortalInstanceStatusByCPIdDeleteAll
*
* @purpose Set 'cpCaptivePortalInstanceStatusByCPIdDeleteAll'
 *@description  [cpCaptivePortalInstanceStatusByCPIdDeleteAll] Delete All
* Connection status with given Captive Portal Instance ID.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjSet_captiveportalcpCaptivePortalInstanceStatusTable_cpCaptivePortalInstanceStatusByCPIdDeleteAll
(void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalInstanceStatusByCPIdDeleteAllValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalInstanceStatusByCPIdDeleteAll */
  owa.len = sizeof (objcpCaptivePortalInstanceStatusByCPIdDeleteAllValue);
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objcpCaptivePortalInstanceStatusByCPIdDeleteAllValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalInstanceStatusByCPIdDeleteAllValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpCaptivePortalInstanceStatusTable_cpCaptivePortalInstanceId,
                   (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmClientConnStatusByCPIdDeleteAll (keycpCaptivePortalInstanceIdValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_CP_FAILED_TO_DEAUTH_CLIENTS;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
