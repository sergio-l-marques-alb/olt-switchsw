
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_securitydot1xUserConfig.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to security-object.xml
*
* @create  19 July 2008, Saturday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_securitydot1xUserConfig_obj.h"
#include "usmdb_user_mgmt_api.h"
#include "user_manager_exports.h"

/*******************************************************************************
* @function fpObjGet_securitydot1xUserConfig_UserName
*
* @purpose Get 'UserName'
 *@description  [UserName] The list of the users assigned to the authentication
* list.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1xUserConfig_UserName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objUserNameValue;
  xLibStr256_t nextObjUserNameValue;
  memset(objUserNameValue,0x00,sizeof(objUserNameValue));
  memset(nextObjUserNameValue,0x00,sizeof(nextObjUserNameValue));
  xLibS32_t index;
  L7_RC_t rc;
  xLibU32_t loginStatus;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UserName */
  owa.len = sizeof (objUserNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitydot1xUserConfig_UserName,
                          (xLibU8_t *) objUserNameValue, &owa.len);

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
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjUserNameValue, strlen (nextObjUserNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1xUserConfig_AuthListForNonDefaultUser
*
* @purpose Get 'AuthListForNonDefaultUser'
 *@description  [AuthListForNonDefaultUser] The APL assigned to given configured
* users for the User Manager component.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1xUserConfig_AuthListForNonDefaultUser (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objAuthListForNonDefaultUserValue;

  xLibStr256_t keyUserNameValue;
  memset(keyUserNameValue,0x00,sizeof(keyUserNameValue));
  memset(objAuthListForNonDefaultUserValue,0x00,sizeof(objAuthListForNonDefaultUserValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UserName */
  owa.len = sizeof (keyUserNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitydot1xUserConfig_UserName,
                          (xLibU8_t *) keyUserNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyUserNameValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbAPLUserGet(L7_UNIT_CURRENT, keyUserNameValue,
                L7_DOT1X_COMPONENT_ID, objAuthListForNonDefaultUserValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objAuthListForNonDefaultUserValue,
                     strlen (objAuthListForNonDefaultUserValue));

  /* return the object value: AuthListForNonDefaultUser */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objAuthListForNonDefaultUserValue,
                           strlen (objAuthListForNonDefaultUserValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_securitydot1xUserConfig_AuthListForNonDefaultUser
*
* @purpose List 'AuthListForNonDefaultUser'
 *@description  [AuthListForNonDefaultUser] The APL assigned to given configured
* users for the User Manager component.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_securitydot1xUserConfig_AuthListForNonDefaultUser (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objAuthListForNonDefaultUserValue;
  xLibStr256_t nextObjAuthListForNonDefaultUserValue;
  memset(nextObjAuthListForNonDefaultUserValue,0x00,sizeof(nextObjAuthListForNonDefaultUserValue));
  memset(objAuthListForNonDefaultUserValue,0x00,sizeof(objAuthListForNonDefaultUserValue));
  xLibS32_t index=0;
  L7_RC_t rc;

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (objAuthListForNonDefaultUserValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitydot1xUserConfig_AuthListForNonDefaultUser,
                          (xLibU8_t *) objAuthListForNonDefaultUserValue, &owa.len);
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

  /* return the object value: AuthListForNonDefaultUser */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjAuthListForNonDefaultUserValue,
                           strlen (nextObjAuthListForNonDefaultUserValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_securitydot1xUserConfig_AuthListForNonDefaultUser
*
* @purpose Set 'AuthListForNonDefaultUser'
 *@description  [AuthListForNonDefaultUser] The APL assigned to given configured
* users for the User Manager component.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydot1xUserConfig_AuthListForNonDefaultUser (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objAuthListForNonDefaultUserValue;

  xLibStr256_t keyUserNameValue;
  memset(keyUserNameValue,0x00,sizeof(keyUserNameValue));
  memset(objAuthListForNonDefaultUserValue,0x00,sizeof(objAuthListForNonDefaultUserValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AuthListForNonDefaultUser */
  owa.len = sizeof (objAuthListForNonDefaultUserValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objAuthListForNonDefaultUserValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objAuthListForNonDefaultUserValue, owa.len);

  /* retrieve key: UserName */
  owa.len = sizeof (keyUserNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitydot1xUserConfig_UserName,
                          (xLibU8_t *) keyUserNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyUserNameValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbAPLUserSet(L7_UNIT_CURRENT, keyUserNameValue,
                L7_DOT1X_COMPONENT_ID, objAuthListForNonDefaultUserValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1xUserConfig_AuthListForDefaultUser
*
* @purpose Get 'AuthListForDefaultUser'
 *@description  [AuthListForDefaultUser] the APL assigned to nonconfigured users
* for the user manager component.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1xUserConfig_AuthListForDefaultUser (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objAuthListForDefaultUserValue;
   memset(objAuthListForDefaultUserValue, 0x00,
         sizeof(objAuthListForDefaultUserValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbAPLNonConfiguredUserGet(L7_UNIT_CURRENT,
                    L7_DOT1X_COMPONENT_ID,
                    objAuthListForDefaultUserValue );
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objAuthListForDefaultUserValue, strlen (objAuthListForDefaultUserValue));

  /* return the object value: AuthListForDefaultUser */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objAuthListForDefaultUserValue,
                           strlen (objAuthListForDefaultUserValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_securitydot1xUserConfig_AuthListForDefaultUser
*
* @purpose List 'AuthListForDefaultUser'
 *@description  [AuthListForDefaultUser] the APL assigned to nonconfigured users
* for the user manager component.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_securitydot1xUserConfig_AuthListForDefaultUser (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objAuthListForDefaultUserValue;
  xLibStr256_t nextObjAuthListForDefaultUserValue;

  xLibS32_t index=0;
  L7_RC_t rc;

  memset(objAuthListForDefaultUserValue, 0x00,
        sizeof(objAuthListForDefaultUserValue));
  memset(nextObjAuthListForDefaultUserValue, 0x00,
        sizeof(nextObjAuthListForDefaultUserValue));
  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (objAuthListForDefaultUserValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitydot1xUserConfig_AuthListForDefaultUser,
                          (xLibU8_t *) objAuthListForDefaultUserValue, &owa.len);
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

  /* return the object value: AuthListForDefaultUser */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjAuthListForDefaultUserValue,
                           strlen (nextObjAuthListForDefaultUserValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_securitydot1xUserConfig_AuthListForDefaultUser
*
* @purpose Set 'AuthListForDefaultUser'
 *@description  [AuthListForDefaultUser] the APL assigned to nonconfigured users
* for the user manager component.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydot1xUserConfig_AuthListForDefaultUser (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objAuthListForDefaultUserValue;
  memset(objAuthListForDefaultUserValue, 0x00,
               sizeof(objAuthListForDefaultUserValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AuthListForDefaultUser */
  owa.len = sizeof (objAuthListForDefaultUserValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objAuthListForDefaultUserValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objAuthListForDefaultUserValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbAPLNonConfiguredUserSet(L7_UNIT_CURRENT,
                    L7_DOT1X_COMPONENT_ID,
                    objAuthListForDefaultUserValue );

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************
* @function  fpObjSet_securitydot1xUserConfig_AuthMethods
*
* @purpose Set 'AuthMethods'
 *@description  [AuthMethods] To configure Dot1x User Auth Method
* 
* @notes
*
* @return
*******************************************************************/
xLibRC_t fpObjSet_securitydot1xUserConfig_AuthMethods (void *wap, void *bufp)
{
    xLibU32_t  authList,i;
   xLibU16_t  len = sizeof(authList);
   xLibU8_t   listName[15];
   L7_RC_t rc = L7_FAILURE;

   rc = xLibBufDataGet (bufp,(xLibU8_t *)&authList, &len);
   if(rc !=  XLIBRC_SUCCESS)
   {
       return XLIBRC_FAILURE;
   }
   memset(listName,0x0,sizeof(listName));
   if (authList == L7_DOT1X_AAA_RADIUSREJECT)
   {
    memcpy(listName,"Radius",sizeof(listName));
   }
   else if (authList == L7_DOT1X_AAA_REJECT)
   {
    memcpy(listName,"Reject",sizeof(listName));
   }

   else if(authList == L7_DOT1X_AAA_LOCAL)
   {
    memcpy(listName,"Local",sizeof(listName));
   }

   if (usmDbAuthenticationListIndexGetByName(ACCESS_LINE_DOT1X,0,listName,&i) !=L7_SUCCESS){
     rc = usmDbAPLCreate(ACCESS_LINE_DOT1X,0,listName);
     if(rc != L7_SUCCESS)
     {
       return XLIBRC_FAILURE;
     }
 }
   if (authList == L7_DOT1X_AAA_RADIUSREJECT)
   {
     rc = usmDbAPLAuthMethodSet(ACCESS_LINE_DOT1X,0, listName, 0, L7_AUTH_METHOD_RADIUS);
     if(rc != L7_SUCCESS)
     {
       return XLIBRC_FAILURE;
     }
     rc = usmDbAPLAuthMethodSet(ACCESS_LINE_DOT1X,0, listName, 1, L7_AUTH_METHOD_REJECT);
     if(rc != L7_SUCCESS)
     {
       return XLIBRC_FAILURE;
     }
   }
   else if (authList == L7_DOT1X_AAA_REJECT)
   {
     rc = usmDbAPLAuthMethodSet(ACCESS_LINE_DOT1X,0, listName, 0, L7_AUTH_METHOD_REJECT);
     if(rc != L7_SUCCESS)
     {
       return XLIBRC_FAILURE;
     }
   }
   else if(authList == L7_DOT1X_AAA_LOCAL)
   {
     rc = usmDbAPLAuthMethodSet(ACCESS_LINE_DOT1X,0, listName, 0, L7_AUTH_METHOD_LOCAL);
     if(rc != L7_SUCCESS)
     {
       return XLIBRC_FAILURE;
     }
   }
   if (usmDbAPLNonConfiguredUserSet(0,L7_DOT1X_COMPONENT_ID, listName) != L7_SUCCESS)
     return XLIBRC_FAILURE;

   return XLIBRC_SUCCESS;
}

/**************************************************************************
* @function  fpObjGet_securitydot1xUserConfig_AuthMethods
*
* @purpose Set 'AuthMethods'
 *@description  [AuthMethods] To Get the configured Dot1x User Auth Method
*
* @notes
*
* @return
***************************************************************************/
xLibRC_t fpObjGet_securitydot1xUserConfig_AuthMethods (void *wap, void *bufp)
{
   xLibU8_t   listName[L7_MAX_APL_NAME_SIZE + 1];
   xLibU32_t  val;

  if (usmDbAPLNonConfiguredUserGet(0,L7_DOT1X_COMPONENT_ID,(L7_char8*)listName) != L7_SUCCESS)
    return XLIBRC_FAILURE;

  if (strcmp(listName,"Radius") == 0)
  {
    val = L7_DOT1X_AAA_RADIUSREJECT;
  }
  else if (strcmp(listName,"Reject") == 0)
  {
    val = L7_DOT1X_AAA_REJECT;
  }
  else
  {
    val = L7_DOT1X_AAA_LOCAL;
  }
  xLibBufDataSet(bufp, (xLibU8_t *)&val,sizeof(val));
  return XLIBRC_SUCCESS;
}

