/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 * @filename   cli_web_util.h
 *
 * @purpose    Cli Web Manager file
 *
 * @component  cliWebUI component
 *
 * @comments   none
 *
 * @create     03/07/2001
 *
 * @author     asuthan
 *
 * @end
 *
 **********************************************************************/

#ifndef CLI_WEB_MGR_API_H
#define CLI_WEB_MGR_API_H

#include "l7_common.h"
#include "l3_addrdefs.h"

#define WEB_CMD_LOGGER_AUDIT_ENTRY_ADD_EXT(wap, str)  

/* Begin Function Prototypes */

/*********************************************************************
 * @purpose  Returns if the CLI and Web are ready.
 *
 * @param    void
 *
 * @returns  L7_BOOL  L7_ENABLE is CLI/Web is ready
 *                    L7_DISABLE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 cliWebStateGet(void);

/*********************************************************************
 * @purpose  Returns the Unit's System Web Mode
 *
 * @param    void
 *
 * @returns  mode  System Web Mode
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 cliWebGetSystemWebMode(void);

/*********************************************************************
 * @purpose  Sets the Unit's System Web Mode
 *
 * @param    mode  System Web Mode
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void cliWebSetSystemWebMode(L7_uint32 mode);

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
L7_RC_t cliWebHttpNumSessionsSet(L7_uint32 val);

/*********************************************************************
 * @purpose  Get the maximum number of web sessions
 *
 * @returns  Return the maximum number of web sessions
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cliWebHttpNumSessionsGet(L7_uint32 *val);

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
L7_RC_t cliWebHttpSessionHardTimeOutSet(L7_uint32 val);

/*********************************************************************
 * @purpose  Get the http session hard timeout (in hours)
 *
 * @returns  Return the http session hard timeout
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cliWebHttpSessionHardTimeOutGet(L7_uint32 *val);

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
L7_RC_t cliWebHttpSessionSoftTimeOutSet(L7_uint32 val);

/*********************************************************************
 * @purpose  Get the http session soft timeout (in minutes)
 *
 * @returns  Return the http session soft timeout
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cliWebHttpSessionSoftTimeOutGet(L7_uint32 *val);

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
L7_uint32 cliWebGetJavaWebMode(void);

/*********************************************************************
 * @purpose  Sets the Unit's Web Java Mode
 *
 * @param    mode  Web Java Mode
 *
 * @returns  L7_uint32 L7_SUCCESS OR L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 cliWebSetJavaWebMode(L7_uint32 mode);

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
void cliWebGetSystemCommandPrompt(L7_char8 *prompt);

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
void cliWebSetSystemCommandPrompt(L7_char8 *prompt);

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
L7_uint32 cliWebGetLoginUserName(L7_uint32 index, L7_char8 *name);

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
L7_uint32 cliWebGetLoginIndex(L7_char8 *name, L7_uint32 *index);

/*********************************************************************
 * @purpose  Sets a login user name per index
 *
 * @param    index       table index
 * @param    commName    pointer to Community name
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    also sets login status to L7_enable
 *
 * @end
 *********************************************************************/
L7_uint32 cliWebSetLoginUserName(L7_uint32 index, L7_char8 *name);

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
L7_uint32 cliWebGetLoginUserPassword(L7_uint32 index, L7_char8 *password);

/*********************************************************************
 * @purpose  Sets a login user password per index
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
L7_uint32 cliWebSetLoginUserPassword(L7_uint32 index, L7_char8 *password);

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
L7_uint32 cliWebGetLoginUserStatus(L7_uint32 index, L7_uint32 *status);

/*********************************************************************
 * @purpose  Sets a login user password per index
 *
 * @param    index       table index
 * @param    password    pointer to login user password
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    can not change index 0, also removes user name and password
 *           when status is L7_DISABLE.  Currently L7_ENABLE has no effect.
 *
 * @end
 *********************************************************************/
L7_uint32 cliWebSetLoginUserStatus(L7_uint32 index, L7_uint32 status);

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
L7_uint32 cliWebGetSystemTelnetTimeout(void);

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
L7_RC_t cliWebSetSystemTelnetTimeout(L7_uint32 val);

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
L7_RC_t cliWebSetSystemTelnetNumSessions(L7_uint32 val);

