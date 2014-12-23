/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    ipmap_arp.c
* @purpose     IP Mapping layer internal ARP functions
* @component   IP Mapping Layer -- ARP
* @comments    Maps to the ARP object API.
*
*
* @comments    Functions in this file explictly state whether they
*              take the ARP component lock depending on their thread
*              context.
*
*
* @create      03/21/2001
* @author      gpaussa
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/



#include "l7_common.h"
#include "ipmap_arp.h"
#include "ipmap_sid.h"
#include "l3end_api.h"
#include "oll.ext"
#include "std.h"
#include "log.h"
#include "l7utils_api.h"
#include "arp_debug.h"
#include "arp.h"

/* External References */
extern void osapiDebugStackTrace (L7_uint32 task_id, FILE *fp);

/* Global data */
ipMapArpCtx_t   ipMapArpCtx_g;          /* IP Mapping layer ARP global context*/
ipMapArpIntf_t  *ipMapArpIntfInUse_g[IPM_ARP_INTF_MAX]; /* not using entry 0 */
L7_BOOL         ipMapRoutingStarted = L7_FALSE;          /* vendor subsystem was started */

/* ARP trace buffer */
ipMapArpTrace_t   ipMapArpTrace_g[IPM_ARP_TRACE_MAX];
L7_uint32         ipMapArpTraceIdx_g = 0;


extern L7_uchar8 ipMapArpBcastMac[L7_MAC_ADDR_LEN];

/**************************************************************************
 * ARP Interface Lock/Sema Debug
 *
 * ipMapArpDebugIntfLockTraceSet - enables lock tracing
 * ipMapArpDebugIntfLockRefCount - prints refcounts for read and write locks
 * void ipMapArpDebugTimeoutSet  - sets the timeout in msec for ARP locks, sema
 *
 *************************************************************************/
#define ARP_HIST_FILE_LEN 15
#define ARP_HIST_FUNC_LEN 30
#define ARP_HIST_ITEMS    200
struct arpLockHistory_s
{
  L7_char8  file[ARP_HIST_FILE_LEN+1];
  L7_uint32 line;
  L7_uint32 tid;
  L7_uint32 lockid;
  L7_char8  fname[ARP_HIST_FUNC_LEN+1];
  L7_uint32 ts;
};

static struct arpLockHistory_s arpLockHistory[ARP_HIST_ITEMS];
static L7_uint32 arpLockHistoryIdx = 0;

L7_int32 arpLockDebugTimeout = L7_WAIT_FOREVER;
L7_BOOL  arpDeadlocked = 0;

void ipMapArpDebugTimeoutSet(L7_int32 val)
{
  arpLockDebugTimeout = val;
  memset(arpLockHistory, 0, sizeof(struct arpLockHistory_s) * ARP_HIST_ITEMS);
  arpLockHistoryIdx = 0;
}

static e_Err ipMapARPResolution (IN t_Handle   arpHandle,
                                 IN t_Handle   userId,
                                 IN OUT t_ARPResInfo *arpResInfo);

void ipMapArpDebugDumpLockHistory()
{
  L7_uint32 cnt = ARP_HIST_ITEMS, idx = arpLockHistoryIdx;
  struct arpLockHistory_s * phist;

  sysapiPrintf("\nARP Lock History");

  if(!idx)
  {
    sysapiPrintf("\nNone");
    return;
  }

  while(cnt && idx)
  {
    idx--;
    cnt--;

    phist = &arpLockHistory[idx % ARP_HIST_ITEMS];

    sysapiPrintf("\n%d:%s\ttid 0x%x on 0x%x from %s:%u",
                 phist->ts, phist->fname, phist->tid,
                 phist->lockid, phist->file, phist->line);
  }
}

void arpLogLockAction(L7_char8 * fname, L7_char8 * file, L7_uint32 line, L7_uint32 lockid)
{
  struct arpLockHistory_s * phist;

  if(arpDeadlocked)
    return;

  phist = &arpLockHistory[arpLockHistoryIdx % ARP_HIST_ITEMS];
  arpLockHistoryIdx++;

  l7utilsFilenameStrip(&file);
  strncpy(phist->file, file, ARP_HIST_FILE_LEN);
  strncpy(phist->fname, fname, ARP_HIST_FUNC_LEN);
  phist->line   = line;
  phist->lockid = lockid;
  phist->tid    = osapiTaskIdSelf();
  phist->ts     = osapiTimeMillisecondsGet();
}

/* when a lock fails (suspected deadlock) this functions dumps the stack trace */
void arpLockShowStack(L7_char8 * fname, L7_uint32 lockid, L7_char8 * file, L7_uint32 line)
{
  osapiSleep(osapiTaskIdSelf() % 60);
  l7utilsFilenameStrip(&file);
  sysapiPrintf("\n%s failed to lock 0x%x tsk 0x%x %s:%u",
               fname, lockid, osapiTaskIdSelf(), file, line);
  L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
          "\n%s failed tsk 0x%x %s:%u",
               fname, osapiTaskIdSelf(), file, line);
  osapiSleep(90);
  if(!arpDeadlocked)
  {
    /* attempt to avoid multiple traces */
    arpDeadlocked = L7_TRUE;

    /* dump debug inforamation */
    ipMapArpDebugDumpLockHistory();
    ipMapArpCtxShow();
    osapiShowTasks();
    osapiDebugStackTrace(0, 0);
    osapiSleep(60);
    LOG_ERROR(0xbadd);
  }

  /* die */
  while(1)
    osapiSleep(60);
}

/*********************************************************************
* @purpose  Initializes IPM ARP context structure
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void ipMapArpCtxInit(void)
{
  ipMapArpCtx_t *pArpCtx = &ipMapArpCtx_g;

  /* set everything to zero, then set any specific values as needed */
  memset(pArpCtx, 0, sizeof(*pArpCtx));
  pArpCtx->cacheMaxCt = (L7_uint32)platRtrArpMaxEntriesGet();
  pArpCtx->staticMaxCt = (L7_uint32)L7_IP_ARP_CACHE_STATIC_MAX;

  /* Used to size this by max routes, but max routes has increased enough
   * that we we wasting memory with this worst case assumption. So make the 
   * gateway table the minimum of 3 * max router interfaces 
   * and max routes. Generally will not have more than three router neighbors 
   * per routing interface. If we do fill up the table, there should be no 
   * harm done. Looks like the table is only used now to re-trigger resolution
   * of static next hops. */
  if (platRtrRouteMaxEntriesGet() < (3 * L7_MAX_NUM_ROUTER_INTF))
  {
    pArpCtx->gwTbl.treeEntryMax = platRtrRouteMaxEntriesGet();
  }
  else
  {
    pArpCtx->gwTbl.treeEntryMax = 3 * L7_MAX_NUM_ROUTER_INTF;
  }
  pArpCtx->ipTbl.treeEntryMax = (L7_uint32)platRtrArpMaxEntriesGet();
  pArpCtx->reissue.pMsgQ = L7_NULLPTR;
  pArpCtx->reissue.maxCt = (L7_uint32)FD_CNFGR_ARP_REISSUE_MSG_Q_COUNT;
  pArpCtx->callback.pMsgQ = L7_NULLPTR;
  pArpCtx->callback.maxCt = (L7_uint32)FD_CNFGR_ARP_CALLBACK_MSG_Q_COUNT;
}


/*********************************************************************
* @purpose  Initialize ARP task
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapArpStartTasks(void)
{
  static const char *routine_name = "ipMapArpStartTasks";
  L7_uint32 queueLengths[IPM_ARP_NUM_QUEUES];

  /* create ARP Query task */
  ipMapArpCtx_g.reissue.taskId =
    osapiTaskCreate(IPM_ARP_REISSUE_TASK_NAME, (void *)ipMapArpReissueTask,
                    0, 0,
                    L7_DEFAULT_STACK_SIZE,
                    L7_DEFAULT_TASK_PRIORITY,
                    L7_DEFAULT_TASK_SLICE);

  if (ipMapArpCtx_g.reissue.taskId == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "%s: Failed to create ARP Reissue task\n", routine_name);
    return L7_FAILURE;
  }

  if (osapiWaitForTaskInit (L7_IPMAP_ARP_REISSUE_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "%s: Unable to initialize ARP Reissue task()\n", routine_name);
    return L7_FAILURE;
  }


  /* create ARP Callback task */
  ipMapArpCtx_g.callback.taskId =
    osapiTaskCreate(IPM_ARP_CALLBACK_TASK_NAME, (void *)ipMapArpCallbackTask,
                    0, 0,
                    L7_DEFAULT_STACK_SIZE,
                    L7_DEFAULT_TASK_PRIORITY,
                    L7_DEFAULT_TASK_SLICE);

  if (ipMapArpCtx_g.callback.taskId == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "%s: Failed to create ARP Callback task\n", routine_name);
    return L7_FAILURE;
  }

  if (osapiWaitForTaskInit (L7_IPMAP_ARP_CALLBACK_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "%s: Unable to initialize ARP Callback task\n", routine_name);
    return L7_FAILURE;
  }
  queueLengths[IPM_ARP_EXP_EVENT_QUEUE] = platRtrArpMaxEntriesGet();
  /* create ARP TimerExp task */
  if (XX_CreateThreadMQueue(255, NULL, FALSE,
                      (t_Handle *) &ipMapArpCtx_g.timerExp.taskId,
                       IPM_ARP_NUM_QUEUES, queueLengths, NULL,"ARP Timer") != E_OK)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "%s: Failed to create ARP Timer Expire task\n", routine_name);
    return L7_FAILURE;
  }

  if (ipmArpTaskCreate() != L7_SUCCESS)
  {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
              "%s: Unable to start task to process ARP requests\n", routine_name);
      return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Kill ARP task
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapArpDeleteTasks(void)
{
  osapiTaskDelete(ipMapArpCtx_g.arpQueryTaskId);
  return ipmArpTaskDelete();
}


