
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
* @filename cli_web_migrate.h
*
* @purpose Cli Web Manager Configuration Migration
*
* @component cliWebUI
*
* @comments none
*
* @create 8/23/2004
*
* @author Rama Sasthri, Kristipati
*
* @end
*
**********************************************************************/

#ifndef CLI_WEB_MIGRATE_H
#define CLI_WEB_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"
#include "cli_web_include.h"

typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_uint32 systemWebMode;      /* SIM_WEB_ENABLE or SIM_WEB_DISABLE */
  L7_uint32 systemJavaMode;     /* for enable/disable of applet in web header */
  L7_char8 systemCommandPrompt[L7_PROMPT_SIZE];
  L7_uint32 systemTelnetNewSessions;
  L7_uint32 systemTelnetNumSessions;
  L7_uint32 systemTelnetTimeout;
  L7_BOOL userPrompt;
  L7_uint32 checkSum;           /* keep this as last 4 bytes */

} cliWebCfgDataV2_t;

typedef struct
{
  L7_fileHdr_t   cfgHdr;
  L7_uint32      systemWebMode;                /* SIM_WEB_ENABLE or SIM_WEB_DISABLE */
  L7_uint32      systemJavaMode;               /* for enable/disable of applet in web header */
  L7_char8       systemCommandPrompt[L7_PROMPT_SIZE];
  L7_uint32      systemTelnetNewSessions;
  L7_uint32      systemTelnetNumSessions;
  L7_uint32      systemTelnetTimeout;
  L7_BOOL        userPrompt;
  L7_char8       cliBanner[BUF_BANNER_DISPLAY_SIZE];
  L7_uint32      checkSum;                     /* keep this as last 4 bytes */

} cliWebCfgDataV3_t;

typedef struct
{
  L7_fileHdr_t   cfgHdr;
  L7_uint32      systemWebMode;                /* SIM_WEB_ENABLE or SIM_WEB_DISABLE */
  L7_uint32      systemJavaMode;               /* for enable/disable of applet in web header */
  L7_char8       systemCommandPrompt[L7_PROMPT_SIZE];
  L7_uint32      systemTelnetNewSessions;
  L7_uint32      systemTelnetNumSessions;
  L7_uint32      systemTelnetAdminMode;
  L7_uint32      systemTelnetTimeout;
  L7_BOOL        userPrompt;
  L7_char8       cliBanner[BUF_BANNER_DISPLAY_SIZE];
  L7_uint32      checkSum;                     /* keep this as last 4 bytes */

} cliWebCfgDataV4_t;

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
  L7_uint32      checkSum;                     /* keep this as last 4 bytes */

} cliWebCfgDataV5_t;

typedef cliWebCfgData_t cliWebCfgDataV6_t;

#endif /* CLI_WEB_MIGRATE_H */

