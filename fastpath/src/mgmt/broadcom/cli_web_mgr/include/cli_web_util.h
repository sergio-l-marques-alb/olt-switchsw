
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

#ifndef CLI_WEB_MGR_UTIL_H
#define CLI_WEB_MGR_UTIL_H

#include "l7_common.h"
#include "default_cnfgr.h"
#include "defaultconfig.h"

#define CLI_WEB_CFG_VER_1      0x1
#define CLI_WEB_CFG_VER_2      0x2
#define CLI_WEB_CFG_VER_3      0x3
#define CLI_WEB_CFG_VER_4      0x4
#define CLI_WEB_CFG_VER_5      0x5
#define CLI_WEB_CFG_VER_6      0x6

#define CLI_WEB_CFG_VER_CURRENT    CLI_WEB_CFG_VER_6

#define CLI_WEB_CFG_FILENAME  "cliWebCfgData.cfg"

/****************************************
 *
 *  Cli Web Configuration Data
 *
 *****************************************/

typedef enum
{
  CLI_WEB_PHASE_INIT_0 = 0,
  CLI_WEB_PHASE_INIT_1,
  CLI_WEB_PHASE_INIT_2,
  CLI_WEB_PHASE_WMU,
  CLI_WEB_PHASE_INIT_3,
  CLI_WEB_PHASE_EXECUTE,
  CLI_WEB_PHASE_UNCONFIG_1,
  CLI_WEB_PHASE_UNCONFIG_2,
} cliWebCnfgrState_t;

#define CLI_WEB_IS_READY (((cliWebCnfgrState == CLI_WEB_PHASE_INIT_3) || \
      (cliWebCnfgrState == CLI_WEB_PHASE_EXECUTE) || \
      (cliWebCnfgrState == CLI_WEB_PHASE_UNCONFIG_1)) ? (L7_TRUE) : (L7_FALSE))

								     typedef struct
{
  L7_fileHdr_t   cfgHdr;
  L7_uint32      systemWebMode;                /* SIM_WEB_ENABLE or SIM_WEB_DISABLE */
  L7_uint32      webSessionHardTimeOut;
  L7_uint32      webSessionSoftTimeOut;
  L7_uint32      webNumSessions;
  L7_uint32      systemJavaMode;               /* for enable/disable of applet in web header */
  L7_char8       systemCommandPrompt[L7_PROMPT_SIZE];
  L7_uint32      systemTelnetNewSessions;
  L7_uint32      systemTelnetNumSessions;
  L7_uint32      systemTelnetAdminMode;
  L7_uint32      systemTelnetTimeout;
  L7_BOOL        userPrompt;
  L7_char8       cliBanner[BUF_BANNER_DISPLAY_SIZE];
  L7_uint32      linesForPagination;           /* Number of Lines for pagination */
  L7_uint32      systemTelnetPortNum;
  L7_uint32      httpListenPortNum;
  L7_uint32      checkSum;                     /* keep this as last 4 bytes */
} cliWebCfgData_t;

typedef struct
{
  L7_char8       loginName[L7_LOGIN_SIZE];
  L7_char8       password[L7_PASSWORD_SIZE];
  L7_uint32      accessMode;
  L7_uint32      loginStatus;
} logins_ver1_t;

typedef struct
{
  L7_fileHdr_t   cfgHdr;
  L7_uint32      systemWebMode;                /* SIM_WEB_ENABLE or SIM_WEB_DISABLE */
  L7_uint32      systemJavaMode;               /* for enable/disable of applet in web header */
  L7_char8       systemCommandPrompt[L7_PROMPT_SIZE];
  L7_uint32      systemTelnetNewSessions;
  L7_uint32      systemTelnetNumSessions;
  L7_uint32      systemTelnetTimeout;
  logins_ver1_t  systemLogins[L7_MAX_LOGINS];  /* FIELD REMOVED AFTER VER 1 */
  L7_BOOL        userPrompt;
  L7_uint32      checkSum;                     /* keep this as last 4 bytes */

} cliWebCfgData_ver1_t;

/*********************************************************************
 * @purpose  Saves cliWeb user config file to NVStore
 *
 * @param    void
 *
 * @returns  L7_SUCCESS or L7_FALIURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t cliWebSave(void);

/*********************************************************************
 * @purpose  Checks if cliWeb user config data is changed
 *
 * @param    void
 *
 * @returns  L7_TRUE
 * @returns  L7_FALSE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_BOOL cliWebHasDataChanged(void);
void cliWebResetDataChanged(void);

/*********************************************************************
 * @purpose  Build default cliWeb config data
 *
 * @param    ver   Software version of Config Data
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void cliWebBuildDefaultConfigData(L7_uint32 ver);

/*********************************************************************
 * @purpose  Migrate old config to new
 *
 * @param    ver   Expected software version of Config Data
 * @param    buffer Outdated config buffer
 *
 * @returns  void
 *
 * @notes    Current implementation resets to factory default.  Future
 *           version could move fields of prior version into new structure.
 *
 * @end
 *********************************************************************/
