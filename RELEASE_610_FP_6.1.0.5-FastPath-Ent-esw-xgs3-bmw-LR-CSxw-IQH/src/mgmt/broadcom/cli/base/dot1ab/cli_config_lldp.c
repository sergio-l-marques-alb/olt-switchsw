/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
 * @filename src/mgmt/cli/base/cli_config_lldp.c
*
* @purpose LLDP config commands for CLI
*
* @component user interface
*
* @comments none
*
* @create  18/02/2005
*
* @author  Rama Krishna Hazari
* @end
*
**********************************************************************/
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "l7_common.h"
#include "usmdb_util_api.h"
#include "cliapi.h"
#include "datatypes.h"
#include "cli_macro.h"
#include "ews.h"
#include <stdio.h>

#include "clicommands_lldp.h"
#include "usmdb_lldp_api.h"
#include "clicommands_card.h"
#include "defaultconfig.h"
#include "lldp_exports.h"

/*********************************************************************
*
* @purpose  To enable the LLDP advertise capability
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  [no] lldp transmit
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8  *commandLLDPTransmit(EwsContext ewsContext,
                                    L7_uint32 argc,
                                    const L7_char8 * * argv,
                                    L7_uint32 index)
{
  L7_uint32 numArg = 0;        /* New variable Added */
  L7_uint32 mode = 0;
  L7_uint32 s = 0, p = 0;
  L7_uint32 interface = 0;
  L7_RC_t   rc = L7_FAILURE;
  L7_uint32 unit = 0;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  /* Error Checking for Number of Arguments.
     No Parameters are passed so checking it to zero
   */
  if (numArg != 0)
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_LLDP_TRANSMIT);
  }

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
  }

  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit, s, p, &interface) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    rc = usmDbLldpIntfTxModeSet(interface, mode);
    if (rc == L7_NOT_SUPPORTED)
    {
      /* port selected cannot perform this function */
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
    else if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_LLDP_TRANSMIT);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To enable the LLDP receive capability
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  [no] lldp receive
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandLLDPReceive(EwsContext ewsContext,
                                  L7_uint32 argc,
                                   const L7_char8 * * argv,
                                  L7_uint32 index)
{
  L7_uint32 numArg = 0;        /* New variable Added */
  L7_uint32 mode = 0;
  L7_uint32 s = 0, p = 0;
  L7_uint32 interface = 0;
  L7_RC_t   rc = L7_FAILURE;
  L7_uint32 unit = 0;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  /* Error Checking for Number of Arguments.
     No Parameters are passed so checking it to zero
   */
  if (numArg != 0)
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_LLDP_RECEIVE);
  }

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
  }
  else
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_LLDP_RECEIVE);
  }

  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit, s, p, &interface) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    rc = usmDbLldpIntfRxModeSet(interface, mode);
    if (rc == L7_NOT_SUPPORTED)
    {
      /* port selected cannot perform this function */
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
    else if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_LLDP_RECEIVE);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To include transmission of the local system management
*           address information in the LLDPDUs
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  [no] lldp transmit-mgmt
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandLLDPTransmitMgmt(EwsContext ewsContext,
                                        L7_uint32 argc,
                                        const L7_char8 * * argv,
                                        L7_uint32 index)
{
  L7_uint32 numArg = 0;        /* New variable Added */
  L7_uint32 mode = 0;
  L7_uint32 s = 0, p = 0;
  L7_uint32 interface = 0;
  L7_RC_t   rc = L7_FAILURE;
  L7_uint32 unit = 0;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  /* Error Checking for Number of Arguments.
     No Parameters are passed so checking it to zero
   */
  if (numArg != 0)
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_LLDP_TXMGMT);
  }

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
  }
  else
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_LLDP_TXMGMT);
  }

  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit, s, p, &interface) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    rc = usmDbLldpIntfTxMgmtAddrSet(interface, mode);
    if (rc == L7_NOT_SUPPORTED)
    {
      /* port selected cannot perform this function */
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
    else if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_LLDP_TXMGMT);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To enable remote data change notifications
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  [no] lldp notification
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandLLDPNotification(EwsContext ewsContext,
                                        L7_uint32 argc,
                                        const L7_char8 * * argv,
                                        L7_uint32 index)
{
  L7_uint32 numArg = 0;        /* New variable Added */
  L7_uint32 mode = 0;
  L7_uint32 s = 0, p = 0;
  L7_uint32 interface = 0;
  L7_RC_t   rc = L7_FAILURE;
  L7_uint32 unit = 0;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();
  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  /* Error Checking for Number of Arguments.
     No Parameters are passed so checking it to zero
   */
  if (numArg != 0)
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_LLDP_NOTIFICATION);
  }

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
  }
  else
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_LLDP_NOTIFICATION);
  }

  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit, s, p, &interface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    rc = usmDbLldpIntfNotificationModeSet(interface, mode);
    if (rc == L7_NOT_SUPPORTED)
    {
      /* port selected cannot perform this function */
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
    else if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_LLDP_NOTIFICATION);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To limit how frequently remote data change notifications
