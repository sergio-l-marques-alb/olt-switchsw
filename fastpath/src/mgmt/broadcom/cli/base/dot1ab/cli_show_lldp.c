/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
 * @filename src/mgmt/cli/base/cli_show_lldp.c
*
* @purpose LLDP show commands for CLI
*
* @component user interface
*
* @comments none
*
* @create  11/03/2005
*
* @author  Rama Krishna Hazari
* @end
*
**********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "l7_common.h"
#include "cli_web_exports.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"
#include "datatypes.h"
#include "cli_macro.h"
#include "ews.h"
#include <stdio.h>

#include "clicommands_lldp.h"
#include "usmdb_lldp_api.h"
#include "clicommands_card.h"
#include "defaultconfig.h"

static L7_RC_t cliLLDPIntIfNumUSPGet(EwsContext      ewsContext,
                                     const L7_char8 * arg,
                                     L7_uint32 * intIfNum,
                                     L7_uint32 * unit,
                                     L7_uint32 * slot,
                                     L7_uint32 * port)

{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  memset (buf, 0, sizeof(buf));
  if (cliIsStackingSupported() == L7_TRUE)
  {
    L7_RC_t rc = cliValidSpecificUSPCheck(arg, unit, slot, port);
    switch (rc)
    {
      case L7_SUCCESS:
        /* Get internal interface number */
        if (usmDbIntIfNumFromUSPGet(*unit, *slot, *port, intIfNum) == L7_SUCCESS)
        {
          return L7_SUCCESS;
        }
      osapiSnprintf(buf, sizeof(buf), cliDisplayInterfaceHelp(*unit, *slot, *port));
      ewsTelnetWriteAddBlanks(1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
        ewsTelnetWrite(ewsContext, buf);
        break;
      case L7_ERROR:
      osapiSnprintf(buf, sizeof(buf), cliDisplayInterfaceHelp(*unit, *slot, *port));
      ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
        ewsTelnetWrite(ewsContext, buf);
        break;
      case L7_NOT_EXIST:
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_UnitDoesntExist);
        break;
      case L7_NOT_SUPPORTED:
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_SlotDoesntExist);
        break;
      default:
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidInput);
        break;
    }
    cliSyntaxBottom(ewsContext);
    return L7_FAILURE;
  }

  return cliSlotPortToIntNum(ewsContext, arg, slot, port, intIfNum);
}

/*********************************************************************
*
* @purpose  Displaying LLDP Information
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8 *
* @returns    cliPrompt(ewsContext)
*
* @notes    None
*
* @cmdsyntax  show lldp
*
* @cmdhelp    Display the current LLDP configuration summary
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandLLDPShow(EwsContext       ewsContext,
                                L7_uint32        argc,
                                const L7_char8 * * argv,
                                L7_uint32        index)
{
  L7_char8  buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 txMode = 0;
  L7_uint32 rxMode = 0;
  L7_uint32 intIfNum = 0;
  L7_BOOL   enabled = L7_FALSE;
  L7_uint32 interval = 0;
  L7_uint32 delay = 0;
  L7_uint32 notifyInterval = 0;
  L7_uint32 hold = 0;

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  while (usmDbGetNextPhysicalIntIfNumber(intIfNum, &intIfNum) == L7_SUCCESS)
  {
    if ((usmDbLldpIntfRxModeGet(intIfNum, &rxMode) != L7_SUCCESS) ||
        (usmDbLldpIntfTxModeGet(intIfNum, &txMode) != L7_SUCCESS))
    {
      continue;
    }
    if ((txMode != L7_ENABLE) && (rxMode != L7_ENABLE))
    {
      continue;
    }
    enabled = L7_TRUE;
    break;
  }

  if (enabled == L7_FALSE)
  {
    ewsTelnetWriteAddBlanks(1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_base_LldpTxReceiveDsbldOnAllIntfs );
    return cliPrompt(ewsContext);
  }

  /* Get the Transmit Interval */
  if ((usmDbLldpTxIntervalGet(&interval) != L7_SUCCESS) ||
      (usmDbLldpTxReinitDelayGet(&delay) != L7_SUCCESS) ||
      (usmDbLldpTxHoldGet(&hold) != L7_SUCCESS) ||
      (usmDbLldpNotificationIntervalGet(&notifyInterval) != L7_SUCCESS))
  {
    return cliPrompt(ewsContext);
  }

  ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_LldpGlobalCfg);

  memset (buf, 0, sizeof(buf));
  cliFormatAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_TxIntvl); /* Transmit Interval */
  ewsTelnetPrintf (ewsContext, pStrInfo_common_Seconds, interval);

  memset (buf, 0, sizeof(buf));
  cliFormatAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_TxHoldMultiplier);/* Transmit Hold Multiplier */
  ewsTelnetPrintf (ewsContext, "%u ", hold);

  memset (buf, 0, sizeof(buf));
  cliFormatAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_DispLldpReinitDelay); /* Reinit Delay */
  ewsTelnetPrintf (ewsContext, pStrInfo_common_Seconds, delay);

  memset (buf, 0, sizeof(buf));
  cliFormatAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_NotificationIntvl); /* Notification Interval */
  ewsTelnetPrintf (ewsContext, pStrInfo_common_Seconds, notifyInterval);

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  display one output line for show lldp interface
*
* @param    EwsContext ewsContext   @b((input))  CLI context
* @param    L7_uint32  intIfNum     @b((input))  internal interface number
* @param    L7_uint32  unit         @b((input))  unit number
* @param    L7_uint32  slot         @b((input))  slot number
* @param    L7_uint32  port         @b((input))  port number
*
* @return   void
*
* @notes    None
*
* @end
*********************************************************************/
static void cliLLDPShowInterfaceWrite(EwsContext ewsContext,
                                      L7_uint32  intIfNum,
                                      L7_uint32  unit,
                                      L7_uint32  slot,
                                      L7_uint32  port)
{
  L7_char8  buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 len         = 0;
  L7_uint32 status      = 0;
  L7_uint32 mode        = 0;
  L7_BOOL   portDesc    = L7_FALSE;
  L7_BOOL   sysName     = L7_FALSE;
  L7_BOOL   sysDesc     = L7_FALSE;
  L7_BOOL   sysCap      = L7_FALSE;
  L7_BOOL   txMgmt      = L7_FALSE;

  ewsTelnetPrintf (ewsContext, "\r\n%-11s", cliDisplayInterfaceHelp(unit, slot, port));

  memset (buf, 0, sizeof(buf));
  if (usmDbIntfStatusGet(intIfNum, &status) != L7_SUCCESS)
  {
    osapiSnprintf(buf, sizeof(buf), "%-8s", pStrErr_common_Err);
  }
  else
  {
    if (status == L7_INTF_ATTACHED)
    {
      if (usmDbIfOperStatusGet(unit, intIfNum, &status) != L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf), "%-8s", pStrErr_common_Err);
      }
      else
      {
        switch (status) /* val = up or down */
        {
          case L7_UP:
          osapiSnprintf(buf, sizeof(buf), "%-8s", pStrInfo_common_Up);
            break;
          case L7_DOWN:
          default:
          osapiSnprintf(buf, sizeof(buf), "%-8s", pStrInfo_common_Down_1);
            break;
        } /* end switch */
      }
    } /* end if */
    else
    {
      osapiSnprintf(buf, sizeof(buf), "%-8s", pStrInfo_common_Detach);
    }
  } /* end else */
  ewsTelnetWrite(ewsContext, buf);

  /* Transmit mode */
  memset (buf, 0, sizeof(buf));
  if (usmDbLldpIntfTxModeGet(intIfNum, &mode) == L7_SUCCESS)
  {
    osapiSnprintf(buf,sizeof(buf),"%-10s",strUtilEnabledDisabledGet(mode ,pStrInfo_common_Dsbld));
  }
  else
  {
    osapiSnprintf(buf, sizeof(buf), "%-10s", " ");
  }
  ewsTelnetWrite(ewsContext, buf);

  /* Receive Mode */
  memset (buf, 0, sizeof(buf));
  if (usmDbLldpIntfRxModeGet(intIfNum, &mode) == L7_SUCCESS)
  {
    osapiSnprintf(buf,sizeof(buf),"%-10s",strUtilEnabledDisabledGet(mode ,pStrInfo_common_Dsbld));
  }
  else
  {
    osapiSnprintf(buf, sizeof(buf), "%-10s", " ");
  }
  ewsTelnetWrite(ewsContext, buf);

  /* Notification Mode */
  memset (buf, 0, sizeof(buf));
  if (usmDbLldpIntfNotificationModeGet(intIfNum, &mode) == L7_SUCCESS)
  {
    osapiSnprintf(buf,sizeof(buf),"%-10s",strUtilEnabledDisabledGet(mode ,pStrInfo_common_Dsbld));
  }
  else
  {
    osapiSnprintf(buf, sizeof(buf), "%-10s", " ");
  }
  ewsTelnetWrite(ewsContext, buf);

  /* advertised TLVS */
  memset (buf, 0, sizeof(buf));
  if (usmDbLldpIntfTxTLVsGet(intIfNum, &portDesc, &sysName,
                             &sysDesc, &sysCap) == L7_SUCCESS)
  {
    if (portDesc == L7_TRUE)
    {
      OSAPI_STRNCAT(buf, pStrInfo_base_0_2);
    }
    if (sysName == L7_TRUE)
    {
      OSAPI_STRNCAT(buf, pStrInfo_base_1);
    }
    if (sysDesc == L7_TRUE)
    {
      OSAPI_STRNCAT(buf, pStrInfo_base_2);
    }
    if (sysCap == L7_TRUE)
    {
      OSAPI_STRNCAT(buf, pStrInfo_base_3);
    }
    len = strlen(buf);
    if (len > 0)
    {
      if (buf[len-1] == ',')
      {
        buf[len-1] = 0;
      }
    }
  }
  while (len < 9)
  {
    OSAPI_STRNCAT(buf, " ");
  len = strlen(buf);
  }
  ewsTelnetWrite(ewsContext, buf);

  /* Transmit Mgmt Address */
  if (usmDbLldpIntfTxMgmtAddrGet(intIfNum, &txMgmt) == L7_SUCCESS)
  {
    if (txMgmt == L7_TRUE)
    {
      ewsTelnetWrite(ewsContext, pStrInfo_base_Y);
    }
    else
    {
      ewsTelnetWrite(ewsContext, pStrInfo_base_N);
    }
  }
  else
  {
    ewsTelnetWrite(ewsContext, " ");
  }

  return;
}