/*********************************************************************
* @purpose  Used to call routingStart() to initialize the routing
*           infrastructure. That call has been moved to sysapiIfNetInit(),
*           which is done prior to phase 1 init.
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ipMapArpL7Start(void)
{
    ipMapRoutingStarted = L7_TRUE;
    return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Common send callback function used by vendor ARP code for
*           address resolution
*
* @param    userId      ptr to interface instance structure
* @param    *pFrame     ptr to Ethernet frame to be transmitted
* @param    flags       contains the interface index number to send the frame
* @param    reservHandle  resource reservation handle for packet data flow
*
* @returns  E_OK
* @returns  E_xxxxxx    anything other than E_OK (see std.h for list)
*
* @notes    This function must copy the frame contents from the pFrame
*           format to an Mblk format used by the routing END object.
*
* @notes    This function assumes the ARP lock is taken
*
* @end
*********************************************************************/
e_Err ipMapArpSendCallback(t_Handle userId, void *pFrame, word flags,
                           t_Handle reservHandle)
{
  L7_uint32         frameSize, sendBufSize;
  ipMapArpIntf_t    *pIntf;
  e_Err             erc = E_OK;
  L7_netBufHandle   bufHandle;
  L7_uchar8         *pDataBuf;

  /* -------------------------------------------------------------------------
   * NOTE:  DO NOT USE THE reservHandle PARM HERE!  Although the ARP
   *        code specifies it in its prototype for this function, all callers
   *        are only passing the first three parameters.  Thus, the value
   *        of reservHandle is indeterminate.
   * -------------------------------------------------------------------------
   */

  do
  {
    frameSize = (L7_uint32)F_HowManyBytes(pFrame);
    if ((frameSize < IPM_ARP_ENET_FRAME_LEN_MIN) ||
        (frameSize > IPM_ARP_ENET_FRAME_LEN_MAX))
    {
      erc = E_INVALID_FRAME;
      break;
    }

    pIntf = (ipMapArpIntf_t *)userId;

    /* check the frame size against the known interface send buffer size */
    sendBufSize = pIntf->sendBufSize;
    if (sendBufSize < frameSize)
    {
      IPM_ARP_PRT(IPM_ARP_MSGLVL_MED,
                  "IPM_ARP: frame size %d but send buff size %d, intf=%d\n",
                  frameSize, sendBufSize, pIntf->index);
      erc = E_INVALID_FRAME;
      break;
    }

    /* verify flags matches this interface instance */
    if (pIntf->index != flags)
    {
      erc = E_BADPARM;
      break;
    }

    /* get a network buffer from the system and copy in the ARP frame */
    SYSAPI_NET_MBUF_GET_NOLOG(bufHandle);
    if (bufHandle == L7_NULL)
    {
      /* sysapi counts the failures. ARP should retry. Forge ahead. */
      break;
    }
    SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, pDataBuf);
    if (F_Read(pDataBuf, pFrame, 0, (word)frameSize) != (word)frameSize)
    {
        IPM_ARP_PRT(IPM_ARP_MSGLVL_MED,
                    "IPM_ARP: ARP Send could not copy frame contents to msg block, "
                    "pFrame=0x%8.8x intf=%d\n",
                    (L7_uint32)pFrame, pIntf->index);
        SYSAPI_NET_MBUF_FREE(bufHandle);
        break;
    }
    SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, frameSize);

    /* invoke the interface raw send function (it now owns sysnet buffer) */
    if (pIntf->rawSendFn != L7_NULLPTR)
      (void)pIntf->rawSendFn(pIntf->intIfNum, bufHandle);
    else
        SYSAPI_NET_MBUF_FREE(bufHandle);

    arpDebugPacketTxTrace(pIntf->intIfNum,(t_ARPFrame *)((pDataBuf) + sizeof(t_EthHeader)));

  } while (0);    /* one-pass loop (for error exit control) */

  /* always delete the frame object upon exit */
  F_Delete(pFrame);
  return erc;
}


/*********************************************************************
* @purpose  Common address resolution callback function. Called when ARP
*           needs to send an asynchronous response to the entity that invoked
*           the ARP Request. This means an ARP Request was sent and either
*           there was no response (call from _destroyAddrEntry()) or we
*           did get a response (call from _updateARPCache()).
*
* @param    userId      original requestor's callback information)
* @param    arpInfo     ARP resolution response information (i.e., MAC addr)
*
* @returns  E_OK
*
* @notes    Invokes original requestor's callback function with its
*           own callback parameters.  This information was saved in a
*           context block that was allocated from the heap and is pointed
*           to by the userId parm.
*
* @notes    Assumes an unsuccessful resolution attempt is denoted by a
*           dllHeaderSize of 0 in the arpInfo structure.
*
* @notes    Assumes the ARP lock has been taken
*
*
* @end
*********************************************************************/
e_Err ipMapArpResCallbackInvoke(t_Handle userId, t_ARPResInfo *arpInfo)
{
  L7_RC_t           rc;
  ipMapArpResCallbackParms_t  *pCbInfo;
  L7_uchar8         macAddrBuf[L7_MAC_ADDR_LEN];
  ipMapArpCbkMsg_t  msg;

  rc = (arpInfo->dllHeaderSize == 0) ? L7_FAILURE : L7_SUCCESS;
  pCbInfo = (ipMapArpResCallbackParms_t *)userId;

  /* Normal case where original resolution request was queued.  Must
   * use our own MAC address buffer for the callback.
   */
  if (rc == L7_SUCCESS)
    memcpy(macAddrBuf, arpInfo->dllHeader, L7_MAC_ADDR_LEN);
  else
    memset(macAddrBuf, 0, L7_MAC_ADDR_LEN);

  /* invoke the original requestor's callback function */
  if(pCbInfo->pCallbackFn != L7_NULLPTR)
  {
    /* send message to ARP Callback task to process on own thread */
    memset((L7_uchar8 *)&msg, 0, sizeof(msg));
    msg.id = IPM_ARP_RES;
    msg.u.res.pCallbackFn = pCbInfo->pCallbackFn;
    msg.u.res.cbParm1 = pCbInfo->cbParm1;
    msg.u.res.cbParm2 = pCbInfo->cbParm2;
    memcpy(msg.u.res.macAddr, macAddrBuf, (size_t)L7_MAC_ADDR_LEN);
    msg.u.res.asyncRc = rc;

    rc = osapiMessageSend(ipMapArpCtx_g.callback.pMsgQ, (void *)&msg,
                          (L7_uint32)sizeof(msg),
                          L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
    if (rc != L7_SUCCESS)
    {
      /* Since cbParm1 points to an mbuf, this causes an mbuf leak. Can't free here,
       * though because we don't know the extact type of cbParm1. It's an mbuf in one
       * case (l3endSend()) and an rtmbuf in another (rtInIPForwardArpTable). Really
       * should not be holding mbuf while ARP resolution is pending and callbacks
       * are queued. */
      /* only emit a log message the first time a queue overflow occurs */
      if (ipMapArpCtx_g.callback.fullQCt == 0)
      {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
                "%s: Message queue overflow (one or more occurrences)", __FUNCTION__);
      }
      /* task msg queue must be full -- record via debug error counter */
      ipMapArpCtx_g.callback.fullQCt++;
    }
  }

  /* free the callback parms struct */
  XX_Free(pCbInfo);

  return E_OK;
}


