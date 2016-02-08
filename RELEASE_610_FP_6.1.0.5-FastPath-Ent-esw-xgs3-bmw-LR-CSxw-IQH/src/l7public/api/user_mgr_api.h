
/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 * @filename   user_mgr.h
 *
 * @purpose    User Manager file
 *
 * @component  user_mgr component
 *
 * @comments   none
 *
 * @create     09/26/2002
 *
 * @author     Jill Flanagan
 *
 * @end
 *
 **********************************************************************/

#ifndef USER_MGR_API_H
#define USER_MGR_API_H

#include "l7_common.h"
#include "user_manager_exports.h"
#include "defaultconfig.h"
#include "user_manager_exports.h"

#define USER_MGR_ACCESS_LESS 0
#define USER_MGR_ACCESS_EQUAL 1
#define USER_MGR_ACCESS_MORE 2

/* User Manager Listener Types */
typedef enum
{
  L7_USER_MGR_USER_LISTENER = 0,
  L7_USER_MGR_AUTH_LISTENER = 1,
  L7_USER_MGR_ENCRYPT_LISTENER,
  L7_USER_MGR_USER_LISTENER_LAST,                  /* total number of enum values */
} L7_USER_MGR_LISTENER_TYPES_t;


/* User Manager User Change Types */
typedef enum
{
  L7_USER_MGR_CHANGE_USER_ADD = 0,
  L7_USER_MGR_CHANGE_USER_MODIFY = 1,
  L7_USER_MGR_CHANGE_USER_DELETE,
  L7_USER_MGR_CHANGE_USER_DISABLE,
  L7_USER_MGR_CHANGE_USER_CHANGE_LAST,                  /* total number of enum values */
} L7_USER_MGR_CHANGE_USER_TYPES_t;

/*********************************************************************
 * @purpose  Returns login user name per index
 *
 * @param    index    table index
 * @param    name     pointer to login user name
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 userMgrLoginUserNameGet(L7_uint32 index, L7_char8 *name);

/*********************************************************************
 * @purpose  Returns table index of Login user name
 *
 * @param    name     pointer to loing user name
 * @param    index    pointer to table index
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 userMgrLoginIndexGet(L7_char8 *name, L7_uint32 *index);

/*********************************************************************
 * @purpose  Returns table index of next available (vacant) table index
 *
 * @param    index    pointer to table index
 *
 * @returns  L7_SUCCESS or L7_FAILURE if no available index exists
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrLoginAvailableIndexGet(L7_uint32 *index);

/*********************************************************************
 * @purpose  Sets a login user name per index
 *
 * @param    index       table index
 * @param    commName    pointer to Community name
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE if name is too long or
 *                      if name contains an invalid character
 *
 * @notes    also sets login status to L7_enable
 *
 * @end
 *********************************************************************/
L7_uint32 userMgrLoginUserNameSet(L7_uint32 index, L7_char8 *name);

/*********************************************************************
 * @purpose  Returns login user password per index
 *
 * @param    index       table index
 * @param    password    pointer to login user password
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 userMgrLoginUserPasswordGet(L7_uint32 index, L7_char8 *password);

/*********************************************************************
* @purpose  Returns encrypted user password
*
* @param    password    input - pointer to clear text password
* @param    encryptedpwd    output = pointer to md5 encrypted password
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t userMgrLoginUserPasswordEncrypt(L7_char8 *password, L7_char8 *encryptedpwd);

/*********************************************************************
 * @purpose  Returns login user password per index
 *
 * @param    index       table index
 * @param    password    pointer to login user password
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 userMgrLoginUserPasswordClearTextGet(L7_uint32 index, L7_char8 *password);

/*********************************************************************
 * @purpose  Sets a login user password per index
 *
 * @param    index       table index
 * @param    password    pointer to login user password
 * @param    encrypted   indicates whether the password param is in encrypted form
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE if password is too long or index is too large
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 userMgrLoginUserPasswordSet(L7_uint32 index, L7_char8 *password, L7_BOOL encrypted);

/*********************************************************************
 * @purpose  Returns login user status per index
 *
 * @param    index    table index
 * @param    status   pointer to login user status (L7_ENABLE of L7_DISABLE)
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 userMgrLoginUserStatusGet(L7_uint32 index, L7_uint32 *status);

/*********************************************************************
 * @purpose  Sets the status of the login user
 *
 * @param    index       table index
 * @param    password    pointer to login user password
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_uint32 userMgrLoginUserStatusSet(L7_uint32 index, L7_uint32 status);

/*********************************************************************
 * @purpose  Resets user logins back to Factory Defaults
 *
 * @param    void
 *
 * @returns  void
 *
 * @notes
 *
 * @end
 *********************************************************************/