/*********************************************************************
*
* @purpose  Displaying LLDP Interface Information
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8 *
* @returns    cliPrompt(ewsContext)
*
* @notes      None
*
* @cmdsyntax  show lldp interface { all | [<unit>/]<slot>/<port> }
*
* @cmdhelp    Display LLDP state for interface(s)
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandLLDPShowInterface(EwsContext       ewsContext,
                                         L7_uint32        argc,
                                         const L7_char8 * * argv,
                                         L7_uint32        index)
{
  L7_uint32   unit      = 0;
  L7_uint32   slot      = 0;
  L7_uint32   port      = 0;
  L7_uint32   numArg    = 0, count, i;
  L7_uint32   mode      = L7_DISABLE;
  L7_char8  cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  static L7_uint32   intIfNum;
  static L7_RC_t rc;

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  /* validate arguments */
  cliSyntaxTop(ewsContext);
  cliCmdScrollSet( L7_FALSE);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_ShowLldpIntf_1, cliSyntaxInterfaceHelp());
  }

  ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_LldpIntfCfg);

  if (strcmp(argv[index+1], pStrInfo_common_All) == 0)
  {

    if (cliGetCharInputID() != CLI_INPUT_EMPTY)
    { /* if our question has been answered */
      if(L7_TRUE == cliIsPromptRespQuit())
      {
        intIfNum = 0;
        rc = L7_SUCCESS;
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliPrompt(ewsContext);
      }
    }
    else
    {
      intIfNum = 0;
      /* default VLAN may not exist if the system is on its way up */
      rc = usmDbGetNextPhysicalIntIfNumber(intIfNum, &intIfNum);
      if (rc != L7_SUCCESS)
      {
        intIfNum = 0;
        rc = L7_SUCCESS;
        return cliSyntaxReturnPrompt (ewsContext, "");
      }
    }   

    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                             pStrInfo_base_IntfLinkTxReceiveNotifyTlvsMgmt );
    ewsTelnetWrite( ewsContext,
                   "\r\n---------  ------  --------  --------  --------  -------  ----" );
    /* iterate through all physical interfaces valid for LLDP */
    count =0;
    /*At The end of table we are printing two lines output.so we have to reduce two more lines */
    while ((rc == L7_SUCCESS) && (count < CLI_MAX_SCROLL_LINES-6-3))
    {
      if (usmDbUnitSlotPortGet(intIfNum, &unit, &slot, &port) == L7_SUCCESS)
      {
        cliLLDPShowInterfaceWrite(ewsContext, intIfNum, unit, slot, port);
        count++;
      }
      rc = usmDbGetNextPhysicalIntIfNumber(intIfNum, &intIfNum);
      if (rc != L7_SUCCESS)
      {
        ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf );
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_TlvCodes0PortDesc1SysName );
        ewsTelnetWriteAddBlanks (1, 0, 11, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_2SysDesc3SysCapabilities );
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        intIfNum = 0;
        rc = L7_SUCCESS;
        return cliSyntaxReturnPrompt (ewsContext, "");
      }

    }
  }
  else
  {
    if (cliLLDPIntIfNumUSPGet(ewsContext, argv[index+1],
                              &intIfNum, &unit, &slot, &port) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }

    if (usmDbLldpIntfRxModeGet(intIfNum, &mode) != L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
      ewsTelnetPrintf (ewsContext, cliDisplayInterfaceHelp(unit, slot, port));
      return cliPrompt(ewsContext);
    }

    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,
                             pStrInfo_base_IntfLinkTxReceiveNotifyTlvsMgmt );
    ewsTelnetWrite( ewsContext,
                   "\r\n---------  ------  --------  --------  --------  -------  ----" );
    cliLLDPShowInterfaceWrite(ewsContext, intIfNum, unit, slot, port);
  }

  ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf );
  ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_TlvCodes0PortDesc1SysName );
  ewsTelnetWriteAddBlanks (1, 0, 11, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_2SysDesc3SysCapabilities );

  if (strcmp(argv[index+1], pStrInfo_common_All) == 0)
  {
    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    cliSyntaxBottom(ewsContext);

    osapiSnprintf(cmdBuf, sizeof(cmdBuf), argv[0]);
    for (i=1; i<argc; i++)
    {
      OSAPI_STRNCAT(cmdBuf, " ");
      OSAPI_STRNCAT(cmdBuf, argv[i]);
    }
    cliAlternateCommandSet(cmdBuf);
    return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
  }
  else
  {
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
    return cliSyntaxReturnPrompt (ewsContext, "");
  }
}

/*********************************************************************
*
* @purpose  display one output line for show lldp statistics
*
* @param    EwsContext ewsContext   @b((input))  CLI context
* @param    L7_uint32  intIfNum     @b((input))  internal interface number
* @param    L7_uint32  unit         @b((input))  unit number
* @param    L7_uint32  slot         @b((input))  slot number
* @param    L7_uint32  port         @b((input))  port number
*
* @return   void
*
* @notes    None
*
* @end
*********************************************************************/
static void cliLLDPShowStatisticsWrite(EwsContext ewsContext,
                                       L7_uint32  intIfNum,
                                       L7_uint32  unit,
                                       L7_uint32  slot,
                                       L7_uint32  port)
{
  L7_char8    buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32   total = 0;

  /* Local Interface */
  ewsTelnetPrintf (ewsContext, "\r\n%-10s", cliDisplayInterfaceHelp(unit, slot, port));

  /* Transmit Total */
  memset (buf, 0, sizeof(buf));
  if (usmDbLldpStatsTxPortFramesTotalGet(intIfNum, &total) == L7_SUCCESS)
  {
    osapiSnprintf(buf, sizeof(buf), "%-6d", total);
  }
  else
  {
    osapiSnprintf(buf, sizeof(buf), "%-6s", " ");
  }
  ewsTelnetWrite(ewsContext, buf);

  /* Receive Total */
  memset (buf, 0, sizeof(buf));
  if (usmDbLldpStatsRxPortFramesTotalGet(intIfNum, &total) == L7_SUCCESS)
  {
    osapiSnprintf(buf, sizeof(buf), "%-6d", total);
  }
  else
  {
    osapiSnprintf(buf, sizeof(buf), "%-6s", " ");
  }
  ewsTelnetWrite(ewsContext, buf);

  /* Discards */
  memset (buf, 0, sizeof(buf));
  if (usmDbLldpStatsRxPortFramesDiscardedGet(intIfNum, &total) == L7_SUCCESS)
  {
    osapiSnprintf(buf, sizeof(buf), "%-9d", total);
  }
  else
  {
    osapiSnprintf(buf, sizeof(buf), "%-9s", " ");
  }
  ewsTelnetWrite(ewsContext, buf);

  /* Errors */
  memset (buf, 0, sizeof(buf));
  if (usmDbLldpStatsRxPortFramesErrorsGet(intIfNum, &total) == L7_SUCCESS)
  {
    osapiSnprintf(buf, sizeof(buf), "%-7d", total);
  }
  else
  {
    osapiSnprintf(buf, sizeof(buf), "%-7s", " ");
  }
  ewsTelnetWrite(ewsContext, buf);

  /* Ageouts */
  memset (buf, 0, sizeof(buf));
  if (usmDbLldpStatsRxPortAgeoutsGet(intIfNum, &total) == L7_SUCCESS)
  {
    osapiSnprintf(buf, sizeof(buf), "%-7d", total);
  }
  else
  {
    osapiSnprintf(buf, sizeof(buf), "%-7s", " ");
  }
  ewsTelnetWrite(ewsContext, buf);

  /* TLV Discards */
  if (usmDbLldpStatsRxPortTLVsDiscardedGet(intIfNum, &total) == L7_SUCCESS)
  {
    osapiSnprintf(buf, sizeof(buf), "%-9d", total);
  }
  else
  {
    osapiSnprintf(buf, sizeof(buf), "%-9s", " ");
  }
  ewsTelnetWrite(ewsContext, buf);

  /* TLV Unknowns */
  if (usmDbLldpStatsRxPortTLVsUnrecognizedGet(intIfNum, &total) == L7_SUCCESS)
  {
    osapiSnprintf(buf, sizeof(buf), "%-9d", total);
  }
  else
  {
    osapiSnprintf(buf, sizeof(buf), "%-9s", " ");
  }
  ewsTelnetWrite(ewsContext, buf);

   /* TLV MED */
  if (usmDbLldpStatsRxPortTLVsMEDGet(intIfNum, &total) == L7_SUCCESS)
  {
    osapiSnprintf(buf, sizeof(buf), "%-5d", total);
  }
  else
  {
    osapiSnprintf(buf, sizeof(buf), "%-5s", " ");
  }
  ewsTelnetWrite(ewsContext, buf);

   /* TLV 802.1 */
  if (usmDbLldpStatsRxPortTLVs8021Get(intIfNum, &total) == L7_SUCCESS)
  {
    osapiSnprintf(buf, sizeof(buf), "%-6d", total);
  }
  else
  {
    osapiSnprintf(buf, sizeof(buf), "%-6s", " ");
  }
  ewsTelnetWrite(ewsContext, buf);

   /* TLV 802.3 */
  if (usmDbLldpStatsRxPortTLVs8023Get(intIfNum, &total) == L7_SUCCESS)
  {
    osapiSnprintf(buf, sizeof(buf), "%-6d", total);
  }
  else
  {
    osapiSnprintf(buf, sizeof(buf), "%-6s", " ");
  }
  ewsTelnetWrite(ewsContext, buf);
  return;
}

