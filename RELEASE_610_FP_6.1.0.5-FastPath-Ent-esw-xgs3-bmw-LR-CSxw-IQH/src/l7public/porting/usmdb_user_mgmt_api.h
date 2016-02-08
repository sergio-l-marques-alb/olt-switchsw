
/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename usmdb_user_mgmt_api.h
 *
 * @purpose interface for user information and login session information
 *
 * @component unitmgr
 *
 * @comments
 *
 * @create 04/10/2001
 *
 * @author fsamuels
 *
 * @end
 **********************************************************************/

#ifndef USMDB_USER_MGMT_API_H
#define USMDB_USER_MGMT_API_H

#include "user_manager_exports.h"
#include "l3_addrdefs.h"
#include "cli_web_exports.h"

extern L7_RC_t usmDbLoginSessionLogin(L7_uint32 UnitIndex, L7_uint32 *val);

extern L7_RC_t usmDbLoginSessionLogout(L7_uint32 UnitIndex, L7_uint32 index);

extern L7_RC_t usmDbLoginSessionValidEntry(L7_uint32 UnitIndex, L7_uint32 index, L7_BOOL *val);

/*********************************************************************
 * @purpose Determine the number of active login sessions
 *
 * @param L7_BOOL allSessions     Get a count of all active sessions
 * @param L7_uint32 sessionType   Session type to count
 *
 * @returns Number of active sessions
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
extern L7_uint32 usmDbLoginNumSessionsActiveGet(L7_BOOL allSessions, L7_uint32 sessionType);

/*********************************************************************
 * @purpose  Get the login session interface index
 *
 * @param    UnitIndex   ptr to Unit Index
 * @param    index       index value
 * @param    ifIndex     interface number
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbLoginSessionIfIndexGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *ifIndex);

/*********************************************************************
 * @purpose  Set the login session interface index
 *
 * @param    UnitIndex   ptr to Unit Index
 * @param    index       index value
 * @param    ifIndex     interface number
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbLoginSessionIfIndexSet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 ifIndex);

/*********************************************************************
 * @purpose  Get remote IP address for current login session
 *
 * @param    UnitIndex   ptr to Unit Index
 * @param    index       index value
 * @param    val         ptr to IP address
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbLoginSessionRemoteIpAddrGet(L7_uint32 UnitIndex, L7_uint32 index, L7_inet_addr_t *val);

/*********************************************************************
 * @purpose  Set remote IP address for current login session
 *
 * @param    UnitIndex   ptr to Unit Index
 * @param    index       index value
 * @param    val         ptr to IP address
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbLoginSessionRemoteIpAddrSet(L7_uint32 UnitIndex, L7_uint32 index, L7_inet_addr_t *val);

/*********************************************************************
 * @purpose  Get login session status
 *
 * @param    UnitIndex   ptr to Unit Index
 * @param    index       index value
 * @param    val         ptr buf contents
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbLoginSessionStatusGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *val);

/*********************************************************************
 * @purpose  Set login session status
 *
 * @param    UnitIndex   ptr to Unit Index
 * @param    index       index value
 * @param    val         ptr buf contents
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbLoginSessionStatusSet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 val);

/*********************************************************************
 *
 * @purpose Get the void pointer of user defined data.
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
 * @param L7_uint32 index      index value
 *
 * @returns *val       ptr to EwsContext for this session
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
extern void *usmDbLoginSessionUserStorageGet(L7_uint32 UnitIndex, L7_uint32 index);

/*********************************************************************
 * @purpose  Set the void pointer of user defined data
 *
 * @param    UnitIndex   ptr to Unit Index
 * @param    index       index value
 * @param    val         ptr buf contents
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbLoginSessionUserStorageSet(L7_uint32 UnitIndex, L7_uint32 index, void *val);

/*********************************************************************
 * @purpose  Get if the connection should be reset or not
 *
 * @param    UnitIndex       ptr to Unit Index
 * @param    index           index value
 * @param    resetConnection L7_TRUE if connection should be reset
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbLoginSessionResetConnectionGet(L7_uint32 UnitIndex, L7_uint32 index, L7_BOOL *resetConnection);

/*********************************************************************
 * @purpose  Set a flag for the connection to be reset
 *
 * @param    UnitIndex   ptr to Unit Index
 * @param    index       index value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbLoginSessionResetConnectionSet(L7_uint32 UnitIndex, L7_uint32 index);

/*********************************************************************
 * @purpose  Set a flag for ALL the connections to be reset
 *
 * @param    UnitIndex   ptr to Unit Index
 * @param    index       index value
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbLoginSessionResetConnectionAllSet(L7_uint32 UnitIndex);

/*********************************************************************
 * @purpose  Get login session time
 *
 * @param    UnitIndex   ptr to Unit Index
 * @param    index       index value
 * @param    *time       time
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbLoginSessionTimeGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *time);

/*********************************************************************
 * @purpose  Get login session connection time
 *
 * @param    UnitIndex   ptr to Unit Index
 * @param    index       index value
 * @param    *time       time
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbLoginSessionStartTimeGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *time);

/*********************************************************************
 * @purpose  Get login session idle time
 *
 * @param    UnitIndex   ptr to Unit Index
 * @param    index       index value
 * @param    *time       time
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbLoginSessionIdleTimeGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *time);

/*********************************************************************
 * @purpose  Update a users activity time
 *
 * @param    UnitIndex   ptr to Unit Index
 * @param    index       index value
 * @param    *time       time
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    called whenever a user does something
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbLoginSessionUpdateActivityTime(L7_uint32 UnitIndex, L7_uint32 index);

/*********************************************************************
 * @purpose  Get login session type
 *
 * @param    UnitIndex   ptr to Unit Index
 * @param    index       index value
 * @param    val         ptr to session type
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbLoginSessionTypeGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *val);

/*********************************************************************
 * @purpose  Set telnet session type
 *
 * @param    UnitIndex   ptr to Unit Index
 * @param    index       index value
 * @param    val         ptr to session type
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbLoginSessionTypeSet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 val);

/*********************************************************************
 * @purpose  Get telnet session user
 *
 * @param    UnitIndex   ptr to Unit Index
 * @param    index       index value
 * @param    buf         ptr buf contents
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbLoginSessionUserGet(L7_uint32 UnitIndex, L7_uint32 index, L7_char8 *buf);

/*********************************************************************
 * @purpose  Set login session username
 *
 * @param    UnitIndex   ptr to Unit Index
 * @param    index       index value
 * @param    buf         ptr buf contents
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbLoginSessionUserSet(L7_uint32 UnitIndex, L7_uint32 index, L7_char8 *buf);

/*********************************************************************
 * @purpose  Get telnet slot and port
 *
 * @param    UnitIndex   ptr to Unit Index
 * @param    index       index value
 * @param    buf         ptr buf contents
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbLoginSessionSlotPortGet(L7_uint32 UnitIndex, L7_uint32 index, L7_char8 *buf);

/*********************************************************************
 * @purpose  Get telnet slot and port
 *
 * @param    UnitIndex   ptr to Unit Index
 * @param    index       index value
 * @param    unit        unit number
 * @param    slot        slot number
 * @param    port        port number
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbLoginSessionUnitSlotPortSet(L7_uint32 UnitIndex, L7_uint32 index,
    L7_uint32 unit, L7_uint32 slot, L7_uint32 port);

/*********************************************************************
 * @purpose  Get a users login information index
 *
 * @param    UnitIndex The unit for this operation
 * @param    name     pointer to loing user name
 * @param    index    pointer to table index
 *
 * @returns  L7_SUCCESS if the user exists and the login index was set
 * @returns L7_FAILURE if the user does not exist or the index was not set
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbUserLoginIndexGet(L7_uint32 UnitIndex, L7_char8 *name, L7_uint32 *index);

/*********************************************************************
 *
 * @purpose Retrieve Access mode
 *
 * @param L7_uint32 UnitIndex @b((input))  The unit for this operation
 * @param L7_uint32 index     @b((input))  Index value
 * @param L7_uint32 val       @b((output)) Snmp community access level
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @notes  none
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbAccessModeGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *val);


/*********************************************************************
 * @purpose  Checks if the index is a valid index.
 *
 * @param    UnitIndex   unit index
 * @param    index       table index
 *
 * @returns  L7_SUCCESS   if the index exist and it is enable
 *           L7_FAILURE   if the index doesn't exist.
 *
 * @notes    Returns success if the next index exists
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbLoginUserIndexValidGet(L7_uint32 UnitIndex, L7_uint32 index);


/*********************************************************************
 * @purpose  Returns the next valid index
 *
 * @param    UnitIndex   unit index
 * @param    index       table index
 *
 * @returns  L7_SUCCESS   if the next index exist and it is enable
 *           L7_FAILURE   if the next index doesn't exist.
 *
 * @notes    Returns success if the next index exists
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbLoginUserIndexNextValidGet(L7_uint32 UnitIndex, L7_uint32 *index);


/*********************************************************************
 * @purpose  Returns login user status per index
 *
 * @param    UnitIndex   table index
 * @param    index       table index
 * @param    val         pointer to login user status (L7_ENABLE of L7_DISABLE)
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbLoginStatusGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *val);

/*********************************************************************
 * @purpose  Set login user status per index
 *
 * @param    UnitIndex   table index
 * @param    index       table index
 * @param    val         pointer to login user status (L7_ENABLE of L7_DISABLE)
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbLoginStatusSet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 val);

/*********************************************************************
 * @purpose  Returns login user name per index
 *
 * @param    UnitIndex   table index
 * @param    index       table index
 * @param    buf         pointer to login user name
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbLoginsGet(L7_uint32 UnitIndex, L7_uint32 index, L7_char8 *buf);

/*********************************************************************
 * @purpose  Setup new username and password
 *
 * @param    UnitIndex   table index
 * @param    index       table index
 * @param    buf         pointer to login user name
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbLoginsSet(L7_uint32 UnitIndex, L7_uint32 index, L7_char8 *buf);

/*********************************************************************
 * @purpose  Returns login user password per index
 *
 * @param    UnitIndex   table index
 * @param    index       table index
 * @param    buf         pointer to login user password
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbPasswordGet(L7_uint32 UnitIndex, L7_uint32 index, L7_char8 *buf);

/*********************************************************************
 * @purpose  Sets a login user password per index
 *
 * @param    UnitIndex   table index
 * @param    index       table index
 * @param    buf         pointer to login user password
 * @param    encrypted  (input) flag indicating whether the password param is encrypted
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbPasswordSet(L7_uint32 UnitIndex, L7_uint32 index, L7_char8 *buf, L7_BOOL encrypted);


/*********************************************************************
 *
 * @purpose Checks an input enteredString against a stored pw.
 *
 * @param L7_uint32 UnitIndex       the unit for this operation
 * @param L7_uint32 pw              the password
 * @param L7_uint32 enteredString   the string to check
 *
 * @returns L7_SUCCESS or   - if passwords match
 * @returns L7_FAILURE      - if passwords don't match
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
extern L7_RC_t usmDbPasswordIsValid(L7_uint32 UnitIndex, L7_char8 *pw, L7_char8 *enteredString);



/*********************************************************************
 *
 * @purpose Set the authentication parameters for a user
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
 * @param L7_uint32 index      table index
 * @param L7_uint32 authProt   authentication protocol
 *
 * @returns L7_SUCCESS or
 * @returns L7_FAILURE
 *
 * @notes The user login password is used as the authentication key
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbUserAuthSet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 authProt);

/*********************************************************************
 *
 * @purpose Set the encryption parameters for a user
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
 * @param L7_uint32 index      table index
 * @param L7_uint32 encryptProt   encryption protocol
 * @param L7_char8 *encryptKey    encryption key
 *
 * @returns L7_SUCCESS or
 * @returns L7_FAILURE
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbUserEncryptSet(L7_uint32 UnitIndex, L7_uint32 index,
    L7_uint32 encryptProt, L7_char8 *encryptKey );


/*********************************************************************
 *
 * @purpose Set the access level for a user
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
 * @param L7_uint32 index      table index
 * @param L7_uint32 accessLevel
 *
 * @returns L7_SUCCESS or
 * @returns L7_FAILURE
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbUserAccessLevelSet(L7_uint32 UnitIndex, L7_uint32 index,
    L7_uint32 accessLevel );


/*********************************************************************
 *
 * @purpose Return the authentication parameters for a user
 *
 * @param L7_uint32 UnitIndex  (input) the unit for this operation
 * @param L7_uint32 index      (input) table index
 * @param L7_uint32 *authProt   (output) authentication protocol
 *
 * @returns L7_SUCCESS or
 * @returns L7_FAILURE
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbUserAuthGet(L7_uint32 UnitIndex, L7_uint32 index,
    L7_uint32 *authProt);

/*********************************************************************
 *
 * @purpose Return the encryption parameters for a user
 *
 * @param L7_uint32 UnitIndex  (input) the unit for this operation
 * @param L7_uint32 index      (input) table index
 * @param L7_uint32 *encryptProt   (output) encryption protocol
 *
 * @returns L7_SUCCESS or
 * @returns L7_FAILURE
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbUserEncryptGet(L7_uint32 UnitIndex, L7_uint32 index,
    L7_uint32 *encryptProt);


/*********************************************************************
 *
 * @purpose Return the access level for a user
 *
 * @param L7_uint32 UnitIndex  (input) the unit for this operation
 * @param L7_uint32 index      (input) table index
 * @param L7_uint32 * accessLevel (output)
 *
 * @returns L7_SUCCESS or
 * @returns L7_FAILURE
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbUserAccessLevelGet(L7_uint32 UnitIndex, L7_uint32 index,
    L7_uint32 *accessLevel );

/*********************************************************************
 *
 * @purpose Deletes a user name and all settings for the user
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
 * @param L7_uint32 index      table index
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE if the attempt is to delete the admin
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbLoginsDelete(L7_uint32 UnitIndex, L7_uint32 index);

/*********************************************************************
 * @purpose  Determine if a user is authenticated
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param   pUserName (input) ptr to user name to authenticate
 * @param   pPwd (input) ptr to user provided password
 * @param   pAccessLevel (output) access of authenticated user
 * @param   line (input) which line type the user is using
 * @param   mode (input) which mode to authenticate (login/enable)
 * @param   port (input) pointer to text describing the port
 * @param   rem_addr (input) pointer to the IP address initiating access
 *
 * @returns L7_SUCCESS  If user is authenticated
 * @returns L7_FAILURE  If user is not authenticated due to unknown user, bad password,
 *                      challenge, or system error
 * @returns L7_ERROR  If all configured authentication methods failed to provide response due to timeout, etc.
 * @returns L7_NOT_SUPPORTED If specified component does not support APLs.
 *
 * @notes   The APL associated with this line is used to determine the
 *          appropriate authentication method(s) for this user.
 *
 * @notes This (simple) interface does NOT support challenges
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbAuthenticateUser(L7_uint32 UnitIndex,
                              L7_char8 *pUserName, L7_char8 *pPwd,
                              L7_uint32 *pAccessLevel,
                              L7_ACCESS_LINE_t line,
                              L7_ACCESS_LEVEL_t mode,
                              L7_uchar8 *port,
                              L7_uchar8 *rem_addr);

/*********************************************************************
 * @purpose  Determine if a user is authenticated
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param   pUserName (input) ptr to user name to authenticate
 * @param   pPwd (input) ptr to user provided password
 * @param   component (input) component requesting authentication
 * @param   pAccessLevel (output) access of authenticated user
 * @param   line (input) which line type the user is using
 * @param   mode (input) which mode to authenticate (login/enable)
 * @param   port (input) pointer to text describing the port
 * @param   rem_addr (input) pointer to the IP address initiating access
 * @param   pState (input/output) state returned on previous challenge (if applicable)
 * @param   pChallengeFlag (output) indicates if authentication is challenged
 * @param   pChallengePhrase (output) challenge phrase
 * @param   pChallengeFlags (output) pointer to flags returned by authentication method
 *
 * @returns L7_SUCCESS  If user is authenticated
 * @returns L7_FAILURE  If user is not authenticated due to unknown user, bad password,
 *                      challenge, or system error
 * @returns L7_ERROR  If all configured authentication methods failed to provide response due to timeout, etc.
 * @returns L7_NOT_SUPPORTED If specified component does not support APLs.
 *
 * @notes   The APL associated with this line is used to determine the
 *          appropriate authentication method(s) for this user.
 *
 * @notes This version of the interface DOES support challenges
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbAuthenticateUserAllowChallenge(L7_uint32 UnitIndex,
                                            L7_char8 *pUserName, L7_char8 *pPwd,
                                            L7_uint32 component, L7_uint32 *pAccessLevel,
                                            L7_ACCESS_LINE_t line,
                                            L7_ACCESS_LEVEL_t mode,
                                            L7_uchar8 *port,
                                            L7_uchar8 *rem_addr,
                                            L7_uchar8 *pState, L7_BOOL *pChallengeFlag,
                                            L7_uchar8 *pChallengePhrase, L7_uint32 *pChallengeFlags);

/*********************************************************************
 * @purpose  Attempt to authenticate a user with enable configuration
 *
 * @param   enteredPwd (input) ptr to user provided password
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
L7_RC_t usmDbLoginAuthenticateUserEnable(L7_uint32 level, L7_char8 *enteredPwd);

/*********************************************************************
 * @purpose  Creates a named APL with no authentication methods
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    pName    pointer to APL name
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE if name is empty or too long or if max number of APLs has
 *              been reached.
 *
 * @notes pName must be null terminated.
 *
 * @end
 *********************************************************************/
