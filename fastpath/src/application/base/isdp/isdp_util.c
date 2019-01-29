/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
* @filename  isdp_util.c
*
* @purpose   ISDP utility file
*
* @component isdp
*
* @comments
*
* @create    15/11/2007
*
* @author    dgaryachy
*
* @end
*
**********************************************************************/
#include "isdp_include.h"
#include "avl_api.h"
#include "osapi.h"
#include "cli_web_exports.h"
#include "cli_web_mgr_api.h"
#include "mirror_api.h"

extern isdpStats_t        *isdpStats;
extern isdpEntryTree_t    *isdpEntryTree;
extern L7_uint32          isdpTaskId;

extern isdpCnfgrState_t   isdpCnfgrState;
extern void               *isdpProcessQueue;
extern isdpCfg_t          *isdpCfg;
extern L7_uint32          *isdpMapTbl;
extern isdpIntfInfoData_t *isdpIntfTbl;
extern isdpStats_t        *isdpStats;
extern L7_uint32          isdpIpAddrPoolId;

PORTEVENT_MASK_t isdpNimEventMask;

static void isdpIntfDevIdZeroEntryDelete(L7_uint32 intIfNum);
static L7_RC_t isdpIntfPolicyApply(L7_uint32 intIfNum, L7_uint32 mode);

/*****************************************************************
* @purpose  Delete an entry from the specified tree
*
* @param    key      @b{(input)} AVL key
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*****************************************************************/
static L7_RC_t isdpEntryDeleteImpl(isdpEntry_t *pEntry)
{
  L7_RC_t     rc      = L7_SUCCESS;

  (void)isdpIpAddrListRelease(&pEntry->ipAddressList);

  rc = (L7_NULLPTR == avlDeleteEntry(&isdpEntryTree->treeData, &pEntry->key)) ? L7_FAILURE : L7_SUCCESS;

  return rc;
}

/*********************************************************************
* @purpose  Obtain a pointer to the specified interface configuration data
*           for this interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to isdp port config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @comments Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @comments The caller can set the pCfg parm to L7_NULL if it does not
*           want the value output from this function.
*
* @end
*********************************************************************/
L7_BOOL isdpIntfIsConfigurable(L7_uint32 intIfNum, isdpIntfCfgData_t **pCfg)
{
  L7_uint32     index    = 0;
  nimConfigID_t cfgId;

  memset(&cfgId, 0, sizeof(nimConfigID_t));

  if (!(ISDP_IS_READY))
  {
    isdpDebugTrace(ISDP_DBG_FLAG_ConfigGroup,
                   "%s: LINE %d: isdp isn't ready \n",
                   __FUNCTION__, __LINE__);
    return L7_FALSE;
  }

  if ( isdpIsValidIntf(intIfNum) != L7_TRUE )
  {
    isdpDebugTrace(ISDP_DBG_FLAG_ConfigGroup,
                   "%s: LINE %d: wrong intIfNum %d \n",
                   __FUNCTION__, __LINE__, intIfNum);
    return L7_FALSE;
  }

  index = isdpMapTbl[intIfNum];

  if (index == 0)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_ConfigGroup,
                   "%s: LINE %d: intIfNum %d is not configured\n",
                   __FUNCTION__, __LINE__, intIfNum);

    return L7_FALSE;
  }

  /* verify that the configId in the config data table entry matches the configId that NIM maps to
   ** the intIfNum we are considering
   */
  if (nimConfigIdGet(intIfNum, &cfgId) == L7_SUCCESS)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&cfgId, &(isdpCfg->cfg.intfCfgData[index].cfgId)) == L7_FALSE)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      /* if we get here, either we have a table management error between isdpCfg and isdpMapTbl or
      ** there is synchronization issue between NIM and components w.r.t. interface creation/deletion
      */
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_ISDP_COMPONENT_ID,
                        "Error accessing isdp config data for interface %s in %s.\n",
                        ifName, __FUNCTION__);

      return L7_FALSE;
    }
  }

  *pCfg = &isdpCfg->cfg.intfCfgData[index];

  return L7_TRUE;
}

/*********************************************************************
* @purpose  Obtain a pointer to the first free interface config struct
*
* @param    intIfNum @b{(input)}  Internal Interface Number
* @param    **pCfg   @b{(output)} Ptr  to isdp port config structure
*                                 or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @comments Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @end
*********************************************************************/
L7_BOOL isdpIntfConfigEntryGet(L7_uint32 intIfNum, isdpIntfCfgData_t **pCfg)
{
  L7_uint32     i            = 0;
  nimConfigID_t cfgId;
  nimConfigID_t configIdNull;
  L7_RC_t       rc           = L7_SUCCESS;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));
  memset(&cfgId,        0, sizeof(nimConfigID_t));

  if (!(ISDP_IS_READY))
  {
    isdpDebugTrace(ISDP_DBG_FLAG_ConfigGroup,
                   "%s: LINE %d: isdp isn't ready \n",
                   __FUNCTION__, __LINE__);

    return L7_FALSE;
  }

  if ((rc = nimConfigIdGet(intIfNum, &cfgId)) == L7_SUCCESS)
  {
    for (i = 1; i < L7_ISDP_INTF_MAX_COUNT; i++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&isdpCfg->cfg.intfCfgData[i].cfgId,
                                 &configIdNull))
      {
        isdpMapTbl[intIfNum] = i;
        *pCfg = &isdpCfg->cfg.intfCfgData[i];
        return L7_TRUE;
      }
    }
  }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Handles events generated by NIM