/*********************************************************************
*
* @purpose  Displaying LLDP Statistics Information
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8 *
* @returns    cliPrompt(ewsContext)
*
* @notes    None
*
* @cmdsyntax  show lldp statistics { all | [<unit>/]<slot>/<port> }
*
* @cmdhelp    Display LLDP statistics for interface(s)
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandLLDPShowStatistics(EwsContext ewsContext,
                                          L7_uint32 argc,
                                          const L7_char8 * * argv,
                                          L7_uint32 index)
{
  L7_char8    buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32   update         = 0;
  L7_uint32   total          = 0;
  L7_uint32   unit           = 0;
  L7_int32    slot           = 0;
  L7_int32    port           = 0;
  L7_uint32   intIfNum       = 0;
  L7_uint32   rxMode         = L7_DISABLE;
  L7_uint32   txMode         = L7_DISABLE;
  L7_uint32   numArg         = 0;
  L7_timespec timespec;

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  /* validate arguments */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_ShowLldpStats, cliSyntaxInterfaceHelp());
  }

  ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LldpDeviceStats);

  if (usmDbLldpStatsRemoteLastUpdateGet(&update) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }
  memset (buf, 0, sizeof(buf));
  osapiConvertRawUpTime(update, &timespec);
  cliFormatAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_LastUpdate); /* Last Update */
  ewsTelnetPrintf (ewsContext, pStrInfo_base_D, timespec.days,
          timespec.hours, timespec.minutes, timespec.seconds );

  if (usmDbLldpStatsRemoteInsertsGet(&total) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }
  memset (buf, 0, sizeof(buf));
  cliFormatAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_TotalInserts); /* Total Inserts */
  ewsTelnetPrintf (ewsContext, "%d", total);

  if (usmDbLldpStatsRemoteDeletesGet(&total) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }
  memset (buf, 0, sizeof(buf));
  cliFormatAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_TotalDels); /* Total Deletes */
  ewsTelnetPrintf (ewsContext, "%d", total);

  if (usmDbLldpStatsRemoteDropsGet(&total) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }
  memset (buf, 0, sizeof(buf));
  cliFormatAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_TotalDrops); /* Total Drops */
  ewsTelnetPrintf (ewsContext, "%d", total);

  if (usmDbLldpStatsRemoteAgeoutsGet(&total) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }
  memset (buf, 0, sizeof(buf));
  cliFormatAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_TotalAgeouts); /* Total Ageouts */
  ewsTelnetPrintf (ewsContext, "%d", total);
  ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);

  if (strcmp(argv[index+1], pStrInfo_common_All) == 0)
  {
    L7_BOOL enabled = L7_FALSE;
    /* iterate through all physical interfaces valid for LLDP */
    ewsTelnetWriteAddBlanks (1, 0, 10, 0, L7_NULLPTR, ewsContext,
                             pStrInfo_base_TxReceiveTlvTlv );
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,
                             pStrInfo_base_IntfTotalTotalDiscardsErrsAgeoutsDiscardsUnknowns );
    ewsTelnetWrite(ewsContext,
                   "\r\n--------- ----- ----- -------- ------ ------ -------- -------- ---- ----- -----" );
    while (usmDbGetNextPhysicalIntIfNumber(intIfNum, &intIfNum) == L7_SUCCESS)
    {
      if ((usmDbLldpIntfRxModeGet(intIfNum, &rxMode) != L7_SUCCESS) ||
          (usmDbLldpIntfTxModeGet(intIfNum, &txMode) != L7_SUCCESS))
      {
        continue;
      }
      if ((txMode != L7_ENABLE) && (rxMode != L7_ENABLE))
      {
        continue;
      }
      if (usmDbUnitSlotPortGet(intIfNum, &unit, &slot, &port) == L7_SUCCESS)
      {
        enabled = L7_TRUE;
        cliLLDPShowStatisticsWrite(ewsContext, intIfNum, unit, slot, port);
      }
    }
    if (enabled == L7_FALSE)
    {
      ewsTelnetWriteAddBlanks (2, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_NoLocalIntfsAreEnbldToTxReceiveLldpData);
    }
  }
  else
  {
    if (cliLLDPIntIfNumUSPGet(ewsContext, argv[index+1],
                              &intIfNum, &unit, &slot, &port) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }

    if ((usmDbLldpIntfRxModeGet(intIfNum, &rxMode) != L7_SUCCESS) ||
        (usmDbLldpIntfTxModeGet(intIfNum, &txMode) != L7_SUCCESS))
    {
      osapiSnprintf(buf, sizeof(buf), cliDisplayInterfaceHelp(unit, slot, port));
      ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
      ewsTelnetWrite(ewsContext, buf);
      return cliPrompt(ewsContext);
    }

    if ((txMode != L7_ENABLE) && (rxMode != L7_ENABLE))
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_IntfIsNotEnbldToTxReceiveLldpData);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 10, 0, L7_NULLPTR, ewsContext,
                               pStrInfo_base_TxReceiveTlvTlv );
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,
                               pStrInfo_base_IntfTotalTotalDiscardsErrsAgeoutsDiscardsUnknowns );
      ewsTelnetWrite(ewsContext,
                     "\r\n--------- ----- ----- -------- ------ ------ -------- -------- ---- ----- -----" );
      cliLLDPShowStatisticsWrite(ewsContext, intIfNum, unit, slot, port);
    }
  }

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  display one output line for show remote-device
*
* @param    EwsContext ewsContext   @b((input))  CLI context
* @param    L7_uint32  intIfNum     @b((input))  internal interface number
* @param    L7_uint32  unit         @b((input))  unit number
* @param    L7_uint32  slot         @b((input))  slot number
* @param    L7_uint32  port         @b((input))  port number
*
* @return   void
*
* @notes    None
*
* @end
*********************************************************************/
static void cliLLDPShowRemoteDeviceWrite(EwsContext ewsContext,
                                         L7_uint32 intIfNum,
                                         L7_uint32 unit,
                                         L7_uint32 slot,
                                         L7_uint32 port,
                                         L7_uint32 *remIndex,
                                         L7_uint32 *timestamp,
                                         L7_uint32 *countlines)
{
  L7_char8               buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8              id[LLDP_MGMT_STRING_SIZE_MAX];
  L7_char8               sysName[LLDP_MGMT_STRING_SIZE_MAX];
  L7_ushort16            length = 0;
  lldpChassisIdSubtype_t chassisIdType = 0;
  lldpPortIdSubtype_t    portIdType = 0;
  L7_BOOL                flag = L7_TRUE;

  /* Iterate through all the remote nodes of the interface */
  while(usmDbLldpRemEntryGetNext(intIfNum,remIndex,timestamp) == L7_SUCCESS)
  {
    flag = L7_FALSE;
    *countlines = *countlines +1;

    /* Local Interface */
    ewsTelnetPrintf (ewsContext, "\r\n%-11s", cliDisplayInterfaceHelp(unit, slot, port));

    /* Remote Id */
    ewsTelnetPrintf (ewsContext, "%-8d", *remIndex);

    /* Chassis ID */
    memset(id, 0, LLDP_MGMT_STRING_SIZE_MAX);
    if ((usmDbLldpRemChassisIdSubtypeGet(intIfNum, *remIndex, *timestamp, &chassisIdType) == L7_SUCCESS) &&
        (usmDbLldpRemChassisIdGet(intIfNum, *remIndex, *timestamp, id, &length) == L7_SUCCESS))
    {
      usmDbLldpChassisIdFormat(buf, 20, chassisIdType, id, length);
    }
    while (strlen(buf) < 20)
    {
      OSAPI_STRNCAT(buf, " ");
    }
    ewsTelnetWrite(ewsContext, buf);
    ewsTelnetWrite(ewsContext, "  ");
  
    /* Port ID */
    memset(id, 0, LLDP_MGMT_STRING_SIZE_MAX);
    if ((usmDbLldpRemPortIdSubtypeGet(intIfNum, *remIndex, *timestamp, &portIdType) == L7_SUCCESS) &&
        (usmDbLldpRemPortIdGet(intIfNum, *remIndex, *timestamp, id, &length) == L7_SUCCESS))
    {
      usmDbLldpPortIdFormat(buf, 20, portIdType, id, length);
    }
    while (strlen(buf) < 18)
    {
      OSAPI_STRNCAT(buf, " ");
    }
    ewsTelnetWrite(ewsContext, buf);
    ewsTelnetWrite(ewsContext, "  ");
  
    /* System Name */
    memset(sysName, 0, LLDP_MGMT_STRING_SIZE_MAX);
    if (usmDbLldpRemSysNameGet(intIfNum, *remIndex, *timestamp, sysName) == L7_SUCCESS)
    {
      /* truncate longer than 18 */
      if (strlen(sysName) > 18)
      {
        osapiSnprintf(sysName+14, sizeof(sysName)-14, " ...");
      }
    }
    while (strlen(sysName) < 18)
    {
      OSAPI_STRNCAT(sysName, " ");
    }
    ewsTelnetWrite(ewsContext, sysName);
    if(*countlines == CLI_MAX_SCROLL_LINES-6)
    {
      break;
    }
  }

  if(flag == L7_TRUE)
  {
    /* Local Interface */
    ewsTelnetPrintf (ewsContext, "\r\n%-11s", cliDisplayInterfaceHelp(unit, slot, port));
  }
  return;
}

