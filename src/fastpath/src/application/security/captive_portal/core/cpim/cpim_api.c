#include "commdefs.h"
#include "datatypes.h"
#include "sysapi.h"
#include "default_cnfgr.h"
#include "l7_cnfgr_api.h"
#include "captive_portal_common.h"
#include "cpdm_api.h"
#include "cpdm_connstatus_api.h"
#include "cpim_api.h"
#include "cpcm_api.h"
#include "cpdm.h"
#include "wireless_commdefs.h"
#include "wdm_network_api.h"
#include "nimapi.h"
#include "cpwio_api.h"
#include "captive_portal_config.h"


#ifdef L7_MGMT_SECURITY_PACKAGE
#include "usmdb_sslt_api.h"
#include "usmdb_sim_api.h"
#endif

#include "dot3ad_api.h"
#include "captive_portal_cnfgr.h"

#ifdef L7_NSF_PACKAGE
#include "cp_ckpt.h"
#endif

extern void *pCaptivePortalMsgQueue;
extern cpdmOprData_t *cpdmOprData;
extern cpdmCfgData_t *cpdmCfgData;
extern captivePortalCnfgrState_t captivePortalCnfgrState;


extern L7_RC_t cpcmAIPStatusCleanup(L7_uint32 intIfNum);

/*********************** Local types **********************/

/* In order to handle most work in the CP task, and keep our
 * data structures de-coupled from the need to do work, we have
 * a "work order" tree.  The NIM callbacks will add work orders,
 * and the CP main task loop will process and remove them.
 *
 * Also note that we make distinctions betweeen work originated
 * by NIM (such as responses to ATTACH and DETACH events) and
 * the UI via CPDM (such as responses to enable or disable an
 * interface).
 */

typedef enum
{
  CP_INTF_FINISH_ADD    = 0,
  CP_INTF_FINISH_ATTACH,
  CP_INTF_FINISH_DETACH,
  CP_INTF_FINISH_DELETE,
  CP_INTF_FINISH_UNBLOCK,    /* Finish call from CP, not NIM */
  CP_INTF_FINISH_BLOCK,      /* Finish call from CP, not NIM */
  CP_INTF_NUM_WORK_ITEMS
} work_t;

typedef struct intfWorkOrder_s
{
  work_t                    work;
  /* use intIfNum field in here for NIM and UI work.  UI work will be
   * denoted by a correlator value of 0 (which NIM avoids).
   */
  NIM_EVENT_COMPLETE_INFO_t comp;
} intfWorkOrder_t;

typedef struct intfWorkOrderQ_s
{
  intfWorkOrder_t * pAll;   /* pointer to buffer with max # work orders */
  L7_uint32  front, back, size;
  void *     sema;
} intfWorkOrderQ_t;

static intfWorkOrderQ_t  intfWOQ =  { L7_NULLPTR, 0, 0, 0};

static L7_RC_t  intfWOQInit(L7_uint32 maxOrders);
static void     intfWOQFinalize(void);
static L7_RC_t  intfWOGet(work_t * pWork,
                          NIM_EVENT_COMPLETE_INFO_t * comp );
static L7_RC_t  intfWOAdd(work_t work,
                          NIM_EVENT_COMPLETE_INFO_t * comp );

static L7_RC_t cpimIntfCapabilitySetHelper(cpdmIntfCapabilityStatusData_t *pStat,
                                           L7_uint32 intIfNum);

/* Syntactic sugar */
#define SEMA_TAKE(access)   \
  if ( CPDM_##access##_LOCK_TAKE(cpdmSema, L7_WAIT_FOREVER) != L7_SUCCESS)  LOG_ERROR(0);
#define SEMA_GIVE(access)   \
  if ( CPDM_##access##_LOCK_GIVE(cpdmSema) != L7_SUCCESS)  LOG_ERROR(0);

static avlTree_t         capabStatusTree;
static L7_BOOL           capabStatusTreeReady = L7_FALSE;
static avlTreeTables_t * capabStatusTreeHeap  = L7_NULLPTR;
static void            * capabStatusDataHeap  = L7_NULLPTR;
static L7_uint32         currMaxCapabStatus   = 0;

static L7_uchar8 * cplabel = "Captive Portal";
static L7_uchar8 * wo_add_failed = "can't get work (%d) for intf (%d) to %s\n";


/* The main CPIM entry points (except for the NIM callback) are given
 * below.  Each of these will cause a "work item" to be prepared for the
 * CP task.
 */

L7_RC_t cpimIntfAdd(L7_uint32 intIfNum);
L7_RC_t cpimIntfDelete(L7_uint32 intIfNum);
L7_RC_t cpimIntfUnblock(L7_uint32 intIfNum);
L7_RC_t cpimIntfBlock(L7_uint32 intIfNum);
L7_RC_t cpimIntfGet(L7_uint32 intIfNum);
L7_RC_t cpimIntfNextGet(L7_uint32 intIfNum, L7_uint32 * nextIntfId);
/* Implementations of the main entry points for the interface
 * capability database
 */

typedef L7_RC_t (*cpimIntfCapabilityFn)(L7_uint32);

static L7_RC_t cpimIntfCapabilityAdd(L7_uint32 intIfNum);
static L7_RC_t cpimIntfCapabilityAttach(L7_uint32 intIfNum);
static L7_RC_t cpimIntfCapabilityDetach(L7_uint32 intIfNum);
static L7_RC_t cpimIntfCapabilityDelete(L7_uint32 intIfNum);
static L7_RC_t cpimIntfCapabilityUnblock(L7_uint32 intIfNum);
static L7_RC_t cpimIntfCapabilityBlock(L7_uint32 intIfNum);

/* Table for function lookup based on work type.  Note that
 * we use the work_t type to index here.
 */

static
cpimIntfCapabilityFn
cpimIntfCapabilityFnTable[CP_INTF_NUM_WORK_ITEMS] =
{
    cpimIntfCapabilityAdd,
    cpimIntfCapabilityAttach,
    cpimIntfCapabilityDetach,
    cpimIntfCapabilityDelete,
    cpimIntfCapabilityUnblock,
    cpimIntfCapabilityBlock
};

/* Implementations of the main entry points for the interface
 * status database
 */

/*
extern L7_RC_t cpdmIntfStatusAdd(L7_uint32 intIfNum);
extern L7_RC_t cpdmIntfStatusDelete(L7_uint32 intIfNum); */
static L7_RC_t cpimIntfStatusUnblock(L7_uint32 intIfNum);
static L7_RC_t cpimIntfStatusBlock(L7_uint32 intIfNum);
static L7_RC_t cpimIntfStatusAttach(L7_uint32 intIfNum);
static L7_RC_t cpimIntfStatusDetach(L7_uint32 intIfNum);


/* Table for function lookup based on work type.  Note that
 * we use the work_t type to index here.
 */

typedef L7_RC_t (*cpimIntfStatusFn)(L7_uint32);

static
cpimIntfStatusFn
cpimIntfStatusFnTable[CP_INTF_NUM_WORK_ITEMS] =
  {
    cpdmIntfStatusAdd,    /* extern! */
    cpimIntfStatusAttach,
    cpimIntfStatusDetach,
    cpdmIntfStatusDelete, /* extern! */
    cpimIntfStatusUnblock,
    cpimIntfStatusBlock
  };

/* Helpers for use from our NIM callback routine. */
static L7_RC_t cpimIntfAddFromCallback(NIM_EVENT_COMPLETE_INFO_t * comp);
static L7_RC_t cpimIntfDeleteFromCallback(NIM_EVENT_COMPLETE_INFO_t * comp);
static L7_RC_t cpimIntfAttachFromCallback(NIM_EVENT_COMPLETE_INFO_t * comp);
static L7_RC_t cpimIntfDetachFromCallback(NIM_EVENT_COMPLETE_INFO_t * comp);


L7_RC_t cpimIntfFinishWork(void);


/* Helpers for the main "finish" routines above.  Some of these
 * are mere stubs.  We use the same table-lookup strategy here too.
 */
typedef L7_RC_t (*cpimIntfCapabilityFinishFn)(L7_uint32 intIfNum, L7_PORT_EVENTS_t event);

static L7_RC_t cpimIntfCapabilityFinishAdd(L7_uint32 intIfNum, L7_PORT_EVENTS_t event);
static L7_RC_t cpimIntfCapabilityFinishAttach(L7_uint32 intIfNum, L7_PORT_EVENTS_t event);
static L7_RC_t cpimIntfCapabilityFinishDetach(L7_uint32 intIfNum, L7_PORT_EVENTS_t event);
static L7_RC_t cpimIntfCapabilityFinishDelete(L7_uint32 intIfNum, L7_PORT_EVENTS_t event);
static L7_RC_t cpimIntfCapabilityFinishUnblock(L7_uint32 intIfNum, L7_PORT_EVENTS_t event);
static L7_RC_t cpimIntfCapabilityFinishBlock(L7_uint32 intIfNum, L7_PORT_EVENTS_t event);

static
cpimIntfCapabilityFinishFn
cpimIntfCapabilityFinishFnTable[CP_INTF_NUM_WORK_ITEMS] =
{
    cpimIntfCapabilityFinishAdd,
    cpimIntfCapabilityFinishAttach,
    cpimIntfCapabilityFinishDetach,
    cpimIntfCapabilityFinishDelete,
    cpimIntfCapabilityFinishUnblock,
    cpimIntfCapabilityFinishBlock
};

typedef L7_RC_t (*cpimIntfStatusFinishFn)(L7_uint32 intIfNum);

static L7_RC_t cpimIntfStatusFinishAdd(L7_uint32 intIfNum);
static L7_RC_t cpimIntfStatusFinishAttach(L7_uint32 intIfNum);
static L7_RC_t cpimIntfStatusFinishDetach(L7_uint32 intIfNum);
static L7_RC_t cpimIntfStatusFinishDelete(L7_uint32 intIfNum);
static L7_RC_t cpimIntfStatusFinishUnblock(L7_uint32 intIfNum);
static L7_RC_t cpimIntfStatusFinishBlock(L7_uint32 intIfNum);

static
cpimIntfStatusFinishFn
cpimIntfStatusFinishFnTable[CP_INTF_NUM_WORK_ITEMS] =
{
    cpimIntfStatusFinishAdd,
    cpimIntfStatusFinishAttach,
    cpimIntfStatusFinishDetach,
    cpimIntfStatusFinishDelete,
    cpimIntfStatusFinishUnblock,
    cpimIntfStatusFinishBlock
};

/*********************************************************************
*
* @purpose  Helper routine for adding an I/F to all of our status DBs
*
* @param    L7_uint32    intIfNum  @b{(input)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
static
L7_RC_t cpimIntfHelper(NIM_EVENT_COMPLETE_INFO_t * comp,
                       work_t work)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];

  do
  {
    if (!comp || (work >= CP_INTF_NUM_WORK_ITEMS))
      break;

    nimGetIntfName(comp->intIfNum, L7_SYSNAME, ifName);
    if (L7_SUCCESS != cpimIntfCapabilityFnTable[work](comp->intIfNum))
    {
      CP_DLOG(CPD_LEVEL_LOG, "intf capability error (work = %d, intf = %d, %s)", work,
              comp->intIfNum, ifName);
      break;
    }
    if (L7_SUCCESS != cpimIntfStatusFnTable[work](comp->intIfNum))
    {
      CP_DLOG(CPD_LEVEL_LOG, "intf status error (work = %d, intf = %d, %s)", work, comp->intIfNum, ifName);
      break;
    }
    if (L7_SUCCESS != intfWOAdd(work, comp))
    {
      CP_DLOG(CPD_LEVEL_LOG, wo_add_failed, work, comp->intIfNum, cplabel);
      break;
    }
    rc = captivePortalTaskWake();
    if (L7_SUCCESS != rc)
    {
      CP_DLOG(CPD_LEVEL_LOG, "Task wake failed (work = %d, intf = %d, %s)", work, comp->intIfNum, ifName);
      break;
    }
  } while(0);
  return rc;
}

/*********************************************************************
*
* @purpose  Add an interface to all of our status DBs
*
* @param    L7_uint32    intIfNum  @b{(input)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfAdd(L7_uint32 intIfNum)
{
  NIM_EVENT_COMPLETE_INFO_t comp;

  comp.intIfNum = intIfNum;
  comp.correlator = 0;
  return cpimIntfHelper(&comp, CP_INTF_FINISH_ADD);
}

/*********************************************************************
*
* @purpose  Add an interface to all of our status DBs
*
* @param    L7_uint32    intIfNum  @b{(input)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfDelete(L7_uint32 intIfNum)
{
  NIM_EVENT_COMPLETE_INFO_t comp;

  comp.intIfNum = intIfNum;
  comp.correlator = 0;
  return cpimIntfHelper(&comp, CP_INTF_FINISH_DELETE);
}


/*********************************************************************
*
* @purpose  Unblock an interface
*
* @param    L7_uint32    intIfNum  @b{(input)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfUnblock(L7_uint32 intIfNum)
{
  NIM_EVENT_COMPLETE_INFO_t comp;

  comp.intIfNum = intIfNum;
  comp.correlator = 0;
  return cpimIntfHelper(&comp, CP_INTF_FINISH_UNBLOCK);
}


/*********************************************************************
*
* @purpose  Block an interface
*
* @param    L7_uint32    intIfNum  @b{(input)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfBlock(L7_uint32 intIfNum)
{
  NIM_EVENT_COMPLETE_INFO_t comp;
  L7_RC_t rc;

  comp.intIfNum = intIfNum;
  comp.correlator = 0;
  rc = cpimIntfHelper(&comp, CP_INTF_FINISH_BLOCK);
  if (L7_SUCCESS==rc)
  {
    rc = cpdmClientConnStatusByIntfDeleteAll(intIfNum);
  }
  return rc;
}

/*********************************************************************
*
* @purpose  Helper routine for enable/disable of CP on an I/F
*
* @param    L7_uint32    intIfNum  @b{(input)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static
L7_RC_t cpimIntfEnableDisableHelper(L7_uint32 intIfNum, CP_OP_t op)
{
  cpRedirIpAddr_t ip;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 cap = L7_DISABLE;
  L7_uint32 portNum, ipMask = 0;
  L7_uchar8 intfMac[L7_ENET_MAC_ADDR_LEN];

#ifdef L7_MGMT_SECURITY_PACKAGE
  L7_uint32 securePort1Num;
  L7_uint32 securePort2Num;
#endif

  intfCpCallbacks_t * pCBs = cpimIntfOwnerCallbacksGet(intIfNum);

  if (!pCBs || !(pCBs->cpEnableDisableFeature))
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: FAILED cpEnableDisableFeature callback is not available", __FUNCTION__);
    return L7_FAILURE;
  }

  ip.redirIpAddrType = CP_IPV4;
  ip.ipAddr.redirIpv4Addr = (L7_IP_ADDR_t) 0;

  switch (op)
  {
  case CP_ENABLE:
    if (L7_SUCCESS != cpdmGlobalCPIPAddressGet(&(ip.ipAddr.redirIpv4Addr), &ipMask))
    {
      CP_DLOG(CPD_LEVEL_LOG, "No global IP");
      return L7_FAILURE;
    }

    if (cpdmGlobalCPMACAddressGet(&intfMac[0]) != L7_SUCCESS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "No interface MAC");
      return L7_FAILURE;
    }

    {
      L7_BOOL wireless = L7_FALSE, ready = L7_FALSE;
      if (L7_SUCCESS != cpimIntfIsReady(intIfNum, &wireless, &ready))
      {
        /*
        * Note no check for ready! We still allow it to be enabled because
        * it does no harm. We're good to go if and when the VAP becomes
        * enabled. It's much more of a hassle to "enable" the CP status
        * when the wireless interface is enabled.
        */
        if (wireless == L7_FALSE)
        {
           L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
           nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

           CP_DLOG(CPD_LEVEL_LOG, "Interface %d, %s not ready for use with captive portal", intIfNum, ifName);
           return L7_FAILURE;
        }
      }
    }
    cap = L7_ENABLE;
    break;
  case CP_DISABLE:
    break;
  default:
    return L7_FAILURE;
    break;
  }

  if (!(pCBs->cpCapabilitySet))
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: FAILED cpCapabilitySet callback is not available", __FUNCTION__);
    return L7_FAILURE;
  }

  rc = pCBs->cpCapabilitySet(intIfNum, L7_INTF_PARM_CP_MODE, cap);
  if (L7_FAILURE == rc)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: FAILED capability set on %sable", __FUNCTION__,
            op == CP_ENABLE ? "en" : "dis");
  }

  rc = cpdmGlobalHTTPPortGet(&portNum);
  if (L7_FAILURE == rc)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: FAILED in getting portNum", __FUNCTION__);
  }

  if (portNum != 0)
  {
    if (!(pCBs->cpSetAuthPort))
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: FAILED cpSetAuthPort callback is not available", __FUNCTION__);
      return L7_FAILURE;
    }
    rc = pCBs->cpSetAuthPort(intIfNum, portNum);
    if (L7_FAILURE == rc)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: FAILED in setting portNum", __FUNCTION__);
    }
  }

