/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  mcast_util.c
*
* @purpose   MCAST Utility Functions
*
* @component MCAST Mapping Layer
*
* @comments  none
*
* @create    05/17/2002
*
* @author    gkiran.
*
* @end
*
**********************************************************************/

#include "l3_defaultconfig.h"
#include "dtl_l3_mcast_api.h"
#include "osapi_support.h"
#include "l7_mcast_api.h"
#include "mcast_util.h"
#include "mcast_debug.h"
#include "mfc_map.h"
#include "mcast_wrap.h"
#include "mcast_map.h"


static L7_RC_t mcastMapAdminModeEventChangeNotify(L7_uint32 event,
                                                  void *pData);
/* static void mcastMapAdminscopeBoundaryGlobalModeApply(L7_uint32 mode);  */

/**************************************************************
* @purpose   Enables multicast forwarding to dtl and MFC
*
* @param     void
* 
* @returns   L7_SUCCESS 
*
* @comments  None
*
* @end
**************************************************************/
L7_RC_t mcastMapMcastAdminModeEnable(L7_uchar8 family)
{
  L7_uint32 intIfNum,i;
  if (mcastIpMapRtrAdminModeGet(family) == L7_DISABLE)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES, 
                    "ip routing is disabled.\n");   
    return L7_SUCCESS;
  }
  if (mcastMapMcastIsOperational(family)== L7_TRUE)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES, 
                    "Already multicast forwarding is enabled.\n");   
    return L7_SUCCESS;
  }
  if (mfcEnable(family) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES, 
                    "mfcEnable failed.\n");   
    return L7_FAILURE;
  }
  if ((mcastGblVariables_g.mcastMapInfo.mcastV4Initialized != L7_TRUE) &&
      (mcastGblVariables_g.mcastMapInfo.mcastV6Initialized != L7_TRUE))
  {
        /* set the unknown packets send to cpu bit in NPU */
    dtlRouterMulticastForwardModeSet(L7_ENABLE, L7_AF_INET);
  }

  /*
   NOTE: Set mcastV6Initialized/mcastV4Initialized after making DTL Call
  */
  if (family == L7_AF_INET)
  {
    mcastGblVariables_g.mcastMapInfo.mcastV4Initialized = L7_TRUE;
    /* enable router interfaces which are configured for routing */
    for (i = 1; i <= L7_RTR_MAX_RTR_INTERFACES; i++)
    {
      /* NOTE:  This call fails if router intf not created yet */
      if (mcastIpMapRtrIntfToIntIfNum(L7_AF_INET, 
                                      i, &intIfNum) == L7_SUCCESS)
      {
        mcastMapIntfConfigApply(intIfNum, L7_ENABLE);
      }
    }
  }
  else if (family == L7_AF_INET6)
  {
    mcastGblVariables_g.mcastMapInfo.mcastV6Initialized = L7_TRUE;
  }
  return L7_SUCCESS;  
}

/**************************************************************
* @purpose   Disables multicast forwarding to dtl and MFC
*
* @param     void
*
* @returns   L7_SUCCESS 
* @returns   L7_FAILURE 
*
* @comments  None
*
* @end
**************************************************************/
L7_RC_t mcastMapMcastAdminModeDisable(L7_uchar8 family)
{
  L7_uint32 i,intIfNum;

  if (mcastMapMcastIsOperational(family) != L7_TRUE)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES, 
                    "Already multicast forwarding is disabled.\n");   
    return L7_SUCCESS;
  }

  if (mfcDisable(family)!= L7_SUCCESS)
  {
     MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
         "\nMCAST_MAP: mfcDisable failed\n");
  }
  if (family == L7_AF_INET)
  {
    for (i = 1; i <= L7_RTR_MAX_RTR_INTERFACES; i++)
    {
      /* NOTE:  This call fails if router intf not created yet */
      if (mcastIpMapRtrIntfToIntIfNum(L7_AF_INET, 
                                      i, &intIfNum) == L7_SUCCESS)
      {
        mcastMapIntfConfigApply(intIfNum, L7_DISABLE);
      }
    }
    mcastGblVariables_g.mcastMapInfo.mcastV4Initialized = L7_FALSE;
  }
  else if (family == L7_AF_INET6)
  {
    mcastGblVariables_g.mcastMapInfo.mcastV6Initialized = L7_FALSE;
  }

  /*
   NOTE: Set mcastV6Initialized/mcastV4Initialized before making DTL Call
  */
  if ((mcastGblVariables_g.mcastMapInfo.mcastV4Initialized != L7_TRUE) &&
      (mcastGblVariables_g.mcastMapInfo.mcastV6Initialized != L7_TRUE))
  {
    /* reset the unknown packets send to cpu bit in NPU */
    dtlRouterMulticastForwardModeSet(L7_DISABLE, L7_AF_INET);
  }
  return L7_SUCCESS;  
}

/**************************************************************
* @purpose   Enables multicast forwarding to dtl and MFC
*
* @param     void
* 
* @returns   L7_SUCCESS 
*
* @comments  None
*
* @end
**************************************************************/
L7_RC_t mcastMapMcastAdminModeEnableAndNotify(void)
{

  if (mcastMapMcastAdminModeEnable(L7_AF_INET) != L7_SUCCESS)
  {
     MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
         "\nMCAST_MAP-v4: mcastMapMcastAdminModeEnable() failed\n");
     return L7_FAILURE;
  }
  if (mcastMapMcastAdminModeEnable(L7_AF_INET6) != L7_SUCCESS)
  {
     MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
         "\nMCAST_MAP-v6: mcastMapMcastAdminModeEnable() failed\n");
     return L7_FAILURE;
  }
  /* Do Notify here */
  mcastMapAdminModeEventChangeNotify(MCAST_RTR_ADMIN_MODE_ENABLED,
                                     L7_NULLPTR);
  return L7_SUCCESS;
}