/*********************************************************************
*
* @purpose  Display LLDP remote device Information
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8 *
* @returns    cliPrompt(ewsContext)
*
* @notes    None
*
* @cmdsyntax  show lldp remote-device {[<unit>/]<slot>/<port> | all}
*
* @cmdhelp    Display LLDP summary state for interface(s)
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandLLDPShowRemoteDevice(EwsContext ewsContext,
                                            L7_uint32 argc,
                                            const L7_char8 * * argv,
                                            L7_uint32 index)
{
  L7_uint32   unit      = 0;
  L7_uint32   slot      = 0;
  L7_uint32   port      = 0;
  static L7_uint32   intIfNum;
  L7_uint32   numArg    = 0;
  L7_uint32   mode      = L7_DISABLE;
  static L7_uint32   remIndex=0,timestamp=0;
  L7_uint32 countlines=0;
  L7_char8    buf[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL  allInterfaces = L7_FALSE,enabled;

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  /* validate arguments */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  cliCmdScrollSet(L7_FALSE);

  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_ShowLldpRemoteDevice, cliSyntaxInterfaceHelp());
  }

  if (strcmp(argv[index+1], pStrInfo_common_All) == 0)
  {
    allInterfaces = L7_TRUE;
  }
  else
  {
    if (cliLLDPIntIfNumUSPGet(ewsContext, argv[index+1],
                              &intIfNum, &unit, &slot, &port) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }

  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {
    if (L7_TRUE == cliIsPromptRespQuit())
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
    if (allInterfaces == L7_TRUE)
    {
      intIfNum--;
    }
  }
  else
  {
    if (allInterfaces == L7_TRUE)
    {
      intIfNum = 0;
    }
    remIndex = 0;
    timestamp = 0;
    countlines = 0;
  } 
  ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_LldpRemoteDeviceSummary);
  enabled = L7_FALSE;
  /* iterate through all physical interfaces valid for LLDP */
  ewsTelnetWriteAddBlanks (1, 0, 0, 70, L7_NULLPTR, ewsContext, pStrInfo_common_WsNwLocal);
  ewsTelnetWriteAddBlanks (1, 0, 0, 9, L7_NULLPTR, ewsContext, pStrInfo_base_IntfChassisIdPortIdSysName);
  ewsTelnetWrite(ewsContext, "\r\n-------  -------   --------------------  ------------------  ------------------");
  while (1)
  {
    if (allInterfaces == L7_TRUE)
    {
      if (usmDbGetNextPhysicalIntIfNumber(intIfNum, &intIfNum) != L7_SUCCESS)
        break;
    }

    if ((usmDbLldpIntfRxModeGet(intIfNum, &mode) != L7_SUCCESS) ||
        (mode != L7_ENABLE))
    {
      if(allInterfaces == L7_TRUE)
      {
        continue;
      }
      else
      {
        break;
      }
    }
    if (usmDbUnitSlotPortGet(intIfNum, &unit, &slot, &port) == L7_SUCCESS)
    {
      enabled = L7_TRUE;
      cliLLDPShowRemoteDeviceWrite(ewsContext, intIfNum, unit, slot, port,&remIndex,&timestamp,&countlines);
      if (countlines == CLI_MAX_SCROLL_LINES-6)
      {
        break;
      }
    }
    if (allInterfaces == L7_FALSE)
    {
      break;
    }
  }

  if (enabled == L7_FALSE)
  {
     ewsTelnetWriteAddBlanks (2, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_NoLocalIntfsAreEnbldToReceiveLldpData);
  }

  if (countlines == CLI_MAX_SCROLL_LINES-6)
  {
    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    memset(buf,0,sizeof(buf));
    osapiSnprintf( buf, sizeof(buf),pStrInfo_base_ShowLldpRemoteDevice,argv[index+1]);
    cliAlternateCommandSet(buf);
    return pStrInfo_common_Name_2;    /* --More-- or (q)uit */
  }
  ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Display LLDP remote device Information
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8 *
* @returns    cliPrompt(ewsContext)
*
* @notes    None
*
* @cmdsyntax  show lldp remote-device detail [<unit>/]<slot>/<port>
*
* @cmdhelp    Display LLDP remote device data received on an interface.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandLLDPShowRemoteDeviceDetail(EwsContext ewsContext,
                                                  L7_uint32 argc,
                                                  const L7_char8 * * argv,
                                                  L7_uint32 index)
{
  L7_char8    buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32   intIfNum  = 0;
  static L7_uint32   remIndex  = 0;
  static L7_uint32   timestamp = 0;
  L7_uint32   unit      = 0;
  L7_int32    slot      = 0;
  L7_int32    port      = 0;
  L7_uint32   mode      = L7_DISABLE;
  L7_uint32   numArg    = 0;
  L7_BOOL     flag = L7_FALSE;

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  /* validate arguments */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  cliCmdScrollSet(L7_FALSE);

  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_ShowLldpRemoteDevice, cliSyntaxInterfaceHelp());
  }

  if (cliLLDPIntIfNumUSPGet(ewsContext, argv[index+1],
                            &intIfNum, &unit, &slot, &port) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  if (usmDbLldpIntfRxModeGet(intIfNum, &mode) != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
    ewsTelnetPrintf (ewsContext, cliDisplayInterfaceHelp(unit, slot, port));
    return cliPrompt(ewsContext);
  }

  if (mode != L7_ENABLE)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_IntfIsNotEnbldToReceiveLldpData);
  }
  else
  {
    if (cliGetCharInputID() != CLI_INPUT_EMPTY)
    {                                                                    
      if (L7_TRUE == cliIsPromptRespQuit())
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliPrompt(ewsContext);
      }
    }
    else
    {
      remIndex = 0;
      timestamp = 0;
      flag = L7_TRUE;
      ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LldpRemoteDeviceDetail);
      osapiSnprintfAddBlanks (1, 2, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_LocalIntf, cliDisplayInterfaceHelp(unit, slot, port));
      ewsTelnetWrite(ewsContext, buf);

    } 
  
    if(usmDbLldpRemEntryGetNext(intIfNum,&remIndex,&timestamp) == L7_SUCCESS)
    {
      L7_uchar8                   id[LLDP_MGMT_STRING_SIZE_MAX];
      L7_ushort16                 length = 0;
      L7_char8                    str[LLDP_MGMT_STRING_SIZE_MAX];
      L7_uchar8                   address[LLDP_MGMT_ADDR_SIZE_MAX];
      L7_uchar8                   addrlen = 0;
      L7_ushort16                 sysCap    = 0;
      L7_uint32                   ttl = 0;
      lldpChassisIdSubtype_t      chassisIdType = 0;
      lldpPortIdSubtype_t         portIdType = 0;
      lldpIANAAddrFamilyNumber_t  family = 0;

      flag = L7_FALSE;
      /* Remote Id */
      memset (buf, 0, sizeof(buf));
      ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
      ewsTelnetWriteAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_RemoteId);
      ewsTelnetPrintf (ewsContext, "%d", remIndex);
      ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);

      /* Chassis ID */
      memset(id, 0, LLDP_MGMT_STRING_SIZE_MAX);
      if ((usmDbLldpRemChassisIdSubtypeGet(intIfNum, remIndex, timestamp, &chassisIdType) == L7_SUCCESS) &&
          (usmDbLldpRemChassisIdGet(intIfNum, remIndex, timestamp, id, &length) == L7_SUCCESS))
      {
        ewsTelnetWriteAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_ChassisIdSubtype);
        ewsTelnetWrite(ewsContext, usmDbLldpChassisIdSubtypeString(chassisIdType));
        ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
        ewsTelnetWriteAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_ChassisId);
        usmDbLldpChassisIdFormat(buf, sizeof(buf), chassisIdType, id, length);
        ewsTelnetWrite(ewsContext, buf);
        ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
      }
      else
      {
        ewsTelnetWriteAddBlanks (0, 1, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_ChassisIdSubtype);
        ewsTelnetWriteAddBlanks (0, 1, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_ChassisId);
      }

      /* Port ID */
      memset(id, 0, LLDP_MGMT_STRING_SIZE_MAX);
      if ((usmDbLldpRemPortIdSubtypeGet(intIfNum, remIndex, timestamp, &portIdType) == L7_SUCCESS) &&
          (usmDbLldpRemPortIdGet(intIfNum, remIndex, timestamp, id, &length) == L7_SUCCESS))
      {
        ewsTelnetWriteAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_PortIdSubtype);
        ewsTelnetWrite(ewsContext, usmDbLldpPortIdSubtypeString(portIdType));
        ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
        ewsTelnetWriteAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_PortId_1);
        usmDbLldpPortIdFormat(buf, sizeof(buf), portIdType, id, length);
        ewsTelnetWrite(ewsContext, buf);
        ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
      }
      else
      {
        ewsTelnetWriteAddBlanks (0, 1, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_PortIdSubtype);
        ewsTelnetWriteAddBlanks (0, 1, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_PortId_1);
      }

      memset(str, 0, LLDP_MGMT_STRING_SIZE_MAX);
      if (usmDbLldpRemSysNameGet(intIfNum, remIndex, timestamp, str) == L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_SysName_1);
        ewsTelnetWrite(ewsContext, str);
        ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
      }
      else
      {
        ewsTelnetWriteAddBlanks (0, 1, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_SysName_1);
      }

      memset(str, 0, LLDP_MGMT_STRING_SIZE_MAX);
      if (usmDbLldpRemSysDescGet(intIfNum, remIndex, timestamp, str) == L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_SysDesc_2);
        ewsTelnetWrite(ewsContext, str);
        ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
      }
      else
      {
        ewsTelnetWriteAddBlanks (0, 1, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_SysDesc_2);
      }

      memset(str, 0, LLDP_MGMT_STRING_SIZE_MAX);
      if (usmDbLldpRemPortDescGet(intIfNum, remIndex, timestamp, str) == L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_PortDesc_1);
        ewsTelnetWrite(ewsContext, str);
        ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
      }
      else
      {
        ewsTelnetWriteAddBlanks (0, 1, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_PortDesc_1);
      }

      if (usmDbLldpRemSysCapSupportedGet(intIfNum, remIndex, timestamp, &sysCap) == L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_SysCapabilitiesSupported);
        usmDbLldpSysCapFormat(buf, sizeof(buf), sysCap);
        ewsTelnetWrite(ewsContext, buf);
        ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
      }
      else
      {
        ewsTelnetWriteAddBlanks (0, 1, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_SysCapabilitiesSupported);
      }

      if (usmDbLldpRemSysCapEnabledGet(intIfNum, remIndex, timestamp, &sysCap) == L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_SysCapabilitiesEnbld);
        usmDbLldpSysCapFormat(buf, sizeof(buf), sysCap);
        ewsTelnetWrite(ewsContext, buf);
        ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
      }
      else
      {
        ewsTelnetWriteAddBlanks (0, 1, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_SysCapabilitiesEnbld);
      }

      addrlen = 0;  /* addrlen == 0 returns first management address */
      while (usmDbLldpRemManAddrEntryNextGet(intIfNum,
                                             remIndex,
                                             timestamp,
                                             &family,
                                             address,
                                             &addrlen) == L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (0, 1, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_MgmtAddr_1);
        ewsTelnetWriteAddBlanks (0, 0, 4, 1, L7_NULLPTR, ewsContext, pStrInfo_base_Type_1);
        ewsTelnetWrite(ewsContext, usmDbLldpManAddrFamilyString(family));
        ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
        ewsTelnetWriteAddBlanks (0, 0, 4, 1, L7_NULLPTR, ewsContext, pStrInfo_base_Addr_3);
        usmDbLldpManAddrFormat(buf, sizeof(buf), family, address, addrlen);
        ewsTelnetWrite(ewsContext, buf);
        ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
      }

      if (usmDbLldpRemTTLGet(intIfNum, remIndex, timestamp, &ttl) == L7_SUCCESS)
      {
        ewsTelnetPrintf (ewsContext, pStrInfo_base_TimeToLiveSeconds, ttl);
        ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
      }
      else
      {
        ewsTelnetWriteAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_TimeToLive);
      }
    }
    else
    {
    if(flag == L7_TRUE)
    {
      /* usmDbLldpRemEntryGet failed, means there is no remote data */
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_NoLldpDataHasBeenRcvdOnIntf);
    }
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    memset(buf,0,sizeof(buf));
    osapiSnprintf( buf, sizeof(buf),pStrInfo_base_ShowLldpRemoteDeviceDetailed,argv[index+1]);
    cliAlternateCommandSet(buf);
    return pStrInfo_common_Name_2;    /* --More-- or (q)uit */
  }

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  display one output line for show local-device
*
* @param    EwsContext ewsContext   @b((input))  CLI context
* @param    L7_uint32  intIfNum     @b((input))  internal interface number
* @param    L7_uint32  unit         @b((input))  unit number
* @param    L7_uint32  slot         @b((input))  slot number
* @param    L7_uint32  port         @b((input))  port number
*
* @return   void
*
* @notes    None
*
* @end
*********************************************************************/
static void cliLLDPShowLocalDeviceWrite(EwsContext ewsContext,
                                        L7_uint32  intIfNum,
                                        L7_uint32  unit,
                                        L7_uint32  slot,
                                        L7_uint32  port)
{
  L7_char8               buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8              id[LLDP_MGMT_STRING_SIZE_MAX];
  L7_char8               desc[LLDP_MGMT_STRING_SIZE_MAX];
  L7_ushort16            length = 0;
  lldpPortIdSubtype_t    portIdType = 0;

  /* Local Interface */
  ewsTelnetPrintf (ewsContext, "\r\n%-11s", cliDisplayInterfaceHelp(unit, slot, port));

  /* Port ID */
  memset(id, 0, LLDP_MGMT_STRING_SIZE_MAX);
  if ((usmDbLldpLocPortIdSubtypeGet(intIfNum, &portIdType) == L7_SUCCESS) &&
      (usmDbLldpLocPortIdGet(intIfNum, id, &length) == L7_SUCCESS))
  {
    usmDbLldpPortIdFormat(buf, 20, portIdType, id, length);
  }
  while (strlen(buf) < 20)
  {
    OSAPI_STRNCAT(buf, " ");
  }
  ewsTelnetWrite(ewsContext, buf);
  ewsTelnetWrite(ewsContext, "  ");

  /* Port Description */
  memset(desc, 0, LLDP_MGMT_STRING_SIZE_MAX);
  if (usmDbLldpLocPortDescGet(intIfNum, desc) == L7_SUCCESS)
  {
    /* truncate longer than 20 */
    if (strlen(desc) > 20)
    {
      osapiSnprintf(desc+16, sizeof(desc)-16, " ...");
    }
  }
  while (strlen(desc) < 20)
  {
    OSAPI_STRNCAT(desc, " ");
  }
  ewsTelnetWrite(ewsContext, desc);

  return;
}

