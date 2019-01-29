/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2009
*
********************************************************************************
*
* @filename fpobj_baseUserMgrIntAuthServUserDBConfig.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  18 November 2009, Wednesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  pradeepk
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_baseUserMgrIntAuthServUserDBConfig_obj.h"
#include "snmp_exports.h"
#include "usmdb_util_api.h"
#include "user_manager_exports.h"
#include "usmdb_util_api.h"
#include "user_mgr_api.h"
#include "usmdb_user_mgmt_api.h"
#include "dot1x_auth_serv_exports.h"

/*******************************************************************************
* @function fpObjUtil_baseUserMgrIntAuthServUserDBConfig_UserIndexGet
*
* @purpose Get 'UserIndex'
 *@description  [UserIndex] Get the first available user index   
* @notes       
*
* @return
*******************************************************************************/
L7_RC_t fpObjUtil_baseUserMgrIntAuthServUserDBConfig_UserIndexGet ( xLibU32_t *userIndexFirstGet)
{
  xLibU32_t index ;
  xLibU32_t userStatus;
  L7_BOOL searchFlag; 
  xLibStr256_t userNameValue;
  L7_RC_t rc;
 
  memset(userNameValue, 0x00, sizeof(userNameValue));
  searchFlag = L7_FALSE;
  index = 0;
  while (index < L7_MAX_IAS_USERS )
  {
    if( (usmDbUserMgrIASUserDBUserStatusGet(index, &userStatus) == L7_SUCCESS) &&
      (userStatus == L7_ENABLE) )
    {
         if( (usmDbUserMgrIASUserDBUserNameGet(index, userNameValue, sizeof(userNameValue)) == L7_SUCCESS )  &&
           (userNameValue[0] != '\0')  )
         {
             searchFlag = L7_TRUE;
              break;
         }
    }
    index = index + 1;
  }
 
  if (searchFlag != L7_TRUE) 
  {
       rc = L7_FAILURE;
  }
  else
  {
    rc = usmDbUserMgrIASUserDBUserNameGet(index, userNameValue, sizeof(userNameValue));
    *userIndexFirstGet = index;
  }
  return rc;
}

/*******************************************************************************
* @function fpObjUtil_baseUserMgrIntAuthServUserDBConfig_UserIndexNextGet
*
* @purpose Get 'UserIndex'
 *@description  [UserIndex] Get the next available user index   
* @notes       
*
* @return
*******************************************************************************/
L7_RC_t 
  fpObjUtil_baseUserMgrIntAuthServUserDBConfig_UserIndexNextGet ( xLibU32_t userIndex, xLibU32_t *userIndexNext)
{
  xLibU32_t index ;
  xLibU32_t userStatus;
  L7_BOOL searchFlag; 
  xLibStr256_t userNameValue;
  L7_RC_t rc;
 
  searchFlag = L7_FALSE;
  index = userIndex+1;
  while (index < L7_MAX_IAS_USERS )
  {
    if( (usmDbUserMgrIASUserDBUserStatusGet(index, &userStatus) == L7_SUCCESS) &&
      (userStatus == L7_ENABLE) )
    {
         if( (usmDbUserMgrIASUserDBUserNameGet(index, userNameValue, sizeof(userNameValue)) == L7_SUCCESS )  &&
           (userNameValue[0] != '\0')  )
         {
             searchFlag = L7_TRUE;
              break;
         }
    }
    index = index + 1;
  }
 
  if (searchFlag != L7_TRUE) 
  {
       rc = L7_FAILURE;
  }
  else
  {
    rc = usmDbUserMgrIASUserDBUserNameGet(index, userNameValue, sizeof(userNameValue));
    *userIndexNext = index;
  }
  
  return rc;
}


/*******************************************************************************
* @function fpObjGet_baseUserMgrIntAuthServUserDBConfig_UserIndex
*
* @purpose Get 'UserIndex'
 *@description  [UserIndex] Login table index   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUserMgrIntAuthServUserDBConfig_UserIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objUserIndexValue;
    xLibU32_t nextObjUserIndexValue;



  FPOBJ_TRACE_ENTER(bufp);

   


      /* retrieve key: UserIndex */
      owa.len = sizeof(objUserIndexValue);
      owa.rc = xLibFilterGet(wap, XOBJ_baseUserMgrIntAuthServUserDBConfig_UserIndex,
                  (xLibU8_t *) &objUserIndexValue, &owa.len);
      if (owa.rc != XLIBRC_SUCCESS)
      {
        FPOBJ_TRACE_CURRENT_KEY(bufp, NULL, 0);
        objUserIndexValue = 0;
        nextObjUserIndexValue = 0;
                owa.l7rc =fpObjUtil_baseUserMgrIntAuthServUserDBConfig_UserIndexGet(&nextObjUserIndexValue);
      }
      else
      {
        FPOBJ_TRACE_CURRENT_KEY(bufp, &objUserIndexValue, owa.len);
        owa.l7rc =
           fpObjUtil_baseUserMgrIntAuthServUserDBConfig_UserIndexNextGet ( objUserIndexValue,
                                            &nextObjUserIndexValue);
      }

  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }


    FPOBJ_TRACE_NEW_KEY(bufp, &nextObjUserIndexValue, owa.len);

  /* return the object value: UserIndex */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&nextObjUserIndexValue,
                      sizeof(nextObjUserIndexValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}



