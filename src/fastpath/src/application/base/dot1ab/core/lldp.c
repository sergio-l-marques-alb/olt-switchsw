/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename lldp.c
*
* @purpose 802.1AB main functions
*
* @component 802.1AB
*
* @comments none
*
* @create 02/01/2005
*
* @author dfowler
* @end
*
**********************************************************************/
/*********************************************************************
 *
 ********************************************************************/

#include <string.h>
#include "l7_common.h"
#include "nimapi.h"
#include "sysapi.h"
#include "log.h"
#include "lldp.h"
#include "lldp_api.h"
#include "lldp_util.h"
#include "lldp_sid.h"

#ifdef L7_NSF_PACKAGE
#include "lldp_ckpt.h"
#endif /* L7_NSF_PACKAGE */

extern lldpCfgData_t  *lldpCfgData;
extern void           *lldpQueue;
L7_int32               lldpTaskId;

/*********************************************************************
* @purpose  Start 802.1AB task
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t lldpTaskStart()
{

  lldpTaskId = osapiTaskCreate("lldpTask", lldpTask, 0, 0,
                               lldpSidDefaultStackSize(),
                               lldpSidDefaultTaskPriority(),
                               lldpSidDefaultTaskSlice());

  if (lldpTaskId == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LLDP_COMPONENT_ID,
            "Could not create task lldpTask\n");
    return L7_FAILURE;
  }

  if (osapiWaitForTaskInit(L7_LLDP_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LLDP_COMPONENT_ID,
            "Unable to initialize lldpTask()\n");
    return(L7_FAILURE);
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose task to handle all 802.1AB management messages
*
* @param void
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
void lldpTask()
{
  lldpMgmtMsg_t msg;
  L7_RC_t rc;

  osapiTaskInitDone(L7_LLDP_TASK_SYNC);

  do
  {
    rc = osapiMessageReceive(lldpQueue, (void *)&msg,
                             LLDP_MSG_SIZE, L7_WAIT_FOREVER);
    switch (msg.msgId)
    {
      case (lldpMsgCnfgr):
        lldpCnfgrParse(&msg.u.CmdData);
        break;

      case (lldpMsgIntfChange):
        lldpIntfChangeProcess(msg.u.lldpIntfChangeParms.intIfNum,
                              msg.u.lldpIntfChangeParms.event,
                              msg.u.lldpIntfChangeParms.correlator);
        break;

      case (lldpMsgStartup):
        if (msg.u.lldpStartupParms.startupPhase == NIM_INTERFACE_CREATE_STARTUP)
        {
            lldpNimCreateStartup();
        }
        else
        {
            lldpNimActivateStartup();
        }
        break;

      case (lldpMsgTimer):
        lldpTimerProcess();
        break;

      case (lldpMsgPduReceive):
        lldpPduReceiveProcess(msg.u.lldpPduReceiveParms.intIfNum,
                              msg.u.lldpPduReceiveParms.bufHandle);
        break;

      case (lldpMsgNewBackupManager):
#ifdef L7_NSF_PACKAGE
        lldpNewBackupManager();
#endif /* L7_NSF_PACKAGE */
        break;

      case (lldpMsgMedPolicyInfo):
        break;

      case (lldpMsgIntfModeApply):
        if(msg.u.lldpMsgIntfModeApplyParms.rxApply == L7_TRUE)
        {
          lldpIntfRxModeApply(msg.u.lldpMsgIntfModeApplyParms.intIfNum);
        }
        if(msg.u.lldpMsgIntfModeApplyParms.txApply == L7_TRUE)
        {
          lldpIntfTxModeApply(msg.u.lldpMsgIntfModeApplyParms.intIfNum);
        }
        break; 

      case lldpMsgRemDbClear:
        lldpRemoteDBClear();
        break;

      default:
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LLDP_COMPONENT_ID, "lldpTask(): invalid message "
                                        "type:%d. %s:%d. Unsupported LLDP packet received.\n",
                                        msg.msgId, __FILE__, __LINE__);
        break;
    }
  } while (1);
}

/*********************************************************************
*
* @functions lldpInfoShow
*
* @purpose   Displays all config and nonconfig info for 802.1AB
*
* @param     none
*
* @returns   L7_SUCCESS
*
* @comments  Used for debug
*
* @end
*
*********************************************************************/
L7_RC_t lldpInfoShow()
{
  lldpCfgDump();

  return L7_SUCCESS;
}