*
* @param    intIfNum   @b{(input)) internal interface number
* @param    intfEvent  @b{(input)) interface event
* @param    correlator @b{(input)) Correlator for the event
*
* @returns  L7_SUCCESS  on a successful operation
* @returns  L7_FAILURE  for any error
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t isdpIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event,
    NIM_CORRELATOR_t correlator)
{
  isdpMsg_t msg;
  L7_RC_t rc = L7_SUCCESS;
  NIM_EVENT_COMPLETE_INFO_t status;

  status.intIfNum        = intIfNum;
  status.component       = L7_ISDP_COMPONENT_ID;
  status.response.rc     = L7_SUCCESS;
  status.response.reason = NIM_ERR_RC_UNUSED;
  status.event           = event;
  status.correlator      = correlator;

  if (isdpProcessQueue == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_ISDP_COMPONENT_ID, "isdpIntfChangeCallback is not ready");
    nimEventStatusCallback(status);
    return L7_FAILURE;
  }

  if (!(ISDP_IS_READY))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_ISDP_COMPONENT_ID, "Received an interface callback while outside the EXECUTE state");
    nimEventStatusCallback(status);
    return L7_FAILURE;
  }

  if (isdpIsValidIntf(intIfNum) != L7_TRUE)
  {
    status.response.rc = L7_SUCCESS;
    nimEventStatusCallback(status);
    return L7_SUCCESS;
  }

  /* LAG events are purposely not processed by ISDP */
  if ((event != L7_CREATE      && event != L7_DELETE)      &&
      (event != L7_ACTIVE      && event != L7_INACTIVE)   && 
      (event != L7_PROBE_SETUP && event != L7_PROBE_TEARDOWN))
  {
    status.response.rc = L7_SUCCESS;
    nimEventStatusCallback(status);
    return L7_SUCCESS;
  }

  memset( (void *)&msg, 0, sizeof(isdpMsg_t) );

  msg.event    = ISDP_INTF_EVENT;
  msg.intIfNum = intIfNum;

  memcpy(&msg.data.status, &status, (L7_uint32) sizeof(NIM_EVENT_COMPLETE_INFO_t));

  rc = isdpProcessMsgQueue(msg);

  return rc;
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
static L7_RC_t isdpIntfCreate(L7_uint32 intIfNum)
{
  nimConfigID_t      cfgId;
  isdpIntfCfgData_t *pCfg = L7_NULLPTR;
  L7_uint32          i = 0;

  memset(&cfgId, 0, sizeof(nimConfigID_t));

  if (isdpIsValidIntf(intIfNum) != L7_TRUE)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_NimGroup,
                   "%s: LINE %d: Wrong interface %d\n",
                   __FUNCTION__, __LINE__, intIfNum);

    return L7_FAILURE;
  }

  if (nimConfigIdGet(intIfNum, &cfgId) != L7_SUCCESS)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_NimGroup,
                   "%s: LINE %d: Can't get config id for interface %d\n",
                   __FUNCTION__, __LINE__, intIfNum);

    return L7_FAILURE;
  }

  for (i = 1; i < L7_ISDP_INTF_MAX_COUNT; i++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&isdpCfg->cfg.intfCfgData[i].cfgId, &cfgId))
    {
      isdpMapTbl[intIfNum] = i;
      break;
    }
  }

  /* If an interface configuration entry is not already
     assigned to the interface, assign one */

  if(isdpIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    if (isdpIntfConfigEntryGet(intIfNum, &pCfg) != L7_TRUE)
    {
      isdpDebugTrace(ISDP_DBG_FLAG_NimGroup,
                   "%s: LINE %d: isdpIntfConfigEntryGet failed, intf = %d\n",
                   __FUNCTION__, __LINE__, intIfNum);

      return L7_FAILURE;
    }

    /* Update the configuration structure with the config id */
    if (pCfg != L7_NULL)
    {
      isdpBuildDefaultIntfConfigData(&cfgId, pCfg);
    }
  }

  memset(&isdpStats->intfStats[intIfNum], 0, sizeof(isdpIntfStats_t));

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
static L7_RC_t isdpIntfDelete(L7_uint32 intIfNum)
{
  isdpIntfCfgData_t *pCfg    = L7_NULLPTR;

  /* delete the configuration entry and all related tables for this interface */
  if (isdpIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_NimGroup,
                   "%s: LINE %d: Can't map interface %d\n",
                   __FUNCTION__, __LINE__, intIfNum);

    return L7_FAILURE;
  }

  memset((void *)&pCfg->cfgId, 0, sizeof(nimConfigID_t));
  memset((void *)&isdpMapTbl[intIfNum], 0, sizeof(L7_uint32));
  memset((void *)&isdpIntfTbl[intIfNum], 0, sizeof(isdpIntfInfoData_t));
  memset((void *)&isdpStats->intfStats[intIfNum], 0, sizeof(isdpIntfStats_t));
  isdpCfg->hdr.dataChanged = L7_TRUE;

  isdpIntfAllEntryDelete(intIfNum);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_ACTIVE
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
static L7_RC_t isdpIntfActive(L7_uint32 intIfNum)
{
  isdpIntfCfgData_t *pCfg = L7_NULLPTR;

  /* get the configuration entry for this interface */
  if (isdpIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_NimGroup,
                   "%s: LINE %d: Can't map interface %d\n",
                   __FUNCTION__, __LINE__, intIfNum);

    return L7_FAILURE;
  }

  isdpIntfTbl[intIfNum].active = L7_TRUE;
  (void)isdpIntfModeApply(intIfNum, pCfg->mode);
  
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_INACTIVE
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
static L7_RC_t isdpIntfInactive(L7_uint32 intIfNum)
{
  isdpIntfCfgData_t *pCfg = L7_NULLPTR;

  /* get the configuration entry for this interface */
  if (isdpIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_NimGroup,
                   "%s: LINE %d: Can't map interface %d\n",
                   __FUNCTION__, __LINE__, intIfNum);

    return L7_FAILURE;
  }

  isdpIntfTbl[intIfNum].active = L7_FALSE;
  /*(void)isdpIntfModeApply(intIfNum, L7_DISABLE);*/ 

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_LAG_ACQUIRE,
*                                       L7_PROBE_SETUP
*
* @param    L7_uint32   intIfNum  internal interface number
* @param    intfEvent   Nim event
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t isdpIntfAcquire(L7_uint32 intIfNum,
                                            L7_uint32 intfEvent)
{
  isdpIntfCfgData_t *pCfg        = L7_NULLPTR;
  L7_BOOL           prevAcquired = L7_FALSE;

  /* get the configuration entry for this interface */
  if (isdpIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_NimGroup,
                   "%s: LINE %d: Can't map interface %d\n",
                   __FUNCTION__, __LINE__, intIfNum);

    return L7_FAILURE;
  }

  COMPONENT_ACQ_NONZEROMASK(isdpIntfTbl[intIfNum].acquiredList, prevAcquired);

  /* update aquired list */
#if 0  /* LAG events are not purposefully processed by ISDP */
  if (intfEvent == L7_LAG_ACQUIRE)
  {
    COMPONENT_ACQ_SETMASKBIT(isdpIntfTbl[intIfNum].acquiredList,
                             L7_DOT3AD_COMPONENT_ID);
  }
#else 
  if (intfEvent == L7_PROBE_SETUP)
  {
    COMPONENT_ACQ_SETMASKBIT(isdpIntfTbl[intIfNum].acquiredList,
                             L7_PORT_MIRROR_COMPONENT_ID);
  }