/*********************************************************************
*
* @purpose  Display LLDP Local Device Information
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8 *
* @returns    cliPrompt(ewsContext)
*
* @notes    None
*
* @cmdsyntax  show lldp local-device {<unit>/<slot>/<port> | all}
*
* @cmdhelp    Display LLDP local device data advertised on an interface.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandLLDPShowLocalDevice(EwsContext ewsContext,
                                           L7_uint32 argc,
                                           const L7_char8 * * argv,
                                           L7_uint32 index)
{
  L7_uint32   unit      = 0;
  L7_uint32   slot      = 0;
  L7_uint32   port      = 0;
  L7_uint32   intIfNum  = 0;
  L7_uint32   numArg    = 0;
  L7_uint32   mode      = L7_DISABLE;

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  /* validate arguments */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_ShowLldpLocalDevice, cliSyntaxInterfaceHelp());
  }

  ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_LldpLocalDeviceSummary);

  if (strcmp(argv[index+1], pStrInfo_common_All) == 0)
  {
    L7_BOOL enabled = L7_FALSE;
    /* iterate through all physical interfaces valid for LLDP */
    ewsTelnetWriteAddBlanks (1, 0, 0, 4, L7_NULLPTR, ewsContext, pStrInfo_base_IntfPortIdPortDesc);
    ewsTelnetWrite(ewsContext, "\r\n---------  --------------------  --------------------");
    while (usmDbGetNextPhysicalIntIfNumber(intIfNum, &intIfNum) == L7_SUCCESS)
    {
      if ((usmDbLldpIntfTxModeGet(intIfNum, &mode) != L7_SUCCESS) ||
          (mode != L7_ENABLE))
      {
        continue;
      }
      if (usmDbUnitSlotPortGet(intIfNum, &unit, &slot, &port) == L7_SUCCESS)
      {
        enabled = L7_TRUE;
        cliLLDPShowLocalDeviceWrite(ewsContext, intIfNum, unit, slot, port);
      }
    }
    if (enabled == L7_FALSE)
    {
      ewsTelnetWriteAddBlanks (2, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_NoLocalIntfsAreEnbldToTxLldpData);
    }
  }
  else
  {
    if (cliLLDPIntIfNumUSPGet(ewsContext, argv[index+1],
                              &intIfNum, &unit, &slot, &port) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }

    if (usmDbLldpIntfTxModeGet(intIfNum, &mode) != L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
      ewsTelnetPrintf (ewsContext, cliDisplayInterfaceHelp(unit, slot, port));
      return cliPrompt(ewsContext);
    }

    if (mode != L7_ENABLE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_IntfIsNotEnbldToTxLldpData);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 4, L7_NULLPTR, ewsContext, pStrInfo_base_IntfPortIdPortDesc);
      ewsTelnetWrite(ewsContext, "\r\n---------  --------------------  --------------------");
      cliLLDPShowLocalDeviceWrite(ewsContext, intIfNum, unit, slot, port);
    }
  }

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Display LLDP Local Device Information
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8 *
* @returns    cliPrompt(ewsContext)
*
* @notes    None
*
* @cmdsyntax  show lldp local-device detail [<unit>/]<slot>/<port>
*
* @cmdhelp    Display LLDP state for interface(s)
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandLLDPShowLocalDeviceDetail(EwsContext ewsContext,
                                                 L7_uint32 argc,
                                                 const L7_char8 * * argv,
                                                 L7_uint32 index)
{
  L7_char8    buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32   intIfNum  = 0;
  L7_uint32   unit      = 0;
  L7_int32    slot      = 0;
  L7_int32    port      = 0;
  L7_uint32   mode      = L7_DISABLE;
  L7_uint32   numArg    = 0;

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  /* validate arguments */
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_ShowLldpLocalDevice, cliSyntaxInterfaceHelp());
  }

  if (cliLLDPIntIfNumUSPGet(ewsContext, argv[index+1],
                            &intIfNum, &unit, &slot, &port) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  if (usmDbLldpIntfTxModeGet(intIfNum, &mode) != L7_SUCCESS)
  {
    osapiSnprintf(buf, sizeof(buf), cliDisplayInterfaceHelp(unit, slot, port));
    ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
    ewsTelnetWrite(ewsContext, buf);
    return cliPrompt(ewsContext);
  }

  if (mode != L7_ENABLE)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_IntfIsNotEnbldToTxLldpData);
  }
  else
  {
    L7_uchar8                   id[LLDP_MGMT_STRING_SIZE_MAX];
    L7_ushort16                 length = 0;
    L7_char8                    str[LLDP_MGMT_STRING_SIZE_MAX];
    L7_uchar8                   address[LLDP_MGMT_ADDR_SIZE_MAX];
    L7_uchar8                   addrlen = 0;
    L7_ushort16                 sysCap = 0;
    lldpChassisIdSubtype_t      chassisIdType = 0;
    lldpPortIdSubtype_t         portIdType = 0;
    lldpIANAAddrFamilyNumber_t  family = 0;

    ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LldpLocalDeviceDetail);

    osapiSnprintfAddBlanks (1, 2, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_Intf_2, cliDisplayInterfaceHelp(unit, slot, port));
    ewsTelnetWrite(ewsContext, buf);

    /* Chassis ID */
    memset(id, 0, LLDP_MGMT_STRING_SIZE_MAX);
    if ((usmDbLldpLocChassisIdSubtypeGet(&chassisIdType) == L7_SUCCESS) &&
        (usmDbLldpLocChassisIdGet(id, &length) == L7_SUCCESS))
    {
      ewsTelnetWriteAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_ChassisIdSubtype);
      ewsTelnetWrite(ewsContext, usmDbLldpChassisIdSubtypeString(chassisIdType));
      ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
      ewsTelnetWriteAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_ChassisId);
      usmDbLldpChassisIdFormat(buf, sizeof(buf), chassisIdType, id, length);
      ewsTelnetWrite(ewsContext, buf);
      ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
    }
    else
    {
      ewsTelnetWriteAddBlanks (0, 1, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_ChassisIdSubtype);
      ewsTelnetWriteAddBlanks (0, 1, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_ChassisId);
    }

    /* Port ID */
    memset(id, 0, LLDP_MGMT_STRING_SIZE_MAX);
    if ((usmDbLldpLocPortIdSubtypeGet(intIfNum, &portIdType) == L7_SUCCESS) &&
        (usmDbLldpLocPortIdGet(intIfNum, id, &length) == L7_SUCCESS))
    {
      ewsTelnetWriteAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_PortIdSubtype);
      ewsTelnetWrite(ewsContext, usmDbLldpPortIdSubtypeString(portIdType));
      ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
      ewsTelnetWriteAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_PortId_1);
      usmDbLldpPortIdFormat(buf, sizeof(buf), portIdType, id, length);
      ewsTelnetWrite(ewsContext, buf);
      ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
    }
    else
    {
      ewsTelnetWriteAddBlanks (0, 1, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_PortIdSubtype);
      ewsTelnetWriteAddBlanks (0, 1, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_PortId_1);
    }

    memset(str, 0, LLDP_MGMT_STRING_SIZE_MAX);
    if (usmDbLldpLocSysNameGet(str) == L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_SysName_1);
      ewsTelnetWrite(ewsContext, str);
      ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
    }
    else
    {
      ewsTelnetWriteAddBlanks (0, 1, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_SysName_1);
    }

    memset(str, 0, LLDP_MGMT_STRING_SIZE_MAX);
    if (usmDbLldpLocSysDescGet(str) == L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_SysDesc_2);
      ewsTelnetWrite(ewsContext, str);
      ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
    }
    else
    {
      ewsTelnetWriteAddBlanks (0, 1, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_SysDesc_2);
    }

    memset(str, 0, LLDP_MGMT_STRING_SIZE_MAX);
    if (usmDbLldpLocPortDescGet(intIfNum, str) == L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_PortDesc_1);
      ewsTelnetWrite(ewsContext, str);
      ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
    }
    else
    {
      ewsTelnetWriteAddBlanks (0, 1, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_PortDesc_1);
    }

    if (usmDbLldpLocSysCapSupportedGet(&sysCap) == L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_SysCapabilitiesSupported);
      usmDbLldpSysCapFormat(buf, sizeof(buf), sysCap);
      ewsTelnetWrite(ewsContext, buf);
      ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
    }
    else
    {
      ewsTelnetWriteAddBlanks (0, 1, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_SysCapabilitiesSupported);
    }

    if (usmDbLldpLocSysCapEnabledGet(&sysCap) == L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_SysCapabilitiesEnbld);
      usmDbLldpSysCapFormat(buf, sizeof(buf), sysCap);
      ewsTelnetWrite(ewsContext, buf);
      ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
    }
    else
    {
      ewsTelnetWriteAddBlanks (0, 1, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_SysCapabilitiesEnbld);
    }

    if (usmDbLldpLocManAddrGet(&family, address, &addrlen) == L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (0, 1, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_MgmtAddr_1);
      ewsTelnetWriteAddBlanks (0, 0, 4, 1, L7_NULLPTR, ewsContext, pStrInfo_base_Type_1);
      ewsTelnetWrite(ewsContext, usmDbLldpManAddrFamilyString(family));
      ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
      ewsTelnetWriteAddBlanks (0, 0, 4, 1, L7_NULLPTR, ewsContext, pStrInfo_base_Addr_3);
      usmDbLldpManAddrFormat(buf, sizeof(buf), family, address, addrlen);
      ewsTelnetWrite(ewsContext, buf);
    }
    else
    {
      ewsTelnetWriteAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_MgmtAddr_1);
    }
  }

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliSyntaxReturnPrompt (ewsContext, "");

};
/*********************************************************************
*
* @purpose  Display LLDP MED Information
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8 *
* @returns    cliPrompt(ewsContext)
*
* @notes    None
*
* @cmdsyntax  show lldp med
*
* @cmdhelp    
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandLLDPMedShow(EwsContext ewsContext,
                                   L7_uint32 argc,
                                   const L7_char8 **argv,
                                   L7_uint32 index)
{
  L7_uint32 count;

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  
  /*LLDP MED Global Configuration */
  ewsTelnetWriteAddBlanks (2, 2, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LLDPMEDGlobalConfiguration);
  if(usmDbLldpXMedFastStartRepeatCountGet(&count) == L7_SUCCESS)
  {  
    ewsTelnetPrintf(ewsContext, "%s %d", pStrInfo_base_FastStartRepeatCount, count); 
  }

  if (usmDbLldpXMedLocDeviceClassGet(&count) == L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_DeviceClass_1);
    ewsTelnetWrite(ewsContext, usmDbLldpXMedDeviceClassString(count)); 
  }
  
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  display one output line for show lldp med interface
*
* @param    EwsContext ewsContext   @b((input))  CLI context
* @param    L7_uint32  intIfNum     @b((input))  internal interface number
* @param    L7_uint32  unit         @b((input))  unit number
* @param    L7_uint32  slot         @b((input))  slot number
* @param    L7_uint32  port         @b((input))  port number
*
* @return   void
*
* @notes    None
*
* @end
*********************************************************************/

static void cliLLDPMedShowInterfaceWrite(EwsContext ewsContext, L7_uint32  intIfNum,
                                         L7_uint32  unit, L7_uint32  slot, L7_uint32  port)
{
  L7_char8  buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 len         = 0;
  L7_uint32 status      = 0;
  L7_uint32 mode        = 0;
  L7_BOOL   notif       = L7_FALSE;
  lldpXMedCapabilities_t medCap;

  ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf );
  ewsTelnetPrintf(ewsContext, "%-11s", cliDisplayInterfaceHelp(unit, slot, port));
  if(usmDbIntfStatusGet(intIfNum, &status) != L7_SUCCESS)
  {
    osapiSnprintf(buf, sizeof(buf)-1, "%-8s", pStrInfo_common_Error_2);
  }
  else
  {
    if(status == L7_INTF_ATTACHED)
    {
      if(usmDbIfOperStatusGet(unit, intIfNum, &mode) != L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf), "%-8s", pStrInfo_common_Error_2);
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf) -1, "%-8s",  strUtilUpDownGet(mode , pStrInfo_common_Down_1));
      }
    } /* end if */
    else
    {
      osapiSnprintf(buf, sizeof(buf), "%-8s", pStrInfo_common_Detach);
    }
  } /* end else */
  ewsTelnetWrite(ewsContext, buf);

  /* Config MED */
  if(usmDbLldpXMedPortAdminModeGet(intIfNum, &mode) == L7_SUCCESS)
  {
    osapiSnprintf(buf,sizeof(buf),"%-10s", strUtilEnabledDisabledGet(mode , pStrInfo_common_Dsbld));
  }
  else
  {
    osapiSnprintf(buf, sizeof(buf), "%-10s", pStrInfo_common_Dsbld);
  }
  ewsTelnetWrite(ewsContext, buf);

  /* Oper MED */
  if(usmDbLldpXMedPortTransmitEnabledGet(intIfNum, &mode) == L7_SUCCESS)
  {
    osapiSnprintf(buf,sizeof(buf),"%-10s",strUtilEnabledDisabledGet(mode , pStrInfo_common_Dsbld));
  }
  else
  {
    osapiSnprintf(buf, sizeof(buf), "%-10s", pStrInfo_common_Dsbld);
  }
  ewsTelnetWrite(ewsContext, buf);

  /* Config Notify */
  if(usmDbLldpXMedPortConfigNotifEnableGet(intIfNum, &notif) == L7_SUCCESS)
  {
    osapiSnprintf(buf,sizeof(buf),"%-13s",strUtilEnabledDisabledGet(notif , pStrInfo_common_Dsbld));
  }
  else
  {
    osapiSnprintf(buf, sizeof(buf), "%-13s", pStrInfo_common_Dsbld);
  }
  ewsTelnetWrite(ewsContext, buf);

  /* TLVsTx */
  memset(buf, 0, sizeof(buf));
  if(usmDbLldpXMedPortConfigTLVsEnabledGet(intIfNum, &medCap) == L7_SUCCESS)
  {
    if(medCap.bitmap[1] & LLDP_MED_CAP_CAPABILITIES_BITMASK)
    {
      OSAPI_STRNCAT(buf, "0,");
    }
    if(medCap.bitmap[1] & LLDP_MED_CAP_NETWORKPOLICY_BITMASK)
    {
      OSAPI_STRNCAT(buf, "1,");
    }
    if(medCap.bitmap[1] & LLDP_MED_CAP_LOCATION_BITMASK)
    {
      OSAPI_STRNCAT(buf, "2,");
    }
    if(medCap.bitmap[1] & LLDP_MED_CAP_EXT_PSE_BITMASK)
    {
      OSAPI_STRNCAT(buf, "3,");
    }
    if(medCap.bitmap[1] & LLDP_MED_CAP_EXT_PD_BITMASK)
    {
      OSAPI_STRNCAT(buf, "4,");
    }
    if(medCap.bitmap[1] & LLDP_MED_CAP_INVENTORY_BITMASK)
    {
      OSAPI_STRNCAT(buf, "5,");
    }
    len = strlen(buf);
    if(len > 0)
    {
      buf[len-1] = L7_EOS;              /* remove the trailing comma */
    }
  }
  ewsTelnetWrite(ewsContext, buf);

  return;
}