/*******************************************************************************
* @function fpObjGet_baseUserMgrIntAuthServUserDBConfig_UserName
*
* @purpose Get 'UserName'
 *@description  [UserName] The user name per index   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUserMgrIntAuthServUserDBConfig_UserName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibStr256_t objUserNameValue;

    xLibU32_t keyUserIndexValue;


  FPOBJ_TRACE_ENTER(bufp);



    /* retrieve key: UserIndex */
    owa.len = sizeof(keyUserIndexValue);
    owa.rc = xLibFilterGet(wap, XOBJ_baseUserMgrIntAuthServUserDBConfig_UserIndex,
                                (xLibU8_t *) &keyUserIndexValue, 
                                &owa.len);
    if(owa.rc != XLIBRC_SUCCESS)
    {
      owa.rc = XLIBRC_FILTER_MISSING; 
      FPOBJ_TRACE_EXIT(bufp, owa);
      return owa.rc;
    }

    FPOBJ_TRACE_CURRENT_KEY(bufp, &keyUserIndexValue, 
            owa.len);

    /* get the value from application */
    owa.l7rc = usmDbUserMgrIASUserDBUserNameGet (keyUserIndexValue, objUserNameValue, sizeof(objUserNameValue));
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }

    FPOBJ_TRACE_VALUE (bufp, objUserNameValue, strlen(objUserNameValue));

  /* return the object value: UserName */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)objUserNameValue,
                      strlen(objUserNameValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseUserMgrIntAuthServUserDBConfig_UserName
*
* @purpose Set 'UserName'
 *@description  [UserName] The user name per index   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUserMgrIntAuthServUserDBConfig_UserName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUserNameValue;
  fpObjWa_t kwaUserIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUserIndexValue = -1;
  FPOBJ_TRACE_ENTER (bufp);

/*get the row status */

  L7_uint32 tempInt;
  L7_char8 temp_buf[255];

  fpObjWa_t kwaRowStatus = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;

  /* retrieve object: UserStatus */
   kwaRowStatus.rc = xLibFilterGet (wap, XOBJ_baseUserMgrIntAuthServUserDBConfig_UserStatus,
                                   (xLibU8_t *) & objRowStatusValue,
                                   &kwaRowStatus.len);
  if (kwaRowStatus.rc == XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objRowStatusValue, kwaUserIndex.len);
     if( (objRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO) ||
            (objRowStatusValue == L7_ROW_STATUS_CREATE_AND_WAIT) )
    {
        return XLIBRC_SUCCESS;
    } 
  } 

  /* retrieve key: UserIndex */
  kwaUserIndex.rc = xLibFilterGet (wap, XOBJ_baseUserMgrIntAuthServUserDBConfig_UserIndex,
                                   (xLibU8_t *) & keyUserIndexValue,
                                   &kwaUserIndex.len);
  if (kwaUserIndex.rc != XLIBRC_SUCCESS)
  {
      kwaUserIndex.rc = XLIBRC_USER_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, kwaUserIndex);
      return kwaUserIndex.rc;
  }

  /* retrieve object: UserName */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objUserNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objUserNameValue, owa.len);

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUserIndexValue, kwaUserIndex.len);
  /* set the value in application */
  /* check name for correct characters and length */
  if( strlen(objUserNameValue) >= L7_LOGIN_SIZE)
  {
    owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (usmDbStringAlphaNumericCheck(objUserNameValue) != L7_SUCCESS)
  {
     owa.rc = XLIBRC_USERNAME_ALPHA_NUMERIC_ERROR;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* check all users for an existing entry by that name */
  for (tempInt = 0; tempInt < L7_MAX_IAS_USERS; tempInt++)
  {
    if (tempInt != keyUserIndexValue)
    {
      memset(temp_buf, 0x00,sizeof(temp_buf));
      usmDbUserMgrIASUserDBUserNameGet(tempInt, temp_buf, sizeof(temp_buf));
      if (usmDbStringCaseInsensitiveCompare(temp_buf, objUserNameValue) == L7_SUCCESS)
      {
      owa.rc = XLIBRC_ALREADY_EXISTS;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
      }
    }
  }
  owa.l7rc = usmDbUserMgrIASUserDBUserNameSet(keyUserIndexValue,
                                 objUserNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_LOGIN_SET_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_baseUserMgrIntAuthServUserDBConfig_UserPassword
*
* @purpose Get 'UserPassword'
 *@description  [UserPassword] The dot1x user password in clear text per index   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUserMgrIntAuthServUserDBConfig_UserPassword (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibStr256_t objUserPasswordValue;

    xLibU32_t keyUserIndexValue;


  FPOBJ_TRACE_ENTER(bufp);



    /* retrieve key: UserIndex */
    owa.len = sizeof(keyUserIndexValue);
    owa.rc = xLibFilterGet(wap, XOBJ_baseUserMgrIntAuthServUserDBConfig_UserIndex,
                                (xLibU8_t *) &keyUserIndexValue, 
                                &owa.len);
    if(owa.rc != XLIBRC_SUCCESS)
    {
      owa.rc = XLIBRC_FILTER_MISSING; 
      FPOBJ_TRACE_EXIT(bufp, owa);
      return owa.rc;
    }

    FPOBJ_TRACE_CURRENT_KEY(bufp, &keyUserIndexValue, 
            owa.len);

    /* get the value from application */
    memset(objUserPasswordValue, 0x00, sizeof(objUserPasswordValue));
    owa.l7rc = L7_SUCCESS;
  
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }

    FPOBJ_TRACE_VALUE (bufp, objUserPasswordValue, strlen(objUserPasswordValue));

  /* return the object value: UserPassword */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)objUserPasswordValue,
                      strlen(objUserPasswordValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseUserMgrIntAuthServUserDBConfig_UserPassword
*
* @purpose Set 'UserPassword'
 *@description  [UserPassword] The dot1x user password in clear text per index   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUserMgrIntAuthServUserDBConfig_UserPassword (void *wap, void *bufp)
{
   fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
   xLibStr256_t objUserPasswordValue;
   char buf[L7_PASSWORD_SIZE];
   fpObjWa_t kwaUserIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
   xLibU32_t keyUserIndexValue;
   FPOBJ_TRACE_ENTER (bufp);
  
   fpObjWa_t kwaRowStatus = FPOBJ_INIT_WA (sizeof (xLibU32_t));
   xLibU32_t objRowStatusValue;
  
   /* retrieve object: UserStatus */
    kwaRowStatus.rc = xLibFilterGet (wap, XOBJ_baseUserMgrIntAuthServUserDBConfig_UserStatus,
                                    (xLibU8_t *) & objRowStatusValue,
                                    &kwaRowStatus.len);
   if (kwaRowStatus.rc == XLIBRC_SUCCESS)
   {
     FPOBJ_TRACE_CURRENT_KEY (bufp, &objRowStatusValue, kwaUserIndex.len);
  
      if( (objRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO) ||
             (objRowStatusValue == L7_ROW_STATUS_CREATE_AND_WAIT) )
     {
         return XLIBRC_SUCCESS;
     }  
   }
         /* retrieve key: UserIndex */
   kwaUserIndex.rc = xLibFilterGet (wap, XOBJ_baseUserMgrIntAuthServUserDBConfig_UserIndex,
                                    (xLibU8_t *) & keyUserIndexValue,
                                    &kwaUserIndex.len);
   if (kwaUserIndex.rc != XLIBRC_SUCCESS)
   {
       kwaUserIndex.rc = XLIBRC_USER_FAILURE;
       FPOBJ_TRACE_EXIT (bufp, kwaUserIndex);
       return kwaUserIndex.rc;
   }
  
   /* retrieve object: UserPassword */
   owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objUserPasswordValue, &owa.len);
   if (owa.rc != XLIBRC_SUCCESS)
   {
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
   }
   FPOBJ_TRACE_VALUE (bufp, objUserPasswordValue, owa.len);

   owa.len--;
   if(owa.len > L7_MAX_PASSWORD_SIZE)
   {
       owa.rc = XLIBRC_FAILURE;
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
   }

   if (usmDbStringPrintableCheck(objUserPasswordValue) != L7_SUCCESS)
   {
       owa.rc = XLIBRC_FAILURE;
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
   }
  
   /* set the value in application */
   memset(buf,0,L7_PASSWORD_SIZE);
   osapiStrncpySafe(buf,objUserPasswordValue,L7_PASSWORD_SIZE);
   owa.l7rc = usmDbUserMgrIASUserDBUserPasswordSet (keyUserIndexValue,
                                buf,L7_FALSE);
   if(owa.l7rc != L7_SUCCESS)
   {
     owa.rc = XLIBRC_PASSWORD_SET_FAILURE;
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
   }
   FPOBJ_TRACE_EXIT (bufp, owa);
   return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseUserMgrIntAuthServUserDBConfig_UserStatus
*
* @purpose Get 'UserStatus'
 *@description  [UserStatus] The login status per index   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUserMgrIntAuthServUserDBConfig_UserStatus (void *wap, void *bufp)
{
  fpObjWa_t kwaUserIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUserIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUserStatusValue;
   xLibU32_t  tempUserStatusValue;
  FPOBJ_TRACE_ENTER (bufp);
  
  /* retrieve key: UserIndex */
  kwaUserIndex.rc = xLibFilterGet (wap, XOBJ_baseUserMgrIntAuthServUserDBConfig_UserIndex,
                                   (xLibU8_t *) & keyUserIndexValue,
                                   &kwaUserIndex.len);
  if (kwaUserIndex.rc != XLIBRC_SUCCESS)
  {
    kwaUserIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaUserIndex);
    return kwaUserIndex.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUserIndexValue, kwaUserIndex.len);
  
  /* get the value from application */
  owa.l7rc = usmDbUserMgrIASUserDBUserStatusGet (keyUserIndexValue,
                                  &tempUserStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  
   switch (tempUserStatusValue)
    {
    case L7_ENABLE:
      objUserStatusValue = L7_ROW_STATUS_ACTIVE;
      break;
  
    case L7_DISABLE:
      objUserStatusValue = L7_ROW_STATUS_NOT_IN_SERVICE;
      break;
  
    default:
      objUserStatusValue = 0;
      owa.l7rc = L7_FAILURE;
    }
  
   if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  /* return the object value: UserStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUserStatusValue,
                           sizeof (objUserStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseUserMgrIntAuthServUserDBConfig_UserStatus
*
* @purpose Set 'UserStatus'
 *@description  [UserStatus] The login status per index   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUserMgrIntAuthServUserDBConfig_UserStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUserStatusValue;
  fpObjWa_t kwaUserIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUserIndexValue;
  fpObjWa_t kwaUserName = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUserNameValue;
  fpObjWa_t kwaPassword = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUserPasswordValue;

  xLibU32_t  indx;
  L7_uint32 status = L7_ENABLE;
  L7_RC_t rc;
  L7_char8 passwd[L7_PASSWORD_SIZE];
  L7_uint32 tempInt;
  L7_char8 temp_buf[255];
  
  FPOBJ_TRACE_ENTER (bufp);
  
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objUserStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUserStatusValue, owa.len);
  
  if ( objUserStatusValue == L7_ROW_STATUS_CREATE_AND_GO ) 
  {
     /*find the index where login status is disable */
    indx=0;
    while (indx < L7_MAX_IAS_USERS)
    {
      rc = usmDbUserMgrIASUserDBUserStatusGet(indx, &status);
      if(status == L7_DISABLE)
      {
         break;
      }
      else
      {
         indx++;
      }
    }
    if( (indx >= L7_MAX_IAS_USERS) || (status !=L7_DISABLE) )
    {
      owa.rc = XLIBRC_MAX_LOGIN_USERS_ERROR;  /* TODO: Change if required */
         FPOBJ_TRACE_EXIT (bufp, owa);
         return owa.rc;
    }
    else
    {
      /*there is a valid empty user index */
          /* retrieve key: username */
      /* Returning XLIBRC_SUCCESS for every filter get failure since error message is handled in Java Script.
         If tool issue is fixed for validation of passowrd fields, we need to add back the checks 
      */
  
      kwaUserName.rc = xLibFilterGet (wap, XOBJ_baseUserMgrIntAuthServUserDBConfig_UserName,
                                       (xLibU8_t *)objUserNameValue,
                                       &kwaUserName.len);
      if (kwaUserName.rc != XLIBRC_SUCCESS)
      {
        kwaUserName.rc = XLIBRC_SUCCESS;
        FPOBJ_TRACE_EXIT (bufp, kwaUserName);
        return kwaUserName.rc;
      }
  
          /* retrieve key: password */
      kwaPassword.rc = xLibFilterGet (wap, XOBJ_baseUserMgrIntAuthServUserDBConfig_UserPassword,
                                       (xLibU8_t *)objUserPasswordValue,
                                       &kwaPassword.len);
      if (kwaPassword.rc != XLIBRC_SUCCESS)
      {
        kwaPassword.rc = XLIBRC_SUCCESS;
        FPOBJ_TRACE_EXIT (bufp, kwaPassword);
        return kwaPassword.rc;
      }
       
      /* String lengths from XLIB are passed including null character hence reduce by 1 to make comparisions life easy */
      kwaPassword.len--;
      kwaUserName.len--;

      if(
      ((kwaUserName.len < 1) || (kwaUserName.len >= L7_LOGIN_SIZE)) ||
         (kwaPassword.len > L7_MAX_PASSWORD_SIZE)
        ) 
      {
         owa.rc = XLIBRC_FAILURE;
         return owa.rc;
      }
  
      if (usmDbStringAlphaNumericCheck(objUserNameValue) != L7_SUCCESS)
      {
        owa.rc = XLIBRC_USERNAME_ALPHA_NUMERIC_ERROR;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }

      /* check all users for an existing entry by that name */
      for (tempInt = 0; tempInt < L7_MAX_IAS_USERS; tempInt++)
      {
          memset(temp_buf, 0x00,sizeof(temp_buf));
          usmDbUserMgrIASUserDBUserNameGet(tempInt, temp_buf, sizeof(temp_buf));
          if (usmDbStringCaseInsensitiveCompare(temp_buf, objUserNameValue) == L7_SUCCESS)
          {
            owa.rc = XLIBRC_ALREADY_EXISTS;    /* TODO: Change if required */
            FPOBJ_TRACE_EXIT (bufp, owa);
            return owa.rc;
          }
      }
  
      owa.rc = XLIBRC_FAILURE;
      owa.l7rc = usmDbUserMgrIASUserDBUserNameSet(indx, objUserNameValue);
      if(owa.l7rc == L7_SUCCESS)
      {
        memset(passwd,0,L7_PASSWORD_SIZE);
        osapiStrncpySafe(passwd,objUserPasswordValue,L7_PASSWORD_SIZE);
        owa.l7rc = usmDbUserMgrIASUserDBUserPasswordSet(indx, passwd, L7_FALSE);
        if(owa.l7rc != L7_SUCCESS)
        {
           usmDbUserMgrIASUserDBUserDelete(indx );
        }
      }
      if(owa.l7rc == L7_SUCCESS)
      {
             owa.l7rc = usmDbUserMgrIASUserDBUserStatusSet(indx, L7_ENABLE);
             if(owa.l7rc == L7_SUCCESS)
             {
                /* Creation of index successful Push the index into the filter*/
                /* Pass type as 0 */
                owa.rc = xLibFilterSet(wap,XOBJ_baseUserMgrIntAuthServUserDBConfig_UserIndex,0,(xLibU8_t *) &indx,sizeof(indx)); 
                if(owa.rc !=XLIBRC_SUCCESS)
                {
                  FPOBJ_TRACE_EXIT (bufp, owa);
                  return owa.rc;
                }  
             }  
      }
    }
       return owa.rc;
  }
  else if (objUserStatusValue == L7_ROW_STATUS_DESTROY)
  {
     /* retrieve key: UserIndex */
  kwaUserIndex.rc = xLibFilterGet (wap, XOBJ_baseUserMgrIntAuthServUserDBConfig_UserIndex,
                                   (xLibU8_t *) & keyUserIndexValue,
                                   &kwaUserIndex.len);
  if (kwaUserIndex.rc != XLIBRC_SUCCESS)
  {
    kwaUserIndex.rc = XLIBRC_USER_DEL_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, kwaUserIndex);
    return kwaUserIndex.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUserIndexValue, kwaUserIndex.len);
  
    /* Delete the existing row */
    owa.l7rc = usmDbUserMgrIASUserDBUserDelete(keyUserIndexValue );
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_USER_DEL_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }
  return XLIBRC_SUCCESS;


}
/*******************************************************************************
* @function fpObjSet_baseUserMgrIntAuthServUserDBConfig_clearAll
*
* @purpose Reset the 'ClearAllUsers'
*
* @description  Clear all the IAS Users configured.
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUserMgrIntAuthServUserDBConfig_ClearAllUsers (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysClearAllUsersValue;
  L7_uint32 userCount;
  FPOBJ_TRACE_ENTER (bufp);

  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objsysClearAllUsersValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysClearAllUsersValue, owa.len);
  /* set the value in application */
  for (userCount = 0; userCount < L7_MAX_IAS_USERS; userCount++ )
  {
    if(usmDbUserMgrIASUserDBUserDelete(userCount) != L7_SUCCESS)
    {
      owa.rc = XLIBRC_USER_DEL_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  return XLIBRC_SUCCESS;
}
