/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2009
*
**********************************************************************
*
* @filename llpf_util.c
*
* @purpose LLPF componenet utitlity fucntions
*
* @component Link Local Protocol Filtering
*
* @comments none
*
* @create 10/08/2009
*
* @author Vijayanand K(kvijayan)
* @end
*
**********************************************************************/
#include "llpf_util.h"
#include "dtlapi.h"
#include "llpf_cnfgr.h"

/*extern L7_uint32 *llpfMapTbl;*/
extern llpfCfgData_t *llpfCfgData;
extern osapiRWLock_t llpfCfgRWLock;
extern PORTEVENT_MASK_t llpfPortEventMask_g;

/*********************************************************************
*
* @purpose  To process the Callback for L7_CREATE
*
* @param    intfNum        @{{input}}The Current Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t llpfIntfCreate(L7_uint32 intIfNum)
{
  llpfBuildIntfDefaultConfigData(intIfNum);
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  To process the Callback for L7_ATTACH or
*                              NIM_INTERFACE_ACTIVATE_STARTUP
*
* @param    intfNum        @{{input}}The Current Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t llpfIntfApplyConfigData(L7_uint32 intIfNum)
{
  L7_uint32 protoIndex;
  L7_BOOL getMode;

  for(protoIndex = L7_LLPF_BLOCK_TYPE_ISDP; protoIndex <L7_LLPF_BLOCK_TYPE_LAST;protoIndex++)
  {
    if(llpfIntfFilterModeGet(intIfNum,protoIndex,&getMode) == L7_SUCCESS)
    {
      if( llpfIntfBlockModeApply(intIfNum,protoIndex,getMode) != L7_SUCCESS)
      {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

        L7_LOGF(L7_LOG_SEVERITY_INFO,L7_LLPF_COMPONENT_ID,
               "\r\nFailed to apply LLPF protocol with ID:%d on interface:%s\r\n",protoIndex, ifName);
      }
    }
  }
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  To process the Callback for L7_DETACH
*
* @param    intfNum        @{{input}}The Current Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t llpfIntfDetach(L7_uint32 intIfNum)
{
  L7_uint32 protoIndex;
  L7_RC_t rc = L7_SUCCESS;

  for(protoIndex= L7_LLPF_BLOCK_TYPE_ISDP; protoIndex <L7_LLPF_BLOCK_TYPE_LAST;protoIndex++)
  {
    if( llpfIntfBlockModeApply(intIfNum,protoIndex,L7_FALSE) != L7_SUCCESS)
    {
       L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
       nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

       L7_LOGF(L7_LOG_SEVERITY_INFO,L7_LLPF_COMPONENT_ID,
              "\r\nFailed to apply LLPF protocol with ID:%d on interface:%s\r\n",protoIndex, ifName);
    }
  }
  return rc;
}
/*********************************************************************
* @purpose  Note that configuration data has changed
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void llpfCfgDataChange(void)
{
    llpfCfgData->cfgHdr.dataChanged = L7_TRUE;
}

/*********************************************************************
* @purpose  Note that configuration data is now in sync with persistent
*           storage.
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void llpfCfgDataNotChanged(void)
{
    llpfCfgData->cfgHdr.dataChanged = L7_TRUE;
}

/*********************************************************************
* @purpose  Set LLPF Protocol Filter Blocking Mode
*           for a given interface in the config
*
* @param    intfNum        @{{input}}The Current Interface Number
* @param    protocolType   @{{input}}  Type of Protocol
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t llpfIntfFilterModeSet(L7_uint32 intIfNum,
                 L7_LLPF_BLOCK_TYPE_t protocolType)
{
  /* Check for appropriate protocol Type;Set the interface block mode bit */
  switch(protocolType)
  {
    case L7_LLPF_BLOCK_TYPE_ISDP:

         L7_INTF_SETMASKBIT(llpfCfgData->blockIsdp,intIfNum);
         break;

    case L7_LLPF_BLOCK_TYPE_VTP:

         L7_INTF_SETMASKBIT(llpfCfgData->blockVtp,intIfNum);
         break;

     case L7_LLPF_BLOCK_TYPE_DTP:

         L7_INTF_SETMASKBIT(llpfCfgData->blockDtp,intIfNum);
         break;

     case L7_LLPF_BLOCK_TYPE_UDLD:

         L7_INTF_SETMASKBIT(llpfCfgData->blockUdld,intIfNum);
         break;

     case L7_LLPF_BLOCK_TYPE_PAGP:

         L7_INTF_SETMASKBIT(llpfCfgData->blockPagp,intIfNum);
         break;

     case L7_LLPF_BLOCK_TYPE_SSTP:

         L7_INTF_SETMASKBIT(llpfCfgData->blockSstp,intIfNum);
         break;

     case L7_LLPF_BLOCK_TYPE_ALL:

         L7_INTF_SETMASKBIT(llpfCfgData->blockAll,intIfNum);
         break;

     default:

        return L7_FAILURE;
        break;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Clear LLPF Protocol Filter Blocking Mode
*           for a given interface in the config
*
* @param    intfNum        @{{input}}The Current Interface Number
* @param    protocolType   @{{input}}  Type of Protocol
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t llpfIntfFilterModeClear(L7_uint32 intIfNum,
                 L7_LLPF_BLOCK_TYPE_t protocolType)

{
  /* Check for appropriate protocol Type;Set the interface block mode bit */
  switch(protocolType)
  {
    case L7_LLPF_BLOCK_TYPE_ISDP:

         L7_INTF_CLRMASKBIT(llpfCfgData->blockIsdp,intIfNum);
         break;

    case L7_LLPF_BLOCK_TYPE_VTP:

         L7_INTF_CLRMASKBIT(llpfCfgData->blockVtp,intIfNum);
         break;

     case L7_LLPF_BLOCK_TYPE_DTP:

         L7_INTF_CLRMASKBIT(llpfCfgData->blockDtp,intIfNum);
         break;

     case L7_LLPF_BLOCK_TYPE_UDLD:

         L7_INTF_CLRMASKBIT(llpfCfgData->blockUdld,intIfNum);
         break;

     case L7_LLPF_BLOCK_TYPE_PAGP:

         L7_INTF_CLRMASKBIT(llpfCfgData->blockPagp,intIfNum);
         break;

     case L7_LLPF_BLOCK_TYPE_SSTP:

         L7_INTF_CLRMASKBIT(llpfCfgData->blockSstp,intIfNum);
         break;

     case L7_LLPF_BLOCK_TYPE_ALL:

         L7_INTF_CLRMASKBIT(llpfCfgData->blockAll,intIfNum);
         break;

     default:

        return L7_FAILURE;
        break;
  }
   return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get LLPF Protocol Filter Blocking Mode
*           for a given interface in the config
*
* @param    intfNum        @{{input}}The Current Interface Number
* @param    protocolType   @{{input}}  Type of Protocol
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t llpfIntfFilterModeGet(L7_uint32 intIfNum,
                 L7_LLPF_BLOCK_TYPE_t protocolType,L7_BOOL *mode)

{
  if(mode != L7_NULL)
  {
    switch(protocolType)
    {
      case L7_LLPF_BLOCK_TYPE_ISDP:

           if(L7_INTF_ISMASKBITSET(llpfCfgData->blockIsdp,intIfNum))
             *mode = L7_TRUE;
           else
             *mode = L7_FALSE;
           break;

      case L7_LLPF_BLOCK_TYPE_VTP:

           if(L7_INTF_ISMASKBITSET(llpfCfgData->blockVtp,intIfNum))
             *mode = L7_TRUE;
           else
             *mode = L7_FALSE;
            break;

       case L7_LLPF_BLOCK_TYPE_DTP:

           if(L7_INTF_ISMASKBITSET(llpfCfgData->blockDtp,intIfNum))
             *mode = L7_TRUE;
           else
             *mode = L7_FALSE;
            break;

       case L7_LLPF_BLOCK_TYPE_UDLD:

           if(L7_INTF_ISMASKBITSET(llpfCfgData->blockUdld,intIfNum))
             *mode = L7_TRUE;
           else
             *mode = L7_FALSE;
            break;

       case L7_LLPF_BLOCK_TYPE_PAGP:

           if(L7_INTF_ISMASKBITSET(llpfCfgData->blockPagp,intIfNum))
             *mode = L7_TRUE;
           else
             *mode = L7_FALSE;
            break;

       case L7_LLPF_BLOCK_TYPE_SSTP:

           if(L7_INTF_ISMASKBITSET(llpfCfgData->blockSstp,intIfNum))
             *mode = L7_TRUE;
           else
             *mode = L7_FALSE;
            break;

       case L7_LLPF_BLOCK_TYPE_ALL:

           if(L7_INTF_ISMASKBITSET(llpfCfgData->blockAll,intIfNum))
             *mode = L7_TRUE;
           else
             *mode = L7_FALSE;
            break;

       default:

          return L7_FAILURE;
          break;
    }
  }
  else
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Propogate Interface notifications to llpf
*
* @param    intIfNum   @{{input}} The Current Interface Number
* @param    event      @{{input}} Event type
* @param    correlator @{{input}} Correlator for event
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t llpfIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event,
NIM_CORRELATOR_t correlator)
{
  return  llpfIntfChangeProcess(intIfNum, event, correlator);
}

/*********************************************************************
* @purpose  Handle Interface notifications
*
* @param    intIfNum   @{{input}} Interface number
* @param    event      @{{input}} Event type
* @param    correlator @{{input}} Correlator for event
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t llpfIntfChangeProcess(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator)
{
  L7_RC_t                    rc = L7_SUCCESS;
  NIM_EVENT_COMPLETE_INFO_t  status;

  status.intIfNum     = intIfNum;
  status.component    = L7_LLPF_COMPONENT_ID;
  status.event        = event;
  status.correlator   = correlator;
  status.response.reason = NIM_ERR_RC_UNUSED;

  if (!(LLPF_IS_READY))
  {
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_LLPF_COMPONENT_ID,\
            "Received an interface change callback while not ready to receive it");
    rc = L7_FAILURE;
        status.response.rc  = rc;
    nimEventStatusCallback(status);
    return rc;
  }

  if (llpfIntfIsValid(intIfNum) != L7_TRUE)
  {
    rc = L7_SUCCESS;
    status.response.rc  = rc;
    nimEventStatusCallback(status);
    return rc;
  }
  if ( intIfNum > L7_LLPF_MAX_INTF)
  {
      rc = L7_SUCCESS;
      status.response.rc = rc;
      nimEventStatusCallback(status);
      return rc;
  }

  switch (event)
  {
    case L7_CREATE:
      (void)osapiWriteLockTake(llpfCfgRWLock, L7_WAIT_FOREVER);
      rc = llpfIntfCreate(intIfNum);
      (void)osapiWriteLockGive(llpfCfgRWLock);
      break;

    case L7_ATTACH:
      (void)osapiWriteLockTake(llpfCfgRWLock, L7_WAIT_FOREVER);
      rc = llpfIntfApplyConfigData(intIfNum);
      (void)osapiWriteLockGive(llpfCfgRWLock);
      break;

    case L7_DETACH:
      (void)osapiWriteLockTake(llpfCfgRWLock, L7_WAIT_FOREVER);
      rc = llpfIntfDetach(intIfNum);
      (void)osapiWriteLockGive(llpfCfgRWLock);
      break;

    case L7_DELETE:
       rc  = L7_SUCCESS; /* If its an event we dont care about, return SUCCESS */
     break;

    default:
      rc = L7_SUCCESS;
  }
  status.response.rc = rc;
  nimEventStatusCallback(status);
  return rc;
}

