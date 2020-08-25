/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
 * @filename src/mgmt/cli/base/clicommands_lldp.c
*
* @purpose create the tree for CLI LLDP
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
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "strlib_switching_common.h"
#include "strlib_switching_cli.h"
#include "clicommands_lldp.h"
#include "lldp_api.h"
#include "lldp_exports.h"

void buildTreeGlobalConfigLldpMed(EwsCliCommandP depth2);
void buildTreeInterfaceConfigLldpMed(EwsCliCommandP depth2);
/*********************************************************************
* @purpose  To Build the LLDP tree in global configuration mode
*
* @param    EwsCliCommandP depth1		@b((input)) The parent node
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void buildTreeGlobalConfigLldp(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7, depth8;
  EwsCliCommandP depth9, depth10;

  /* Building the LLDP Command Tree */
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Lldp_1, pStrInfo_base_Lldp, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  /* sub tree for LLdp MED feature */
  buildTreeGlobalConfigLldpMed(depth2);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_NotificationIntvl_1,
                         pStrInfo_base_LldpNotIfIntvl,
                        commandLLDPNotificationInterval, 2,
                        L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_IntvlSeconds,
                         pStrInfo_base_LldpNotIfIntvlRange, NULL,
                        L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                        L7_NO_OPTIONAL_PARAMS);

  /*
    lldp timers interval <interval time> hold <hold value> reinit <reinit time>

    lldp timers
                |-> interval
                  |--> <interval time>
                    |---> <cr>
                    |---> hold
                      |---> <hold value>
                        |---> <cr>
                        |---> reinit
                          |---> <reinit time>
                            |---> <cr>
                    |---> reinit
                      |---> <reinit time>
                        |---> <cr>
                        |---> hold
                          |---> <hold value>
                            |---> <cr>

                |-> hold
                  |--> <hold value>
                    |---> <cr>
                    |---> interval
                      |---> <interval time>
                        |---> <cr>
                        |---> reinit
                          |---> <reinit time>

                            |---> <cr>
                    |---> reinit
                      |---> <reinit time>
                        |---> <cr>
                        |---> interval
                          |---> <interval time>
                            |---> <cr>

                |-> reinit
                  |--> <reinit time>
                    |---> <cr>
                    |---> interval
                      |---> <interval time>
                        |---> <cr>
                        |---> hold
                          |---> <hold value>
                            |---> <cr>
                    |---> hold
                      |---> <hold value>
                        |---> <cr>
                        |---> interval
                          |---> <interval time>
                            |---> <cr>
  */

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Timers, pStrInfo_base_LldpTimers,
                        commandLLDPTimers, 2, L7_NO_COMMAND_SUPPORTED,
                        L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, L7_NO_OPTIONAL_PARAMS);

  /* start */
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_LldpHold_1, pStrInfo_base_LldpTimersHold, NULL,
              2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_base_HoldVal,
                         pStrInfo_base_LldpTimersHoldValRange2And10, NULL,
                        L7_NO_OPTIONAL_PARAMS);

  /* start */
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                        L7_NO_OPTIONAL_PARAMS);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_WsInputIntvl, pStrInfo_base_LldpTimersInt, NULL,
                        2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_IntvlSeconds,
                         pStrInfo_base_LldpTimersRange5And32768, NULL,
                        L7_NO_OPTIONAL_PARAMS);

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                        L7_NO_OPTIONAL_PARAMS);

  depth8 = ewsCliAddNode(depth7, pStrInfo_base_LldpReinit_1, pStrInfo_base_LldpTimersReinit, NULL,
                        2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth9 = ewsCliAddNode(depth8, pStrInfo_base_ReinitSeconds,
                         pStrInfo_base_LldpTimersReinitVal, NULL,
                        L7_NO_OPTIONAL_PARAMS);

  depth10 = ewsCliAddNode(depth9, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);
  /* end */

  /* start */
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LldpReinit_1, pStrInfo_base_LldpTimersReinit, NULL,
                        2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_ReinitSeconds,
                         pStrInfo_base_LldpTimersReinitVal, NULL,
                        L7_NO_OPTIONAL_PARAMS);

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                        L7_NO_OPTIONAL_PARAMS);

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_WsInputIntvl, pStrInfo_base_LldpTimersInt, NULL,
                        2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth9 = ewsCliAddNode(depth8, pStrInfo_base_IntvlSeconds,
                         pStrInfo_base_LldpTimersRange5And32768, NULL,
                        L7_NO_OPTIONAL_PARAMS);

  depth10 = ewsCliAddNode(depth9, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);
  /* end */
  /* end */

  /* start */
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_WsInputIntvl, pStrInfo_base_LldpTimersInt, NULL,
                        2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_base_IntvlSeconds,
                         pStrInfo_base_LldpTimersRange5And32768, NULL,
                        L7_NO_OPTIONAL_PARAMS);

  /* start */
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                        L7_NO_OPTIONAL_PARAMS);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LldpHold_1, pStrInfo_base_LldpTimersHold, NULL,
                        2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_HoldVal,
                         pStrInfo_base_LldpTimersHoldValRange2And10, NULL,
                        L7_NO_OPTIONAL_PARAMS);

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                        L7_NO_OPTIONAL_PARAMS);

  depth8 = ewsCliAddNode(depth7, pStrInfo_base_LldpReinit_1, pStrInfo_base_LldpTimersReinit, NULL,
                        2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth9 = ewsCliAddNode(depth8, pStrInfo_base_ReinitSeconds,
                         pStrInfo_base_LldpTimersReinitVal, NULL,
                        L7_NO_OPTIONAL_PARAMS);

  depth10 = ewsCliAddNode(depth9, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                        L7_NO_OPTIONAL_PARAMS);
  /* end */

  /* start */
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LldpReinit_1, pStrInfo_base_LldpTimersReinit, NULL,
                        2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_ReinitSeconds,
                         pStrInfo_base_LldpTimersReinitVal, NULL,
                        L7_NO_OPTIONAL_PARAMS);

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                        L7_NO_OPTIONAL_PARAMS);

  depth8 = ewsCliAddNode(depth7, pStrInfo_base_LldpHold_1, pStrInfo_base_LldpTimersHold, NULL,
                        2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth9 = ewsCliAddNode(depth8, pStrInfo_base_HoldVal,
                         pStrInfo_base_LldpTimersHoldValRange2And10, NULL,
                        L7_NO_OPTIONAL_PARAMS);

  depth10 = ewsCliAddNode(depth9, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                        L7_NO_OPTIONAL_PARAMS);
  /* end */
  /* end */

  /* start */
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_LldpReinit_1, pStrInfo_base_LldpTimersReinit, NULL,
                        2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_base_ReinitSeconds,
                         pStrInfo_base_LldpTimersReinitVal, NULL,
                        L7_NO_OPTIONAL_PARAMS);

  /* start */
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                        L7_NO_OPTIONAL_PARAMS);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LldpHold_1, pStrInfo_base_LldpTimersHold, NULL,
                        2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_HoldVal,
                         pStrInfo_base_LldpTimersHoldValRange2And10, NULL,
                        L7_NO_OPTIONAL_PARAMS);

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                        L7_NO_OPTIONAL_PARAMS);

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_WsInputIntvl, pStrInfo_base_LldpTimersInt, NULL,
                        2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth9 = ewsCliAddNode(depth8, pStrInfo_base_IntvlSeconds,
                         pStrInfo_base_LldpTimersRange5And32768, NULL,
                        L7_NO_OPTIONAL_PARAMS);

  depth10 = ewsCliAddNode(depth9, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);
  /* end */

  /* start */
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_WsInputIntvl, pStrInfo_base_LldpTimersInt, NULL,
                        2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_IntvlSeconds,
                         pStrInfo_base_LldpTimersRange5And32768, NULL,
                        L7_NO_OPTIONAL_PARAMS);

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                        L7_NO_OPTIONAL_PARAMS);

  depth8 = ewsCliAddNode(depth7, pStrInfo_base_LldpHold_1, pStrInfo_base_LldpTimersHold, NULL,
                        2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth9 = ewsCliAddNode(depth8, pStrInfo_base_HoldVal,
                         pStrInfo_base_LldpTimersHoldValRange2And10, NULL,
                        L7_NO_OPTIONAL_PARAMS);

  depth10 = ewsCliAddNode(depth9, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);
  /* end */
  /* end */

}

