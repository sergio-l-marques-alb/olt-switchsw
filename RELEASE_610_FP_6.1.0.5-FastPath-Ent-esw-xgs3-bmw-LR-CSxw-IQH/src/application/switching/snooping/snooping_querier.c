/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename snooping_querier.c
*
* @purpose Contains definitions to support the Snooping Querier 
*
* @component
*
* @comments
*
* @create 07-Dec-2006
*
* @author drajendra
* @end
*
**********************************************************************/
#include "datatypes.h"
#include "osapi_support.h"
#include "ipv6_commdefs.h"
#include "l7utils_inet_addr_api.h"
#include "simapi.h"

#include "snooping_util.h"
#include "snooping_outcalls.h"
#include "snooping_debug.h"
#include "snooping_db.h"
#include "snooping_proto.h"
#include "ptin_snoop_stat_api.h"

#include "ptin/logger.h"

/* Snooping Querier state transition table */
static snoopQuerierState_t snoopQuerierStateTable[snoopQuerierSTEvents][SNOOP_QUERIER_STATES] =
{
 /*Ev/St      S0(Disabled)               S1(Querier)           S2(Non-Querier)  */
 /*E0*/  {SNOOP_QUERIER_QUERIER,   SNOOP_QUERIER_QUERIER,      SNOOP_QUERIER_NON_QUERIER},
 /*E1*/  {SNOOP_QUERIER_DISABLED,  SNOOP_QUERIER_NON_QUERIER,  SNOOP_QUERIER_NON_QUERIER},
 /*E2*/  {SNOOP_QUERIER_DISABLED,  SNOOP_QUERIER_QUERIER,      SNOOP_QUERIER_QUERIER},
 /*E3*/  {SNOOP_QUERIER_DISABLED,  SNOOP_QUERIER_QUERIER,      SNOOP_QUERIER_QUERIER},
 /*E4*/  {SNOOP_QUERIER_DISABLED,  SNOOP_QUERIER_DISABLED,     SNOOP_QUERIER_DISABLED},
 /*E5*/  {SNOOP_QUERIER_DISABLED,  SNOOP_QUERIER_QUERIER,      SNOOP_QUERIER_QUERIER},
 /*E6*/  {SNOOP_QUERIER_DISABLED,  SNOOP_QUERIER_QUERIER,      SNOOP_QUERIER_NON_QUERIER},
 /*E7*/  {SNOOP_QUERIER_NON_QUERIER, SNOOP_QUERIER_NON_QUERIER, SNOOP_QUERIER_NON_QUERIER}
};

/*********************************************************************
* @purpose  Process snooping querier event and take action according
*           to the snooping querier state transition table
*
* @param    pSnoopOperEntry  @b{(input)} Snooping operational entry
* @param    event            @b{(input)} Querier Event
* @param    pSnoopCB         @b{(input)} Control Blcok
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
void snoopQuerierSMProcessEvent(snoopOperData_t *pSnoopOperEntry, 
                                snoopQuerierSTEvents_t event, 
                                snoop_cb_t *pSnoopCB)
{
  snoopQuerierState_t previousState, nextState;

  previousState = pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperState;

  switch (event)
  {
  case snoopQuerierBegin:
    /* To start with set the oper version same as config version */
    pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperVersion = 
    pSnoopCB->snoopCfgData->snoopQuerierCfgData.snoopQuerierVersion; 
    break;
  default:
    break;
  }

  nextState = snoopQuerierStateTable[event][previousState];
  pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperState =  nextState;

  SNOOP_TRACE(SNOOP_DEBUG_QUERIER,pSnoopCB->family,\
              "Previos State = S%d  Next State = S%d  Event = E%d",\
              previousState, nextState, event);

  switch (nextState)
  {
  case SNOOP_QUERIER_DISABLED: /*S0*/
  case SNOOP_QUERIER_NON_QUERIER: /*S2*/
     if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.queryIntervalTimer != L7_NULL)
     {
       if (snoopQuerierTimerStop(pSnoopOperEntry, SNOOP_QUERIER_QUERY_INTERVAL_TIMER) != L7_SUCCESS)
       {
         LOG_MSG("snoopQuerierSMProcessEvent: Failed to stop Query Timer for vlan %d family %d\n",
                 pSnoopOperEntry->vlanId, pSnoopCB->cbIndex);
       }
     }
     break;

  case SNOOP_QUERIER_QUERIER:  /*S1*/
     /* Start/Update the periodic timer */
     /* Assign version */
     if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperVersion == 0
         || pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperVersion > 
         pSnoopCB->snoopCfgData->snoopQuerierCfgData.snoopQuerierVersion ||
         pSnoopOperEntry->snoopQuerierInfo.querierVersion == 0)/* no last querier detected */
     {
       pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperVersion = 
       pSnoopCB->snoopCfgData->snoopQuerierCfgData.snoopQuerierVersion; 
     }

     if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.queryIntervalTimer == L7_NULL)
     {
       /* Send out the query */
       snoopQuerierPeriodicQuerySend(pSnoopOperEntry);
       if (snoopQuerierTimerStart(pSnoopOperEntry, SNOOP_QUERIER_QUERY_INTERVAL_TIMER, 
                                  pSnoopCB->snoopCfgData->snoopQuerierCfgData.snoopQuerierQueryInterval) 
                                  != L7_SUCCESS)
       {
         LOG_MSG("snoopQuerierQueryProcess: Failed to start Querier Expiry Timer for vlan %d family %d\n",
                 pSnoopOperEntry->vlanId, pSnoopCB->family);
       }
     }
     else
     {
       if (event == snoopQuerierVersionConflict) 
       {
         snoopQuerierPeriodicQuerySend(pSnoopOperEntry);

         if (snoopQuerierTimerUpdate(pSnoopOperEntry, SNOOP_QUERIER_QUERY_INTERVAL_TIMER, 
                                     pSnoopCB->snoopCfgData->snoopQuerierCfgData.snoopQuerierQueryInterval) 
                                     != L7_SUCCESS)
         {
           LOG_MSG("snoopQuerierQueryProcess: Failed to update Querier Expiry Timer for vlan %d family %d\n",
                   pSnoopOperEntry->vlanId, pSnoopCB->family);
         }
       }
     }   
     break;

   default:
     LOG_MSG("snoopQuerierProcessStateChange: Snooping Querier is moved to an invalid state\n");
     break;
  }
}
/*********************************************************************
* @purpose  Process a Snoop Querier internal event 
*
* @param    msg  @b{(input)} Received internal event
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void snoopQuerierProcessMessage(snoopMgmtMsg_t *msg)
{
  // Process message
  switch (msg->msgId)
  {
  case snoopQuerierModeChange:
     (void)snoopQuerierAdminModeApply(msg->u.mode.admin, msg->cbHandle);
     break;

  case snoopQuerierVlanModeChange:
     (void)snoopQuerierVlanModeApply(msg->vlanId, msg->u.mode.admin, msg->cbHandle);
     break;

  case snoopQuerierAddressChange:
     (void)snoopQuerierAddressApply(msg->cbHandle);
     break;

  case snoopQuerierNetworkAddressChange:
     (void)snoopQuerierNetworkAddressApply(msg->cbHandle);
     break;

  case snoopQuerierVlanAddressChange:
     (void)snoopQuerierVlanAddressApply(msg->vlanId, msg->cbHandle);
     break;

  case snoopQuerierVersionChange:
     (void)snoopQuerierVersionApply(msg->cbHandle);
     break;

  case snoopQuerierVlanElectionModeChange:
     (void)snoopQuerierVlanElectionParticipateModeApply(msg->vlanId, 
                                                        msg->u.mode.admin, 
                                                        msg->cbHandle);

  default:
     break;
  }
}
/*********************************************************************
* @purpose  Apply snoop querier global admin mode
*
* @param    mode      @b{(input)} Snoop Querier Admin Mode
* @param    pSnoopCB  @b{(input)} Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopQuerierAdminModeApply(L7_uint32 mode, snoop_cb_t *pSnoopCB)
{
  L7_RC_t           rc = L7_SUCCESS;
  L7_uint32         vlanId;
  snoopOperData_t  *pSnoopOperEntry  = L7_NULLPTR;
  /* Modify the querier mode of all the VLAN's and transition the SM for all
     querier enabled vlans. Call vlanMode apply internally */
  if (mode == L7_ENABLE)
  { 
    pSnoopOperEntry = snoopOperEntryFirstGet(pSnoopCB);
    while(pSnoopOperEntry)
    {
      vlanId = pSnoopOperEntry->vlanId;

      if (pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].snoopVlanQuerierCfgData.snoopQuerierVlanAdminMode
                     & SNOOP_QUERIER_MODE)
      {  
        if (snoopQuerierVlanModeApply(vlanId, L7_ENABLE, pSnoopCB) != L7_SUCCESS)
        {
          rc = L7_FAILURE;
        }
      }/* Snooping querier */
      pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_GETNEXT);
    } /* End of vlan iterations */
  }
  else
  {
    pSnoopOperEntry = snoopOperEntryFirstGet(pSnoopCB);
    while(pSnoopOperEntry)
    {
      vlanId = pSnoopOperEntry->vlanId;

      if (snoopQuerierVlanModeApply(vlanId, L7_DISABLE, pSnoopCB) != L7_SUCCESS)
      {
        rc = L7_FAILURE;
      }
      pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_GETNEXT);
    } /* End of vlan iterations */
  }/* End of mode comparision */

  if (rc != L7_SUCCESS)
  {
    LOG_MSG("snoopQuerierAdminModeApply: Failed to apply admin mode %d family %d\n", 
            mode, pSnoopCB->family);
  }
  return rc;
}
/*********************************************************************
* @purpose  Apply snoop querier Vlan admin mode
*
* @param    vlanId     @b{(input)} VLAN ID
* @param    mode       @b{(input)} Snoop Querier VLAN Admin Mode
* @param    pSnoopCB   @b{(input)} Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopQuerierVlanModeApply(L7_uint32 vlanId, L7_uint32 mode,
                                  snoop_cb_t *pSnoopCB)
{
  snoopOperData_t  *pSnoopOperEntry  = L7_NULLPTR;

  /* Modify the querier mode of this VLAN's and transition the SM */
  if ((pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_EXACT)) 
                      == L7_NULLPTR)
  { 
    return L7_FAILURE;
  }

  if (mode == L7_ENABLE)
  {
    if (snoopQuerierVlanReady(vlanId, pSnoopCB) == L7_TRUE)
    {
      snoopQuerierStart(pSnoopOperEntry, pSnoopCB);
    }
  }
  else
  {
    snoopQuerierSMProcessEvent(pSnoopOperEntry, snoopQuerierDisable, pSnoopCB);
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Apply snoop querier Vlan Election participate admin mode
*
* @param    vlanId     @b{(input)} VLAN ID
* @param    mode       @b{(input)} Snoop Querier VLAN Election participate
*                                  Admin Mode
* @param    pSnoopCB   @b{(input)} Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopQuerierVlanElectionParticipateModeApply(L7_uint32 vlanId, 
                                                     L7_uint32 mode,
                                                     snoop_cb_t *pSnoopCB)
{
  snoopOperData_t  *pSnoopOperEntry  = L7_NULLPTR;

  /* Modify the querier mode of this VLAN's and transition the SM */
  if ((pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_EXACT)) 
                      == L7_NULLPTR)
  { 
    return L7_FAILURE;
  }

  if (mode == L7_ENABLE)
  {
    if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperState == SNOOP_QUERIER_NON_QUERIER)
    {
      /* Re-start querier election process */           
      snoopQuerierSMProcessEvent(pSnoopOperEntry, snoopQuerierRxInferiorQryMoveToQ, 
                                 pSnoopCB);
    }
  }/* End of mode enable check */
  else
  { 
    if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperState != SNOOP_QUERIER_DISABLED)
    {
      /* Take last querier's version as current version as election mode is
         now disabled */
      if (pSnoopOperEntry->snoopQuerierInfo.querierVersion &&
          pSnoopOperEntry->snoopQuerierInfo.querierVersion != pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperVersion)
      {
        pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperVersion =
        pSnoopOperEntry->snoopQuerierInfo.querierVersion;
      }
      snoopQuerierSMProcessEvent(pSnoopOperEntry, snoopQuerierRxSuperiorQryMoveToNQ, 
                                 pSnoopCB);
      if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.querierExpiryTimer == L7_NULL)
      {
        (void)snoopQuerierTimerStart(pSnoopOperEntry, 
                                     SNOOP_QUERIER_QUERIER_EXPIRY_TIMER,
                                     pSnoopCB->snoopCfgData->snoopQuerierCfgData.snoopQuerierExpiryInterval);
      }/* End of other timer running check */
    }/* End of querier oper state disabled check */
  } /* End of mode disable check */

  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Apply configured snoop querier version for a snoop instance
