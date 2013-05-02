/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename usmdb_ias_user_mgmt.c
 *
 * @purpose interface for Internal Authentication user database
 *
 * @component Internal Authentication Server component
 *
 * @comments 
 *
 * @create 01/25/2010
 *
 * @author msaleem
 *
 * @end
 **********************************************************************/
#include "user_mgr_ias_api.h"

/*********************************************************************
 * @purpose  Returns user name per index
 *
 * @param    index    @b{(input)}  table index. 
 * @param    *name    @b{(output)} user name. 
 * @param    nameLen @b{(input)}   user name length. 
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrIASUserDBUserNameGet(L7_uint32 index, L7_char8 *name, L7_uint32 nameLen)
{
  return userMgrIASUserDBUserNameGet(index, name, nameLen);
}

/*********************************************************************
 * @purpose  Returns table index of user name
 *
 * @param    *name    @b{(input)}  user name. 
 * @param    *index    @b{(output)}  table index. 
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrIASUserDBUserIndexGet(L7_char8 *name, L7_uint32 *index)
{
  return userMgrIASUserDBUserIndexGet(name, index);
}

/*********************************************************************
 * @purpose  Returns table index of next available (vacant) table index
 *
 * @param    *index    @b{(input)}  table index. 
 *
 * @returns  L7_SUCCESS or L7_FAILURE if no available index exists
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrIASUserDBAvailableIndexGet(L7_uint32 *index)
{
  return userMgrIASUserDBAvailableIndexGet(index);
}

/*********************************************************************
 * @purpose  Sets a user name per index
 *
 * @param    index    @b{(input)}  table index. 
 * @param    *name    @b{(input)}  user name. 
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE if name is too long or
 *                      if name contains an invalid character
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrIASUserDBUserNameSet(L7_uint32 index, L7_char8 *name)
{
  return userMgrIASUserDBUserNameSet(index, name);
}

/*********************************************************************
 * @purpose  Returns user encrypted password per index
 *
 * @param    index    @b{(input)}  table index. 
 * @param    *password    @b{(output)}  encrypted user password. 
 * @param    pwdLen    @b{(input)}  encrypted user password length. 
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrIASUserDBUserPasswordGet(L7_uint32 index, L7_char8 *password, L7_uint32 pwdLen)
{
  return userMgrIASUserDBUserPasswordGet(index, password, pwdLen);
}

/*********************************************************************
* @purpose  Returns encrypted user password
*
* @param    *password    @b{(input)}  clear text password. 
* @param    *encryptedpwd    @b{(output)}  encrypted password. 
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbUserMgrIASUserDBUserPasswordEncrypt(L7_char8 *password, L7_char8 *encryptedpwd)
{
  return userMgrIASUserDBUserPasswordEncrypt(password, encryptedpwd);
}

/*********************************************************************
 * @purpose  Returns user cleartext password per index
 *
 * @param    index    @b{(input)}  table index. 
 * @param    *password    @b{(output)}  user password in clear text format. 
 * @param    pwdLen    @b{(input)}  clear text user password length. 
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrIASUserDBUserPasswordClearTextGet(L7_uint32 index, L7_char8 *password, L7_uint32 pwdLen)
{
  return userMgrIASUserDBUserPasswordClearTextGet(index, password, pwdLen);
}

/*********************************************************************
 * @purpose  Sets a user password per index
 *
 * @param    index    @b{(input)}  table index. 
 * @param    *password    @b{(input)}  user password. 
 * @param    encrypted    @b{(input)}  indicates whether the password param is in
 *                                      encrypted form. 
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE if password is too long or index is too large
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrIASUserDBUserPasswordSet(L7_uint32 index, L7_char8 *password, L7_BOOL encrypted)
{
  return userMgrIASUserDBUserPasswordSet(index, password, encrypted);
}

/*********************************************************************
* @purpose  Returns user status per index
*
* @param    index    @b{(input)}  table index. 
* @param    *status    @b{(output)}  status of the user. 
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbUserMgrIASUserDBUserStatusGet(L7_uint32 index, L7_uint32 *status)
{
  return userMgrIASUserDBUserStatusGet(index, status);
}

/*********************************************************************
* @purpose  Sets the status of the user
*
* @param    index    @b{(input)}  table index. 
* @param    status    @b{(input)}  status of the user. 
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbUserMgrIASUserDBUserStatusSet(L7_uint32 index, L7_uint32 status)
{
  return userMgrIASUserDBUserStatusSet(index, status);
}

/*********************************************************************
 * @purpose  Delete a user entry from the IAS user database
 *
 * @param    index    @b{(input)}  table index. 
 *
 * @returns  L7_SUCCESS
 * @returns L7_FAILURE if attempt to remove admin
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrIASUserDBUserDelete(L7_uint32 index)
{
  return userMgrIASUserDBUserDelete(index);
}

