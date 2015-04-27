/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2006-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_show_running_config_util.c
 *
 * @purpose show running config helper functions
 *
 * @component user interface
 *
 * @comments
 *
 * @create  12/13/2006
 *
 * @author  Rama Sasthri, Kristipati
 *
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"

#include "osapi_support.h"
#include "usmdb_util_api.h"
#include "cli_web_exports.h"

/*********************************************************************
* @purpose  To clean up the show running config
* @prints the command if Enable
*
* @param    EwsContext ewsContext
* @param    L7_uint32 val
* @param    L7_uint32 def
* @param    L7_char8 * cmd
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
   @end
*********************************************************************/

L7_RC_t cliShowCmdEnable (EwsContext context, L7_uint32 val, L7_uint32 def, L7_char8 * cmd)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  if (val != def || EWSSHOWALL (context) == L7_SHOW_DEFAULT)
  {
    if (val == L7_ENABLE)
    {
      osapiSnprintf (buf, sizeof (buf), "\r\n%s", cmd);
    }
    else
    {
      osapiSnprintf (buf, sizeof (buf), "\r\n%s %s", pStrInfo_common_No_1, cmd);
    }
    EWSWRITEBUFFER (context, buf);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To clean up the show running config
* @prints the command if Boolean
*
* @param    EwsContext ewsContext
* @param    L7_uint32 val
* @param    L7_char8 * cmd
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
   @end
*********************************************************************/

L7_RC_t cliShowCmdBoolean (EwsContext context, L7_uint32 val, L7_char8 * cmd)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  if (val != 0)
  {
    osapiSnprintf (buf, sizeof (buf), "\r\n%s", cmd);
  }
  else
  {
    osapiSnprintf (buf, sizeof (buf), "\r\n%s %s", pStrInfo_common_No_1, cmd);
  }
  EWSWRITEBUFFER (context, buf);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To clean up the show running config
* @prints the command if true
*
* @param    EwsContext ewsContext
* @param    L7_uint32 val
* @param    L7_uint32 def
* @param    L7_char8 * cmd
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
   @end
*********************************************************************/

L7_RC_t cliShowCmdTrue (EwsContext context, L7_uint32 val, L7_uint32 def, L7_char8 * cmd)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  if (val != def || EWSSHOWALL (context) == L7_SHOW_DEFAULT)
  {
    if (val == L7_TRUE)
    {
      osapiSnprintf (buf, sizeof (buf), "\r\n%s", cmd);
    }
    else
    {
      osapiSnprintf (buf, sizeof (buf), "\r\n%s %s", pStrInfo_common_No_1, cmd);
    }
    EWSWRITEBUFFER (context, buf);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To clean up the show running config
* @prints the command with integer
*
* @param    EwsContext ewsContext
* @param    L7_uint32 val
* @param    L7_uint32 def
* @param    L7_char8 * cmd
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
   @end
*********************************************************************/

L7_RC_t cliShowCmdInt (EwsContext context, L7_uint32 val, L7_uint32 def, L7_char8 * cmd)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  if (val != def)
  {
    osapiSnprintf (buf, sizeof (buf), "\r\n%s %u", cmd, val);
    EWSWRITEBUFFER (context, buf);
  }
  else if (EWSSHOWALL (context) == L7_SHOW_DEFAULT)
  {
    osapiSnprintf (buf, sizeof (buf), "\r\n%s %s", pStrInfo_common_No_1, cmd);
    EWSWRITEBUFFER (context, buf);
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  To clean up the show running config
* @prints the command with integer  and additional interger
*
* @param    EwsContext ewsContext
* @param    L7_uint32 val
* @param    L7_uint32 def
* @param    L7_char8 * cmd
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
   @end
*********************************************************************/

L7_RC_t cliShowCmdIntAddValue (EwsContext context, L7_uint32 val, L7_uint32 def, L7_char8 * cmd, L7_uint32 val2)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  if (val != def)
  {
    osapiSnprintf (buf, sizeof (buf), "\r\n%s %u", cmd, val2);
    EWSWRITEBUFFER (context, buf);
  }
  else if (EWSSHOWALL (context) == L7_SHOW_DEFAULT)
  {
    osapiSnprintf (buf, sizeof (buf), "\r\n%s %s %u", pStrInfo_common_No_1, cmd, val2);
    EWSWRITEBUFFER (context, buf);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To clean up the show running config
* @prints the command with integers
*
* @param    EwsContext ewsContext
* @param    L7_uint32 val
* @param    L7_uint32 def
* @param    L7_char8 * cmd
* @param    L7_uint32 add
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
   @end
*********************************************************************/

L7_RC_t cliShowCmdIntAddInt (EwsContext context, L7_uint32 val, L7_uint32 def, L7_char8 * cmd, L7_uint32 add)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  if (val != def)
  {
    osapiSnprintf (buf, sizeof (buf), "\r\n%s %u %u", cmd, add, val);
    EWSWRITEBUFFER (context, buf);
  }
  else if (EWSSHOWALL (context) == L7_SHOW_DEFAULT)
  {
    osapiSnprintf (buf, sizeof (buf), "\r\n%s %s", pStrInfo_common_No_1, cmd);
    EWSWRITEBUFFER (context, buf);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To clean up the show running config
* @prints the IpAddress
*
* @param    EwsContext ewsContext
* @param    L7_uchar8 family L7_AF_INET L7_AF_INET6
* @param    void *val 
* @param    void *def
* @param    L7_char8 * cmd
* @param    L7_uint32 add
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
   @end
*********************************************************************/
L7_RC_t cliShowCmdIpAddr (EwsContext context, L7_uchar8 family, void *val,
                          void *def, L7_char8 * cmd)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL  diff = L7_FALSE;
  L7_in6_addr_t  nullIp6Ip;

  if (val == L7_NULLPTR)
   return L7_FAILURE;

  if (family == L7_AF_INET)
  {
    if (def == L7_NULLPTR)
    {
      if (*((L7_uint32 *)val) != 0)
      {
        diff = L7_TRUE;
      }
    }
    else
    {
      if (*((L7_uint32 *)val) != *((L7_uint32 *)def))
      {
        diff = L7_TRUE;
      }
    }
  }
  else if (family == L7_AF_INET6)
  {
    if (def == L7_NULLPTR)
    {
      memset(&nullIp6Ip, 0, sizeof(L7_in6_addr_t));
      if (memcmp(val, &nullIp6Ip, sizeof(L7_in6_addr_t)))
      {
        diff = L7_TRUE;
      }
    }
    else
    {
      if (memcmp(val, def, sizeof(L7_in6_addr_t)))
      {
        diff = L7_TRUE;
      }
    }
  }
  else
  {
    return L7_FAILURE;
  }

  if (diff == L7_TRUE)
  {
    if (family == L7_AF_INET)
    {
      usmDbInetNtoa (*((L7_uint32 *)val), stat);
    }
    else
    {
      osapiInetNtop(L7_AF_INET6, val, stat, L7_CLI_MAX_STRING_LENGTH);
    }

    osapiSnprintf (buf, sizeof (buf), "\r\n%s %s", cmd, stat);
    EWSWRITEBUFFER (context, buf);
  }
  else if (EWSSHOWALL (context) == L7_SHOW_DEFAULT)
  {
    osapiSnprintf (buf, sizeof (buf), "\r\n%s %s", pStrInfo_common_No_1, cmd);
    EWSWRITEBUFFER (context, buf);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To clean up the show running config
* @prints the command with string
*
* @param    EwsContext ewsContext
* @param    L7_uint32 val
* @param    L7_uint32 def
* @param    L7_char8 * cmd
* @param    L7_uint32 add
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
   @end
*********************************************************************/

L7_RC_t cliShowCmdStr (EwsContext context, L7_uint32 val, L7_uint32 def, L7_char8 * cmd,
                       L7_char8 * str)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  if (val != def || (EWSSHOWALL (context) == L7_SHOW_DEFAULT))
  {
    if (val == L7_ENABLE)
    {
      osapiSnprintf (buf, sizeof (buf), "\r\n%s \"%s\"", cmd, str);
    }
    else
    {
      osapiSnprintf (buf, sizeof (buf), "\r\n%s %s \"%s\"", pStrInfo_common_No_1, cmd, str);
    }
    EWSWRITEBUFFER (context, buf);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To clean up the show running config
* @prints the Quoted Strings
*
* @param    EwsContext ewsContext
* @param    L7_uint32 val
* @param    L7_uint32 def
* @param    L7_char8 * cmd
* @param    L7_uint32 add
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
   @end
*********************************************************************/

L7_RC_t cliShowCmdQuotedStrcmp (EwsContext context, L7_char8 * val, L7_char8 * def, L7_char8 * cmd)
{
  L7_char8 buf[L7_CLI_MAX_LARGE_STRING_LENGTH];

  if ((EWSSHOWALL (context) == L7_SHOW_DEFAULT))
  {
    osapiSnprintf (buf, sizeof (buf), "\r\n%s %s \"%s\"", pStrInfo_common_No_1, cmd, val);
    EWSWRITEBUFFER (context, buf);
  }
  else if (strcmp (val, def) != 0)
  {
    osapiSnprintf (buf, sizeof (buf), "\r\n%s \"%s\"", cmd, val);
    EWSWRITEBUFFER (context, buf);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To clean up the show running config
* @prints the command if Disable
*
* @param    EwsContext ewsContext
* @param    L7_uint32 val
* @param    L7_uint32 def
* @param    L7_char8 * cmd
* @param    L7_uint32 add
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
   @end
*********************************************************************/

L7_RC_t cliShowCmdDisable (EwsContext context, L7_uint32 val, L7_uint32 def, L7_char8 * cmd)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  if (val != def || EWSSHOWALL (context) == L7_SHOW_DEFAULT)
  {
    if (val == L7_DISABLE)
    {
      osapiSnprintf (buf, sizeof (buf), "\r\n%s", cmd);
    }
    else
    {
      osapiSnprintf (buf, sizeof (buf), "\r\n%s %s", pStrInfo_common_No_1, cmd);
    }
    EWSWRITEBUFFER (context, buf);
  }
  return L7_SUCCESS;
}
