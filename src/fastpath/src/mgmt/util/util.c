/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* Name: util.c
*
* Purpose: utility task
*
*
* Created by: Colin Verne 06/19/2000
*
* Component:
*
*********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_common.h"
#include "strlib_base_common.h"
#include "strlib_base_common.h"
#include <cliapi.h>
#include <ews_api.h>
#include <login_sessions_api.h>
#include "cli_web_include.h"
#include "user_mgr_api.h"
#include "usmdb_user_mgmt_api.h"
#include "cli_web_exports.h"
#include "user_manager_exports.h"
#include "usmdb_sim_api.h"
#include "usmdb_sshd_api.h"
#include "usmdb_sslt_api.h"
#include "usmdb_telnet_api.h"

#define MAX_RESET_PER_LOOP   1

#define UTIL_TASK_SLEEP_TIME 250

extern cliWebCfgData_t cliWebCfgData;

extern void ewaNetHTTPModeToggle(void);
extern void ewaNetSSLModeToggle(void);
extern L7_RC_t ewaNetTelnetSocketActivate(void);
extern L7_RC_t ewaNetTelnetSocketClose(void);
extern void ewaNetVarsInit(void);

extern void usmDbWebSessionTableTimeoutScan(void);

static void *timeOutSemaId = L7_NULLPTR;
static L7_uint32 ssl_mode, web_mode, telnet_mode;
static L7_int32 httpListenPort, telnetListenPort;

#ifdef L7_CAPTIVE_PORTAL_PACKAGE
  #include "usmdb_cpdm_api.h"
  #include "usmdb_cpcm_api.h"
  #include "usmdb_cpdm_connstatus_api.h"
  #include "usmdb_cpim_api.h"
  /* #include "wdm_ap_api.h" */
  static int cp_web_mode;
  static int cp_http_port;
  extern void ewaNetCaptivePortalHTTPPortModeSet(int cp_mode, int cp_port);
  extern void ewaFlushAndAbortHandles(L7_IP_ADDR_t ipAddr);
#endif /* L7_CAPTIVE_PORTAL_PACKAGE */

void timeOutGetSemaId( void )
{
  if (timeOutSemaId == L7_NULLPTR)
  {
    timeOutSemaId = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  }

  (void)osapiSemaTake(timeOutSemaId, L7_WAIT_FOREVER);
}

void timeOutReturnSemaId( void )
{
  (void)osapiSemaGive(timeOutSemaId);
}

void l7UtilVarsInit( void )
{
  ssl_mode = L7_DISABLE;
  web_mode = L7_DISABLE;
  telnet_mode = L7_DISABLE;
  httpListenPort = -1;
  telnetListenPort = -1;
#ifdef L7_CAPTIVE_PORTAL_PACKAGE
  cp_web_mode = L7_DISABLE;
  cp_http_port = -1; /* initially flagged invalid */
#endif /* L7_CAPTIVE_PORTAL_PACKAGE */
}