#ifdef L7_MGMT_SECURITY_PACKAGE
  {

    L7_uint32 unit;
    unit = usmDbThisUnitGet();

    /* Send the default auxiliary port that is configured for SSLT */
    rc = usmDbssltAuxSecurePort1Get(unit,&securePort1Num);
    if (L7_FAILURE == rc)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: FAILED in getting securePort1Num", __FUNCTION__);
    }

    if (!(pCBs->cpSetAuthSecurePort1))
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: FAILED cpSetAuthSecurePort1 callback is not available", __FUNCTION__);
      return L7_FAILURE;
    }

    rc = pCBs->cpSetAuthSecurePort1(intIfNum, securePort1Num);
    if (L7_FAILURE == rc)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: FAILED in setting securePort1Num", __FUNCTION__);
    }

    /* Send the additional auxiliary port that is configured for SSLT */
    rc = usmDbssltAuxSecurePort2Get(unit,&securePort2Num);
    if (L7_FAILURE == rc)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: FAILED in getting securePort2Num", __FUNCTION__);
    }

    if (!(pCBs->cpSetAuthSecurePort2))
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: FAILED cpSetAuthSecurePort2 callback is not available", __FUNCTION__);
      return L7_FAILURE;
    }
    rc = pCBs->cpSetAuthSecurePort2(intIfNum, securePort2Num);
    if (L7_FAILURE == rc)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: FAILED in setting securePort2Num", __FUNCTION__);
    }
  }
#endif

  rc = pCBs->cpEnableDisableFeature(op, intIfNum, ip, ipMask, intfMac);
  if (L7_FAILURE == rc)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: FAILED on %sable", __FUNCTION__, op == CP_ENABLE ? "en" : "dis");
  }
  return rc;
}


/*********************************************************************
*
* @purpose  Enable captive portal on an interface
*
* @param    L7_uint32    intIfNum  @b{(input)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/

L7_RC_t cpimIntfEnable(L7_uint32 intIfNum)
{
  return cpimIntfEnableDisableHelper(intIfNum, CP_ENABLE);
}

/*********************************************************************
*
* @purpose  Disable captive portal on an interface
*
* @param    L7_uint32    intIfNum  @b{(input)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/

L7_RC_t cpimIntfDisable(L7_uint32 intIfNum)
{
  return cpimIntfEnableDisableHelper(intIfNum, CP_DISABLE);
}


/*********************************************************************
*
* @purpose  Verify the existence of an interface
*
* @param    L7_uint32    intIfNum  @b{(input)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfGet(L7_uint32 intIfNum)
{
  return cpdmIntfStatusGet(intIfNum);
}

/*********************************************************************
*
* @purpose  Get the "next" interface that supports Captive Portal
*
* @param    L7_uint32  intIfNum       @b{(input)} interface id
* @param    L7_uint32  *nextIntIfNum  @b{(output)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfNextGet(L7_uint32 intIfNum, L7_uint32 * nextIntIfNum)
{
  return cpdmIntfStatusNextGet(intIfNum, nextIntIfNum);
}


/*********************************************************************
* @purpose  NIM callback for captive portal
*
* @param    L7_uint32 intIfnum @b{(input)} interface ID
* @param    L7_uint32 intIfEvent @b{(input)} interface event
* @param    NIM_CORRELATOR_t correlator @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only NIM should be calling this function.
*           If you're not the NIM task, don't even think about it.
*
* @end
*
*********************************************************************/

L7_RC_t cpimIntfChangeCallback(L7_uint32        intIfNum,
                               L7_uint32        intfEvent,
                               NIM_CORRELATOR_t correlator)
{
  NIM_EVENT_COMPLETE_INFO_t status;
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL  isCPenabled = L7_FALSE;
  L7_BOOL  ourEvent = L7_TRUE; /* Do we care about this event? */

  status.response.reason = 0;
  status.correlator = correlator;
  status.component = L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID;
  status.event = intfEvent;
  status.intIfNum = intIfNum;

  do
  {
    if (! captivePortalTaskActive())
    {
      rc = L7_SUCCESS; /* Whatever. */
      ourEvent = L7_FALSE;
      break;  /* Keep NIM happy, but get out ASAP */
    }

    if (L7_SUCCESS != cpimIntfSupportedGet(intIfNum, &isCPenabled))
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      CP_DLOG(CPD_LEVEL_LOG, "Can't determine if intf %d, %s supports Captive Portal", intIfNum, ifName);
      break;
    }

    if (L7_FALSE == isCPenabled)
    {
      rc = L7_SUCCESS; /* Not a CP-enabled interface, wave it through */
      break;
    }

    if (CP_IS_READY)
    {
      switch(intfEvent)
      {
      case L7_CREATE:
        rc = cpimIntfAddFromCallback(&status);
        break;

      case L7_DELETE:
        rc = cpimIntfDeleteFromCallback(&status);
        break;

      case L7_ATTACH:
      case L7_LAG_RELEASE:
        rc = cpimIntfAttachFromCallback(&status);
        break;

      case L7_DETACH:
      case L7_LAG_ACQUIRE:
        rc = cpimIntfDetachFromCallback(&status);
        break;

      default:
        rc = L7_SUCCESS; /* Whatever. */
        ourEvent = L7_FALSE;
        break;
      }
    }
    else
    {
      CP_DLOG(CPD_LEVEL_ERROR,
              "%s: Received intf change while in phase %u.", captivePortalCnfgrState);
    }
  } while (0);

  status.response.rc = L7_SUCCESS; /* Don't let NIM die.. yet */

  if (!ourEvent || (L7_FALSE == isCPenabled))
  {
      nimEventStatusCallback(status);
  }
  /* else don't sweat it, CP task will deal with it */
  return rc;
}