/*********************************************************************
* @purpose  Apply ARP IP Table events to the device ARP table
*
* @param    *pIp        ARP IP entry node ptr
* @param    event       ARP table event (e.g., insert, update, remove)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This function occurs in conjunction with the ARP table operation
*           and not as part of a callback.  This means the ARP entry node
*           info is intact, even in the case of an entry remove.
*
* @notes    This function assumes the ARP lock is taken
*
* @end
*********************************************************************/
L7_RC_t ipMapArpDeviceTableApply(ipMapArpIpNode_t *pIp,
                                 ipMapArpTableEvent_t event)
{
  L7_uchar8             nullMac[L7_MAC_ADDR_LEN] = {0,0,0,0,0,0};
  L7_RC_t               rc;
  ipMapArpTableEvent_t  deviceEvent;
  L7_BOOL               inDeviceSaved;
  ipMapArpTrace_t       trcData;
  ipMapArpIntf_t        *pIntf;
  L7_arpEntry_t         arpEntry;
  L7_uint32             ipAddr, vlanId;
  ipMapArpGwNode_t      *pGw = L7_NULLPTR;
  ipMapArpReissueMsg_t  msg;
  L7_uint32             tmp;
  L7_uchar8             *tmpip;

  /* init some local values */
  rc = L7_SUCCESS;
  deviceEvent = event;                  /* start out the same */
  inDeviceSaved = pIp->inDevice;        /* save for trace data below */
  memset((L7_uchar8 *)&trcData, 0, sizeof(trcData));

  /* set ptr to this intf instance info */
  pIntf = ipMapArpIntfInUse_g[(L7_uint32)pIp->arpEntry.intfNum];

  if(!pIntf)
    return L7_FAILURE;

  /* set up an ARP entry structure from the callback info */
  memset(&arpEntry, 0, sizeof(arpEntry));

  /* PTIn Modified */
  tmpip = pIp->arpEntry.ipAddr;
  tmp = (tmpip[0] << 24) | (tmpip[1] << 16) | (tmpip[2] << 8) | (tmpip[3]);
  //ipAddr = (L7_uint32) osapiNtohl(tmp);
  ipAddr = tmp;

  if(ipMapVlanRtrIntIfNumToVlanId(pIntf->intIfNum, &vlanId) != L7_SUCCESS)
    vlanId = 0;

  ipMapArpDtlEntryBuild(&arpEntry, &ipAddr, &pIp->arpEntry.macAddr[0],
                        vlanId, pIntf->intIfNum);

  /* update the arpEntry flag for per the entry type */
  if (pIp->arpEntry.entryType == ARP_ENTRY_TYPE_LOCAL)
    arpEntry.flags |= L7_ARP_LOCAL;
  else if (pIp->arpEntry.entryType == ARP_ENTRY_TYPE_GATEWAY)
    arpEntry.flags |= L7_ARP_GATEWAY;

  if (pIntf->unnumbered)
    arpEntry.flags |= L7_ARP_UNNUMBERED;

  /* evaluate various flags and other conditions in light of the specified
   * event, overriding where necessary to derive the appropriate device
   * action (this evaluation may result in not taking any action at this time)
   */
  switch (event)
  {
  case IPM_ARP_TABLE_INSERT:
  case IPM_ARP_TABLE_REISSUE:
    /* take no action if entry already in device, or if entry MAC addr is
     *  unresolved (i.e., null)
     */
    if (pIp->inDevice == L7_TRUE)
      deviceEvent = IPM_ARP_TABLE_NO_ACTION;
    else if (memcmp(pIp->arpEntry.macAddr, nullMac, L7_MAC_ADDR_LEN) == 0)
      deviceEvent = IPM_ARP_TABLE_NO_ACTION;
    break;

  case IPM_ARP_TABLE_UPDATE:
    /* insert the entry if not already in device; remove it if already in
     * the device but now the MAC addr is unresolved (unless this is a gateway
     * holdover condition, then just update the device with the now unresolved
     * entry)
     */
    if (pIp->inDevice != L7_TRUE)
    {
      if( (pGw) && (pGw->holdover == L7_TRUE) )
        deviceEvent = IPM_ARP_TABLE_UPDATE;
      else
        deviceEvent = IPM_ARP_TABLE_INSERT;
    }
    else if (memcmp(pIp->arpEntry.macAddr, nullMac, L7_MAC_ADDR_LEN) == 0)
    {
      deviceEvent = IPM_ARP_TABLE_REMOVE;
    }
    break;

  case IPM_ARP_TABLE_REMOVE:
    /* take no action if entry is not in device; for a gateway holdover
     * condition, change the remove action to an update instead (the DTL flags
     * indicate the entry is now unresolved)
     */
    if (pIp->inDevice != L7_TRUE)
      deviceEvent = IPM_ARP_TABLE_NO_ACTION;
    break;

  default:
    break;

  } /* endswitch event */

  /* use the derived deviceEvent to perform the appropriate device ARP table
   * action
   */
  switch (deviceEvent)
  {
  case IPM_ARP_TABLE_INSERT:
  case IPM_ARP_TABLE_REISSUE:
    if (ipMapArpEntryAdd(&arpEntry) == L7_SUCCESS)
    {
      pIp->inDevice = L7_TRUE;

      /* Since the ARP has been added, reset Gw holdover (if any) */
      if(pGw)
        pGw->holdover = L7_FALSE;

      /* Notify interested parties on successful insert */
      ipMapArpCacheEventNotify(IP_MAP_ARP_CACHE_INSERT, &arpEntry);
    }
    else
    {
      /* this insert/reissue attempt failed, so enqueue a message to the
       * ARP Reissue Task if the allowed upper limit of 'add' attempts
       * has not yet been reached
       *
       * NOTE:  This message won't be processed until the next new
       *        invocation of the task (i.e., this same IP address will
       *        not be reprocessed during the current task interation).
       */
      pIp->failedDevAdds++;

      if (pIp->failedDevAdds < IPM_ARP_REISSUE_DEV_FAIL_MAX)
      {
        memset((L7_uchar8 *)&msg, 0, sizeof(msg));
        msg.ipAddr = ipAddr;
        msg.intIfNum = pIntf->intIfNum;

        (void)ipMapArpReissueTaskMsgSend(&msg); /* let called fn handle errors*/
      }
      else
      {
        /* all reissue attempts failed -- record via debug error counter */
        ipMapArpCtx_g.reissue.failCt++;
      }
    }
    break;

  case IPM_ARP_TABLE_UPDATE:
    if (ipMapArpEntryModify(&arpEntry) != L7_SUCCESS)
    {
      rc = L7_FAILURE;
    }
    else
    {
      /* If the ARP has been set, reset Gw holdover flag (if any) */
      if( (pGw) && (memcmp((L7_uchar8*)&arpEntry.macAddr.addr.enetAddr, nullMac, L7_MAC_ADDR_LEN) != 0))
      {
        pGw->holdover = L7_FALSE;
        pIp->inDevice = L7_TRUE;
      }

      /* Notify interested parties on successful update */
      ipMapArpCacheEventNotify(IP_MAP_ARP_CACHE_UPDATE, &arpEntry);
    }

    break;

  case IPM_ARP_TABLE_REMOVE:
    if (ipMapArpEntryDelete(&arpEntry) == L7_SUCCESS)
    {
      pIp->inDevice = L7_FALSE;

      /* Notify interested parties on successful remove */
      ipMapArpCacheEventNotify(IP_MAP_ARP_CACHE_REMOVE, &arpEntry);
    }
    else
    {
      rc = L7_FAILURE;
    }
    break;

  case IPM_ARP_TABLE_NO_ACTION:
    /* special internal event value -- do nothing to device */
    break;

  default:
    rc = L7_FAILURE;
    break;

  } /* endswitch deviceEvent */

  /* trace this ARP event */
  trcData.event = event;
  trcData.deviceEvent = deviceEvent;
  trcData.ipAddr = (L7_uint32)arpEntry.ipAddr;
  trcData.intIfNum = arpEntry.intIfNum;
  trcData.flags = arpEntry.flags;
  trcData.inDeviceBefore = inDeviceSaved;
  trcData.inDeviceAfter = pIp->inDevice;
  trcData.failedDevAdds = pIp->failedDevAdds;
  ipMapArpTraceWrite(&trcData);

  return rc;
}