#endif

  /* If the interface was not previously acquired by some component, cause
   * the acquisition to be reflected in the state of the spanning tree
   */
  if (prevAcquired == L7_FALSE)
  {
    /*Stop transmitting out of the interface.*/
    /* Signal the interface as being unavailable to ISDP */
    isdpIntfTbl[intIfNum].intfAcquired = L7_TRUE;
    (void)isdpIntfModeApply(intIfNum, L7_DISABLE);
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_LAG_RELEASE,
*                                       L7_PROBE_TEARDOWN
*
* @param    L7_uint32   intIfNum  internal interface number
* @param    intfEvent   Nim event
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t isdpIntfRelease(L7_uint32 intIfNum, L7_uint32 intfEvent)
{
  isdpIntfCfgData_t *pCfg        = L7_NULLPTR;
  L7_BOOL           currAcquired = L7_FALSE;

  /* get the configuration entry for this interface */
  if (isdpIntfIsConfigurable(intIfNum, &pCfg) == L7_FALSE)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_NimGroup,
                   "%s: LINE %d: Can't map interface %d\n",
                   __FUNCTION__, __LINE__, intIfNum);

    return L7_FAILURE;
  }

   /* update aquired list */
#if 0  /* LAG events are not purposefully processed by ISDP */
  if (intfEvent == L7_LAG_RELEASE)
  {
    COMPONENT_ACQ_CLRMASKBIT(isdpIntfTbl[intIfNum].acquiredList,
                             L7_DOT3AD_COMPONENT_ID);
  }
#endif
  if (intfEvent == L7_PROBE_TEARDOWN)
  {
    COMPONENT_ACQ_CLRMASKBIT(isdpIntfTbl[intIfNum].acquiredList,
                             L7_PORT_MIRROR_COMPONENT_ID);
  }

  /* check if currently acquired */
  COMPONENT_ACQ_NONZEROMASK(isdpIntfTbl[intIfNum].acquiredList, currAcquired);

  /* If the interface is not currently acquired by some component, cause
   * the acquisition to be reflected in the state of the spanning tree
   */
  if (currAcquired == L7_FALSE)
  {
    isdpIntfTbl[intIfNum].intfAcquired = L7_FALSE;
    (void)isdpIntfModeApply(intIfNum, pCfg->mode);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process Link state changes
*
* @param    intIfnum @b{(input)} interface number
* @param    intIfEvent @b{(input)} interface event
* @param    @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 isdpIntfChangeProcess(L7_uint32 intIfNum, NIM_EVENT_COMPLETE_INFO_t status)
{
  L7_RC_t rc = L7_SUCCESS;

  if (isdpIsValidIntf(intIfNum) != L7_TRUE)
  {
    rc = L7_SUCCESS;
    status.response.rc  = rc;
    nimEventStatusCallback(status);
    return rc;
  }

  /*------------------*/
  /* Handle the event */
  /*------------------*/

  switch (status.event)
  {
  case (L7_CREATE):
    rc = isdpIntfCreate(intIfNum);
    break;

  case (L7_DELETE):
    rc = isdpIntfDelete(intIfNum);
    break;

  case (L7_ACTIVE):
    rc = isdpIntfActive(intIfNum);
    break;

  case (L7_INACTIVE):
    rc = isdpIntfInactive(intIfNum);
    break;

  /* ISDP purposely does not respond to LAG events */
  case (L7_PROBE_SETUP):
    rc = isdpIntfAcquire(intIfNum, status.event);
    break;

  case (L7_PROBE_TEARDOWN):
    rc = isdpIntfRelease(intIfNum, status.event);
    break;

  default:
    rc = L7_SUCCESS;
  }

  status.response.rc  = rc;
  nimEventStatusCallback(status);

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
void isdpIntfStartupCallback(NIM_STARTUP_PHASE_t startupPhase)
{
  isdpMsg_t msg;

  if (isdpProcessQueue == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "%s: Rcvd NIM startup %d callback when ISDP msg queue not created.",
            __FUNCTION__, startupPhase);
    return;
  }

  if (isdpCnfgrState != ISDP_PHASE_EXECUTE)
  {
    nimStartupEventDone(L7_ISDP_COMPONENT_ID);
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "%s: Rcvd NIM startup %d callback in ISDP init state %d.",
            __FUNCTION__, startupPhase, isdpCnfgrState);
    return;
  }

  memset((void *)&msg, 0, sizeof(isdpMsg_t));
  msg.event = ISDP_INTF_STARTUP_EVENT;
  msg.data.nimStartupEvent.startupPhase = startupPhase;

  if (isdpProcessMsgQueue(msg) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "%s: Failed to send NIM startup %s msg to ISDP msg queue.",
            __FUNCTION__, startupPhase);
  }

  return;
}