/*********************************************************************
* @purpose  Handle NIM startup callback
*
* @param    NIM_STARTUP_PHASE_t startupPhase @b{(input)} create/activate
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void cpimIntfStartupCallback(NIM_STARTUP_PHASE_t startupPhase)
{
  if (captivePortalCnfgrState != CAPTIVE_PORTAL_PHASE_EXECUTE)
  {
    nimStartupEventDone(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID);
    CP_DLOG(CPD_LEVEL_ERROR,
            "%s: Rcvd NIM startup %d callback in CP init state %d.",
            __FUNCTION__, startupPhase, captivePortalCnfgrState);
    return;
  }

  if (startupPhase == NIM_INTERFACE_CREATE_STARTUP)
  {
    (void)cpimNimCreateStartupProcess();
  }
  else if (startupPhase == NIM_INTERFACE_ACTIVATE_STARTUP)
  {
    (void)cpimNimActivateStartupProcess();
  }
  else
  {
    CP_DLOG(CPD_LEVEL_ERROR,
            "%s: Unknown NIM startup phase %d.", startupPhase);
  }
}

/*********************************************************************
* @purpose  Process NIM startup callback for interface create event
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @comments CP has no configuration at this point
*
* @end
*********************************************************************/
L7_RC_t cpimNimCreateStartupProcess(void)
{
  L7_uint32  intf;
  L7_BOOL    cpSupport;
  L7_RC_t    rc;
  NIM_EVENT_COMPLETE_INFO_t status;

  status.correlator = 0;
  rc = nimFirstValidIntfNumber(&intf);
  while (rc == L7_SUCCESS)
  {

    cpSupport = L7_FALSE;
    if ((cpimIntfSupportedGet(intf, &cpSupport) == L7_SUCCESS) &&
        (cpSupport == L7_TRUE))
    {
      status.intIfNum = intf;
      if (cpimIntfAddFromCallback(&status) != L7_SUCCESS)
      {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(intf, L7_SYSNAME, ifName);

        CP_DLOG(CPD_LEVEL_ERROR,
                "%s: Failed to create intf %u, %s.", __FUNCTION__, intf, ifName);
      }
    }
    rc = nimNextValidIntfNumber(intf, &intf);
  }

  /* set NIM event list to receive individual events */
  PORTEVENT_SETMASKBIT(cpdmOprData->nimEventMask, L7_CREATE);
  PORTEVENT_SETMASKBIT(cpdmOprData->nimEventMask, L7_DELETE);

  nimRegisterIntfEvents(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID, cpdmOprData->nimEventMask);
  nimStartupEventDone(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process NIM startup callback for interface activate event
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cpimNimActivateStartupProcess(void)
{
  L7_uint32         intf;
  L7_BOOL           cpSupport;
  L7_INTF_STATES_t  intfState;
  L7_RC_t           rc;

  NIM_EVENT_COMPLETE_INFO_t status;
  status.correlator = 0;

  rc = nimFirstValidIntfNumber(&intf);
  while (rc == L7_SUCCESS)
  {
    cpSupport = L7_FALSE;
    intfState = nimGetIntfState(intf);
    switch (intfState)
    {
      case L7_INTF_UNINITIALIZED:
      case L7_INTF_CREATING:
      case L7_INTF_CREATED:   /* already receiving this event in startup callback */
      case L7_INTF_DELETING:  /* already receiving this event in startup callback */
      case L7_INTF_ATTACHING:
      case L7_INTF_DETACHING:
        break;
      case L7_INTF_ATTACHED:
        if ((dot3adIsLagActiveMember(intf) != L7_TRUE) &&
            (cpimIntfSupportedGet(intf, &cpSupport) == L7_SUCCESS) &&
            (cpSupport == L7_TRUE))
        {
          status.intIfNum = intf;
          if (cpimIntfAttachFromCallback(&status) != L7_SUCCESS)
          {
            CP_DLOG(CPD_LEVEL_ERROR,
                    "%s: cpimIntfAttachFromCallback failed.", __FUNCTION__);
          }
        }
        break;
      default:
        CP_DLOG(CPD_LEVEL_ERROR,
                "%s: Received unknown NIM intfState %d.", __FUNCTION__, intfState);
        break;
    }
    rc = nimNextValidIntfNumber(intf, &intf);
  }

  /* set NIM event list to receive individual events (based on cpimIntfChangeCallback) */
  PORTEVENT_SETMASKBIT(cpdmOprData->nimEventMask, L7_ATTACH);
  PORTEVENT_SETMASKBIT(cpdmOprData->nimEventMask, L7_DETACH);
  PORTEVENT_SETMASKBIT(cpdmOprData->nimEventMask, L7_LAG_ACQUIRE);
  PORTEVENT_SETMASKBIT(cpdmOprData->nimEventMask, L7_LAG_RELEASE);

  nimRegisterIntfEvents(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID, cpdmOprData->nimEventMask);
  nimStartupEventDone(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID);

  if (cpdmOprData->cpWarmRestart == L7_TRUE)
  {
    #ifdef L7_NSF_PACKAGE
    L7_BOOL dataExists;
    cpCkptDataApply(&dataExists);
    if (dataExists == L7_TRUE)
    {
      cpdmOprData->cpCkptReqd = L7_TRUE;
    }
    cpdmOprData->cpWarmRestart = L7_FALSE;
    #endif
  }

  cnfgrApiComponentHwUpdateDone(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID, L7_CNFGR_HW_APPLY_CONFIG);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Return if captive portal is supported on an interface
*
*
* @param    L7_uint32 intIfNum, interface number in question
* @param    L7_BOOL *isCPEnabled, output
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/

L7_RC_t cpimIntfSupportedGet(L7_uint32 intIfNum, L7_BOOL * isCPEnabled)
{
  L7_RC_t rc = L7_FAILURE;
  nimUSP_t usp;
  L7_INTF_TYPES_t intfType;

  if (L7_NULLPTR == isCPEnabled)
  {
    return rc;
  }

  if ((L7_SUCCESS == nimGetUnitSlotPort(intIfNum, &usp)) &&
      (L7_SUCCESS == nimGetIntfTypeFromUSP(&usp, &intfType)))
  {
    rc = L7_SUCCESS;

#ifdef L7_WIRELESS_PACKAGE
    if (L7_WIRELESS_INTF == intfType)
     *isCPEnabled =  L7_TRUE;
#endif

#ifdef L7_CP_WIO_PACKAGE
    if (L7_PHYSICAL_INTF == intfType)
     *isCPEnabled =  L7_TRUE;
#endif
  }
  return rc;
}


/*********************************************************************
*
* @purpose  Return a pointer to the set of I/F owner callbacks
*
*
* @param    intIfNum, interface number in question
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Right now, our I/F parameter isn't used.  But it
*            probably will be someday soon as we expand CP to
*            use multiple interface types.
*
* @end
*
*********************************************************************/


intfCpCallbacks_t * cpimIntfOwnerCallbacksGet(L7_uint32 intIfNum)
{
  nimUSP_t usp;
  L7_INTF_TYPES_t intfType;
  intfCpCallbacks_t * pRet = L7_NULLPTR;

  if ((L7_SUCCESS == nimGetUnitSlotPort(intIfNum, &usp)) &&
      (L7_SUCCESS == nimGetIntfTypeFromUSP(&usp, &intfType)))
  {

    if (L7_WIRELESS_INTF == intfType)
    {
#ifdef L7_WIRELESS_PACKAGE
      wdmCpCallbackGet((void **) &pRet);
#endif
    }
    else if (L7_PHYSICAL_INTF == intfType)
    {
#ifdef L7_CP_WIO_PACKAGE
      wioCpCallbackGet((void**) &pRet);
#endif
    }
  }
  return pRet;
}


/*********************************************************************
*
* @purpose  Begin the addition of interfaces via NIM callback
*
*
* @param    intIfNum, interface number of NIM interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  ONLY called as part of the registered CP NIM callback!
*
* @end
*
*********************************************************************/

L7_RC_t cpimIntfAddFromCallback(NIM_EVENT_COMPLETE_INFO_t * comp)
{
  L7_RC_t   rc = L7_FAILURE;

  /* Add this I/F to our internal DB. */
  rc = cpimIntfHelper(comp, CP_INTF_FINISH_ADD);
  if (L7_SUCCESS != rc)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: NIM intf add failed.", cplabel);
  }
  return rc;
}


/*********************************************************************
*
* @purpose  Begin the addition of interfaces via NIM callback
*
*
* @param    intIfNum, interface number of NIM interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  ONLY called as part of the registered CP NIM callback!
*
* @end
*
*********************************************************************/

static
L7_RC_t cpimIntfDeleteFromCallback(NIM_EVENT_COMPLETE_INFO_t * comp)
{
  L7_RC_t rc = L7_SUCCESS;

  /* Delete this I/F FROM our internal DB */
  rc = cpimIntfHelper(comp, CP_INTF_FINISH_DELETE);
  if (L7_SUCCESS != rc)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: NIM intf delete failed.", cplabel);
  }
  return rc;
}



/*********************************************************************
*
* @purpose  Handle an ATTACH of a CP-enabled I/F via NIM callback
*
*
* @param    intIfNum, interface number of NIM interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  ONLY called as part of the registered CP NIM callback!
*
* @end
*
*********************************************************************/

static
L7_RC_t cpimIntfAttachFromCallback(NIM_EVENT_COMPLETE_INFO_t * comp)
{
  L7_RC_t rc = L7_SUCCESS;

  /* Mark this interface in our internal capability DB and
   * kick off the CP task.
   */

  rc = cpimIntfHelper(comp,CP_INTF_FINISH_ATTACH);
  if (L7_SUCCESS != rc)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: NIM intf attach failed.", cplabel);
  }
  return rc;
}

/*********************************************************************
*
* @purpose  Handle an DETACH of a CP-enabled I/F via NIM callback
*
*
* @param    intIfNum, interface number of NIM interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  ONLY called as part of the registered CP NIM callback!
*
* @end
*
*********************************************************************/

static
L7_RC_t cpimIntfDetachFromCallback(NIM_EVENT_COMPLETE_INFO_t * comp)
{
  L7_RC_t rc = L7_SUCCESS;

  /* Mark this interface in our internal capability DB and
   * kick off the CP task.
   */
  rc = cpimIntfHelper(comp,CP_INTF_FINISH_DETACH);
  if (L7_SUCCESS != rc)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: NIM intf detach failed.", cplabel);
  }
  return rc;
}


/*********************************************************************
*
* @purpose  Initialize our intf id/ capabStatus  tree
*
* @param    L7_uint32  maxCapabStatus    @b{(input)} max entries to support
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpIntfCapabStatusInit(L7_uint32 maxCapabStatus)
{
  L7_RC_t rc = L7_FAILURE;

  /* initialize our work order queue first.  No point continuing
   *  if we can't.
   *
   * NB: do not let the individual work items get any larger.  There
   * should be no reason whatsoever for an increase.  Do not enlarge
   * them.  Ever.
   */

  if (L7_SUCCESS != intfWOQInit(4 * CP_INTERFACE_MAX *
                                CP_INTF_NUM_WORK_ITEMS))
    LOG_ERROR(0);

  do
  {
    capabStatusTreeHeap = (avlTreeTables_t *)
      osapiMalloc(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
                  maxCapabStatus * sizeof(avlTreeTables_t));
    if (L7_NULLPTR == capabStatusTreeHeap)
    {
       break; /* no point in continuing */
    }
    capabStatusDataHeap = (void *)
      osapiMalloc(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
                  maxCapabStatus * sizeof(cpdmIntfCapabilityStatusData_t));
    if (L7_NULLPTR == capabStatusDataHeap)
    {
       break; /* no point in continuing */
    }
    avlCreateAvlTree(&capabStatusTree, capabStatusTreeHeap, capabStatusDataHeap,
                     maxCapabStatus, sizeof(cpdmIntfCapabilityStatusData_t),
                     0,  sizeof(L7_uint32));
    capabStatusTreeReady = L7_TRUE;
    currMaxCapabStatus = maxCapabStatus;
    rc = L7_SUCCESS;
  } while(0);

  if (L7_SUCCESS != rc)
  {
    if ( (L7_NULLPTR == capabStatusDataHeap) ||
         (L7_NULLPTR == capabStatusTreeHeap))
    {
      LOG_ERROR(0); /* no point in continuing */
    }
  }
  return rc;
}

