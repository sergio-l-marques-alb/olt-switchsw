
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_captiveportalcpCaptivePortalTable.c
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
#include "_xe_captiveportalcpCaptivePortalTable_obj.h"

#include "captive_portal_commdefs.h"
#include "usmdb_cpdm_api.h"
#include "usmdb_cpdm_connstatus_api.h"
#include "usmdb_cpdm_web_api.h"

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId
*
* @purpose Get 'cpCaptivePortalInstanceId'
 *@description  [cpCaptivePortalInstanceId] The identifier associated with this
* instance of captive portal.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId (void *wap,
                                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalInstanceIdValue;
  xLibU32_t nextObjcpCaptivePortalInstanceIdValue;
  xLibU16_t cpInstanceId;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (objcpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
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
* @function fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalWebId
*
* @purpose Get 'cpCaptivePortalWebId'
 *@description  [cpCaptivePortalWebId] Web ID.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalWebId (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalWebIdValue;
  xLibU32_t nextObjcpCaptivePortalWebIdValue;
  xLibU16_t cpWebId;

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
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
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalWebId,
                          (xLibU8_t *) & objcpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    FPOBJ_CLR_U32(objcpCaptivePortalWebIdValue);
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
* @function fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalConfigName
*
* @purpose Get 'cpCaptivePortalConfigName'
 *@description  [cpCaptivePortalConfigName] The name, in alpha-numeric
* characters, assigned to this captive portal configuration.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalConfigName (void *wap,
                                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objcpCaptivePortalConfigNameValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
                          (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigNameGet (keycpCaptivePortalInstanceIdValue,
                              objcpCaptivePortalConfigNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigNameValue,
                     strlen (objcpCaptivePortalConfigNameValue));

  /* return the object value: cpCaptivePortalConfigName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalConfigNameValue,
                           strlen (objcpCaptivePortalConfigNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalConfigName
*
* @purpose Set 'cpCaptivePortalConfigName'
 *@description  [cpCaptivePortalConfigName] The name, in alpha-numeric
* characters, assigned to this captive portal configuration.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalConfigName (void *wap,
                                                                               void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objcpCaptivePortalConfigNameValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalConfigName */
  owa.len = sizeof (objcpCaptivePortalConfigNameValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objcpCaptivePortalConfigNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalConfigNameValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
                          (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigNameSet (keycpCaptivePortalInstanceIdValue,
                              objcpCaptivePortalConfigNameValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalAdminMode
*
* @purpose Get 'cpCaptivePortalAdminMode'
 *@description  [cpCaptivePortalAdminMode] Enable or disable this instance of
* captive portal on the system.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalAdminMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalAdminModeValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
                          (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigModeGet (keycpCaptivePortalInstanceIdValue,
                              &objcpCaptivePortalAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalAdminModeValue,
                     sizeof (objcpCaptivePortalAdminModeValue));

  /* return the object value: cpCaptivePortalAdminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalAdminModeValue,
                           sizeof (objcpCaptivePortalAdminModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalAdminMode
*
* @purpose Set 'cpCaptivePortalAdminMode'
 *@description  [cpCaptivePortalAdminMode] Enable or disable this instance of
* captive portal on the system.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalAdminMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalAdminModeValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalAdminMode */
  owa.len = sizeof (objcpCaptivePortalAdminModeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcpCaptivePortalAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalAdminModeValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
                          (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigModeSet (keycpCaptivePortalInstanceIdValue,
                              objcpCaptivePortalAdminModeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalProtocolMode
*
* @purpose Get 'cpCaptivePortalProtocolMode'
 *@description  [cpCaptivePortalProtocolMode] The protocol mode to be used.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalProtocolMode (void *wap,
                                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalProtocolModeValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
                          (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigProtocolModeGet (keycpCaptivePortalInstanceIdValue,
                              &objcpCaptivePortalProtocolModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalProtocolModeValue,
                     sizeof (objcpCaptivePortalProtocolModeValue));

  /* return the object value: cpCaptivePortalProtocolMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalProtocolModeValue,
                           sizeof (objcpCaptivePortalProtocolModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalProtocolMode
*
* @purpose Set 'cpCaptivePortalProtocolMode'
 *@description  [cpCaptivePortalProtocolMode] The protocol mode to be used.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalProtocolMode (void *wap,
                                                                                 void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalProtocolModeValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU16_t next_cpId;
  xLibU32_t intfId;
  xLibU32_t nextIntfId;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalProtocolMode */
  owa.len = sizeof (objcpCaptivePortalProtocolModeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcpCaptivePortalProtocolModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalProtocolModeValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
                          (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigProtocolModeSet (keycpCaptivePortalInstanceIdValue,
                              objcpCaptivePortalProtocolModeValue);

  if (owa.l7rc == L7_SUCCESS)
  {
  next_cpId = 0;
  intfId = 0;
  nextIntfId = 0;
  while ((L7_SUCCESS == usmDbCpdmCPConfigIntIfNumNextGet(keycpCaptivePortalInstanceIdValue, intfId, &next_cpId, &nextIntfId)) && (keycpCaptivePortalInstanceIdValue == next_cpId))
    {
       usmDbCpdmClientConnStatusIntfDeleteAll(nextIntfId);
       intfId = nextIntfId;
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
* @function fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalVerificationMode
*
* @purpose Get 'cpCaptivePortalVerificationMode'
 *@description  [cpCaptivePortalVerificationMode] The type of user verification
* to perform.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalVerificationMode (void *wap,
                                                                                     void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalVerificationModeValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
                          (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigVerifyModeGet (keycpCaptivePortalInstanceIdValue,
                              &objcpCaptivePortalVerificationModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalVerificationModeValue,
                     sizeof (objcpCaptivePortalVerificationModeValue));

  /* return the object value: cpCaptivePortalVerificationMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalVerificationModeValue,
                           sizeof (objcpCaptivePortalVerificationModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalVerificationMode
*
* @purpose Set 'cpCaptivePortalVerificationMode'
 *@description  [cpCaptivePortalVerificationMode] The type of user verification
* to perform.   cpCaptivePortalUserGroup
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalVerificationMode (void *wap,
                                                                                     void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalVerificationModeValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU16_t next_cpId;
  xLibU32_t intfId;
  xLibU32_t nextIntfId;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalVerificationMode */
  owa.len = sizeof (objcpCaptivePortalVerificationModeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcpCaptivePortalVerificationModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalVerificationModeValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
                          (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigVerifyModeSet (keycpCaptivePortalInstanceIdValue,
                              objcpCaptivePortalVerificationModeValue);

  if (owa.l7rc == L7_SUCCESS)
  {
  next_cpId = 0;
  intfId = 0;
  nextIntfId = 0;
  while ((L7_SUCCESS == usmDbCpdmCPConfigIntIfNumNextGet(keycpCaptivePortalInstanceIdValue, intfId, &next_cpId, &nextIntfId)) && (keycpCaptivePortalInstanceIdValue == next_cpId))
    {
       usmDbCpdmClientConnStatusIntfDeleteAll(nextIntfId);
       intfId = nextIntfId;
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
* @function fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalLanguages
*
* @purpose Get 'cpCaptivePortalLanguages'
 *@description  [cpCaptivePortalLanguages] Number of languages configured
* for given CP configuration.
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalLanguages (void *wap,
                                                                                     void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalLanguagesValue=0;

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU16_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
                          (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* retrieve key: cpCaptivePortalWebId */
  owa.len = sizeof (keycpCaptivePortalWebIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalWebId,
                          (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* get the value from application */
  while (L7_SUCCESS == usmDbCpdmCPConfigWebIdNextGet(keycpCaptivePortalInstanceIdValue, keycpCaptivePortalWebIdValue, &keycpCaptivePortalWebIdValue))
  {
    objcpCaptivePortalLanguagesValue++;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalLanguagesValue,
                     sizeof (objcpCaptivePortalLanguagesValue));

  /* return the object value: cpCaptivePortalLanguagesMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalLanguagesValue,
                           sizeof (objcpCaptivePortalLanguagesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalUserGroup
*
* @purpose Get 'cpCaptivePortalUserGroup'
 *@description  [cpCaptivePortalUserGroup] The group ID for this captive portal
* user database.A group ID may be assigned for databases which use
* verification mode LOCAL.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalUserGroup (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU16_t objcpCaptivePortalUserGroupValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t objcpCaptivePortalVerificationModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
                          (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  if ((usmDbCpdmCPConfigVerifyModeGet (keycpCaptivePortalInstanceIdValue,
                    &objcpCaptivePortalVerificationModeValue) == L7_SUCCESS)
                  && (objcpCaptivePortalVerificationModeValue == CP_VERIFY_MODE_LOCAL))
      {
        owa.l7rc = usmDbCpdmCPConfigGpIdGet (keycpCaptivePortalInstanceIdValue,
                        &objcpCaptivePortalUserGroupValue);
      }
  else
      {
        objcpCaptivePortalUserGroupValue = 0;
      }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalUserGroupValue,
                     sizeof (objcpCaptivePortalUserGroupValue));

  /* return the object value: cpCaptivePortalUserGroup */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalUserGroupValue,
                           sizeof (objcpCaptivePortalUserGroupValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalUserGroup
*
* @purpose Set 'cpCaptivePortalUserGroup'
 *@description  [cpCaptivePortalUserGroup] The group ID for this captive portal
* user database.A group ID may be assigned for databases which use
* verification mode LOCAL.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalUserGroup (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalUserGroupValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t objcpCaptivePortalVerificationModeValue;
  xLibU16_t next_cpId;
  xLibU32_t intfId;
  xLibU32_t nextIntfId;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalUserGroup */
  owa.len = sizeof (objcpCaptivePortalUserGroupValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcpCaptivePortalUserGroupValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalUserGroupValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
                          (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  if ((usmDbCpdmCPConfigVerifyModeGet (keycpCaptivePortalInstanceIdValue,
                              &objcpCaptivePortalVerificationModeValue) != L7_SUCCESS)
                  || (objcpCaptivePortalVerificationModeValue == CP_VERIFY_MODE_GUEST))
      {
        owa.rc = XLIBRC_CP_UNABLE_TO_SET_GROUP;
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }

  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigGpIdSet (keycpCaptivePortalInstanceIdValue,
                  objcpCaptivePortalUserGroupValue);

  if (owa.l7rc == L7_SUCCESS)
  {
  next_cpId = 0;
  intfId = 0;
  nextIntfId = 0;
  while ((L7_SUCCESS == usmDbCpdmCPConfigIntIfNumNextGet(keycpCaptivePortalInstanceIdValue, intfId, &next_cpId, &nextIntfId)) && (keycpCaptivePortalInstanceIdValue == next_cpId))
    {
       usmDbCpdmClientConnStatusIntfDeleteAll(nextIntfId);
       intfId = nextIntfId;
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
* @function fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalURLRedirectMode
*
* @purpose Get 'cpCaptivePortalURLRedirectMode'
 *@description  [cpCaptivePortalURLRedirectMode] Whether the captive portal
* should redirect newly authenticated clients to the configured URL. If
* this mode is disabled, the default locale specific welcome page
* is used. If enabled, the client is redirected to a configured URL.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalURLRedirectMode (void *wap,
                                                                                    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalURLRedirectModeValue;
  xLibU8_t cpCaptivePortalURLRedirectModeValue;
  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
                          (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigRedirectModeGet (keycpCaptivePortalInstanceIdValue,
                              &cpCaptivePortalURLRedirectModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objcpCaptivePortalURLRedirectModeValue = (xLibU32_t)cpCaptivePortalURLRedirectModeValue;

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalURLRedirectModeValue,
                     sizeof (objcpCaptivePortalURLRedirectModeValue));

  /* return the object value: cpCaptivePortalURLRedirectMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalURLRedirectModeValue,
                           sizeof (objcpCaptivePortalURLRedirectModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalURLRedirectMode
*
* @purpose Set 'cpCaptivePortalURLRedirectMode'
 *@description  [cpCaptivePortalURLRedirectMode] Whether the captive portal
* should redirect newly authenticated clients to the configured URL. If
* this mode is disabled, the default locale specific welcome page
* is used. If enabled, the client is redirected to a configured URL.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalURLRedirectMode (void *wap,
                                                                                    void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalURLRedirectModeValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalURLRedirectMode */
  owa.len = sizeof (objcpCaptivePortalURLRedirectModeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcpCaptivePortalURLRedirectModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalURLRedirectModeValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
                          (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigRedirectModeSet (keycpCaptivePortalInstanceIdValue,
                              objcpCaptivePortalURLRedirectModeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalRedirectURL
*
* @purpose Get 'cpCaptivePortalRedirectURL'
 *@description  [cpCaptivePortalRedirectURL] The URL to which the newly
* authenticated client is redirected if cpCaptivePortalURLRedirectMode is
* enabled.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalRedirectURL (void *wap,
                                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU8_t objcpCaptivePortalRedirectURLValue[CP_WELCOME_URL_MAX + 1];
  xLibU8_t tmp[CP_WELCOME_URL_MAX + 1];

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU8_t  objcpCaptivePortalURLRedirectModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
                          (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  owa.len = sizeof (objcpCaptivePortalURLRedirectModeValue);
  owa.l7rc = usmDbCpdmCPConfigRedirectModeGet(keycpCaptivePortalInstanceIdValue, 
                  &objcpCaptivePortalURLRedirectModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  memset(objcpCaptivePortalRedirectURLValue, 0 , sizeof(objcpCaptivePortalRedirectURLValue));
  memset(tmp, 0 ,sizeof(tmp));

  if (objcpCaptivePortalURLRedirectModeValue == L7_ENABLE)
  {
    owa.l7rc = usmDbCpdmCPConfigRedirectURLGet (keycpCaptivePortalInstanceIdValue, objcpCaptivePortalRedirectURLValue);
  }
  else
  {
    osapiStrncpySafe(objcpCaptivePortalRedirectURLValue, CP_URL_WELCOME_PATH, sizeof(objcpCaptivePortalRedirectURLValue));          
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalRedirectURLValue,
                     strlen (objcpCaptivePortalRedirectURLValue));

  /* return the object value: cpCaptivePortalRedirectURL */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalRedirectURLValue,
                           strlen (objcpCaptivePortalRedirectURLValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalRedirectURL
*
* @purpose Set 'cpCaptivePortalRedirectURL'
 *@description  [cpCaptivePortalRedirectURL] The URL to which the newly
* authenticated client is redirected if cpCaptivePortalURLRedirectMode is
* enabled.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalRedirectURL (void *wap,
                                                                                void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU8_t objcpCaptivePortalRedirectURLValue[CP_WELCOME_URL_MAX + 1];
  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU8_t  objcpCaptivePortalURLRedirectModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  memset(objcpCaptivePortalRedirectURLValue, 0, sizeof(objcpCaptivePortalRedirectURLValue));

  /* retrieve object: cpCaptivePortalRedirectURL */
  owa.len = sizeof (objcpCaptivePortalRedirectURLValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objcpCaptivePortalRedirectURLValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalRedirectURLValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
                          (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  if ((usmDbCpdmCPConfigRedirectModeGet(keycpCaptivePortalInstanceIdValue, 
                          &objcpCaptivePortalURLRedirectModeValue) != L7_SUCCESS)
                  || (objcpCaptivePortalURLRedirectModeValue != L7_ENABLE))
  {
    owa.rc = XLIBRC_CP_UNABLE_TO_SET_URL_MODE_NOT_ENABLED;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigRedirectURLSet (keycpCaptivePortalInstanceIdValue,
                              objcpCaptivePortalRedirectURLValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalSessionTimeout
*
* @purpose Get 'cpCaptivePortalSessionTimeout'
 *@description  [cpCaptivePortalSessionTimeout] The session timeout. After this
* limit has been reached the user will be disconnected. If this
* value is set to 0, no timeout is enforced.This value is represented
* in seconds.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalSessionTimeout (void *wap,
                                                                                   void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalSessionTimeoutValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
                          (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigSessionTimeoutGet (keycpCaptivePortalInstanceIdValue,
                              &objcpCaptivePortalSessionTimeoutValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalSessionTimeoutValue,
                     sizeof (objcpCaptivePortalSessionTimeoutValue));

  /* return the object value: cpCaptivePortalSessionTimeout */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalSessionTimeoutValue,
                           sizeof (objcpCaptivePortalSessionTimeoutValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalSessionTimeout
*
* @purpose Set 'cpCaptivePortalSessionTimeout'
 *@description  [cpCaptivePortalSessionTimeout] The session timeout. After this
* limit has been reached the user will be disconnected. If this
* value is set to 0, no timeout is enforced.This value is represented
* in seconds.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalSessionTimeout (void *wap,
                                                                                   void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalSessionTimeoutValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalSessionTimeout */
  owa.len = sizeof (objcpCaptivePortalSessionTimeoutValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcpCaptivePortalSessionTimeoutValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalSessionTimeoutValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
                          (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigSessionTimeoutSet (keycpCaptivePortalInstanceIdValue,
                              objcpCaptivePortalSessionTimeoutValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalIdleTimeout
*
* @purpose Get 'cpCaptivePortalIdleTimeout'
 *@description  [cpCaptivePortalIdleTimeout] The idle timeout of the session.
* After this limit has been reached the user will be disconnected. If
* this value is set to 0, no timeout is enforced.This value is
* represented in seconds.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalIdleTimeout (void *wap,
                                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalIdleTimeoutValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
                          (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigIdleTimeoutGet (keycpCaptivePortalInstanceIdValue,
                              &objcpCaptivePortalIdleTimeoutValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalIdleTimeoutValue,
                     sizeof (objcpCaptivePortalIdleTimeoutValue));

  /* return the object value: cpCaptivePortalIdleTimeout */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalIdleTimeoutValue,
                           sizeof (objcpCaptivePortalIdleTimeoutValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalIdleTimeout
*
* @purpose Set 'cpCaptivePortalIdleTimeout'
 *@description  [cpCaptivePortalIdleTimeout] The idle timeout of the session.
* After this limit has been reached the user will be disconnected. If
* this value is set to 0, no timeout is enforced.This value is
* represented in seconds.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalIdleTimeout (void *wap,
                                                                                void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalIdleTimeoutValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalIdleTimeout */
  owa.len = sizeof (objcpCaptivePortalIdleTimeoutValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcpCaptivePortalIdleTimeoutValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalIdleTimeoutValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
                          (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigIdleTimeoutSet (keycpCaptivePortalInstanceIdValue,
                              objcpCaptivePortalIdleTimeoutValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalRadiusAuthServer
*
* @purpose Get 'cpCaptivePortalRadiusAuthServer'
 *@description  [cpCaptivePortalRadiusAuthServer] The RADIUS authentication
* server name, in alpha-numeric characters (or dotted IP address),
* assigned to this captive portal configuration.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalRadiusAuthServer (void *wap,
                                                                                     void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objcpCaptivePortalRadiusAuthServerValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t objcpCaptivePortalVerificationModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
                          (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  if ((usmDbCpdmCPConfigVerifyModeGet(keycpCaptivePortalInstanceIdValue, 
                          &objcpCaptivePortalVerificationModeValue) != L7_SUCCESS)
                  || (objcpCaptivePortalVerificationModeValue != CP_VERIFY_MODE_RADIUS))
  {
    owa.rc = XLIBRC_CP_RADIUS_MODE_NOT_SET;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigRadiusAuthServerGet (keycpCaptivePortalInstanceIdValue,
                              objcpCaptivePortalRadiusAuthServerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalRadiusAuthServerValue,
                     strlen (objcpCaptivePortalRadiusAuthServerValue));

  /* return the object value: cpCaptivePortalRadiusAuthServer */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalRadiusAuthServerValue,
                           strlen (objcpCaptivePortalRadiusAuthServerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalRadiusAuthServer
*
* @purpose Set 'cpCaptivePortalRadiusAuthServer'
 *@description  [cpCaptivePortalRadiusAuthServer] The RADIUS authentication
* server name, in alpha-numeric characters (or dotted IP address),
* assigned to this captive portal configuration.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalRadiusAuthServer (void *wap,
                                                                                     void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objcpCaptivePortalRadiusAuthServerValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t objcpCaptivePortalVerificationModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalRadiusAuthServer */
  owa.len = sizeof (objcpCaptivePortalRadiusAuthServerValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objcpCaptivePortalRadiusAuthServerValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalRadiusAuthServerValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
                          (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  if(strcmp(objcpCaptivePortalRadiusAuthServerValue, "") == 0)
  {
    owa.rc = XLIBRC_CP_NO_RAD_SERVERS;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if ((usmDbCpdmCPConfigVerifyModeGet(keycpCaptivePortalInstanceIdValue, 
                          &objcpCaptivePortalVerificationModeValue) != L7_SUCCESS)
                  || (objcpCaptivePortalVerificationModeValue != CP_VERIFY_MODE_RADIUS))
  {
    owa.rc = XLIBRC_CP_RADIUS_MODE_NOT_SET;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigRadiusAuthServerSet (keycpCaptivePortalInstanceIdValue,
                              objcpCaptivePortalRadiusAuthServerValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalMaxBandwidthUp
*
* @purpose Get 'cpCaptivePortalMaxBandwidthUp'
 *@description  [cpCaptivePortalMaxBandwidthUp] The maximum bandwidth
* up/transmit bytes per second rate of the session.After this limit has been
* reached the user will be disconnected.If this value is set to 0,
* no rate limiting is enforced.This value is represented in bytes
* (however; converted and stored as bits/sec).   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalMaxBandwidthUp (void *wap,
                                                                                   void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalMaxBandwidthUpValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
                          (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigUserUpRateGet (keycpCaptivePortalInstanceIdValue,
                              &objcpCaptivePortalMaxBandwidthUpValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objcpCaptivePortalMaxBandwidthUpValue /= 8;

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalMaxBandwidthUpValue,
                     sizeof (objcpCaptivePortalMaxBandwidthUpValue));

  /* return the object value: cpCaptivePortalMaxBandwidthUp */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalMaxBandwidthUpValue,
                           sizeof (objcpCaptivePortalMaxBandwidthUpValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalMaxBandwidthUp
*
* @purpose Set 'cpCaptivePortalMaxBandwidthUp'
 *@description  [cpCaptivePortalMaxBandwidthUp] The maximum bandwidth
* up/transmit bytes per second rate of the session.After this limit has been
* reached the user will be disconnected.If this value is set to 0,
* no rate limiting is enforced.This value is represented in bytes
* (however; converted and stored as bits/sec).   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalMaxBandwidthUp (void *wap,
                                                                                   void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalMaxBandwidthUpValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalMaxBandwidthUp */
  owa.len = sizeof (objcpCaptivePortalMaxBandwidthUpValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcpCaptivePortalMaxBandwidthUpValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalMaxBandwidthUpValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
                          (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigUserUpRateSet (keycpCaptivePortalInstanceIdValue,
                              objcpCaptivePortalMaxBandwidthUpValue*8);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalMaxBandwidthDown
*
* @purpose Get 'cpCaptivePortalMaxBandwidthDown'
 *@description  [cpCaptivePortalMaxBandwidthDown] The maximum bandwidth
* down/receive bytes per second rate of the session.After this limit has
* been reached the user will be disconnected.If this value is set to
* 0, no rate limiting is enforced.This value is represented in
* bytes (however; converted and stored as bits/sec).   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalMaxBandwidthDown (void *wap,
                                                                                     void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalMaxBandwidthDownValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
                          (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigUserDownRateGet (keycpCaptivePortalInstanceIdValue,
                              &objcpCaptivePortalMaxBandwidthDownValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objcpCaptivePortalMaxBandwidthDownValue /= 8;

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalMaxBandwidthDownValue,
                     sizeof (objcpCaptivePortalMaxBandwidthDownValue));

  /* return the object value: cpCaptivePortalMaxBandwidthDown */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalMaxBandwidthDownValue,
                           sizeof (objcpCaptivePortalMaxBandwidthDownValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalMaxBandwidthDown
*
* @purpose Set 'cpCaptivePortalMaxBandwidthDown'
 *@description  [cpCaptivePortalMaxBandwidthDown] The maximum bandwidth
* down/receive bytes per second rate of the session.After this limit has
* been reached the user will be disconnected.If this value is set to
* 0, no rate limiting is enforced.This value is represented in
* bytes (however; converted and stored as bits/sec).   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalMaxBandwidthDown (void *wap,
                                                                                     void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalMaxBandwidthDownValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalMaxBandwidthDown */
  owa.len = sizeof (objcpCaptivePortalMaxBandwidthDownValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcpCaptivePortalMaxBandwidthDownValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalMaxBandwidthDownValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
                          (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigUserDownRateSet (keycpCaptivePortalInstanceIdValue,
                              objcpCaptivePortalMaxBandwidthDownValue*8);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalMaxInputOctets
*
* @purpose Get 'cpCaptivePortalMaxInputOctets'
 *@description  [cpCaptivePortalMaxInputOctets] The maximum input octets (bytes)
* allowed for the session.After this limit has been reached the
* user will be disconnected.If this value is set to 0, no rate
* limiting is enforced.This value is represented in bytes.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalMaxInputOctets (void *wap,
                                                                                   void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalMaxInputOctetsValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
                          (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigMaxInputOctetsGet (keycpCaptivePortalInstanceIdValue,
                              &objcpCaptivePortalMaxInputOctetsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalMaxInputOctetsValue,
                     sizeof (objcpCaptivePortalMaxInputOctetsValue));

  /* return the object value: cpCaptivePortalMaxInputOctets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalMaxInputOctetsValue,
                           sizeof (objcpCaptivePortalMaxInputOctetsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalMaxInputOctets
*
* @purpose Set 'cpCaptivePortalMaxInputOctets'
 *@description  [cpCaptivePortalMaxInputOctets] The maximum input octets (bytes)
* allowed for the session.After this limit has been reached the
* user will be disconnected.If this value is set to 0, no rate
* limiting is enforced.This value is represented in bytes.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalMaxInputOctets (void *wap,
                                                                                   void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalMaxInputOctetsValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalMaxInputOctets */
  owa.len = sizeof (objcpCaptivePortalMaxInputOctetsValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcpCaptivePortalMaxInputOctetsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalMaxInputOctetsValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
                          (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigMaxInputOctetsSet (keycpCaptivePortalInstanceIdValue,
                              objcpCaptivePortalMaxInputOctetsValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalMaxOutputOctets
*
* @purpose Get 'cpCaptivePortalMaxOutputOctets'
 *@description  [cpCaptivePortalMaxOutputOctets] The maximum Output octets
* (bytes) allowed for the session.After this limit has been reached the
* user will be disconnected.If this value is set to 0, no rate
* limiting is enforced.This value is represented in bytes.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalMaxOutputOctets (void *wap,
                                                                                    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalMaxOutputOctetsValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
                          (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigMaxOutputOctetsGet (keycpCaptivePortalInstanceIdValue,
                              &objcpCaptivePortalMaxOutputOctetsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalMaxOutputOctetsValue,
                     sizeof (objcpCaptivePortalMaxOutputOctetsValue));

  /* return the object value: cpCaptivePortalMaxOutputOctets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalMaxOutputOctetsValue,
                           sizeof (objcpCaptivePortalMaxOutputOctetsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalMaxOutputOctets
*
* @purpose Set 'cpCaptivePortalMaxOutputOctets'
 *@description  [cpCaptivePortalMaxOutputOctets] The maximum Output octets
* (bytes) allowed for the session.After this limit has been reached the
* user will be disconnected.If this value is set to 0, no rate
* limiting is enforced.This value is represented in bytes.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalMaxOutputOctets (void *wap,
                                                                                    void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalMaxOutputOctetsValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalMaxOutputOctets */
  owa.len = sizeof (objcpCaptivePortalMaxOutputOctetsValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcpCaptivePortalMaxOutputOctetsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalMaxOutputOctetsValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
                          (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigMaxOutputOctetsSet (keycpCaptivePortalInstanceIdValue,
                              objcpCaptivePortalMaxOutputOctetsValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalMaxTotalOctets
*
* @purpose Get 'cpCaptivePortalMaxTotalOctets'
 *@description  [cpCaptivePortalMaxTotalOctets] The maximum input/output octets
* (bytes) allowed for the session.After this limit has been reached
* the user will be disconnected.If this value is set to 0, no rate
* limiting is enforced.This value is represented in bytes.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalMaxTotalOctets (void *wap,
                                                                                   void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalMaxTotalOctetsValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
                          (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigMaxTotalOctetsGet (keycpCaptivePortalInstanceIdValue,
                              &objcpCaptivePortalMaxTotalOctetsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalMaxTotalOctetsValue,
                     sizeof (objcpCaptivePortalMaxTotalOctetsValue));

  /* return the object value: cpCaptivePortalMaxTotalOctets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalMaxTotalOctetsValue,
                           sizeof (objcpCaptivePortalMaxTotalOctetsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalMaxTotalOctets
*
* @purpose Set 'cpCaptivePortalMaxTotalOctets'
 *@description  [cpCaptivePortalMaxTotalOctets] The maximum input/output octets
* (bytes) allowed for the session.After this limit has been reached
* the user will be disconnected.If this value is set to 0, no rate
* limiting is enforced.This value is represented in bytes.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalMaxTotalOctets (void *wap,
                                                                                   void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalMaxTotalOctetsValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalMaxTotalOctets */
  owa.len = sizeof (objcpCaptivePortalMaxTotalOctetsValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcpCaptivePortalMaxTotalOctetsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalMaxTotalOctetsValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
                          (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigMaxTotalOctetsSet (keycpCaptivePortalInstanceIdValue,
                              objcpCaptivePortalMaxTotalOctetsValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalLangCode
*
* @purpose Get 'cpCaptivePortalLangCode'
 *@description  [cpCaptivePortalLangCode] Language Code.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalLangCode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalLangCodeValue[CP_LANG_CODE_MAX+1];

  xLibU16_t keycpCaptivePortalInstanceIdValue;
  xLibU16_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
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
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalWebId,
                          (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  FPOBJ_CLR_STR256(objcpCaptivePortalLangCodeValue);
  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigWebLangCodeGet (keycpCaptivePortalInstanceIdValue,
                              keycpCaptivePortalWebIdValue, objcpCaptivePortalLangCodeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalLangCodeValue,
                     strlen (objcpCaptivePortalLangCodeValue));

  /* return the object value: cpCaptivePortalLangCode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalLangCodeValue,
                           strlen (objcpCaptivePortalLangCodeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalLangCode
*
* @purpose Set 'cpCaptivePortalLangCode'
 *@description  [cpCaptivePortalLangCode] Language Code.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalLangCode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalLangCodeValue[CP_LANG_CODE_MAX+1];

  xLibU16_t keycpCaptivePortalInstanceIdValue;
  xLibU16_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalLangCode */
  owa.len = sizeof (objcpCaptivePortalLangCodeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objcpCaptivePortalLangCodeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalLangCodeValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
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
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalWebId,
                          (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigWebLangCodeSet (keycpCaptivePortalInstanceIdValue,
                              keycpCaptivePortalWebIdValue, objcpCaptivePortalLangCodeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalLocaleLink
*
* @purpose Get 'cpCaptivePortalLocaleLink'
 *@description  [cpCaptivePortalLocaleLink] Locale Link.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalLocaleLink (void *wap,
                                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalLocaleLinkValue[CP_LOCALE_LINK_MAX+1];

  xLibU16_t keycpCaptivePortalInstanceIdValue;
  xLibU16_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
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
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalWebId,
                          (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  FPOBJ_CLR_STR256(objcpCaptivePortalLocaleLinkValue);
  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigWebLocaleLinkGet (keycpCaptivePortalInstanceIdValue,
                              keycpCaptivePortalWebIdValue, objcpCaptivePortalLocaleLinkValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalLocaleLinkValue,
                     strlen (objcpCaptivePortalLocaleLinkValue));

  /* return the object value: cpCaptivePortalLocaleLink */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpCaptivePortalLocaleLinkValue,
                           strlen (objcpCaptivePortalLocaleLinkValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalLocaleLink
*
* @purpose Set 'cpCaptivePortalLocaleLink'
 *@description  [cpCaptivePortalLocaleLink] Locale Link.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalLocaleLink (void *wap,
                                                                               void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS8_t objcpCaptivePortalLocaleLinkValue[CP_LOCALE_LINK_MAX+1];

  xLibU16_t keycpCaptivePortalInstanceIdValue;
  xLibU16_t keycpCaptivePortalWebIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalLocaleLink */
  owa.len = sizeof (objcpCaptivePortalLocaleLinkValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objcpCaptivePortalLocaleLinkValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objcpCaptivePortalLocaleLinkValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
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
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalWebId,
                          (xLibU8_t *) & keycpCaptivePortalWebIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalWebIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbCpdmCPConfigWebLocaleLinkSet (keycpCaptivePortalInstanceIdValue,
                              keycpCaptivePortalWebIdValue, objcpCaptivePortalLocaleLinkValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalConfigReset
*
* @purpose Set 'cpCaptivePortalConfigReset'
 *@description  [cpCaptivePortalConfigReset] Locale Link.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalConfigReset (void *wap,
                                                                                void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalConfigResetValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalConfigReset */
  owa.len = sizeof (objcpCaptivePortalConfigResetValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcpCaptivePortalConfigResetValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalConfigResetValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
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
    owa.rc = XLIBRC_CP_FAILED_TO_CLR_CFG;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalRowStatus
*
* @purpose Get 'cpCaptivePortalRowStatus'
 *@description  [cpCaptivePortalRowStatus] The captive portal instance
* status.Supported values: active(1)      - valid instance     
* createAndGo(4) - creates a new instance     destroy(6)     - removes an
* instance   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalTable_cpCaptivePortalRowStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalRowStatusValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
                          (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmCPConfigGet (keycpCaptivePortalInstanceIdValue);
  objcpCaptivePortalRowStatusValue = ((owa.l7rc == L7_SUCCESS) ? L7_ROW_STATUS_ACTIVE:L7_ROW_STATUS_INVALID); 
  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalRowStatusValue,
                     sizeof (objcpCaptivePortalRowStatusValue));

  /* return the object value: cpCaptivePortalRowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalRowStatusValue,
                           sizeof (objcpCaptivePortalRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalRowStatus
*
* @purpose Set 'cpCaptivePortalRowStatus'
 *@description  [cpCaptivePortalRowStatus] The captive portal instance
* status.Supported values: active(1)      - valid instance     
* createAndGo(4) - creates a new instance     destroy(6)     - removes an
* instance   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpCaptivePortalTable_cpCaptivePortalRowStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalRowStatusValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t cpCaptivePortalInstanceIdValue;
  L7_RC_t rc;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpCaptivePortalRowStatus */
  owa.len = sizeof (objcpCaptivePortalRowStatusValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcpCaptivePortalRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalRowStatusValue, owa.len);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,
                          (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  owa.l7rc = L7_SUCCESS;
  rc = L7_SUCCESS;
  cpCaptivePortalInstanceIdValue = CP_ID_MIN;
  if (objcpCaptivePortalRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    while ((rc == L7_SUCCESS) && (cpCaptivePortalInstanceIdValue <= CP_ID_MAX))
        {
            rc = usmDbCpdmCPConfigGet(cpCaptivePortalInstanceIdValue);
            if (rc == L7_SUCCESS)
            cpCaptivePortalInstanceIdValue++;
        }
    
  owa.l7rc = usmDbCpdmCPConfigAdd (cpCaptivePortalInstanceIdValue);

  if (owa.l7rc != L7_SUCCESS)
    {
     owa.rc = XLIBRC_CP_FAILED_TO_ADD_CONFIG;
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
    }

  owa.rc = xLibFilterSet(wap,XOBJ_captiveportalcpCaptivePortalTable_cpCaptivePortalInstanceId,0,(xLibU8_t *) &cpCaptivePortalInstanceIdValue,sizeof(cpCaptivePortalInstanceIdValue));
  }
  else if (objcpCaptivePortalRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    if (CP_ID_MIN == keycpCaptivePortalInstanceIdValue)
    {
     owa.rc = XLIBRC_CP_FAILED_TO_DELETE_DEFAULT_CONFIG;
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
    }
    
    owa.l7rc = usmDbCpdmCPConfigDelete (keycpCaptivePortalInstanceIdValue);
    if (owa.l7rc != L7_SUCCESS)
    {
       owa.rc = XLIBRC_CP_FAILED_TO_DELETE_CONFIG;
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
    }
    
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