/**************************************************************
* @purpose   Disables multicast forwarding to dtl and MFC
*
* @param     void
*
* @returns   L7_SUCCESS 
* @returns   L7_FAILURE 
*
* @comments  None
*
* @end
**************************************************************/
L7_RC_t mcastMapMcastAdminModeDisableAndNotify(void)
{
  if (mcastMapMcastAdminModeDisable(L7_AF_INET) != L7_SUCCESS)
  {
     MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
         "\nMCAST_MAP-v4: mcastMapMcastAdminModeDisable() failed\n");
     return L7_FAILURE;
  }
  if (mcastMapMcastAdminModeDisable(L7_AF_INET6) != L7_SUCCESS)
  {
     MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
         "\nMCAST_MAP-v6: mcastMapMcastAdminModeDisable() failed\n");
     return L7_FAILURE;
  }
  /* Do Notify here */
  mcastMapAdminModeEventChangeNotify(MCAST_RTR_ADMIN_MODE_DISABLE_PENDING,
                                     L7_NULLPTR);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Configures given interface with corresponding ttl 
*           threshold to lower layers
*
* @param    intIfNum      @b{(input)}   Internal Interface Number
* @param    ttl           @b{(input)}   TTL threshold to be applied
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments None
*
* @end
*
*********************************************************************/
L7_RC_t mcastMapIntfTtlThreshApply(L7_uint32 intIfNum, L7_uint32 ttl)
{

  if (mcastMapMcastIsOperational(L7_AF_INET)== L7_TRUE)
  {
    dtlMulticastTTLScopingSet(intIfNum, ttl);
    return L7_SUCCESS;
  }
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: interface are not enabled."
          "Not able to apply ttl threshold to interface = %d\n", intIfNum);
  return L7_FAILURE;
}


/**********************************************************************
* @purpose    Notify Admin Scope change ro registered MRPS
*
* @param      event_type      @b{(input)}     Event Type
* @param      msg_len         @b{(input)}     Messagel length
* @param      adminInfo       @b{(input)}     Admin Scope Info
*
* @returns    none
*
* @comments   none
*
* @end
**********************************************************************/
static void mcastMapAdminScopeEventChangeNotify(L7_uint32 event_type, 
                                                L7_uint32 msg_len, mcastAdminMsgInfo_t *adminInfo)
{
  L7_uint32 mrpIndex;
  for ( mrpIndex = 0; mrpIndex < L7_MRP_MAXIMUM; mrpIndex++)
  {
    if (mcastGblVariables_g.notifyAdminScopeChange[mrpIndex] != L7_NULLPTR)
    {
      (mcastGblVariables_g.notifyAdminScopeChange[mrpIndex]) 
      (event_type, msg_len, (void *)adminInfo);
    }
  }
  return;
}

/**********************************************************************
* @purpose    Adds an entry to the Admin Scope Boundary List for the
*             given interface
*
* @param      intIfNum        @b{(input)}     interface Number
* @param      ipGroup         @b{(input)}     Group IP Address
* @param      ipMask          @b{(input)}     Group IP Address Mask
*
* @returns    L7_SUCCESS
* @returns    L7_FAILURE
*
* @comments   none
*
* @end
**********************************************************************/
L7_RC_t mcastMapAdminScopeBoundaryAddApply(L7_uint32 intIfNum,
                                           L7_inet_addr_t *ipGroup,
                                           L7_inet_addr_t *ipMask)
{
  mcastAdminMsgInfo_t adminInfo;

  if (inetCopy(&(adminInfo.groupAddress), ipGroup) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG_ADDR(MCAST_MAP_DEBUG_FAILURES,"inetAddress Copy Failed for ",
                         ipGroup);
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: inetAddress Copy Failed.\n");
    return L7_FAILURE;
  }
  if (inetCopy(&(adminInfo.groupMask), ipMask) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG_ADDR(MCAST_MAP_DEBUG_FAILURES,"inetAddress Copy Failed for ",
                         ipMask);
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: inetAddress Copy Failed.\n");    
    return L7_FAILURE;
  }
  adminInfo.intIfNum     = intIfNum;
  adminInfo.mode         = L7_ENABLE;
  mcastMapAdminScopeEventChangeNotify(MCAST_EVENT_ADMINSCOPE_BOUNDARY, 
                                      sizeof(mcastAdminMsgInfo_t), &adminInfo);
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Deletes an entry from the Admin Scope Boundary List for
*           the given interface
*
* @param   intIfNum        @b{(input)}     interface Number
* @param   ipGroup         @b{(input)}     Group IP Address
* @param   ipMask          @b{(input)}     Group IP Address Mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
**********************************************************************/
L7_RC_t mcastMapAdminScopeBoundaryDeleteApply(L7_uint32 intIfNum,
                                              L7_inet_addr_t *ipGroup,
                                              L7_inet_addr_t *ipMask)
{
  mcastAdminMsgInfo_t adminInfo;

  if (inetCopy(&(adminInfo.groupAddress), ipGroup) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG_ADDR(MCAST_MAP_DEBUG_FAILURES,"inetAddress Copy Failed for ",
                         ipGroup);
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: inetAddress Copy Failed.\n");
    return L7_FAILURE;
  }
  if (inetCopy(&(adminInfo.groupMask), ipMask) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG_ADDR(MCAST_MAP_DEBUG_FAILURES,"inetAddress Copy Failed for ",
                         ipMask);  
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: inetAddress Copy Failed.\n");
    return L7_FAILURE;
  }
  adminInfo.intIfNum     = intIfNum;
  adminInfo.mode         = L7_DISABLE;
  mcastMapAdminScopeEventChangeNotify(MCAST_EVENT_ADMINSCOPE_BOUNDARY, 
                                      sizeof(mcastAdminMsgInfo_t), &adminInfo);
  return L7_SUCCESS;
}

/********************************************************************
* purpose   De-initializes the lower level admin scope data structures
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Memory cleanup done here
*
* @end
*
********************************************************************/
L7_RC_t mcastMapAdminScopeBoundaryFlush(void)
{
  L7_uint32 itr, tmpIfNum, entriesChecked = 0;

  for (itr = 0; 
      (entriesChecked < mcastGblVariables_g.mcastMapCfgData.rtr.
       numAdminScopeEntries) &&
      (itr < L7_MCAST_MAX_ADMINSCOPE_ENTRIES); itr++)
  {

    entriesChecked++;
    if (nimIntIfFromConfigIDGet(&mcastGblVariables_g.mcastMapCfgData.rtr.
                                mcastAdminScopeCfgData[itr].ifConfigId, &tmpIfNum) == L7_SUCCESS)
    {
      mcastMapAdminScopeBoundaryDeleteApply(tmpIfNum,
                                            &mcastGblVariables_g.mcastMapCfgData.rtr.
                                            mcastAdminScopeCfgData[itr].groupIpAddr,
                                            &mcastGblVariables_g.mcastMapCfgData.rtr.
                                            mcastAdminScopeCfgData[itr].groupIpMask);

    }

  }
  return L7_SUCCESS;
}

#if 0
/**************************************************************************
* @purpose  Enables/Disables all AdminScope Boundaries on all interfaces, 
*           called when global routing or multicast mode changes.
*
* @param    mode       @b{(input)}   L7_ENABLE or L7_DISABLE
*
* @returns  void
*
* @comments none
*
* @end
*
***************************************************************/
void mcastMapAdminscopeBoundaryGlobalModeApply(L7_uint32 mode)
{
  L7_uint32 intIfNum,itr;
  L7_uint32 entriesChecked =0;
  L7_mcastMapAdminScopeCfgData_t *scope;

  for (itr = 0; (itr < L7_MCAST_MAX_ADMINSCOPE_ENTRIES) &&
      (entriesChecked < mcastGblVariables_g.mcastMapCfgData.rtr.
       numAdminScopeEntries); itr++)
  {
    if (mcastGblVariables_g.mcastMapCfgData.rtr.
        mcastAdminScopeCfgData[itr].inUse == L7_TRUE)
    {
      entriesChecked++;
      scope = &mcastGblVariables_g.mcastMapCfgData.rtr.
              mcastAdminScopeCfgData[itr];
      if (nimIntIfFromConfigIDGet(&scope->ifConfigId, 
                                  &intIfNum)== L7_SUCCESS)
      {
        if (mode == L7_ENABLE)
        {
          mcastMapAdminScopeBoundaryAddApply(intIfNum,
                                             &scope->groupIpAddr,
                                             &scope->groupIpMask);
        }
        else if (mode == L7_DISABLE)
        {
          mcastMapAdminScopeBoundaryDeleteApply(intIfNum,
                                                &scope->groupIpAddr,
                                                &scope->groupIpMask);
        }
        else
        {
          MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\n Invalid admin-scope mode \n ");
          MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
                          "Invalid admin scope mode");
          return;
        }
      }
    }
  }   
}
#endif