/*********************************************************************
*
* @purpose  Finalize our intf id/ cp id capab tree
*
* @param    void
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/

void cpIntfCapabStatusFinalize(void)
{
  intfWOQFinalize(); /* accept NO new work! */
  SEMA_TAKE(WRITE);
  avlPurgeAvlTree(&capabStatusTree, currMaxCapabStatus);
  SEMA_GIVE(WRITE);
}


/*********************************************************************
*
* @purpose  Find a tree entry with the given intf id as a key
*
* @param    L7_uint32  intIfNum   @b{(input)} the key, of course
*
* @returns  void  pointer to cpdmIntfCapabilityStatusData_t (the value)
*
* @comments none  strictly for use within its own module!!!
*
* @end
*
*********************************************************************/
static
cpdmIntfCapabilityStatusData_t *
cpimFindCapabStatus(L7_uint32 intIfNum)
{
  if (!capabStatusTreeReady)
  {
    return L7_NULLPTR;
  }
  return avlSearchLVL7(&capabStatusTree, &intIfNum, AVL_EXACT);
}

/*********************************************************************
 *
 *  STATUS CAPABILITY ASSOCIATION LOOKUP FUNCTIONS BEGIN HERE
 *
 *********************************************************************/

/*********************************************************************
*
* @purpose  Get the next interface ID.
*
* @param    L7_uint32  intIfNum      @b{(input)} interface id
* @param    L7_uint32  nextIntfId  @b{(output)} next interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityNextGet(L7_uint32 intIfNum, L7_uint32 *nextIntfId)
{
  L7_RC_t rc = L7_FAILURE;
  cpdmIntfCapabilityStatusData_t *pEntry = L7_NULLPTR;

  if ((L7_NULLPTR == nextIntfId) || (! capabStatusTreeReady))
    return rc;

  SEMA_TAKE(READ);
  do
  {
    pEntry = avlSearchLVL7(&capabStatusTree, &intIfNum, AVL_NEXT);
    if (L7_NULLPTR == pEntry)
      break;
    *nextIntfId = pEntry->intfId;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);

  return rc;
}



/*********************************************************************
*
* @purpose  Delete the CP configuration from an interface.
*
* @param    L7_uint32    intIfNum  @b{(input)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityDelete(L7_uint32 intIfNum)
{
  /* Right now, not much more than a stub */
  if (! capabStatusTreeReady)
    return L7_FAILURE;
  return L7_SUCCESS;
}



/*********************************************************************
*
* @purpose  Add an interface to the capability status DB
*
* @param    L7_uint32    intIfNum  @b{(input)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Will probably only be called in the NIM task context.
*           Triggers an update in the CP task -- if you're calling
*           this from outside the NIM task, prepare for it to fail!
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityAdd(L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL isCPenabled = L7_FALSE;

  cpdmIntfCapabilityStatusData_t entry, *pEntry = L7_NULLPTR;

  if (! capabStatusTreeReady)
    return rc;

  if ((L7_SUCCESS != cpimIntfSupportedGet(intIfNum, &isCPenabled)) ||
      (L7_FALSE == isCPenabled))
    return rc;

  memset(&entry, 0, sizeof(entry));
  entry.intfId = intIfNum;
  entry.state = L7_CP_MODE_DISABLED;

  SEMA_TAKE(WRITE);
  do
  {
    pEntry = avlInsertEntry(&capabStatusTree, &entry);
    if (L7_NULLPTR != pEntry) /* if item not inserted */
    {
      if (&entry == pEntry)
        break; /* Item neither inserted nor present. Ouch! */
    }
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);

  return rc;
}


/*********************************************************************
*
* @purpose  "Attach" an interface to the capability status DB
*
* @param    L7_uint32    intIfNum  @b{(input)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Local helper function.  Not to be called externally.
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityAttach(L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL isCPenabled = L7_FALSE;
  cpdmIntfCapabilityStatusData_t *pEntry = L7_NULLPTR;

  if ((L7_SUCCESS != cpimIntfSupportedGet(intIfNum, &isCPenabled)) ||
      (L7_FALSE == isCPenabled))
  {
    return rc;
  }

  SEMA_TAKE(WRITE);
  do
  {
    /* Find the intf capability entry; bail if not found */
    pEntry = cpimFindCapabStatus(intIfNum);
    if (L7_NULLPTR == pEntry) /* if item not inserted */
      break;

    pEntry->state = L7_CP_MODE_DISABLED;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);
  return rc;
}


/*********************************************************************
*
* @purpose  "Unblock" an interface to the capability status DB
*
* @param    L7_uint32    intIfNum  @b{(input)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Local helper function.  Not to be called externally.
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityUnblock(L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL isCPenabled = L7_FALSE;
  cpdmIntfCapabilityStatusData_t *pEntry = L7_NULLPTR;

  if ((L7_SUCCESS != cpimIntfSupportedGet(intIfNum, &isCPenabled)) ||
      (L7_FALSE == isCPenabled))
  {
    return rc;
  }

  SEMA_TAKE(WRITE);
  do
  {
    /* Find the intf capability entry; bail if not found */
    pEntry = cpimFindCapabStatus(intIfNum);
    if (L7_NULLPTR == pEntry) /* if item not inserted */
      break;

    pEntry->state = L7_CP_MODE_ENABLE_PENDING;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);
  return rc;
}

/*********************************************************************
*
* @purpose  "Detach" an interface to the capability status DB
*
* @param    L7_uint32    intIfNum  @b{(input)} interface id
* @param    L7_BOOL      L7_TRUE if attaching,
*                        L7_FALSE if detaching
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Local helper function.  Not to be called externally.
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityDetach(L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL isCPenabled = L7_FALSE;
  cpdmIntfCapabilityStatusData_t *pEntry = L7_NULLPTR;

  if ((L7_SUCCESS != cpimIntfSupportedGet(intIfNum, &isCPenabled)) ||
      (L7_FALSE == isCPenabled))
  {
    return rc;
  }

  SEMA_TAKE(WRITE);
  do
  {
    /* Find the intf capability entry; bail if not found */
    pEntry = cpimFindCapabStatus(intIfNum);
    if (L7_NULLPTR == pEntry) /* if item not inserted */
      break;

    if ((L7_CP_MODE_ENABLED == pEntry->state) ||
          (L7_CP_MODE_ENABLE_PENDING == pEntry->state))
    {
       pEntry->state  = L7_CP_MODE_DISABLE_PENDING;
    }
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);
  return rc;
}

/*********************************************************************
*
* @purpose  "Disable" an interface to the capability status DB
*
* @param    L7_uint32    intIfNum  @b{(input)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Local helper function.  Not to be called externally.
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityBlock(L7_uint32 intIfNum)
{
  return cpimIntfCapabilityDetach(intIfNum);
}


/*********************************************************************
*
* @purpose  Complete pending addition of interfaces
*           to our capability database
*
* @param    L7_uint32 intIfNum -- corresponding interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This function is called ONLY from within this module,
*            and is ONLY a helper routine.
*
* @end
*
*********************************************************************/


L7_RC_t
cpimIntfCapabilityFinishAdd(L7_uint32 intIfNum, L7_PORT_EVENTS_t event)
{
  intfCpCallbacks_t  *pCBs  = L7_NULLPTR;
  L7_uint32       capVal = 0;
  L7_RC_t         rc     = L7_FAILURE;
  cpdmIntfCapabilityStatusData_t *pStat;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  pCBs = cpimIntfOwnerCallbacksGet(intIfNum);
  if (L7_NULLPTR == pCBs)
  {
      CP_DLOG(CPD_LEVEL_LOG, "Add: could not get callbacks for intf %d, %s", intIfNum, ifName);
      return L7_FAILURE;
  }

  /* ask interface owner for capabilities. If we don't get them,
   * mark all of them as disabled (capVal will be 0)
   */
  if (L7_NULLPTR != pCBs->cpCapabilityGet)
  {
    pCBs->cpCapabilityGet(intIfNum, L7_INTF_PARM_CP_ALL, &capVal);
  }
  else
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: No capabilities found for intf %d, %s", cplabel, ifName);
  }

  SEMA_TAKE(WRITE);
  do
  {
    pStat = cpimFindCapabStatus(intIfNum);
    if (L7_NULLPTR == pStat)
    {
      break;
    }

    pStat->blockStatus = L7_CP_INST_BLOCK_STATUS_NOT_BLOCKED;

    /* fill in the capabilities -- ignore the "all" flag in the
     * capability value (the legacy API is a little dain-bramaged).
     */
    pStat->sessionTimeout =
        (capVal & L7_INTF_PARM_CP_SESTIMEOUT)   ? L7_ENABLE : L7_DISABLE;
    pStat->idleTimeout =
        (capVal & L7_INTF_PARM_CP_IDLETIMEOUT)  ? L7_ENABLE : L7_DISABLE;
    pStat->bandwidthUpRateControl =
        (capVal & L7_INTF_PARM_CP_BWUPRATECTRL) ? L7_ENABLE : L7_DISABLE;
    pStat->bandwidthDownRateControl =
        (capVal & L7_INTF_PARM_CP_BWDNRATECTRL) ? L7_ENABLE : L7_DISABLE;
    pStat->maxInputOctetMonitor =
        (capVal & L7_INTF_PARM_CP_MAXINOCTMON)  ? L7_ENABLE : L7_DISABLE;
    pStat->maxOutputOctetMonitor =
        (capVal & L7_INTF_PARM_CP_MAXOUTOCTMON) ? L7_ENABLE : L7_DISABLE;
    pStat->bytesReceivedCounter =
        (capVal & L7_INTF_PARM_CP_BYTESRECVD)   ? L7_ENABLE : L7_DISABLE;
    pStat->bytesTransmittedCounter =
        (capVal & L7_INTF_PARM_CP_BYTESTXD)     ? L7_ENABLE : L7_DISABLE;
    pStat->packetsReceivedCounter =
        (capVal & L7_INTF_PARM_CP_PKTRECVD)     ? L7_ENABLE : L7_DISABLE;
    pStat->packetsTransmittedCounter =
        (capVal & L7_INTF_PARM_CP_PKTTXD)       ? L7_ENABLE : L7_DISABLE;
    pStat->roamingSupport =
        (capVal & L7_INTF_PARM_CP_ROAMING)      ? L7_ENABLE : L7_DISABLE;

    pStat->state =  L7_CP_MODE_DISABLED;

    if (L7_NULLPTR != pCBs->cpCapabilityGet)
    {
      rc = L7_SUCCESS;
    }
  } while(0);
  SEMA_GIVE(WRITE);

  if (L7_SUCCESS != rc)
  {
    CP_DLOG(CPD_LEVEL_LOG, "No capability get entry point for intf %d, %s", intIfNum, ifName);
  }
  return rc;
}


/*********************************************************************
*
* @purpose  Complete pending addition of interfaces
*           to our capability database
*
* @param    cpdmIntfCapabilityStatusData_t *pStat -- status data
* @param    L7_uint32 intIfNum -- corresponding interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This function is called ONLY from within this module,
*            and is ONLY a helper routine.  CALLER MUST SUPPLY
*            SEMAPHORE PROTECTION!
*
* @end
*
*********************************************************************/


