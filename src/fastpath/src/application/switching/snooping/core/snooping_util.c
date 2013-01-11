/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   snooping_util.c
*
* @purpose    Contains definitions to snooping utility routines
*
* @component  Snooping
*
* @comments   none
*
* @create     07-Dec-2006
*
* @author     drajendra
*
* @end
*
**********************************************************************/
#include "datatypes.h"
#include "osapi_support.h"
#include "sysnet_api_ipv4.h"
#include "sysnet_api_ipv6.h"
#include "l7utils_inet_addr_api.h"
#include "simapi.h"

#ifdef L7_DVLAN_PACKAGE
#include "dvlantag_api.h"
#endif

#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
#include "dot1ad_api.h"
#include "dot1ad_l2tp_api.h"
#endif

#include "snooping.h"
#include "snooping_util.h"
#include "snooping_api.h"
#include "snooping_outcalls.h"
#include "snooping_proto.h"
#include "snooping_db.h"
#include "snooping_debug.h"

#ifdef L7_NSF_PACKAGE
#include "snooping_ckpt.h"
#endif /* L7_NSF_PACKAGE */

/* PTin added: IGMP snooping */
#if 1
#include "ptin_evc.h"
#include "ptin_igmp.h"
#include "logger.h"
#include "ptin_debug.h"
#endif

/*********************************************************************
* @purpose  Find if a VLAN is operationaly enabled for snooping
*
*
* @param    destMac       @b{(input)}  Multicast MAC Address of group
*                                      entry to be modified
* @param    vlanId        @b{(input)}  VLAN Id of incoming packet
* @param    l2BitMask     @b{(output)} Intended IPMC entry L2 port bitmap
* @param    family    @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                  L7_AF_INET6 => MLD Snooping
*
* @returns  L7_TRUE   Atleast one interface in the VLAN snooping is
*                     operational
*           L7_FALSE
*
* @notes
*         Find all VLAN member interfaces on which snooping is operationally enabled.
*         Even if one interface in the VLAN is enabled return L7_TRUE
*
* @end
*********************************************************************/
L7_BOOL snoopVlanOperationalStateGet(L7_uint32 vlanId, L7_uchar8 family)
{
  snoopOperData_t   *pSnoopOperEntry;
  snoop_cb_t        *pSnoopCB;
  L7_BOOL            result = L7_FALSE;

  if (!(pSnoopCB = snoopCBGet(family)))
  {
    return L7_FALSE;
  }

  if ((pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_EXACT)) == L7_NULLPTR)
  {
    return L7_FALSE;
  }

  L7_INTF_NONZEROMASK(pSnoopOperEntry->snoopIntfMode, result);

  return result;
}

#ifdef L7_MCAST_PACKAGE
/*********************************************************************
* @purpose  Fine tunes the L2 Bitmap of  L3 IPMC table when snooping 
*           is enabled/disabled on a VLAN or mcast rtr is detected
*           on the VLAN
*
* @param    vlanId   @b{(input)} VLAN Id on which snooping operational
*                                mode changed
* @param    snoopVlanOperState  @b{(input)} New snooping operational mode for
*                                (vlanId, intIfNum) pair
*
* @returns  L7_SUCCESS
*
* @notes    
*
* @end
*********************************************************************/
void snoopVlanNotifyL3Mcast(L7_uint32 vlanId, L7_BOOL snoopVlanOperState)
{
  snoopL3InfoData_t    *snoopL3Entry;
  snoopL3InfoDataKey_t  key;
  snoop_eb_t           *pSnoopEB;
  L7_uchar8             mcastMacAddr[L7_MAC_ADDR_LEN];
  L7_uint32             rtrIfNum;
  L7_INTF_MASK_t        l2BitMask;
  snoopOperData_t      *pSnoopIGMPOperEntry = L7_NULLPTR, *pSnoopMLDOperEntry = L7_NULLPTR;
  snoop_cb_t           *pIGMPSnoopCB, *pMLDSnoopCB;
  snoopInfoData_t      *pSnoopEntry;

  pSnoopEB = snoopEBGet();
  pIGMPSnoopCB = snoopCBGet(L7_AF_INET);
  pMLDSnoopCB = snoopCBGet(L7_AF_INET6); 
 if (dot1qVlanIntfVlanIdToIntIfNum(vlanId, &rtrIfNum) != L7_SUCCESS)
 {
   return;
 }

  if ( pIGMPSnoopCB == L7_NULLPTR ||
      (pSnoopIGMPOperEntry = snoopOperEntryGet(vlanId, pIGMPSnoopCB, L7_MATCH_EXACT)) == L7_NULLPTR)
  {
    return;
  }

  if ( pMLDSnoopCB == L7_NULLPTR ||
      (pSnoopMLDOperEntry = snoopOperEntryGet(vlanId, pMLDSnoopCB, L7_MATCH_EXACT)) == L7_NULLPTR)
  {
    return;
  }

  memset((void *)&key, 0x00, sizeof(snoopL3InfoDataKey_t));
  snoopL3Entry = avlSearchLVL7(&pSnoopEB->snoopL3AvlTree, &key, AVL_NEXT);
  while(snoopL3Entry)
  {
    if (snoopL3Entry->srcIntfVlan && 
        snoopL3Entry->srcIntfVlan == vlanId)

    {
      snoopMulticastMacFromIpAddr(&snoopL3Entry->snoopL3InfoDataKey.mcastGroupAddr, 
                                  mcastMacAddr);

      if (snoopL3Entry->snoopL3InfoDataKey.mcastGroupAddr.family == L7_AF_INET)
      {
        memcpy(&l2BitMask, &pSnoopIGMPOperEntry->mcastRtrAttached, sizeof(l2BitMask));
      }
      else
      {
        memcpy(&l2BitMask, &pSnoopMLDOperEntry->mcastRtrAttached, sizeof(l2BitMask));
      }

      /* Check if any join learnt entries are present */
      if ((pSnoopEntry = snoopEntryFind(mcastMacAddr, vlanId, 
                                        snoopL3Entry->snoopL3InfoDataKey.mcastGroupAddr.family,
                                        L7_MATCH_EXACT)) != L7_NULLPTR)
      {
        L7_INTF_MASKOREQ(l2BitMask, pSnoopEntry->snoopGrpMemberList);
      }

      dtlSnoopingL3Notify(mcastMacAddr, L7_TRUE, rtrIfNum, 
                          &l2BitMask, snoopVlanOperState);
    }
    memcpy(key.macAddrSuffix, snoopL3Entry->snoopL3InfoDataKey.macAddrSuffix,
           SNOOP_MAC_ADDR_SUFFIX_LEN);
    memcpy(&key.mcastGroupAddr, &snoopL3Entry->snoopL3InfoDataKey.mcastGroupAddr,
           sizeof(L7_inet_addr_t));
    memcpy(&key.mcastSrcAddr, &snoopL3Entry->snoopL3InfoDataKey.mcastSrcAddr,
           sizeof(L7_inet_addr_t));
    snoopL3Entry = avlSearchLVL7(&pSnoopEB->snoopL3AvlTree, &key, AVL_NEXT);
  }
}
/*********************************************************************
* @purpose  Fine tunes the L3 IPMC table with snooping learnt information
*
* @param    destMac       @b{(input)} Multicast MAC Address of group
*                                     entry to be modified
* @param    vlanId        @b{(input)} VLAN Id of incoming packet
* @param    outIntIfMask  @b{(input)} Interface list of the groups part
*                                     of VID-DMAC L2MC entry. This entry
*                                     is valid only vlanId is non-zero
*
* @returns  L7_SUCCESS
*
* @notes    Note that when vlanId is non-zero means there was an incoming
*           join or entry expiry in Snooping component. vlanId is indicates
*           zero, a new L3 Mcast entry has been reported.
*
* @end
*********************************************************************/
void snoopNotifyL3Mcast(L7_uchar8 *destMac, L7_uint32 vlanId,
                        L7_INTF_MASK_t *outIntIfMask)
{
  L7_uint32          uintVar, result;
  L7_uchar8          family;
  L7_VLAN_MASK_t     outVlanRtrIntfMask;
  L7_VLAN_MASK_t     inVlanRtrIntfMask;
  L7_INTF_MASK_t     l2BitMask;
  L7_BOOL            snoopVlanOperState = L7_FALSE;
  L7_INTF_MASK_t     snoopMemberList;
  snoopOperData_t   *pSnoopOperEntry;
  snoop_cb_t        *pSnoopCB;


  if (snoopMacAddrCheck(destMac, L7_AF_INET)== L7_SUCCESS)
  {
    family = L7_AF_INET;
  }
  else
  {
    family = L7_AF_INET6;
  }

  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return;
  }

  /* Event from Snooping */
  if (vlanId)
  {
    snoopVlanOperState = snoopVlanOperationalStateGet(vlanId, family);
    /* Notify a single upstream/downstream Vlan routing interface change */
    if ((result = snoopL3EntryInOutVlanMaskGet(destMac, &inVlanRtrIntfMask,
                                               &outVlanRtrIntfMask)) != L7_NULL)
    {
      /* downstream interface mask */
      if (result & SNOOP_L3_EGRESS_VLAN_MASK_PRESENT)
      {
        if (L7_VLAN_ISMASKBITSET(outVlanRtrIntfMask, vlanId))
        {
          if (dot1qVlanIntfVlanIdToIntIfNum(vlanId, &uintVar) == L7_SUCCESS)
          {
            dtlSnoopingL3Notify(destMac, L7_FALSE, uintVar, outIntIfMask, snoopVlanOperState);
          }
        }
      }
      /* upstream interface mask */
      if (result & SNOOP_L3_INGRESS_VLAN_MASK_PRESENT)
      {
        if (L7_VLAN_ISMASKBITSET(inVlanRtrIntfMask, vlanId))
        {
          if (dot1qVlanIntfVlanIdToIntIfNum(vlanId, &uintVar) == L7_SUCCESS)
          {
            dtlSnoopingL3Notify(destMac, L7_TRUE, uintVar, outIntIfMask, snoopVlanOperState);
          }
        }
      }/*  */
    }/* end of vlan mask get func */
  }
  else /* Event from L3 */
  {
    /* notify all the changes in the l3 entry */
    if ((result = snoopL3EntryInOutVlanMaskGet(destMac, &inVlanRtrIntfMask,
                                               &outVlanRtrIntfMask)) != L7_NULL)
    {
      while (dot1qVlanIntfNextVlanIntfGetNext(vlanId, &vlanId) == L7_SUCCESS)
      {
        /* downstream interface mask */
        if (result & SNOOP_L3_EGRESS_VLAN_MASK_PRESENT)
        {
          if (L7_VLAN_ISMASKBITSET(outVlanRtrIntfMask, vlanId))
          {
            if (dot1qVlanIntfVlanIdToIntIfNum(vlanId, &uintVar) == L7_SUCCESS)
            {
              snoopVlanOperState = snoopVlanOperationalStateGet(vlanId, family);
              if (snoopIntfListGet(destMac, vlanId, &snoopMemberList, family) == L7_SUCCESS)
              {
                dtlSnoopingL3Notify(destMac, L7_FALSE, uintVar, &snoopMemberList,
                                    snoopVlanOperState);
              }/* found a snoop entry */
            }
          }
        }

        /* upstream interface mask */
        if (result & SNOOP_L3_INGRESS_VLAN_MASK_PRESENT)
        {
          if (L7_VLAN_ISMASKBITSET(inVlanRtrIntfMask, vlanId))
          {
            if (dot1qVlanIntfVlanIdToIntIfNum(vlanId, &uintVar) == L7_SUCCESS)
            {
              snoopVlanOperState = snoopVlanOperationalStateGet(vlanId, family);
              if (snoopIntfListGet(destMac, vlanId, &snoopMemberList, family) == L7_SUCCESS)
              {
                dtlSnoopingL3Notify(destMac, L7_TRUE, uintVar, 
                                    &snoopMemberList, snoopVlanOperState);
              }/* found a snoop entry */
              else
              {
                /* Because this is fresh L3 notification, in case we need not add null L2PBMP entry 
                   as snooping is not operational */
                if (snoopVlanOperationalStateGet(vlanId, family) == L7_TRUE)
                {
                  memset(&l2BitMask, 0, sizeof(l2BitMask));
                  if ((pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_EXACT))) 
                  {
                    memcpy(&l2BitMask, &pSnoopOperEntry->mcastRtrAttached, sizeof(l2BitMask));
                  }
                  dtlSnoopingL3Notify(destMac, L7_TRUE, uintVar, &l2BitMask, snoopVlanOperState);
            }
              }
            }/* End of vlan routing interface check */
          }
        }
      }/* loop thru all vlans */
    }/* Find L3 entry */
  }
}
#endif /* L7_MCAST_PACKAGE */

