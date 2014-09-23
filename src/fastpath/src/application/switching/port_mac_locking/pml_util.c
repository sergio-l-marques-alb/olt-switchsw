/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename pml_util.c
*
* @purpose Port MAC Locking utility functions
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
#include "pml.h"
#include "pml_util.h"
#include "pml_api.h"
#include "log.h"
#include "nimapi.h"
#include "dtlapi.h"
#include "l7_ip_api.h"
#include "mirror_api.h"
#include "dot3ad_api.h"
#include "fdb_api.h"


extern void fdbInsert(char *mac, L7_uint32 intIfNum, L7_uint32 vlanId, L7_ushort16 entryType);
extern void fdbDelete(char *mac, L7_uint32 vlanId);

extern pmlCfgData_t     *pmlCfgData;
extern pmlCnfgrState_t   pmlCnfgrState;
extern L7_BOOL           pmlWarmStart;
extern L7_uint32        *pmlMapTbl;
extern void             *pmlQueue;
extern pmlIntfOprData_t *pmlIntfOprData;

static void *pmlSemaphore = L7_NULL;
static L7_BOOL pmlSyncComplete = L7_FALSE;

#define PML_SYNC_TIMEOUT 6000 /* 60 seconds */

PORTEVENT_MASK_t pmlPortEventMask_g;
/*********************************************************************
* @purpose  Take the Port MAC Locking semaphore
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void pmlSemaTake(void)
{
  if (pmlSemaphore == L7_NULL)
  {
    pmlSemaphore = osapiSemaMCreate(OSAPI_SEM_Q_FIFO);
    if (pmlSemaphore == L7_NULL)
    {
      LOG_ERROR(0);
    }
  }

  osapiSemaTake(pmlSemaphore, L7_WAIT_FOREVER);

  return;
}

/*********************************************************************
* @purpose  Give the Port MAC Locking semaphore
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void pmlSemaGive(void)
{
  if (pmlSemaphore != L7_NULL)
  {
    osapiSemaGive(pmlSemaphore);
  }
}

/*********************************************************************
* @purpose  Obtain a pointer to the specified interface configuration data
*           for this interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to pml interface config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @notes    The caller can set the pCfg parm to L7_NULL if it does not
*           want the value output from this function.
*
* @end
*********************************************************************/
L7_BOOL pmlMapIntfIsConfigurable(L7_uint32 intIfNum, pmlIntfCfgData_t **pCfg)
{
  L7_uint32 index;
  nimConfigID_t configId;

  if (!(PML_IS_READY))
    return L7_FALSE;

  /* Check boundary conditions */
  if (intIfNum <= 0 || intIfNum >= platIntfMaxCountGet())
    return L7_FALSE;

  index = pmlMapTbl[intIfNum];

  if (index == 0)
    return L7_FALSE;

  /* verify that the configId in the config data table entry matches the configId that NIM maps to
   ** the intIfNum we are considering
   */
  if (nimConfigIdGet(intIfNum, &configId) == L7_SUCCESS)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&configId, &(pmlCfgData->pmlIntfCfgData[index].configId)) == L7_FALSE)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      /* if we get here, either we have a table management error between pmlCfgData and pmlMapTbl or
      ** there is synchronization issue between NIM and components w.r.t. interface creation/deletion
      */
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_PORT_MACLOCK_COMPONENT_ID,
          "Error accessing PML config data for interface %s in pmlMapIntfIsConfigurable."
          " A default configuration does not exist for this interface. Typically a case when"
          " a new interface is created and has no pre-configuration.", ifName);
      return L7_FALSE;
    }
  }

  *pCfg = &pmlCfgData->pmlIntfCfgData[index];

  return L7_TRUE;
}

