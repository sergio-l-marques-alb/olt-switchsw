
/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 * @filename  cli_web_api.c
 *
 * @purpose   cli and web manager api functions
 *
 * @component cliWebUI component
 *
 * @comments  none
 *
 * @create    03/26/2001
 *
 * @author    asuthan
 *
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "cli_web_include.h"
#include "usmdb_cmd_logger_api.h"
#include "usmdb_util_api.h"
#include "osapi_support.h"
#include "cli_web_user_mgmt.h"
#include "usmdb_user_mgmt_api.h"
#include "defaultconfig.h"

static L7_BOOL cliWebSessionDebugDisplayCanBeActive ( L7_int32 session);

extern cliWebCfgData_t cliWebCfgData;
extern cliWebCnfgrState_t cliWebCnfgrState;

extern L7_RC_t cliWebIoRedirectStart(void);
extern L7_RC_t cliWebIoRedirectStop(void);

/*********************************************************************
 * @purpose  Returns the Unit's Web Java Mode
 *
 * @param    void
 *
 * @returns  mode  Web Java Mode
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
  L7_uint32
cliWebGetJavaWebMode(void)
{
  return(cliWebCfgData.systemJavaMode);
}

/*********************************************************************
 * @purpose  Sets the Unit's Web Java Mode
 *
 * @param    void
 *
 * @returns  mode  Web Java Mode
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
  L7_uint32
cliWebSetJavaWebMode(L7_uint32 mode)
{
  cliWebCfgData.systemJavaMode = mode;
  cliWebCfgData.cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Returns the Unit's Web Mode
 *
 * @param    void
 *
 * @returns  mode  System Web Mode
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
  L7_uint32
cliWebGetSystemWebMode(void)
{
  if(CLI_WEB_IS_READY)
    return(cliWebCfgData.systemWebMode);
  else
    return L7_DISABLE;
}

/*********************************************************************
 * @purpose  Sets the Unit's Web Mode
 *
 * @param    mode  System Web Mode
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
  void
cliWebSetSystemWebMode(L7_uint32 mode)
{
  L7_uint32 i;
  EwaSession session;

  cliWebCfgData.systemWebMode = mode;
  cliWebCfgData.cfgHdr.dataChanged = L7_TRUE;
  
  if(mode == L7_DISABLE)
  {
    for (i=0; i < FD_UI_DEFAULT_MAX_CONNECTIONS; i++)
    {
      if (EwaSessionGet(i, &session) != L7_SUCCESS)
      {
            continue;
      }
      if(session.type == L7_LOGIN_TYPE_HTTP)
      {
            EwaSessionRemove(session.sid);
      }
    }
  }
}

/*********************************************************************
 * @purpose  Sets maximum number of web sessions
 *
 * @param    val  maximum allowable number of web sessions
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
  L7_RC_t
cliWebHttpNumSessionsSet(L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;

  if (cliWebCfgData.webNumSessions != val)
  {
    if ((val >= 0) && (val <= FD_HTTP_DEFAULT_MAX_CONNECTIONS))
    {
      cliWebCfgData.webNumSessions = val;
      cliWebCfgData.cfgHdr.dataChanged = L7_TRUE;
    }
    else
    {
      rc = L7_FAILURE;
    }
  }
  return rc;
}

/*********************************************************************
 * @purpose  Get the maximum number of web sessions
 *
 * @returns  Return the maximum number of web sessions
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
  L7_RC_t
cliWebHttpNumSessionsGet(L7_uint32 *val)
{
  *val = cliWebCfgData.webNumSessions;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Sets http session hard timeout (in hours)
 *
 * @param    val  http session hard timeout
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
  L7_RC_t
cliWebHttpSessionHardTimeOutSet(L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;

  if (cliWebCfgData.webSessionHardTimeOut != val)
  {
    if ((val >= FD_HTTP_SESSION_HARD_TIMEOUT_MIN) && (val <= FD_HTTP_SESSION_HARD_TIMEOUT_MAX))
    {
      cliWebCfgData.webSessionHardTimeOut = val;
      cliWebCfgData.cfgHdr.dataChanged = L7_TRUE;
    }
    else
    {
      rc = L7_FAILURE;
    }
  }
  return rc;
}

/*********************************************************************
 * @purpose  Get the http session hard timeout (in hours)
 *
 * @returns  Return the http session hard timeout
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
  L7_RC_t
cliWebHttpSessionHardTimeOutGet(L7_uint32 *val)
{
  *val = cliWebCfgData.webSessionHardTimeOut;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Sets http session soft timeout (in minutes)
 *
 * @param    val  http session soft timeout
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
  L7_RC_t
cliWebHttpSessionSoftTimeOutSet(L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;

  if (cliWebCfgData.webSessionSoftTimeOut != val)
  {
    if ((val >= FD_HTTP_SESSION_SOFT_TIMEOUT_MIN) && (val <= FD_HTTP_SESSION_SOFT_TIMEOUT_MAX))
    {
      cliWebCfgData.webSessionSoftTimeOut = val;
      cliWebCfgData.cfgHdr.dataChanged = L7_TRUE;
    }
    else
    {
      rc = L7_FAILURE;
    }
  }
  return rc;
}

/*********************************************************************
 * @purpose  Get the http session soft timeout (in minutes)
 *
 * @returns  Return the http session soft timeout
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
  L7_RC_t
cliWebHttpSessionSoftTimeOutGet(L7_uint32 *val)
{
  *val = cliWebCfgData.webSessionSoftTimeOut;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Returns the Unit's System prompt
 *
 * @param    prompt   L7_char8 pointer, max length L7_PROMPT_SIZE
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
  void
cliWebGetSystemCommandPrompt(L7_char8 *prompt)
{
  memcpy(prompt, cliWebCfgData.systemCommandPrompt, L7_PROMPT_SIZE);
}

/*********************************************************************
 * @purpose  Sets the Unit's System prompt
 *
 * @param    prompt   L7_char8 pointer, max length L7_PROMPT_SIZE
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
  void
cliWebSetSystemCommandPrompt(L7_char8 *prompt)
{
  OSAPI_STRNCPY_SAFE( cliWebCfgData.systemCommandPrompt, prompt );
  cliWebCfgData.userPrompt = L7_TRUE;
  cliWebCfgData.cfgHdr.dataChanged = L7_TRUE;
}

/*********************************************************************
 * @purpose  Returns the Unit's System Telnet Timeout
 *
 * @param    void
 *
 * @returns  The Telnet Timeout Value
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 cliWebGetSystemTelnetTimeout(void)
{
  return(cliWebCfgData.systemTelnetTimeout);
}

/*********************************************************************
 * @purpose  Sets the Unit's System Telnet Timeout Value
 *
 * @param    val   The timeout value
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cliWebSetSystemTelnetTimeout(L7_uint32 val)
{
  if ((val < FD_CLI_WEB_DEFAULT_REMOTECON_MIN_TIMEOUT) || (val > FD_CLI_WEB_DEFAULT_REMOTECON_MAX_TIMEOUT))
  {
    return L7_FAILURE;
  }

  cliWebCfgData.systemTelnetTimeout = val;
  cliWebCfgData.cfgHdr.dataChanged = L7_TRUE;
  return  L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Sets the Unit's System Number of Telnet Sessions
 *
 * @param    val   The number of Telnet Sessions
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cliWebSetSystemTelnetNumSessions(L7_uint32 val)
{
  cliWebCfgData.systemTelnetNumSessions = val;
  cliWebCfgData.cfgHdr.dataChanged = L7_TRUE;
  return  L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Set the Admin Mode of Telnet Session
 *
 * @param unitIndex @b((input)) the unit for this operation
 *
 * @param    mode   value of new Admin mode Setting,(L7_ENABLE or L7_DISABLE)
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbAgentTelnetAdminModeSet(L7_uint32 unitIndex, L7_uint32 mode)
{
  cliWebCfgData.systemTelnetAdminMode = mode;
  cliWebCfgData.cfgHdr.dataChanged = L7_TRUE;
  return  L7_SUCCESS;

}

/*********************************************************************
 * @purpose  Get the Admin Mode of Telnet Session
 *
 * @param unitIndex @b((input)) the unit for this operation
 *
 * @param    mode   location to store the Admin mode Setting
 *
 * @returns  L7_SUCCESS
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbAgentTelnetAdminModeGet(L7_uint32 unitIndex, L7_uint32 *mode)
{
  if (CLI_WEB_IS_READY)
  {
    *mode = cliWebCfgData.systemTelnetAdminMode;
    return L7_SUCCESS;
  }
  else
    return L7_FAILURE;
}


/*********************************************************************
 * @purpose  Returns the Unit's System number of Telnet Sessions
 *
 * @param    void
 *
 * @returns  The number of telnet Sessions
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 cliWebGetSystemTelnetNumSessions(void)
{
  if (CLI_WEB_IS_READY)
    return (cliWebCfgData.systemTelnetNumSessions);
  else
    return 0;
}


/*********************************************************************
 * @purpose  Sets the Unit's System Number of new Telnet Sessions
 *
 * @param    val   The number of new Telnet Sessions
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cliWebSetSystemTelnetNewSessions(L7_uint32 val)
{
  cliWebCfgData.systemTelnetNewSessions = val;
  cliWebCfgData.cfgHdr.dataChanged = L7_TRUE;
  return  L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Returns the Unit's System number of new Telnet Sessions
 *
 * @param    void
 *
 * @returns  The number of new telnet Sessions
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 cliWebGetSystemTelnetNewSessions(void)
{
  if (CLI_WEB_IS_READY)
    return(cliWebCfgData.systemTelnetNewSessions);
  else
    return 0;
}


/***************************************************************************
 *
 * @purpose  Read the banner from file
 *
 * @param  *filename  @b{(input))  File from where banner text is to be read
 * @param  *buffer    @b{(output)) Global array to read the banner text
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR
 *
 * @comments
 *
 * @end
 *
 ****************************************************************************/
