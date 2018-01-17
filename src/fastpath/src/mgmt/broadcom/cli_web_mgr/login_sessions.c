/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename login_sessions.c
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

#include "cliapi.h"
#include "l7_common.h"
#include "cli_web_exports.h"
#include "osapi.h"
#include "defaultconfig.h"
#include "login_sessions_api.h"
#include "trapapi.h"
#include "log.h"
#include <stdio.h>
#include "usmdb_log_api.h"
#include "sysapi.h"
#include "cli_web_mgr_api.h"

extern void cliInitConnection(L7_uint32 handle);
extern L7_uint32 cliCurrentHandleGet();


loginSessionsInfo_t loginInfo[FD_CLI_DEFAULT_MAX_CONNECTIONS];
/*created array for storing loggining information */

L7_uint32 conTelnet = 0;
L7_uint32 conSSH    = 0;



void *loginSessionSemaphore;

void loginSessionInit()
{
  L7_uint32 i;

  loginSessionSemaphore = osapiSemaMCreate(OSAPI_SEM_Q_FIFO);

  if (loginSessionSemaphore == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_CLI_WEB_COMPONENT_ID,
            "Login Session semaphore not created.\n");
  }

  for (i = 0; i < FD_CLI_DEFAULT_MAX_CONNECTIONS; i++)
  {
    memset(loginInfo[i].userName, 0, L7_LOGIN_SIZE);
    sprintf(loginInfo[i].userName, "%s", "NONE");
    memset(&loginInfo[i].ip,0,sizeof(L7_inet_addr_t));
    loginInfo[i].unit = 0;
    loginInfo[i].slot = 0;
    loginInfo[i].port = 0;
    loginInfo[i].ifIndex = 0;
    loginInfo[i].sessionStartTime = 0;
    loginInfo[i].lastActivityTime = 0;
    loginInfo[i].sessionType = L7_LOGIN_TYPE_UNKNWN;
    loginInfo[i].emptyIndex = L7_TRUE;
    loginInfo[i].resetConnection = L7_FALSE;
    loginInfo[i].userStorage = L7_NULL;
    loginInfo[i].obtSessionStartTime = 0;
    loginInfo[i].obtLastActivityTime = 0;
    loginInfo[i].featureHiddenMode = L7_DISABLE;
  }
}


L7_uint32 loginSessionLogin()
{
  L7_uint32 index;

  /*
  Linux port 1964
  Move osapiSemaTake outside for loop; otherwise it can get taken
  several times with only one osapiSemaGive -- under Linux this
  results in a deadlock.  While this behavior is allowed for a
  Mutex semaphore under VxWorks, it doesn't seem to be doing the
  right thing here.
  */

  osapiSemaTake (loginSessionSemaphore, L7_WAIT_FOREVER);

  for (index = 0; index < FD_CLI_DEFAULT_MAX_CONNECTIONS; index++)
  {
    if (loginInfo[index].emptyIndex == L7_TRUE)
    {
      loginInfo[index].sessionStartTime = osapiUpTimeRaw();
      loginInfo[index].lastActivityTime = osapiUpTimeRaw();
      loginInfo[index].obtSessionStartTime = 0;
      loginInfo[index].obtLastActivityTime = 0;
      loginInfo[index].resetConnection = L7_FALSE;
      loginInfo[index].emptyIndex = L7_FALSE;
      osapiSemaGive (loginSessionSemaphore);
      return index;
    }
  }
  osapiSemaGive (loginSessionSemaphore);

  return FD_CLI_DEFAULT_MAX_CONNECTIONS + 1;

}