*           are sent.
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  lldp notification-interval <interval value>
*             no lldp notification-interval
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandLLDPNotificationInterval(EwsContext ewsContext,
                                                L7_uint32 argc,
                                                const L7_char8 * * argv,
                                                L7_uint32 index)
{
  L7_uint32 numArg = 0;        /* New variable Added */
  L7_uint32 interval = 0;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /* If the command is of type 'normal' the 'if' condition is executed
  otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Error Checking for Number of Arguments.
       No Parameters are passed so checking it to zero
    */
    if (numArg != 1)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_LLDP_NOTIF_INTERVAL);
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index + 1], &interval) !=
        L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_LLDP_NOTIF_INTV_RANGE);
    }
    if (interval < CLILLDP_NOTI_INTERVAL_MIN ||
        interval > CLILLDP_NOTI_INTERVAL_MAX)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_LLDP_NOTIF_INTV_RANGE);
    }

    /* calling of USMDB Function. */
    if (usmDbLldpNotificationIntervalSet(interval) != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /* Error Checking for Number of Arguments.
       No Parameters are passed so checking it to zero
    */
    if (numArg != 0)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_LLDP_NOTIF_INTERVAL);
    }

    /* Assign the notification interval to factory default value ie.,
       5 sec
    */
    if (usmDbLldpNotificationIntervalSet(FD_LLDP_NOTIFICATION_INTERVAL)         != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }
  else
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_LLDP_NOTIF_INTERVAL);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To specify which optional TLVs in the 802.1AB basic
*           management set will be transmitted in the LLDPDUs.
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  [no] lldp transmit-tlv [sys-desc] [sys-name] [sys-cap]
*                                     [port-desc]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandLLDPTransmitTLV(EwsContext ewsContext,
                                      L7_uint32 argc,
                                       const L7_char8 * * argv,
                                      L7_uint32 index)
{
  L7_uint32 numArg = 0;        /* New variable Added */
  L7_uint32 s = 0, p = 0;
  L7_uint32 interface = 0;
  L7_uint32 unit = 0;
  L7_BOOL   sysName, sysDesc, sysCap, portDesc;
  L7_BOOL   name, desc, cap, port;
  L7_uint32 i = 0;

  sysName = sysDesc = sysCap = portDesc = L7_FALSE;
  name = desc = cap = port = L7_FALSE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  /* Error Checking for Number of Arguments.
     No Parameters are passed so checking it to zero
  */
  if (numArg < 0 || numArg > 4)
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_LLDP_TRAN_TLV);
  }

  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit, s, p, &interface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    if (usmDbLldpIntfTxTLVsGet(interface, &port, &name, &desc, &cap) !=
      L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_LldpIntfRange);
    }
  }
  for (i = 1; i <= numArg; i++)
  {
    if (strcmp(argv[index + i], pStrInfo_base_LldpSysname) == 0)
    {
      sysName = L7_TRUE;
      continue;
    }
    else if (strcmp(argv[index + i], pStrInfo_base_LldpSysdesc) == 0)
    {
      sysDesc = L7_TRUE;
      continue;
    }
    else if (strcmp(argv[index + i], pStrInfo_base_LldpSyscap) == 0)
    {
      sysCap = L7_TRUE;
      continue;
    }
    else if (strcmp(argv[index + i], pStrInfo_base_LldpPortDesc) == 0)
    {
      portDesc = L7_TRUE;
    }
  }

  /* Check if the command is normal command */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (sysName != L7_TRUE)
    {
      sysName = name;
    }

    if (sysDesc != L7_TRUE)
    {
      sysDesc = desc;
    }

    if (sysCap != L7_TRUE)
    {
      sysCap = cap;
    }

    if (portDesc != L7_TRUE)
    {
      portDesc = port;
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg == 0)
    {
      sysName = sysDesc = sysCap = portDesc = L7_FALSE;
    }
    else
    {
      if (sysName == L7_TRUE)
      {
        sysName = L7_FALSE;
      }
      else
      {
        sysName = name;
      }

      if (sysDesc == L7_TRUE)
      {
        sysDesc = L7_FALSE;
      }
      else
      {
        sysDesc = desc;
      }

      if (sysCap == L7_TRUE)
      {
        sysCap = L7_FALSE;
      }
      else
      {
        sysCap = cap;
      }

      if (portDesc == L7_TRUE)
      {
        portDesc = L7_FALSE;
      }
      else
      {
        portDesc = port;
      }
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    /* calling of USMDB Function. */
    if (usmDbLldpIntfTxTLVsSet(interface, portDesc, sysName, sysDesc,
        sysCap) != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To set the timing parameters for local data transmission
*           on ports enabled for LLDP.
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  lldp timers [interval transmit-interval]
*                       [hold hold-multiplier] [reinit reinit-delay]
*             no lldp timers [interval] [hold] [reinit]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandLLDPTimers(EwsContext ewsContext,
                                  L7_uint32 argc,
                                  const L7_char8 * * argv,
                                  L7_uint32 index)
{
  L7_uint32 numArg = 0;        /* New variable Added */
  L7_uint32 i = 1;
  L7_uint32 hold = 0, reinit = 0, interval = 0;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if (numArg == 0)
  {
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      /* calling of USMDB Function. */
      if (usmDbLldpTxIntervalSet(FD_LLDP_TX_INTERVAL) != L7_SUCCESS)
      {
        return cliSyntaxReturnPrompt (ewsContext, "");
      }

      /* calling of USMDB Function. */
      if (usmDbLldpTxHoldSet(FD_LLDP_TX_HOLD_MULTIPLIER) != L7_SUCCESS)
      {
        return cliSyntaxReturnPrompt (ewsContext, "");
      }

      /* calling of USMDB Function. */
      if (usmDbLldpTxReinitDelaySet(FD_LLDP_REINIT_DELAY) != L7_SUCCESS)
      {
        return cliSyntaxReturnPrompt (ewsContext, "");
      }
    }
  } /* end if */
  else  /* If more than one params exists */
  {
    /* If the command is of type 'normal' the 'if' condition is executed
    otherwise 'else-if' condition is excuted */
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      /* Error Checking for Number of Arguments.
        No Parameters are passed so checking it to zero
      */
      if (numArg < 0 || numArg > 6)
      {
        return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_LLDP_TIMERS);
      }
      while (i <= numArg)
      {
        if (strcmp(argv[index + i], pStrInfo_common_WsInputIntvl) == 0)
        {
          if (cliConvertTo32BitUnsignedInteger(argv[index + (++i)],
              &interval) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_LldpIntvl);
          }
          if (interval < CLILLDP_TIMER_INTERVAL_MIN ||
              interval > CLILLDP_TIMER_INTERVAL_MAX)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_LldpIntvl);
          }

          /*******Check if the Flag is Set for Execution*************/
          if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
          {
            /* calling of USMDB Function. */
            if (usmDbLldpTxIntervalSet(interval) != L7_SUCCESS)
            {
              return cliSyntaxReturnPrompt (ewsContext, "");
            }
          }
        } /* end interval */
        else if (strcmp(argv[index + (i)], pStrInfo_base_LldpHold_1) == 0)
        {
          if (cliConvertTo32BitUnsignedInteger(argv[index + (++i)], &hold)
              != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_LldpHold);
          }
          if (hold < CLILLDP_TIMER_HOLD_MIN ||
              hold > CLILLDP_TIMER_HOLD_MAX)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_LldpHold);
          }

          /*******Check if the Flag is Set for Execution*************/
          if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
          {
            /* calling of USMDB Function. */
            if (usmDbLldpTxHoldSet(hold) != L7_SUCCESS)
            {
              return cliSyntaxReturnPrompt (ewsContext, "");
            }
          }
        } /* end hold */
        else if (strcmp(argv[index + (i)], pStrInfo_base_LldpReinit_1) == 0)
        {
          if (cliConvertTo32BitUnsignedInteger(argv[index + (++i)],
              &reinit) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_LldpReinit);
          }
          if (reinit < CLILLDP_TIMER_REINIT_MIN ||
              reinit > CLILLDP_TIMER_REINIT_MAX)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_LldpReinit);
          }

          /*******Check if the Flag is Set for Execution*************/
          if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
          {
            /* calling of USMDB Function. */
            if (usmDbLldpTxReinitDelaySet(reinit) != L7_SUCCESS)
            {
              return cliSyntaxReturnPrompt (ewsContext, "");
            }
          }
        } /* end reinit */

        i++;
      }
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      /* Error Checking for Number of Arguments.
        No Parameters are passed so checking it to zero
      */
      if (numArg < 0 || numArg > 3)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_LldpNotimers);
      }
      while (i <= numArg)
      {
        if (strcmp(argv[index + i], pStrInfo_common_WsInputIntvl) == 0)
          {
            /*******Check if the Flag is Set for Execution*************/
            if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
            {
              /* calling of USMDB Function. */
              if (usmDbLldpTxIntervalSet(FD_LLDP_TX_INTERVAL) !=
                  L7_SUCCESS)
              {
              return cliSyntaxReturnPrompt (ewsContext, "");
            }
          }
        }   /* end interval */
        else if (strcmp(argv[index + i], pStrInfo_base_LldpHold_1) == 0)
          {
            /*******Check if the Flag is Set for Execution*************/
            if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
            {
              /* calling of USMDB Function. */
              if (usmDbLldpTxHoldSet(FD_LLDP_TX_HOLD_MULTIPLIER) !=
                  L7_SUCCESS)
              {
              return cliSyntaxReturnPrompt (ewsContext, "");
            }
          }
        }   /* end hold */
        else if (strcmp(argv[index + i], pStrInfo_base_LldpReinit_1) == 0)
          {
            /*******Check if the Flag is Set for Execution*************/
            if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
            {
              /* calling of USMDB Function. */
              if (usmDbLldpTxReinitDelaySet(FD_LLDP_REINIT_DELAY) !=
                  L7_SUCCESS)
              {
              return cliSyntaxReturnPrompt (ewsContext, "");
              }
            }
          } /* reinit end */

          i++;

        } /* while end */
      }
    } /* else end */

    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
    return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To reset all LLDP statistics
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  clear lldp statistics
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandLLDPClearStats(EwsContext ewsContext,
                                      L7_uint32 argc,
                                      const L7_char8 * * argv,
                                      L7_uint32 index)
{
  L7_uint32 numArg = 0;        /* New variable Added */

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /* Error Checking for Number of Arguments.
    No Parameters are passed so checking it to zero
  */
  if (numArg != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_LldpClrStats);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    /* calling of USMDB Function. */
    if (usmDbLldpStatsClear() != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To delete all data from the remote data table
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  clear lldp connections
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandLLDPClearRemoteData(EwsContext ewsContext,
                                           L7_uint32 argc,
                                           const L7_char8 * * argv,
                                           L7_uint32 index)
{
  L7_uint32 numArg = 0;        /* New variable Added */

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /* Error Checking for Number of Arguments.
    No Parameters are passed so checking it to zero
  */
  if (numArg != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_LldpClrRemoteData);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    /* calling of USMDB Function. */
    if (usmDbLldpRemTableClear() != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To enable LLDP MED 
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  [no] lldp med
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8  *commandLLDPMed(EwsContext ewsContext,
                                L7_uint32 argc,
                                const L7_char8 **argv,
                                L7_uint32 index)
{
  
  L7_uint32 mode = 0;
  L7_uint32 s = 0, p = 0;
  L7_uint32 interface = 0;
  L7_uint32 unit = 0;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }
  
  /* If the command is of type 'normal' the 'if' condition is executed
  otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
  }
  else
  {
    return cliSyntaxReturnPrompt(ewsContext, "");
  }
  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);

  if(usmDbIntIfNumFromUSPGet(unit, s, p, &interface) != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks(1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
    return cliSyntaxReturnPrompt(ewsContext,"%u/%u/%u", unit, s, p);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    if (usmDbLldpXMedPortAdminModeSet(interface, mode) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_LLDPMEDModeIntIf);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To enable LLDP MED Config Notification
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  [no] lldp med confignotification
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8  *commandLLDPMedConfigNotification(EwsContext ewsContext,
                                                  L7_uint32 argc,
                                                  const L7_char8 **argv,
                                                  L7_uint32 index)
{
  L7_BOOL mode = L7_FALSE;
  L7_uint32 s = 0, p = 0;
  L7_uint32 interface = 0;
  L7_uint32 unit = 0;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }
  /* If the command is of type 'normal' the 'if' condition is executed
  otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_TRUE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_FALSE;
  }
  else
  {
    return cliSyntaxReturnPrompt(ewsContext, " ");
  }
  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit, s, p, &interface) != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
    return cliSyntaxReturnPrompt (ewsContext,"%u/%u/%u", unit, s, p);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    if(usmDbLldpXMedPortConfigNotifEnableSet(interface, mode) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_LLDPMEDNotifyIntIf);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliSyntaxReturnPrompt(ewsContext, " ");
  
}

/*********************************************************************
*
* @purpose  To configure the LLDP MED Fast Start Repeat Count
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  [no] lldp med faststartrepeatcount <count>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8  *commandLLDPMedFastStartRepeatCount(EwsContext ewsContext,
                                                    L7_uint32 argc,
                                                    const L7_char8 **argv,
                                                    L7_uint32 index)
{
  L7_BOOL mode = L7_FALSE;
  L7_uint32 count = 0;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  /* If the command is of type 'normal' the 'if' condition is executed
  otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
     mode = L7_TRUE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
     mode = L7_FALSE;
  }
  else
  {
    return cliSyntaxReturnPrompt(ewsContext, " ");
  }

  if(mode == L7_TRUE)
  {
    if(cliConvertTo32BitUnsignedInteger(argv[index + 1], &count) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_InvalidValofFastStartRepeatCount);
    }
    if((count < LLDP_MED_FASTSTART_REPEAT_COUNT_MIN) || (count > LLDP_MED_FASTSTART_REPEAT_COUNT_MAX))
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_FastStartRepeatCount);
    }
  }
  else
  {
    count = FD_LLDP_MED_FASTSTART_REPEAT_COUNT;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    if(usmDbLldpXMedFastStartRepeatCountSet(count) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_SetFastStartRepeatCount);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliSyntaxReturnPrompt(ewsContext, " ");
}

/*********************************************************************
*
* @purpose  To enable LLDP MED TLVs
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  [no] med lldp transmit-tlv [capabilities] [network-policy] 
*             [ex-pse] [ex-pd] [location] [inventory]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8  *commandLLDPMedTransmitTlv(EwsContext ewsContext,
                                           L7_uint32 argc,
                                           const L7_char8 **argv,
                                           L7_uint32 index)
{
  L7_uint32 numArg = cliNumFunctionArgsGet();        /* New variable Added */
  L7_BOOL mode = L7_FALSE;
  L7_uint32 s = 0, p = 0;
  L7_uint32 interface = 0;
  L7_uint32 unit = 0;
  lldpXMedCapabilities_t medCap;
  L7_uint32 i=0;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf );
  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  /* If the command is of type 'normal' the 'if' condition is executed
  otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
     mode = L7_TRUE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
     mode = L7_FALSE;
  }
  else
  {
    return cliSyntaxReturnPrompt(ewsContext, " ");
  }
  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);
  if (usmDbIntIfNumFromUSPGet(unit, s, p, &interface) != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
    return cliSyntaxReturnPrompt(ewsContext,"%u/%u/%u", unit, s, p);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    memset(&medCap , 0, sizeof(medCap));
    if (usmDbLldpXMedPortConfigTLVsEnabledGet(interface, &medCap) == L7_SUCCESS)
    {
      if((mode == L7_TRUE) && (numArg == 0))
      {
         memset(&medCap , 0, sizeof(medCap));
         medCap.bitmap[1] |= (LLDP_MED_CAP_CAPABILITIES_BITMASK | LLDP_MED_CAP_NETWORKPOLICY_BITMASK | 
                               LLDP_MED_CAP_EXT_PSE_BITMASK  | LLDP_MED_CAP_EXT_PD_BITMASK |
                              LLDP_MED_CAP_LOCATION_BITMASK | LLDP_MED_CAP_INVENTORY_BITMASK
                              );
      }
      else if((mode == L7_FALSE) && (numArg == 0))
      {
         memset(&medCap , 0, sizeof(medCap));
         medCap.bitmap[1] &= ((~LLDP_MED_CAP_CAPABILITIES_BITMASK )
                              & (~LLDP_MED_CAP_NETWORKPOLICY_BITMASK)
                              & (~LLDP_MED_CAP_EXT_PSE_BITMASK)
                              & (~LLDP_MED_CAP_EXT_PD_BITMASK)
                              & (~LLDP_MED_CAP_LOCATION_BITMASK)
                              & (~LLDP_MED_CAP_INVENTORY_BITMASK));
      }

      i = 1;
      while (i <= numArg)
      {
        if (strcmp(argv[index+i], pStrInfo_base_capabilities)==0)
        {
          if (mode == L7_TRUE)
          {
             medCap.bitmap[1] |= LLDP_MED_CAP_CAPABILITIES_BITMASK;
          }
          else
          {
             medCap.bitmap[1] &= ~LLDP_MED_CAP_CAPABILITIES_BITMASK;
          }
        }
        else if (strcmp(argv[index+i], pStrInfo_base_networkPolicy)==0)
        {
          if (mode == L7_TRUE)
          {
             medCap.bitmap[1] |= LLDP_MED_CAP_NETWORKPOLICY_BITMASK;
          }
          else
          {
             medCap.bitmap[1] &= ~LLDP_MED_CAP_NETWORKPOLICY_BITMASK;
          }
        }
        else if (strcmp(argv[index+i], pStrInfo_base_EXPSE)==0)
        {
          if (mode == L7_TRUE)
          {
             medCap.bitmap[1] |= LLDP_MED_CAP_EXT_PSE_BITMASK;
          }
          else
          {
             medCap.bitmap[1] &= ~LLDP_MED_CAP_EXT_PSE_BITMASK;
          }
        }
        else if (strcmp(argv[index+i], pStrInfo_base_EXPD)==0)
        {
          if (mode == L7_TRUE)
          {
             medCap.bitmap[1] |= LLDP_MED_CAP_EXT_PD_BITMASK;
          }
          else
          {
             medCap.bitmap[1] &= ~LLDP_MED_CAP_EXT_PD_BITMASK;
          }
        }
        else if (strcmp(argv[index+i], pStrInfo_base_location)==0)
        {
          if (mode == L7_TRUE)
          {
             medCap.bitmap[1] |= LLDP_MED_CAP_LOCATION_BITMASK;
          }
          else
          {
             medCap.bitmap[1] &= ~LLDP_MED_CAP_LOCATION_BITMASK;
          }
        }
        else if (strcmp(argv[index+i], pStrInfo_base_inventory)==0)
        {
          if (mode == L7_TRUE)
          {
             medCap.bitmap[1] |= LLDP_MED_CAP_INVENTORY_BITMASK;
          }
          else
          {
             medCap.bitmap[1] &= ~LLDP_MED_CAP_INVENTORY_BITMASK;
          }
        }
        else
        {
           /* do nothing */
        }
        i++;
      }
      
      if (usmDbLldpXMedPortConfigTLVsEnabledSet(interface, &medCap) != L7_SUCCESS)
      {
         return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_TLVsTransmittedLLDPPDU);
      }
    }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_TLVsTransmittedLLDPPDU);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf );
  return cliPrompt(ewsContext);
}


