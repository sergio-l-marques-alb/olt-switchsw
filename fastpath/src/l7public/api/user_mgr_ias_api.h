/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 * @filename   user_mgr_ias_api.h
 *
 * @purpose    Internal Authentication Server api file
 *
 * @component  Internal Authentication Server
 *
 * @comments   none
 *
 * @create     01/28/2010
 *
 * @author     msaleem
 *
 * @end
 *
 **********************************************************************/

#ifndef USER_MGR_IAS_API_H
#define USER_MGR_IAS_API_H

#include "l7_common.h"

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
L7_RC_t userMgrIASUserDBUserNameGet(L7_uint32 index, L7_char8 *name, L7_uint32 nameLen);

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
L7_RC_t userMgrIASUserDBUserIndexGet(L7_char8 *name, L7_uint32 *index);

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
L7_RC_t userMgrIASUserDBAvailableIndexGet(L7_uint32 *index);

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
L7_RC_t userMgrIASUserDBUserNameSet(L7_uint32 index, L7_char8 *name);

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
L7_RC_t userMgrIASUserDBUserPasswordGet(L7_uint32 index, L7_char8 *password, L7_uint32 pwdLen);

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
L7_RC_t userMgrIASUserDBUserPasswordEncrypt(L7_char8 *password, L7_char8 *encryptedpwd);

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
L7_RC_t userMgrIASUserDBUserPasswordClearTextGet(L7_uint32 index, L7_char8 *password, L7_uint32 pwdLen);

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
L7_RC_t userMgrIASUserDBUserPasswordSet(L7_uint32 index, L7_char8 *password, L7_BOOL encrypted);

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
L7_RC_t userMgrIASUserDBUserStatusGet(L7_uint32 index, L7_uint32 *status);

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
L7_RC_t userMgrIASUserDBUserStatusSet(L7_uint32 index, L7_uint32 status);

/*********************************************************************
 * @purpose  Delete a user entry from the Dot1x user database
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
L7_RC_t userMgrIASUserDBUserDelete(L7_uint32 index);

#endif