int L7_util_task( void )
{
  L7_uint32 index = 0;
  L7_uint32 telnetTimeout = 0;
  L7_uint32 serialTimeout = 0;
  L7_uint32 sshTimeout = 0;
#ifdef L7_OUTBOUND_TELNET_PACKAGE
  L7_uint32 obtTimeout = 0;
#endif
  L7_uint32 loginType = 0;
  L7_uint32 idleTime = 0;
  L7_uint32 webSessionTimoutInterval = 30;  /* 30 secs */
  L7_uint32 loopCounter = 0;
  L7_BOOL validConnection = L7_FALSE;
  L7_BOOL resetConnection = L7_FALSE;
  EwsContext context = L7_NULL;
  L7_uchar8 temp_buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 resetCount;
  L7_uint32 telnet_admin_mode = 0;
  L7_uint32 telnet_sessions_max = 0;
  L7_uint32 telnet_sessions_new = 0;
  L7_uint32 telnet_sessions_current = 0;
  L7_uint32 telnetCurrentListenPort = 0;
  L7_uint32 httpCurrentListenPort = 0;
  L7_uint32 new_mode;
#ifdef L7_CAPTIVE_PORTAL_PACKAGE
  L7_uint32 new_cp_http_port;
#endif /* L7_CAPTIVE_PORTAL_PACKAGE */
  loginSessionInit();

  ewaNetVarsInit();
  l7UtilVarsInit();

  while (1)
  {
    osapiSleepMSec(UTIL_TASK_SLEEP_TIME);

    resetCount = 0;

    if (loopCounter >= webSessionTimoutInterval)
    {
      usmDbWebSessionTableTimeoutScan();
      loopCounter = 0;
    }
    else
    {
      loopCounter++;
    }

    usmDbSerialTimeOutGet(U_IDX, &serialTimeout);
    usmDbAgentTelnetTimeoutGet(U_IDX, &telnetTimeout);
#ifdef L7_OUTBOUND_TELNET_PACKAGE
    usmDbTelnetTimeoutGet(U_IDX, &obtTimeout);
#endif
#ifdef L7_MGMT_SECURITY_PACKAGE
    if (usmDbsshdTimeoutGet(U_IDX, &sshTimeout) != L7_SUCCESS)
    {
      sshTimeout = 1;
    }
#endif
    /* the timeouts can be stored as minutes or seconds, depending upon the platform,
       but we store idle time in seconds. A platform specific conversion factor is thus
       required to convert the timeout in seconds. */
    serialTimeout *= L7_CLI_WEB_TIME_CONVERSION_FACTOR;
    telnetTimeout *= L7_CLI_WEB_TIME_CONVERSION_FACTOR;
    sshTimeout    *= L7_CLI_WEB_TIME_CONVERSION_FACTOR;
#ifdef L7_OUTBOUND_TELNET_PACKAGE
    obtTimeout    *= L7_CLI_WEB_TIME_CONVERSION_FACTOR;
#endif

    timeOutGetSemaId();

    for (index = 0; index < FD_CLI_DEFAULT_MAX_CONNECTIONS; index++)
    {
      usmDbLoginSessionValidEntry(U_IDX, index, &validConnection);

      if (validConnection == L7_TRUE)
      {
        usmDbLoginSessionTypeGet(U_IDX, index, &loginType);
        usmDbLoginSessionIdleTimeGet(U_IDX, index, &idleTime);
        context = usmDbLoginSessionUserStorageGet(U_IDX, index);
        usmDbLoginSessionResetConnectionGet(U_IDX, index, &resetConnection);

        if (loginType == L7_LOGIN_TYPE_SERIAL)
        {                                      /* Never reset the Serial Connection */
          resetConnection = L7_FALSE;
        }

        if (resetConnection == L7_TRUE)
        {
          resetCount++;
          if (resetCount > MAX_RESET_PER_LOOP)
          {
            resetConnection = L7_FALSE;
          }
        }

#ifdef L7_OUTBOUND_TELNET_PACKAGE
        if (context != NULL && context->obtActive == L7_TRUE)
        {
          if ((obtTimeout != 0)  && (idleTime >= obtTimeout)) /* 0 => infinite timeout */
          {
            /*
               OBT Connection timed out, so display timeout message
             */
            if (context)
            {
              context->obtActive = L7_FALSE;
              usmDbTelnetDisconnect(cliCurrentSocketGet(), context->obtSocket);
            #ifdef L7_CLI_PACKAGE
              usmDbObtLoginSessionEnd(index);
            #endif
              context->obtSocket = 0;

              if ((loginType == L7_LOGIN_TYPE_TELNET) || (loginType == L7_LOGIN_TYPE_SSH))
              {
                context->unbufferedWrite = TRUE;
                ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, context, pStrInfo_base_ObtIdleDisconnectMsg);
                ewsTelnetWrite(context, context->telnet->prompt);
                ewsFlushAll(context);
              }
              if (loginType == L7_LOGIN_TYPE_SERIAL)
              {
                cliWriteSerialAddBlanks (2, 1, 0, 0, L7_NULLPTR, pStrInfo_base_ObtIdleDisconnectMsg);
                cliWriteSerial((L7_char8 *)(context->telnet->prompt));
              }
              break;
            }
          }
        }
#endif /* L7_OUTBOUND_TELNET_PACKAGE */
        if (loginType == L7_LOGIN_TYPE_SERIAL)
        {

          if ((serialTimeout != 0) && (idleTime >= serialTimeout))
          {
            if (cliIsCurrUnitMgmtUnit() == L7_TRUE)
            {
              /* Log the user disconnections in the command logger */
              osapiSnprintf(temp_buf, sizeof(temp_buf), (L7_uchar8*)pStrInfo_base_DisconnectedDueToIdleTimeout);
              cliWebCmdLoggerEntryAdd(temp_buf, index);
              /* do not logout for transfer connections until the transfer is complete */
              /* Also, do not logout if I/O is currently redirected.
                 I/O may be redirected in the case of remote devshell access */
              if (((usmDbTransferInProgressGet(U_IDX) != L7_TRUE) || (context != usmDbTransferContextGet(U_IDX))) &&
                  (cliIoRedirectToCliGet() != L7_TRUE))
              {
                if (userMgrIsAuthenticationRequired(ACCESS_LINE_CONSOLE, ACCESS_LEVEL_LOGIN))
                {
                  cliWriteSerialAddBlanks(1, 1, 0, 0, L7_NULLPTR, pStrInfo_base_IdleDisconnectMsg);
                }
                cliContextLogout(context);
                break;
              }
              else
              {
                cliLogoutDeferredSet(context, L7_TRUE);
              }
            }
          }
        }
        else if ((loginType == L7_LOGIN_TYPE_TELNET) || (loginType == L7_LOGIN_TYPE_SSH))
        {
          if (((loginType == L7_LOGIN_TYPE_TELNET) && ((telnetTimeout != 0) && (idleTime >= telnetTimeout))) ||
              ((loginType == L7_LOGIN_TYPE_SSH) && ((sshTimeout != 0) && (idleTime >= sshTimeout)))
            )
          {
            /*
               Connection timed out, so display timeout message
             */

            /* do not logout for transfer connections until the transfer is complete */
            if (!((usmDbTransferInProgressGet(U_IDX) == L7_TRUE) && (context == usmDbTransferContextGet(U_IDX))))
            {
              /*
                 There is a window during which a user can logout, but the UtilTask
                 has already determined that the session should be logged out and
                 context could be 0 during that interval; when that is the case,
                 skip the forced logout
               */

              if (context)
              {
                /* Only attempt to write if the telnet session is still valid. */
                if (ewsTelnetSessionValidate(context) == L7_TRUE)
                {
                  osapiSnprintf(temp_buf, sizeof(temp_buf), pStrInfo_base_DisconnectedDueToIdleTimeout);
                  cliWebCmdLoggerEntryAdd(temp_buf, index);
                  /* Perform an unbuffered write. This ensures
                   * that the Web Buffer gets freed. The ewsFlushAll(),
                   * below, was not freeing the buffer being allocated
                   * by ewsTelnetWrite().
                   */
                  context->unbufferedWrite = TRUE;
                  ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, context, pStrInfo_base_IdleDisconnectMsg);
                }

                cliContextLogout(context);
                break;
              }
            }
            else
            {
              cliLogoutDeferredSet(context, L7_TRUE);
            }
          }

          if (resetConnection == L7_TRUE)
          {
            /*
               Connection was reset, so display reset message
             */

            /* do not logout for transfer connections until the transfer is complete */
            if (!((usmDbTransferInProgressGet(U_IDX) == L7_TRUE) && (context == usmDbTransferContextGet(U_IDX))))
            {
              /*
                 There is a window during which a user can logout, but the UtilTask
                 has already determined that the session should be logged out and
                 context could be 0 during that interval; when that is the case,
                 skip the forced logout
               */

              if (context)
              {
                /* Only attempt to write if the telnet session is still valid. */
                if (ewsTelnetSessionValidate(context) == L7_TRUE)
                {
                  /* Log the user disconnections in the command logger */
                  osapiSnprintf(temp_buf, sizeof(temp_buf), pStrInfo_base_DisconnectedByAdmin);
                  cliWebCmdLoggerEntryAdd(temp_buf, index);
                  context->unbufferedWrite = TRUE;
                  ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, context, pStrInfo_base_ResetDisconnectMsg);
                }

                cliContextLogout(context);
                break;
              }
            }
            else
            {
              cliLogoutDeferredSet(context, L7_TRUE);
            }
          }
        }
      }
    }

    /* Check for HTTP and SSL mode changes */
    if (usmDbSwDevCtrlWebMgmtModeGet(0, &new_mode) != L7_SUCCESS)
    {
      new_mode = L7_DISABLE;
    }
    if (web_mode != new_mode)
    {
      ewaNetHTTPModeToggle();
      web_mode = new_mode;
      usmDbSwDevCtrlWebMgmtPortNumGet(0, &httpListenPort);
    }

