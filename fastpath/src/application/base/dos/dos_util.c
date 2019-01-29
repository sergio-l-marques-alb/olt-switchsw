/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename dos_util.c
*
* @purpose DOS componenet utitlity fucntions
*
* @component DENIAL OF SERVICE
*
* @comments none
*
* @create 04/13/2007
*
* @author aprashant
* @end
*
**********************************************************************/
#include "dos_util.h"
#include "dtlapi.h"
#include "dos.h"
#include "dos_api.h"

extern L7_uint32 *dosMapTbl;
extern void *dosQueue;
extern doSCfgData_t *doSCfgData;
extern doSCnfgrState_t doSCnfgrState;
extern osapiRWLock_t doSCfgRWLock;
extern PORTEVENT_MASK_t dosPortEventMask_g;

/*********************************************************************
* @purpose  Obtain a pointer to the specified interface configuration data
*           for this interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to dos interface config structure
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
L7_BOOL dosMapIntfIsConfigurable(L7_uint32 intIfNum, dosIntfCfgData_t **pCfg)
{
  L7_uint32 index;
  nimConfigID_t configId;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  if (!(DOSCONTROL_IS_READY))
    return L7_FALSE;

  /* Check boundary conditions */
  if (intIfNum <= 0 || intIfNum > platIntfPhysicalIntfMaxCountGet())
    return L7_FALSE;

  index = dosMapTbl[intIfNum];

  if (index == 0)
    return L7_FALSE;

  /* verify that the configId in the config data table entry matches the configId that NIM maps to
   ** the intIfNum we are considering
   */
  if (nimConfigIdGet(intIfNum, &configId) == L7_SUCCESS)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&configId, &(doSCfgData->dosIntfCfgData[index].configId)) == L7_FALSE)
    {
      /* if we get here, either we have a table management error between doSCfgData and dosMapTbl or
      ** there is synchronization issue between NIM and components w.r.t. interface creation/deletion
      */
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
              "Error accessing DOS config data for interface %s in dosMapIntfIsConfigurable.\n", ifName);
      return L7_FALSE;
    }
  }

  *pCfg = &doSCfgData->dosIntfCfgData[index];

  return L7_TRUE;
}
/*********************************************************************
* @purpose  Obtain a pointer to the first free interface config struct
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to dos interface config structure
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
L7_BOOL dosMapIntfConfigEntryGet(L7_uint32 intIfNum, dosIntfCfgData_t **pCfg)
{
  L7_uint32 i;
  nimConfigID_t configIdNull;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  /* Avoid N^2 processing when interfaces created at startup */
  static L7_uint32 nextIndex = 1;

  if (!(DOSCONTROL_IS_READY))
    return L7_FALSE;

  if (nextIndex < L7_DOS_MAX_INTF)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&doSCfgData->dosIntfCfgData[nextIndex].configId, &configIdNull))
    {
      dosMapTbl[intIfNum] = nextIndex;
      *pCfg = &doSCfgData->dosIntfCfgData[nextIndex];
      nextIndex++;
      return L7_TRUE;
    }
  }

  /* cached nextIndex is in use. Resort to search from beginning. */
  for (i = 1; i < L7_DOS_MAX_INTF; i++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&doSCfgData->dosIntfCfgData[i].configId, &configIdNull))
    {
      dosMapTbl[intIfNum] = i;
      *pCfg = &doSCfgData->dosIntfCfgData[i];
      nextIndex = i + 1;
      return L7_TRUE;
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
L7_RC_t dosIntfCreate(L7_uint32 intIfNum)
{
  nimConfigID_t      configId;
  dosIntfCfgData_t  *pCfg;
  L7_uint32          i;

  if (dosIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
    return L7_FAILURE;

  for (i = 1; i < L7_DOS_MAX_INTF; i++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&doSCfgData->dosIntfCfgData[i].configId, &configId))
    {
      dosMapTbl[intIfNum] = i;
      break;
    }
  }

  pCfg = L7_NULL;

  /* If an interface configuration entry is not already assigned to the interface,
     assign one */
  if (dosMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    if(dosMapIntfConfigEntryGet(intIfNum, &pCfg) != L7_TRUE)
      return L7_FAILURE;

    /* Update the configuration structure with the config id */
  if (pCfg != L7_NULL)
      dosBuildDefaultIntfConfigData(&configId, pCfg);
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
L7_RC_t dosApplyIntfConfigData(L7_uint32 intIfNum)
{
  dosIntfCfgData_t *pCfg;

  if (dosMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {

    if(dosIntfPingFloodingApply(intIfNum,pCfg->intfPingFlooding,
       pCfg->intfparams[DOSINTFPINGPARAM])!= L7_SUCCESS)
      return L7_FAILURE;

    if(dosIntfSmurfAttackApply(intIfNum,pCfg->intfSmurfAttack)!=L7_SUCCESS)
      return L7_FAILURE;

    if(dosIntfSynAckFloodingApply(intIfNum,pCfg->intfSynAckFlooding,
       pCfg->intfparams[DOSINTFSYNACKPARAM])!= L7_SUCCESS)
      return L7_FAILURE;
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
L7_RC_t dosIntfDetach(L7_uint32 intIfNum)
{
  dosIntfCfgData_t *pCfg;

  if (dosMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {

    if(dosIntfPingFloodingApply(intIfNum,FD_DOS_DEFAULT_PINGFLOODING_MODE,
       FD_DOS_DEFAULT_PING_PARAM)!= L7_SUCCESS)
      return L7_FAILURE;

    if(dosIntfSmurfAttackApply(intIfNum, FD_DOS_DEFAULT_SMURFATTACK_MODE)!=L7_SUCCESS)
      return L7_FAILURE;

    if(dosIntfSynAckFloodingApply(intIfNum, FD_DOS_DEFAULT_SYNACK_MODE,
       FD_DOS_DEFAULT_SYNACK_PARAM)!= L7_SUCCESS)
      return L7_FAILURE;
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
L7_RC_t dosIntfDelete(L7_uint32 intIfNum)
{
  dosIntfCfgData_t *pCfg;

  if (dosMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    memset((void *)&pCfg->configId, 0, sizeof(nimConfigID_t));
    memset((void *)&dosMapTbl[intIfNum], 0, sizeof(L7_uint32));
    doSCfgData->cfgHdr.dataChanged = L7_TRUE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Propogate Interface notifications to dos Task
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
L7_RC_t dosIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event,
NIM_CORRELATOR_t correlator)
{
  dosMgmtMsg_t msg;

  msg.intIfNum = intIfNum;
  msg.dosIntfChangeParms.event = event;
  msg.dosIntfChangeParms.correlator = correlator;

  osapiMessageSend(dosQueue, &msg, DOS_MSG_SIZE, L7_NO_WAIT, L7_MSG_PRIORITY_NORM);

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
L7_RC_t dosIntfChangeProcess(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator)
{
  L7_RC_t                    rc = L7_SUCCESS;
  NIM_EVENT_COMPLETE_INFO_t  status;

  status.intIfNum     = intIfNum;
  status.component    = L7_DOSCONTROL_COMPONENT_ID;
  status.event        = event;
  status.correlator   = correlator;
  status.response.reason = NIM_ERR_RC_UNUSED;

  if (!(DOSCONTROL_IS_READY))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
            "Received an interface change callback while not ready to receive it");
    rc = L7_FAILURE;
        status.response.rc  = rc;
    nimEventStatusCallback(status);
    return rc;
  }

  if (dosIsValidIntf(intIfNum) != L7_TRUE)
  {
    rc = L7_SUCCESS;
    status.response.rc  = rc;
    nimEventStatusCallback(status);
    return rc;
  }
  if (intIfNum>L7_DOS_MAX_INTF)
  {
      rc = L7_SUCCESS;
      status.response.rc = rc;
      nimEventStatusCallback(status);
      return rc;
  }

  switch (event)
  {
    case L7_CREATE:
      (void)osapiWriteLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
      rc = dosIntfCreate(intIfNum);
      (void)osapiWriteLockGive(doSCfgRWLock);
      break;

    case L7_ATTACH:
      rc = dosApplyIntfConfigData(intIfNum);
      break;

    case L7_DETACH:
      rc = dosIntfDetach(intIfNum);
      break;

    case L7_DELETE:
      (void)osapiWriteLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
      rc = dosIntfDelete(intIfNum);
      (void)osapiWriteLockGive(doSCfgRWLock);
      break;

    default:
      rc = L7_SUCCESS;
  }
  status.response.rc = rc;
  nimEventStatusCallback(status);
  return rc;
}
/*********************************************************************
*
* @purpose  Applies the Ping flooding mode for an interface
*
* @param    L7_uint32  intIfNum  @b((input)) Internal interface number
* @param    L7_uint32  mode      @b((input)) Ping flodding intf mode
* @param    L7_uint32  param     @b((input)) Param value
*
* @returns  L7_SUCCESS, if interface mode was successfully applied
* @returns  L7_FAILURE, if interface number is invalid
*
* @notes    This function assumes mode parameter is valid
*
* @end
*********************************************************************/
L7_RC_t dosIntfPingFloodingApply(L7_uint32 intIfNum, L7_uint32 mode ,L7_uint32 param)
{
  return dtlDosIntfPingFlooding(intIfNum, mode, param);
}
/*********************************************************************
*
* @purpose  Applies the Smurf Attack mode for an interface
*
* @param    L7_uint32  intIfNum  @b((input)) Internal interface number
* @param    L7_uint32  mode      @b((input)) Smurf Attack intf mode
*
* @returns  L7_SUCCESS, if interface mode was successfully applied
* @returns  L7_FAILURE, if interface number is invalid
*
* @notes    This function assumes mode parameter is valid
*
* @end
*********************************************************************/
L7_RC_t dosIntfSmurfAttackApply(L7_uint32 intIfNum,L7_uint32 mode)
{
  return dtlDosIntfSmurfAttack(intIfNum, mode);
}
/*********************************************************************
*
* @purpose  Applies the SynAck flooding mode for an interface
*
* @param    L7_uint32  intIfNum  @b((input)) Internal interface number
* @param    L7_uint32  mode      @b((input)) Syn Ack flodding intf mode
* @param    L7_uint32  param     @b((input)) Param value
*
* @returns  L7_SUCCESS, if interface mode was successfully applied
* @returns  L7_FAILURE, if interface number is invalid
*
* @notes    This function assumes mode parameter is valid
*
* @end
*********************************************************************/
L7_RC_t dosIntfSynAckFloodingApply(L7_uint32 intIfNum, L7_uint32  mode, L7_uint32 param)
{
  return dtlDosIntfSynAckFlooding(intIfNum, mode, param);
}

/*********************************************************************
* @purpose  Propogate Startup notifications to dos Task
*
* @param    NIM_STARTUP_PHASE_t  startupPhase
*
* @notes
*
* @end
*********************************************************************/
void dosStartupNotifyCallback(NIM_STARTUP_PHASE_t startupPhase)
{
  L7_RC_t rc;
  L7_uint32 intIfNum;

  rc = nimFirstValidIntfNumber(&intIfNum);

  while (rc == L7_SUCCESS)
  {

    switch (startupPhase)
    {
      case NIM_INTERFACE_CREATE_STARTUP:
        (void)osapiWriteLockTake(doSCfgRWLock, L7_WAIT_FOREVER);
        dosIntfCreate(intIfNum);
        (void)osapiWriteLockGive(doSCfgRWLock);
        break;

      case NIM_INTERFACE_ACTIVATE_STARTUP:
        dosApplyIntfConfigData(intIfNum);
        break;

      default:
        break;
    }
    rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
  }

  switch (startupPhase)
  {
    case NIM_INTERFACE_CREATE_STARTUP:

      /* Register with NIM to receive port CREATE/DELETE events */
      PORTEVENT_SETMASKBIT(dosPortEventMask_g, L7_CREATE);
      PORTEVENT_SETMASKBIT(dosPortEventMask_g, L7_DELETE);
      nimRegisterIntfEvents(L7_DOSCONTROL_COMPONENT_ID, dosPortEventMask_g);
      L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_DOSCONTROL_COMPONENT_ID,
              "DoS Create Startup done");
      break;

    case NIM_INTERFACE_ACTIVATE_STARTUP:

      /* Add ATTACH/DETACH events to our NIM registration */
      PORTEVENT_SETMASKBIT(dosPortEventMask_g, L7_ATTACH);
      PORTEVENT_SETMASKBIT(dosPortEventMask_g, L7_DETACH);
      nimRegisterIntfEvents(L7_DOSCONTROL_COMPONENT_ID, dosPortEventMask_g);
      break;

    default:
      break;
  }

  nimStartupEventDone(L7_DOSCONTROL_COMPONENT_ID);

  /* Inform cnfgr that DOS has completed it HW updates */
  if (startupPhase == NIM_INTERFACE_ACTIVATE_STARTUP)
  {
    cnfgrApiComponentHwUpdateDone(L7_DOSCONTROL_COMPONENT_ID,
                                  L7_CNFGR_HW_APPLY_CONFIG);
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_DOSCONTROL_COMPONENT_ID,
            "DoS Activate Startup done");
  }
}