/*********************************************************************
* @purpose  Process NIM startup callback for interface create event
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t isdpNimCreateStartupProcess(void)
{
  L7_uint32  intf;
  L7_RC_t    rc;

  rc = nimFirstValidIntfNumber(&intf);
  while (rc == L7_SUCCESS)
  {
    if (isdpIsValidIntf(intf) == L7_TRUE)
    {
      if (isdpIntfCreate(intf) != L7_SUCCESS)
      {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(intf, L7_SYSNAME, ifName);

        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
                "%s: Failed to create intf %u, %s.", __FUNCTION__, intf, ifName);
      }
    }
    rc = nimNextValidIntfNumber(intf, &intf);
  }

  /* set NIM port event mask to receive individual events */
  PORTEVENT_SETMASKBIT(isdpNimEventMask, L7_CREATE);
  PORTEVENT_SETMASKBIT(isdpNimEventMask, L7_DELETE);

  nimRegisterIntfEvents(L7_ISDP_COMPONENT_ID, isdpNimEventMask);
  nimStartupEventDone(L7_ISDP_COMPONENT_ID);
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
L7_RC_t isdpNimActivateStartupProcess(void)
{
  L7_uint32         intf,  intfEvent = 0, activeState = 0;
  L7_INTF_STATES_t  intfState;
  L7_BOOL           probeSetup = L7_FALSE;
  L7_RC_t           rc;
  isdpIntfCfgData_t *pCfg = L7_NULLPTR;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];

  rc = nimFirstValidIntfNumber(&intf);
  while (rc == L7_SUCCESS)
  {
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
        if (isdpIsValidIntf(intf) != L7_TRUE)
        {
          break;
        }
        if ((probeSetup=mirrorIsActiveProbePort(intf)) == L7_TRUE)
        {
          intfEvent = L7_PROBE_SETUP;
        }

        if (probeSetup == L7_TRUE)
        {
          if (isdpIntfAcquire(intf, intfEvent) != L7_SUCCESS)
          {
            nimGetIntfName(intf, L7_SYSNAME, ifName);

            L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
                    "%s: isdpIntfAcquire failed for intf %d, %s.", __FUNCTION__, intf, ifName);
          }
        }
        else
        {
          /* get the configuration entry for this interface */
          if (isdpIntfIsConfigurable(intf, &pCfg) == L7_FALSE)
          {
            isdpDebugTrace(ISDP_DBG_FLAG_NimGroup,
                   "%s: LINE %d: Can't map interface %d\n",
                   __FUNCTION__, __LINE__, intf);

          }
          (void)isdpIntfPolicyApply(intf, pCfg->mode);

          if ((nimGetIntfLinkState(intf, &activeState) == L7_SUCCESS) &&
              (activeState == L7_UP))
          {
            if (isdpIntfActive(intf) != L7_SUCCESS)
            {
              nimGetIntfName(intf, L7_SYSNAME, ifName);

              L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
                      "%s: isdpIntfActive failed for intf %d, %s.", __FUNCTION__, intf, ifName);
            }
          }
        }
        break;
      default:
        nimGetIntfName(intf, L7_SYSNAME, ifName);
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_ISDP_COMPONENT_ID,
                "%s: Rcvd unknown NIM intf state %d for intf %d, %s.",
                __FUNCTION__, intfState, intf, ifName);
        break;
    }

    rc = nimNextValidIntfNumber(intf, &intf);
  }

  /* set NIM port event mask to receive individual events (based on isdpIntfChangeCallback) */
  PORTEVENT_SETMASKBIT(isdpNimEventMask, L7_ATTACH);
  PORTEVENT_SETMASKBIT(isdpNimEventMask, L7_DETACH);
  PORTEVENT_SETMASKBIT(isdpNimEventMask, L7_ACTIVE);
  PORTEVENT_SETMASKBIT(isdpNimEventMask, L7_INACTIVE);
  PORTEVENT_SETMASKBIT(isdpNimEventMask, L7_PROBE_SETUP);
  PORTEVENT_SETMASKBIT(isdpNimEventMask, L7_PROBE_TEARDOWN);

  nimRegisterIntfEvents(L7_ISDP_COMPONENT_ID, isdpNimEventMask);
  nimStartupEventDone(L7_ISDP_COMPONENT_ID);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determine if the interface is valid
*
* @param    sysIntfType  @b{(input)} interface type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL isdpIsValidIntfType(L7_uint32 sysIntfType)
{
  switch (sysIntfType)
  {
  case L7_PHYSICAL_INTF:

      return L7_TRUE;
      break;

    default:
      break;
  }

  isdpDebugTrace(ISDP_DBG_FLAG_NimGroup,
                 "%s: LINE %d: Interface type %d is not valid\n",
                 __FUNCTION__, __LINE__, sysIntfType);

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Determine if the interface is valid for isdp
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL isdpIsValidIntf(L7_uint32 intIfNum)
{
  L7_uint32 sysIntfType = 0;

  if (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS)
  {
    return isdpIsValidIntfType(sysIntfType);
  }

  isdpDebugTrace(ISDP_DBG_FLAG_NimGroup,
                 "%s: LINE %d: Interface %d is not valid\n",
                 __FUNCTION__, __LINE__, intIfNum);

  return L7_FALSE;
}

/*********************************************************************
*
* @purpose  To delete neighbors tree.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Destroys the instance of tree, releasing the previously
*           allocated database and control space.
*
* @end
*
*********************************************************************/
L7_RC_t isdpTreeDelete(void)
{
  return avlDeleteAvlTree(&isdpEntryTree->treeData);
}

/*****************************************************************
* @purpose  Searches for entry in the specified tree
*
* @param    deviceId    @b{(input)}    device id
*
* @returns  void *      pointer to the item if matched
* @returns  NULL        if item does not exist in the tree
*
* @notes
*
* @end
*****************************************************************/
isdpEntry_t * isdpEntryFind(isdpEntryKey_t key)
{
  isdpEntry_t *pEntry = L7_NULLPTR;

  osapiSemaTake(isdpEntryTree->treeData.semId, L7_WAIT_FOREVER);
  pEntry = avlSearchLVL7(&isdpEntryTree->treeData, &key, AVL_EXACT);
  osapiSemaGive(isdpEntryTree->treeData.semId);

  return pEntry;
}

/*****************************************************************
* @purpose  Searches for entry in the specified tree
*
* @param    deviceId    @b{(input)}    device id
*
* @returns  void *      pointer to the item if matched
* @returns  NULL        if item does not exist in the tree
*
* @notes
*
* @end
*****************************************************************/
isdpEntry_t * isdpEntryDeviceIdFind(isdpEntryKey_t key, L7_uchar8* deviceId)
{
  isdpEntry_t *pEntry    = L7_NULLPTR;
  L7_uchar8   localDevId[L7_ISDP_DEVICE_ID_LEN] = "";

  if (L7_NULLPTR != deviceId)
  {
    osapiStrncpySafe(localDevId, deviceId, L7_ISDP_DEVICE_ID_LEN);
  }

  pEntry = isdpEntryGetNext(key);
  while(pEntry != L7_NULLPTR)
  {
    if(strcmp(pEntry->key.deviceId, localDevId) == 0)
    {
      return pEntry;
    }

    pEntry = isdpEntryGetNext(pEntry->key);
  }

  return L7_NULL;
}

/*****************************************************************
* @purpose  Add an entry into the specified tree
*
* @param    pData       @b{(input)}    pointer to the data to be added
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*****************************************************************/
L7_RC_t isdpEntryAdd(isdpEntry_t * pEntry)
{
    isdpEntry_t *pEntryDup = L7_NULLPTR;
    L7_RC_t rc = L7_SUCCESS;

    if (pEntry == L7_NULLPTR)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_ISDP_COMPONENT_ID, "Param is NULL");
      return L7_FAILURE;
    }

    osapiSemaTake(isdpEntryTree->treeData.semId, L7_WAIT_FOREVER);

    pEntryDup = avlInsertEntry(&isdpEntryTree->treeData, pEntry);
    if (pEntryDup == pEntry)
    { /* no more room in table */

      isdpInsertEntryDropsInc();
      isdpDebugTrace(ISDP_DBG_FLAG_DatabaseGroup,
                     "%s: LINE %d: neighbors tree is full \n",
                     __FUNCTION__, __LINE__);

      rc = L7_TABLE_IS_FULL;
    }
    else if (pEntryDup != L7_NULLPTR)
    { /* found existing entry */

      rc = isdpEntryDeleteImpl(pEntryDup);
      if(rc != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_ISDP_COMPONENT_ID, "couldn't delete the entry");
        osapiSemaGive(isdpEntryTree->treeData.semId);
        return rc;
      }

      avlInsertEntry(&isdpEntryTree->treeData, pEntry);

      isdpDebugTrace(ISDP_DBG_FLAG_DatabaseGroup,
                     "%s: LINE %d: updating existing entry \n",
                     __FUNCTION__, __LINE__);

      rc = L7_SUCCESS;
    }
    else
    { /* new entry was successfully inserted */

      isdpDebugTrace(ISDP_DBG_FLAG_DatabaseGroup,
                     "%s: LINE %d: entry was inserted\n",
                     __FUNCTION__, __LINE__);
    }

    osapiSemaGive(isdpEntryTree->treeData.semId);

    isdpNeighborsLastChangeTimeUpdate();

    return rc;
}