#ifdef L7_CAPTIVE_PORTAL_PACKAGE
    /* Check for CP global mode changes */
    if (L7_SUCCESS == usmDbCpdmGlobalModeGet(&new_mode))
    {
      if (usmDbCpdmGlobalHTTPPortGet(&new_cp_http_port) != L7_SUCCESS)
      {
        new_mode = L7_DISABLE;
        cp_http_port = 0;
      }
      if (-1==cp_http_port)
      {
        cp_http_port = new_cp_http_port;
      }
      if ((cp_web_mode != new_mode) || (cp_http_port != new_cp_http_port))
      {
        cp_http_port = new_cp_http_port;
        cp_web_mode = new_mode;
        ewaNetCaptivePortalHTTPPortModeSet(cp_web_mode,cp_http_port);
      }
    }

    {
      L7_enetMacAddr_t mac;
      L7_uint32 sessionTime = 0;
      L7_IP_ADDR_t ipAddr=0;
      L7_uint32 intIfNum = 0;
      L7_uint32 timeout = 0;
      time_t connTime = 0;
      time_t now = 0;
      L7_uint32 SECONDS_AFTER_AUTH = 45;
      L7_uint32 SECONDS_BEFORE_ADMIN_LOGIN = 60;

      if (usmDbCpdmGlobalStatusSessionTimeoutGet(&timeout) != L7_SUCCESS)
      {
        timeout = CP_AUTH_SESSION_TIMEOUT_MIN;
      }
      now = time(NULL);

      /*
      Cleanup connection handles for timed out authentication attempts
      If we have a connection that has exceeded the authentication time, then;
      - Disassociate client
      - Flush and abort *all* HTTP connection handles
      - Remove the AIP entry
      */
      while (L7_SUCCESS==usmDbCpcmAIPStatusNextGet(ipAddr, &ipAddr))
      {
        if (L7_SUCCESS==usmDbCpcmAIPStatusConnTimeGet(ipAddr,&connTime))
        {
          if (now>(timeout+connTime))
          {
#if 0
            L7_LOGF(L7_LOG_SEVERITY_DEBUG,L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
                    "%s: AIP TIMEOUT: Deauth; AIP delete; Flush & Abort handles for 0x%08x",
                    __FUNCTION__,ipAddr);
#endif
            if ((L7_SUCCESS==usmDbCpcmAIPStatusMacAddrGet(ipAddr,&mac)) &&
                (L7_SUCCESS==usmDbCpcmAIPStatusIntfIdGet(ipAddr, &intIfNum)))
            {
              usmDbCpimIntfDeauthenticate(intIfNum, &mac);
            }
            usmDbCpcmAIPStatusDelete(ipAddr);
            ewaFlushAndAbortHandles(ipAddr);
          }
        }
      }

      /*
      Cleanup connection handles for connected clients.
      However, check the session time and only cleanup new handles (new handles
      are from the initial CP authentication session). This is because we want to
      allow this CP user administrator connectivity (after SECONDS_BEFORE_ADMIN_LOGIN).
      Without this time check, we would kill all our client connections when this CP
      user connects back to our server for administration purposes.

      Note: the abort window is deliberate. We need to allow enough time for the server
      to serve the welcome page before we kill the connection. In addition, we don't want
      to kill any new connections to the server for subsequent administrator logins.
      Plus we don't need to cycle through the server handle array needlessly while the
      CP client is connected.
      */
      memset(&mac, 0, sizeof(L7_enetMacAddr_t));
      while(L7_SUCCESS==usmDbCpdmClientConnStatusNextGet(&mac,&mac))
      {
        if ((L7_SUCCESS==usmDbCpdmClientConnStatusIpGet(&mac,&ipAddr)) &&
            (L7_SUCCESS!=usmDbCpcmAIPStatusGet(ipAddr)) &&
            (L7_SUCCESS==usmDbCpdmClientConnStatusSessionTimeGet(&mac,&sessionTime)) &&
            ((sessionTime>SECONDS_AFTER_AUTH) && (sessionTime<SECONDS_BEFORE_ADMIN_LOGIN)))
        {
#if 0
          L7_LOGF(L7_LOG_SEVERITY_DEBUG,L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
                  "%s: CLIENT CLEANUP: Flush and Abort handles for 0x%08x",
                  __FUNCTION__,ipAddr);
#endif
          ewaFlushAndAbortHandles(ipAddr);
        }
      }
    }