/***************************************************************
*
* @purpose   Enables/Disables all AdminScope Boundaries on specified
*            interface, called when interface mode changes.
*
* @param    intIfNum    @b{(input)}   Internal Interface Number
* @param    mode        @b{(input)}   L7_ENABLE or L7_DISABLE
*
* @returns  void
*
* @comments none
*
* @end
*
***************************************************************/
void mcastMapAdminscopeBoundaryInterfaceModeApply(L7_uint32 intIfNum,
                                                  L7_uint32 mode)
{
  L7_uint32 itr, tmpIfNum, entriesChecked = 0;
  L7_mcastMapAdminScopeCfgData_t *scope;

  /* Apply configured admin scope data for interface. 
   * Check on number of entries checked 
   * to reduce no. of iterations 
   */
  for (itr = 0; 
      (entriesChecked < mcastGblVariables_g.mcastMapCfgData.rtr.numAdminScopeEntries) 
      && (itr < L7_MCAST_MAX_ADMINSCOPE_ENTRIES); itr++)
  {
    if (mcastGblVariables_g.mcastMapCfgData.rtr.
        mcastAdminScopeCfgData[itr].inUse == L7_TRUE)
    {
      entriesChecked++;
      scope = &mcastGblVariables_g.mcastMapCfgData.rtr.
              mcastAdminScopeCfgData[itr];
      if ((nimIntIfFromConfigIDGet(&scope->ifConfigId, 
                                   &tmpIfNum)== L7_SUCCESS) && 
          (tmpIfNum == intIfNum))
      {
        if (mode == L7_ENABLE)
        {
          mcastMapAdminScopeBoundaryAddApply(intIfNum,&scope->groupIpAddr,
                                             &scope->groupIpMask);
        }
        else if (mode == L7_DISABLE)
        {
          mcastMapAdminScopeBoundaryDeleteApply(intIfNum,&scope->groupIpAddr,
                                                &scope->groupIpMask);
        }
        else
        {
          MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"Invalid admin scope mode");
          return;
        }
      }
    }
  }
}

