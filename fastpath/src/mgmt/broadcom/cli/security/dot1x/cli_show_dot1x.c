/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/security/dot1x/cli_show_dot1x.c
 *
 * @purpose Dot1x show commands for the cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  3/05/2003
 *
 * @author  jlanagan
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_security_common.h"
#include "strlib_security_cli.h"
#include "l7_common.h"
#include "diffserv_exports.h"
#include "dot1x_exports.h"
#include "cli_web_exports.h"
#include "usmdb_dot1x_api.h"
#include "usmdb_port_user.h"
#include "usmdb_util_api.h"
#include "datatypes.h"
#include "clicommands_dot1x.h"
#include "clicommands_card.h"
#include "cliutil_dot1x.h"
#include "usmdb_counters_api.h"
#include "usmdb_common.h"

/*********************************************************************
*
* @purpose  display dot1x summary information
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes add range checking
*
* @cmdsyntax  show dot1x  [{summary <slot/port|all>}  | {detail <slot/port>} | {statistics <slot/port>}]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandShowDot1x(EwsContext ewsContext,
                                 L7_uint32 argc,
                                 const L7_char8 * * argv,
                                 L7_uint32 index)
{
  L7_BOOL mode;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strSlotPort[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 arg[L7_CLI_MAX_STRING_LENGTH];
  static L7_BOOL all;
  static L7_uint32 interface;
  L7_uint32 u = 1;
  L7_uint32 unit,s,p,slot,port,nextInterface,val,val1;
  L7_uint32 numArgs, count = 0;
  L7_uint32 argToken = 1;
  L7_uint32 argValue = 2;
  L7_uchar8 cap;
  L7_int32 retVal;
  L7_uint32 controlMode;
  L7_uchar8 macAddr[6];
  L7_RC_t rc;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  cliCmdScrollSet( L7_FALSE);

  numArgs = cliNumFunctionArgsGet();

  if (( numArgs != 0 )&&( numArgs != 2 ))
  {

    osapiStrncpySafe(arg, argv[index+argToken], sizeof(arg));

    if (strcmp(arg, pStrInfo_common_Dot1xShowSummary) == 0)
    {
      sprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, pStrErr_security_ShowDot1xPortSummary, cliSyntaxInterfaceHelp());
    }
    else if (strcmp(arg, pStrInfo_common_IgmpProxyGrpsDetail) == 0)
    {
      sprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, pStrErr_security_ShowDot1xPortDetailed, cliSyntaxInterfaceHelp());
    }
    else if (strcmp(arg, pStrInfo_common_Dot1xShowStats) == 0)
    {
      sprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, pStrErr_security_ShowDot1xPortStats, cliSyntaxInterfaceHelp());
    }
    else
    {
      sprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, pStrErr_security_ShowDot1xSummary_1, cliSyntaxInterfaceHelp(),
                        cliSyntaxInterfaceHelp(), cliSyntaxInterfaceHelp());
    }

    return cliSyntaxReturnPrompt (ewsContext, buf);
  }

  if (numArgs == 0)
  {
    /* Administrative Mode */
    if ( usmDbDot1xAdminModeGet(unit, &mode) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_Dot1xShowAdminMode);
    }
    if (mode == L7_ENABLE)
    {
      osapiStrncpySafe(buf, pStrInfo_common_Enbld, sizeof(buf));
    }                                /* Enabled */
    else if (mode == L7_DISABLE)
    {
      osapiStrncpySafe(buf, pStrInfo_common_Dsbld, sizeof(buf));
    }                                   /* Disabled */
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_Dot1xShowAdminMode);
    }
    cliFormatShortText(ewsContext,pStrInfo_common_AdministrativeMode);    /* Administrative Mode */
    ewsTelnetWrite(ewsContext, buf);

    /*Vlan Assignment Mode*/
    if ( usmDbDot1xVlanAssignmentModeGet(unit, &mode) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_Dot1xShowVlanAssignmentMode);
    }
    if (mode == L7_ENABLE)
    {
      osapiStrncpySafe(buf, pStrInfo_common_Enbld, sizeof(buf));
    }                                /* Enabled */
    else if (mode == L7_DISABLE)
    {
      osapiStrncpySafe(buf, pStrInfo_common_Dsbld, sizeof(buf));
    }                                   /* Disabled */
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext,pStrErr_security_Dot1xShowVlanAssignmentMode);
    }
    cliFormatShortText(ewsContext,pStrInfo_common_VlanAssignmentMode);    /* VLAN Assignment Mode */
    ewsTelnetWrite(ewsContext, buf);

    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1X_COMPONENT_ID, L7_DOT1X_DYNAMIC_VLANASSIGN_FEATURE_ID) == L7_TRUE)
    {
      /* Get the Dynamic Vlan Creation configuration Mode*/
      if ( usmDbDot1xDynamicVlanCreationModeGet(unit, &mode) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_Dot1xShowDynamicVlanAssignmentMode);
      }
      if (mode == L7_ENABLE)
      {
        osapiStrncpySafe( buf, pStrInfo_common_Enbld, sizeof(buf));
      }                                /* Enabled */
      else if (mode == L7_DISABLE)
      {
        osapiStrncpySafe( buf, pStrInfo_common_Dsbld, sizeof(buf));
      }                                   /* Disabled */
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext,pStrErr_security_Dot1xShowDynamicVlanAssignmentMode);
      }
      cliFormatShortText(ewsContext,pStrInfo_common_DynamicVlanAssignmentMode);    /* VLAN Assignment Mode */
      ewsTelnetWrite(ewsContext, buf);
    }

    /*Monitor Mode*/
    if ( usmDbDot1xMonitorModeGet(unit, &mode) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_security_Dot1xShowMonitorMode);
    }
    memset(buf, 0x00, sizeof(buf));
    if (mode == L7_ENABLE)
    {
      osapiStrncpy(buf, pStrInfo_common_Enbld, sizeof(buf));
    }                                /* Enabled */
    else if (mode == L7_DISABLE)
    {
      osapiStrncpy(buf, pStrInfo_common_Dsbld, sizeof(buf));
    }                                /* Disabled */
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_Dot1xShowMonitorMode);
    }
    cliFormatShortText(ewsContext,pStrInfo_common_MonitorMode);    /* Monitor Mode */
    ewsTelnetWrite(ewsContext, buf);
 
  }
  else if (numArgs == 2)
  {
    if (strcmp(argv[index+argToken],pStrInfo_common_Dot1xShowSummary) == 0)
    {
      if ( cliGetCharInputID() != CLI_INPUT_EMPTY ) /* Not first pass */
      {
        if(L7_TRUE == cliIsPromptRespQuit())
        {
          ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
          return cliPrompt(ewsContext);
        }
      }
      else
      {
        if (strlen(argv[index+argValue]) >= sizeof(strSlotPort))
        {
          sprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, pStrErr_security_ShowDot1xPortSummary, cliSyntaxInterfaceHelp());
          return cliSyntaxReturnPrompt (ewsContext, buf);
        }
        osapiStrncpySafe(strSlotPort, argv[index+argValue], sizeof(strSlotPort));
        cliConvertToLowerCase(strSlotPort);

        if ( strcmp(strSlotPort, pStrInfo_common_All /* "all" */) == 0 )
        {
          all = L7_TRUE;
          interface = 1;
        }
        else
        {
          all = L7_FALSE;

          if (cliIsStackingSupported() == L7_TRUE)
          {
            if ((rc = cliValidSpecificUSPCheck(argv[index+argValue], &unit, &slot, &port)) != L7_SUCCESS)
            {
              if (rc == L7_ERROR)
              {
                ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
                ewsTelnetPrintf (ewsContext, "%u/%u/%u", unit, slot, port);
              }
              else if (rc == L7_NOT_EXIST)
              {
                ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_UnitDoesntExist);
              }
              else if (rc == L7_NOT_SUPPORTED)
              {
                ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_SlotDoesntExist);
              }
              else
              {
                ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidInput);
              }

              return cliSyntaxReturnPrompt (ewsContext, "");
            }

            /* Get interface and check its validity */
            if (usmDbIntIfNumFromUSPGet(unit, slot, port, &interface) != L7_SUCCESS ||
                cliDot1xInterfaceValidate(ewsContext, unit, interface) != L7_SUCCESS)
            {
              ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
              return cliSyntaxReturnPrompt (ewsContext, "%u/%u/%u", unit, slot, port);
            }
          }
          else
          {
            /* NOTE: No need to check the value of `unit` as
             *       ID of a standalone switch is always `U_IDX` (=> 1).
             */
            unit = cliGetUnitId();
            if (cliSlotPortToIntNum(ewsContext, argv[index+argValue], &slot, &port, &interface) != L7_SUCCESS ||
                cliDot1xInterfaceValidate(ewsContext, unit, interface) != L7_SUCCESS)
            {
              return cliPrompt(ewsContext);
            }
          }
        }
      } /* end first pass setup */

      /* Print headers on each page */
      ewsTelnetWriteAddBlanks (1, 0, 33, 15, L7_NULLPTR, ewsContext,
                               pStrInfo_security_OperatingReauthentication);
      ewsTelnetWriteAddBlanks (1, 0, 0, 2, L7_NULLPTR, ewsContext,
                               pStrInfo_security_IntfCntrlModeCntrlModeEnbldPortStatus);
      ewsTelnetWrite(ewsContext,
                     "\r\n--------- ------------------ ------------------ ---------------- ------------- ");

      while ( interface != 0 )
      {
        if ( usmDbVisibleInterfaceCheck(unit, interface, &retVal) == L7_SUCCESS &&
            usmDbUnitSlotPortGet(interface, &u, &s, &p) == L7_SUCCESS &&
            cliDot1xInterfaceValidateWithMessage(ewsContext, unit, interface, L7_FALSE ) == L7_SUCCESS )
        {
          osapiStrncpySafe(stat, cliDisplayInterfaceHelp(u, s, p), sizeof(stat));
          ewsTelnetPrintf (ewsContext, "\r\n%-10s", stat );
          cap = 0x00;
          rc=usmDbDot1xPortPaeCapabilitiesGet(unit, interface, &cap);
          if ( rc!=L7_SUCCESS)
          {
            ewsTelnetWrite(ewsContext,"                   ");
          }
          if (cap & L7_DOT1X_PAE_PORT_AUTH_CAPABLE)
          {
             rc = usmDbDot1xPortControlModeGet(unit, interface, &val);
          }
          else if (cap & L7_DOT1X_PAE_PORT_SUPP_CAPABLE)
          {
            rc = usmDbDot1xSupplicantPortControlModeGet (interface, &val);
          }
          if ( rc  == L7_SUCCESS )
          {
            switch (val)
            {
            case L7_DOT1X_PORT_AUTO:
              osapiSnprintf(stat, sizeof(stat),"%-19s",pStrInfo_common_Auto2);    /* auto */
              break;
            case L7_DOT1X_PORT_FORCE_AUTHORIZED:
              osapiSnprintf(stat, sizeof(stat),"%-19s",pStrInfo_security_ForceAuthorizedDot1x);    /* force-authorized */
              break;
            case L7_DOT1X_PORT_FORCE_UNAUTHORIZED:
              osapiSnprintf(stat, sizeof(stat),"%-19s",pStrInfo_security_ForceUnAuthorizedDot1x);    /* force-unauthorized */
              break;
            case L7_DOT1X_PORT_AUTO_MAC_BASED:
              osapiSnprintf(stat, sizeof(stat),"%-19s",pStrInfo_security_MacBasedDot1x);    /* mac-based */
              break;
            default:
              break;
            }
            ewsTelnetWrite(ewsContext,stat);
          }
          else
          {
            ewsTelnetWrite(ewsContext,"                   ");
          }
          if (cap & L7_DOT1X_PAE_PORT_AUTH_CAPABLE)
          {
             rc = usmDbDot1xPortOperControlModeGet(unit, interface, &val);
          }
          else if (cap & L7_DOT1X_PAE_PORT_SUPP_CAPABLE)
          {
            rc = usmDbDot1xSupplicantPortControlModeGet (interface, &val);
          }

          /* Operating Control Mode */
          if ( rc == L7_SUCCESS )
          {
            switch (val)
            {
            case L7_DOT1X_PORT_AUTO:
              osapiSnprintf(stat, sizeof(stat),"%-19s",pStrInfo_common_Auto2);  /* auto */
              break;
            case L7_DOT1X_PORT_FORCE_AUTHORIZED:
              osapiSnprintf(stat, sizeof(stat),"%-19s",pStrInfo_security_ForceAuthorized_1);  /* forceauthorized */
              break;
            case L7_DOT1X_PORT_FORCE_UNAUTHORIZED:
              osapiSnprintf(stat, sizeof(stat),"%-19s",pStrInfo_security_ForceUnAuthorized_1);  /* forceunauthorized */
              break;
            case L7_DOT1X_PORT_AUTO_MAC_BASED:
              osapiSnprintf(stat, sizeof(stat),"%-19s",pStrInfo_security_MacBasedDot1x);    /* mac-based */
              break;
            default:
              osapiSnprintf(stat, sizeof(stat),"%-19s",pStrInfo_common_NotApplicable);  /* N/A */
              break;
            }
            ewsTelnetWrite(ewsContext,stat);
          }
          else
          {
            ewsTelnetWrite(ewsContext,"                   ");
          }

          /* Reauthentication Enabled */
          if (cap & L7_DOT1X_PAE_PORT_AUTH_CAPABLE)
          {
   
            if ( usmDbDot1xPortReAuthEnabledGet(unit, interface, &mode) == L7_SUCCESS )
            {
              if (mode == L7_TRUE)
              {
                osapiSnprintf(stat, sizeof(stat), "%-17s", pStrInfo_common_True);
              }                                        /* true */
              else if (mode == L7_FALSE)
              {
                osapiSnprintf(stat, sizeof(stat), "%-17s", pStrInfo_common_False);
              }                                         /* false */
              ewsTelnetWrite(ewsContext,stat);
            }
            else
            {
              ewsTelnetWrite(ewsContext,"                 ");
            }
         }
         else
         {
           osapiSnprintf(stat, sizeof(stat),"%-17s",pStrInfo_common_NotApplicable);  /* N/A */
           ewsTelnetWrite(ewsContext,stat);
         }
         if (cap & L7_DOT1X_PAE_PORT_AUTH_CAPABLE)
         {
            rc = usmDbDot1xPortAuthControlledPortStatusGet (unit, interface, &val);
         }
         else
         {
           cap=0x00;
           rc = usmDbDot1xSupplicantPortStatusGet (interface, &cap);
           val=cap;
         }
          /* Port Status */
          if (rc  == L7_SUCCESS)
          {
            if (val == L7_DOT1X_PORT_STATUS_AUTHORIZED )
            {
              osapiSnprintf(stat, sizeof(stat), "%-14s", pStrInfo_security_Authorized);
            }                                              /* "Authorized" */
            else if (val == L7_DOT1X_PORT_STATUS_UNAUTHORIZED)
            {
              osapiSnprintf(stat, sizeof(stat), "%-14s", pStrInfo_common_UnAuthorized);
            }                                                /* "Unauthorized */
            else
            {
              osapiSnprintf(stat, sizeof(stat), "%-14s", pStrInfo_common_NotApplicable);
            }                                    /* N/A */
            ewsTelnetWrite(ewsContext,stat);
          }
          else
          {
            ewsTelnetWrite(ewsContext,"              ");
          }
          count++;
        } /* end if interface is valid */

        if ( !all )
        {
          interface = 0;
        }
        else
        {
          if ( usmDbValidIntIfNumNext(interface, &nextInterface) == L7_SUCCESS )
          {
            interface = nextInterface;
          }
          else
          {
            interface = 0;
          }
        }
        if((count == CLI_MAX_SCROLL_LINES-4) && (interface != 0))
        {
          cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
          cliAlternateCommandSet(pStrInfo_security_ShowDot1xSummaryAllCmd);
          cliSyntaxBottom(ewsContext);
          return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
        }
      } /* end while more interfaces */
    }
    else if (strcmp(argv[index+argToken],pStrInfo_common_IgmpProxyGrpsDetail) == 0)
    {
      if ( cliGetCharInputID() == CLI_INPUT_EMPTY ) /* First pass */
      {
        /* Validate the port */
        if (strlen(argv[index+argValue]) >= sizeof(strSlotPort))
        {
          sprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, pStrErr_security_ShowDot1xPortDetailed, cliSyntaxInterfaceHelp());
          return cliSyntaxReturnPrompt (ewsContext, buf);
        }
        osapiStrncpySafe(strSlotPort, argv[index+argValue], sizeof(strSlotPort));

        if (cliIsStackingSupported() == L7_TRUE)
        {
          if ((rc = cliValidSpecificUSPCheck(strSlotPort, &unit, &slot, &port)) != L7_SUCCESS)
          {
            if (rc == L7_ERROR)
            {
              ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
              ewsTelnetPrintf (ewsContext, "%u/%u/%u", unit, slot, port);
            }
            else if (rc == L7_NOT_EXIST)
            {
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_UnitDoesntExist);
            }
            else if (rc == L7_NOT_SUPPORTED)
            {
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_SlotDoesntExist);
            }
            else
            {
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidInput);
            }

            return cliSyntaxReturnPrompt (ewsContext, "");
          }

          /* Get interface and check its validity */
          if (usmDbIntIfNumFromUSPGet(unit, slot, port, &interface) != L7_SUCCESS ||
              cliDot1xInterfaceValidate(ewsContext, unit, interface) != L7_SUCCESS)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
            return cliSyntaxReturnPrompt (ewsContext, "%u/%u/%u", unit, slot, port);
          }
        }
        else
        {
          /* NOTE: No need to check the value of `unit` as
           *       ID of a standalone switch is always `U_IDX` (=> 1).
           */
          unit = cliGetUnitId();
          if (cliSlotPortToIntNum(ewsContext, strSlotPort, &slot, &port, &interface) != L7_SUCCESS ||
              cliDot1xInterfaceValidate(ewsContext, unit, interface) != L7_SUCCESS)
          {
            return cliPrompt(ewsContext);
          }
        }

        /* Port */
        cliFormat(ewsContext,pStrInfo_common_Port_2); /* "Port" */
        ewsTelnetWrite(ewsContext, strSlotPort);

        /* Protocol Version */
        if ( usmDbDot1xPortProtocolVersionGet(unit, interface, &val) == L7_SUCCESS )
        {
          cliFormat(ewsContext,pStrInfo_common_ApProtoVer); /* Protocol Version */
          ewsTelnetPrintf (ewsContext, "%u", val );
        }

        /* PAE Capabilities */
        cap = 0x00;
        if ( usmDbDot1xPortPaeCapabilitiesGet(unit, interface, &cap) == L7_SUCCESS )
        {
          cliFormat(ewsContext,pStrInfo_security_PaeCapabilities); /* PAE Capabilities */
          if (cap & L7_DOT1X_PAE_PORT_AUTH_CAPABLE)
          {
            ewsTelnetWrite(ewsContext, pStrInfo_security_Authenticator);
          }
          if (cap & L7_DOT1X_PAE_PORT_SUPP_CAPABLE)
          {
            ewsTelnetWrite(ewsContext, pStrInfo_security_Supplicant);
          }
        }
        if (cap & L7_DOT1X_PAE_PORT_AUTH_CAPABLE)
        { 
           /* Control Mode */
           if ( usmDbDot1xPortControlModeGet(unit, interface, &controlMode) == L7_SUCCESS )
           {
             cliFormat(ewsContext, pStrInfo_security_CntrlMode); /* Control Mode */
             switch (controlMode)
             {
               case L7_DOT1X_PORT_AUTO:
                ewsTelnetWrite(ewsContext,pStrInfo_common_Auto2);    /* auto */
               break;
              case L7_DOT1X_PORT_FORCE_AUTHORIZED:
               ewsTelnetWrite(ewsContext,pStrInfo_security_ForceAuthorizedDot1x);    /* force-authorized */
              break;
             case L7_DOT1X_PORT_FORCE_UNAUTHORIZED:
               ewsTelnetWrite(ewsContext,pStrInfo_security_ForceUnAuthorizedDot1x);    /* force-unauthorized */
              break;
            case L7_DOT1X_PORT_AUTO_MAC_BASED:
              ewsTelnetWrite(ewsContext,pStrInfo_security_MacBasedDot1x);    /* mac-based */
            break;
           default:
             ewsTelnetWrite(ewsContext, pStrInfo_common_NotApplicable);
            break;
          }
         }

         /* Authenticator PAE State */
        if ( usmDbDot1xPortAuthPaeStateGet(unit, interface, &val) == L7_SUCCESS &&
             controlMode != L7_DOT1X_PORT_AUTO_MAC_BASED)
        {
          cliFormat(ewsContext, pStrInfo_security_AuthenticatorPaeState); /* Authenticator PAE State */
          switch (val)
          {
          case L7_DOT1X_APM_INITIALIZE:
            ewsTelnetWrite(ewsContext, pStrInfo_common_Initialize);      /* "Initialize" */
            break;
          case L7_DOT1X_APM_DISCONNECTED:
            ewsTelnetWrite(ewsContext, pStrInfo_security_Disconnected);      /* Disconnected */
            break;
          case L7_DOT1X_APM_CONNECTING:
            ewsTelnetWrite(ewsContext, pStrInfo_security_Connecting);      /* Connecting */
            break;
          case L7_DOT1X_APM_AUTHENTICATING:
            ewsTelnetWrite(ewsContext, pStrInfo_security_Authenticating);      /* Authenticating */
            break;
          case L7_DOT1X_APM_AUTHENTICATED:
            ewsTelnetWrite(ewsContext, pStrInfo_common_ClientAuth);      /* Authenticated */
            break;
          case L7_DOT1X_APM_ABORTING:
            ewsTelnetWrite(ewsContext, pStrInfo_security_Aborting);      /* Aborting */
            break;
          case L7_DOT1X_APM_HELD:
            ewsTelnetWrite(ewsContext, pStrInfo_security_Held);      /* Held */
            break;
          case L7_DOT1X_APM_FORCE_AUTH:
            ewsTelnetWrite(ewsContext, pStrInfo_security_ForceAuthorized);      /* Force Authorized */
            break;
          case L7_DOT1X_APM_FORCE_UNAUTH:
            ewsTelnetWrite(ewsContext, pStrInfo_security_ForceUnAuthorized);      /* Force Unauthorized */
            break;
          default:
            ewsTelnetWrite(ewsContext, pStrInfo_common_NotApplicable);      /* "N/A" */
            break;
          }
        }

        /* Backend Authentication State */
        if ( usmDbDot1xPortBackendAuthStateGet(unit, interface, &val) == L7_SUCCESS &&
             controlMode != L7_DOT1X_PORT_AUTO_MAC_BASED)
        {
          cliFormat(ewsContext,pStrInfo_security_BackEndAuthState); /* Backend Authentication State */
          switch (val)
          {
          case L7_DOT1X_BAM_REQUEST:
            ewsTelnetWrite(ewsContext, pStrInfo_security_Req);      /* Request */
            break;
          case L7_DOT1X_BAM_RESPONSE:
            ewsTelnetWrite(ewsContext, pStrInfo_security_Resp);      /* Response */
            break;
          case L7_DOT1X_BAM_SUCCESS:
            ewsTelnetWrite(ewsContext, pStrInfo_common_ApActionSuccess);      /* Success */
            break;
          case L7_DOT1X_BAM_FAIL:
            ewsTelnetWrite(ewsContext, pStrInfo_common_Fail);      /* Fail */
            break;
          case L7_DOT1X_BAM_TIMEOUT:
            ewsTelnetWrite(ewsContext, pStrInfo_security_Timeout_1);      /* Timeout */
            break;
          case L7_DOT1X_BAM_IDLE:
            ewsTelnetWrite(ewsContext, pStrInfo_common_Idle_1);      /* Idle */
            break;
          case L7_DOT1X_BAM_INITIALIZE:
            ewsTelnetWrite(ewsContext, pStrInfo_common_Initialize);      /* "Initialize" */
            break;
          default:
            ewsTelnetWrite(ewsContext, pStrInfo_common_NotApplicable);      /* "N/A" */
            break;
          }
        }

        /* Quiet Period */
        if ( usmDbDot1xPortQuietPeriodGet(unit, interface, &val) == L7_SUCCESS )
        {
          cliFormat(ewsContext, pStrInfo_security_QuietPeriodSecs); /* Quiet Period (secs) */
          ewsTelnetPrintf (ewsContext, "%u", val );
        }

        /* Transmit Period */
        if ( usmDbDot1xPortTxPeriodGet(unit, interface, &val) == L7_SUCCESS )
        {
          cliFormat(ewsContext, pStrInfo_security_TxPeriodSecs); /* Transmit Period (secs)*/
          ewsTelnetPrintf (ewsContext, "%u", val );
        }

        /* Guest Vlan <vlan-id>*/
        if ( usmDbDot1xAdvancedGuestPortsCfgGet(unit, interface, &val) == L7_SUCCESS )
        {
          cliFormat(ewsContext, pStrInfo_security_GuestVlanId);
          ewsTelnetPrintf (ewsContext, "%u", val );
        }

        if(usmDbDot1xAdvancedPortGuestVlanPeriodGet(unit, interface,&val) == L7_SUCCESS )
        {
          cliFormat(ewsContext, pStrInfo_security_GuestVlanPeriod); /*Guest VLAN Period (secs)*/
          ewsTelnetPrintf (ewsContext, "%u", val );
        }

        /* Supplicant Timeout */
        if ( usmDbDot1xPortSuppTimeoutGet(unit, interface, &val) == L7_SUCCESS )
        {
          cliFormat(ewsContext, pStrInfo_security_SupplicantTimeoutSecs); /* Supplicant Timeout (secs) */
          ewsTelnetPrintf (ewsContext, "%u", val );
        }

        /* Server Timeout */
        if ( usmDbDot1xPortServerTimeoutGet(unit, interface, &val) == L7_SUCCESS )
        {
          cliFormat(ewsContext, pStrInfo_security_SrvrTimeoutSecs);   /* Server Timeout */
          ewsTelnetPrintf (ewsContext, "%u", val );
        }

        /* Maximum Requested */
        if ( usmDbDot1xPortMaxReqGet(unit, interface, &val) == L7_SUCCESS )
        {
          cliFormat(ewsContext, pStrInfo_security_MaxReqs); /* Maximum Requests */
          ewsTelnetPrintf (ewsContext, "%u", val );
        }
      
     

      /*if (usmDbFeaturePresentCheck (unit,L7_DOT1X_COMPONENT_ID, 
                                   L7_DOT1X_VLANASSIGN_FEATURE_ID) == L7_TRUE)
      { */
      /* Vlan Assigned by Dot1x*/
      if ( usmDbDot1xPortVlanAssignedGet(unit, interface, &val) == L7_SUCCESS &&
           controlMode != L7_DOT1X_PORT_AUTO_MAC_BASED)
      {
           cliFormat(ewsContext, pStrInfo_security_Dot1xShowClientVland); /* Vlan Assigned */
           ewsTelnetPrintf (ewsContext, "%u", val );
      }

      /* Reason Vlan was assigned by Dot1x*/
      if ((usmDbDot1xPortVlanAssignedReasonGet(unit,interface,&val) == L7_SUCCESS) &&
           (controlMode != L7_DOT1X_PORT_AUTO_MAC_BASED)) 
      {
          cliFormat(ewsContext, pStrInfo_security_VlanAssignedReason); /* VLAN Assigned Reason*/
          switch (val)
          {
          case L7_DOT1X_RADIUS_ASSIGNED_VLAN:
              ewsTelnetPrintf (ewsContext, "%s ",pStrInfo_security_Dot1xVlanAssigned_Radius);
             break;
          case L7_DOT1X_UNAUTHENTICATED_VLAN:
             ewsTelnetPrintf (ewsContext, "%s ",pStrInfo_security_Dot1xVlanAssigned_unauth);
             break;
          case L7_DOT1X_GUEST_VLAN:
             ewsTelnetPrintf (ewsContext, "%s ",pStrInfo_security_Dot1xVlanAssigned_Guest);
             break;
          case L7_DOT1X_VOICE_VLAN:
             ewsTelnetPrintf (ewsContext, "%s ",pStrInfo_security_Dot1xVlanAssigned_Voice);
             break;
          case L7_DOT1X_DEFAULT_ASSIGNED_VLAN:
             ewsTelnetPrintf (ewsContext, "%s ",pStrInfo_security_Dot1xVlanAssigned_Default);
             break;
          case L7_DOT1X_MONITOR_MODE_VLAN:
             ewsTelnetPrintf (ewsContext, "%s ",pStrInfo_security_Dot1xVlanAssigned_MonitorMode);
             break;
          case L7_DOT1X_NOT_ASSIGNED:
             ewsTelnetPrintf (ewsContext, "%s ",pStrInfo_security_Dot1xVlanAssigned_NotAssigned);
             break;
          default:
             ewsTelnetPrintf (ewsContext, "%s ",pStrInfo_common_EmptyString);  
              break;
          }
      }
     /*}*/

        /* Reauthentication Period */
        if ( usmDbDot1xPortReAuthPeriodGet(unit, interface, &val) == L7_SUCCESS )
        {
          cliFormat(ewsContext, pStrInfo_security_ReauthenticationPeriodSecs); /* Reauthentication Period (secs) */
          ewsTelnetPrintf (ewsContext, "%u", val );
        }

        /* Reauthentication Enabled */
        if ( usmDbDot1xPortReAuthEnabledGet(unit, interface, &mode) == L7_SUCCESS )
        {
          cliFormat(ewsContext, pStrInfo_security_ReauthenticationEnbld); /* Reauthentication Enabled */
          if (mode == L7_TRUE)
          {
            ewsTelnetWrite(ewsContext, pStrInfo_common_True);
          }                                            /* true */
          else if (mode == L7_FALSE)
          {
            ewsTelnetWrite(ewsContext, pStrInfo_common_False);
          }                                             /* false */
        }

        /* Key Transmission Enabled */
        if ( usmDbDot1xPortKeyTransmissionEnabledGet(unit, interface, &mode) == L7_SUCCESS )
        {
          cliFormat(ewsContext, pStrInfo_security_KeyTransmissionEnbld); /* Key Transmission Enabled */
          if (mode == L7_TRUE)
          {
            osapiSnprintf(stat, sizeof(stat), "%-16s", pStrInfo_common_True);
          }                                       /* true */
          else if (mode == L7_FALSE)
          {
            osapiSnprintf(stat, sizeof(stat), "%-16s", pStrInfo_common_False);
          }                                        /* false */
          ewsTelnetWrite(ewsContext, stat);
        }

        /* Control Direction */
        if ( usmDbDot1xPortAdminControlledDirectionsGet(unit, interface, &val) == L7_SUCCESS )
        {
          cliFormat(ewsContext,  pStrInfo_security_CntrlDirection); /* Control Direction */

          switch (val)
          {
          case L7_DOT1X_PORT_DIRECTION_BOTH:
            osapiSnprintf(stat, sizeof(stat), "%-16s", pStrInfo_common_Both_1);    /* both */
            break;
          case L7_DOT1X_PORT_DIRECTION_IN:
            osapiSnprintf(stat, sizeof(stat), "%-16s", pStrInfo_common_AclInStr);    /* in */
            break;
          default:
            osapiSnprintf(stat, sizeof(stat), "%-16s", pStrInfo_common_NotApplicable);    /* N/A */
            break;
          }
          ewsTelnetWrite(ewsContext, stat);
        }

        /* Max-user */
        if(usmDbDot1xPortMaxUsersGet(unit, interface, &val) == L7_SUCCESS )
        {
          cliFormat(ewsContext, pStrInfo_security_Max_Users); /* Maximum Users */
          ewsTelnetPrintf (ewsContext, "%u", val );
        }

        /* Unauthenticated vlan */
        if(usmDbDot1xPortUnauthenticatedVlanGet(unit, interface, &val) == L7_SUCCESS )
        {
          cliFormat(ewsContext, pStrInfo_security_UnauthenticatedVlanId);  /*Unauthenticated VLAN ID*/
          ewsTelnetPrintf (ewsContext, "%u", val );
        }

        /*Session Timeout assigned by the Radius server */
        if ((usmDbDot1xPortSessionTimeoutGet(unit,interface,&val)==L7_SUCCESS)&&
            (controlMode != L7_DOT1X_PORT_AUTO_MAC_BASED)) 
        {
            cliFormat(ewsContext,pStrInfo_security_SessionTimeout); /* Session Timeout */
            ewsTelnetPrintf (ewsContext, "%u", val );
        }

        /*Session Termination Action assigned by the Radius Server*/
        if ((usmDbDot1xPortTerminationActionGet(unit,interface,&val)==L7_SUCCESS)&&
            (controlMode != L7_DOT1X_PORT_AUTO_MAC_BASED)) 
        {
            cliFormat(ewsContext,pStrInfo_security_SessionTerminateAction); /* Session Termination Action */
            switch(val) 
            {
            case L7_DOT1X_TERMINATION_ACTION_RADIUS:
                    ewsTelnetPrintf (ewsContext, "%s\n",pStrInfo_security_Dot1xSessionTerminationAction_Radius);
                break;
            case L7_DOT1X_TERMINATION_ACTION_DEFAULT:
                    ewsTelnetPrintf (ewsContext, "%s\n",pStrInfo_security_Dot1xVlanAssigned_Default);
                break;

            }
            
        }

        /* Get each supplicant info if the control mode is mac-based */
        if(usmDbDot1xPortControlModeGet(unit, interface, &val) == L7_SUCCESS &&
           val == L7_DOT1X_PORT_AUTO_MAC_BASED )
        {
          L7_uint32 lIntIfNum;
          L7_uchar8 suppMacAddr[L7_ENET_MAC_ADDR_LEN];

          if(usmDbDot1xLogicalPortFirstGet(interface,&lIntIfNum) != L7_SUCCESS)
          {
            return cliSyntaxReturnPrompt (ewsContext, "");
          }
          ewsTelnetWrite(ewsContext,"\r\n\n");
          ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext,pStrInfo_security_Dot1xLogicalPort);
          ewsTelnetWrite(ewsContext,"\n ------------  -----------------  ------------------  -------------  ---- --------");

          do
          {
            ewsTelnetPrintf (ewsContext, "\r\n %-14u", lIntIfNum);

            /* Supplicant Mac address */
            if(usmDbDot1xLogicalPortSupplicantMacAddrGet(lIntIfNum,suppMacAddr) == L7_SUCCESS)
            {
              osapiSnprintf(buf,sizeof(buf),"%02X:%02X:%02X:%02X:%02X:%02X",
                      suppMacAddr[0], suppMacAddr[1], suppMacAddr[2], suppMacAddr[3], suppMacAddr[4], suppMacAddr[5]);
               ewsTelnetPrintf(ewsContext,"%-19s ", buf);
            }

            /* Authenticator PAE State machine*/
            if(usmDbDot1xLogicalPortPaeStateGet(lIntIfNum,&val) == L7_SUCCESS)
            {
              switch (val)
              {
              case L7_DOT1X_APM_INITIALIZE:
                ewsTelnetPrintf (ewsContext,"%-19s", pStrInfo_common_Initialize);      /* "Initialize" */
                break;                           
              case L7_DOT1X_APM_DISCONNECTED:
                ewsTelnetPrintf (ewsContext,"%-19s", pStrInfo_security_Disconnected);      /* Disconnected */
                break;
              case L7_DOT1X_APM_CONNECTING:
                ewsTelnetPrintf (ewsContext,"%-19s", pStrInfo_security_Connecting);      /* Connecting */
                break;
              case L7_DOT1X_APM_AUTHENTICATING:
                ewsTelnetPrintf (ewsContext,"%-19s", pStrInfo_security_Authenticating);      /* Authenticating */
                break;
              case L7_DOT1X_APM_AUTHENTICATED:
                ewsTelnetPrintf (ewsContext,"%-19s", pStrInfo_common_ClientAuth);      /* Authenticated */
                break;
              case L7_DOT1X_APM_ABORTING:
                ewsTelnetPrintf (ewsContext,"%-19s", pStrInfo_security_Aborting);      /* Aborting */
                break;
              case L7_DOT1X_APM_HELD:
                ewsTelnetPrintf (ewsContext,"%-19s", pStrInfo_security_Held);      /* Held */
                break;
              case L7_DOT1X_APM_FORCE_AUTH:
                ewsTelnetPrintf (ewsContext,"%-19s", pStrInfo_security_ForceAuthorized);      /* Force Authorized */
                break;
              case L7_DOT1X_APM_FORCE_UNAUTH:
                ewsTelnetPrintf (ewsContext,"%-19s", pStrInfo_security_ForceUnAuthorized);      /* Force Unauthorized */
                break;
              default:
                ewsTelnetPrintf (ewsContext,"%-19s", pStrInfo_common_NotApplicable);      /* "N/A" */
                break;
              }
            }

            /* Backend Auth State machine*/
            if(usmDbDot1xLogicalPortBackendAuthStateGet(lIntIfNum,&val) == L7_SUCCESS)
            {
              switch (val)
              {
              case L7_DOT1X_BAM_REQUEST:
                ewsTelnetPrintf (ewsContext,"%-15s", pStrInfo_security_Req);      /* Request */
                break;
              case L7_DOT1X_BAM_RESPONSE:
                ewsTelnetPrintf (ewsContext,"%-15s", pStrInfo_security_Resp);      /* Response */
                break;
              case L7_DOT1X_BAM_SUCCESS:
                ewsTelnetPrintf (ewsContext,"%-15s", pStrInfo_common_ApActionSuccess);      /* Success */
                break;
              case L7_DOT1X_BAM_FAIL:
                ewsTelnetPrintf (ewsContext,"%-15s", pStrInfo_common_Fail);      /* Fail */
                break;
              case L7_DOT1X_BAM_TIMEOUT:
                ewsTelnetPrintf (ewsContext,"%-15s", pStrInfo_security_Timeout_1);      /* Timeout */
                break;
              case L7_DOT1X_BAM_IDLE:
                ewsTelnetPrintf (ewsContext,"%-15s", pStrInfo_common_Idle_1);      /* Idle */
                break;
              case L7_DOT1X_BAM_INITIALIZE:
                ewsTelnetPrintf (ewsContext,"%-15s", pStrInfo_common_Initialize);      /* "Initialize" */
                break;
              default:
                ewsTelnetPrintf (ewsContext,"%-15s", pStrInfo_common_NotApplicable);      /* "N/A" */
                break;
              }
            }

            /* vlan assigned */
            if(usmDbDot1xLogicalPortVlanAssignmentGet(lIntIfNum,&val,&val1) == L7_SUCCESS)
            {
              switch (val1)
              {
              case L7_DOT1X_RADIUS_ASSIGNED_VLAN:
                ewsTelnetPrintf (ewsContext, "%-4u %s", val,pStrInfo_security_Dot1xVlanAssigned_Radius);
                break;
              case L7_DOT1X_UNAUTHENTICATED_VLAN:
                ewsTelnetPrintf (ewsContext, "%-4u %s", val,pStrInfo_security_Dot1xVlanAssigned_unauth);
                break;
              case L7_DOT1X_GUEST_VLAN:
                ewsTelnetPrintf (ewsContext, "%-4u %s ",val,pStrInfo_security_Dot1xVlanAssigned_Guest);
                break;
              case L7_DOT1X_VOICE_VLAN:
                ewsTelnetPrintf (ewsContext, "%-4u %s ",val,pStrInfo_security_Dot1xVlanAssigned_Voice);
                break;
              case L7_DOT1X_DEFAULT_ASSIGNED_VLAN:
                ewsTelnetPrintf (ewsContext, "%-4u %s", val,pStrInfo_security_Dot1xVlanAssigned_Default);
                break;
              case L7_DOT1X_MONITOR_MODE_VLAN:
                ewsTelnetPrintf (ewsContext, "%-4u %s",val,pStrInfo_security_Dot1xVlanAssigned_Monitor);
                break;
              }
            }
            
          }while(usmDbDot1xLogicalPortNextGet(lIntIfNum,&lIntIfNum) == L7_SUCCESS);
        }
      } /* End of Authenticator Data */
      else if (cap & L7_DOT1X_PAE_PORT_SUPP_CAPABLE)
      {
        /* Control Mode */
        if ( usmDbDot1xSupplicantPortControlModeGet(interface, &controlMode) == L7_SUCCESS )
        {
            cliFormat(ewsContext, pStrInfo_security_CntrlMode); /* Control Mode */
            switch (controlMode)
            {
               case L7_DOT1X_PORT_AUTO:
                ewsTelnetWrite(ewsContext,pStrInfo_common_Auto2);    /* auto */
               break;
              case L7_DOT1X_PORT_FORCE_AUTHORIZED:
                ewsTelnetWrite(ewsContext,pStrInfo_security_ForceAuthorizedDot1x);    /* force-authorized */
              break;
             case L7_DOT1X_PORT_FORCE_UNAUTHORIZED:
               ewsTelnetWrite(ewsContext,pStrInfo_security_ForceUnAuthorizedDot1x);    /* force-unauthorized */
             break;
           default:
             ewsTelnetWrite(ewsContext, pStrInfo_common_NotApplicable);
            break;
          }
      }
      if ( usmDbDot1xSupplicantPortPaeStateGet(unit, interface, &val) == L7_SUCCESS ) 
      {
        cliFormat(ewsContext, pStrInfo_security_SupplicantPaeState); /* Authenticator PAE State */
        switch (val)
        {
          case L7_DOT1X_SPM_LOGOFF:
            ewsTelnetWrite(ewsContext, pStrInfo_common_Initialize);      /* "Initialize" */
            break;
          case L7_DOT1X_SPM_DISCONNECTED:
            ewsTelnetWrite(ewsContext, pStrInfo_security_Disconnected);      /* Disconnected */
            break;
          case L7_DOT1X_SPM_CONNECTING:
            ewsTelnetWrite(ewsContext, pStrInfo_security_Connecting);      /* Connecting */
            break;
          case L7_DOT1X_SPM_AUTHENTICATING:
            ewsTelnetWrite(ewsContext, pStrInfo_security_Authenticating);      /* Authenticating */
            break;
          case L7_DOT1X_SPM_AUTHENTICATED:
            ewsTelnetWrite(ewsContext, pStrInfo_common_ClientAuth);      /* Authenticated */
            break;
          case L7_DOT1X_SPM_RESTART:
            ewsTelnetWrite(ewsContext, pStrInfo_security_Aborting);      /* Aborting */
            break;
          case L7_DOT1X_SPM_HELD:
            ewsTelnetWrite(ewsContext, pStrInfo_security_Held);      /* Held */
            break;
          case L7_DOT1X_SPM_FORCE_AUTH:
            ewsTelnetWrite(ewsContext, pStrInfo_security_ForceAuthorized);      /* Force Authorized */
            break;
          case L7_DOT1X_SPM_FORCE_UNAUTH:
            ewsTelnetWrite(ewsContext, pStrInfo_security_ForceUnAuthorized);      /* Force Unauthorized */
            break;
          default:
            ewsTelnetWrite(ewsContext, pStrInfo_common_NotApplicable);      /* "N/A" */
            break;
          }
      }
      if ( usmDbDot1xSupplicantPortSbmStateGet(unit, interface, &val) == L7_SUCCESS )
      {
        cliFormat(ewsContext, pStrInfo_security_BackEndAuthState); /* Authenticator PAE State */
        switch (val)
        {
          case L7_DOT1X_SBM_REQUEST:
            ewsTelnetWrite(ewsContext, pStrInfo_security_Req);      /* Request */
            break;
          case L7_DOT1X_SBM_RESPONSE:
            ewsTelnetWrite(ewsContext, pStrInfo_security_Resp);      /* Response */
            break;
          case L7_DOT1X_SBM_SUCCESS:
            ewsTelnetWrite(ewsContext, pStrInfo_common_ApActionSuccess);      /* Success */
            break;
          case L7_DOT1X_SBM_FAIL:
            ewsTelnetWrite(ewsContext, pStrInfo_common_Fail);      /* Fail */
            break;
          case L7_DOT1X_SBM_TIMEOUT:
            ewsTelnetWrite(ewsContext, pStrInfo_security_Timeout_1);      /* Timeout */
            break;
          case L7_DOT1X_SBM_IDLE:
            ewsTelnetWrite(ewsContext, pStrInfo_common_Idle_1);      /* Idle */
            break;
          case L7_DOT1X_SBM_INITIALIZE:
            ewsTelnetWrite(ewsContext, pStrInfo_common_Initialize);      /* "Initialize" */
            break;
          case L7_DOT1X_SBM_RECEIVE:
            ewsTelnetWrite(ewsContext, "Receive");      /* "Initialize" */
            break;
          default:
            ewsTelnetWrite(ewsContext, pStrInfo_common_NotApplicable);      /* "N/A" */
            break; 
        }
     }

      if (usmDbDot1xSupplicantPortMaxStartGet(interface, &val) == L7_SUCCESS )
      {
        cliFormat(ewsContext, pStrInfo_security_MaxStarts); /* Maximum Requests */
        ewsTelnetPrintf (ewsContext, "%u", val );
      }

      if ( usmDbDot1xSupplicantPortStartPeriodGet(interface, &val) == L7_SUCCESS )
      {
        cliFormat(ewsContext, pStrInfo_security_StartPeriodSecs); /* Start Period (secs) */
        ewsTelnetPrintf (ewsContext, "%u", val );
      }
      if ( usmDbDot1xSupplicantPortHeldPeriodGet(interface, &val) == L7_SUCCESS )
      {
        cliFormat(ewsContext, pStrInfo_security_HeldPeriodSecs); /* Start Period (secs) */
        ewsTelnetPrintf (ewsContext, "%u", val );
      }
      if ( usmDbDot1xSupplicantPortAuthPeriodGet(interface, &val) == L7_SUCCESS )
      {
        cliFormat(ewsContext, pStrInfo_security_AuthPeriodSecs); /* Authentication Period (secs)*/
        ewsTelnetPrintf (ewsContext, "%u", val );
      }
    } /* End of Supplicant  Data */
   }
    else
    {
        if(L7_TRUE == cliIsPromptRespQuit())
        {
          ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
          return cliPrompt(ewsContext);
        }
    }
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
    }
    else if (strcmp(argv[index+argToken],pStrInfo_common_Dot1xShowStats) == 0)
    {
      cap = 0x00;
      usmDbDot1xPortPaeCapabilitiesGet(unit, interface, &cap);
        /* Validate the port */
        if (strlen(argv[index+argValue]) >= sizeof(stat))
        {
            sprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, pStrErr_security_ShowDot1xPortStats,                              cliSyntaxInterfaceHelp());
            return cliSyntaxReturnPrompt (ewsContext, buf);
        }
        osapiStrncpySafe(stat, argv[index+argValue], sizeof(stat));
        if (cliIsStackingSupported() == L7_TRUE)
        {
           if ((rc = cliValidSpecificUSPCheck(stat, &unit, &slot, &port)) != L7_SUCCESS)
           {
             if (rc == L7_ERROR)
             {
               ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
               ewsTelnetPrintf (ewsContext, "%u/%u/%u", unit, slot, port);
             }
             else if (rc == L7_NOT_EXIST)
             {
               ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_UnitDoesntExist);
             }
             else if (rc == L7_NOT_SUPPORTED)
             {
               ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_SlotDoesntExist);
             }
             else
             {
               ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidInput);
             }
             return cliSyntaxReturnPrompt (ewsContext, "");
          }

          /* Get interface and check its validity */
          if (usmDbIntIfNumFromUSPGet(unit, slot, port, &interface) != L7_SUCCESS ||
            cliDot1xInterfaceValidate(ewsContext, unit, interface) != L7_SUCCESS)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
            return cliSyntaxReturnPrompt (ewsContext, "%u/%u/%u", unit, slot, port);
          }
        }
        else
        {
          /* NOTE: No need to check the value of `unit` as
           *       ID of a standalone switch is always `U_IDX` (=> 1).
           */
           unit = cliGetUnitId();
           if (cliSlotPortToIntNum(ewsContext, stat, &slot, &port, &interface) != L7_SUCCESS ||
             cliDot1xInterfaceValidate(ewsContext, unit, interface) != L7_SUCCESS)
           {
             return cliPrompt(ewsContext);
           }
        }

        /* Port */
        cliFormat(ewsContext,pStrInfo_common_Port_2); /* "Port" */
        ewsTelnetWrite(ewsContext, stat);
        cap = 0x00;
        if ( usmDbDot1xPortPaeCapabilitiesGet(unit, interface, &cap) == L7_SUCCESS )
        {
          cliFormat(ewsContext,pStrInfo_security_PaeCapabilities); /* PAE Capabilities */
          if (cap & L7_DOT1X_PAE_PORT_AUTH_CAPABLE)
          {
            ewsTelnetWrite(ewsContext, pStrInfo_security_Authenticator);
          }
          if (cap & L7_DOT1X_PAE_PORT_SUPP_CAPABLE)
          {
            ewsTelnetWrite(ewsContext, pStrInfo_security_Supplicant);
          }
       }

      if (cap & L7_DOT1X_PAE_PORT_AUTH_CAPABLE)
      {
        if (usmDbDot1xPortEapolFramesRxGet(unit, interface, &val) == L7_SUCCESS)
        {
          cliFormat(ewsContext, pStrInfo_security_EapolFramesRcvd);   /* EAPOL Frames Received */
          ewsTelnetPrintf (ewsContext, "%u", val);
        }
        if (usmDbDot1xPortEapolFramesTxGet(unit, interface, &val) == L7_SUCCESS)
        {
          cliFormat(ewsContext, pStrInfo_common_EapolFramesTxed);   /* EAPOL Frames Transmitted */
          ewsTelnetPrintf (ewsContext, "%u", val);
        }
        if (usmDbDot1xPortEapolStartFramesRxGet(unit, interface, &val) == L7_SUCCESS)
        {
          cliFormat(ewsContext, pStrInfo_common_EapolStartFramesRcvd);   /* EAPOL Start Frames Received*/
          ewsTelnetPrintf (ewsContext, "%u", val);
        }
        if (usmDbDot1xPortEapolLogoffFramesRxGet(unit, interface, &val) == L7_SUCCESS)
        {
          cliFormat(ewsContext, pStrInfo_security_EapolLogoffFramesRcvd);   /* EAPOL Logoff Frames Received */
          ewsTelnetPrintf (ewsContext, "%u", val);
        }

       if (usmDbDot1xPortLastEapolFrameVersionGet(unit, interface, &val) == L7_SUCCESS)
       {
         cliFormat(ewsContext, pStrInfo_security_LastEapolFrameVer);   /* Last EAPOL Frame Version */
         ewsTelnetPrintf (ewsContext, "%u", val);
       }
       if (usmDbDot1xPortLastEapolFrameSourceGet(unit, interface, macAddr) == L7_SUCCESS)
       {
         cliFormat(ewsContext, pStrInfo_security_LastEapolFrameSrc);   /* Last EAPOL Frame Source */
         ewsTelnetPrintf (ewsContext, "%02X:%02X:%02X:%02X:%02X:%02X",
                macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);
       }

       if (usmDbDot1xPortEapRespIdFramesRxGet(unit, interface, &val) == L7_SUCCESS)
       {
         cliFormat(ewsContext, pStrInfo_security_EapRespIdFramesRcvd_1);   /* EAP Response/Id Frames Received */
         ewsTelnetPrintf (ewsContext, "%u", val);
       }
       if (usmDbDot1xPortEapResponseFramesRxGet(unit, interface, &val) == L7_SUCCESS)
      {
        cliFormat(ewsContext, pStrInfo_security_EapRespFramesRcvd);   /* EAP Response Frames Received */
        ewsTelnetPrintf (ewsContext, "%u", val);
      }

      if (usmDbDot1xPortEapReqIdFramesTxGet(unit, interface, &val) == L7_SUCCESS)
      {
        cliFormat(ewsContext, pStrInfo_security_EapReqIdFramesTxed_1);   /* EAP Request/Id Frames Transmitted */
        ewsTelnetPrintf (ewsContext, "%u", val);
      }

      if (usmDbDot1xPortEapReqFramesTxGet(unit, interface, &val) == L7_SUCCESS)
      {
        cliFormat(ewsContext, pStrInfo_security_EapReqFramesTxed);   /* EAP Request Frames Transmitted */
        ewsTelnetPrintf (ewsContext, "%u", val);
      }

      if (usmDbDot1xPortInvalidEapolFramesRxGet(unit, interface, &val) == L7_SUCCESS)
      {
        cliFormat(ewsContext, pStrErr_security_EapolFramesRcvd_1);   /* Invalid EAPOL Frames Received */
        ewsTelnetPrintf (ewsContext, "%u", val);
      }

      if (usmDbDot1xPortEapLengthErrorFramesRxGet(unit, interface, &val) == L7_SUCCESS)
      {
        cliFormat(ewsContext, pStrErr_security_EapolLenFramesRcvd);   /* EAPOL Length Error Frames Received */
        ewsTelnetPrintf (ewsContext, "%u", val);
      }
    }
    else if (cap & L7_DOT1X_PAE_PORT_SUPP_CAPABLE)
    {
        if (usmDbDot1xSupplicantPortEapolFramesRxGet(interface, &val) == L7_SUCCESS)
        {
          cliFormat(ewsContext, pStrInfo_security_EapolFramesRcvd);   /* EAPOL Frames Received */
          ewsTelnetPrintf (ewsContext, "%u", val);
        }
        if (usmDbDot1xSupplicantPortEapolFramesTxGet(interface, &val) == L7_SUCCESS)
        {
          cliFormat(ewsContext, pStrInfo_common_EapolFramesTxed);   /* EAPOL Frames Transmitted */
          ewsTelnetPrintf (ewsContext, "%u", val);
        }
        if (usmDbDot1xSupplicantPortEapolStartFramesTxGet(interface, &val) == L7_SUCCESS)
        {
          cliFormat(ewsContext, pStrInfo_common_EapolStartFramesTxed);   /* EAPOL Start Frames Transmitted*/
          ewsTelnetPrintf (ewsContext, "%u", val);
        }

        if (usmDbDot1xSupplicantPortEapolLogoffFramesTxGet(interface, &val) == L7_SUCCESS)
        {
          cliFormat(ewsContext, pStrInfo_security_EapolLogoffFramesRcvd);   /* EAPOL Logoff Frames Received*/
          ewsTelnetPrintf (ewsContext, "%u", val);
        }
        if (usmDbDot1xSupplicantPortLastEapolFrameVersionGet(interface, &val) == L7_SUCCESS)
        {
          cliFormat(ewsContext, pStrInfo_security_LastEapolFrameVer);   /* Last EAPOL Frame Version */
          ewsTelnetPrintf (ewsContext, "%u", val);
        }
        if (usmDbDot1xSupplicantPortLastEapolFrameSourceGet(interface, (L7_enetMacAddr_t *)macAddr) == L7_SUCCESS)
        {
          cliFormat(ewsContext, pStrInfo_security_LastEapolFrameSrc);   /* Last EAPOL Frame Source */
          ewsTelnetPrintf (ewsContext, "%02X:%02X:%02X:%02X:%02X:%02X",
                macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);
        }
        if (usmDbDot1xSupplicantPortEapolRespIdFramesTxGet(interface, &val) == L7_SUCCESS)
        {
          /* EAP Response/Id Frames Transmitted */
         cliFormat(ewsContext, pStrInfo_security_EapRespIdFramesTxed_1);
         ewsTelnetPrintf (ewsContext, "%u", val);
       }
       if (usmDbDot1xSupplicantPortEapolResponseFramesTxGet(interface, &val) == L7_SUCCESS)
       {
         cliFormat(ewsContext, pStrInfo_security_EapRespFramesTxed);   /* EAP Response Frames Transmitted */
         ewsTelnetPrintf (ewsContext, "%u", val);
       }
       if (usmDbDot1xSupplicantPortEapolReqIdFramesRxGet(interface, &val) == L7_SUCCESS)
       {
         cliFormat(ewsContext, pStrInfo_security_EapReqIdFramesRcvd_1);   /* EAP Request/Id Frames Received */
         ewsTelnetPrintf (ewsContext, "%u", val);
       }

       if (usmDbDot1xSupplicantPortEapolRequestFramesRxGet(interface, &val) == L7_SUCCESS)
       {
         cliFormat(ewsContext, pStrInfo_security_EapReqFramesRcvd);   /* EAP Request Frames Received */
         ewsTelnetPrintf (ewsContext, "%u", val);
       }
       if (usmDbDot1xSupplicantPortInvalidEapolFramesRxGet(interface, &val) == L7_SUCCESS)
       {
         cliFormat(ewsContext, pStrErr_security_EapolFramesRcvd_1);   /* Invalid EAPOL Frames Received */
         ewsTelnetPrintf (ewsContext, "%u", val);
       }
       if (usmDbDot1xSupplicantPortEapLengthErrorFramesRxGet(interface, &val) == L7_SUCCESS)
       {
         cliFormat(ewsContext, pStrErr_security_EapolLenFramesRcvd);   /* EAPOL Length Error Frames Received */
         ewsTelnetPrintf (ewsContext, "%u", val);
       }
    }
  }
  else
  {
      sprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, pStrErr_security_ShowDot1xSummary_1, 
                         cliSyntaxInterfaceHelp(),cliSyntaxInterfaceHelp(), cliSyntaxInterfaceHelp());
      return cliSyntaxReturnPrompt (ewsContext, buf);
  }
  }

  return cliSyntaxReturnPrompt (ewsContext, "");

}