/*********************************************************************
* @purpose  Common function to send message to ARP Reissue task queue
*
* @param    *msg        ptr to ARP Reissue task message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This function assumes the ARP lock is taken
*
* @end
*********************************************************************/
L7_RC_t ipMapArpReissueTaskMsgSend(ipMapArpReissueMsg_t *msg)
{
  static const char *routine_name = "ipMapArpReissueTaskMsgSend";
  L7_RC_t           rc = L7_SUCCESS;

  rc = osapiMessageSend(ipMapArpCtx_g.reissue.pMsgQ, (void *)msg,
                        (L7_uint32)sizeof(*msg),
                        L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    /* only emit a log message the first time a queue overflow occurs */
    if (ipMapArpCtx_g.reissue.fullQCt == 0)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
              "%s: Message queue overflow (one or more occurrences)\n",
              routine_name);
    }
    /* task msg queue must be full -- record via debug error counter */
    ipMapArpCtx_g.reissue.fullQCt++;
  }

  return rc;
}


/*********************************************************************
* @purpose  ARP Reissue Task
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Tries to reissue a device ARP add for entries that had
*           previously failed during the DTL call.
*
* @end
*********************************************************************/
void ipMapArpReissueTask(void)
{
  static const char     *routine_name = "ipMapArpReissueTask";
  static L7_uint32      msgServiceCount = 0;  /* max sequential msgs to service */
  static L7_uint32      currSeqNum = 0;       /* incremented once per pass */
  L7_uint32             numMsgs, numToProcess;
  ipMapArpReissueMsg_t  msg;
  ipMapArpIpNode_t      ip;


  /* create task message queue */
  ipMapArpCtx_g.reissue.pMsgQ =
    osapiMsgQueueCreate(IPM_ARP_REISSUE_MSG_Q_NAME,
                        (L7_uint32)FD_CNFGR_ARP_REISSUE_MSG_Q_COUNT,
                        (L7_uint32)sizeof(ipMapArpReissueMsg_t));

  if (ipMapArpCtx_g.reissue.pMsgQ == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "%s: Unable to create ARP Map Reissue Task msg queue\n", routine_name);
    L7_assert(1);
    return;
  }

  (void)osapiTaskInitDone(L7_IPMAP_ARP_REISSUE_TASK_SYNC);


  /* main task body */
  while (1)
  {
    /* per-pass initialization */
    currSeqNum++;
    msgServiceCount = 0;

    /* get current number of messages on queue (only that many are processed
     * per pass)
     */
    if (osapiMsgQueueGetNumMsgs(ipMapArpCtx_g.reissue.pMsgQ, (L7_int32 *)&numMsgs) != L7_SUCCESS)
      numMsgs = 0;

    /* update debug counters */
    ipMapArpCtx_g.reissue.totalCt += numMsgs;
    if (numMsgs > ipMapArpCtx_g.reissue.peakCt)
      ipMapArpCtx_g.reissue.peakCt = numMsgs;

    /* set up variable to control inner while loop iterations */
    numToProcess = numMsgs;

    /* Only process the current number of messages in the queue.  As this
     * pass is executed, additional retry requests may be queued, but these
     * should not be processed until the next time the task runs.
     */
    while (numToProcess > 0)
    {
      /* check for a message in the msg queue */
      memset((L7_uchar8 *)&msg, 0, sizeof(msg));
      if (osapiMessageReceive(ipMapArpCtx_g.reissue.pMsgQ,
                              &msg, (L7_uint32)sizeof(msg),
                              L7_NO_WAIT) != L7_SUCCESS)
      {
        break;                          /* queue is empty */
      }

      numToProcess--;

      /* attempt to reissue the ARP entry add to the device ARP table
       * (this is a no-op if entry no longer exists, or is already
       * in the device)
       *
       * NOTE: All error conditions are handled by the called function.
       */
      memset((L7_uchar8 *)&ip, 0, sizeof(ip));
      ip.key = msg.ipAddr;
      ip.intIfNum = msg.intIfNum;

      IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);

      (void)ipMapArpIpTableUpdate(&ip, IPM_ARP_TABLE_REISSUE);

      IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);

      /* limit the number of msgs to service before yielding to other tasks */
      if (++msgServiceCount >= FD_CNFGR_ARP_REISSUE_MSG_SERVICE_MAX)
      {
        msgServiceCount = 0;
        (void)osapiTaskYield();
      }

    } /* endwhile numToProcess > 0 */

    IPM_ARP_PRT(IPM_ARP_MSGLVL_LO_2,
                "ARP Reissue task: seq=%u msgs=%d peak=%u tot=%u\n",
                currSeqNum, numMsgs, ipMapArpCtx_g.reissue.peakCt,
                ipMapArpCtx_g.reissue.totalCt);

    osapiSleep(FD_CNFGR_ARP_REISSUE_SLEEP_TIME);

  } /* endwhile main task body */

}


/*********************************************************************
* @purpose  ARP Callback Task
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Handles work deferred to the ARP callback task.
*
* @end
*********************************************************************/
void ipMapArpCallbackTask(void)
{
  static const char *routine_name = "ipMapArpCallbackTask";
  L7_uint32         msgServiceCount = 0;  /* max sequential msgs to service */
  L7_uint32         numMsgs;
  ipMapArpCbkMsg_t  msg;
  L7_uint32         status;
  ipMapArpCbkResMsg_t   *pMsgRes;


  /* create task message queue */
  ipMapArpCtx_g.callback.pMsgQ =
    osapiMsgQueueCreate(IPM_ARP_CALLBACK_MSG_Q_NAME,
                        (L7_uint32)FD_CNFGR_ARP_CALLBACK_MSG_Q_COUNT,
                        (L7_uint32)sizeof(ipMapArpCbkMsg_t));

  if (ipMapArpCtx_g.callback.pMsgQ == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "%s: Unable to create ARP Map Callback Task msg queue\n", routine_name);
    L7_assert(1);
    return;
  }

  (void)osapiTaskInitDone(L7_IPMAP_ARP_CALLBACK_TASK_SYNC);


  /* main task body */
  while (1)
  {
    /* wait for a message from the queue */
    memset((L7_uchar8 *)&msg, 0, sizeof(msg));
    status = osapiMessageReceive(ipMapArpCtx_g.callback.pMsgQ,
                                 &msg, (L7_uint32)sizeof(msg),
                                 L7_WAIT_FOREVER);

    /* get current queue depth (for debug info) */
    if (osapiMsgQueueGetNumMsgs(ipMapArpCtx_g.callback.pMsgQ, (L7_int32 *)&numMsgs) != L7_SUCCESS)
      numMsgs = 0;
    else
      numMsgs += 1;                     /* count msg just read from queue */
    if (numMsgs > ipMapArpCtx_g.callback.peakCt)
      ipMapArpCtx_g.callback.peakCt = numMsgs;

    /* process this message */
    if (status == L7_SUCCESS)
    {
      /* update debug counter */
      ipMapArpCtx_g.callback.totalCt++;

      switch (msg.id)
      {
      case IPM_ARP_RES:                 /* ARP resolution callback */
        pMsgRes = &msg.u.res;
        /* invoke the original requestor's callback function */
        pMsgRes->pCallbackFn(pMsgRes->cbParm1, pMsgRes->cbParm2,
                             pMsgRes->macAddr, pMsgRes->asyncRc);
        break;

      default:
        break;
      } /* endswitch */
    }
    else
    {
      /* message queue read error -- record via debug error counter */
      ipMapArpCtx_g.callback.failCt++;
    }

    /* limit the number of msgs to service before yielding to other tasks */
    if (++msgServiceCount >= FD_CNFGR_ARP_CALLBACK_MSG_SERVICE_MAX)
    {
      msgServiceCount = 0;
      (void)osapiTaskYield();
    }

  } /* endwhile main task body */

}

