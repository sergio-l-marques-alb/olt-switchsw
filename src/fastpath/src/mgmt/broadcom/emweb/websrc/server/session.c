/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename session.c
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
#include <string.h>
#include <stdlib.h>
#include "ews.h"
#include "l7_common.h"
#include "cli_web_exports.h"
#include "usmdb_sim_api.h"
#include "usmdb_sslt_api.h"
#include "usmdb_util_api.h"

#include "osapi.h"
#include "cli_web_mgr_api.h"
#include "osapi_support.h"
#include "session.h"


/* Our mutex to protect access to shared data structures. */
static void * ewaSessionDataSemaphore = L7_NULL;

void EwaDisconnectLogMsg(L7_uint32 sessionId);

EwaSession session_table[L7_SESSION_MAX_COUNT];


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

L7_RC_t ewaSessionDataLock()
{
  return osapiSemaTake(ewaSessionDataSemaphore, L7_WAIT_FOREVER);
}

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

L7_RC_t ewaSessionDataUnlock()
{
  return osapiSemaGive(ewaSessionDataSemaphore);
}


/**********************************************************************
* @purpose  Call L7_LOGF for the session disconnect
*
* @param    sessionId
*
* @returns  Nothing
*
* @notes
*
* @end
*********************************************************************/
void EwaDisconnectLogMsg(L7_uint32 sessionId)
{
  L7_char8  buf[128];
  L7_char8  ipAddr[128];

  if (session_table[sessionId].type == L7_LOGIN_TYPE_HTTPS)
  {
    osapiStrncpy(buf, "HTTPS", sizeof(buf));
  }
  else
  {
    osapiStrncpy(buf, "HTTP", sizeof(buf));
  }
  if (session_table[sessionId].inetAddr.family == L7_AF_INET)
  {
     if (usmDbInetNtoa(osapiNtohl(session_table[sessionId].inetAddr.addr.ipv4.s_addr), ipAddr) != L7_SUCCESS)
     {
       osapiStrncpy(ipAddr, "Unknown", sizeof(ipAddr));
     }
  }
  else /* L7_AF_INET6 */
  {
     osapiInetNtop(L7_AF_INET6,(L7_uchar8 *)&session_table[sessionId].inetAddr.addr.ipv6, ipAddr, sizeof(ipAddr));
  }
  L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
          "%s Session %d ended for user %s connected from %s\n",
          buf, (sessionId+FD_CLI_DEFAULT_MAX_CONNECTIONS), session_table[sessionId].uname, ipAddr);
}


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
EwaSessionInit()
{
  /* Zero out table */
  memset((char*)session_table, 0, sizeof(session_table));

  /** semaphore creation for task protection over the common data. */
  ewaSessionDataSemaphore = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (ewaSessionDataSemaphore == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
            "EwaSessionInit %s(%d): unable to create semaphore!\n",
        __FILE__, __LINE__);
    return;
  }

}