void userMgrUserLoginsFactoryDefaultSet(void);


/*********************************************************************
 * @purpose  Sets a login user authentication protocol
 *
 * @param    index       table index
 * @param    authProt    authentication protocol (none, md5, or sha)
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 userMgrLoginUserAuthenticationSet(L7_uint32 index, L7_uint32 authProt);


/*********************************************************************
 * @purpose  Sets a login user encryption protocol and key
 *
 * @param    index       table index
 * @param    encryptProt encryption protocol (none, md5, or sha)
 * @param    encryptKey  encryption key (ignored if prot = none)
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 userMgrLoginUserEncryptionSet(L7_uint32 index, L7_uint32 encryptProt,
                                        L7_char8 *encryptKey);


/*********************************************************************
 * @purpose  Sets a login user access level
 *
 * @param    index       table index
 * @param    accessLevel readonly or readwrite
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 userMgrLoginUserAccessModeSet(L7_uint32 index, L7_uint32 accessLevel );


/*********************************************************************
 * @purpose  Sets the snmpv3 accessmode for a login user
 *
 * @param    index       table index
 * @param    accessLevel readonly or readwrite
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 userMgrLoginUserSnmpv3AccessModeSet(L7_uint32 index, L7_uint32 accessLevel );

/*********************************************************************
 * @purpose  Return a login user authentication protocol and key
 *
 * @param    index       (input) table index
 * @param    authProt    (outupt) authentication protocol (none, md5, or sha)
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 userMgrLoginUserAuthenticationGet(L7_uint32 index, L7_uint32 *authProt);


/*********************************************************************
 * @purpose  Returns a login user encryption protocol and key
 *
 * @param    index       table index
 * @param    encryptProt (output) encryption protocol (none, md5, or sha)
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 userMgrLoginUserEncryptionGet(L7_uint32 index, L7_uint32 *encryptProt);

/*********************************************************************
* @purpose  Returns a login user encryption key
*
* @param    index       table index
* @param    encryptKey (output) encryption Key
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 userMgrLoginUserEncryptKeyGet(L7_uint32 index,  L7_char8 *encryptKey);

/*********************************************************************
 * @purpose  Return a login user access level
 *
 * @param    index       (input) table index
 * @param    accessLevel (output) readonly or readwrite
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 userMgrLoginUserAccessModeGet(L7_uint32 index, L7_uint32 *accessLevel );


/*********************************************************************
 * @purpose  Return a login user snmpv3 access level
 *
 * @param    index       (input) table index
 * @param    accessLevel (output) readonly or readwrite
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 userMgrLoginUserSnmpv3AccessModeGet(L7_uint32 index, L7_uint32 *accessLevel );

/*********************************************************************
 * @purpose  Delete a login user name and settings
 *
 * @param    index       table index
 *
 * @returns  L7_SUCCESS
 * @returns L7_FAILURE if attempt to remove admin
 *
 * @notes    also sets login status to L7_disable
 *
 * @end
 *********************************************************************/
L7_uint32 userMgrLoginUserDelete(L7_uint32 index);