/************************************************************************
* @purpose  Multicast events call back.
*
* @param    intIfNum   @b{(input)} Internal Interface Number
* @param    event      @b{(input)} Multicast Event
* @param    pData      @b{(output)} Data
*
* @returns  L7_SUCCESS
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mcastMapAdminModeEventChangeNotify(L7_uint32 event,
                                           void *pData)
{
  L7_uint32                 index;
  L7_RC_t                   rc = L7_FAILURE;
  /* intentionally not initializing since this type is opaque to this app, 
   * and app really has no idea how to initialize it. Initialization really
   * done through asyncEventCorrelatorCreate() below. */
  ASYNC_CORRELATOR_t        mcastAsyncEventCorrelator;   
  ASYNC_EVENT_NOTIFY_INFO_t mcastAsyncEventInfo;
  static L7_uint32          mcastEventData;

  mcastEventData = event;

  /* Create an async event correlator  */
  if (asyncEventCorrelatorCreate(mcastGblVariables_g.mcastMapInfo.eventHandlerId, 
                                 &mcastAsyncEventCorrelator) != L7_SUCCESS)
  {
    L7_LOG(L7_LOG_SEVERITY_ERROR, L7_FLEX_MCAST_MAP_COMPONENT_ID,
           "Failed to create async event correlator.");
    return L7_FAILURE;
  }

  /* start async event handler for event */
  if (asyncEventSyncStart(mcastGblVariables_g.mcastMapInfo.eventHandlerId,
                          mcastAsyncEventCorrelator, &mcastEventData) == L7_SUCCESS)
  {
    memset(&mcastAsyncEventInfo, 0, sizeof(ASYNC_EVENT_NOTIFY_INFO_t));
    mcastAsyncEventInfo.correlator = mcastAsyncEventCorrelator;
    mcastAsyncEventInfo.handlerId  = mcastGblVariables_g.
                                     mcastMapInfo.eventHandlerId;

    for (index = 0; index < L7_MRP_MAXIMUM; index++)
    {
      if (mcastGblVariables_g.notifyMcastModeChange[index] != L7_NULLPTR)
      {
        rc = (mcastGblVariables_g.notifyMcastModeChange[index])
             (event, pData, &mcastAsyncEventInfo);
        if (rc == L7_FAILURE)
        {
          MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Failed to notify Mcast admin mode change for index = %d \n", index);
        }
      }
    }
    /* wait until all notified components have completed processing the event */
    osapiSemaTake(mcastGblVariables_g.mcastMapInfo.eventCompleteSignal, 
                  L7_WAIT_FOREVER);
  }
  else
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "\nMCAST_MAP: Error returned from asyncEventSyncStart().\n");
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Status callback from components for EVENT Notifications
*
* @param    pComplete         @b{(input)}  complete information.
* @param    pRemainingMask    @b{(input)}  Remaining Mask.
* @param    pFailingMask      @b{(input)}  Failing Mask.
*
* @returns  void     
*
* @comments Called by the async event utility at the conclusion of 
*           processing an MCAST event.
*          
* @end
*********************************************************************/
void mcastMapAsyncEventCompleteNotify(ASYNC_EVENT_COMPLETE_INFO_t *pComplete, 
                                      COMPONENT_MASK_t *pRemainingMask,
                                      COMPONENT_MASK_t *pFailingMask)
{
  L7_uint32    *pMcastEventData;

  /*----------------------------------------------------*/
  /* Validity check the parameters                      */
  /*----------------------------------------------------*/

  if ((pComplete == L7_NULLPTR) || (pRemainingMask == L7_NULLPTR) || 
      (pFailingMask == L7_NULLPTR))
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "\nMCAST_MAP: NULL Async Event info for mcast mode change\n");
    return;
  }

  if ((pComplete->handlerId == L7_NULL) || (pComplete->correlator == L7_NULL))
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES, "\nERROR:"
                    "Null handler  %d or correlator %d.\n", 
                    pComplete->handlerId, pComplete->correlator);
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: NULL handler  %d or correlator %d.\n", 
                    pComplete->handlerId, pComplete->correlator);
    return;

  }

  if ((asyncEventCorrelatorDataGet(pComplete->handlerId, 
                                   pComplete->correlator, (void *)&pMcastEventData) != L7_SUCCESS) || 
      (pMcastEventData == L7_NULLPTR) )
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "\nMCAST_MAP: Failed to get Asyn event correlator for handlerId  = %d\n",
             pComplete->handlerId );
    return;
  }

  /*----------------------------------------------------*/
  /* Complete the asynchronous response                 */
  /*----------------------------------------------------*/

  if (pComplete->async_rc.rc != L7_SUCCESS)
  {
    L7_int32 i;
    L7_uchar8 remainingMaskString[MCAST_MASK_STRING_LENGTH],
    failingMaskString[MCAST_MASK_STRING_LENGTH];

    if (pComplete->async_rc.reason == ASYNC_EVENT_REASON_RC_TIMEOUT)
    {
      memset(remainingMaskString, 0, sizeof(remainingMaskString));
      for (i = COMPONENT_INDICES; i >= 0; i--)
      {
        if ((MCAST_MASK_STRING_LENGTH - strlen(remainingMaskString)) >= 3)
        {
          osapiSnprintf(remainingMaskString, MCAST_MASK_STRING_LENGTH, 
                     "%s%.2x", remainingMaskString,
                  pRemainingMask->value[i]);
        }
      }
      MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCASTMAP :Timeout event(%d), remainingMask=%s\n",  
                      pMcastEventData,  remainingMaskString);
      MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES, "\nMCAST:Timeout"
                      "event(%d), remainingMask=%s\n",  
                      pMcastEventData, 
                      remainingMaskString);
    }
    else
    {
      memset(failingMaskString, 0, sizeof(failingMaskString));
      for (i = COMPONENT_INDICES; i >= 0; i--)
      {
        if ((MCAST_MASK_STRING_LENGTH - strlen(failingMaskString)) >= 3)
        {
          osapiSnprintf(failingMaskString, MCAST_MASK_STRING_LENGTH, 
                  "%s%.2x", failingMaskString, 
                  pFailingMask->value[i]);
        }
      }
      MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCASTMAP : Failed event(%d), reason(%d),failingMask=%s\n", 
                 pMcastEventData, pComplete->async_rc.reason,failingMaskString);
       
      MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES, "\nMCAST:"
                      "Failed event(%d), reason(%d),"
                      "failingMask=%s\n", 
                      pMcastEventData, 
                      pComplete->async_rc.reason, 
                      failingMaskString);
    }
  }

  /* Delete the event only after the response has been completely gotten */
  asyncEventSyncFinish(pComplete->handlerId, pComplete->correlator);
  osapiSemaGive(mcastGblVariables_g.mcastMapInfo.eventCompleteSignal);
}

/*********************************************************************
* @purpose  Determine if the interface is valid for MCAST configuration
*           and optionally output a pointer to the configuration structure
*           entry for this interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    pCfg        @b{(inout)} Pointer to MCAST interface config structure,
*                                   or L7_NULL if not present
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the MCAST component has been
*           started (regardless of whether the MCAST protocol is enabled
*           or not).
*
* @comments Does not check for a valid router interface, since that may
*           not get configured until later.  All that is required here
*           is a valid slot.port number to use when referencing the
*           interface config data structure.
*
* @comments The caller can set the pCfg parm to L7_NULL if it does not
*           want the value output from this function.
*
* @end
*********************************************************************/
L7_BOOL mcastMapIntfIsConfigurable(L7_uint32 intIfNum,
                                   L7_mcastMapIfCfgData_t **pCfg)
{
  L7_uint32 index;
  nimConfigID_t configId;

  if (!(MCAST_IS_READY))
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: MCAST is not ready.\n");
    return L7_FALSE;
  }
  if (mcastIntfIsValid(intIfNum) != L7_TRUE)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Interface %d is not valid.\n", intIfNum);
    return L7_FALSE;
  }

  index = mcastGblVariables_g.pMcastMapCfgMapTbl[intIfNum];

  if (index == 0)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Map Table is not initialized"
            " for the interface = %d.\n", intIfNum);
    return L7_FALSE;
  }

  /* verify that the configId in the config data table entry matches the
   * configId that NIM maps to the intIfNum we are considering.
   */
  if (nimConfigIdGet(intIfNum, &configId) == L7_SUCCESS)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&configId, &(mcastGblVariables_g.
                                            mcastMapCfgData.intf[index].configId)) == L7_FALSE)
    {
      /* if we get here, either we have a table management error between
       * mcastMapCfgData and pMcastMapCfgMapTbl or there is
       * synchronization issue between NIM and components w.r.t. interface
       *  creation/deletion.
       */
      MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES, "\nError"
                      "accessing MCAST config data for interface %d in"
                      "mcastMapIntfIsConfigurable.\n", 
                      intIfNum);
      MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Failed to access MCAST"
              "config data for interface = %d \n", intIfNum);
      return L7_FALSE;
    }
  }

  if (pCfg != L7_NULLPTR)
  {
    *pCfg = &(mcastGblVariables_g.mcastMapCfgData.intf[index]);
  }

  return L7_TRUE;
}