/*********************************************************************
* @purpose  Queries the ARP entry from the hardware
*
* @param    pArp @b{(input)} ARP Query Entry
*
* @returns  L7_SUCCESS  on successful query of arp entry
* @returns  L7_FAILURE  if the query fails
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapIpArpEntryQuery(L7_arpQuery_t *pArp)
{
  return dtlIpv4ArpEntryQuery(pArp);
}

/*********************************************************************
* @purpose  Determines if an aged-out ARP entry should be renewed
*
* @param    ipAddr      IP address of ARP entry being evaluated
* @param    intIfNum    Internal interface number of ARP entry
*
* @returns  L7_TRUE     keep ARP entry in the ARP table
* @returns  L7_FALSE    allow ARP entry to expire
*
* @notes    This function assumes the ARP lock is taken
*
* @end
*********************************************************************/
L7_BOOL ipMapArpEntryKeep(L7_uint32 ipAddr, L7_uint32 intIfNum)
{
  ipMapArpIpNode_t  *pIp;
  L7_arpQuery_t     arpQueryEntry;

  /* find ARP table entry */
  if (ipMapArpIpTableSearch(ipAddr, intIfNum, (L7_uint32)L7_MATCH_EXACT, &pIp) != L7_SUCCESS)
    return L7_FALSE;

  /* keep all entry types that are not dynamic */
  if (pIp->arpEntry.entryType != ARP_ENTRY_TYPE_DYNAMIC)
    return L7_TRUE;

  /* Algo:
   * When a dynamic ARP entry ages out, do the following:
   * (a) If the dynamic-renew-arp is configured, return TRUE.
   * (b) Else,
   *     Get the (hit-bit + last-hit-time) values on this ARP entry from h/w
   *     If the hit-bit is set, return TRUE.
   *     Else, return FALSE
   */

  /* only keep dynamic entries if the config parm is enabled */
  if (ipMapIpArpDynamicRenewGet() == L7_ENABLE)
    return L7_TRUE;

  memcpy((void *)&(arpQueryEntry.ipAddr),
         (void *)pIp->arpEntry.ipAddr, sizeof(pIp->arpEntry.ipAddr));
  arpQueryEntry.vlanId        = 0; /* ignored now */
  arpQueryEntry.intIfNum      = pIp->arpEntry.interfaceNumber;
  arpQueryEntry.arpQueryFlags = 0; /* set in the layers below */
  arpQueryEntry.lastHitTime   = pIp->arpEntry.lastHitTime;

  if (ipMapIpArpEntryQuery(&arpQueryEntry) != L7_SUCCESS)
    return L7_FALSE;

  /* store the updated last hit time in the ARP entry structure */
  pIp->arpEntry.lastHitTime   = arpQueryEntry.lastHitTime;

  if (arpQueryEntry.arpQueryFlags & L7_ARP_HIT_ANY)
    return L7_TRUE;

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Fills in an ARP entry structure in a format expected by DTL
*
* @param    pArpEntry   pointer to ARP entry structure to be filled in
* @param    pIpAddr     pointer to IP address
* @param    pMacAddr    pointer to Ethernet MAC address
* @param    vlanId      VLAN associated with the routing circuit
* @param    intIfNum    internal interface number for the routing circuit
*
* @returns  void
*
* @notes    Only a generic flags value is setup here.  All ARP entries
*           are assumed to be resolved here.  The caller must set any
*           additional flags, as needed.
*
* @notes    This function assumes the ARP lock is taken
*
* @end
*********************************************************************/
void ipMapArpDtlEntryBuild(L7_arpEntry_t *pArpEntry,
                           L7_uint32 *pIpAddr, L7_uchar8 *pMacAddr,
                           L7_uint32 vlanId, L7_uint32 intIfNum)
{
  memcpy(&pArpEntry->ipAddr, pIpAddr, L7_IP_ADDR_LEN);
  pArpEntry->macAddr.type = L7_LL_ETHERNET;
  pArpEntry->macAddr.len = L7_ENET_MAC_ADDR_LEN;
  memcpy(&pArpEntry->macAddr.addr.enetAddr, pMacAddr, L7_ENET_MAC_ADDR_LEN);
  pArpEntry->vlanId = (L7_ushort16)vlanId;
  pArpEntry->intIfNum = intIfNum;
  pArpEntry->hits = 0;
  pArpEntry->flags = L7_ARP_RESOLVED;
}


/*********************************************************************
* @purpose  Build the ARP entry structure flags from the IP table entry info
*
* @param    pIp         @b{(Input)}  pointer to ARP IP table entry node
* @param    pFlags      @b{(Output)} pointer to flags output location
*
* @returns  void
*
* @comments The ARP table does not contain any net directed broadcast entries,
*           so that flag is never set here.
*
* @notes    This function assumes the ARP lock is taken
*
* @end
*********************************************************************/
void ipMapArpEntryFlagsBuild(ipMapArpIpNode_t *pIp, L7_uint32 *pFlags)
{
  L7_uchar8     nullMac[L7_MAC_ADDR_LEN] = {0,0,0,0,0,0};
  L7_uint32     flags;

  if (pFlags == L7_NULLPTR)
    return;

  flags = 0;

  /* build the ARP flags field from the ARP table entry node info
   *
   * NOTE:  The ARP table does not contain any net directed broadcast entries
   */
  if (pIp != L7_NULLPTR)
  {
    switch (pIp->arpEntry.entryType)
    {
    case ARP_ENTRY_TYPE_LOCAL:
      flags |= L7_ARP_LOCAL;
      break;
    case ARP_ENTRY_TYPE_STATIC:
      flags |= L7_ARP_STATIC;
      break;
    case ARP_ENTRY_TYPE_GATEWAY:
      flags |= L7_ARP_GATEWAY;
      break;
    default:
      break;
    }

    /* consider an entry with a non-null MAC address as resolved */
    if (memcmp(pIp->arpEntry.macAddr, nullMac, (size_t)L7_MAC_ADDR_LEN) != 0)
      flags |= L7_ARP_RESOLVED;
  }

  *pFlags = flags;
}


/*********************************************************************
* @purpose  Converts an internal interface number to an ARP interface
*           instance
*
* @param    intIfNum    internal interface number
* @param    *ppIntf     ptr to output location
*                         (@b{Output:} pointer to corresponding interface
*                          instance)
*
* @returns  L7_SUCCESS  found matching interface instance
* @returns  L7_FAILURE  interface instance not found
*
* @notes    An internal interface number of 0 is normally not a valid
*           value, but is used in certain cases when an interface-independent
*           ARP value is involved (such as certain multicast addresses).
*           For this reason, an internal interface number of 0 is translated
*           to the special interface instance 0.
*
* @notes    The *ppIntf output location is set to L7_NULL for a failure,
*           indicating a matching interface instance was not found.
*
* @notes    This function assumes the ARP lock is taken
*
*
* @end
*********************************************************************/
L7_RC_t ipMapArpIntfInstanceGet(L7_uint32 intIfNum, ipMapArpIntf_t **ppIntf)
{
  ipMapArpIntf_t  *pIntf;
  L7_uint32     i;

  *ppIntf = L7_NULL;

  /* search through the in-use table looking for a matching internal interface
   * Note:  interface number 0 is handled naturally (see prolog note)
   */
  for (i = 0; i < (L7_uint32)IPM_ARP_INTF_MAX; i++)
  {
    pIntf = ipMapArpIntfInUse_g[i];
    if ((pIntf != L7_NULL) && (pIntf->intIfNum == intIfNum))
    {
      *ppIntf = pIntf;
      break;
    }
  }
  return (*ppIntf != L7_NULL) ? L7_SUCCESS : L7_FAILURE;
}

/*********************************************************************
* @purpose  Find the ARP interface configured with an IP subnet
*           that contains a given unicast IP address.
*
* @param    ipAddr      IP address of interest
* @param    ppIntf      handle to ARP interface
* @param    localAddr   Local IP address in matching subnet
* @param    localMask   Network mask on matching subnet
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Failure to find any matching interface instance results in
*           the *ppIntf output location being set to L7_NULL.
*
*           Since FASTPATH doesn't allow the operator to configure
*           different interfaces with the same or overlapping subnets,
*           there can only be one match.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpSubnetFind(L7_uint32 ipAddr, ipMapArpIntf_t **ppIntf,
                           L7_uint32 *localAddr, L7_uint32 *localMask)
{
  ipMapArpIntf_t  *pIntf;
  L7_uint32     i, j;

  *ppIntf = L7_NULL;

  /* Use ipMapArpNetidSearch() for mcast addrs */
  if (ipAddr >= (L7_uint32)L7_IP_MCAST_BASE_ADDR)
    return L7_FAILURE;

  /* search through the in-use table looking for a matching internal interface
   * Note:  interface number 0 is handled naturally (see prolog note)
   */
  for (i = 0; i < (L7_uint32)IPM_ARP_INTF_MAX; i++)
  {
    pIntf = ipMapArpIntfInUse_g[i];
    if (pIntf && pIntf->ipAddr && pIntf->netMask)
    {
      /* check primary IP address */
      if ((pIntf->ipAddr & pIntf->netMask) == (ipAddr & pIntf->netMask))
      {
        *ppIntf = pIntf;
        if (localAddr)
          *localAddr = pIntf->ipAddr;
        if (localMask)
          *localMask = pIntf->netMask;
        return L7_SUCCESS;
      }
      /* check secondary IP address */
      for (j = 0; j < L7_L3_NUM_SECONDARIES; j++)
      {
        if (pIntf->secondaries[j].ipAddr && pIntf->secondaries[j].ipMask)
        {
          if ((pIntf->secondaries[j].ipAddr & pIntf->secondaries[j].ipMask) ==
              (ipAddr & pIntf->secondaries[j].ipMask))
          {
            *ppIntf = pIntf;
            if (localAddr)
              *localAddr = pIntf->secondaries[j].ipAddr;
            if (localMask)
              *localMask = pIntf->secondaries[j].ipMask;
            return L7_SUCCESS;
          }
        }
      }
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Searches all router interface instances for a subnet which
*           matches the IP address parameter
*
* @param    ipAddr      IP address of interest
* @param    *ppIntf     ptr to output location
*                         (@b{Output:} matching router interface instance
*                          number, or L7_NULL if not found)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Must handle multicast IP (e.g. 224.x.x.x) addresses as a
*           special case by outputting interface instance 0.  The ARP
*           mapping layer and DTL are designed to treat this as a "system"
*           interface.
*
* @notes    Failure to find any matching interface instance results in
*           the *ppIntf output location being set to L7_NULL.
*
* @notes    This function assumes the ARP lock is taken
*
*
* @end
*********************************************************************/
L7_RC_t ipMapArpNetidSearch(L7_uint32 ipAddr, ipMapArpIntf_t **ppIntf)
{
    *ppIntf = L7_NULL;

    /* check for multicast IP addr and output an interface ID of 0 */
    if (ipAddr >= (L7_uint32)L7_IP_MCAST_BASE_ADDR)
    {
        *ppIntf = ipMapArpIntfInUse_g[0];   /* denotes "system" interface */
        return L7_SUCCESS;
    }

    return ipMapArpSubnetFind(ipAddr, ppIntf, NULL, NULL);
}

/*********************************************************************
* @purpose  Checks if the specified IP address matches the net directed
*           broadcast address of one of the IP addresses configured
*           on the given interface.
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    ipAddr      @b{(input)} IP address to check
* @param    pMacAddr    @b{(output)} Pointer to a buffer to hold the Net
*                       Directed Broadcast MAC Addresses if a match is
*                       found
*
* @returns  L7_SUCCESS  Match found
* @returns  L7_FAILURE  Could not find a match
* @returns  L7_ERROR    Invalid buffer pointer
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t  ipMapArpNetDirBcastMatchCheck(L7_uint32 intIfNum, L7_IP_ADDR_t ipAddr,
                                       L7_uchar8 *pMacAddr)
{
    L7_uint32 netDirBcastAddr = 0;
    L7_uint32   j;
  ipMapArpIntf_t *pIntf = NULL;

  if ((pMacAddr == L7_NULLPTR) || (ipAddr == 0))
    {
        return L7_ERROR;
    }

  /* get ARP interface object */
  if (ipMapArpIntfInstanceGet(intIfNum, &pIntf) != L7_SUCCESS)
    return L7_FAILURE;

  /* Check primary address */
  netDirBcastAddr = (pIntf->ipAddr & pIntf->netMask) | ~(pIntf->netMask);
  if (ipAddr == netDirBcastAddr)
  {
    memcpy(pMacAddr, ipMapArpBcastMac, (size_t)L7_MAC_ADDR_LEN);
    return L7_SUCCESS;
  }

  /* check secondaries */
  for (j = 0; j < L7_L3_NUM_SECONDARIES; j++)
  {
    /* skip 0/0 entries in secondaries list */
    if (pIntf->secondaries[j].ipAddr)
    {
      netDirBcastAddr =
        (pIntf->secondaries[j].ipAddr & pIntf->secondaries[j].ipMask) |
        ~(pIntf->secondaries[j].ipMask);

      if (ipAddr == netDirBcastAddr)
      {
        memcpy(pMacAddr, ipMapArpBcastMac, (size_t)L7_MAC_ADDR_LEN);
        return L7_SUCCESS;
      }
    }
  }
  return L7_FAILURE;
}


/*********************************************************************
* @purpose  Dispatch function for ARP Cache updates
*
* @param    event     @{(input)} Type of event
* @param    pArpEntry @{(input)} ARP Entry
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void ipMapArpCacheEventNotify(ipMapArpCacheEvent_t event,
                              L7_arpEntry_t *pArpEntry)
{
  L7_uint32 i;
  ipMapArpCacheCallback_ft func;

  for (i = 0; i < IPM_ARP_CACHE_REG_MAX; i++)
  {
    func = ipMapArpCtx_g.arpCacheCallback[i];
    if (func != L7_NULL)
    {
      (*func)(event, pArpEntry);
    }
  }
}


/**************************************************************************
* @purpose  Take a semaphore and check return code
*
* @param    pLock   @{(input)} Lock control info ptr (references semaphore ID)
* @param    timeout @{(input)} Time to wait (in ticks), or L7_WAIT_FOREVER
*                                or L7_NO_WAIT
* @param    file    @{(input)} File name of caller (for error logging)
* @param    line    @{(input)} File line number of caller (for error logging)
*
* @returns  void
*
* @notes    Wrapper function for OSAPI call, so that errors can be checked
*           in one place.  The assumption here is that there are no "expected"
*           errors (i.e., the semaphore id is legitimate and the caller
*           typically waits forever), so any error gets logged.
*
* @notes    DO NOT USE if specifying a timeout other than L7_WAIT_FOREVER
*           where awareness of the time expiration is needed.  Make a direct
*           call to osapiSemaTake() instead.
*
* @end
*************************************************************************/
void ipMapArpSemaTake(ipMapArpLockCtrl_t *pLock, L7_int32 timeout,
                      char *file, L7_ulong32 line)
{
  L7_int32      taskId = osapiTaskIdSelf();
  L7_RC_t       rc;

  if(timeout == L7_WAIT_FOREVER)
    timeout = arpLockDebugTimeout;

  l7utilsFilenameStrip(&file);

  if ((rc = osapiSemaTake(pLock->semId, arpLockDebugTimeout)) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "Semaphore take failure for file %s:%lu, rc=%u, id=0x%8.8x\n",
            file, line, rc, (L7_uint32)pLock->semId);
    ARP_LOCK_DEBUG_DUMP(pLock->semId, file, line);
  }

  pLock->ownerTaskId = taskId;
  pLock->ownerLockCount++;
  pLock->lockTotal++;                 /* used for debug show function */

  if(timeout != L7_WAIT_FOREVER)
  {
    pLock->ownerLine = line;
    strncpy(pLock->ownerFile, file, ARP_LOCK_OWNER_LEN);
  }

  ARP_LOCK_DEBUG_LOG(file, line, pLock->semId);
}

/**************************************************************************
* @purpose  Give a semaphore and check return code
*
* @param    pLock   @{(input)} Lock control info ptr (references semaphore ID)
* @param    file    @{(input)} File name of caller (for error logging)
* @param    line    @{(input)} File line number of caller (for error logging)
*
* @returns  void
*
* @notes    Wrapper function for OSAPI call, so that errors can be checked
*           in one place.  The assumption here is that there are no "expected"
*           errors (i.e., the semaphore id is legitimate) so any error gets
*           logged.
*
* @end
*************************************************************************/
void ipMapArpSemaGive(ipMapArpLockCtrl_t *pLock, char *file, L7_ulong32 line)
{
  L7_RC_t       rc;

  pLock->unlockTotal++;                 /* used for debug show function */
  pLock->ownerLockCount--;
  pLock->ownerTaskId = 0;

  pLock->ownerLine = 0;
  pLock->ownerFile[0] = 0;

  l7utilsFilenameStrip(&file);

  if ((rc = osapiSemaGive(pLock->semId)) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "Semaphore give failure for file %s:%lu, rc=%u, id=0x%8.8x\n",
            file, line, rc, (L7_uint32)pLock->semId);
  }

  ARP_LOCK_DEBUG_LOG(file, line, pLock->semId);
}