/*********************************************************************
*
* @purpose  To enable LLDP MED on all the ports
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  [no] lldp med all
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8  *commandLLDPMedAll(EwsContext ewsContext, L7_uint32 argc,
                                   const L7_char8 **argv, L7_uint32 index)
{
  
  L7_uint32 mode = L7_DISABLE;
  L7_uint32 intIfNum = 0;
  L7_BOOL countFailure=L7_FALSE;
  L7_RC_t rc=L7_FAILURE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
 
  /* If the command is of type 'normal' the 'if' condition is executed */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }
  
  rc = usmDbLldpValidIntfFirstGet(&intIfNum);
  if(rc != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_UnableToGetFirstIntIfNum);
  }

  while(rc == L7_SUCCESS)
  {
     /*******Check if the Flag is Set for Execution*************/
     if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
     {
       if(usmDbLldpXMedPortAdminModeSet(intIfNum, mode) != L7_SUCCESS)
       {
         countFailure = L7_TRUE;
       }
     }
     rc = usmDbLldpValidIntfNextGet(intIfNum, &intIfNum);
  }

  if(countFailure)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_UnableToSetOneOrMoreIntIfNum);
  }
   
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf );
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To enable LLDP MED Config Notification on all the ports
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  [no] lldp med confignotification all
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8  *commandLLDPMedConfigNotificationAll(EwsContext ewsContext, L7_uint32 argc,
                                                     const L7_char8 **argv, L7_uint32 index)
{
  L7_BOOL mode = L7_FALSE;
  L7_uint32 intIfNum = 0;
  L7_BOOL countFailure = L7_FALSE;
  L7_RC_t rc=L7_FAILURE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  
  /* If the command is of type 'normal' the 'if' condition is executed */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_TRUE;
  }

  rc = usmDbLldpValidIntfFirstGet(&intIfNum);
  if(rc != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_UnableToGetFirstIntIfNum);
  }
  while(rc == L7_SUCCESS)
  {
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      if(usmDbLldpXMedPortConfigNotifEnableSet(intIfNum, mode) != L7_SUCCESS)
      {
        countFailure = L7_TRUE;
      }
    }
    rc = usmDbLldpValidIntfNextGet(intIfNum, &intIfNum);
  }

  if(countFailure)
  {
     return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_LLDPMEDNotifyIntIfAll);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
  return cliSyntaxReturnPrompt(ewsContext, " ");
  
}



