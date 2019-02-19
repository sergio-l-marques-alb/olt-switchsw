/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
 * @filename src/mgmt/cli/qos/cos/cli_show_voip.c
*
* @purpose Class of Service show commands for the cli
*
* @component user interface
*
* @comments
*
* @create  11/23/2007
*
* @author  Amitabha Sen
* @end
*
**********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_qos_common.h"
#include "strlib_qos_cli.h"
#include "l7_common.h"
#include "cos_exports.h"
#include "cli_web_exports.h"
#include "usmdb_util_api.h"
#include "cliapi.h"

#include <datatypes.h>
#include "default_cnfgr.h"
#include "clicommands_card.h"
#include "clicommands_cos.h"
#include "cliutil.h"
#include "ews.h"

#include "l7_cos_api.h"
#include "usmdb_qos_voip_api.h"
#include "strlib_qos_cli.h"
/*********************************************************************
*
* @purpose  Display Auto VoIP Parameters.
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
* @cmdsyntax  show auto-voip
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowAutoVoIP(EwsContext ewsContext,
                                    L7_uint32 argc,
                                    const L7_char8 **argv,
                                    L7_uint32 index)
{
  L7_BOOL mode;
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

  L7_uint32 minBandwidth;
  L7_uint32 CosQueue;

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
  
  if( numArgs != 2 )
  {
    osapiSnprintf(buf,sizeof(buf),pStrErr_qos_AutoVoIPShow);
    ewsTelnetWrite(ewsContext, buf);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

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
        osapiSnprintf(buf,sizeof(buf),pStrErr_qos_AutoVoIPShow);
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
              usmdbQoSVoIPIsValidIntf(interface) != L7_TRUE)
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
              usmdbQoSVoIPIsValidIntf(interface) != L7_TRUE)
          {
              ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
              ewsTelnetPrintf (ewsContext, "%u/%u/%u", unit, slot, port);
              return cliSyntaxReturnPrompt (ewsContext, "");
          }
        }
      }
    } /* end first pass setup */

    ewsTelnetWrite(ewsContext,pStrErr_qos_AutoVoIPShowHead1);
    ewsTelnetWrite(ewsContext,pStrErr_qos_AutoVoIPShowHead2);

    while ( interface != 0 )
    {
      if ( usmDbVisibleInterfaceCheck(unit, interface, &retVal) == L7_SUCCESS &&
          usmDbUnitSlotPortGet(interface, &u, &s, &p) == L7_SUCCESS &&
          usmdbQoSVoIPIsValidIntf(interface) == L7_TRUE )
      {
        if (usmDbVoipIntfVoipProfileGet(unit,interface, 
                                       &mode, &minBandwidth,
                                       &CosQueue) != L7_SUCCESS)

        {
          ewsTelnetWrite( ewsContext, pStrErr_qos_AutoVoIPShowProfile);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }

        /* Port */
        osapiSnprintf(stat,sizeof(stat),"%-11s", cliDisplayInterfaceHelp(u, s, p));
        ewsTelnetWrite(ewsContext, stat);

        /* Port Auto VoIP Mode */
        switch(mode)
        {
        case L7_ENABLE:
          osapiSnprintf(buf, sizeof(buf),"%-15s",pStrInfo_common_Enbld);
          ewsTelnetWrite(ewsContext,buf);
          break;
        default:
          osapiSnprintf(buf, sizeof(buf),"%-15s",pStrInfo_common_Dsbld);
          ewsTelnetWrite(ewsContext,buf);
          break;
        }

        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_BUCKET_FEATURE_ID) == L7_TRUE)
        {
          /* Min Bandwidth */
          osapiSnprintf(buf, sizeof(buf),"%-12d",minBandwidth);
          ewsTelnetWrite(ewsContext,buf);
        }
        /* Cos Queue */
        osapiSnprintf(buf, sizeof(buf),"%-9d",CosQueue);
        ewsTelnetWrite(ewsContext,buf);
        
        ewsTelnetWrite(ewsContext, "\r\n");  
      }

      if ( !all )
      {
        interface = 0;
      }
      else
      {
        count+=1;
        if ( usmDbValidIntIfNumNext(interface, &nextInterface) == L7_SUCCESS )
          interface = nextInterface;
        else
          interface = 0;
      }
      if((count == CLI_MAX_SCROLL_LINES-4) && (interface != 0))
      {
        cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
        cliSyntaxBottom(ewsContext);
        cliAlternateCommandSet(pStrInfo_qos_ShowAutoVoIPAll);
        return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
      }
    } /* end while more interfaces */

    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
}