/*********************************************************************
* @purpose  check if the interface is acquired by another component
*           and is therefore unavailable to PML
*
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
L7_BOOL pmlMapIntfIsAcquired(L7_uint32 intIfNum)
{
  L7_uint32 routingMode;

  if (ipMapRtrIntfModeGet(intIfNum, &routingMode) != L7_SUCCESS)
  {
    routingMode = L7_DISABLE;
  }

  if ((mirrorIsActiveProbePort(intIfNum)) ||
      (dot3adIsLagActiveMember(intIfNum)) ||
      (routingMode == L7_ENABLE))
  {
    return L7_TRUE;
  }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  check if the interface is attached
*
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
L7_BOOL pmlMapIntfIsAttached(L7_uint32 intIfNum)
{
  L7_NIM_QUERY_DATA_t       nimQueryData;

  memset((void *)&nimQueryData,0,sizeof(nimQueryData));
  nimQueryData.request  = L7_NIM_QRY_RQST_STATE;

  nimQueryData.intIfNum = intIfNum;
  if (nimIntfQuery(&nimQueryData) != L7_SUCCESS)
  {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      /* should never get here */
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_PORT_MACLOCK_COMPONENT_ID,
              " nimIntfQuery failed for intf %s\n", ifName);
      return L7_FALSE;
  }
  if (nimQueryData.data.state == L7_INTF_ATTACHED ||
      nimQueryData.data.state == L7_INTF_ATTACHING ||
      nimQueryData.data.state == L7_INTF_DETACHING)
  {
    return L7_TRUE;
  }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Obtain a pointer to the specified interface operational data
*           for this interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pOpr   @b{(output)}  Ptr  to pml interface operational structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes    The caller can set the pOpr parm to L7_NULL if it does not
*           want the value output from this function.
*
* @end
*********************************************************************/
L7_BOOL pmlMapIntfIsOperational(L7_uint32 intIfNum, pmlIntfOprData_t **pOpr)
{
  L7_uint32 index;
  nimConfigID_t configId;

  if (!(PML_IS_READY))
    return L7_FALSE;

  /* Check boundary conditions */
  if (intIfNum <= 0 || intIfNum >= platIntfMaxCountGet())
    return L7_FALSE;

  index = pmlMapTbl[intIfNum];

  if (index == 0)
    return L7_FALSE;

  /* verify that the configId in the config data table entry matches the configId that NIM maps to
   ** the intIfNum we are considering
   */
  if (nimConfigIdGet(intIfNum, &configId) == L7_SUCCESS)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&configId, &(pmlCfgData->pmlIntfCfgData[index].configId)) == L7_FALSE)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      /* if we get here, either we have a table management error between pmlCfgData and pmlMapTbl or
      ** there is synchronization issue between NIM and components w.r.t. interface creation/deletion
      */
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_PORT_MACLOCK_COMPONENT_ID,
              "Error accessing PML cfg data for interface %s in pmlMapIntfIsOperational.\n", ifName);
      return L7_FALSE;
    }
  }

  *pOpr = &pmlIntfOprData[index];

  return L7_TRUE;
}

