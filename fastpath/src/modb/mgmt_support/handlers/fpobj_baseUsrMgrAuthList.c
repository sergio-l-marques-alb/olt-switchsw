/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseUsrMgrAuthList.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to system-object.xml
*
* @create  12 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseUsrMgrAuthList_obj.h"
#include "usmdb_user_mgmt_api.h"
#include "user_manager_exports.h"

/*******************************************************************************
* @function fpObjGet_baseUsrMgrAuthList_AuthProfileListCount
*
* @purpose Get 'AuthProfileListCount'
*
* @description [AuthProfileListCount]: Total number of configured APLs 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrAuthList_AuthProfileListCount (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAuthProfileListCountValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  /* owa.l7rc =
    usmDbAPLListGetCount (L7_UNIT_CURRENT, &objAuthProfileListCountValue);*/
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAuthProfileListCountValue,
                     sizeof (objAuthProfileListCountValue));

  /* return the object value: AuthProfileListCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAuthProfileListCountValue,
                           sizeof (objAuthProfileListCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseUsrMgrAuthList_AuthProfileListCreate
*
* @purpose Set 'AuthProfileListCreate'
*
* @description [AuthProfileListCreate]: Creates a named APL with no authentication
*              methods 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrAuthList_AuthProfileListCreate (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAuthProfileListCreateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AuthProfileListCreate */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objAuthProfileListCreateValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objAuthProfileListCreateValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbAPLCreate (ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, objAuthProfileListCreateValue);
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
* @function fpObjGet_baseUsrMgrAuthList_UserConfigDefaultAuthenticationList
*
* @purpose Get 'UserConfigDefaultAuthenticationList'
*
* @description [UserConfigDefaultAuthenticationList]: the APL assigned to
*              nonconfigured users for a specific component 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrAuthList_UserConfigDefaultAuthenticationList (void
                                                                          *wap,
                                                                          void
                                                                          *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUserConfigDefaultAuthenticationListValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbAPLNonConfiguredUserGet (L7_UNIT_CURRENT, L7_USER_MGR_COMPONENT_ID,
                                  objUserConfigDefaultAuthenticationListValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objUserConfigDefaultAuthenticationListValue,
                     strlen (objUserConfigDefaultAuthenticationListValue));

  /* return the object value: UserConfigDefaultAuthenticationList */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) objUserConfigDefaultAuthenticationListValue,
                    strlen (objUserConfigDefaultAuthenticationListValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseUsrMgrAuthList_UserConfigDefaultAuthenticationList
*
* @purpose Set 'UserConfigDefaultAuthenticationList'
*
* @description [UserConfigDefaultAuthenticationList]: the APL assigned to
*              nonconfigured users for a specific component 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrAuthList_UserConfigDefaultAuthenticationList (void
                                                                          *wap,
                                                                          void
                                                                          *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUserConfigDefaultAuthenticationListValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UserConfigDefaultAuthenticationList */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *)
                           objUserConfigDefaultAuthenticationListValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objUserConfigDefaultAuthenticationListValue,
                     owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbAPLNonConfiguredUserSet (L7_UNIT_CURRENT, L7_USER_MGR_COMPONENT_ID,
                                  objUserConfigDefaultAuthenticationListValue);
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
* @function fpObjGet_baseUsrMgrAuthList_AuthenticationListIndex
*
* @purpose Get 'AuthenticationListIndex'
*
* @description [AuthenticationListIndex]: the index of APL in the list 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrAuthList_AuthenticationListIndex (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAuthenticationListIndexValue;
  xLibU32_t nextObjAuthenticationListIndexValue;
  xLibStr256_t tempAuthListName;
  xLibStr256_t tempNextAuthListName;
  FPOBJ_TRACE_ENTER (bufp);
  memset(tempAuthListName,0x00,sizeof(tempAuthListName));
  memset(tempNextAuthListName,0x00,sizeof(tempNextAuthListName));

  /* retrieve key: AuthenticationListIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrAuthList_AuthenticationListIndex,
                          (xLibU8_t *) & objAuthenticationListIndexValue,
                          &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbAPLListGetFirst(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, tempAuthListName );
    if(owa.l7rc  == L7_SUCCESS)
    {
    /*  owa.l7rc  = usmDbAPLIndexGetByName(L7_UNIT_CURRENT, tempAuthListName, 
	  	                             &nextObjAuthenticationListIndexValue);*/
    }    
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objAuthenticationListIndexValue, owa.len);    

    owa.l7rc = usmDbAPLListGetIndex(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, 
		                             objAuthenticationListIndexValue, tempAuthListName );
    if(owa.l7rc  == L7_SUCCESS)
    { 
      owa.l7rc = usmDbAPLListGetNext(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN,
		                                  tempAuthListName, tempNextAuthListName );
      if(owa.l7rc  == L7_SUCCESS)
      {
        /* owa.l7rc  = usmDbAPLIndexGetByName(L7_UNIT_CURRENT, 
		 	tempNextAuthListName, &nextObjAuthenticationListIndexValue);*/
      }
    }
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjAuthenticationListIndexValue, owa.len);

  /* return the object value: AuthenticationListIndex */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & nextObjAuthenticationListIndexValue,
                    sizeof (objAuthenticationListIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseUsrMgrAuthList_AuthenticationListName
*
* @purpose Get 'AuthenticationListName'
*
* @description [AuthenticationListName]: configured APL Name 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrAuthList_AuthenticationListName (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAuthenticationListIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAuthenticationListNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AuthenticationListIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrAuthList_AuthenticationListIndex,
                          (xLibU8_t *) & keyAuthenticationListIndexValue,
                          &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAuthenticationListIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbAPLListGetIndex (ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, keyAuthenticationListIndexValue,
                          objAuthenticationListNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AuthenticationListName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objAuthenticationListNameValue,
                           strlen (objAuthenticationListNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseUsrMgrAuthList_AuthenticationListName
*
* @purpose Get 'AuthenticationListName'
*
* @description [AuthenticationListName]: configured APL Name 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrAuthList_AuthenticationListName (void *wap,
                                                             void *bufp)
{
  return XLIBRC_SUCCESS;
}



/*******************************************************************************
* @function fpObjGet_baseUsrMgrAuthList_AuthenticationListMethod1
*
* @purpose Get 'AuthenticationListMethod1'
*
* @description [AuthenticationListMethod1]: authentication method1 at the
*              specified index in the list 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrAuthList_AuthenticationListMethod1 (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAuthenticationListIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAuthenticationListMethod1Value;
  
  xLibStr256_t tempAuthListName;
  memset(tempAuthListName,0x00,sizeof(tempAuthListName));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AuthenticationListIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrAuthList_AuthenticationListIndex,
                          (xLibU8_t *) & keyAuthenticationListIndexValue,
                          &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAuthenticationListIndexValue, kwa.len);

  owa.l7rc = usmDbAPLListGetIndex(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, 
  	           keyAuthenticationListIndexValue, tempAuthListName);
  if (owa.l7rc == L7_SUCCESS)
  {
     /* get the value from application */
     owa.l7rc =
       usmDbAPLAuthMethodGet (ACCESS_LINE_CTS,
                                                  ACCESS_LEVEL_LOGIN,
                                         tempAuthListName,
                                         0,
                                        &objAuthenticationListMethod1Value);
  }

  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AuthenticationListMethod1 */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objAuthenticationListMethod1Value,
                    sizeof (objAuthenticationListMethod1Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseUsrMgrAuthList_AuthenticationListMethod1
*
* @purpose Set 'AuthenticationListMethod1'
*
* @description [AuthenticationListMethod1]: authentication method1 at the
*              specified index in the list 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrAuthList_AuthenticationListMethod1 (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAuthenticationListMethod1Value;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAuthenticationListIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  xLibStr256_t tempAuthListName;
  memset(tempAuthListName,0x00,sizeof(tempAuthListName));

  /* retrieve object: AuthenticationListMethod1 */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objAuthenticationListMethod1Value,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAuthenticationListMethod1Value, owa.len);


  /* retrieve key: AuthenticationListIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrAuthList_AuthenticationListIndex,
                          (xLibU8_t *) & keyAuthenticationListIndexValue,
                          &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAuthenticationListIndexValue, kwa.len);

  owa.l7rc = usmDbAPLListGetIndex(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, 
  	           keyAuthenticationListIndexValue, tempAuthListName);
  if (owa.l7rc == L7_SUCCESS)
  {
     /* get the value from application */
     owa.l7rc =
       usmDbAPLAuthMethodSet (ACCESS_LINE_CTS,
                                                  ACCESS_LEVEL_LOGIN,
                                         tempAuthListName,
                                         0,
                                        objAuthenticationListMethod1Value);
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
* @function fpObjGet_baseUsrMgrAuthList_AuthenticationListMethod2
*
* @purpose Get 'AuthenticationListMethod2'
*
* @description [AuthenticationListMethod2]: authentication method2 at the
*              specified index in the list 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrAuthList_AuthenticationListMethod2 (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAuthenticationListIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAuthenticationListMethod2Value;
  FPOBJ_TRACE_ENTER (bufp);
  xLibStr256_t tempAuthListName;
  memset(tempAuthListName,0x00,sizeof(tempAuthListName));

  /* retrieve key: AuthenticationListIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrAuthList_AuthenticationListIndex,
                          (xLibU8_t *) & keyAuthenticationListIndexValue,
                          &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAuthenticationListIndexValue, kwa.len);

  owa.l7rc = usmDbAPLListGetIndex(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, 
  	           keyAuthenticationListIndexValue, tempAuthListName);
  if (owa.l7rc == L7_SUCCESS)
  {
     /* get the value from application */
     owa.l7rc =
       usmDbAPLAuthMethodGet (ACCESS_LINE_CTS,
                                                  ACCESS_LEVEL_LOGIN,
                                         tempAuthListName,
                                         1,
                                        &objAuthenticationListMethod2Value);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AuthenticationListMethod2 */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objAuthenticationListMethod2Value,
                    sizeof (objAuthenticationListMethod2Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseUsrMgrAuthList_AuthenticationListMethod2
*
* @purpose Set 'AuthenticationListMethod2'
*
* @description [AuthenticationListMethod2]: authentication method2 at the
*              specified index in the list 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrAuthList_AuthenticationListMethod2 (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAuthenticationListMethod2Value;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAuthenticationListIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  xLibStr256_t tempAuthListName;
  memset(tempAuthListName,0x00,sizeof(tempAuthListName));

  /* retrieve object: AuthenticationListMethod2 */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objAuthenticationListMethod2Value,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAuthenticationListMethod2Value, owa.len);


  /* retrieve key: AuthenticationListIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrAuthList_AuthenticationListIndex,
                          (xLibU8_t *) & keyAuthenticationListIndexValue,
                          &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAuthenticationListIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbAPLListGetIndex(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, 
  	           keyAuthenticationListIndexValue, tempAuthListName);
  if (owa.l7rc == L7_SUCCESS)
  {
     /* get the value from application */
     owa.l7rc =
       usmDbAPLAuthMethodSet (ACCESS_LINE_CTS,
                                         ACCESS_LEVEL_LOGIN,
                                         tempAuthListName,
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
* @function fpObjGet_baseUsrMgrAuthList_AuthenticationListMethod3
*
* @purpose Get 'AuthenticationListMethod3'
*
* @description [AuthenticationListMethod3]: authentication method3 at the
*              specified index in the list 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrAuthList_AuthenticationListMethod3 (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAuthenticationListIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAuthenticationListMethod3Value;
  FPOBJ_TRACE_ENTER (bufp);

  xLibStr256_t tempAuthListName;
  memset(tempAuthListName,0x00,sizeof(tempAuthListName));

  /* retrieve key: AuthenticationListIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrAuthList_AuthenticationListIndex,
                          (xLibU8_t *) & keyAuthenticationListIndexValue,
                          &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAuthenticationListIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbAPLListGetIndex(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, 
  	           keyAuthenticationListIndexValue, tempAuthListName);
  if (owa.l7rc == L7_SUCCESS)
  {
     /* get the value from application */
     owa.l7rc =
       usmDbAPLAuthMethodGet (ACCESS_LINE_CTS,
                                                  ACCESS_LEVEL_LOGIN,
                                         tempAuthListName,
                                         2,
                                        &objAuthenticationListMethod3Value);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AuthenticationListMethod3 */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objAuthenticationListMethod3Value,
                    sizeof (objAuthenticationListMethod3Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseUsrMgrAuthList_AuthenticationListMethod3
*
* @purpose Set 'AuthenticationListMethod3'
*
* @description [AuthenticationListMethod3]: authentication method3 at the
*              specified index in the list 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrAuthList_AuthenticationListMethod3 (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAuthenticationListMethod3Value;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAuthenticationListIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  xLibStr256_t tempAuthListName;
  memset(tempAuthListName,0x00,sizeof(tempAuthListName));

  /* retrieve object: AuthenticationListMethod3 */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objAuthenticationListMethod3Value,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAuthenticationListMethod3Value, owa.len);

  /* retrieve key: AuthenticationListIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrAuthList_AuthenticationListIndex,
                          (xLibU8_t *) & keyAuthenticationListIndexValue,
                          &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAuthenticationListIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbAPLListGetIndex(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, 
  	           keyAuthenticationListIndexValue, tempAuthListName);
  if (owa.l7rc == L7_SUCCESS)
  {
     /* get the value from application */
     owa.l7rc =
       usmDbAPLAuthMethodSet (ACCESS_LINE_CTS,
                                         ACCESS_LEVEL_LOGIN,
                                         tempAuthListName,
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
* @function fpObjGet_baseUsrMgrAuthList_AuthenticationListMethod4
*
* @purpose Get 'AuthenticationListMethod4'
*
* @description [AuthenticationListMethod4]: authentication method4 at the
*              specified index in the list 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrAuthList_AuthenticationListMethod4 (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAuthenticationListIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAuthenticationListMethod4Value;
  FPOBJ_TRACE_ENTER (bufp);

  xLibStr256_t tempAuthListName;
  memset(tempAuthListName,0x00,sizeof(tempAuthListName));

  /* retrieve key: AuthenticationListIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrAuthList_AuthenticationListIndex,
                          (xLibU8_t *) & keyAuthenticationListIndexValue,
                          &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAuthenticationListIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbAPLListGetIndex(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, 
  	           keyAuthenticationListIndexValue, tempAuthListName);
  if (owa.l7rc == L7_SUCCESS)
  {
     /* get the value from application */
     owa.l7rc =
       usmDbAPLAuthMethodGet (ACCESS_LINE_CTS,
                                                  ACCESS_LEVEL_LOGIN,
                                         tempAuthListName,
                                         3,
                                        &objAuthenticationListMethod4Value);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AuthenticationListMethod4 */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objAuthenticationListMethod4Value,
                    sizeof (objAuthenticationListMethod4Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseUsrMgrAuthList_AuthenticationListMethod4
*
* @purpose Set 'AuthenticationListMethod4'
*
* @description [AuthenticationListMethod4]: authentication method4 at the
*              specified index in the list 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrAuthList_AuthenticationListMethod4 (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAuthenticationListMethod4Value;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAuthenticationListIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  xLibStr256_t tempAuthListName;
  memset(tempAuthListName,0x00,sizeof(tempAuthListName));

  /* retrieve object: AuthenticationListMethod4 */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objAuthenticationListMethod4Value,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAuthenticationListMethod4Value, owa.len);


  /* retrieve key: AuthenticationListIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrAuthList_AuthenticationListIndex,
                          (xLibU8_t *) & keyAuthenticationListIndexValue,
                          &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAuthenticationListIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbAPLListGetIndex(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, 
  	           keyAuthenticationListIndexValue, tempAuthListName);
  if (owa.l7rc == L7_SUCCESS)
  {
     /* get the value from application */
     owa.l7rc =
       usmDbAPLAuthMethodSet (ACCESS_LINE_CTS,
                                         ACCESS_LEVEL_LOGIN,
                                         tempAuthListName,
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
* @function fpObjGet_baseUsrMgrAuthList_AuthenticationListStatus
*
* @purpose Get 'AuthenticationListStatus'
*
* @description [AuthenticationListStatus]: Status of given index row of APL
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrAuthList_AuthenticationListStatus (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAuthenticationListIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAuthenticationListStatusValue=0;
  FPOBJ_TRACE_ENTER (bufp);
  xLibStr256_t tempAuthListName;

  memset(tempAuthListName,0x00,sizeof(tempAuthListName));

  /* retrieve key: AuthenticationListIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrAuthList_AuthenticationListIndex,
                          (xLibU8_t *) & keyAuthenticationListIndexValue,
                          &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAuthenticationListIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbAPLListGetIndex(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, 
  	           keyAuthenticationListIndexValue, tempAuthListName);
  if (owa.l7rc == L7_SUCCESS)
  {
    objAuthenticationListStatusValue = L7_ROW_STATUS_ACTIVE;
  }

  /* return the object value: AuthenticationListStatus */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objAuthenticationListStatusValue,
                    sizeof (objAuthenticationListStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseUsrMgrAuthList_AuthenticationListStatus
*
* @purpose Set 'AuthenticationListStatus'
*
* @description [AuthenticationListStatus]: Status of given index row of APL
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrAuthList_AuthenticationListStatus (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAuthenticationListStatusValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAuthenticationListIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  xLibStr256_t tempAuthListName;
  memset(tempAuthListName,0x00,sizeof(tempAuthListName));

  /* retrieve object: AuthenticationListStatus */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objAuthenticationListStatusValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAuthenticationListStatusValue, owa.len);

  /* retrieve key: AuthenticationListIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrAuthList_AuthenticationListIndex,
                          (xLibU8_t *) & keyAuthenticationListIndexValue,
                          &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAuthenticationListIndexValue, kwa.len);

  /* call the usmdb only for add and delete */
  if (objAuthenticationListStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Delete the existing row */
    owa.l7rc = usmDbAPLListGetIndex(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, 
  	           keyAuthenticationListIndexValue, tempAuthListName);
    if (owa.l7rc == L7_SUCCESS)
    {
       /* get the value from application */
       owa.l7rc =
         usmDbAPLDelete (ACCESS_LINE_CTS,
                                        ACCESS_LEVEL_ENABLE,
                                         tempAuthListName);
    }
   
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  return XLIBRC_SUCCESS;
}



/*******************************************************************************
* @function fpObjGet_baseUsrMgrAuthList_UserName
*
* @purpose Get 'UserName'
*
* @description The userName from login account.. 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrAuthList_UserName (void *wap, void *bufp)
{
	fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
	xLibStr256_t objUserNameValue;
	xLibStr256_t nextObjUserNameValue;

	xLibS32_t index;
	L7_RC_t rc;
	xLibU32_t loginStatus;

	FPOBJ_TRACE_ENTER (bufp);

    memset(objUserNameValue,0x00, sizeof(objUserNameValue));
	/* retrieve key: UserName */
	owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrAuthList_UserName,
	                        (xLibU8_t *)  objUserNameValue,
	                        &owa.len);
	if (owa.rc != XLIBRC_SUCCESS)
	{
	  FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
	  index = -1;
	}
	else
	{
	  FPOBJ_TRACE_CURRENT_KEY (bufp, objUserNameValue, owa.len);	
		
	  owa.l7rc = usmDbUserLoginIndexGet(L7_UNIT_CURRENT, 
			                                   objUserNameValue, &index);
	  if (owa.l7rc != L7_SUCCESS)
	  {
	     owa.rc = XLIBRC_ENDOF_TABLE;
	     FPOBJ_TRACE_EXIT (bufp, owa);
	     return owa.rc;
	  }
	}

	rc = L7_FAILURE;
	loginStatus = L7_DISABLE;

   index+=1;
   memset(nextObjUserNameValue, 0x00, sizeof(nextObjUserNameValue));
 

	while (index < L7_MAX_LOGINS && rc != L7_SUCCESS)
	{
	  
	  rc = usmDbLoginStatusGet(L7_UNIT_CURRENT, 
			    index, &loginStatus);
	  if (loginStatus != L7_ENABLE)
	  {
	    rc = L7_FAILURE;
	  }
	  index = index + 1;
	}

	 if( (rc != L7_SUCCESS) || (index >= L7_MAX_LOGINS) )
	 {
	    owa.l7rc = L7_FAILURE;
	 }
	 else
	 {
	    owa.l7rc = usmDbLoginsGet(L7_UNIT_CURRENT, index-1, nextObjUserNameValue);
	 }
	
	if (owa.l7rc != L7_SUCCESS)
	{
	  owa.rc = XLIBRC_ENDOF_TABLE;
	  FPOBJ_TRACE_EXIT (bufp, owa);
	  return owa.rc;
	}
	FPOBJ_TRACE_NEW_KEY (bufp, nextObjUserNameValue, owa.len);

	/* return the object value: UserName */
	owa.rc =
	  xLibBufDataSet (bufp, (xLibU8_t *)  nextObjUserNameValue,
	                  strlen (nextObjUserNameValue));
	FPOBJ_TRACE_EXIT (bufp, owa);
	return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseUsrMgrAuthList_AuthListForNonDefaultUser
*
* @purpose Get 'AuthListForNonDefaultUser'
*
* @description The APL assigned to given configured users for the User Manager component. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrAuthList_AuthListForNonDefaultUser (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyUserNameValue;
	
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAuthListForNonDefaultUserValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UserName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrAuthList_UserName,
                          (xLibU8_t *)  keyUserNameValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyUserNameValue, kwa.len);

  /* get the value from application */
  owa.l7rc = 
    usmDbAPLUserGet(L7_UNIT_CURRENT, keyUserNameValue, 
                L7_USER_MGR_COMPONENT_ID, objAuthListForNonDefaultUserValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AuthListForNonDefaultUser */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *)  objAuthListForNonDefaultUserValue,
                    strlen (objAuthListForNonDefaultUserValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjList_TestuspRange_AuthListForNonDefaultUser
*
* @purpose Set 'AuthListForNonDefaultUser'
*
* @description The APL assigned to given configured users for the User Manager component. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseUsrMgrAuthList_AuthListForNonDefaultUser (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
	
  xLibStr256_t objAuthListForNonDefaultUserValue;
  xLibStr256_t nextObjAuthListForNonDefaultUserValue;

  xLibS32_t index=0;
  L7_RC_t rc;
	
  FPOBJ_TRACE_ENTER (bufp);

  memset(objAuthListForNonDefaultUserValue, 0x00,
		    sizeof(objAuthListForNonDefaultUserValue));

  /* retrieve key: UserName */
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrAuthList_AuthListForNonDefaultUser,
                          (xLibU8_t *) objAuthListForNonDefaultUserValue,
                          &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
	index = -1;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objAuthListForNonDefaultUserValue, owa.len);
	/* owa.l7rc = usmDbAPLIndexGetByName(L7_UNIT_CURRENT,
	 	         objAuthListForNonDefaultUserValue, &index );*/
	 if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }


  rc = L7_FAILURE;
  index = index + 1;
  while (index < L7_MAX_APL_COUNT && rc != L7_SUCCESS)
  {
    rc = usmDbAPLListGetIndex(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, index,
			            nextObjAuthListForNonDefaultUserValue);
	index = index + 1;

  }

  if( (rc != L7_SUCCESS) || (index >= L7_MAX_APL_COUNT)  )
  {
    owa.l7rc = L7_FAILURE;
  }
  else
  {
    owa.l7rc = L7_SUCCESS;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
	
  FPOBJ_TRACE_NEW_KEY (bufp, nextObjAuthListForNonDefaultUserValue, owa.len);

  /* return the object value: UserName */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *)  nextObjAuthListForNonDefaultUserValue,
                    strlen (nextObjAuthListForNonDefaultUserValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseUsrMgrAuthList_AuthListForNonDefaultUser
*
* @purpose Set 'AuthListForNonDefaultUser'
*
* @description The APL assigned to given configured users for the User Manager component. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrAuthList_AuthListForNonDefaultUser(void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAuthListForNonDefaultUserValue;
	
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyUserNameValue;
	
  FPOBJ_TRACE_ENTER (bufp);
  memset(objAuthListForNonDefaultUserValue, 0x00,
		           sizeof(objAuthListForNonDefaultUserValue));

  /* retrieve object: AuthListForNonDefaultUser */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objAuthListForNonDefaultUserValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objAuthListForNonDefaultUserValue, owa.len);

  /* retrieve key: UserName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrAuthList_UserName,
                          (xLibU8_t *) keyUserNameValue,
                          &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyUserNameValue, kwa.len);

  /* set the value in application */
  owa.l7rc = 
    usmDbAPLUserSet(L7_UNIT_CURRENT, keyUserNameValue, 
                L7_USER_MGR_COMPONENT_ID, objAuthListForNonDefaultUserValue);
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
* @function fpObjGet_baseUsrMgrAuthList_AuthListForDefaultUser
*
* @purpose Get 'AuthListForDefaultUser'
*
* @description the APL assigned to nonconfigured users for the user manager component.. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrAuthList_AuthListForDefaultUser (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAuthListForDefaultUserValue;
  FPOBJ_TRACE_ENTER (bufp);

  memset(objAuthListForDefaultUserValue, 0x00,
		     sizeof(objAuthListForDefaultUserValue));
  /* get the value from application */
  owa.l7rc = 
    usmDbAPLNonConfiguredUserGet(L7_UNIT_CURRENT, 
                    L7_USER_MGR_COMPONENT_ID,
                    objAuthListForDefaultUserValue );

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AuthListForDefaultUser */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *)  objAuthListForDefaultUserValue,
                    strlen (objAuthListForDefaultUserValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjList_TestuspRange_AuthListForDefaultUser
*
* @purpose Set 'AuthListForDefaultUser'
*
* @description the APL assigned to nonconfigured users for the user manager component.. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseUsrMgrAuthList_AuthListForDefaultUser (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
	
  xLibStr256_t objAuthListForDefaultUserValue;
  xLibStr256_t nextObjAuthListForDefaultUserValue;

  xLibS32_t index=0;
  L7_RC_t rc;
	
  FPOBJ_TRACE_ENTER (bufp);

  memset(objAuthListForDefaultUserValue, 0x00,
		    sizeof(objAuthListForDefaultUserValue));

  /* retrieve key: UserName */
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrAuthList_AuthListForDefaultUser,
                          (xLibU8_t *) objAuthListForDefaultUserValue,
                          &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
	index = -1;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objAuthListForDefaultUserValue, owa.len);
	 /*owa.l7rc = usmDbAPLIndexGetByName(L7_UNIT_CURRENT,
	 	         objAuthListForDefaultUserValue, &index );*/
	 if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }


  rc = L7_FAILURE;
  index = index + 1;
  while (index < L7_MAX_APL_COUNT && rc != L7_SUCCESS)
  {
    
    rc = usmDbAPLListGetIndex(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, index,
			            nextObjAuthListForDefaultUserValue);
	index = index + 1;
  }

  if( (rc != L7_SUCCESS) || (index >= L7_MAX_APL_COUNT)  )
  {
    owa.l7rc = L7_FAILURE;
  }
  else
  {
    owa.l7rc = L7_SUCCESS;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
	
  FPOBJ_TRACE_NEW_KEY (bufp, nextObjAuthListForDefaultUserValue, owa.len);

  /* return the object value: UserName */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *)  nextObjAuthListForDefaultUserValue,
                    strlen (nextObjAuthListForDefaultUserValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseUsrMgrAuthList_AuthListForDefaultUser
*
* @purpose Set 'AuthListForDefaultUser'
*
* @description the APL assigned to nonconfigured users for the user manager component.. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrAuthList_AuthListForDefaultUser(void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAuthListForDefaultUserValue;
		
  FPOBJ_TRACE_ENTER (bufp);
  memset(objAuthListForDefaultUserValue, 0x00,
		           sizeof(objAuthListForDefaultUserValue));

  /* retrieve object: AuthListForDefaultUser */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objAuthListForDefaultUserValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objAuthListForDefaultUserValue, owa.len);


  /* set the value in application */
  owa.l7rc = 
    usmDbAPLNonConfiguredUserSet(L7_UNIT_CURRENT, 
                    L7_USER_MGR_COMPONENT_ID,
                    objAuthListForDefaultUserValue );
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
* @function fpObjGet_baseUsrMgrAuthList_MethodList
*
* @purpose Get 'MethodList'
*
* @description
*             
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrAuthList_MethodList (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAuthenticationListIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objMethodListValue;

  xLibStr256_t tempAuthListName;
  memset(tempAuthListName,0x00,sizeof(tempAuthListName));

  xLibU32_t temp,i;
  L7_BOOL flag = L7_FALSE;
  FPOBJ_TRACE_ENTER (bufp);

  memset(objMethodListValue,0x00,sizeof(objMethodListValue));
  /* retrieve key: AuthenticationListIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrAuthList_AuthenticationListIndex,
                          (xLibU8_t *) & keyAuthenticationListIndexValue,
                          &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAuthenticationListIndexValue, kwa.len);

  owa.l7rc = usmDbAPLListGetIndex(ACCESS_LINE_CTS,
                                                  ACCESS_LEVEL_LOGIN,
               keyAuthenticationListIndexValue, tempAuthListName);
  if (owa.l7rc == L7_SUCCESS)
  {
     for (i = 0; i < L7_MAX_AUTH_METHODS; i++)
     {
      
     /* get the value from application */
       owa.l7rc = usmDbAPLAuthMethodGet (ACCESS_LINE_CTS,
                                                  ACCESS_LEVEL_LOGIN,
                                         tempAuthListName,
                                         i,
                                         &temp);
       
       if (owa.l7rc == L7_SUCCESS)
       {
         switch (temp)        /* auth method */
         {
           case L7_AUTH_METHOD_LOCAL:
             if(flag == L7_TRUE )
             {
               osapiStrncat(objMethodListValue,",Local",sizeof(objMethodListValue));
             }
             else
             {
               osapiStrncat(objMethodListValue,"Local",sizeof(objMethodListValue));
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
               osapiStrncat(objMethodListValue,",Reject",sizeof(objMethodListValue));  
             }
             else
             {
               osapiStrncat(objMethodListValue,"Reject",sizeof(objMethodListValue));  
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

/*******************************************************************************
* @function fpObjGet_baseUsrMgrAuthList_AuthenticationListLoginUsers
*
* @purpose Get 'AuthenticationListLoginUsers'
*
* @description
*
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrAuthList_AuthenticationListLoginUsers (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAuthenticationListIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAuthenticationListLoginUsersValue;

  xLibStr256_t tempAuthListName;
  memset(tempAuthListName,0x00,sizeof(tempAuthListName));
  L7_uint32 val, prev, i;
  L7_RC_t rc;
  xLibStr256_t temp;
  memset(temp,0x00,sizeof(temp));
  xLibStr256_t temp1;
  memset(temp,0x00,sizeof(temp1));
  xLibStr256_t prevbuf;
  memset(prevbuf,0x00,sizeof(prevbuf));
  
  FPOBJ_TRACE_ENTER (bufp);

  memset(objAuthenticationListLoginUsersValue,0x00,
         sizeof(objAuthenticationListLoginUsersValue));

  /* retrieve key: AuthenticationListIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrAuthList_AuthenticationListIndex,
                          (xLibU8_t *) & keyAuthenticationListIndexValue,
                          &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAuthenticationListIndexValue, kwa.len);

  owa.l7rc = usmDbAPLListGetIndex(ACCESS_LINE_CTS,
                                                  ACCESS_LEVEL_LOGIN, keyAuthenticationListIndexValue, 
                                  tempAuthListName);

  if (owa.l7rc == L7_SUCCESS)
  {
     /* get the value from application */
     i = 0;
     rc = usmDbAPLAllUsersGetFirst(L7_UNIT_CURRENT, tempAuthListName, temp, &val);
     while ( rc == L7_SUCCESS)
     {
       if (val == L7_USER_MGR_COMPONENT_ID)
       {
         if(i>0) 
         {
           osapiStrncat(objAuthenticationListLoginUsersValue,",",sizeof(objAuthenticationListLoginUsersValue)); 
           osapiStrncat(objAuthenticationListLoginUsersValue,temp,sizeof(objAuthenticationListLoginUsersValue));
         }
         else
         {
           osapiStrncat(objAuthenticationListLoginUsersValue,temp,sizeof(objAuthenticationListLoginUsersValue));
         }
        i++;  
       } 
       else if (val == L7_DOT1X_COMPONENT_ID)
       {
         memcpy(temp1, temp, sizeof(temp));
       }
       prev = val;
       memcpy(prevbuf, temp, sizeof(temp));
       rc = usmDbAPLAllUsersGetNext(L7_UNIT_CURRENT, tempAuthListName, prevbuf, prev,
                                          temp, &val);
     }
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  /* return the object value: AuthListForDefaultUser */
  owa.rc =  xLibBufDataSet (bufp, (xLibU8_t *)  objAuthenticationListLoginUsersValue,
                            strlen (objAuthenticationListLoginUsersValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrAuthList_AuthenticationList802xUsers
*
* @purpose Get 'AuthenticationList802xUsers'
*
* @description
*
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrAuthList_AuthenticationList802xUsers (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAuthenticationListIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAuthenticationList802xUsersValue;

  xLibStr256_t tempAuthListName;
  memset(tempAuthListName,0x00,sizeof(tempAuthListName));
  L7_uint32 val, prev, i;
  L7_RC_t rc;
  xLibStr256_t temp;
  memset(temp,0x00,sizeof(temp));
  xLibStr256_t temp1;
  memset(temp1,0x00,sizeof(temp1));
  xLibStr256_t prevbuf;
  memset(prevbuf,0x00,sizeof(prevbuf));

  FPOBJ_TRACE_ENTER (bufp);

  memset(objAuthenticationList802xUsersValue,0x00,
         sizeof(objAuthenticationList802xUsersValue));

  /* retrieve key: AuthenticationListIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrAuthList_AuthenticationListIndex,
                          (xLibU8_t *) & keyAuthenticationListIndexValue,
                          &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAuthenticationListIndexValue, kwa.len);

  owa.l7rc = usmDbAPLListGetIndex(ACCESS_LINE_CTS,
                                                  ACCESS_LEVEL_LOGIN, keyAuthenticationListIndexValue,
                                  tempAuthListName);

  if (owa.l7rc == L7_SUCCESS)
  {
     /* get the value from application */
     i = 0;
     rc = usmDbAPLAllUsersGetFirst(L7_UNIT_CURRENT, tempAuthListName, temp, &val);
     while ( rc == L7_SUCCESS)
     {
       if (val == L7_DOT1X_COMPONENT_ID)
       {
         if(i>0)
         {
           osapiStrncat(objAuthenticationList802xUsersValue,",",sizeof(objAuthenticationList802xUsersValue));
           osapiStrncat(objAuthenticationList802xUsersValue,temp,sizeof(objAuthenticationList802xUsersValue));
         }
         else
         {
           osapiStrncat(objAuthenticationList802xUsersValue,temp,sizeof(objAuthenticationList802xUsersValue));
         }
        i++;
       }
       else if (val == L7_USER_MGR_COMPONENT_ID)
       {
         memcpy(temp1, temp, sizeof(temp));
       }
       prev = val;
       memcpy(prevbuf, temp, sizeof(temp));
       rc = usmDbAPLAllUsersGetNext(L7_UNIT_CURRENT,tempAuthListName, prevbuf, prev,
                                          temp, &val);
     }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  /* return the object value: AuthListForDefaultUser */
  owa.rc =  xLibBufDataSet (bufp, (xLibU8_t *)  objAuthenticationList802xUsersValue,
                            strlen (objAuthenticationList802xUsersValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