/*********************************************************************
 * @purpose  Set the Admin Mode of Telnet Session
 *
 * @param unitIndex @b((input)) the unit for this operation
 *
 * @param    val   The number of Telnet Sessions
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbAgentTelnetAdminModeSet(L7_uint32 unitIndex, L7_uint32 mode);

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
L7_RC_t usmDbAgentTelnetAdminModeGet(L7_uint32 unitIndex, L7_uint32 *mode);

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
L7_uint32 cliWebGetSystemTelnetNumSessions(void);

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
L7_RC_t cliWebSetSystemTelnetNewSessions(L7_uint32 val);

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
L7_uint32 cliWebGetSystemTelnetNewSessions(void);

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
L7_uint32 cliWebSetLoginUserAuthentication(L7_uint32 index, L7_uint32 authProt);


/*********************************************************************
 * @purpose  Sets a login user encryption protocol and key
 *
 * @param    index       table index
 * @param    authProt    encryption protocol (none, md5, or sha)
 * @param    authKey     encryption key (ignored if prot = none)
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 cliWebSetLoginUserEncryption(L7_uint32 index, L7_uint32 encryptProt,
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
L7_uint32 cliWebSetLoginUserAccessLevel(L7_uint32 index, L7_uint32 accessLevel);

/*********************************************************************
 * @purpose  Return a login user authentication protocol
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
L7_uint32 cliWebGetLoginUserAuthenticationProt(L7_uint32 index, L7_uint32 *authProt);

/*********************************************************************
 * @purpose  Returns a login user encryption protocol
 *
 * @param    index       table index
 * @param    encryptProt (output) encryption protocol (none, md5, or sha)
 * @param    encryptKey  (output) encryption key (ignored if prot = none)
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 cliWebGetLoginUserEncryptionProt(L7_uint32 index, L7_uint32 *encryptProt);

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
L7_uint32 cliWebGetLoginUserAccessLevel(L7_uint32 index, L7_uint32 *accessLevel);

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
L7_uint32 cliWebDeleteLoginUser(L7_uint32 index);

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
L7_RC_t cliWebSystemReadBannerFile(L7_char8 *file_name);

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
L7_RC_t cliWebSystemCopyBanner(L7_char8 *buff);

/*********************************************************************
 * @purpose  write the banner config structure to a file
 *
 * @param    outputFileName    filename
 *
 * @returns
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cliWriteBannerFile(L7_char8 *outputFileName);

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
L7_BOOL cliWebDebugTraceDisplayModeGet(L7_int32 session);

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
 * @end
 *********************************************************************/
L7_RC_t cliWebDebugTraceDisplayModeSet(L7_BOOL enable_flag, L7_int32 session);

/*********************************************************************
 * @purpose  Enable display of trace output on the current session.
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
L7_RC_t cliWebSupportConsoleDisplayModeSet(L7_BOOL enable_flag, L7_int32 session);

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
void cliWebConsoleLog(L7_uchar8 *buf);

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
L7_RC_t cliWebCmdLoggerEntryAdd(L7_char8 *strInput, L7_uint32 sessionId);

/*********************************************************************
* @purpose  Add an entry for auditing
*
* @param    strInput   input string
* @param    uName      username
* @param    ipAddr     remote IP address string
*
* @returns  nothing
*
* @notes
*
* @end
*********************************************************************/
void cliCmdLoggerAuditEntryAdd(L7_char8 *uName,
                               L7_char8 *ipAddr,
                               const L7_char8 *strInput);

/*********************************************************************
* @purpose  Add an entry for auditing
*
* @param    strInput       input string
* @param    sessionId      session id
*
* @returns  nothing
*
* @notes
*
* @end
*********************************************************************/
void cliCmdLoggerAuditEntryAddExt(L7_uint32 sessionId, const L7_char8 *strInput);

/*********************************************************************
* @purpose  Add an entry for web session logging
*
* @param    strInput       input string
* @param    sessionId      session id
*
* @returns  nothing
*
* @notes
*
* @end
*********************************************************************/
void webCmdLoggerAuditEntryAdd(L7_char8 *uName,
                               L7_inet_addr_t inetAddr,
                               L7_char8 *strInput);

/*********************************************************************
* @purpose  Add an entry for web session logging
*
* @param    strInput  input string
* @param    wap       pointer
*
* @returns  nothing
*
* @notes
*
* @end
*********************************************************************/
void webCmdLoggerAuditEntryAddExt(void *wap, L7_char8 *strInput);