L7_RC_t
cpimIntfCapabilitySetHelper(cpdmIntfCapabilityStatusData_t *pStat,
                            L7_uint32 intIfNum)
{
  intfCpCallbacks_t  *pCBs  = L7_NULLPTR;
  L7_uint32       capVal = 0;
  L7_RC_t         rc     = L7_FAILURE;

  if (L7_NULLPTR == pStat)
    return L7_FAILURE;
  pCBs = cpimIntfOwnerCallbacksGet(intIfNum);
  if ((L7_NULLPTR == pCBs) ||
      (L7_NULLPTR == pCBs->cpCapabilitySet))
    return L7_FAILURE;

  do
  {
    /* fill in the capabilities -- ignore the "all" flag in the
     * capability value.
     */

    if (L7_ENABLE == pStat->sessionTimeout)
        capVal |=  L7_INTF_PARM_CP_SESTIMEOUT;
    if (L7_ENABLE == pStat->idleTimeout)
        capVal |=  L7_INTF_PARM_CP_IDLETIMEOUT;
    if (L7_ENABLE == pStat->bandwidthUpRateControl)
        capVal |=  L7_INTF_PARM_CP_BWUPRATECTRL;
    if (L7_ENABLE == pStat->bandwidthDownRateControl)
        capVal |=  L7_INTF_PARM_CP_BWDNRATECTRL;
    if (L7_ENABLE == pStat->maxInputOctetMonitor)
        capVal |=  L7_INTF_PARM_CP_MAXINOCTMON;
    if (L7_ENABLE == pStat->maxOutputOctetMonitor)
        capVal |=  L7_INTF_PARM_CP_MAXOUTOCTMON;
    if (L7_ENABLE == pStat->bytesReceivedCounter)
        capVal |=  L7_INTF_PARM_CP_BYTESRECVD;
    if (L7_ENABLE == pStat->bytesTransmittedCounter)
        capVal |=  L7_INTF_PARM_CP_BYTESTXD;
    if (L7_ENABLE == pStat->packetsReceivedCounter)
        capVal |=  L7_INTF_PARM_CP_PKTRECVD;
    if (L7_ENABLE == pStat->packetsTransmittedCounter)
        capVal |=  L7_INTF_PARM_CP_PKTTXD;
    if (L7_ENABLE == pStat->roamingSupport)
        capVal |=  L7_INTF_PARM_CP_ROAMING;

    rc = pCBs->cpCapabilitySet(intIfNum, L7_INTF_PARM_CP_ALL, capVal);
  } while(0);

  if (rc == L7_FAILURE)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    CP_DLOG(CPD_LEVEL_LOG, "%s: No capabilities SET for intf %d", cplabel, intIfNum, ifName);
  }
  return rc;
}



/*********************************************************************
*
* @purpose  Complete pending deletion of interfaces
*           to our capability database
*
* @param    cpdmIntfCapabilityStatusData_t *pStat -- data to add
* @param    L7_uint32 intIfNum -- corresponding interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This function is called ONLY from within this module,
*            and is ONLY a helper routine.  Semaphore protection
*            IS ASSUMED TO BE IN FORCE HERE!!
* @end
*
*********************************************************************/


L7_RC_t
cpimIntfCapabilityFinishDelete(L7_uint32 intIfNum, L7_PORT_EVENTS_t event)
{
  cpdmIntfCapabilityStatusData_t stat, *pStat = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  stat.intfId = intIfNum;

  /* Check to see if item exists.  If not, say "we did it" to
   * the caller.  Otherwise, return status of deletion.
   */
  SEMA_TAKE(WRITE);
  do
  {
    pStat = avlSearchLVL7(&capabStatusTree, &intIfNum, AVL_EXACT);
    if (L7_NULLPTR == pStat)
    {
      rc = L7_SUCCESS;
      break;
    }
    pStat = avlDeleteEntry(&capabStatusTree, &stat);
    if (L7_NULLPTR == pStat)
      break;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);

  return rc;
}


/*********************************************************************
*
* @purpose  Complete pending addition of interfaces
*           to our capability database
*
* @param    L7_uint32 intIfNum -- corresponding interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This function is called ONLY from within this module,
*            and is ONLY a helper routine.
*
* @end
*
*********************************************************************/
static
L7_RC_t
cpimIntfCapabilityFinishAttach(L7_uint32 intIfNum, L7_PORT_EVENTS_t event)
{
    L7_uint32 mode = L7_CP_MODE_DISABLED;
    cpId_t cpId = 0;

    /*
     * LAG release events need special handling because we need to reinit the
     * HTTP ports, etc.
     */
    if (L7_LAG_RELEASE == event) {
      if (L7_SUCCESS != cpdmCPConfigIntIfNumFind(intIfNum,&cpId)) {
          return L7_SUCCESS;
      }
      if (L7_SUCCESS != cpdmCfgGlobalModeGet(&mode)) {
          return L7_SUCCESS;
      }
      if (L7_CP_MODE_ENABLED != mode) {
          return L7_SUCCESS;
      }
      cpdmCPConfigModeSet(cpId,L7_CP_MODE_ENABLED);
    }
    return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Complete pending addition of interfaces
*           to our capability database
*
* @param    L7_uint32 intIfNum -- corresponding interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This function is called ONLY from within this module,
*            and is ONLY a helper routine.
*
* @end
*
*********************************************************************/

static
L7_RC_t
cpimIntfCapabilityFinishUnblock(L7_uint32  intIfNum, L7_PORT_EVENTS_t event)
{
  cpdmIntfCapabilityStatusData_t * pStat = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;
  intfCpCallbacks_t  *pCBs  = L7_NULLPTR;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  pCBs = cpimIntfOwnerCallbacksGet(intIfNum);
  if (!pCBs || !pCBs->cpBlockUnblockAccess)
  {
    CP_DLOG(CPD_LEVEL_LOG, "Callback not found");
    return L7_FAILURE;
  }

  SEMA_TAKE(WRITE);
  do
  {
    pStat = cpimFindCapabStatus(intIfNum);
    if (L7_NULLPTR == pStat)
    {
      CP_DLOG(CPD_LEVEL_LOG, "no capability status for intf %d, %s", intIfNum, ifName);
      break;
    }
    rc = pCBs->cpBlockUnblockAccess(CP_UNBLOCK, intIfNum);
    if (L7_SUCCESS == rc)
    {
      pStat->blockStatus =  L7_CP_INST_BLOCK_STATUS_NOT_BLOCKED;
    }
    else
    {
      CP_DLOG(CPD_LEVEL_LOG, "call to unblock access on intf %d, %s FAILED", intIfNum, ifName);
      pStat->blockStatus =  L7_CP_INST_BLOCK_STATUS_BLOCKED;
    }
  } while(0);
  SEMA_GIVE(WRITE);

  return rc;
}


/*********************************************************************
*
* @purpose  Complete pending detach of interfaces to our capability
*           database. This means that in addition to disabling the
*           interface (via the callback), we'll also disable the CP
*           interface status and remove any authenticated clients.
*
* @param    L7_uint32 intIfNum -- corresponding interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This function is called ONLY from within this module,
*            and is ONLY a helper routine.
*
* @end
*
*********************************************************************/

static
L7_RC_t
cpimIntfCapabilityFinishDetach(L7_uint32 intIfNum, L7_PORT_EVENTS_t event)
{
    cpRedirIpAddr_t redirCopy;
    intfCpCallbacks_t  *pCBs  = L7_NULLPTR;
    L7_uchar8 intfMac[L7_ENET_MAC_ADDR_LEN];
    L7_uint32 mode = L7_CP_MODE_DISABLED;

    if (L7_SUCCESS != cpdmCfgGlobalModeGet(&mode)) {
        CP_DLOG(CPD_LEVEL_LOG, "Unable to get CP global status");
        return L7_SUCCESS;
    }

    if (L7_CP_MODE_ENABLED != mode)
    {
        CP_DLOG(CPD_LEVEL_LOG, "cpimIntfCapabilityFinishDetach: CP is not ready");
        return L7_SUCCESS;
    }

    pCBs = cpimIntfOwnerCallbacksGet(intIfNum);

    if (!pCBs) {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

        CP_DLOG(CPD_LEVEL_LOG, "Failed to get callbacks for interface: %d, %s", intIfNum, ifName);
        return L7_FAILURE;
    }

    if (! pCBs->cpEnableDisableFeature) {
        CP_DLOG(CPD_LEVEL_LOG, "Enab/disab callback not found");
        return L7_FAILURE;
    }

    cpcmAIPStatusCleanup(intIfNum);
    cpdmClientConnStatusByIntfDeleteAll(intIfNum);

    memset(&intfMac[0], 0, L7_ENET_MAC_ADDR_LEN);
    redirCopy.redirIpAddrType = CP_IPV4;
    redirCopy.ipAddr.redirIpv4Addr = (L7_IP_ADDR_t) 0;
    pCBs->cpEnableDisableFeature(CP_DISABLE, intIfNum, redirCopy, 0, intfMac);

    cpdmIntfStatusSetIsEnabled(intIfNum, L7_FALSE);

    return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Complete pending disabling interfaces
*           to our capability database
*
* @param    cpdmIntfCapabilityStatusData_t *pStat -- data to add
* @param    L7_uint32 intIfNum -- corresponding interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This function is called ONLY from within this module,
*            and is ONLY a helper routine.
*
* @end
*
*********************************************************************/

static
L7_RC_t
cpimIntfCapabilityFinishBlock(L7_uint32  intIfNum, L7_PORT_EVENTS_t event)
{
  cpdmIntfCapabilityStatusData_t * pStat = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;
  intfCpCallbacks_t  *pCBs  = L7_NULLPTR;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  pCBs = cpimIntfOwnerCallbacksGet(intIfNum);
  if (!pCBs || !pCBs->cpBlockUnblockAccess)
  {
    CP_DLOG(CPD_LEVEL_LOG, "Callback not found");
    return L7_FAILURE;
  }

  SEMA_TAKE(WRITE);
  do
  {
    pStat = cpimFindCapabStatus(intIfNum);
    if (L7_NULLPTR == pStat)
    {
      CP_DLOG(CPD_LEVEL_LOG, "no capability status for intf %d, %s", intIfNum, ifName);
      break;
    }
    rc = pCBs->cpBlockUnblockAccess(CP_BLOCK, intIfNum);
    if (L7_SUCCESS == rc)
    {
      pStat->blockStatus =  L7_CP_INST_BLOCK_STATUS_BLOCKED;
    }
    else
    {
      CP_DLOG(CPD_LEVEL_LOG, "Call to block access on intf %d, %s FAILED", intIfNum, ifName);
      pStat->blockStatus =  L7_CP_INST_BLOCK_STATUS_NOT_BLOCKED;
    }
  } while(0);
  SEMA_GIVE(WRITE);

  return rc;
}


/*********************************************************************
*
* @purpose  Complete pending work for our I/F-related databases
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This function is called ONLY from the CP task!
*
* @end
*
*********************************************************************/

L7_RC_t cpimIntfFinishWork(void)
{
  intfWorkOrder_t wo;
  L7_RC_t moreWork;
  L7_RC_t loop_rc = L7_SUCCESS, rc = L7_SUCCESS;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];

  moreWork = intfWOGet(&wo.work, &wo.comp);

  while (L7_SUCCESS == moreWork)
  {
    nimGetIntfName(wo.comp.intIfNum, L7_SYSNAME, ifName);

    if (wo.work >= CP_INTF_NUM_WORK_ITEMS)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s task: asked to finish \"%d\", an unknown work item",
              cplabel, wo.work);
      moreWork = intfWOGet(&wo.work, &wo.comp);
      continue;
    }

#if 0
    CP_DLOG(CPD_LEVEL_LOG,
      "%s task: asked for work item %d for intIfNum %d, %s", cplabel,
      wo.work, wo.comp.intIfNum, ifNam3);
#endif

    /* REAL WORK STARTS HERE */
    loop_rc = cpimIntfStatusFinishFnTable[wo.work](wo.comp.intIfNum);
    if (L7_SUCCESS == loop_rc)
    {
      loop_rc = cpimIntfCapabilityFinishFnTable[wo.work](wo.comp.intIfNum, wo.comp.event);
      if (L7_SUCCESS != loop_rc)
        CP_DLOG(CPD_LEVEL_LOG, "capab finish fn failed: work = %d, I/F = %d, %s",
          wo.work, wo.comp.intIfNum, ifName);
    }
    else
    {
      CP_DLOG(CPD_LEVEL_LOG, "status finish fn failed: work = %d, I/F = %d, %s",
        wo.work, wo.comp.intIfNum, ifName);
    }
    /* REAL WORK ENDS HERE */

    if (0 != wo.comp.correlator) /* if request came from NIM */
    {
      switch (wo.work)
      {
        case CP_INTF_FINISH_UNBLOCK:
        case CP_INTF_FINISH_BLOCK:
          /* NIM should never try these operations.  But if it does,
           * we'll try to abide WHILE SCREAMING ABOUT IT.
           */
          CP_DLOG(CPD_LEVEL_LOG, "%s: NIM trying work %d from callback!?", cplabel, wo.work);
          /* no break, MUST drop through */

        default:
          wo.comp.response.rc = L7_SUCCESS; /* Don't let NIM puke */
          wo.comp.response.reason = 0;
          wo.comp.component = L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID;
          nimEventStatusCallback(wo.comp);
          break;
      }
    }

    if (L7_SUCCESS != loop_rc)
    {
      rc = L7_FAILURE;
    }
    moreWork = intfWOGet(&wo.work, &wo.comp);
  }
  return rc;
}


