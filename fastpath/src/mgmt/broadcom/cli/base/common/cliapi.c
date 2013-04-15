/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cliapi.c
 *
 * @purpose api for the private cli structures
 *
 * @component user interface
 *
 * @comments this is to allow for easy access to the cliCommon
 * @comments and the cliUtil structures as defined in cli.h
 *
 * @create  04/02/2001
 *
 * @author  fsamuels
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"

#include "usmdb_unitmgr_api.h"

/* cliUtil */

L7_uint32 cliCurrentHandleGet(void)
{
  return cliUtil.handleNum;
}

void cliCurrentHandleSet(L7_uint32 handle)
{
  if (cliUtil.handleNum < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    cliUtil.handleNum = handle;
  }
}

L7_uint32 cliDevShellHandleGet(void)
{
  return cliUtil.devshellHandleNum;
}

void cliDevShellHandleSet(L7_uint32 handle)
{
  cliUtil.devshellHandleNum = handle;
}

L7_BOOL cliIoRedirectToCliGet(void)
{
  return cliUtil.ioRedirectedToCLI;
}

void cliIoRedirectToCliSet(L7_BOOL mode)
{
  cliUtil.ioRedirectedToCLI = mode;
}

L7_char8 *cliSystemPromptGet(void)
{
  return cliUtil.systemPrompt;
}

void cliSystemPromptSet(L7_char8 * sysPrompt)
{
  osapiStrncpySafe(cliUtil.systemPrompt, sysPrompt, sizeof(cliUtil.systemPrompt));
}

void cliLoginUserPromptSet(L7_char8 * prompt)
{
  osapiStrncpySafe(cliUtil.loginUserPrompt, prompt, sizeof(cliUtil.loginUserPrompt));
}

L7_char8 *cliTrapMsgStringGet(void)
{
  return cliUtil.trapMsgString;
}

void cliTrapMsgStringSet(L7_char8 * trapString)
{
  osapiStrncpySafe(cliUtil.trapMsgString, trapString, sizeof(cliUtil.trapMsgString));

  /* initialize `trapMsgDisplay` as the trap message is to be displayed only once */
  cliUtil.trapMsgDisplay = L7_TRUE;
}

L7_BOOL cliTrapMsgStatusGet(void)
{
  return cliUtil.trapMsgDisplay;
}

void cliTrapMsgStatusSet(L7_BOOL val)
{
  cliUtil.trapMsgDisplay = val;
}

L7_BOOL cliResetSerialConnectionGet(void)
{
  return cliUtil.resetSerialConnection;
}

void cliResetSerialConnectionSet(L7_BOOL value)
{
  cliUtil.resetSerialConnection = value;
}

/* cliCommon */

L7_BOOL cliTelnetConnectionGet(void)
{
  if (cliCurrentHandleGet() < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    return cliCommon[cliCurrentHandleGet()].telnetConnection;
  }
  return L7_FALSE;
}

void cliTelnetConnectionSet(L7_BOOL value)
{
  if (cliCurrentHandleGet() < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    cliCommon[cliCurrentHandleGet()].telnetConnection = value;
  }
}

L7_BOOL cliScrollGet()
{
  if (cliCurrentHandleGet() < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    return cliCommon[cliCurrentHandleGet()].scroll;
  }
  return L7_FALSE;
}

void cliScrollSet(L7_BOOL value)
{
  if (cliCurrentHandleGet() < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    cliCommon[cliCurrentHandleGet()].scroll = value;
  }
}

L7_BOOL cliHaveUserGet(void)
{
  if (cliCurrentHandleGet() < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    return cliCommon[cliCurrentHandleGet()].haveUser;
  }
  return L7_FALSE;
}

void cliHaveUserSet(L7_BOOL value)
{
  if (cliCurrentHandleGet() < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    cliCommon[cliCurrentHandleGet()].haveUser = value;
  }
}

L7_uint32 cliPrevDepthGet(void)
{
  if (cliCurrentHandleGet() < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    return cliCommon[cliCurrentHandleGet()].prevDepth;
  }
  return 0;
}

void cliPrevDepthSet(L7_uint32 depth)
{
  if (cliCurrentHandleGet() < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    cliCommon[cliCurrentHandleGet()].prevDepth = depth;
  }
}

L7_uint32 cliNumFunctionArgsGet(void)
{
  if (cliCurrentHandleGet() < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    return cliCommon[cliCurrentHandleGet()].functionArguments;
  }
  return 0;
}

void cliNumFunctionArgsSet(L7_uint32 numberOfArgs)
{
  if (cliCurrentHandleGet() < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    cliCommon[cliCurrentHandleGet()].functionArguments = numberOfArgs;
  }
}

L7_uint32 cliUserAccessGet(void)
{
  if (cliCurrentHandleGet() < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    return cliCommon[cliCurrentHandleGet()].userAccess;
  }
  return 0;
}

