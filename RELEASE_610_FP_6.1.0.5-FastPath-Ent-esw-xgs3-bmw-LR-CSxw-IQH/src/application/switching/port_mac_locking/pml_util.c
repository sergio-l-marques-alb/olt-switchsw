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


extern void fdbInsert(char *mac, L7_uint32 intIfNum, L7_uint32 vlanId, L7_ushort16 entryType);
extern void fdbDelete(char *mac, L7_uint32 vlanId);

extern pmlCfgData_t     *pmlCfgData;
extern pmlCnfgrState_t   pmlCnfgrState;
extern L7_uint32        *pmlMapTbl;
extern void             *pmlQueue;
extern pmlIntfOprData_t *pmlIntfOprData;
extern pmlIntfInfo_t    *pmlIntfInfo;

static void *pmlSemaphore = L7_NULL;

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
      /* if we get here, either we have a table management error between pmlCfgData and pmlMapTbl or
      ** there is synchronization issue between NIM and components w.r.t. interface creation/deletion
      */
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_PORT_MACLOCK_COMPONENT_ID,
          "Error accessing PML config data for interface %d in pmlMapIntfIsConfigurable."
          " A default configuration does not exist for this interface. Typically a case when"
          " a new interface is created and has no pre-configuration.", intIfNum);
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
  L7_BOOL result;

  COMPONENT_NONZEROMASK(pmlIntfInfo[intIfNum].acquiredList, result);

  return result;
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
      /* if we get here, either we have a table management error between pmlCfgData and pmlMapTbl or
      ** there is synchronization issue between NIM and components w.r.t. interface creation/deletion
      */
      LOG_MSG("Error accessing PML cfg data for interface %d in pmlMapIntfIsOperational.\n", intIfNum);
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
  L7_RC_t rc;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  if (!(PML_IS_READY))
    return L7_FALSE;

  if ((rc = nimConfigIdGet(intIfNum, &configId)) == L7_SUCCESS)
  {
    for (i = 1; i < L7_PML_MAX_INTF; i++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&pmlCfgData->pmlIntfCfgData[i].configId, &configIdNull))
      {
        pmlMapTbl[intIfNum] = i;
        *pCfg = &pmlCfgData->pmlIntfCfgData[i];
        return L7_TRUE;
      }
    }
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
L7_RC_t pmlApplyIntfConfigData(L7_uint32 intIfNum)
{
  pmlIntfCfgData_t *pCfg;

  if (pmlMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    /* Cannot enable PML when interface is acquired. Return without updating
     * hardware. The configuration will be completed when the interface is made
     * available again */
    if(pmlMapIntfIsAcquired(intIfNum) && (pCfg->intfLockEnabled != L7_DISABLE))
      return L7_SUCCESS;

    if (pmlIntfModeApply(intIfNum, pCfg->intfLockEnabled) != L7_SUCCESS)
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

    if (pmlIntfModeApply(intIfNum, FD_PML_INTF_LOCK_MODE) != L7_SUCCESS)
      return L7_FAILURE;

    /* remove all statically locked entries from HW for this port */
    if (pmlMapIntfIsOperational(intIfNum, &pOpr) == L7_TRUE)
    {
      pmlIntfAllStaticEntriesDelete(intIfNum);

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
      event != L7_CREATE &&
      event != L7_ATTACH &&
      event != L7_DETACH &&
      event != L7_DELETE &&
      event != L7_DOWN && 
      event != L7_UP)
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
  L7_BOOL                    currAcquired, prevAcquired;


  status.intIfNum     = intIfNum;
  status.component    = L7_PORT_MACLOCK_COMPONENT_ID;
  status.event        = event;
  status.correlator   = correlator;
  status.response.reason = NIM_ERR_RC_UNUSED;

  if (!(PML_IS_READY))
  {
    LOG_MSG("Received an interface change callback while not ready to receive it");
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
  case L7_ACTIVE:
    break;

  case L7_LAG_RELEASE:
    break;

  case L7_PROBE_TEARDOWN:
    break;            

  case L7_PORT_ROUTING_DISABLED:
    pmlSemaTake();

    /* When port based routing is disabled, restore PML configuration */
    COMPONENT_ACQ_CLRMASKBIT(pmlIntfInfo[intIfNum].acquiredList, L7_IP_MAP_COMPONENT_ID);

    COMPONENT_ACQ_NONZEROMASK(pmlIntfInfo[intIfNum].acquiredList, currAcquired);

    if (currAcquired == L7_FALSE)
    {
      rc = pmlApplyIntfConfigData(intIfNum);
    } else
    {
      rc = L7_SUCCESS;
    }

    pmlSemaGive();
    break;
    
  case L7_INACTIVE:
    break;

  case L7_LAG_ACQUIRE:
    break;
  
  case L7_PROBE_SETUP:
    break;
  
  case L7_PORT_ROUTING_ENABLED:
    pmlSemaTake();

    /* When port based routing is enabled, disable PML */
    COMPONENT_ACQ_NONZEROMASK(pmlIntfInfo[intIfNum].acquiredList, prevAcquired);

    /* If the interface was not previously acquired, disable PML */
    if (prevAcquired == L7_FALSE)
    {
        /* update acquired list */
        COMPONENT_ACQ_SETMASKBIT(pmlIntfInfo[intIfNum].acquiredList, L7_IP_MAP_COMPONENT_ID);
        rc = pmlIntfDetach(intIfNum);
    } else
    {
      rc = L7_SUCCESS;
    }

    pmlSemaGive();
    break;

  case L7_CREATE:
    pmlSemaTake();
    rc = pmlIntfCreate(intIfNum);
    pmlSemaGive();
    break;

  case L7_ATTACH:
    break;

  case L7_DETACH:
    pmlSemaTake();
    rc = pmlIntfDetach(intIfNum);
    pmlSemaGive();
    break;

  case L7_DELETE:
    pmlSemaTake();
    rc = pmlIntfDelete(intIfNum);
    pmlSemaGive();
    break;

  case L7_UP:
    pmlSemaTake();
    rc = pmlApplyIntfConfigData(intIfNum);
    pmlSemaGive();
    break;

  case L7_DOWN:
    /* purge the violation address tree */
    pmlSemaTake();
    if (pmlMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
    {
      if (pmlMapIntfIsOperational(intIfNum, &pOpr) == L7_TRUE)
      {
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

        pmlIntfModeApply(intIfNum, pCfg->intfLockEnabled);
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
          pmlIntfModeApply(intIfNum, L7_DISABLE);

          /* purge the violation tree for this port */
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
*
* @returns  L7_SUCCESS, if interface mode was successfully applied
* @returns  L7_FAILURE, if interface number is invalid
*
* @notes    This function assumes mode parameter is valid
*
* @end
*********************************************************************/
L7_RC_t pmlIntfModeApply(L7_uint32 intIfNum, L7_uint32 mode)
{
  L7_RC_t rc = L7_FAILURE;

  if (pmlCfgData->globalLockEnabled == L7_TRUE)
  {
    /* apply config to HW */
    rc = dtlPmlLockConfig(intIfNum, mode);

    if (mode == L7_TRUE && rc== L7_SUCCESS)
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
      dtlFdbMacAddrAdd(pCfg->staticMacEntry[i].macAddr.addr, 
                       intIfNum, 
                       pCfg->staticMacEntry[i].vlanId, 
                       L7_FDB_ADDR_FLAG_STATIC, 
                       L7_NULL);

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
      dtlFdbMacAddrDelete(pCfg->staticMacEntry[i].macAddr.addr, 
                          intIfNum,
                          pCfg->staticMacEntry[i].vlanId, 
                          0, 
                          L7_NULL);

      fdbDelete(pCfg->staticMacEntry[i].macAddr.addr,
                pCfg->staticMacEntry[i].vlanId);
    }
  }

  return L7_SUCCESS;
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
