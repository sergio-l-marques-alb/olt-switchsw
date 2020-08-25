
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baseUsrMgrEnableAuthList.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  15 October 2008, Wednesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_baseUsrMgrEnableAuthList_obj.h"
#include "user_manager_exports.h"
#include "usmdb_user_mgmt_api.h"

/*******************************************************************************
* @function fpObjSet_baseUsrMgrEnableAuthList_CreateEnableAPLName
*
* @purpose Set 'CreateEnableAPLName'
 *@description  [CreateEnableAPLName] Create  enable Authentication profile list.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrEnableAuthList_CreateEnableAPLName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objCreateEnableAPLNameValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CreateEnableAPLName */
  owa.len = sizeof (objCreateEnableAPLNameValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objCreateEnableAPLNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objCreateEnableAPLNameValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbAPLCreate(ACCESS_LINE_CTS,
                         ACCESS_LEVEL_ENABLE,  objCreateEnableAPLNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrEnableAuthList_DefaultEnableConsoleAuthList
*
* @purpose Get 'DefaultEnableConsoleAuthList'
 *@description  [DefaultEnableConsoleAuthList] default enable console
* authentication list.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrEnableAuthList_DefaultEnableConsoleAuthList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objDefaultEnableConsoleAuthListValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbAPLLineGet(ACCESS_LINE_CONSOLE,
                         ACCESS_LEVEL_ENABLE,
                         objDefaultEnableConsoleAuthListValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objDefaultEnableConsoleAuthListValue,
                     strlen (objDefaultEnableConsoleAuthListValue));

  /* return the object value: DefaultEnableConsoleAuthList */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDefaultEnableConsoleAuthListValue,
                           strlen (objDefaultEnableConsoleAuthListValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_baseUsrMgrEnableAuthList_DefaultEnableConsoleAuthList
*
* @purpose List 'DefaultEnableConsoleAuthList'
 *@description  [DefaultEnableConsoleAuthList] default enable console
* authentication list.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseUsrMgrEnableAuthList_DefaultEnableConsoleAuthList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objDefaultEnableConsoleAuthListValue;
  xLibStr256_t nextObjDefaultEnableConsoleAuthListValue;

  FPOBJ_TRACE_ENTER (bufp);
  memset(objDefaultEnableConsoleAuthListValue,0x0,sizeof( objDefaultEnableConsoleAuthListValue));
  memset(nextObjDefaultEnableConsoleAuthListValue,0x0,sizeof(nextObjDefaultEnableConsoleAuthListValue));
  owa.len = sizeof (objDefaultEnableConsoleAuthListValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrEnableAuthList_DefaultEnableConsoleAuthList,
                          (xLibU8_t *) objDefaultEnableConsoleAuthListValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbAPLListGetFirst(ACCESS_LINE_CONSOLE,
                         ACCESS_LEVEL_ENABLE,
                         nextObjDefaultEnableConsoleAuthListValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objDefaultEnableConsoleAuthListValue, owa.len);
    owa.l7rc =
      usmDbAPLListGetNext(ACCESS_LINE_CONSOLE,
                         ACCESS_LEVEL_ENABLE,
                         objDefaultEnableConsoleAuthListValue,
                         nextObjDefaultEnableConsoleAuthListValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjDefaultEnableConsoleAuthListValue, owa.len);

  /* return the object value: DefaultEnableConsoleAuthList */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjDefaultEnableConsoleAuthListValue,
                           strlen (nextObjDefaultEnableConsoleAuthListValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseUsrMgrEnableAuthList_DefaultEnableConsoleAuthList
*
* @purpose Set 'DefaultEnableConsoleAuthList'
 *@description  [DefaultEnableConsoleAuthList] default enable console
* authentication list.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrEnableAuthList_DefaultEnableConsoleAuthList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objDefaultEnableConsoleAuthListValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DefaultEnableConsoleAuthList */
  owa.len = sizeof (objDefaultEnableConsoleAuthListValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objDefaultEnableConsoleAuthListValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objDefaultEnableConsoleAuthListValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbAPLLineSet(ACCESS_LINE_CONSOLE,
                         ACCESS_LEVEL_ENABLE,
                         objDefaultEnableConsoleAuthListValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    if (owa.l7rc == L7_DEPENDENCY_NOT_MET)
       owa.rc = XLIBRC_AUTHLIST_CONSOLE_ENABLE_ERROR;
    else
       owa.rc = XLIBRC_FAILURE;   
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrEnableAuthList_DefaultEnableTelnetAuthList
*
* @purpose Get 'DefaultEnableTelnetAuthList'
 *@description  [DefaultEnableTelnetAuthList] default enable telnet
* authentication list.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrEnableAuthList_DefaultEnableTelnetAuthList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objDefaultEnableTelnetAuthListValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbAPLLineGet(ACCESS_LINE_TELNET,
                         ACCESS_LEVEL_ENABLE,
                         objDefaultEnableTelnetAuthListValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objDefaultEnableTelnetAuthListValue,
                     strlen (objDefaultEnableTelnetAuthListValue));

  /* return the object value: DefaultEnableTelnetAuthList */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDefaultEnableTelnetAuthListValue,
                           strlen (objDefaultEnableTelnetAuthListValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_baseUsrMgrEnableAuthList_DefaultEnableTelnetAuthList
*
* @purpose List 'DefaultEnableTelnetAuthList'
 *@description  [DefaultEnableTelnetAuthList] default enable telnet
* authentication list.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseUsrMgrEnableAuthList_DefaultEnableTelnetAuthList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objDefaultEnableTelnetAuthListValue;
  xLibStr256_t nextObjDefaultEnableTelnetAuthListValue;

  FPOBJ_TRACE_ENTER (bufp);
  memset(objDefaultEnableTelnetAuthListValue,0x0,sizeof(objDefaultEnableTelnetAuthListValue));
  memset(nextObjDefaultEnableTelnetAuthListValue,0x0,sizeof(nextObjDefaultEnableTelnetAuthListValue));
  owa.len = sizeof (objDefaultEnableTelnetAuthListValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrEnableAuthList_DefaultEnableTelnetAuthList,
                          (xLibU8_t *) objDefaultEnableTelnetAuthListValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbAPLListGetFirst(ACCESS_LINE_TELNET,
                         ACCESS_LEVEL_ENABLE,
                         nextObjDefaultEnableTelnetAuthListValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objDefaultEnableTelnetAuthListValue, owa.len);
    owa.l7rc =
      usmDbAPLListGetNext(ACCESS_LINE_TELNET,
                         ACCESS_LEVEL_ENABLE,
                         objDefaultEnableTelnetAuthListValue,
                         nextObjDefaultEnableTelnetAuthListValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjDefaultEnableTelnetAuthListValue, owa.len);

  /* return the object value: DefaultEnableTelnetAuthList */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjDefaultEnableTelnetAuthListValue,
                           strlen (nextObjDefaultEnableTelnetAuthListValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseUsrMgrEnableAuthList_DefaultEnableTelnetAuthList
*
* @purpose Set 'DefaultEnableTelnetAuthList'
 *@description  [DefaultEnableTelnetAuthList] default enable telnet
* authentication list.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrEnableAuthList_DefaultEnableTelnetAuthList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objDefaultEnableTelnetAuthListValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DefaultEnableTelnetAuthList */
  owa.len = sizeof (objDefaultEnableTelnetAuthListValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objDefaultEnableTelnetAuthListValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objDefaultEnableTelnetAuthListValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbAPLLineSet(ACCESS_LINE_TELNET,
                         ACCESS_LEVEL_ENABLE,
                         objDefaultEnableTelnetAuthListValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrEnableAuthList_DefaultEnableSSHAuthList
*
* @purpose Get 'DefaultEnableSSHAuthList'
 *@description  [DefaultEnableSSHAuthList] default enable SSH authentication
* list.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrEnableAuthList_DefaultEnableSSHAuthList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objDefaultEnableSSHAuthListValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbAPLLineGet(ACCESS_LINE_SSH,
                         ACCESS_LEVEL_ENABLE,
                         objDefaultEnableSSHAuthListValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objDefaultEnableSSHAuthListValue,
                     strlen (objDefaultEnableSSHAuthListValue));

  /* return the object value: DefaultEnableSSHAuthList */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDefaultEnableSSHAuthListValue,
                           strlen (objDefaultEnableSSHAuthListValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_baseUsrMgrEnableAuthList_DefaultEnableSSHAuthList
*
* @purpose List 'DefaultEnableSSHAuthList'
 *@description  [DefaultEnableSSHAuthList] default enable SSH authentication
* list.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseUsrMgrEnableAuthList_DefaultEnableSSHAuthList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objDefaultEnableSSHAuthListValue;
  xLibStr256_t nextObjDefaultEnableSSHAuthListValue;

  FPOBJ_TRACE_ENTER (bufp);
  memset(objDefaultEnableSSHAuthListValue,0x0,sizeof(objDefaultEnableSSHAuthListValue));
  memset(nextObjDefaultEnableSSHAuthListValue,0x0,sizeof(nextObjDefaultEnableSSHAuthListValue));
  owa.len = sizeof (objDefaultEnableSSHAuthListValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrEnableAuthList_DefaultEnableSSHAuthList,
                          (xLibU8_t *) objDefaultEnableSSHAuthListValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbAPLListGetFirst(ACCESS_LINE_SSH,
                         ACCESS_LEVEL_ENABLE,
                         nextObjDefaultEnableSSHAuthListValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objDefaultEnableSSHAuthListValue, owa.len);
    owa.l7rc =
      usmDbAPLListGetNext(ACCESS_LINE_SSH,
                         ACCESS_LEVEL_ENABLE,
                         objDefaultEnableSSHAuthListValue,
                           nextObjDefaultEnableSSHAuthListValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjDefaultEnableSSHAuthListValue, owa.len);

  /* return the object value: DefaultEnableSSHAuthList */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjDefaultEnableSSHAuthListValue,
                           strlen (nextObjDefaultEnableSSHAuthListValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseUsrMgrEnableAuthList_DefaultEnableSSHAuthList
*
* @purpose Set 'DefaultEnableSSHAuthList'
 *@description  [DefaultEnableSSHAuthList] default enable SSH authentication
* list.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrEnableAuthList_DefaultEnableSSHAuthList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objDefaultEnableSSHAuthListValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DefaultEnableSSHAuthList */
  owa.len = sizeof (objDefaultEnableSSHAuthListValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objDefaultEnableSSHAuthListValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objDefaultEnableSSHAuthListValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbAPLLineSet(ACCESS_LINE_SSH,
                         ACCESS_LEVEL_ENABLE,
                         objDefaultEnableSSHAuthListValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrEnableAuthList_EnableAPLName
*
* @purpose Get 'EnableAPLName'
 *@description  [EnableAPLName] The name of the enable Authentication profile
* list.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrEnableAuthList_EnableAPLName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objEnableAPLNameValue;
  xLibStr256_t nextObjEnableAPLNameValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: EnableAPLName */
  owa.len = sizeof (objEnableAPLNameValue);

  memset( objEnableAPLNameValue,0x00,sizeof(objEnableAPLNameValue));
  memset( nextObjEnableAPLNameValue,0x00,sizeof( nextObjEnableAPLNameValue));
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrEnableAuthList_EnableAPLName,
                          (xLibU8_t *) objEnableAPLNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbAPLListGetFirst(ACCESS_LINE_CTS,
                         ACCESS_LEVEL_ENABLE,
                         nextObjEnableAPLNameValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objEnableAPLNameValue, owa.len);
    owa.l7rc = usmDbAPLListGetNext(ACCESS_LINE_CTS,
                         ACCESS_LEVEL_ENABLE,
                         objEnableAPLNameValue,
                         nextObjEnableAPLNameValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjEnableAPLNameValue, owa.len);

  /* return the object value: EnableAPLName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjEnableAPLNameValue,
                           strlen (nextObjEnableAPLNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrEnableAuthList_AuthenticationListMethod1
*
* @purpose Get 'AuthenticationListMethod1'
*
* @description [AuthenticationListMethod1]: authentication method1 at the
*              specified index in the list 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrEnableAuthList_AuthenticationListMethod1 (void *wap,
                                                                void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAuthenticationListMethod1Value;

  xLibStr256_t keyEnableAPLNameValue;

  FPOBJ_TRACE_ENTER (bufp);
  memset(keyEnableAPLNameValue,0x00,sizeof(keyEnableAPLNameValue));

  /* retrieve key: EnableAPLName */
  owa.len = sizeof (keyEnableAPLNameValue);
  owa.rc = xLibFilterGet (wap,  XOBJ_baseUsrMgrEnableAuthList_EnableAPLName,
                          (xLibU8_t *) keyEnableAPLNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyEnableAPLNameValue, owa.len);

/* get the value from application */
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_CTS,
                                             ACCESS_LEVEL_ENABLE,
                                             keyEnableAPLNameValue,
                                             0,
                                             &objAuthenticationListMethod1Value);



  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AuthenticationListMethod1 */
  owa.len = sizeof (xLibU32_t);
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objAuthenticationListMethod1Value,
                    sizeof (objAuthenticationListMethod1Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseUsrMgrEnableAuthList_AuthenticationListMethod1
*
* @purpose Set 'AuthenticationListMethod1'
*
* @description [AuthenticationListMethod1]: authentication method1 at the
*              specified index in the list 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrEnableAuthList_AuthenticationListMethod1 (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAuthenticationListMethod1Value;

  xLibStr256_t keyEnableAPLNameValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AuthenticationListMethod1 */
  owa.len = sizeof (keyEnableAPLNameValue);
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objAuthenticationListMethod1Value,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAuthenticationListMethod1Value, owa.len);



  memset(keyEnableAPLNameValue,0x00,sizeof(keyEnableAPLNameValue));

  /* retrieve key: EnableAPLName */
  owa.len = sizeof (keyEnableAPLNameValue);
  owa.rc = xLibFilterGet (wap,  XOBJ_baseUsrMgrEnableAuthList_EnableAPLName,
                          (xLibU8_t *) keyEnableAPLNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyEnableAPLNameValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_CTS,
                                             ACCESS_LEVEL_ENABLE,
                                             keyEnableAPLNameValue,
                                          0,
                                           objAuthenticationListMethod1Value);



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
* @function fpObjGet_baseUsrMgrEnableAuthList_AuthenticationListMethod2
*
* @purpose Get 'AuthenticationListMethod2'
*
* @description [AuthenticationListMethod2]: authentication method2 at the
*              specified index in the list 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrEnableAuthList_AuthenticationListMethod2 (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAuthenticationListMethod2Value;

  xLibStr256_t keyEnableAPLNameValue;

  FPOBJ_TRACE_ENTER (bufp);
  memset(keyEnableAPLNameValue,0x00,sizeof(keyEnableAPLNameValue));

  /* retrieve key: EnableAPLName */
  owa.len = sizeof (keyEnableAPLNameValue);
  owa.rc = xLibFilterGet (wap,  XOBJ_baseUsrMgrEnableAuthList_EnableAPLName,
                          (xLibU8_t *) keyEnableAPLNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyEnableAPLNameValue, owa.len);

/* get the value from application */
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_CTS,
                                             ACCESS_LEVEL_ENABLE,
                                             keyEnableAPLNameValue,
                                             1,
                                             &objAuthenticationListMethod2Value);



  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AuthenticationListMethod1 */
  owa.len = sizeof (xLibU32_t);
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objAuthenticationListMethod2Value,
                    sizeof (objAuthenticationListMethod2Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}



/*******************************************************************************
* @function fpObjSet_baseUsrMgrEnableAuthList_AuthenticationListMethod2
*
* @purpose Set 'AuthenticationListMethod2'
*
* @description [AuthenticationListMethod2]: authentication method2 at the
*              specified index in the list 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrEnableAuthList_AuthenticationListMethod2 (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAuthenticationListMethod2Value;
  xLibU32_t objAuthenticationListMethod1Value;

  xLibStr256_t keyEnableAPLNameValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AuthenticationListMethod1 */
  owa.len = sizeof (keyEnableAPLNameValue);
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objAuthenticationListMethod2Value,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAuthenticationListMethod2Value, owa.len);



  memset(keyEnableAPLNameValue,0x00,sizeof(keyEnableAPLNameValue));

  /* retrieve key: EnableAPLName */
  owa.len = sizeof (keyEnableAPLNameValue);
  owa.rc = xLibFilterGet (wap,  XOBJ_baseUsrMgrEnableAuthList_EnableAPLName,
                          (xLibU8_t *) keyEnableAPLNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyEnableAPLNameValue, owa.len);

  /* get the method1 value from application */
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_CTS,
                                             ACCESS_LEVEL_ENABLE,
                                             keyEnableAPLNameValue,
                                             0,
                                             &objAuthenticationListMethod1Value);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  if(objAuthenticationListMethod1Value == L7_AUTH_METHOD_UNDEFINED)
  {
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_CTS,
                                             ACCESS_LEVEL_ENABLE,
                                             keyEnableAPLNameValue,
                                          1,
                                           objAuthenticationListMethod1Value);
  }
  else
  {
    /* set the value in application */
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_CTS,
                                             ACCESS_LEVEL_ENABLE,
                                             keyEnableAPLNameValue,
                                          1,
                                           objAuthenticationListMethod2Value);

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


/*******************************************************************************
* @function fpObjGet_baseUsrMgrEnableAuthList_AuthenticationListMethod3
*
* @purpose Get 'AuthenticationListMethod3'
*
* @description [AuthenticationListMethod3]: authentication method3 at the
*              specified index in the list 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrEnableAuthList_AuthenticationListMethod3 (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAuthenticationListMethod3Value;

  xLibStr256_t keyEnableAPLNameValue;

  FPOBJ_TRACE_ENTER (bufp);
  memset(keyEnableAPLNameValue,0x00,sizeof(keyEnableAPLNameValue));

  /* retrieve key: EnableAPLName */
  owa.len = sizeof (keyEnableAPLNameValue);
  owa.rc = xLibFilterGet (wap,  XOBJ_baseUsrMgrEnableAuthList_EnableAPLName,
                          (xLibU8_t *) keyEnableAPLNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyEnableAPLNameValue, owa.len);

/* get the value from application */
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_CTS,
                                             ACCESS_LEVEL_ENABLE,
                                             keyEnableAPLNameValue,
                                             2,
                                             &objAuthenticationListMethod3Value);



  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AuthenticationListMethod1 */
  owa.len = sizeof (xLibU32_t);
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objAuthenticationListMethod3Value,
                    sizeof (objAuthenticationListMethod3Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}



/*******************************************************************************
* @function fpObjSet_baseUsrMgrEnableAuthList_AuthenticationListMethod3
*
* @purpose Set 'AuthenticationListMethod3'
*
* @description [AuthenticationListMethod3]: authentication method3 at the
*              specified index in the list 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrEnableAuthList_AuthenticationListMethod3 (void *wap,
                                                                void *bufp)
{
 
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAuthenticationListMethod3Value;
  xLibU32_t objAuthenticationListMethod2Value;

  xLibStr256_t keyEnableAPLNameValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AuthenticationListMethod1 */
  owa.len = sizeof (keyEnableAPLNameValue);
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objAuthenticationListMethod3Value,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAuthenticationListMethod3Value, owa.len);



  memset(keyEnableAPLNameValue,0x00,sizeof(keyEnableAPLNameValue));

  /* retrieve key: EnableAPLName */
  owa.len = sizeof (keyEnableAPLNameValue);
  owa.rc = xLibFilterGet (wap,  XOBJ_baseUsrMgrEnableAuthList_EnableAPLName,
                          (xLibU8_t *) keyEnableAPLNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyEnableAPLNameValue, owa.len);

  /* get the method2 value from application */
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_CTS,
                                             ACCESS_LEVEL_ENABLE,
                                             keyEnableAPLNameValue,
                                             1,
                                             &objAuthenticationListMethod2Value);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  if(objAuthenticationListMethod2Value == L7_AUTH_METHOD_UNDEFINED)
  {
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_CTS,
                                             ACCESS_LEVEL_ENABLE,
                                             keyEnableAPLNameValue,
                                          2,
                                           objAuthenticationListMethod2Value);
  }
  else
  {
    /* set the value in application */
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_CTS,
                                             ACCESS_LEVEL_ENABLE,
                                             keyEnableAPLNameValue,
                                          2,
                                           objAuthenticationListMethod3Value);

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

/*******************************************************************************
* @function fpObjGet_baseUsrMgrEnableAuthList_AuthenticationListMethod4
*
* @purpose Get 'AuthenticationListMethod3'
*
* @description [AuthenticationListMethod3]: authentication method3 at the
*              specified index in the list 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrEnableAuthList_AuthenticationListMethod4 (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAuthenticationListMethod4Value;

  xLibStr256_t keyEnableAPLNameValue;

  FPOBJ_TRACE_ENTER (bufp);
  memset(keyEnableAPLNameValue,0x00,sizeof(keyEnableAPLNameValue));

  /* retrieve key: EnableAPLName */
  owa.len = sizeof (keyEnableAPLNameValue);
  owa.rc = xLibFilterGet (wap,  XOBJ_baseUsrMgrEnableAuthList_EnableAPLName,
                          (xLibU8_t *) keyEnableAPLNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyEnableAPLNameValue, owa.len);

/* get the value from application */
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_CTS,
                                             ACCESS_LEVEL_ENABLE,
                                             keyEnableAPLNameValue,
                                             3,
                                             &objAuthenticationListMethod4Value);



  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AuthenticationListMethod1 */
  owa.len = sizeof (xLibU32_t);
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objAuthenticationListMethod4Value,
                    sizeof (objAuthenticationListMethod4Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}



/*******************************************************************************
* @function fpObjSet_baseUsrMgrEnableAuthList_AuthenticationListMethod4
*
* @purpose Set 'AuthenticationListMethod3'
*
* @description [AuthenticationListMethod3]: authentication method3 at the
*              specified index in the list 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrEnableAuthList_AuthenticationListMethod4 (void *wap,
                                                                void *bufp)
{
 
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAuthenticationListMethod4Value;
  xLibU32_t objAuthenticationListMethod3Value;

  xLibStr256_t keyEnableAPLNameValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AuthenticationListMethod1 */
  owa.len = sizeof (keyEnableAPLNameValue);
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objAuthenticationListMethod4Value,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAuthenticationListMethod3Value, owa.len);



  memset(keyEnableAPLNameValue,0x00,sizeof(keyEnableAPLNameValue));

  /* retrieve key: EnableAPLName */
  owa.len = sizeof (keyEnableAPLNameValue);
  owa.rc = xLibFilterGet (wap,  XOBJ_baseUsrMgrEnableAuthList_EnableAPLName,
                          (xLibU8_t *) keyEnableAPLNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyEnableAPLNameValue, owa.len);

  /* get the method3 value from application */
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_CTS,
                                             ACCESS_LEVEL_ENABLE,
                                             keyEnableAPLNameValue,
                                             2,
                                             &objAuthenticationListMethod3Value);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  if(objAuthenticationListMethod3Value == L7_AUTH_METHOD_UNDEFINED)
  {
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_CTS,
                                             ACCESS_LEVEL_ENABLE,
                                             keyEnableAPLNameValue,
                                          3,
                                           objAuthenticationListMethod3Value);
  }
  else
  {
    /* set the value in application */
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_CTS,
                                             ACCESS_LEVEL_ENABLE,
                                             keyEnableAPLNameValue,
                                          3,
                                           objAuthenticationListMethod4Value);

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

/*******************************************************************************
* @function fpObjGet_baseUsrMgrEnableAuthList_AuthenticationListMethod5
*
* @purpose Get 'AuthenticationListMethod5'
*
* @description [AuthenticationListMethod5]: authentication method5 at the
*              specified index in the list 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrEnableAuthList_AuthenticationListMethod5 (void *wap,
                                                                void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAuthenticationListMethod5Value;

  xLibStr256_t keyEnableAPLNameValue;

  FPOBJ_TRACE_ENTER (bufp);
  memset(keyEnableAPLNameValue,0x00,sizeof(keyEnableAPLNameValue));

  /* retrieve key: EnableAPLName */
  owa.len = sizeof (keyEnableAPLNameValue);
  owa.rc = xLibFilterGet (wap,  XOBJ_baseUsrMgrEnableAuthList_EnableAPLName,
                          (xLibU8_t *) keyEnableAPLNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyEnableAPLNameValue, owa.len);

/* get the value from application */
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_CTS,
                                             ACCESS_LEVEL_ENABLE,
                                             keyEnableAPLNameValue,
                                             4,
                                             &objAuthenticationListMethod5Value);



  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AuthenticationListMethod1 */
  owa.len = sizeof (xLibU32_t);
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objAuthenticationListMethod5Value,
                    sizeof (objAuthenticationListMethod5Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseUsrMgrEnableAuthList_AuthenticationListMethod5
*
* @purpose Set 'AuthenticationListMethod1'
*
* @description [AuthenticationListMethod1]: authentication method1 at the
*              specified index in the list 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrEnableAuthList_AuthenticationListMethod5 (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAuthenticationListMethod4Value;
  xLibU32_t objAuthenticationListMethod5Value;

  xLibStr256_t keyEnableAPLNameValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AuthenticationListMethod5 */
  owa.len = sizeof (keyEnableAPLNameValue);
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objAuthenticationListMethod5Value,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAuthenticationListMethod5Value, owa.len);



  memset(keyEnableAPLNameValue,0x00,sizeof(keyEnableAPLNameValue));

  /* retrieve key: EnableAPLName */
  owa.len = sizeof (keyEnableAPLNameValue);
  owa.rc = xLibFilterGet (wap,  XOBJ_baseUsrMgrEnableAuthList_EnableAPLName,
                          (xLibU8_t *) keyEnableAPLNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyEnableAPLNameValue, owa.len);

  /* get the method4 value from application */
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_CTS,
                                             ACCESS_LEVEL_ENABLE,
                                             keyEnableAPLNameValue,
                                             3,
                                             &objAuthenticationListMethod4Value);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  if(objAuthenticationListMethod4Value == L7_AUTH_METHOD_UNDEFINED)
  {
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_CTS,
                                             ACCESS_LEVEL_ENABLE,
                                             keyEnableAPLNameValue,
                                          4,
                                           objAuthenticationListMethod4Value);
  }
  else
  {
    /* set the value in application */
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_CTS,
                                             ACCESS_LEVEL_ENABLE,
                                             keyEnableAPLNameValue,
                                          4,
                                           objAuthenticationListMethod5Value);
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

/*******************************************************************************
* @function fpObjSet_baseUsrMgrEnableAuthList_AuthenticationListStatus
*
* @purpose Set 'AuthenticationListStatus'
*
* @description [AuthenticationListStatus]: Status of given index row of APL
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrEnableAuthList_EnableAuthListRowStatus (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAuthenticationListStatusValue;
  xLibStr256_t objAuthProfileNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  xLibStr256_t tempAuthListName;
  memset(tempAuthListName,0x00,sizeof(tempAuthListName));

  /* retrieve object: AuthenticationListStatus */
  owa.len = sizeof(objAuthenticationListStatusValue);
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objAuthenticationListStatusValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAuthenticationListStatusValue, owa.len);

  /* call the usmdb only for add and delete */
  if (objAuthenticationListStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /* retrieve object: objAuthProfileNameValue */
    owa.len = sizeof(objAuthProfileNameValue);
    owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrEnableAuthList_EnableAPLName,
                             (xLibU8_t *) objAuthProfileNameValue,
                             &owa.len);
    if (owa.rc != XLIBRC_SUCCESS)
    {
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_VALUE (bufp, objAuthProfileNameValue, owa.len);
  
    owa.l7rc = usmDbAPLCreate(ACCESS_LINE_CTS,
                         ACCESS_LEVEL_ENABLE, 
                         objAuthProfileNameValue);

    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_AUTH_LIST_CREATE_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }


  }
  if (objAuthenticationListStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* retrieve object: objAuthProfileNameValue */
    owa.len = sizeof(objAuthProfileNameValue);
    owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrEnableAuthList_EnableAPLName,
                             (xLibU8_t *) objAuthProfileNameValue,
                             &owa.len);
    if (owa.rc != XLIBRC_SUCCESS)
    {

      owa.rc = XLIBRC_AUTH_LIST_DEL_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_VALUE (bufp, objAuthProfileNameValue, owa.len);
  

       /* get the value from application */

         owa.l7rc = usmDbAPLDelete (ACCESS_LINE_CTS,
                         ACCESS_LEVEL_ENABLE,
                         objAuthProfileNameValue);
   
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_AUTH_LIST_DEL_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  return XLIBRC_SUCCESS;
}





/*******************************************************************************
* @function fpObjGet_baseUsrMgrEnableAuthList_EnableAuthListRowStatus
*
* @purpose Get 'EnableAuthListRowStatus'
 *@description  [EnableAuthListRowStatus] Create or Delete the enable auth list.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrEnableAuthList_EnableAuthListRowStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objEnableAuthListRowStatusValue;

  xLibStr256_t keyEnableAPLNameValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: EnableAPLName */
  owa.len = sizeof (keyEnableAPLNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrEnableAuthList_EnableAPLName,
                          (xLibU8_t *) keyEnableAPLNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyEnableAPLNameValue, owa.len);

  /* get the value from application */
  objEnableAuthListRowStatusValue = L7_ROW_STATUS_ACTIVE;
  owa.l7rc = L7_SUCCESS;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objEnableAuthListRowStatusValue,
                     sizeof (objEnableAuthListRowStatusValue));

  /* return the object value: EnableAuthListRowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objEnableAuthListRowStatusValue,
                           sizeof (objEnableAuthListRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

#if 0

/*******************************************************************************
* @function fpObjSet_baseUsrMgrEnableAuthList_EnableAuthListRowStatus
*
* @purpose Set 'EnableAuthListRowStatus'
 *@description  [EnableAuthListRowStatus] Create or Delete the enable auth list.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrEnableAuthList_EnableAuthListRowStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objEnableAuthListRowStatusValue;

  xLibStr256_t keyEnableAPLNameValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: EnableAuthListRowStatus */
  owa.len = sizeof (objEnableAuthListRowStatusValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objEnableAuthListRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objEnableAuthListRowStatusValue, owa.len);

  /* retrieve key: EnableAPLName */
  owa.len = sizeof (keyEnableAPLNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrEnableAuthList_EnableAPLName,
                          (xLibU8_t *) keyEnableAPLNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyEnableAPLNameValue, owa.len);

  owa.l7rc = L7_SUCCESS;
  if (objEnableAuthListRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    owa.l7rc = usmDbAPLCreate(ACCESS_LINE_CTS,
                         ACCESS_LEVEL_ENABLE, 
                         keyEnableAPLNameValue);
  }
  else if (objEnableAuthListRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    owa.l7rc = usmDbAPLDelete (ACCESS_LINE_CTS,
                         ACCESS_LEVEL_ENABLE,
                         keyEnableAPLNameValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
#endif

/*******************************************************************************
* @function fpObjGet_baseUsrMgrEnableAuthList_EnableAuthMethodOrder
*
* @purpose Get 'EnableAuthMethodOrder'
 *@description  [EnableAuthMethodOrder] The order of the enable authentication
* method.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrEnableAuthList_EnableAuthMethodOrder (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objEnableAuthMethodOrderValue;
  xLibU32_t nextObjEnableAuthMethodOrderValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: EnableAuthMethodOrder */
  owa.len = sizeof (objEnableAuthMethodOrderValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrEnableAuthList_EnableAuthMethodOrder,
                          (xLibU8_t *) & objEnableAuthMethodOrderValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = L7_SUCCESS;
	nextObjEnableAuthMethodOrderValue = 0;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objEnableAuthMethodOrderValue, owa.len);

	if(objEnableAuthMethodOrderValue < L7_ENABLE_MAX_AUTH_METHODS)
	 {
	    nextObjEnableAuthMethodOrderValue = objEnableAuthMethodOrderValue + 1;
		owa.l7rc = L7_SUCCESS;
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

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjEnableAuthMethodOrderValue, owa.len);

  /* return the object value: EnableAuthMethodOrder */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjEnableAuthMethodOrderValue,
                           sizeof (nextObjEnableAuthMethodOrderValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrEnableAuthList_EnableAuthMethod
*
* @purpose Get 'EnableAuthMethod'
 *@description  [EnableAuthMethod] The enable authentication method being set .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrEnableAuthList_EnableAuthMethod (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objEnableAuthMethodValue;

  xLibStr256_t keyEnableAPLNameValue;
  xLibU32_t keyEnableAuthMethodOrderValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: EnableAPLName */
  owa.len = sizeof (keyEnableAPLNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrEnableAuthList_EnableAPLName,
                          (xLibU8_t *) keyEnableAPLNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyEnableAPLNameValue, owa.len);

  /* retrieve key: EnableAuthMethodOrder */
  owa.len = sizeof (keyEnableAuthMethodOrderValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrEnableAuthList_EnableAuthMethodOrder,
                          (xLibU8_t *) & keyEnableAuthMethodOrderValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyEnableAuthMethodOrderValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_CTS,
                                             ACCESS_LEVEL_ENABLE,
                                             keyEnableAPLNameValue,
                                             keyEnableAuthMethodOrderValue,
                                             &objEnableAuthMethodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objEnableAuthMethodValue, sizeof (objEnableAuthMethodValue));

  /* return the object value: EnableAuthMethod */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objEnableAuthMethodValue,
                           sizeof (objEnableAuthMethodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseUsrMgrEnableAuthList_EnableAuthMethod
*
* @purpose Set 'EnableAuthMethod'
 *@description  [EnableAuthMethod] The enable authentication method being set .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrEnableAuthList_EnableAuthMethod (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objEnableAuthMethodValue;

  xLibStr256_t keyEnableAPLNameValue;
  xLibU32_t keyEnableAuthMethodOrderValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: EnableAuthMethod */
  owa.len = sizeof (objEnableAuthMethodValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objEnableAuthMethodValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objEnableAuthMethodValue, owa.len);

  /* retrieve key: EnableAPLName */
  owa.len = sizeof (keyEnableAPLNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrEnableAuthList_EnableAPLName,
                          (xLibU8_t *) keyEnableAPLNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyEnableAPLNameValue, owa.len);

  /* retrieve key: EnableAuthMethodOrder */
  owa.len = sizeof (keyEnableAuthMethodOrderValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrEnableAuthList_EnableAuthMethodOrder,
                          (xLibU8_t *) & keyEnableAuthMethodOrderValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyEnableAuthMethodOrderValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_CTS,
                                             ACCESS_LEVEL_ENABLE,
                                             keyEnableAPLNameValue,
                                             keyEnableAuthMethodOrderValue,
                                             objEnableAuthMethodValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrEnableAuthList_MethodList
*
* @purpose Get 'MethodList'
*
* @description
*             
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrEnableAuthList_MethodList (void *wap,
                                                                void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objMethodListValue;

  xLibStr256_t keyEnableAPLNameValue;
  
  xLibU32_t temp,i;
  L7_BOOL flag = L7_FALSE;
  
  FPOBJ_TRACE_ENTER (bufp);
  memset(objMethodListValue,0x00,sizeof(objMethodListValue));

  FPOBJ_TRACE_ENTER (bufp);
  memset(keyEnableAPLNameValue,0x00,sizeof(keyEnableAPLNameValue));

  /* retrieve key: EnableAPLName */
  owa.len = sizeof (keyEnableAPLNameValue);
  owa.rc = xLibFilterGet (wap,  XOBJ_baseUsrMgrEnableAuthList_EnableAPLName,
                          (xLibU8_t *) keyEnableAPLNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyEnableAPLNameValue, owa.len);

  if (owa.l7rc == L7_SUCCESS)
  {
     for (i = 0; i < L7_MAX_ENABLEAUTH_METHODS; i++)
     {
      
     /* get the value from application */
       owa.l7rc = usmDbAPLAuthMethodGet (ACCESS_LINE_CTS,
                                                  ACCESS_LEVEL_ENABLE,
                                         keyEnableAPLNameValue,
                                         i,
                                         &temp);
       
       if (owa.l7rc == L7_SUCCESS)
       {
         switch (temp)        /* auth method */
         {
           case L7_AUTH_METHOD_ENABLE:
             if(flag == L7_TRUE )
             {
               osapiStrncat(objMethodListValue,",ENABLE",sizeof(objMethodListValue));
             }
             else
             {
               osapiStrncat(objMethodListValue,"ENABLE",sizeof(objMethodListValue));
             }
             break;

		   case L7_AUTH_METHOD_LINE:
             if(flag == L7_TRUE )
             {
               osapiStrncat(objMethodListValue,",LINE",sizeof(objMethodListValue));
             }
             else
             {
               osapiStrncat(objMethodListValue,"LINE",sizeof(objMethodListValue));
             }
             break;

           case L7_AUTH_METHOD_LOCAL:
             if(flag == L7_TRUE )
             {
               osapiStrncat(objMethodListValue,",LOCAL",sizeof(objMethodListValue));
             }
             else
             {
               osapiStrncat(objMethodListValue,"LOCAL",sizeof(objMethodListValue));
             }
             break;
			 
		   case L7_AUTH_METHOD_NONE:
             if(flag == L7_TRUE )
             {
               osapiStrncat(objMethodListValue,",NONE",sizeof(objMethodListValue));
             }
             else
             {
               osapiStrncat(objMethodListValue,"NONE",sizeof(objMethodListValue));
             }
             break;
			 	 
           case L7_AUTH_METHOD_RADIUS:
             if(flag == L7_TRUE ) 
             {
               osapiStrncat(objMethodListValue,",RADIUS",sizeof(objMethodListValue));
             }
             else
             {
               osapiStrncat(objMethodListValue,"RADIUS",sizeof(objMethodListValue));
             } 
             break;

           case L7_AUTH_METHOD_REJECT:
             if(flag == L7_TRUE )
             {
               osapiStrncat(objMethodListValue,",REJECT",sizeof(objMethodListValue));  
             }
             else
             {
               osapiStrncat(objMethodListValue,"REJECT",sizeof(objMethodListValue));  
             }
             break;

           case L7_AUTH_METHOD_TACACS:
             if(flag == L7_TRUE )
             {  
               osapiStrncat(objMethodListValue,",TACACS+",sizeof(objMethodListValue));
             }
             else
             {
               osapiStrncat(objMethodListValue,"TACACS+",sizeof(objMethodListValue));
             }
             break;

           default:
             break;
         }
         flag = L7_TRUE; 
       }
     }
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    flag = L7_FALSE;
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  
  /* return the object value: AuthListForDefaultUser */
  owa.rc =  xLibBufDataSet (bufp, (xLibU8_t *)  objMethodListValue,
                            strlen (objMethodListValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