L7_uint32
usmDbAPLCreate(L7_ACCESS_LINE_t accessLine,
                                        L7_ACCESS_LEVEL_t accessLevel,
                                        L7_char8 *pName );

/*********************************************************************
 * @purpose  Deletes a named APL
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    pName    pointer to APL name
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE if APL does not exist or users have this APL assigned
 *             and therefore it cannot be deleted or if APL name is not valid.
 *
 * @notes  pName must be null terminated.
 *
 * @end
 *********************************************************************/
L7_uint32
usmDbAPLDelete(L7_ACCESS_LINE_t accessLine,
                                      L7_ACCESS_LEVEL_t accessLevel, 
                                      L7_char8 *pName );

/*********************************************************************
 * @purpose  Determines if the specified APL is the system default
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    pName    pointer to APL name
 *
 * @returns  L7_TRUE if the specified APL is the defaults
 *           L7_FALSE if the specified APL is not the default
 *
 * @notes  This routine is helpful to determine why an attempt to delete
 *         an APL fails.
 *
 * @end
 *********************************************************************/
L7_uint32
usmDbAPLDefaultCheck(L7_ACCESS_LINE_t accessLine,
                                            L7_ACCESS_LEVEL_t accessLevel, 
                                            L7_char8 *pName );

/*********************************************************************
 * @purpose  Returns the APL at the specified index in the list
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    pName    (output) pointer to APL name
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE if index is not valid or if there is not
 *                      an APL at the specified index.
 *
 * @notes index begins with 0
 *
 * @end
 *********************************************************************/
