/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
 * @filename src/mgmt/cli/security/ssh/cli_show_ssh.c
*
* @purpose show commands for the cli
*
* @component user interface
*
* @comments
*
* @create  09/16/2003
*
* @author  Kim Mans
* @end
*
**********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_security_common.h"
#include "strlib_security_cli.h"
#include "cliapi.h"
#include "datatypes.h"

#include "clicommands_ssh.h"
#include "default_cnfgr.h"
#include "usmdb_sshd_api.h"
#include "cli_web_exports.h"
#include "cli.h"
#include "ews.h"
#include "sshd_api.h"
#include "cliutil.h"
#include "sshd_exports.h"

/*********************************************************************
*
* @purpose Displays Arp table information
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const char **argv
* @param uintf index
*
* @returntype const char  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax    show ip ssh
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowIpSsh(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{

   L7_uint32 unit;   L7_uint32 numArg;        /* New variable Added */
   L7_uint32 sshAdminMode = L7_DISABLE;
   L7_uint32 sshProtoMode = L7_DISABLE;
   L7_uchar8 strSshAdminMode[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 strSshProtoMode[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8 stat[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 val;

   cliSyntaxTop(ewsContext);
   unit = cliGetUnitId();

   numArg = cliNumFunctionArgsGet();

   if ( numArg != 0 )
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_security_ShowSshCon);
   }
   /* -- SSH TELNET REMOVED------

   ewsTelnetWrite(ewsContext, "\r\nAdmin Mode  Protocol Level(s)  SSH Sessions active  Max SSH Sessions allowed");
   ewsTelnetWrite(ewsContext, "\r\n----------  -----------------  -------------------  ------------------------\r\n");

   bzero(strSshAdminMode,sizeof(strSshAdminMode));
   if (usmDbsshdAdminModeGet(unit, &sshAdminMode) == L7_SUCCESS)
   {
      if (sshAdminMode == L7_ENABLE)
         sprintf(strSshAdminMode,"%-21.15s",CLIENABLED);
      else if (sshAdminMode == L7_DISABLE)
         sprintf(strSshAdminMode,"%-21.15s",CLIDISABLED);
      else
         sprintf(strSshAdminMode,"%-21.15s",CLILINE);
   }
   else
     ewsTelnetPrintf (ewsContext, "%-21.15s",CLILINE);

   bzero(strSshProtoMode,sizeof(strSshProtoMode));
   if (usmDbsshdProtoLevelGet(unit, &sshProtoMode) == L7_SUCCESS)
   {
      if (sshProtoMode == L7_SSHD_PROTO_LEVEL)
         sprintf(strSshProtoMode,"%-18.17s","Versions 1 and 2");
      else if ((sshProtoMode == L7_SSHD_PROTO_LEVEL_V1_ONLY)||(sshProtoMode == L7_SSHD_PROTO_LEVEL_BOTH_V1))
         sprintf(strSshProtoMode,"%-18.11s","Version 1");
      else if ((sshProtoMode == L7_SSHD_PROTO_LEVEL_V2_ONLY)||(sshProtoMode == L7_SSHD_PROTO_LEVEL_BOTH_V2))
         sprintf(strSshProtoMode,"%-18.11s","Version 2");
      else
         sprintf(strSshProtoMode,"%-18.10s",CLILINE);
   }
   else
      sprintf(strSshAdminMode,"%-18.10s",CLILINE);

   ewsTelnetWrite(ewsContext,strSshProtoMode);

  bzero(stat,sizeof(stat));
  if (usmDbsshdNumSessionsGet(unit, &val) == L7_SUCCESS && ((L7_int32)val >= (L7_int32) 0))
  {
     sprintf(stat,"%-15d",(L7_int32) val);
  }
  else
     ewsTelnetPrintf (ewsContext, "%-15.8s",CLILINE);

  bzero(stat,sizeof(stat));
  if (usmDbSshdMaxMumSessionsGet(unit, &val) == L7_SUCCESS && ((L7_int32)val >= (L7_int32) 0) )
  {
      sprintf(stat,"%-15d",(L7_int32) val);
  }
  else
  {
      sprintf(stat,"%-15.8s",CLILINE);
  }
  ewsTelnetWrite(ewsContext,stat);
  ---------- SSH TELNET REMOVED-------*/

  ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_security_SshCfg);

  memset (strSshAdminMode, 0,sizeof(strSshAdminMode));
  if (usmDbsshdAdminModeGet(unit, &sshAdminMode) == L7_SUCCESS)
  {
    cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_security_AdministrativeMode_1);
    sprintf(strSshAdminMode,strUtilEnabledDisabledGet(sshAdminMode ,"-----"));
  }
  else
  {
    sprintf(strSshAdminMode,"%-21.15s",pStrInfo_common_Line);
  }
   ewsTelnetWrite(ewsContext, strSshAdminMode);

  memset (strSshProtoMode, 0,sizeof(strSshProtoMode));
  if (usmDbsshdProtoLevelGet(unit, &sshProtoMode) == L7_SUCCESS)
  {
    cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_security_ProtoLvls);
    if (sshProtoMode == L7_SSHD_PROTO_LEVEL)
    {
      sprintf(strSshProtoMode,"%-18.17s",pStrInfo_security_Vers1And2);
    }
      else if ((sshProtoMode == L7_SSHD_PROTO_LEVEL_V1_ONLY))
    {
      sprintf(strSshProtoMode,"%-18.11s",pStrInfo_security_Ver1);
    }
      else if ((sshProtoMode == L7_SSHD_PROTO_LEVEL_V2_ONLY))
    {
      sprintf(strSshProtoMode,"%-18.11s",pStrInfo_security_Ver2);
    }
    else
    {
      sprintf(strSshProtoMode,"%-18.10s",pStrInfo_common_Line);
    }
  }
  else
  {
    sprintf(strSshAdminMode,"%-18.10s",pStrInfo_common_Line);
  }

  ewsTelnetWrite(ewsContext,strSshProtoMode);

  memset (stat, 0,sizeof(stat));
  if (usmDbsshdNumSessionsGet(unit, &val) == L7_SUCCESS && ((L7_int32)val >= (L7_int32) 0))
  {
    cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_security_SshSessionsCurrentlyActive);
    sprintf(stat,"%-15d",(L7_int32) val);
  }
  else
  {
    sprintf(stat,"%-15.8s",pStrInfo_common_Line);
  }

  ewsTelnetWrite(ewsContext,stat);

  memset (stat, 0,sizeof(stat));
  if (usmDbSshdMaxNumSessionsGet(unit, &val) == L7_SUCCESS && ((L7_int32)val >= (L7_int32) 0) )
  {
    cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_security_MaxSshSessionsAllowed);
      sprintf(stat,"%-15d",(L7_int32) val);
  }
  else
  {
    sprintf(stat,"%-15.8s",pStrInfo_common_Line);
  }
  ewsTelnetWrite(ewsContext,stat);

  memset (stat, 0,sizeof(stat));
  if (usmDbsshdTimeoutGet(unit, &val) == L7_SUCCESS && ((L7_int32)val >= (L7_int32) 0) )
  {
    cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_security_SshTimeout);
      sprintf(stat,"%-15d",(L7_int32) val);
  }
  else
  {
    sprintf(stat,"%-15.8s",pStrInfo_common_Line);
  }
  ewsTelnetWrite(ewsContext,stat);

  cliFormat(ewsContext, "Keys Present: ");
  if (usmDbsshdKeyExists(SSHD_KEY_TYPE_DSA) == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat), "%s", "DSA ");
    ewsTelnetWrite(ewsContext,stat);
  }
  if (usmDbsshdKeyExists(SSHD_KEY_TYPE_RSA) == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat), "%s", "RSA");
    ewsTelnetWrite(ewsContext,stat);
  }

  cliFormat(ewsContext, "Key Generation In Progress: ");
  if (usmDbsshdKeyExists(SSHD_KEY_TYPE_DSA) == L7_FAILURE)
  {
    osapiSnprintf(stat, sizeof(stat), "%s", "DSA ");
    ewsTelnetWrite(ewsContext,stat);
  }
  if (usmDbsshdKeyExists(SSHD_KEY_TYPE_RSA) == L7_FAILURE)
  {
    osapiSnprintf(stat, sizeof(stat), "%s", "RSA");
    ewsTelnetWrite(ewsContext,stat);
  }

  if ((usmDbsshdKeyExists(SSHD_KEY_TYPE_RSA) != L7_FAILURE) &&
      (usmDbsshdKeyExists(SSHD_KEY_TYPE_DSA) != L7_FAILURE))
  {
    osapiSnprintf(stat, sizeof(stat), "%-15s", "None");
    ewsTelnetWrite(ewsContext,stat);
  }

  cliSyntaxBottom(ewsContext);
  return cliPrompt(ewsContext);
}






