/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename login_sessions_api.h
*
* @purpose allow central location for current login session information
*
* @component user interface
*
* @comments none
*
* @create  04/10/2001
*
* @author  Forrest Samuels 
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef LOGIN_SESSIONS_H
#define LOGIN_SESSIONS_H

#include "cli_web_exports.h" 

typedef struct
{

  L7_char8  userName[L7_LOGIN_SIZE];
  L7_inet_addr_t ip;
  L7_uint32 unit;
  L7_uint32 slot;
  L7_uint32 port;
  L7_uint32 ifIndex;
  L7_uint32 sessionStartTime;
  L7_uint32 lastActivityTime;
  L7_uint32 obtSessionStartTime;
  L7_uint32 obtLastActivityTime;
  L7_uint32 sessionType;
  L7_BOOL   emptyIndex;
  L7_BOOL   resetConnection;
  void      *userStorage;
  L7_uint32 featureHiddenMode;

} loginSessionsInfo_t;



void loginSessionInit();

L7_uint32 loginSessionLogin();
L7_RC_t loginSessionLogout(L7_uint32 sessionIndex);

void obtLoginSessionStart(L7_uint32 sessionIndex);
void obtLoginSessionEnd(L7_uint32 sessionIndex);
void loginSessionUpdateObtActivityTime(L7_uint32 sessionIndex);

L7_BOOL loginSessionDoesLoginExist();

L7_char8 *loginSessionUserNameGet(L7_uint32 sessionIndex);
void loginSessionUserNameSet(L7_uint32 sessionIndex, L7_char8 *userName);

L7_inet_addr_t *loginSessionIPAddrGet(L7_uint32 sessionIndex);
void loginSessionIPAddrSet(L7_uint32 sessionIndex, L7_inet_addr_t *ip);

L7_uint32 loginSessionUnitGet(L7_uint32 sessionIndex);
L7_uint32 loginSessionSlotGet(L7_uint32 sessionIndex);
L7_uint32 loginSessionPortGet(L7_uint32 sessionIndex);
void loginSessionUSPSet(L7_uint32 sessionIndex, L7_uint32 u, L7_uint32 s, L7_uint32 p);

L7_uint32 loginSessionIfIndexGet(L7_uint32 sessionIndex);
void loginSessionIfIndexSet(L7_uint32 sessionIndex, L7_uint32 ifIndex);

void loginSessionUpdateActivityTime();
L7_uint32 loginSessionLoginTimeGet(L7_uint32 sessionIndex);
L7_uint32 loginSessionConnectionDurationGet(L7_uint32 sessionIndex);
L7_uint32 loginSessionLoginIdleTimeGet(L7_uint32 sessionIndex);

void *loginSessionUserStorageGet(L7_uint32 sessionIndex);
void loginSessionUserStorageSet(L7_uint32 sessionIndex, void *userPointer);

L7_uint32 loginSessionConnectionTypeGet(L7_uint32 sessionIndex);
void loginSessionConnectionTypeSet(L7_uint32 sessionIndex, L7_uint32 sessionType);

L7_BOOL loginSessionUserCurrentlyLoggedIn(L7_char8 *userName);

void loginSessionResetConnectionSet(L7_uint32 sessionIndex);
L7_BOOL loginSessionResetConnectionGet(L7_uint32 sessionIndex);
void loginSessionSupportFeatureHiddenModeSet(L7_int32 sessionIndex,L7_uint32 mode);
L7_uint32 loginSessionSupportFeatureHiddenModeGet(L7_int32 sessionIndex);

#endif /* LOGIN_SESSIONS_H */