/*********************************************************************
* @purpose  Deauthenticate client with MAC addr on the given interface
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_enetMacAddr_t * mac @b{(input)} client MAC
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfDeauthenticate(L7_uint32 intIfNum, L7_enetMacAddr_t * mac)
{
  intfCpCallbacks_t * pCBs = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  if (!mac)
    return rc;
  pCBs = cpimIntfOwnerCallbacksGet(intIfNum);
  if (!pCBs || !pCBs->cpDeauthenClient)
  {
    CP_DLOG(CPD_LEVEL_LOG, "Required callback for intf %d, %s not found", intIfNum, ifName);
    return rc;
  }
  rc = pCBs->cpDeauthenClient(*mac);
  if (L7_FAILURE == rc)
  {
    CP_DLOG(CPD_LEVEL_LOG, "Callback for deauth FAILED, intf %d, %s", intIfNum, ifName);
  }
  return rc;
}



/*********************************************************************
* @purpose  Get statistics for a given interface
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_enetMacAddr_t *mac @b{(input)} MAC address
* @param    txRxCount_t * txRx @b{(output)} gathered statistics
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimGetStatistics(L7_uint32 intIfNum,
                          L7_enetMacAddr_t * macAddr,
                          txRxCount_t * txRx)
{
  intfCpCallbacks_t * pCBs = L7_NULLPTR;
  L7_uint32 ignored;
  L7_RC_t rc = L7_FAILURE;

  do
  {
    if (!txRx || !macAddr)
      break;

    pCBs = cpimIntfOwnerCallbacksGet(intIfNum);
    if (!pCBs || !pCBs->cpClientStatisticsGet)
    {
      break;
    }

    rc = pCBs->cpClientStatisticsGet(*macAddr, txRx, &ignored);
   /* This is NOT NECESSARILY an error.  For wireless connections,
    * it could be that our AP cycled power and is just coming back.
    * Let's not fill up the log for the time being.
    if (L7_FAILURE == rc)
    {
      CP_DLOG(CPD_LEVEL_LOG, "callback failed (can't get stats)");
    }
    */
  }while(0);
  return rc;
}

/*********************************************************************
* @purpose  Get interface capability field: state
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_CP_MODE_STATUS_t *state @b{(output)} state of
*           interface capability item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityStateGet(L7_uint32 intIfNum, L7_CP_MODE_STATUS_t *state)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == state)
  {
     return rc;
  }
  SEMA_TAKE(READ);
  do {
    cpdmIntfCapabilityStatusData_t *pStat = cpimFindCapabStatus(intIfNum);
    if (L7_NULLPTR == pStat)   break;
    *state = pStat->state;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);
  return rc;
}

/*********************************************************************
* @purpose  Get interface capability field: blockStatus
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_CP_INST_BLOCK_STATUS_t *state @b{(output)} state of
*           interface capability item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityBlockStatusGet(L7_uint32 intIfNum, L7_CP_INST_BLOCK_STATUS_t *blockStatus)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == blockStatus)
  {
     return rc;
  }
  SEMA_TAKE(READ);
  do {
    cpdmIntfCapabilityStatusData_t *pStat = cpimFindCapabStatus(intIfNum);
    if (L7_NULLPTR == pStat)   break;
    *blockStatus = pStat->blockStatus;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);
  return rc;
}

/*********************************************************************
* @purpose  Get interface capability field: sessionTimeout
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_uchar8 *sessionTimeout @b{(output)} sessionTimeout of
*           interface capability item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilitySessionTimeoutGet(L7_uint32 intIfNum, L7_uchar8 *sessionTimeout)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == sessionTimeout)
  {
     return rc;
  }
  SEMA_TAKE(READ);
  do {
    cpdmIntfCapabilityStatusData_t *pStat = cpimFindCapabStatus(intIfNum);
    if (L7_NULLPTR == pStat)   break;
    *sessionTimeout = pStat->sessionTimeout;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);
  return rc;
}

/*********************************************************************
* @purpose  Get interface capability field: idleTimeout
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_uchar8 *idleTimeout @b{(output)} idleTimeout of
*           interface capability item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityIdleTimeoutGet(L7_uint32 intIfNum, L7_uchar8 *idleTimeout)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == idleTimeout)
  {
     return rc;
  }
  SEMA_TAKE(READ);
  do {
    cpdmIntfCapabilityStatusData_t *pStat = cpimFindCapabStatus(intIfNum);
    if (L7_NULLPTR == pStat)   break;
    *idleTimeout = pStat->idleTimeout;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);
  return rc;
}

/*********************************************************************
* @purpose  Get interface capability field: bandwidthUpRateControl
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_uchar8 *bandwidthUpRateControl @b{(output)} bandwidthUpRateControl of
*           interface capability item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityBandwidthUpRateControlGet(L7_uint32 intIfNum,
                                    L7_uchar8 *bandwidthUpRateControl)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == bandwidthUpRateControl)
  {
     return rc;
  }
  SEMA_TAKE(READ);
  do {
    cpdmIntfCapabilityStatusData_t *pStat = cpimFindCapabStatus(intIfNum);
    if (L7_NULLPTR == pStat)   break;
    *bandwidthUpRateControl = pStat->bandwidthUpRateControl;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);
  return rc;
}

/*********************************************************************
* @purpose  Get interface capability field: bandwidthDownRateControl
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_uchar8 *bandwidthDownRateControl @b{(output)} bandwidthDownRateControl of
*           interface capability item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityBandwidthDownRateControlGet(L7_uint32 intIfNum,
                                  L7_uchar8 *bandwidthDownRateControl)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == bandwidthDownRateControl)
  {
     return rc;
  }
  SEMA_TAKE(READ);
  do {
    cpdmIntfCapabilityStatusData_t *pStat = cpimFindCapabStatus(intIfNum);
    if (L7_NULLPTR == pStat)   break;
    *bandwidthDownRateControl = pStat->bandwidthDownRateControl;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);
  return rc;
}

/*********************************************************************
* @purpose  Get interface capability field: maxInputOctetMonitor
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_uchar8 *maxInputOctetMonitor @b{(output)} maxInputOctetMonitor of
*           interface capability item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
 L7_RC_t cpimIntfCapabilityMaxInputOctetMonitorGet(L7_uint32 intIfNum,
                                                   L7_uchar8 *maxInputOctetMonitor)
 {
   L7_RC_t rc = L7_FAILURE;

   if (L7_NULLPTR == maxInputOctetMonitor)
     {
       return rc;
     }
   SEMA_TAKE(READ);
   do {
     cpdmIntfCapabilityStatusData_t *pStat = cpimFindCapabStatus(intIfNum);
     if (L7_NULLPTR == pStat)   break;
     *maxInputOctetMonitor = pStat->maxInputOctetMonitor;
     rc = L7_SUCCESS;
   } while(0);
   SEMA_GIVE(READ);
   return rc;
 }

 /*********************************************************************
  * @purpose  Get interface capability field: maxOutputOctetMonitor
  *
  * @param    L7_uint32 intIfNum @b{(input)} interface ID
  * @param    L7_uchar8 *maxOutputOctetMonitor @b{(output)} maxOutputOctetMonitor of
  *           interface capability item with given ID
  *
  * @returns  L7_SUCCESS
  * @returns  L7_FAILURE
  *
  * @comments none
  *
  * @end
  *
  *********************************************************************/
 L7_RC_t cpimIntfCapabilityMaxOutputOctetMonitorGet(L7_uint32 intIfNum,
                                                    L7_uchar8 *maxOutputOctetMonitor)
 {
   L7_RC_t rc = L7_FAILURE;

   if (L7_NULLPTR == maxOutputOctetMonitor)
     {
       return rc;
     }
   SEMA_TAKE(READ);
   do {
     cpdmIntfCapabilityStatusData_t *pStat = cpimFindCapabStatus(intIfNum);
     if (L7_NULLPTR == pStat)   break;
     *maxOutputOctetMonitor = pStat->maxOutputOctetMonitor;
     rc = L7_SUCCESS;
   } while(0);
   SEMA_GIVE(READ);
   return rc;
 }

/*********************************************************************
* @purpose  Get interface capability field: bytesReceivedCounter
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_uchar8 *bytesReceivedCounter @b{(output)} bytesReceivedCounter of
*           interface capability item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityBytesReceivedCounterGet(L7_uint32 intIfNum,
                                   L7_uchar8 *bytesReceivedCounter)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == bytesReceivedCounter)
  {
     return rc;
  }
  SEMA_TAKE(READ);
  do {
    cpdmIntfCapabilityStatusData_t *pStat = cpimFindCapabStatus(intIfNum);
    if (L7_NULLPTR == pStat)   break;
    *bytesReceivedCounter = pStat->bytesReceivedCounter;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);
  return rc;
}

/*********************************************************************
* @purpose  Get interface capability field: bytesTransmittedCounter
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_uchar8 *bytesTransmittedCounter @b{(output)} bytesTransmittedCounter of
*           interface capability item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityBytesTransmittedCounterGet(L7_uint32 intIfNum, L7_uchar8 *bytesTransmittedCounter)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == bytesTransmittedCounter)
  {
     return rc;
  }
  SEMA_TAKE(READ);
  do {
    cpdmIntfCapabilityStatusData_t *pStat = cpimFindCapabStatus(intIfNum);
    if (L7_NULLPTR == pStat)   break;
    *bytesTransmittedCounter = pStat->bytesTransmittedCounter;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);
  return rc;
}

/*********************************************************************
* @purpose  Get interface capability field: packetsReceivedCounter
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_uchar8 *packetsReceivedCounter @b{(output)} packetsReceivedCounter of
*           interface capability item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityPacketsReceivedCounterGet(L7_uint32 intIfNum, L7_uchar8 *packetsReceivedCounter)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == packetsReceivedCounter)
  {
     return rc;
  }
  SEMA_TAKE(READ);
  do {
    cpdmIntfCapabilityStatusData_t *pStat = cpimFindCapabStatus(intIfNum);
    if (L7_NULLPTR == pStat)   break;
    *packetsReceivedCounter = pStat->packetsReceivedCounter;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);
  return rc;
}

/*********************************************************************
* @purpose  Get interface capability field: packetsTransmittedCounter
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_uchar8 *packetsTransmittedCounter @b{(output)} packetsTransmittedCounter of
*           interface capability item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityPacketsTransmittedCounterGet(L7_uint32 intIfNum, L7_uchar8 *packetsTransmittedCounter)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == packetsTransmittedCounter)
  {
     return rc;
  }
  SEMA_TAKE(READ);
  do {
    cpdmIntfCapabilityStatusData_t *pStat = cpimFindCapabStatus(intIfNum);
    if (L7_NULLPTR == pStat)   break;
    *packetsTransmittedCounter = pStat->packetsTransmittedCounter;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);
  return rc;
}

/*********************************************************************
* @purpose  Get interface capability field: roamingSupport
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_uchar8 *roamingSupport @b{(output)} roamingSupport of
*           interface capability item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityRoamingSupportGet(L7_uint32 intIfNum, L7_uchar8 *roamingSupport)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == roamingSupport)
  {
     return rc;
  }
  SEMA_TAKE(READ);
  do {
    cpdmIntfCapabilityStatusData_t *pStat = cpimFindCapabStatus(intIfNum);
    if (L7_NULLPTR == pStat)   break;
    *roamingSupport = pStat->roamingSupport;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);
  return rc;
}



/*********************************************************************
* @purpose  Get interface capability field: description
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_uchar8 *description @b{(output)} description of
*           interface capability item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityDescriptionGet(L7_uint32 intIfNum, L7_uchar8 *description)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULLPTR == description)
  {
     return rc;
  }

  SEMA_TAKE(WRITE); /* NOT read this time, believe it or not */
  do
  {
    cpdmIntfCapabilityStatusData_t *pStat = cpimFindCapabStatus(intIfNum);
    if ((L7_NULLPTR == pStat) ||
        (L7_SUCCESS != nimGetIfDescr(intIfNum, description)))
    {
      break;
    }
    rc = L7_SUCCESS;
  } while (0);
  SEMA_GIVE(WRITE);

  return rc;
}

