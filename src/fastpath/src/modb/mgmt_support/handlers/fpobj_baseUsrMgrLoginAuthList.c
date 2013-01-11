
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baseUsrMgrLoginAuthList.c
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
#include "_xe_baseUsrMgrLoginAuthList_obj.h"
#include "user_manager_exports.h"
#include "usmdb_user_mgmt_api.h"

/*******************************************************************************
* @function fpObjSet_baseUsrMgrLoginAuthList_CreateLoginAPLName
*
* @purpose Set 'CreateLoginAPLName'
 *@description  [CreateLoginAPLName] Create  login Authentication profile list.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrLoginAuthList_CreateLoginAPLName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objCreateLoginAPLNameValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CreateLoginAPLName */
  owa.len = sizeof (objCreateLoginAPLNameValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objCreateLoginAPLNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objCreateLoginAPLNameValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
	
  owa.l7rc = usmDbAPLCreate(ACCESS_LINE_CTS,
                         ACCESS_LEVEL_LOGIN, 
                         objCreateLoginAPLNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrLoginAuthList_DefaultLoginConsoleAuthList
*
* @purpose Get 'DefaultLoginConsoleAuthList'
 *@description  [DefaultLoginConsoleAuthList] default login console
* authentication list.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrLoginAuthList_DefaultLoginConsoleAuthList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objDefaultLoginConsoleAuthListValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbAPLLineGet(ACCESS_LINE_CONSOLE,
                         ACCESS_LEVEL_LOGIN,
                        objDefaultLoginConsoleAuthListValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objDefaultLoginConsoleAuthListValue,
                     strlen (objDefaultLoginConsoleAuthListValue));

  /* return the object value: DefaultLoginConsoleAuthList */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDefaultLoginConsoleAuthListValue,
                           strlen (objDefaultLoginConsoleAuthListValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_baseUsrMgrLoginAuthList_DefaultLoginConsoleAuthList
*
* @purpose List 'DefaultLoginConsoleAuthList'
 *@description  [DefaultLoginConsoleAuthList] default login console
* authentication list.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseUsrMgrLoginAuthList_DefaultLoginConsoleAuthList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objDefaultLoginConsoleAuthListValue;
  xLibStr256_t nextObjDefaultLoginConsoleAuthListValue;

  FPOBJ_TRACE_ENTER (bufp);
  memset(objDefaultLoginConsoleAuthListValue, 0x00, sizeof(objDefaultLoginConsoleAuthListValue));
  memset(nextObjDefaultLoginConsoleAuthListValue, 0x00, sizeof(nextObjDefaultLoginConsoleAuthListValue));

  owa.len = sizeof (objDefaultLoginConsoleAuthListValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrLoginAuthList_DefaultLoginConsoleAuthList,
                          (xLibU8_t *) objDefaultLoginConsoleAuthListValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);	
    owa.l7rc = usmDbAPLListGetFirst(ACCESS_LINE_CONSOLE,
                         ACCESS_LEVEL_LOGIN,
                                        nextObjDefaultLoginConsoleAuthListValue );
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objDefaultLoginConsoleAuthListValue, owa.len);
    owa.l7rc =
      usmDbAPLListGetNext(ACCESS_LINE_CONSOLE,
                         ACCESS_LEVEL_LOGIN,
                         objDefaultLoginConsoleAuthListValue,
                         nextObjDefaultLoginConsoleAuthListValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjDefaultLoginConsoleAuthListValue, owa.len);

  /* return the object value: DefaultLoginConsoleAuthList */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjDefaultLoginConsoleAuthListValue,
                           strlen (nextObjDefaultLoginConsoleAuthListValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseUsrMgrLoginAuthList_DefaultLoginConsoleAuthList
*
* @purpose Set 'DefaultLoginConsoleAuthList'
 *@description  [DefaultLoginConsoleAuthList] default login console
* authentication list.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrLoginAuthList_DefaultLoginConsoleAuthList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objDefaultLoginConsoleAuthListValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DefaultLoginConsoleAuthList */
  owa.len = sizeof (objDefaultLoginConsoleAuthListValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objDefaultLoginConsoleAuthListValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objDefaultLoginConsoleAuthListValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
	;
  owa.l7rc = usmDbAPLLineSet(ACCESS_LINE_CONSOLE,
                         ACCESS_LEVEL_LOGIN,
                        objDefaultLoginConsoleAuthListValue);
  if (owa.l7rc != L7_SUCCESS)
  {
      if (owa.l7rc == L7_DEPENDENCY_NOT_MET)
       owa.rc = XLIBRC_AUTHLIST_CONSOLE_LOGIN_ERROR;
    else
       owa.rc = XLIBRC_FAILURE;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrLoginAuthList_DefaultLoginTelnetAuthList
*
* @purpose Get 'DefaultLoginTelnetAuthList'
 *@description  [DefaultLoginTelnetAuthList] default login telnet authentication
* list.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrLoginAuthList_DefaultLoginTelnetAuthList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objDefaultLoginTelnetAuthListValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbAPLLineGet(ACCESS_LINE_TELNET,
                         ACCESS_LEVEL_LOGIN,
                         objDefaultLoginTelnetAuthListValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objDefaultLoginTelnetAuthListValue,
                     strlen (objDefaultLoginTelnetAuthListValue));

  /* return the object value: DefaultLoginTelnetAuthList */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDefaultLoginTelnetAuthListValue,
                           strlen (objDefaultLoginTelnetAuthListValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_baseUsrMgrLoginAuthList_DefaultLoginTelnetAuthList
*
* @purpose List 'DefaultLoginTelnetAuthList'
 *@description  [DefaultLoginTelnetAuthList] default login telnet authentication
* list.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseUsrMgrLoginAuthList_DefaultLoginTelnetAuthList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objDefaultLoginTelnetAuthListValue;
  xLibStr256_t nextObjDefaultLoginTelnetAuthListValue;

  FPOBJ_TRACE_ENTER (bufp);
  memset(objDefaultLoginTelnetAuthListValue,0x0,sizeof(objDefaultLoginTelnetAuthListValue));
  memset(nextObjDefaultLoginTelnetAuthListValue,0x0,sizeof(nextObjDefaultLoginTelnetAuthListValue));
  owa.len = sizeof (objDefaultLoginTelnetAuthListValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrLoginAuthList_DefaultLoginTelnetAuthList,
                          (xLibU8_t *) objDefaultLoginTelnetAuthListValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbAPLListGetFirst(ACCESS_LINE_TELNET,
                         ACCESS_LEVEL_LOGIN,
                         nextObjDefaultLoginTelnetAuthListValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objDefaultLoginTelnetAuthListValue, owa.len);
    owa.l7rc =
      usmDbAPLListGetNext(ACCESS_LINE_TELNET,
                         ACCESS_LEVEL_LOGIN,
                         objDefaultLoginTelnetAuthListValue,
                           nextObjDefaultLoginTelnetAuthListValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjDefaultLoginTelnetAuthListValue, owa.len);

  /* return the object value: DefaultLoginTelnetAuthList */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjDefaultLoginTelnetAuthListValue,
                           strlen (nextObjDefaultLoginTelnetAuthListValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseUsrMgrLoginAuthList_DefaultLoginTelnetAuthList
*
* @purpose Set 'DefaultLoginTelnetAuthList'
 *@description  [DefaultLoginTelnetAuthList] default login telnet authentication
* list.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrLoginAuthList_DefaultLoginTelnetAuthList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objDefaultLoginTelnetAuthListValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DefaultLoginTelnetAuthList */
  owa.len = sizeof (objDefaultLoginTelnetAuthListValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objDefaultLoginTelnetAuthListValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objDefaultLoginTelnetAuthListValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbAPLLineSet(ACCESS_LINE_TELNET,
                         ACCESS_LEVEL_LOGIN,
                         objDefaultLoginTelnetAuthListValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrLoginAuthList_DefaultLoginSSHAuthList
*
* @purpose Get 'DefaultLoginSSHAuthList'
 *@description  [DefaultLoginSSHAuthList] default login SSH authentication list.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrLoginAuthList_DefaultLoginSSHAuthList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objDefaultLoginSSHAuthListValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbAPLLineGet(ACCESS_LINE_SSH,
                         ACCESS_LEVEL_LOGIN,
                         objDefaultLoginSSHAuthListValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objDefaultLoginSSHAuthListValue,
                     strlen (objDefaultLoginSSHAuthListValue));

  /* return the object value: DefaultLoginSSHAuthList */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDefaultLoginSSHAuthListValue,
                           strlen (objDefaultLoginSSHAuthListValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_baseUsrMgrLoginAuthList_DefaultLoginSSHAuthList
*
* @purpose List 'DefaultLoginSSHAuthList'
 *@description  [DefaultLoginSSHAuthList] default login SSH authentication list.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseUsrMgrLoginAuthList_DefaultLoginSSHAuthList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objDefaultLoginSSHAuthListValue;
  xLibStr256_t nextObjDefaultLoginSSHAuthListValue;

  FPOBJ_TRACE_ENTER (bufp);
  memset(objDefaultLoginSSHAuthListValue,0x0,sizeof(objDefaultLoginSSHAuthListValue));
  memset(nextObjDefaultLoginSSHAuthListValue,0x0,sizeof(nextObjDefaultLoginSSHAuthListValue));
  owa.len = sizeof (objDefaultLoginSSHAuthListValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrLoginAuthList_DefaultLoginSSHAuthList,
                          (xLibU8_t *) objDefaultLoginSSHAuthListValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbAPLListGetFirst(ACCESS_LINE_SSH,
                         ACCESS_LEVEL_LOGIN, nextObjDefaultLoginSSHAuthListValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objDefaultLoginSSHAuthListValue, owa.len);
    owa.l7rc =
      usmDbAPLListGetNext(ACCESS_LINE_SSH,
                         ACCESS_LEVEL_LOGIN,
                         objDefaultLoginSSHAuthListValue,
                           nextObjDefaultLoginSSHAuthListValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjDefaultLoginSSHAuthListValue, owa.len);

  /* return the object value: DefaultLoginSSHAuthList */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjDefaultLoginSSHAuthListValue,
                           strlen (nextObjDefaultLoginSSHAuthListValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseUsrMgrLoginAuthList_DefaultLoginSSHAuthList
*
* @purpose Set 'DefaultLoginSSHAuthList'
 *@description  [DefaultLoginSSHAuthList] default login SSH authentication list.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrLoginAuthList_DefaultLoginSSHAuthList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objDefaultLoginSSHAuthListValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DefaultLoginSSHAuthList */
  owa.len = sizeof (objDefaultLoginSSHAuthListValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objDefaultLoginSSHAuthListValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objDefaultLoginSSHAuthListValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbAPLLineSet(ACCESS_LINE_SSH,
                         ACCESS_LEVEL_LOGIN,
                         objDefaultLoginSSHAuthListValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrLoginAuthList_LoginAPLName
*
* @purpose Get 'LoginAPLName'
 *@description  [LoginAPLName] The name of the login Authentication profile list.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrLoginAuthList_LoginAPLName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objLoginAPLNameValue;
  xLibStr256_t nextObjLoginAPLNameValue;

  FPOBJ_TRACE_ENTER (bufp);
  memset(objLoginAPLNameValue,0x00,sizeof( objLoginAPLNameValue));
  memset( nextObjLoginAPLNameValue,0x00,sizeof( nextObjLoginAPLNameValue));
  /* retrieve key: LoginAPLName */
  owa.len = sizeof (objLoginAPLNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrLoginAuthList_LoginAPLName,
                          (xLibU8_t *) objLoginAPLNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbAPLListGetFirst(ACCESS_LINE_CTS,
                         ACCESS_LEVEL_LOGIN,
                         nextObjLoginAPLNameValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objLoginAPLNameValue, owa.len);
    owa.l7rc = usmDbAPLListGetNext(ACCESS_LINE_CTS,
                         ACCESS_LEVEL_LOGIN,
                         objLoginAPLNameValue,
                         nextObjLoginAPLNameValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjLoginAPLNameValue, owa.len);

  /* return the object value: LoginAPLName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjLoginAPLNameValue,
                           strlen (nextObjLoginAPLNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrLoginAuthList_LoginAuthListRowStatus
*
* @purpose Get 'LoginAuthListRowStatus'
 *@description  [LoginAuthListRowStatus] Create or Delete the login auth list.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrLoginAuthList_LoginAuthListRowStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objLoginAuthListRowStatusValue;

  xLibStr256_t keyLoginAPLNameValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LoginAPLName */
  owa.len = sizeof (keyLoginAPLNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrLoginAuthList_LoginAPLName,
                          (xLibU8_t *) keyLoginAPLNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyLoginAPLNameValue, owa.len);
  /* get the value from application */
  objLoginAuthListRowStatusValue = L7_ROW_STATUS_ACTIVE;
  owa.l7rc = L7_SUCCESS;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objLoginAuthListRowStatusValue,
                     sizeof (objLoginAuthListRowStatusValue));

  /* return the object value: LoginAuthListRowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLoginAuthListRowStatusValue,
                           sizeof (objLoginAuthListRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseUsrMgrLoginAuthList_LoginAuthListRowStatus
*
* @purpose Set 'LoginAuthListRowStatus'
 *@description  [LoginAuthListRowStatus] Create or Delete the login auth list.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrLoginAuthList_LoginAuthListRowStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objLoginAuthListRowStatusValue;

  xLibStr256_t keyLoginAPLNameValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LoginAuthListRowStatus */
  owa.len = sizeof (objLoginAuthListRowStatusValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objLoginAuthListRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLoginAuthListRowStatusValue, owa.len);

  /* retrieve key: LoginAPLName */
  owa.len = sizeof (keyLoginAPLNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrLoginAuthList_LoginAPLName,
                          (xLibU8_t *) keyLoginAPLNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyLoginAPLNameValue, owa.len);

  owa.l7rc = L7_SUCCESS;

  if (objLoginAuthListRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    owa.l7rc = usmDbAPLCreate(ACCESS_LINE_CTS,
                         ACCESS_LEVEL_LOGIN, 
                        keyLoginAPLNameValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_AUTH_LIST_CREATE_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }


  }
  else if (objLoginAuthListRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    owa.l7rc = usmDbAPLDelete (ACCESS_LINE_CTS,
                         ACCESS_LEVEL_LOGIN, 
                         keyLoginAPLNameValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_AUTH_LIST_DEL_FAILURE;  /* TODO: Change if required */
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

/*******************************************************************************
* @function fpObjGet_baseUsrMgrLoginAuthList_LoginAuthMethodOrder
*
* @purpose Get 'LoginAuthMethodOrder'
 *@description  [LoginAuthMethodOrder] The order of the Login authentication
* method.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrLoginAuthList_LoginAuthMethodOrder (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objLoginAuthMethodOrderValue;
  xLibU32_t nextObjLoginAuthMethodOrderValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LoginAuthMethodOrder */
  owa.len = sizeof (objLoginAuthMethodOrderValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrLoginAuthList_LoginAuthMethodOrder,
                          (xLibU8_t *) & objLoginAuthMethodOrderValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
	nextObjLoginAuthMethodOrderValue = 0;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objLoginAuthMethodOrderValue, owa.len);
	 if(objLoginAuthMethodOrderValue < L7_LOGIN_MAX_AUTH_METHODS)
	 {
	    nextObjLoginAuthMethodOrderValue = objLoginAuthMethodOrderValue + 1;
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

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjLoginAuthMethodOrderValue, owa.len);

  /* return the object value: LoginAuthMethodOrder */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjLoginAuthMethodOrderValue,
                           sizeof (nextObjLoginAuthMethodOrderValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrLoginAuthList_LoginAuthMethod
*
* @purpose Get 'LoginAuthMethod'
 *@description  [LoginAuthMethod] The Login authentication method being set .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrLoginAuthList_LoginAuthMethod (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objLoginAuthMethodValue;

  xLibStr256_t keyLoginAPLNameValue;
  xLibU32_t keyLoginAuthMethodOrderValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LoginAPLName */
  owa.len = sizeof (keyLoginAPLNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrLoginAuthList_LoginAPLName,
                          (xLibU8_t *) keyLoginAPLNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyLoginAPLNameValue, owa.len);

  /* retrieve key: LoginAuthMethodOrder */
  owa.len = sizeof (keyLoginAuthMethodOrderValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrLoginAuthList_LoginAuthMethodOrder,
                          (xLibU8_t *) & keyLoginAuthMethodOrderValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLoginAuthMethodOrderValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_CTS,
                                             ACCESS_LEVEL_LOGIN,
                                             keyLoginAPLNameValue,
                                             keyLoginAuthMethodOrderValue,
                                             &objLoginAuthMethodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objLoginAuthMethodValue, sizeof (objLoginAuthMethodValue));

  /* return the object value: LoginAuthMethod */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLoginAuthMethodValue,
                           sizeof (objLoginAuthMethodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseUsrMgrLoginAuthList_LoginAuthMethod
*
* @purpose Set 'LoginAuthMethod'
 *@description  [LoginAuthMethod] The Login authentication method being set .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrLoginAuthList_LoginAuthMethod (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objLoginAuthMethodValue;

  xLibStr256_t keyLoginAPLNameValue;
  xLibU32_t keyLoginAuthMethodOrderValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LoginAuthMethod */
  owa.len = sizeof (objLoginAuthMethodValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objLoginAuthMethodValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLoginAuthMethodValue, owa.len);

  /* retrieve key: LoginAPLName */
  owa.len = sizeof (keyLoginAPLNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrLoginAuthList_LoginAPLName,
                          (xLibU8_t *) keyLoginAPLNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyLoginAPLNameValue, owa.len);

  /* retrieve key: LoginAuthMethodOrder */
  owa.len = sizeof (keyLoginAuthMethodOrderValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrLoginAuthList_LoginAuthMethodOrder,
                          (xLibU8_t *) & keyLoginAuthMethodOrderValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLoginAuthMethodOrderValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_CTS,
                                             ACCESS_LEVEL_LOGIN,
                                             keyLoginAPLNameValue,
                                             keyLoginAuthMethodOrderValue,
                                             objLoginAuthMethodValue);
	
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrLoginAuthList_AuthenticationListMethod1
*
* @purpose Get 'AuthenticationListMethod1'
*
* @description [AuthenticationListMethod1]: authentication method1 at the
*              specified index in the list 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrLoginAuthList_AuthenticationListMethod1 (void *wap,
                                                                void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAuthenticationListMethod1Value;

  xLibStr256_t keyEnableAPLNameValue;

  FPOBJ_TRACE_ENTER (bufp);
  memset(keyEnableAPLNameValue,0x00,sizeof(keyEnableAPLNameValue));

  /* retrieve key: EnableAPLName */
  owa.len = sizeof (keyEnableAPLNameValue);
  owa.rc = xLibFilterGet (wap,  XOBJ_baseUsrMgrLoginAuthList_LoginAPLName,
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
                                             ACCESS_LEVEL_LOGIN,
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
* @function fpObjSet_baseUsrMgrLoginAuthList_AuthenticationListMethod1
*
* @purpose Set 'AuthenticationListMethod1'
*
* @description [AuthenticationListMethod1]: authentication method1 at the
*              specified index in the list 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrLoginAuthList_AuthenticationListMethod1 (void *wap,
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
  owa.rc = xLibFilterGet (wap,  XOBJ_baseUsrMgrLoginAuthList_LoginAPLName,
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
                                             ACCESS_LEVEL_LOGIN,
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
* @function fpObjGet_baseUsrMgrLoginAuthList_AuthenticationListMethod2
*
* @purpose Get 'AuthenticationListMethod2'
*
* @description [AuthenticationListMethod2]: authentication method2 at the
*              specified index in the list 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrLoginAuthList_AuthenticationListMethod2 (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAuthenticationListMethod2Value;

  xLibStr256_t keyEnableAPLNameValue;

  FPOBJ_TRACE_ENTER (bufp);
  memset(keyEnableAPLNameValue,0x00,sizeof(keyEnableAPLNameValue));

  /* retrieve key: EnableAPLName */
  owa.len = sizeof (keyEnableAPLNameValue);
  owa.rc = xLibFilterGet (wap,  XOBJ_baseUsrMgrLoginAuthList_LoginAPLName,
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
                                             ACCESS_LEVEL_LOGIN,
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
* @function fpObjSet_baseUsrMgrLoginAuthList_AuthenticationListMethod2
*
* @purpose Set 'AuthenticationListMethod2'
*
* @description [AuthenticationListMethod2]: authentication method2 at the
*              specified index in the list 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrLoginAuthList_AuthenticationListMethod2 (void *wap,
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
  owa.rc = xLibFilterGet (wap,  XOBJ_baseUsrMgrLoginAuthList_LoginAPLName,
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
                                             ACCESS_LEVEL_LOGIN,
                                             keyEnableAPLNameValue,
                                             0,
                                             &objAuthenticationListMethod1Value);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  if((objAuthenticationListMethod1Value == L7_AUTH_METHOD_UNDEFINED) &&
    (objAuthenticationListMethod2Value != L7_AUTH_METHOD_UNDEFINED))
  {
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_CTS,
                                             ACCESS_LEVEL_LOGIN,
                                             keyEnableAPLNameValue,
                                          1,
                                           objAuthenticationListMethod1Value);
    owa.rc = XLIBRC_PREVIOUS_METHOD_UNCONFIGURED;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;                                       
  }
  else
  {
     /* set the method2 value in application */
     owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_CTS,
                                             ACCESS_LEVEL_LOGIN,
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
* @function fpObjGet_baseUsrMgrLoginAuthList_AuthenticationListMethod3
*
* @purpose Get 'AuthenticationListMethod3'
*
* @description [AuthenticationListMethod3]: authentication method3 at the
*              specified index in the list 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrLoginAuthList_AuthenticationListMethod3 (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAuthenticationListMethod3Value;

  xLibStr256_t keyEnableAPLNameValue;

  FPOBJ_TRACE_ENTER (bufp);
  memset(keyEnableAPLNameValue,0x00,sizeof(keyEnableAPLNameValue));

  /* retrieve key: EnableAPLName */
  owa.len = sizeof (keyEnableAPLNameValue);
  owa.rc = xLibFilterGet (wap,  XOBJ_baseUsrMgrLoginAuthList_LoginAPLName,
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
                                             ACCESS_LEVEL_LOGIN,
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
* @function fpObjSet_baseUsrMgrLoginAuthList_AuthenticationListMethod3
*
* @purpose Set 'AuthenticationListMethod3'
*
* @description [AuthenticationListMethod3]: authentication method3 at the
*              specified index in the list 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrLoginAuthList_AuthenticationListMethod3 (void *wap,
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
  owa.rc = xLibFilterGet (wap,  XOBJ_baseUsrMgrLoginAuthList_LoginAPLName,
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
                                             ACCESS_LEVEL_LOGIN,
                                             keyEnableAPLNameValue,
                                             1,
                                             &objAuthenticationListMethod2Value);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  if((objAuthenticationListMethod2Value == L7_AUTH_METHOD_UNDEFINED) && 
    (objAuthenticationListMethod3Value != L7_AUTH_METHOD_UNDEFINED))
  {
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_CTS,
                                             ACCESS_LEVEL_LOGIN,
                                             keyEnableAPLNameValue,
                                          2,
                                           objAuthenticationListMethod2Value);
    owa.rc = XLIBRC_PREVIOUS_METHOD_UNCONFIGURED;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;                                           
  }
  else
  {
  
    /* set the value in application */
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_CTS,
                                             ACCESS_LEVEL_LOGIN,
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
* @function fpObjGet_baseUsrMgrLoginAuthList_AuthenticationListMethod4
*
* @purpose Get 'AuthenticationListMethod3'
*
* @description [AuthenticationListMethod3]: authentication method3 at the
*              specified index in the list 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrLoginAuthList_AuthenticationListMethod4 (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAuthenticationListMethod4Value;

  xLibStr256_t keyEnableAPLNameValue;

  FPOBJ_TRACE_ENTER (bufp);
  memset(keyEnableAPLNameValue,0x00,sizeof(keyEnableAPLNameValue));

  /* retrieve key: EnableAPLName */
  owa.len = sizeof (keyEnableAPLNameValue);
  owa.rc = xLibFilterGet (wap,  XOBJ_baseUsrMgrLoginAuthList_LoginAPLName,
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
                                             ACCESS_LEVEL_LOGIN,
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
* @function fpObjSet_baseUsrMgrLoginAuthList_AuthenticationListMethod4
*
* @purpose Set 'AuthenticationListMethod3'
*
* @description [AuthenticationListMethod3]: authentication method3 at the
*              specified index in the list 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrLoginAuthList_AuthenticationListMethod4 (void *wap,
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
  FPOBJ_TRACE_VALUE (bufp, &objAuthenticationListMethod4Value, owa.len);



  memset(keyEnableAPLNameValue,0x00,sizeof(keyEnableAPLNameValue));

  /* retrieve key: EnableAPLName */
  owa.len = sizeof (keyEnableAPLNameValue);
  owa.rc = xLibFilterGet (wap,  XOBJ_baseUsrMgrLoginAuthList_LoginAPLName,
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
                                             ACCESS_LEVEL_LOGIN,
                                             keyEnableAPLNameValue,
                                             2,
                                             &objAuthenticationListMethod3Value);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  if((objAuthenticationListMethod3Value == L7_AUTH_METHOD_UNDEFINED) && 
    (objAuthenticationListMethod4Value != L7_AUTH_METHOD_UNDEFINED))
  {
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_CTS,
                                             ACCESS_LEVEL_LOGIN,
                                             keyEnableAPLNameValue,
                                          3,
                                           objAuthenticationListMethod3Value);
    owa.rc = XLIBRC_PREVIOUS_METHOD_UNCONFIGURED;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;                                       
  }
  else
  {

    /* set the value in application */
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_CTS,
                                             ACCESS_LEVEL_LOGIN,
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
* @function fpObjGet_baseUsrMgrLoginAuthList_AuthenticationListMethod5
*
* @purpose Get 'AuthenticationListMethod5'
*
* @description [AuthenticationListMethod5]: authentication method5 at the
*              specified index in the list 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrLoginAuthList_AuthenticationListMethod5 (void *wap,
                                                                void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAuthenticationListMethod5Value;

  xLibStr256_t keyEnableAPLNameValue;

  FPOBJ_TRACE_ENTER (bufp);
  memset(keyEnableAPLNameValue,0x00,sizeof(keyEnableAPLNameValue));

  /* retrieve key: EnableAPLName */
  owa.len = sizeof (keyEnableAPLNameValue);
  owa.rc = xLibFilterGet (wap,  XOBJ_baseUsrMgrLoginAuthList_LoginAPLName,
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
                                             ACCESS_LEVEL_LOGIN,
                                             keyEnableAPLNameValue,
                                             4,
                                             &objAuthenticationListMethod5Value);



  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AuthenticationListMethod5 */
  owa.len = sizeof (xLibU32_t);
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objAuthenticationListMethod5Value,
                    sizeof (objAuthenticationListMethod5Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseUsrMgrLoginAuthList_AuthenticationListMethod5
*
* @purpose Set 'AuthenticationListMethod5'
*
* @description [AuthenticationListMethod5]: authentication method5 at the
*              specified index in the list 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrLoginAuthList_AuthenticationListMethod5 (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAuthenticationListMethod5Value;
  xLibU32_t objAuthenticationListMethod4Value;

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
  owa.rc = xLibFilterGet (wap,  XOBJ_baseUsrMgrLoginAuthList_LoginAPLName,
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
                                             ACCESS_LEVEL_LOGIN,
                                             keyEnableAPLNameValue,
                                             3,
                                             &objAuthenticationListMethod4Value);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  if((objAuthenticationListMethod4Value == L7_AUTH_METHOD_UNDEFINED) && 
    (objAuthenticationListMethod5Value != L7_AUTH_METHOD_UNDEFINED))
  {
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_CTS,
                                             ACCESS_LEVEL_LOGIN,
                                             keyEnableAPLNameValue,
                                          4,
                                           objAuthenticationListMethod4Value);
    owa.rc = XLIBRC_PREVIOUS_METHOD_UNCONFIGURED;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;                                       
  }
  else
  {

    /* set the value in application */
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_CTS,
                                             ACCESS_LEVEL_LOGIN,
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
* @function fpObjGet_baseUsrMgrLoginAuthList_AuthenticationListMethod6
*
* @purpose Get 'AuthenticationListMethod6'
*
* @description [AuthenticationListMethod6]: authentication method6 at the
*              specified index in the list 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrLoginAuthList_AuthenticationListMethod6 (void *wap,
                                                                void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAuthenticationListMethod6Value;

  xLibStr256_t keyEnableAPLNameValue;

  FPOBJ_TRACE_ENTER (bufp);
  memset(keyEnableAPLNameValue,0x00,sizeof(keyEnableAPLNameValue));

  /* retrieve key: EnableAPLName */
  owa.len = sizeof (keyEnableAPLNameValue);
  owa.rc = xLibFilterGet (wap,  XOBJ_baseUsrMgrLoginAuthList_LoginAPLName,
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
                                             ACCESS_LEVEL_LOGIN,
                                             keyEnableAPLNameValue,
                                             5,
                                             &objAuthenticationListMethod6Value);



  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AuthenticationListMethod6 */
  owa.len = sizeof (xLibU32_t);
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objAuthenticationListMethod6Value,
                    sizeof (objAuthenticationListMethod6Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseUsrMgrLoginAuthList_AuthenticationListMethod6
*
* @purpose Set 'AuthenticationListMethod6'
*
* @description [AuthenticationListMethod6]: authentication method6 at the
*              specified index in the list 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrLoginAuthList_AuthenticationListMethod6 (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAuthenticationListMethod6Value;
  xLibU32_t objAuthenticationListMethod5Value;
  
  xLibStr256_t keyEnableAPLNameValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AuthenticationListMethod6 */
  owa.len = sizeof (keyEnableAPLNameValue);
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objAuthenticationListMethod6Value,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAuthenticationListMethod6Value, owa.len);



  memset(keyEnableAPLNameValue,0x00,sizeof(keyEnableAPLNameValue));

  /* retrieve key: EnableAPLName */
  owa.len = sizeof (keyEnableAPLNameValue);
  owa.rc = xLibFilterGet (wap,  XOBJ_baseUsrMgrLoginAuthList_LoginAPLName,
                          (xLibU8_t *) keyEnableAPLNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyEnableAPLNameValue, owa.len);
  
  /* get the method5 value from application */
  owa.l7rc = usmDbAPLAuthMethodGet(ACCESS_LINE_CTS,
                                             ACCESS_LEVEL_LOGIN,
                                             keyEnableAPLNameValue,
                                             4,
                                             &objAuthenticationListMethod5Value);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  if((objAuthenticationListMethod5Value == L7_AUTH_METHOD_UNDEFINED) && 
    (objAuthenticationListMethod6Value != L7_AUTH_METHOD_UNDEFINED))
  {
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_CTS,
                                             ACCESS_LEVEL_LOGIN,
                                             keyEnableAPLNameValue,
                                          5,
                                           objAuthenticationListMethod5Value);
    owa.rc = XLIBRC_PREVIOUS_METHOD_UNCONFIGURED;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;                                        
  }
  else
  {

    /* set the value in application */
    owa.l7rc = usmDbAPLAuthMethodSet(ACCESS_LINE_CTS,
                                             ACCESS_LEVEL_LOGIN,
                                             keyEnableAPLNameValue,
                                          5,
                                           objAuthenticationListMethod6Value);

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
* @function fpObjGet_baseUsrMgrLoginAuthList_MethodList
*
* @purpose Get 'MethodList'
*
* @description
*             
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrLoginAuthList_MethodList (void *wap,
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
  owa.rc = xLibFilterGet (wap,  XOBJ_baseUsrMgrLoginAuthList_LoginAPLName,
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
     for (i = 0; i < L7_MAX_LOGINAUTH_METHODS; i++)
     {
      
     /* get the value from application */
       owa.l7rc = usmDbAPLAuthMethodGet (ACCESS_LINE_CTS,
                                                  ACCESS_LEVEL_LOGIN,
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