/*********************************************************************
*
* @purpose Dump the session table
*
* @param activeOnly 1-active entries, 0-all entries
*
* @returns none
*
* @end
*
*********************************************************************/
void
debugEwaSessionTableDump(L7_uint32 activeOnly)
{
  L7_uint32 i;
  L7_char8  ipAddr[128];
  L7_uint32 val;
  L7_uint32 hrs;
  L7_uint32 min;
  L7_uint32 sec;
  L7_uint32 current_time;
  L7_uint32 timeout_hard = 0;
  L7_uint32 timeout_soft = 0;
  L7_uint32 http_hard_t = 0;
  L7_uint32 http_soft_t = 0;
  L7_uint32 ssl_hard_t = 0;
  L7_uint32 ssl_soft_t = 0;
  L7_char8  buf[64];

  sysapiPrintf("EmWeb Session Dump\n\n");

#ifdef L7_MGMT_SECURITY_PACKAGE
  if (usmDbssltSessionSoftTimeOutGet(&ssl_soft_t) != L7_SUCCESS)
  {
    sysapiPrintf("ERROR: usmDbssltSessionSoftTimeOutGet() failure\n");
    ssl_soft_t = FD_SECURE_HTTP_SESSION_SOFT_TIMEOUT_DEFAULT;
  }
  ssl_soft_t *= L7_SESSION_SOFT_TIMEOUT_SCALE;

  if (usmDbssltSessionHardTimeOutGet(&ssl_hard_t) != L7_SUCCESS)
  {
    sysapiPrintf("ERROR: usmDbssltSessionHardTimeOutGet() failure\n");
    ssl_hard_t = FD_SECURE_HTTP_SESSION_HARD_TIMEOUT_DEFAULT;
  }
  ssl_hard_t *= L7_SESSION_HARD_TIMEOUT_SCALE;
#endif

  if (usmDbCliWebHttpSessionSoftTimeOutGet(&http_soft_t) != L7_SUCCESS)
  {
    sysapiPrintf("ERROR: usmDbCliWebHttpSessionSoftTimeOutGet() failure\n");
    http_soft_t = FD_HTTP_SESSION_SOFT_TIMEOUT_DEFAULT;
  }
  http_soft_t *= L7_SESSION_SOFT_TIMEOUT_SCALE;

  if (usmDbCliWebHttpSessionHardTimeOutGet(&http_hard_t) != L7_SUCCESS)
  {
    sysapiPrintf("ERROR: usmDbCliWebHttpSessionHardTimeOutGet() failure\n");
    http_hard_t = FD_HTTP_SESSION_HARD_TIMEOUT_DEFAULT;
  }
  http_hard_t *= L7_SESSION_HARD_TIMEOUT_SCALE;

  for (i = 0; i < L7_SESSION_MAX_COUNT; i++)
  {
    if ((activeOnly == 0) || (session_table[i].active == L7_TRUE))
    {
      current_time = osapiUpTimeRaw();
      if (session_table[i].inetAddr.family == L7_AF_INET)
      {
         if (usmDbInetNtoa(osapiNtohl(session_table[i].inetAddr.addr.ipv4.s_addr), ipAddr) != L7_SUCCESS)
         {
           strcpy(ipAddr, "Error");
         }
      }
      else /* L7_AF_INET6 */
      {
         osapiInetNtop(L7_AF_INET6,(L7_uchar8 *)&session_table[i].inetAddr.addr.ipv6, ipAddr, sizeof(ipAddr));
      }
      sysapiPrintf("\n");
      sysapiPrintf("session[%d].entryIndex   = %d\n", i, session_table[i].entryIndex);
      sysapiPrintf("session[%d].active       = %d\n", i, session_table[i].active);
      sysapiPrintf("session[%d].inetAddr     = %s\n", i, ipAddr);
      if (session_table[i].type == L7_LOGIN_TYPE_HTTPS)
      {
        sysapiPrintf("session[%d].type         = Secure HTTP\n", i);
      }
      else
      {
        sysapiPrintf("session[%d].type         = HTTP\n", i);
      }
      sysapiPrintf("session[%d].sid          = %s\n", i, session_table[i].sid);
      if (session_table[i].sendCookie == L7_TRUE)
      {
        sysapiPrintf("session[%d].sendCookie   = TRUE\n", i);
      }
      else
      {
        sysapiPrintf("session[%d].sendCookie   = FALSE\n", i);
      }
      sysapiPrintf("session[%d].uname        = %s\n", i, session_table[i].uname);

      switch (session_table[i].access_level)
      {
        case L7_LOGIN_ACCESS_NONE:
          strcpy(buf, "L7_LOGIN_ACCESS_NONE");
          break;
        case L7_LOGIN_ACCESS_READ_ONLY:
          strcpy(buf, "L7_LOGIN_ACCESS_READ_ONLY");
          break;
        case L7_LOGIN_ACCESS_READ_WRITE:
          strcpy(buf, "L7_LOGIN_ACCESS_READ_WRITE");
          break;
        default:
          strcpy(buf, "Unknown");
          break;

      }
      sysapiPrintf("session[%d].access_level = %d - %s\n", i, session_table[i].access_level, buf);

      sysapiPrintf("session[%d].first_access = %d\n", i, session_table[i].first_access);
      sysapiPrintf("session[%d].last_access  = %d\n", i, session_table[i].last_access);

      sec = (current_time - session_table[i].last_access) % 60;
      min = ((current_time - session_table[i].last_access) - sec) / 60;
      sysapiPrintf("session[%d]              = last access was %d minutes and %d seconds ago\n", i, min, sec);

      if (session_table[i].type == L7_LOGIN_TYPE_HTTPS)
      {
        timeout_hard = ssl_hard_t;
        timeout_soft = ssl_soft_t;
      }
      else
      {
        timeout_hard = http_hard_t;
        timeout_soft = http_soft_t;
      }
      if (timeout_hard != 0)
      {
        if ((current_time - session_table[i].first_access) >= timeout_hard)
        {
          sysapiPrintf("session[%d]              = hard time-out has occurred\n");
        }
        else
        {
          sec = (timeout_hard - (current_time - session_table[i].first_access)) % 60;
          val = ((timeout_hard - (current_time - session_table[i].first_access)) - sec) / 60;
          min = (val % 60);
          hrs = (val-min) / 60;
          sysapiPrintf("session[%d]              = hard time out in %d hours %d minutes and %d seconds\n", i, hrs, min, sec);
        }
      }
      else
      {
        sysapiPrintf("session[%d]              = hard time out is infinite\n", i);
      }
      if (timeout_soft != 0)
      {
        if ((current_time - session_table[i].last_access) >= timeout_soft)
        {
          sysapiPrintf("session[%d]              = soft time-out has occurred\n");
        }
        else
        {
          sec = (timeout_soft - (current_time - session_table[i].last_access)) % 60;
          min = ((timeout_soft - (current_time - session_table[i].last_access)) - sec) / 60;
          sysapiPrintf("session[%d]              = soft time out in %d minutes and %d seconds\n", i, min, sec);
        }
      }
      else
      {
        sysapiPrintf("session[%d]              = soft time out is infinite\n", i);
      }
    }
  }
  sysapiPrintf("\n");
}


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
EwaSessionTableTimeoutScan(void)
{
  L7_uint32 i;

  if (ewaSessionDataLock() == L7_SUCCESS)
  {
    for (i = 0; i < L7_SESSION_MAX_COUNT; i++)
    {
      if (session_table[i].active == L7_TRUE)
      {
        /* check to see if this session has timed out */
        if (EwaSessionTimeoutTest(session_table[i].first_access, session_table[i].last_access, session_table[i].type) == L7_TRUE)
        {
          /* Session has timed out, clear this entry */
          EwaDisconnectLogMsg(i);
          session_table[i].active = L7_FALSE;
          session_table[i].entryIndex = 0;
          memset(&session_table[i].inetAddr, 0x00, sizeof(L7_inet_addr_t));
          session_table[i].first_access = 0;
          session_table[i].last_access = 0;
          session_table[i].type = 0;
          memset(&session_table[i].sid, 0x00, L7_SESSION_ID_LENGTH+1);
          session_table[i].sendCookie = L7_FALSE;
          memset(&session_table[i].uname, 0x00, L7_LOGIN_SIZE);
          memset(&session_table[i].pwd, 0x00, L7_PASSWORD_SIZE);
        }
      }
    }
    if (ewaSessionDataUnlock() != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
              "EwaSessionTableTimeoutScan %s(%d): unable to release semaphore!\n",
          __FILE__, __LINE__);
    }
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
            "EwaSessionTableTimeoutScan %s(%d): unable to obtain semaphore!\n",
        __FILE__, __LINE__);
  }
}