/*********************************************************************
*
* @purpose  Display LLDP MED interface Information
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8 *
* @returns    cliPrompt(ewsContext)
*
* @notes    None
*
* @cmdsyntax  show lldp med interface {<unit/slot/port> | all} *
* @cmdhelp    
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandLLDPMedShowInterface(EwsContext ewsContext,
                                            L7_uint32 argc,
                                            const L7_char8 **argv,
                                            L7_uint32 index)
{
  L7_char8    buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32   unit      = 0;
  L7_uint32   slot      = 0;
  L7_uint32   port      = 0, count =0, i=0;
  L7_uint32   numArg    = cliNumFunctionArgsGet();
  L7_uint32   mode      = L7_DISABLE;
  L7_char8  cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  static L7_uint32   intIfNum;
  static L7_RC_t rc;

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  /* validate arguments */
  ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf );

  if(numArg != 1)
  {  /*Use 'show lldp med interface {<%s> | all}'.*/
    osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, sizeof(buf), pStrInfo_base_Useshowlldpmedinterface, cliSyntaxLogInterfaceHelp());
    return cliSyntaxReturnPrompt (ewsContext, buf);
  }

  if(strcmp(argv[index+1], pStrInfo_common_All) == 0)
  {
    if (cliGetCharInputID() != CLI_INPUT_EMPTY)
    {  /* if our question has been answered */
      if(L7_TRUE == cliIsPromptRespQuit())
      {
        intIfNum = 0;
        rc = L7_SUCCESS;
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliPrompt(ewsContext);
      }
    }
    else
    {
      intIfNum = 0;
      /* default VLAN may not exist if the system is on its way up */
      rc = usmDbGetNextPhysicalIntIfNumber(intIfNum, &intIfNum);
      if (rc != L7_SUCCESS)
      {
        intIfNum = 0;
        rc = L7_SUCCESS;
        return cliSyntaxReturnPrompt (ewsContext, "");
      }
    }

    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_DsplyLLDPMedShowIntfDetail);
    ewsTelnetWrite(ewsContext, "\r\n---------  ------  --------- --------  ------------ -----------" );

    /* iterate through all physical interfaces valid for LLDP */
    count =0;
    /*At The end of table we are printing four lines output.so we have to reduce four  more lines */
    while ((rc == L7_SUCCESS) && (count < CLI_MAX_SCROLL_LINES-6-4))
    {
      if(usmDbUnitSlotPortGet(intIfNum, &unit, &slot, &port) == L7_SUCCESS)
      {
        cliLLDPMedShowInterfaceWrite(ewsContext, intIfNum, unit, slot, port);
        count++;
      }
      rc = usmDbGetNextPhysicalIntIfNumber(intIfNum, &intIfNum);
      if (rc != L7_SUCCESS)
      {
        ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf );
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_TlvCodes0Capa1NetPolicy );
        ewsTelnetWriteAddBlanks (1, 0, 11, 0, L7_NULLPTR, ewsContext, pStrInfo_base_2Location3ExtendedPSE );
        ewsTelnetWriteAddBlanks (1, 0, 11, 0, L7_NULLPTR, ewsContext, pStrInfo_base_4EnxtendedPd5Inventory );
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        intIfNum = 0;
        rc = L7_SUCCESS;
        return cliSyntaxReturnPrompt (ewsContext, "");
      }
    }
  }
  else
  {
    if(cliLLDPIntIfNumUSPGet(ewsContext, argv[index+1], 
                             &intIfNum, &unit, &slot, &port) != L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
      return cliSyntaxReturnPrompt (ewsContext,"%u/%u/%u", unit, slot, port);
    }

    if(usmDbLldpIntfRxModeGet(intIfNum, &mode) != L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks(1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
      ewsTelnetPrintf(ewsContext, cliDisplayInterfaceHelp(unit, slot, port));
      return cliPrompt(ewsContext);
    }

    ewsTelnetWriteAddBlanks(1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_DsplyLLDPMedShowIntfDetail);
    ewsTelnetWrite( ewsContext, "\r\n---------  ------  --------- --------  ------------ -----------" );
    cliLLDPMedShowInterfaceWrite(ewsContext, intIfNum, unit, slot, port);
  }

  ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf );
  ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_TlvCodes0Capa1NetPolicy );
  ewsTelnetWriteAddBlanks (1, 0, 11, 0, L7_NULLPTR, ewsContext, pStrInfo_base_2Location3ExtendedPSE );
  ewsTelnetWriteAddBlanks (1, 0, 11, 0, L7_NULLPTR, ewsContext, pStrInfo_base_4EnxtendedPd5Inventory );
  if (strcmp(argv[index+1], pStrInfo_common_All) == 0)
  {
    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    cliSyntaxBottom(ewsContext);

    osapiSnprintf(cmdBuf, sizeof(cmdBuf), argv[0]);
    for (i=1; i<argc; i++)
    {
      OSAPI_STRNCAT(cmdBuf, " ");
      OSAPI_STRNCAT(cmdBuf, argv[i]);
    }
    cliAlternateCommandSet(cmdBuf);
    return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
  }
  else
  {
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
    ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf );
    return cliPrompt(ewsContext);
  }
}

/*********************************************************************
*
* @purpose  display one output line for show remote-device detailed
*
* @param    EwsContext ewsContext   @b((input))  CLI context
* @param    L7_uint32  intIfNum     @b((input))  internal interface number
* @param    L7_uint32  unit         @b((input))  unit number
* @param    L7_uint32  slot         @b((input))  slot number
* @param    L7_uint32  port         @b((input))  port number
*
* @return   void
*
* @notes    None
*
* @end
*********************************************************************/
static void cliLLDPMedShowLocalDeviceDetailedPolicyAppTypeWrite(EwsContext ewsContext,
                                                                L7_uint32 intIfNum, 
                                                                L7_uint32 policyAppType)
{
  L7_uint32  val=0;
  L7_BOOL    flag=L7_FALSE;

  /* Media Policy Application Type */
  /* Check for the vlan to see if this policy app type exists */
  if(usmDbLldpXMedLocMediaPolicyVlanIDGet(intIfNum, policyAppType, &val) == L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks(1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_MediaPolicyApplicationType);
    ewsTelnetWrite(ewsContext, usmDbLldpXMedPolicyAppTypesString(policyAppType));
    ewsTelnetWriteAddBlanks(1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_VlanIdValue);
    ewsTelnetPrintf(ewsContext, "%d", val);
  }
  else
  {
    return;
  }
  if(lldpXMedLocMediaPolicyPriorityGet(intIfNum, policyAppType, &val) == L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks(1,0,1,1, L7_NULLPTR, ewsContext, pStrInfo_base_PriorityValue);
    ewsTelnetPrintf(ewsContext, "%d", val);
  }
  if(lldpXMedLocMediaPolicyDscpGet(intIfNum, policyAppType, &val) == L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks(1,0,1,1, L7_NULLPTR, ewsContext, pStrInfo_base_DSCPValue);
    ewsTelnetPrintf(ewsContext, "%d", val);
  }
  if(lldpXMedLocMediaPolicyUnknownGet(intIfNum, policyAppType, &flag) == L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks(1,0,1,1, L7_NULLPTR, ewsContext, pStrInfo_base_UnknownValue );
    ewsTelnetPrintf(ewsContext, "%s", strUtilTrueFalseGet(flag, pStrInfo_common_False2));
  }
  if(lldpXMedLocMediaPolicyTaggedGet(intIfNum, policyAppType, &flag) == L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks(1,0,1,1, L7_NULLPTR, ewsContext, pStrInfo_base_TaggedValue);
    ewsTelnetPrintf(ewsContext, "%s", strUtilTrueFalseGet(flag, pStrInfo_common_False2));   
  }

  ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
  return;
}

/*********************************************************************
*
* @purpose  Display LLDP MED Detailed Local Device Information
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8 *
* @returns    cliPrompt(ewsContext)
*
* @notes    None
*
* @cmdsyntax  show lldp med local-device detail [<unit>/]<slot>/<port>
*
* @cmdhelp    Display LLDP state for interface(s)
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandLLDPMedShowLocalDeviceDetail(EwsContext ewsContext,
                                                    L7_uint32 argc,
                                                    const L7_char8 **argv,
                                                    L7_uint32 index)
{
  L7_char8    buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8    stat[L7_CLI_MAX_STRING_LENGTH];
  lldpXMedCapabilities_t medCap;
  L7_uint32   intIfNum  = 0;
  L7_uint32   unit      = 0;
  L7_int32    slot      = 0;
  L7_int32    port      = 0;
  L7_uint32   mode      = L7_DISABLE;
  L7_uint32   numArg    = cliNumFunctionArgsGet();
  L7_uint32   val       = 0;
  L7_uchar8   loc[262];  /* Greater than the larget Location data length (256 + 1) */

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  if(numArg != 1)
  {
    osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, sizeof(buf), pStrErr_base_ShowLldpMedIntf, cliSyntaxInterfaceHelp());
    return cliSyntaxReturnPrompt(ewsContext, buf);
  }

  if(cliLLDPIntIfNumUSPGet(ewsContext, argv[index+1], 
                           &intIfNum, &unit, &slot, &port) != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
    return cliSyntaxReturnPrompt (ewsContext,"%u/%u/%u", unit, slot, port);
  }

  if(usmDbLldpIntfTxModeGet(intIfNum, &mode) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  if(mode != L7_ENABLE)
  {
    ewsTelnetWriteAddBlanks (2, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_TheinterfaceisnotenabledtotransmitLLDPdata);
  }
  else
  {
    ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LLDPMEDLocalDeviceDetail);
    ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_Intf_3);
    ewsTelnetPrintf(ewsContext, "%s", cliDisplayInterfaceHelp(unit, slot, port));

    memset(medCap.bitmap, 0, sizeof(medCap.bitmap));
    if(usmDbLldpXMedPortConfigTLVsEnabledGet(intIfNum, &medCap) == L7_SUCCESS)
    {
      if(medCap.bitmap[1] & LLDP_MED_CAP_NETWORKPOLICY_BITMASK)
      {
        ewsTelnetWriteAddBlanks (2, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_NetworkPolicy);
        cliLLDPMedShowLocalDeviceDetailedPolicyAppTypeWrite(ewsContext, intIfNum, voice_policyapptype);
        cliLLDPMedShowLocalDeviceDetailedPolicyAppTypeWrite(ewsContext, intIfNum, voiceSignaling_policyapptype);
        cliLLDPMedShowLocalDeviceDetailedPolicyAppTypeWrite(ewsContext, intIfNum, guestVoice_policyapptype);
        cliLLDPMedShowLocalDeviceDetailedPolicyAppTypeWrite(ewsContext, intIfNum, guestVoiceSignaling_policyapptype);
        cliLLDPMedShowLocalDeviceDetailedPolicyAppTypeWrite(ewsContext, intIfNum, softPhoneVoice_policyapptype);
        cliLLDPMedShowLocalDeviceDetailedPolicyAppTypeWrite(ewsContext, intIfNum, videoConferencing_policyapptype);
        cliLLDPMedShowLocalDeviceDetailedPolicyAppTypeWrite(ewsContext, intIfNum, streamingVideo_policyapptype);
        cliLLDPMedShowLocalDeviceDetailedPolicyAppTypeWrite(ewsContext, intIfNum, videoSignaling_policyapptype);
      }

      if(medCap.bitmap[1] & LLDP_MED_CAP_INVENTORY_BITMASK)
      {
        if(usmDbLldpXMedLocHardwareRevGet(stat) == L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_Inventory);
          ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_HardwareRev);
          ewsTelnetPrintf(ewsContext, "%s", stat);
        }
        if(usmDbLldpXMedLocFirmwareRevGet(stat) == L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_FirmwareRev);
          ewsTelnetPrintf(ewsContext, "%s", stat);
        }
        if(usmDbLldpXMedLocSoftwareRevGet(stat) == L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_SoftwareRev);
          ewsTelnetPrintf(ewsContext, "%s", stat);
        }
        if(usmDbLldpXMedLocSerialNumGet(stat) == L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_SerialNum);
          ewsTelnetPrintf(ewsContext, "%s", stat);
        }
        if(usmDbLldpXMedLocMfgNameGet(stat) == L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_MfgName);
          ewsTelnetPrintf(ewsContext, "%s", stat);
        }
        if(usmDbLldpXMedLocModelNameGet(stat) == L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_ModelName);
          ewsTelnetPrintf(ewsContext, "%s", stat);
        }
        if(usmDbLldpXMedLocAssetIDGet(stat) == L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_AssetID);
          ewsTelnetPrintf(ewsContext, "%s", stat);
        }
      }

      if(medCap.bitmap[1] & LLDP_MED_CAP_LOCATION_BITMASK)
      {
        lldpXMedLocationSubtype_t subType =   unknown_locsubtype;
        while(subType < elin_locsubtype)
        {
      memset(loc, 0x00, sizeof(loc));
      subType = subType+1;
          if((usmDbLldpXMedLocLocationInfoGet(intIfNum, subType, loc) == L7_SUCCESS) && 
             (strcmp(stat, " ")!= 0))
          {
            ewsTelnetWriteAddBlanks (2, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_Location);          
            ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_Subtype);          
            ewsTelnetPrintf(ewsContext, "%s", usmDbLldpXMedLocationSubtypeString(subType));
            ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_Info);          
            ewsTelnetPrintf(ewsContext, "%s", loc);
          }
        }
      }

      if(((medCap.bitmap[1] & LLDP_MED_CAP_EXT_PSE_BITMASK) 
              || (medCap.bitmap[1] & LLDP_MED_CAP_EXT_PD_BITMASK))
                   && (usmDbLldpPoeIsValidIntf(intIfNum) == L7_SUCCESS))
      {
        if(usmDbLldpXMedLocXPoeDeviceTypeGet(intIfNum,&val) == L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (2, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_ExtendedPOE);          
          ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_DeviceType);
          ewsTelnetPrintf(ewsContext, "%s", usmDbLldpXMedPoeDeviceTypeString(val));
        }
        if((medCap.bitmap[1] & LLDP_MED_CAP_EXT_PSE_BITMASK)&&(usmDbLldpPoeIsValidPSEIntf(intIfNum)==L7_SUCCESS))
        {
          if(usmDbLldpXMedLocXPoePSEPortPowerAvGet(intIfNum, &val) == L7_SUCCESS)
          {
            ewsTelnetWriteAddBlanks (2, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_ExtendedPOEPSE);
            ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_Available);
            ewsTelnetPrintf(ewsContext, " %.2f %s", (float) val/10, pStrInfo_base_Watts);
          }

          if(usmDbLldpXMedLocXPoePSEPowerSourceGet(&val) == L7_SUCCESS)
          {
            ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_Source);
            ewsTelnetPrintf(ewsContext, "%s", usmDbLldpXMedPoePsePowerSourceString(val));
          }
          if(usmDbLldpXMedLocXPoePSEPortPDPriorityGet(intIfNum, &val) == L7_SUCCESS)
          {
            ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_Priority);
            ewsTelnetPrintf(ewsContext, "%s", usmDbLldpXMedPoePowerPriorityString(val));
          }
        }
        if((medCap.bitmap[1] & LLDP_MED_CAP_EXT_PD_BITMASK)&&(usmDbLldpPoeIsValidPDIntf(intIfNum)==L7_SUCCESS))
        {
          if(usmDbLldpXMedLocXPoePDPowerReqGet(intIfNum, &val) == L7_SUCCESS)
          {
            ewsTelnetWriteAddBlanks (2, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_ExtendedPOEPD);
            ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_Required);
            ewsTelnetPrintf(ewsContext, " %.2f %s", (float) val/10, pStrInfo_base_Watts);
          }
          if(usmDbLldpXMedLocXPoePDPowerSouceGet(intIfNum, &val) == L7_SUCCESS)
          {
            ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_Source);
            ewsTelnetPrintf(ewsContext, " %s", usmDbLldpXMedPoePdPowerSourceString(val));
          }
          if(usmDbLldpXMedLocXPoePDPowerPriorityGet(intIfNum, &val) == L7_SUCCESS)
          {
            ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_Priority);
            ewsTelnetPrintf(ewsContext, "%s", usmDbLldpXMedPoePowerPriorityString(val));
          }
        }
      }
    }
  }

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf );
  return cliPrompt(ewsContext);
}