/*********************************************************************
*
* @purpose   display dot1x users for a specific port
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const L7_char8 **argv
* @param uintf index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  show dot1x users <slot/port>
*
*
* @cmdhelp
*
* @cmddescript  Displays dot1x port security user information.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowDot1xUsers(EwsContext ewsContext,
                                      uintf argc,
                                      const L7_char8 * * argv,
                                      uintf index)
{

  L7_uint32 unit;
  L7_uint32 slot, port;
  L7_char8 strSlotPort[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 strUserName[L7_LOGIN_SIZE];
  L7_uchar8 strUserNamePrev[L7_LOGIN_SIZE];
  L7_RC_t rc;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 interface;
  L7_uchar8 paeCap;
  L7_uint32 usrIndex;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if ( cliNumFunctionArgsGet() != 1 )
  {
    sprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, pStrErr_security_ShowDot1xPortUsrs, cliSyntaxInterfaceHelp());
    return cliSyntaxReturnPrompt (ewsContext, buf);
  }

  if (strlen(argv[index+1]) >= sizeof(strSlotPort))
  {
    sprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, pStrErr_security_ShowDot1xPortUsrs, cliSyntaxInterfaceHelp());
    return cliSyntaxReturnPrompt (ewsContext, buf);
  }

  osapiStrncpySafe( strSlotPort, argv[index+1], sizeof(strSlotPort));
  cliConvertToLowerCase(strSlotPort);

  if (cliIsStackingSupported() == L7_TRUE)
  {
    if ((rc = cliValidSpecificUSPCheck(strSlotPort, &unit, &slot, &port)) != L7_SUCCESS)
    {
      if (rc == L7_ERROR)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
        ewsTelnetPrintf (ewsContext, "%u/%u/%u", unit, slot, port);
      }
      else if (rc == L7_NOT_EXIST)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_UnitDoesntExist);
      }
      else if (rc == L7_NOT_SUPPORTED)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_SlotDoesntExist);
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidInput);
      }

      return cliSyntaxReturnPrompt (ewsContext, "");
    }

    /* Get interface and check its validity */
    if (usmDbIntIfNumFromUSPGet(unit, slot, port, &interface) != L7_SUCCESS ||
        cliDot1xInterfaceValidate(ewsContext, unit, interface) != L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
      return cliSyntaxReturnPrompt (ewsContext, "%u/%u/%u", unit, slot, port);
    }
  }
  else
  {
    /* NOTE: No need to check the value of `unit` as
     *       ID of a standalone switch is always `U_IDX` (=> 1).
     */
    unit = cliGetUnitId();
    if (cliSlotPortToIntNum(ewsContext, strSlotPort, &slot, &port, &interface) != L7_SUCCESS ||
        cliDot1xInterfaceValidate(ewsContext, unit, interface) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }

  memset( strUserName, 0, sizeof(strUserName));

  paeCap = 0x00;
  if (usmDbDot1xPortPaeCapabilitiesGet(unit,interface, &paeCap) != L7_SUCCESS)
  {
      ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
      return cliSyntaxReturnPrompt (ewsContext, "%u/%u/%u", unit, slot, port);
  }
  if (paeCap & L7_DOT1X_PAE_PORT_AUTH_CAPABLE)
  { 
    if ( usmDbPortUserGetFirst(unit, interface, &usrIndex, strUserName, sizeof(strUserName)) != L7_SUCCESS )
    {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,
                 ewsContext, pStrErr_security_Dot1xUsrsNotPermitted);
    }

    ewsTelnetWriteAddBlanks (1, 0, 6, 6, L7_NULLPTR, ewsContext, pStrInfo_common_Usrs);
    ewsTelnetWrite(ewsContext, "\r\n-----------------");
    do
    {
      ewsTelnetPrintf (ewsContext, "\r\n%-15s", (L7_char8 *)strUserName );
      osapiStrncpy((L7_char8 *)strUserNamePrev, (L7_char8 *)strUserName, sizeof(strUserNamePrev));
      memset( strUserName, 0, sizeof(strUserName));
    } while (usmDbPortUserGetNext(unit, interface, &usrIndex, strUserName, sizeof(strUserName)) == L7_SUCCESS);
  }
  else
  {
    ewsTelnetWriteAddBlanks (1, 0, 6, 6, L7_NULLPTR, ewsContext, pStrInfo_common_Usrs);
    ewsTelnetWrite(ewsContext, "\r\n-----------------");
    memset( strUserName, 0, sizeof(strUserName));
    if (usmDbDot1xSupplicantPortUserNameGet (interface,strUserName)==L7_SUCCESS)
    {
      ewsTelnetPrintf (ewsContext, "\r\n%-15s", (L7_char8 *)strUserName);
    }
  }
  

  return cliSyntaxReturnPrompt (ewsContext, "");
}