/*********************************************************************
*
* @purpose Performs a lookup in the session table for a matching session
*
* @param sid    string containing SID cookie value
*
* @returns EwaSession pointer
*
* @end
*
*********************************************************************/
EwaSession*
EwaSessionLookup(char* sid, L7_uint32 sessionType, L7_inet_addr_t inetAddr, L7_BOOL createFlag)
{
  L7_uint32 i;
  L7_int32  nextavailable = -1;
  L7_uint32 sslSessionCount;
  L7_uint32 sslSessionsMax;
  L7_uint32 httpSessionCount;
  L7_uint32 httpSessionsMax;

  if (ewaSessionDataLock() != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
            "EwaSessionLookup %s(%d): unable to obtain semaphore!\n",
        __FILE__, __LINE__);
    return NULL;
  }

  httpSessionCount = 0;
  if (usmDbCliWebHttpNumSessionsGet(&httpSessionsMax) != L7_SUCCESS)
  {
    httpSessionsMax = 0;
  }

  sslSessionCount = 0;
  #ifdef L7_MGMT_SECURITY_PACKAGE
    if (usmDbssltNumSessionsGet(&sslSessionsMax) != L7_SUCCESS)
    {
      sslSessionsMax = 0;
    }
  #else
    sslSessionsMax = 0;
  #endif


  for (i = 0; i < L7_SESSION_MAX_COUNT; i++)
  {
    if (session_table[i].active == L7_TRUE)
    {
      /* check to see if this session has timed out */
      if (EwaSessionTimeoutTest(session_table[i].first_access, session_table[i].last_access, session_table[i].type) == L7_TRUE)
      {
        /* Session has timed out, clear this entry */
        EwaDisconnectLogMsg(i);
        session_table[i].active = L7_FALSE;
        session_table[i].entryIndex = 0;
        memset(&session_table[i].inetAddr, 0x00, sizeof(L7_inet_addr_t));
        session_table[i].first_access = 0;
        session_table[i].last_access = 0;
        session_table[i].type = 0;
        memset(&session_table[i].sid, 0x00, L7_SESSION_ID_LENGTH+1);
        session_table[i].sendCookie = L7_FALSE;
        memset(&session_table[i].uname, 0x00, L7_LOGIN_SIZE);
        memset(&session_table[i].pwd, 0x00, L7_PASSWORD_SIZE);
        if (nextavailable == -1)
          nextavailable = i;
      }
      else
      {
        if (session_table[i].type == L7_LOGIN_TYPE_HTTPS)
        {
          sslSessionCount++;
        }
        else /* L7_LOGIN_TYPE_HTTP */
        {
          httpSessionCount++;
        }
        if (sid != NULL)
        {
          /* check to see if this is the requested session */
          if ((strlen(session_table[i].sid) == strlen(sid)) &&
              (strcmp(session_table[i].sid, sid) == 0))
          {
            /* update last access to now and return */
            session_table[i].last_access = osapiUpTimeRaw();
            if (ewaSessionDataUnlock() != L7_SUCCESS)
            {
              L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
                      "EwaSessionLookup %s(%d): unable to release semaphore!\n",
                  __FILE__, __LINE__);
            }
            return &session_table[i];
          }
        }
      }
    }
    else
    {
      if (nextavailable == -1)
        nextavailable = i;
    }
  }

  if ((nextavailable >= 0) && (createFlag == L7_TRUE))
  {
    if ((sessionType == L7_LOGIN_TYPE_HTTPS) && (sslSessionCount >= sslSessionsMax))
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
              "EwaSessionLookup %s(%d): HTTPS session denied.  Maximum sessions reached.\n",
              __FILE__, __LINE__);
      nextavailable = -1;
    }
    if ((sessionType == L7_LOGIN_TYPE_HTTP) && (httpSessionCount >= httpSessionsMax))
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
              "EwaSessionLookup %s(%d): HTTP session denied.  Maximum sessions reached.\n",
              __FILE__, __LINE__);
      nextavailable = -1;
    }

    if (nextavailable >= 0)
    {
      /* fill in initial values */
      session_table[nextavailable].active = L7_TRUE;
      session_table[nextavailable].entryIndex = nextavailable;
      memcpy(&session_table[nextavailable].inetAddr, &inetAddr, sizeof(L7_inet_addr_t));
      session_table[nextavailable].first_access = osapiUpTimeRaw();
      session_table[nextavailable].last_access = session_table[nextavailable].first_access;
      session_table[nextavailable].type = sessionType;
      EwaSessionGenerateSid(session_table[nextavailable].sid, L7_SESSION_ID_LENGTH);
      session_table[nextavailable].sendCookie = L7_TRUE;
    }

    if (ewaSessionDataUnlock() != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
              "EwaSessionLookup %s(%d): unable to release semaphore!\n",
          __FILE__, __LINE__);
    }
    if (nextavailable >= 0)
    {
      return &session_table[nextavailable];
    }
    else
    {
      return NULL;
    }
  }

  if (ewaSessionDataUnlock() != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
            "EwaSessionLookup %s(%d): unable to release semaphore!\n",
        __FILE__, __LINE__);
  }
  return NULL;
}

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
EwaSessionGenerateSid(L7_char8* sid, L7_uint32 length)
{
  L7_uint32 i;
  L7_uint32 seed = osapiUpTimeRaw();
  L7_uint32 char_val;

  srand(seed);

  for (i = 0; i < length; i++)
  {
    char_val = rand();
    /* get a random character between a-z,A-Z */
    char_val %= 52;
    if (char_val >= 26)
    {
      char_val -= 26;
      char_val += ('A');
    }
    else
    {
      char_val += ('a');
    }

    sid[i] = char_val;
  }
}