/*********************************************************************
* @purpose  Applies the IGMP Snooping Admin mode
*
* @param    adminMode   @b{(input)} IGMP Snooping admin mode
*
* @returns  L7_SUCCESS
*
* @notes    This function assumes adminMode is valid
*
* @end
*********************************************************************/
L7_RC_t snoopIGMPAdminModeApply(L7_uint32 adminMode, L7_uint16 vlanId /* PTin added: IGMP snooping */)
{
  sysnetPduIntercept_t sysnetPduIntercept;

  sysnetPduIntercept.addressFamily   = L7_AF_INET;
  sysnetPduIntercept.hookId          = SYSNET_INET_RECV_IN;
  sysnetPduIntercept.hookPrecedence  = FD_SYSNET_HOOK_IGMP_SNOOPING_PRECEDENCE;
  sysnetPduIntercept.interceptFunc   = snoopIGMPPktIntercept;
  strcpy(sysnetPduIntercept.interceptFuncName, "snoopIGMPPktIntercept");

  if (adminMode == L7_ENABLE)
  {
    /* PTin modified: IGMP snooping */
    #if 0
    /* Register with sysnet */
    if (sysNetPduInterceptRegister(&sysnetPduIntercept) == L7_SUCCESS)
    {
      /* Tell driver to enable IGMP Snooping */
      (void)dtlSnoopingEnable(L7_AF_INET, vlanId);
    }
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopIGMPAdminModeApply: Failed to register to sysnet to intercept IGMP pkts");
      return L7_FAILURE;
    }
    #else
    /* Register with sysnet */
    if (vlanId==L7_NULL)
    {
      sysNetPduInterceptRegister(&sysnetPduIntercept);
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"sysNetPduInterceptRegister executed (mode=%u, vlanId=%u)",adminMode,vlanId);
    }
    /* Tell driver to enable IGMP Snooping */
    (void)dtlSnoopingEnable(L7_AF_INET, vlanId);          /* PTin modified: IGMP snooping */
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"dtlSnoopingEnable executed (mode=%u, vlanId=%u)",adminMode,vlanId);
    #endif
  }
  else if (adminMode == L7_DISABLE)
  {
    /* Tell driver to disable IGMP Snooping */
    (void)dtlSnoopingDisable(L7_AF_INET, vlanId);         /* PTin modified: IGMP snooping */
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"dtlSnoopingDisable executed (mode=%u, vlanId=%u)",adminMode,vlanId);
    /* Deregister with sysnet */
    if (vlanId==L7_NULL)                                  /* PTin added: IGMP snooping */
    {
      (void)sysNetPduInterceptDeregister(&sysnetPduIntercept);
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"sysNetPduInterceptDeregister executed (mode=%u, vlanId=%u)",adminMode,vlanId);
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Applies the MLD Snooping Admin mode
*
* @param    adminMode   @b{(input)} MLD Snooping admin mode
*
* @returns  L7_SUCCESS
*
* @notes    This function assumes adminMode is valid
*
* @end
*********************************************************************/
L7_RC_t snoopMLDAdminModeApply(L7_uint32 adminMode)
{
  sysnetPduIntercept_t sysnetPduIntercept;

  /*-----------------------------------------------------------------------*/
  /*    Register for SysNet packet intercept for IPv6 multicast packets    */
  /*-----------------------------------------------------------------------*/
  sysnetPduIntercept.addressFamily  = L7_AF_INET6;
  sysnetPduIntercept.hookId         = SYSNET_INET6_MCAST_IN;
  sysnetPduIntercept.hookPrecedence = FD_SYSNET_HOOK_IGMP_SNOOPING_PRECEDENCE;
  sysnetPduIntercept.interceptFunc  = snoopMLDPktIntercept;
  osapiStrncpy(sysnetPduIntercept.interceptFuncName, "snoopMLDPktIntercept",
               sizeof(sysnetPduIntercept.interceptFuncName));

  if (adminMode == L7_ENABLE)
  {
    /* Register with SYSNET for PDUs */
    if (sysNetPduInterceptRegister(&sysnetPduIntercept) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopMLDAdminModeApply :Failed to register to sysnet to intercept MLD pkts");
      return L7_FAILURE;
    }
    /* Tell driver to enable IGMP Snooping */
    dtlSnoopingEnable(L7_AF_INET6, L7_NULL);          /* PTin modified: IGMP snooping */
  }
  else if (adminMode == L7_DISABLE)
  {
    /* Tell driver to disable IGMP Snooping */
    dtlSnoopingDisable(L7_AF_INET6, L7_NULL);         /* PTin modified: IGMP snooping */

    /* Deregister with sysnet */
    (void)sysNetPduInterceptDeregister(&sysnetPduIntercept);
  }

  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  Add an interface to the mcast router list and to all
*           snooping entries of a snoop instance
*
* @param    vlanId   @b{(input)} Vlan ID for the MAC Address
* @param    intIfNum @b{(input)} Internal interface number of mcast router
* @param    req      @b{(input)} "Query receive" or "configuration added"
* @param    pSnoopCB @b{(input)} Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes none
*
* @end
*
******************************************************************************/
L7_RC_t snoopMcastRtrListAdd(L7_uint32 vlanId, L7_uint32 intIfNum,
                             snoop_requestor_t req, snoop_cb_t *pSnoopCB)
{
  L7_uchar8               macAddr[L7_MAC_ADDR_LEN];
  L7_uint32               vid, timerValue = 0;
  L7_RC_t                 rc;
  snoop_interface_type_t  intfType;
  snoopOperData_t        *pSnoopOperEntry;

  if ((pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_EXACT)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (req == SNOOP_STATIC)
  {
    intfType = SNOOP_STATIC_MCAST_ROUTER_ATTACHED;
  }
  else
  {
    intfType = SNOOP_MCAST_ROUTER_ATTACHED;
  }

  if (L7_INTF_ISMASKBITSET(pSnoopOperEntry->mcastRtrAttached, intIfNum))
  {
    /* This Query has already been processed, this interface has already been
     * added as one of the multicast address destination ports. Even the case of an address
     * being learnt after the initial set of macs are learnt is covered as any new entry
     * that is learnt (via a membership report) goes and queries the list of ports that have seen a mcast router
     * and adds those ports to the destination list.
     * By returning early, we avoid unneccessary avl tree traversal, which can take a
     * long time if the number of mcast entries are high .
     */
    if (req == SNOOP_DYNAMIC)
    {
      if ((timerValue = snoopCheckPrecedenceParamGet(vlanId, intIfNum,
                                                     SNOOP_PARAM_MCAST_RTR_EXPIRY_TIME,
                                                     pSnoopCB->family)) != 0)
      {
        snoopTimerUpdate(L7_NULL, intIfNum, vlanId, SNOOP_MCAST_ROUTER_ATTACHED,
                         timerValue, pSnoopCB);
      }
    }
    return L7_SUCCESS;
  }

  /* Start timer only if it is a dynamically learnt mrouter */
  if (req == SNOOP_DYNAMIC)
  {
    if (snoopTimerStart(L7_NULL, intIfNum, vlanId, SNOOP_MCAST_ROUTER_ATTACHED,
                        pSnoopCB)!= L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopMcastRtrListAdd: Failed to start mrtr timer");
    }
  }

  L7_INTF_SETMASKBIT(pSnoopOperEntry->mcastRtrAttached, intIfNum);
#ifdef L7_MCAST_PACKAGE
  snoopVlanNotifyL3Mcast(vlanId, L7_TRUE);
#endif /* L7_MCAST_PACKAGE*/

  rc = snoopFirstGet(macAddr, &vid, pSnoopCB->family);
  while (rc == L7_SUCCESS)
  {
    if (vid == vlanId)
    {
      snoopIntfAdd(macAddr, vid, intIfNum, intfType, pSnoopCB);
    }
    rc = snoopNextGet(macAddr, vid, macAddr, &vid, pSnoopCB->family);
  }

#ifdef L7_NSF_PACKAGE
    if (pSnoopCB->snoopExec->snoopBackupElected == L7_TRUE)
    {
      snoopCheckpointCallback1(SNOOP_CKPT_MSG_OD_ADD_DATA, NULL, vlanId,
                               pSnoopCB->family, 0, &pSnoopOperEntry->snoopQuerierInfo,
                               L7_NULL, &pSnoopOperEntry->mcastRtrAttached);
    }
#endif /* L7_NSF_PACKAGE */

  return L7_SUCCESS;
}

/**************************************************************************
* @purpose  Remove an interface from the mcast router list and of all
*           snooping entries of a snoop instance
*
* @param    vlanId   @b{(input)} Vlan ID for the MAC Address
* @param    intIfNum @b{(input)} Internal interface number of mcast router
* @param    req      @b{(input)} "Rtr Expired" or "configuration removed"
* @param    pSnoopCB @b{(input)} Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes none
*
* @end
*
***************************************************************************/
L7_RC_t snoopMcastRtrListRemove(L7_uint32 vlanId, L7_uint32 intIfNum,
                                snoop_requestor_t req, snoop_cb_t *pSnoopCB)
{
  L7_uchar8               macAddr[L7_MAC_ADDR_LEN];
  L7_uint32               vid;
  L7_RC_t                 rc;
  snoop_interface_type_t  intfType;
  L7_BOOL                 isTimerRunning = L7_FALSE, isTimerNodePresent = L7_FALSE;
  snoopMrtrTimerData_t   *pmrtrTimerData;
  snoopMrtrTimerDataKey_t key;
  snoopOperData_t        *pSnoopOperEntry;
  snoopIntfCfgData_t     *pCfg;
#ifdef L7_MCAST_PACKAGE
  L7_BOOL                 snoopVlanOperState = L7_FALSE;
#endif /* L7_MCAST_PACKAGE */


  if ((pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_EXACT))
      == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (!L7_INTF_ISMASKBITSET(pSnoopOperEntry->mcastRtrAttached, intIfNum))
  {
    /* Already  removed */
    return L7_SUCCESS;
  }

  key.intIfNum = intIfNum;
  key.vlanId   = vlanId;
  pmrtrTimerData = (snoopMrtrTimerData_t *)
    avlSearchLVL7(&pSnoopCB->snoopMrtrTimerAvlTree, &key,
                  L7_MATCH_EXACT);

  if (pmrtrTimerData != L7_NULLPTR)
  {
    isTimerNodePresent = L7_TRUE;
    if (pmrtrTimerData->mrtrTimer != L7_NULL)
    {
      isTimerRunning = L7_TRUE;
    }
  }

  if (req == SNOOP_STATIC)
  {
    /* Dont clear mrouter attached info if the timer is running
       we have detected that there is a querier on the other side
    */
    if (isTimerRunning == L7_TRUE)
    {
      return L7_SUCCESS;
    }
    intfType = SNOOP_STATIC_MCAST_ROUTER_ATTACHED;
  }
  else
  {
    if (snoopMapIntfIsConfigurable(intIfNum, &pCfg, pSnoopCB) == L7_TRUE)
    {
      /* Do not remove the interface if it is configured statically. Stop
         the timer and return */
      if (pCfg->intfMcastRtrAttached == L7_ENABLE ||
          L7_VLAN_ISMASKBITSET(pCfg->vlanStaticMcastRtr, vlanId))
      {
        snoopTimerStop(L7_NULL, intIfNum, vlanId, SNOOP_MCAST_ROUTER_ATTACHED,
                       pSnoopCB);
        return L7_SUCCESS;
      }
    }
    else
    {
      return L7_FAILURE;
    }
    intfType = SNOOP_MCAST_ROUTER_ATTACHED;
  }

  L7_INTF_CLRMASKBIT(pSnoopOperEntry->mcastRtrAttached, intIfNum);
#ifdef L7_MCAST_PACKAGE
  L7_INTF_NONZEROMASK(pSnoopOperEntry->snoopIntfMode, snoopVlanOperState);
  /* Modify L2-Bitmask of all entries */
  snoopVlanNotifyL3Mcast(vlanId, snoopVlanOperState);
#endif /* L7_MCAST_PACKAGE */
  if (isTimerNodePresent == L7_TRUE)
  {
    snoopTimerStop(L7_NULL, intIfNum, vlanId, SNOOP_MCAST_ROUTER_ATTACHED, pSnoopCB);
  }

  rc = snoopFirstGet(macAddr, &vid, pSnoopCB->family);
  while (rc == L7_SUCCESS)
  {
    if (vid == vlanId)
    {
      snoopIntfRemove(macAddr, vid, intIfNum, intfType, pSnoopCB);
    } /* IF the entry is of this vlan */
    rc = snoopNextGet(macAddr, vid, macAddr, &vid, pSnoopCB->family);
  } /* End of snoo entry iterations */
#ifdef L7_NSF_PACKAGE
  if (pSnoopCB->snoopExec->snoopBackupElected == L7_TRUE)
  {
    snoopCheckpointCallback1(SNOOP_CKPT_MSG_OD_ADD_DATA, NULL, vlanId,
                             pSnoopCB->family, 0, &pSnoopOperEntry->snoopQuerierInfo,
                             L7_NULL, &pSnoopOperEntry->mcastRtrAttached);
  }
#endif /* L7_NSF_PACKAGE */
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Add all multicast router interfaces to a snooping entry
*
* @param    macAddr  @b{(input)} Multicast MAC Address
* @param    vlanId   @b{(input)} Vlan ID for the MAC Address
* @param    intIfNum @b{(input)} Internal interface number
* @param    pSnoopCB @b{(input)} Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t snoopAllMcastRtrIntfsAdd(L7_uchar8 *macAddr, L7_uint32 vlanId,
                                 L7_uint32 intIfNum, snoop_cb_t *pSnoopCB)
{
  L7_uint32         intf; /* Loop through internal interface numbers */
  L7_uint32         mode;
  snoopOperData_t  *pSnoopOperEntry  = L7_NULLPTR;
  L7_RC_t           rc = L7_SUCCESS;

  if ((pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_EXACT))
      == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* Loop through interfaces looking for multicast routers that have been detected */
  for (intf = 1; intf <= L7_MAX_INTERFACE_COUNT; intf++)
  {
    if ((intf != intIfNum) &&
        (L7_INTF_ISMASKBITSET(pSnoopOperEntry->mcastRtrAttached, intf) &&
         (dot1qOperVlanMemberGet(vlanId, intf, &mode) == L7_SUCCESS) &&
         (mode == L7_DOT1Q_FIXED)
        )
       )
    {
      if (snoopIntfAdd(macAddr, vlanId, intf, SNOOP_STATIC_MCAST_ROUTER_ATTACHED,
                       pSnoopCB) != L7_SUCCESS)
      {
        rc = L7_FAILURE;
      }
    }
  } /* End of interface iterations */

  return rc;
}

/*****************************************************************************
* @purpose  Gets list of interfaces for a Multicast address for a
*           specified snoop instance
*
* @param    macAddr   @b{(input)}  MAC Address
* @param    vlanId    @b{(input)}  Vlan ID
* @param    numIntf   @b{(output)} Number of interfaces
* @param    intfList  @b{(output)} Densely packed array of source interfaces
* @param    family    @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                  L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  If there is no group entry with the VID-MAC
*
* @notes    numIntf will be returned as zero if there is no port for the
*           MAC address Only the first numIntf entries in the intfList have
*           valid interfaces, rest are garbage
*
* @end
******************************************************************************/
L7_RC_t snoopIntfListGet(L7_uchar8 *macaddr,L7_uint32 vlanId,
                         L7_INTF_MASK_t *snoopMemberList, L7_uchar8 family)
{
  snoopInfoData_t *snoopEntry = L7_NULLPTR;
  L7_INTF_MASK_t   tempList;

  snoopEntry = snoopEntryFind(macaddr, vlanId, family, L7_MATCH_EXACT);
  if (snoopEntry != L7_NULLPTR)
  {
    memcpy(&tempList, &snoopEntry->snoopGrpMemberList, sizeof(tempList));
    L7_INTF_MASKOREQ(tempList, snoopEntry->snoopMrtrMemberList);
    memcpy(snoopMemberList, &tempList, sizeof(*snoopMemberList));
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Flush all Snooping entries and multicast router entries
*           of a snoop instance
*
* @param    family     @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                   L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopEntriesDoFlush(snoop_cb_t *pSnoopCB)
{
  L7_uchar8         macAddr[L7_MAC_ADDR_LEN];
  L7_uint32         vlanId, intIfNum;
  L7_RC_t           rc;
  snoopOperData_t  *pSnoopOperEntry  = L7_NULLPTR;
  snoopIntfCfgData_t  *pCfg = L7_NULLPTR;

  rc = snoopFirstGet(macAddr, &vlanId, pSnoopCB->family);
  while (rc == L7_SUCCESS)
  {
    snoopEntryRemove(macAddr, vlanId, pSnoopCB->family);
    /* Since we deleted the first entry, we just need to find the first one again */
    rc = snoopFirstGet(macAddr, &vlanId, pSnoopCB->family);
  }

  pSnoopOperEntry = snoopOperEntryFirstGet(pSnoopCB);
  while(pSnoopOperEntry)
  {
#ifdef L7_NSF_PACKAGE
     L7_BOOL changeFlag = L7_FALSE;
#endif /* L7_NSF_PACKAGE  */
    vlanId = pSnoopOperEntry->vlanId;
    rc = nimFirstValidIntfNumber(&intIfNum);
    while (rc == L7_SUCCESS)
    {
      if (snoopIsValidIntf(intIfNum) == L7_TRUE)
      {
        /* Stop the  mrtimers if they are running */
        if (L7_INTF_ISMASKBITSET(pSnoopOperEntry->mcastRtrAttached, intIfNum))
        {
          snoopTimerStop(L7_NULL, intIfNum, vlanId, SNOOP_MCAST_ROUTER_ATTACHED,
                         pSnoopCB);
        }
        if (snoopMapIntfIsConfigurable(intIfNum, &pCfg, pSnoopCB) != L7_TRUE ||
                                      !(L7_VLAN_ISMASKBITSET(pCfg->vlanStaticMcastRtr, vlanId)) ||
                                      pCfg->intfMcastRtrAttached == L7_DISABLE)
          {
            L7_INTF_CLRMASKBIT(pSnoopOperEntry->mcastRtrAttached, intIfNum);
#ifdef L7_NSF_PACKAGE
            changeFlag = L7_TRUE;
#endif /* L7_NSF_PACKAGE  */
          }
      } /* end of is valid intf check */
      rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
    } /* Iterate through the interfaces */
#ifdef L7_NSF_PACKAGE
    if (pSnoopCB->snoopExec->snoopBackupElected == L7_TRUE && 
        changeFlag == L7_TRUE)
    {
      snoopCheckpointCallback1(SNOOP_CKPT_MSG_OD_ADD_DATA, NULL, vlanId,
                               pSnoopCB->family, 0, &pSnoopOperEntry->snoopQuerierInfo,
                               L7_NULL, &pSnoopOperEntry->mcastRtrAttached);
    }
#endif /* L7_NSF_PACKAGE */
    pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_GETNEXT);
  } /* End of vlan iterations */
  pSnoopCB->counters.controlFramesProcessed = 0;
  return L7_SUCCESS;
}

/* PTin added: IGMP */
#if 1
/*********************************************************************
* @purpose  Flush a snooping entry of a snoop instance
*
* @param    family     @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                   L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopEntryDoFlush(L7_uint16 vlan_id, snoop_cb_t *pSnoopCB)
{
  L7_uchar8         macAddr[L7_MAC_ADDR_LEN];
  L7_uint32         vlanId, intIfNum;
  L7_RC_t           rc;
  snoopOperData_t  *pSnoopOperEntry  = L7_NULLPTR;
  snoopIntfCfgData_t  *pCfg = L7_NULLPTR;

  memset((void *)macAddr, 0x00, L7_MAC_ADDR_LEN);

  rc = snoopFirstGet(macAddr, &vlanId, pSnoopCB->family);
  while (rc == L7_SUCCESS)
  {
    if (vlan_id == vlanId)
    {
      snoopEntryRemove(macAddr, vlanId, pSnoopCB->family);
    }
    rc = snoopNextGet(macAddr, vlanId, macAddr, &vlanId, pSnoopCB->family);
  }
  //snoopVlanEntriesRemove(vlan_id, pSnoopCB);

  pSnoopOperEntry = snoopOperEntryFirstGet(pSnoopCB);
  while(pSnoopOperEntry)
  {
#ifdef L7_NSF_PACKAGE
     L7_BOOL changeFlag = L7_FALSE;
#endif /* L7_NSF_PACKAGE  */
    vlanId = pSnoopOperEntry->vlanId;

    if ( vlan_id==vlanId )
    {
      rc = nimFirstValidIntfNumber(&intIfNum);
      while (rc == L7_SUCCESS)
      {
        if (snoopIsValidIntf(intIfNum) == L7_TRUE)
        {
          /* Stop the  mrtimers if they are running */
          if (L7_INTF_ISMASKBITSET(pSnoopOperEntry->mcastRtrAttached, intIfNum))
          {
            snoopTimerStop(L7_NULL, intIfNum, vlanId, SNOOP_MCAST_ROUTER_ATTACHED,
                           pSnoopCB);
          }
          if (snoopMapIntfIsConfigurable(intIfNum, &pCfg, pSnoopCB) != L7_TRUE ||
                                        !(L7_VLAN_ISMASKBITSET(pCfg->vlanStaticMcastRtr, vlanId)) ||
                                        pCfg->intfMcastRtrAttached == L7_DISABLE)
            {
              L7_INTF_CLRMASKBIT(pSnoopOperEntry->mcastRtrAttached, intIfNum);
  #ifdef L7_NSF_PACKAGE
              changeFlag = L7_TRUE;
  #endif /* L7_NSF_PACKAGE  */
            }
        } /* end of is valid intf check */
        rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
      } /* Iterate through the interfaces */
#ifdef L7_NSF_PACKAGE
      if (pSnoopCB->snoopExec->snoopBackupElected == L7_TRUE && 
          changeFlag == L7_TRUE)
      {
        snoopCheckpointCallback1(SNOOP_CKPT_MSG_OD_ADD_DATA, NULL, vlanId,
                                 pSnoopCB->family, 0, &pSnoopOperEntry->snoopQuerierInfo,
                                 L7_NULL, &pSnoopOperEntry->mcastRtrAttached);
      }
#endif /* L7_NSF_PACKAGE */
    }
    pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_GETNEXT);
  } /* End of vlan iterations */
  pSnoopCB->counters.controlFramesProcessed = 0;
  return L7_SUCCESS;
}
#endif


/*********************************************************************
* @purpose  Remove interface from all snoop entries with the specified
*           VLAN
*
* @param    intIfNum    @b{(input)} Internal interface number
* @param    vlanId      @b{(input)} VLAN ID
* @param    pSnoopCB    @b{(input)} Control Block
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopIntfFromVlanRemove(L7_uint32 intIfNum, L7_uint32 vlanId,
                                snoop_cb_t *pSnoopCB)
{
  L7_RC_t         rc;
  L7_uchar8       macAddr[L7_MAC_ADDR_LEN];
  L7_uint32       vid;
  L7_INTF_MASK_t  snoopMemberList;

  memset((void *)macAddr, 0x00, L7_MAC_ADDR_LEN);
  rc = snoopFirstGet(macAddr, &vid, pSnoopCB->family);
  while (rc == L7_SUCCESS)
  {
    if (vid == vlanId)
    {
      if (snoopIntfListGet(macAddr, vid, &snoopMemberList, pSnoopCB->family)
          == L7_SUCCESS)
      {
        if (L7_INTF_ISMASKBITSET(snoopMemberList, intIfNum))
        {
          snoopIntfRemove(macAddr, vid, intIfNum, SNOOP_GROUP_MEMBERSHIP,
                          pSnoopCB);
        }
      }
    }
    rc = snoopNextGet(macAddr, vid, macAddr, &vid, pSnoopCB->family);
  }

  return L7_SUCCESS;
}

/**************************************************************************
* @purpose  Determine if the interface is valid for Snooping configuration
*           and optionally output a pointer to the configuration structure
*           entry for this interface
*
* @param    intIfNum   @b{(input)}  Internal Interface Number
* @param    pCfg       @b{(output)} Output pointer location, or
*                                   L7_NULL if not needed
* @param    pSnoopCB   @b{(input)}  Control Block
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    The caller can set the pCfg parm to L7_NULL if it does not
*           want the value output from this function.
*
* @end
*************************************************************************/
L7_BOOL snoopMapIntfIsConfigurable(L7_uint32 intIfNum,
                                   snoopIntfCfgData_t **pCfg,
                                   snoop_cb_t *pSnoopCB)
{
  L7_uint32      index;
  nimConfigID_t  configId;

  if (snoopIsReady() == L7_FALSE)
  {
    return L7_FALSE;
  }

  /* Check boundary conditions */
  if (intIfNum <= 0 || intIfNum >= platIntfMaxCountGet())
  {
    return L7_FALSE;
  }

  /* Get the Index for accessing the interface config structure */
  index = pSnoopCB->snoopExec->snoopIntfMapTbl[intIfNum];
  /* Do we have a valid index ? */
  if (index == 0)
  {
    return L7_FALSE;
  }

  /* verify that the configId in the config data table entry matches the
  ** configId that NIM maps tothe intIfNum we are considering
  */
  if (nimConfigIdGet(intIfNum, &configId) == L7_SUCCESS)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&configId,
                               &(pSnoopCB->snoopCfgData->snoopIntfCfgData[index].configId)
                              ) == L7_FALSE)
    {
      /* if we get here, either we have a table management error between
       * snoopCfgData and snoopIntfMapTbl or
       * there is synchronization issue between NIM and components w.r.t.
       * interface creation/deletion
       */
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
              "snoopMapIntfIsConfigurable: Error accessing Snooping config data for interface %d, %s, family %d cbIndex %d\n",
              intIfNum, ifName, pSnoopCB->family, pSnoopCB->cbIndex);
      return L7_FALSE;
    }
  }

  *pCfg = &pSnoopCB->snoopCfgData->snoopIntfCfgData[index];
  return L7_TRUE;
}