/*********************************************************************
*
* @purpose  display one output line for show remote-device
*
* @param    EwsContext ewsContext   @b((input))  CLI context
* @param    L7_uint32  intIfNum     @b((input))  internal interface number
* @param    L7_uint32  unit         @b((input))  unit number
* @param    L7_uint32  slot         @b((input))  slot number
* @param    L7_uint32  port         @b((input))  port number
*
* @return   void
*
* @notes    None
*
* @end
*********************************************************************/
static void cliLLDPMedShowRemoteDeviceWrite(EwsContext ewsContext,
                                            L7_uint32 intIfNum,
                                            L7_uint32 unit,
                                            L7_uint32 slot,
                                            L7_uint32 port,
                                            L7_uint32 *remIndex,
                                            L7_uint32 *timestamp,
                                            L7_uint32 *countlines)
{
  L7_uint32   deviceClass=0;

  /* Iterate through all the remote nodes of the interface */
  while(usmDbLldpRemEntryGetNext(intIfNum,remIndex,timestamp) == L7_SUCCESS)
  {
    *countlines = *countlines +1;
    /* Local Interface */
    ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
    ewsTelnetPrintf(ewsContext, "%-11s", cliDisplayInterfaceHelp(unit, slot, port));

    /* Remote Id */
    ewsTelnetPrintf (ewsContext, "%-12d", *remIndex);

    /* Device Class */
    if(usmDbLldpXMedRemDeviceClassGet(intIfNum, *remIndex, *timestamp, &deviceClass) == L7_SUCCESS)
    {
      ewsTelnetPrintf(ewsContext, "%s", usmDbLldpXMedDeviceClassString(deviceClass));
    }
    if(*countlines == CLI_MAX_SCROLL_LINES-6)
    {
      break;
    }
  }

  return;
}

/*********************************************************************
*
* @purpose  Display LLDP remote device Information
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8 *
* @returns    cliPrompt(ewsContext)
*
* @notes    None
*
* @cmdsyntax  show lldp med remote-device {<unit/slot/port> | all} 
*
* @cmdhelp    Display LLDP summary state for interface(s)
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandLLDPMedShowRemoteDevice(EwsContext ewsContext,
                                               L7_uint32 argc,
                                               const L7_char8 **argv,
                                               L7_uint32 index)
{
  L7_char8    buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32   unit      = 0;
  L7_uint32   slot      = 0;
  L7_uint32   port      = 0;
  static L7_uint32   intIfNum  = 0;
  L7_uint32   numArg    = cliNumFunctionArgsGet();
  L7_uint32   mode      = L7_DISABLE;
  L7_BOOL     enabled=L7_FALSE;
  static L7_uint32   remIndex=0,timestamp=0;
  L7_uint32 countlines=0;
  L7_BOOL  allInterfaces = L7_FALSE;

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  cliCmdScrollSet(L7_FALSE);
  if (numArg != 1)
  {
    osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, sizeof(buf), pStrErr_base_ShowLLDPMEDRemoteDevice, cliSyntaxInterfaceHelp());
    return cliSyntaxReturnPrompt (ewsContext, buf);
  }

  if (strcmp(argv[index+1], pStrInfo_common_All) == 0)
  {
    allInterfaces = L7_TRUE;
  }
  else
  {
    if (cliLLDPIntIfNumUSPGet(ewsContext, argv[index+1],
                              &intIfNum, &unit, &slot, &port) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }

  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {
    if (L7_TRUE == cliIsPromptRespQuit())
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
    if (allInterfaces == L7_TRUE)
    {
      intIfNum--;
    }
  }
  else
  {
    if (allInterfaces == L7_TRUE)
    {
      intIfNum=0;
    }
    remIndex = 0;
    timestamp = 0;
    countlines = 0;
  } 

  /* iterate through all physical interfaces valid for LLDP MED */
  ewsTelnetWriteAddBlanks(2, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_ShowLLDPMEDRemoteDeviceSumm);
  ewsTelnetWriteAddBlanks(1, 0, 0, 26, L7_NULLPTR, ewsContext, pStrInfo_base_ShowLLDPMEDRemoteDeviceSummLocal);
  ewsTelnetWriteAddBlanks(1, 0, 0, 8, L7_NULLPTR, ewsContext, pStrInfo_base_ShowLLDPMEDRemoteDeviceSummHeading);
  ewsTelnetWrite(ewsContext, "\r\n---------  ----------  --------------------");

  while (1)
  {
    if (allInterfaces == L7_TRUE)
    {
      if (usmDbGetNextPhysicalIntIfNumber(intIfNum, &intIfNum) != L7_SUCCESS)
        break;
    }
    if (usmDbLldpXMedPortAdminModeGet(intIfNum, &mode) != L7_SUCCESS || mode != L7_ENABLE)
    {
      if (allInterfaces == L7_TRUE)
      {
        continue;
      }
      else
      {
        break;
      }
    }
    if (usmDbUnitSlotPortGet(intIfNum, &unit, &slot, &port) == L7_SUCCESS)
    {
      enabled = L7_TRUE;
      cliLLDPMedShowRemoteDeviceWrite(ewsContext, intIfNum, unit, slot, port,&remIndex,&timestamp,&countlines);
      if (countlines == CLI_MAX_SCROLL_LINES-6)
      {
        break;
      }
    }
    if (allInterfaces == L7_FALSE)
    {
      break;
    }
  }

  if (enabled == L7_FALSE)
  {
    ewsTelnetWriteAddBlanks (2, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_NolocalinterfacesenabledreceiveLLDPdata);
  }

  if (countlines == CLI_MAX_SCROLL_LINES-6)
  {
    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    memset(buf,0,sizeof(buf));
    osapiSnprintf( buf, sizeof(buf),pStrInfo_base_ShowLldpMedRemoteDevice,argv[index+1]);
    cliAlternateCommandSet(buf);
    return pStrInfo_common_Name_2;    /* --More-- or (q)uit */
  }
  ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliSyntaxReturnPrompt (ewsContext, "");

}

/*********************************************************************
*
* @purpose  display one output line for show remote-device detailed
*
* @param    EwsContext ewsContext   @b((input))  CLI context
* @param    L7_uint32  intIfNum     @b((input))  internal interface number
* @param    L7_uint32  unit         @b((input))  unit number
* @param    L7_uint32  slot         @b((input))  slot number
* @param    L7_uint32  port         @b((input))  port number
*
* @return   void
*
* @notes    None
*
* @end
*********************************************************************/
static void cliLLDPMedShowRemoteDeviceDetailedPolicyAppTypeWrite(EwsContext ewsContext,
                                                                 L7_uint32 intIfNum, 
                                                                 L7_uint32 remIndex, 
                                                                 L7_uint32 timestamp, 
                                                                 L7_uint32 policyAppType)
{
  L7_uint32  val=0;
  L7_BOOL    flag=L7_FALSE;

  /* Media Policy Application Type */
  /* Check for the vlan to see if this policy app type exists */
  if(usmDbLldpXMedRemMediaPolicyVlanIdGet(intIfNum, remIndex, timestamp, policyAppType, &val) == L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks(1,0,1,1, L7_NULLPTR, ewsContext, pStrInfo_base_MediaPolicyApplicationType);
    ewsTelnetPrintf(ewsContext, "%s", usmDbLldpXMedPolicyAppTypesString(policyAppType));
    ewsTelnetWriteAddBlanks(1,0,1,1, L7_NULLPTR, ewsContext, pStrInfo_base_VlanIdValue);
    ewsTelnetPrintf(ewsContext, "%d", val);
  }
  else
  {
    return;
  }

  if(lldpXMedRemMediaPolicyPriorityGet(intIfNum, remIndex, timestamp, policyAppType, &val) == L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks(1,0,1,1, L7_NULLPTR, ewsContext, pStrInfo_base_PriorityValue);
    ewsTelnetPrintf(ewsContext, "%d", val);
  }
  if(lldpXMedRemMediaPolicyDscpGet(intIfNum, remIndex, timestamp, policyAppType, &val) == L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks(1,0,1,1, L7_NULLPTR, ewsContext, pStrInfo_base_DSCPValue);
    ewsTelnetPrintf(ewsContext, "%d", val);
  }

  if(lldpXMedRemMediaPolicyUnknownGet(intIfNum, remIndex, timestamp, policyAppType, &flag) == L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks(1,0,1,1, L7_NULLPTR, ewsContext, pStrInfo_base_UnknownValue );
    ewsTelnetPrintf(ewsContext, "%s", strUtilTrueFalseGet(flag, pStrInfo_common_False2));
  }

  if(lldpXMedRemMediaPolicyTaggedGet(intIfNum, remIndex, timestamp, policyAppType, &flag) == L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks(1,0,1,1, L7_NULLPTR, ewsContext, pStrInfo_base_TaggedValue);
    ewsTelnetPrintf(ewsContext, "%s", strUtilTrueFalseGet(flag, pStrInfo_common_False2));   
  }
  ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf );
  return;
}