/*********************************************************************
*
* @purpose Performs timeout checks against hard and soft timeout values
*
* @param first_access  sysUpTime that the session first connected
* @param last_access   sysUpTime that the session last connected
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
EwaSessionTimeoutTest(L7_uint32 first_access, L7_uint32 last_access, L7_uint32 sessionType)
{
  L7_uint32 current_time;
  L7_uint32 sessionHardTimeOut;
  L7_uint32 sessionSoftTimeOut;

  if (sessionType == L7_LOGIN_TYPE_HTTPS)
  {
    #ifdef L7_MGMT_SECURITY_PACKAGE
    if (usmDbssltSessionHardTimeOutGet(&sessionHardTimeOut) != L7_SUCCESS)
    {
      sessionHardTimeOut = FD_SECURE_HTTP_SESSION_HARD_TIMEOUT_MIN;
    }
    if (usmDbssltSessionSoftTimeOutGet(&sessionSoftTimeOut) != L7_SUCCESS)
    {
      sessionSoftTimeOut = FD_SECURE_HTTP_SESSION_SOFT_TIMEOUT_MIN;
    }
    #else
    sessionSoftTimeOut = FD_SECURE_HTTP_SESSION_SOFT_TIMEOUT_MIN;
    sessionHardTimeOut = FD_SECURE_HTTP_SESSION_HARD_TIMEOUT_MIN;
    #endif
  }
  else /* L7_LOGIN_TYPE_HTTP */
  {
    if (usmDbCliWebHttpSessionSoftTimeOutGet(&sessionSoftTimeOut) != L7_SUCCESS)
    {
      sessionSoftTimeOut = FD_HTTP_SESSION_SOFT_TIMEOUT_DEFAULT;
    }
    if (usmDbCliWebHttpSessionHardTimeOutGet(&sessionHardTimeOut) != L7_SUCCESS)
    {
      sessionHardTimeOut = FD_HTTP_SESSION_SOFT_TIMEOUT_DEFAULT;
    }
  }

  if ((sessionSoftTimeOut == 0) && (sessionHardTimeOut == 0))  /* 0 == Never time out */
  {
    return L7_FALSE;
  }

  sessionSoftTimeOut *= L7_SESSION_SOFT_TIMEOUT_SCALE;
  sessionHardTimeOut *= L7_SESSION_HARD_TIMEOUT_SCALE;

  current_time = osapiUpTimeRaw();

  if ((sessionHardTimeOut != 0) && (current_time > (first_access + sessionHardTimeOut)))
  {
    return L7_TRUE;
  }

  if ((sessionSoftTimeOut != 0) && (current_time > (last_access + sessionSoftTimeOut)))
  {
    return L7_TRUE;
  }

  return L7_FALSE;
}

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
L7_RC_t EwaSessionGet(L7_uint32 sIndex, EwaSession *session)
{
  L7_RC_t   rc = L7_FAILURE;

  if ((sIndex < 0) || (sIndex >= L7_SESSION_MAX_COUNT))
  {
    return L7_FAILURE;
  }

  if (session_table[sIndex].active == L7_TRUE)
  {
    rc = L7_SUCCESS;
    memcpy(session, &session_table[sIndex], sizeof(EwaSession));
  }

  return rc;
}

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
L7_BOOL EwaSessionIsActive(L7_uint32 sIndex)
{
  if ((sIndex < 0) || (sIndex >= L7_SESSION_MAX_COUNT))
  {
    return L7_FALSE;
  }
  else
  {
    return session_table[sIndex].active;
  }
}


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
L7_RC_t EwaSessionNextGet(L7_uint32 *sIndex, EwaSession *session)
{
  L7_uint32 i;
  L7_RC_t   rc = L7_FAILURE;

  for (i = *sIndex+1; i < L7_SESSION_MAX_COUNT; i++)
  {
    if (session_table[i].active == L7_TRUE)
    {
      rc = L7_SUCCESS;
      *sIndex = i;
      memcpy(session, &session_table[i], sizeof(EwaSession));
      break;
    }
  }

  return rc;
}

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
L7_RC_t EwaSessionFirstGet(L7_uint32 *sIndex, EwaSession *session)
{
  L7_uint32 i;
  L7_RC_t   rc = L7_FAILURE;

  for (i = 0; i < L7_SESSION_MAX_COUNT; i++)
  {
    if (session_table[i].active == L7_TRUE)
    {
      rc = L7_SUCCESS;
      *sIndex = i;
      memcpy(session, &session_table[i], sizeof(EwaSession));
      break;
    }
  }

  return rc;
}


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
void EwaSessionRemove(char* sid)
{
  L7_uint32 i;

  if (sid == NULL)
  {
    return;
  }


  if (ewaSessionDataLock() != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
            "EwaSessionRemove %s(%d): unable to obtain semaphore!\n",
        __FILE__, __LINE__);
    return;
  }

  for (i = 0; i < L7_SESSION_MAX_COUNT; i++)
  {
    if (session_table[i].active == L7_TRUE)
    {
      if ((strlen(session_table[i].sid) == strlen(sid)) &&
          (strcmp(session_table[i].sid, sid) == 0))     /* clear this entry */
      {
        EwaDisconnectLogMsg(i);
        session_table[i].active = L7_FALSE;
        session_table[i].entryIndex = 0;
        memset(&session_table[i].inetAddr, 0x00, sizeof(L7_inet_addr_t));
        session_table[i].first_access = 0;
        session_table[i].last_access = 0;
        session_table[i].type = 0;
        memset(&session_table[i].sid, 0x00, L7_SESSION_ID_LENGTH+1);
        session_table[i].sendCookie = L7_FALSE;
        memset(&session_table[i].uname, 0x00, L7_LOGIN_SIZE);
        memset(&session_table[i].pwd, 0x00, L7_PASSWORD_SIZE);
        break;
      }
    }
  }

  if (ewaSessionDataUnlock() != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
            "EwaSessionRemove %s(%d): unable to release semaphore!\n",
        __FILE__, __LINE__);
  }

  return;
}

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
void EwaSessionRemoveAll(L7_uint32 sessionType)
{
  L7_uint32 i;

  for (i = 0; i < L7_SESSION_MAX_COUNT; i++)
  {
    if (session_table[i].active == L7_TRUE)
    {
      if ((sessionType == 0) || (session_table[i].type == sessionType))
      {
        EwaSessionRemove(session_table[i].sid);
      }
    }
  }
}