/*********************************************************************
*
* @purpose  display dot1x client details
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes 
*
* @cmdsyntax  show dot1x clients <slot/port|all> 
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandShowDot1xClients(EwsContext ewsContext,
                                        L7_uint32 argc,
                                        const L7_char8 **argv,
                                        L7_uint32 index)
{
  L7_uint32 unit,interface;
  L7_uint32 slot, port,retVal,value,val,val1,controlMode;
  L7_char8 strSlotPort[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 userName[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 suppMacAddr[L7_ENET_MAC_ADDR_LEN];
  L7_uchar8 filter[2*L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc;
  L7_BOOL all;
  L7_uint32 nextInterface;
  L7_uint32 lIntIfNum;
  L7_uint32 reason;
  L7_uint32 physport;
  

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  cliCmdScrollSet( L7_FALSE);

  
  if ( cliNumFunctionArgsGet() != 1 )
  {
    sprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf,"Use 'show dot1x clients <%s>'", cliSyntaxInterfaceHelp());
    return cliSyntaxReturnPrompt (ewsContext, buf);
  }

  if (strlen(argv[index+1]) >= sizeof(strSlotPort))
  {
    sprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, "Use 'show dot1x clients <%s>'", cliSyntaxInterfaceHelp());
    return cliSyntaxReturnPrompt (ewsContext, buf);
  }

  osapiStrncpySafe( strSlotPort, argv[index+1], sizeof(strSlotPort));
  cliConvertToLowerCase(strSlotPort);

  if ( strcmp(strSlotPort, pStrInfo_common_All /* "all" */) == 0 )
  {
    all = L7_TRUE;
    interface = 1;
  }
  else
  {
    all = L7_FALSE;

    if (cliIsStackingSupported() == L7_TRUE)
    {
      if ((rc = cliValidSpecificUSPCheck(strSlotPort, &unit, &slot, &port)) != L7_SUCCESS)
      {
        if (rc == L7_ERROR)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
          ewsTelnetPrintf (ewsContext, "%u/%u/%u", unit, slot, port);
        }
        else if (rc == L7_NOT_EXIST)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_UnitDoesntExist);
        }
        else if (rc == L7_NOT_SUPPORTED)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_SlotDoesntExist);
        }
        else
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidInput);
        }
  
        return cliSyntaxReturnPrompt (ewsContext, "");
      }
  
      /* Get interface and check its validity */
      if (usmDbIntIfNumFromUSPGet(unit, slot, port, &interface) != L7_SUCCESS ||
          cliDot1xInterfaceValidate(ewsContext, unit, interface) != L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
        return cliSyntaxReturnPrompt (ewsContext, "%u/%u/%u", unit, slot, port);
      }
    }
    else
    {
      /* NOTE: No need to check the value of `unit` as
       *       ID of a standalone switch is always `U_IDX` (=> 1).
       */
      unit = cliGetUnitId();
      if (cliSlotPortToIntNum(ewsContext, strSlotPort, &slot, &port, &interface) != L7_SUCCESS ||
          cliDot1xInterfaceValidate(ewsContext, unit, interface) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }
    }
  }

  if(all == L7_TRUE)
  {
    /* Clients Authenticated using Monitor Mode */
    cliFormat(ewsContext, pStrInfo_security_Dot1xShowNumMonitorModeClients); 
    if(usmDbDot1xMonitorModeNumClientsGet(unit, &val) == L7_SUCCESS)
    {
      ewsTelnetPrintf (ewsContext, "%u", val);
    }

    /* Clients Authenticated using Dot1x */
    cliFormat(ewsContext, pStrInfo_security_Dot1xShowNumClients); 
    if(usmDbDot1xNonMonitorModeNumClientsGet(unit, &val1) == L7_SUCCESS)
    {
      ewsTelnetPrintf (ewsContext, "%u", val1);
    }
  }

  while( interface != 0)
  {
    if (usmDbVisibleInterfaceCheck(unit, interface, &retVal) == L7_SUCCESS &&
           usmDbUnitSlotPortGet(interface, &unit, &slot, &port) == L7_SUCCESS &&
           cliDot1xInterfaceValidateWithMessage(ewsContext, unit, interface, L7_FALSE ) == L7_SUCCESS )
    {
      
      if(usmDbDot1xPortControlModeGet(unit, interface, &controlMode) == L7_SUCCESS &&
         (controlMode == L7_DOT1X_PORT_AUTO_MAC_BASED || controlMode == L7_DOT1X_PORT_AUTO))
      {

        if(usmDbDot1xLogicalPortFirstGet(interface,&lIntIfNum) == L7_SUCCESS)
        {
           
           if ((controlMode == L7_DOT1X_PORT_AUTO_MAC_BASED) || 
               ((controlMode == L7_DOT1X_PORT_AUTO) && 
               (usmDbDot1xPortVlanAssignedReasonGet(unit,interface,&reason)== L7_SUCCESS)&&
               (reason != L7_DOT1X_GUEST_VLAN))) 
           {
              
               if(usmDbDot1xPortAuthControlledPortStatusGet(unit, interface, &val) == L7_SUCCESS &&
                  val == L7_DOT1X_PORT_STATUS_AUTHORIZED)
               {
                   do
                   {
                       if ((usmDbDot1xLogicalPortPaeStateGet(lIntIfNum,&val) == L7_SUCCESS)&&
                           (val == L7_DOT1X_APM_AUTHENTICATED)) 
                      {
                           /*Print Logical Port Interface*/
                           cliFormat(ewsContext, pStrInfo_security_Dot1xLogicalInterface); /* Session Time */
                           ewsTelnetPrintf (ewsContext, "%d", lIntIfNum);

                           /* Print the  port details */
                           cliFormat(ewsContext, pStrInfo_security_Dot1xShowClientInterface); 
                           osapiStrncpySafe(buf, cliDisplayInterfaceHelp(unit, slot, port), sizeof(buf));
                           ewsTelnetPrintf (ewsContext, "%s", buf ); 
        
                 
                           /* print the user details */
                           memset(userName,0,sizeof(userName));
                           if(usmdbDot1xLogicalPortUserNameGet(unit,lIntIfNum,userName) == L7_SUCCESS)
                           {
                               cliFormat(ewsContext, pStrInfo_security_Dot1xShowClientUserName); /* User Name */
                               ewsTelnetPrintf (ewsContext, "%s", userName);
                           }
                    
                           /*print the supp mac address */
                           if(usmDbDot1xLogicalPortSupplicantMacAddrGet(lIntIfNum,suppMacAddr) == L7_SUCCESS)
                           {
                               cliFormat(ewsContext, pStrInfo_security_Dot1xShowClientMacAddr); /* Supp Mac Address */
                               osapiSnprintf(buf,sizeof(buf),"%02X:%02X:%02X:%02X:%02X:%02X",
                                             suppMacAddr[0], suppMacAddr[1], suppMacAddr[2], suppMacAddr[3], suppMacAddr[4], suppMacAddr[5]);
                               ewsTelnetPrintf(ewsContext,"%s", buf);
                           }
            
                           /* print session time */
                           if(usmdbDot1xPortSessionTimeGet(unit,lIntIfNum,&value) == L7_SUCCESS)
                           {
                               cliFormat(ewsContext, pStrInfo_security_Dot1xShowClientSessionTime); /* Session Time */
                               ewsTelnetPrintf (ewsContext, "%d", value);
                           }
            
                           /* print Filter Id */
                           if ((usmDbFeaturePresentCheck(unit,L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,L7_DIFFSERV_FEATURE_SUPPORTED)==L7_TRUE)&&
                               (controlMode == L7_DOT1X_PORT_AUTO_MAC_BASED)) 
                           {
                               if(usmdbDot1xLogicalPortFilterIdGet(unit,lIntIfNum,filter) == L7_SUCCESS)
                               {
                                   cliFormat(ewsContext, pStrInfo_security_Dot1xShowClientFilterId); /* Filter Id */
                                   ewsTelnetPrintf (ewsContext, "%s", filter);
                               }
                           }
        
                           /* print vlan id associated */
                           if(usmDbDot1xLogicalPortVlanAssignmentGet(lIntIfNum,&val,&val1) == L7_SUCCESS)
                           {
                               cliFormat(ewsContext, pStrInfo_security_Dot1xShowClientVland); /* Vlan Id */
                               ewsTelnetPrintf (ewsContext, "%u ", val);
        
                               cliFormat(ewsContext, pStrInfo_security_Dot1xShowClientVlanMode); /* Vlan Assigned */
                               switch (val1)
                               {
                               case L7_DOT1X_RADIUS_ASSIGNED_VLAN:
                                    ewsTelnetPrintf (ewsContext, "%s ",pStrInfo_security_Dot1xVlanAssigned_Radius);
                                    break;
                               case L7_DOT1X_UNAUTHENTICATED_VLAN:
                                    ewsTelnetPrintf (ewsContext, "%s ",pStrInfo_security_Dot1xVlanAssigned_unauth);
                                    break;
                               case L7_DOT1X_DEFAULT_ASSIGNED_VLAN:
                                    ewsTelnetPrintf (ewsContext, "%s ",pStrInfo_security_Dot1xVlanAssigned_Default);
                                    break;
                               case L7_DOT1X_MONITOR_MODE_VLAN:
                                    ewsTelnetPrintf (ewsContext, "%s ",pStrInfo_security_Dot1xVlanAssigned_MonitorMode);
                                    break;
                               }
        
                               /*Session timeout Assigned by the Radius Server*/
                               if (usmDbDot1xLogicalPortSessionTimeoutGet(unit,lIntIfNum,&val)==L7_SUCCESS)                   
                               {   
                                   cliFormat(ewsContext,pStrInfo_security_SessionTimeout); /* Session Timeout */
                                   ewsTelnetPrintf (ewsContext, "%u ", val );
                               }
        
        
                               /*Session Termination Action Assigned by the Radius Server*/
                               if (usmDbDot1xLogicalPortTerminationActionGet(unit,lIntIfNum,&val)==L7_SUCCESS)
                               {
                                   cliFormat(ewsContext,pStrInfo_security_SessionTerminateAction); /* Session Termination Action */
                                   switch(val) 
                                   {
                                   case L7_DOT1X_TERMINATION_ACTION_RADIUS:
                                        ewsTelnetPrintf (ewsContext, "%s \n",pStrInfo_security_Dot1xSessionTerminationAction_Radius);
                                        break;
                                   case L7_DOT1X_TERMINATION_ACTION_DEFAULT:
                                        ewsTelnetPrintf (ewsContext, "%s \n",pStrInfo_security_Dot1xVlanAssigned_Default);
                                        break;
        
                                   }
                    
                               }
                           }
                        
                  
                      }/* client is authenticated*/
                      if ((usmDbDot1xPhysicalPortGet(unit,lIntIfNum,&physport)== L7_SUCCESS) &&
                           (physport != interface)) 
                          break; /*break from while loop if logical port not associated with physical port*/
                   }while(usmDbDot1xLogicalPortNextGet(lIntIfNum,&lIntIfNum) == L7_SUCCESS);
               }
           }/*port not in guest vlan*/
        }/* logical port First get = success*/
      }
    }
    if (!all)
    {
      interface = 0;
    }
    else if ( usmDbValidIntIfNumNext(interface, &nextInterface) == L7_SUCCESS )
    {
      interface = nextInterface;
    }
    else
    {
      interface = 0;
    }
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}