L7_RC_t
usmDbAPLListGetIndex(L7_ACCESS_LINE_t accessLine, L7_ACCESS_LEVEL_t accessLevel, 
                     L7_uint32 index, L7_char8 *pName );

/*********************************************************************
 * @purpose  Returns the first configured APL
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    pName    (output) pointer to APL name
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE
 *
 * @notes pName must be at least L7_MAX_APL_NAME_SIZE+1 in length
 *
 * @end
 *********************************************************************/
L7_RC_t
usmDbAPLListGetFirst(L7_ACCESS_LINE_t accessLine, 
                                        L7_ACCESS_LEVEL_t accessLevel, 
                                        L7_char8 *pName );

/*********************************************************************
 * @purpose  Gets the APL following the specified name in the APL list
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    pName    pointer to current APL name
 * @param    pNextName pointer to next APL name
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE if current APL name does not correspond to a
 *              configured APL or if the end of the list has been reached.
 *
 * @notes pName must be NULL terminated
 * @notes pNextName must be at least L7_MAX_APL_NAME_SIZE+1 in length
 *
 * @end
 *********************************************************************/
L7_RC_t
usmDbAPLListGetNext(L7_ACCESS_LINE_t accessLine,
                                       L7_ACCESS_LEVEL_t accessLevel,
                                       L7_char8 *pName, 
                                       L7_char8 *pNextName );

