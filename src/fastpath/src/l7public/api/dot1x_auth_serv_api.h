/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 * @filename   dot1x_auth_serv_api.h
 *
 * @purpose    Dot1x Authentication Server api file
 *
 * @component  Dot1x Authentication Server
 *
 * @comments   none
 *
 * @create     11/17/2009
 *
 * @author     pradeepk
 *
 * @end
 *
 **********************************************************************/

#ifndef DOT1X_AUTH_SERV_API_H
#define DOT1X_AUTH_SERV_API_H

#include "l7_common.h"

/*********************************************************************
 * @purpose  Returns user name per index
 *
 * @param    index    @b{(input)}  table index. 
 * @param    *name    @b{(output)}  user name. 
 * @param    nameLen  @b{(input)}  username length. 
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dot1xAuthServUserDBUserNameGet(L7_uint32 index, L7_char8 *name, L7_uint32 nameLen);

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
L7_RC_t dot1xAuthServUserDBUserIndexGet(L7_char8 *name, L7_uint32 *index);

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
L7_RC_t dot1xAuthServUserDBAvailableIndexGet(L7_uint32 *index);

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
L7_RC_t dot1xAuthServUserDBUserNameSet(L7_uint32 index, L7_char8 *name);

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
L7_RC_t dot1xAuthServUserDBUserPasswordGet(L7_uint32 index, L7_char8 *password, L7_uint32 pwdLen);

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
L7_RC_t dot1xAuthServUserDBUserPasswordEncrypt(L7_char8 *password, L7_char8 *encryptedpwd);

/*********************************************************************
 * @purpose  Returns user cleartext password per index
 *
 * @param    index    @b{(input)}  table index. 
 * @param    *password    @b{(output)}  user password in clear text format. 
 * @param    pwdLen    @b{(input)}  clear text user password length. 
 *
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dot1xAuthServUserDBUserPasswordClearTextGet(L7_uint32 index, L7_char8 *password, L7_uint32 pwdLen);

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
L7_RC_t dot1xAuthServUserDBUserPasswordSet(L7_uint32 index, L7_char8 *password, L7_BOOL encrypted);

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
L7_RC_t dot1xAuthServUserDBUserStatusGet(L7_uint32 index, L7_uint32 *status);

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
L7_RC_t dot1xAuthServUserDBUserStatusSet(L7_uint32 index, L7_uint32 status);

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
L7_RC_t dot1xAuthServUserDBUserDelete(L7_uint32 index);

#endif