/*****************************************************************
* @purpose  Delete an entry from the specified tree
*
* @param    vlanID      @b{(input)}     pointer to the data to be added
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*****************************************************************/
L7_RC_t isdpEntryDelete(isdpEntryKey_t key)
{
  L7_RC_t     rc      = L7_SUCCESS;
  isdpEntry_t *pEntry = L7_NULLPTR;

  osapiSemaTake(isdpEntryTree->treeData.semId, L7_WAIT_FOREVER);

  pEntry = avlSearchLVL7(&isdpEntryTree->treeData, &key, AVL_EXACT);
  if(pEntry == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_ISDP_COMPONENT_ID, "entry not found");
    rc = L7_FAILURE;
  }

  rc = isdpEntryDeleteImpl(pEntry);
  if(rc == L7_FAILURE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_ISDP_COMPONENT_ID, "couldn't delete entry");
  }

  osapiSemaGive(isdpEntryTree->treeData.semId);

  return rc;
}

/*****************************************************************
* @purpose  Obtain count of entries in neighbors tree
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*****************************************************************/
L7_uint32 isdpDataCountEntryTree(void)
{
    L7_uint32 count = 0;

    osapiSemaTake(isdpEntryTree->treeData.semId, L7_WAIT_FOREVER);
    count = avlTreeCount(&isdpEntryTree->treeData);
    osapiSemaGive(isdpEntryTree->treeData.semId);
    return(count);
}

/*****************************************************************
* @purpose  Obtain pointer to the next entry item in the tree
*
* @param    deviceId  @b{(output)} device id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*****************************************************************/
isdpEntry_t * isdpEntryGetNext(isdpEntryKey_t key)
{
  isdpEntry_t *pEntry = L7_NULLPTR;

  osapiSemaTake(isdpEntryTree->treeData.semId, L7_WAIT_FOREVER);

  pEntry = avlSearchLVL7(&isdpEntryTree->treeData, &key, AVL_NEXT);

  osapiSemaGive(isdpEntryTree->treeData.semId);

  return pEntry;
}

/*****************************************************************
* @purpose  Fill neighbor entry from isdp packet
*
* @param    pIsdpEntry  @b{(output)} isdp neighbor entry
* @param    pdu         @b{(input)}  isdp packet
* @param    intIfNum    @b{(input)}  interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*****************************************************************/
L7_RC_t isdpFillEntry(isdpEntry_t *pIsdpEntry, isdpPacket_t *pdu, L7_uint32 intIfNum)
{
  if(pIsdpEntry == L7_NULLPTR || pdu == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_ISDP_COMPONENT_ID, "Param/s is/are NULL");
    return L7_FAILURE;
  }

  osapiStrncpySafe(pIsdpEntry->key.deviceId, pdu->isdp_device_id, L7_ISDP_DEVICE_ID_LEN);
  pIsdpEntry->key.intIfNum = intIfNum;

  pIsdpEntry->holdTime = pdu->isdp_header.holdTime;
  pIsdpEntry->protoVersion = pdu->isdp_header.version;
  pIsdpEntry->capabilities = pdu->isdp_capabilities;
  osapiStrncpySafe(pIsdpEntry->platform, pdu->isdp_platform, L7_ISDP_PLATFORM_LEN);
  osapiStrncpySafe(pIsdpEntry->portId, pdu->isdp_port_id, L7_ISDP_PORT_ID_LEN);
  osapiStrncpySafe(pIsdpEntry->version, pdu->isdp_version, L7_ISDP_VERSION_LEN);
  pIsdpEntry->ipAddressList = pdu->ipAddressList;
  pIsdpEntry->addressNumber = pdu->isdp_address_number;
  pIsdpEntry->lastChangeTime = osapiUpTimeRaw();
  memcpy(pIsdpEntry->mac.addr, pdu->ether.src.addr, sizeof(pIsdpEntry->mac.addr));
  pIsdpEntry->voipVlan = pdu->isdp_voip_vlan;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Increment the number of isdp pdu received
*
* @param    intIfNum    @b{(input)}  interface number
*
* @returns
*
* @comments
*
* @end
*********************************************************************/
void isdpTrafficPduReceivedInc(L7_uint32 intIfNum)
{
  isdpIntfCfgData_t *pCfg        = L7_NULLPTR;

  isdpStats->globalStats.pduRx++;
  if (isdpIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    isdpStats->intfStats[intIfNum].pduRx++;
  }
}

/*********************************************************************
* @purpose  Increment the number of isdp v1 pdu received
*
* @param    intIfNum    @b{(input)}  interface number
*
* @returns
*
* @comments
*
* @end
*********************************************************************/
void isdpTrafficV1PduReceivedInc(L7_uint32 intIfNum)
{
  isdpIntfCfgData_t *pCfg        = L7_NULLPTR;

  isdpStats->globalStats.v1PduRx++;
  if (isdpIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    isdpStats->intfStats[intIfNum].v1PduRx++;
  }
}

