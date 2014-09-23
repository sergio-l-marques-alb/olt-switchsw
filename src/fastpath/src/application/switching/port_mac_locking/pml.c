/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename pml.c
*
* @purpose Port MAC Locking main functions
*
* @component Port MAC Locking
*
* @comments none
*
* @create 05/21/2004
*
* @author colinw
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
#include "pml.h"
#include "pml_api.h"
#include "pml_util.h"
#include "pml_sid.h"


extern pmlCfgData_t *pmlCfgData;
extern void         *pmlQueue;
L7_int32             pmlTaskId;
pmlIntfOprData_t    *pmlIntfOprData = L7_NULLPTR;

/*********************************************************************
* @purpose  Start Port MAC Locking task
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
L7_RC_t pmlStartTask()
{

  pmlTaskId = osapiTaskCreate("pmlTask", pmlTask, 0, 0,
                           pmlSidDefaultStackSize(),
                           pmlSidDefaultTaskPriority(),
                           pmlSidDefaultTaskSlice());

  if (pmlTaskId == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_PORT_MACLOCK_COMPONENT_ID,
            "Could not create task pmlTask\n");
    return L7_FAILURE;
  }

  if (osapiWaitForTaskInit (L7_PORT_MACLOCK_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_PORT_MACLOCK_COMPONENT_ID,
            "Unable to initialize pmlTask()\n");
    return(L7_FAILURE);
  }

  return(L7_SUCCESS);
}

/*********************************************************************
*
* @purpose task to handle all Port MAC Locking management messages
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
void pmlTask()
{
  pmlMgmtMsg_t msg;
  L7_RC_t rc;

  osapiTaskInitDone(L7_PORT_MACLOCK_TASK_SYNC);

  do
  {
    rc = osapiMessageReceive(pmlQueue, (void *)&msg, PML_MSG_SIZE, L7_WAIT_FOREVER);
    switch (msg.msgId)
    {
      case (pmlCnfgr):
        pmlCnfgrParse(&msg.u.CmdData);
        break;

      case (pmlIntfChange):
        pmlIntfChangeProcess(msg.intIfNum, msg.u.pmlIntfChangeParms.event, msg.u.pmlIntfChangeParms.correlator);
        break;

      case (pmlStartupNotify):
        pmlStartupNotifyProcess(msg.u.pmlStartupNotifyParms.startupPhase);
        break;

      default:
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_PORT_MACLOCK_COMPONENT_ID,
                "pmlTask(): invalid message type:%d. %s:%d\n", msg.msgId, __FILE__, __LINE__);
        break;
    }
  } while (1);
}

/*********************************************************************
*
* @functions pmlInfoShow
*
* @purpose   Displays all config and nonconfig info for Port MAC Locking
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
L7_RC_t pmlInfoShow()
{
  pmlCfgDump();

  return L7_SUCCESS;
}