L7_RC_t loginSessionLogout(L7_uint32 sessionIndex)
{

  if ((sessionIndex >= 0) && (sessionIndex < FD_CLI_DEFAULT_MAX_CONNECTIONS))
  {
    cliCommon [sessionIndex].debugDisplayCtl.consoleTraceDisplayEnabled = L7_FALSE;
    cliCommon [sessionIndex].debugDisplayCtl.supportConsoleDisplayEnabled = L7_FALSE;
    cliCommon [sessionIndex].debugDisplayCtl.debugConsoleDisplayEnabled = L7_FALSE;
    cliWebSupportConsoleDisplayModeSet (L7_FALSE, sessionIndex);
    loginSessionSupportFeatureHiddenModeSet(sessionIndex,L7_DISABLE);

    memset(loginInfo[sessionIndex].userName, 0, L7_LOGIN_SIZE);
    sprintf(loginInfo[sessionIndex].userName, "%s", "NONE");
    memset(&loginInfo[sessionIndex].ip,0,sizeof(L7_inet_addr_t));
    loginInfo[sessionIndex].unit = 0;
    loginInfo[sessionIndex].slot = 0;
    loginInfo[sessionIndex].port = 0;
    loginInfo[sessionIndex].ifIndex = 0;
    loginInfo[sessionIndex].sessionStartTime = 0;
    loginInfo[sessionIndex].lastActivityTime = 0;
    loginInfo[sessionIndex].obtSessionStartTime = 0;
    loginInfo[sessionIndex].obtLastActivityTime = 0;
    loginInfo[sessionIndex].resetConnection = L7_FALSE;
    /*SSH TELNET*/
        if (loginInfo[sessionIndex].sessionType == L7_LOGIN_TYPE_TELNET)
            conTelnet--;
        else
            if (loginInfo[sessionIndex].sessionType == L7_LOGIN_TYPE_SSH)
                conSSH--;
    /*SSH TELNET*/
    loginInfo[sessionIndex].sessionType = L7_LOGIN_TYPE_UNKNWN;
    loginInfo[sessionIndex].userStorage = L7_NULL;

    /* reset connection values, specifically the index to this connection */
    /* cliInitConnection(cliCurrentHandleGet()); */

    /* we must be sure to set the index as empty last to ensure we
        do not overwrite any variables */
    loginInfo[sessionIndex].emptyIndex = L7_TRUE;
    return L7_SUCCESS;
  }
  L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_CLI_WEB_COMPONENT_ID,
          "\r\nLogin Session logout index out of bounds (%u)",sessionIndex);
  return L7_FAILURE;
}


void obtLoginSessionStart(L7_uint32 sessionIndex)
{
  if ((sessionIndex >= 0) && (sessionIndex < FD_CLI_DEFAULT_MAX_CONNECTIONS))
  {
    osapiSemaTake (loginSessionSemaphore, L7_WAIT_FOREVER);

    if (loginInfo[sessionIndex].emptyIndex != L7_TRUE)
    {
      loginInfo[sessionIndex].obtSessionStartTime = osapiUpTimeRaw();
      loginInfo[sessionIndex].obtLastActivityTime = osapiUpTimeRaw();
    }

    osapiSemaGive (loginSessionSemaphore);
  }

  return;

}



void obtLoginSessionEnd(L7_uint32 sessionIndex)
{
  if ((sessionIndex >= 0) && (sessionIndex < FD_CLI_DEFAULT_MAX_CONNECTIONS))
  {
    osapiSemaTake (loginSessionSemaphore, L7_WAIT_FOREVER);

    if (loginInfo[sessionIndex].emptyIndex != L7_TRUE)
    {
      loginInfo[sessionIndex].obtSessionStartTime = 0;
      loginInfo[sessionIndex].obtLastActivityTime = 0;
      loginInfo[sessionIndex].lastActivityTime    = osapiUpTimeRaw();
    }

    osapiSemaGive (loginSessionSemaphore);
  }

  return;

}

void loginSessionUpdateObtActivityTime(L7_uint32 sessionIndex)
{
  if (sessionIndex >= 0 && loginInfo[sessionIndex].emptyIndex != L7_TRUE)
  {
    loginInfo[sessionIndex].obtLastActivityTime = osapiUpTimeRaw();
  }
}

L7_BOOL loginSessionDoesLoginExist(L7_uint32 sessionIndex)
{

  if ((sessionIndex >= 0) && (sessionIndex < FD_CLI_DEFAULT_MAX_CONNECTIONS))
  {
    if (loginInfo[sessionIndex].emptyIndex == L7_FALSE)
    {
      return L7_TRUE;
    }
  }
  return L7_FALSE;
}

L7_char8 *loginSessionUserNameGet(L7_uint32 sessionIndex)
{

  if ((sessionIndex >= 0) && (sessionIndex < FD_CLI_DEFAULT_MAX_CONNECTIONS))
  {
    return loginInfo[sessionIndex].userName;
  }
  return NULL;
}

void loginSessionUserNameSet(L7_uint32 sessionIndex, L7_char8 *userName)
{
  L7_uint32 cpuIntIfNum;


  if ((sessionIndex >= 0) && (sessionIndex < FD_CLI_DEFAULT_MAX_CONNECTIONS))
  {
    if (loginSessionUserCurrentlyLoggedIn(userName) == L7_TRUE && (strcmp(userName, "admin") == 0))
    {
      if (nimFirstValidIntfNumberByType(L7_CPU_INTF, &cpuIntIfNum) == L7_SUCCESS)
        trapMgrMultipleUsersLogTrap(cpuIntIfNum);
    }
    memset(loginInfo[sessionIndex].userName, 0, L7_LOGIN_SIZE);
    memcpy(loginInfo[sessionIndex].userName, userName, L7_LOGIN_SIZE-1);
  }
}