/*********************************************************************
 * @purpose  Checks if userMgr user config data is changed
 *
 * @param    void
 *
 * @returns  L7_TRUE
 * @returns  L7_FALSE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_BOOL userMgrHasDataChanged(void);
void userMgrResetDataChanged(void);

/*********************************************************************
 * @purpose  Build default userMgr config data
 *
 * @param    ver   Software version of Config Data
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void userMgrBuildDefaultConfigData(L7_uint32 ver);


/*********************************************************************
 * @purpose  Set logins back to Factory Defaults
 *
 * @param    void
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void userMgrConfigLoginsFactoryDefault(void);

/*********************************************************************
 *
 * @purpose compare 2 buffers.
 *
 * @param L7_char8 *buf1
 * @param L7_char8 *buf2
 *
 * @returns  L7_TRUE if buffers are the same.  L7_FALSE otherwise.
 *
 * @notes If the buffers can not be compared, L7_FALSE is returned.
 * @notes The buffers must be null terminated.
 *
 * @end
 *
 *********************************************************************/
L7_BOOL userMgrNoCaseCompare(L7_char8 *buf1, L7_char8 *buf2 );

/*********************************************************************
 * @purpose  Set APLs back to Factory Defaults
 *
 * @param    void
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void userMgrConfigAPLsFactoryDefault(void);

/*********************************************************************
 * @purpose  Pass all user config data to snmp through usmdb for snmpv3
 *
 * @param    none
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void userMgrInitUsers(void);


/*********************************************************************
 * @purpose  Saves userMgr user config file to NVStore
 *
 * @param    void
 *
 * @returns  L7_SUCCESS or L7_FALIURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrSave(void);

/*********************************************************************
 * @purpose  Enables the user manager component to be reset by changing
 *           the user manager ready flag to false.
 *
 * @returns  L7_SUCCESS
 *
 * @notes    Intended to be called before userMgrSysInit in case there
 *           is ever a need to re-initialize the user manager.
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrPrepareToReset(void);

/*********************************************************************
 * @purpose  Indicates when the user manager has completed loading config
 *
 * @returns  L7_SUCCESS  If userMgr has completed loading config
 * @returns  L7_FAILURE  If userMgr has NOT completed loading config
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_BOOL userMgrReady(void);


/*********************************************************************
 * @purpose  Restores the users into snmpv3 tables
 *
 * @returns  L7_SUCCESS  Retrored successfully
 * @returns  L7_FAILURE  Retore failed
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrSnmpUsersRestore(void);

/*********************************************************************
 * @purpose  Indicates when the user manager has completed loading config
 *
 * @returns  L7_SUCCESS  If userMgr has completed loading config
 * @returns  L7_FAILURE  If userMgr has NOT completed loading config
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrSnmpConfigSet(void);

/*********************************************************************
 * @purpose  Determine if a user is authenticated
 *
 * @param   request (input/output) pointer to structure containing
 *          data used for authentication.  Refer to the definition
 *          of the data structure for more information about each
 *          field in the structure.
 *
 * @returns L7_SUCCESS  If user is authenticated
 * @returns L7_FAILURE  If user is not authenticated due to unknown user, bad password,
 *                      challenge, or system error
 * @returns L7_ERROR  If all configured authentication methods failed to provide response due to timeout, etc.
 * @returns L7_NOT_SUPPORTED If specified component does not support APLs.
 *
 * @notes   The APL associated with the line is used to determine the
 *          appropriate authentication method(s) for this user.
 *
 * @notes This version of the interface does NOT support challenges
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrAuthenticateUser(userMgrAuthRequest_t *request);

/*********************************************************************
 * @purpose  Determine if a user is authenticated
 *
 * @param   request (input/output) pointer to structure containing
 *          data used for authentication.  Refer to the definition
 *          of the data structure for more information about each
 *          field in the structure.
 *
 * @returns L7_SUCCESS  If user is authenticated
 * @returns L7_FAILURE  If user is not authenticated due to unknown user, bad password,
 *                      challenge, or system error
 * @returns L7_ERROR  If all configured authentication methods failed to provide response due to timeout, etc.
 * @returns L7_NOT_SUPPORTED If specified component does not support APLs.
 *
 * @notes   The APL associated with the line is used to determine the
 *          appropriate authentication method(s) for this user.
 *
 * @notes This version of the interface DOES support challenges
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrAuthenticateUserAllowChallenge(userMgrAuthRequest_t *request);

/*********************************************************************
 * @purpose  Attempt to authenticate a user with local configuration
 *
 * @param   pUserName (input) ptr to user name to authenticate
 * @param   pPwd (input) ptr to user provided password
 * @param   pAccessLevel (output) access of authenticated user
 *
 * @returns L7_SUCCESS  If user is authenticated
 * @returns L7_FAILURE  If user is not authenticated due to unknown user, bad password, or system error
 *
 * @notes Local authentication does not incorporate the concept of timeout.
 *        All requests will either succeed or fail.
 *
 * @notes User login status must be set to enable for local authentication to succeed.
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrAuthenticateUserLocal(L7_char8 *pUserName, L7_char8 *pPwd, L7_uint32 *pAccessLevel);

/*********************************************************************
* @purpose  Determine if authentication is required (configured)
*
* @param   line (input) line application requesting authentication
* @param   mode (input) access level of application's auth sequence
*
* @returns L7_TRUE  If authentication is required, or other error
* @returns L7_FALSE  If authentication is not required, allow anyone access
*
* @notes   The APL associated with this user is used to determine the
*          appropriate authentication method(s) for this user.  If the
*          user is not configured locally, the APL for non-configured users
*          is consulted to determine handling.
* @end
*********************************************************************/
L7_BOOL userMgrIsAuthenticationRequired(L7_ACCESS_LINE_t line, L7_ACCESS_LEVEL_t mode);