/*********************************************************************
* @purpose  Validate an Multicast MAC address
*
* @param    macAddr   @b{(input)}  Multicast MAC Address
* @param    family    @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                  L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS  Valid Multicast MAC
* @returns  L7_FAILURE  Invalid multicast MAC
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopMacAddrCheck(L7_uchar8 *macAddr, L7_uchar8 family)
{
  if (family == L7_AF_INET)
  {
    if(macAddr[0] != 0x01 || macAddr[1] != 0x00 || macAddr[2] != 0x5E)
    {
      return L7_FAILURE;
    }
  }
  else if (family == L7_AF_INET6)
  {
    if(macAddr[0] != 0x33 || macAddr[1] != 0x33)
    {
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Checks if the Interface/VLAN ID is valid for Snooping
*
* @param    intIfNum  @b{(input)}  Internal Interface Number
* @param    vlanId    @b{(input)}  VLAN ID
* @param    family    @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                  L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS  If interface is acceptable
* @returns  L7_FAILURE  If interface is not acceptable
*
* @notes    Acceptability criteria
*           1. Must be a valid configured interface number &&
*           2. Must be a physical or LAG interface         &&
*           3. Must be enabled for Snooping
*
* @end
*********************************************************************/
L7_RC_t snoopIntfCheck(L7_uint32 intIfNum, L7_uint32 vlanId, L7_uchar8 family)
{
  L7_uint32 mode;

  if (snoopIsValidIntf(intIfNum) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  /* Check to see if Snooping is enabled on this interface */
  if ( (snoopIntfModeGet(intIfNum, vlanId, &mode, family) == L7_SUCCESS) &&
       (mode == L7_ENABLE) )
  {
    return L7_SUCCESS;
  }


  /* Check to see if Snooping is enabled on this VLAN */
  if ( (snoopVlanModeGet(vlanId, &mode, family) == L7_SUCCESS) &&
       (mode == L7_ENABLE) )
  {
    return L7_SUCCESS;
  }

  /* If we got here, neither the interface nor the VLAN is enabled for Snooping
   */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Obtain a pointer to the first free interface config struct
*
* @param    intIfNum @b{(input)}   Internal Interface Number
* @param    pCfg     @b{(output)}  Ptr to snoop interface config structure
* @param    pSnoopCB @b{(input)}   Control Block
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
L7_BOOL snoopMapIntfConfigEntryGet(L7_uint32 intIfNum,
                                   snoopIntfCfgData_t **pCfg,
                                   snoop_cb_t *pSnoopCB)
{
  L7_uint32      i;
  nimConfigID_t  configId;
  nimConfigID_t  configIdNull;
  L7_RC_t        rc;
  /*
  Size of Array is determined by maxSnoopInstances, currently 2,
  however, there is no define for it -- it is defined by the presence
  of both MLD snooping and IGMP snooping at init time
  */
  static L7_uint32  nextIndex[2] = {1,1};

  if (snoopIsReady() == L7_FALSE)
  {
    return L7_FALSE;
  }

  if ((rc = nimConfigIdGet(intIfNum, &configId)) != L7_SUCCESS)
  {
    return L7_FALSE;
  }

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  /* Avoid N^2 processing when interfaces created at startup */
  if (nextIndex[pSnoopCB->cbIndex] < L7_IGMP_SNOOPING_MAX_INTF)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&(pSnoopCB->snoopCfgData->snoopIntfCfgData[nextIndex[pSnoopCB->cbIndex]].configId),
                               &configIdNull))
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_SNOOPING_COMPONENT_ID,
              "snoopMapIntfConfigEntryGet: Found empty config ID 1 for interface %d, %s family %d cbIndex %d\n",
              intIfNum, ifName, pSnoopCB->family, pSnoopCB->cbIndex);
      if (pSnoopCB->cbIndex == (pSnoopCB->snoopExec->maxSnoopInstances - 1))
      {
        pSnoopCB->snoopExec->snoopIntfMapTbl[intIfNum] = nextIndex[pSnoopCB->cbIndex];
      }
      *pCfg = &pSnoopCB->snoopCfgData->snoopIntfCfgData[nextIndex[pSnoopCB->cbIndex]];
      nextIndex[pSnoopCB->cbIndex]++;
      return L7_TRUE;
    }
  }

  /* Cached nextIndex is in use. Resort to search from beginning. N^2. */
    for (i = 1; i < L7_IGMP_SNOOPING_MAX_INTF; i++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&(pSnoopCB->snoopCfgData->snoopIntfCfgData[i].configId),
                                 &configIdNull))
      {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_SNOOPING_COMPONENT_ID,
              "snoopMapIntfConfigEntryGet: Found empty config ID 2 for interface %d, %s family %d cbIndex %d\n",
              intIfNum, ifName, pSnoopCB->family, pSnoopCB->cbIndex);
        if (pSnoopCB->cbIndex == (pSnoopCB->snoopExec->maxSnoopInstances - 1))
        {
          pSnoopCB->snoopExec->snoopIntfMapTbl[intIfNum] = i;
        }
        *pCfg = &pSnoopCB->snoopCfgData->snoopIntfCfgData[i];
      nextIndex[pSnoopCB->cbIndex] = i + 1;
        return L7_TRUE;
      }
    }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Send a multicast packet on a specified interface and vlan