L7_RC_t cliReadBannerFile(L7_char8 *filename, L7_char8 *buffer)
{
  L7_char8  tempBannerDisplay[BANNER_MAX_FILE_SIZE];
  L7_uint32 fileSize=0;
  L7_uint32 lnWidthCount=0;
  L7_uint32 buffCount=0;
  L7_uint32 tempCount=0;
  L7_uint32 tempLen=0;
  L7_uint32 b=0;
  L7_uint32 n=0;
  L7_uint32 wordLength=0;
  L7_uint32 ipRowNo=0;
  L7_uint32 rowOpCount=0;
  L7_uint32 fileHandle;
  L7_uint32 c=0;
  L7_uint32 i=0;
  L7_RC_t   rc;

  /* check for file opening */
  if(osapiFsOpen(filename, &fileHandle)==L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
            "Failed to open the CLI banner file %s.", filename);
    return L7_ERROR;
  }

  /* close the opened file */
  if(osapiFsClose(fileHandle)==L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
            "Failed to close the CLI banner file %s.", filename);
    return L7_ERROR;
  }

  /* Get the size of the file */
  if (osapiFsFileSizeGet(filename, &fileSize) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
            " File size is zero or file do not exists");
    return L7_ERROR;
  }

  /* check the size of the file */
  if (fileSize>BANNER_MAX_FILE_SIZE)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_CLI_WEB_COMPONENT_ID, " File size is greater "
                           "than 2K. The banner file size is greater than 2K bytes.");
    return L7_ERROR;
  }

  memset(tempBannerDisplay, 0, sizeof(tempBannerDisplay));

  /* read the contents of file in a temporary array */
  rc = osapiFsRead (filename, tempBannerDisplay, fileSize);
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
            "Not able to read the file");
    return L7_ERROR;
  }

  /* copy the contents of temporary array in global array */
  tempLen = strlen(tempBannerDisplay);
  for (tempCount=0, buffCount=0; tempCount < tempLen; tempCount++, buffCount++)
  {
    /* Check for end of file (ctrl-Z).
     */
    if (tempBannerDisplay[tempCount] == 0x1a)
    {
      break;
    }
    /* check for the word length */
    if ( tempBannerDisplay[tempCount] != ' ' )
    {
      wordLength = wordLength + 1;
    }
    else
    {
      wordLength = 0;
    }

    /* keep track of No. of characters in a line */
    lnWidthCount++;

    /* check for the end of line */
    if(tempBannerDisplay[tempCount]!='\n')
    {
      /* check for the width of line */
      if(lnWidthCount > BANNER_MAX_ROW_WIDTH)
      {
    if(tempBannerDisplay[tempCount]==' ')
    {
      if(rowOpCount==50)
      {
        return 0;
      }
      else
      {
        rowOpCount++;
      } /* End if checking rowOpCount*/
    }
    else
    {
      lnWidthCount=1;

      /* check for the width of the word */
      if(wordLength > BANNER_MAX_WORD_SIZE)
      {
        buffer[buffCount]='\r';
        buffCount++;
        buffer[buffCount]='\n';
        /* check for the No. of rows in output */
        if(rowOpCount==BANNER_MAX_ROW_DISPLAY_COUNT)
        {
          return L7_ERROR;
        }
        else
        {
          rowOpCount++;
        } /* End if checking rowOpCount*/

        buffCount++;
      }
      else
      {
        b=tempCount;
        n=0;
        while(tempBannerDisplay[b]!=' ')
        {
          b=b-1;
          n=n+1;
        }/* end while */
        c=buffCount;
        for(i=0;i<(n-1);i++)
        {
          buffer[c]=buffer[c-1];
          c=c-1;
        }

        lnWidthCount=lnWidthCount+(n-1);
        b=buffCount-n;
        buffer[b]='\r';
        b=b+1;
        buffer[b]='\n';
        buffCount=buffCount+1;

        if(rowOpCount==BANNER_MAX_ROW_DISPLAY_COUNT)
        {
          return L7_ERROR;
        }
        else
        {
          rowOpCount++;
        }
      }/* end if wordLength > BANNER_MAX_WORD_SIZE */
    }/* end if tempBannerDisplay[tempCount]==' ' */
      }/* end if lnWidthCount>BANNER_MAX_ROW_WIDTH */

      buffer[buffCount]=tempBannerDisplay[tempCount];
    }
    else
    {
      lnWidthCount=0;
      ipRowNo=ipRowNo+1;
      buffer[buffCount]='\r';
      buffCount++;

      buffer[buffCount]=tempBannerDisplay[tempCount];

      if(rowOpCount==BANNER_MAX_ROW_DISPLAY_COUNT)
      {
    return L7_ERROR;
      }
      else
      {
    rowOpCount++;
      }

      if(ipRowNo==BANNER_MAX_ROW_FILE_COUNT)
      {
    tempCount=tempCount+1;
    if(tempBannerDisplay[tempCount]!='\0')
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_CLI_WEB_COMPONENT_ID, " No. of rows greater than allowed"
                             " maximum of %d. When the number of rows exceeds the maximum allowed"
                             " rows",BANNER_MAX_ROW_FILE_COUNT);
      return L7_ERROR;
    }
    else
    {
      tempCount=tempCount-1;
    }
      }/* ipRowNo==BANNER_MAX_ROW_FILE_COUNT */
    }/* end if tempBannerDisplay[tempCount]!='\n' */
  }/* end for */

  buffer[buffCount]='\0';
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  write the banner config structure to a file
 *
 * @param    outputFileName    filename
 *
 * @returns  rc
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cliWriteBannerFile (L7_char8 * outputFileName)
{
  L7_uint32 filedesc;
  L7_uint32 banner_length;

  /*remove file */
  (void) osapiFsDeleteFile (outputFileName);

  /*create temp file */
  if (osapiFsOpen (outputFileName, &filedesc) == L7_ERROR)
  {
    osapiFsCreateFile (outputFileName);
    if (osapiFsOpen (outputFileName, &filedesc) == L7_ERROR)
    {
      return L7_ERROR;
    }
  }

  banner_length = strlen (cliWebCfgData.cliBanner);
  if (banner_length > BANNER_MAX_FILE_SIZE)
  {
    banner_length = BANNER_MAX_FILE_SIZE;
  }

  if (osapiFsWriteNoClose (filedesc, cliWebCfgData.cliBanner, banner_length) != L7_SUCCESS)
  {
    osapiFsClose (filedesc);
    return L7_ERROR;
  }

  osapiFsClose (filedesc);
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Read the cli banner from a file
 *
 * @param    fileName   Name of the banner file
 *
 * @returns
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cliWebSystemReadBannerFile(L7_char8 *file_name)
{
  return (cliReadBannerFile(file_name, cliWebCfgData.cliBanner));
}

/*********************************************************************
 * @purpose  Read the banner from config structure to a buffer
 *
 * @param    buff    pointer to buffer to which banner is to be copied
 *
 * @returns
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cliWebSystemCopyBanner(L7_char8 *buff)
{
  memcpy(buff, cliWebCfgData.cliBanner,BANNER_MAX_FILE_SIZE);
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Determine if any debug or support display is active for the session
 *
 * @param    session     - CLI login session

 * @returns  L7_TRUE or L7_FALSE
 *
 * @notes    for the particular session, this tests whether
 *           1) any "debug" trace commands are active, OR
 *           2) the "support console" command is in effect.
 *
 *           If either is in effect, the output of all messages submitted to the
 *           syslog utility is displayed on the session.
 *
 *           If either is in effect, the output of all devshell commands is displayed
 *           on the session.
 *
 * @end
 *********************************************************************/
static L7_BOOL cliWebSessionDebugDisplayCanBeActive ( L7_int32 session)
{

  if ( (cliCommon[session].debugDisplayCtl.debugConsoleDisplayEnabled == L7_TRUE) ||
      (cliCommon[session].debugDisplayCtl.supportConsoleDisplayEnabled == L7_TRUE ))
  {
    return L7_TRUE;
  }
  return L7_FALSE;

}


/*********************************************************************
 * @purpose  Return Debug Trace Mode setting for selected session.
 *
 * @param    session     - CLI login session
 *
 * @returns  L7_TRUE or L7_FALSE
 *
 * @returns  L7_TRUE -  display of trace output is enabled on the login session.
 *           L7_FALSE - display of trace output is disabled on the login session.
 *
 * @notes
 *
 *
 * @end
 *********************************************************************/
L7_BOOL cliWebDebugTraceDisplayModeGet (L7_int32 session)
{
  return cliCommon[session].debugDisplayCtl.debugConsoleDisplayEnabled;
}

/*********************************************************************
 * @purpose  Enable display of debug trace output on the current session.
 *
 * @param    enable_flag - L7_TRUE -  display of trace output is enabled on the login session.
 *                         L7_FALSE - display of trace output is disabled on the login session.
 * @param    session     - CLI login session

 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes
 *
 *
 * @end
 *********************************************************************/
L7_RC_t cliWebDebugTraceDisplayModeSet (L7_BOOL enable_flag, L7_int32 session)
{
  L7_int32 i;
  L7_BOOL ioRedirectForTelnet;

  /*  Error checking */
  if (session > FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    return L7_FAILURE;
  }

  /* Configure the session */
  if (enable_flag == L7_TRUE)
  {
    cliCommon[session].debugDisplayCtl.debugConsoleDisplayEnabled = L7_TRUE;

    /* Enable the display of tracing if this is the first "debug" trace command
       enabled for this session
     */
    if ( cliWebSessionDebugDisplayCanBeActive(session) == L7_TRUE)
    {
      /* Indicate that some sort of backdoor session display is enabled for the session */
      cliCommon[session].debugDisplayCtl.consoleTraceDisplayEnabled = L7_TRUE;

      /* Indicate that some sort of backdoor session display is enabled for the system */
      sysapiDebugSupportDisplayModeSet(L7_ENABLE);

      /* If this is a telnet session, allow printfs and the output of devshell
         to be displayed on the remote session.

         This code segment does not affect the output of trace messages. */
      if ( session != CLI_SERIAL_HANDLE_NUM )
      {
        cliWebIoRedirectStart();
      }
    }
  }
  else
  {

    cliCommon[session].debugDisplayCtl.debugConsoleDisplayEnabled = L7_FALSE;

    /* Disable the display of tracing if this is the last "debug" trace command
       enabled for this session AND the support user has not explicitly enabled
       support console for other reasons (e.g. a devshell display)
     */

    if ( cliWebSessionDebugDisplayCanBeActive(session) != L7_TRUE)
    {
      /* Indicate that no backdoor session display is in effect for the session */
      cliCommon[session].debugDisplayCtl.consoleTraceDisplayEnabled = L7_FALSE;

      /* Indicate to the system that no backdoor session display is in effect
         only if no session has it in effect. */
      ioRedirectForTelnet = L7_FALSE;
      /* Only disable sysapi if no session is enabled for trace display */
      for (i = 0; i < FD_CLI_DEFAULT_MAX_CONNECTIONS; i++)
      {
        if (cliCommon[i].debugDisplayCtl.consoleTraceDisplayEnabled == L7_TRUE)
        {
          /* Only disable ioRedirect if no telnet session is enabled for display */
          if (i != CLI_SERIAL_HANDLE_NUM)
          {
            ioRedirectForTelnet = L7_TRUE;
          }
          break;
        }
      }

      if (i == FD_CLI_DEFAULT_MAX_CONNECTIONS )
      {
        /* no sessions are enabled */
        sysapiDebugSupportDisplayModeSet(L7_DISABLE);
      }

      if (cliIoRedirectToCliGet() == L7_TRUE)
      {
        if (ioRedirectForTelnet != L7_TRUE)
        {
          /* No more telnet sessions have a need for IO redirection.
             End the IO redirection.

             Note that as long as IO is redirected, the serial console
             is designed such that it will not time out. */
          cliWebIoRedirectStop();
        }
      }
    }

  }

  return L7_SUCCESS;
}
/*********************************************************************
 * @purpose  Enable display of trace and devshell output on the current session.
 *
 * @param    enable_flag - L7_TRUE -  display of trace output is enabled on the login session.
 *                         L7_FALSE - display of trace output is disabled on the login session.
 * @param    session     - CLI login session
 * @returns
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cliWebSupportConsoleDisplayModeSet (L7_BOOL enable_flag, L7_int32 session)
{
  L7_int32 i;

  /*  Error checking */
  if (session > FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    return L7_ERROR;
  }

  /* Configure the session */

  cliCommon[session].debugDisplayCtl.supportConsoleDisplayEnabled = enable_flag;


  /* Set the sysapi parameter */
  if (enable_flag == L7_TRUE)
  {
    /* Indicate that some sort of backdoor session display is enabled for the session */
    cliCommon[session].debugDisplayCtl.consoleTraceDisplayEnabled = L7_TRUE;

    /* Indicate that some sort of backdoor session display is enabled for the system */
    sysapiDebugSupportDisplayModeSet(L7_ENABLE);

  }
  else
  {
    if ( cliWebSessionDebugDisplayCanBeActive(session) != L7_TRUE)
    {

      /* Indicate that no backdoor session display is in effect for the session */
      cliCommon[session].debugDisplayCtl.consoleTraceDisplayEnabled = L7_FALSE;

      for (i = 0; i < FD_CLI_DEFAULT_MAX_CONNECTIONS; i++)
      {
        /* Only disable traceDisplay if no session is enabled for trace display */
        if (cliCommon[i].debugDisplayCtl.consoleTraceDisplayEnabled == L7_TRUE)
        {
          break;
        }
      }


      if (i == FD_CLI_DEFAULT_MAX_CONNECTIONS )
      {
        /* no sessions are enabled */
        sysapiDebugSupportDisplayModeSet(L7_DISABLE);
      }

    }  /* else  (cliWebSessionDebugDisplayCanBeActive() != L7_TRUE) */
  } /* else (enable_flag != L7_TRUE) */


  return L7_SUCCESS;
}