/*********************************************************************
* @purpose  Determine if authentication requires a username
*
* @param   line (input) line application requesting authentication
* @param   mode (input) access level of application's auth sequence
*
* @returns L7_TRUE  If username is required, or other error
* @returns L7_FALSE  If username is not required, allow anyone access
*
* @notes   The APL associated with this user is used to determine the
*          appropriate authentication method(s) for this user.  If the
*          user is not configured locally, the APL for non-configured users
*          is consulted to determine handling.
* @end
*********************************************************************/
L7_BOOL userMgrIsAuthenticationUsernameRequired(L7_ACCESS_LINE_t line, L7_ACCESS_LEVEL_t mode);

/*********************************************************************
* @purpose  Determine if authentication method prompts for input
*
* @param   line (input) line application requesting authentication
* @param   mode (input) access level of application's auth sequence
*
* @returns L7_TRUE   If authentication method provides prompts
* @returns L7_FALSE  If authentication does not prompt
*
* @notes   The APL associated with this line is used to determine the
*          appropriate authentication method(s).
*
* @end
*********************************************************************/
L7_BOOL userMgrIsAuthenticationPromptSupplied(L7_ACCESS_LINE_t line, L7_ACCESS_LEVEL_t mode);

/*********************************************************************
* @purpose  Attempt to authenticate a user with line configuration
*
* @param   pUserName (input) ptr to user name to authenticate
* @param   pPwd (input) ptr to user provided password
* @param   pAccessLevel (output) access of authenticated user
*
* @returns L7_SUCCESS  If user is authenticated
* @returns L7_FAILURE  If user is not authenticated due to unknown user, bad password, or system error
*
* @notes Local authentication does not incorporate the concept of timeout.
*        All requests will either succeed or fail.
*
* @notes User login status must be set to enable for local authentication to succeed.
*
* @notes If the user access level can not be determined, READ_ONLY access will be set
*        and a message will be logged.
*
* @end
*********************************************************************/
L7_RC_t userMgrAuthenticateUserLine(L7_ACCESS_LINE_t line, L7_char8 *pPwd);