*
* @param    intIfNum   @b{(input)} Outgoing internal interface number
* @param    vlanId     @b{(input)} VLAN ID
* @param    payload    @b{(input)} Message to be forwarded
* @param    payloadLen @b{(input)} Length of message
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void snoopPacketSend(L7_uint32 intIfNum,
                     L7_uint32 vlanId,
                     L7_uint32 innerVIDUntagged,
                     L7_uchar8 *payload,
                     L7_uint32 payloadLen, L7_uchar8 family)
{
  L7_netBufHandle   bufHandle;
  L7_uchar8        *dataStart;
  L7_INTF_TYPES_t   sysIntfType;

  /* Make sure this port has not been enabled for routing, is not the mirror dest port,
  ** is not a LAG member and is active.
  */
  if (snoopIntfCanBeEnabled(intIfNum, vlanId) != L7_TRUE)
  {
    return;
  }

  /* If outgoing interface is CPU interface, don't send it */
  if ( (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS) &&
       (sysIntfType == L7_CPU_INTF) )
  {
    return;
  }

  SYSAPI_NET_MBUF_GET(bufHandle);
  if (bufHandle == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
           "snoopPacketSend: System out of netbuffs");
    return;
  }

  /* PTin added: IGMP snooping */
  #if 1
  L7_uint16 extOVlan = vlanId;
  L7_uint16 extIVlan = 0;

  ptin_timer_start(31,"snoopPacketSend-ptin_evc_extVlans_get_fromIntVlan");
  /* Extract external outer and inner vlan for this tx interface */
  if (ptin_evc_extVlans_get_fromIntVlan(intIfNum,vlanId,innerVIDUntagged,&extOVlan,&extIVlan)==L7_SUCCESS)
  {
    /* Add inner vlan when there exists */
    #if 0
    if (extIVlan!=0)
    {
      memmove(&payload[20],&payload[16],payloadLen);
      payload[16] = 0x81;
      payload[17] = 0x00;
      payload[18] = extIVlan>>8;
      payload[19] = extIVlan & 0xff;
      payloadLen += 4;
      //innerVlanId = extIVlan;
    }
    #endif
    /* Modify outer vlan */
    if (vlanId!=extOVlan)
    {
      payload[14] &= 0xf0;
      payload[14] |= ((extOVlan>>8) & 0x0f);
      payload[15]  = extOVlan & 0xff;
      //vlanId = extOVlan;
      //LOG_TRACE(LOG_CTX_PTIN_IGMP,"Packet will be transmitted in intIfNum=%u, with vlan=%u (original=%u)",intIfNum,extOVlan,vlanId);
    }
  }
  ptin_timer_stop(31);
  #endif

  ptin_timer_start(32,"snoopPacketSend-snoopFrameTransmit before");
  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, dataStart);
  memcpy(dataStart, payload, payloadLen);
  SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, payloadLen);

#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
  if (snoopPacketModifyInnerTag(intIfNum, vlanId,
                                innerVIDUntagged, bufHandle) != L7_SUCCESS)
  {
    return;
  }
#endif

  snoopDebugPacketTxTrace(intIfNum, vlanId, payload, family);
  ptin_timer_start(33,"snoopPacketSend-snoopFrameTransmit");
  snoopFrameTransmit(intIfNum, vlanId, bufHandle, SNOOP_VLAN_INTF_SEND);
  ptin_timer_stop(33);
  ptin_timer_stop(32);

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Packet transmited to intIfNum=%u, with oVlan=%u (intVlan=%u)",
            intIfNum, extOVlan, vlanId);

  return;
}

/*********************************************************************
* @purpose  Transmit a frame
*
* @param    intIfNum  @b{(input)}   Outgoing internal interface number
* @param    vlanId    @b{(input)}   VLAN ID
* @param    bufHandle @b{(input)}   Handle to the network buffer where
*                                   frame is stored
* @param    sendType  @b{(input)}   SNOOP_VLAN_SEND - Send it on specified
*                                   VLAN
*                                   SNOOP_VLAN_INTF_SEND - Send it on
*                                   specified VLAN, interface
*
*
* @notes
*
* @end
*********************************************************************/
void snoopFrameTransmit(L7_uint32 intIfNum, L7_uint32 vlanId,
                        L7_netBufHandle bufHandle, snoop_send_type_t type)
{
  DTL_CMD_TX_INFO_t  dtlCmd;
  snoop_cb_t     *pSnoopCB;

  memset((L7_uchar8 *)&dtlCmd, 0, sizeof(DTL_CMD_TX_INFO_t));

  #if 0
  dtlCmd.priority            = 1;
  #else
  if ((pSnoopCB=snoopCBGet(L7_AF_INET)) != L7_NULLPTR)
  {
    dtlCmd.priority = pSnoopCB->snoopCfgData->snoopAdminIGMPPrio;
  }
  else
  {
    dtlCmd.priority = SNOOP_IGMP_DEFAULT_PRIO;
  }
  #endif

  if (SNOOP_VLAN_SEND == type)
  {
    dtlCmd.typeToSend          = DTL_VLAN_MULTICAST;
  }
  else
  {
    dtlCmd.typeToSend          = DTL_NORMAL_UNICAST;
    dtlCmd.intfNum             = intIfNum;
  }
  dtlCmd.cmdType.L2.domainId = vlanId;

  dtlPduTransmit (bufHandle, DTL_CMD_TX_L2, &dtlCmd);

  return;
}

