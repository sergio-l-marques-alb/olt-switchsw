/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename voip_util.c
*
* @purpose VOIP componenet utitlity fucntions
*
* @component VOIP
*
* @comments none
*
* @create 05/12/2007
*
* @author aprashant
* @end
*
**********************************************************************/
#include "voip_util.h"
#include "dtlapi.h"
#include "voip.h"
#include "voip_cnfgr.h"
#include "voip_api.h"
#include "voip_parse.h"
#include "voip_control.h"
#include "voip_sip.h"
#include "voip_sccp.h"
#include "voip_h323.h"
#include "voip_mgcp.h"
#include "dtl_voip.h"

extern L7_uint32 *voipMapTbl;
extern void *voipQueue;
extern voipCfgData_t    *voipCfgData;
extern voipIntfInfo_t  *voipIntfInfo;
extern osapiRWLock_t   voipCfgRWLock;

/*********************************************************************
* @purpose  Obtain a pointer to the specified interface configuration data
*           for this interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to voip interface config structure
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
L7_BOOL voipMapIntfIsConfigurable(L7_uint32 intIfNum, voipIntfCfgData_t **pCfg)
{
  L7_uint32 index;
  nimConfigID_t configId;

  if (!(VOIP_IS_READY))
    return L7_FALSE;

  /* Check boundary conditions */
  if (intIfNum <= 0 || intIfNum >= platIntfMaxCountGet())
    return L7_FALSE;

  index = voipMapTbl[intIfNum];

  if (index == 0)
    return L7_FALSE;

  /* verify that the configId in the config data table entry matches the configId that NIM maps to
   ** the intIfNum we are considering
   */
  if (nimConfigIdGet(intIfNum, &configId) == L7_SUCCESS)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&configId, &(voipCfgData->voipIntfCfgData[index].configId)) == L7_FALSE)
    {
      /* if we get here, either we have a table management error between doSCfgData and voipMapTbl or
      ** there is synchronization issue between NIM and components w.r.t. interface creation/deletion
      */
      LOG_MSG("Error accessing VOIP config data for interface %d in voipMapIntfIsConfigurable.\n", intIfNum);
      return L7_FALSE;
    }
  }

  *pCfg = &voipCfgData->voipIntfCfgData[index];

  return L7_TRUE;
}
/*********************************************************************
* @purpose  check if the interface is acquired by another component
*           and is therefore unavailable to VOIP
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
L7_BOOL voipMapIntfIsAcquired(L7_uint32 intIfNum)
{
  L7_BOOL result = L7_FALSE;

  /*COMPONENT_NONZEROMASK(voipIntfInfo[intIfNum].acquiredList, result);*/

  return result;

}
/*********************************************************************
* @purpose  Obtain a pointer to the first free interface config struct
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to voip interface config structure
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
L7_BOOL voipMapIntfConfigEntryGet(L7_uint32 intIfNum, voipIntfCfgData_t **pCfg)
{
  L7_uint32 i;
  nimConfigID_t configId;
  nimConfigID_t configIdNull;
  L7_RC_t rc;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  if (!(VOIP_IS_READY))
    return L7_FALSE;

  if ((rc = nimConfigIdGet(intIfNum, &configId)) == L7_SUCCESS)
  {
    for (i = 1; i < L7_VOIP_MAX_INTF; i++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&voipCfgData->voipIntfCfgData[i].configId, &configIdNull))
      {
        voipMapTbl[intIfNum] = i;
        *pCfg = &voipCfgData->voipIntfCfgData[i];
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
L7_RC_t voipIntfCreate(L7_uint32 intIfNum)
{
  nimConfigID_t      configId;
  voipIntfCfgData_t  *pCfg;
  L7_uint32          i;

  if (voipIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
    return L7_FAILURE;

  for (i = 1; i < L7_VOIP_MAX_INTF; i++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&voipCfgData->voipIntfCfgData[i].configId, &configId))
    {
      voipMapTbl[intIfNum] = i;
      break;
    }
  }

  pCfg = L7_NULL;

  /* If an interface configuration entry is not already assigned to the interface,
     assign one */
  if (voipMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    if(voipMapIntfConfigEntryGet(intIfNum, &pCfg) != L7_TRUE)
      return L7_FAILURE;

    /* Update the configuration structure with the config id */
    if (pCfg != L7_NULL)
    {
      voipBuildDefaultIntfConfigData(&configId, pCfg);
    }
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Applies interface config data
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
L7_RC_t voipApplyIntfConfigData(L7_uint32 intIfNum)
{
  voipIntfCfgData_t *pCfg;

  if (voipMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    /* Cannot enable voip when interface is acquired. Return without updating
     * hardware. The configuration will be completed when the interface is made
     * available again */
    if(voipMapIntfIsAcquired(intIfNum))
      return L7_SUCCESS;
    if(voipProfileIntfModeApply(intIfNum,pCfg->voipProfile,
       pCfg->voipBandwidth)!=L7_SUCCESS)
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
L7_RC_t voipIntfDetach(L7_uint32 intIfNum)
{
  voipIntfCfgData_t *pCfg;

  if (voipMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    /* when interface is acquired - Don't update hardware */
    if(voipMapIntfIsAcquired(intIfNum))
      return L7_SUCCESS;

    if (voipProfileIntfModeApply(intIfNum, FD_VOIP_INTF_PROFILE_MODE,
        FD_VOIP_INTF_PROFILE_BANDWIDTH) != L7_SUCCESS)
      return L7_FAILURE;
     
      
  }

  return L7_SUCCESS;
}
/*********************************************************************
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
L7_RC_t voipIntfDelete(L7_uint32 intIfNum)
{
  voipIntfCfgData_t *pCfg;

  if (voipMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    memset((void *)&pCfg->configId, 0, sizeof(nimConfigID_t));
    memset((void *)&voipMapTbl[intIfNum], 0, sizeof(L7_uint32));
    voipCfgData->cfgHdr.dataChanged = L7_TRUE;
  }
  
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Propogate Interface notifications to voip Task
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
L7_RC_t voipIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator)
{
  voipMgmtMsg_t msg;
  NIM_EVENT_COMPLETE_INFO_t  status;

  if (event != L7_CREATE &&
      event != L7_ATTACH &&
      event != L7_DETACH &&
      event != L7_DELETE &&
      event != L7_DOWN)
  {
    status.intIfNum     = intIfNum;
    status.component    = L7_FLEX_QOS_VOIP_COMPONENT_ID;
    status.event        = event;
    status.correlator   = correlator;
    status.response.reason = NIM_ERR_RC_UNUSED;
    status.response.rc  = L7_SUCCESS;
    nimEventStatusCallback(status);
    return L7_SUCCESS;
  }

  msg.msgId = voipMsgIntfChange;
  msg.u.voipIntfChangeParms.intIfNum = intIfNum;
  msg.u.voipIntfChangeParms.event = event;
  msg.u.voipIntfChangeParms.correlator = correlator;

  osapiMessageSend(voipQueue, &msg, VOIP_MSG_SIZE, L7_NO_WAIT, L7_MSG_PRIORITY_NORM);

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
L7_RC_t voipIntfChangeProcess(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator)
{
  L7_RC_t                    rc = L7_SUCCESS;
  NIM_EVENT_COMPLETE_INFO_t  status;
  
  status.intIfNum     = intIfNum;
  status.component    = L7_FLEX_QOS_VOIP_COMPONENT_ID;
  status.event        = event;
  status.correlator   = correlator;
  status.response.reason = NIM_ERR_RC_UNUSED;
  
  if (!(VOIP_IS_READY))	
  {
    LOG_MSG("Received an interface change callback while not ready to receive it");
    rc = L7_FAILURE;
        status.response.rc  = rc;
    nimEventStatusCallback(status);
    return rc;
  }
  if (voipIsValidIntf(intIfNum) != L7_TRUE)
  {
    rc = L7_SUCCESS;
    status.response.rc  = rc;
    nimEventStatusCallback(status);
    return rc;
  }
  if (intIfNum>L7_VOIP_MAX_INTF)
  {
      rc = L7_SUCCESS;
      status.response.rc = rc;
      nimEventStatusCallback(status);
      return rc;
  }

  switch (event)
  {
    case L7_CREATE:
      (void)osapiWriteLockTake(voipCfgRWLock, L7_WAIT_FOREVER);
      rc = voipIntfCreate(intIfNum);   
      (void)osapiWriteLockGive(voipCfgRWLock);
      break;

    case L7_ATTACH:
      rc = voipApplyIntfConfigData(intIfNum);
      break;

    case L7_DETACH:
      rc = voipIntfDetach(intIfNum);
      break;

    case L7_DELETE:
      (void)osapiWriteLockTake(voipCfgRWLock, L7_WAIT_FOREVER);
      rc = voipIntfDelete(intIfNum);
      (void)osapiWriteLockGive(voipCfgRWLock);    
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
* @purpose  Applies the voip Profile for an interface
*
* @param    L7_uint32  intIfNum  @b((input)) Internal interface number
* @param    L7_uint32  mode      @b((input)) voip Profile intf mode
* @param    L7_uint32  bandwidth @b((input)) bandwidth
*
* @returns  L7_SUCCESS, if interface mode was successfully applied
* @returns  L7_FAILURE, if interface number is invalid
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t voipProfileIntfModeApply(L7_uint32 intIfNum,L7_uint32 mode, L7_uint32 bandwidth)
{
   if ((mode == L7_ENABLE) || (mode == L7_DISABLE))
      return  dtlVoipProfileSet(intIfNum,mode,bandwidth);   
   else
      return L7_FAILURE;
   return L7_SUCCESS; 
}