/*********************************************************************
 * @purpose  Adds an authentication method to an APL
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    pName    (input) pointer to APL name
 * @param    order    (input) order this method should be attempted in the APL
 * @param    method   (input) authentication method value
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE if order is >= L7_MAX_AUTH_METHODS or
 *                      if method is not recognized or
 *                      if name does not match a configured APL
 *
 * @notes Order begins with 0
 * @notes To remove an auth method, issue a set with L7_AUTH_METHOD_UNDEFINED.
 *        All auth methods will be attempted in the APL until a definitive
 *        response is found.  If there is an UNDEFINED entry in the middle of
 *        the APL, that entry will be skipped and the remaining entries will
 *        be used.
 *
 * @end
 *********************************************************************/
L7_RC_t
usmDbAPLAuthMethodSet(L7_ACCESS_LINE_t accessLine, 
                                             L7_ACCESS_LEVEL_t accessLevel,
                                             L7_char8 *pName, 
                                             L7_uint32 order, 
                                             L7_USER_MGR_AUTH_METHOD_t method );

/*********************************************************************
 * @purpose  Returns the authentication method at the specified index in
 *           the APL
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    pName    (input) pointer to APL name
 * @param    order    (input) order this method should be attempted in the APL
 * @param    pMethod   (output) authentication method value
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE if order is >= L7_MAX_AUTH_METHODS or
 *                      if method is not recognized or
 *                      if name does not match a configured APL
 *
 * @notes Order begins with 0
 * @notes The authentication methods should be attempted in order
 *
 * @end
 *********************************************************************/