/***********************************************************************
* @purpose Flood multicast packet within the VLAN, except incoming intf
*
* @param   mcastPacket  @b{(input)} Pointer to data structure to hold
*                                   control packet
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
***********************************************************************/
L7_RC_t snoopPacketFlood(mgmdSnoopControlPkt_t *mcastPacket)
{
  /* PTin removed:IGMP snooping */
  // Do not allow flooding!
  #if 0
  NIM_INTF_MASK_t maskBuf, reverseMaskBuf;
  L7_uint32       i, maskSize, numPorts;
  L7_uint32       intfList[L7_MAX_INTERFACE_COUNT + 1];
#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
  L7_uint32 inIntfType, outIntfType, svType;
  dot1adSubscriptionStruct_t  *subscrEntry;
  L7_BOOL   skipServiceCheck = L7_FALSE;
#endif

  /* Get list of all ports in the VLAN */
  memset((L7_uchar8 *)&reverseMaskBuf, 0, sizeof(reverseMaskBuf));
  if (dot1qVlanCurrentEgressPortsGet(mcastPacket->vlanId,
                                     (NIM_INTF_MASK_t *)&reverseMaskBuf,
                                     &maskSize) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Since dot1q returns the mask reversed for SNMP, we need to reverse it again */
  memset((L7_uchar8 *)&maskBuf, 0, sizeof(maskBuf));
  nimReverseMask(reverseMaskBuf, &maskBuf);

  /* Convert mask to a list of interfaces */
  memset((L7_uchar8 *)intfList, 0, sizeof(intfList));
  if (nimMaskToList(&maskBuf, intfList, &numPorts) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
  do
  {
    if (dot1adInterfaceTypeGet(mcastPacket->intIfNum, &inIntfType) != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(mcastPacket->intIfNum, L7_SYSNAME, ifName);

      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
             "Could not get dot1ad interface type for interface:%s",
              ifName);
      return L7_FAILURE;
    }

    if (inIntfType == DOT1AD_INTFERFACE_TYPE_SWITCHPORT)
    {
      skipServiceCheck = L7_TRUE;
    }
    else if (dot1adServiceTypeGet(mcastPacket->vlanId, &svType) != L7_SUCCESS)
    {
      if (inIntfType == DOT1AD_INTFERFACE_TYPE_NNI ||
          dot1adSubscriptionTreeFirstIntfEntryGet(mcastPacket->intIfNum, &subscrEntry) != L7_SUCCESS)
      {
        skipServiceCheck = L7_TRUE;
      }
      else
      {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
             "Could not get service type for svid:%d",mcastPacket->vlanId);
        return L7_FAILURE;
      }
      break;
    }

  } while(0);
#endif

  /* Forward on all interfaces in the VLAN, except the one the packet came in on. */
  for (i = 0; i < numPorts; i++)
  {
    if (intfList[i] != mcastPacket->intIfNum)
    {
#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
      if ((skipServiceCheck == L7_FALSE) &&
          (svType == L7_DOT1AD_SVCTYPE_ELINE ||
           svType == L7_DOT1AD_SVCTYPE_ETREE))
      {
      if (dot1adInterfaceTypeGet(intfList[i], &outIntfType) != L7_SUCCESS)
      {
        continue;
      }
      else if ( ((inIntfType == DOT1AD_INTFERFACE_TYPE_UNI) ||
                 (inIntfType == DOT1AD_INTFERFACE_TYPE_UNI_P) ||
                 (inIntfType == DOT1AD_INTFERFACE_TYPE_UNI_S))
                 &&
               outIntfType != DOT1AD_INTFERFACE_TYPE_NNI)
      {
        continue;
      }
      }
#endif

 snoopPacketSend(intfList[i], mcastPacket->vlanId, mcastPacket->innerVlanId,
                       mcastPacket->payLoad,
                      mcastPacket->length, mcastPacket->cbHandle->family);
    }
  } /* iterate throught the egress port list */
  #endif

  return L7_SUCCESS;
}

/**********************************************************************
* @purpose Forward packet to all interfaces with multicast routers
*          attached
*
* @param   mcastPacket  @b{(input)} Pointer to data structure to hold
*                                   control packet
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   This function will send to all interfaces within the specified
*          VLAN where multicast routers have detected, except for the
*          interface on which the packet arrived.
*
* @end
*
*********************************************************************/
L7_RC_t snoopPacketRtrIntfsForward(mgmdSnoopControlPkt_t *mcastPacket, L7_uint8 igmp_type)
{
  L7_uint32         intf; /* Loop through internal interface numbers */
  /* PTin removed: IGMP snooping */
  #if 0
  L7_INTF_MASK_t    zeroMask;
  snoop_cb_t       *pSnoopCB = L7_NULLPTR;
  snoopOperData_t  *pSnoopOperEntry;
  #endif
#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
  L7_uint32         inIntfType=0;
  L7_uint32         outIntfType=0;
  L7_uint32         svType=0;
  dot1adSubscriptionStruct_t  *subscrEntry;
  L7_BOOL   skipServiceCheck = L7_FALSE;
#endif

  /* PTin removed: IGMP snooping */
  #if 0
  if ((pSnoopOperEntry = snoopOperEntryGet(mcastPacket->vlanId, mcastPacket->cbHandle,
                                           L7_MATCH_EXACT)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(mcastPacket->cbHandle->family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  memset(&zeroMask, 0, sizeof(zeroMask));
  if (memcmp(&(pSnoopOperEntry->mcastRtrAttached), &zeroMask,
             sizeof(zeroMask)) == 0)
  {
    return L7_SUCCESS;
  }
  #endif

  /* PTin added: IGMP snooping */
  #if 1
  L7_INTF_MASK_t mcastRtrAttached;

  ptin_timer_start(29,"snoopPacketRtrIntfsForward-ptin_igmp_rootIntfs_getList");
  if (ptin_igmp_rootIntfs_getList(mcastPacket->vlanId, &mcastRtrAttached)!=L7_SUCCESS)
  {
    //printf("%s(%d) Error getting list of router interfaces\r\n",__FUNCTION__,__LINE__);
    ptin_igmp_stat_increment_field(mcastPacket->intIfNum,mcastPacket->vlanId,mcastPacket->client_idx,SNOOP_STAT_FIELD_IGMP_TX_FAILED);
    return L7_FAILURE;
  }
  ptin_timer_stop(29);
  #endif

#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
  do
  {
    if (dot1adInterfaceTypeGet(mcastPacket->intIfNum, &inIntfType) != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(mcastPacket->intIfNum, L7_SYSNAME, ifName);

      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
             "Could not get dot1ad interface type for interface:%s", ifName);
      break;
    }

    if (inIntfType == DOT1AD_INTFERFACE_TYPE_SWITCHPORT)
    {
      skipServiceCheck = L7_TRUE;
    }
    else if (dot1adServiceTypeGet(mcastPacket->vlanId, &svType) != L7_SUCCESS)
    {
      if (inIntfType == DOT1AD_INTFERFACE_TYPE_NNI ||
          dot1adSubscriptionTreeFirstIntfEntryGet(mcastPacket->intIfNum, &subscrEntry) != L7_SUCCESS)
      {
        skipServiceCheck = L7_TRUE;
      }
      else
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
                "Could not get service type for svid:%d",mcastPacket->vlanId);
        ptin_igmp_stat_increment_field(mcastPacket->intIfNum,mcastPacket->vlanId,mcastPacket->client_idx,SNOOP_STAT_FIELD_IGMP_TX_FAILED);
        //return L7_SUCCESS;
        return L7_FAILURE;
      }
      break;
    }
  } while(0);
#endif

     /* Forward frame to all interfaces in this VLAN with multicast routers attached */
  for (intf = 1; intf <= L7_MAX_INTERFACE_COUNT; intf++)
  {
    if ( (L7_INTF_ISMASKBITSET(/*pSnoopOperEntry->*/mcastRtrAttached,           /* PTin modified: IGMP snooping */
                               intf)) /*&& (intf != mcastPacket->intIfNum)*/ )
    {
#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
      if ((skipServiceCheck == L7_FALSE) &&
          (svType == L7_DOT1AD_SVCTYPE_ELINE ||
           svType == L7_DOT1AD_SVCTYPE_ETREE))
      {
      if (dot1adInterfaceTypeGet(intf, &outIntfType) != L7_SUCCESS)
      {
        continue;
      }
      else if ( ((inIntfType == DOT1AD_INTFERFACE_TYPE_UNI)||
                 (inIntfType == DOT1AD_INTFERFACE_TYPE_UNI_P) ||
                 (inIntfType == DOT1AD_INTFERFACE_TYPE_UNI_S)
                )
                 &&
               outIntfType != DOT1AD_INTFERFACE_TYPE_NNI)
      {
        continue;
      }
      }
#endif
      ptin_timer_start(30,"snoopPacketRtrIntfsForward-snoopPacketSend");
      snoopPacketSend(intf, mcastPacket->vlanId, mcastPacket->innerVlanId,
                      mcastPacket->payLoad,
                      mcastPacket->length, mcastPacket->cbHandle->family);
      ptin_timer_stop(30);
      //printf("%s(%d) Packet sent to intf %u with vlan %u\r\n",__FUNCTION__,__LINE__,intf,mcastPacket->vlanId);

      switch (igmp_type)
      {
      case L7_IGMP_MEMBERSHIP_QUERY:
        ptin_igmp_stat_increment_field(intf,mcastPacket->vlanId,mcastPacket->client_idx,SNOOP_STAT_FIELD_GENERAL_QUERIES_SENT);
        //ptin_igmp_stat_increment_field(intf,mcastPacket->vlanId,mcastPacket->client_idx,SNOOP_STAT_FIELD_SPECIFIC_QUERIES_SENT);
        break;      
      case L7_IGMP_V1_MEMBERSHIP_REPORT:
      case L7_IGMP_V2_MEMBERSHIP_REPORT:
      case L7_IGMP_V3_MEMBERSHIP_REPORT:
        ptin_igmp_stat_increment_field(intf,mcastPacket->vlanId,mcastPacket->client_idx,SNOOP_STAT_FIELD_JOINS_SENT);
        break;
      case L7_IGMP_V2_LEAVE_GROUP:
        ptin_igmp_stat_increment_field(intf,mcastPacket->vlanId,mcastPacket->client_idx,SNOOP_STAT_FIELD_LEAVES_SENT);
        break;
      }
      ptin_igmp_stat_increment_field(intf,mcastPacket->vlanId,mcastPacket->client_idx,SNOOP_STAT_FIELD_IGMP_SENT);
    }
  } /* End of interface iterations */
  return L7_SUCCESS;
}

/* PTin added: IGMP snooping */
#if 1
/**********************************************************************
* @purpose Forward packet to all interfaces with multicast clients
*          attached
*
* @param   mcastPacket  @b{(input)} Pointer to data structure to hold
*                                   control packet
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   This function will send to all interfaces within the specified
*          VLAN where multicast clients have detected, except for the
*          interface on which the packet arrived.
*
* @end
*
*********************************************************************/
L7_RC_t snoopPacketClientIntfsForward(mgmdSnoopControlPkt_t *mcastPacket, L7_uint8 igmp_type)
{
  L7_uint32      intf; /* Loop through internal interface numbers */
  L7_INTF_MASK_t mcastClientAttached;

  if (ptin_igmp_clientIntfs_getList(mcastPacket->vlanId, &mcastClientAttached)!=L7_SUCCESS)
  {
    //printf("%s(%d) Error getting list of router interfaces\r\n",__FUNCTION__,__LINE__);
    ptin_igmp_stat_increment_field(mcastPacket->intIfNum,mcastPacket->vlanId,mcastPacket->client_idx,SNOOP_STAT_FIELD_IGMP_TX_FAILED);
    return L7_FAILURE;
  }

  /* Forward frame to all interfaces in this VLAN with multicast routers attached */
  for (intf = 1; intf <= L7_MAX_INTERFACE_COUNT; intf++)
  {
    if ( (L7_INTF_ISMASKBITSET(mcastClientAttached,intf)) )
    {
      snoopPacketSend(intf, mcastPacket->vlanId, mcastPacket->innerVlanId,
                      mcastPacket->payLoad,
                      mcastPacket->length, mcastPacket->cbHandle->family);

      switch (igmp_type)
      {
      case L7_IGMP_MEMBERSHIP_QUERY:
        ptin_igmp_stat_increment_field(intf,mcastPacket->vlanId,mcastPacket->client_idx,SNOOP_STAT_FIELD_GENERAL_QUERIES_SENT);
        //ptin_igmp_stat_increment_field(intf,mcastPacket->vlanId,mcastPacket->client_idx,SNOOP_STAT_FIELD_SPECIFIC_QUERIES_SENT);
        break;      
      case L7_IGMP_V1_MEMBERSHIP_REPORT:
      case L7_IGMP_V2_MEMBERSHIP_REPORT:
      case L7_IGMP_V3_MEMBERSHIP_REPORT:
        ptin_igmp_stat_increment_field(intf,mcastPacket->vlanId,mcastPacket->client_idx,SNOOP_STAT_FIELD_JOINS_SENT);
        break;
      case L7_IGMP_V2_LEAVE_GROUP:
        ptin_igmp_stat_increment_field(intf,mcastPacket->vlanId,mcastPacket->client_idx,SNOOP_STAT_FIELD_LEAVES_SENT);
        break;
      }
      ptin_igmp_stat_increment_field(intf,mcastPacket->vlanId,mcastPacket->client_idx,SNOOP_STAT_FIELD_IGMP_SENT);
    }
  } /* End of interface iterations */
  return L7_SUCCESS;
}
#endif