/*********************************************************************
* @purpose  Increment the number of isdp v2 pdu received
*
* @param    intIfNum    @b{(input)}  interface number
*
* @returns
*
* @comments
*
* @end
*********************************************************************/
void isdpTrafficV2PduReceivedInc(L7_uint32 intIfNum)
{
  isdpIntfCfgData_t *pCfg        = L7_NULLPTR;

  isdpStats->globalStats.v2PduRx++;
  if (isdpIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    isdpStats->intfStats[intIfNum].v2PduRx++;
  }
}

/*********************************************************************
* @purpose  Increment the number of isdp pdu received with bad header
*
* @param    intIfNum    @b{(input)}  interface number
*
* @returns
*
* @comments
*
* @end
*********************************************************************/
void isdpTrafficBadHeaderPduReceivedInc(L7_uint32 intIfNum)
{
  isdpIntfCfgData_t *pCfg        = L7_NULLPTR;

  isdpStats->globalStats.badHeaderRx++;
  if (isdpIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    isdpStats->intfStats[intIfNum].badHeaderRx++;
  }
}

/*********************************************************************
* @purpose  Increment the number of isdp pdu received with checksum
* error
*
* @param    intIfNum    @b{(input)}  interface number
*
* @returns
*
* @comments
*
* @end
*********************************************************************/
void isdpTrafficChkSumErrorPduReceivedInc(L7_uint32 intIfNum)
{
  isdpIntfCfgData_t *pCfg        = L7_NULLPTR;

  isdpStats->globalStats.chkSumErrorRx++;
  if (isdpIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    isdpStats->intfStats[intIfNum].chkSumErrorRx++;
  }
}

/*********************************************************************
* @purpose  Increment the number of isdp pdu received with invalid
* format
*
* @param    intIfNum    @b{(input)}  interface number
*
* @returns
*
* @comments
*
* @end
*********************************************************************/
void isdpTrafficInvalidFormatPduReceivedInc(L7_uint32 intIfNum)
{
  isdpIntfCfgData_t *pCfg        = L7_NULLPTR;

  isdpStats->globalStats.invalidFormatRx++;
  if (isdpIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    isdpStats->intfStats[intIfNum].invalidFormatRx++;
  }
}

/*********************************************************************
* @purpose  Increment the number of isdp pdu transmitted
*
* @param    intIfNum    @b{(input)}  interface number
*
* @returns
*
* @comments
*
* @end
*********************************************************************/
void isdpTrafficPduTransmitInc(L7_uint32 intIfNum)
{
  isdpIntfCfgData_t *pCfg        = L7_NULLPTR;

  isdpStats->globalStats.pduTx++;
  if (isdpIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    isdpStats->intfStats[intIfNum].pduTx++;
  }
}

/*********************************************************************
* @purpose  Increment the number of isdp v1 pdu transmitted
*
* @param    intIfNum    @b{(input)}  interface number
*
* @returns
*
* @comments
*
* @end
*********************************************************************/
void isdpTrafficV1PduTransmitInc(L7_uint32 intIfNum)
{
  isdpIntfCfgData_t *pCfg        = L7_NULLPTR;

  isdpStats->globalStats.v1PduTx++;
  if (isdpIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    isdpStats->intfStats[intIfNum].v1PduTx++;
  }
}

/*********************************************************************
* @purpose  Increment the number of isdp v2 pdu transmitted
*
* @param    intIfNum    @b{(input)}  interface number
*
* @returns
*
* @comments
*
* @end
*********************************************************************/
void isdpTrafficV2PduTransmitInc(L7_uint32 intIfNum)
{
  isdpIntfCfgData_t *pCfg        = L7_NULLPTR;

  isdpStats->globalStats.v2PduTx++;
  if (isdpIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    isdpStats->intfStats[intIfNum].v2PduTx++;
  }
}

/*********************************************************************
* @purpose  Increment the number of isdp pdu transmitted failures
*
* @param    intIfNum    @b{(input)}  interface number
*
* @returns
*
* @comments
*
* @end
*********************************************************************/
void isdpTrafficFailurePduTransmitInc(L7_uint32 intIfNum)
{
  isdpIntfCfgData_t *pCfg        = L7_NULLPTR;

  isdpStats->globalStats.failureTx++;
  if (isdpIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    isdpStats->intfStats[intIfNum].failureTx++;
  }
}

/*********************************************************************
* @purpose  Clear isd statistics
*
* @returns
*
* @comments
*
* @end
*********************************************************************/
void isdpStatsClear(void)
{
  memset((void *)isdpStats, 0x00, (L7_uint32)sizeof(isdpStats_t));
}

/*********************************************************************
* @purpose  Increment the number of neighbor entry insert failures
*
* @returns
*
* @comments
*
* @end
*********************************************************************/
void isdpInsertEntryDropsInc(void)
{
  isdpStats->insertEntryDrops++;
}

/*********************************************************************
* @purpose  Increment the number of insert failures to ip address buffer
*
* @returns
*
* @comments
*
* @end
*********************************************************************/
void isdpInsertIpAddressDropsInc(void)
{
  isdpStats->insertIpAddressDrops++;
}

/*********************************************************************
* @purpose  Record a neighbor table change.
*
*
* @returns  L7_SUCCESS,
*           L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpNeighborsLastChangeTimeUpdate()
{
  isdpStats->lastChangeTime = osapiUpTimeRaw();
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Allocate an ip address entry from the buffer pool
*
* @param    **pIpAddrList  @b((input)) current interface list
*
* @returns  isdpIpAddrEntry_t*
* @returns  L7_NULLPTR, no buffer available
*
* @notes    none
*
* @end
*********************************************************************/
isdpIpAddressBuffer_t* isdpIpAddrBufferGet(isdpIpAddressBuffer_t **pIpAddrList)
{
  L7_uchar8 *buffer = L7_NULLPTR;

  if (pIpAddrList == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_ISDP_COMPONENT_ID, "Param is NULL");
    return L7_NULLPTR;
  }

  if ((bufferPoolAllocate(isdpIpAddrPoolId, &buffer) != L7_SUCCESS) ||
      (buffer == L7_NULLPTR))
  {
    isdpInsertIpAddressDropsInc();
    return L7_NULLPTR;
  }

  memset((void *)buffer, 0, sizeof(isdpIpAddressBuffer_t));

  if (*pIpAddrList == L7_NULLPTR)
  {
    *pIpAddrList = (isdpIpAddressBuffer_t *)buffer;
  }
  else
  {
    isdpIpAddressBuffer_t *p = L7_NULLPTR;
    for (p = *pIpAddrList; p->next; p = p->next);
    p->next = (isdpIpAddressBuffer_t *)buffer;
  }

  return (isdpIpAddressBuffer_t *)buffer;
}