L7_uint32
usmDbAPLAuthMethodGet(L7_ACCESS_LINE_t accessLine, 
                                             L7_ACCESS_LEVEL_t accessLevel, 
                                             L7_char8 *pName, 
                                             L7_uint32 order, 
                                             L7_USER_MGR_AUTH_METHOD_t *pMethod );

/*********************************************************************
 * @purpose  Assign an APL to a user for a specific component
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    pUser    (input) pointer to user name
 * @param    component (input)ID of component to use APL
 * @param    pAPL     (input) pointer to APL name
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE if APL name is not a configured APL or
 *                      if user name is not a configured user or
 *                      if component id is not a component that handles APLs
 *
 * @notes All users will always have an APL configured for all components that
 *        expect them.  Therefore, APL assignments can not be removed just re-set.
 * @notes APL name must be NULL terminated.
 *
 * @end
 *********************************************************************/
L7_uint32
usmDbAPLUserSet(L7_uint32 UnitIndex, L7_char8 *pUser, L7_uint32 component, L7_char8 *pAPL );

/*********************************************************************
 * @purpose  Return an APL assigned to a user for a specific component
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    pUser    (input ) pointer to user name
 * @param    component (input) ID of component to use APL
 * @param    pAPL     (output) pointer to APL name
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE  if component id is not a component that handles APLs
 *
 * @notes If the user is not configured, the APL for non-configured users
 *        is returned.  Therefore, this function can not be used to determine
 *        if a user is configured locally.
 *
 * @notes The APL name must be at least L7_MAX_APL_NAME_SIZE+1 in length.  The
 *        value set will be NULL terminated.
 *
 * @end
 *********************************************************************/
L7_uint32
usmDbAPLUserGet(L7_uint32 UnitIndex, L7_char8 *pUser, L7_uint32 component, L7_char8 *pAPL );

/*********************************************************************
 * @purpose  Return an APL assigned to a user (by index) for a specific component
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    index    (input ) index of user in login structure
 * @param    component (input) ID of component to use APL
 * @param    pAPL     (output) pointer to APL name
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE  if the index is not valid or
 *                       if the component is not valid
 *
 * @notes This function will only return the APL for the locally configured
 *        user at the specified index.
 *
 * @notes The APL name must be at least L7_MAX_APL_NAME_SIZE+1 in length.  The
 *        value set will be NULL terminated.
 *
 * @end
 *********************************************************************/
L7_uint32
usmDbAPLUserGetIndex(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 component, L7_char8 *pAPL );

/*********************************************************************
 * @purpose  Assign an APL for nonconfigured users for a specific component
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    component (input)ID of component to use APL
 * @param    pAPL     (input) pointer to APL name
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE if APL name is not a configured APL or
 *                      if component id is not a component that handles APLs
 *
 * @notes pAPL must be NULL terminated
 *
 * @end
 *********************************************************************/