/*********************************************************************
* @purpose  Retrieve a pointer to an empty configuration data for a given
*           interface if found.
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    pCfg        @b{(input)} Output pointer location
*                       @b{(output)} Pointer to MCAST interface config structure
*
* @returns  L7_TRUE     an empty interface config structure was found
* @returns  L7_FALSE    no empty interface config structure entry is available
*
* @comments Does not check for a valid router interface, since that may
*           not get configured until later.  All that is required here
*           is a valid slot.port number to use when referencing the  
*           interface config data structure.
*
* @end
*********************************************************************/
L7_BOOL mcastMapIntfConfigEntryGet(L7_uint32 intIfNum, L7_mcastMapIfCfgData_t **pCfg)
{
  L7_uint32 i;
  nimConfigID_t configIdNull;

  if (!(MCAST_IS_READY))
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: MCAST is not ready.\n");
    *pCfg = L7_NULLPTR;
    return L7_FALSE;
  }

  memset((void *)&configIdNull, 0, sizeof(nimConfigID_t));

  for (i = 1; i < L7_IPMAP_INTF_MAX_COUNT; i++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&(mcastGblVariables_g.
                                 mcastMapCfgData.intf[i].configId), &configIdNull))
    {
      mcastGblVariables_g.pMcastMapCfgMapTbl[intIfNum] = i;
      *pCfg = &mcastGblVariables_g.mcastMapCfgData.intf[i];
      return L7_TRUE;
    }
  }
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Failed to access MCAST"
          "config data for interface = %d \n", intIfNum);
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Retrieve a pointer to the CFG Structure if found.
*
* @param    addrFamily  @b{(input)}  Address Family Identifier
* @param    cfgData     @b{(output)} Pointer to MCAST CFG Structure
*
* @returns  L7_SUCCESS  if the config struct entry is available
* @returns  L7_FAILURE  if the input validations fail.
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t
mcastMapCfgDataGet (L7_uchar8 addrFamily,
                    L7_mcastMapCfg_t **cfgData)
{
  L7_RC_t rc = L7_SUCCESS;

  if (cfgData == L7_NULLPTR)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "pCfg is NULL");
    return L7_FAILURE;
  }

  switch (addrFamily)
  {
    case L7_AF_INET:
      *cfgData = &mcastGblVariables_g.mcastMapCfgData;
      break;

    case L7_AF_INET6:
      if (mcastGblVariables_g.mcastMapV6CfgData == L7_NULLPTR)
      {
        MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "mcastMapV6CfgData is NULL");
        return L7_FAILURE;
      }
      *cfgData = mcastGblVariables_g.mcastMapV6CfgData;
      break;

    default:
      *cfgData = L7_NULLPTR;
      MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Invalid Address Family - %d",
                       addrFamily);
      rc = L7_FAILURE;
  }

  return rc;
}