/*********************************************************************
* @purpose  Deallocate an ip address entry list from the buffer pool
*
* @param    *ipAddrlist  @b((input)) current ip address list
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void isdpIpAddrListRelease(isdpIpAddressBuffer_t **pIpAddrlist)
{
  L7_uchar8   *p      = L7_NULLPTR;
  L7_uchar8   *next   = L7_NULLPTR;

  if (pIpAddrlist == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_ISDP_COMPONENT_ID, "Param is NULL");
    return;
  }

  /* return allocated buffer to buffer pools */
  p = (L7_uchar8 *)*pIpAddrlist;
  while (p != L7_NULLPTR)
  {
    next = (L7_uchar8 *)((isdpIpAddressBuffer_t *)p)->next;
    bufferPoolFree(isdpIpAddrPoolId, p);
    p = next;
  }

  *pIpAddrlist = L7_NULLPTR;
}

/*********************************************************************
* @purpose  Get next ip address from address list using index
*
* @param    *ipAddrlist  @b((input))  ip address list
* @param    index        @b((input))  ip address index
* @param    *address     @b((output)) ip address
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    indexing starts from 1
*
* @end
*********************************************************************/
L7_RC_t isdpIpAddrListIndexGet(isdpIpAddressBuffer_t *ipAddrList,
                               L7_uint32 index,
                               L7_uint32 *address)
{
  L7_uint32 i = 1;

  if (ipAddrList == L7_NULLPTR)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_DatabaseGroup,
                   "%s: LINE %d: address list is empty \n",
                   __FUNCTION__, __LINE__);

    return L7_FAILURE;
  }

  if (index < 1)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_DatabaseGroup,
                   "%s: LINE %d: index is too small %d\n",
                   __FUNCTION__, __LINE__, index);

    return L7_FAILURE;
  }

  while (ipAddrList != L7_NULLPTR)
  {
    if(i == index)
    {
      *address = ipAddrList->ipAddr;
      return L7_SUCCESS;
    }

    i++;
    ipAddrList = ipAddrList->next;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Put the ip address to address list
*
* @param    **pIpAddressList @b((input)) ip address list
* @param    ipAddress        @b((input)) ip address value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t isdpIpAddrPutList(isdpIpAddressBuffer_t **pIpAddressList,
                       L7_uint32 ipAddress)
{
  L7_RC_t               rc    = L7_SUCCESS;
  isdpIpAddressBuffer_t *addr = L7_NULLPTR;

  if (pIpAddressList == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_ISDP_COMPONENT_ID, "param is NULL");
    return L7_FAILURE;
  }

  /* need a ip address buffer */
  addr = isdpIpAddrBufferGet(pIpAddressList);
  if (addr == L7_NULLPTR)
  {
    rc = L7_TABLE_IS_FULL;
  }
  else
  {
    addr->ipAddr = ipAddress;
    addr->next   = L7_NULLPTR;
  }

  return rc;
}

/*********************************************************************
* @purpose  Apply isdp mode
*
* @param    mode      @b{(input)) isdp mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t isdpModeApply(L7_uint32 mode)
{
  L7_RC_t rc = L7_SUCCESS;

  if (mode == L7_DISABLE)
  {
    rc = isdpAllEntryDelete();
    if(rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_ISDP_COMPONENT_ID, "couldn't clean neighbors table!");
    }

    rc = isdpTxRxDeinit();
    if (rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_ISDP_COMPONENT_ID, "Unable to deregister isdp callback with Sysnet!");
      rc = L7_FAILURE;
    }
  }
  else if (mode == L7_ENABLE)
  {
    rc = isdpTxRxInit();
    if (rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_ISDP_COMPONENT_ID, "Unable to register for isdp callback with Sysnet!");
      rc = L7_FAILURE;
    }
  }

  rc = isdpIntfModeApplyAll(mode);

  return rc;
}

/*********************************************************************
* @purpose  Apply isdp mode to specified interface
*
* @param    index     @b{(input)) internal interface index
* @param    mode      @b{(input)) isdp mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t isdpIntfModeApply(L7_uint32 intIfNum, L7_uint32 mode)
{
  L7_RC_t rc    = L7_SUCCESS;
  L7_RC_t rcDtl = L7_SUCCESS;

  if (mode == L7_DISABLE)
  {
    rcDtl = isdpIntfPolicyApply(intIfNum,mode);

    rc = isdpIntfAllEntryDelete(intIfNum);
    if(rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_ISDP_COMPONENT_ID, "Couldn't delete entries");
    }
  }
  else
  {
    rcDtl = isdpIntfPolicyApply(intIfNum,mode);
  }
  if(L7_SUCCESS != rcDtl)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_DebugGroup,
                   "%s: LINE - Dtl call failed for mode %u\n",
                   __FUNCTION__, __LINE__, mode);
  }

  return rc;
}

/*****************************************************************
* @purpose  Delete all entries discovered on the specified interface
*
* @param    intIfNum  @b{(output)} interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
****************************************************************/
L7_RC_t isdpIntfAllEntryDelete(L7_uint32 intIfNum)
{
  isdpEntry_t    *pEntry   = L7_NULLPTR;
  L7_RC_t        rc        = L7_SUCCESS;
  isdpEntryKey_t searchKey;

  memset((void *)&searchKey, 0x00, sizeof(isdpEntryKey_t));

  if(intIfNum != 0 && isdpIsValidIntf(intIfNum) != L7_TRUE)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_ISDP_COMPONENT_ID,
            "Invalid interface %s\n", ifName);
    return L7_FAILURE;
  }

  searchKey.intIfNum = intIfNum;

  osapiSemaTake(isdpEntryTree->treeData.semId, L7_WAIT_FOREVER);

  if (intIfNum != 0)
  {
    isdpIntfDevIdZeroEntryDelete(intIfNum);
  }

  pEntry = avlSearchLVL7(&isdpEntryTree->treeData, &searchKey, AVL_NEXT);

  while(pEntry != L7_NULLPTR)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_DatabaseGroup,
                 "%s: LINE %d: deviceId = %s, intIfNum = %d\n",
                 __FUNCTION__, __LINE__,
                 pEntry->key.deviceId,
                 pEntry->key.intIfNum);

    if(intIfNum != 0 && pEntry->key.intIfNum != intIfNum)
    {
      isdpDebugTrace(ISDP_DBG_FLAG_DatabaseGroup,
                     "%s: LINE %d: no more entries\n",
                     __FUNCTION__, __LINE__);
      break;
    }
    rc = isdpEntryDeleteImpl(pEntry);
    if(rc == L7_FAILURE)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_ISDP_COMPONENT_ID, "couldn't delete entry");
      break;
    }

    isdpDebugTrace(ISDP_DBG_FLAG_DatabaseGroup,
                   "%s: LINE %d: entry deleted\n",
                   __FUNCTION__, __LINE__);

    pEntry = avlSearchLVL7(&isdpEntryTree->treeData, &searchKey, AVL_NEXT);
  }

  osapiSemaGive(isdpEntryTree->treeData.semId);

  return rc;
}