L7_uint32
usmDbAPLNonConfiguredUserSet(L7_uint32 UnitIndex, L7_uint32 component, L7_char8 *pAPL );

/*********************************************************************
 * @purpose  Return the APL assigned to nonconfigured users for a specific component
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    component (input) ID of component to use APL
 * @param    pAPL     (output) pointer to APL name
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE  if component id is not a component that handles APLs
 *
 * @notes pAPL must be at least L7_MAX_APL_NAME_SIZE+1 in length
 *
 * @end
 *********************************************************************/
L7_uint32
usmDbAPLNonConfiguredUserGet(L7_uint32 UnitIndex, L7_uint32 component, L7_char8 *pAPL );

/*********************************************************************
 * @purpose  Return the first user in the login structure using the specified APL
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    pAPLName (input) ptr to APL name
 * @param    pUserName (output) ptr to user name
 * @param    pComponent (output) ptr to component
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE  if there are no users assigned this APL or
 *                       if the APL name is not configured
 *
 * @notes If the nonconfigured user is assigned this APL, the user name is set
 *        with text that indicates non-configured user.
 *
 * @end
 *********************************************************************/
L7_uint32
usmDbAPLAllUsersGetFirst(L7_uint32 UnitIndex, L7_char8 *pAPLName, L7_char8 *pUserName, L7_uint32 *pComponent );

/*********************************************************************
 * @purpose  Return the first user in the login structure using the specified APL
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    pAPLName (input) ptr to APL name
 * @param    pCurrentUserName (input) ptr to user name from getFirst or priorGetNext
 * @param    currentComponent (input) component ID from getFirst or priorGetNext
 * @param    pNextUserName (output) ptr to next user name
 * @param    pNextComponent (output) ptr to next component ID
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE  if there are no more users assigned this APL.
 *           L7_ERROR  if the APL name is not configured or if the specified
 *                     user name or component id is not valid
 *
 * @notes If the nonconfigured user is assigned this APL, the user name is set
 *        with text that indicates non-configured user.
 *
 * @end
 *********************************************************************/
L7_uint32
usmDbAPLAllUsersGetNext(L7_uint32 UnitIndex, L7_char8 *pAPLName,
    L7_char8 *pCurrentUserName, L7_uint32 currentComponent,
    L7_char8 *pNextUserName, L7_uint32 *pNextComponent );