void cliWebMigrateConfigData(L7_uint32 oldVer, L7_uint32 ver, L7_char8 * buffer);

/*********************************************************************
 *
 * @purpose  Starts cliWeb task function
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void L7_cli_web_task (void);

/*********************************************************************
 *
 * @purpose  Starts cliWeb task function
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void L7_web_java_task (void);

/*********************************************************************
 * @purpose  cliWeb set to factory defaults
 *
 * @param    none
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void cliWebInitData(void);

/*********************************************************************
 * @purpose  initialize the util task
 *
 * @param    void
 *
 * @returns  int
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_int32 L7_util_task(void);

/*********************************************************************
 * @purpose Initialize the cliWeb for Phase 1
 *
 * @param   void
 *
 * @returns L7_SUCCESS  Phase 1 completed
 * @returns L7_FAILURE  Phase 1 incomplete
 *
 * @notes  If phase 1 is incomplete, it is up to the caller to call the fini
 *         function if desired.  If this fails, it is due to an inability to
 *         to acquire resources.
 *
 * @end
 *********************************************************************/
L7_RC_t cliWebPhaseOneInit(void);

/*********************************************************************
 * @purpose  Release all resources collected during phase 1
 *
 * @param    void
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void cliWebPhaseOneFini(void);

/*********************************************************************
 * @purpose Initialize the cliWeb for Phase 2
 *
 * @param   void
 *
 * @returns L7_SUCCESS  Phase 2 completed
 * @returns L7_FAILURE  Phase 2 incomplete
 *
 * @notes  If phase 2 is incomplete, it is up to the caller to call the fini
 *         function if desired
 *
 * @end
 *********************************************************************/
L7_RC_t cliWebPhaseTwoInit(void);

/*********************************************************************
 * @purpose Free the resource for phase 2
 *
 * @param   void
 *
 * @returns void
 *
 * @notes  If phase 2 is incomplete, it is up to the caller to call the fini
 *         function if desired
 *
 * @end
 *********************************************************************/
void cliWebPhaseTwoFini(void);

/*********************************************************************
 * @purpose Initialize the cliWeb for Phase 3
 *
 * @param   void
 *
 * @returns L7_SUCCESS  Phase 3 completed
 * @returns L7_FAILURE  Phase 3 incomplete
 *
 * @notes  If phase 3 is incomplete, it is up to the caller to call the fini
 *         function if desired
 *
 * @end
 *********************************************************************/
L7_RC_t cliWebPhaseThreeInit(void);

/*********************************************************************
 * @purpose Reset the cliWeb to prior to phase 3
 *
 * @param   void
 *
 * @returns void
 *
 * @notes  If phase 3 is incomplete, it is up to the caller to call the fini
 *         function if desired
 *
 * @end
 *********************************************************************/
void cliWebPhaseThreeFini(void);

/*********************************************************************
 *
 * @purpose convert the provided char array into a 32 bit unsigned integer
 *          the value is >= 0 and <= 4294967295.
 *
 * @param L7_char8 *buf, L7_uint32 * pVal
 *
 * @returns  L7_SUCCESS  means that all chars are integers and together
 *              they represent a valid 32 bit unsigned integer
 * @returns  L7_FAILURE  means the value does not represent a valid
 *              32 bit unsigned integer.  I.e. the value is negative, larger
 *              than the max allowed 32 bit int or a non-numeric character
 *              is included in buf.
 *
 * @notes This f(x) checks each letter inside the buf to make certain
 *         it is an integer.  It initially verifies the number of digits
 *         does not exceed the number of digits in Max 32 bit unsigned int.
 *         Start with the last digit, convert each ascii character into its
 *         integer equivalent.  Multiply each consecutive digit by the next
 *         power of 10.  Verify adding the new digit to the old sum will not
 *         exceed MAXINT.  If so, this indicates the value is too large to be
 *         represented by a 32 bit int.  If ok, add the new digit.
 *
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cliWebConvertTo32BitUnsignedInteger(const L7_char8 *buf, L7_uint32 *pVal);

/*********************************************************************
*
* @purpose convert the provided char array in HEX form into a 32 bit unsigned integer
*          the value is >= 0 and <= 0xffffffff.
*
* @param L7_char8 *buf, L7_uint32 * pVal
*
* @returns  L7_SUCCESS  means that all chars are HEX integers and together
*              they represent a valid 32 bit unsigned integer
* @returns  L7_FAILURE  means the value does not represent a valid
*              32 bit unsigned integer.  I.e. the value is negative, larger
*              than the max allowed 32 bit int or a non-hex character
*              is included in buf.
*
* @notes 
*
*
* @end
*
*********************************************************************/
L7_RC_t cliWebConvertHexTo32BitUnsignedInteger(const L7_char8 *buf, L7_uint32 *pVal);