#endif /* L7_CAPTIVE_PORTAL_PACKAGE */

#ifdef L7_MGMT_SECURITY_PACKAGE
    if (usmDbssltAdminModeGet(0, &new_mode) != L7_SUCCESS)
    {
      new_mode = L7_DISABLE;
    }
    if (ssl_mode != new_mode)
    {
      ewaNetSSLModeToggle();
      ssl_mode = new_mode;
      usmDbSwDevCtrlWebMgmtPortNumGet(0,&httpListenPort);
    }
#endif
    if ((ssl_mode == L7_ENABLE) || (web_mode == L7_ENABLE))
    {
      if (L7_SUCCESS != usmDbSwDevCtrlWebMgmtPortNumGet(0, &httpCurrentListenPort))
      {
        httpListenPort = FD_HTTP_PORT_NUM;
      }
      if ( httpListenPort == -1 )
      {
        httpListenPort = httpCurrentListenPort;
      }
      if (httpListenPort != httpCurrentListenPort)
      {
         cliWebHttpPortUpdate(httpCurrentListenPort);
         httpListenPort = httpCurrentListenPort;
      }
    }

    if (usmDbAgentTelnetNumSessionsGet(0, &telnet_sessions_max) != L7_SUCCESS)
    {
      telnet_sessions_max = 0;
    }
    if (usmDbAgentTelnetNewSessionsGet(0, &telnet_sessions_new) != L7_SUCCESS)
    {
      telnet_sessions_new = 0;
    }
    telnet_sessions_current = usmDbLoginNumSessionsActiveGet(L7_FALSE, L7_LOGIN_TYPE_TELNET);

    if (telnet_sessions_current > 0)
    {
      new_mode = L7_ENABLE;
    }
    else
    {
      if ((telnet_sessions_max == 0) || (telnet_sessions_new == 0))
      {
        new_mode = L7_DISABLE;
      }
      else
      {
        new_mode = L7_ENABLE;
      }
    }

    if (usmDbAgentTelnetAdminModeGet(0, &telnet_admin_mode) != L7_SUCCESS)
    {
      new_mode = L7_DISABLE;
    }
    else
    {
      if (telnet_admin_mode == L7_ENABLE)
      {
        new_mode = L7_ENABLE;
      }
      else
      {
        if (usmDbLoginNumSessionsActiveGet(L7_FALSE, L7_LOGIN_TYPE_TELNET) == 0)
        {
          new_mode = L7_DISABLE;
        }
      }
    }

    if (telnet_mode != new_mode)
    {
      if (new_mode == L7_ENABLE)
      {
        ewaNetTelnetSocketActivate();
        usmDbSwDevCtrlTelnetMgmtPortNumGet(0, &telnetListenPort);
      }
      else
      {
        ewaNetTelnetSocketClose();
      }
      telnet_mode = new_mode;
    }
    if (telnet_mode == L7_ENABLE)
    {
      if (L7_SUCCESS != usmDbSwDevCtrlTelnetMgmtPortNumGet(0, &telnetCurrentListenPort))
      {
        telnetListenPort = FD_TELNET_PORT_NUM;
      }
      if ( telnetListenPort == -1 )
      {
        telnetListenPort = telnetCurrentListenPort;
      }
      if (telnetListenPort != telnetCurrentListenPort)
      {
         cliWebTelnetPortUpdate();
         telnetListenPort = telnetCurrentListenPort;
      }
    }

    timeOutReturnSemaId();
  }

  return 0;
}