/*********************************************************************
* @purpose  To Build the LLDP tree in interface configuration mode
*
* @param    EwsCliCommandP depth1		@b((input)) The parent node
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void buildTreeInterfaceConfigLldp(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7, depth8;

  /* Building the LLDP Command Tree */
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Lldp_1, pStrInfo_base_Lldp, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  
  buildTreeInterfaceConfigLldpMed(depth2);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Notification, pStrInfo_base_LldpNotification,
                        commandLLDPNotification, 2,
                        L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                        L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Receive_1, pStrInfo_base_LldpReceive,
                        commandLLDPReceive, 2, L7_NO_COMMAND_SUPPORTED,
                        L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                        L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Tx_1, pStrInfo_base_LldpTx,
                        commandLLDPTransmit, 2, L7_NO_COMMAND_SUPPORTED,
                        L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                        L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_TxMgmt, pStrInfo_base_LldpTransMgmt,
                        commandLLDPTransmitMgmt, 2,
                        L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                        L7_NO_OPTIONAL_PARAMS);
  /*
    lldp transmit-tlv
            |->sys-name
              |-> <cr>
              |-> <sys-cap>
                |--> <cr>
                |--> <sys-desc>
                  |---> <cr>
                  |---> <port-desc>
                    |-> <cr>
                |--> <port-desc>
                  |---> <cr>
                  |---> <sys-desc>
                    |-> <cr>
              |-> <sys-desc>
                |--> <cr>
                |--> <sys-cap>
                  |---> <cr>
                  |---> <port-desc>
                    |-> <cr>
                |--> <port-desc>
                  |---> <cr>
                  |---> <sys-cap>
                    |-> <cr>
              |-> <port-desc>
                |--> <cr>
                |--> <sys-cap>
                  |---> <cr>
                  |---> <sys-desc>
                    |-> <cr>
                |--> <sys-desc>
                  |---> <cr>
                  |---> <sys-cap>
                    |-> <cr>

            |->sys-desc
              |-> <cr>
              |-> <sys-name>
                |--> <cr>
                |--> <sys-cap>
                  |---> <cr>
                  |---> <port-desc>
                    |-> <cr>
                |--> <port-desc>
                  |---> <cr>
                  |---> <sys-cap>
                    |-> <cr>
              |-> <sys-cap>
                |--> <cr>
                |--> <sys-name>
                  |---> <cr>
                  |---> <port-desc>
                    |-> <cr>
                |--> <port-desc>
                  |---> <cr>
                  |---> <sys-name>
                    |-> <cr>
              |-> <port-desc>
                |--> <cr>
                |--> <sys-name>
                  |---> <cr>
                  |---> <sys-cap>
                    |-> <cr>
                |--> <sys-cap>
                  |---> <cr>
                  |---> <sys-name>
                    |-> <cr>

            |->sys-cap
              |-> <cr>
              |-> <sys-name>
                |--> <cr>
                |--> <sys-desc>
                  |---> <cr>
                  |---> <port-desc>
                    |-> <cr>
                |--> <port-desc>
                  |---> <cr>
                  |---> <sys-desc>
                    |-> <cr>
              |-> <sys-desc>
                |--> <cr>
                |--> <sys-name>
                  |---> <cr>
                  |---> <port-desc>
                    |-> <cr>
                |--> <port-desc>
                  |---> <cr>
                  |---> <sys-name>
                    |-> <cr>
              |-> <port-desc>
                |--> <cr>
                |--> <sys-name>
                  |---> <cr>
                  |---> <sys-desc>
                    |-> <cr>
                |--> <sys-desc>
                  |---> <cr>
                  |---> <sys-name>
                    |-> <cr>

            |->port-desc
              |-> <cr>
              |-> <sys-name>
                |--> <cr>
                |--> <sys-desc>
                  |---> <cr>
                  |---> <sys-cap>
                    |-> <cr>
                |--> <sys-cap>
                  |---> <cr>
                  |---> <sys-desc>
                    |-> <cr>
              |-> <sys-desc>
                |--> <cr>
                |--> <sys-name>
                  |---> <cr>
                  |---> <sys-cap>
                    |-> <cr>
                |--> <sys-cap>
                  |---> <cr>
                  |---> <sys-name>
                    |-> <cr>
              |-> <sys-cap>
                |--> <cr>
                |--> <sys-name>
                  |---> <cr>
                  |---> <sys-desc>
                    |-> <cr>
                |--> <sys-desc>
                  |---> <cr>
                  |---> <sys-name>
                    |-> <cr>

  */
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_TxTlv, pStrInfo_base_LldpTransTlv,
                        commandLLDPTransmitTLV, 2, L7_NO_COMMAND_SUPPORTED,
                        L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