/*********************************************************************
* @purpose  Checks if a MAC address value is all zeros
*
* @param    pMac        ptr to Ethernet MAC address
*
* @returns  L7_TRUE     MAC address is all zeros
* @returns  L7_FALSE    MAC address is non-zero
*
* @notes    Checks byte by byte to avoid any alignment restrictions
*           on certain CPUs.
*
* @end
*********************************************************************/
L7_BOOL ipMapArpIsMacZero(L7_uchar8 *pMac)
{
  L7_uint32     i;

  for (i = 0; i < L7_MAC_ADDR_LEN; i++)
  {
    if (pMac[i] != 0)
      return L7_FALSE;
  }
  return L7_TRUE;
}

/*********************************************************************
* @purpose  Sets the level of IPM ARP debugging messages to display
*
* @param    msgLvl      message level (0 = off, >0 on)
*
* @returns  void
*
* @notes    Messages are defined according to interest level.  Higher
*           values generally display more debug messages.
*
* @end
*********************************************************************/
void ipMapArpMsgLvlSet(L7_uint32 msgLvl)
{
  ipMapArpCtx_g.msgLvl = msgLvl;
}


/*********************************************************************
* @purpose  Records an ARP event entry in the ARP trace buffer
*
* @param    *pTrcData   ptr to trace point data
*
* @returns  void
*
* @notes    The ARP trace buffer is circular.  There is no semaphore
*           protection at present (can be added later, if needed).
*
* @end
*********************************************************************/
void ipMapArpTraceWrite(ipMapArpTrace_t *pTrcData)
{
  ipMapArpTrace_t *pTrcBuf;

  pTrcBuf = &ipMapArpTrace_g[ipMapArpTraceIdx_g];

  *pTrcBuf = *pTrcData;

  /* set the timestamp field here */
  pTrcBuf->timestamp = osapiTimeMillisecondsGet();

  if (++ipMapArpTraceIdx_g >= IPM_ARP_TRACE_MAX)
    ipMapArpTraceIdx_g = 0;
}