/*********************************************************************
* @purpose  Get interface capability field: sessionTimeout
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_uchar8 sessionTimeout @b{(output)} sessionTimeout of interface capability
*                     item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilitySessionTimeoutSet(L7_uint32 intIfNum,
                                            L7_uchar8 sessionTimeout)
{
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);
  do {
    L7_uchar8 oldVal;
    cpdmIntfCapabilityStatusData_t *pStat = cpimFindCapabStatus(intIfNum);
    if (L7_NULLPTR == pStat)   break;
    oldVal = pStat->sessionTimeout;
    pStat->sessionTimeout = sessionTimeout;
    if (L7_SUCCESS != cpimIntfCapabilitySetHelper(pStat, intIfNum))
    {
      pStat->sessionTimeout = oldVal;
      (void) cpimIntfCapabilitySetHelper(pStat, intIfNum);
      break;
    }
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);
  return rc;
}


/*********************************************************************
* @purpose  Get interface capability field: idleTimeout
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_uchar8 idleTimeout @b{(output)} idleTimeout of interface capability
*                     item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityIdleTimeoutSet(L7_uint32 intIfNum,
                                         L7_uchar8 idleTimeout)
{
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);
  do {
    L7_uchar8 oldVal;
    cpdmIntfCapabilityStatusData_t *pStat = cpimFindCapabStatus(intIfNum);
    if (L7_NULLPTR == pStat)   break;
    oldVal = pStat->idleTimeout;
    pStat->idleTimeout = idleTimeout;
    if (L7_SUCCESS != cpimIntfCapabilitySetHelper(pStat, intIfNum))
    {
      pStat->idleTimeout = oldVal;
      (void) cpimIntfCapabilitySetHelper(pStat, intIfNum);
      break;
    }
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);
  return rc;
}


/*********************************************************************
* @purpose  Get interface capability field: bandwidthUpRateControl
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_uchar8 bandwidthUpRateControl @b{(output)}
*            bandwidthUpRateControl of interface capability
*             item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityBandwidthUpRateControlSet(L7_uint32 intIfNum,
                                                    L7_uchar8 bandwidthUpRateControl)
{
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);
  do {
    L7_uchar8 oldVal;
    cpdmIntfCapabilityStatusData_t *pStat = cpimFindCapabStatus(intIfNum);
    if (L7_NULLPTR == pStat)   break;
    oldVal = pStat->bandwidthUpRateControl;
    pStat->bandwidthUpRateControl = bandwidthUpRateControl;
    if (L7_SUCCESS != cpimIntfCapabilitySetHelper(pStat, intIfNum))
    {
      pStat->bandwidthUpRateControl = oldVal;
      (void) cpimIntfCapabilitySetHelper(pStat, intIfNum);
      break;
    }
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);
  return rc;
}
/*********************************************************************
* @purpose  Get interface capability field: bandwidthDownRateControl
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_uchar8 bandwidthDownRateControl @b{(output)} bandwidthDownRateControl of interface capability
*                     item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityBandwidthDownRateControlSet(L7_uint32 intIfNum,
                                      L7_uchar8 bandwidthDownRateControl)
{
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);
  do {
    L7_uchar8 oldVal;
    cpdmIntfCapabilityStatusData_t *pStat = cpimFindCapabStatus(intIfNum);
    if (L7_NULLPTR == pStat)   break;
    oldVal = pStat->bandwidthDownRateControl;
    pStat->bandwidthDownRateControl = bandwidthDownRateControl;
    if (L7_SUCCESS != cpimIntfCapabilitySetHelper(pStat, intIfNum))
    {
      pStat->bandwidthDownRateControl = oldVal;
      (void) cpimIntfCapabilitySetHelper(pStat, intIfNum);
      break;
    }
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);
  return rc;
}


/*********************************************************************
* @purpose  Get interface capability field: maxInputOctetMonitor
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_uchar8 maxInputOctetMonitor @b{(output)} maxInputOctetMonitor of interface capability
*                     item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityMaxInputOctetMonitorSet(L7_uint32 intIfNum,
                                        L7_uchar8 maxInputOctetMonitor)
{
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);
  do {
    L7_uchar8 oldVal;
    cpdmIntfCapabilityStatusData_t *pStat = cpimFindCapabStatus(intIfNum);
    if (L7_NULLPTR == pStat)   break;
    oldVal = pStat->maxInputOctetMonitor;
    pStat->maxInputOctetMonitor = maxInputOctetMonitor;
    if (L7_SUCCESS != cpimIntfCapabilitySetHelper(pStat, intIfNum))
    {
      pStat->maxInputOctetMonitor = oldVal;
      (void) cpimIntfCapabilitySetHelper(pStat, intIfNum);
      break;
    }
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);
  return rc;
}


/*********************************************************************
* @purpose  Get interface capability field: maxOutputOctetMonitor
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_uchar8 maxOutputOctetMonitor @b{(output)}
*             maxOutputOctetMonitor of interface capability
*             item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityMaxOutputOctetMonitorSet(L7_uint32 intIfNum,
                                       L7_uchar8 maxOutputOctetMonitor)
{
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);
  do {
    L7_uchar8 oldVal;
    cpdmIntfCapabilityStatusData_t *pStat = cpimFindCapabStatus(intIfNum);
    if (L7_NULLPTR == pStat)   break;
    oldVal = pStat->maxOutputOctetMonitor;
    pStat->maxOutputOctetMonitor = maxOutputOctetMonitor;
    if (L7_SUCCESS != cpimIntfCapabilitySetHelper(pStat, intIfNum))
    {
      pStat->maxOutputOctetMonitor = oldVal;
      (void) cpimIntfCapabilitySetHelper(pStat, intIfNum);
      break;
    }
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);
  return rc;
}


/*********************************************************************
* @purpose  Get interface capability field: bytesReceivedCounter
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_uchar8 bytesReceivedCounter @b{(output)} bytesReceivedCounter
*               of interface capability item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityBytesReceivedCounterSet(L7_uint32 intIfNum,
                                      L7_uchar8 bytesReceivedCounter)
{
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);
  do {
    L7_uchar8 oldVal;
    cpdmIntfCapabilityStatusData_t *pStat = cpimFindCapabStatus(intIfNum);
    if (L7_NULLPTR == pStat)   break;
    oldVal = pStat->bytesReceivedCounter;
    pStat->bytesReceivedCounter = bytesReceivedCounter;
    if (L7_SUCCESS != cpimIntfCapabilitySetHelper(pStat, intIfNum))
    {
      pStat->bytesReceivedCounter = oldVal;
      (void) cpimIntfCapabilitySetHelper(pStat, intIfNum);
      break;
    }
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);
  return rc;
}


/*********************************************************************
* @purpose  Get interface capability field: bytesTransmittedCounter
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_uchar8 bytesTransmittedCounter @b{(output)}
*              bytesTransmittedCounter of interface capability
*              item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityBytesTransmittedCounterSet(L7_uint32 intIfNum,
                                      L7_uchar8 bytesTransmittedCounter)
{
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);
  do {
    L7_uchar8 oldVal;
    cpdmIntfCapabilityStatusData_t *pStat = cpimFindCapabStatus(intIfNum);
    if (L7_NULLPTR == pStat)   break;
    oldVal = pStat->bytesTransmittedCounter;
    pStat->bytesTransmittedCounter = bytesTransmittedCounter;
    if (L7_SUCCESS != cpimIntfCapabilitySetHelper(pStat, intIfNum))
    {
      pStat->bytesTransmittedCounter = oldVal;
      (void) cpimIntfCapabilitySetHelper(pStat, intIfNum);
      break;
    }
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);
  return rc;
}


/*********************************************************************
* @purpose  Get interface capability field: packetsReceivedCounter
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_uchar8 packetsReceivedCounter @b{(output)}
*              packetsReceivedCounter of interface capability
*                     item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityPacketsReceivedCounterSet(L7_uint32 intIfNum,
                                      L7_uchar8 packetsReceivedCounter)
{
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);
  do {
    L7_uchar8 oldVal;
    cpdmIntfCapabilityStatusData_t *pStat = cpimFindCapabStatus(intIfNum);
    if (L7_NULLPTR == pStat)   break;
    oldVal = pStat->packetsReceivedCounter;
    pStat->packetsReceivedCounter = packetsReceivedCounter;
    if (L7_SUCCESS != cpimIntfCapabilitySetHelper(pStat, intIfNum))
    {
      pStat->packetsReceivedCounter = oldVal;
      (void) cpimIntfCapabilitySetHelper(pStat, intIfNum);
      break;
    }
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);
  return rc;
}


/*********************************************************************
* @purpose  Get interface capability field: packetsTransmittedCounter
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_uchar8 packetsTransmittedCounter @b{(output)}
*              packetsTransmittedCounter of interface capability
*                     item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityPacketsTransmittedCounterSet(L7_uint32 intIfNum,
                                      L7_uchar8 packetsTransmittedCounter)
{
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);
  do {
    L7_uchar8 oldVal;
    cpdmIntfCapabilityStatusData_t *pStat = cpimFindCapabStatus(intIfNum);
    if (L7_NULLPTR == pStat)   break;
    oldVal = pStat->packetsTransmittedCounter;
    pStat->packetsTransmittedCounter = packetsTransmittedCounter;
    if (L7_SUCCESS != cpimIntfCapabilitySetHelper(pStat, intIfNum))
    {
      pStat->packetsTransmittedCounter = oldVal;
      (void) cpimIntfCapabilitySetHelper(pStat, intIfNum);
      break;
    }
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);
  return rc;
}



/*********************************************************************
* @purpose  Get interface capability field: roamingSupport
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_uchar8 roamingSupport @b{(output)}
*              roamingSupport of interface capability
*                     item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityRoamingSupportSet(L7_uint32 intIfNum,
                                      L7_uchar8 roamingSupport)
{
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);
  do {
    L7_uchar8 oldVal;
    cpdmIntfCapabilityStatusData_t *pStat = cpimFindCapabStatus(intIfNum);
    if (L7_NULLPTR == pStat)   break;
    oldVal = pStat->roamingSupport;
    pStat->roamingSupport = roamingSupport;
    if (L7_SUCCESS != cpimIntfCapabilitySetHelper(pStat, intIfNum))
    {
      pStat->roamingSupport = oldVal;
      (void) cpimIntfCapabilitySetHelper(pStat, intIfNum);
      break;
    }
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);
  return rc;
}



/*************************************************************************
 *
 *   CPIM API -- used by both NIM callbacks and other captive portal
 *   components
 *
 **************************************************************************/