L7_inet_addr_t *loginSessionIPAddrGet(L7_uint32 sessionIndex)
{

  if ((sessionIndex >= 0) && (sessionIndex < FD_CLI_DEFAULT_MAX_CONNECTIONS))
  {
    return &loginInfo[sessionIndex].ip;
  }
  return 0;
}

void loginSessionIPAddrSet(L7_uint32 sessionIndex, L7_inet_addr_t *ip)
{

  if ((sessionIndex >= 0) && (sessionIndex < FD_CLI_DEFAULT_MAX_CONNECTIONS))
  {
    loginInfo[sessionIndex].ip = *ip;
  }
}

L7_uint32 loginSessionUnitGet(L7_uint32 sessionIndex)
{

  if ((sessionIndex >= 0) && (sessionIndex < FD_CLI_DEFAULT_MAX_CONNECTIONS))
  {
    return loginInfo[sessionIndex].unit;
  }
  return 0;
}

L7_uint32 loginSessionSlotGet(L7_uint32 sessionIndex)
{

  if ((sessionIndex >= 0) && (sessionIndex < FD_CLI_DEFAULT_MAX_CONNECTIONS))
  {
    return loginInfo[sessionIndex].slot;
  }
  return 0;
}

L7_uint32 loginSessionPortGet(L7_uint32 sessionIndex)
{

  if ((sessionIndex >= 0) && (sessionIndex < FD_CLI_DEFAULT_MAX_CONNECTIONS))
  {
    return loginInfo[sessionIndex].port;
  }
  return 0;
}

void loginSessionUSPSet(L7_uint32 sessionIndex, L7_uint32 u, L7_uint32 s, L7_uint32 p)
{

  if ((sessionIndex >= 0) && (sessionIndex < FD_CLI_DEFAULT_MAX_CONNECTIONS))
  {
    loginInfo[sessionIndex].unit = u;
    loginInfo[sessionIndex].slot = s;
    loginInfo[sessionIndex].port = p;
  }
}

L7_uint32 loginSessionIfIndexGet(L7_uint32 sessionIndex)
{

  if ((sessionIndex >= 0) && (sessionIndex < FD_CLI_DEFAULT_MAX_CONNECTIONS))
  {
    return loginInfo[sessionIndex].ifIndex;
  }
  return 0;
}

void loginSessionIfIndexSet(L7_uint32 sessionIndex, L7_uint32 ifIndex)
{

  if ((sessionIndex >= 0) && (sessionIndex < FD_CLI_DEFAULT_MAX_CONNECTIONS))
  {
    loginInfo[sessionIndex].ifIndex = ifIndex;
  }
}

void loginSessionUpdateActivityTime(L7_uint32 sessionIndex)
{

  if ((sessionIndex >= 0) && (sessionIndex < FD_CLI_DEFAULT_MAX_CONNECTIONS))
  {
    loginInfo[sessionIndex].lastActivityTime = osapiUpTimeRaw();
  }
}

L7_uint32 loginSessionLoginTimeGet(L7_uint32 sessionIndex)
{

  if ((sessionIndex >= 0) && (sessionIndex < FD_CLI_DEFAULT_MAX_CONNECTIONS))
  {
    return loginInfo[sessionIndex].sessionStartTime;
  }
  return 0;
}

L7_uint32 loginSessionConnectionDurationGet(L7_uint32 sessionIndex)
{

  if ((sessionIndex >= 0) && (sessionIndex < FD_CLI_DEFAULT_MAX_CONNECTIONS))
  {
      if (loginInfo[sessionIndex].sessionStartTime != 0)
      {
          /*
          There is a window where the logout could have already occurred, but
          the session record is still active and sessionStartTime has been
          set to 0.  In that case, return 0 instead of osapiUpTimeRaw - 0.
          */
          return(osapiUpTimeRaw() - loginInfo[sessionIndex].sessionStartTime);
      }
  }
  return 0;
}