/* start port-desc */
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_LldpPortDesc, pStrInfo_base_LldpTransTlvPort,
                        NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  /* start PORT-DESC SYS-CAP*/
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_LldpSyscap, pStrInfo_base_LldpTransTlvCap,
                         NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LldpSysdesc, pStrInfo_base_LldpTransTlvDesc,
                         NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_LldpSysname, pStrInfo_base_LldpTransTlvName,
                         NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LldpSysname, pStrInfo_base_LldpTransTlvName,
                         NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_LldpSysdesc, pStrInfo_base_LldpTransTlvDesc,
                         NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

/* end PORT-DESC SYS-CAP*/


  depth5 = ewsCliAddNode(depth4, pStrInfo_base_LldpSysdesc, pStrInfo_base_LldpTransTlvDesc,
                        NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LldpSyscap, pStrInfo_base_LldpTransTlvCap,
                        NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_LldpSysname, pStrInfo_base_LldpTransTlvName,
                        NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LldpSysname, pStrInfo_base_LldpTransTlvName,
                        NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_LldpSyscap, pStrInfo_base_LldpTransTlvCap,
                        NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* end PORT-DESC SYS-DESC*/

  /* start PORT-DESC SYS-name*/
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_LldpSysname, pStrInfo_base_LldpTransTlvName,
                         NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LldpSyscap, pStrInfo_base_LldpTransTlvCap,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_LldpSysdesc, pStrInfo_base_LldpTransTlvDesc,
                        NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LldpSysdesc, pStrInfo_base_LldpTransTlvDesc,
                         NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_LldpSyscap, pStrInfo_base_LldpTransTlvCap,
                         NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  /* end PORT-DESC SYS-name*/

/* end port-desc*/

  /* start  sys-cap*/
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_LldpSyscap, pStrInfo_base_LldpTransTlvCap,
                         NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

/* start sys-cap port-desc*/
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_LldpPortDesc, pStrInfo_base_LldpTransTlvPort,
                        NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LldpSysdesc, pStrInfo_base_LldpTransTlvDesc,
                        NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_LldpSysname, pStrInfo_base_LldpTransTlvName,
                        NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LldpSysname, pStrInfo_base_LldpTransTlvName,
                        NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_LldpSysdesc, pStrInfo_base_LldpTransTlvDesc,
                        NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  /* end sys-cap port-desc*/

/* start sys-cap sys-desc*/
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_LldpSysdesc, pStrInfo_base_LldpTransTlvDesc,
                        NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LldpPortDesc, pStrInfo_base_LldpTransTlvPort,
                        NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_LldpSysname, pStrInfo_base_LldpTransTlvName,
                        NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LldpSysname, pStrInfo_base_LldpTransTlvName,
                        NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_LldpPortDesc, pStrInfo_base_LldpTransTlvPort,
                        NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
/* end sys-cap sys-desc*/

/* start sys-cap sys-name*/
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_LldpSysname, pStrInfo_base_LldpTransTlvName,
                        NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LldpPortDesc, pStrInfo_base_LldpTransTlvPort,
                        NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_LldpSysdesc, pStrInfo_base_LldpTransTlvDesc,
                        NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LldpSysdesc, pStrInfo_base_LldpTransTlvDesc,
                        NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_LldpPortDesc, pStrInfo_base_LldpTransTlvPort,
                        NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  /* end sys-cap sys-name*/

/* end sys-cap*/

/* start sys-desc*/
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_LldpSysdesc, pStrInfo_base_LldpTransTlvDesc,
                        NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

/* start port-desc*/
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_LldpPortDesc, pStrInfo_base_LldpTransTlvPort,
                        NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LldpSyscap, pStrInfo_base_LldpTransTlvCap,
                        NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_LldpSysname, pStrInfo_base_LldpTransTlvName,
                        NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LldpSysname, pStrInfo_base_LldpTransTlvName,
                        NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_LldpSyscap, pStrInfo_base_LldpTransTlvCap,
                        NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

/* end  sys-desc port-desc*/

  /* start sys-desc  sys-cap*/
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_LldpSyscap, pStrInfo_base_LldpTransTlvCap,
                          NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                          NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LldpPortDesc, pStrInfo_base_LldpTransTlvPort,
                          NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                          NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_LldpSysname, pStrInfo_base_LldpTransTlvName,
                          NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                          NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LldpSysname, pStrInfo_base_LldpTransTlvName,
                          NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                          NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_LldpPortDesc, pStrInfo_base_LldpTransTlvPort,
                          NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                          NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  /* end sys-desc sys-cap*/

  /* start sys-desc sys-name*/
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_LldpSysname, pStrInfo_base_LldpTransTlvName,
                          NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                          NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LldpPortDesc, pStrInfo_base_LldpTransTlvPort,
                          NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                          NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_LldpSyscap, pStrInfo_base_LldpTransTlvCap,
                          NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                          NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LldpSyscap, pStrInfo_base_LldpTransTlvCap,
                          NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                          NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_LldpPortDesc, pStrInfo_base_LldpTransTlvPort,
                          NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                          NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    /* end sys-desc sys-name*/

/* end sys-desc*/

  /* start  sys-name*/
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_LldpSysname, pStrInfo_base_LldpTransTlvName,
                          NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                          NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* start sys-name port-desc*/
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_LldpPortDesc, pStrInfo_base_LldpTransTlvPort,
                        NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LldpSyscap, pStrInfo_base_LldpTransTlvCap,
                        NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_LldpSysdesc, pStrInfo_base_LldpTransTlvDesc,
                        NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LldpSysdesc, pStrInfo_base_LldpTransTlvDesc,
                        NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_LldpSyscap, pStrInfo_base_LldpTransTlvCap,
                        NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                        NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  /* end sys-name port-desc*/

  /* start sys-name sys-cap*/
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_LldpSyscap, pStrInfo_base_LldpTransTlvCap,
                         NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LldpPortDesc, pStrInfo_base_LldpTransTlvPort,
                         NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_LldpSysdesc, pStrInfo_base_LldpTransTlvDesc,
                         NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LldpSysdesc, pStrInfo_base_LldpTransTlvDesc,
                         NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_LldpPortDesc, pStrInfo_base_LldpTransTlvPort,
                         NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  /* end sys-name sys-cap*/

  /* start sys-name sys-desc*/
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_LldpSysdesc, pStrInfo_base_LldpTransTlvDesc,
                         NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LldpPortDesc, pStrInfo_base_LldpTransTlvPort,
                         NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_LldpSyscap, pStrInfo_base_LldpTransTlvCap,
                         NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LldpSyscap, pStrInfo_base_LldpTransTlvCap,
                         NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_base_LldpPortDesc, pStrInfo_base_LldpTransTlvPort,
                         NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* end sys-name sys-desc*/

  /* end sys-name*/
}

