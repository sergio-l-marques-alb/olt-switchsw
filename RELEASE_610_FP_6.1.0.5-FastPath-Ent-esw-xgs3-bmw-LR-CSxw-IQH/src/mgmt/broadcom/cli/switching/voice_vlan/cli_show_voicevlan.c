/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename cli_show_voicevlan.c
*
* @purpose  Voice Vlan show commands for the cli
*
* @component user interface
*
* @comments none
*
* @create 
*
* @author  
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_switching_cli.h"

#include "cliapi.h"
#include "clicommands_card.h"
#include "clicommands_voice_vlan.h"
#include "cliutil_voicevlan.h"
#include "usmdb_util_api.h"
#include "cli_web_exports.h"
#include "usmdb_voice_vlan_api.h"

/*********************************************************************
*
* @purpose  Display voice vlan is enabled.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes add range checking, replace help string
*
* @cmdsyntax  show voice vlan
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8  *commandShowVoiceVlan(EwsContext ewsContext,
    L7_uint32 argc,
    const L7_char8 **argv,
    L7_uint32 index)
{
  L7_BOOL mode;
  L7_uint32 val;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strSlotPort[L7_CLI_MAX_STRING_LENGTH];
  static L7_BOOL all;
  static L7_uint32 interface,nextInterface;
  L7_uint32 u = 1;
  L7_uint32 numArgs, count = 0;
  L7_uint32 argValue = 2;
  L7_char8 checkChar;
  L7_int32 retVal;
  L7_uint32 unit,slot,port,s,p;
  L7_RC_t   rc;
  VOICE_VLAN_CONFIG_TYPE_t type;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if(unit == 0)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  cliCmdScrollSet( L7_FALSE);

  numArgs = cliNumFunctionArgsGet();
  
  if(( numArgs != 0 )&&( numArgs != 2 ))
  {
    osapiSnprintf(buf,sizeof(buf),CLISYNTAX_SHOWVOICEVLANPORTDETAIL, cliSyntaxInterfaceHelp());
    ewsTelnetWrite(ewsContext, buf);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if(numArgs == 0)
  {
    /* Administrative Mode */
    if(usmdbVoiceVlanAdminModeGet(unit, &mode) != L7_SUCCESS)
    {
      ewsTelnetWrite( ewsContext, CLISHOWVOICEVLANADMINMODE_ERR);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    if(mode == L7_ENABLE)
      osapiSnprintf(buf, sizeof(buf),"%-5s",pStrInfo_common_Enbl_1);
    else if(mode == L7_DISABLE)
      osapiSnprintf(buf, sizeof(buf),"%-5s",pStrInfo_common_Dsbl_1);
    else
    {
      ewsTelnetWrite( ewsContext, CLISHOWVOICEVLANADMINMODE_ERR);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    cliFormatShortText(ewsContext,"Administrative Mode"); 
    ewsTelnetWrite(ewsContext, buf);

    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);

  }
  else if (numArgs == 2)
  {
    if (cliGetCharInputID() != CLI_INPUT_EMPTY ) /* Not first pass */
    {
      checkChar = tolower(cliGetCharInput());
      if ((checkChar == 'q' ) ||
          (checkChar == 26 )) /*ctrl-z*/
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliPrompt(ewsContext);
      }
    }
    else
    {
      if (strlen(argv[index+argValue]) >= sizeof(strSlotPort))
      {
        osapiSnprintf(buf,sizeof(buf),CLISYNTAX_SHOWVOICEVLANPORTDETAIL, cliSyntaxInterfaceHelp());
        ewsTelnetWrite(ewsContext, buf);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
      osapiStrncpySafe(strSlotPort, argv[index+argValue], sizeof(strSlotPort));
      cliConvertToLowerCase(strSlotPort);

      if (strcmp(strSlotPort, pStrInfo_common_All) == 0 )
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
              ewsTelnetPrintf (ewsContext, "%u/%u/%u", unit, slot, port);            }
            else if (rc == L7_NOT_EXIST)
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_UnitDoesntExist);
            else if (rc == L7_NOT_SUPPORTED)
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_SlotDoesntExist);
            else
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidInput);

            return cliSyntaxReturnPrompt (ewsContext, "");
          }

          /* Get interface and check its validity */
          if (usmDbIntIfNumFromUSPGet(unit, slot, port, &interface) != L7_SUCCESS ||
              cliVoiceVlanInterfaceValidate(ewsContext, unit, interface) != L7_SUCCESS)
          {
              ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
              ewsTelnetPrintf (ewsContext, "%u/%u/%u", unit, slot, port);
              return cliSyntaxReturnPrompt (ewsContext, "");
          }
        }
        else
        {
          /* NOTE: No need to check the value of `unit` as
           *       ID of a standalone switch is always `U_IDX` (=> 1).
           */
          unit = cliGetUnitId();
          if (cliSlotPortToIntNum(ewsContext, argv[index+argValue], &slot, &port, &interface) != L7_SUCCESS ||
              cliVoiceVlanInterfaceValidate(ewsContext, unit, interface) != L7_SUCCESS)
            return cliPrompt(ewsContext);
        }
      }
    } /* end first pass setup */

    while ( interface != 0 )
    {
      if ( usmDbVisibleInterfaceCheck(unit, interface, &retVal) == L7_SUCCESS &&
          usmDbUnitSlotPortGet(interface, &u, &s, &p) == L7_SUCCESS &&
          cliVoiceVlanInterfaceValidateWithMessage(ewsContext, unit, interface, L7_FALSE ) == L7_SUCCESS )
      {
        cliFormat(ewsContext, CLIVOICEVLAN_INTERFACE); 
        osapiSnprintf(stat,sizeof(stat),"%s", cliDisplayInterfaceHelp(u, s, p));
        ewsTelnetWrite(ewsContext, stat);

        /* Port Mode */
        if ( usmdbVoiceVlanPortAdminModeGet(unit, interface, &type) != L7_SUCCESS )
        {
          ewsTelnetWrite( ewsContext, CLISHOWVOICEVLANMODE_ERR);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
        switch(type)
        {
        case VOICE_VLAN_CONFIG_DISABLE:
          cliFormat(ewsContext,CLIVOICEVLAN_MODE);
          osapiSnprintf(buf, sizeof(buf),"%-5s",pStrInfo_common_Dsbld);
          ewsTelnetWrite(ewsContext,buf);
          break;
        default:
          cliFormat(ewsContext,CLIVOICEVLAN_MODE);
          osapiSnprintf(buf, sizeof(buf),"%-5s",pStrInfo_common_Enbld);
          ewsTelnetWrite(ewsContext,buf);
          break;
        }

        switch(type)
        {
        case VOICE_VLAN_CONFIG_VLAN_ID:
          if(usmdbVoiceVlanPortVlanIdGet(unit, interface, &val) == L7_SUCCESS &&
             val != FD_VOICE_VLAN_VLAN_ID)
          {
            cliFormat(ewsContext, CLIVOICEVLAN_ID); 
            sprintf( stat, "%u", val );
            ewsTelnetWrite(ewsContext, stat);
          }
          break;
        case VOICE_VLAN_CONFIG_DOT1P:
          if(usmdbVoiceVlanPortVlanPriorityGet(unit, interface, &val) == L7_SUCCESS &&
                  val != FD_VOICE_VLAN_PRIORITY)
          {
            cliFormat(ewsContext, CLIVOICEVLAN_PRIORITY); 
            sprintf( stat, "%u", val );
            ewsTelnetWrite(ewsContext, stat);
          }
          break;
        case VOICE_VLAN_CONFIG_NONE:
          if(usmdbVoiceVlanPortNoneGet(unit, interface, &mode) == L7_SUCCESS &&
                  mode != FD_VOICE_VLAN_NONE_MODE)
          {
            cliFormat(ewsContext, CLIVOICEVLAN_NONE); 
            if (mode == L7_TRUE)
              ewsTelnetWrite(ewsContext, pStrInfo_common_True2);   /* true */
            else if (mode == L7_FALSE)
              ewsTelnetWrite(ewsContext, pStrInfo_common_False2);   /* false */
          }
          break;
        case VOICE_VLAN_CONFIG_UNTAGGED:
          if(usmdbVoiceVlanPortUntagGet(unit, interface, &mode) == L7_SUCCESS &&
                  mode != FD_VOICE_VLAN_TAGGING)
          {
            cliFormat(ewsContext, CLIVOICEVLANUNTAG); 
            if (mode == L7_TRUE)
              ewsTelnetWrite(ewsContext, pStrInfo_common_True2);   /* true */
            else if (mode == L7_FALSE)
              ewsTelnetWrite(ewsContext, pStrInfo_common_False2);   /* false */

          }
          break;
        default:break;
        }

        if(usmdbVoiceVlanPortCosOverrideGet(unit, interface, &mode) == L7_SUCCESS)
        {
          cliFormat(ewsContext, CLIVOICEVLANCOSOVERRIDE); 
          if (mode == L7_TRUE)
            ewsTelnetWrite(ewsContext, pStrInfo_common_True2);   /* true */
          else if (mode == L7_FALSE)
            ewsTelnetWrite(ewsContext, pStrInfo_common_False2);   /* false */
        }
        if(usmdbVoiceVlanPorStatusGet(unit, interface, &mode) == L7_SUCCESS)
        {
          cliFormat(ewsContext, CLIVOICEVLAN_STATUS); 
          if (mode == L7_ENABLE)
            ewsTelnetWrite(ewsContext, pStrInfo_common_Enbld);   
          else if (mode == L7_DISABLE)
            ewsTelnetWrite(ewsContext, pStrInfo_common_Dsbld);   
        }
        ewsTelnetWrite(ewsContext, "\n");  
      }

      if ( !all )
      {
        interface = 0;
      }
      else
      {
        count+=5;
        if ( usmDbValidIntIfNumNext(interface, &nextInterface) == L7_SUCCESS )
          interface = nextInterface;
        else
          interface = 0;
      }
      if((count == CLI_MAX_SCROLL_LINES-4) && (interface != 0))
      {
        cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
        cliSyntaxBottom(ewsContext);
        cliAlternateCommandSet(pStrInfo_switching_ShowVoiceVlan);
        return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
      }
    } /* end while more interfaces */

  }
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Build the tree to dispaly the Voice vlan config
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void  buildTreeUserExecVoiceVlan(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2,depth3,depth4,depth5,depth6;

  depth2 = ewsCliAddNode(depth1, pStrInfo_switching_ShowVoice, pStrInfo_switching_WriteVoiceVlanAdminMode,NULL , L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, "vlan ", pStrInfo_switching_WriteVoiceVlanAdminMode, commandShowVoiceVlan, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, "interface ", pStrInfo_switching_ShowVoiceVlanInterface, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_switching_VoiceVlanAll, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  return;
}