/*********************************************************************
 *
 * @purpose convert the provided char array into a 32 bit signed integer
 *          the value is >= -2,147,483,648 and <= 2,147,483,647.
 *
 * @param L7_char8 *buf
 * @param L7_int32 * pVal
 *
 * @return  L7_SUCCESS  means that all chars are integers and together
 *              they represent a valid 32 bit signed integer
 * @return  L7_FAILURE  means the value does not represent a valid
 *              32 bit signed integer.  I.e. the value is outside of
 *              the valid range for a signed integer.
 *
 * @note This f(x) checks each letter inside the buf to make certain
 *       it is an integer.  It initially verifies the number of digits
 *       does not exceed the number of digits in Max 32 bit signed int.
 *       Start with the last digit, convert each ascii character into its
 *       integer equivalent.  Multiply each consecutive digit by the next
 *       power of 10.  Verify adding the new digit to the old sum will not
 *       exceed MAXINT.  If so, this indicates the value is too large to be
 *       represented by a 32 bit int.  If ok, add the new digit.
 *
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cliWebConvertTo32BitSignedInteger(const L7_char8 *buf, L7_int32 *pVal);

/*********************************************************************
 * @purpose  Converts a DSCP value to its associated keyword, if one
 *           is available.  Otherwise, a string of the numeric dscpVal
 *           input value is returned.
 *
 * @param    dscpVal         value to convert
 * @param    dscpString      string to return
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    Primarily used for QOS Diffserv.
 *
 * @end
 *********************************************************************/
L7_RC_t cliWebConvertDSCPValToString(L7_uint32 dscpVal, L7_char8* dscpString);

/*********************************************************************
 * @purpose  Get the global CLI/WEB access semaphore.
 *
 * @param    none
 *
 * @returns  none
 *
 * @notes    The caller blocks until the semaphore is available.
 *
 * @end
 *********************************************************************/
void cliWebAccessSemaGet(void);

/*********************************************************************
 * @purpose  Free the global CLI/WEB access semaphore.
 *
 * @param    none
 *
 * @returns  none
 *
 * @notes    The caller blocks until the semaphore is available.
 *
 * @end
 *********************************************************************/
void cliWebAccessSemaFree(void);

/*************************SERVICEABILITY**************************************/
#ifdef LVL7_DEBUG_BREAKIN
typedef struct consoleBreakinCfgData_s
{
  L7_char8  breakinString[L7_PASSWORD_SIZE];
  L7_uint32 consoleBreakinFlag;
}consoleBreakinCfgData_t;

typedef struct consoleBreakinCfg_s
{
  L7_fileHdr_t           hdr;
  consoleBreakinCfgData_t cfg;
  L7_uint32              checksum;
}consoleBreakinCfg_t;

/*********************************************************************
* @purpose  Register to general debug infrastructure
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void consoleBreakinRegister(void);

/*********************************************************************
* @purpose  Saves CONSOLE BREAKIN configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    consoleBreakinCfg is the overlay
*
* @end
*********************************************************************/
L7_RC_t consoleBreakinSave(void);

/*********************************************************************
* @purpose  Save configuration settings for console break-in
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t consoleBreakinConfigSave();

/*********************************************************************
* @purpose  Copy the configuration  settings to the config file
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void consoleBreakinCfgUpdate(void);

/*********************************************************************
* @purpose  Checks if console break-in config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL consoleBreakinHasDataChanged(void);

/*********************************************************************
* @purpose  Apply console break-in  config data
*
* @param    void
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    Called after a default config is built
*
* @end
*********************************************************************/
L7_RC_t consoleBreakinApplyConfigData(void);

/*********************************************************************
* @purpose  Read and apply the debug config
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void consoleBreakinCfgRead(void);

/*********************************************************************
* @purpose  Build default console break-in config data
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void consoleBreakinBuildDefaultConfigData(L7_uint32 ver);
#endif
/*********************************************************************
* @purpose  To change the listening port of the telnet server
*
* @param    port - port on which the telnet runs
*
* @returns  L7_SUCCESS
*           L7_ADDR_INUSE If failed to bind since address given is already in use
*           L7_FAILURE
*
* @notes    Closes the existing listening socket and opens the listening socket and puts it
*                into the Listening state.
*
* @end
*********************************************************************/
L7_RC_t cliWebTelnetPortUpdate(void);

/*********************************************************************
* @purpose  To change the listening port of the http server
*
* @param    port - port on which the http runs
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Closes the existing listening socet and opens the listening socket and puts it
*                into the Listening state.
*
* @end
*********************************************************************/
L7_RC_t cliWebHttpPortUpdate(L7_uint32 port);

#endif /* CLI_WEB_MGR_UTIL_H */