/*********************************************************************
 * @purpose  configures the min password
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    val     min length of the password
 *
 * @returns  L7_SUCCESS  if the min length of the password is set
 *           L7_FAILURE  if the min length of the password is not set
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbMinPasswdSet(L7_uint32 UnitIndex,  L7_uint32 val);

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
L7_RC_t usmDbPasswdLengthValid(L7_uint32 UnitIndex,  L7_char8 * password, L7_BOOL encryptedFlag);

/*********************************************************************
 * @purpose  configures the history table size
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    val      length of the history table
 *
 * @returns  L7_SUCCESS if the history table size is set
 *           L7_FAILURE  if the history table size is not  set
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbPasswdHistoryLengthSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
 * @purpose  configures the aging of the password
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    val      aging value of the password
 *
 * @returns  L7_SUCCESS if the aging value is set
 *           L7_FAILURE  if the aging value is not set
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbPasswdAgingSet(L7_uint32 UnitIndex,  L7_uint32 val);

/*********************************************************************
 * @purpose  configures the lockout count of the password
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    val      lock out count
 *
 * @returns  L7_SUCCESS if the lockout count is set
 *           L7_FAILURE  if the lockout count is not set
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbPasswdsLockoutSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
 * @purpose  Gives the time when the password got set
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    index    (input ) index of user in login structure
 * @param    val      lock out count
 *
 * @returns  L7_SUCCESS if the  value is proper
 *           L7_FAILURE  if the value is not proper
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbPasswordChangeTimeGet(L7_uint32 UnitIndex,L7_uint32 index, L7_uint32 *time);

/*********************************************************************
 * @purpose  Gives the current time
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param   tempint   (output )pointer to the time variable
 *
 * @returns  L7_SUCCESS if the  value is proper
 *           L7_FAILURE  if the value is not proper
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbPasswdValidationTimeGet(L7_uint32 UnitIndex,L7_uint32 *tempInt);

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

L7_BOOL usmDbPasswordExpiredCheck(L7_uint32 UnitIndex,L7_uint32 index);

/*********************************************************************
 * @purpose  Gets the time the user's password expired
 *
 * @param   UnitIndex (input) the unit for this operation
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
L7_RC_t usmDbPasswordExpireTimeGet(L7_uint32 UnitIndex,L7_uint32 index, L7_uint32 *time);

/*********************************************************************
 * @purpose  decides whther the password is expired or not
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    index    (output ) index of user in login structure
 * @param    name    (input ) user name
 *
 * @returns  L7_SUCCESS if the  value is proper
 *           L7_FAILURE  if the value is not proper
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbLoginUserIndexGet(L7_uint32 UnitIndex, L7_char8 *name, L7_uint32 *index);

/*********************************************************************
 * @purpose  To know whether the password aging is enabled or not
 *
 * @param   none
 *
 * @returns  L7_SUCCESS if the  value is proper
 *           L7_FAILURE  if the value is not proper
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbPasswdAgingStatusGet();

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
L7_RC_t usmDbHistoryStatusGet();

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
L7_RC_t usmDbLockoutStatusGet();

/*********************************************************************
 * @purpose  To get the password aging value
 *
 * @param   val - (output) number of days the passwords are valid
 *
 * @returns  L7_SUCCESS  if value retrieved
 *           L7_FAILURE  if value was not retrieved
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbPasswdAgingValueGet(L7_ushort16 *val);

/*********************************************************************
 * @purpose  To get the min password length value
 *
 * @param   val - (output) min number of characters required for passwords
 *
 * @returns  L7_SUCCESS  if value retrieved
 *           L7_FAILURE  if value was not retrieved
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbMinPassLengthGet(L7_ushort16 *val);

/*********************************************************************
 * @purpose  To get the history length value
 *
 * @param   val - (output) num passwords to store for prevention of reuse
 *
 * @returns  L7_SUCCESS  if value retrieved
 *           L7_FAILURE  if value was not retrieved
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbHistoryLengthGet(L7_ushort16 *val);

/*********************************************************************
 * @purpose  To get the number of user lockout attempts
 *
 * @param   val - (output) num failed attempts allowed before lockout
 *
 * @returns  L7_SUCCESS  if value retrieved
 *           L7_FAILURE  if value was not retrieved
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbLockoutAttemptsGet(L7_ushort16 *val);

/*********************************************************************
 * @purpose  To know whether the user is locked or not
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    index    (input ) index of user in login structure
 * @param    val      (output ) lock out status value
 *
 * @returns  L7_SUCCESS if the  value is proper
 *           L7_FAILURE  if the value is not proper
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserLockStatusGet(L7_uint32 UnitIndex, L7_uint32 index, L7_BOOL *val);

/*********************************************************************
 * @purpose  To unlock the locked user
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    index    (output ) index of user in login structure
 *
 * @returns  L7_SUCCESS if the  value is proper
 *           L7_FAILURE  if the value is not proper
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserUnLock(L7_uint32 UnitIndex, L7_uint32 index);

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
L7_RC_t usmDbUserTotalLockoutCountGet(L7_uint32 index, L7_uint32 *val);

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
L7_RC_t usmDbUserTotalLoginFailuresGet(L7_uint32 index, L7_uint32 *val);

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
L7_RC_t usmDbUserCurrentLockoutCountGet(L7_uint32 index, L7_uint32 *val);

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
L7_RC_t usmDbEnablePasswordSet(L7_uint32 level, L7_char8 *password, L7_BOOL encrypted);

/*********************************************************************
 * @purpose  Get enable password
 *
 * @param    password    (output) enable password
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbEnablePasswordGet(L7_uint32 level, 
                               L7_char8 *password);

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
L7_RC_t usmDbEnablePasswordRemove(L7_uint32 level);

/*********************************************************************
 * @purpose  Set the Terminal Lines for show running-config
 *
 * @param unitIndex @b((input)) the unit for this operation
 *
 * @param    termLine   value of terminal lines for pagination<5-48>
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbTerminalLineSet(L7_uint32 unitIndex, L7_uint32 termLine);

/*********************************************************************
 * @purpose  Get the Terminal Lines for show running-config
 *
 * @param unitIndex @b((input)) the unit for this operation
 *
 * @param    *termLine   value of terminal lines for pagination<5-48>
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbTerminalLineGet(L7_uint32 UnitIndex, L7_uint32 *termLine);

/*********************************************************************
* @purpose  Get the APL assigned to an access line
*
* @param    component (input) ID of component to use APL
* @param    pAPL     (output) pointer to APL name
*
* @returns  L7_SUCCESS
*           L7_FAILURE  if component id is not a component that handles APLs
*
* @notes pAPL must be at least L7_MAX_APL_NAME_SIZE+1 in length
*
* @end
*********************************************************************/
L7_RC_t usmDbAPLLineGet(L7_ACCESS_LINE_t accessLine,
                        L7_ACCESS_LEVEL_t accessLevel,
                        L7_char8 *pAPL);

/*********************************************************************
* @purpose  Set the APL assigned to an access line
*
* @param    component (input) ID of component to use APL
* @param    pAPL     (output) pointer to APL name
*
* @returns  L7_SUCCESS
*           L7_FAILURE  if component id is not a component that handles APLs
*
* @notes pAPL must be at least L7_MAX_APL_NAME_SIZE+1 in length
*
* @end
*********************************************************************/
L7_RC_t usmDbAPLLineSet(L7_ACCESS_LINE_t accessLine,
                        L7_ACCESS_LEVEL_t accessLevel,
                        L7_char8 *pAPL);

