/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  rip_vend_ctrl.c
*
* @purpose   RIP vendor-specific control routines
*
* @component RIP Mapping Layer
*
* @comments  none
*
* @create    05/21/2001
*
* @author    gpaussa
*
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#include <time.h>
#include "rip_vend_ctrl.h"
#include "rip_util.h"
#include "rto_api.h"
#include "osapi.h"
#ifdef _L7_OS_VXWORKS_
#include "sockLib.h"
#include "netLib.h"
#endif
#include <stdarg.h>
#ifdef _L7_OS_LINUX_
#include "fcntl.h"
#endif

extern ripMapCfg_t      *pRipMapCfgData;
extern ripInfo_t        *pRipInfo;

/* RIP mapping control context (global) */
ripMapCtrl_t    ripMapCtrl_g;

/* external function prototypes */
void rip_start(void);

/* internal function prototypes */
L7_RC_t ripMapVendRouteChangeCallback(int action, naddr ipAddr, 
                                      naddr ipMask, naddr gateway, 
                                      int metric);

/*********************************************************************
* @purpose  Initializes the vendor RIP component
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ripMapVendInit(void)
{
  L7_uint32     i;

  if (pRipInfo->ripInitialized == L7_TRUE)
    return L7_SUCCESS;                  /* must have already been here */

  /* initialize RIP interface state array */
  for (i = 0; i <= L7_RIP_MAP_INTF_MAX_COUNT; i++)
    ripIntfStateSet(i, RIP_MAP_INTF_STATE_NONE);

  /* enable INET and ROUTE socket operation for the system */
  if (osapiSocketLibInit(OSAPI_SOCKETLIB_INET_BSD) != L7_SUCCESS)
  {
    LOG_MSG("ripMapVendInit: INET BSD socket initialization error\n");
    return L7_FAILURE;
  }
  if (osapiSocketLibInit(OSAPI_SOCKETLIB_ROUTE_BSD) != L7_SUCCESS)
  {
    LOG_MSG("ripMapVendInit: ROUTE BSD socket initialization error\n");
    return L7_FAILURE;
  }

  /* create a mutex semaphore to control the otherwise free-running 
   * execution of the rip_Task() so that it only runs while RIP is 
   * enabled for the system; initial state is empty (so task doesn't run)
   */
  ripMapCtrl_g.taskCtrl.semAvail = L7_FALSE;
  ripMapCtrl_g.taskCtrl.semId = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_EMPTY);
  if (ripMapCtrl_g.taskCtrl.semId == L7_NULL)
  {
    LOG_MSG("ripMapVendInit: Unable to create RIP task control semaphore\n");
    return L7_FAILURE;
  }

  /* create a semaphore to control mutual exclusion access to the RIP 
   * radix node tree (i.e., the RIP route table)
   */
  ripMapCtrl_g.rnTreeSemId = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (ripMapCtrl_g.rnTreeSemId == L7_NULL)
  {
    LOG_MSG("ripMapVendInit: Unable to create RIP radix tree semaphore\n");
    return L7_FAILURE;
  }

  bzero((char *)&intfCom, sizeof(ripIntfCom_t));
  intfCom.intfPipeMaxMsgs = L7_RTR_MAX_RTR_INTERFACES * 2; 
  intfCom.intfPipeMsgLen = RIP_INTF_PIPE_MSG_LEN;  
  (void) strncpy(intfCom.intfPipeName, RIP_INTF_PIPE_NAME, strlen(RIP_INTF_PIPE_NAME));
  intfCom.intfPipe = -1;
  if (osapiPipeCreate(intfCom.intfPipeName, intfCom.intfPipeMaxMsgs, 
					  intfCom.intfPipeMsgLen) != L7_SUCCESS)
  {
    LOG_MSG("ripMapVendInit: Unable to create RIP communication pipe\n");
    return L7_FAILURE;
  }
  if (osapiPipeOpen(intfCom.intfPipeName, O_RDWR, 666, &intfCom.intfPipe) 
	  != L7_SUCCESS)
  {
    LOG_MSG("ripMapVendInit: Unable to open RIP communication pipe\n");
    return L7_FAILURE;
  }

  /* start the vendor RIP code */
  if (ripMapStartTasks() != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Deinitializes the vendor RIP component
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ripMapVendDeinit(void)
{
  L7_uint32     i;


  /* Fini the RIP tasks */
  if (ripMapFiniTasks() != L7_SUCCESS)
      LOG_MSG("Unable to FINI RIP tasks\n");


  if (osapiPipeClose(intfCom.intfPipe) != L7_SUCCESS)
  {
    LOG_MSG("Unable to close RIP communication pipe\n");
  }

  #if WPJ_TBD_NEED_OSAPI_CALL
  
  if (osapiPipeDelete( intfCom.intfPipeName) != L7_SUCCESS)
  {
    LOG_MSG(" Unable to delete RIP communication pipe\n");
  }

  #endif

  (void) osapiSemaDelete(ripMapCtrl_g.rnTreeSemId);
  /* create a mutex semaphore to control the otherwise free-running 
   * execution of the rip_Task() so that it only runs while RIP is 
   * enabled for the system; initial state is empty (so task doesn't run)
   */
  (void) osapiSemaDelete(ripMapCtrl_g.taskCtrl.semId);


  #if WPJ_TBD_NEED_OSAPI_CALL
  
  if (osapiSocketLibDeinit( OSAPI_SOCKETLIB_INET_BSD) != L7_SUCCESS)
  {
      LOG_MSG("INET BSD socket deinitialization error\n");
  }

  if (osapiSocketLibDeinit( OSAPI_SOCKETLIB_ROUTE_BSD) != L7_SUCCESS)
  {
      LOG_MSG("ROUTE BSD socket deinitialization error\n");
  }
  #endif

   /* initialize RIP interface state array */
  for (i = 0; i <= L7_RIP_MAP_INTF_MAX_COUNT; i++)
    ripIntfStateSet(i, RIP_MAP_INTF_STATE_NONE);

  pRipInfo->ripInitialized = L7_FALSE;


  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Initializes RIP control context structure
*
* @param    void
*
* @returns  void
*
* @notes    none
*       
* @end
*********************************************************************/
void ripMapVendCtrlCtxInit(void)
{
  ripMapCtrl_g.taskCtrl.semId = 0;
  ripMapCtrl_g.taskCtrl.semAvail = L7_FALSE;
  ripMapCtrl_g.taskCtrl.taskInit = L7_FALSE;
  ripMapCtrl_g.msgLvl = 0;
  ripMapCtrl_g.traceEnabled = L7_FALSE;
}

/*********************************************************************
* @purpose  Signals router interface UP condition to vendor code
*
* @param    intIfNum    router internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Assumes caller has verified that all configuration conditions
*           are met for RIP being activated on this interface.
*           Does not check if current state is UP because this function
*           is called when the IP address on an UP interface changes. 
*       
* @end
*********************************************************************/
L7_RC_t ripMapVendIntfUp(L7_uint32 intIfNum)
{
  ripPipeMsg_t  msg;

  ripIntfStateSet(intIfNum, RIP_MAP_INTF_STATE_UP);

  if (ripMapIsInitialized() == L7_TRUE)
  {
    /* Send a message to the pipe. */
    msg.msgId = L7_RIP_INTF_STATE_CHANGE;
    msg.arg1 = 0; /* redundant. Not used for this messaged id*/

    /* Write to the pipe */
    osapiPipeWrite(intfCom.intfPipe, (L7_char8 *) &msg, RIP_INTF_PIPE_MSG_LEN); 

    /* interface up event is always non-async. RIP vendor thread is going to 
     * call asyncEventSignalSend() on any interface state change. To avoid 
     * sem give failure in asyncEventSignalSend(), call asyncEventSignalWait()
     * here anyway. */
    pRipInfo->ripVendIfWaitForCompletionSignal = L7_FALSE;
    (void) asyncEventSignalWait( &( pRipInfo->ripVendIfCompletionSignal ) );
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Signals router interface DOWN condition to vendor code
*
* @param    intIfNum    router internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ripMapSysctl_iflist() only cares about interfaces in the UP
*           state. So if interface was not previously UP, no need to 
*           poke the protocol engine.
*       
* @end
*********************************************************************/
L7_RC_t ripMapVendIntfDown(L7_uint32 intIfNum)
{
  ripPipeMsg_t  msg;
  ripMapIntfState_t prevState = pRipIntfInfo[intIfNum].state;

  ripIntfStateSet(intIfNum, RIP_MAP_INTF_STATE_DOWN);

  if ((ripMapIsInitialized() == L7_TRUE) &&
      (prevState == RIP_MAP_INTF_STATE_UP))
  {
    /* Send a message to the pipe. */
    msg.msgId = L7_RIP_INTF_STATE_CHANGE;
    msg.arg1 = 0; /* redundant. Not used for this messaged id*/
    osapiPipeWrite(intfCom.intfPipe, (L7_char8 *) &msg, RIP_INTF_PIPE_MSG_LEN); 

    pRipInfo->ripVendIfWaitForCompletionSignal = L7_TRUE;
    (void) asyncEventSignalWait( &( pRipInfo->ripVendIfCompletionSignal ) );
  }

  return L7_SUCCESS;
}


/* -------------------------------------------------------------------------- */
/* ----------------------------- RIP Callbacks ------------------------------ */
/* -------------------------------------------------------------------------- */

/*********************************************************************
* @purpose  Callback for RIP route table notifications from vendor code
*
* @param    action      type of route notification
* @param    ipAddr      destination IP address of the route entry
* @param    ipMask      network address mask of the route entry
* @param    gateway     next hop gateway IP address
* @param    metric      route cost metric
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Ignores route changes, since they typically do not convey
*           any new information of interest to our RTO component.
*       
* @end
*********************************************************************/
L7_RC_t ripMapVendRouteChangeCallback(int action, naddr ipAddr, 
                                      naddr ipMask, naddr gateway, 
                                      int metric)
{
  L7_uchar8     *changeType;
  L7_routeEntry_t routeEntry;
  L7_RC_t rc = L7_SUCCESS;

  switch (action)
  {
  case RTM_ADD:
    changeType = "RTM_ADD   ";
    break;
  case RTM_CHANGE:
    changeType = "RTM_CHANGE";
    break;
  case RTM_DELETE:
    changeType = "RTM_DELETE";
    break;
  default:
    changeType = "Unknown   ";
    rc = L7_FAILURE;
    break;
  }

  /* debug message */
  {
    L7_clocktime ct;

    memset((L7_uchar8 *)&ct, 0, sizeof(ct));
    osapiClockTimeRaw(&ct);
    RIP_MAP_PRT(RIP_MAP_MSGLVL_MED,
                "RIP %s:  dest=%8.8x mask=%8.8x ", 
                changeType, (L7_uint32)ipAddr, (L7_uint32)ipMask);
    RIP_MAP_PRT(RIP_MAP_MSGLVL_MED,
                "gw=%8.8x met=%d time=%u\n", 
                (L7_uint32)gateway, metric, ct.seconds);
  }

  bzero((char *)&routeEntry, sizeof(L7_routeEntry_t));
  routeEntry.ipAddr      = ipAddr;
  routeEntry.subnetMask  = ipMask;
  routeEntry.protocol    = RTO_RIP;
  routeEntry.pref = ipMapRouterPreferenceGet(ROUTE_PREF_RIP);
  routeEntry.metric      = metric;
  routeEntry.ecmpRoutes.equalCostPath[0].arpEntry.ipAddr = gateway;
  routeEntry.ecmpRoutes.numOfRoutes = 1;

  if (action == RTM_ADD)
  {
    rc = rtoRouteAdd(&routeEntry);

    if(rc != L7_SUCCESS)
      RIP_MAP_PRT(RIP_MAP_MSGLVL_MED,
                  "RIP %s:  dest=%8.8x mask=%8.8x gw=%8.8x. Failed to add entry to RTO",
                  changeType, (L7_uint32)ipAddr, (L7_uint32)ipMask, (L7_uint32)gateway);
  }
  else if (action == RTM_DELETE)
  {
    rc = rtoRouteDelete(&routeEntry);
  }
  else if (action == RTM_CHANGE)
  {
    ;   /* occurs when metric or flags change; nothing to do here, though */
  }
  else
  {
    RIP_MAP_PRT(RIP_MAP_MSGLVL_MED, 
                "RIP Route Change: Unknown route change type (%d)\n",
                action);
  }

  return rc;
}


/* -------------------------------------------------------------------------- */
/* --------------------- Calls From RIP Vendor App -------------------------- */
/* -------------------------------------------------------------------------- */

/*********************************************************************
* @purpose  Get the current system up time since the last reset
*
* @param    clockId     clock ID (only CLOCK_REALTIME allowed)
* @param    tp          @b{(output)} time specification in secs, nsecs    
*
* @returns  OK          successful
* @returns  EINVAL      error occurred
*
* @notes    none
*       
* @end
*********************************************************************/
int ripMapVend_clock_gettime(int clockId, struct timespec *tp)
{
  L7_clocktime ct;

  if (clockId != CLOCK_REALTIME)
    return EINVAL;

  memset((L7_uchar8 *)&ct, 0, sizeof(ct));
  osapiClockTimeRaw(&ct);

  /* check to see if the clock is not working */
  if ((ct.seconds == 0) && (ct.nanoseconds == 0))
    return EINVAL;

  tp->tv_sec  = ct.seconds;
  tp->tv_nsec = ct.nanoseconds;
  return L7_OK;
}

/*********************************************************************
* @purpose  Issue an IOCTL request to the system
*
* @param    fd          file descriptor handle
* @param    cmd         requested function to be performed
* @param    data        function-specific argument
*
* @returns  0           successful
* @returns  -1          error occurred
*
* @notes    Only handles file-related IOCTL commands (e.g., FIOxxxx).
*       
* @end
*********************************************************************/
int ripMapVend_ioctl(int fd, int cmd, int data)
{
  return((osapiIoctl(fd, cmd, data) == L7_SUCCESS) ? 0 : -1);
}


/*********************************************************************
* @purpose  Set up relevant config parms in the specified RIP 
*           interface structure
*
* @param    ifp         pointer to vendor RIP interface structure
*
* @returns  void
*
* @notes    This function is called directly by the RIP vendor code.
* @notes    This function MUST not call any external RIP API functions
*           that will try to lock the global ripMapSemaphore, we 
*           intiate the events that cause the vendor code to call this
*           and therefore already have the lock.
*       
* @end
*********************************************************************/
void ripMapVendUserParmsGet(struct interface *ifp)
{
  L7_uint32        intIfNum;
  L7_uint32        val;
  ripMapCfgIntf_t *pCfg;

  /* convert interface index to an internal interface number */
  /* NOTE:  The int_index value should match one of our router intf numbers */
  if (ipMapRtrIntfToIntIfNum((L7_uint32)ifp->int_index, &intIfNum) != L7_SUCCESS)
    return;

  /*----------------------*/
  /* "user" configuration */
  /*----------------------*/

  /* NOTE:  Only one authorization type/key is supported by RIP mapping layer */
  /* NOTE:  Must set auth type before key */

  if (ripMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    return;
  }
  ripMapExtenIntfAuthTypeSet(intIfNum, ifp, pCfg->authType);
  /* the vendor code copies authKey, we don't need an extra copy here */
  ripMapExtenIntfAuthKeySet(intIfNum, ifp, pCfg->authKey);
  ripMapExtenIntfAuthKeyIdSet(intIfNum, ifp, pCfg->authKeyId);
  ripMapExtenIntfVerSendSet(intIfNum, ifp, pCfg->verSend);
  ripMapExtenIntfVerRecvSet(intIfNum, ifp, pCfg->verRecv);

  if (ripMapDefaultRouteAdvertiseModeGet(&val) == L7_SUCCESS)
  {
    if (val == L7_ENABLE)
    {
      if (ripMapDefaultMetricGet(&val) == L7_SUCCESS)
        ifp->int_d_metric = val;
      else
        ifp->int_d_metric = L7_RIP_DEFAULT_METRIC_DEFAULT_DECONFIG;
    }
  }
  /*-----------------------*/
  /* "fixed" configuration */
  /*-----------------------*/
  ifp->int_rdisc_pref = RIP_MAP_CFG_RDISC_PREF;
  ifp->int_rdisc_int = RIP_MAP_CFG_RDISC_INT;

  /* int_state setup */
  RIP_MAP_CFG_INT_STATE_SET(ifp->int_state, RIP_MAP_CFG_NO_AG);
  RIP_MAP_CFG_INT_STATE_SET(ifp->int_state, RIP_MAP_CFG_NO_SUPER_AG);
  RIP_MAP_CFG_INT_STATE_SET(ifp->int_state, RIP_MAP_CFG_NO_RDISC);
  RIP_MAP_CFG_INT_STATE_SET(ifp->int_state, RIP_MAP_CFG_NO_SOL_OUT);
  RIP_MAP_CFG_INT_STATE_SET(ifp->int_state, RIP_MAP_CFG_SOL_OUT);
  RIP_MAP_CFG_INT_STATE_SET(ifp->int_state, RIP_MAP_CFG_NO_ADV_OUT);
  RIP_MAP_CFG_INT_STATE_SET(ifp->int_state, RIP_MAP_CFG_ADV_OUT);
  RIP_MAP_CFG_INT_STATE_SET(ifp->int_state, RIP_MAP_CFG_BCAST_RDISC);
  RIP_MAP_CFG_INT_STATE_SET(ifp->int_state, RIP_MAP_CFG_NO_RIP);
  RIP_MAP_CFG_INT_STATE_SET(ifp->int_state, RIP_MAP_CFG_PM_RDISC);
  RIP_MAP_CFG_INT_STATE_SET(ifp->int_state, RIP_MAP_CFG_DISTRUST);
  RIP_MAP_CFG_INT_STATE_SET(ifp->int_state, RIP_MAP_CFG_REDIRECT_OK);

  /*-----------------------------*/
  /* config consistency checking */
  /*-----------------------------*/

  /* NOTE:  These are rudimentary...assumes you specified a legal config.
   *        Can add more as needed.
   */

  /* no aggregation also implies no super aggregation */
  if (ifp->int_state & IS_NO_AG)
    ifp->int_state |= IS_NO_SUPER_AG;

  /* don't send solicitations if not accepting advertisements */
  if (ifp->int_state & IS_NO_ADV_IN)
    ifp->int_state |= IS_NO_SOL_OUT;

  /* don't send advertisements if not accepting solicitations */
  if (ifp->int_state & IS_NO_SOL_OUT)
    ifp->int_state |= IS_NO_ADV_IN;

  /* don't allow poor man's router discovery if user disabled RIP sends */
  if (IS_RIP_OUT_OFF(ifp->int_state))
    ifp->int_state &= ~IS_PM_RDISC;

  return;
}

/*********************************************************************
* @purpose  Provides the maximum number of RIP routes allowed
*
* @param    void
*
* @returns  Maximum number of RIP routes to support
*
* @notes    none
*       
* @end
*********************************************************************/
int ripMapVendRipRoutesMaxGet(void)
{
  /* just return the maximum number of routes allowed for the platform */
  return (int)L7_L3_ROUTE_TBL_SIZE_TOTAL;
}

/*********************************************************************
* @purpose  Checks a semaphore to determine if the RIP task should be
*           suspended
*
* @param    void
*
* @returns  void
*
* @notes    Called by the vendor RIP task during each loop iteration to
*           provide a hook for suspending task execution in a controlled
*           manner.
*       
* @end
*********************************************************************/
void ripMapVendTaskControl(void)
{
  /* the first time this function is called indicates that the rip_Task
   * has progressed at least to the point where it is initialized, so
   * signal the waiting RIP mapping layer init code so that it can 
   * proceed with the remaining initialization
   */
  if (ripMapCtrl_g.taskCtrl.taskInit == L7_FALSE)
  {
    ripMapCtrl_g.taskCtrl.taskInit = L7_TRUE;
    osapiTaskInitDone(L7_RIP_TASK_SYNC);
  }

  /* take and give the semaphore -- when complete, control returns to the
   * vendor RIP task; if not available, the task is effectively suspended
   */
/*  if (osapiSemaTake(ripMapCtrl_g.taskCtrl.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    LOG_MSG("ripMapVendTaskControl: osapiSemTake error\n");
    return;
  }
  if (osapiSemaGive(ripMapCtrl_g.taskCtrl.semId) != L7_SUCCESS)
  {
    LOG_MSG("ripMapVendTaskControl: osapiSemGive error\n");
    return;
  }
*/  
}

/*********************************************************************
* @purpose  Obtains a semaphore for mutually-exclusive access to the 
*           RIP radix node tree processing critical section
*
* @param    void
*
* @returns  0           successful
* @returns  -1          error occurred
*
* @notes    none
*       
* @end
*********************************************************************/
int ripMapVendRnTreeCritEnter(void)
{
  if (osapiSemaTake(ripMapCtrl_g.rnTreeSemId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    LOG_MSG("ripMapVendRnTreeCritEnter: osapiSemTake error\n");
    return -1;
  }
  return 0;
}

/*********************************************************************
* @purpose  Releases RIP radix node tree mutual exclusion semaphore  
*
* @param    void
*
* @returns  0           successful
* @returns  -1          error occurred
*
* @notes    none
*       
* @end
*********************************************************************/
int ripMapVendRnTreeCritExit(void)
{
  if (osapiSemaGive(ripMapCtrl_g.rnTreeSemId) != L7_SUCCESS)
  {
    LOG_MSG("ripMapVendRnTreeCritExit: osapiSemGive error\n");
    return -1;
  }
  return 0;
}

/*********************************************************************
* @purpose  Sets RIP vendor code trace level
*
* @param    level       RIP trace level (0=off, any other value=on)
*
* @returns  void
*
* @notes    The RIP vendor code supports various levels of tracing,
*           depending on the value set here (1, 2, etc...).
*       
* @end
*********************************************************************/
void ripMapVendTraceLevelSet(L7_uint32 level)
{
  tracelevel = (level <= MAX_TRACELEVEL) ? level : MAX_TRACELEVEL;
  new_tracelevel++;                     /* number of times level has changed */
  ripMapCtrl_g.traceEnabled = (tracelevel == 0) ? L7_FALSE : L7_TRUE; 
}

/*********************************************************************
* @purpose  Handles trace logging from the RIP application
*
* @param    id          trace point identifier
* @param    fmt         format string
* @param    args...     variable length argument list
*
* @returns  void
*
* @notes    Displaying RIP trace entries requires both the 
*           ripMapCtrl_g.traceEnabled mode flag to be set to L7_TRUE 
*           **AND** the RIP mapping layer message level to be set
*           to medium or better.
*       
* @end
*********************************************************************/
void ripMapVendTrace(int id, const char *fmt, ...)
{
#define RIP_FMTSTR_MAX      256         /* assumed big enough for worst case */

  static L7_char8  fmtStr[RIP_FMTSTR_MAX];
  L7_char8      *banner = "RIP trace_%s: ";
  L7_char8      *type;
  va_list       args;
 
  /* this is a lot of work -- first make sure it will be worth it */
  if ((ripMapCtrl_g.traceEnabled != L7_TRUE) || 
      (ripMapCtrl_g.msgLvl < RIP_MAP_MSGLVL_MED))
    return;

  switch (id)
  {
  case RIP_TRACE_ACT:       type = "act    ";  break;
  case RIP_TRACE_ADD_DEL:   type = "add_del";  break;
  case RIP_TRACE_CHANGE:    type = "change ";  break;
  case RIP_TRACE_UPSLOT:    type = "upslot ";  break;
  case RIP_TRACE_MISC:      type = "misc   ";  break;
  case RIP_TRACE_IF:        type = "intf   ";  break;
  case RIP_TRACE_RIP:       /*type = "rip    "; */ return; /* not working properly*/
  case RIP_TRACE_PKT:       type = "pkt    ";  break;
  case RIP_TRACE_RDISC:     type = "rdisc  ";  break;
  case RIP_TRACE_OFF:       type = "off    ";  break;
  default:
    return;
  }

  memset(&args, 0, sizeof(args));
  va_start(args, fmt);

  /* prepend a banner to the caller's format specifier */
  memset(fmtStr, 0, RIP_FMTSTR_MAX);
  if (id != RIP_TRACE_RDISC)  /* let rdisc "fprintf" display w/o our hdr */
    sprintf(fmtStr, banner, type);
  osapiStrncat(fmtStr, fmt, sizeof(fmtStr) - strlen(fmtStr)-1);
  /* make sure entire format string fits (including null terminator)
   * NOTE:  If this assert ever hits, need to increase RIP_FMTSTR_MAX value
   */
  L7_assert(strlen(fmtStr) >= RIP_FMTSTR_MAX); /* local buffer too small */

  /* display trace message */
  ripMapVendSysPrint(RIP_MAP_MSGLVL_MED, fmtStr, args);

  va_end(args);

#undef RIP_FMTSTR_MAX
}

/*********************************************************************
* @purpose  Invokes the message logging facility
*
* @param    message     Message to be logged
*
* @returns  void
*
* @notes    none
*       
* @end
*********************************************************************/
void ripMapVendLogMsg(char *message)
{
  LOG_MSG("%s\n", message);
}

/*********************************************************************
* @purpose  Wrapper for SYSAPI_PRINTF to display longer output information
*
* @param    msgLvl      message level specifier
* @param    fmt         format string
* @param    args...     variable length argument list
*
* @returns  void
*
* @notes    Function currently limited to printing up to 254 characters.
*       
* @end
*********************************************************************/
void ripMapVendSysPrint(L7_uint32 msgLvl, const char *fmt, ...)
{
#define RIP_BUF_MAX         (254+2)     /* assumed big enough for worst case */
#define RIP_SYSPRT_AMT      64          /* our printf chunk size */

  static L7_char8  buf[RIP_BUF_MAX];
  L7_char8      *pBuf;
  va_list       args;
  L7_uint32     len;

  /* check this invocation against current message level setting */
  if (ripMapCtrl_g.msgLvl < msgLvl)
    return;

  memset(&args, 0, sizeof(args));
  va_start(args, fmt);

  /* fully expand the trace message in local buffer to determine overall size */
  memset(buf, 0, RIP_BUF_MAX);
  sprintf(buf, fmt, args);
  /* add a newline char if string does not already end with one */
  len=strlen(buf);
  if (len > 0 )
  {
    if (len >= sizeof(buf) - 1)
      len = len - 1;
    *(buf + len) = '\n';
    *(buf + len + 1) = '\0';
  }
  /* make sure entire print string fits (including null terminator)
   * NOTE:  If this assert ever hits, need to increase RIP_BUF_MAX value
   */
  L7_assert(strlen(buf) >= RIP_BUF_MAX); /* local buffer too small */

  /* the trace message can be longer than the sysapi printf function
   * is able to handle, so break it down into multiple invocations
   */
  pBuf = buf;
  while ((len = strlen(pBuf)) > RIP_SYSPRT_AMT)
  {
    L7_char8    saveChar;

    /* create a null-terminated substring by saving/restoring the print
     * character at the chopping point, and replace it with a '\0' for 
     * the API call
     */
    saveChar = *(pBuf + RIP_SYSPRT_AMT);
    *(pBuf + RIP_SYSPRT_AMT) = '\0';
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, pBuf);
    pBuf += RIP_SYSPRT_AMT;
    *pBuf = saveChar;                   /* restore original char */
  }
  /* display remaining portion */
  if (len > 0)
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, pBuf);

  va_end(args);

#undef RIP_SYSPRT_AMT
#undef RIP_BUF_MAX
}

/**************************************************************************
* @purpose     Convert an IP address from a hex value to an ASCII string
*
* @param       ipAddr     @b{(input)}  IP address to be converted (host byte
*                                        order)
* @param       buf        @b{(output)} location to store IP address string
* @param       buflen     @b{(input)}  length of the buffer provided by the
*                                      calling function.
*
* @returns     L7_SUCCESS
* @returns     L7_FAILURE
*
* @comments    Caller must provide an output buffer of at least
* @comments    OSAPI_INET_NTOA_BUF_SIZE bytes (as defined in
* @comments    osapi.h).
*
* @end
*************************************************************************/
uint ripMapVendInetNtoa (naddr ipAddr, u_char *buf, uint buflen)
{
#if (INET_NTOA_BUFSIZE < OSAPI_INET_NTOA_BUF_SIZE)
#error INET_NTOA_BUFSIZE too small!
#endif
  char addrStr[OSAPI_INET_NTOA_BUF_SIZE];
  
  /* clean up buffers */
  memset(addrStr, 0, OSAPI_INET_NTOA_BUF_SIZE);
  memset(buf, 0, buflen);
  
  /* call osapi for ip address conversion */
  osapiInetNtoa(ipAddr, addrStr);

  if (buflen < OSAPI_INET_NTOA_BUF_SIZE)
  {            
    if (buflen > 0)
    {
      memcpy(buf, addrStr, buflen);
      buf[buflen-1]=0;
    }
    return L7_FAILURE;
  }

  strcpy(buf,addrStr);
  return L7_SUCCESS;
}

/**************************************************************************
* @purpose     To get the rip intf communication pipe and len of msg
*
* @param       intfPipe         Pointer to the pipe descriptor
* @param       intfPipeMsgLen   Pointer to the pipe msg len
*
* @returns     none
*
* @comments    Used by vendor code.
*
* @end
*************************************************************************/
void ripMapVendRipIntfPipeGet(int *intfPipe, uint *intfPipeMsgLen)
{
  if (intfPipe == L7_NULLPTR || intfPipeMsgLen == L7_NULLPTR)
  {
    return;
  }
  *intfPipe = intfCom.intfPipe;
  *intfPipeMsgLen = intfCom.intfPipeMsgLen;
  return;
}

/**************************************************************************
* @purpose     To get the rip global settings
*
* @param       *ripSplitHorizon		 @b{(output)} Split Horizon Mode
* @param       *ripAutoSummary       @b{(output)} Auto Summary Mode
* @param       *ripHostRoutesAccept  @b{(output)} Host Routes Accept Mode
*
* @returns     none
*
* @comments    Used by vendor code. Can't include defs.h here, so can't 
*              use HOSTROUTESACCEPT_ON/OFF.
*
* @end
*************************************************************************/
void ripMapVendGlobalsGet(int *ripSplitHorizon, int *ripAutoSummary, 
						  int *ripHostRoutesAccept)
{
  if (ripSplitHorizon == L7_NULLPTR || ripAutoSummary == L7_NULLPTR || 
	  ripHostRoutesAccept == L7_NULLPTR)
  {
	return;
  }
  *ripSplitHorizon = FD_RIP_MAP_DEFAULT_SPLITHORIZON_MODE;
  *ripAutoSummary = FD_RIP_MAP_DEFAULT_AUTOSUMMARY_MODE;
  if (FD_RIP_MAP_DEFAULT_HOSTROUTESACCEPT_MODE == L7_ENABLE)
    *ripHostRoutesAccept = 2;   /* HOSTROUTESACCEPT_ON */
  else
    *ripHostRoutesAccept = 1;   /* HOSTROUTESACCEPT_OFF */
  return;
}


/*********************************************************************
* @purpose  This routine is called by the protocol whenever it receives
*           default metric change message.
*
* @param    metric   		 @b{(input)} Changed default metric.
*
* @returns  
*
* @comments 
*           
* @end
*********************************************************************/
void ripMapVendRipDefaultMetricSet(uint metric)
{
  ripPassThru_t    ripPassThru;
  struct walkarg  *temp;
  
  /* Walk thru all the routes and change the metric of the routes. */
  /* Put metric into a structure and pass it as
     pointer to the walk function.
  */
  ripPassThru.metric = metric;
  temp = (void *) &ripPassThru;

  (void)rn_walktree(rhead, ripMapVendRipDefaultMetricRouteTableCallback, temp);
}


/*********************************************************************
* @purpose  This routine is called whenever the default metric has changed
*
* @param    
*
* @returns  0 if successful, non-zero otherwise
*
* @comments This function is called by rn_walktree to change the metric of
*           redistributed routes if default metric has changed.
*       
* @end
*********************************************************************/
int ripMapVendRipDefaultMetricRouteTableCallback(struct radix_node *rn, struct walkarg *arp)
{
  L7_uint32 val,metric;
  L7_REDIST_RT_INDICES_t protocol;
  struct rt_entry *rt = (struct rt_entry *)rn;
  struct rt_spare *rts;
  int             i;
  ripPassThru_t *ripPassThru = (ripPassThru_t*) arp;
  val = (L7_uint32) ripPassThru->metric;

  /* Make sure to check all the spares. */
  /* If this route is not redist route, return 0*/
  if ((rt->rt_state & RS_REDIST) == 0)
    return 0;

  for (rts = rt->rt_spares, i = 0; i < NUM_SPARES; i++, rts++)
  {
    /* assume only one spare contains a redist proto for a given route entry */
    if ((rts->rts_redist_proto > RIP_RT_REDIST_FIRST) && 
        (rts->rts_redist_proto < RIP_RT_REDIST_LAST))
    {
      break;                /* found spare with matching redist proto */
    }
  } /* endfor */
  if (i == NUM_SPARES)
    return 0;                 /* matching redist proto not found */
  
  /* Convert the protocol from vendor rip to ripMap type. */
  if (ripMapVendRouteRedistRipProtoProtocolConvert(rts->rts_redist_proto, 
                                                   &protocol) != L7_SUCCESS)
    return 0; 

  /* If it is same metric, return 0. If the route has been withdrawn (is being
   * advertised with a metric of 16, don't apply the new default metric to it. */
  if ((rts->rts_metric == val) || (rts->rts_metric == HOPCNT_INFINITY))
    return 0;

  /* If there is a redistribution metric configured for the source of this
   * route, then the redist metric takes precedence over the default metric.
   * Therefore, nothing needs to be done.
  */
  metric = 0;
  if (ripMapRouteRedistributeMetricGet(protocol, &metric) == L7_SUCCESS)
  {
      /* A redistribution metric is configured */
      return 0;
  }

  /* Change the metric and set the flash update only if it is the best route. */
  rts->rts_metric = val;
  if (rts == rt->rt_spares)
  {
    rt->rt_seqno = update_seqno;
    set_need_flash();
  }
  rtswitch(rt, 0);            /* check for a new best route */
  
  return 0;
}


/*********************************************************************
* @purpose  This routine is called by the protocol whenever it receives
*           metric change message.
*
* @param    rip_proto        @b{(input)} Protocol for which metric has changed.
* @param    metric           @b{(input)} Changed default metric.
*
* @returns  
*
* @comments 
*       
* @end
*********************************************************************/
void ripMapVendRipRouteRedistMetricSet(rip_redist_proto rip_proto, uint metric)
{
  ripPassThru_t    ripPassThru;
  struct walkarg  *temp;
  
  /* Walk thru all the routes and change the metric of the routes. */
  /* Convert the protocol and put both protocol and metric into a structure and pass it as
     pointer to the walk function.
  */
  ripPassThru.rip_proto = rip_proto;
  ripPassThru.metric = metric;
  temp = (void *) &ripPassThru;

  (void)rn_walktree(rhead, ripMapVendRipRouteRedistMetricRouteTableCallback, temp);
}


/*********************************************************************
* @purpose  This routine is called whenever the metric for a protocol has changed
*
* @param    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This function is called by rn_walktree to change the metric of
*           redistributed routes for a particular protocol.
*       
* @end
*********************************************************************/
int ripMapVendRipRouteRedistMetricRouteTableCallback(struct radix_node *rn, struct walkarg *arp)
{
  struct rt_entry *rt = (struct rt_entry *)rn;
  struct rt_spare *rts;
  int             i;
  ripPassThru_t    *ripPassThru;

  ripPassThru = (ripPassThru_t *) arp;

  /* If this route does not contain any redist spares, return 0. */
  if ((rt->rt_state & RS_REDIST) == 0)
    return 0;

  /* Make sure to check all the spares. */
  /* If this route is not redist route for this protocol, return 0. */
  for (rts = rt->rt_spares, i = 0; i < NUM_SPARES; i++, rts++)
  {
    /* assume only one spare contains a redist proto for a given route entry */
    if ((rts->rts_redist_proto > RIP_RT_REDIST_FIRST) && 
        (rts->rts_redist_proto < RIP_RT_REDIST_LAST))
    {
      if (rts->rts_redist_proto != ripPassThru->rip_proto) 
        return 0;
      else
        break;                /* found spare with matching redist proto */
    }
  } /* endfor */

  if (i == NUM_SPARES)
    return 0;                 /* matching redist proto not found */

  /* ignore routes that are being poisoned (they're on their way out!) */
  if (rts->rts_metric == HOPCNT_INFINITY)
    return 0;                 

  /* If the metric is same as the metric passed in the structure, return 0. */
  if (rts->rts_metric == ripPassThru->metric)
    return 0;

  /* Change the metric and set the flash update only if it is the best route. return 0. */
  rts->rts_metric = ripPassThru->metric;
  if (rts == rt->rt_spares)
  {
    rt->rt_seqno = update_seqno;
    set_need_flash();
  }
  rtswitch(rt, 0);            /* check for a new best route */
  
  return 0;
}


/*********************************************************************
* @purpose  Convert the redist protocol type to vendor rip redist protocol type.
*
* @param    protocol         @b{(input)} Redistributed protocol type
* @param    *rip_proto       @b{(output)} Vendor rip protocol type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
L7_RC_t ripMapVendRouteRedistProtocolConvert(L7_REDIST_RT_INDICES_t protocol, rip_redist_proto *rip_proto)
{
  if (rip_proto == L7_NULLPTR)
    return L7_FAILURE;

  switch (protocol)
  {
    case REDIST_RT_OSPF:
      *rip_proto = RIP_RT_REDIST_OSPF;
      break;

    case REDIST_RT_BGP:
      *rip_proto = RIP_RT_REDIST_BGP;
      break;

    case REDIST_RT_LOCAL:
      *rip_proto = RIP_RT_REDIST_LOCAL;
      break;

    case REDIST_RT_STATIC:
      *rip_proto = RIP_RT_REDIST_STATIC;
      break;

    default:
      return L7_FAILURE;
      /*passthru*/
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Convert the vendor rip redist protocol type to redist protocol type.
*
* @param    rip_proto        @b{(input)} Vendor rip protocol type
* @param    *protocol        @b{(output)} Redistributed protocol type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
L7_RC_t ripMapVendRouteRedistRipProtoProtocolConvert(rip_redist_proto rip_proto, L7_REDIST_RT_INDICES_t *protocol)
{
  if (protocol == L7_NULLPTR)
    return L7_FAILURE;

  switch (rip_proto)
  {
    case RIP_RT_REDIST_OSPF:
      *protocol = REDIST_RT_OSPF;
      break;

    case RIP_RT_REDIST_BGP:
      *protocol = REDIST_RT_BGP;
      break;

    case RIP_RT_REDIST_LOCAL:
      *protocol = REDIST_RT_LOCAL;
      break;

    case RIP_RT_REDIST_STATIC:
      *protocol = REDIST_RT_STATIC;
      break;

    default:
      return L7_FAILURE;
      /* Pass thru */
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This routine converts the internal interface number to the
*           router interface number to be used by the vendor code
*
* @param    intIfNum   	@b{(input)} internal interface number
* @param    *rtrIfNum   @b{(output)} router interface number
*
* @returns  0   if the matching router interface number exists
* @returns  -1  if no matching router interface number exists
*
* @comments 
*           
* @end
*********************************************************************/
int ripMapVendIfNumToRtrIfNum(L7_uint32 intIfNum, L7_uint32 *rtrIfNum)
{
  if(ipMapIntIfNumToRtrIntf(intIfNum, rtrIfNum) != L7_SUCCESS)
    return -1;
  return 0;
}

/* -------------------------------------------------------------------------- */
/* -------------------------- Debug Functions ------------------------------- */
/* -------------------------------------------------------------------------- */

static char *ripMapVendRedistProtoStr[] = { "---", "local", "static", "ospf", "bgp", "---" };

static int ripRouteEntryDisplay(struct radix_node *rn, struct walkarg *argp)
{
  struct rt_entry *RT = (struct rt_entry *)rn;
  L7_BOOL showBest = *(L7_BOOL *)argp;
  char  *ip;
  L7_clocktime ct;
  L7_uint32 ageTime;
  struct rt_spare *rts;
  int i, max;
  char *pIntName;

  /* compute current route age (normalize route entry to clock time) */
  memset((L7_uchar8 *)&ct, 0, sizeof(ct));
  osapiClockTimeRaw(&ct);
  if (AGE_RT(RT->rt_state, RT->rt_ifp))
    ageTime = ct.seconds - (L7_uint32)(RT->rt_time + epoch.tv_sec);
  else
  {
    if ((!(RT->rt_state | RS_NET_SYN)) || 
        ((RT->rt_state | RS_NET_SYN) && (RT->rt_metric < HOPCNT_INFINITY)))
      ageTime = 0;
    else
      ageTime = ct.seconds - (L7_uint32)(RT->rt_time + epoch.tv_sec);
  }

  max = (showBest == L7_TRUE) ? NUM_SPARES : 1;
  for (rts = RT->rt_spares, i = 0; i < max; i++, rts++) 
  {
    if (i == 0)
    {
      ip = naddr_ntoa(RT->rt_dst);
      RIP_MAP_PRT(RIP_MAP_MSGLVL_ON, "%-15.15s  ", ip);

      ip = naddr_ntoa(RT->rt_msk);
      RIP_MAP_PRT(RIP_MAP_MSGLVL_ON, "%-15.15s    ", ip);
    }
    else
    {
      if (rts->rts_gate == 0)
        continue;

      RIP_MAP_PRT(RIP_MAP_MSGLVL_ON, "  spare[%1d]       ", i);

      ip = " ";
      RIP_MAP_PRT(RIP_MAP_MSGLVL_ON, "%-15.15s    ", ip);
    }

    ip = naddr_ntoa(rts->rts_gate);
    RIP_MAP_PRT(RIP_MAP_MSGLVL_ON, "%-15.15s  ", ip);

    ip = naddr_ntoa(rts->rts_router);
    RIP_MAP_PRT(RIP_MAP_MSGLVL_ON, "%-15.15s  ", ip);

    RIP_MAP_PRT(RIP_MAP_MSGLVL_ON,
               "%2.1d    %4u  %8.8x  %-6.6s  ",
               rts->rts_metric, ageTime, RT->rt_state, 
               ripMapVendRedistProtoStr[rts->rts_redist_proto]);

    if (RT->rt_ifp != 0)
      pIntName = RT->rt_ifp->int_name;
    else
      pIntName = "n/a";
    RIP_MAP_PRT(RIP_MAP_MSGLVL_ON, "%s (0x%x)\n", pIntName, (L7_uint32)RT->rt_ifp);

  } /* endfor */

  return 0;
}

void ripMapVendRouteShow(L7_uint32 mode)
{
  L7_BOOL showBest = (mode == 1) ? L7_TRUE : L7_FALSE;

  if (pRipInfo->ripInitialized != L7_TRUE)
  {
    RIP_MAP_PRT(RIP_MAP_MSGLVL_ON, "RIP application not initialized\n");
    return;
  }

  if (ripMapVendRnTreeCritEnter())
  {
    RIP_MAP_PRT(RIP_MAP_MSGLVL_ON, "Could not enter critical region.\n");
    return;
  }
  
  RIP_MAP_PRT(RIP_MAP_MSGLVL_ON,
    "---------------------------------------------------");
  RIP_MAP_PRT(RIP_MAP_MSGLVL_ON,
    "---------------------------------------------------------\n");
  RIP_MAP_PRT(RIP_MAP_MSGLVL_ON,
    "  Destination         Mask            Gateway      ");
  RIP_MAP_PRT(RIP_MAP_MSGLVL_ON,
    "    Router       Metric  Time   State    Redist  Intf    \n");
  RIP_MAP_PRT(RIP_MAP_MSGLVL_ON,
    "---------------------------------------------------");
  RIP_MAP_PRT(RIP_MAP_MSGLVL_ON,
    "---------------------------------------------------------\n");
  (void)rn_walktree(rhead, ripRouteEntryDisplay, (void *)&showBest);
  RIP_MAP_PRT(RIP_MAP_MSGLVL_ON,
    "---------------------------------------------------");
  RIP_MAP_PRT(RIP_MAP_MSGLVL_ON,
    "---------------------------------------------------------\n");
  ripMapVendRnTreeCritExit();
}

void ripMapVendMibShow(void)
{
  struct interface *ifp;

  if (pRipInfo->ripInitialized != L7_TRUE)
  {
    RIP_MAP_PRT(RIP_MAP_MSGLVL_ON, "RIP application not initialized\n");
    return;
  }

  RIP_MAP_PRT(RIP_MAP_MSGLVL_ON, "RFC 1724 RIPv2 MIB Contents\n");
  RIP_MAP_PRT(RIP_MAP_MSGLVL_ON, "---------------------------\n");

  /* global stats */
  RIP_MAP_PRT(RIP_MAP_MSGLVL_ON, "Global Stats:  ");
  RIP_MAP_PRT(RIP_MAP_MSGLVL_ON, "route changes=%u  queries=%u\n", 
              rip_glob_mibstats.route_changes, rip_glob_mibstats.queries);

  /* interface stats */
  RIP_MAP_PRT(RIP_MAP_MSGLVL_ON, "\nInterface Stats\n");
  for (ifp = ifnet; ifp != 0; ifp = ifp->int_next)
  {
    char          ipStr[16];

    ripMapIpAddrToStr(osapiNtohl(ifp->int_addr), ipStr);
    RIP_MAP_PRT(RIP_MAP_MSGLVL_ON, 
                "[%d]:  addr=%-15.15s tot=%u  badpkts=%u  badrtes=%u  trupds=%u\n",
                ifp->int_index, ipStr, ifp->int_rip_pkts_rcvd, ifp->int_mibstats.ibadpkts, 
                ifp->int_mibstats.ibadroutes, ifp->int_mibstats.otrupdates);
  }

  /* interface stats */
  RIP_MAP_PRT(RIP_MAP_MSGLVL_ON, "\nInterface config\n");
  for (ifp = ifnet; ifp != 0; ifp = ifp->int_next)
  {
    u_int state;
    char *str;
    L7_uchar8 key[RIP_AUTH_PW_LEN+1];
    L7_uint32 i;

    RIP_MAP_PRT(RIP_MAP_MSGLVL_ON, "[%d]: ", ifp->int_index);

    state = ifp->int_state;

    if (IS_RIP_OUT_OFF(state))
      str = "None  ";
    else if (state & IS_NO_RIPV2_OUT)
      str = "RIP-1 ";
    else if (state & IS_NO_RIP_MCAST)
      str = "RIP-1c";
    else
      str = "RIP-2 ";
    RIP_MAP_PRT(RIP_MAP_MSGLVL_ON, " send=%s", str);

    if (IS_RIP_IN_OFF(state))
      str = "None ";
    else if (state & IS_NO_RIPV2_IN)
      str = "RIP-1";
    else if (state & IS_NO_RIPV1_IN)
      str = "RIP-2";
    else
      str = "Both ";
    RIP_MAP_PRT(RIP_MAP_MSGLVL_ON, "  recv=%s", str);

    memcpy(key, ifp->int_auth[0].key, RIP_AUTH_PW_LEN);
    *(key+RIP_AUTH_PW_LEN) = L7_EOS;
    RIP_MAP_PRT(RIP_MAP_MSGLVL_ON, "[%d]:  auType=%hu  auId=%u  auKey=\"%s\"", 
                ifp->int_index, ifp->int_auth[0].type, (u_int)ifp->int_auth[0].keyid, key);

    RIP_MAP_PRT(RIP_MAP_MSGLVL_ON, " (hex: 0x");
    for (i = 0; i < RIP_AUTH_PW_LEN; i++)
    {
      RIP_MAP_PRT(RIP_MAP_MSGLVL_ON, "%2.2x", key[i]);
    }
    RIP_MAP_PRT(RIP_MAP_MSGLVL_ON, ")\n");
  }
}