/*********************************************************************
 * @purpose  Display either a trace event or a syslog message directed to the console
 *
 * @param    buf - Output String.
 *
 * @returns
 *
 * @notes    This routine uses the cliWebAccessSema. A semaphore deadlock may occur if
 *           this routine is invoked from the tEmWeb task.
 *
 *           This routine is used both for the display of
 *               1) syslog messages at a severity configured to display on a console log
 *               2) internal or debug traces
 *
 *           Internal or debug traces are only directed to sessions which
 *           have been enabled for trace display.
 *
 *           Syslog messages are always directed to the serial port session.
 *           as well as any session also enabled for trace display.
 *
 *
 *
 * @end
 *********************************************************************/
void cliWebConsoleLog (L7_uchar8 * buf)
{
  L7_uint32 i;
  L7_BOOL logFlag = L7_TRUE;
  EwsContext context = L7_NULL;
  L7_ADMIN_MODE_t status = L7_ADMIN_MODE_DISABLE;

  if (cliCommon == L7_NULLPTR)
  {
    cliWriteSerial(buf);
    return;
  }

  for (i = 0; i < FD_CLI_DEFAULT_MAX_CONNECTIONS; i++)
  {
    cliWebAccessSemaGet ();
    if (cliCommon[i].debugDisplayCtl.consoleTraceDisplayEnabled == L7_TRUE)
    {
      context = cliWebLoginSessionUserStorageGet(cliCommon[i].userLoginSessionIndex);
      if (context != L7_NULL)
      {
    context->unbufferedWrite = L7_TRUE;
    ewsTelnetWrite (context, buf);
    context->unbufferedWrite = L7_FALSE;
    logFlag = L7_FALSE;
      }
    }
    cliWebAccessSemaFree ();
  }

  if ((logConsoleAdminStatusGet(&status) == L7_SUCCESS) &&
      (status == L7_ADMIN_MODE_ENABLE))
  {
    if (cliCommon[CLI_SERIAL_HANDLE_NUM].debugDisplayCtl.consoleTraceDisplayEnabled == L7_TRUE)
      return;

    else
    {
      cliWriteSerial(buf);
      return;
    }
  }
}