#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
/**********************************************************************
* @purpose Forward packet to given interface,vlanid after inserting
*          the innerVlanId and outterVlanid
*
* @param   intIfNum  @b{(input)} interface to which pkt is sent
* @param   vlanId    @b{(input)} Outter VLANID to which pkt is sent
* @param   orgBufHandle @b{(input)} Handle to packet data
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   This function will send to all the possible subscriptions for
*          the specified intIfNum, vlanId combination
*
* @end
*
*********************************************************************/
L7_RC_t snoopBuildInsertInnerTagFrameTransmit(L7_uint32 intIfNum,  L7_uint32 vlanId,
                                              L7_netBufHandle orgBufHandle)
{
  DOT1AD_INTFERFACE_TYPE_t intfType;
  L7_ushort16 dvlanEthType = 0;
  L7_ushort16                innerTpid = L7_8021Q_VLAN_PROTOCOL_ID;
  L7_uint32                  dEthType;
  L7_ushort16                shortVal;
  L7_uint32                  dataLength, orgDataLength;
  L7_uchar8                  *dataStart, *orgDataStart;
  L7_netBufHandle            bufHandle;
  dot1adSubscriptionStruct_t *subscrEntry = L7_NULLPTR;
  L7_DOT1AD_SVCTYPE_t        serviceType;

  if (dot1adInterfaceTypeGet(intIfNum, &intfType) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
           "Could not get dot1ad interface type for interface:%s", ifName);
    return L7_FAILURE;
  }

  if (dvlantagIntfEthertypeGet(intIfNum , &dEthType, 0) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
           "Could not get dvlan ethertype for interface:%s", ifName);
    return L7_FAILURE;
  }
  dvlanEthType = dEthType;

  if (intfType == DOT1AD_INTFERFACE_TYPE_SWITCHPORT ||
      dot1adServiceTypeGet(vlanId, &serviceType) != L7_SUCCESS)
  {
    /* This VLAN is not associated with any service,
     * so send the query as normal untagged packet on the interface */
    if(((dot1adSubscriptionTreeFirstIntfEntryGet(intIfNum, &subscrEntry) != L7_SUCCESS) &&
       ((intfType == DOT1AD_INTFERFACE_TYPE_UNI_S) ||
        (intfType == DOT1AD_INTFERFACE_TYPE_UNI_P) ||
        (intfType == DOT1AD_INTFERFACE_TYPE_UNI))) ||
        (intfType == DOT1AD_INTFERFACE_TYPE_NNI)   ||
        (intfType == DOT1AD_INTFERFACE_TYPE_SWITCHPORT) )
    {
      SYSAPI_NET_MBUF_GET_DATASTART(orgBufHandle, orgDataStart);
      SYSAPI_NET_MBUF_GET_DATALENGTH(orgBufHandle, orgDataLength);
      SYSAPI_NET_MBUF_GET(bufHandle);
      if (bufHandle == L7_NULL)
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,"Could not get MBUF");
        return L7_FAILURE;
      }
      SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, dataStart);
      memcpy(dataStart, orgDataStart, orgDataLength);
      SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, orgDataLength);

      /* Insert some junk tag so it will be removed in hapi */
      SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, dataStart);
      SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, dataLength);

      if (intfType == DOT1AD_INTFERFACE_TYPE_NNI)
      {
        memmove(dataStart+20, dataStart+12, dataLength-12);
        dataStart += 12;
        SNOOP_PUT_SHORT(innerTpid, dataStart);
        shortVal = (0x0FFFF & vlanId);
        SNOOP_PUT_SHORT(shortVal,dataStart);
        dataLength += sizeof(L7_8021QTag_t);
      }
      else
      {
      memmove(dataStart+16, dataStart+12, dataLength-12);
      dataStart += 12;
      }
      SNOOP_PUT_SHORT(innerTpid, dataStart);
      shortVal = (0x0FFFF & vlanId);
      SNOOP_PUT_SHORT(shortVal,dataStart);
      SNOOP_UNUSED_PARAM(dataStart);

      dataLength += sizeof(L7_8021QTag_t);
      SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, dataLength);
      snoopFrameTransmit(intIfNum, vlanId, bufHandle, SNOOP_VLAN_INTF_SEND);
      return L7_SUCCESS;
    }
  }


  SYSAPI_NET_MBUF_GET_DATASTART(orgBufHandle, orgDataStart);
  SYSAPI_NET_MBUF_GET_DATALENGTH(orgBufHandle, orgDataLength);

  if((intfType == DOT1AD_INTFERFACE_TYPE_UNI_S) ||
     (intfType == DOT1AD_INTFERFACE_TYPE_UNI_P) ||
     (intfType == DOT1AD_INTFERFACE_TYPE_UNI))
  {
    if(serviceType != L7_DOT1AD_SVCTYPE_TLS)
    {
      if(dot1adSubscriptionEntryTreeSearch(intIfNum, vlanId,
                                           0,
                                           L7_MATCH_EXACT,
                                           &subscrEntry) != L7_SUCCESS)
      {
        if(dot1adSubscriptionEntryTreeSearch(intIfNum, vlanId,
                                             0,
                                             L7_MATCH_GETNEXT,
                                             &subscrEntry) != L7_SUCCESS)
        {
          L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
          nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
                 "Could not get subscription details on interface:%s vlan:%d",
                  ifName,vlanId);
          return L7_FAILURE;
        }
      }
    }
  }
  else
  {
    if(serviceType != L7_DOT1AD_SVCTYPE_TLS)
    {
      if(dot1adSubscriptionEntryTreeSearchOnSvid(vlanId, &subscrEntry) != L7_SUCCESS)
      {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
               "Could not get subscription details on interface:%s vlan:%d",
                ifName,vlanId);
        return L7_FAILURE;
      }
      if(subscrEntry->subscrEntryKey.svid != vlanId)
      {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
               "Could not get subscription details on interface:%s vlan:%d",
               ifName,vlanId);
        return L7_FAILURE;
      }
    }
  }

  SYSAPI_NET_MBUF_GET(bufHandle);
  if (bufHandle == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,"Could not get MBUF");
    return L7_FAILURE;
  }
  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, dataStart);
  memcpy(dataStart, orgDataStart, orgDataLength);
  SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, orgDataLength);

    /* Insert CTAG if necessary */
  if (((serviceType != L7_DOT1AD_SVCTYPE_TLS) &&
       (subscrEntry->pktType != L7_DOT1AD_PKTTYPE_PRIOTAGGED) &&
       (subscrEntry->pktType != L7_DOT1AD_PKTTYPE_UNTAGGED) &&
      ((intfType == DOT1AD_INTFERFACE_TYPE_UNI_P) ||
       (intfType == DOT1AD_INTFERFACE_TYPE_UNI))) ||
     (((intfType == DOT1AD_INTFERFACE_TYPE_NNI) &&
       (serviceType != L7_DOT1AD_SVCTYPE_TLS) &&
       (subscrEntry->subscrEntryKey.cvid != 0)) ||
      ((intfType == DOT1AD_INTFERFACE_TYPE_UNI_S) &&
       (serviceType != L7_DOT1AD_SVCTYPE_TLS) &&
       (subscrEntry->cvidToMatch != 0))))
  {
    SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, dataStart);
    SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, dataLength);
      /* Remark or Insert ctag */
      memmove(dataStart+16,dataStart+12,dataLength-12);
      dataStart += 12;
    SNOOP_PUT_SHORT(innerTpid, dataStart);
    if(intfType == DOT1AD_INTFERFACE_TYPE_NNI)
    {
      shortVal = (subscrEntry->subscrEntryKey.cvid & 0x0FFF);
    }
    else
    {
      shortVal = (subscrEntry->cvidToMatch & 0x0FFF);
    }
    shortVal = (shortVal | (subscrEntry->priorityToMatch << 13));
    SNOOP_PUT_SHORT(shortVal,dataStart);
    SNOOP_UNUSED_PARAM(dataStart);
    dataLength += sizeof(L7_8021QTag_t);
    SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, dataLength);
  }

  if ((intfType == DOT1AD_INTFERFACE_TYPE_NNI) || (intfType == DOT1AD_INTFERFACE_TYPE_UNI_S))
  {
    /* Insert STAG */
    SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, dataStart);
    SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, dataLength);

    memmove(dataStart+16,dataStart+12,dataLength-12);
    dataStart += 12;
    SNOOP_PUT_SHORT(dvlanEthType,dataStart);
    if((intfType == DOT1AD_INTFERFACE_TYPE_UNI_S) && (serviceType != L7_DOT1AD_SVCTYPE_TLS))
    {
      shortVal = (subscrEntry->svidToMatch & 0x0FFF);
    }
    else
    {
    shortVal = vlanId;
    }
    shortVal &= 0x0FFF;
    SNOOP_PUT_SHORT(shortVal,dataStart);
    SNOOP_UNUSED_PARAM(dataStart);

    dataLength += sizeof(L7_8021QTag_t);
    SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, dataLength);
  }
  if(((serviceType          == L7_DOT1AD_SVCTYPE_TLS)        ||
      (subscrEntry->pktType == L7_DOT1AD_PKTTYPE_PRIOTAGGED) ||
      (subscrEntry->pktType == L7_DOT1AD_PKTTYPE_UNTAGGED))  &&
     ((intfType == DOT1AD_INTFERFACE_TYPE_UNI_P) || (intfType == DOT1AD_INTFERFACE_TYPE_UNI)))
  {
    /* Insert some junk tag so it will be removed in hapi */
    SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, dataStart);
    SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, dataLength);

    memmove(dataStart+16, dataStart+12, dataLength-12);
    dataStart += 12;
    SNOOP_PUT_SHORT(innerTpid, dataStart);
    shortVal = 0;
    SNOOP_PUT_SHORT(shortVal,dataStart);
    SNOOP_UNUSED_PARAM(dataStart);

    dataLength += sizeof(L7_8021QTag_t);
    SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, dataLength);
  }
  snoopFrameTransmit(intIfNum, vlanId, bufHandle, SNOOP_VLAN_INTF_SEND);
  return L7_SUCCESS;
}
/**********************************************************************
* @purpose Modify packet by inserting or re-marking the innerVlanId and
*          insert outterVlanid according to the service
*
* @param   intIfNum  @b{(input)} interface to which pkt is sent
* @param   vlanId    @b{(input)} Outter VLANID to which pkt is sent
* @param   innerVIDUntagged  @b{(input)} Inner VLANID with which pkt
*                    was received. It is 0 if the received pkt is inner
*                    untagged.
* @param   bufHandle @b{(input)} Handle to packet data
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  This function finds out the subscription on which the pkt was
*         received. Based on the match criteria of that subscription,
*         innertag is decided
*
* @end
*
*********************************************************************/
L7_RC_t snoopPacketModifyInnerTag(L7_uint32 intIfNum, L7_uint32 vlanId,
                                  L7_uint32 innerVIDUntagged,
                                  L7_netBufHandle bufHandle)
{
  DOT1AD_INTFERFACE_TYPE_t intfType;
  L7_ushort16 svid, dvlanEthType = 0;
  L7_uint32  dataLength,dEthType=0;
  L7_DOT1AD_SVCTYPE_t svType;
  L7_uchar8        *dataStart;
  L7_ushort16       tmpVid;
  dot1adSubscriptionStruct_t *subscrEntry = L7_NULLPTR;
  L7_uint32 matchType = L7_MATCH_EXACT;
  L7_uint32 searchVlanId = 0;
  L7_uint32 innerVlanPos = 0;
  L7_BOOL   foundEntry = L7_FALSE;

  if (dot1adInterfaceTypeGet(intIfNum, &intfType) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
           "Could not get dot1ad interface type for interface:%s", ifName);
    SYSAPI_NET_MBUF_FREE(bufHandle);
    return L7_FAILURE;
  }

  if (intfType == DOT1AD_INTFERFACE_TYPE_SWITCHPORT)
  {
    return L7_SUCCESS;
  }

  if (dvlantagIntfEthertypeGet(intIfNum , &dEthType, 0) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
           "Could not get dvlan ethertype for interface:%s", ifName);
    SYSAPI_NET_MBUF_FREE(bufHandle);
    return L7_FAILURE;
  }

  svid = vlanId;
  if (dot1adServiceTypeGet(svid, &svType) != L7_SUCCESS)
  {
    return L7_SUCCESS;
  }

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, dataStart);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, dataLength);
  dvlanEthType = dEthType;

  if (intfType == DOT1AD_INTFERFACE_TYPE_NNI)
  {
    /* CPU receives always 0x8100 as the outer TPID */
    /* So setting the TPID to the configured values */
    dataStart += 12; /* CPU receives always 0x8100 as the outer TPID */
    SNOOP_PUT_SHORT(dvlanEthType,dataStart);
    SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, dataStart);

    if (innerVIDUntagged > 0)
    {
      memmove(dataStart+20,dataStart+16,dataLength-16);
      dataStart += 16;
      /* Inner VLAN ID should be always 0x8100 */
      dvlanEthType = L7_8021Q_VLAN_PROTOCOL_ID;
      SNOOP_PUT_SHORT(dvlanEthType,dataStart);
      tmpVid = innerVIDUntagged;
      SNOOP_PUT_SHORT(tmpVid,dataStart);
      SNOOP_UNUSED_PARAM(dataStart);
      dataLength += sizeof(L7_8021QTag_t);
      SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, dataLength);
   }
  }
  else if ( (intfType == DOT1AD_INTFERFACE_TYPE_UNI)||
            (intfType == DOT1AD_INTFERFACE_TYPE_UNI_P)||
            (intfType == DOT1AD_INTFERFACE_TYPE_UNI_S)
          )
  {
    if (svType == L7_DOT1AD_SVCTYPE_ETREE)
    {
      if (innerVIDUntagged != L7_DOT1AD_ETREE_RESERVED_INNER_VLAN)
      {
        SYSAPI_NET_MBUF_FREE(bufHandle);
        return L7_FAILURE;
      }
    }
    if ( (svType == L7_DOT1AD_SVCTYPE_TLS) ||
         (svType == L7_DOT1AD_SVCTYPE_ETREE)
       )
    {

      matchType = L7_MATCH_GETNEXT;
      searchVlanId = 0;
    }
    else
    {
      searchVlanId = innerVIDUntagged;
    }
    if(dot1adSubscriptionEntryTreeSearch(intIfNum, vlanId,
                                         searchVlanId,
                                         matchType,
                                         &subscrEntry) == L7_SUCCESS)
    {
      if (subscrEntry->subscrEntryKey.svid == vlanId)
      {
        foundEntry = L7_TRUE;
      }
    }
    else /* Exact match not found */
    {
      searchVlanId = 0;
      matchType = L7_MATCH_EXACT; /* Try with vlanId + 0 exact combination */
      if (dot1adSubscriptionEntryTreeSearch(intIfNum, vlanId,
                                         searchVlanId,
                                         matchType,
                                         &subscrEntry) == L7_SUCCESS)
      {

        if (subscrEntry->subscrEntryKey.svid == vlanId)
        {
          switch(intfType)
          {
            case DOT1AD_INTFERFACE_TYPE_UNI_S:
            /* Match SVID alone can get DT frames */
            if (subscrEntry->cvidToMatch == 0)
            {
              foundEntry = L7_TRUE;
            }
            break;
            case DOT1AD_INTFERFACE_TYPE_UNI_P:
            case DOT1AD_INTFERFACE_TYPE_UNI:
            /* Match Untagged frames alone */
            if ( (subscrEntry->cvidToMatch == 0) ||
                 (subscrEntry->pktType == L7_DOT1AD_PKTTYPE_UNTAGGED) ||
                 (subscrEntry->pktType == L7_DOT1AD_PKTTYPE_PRIOTAGGED)
               )
            {
              foundEntry = L7_TRUE;
            }
            break;
           default:
           break;
         }
        }
      }
      else
      {
        searchVlanId = 0;
        matchType = L7_MATCH_GETNEXT; /* Try with vlanId + 0 next combination */

        if (dot1adSubscriptionEntryTreeSearch(intIfNum, vlanId,
                                              searchVlanId,
                                               matchType,
                                             &subscrEntry) == L7_SUCCESS)
        {

          if (subscrEntry->subscrEntryKey.svid == vlanId)
          {
            switch(intfType)
            {
              case DOT1AD_INTFERFACE_TYPE_UNI_S:
              /* Match SVID alone can get DT frames */
              if (subscrEntry->cvidToMatch == 0)
              {
                foundEntry = L7_TRUE;
              }
             break;
            case DOT1AD_INTFERFACE_TYPE_UNI_P:
            case DOT1AD_INTFERFACE_TYPE_UNI:
            /* Match Untagged frames alone */
             if ( (subscrEntry->cvidToMatch == 0) ||
                  (subscrEntry->pktType == L7_DOT1AD_PKTTYPE_UNTAGGED) ||
                  (subscrEntry->pktType == L7_DOT1AD_PKTTYPE_PRIOTAGGED)
                )
             {
               foundEntry = L7_TRUE;
             }
             break;
            default:
            break;
          }
        }
      }
     }
    }
    if (foundEntry == L7_TRUE)
    {

      if ( (subscrEntry->pktType == L7_DOT1AD_PKTTYPE_UNTAGGED) ||
           (subscrEntry->pktType == L7_DOT1AD_PKTTYPE_PRIOTAGGED) ||
           ( (svType == L7_DOT1AD_SVCTYPE_TLS) &&
             ( (intfType == DOT1AD_INTFERFACE_TYPE_UNI)||
               (intfType == DOT1AD_INTFERFACE_TYPE_UNI_P)
             )
           )
         )
      {
        /* We need not to remove only the Outer VLAN id */
        /* Inner Vlan ID is already removed */

          if (svType != L7_DOT1AD_SVCTYPE_TLS)
          {
          SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, dataStart);
          dataStart += 12; /* CPU receives always 0x8100 as the outer TPID */
          SNOOP_PUT_SHORT(dvlanEthType,dataStart);
          tmpVid = 0;
          SNOOP_PUT_SHORT(tmpVid,dataStart);
          SNOOP_UNUSED_PARAM(dataStart);
      }
          else
          {
            SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, dataStart);
            dataStart += 12; /* CPU receives always 0x8100 as the outer TPID */
            SNOOP_PUT_SHORT(dvlanEthType,dataStart);
            tmpVid = innerVIDUntagged;
            SNOOP_PUT_SHORT(tmpVid,dataStart);
            SNOOP_UNUSED_PARAM(dataStart);
          }
      }
      else /* Frames with tagged Match */
      {

        innerVlanPos = L7_ENET_HDR_SIZE+4;
        /* Remove the Outer VLAN info */
        if (svType != L7_DOT1AD_SVCTYPE_TLS)
        {
          memmove(dataStart+12,dataStart+16,dataLength-16);
          dataLength -= sizeof(L7_8021QTag_t);
          SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, dataLength);
          innerVlanPos = L7_ENET_HDR_SIZE;
        }
        else
        {
          dataStart += L7_ENET_HDR_SIZE; /* CPU receives always 0x8100 as the outer TPID */
          SNOOP_PUT_SHORT(dvlanEthType,dataStart);
        }

        SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, dataStart);

        if (subscrEntry->svidToMatch > 0)
        {
          /* CPU receives always 0x8100 as the outer TPID */
          /* So setting the TPID to the configured values */
          memmove(dataStart+16,dataStart+12,dataLength-12);
          dataStart += L7_ENET_HDR_SIZE; /* CPU receives always 0x8100 as the outer TPID */
          SNOOP_PUT_SHORT(dvlanEthType,dataStart);
          tmpVid = subscrEntry->svidToMatch;
          SNOOP_PUT_SHORT(tmpVid,dataStart);
          dataLength -= sizeof(L7_8021QTag_t);
          SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, dataLength);
          SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, dataStart);
          innerVlanPos = L7_ENET_HDR_SIZE+4;
        }
        SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, dataStart);

        if (subscrEntry->cvidToMatch > 0)
        {
          memmove(dataStart+innerVlanPos+4,dataStart+innerVlanPos,dataLength-innerVlanPos);
          dataStart += innerVlanPos;
          /* Inner VLAN ID should be always 0x8100 */
          dvlanEthType = L7_8021Q_VLAN_PROTOCOL_ID;
          SNOOP_PUT_SHORT(dvlanEthType,dataStart);
          tmpVid = subscrEntry->cvidToMatch;;
          SNOOP_PUT_SHORT(tmpVid,dataStart);
          SNOOP_UNUSED_PARAM(dataStart);
          dataLength += sizeof(L7_8021QTag_t);
          SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, dataLength);
        }
        else if ( ( (subscrEntry->svidToMatch > 0)||
                    (svType == L7_DOT1AD_SVCTYPE_TLS)
                   ) &&
                  (innerVIDUntagged > 0) &&
                  (svType != L7_DOT1AD_SVCTYPE_ETREE)
                ) /* To handle match on SVID alone */
        {
          if (innerVlanPos == (L7_ENET_HDR_SIZE + 4))
          {
            memmove(dataStart+innerVlanPos+4,dataStart+innerVlanPos,dataLength-innerVlanPos);
          }
          dataStart += innerVlanPos;
          /* Inner VLAN ID should be always 0x8100 */
          dvlanEthType = L7_8021Q_VLAN_PROTOCOL_ID;
          SNOOP_PUT_SHORT(dvlanEthType,dataStart);
          tmpVid = innerVIDUntagged;
          SNOOP_PUT_SHORT(tmpVid,dataStart);
          SNOOP_UNUSED_PARAM(dataStart);
          if (innerVlanPos == (L7_ENET_HDR_SIZE+4))
          {
            dataLength += sizeof(L7_8021QTag_t);
            SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, dataLength);
          }
        }
      }
    }
    else
    {
      SYSAPI_NET_MBUF_FREE(bufHandle);
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}
/**********************************************************************
* @purpose Modify packet by inserting or re-marking the innerVlanId and
*          insert outterVlanid according to the service
*
* @param   vlanId    @b{(input)} Outter VLANID to which pkt is sent
* @param   bufHandle @b{(input)} Handle to packet data
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   This function will send to all the possible subscriptions for
*          the specified vlanId, member interfaces combination
*
* @end
*
*********************************************************************/
L7_RC_t snoopPacketFloodInsertInnerTag(L7_uint32 vlanId,
                                       L7_netBufHandle bufHandle)
{
  NIM_INTF_MASK_t maskBuf, reverseMaskBuf;
  L7_uint32       i, maskSize, numPorts;
  L7_uint32       intfList[L7_MAX_INTERFACE_COUNT + 1];

  /* Get list of all ports in the VLAN */
  memset((L7_uchar8 *)&reverseMaskBuf, 0, sizeof(reverseMaskBuf));
  if (dot1qVlanCurrentEgressPortsGet(vlanId,
                                    (NIM_INTF_MASK_t *)&reverseMaskBuf,
                                     &maskSize) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Since dot1q returns the mask reversed for SNMP, we need to reverse it again */
  memset((L7_uchar8 *)&maskBuf, 0, sizeof(maskBuf));
  nimReverseMask(reverseMaskBuf, &maskBuf);

  /* Convert mask to a list of interfaces */
  memset((L7_uchar8 *)intfList, 0, sizeof(intfList));
  if (nimMaskToList(&maskBuf, intfList, &numPorts) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Forward on all interfaces in the VLAN */
  for (i = 0; i < numPorts; i++)
  {
    if(snoopBuildInsertInnerTagFrameTransmit(intfList[i], vlanId, bufHandle) != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intfList[i], L7_SYSNAME, ifName);

      L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_SNOOPING_COMPONENT_ID,
              "snoopBuildInsertInnerTagFrameTransmit() failed for interface index:%d, %s vlan:%d",intfList[i], ifName, vlanId);
    }
  } /* iterate throught the egress port list */

  return L7_SUCCESS;
}
#endif
/*********************************************************************
* @purpose  Get Multicast MAC address for a specified IP address
*
* @param    ipAddr  @b{(input)} IP address
* @param    macAddr @b{(input)} Multicast MAC Address
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void snoopMulticastMacFromIpAddr(L7_inet_addr_t *ipAddr, L7_uchar8 *macAddr)
{
  L7_uint32  ipv4Addr;
  L7_uchar8  ip6Addr[L7_IP6_ADDR_LEN];

  if (ipAddr->family == L7_AF_INET)
  {
    if (inetAddressGet(L7_AF_INET, ipAddr, &ipv4Addr) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
             "snoopMulticastMacFromIpAddr: Invalid IPv4Address");
      memset(macAddr, 0x00, L7_MAC_ADDR_LEN);
      return;
    }

    macAddr[0] = 0x01;
    macAddr[1] = 0x00;
    macAddr[2] = 0x5E;
    macAddr[3] = (ipv4Addr & 0x007f0000) >> 16;
    macAddr[4] = (ipv4Addr & 0x0000ff00) >> 8;
    macAddr[5] = (ipv4Addr & 0x000000ff);
  }
  else if (ipAddr->family == L7_AF_INET6)
  {
    /* MLD */
    macAddr[0] = 0x33;
    macAddr[1] = 0x33;

    if (inetAddressGet(L7_AF_INET6, ipAddr, ip6Addr) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
             "snoopMulticastMacFromIpAddr: Invalid IPv6Address");
      memset(macAddr, 0x00, L7_MAC_ADDR_LEN);
      return;
    }
    memcpy(&macAddr[2], &ip6Addr[12], 4);
  }
  else
  {
    memset(macAddr, 0x0, L7_MAC_ADDR_LEN);
  }
}