/*********************************************************************
*
* @purpose Remove the session indicated by the supplied index
*
* @end
*
*********************************************************************/
void EwaSessionRemoveByIndex(L7_uint32 sessionIndex)
{
  if (session_table[sessionIndex].active == L7_TRUE)
  {
    EwaSessionRemove(session_table[sessionIndex].sid);
  }
}


/*********************************************************************
*
* @purpose Return the number of active sessions
*
* @returns
*
* @end
*
*********************************************************************/
L7_uint32
EwaSessionNumActiveGet(void)
{
  L7_uint32 i;
  L7_uint32 numSessions = 0;

  if (ewaSessionDataLock() == L7_SUCCESS)
  {
    for (i = 0; i < L7_SESSION_MAX_COUNT; i++)
    {
      if (session_table[i].active == L7_TRUE)
      {
        numSessions++;
      }
    }
    if (ewaSessionDataUnlock() != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
              "EwaSessionNumActiveGet %s(%d): unable to release semaphore!\n",
          __FILE__, __LINE__);
    }
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
            "EwaSessionNumActiveGet %s(%d): unable to obtain semaphore!\n",
        __FILE__, __LINE__);
  }
  return numSessions;
}


/*********************************************************************
*
* @purpose Return the access level for the specified session
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t EwaSessionAccessLevelGet(char *sid, lvl7SecurityLevels_t *access_level)
{
  L7_uint32  i;
  L7_RC_t    rc = L7_FAILURE;

  for (i = 0; i < L7_SESSION_MAX_COUNT; i++)
  {
    if (session_table[i].active == L7_TRUE)
    {
      if ((strlen(session_table[i].sid) == strlen(sid)) &&
          (strcmp(session_table[i].sid, sid) == 0))     /* clear this entry */
      {
        *access_level = session_table[i].access_level;
        rc = L7_SUCCESS;
        break;
      }
    }
  }

  return rc;
}

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
L7_RC_t EwaSessionIdleTimeGet(L7_uint32 sIndex, L7_uint32 *idleTime)
{
  L7_RC_t rc = L7_FAILURE;

  if ((sIndex >= 0) && (sIndex < L7_SESSION_MAX_COUNT))
  {
    *idleTime = osapiUpTimeRaw() - session_table[sIndex].last_access;
    rc = L7_SUCCESS;
  }
  return rc;
}

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
L7_RC_t EwaSessionUpTimeGet(L7_uint32 sIndex, L7_uint32 *upTime)
{
  L7_RC_t rc = L7_FAILURE;

  if ((sIndex >= 0) && (sIndex < L7_SESSION_MAX_COUNT))
  {
    *upTime = osapiUpTimeRaw() - session_table[sIndex].first_access;
    rc = L7_SUCCESS;
  }
  return rc;
}

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
L7_RC_t EwaSessionTypeGet(L7_uint32 sIndex, L7_uint32 *sType)
{
  L7_RC_t rc = L7_FAILURE;

  if ((sIndex >= 0) && (sIndex < L7_SESSION_MAX_COUNT))
  {
    *sType = session_table[sIndex].type;
    rc = L7_SUCCESS;
  }
  return rc;
}

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
L7_RC_t EwaSessionUserGet(L7_uint32 sIndex, L7_char8 *user)
{
  L7_RC_t rc = L7_FAILURE;

  if ((sIndex >= 0) && (sIndex < L7_SESSION_MAX_COUNT))
  {
    strcpy(user, session_table[sIndex].uname);
    rc = L7_SUCCESS;
  }
  return rc;
}

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
L7_RC_t EwaSessionInetAddrGet(L7_uint32 sIndex, L7_inet_addr_t *inetAddr)
{
  L7_RC_t rc = L7_FAILURE;

  if ((sIndex >= 0) && (sIndex < L7_SESSION_MAX_COUNT))
  {
    *inetAddr = session_table[sIndex].inetAddr;
    rc = L7_SUCCESS;
  }
  return rc;
}