/*********************************************************************
*
* @purpose  display dot1x authentication History Log information
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes add range checking
*
* @cmdsyntax  show dot1x  [{authentication-history <slot/port|all>} [failed-auth-only] [detail]] 
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandShowDot1xHistoryLogInfo(EwsContext ewsContext,
                                               L7_uint32 argc,
                                               const L7_char8 * * argv,
                                               L7_uint32 index)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strSlotPort[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 arg[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 cmdBuf[L7_CLI_MAX_STRING_LENGTH]={0};
  static L7_BOOL all,
                 detailFlg=L7_FALSE, failedAuthOnly=L7_FALSE;
  static L7_uint32 interface=L7_NULL, entryIndex=L7_NULL; 
  L7_uint32 u = 1;
  L7_uint32 unit,s,p,slot,port;
  L7_uint32 ifIndex=0, numArgs, count = 0,detailCt=0;
  L7_uint32 argToken = 0;
  L7_uint32 argIntfVal = 1;
  L7_uint32 argFailedAuth = 2;
  L7_uint32 argDetail = 3;
  L7_int32  retVal;
  L7_uchar8 macAddr[6];
  L7_enetMacAddr_t macAddress;
  L7_uint32   timeStamp, accessStatus, authStatus, reasonCode;
  L7_ushort16 vlanId;
  L7_uint8  assignedType;
  L7_char8  strTimeStamp[L7_CLI_MAX_STRING_LENGTH + 1]={0};
  L7_char8  strMacAddr[L7_CLI_MAX_STRING_LENGTH + 1]={0};
  L7_char8  strVlanId[L7_CLI_MAX_STRING_LENGTH + 1]={0};
  L7_uchar8 strFilter[L7_CLI_MAX_STRING_LENGTH + 1]={0};
  L7_uint32 filterLen=0;
#if 0
  L7_char8  strAccessStatus[L7_CLI_MAX_STRING_LENGTH + 1]={0};
#endif
  L7_char8  strAuthStatus[L7_CLI_MAX_STRING_LENGTH + 1]={0};
  L7_char8  strInterface[L7_CLI_MAX_STRING_LENGTH + 1]={0};
  L7_char8  strReason[L7_CLI_MAX_STRING_LENGTH * 2]={0};
  L7_uint32 strLen;
  L7_RC_t   rc;
  L7_BOOL   showFlag = L7_TRUE, moreFlg = L7_FALSE;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  cliCmdScrollSet( L7_FALSE);

  numArgs = cliNumFunctionArgsGet();
  
  if ((numArgs < 1) || ( numArgs > 3))
  {
    osapiStrncpySafe(arg, argv[index], sizeof(arg));
    sprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, pStrErr_security_ShowDot1xAuthHistory, cliSyntaxInterfaceHelp());
    return cliSyntaxReturnPrompt (ewsContext, buf);
  }

  if (numArgs == 2)
  {
    osapiStrncpySafe(arg, argv[index+argFailedAuth], sizeof(arg));
    if (osapiStrncmp(arg, pStrInfo_common_Dot1xShowFailedAuthOnly, sizeof(arg)) == 0)
    {
      failedAuthOnly = L7_TRUE;  
      detailFlg = L7_FALSE;
    }
    else if(osapiStrncmp(arg, pStrInfo_common_IgmpProxyGrpsDetail, sizeof(arg)) == 0)
    {
      detailFlg = L7_TRUE;
      failedAuthOnly = L7_FALSE;
    }
  }    
  else if(numArgs == 3)
  {
    osapiStrncpySafe(arg, argv[index+argDetail], sizeof(arg));
    if(osapiStrncmp(arg, pStrInfo_common_IgmpProxyGrpsDetail, sizeof(arg)) == 0)
    {
      detailFlg = L7_TRUE;
    }
  }
  else if(numArgs == 1)
  {
    detailFlg = L7_FALSE;
    failedAuthOnly = L7_FALSE;
  }
  
  if (osapiStrncmp(argv[index+argToken], pStrInfo_common_Dot1xShowAuthHistory,sizeof(arg)) != 0)
  {
    sprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, pStrErr_security_ShowDot1xAuthHistory, cliSyntaxInterfaceHelp());
    return cliSyntaxReturnPrompt (ewsContext, buf);
  }

  if ( cliGetCharInputID() != CLI_INPUT_EMPTY ) /* Not first pass */
  {
      if(L7_TRUE == cliIsPromptRespQuit())
      {
        interface = 0;
        entryIndex = 0;
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliPrompt(ewsContext);
      }
  }
  else
  {
      if (strlen(argv[index+argIntfVal]) >= sizeof(strSlotPort))
      {
        sprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, pStrErr_security_ShowDot1xAuthHistory, cliSyntaxInterfaceHelp());
        return cliSyntaxReturnPrompt (ewsContext, buf);
      }
      osapiStrncpySafe(strSlotPort, argv[index+argIntfVal], sizeof(strSlotPort));
      cliConvertToLowerCase(strSlotPort);

      if ( strcmp(strSlotPort, pStrInfo_common_All /* "all" */) == 0 )
      {
        all = L7_TRUE;
      }
      else
      {
        all = L7_FALSE;
        if (cliIsStackingSupported() == L7_TRUE)
        {
          if ((rc = cliValidSpecificUSPCheck(argv[index+argIntfVal], &unit, &slot, &port)) != L7_SUCCESS)
          {
            if (rc == L7_ERROR)
            {
              ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
              ewsTelnetPrintf (ewsContext, "%u/%u/%u", unit, slot, port);
            }
            else if (rc == L7_NOT_EXIST)
            {
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_UnitDoesntExist);
            }
            else if (rc == L7_NOT_SUPPORTED)
            {
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_SlotDoesntExist);
            }
            else
            {
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidInput);
            }
            return cliSyntaxReturnPrompt (ewsContext, "");
          }

          /* Get interface and check its validity */
          if (usmDbIntIfNumFromUSPGet(unit, slot, port, &interface) != L7_SUCCESS ||
              cliDot1xInterfaceValidate(ewsContext, unit, interface) != L7_SUCCESS)
          {
            interface = 0;
            entryIndex = 0;
            ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
            return cliSyntaxReturnPrompt (ewsContext, "%u/%u/%u", unit, slot, port);
          }
        }
        else
        {
          /* NOTE: No need to check the value of `unit` as
           *       ID of a standalone switch is always `U_IDX` (=> 1).
           */
          unit = cliGetUnitId();
          if (cliSlotPortToIntNum(ewsContext, argv[index+argIntfVal], &slot, &port, &interface) != L7_SUCCESS ||
              cliDot1xInterfaceValidate(ewsContext, unit, interface) != L7_SUCCESS)
          {
            interface = 0;
            entryIndex = 0;
            return cliPrompt(ewsContext);
          }
        }
      }

 
      ifIndex = interface;
      if(usmDbDot1xAuthHistoryLogReverseIndexNextGet(unit, &ifIndex, &entryIndex) != L7_SUCCESS)
      {
        interface = 0;
        entryIndex = 0;
        return cliPrompt(ewsContext);
      }
 
      if((all == L7_FALSE) && (interface != ifIndex))
      {
        interface = 0;
        entryIndex = 0;
        return cliPrompt(ewsContext);
      }
      interface = ifIndex;
  } /* else first pass setup */

  if(detailFlg == L7_FALSE)
  {
    /* Print headers on each page */
    ewsTelnetWriteAddBlanks (1, 0, 0, 2, L7_NULLPTR, ewsContext,
                               pStrInfo_security_Dot1xAuthHistoryLogPortStatus);
    ewsTelnetWrite(ewsContext,
                   "\r\n--------------------- --------- ----------------- ------ ------------");
  }

  while ( interface != 0 )
  {
     if(failedAuthOnly == L7_TRUE)
     {
       showFlag = L7_FALSE;
     }

     if ( usmDbVisibleInterfaceCheck(unit, interface, &retVal) == L7_SUCCESS &&
          usmDbUnitSlotPortGet(interface, &u, &s, &p) == L7_SUCCESS &&
          cliDot1xInterfaceValidateWithMessage(ewsContext, unit, interface, L7_FALSE ) == L7_SUCCESS )
     {
       if((failedAuthOnly == L7_TRUE) && (usmDbDot1xAuthHistoryLogAuthStatusGet(unit, interface, entryIndex, &authStatus) == L7_SUCCESS)
              && (authStatus == L7_DOT1X_PORT_STATUS_UNAUTHORIZED))
       {
          showFlag = L7_TRUE;
       } 
        
       if(showFlag == L7_TRUE)
       {
          osapiStrncpy(strInterface, cliDisplayInterfaceHelp(u, s, p), L7_CLI_MAX_STRING_LENGTH);
          (void)usmDbDot1xAuthHistoryLogTimeStampGet(unit, interface, entryIndex, &timeStamp);
          (void)usmDbDot1xAuthHistoryLogVlanIdAssignedGet(unit, interface, entryIndex, &vlanId);
          memset(&macAddress, 0x00, sizeof(macAddress));
          (void)usmDbDot1xAuthHistoryLogSupplicantMacAddressGet(unit, interface, entryIndex, &macAddress);
          memcpy(macAddr, macAddress.addr, sizeof(macAddr));
          (void)usmDbDot1xAuthHistoryLogAccessStatusGet(unit, interface, entryIndex, &accessStatus);
          (void)usmDbDot1xAuthHistoryLogAuthStatusGet(unit, interface, entryIndex, &authStatus);

          switch(authStatus)
          {
            case L7_DOT1X_PORT_STATUS_AUTHORIZED:
                 osapiSnprintf(strAuthStatus, sizeof(strAuthStatus), "Authorized");
                 break;
            case L7_DOT1X_PORT_STATUS_UNAUTHORIZED:
                 osapiSnprintf(strAuthStatus, sizeof(strAuthStatus), "Unauthorized");
                 break;
            default:
                 osapiSnprintf(strAuthStatus, sizeof(strAuthStatus), "Unknown");
                 break;
          }
  
          bzero(strTimeStamp, sizeof(strTimeStamp));
          cliGetUtcTimeInMonthsAndDays(timeStamp, strTimeStamp);
        
          osapiSnprintf(strMacAddr,sizeof(strMacAddr),"%02X:%02X:%02X:%02X:%02X:%02X",
                    macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
          if(detailFlg == L7_FALSE)
          {            
            memset(stat, 0x00, sizeof(stat));
            osapiSnprintf(stat, sizeof(stat), "\r\n%-22s%-10s%-18s%-7d%s",strTimeStamp, strInterface,
                    strMacAddr, vlanId, strAuthStatus);
            ewsTelnetWrite(ewsContext,stat);
            count++;
          }
          else
          {
            cliFormat(ewsContext, "Time Stamp");
            ewsTelnetWrite(ewsContext, strTimeStamp);

            cliFormat(ewsContext, "Interface");
            ewsTelnetWrite(ewsContext, strInterface);

            cliFormat(ewsContext, "MAC-Address");
            ewsTelnetWrite(ewsContext, strMacAddr);

            (void)usmDbDot1xAuthHistoryLogVlanIdAssignedGet(unit, interface, entryIndex, &vlanId);
            memset(strVlanId, 0x00, sizeof(strVlanId));
            osapiSnprintf(strVlanId,sizeof(strVlanId),"%d", vlanId);
            cliFormat(ewsContext, "VLAN Assigned");
            ewsTelnetWrite(ewsContext, strVlanId);

            (void)usmDbDot1xAuthHistoryLogVlanAssignedTypeGet(unit, interface, entryIndex, &assignedType);
            memset(strVlanId, 0x00, sizeof(strVlanId));
            switch(assignedType)
            {
              case L7_DOT1X_DEFAULT_ASSIGNED_VLAN:
                   osapiSnprintf(strVlanId,sizeof(strVlanId),"%s", "Default Assigned VLAN");
                   break;
              case L7_DOT1X_RADIUS_ASSIGNED_VLAN:
                   osapiSnprintf(strVlanId,sizeof(strVlanId),"%s", "Radius Assigned VLAN");
                   break;
              case L7_DOT1X_UNAUTHENTICATED_VLAN:
                   osapiSnprintf(strVlanId,sizeof(strVlanId),"%s", "Unauthenticated VLAN");
                   break;
              case L7_DOT1X_GUEST_VLAN:
                   osapiSnprintf(strVlanId,sizeof(strVlanId),"%s", "Guest VLAN");
                   break;
              case L7_DOT1X_VOICE_VLAN:
                   osapiSnprintf(strVlanId,sizeof(strVlanId),"%s", "Voice VLAN");
                   break;
              case L7_DOT1X_MONITOR_MODE_VLAN:
                   osapiSnprintf(strVlanId,sizeof(strVlanId),"%s", "Monitor Mode VLAN");
                   break;
              case L7_DOT1X_NOT_ASSIGNED:
                   osapiSnprintf(strVlanId,sizeof(strVlanId),"%s", "Not Assigned");
                   break;
            } 

            cliFormat(ewsContext, "VLAN Assigned Reason");
            ewsTelnetWrite(ewsContext, strVlanId);

            filterLen = sizeof(strFilter);
            if(usmDbDot1xAuthHistoryLogFilterIdGet(unit, interface, entryIndex, strFilter, &filterLen) == L7_SUCCESS)
            {
              cliFormat(ewsContext, "Filter Name");
              ewsTelnetWrite(ewsContext, strFilter);
            }

            cliFormat(ewsContext, "Auth Status");
            ewsTelnetWrite(ewsContext, strAuthStatus);

            rc= usmDbDot1xAuthHistoryLogReasonCodeGet(unit, interface, entryIndex, &reasonCode);
            if(rc == L7_SUCCESS)
            {
              memset(strReason, 0x00, sizeof(strReason));
              strLen = sizeof(strReason);  
              usmDbDot1xAuthHistoryLogReasonStringGet(interface, entryIndex, reasonCode, strReason, &strLen);
              cliFormat(ewsContext, "Reason");
              ewsTelnetWrite(ewsContext, strReason);
            }
            ewsTelnetWrite(ewsContext, "\n\r");

            detailCt += 9; 
            if(detailCt >= CLI_MAX_SCROLL_LINES-8) 
            {
              moreFlg = L7_TRUE;
            }
          }
       } /* end if (showFlag) */
     } /* end if valid interface */

     ifIndex = interface;
     if(usmDbDot1xAuthHistoryLogReverseIndexNextGet(unit, &ifIndex, &entryIndex) != L7_SUCCESS)
     {
       interface = 0; 
     }
     else if(ifIndex != interface)
     {
       if(all == L7_FALSE)
       {
         interface = 0;
       }
       else
       {
         interface = ifIndex;
       } 
     }

     if(((moreFlg == L7_TRUE) && (interface != 0)) ||
        ((count == CLI_MAX_SCROLL_LINES-4) && (interface != 0)))
     {
        cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
        cliSyntaxBottom(ewsContext);
        osapiSnprintf(cmdBuf, sizeof(cmdBuf), argv[0]);
        for (count=1; count<argc; count++)
        {
          OSAPI_STRNCAT(cmdBuf, " ");
          OSAPI_STRNCAT(cmdBuf, argv[count]);
        }
        cliAlternateCommandSet(cmdBuf);
        return pStrInfo_common_Name_2;    /* --More-- or (q)uit */
     }
  } /* end while more interfaces */

  interface = 0;
  entryIndex = 0;
  return cliSyntaxReturnPrompt (ewsContext, "");

}