/*********************************************************************
*
* @purpose  Complete pending addition of interfaces
*           to our status database
*
* @param    L7_uint32 intIfNum -- corresponding interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This function is called ONLY from within this module,
*            and is ONLY a helper routine.
*            CALLER MUST PROVIDE SEMAPHORE PROTECTION!
*
* @end
*
*********************************************************************/

static
L7_RC_t
cpimIntfStatusFinishAdd(L7_uint32   intIfNum)
{
    /* For D-link, fake it */
  (void) cpdmIntfStatusSetWhy(intIfNum, L7_INTF_WHY_DISABLED_UNATTACHED);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Complete pending deletion of interfaces
*           to our status database.  Also, wipe out any
*           CP/Interface association we may have
*
* @param    L7_uint32 intIfNum -- corresponding interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This function is called ONLY from within this module,
*            and is ONLY a helper routine.
*            Caller or called functions must provide sema protection!
* @end
*
*********************************************************************/

static
L7_RC_t
cpimIntfStatusFinishDelete(L7_uint32  intIfNum)
{
  cpId_t cpId = 0;
  L7_RC_t rc = L7_SUCCESS;

  if (L7_SUCCESS == cpdmCPConfigIntIfNumFind(intIfNum, &cpId))
  {
    rc = cpdmCPConfigIntIfNumDelete(cpId, intIfNum);
  }
  return rc;
}


/*********************************************************************
*
* @purpose  Complete pending attach/enable of interfaces
*           to our status database
*
* @param    cpdmIntfCapabilityStatusData_t *pStat -- for NIM
*                                                    callbacks
* @param    L7_uint32 intIfNum -- corresponding interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This function is called ONLY from within this module,
*            and is ONLY a helper routine.
*
* @end
*
*********************************************************************/

static
L7_RC_t
cpimIntfStatusFinishUnblock( L7_uint32  intIfNum)
{
  return L7_SUCCESS; /* no meaning in this context */
}



/*********************************************************************
*
* @purpose  Complete pending enable of interfaces
*           to our status database
*
* @param    L7_uint32 intIfNum -- corresponding interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This function is called ONLY from within this module,
*            and is ONLY a helper routine.
*            CALLER MUST PROVIDE SEMAPHORE PROTECTION!
*
* @end
*
*********************************************************************/

static
L7_RC_t
cpimIntfStatusFinishAttach(L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_FAILURE;

  rc = cpdmIntfStatusSetWhy(intIfNum,
                            L7_INTF_WHY_DISABLED_ADMIN);
  return rc;
}



/*********************************************************************
*
* @purpose  Complete pending detach/disable of interfaces
*           to our status database
*
* @param    L7_uint32 intIfNum -- corresponding interface number
* @param    cpdmIntfCapabilityStatusData_t *pStat -- I/F stat entry
* @param    L7_INTF_WHY_DISABLED_t reason -- reason to give
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This function is called ONLY from within this module,
*            and is ONLY a helper routine.
*            CALLER MUST PROVIDE SEMAPHORE PROTECTION!
*
* @end
*
*********************************************************************/
static L7_RC_t
cpimIntfStatusFinishDetachHelper(L7_uint32  intIfNum)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  do
  {
    if (L7_SUCCESS != cpdmIntfStatusGet(intIfNum))
    {
      CP_DLOG(CPD_LEVEL_LOG, "Can't get status for I/F %d, %s", intIfNum, ifName);
      break;
    }
    if (L7_SUCCESS != cpdmIntfStatusSetUsers(intIfNum, 0))
    {
      CP_DLOG(CPD_LEVEL_LOG, "Can't set user count for I/F %d, %s", intIfNum, ifName);
      break;
    }
    if (L7_SUCCESS != cpdmIntfStatusSetIsEnabled(intIfNum, L7_FALSE))
    {
      CP_DLOG(CPD_LEVEL_LOG, "Can't set status to Disabled for I/F %d, %s", intIfNum, ifName);
      break;
    }
    if (L7_SUCCESS != cpdmIntfStatusSetWhy(intIfNum,
                           L7_INTF_WHY_DISABLED_UNATTACHED))
    {
      CP_DLOG(CPD_LEVEL_LOG, "Can't set reason for disabling for I/F %d, %s", intIfNum, ifName);
      break;
    }
    rc = L7_SUCCESS;
  } while(0);

  if (L7_SUCCESS != rc)
  {
    CP_DLOG(CPD_LEVEL_LOG, "%s: couldn't set detached interface status", cplabel);
  }
  return rc;
}


/*********************************************************************
*
* @purpose  Complete pending detach of interfaces
*           to our status database
*
* @param    L7_uint32 intIfNum -- corresponding interface number
* @param    cpdmIntfCapabilityStatusData_t *pStat -- I/F stat entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This function is called ONLY from within this module,
*            and is ONLY a helper routine.
*            CALLER MUST PROVIDE SEMAPHORE PROTECTION!
*
* @end
*
*********************************************************************/
static L7_RC_t
cpimIntfStatusFinishDetach(L7_uint32  intIfNum)
{
  L7_RC_t rc = L7_FAILURE;
  L7_INTF_WHY_DISABLED_t reason;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  rc = cpdmIntfStatusGetWhy(intIfNum, &reason);
  if (L7_SUCCESS != rc)
  {
    CP_DLOG(CPD_LEVEL_LOG, "Can't get reason for disabling for I/F %d, %s", intIfNum, ifName);
  }
  else
  {
    switch (reason)
    {
    case L7_INTF_WHY_DISABLED_ADMIN:
     /* Else just set our state to "unattached" */
     rc = cpdmIntfStatusSetWhy(intIfNum, L7_INTF_WHY_DISABLED_UNATTACHED);
     if (L7_SUCCESS != rc)
     {
        CP_DLOG(CPD_LEVEL_LOG, "Can't set reason for disabling for I/F %d, %s", intIfNum, ifName);
     }
     break;
    case L7_INTF_WHY_DISABLED_UNATTACHED:
      rc = L7_SUCCESS;       /* nothing to do! */
      break;
    default:
      rc = cpimIntfStatusFinishDetachHelper(intIfNum);
    }
  }
  return rc;
}

/*********************************************************************
*
* @purpose  Complete pending disable of interfaces
*           to our status database
*
* @param    L7_uint32 intIfNum -- corresponding interface number
* @param    cpdmIntfCapabilityStatusData_t *pStat -- I/F stat entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This function is called ONLY from within this module,
*            and is ONLY a helper routine.
*            CALLER MUST PROVIDE SEMAPHORE PROTECTION!
*
* @end
*
*********************************************************************/
static L7_RC_t
cpimIntfStatusFinishBlock(L7_uint32  intIfNum)
{
  return L7_SUCCESS; /* no meaning in this context */

}

/*********************************************************************
*
* @purpose  Enable interface status
*
* @param    L7_uint32 intIfNum -- corresponding interface number
* @param    cpRedirIpAddr_t * redirIp -- IP addr to use for redirect
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfStatusUnblock(L7_uint32 intIfNum)
{
  return L7_SUCCESS; /* just a placeholder for now */
}

L7_RC_t cpimIntfStatusAttach(L7_uint32 intIfNum)
{
  return L7_SUCCESS; /* just a placeholder for now */
}

L7_RC_t cpimIntfStatusBlock(L7_uint32 intIfNum)
{
  return L7_SUCCESS; /* just a placeholder for now */
}

L7_RC_t cpimIntfStatusDetach(L7_uint32 intIfNum)
{
  return L7_SUCCESS; /* just a placeholder for now */
}


/*********************************************************************
*
* @purpose  Check to see if interface belongs to a wireless network
*           and that it is ready for use
*
* @param    L7_uint32 intIfNum -- corresponding interface number
* @param    L7_BOOL * pReady   -- is I/F wireless?
* @param    L7_BOOL * pWireless-- is wireless I/F ready?
*
* @returns  L7_SUCCESS if parms are good, and calls succeeded
* @returns  L7_FAILURE if something very bad beyond this module's
*                      control happened.
*
* @comments "wireless" means that the interface is a recognized
*           wireless interface. "ready" means that the wireless
*           interface has at least one VAP enabled. No other checks
*           are performed.
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfIsReady(L7_uint32 intIfNum,
                           L7_BOOL * pWireless,
                        L7_BOOL * pReady)
{
  if ((L7_NULLPTR == pWireless) || (L7_NULLPTR == pReady))
  {
    return L7_FAILURE;
  }

  *pWireless = L7_FALSE;
  *pReady = L7_FALSE;

#ifdef L7_WIRELESS_PACKAGE
  {
    L7_uint32 networkID = 0;

    *pWireless = (L7_SUCCESS == wdmNetworkIDGet(intIfNum, &networkID));
    if (*pWireless)
    {
      *pReady = (L7_SUCCESS == wdmNetworkVAPEnabled(networkID));
    }
  }
#endif

  return L7_SUCCESS;
}


/*****************************************************************
 *
 *                  INTF  WORK ORDER QUEUE FUNCTIONS
 *
 *****************************************************************/


L7_RC_t   intfWOQInit(L7_uint32 maxOrders)
{
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(WRITE);
  do
  {
    L7_uint32 size = sizeof(intfWorkOrder_t) * (maxOrders + 1);
    intfWOQ.pAll = (intfWorkOrder_t *)
      osapiMalloc(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID, size);

    if (L7_NULLPTR == intfWOQ.pAll)
      break;
    memset(intfWOQ.pAll, 0, size);
    intfWOQ.front = intfWOQ.back = 0;
    intfWOQ.size = maxOrders;
    intfWOQ.sema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
    if (L7_NULLPTR == intfWOQ.sema)
      break;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);
  return rc;
}


void   intfWOQFinalize(void)
{
  SEMA_TAKE(WRITE);
  osapiSemaDelete(intfWOQ.sema);
  osapiFree(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID, intfWOQ.pAll);
  intfWOQ.front = intfWOQ.back = 0;
  intfWOQ.size = 0;
  SEMA_GIVE(WRITE);
}

L7_RC_t intfWOGet(work_t * pWork,
                  NIM_EVENT_COMPLETE_INFO_t * comp)
{
  L7_RC_t rc = L7_FAILURE;

  if (!pWork || !comp)
    return L7_ERROR;

  osapiSemaTake(intfWOQ.sema,L7_WAIT_FOREVER);
  do
  {
    if (intfWOQ.front == intfWOQ.back)
      break;
    *pWork  = intfWOQ.pAll[intfWOQ.front].work;
    memcpy(comp, &(intfWOQ.pAll[intfWOQ.front].comp),
           sizeof(NIM_EVENT_COMPLETE_INFO_t));
    intfWOQ.front = (intfWOQ.front + 1) % intfWOQ.size;
    rc = L7_SUCCESS;
  } while(0);
  osapiSemaGive(intfWOQ.sema);

  return rc;
}

L7_RC_t intfWOAdd(work_t work, NIM_EVENT_COMPLETE_INFO_t * comp )
{
  L7_RC_t rc = L7_FAILURE;

  if (!comp)
    return rc;

  osapiSemaTake(intfWOQ.sema,L7_WAIT_FOREVER);
  do
  {
    if (intfWOQ.front == (intfWOQ.back + 1) % intfWOQ.size)
      break;  /* queue full */
    intfWOQ.pAll[intfWOQ.back].work = work;
    memcpy(&(intfWOQ.pAll[intfWOQ.back].comp), comp,
             sizeof(NIM_EVENT_COMPLETE_INFO_t));
    intfWOQ.back = (intfWOQ.back + 1) % intfWOQ.size;
    rc = L7_SUCCESS;
  } while(0);
  osapiSemaGive(intfWOQ.sema);

  return rc;
}
