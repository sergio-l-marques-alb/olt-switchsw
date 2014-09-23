/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename session.h
*
* @purpose Code to support cookie based sessions
*
* @component 
*
* @comments tba
*
* @create 01/13/2005
*
* @author cpverne
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/

#ifndef SESSION_H
#define SESSION_H

#include "l7_common.h"

#define L7_SESSION_MAX_COUNT  FD_WEB_DEFAULT_MAX_CONNECTIONS  
/* Number of concurrent user sessions available 
   If this number is exceeded, then the user will 
   be denied access until the other entries timeout */
   
#define L7_SESSION_ID_LENGTH  75
/* Length of string used to identify the user */

#define L7_SESSION_SOFT_TIMEOUT_SCALE (60)       /* minutes */
#define L7_SESSION_HARD_TIMEOUT_SCALE (60*60)    /* hours */

/* Number of seconds to allow the user to remain logged into a session */
/* Soft limit is the number of seconds between accesses */
/* Hard limit is the total time that session can remain logged in (prevents automatic refreshes)*/

typedef struct EwaSession_s {
  L7_uint32            entryIndex;                  /* Session Index */
  L7_char8             sid[L7_SESSION_ID_LENGTH+1]; /* Session Id */
  L7_BOOL              sendCookie;                  /* Use this flag so we only send the cookie once */
  L7_uint32            type;                        /* Session Type ... L7_LOGIN_TYPE_HTTP or L7_LOGIN_TYPE_SSL */
  L7_inet_addr_t       inetAddr;                    /* IP Address of the original client */
  L7_uint32            last_access;                 /* SysUpTime when this user last performed a query */
  L7_uint32            first_access;                /* SysUpTime when this user first performed a query */
  L7_BOOL              active;                      /* Flag to indicate if this session is used or not */
  L7_char8             uname[L7_LOGIN_SIZE];
  L7_char8             pwd[L7_PASSWORD_SIZE];
  lvl7SecurityLevels_t access_level;                /* L7_LOGIN_ACCESS_NONE, L7_LOGIN_ACCESS_READ_ONLY, L7_LOGIN_ACCESS_READ_WRITE */
} EwaSession;


/**********************************************************************
* @purpose  Attempt to obtain the ewa session mutex
*
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    This is a blocking call. Caller must NOT hold the ewa
*           session mutex.
*
* @end
*********************************************************************/
L7_RC_t ewaSessionDataLock();

/**********************************************************************
* @purpose  Release the ewa session mutex
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    Caller must hold the ewa session mutex
*
* @end
*********************************************************************/
L7_RC_t ewaSessionDataUnlock();

void
debugEwaSessionTableDump(L7_uint32 activeOnly);

/*********************************************************************
*
* @purpose Initialize the session table
*          
* @param none
*
* @returns none
*
* @end
*
*********************************************************************/
void 
EwaSessionInit();

/*********************************************************************
*
* @purpose Scans the session table for sessions that have timed out and
*          removes them
*          
* @returns 
*
* @end
*
*********************************************************************/
void
EwaSessionTableTimeoutScan(void);

/*********************************************************************
*
* @purpose Performs a lookup in the session table for a matching session
*          
* @param sid        string containing SID cookie value
* @param ipAddress  IP address of the connecting client
*
* @returns EwaSession pointer
*
* @end
*
*********************************************************************/
EwaSession* 
EwaSessionLookup(char* sid, L7_uint32 sessionType, L7_inet_addr_t inetAddr, L7_BOOL createFlag);

/*********************************************************************
*
* @purpose Generates a random SID of a given length
*          
* @param sid     character pointer to string location
* @param length  length of string to generate
*
* @returns none
*
* @end
*
*********************************************************************/
void 
EwaSessionGenerateSid(L7_char8* sid, L7_uint32 length);