/*********************************************************************
 * @purpose  Attempt to authenticate a user with enable configuration
 *
 * @param   pPwd (input) ptr to user provided password
 *
 * @returns L7_SUCCESS  If user is authenticated
 * @returns L7_FAILURE  If user is not authenticated due to unknown user, bad password, or system error
 *
 * @notes Local authentication does not incorporate the concept of timeout.
 *        All requests will either succeed or fail.
 *
 * @notes User login status must be set to enable for local authentication to succeed.
 *
 * @notes If the user access level can not be determined, READ_ONLY access will be set
 *        and a message will be logged.
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrAuthenticateUserEnable(L7_uint32 level, L7_char8 *pPwd);

/*********************************************************************
 * @purpose To check whether the given password is of valid length or not
 *
 * @param  password - Password of the user
 *
 * @returns L7_SUCCESS -if the password is valid

 * @returns L7_FAILURE -if the password is not valid
 *
 * @notes  none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrPasswdLengthValid(L7_char8 *password, L7_BOOL encryptedFlag);

/*********************************************************************
 * @purpose To get the min password length
 *
 * @param void
 *
 * @returns Min Password Length
 * @notes  none
 *
 * @end
 *********************************************************************/
L7_uint32 userMgrPasswdGetConfiguredLength(void);