/*****************************************************************
* @purpose  Delete all entries with device id zero discovered on
*           the specified interface
*
* @param    intIfNum  @b{(output)} interface number
*
* @returns  void
*
* @notes    This is an anamoly. Any switch with device 0 should be
*           considered to be a faulty device. But since no
*           standards are present, we wouldn't a stand now.
* @end
****************************************************************/
static void isdpIntfDevIdZeroEntryDelete(L7_uint32 intIfNum)
{
  isdpEntryKey_t searchKey;
  isdpEntry_t    *pEntry   = L7_NULLPTR;
  L7_RC_t        rc        = L7_SUCCESS;

  memset((void *)&searchKey, 0x00, sizeof(isdpEntryKey_t));

  searchKey.intIfNum = intIfNum;

  pEntry = avlSearchLVL7(&isdpEntryTree->treeData, &searchKey, AVL_EXACT);

  while(pEntry != L7_NULLPTR)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_DatabaseGroup,
                 "%s: LINE %d: deviceId = %s, intIfNum = %d\n",
                 __FUNCTION__, __LINE__,
                 pEntry->key.deviceId,
                 pEntry->key.intIfNum);

    rc = isdpEntryDeleteImpl(pEntry);
    if(rc == L7_FAILURE)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_ISDP_COMPONENT_ID, "couldn't delete entry");
      break;
    }

    isdpDebugTrace(ISDP_DBG_FLAG_DatabaseGroup,
                   "%s: LINE %d: entry deleted\n",
                   __FUNCTION__, __LINE__);

    pEntry = avlSearchLVL7(&isdpEntryTree->treeData, &searchKey, AVL_EXACT);
  }

}

/*****************************************************************
* @purpose  Delete all entries from neighbors AVL tree
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
****************************************************************/
L7_RC_t isdpAllEntryDelete(void)
{
  return isdpIntfAllEntryDelete(0);
}

/*****************************************************************
* @purpose  Determines the Device ID
*
*
* @returns  void
*
* @notes    1) uses hostname as device id if the hostname is
*              not configured to be the default.
*           2) If hostname == default, then device id
*              is configured = serial number
*
* @end
****************************************************************/
void isdpDevIdGet(L7_uchar8 *isdp_device_id)
{
  L7_char8  prompt[L7_PROMPT_SIZE];

  cliWebGetSystemCommandPrompt(prompt);
  if ((strcmp(prompt, FD_CLI_WEB_COMMAND_PROMPT) != 0))
  {
    osapiStrncpySafe(isdp_device_id, prompt,L7_ISDP_DEVICE_ID_LEN);
  }
  else
  {
    sysapiRegistryGet(SERIAL_NUM, STR_ENTRY, (void*)isdp_device_id);
  }
}

/*****************************************************************
* @purpose  Returns the Device ID Format
*
* @returns  L7_uint32 (device id)
*
* @notes    1) uses hostname as device id if the hostname is
*              not configured to be the default.
*           2) If hostname == default, then device id
*              is configured = serial number
*
* @end
****************************************************************/
L7_uint32 isdpDevIdFmtGet(void)
{
  L7_char8  prompt[L7_PROMPT_SIZE];
  L7_uint32 isdpCurDevIdFormat;

  cliWebGetSystemCommandPrompt(prompt);
  if ((strcmp(prompt, FD_CLI_WEB_COMMAND_PROMPT) != 0))
  {
    isdpCurDevIdFormat = ISDP_DEVICE_ID_HOST;
  }
  else
  {
    isdpCurDevIdFormat = ISDP_DEVICE_ID_SERIAL;
  }

  return isdpCurDevIdFormat;
}

/*********************************************************************
* @purpose  Apply isdp mode on All isdp interfaces
*
* @param    mode      @b{(input)) isdp mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t isdpIntfModeApplyAll(L7_uint32 mode)
{
  L7_RC_t rc = L7_SUCCESS;
  nimConfigID_t     cfgIdNull;
  isdpIntfCfgData_t *pCfg     = L7_NULLPTR;
  L7_uint32 i;

  memset(&cfgIdNull, 0, sizeof(nimConfigID_t));

  /* Iterate through all ISDP interfaces and apply this mode */
  for (i = 1; i < L7_ISDP_INTF_MAX_COUNT; i++)
  {
    L7_uint32 intIfNum;

    if (NIM_CONFIG_ID_IS_EQUAL(&isdpCfg->cfg.intfCfgData[i].cfgId, &cfgIdNull))
    {
      continue;
    }
  
    if (nimIntIfFromConfigIDGet(&(isdpCfg->cfg.intfCfgData[i].cfgId),
                                  &intIfNum) != L7_SUCCESS)
    {
      continue;
    }

    if (isdpIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
    {
      /* If global mode is enable,then Apply the intf cfg mode,else the same as global*/
      if (L7_ENABLE == mode)
      {
        rc = isdpIntfModeApply(intIfNum, pCfg->mode);
      }
      else
      {
        rc = isdpIntfModeApply(intIfNum, mode);
      }
 
      if (rc != L7_SUCCESS)
      { 
         L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_ISDP_COMPONENT_ID,"isdpIntfModeApply failed on interface:%u!\n",intIfNum);
         rc = L7_FAILURE;
      }
    }
  }
  return rc;
}

/*********************************************************************
* @purpose  Apply ISDP Policy on interface
*
* @param    mode      @b{(input)) isdp mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t isdpIntfPolicyApply(L7_uint32 intIfNum, L7_uint32 mode)
{
  if((L7_ENABLE == isdpCfg->cfg.globalCfgData.mode) && (L7_ENABLE ==  mode))
  {
    return dtlIsdpPortStatusSet(intIfNum, L7_TRUE);
  }
  else if(L7_DISABLE == mode)
  {
    return dtlIsdpPortStatusSet(intIfNum, L7_FALSE);
  }

  return L7_SUCCESS;
}