/*********************************************************************
 * @purpose  Add an entry for command logging
 *
 * @param    strInput       input string
 * @param    sessionId      session id
 *
 * @returns  L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/

L7_RC_t cliWebCmdLoggerEntryAdd(L7_char8 *strInput, L7_uint32 sessionId)
{

  L7_uint32 val;
  L7_inet_addr_t aval;
  L7_uint32 adminMode;
  L7_char8  userName[L7_LOGIN_SIZE];
  L7_char8  stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  formattedEntry[L7_CLI_MAX_LARGE_STRING_LENGTH];

  if((usmDbCmdLoggerAdminModeGet(&adminMode) == L7_SUCCESS) && (adminMode == L7_ENABLE))
  {
    bzero(userName, L7_LOGIN_SIZE);
    bzero(stat, L7_CLI_MAX_STRING_LENGTH);
    bzero(formattedEntry, L7_CLI_MAX_LARGE_STRING_LENGTH);

    if (sessionId > FD_WEB_DEFAULT_MAX_CONNECTIONS)
      sessionId = cliLoginSessionIndexGet();

    if (usmDbLoginSessionUserGet(0, sessionId, userName) == L7_SUCCESS)
    {
      /* Must deal with these cases earlier in the command processing */
      if (strcmp(userName, "NONE") == 0)
      {
    return L7_SUCCESS;
      }
      if (usmDbLoginSessionRemoteIpAddrGet(0, sessionId, &aval) == L7_SUCCESS)
      {
    if ( aval.family == L7_AF_INET )
    {
      if (usmDbInetNtoa(osapiNtohl(aval.addr.ipv4.s_addr), stat) != L7_SUCCESS)
        sprintf(stat,"%-s", "0.0.0.0");
    }
    else
    {
      sprintf(stat,"%-7s", "EIA-232");
    }
      }
      else
      {
    sprintf(stat,"%-s", "0.0.0.0");
      }
    }
    else
    {
      strcpy(userName, "<<UNKNOWN>>");
    }
    sprintf(formattedEntry, "CLI:%s:%s:", stat, userName);
    val = strlen(formattedEntry);
    strncat(formattedEntry, strInput, (L7_CLI_MAX_LARGE_STRING_LENGTH - val -1) );
    usmDbCmdLoggerEntryAdd(formattedEntry);
  }

  return L7_SUCCESS;
}
/*********************************************************************
 * @purpose  Set the Terminal Lines for show running-config
 *
 * @param    termLine   value of terminal lines for pagination <5-48>
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cliWebSetTerminalLine(L7_uint32 termLine)
{
  cliWebCfgData.linesForPagination = termLine;
  cliWebCfgData.cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Get the Terminal Lines for show running-config
 *
 * @param   *termLine   value of terminal lines for pagination <5-48>
 *
 * @returns L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cliWebGetTerminalLine(L7_uint32 *termLine)
{
  *termLine = cliWebCfgData.linesForPagination;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To change the listening port of the http server
*
* @param    port - port on which the http runs
* @param    apply- if port change needs to be applied immediately
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t cliWebHttpPortSet(L7_uint32 port,L7_BOOL apply)
{
  if ( port == cliWebCfgData.httpListenPortNum)
    return L7_SUCCESS;

  if (apply==L7_TRUE)
  {
    if (cliWebCfgData.systemWebMode==L7_ENABLE)
    {
      cliWebHttpPortUpdate(port);
      cliWebCfgData.httpListenPortNum= port;
      return L7_SUCCESS;
    }
    else
    {
      cliWebCfgData.httpListenPortNum= port;
      return L7_SUCCESS;
    }

  }
  else if (apply == L7_FALSE)
  {
    cliWebCfgData.httpListenPortNum= port;
    return L7_SUCCESS;

  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose To get the port over which the http is running
*
* @param    port -port value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t cliWebHttpPortGet(L7_uint32 *port)
{
  if (CLI_WEB_IS_READY)
  {
    *port= cliWebCfgData.httpListenPortNum;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  To change the listening port of the telnet server
*
* @param    port - port on which the telnet runs
* @param    apply- if port change needs to be applied immediately
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    apply doesn't have any effect right now. It is as good as
*           L7_FALSE. But it can be enhanced on the similar lines as
*           cliWebHttpPortNumSet
*
* @end
*********************************************************************/
L7_RC_t cliWebTelnetPortSet(L7_uint32 port,L7_BOOL apply)
{
  L7_RC_t rc = L7_SUCCESS;

  if ( port != cliWebCfgData.systemTelnetPortNum)
  {
    cliWebCfgData.systemTelnetPortNum = port;
    cliWebCfgData.cfgHdr.dataChanged = L7_TRUE;
    rc = L7_SUCCESS;
  }
  return rc;
}

/*********************************************************************
* @purpose To get the port over which the telnet is running
*
* @param    port -port value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t cliWebTelnetPortGet(L7_uint32 *port)
{
  if (CLI_WEB_IS_READY)
  {
   *port=  cliWebCfgData.systemTelnetPortNum;
   return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose Get the CPU Free Memory max value
*
* @param    void
*
* @returns  Free Memory max value
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 cliWebCpuFreeMemMaxGet(void)
{
  L7_uint32 maxThreshold=256, bytesTotal, bytesAlloc, bytesFree;

  if (osapiGetMemInfo(&bytesTotal, &bytesAlloc, &bytesFree) == L7_SUCCESS)
  {
    maxThreshold = bytesTotal/1024;
  }

  return maxThreshold;
}