/*********************************************************************
* @purpose  Displays the specified number of most recent ARP trace buffer
*           entries
*
* @param    amt         number of ARP trace buffer entries to display
*
* @returns  void
*
* @notes    Entering a 0 (or an out-of-range value) for the amt parameter
*           causes the entire ARP trace to be displayed.
*
* @end
*********************************************************************/
void ipMapArpTraceShow(L7_uint32 amt)
{
  ipMapArpTrace_t   trc[IPM_ARP_TRACE_MAX], *pTrc;
  L7_uint32         count, idx;
  L7_uchar8         ipStr[OSAPI_INET_NTOA_BUF_SIZE];
  char              *pEventStr, *pDevEventStr;

  memcpy(trc, ipMapArpTrace_g, sizeof(ipMapArpTrace_g));
  idx = ipMapArpTraceIdx_g;
  count = IPM_ARP_TRACE_MAX;
  if ((amt == 0) || (amt > count))
    amt = count;

  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "\n                                                               inDev fail \n");
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON,   "idx  timestmp   event  devEvnt     ipaddr      intf    flags    B A  count\n");
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON,   "--- ---------- ------- ------- --------------- ---- ---------- ----- -----\n");

  while (count--)
  {
    if (amt > count)
    {
      pTrc = &trc[idx];

      switch (pTrc->event)
      {
      case IPM_ARP_TABLE_INSERT:  pEventStr = "Insert"; break;
      case IPM_ARP_TABLE_REISSUE: pEventStr = "Reissue"; break;
      case IPM_ARP_TABLE_UPDATE:  pEventStr = "Update"; break;
      case IPM_ARP_TABLE_REMOVE:  pEventStr = "Remove"; break;
      default:                    pEventStr = "???"; break;
      }

      switch (pTrc->deviceEvent)
      {
      case IPM_ARP_TABLE_INSERT:    pDevEventStr = "Insert"; break;
      case IPM_ARP_TABLE_REISSUE:   pDevEventStr = "Reissue"; break;
      case IPM_ARP_TABLE_UPDATE:    pDevEventStr = "Update"; break;
      case IPM_ARP_TABLE_REMOVE:    pDevEventStr = "Remove"; break;
      case IPM_ARP_TABLE_NO_ACTION: pDevEventStr = "No-op "; break;
      default:                      pDevEventStr = "???"; break;
      }

      osapiInetNtoa(pTrc->ipAddr, ipStr);

      IPM_ARP_PRT(IPM_ARP_MSGLVL_ON,
                  "%3u %10.10u %-7s %-7s %-15s %4u 0x%8.8x  %1s %1s   %2u  \n",
                  idx, pTrc->timestamp, pEventStr, pDevEventStr, ipStr,
                  pTrc->intIfNum, pTrc->flags,
                  (pTrc->inDeviceBefore == L7_TRUE) ? "Y" : "N",
                  (pTrc->inDeviceAfter == L7_TRUE) ? "Y" : "N",
                  pTrc->failedDevAdds);
    }

    if (++idx >= IPM_ARP_TRACE_MAX)
      idx = 0;

  } /* endwhile */

  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "\n\n");
}


/*********************************************************************
* @purpose  Displays ARP lock information
*
* @param    void
*
* @returns  void
*
* @notes    Intended for engineering debug use only.
*
* @end
*********************************************************************/
void ipMapArpLockShow(void)
{
  ipMapArpLockCtrl_t  *pLock = &ipMapArpCtx_g.arpLock;

  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "  ARP Component Lock:\n");
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "    lock info      semId=0x%x  owner task=0x%x  owner lock count=%ld  lkTot=%lu  unlkTot=%lu\n",
        (L7_uint32)pLock->semId, (L7_uint32)pLock->ownerTaskId, pLock->ownerLockCount,
        pLock->lockTotal, pLock->unlockTotal);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "    owner at %s:%u\n", pLock->ownerFile, pLock->ownerLine);
}


/*********************************************************************
* @purpose  Displays the global ARP context info
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void ipMapArpCtxShow(void)
{
  ipMapArpCtx_t *pCtx = &ipMapArpCtx_g;
  ipMapArpLockCtrl_t  *pLock;

  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "\nIPMAP ARP Global Context Contents\n");

  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "  arpHandle       = 0x%8.8x\n",
              (L7_uint32)pCtx->arpHandle);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "  regUserId        = 0x%8.8x\n",
              (L7_uint32)pCtx->regUserId);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "  pIntfList        = 0x%8.8x\n",
              (L7_uint32)pCtx->pIntfList);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "  Reissue Task:\n");
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "    taskId         = 0x%8.8x\n",
              (L7_uint32)pCtx->reissue.taskId);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "    pMsgQ          = 0x%8.8x\n",
              (L7_uint32)pCtx->reissue.pMsgQ);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "    totalCt        = %u\n",
              (L7_uint32)pCtx->reissue.totalCt);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "    peakCt         = %u\n",
              (L7_uint32)pCtx->reissue.peakCt);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "    maxCt          = %u\n",
              (L7_uint32)pCtx->reissue.maxCt);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "    fullQCt        = %u\n",
              (L7_uint32)pCtx->reissue.fullQCt);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "    failCt         = %u\n",
              (L7_uint32)pCtx->reissue.failCt);

  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "  Callback Task:\n");
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "    taskId         = 0x%8.8x\n",
              (L7_uint32)pCtx->callback.taskId);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "    pMsgQ          = 0x%8.8x\n",
              (L7_uint32)pCtx->callback.pMsgQ);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "    totalCt        = %u\n",
              (L7_uint32)pCtx->callback.totalCt);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "    peakCt         = %u\n",
              (L7_uint32)pCtx->callback.peakCt);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "    maxCt          = %u\n",
              (L7_uint32)pCtx->callback.maxCt);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "    fullQCt        = %u\n",
              (L7_uint32)pCtx->callback.fullQCt);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "    failCt         = %u\n",
              (L7_uint32)pCtx->callback.failCt);

  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "  Timer Expire (XX) Task:\n");
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "    taskId         = 0x%8.8x\n",
              (L7_uint32)pCtx->timerExp.taskId);

  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "  routingStarted      = %s\n",
              (ipMapRoutingStarted == L7_TRUE) ? "True" : "False");
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "  msgLvl           = %u\n",
              pCtx->msgLvl);

  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "  dynamicRenew     = %s\n",
              (ipMapIpArpDynamicRenewGet() == L7_ENABLE) ? "Enable" : "Disable");
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "  cacheCurrCt      = %u\n",
              pCtx->cacheCurrCt);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "  cachePeakCt      = %u\n",
              pCtx->cachePeakCt);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "  cacheMaxCt       = %u\n",
              pCtx->cacheMaxCt);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "  staticCurrCt     = %u\n",
              pCtx->staticCurrCt);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "  staticMaxCt      = %u\n",
              pCtx->staticMaxCt);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "  inSrcDiscard     = %u\n",
              pCtx->inSrcDiscard);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "  inTgtDiscard     = %u\n",
              pCtx->inTgtDiscard);

  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "  Gateway Table:\n");
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "    treeEntryMax   = %u\n",
              pCtx->gwTbl.treeEntryMax);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "    *treeHeap      = 0x%8.8x\n",
              (L7_uint32)pCtx->gwTbl.treeHeap);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "    treeHeapSize   = %u\n",
              pCtx->gwTbl.treeHeapSize);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "    *dataHeap      = 0x%8.8x\n",
              (L7_uint32)pCtx->gwTbl.dataHeap);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "    dataHeapSize   = %u\n",
              pCtx->gwTbl.dataHeapSize);

  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "  IP Lookup Table:\n");
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "    treeEntryMax   = %u\n",
              pCtx->ipTbl.treeEntryMax);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "    *treeHeap      = 0x%8.8x\n",
              (L7_uint32)pCtx->ipTbl.treeHeap);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "    treeHeapSize   = %u\n",
              pCtx->ipTbl.treeHeapSize);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "    *dataHeap      = 0x%8.8x\n",
              (L7_uint32)pCtx->ipTbl.dataHeap);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "    dataHeapSize   = %u\n",
              pCtx->ipTbl.dataHeapSize);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "\n");
  pLock = &pCtx->arpLock;
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "  ARP Component Lock:\n");
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "    lock info      semId=0x%x  owner task=0x%x  owner lock count=%ld  lkTot=%lu  unlkTot=%lu\n",
              (L7_uint32)pLock->semId, (L7_uint32)pLock->ownerTaskId, pLock->ownerLockCount,
              pLock->lockTotal, pLock->unlockTotal);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "    owner at %s:%u\n", pLock->ownerFile, pLock->ownerLine);

  ARP_ObjectInfoShow(ipMapArpCtx_g.arpHandle);
}

/*********************************************************************
* @purpose  Dumps all ARP-related debug information
*
* @param    amt         number of entries to display (in certain items)
*
* @returns  void
*
* @notes    This function intended for debugging use only.
*
* @end
*********************************************************************/
void ipMapArpAllShow(L7_uint32 amt)
{
  ipMapArpIpTableShow(IP_MAP_ARP_INTF_ALL, amt);
  ipMapArpIpAgeListShow(IP_MAP_ARP_INTF_ALL, amt);
  ipMapArpGwTableShow();
  ipMapArpCtxShow();
  ipMapArpTraceShow(amt);
}