/***************************************************************
*
* @purpose  Notify the registered MRPs of this Static MRoute
*           Status
*
* @param    addrFamily         @b{(input)}  Address Family Identifier
* @param    staticMRouteEntry  @b{(input)}  Pointer to the Static
*                                           MRoute Entry
* @param    mRouteStatus       @b{(input)}  Route Change Type
*
* @returns  L7_SUCCESS  Notification success
* @returns  L7_FAILURE  Notification failed
*
* @comments Only PIMSM and PIMDM need to be registered for these
*           updates.  IGMP-Proxy and DVMRP should not register nor
*           depend on this data.
*
* @end
*
***************************************************************/
L7_RC_t
mcastMapStaticMRouteRegisteredUsersNotify (L7_uchar8 addrFamily,
                                           L7_mcastMapStaticRtsCfgData_t *staticMRouteEntry,
                                           L7_uint32 mRouteStatus)
{
  L7_inet_addr_t *srcAddr = L7_NULLPTR;
  L7_inet_addr_t *srcMask = L7_NULLPTR;
  L7_inet_addr_t *rpfAddr = L7_NULLPTR;
  L7_uint32 intIfNum = 0;
  L7_uint32 rtrIfNum = 0;
  mcastRPFInfo_t rpfInfo;
  L7_MRP_TYPE_t mrpId = 0;
  L7_uchar8 src[MCAST_MAP_MAX_DBG_ADDR_SIZE];
  L7_uchar8 msk[MCAST_MAP_MAX_DBG_ADDR_SIZE];
  L7_uchar8 rpf[MCAST_MAP_MAX_DBG_ADDR_SIZE];

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_APIS, "Entry");

  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Invalid Address Family Identifier"
                     " -%d", addrFamily);
    return L7_FAILURE;
  }
  if (staticMRouteEntry == L7_NULLPTR)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES,"staticMRouteEntry-%p is NULL",
                     staticMRouteEntry);
    return L7_FAILURE;
  }

  srcAddr = &staticMRouteEntry->source;
  srcMask = &staticMRouteEntry->mask;
  rpfAddr = &staticMRouteEntry->rpfAddr;

  memset (&rpfInfo, 0, sizeof(mcastRPFInfo_t));

  /* rpfNextHop */
  inetAddressZeroSet (addrFamily, &rpfInfo.rpfNextHop);
  inetCopy (&rpfInfo.rpfNextHop, rpfAddr);

  /* routeProtocol */
  rpfInfo.rpfRouteProtocol = RTO_STATIC; /* TODO */

  /* rpfRouteAddress */
  inetAddressZeroSet (addrFamily, &rpfInfo.rpfRouteAddress);
  inetCopy (&rpfInfo.rpfRouteAddress, srcAddr);

  /* rpfRouteMetricPref */
  rpfInfo.rpfRouteMetricPref = staticMRouteEntry->preference;

  /* rpfRouteMetric */
  rpfInfo.rpfRouteMetric = 0;

  /* rpfRouteStatus */
  rpfInfo.status = mRouteStatus;

  /* prefixLength */
  if (inetMaskToMaskLen (srcMask, (L7_uchar8*) &rpfInfo.prefixLength)
                      != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Mask to MaskLen Conversion "
                     "Failed for srcMask - %s", inetAddrPrint(srcMask,msk));
    return L7_FAILURE;
  }

  /* rpfIfIndex */
  if (addrFamily == L7_AF_INET6)
  {
    if (L7_IP6_IS_ADDR_LINK_LOCAL (&(rpfAddr->addr.ipv6)) != 0)
    {
      if (nimIntIfFromConfigIDGet (&staticMRouteEntry->ifConfigId, &intIfNum)
                                != L7_SUCCESS)
      {
        MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "intIfNum Get Failed");
        return L7_FAILURE;
      }
    }
  }
  if (intIfNum != 0)
  {
    if (mcastIpMapIntIfNumToRtrIntf (addrFamily, intIfNum, &rtrIfNum) != L7_SUCCESS)
    {
      MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "intIfNum - %d to rtrIfNum -%d "
                       "Conversion Failed", intIfNum, rtrIfNum);
      return L7_FAILURE;
    }
  }
  else
  {
    if (mcastIpAddrRtrIntfResolve (&rpfInfo.rpfNextHop, &rtrIfNum) != L7_SUCCESS)
    {
      MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "rpfNextHop to rtrIfNum Conversion "
                       "Failed for rpfAddr - %s", inetAddrPrint(&rpfInfo.rpfNextHop,rpf));
      return L7_FAILURE;
    }
  }
  rpfInfo.rpfIfIndex = rtrIfNum;

  /* Notify the Registered protocols */
  for (mrpId = 0; mrpId < L7_MRP_MAXIMUM; mrpId++)
  {
    if (addrFamily == L7_AF_INET)
    {
      if (mcastGblVariables_g.mcastV4StaticMRouteNotifyFn[mrpId] != L7_NULLPTR)
      {
        if (((mcastGblVariables_g.mcastV4StaticMRouteNotifyFn[mrpId])(addrFamily,
                                  MCAST_EVENT_STATIC_MROUTE_CHANGE,
                                  sizeof(mcastRPFInfo_t), &rpfInfo))
                               != L7_SUCCESS)
        {
          MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Static V4 MRoute Notification to "
                           "Protocol ID - %d Failed for srcAddr - %s",
                           mrpId, inetAddrPrint(srcAddr,src));
        }
      }
    }
    else
    {
      if (mcastGblVariables_g.mcastV6StaticMRouteNotifyFn[mrpId] != L7_NULLPTR)
      {
        if (((mcastGblVariables_g.mcastV6StaticMRouteNotifyFn[mrpId])(addrFamily,
                                  MCAST_EVENT_STATIC_MROUTE_CHANGE,
                                  sizeof(mcastRPFInfo_t), &rpfInfo))
                               != L7_SUCCESS)
        {
          MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Static V6 MRoute Notification to "
                           "Protocol ID - %d Failed for srcAddr - %s",
                           mrpId, inetAddrPrint(srcAddr,src));
        }
      }
    }
  }

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Register a routine to be called when changes occur within
*           the Multicast Static MRoute module that necessitates
*           communication with other modules for the given protocol.
*
* @param    addrFamily    @b{(input)} Address Family Identifier
* @param    protocolId    @b{(input)} Protocol ID (A identified constant in L7_MRP_TYPE_t )
* @param    *notifyFn     @b{(input)} pointer to a routine to be invoked
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  The registration from MRPs is reentrant, yet the notify list is not 
*            protected as function pointer location is fixed for each 
*
* @end
*
*********************************************************************/
L7_RC_t
mcastMapStaticMRouteEventRegister (L7_uchar8 addrFamily,
                                   L7_MRP_TYPE_t protocolId,
                                   mcastMapStaticMRouteNotifyFn_t notifyFn)
{
  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_APIS, "Entry");

  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Invalid addrFamily - %d", addrFamily);
    return L7_FAILURE;
  }

  if ((protocolId == L7_MRP_UNKNOWN) || (protocolId >= L7_MRP_MAXIMUM))
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Invalid protocol Id - %d", protocolId);
    return L7_FAILURE;
  }

  if (notifyFn == L7_NULLPTR)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Invalid notifyFn");
    return L7_FAILURE;
  }

  if (addrFamily == L7_AF_INET)
  {
    mcastGblVariables_g.mcastV4StaticMRouteNotifyFn[protocolId] = notifyFn;
  }
  else
  {
    mcastGblVariables_g.mcastV6StaticMRouteNotifyFn[protocolId] = notifyFn;
  }

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  De-Register a routine to be called when changes occur within
*           the Multicast Static MRoute module that necessitates
*           communication with other modules for the given protocol.
*
* @param    addrFamily    @b{(input)} Address Family Identifier
* @param    protocolId    @b{(input)} Protocol ID (A identified constant in L7_MRP_TYPE_t )
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  The registration from MRPs is reentrant, yet the notify list is not 
*            protected as function pointer location is fixed for each 
*
* @end
*
*********************************************************************/
L7_RC_t
mcastMapStaticMRouteEventDeRegister (L7_uchar8 addrFamily,
                                     L7_MRP_TYPE_t protocolId)
{
  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_APIS, "Entry");

  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Invalid addrFamily - %d", addrFamily);
    return L7_FAILURE;
  }

  if ((protocolId == L7_MRP_UNKNOWN) || (protocolId >= L7_MRP_MAXIMUM))
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Invalid protocol Id - %d", protocolId);
    return L7_FAILURE;
  }

  if (addrFamily == L7_AF_INET)
  {
    mcastGblVariables_g.mcastV4StaticMRouteNotifyFn[protocolId] = L7_NULLPTR;
  }
  else
  {
    mcastGblVariables_g.mcastV6StaticMRouteNotifyFn[protocolId] = L7_NULLPTR;
  }

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/***************************************************************
*
* @purpose  Gets info corresponding to a given source address in
*           the static route table
*
* @param    addrFamily @b{(input)}  Address Family Identifier
* @param    srcAddr    @b{(input)}  Source IP Address
* @param    srcMask    @b{(output)} Source IP Address  Mask
* @param    rpfAddr    @b{(output)} RPF nexthop IP Address
* @param    intIfNum   @b{(output)} Internal Interface Num
* @param    preference @b{(output)} Route preference for the prefix
*
*
* @returns  L7_SUCCESS  Route found
* @returns  L7_FAILURE  Route not found.
*
* @comments
*
* @end
*
***************************************************************/
L7_RC_t
mcastMapStaticMRouteLookup (L7_uchar8 addrFamily,
                            L7_inet_addr_t *srcAddr,
                            L7_inet_addr_t *srcMask,
                            L7_inet_addr_t *rpfAddr,
                            L7_uint32 *intIfNum,
                            L7_uint32 *preference)
{
  L7_mcastMapCfg_t *cfgData = L7_NULLPTR;
  L7_mcastMapStaticRtsCfgData_t *staticMRouteEntry = L7_NULLPTR;
  L7_inet_addr_t tmpAddr1;
  L7_inet_addr_t tmpAddr2;
  L7_uint32 currNumStaticMrouteEntries = 0;
  L7_uint32 tableIndex = 0;
  L7_uchar8 src[MCAST_MAP_MAX_DBG_ADDR_SIZE];
  L7_uchar8 msk[MCAST_MAP_MAX_DBG_ADDR_SIZE];
  L7_uchar8 rpf[MCAST_MAP_MAX_DBG_ADDR_SIZE];

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_APIS, "Entry");

  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Invalid Address Family Identifier"
                     " -%d", addrFamily);
    return L7_FAILURE;
  }
  if ((srcAddr == L7_NULLPTR) || (srcMask == L7_NULLPTR) ||
      (rpfAddr == L7_NULLPTR) || (intIfNum == L7_NULLPTR) ||
      (preference == L7_NULLPTR))
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "srcAddr-%p (or) srcMask-%p (or) rpfAddr-%p "
                     "(or) intIfNum-%p (or) preference-%p is NULL", srcAddr,
                     srcMask, rpfAddr, intIfNum, preference);
    return L7_FAILURE;
  }

  if (mcastMapCfgDataGet (addrFamily, &cfgData) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "cfgData Get Failed for addrFamily "
                     "-%d", L7_INET_GET_FAMILY (srcAddr));
    return L7_FAILURE;
  }
  if (cfgData == L7_NULLPTR)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "cfgData is NULL for addrFamily "
                     "-%d", L7_INET_GET_FAMILY (srcAddr));
    return L7_FAILURE;
  }

  if ((currNumStaticMrouteEntries = cfgData->rtr.numStaticMRouteEntries) == 0)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Static MRoute Table is Empty");
    return L7_FAILURE;
  }

  for (tableIndex = 0; tableIndex < currNumStaticMrouteEntries; tableIndex++)
  {
    staticMRouteEntry = &cfgData->rtr.mcastStaticRtsCfgData[tableIndex];

    if (inetIsAddressZero (&staticMRouteEntry->source) != L7_TRUE)
    {
      inetAddressAnd (&staticMRouteEntry->source, &staticMRouteEntry->mask, &tmpAddr1);
      inetAddressAnd (srcAddr, srcMask, &tmpAddr2);

      if (L7_INET_IS_ADDR_EQUAL (&tmpAddr1, &tmpAddr2) == L7_TRUE)
      {
        if (addrFamily == L7_AF_INET6)
        {
          if (L7_IP6_IS_ADDR_LINK_LOCAL (&(staticMRouteEntry->rpfAddr.addr.ipv6)) != 0) 
          {
            *intIfNum = 0;
            if (nimIntIfFromConfigIDGet (&staticMRouteEntry->ifConfigId, intIfNum)
                                      != L7_SUCCESS)
            {
              MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "intIfNum Get Failed");
              return L7_FAILURE;
            }
            MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_CONFIGURED, "RPF Interface- %d", *intIfNum);
          }
          else
          {
            *intIfNum = 0;
          }
        }
        inetCopy (srcMask, &staticMRouteEntry->mask);
        inetCopy (rpfAddr, &(staticMRouteEntry->rpfAddr));
        *preference = staticMRouteEntry->preference;

        MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_CONFIGURED, "Static MRoute Entry Found; "
                         "srcAddr-%s, srcMask-%s, rpfAddr-%s, Metric-%d",
                         inetAddrPrint(srcAddr,src), inetAddrPrint(srcMask,msk),
                         inetAddrPrint(rpfAddr,rpf), *preference);
        return L7_SUCCESS;
      }
    }
  }

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Static MRoute Entry with "
                   "srcAddr - %s; Not Found", inetAddrPrint(srcAddr,src));
  inetAddressZeroSet (addrFamily,rpfAddr);
  *intIfNum = 0;
  *preference = 0;

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_APIS, "Exit");
  return L7_FAILURE;
}