L7_uint32 loginSessionLoginIdleTimeGet(L7_uint32 sessionIndex)
{

  if((sessionIndex >= 0) &&  loginInfo[sessionIndex].obtSessionStartTime > 0)
  {
    if (loginInfo[sessionIndex].obtLastActivityTime != 0)
    {
      return(osapiUpTimeRaw() - loginInfo[sessionIndex].obtLastActivityTime);
    }
  }
  else if ((sessionIndex >= 0) && (sessionIndex < FD_CLI_DEFAULT_MAX_CONNECTIONS))
  {
    if (loginInfo[sessionIndex].lastActivityTime != 0)
    {
        /*
        There is a window where the logout could have already occurred, but
        the session record is still active and lastActivityTime has been
        set to 0.  In that case, return 0 instead of osapiUpTimeRaw - 0.
        This will prevent a segmentation fault when the UtilTask has just
        the right timing to look at the lastActivityTime value of an
        inactive login and tries to do a force logout of that session.
        */
        return(osapiUpTimeRaw() - loginInfo[sessionIndex].lastActivityTime);
    }
  }
  return 0;
}

void *loginSessionUserStorageGet(L7_uint32 sessionIndex)
{

  if ((sessionIndex >= 0) && (sessionIndex < FD_CLI_DEFAULT_MAX_CONNECTIONS))
  {
    return loginInfo[sessionIndex].userStorage;
  }
  return L7_NULL;
}

void loginSessionUserStorageSet(L7_uint32 sessionIndex, void *userPointer)
{

  if ((sessionIndex >= 0) && (sessionIndex < FD_CLI_DEFAULT_MAX_CONNECTIONS))
  {
    loginInfo[sessionIndex].userStorage = userPointer;;
  }
}

L7_uint32 loginSessionConnectionTypeGet(L7_uint32 sessionIndex)
{

  if ((sessionIndex >= 0) && (sessionIndex < FD_CLI_DEFAULT_MAX_CONNECTIONS))
  {
    return loginInfo[sessionIndex].sessionType;
  }
  return L7_LOGIN_TYPE_UNKNWN;
}

void loginSessionConnectionTypeSet(L7_uint32 sessionIndex, L7_uint32 sessionType)
{

  if ((sessionIndex >= 0) && (sessionIndex < FD_CLI_DEFAULT_MAX_CONNECTIONS))
  {
    loginInfo[sessionIndex].sessionType = sessionType;

    /*SSH TELNET*/
        if (loginInfo[sessionIndex].sessionType == L7_LOGIN_TYPE_TELNET )
            conTelnet++;
        else
            if(loginInfo[sessionIndex].sessionType == L7_LOGIN_TYPE_SSH)
                conSSH++;
    /*SSH TELNET*/
  }
}

L7_BOOL loginSessionUserCurrentlyLoggedIn(L7_char8 *userName)
{
  L7_uint32 index;


  for (index = 0; index < FD_CLI_DEFAULT_MAX_CONNECTIONS; index++)
  {
    if (loginInfo[index].emptyIndex == L7_FALSE)
    {
      if (strncmp(userName, loginInfo[index].userName, L7_LOGIN_SIZE) == 0)
      {
        return L7_TRUE;
      }
    }
  }
  return L7_FALSE;
}

void loginSessionResetConnectionSet(L7_uint32 sessionIndex)
{

  if ((sessionIndex >= 0) && (sessionIndex < FD_CLI_DEFAULT_MAX_CONNECTIONS))
  {
    loginInfo[sessionIndex].resetConnection = L7_TRUE;
  }
}

L7_BOOL loginSessionResetConnectionGet(L7_uint32 sessionIndex)
{

  if ((sessionIndex >= 0) && (sessionIndex < FD_CLI_DEFAULT_MAX_CONNECTIONS))
  {
    return loginInfo[sessionIndex].resetConnection;
  }
  return L7_FALSE;
}


/**************************************************************************
*
* @purpose  Enable or disable hiding of "support" commands for a session
*
* @param    sessionIndex  CLI session
* @param    mode    L7_ENABLE or L7_DISABLE
*
* @returns  void
*
* @notes   The setting of this mode controls whether hidden support commands
*          are available from the user interface.
*
* @end
*
*************************************************************************/
void loginSessionSupportFeatureHiddenModeSet(L7_int32 sessionIndex,L7_uint32 mode)
{
    loginInfo[sessionIndex].featureHiddenMode = mode;
}


/**************************************************************************
*
* @purpose  Get setting for display of hidden of "support" commands for a session
*
* @param    sessionIndex  CLI session
*
* @returns  mode    L7_ENABLE or L7_DISABLE
*
*
* @notes   The setting of this mode controls whether hidden support commands
*          are available from the user interface.
*
* @end
*
*************************************************************************/
L7_uint32 loginSessionSupportFeatureHiddenModeGet(L7_int32 sessionIndex)
{
    return (loginInfo[sessionIndex].featureHiddenMode);
}

