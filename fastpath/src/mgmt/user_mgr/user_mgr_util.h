
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename   user_mgr.h
*
* @purpose    User Manager util header file
*
* @component  user_mgr component
*
* @comments   For utility functions private to user_mgr.
*
* @create     09/27/2002
*
* @author     Jill Flanagan
*
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/


#ifndef USER_MGR_UTIL_H
#define USER_MGR_UTIL_H


/*********************************************************************
* @purpose  Build default config data
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void
userMgrBuildDefaultConfigData(L7_uint32 ver);

/*********************************************************************
* @purpose  Calls snmpUserSet if snmp is supported
*
* @param    index - offset into cfg structure
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t userMgrSnmpUserSet(L7_uint32 index );


/*********************************************************************
* @purpose  Calls snmpUserDelete if snmp is supported
*
* @param    index - offset into cfg structure
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t userMgrSnmpUserDelete(L7_uint32 index );

/*********************************************************************
* @purpose  Restore all user config data to snmp through usmdb for snmpv3
*
* @param    none
* * @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
userMgrRestoreSnmpUsers(void);

/*********************************************************************
* @purpose  Pass all user config data to snmp for snmpv3
*
* @param    none
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
userMgrInitSnmpUsers(void);


/*********************************************************************
* @purpose  Private utility function to initialize all data structure fields
*
* @param    index       table index
*
* @returns  void
*
* @end
*********************************************************************/
void userMgrInitialize(L7_uint32 index );


/*********************************************************************
* @purpose  Notify interested third parties of changes to user db
*
* @param    category   type of change (USER, AUTH, or ENCRYPT)
* @param    index       table index of changed entry
* @param    changeType add, modify, delete
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes Eventually, listeners will register.  For now, listeners
*        are hard coded.
* @notes There is no handling for a failure of user login changes
*        communicated to cliWeb.  For this reason, there is the possibility of the
*        user databases becoming out of sync.
*
* @end
*********************************************************************/
L7_RC_t
userMgrNotifyListeners(L7_uint32 category, L7_uint32 index, L7_uint32 changeType );

/*********************************************************************
*
* @purpose convert all letters inside a buffer (char8) to lower case
*
* @param L7_char8 *buf
*
* @returns  void
*
* @notes   This f(x) returns the same letter in the same buffer but all
*          lower case, checking the buffer for empty string
* @end
*
*********************************************************************/
void userMgrConvertToLowerCase(L7_char8 *buf);

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
L7_BOOL userMgrCaseCompare(L7_char8 *buf1, L7_char8 *buf2 );

/*********************************************************************
* @purpose  Set password lockout counts fro all logins to Factory Defaults
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void userMgrConfigPasswordLockoutFactoryDefault(void);

/*********************************************************************
* @purpose Debug function to view contents of user port array
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void dumpUserMgrPortStruct(void);

/*********************************************************************
* @purpose  Reverse the given string
*
* @param    str (input/output) - String
*
* @returns  L7_SUCCESS on success 
*           L7_FAILURE on failure
*
* @notes    The function reverse the given string to str
*
* @end
*********************************************************************/
L7_RC_t userMgrStringReverse(L7_char8 *str);


/******************************************************************************
 * @purpose To compare two strings and check if the source string is present in
 *          the destination string
 *
 * @param   dstString - (input) Destination String is to compare for srcString
 *          srcString - (input) Source String is to be search for
 *
 * @returns L7_SUCCESS - if srcString is present in the dstString in any form
 *                       (case in-sensitive or reverse or substring) 
 *
 * @returns L7_FAILURE - if srcString is not present in the dstString
 *
 * @returns L7_ERROR - if src or dst string are NULL pointers (or)
 *                     src string length is greater than dst string length (or)
 *                     failed to Reverse the string
 *
 * @notes  
 * @end
 ******************************************************************************/
L7_RC_t userMgrStrNoCaseReverseCompareCheck(L7_char8 *dstString, L7_char8 *srcString);  

/******************************************************************************
 * @purpose To check for Repeated characters in the given string
 *
 * @param   string - (input) String 
 *          maxNum - (input) Maximum Number of repeated characters 
 *
 * @returns L7_SUCCESS - if string contains maxNum of repeated characters
 *
 * @returns L7_FAILURE - if string doesn't contain maxNum of repeated characters
 *
 * @returns L7_ERROR - if the given string is NULL pointer
 *
 * @notes  
 * @end
 ******************************************************************************/
L7_RC_t userMgrStrMaxNumRepeatedCharsCheck(L7_char8 *string, L7_ushort16 maxNum);

/******************************************************************************
 * @purpose To check for Consecutive characters in the given string
 *
 * @param   string - (input) String 
 *          maxNum - (input) Maximum Number of consecutive characters 
 *
 * @returns L7_SUCCESS - if string contains maxNum of consecutive characters
 *
 * @returns L7_FAILURE - if string doesn't contain maxNum of consecutive characters
 *
 * @returns L7_ERROR - if the given string is NULL pointer
 *
 * @notes  
 * @end
 ******************************************************************************/
L7_RC_t userMgrStrMaxNumConsecutiveCharsCheck(L7_char8 *string, L7_ushort16 maxNum);

/**************************************************************************
*
* @purpose  The function shall test whether the value is a printable string
*
* @param    string  @b{(input)}  String
*
* @returns  L7_SUCCESS if string is a printable character; 
*           L7_FAILURE otherwise.
*
* @end
*
*************************************************************************/
L7_RC_t userMgrStringPrintableCheck(L7_char8 *string);

/**************************************************************************
*
* @purpose  the function sort the strings in the given string list
*
* @param    strlist  @b{(input)}  string list
*           maxlen   #b{(input)}  max length of a string
*           count    #b{(input)}  number of strings in the given string list
*
* @returns  l7_success sort the list successfully
*           l7_failure if given strlist or maxlen or count is null
*
* @end
*
*************************************************************************/
L7_RC_t userMgrStringListSort(L7_char8 strlist[][L7_PASSWD_EXCLUDE_KEYWORD_MAX_LEN],
                              L7_uint32 maxLen, 
                              L7_uint32 count);

/*********************************************************************
 * @purpose  Validate restricted characters in the user password
 *
 * @param    password   @b{(input)} pointer to login user password
 * @param    encrypted  @b{(input)} indicates whether the password param 
 *                                  is in encrypted form
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE if password is invalid
 *           L7_USER_MGR_PASSWD_INVALID_QUOTATION_CHAR if password has 
 *                            quotation as a character
 *           L7_USER_MGR_PASSWD_INVALID_QUESTIONMARK_CHAR if password has
 *                            question mark as a character
 *
 * @notes    none
 * @end
 *********************************************************************/
L7_RC_t userMgrPasswdRestrictedCharactersCheck(L7_char8 *password, L7_BOOL encrypted);

#endif /* USER_MGR_UTIL_H */