/*********************************************************************
 * @purpose To configure the min password length
 *
 * @param val -Min password length
 *
 * @returns L7_SUCCESS -if the value supplied is correct

 * @returns L7_FAILURE -if the value supplied is not correct

 * @notes This function is used to configure the min password length and to disable
 *         the already configued value.
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrMinPasswdSet(L7_uint32 val);

/*********************************************************************
 * @purpose To configure the History table size
 *
 * @param val -History table size
 *
 * @returns L7_SUCCESS -if the value supplied is correct
 *
 * @returns L7_FAILURE -if the value supplied is not correct
 *
 * @notes This function is used to configure the history table size and to disable
 *          the already configued value.
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrPasswdHistoryLengthSet(L7_uint32 val);

/*********************************************************************
 * @purpose To configure the aging value of the password
 *
 * @param val -indiactes for how many days the password is valid
 *
 * @returns L7_SUCCESS -if the value supplied is correct
 *
 * @returns L7_FAILURE -if the value supplied is not correct
 *
 * @notes This function is used to configure the aging of the password and to
 *         disable the already configued value.
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrPasswdAgingSet(L7_uint32 val);

/*********************************************************************
 * @purpose To configure the lockout count for the password
 *
 * @param val -indiactes the lock out count of the password
 *
 * @returns L7_SUCCESS -if the value supplied is correct

 * @returns L7_FAILURE -if the value supplied is not correct

 * @notes This function is used to configure the lockout count of the password
 *         and to disble the already configued value.
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrPasswdsLockoutSet(L7_uint32 val);

/*********************************************************************
 * @purpose To get the time when the password was created
 *
 * @param index -indiactes the corresponding user
 *
 * @output time - time when the password was set
 * @returns L7_SUCCESS -if the user index valid

 * @returns L7_FAILURE -if the user index is not valid

 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrPasswordChangeTimeGet(L7_uint32 index, L7_uint32 *time);

/*********************************************************************
 * @purpose To get the aging count of the password
 *
 * @param index -indiactes the corresponding user
 *
 * @output time - time when the password was set
 * @returns L7_SUCCESS -if the user index valid

 * @returns L7_FAILURE -if the user index is not valid

 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrPasswdValidationTimeGet(L7_uint32 *tempInt);

/*********************************************************************
 * @purpose  decides whther the password is expired or not
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    index    (input ) index of user in login structure
 *
 * @returns  L7_SUCCESS if the  value is proper
 *           L7_FAILURE  if the value is not proper
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrPasswordExpiredCheck (L7_uint32 index);

/*********************************************************************
 * @purpose  Gets the expiry time of the user at given the user table index
 *
 * @param    index    (input ) index of user in login structure
 * @param    time     (output ) expiry time (past or future)
 *
 * @returns  L7_SUCCESS if the  value is proper
 *           L7_FAILURE  if the value is not proper
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrExpireTimeGet(L7_uint32 index, L7_uint32 *time);

/*********************************************************************
 * @purpose To know whether the aging is enabled or disabled
 *
 * @param none
 *
 * @returns L7_SUCCESS -enabled

 * @returns L7_FAILURE -disabled

 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrPasswdAgingStatusGet(void);

/*********************************************************************
 * @purpose  To know whether the password history is enabled or not
 *
 * @param   none
 *
 * @returns  L7_SUCCESS if enabled
 *           L7_FAILURE  if not enabled
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrHistoryStatusGet(void);

/*********************************************************************
 * @purpose  To know whether the user lockout is enabled or not
 *
 * @param   none
 *
 * @returns  L7_SUCCESS if enabled
 *           L7_FAILURE  if not enabled
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrLockoutStatusGet(void);

/*********************************************************************
 * @purpose  To get the password aging value
 *
 * @param   val - (output) number of days the passwords are valid
 *
 * @returns L7_SUCCESS
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrPasswdAgingValueGet(L7_ushort16 *val);

/*********************************************************************
 * @purpose  To get the min password length value
 *
 * @param   val - (output) min number of characters required for passwords
 *
 * @returns  L7_SUCCESS
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrMinPassLengthGet(L7_ushort16 *val);

/*********************************************************************
 * @purpose  To get the history length value
 *
 * @param   val - (output) num passwords to store for prevention of reuse
 *
 * @returns  L7_SUCCESS
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrHistoryLengthGet(L7_ushort16 *val);

/*********************************************************************
 * @purpose  To get the no of user lockout attempts
 *
 * @param   val - (output) num failed attempts allowed before lockout
 *
 * @returns  L7_SUCCESS
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrLockoutAttemptsGet(L7_ushort16 *val);

/*********************************************************************
 * @purpose To know whether the user is been locked or not
 *
 * @param  index - (input) index for user in login structure
 * @param  val   - (output) whether the user account is locked out
 *
 * @returns L7_SUCCESS  value retrieved
 * @returns L7_FAILURE  value not retrieved
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrUserLockStatusGet(L7_uint32 index, L7_BOOL *val);

/*********************************************************************
 * @purpose To make the locked user unlocked
 *
 * @param none
 *
 * @returns L7_SUCCESS  -user is unlocked

 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrUserUnLock(L7_uint32 index);

/*********************************************************************
 * @purpose  Blocks/Unblocks a user
 *
 * @param    index       table index
 * @param    blockStatus L7_TRUE or L7_FALSE
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 userMgrLoginUserBlockStatusSet (L7_uint32 index, L7_BOOL blockStatus);

/*********************************************************************
 * @purpose  Gets the total number of times the user account
 *           has been locked
 *
 * @param    index    (input ) index of user in login structure
 * @param    val      (output ) count
 *
 * @returns  L7_SUCCESS if the  value is proper
 *           L7_FAILURE  if the value is not proper
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrTotalLockoutCountGet(L7_uint32 index, L7_uint32 *val);

/*********************************************************************
 * @purpose  Gets the total number of password failures for the user account
 *
 * @param    index    (input ) index of user in login structure
 * @param    val      (output ) count
 *
 * @returns  L7_SUCCESS if the  value is proper
 *           L7_FAILURE  if the value is not proper
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrTotalLoginFailuresGet(L7_uint32 index, L7_uint32 *val);

/*********************************************************************
 * @purpose  Gets the current password failure count used for determining
 *           when the account should be locked.  The value is reset on
 *           a successful login.
 *
 * @param    index    (input ) index of user in login structure
 * @param    val      (output ) count
 *
 * @returns  L7_SUCCESS if the  value is proper
 *           L7_FAILURE  if the value is not proper
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrCurrentLockoutCountGet(L7_uint32 index, L7_uint32 *val);

/*********************************************************************
 * @purpose  Get enable password
 *
 * @param    password    (input) enable password
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrEnablePasswordGet(L7_uint32 level,
                                 L7_char8 *password);


/*********************************************************************
 * @purpose  Set enable password
 *
 * @param    password    (input) enable password
 * @param    encrypted   (input) flag indicating whether the password param is encrypted
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrEnablePasswordSet(L7_uint32 level, L7_char8 *password, L7_BOOL encrypted);

/*********************************************************************
 * @purpose  Removes the enable password
 *
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrEnablePasswordRemove(L7_uint32 level);

/*********************************************************************
* @purpose  Returns table index of first available table index
*
* @param    index    pointer to table index
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
userMgrLoginAvailableIndexGet(L7_uint32 *index);

/*********************************************************************
* @purpose  Returns encrypted user password
*
* @param    password    input - pointer to clear text password
* @param    encryptedpwd    output = pointer to md5 encrypted password
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t userMgrLoginUserPasswordEncrypt(L7_char8 *password, L7_char8 *encryptedpwd);

/*********************************************************************
* @purpose  Blocks/Unblocks a user
*
* @param    index       table index
* @param    blockStatus L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 userMgrLoginUserBlockStatusSet(L7_uint32 index, L7_BOOL blockStatus);

/*********************************************************************
* @purpose To add an entry into the history table
*
* @param none
*
* @returns L7_SUCCESS  -user is unlocked

* @notes none
*
* @end
*********************************************************************/
L7_RC_t userMgrAddToLoginHistory(L7_char8 *name, L7_LOGIN_TYPE_t protocol, L7_inet_addr_t *location);