/*********************************************************************
* @purpose  Set LLPF Protocol Filter Blocking Mode.
*
* @param    intIfNum      @{{input}} Interface number
* @param    protocolType  @{{input}}  Type of Protocol
* @param    mode          @{{input}}  Enable/Disable LLPF
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t llpfIntfBlockModeApply(L7_uint32 intIfNum,
                 L7_LLPF_BLOCK_TYPE_t protocolType, L7_uint32 mode)
{
  return dtlLlpfIntfBlockConfig(intIfNum, protocolType, mode);
}
/*********************************************************************
* @purpose  Propogate Startup notifications to llpf
*
* @param    startupPhase  @{{input}}NIM phase of operation
*
* @notes
*
* @end
*********************************************************************/
void llpfStartupNotifyCallback(NIM_STARTUP_PHASE_t startupPhase)
{
  L7_RC_t rc;
  L7_uint32 intIfNum;

  rc = nimFirstValidIntfNumber(&intIfNum);

  while (rc == L7_SUCCESS)
  {
    if (llpfIntfIsValid(intIfNum))
    {
      switch (startupPhase)
      {
        case NIM_INTERFACE_CREATE_STARTUP:
          (void)osapiWriteLockTake(llpfCfgRWLock, L7_WAIT_FOREVER);
          llpfIntfCreate(intIfNum);
          (void)osapiWriteLockGive(llpfCfgRWLock);
          break;

        case NIM_INTERFACE_ACTIVATE_STARTUP:
          llpfIntfApplyConfigData(intIfNum);
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

      /* Register with NIM to receive port CREATE/DELETE events */
      PORTEVENT_SETMASKBIT(llpfPortEventMask_g, L7_CREATE);
      PORTEVENT_SETMASKBIT(llpfPortEventMask_g, L7_DELETE);
      nimRegisterIntfEvents(L7_LLPF_COMPONENT_ID, llpfPortEventMask_g);
      L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_LLPF_COMPONENT_ID,
              "LLPF Create Startup done");
      break;

    case NIM_INTERFACE_ACTIVATE_STARTUP:

      /* Add ATTACH/DETACH events to our NIM registration */
      PORTEVENT_SETMASKBIT(llpfPortEventMask_g, L7_ATTACH);
      PORTEVENT_SETMASKBIT(llpfPortEventMask_g, L7_DETACH);
      nimRegisterIntfEvents(L7_LLPF_COMPONENT_ID, llpfPortEventMask_g);
      break;

    default:
      break;
  }

  nimStartupEventDone(L7_LLPF_COMPONENT_ID);

  /* Inform cnfgr that LLPF has completed it HW updates */
  if (startupPhase == NIM_INTERFACE_ACTIVATE_STARTUP)
  {
    cnfgrApiComponentHwUpdateDone(L7_LLPF_COMPONENT_ID,
                                  L7_CNFGR_HW_APPLY_CONFIG);
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_LLPF_COMPONENT_ID,
            "LLPF Activate Startup done");
  }
}