*
* @param    pSnoopCB   @b{(input)} Control Block
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
***********************************************************************/
L7_RC_t snoopQuerierVersionApply(snoop_cb_t *pSnoopCB)
{
  L7_uint32         vlanId;
  L7_RC_t           rc = L7_SUCCESS;
  snoopOperData_t  *pSnoopOperEntry  = L7_NULLPTR;
  /* Go through the enabled vlans and update querier oper info to a lower
     one if changed */
  pSnoopOperEntry = snoopOperEntryFirstGet(pSnoopCB);
  while(pSnoopOperEntry)
  {
    if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperState 
                                                      != SNOOP_QUERIER_DISABLED)
    {/* Restart the querier- Send disable and then enable events */
      snoopQuerierSMProcessEvent(pSnoopOperEntry, snoopQuerierDisable, pSnoopCB);
      snoopQuerierStart(pSnoopOperEntry, pSnoopCB);
    }
    vlanId = pSnoopOperEntry->vlanId;
    pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_GETNEXT);
  } /* End of vlan iterations */

  return rc;
}
/*********************************************************************
* @purpose  Apply configured snoop querier address of a snoop instance
*
* @param    pSnoopCB   @b{(input)} Control Block
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopQuerierAddressApply(snoop_cb_t *pSnoopCB)
{
  L7_uint32         vlanId;
  L7_RC_t           rc = L7_SUCCESS;
  snoopOperData_t  *pSnoopOperEntry  = L7_NULLPTR;
  /* Go through the enabled vlans and update querier info snoopQuerierOperAddress
     to the configured value. Call  snoopQuerierVlanAddressApply internally */
  pSnoopOperEntry = snoopOperEntryFirstGet(pSnoopCB);
  while(pSnoopOperEntry)
  {
    vlanId = pSnoopOperEntry->vlanId;
    if (inetIsAddressZero(&pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].snoopVlanQuerierCfgData.snoopQuerierVlanAddress) == L7_TRUE)
    {
      if (snoopQuerierVlanAddressApply(vlanId, pSnoopCB) != L7_SUCCESS)
      { 
        rc = L7_FAILURE;
      }
    }/* Check if vlan address is already present. If so no need to re-apply */
    pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_GETNEXT);
  } /* End of vlan iterations */

  return rc;
}
/*********************************************************************
* @purpose  Apply newly assigned/DHCP learnt network address
*           as snoop querier address of a snoop instance
*
* @param    pSnoopCB   @b{(input)} Control Block
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopQuerierNetworkAddressApply(snoop_cb_t *pSnoopCB)
{
  L7_uint32         vlanId;
  L7_RC_t           rc = L7_SUCCESS;
  snoopOperData_t  *pSnoopOperEntry  = L7_NULLPTR;
  /* Go through the enabled vlans and update querier info snoopQuerierOperAddress
     to the configured value. Call  snoopQuerierVlanAddressApply internally */
  pSnoopOperEntry = snoopOperEntryFirstGet(pSnoopCB);
  while(pSnoopOperEntry)
  {
    vlanId = pSnoopOperEntry->vlanId;
    if (inetIsAddressZero(&pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].snoopVlanQuerierCfgData.snoopQuerierVlanAddress) == L7_TRUE &&
        inetIsAddressZero(&pSnoopCB->snoopCfgData->snoopQuerierCfgData.snoopQuerierAddress) == L7_TRUE)
    {
      if (snoopQuerierVlanAddressApply(vlanId, pSnoopCB) != L7_SUCCESS)
      {
        rc = L7_FAILURE;
      }
    }/* Check if vlan address is already present. If so no need to re-apply */
    pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_GETNEXT);
  } /* End of vlan iterations */

  return rc;
}
/***************************************************************************
* @purpose  Apply configured snoop querier vlan address of a snoop instance
*
* @param    vlanId     @b{(input)} VLAN ID
* @param    pSnoopCB   @b{(input)} Control Block
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
***************************************************************************/
L7_RC_t snoopQuerierVlanAddressApply(L7_uint32 vlanId, snoop_cb_t *pSnoopCB)
{
  L7_inet_addr_t   inetAddr;

  /* update querier info snoopQuerierOperAddress to the configured value. 
     try to transition the SM if it is in disabled state */

   if (snoopQuerierAddressReady(vlanId, pSnoopCB, &inetAddr) == L7_TRUE)
   {
     if (snoopQuerierVlanModeApply(vlanId, L7_DISABLE, pSnoopCB) != L7_SUCCESS)
     {
       LOG_MSG("snoopQuerierVlanAddressApply: Failed to disable querier on vlan %d family %d",
               vlanId, pSnoopCB->family);
       return L7_FAILURE;
     }
     if (snoopQuerierVlanModeApply(vlanId, L7_ENABLE, pSnoopCB)!=L7_SUCCESS)  return L7_FAILURE;
   }
   else
   {
     if (snoopQuerierVlanModeApply(vlanId, L7_DISABLE, pSnoopCB)!=L7_SUCCESS)  return L7_FAILURE;
   }

   return L7_SUCCESS;
}
/***************************************************************************
* @purpose  Obtain the snooping querier's ip address
*
* @param    vlanId       @b{(input)}  VLAN ID
* @param    pSnoopCB     @b{(input)}  Control Block
* @param    querierAddr  @b{(output)} Querier Address
*
* @returns  L7_TRUE   A non-zero querier address exists
* @returns  L7_FALSE  Querier address is not configured
*
* @notes    The querier address is decided in the following priority
*           1. VLAN Querier Address
*           2. Global Querier Address
*           3. Management Address (in the case of IGMP)
*
* @end
***************************************************************************/
L7_BOOL snoopQuerierAddressReady(L7_uint32 vlanId, snoop_cb_t *pSnoopCB, 
                                 L7_inet_addr_t *querierAddr)
{
  L7_uint32       ipAddr, acount, idx;
  L7_in6_prefix_t ipv6Prefixes[L7_RTR6_MAX_INTF_ADDRS*2];

  if (inetIsAddressZero(&pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].snoopVlanQuerierCfgData.snoopQuerierVlanAddress) == L7_FALSE)
  {
    memcpy(querierAddr, &pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].snoopVlanQuerierCfgData.snoopQuerierVlanAddress,
           sizeof(L7_inet_addr_t));
    return L7_TRUE;
  }
  else if (inetIsAddressZero(&pSnoopCB->snoopCfgData->snoopQuerierCfgData.snoopQuerierAddress) == L7_FALSE)
  {
    memcpy(querierAddr, 
           &pSnoopCB->snoopCfgData->snoopQuerierCfgData.snoopQuerierAddress,
           sizeof(L7_inet_addr_t));
    return L7_TRUE;
  }
  else if (pSnoopCB->family == L7_AF_INET)
  {
    ipAddr = simGetSystemIPAddr();
    if (ipAddr != 0)
    {
      inetAddressSet(L7_AF_INET, &ipAddr, querierAddr);
      return L7_TRUE;
    }
  }
  else if (pSnoopCB->family == L7_AF_INET6)
  {
    acount = L7_RTR6_MAX_INTF_ADDRS*2;
    if (simGetSystemIPV6Addrs(ipv6Prefixes, &acount) == L7_SUCCESS)
    {  
       for (idx = 0; idx < acount; idx++)
       {
         if (L7_IP6_IS_ADDR_LINK_LOCAL(&ipv6Prefixes[idx].in6Addr))
         {
           inetAddressSet(L7_AF_INET6, &ipv6Prefixes[idx].in6Addr, querierAddr);
           return L7_TRUE;
         }
       }/* End of iterations of all ipv6 addresses */
    } /* End of get ipv6 mgmt address */
  }/* End of address valid checks */
  
  return L7_FALSE;
}
/***************************************************************************
* @purpose  Check if VLAN is ready to move to operationally enabled state
*
* @param    vlanId       @b{(input)}  VLAN ID
* @param    pSnoopCB     @b{(input)}  Control Block
*
* @returns  L7_TRUE   Snooping querier can be enabled on VLAN operationally
* @returns  L7_FALSE  VLAN is not ready
*
* @notes    The vlan state  if all the following is true
*           1. Snoop Admin mode enabled
*           2. Snoop Querier Admin mode enabled
*           3. Snoop VLAN Mode enabled
*           4. Snoop Querier VLAN mode enabled
*           5. Snoop Querier Address is non-zero
*
* @end
***************************************************************************/
L7_BOOL snoopQuerierVlanReady(L7_uint32 vlanId, snoop_cb_t *pSnoopCB)
{
  L7_inet_addr_t inetAddr; /* temp var */

  if (pSnoopCB->snoopCfgData->snoopAdminMode == L7_ENABLE)
  {
    if (pSnoopCB->snoopCfgData->snoopQuerierCfgData.snoopQuerierAdminMode == L7_ENABLE)
    {
      if (pSnoopCB->snoopCfgData->snoopVlanMode[vlanId] & SNOOP_VLAN_MODE)
      {
        if (L7_VLAN_ISMASKBITSET(pSnoopCB->snoopExec->routingVlanVidMask, vlanId) == 0)
        {
          if (pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].snoopVlanQuerierCfgData.snoopQuerierVlanAdminMode
              & SNOOP_QUERIER_MODE)
          {
            if (snoopQuerierAddressReady(vlanId, pSnoopCB, &inetAddr) == L7_TRUE)
            {
              return L7_TRUE;
            }/* Have a valid address */
          }/* Snoop Querier mode enabled on vlan */
        }/* Not a vlan routing interfafce */
      }/* Snoop mode enabled on vlan */
    } /* Snoop Querier enabled globally */
  } /* Snoop enabled globally */

  return L7_FALSE;
}
/*********************************************************************
* @purpose  Process a non-zero source ip address query message
*
* @param    mcastPacket  @b{(input)} Received control message
*
* @returns  L7_SUCCESS if it is a valid packet
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t  snoopQuerierQueryProcess(mgmdSnoopControlPkt_t *mcastPacket)
{
  L7_uchar8            *dataPtr, byteVal;
  L7_mgmdQueryMsg_t     mgmdMsg;
  L7_uint32             ipv4Addr, incomingVersion = 0;
  L7_uint32             otherQuerierTimer = 1;
  L7_uint32             maxRespTime, qqic, qrv;
  snoop_cb_t           *pSnoopCB;
  snoopQuerierState_t   querierState;
  L7_in6_addr_t         ipv6Addr;
  L7_ushort16           shortVal;
  snoopOperData_t      *pSnoopOperEntry  = L7_NULLPTR;

  /* Set pointer to IGMP message */
  dataPtr = mcastPacket->ip_payload;
  pSnoopCB = mcastPacket->cbHandle;
  memset(&mgmdMsg, 0x00, sizeof(L7_mgmdQueryMsg_t));

  if ((pSnoopOperEntry = snoopOperEntryGet(mcastPacket->vlanId, pSnoopCB, 
                         L7_MATCH_EXACT)) == L7_NULLPTR)
  {
     LOG_MSG("snoopMgmdMembershipQueryProcess: Invalid vlan %d", 
            mcastPacket->vlanId);
     return L7_FAILURE;
  }

  if (pSnoopCB->family == L7_AF_INET) /* IGMP Message */
  {
    SNOOP_GET_BYTE(mgmdMsg.mgmdType, dataPtr);         /* Version/Type */
    if (mgmdMsg.mgmdType == L7_IGMP_MEMBERSHIP_QUERY)
    {
      SNOOP_GET_BYTE(mgmdMsg.igmpMaxRespTime, dataPtr);  /* Max response time */
      SNOOP_GET_SHORT(mgmdMsg.mgmdChecksum, dataPtr);    /* Checksum */
      SNOOP_GET_ADDR(&ipv4Addr, dataPtr);    /* Group Address */
      inetAddressSet(L7_AF_INET, &ipv4Addr, &mgmdMsg.mgmdGroupAddr);
      if (mcastPacket->ip_payload_length > SNOOP_IGMPv1v2_HEADER_LENGTH)
      {
        SNOOP_GET_BYTE(mgmdMsg.sQRV, dataPtr);  /* Robustness Var */
        SNOOP_GET_BYTE(mgmdMsg.qqic, dataPtr);  /* QQIC */
        SNOOP_GET_SHORT(mgmdMsg.numSources, dataPtr);  /* Number of sources */
        SNOOP_UNUSED_PARAM(dataPtr);
        if (mcastPacket->ip_payload_length != 
            (SNOOP_IGMPV3_HEADER_MIN_LENGTH + 
             mgmdMsg.numSources * sizeof(L7_in_addr_t)))
        {
         SNOOP_TRACE(SNOOP_DEBUG_PROTO,pSnoopCB->family,\
             "Illegal IGMP v3packet length = %d", mcastPacket->ip_payload_length);
          return L7_FAILURE;
        }
        incomingVersion = SNOOP_IGMP_VERSION_3;
      }
      else if (mgmdMsg.igmpMaxRespTime == 0)
      {
        if (mcastPacket->ip_payload_length != SNOOP_IGMPv1v2_HEADER_LENGTH)
        {
         SNOOP_TRACE(SNOOP_DEBUG_PROTO,pSnoopCB->family,\
             "Illegal IGMPv1 packet length = %d", mcastPacket->ip_payload_length);
          return L7_FAILURE;
        }
        incomingVersion = SNOOP_IGMP_VERSION_1;
      }
      else
      {
        if (mcastPacket->ip_payload_length != SNOOP_IGMPv1v2_HEADER_LENGTH)
        {
           SNOOP_TRACE(SNOOP_DEBUG_PROTO,pSnoopCB->family,\
             "Illegal IGMPv2 packet length = %d", mcastPacket->ip_payload_length);
          return L7_FAILURE;
        }
        incomingVersion = SNOOP_IGMP_VERSION_2;
      }
    }/* Is IGMP pkt check */
  }
  else /* MLD Message */
  {
    memset(&mgmdMsg, 0x00, sizeof(L7_mgmdQueryMsg_t));
    SNOOP_GET_BYTE(mgmdMsg.mgmdType, dataPtr);   /* Version/Type */
    SNOOP_GET_BYTE(byteVal, dataPtr);  /* Code */
    SNOOP_UNUSED_PARAM(byteVal); 
    SNOOP_GET_SHORT(mgmdMsg.mgmdChecksum, dataPtr);    /* Checksum */
    SNOOP_GET_SHORT(mgmdMsg.mldMaxRespTime, dataPtr);  /* Max response time */
    SNOOP_GET_SHORT(mgmdMsg.mgmdReserved, dataPtr);               /* rserved */

    SNOOP_GET_ADDR6(&ipv6Addr, dataPtr); /* Group Address */
    inetAddressSet(L7_AF_INET6, &ipv6Addr, &mgmdMsg.mgmdGroupAddr);
    if (mcastPacket->ip_payload_length > SNOOP_MLDV1_HEADER_LENGTH) /* MIN MLD qry length */
    {
      SNOOP_GET_BYTE(mgmdMsg.sQRV, dataPtr);  /* Robustness Var */
      SNOOP_GET_BYTE(mgmdMsg.qqic, dataPtr);  /* QQIC */
      SNOOP_GET_SHORT(mgmdMsg.numSources, dataPtr);  /* Number of sources */
      SNOOP_UNUSED_PARAM(dataPtr);
      if (mcastPacket->ip_payload_length != 
          (mgmdMsg.numSources * sizeof(L7_in6_addr_t) + SNOOP_MLDV2_HEADER_MIN_LENGTH))
      {
       SNOOP_TRACE(SNOOP_DEBUG_PROTO,pSnoopCB->family,\
            "Illegal MLDv2 packet length = %d", mcastPacket->ip_payload_length);
        return L7_FAILURE;
      }
      incomingVersion = SNOOP_MLD_VERSION_2;
    }
    else
    {
      if (mcastPacket->ip_payload_length != SNOOP_MLDV1_HEADER_LENGTH)
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO,pSnoopCB->family,\
           "Illegal MLDv1 packet length = %d", mcastPacket->ip_payload_length);
        return L7_FAILURE;
      }
      incomingVersion = SNOOP_MLD_VERSION_1;
    }
  }/* End of MLD Message check */

  /* Update the local information with the received info */
  memcpy(&pSnoopOperEntry->snoopQuerierInfo.querierAddress,
         &mcastPacket->srcAddr, sizeof(L7_inet_addr_t));
  pSnoopOperEntry->snoopQuerierInfo.querierVersion = incomingVersion;
  pSnoopOperEntry->snoopQuerierInfo.maxResponseCode
                                = ((L7_AF_INET == pSnoopCB->family)
                                   ? mgmdMsg.igmpMaxRespTime
                                   : mgmdMsg.mldMaxRespTime);
  pSnoopOperEntry->snoopQuerierInfo.qqic = mgmdMsg.qqic;
  pSnoopOperEntry->snoopQuerierInfo.sFlagQRV = mgmdMsg.sQRV;

  querierState = pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperState;

  if (querierState != SNOOP_QUERIER_DISABLED)
  {
    if (snoopQuerierOtherQuerierDetected(pSnoopOperEntry, mcastPacket, 
                                         &mgmdMsg, incomingVersion) 
                                         != L7_SUCCESS)
    {
      LOG_MSG("snoopQuerierQueryProcess: Failed to process other querier message\n");
    }
  }
  else
  {
    pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperVersion = incomingVersion;
  }

  /* Update the (other) querier expiry timer Based on the current state 
     of snooping querier */
  if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperState ==
                                      SNOOP_QUERIER_DISABLED)
  {
    if (pSnoopCB->family == L7_AF_INET)
    {
     if (incomingVersion == SNOOP_IGMP_VERSION_1)
      {
        otherQuerierTimer = SNOOP_MGMD_OTHER_QRYR_INTRVL_GET(SNOOP_MGMD_DEFAULT_RBST_VAR,\
                            SNOOP_MGMD_DEFAULT_QRY_INTRVL,SNOOP_MGMD_DEFAULT_MAX_RESP_TIME);
      }
      else if (incomingVersion == SNOOP_IGMP_VERSION_2)
      {
        maxRespTime = mgmdMsg.igmpMaxRespTime;
        maxRespTime = SNOOP_MAXRESP_INTVL_ROUND(maxRespTime, SNOOP_IGMP_FP_DIVISOR);
        otherQuerierTimer = SNOOP_MGMD_OTHER_QRYR_INTRVL_GET(SNOOP_MGMD_DEFAULT_RBST_VAR,\
                            SNOOP_MGMD_DEFAULT_QRY_INTRVL,maxRespTime);
      }
      else if (incomingVersion == SNOOP_IGMP_VERSION_3) 
      {
        L7_ushort16 shortVal;              /* IGMP v3 */
        snoopFPDecode(mgmdMsg.igmpMaxRespTime, &shortVal);
        maxRespTime = SNOOP_MAXRESP_INTVL_ROUND(shortVal,SNOOP_IGMP_FP_DIVISOR);
        if (mgmdMsg.sQRV > 7 || (SNOOP_MGMD_RBST_VAR_GET(mgmdMsg.sQRV) == 0))
        {
          qrv = SNOOP_MGMD_DEFAULT_RBST_VAR; 
        }
        else
        {
          qrv = SNOOP_MGMD_RBST_VAR_GET(mgmdMsg.sQRV);
        }

        snoopFPDecode(mgmdMsg.qqic, &shortVal);
        qqic = shortVal;
        if (!qqic)
        {
          qqic = SNOOP_MGMD_DEFAULT_QRY_INTRVL;
        }
        otherQuerierTimer = SNOOP_MGMD_OTHER_QRYR_INTRVL_GET(qrv, qqic, maxRespTime);
      }
    }
    else
    {
      if (incomingVersion == SNOOP_MLD_VERSION_1)
      {
        /* MLD v1 */
        maxRespTime  = mgmdMsg.mldMaxRespTime;
        maxRespTime = SNOOP_INTERVAL_ROUND(maxRespTime,SNOOP_MLD_FP_DIVISOR);
        otherQuerierTimer = SNOOP_MGMD_OTHER_QRYR_INTRVL_GET(SNOOP_MGMD_DEFAULT_RBST_VAR,\
                                                             SNOOP_MGMD_DEFAULT_QRY_INTRVL,\
                                                             maxRespTime);
      }
      else if (incomingVersion == SNOOP_MLD_VERSION_2) 
      {
        /* MLD v2 */
        snoopMLDFPDecode(mgmdMsg.mldMaxRespTime, &maxRespTime);
        maxRespTime = SNOOP_INTERVAL_ROUND(maxRespTime,SNOOP_MLD_FP_DIVISOR);
        if (mgmdMsg.sQRV > 7 || (SNOOP_MGMD_RBST_VAR_GET(mgmdMsg.sQRV) == 0))
        {
          qrv = SNOOP_MGMD_DEFAULT_RBST_VAR;
        }
        else
        {
          qrv = SNOOP_MGMD_RBST_VAR_GET(mgmdMsg.sQRV);
        }
        snoopFPDecode(mgmdMsg.qqic, &shortVal);
        qqic = shortVal;
        if (!qqic)
        {
          qqic = SNOOP_MGMD_DEFAULT_QRY_INTRVL;
        }
        otherQuerierTimer = SNOOP_MGMD_OTHER_QRYR_INTRVL_GET(qrv, qqic, maxRespTime);
      }/* End of Version checks */
    }/* Endof MLD Check */
  }
  else
  {
    otherQuerierTimer = 
    pSnoopCB->snoopCfgData->snoopQuerierCfgData.snoopQuerierExpiryInterval ;
  }/* End of querier state check */

  if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.querierExpiryTimer == L7_NULL)
  {
    if (snoopQuerierTimerStart(pSnoopOperEntry, SNOOP_QUERIER_QUERIER_EXPIRY_TIMER, 
                               otherQuerierTimer) != L7_SUCCESS)
    {
      LOG_MSG("snoopQuerierQueryProcess: Failed to start Querier Expiry Timer for vlan %d family %d\n",
              pSnoopOperEntry->vlanId, pSnoopCB->family);
    }
  }
  else
  {
    if (snoopQuerierTimerUpdate(pSnoopOperEntry, SNOOP_QUERIER_QUERIER_EXPIRY_TIMER,
                                otherQuerierTimer) != L7_SUCCESS)
    {
      LOG_MSG("snoopQuerierQueryProcess: Failed to update Querier Expiry Timer for vlan %d family %d\n",
              pSnoopOperEntry->vlanId, pSnoopCB->family);
    }
  }
  return L7_SUCCESS;
}