/*********************************************************************
*
* @purpose Performs timeout checks against hard and soft timeout values
*          
* @param first_access  sysUpTime that the session first connected
* @param last_access   sysUpTime that the session last connected
* @param sessionType   L7_LOGIN_TYPE_HTTPS or L7_LOGIN_TYPE_HTTP
*
* @returns L7_TRUE   if session has timed out
* @returns L7_FALSE  if session has not yet timed out
*
* @notes  Timeout determined by the delta between first_access and 
*         current time being larger than L7_SESSION_HARD_TIMEOUT or
*         the delta between last_access and current time being larger
*         than L7_SESSION_SOFT_TIMEOUT
*
* @end
*
*********************************************************************/
L7_BOOL
EwaSessionTimeoutTest(L7_uint32 first_access, L7_uint32 last_access, L7_uint32 sessionType);

/*********************************************************************
*
* @purpose Performs a lookup in the session table for the specified session
*          
* @param sIndex    session Index 
*
* @returns EwaSession pointer  and L7_SUCCESS or L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t 
EwaSessionGet(L7_uint32 sIndex, EwaSession *session);

/*********************************************************************
*
* @purpose Performs a lookup in the session table for the specified session
*          
* @param sIndex    session Index 
*
* @returns L7_TRUE or L7_FALSE
*
* @end
*
*********************************************************************/
L7_BOOL EwaSessionIsActive(L7_uint32 sIndex);

/*********************************************************************
*
* @purpose Performs a lookup in the session table for the next session
*          
* @param sIndex    session Index to start looking for next session
*
* @returns EwaSession pointer  and L7_SUCCESS or L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t 
EwaSessionNextGet(L7_uint32 *sIndex, EwaSession *session);

/*********************************************************************
*
* @purpose Performs a lookup in the session table for the first session
*          
* @param sIndex    session Index to start looking for next session
*
* @returns EwaSession pointer  and L7_SUCCESS or L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t 
EwaSessionFirstGet(L7_uint32 *sIndex, EwaSession *session);

/*********************************************************************
*
* @purpose Removes the session identified by session id
*          
* @param sid    string containing SID cookie value
*
* @returns nothing
*
* @end
*
*********************************************************************/
void EwaSessionRemove(char* sid);

/*********************************************************************
*
* @purpose Remove active sessions specified by sessionType
*          
* @param sessionType Where 0 Removes All Sessions,
*                    L7_LOGIN_TYPE_HTTPS removes all Secure HTTP Sessions, and
*                    L7_LOGIN_TYPE_HTTP removes all un-secure HTTP Sessions
*
* @end
*
*********************************************************************/
void EwaSessionRemoveAll(L7_uint32 sessionType);

/*********************************************************************
*
* @purpose Remove the session indicated by the supplied index
*          
* @end
*
*********************************************************************/
void EwaSessionRemoveByIndex(L7_uint32 sessionIndex);

/*********************************************************************
*
* @purpose Return the number of active sessions
*          
* @returns 
*
* @end
*
*********************************************************************/
L7_uint32 EwaSessionNumActiveGet(void);

/*********************************************************************
*
* @purpose Return the access level for the specified session
*          
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t EwaSessionAccessLevelGet(char *sid, lvl7SecurityLevels_t *access_level);


/*********************************************************************
*
* @purpose Get the idle time for the specified session id
*          
* @param sIndex    session Index 
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t EwaSessionIdleTimeGet(L7_uint32 sIndex, L7_uint32 *idleTime);

/*********************************************************************
*
* @purpose Get the up time for the specified session id
*          
* @param sIndex    session Index 
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t EwaSessionUpTimeGet(L7_uint32 sIndex, L7_uint32 *upTime);

/*********************************************************************
*
* @purpose Get the type for the specified session id
*          
* @param sIndex    session Index 
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t EwaSessionTypeGet(L7_uint32 sIndex, L7_uint32 *sType);

/*********************************************************************
*
* @purpose Get the user for the specified session id
*          
* @param sIndex    session Index 
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t EwaSessionUserGet(L7_uint32 sIndex, L7_char8 *user);

/*********************************************************************
*
* @purpose Get the inet address for the specified session id
*          
* @param sIndex    session Index 
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t EwaSessionInetAddrGet(L7_uint32 sIndex, L7_inet_addr_t *inetAddr);

#endif /* SESSION_H */
