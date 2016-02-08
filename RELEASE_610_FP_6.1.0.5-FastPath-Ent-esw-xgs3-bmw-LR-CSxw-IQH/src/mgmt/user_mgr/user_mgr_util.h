
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

#endif /* USER_MGR_UTIL_H */