void buildTreeShowConfigLldpMed(EwsCliCommandP depth2);

/*********************************************************************
* @purpose  To Build the LLDP tree for show commands
*
* @param    EwsCliCommandP depth1		@b((input)) The parent node
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void buildTreeShowConfigLldp(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6;

  /* Building the LLDP Command Tree */
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Lldp_1, pStrInfo_base_ShowLldp, commandLLDPShow, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine,NULL, L7_NO_OPTIONAL_PARAMS);

  /* Interface Information */
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Ipv6DhcpRelayIntf_1, pStrInfo_base_ShowLldpIntf, commandLLDPShowInterface, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_All, pStrInfo_common_ShowRtrMcastBoundaryAll, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr,  pStrInfo_common_NewLine,NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = buildTreeInterfaceHelp(depth3, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr,  pStrInfo_common_NewLine,NULL, L7_NO_OPTIONAL_PARAMS);

  /* Local Information */
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_LocalDevice, pStrInfo_base_ShowLocalDevice, commandLLDPShowLocalDevice, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_All, pStrInfo_common_ShowRtrMcastBoundaryAll, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr,  pStrInfo_common_NewLine,NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_IgmpProxyGrpsDetail, pStrInfo_base_ShowLocalDetail, commandLLDPShowLocalDeviceDetail, L7_NO_OPTIONAL_PARAMS);

  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr,  pStrInfo_common_NewLine,NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = buildTreeInterfaceHelp(depth3, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr,  pStrInfo_common_NewLine,NULL, L7_NO_OPTIONAL_PARAMS);

  /* MED Information */
  buildTreeShowConfigLldpMed(depth2);
  
  /* Remote Information */
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_RemoteDevice_1, pStrInfo_base_ShowRemoteDevice, commandLLDPShowRemoteDevice, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_All, pStrInfo_common_ShowRtrMcastBoundaryAll, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr,  pStrInfo_common_NewLine,NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_IgmpProxyGrpsDetail, pStrInfo_base_ShowRemoteDetail, commandLLDPShowRemoteDeviceDetail, L7_NO_OPTIONAL_PARAMS);

  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr,  pStrInfo_common_NewLine,NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = buildTreeInterfaceHelp(depth3, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr,  pStrInfo_common_NewLine,NULL, L7_NO_OPTIONAL_PARAMS);

  /* Statistics Information */
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Dot1xShowStats, pStrInfo_base_ShowStats, commandLLDPShowStatistics, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_All, pStrInfo_common_ShowRtrMcastBoundaryAll, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr,  pStrInfo_common_NewLine,NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = buildTreeInterfaceHelp(depth3, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr,  pStrInfo_common_NewLine,NULL, L7_NO_OPTIONAL_PARAMS);

}
/*********************************************************************
* @purpose  To Build the LLDP tree in global clear configuration mode
*
* @param    EwsCliCommandP depth1		@b((input)) The parent node
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void buildTreeGlobalLldpClear(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5;

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Lldp_1, pStrInfo_base_ClrLldpInfo, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_RemoteData_1, pStrInfo_base_ClrLldpRemoteData, commandLLDPClearRemoteData, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Dot1xShowStats, pStrInfo_base_ClrLldpStats, commandLLDPClearStats, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}


/*********************************************************************
* @purpose  To Build the LLDP MED tree in global configuration mode
*
* @param    EwsCliCommandP depth2		@b((input)) The parent node
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void buildTreeGlobalConfigLldpMed(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6, depth7;
  
  /* lldp med all  */
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_MED, pStrInfo_base_LldpMedCfg, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_all_1, pStrInfo_base_LldpMedCfgAllPorts, 
                         commandLLDPMedAll,  6, 
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                         L7_SYNTAX_NORMAL, pStrInfo_switching_HelpLldpMedAll,
                         L7_SYNTAX_NO, pStrInfo_switching_HelpNoLldpMedAll);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  /* lldp med confignotification all  */
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_confignotification, pStrInfo_base_CfgSendTopologyNotifyAll, 
                         NULL,  6, 
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                         L7_SYNTAX_NORMAL, pStrInfo_switching_HelpLldpMedCfgAll,
                         L7_SYNTAX_NO, pStrInfo_switching_HelpNoLldpMedCfgAll);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_all_1, pStrInfo_base_CfgSendTopologyNotifyAll, 
                         commandLLDPMedConfigNotificationAll,  6, 
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                         L7_SYNTAX_NORMAL, pStrInfo_switching_HelpLldpMedCfgAll,
                         L7_SYNTAX_NO, pStrInfo_switching_HelpNoLldpMedCfgAll);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_faststartrepeatcount, pStrInfo_base_ConfigureFastStartRepeatCount, 
                         commandLLDPMedFastStartRepeatCount, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range1to10, pStrInfo_base_HelpNoLLDPPDUsSent, NULL, 5, 
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY,
                         L7_NODE_UINT_RANGE, LLDP_MED_FASTSTART_REPEAT_COUNT_MIN, LLDP_MED_FASTSTART_REPEAT_COUNT_MAX);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  /* Transmit TLVs */
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_transmitTLV, pStrInfo_base_ConfigoptTLVssent, 
                         NULL, 6, 
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                         L7_SYNTAX_NORMAL, pStrInfo_switching_HelpLldpMedTransmitTLVAll,
                         L7_SYNTAX_NO, pStrInfo_switching_HelpLldpMedTransmitTLVNoAll);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_all_1, pStrInfo_base_ConfigoptTLVssent, 
                         commandLLDPMedTransmitTlvAll, 6, 
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                         L7_SYNTAX_NORMAL, pStrInfo_switching_HelpLldpMedTransmitTLVAll,
                         L7_SYNTAX_NO, pStrInfo_switching_HelpLldpMedTransmitTLVNoAll);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_capabilities, pStrInfo_base_IncldExcldLLDPcapTLV, 
                         NULL, 4, 
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                         L7_OPTIONS_NODE, depth5);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_networkPolicy, pStrInfo_base_IncldExcldLLDPNetworkPolicyTLV, 
                         NULL, 4, 
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                         L7_OPTIONS_NODE, depth5);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_EXPSE, pStrInfo_base_IncldExcldLLDPPSETLV, 
                         NULL, 4, 
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                         L7_OPTIONS_NODE, depth5);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_EXPD, pStrInfo_base_IncldExcldLLDPPDTLV, 
                         NULL, 4, 
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                         L7_OPTIONS_NODE, depth5);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_location, pStrInfo_base_IncldExcldLLDPLocationTLV, 
                         NULL, 4, 
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                         L7_OPTIONS_NODE, depth5);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_inventory, pStrInfo_base_IncldExcldLLDPInventoryTLV, 
                         NULL, 4, 
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                         L7_OPTIONS_NODE, depth5);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
}