/*********************************************************************
 * @purpose  Helper function for retrieving transfer info during 
 *           upload/download file to/from the switch
 *
 * @param    isUpload   determine if upload/download
 *                      transfer info should be retrieved
 * @param    outputStr  pointer to string where transfer info 
 *                      will be stored
 * @param    size       maximum size of outputStr
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cliWebTransferInfoGet(L7_BOOL isUpload, L7_char8 *outputStr, L7_uint32 size);

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
L7_RC_t cliWebSetTerminalLine(L7_uint32 termLine);

/*********************************************************************
 * @purpose  Get the Terminal Lines for show running-config
 *
 * @param   *termLine   value of terminal lines for pagination <5-48>
 *
 * @returns termLine   L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cliWebGetTerminalLine(L7_uint32 *termLine);

/*********************************************************************
* @purpose  Enable display of trace output on the current session.
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
L7_RC_t cliWebConsoleTraceDisplayEnable(L7_BOOL enable_flag, L7_int32 session);

#ifdef LVL7_DEBUG_BREAKIN
/*********************************************************************
* @purpose  Sets  console breakin string.
*
* @param    index       table index
* @param    password    pointer to password
*
* @returns  L7_SUCCESS
*           L7_FAILURE if password is too long or index is too large
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t cliWebConsoleBreakinstringSet(L7_char8 *password);

/*********************************************************************
* @purpose  Gets the console breakin string
*
* @param    index       table index
* @param    password    pointer to password
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t cliWebConsoleBreakinstringGet(L7_char8 *password);

/***********************************************************************
* @purpose  Enable/Disable console break-in
*
* @param    enable_flag - L7_TRUE.
*                         L7_FALSE.
* @param    session     - CLI login session
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t cliWebConsoleBreakinModeSet(L7_BOOL enable_flag);

/*********************************************************************
* @purpose  Gets the console break-in mode setting for selected session.
*
*
*
* @returns  L7_TRUE -  display of Debug Console Mode Enabled.
*           L7_FALSE - display of Debug Console Mode Disabled.
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t cliWebConsoleBreakinModeGet(void);
#endif  /* LVL7_DEBUG_BREAKIN */

/*********************************************************************
* @purpose  To change the listening port of the http server
*
* @param    port - port on which the http runs
* @param    apply- if port change needs to be applied immediately
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Primarily used for QOS Diffserv.
*
* @end
*********************************************************************/
L7_RC_t cliWebHttpPortSet(L7_uint32 port, L7_BOOL apply);

/*********************************************************************
* @purpose  To change the listening port of the telnet server
*
* @param    port - port on which the telnet runs
* @param    apply- if port change needs to be applied immediately
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t cliWebTelnetPortSet(L7_uint32 port, L7_BOOL apply);

/*********************************************************************
* @purpose To get the port over which the http is running
*
* @param    port -port value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Primarily used for QOS Diffserv.
*
* @end
*********************************************************************/
L7_RC_t cliWebHttpPortGet(L7_uint32 *port);

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
L7_RC_t cliWebTelnetPortGet(L7_uint32 *port);
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
L7_RC_t cliWebewaSessionRemoveAll(L7_uint32 sessionType);

/*********************************************************************
*
* @purpose Remove the session indicated by the supplied index
*
* @end
*
*********************************************************************/
L7_RC_t cliWebewaSessionRemoveByIndex(L7_uint32 sessionIndex);

#if L7_FEAT_BANNER_MOTD
/*********************************************************************
* @purpose  Sets the banner
*
* @param    L7_uint32       UnitIndex unit number
* @param    L7_char8 *      buffer    pointer to banner buffer
*
* @returns L7_SUCCESS
*          L7_FAILURE
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t cliWebBannerSet(L7_uint32 UnitIndex, L7_char8 *buffer);

/*********************************************************************
* @purpose  Gets the banner
*
* @param    L7_uint32       UnitIndex unit number
* @param    L7_char8 *      buffer    pointer to banner buffer
*
* @returns L7_SUCCESS
*          L7_FAILURE
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t cliWebBannerGet(L7_uint32 UnitIndex, L7_char8 *buffer);

/*********************************************************************
* @purpose  Sets the banner ack
*
* @param    L7_uint32       UnitIndex unit number
* @param    L7_BOOL      val banner ack value
*
* @returns L7_SUCCESS
*          L7_FAILURE
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t cliWebBannerAckSet(L7_uint32 UnitIndex, L7_BOOL val);

/*********************************************************************
* @purpose  Gets the banner ack
*
* @param    L7_uint32       UnitIndex unit number
* @param    L7_BOOL      val banner ack value
*
* @returns L7_SUCCESS
*          L7_FAILURE
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t cliWebBannerAckGet(L7_uint32 UnitIndex, L7_BOOL *val);

/*********************************************************************
* @purpose  Returns the CLI Banner Acknowledge status
*
* @param    void
*
* @returns  
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL cliWebGetCliBannerAck(void);
#endif

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
L7_uint32 cliWebCpuFreeMemMaxGet(void);

/* End Function Prototypes */
#endif /* CLI_WEB_MGR_API_H */
