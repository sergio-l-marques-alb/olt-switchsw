
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_captiveportalcpLocalUserTable.c
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
#include "_xe_captiveportalcpLocalUserTable_obj.h"

#include "usmdb_cpdm_user_api.h"

/*******************************************************************************
* @function fpObjGet_captiveportalcpLocalUserTable_cpLocalUserIndex
*
* @purpose Get 'cpLocalUserIndex'
 *@description  [cpLocalUserIndex] The local user entry index.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpLocalUserTable_cpLocalUserIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpLocalUserIndexValue;
  xLibU32_t nextObjcpLocalUserIndexValue;
  xLibU16_t cpLocalUserIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpLocalUserIndex */
  owa.len = sizeof (objcpLocalUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpLocalUserTable_cpLocalUserIndex,
                          (xLibU8_t *) & objcpLocalUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objcpLocalUserIndexValue = 0;
    owa.l7rc = usmDbCpdmUserEntryNextGet (objcpLocalUserIndexValue, &cpLocalUserIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objcpLocalUserIndexValue, owa.len);
    owa.l7rc = usmDbCpdmUserEntryNextGet (objcpLocalUserIndexValue, &cpLocalUserIndexValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  nextObjcpLocalUserIndexValue = (xLibU32_t)cpLocalUserIndexValue;

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjcpLocalUserIndexValue, owa.len);

  /* return the object value: cpLocalUserIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjcpLocalUserIndexValue,
                           sizeof (nextObjcpLocalUserIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpLocalUserTable_cpLocalUserName
*
* @purpose Get 'cpLocalUserName'
 *@description  [cpLocalUserName] The user name, limited to alpha-numeric
* strings (including the '-' and '_' characters).   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpLocalUserTable_cpLocalUserName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objcpLocalUserNameValue;

  xLibU32_t keycpLocalUserIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpLocalUserIndex */
  owa.len = sizeof (keycpLocalUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpLocalUserTable_cpLocalUserIndex,
                          (xLibU8_t *) & keycpLocalUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpLocalUserIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmUserEntryLoginNameGet (keycpLocalUserIndexValue, objcpLocalUserNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpLocalUserNameValue, strlen (objcpLocalUserNameValue));

  /* return the object value: cpLocalUserName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpLocalUserNameValue,
                           strlen (objcpLocalUserNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpLocalUserTable_cpLocalUserName
*
* @purpose Set 'cpLocalUserName'
 *@description  [cpLocalUserName] The user name, limited to alpha-numeric
* strings (including the '-' and '_' characters).   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpLocalUserTable_cpLocalUserName (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objcpLocalUserNameValue;

  xLibU32_t keycpLocalUserIndexValue;
  xLibU16_t cpLocalUserId;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpLocalUserName */
  owa.len = sizeof (objcpLocalUserNameValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objcpLocalUserNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objcpLocalUserNameValue, owa.len);

  /* retrieve key: cpLocalUserIndex */
  owa.len = sizeof (keycpLocalUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpLocalUserTable_cpLocalUserIndex,
                          (xLibU8_t *) & keycpLocalUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpLocalUserIndexValue, owa.len);

  if (usmDbCpdmUserEntryByNameGet (objcpLocalUserNameValue, (xLibU16_t *)&cpLocalUserId) == L7_SUCCESS)
  {
     usmDbCpdmUserEntryDelete (keycpLocalUserIndexValue);
     owa.rc = XLIBRC_CP_USER_ALREADY_EXISTS;
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbCpdmUserEntryLoginNameSet (keycpLocalUserIndexValue, objcpLocalUserNameValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpLocalUserTable_cpLocalUserPassword
*
* @purpose Get 'cpLocalUserPassword'
 *@description  [cpLocalUserPassword] The user password. This object will return
* an empty string even if a password is set.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpLocalUserTable_cpLocalUserPassword (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objcpLocalUserPasswordValue;

  xLibU32_t keycpLocalUserIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpLocalUserIndex */
  owa.len = sizeof (keycpLocalUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpLocalUserTable_cpLocalUserIndex,
                          (xLibU8_t *) & keycpLocalUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpLocalUserIndexValue, owa.len);

  FPOBJ_CLR_STR256(objcpLocalUserPasswordValue);
  /* get the value from application */
  owa.l7rc = usmDbCpdmUserEntryPasswordGet (keycpLocalUserIndexValue,
                              objcpLocalUserPasswordValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpLocalUserPasswordValue, strlen (objcpLocalUserPasswordValue));

  /* return the object value: cpLocalUserPassword */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpLocalUserPasswordValue,
                           strlen (objcpLocalUserPasswordValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpLocalUserTable_cpLocalUserPassword
*
* @purpose Set 'cpLocalUserPassword'
 *@description  [cpLocalUserPassword] The user password. This object will return
* an empty string even if a password is set.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpLocalUserTable_cpLocalUserPassword (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objcpLocalUserPasswordValue;

  xLibU32_t keycpLocalUserIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpLocalUserPassword */
  owa.len = sizeof (objcpLocalUserPasswordValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objcpLocalUserPasswordValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objcpLocalUserPasswordValue, owa.len);

  /* retrieve key: cpLocalUserIndex */
  owa.len = sizeof (keycpLocalUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpLocalUserTable_cpLocalUserIndex,
                          (xLibU8_t *) & keycpLocalUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpLocalUserIndexValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmUserEntryPasswordSet (keycpLocalUserIndexValue,
                              objcpLocalUserPasswordValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_CP_FAILED_TO_SET_PASSWORD_USER_PASSWORD;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpLocalUserTable_cpLocalUserSessionTimeout
*
* @purpose Get 'cpLocalUserSessionTimeout'
 *@description  [cpLocalUserSessionTimeout] The session timeout in seconds.
* After this limit is reached, the user is disconnected.If this value
* is set to 0, the default value for the captive portal is used.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpLocalUserTable_cpLocalUserSessionTimeout (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpLocalUserSessionTimeoutValue;

  xLibU32_t keycpLocalUserIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpLocalUserIndex */
  owa.len = sizeof (keycpLocalUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpLocalUserTable_cpLocalUserIndex,
                          (xLibU8_t *) & keycpLocalUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpLocalUserIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmUserEntrySessionTimeoutGet (keycpLocalUserIndexValue,
                              &objcpLocalUserSessionTimeoutValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpLocalUserSessionTimeoutValue,
                     sizeof (objcpLocalUserSessionTimeoutValue));

  /* return the object value: cpLocalUserSessionTimeout */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpLocalUserSessionTimeoutValue,
                           sizeof (objcpLocalUserSessionTimeoutValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpLocalUserTable_cpLocalUserSessionTimeout
*
* @purpose Set 'cpLocalUserSessionTimeout'
 *@description  [cpLocalUserSessionTimeout] The session timeout in seconds.
* After this limit is reached, the user is disconnected.If this value
* is set to 0, the default value for the captive portal is used.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpLocalUserTable_cpLocalUserSessionTimeout (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpLocalUserSessionTimeoutValue;

  xLibU32_t keycpLocalUserIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpLocalUserSessionTimeout */
  owa.len = sizeof (objcpLocalUserSessionTimeoutValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcpLocalUserSessionTimeoutValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpLocalUserSessionTimeoutValue, owa.len);

  /* retrieve key: cpLocalUserIndex */
  owa.len = sizeof (keycpLocalUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpLocalUserTable_cpLocalUserIndex,
                          (xLibU8_t *) & keycpLocalUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpLocalUserIndexValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmUserEntrySessionTimeoutSet (keycpLocalUserIndexValue,
                              objcpLocalUserSessionTimeoutValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpLocalUserTable_cpLocalUserIdleTimeout
*
* @purpose Get 'cpLocalUserIdleTimeout'
 *@description  [cpLocalUserIdleTimeout] The idle timeout of the session in
* seconds. After this limit is reached, the user is disconnected.If
* this value is set to 0, the default value for the captive portal is
* used.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpLocalUserTable_cpLocalUserIdleTimeout (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpLocalUserIdleTimeoutValue;

  xLibU32_t keycpLocalUserIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpLocalUserIndex */
  owa.len = sizeof (keycpLocalUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpLocalUserTable_cpLocalUserIndex,
                          (xLibU8_t *) & keycpLocalUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpLocalUserIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmUserEntryIdleTimeoutGet (keycpLocalUserIndexValue,
                              &objcpLocalUserIdleTimeoutValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpLocalUserIdleTimeoutValue,
                     sizeof (objcpLocalUserIdleTimeoutValue));

  /* return the object value: cpLocalUserIdleTimeout */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpLocalUserIdleTimeoutValue,
                           sizeof (objcpLocalUserIdleTimeoutValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpLocalUserTable_cpLocalUserIdleTimeout
*
* @purpose Set 'cpLocalUserIdleTimeout'
 *@description  [cpLocalUserIdleTimeout] The idle timeout of the session in
* seconds. After this limit is reached, the user is disconnected.If
* this value is set to 0, the default value for the captive portal is
* used.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpLocalUserTable_cpLocalUserIdleTimeout (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpLocalUserIdleTimeoutValue;

  xLibU32_t keycpLocalUserIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpLocalUserIdleTimeout */
  owa.len = sizeof (objcpLocalUserIdleTimeoutValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcpLocalUserIdleTimeoutValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpLocalUserIdleTimeoutValue, owa.len);

  /* retrieve key: cpLocalUserIndex */
  owa.len = sizeof (keycpLocalUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpLocalUserTable_cpLocalUserIndex,
                          (xLibU8_t *) & keycpLocalUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpLocalUserIndexValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmUserEntryIdleTimeoutSet (keycpLocalUserIndexValue,
                              objcpLocalUserIdleTimeoutValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpLocalUserTable_cpLocalUserMaxBandwidthUp
*
* @purpose Get 'cpLocalUserMaxBandwidthUp'
 *@description  [cpLocalUserMaxBandwidthUp] The maximum bandwidth up/transmit
* bytes per second rate of the session.After this limit has been
* reached the user will be disconnected.If this value is set to 0, no
* rate limiting is enforced.This value is represented in bytes
* (however; converted and stored as bits/sec).   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpLocalUserTable_cpLocalUserMaxBandwidthUp (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpLocalUserMaxBandwidthUpValue;

  xLibU32_t keycpLocalUserIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpLocalUserIndex */
  owa.len = sizeof (keycpLocalUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpLocalUserTable_cpLocalUserIndex,
                          (xLibU8_t *) & keycpLocalUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpLocalUserIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmUserEntryMaxBandwidthUpGet (keycpLocalUserIndexValue,
                              &objcpLocalUserMaxBandwidthUpValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpLocalUserMaxBandwidthUpValue,
                     sizeof (objcpLocalUserMaxBandwidthUpValue));

  /* return the object value: cpLocalUserMaxBandwidthUp */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpLocalUserMaxBandwidthUpValue,
                           sizeof (objcpLocalUserMaxBandwidthUpValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpLocalUserTable_cpLocalUserMaxBandwidthUp
*
* @purpose Set 'cpLocalUserMaxBandwidthUp'
 *@description  [cpLocalUserMaxBandwidthUp] The maximum bandwidth up/transmit
* bytes per second rate of the session.After this limit has been
* reached the user will be disconnected.If this value is set to 0, no
* rate limiting is enforced.This value is represented in bytes
* (however; converted and stored as bits/sec).   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpLocalUserTable_cpLocalUserMaxBandwidthUp (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpLocalUserMaxBandwidthUpValue;

  xLibU32_t keycpLocalUserIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpLocalUserMaxBandwidthUp */
  owa.len = sizeof (objcpLocalUserMaxBandwidthUpValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcpLocalUserMaxBandwidthUpValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpLocalUserMaxBandwidthUpValue, owa.len);

  /* retrieve key: cpLocalUserIndex */
  owa.len = sizeof (keycpLocalUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpLocalUserTable_cpLocalUserIndex,
                          (xLibU8_t *) & keycpLocalUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpLocalUserIndexValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmUserEntryMaxBandwidthUpSet (keycpLocalUserIndexValue,
                              objcpLocalUserMaxBandwidthUpValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpLocalUserTable_cpLocalUserMaxBandwidthDown
*
* @purpose Get 'cpLocalUserMaxBandwidthDown'
 *@description  [cpLocalUserMaxBandwidthDown] The maximum bandwidth down/receive
* bytes per second rate of the session.After this limit has been
* reached the user will be disconnected.If this value is set to 0,
* no rate limiting is enforced.This value is represented in bytes
* (however; converted and stored as bits/sec).   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpLocalUserTable_cpLocalUserMaxBandwidthDown (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpLocalUserMaxBandwidthDownValue;

  xLibU32_t keycpLocalUserIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpLocalUserIndex */
  owa.len = sizeof (keycpLocalUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpLocalUserTable_cpLocalUserIndex,
                          (xLibU8_t *) & keycpLocalUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpLocalUserIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmUserEntryMaxBandwidthDownGet (keycpLocalUserIndexValue,
                              &objcpLocalUserMaxBandwidthDownValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpLocalUserMaxBandwidthDownValue,
                     sizeof (objcpLocalUserMaxBandwidthDownValue));

  /* return the object value: cpLocalUserMaxBandwidthDown */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpLocalUserMaxBandwidthDownValue,
                           sizeof (objcpLocalUserMaxBandwidthDownValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpLocalUserTable_cpLocalUserMaxBandwidthDown
*
* @purpose Set 'cpLocalUserMaxBandwidthDown'
 *@description  [cpLocalUserMaxBandwidthDown] The maximum bandwidth down/receive
* bytes per second rate of the session.After this limit has been
* reached the user will be disconnected.If this value is set to 0,
* no rate limiting is enforced.This value is represented in bytes
* (however; converted and stored as bits/sec).   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpLocalUserTable_cpLocalUserMaxBandwidthDown (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpLocalUserMaxBandwidthDownValue;

  xLibU32_t keycpLocalUserIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpLocalUserMaxBandwidthDown */
  owa.len = sizeof (objcpLocalUserMaxBandwidthDownValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcpLocalUserMaxBandwidthDownValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpLocalUserMaxBandwidthDownValue, owa.len);

  /* retrieve key: cpLocalUserIndex */
  owa.len = sizeof (keycpLocalUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpLocalUserTable_cpLocalUserIndex,
                          (xLibU8_t *) & keycpLocalUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpLocalUserIndexValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmUserEntryMaxBandwidthDownSet (keycpLocalUserIndexValue,
                              objcpLocalUserMaxBandwidthDownValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpLocalUserTable_cpLocalUserMaxInputOctets
*
* @purpose Get 'cpLocalUserMaxInputOctets'
 *@description  [cpLocalUserMaxInputOctets] The maximum input octets (bytes)
* allowed for the session.After this limit has been reached the user
* will be disconnected.If this value is set to 0, no rate limiting
* is enforced.This value is represented in bytes.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpLocalUserTable_cpLocalUserMaxInputOctets (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpLocalUserMaxInputOctetsValue;

  xLibU32_t keycpLocalUserIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpLocalUserIndex */
  owa.len = sizeof (keycpLocalUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpLocalUserTable_cpLocalUserIndex,
                          (xLibU8_t *) & keycpLocalUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpLocalUserIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmUserEntryMaxInputOctetsGet (keycpLocalUserIndexValue,
                              &objcpLocalUserMaxInputOctetsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpLocalUserMaxInputOctetsValue,
                     sizeof (objcpLocalUserMaxInputOctetsValue));

  /* return the object value: cpLocalUserMaxInputOctets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpLocalUserMaxInputOctetsValue,
                           sizeof (objcpLocalUserMaxInputOctetsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpLocalUserTable_cpLocalUserMaxInputOctets
*
* @purpose Set 'cpLocalUserMaxInputOctets'
 *@description  [cpLocalUserMaxInputOctets] The maximum input octets (bytes)
* allowed for the session.After this limit has been reached the user
* will be disconnected.If this value is set to 0, no rate limiting
* is enforced.This value is represented in bytes.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpLocalUserTable_cpLocalUserMaxInputOctets (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpLocalUserMaxInputOctetsValue;

  xLibU32_t keycpLocalUserIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpLocalUserMaxInputOctets */
  owa.len = sizeof (objcpLocalUserMaxInputOctetsValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcpLocalUserMaxInputOctetsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpLocalUserMaxInputOctetsValue, owa.len);

  /* retrieve key: cpLocalUserIndex */
  owa.len = sizeof (keycpLocalUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpLocalUserTable_cpLocalUserIndex,
                          (xLibU8_t *) & keycpLocalUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpLocalUserIndexValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmUserEntryMaxInputOctetsSet (keycpLocalUserIndexValue,
                              objcpLocalUserMaxInputOctetsValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpLocalUserTable_cpLocalUserMaxOutputOctets
*
* @purpose Get 'cpLocalUserMaxOutputOctets'
 *@description  [cpLocalUserMaxOutputOctets] The maximum Output octets (bytes)
* allowed for the session.After this limit has been reached the user
* will be disconnected.If this value is set to 0, no rate limiting
* is enforced.This value is represented in bytes.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpLocalUserTable_cpLocalUserMaxOutputOctets (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpLocalUserMaxOutputOctetsValue;

  xLibU32_t keycpLocalUserIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpLocalUserIndex */
  owa.len = sizeof (keycpLocalUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpLocalUserTable_cpLocalUserIndex,
                          (xLibU8_t *) & keycpLocalUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpLocalUserIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmUserEntryMaxOutputOctetsGet (keycpLocalUserIndexValue,
                              &objcpLocalUserMaxOutputOctetsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpLocalUserMaxOutputOctetsValue,
                     sizeof (objcpLocalUserMaxOutputOctetsValue));

  /* return the object value: cpLocalUserMaxOutputOctets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpLocalUserMaxOutputOctetsValue,
                           sizeof (objcpLocalUserMaxOutputOctetsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpLocalUserTable_cpLocalUserMaxOutputOctets
*
* @purpose Set 'cpLocalUserMaxOutputOctets'
 *@description  [cpLocalUserMaxOutputOctets] The maximum Output octets (bytes)
* allowed for the session.After this limit has been reached the user
* will be disconnected.If this value is set to 0, no rate limiting
* is enforced.This value is represented in bytes.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpLocalUserTable_cpLocalUserMaxOutputOctets (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpLocalUserMaxOutputOctetsValue;

  xLibU32_t keycpLocalUserIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpLocalUserMaxOutputOctets */
  owa.len = sizeof (objcpLocalUserMaxOutputOctetsValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcpLocalUserMaxOutputOctetsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpLocalUserMaxOutputOctetsValue, owa.len);

  /* retrieve key: cpLocalUserIndex */
  owa.len = sizeof (keycpLocalUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpLocalUserTable_cpLocalUserIndex,
                          (xLibU8_t *) & keycpLocalUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpLocalUserIndexValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmUserEntryMaxOutputOctetsSet (keycpLocalUserIndexValue,
                              objcpLocalUserMaxOutputOctetsValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpLocalUserTable_cpLocalUserMaxTotalOctets
*
* @purpose Get 'cpLocalUserMaxTotalOctets'
 *@description  [cpLocalUserMaxTotalOctets] The maximum input/output octets
* (bytes) allowed for the session.After this limit has been reached the
* user will be disconnected.If this value is set to 0, no rate
* limiting is enforced.This value is represented in bytes.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpLocalUserTable_cpLocalUserMaxTotalOctets (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpLocalUserMaxTotalOctetsValue;

  xLibU32_t keycpLocalUserIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpLocalUserIndex */
  owa.len = sizeof (keycpLocalUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpLocalUserTable_cpLocalUserIndex,
                          (xLibU8_t *) & keycpLocalUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpLocalUserIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmUserEntryMaxTotalOctetsGet (keycpLocalUserIndexValue,
                              &objcpLocalUserMaxTotalOctetsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpLocalUserMaxTotalOctetsValue,
                     sizeof (objcpLocalUserMaxTotalOctetsValue));

  /* return the object value: cpLocalUserMaxTotalOctets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpLocalUserMaxTotalOctetsValue,
                           sizeof (objcpLocalUserMaxTotalOctetsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpLocalUserTable_cpLocalUserMaxTotalOctets
*
* @purpose Set 'cpLocalUserMaxTotalOctets'
 *@description  [cpLocalUserMaxTotalOctets] The maximum input/output octets
* (bytes) allowed for the session.After this limit has been reached the
* user will be disconnected.If this value is set to 0, no rate
* limiting is enforced.This value is represented in bytes.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpLocalUserTable_cpLocalUserMaxTotalOctets (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpLocalUserMaxTotalOctetsValue;

  xLibU32_t keycpLocalUserIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpLocalUserMaxTotalOctets */
  owa.len = sizeof (objcpLocalUserMaxTotalOctetsValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcpLocalUserMaxTotalOctetsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpLocalUserMaxTotalOctetsValue, owa.len);

  /* retrieve key: cpLocalUserIndex */
  owa.len = sizeof (keycpLocalUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpLocalUserTable_cpLocalUserIndex,
                          (xLibU8_t *) & keycpLocalUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpLocalUserIndexValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmUserEntryMaxTotalOctetsSet (keycpLocalUserIndexValue,
                              objcpLocalUserMaxTotalOctetsValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpLocalUserTable_cpLocalUserDeleteAll
*
* @purpose Set 'cpLocalUserDeleteAll'
 *@description  [cpLocalUserDeleteAll] Delete all Users.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpLocalUserTable_cpLocalUserDeleteAll (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpLocalUserDeleteAllValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpLocalUserDeleteAll */
  owa.len = sizeof (objcpLocalUserDeleteAllValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcpLocalUserDeleteAllValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpLocalUserDeleteAllValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbCpdmUserEntryPurge ();
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_captiveportalcpLocalUserTable_cpLocalUserRowStatus
*
* @purpose Get 'cpLocalUserRowStatus'
 *@description  [cpLocalUserRowStatus] The user entry status.Supported
* values:active(1)      - valid entry      createAndGo(4) - creates a new
* entry      destroy(6)     - removes an entry   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpLocalUserTable_cpLocalUserRowStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpLocalUserRowStatusValue;

  xLibU32_t keycpLocalUserIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpLocalUserIndex */
  owa.len = sizeof (keycpLocalUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpLocalUserTable_cpLocalUserIndex,
                          (xLibU8_t *) & keycpLocalUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpLocalUserIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmUserEntryGet (keycpLocalUserIndexValue);

  if (owa.l7rc == L7_SUCCESS)
    objcpLocalUserRowStatusValue = L7_ROW_STATUS_ACTIVE;
  else
    objcpLocalUserRowStatusValue = L7_ROW_STATUS_INVALID;

  FPOBJ_TRACE_VALUE (bufp, &objcpLocalUserRowStatusValue, sizeof (objcpLocalUserRowStatusValue));

  /* return the object value: cpLocalUserRowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpLocalUserRowStatusValue,
                           sizeof (objcpLocalUserRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpLocalUserTable_cpLocalUserRowStatus
*
* @purpose Set 'cpLocalUserRowStatus'
 *@description  [cpLocalUserRowStatus] The user entry status.Supported
* values:active(1)      - valid entry      createAndGo(4) - creates a new
* entry      destroy(6)     - removes an entry   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpLocalUserTable_cpLocalUserRowStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpLocalUserRowStatusValue;

  xLibU32_t keycpLocalUserIndexValue;
  xLibU16_t cpLocalUserIndexValue;
  xLibU32_t objcpLocalUserIndexValue;
  xLibStr256_t objcpLocalUserNameValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpLocalUserRowStatus */
  owa.len = sizeof (objcpLocalUserRowStatusValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcpLocalUserRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpLocalUserRowStatusValue, owa.len);

  /* retrieve key: cpLocalUserIndex */
  owa.len = sizeof (keycpLocalUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpLocalUserTable_cpLocalUserIndex,
                          (xLibU8_t *) & keycpLocalUserIndexValue, &owa.len);
  if ((owa.rc != XLIBRC_SUCCESS) && (objcpLocalUserRowStatusValue != L7_ROW_STATUS_CREATE_AND_GO))
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpLocalUserIndexValue, owa.len);

  owa.l7rc = L7_SUCCESS;
  if (objcpLocalUserRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
  /* retrieve user name from filters */
  owa.len = sizeof (objcpLocalUserNameValue);
  FPOBJ_CLR_STR256(objcpLocalUserNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpLocalUserTable_cpLocalUserName,
                          (xLibU8_t *) objcpLocalUserNameValue, &owa.len);

  cpLocalUserIndexValue = 1;
  while ((usmDbCpdmUserEntryGet(cpLocalUserIndexValue) == L7_SUCCESS) && (cpLocalUserIndexValue <= FD_CP_LOCAL_USERS_MAX))
    {
      cpLocalUserIndexValue++;
    }

  if (cpLocalUserIndexValue > FD_CP_LOCAL_USERS_MAX)
  {
     owa.rc = XLIBRC_CP_USER_MAX_ADDED;
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
  }
    
  owa.l7rc = usmDbCpdmUserEntryAdd (cpLocalUserIndexValue);

  if (owa.l7rc != L7_SUCCESS)
  {
     owa.rc = XLIBRC_CP_FAILED_TO_ADD_USER;
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
  }

  if (usmDbCpdmUserGroupAssocEntryGet(cpLocalUserIndexValue, GP_ID_MIN) != L7_SUCCESS)
  {
     owa.l7rc = usmDbCpdmUserGroupAssocEntryAdd (cpLocalUserIndexValue, GP_ID_MIN);
  }

  objcpLocalUserIndexValue = (xLibU32_t)cpLocalUserIndexValue;

  owa.rc = xLibFilterSet(wap,XOBJ_captiveportalcpLocalUserTable_cpLocalUserIndex,0,(xLibU8_t *) &objcpLocalUserIndexValue,sizeof(objcpLocalUserIndexValue));
  }
  else if (objcpLocalUserRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    if ((owa.l7rc = usmDbCpdmUserEntryDelete (keycpLocalUserIndexValue)) != L7_SUCCESS)
    {
      owa.rc = XLIBRC_CP_FAILED_TO_DELETE_USER;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

  usmDbCpdmUserGroupAssocEntryByUIdDeleteAll(keycpLocalUserIndexValue);
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

xLibRC_t fpObjGet_captiveportalcpLocalUserTable_cpLocalUserGroupIndexRange(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  L7_uint32 keycpLocalUserIndex;

  L7_uint32 grpIdList[GP_ID_MAX];
  L7_uint32 grpId;
  xLibU16_t objcpGrpId;
  xLibU16_t nextobjcpGrpId;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpLocalUserGroupIndex */
  owa.len = sizeof (keycpLocalUserIndex);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpLocalUserTable_cpLocalUserIndex,
                          (xLibU8_t *) & keycpLocalUserIndex, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpLocalUserIndex, owa.len);

  grpId = 0;
  nextobjcpGrpId = objcpGrpId = 0;
  memset(grpIdList,0,sizeof(grpIdList));
  while (owa.l7rc == L7_SUCCESS)
  {
    objcpGrpId = nextobjcpGrpId;
    owa.l7rc = usmDbCpdmUserGroupEntryNextGet (objcpGrpId, &nextobjcpGrpId);
    if((nextobjcpGrpId != objcpGrpId) 
        && (usmDbCpdmUserGroupAssocEntryGet (keycpLocalUserIndex,
                        nextobjcpGrpId) == L7_SUCCESS)) {
       grpIdList[grpId] = nextobjcpGrpId;
       grpId++;
    }
  }

  /* return the object value: GroupInterface*/
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &grpIdList,
                           sizeof (grpIdList));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

xLibRC_t fpObjSet_captiveportalcpLocalUserTable_cpLocalUserGroupIndexRange(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  L7_uint32 grpIdList[GP_ID_MAX];
  L7_uint32 index;

  xLibU32_t keycpLocalUserIndexValue;

  memset(grpIdList,0x0,sizeof(grpIdList));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpLocalUserRowStatus */
  owa.len = sizeof (grpIdList);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) grpIdList, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &grpIdList, owa.len);

  /* retrieve key: cpLocalUserIndex */
  owa.len = sizeof (keycpLocalUserIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpLocalUserTable_cpLocalUserIndex,
                          (xLibU8_t *) & keycpLocalUserIndexValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpLocalUserIndexValue, owa.len);

  /* This can be optmised. */
  usmDbCpdmUserGroupAssocEntryByUIdDeleteAll(keycpLocalUserIndexValue);

  index = 0;
  while((owa.l7rc == L7_SUCCESS) && (index<GP_ID_MAX) &&(grpIdList[index] != 0))
  {
    if (usmDbCpdmUserGroupAssocEntryGet(keycpLocalUserIndexValue, grpIdList[index]) != L7_SUCCESS)
    {
        owa.l7rc = usmDbCpdmUserGroupAssocEntryAdd (keycpLocalUserIndexValue,
                           grpIdList[index]);
    }
    index++;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
