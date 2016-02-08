
/*******************************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2008
 *
 ********************************************************************************
 *
 * @filename fpobj_captiveportalcpCaptivePortalConfigWeb.c
 *
 * @purpose  
 *
 * @component object handlers
 *
 * @comments  Refer to captiveportal-object.xml
 *
 * @create  24 July 2008, Thursday
 *
 * @notes   This file is auto generated and should be used as starting point to
 *          develop the object handlers
 *
 * @author  Rama Sasthri, Kristipati
 * @end
 *
 ********************************************************************************/
#include<dirent.h>
#include "fpobj_util.h"
#include "_xe_captiveportalcpCaptivePortalConfigWeb_obj.h"
#include "xlib_private.h"

#include "usmdb_cpdm_api.h"
#include "usmdb_cpdm_web_api.h"
#include "captive_portal_commdefs.h"
#include "captive_portal_util.h"

#define FILE_SEPARATOR "/"

/*******************************************************************************
 * @function fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId
 *
 * @purpose Get 'cpCaptivePortalInstanceId'
 *@description  [cpCaptivePortalInstanceId] The identifier associated with this
 * instance of captive portal.   
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId (void *wap,
    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalInstanceIdValue;
  xLibU32_t nextObjcpCaptivePortalInstanceIdValue;

  xLibU16_t cpInstanceId;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (objcpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & objcpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objcpCaptivePortalInstanceIdValue = 0;
    owa.l7rc = usmDbCpdmCPConfigNextGet (objcpCaptivePortalInstanceIdValue, 
        &cpInstanceId);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objcpCaptivePortalInstanceIdValue, owa.len);
    owa.l7rc = usmDbCpdmCPConfigNextGet (objcpCaptivePortalInstanceIdValue,
        &cpInstanceId);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  nextObjcpCaptivePortalInstanceIdValue = (xLibU32_t)cpInstanceId;

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjcpCaptivePortalInstanceIdValue, owa.len);

  /* return the object value: cpCaptivePortalInstanceId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjcpCaptivePortalInstanceIdValue,
      sizeof (nextObjcpCaptivePortalInstanceIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId
 *
 * @purpose Get 'cpCaptivePortalWebId'
 *@description  [cpCaptivePortalWebId] Web ID.   
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalWebIdValue;
  xLibU32_t nextObjcpCaptivePortalWebIdValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU16_t cpWebId;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (objcpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & objcpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objcpCaptivePortalWebIdValue = 0;
    owa.l7rc = usmDbCpdmCPConfigWebIdNextGet (keycpCaptivePortalInstanceIdValue,
        objcpCaptivePortalWebIdValue,
        &cpWebId);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objcpCaptivePortalWebIdValue, owa.len);
    owa.l7rc = usmDbCpdmCPConfigWebIdNextGet (keycpCaptivePortalInstanceIdValue,
        objcpCaptivePortalWebIdValue,
        &cpWebId);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  nextObjcpCaptivePortalWebIdValue = (xLibU32_t)cpWebId;

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjcpCaptivePortalWebIdValue, owa.len);

  /* return the object value: cpCaptivePortalWebId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjcpCaptivePortalWebIdValue,
      sizeof (nextObjcpCaptivePortalWebIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebBackgroundColor
 *
 * @purpose Get 'cpCaptivePortalConfigWebBackgroundColor'
 *@description  [cpCaptivePortalConfigWebBackgroundColor] String of the
 * Background color.   
 * @notes       
 *
 * @return
 *******************************************************************************/
  xLibRC_t
fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebBackgroundColor (void *wap,
    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebBackgroundColorValue[CP_BACKGROUND_COLOR_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  FPOBJ_CLR_STR256(objcpCaptivePortalConfigWebBackgroundColorValue);
  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigBackgroundColorGet (keycpCaptivePortalInstanceIdValue,
      objcpCaptivePortalConfigWebBackgroundColorValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebBackgroundColorValue,
      strlen (objcpCaptivePortalConfigWebBackgroundColorValue));

  /* return the object value: cpCaptivePortalConfigWebBackgroundColor */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalConfigWebBackgroundColorValue,
      strlen (objcpCaptivePortalConfigWebBackgroundColorValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebBackgroundColor
 *
 * @purpose Set 'cpCaptivePortalConfigWebBackgroundColor'
 *@description  [cpCaptivePortalConfigWebBackgroundColor] String of the
 * Background color.   
 * @notes       
 *
 * @return
 *******************************************************************************/
  xLibRC_t
fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebBackgroundColor (void *wap,
    void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebBackgroundColorValue[CP_BACKGROUND_COLOR_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalConfigWebBackgroundColor */
  owa.len = sizeof (objcpCaptivePortalConfigWebBackgroundColorValue);
  memset(objcpCaptivePortalConfigWebBackgroundColorValue, 0, owa.len);
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) objcpCaptivePortalConfigWebBackgroundColorValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebBackgroundColorValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigBackgroundColorSet (keycpCaptivePortalInstanceIdValue,
      objcpCaptivePortalConfigWebBackgroundColorValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_CP_FAILED_TO_SET_BACKGROUND_COLOR;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebForegroundColor
 *
 * @purpose Get 'cpCaptivePortalConfigWebForegroundColor'
 *@description  [cpCaptivePortalConfigWebForegroundColor] String of the
 * Foreground color.   
 * @notes       
 *
 * @return
 *******************************************************************************/
  xLibRC_t
fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebForegroundColor (void *wap,
    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebForegroundColorValue[CP_FOREGROUND_COLOR_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  FPOBJ_CLR_STR256(objcpCaptivePortalConfigWebForegroundColorValue);
  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigForegroundColorGet (keycpCaptivePortalInstanceIdValue,
      objcpCaptivePortalConfigWebForegroundColorValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebForegroundColorValue,
      strlen (objcpCaptivePortalConfigWebForegroundColorValue));

  /* return the object value: cpCaptivePortalConfigWebForegroundColor */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalConfigWebForegroundColorValue,
      strlen (objcpCaptivePortalConfigWebForegroundColorValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebForegroundColor
 *
 * @purpose Set 'cpCaptivePortalConfigWebForegroundColor'
 *@description  [cpCaptivePortalConfigWebForegroundColor] String of the
 * Foreground color.   
 * @notes       
 *
 * @return
 *******************************************************************************/
  xLibRC_t
fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebForegroundColor (void *wap,
    void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebForegroundColorValue[CP_FOREGROUND_COLOR_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalConfigWebForegroundColor */
  owa.len = sizeof (objcpCaptivePortalConfigWebForegroundColorValue);
  memset(objcpCaptivePortalConfigWebForegroundColorValue, 0, owa.len);
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) objcpCaptivePortalConfigWebForegroundColorValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebForegroundColorValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigForegroundColorSet (keycpCaptivePortalInstanceIdValue,
      objcpCaptivePortalConfigWebForegroundColorValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_CP_FAILED_TO_SET_FOREGROUND_COLOR;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebForegroundColor
 *
 * @purpose Set 'cpCaptivePortalConfigWebForegroundColor'
 *@description  [cpCaptivePortalConfigWebForegroundColor] String of the
 * Foreground color.   
 * @notes       
 *
 * @return
 *******************************************************************************/
  xLibRC_t
fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebClear (void *wap,
    void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t value;

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalConfigWebForegroundColor */
  owa.len = sizeof (value);
	
  owa.rc = xLibBufDataGet (bufp,(xLibU8_t *) &value, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, value, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigReset (keycpCaptivePortalInstanceIdValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
 * @function fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebSeparatorColor
 *
 * @purpose Get 'cpCaptivePortalConfigWebSeparatorColor'
 *@description  [cpCaptivePortalConfigWebSeparatorColor] String of the Separator
 * color.   
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebSeparatorColor (void
    *wap,
    void
    *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebSeparatorColorValue[CP_SEPARATOR_COLOR_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  FPOBJ_CLR_STR256(objcpCaptivePortalConfigWebSeparatorColorValue);
  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigSeparatorColorGet (keycpCaptivePortalInstanceIdValue,
      objcpCaptivePortalConfigWebSeparatorColorValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebSeparatorColorValue,
      strlen (objcpCaptivePortalConfigWebSeparatorColorValue));

  /* return the object value: cpCaptivePortalConfigWebSeparatorColor */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalConfigWebSeparatorColorValue,
      strlen (objcpCaptivePortalConfigWebSeparatorColorValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebSeparatorColor
 *
 * @purpose Set 'cpCaptivePortalConfigWebSeparatorColor'
 *@description  [cpCaptivePortalConfigWebSeparatorColor] String of the Separator
 * color.   
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebSeparatorColor (void
    *wap,
    void
    *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebSeparatorColorValue[CP_SEPARATOR_COLOR_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalConfigWebSeparatorColor */
  owa.len = sizeof (objcpCaptivePortalConfigWebSeparatorColorValue);
  memset(objcpCaptivePortalConfigWebSeparatorColorValue, 0, owa.len);
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) objcpCaptivePortalConfigWebSeparatorColorValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebSeparatorColorValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigSeparatorColorSet (keycpCaptivePortalInstanceIdValue,
      objcpCaptivePortalConfigWebSeparatorColorValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_CP_FAILED_TO_SET_SEPARATOR_COLOR;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebAcceptText
 *
 * @purpose Get 'cpCaptivePortalConfigWebAcceptText'
 *@description  [cpCaptivePortalConfigWebAcceptText] String of the Backgroung
 * color.   
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebAcceptText (void
    *wap,
    void
    *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebAcceptTextValue[CP_ACCEPT_TEXT_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  FPOBJ_CLR_STR256(objcpCaptivePortalConfigWebAcceptTextValue);
  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigWebAcceptTextGet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebAcceptTextValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebAcceptTextValue,
      strlen (objcpCaptivePortalConfigWebAcceptTextValue));

  /* return the object value: cpCaptivePortalConfigWebAcceptText */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalConfigWebAcceptTextValue,
      strlen (objcpCaptivePortalConfigWebAcceptTextValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebAcceptText
 *
 * @purpose Set 'cpCaptivePortalConfigWebAcceptText'
 *@description  [cpCaptivePortalConfigWebAcceptText] String of the Backgroung
 * color.   
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebAcceptText (void
    *wap,
    void
    *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebAcceptTextValue[CP_AUP_TEXT_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalConfigWebAcceptText */
  owa.len = sizeof (objcpCaptivePortalConfigWebAcceptTextValue);
  memset(objcpCaptivePortalConfigWebAcceptTextValue, 0, owa.len);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objcpCaptivePortalConfigWebAcceptTextValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebAcceptTextValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigWebAcceptTextSet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebAcceptTextValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_CP_FAILED_TO_SET_ACCEPT_TEXT;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjList_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebWebBrandingImageName
 *
 * @purpose Get 'cpCaptivePortalConfigWebWebBrandingImageName'
 *@description  [cpCaptivePortalConfigWebWebBrandingImageName]    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t
fpObjList_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebAccountImageName (void
    *wap,
    void
    *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t current[CP_FILE_NAME_MAX+1];
  xLibS8_t next[CP_FILE_NAME_MAX+1];
  xLibRC_t rc;
 
  FPOBJ_TRACE_ENTER (bufp);

  memset(current,0x0,sizeof(current));
  memset(next,0x0,sizeof(next));

  owa.len = sizeof (current);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebAccountImageName,
      (xLibU8_t *) current, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    strncpy(next,WEB_DEF_BRANDING_IMAGE_NAME,strlen(WEB_DEF_BRANDING_IMAGE_NAME));
		owa.rc = XLIBRC_SUCCESS; 
  }
  else
  {
    rc = cpGetNextDefaultImage(current,next); 
    if(rc == XLIBRC_ENDOF_TABLE)
    {
      rc = cpGetNexFSImage(current,next); 
      if(rc == XLIBRC_ENDOF_TABLE || rc == XLIBRC_FAILURE)
      {
         return rc;
      }
      
      owa.rc = XLIBRC_SUCCESS;  
    }
    else if(rc == XLIBRC_FAILURE)
    {
       return rc;
    }
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, next, owa.len);

  FPOBJ_TRACE_VALUE (bufp, next,
      strlen (next));

  /* return the object value: cpCaptivePortalConfigWebWebBrandingImageName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) next,
      strlen (next));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebAccountImageName
 *
 * @purpose Get 'cpCaptivePortalConfigWebAccountImageName'
 *@description  [cpCaptivePortalConfigWebAccountImageName] String of the
 * Backgroung color.   
 * @notes       
 *
 * @return
 *******************************************************************************/
  xLibRC_t
fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebAccountImageName (void *wap,
    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebAccountImageNameValue[CP_FILE_NAME_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  FPOBJ_CLR_STR256(objcpCaptivePortalConfigWebAccountImageNameValue);
  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigWebAccountImageNameGet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebAccountImageNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebAccountImageNameValue,
      strlen (objcpCaptivePortalConfigWebAccountImageNameValue));

  /* return the object value: cpCaptivePortalConfigWebAccountImageName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalConfigWebAccountImageNameValue,
      strlen (objcpCaptivePortalConfigWebAccountImageNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebAccountImageName
 *
 * @purpose Set 'cpCaptivePortalConfigWebAccountImageName'
 *@description  [cpCaptivePortalConfigWebAccountImageName] String of the
 * Backgroung color.   
 * @notes       
 *
 * @return
 *******************************************************************************/
  xLibRC_t
fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebAccountImageName (void *wap,
    void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebAccountImageNameValue[CP_FILE_NAME_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalConfigWebAccountImageName */
  owa.len = sizeof (objcpCaptivePortalConfigWebAccountImageNameValue);
  memset(objcpCaptivePortalConfigWebAccountImageNameValue, 0, owa.len);
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) objcpCaptivePortalConfigWebAccountImageNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebAccountImageNameValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigWebAccountImageNameSet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebAccountImageNameValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_CP_FAILED_TO_SET_ACCOUNT_IMAGE_NAME;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebAccountLabel
 *
 * @purpose Get 'cpCaptivePortalConfigWebAccountLabel'
 *@description  [cpCaptivePortalConfigWebAccountLabel]    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebAccountLabel (void
    *wap,
    void
    *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebAccountLabelValue[CP_ACCOUNT_LABEL_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  FPOBJ_CLR_STR256(objcpCaptivePortalConfigWebAccountLabelValue);
  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigWebAccountLabelGet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebAccountLabelValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebAccountLabelValue,
      strlen (objcpCaptivePortalConfigWebAccountLabelValue));

  /* return the object value: cpCaptivePortalConfigWebAccountLabel */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalConfigWebAccountLabelValue,
      strlen (objcpCaptivePortalConfigWebAccountLabelValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebAccountLabel
 *
 * @purpose Set 'cpCaptivePortalConfigWebAccountLabel'
 *@description  [cpCaptivePortalConfigWebAccountLabel]    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebAccountLabel (void
    *wap,
    void
    *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebAccountLabelValue[CP_ACCOUNT_LABEL_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalConfigWebAccountLabel */
  owa.len = sizeof (objcpCaptivePortalConfigWebAccountLabelValue);
  memset(objcpCaptivePortalConfigWebAccountLabelValue, 0, owa.len);
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) objcpCaptivePortalConfigWebAccountLabelValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebAccountLabelValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigWebAccountLabelSet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebAccountLabelValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_CP_FAILED_TO_SET_ACCOUNT_LABEL;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebWebAUPText
 *
 * @purpose Get 'cpCaptivePortalConfigWebWebAUPText'
 *@description  [cpCaptivePortalConfigWebWebAUPText]    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebWebAUPText (void
    *wap,
    void
    *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebWebAUPTextValue[CP_AUP_TEXT_MAX];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  FPOBJ_CLR_STR256(objcpCaptivePortalConfigWebWebAUPTextValue);
  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigWebAUPTextGet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebWebAUPTextValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebWebAUPTextValue,
      strlen (objcpCaptivePortalConfigWebWebAUPTextValue));

  /* return the object value: cpCaptivePortalConfigWebWebAUPText */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalConfigWebWebAUPTextValue,
      strlen (objcpCaptivePortalConfigWebWebAUPTextValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebWebAUPText
 *
 * @purpose Set 'cpCaptivePortalConfigWebWebAUPText'
 *@description  [cpCaptivePortalConfigWebWebAUPText]    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebWebAUPText (void
    *wap,
    void
    *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebWebAUPTextValue[CP_AUP_TEXT_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalConfigWebWebAUPText */
  owa.len = sizeof (objcpCaptivePortalConfigWebWebAUPTextValue);
  memset(objcpCaptivePortalConfigWebWebAUPTextValue, 0, owa.len);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objcpCaptivePortalConfigWebWebAUPTextValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebWebAUPTextValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigWebAUPTextSet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebWebAUPTextValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_CP_FAILED_TO_SET_AUP_TEXT;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
 * @function fpObjList_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebWebBrandingImageName
 *
 * @purpose Get 'cpCaptivePortalConfigWebWebBrandingImageName'
 *@description  [cpCaptivePortalConfigWebWebBrandingImageName]    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebAllImages (void
    *wap,
    void
    *bufp)
{
 fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t current[CP_FILE_NAME_MAX+1];
  xLibS8_t next[CP_FILE_NAME_MAX+1];
  
  FPOBJ_TRACE_ENTER (bufp);

  memset(current,0x0,sizeof(current));
  memset(next,0x0,sizeof(next));

  owa.len = sizeof (current);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebAllImages,
      (xLibU8_t *) current, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    strncpy(current,WEB_DEF_ACCOUNT_IMAGE_NAME,strlen(WEB_DEF_ACCOUNT_IMAGE_NAME));
	}
 
  if(cpGetNexFSImage(current,next) == XLIBRC_ENDOF_TABLE)
  {
    return XLIBRC_ENDOF_TABLE;
  }
  owa.rc = XLIBRC_SUCCESS;  
    
 
  FPOBJ_TRACE_CURRENT_KEY (bufp, next, owa.len);

  FPOBJ_TRACE_VALUE (bufp, next,
      strlen (next));

  /* return the object value: cpCaptivePortalConfigWebWebBrandingImageName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) next,
      strlen (next));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
 * @function fpObjList_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebWebBrandingImageName
 *
 * @purpose Get 'cpCaptivePortalConfigWebWebBrandingImageName'
 *@description  [cpCaptivePortalConfigWebWebBrandingImageName]    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t
fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalDeleteImage (void
    *wap,
    void
    *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  struct dirent *entry;  
  DIR *dir = NULL;
  char* ext;
	char filename[32];
	xLibU32_t value;
	char absfilepath[64];

  FPOBJ_TRACE_ENTER (bufp);
  memset(filename,0x0,sizeof(filename));
  owa.len = sizeof(value);
	
	owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) &value,
      &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
	
  owa.len = sizeof (filename);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebAllImages,
      (xLibU8_t *) filename, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
		 return owa.rc;
  }
	
  if ((dir = opendir(RAM_CP_PATH RAM_CP_NAME)) == NULL)   
  {
    return XLIBRC_FAILURE;   
  } 

  while ((NULL!=dir) && (entry = readdir(dir)) != NULL)
  {
    ext = strrchr(entry->d_name,'.');
    if ((NULL!=ext) && (4==strlen(ext)))
    {
      if ((strcmp(ext,".gif")==0) || (strcmp(ext,".GIF")==0) ||
          (strcmp(ext,".jpg")==0) || (strcmp(ext,".JPG")==0))
      {
         if(strcmp(filename,entry->d_name) == 0)
         {
	   memset(absfilepath,0x0,sizeof(absfilepath));
	   sprintf(absfilepath,"%s%s%s%s",RAM_CP_PATH,RAM_CP_NAME,FILE_SEPARATOR,entry->d_name);
	   if(osapiFsDeleteFile(absfilepath)!=L7_SUCCESS)
	   {
         closedir(dir);
	     return XLIBRC_FAILURE;
	   }
	   owa.rc = XLIBRC_SUCCESS;
	   break;
        }
      }
    }
  }

  closedir(dir);
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



/*******************************************************************************
 * @function fpObjList_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebWebBrandingImageName
 *
 * @purpose Get 'cpCaptivePortalConfigWebWebBrandingImageName'
 *@description  [cpCaptivePortalConfigWebWebBrandingImageName]    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t
fpObjList_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebWebBrandingImageName (void
    *wap,
    void
    *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t current[CP_FILE_NAME_MAX+1];
  xLibS8_t next[CP_FILE_NAME_MAX+1];
  xLibRC_t rc ; 
  FPOBJ_TRACE_ENTER (bufp);

  memset(current,0x0,sizeof(current));
  memset(next,0x0,sizeof(next));

  owa.len = sizeof (current);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebWebBrandingImageName,
      (xLibU8_t *) current, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    strncpy(next,WEB_DEF_BRANDING_IMAGE_NAME,strlen(WEB_DEF_BRANDING_IMAGE_NAME));
    owa.rc = XLIBRC_SUCCESS; 
  }
  else
  {
    rc = cpGetNextDefaultImage(current,next);
    if(rc == XLIBRC_ENDOF_TABLE)
    {
      rc = cpGetNexFSImage(current,next);
      if(rc == XLIBRC_ENDOF_TABLE || rc == XLIBRC_FAILURE)
      {
        return rc;
      }
     owa.rc = XLIBRC_SUCCESS;  
    }
    else if(rc == XLIBRC_FAILURE)
    {
      return rc;
    }
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, next, owa.len);

  FPOBJ_TRACE_VALUE (bufp, next,
      strlen (next));

  /* return the object value: cpCaptivePortalConfigWebWebBrandingImageName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) next,
      strlen (next));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebWebBrandingImageName
 *
 * @purpose Get 'cpCaptivePortalConfigWebWebBrandingImageName'
 *@description  [cpCaptivePortalConfigWebWebBrandingImageName]    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebWebBrandingImageName (void
    *wap,
    void
    *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebWebBrandingImageNameValue[CP_FILE_NAME_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  FPOBJ_CLR_STR256(objcpCaptivePortalConfigWebWebBrandingImageNameValue);
  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigWebBrandingImageNameGet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebWebBrandingImageNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebWebBrandingImageNameValue,
      strlen (objcpCaptivePortalConfigWebWebBrandingImageNameValue));

  /* return the object value: cpCaptivePortalConfigWebWebBrandingImageName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalConfigWebWebBrandingImageNameValue,
      strlen (objcpCaptivePortalConfigWebWebBrandingImageNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebWebBrandingImageName
 *
 * @purpose Set 'cpCaptivePortalConfigWebWebBrandingImageName'
 *@description  [cpCaptivePortalConfigWebWebBrandingImageName]    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t
fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebWebBrandingImageName (void
    *wap,
    void
    *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebWebBrandingImageNameValue[CP_FILE_NAME_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalConfigWebWebBrandingImageName */
  owa.len = sizeof (objcpCaptivePortalConfigWebWebBrandingImageNameValue);
  memset(objcpCaptivePortalConfigWebWebBrandingImageNameValue, 0, owa.len);
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) objcpCaptivePortalConfigWebWebBrandingImageNameValue,
      &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebWebBrandingImageNameValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigWebBrandingImageNameSet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebWebBrandingImageNameValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_CP_FAILED_TO_SET_BRANDING_IMAGE_NAME;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebWebBrowserTitleText
 *
 * @purpose Get 'cpCaptivePortalConfigWebWebBrowserTitleText'
 *@description  [cpCaptivePortalConfigWebWebBrowserTitleText]    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebWebBrowserTitleText (void
    *wap,
    void
    *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebWebBrowserTitleTextValue[CP_BROWSER_TITLE_TEXT_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  FPOBJ_CLR_STR256(objcpCaptivePortalConfigWebWebBrowserTitleTextValue);
  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigWebBrowserTitleTextGet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebWebBrowserTitleTextValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebWebBrowserTitleTextValue,
      strlen (objcpCaptivePortalConfigWebWebBrowserTitleTextValue));

  /* return the object value: cpCaptivePortalConfigWebWebBrowserTitleText */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalConfigWebWebBrowserTitleTextValue,
      strlen (objcpCaptivePortalConfigWebWebBrowserTitleTextValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebWebBrowserTitleText
 *
 * @purpose Set 'cpCaptivePortalConfigWebWebBrowserTitleText'
 *@description  [cpCaptivePortalConfigWebWebBrowserTitleText]    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t
fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebWebBrowserTitleText (void
    *wap,
    void
    *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebWebBrowserTitleTextValue[CP_BROWSER_TITLE_TEXT_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalConfigWebWebBrowserTitleText */
  owa.len = sizeof (objcpCaptivePortalConfigWebWebBrowserTitleTextValue);
  memset(objcpCaptivePortalConfigWebWebBrowserTitleTextValue, 0, owa.len);
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) objcpCaptivePortalConfigWebWebBrowserTitleTextValue,
      &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebWebBrowserTitleTextValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigWebBrowserTitleTextSet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebWebBrowserTitleTextValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_CP_FAILED_TO_SET_BROWSER_TITLE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebButtonLabel
 *
 * @purpose Get 'cpCaptivePortalConfigWebButtonLabel'
 *@description  [cpCaptivePortalConfigWebButtonLabel]    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebButtonLabel (void
    *wap,
    void
    *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebButtonLabelValue[CP_BUTTON_LABEL_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  FPOBJ_CLR_STR256(objcpCaptivePortalConfigWebButtonLabelValue);
  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigWebButtonLabelGet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebButtonLabelValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebButtonLabelValue,
      strlen (objcpCaptivePortalConfigWebButtonLabelValue));

  /* return the object value: cpCaptivePortalConfigWebButtonLabel */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalConfigWebButtonLabelValue,
      strlen (objcpCaptivePortalConfigWebButtonLabelValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebButtonLabel
 *
 * @purpose Set 'cpCaptivePortalConfigWebButtonLabel'
 *@description  [cpCaptivePortalConfigWebButtonLabel]    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebButtonLabel (void
    *wap,
    void
    *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebButtonLabelValue[CP_BUTTON_LABEL_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalConfigWebButtonLabel */
  owa.len = sizeof (objcpCaptivePortalConfigWebButtonLabelValue);
  memset(objcpCaptivePortalConfigWebButtonLabelValue, 0, owa.len);
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) objcpCaptivePortalConfigWebButtonLabelValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebButtonLabelValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigWebButtonLabelSet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebButtonLabelValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_CP_FAILED_TO_SET_BUTTON_LABEL;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebDeniedMsgText
 *
 * @purpose Get 'cpCaptivePortalConfigWebDeniedMsgText'
 *@description  [cpCaptivePortalConfigWebDeniedMsgText]    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebDeniedMsgText (void
    *wap,
    void
    *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebDeniedMsgTextValue[CP_MSG_TEXT_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  FPOBJ_CLR_STR256(objcpCaptivePortalConfigWebDeniedMsgTextValue);
  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigWebDeniedMsgTextGet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebDeniedMsgTextValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebDeniedMsgTextValue,
      strlen (objcpCaptivePortalConfigWebDeniedMsgTextValue));

  /* return the object value: cpCaptivePortalConfigWebDeniedMsgText */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalConfigWebDeniedMsgTextValue,
      strlen (objcpCaptivePortalConfigWebDeniedMsgTextValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebDeniedMsgText
 *
 * @purpose Set 'cpCaptivePortalConfigWebDeniedMsgText'
 *@description  [cpCaptivePortalConfigWebDeniedMsgText]    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebDeniedMsgText (void
    *wap,
    void
    *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebDeniedMsgTextValue[CP_MSG_TEXT_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalConfigWebDeniedMsgText */
  owa.len = sizeof (objcpCaptivePortalConfigWebDeniedMsgTextValue);
  memset(objcpCaptivePortalConfigWebDeniedMsgTextValue, 0, owa.len);
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) objcpCaptivePortalConfigWebDeniedMsgTextValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebDeniedMsgTextValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigWebDeniedMsgTextSet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebDeniedMsgTextValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_CP_FAILED_TO_SET_DENIED_MSG;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebFontList
 *
 * @purpose Get 'cpCaptivePortalConfigWebFontList'
 *@description  [cpCaptivePortalConfigWebFontList]    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebFontList (void *wap,
    void
    *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebFontListValue[CP_FONT_LIST_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  FPOBJ_CLR_STR256(objcpCaptivePortalConfigWebFontListValue);
  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigWebFontListGet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebFontListValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebFontListValue,
      strlen (objcpCaptivePortalConfigWebFontListValue));

  /* return the object value: cpCaptivePortalConfigWebFontList */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalConfigWebFontListValue,
      strlen (objcpCaptivePortalConfigWebFontListValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebFontList
 *
 * @purpose Set 'cpCaptivePortalConfigWebFontList'
 *@description  [cpCaptivePortalConfigWebFontList]    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebFontList (void *wap,
    void
    *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebFontListValue[CP_FONT_LIST_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalConfigWebFontList */
  owa.len = sizeof (objcpCaptivePortalConfigWebFontListValue);
  memset(objcpCaptivePortalConfigWebFontListValue, 0, owa.len);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objcpCaptivePortalConfigWebFontListValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebFontListValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigWebFontListSet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebFontListValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_CP_FAILED_TO_SET_FONT_LIST;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebInstructionalText
 *
 * @purpose Get 'cpCaptivePortalConfigWebInstructionalText'
 *@description  [cpCaptivePortalConfigWebInstructionalText]    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebInstructionalText (void *wap,
    void
    *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebInstructionalTextValue[CP_AUP_TEXT_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  FPOBJ_CLR_STR256(objcpCaptivePortalConfigWebInstructionalTextValue);
  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigWebInstructionalTextGet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebInstructionalTextValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebInstructionalTextValue,
      strlen (objcpCaptivePortalConfigWebInstructionalTextValue));

  /* return the object value: cpCaptivePortalConfigWebInstructionalText */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalConfigWebInstructionalTextValue,
      strlen (objcpCaptivePortalConfigWebInstructionalTextValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebInstructionalText
 *
 * @purpose Set 'cpCaptivePortalConfigWebInstructionalText'
 *@description  [cpCaptivePortalConfigWebInstructionalText]    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t
fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebInstructionalText (void *wap,
    void
    *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebInstructionalTextValue[CP_AUP_TEXT_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalConfigWebInstructionalText */
  owa.len = sizeof (objcpCaptivePortalConfigWebInstructionalTextValue);
  memset(objcpCaptivePortalConfigWebInstructionalTextValue, 0, owa.len);
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) objcpCaptivePortalConfigWebInstructionalTextValue,
      &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebInstructionalTextValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigWebInstructionalTextSet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebInstructionalTextValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_CP_FAILED_TO_SET_INSTRUCTIONAL_TEXT;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebNoAcceptMsgText
 *
 * @purpose Get 'cpCaptivePortalConfigWebNoAcceptMsgText'
 *@description  [cpCaptivePortalConfigWebNoAcceptMsgText]    
 * @notes       
 *
 * @return
 *******************************************************************************/
  xLibRC_t
fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebNoAcceptMsgText (void *wap,
    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebNoAcceptMsgTextValue[CP_AUP_TEXT_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  FPOBJ_CLR_STR256(objcpCaptivePortalConfigWebNoAcceptMsgTextValue);
  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigWebNoAcceptMsgTextGet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebNoAcceptMsgTextValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebNoAcceptMsgTextValue,
      strlen (objcpCaptivePortalConfigWebNoAcceptMsgTextValue));

  /* return the object value: cpCaptivePortalConfigWebNoAcceptMsgText */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalConfigWebNoAcceptMsgTextValue,
      strlen (objcpCaptivePortalConfigWebNoAcceptMsgTextValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebNoAcceptMsgText
 *
 * @purpose Set 'cpCaptivePortalConfigWebNoAcceptMsgText'
 *@description  [cpCaptivePortalConfigWebNoAcceptMsgText]    
 * @notes       
 *
 * @return
 *******************************************************************************/
  xLibRC_t
fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebNoAcceptMsgText (void *wap,
    void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebNoAcceptMsgTextValue[CP_AUP_TEXT_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalConfigWebNoAcceptMsgText */
  owa.len = sizeof (objcpCaptivePortalConfigWebNoAcceptMsgTextValue);
  memset(objcpCaptivePortalConfigWebNoAcceptMsgTextValue, 0, owa.len);
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) objcpCaptivePortalConfigWebNoAcceptMsgTextValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebNoAcceptMsgTextValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigWebNoAcceptMsgTextSet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebNoAcceptMsgTextValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_CP_FAILED_TO_SET_NO_ACCEPT_MSG;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebPasswordLabel
 *
 * @purpose Get 'cpCaptivePortalConfigWebPasswordLabel'
 *@description  [cpCaptivePortalConfigWebPasswordLabel]    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebPasswordLabel (void
    *wap,
    void
    *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebPasswordLabelValue[CP_PASSWORD_LABEL_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  FPOBJ_CLR_STR256(objcpCaptivePortalConfigWebPasswordLabelValue);
  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigWebPasswordLabelGet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebPasswordLabelValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebPasswordLabelValue,
      strlen (objcpCaptivePortalConfigWebPasswordLabelValue));

  /* return the object value: cpCaptivePortalConfigWebPasswordLabel */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalConfigWebPasswordLabelValue,
      strlen (objcpCaptivePortalConfigWebPasswordLabelValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebPasswordLabel
 *
 * @purpose Set 'cpCaptivePortalConfigWebPasswordLabel'
 *@description  [cpCaptivePortalConfigWebPasswordLabel]    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebPasswordLabel (void
    *wap,
    void
    *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebPasswordLabelValue[CP_PASSWORD_LABEL_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalConfigWebPasswordLabel */
  owa.len = sizeof (objcpCaptivePortalConfigWebPasswordLabelValue);
  memset(objcpCaptivePortalConfigWebPasswordLabelValue, 0, owa.len);
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) objcpCaptivePortalConfigWebPasswordLabelValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebPasswordLabelValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigWebPasswordLabelSet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebPasswordLabelValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_CP_FAILED_TO_SET_PASSWORD_LABEL;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebResourceMsgText
 *
 * @purpose Get 'cpCaptivePortalConfigWebResourceMsgText'
 *@description  [cpCaptivePortalConfigWebResourceMsgText]    
 * @notes       
 *
 * @return
 *******************************************************************************/
  xLibRC_t
fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebResourceMsgText (void *wap,
    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebResourceMsgTextValue[CP_MSG_TEXT_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  FPOBJ_CLR_STR256(objcpCaptivePortalConfigWebResourceMsgTextValue);
  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigWebResourceMsgTextGet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebResourceMsgTextValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebResourceMsgTextValue,
      strlen (objcpCaptivePortalConfigWebResourceMsgTextValue));

  /* return the object value: cpCaptivePortalConfigWebResourceMsgText */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalConfigWebResourceMsgTextValue,
      strlen (objcpCaptivePortalConfigWebResourceMsgTextValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebResourceMsgText
 *
 * @purpose Set 'cpCaptivePortalConfigWebResourceMsgText'
 *@description  [cpCaptivePortalConfigWebResourceMsgText]    
 * @notes       
 *
 * @return
 *******************************************************************************/
  xLibRC_t
fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebResourceMsgText (void *wap,
    void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebResourceMsgTextValue[CP_MSG_TEXT_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalConfigWebResourceMsgText */
  owa.len = sizeof (objcpCaptivePortalConfigWebResourceMsgTextValue);
  memset(objcpCaptivePortalConfigWebResourceMsgTextValue, 0, owa.len);
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) objcpCaptivePortalConfigWebResourceMsgTextValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebResourceMsgTextValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigWebResourceMsgTextSet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebResourceMsgTextValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_CP_FAILED_TO_SET_RESOURCE_TEXT;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebTimeoutMsgText
 *
 * @purpose Get 'cpCaptivePortalConfigWebTimeoutMsgText'
 *@description  [cpCaptivePortalConfigWebTimeoutMsgText]    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebTimeoutMsgText (void
    *wap,
    void
    *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebTimeoutMsgTextValue[CP_MSG_TEXT_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  FPOBJ_CLR_STR256(objcpCaptivePortalConfigWebTimeoutMsgTextValue);
  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigWebTimeoutMsgTextGet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebTimeoutMsgTextValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebTimeoutMsgTextValue,
      strlen (objcpCaptivePortalConfigWebTimeoutMsgTextValue));

  /* return the object value: cpCaptivePortalConfigWebTimeoutMsgText */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalConfigWebTimeoutMsgTextValue,
      strlen (objcpCaptivePortalConfigWebTimeoutMsgTextValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebTimeoutMsgText
 *
 * @purpose Set 'cpCaptivePortalConfigWebTimeoutMsgText'
 *@description  [cpCaptivePortalConfigWebTimeoutMsgText]    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebTimeoutMsgText (void
    *wap,
    void
    *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebTimeoutMsgTextValue[CP_MSG_TEXT_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalConfigWebTimeoutMsgText */
  owa.len = sizeof (objcpCaptivePortalConfigWebTimeoutMsgTextValue);
  memset(objcpCaptivePortalConfigWebTimeoutMsgTextValue, 0, owa.len);
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) objcpCaptivePortalConfigWebTimeoutMsgTextValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebTimeoutMsgTextValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigWebTimeoutMsgTextSet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebTimeoutMsgTextValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_CP_FAILED_TO_SET_TIMEOUT_MSG;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebTitleText
 *
 * @purpose Get 'cpCaptivePortalConfigWebTitleText'
 *@description  [cpCaptivePortalConfigWebTitleText]    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebTitleText (void
    *wap,
    void
    *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebTitleTextValue[CP_TITLE_TEXT_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  FPOBJ_CLR_STR256(objcpCaptivePortalConfigWebTitleTextValue);
  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigWebTitleTextGet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebTitleTextValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebTitleTextValue,
      strlen (objcpCaptivePortalConfigWebTitleTextValue));

  /* return the object value: cpCaptivePortalConfigWebTitleText */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalConfigWebTitleTextValue,
      strlen (objcpCaptivePortalConfigWebTitleTextValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebTitleText
 *
 * @purpose Set 'cpCaptivePortalConfigWebTitleText'
 *@description  [cpCaptivePortalConfigWebTitleText]    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebTitleText (void
    *wap,
    void
    *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebTitleTextValue[CP_TITLE_TEXT_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalConfigWebTitleText */
  owa.len = sizeof (objcpCaptivePortalConfigWebTitleTextValue);
  memset(objcpCaptivePortalConfigWebTitleTextValue, 0, owa.len);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objcpCaptivePortalConfigWebTitleTextValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebTitleTextValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigWebTitleTextSet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebTitleTextValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_CP_FAILED_TO_SET_PAGE_TITLE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebUserLabel
 *
 * @purpose Get 'cpCaptivePortalConfigWebUserLabel'
 *@description  [cpCaptivePortalConfigWebUserLabel]    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebUserLabel (void
    *wap,
    void
    *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebUserLabelValue[CP_USER_LABEL_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  FPOBJ_CLR_STR256(objcpCaptivePortalConfigWebUserLabelValue);
  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigWebUserLabelGet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebUserLabelValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebUserLabelValue,
      strlen (objcpCaptivePortalConfigWebUserLabelValue));

  /* return the object value: cpCaptivePortalConfigWebUserLabel */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalConfigWebUserLabelValue,
      strlen (objcpCaptivePortalConfigWebUserLabelValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebUserLabel
 *
 * @purpose Set 'cpCaptivePortalConfigWebUserLabel'
 *@description  [cpCaptivePortalConfigWebUserLabel]    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebUserLabel (void
    *wap,
    void
    *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebUserLabelValue[CP_USER_LABEL_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalConfigWebUserLabel */
  owa.len = sizeof (objcpCaptivePortalConfigWebUserLabelValue);
  memset(objcpCaptivePortalConfigWebUserLabelValue, 0, owa.len);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objcpCaptivePortalConfigWebUserLabelValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebUserLabelValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigWebUserLabelSet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebUserLabelValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_CP_FAILED_TO_SET_USER_LABEL;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebWelcomeText
 *
 * @purpose Get 'cpCaptivePortalConfigWebWelcomeText'
 *@description  [cpCaptivePortalConfigWebWelcomeText]    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebWelcomeText (void
    *wap,
    void
    *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebWelcomeTextValue[CP_WELCOME_TEXT_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  FPOBJ_CLR_STR256(objcpCaptivePortalConfigWebWelcomeTextValue);
  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigWebWelcomeTextGet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebWelcomeTextValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebWelcomeTextValue,
      strlen (objcpCaptivePortalConfigWebWelcomeTextValue));

  /* return the object value: cpCaptivePortalConfigWebWelcomeText */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalConfigWebWelcomeTextValue,
      strlen (objcpCaptivePortalConfigWebWelcomeTextValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebWelcomeText
 *
 * @purpose Set 'cpCaptivePortalConfigWebWelcomeText'
 *@description  [cpCaptivePortalConfigWebWelcomeText]    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebWelcomeText (void
    *wap,
    void
    *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebWelcomeTextValue[CP_WELCOME_TEXT_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalConfigWebWelcomeText */
  owa.len = sizeof (objcpCaptivePortalConfigWebWelcomeTextValue);
  memset(objcpCaptivePortalConfigWebWelcomeTextValue, 0, owa.len);
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) objcpCaptivePortalConfigWebWelcomeTextValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebWelcomeTextValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigWebWelcomeTextSet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebWelcomeTextValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_CP_FAILED_TO_SET_WELCOME_TEXT;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebWelcomeTitleText
 *
 * @purpose Get 'cpCaptivePortalConfigWebWelcomeTitleText'
 *@description  [cpCaptivePortalConfigWebWelcomeTitleText]    
 * @notes       
 *
 * @return
 *******************************************************************************/
  xLibRC_t
fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebWelcomeTitleText (void *wap,
    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebWelcomeTitleTextValue[CP_WELCOME_TITLE_TEXT_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  FPOBJ_CLR_STR256(objcpCaptivePortalConfigWebWelcomeTitleTextValue);
  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigWebWelcomeTitleTextGet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebWelcomeTitleTextValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebWelcomeTitleTextValue,
      strlen (objcpCaptivePortalConfigWebWelcomeTitleTextValue));

  /* return the object value: cpCaptivePortalConfigWebWelcomeTitleText */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalConfigWebWelcomeTitleTextValue,
      strlen (objcpCaptivePortalConfigWebWelcomeTitleTextValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebWelcomeTitleText
 *
 * @purpose Set 'cpCaptivePortalConfigWebWelcomeTitleText'
 *@description  [cpCaptivePortalConfigWebWelcomeTitleText]    
 * @notes       
 *
 * @return
 *******************************************************************************/
  xLibRC_t
fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebWelcomeTitleText (void *wap,
    void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebWelcomeTitleTextValue[CP_WELCOME_TITLE_TEXT_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalConfigWebWelcomeTitleText */
  owa.len = sizeof (objcpCaptivePortalConfigWebWelcomeTitleTextValue);
  memset(objcpCaptivePortalConfigWebWelcomeTitleTextValue, 0, owa.len);
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) objcpCaptivePortalConfigWebWelcomeTitleTextValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebWelcomeTitleTextValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigWebWelcomeTitleTextSet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebWelcomeTitleTextValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_CP_FAILED_TO_SET_WELCOME_TITLE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebWipMsgText
 *
 * @purpose Get 'cpCaptivePortalConfigWebWipMsgText'
 *@description  [cpCaptivePortalConfigWebWipMsgText]    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebWipMsgText (void
    *wap,
    void
    *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebWipMsgTextValue[CP_AUP_TEXT_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  FPOBJ_CLR_STR256(objcpCaptivePortalConfigWebWipMsgTextValue);
  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigWebWipMsgTextGet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebWipMsgTextValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebWipMsgTextValue,
      strlen (objcpCaptivePortalConfigWebWipMsgTextValue));

  /* return the object value: cpCaptivePortalConfigWebWipMsgText */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalConfigWebWipMsgTextValue,
      strlen (objcpCaptivePortalConfigWebWipMsgTextValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebWipMsgText
 *
 * @purpose Set 'cpCaptivePortalConfigWebWipMsgText'
 *@description  [cpCaptivePortalConfigWebWipMsgText]    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalConfigWebWipMsgText (void
    *wap,
    void
    *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalConfigWebWipMsgTextValue[CP_AUP_TEXT_MAX+1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalConfigWebWipMsgText */
  owa.len = sizeof (objcpCaptivePortalConfigWebWipMsgTextValue);
  memset(objcpCaptivePortalConfigWebWipMsgTextValue, 0, owa.len);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objcpCaptivePortalConfigWebWipMsgTextValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigWebWipMsgTextValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalInstanceId,
      (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalConfigWeb_cpCaptivePortalWebId,
      (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigWebWipMsgTextSet (keycpCaptivePortalInstanceIdValue,
      keycpCaptivePortalWebIdValue,
      objcpCaptivePortalConfigWebWipMsgTextValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_CP_FAILED_TO_SET_WIP_MSG;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