/*********************************************************************
* @purpose  Return the index of the specified APL in the APL list
*
* @param   UnitIndex (input) the unit for this operation
* @param    pAPLName (input) name of APL
* @param    pIndex (output) ptr to APL index
*
* @returns  L7_SUCCESS
*           L7_FAILURE if APL name is not configured
*
* @notes This function can be used to find the first blank entry in the
*        APL list.  Therefore, a name of size 0 is valid.
*
* @end
*********************************************************************/
L7_RC_t usmDbAuthenticationListIndexGetByName(L7_ACCESS_LINE_t accessLine,
                                              L7_ACCESS_LEVEL_t accessLevel,
                                              L7_char8 * pAPLName,
                                              L7_uint32 *pIndex);

/*********************************************************************
* @purpose  Removes the methods from a specific APL
*
* @param    authMode <input>  the authentication Mode for this operation
* @param    pName    pointer to APL name
*
* @returns  L7_SUCCESS
*           L7_FAILURE if APL does not exist or users have this APL assigned
*             and therefore it cannot be deleted or if APL name is not valid.
*
* @notes  pName must be null terminated.
*
* @end
*********************************************************************/
L7_RC_t usmDbAuthenticationListMethodRemove(L7_ACCESS_LINE_t accessLine,
                                            L7_ACCESS_LEVEL_t accessLevel,
                                            L7_char8 *pName);

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
L7_RC_t usmDbMinPassStatusGet();

/*********************************************************************
* @purpose  Set the line password
*
* @param    accessLine      (input) line type (Console/Telnet/SSH))
* @param    password        (input) password to set
* @param    encrypted       (input) flag indicating whether the password param is encrypted
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLinePasswordSet(L7_ACCESS_LINE_t accessLine,
                             L7_char8 *password,
                             L7_BOOL encrypted);

/*********************************************************************
* @purpose  Get the line password
*
* @param    accessLine      (input) line type (Console/Telnet/SSH))
* @param    password        (output) password to set
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLinePasswordGet(L7_ACCESS_LINE_t accessLine,
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
L7_RC_t usmDbLinePasswordRemove(L7_ACCESS_LINE_t accessLine);

/*********************************************************************
* @purpose  To add the entry into the login history table.
*
* @param   UnitIndex (input) the unit for this operation
* @param    index    (output ) index of user in login structure
*
* @returns  L7_SUCCESS if the  value is proper
*           L7_FAILURE  if the value is not proper
*
* @notes none
*
* @end
*********************************************************************/

L7_RC_t  usmdbUserMgrAddToLoginHistory(L7_uint32 UnitIndex, L7_char8 *name, L7_LOGIN_TYPE_t protocol, L7_inet_addr_t *location);

/*********************************************************************
* @purpose  To get the first entry from the history table.
*
* @param   UnitIndex (input) the unit for this operation
* @param    index    (output ) index of user in login structure
*
* @returns  L7_SUCCESS if the  value is proper
*           L7_FAILURE  if the value is not proper
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t  usmdbUserMgrLoginHistoryGetFirst(L7_uint32 UnitIndex, L7_uint32 *index,
                                          L7_char8 *name, L7_LOGIN_TYPE_t *protocol,
                                          L7_inet_addr_t  *location, L7_uint32 *time );

/*********************************************************************
* @purpose  To get the next entry from the history table.
*
* @param   UnitIndex (input) the unit for this operation
* @param    index    (output ) index of user in login structure
*
* @returns  L7_SUCCESS if the  value is proper
*           L7_FAILURE  if the value is not proper
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t usmdbUserMgrLoginHistoryGetNext(L7_uint32 UnitIndex, L7_uint32 index,
                                        L7_uint32 *nextIndex,L7_char8 *name,
                                        L7_LOGIN_TYPE_t *protocol,
                                        L7_inet_addr_t  *location, L7_uint32 *time );

/*********************************************************************
* @purpose  Get the next vacant index in the user table
*
* @param    index    (output ) index of user in login structure
*
* @returns  L7_SUCCESS if the  value is proper
*           L7_FAILURE  if the value is not proper
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t usmDbLoginUserIndexNextAvailableGet(L7_uint32 *index);

/*********************************************************************
* @purpose  Check if Authentication is required
*
* @param    accessLine   (input) line type (Console/Telnet/SSH))
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL usmDbIsAuthenticationRequired(L7_ACCESS_LINE_t accessLine, L7_ACCESS_LEVEL_t accessMode);

/*********************************************************************
* @purpose  Check if Authentication is required for this User
*
* @param    accessLine   (input) line type (Console/Telnet/SSH))
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL usmDbIsAuthenticationUsernameRequired(L7_ACCESS_LINE_t accessLine, L7_ACCESS_LEVEL_t accessMode);

/*********************************************************************
* @purpose  Check if Prompt is required
*
* @param    accessLine   (input) line type (Console/Telnet/SSH))
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL usmDbIsAuthenticationPromptSupplied(L7_ACCESS_LINE_t accessLine, L7_ACCESS_LEVEL_t accessMode);

#endif /* USMDB_USER_MGMT_API_H */
