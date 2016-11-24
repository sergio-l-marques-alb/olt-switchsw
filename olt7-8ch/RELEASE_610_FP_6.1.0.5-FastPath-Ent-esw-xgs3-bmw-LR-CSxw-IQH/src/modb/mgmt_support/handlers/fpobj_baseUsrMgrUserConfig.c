/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseUsrMgrUserConfig.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to system-object.xml
*
* @create  17 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseUsrMgrUserConfig_obj.h"
#include "usmdb_user_mgmt_api.h"
#include "snmp_exports.h"
#include "user_manager_exports.h"
#include "usmdb_util_api.h"
#include "user_mgr_api.h"

L7_RC_t fpObjUtil_baseUsrMgrUserConfig_UserIndexGet ( xLibU32_t *userIndexFirstGet)
{
  xLibU32_t index ;
  xLibU32_t loginStatus;
  L7_BOOL searchFlag; 
  xLibStr256_t userNameValue;
  L7_RC_t rc;
 
  memset(userNameValue, 0x00, sizeof(userNameValue));
  searchFlag = L7_FALSE;
  index = 0;
  while (index < L7_MAX_LOGINS )
  {
		if( (usmDbLoginStatusGet(L7_UNIT_CURRENT,index, &loginStatus) == L7_SUCCESS) &&
			(loginStatus == L7_ENABLE) )
		{
		     if( (usmDbLoginsGet(L7_UNIT_CURRENT, index, userNameValue) == L7_SUCCESS )  &&
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
    rc = usmDbLoginsGet(L7_UNIT_CURRENT, index, userNameValue);
    *userIndexFirstGet = index;
  }
  return rc;
}
 
L7_RC_t 
	fpObjUtil_baseUsrMgrUserConfig_UserIndexNextGet ( xLibU32_t userIndex, xLibU32_t *userIndexNext)
{
  xLibU32_t index ;
  xLibU32_t loginStatus;
  L7_BOOL searchFlag; 
  xLibStr256_t userNameValue;
  L7_RC_t rc;
 
  searchFlag = L7_FALSE;
  index = userIndex+1;
  while (index < L7_MAX_LOGINS )
  {
		if( (usmDbLoginStatusGet(L7_UNIT_CURRENT,index, &loginStatus) == L7_SUCCESS) &&
			(loginStatus == L7_ENABLE) )
		{
		     if( (usmDbLoginsGet(L7_UNIT_CURRENT, index, userNameValue) == L7_SUCCESS )  &&
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
    rc = usmDbLoginsGet(L7_UNIT_CURRENT, index, userNameValue);
    *userIndexNext = index;
  }
	
  return rc;
}
/*******************************************************************************
* @function fpObjGet_baseUsrMgrUserConfig_UserIndex
*
* @purpose Get 'UserIndex'
*
* @description [UserIndex]: Login table index 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrUserConfig_UserIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUserIndexValue;
  xLibU32_t nextObjUserIndexValue;
  FPOBJ_TRACE_ENTER (bufp);
 
  /* retrieve key: UserIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrUserConfig_UserIndex,
                          (xLibU8_t *) & objUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objUserIndexValue = 0;
    nextObjUserIndexValue = 0;
            owa.l7rc =fpObjUtil_baseUsrMgrUserConfig_UserIndexGet(&nextObjUserIndexValue);
  }
  else
  {
  
   FPOBJ_TRACE_CURRENT_KEY (bufp, &objUserIndexValue, owa.len);
   owa.l7rc =
      fpObjUtil_baseUsrMgrUserConfig_UserIndexNextGet ( objUserIndexValue,
                                       &nextObjUserIndexValue);
  }
            
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjUserIndexValue, owa.len);
 
  /* return the object value: UserIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjUserIndexValue,
                           sizeof (objUserIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrUserConfig_UserName
*
* @purpose Get 'UserName'
*
* @description [UserName]: the login user name per index 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrUserConfig_UserName (void *wap, void *bufp)
{
  fpObjWa_t kwaUserIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUserIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUserNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UserIndex */
  kwaUserIndex.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrUserConfig_UserIndex,
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
  owa.l7rc = usmDbLoginsGet (L7_UNIT_CURRENT, keyUserIndexValue,
                             objUserNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UserName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objUserNameValue,
                           strlen (objUserNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseUsrMgrUserConfig_UserName
*
* @purpose Set 'UserName'
*
* @description [UserName]: the login user name per index 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrUserConfig_UserName (void *wap, void *bufp)
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
   kwaRowStatus.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrUserConfig_UserStatus,
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
  kwaUserIndex.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrUserConfig_UserIndex,
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

  /* do not let a user with user name "default" created*/
  if (usmDbStringCaseInsensitiveCompare(L7_USER_MGR_DEFAULT_USER_STRING, objUserNameValue)
                                                                == L7_SUCCESS)
  {
    owa.rc = XLIBRC_USERNAME_DEFAULT_ERROR;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* check all users for an existing entry by that name */
  for (tempInt = 0; tempInt < L7_MAX_LOGINS; tempInt++)
  {
    if (tempInt != keyUserIndexValue)
    {
      memset(temp_buf, 0x00,sizeof(temp_buf));
      usmDbLoginsGet(L7_UNIT_CURRENT, tempInt, temp_buf);
      if (usmDbStringCaseInsensitiveCompare(temp_buf, objUserNameValue) == L7_SUCCESS)
      {
     	owa.rc = XLIBRC_ALREADY_EXISTS;    /* TODO: Change if required */
    	FPOBJ_TRACE_EXIT (bufp, owa);
    	return owa.rc;
      }
    }
  }
  owa.l7rc = usmDbLoginsSet(L7_UNIT_CURRENT, keyUserIndexValue,
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
* @function fpObjSet_baseUsrMgrUserConfig_UserPassword
*
* @purpose Set 'UserPassword'
*
* @description [UserPassword]: the login user password in clear text per index
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrUserConfig_UserPassword (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUserPasswordValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t oldPasswordValue;
  char buf[L7_PASSWORD_SIZE];
  char oldSetPasswBuf[L7_ENCRYPTED_PASSWORD_SIZE];
  char oldEnteredPasswBuf[L7_PASSWORD_SIZE];
  fpObjWa_t kwaUserIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUserIndexValue;
  L7_ushort16 min_len;
  FPOBJ_TRACE_ENTER (bufp);

  fpObjWa_t kwaRowStatus = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;

  /* retrieve object: UserStatus */
   kwaRowStatus.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrUserConfig_UserStatus,
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
  kwaUserIndex.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrUserConfig_UserIndex,
                                   (xLibU8_t *) & keyUserIndexValue,
                                   &kwaUserIndex.len);
  if (kwaUserIndex.rc != XLIBRC_SUCCESS)
  {
      kwaUserIndex.rc = XLIBRC_USER_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, kwaUserIndex);
      return kwaUserIndex.rc;
  }

  memset(oldPasswordValue,0,sizeof(oldPasswordValue));
  kwa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrUserConfig_OldPassword,
                                   (xLibU8_t *) oldPasswordValue ,
                                   &kwa.len);
  /* If the filter exists */
  if (kwa.rc == XLIBRC_SUCCESS)
  {
    /* You have the old password in oldPasswordValue
      Check if this value matches the value in the box. If that's success proceed down.
      Else set kwa.rc = XLIBRC_USER_FAILURE; and return kwa */
      memset(oldSetPasswBuf,0,L7_ENCRYPTED_PASSWORD_SIZE);
      memset(oldEnteredPasswBuf,0,L7_PASSWORD_SIZE);
      kwa.l7rc = usmDbPasswordGet(L7_UNIT_CURRENT, keyUserIndexValue, oldSetPasswBuf);
      if ( kwa.l7rc != L7_SUCCESS )
      {
         kwa.rc = XLIBRC_USER_FAILURE;
         FPOBJ_TRACE_EXIT (bufp, kwa);
         return kwa.rc;
      }    
      osapiStrncpySafe(oldEnteredPasswBuf,oldPasswordValue,L7_PASSWORD_SIZE);
      kwa.l7rc = usmDbPasswordIsValid(L7_UNIT_CURRENT, oldSetPasswBuf, oldEnteredPasswBuf);     
      if ( kwa.l7rc != L7_SUCCESS )
      {
         kwa.rc = XLIBRC_USER_FAILURE;
         FPOBJ_TRACE_EXIT (bufp, kwa);
         return kwa.rc;
      }
  }

  /* retrieve object: UserPassword */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objUserPasswordValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objUserPasswordValue, owa.len);
  /* set the value in application */
  owa.l7rc = usmDbMinPassLengthGet(&min_len);
  if(owa.l7rc != L7_SUCCESS)
  {
      owa.rc = XLIBRC_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }
  owa.len--;
  if( ((owa.len > 0) && ((owa.len < min_len) || (owa.len > L7_MAX_PASSWORD_SIZE))) )
  {
      owa.rc = XLIBRC_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }

  /* set block status */
 if (userMgrLoginUserBlockStatusSet (keyUserIndexValue, L7_FALSE) == L7_FAILURE)
 {
      owa.rc = XLIBRC_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
 }

  /* set the value in application */
  memset(buf,0,L7_PASSWORD_SIZE);
  osapiStrncpySafe(buf,objUserPasswordValue,L7_PASSWORD_SIZE);
  owa.l7rc = usmDbPasswordSet (L7_UNIT_CURRENT, keyUserIndexValue,
                               buf,L7_FALSE);
  if((owa.l7rc != L7_ERROR)  && (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_PASSWORD_SET_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  owa.l7rc = L7_SUCCESS;    
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseUsrMgrUserConfig_OldPassword
*
* @purpose Set 'OldPassword'
*
* @description [OldPassword]: This is a dummy function to support OldPassword
*                             object.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrUserConfig_OldPassword (void *wap, void *bufp)
{
  /* Dummy */
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrUserConfig_UserStatus
*
* @purpose Get 'UserStatus'
*
* @description [UserStatus]: the login status per index 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrUserConfig_UserStatus (void *wap, void *bufp)
{
  fpObjWa_t kwaUserIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUserIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUserStatusValue;
   xLibU32_t  tempUserStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UserIndex */
  kwaUserIndex.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrUserConfig_UserIndex,
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
  owa.l7rc = usmDbLoginStatusGet (L7_UNIT_CURRENT, keyUserIndexValue,
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
* @function fpObjSet_baseUsrMgrUserConfig_UserStatus
*
* @purpose Set 'UserStatus'
*
* @description [UserStatus]: the login status per index 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrUserConfig_UserStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objUserStatusValue;

  fpObjWa_t kwaUserIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUserIndexValue;

  fpObjWa_t kwaUserName = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUserNameValue;

  fpObjWa_t kwaPassword = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUserPasswordValue;

  fpObjWa_t kwaUserAccessLevel = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUserAccessLevelValue;	


  xLibU32_t  indx;
  L7_uint32 status = L7_ENABLE;
  L7_RC_t rc;
  L7_char8 passwd[L7_PASSWORD_SIZE];
  L7_uint32 tempInt;
  L7_char8 temp_buf[255];
  L7_ushort16 min_len;

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
		while (indx < L7_MAX_LOGINS)
		{
			rc = usmDbLoginStatusGet(L7_UNIT_CURRENT, indx, &status);
			if(status == L7_DISABLE)
			{
			   break;
			}
			else
			{
			   indx++;
			}
		}
		if( (indx >= L7_MAX_LOGINS) || (status !=L7_DISABLE) )
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

			kwaUserName.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrUserConfig_UserName,
			                                 (xLibU8_t *)objUserNameValue,
			                                 &kwaUserName.len);
			if (kwaUserName.rc != XLIBRC_SUCCESS)
			{
			  kwaUserName.rc = XLIBRC_SUCCESS;
			  FPOBJ_TRACE_EXIT (bufp, kwaUserName);
			  return kwaUserName.rc;
			}

			    /* retrieve key: password */
			kwaPassword.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrUserConfig_UserPassword,
			                                 (xLibU8_t *)objUserPasswordValue,
			                                 &kwaPassword.len);
			if (kwaPassword.rc != XLIBRC_SUCCESS)
			{
			  kwaPassword.rc = XLIBRC_SUCCESS;
			  FPOBJ_TRACE_EXIT (bufp, kwaPassword);
			  return kwaPassword.rc;
			}

			 /* retrieve object: UserAuthType */
			 kwaUserAccessLevel.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrUserConfig_UserAccessLevel,
			                                  (xLibU8_t *) & objUserAccessLevelValue, 
			                                  &kwaUserAccessLevel.len);
			 if (kwaUserAccessLevel.rc != XLIBRC_SUCCESS)
			 {
			   kwaUserAccessLevel.rc = XLIBRC_SUCCESS;
			   FPOBJ_TRACE_EXIT (bufp, kwaUserAccessLevel);
			   return kwaUserAccessLevel.rc;
			 }
			 
			/* String lengths from XLIB are passed including null character hence reduce by 1 to make comparisions life easy */
			kwaPassword.len--;
			kwaUserName.len--;
			owa.l7rc = usmDbMinPassLengthGet(&min_len);
			if(owa.l7rc != L7_SUCCESS)
			{
				owa.rc = XLIBRC_FAILURE;
				FPOBJ_TRACE_EXIT (bufp, owa);
				return owa.rc;
			}



			if(
			((kwaUserName.len < 1) || (kwaUserName.len >= L7_LOGIN_SIZE)) ||
			   ((kwaPassword.len < min_len) || (kwaPassword.len > L7_MAX_PASSWORD_SIZE))
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

			/* do not let a user with user name "default" created*/
			if (usmDbStringCaseInsensitiveCompare(L7_USER_MGR_DEFAULT_USER_STRING, objUserNameValue)
			                                                              == L7_SUCCESS)
			{
			  owa.rc = XLIBRC_USERNAME_DEFAULT_ERROR;    /* TODO: Change if required */
			  FPOBJ_TRACE_EXIT (bufp, owa);
			  return owa.rc;
			}

			/* check all users for an existing entry by that name */
			for (tempInt = 0; tempInt < L7_MAX_LOGINS; tempInt++)
			{
			    memset(temp_buf, 0x00,sizeof(temp_buf));
			    usmDbLoginsGet(L7_UNIT_CURRENT, tempInt, temp_buf);
			    if (usmDbStringCaseInsensitiveCompare(temp_buf, objUserNameValue) == L7_SUCCESS)
			    {
				    owa.rc = XLIBRC_ALREADY_EXISTS;    /* TODO: Change if required */
				    FPOBJ_TRACE_EXIT (bufp, owa);
				    return owa.rc;
			    }
			}

			owa.rc = XLIBRC_FAILURE;
			owa.l7rc = usmDbLoginsSet(L7_UNIT_CURRENT, indx, objUserNameValue);
			if(owa.l7rc == L7_SUCCESS)
			{
			  memset(passwd,0,L7_PASSWORD_SIZE);
			  osapiStrncpySafe(passwd,objUserPasswordValue,L7_PASSWORD_SIZE);
			  owa.l7rc = usmDbPasswordSet(L7_UNIT_CURRENT,indx, passwd, L7_FALSE);
			   if(owa.l7rc == L7_SUCCESS)
			   {
			     owa.l7rc = usmDbUserAccessLevelSet (L7_UNIT_CURRENT, indx,
			                                objUserAccessLevelValue);
			   	}
				if(owa.l7rc != L7_SUCCESS)
				{
				   usmDbLoginsDelete( L7_UNIT_CURRENT, indx );
				}
			}
			if(owa.l7rc == L7_SUCCESS)
			{
	           owa.l7rc = usmDbLoginStatusSet(L7_UNIT_CURRENT,indx, L7_ENABLE);
	           if(owa.l7rc == L7_SUCCESS)
	           {
	              /* Creation of index successful Push the index into the filter*/
	              /* Pass type as 0 */
	              owa.rc = xLibFilterSet(wap,XOBJ_baseUsrMgrUserConfig_UserIndex,0,(xLibU8_t *) &indx,sizeof(indx)); 
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
	kwaUserIndex.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrUserConfig_UserIndex,
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
    owa.l7rc = usmDbLoginsDelete( L7_UNIT_CURRENT, keyUserIndexValue );
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
* @function fpObjGet_baseUsrMgrUserConfig_UserAuthType
*
* @purpose Get 'UserAuthType'
*
* @description [UserAuthType]: the authentication parameters for a user 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrUserConfig_UserAuthType (void *wap, void *bufp)
{
  fpObjWa_t kwaUserIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUserIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUserAuthTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UserIndex */
  kwaUserIndex.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrUserConfig_UserIndex,
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
  owa.l7rc =
    userMgrLoginUserAuthenticationGet ( keyUserIndexValue,
                                       &objUserAuthTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UserAuthType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUserAuthTypeValue,
                           sizeof (objUserAuthTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseUsrMgrUserConfig_UserAuthType
*
* @purpose Set 'UserAuthType'
*
* @description [UserAuthType]: the authentication parameters for a user 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrUserConfig_UserAuthType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUserAuthTypeValue;
  xLibU32_t tempValue;
  fpObjWa_t kwaUserIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUserIndexValue; 
  xLibU32_t objUserEncryptTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  fpObjWa_t kwaRowStatus = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUserIndexValue, kwaUserIndex.len);

  kwaRowStatus.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrUserConfig_UserStatus,
                                   (xLibU8_t *) & objRowStatusValue,
                                   &kwaRowStatus.len);
  if (kwaRowStatus.rc == XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objRowStatusValue, kwaRowStatus.len);

    /* if( (objRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
         ||(objRowStatusValue == L7_ROW_STATUS_CREATE_AND_WAIT)
       )
    {
        return XLIBRC_SUCCESS;
    }*/
  }
        /* retrieve key: UserIndex */
  kwaUserIndex.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrUserConfig_UserIndex,
                                   (xLibU8_t *) & keyUserIndexValue,
                                   &kwaUserIndex.len);
  if (kwaUserIndex.rc != XLIBRC_SUCCESS)
  {
      kwaUserIndex.rc = XLIBRC_USER_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, kwaUserIndex);
      return kwaUserIndex.rc;
  }


  /* retrieve object: UserAuthType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objUserAuthTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUserAuthTypeValue, owa.len);

   owa.l7rc =
    userMgrLoginUserAuthenticationGet ( keyUserIndexValue, &tempValue);
   if( tempValue == objUserAuthTypeValue)
   {
     owa.rc = XLIBRC_SUCCESS;
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
   }

   if ((objUserAuthTypeValue != L7_SNMP_USER_AUTH_PROTO_NONE) &&
           (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNMP_COMPONENT_ID, L7_SNMP_USER_AUTHENTICATION_FEATURE_ID) == L7_FALSE))
   {
    owa.rc = XLIBRC_SNMP_AUTH_NOT_SUPPORTED;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
   } 
  /* get the value from application */
  owa.l7rc = usmDbUserEncryptGet (L7_UNIT_CURRENT, keyUserIndexValue,
                                  &objUserEncryptTypeValue);
  
   if((owa.l7rc != L7_SUCCESS) || 
      ((objUserAuthTypeValue == L7_SNMP_USER_AUTH_PROTO_NONE) && 
       (objUserEncryptTypeValue != L7_SNMP_USER_PRIV_PROTO_NONE)))
   {
      owa.rc = XLIBRC_USER_ENCRYPT_ISSET_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
   }
 
  /* set the value in application */
  owa.l7rc =
    userMgrLoginUserAuthenticationSet ( keyUserIndexValue,
                                       objUserAuthTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_AUTH_PROTOCOL_SET_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrUserConfig_UserEncryptType
*
* @purpose Get 'UserEncryptType'
*
* @description [UserEncryptType]: encryption type used to store the userr
*              key 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrUserConfig_UserEncryptType (void *wap, void *bufp)
{
  fpObjWa_t kwaUserIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUserIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUserEncryptTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UserIndex */
  kwaUserIndex.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrUserConfig_UserIndex,
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
  owa.l7rc = usmDbUserEncryptGet (L7_UNIT_CURRENT, keyUserIndexValue,
                                  &objUserEncryptTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  /* return the object value: UserEncryptType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUserEncryptTypeValue,
                           sizeof (objUserEncryptTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseUsrMgrUserConfig_UserEncryptType
*
* @purpose Set 'UserEncryptType'
*
* @description [UserEncryptType]: encryption type used to store the userr
*              key 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrUserConfig_UserEncryptType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUserEncryptTypeValue;
  fpObjWa_t kwaUserIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUserIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  fpObjWa_t kwaRowStatus = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;

  /* retrieve object: UserStatus */
   kwaRowStatus.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrUserConfig_UserStatus,
                                   (xLibU8_t *) & objRowStatusValue,
                                   &kwaRowStatus.len);
  if (kwaRowStatus.rc == XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objRowStatusValue, kwaUserIndex.len);

    /*if( (objRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
      ||(objRowStatusValue == L7_ROW_STATUS_CREATE_AND_WAIT)
      )
    {
        return XLIBRC_SUCCESS;
    }*/
  }

          /* retrieve key: UserIndex */
  kwaUserIndex.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrUserConfig_UserIndex,
                                   (xLibU8_t *) & keyUserIndexValue,
                                   &kwaUserIndex.len);
  if (kwaUserIndex.rc != XLIBRC_SUCCESS)
  {
      kwaUserIndex.rc = XLIBRC_USER_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, kwaUserIndex);
      return kwaUserIndex.rc;
  }


  /* retrieve object: UserEncryptType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objUserEncryptTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUserEncryptTypeValue, owa.len);

  owa.l7rc = L7_SUCCESS;

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
* @function fpObjSet_baseUsrMgrUserConfig_UserEncryptPassword
*
* @purpose Set 'UserEncryptPassword'
*
* @description [UserEncryptPassword]: the encryption parameters for a user
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrUserConfig_UserEncryptPassword (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUserEncryptPasswordValue;
  fpObjWa_t kwaUserIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUserIndexValue;
  xLibU32_t tempUserEncryptType;
  FPOBJ_TRACE_ENTER (bufp);

  fpObjWa_t kwaRowStatus = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;
  L7_char8 encrypt_key[L7_ENCRYPTION_KEY_SIZE];

  /* retrieve object: UserStatus */
   kwaRowStatus.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrUserConfig_UserStatus,
                                   (xLibU8_t *) & objRowStatusValue,
                                   &kwaRowStatus.len);
  if (kwaRowStatus.rc == XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objRowStatusValue, kwaUserIndex.len);

     /*if( (objRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
       ||(objRowStatusValue == L7_ROW_STATUS_CREATE_AND_WAIT)
       )
    {
        return XLIBRC_SUCCESS;
    }*/
  }
        /* retrieve key: UserIndex */
  kwaUserIndex.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrUserConfig_UserIndex,
                                   (xLibU8_t *) & keyUserIndexValue,
                                   &kwaUserIndex.len);
  if (kwaUserIndex.rc != XLIBRC_SUCCESS)
  {
      kwaUserIndex.rc = XLIBRC_USER_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, kwaUserIndex);
      return kwaUserIndex.rc;
  }


  /* retrieve object: UserEncryptPassword */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objUserEncryptPasswordValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objUserEncryptPasswordValue, owa.len);

   /* retrieve key: UserIndex */
  kwaUserIndex.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrUserConfig_UserEncryptType,
                                   (xLibU8_t *) & tempUserEncryptType,
                                   &kwaUserIndex.len);
  if (kwaUserIndex.rc != XLIBRC_SUCCESS)
  {
    kwaUserIndex.rc = XLIBRC_USER_ENCRYPT_TYPE_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, kwaUserIndex);
    return kwaUserIndex.rc;
  }

  if( (tempUserEncryptType  !=  L7_SNMP_USER_PRIV_PROTO_NONE)  && 
  	   (tempUserEncryptType  !=  L7_SNMP_USER_PRIV_PROTO_DES) )
  {
    owa.rc = XLIBRC_USER_ENCRYPT_TYPE_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  memset(encrypt_key,0,L7_ENCRYPTION_KEY_SIZE);
  osapiStrncpySafe(encrypt_key,objUserEncryptPasswordValue,L7_ENCRYPTION_KEY_SIZE);
  /* set the value in application */
  owa.l7rc = userMgrLoginUserEncryptionSet ( keyUserIndexValue,
                         tempUserEncryptType, encrypt_key);
  if (owa.l7rc != L7_SUCCESS)
  {
    /*owa.rc = XLIBRC_FAILURE;*/    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseUsrMgrUserConfig_UserAuthenticationList
*
* @purpose Get 'UserAuthenticationList'
*
* @description [UserAuthenticationList]: an APL assigned to a user for a specific
*              component 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrUserConfig_UserAuthenticationList (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwaUserIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUserIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUserAuthenticationListValue;
  L7_uchar8 userName[255];
  memset(userName,0x00,sizeof(userName));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UserIndex */
  kwaUserIndex.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrUserConfig_UserIndex,
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

  owa.l7rc = usmDbLoginsGet(L7_UNIT_CURRENT, keyUserIndexValue, userName);

  if (owa.l7rc == L7_SUCCESS)
  {
   owa.l7rc = usmDbAPLUserGet(L7_UNIT_CURRENT, userName, 
   	               L7_USER_MGR_COMPONENT_ID, objUserAuthenticationListValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UserAuthenticationList */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objUserAuthenticationListValue,
                           strlen (objUserAuthenticationListValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseUsrMgrUserConfig_UserAuthenticationList
*
* @purpose Set 'UserAuthenticationList'
*
* @description [UserAuthenticationList]: an APL assigned to a user for a specific
*              component 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrUserConfig_UserAuthenticationList (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUserAuthenticationListValue;
  fpObjWa_t kwaUserIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUserIndexValue;
  L7_uchar8 userName[255];
  memset(userName,0x00,sizeof(userName));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UserAuthenticationList */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objUserAuthenticationListValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objUserAuthenticationListValue, owa.len);

  /* retrieve key: UserIndex */
  kwaUserIndex.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrUserConfig_UserIndex,
                                   (xLibU8_t *) & keyUserIndexValue,
                                   &kwaUserIndex.len);
  if (kwaUserIndex.rc != XLIBRC_SUCCESS)
  {
    kwaUserIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaUserIndex);
    return kwaUserIndex.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUserIndexValue, kwaUserIndex.len);

  /* set the value in application */

  owa.l7rc = usmDbLoginsGet(L7_UNIT_CURRENT, keyUserIndexValue, userName);

  if (owa.l7rc == L7_SUCCESS)
  {
    owa.l7rc = usmDbAPLUserSet(L7_UNIT_CURRENT, userName,
		                   L7_USER_MGR_COMPONENT_ID, objUserAuthenticationListValue);
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
* @function fpObjGet_baseUsrMgrUserConfig_UserLockoutStatus
*
* @purpose Get 'UserLockoutStatus'
*
* @description [UserLockoutStatus]: To know whether the user is locked or
*              not 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrUserConfig_UserLockoutStatus (void *wap, void *bufp)
{
  fpObjWa_t kwaUserIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUserIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUserLockoutStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UserIndex */
  kwaUserIndex.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrUserConfig_UserIndex,
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
  owa.l7rc = usmDbUserLockStatusGet (L7_UNIT_CURRENT, keyUserIndexValue,
                                     &objUserLockoutStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UserLockoutStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUserLockoutStatusValue,
                           sizeof (objUserLockoutStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseUsrMgrUserConfig_UserPasswordExpireTime
*
* @purpose Get 'UserPasswordExpireTime'
*
* @description [UserPasswordExpireTime]: the time the user's password expired
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrUserConfig_UserPasswordExpireTime (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwaUserIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUserIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objUserPasswordExpireTimeValue;

  xLibU32_t val;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UserIndex */
  kwaUserIndex.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrUserConfig_UserIndex,
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
  owa.l7rc = usmDbPasswordExpireTimeGet (L7_UNIT_CURRENT, keyUserIndexValue, &val);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  osapiStrncpySafe(objUserPasswordExpireTimeValue, (const char *)usmDbConvertTimeToDateString(val), 21);
  /* return the object value: UserPasswordExpireTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUserPasswordExpireTimeValue,
                           sizeof (objUserPasswordExpireTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseUsrMgrUserConfig_UserPasswordChangeTime
*
* @purpose Get 'UserPasswordChangeTime'
*
* @description [UserPasswordChangeTime]: the time when the password got set
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrUserConfig_UserPasswordChangeTime (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwaUserIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUserIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUserPasswordChangeTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UserIndex */
  kwaUserIndex.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrUserConfig_UserIndex,
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
  owa.l7rc = usmDbPasswordChangeTimeGet (L7_UNIT_CURRENT, keyUserIndexValue,
                                         &objUserPasswordChangeTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UserPasswordChangeTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUserPasswordChangeTimeValue,
                           sizeof (objUserPasswordChangeTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseUsrMgrUserConfig_UserUnLock
*
* @purpose Set 'UserUnLock'
*
* @description [UserUnLock]: unlock the locked user 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrUserConfig_UserUnLock (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUserUnLockValue;
  fpObjWa_t kwaUserIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUserIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UserUnLock */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objUserUnLockValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUserUnLockValue, owa.len);  

  /* retrieve key: UserIndex */
  kwaUserIndex.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrUserConfig_UserIndex,
                                   (xLibU8_t *) & keyUserIndexValue,
                                   &kwaUserIndex.len);
  if (kwaUserIndex.rc != XLIBRC_SUCCESS)
  {
    kwaUserIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaUserIndex);
    return kwaUserIndex.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUserIndexValue, kwaUserIndex.len);
  if(objUserUnLockValue == L7_DISABLE)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbUserUnLock (L7_UNIT_CURRENT, keyUserIndexValue);
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
* @function fpObjGet_baseUsrMgrUserConfig_UserTotalLockoutCount
*
* @purpose Get 'UserTotalLockoutCount'
*
* @description [UserTotalLockoutCount]: Gets the total number of times the
*              user account has been locked 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrUserConfig_UserTotalLockoutCount (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwaUserIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUserIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUserTotalLockoutCountValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UserIndex */
  kwaUserIndex.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrUserConfig_UserIndex,
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
  owa.l7rc = usmDbUserTotalLockoutCountGet ( keyUserIndexValue,
                                            &objUserTotalLockoutCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UserTotalLockoutCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUserTotalLockoutCountValue,
                           sizeof (objUserTotalLockoutCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseUsrMgrUserConfig_UserTotalLoginFailures
*
* @purpose Get 'UserTotalLoginFailures'
*
* @description [UserTotalLoginFailures]: the total number of password failures
*              for the user account 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrUserConfig_UserTotalLoginFailures (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwaUserIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUserIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUserTotalLoginFailuresValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UserIndex */
  kwaUserIndex.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrUserConfig_UserIndex,
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
  owa.l7rc = usmDbUserTotalLoginFailuresGet ( keyUserIndexValue,
                                             &objUserTotalLoginFailuresValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UserTotalLoginFailures */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUserTotalLoginFailuresValue,
                           sizeof (objUserTotalLoginFailuresValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseUsrMgrUserConfig_UserCurrentLockoutCount
*
* @purpose Get 'UserCurrentLockoutCount'
*
* @description [UserCurrentLockoutCount]: the current password failure count
*              used for determining when the account should be locked.The
*              value is reset on a successful login 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrUserConfig_UserCurrentLockoutCount (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwaUserIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUserIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUserCurrentLockoutCountValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UserIndex */
  kwaUserIndex.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrUserConfig_UserIndex,
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
  owa.l7rc =
    usmDbUserCurrentLockoutCountGet ( keyUserIndexValue,
                                     &objUserCurrentLockoutCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UserCurrentLockoutCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUserCurrentLockoutCountValue,
                           sizeof (objUserCurrentLockoutCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseUsrMgrUserConfig_UserAccessMode
*
* @purpose Get 'UserAccessMode'
*
* @description [UserAccessMode]: the login user password after encryption
*              per index 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrUserConfig_UserAccessMode (void *wap, void *bufp)
{
  fpObjWa_t kwaUserIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUserIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUserAccessModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UserIndex */
  kwaUserIndex.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrUserConfig_UserIndex,
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
  owa.l7rc = usmDbAccessModeGet (L7_UNIT_CURRENT, keyUserIndexValue,
                                 &objUserAccessModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  /* return the object value: UserAccessMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUserAccessModeValue,
                           sizeof (objUserAccessModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseUsrMgrUserConfig_UserAccessMode
*
* @purpose Set 'UserAccessMode'
*
* @description [UserAccessMode]: the login user password after encryption
*              per index 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrUserConfig_UserAccessMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUserAccessModeValue;
  fpObjWa_t kwaUserIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUserIndexValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UserAccessMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objUserAccessModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUserAccessModeValue, owa.len);


  /* retrieve key: UserIndex */
  kwaUserIndex.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrUserConfig_UserIndex,
                                   (xLibU8_t *) & keyUserIndexValue,
                                   &kwaUserIndex.len);
  if (kwaUserIndex.rc != XLIBRC_SUCCESS)
  {
    kwaUserIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaUserIndex);
    return kwaUserIndex.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUserIndexValue, kwaUserIndex.len);

  /* set the value in application */
  owa.l7rc = usmDbAccessModeGet (L7_UNIT_CURRENT, keyUserIndexValue,
                                 &objUserAccessModeValue);
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
* @function fpObjGet_baseUsrMgrUserConfig_UserAccessLevel
*
* @purpose Get 'UserAccessLevel'
*
* @description [UserAccessLevel]: login user access level 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrUserConfig_UserAccessLevel (void *wap, void *bufp)
{
  fpObjWa_t kwaUserIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUserIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUserAccessLevelValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: UserIndex */
  kwaUserIndex.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrUserConfig_UserIndex,
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
  owa.l7rc = usmDbUserAccessLevelGet (L7_UNIT_CURRENT, keyUserIndexValue,
                                      &objUserAccessLevelValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UserAccessLevel */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUserAccessLevelValue,
                           sizeof (objUserAccessLevelValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseUsrMgrUserConfig_UserAccessLevel
*
* @purpose Set 'UserAccessLevel'
*
* @description [UserAccessLevel]: login user access level 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrUserConfig_UserAccessLevel (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUserAccessLevelValue;
  fpObjWa_t kwaUserIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyUserIndexValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UserAccessLevel */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objUserAccessLevelValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUserAccessLevelValue, owa.len);

  
  /* retrieve key: UserIndex */
  kwaUserIndex.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrUserConfig_UserIndex,
                                   (xLibU8_t *) & keyUserIndexValue,
                                   &kwaUserIndex.len);
  if (kwaUserIndex.rc != XLIBRC_SUCCESS)
  {
    kwaUserIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaUserIndex);
    return kwaUserIndex.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyUserIndexValue, kwaUserIndex.len);

  /* set the value in application */
  owa.l7rc = usmDbUserAccessLevelSet (L7_UNIT_CURRENT, keyUserIndexValue,
                                      objUserAccessLevelValue);
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
* @function fpObjSet_baseUsrMgrUserConfig_ResetPassword
*
* @purpose Set 'ResetPassword'
*
* @description [ResetPassword]: the login user password in clear text per index
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrUserConfig_ResetPassword (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  char buf[L7_PASSWORD_SIZE];
  xLibU32_t index = 0;
xLibStr256_t userNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  while(index < L7_MAX_LOGINS )
  {
      /* set the value in application */
      memset(buf,0,L7_PASSWORD_SIZE);
if( (usmDbLoginsGet(L7_UNIT_CURRENT, index, userNameValue) == L7_SUCCESS ) &&
           (userNameValue[0] != '\0'))
{
      owa.l7rc = usmDbPasswordSet (L7_UNIT_CURRENT, index,
                                 buf,L7_FALSE);
      if((owa.l7rc != L7_ERROR)  && (owa.l7rc != L7_SUCCESS))
      {
         owa.rc = XLIBRC_PASSWORD_SET_FAILURE;
         FPOBJ_TRACE_EXIT (bufp, owa);
         return owa.rc;
      }
}
	  index = index + 1;      
  }
  owa.l7rc = L7_SUCCESS;    
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