/*********************************************************************
* @purpose  To Build the LLDP MED tree in interface configuration mode
*
* @param    EwsCliCommandP depth2		@b((input)) The parent node
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void buildTreeInterfaceConfigLldpMed(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6;

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_MED, pStrInfo_base_LldpMedCfg, 
                         commandLLDPMed, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_confignotification, pStrInfo_base_CfgSendTopologyNotify, 
                         commandLLDPMedConfigNotification, 6, 
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                         L7_SYNTAX_NORMAL, pStrInfo_switching_HelpLldpMedCfg,
                         L7_SYNTAX_NO, pStrInfo_switching_HelpLldpMedCfgNo);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_transmitTLV, pStrInfo_base_ConfigoptIntfTLVssent, 
                         commandLLDPMedTransmitTlv, 6, 
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                         L7_SYNTAX_NORMAL, pStrInfo_switching_HelpLldpMedTransmitTLV,
                         L7_SYNTAX_NO, pStrInfo_switching_HelpLldpMedTransmitTLVNo);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_base_capabilities, pStrInfo_base_IncldExcldLLDPcapTLV, 
                         NULL, 4, 
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                         L7_OPTIONS_NODE, depth4);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_base_networkPolicy, pStrInfo_base_IncldExcldLLDPNetworkPolicyTLV, 
                         NULL, 4, 
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                         L7_OPTIONS_NODE, depth4);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_base_EXPSE, pStrInfo_base_IncldExcldLLDPPSETLV, 
                         NULL, 4, 
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                         L7_OPTIONS_NODE, depth4);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_base_EXPD, pStrInfo_base_IncldExcldLLDPPDTLV, 
                         NULL, 4, 
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                         L7_OPTIONS_NODE, depth4);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_base_location, pStrInfo_base_IncldExcldLLDPLocationTLV, 
                         NULL, 4, 
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                         L7_OPTIONS_NODE, depth4);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_base_inventory, pStrInfo_base_IncldExcldLLDPInventoryTLV, 
                         NULL, 4, 
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                         L7_OPTIONS_NODE, depth4);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
}

/*********************************************************************
* @purpose  To Build the LLDP MED tree for show commands
*
* @param    EwsCliCommandP depth2		@b((input)) The parent node
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void buildTreeShowConfigLldpMed(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6, depth7;

  /* Building the LLDP MED Command Tree */
  /* show lldp med */
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_MED, pStrInfo_base_DsplyLLDPMEDCfg, 
                         commandLLDPMedShow, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* show lldp med interface {<unit/slot/port> | all} */
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_interface, pStrInfo_base_DsplyLLDPMEDCfgIntIf, 
                         commandLLDPMedShowInterface, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_base_all_1, pStrInfo_base_DsplyLLDPMEDCfgIntIf, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr,  pStrInfo_common_NewLine,NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* show lldp med local-device detail <unit/slot/port> */
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_localDevice, pStrInfo_base_DsplyLLDPMEDCfgLocalDevice, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_base_detail, pStrInfo_base_DsplyLLDPMEDCfgLocalDevice, 
                         commandLLDPMedShowLocalDeviceDetail, L7_NO_OPTIONAL_PARAMS);
  depth6 = buildTreeInterfaceHelp(depth5, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr,  pStrInfo_common_NewLine,NULL, L7_NO_OPTIONAL_PARAMS);

  /* show lldp med interface {<unit/slot/port> | all | detail <unit/slot/port>} */
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_remoteDevice, pStrInfo_base_DsplyLLDPMEDCfgRemoteDevice, 
                         commandLLDPMedShowRemoteDevice, 2,
                         L7_SYNTAX_NORMAL, pStrInfo_switching_HelpLldpMedRemote);

  depth5 = ewsCliAddNode(depth4, pStrInfo_base_all_1, pStrInfo_base_DsplyLLDPMEDCfgRemoteDevices_All, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_base_detail, pStrInfo_base_DsplyLLDPMEDDetailedCfgRemoteDevice, 
                         commandLLDPMedShowRemoteDeviceDetail, L7_NO_OPTIONAL_PARAMS);
  depth6 = buildTreeInterfaceHelp(depth5, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine,NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