/**************************************************************************
* @purpose  Process the received query from a pottential querier
*           in the same VLAN
*
* @param    pSnoopOperEntry  @b{(input)} Snooping operational entry
* @param    mcastPacket      @b{(input)} Received Frame
* @param    mgmdMsg          @b{(input)} Received query message
* @param    incomingVersion  @b{(input)} Received query version
*
* @returns  L7_FAILURE
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
****************************************************************************/
L7_RC_t snoopQuerierOtherQuerierDetected(snoopOperData_t  *pSnoopOperEntry,
                                         mgmdSnoopControlPkt_t *mcastPacket,
                                         L7_mgmdQueryMsg_t *mgmdMsg,
                                         L7_uint32 incomingVersion)
{
  L7_RC_t           rc = L7_SUCCESS;
  snoop_cb_t       *pSnoopCB;
  L7_uint32         vlanId, electionMode;
  L7_int32          diff;
  L7_inet_addr_t    snoopQuerierddr;

  pSnoopCB = mcastPacket->cbHandle;
  vlanId = mcastPacket->vlanId;

  /* Get the Election participation mode */
  electionMode = (pSnoopCB->snoopCfgData->snoopVlanCfgData[mcastPacket->vlanId].snoopVlanQuerierCfgData.snoopQuerierVlanAdminMode
                   & SNOOP_QUERIER_ELECTION_PARTICIPATE);

  if (electionMode)
  {
    if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperVersion 
        > incomingVersion)
    {
      /* Down grade snoop queriers version. If election mode disable. Go to
         non-querier mode and wait. If election mode enable, go ahead and
         send a general query to start election */
      pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperVersion = incomingVersion;
      if (electionMode)
      {
        snoopQuerierSMProcessEvent(pSnoopOperEntry, snoopQuerierVersionConflict, pSnoopCB);
      }
    }
    else if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperVersion 
             < incomingVersion)
    {
      /* Wait till other querier down grades the version. 
         If possible upgrade to the maximum possible extent. */
      if (incomingVersion <= pSnoopCB->snoopCfgData->snoopQuerierCfgData.snoopQuerierVersion)
      {
        pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperVersion = incomingVersion;
      }
      snoopQuerierSMProcessEvent(pSnoopOperEntry, snoopQuerierVersionConflict, pSnoopCB);
    }
    else /* Snooping Querier and other querier(s) versions have converged */
    {
      /* Check if we have a valid address */
      if (snoopQuerierAddressReady(vlanId, pSnoopCB, &snoopQuerierddr) != L7_TRUE)
      {
        snoopQuerierSMProcessEvent(pSnoopOperEntry, snoopQuerierDisable, pSnoopCB);
        return L7_SUCCESS; /* This is a valid scenario where in querier address
                              was modified */
      }

      diff = L7_INET_ADDR_COMPARE(&snoopQuerierddr, &mcastPacket->srcAddr);
      if (diff == 0)
      {
        /* This indicates that a loop is present in the network */
        snoopQuerierSMProcessEvent(pSnoopOperEntry, snoopQuerierRxInferiorQryMoveToQ, pSnoopCB);
      }
      else if (diff < 0)
      {
        /* My address < detected querier Address. I am better than true querier */
        snoopQuerierSMProcessEvent(pSnoopOperEntry, snoopQuerierRxInferiorQryMoveToQ, pSnoopCB);
      }
      else
      {
        /* My address > detected querier Address. The other querier is better */
        snoopQuerierSMProcessEvent(pSnoopOperEntry, snoopQuerierRxSuperiorQryMoveToNQ, pSnoopCB);
      }
    } /* End of version conflict check */
  }
  else
  {
    /* I dont want to participate in querier election */
    snoopQuerierSMProcessEvent(pSnoopOperEntry, 
                               snoopQuerierRxSuperiorQryMoveToNQ, 
                               pSnoopCB);
      /* Update the local information with the received info */
     pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperVersion = incomingVersion;
  } /* End of querier election participate mode check */

  return rc;
}
/***************************************************************************
* @purpose  Process vlan add notification and reset the querier information
*
* @param    vlanId  @b{(input)} VLAN ID of the newly created vlan
*
* @returns  void
*
* @notes    none
*
* @end
****************************************************************************/
void snoopQuerierVlanAdd(L7_uint32 vlanId)
{
  snoop_cb_t      *pSnoopCB, *pSnoopFirstCB;
  L7_uint32        maxInstances, cbIndex;
  snoopOperData_t *pSnoopOperEntry = L7_NULLPTR;

  maxInstances = maxSnoopInstancesGet();
  pSnoopFirstCB = snoopCBFirstGet();

  for (cbIndex = 0; cbIndex < maxInstances; cbIndex++)
  {
    pSnoopCB = pSnoopFirstCB + cbIndex;

    /* Initialize Timer Nodes */
    if ((pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_EXACT)) 
                         == L7_NULLPTR)
    {
      LOG_MSG("snoopQuerierVlanAdd: Invalid vlanId\n");
      return;
    }

    pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperVersion = 0;
    pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperState = SNOOP_QUERIER_DISABLED;
    inetAddressZeroSet(pSnoopCB->family, 
                       &pSnoopOperEntry->snoopQuerierInfo.querierAddress);
    pSnoopOperEntry->snoopQuerierInfo.querierVersion = 0;

    snoopQuerierVlanModeApply(vlanId, L7_ENABLE, pSnoopCB);
  }/* End of snoop instances loop */
}
/***************************************************************************
* @purpose  Process vlan delete notification 
*
* @param    vlanId  @b{(input)} VLAN ID of the deleted vlan
*
* @returns  void
*
* @notes    none
*
* @end
****************************************************************************/
void snoopQuerierVlanDelete(L7_uint32 vlanId)
{
  snoop_cb_t *pSnoopCB, *pSnoopFirstCB;
  L7_uint32   maxInstances, cbIndex;

  maxInstances = maxSnoopInstancesGet();
  pSnoopFirstCB = snoopCBFirstGet();

  for (cbIndex = 0; cbIndex < maxInstances; cbIndex++)
  {
    pSnoopCB = pSnoopFirstCB + cbIndex;
    snoopQuerierVlanModeApply(vlanId, L7_DISABLE, pSnoopCB);
  }/* End of snoop instances loop */
}
/*********************************************************************
* @purpose  Send start event to the Querier State Machine
*
* @param    pSnoopOperEntry  @b{(input)} Snooping operational entry
* @param    pSnoopCB         @b{(input)} Control Blcok
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void snoopQuerierStart(snoopOperData_t *pSnoopOperEntry, snoop_cb_t *pSnoopCB)
{
  if (pSnoopCB->snoopCfgData->snoopVlanCfgData[pSnoopOperEntry->vlanId].snoopVlanQuerierCfgData.snoopQuerierVlanAdminMode & SNOOP_QUERIER_ELECTION_PARTICIPATE)
  {
    snoopQuerierSMProcessEvent(pSnoopOperEntry, snoopQuerierBegin, pSnoopCB);
  }
  else
  {
    snoopQuerierSMProcessEvent(pSnoopOperEntry, snoopQuerierBeginNQ, pSnoopCB);
    if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.querierExpiryTimer == L7_NULL)
    {
      (void)snoopQuerierTimerStart(pSnoopOperEntry, SNOOP_QUERIER_QUERIER_EXPIRY_TIMER,
                                   pSnoopCB->snoopCfgData->snoopQuerierCfgData.snoopQuerierExpiryInterval);
    }
  }/* End of querier vlan election participate mode check */
}
/*********************************************************************
* @purpose  Send a General query on specified interface and vlan
*
* @param    intIfNum  @b{(input)} Internal interface number
* @param    vlanId    @b{(input)} VLAN ID
* @param    pSnoopCB  @b{(input)} Control Block
* @param    qryType   @b{(input)} Query type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes TCN Queries & Leave processing queries are sent only to
*        non-router attached interfaces. Pediodic queries are sent to
*        all the active interface.
*
* @end
*
*********************************************************************/
L7_RC_t snoopGeneralQuerySend(L7_uint32 intIfNum, L7_uint32 vlanId,
                              snoop_cb_t *pSnoopCB, snoopQueryType_t qryType)
{
  L7_netBufHandle  bufHandle;
  L7_uchar8       *dataStart, type;
  L7_INTF_TYPES_t    sysIntfType;   
  L7_uint32        ipv4Addr, version, frameLength = 0;
  L7_RC_t          rc = L7_SUCCESS;
  L7_inet_addr_t   destIp, groupAddr;
  L7_in6_addr_t    ipv6Addr;
  snoopOperData_t *pSnoopOperEntry  = L7_NULLPTR;
  mgmdSnoopControlPkt_t mcastPacket;

  /* Make sure this port has not been enabled for routing, is not the mirror 
     dest port, is not a LAG member and is active.
  */
  if (snoopIntfCanBeEnabled(intIfNum, vlanId) != L7_TRUE)
  {
    return L7_SUCCESS;
  }

  /* If outgoing interface is CPU interface, don't send it */
  if ( (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS) &&
       (sysIntfType == L7_CPU_INTF) )
  {
    return L7_SUCCESS;
  }

  if ((pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_EXACT)) 
                       == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  version = pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperVersion;
  if (version == 0)
  { /* Do not send queries if a true querier is not detected
       or if we are not the querier */
    return L7_SUCCESS;
  }

  SYSAPI_NET_MBUF_GET(bufHandle);
  if (bufHandle == L7_NULL)
  {
    return L7_FAILURE;
  }

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, dataStart);
  if (pSnoopCB->family == L7_AF_INET)
  {
    memset((void *)dataStart, 0x00, IGMP_FRAME_SIZE);
    ipv4Addr = L7_IP_ALL_HOSTS_ADDR;
    inetAddressSet(L7_AF_INET, &ipv4Addr, &destIp);
    type = L7_IGMP_MEMBERSHIP_QUERY;
  }
  else
  {
    memset((void *)dataStart, 0x00, MLD_FRAME_SIZE);
    memset(&ipv6Addr, 0x00, sizeof(L7_in6_addr_t));
    osapiInetPton(L7_AF_INET6, SNOOP_IP6_ALL_HOSTS_ADDR, (L7_uchar8 *)&ipv6Addr);

     /* MLD */
    inetAddressSet(L7_AF_INET6, &ipv6Addr, &destIp);
    type = L7_MLD_MEMBERSHIP_QUERY;
  }

  if (pSnoopCB->family == L7_AF_INET)
  {
    ipv4Addr = 0;
    inetAddressSet(L7_AF_INET, &ipv4Addr, &groupAddr);
    rc = snoopIGMPFrameBuild(intIfNum,
                             &destIp,       /* 224.0.0.1 */
                              type,         /* 0x11 or 130*/
                             &groupAddr,    /* Group address for General query */
                              dataStart,
                              pSnoopCB,
                              version,
                              pSnoopOperEntry);
  }
  else
  {
    memset(&ipv6Addr, 0x00, sizeof(L7_in6_addr_t));
    inetAddressSet(L7_AF_INET6, &ipv6Addr, &groupAddr);
    rc = snoopMLDFrameBuild(intIfNum,
                             &destIp,     /* FF02::01 */
                              type,       /* 130 */
                             &groupAddr,  /* Group address for General query :: */
                              dataStart,
                              pSnoopCB,
                              version,
                              pSnoopOperEntry);
  }

  if (rc == L7_SUCCESS)
  {
    /* General Query will be sent on all active non-router ports */

    /* Search for non-multicast router interfaces */
    if (!(L7_INTF_ISMASKBITSET(pSnoopOperEntry->mcastRtrAttached, intIfNum))
        || qryType == SNOOP_PERIODIC_QUERY)
    {
      if (pSnoopCB->family == L7_AF_INET)
      {
        if (version == SNOOP_IGMP_VERSION_3)
        {
          frameLength = IGMPv3_FRAME_SIZE;
        }
        else
        {
          frameLength = IGMP_FRAME_SIZE;
        }
      }
      else
      {
        if (version == SNOOP_MLD_VERSION_2)
        {
          frameLength = MLDv2_FRAME_SIZE;
          if (pSnoopCB->snoopExec->ipv6OptionsSupport == L7_TRUE)
          {
            frameLength += SNOOP_IP6_RTR_ALERT_IPPROTO_HOPOPTS_LEN;
          }
        }
        else
        {
          frameLength = MLD_FRAME_SIZE;
          if (pSnoopCB->snoopExec->ipv6OptionsSupport == L7_TRUE)
          {
            frameLength += SNOOP_IP6_RTR_ALERT_IPPROTO_HOPOPTS_LEN;
          }
        }
        
        /* MLD Frame size */
      }
      SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, frameLength);
      snoopDebugPacketTxTrace(intIfNum, vlanId, dataStart, pSnoopCB->family);
#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
      snoopBuildInsertInnerTagFrameTransmit(intIfNum, vlanId, bufHandle);
#else
      // PTin added
      mcastPacket.cbHandle    = pSnoopCB;
      mcastPacket.intIfNum    = 0;
      mcastPacket.vlanId      = vlanId;
      mcastPacket.innerVlanId = 0;
      mcastPacket.length      = frameLength;
      memcpy(mcastPacket.payLoad,dataStart,frameLength);
      snoopPacketClientIntfsForward(&mcastPacket,SNOOP_IGMP_QUERY);
      //snoopFrameTransmit(intIfNum, vlanId, bufHandle, SNOOP_VLAN_INTF_SEND);
#endif
    }
    else
    {
      SYSAPI_NET_MBUF_FREE(bufHandle);
    }
  }
  else
  {
    SYSAPI_NET_MBUF_FREE(bufHandle);
  }

  return rc;
}
/*********************************************************************
* @purpose  Build IGMP Message
*
* @param    intIfNum         @b{(input)}  Internal interface number
* @param    destIp           @b{(input)}  Destination IP Address
* @param    type             @b{(input)}  IGMP Packet type
* @param    groupAddr        @b{(input)}  IGMP Group address
* @param    buffer           @b{(output)} Buffer to hold the packet
* @param    pSnoopCB         @b{(input)}  Control Block
* @param    version          @b{(input)}  IGMP Version
* @param    pSnoopOperEntry  @b{(input)}  Operational data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t snoopIGMPFrameBuild(L7_uint32        intIfNum,
                            L7_inet_addr_t  *destIp,
                            L7_uchar8        type,
                            L7_inet_addr_t  *groupAddr,
                            L7_uchar8       *buffer,
                            snoop_cb_t      *pSnoopCB,
                            L7_uint32        version,
                            snoopOperData_t *pSnoopOperEntry)
{
  L7_uchar8      *dataPtr, *tempPtr, *startPtr;
  L7_uchar8       baseMac[L7_MAC_ADDR_LEN], byteVal;
  L7_uchar8       destMac[L7_MAC_ADDR_LEN];
  L7_ushort16     shortVal;
  static L7_ushort16 iph_ident = 1;
  L7_uint32       ipv4Addr, vlanId, val;
  L7_inet_addr_t  querierAddr;

  dataPtr = buffer;

  /* Validate the group address being reported */
  snoopMulticastMacFromIpAddr(destIp, destMac);
  /* Validate MAC address */
  if (snoopMacAddrCheck(destMac, pSnoopCB->family) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Get base MAC address (could be BIA or LAA) and use it as src MAC */
  if (simGetSystemIPMacType() == L7_SYSMAC_BIA)
  {
    simGetSystemIPBurnedInMac(baseMac);
  }
  else
  {
    simGetSystemIPLocalAdminMac(baseMac);
  }

  /* Set source and dest MAC in ethernet header */
  SNOOP_PUT_DATA(destMac, L7_MAC_ADDR_LEN, dataPtr);
  SNOOP_PUT_DATA(baseMac, L7_MAC_ADDR_LEN, dataPtr);

  
  /* IP Ether type */
  shortVal = L7_ETYPE_IP;
  SNOOP_PUT_SHORT(shortVal, dataPtr);
  
  /* Start of IP Header */
  startPtr = dataPtr; 
  /* IP Version */
  byteVal    = (L7_IP_VERSION<<4) | L7_IP_HDR_VER_LEN;
  SNOOP_PUT_BYTE(byteVal, dataPtr);

  /* TOS */
  byteVal     = SNOOP_IP_TOS;
  SNOOP_PUT_BYTE(byteVal, dataPtr);
  /* Payload Length */
  if (version > SNOOP_IGMP_VERSION_2)
  {
    shortVal = L7_IP_HDR_LEN + SNOOP_IGMPV3_HEADER_MIN_LENGTH;
  }
  else
  {
    shortVal = L7_IP_HDR_LEN + SNOOP_IGMPv1v2_HEADER_LENGTH;
  }
  SNOOP_PUT_SHORT(shortVal, dataPtr);

  /* Identified */
  shortVal = iph_ident++;
  SNOOP_PUT_SHORT(shortVal, dataPtr);

  /* Fragment flags */
  shortVal = 0;
  SNOOP_PUT_SHORT(shortVal, dataPtr);
  /* TTL */
  byteVal = SNOOP_IP_TTL;
  SNOOP_PUT_BYTE(byteVal, dataPtr);
  /* Protocol */
  byteVal = IGMP_PROT;
  SNOOP_PUT_BYTE(byteVal, dataPtr);
  /* Checksum = 0*/
  shortVal = 0;
  tempPtr = dataPtr;
  SNOOP_PUT_SHORT(shortVal, dataPtr);
  /* Source Address - Snooping Switches send it with null source address */
  vlanId = pSnoopOperEntry->vlanId;

  if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperState ==
                                        SNOOP_QUERIER_QUERIER)
  {
    if (snoopQuerierAddressReady(vlanId, pSnoopCB, &querierAddr) == L7_FALSE)
    {
      /* Invalid state - Disable querier operationally */
      snoopQuerierSMProcessEvent(pSnoopOperEntry, snoopQuerierDisable, pSnoopCB);
      inetAddressZeroSet(L7_AF_INET, &querierAddr);
    }
  }
  else
  {
    /* When in non-querier mode if we have to send out a general query
       send it with a null source IP */
    inetAddressZeroSet(L7_AF_INET, &querierAddr);
  }

  inetAddressGet(L7_AF_INET, &querierAddr, &ipv4Addr);
  memcpy(dataPtr, &ipv4Addr, L7_IP_ADDR_LEN); dataPtr += L7_IP_ADDR_LEN;
  /* Destination Address */
  inetAddressGet(L7_AF_INET, destIp, &ipv4Addr);
  memcpy(dataPtr, &ipv4Addr, L7_IP_ADDR_LEN); dataPtr += L7_IP_ADDR_LEN;

  shortVal = snoopCheckSum((L7_ushort16 *)startPtr, L7_IP_HDR_LEN, 0);
  SNOOP_PUT_SHORT(shortVal, tempPtr); 
  SNOOP_UNUSED_PARAM(tempPtr);
  /* End of IP Header */

  /* Start IGMP Header */
  startPtr = dataPtr;
  /* IGMP Type */
  byteVal = type;
  SNOOP_PUT_BYTE(byteVal, dataPtr);
  /* Max response code */
  if (version >= SNOOP_IGMP_VERSION_2)
  {
    if (version == SNOOP_IGMP_VERSION_2 && pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperState
        == SNOOP_QUERIER_QUERIER)
    {
      val = snoopCheckPrecedenceParamGet(vlanId, intIfNum, 
                                         SNOOP_PARAM_MAX_RESPONSE_TIME, 
                                         pSnoopCB->family);
      val *= SNOOP_IGMP_FP_DIVISOR;
        /* Check for byteVal overflow */
      if (val >= (1 << (sizeof(byteVal) * 8)))
      {
        byteVal = 0xff;
      }
      else
      {
        byteVal = val;
      }

    }
    else
    {
      byteVal = pSnoopOperEntry->snoopQuerierInfo.maxResponseCode;
    }
  }
  else
  {
    byteVal = 0;
  }
  SNOOP_PUT_BYTE(byteVal, dataPtr);

  /* Checksum = 0*/
  shortVal = 0;
  tempPtr = dataPtr;
  SNOOP_PUT_SHORT(shortVal, dataPtr);

  /* Group Address */
  inetAddressGet(L7_AF_INET, groupAddr, &ipv4Addr);
  memcpy(dataPtr, &ipv4Addr, L7_IP_ADDR_LEN);
  dataPtr += L7_IP_ADDR_LEN;

  if (version == SNOOP_IGMP_VERSION_3)
  {
    /* QRV */
    byteVal = pSnoopOperEntry->snoopQuerierInfo.sFlagQRV;
    SNOOP_PUT_BYTE(byteVal, dataPtr);
    /* QQIC */
    byteVal = pSnoopOperEntry->snoopQuerierInfo.qqic;
    SNOOP_PUT_BYTE(byteVal, dataPtr);
    shortVal = 0;
    SNOOP_PUT_SHORT(shortVal, dataPtr);
    SNOOP_UNUSED_PARAM(dataPtr);

    shortVal = snoopCheckSum((L7_ushort16 *)startPtr, SNOOP_IGMPV3_HEADER_MIN_LENGTH, 0);
    SNOOP_PUT_SHORT(shortVal, tempPtr);/* Copy the calculated checksum 
                                          to stored checksum ptr */
  }
  else
  {
    shortVal = snoopCheckSum((L7_ushort16 *)startPtr, SNOOP_IGMPv1v2_HEADER_LENGTH, 0);
    SNOOP_PUT_SHORT(shortVal, tempPtr); /* Copy the calculated checksum 
                                          to stored checksum ptr */
  }
  SNOOP_UNUSED_PARAM(tempPtr);
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Build MLD Message
*
* @param    intIfNum         @b{(input)}  Internal interface number
* @param    destIp           @b{(input)}  Destination IPv6 Address
* @param    type             @b{(input)}  MLD Packet type
* @param    groupAddr        @b{(input)}  MLD Group address
* @param    buffer           @b{(output)} Buffer to hold the packet
* @param    pSnoopCB         @b{(input)}  Control Block
* @param    version          @b{(input)}  MLD Version
* @param    pSnoopOperEntry  @b{(input)}  Operational data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t snoopMLDFrameBuild(L7_uint32       intIfNum,
                            L7_inet_addr_t *destIp,
                            L7_uchar8       type,
                            L7_inet_addr_t *groupAddr,
                            L7_uchar8      *buffer,
                            snoop_cb_t     *pSnoopCB,
                            L7_uint32       version,
                            snoopOperData_t *pSnoopOperEntry)
{
  L7_uchar8      *dataPtr, *tempPtr, *startPtr;
  L7_uchar8       baseMac[L7_MAC_ADDR_LEN], byteVal;
  L7_uchar8       destMac[L7_MAC_ADDR_LEN];
  L7_ushort16     shortVal;
  L7_uint32       vlanId, uintVal;
  L7_inet_addr_t  querierAddr;
  L7_ip6Header_t  ipv6Header;
  mgmdIpv6PseudoHdr_t ipv6PseudoHdr;

  dataPtr = buffer;

  /* Validate the group address being reported */
  snoopMulticastMacFromIpAddr(destIp, destMac);
  /* Validate MAC address */
  if (snoopMacAddrCheck(destMac, pSnoopCB->family) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Get base MAC address (could be BIA or LAA) and use it as src MAC */
  if (simGetSystemIPMacType() == L7_SYSMAC_BIA)
  {
    simGetSystemIPBurnedInMac(baseMac);
  }
  else
  {
    simGetSystemIPLocalAdminMac(baseMac);
  }

  /* Set source and dest MAC in ethernet header */
  SNOOP_PUT_DATA(destMac, L7_MAC_ADDR_LEN, dataPtr);
  SNOOP_PUT_DATA(baseMac, L7_MAC_ADDR_LEN, dataPtr);
  
  
  /* IPv6 Ether type */
  shortVal = L7_ETYPE_IPV6;
  SNOOP_PUT_SHORT(shortVal, dataPtr);

  /* Start of IPv6 Header */
  startPtr = dataPtr; 
  memset(&ipv6Header, 0x00, sizeof(L7_ip6Header_t));

  /* IP Version 6 */
  ipv6Header.ver_class_flow = (0x0006 << 28 );

  
  /* Payload Length */
  if (version == SNOOP_MLD_VERSION_1)
  {
    ipv6Header.paylen = SNOOP_MLDV1_HEADER_LENGTH;
  }
  else if (version == SNOOP_MLD_VERSION_2)
  {
    ipv6Header.paylen = SNOOP_MLDV2_HEADER_MIN_LENGTH;
  }

  /* Next header - ICMPv6 as MLD packets are a type of ICMPv6 packets */
  ipv6Header.next = IP_PROT_ICMPV6;

  /* Hop Limit */
  ipv6Header.hoplim = SNOOP_IP6_HOP_LIMIT;

  /* Destination IP Address */
   osapiInetPton(L7_AF_INET6, SNOOP_IP6_ALL_HOSTS_ADDR, ipv6Header.dst);

  /* Source Address - Snooping Switches send it with null source address */
  vlanId = pSnoopOperEntry->vlanId;

  if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperState == 
                                        SNOOP_QUERIER_QUERIER)
  {
    if (snoopQuerierAddressReady(vlanId, pSnoopCB, &querierAddr) == L7_FALSE)
    {
      /* Invalid state - Disable querier operationally */
      snoopQuerierSMProcessEvent(pSnoopOperEntry, snoopQuerierDisable, pSnoopCB);
      inetAddressZeroSet(L7_AF_INET6, &querierAddr);
    }
  }
  else
  {
    /* When in non-querier mode if we have to send out a general query
       send it with a null source IP */
    inetAddressZeroSet(L7_AF_INET6, &querierAddr);
  }

  inetAddressGet(L7_AF_INET6, &querierAddr, ipv6Header.src);

  /* Construct ipv6 pseudo header to calculate icmpv6 checksum field */ 
  memset(&ipv6PseudoHdr, 0x00, sizeof(mgmdIpv6PseudoHdr_t));
  memcpy(&(ipv6PseudoHdr.srcAddr.in6.addr8), ipv6Header.src, sizeof(L7_in6_addr_t));
  memcpy(&(ipv6PseudoHdr.dstAddr.in6.addr8), ipv6Header.dst, sizeof(L7_in6_addr_t));
  ipv6PseudoHdr.dataLen = ipv6Header.paylen;

  ipv6PseudoHdr.zero[0] = 0;
  ipv6PseudoHdr.zero[1] = 0;
  ipv6PseudoHdr.zero[2] = 0;

  ipv6PseudoHdr.nxtHdr  = IP_PROT_ICMPV6;
  if (pSnoopCB->snoopExec->ipv6OptionsSupport == L7_TRUE)
  {
    /* Construct Hop by hop extension header with router alert option */
     ipv6Header.next = SNOOP_IP6_IPPROTO_HOPOPTS;
     ipv6Header.paylen += SNOOP_IP6_RTR_ALERT_IPPROTO_HOPOPTS_LEN;
  }
  /* End of IP Header */

  /* Start ICMPv6-MLD Header */
  dataPtr = &ipv6PseudoHdr.icmpv6pkt[0];
  /* MLD Type */
  byteVal = type;
  SNOOP_PUT_BYTE(byteVal, dataPtr);
  /* Code */
  byteVal = 0;
  SNOOP_PUT_BYTE(byteVal, dataPtr);

  /* Checksum = 0*/
  shortVal = 0;
  SNOOP_PUT_SHORT(shortVal, dataPtr);

  /* Max response code */
   if (version == SNOOP_MLD_VERSION_1 &&
       pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperState
       == SNOOP_QUERIER_QUERIER)
   { /* Querier is supported only for MLDv1 */
     uintVal = snoopCheckPrecedenceParamGet(vlanId, intIfNum, 
                                        SNOOP_PARAM_MAX_RESPONSE_TIME, 
                                        pSnoopCB->family);
     shortVal = uintVal * SNOOP_MLD_FP_DIVISOR;
   }
   else
   {
     shortVal = pSnoopOperEntry->snoopQuerierInfo.maxResponseCode;
   }
   SNOOP_PUT_SHORT(shortVal, dataPtr);
  
   /* Reserved = 0*/
   shortVal = 0;
   SNOOP_PUT_SHORT(shortVal, dataPtr);

  /* Group Address */
  inetAddressGet(L7_AF_INET6, groupAddr, dataPtr);
  dataPtr += L7_IP6_ADDR_LEN;

  if (version == SNOOP_MLD_VERSION_2)
  {
    /* QRV */
    byteVal = pSnoopOperEntry->snoopQuerierInfo.sFlagQRV;
    SNOOP_PUT_BYTE(byteVal, dataPtr);
    /* QQIC */
    byteVal = pSnoopOperEntry->snoopQuerierInfo.qqic;
    SNOOP_PUT_BYTE(byteVal, dataPtr);

    shortVal = 0;
    SNOOP_PUT_SHORT(shortVal, dataPtr);
    SNOOP_UNUSED_PARAM(dataPtr);
    shortVal = snoopCheckSum((L7_ushort16 *)(&ipv6PseudoHdr), 
                             L7_IP6_HEADER_LEN + SNOOP_MLDV2_HEADER_MIN_LENGTH, 0);
    tempPtr = &ipv6PseudoHdr.icmpv6pkt[2];
    SNOOP_PUT_SHORT(shortVal, tempPtr);/* Copy the calculated checksum 
                                          to stored checksum ptr */
  }
  else
  {
    tempPtr = &ipv6PseudoHdr.icmpv6pkt[2];
    shortVal = snoopCheckSum((L7_ushort16 *)(&ipv6PseudoHdr), 
                             L7_IP6_HEADER_LEN + SNOOP_MLDV1_HEADER_LENGTH, 0);
    SNOOP_PUT_SHORT(shortVal, tempPtr); /* Copy the calculated checksum 
                                           to stored checksum ptr */
  }
  SNOOP_UNUSED_PARAM(tempPtr);
  dataPtr = startPtr; /* Point to the start of IP header in the pkt */
  memcpy(dataPtr, &ipv6Header, sizeof(L7_ip6Header_t)); 
  dataPtr += sizeof(L7_ip6Header_t);
 if (pSnoopCB->snoopExec->ipv6OptionsSupport == L7_TRUE)
  {
     byteVal = IP_PROT_ICMPV6;
     SNOOP_PUT_BYTE(byteVal,dataPtr);
     byteVal = SNOOP_IP6_RTR_ALERT_IPPROTO_HOPOPTS_LEN_CODE;
     SNOOP_PUT_BYTE(byteVal,dataPtr);
    /* PAD of 2 Bytes */
     byteVal = SNOOP_IP6_IPPROTO_OPT_TYPE_PAD;
     SNOOP_PUT_BYTE(byteVal,dataPtr);
     byteVal = 0;
     SNOOP_PUT_BYTE(byteVal,dataPtr);
    /* Router Alert option - 2Bytes */
     byteVal = SNOOP_IP6_IPPROTO_OPT_TYPE_RTR_ALERT;
     SNOOP_PUT_BYTE(byteVal,dataPtr);
     byteVal = 2;
     SNOOP_PUT_BYTE(byteVal,dataPtr);
     shortVal = 0;
     SNOOP_PUT_SHORT(shortVal,dataPtr);
  }
  /* Copy the constructed MLD packet */
  memcpy(dataPtr, ipv6PseudoHdr.icmpv6pkt, 
         (version == SNOOP_MLD_VERSION_2)? SNOOP_MLDV2_HEADER_MIN_LENGTH : SNOOP_MLDV1_HEADER_LENGTH);

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Send peridic query on all the egress ports of querier 
*           enabled vlan
*
* @param    pSnoopOperEntry   @b{(input)} Snoop Operational data
*
* @returns  void
*
* @notes none
*
* @end
*
*********************************************************************/
void snoopQuerierPeriodicQuerySend(snoopOperData_t *pSnoopOperEntry)
{
  L7_netBufHandle  bufHandle;
  L7_uchar8       *dataStart, type;
  L7_uint32        ipv4Addr, version, frameLength = 0, vlanId;
  L7_RC_t          rc = L7_SUCCESS;
  L7_inet_addr_t   destIp, groupAddr;
  L7_in6_addr_t    ipv6Addr;
  snoop_cb_t      *pSnoopCB = L7_NULLPTR;
  mgmdSnoopControlPkt_t mcastPacket;

  pSnoopCB = pSnoopOperEntry->cbHandle;

  osapiSemaTake(pSnoopCB->snoopExec->snoopAvlTreeSema, L7_WAIT_FOREVER);

  vlanId   = pSnoopOperEntry->vlanId;
  version  = pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperVersion;
  if (version == 0 || version > 
      pSnoopCB->snoopCfgData->snoopQuerierCfgData.snoopQuerierVersion)
  { /* Check for a valid version */
    SNOOP_TRACE(SNOOP_DEBUG_QUERIER, pSnoopCB->family, 
                "Invalid version %d\n", version);
    osapiSemaGive(pSnoopCB->snoopExec->snoopAvlTreeSema);
    return;
  }

  SYSAPI_NET_MBUF_GET(bufHandle);
  if (bufHandle == L7_NULL)
  {
    osapiSemaGive(pSnoopCB->snoopExec->snoopAvlTreeSema);
    return;
  }

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, dataStart);
  if (pSnoopCB->family == L7_AF_INET)
  {
    memset((void *)dataStart, 0x00, IGMP_FRAME_SIZE);
    ipv4Addr = L7_IP_ALL_HOSTS_ADDR;
    inetAddressSet(L7_AF_INET, &ipv4Addr, &destIp);
    type = L7_IGMP_MEMBERSHIP_QUERY;
  }
  else
  {
    memset((void *)dataStart, 0x00, MLD_FRAME_SIZE);
    memset(&ipv6Addr, 0x00, sizeof(L7_in6_addr_t));

    osapiInetPton(L7_AF_INET6, SNOOP_IP6_ALL_HOSTS_ADDR, (L7_uchar8 *)&ipv6Addr);

     /* MLD */
    inetAddressSet(L7_AF_INET6, &ipv6Addr, &destIp);
    type = L7_MLD_MEMBERSHIP_QUERY;
  }

  if (pSnoopCB->family == L7_AF_INET)
  {
    ipv4Addr = 0;
    inetAddressSet(L7_AF_INET, &ipv4Addr, &groupAddr);
    rc = snoopIGMPFrameBuild(L7_NULL,
                             &destIp,       /* 224.0.0.1 */
                              type,         /* 0x11 or 130*/
                             &groupAddr,    /* Group address for General query */
                              dataStart,
                              pSnoopCB,
                              version,
                              pSnoopOperEntry);
  }
  else
  {
    memset(&ipv6Addr, 0x00, sizeof(L7_in6_addr_t));
    inetAddressSet(L7_AF_INET6, &ipv6Addr, &groupAddr);
    rc = snoopMLDFrameBuild(L7_NULL,
                             &destIp,     /* FF02::01 */
                              type,       /* 130 */
                             &groupAddr,  /* Group address for General query :: */
                              dataStart,
                              pSnoopCB,
                              version,
                              pSnoopOperEntry);
  }

  if (rc == L7_SUCCESS)
  {
    /* Check if in querier mode */
    if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperState != 
                                                         SNOOP_QUERIER_QUERIER)
    {
      SYSAPI_NET_MBUF_FREE(bufHandle);
      osapiSemaGive(pSnoopCB->snoopExec->snoopAvlTreeSema);
      return;
    }

    /* General Query will be sent on all active ports of the vlan*/
    if (pSnoopCB->family == L7_AF_INET)
    {
      if (version == SNOOP_IGMP_VERSION_3)
      {
        frameLength = IGMPv3_FRAME_SIZE;
      }
      else
      {
        frameLength = IGMP_FRAME_SIZE;
      }
    }
    else
    {
      if (version == SNOOP_MLD_VERSION_2)
      {
        frameLength = MLDv2_FRAME_SIZE;
        if (pSnoopCB->snoopExec->ipv6OptionsSupport == L7_TRUE)
        {
          frameLength += SNOOP_IP6_RTR_ALERT_IPPROTO_HOPOPTS_LEN;
        }
      }
      else
      {
        frameLength = MLD_FRAME_SIZE;
        if (pSnoopCB->snoopExec->ipv6OptionsSupport == L7_TRUE)
        {
          frameLength += SNOOP_IP6_RTR_ALERT_IPPROTO_HOPOPTS_LEN;
        }
      }

      /* MLD Frame size */
    }

    SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, frameLength);
    snoopDebugPacketTxTrace(L7_NULL, vlanId, dataStart, pSnoopCB->family);
#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
      snoopPacketFloodInsertInnerTag(vlanId, bufHandle);
      SYSAPI_NET_MBUF_FREE(bufHandle);
#else
    // PTin added
    mcastPacket.cbHandle    = pSnoopCB;
    mcastPacket.intIfNum    = L7_NULL;
    mcastPacket.vlanId      = vlanId;
    mcastPacket.innerVlanId = 0;
    mcastPacket.length      = frameLength;
    memcpy(mcastPacket.payLoad,dataStart,frameLength);
    rc=snoopPacketClientIntfsForward(&mcastPacket,SNOOP_IGMP_QUERY);

    LOG_TRACE(LOG_CTX_PTIN_IGMP,"General Query transmitted, VLAN=%u (result=%d)\n",vlanId, rc);

    SYSAPI_NET_MBUF_FREE(bufHandle);      // PTin added
    //snoopFrameTransmit(L7_NULL, vlanId, bufHandle, SNOOP_VLAN_SEND);
#endif
  }
  else
  {
    SYSAPI_NET_MBUF_FREE(bufHandle);
  }
  osapiSemaGive(pSnoopCB->snoopExec->snoopAvlTreeSema);
}