/***************************************************************
*
* @purpose  Gets the best route corresponding to a given source
*           address in the static route table
*
* @param    addrFamily   @b{(input)}  Address Family Identifier
* @param    srcAddr      @b{(input)}  Source IP Address
* @param    bestRPFRoute @b{(output)} Mcast RPF Info
*
* @returns  L7_SUCCESS  Route found
* @returns  L7_FAILURE  Route not found.
*
* @comments
*
* @end
*
***************************************************************/
L7_RC_t
mcastMapStaticMRouteBestRouteLookup (L7_uchar8 addrFamily,
                                     L7_inet_addr_t *srcAddr,
                                     mcastRPFInfo_t *bestRPFRoute)
{
  L7_mcastMapCfg_t *cfgData = L7_NULLPTR;
  mcastRPFInfo_t rpfInfo;
  L7_uint32 intIfNum = 0;
  L7_uint32 rtrIfNum = 0;
  L7_uint32 preference = 0;
  L7_mcastMapStaticRtsCfgData_t *staticMRouteEntry = L7_NULLPTR;
  L7_mcastMapStaticRtsCfgData_t *bestStaticMRouteEntry = L7_NULLPTR;
  L7_inet_addr_t tmpAddr1;
  L7_inet_addr_t tmpAddr2;
  L7_uint32 currNumStaticMrouteEntries = 0;
  L7_uint32 tableIndex = 0;
  L7_uchar8 src[MCAST_MAP_MAX_DBG_ADDR_SIZE];
  L7_uchar8 msk[MCAST_MAP_MAX_DBG_ADDR_SIZE];
  L7_uchar8 rpf[MCAST_MAP_MAX_DBG_ADDR_SIZE];
  L7_int32 compResult = 0;

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_APIS, "Entry");

  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Invalid Address Family Identifier"
                     " -%d", addrFamily);
    return L7_FAILURE;
  }
  if ((srcAddr == L7_NULLPTR) || (bestRPFRoute == L7_NULLPTR))
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES,"srcAddr-%p or bestRPFRoute-%p is "
                     "NULL", srcAddr, bestRPFRoute);
    return L7_FAILURE;
  }

  if (mcastMapCfgDataGet (addrFamily, &cfgData) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "cfgData Get Failed for addrFamily "
                     "-%d", L7_INET_GET_FAMILY (srcAddr));
    return L7_FAILURE;
  }
  if (cfgData == L7_NULLPTR)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "cfgData is NULL for addrFamily "
                     "-%d", L7_INET_GET_FAMILY (srcAddr));
    return L7_FAILURE;
  }

  if ((currNumStaticMrouteEntries = cfgData->rtr.numStaticMRouteEntries) == 0)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Static MRoute Table is Empty");
    return L7_FAILURE;
  }

  for (tableIndex = 0; tableIndex < currNumStaticMrouteEntries; tableIndex++)
  {
    staticMRouteEntry = &cfgData->rtr.mcastStaticRtsCfgData[tableIndex];

    if (inetIsAddressZero (&staticMRouteEntry->source) != L7_TRUE)
    {
      inetAddressAnd (&staticMRouteEntry->source, &staticMRouteEntry->mask, &tmpAddr1);
      inetAddressAnd (srcAddr, &staticMRouteEntry->mask, &tmpAddr2);

      if (L7_INET_IS_ADDR_EQUAL (&tmpAddr1, &tmpAddr2) == L7_TRUE)
      {
        if (bestStaticMRouteEntry == L7_NULLPTR)
        {
          bestStaticMRouteEntry = staticMRouteEntry;
          continue;
        }
        else
        {
          compResult = L7_INET_ADDR_COMPARE (&staticMRouteEntry->mask, &bestStaticMRouteEntry->mask);
          if (compResult > 0)
          {
            bestStaticMRouteEntry = staticMRouteEntry;
            continue;
          }
          else if (compResult == 0)
          {
            if (staticMRouteEntry->preference < bestStaticMRouteEntry->preference)
            {
              bestStaticMRouteEntry = staticMRouteEntry;
              continue;
            }
          }
          else
          {
            /* Do nothing */
          }
        }
      }
    }
  }

  if (bestStaticMRouteEntry == L7_NULLPTR)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Static MRoute Entry with "
                     "srcAddr - %s; Not Found", inetAddrPrint(srcAddr,src));
    return L7_FAILURE;
  }

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_CONFIGURED, "Static MRoute Entry Best Match Found; "
                   "srcAddr-%s, srcMask-%s, rpfAddr-%s, Preference-%d",
                   inetAddrPrint(&bestStaticMRouteEntry->source,src), inetAddrPrint(&bestStaticMRouteEntry->mask,msk),
                   inetAddrPrint(&bestStaticMRouteEntry->rpfAddr,rpf), preference);

  memset (&rpfInfo, 0, sizeof(mcastRPFInfo_t));

  /* rpfNextHop */
  inetAddressZeroSet (addrFamily, &rpfInfo.rpfNextHop);
  inetCopy (&rpfInfo.rpfNextHop, &bestStaticMRouteEntry->rpfAddr);

  /* routeProtocol */
  rpfInfo.rpfRouteProtocol = RTO_STATIC; /* TODO */

  /* rpfRouteAddress */
  inetAddressZeroSet (addrFamily, &rpfInfo.rpfRouteAddress);
  inetCopy (&rpfInfo.rpfRouteAddress, &bestStaticMRouteEntry->source);

  /* rpfRouteMetricPref */
  rpfInfo.rpfRouteMetricPref = bestStaticMRouteEntry->preference;

  /* rpfRouteMetric */
  rpfInfo.rpfRouteMetric = 0;

  /* prefixLength */
  if (inetMaskToMaskLen (&bestStaticMRouteEntry->mask, (L7_uchar8*) &rpfInfo.prefixLength)
                      != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Mask to MaskLen Conversion "
                     "Failed for srcMask - %s", inetAddrPrint(&bestStaticMRouteEntry->mask,msk));
    return L7_FAILURE;
  }

  /* rpfIfIndex */
  if (addrFamily == L7_AF_INET6)
  {
    if (L7_IP6_IS_ADDR_LINK_LOCAL (&(bestStaticMRouteEntry->rpfAddr.addr.ipv6)) != 0) 
    {
      intIfNum = 0;
      if (nimIntIfFromConfigIDGet (&bestStaticMRouteEntry->ifConfigId, &intIfNum)
                                != L7_SUCCESS)
      {
        MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "intIfNum Get Failed");
        return L7_FAILURE;
      }
      MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_CONFIGURED, "RPF Interface- %d", intIfNum);
    }
    else
    {
      intIfNum = 0;
    }
  }
  if (intIfNum != 0)
  {
    if (mcastIpMapIntIfNumToRtrIntf (addrFamily, intIfNum, &rtrIfNum) != L7_SUCCESS)
    {
      MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "intIfNum - %d to rtrIfNum -%d "
                       "Conversion Failed", intIfNum, rtrIfNum);
      return L7_FAILURE;
    }
  }
  else
  {
    if (mcastIpAddrRtrIntfResolve (&rpfInfo.rpfNextHop, &rtrIfNum) != L7_SUCCESS)
    {
      MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "rpfNextHop to rtrIfNum Conversion "
                       "Failed for rpfAddr - %s", inetAddrPrint(&rpfInfo.rpfNextHop,rpf));
      return L7_FAILURE;
    }
  }
  rpfInfo.rpfIfIndex = rtrIfNum;

  memcpy (bestRPFRoute, &rpfInfo, sizeof(mcastRPFInfo_t));

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_CONFIGURED, "RPF Info Found; "
                   "srcAddr-%s, srcMaskLen-%d, rpfAddr-%s, rtrIfNum-%d, Preference-%d",
                   inetAddrPrint(&rpfInfo.rpfRouteAddress,src), rpfInfo.prefixLength,
                   inetAddrPrint(&rpfInfo.rpfNextHop,rpf), rtrIfNum, preference);

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