/*********************************************************************
* @purpose  Obtain a pointer to the first free interface config struct
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to pml nterface config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL pmlMapIntfConfigEntryGet(L7_uint32 intIfNum, pmlIntfCfgData_t **pCfg)
{
  L7_uint32 i;
  nimConfigID_t configId;
  nimConfigID_t configIdNull;
  static L7_uint32 nextIndex = 1;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  if ((PML_IS_READY != L7_TRUE) ||
      (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS))
    return L7_FALSE;

  /* Avoid N^2 processing when interfaces created at startup */
  if ((nextIndex < L7_PML_MAX_INTF) &&
      (NIM_CONFIG_ID_IS_EQUAL(&pmlCfgData->pmlIntfCfgData[nextIndex].configId, &configIdNull)))
  {
    /* entry corresponding to nextIndex is free so use it, move speculative index to next entry
     * for next time
     */
    i = nextIndex++;
  }
  else
  /* Cached nextIndex is in use. Resort to search from beginning. N^2. */
  {
    for (i = 1; i < L7_PML_MAX_INTF; i++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&pmlCfgData->pmlIntfCfgData[i].configId, &configIdNull))
      {
        /* found a free entry, update the speculative index to next entry for next time */
        nextIndex = i+1;
        break;
      }
    }
  }

  if (i < L7_PML_MAX_INTF)
  {
    pmlMapTbl[intIfNum] = i;
    *pCfg = &pmlCfgData->pmlIntfCfgData[i];
    return L7_TRUE;
  }

  return L7_FALSE;
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_CREATE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t pmlIntfCreate(L7_uint32 intIfNum)
{
  nimConfigID_t      configId;
  pmlIntfCfgData_t  *pCfg;
  pmlIntfOprData_t  *pOpr;
  L7_uint32          i;

  if (pmlIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
    return L7_FAILURE;

  for (i = 1; i < L7_PML_MAX_INTF; i++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&pmlCfgData->pmlIntfCfgData[i].configId, &configId))
    {
      pmlMapTbl[intIfNum] = i;
      break;
    }
  }

  pCfg = L7_NULL;

  /* If an interface configuration entry is not already assigned to the interface,
     assign one */
  if (pmlMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    if(pmlMapIntfConfigEntryGet(intIfNum, &pCfg) != L7_TRUE)
      return L7_FAILURE;

    /* Update the configuration structure with the config id */
  if (pCfg != L7_NULL)
      pmlBuildDefaultIntfConfigData(&configId, pCfg);
  }

  /* allocate operational data structures for this interface */
  if (pmlMapIntfIsOperational(intIfNum, &pOpr) != L7_TRUE)
  {
    LOG_ERROR(intIfNum);
  }

  pOpr->dynamicCount = 0;
  pOpr->staticCount = 0;

  /* violation tree */
  pOpr->violationTreeHeap = (avlTreeTables_t *)osapiMalloc(L7_PORT_MACLOCK_COMPONENT_ID, PML_VIOLATION_TREE_SIZE *
                                                         sizeof(avlTreeTables_t));
  if (pOpr->violationTreeHeap == L7_NULL)
  {
    LOG_ERROR(0);
  }
  pOpr->violationDataHeap = (pmlLockInfoData_t *)osapiMalloc(L7_PORT_MACLOCK_COMPONENT_ID, PML_VIOLATION_TREE_SIZE *
                                                           sizeof(pmlLockInfoData_t));
  if (pOpr->violationDataHeap == L7_NULL)
  {
    LOG_ERROR(0);
  }

  avlCreateAvlTree(&pOpr->violationAvlTree, pOpr->violationTreeHeap, pOpr->violationDataHeap,
                   PML_VIOLATION_TREE_SIZE, sizeof(pmlLockInfoData_t),
                   0x10, sizeof(L7_uchar8)*L7_FDB_KEY_SIZE);

  /* update static count */
  for (i = 0; i < L7_MACLOCKING_MAX_STATIC_ADDRESSES; i++)
  {
    if (pCfg != L7_NULL && pCfg->staticMacEntry[i].vlanId != 0)
    {
      pOpr->staticCount++;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose
*
* @param    L7_uint32  intIfNum      internal interface number
* @param    L7_BOOL    warmRestart   Indicates if a warm restart is occuring
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t pmlApplyIntfConfigData(L7_uint32 intIfNum, L7_BOOL warmStart)
{
  pmlIntfCfgData_t *pCfg;

  if (pmlMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    /* Cannot enable PML when interface is acquired. Return without updating
     * hardware. The configuration will be completed when the interface is made
     * available again */
    if(pmlMapIntfIsAcquired(intIfNum) && (pCfg->intfLockEnabled != L7_DISABLE))
      return L7_SUCCESS;

    if (pmlIntfModeApply(intIfNum, pCfg->intfLockEnabled, warmStart) != L7_SUCCESS)
      return L7_FAILURE;

    /* add the statically locked entries to HW and FDB component */
    pmlIntfAllStaticEntriesAdd(intIfNum);
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_DETACH
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t pmlIntfDetach(L7_uint32 intIfNum)
{
  pmlIntfCfgData_t *pCfg;
  pmlIntfOprData_t *pOpr;

  if (pmlMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    /* Cannot enable PML when interface is acquired - Don't update hardware */
    if(pmlMapIntfIsAcquired(intIfNum) && ((FD_PML_INTF_LOCK_MODE) != L7_DISABLE))
      return L7_SUCCESS;

    if (pmlIntfModeApply(intIfNum, FD_PML_INTF_LOCK_MODE, L7_FALSE) != L7_SUCCESS)
      return L7_FAILURE;

    /* remove all statically locked entries from HW for this port */
    if (pmlMapIntfIsOperational(intIfNum, &pOpr) == L7_TRUE)
    {
      pmlIntfAllStaticEntriesDelete(intIfNum);

      pOpr->lastViolationAddr.vlanId = 0;
      memset(pOpr->lastViolationAddr.macAddr.addr, 0 ,sizeof(pOpr->lastViolationAddr.macAddr.addr));

      if (pCfg->violationTrapsEnabled == L7_TRUE)
      {
        avlPurgeAvlTree(&pOpr->violationAvlTree, PML_VIOLATION_TREE_SIZE);
      }
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_DELETE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t pmlIntfDelete(L7_uint32 intIfNum)
{
  pmlIntfCfgData_t *pCfg;
  pmlIntfOprData_t *pOpr;

  if (pmlMapIntfIsOperational(intIfNum, &pOpr) == L7_TRUE)
  {
    pOpr->dynamicCount = 0;
    pOpr->staticCount = 0;

    avlDeleteAvlTree(&pOpr->violationAvlTree);
    osapiFree(L7_PORT_MACLOCK_COMPONENT_ID, pOpr->violationTreeHeap);
    osapiFree(L7_PORT_MACLOCK_COMPONENT_ID, pOpr->violationDataHeap);
  }

  if (pmlMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    memset((void *)&pCfg->configId, 0, sizeof(nimConfigID_t));
    memset((void *)&pmlMapTbl[intIfNum], 0, sizeof(L7_uint32));
    pmlCfgData->cfgHdr.dataChanged = L7_TRUE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Propogate Interface notifications to pmling Task
*
* @param    L7_uint32  intIfNum   Interface number
* @param    L7_uint32  event      Event type
* @param    NIM_CORRELATOR_t  correlator  Correlator for event
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pmlIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator)
{
  pmlMgmtMsg_t msg;
  NIM_EVENT_COMPLETE_INFO_t  status;

  /* if this is an event we are not interested in, perform an early return*/
  if (event != L7_PORT_ROUTING_DISABLED &&
      event != L7_PORT_ROUTING_ENABLED &&
      event != L7_LAG_ACQUIRE &&
      event != L7_LAG_RELEASE &&
      event != L7_PROBE_SETUP &&
      event != L7_PROBE_TEARDOWN &&
      event != L7_CREATE &&
      event != L7_ATTACH &&
      event != L7_DETACH &&
      event != L7_DELETE &&
      event != L7_DOWN)
  {
    status.intIfNum     = intIfNum;
    status.component    = L7_PORT_MACLOCK_COMPONENT_ID;
    status.event        = event;
    status.correlator   = correlator;
    status.response.reason = NIM_ERR_RC_UNUSED;
    status.response.rc  = L7_SUCCESS;
    nimEventStatusCallback(status);
    return L7_SUCCESS;
  }


  msg.msgId = pmlIntfChange;

  msg.intIfNum = intIfNum;
  msg.u.pmlIntfChangeParms.event = event;
  msg.u.pmlIntfChangeParms.correlator = correlator;

  osapiMessageSend(pmlQueue, &msg, PML_MSG_SIZE, L7_NO_WAIT, L7_MSG_PRIORITY_NORM);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Propogate Startup notifications to PML component
*
* @param    NIM_STARTUP_PHASE_t  startupPhase
*
* @notes
*
* @end
*********************************************************************/
void pmlStartupNotifyCallback(NIM_STARTUP_PHASE_t startupPhase)
{
  pmlMgmtMsg_t msg;

  msg.msgId                                = pmlStartupNotify;
  msg.u.pmlStartupNotifyParms.startupPhase = startupPhase;

  osapiMessageSend(pmlQueue, &msg, PML_MSG_SIZE, L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
}

/*********************************************************************
* @purpose  Handle Interface notifications
*
* @param    L7_uint32  intIfNum   Interface number
* @param    L7_uint32  event      Event type
* @param    NIM_CORRELATOR_t  correlator  Correlator for event
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pmlIntfChangeProcess(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator)
{
  L7_RC_t                    rc = L7_SUCCESS;
  NIM_EVENT_COMPLETE_INFO_t  status;
  pmlIntfCfgData_t          *pCfg;
  pmlIntfOprData_t          *pOpr;

  status.intIfNum     = intIfNum;
  status.component    = L7_PORT_MACLOCK_COMPONENT_ID;
  status.event        = event;
  status.correlator   = correlator;
  status.response.reason = NIM_ERR_RC_UNUSED;

  if (!(PML_IS_READY))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_PORT_MACLOCK_COMPONENT_ID,
            "Received an interface change callback while not ready to receive it");
    rc = L7_FAILURE;
    status.response.rc  = rc;
    nimEventStatusCallback(status);

    return rc;
  }

  if (pmlIsValidIntf(intIfNum) != L7_TRUE)
  {
    rc = L7_SUCCESS;
    status.response.rc  = rc;
    nimEventStatusCallback(status);

    return rc;
  }

  switch (event)
  {
  case L7_CREATE:
    pmlSemaTake();
    rc = pmlIntfCreate(intIfNum);
    pmlSemaGive();
    break;

  case L7_DELETE:
    pmlSemaTake();
    rc = pmlIntfDelete(intIfNum);
    pmlSemaGive();
    break;

  case L7_ATTACH:
  case L7_PORT_ROUTING_DISABLED:
  case L7_PROBE_TEARDOWN:
  case L7_LAG_RELEASE:
    pmlSemaTake();
    rc = pmlApplyIntfConfigData(intIfNum, L7_FALSE);
    pmlSemaGive();
    break;

  case L7_DETACH:
  case L7_PORT_ROUTING_ENABLED:
  case L7_PROBE_SETUP:
  case L7_LAG_ACQUIRE:
    pmlSemaTake();
    rc = pmlIntfDetach(intIfNum);
    pmlSemaGive();
    break;

  case L7_DOWN:
    /* purge the violation address tree */
    pmlSemaTake();
    if (pmlMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
    {
      if (pmlMapIntfIsOperational(intIfNum, &pOpr) == L7_TRUE)
      {
        pOpr->lastViolationAddr.vlanId = 0;
        memset(pOpr->lastViolationAddr.macAddr.addr, 0 ,sizeof(pOpr->lastViolationAddr.macAddr.addr));

        if (pCfg->violationTrapsEnabled == L7_TRUE)
        {
          avlPurgeAvlTree(&pOpr->violationAvlTree, PML_VIOLATION_TREE_SIZE);
        }
      }
    }
    pmlSemaGive();
    break;

  default:
    break;
  }

  status.response.rc  = rc;
  nimEventStatusCallback(status);

  return rc;
}

/*********************************************************************
* @purpose  Propogate Startup notifications to PML component
*
* @param    NIM_STARTUP_PHASE_t  startupPhase
*
* @notes
*
* @end
*********************************************************************/
void pmlStartupNotifyProcess(NIM_STARTUP_PHASE_t startupPhase)
{
  L7_RC_t           rc;
  L7_uint32         intIfNum;
  pmlIntfCfgData_t *pCfg;
  pmlIntfOprData_t *pOpr;
  L7_uint32         fdbDynamicCount;
  L7_uint32         pmlSyncTimeout;

  rc = nimFirstValidIntfNumber(&intIfNum);

  while (rc == L7_SUCCESS)
  {
    if (pmlIsValidIntf(intIfNum) == L7_TRUE)
    {
      switch (startupPhase)
      {
      case NIM_INTERFACE_CREATE_STARTUP:
        pmlSemaTake();
        rc = pmlIntfCreate(intIfNum);
        pmlSemaGive();
        break;

      case NIM_INTERFACE_ACTIVATE_STARTUP:
        /* If the interface is attached, do the attach processing. */
        if (pmlMapIntfIsAttached(intIfNum))
        {
          pmlSemaTake();
          rc = pmlApplyIntfConfigData(intIfNum, pmlWarmStart);
          pmlSemaGive();
        }
        break;

      default:
        break;
      }
    }

    rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
  }

  switch (startupPhase)
  {
  case NIM_INTERFACE_CREATE_STARTUP:
    /* Add CREATE/DELETE events to our NIM registration */
    PORTEVENT_SETMASKBIT(pmlPortEventMask_g, L7_CREATE);
    PORTEVENT_SETMASKBIT(pmlPortEventMask_g, L7_DELETE);
    nimRegisterIntfEvents(L7_PORT_MACLOCK_COMPONENT_ID, pmlPortEventMask_g);
    break;

  case NIM_INTERFACE_ACTIVATE_STARTUP:
    if (pmlWarmStart)
    {
      /* If this is a warm start, then we need to wait until the FDB sync completes.
         Once the FDB is synced, then PML can ensure that the dynamic counts for each port are
         within the configured limits. */
      pmlSyncTimeout = PML_SYNC_TIMEOUT;
      while ((pmlSyncCompleteGet() == L7_FALSE) && pmlSyncTimeout)
      {
        pmlSyncTimeout--;
        osapiSleepMSec(10);
      }
      pmlSyncCompleteSet(L7_FALSE);

      rc = nimFirstValidIntfNumber(&intIfNum);

      while (rc == L7_SUCCESS)
      {
        if (pmlIsValidIntf(intIfNum) == L7_TRUE)
        {
          pmlSemaTake();
          if (pmlMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
          {
            pmlSemaGive();
            break;
          }

          if (pmlMapIntfIsOperational(intIfNum, &pOpr) != L7_TRUE)
          {
            pmlSemaGive();
            break;
          }
          /* Ensure the dynamic counts for PML match FDB. */
          fdbIntfDynamicEntriesGet(intIfNum, &fdbDynamicCount);
          pOpr->dynamicCount = fdbDynamicCount;

          /* If PML is enabled globally and on the interface,
             ensure the dynamic count hasn't exceeded the configured
             limit. */
          if ((pmlCfgData->globalLockEnabled == L7_TRUE) && (pCfg->intfLockEnabled == L7_TRUE))
          {
            if (pOpr->dynamicCount > pCfg->dynamicLimit)
            {
              L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
              nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

              L7_LOGF(L7_LOG_SEVERITY_WARNING,
                      L7_PORT_MACLOCK_COMPONENT_ID,
                      "PML dynamic count (%d) exceeded limit (%d) on intf %s, flushing",
                      pOpr->dynamicCount,
                      pCfg->dynamicLimit,
                      ifName);
              dtlDot1sFlush(intIfNum);
            }
          }
          pmlSemaGive();
        }

        rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
      }
    }

    pmlWarmStart = L7_FALSE;

    /* Add ATTACH/DETACH events to our NIM registration */
    PORTEVENT_SETMASKBIT(pmlPortEventMask_g, L7_ATTACH);
    PORTEVENT_SETMASKBIT(pmlPortEventMask_g, L7_DETACH);
    PORTEVENT_SETMASKBIT(pmlPortEventMask_g, L7_PORT_ROUTING_ENABLED);
    PORTEVENT_SETMASKBIT(pmlPortEventMask_g, L7_PORT_ROUTING_DISABLED);
    PORTEVENT_SETMASKBIT(pmlPortEventMask_g, L7_PROBE_SETUP);
    PORTEVENT_SETMASKBIT(pmlPortEventMask_g, L7_PROBE_TEARDOWN);
    PORTEVENT_SETMASKBIT(pmlPortEventMask_g, L7_LAG_ACQUIRE);
    PORTEVENT_SETMASKBIT(pmlPortEventMask_g, L7_LAG_RELEASE);
    PORTEVENT_SETMASKBIT(pmlPortEventMask_g, L7_DOWN);
    nimRegisterIntfEvents(L7_PORT_MACLOCK_COMPONENT_ID, pmlPortEventMask_g);
    break;

  default:
    break;
  }

  nimStartupEventDone(L7_PORT_MACLOCK_COMPONENT_ID);

  /* Inform cnfgr that pml has completed it HW updates */
  if (startupPhase == NIM_INTERFACE_ACTIVATE_STARTUP)
  {
    cnfgrApiComponentHwUpdateDone(L7_PORT_MACLOCK_COMPONENT_ID,
                                  L7_CNFGR_HW_APPLY_CONFIG);
  }
}

/*********************************************************************
*
* @purpose  Applies the Port MAC Locking Admin mode
*
* @param    L7_uchar8  adminMode   @b((input)) Port MAC Locking admin mode
*
* @returns  L7_SUCCESS
*
* @notes    This function assumes adminMode is valid
*
* @end
*********************************************************************/
L7_RC_t pmlAdminModeApply(L7_uint32 adminMode)
{
  L7_uint32 intIfNum, cfgIndex;
  nimConfigID_t configIdNull;
  pmlIntfCfgData_t *pCfg;
  pmlIntfOprData_t *pOpr;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  if (adminMode == L7_ENABLE)
  {
    /* Enable all interfaces that are configured for MAC Locking.
    */
    if (PML_IS_READY)
    {
      for (cfgIndex = 1; cfgIndex < L7_PML_MAX_INTF; cfgIndex++)
      {
        if (NIM_CONFIG_ID_IS_EQUAL(&pmlCfgData->pmlIntfCfgData[cfgIndex].configId, &configIdNull))
          continue;
        if (nimIntIfFromConfigIDGet(&(pmlCfgData->pmlIntfCfgData[cfgIndex].configId), &intIfNum) != L7_SUCCESS)
          continue;
        if (pmlMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
          continue;
        /* Cannot enable PML when interface is acquired */
        if(pmlMapIntfIsAcquired(intIfNum) && (pCfg->intfLockEnabled != L7_DISABLE))
          continue;

        pmlIntfModeApply(intIfNum, pCfg->intfLockEnabled, L7_FALSE);
      }
    }
  }
  else
  {
    /* disable all interfaces for MAC Locking */
    if (PML_IS_READY)
    {
      for (cfgIndex = 1; cfgIndex < L7_PML_MAX_INTF; cfgIndex++)
      {
        if (NIM_CONFIG_ID_IS_EQUAL(&pmlCfgData->pmlIntfCfgData[cfgIndex].configId, &configIdNull))
          continue;
        if (nimIntIfFromConfigIDGet(&(pmlCfgData->pmlIntfCfgData[cfgIndex].configId), &intIfNum) != L7_SUCCESS)
          continue;
        if (pmlMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
          continue;
        if (pmlMapIntfIsOperational(intIfNum, &pOpr) == L7_TRUE)
        {
          pmlIntfModeApply(intIfNum, L7_DISABLE, L7_FALSE);

          /* purge the violation tree for this port */
          pOpr->lastViolationAddr.vlanId = 0;
          memset(pOpr->lastViolationAddr.macAddr.addr, 0 ,sizeof(pOpr->lastViolationAddr.macAddr.addr));

          if (pCfg->violationTrapsEnabled == L7_TRUE)
          {
            avlPurgeAvlTree(&pOpr->violationAvlTree, PML_VIOLATION_TREE_SIZE);
          }
        }
      }
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Applies the Port MAC Locking mode for the specified interface in all VLANs
*
* @param    L7_uint32  intIfNum  @b((input)) Internal interface number
* @param    L7_uchar8  mode      @b((input)) Port MAC Locking intf mode
* @param    L7_BOOL    warmRestart @b((input)) Indicates if a warm restart is occuring
*
* @returns  L7_SUCCESS, if interface mode was successfully applied
* @returns  L7_FAILURE, if interface number is invalid
*
* @notes    This function assumes mode parameter is valid
*
* @end
*********************************************************************/
L7_RC_t pmlIntfModeApply(L7_uint32 intIfNum, L7_uint32 mode, L7_BOOL warmStart)
{
  L7_RC_t rc = L7_FAILURE;

  /* Only modify HW if intf is attached. */
  if (pmlMapIntfIsAttached(intIfNum))
  {
    if (pmlCfgData->globalLockEnabled == L7_TRUE)
    {
      /* apply config to HW */
      rc = dtlPmlLockConfig(intIfNum, mode);

      /* Flush L2 entries on this port if we are enabling PML. If we are
         processing a warm restart event, then we don't want to flush
         the L2 entries for this port. */
      if ((mode == L7_TRUE) && (rc == L7_SUCCESS) && (warmStart == L7_FALSE))
      {
        /* flush the dynamic entries on this port */
        dtlDot1sFlush(intIfNum);
      }
    }
    else
    {
      /* apply config to HW */
      dtlPmlLockConfig(intIfNum, L7_FALSE);
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Adds all configured static entries for an intf to HW
*
* @param    L7_uint32  intIfNum  @b((input)) Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t pmlIntfAllStaticEntriesAdd(L7_uint32 intIfNum)
{
  pmlIntfCfgData_t  *pCfg;
  L7_uint32          i;

  /* apply config to HW */
  if (pmlMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  for (i = 0; i < L7_MACLOCKING_MAX_STATIC_ADDRESSES; i++)
  {
    if (pCfg->staticMacEntry[i].vlanId != 0)
    {
      /* Only modify HW if intf is attached. */
      if (pmlMapIntfIsAttached(intIfNum))
      {
        dtlFdbMacAddrAdd(pCfg->staticMacEntry[i].macAddr.addr,
                         intIfNum,
                         pCfg->staticMacEntry[i].vlanId,
                         L7_FDB_ADDR_FLAG_STATIC,
                         L7_NULL);
      }

      fdbInsert(pCfg->staticMacEntry[i].macAddr.addr,
                intIfNum,
                pCfg->staticMacEntry[i].vlanId,
                L7_FDB_ADDR_FLAG_STATIC);
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Removes all configured static entries for an intf from HW
*
* @param    L7_uint32  intIfNum  @b((input)) Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t pmlIntfAllStaticEntriesDelete(L7_uint32 intIfNum)
{
  pmlIntfCfgData_t  *pCfg;
  L7_uint32          i;

  /* apply config to HW */
  if (pmlMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  for (i = 0; i < L7_MACLOCKING_MAX_STATIC_ADDRESSES; i++)
  {
    if (pCfg->staticMacEntry[i].vlanId != 0)
    {
      /* Only modify HW if intf is attached. */
      if (pmlMapIntfIsAttached(intIfNum))
      {
        dtlFdbMacAddrDelete(pCfg->staticMacEntry[i].macAddr.addr,
                            intIfNum,
                            pCfg->staticMacEntry[i].vlanId,
                            0,
                            L7_NULL);
      }

      fdbDelete(pCfg->staticMacEntry[i].macAddr.addr,
                pCfg->staticMacEntry[i].vlanId);
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the PML sync flag to true or false
*
* @end
*********************************************************************/
void pmlSyncCompleteSet(L7_BOOL syncComplete)
{
  pmlSyncComplete = syncComplete;
}

/*********************************************************************
* @purpose  Gets the PML sync flag to true or false
*
* @returns  L7_BOOL
*
* @end
*********************************************************************/
L7_BOOL pmlSyncCompleteGet()
{
  return pmlSyncComplete;
}

/*********************************************************************
* @purpose  Print the current Port MAC Locking config values to serial port
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlCfgDump(void)
{
  L7_char8 buf[32];
  L7_uint32 i, j, extIfNum;
  L7_uint32 intIfNum, mode;
  pmlIntfCfgData_t *pCfg;
  nimConfigID_t configIdNull;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  printf("\n");
  printf("Port MAC Locking\n");
  printf("=============\n");

  if (pmlCfgData->globalLockEnabled == L7_ENABLE)
    sprintf(buf, "%s", "Enable");
  else
    sprintf(buf, "%s", "Disable");
  printf("Admin Mode - %s\n", buf);

  printf("Ext Intfs configured - ");
  for (i = 1; i < L7_PML_MAX_INTF; i++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&pmlCfgData->pmlIntfCfgData[i].configId, &configIdNull))
      continue;
    if (nimIntIfFromConfigIDGet(&(pmlCfgData->pmlIntfCfgData[i].configId), &intIfNum) != L7_SUCCESS)
      continue;
    if (pmlMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
    {
      if ((pmlIntfModeGet(intIfNum, &mode) == L7_SUCCESS) && (mode == L7_ENABLE))
      {
        if (nimGetIntfIfIndex(intIfNum, &extIfNum) == L7_SUCCESS)
        {
          if (pmlMapIntfIsAcquired(intIfNum))
            sprintf(buf, "%s", "Yes");
          else
            sprintf(buf, "%s", "No");

          printf("Intf %d\n", extIfNum);
          printf("Acquired %s\n", buf);
          printf("Dynamic Limit %d\n", pCfg->dynamicLimit);
          printf("Static Limit %d\n", pCfg->staticLimit);
          printf("Violation Traps Enabled  %d\n", pCfg->violationTrapsEnabled);
          printf("Violation Traps Seconds %d\n", pCfg->violationTrapsSeconds);
          for (j = 0; j < L7_MACLOCKING_MAX_STATIC_ADDRESSES; j++)
          {
            if (pCfg->staticMacEntry[j].vlanId != 0)
            {
              printf("Vlan %d, %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n",
                     pCfg->staticMacEntry[j].vlanId,
                     pCfg->staticMacEntry[j].macAddr.addr[0],
                     pCfg->staticMacEntry[j].macAddr.addr[1],
                     pCfg->staticMacEntry[j].macAddr.addr[2],
                     pCfg->staticMacEntry[j].macAddr.addr[3],
                     pCfg->staticMacEntry[j].macAddr.addr[4],
                     pCfg->staticMacEntry[j].macAddr.addr[5]);
            }
          }
        }
      }
    }
  }
  printf("\n");

  printf("\n=============\n");

  return L7_SUCCESS;
}