/*********************************************************************
* @purpose  Convert inet addr to a string
*
* @param   addr   @b{(input)} inetAddress
* @param   string @b{(input)} Output String
*
* @returns  L7_FAILURE
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*
*********************************************************************/
void snoopInetHtoP(L7_inet_addr_t *addr, L7_uchar8 *string)
{
  L7_char8 *a;

  if (addr == L7_NULLPTR || string == L7_NULLPTR)
  {
    return;
  }
  if (addr->family == L7_AF_INET)
  {
    a = (L7_char8 *)&addr->addr.ipv4.s_addr;
    sprintf(string, "%u.%u.%u.%u", a[0], a[1], a[2], a[3]);
  }
  else if (addr->family == L7_AF_INET6)
  {
    osapiInetNtop(L7_AF_INET6,(L7_uchar8 *)&addr->addr.ipv6,string,
                  SNOOP_IP6_ADDR_BUFF_SIZE);
  }
  else
  {
    sprintf(string, "Wrong-family-addr");
  }
  return;
}

/*********************************************************************
* @purpose  Compute the checksum
*
* @param    addr   @b{(input)}  Pointer to the data on which check sum
*                               needs to be computed
* @param    len    @b{(input)}  Length of the data
* @param    csum   @b{(input)}  Initial checksum value
*
* @returns  Computed check sum

* @notes    none
*
* @end
*********************************************************************/
L7_ushort16 snoopCheckSum(L7_ushort16 *addr, L7_ushort16 len, L7_ushort16 csum)
{
  register L7_uint32 nleft = len;
  const    L7_ushort16 *w = addr;
  register L7_ushort16 answer;
  register L7_uint32 sum = csum;

  /*  Our algorithm is simple, using a 32 bit accumulator (sum),
   *  we add sequential 16 bit words to it, and at the end, fold
   *  back all the carry bits from the top 16 bits into the lower
   *  16 bits.
   */
  while (nleft > 1)
  {
    sum += *w++;
    nleft -= 2;
  }


  if (nleft == 1)
  {
    sum += osapiHtons(*(L7_uchar8 *)w << 8);
  }

  sum = (sum >> 16) + (sum & 0xffff);     /* add hi 16 to low 16 */
  sum += (sum >> 16);                     /* add carry */
  answer = ~sum;                          /* truncate to 16 bits */

  return(answer);
}