/*********************************************************************
*
* @purpose  To enable LLDP MED TLVs on all the ports
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  [no] med lldp transmit-tlv all [capabilities] [network-policy] 
*             [ex-pse] [ex-pd] [location] [inventory]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8  *commandLLDPMedTransmitTlvAll(EwsContext ewsContext, L7_uint32 argc,
                                              const L7_char8 **argv, L7_uint32 index)
{
  L7_BOOL   mode = L7_FALSE, countFailure=L7_FALSE;
  L7_uint32 intIfNum = 0, 
            numArg = cliNumFunctionArgsGet();
  lldpXMedCapabilities_t medCap;
  L7_uint32 i=0;
  L7_RC_t   rc=L7_FAILURE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf );
  
  /* If the command is of type 'normal' the 'if' condition is executed*/
  
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
     mode = L7_TRUE;
  }

  rc = usmDbLldpValidIntfFirstGet(&intIfNum);
  if(rc != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_UnableToGetFirstIntIfNum);
  }
  while(rc == L7_SUCCESS)
  {
     /*******Check if the Flag is Set for Execution*************/
     if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
     {
       memset(&medCap , 0, sizeof(medCap));
       if (usmDbLldpXMedPortConfigTLVsEnabledGet(intIfNum, &medCap) == L7_SUCCESS)
       {
         if((mode == L7_TRUE) && (numArg == 0))
         {
            memset(&medCap , 0, sizeof(medCap));
            medCap.bitmap[1] |= (LLDP_MED_CAP_CAPABILITIES_BITMASK | LLDP_MED_CAP_NETWORKPOLICY_BITMASK | 
                                 LLDP_MED_CAP_EXT_PSE_BITMASK  | LLDP_MED_CAP_EXT_PD_BITMASK | 
                                 LLDP_MED_CAP_LOCATION_BITMASK | LLDP_MED_CAP_INVENTORY_BITMASK); 
         }
         else if((mode == L7_FALSE) && (numArg == 0))
         {
            memset(&medCap , 0, sizeof(medCap));
            medCap.bitmap[1] &= ((~LLDP_MED_CAP_CAPABILITIES_BITMASK )
                                 & (~LLDP_MED_CAP_NETWORKPOLICY_BITMASK)
                                 & (~LLDP_MED_CAP_EXT_PSE_BITMASK)
                                 & (~LLDP_MED_CAP_EXT_PD_BITMASK)
                                 & (~LLDP_MED_CAP_LOCATION_BITMASK)
                                 & (~LLDP_MED_CAP_INVENTORY_BITMASK));
         }
   
         i = 1;
         while (i <= numArg)
         {
           if (strcmp(argv[index+i], pStrInfo_base_capabilities)==0)
           {
             if (mode == L7_TRUE)
             {
                medCap.bitmap[1] |= LLDP_MED_CAP_CAPABILITIES_BITMASK;
             }
             else
             {
                medCap.bitmap[1] &= ~LLDP_MED_CAP_CAPABILITIES_BITMASK;
             }
           }
           else if (strcmp(argv[index+i], pStrInfo_base_networkPolicy)==0)
           {
             if (mode == L7_TRUE)
             {
                medCap.bitmap[1] |= LLDP_MED_CAP_NETWORKPOLICY_BITMASK;
             }
             else
             {
                medCap.bitmap[1] &= ~LLDP_MED_CAP_NETWORKPOLICY_BITMASK;
             }
           }
           else if (strcmp(argv[index+i], pStrInfo_base_EXPSE)==0)
           {
             if (mode == L7_TRUE)
             {
                medCap.bitmap[1] |= LLDP_MED_CAP_EXT_PSE_BITMASK;
             }
             else
             {
                medCap.bitmap[1] &= ~LLDP_MED_CAP_EXT_PSE_BITMASK;
             }
           }
           else if (strcmp(argv[index+i], pStrInfo_base_EXPD)==0)
           {
             if (mode == L7_TRUE)
             {
                medCap.bitmap[1] |= LLDP_MED_CAP_EXT_PD_BITMASK;
             }
             else
             {
                medCap.bitmap[1] &= ~LLDP_MED_CAP_EXT_PD_BITMASK;
             }
           }
           else if (strcmp(argv[index+i], pStrInfo_base_location)==0)
           {
             if (mode == L7_TRUE)
             {
                medCap.bitmap[1] |= LLDP_MED_CAP_LOCATION_BITMASK;
             }
             else
             {
                medCap.bitmap[1] &= ~LLDP_MED_CAP_LOCATION_BITMASK;
             }
           }
           else if (strcmp(argv[index+i], pStrInfo_base_inventory)==0)
           {
             if (mode == L7_TRUE)
             {
                medCap.bitmap[1] |= LLDP_MED_CAP_INVENTORY_BITMASK;
             }
             else
             {
                medCap.bitmap[1] &= ~LLDP_MED_CAP_INVENTORY_BITMASK;
             }
           }
           else
           {
              /* do nothing */
           }
           i++;
         }

         if (usmDbLldpXMedPortConfigTLVsEnabledSet(intIfNum, &medCap) != L7_SUCCESS)
         {
            countFailure = L7_FALSE;
         }

         rc = usmDbLldpValidIntfNextGet(intIfNum, &intIfNum);
       }
     }
  }

  if(countFailure)
  {
     return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_UnableToSetLLDPTxTLVsAll);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf );
  return cliPrompt(ewsContext);
}