/*********************************************************************
* @purpose  Display LLDP remote device Information
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8 *
* @returns    cliPrompt(ewsContext)
*
* @notes    None
*
* @cmdsyntax  show lldp med remote-device detail {<unit/slot/port>} 
*
* @cmdhelp    Display LLDP remote device data received on an interface.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandLLDPMedShowRemoteDeviceDetail(EwsContext ewsContext, L7_uint32 argc,
                                                     const L7_char8 **argv, L7_uint32 index)
{

  L7_char8    buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32   intIfNum  = 0;
  static L7_uint32   remIndex  = 0;
  static L7_uint32   timestamp = 0;
  L7_uint32   unit      = 0;
  L7_int32    slot      = 0;
  L7_int32    port      = 0;
  L7_uint32   mode      = L7_DISABLE;
  L7_uint32   numArg = cliNumFunctionArgsGet();
  lldpXMedCapabilities_t medCap;
  L7_uint32   val=0;
  L7_uint32   deviceClass=0;
  L7_uchar8   loc[262];  /* Greater than the larget Location data length (256 + 1) */
  lldpXMedLocationSubtype_t subType =   unknown_locsubtype;
  L7_BOOL flag = L7_FALSE;

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  /* validate arguments */

  if(numArg != 1)
  {
    osapiSnprintfAddBlanks(2, 1, 0, 0, pStrErr_common_IncorrectInput, buf, sizeof(buf), pStrErr_base_ShowLldpIntf_1, cliSyntaxInterfaceHelp());
    return cliSyntaxReturnPrompt (ewsContext, buf);
  }

  cliCmdScrollSet(L7_FALSE);

  if(cliLLDPIntIfNumUSPGet(ewsContext, argv[index+1], 
                           &intIfNum, &unit, &slot, &port) != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (2, 1, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
    return cliSyntaxReturnPrompt (ewsContext,"%s", cliDisplayInterfaceHelp(unit, slot, port));
  }


  if(usmDbLldpXMedPortAdminModeGet(intIfNum, &mode) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks(2, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_NolocalinterfacesenabledreceiveLLDPdata);
  }

  if(mode != L7_ENABLE)
  {
    return cliSyntaxReturnPromptAddBlanks(2, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrInfo_base_IntfIsNotEnbldToReceiveLldpMedData);
  }
  else
  {
    if (cliGetCharInputID() != CLI_INPUT_EMPTY)
    {                                                                    
      if (L7_TRUE == cliIsPromptRespQuit())
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliPrompt(ewsContext);
      }
    }
    else
    {
        remIndex = 0;
        timestamp = 0;
        flag = L7_TRUE;
        memset (buf, 0, sizeof(buf));
        ewsTelnetWriteAddBlanks(2, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LLDPMEDRemoteDeviceDetail);
        osapiSnprintfAddBlanks (2, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_LocalIntf, cliDisplayInterfaceHelp(unit, slot, port));
        ewsTelnetWrite(ewsContext, buf);
     }

    if(usmDbLldpRemEntryGetNext(intIfNum,&remIndex,&timestamp) == L7_SUCCESS)
    {
      flag = L7_FALSE;
      /* Remote Id */
      ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
      ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
      ewsTelnetWriteAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_RemoteId);
      ewsTelnetPrintf (ewsContext, "%d", remIndex);
      ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);

      ewsTelnetWriteAddBlanks(2, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_capabilities_1);
      ewsTelnetWriteAddBlanks(1, 0, 1, 1, L7_NULLPTR, ewsContext,  pStrInfo_base_MEDCapabilitiesSupported);
      if(usmDbLldpXMedRemCapSupportedGet(intIfNum, remIndex, timestamp, &medCap) == L7_SUCCESS)
      {
         ewsTelnetPrintf(ewsContext, "%s", strUtilLLdpMedCapabilityGet(medCap, buf));
      }
  
      memset(medCap.bitmap, 0, sizeof(medCap.bitmap));
      ewsTelnetWriteAddBlanks(1, 0, 1, 1, L7_NULLPTR, ewsContext,  pStrInfo_base_MEDCapabilitiesEnabled);
      if(usmDbLldpXMedRemCapCurrentGet(intIfNum, remIndex, timestamp, &medCap) == L7_SUCCESS)
      {
        ewsTelnetPrintf(ewsContext, "%s", strUtilLLdpMedCapabilityGet(medCap, buf));
      }
  
      /* Device Class */
      if(usmDbLldpXMedRemDeviceClassGet(intIfNum, remIndex, timestamp, &deviceClass) == L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_DeviceClass_1);
        ewsTelnetPrintf(ewsContext, "%s", usmDbLldpXMedDeviceClassString(deviceClass));
      }
  
      ewsTelnetWriteAddBlanks(2, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_NetworkPolicy);
      cliLLDPMedShowRemoteDeviceDetailedPolicyAppTypeWrite(ewsContext, intIfNum, remIndex, timestamp, voice_policyapptype);
      cliLLDPMedShowRemoteDeviceDetailedPolicyAppTypeWrite(ewsContext, intIfNum, remIndex, timestamp, voiceSignaling_policyapptype);
      cliLLDPMedShowRemoteDeviceDetailedPolicyAppTypeWrite(ewsContext, intIfNum, remIndex, timestamp, guestVoice_policyapptype);
      cliLLDPMedShowRemoteDeviceDetailedPolicyAppTypeWrite(ewsContext, intIfNum, remIndex, timestamp, guestVoiceSignaling_policyapptype);
      cliLLDPMedShowRemoteDeviceDetailedPolicyAppTypeWrite(ewsContext, intIfNum, remIndex, timestamp, softPhoneVoice_policyapptype);
      cliLLDPMedShowRemoteDeviceDetailedPolicyAppTypeWrite(ewsContext, intIfNum, remIndex, timestamp, videoConferencing_policyapptype);
      cliLLDPMedShowRemoteDeviceDetailedPolicyAppTypeWrite(ewsContext, intIfNum, remIndex, timestamp, streamingVideo_policyapptype);
      cliLLDPMedShowRemoteDeviceDetailedPolicyAppTypeWrite(ewsContext, intIfNum, remIndex, timestamp, videoSignaling_policyapptype);
  
      if(usmDbLldpXMedRemHardwareRevGet(intIfNum, remIndex, timestamp, buf) == L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks(1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_Inventory);
        ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_HardwareRev);
        ewsTelnetPrintf(ewsContext, "%s", buf);
      }
      if(usmDbLldpXMedRemFirmwareRevGet(intIfNum, remIndex, timestamp, buf) == L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_FirmwareRev);
        ewsTelnetPrintf(ewsContext, "%s", buf);
      }
      if(usmDbLldpXMedRemSoftwareRevGet(intIfNum, remIndex, timestamp, buf) == L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_SoftwareRev);
        ewsTelnetPrintf(ewsContext, "%s", buf);
      }
      if(usmDbLldpXMedRemSerialNumGet(intIfNum, remIndex, timestamp, buf) == L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_SerialNum);
        ewsTelnetPrintf(ewsContext, "%s", buf);
      }
      if(usmDbLldpXMedRemMfgNameGet(intIfNum, remIndex, timestamp, buf) == L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_MfgName);
        ewsTelnetPrintf(ewsContext, "%s", buf);
      }
      if(usmDbLldpXMedRemModelNameGet(intIfNum, remIndex, timestamp, buf) == L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_ModelName);
        ewsTelnetPrintf(ewsContext, "%s", buf);
      }
      if(usmDbLldpXMedRemAssetIdGet(intIfNum, remIndex, timestamp, buf) == L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_AssetID);
        ewsTelnetPrintf(ewsContext, "%s", buf);
      }
  
      
      while(subType < elin_locsubtype)
      {
      memset(loc, 0x00, sizeof(loc));
      subType = subType+1;
        if((usmDbLldpXMedRemLocationInfoGet(intIfNum, remIndex, timestamp, subType, loc) == L7_SUCCESS) && 
           (strcmp(buf, " ")!= 0))
        {
          ewsTelnetWriteAddBlanks (2, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_base_Location);          
          ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_Subtype);          
          ewsTelnetPrintf(ewsContext, "%s", usmDbLldpXMedLocationSubtypeString(subType));
          ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_Info);          
          ewsTelnetPrintf(ewsContext, "%s", loc);
        }
      }
  
      if(usmDbLldpXMedRemXPoeDeviceTypeGet(intIfNum, remIndex, timestamp, &val) == L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (2, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_ExtendedPOE);          
        ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_DeviceType);
        ewsTelnetPrintf(ewsContext, "%s", usmDbLldpXMedPoeDeviceTypeString(val));
      }
  
      if(usmDbLldpXMedRemXPoePSEPowerAvGet(intIfNum, remIndex, timestamp, &val) == L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (2, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_ExtendedPOEPSE);
        ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_Available);
        ewsTelnetPrintf(ewsContext, " %.2f %s", (float) val/10, pStrInfo_base_Watts);
      }
  
      if(usmDbLldpXMedRemXPoePSEPowerSrcGet(intIfNum, remIndex, timestamp, &val) == L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_Source);
        ewsTelnetPrintf(ewsContext, "%s", usmDbLldpXMedPoePsePowerSourceString(val));
      }
  
      if(usmDbLldpXMedRemXPoePSEPowerPriGet(intIfNum, remIndex, timestamp, &val) == L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_Priority);
        ewsTelnetPrintf(ewsContext, "%s", usmDbLldpXMedPoePowerPriorityString(val));
      }
  
      if(usmDbLldpXMedRemXPoePDPowerReqGet(intIfNum, remIndex, timestamp, &val) == L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (2, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_ExtendedPOEPD);
        ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_Required);
        ewsTelnetPrintf(ewsContext, " %.2f %s", (float) val/10, pStrInfo_base_Watts);
      }
      if(usmDbLldpXMedRemXPoePDPowerSrcGet(intIfNum, remIndex, timestamp, &val) == L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_Source);
        ewsTelnetPrintf(ewsContext, " %s", usmDbLldpXMedPoePdPowerSourceString(val));
      }
      if(usmDbLldpXMedRemXPoePDPowerPriGet(intIfNum, remIndex, timestamp, &val) == L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 1, 1, L7_NULLPTR, ewsContext, pStrInfo_base_Priority);
        ewsTelnetPrintf(ewsContext, "%s", usmDbLldpXMedPoePowerPriorityString(val));
      }
      ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
    }
    else
    {
      if(flag == L7_TRUE)
      {
        return cliSyntaxReturnPromptAddBlanks(2, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_RemoteClientEntriesarenotpresentonthisinterface);
      }
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    memset(buf,0,sizeof(buf));
    osapiSnprintf( buf, sizeof(buf),"show lldp med remote-device detail %s",argv[index+1]);
    cliAlternateCommandSet(buf);
    return pStrInfo_common_Name_2;    /* --More-- or (q)uit */
  
  }
}
