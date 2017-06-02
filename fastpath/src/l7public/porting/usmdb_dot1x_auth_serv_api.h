
/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename usmdb_dot1x_auth_serv_api.h
 *
 * @purpose interface for dot1x user database
 *
 * @component Dot1x Authentication Server component
 *
 * @comments
 *
 * @create 11/17/2009
 *
 * @author pradeepk
 *
 * @end
 **********************************************************************/

#ifndef USMDB_DOT1X_AUTH_SERV_API_H
#define USMDB_DOT1X_AUTH_SERV_API_H

/*********************************************************************
 * @purpose  Returns user name per index
 *
 * @param    index    @b{(input)}  table index. 
 * @param    *name    @b{(output)}  user name. 
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDot1xAuthServUserDBUserNameGet(L7_uint32 index, L7_char8 *name);

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
L7_RC_t usmDbDot1xAuthServUserDBUserIndexGet(L7_char8 *name, L7_uint32 *index);

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
L7_RC_t usmDbDot1xAuthServUserDBAvailableIndexGet(L7_uint32 *index);

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
L7_RC_t usmDbDot1xAuthServUserDBUserNameSet(L7_uint32 index, L7_char8 *name);

/*********************************************************************
 * @purpose  Returns user encrypted password per index
 *
 * @param    index    @b{(input)}  table index. 
 * @param    *password    @b{(output)}  encrypted user password. 
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDot1xAuthServUserDBUserPasswordGet(L7_uint32 index, L7_char8 *password);

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
L7_RC_t usmDbDot1xAuthServUserDBUserPasswordEncrypt(L7_char8 *password, L7_char8 *encryptedpwd);

/*********************************************************************
 * @purpose  Returns user cleartext password per index
 *
 * @param    index    @b{(input)}  table index. 
 * @param    *password    @b{(output)}  user password in clear text format. 
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbDot1xAuthServUserDBUserPasswordClearTextGet(L7_uint32 index, L7_char8 *password);

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
L7_RC_t usmDbDot1xAuthServUserDBUserPasswordSet(L7_uint32 index, L7_char8 *password, L7_BOOL encrypted);

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
L7_RC_t usmDbDot1xAuthServUserDBUserStatusGet(L7_uint32 index, L7_uint32 *status);

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
L7_RC_t usmDbDot1xAuthServUserDBUserStatusSet(L7_uint32 index, L7_uint32 status);

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
L7_RC_t usmDbDot1xAuthServUserDBUserDelete(L7_uint32 index);

#endif /* USMDB_DOT1X_AUTH_SERV_API_H */