/*********************************************************************
* @purpose  To know whether the min password feature is enabled or not
*
* @param   none
*
* @returns  L7_SUCCESS if enabled
*           L7_FAILURE  if not enabled
*
* @notes none
*
* @end
*********************************************************************/

L7_RC_t userMgrMinPassStatusGet();

/*********************************************************************
* @purpose To add an entry into the history table
*
* @param none
*
* @returns L7_SUCCESS  -user is unlocked

* @notes none
*
* @end
*********************************************************************/
L7_RC_t userMgrLoginHistoryGetFirst(L7_uint32 *index, L7_char8 *name, L7_LOGIN_TYPE_t *protocol, L7_inet_addr_t  *location, L7_uint32 *time );

/*********************************************************************
* @purpose To add an entry into the history table
*
* @param none
*
* @returns L7_SUCCESS  -user is unlocked

* @notes none
*
* @end
*********************************************************************/
L7_RC_t userMgrLoginHistoryGetNext(L7_uint32 index, L7_uint32 *nextIndex,L7_char8 *name, L7_LOGIN_TYPE_t *protocol, L7_inet_addr_t  *location, L7_uint32 *time );

/*********************************************************************
* @purpose  Set the line password
*
* @param    accessLine      (input) line type (Console/Telnet/SSH))
* @param    password        (input) password to set
* @param    encrypted       (input) flag indicating whether the password param is encrypted
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t userMgrLinePasswordSet(L7_ACCESS_LINE_t accessLine,
                               L7_char8 *password,
                               L7_BOOL encrypted);

/*********************************************************************
* @purpose  Get the line password
*
* @param    accessLine      (input) line type (Console/Telnet/SSH))
* @param    password        (input) password to set
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t userMgrLinePasswordGet(L7_ACCESS_LINE_t accessLine,
                               L7_char8 *password);

/*********************************************************************
* @purpose  Remove the line password
*
* @param    accessLine   (input) line type (Console/Telnet/SSH))
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t userMgrLinePasswordRemove(L7_ACCESS_LINE_t accessLine);

/*********************************************************************
 * @purpose used to check the node level
 *
 *
 * @returns  L7_TRUE or L7_FALSE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_BOOL userMgrNodeLevelCheck(unsigned int user_level,unsigned int level,char condition);


#endif /* USER_MGR__API_H */