/*********************************************************************
* @purpose  Get the configured value after checking the precedence
*           between vlan configured and interface configured value
*
* @param    vlanId    @b{(input)}  VLAN ID
* @param    intIfNum  @b{(input)}  Internal Interface Number
* @param    param     @b{(input)}  Snoop Configuration paramter
* @param    family     @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                   L7_AF_INET6 => MLD Snooping
*
* @returns  param value

* @notes    none
*
* @end
*********************************************************************/
L7_uint32 snoopCheckPrecedenceParamGet(L7_uint32 vlanId, L7_uint32 intIfNum,
                                       snoop_params_t param, L7_uchar8 family)
{
  /* PTin removed: IGMP snooping */
  #if 0
  L7_uchar8        charValue;
  L7_uint32        vlanValue, intfValue, fdValue;
  L7_ushort16      shortValue;
  snoopOperData_t *pSnoopOperEntry;
  snoop_cb_t      *pSnoopCB;

  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return 0;
  }

  switch (param)
  {
    case SNOOP_PARAM_GROUP_MEMBERSHIP_INTERVAL:
      fdValue = (family == L7_AF_INET)? FD_IGMP_SNOOPING_GROUP_MEMBERSHIP_INTERVAL
        :FD_MLD_SNOOPING_GROUP_MEMBERSHIP_INTERVAL;
      vlanValue = pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].groupMembershipInterval;

      if (snoopIntfGroupMembershipIntervalGet(intIfNum, &intfValue, family) != L7_SUCCESS)
      {
        intfValue = fdValue;
      }
      break;

    case SNOOP_PARAM_MAX_RESPONSE_TIME:
      if ((pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_EXACT))
          != L7_NULLPTR)
      {
        if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperState == SNOOP_QUERIER_QUERIER)
        {
          /* Always used VLAN configured value when in querier mode */
          vlanValue = pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].maxResponseTime;
        }
        else
        {
          if (family == L7_AF_INET) /* IGMP */
          {
            if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperVersion == SNOOP_IGMP_VERSION_3)
            {
              charValue = pSnoopOperEntry->snoopQuerierInfo.maxResponseCode;
              snoopFPDecode(charValue, &shortValue);
              shortValue = SNOOP_MAXRESP_INTVL_ROUND(shortValue,SNOOP_IGMP_FP_DIVISOR);
              return shortValue;
            }
            else if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperVersion == SNOOP_IGMP_VERSION_2)
            {
              shortValue = pSnoopOperEntry->snoopQuerierInfo.maxResponseCode;
              shortValue = SNOOP_MAXRESP_INTVL_ROUND(shortValue,SNOOP_IGMP_FP_DIVISOR);
              return shortValue;
            }
          }
          else /* MLD */
          {
            /* MLD v 2 */
            if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperVersion == SNOOP_MLD_VERSION_2)
            {
              snoopMLDFPDecode(pSnoopOperEntry->snoopQuerierInfo.maxResponseCode,
                               &vlanValue);
              vlanValue = SNOOP_MAXRESP_INTVL_ROUND(vlanValue,SNOOP_MLD_FP_DIVISOR);
              return vlanValue;
            }
            else if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperVersion == SNOOP_MLD_VERSION_1)
            {
              vlanValue = pSnoopOperEntry->snoopQuerierInfo.maxResponseCode;
              vlanValue = SNOOP_MAXRESP_INTVL_ROUND(vlanValue,SNOOP_MLD_FP_DIVISOR);
              return vlanValue;
            }
          }/* End of MLD Check */
        }/* End of non-querier check */
      }/* end of vlan existing check */
      fdValue = (family == L7_AF_INET)? FD_IGMP_SNOOPING_MAX_RESPONSE_TIME
        :FD_MLD_SNOOPING_MAX_RESPONSE_TIME;
      if (snoopIntfResponseTimeGet(intIfNum, &intfValue, family) != L7_SUCCESS)
      {
        intfValue = fdValue;
      }

      vlanValue = pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].maxResponseTime;
      break;

    case SNOOP_PARAM_MCAST_RTR_EXPIRY_TIME:
      fdValue = (family == L7_AF_INET)? FD_IGMP_SNOOPING_MCAST_RTR_EXPIRY_TIME
        :FD_MLD_SNOOPING_MCAST_RTR_EXPIRY_TIME;

      if (snoopIntfMcastRtrExpiryTimeGet(intIfNum, &intfValue, family) != L7_SUCCESS)
      {
        intfValue = fdValue;
      }

      vlanValue = pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].mcastRtrExpiryTime;
      break;

    default:
      return 0;
  }

  if ((vlanValue != fdValue ) &&  (intfValue != fdValue))
  {
    return vlanValue;
  }
  else if (vlanValue == fdValue)
  {
    return intfValue;
  }
  else if (intfValue == fdValue)
  {
    return vlanValue;
  }
  return fdValue;
  #endif

  /* PTin added: IGMP snooping */
  #if 1
  ptin_IgmpProxyCfg_t igmpCfg;

  if (ptin_igmp_proxy_config_get(&igmpCfg)!=L7_SUCCESS)
  {
    return 0;
  }

  switch ((L7_uint8) param)
  {
    case SNOOP_PARAM_GROUP_MEMBERSHIP_INTERVAL:
      return igmpCfg.querier.group_membership_interval;
      break;

    case SNOOP_PARAM_MAX_RESPONSE_TIME:
      return igmpCfg.querier.query_response_interval;
      break;

    case SNOOP_PARAM_MCAST_RTR_EXPIRY_TIME:
      return FD_IGMP_SNOOPING_MCAST_RTR_EXPIRY_TIME;
      break;
  }
  return 0;
  #endif
}

/*********************************************************************
* @purpose   Decode char code into equivalent integer value
*
* @param     charCode  @b{(input)}   Integer Value interpreted as a
*                                    char value
* @param     val       @b{(output)}  Equivalent integer value
*
* @returns   void
*
* @comments  As per RFC 3376
*
* @end
*********************************************************************/
void snoopFPDecode(L7_uchar8 charCode, L7_short16 *val)
{
  if ((charCode & 0x80) == 0)       /* RFC 3376 */
  {
    *val = charCode;
  }
  else
  {
    *val = (((charCode & 0x0F) | 0x10) << (((charCode & 0x70) >> 4) + 3)) ;
  }
}

/*********************************************************************
* @purpose   Decode 16 bit code into equivalent integer value
*
* @param     shortCode  @b{(input)}   Integer Value interpreted as a
*                                    char value
* @param     val       @b{(output)}  Equivalent integer value
*
* @returns   void
*
* @comments   As per RFC 3810
*
* @end
*********************************************************************/
void snoopMLDFPDecode(L7_ushort16 shortCode, L7_uint32 *val)
{
  if ((shortCode & 0x8000) == 0)   /* RFC 3810 */
  {
    *val = shortCode;
  }
  else
  {
    *val = ((shortCode & 0x0FFF)| 0X1000) << (((shortCode & 0x7000) >> 12) + 3);
  }
}

/*********************************************************************
* @purpose   Get the endianess of the platform.
*
* @param     void
*
* @returns   0 for little endian, 1 for big endian
*
* @comments   Used with snoopCheckSum
*
* @end
*********************************************************************/
L7_uchar8 snoopGetEndianess(void)
{
    L7_ushort16 endian = 0xaabb;
    L7_uchar8 * byte;

    byte = (L7_uchar8 *)&endian;

    if(*byte == 0xbb)
        return SNOOP_LITTLE_ENDIAN;
    else
    return SNOOP_BIG_ENDIAN;
}