void cliUserAccessSet(L7_uint32 access)
{
  if (cliCurrentHandleGet() < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    cliCommon[cliCurrentHandleGet()].userAccess = access;
  }
}

/*
 * Map the deprecated userNumberGet function which was previously used
 * to identify the admin user to the new adminUserFlagGet function.
 */
L7_uint32 cliUserNumberGet(void)
{
  if (cliCurrentHandleGet() < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    if ( cliCommon[cliCurrentHandleGet()].adminUserFlag == L7_TRUE )
    {
      return 0;
    }
  }
  return 1;
}

L7_BOOL cliAdminUserFlagGet(void)
{
  if (cliCurrentHandleGet() < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    return cliCommon[cliCurrentHandleGet()].adminUserFlag;
  }
  return L7_FALSE;
}

void cliAdminUserFlagSet(L7_BOOL flag)
{
  if (cliCurrentHandleGet() < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    cliCommon[cliCurrentHandleGet()].adminUserFlag = flag;
  }
}

L7_uint32 cliLoginSessionIndexGet(void)
{
  if (cliCurrentHandleGet() < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    return cliCommon[cliCurrentHandleGet()].userLoginSessionIndex;
  }
  return 0;
}

void cliLoginSessionIndexSet(L7_uint32 index)
{
  if (cliCurrentHandleGet() < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    cliCommon[cliCurrentHandleGet()].userLoginSessionIndex = index;
  }
}

L7_uint32 cliScrollLineGet(void)
{
  if (cliCurrentHandleGet() < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    return cliCommon[cliCurrentHandleGet()].scrollLine;
  }
  return 0;
}

void cliScrollLineSet(L7_uint32 line)
{
  if (cliCurrentHandleGet() < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    cliCommon[cliCurrentHandleGet()].scrollLine = line;
  }
}

L7_uint32 cliScrollLineMaxGet()
{
  if (cliCurrentHandleGet() < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    return cliCommon[cliCurrentHandleGet()].scrollLineMax;
  }
  return 0;
}

void cliScrollLineMaxSet(L7_uint32 scrollMax)
{
  if (cliCurrentHandleGet() < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    cliCommon[cliCurrentHandleGet()].scrollLineMax = scrollMax;
  }
}

L7_uint32 cliCurrentSocketGet(void)
{
  if (cliCurrentHandleGet() < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    return cliCommon[cliCurrentHandleGet()].socket;
  }
  return 0;
}

void cliCurrentSocketSet(L7_uint32 socket)
{
  if (cliCurrentHandleGet() < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    cliCommon[cliCurrentHandleGet()].socket = socket;
  }
}

L7_char8 *cliCurrentPromptGet(void)
{
  if (cliCurrentHandleGet() < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    return cliCommon[cliCurrentHandleGet()].prompt;
  }
  return L7_NULL;
}

void cliCurrentPromptSet(L7_char8 * prompt)
{
  L7_uint32 hand = cliCurrentHandleGet();

  if (hand < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    osapiStrncpySafe(cliCommon[hand].prompt, prompt, sizeof(cliCommon[hand].prompt));
  }
}

L7_char8 *cliAlternateCommandGet(void)
{
  if (cliCurrentHandleGet() < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    return cliCommon[cliCurrentHandleGet()].command;
  }
  return L7_NULL;
}

void cliAlternateCommandSet(L7_char8 * cmd)
{
  L7_uint32 hand = cliCurrentHandleGet();

  if (hand < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    osapiStrncpySafe(cliCommon[hand].command, cmd, sizeof(cliCommon[hand].command));
  }
}

L7_char8 *cliPasswordGet(void)
{
  L7_uint32 hand = cliCurrentHandleGet();

  if (hand < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    return cliCommon[hand].password;
  }
  return L7_NULL;
}

void cliPasswordSet(L7_char8 * pass)
{
  L7_uint32 hand = cliCurrentHandleGet();

  if (hand < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    osapiStrncpySafe(cliCommon[hand].password, pass, sizeof(cliCommon[hand].password));
  }
}

L7_BOOL cliDevShellActiveGet(void)
{
  if (cliCurrentHandleGet() < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    return cliCommon[cliCurrentHandleGet()].devshellActive;
  }
  return L7_FALSE;
}

void cliDevShellActiveset(L7_BOOL value)
{
  if (cliCurrentHandleGet() < FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    cliCommon[cliCurrentHandleGet()].devshellActive = value;
  }
}

L7_BOOL cliIsCurrUnitMgmtUnit(void)
{
  L7_uint32 curr_unit = 0;
  L7_uint32 mgmt_unit = 0;

  if (usmDbUnitMgrMgrNumberGet(&mgmt_unit) == L7_SUCCESS &&
      usmDbUnitMgrNumberGet(&curr_unit) == L7_SUCCESS)
  {
    if (mgmt_unit == curr_unit)
    {
      return L7_TRUE;
    }
  }
  return L7_FALSE;
}
