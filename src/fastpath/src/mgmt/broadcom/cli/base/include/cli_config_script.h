/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_config_script.h
 *
 * @purpose header for the cli_config_script.c
 *
 * @component user interface
 *
 * @comments
 *
 * @create  01/15/2004
 *
 * @author  Rahul Hak, Sarvesh Kumar
 * @end
 *
 **********************************************************************/

#ifndef CLI_CONFIG_SCRIPT_H
#define CLI_CONFIG_SCRIPT_H

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "config_script_api.h"

typedef enum
{
  CLI_CONFIG_CMD_SUCCESS = 100,
  CLI_CONFIG_CMD_NOT_FOUND,
  CLI_CONFIG_CMD_AMBIGIOUS,
  CLI_CONFIG_CMD_INVALID,
  CLI_CONFIG_CMD_SKIPPED
} L7_CLI_CFG_ERRTYPE_t;

/*******Functions for Configuration scripting*************************/
L7_RC_t cliExecuteBuffer( EwsContext context, L7_ConfigScript_t * configScriptData);
L7_RC_t ewsCliDataConfigScript(EwsContext context, L7_char8 * datap, EwsCliCommandP menu);
L7_RC_t ewsCliDataConfigScript2(EwsContext context, L7_char8 * datap, EwsCliCommandP menu, L7_int32 * skipExit);
L7_RC_t readDownloadedConfigScript(const L7_char8 * filename, const L7_char8 * newFilename,
                                   L7_ConfigScript_t * configScriptData) ;
L7_RC_t configScriptValidate(EwsContext ewsContext, L7_ConfigScript_t * configScriptData);
L7_RC_t configScriptValidateAndDownload(EwsContext ewsContext, L7_char8 * scriptFileName, L7_char8 * newFilename, L7_uint32 argc);
L7_RC_t IASUsersScriptValidateAndApply(L7_char8 * scriptFileName, L7_char8 * newFilename);

#endif