/*********************************************************************
* @purpose  Internal function that attempts to resolve an IP address
*           to an Ethernet MAC address. Assumes IPM_ARP_SEMA_TAKE has
*           been invoked and IPM_ARP_SEMA_GIVE will be invoked by the
*           calling function.
*
* @param    intIfNum    internal interface number
* @param    ipAddr      layer 3 IP address to resolve
* @param    *pMacAddr   ptr to output buffer where MAC address is returned if
*                       ipAddr is already in the ARP cache
* @param    pCallbackFn address of callback function for asynchronous response
* @param    cbParm1     callback parameter 1
* @param    cbParm2     callback parameter 2
*
* @returns  L7_SUCCESS          address was resolved, MAC addr returned via pMacAddr
* @returns  L7_ASYNCH_RESPONSE  resolution in progress. will notify asynchronously
*                               through callback.
* @returns  L7_FAILURE          cannot resolve address
*
* @notes    An L7_ASYNCH_RESPONSE return code indicates the address was not found in
*           the ARP cache and is in the process of being resolved.  If the
*           pCallbackFn pointer is non-null, an asynchronous callback will be
*           made indicating the outcome of the resolution attempt.  If the
*           pCallbackFn is null, there will not be a notification should the
*           address get resolved.
*
* @notes    An L7_FAILURE can occur due to temporary resource constraints
*           (such as a full queue), or other programming/usage errors.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpAddrResolveInternal(L7_uint32 intIfNum, L7_uint32 ipAddr,
                                    L7_uchar8 *pMacAddr,
                                    ipMapArpResCallback_ft pCallbackFn,
                                    L7_uint32 cbParm1, L7_uint32 cbParm2)
{
  ipMapArpIntf_t  *pIntf;
  t_ARPResInfo  arpResInfo;
  ipMapArpResCallbackParms_t  *pCbInfo;
  L7_uint32 netOrderIpAddr = osapiHtonl(ipAddr);
  e_Err         erc;

  /* check MAC buffer pointer */
  if (pMacAddr == L7_NULL)
  {
    IPM_ARP_PRT(IPM_ARP_MSGLVL_MED,
                "IPM_ARP: MAC buffer ptr is null for addr resolve on intf %d\n",
                intIfNum);

    return L7_FAILURE;
  }

  /* check if resolving the net directed broadcast IP address
   *
   * NOTE:  The net dir bcast addr entries for each routing interface were
   *        never added to the application ARP table and therefore would
   *        not be found via a table search.
   */
  if(ipMapArpNetDirBcastMatchCheck(intIfNum, ipAddr, pMacAddr) == L7_SUCCESS)
  {
    return L7_SUCCESS;
  }

  /* obtain interface instance */
  if (ipMapArpIntfInstanceGet(intIfNum, &pIntf) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* set up user portion of ARP resolution info block */
  memset(&arpResInfo, 0, sizeof(t_ARPResInfo));
  memcpy(arpResInfo.destAddr.lay3Addr, &netOrderIpAddr, L7_IP_ADDR_LEN);
  arpResInfo.destAddr.protocolId = 0;   /* IPv4 */
  /* Just to be clear this is initialized to zero, do it explicitly here. If
   * resolution succeeds, this gets set in _updateArpCache(). */
  arpResInfo.dllHeaderSize = 0;
  arpResInfo.lanNmb = (t_LIH)pIntf->index;

  /* allocate structure to save caller's callback information */
  pCbInfo = (ipMapArpResCallbackParms_t *)XX_Malloc(sizeof(ipMapArpResCallbackParms_t));
  if (pCbInfo == 0)
  {
    IPM_ARP_PRT(IPM_ARP_MSGLVL_MED,
                "IPM_ARP: Unable to allocate callback struct for intf %d\n",
                intIfNum);
    return L7_FAILURE;
  }
  memset(pCbInfo, 0, sizeof(ipMapArpResCallbackParms_t));
  pCbInfo->pCallbackFn = pCallbackFn;
  pCbInfo->cbParm1 = cbParm1;
  pCbInfo->cbParm2 = cbParm2;
  pCbInfo->pMacBuffer = pMacAddr;
  pCbInfo->asyncRc = L7_FAILURE;

  /* invoke ARP resolution */
  erc = ipMapARPResolution(ipMapArpCtx_g.arpHandle, (t_Handle)pCbInfo, &arpResInfo);
  if (erc == E_BUSY)
  {
    /* ARP resolution is in progress. Zero the pointer to the MAC buffer
     * so async callback won't try to write to it. */
      pCbInfo->pMacBuffer = L7_NULLPTR;
      return L7_ASYNCH_RESPONSE;
  }

  /* any other return code indicates no async response will be sent. */
  XX_Free(pCbInfo);

  if ((erc != E_OK) || (arpResInfo.dllHeaderSize == 0))
  {
    return L7_FAILURE;                  /* address resolution failed */
  }

  /* address successfully resolved */
  memcpy(pMacAddr, arpResInfo.dllHeader, L7_MAC_ADDR_LEN);

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  If the ARP cache has an entry for a given IP address, return
 *           the corresponding MAC address, along with the MAC address on
 *           the local interface to the neighbor. If no cache entry exists,
 *           send an ARP Request.
 *
 * @param   arpHandle       @b{(input)} handle returned by the ARP_Init
 * @param   userId          @b{(input)} async callback information (ipMapArpResCallbackParms_t*)
 * @param   rpResInfo       @b{(input)} IP address to be resolved and interface
 *
 * @returns   E_OK          - success
 * @returns   E_BUSY        - request was successfully initiated and pending, the
 *                              callback will be called when it is finished
 * @returns   other        - request failed
 *
 * @notes  Only caller is ipMapArpAddrResolveInternal() and that function ignores
 *         the source MAC and ethertype in arpResInfo. So don't bother copying
 *         a source MAC or ethertype into arpResInfo.
 *
 * @end
 * ********************************************************************/
static e_Err ipMapARPResolution (IN t_Handle   arpHandle,
                                 IN t_Handle   userId,
                                 IN OUT t_ARPResInfo *arpResInfo)
{
    t_MACAddr   dstMac;
    e_Err       e;

    /* First check the ARP cache */
    if(ARP_GetMACAddr(arpHandle, (t_IPAddr *)arpResInfo->destAddr.lay3Addr,
                      arpResInfo->lanNmb, &dstMac) == E_OK)
    {
        memcpy(arpResInfo->dllHeader, dstMac, sizeof(t_MACAddr));
        arpResInfo->dllHeaderSize = sizeof(t_MACAddr);
        return E_OK;
    }

    /* Not in the ARP cache, so send an ARP Request */
    e =  ARP_SendARPReq(arpHandle, arpResInfo->destAddr.lay3Addr, userId,
                        (word)arpResInfo->lanNmb);
    if(e == E_OK)
        return E_BUSY;
    return e;
}

/*********************************************************************
* @purpose  Dumps all ARP-related statistics
*
* @param    none
*
* @returns  void
*
* @notes    This function intended for debugging use only.
*
* @end
*********************************************************************/
void dbg_arp_stat()
{
  ARP_ObjectInfoShow(ipMapArpCtx_g.arpHandle);
}
