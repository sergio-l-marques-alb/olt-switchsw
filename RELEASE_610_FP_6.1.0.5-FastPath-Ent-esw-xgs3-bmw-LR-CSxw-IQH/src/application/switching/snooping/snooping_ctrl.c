/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   snooping_ctrl.c
*
* @purpose    
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
#include "dot1s_api.h"

#include "snooping_api.h"
#include "snooping_util.h"
#include "snooping_outcalls.h"
#include "snooping_ctrl.h"
#include "snooping_proto.h"
#include "snooping_db.h"
#include "snooping_debug.h"
#include "ptin_translate_api.h"
#include "dtlinclude.h"

/*****************************************************************************
  Local Functions
 ****************************************************************************/
/* Operational mode apply processing */

static L7_RC_t snoopIntfModeApply(L7_uint32 intIfNum, L7_uint32 mode, 
                                  snoop_cb_t *pSnoopCB);
static L7_RC_t snoopVlanMrouterApply(L7_uint32 intIfNum, L7_uint32 vlanId, 
                                     L7_uint32 mrouter, snoop_cb_t *pSnoopCB);
static L7_RC_t snoopIntfMrouterApply(L7_uint32 intIfNum, L7_uint32 mrouter,
                                     snoop_cb_t *pSnoopCB);
static void snoopVlanModeChangeProcess(L7_uint32 vlanId, L7_uint32 mode, 
                                       snoop_cb_t *pSnoopCB);
static void snoopVlanMrouterModeChangeProcess(L7_uint32 vlanId, 
                                              L7_uint32 intIfNum,
                                              L7_uint32 mode, 
                                              snoop_cb_t *pSnoopCB);
static L7_RC_t snoopIntfVlanEntriesRemove(L7_uint32 intIfNum, L7_uint32 vlanId, 
                                  snoop_cb_t *pSnoopCB);

/* Interface Callback processing */
static void snoopIntfChangeProcess(L7_uint32 intIfNum, L7_uint32 event, 
                                   NIM_CORRELATOR_t correlator);

static L7_RC_t snoopIntfCreate(L7_uint32 intIfNum);
static L7_RC_t snoopIntfDelete(L7_uint32 intIfNum);
static L7_RC_t snoopIntfAttach(L7_uint32 intIfNum);
static L7_RC_t snoopIntfDetach(L7_uint32 intIfNum);
static L7_RC_t snoopIntfActive(L7_uint32 intIfNum);
static L7_RC_t snoopIntfInActive(L7_uint32 intIfNum);
/* Dot1Q call back processing */
static L7_RC_t snoopVlanChangeProcess(dot1qNotifyData_t *vlanData, L7_uint32 intIfNum, 
                               L7_uint32 event);
static L7_RC_t snoopVlanDelete(L7_uint32 vlanId);
static L7_RC_t snoopVlanEntriesRemove(L7_uint32 vlanId, snoop_cb_t *pSnoopCB);
static L7_RC_t snoopVlanPortAdd(L7_uint32 vlanId, L7_uint32 intIfNum);
static L7_RC_t snoopVlanPortDelete(L7_uint32 vlanId, L7_uint32 intIfNum);
static L7_RC_t snoopAllVlanDelete(void);
/* Dot1S call back processing */
static L7_RC_t snoopDot1sIntfChangeProcess(L7_uint32 mstID, L7_uint32 intIfNum, 
                                           L7_uint32 event);
static L7_RC_t snoopDot1sForwarding(L7_uint32 intIfNum, L7_ushort16 *vidList,
                                    L7_uint32 numVlans);
static L7_RC_t snoopDot1sNotForwarding(L7_uint32 intIfNum, L7_ushort16 *vidList,
                                       L7_uint32 numVlans);
static L7_RC_t snoopDot1sTCNProcess(L7_uint32 mstID, L7_ushort16 *vidList,
                                    L7_uint32 numVlans);
/* L3 Mcas Notify processing */
static L7_RC_t snoopL3McastAddNotifyProcess(L7_inet_addr_t *mcastGroupAddr, 
                                            L7_inet_addr_t *mcastSrcAddr, 
                                            L7_int32        srcVlan,
                                            L7_VLAN_MASK_t *vlanMask);
static L7_RC_t snoopL3McastDeleteNotifyProcess(L7_inet_addr_t *mcastGroupAddr, 
                                               L7_inet_addr_t *mcastSrcAddr);
#ifdef L7_MCAST_PACKAGE
static  void snoopL3McastLagPortRelease(L7_uint32 intIfNum);
static  void snoopL3McastVlanPortAdd(L7_uint32 vlanId, snoop_cb_t *pSnoopCB);
#endif

static L7_uchar8 snoopMsgQueueSchedule();


#define SNOOP_SCHEDULE_NONE       0
#define SNOOP_SCHEDULE_IGMP_ONLY  2
#define SNOOP_SCHEDULE_MLD_ONLY   1
#define SNOOP_SCHEDULE_BOTH       3

/*********************************************************************
* @purpose  Based on the supported snoop instance this routine will
*           specify which snoop pdu queue is to be scheduled
*           
*
* @param    none
*
* @returns  SNOOP_SCHEDULE_NONE      No snoop instances are supported
* @returns  SNOOP_SCHEDULE_IGMP_ONLY IGMP Snooping instance queue is 
*                                    scheduled to execute
* @returns  SNOOP_SCHEDULE_MLD_ONLY  MLD Snooping instance queue is 
*                                    scheduled to execute
*
* @notes    When both IGMP and MLD snoop instances are operational
*           then the result will alternate between IGMP and MLD
*
* @end
*********************************************************************/
static L7_uchar8 snoopMsgQueueSchedule()
{
  static L7_BOOL isFirstTime = L7_TRUE;
  static L7_uchar8 buck, schdlType;
  snoop_eb_t  *pSnoopEB;

  if (isFirstTime == L7_TRUE)
  {
    isFirstTime = L7_FALSE;

    pSnoopEB = snoopEBGet();
    if (pSnoopEB->maxSnoopInstances == 0)
    {
      buck = schdlType = SNOOP_SCHEDULE_NONE;
    }
    else if (pSnoopEB->maxSnoopInstances == 1)
    {
      if (pSnoopEB->snoopIGMPQueue != L7_NULLPTR)
      {
        buck      = SNOOP_SCHEDULE_IGMP_ONLY;
        schdlType = SNOOP_SCHEDULE_IGMP_ONLY;
      }
      else
      {
        schdlType = SNOOP_SCHEDULE_MLD_ONLY;
        buck      = SNOOP_SCHEDULE_MLD_ONLY;
      }
    }
    else
    {
      schdlType = SNOOP_SCHEDULE_BOTH;
      buck      = SNOOP_SCHEDULE_IGMP_ONLY;
    }
  }

  switch (schdlType)
  {                
    case SNOOP_SCHEDULE_BOTH:
      buck = (~(buck ^ 0)) & (3);
      break;
    case SNOOP_SCHEDULE_MLD_ONLY:
    case SNOOP_SCHEDULE_IGMP_ONLY:
    case SNOOP_SCHEDULE_NONE:
      break;
    default:
      break;
  } 
  return buck;
}

/*********************************************************************
* @purpose   Task to handle all Snooping management & PDU messages
*
* @param void
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
void snoopTask(void)
{
  snoopMgmtMsg_t     msg;
  snoopTimerParams_t timerMsg;
  snoopPDU_Msg_t     pduMsg;
  L7_RC_t            rc;
  snoop_eb_t        *pSnoopEB;
  L7_uint32          token, idx;
  L7_BOOL            isPduMsg = L7_FALSE, isTimerMsg = L7_FALSE;
  L7_uint32          msgId;

  pSnoopEB = snoopEBGet();
  osapiTaskInitDone(L7_IGMP_SNOOPING_TASK_SYNC);

  do
  {
    /* Queue's sync sema */
    osapiSemaTake(pSnoopEB->snoopMsgQSema, L7_WAIT_FOREVER);
    isPduMsg = L7_FALSE;
    isTimerMsg = L7_FALSE;
    rc = osapiMessageReceive(pSnoopEB->snoopQueue, (void *)&msg, SNOOP_MSG_SIZE, 
                             L7_NO_WAIT);

    if (rc != L7_SUCCESS)
    {
      rc = osapiMessageReceive(pSnoopEB->snoopTimerQueue, (void *)&timerMsg, SNOOP_TIMER_MSG_SIZE,
                               L7_NO_WAIT);
      if (rc != L7_SUCCESS)
      {

     idx = pSnoopEB->maxSnoopInstances; 
      if (idx)
      {
        do
        {
          token = snoopMsgQueueSchedule();
          if (token == SNOOP_SCHEDULE_IGMP_ONLY)
          { /* IGMP PDU Queue */
            rc = osapiMessageReceive(pSnoopEB->snoopIGMPQueue, 
                                     (void *)&pduMsg, SNOOP_PDU_MSG_SIZE, 
                                     L7_NO_WAIT);
          }
          if (token == SNOOP_SCHEDULE_MLD_ONLY)
          {  /* MLD PDU Queue */
            rc = osapiMessageReceive(pSnoopEB->snoopMLDQueue, 
                                     (void *)&pduMsg, SNOOP_PDU_MSG_SIZE, 
                                     L7_NO_WAIT);
          }
          if (rc == L7_SUCCESS)
          {
            isPduMsg = L7_TRUE;
            break; /* Got one message */
          }
          idx--;
        } while(idx > 0);
      }
      if (rc != L7_SUCCESS)
      { 
        LOG_MSG("snoopTask: Queues out of sync\n");
        continue;
      }
    }
     else
     {
       isTimerMsg = L7_TRUE;
     }
    }
    if (isPduMsg == L7_TRUE)
    {
      msgId = pduMsg.msgId;
    }
    else if (isTimerMsg == L7_FALSE)
    {
      msgId = msg.msgId;
    }
    else
    {
     msgId = snoopMsgTimerTick;
    }
    
    switch (msgId)
    {
    case (snoopCnfgr):
      snoopCnfgrParse(&msg.u.CmdData);
      break;

    case (snoopIntfChange):
      /* Process interface change event notfied by NIM */
      snoopIntfChangeProcess(msg.intIfNum, 
                             msg.u.snoopIntfChangeParms.event, 
                             msg.u.snoopIntfChangeParms.correlator);
      break;

    case (snoopAdminModeChange):
      /* Enable Snooping Admin Mode */
      (void)snoopAdminModeApply(msg.u.mode.admin, msg.intIfNum, msg.u.mode.vlanId, msg.u.mode.CoS_intPrio, msg.cbHandle);
      break;

    case (snoopIntfModeChange):
      /* Enable Snooping Admin Mode */
      (void)snoopIntfModeApply(msg.intIfNum, msg.u.mode.admin, msg.cbHandle);
      break;

    case (snoopIntfMrouterModeChange):
      /* Interface is made a static multicast router */
      (void)snoopIntfMrouterApply(msg.intIfNum, msg.u.mode.admin, msg.cbHandle);
      break;

    case (snoopVlanChange):
      /* Vlan change notification */
      (void)snoopVlanChangeProcess(&msg.u.vlanData, msg.intIfNum, 
                                   msg.vlanEvent);
      break;

    case (snoopVlanModeChange):
      /* Enable VLAN Snooping Mode */
      snoopVlanModeChangeProcess(msg.vlanId, msg.u.mode.admin, msg.cbHandle);
      break;

    case (snoopVlanMrouterModeChange):
     /* To enable/disable static mcast router attached on an interface/vlan */
       snoopVlanMrouterModeChangeProcess(msg.vlanId, msg.intIfNum, msg.u.mode.admin,
                                         msg.cbHandle);
       break;
    case snoopDot1sIntfChange:
      /* dot1s notification */
      (void)snoopDot1sIntfChangeProcess(msg.u.snoopDot1sIntfChangeParms.mstID, 
                                        msg.intIfNum, 
                                        msg.u.snoopDot1sIntfChangeParms.event);
      break;
    case (snoopMsgTimerTick):
     /* Trigger the expired timers */
       snoopTimerProcess(timerMsg.timerCBHandle, pSnoopEB);
       break;

    case snoopPDUMsgRecv:
        /* PDU Receive */
       snoopPacketProcess(&pduMsg);
       break;

    case (snoopFlush):
        /* Clear snoop entries */
        snoopEntriesDoFlush(msg.cbHandle);
      break;

    case   snoopL3McastAdd:
           snoopL3McastAddNotifyProcess(&msg.u.mcastChangeParams.mcastGroupAddr,
                                        &msg.u.mcastChangeParams.mcastSrcAddr,
                                         msg.u.mcastChangeParams.srcIntfVlan, 
                                        &msg.u.mcastChangeParams.outVlanRtrIntfMask);
           break;

    case   snoopL3McastDelete:
           snoopL3McastDeleteNotifyProcess(&msg.u.mcastChangeParams.mcastGroupAddr,
                                           &msg.u.mcastChangeParams.mcastSrcAddr);
           break;

     /* Snooping querier messages */
    case snoopQuerierModeChange:
    case snoopQuerierVlanModeChange:
    case snoopQuerierAddressChange:
    case snoopQuerierVlanAddressChange:
    case snoopQuerierNetworkAddressChange:
    case snoopQuerierVersionChange:
    case snoopQuerierVlanElectionModeChange:
       snoopQuerierProcessMessage(&msg);
       break;
    default:
      LOG_MSG("snoopTask(): invalid message type:%d. %s:%d\n", msg.msgId,
               __FILE__, __LINE__);
      break;
    }
    
  } while (1);
}

/*********************************************************************
* @purpose  Remove interface from all Snooping entries and from multicast 
*           router list(dynamically learnt entries only) of a 
*           snoop instance
*
* @param    intIfNum     @b{(input)} Internal interface number
* @param    vlanId       @b{(input)} VLAN ID
* @param    pSnoopCB     @b{(input)} Control Block
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t snoopIntfVlanEntriesRemove(L7_uint32 intIfNum, L7_uint32 vlanId, 
                                          snoop_cb_t *pSnoopCB)
{
  L7_RC_t           rc;
  L7_uchar8         macAddr[L7_MAC_ADDR_LEN];
  L7_uint32         vid;
  L7_INTF_MASK_t    snoopMemberList;
  /* First remove all the static mrouters. It is important to this first
     as the mfdb membership is not removed if there is a mrouter attached on 
     the interface. We can as well give a snoopIntfRemove with interface type
     as SNOOP_MCAST_ROUTER_ATTACHED, but for that we will have to traverse
     the entire snoop tree again. For better performance and same result we do
     this.
   */
  snoopMcastRtrListRemove(vlanId, intIfNum , SNOOP_DYNAMIC, pSnoopCB);

  /* Secondly remove the group interface from all the snoop entries with 
     this vlanId
   */
  memset((void *)macAddr, 0x00, L7_MAC_ADDR_LEN);
  rc = snoopFirstGet(macAddr, &vid, pSnoopCB->family);
  while (rc == L7_SUCCESS)
  {
    if (snoopIntfListGet(macAddr, vid, &snoopMemberList, pSnoopCB->family) 
                         == L7_SUCCESS)
    {
      if (vid == vlanId && (L7_INTF_ISMASKBITSET(snoopMemberList, intIfNum)))
      {
       /* The interface may have received reports on the same intf, vlan */
        snoopIntfRemove(macAddr, vid, intIfNum, SNOOP_GROUP_MEMBERSHIP, 
                        pSnoopCB);
      }
    }
    rc = snoopNextGet(macAddr, vid, macAddr, &vid, pSnoopCB->family);
  }

  return L7_SUCCESS;
}

// PTin added
L7_RC_t snoopIntfVlanStaticEntriesAdd(L7_uint32 intIfNum, L7_uint32 vlanId, snoop_cb_t *pSnoopCB)
{
  L7_RC_t           rc;
  L7_uchar8         macAddr[L7_MAC_ADDR_LEN];
  L7_uint32         vid;
  L7_INTF_MASK_t    snoopMemberList;
  snoopInfoData_t  *snoopEntry = L7_NULLPTR;

  //printf("%s(%d) intf=%u, vlanId=%u\r\n",__FUNCTION__,__LINE__,intIfNum,vlanId);

  /* Secondly remove the group interface from all the snoop entries with 
     this vlanId
   */
  memset((void *)macAddr, 0x00, L7_MAC_ADDR_LEN);
  rc = snoopFirstGet(macAddr, &vid, pSnoopCB->family);
  while (rc == L7_SUCCESS)
  {
    if (snoopIntfListGet(macAddr, vid, &snoopMemberList, pSnoopCB->family) == L7_SUCCESS &&
        (snoopEntry = snoopEntryFind(macAddr, vid, pSnoopCB->family, L7_MATCH_EXACT)) != L7_NULLPTR )
    {
      // Only add this interface for static entries
      if (vid == vlanId && (!L7_INTF_ISMASKBITSET(snoopMemberList, intIfNum)) && snoopEntry->staticEntry)
      {
        if (vlanId>0 && vlanId<4096 && snoop_mcast_vlan[vlanId]>0 && snoop_mcast_vlan[vlanId]<4096 &&
            ptin_intif_vlan_translate_get(intIfNum, vlanId, L7_NULLPTR) == L7_SUCCESS &&
            ptin_intif_vlan_translate_get(intIfNum, snoop_mcast_vlan[vlanId], L7_NULLPTR) == L7_SUCCESS)
        {  
         /* The interface may have received reports on the same intf, vlan */
          snoopIntfAdd(macAddr, vid, intIfNum, SNOOP_GROUP_MEMBERSHIP, pSnoopCB);
        }
      }
    }
    rc = snoopNextGet(macAddr, vid, macAddr, &vid, pSnoopCB->family);
  }

  return L7_SUCCESS;
}

// PTin added
L7_RC_t snoopVlanAdminModeApply(L7_uint32 adminMode, L7_uint32 intf, L7_uint16 vlan_id, L7_uint8 prio)
{
  return snoopIGMPAdminModeApply(adminMode & 1, intf, vlan_id, prio);
}
// PTin end

/*********************************************************************
* @purpose  Applies the Snooping Admin mode of a snoop instance
*
* @param    adminMode   @b((input)) Snooping admin mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This function assumes adminMode is valid
*
* @end
*********************************************************************/
L7_RC_t snoopAdminModeApply(L7_uint32 adminMode, L7_uint32 intf, L7_uint16 vlan_id, L7_uint8 prio, snoop_cb_t *pSnoopCB)
{
  L7_uint32             intIfNum, cfgIndex, mode;
  L7_uint32             vlanId;
  nimConfigID_t         configIdNull;
  L7_RC_t               rc, result=L7_SUCCESS;
  snoopIntfCfgData_t   *pCfg = L7_NULLPTR;
  snoopOperData_t      *pSnoopOperEntry  = L7_NULLPTR;

  if (adminMode == L7_ENABLE)
  {
    /* Fill in the Vlan ID map by getting all the currently configured VLANs */

     /* Apply Snooping Mode in hardware */
      if (pSnoopCB->family == L7_AF_INET)
      {

        if ((result=snoopIGMPAdminModeApply(L7_ENABLE, intf, vlan_id, prio)) != L7_SUCCESS)
        {
          LOG_MSG("snoopAdminModeApply (Enable): snoopIGMPAdminModeApply failed\n");
          return result;
        }
      }
      else if (pSnoopCB->family == L7_AF_INET6)
      {
        if (snoopMLDAdminModeApply(L7_ENABLE) != L7_SUCCESS) 
        {
          LOG_MSG("snoopAdminModeApply (Enable): snoopMLDAdminModeApply failed\n");
          return L7_FAILURE;
        }
      }
      else
      {
        return L7_FAILURE;
      } 
    /* Enable all interfaces that are configured for Snooping.  This will set the
    ** operational bitmask and send an IGMP query on interfaces that can be enabled.
    */
    if (snoopIsReady() == L7_TRUE)
    {
      pSnoopOperEntry = snoopOperEntryFirstGet(pSnoopCB);
      while(pSnoopOperEntry)
      {
        vlanId = pSnoopOperEntry->vlanId;

        if ( vlan_id==0 || vlan_id==vlanId )    /* PTin modified: IGMP */
        {
          /* Start iterations of all interfaces */
          for (cfgIndex = 1; cfgIndex < L7_IGMP_SNOOPING_MAX_INTF; cfgIndex++)
          {
            if (NIM_CONFIG_ID_IS_EQUAL(&(pSnoopCB->snoopCfgData->snoopIntfCfgData[cfgIndex].configId), 
                                       &configIdNull))
            {
              continue;
            }
            if (nimIntIfFromConfigIDGet(&(pSnoopCB->snoopCfgData->snoopIntfCfgData[cfgIndex].configId), 
                                        &intIfNum) != L7_SUCCESS)
            {
              continue;
            }

            pCfg = &(pSnoopCB->snoopCfgData->snoopIntfCfgData[cfgIndex]);

            if ( (snoop_dot1qOperVlanMemberGet(vlanId, intIfNum, &mode) == L7_SUCCESS) 
                  &&(mode == L7_DOT1Q_FIXED)) 
            {
              /* Snoop operational mode */
              if (pCfg->intfMode & SNOOP_VLAN_MODE ||
                  pSnoopCB->snoopCfgData->snoopVlanMode[vlanId] & SNOOP_VLAN_MODE)
              {
                if (snoopIntfVlanModeApply(intIfNum, vlanId, L7_ENABLE, pSnoopCB) != L7_SUCCESS)
                {
                  LOG_MSG("snoopAdminModeApply: Failed to apply mode intIfNum %d vlan %d family %d",
                  intIfNum, vlanId, pSnoopCB->family);
                }
              }

              /* Snoop static mrouter (interface, vlan) mode */
              if (pCfg->intfMcastRtrAttached == L7_ENABLE ||
                  L7_VLAN_ISMASKBITSET(pCfg->vlanStaticMcastRtr, vlanId))
              {
                snoopVlanMrouterApply(intIfNum, vlanId, L7_ENABLE, pSnoopCB);
              }
            }
          } /* End of iterations of all interfaces */

          /* Snoop Querier */
          if (snoopQuerierVlanModeApply(vlanId, L7_ENABLE, pSnoopCB) != L7_SUCCESS)
          {
            LOG_MSG("snoopAdminModeApply: Failed to enabled querier vlan mode vlan %d family %d",
                    vlanId, pSnoopCB->family);
          }
        }

        pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, 
                                            L7_MATCH_GETNEXT);
      } /* End of iterations of all vlans */
    } /* SNOOP CNFGR STATE CHECK */
  }
  else
  {
    /* Apply Snooping Mode in hardware */
    if (pSnoopCB->family == L7_AF_INET)
    {
      if ((result=snoopIGMPAdminModeApply(L7_DISABLE, intf, vlan_id, prio)) != L7_SUCCESS) 
      {
        LOG_MSG("snoopAdminModeApply (Disable): snoopIGMPAdminModeApply failed\n");
        return result;
      }
    }
    else if (pSnoopCB->family == L7_AF_INET6)
    {
      if (snoopMLDAdminModeApply(L7_DISABLE) != L7_SUCCESS) 
      {
        LOG_MSG("snoopAdminModeApply (Disable): snoopMLDAdminModeApply failed\n");
        return L7_FAILURE;
      }
    }
    else
    {
      return L7_FAILURE;
    }

    /* PTin modified: IGMP */
    if (vlan_id == 0)
    {
      /* Flush all Snooping entries */
      snoopEntriesDoFlush(pSnoopCB);
    }
    else
    {
      /* PTin added: IGMP */
      snoopEntryDoFlush(vlan_id,pSnoopCB);
    }

    if (nimPhaseStatusCheck() == L7_TRUE)
    {
     /* Clear operational bitmask */
      pSnoopOperEntry = snoopOperEntryFirstGet(pSnoopCB);
      while(pSnoopOperEntry)
      {
        vlanId = pSnoopOperEntry->vlanId;

        if ( vlan_id==0 || vlan_id==vlanId )    /* PTin added: IGMP */
        {
          rc = nimFirstValidIntfNumber(&intIfNum);
          while (rc == L7_SUCCESS)
          {
            if (snoopIsValidIntf(intIfNum) == L7_TRUE)
            {
              L7_INTF_CLRMASKBIT(pSnoopOperEntry->snoopIntfMode, intIfNum);
            }
            rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
          } /* End of interface iterations */

          if (snoopQuerierVlanModeApply(vlanId, L7_DISABLE, pSnoopCB) != L7_SUCCESS)
          {
            LOG_MSG("snoopAdminModeApply (Disable): Failed to enabled querier vlan mode vlan %d family %d\n",
                    vlanId, pSnoopCB->family);
          }

          /* De-Initialize the operational info */
          snoopOperEntryDeInit(pSnoopOperEntry);
        }
        pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_GETNEXT);
      } /* End of iterations of vlans */
    } /* End of NIM Phase Status check */
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Applies the Snooping mode for the specified interface in 
*           all VLANs
*
* @param    intIfNum  @b{(input)} Internal interface number
* @param    mode      @b{(input)} Snooping intf mode
* @param    pSnoopCB  @b{(input)} Control Block
*
* @returns  L7_SUCCESS  if interface mode was successfully applied
* @returns  L7_FAILURE  if interface number is invalid
*
* @notes    This function assumes mode parameter is valid
*
* @end
*********************************************************************/
static L7_RC_t snoopIntfModeApply(L7_uint32 intIfNum, L7_uint32 mode, 
                                  snoop_cb_t *pSnoopCB)
{
  L7_uint32          vlanId;
  L7_RC_t            rc = L7_SUCCESS;
  L7_VLAN_MASK_t  vidMask;
  snoopOperData_t   *pSnoopOperEntry  = L7_NULLPTR;

  /* If Snooping is not globally (admin) enabled, just return a success.  
   * We will come back through here for each configured interface when admin 
   * mode is enabled.
   */

  if (pSnoopCB->snoopCfgData->snoopAdminMode != L7_ENABLE)
  {
    return L7_SUCCESS;
  }

  if (snoop_dot1qVIDListMaskGet(intIfNum, &vidMask) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  pSnoopOperEntry = snoopOperEntryFirstGet(pSnoopCB);
  while(pSnoopOperEntry)
  {
    vlanId = pSnoopOperEntry->vlanId;
    if (L7_VLAN_ISMASKBITSET(vidMask, vlanId))
    {
      /* Don't disable interface if vlan mode is enabled */
      if (mode == L7_DISABLE && 
          (pSnoopCB->snoopCfgData->snoopVlanMode[vlanId] & SNOOP_VLAN_MODE))
      {
        pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_GETNEXT);
        continue;
      }
      
      if (snoopIntfVlanModeApply(intIfNum, vlanId, mode, pSnoopCB) != L7_SUCCESS)
      {
        LOG_MSG("snoopIntfModeApply: Failed to apply mode intIfNum %d vlan %d family %d",
                intIfNum, vlanId, pSnoopCB->family);
      }
    } /* End if vidmask is mask bit set */
    pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_GETNEXT);
  } /* End of VLAN iterations */

  return rc;
}

/*********************************************************************
* @purpose  Applies the Multicast Router Attached mode for the specified 
*           interface in the specified VLAN of a snoop instance
*
* @param    intIfNum  @b{(input)} Internal interface number
* @param    vlanId    @b{(input)} Vlan ID
* @param    mode      @b{(input)} Snooping intf mode
* @param    pSnoopCB  @b{(input)} Control Block
*
* @returns  L7_SUCCESS, if admin mode was successfully applied
* @returns  L7_FAILURE, if interface number is invalid
*
* @notes    This function assumes mode parameter is valid
*
* @end
*********************************************************************/
static L7_RC_t snoopVlanMrouterApply(L7_uint32 intIfNum, L7_uint32 vlanId, 
                                     L7_uint32 mrouter, snoop_cb_t *pSnoopCB)
{
  L7_RC_t          rc = L7_FAILURE;
  snoopOperData_t *pSnoopOperEntry  = L7_NULLPTR;
  L7_uint32        state = L7_INACTIVE;

  /* If Snooping is not globally (admin) enabled, just return a success.  We will come back
  ** through here for each configured interface when admin mode is enabled.
  */
  if (pSnoopCB->snoopCfgData->snoopAdminMode != L7_ENABLE)
  {
    return L7_SUCCESS;
  }

  if ((pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_EXACT)) 
                       == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (mrouter == L7_ENABLE) /* Mode is Enable */
  {
    if ((nimGetIntfActiveState(intIfNum, &state) != L7_SUCCESS) || 
        (state != L7_ACTIVE))
    {
      return L7_SUCCESS;
    }

    rc = snoopMcastRtrListAdd(vlanId, intIfNum, SNOOP_STATIC, pSnoopCB);
  }
  else /* Mode is Disable */
  {
    rc = snoopMcastRtrListRemove(vlanId, intIfNum, SNOOP_STATIC, pSnoopCB);
  } /* End of mode disable */
  return rc;
}

/*****************************************************************************
* @purpose  Applies Snooping operational mode for the specified interface and 
*           VLAN of a snoop instance
*
* @param    intIfNum  @b{(input)} Internal interface number
* @param    vlanId    @b{(input)} VLAN ID
* @param    mode      @b{(input)} Snooping intf mode
* @param    pSnoopCB  @b{(input)} Control Block
*
* @returns  L7_SUCCESS, if interface mode was successfully applied
* @returns  L7_FAILURE, if the interface number is invalid or VLAN ID was not 
*                       found in our map
*
* @notes    This function assumes mode parameter is valid and that the specified
*           intIfNum is a member of the specified VLAN
*
* @end
*******************************************************************************/
L7_RC_t snoopIntfVlanModeApply(L7_uint32 intIfNum, L7_uint32 vlanId, 
                               L7_uint32 mode, snoop_cb_t *pSnoopCB)
{
  snoopOperData_t *pSnoopOperEntry  = L7_NULLPTR;

  //printf("%s(%d) intf=%u, vlanId=%u, mode=%u\r\n",__FUNCTION__,__LINE__,intIfNum,vlanId,mode);

  if (pSnoopCB->snoopCfgData->snoopAdminMode != L7_ENABLE)
  {
    return L7_SUCCESS;
  }

  if (snoopIsValidIntf(intIfNum) != L7_TRUE)
  {
    LOG_MSG("snoopIntfVlanModeApply: snoopIsValidIntf failed for interface %d",
             intIfNum);
    return L7_FAILURE;
  } 

  if ((pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_EXACT)) 
                       == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (mode == L7_ENABLE)
  {
    // PTin added
    snoopIntfVlanStaticEntriesAdd(intIfNum, pSnoopOperEntry->vlanId, pSnoopCB);

    /* If interface is already enabled in this VLAN, don't enable again */
    if (L7_INTF_ISMASKBITSET(pSnoopOperEntry->snoopIntfMode, intIfNum))
    {
      return L7_SUCCESS;
    }

    if (snoopIntfCanBeEnabled(intIfNum, vlanId) == L7_TRUE)
    { 
      L7_INTF_SETMASKBIT(pSnoopOperEntry->snoopIntfMode, intIfNum);
      /* If Snooping is globally enabled, send IGMP general query on this interface */
    }
    else
    { 
      L7_INTF_CLRMASKBIT(pSnoopOperEntry->snoopIntfMode, intIfNum);
    }
  }
  else
  {
    /* If interface is already disabled in this VLAN, don't disable again */
    if ( !(L7_INTF_ISMASKBITSET(pSnoopOperEntry->snoopIntfMode, intIfNum)) )
    {
      return L7_SUCCESS;
    }
    
    /* Mode is disable... Remove this interface/VLAN ID from all entries */
    L7_INTF_CLRMASKBIT(pSnoopOperEntry->snoopIntfMode, intIfNum);

  /* Remove the interface from Multicast Router Attached List and snoopEntries*/
    snoopIntfVlanEntriesRemove(intIfNum, pSnoopOperEntry->vlanId, pSnoopCB);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Applies the Multicast Router Attached mode for the 
*           specified interface in all VLANs of a snoop instance
*
* @param    intIfNum  @b{(input)} Internal interface number
* @param    mrouter   @b{(input)} Snooping intf mode
* @param    pSnoopCB  @b{(input)} Control Block
*
* @returns  L7_SUCCESS, if admin mode was successfully applied
* @returns  L7_FAILURE, if interface number is invalid
*
* @notes    This function assumes mode parameter is valid
*
* @end
*********************************************************************/
static L7_RC_t snoopIntfMrouterApply(L7_uint32 intIfNum, L7_uint32 mrouter,
                                     snoop_cb_t *pSnoopCB)
{
  L7_uint32           mode, vlanId;
  snoopOperData_t    *pSnoopOperEntry  = L7_NULLPTR;
  snoopIntfCfgData_t *pCfg = L7_NULLPTR;
  L7_VLAN_MASK_t   vidMask;
  
  /* If Snooping is not globally (admin) enabled, just return a success.  We will come back
  ** through here for each configured interface when admin mode is enabled.
  */
  if (pSnoopCB->snoopCfgData->snoopAdminMode != L7_ENABLE)
  {
    return L7_SUCCESS;
  }

  if (snoop_dot1qVIDListMaskGet(intIfNum, &vidMask) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (mrouter == L7_ENABLE)
  {
    mode = L7_INACTIVE;
    if ((nimGetIntfActiveState(intIfNum, &mode) != L7_SUCCESS) || 
        (mode != L7_ACTIVE))
    {
      return L7_SUCCESS;
    }

    pSnoopOperEntry = snoopOperEntryFirstGet(pSnoopCB);
    while(pSnoopOperEntry)
    {
      vlanId = pSnoopOperEntry->vlanId;
  
     /* Skip vlan only if interface is not member of the VLAN */
     if (L7_VLAN_ISMASKBITSET(vidMask, vlanId) == 0)
     {
        pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_GETNEXT);
        continue;
      }
      
      snoopMcastRtrListAdd(vlanId, intIfNum, SNOOP_STATIC, pSnoopCB);
      pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_GETNEXT);

    } /* End of vlan iterations */
  }
  else /* Mode is disable*/
  {
    pSnoopOperEntry = snoopOperEntryFirstGet(pSnoopCB);
    while(pSnoopOperEntry)
    {
      vlanId = pSnoopOperEntry->vlanId;
      /* Verify that the interface has a multicast router attached */
      if (L7_INTF_ISMASKBITSET(pSnoopOperEntry->mcastRtrAttached, 
                               intIfNum) == L7_FALSE)
      {
        pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_GETNEXT);
        continue;
      }

      /* Verify if the interface is enabled on a per vlan basis */
      if (snoopMapIntfIsConfigurable(intIfNum, &pCfg, pSnoopCB) != L7_TRUE ||
          (L7_VLAN_ISMASKBITSET(pCfg->vlanStaticMcastRtr, vlanId)))
      {
        pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_GETNEXT);
        continue;
      }

      snoopMcastRtrListRemove(vlanId, intIfNum, SNOOP_STATIC, pSnoopCB);
      pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_GETNEXT);
    } /* End of VLAN iterations */
  } /* end of mode disable */

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Process the change in the VLAN admin mode
*
* @param   vlanId       @b{(input)} VLAN ID
* @param   mode         @b{(input)} VLAN Snoop Admin Mode
* @param   pSnoopCB     @b{(input)} Control Block
*
* @returns  None
*
* @notes none
*
* @end
*
*********************************************************************/
static void snoopVlanModeChangeProcess(L7_uint32 vlanId, L7_uint32 mode, 
                                       snoop_cb_t *pSnoopCB)
{
  L7_uint32            intIfNum, vlanStatus;
  snoopIntfCfgData_t  *pCfg;
  L7_BOOL              intfFlag = L7_FALSE;
  snoopOperData_t     *pSnoopOperEntry;
  L7_RC_t              rc_intIf, rc;

  rc_intIf = nimFirstValidIntfNumber(&intIfNum);
  while (rc_intIf == L7_SUCCESS)
  {
    if (snoopMapIntfIsConfigurable(intIfNum, &pCfg, pSnoopCB) 
                                   == L7_TRUE)
    {
     /* When snoop is enabled on interface basis, do change it operationally */
      if (mode == L7_DISABLE && pCfg->intfMode & SNOOP_VLAN_MODE)
      {
        rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
        intfFlag = L7_TRUE;
        continue;
      }

      if ((snoop_dot1qOperVlanMemberGet(vlanId, intIfNum, &vlanStatus) == L7_SUCCESS) 
          && (vlanStatus == L7_DOT1Q_FIXED))
      {
        (void)snoopIntfVlanModeApply(intIfNum, vlanId, mode, pSnoopCB);
      }
    }

    rc_intIf = nimNextValidIntfNumber(intIfNum, &intIfNum);
  }

  /* Enable Snooping Querier if the vlan is now ready for query processing */
  snoopQuerierVlanModeApply(/*vid_original*/ vlanId, mode, pSnoopCB);

  /* If no interface is using the operational information, de-initialize it */
  if (mode == L7_DISABLE && intfFlag == L7_FALSE)
  {
    if ((pSnoopOperEntry = snoopOperEntryGet(/*vid_original*/ vlanId,pSnoopCB, L7_MATCH_EXACT)))
    {
      snoopOperEntryDeInit(pSnoopOperEntry);
    }
  }
}

/*********************************************************************
* @purpose Process the change in the staic mcast rotuer for a vlan,
*          intIfNum of a snoop instance
*
* @param   vlanId       @b{(input)} Vlan ID
* @param   intIfNum     @b{(input)} Internal Interface Number
* @param   mode         @b{(input)} Mrouter Attached mode
* @param   pSnoopCB     @b{(input)} Control Block
*
* @returns  None
*
* @notes none
*
* @end
*
*********************************************************************/
static void snoopVlanMrouterModeChangeProcess(L7_uint32 vlanId, 
                                              L7_uint32 intIfNum, L7_uint32 mode, 
                                              snoop_cb_t *pSnoopCB)
{
  L7_uint32          vlanStatus;
  snoopIntfCfgData_t *pCfg;

  if ( (snoop_dot1qOperVlanMemberGet(vlanId, intIfNum, &vlanStatus) == L7_SUCCESS) &&
       (vlanStatus == L7_DOT1Q_FIXED))
  {
    /* Do not disable vlan mode, if it is enabled at interface level */
    if (mode == L7_DISABLE)
    {
      if (snoopMapIntfIsConfigurable(intIfNum, &pCfg, pSnoopCB) 
                                     == L7_TRUE)
      {
        if (pCfg->intfMcastRtrAttached == L7_ENABLE)
        {
          return;
        }
      }/* End of inteface config struct get */
    }/* End of disable check */
    snoopVlanMrouterApply(intIfNum, vlanId, mode, pSnoopCB);
  }
}

/*********************************************************************
* @purpose  Remove snoop entries with the specified VLAN of a 
*           snoop instance
*
* @param    vlanId     @b{(input)} VLAN ID
* @param    pSnoopCB   @b{(input)} Control Block
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t snoopVlanEntriesRemove(L7_uint32 vlanId, snoop_cb_t *pSnoopCB)
{
  L7_RC_t rc;
  L7_uchar8 macAddr[L7_MAC_ADDR_LEN];
  L7_uint32 vid;

  memset((void *)macAddr, 0x00, L7_MAC_ADDR_LEN);

  rc = snoopFirstGet(macAddr, &vid, pSnoopCB->family);
  while (rc == L7_SUCCESS)
  {
    if (vid == vlanId)
    {
      snoopEntryRemove(macAddr, vid, pSnoopCB->family);
    }
    rc = snoopNextGet(macAddr, vid, macAddr, &vid, pSnoopCB->family);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To apply interface configuration of a snoop instance
*
* @param    intIfNum   @b{(input)} Internal Interface Number
* @param    pSnoopCB   @b{(input)} Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t snoopApplyIntfConfigData(L7_uint32 intIfNum, snoop_cb_t *pSnoopCB)
{
  snoopIntfCfgData_t *pCfg;
  L7_RC_t             rc = L7_SUCCESS;
  L7_uint32           mode, vlanId;
  snoopOperData_t    *pSnoopOperEntry  = L7_NULLPTR;

  if (snoopMapIntfIsConfigurable(intIfNum, &pCfg, pSnoopCB) == L7_TRUE)
  {
    /* Applying the Snooping Configuration on a per-VLAN basis */
    pSnoopOperEntry = snoopOperEntryFirstGet(pSnoopCB);
    while(pSnoopOperEntry)
    {
      vlanId = pSnoopOperEntry->vlanId;
      /* Verify if Interface participates in the VLAN */
      if ( (snoop_dot1qOperVlanMemberGet(vlanId, intIfNum, &mode) == L7_SUCCESS) 
           && (mode != L7_DOT1Q_FIXED))
      {
        pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_GETNEXT);
        continue;
      }

      if (pCfg->intfMode & SNOOP_VLAN_MODE ||
          pSnoopCB->snoopCfgData->snoopVlanMode[vlanId] & SNOOP_VLAN_MODE)
      {
        if (snoopIntfVlanModeApply(intIfNum, vlanId, L7_ENABLE, pSnoopCB) != L7_SUCCESS)
        {
          LOG_MSG("snoopApplyIntfConfigData: Failed to apply configuration intIfNum %d vlan %d family %d",
                  intIfNum, vlanId, pSnoopCB->family);
          return L7_FAILURE;
        }
      }
      else
      {
        if (snoopIntfVlanModeApply(intIfNum, vlanId, L7_DISABLE, pSnoopCB) != L7_SUCCESS)
        {
          LOG_MSG("snoopApplyIntfConfigData: Failed to apply configuration intIfNum %d vlan %d family %d",
                  intIfNum, vlanId, pSnoopCB->family);
          return L7_FAILURE;
        }
      }

      /* Applying the Multicast Router Configuration on a per-interface/per-VLAN basis */
      if (L7_VLAN_ISMASKBITSET(pCfg->vlanStaticMcastRtr, vlanId) ||
          pCfg->intfMcastRtrAttached == L7_ENABLE)
      {
        if (snoopVlanMrouterApply(intIfNum, vlanId, L7_ENABLE, pSnoopCB) 
                                  != L7_SUCCESS)
        {
          LOG_MSG("snoopApplyIntfConfigData: Failed to apply static mrtr configuration intIfNum %d vlan %d family %d",
                  intIfNum, vlanId, pSnoopCB->family);
          return L7_FAILURE;
        }
      }
      else
      {
        if (snoopVlanMrouterApply(intIfNum, vlanId, L7_DISABLE, pSnoopCB) 
                                  != L7_SUCCESS)
        {
          LOG_MSG("snoopApplyIntfConfigData: Failed to apply static mrtr configuration intIfNum %d vlan %d family %d",
                  intIfNum, vlanId, pSnoopCB->family);
          return L7_FAILURE;
        }
      }
      pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_GETNEXT);
    } /* End of VLAN iterations */
  } /* End of interface config get */
  return rc;
}

/*********************************************************************
* @purpose  Callback function to propogate Interface notifications to 
*           snooping Task
*
* @param    intIfNum   @b{(input)}  Interface number
* @param    event      @b{(input)}  Event type
* @param    correlator @b{(input)}  Correlator for NIM event
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t snoopIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event, 
                                NIM_CORRELATOR_t correlator)
{
  snoopMgmtMsg_t  msg;
  snoop_eb_t     *pSnoopEB;
  NIM_EVENT_COMPLETE_INFO_t status;

  /* If this is an event we are not interested in, perform early return*/
  if (event != L7_LAG_RELEASE &&
      event != L7_PROBE_TEARDOWN &&
      event != L7_PORT_ROUTING_DISABLED &&
      event != L7_ACTIVE &&
      event != L7_LAG_ACQUIRE &&
      event != L7_PROBE_SETUP &&
      event != L7_PORT_ROUTING_ENABLED &&
      event != L7_INACTIVE &&
      event != L7_CREATE &&
      event != L7_ATTACH &&
      event != L7_DETACH &&
      event != L7_DOWN &&
      event != L7_DELETE)
  {
    status.intIfNum     = intIfNum;
    status.component    = L7_SNOOPING_COMPONENT_ID;
    status.event        = event;
    status.correlator   = correlator;
    status.response.reason = NIM_ERR_RC_UNUSED ;
    status.response.rc  = L7_SUCCESS;
    nimEventStatusCallback(status);
    return L7_SUCCESS;
  }


  msg.msgId    = snoopIntfChange;
  msg.intIfNum = intIfNum;
  msg.u.snoopIntfChangeParms.event = event;
  msg.u.snoopIntfChangeParms.correlator = correlator;

  pSnoopEB = snoopEBGet();
  if (osapiMessageSend(pSnoopEB->snoopQueue, &msg, SNOOP_MSG_SIZE, L7_WAIT_FOREVER, 
                         L7_MSG_PRIORITY_NORM) == L7_SUCCESS)
  {
    if (osapiSemaGive(pSnoopEB->snoopMsgQSema) != L7_SUCCESS)
    {
      LOG_MSG("snoopIntfChangeCallback: Failed to give msgQueue semaphore\n"); 
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Callback function to propogate vlan notifications to 
*           snooping Task
*
* @param    vlanId     @b{(input)}  vlan id of the event
* @param    intIfNum   @b{(input)}  Interface number
* @param    event      @b{(input)}  Event type
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t snoopVlanChangeCallback(dot1qNotifyData_t *vlanData, L7_uint32 intIfNum, 
                                L7_uint32 event)
{
  snoopMgmtMsg_t  msg;
  snoop_eb_t     *pSnoopEB;
  L7_uint32       port, intf;
  L7_uint16       vlan_original, vlan_to_use;
  L7_int          xlate_once=L7_FALSE;

  switch (event)
  {
    case VLAN_ADD_PORT_NOTIFY: 
    case VLAN_DELETE_PORT_NOTIFY:
      {
        // PTin added
        if (vlanData->numVlans==1 &&
            ptin_intif_oldvlan_translate_get(intIfNum,&vlan_to_use,vlanData->data.vlanId)==L7_SUCCESS)
        {
          //printf("%s:%s(%d) [1] interf=%u: vlan=%u converted to %u\n",__FILE__, __FUNCTION__, __LINE__,intIfNum,vlanData->data.vlanId,vlan_to_use);
          vlanData->data.vlanId = vlan_to_use;
        }
        else
        {
          //printf("%s:%s(%d) [1] interf=%u: No translation detected (vlan=%u)\n",__FILE__, __FUNCTION__, __LINE__,intIfNum,vlanData->data.vlanId);
        }
        // PTin end
      }
    case VLAN_INITIALIZED_NOTIFY:
    case VLAN_RESTORE_NOTIFY:
      {
        msg.msgId     = snoopVlanChange;
        msg.intIfNum  = intIfNum;
        memcpy(&msg.u.vlanData, vlanData, sizeof(dot1qNotifyData_t));
        msg.vlanEvent = event;

        pSnoopEB = snoopEBGet();

        if (osapiMessageSend(pSnoopEB->snoopQueue, &msg, SNOOP_MSG_SIZE, L7_WAIT_FOREVER, 
                               L7_MSG_PRIORITY_NORM) == L7_SUCCESS)
        {
          if (osapiSemaGive(pSnoopEB->snoopMsgQSema) != L7_SUCCESS)
          {
            LOG_MSG("snoopVlanChangeCallback: Failed to give msgQueue semaphore\n"); 
          }
        }
        else
        {
          LOG_MSG("snoopVlanChangeCallback: Failed to post message event %d intIfNum %d\n",
                  event, intIfNum); 
        } 
      }
      break;

    case VLAN_ADD_NOTIFY:
    case VLAN_DELETE_PENDING_NOTIFY:
      {
        // PTin added
        vlan_original = vlanData->data.vlanId;
        xlate_once=L7_FALSE;

        for (port=0; port<L7_SYSTEM_N_INTERF; port++)
        {
          // Determine the interface reference
          if (ptin_translate_port2intf(port,&intf)!=L7_SUCCESS || intf==0)  continue;

          if (vlanData->numVlans==1)
          {
            // Determine original vlan
            if (ptin_intif_oldvlan_translate_get(intf,&vlan_to_use,vlan_original)==L7_SUCCESS)
            {
              vlanData->data.vlanId = vlan_to_use;
              xlate_once=L7_TRUE;
            }
            else if (port<L7_SYSTEM_N_INTERF-1 || xlate_once==L7_TRUE)
            {
              continue;
            }
            else
            {
              vlanData->data.vlanId = vlan_original;
            }
          }
          // PTin end
  
          msg.msgId     = snoopVlanChange;
          msg.intIfNum  = (xlate_once==L7_TRUE) ? intf : intIfNum;    // PTin added
          memcpy(&msg.u.vlanData, vlanData, sizeof(dot1qNotifyData_t));
          msg.vlanEvent = event;
  
          pSnoopEB = snoopEBGet(); 
  
          if (osapiMessageSend(pSnoopEB->snoopQueue, &msg, SNOOP_MSG_SIZE, L7_WAIT_FOREVER, 
                                 L7_MSG_PRIORITY_NORM) == L7_SUCCESS)
          {
            if (osapiSemaGive(pSnoopEB->snoopMsgQSema) != L7_SUCCESS)
            {
              LOG_MSG("snoopVlanChangeCallback: Failed to give msgQueue semaphore\n"); 
            }
          }
          else
          {
            LOG_MSG("snoopVlanChangeCallback: Failed to post message event %d intIfNum %d\n",
                    event, intIfNum); 
          }
          if (vlanData->numVlans!=1)  break;        // PTin added
        }
      }
      break;

    default:
      return L7_SUCCESS;
  }



  return L7_SUCCESS;
}

// PTin added
L7_int32 snoopQueueGetNumMsgs( void )
{
  L7_int32 num;
  snoop_eb_t *pSnoopEB=snoopEBGet();

  (void)osapiMsgQueueGetNumMsgs( pSnoopEB->snoopQueue, &num);

  return num;
}
// PTin end


/*********************************************************************
 * @purpose  Processing function to handle VLAN change notifications
 *
 * @param    vlanId     @b{(input)} VLAN ID
 * @param    intIfNum   @b{(input)} Interface number
 * @param    event      @b{(input)} Event type
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_RC_t snoopVlanChangeProcess(dot1qNotifyData_t *vlanData, L7_uint32 intIfNum, 
                                      L7_uint32 event)
{                    
  L7_RC_t     rc = L7_SUCCESS;
  L7_uint32      i = 0, vlanId = 0, numVlans = 0;

  for (i = 1; i<=L7_VLAN_MAX_MASK_BIT; i++) 
  {
      if (vlanData->numVlans == 1) 
      {
          vlanId = vlanData->data.vlanId;
          /* For any continue, we will break out */
          i = L7_VLAN_MAX_MASK_BIT + 1;
      }
      else
      {
          if (L7_VLAN_ISMASKBITSET(vlanData->data.vlanMask,i)) 
          {
              vlanId = i;
          }
          else
          {
              if (numVlans == vlanData->numVlans) 
              {
                  /* Already taken care of all the bits in the mask so break out of for loop */
                  break;
              }
              else
              {
                  /* Vlan is not set check for the next bit since there are more bits that are set*/
                  continue;
              }
          }
      }
  switch (event)
  {
  case VLAN_ADD_NOTIFY:
    /* New VLAN is created */
    rc = snoopOperEntryAdd(vlanId);
    if (rc == L7_SUCCESS)
    {
      snoopQuerierVlanAdd(vlanId);
    }
    break;

  case VLAN_DELETE_PENDING_NOTIFY:
    /* About to delete a VLAN */
      (void)snoopQuerierVlanDelete(vlanId);
      rc = snoopVlanDelete(vlanId);
    break;

  case VLAN_ADD_PORT_NOTIFY: 
    // PTin added
    if (snoopOperEntryGet(vlanId,snoopCBGet(L7_AF_INET),L7_MATCH_EXACT)==L7_NULLPTR)
    {
      rc = snoopOperEntryAdd(vlanId);
      if (rc == L7_SUCCESS)
      {
        snoopQuerierVlanAdd(vlanId);
      }
      else
        break;
    }
    // PTin end
    rc = snoopVlanPortAdd(vlanId, intIfNum);

    /* Port added to a VLAN */
    break;

  case VLAN_DELETE_PORT_NOTIFY:
    /* Port removed from a VLAN */
    rc=snoopVlanPortDelete(vlanId, intIfNum);
    break;
  case VLAN_INITIALIZED_NOTIFY:
    /* Get all the missed vlan's */
    snoopAllVlansGet();
    break;
  case VLAN_RESTORE_NOTIFY:
    /* Delete all the vlan's */
    snoopAllVlanDelete();
    break;

  default:
    break;
  }
      numVlans++;
  }

  ptin_vlan_translate_delete_flush();  /* PTin added */
  return rc;
}

/*********************************************************************
* @purpose  Callback function to handle dot1s interface change notifications
*
* @param    mstID     @b{(input)}  Multiple Spanning Tree instance ID
* @param    intIfnum  @b{(input)}  Internal interface whose state has changed
* @param    event     @b{(input)}  MST event
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t snoopDot1sIntfChangeCallback(L7_uint32 mstID, L7_uint32 intIfNum, 
                                     L7_uint32 event)
{
  snoopMgmtMsg_t  msg;
  snoop_eb_t     *pSnoopEB;

  if (event != L7_DOT1S_EVENT_TOPOLOGY_CHANGE)
  {
    if (intIfNum == 0 || snoopIsValidIntf(intIfNum) != L7_TRUE)
    {
      return L7_SUCCESS;
    }
  }

  /* We are only interested in these two events */
  if (event != L7_DOT1S_EVENT_FORWARDING && 
      event != L7_DOT1S_EVENT_NOT_FORWARDING && 
      event != L7_DOT1S_EVENT_TOPOLOGY_CHANGE)
  {
    return L7_SUCCESS;
  }

  msg.msgId     = snoopDot1sIntfChange;
  msg.u.snoopDot1sIntfChangeParms.mstID = mstID;
  msg.intIfNum  = intIfNum;
  msg.u.snoopDot1sIntfChangeParms.event = event;

  pSnoopEB = snoopEBGet();
  if (osapiMessageSend(pSnoopEB->snoopQueue, &msg, SNOOP_MSG_SIZE, L7_WAIT_FOREVER, 
                         L7_MSG_PRIORITY_NORM) == L7_SUCCESS)
  {
    if (osapiSemaGive(pSnoopEB->snoopMsgQSema) != L7_SUCCESS)
    {
      LOG_MSG("snoopDot1sIntfChangeCallback: Failed to give msgQueue semaphore\n"); 
    }
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Posts message to vendor code to specify the event of
*           change in wanIP address
*
* @param    registrar_ID   routine registrar id  (See L7_COMPONENT_ID_t)
* @param    addrType      Address type to notify (SIM_ADDR_CHANGE_TYPE_t)
* @param    addrFamily    Family of address (L7_AF_INET & L7_AF_INET6)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t snoopNetworkAddrChanged(L7_COMPONENT_IDS_t registrar_ID, L7_uint32 addrType,
                                L7_uchar8 addrFamily)
{
  snoopMgmtMsg_t  msg;
  snoop_cb_t     *pSnoopCB;

 /* Apply the config change */
  memset(&msg, 0, sizeof(snoopMgmtMsg_t) );
  msg.msgId    = snoopQuerierNetworkAddressChange;

  if ((pSnoopCB = snoopCBGet(addrFamily)) == L7_NULLPTR)
  {
    LOG_MSG("snoopNetworkAddrChanged: Invalid address change event received\n");
    return L7_SUCCESS;
  }
  msg.cbHandle = pSnoopCB;

  if (osapiMessageSend(pSnoopCB->snoopExec->snoopQueue, &msg, SNOOP_MSG_SIZE,
                       L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    LOG_MSG("snoopNetworkAddrChanged: Failed to post a message to queue\n");
  }
  else
  {
    if (osapiSemaGive(pSnoopCB->snoopExec->snoopMsgQSema) != L7_SUCCESS)
    {
      LOG_MSG("snoopNetworkAddrChanged: Failed to give msgQueue semaphore\n");
    }
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose   Function to process dot1s interface change notifications
*
* @param    mstID     @b{(input)}  Multiple Spanning Tree instance ID
* @param    intIfnum  @b{(input)}  Internal interface whose state has changed
* @param    event     @b{(input)}  MST event
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t snoopDot1sIntfChangeProcess(L7_uint32 mstID, L7_uint32 intIfNum, 
                                    L7_uint32 event)
{
  L7_uint32           numVlans;
  L7_RC_t             rc = L7_FAILURE;
  static L7_ushort16  vidList[L7_MAX_VLANS+1];

  memset((L7_uchar8 *)vidList, 0, sizeof(vidList));

  (void)dot1sMstiVIDList(mstID, vidList, &numVlans);

  switch (event)
  {
  case L7_DOT1S_EVENT_FORWARDING:
    rc = snoopDot1sForwarding(intIfNum, vidList, numVlans);
    break;

  case L7_DOT1S_EVENT_NOT_FORWARDING:
    rc = snoopDot1sNotForwarding(intIfNum, vidList, numVlans);
    break;

  case L7_DOT1S_EVENT_TOPOLOGY_CHANGE:
    
    if (dot1sIsBridgeRoot(mstID) == L7_TRUE)
    { 
      rc = snoopDot1sTCNProcess(mstID, vidList, numVlans);
    }
    break;
  default:
    rc = L7_SUCCESS;
    break;
  }
  return rc;
}

/*********************************************************************
* @purpose   This function is used to send timer events
*        
* @param     timerCtrlBlk    @b{(input)}   Timer Control Block
* @param     ptrData         @b{(input)}   Ptr to passed data
*
* @returns   None
*
* @notes     None
* @end
*********************************************************************/
void snoopTimerExpiryHdlr(L7_APP_TMR_CTRL_BLK_t timerCtrlBlk, void* ptrData)
{
  L7_int32       rc;
  snoopTimerParams_t msg;
  snoop_eb_t    *pSnoopEB = L7_NULLPTR;

  pSnoopEB = snoopEBGet();
  msg.timerCBHandle = (L7_uint32)ptrData;

#if 0
  if ((pSnoopEB = (snoop_eb_t *)ptrData) == L7_NULLPTR)
    return;
#endif
  rc = osapiMessageSend(pSnoopEB->snoopTimerQueue, &msg, SNOOP_TIMER_MSG_SIZE, L7_NO_WAIT, 
                        L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    LOG_MSG("snoopTimer(): Snooping timer tick send failed\n");
  }
  else
  {
    if (osapiSemaGive(pSnoopEB->snoopMsgQSema) != L7_SUCCESS)
    {
      LOG_MSG("snoopTimerExpiryHdlr: MsgQSema give failed"); 
    }
  }
}

/*********************************************************************
* @purpose  Process interface change notifications
*
* @param    intIfNum   @b{(input)}  Interface number
* @param    event      @b{(input)}  Event type
* @param    correlator @b{(output)}  Correlator for event
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
static void snoopIntfChangeProcess(L7_uint32 intIfNum, L7_uint32 event, 
                                   NIM_CORRELATOR_t correlator)
{
  L7_RC_t                   rc = L7_SUCCESS;
  NIM_EVENT_COMPLETE_INFO_t status;
  L7_COMPONENT_IDS_t        componentID;
  L7_uint32                 prevAcquired = L7_FALSE, activeState = L7_INACTIVE, vlanId, cbIndex;
  snoop_eb_t               *pSnoopEB;
  snoop_cb_t               *pSnoopCB  = L7_NULLPTR;

  status.intIfNum     = intIfNum;
  status.component    = L7_SNOOPING_COMPONENT_ID;
  status.event        = event;
  status.correlator   = correlator;
  status.response.reason = NIM_ERR_RC_UNUSED ;

  if (snoopIsReady() == L7_FALSE)
  {
    LOG_MSG("snoopIntfChangeProcess: Received an interface change callback while not ready to receive it");
    rc = L7_FAILURE;
    status.response.rc  = rc;
    nimEventStatusCallback(status);
    return;
  }
  /* Skip intfValidCheck for CREATE & DELETE for vlan routing intfs */
  if (event != L7_CREATE && event != L7_DELETE && 
      snoopIsValidIntf(intIfNum) != L7_TRUE)
  {
    rc = L7_SUCCESS;
    status.response.rc  = rc;
    nimEventStatusCallback(status);
    return;
  }
  pSnoopEB = snoopEBGet();
  switch (event)
  {
  case L7_LAG_RELEASE:
#ifdef L7_MCAST_PACKAGE
       snoopL3McastLagPortRelease(intIfNum);
#endif
  case L7_PROBE_TEARDOWN:
  case L7_PORT_ROUTING_DISABLED:
    {
      if (event == L7_LAG_RELEASE)
      {
        componentID = L7_DOT3AD_COMPONENT_ID;
      }
      else if (event == L7_PROBE_TEARDOWN)
      {
        componentID = L7_PORT_MIRROR_COMPONENT_ID;
      }
      else if (event == L7_PORT_ROUTING_DISABLED)
      {
        componentID = L7_IP_MAP_COMPONENT_ID;
      }
      else
      {
        componentID = L7_LAST_COMPONENT_ID; /* we dont process this component id */
      }

      /* update aquired list */    
      COMPONENT_ACQ_CLRMASKBIT(pSnoopEB->snoopIntfInfo[intIfNum].acquiredList, 
                               componentID);

      /* Try to enable only if the port is active. Or break off and wait
         for the port L7_ACTIVE event */
      if (nimGetIntfActiveState(intIfNum, &activeState) != L7_SUCCESS ||
                                activeState != L7_ACTIVE)
      {
        break;
      }
      COMPONENT_ACQ_NONZEROMASK(pSnoopEB->snoopIntfInfo[intIfNum].acquiredList, 
                                prevAcquired);
    }
    /* fall through the case statement to re-apply the 
       snooping interface mode in case the port is already active */
  case L7_ACTIVE: 
    if (prevAcquired == L7_FALSE)
     rc = snoopIntfActive(intIfNum);
    break;

  case L7_LAG_ACQUIRE:
  case L7_PROBE_SETUP:
  case L7_PORT_ROUTING_ENABLED:
	 {
		if (event == L7_LAG_ACQUIRE)
        {
		  componentID = L7_DOT3AD_COMPONENT_ID;
        }
		else if (event == L7_PROBE_SETUP)
        { 
		  componentID = L7_PORT_MIRROR_COMPONENT_ID;
        }
		else if (event == L7_PORT_ROUTING_ENABLED)
        {
		  componentID = L7_IP_MAP_COMPONENT_ID;
        }
        else 
        {
          componentID = L7_LAST_COMPONENT_ID; /* we dont process this component id */
        }
      /* update aquired list */
    COMPONENT_ACQ_NONZEROMASK(pSnoopEB->snoopIntfInfo[intIfNum].acquiredList, 
                              prevAcquired);
    COMPONENT_ACQ_SETMASKBIT(pSnoopEB->snoopIntfInfo[intIfNum].acquiredList, 
                             componentID);
      /* fall through the case statement to disable 
         snooping interface mode */
   }
  case L7_DOWN:
  case L7_INACTIVE:		      
    if (prevAcquired == L7_FALSE)
    {
      rc = snoopIntfInActive(intIfNum);
    }
        
    break;

  case L7_CREATE:
    if(snoopIsValidIntf(intIfNum) == L7_TRUE)
    {
      rc = snoopIntfCreate(intIfNum);
    }
    else if (snoop_dot1qVlanIntfIntIfNumToVlanId(intIfNum, &vlanId) == L7_SUCCESS)
    { /* It is a vlan routing interface */
      L7_VLAN_SETMASKBIT(pSnoopEB->routingVlanVidMask, vlanId);
      pSnoopCB = snoopCBFirstGet();
      for (cbIndex = L7_NULL;
           cbIndex < pSnoopEB->maxSnoopInstances; cbIndex++, pSnoopCB++)
      {
       /* Disable snooping querier on newly created vlan routing interface */
       (void)snoopQuerierVlanModeApply(vlanId, L7_DISABLE, pSnoopCB);
      }
      rc = L7_SUCCESS;
    }
    else
    {
      rc = L7_SUCCESS; /* Invalid interface simply return */
    }
    break;

  case L7_ATTACH:
    rc = snoopIntfAttach(intIfNum);
    break;

  case L7_DETACH:
      rc = snoopIntfDetach(intIfNum);
    break;

  case L7_DELETE:
    if(snoopIsValidIntf(intIfNum) == L7_TRUE)
    {
      rc = snoopIntfDelete(intIfNum);
    }
    else if (snoop_dot1qVlanIntfIntIfNumToVlanId(intIfNum, &vlanId) == L7_SUCCESS)
    { /* It is a vlan routing interface */
      L7_VLAN_CLRMASKBIT(pSnoopEB->routingVlanVidMask, vlanId);
      pSnoopCB = snoopCBFirstGet();
      for (cbIndex = L7_NULL;
           cbIndex < pSnoopEB->maxSnoopInstances; cbIndex++, pSnoopCB++)
      {
        /* Re-enable snooping querier as routing vlan no more exists */
       (void)snoopQuerierVlanModeApply(vlanId, L7_ENABLE, pSnoopCB);
      }
      rc = L7_SUCCESS;
    }
    else
    { 
      rc = L7_SUCCESS; /* Invalid interface simply return */
    }
    break;
  default:
    break;
  }

  status.response.rc  = rc;
  nimEventStatusCallback(status);
}

/*********************************************************************
* @purpose  To process the NIM Callback for L7_CREATE
*
* @param    intIfNum @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
static L7_RC_t snoopIntfCreate(L7_uint32 intIfNum)
{
  nimConfigID_t       configId;
  snoopIntfCfgData_t *pCfg;
  snoop_cb_t         *pSnoopCB = L7_NULLPTR;
  L7_uint32           cid, cbIndex;
  snoop_eb_t         *pSnoopEB;

  if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  pSnoopEB = snoopEBGet();
  pSnoopCB = snoopCBFirstGet(); /* Point to the first snoop instance */
  for (cid = 1; cid < L7_IGMP_SNOOPING_MAX_INTF; cid++)
  {
    /* The index table is common for MLD and IGMP */
    if (NIM_CONFIG_ID_IS_EQUAL(&(pSnoopCB->snoopCfgData->snoopIntfCfgData[cid].configId), 
                               &configId))
    {
      pSnoopCB->snoopExec->snoopIntfMapTbl[intIfNum] = cid;
      break;
    }
  }

  /* Clear component acquire info */
  memset(&(pSnoopEB->snoopIntfInfo[intIfNum].acquiredList),
         0, sizeof(AcquiredMask));

  pCfg = L7_NULL;
  /* If an interface configuration entry is not already assigned to the interface,
     assign one. */

  pSnoopCB = snoopCBFirstGet();
  for (cbIndex = L7_NULL; 
       cbIndex < pSnoopEB->maxSnoopInstances; cbIndex++, pSnoopCB++)
  {
    if (snoopMapIntfIsConfigurable(intIfNum, &pCfg, pSnoopCB) != L7_TRUE)
    {
      if (snoopMapIntfConfigEntryGet(intIfNum, &pCfg, pSnoopCB) != L7_TRUE)
      {
        LOG_MSG("snoopIntfCreate Failure for snoopMapIntfConfigEntryGet for interface %d family %d\n", 
                     intIfNum, pSnoopCB->family);
        return L7_FAILURE;
      }

      /* Update the configuration structure with the config id */
      if (pCfg != L7_NULL)
      {
        snoopBuildDefaultIntfConfigData(&configId, pCfg, pSnoopCB);
      }
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To process the Callback for L7_DELETE
*
* @param    intIfNum @b{(input)} Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t snoopIntfDelete(L7_uint32 intIfNum)
{
  snoopIntfCfgData_t *pCfg;
  snoop_cb_t         *pSnoopCB = L7_NULLPTR;
  L7_uint32           cbIndex = L7_NULL;
  snoop_eb_t         *pSnoopEB;

  pSnoopEB = snoopEBGet();
  pSnoopCB = snoopCBFirstGet();
  for (cbIndex = L7_NULL;
       cbIndex < pSnoopEB->maxSnoopInstances; cbIndex++, pSnoopCB++)
  {
    if (snoopMapIntfIsConfigurable(intIfNum, &pCfg, pSnoopCB) == L7_TRUE)
    {
      memset((void *)&pCfg->configId, 0, sizeof(nimConfigID_t));
      /* Snoop interface map table is common for all instances, delete
         the entry only during the last instance call */
      if (cbIndex == pSnoopEB->maxSnoopInstances-1)
      {
        memset((void *)& (pSnoopCB->snoopExec->snoopIntfMapTbl[intIfNum]), 
               0, sizeof(L7_uint32));
      }
      pSnoopCB->snoopCfgData->cfgHdr.dataChanged = L7_TRUE;
    }
  }

  /* Clear component acquire info */
  memset(&(pSnoopEB->snoopIntfInfo[intIfNum].acquiredList),
         0, sizeof(AcquiredMask));

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To process the Callback for L7_ATTACH
*
* @param    intIfNum @b{(input)} Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t snoopIntfAttach(L7_uint32 intIfNum)
{
  snoop_cb_t         *pSnoopCB = L7_NULLPTR;
  L7_uint32           cbIndex = L7_NULL;
  L7_RC_t             rc = L7_SUCCESS;
  snoop_eb_t         *pSnoopEB;

  pSnoopEB = snoopEBGet();
  pSnoopCB = snoopCBFirstGet();
  for (cbIndex = L7_NULL;
       cbIndex < pSnoopEB->maxSnoopInstances; cbIndex++, pSnoopCB++)
  {
    if (pSnoopCB->snoopCfgData->snoopAdminMode != L7_ENABLE)
    {
      continue;
    }

    if (pSnoopCB->snoopCfgData->snoopAdminMode == L7_ENABLE)
    {
      rc = snoopApplyIntfConfigData(intIfNum, pSnoopCB);
    }

    if (rc != L7_SUCCESS)
    {
      LOG_MSG("snoopIntfAttach: Failed to attach intIfNum %d family %d\n",
              intIfNum, pSnoopCB->family);
      return L7_FAILURE;
    }
  } 
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To process the Callback for L7_DETACH
*
* @param    intIfNum @b{(input)} Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t snoopIntfDetach(L7_uint32 intIfNum)
{
  snoopIntfCfgData_t *pCfg;
  snoop_cb_t         *pSnoopCB = L7_NULLPTR;
  L7_uint32           cbIndex = L7_NULL;
  snoop_eb_t         *pSnoopEB;
  L7_uint32           fdVal;
  nimConfigID_t       configId;
  pSnoopEB = snoopEBGet();
  pSnoopCB = snoopCBFirstGet();
  for (cbIndex = L7_NULL;
       cbIndex < pSnoopEB->maxSnoopInstances; cbIndex++, pSnoopCB++)
  {
    if (pSnoopCB->snoopCfgData->snoopAdminMode != L7_ENABLE)
    {
      continue;
    }

    if (snoopMapIntfIsConfigurable(intIfNum, &pCfg, pSnoopCB) == L7_TRUE)
    {
      if (pSnoopCB->family == L7_AF_INET)
      {
        fdVal = FD_IGMP_SNOOPING_INTF_MODE;
      }
      else
      {
        fdVal = FD_MLD_SNOOPING_INTF_MODE;
      }

      if (snoopIntfModeApply(intIfNum, fdVal, pSnoopCB) != L7_SUCCESS)
      {
        LOG_MSG("snoopIntfDetach: Failed to detach intIfNum %d family %d\n",
                 intIfNum, pSnoopCB->family);
        return L7_FAILURE;
      }

      if (pSnoopCB->family == L7_AF_INET)
      {
        fdVal = FD_IGMP_SNOOPING_MCASTRTR_STATUS;
      }
      else
      {
        fdVal = FD_MLD_SNOOPING_MCASTRTR_STATUS;
      }

      if (snoopIntfMrouterApply(intIfNum, fdVal, pSnoopCB) != L7_SUCCESS)
      {
        LOG_MSG("snoopIntfDetach: Failed to detach intIfNum %d family %d\n",
                  intIfNum, pSnoopCB->family);
         return L7_FAILURE;
      }

      if (nimConfigIdGet( intIfNum, &configId) == L7_SUCCESS)
      {
        /* Other factory defaults */
        snoopBuildDefaultIntfConfigData( &configId, 
                                      pCfg, 
                                      pSnoopCB);
      	}
      
    } /* End of interface cfg check */
  } /* End of snoop iterations */
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To process the Callback for L7_ACTIVE
*
* @param    intIfNum @b{(input)} Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t snoopIntfActive(L7_uint32 intIfNum)
{
  snoopIntfCfgData_t *pCfg;
  snoop_cb_t         *pSnoopCB = L7_NULLPTR;
  L7_uint32           cbIndex = L7_NULL;
  L7_uint32           vlanId;
  L7_VLAN_MASK_t   vidMask;
  L7_RC_t             rv = L7_SUCCESS, rc;
  snoop_eb_t         *pSnoopEB;
  snoopOperData_t    *pSnoopOperEntry = L7_NULLPTR;

  pSnoopCB = snoopCBFirstGet();
  pSnoopEB = snoopEBGet();
  for (cbIndex = L7_NULL;
       cbIndex < pSnoopEB->maxSnoopInstances; cbIndex++, pSnoopCB++)
  {
    if (pSnoopCB->snoopCfgData->snoopAdminMode != L7_ENABLE)
    {
      continue;
    }

    if (snoopMapIntfIsConfigurable(intIfNum, &pCfg, pSnoopCB) 
                                   == L7_TRUE)
    {
      /* Get the list of vlans interface is member of */
      snoop_dot1qVIDListMaskGet(intIfNum,&vidMask);
      /* Iterate through all the vlan indices we have */
      pSnoopOperEntry = snoopOperEntryFirstGet(pSnoopCB);
      while(pSnoopOperEntry)
      {
        vlanId = pSnoopOperEntry->vlanId;

        /* Verify VLAN is in our map array */
        if (L7_VLAN_ISMASKBITSET(vidMask, vlanId))
        {
          /*Apply the snoop interface mode this vlan */
          if ((pCfg->intfMode & SNOOP_VLAN_MODE) ||
              (pSnoopCB->snoopCfgData->snoopVlanMode[vlanId] & SNOOP_VLAN_MODE))
          {
            rc = snoopIntfVlanModeApply(intIfNum, vlanId, L7_ENABLE, pSnoopCB);
            if (rc != L7_SUCCESS)
            {
              rv = L7_FAILURE;
            }
          }

          /*Apply the Mcast router if it is configured for this vlan or
            globally for the interface */
          if ((L7_VLAN_ISMASKBITSET(pCfg->vlanStaticMcastRtr, vlanId)) ||
              (pCfg->intfMcastRtrAttached == L7_ENABLE))
          {
            rc = snoopVlanMrouterApply(intIfNum, vlanId, L7_ENABLE, 
                                       pSnoopCB);
            if (rc != L7_SUCCESS)
            {
              rv = L7_FAILURE;
              LOG_MSG("snoopIntfActive: Mcast router Apply failed for interface %d vlan %d family %d",
                      intIfNum, vlanId, pSnoopCB->family);
            }
          }
        }/* End of vlan member check */
        pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_GETNEXT);
      }  /* end of vidx iterations */
    } /* end of get interface cfg structure */
  } /* end of snoop instances iterations */
  return rv;
}

/*********************************************************************
* @purpose  To process the Callback for L7_INACTIVE
*
* @param    intIfNum @b{(input)} Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t snoopIntfInActive(L7_uint32 intIfNum)
{
  snoopIntfCfgData_t *pCfg;
  snoop_cb_t         *pSnoopCB = L7_NULLPTR;
  L7_uint32           cbIndex = L7_NULL;
  L7_uint32           vlanId;
  L7_VLAN_MASK_t   vidMask;
  L7_RC_t             rv = L7_SUCCESS, rc;
  snoop_eb_t         *pSnoopEB;
  snoopOperData_t    *pSnoopOperEntry = L7_NULLPTR;

  pSnoopCB = snoopCBFirstGet();
  pSnoopEB = snoopEBGet();
  for (cbIndex = L7_NULL;
       cbIndex < pSnoopEB->maxSnoopInstances; cbIndex++, pSnoopCB++)
  {
    if (pSnoopCB->snoopCfgData->snoopAdminMode != L7_ENABLE)
    {
      continue;
    }

    if (snoopMapIntfIsConfigurable(intIfNum, &pCfg, pSnoopCB)
                                   == L7_TRUE)
    {
      /* Get the list of vlans interface is member of */
      snoop_dot1qVIDListMaskGet(intIfNum,&vidMask);
      /* Iterate through all the vlan indices we have */
      pSnoopOperEntry = snoopOperEntryFirstGet(pSnoopCB);
      while(pSnoopOperEntry)
      {
        vlanId = pSnoopOperEntry->vlanId;

        /* Disable snoop operational mode */
        if (L7_INTF_ISMASKBITSET(pSnoopOperEntry->snoopIntfMode, intIfNum))
        {
          rc = snoopIntfVlanModeApply(intIfNum, vlanId, L7_DISABLE, pSnoopCB);
          if (rc != L7_SUCCESS)
          {
            rv = L7_FAILURE;
            LOG_MSG("snoopIntfInActive: interface vlan mode apply failed for interface %d vlan %d family %d",
                    intIfNum, vlanId, pSnoopCB->family);
          }
        }

        /* Remove this interface from the static mcast router list */
        if (L7_INTF_ISMASKBITSET(pSnoopOperEntry->mcastRtrAttached, intIfNum))
        {
          rc = snoopVlanMrouterApply(intIfNum, vlanId, L7_DISABLE,
                                     pSnoopCB);
          if (rc != L7_SUCCESS)
          {
            rv = L7_FAILURE;
            LOG_MSG("snoopIntfInActive: Mcast router Apply failed for interface %d vlan %d family %d",
                    intIfNum, vlanId, pSnoopCB->family);
          }
        }
        pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_GETNEXT);
      }  /* end of vidx iterations */
    } /* end of get interface cfg structure */
  } /* end of snoop instances iterations */
  return rv;
}

/*********************************************************************
* @purpose  To process the Callback for VLAN_DELETE_PENDING_NOTIFY
*
* @param    vlanId @b{(input)} VLAN ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t snoopVlanDelete(L7_uint32 vlanId)
{
  snoop_cb_t      *pSnoopCB = L7_NULLPTR;
  L7_RC_t          rc = L7_SUCCESS, rc_2 = L7_SUCCESS;   
  L7_uint32        cbIndex;
  snoop_eb_t      *pSnoopEB;

  /* Remove all entries for this vlan id from the snoop Tree and MFDB */
  pSnoopEB = snoopEBGet();
  pSnoopCB = snoopCBFirstGet();
  for (cbIndex = L7_NULL; 
       cbIndex < pSnoopEB->maxSnoopInstances; cbIndex++, pSnoopCB++)
  {
    if (snoopVlanEntriesRemove(vlanId, pSnoopCB) != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      LOG_MSG("snoopVlanDelete: Error while removing entries for vlan %d from snoop tree",
                vlanId);
    }
  }

  /* Remove the operational entry */
  rc_2 = snoopOperEntryDelete(vlanId);
  if (rc_2 != L7_SUCCESS && rc_2 != L7_NOT_EXIST)
  {/* NOT_EXIST can happen during clear config, clear vlan */ 
    rc = L7_FAILURE;
    LOG_MSG("snoopVlanDelete: Failed to delete the vlan map index for vlan %d ", 
            vlanId);
  }
  return rc;
}

/*********************************************************************
* @purpose  To process the Callback for VLAN_ADD_PORT_NOTIFY
*
* @param    vlanId   @b{(input)} VLAN ID
* @param    intIfNum @b{(input)} Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t snoopVlanPortAdd(L7_uint32 vlanId, L7_uint32 intIfNum)
{
  snoop_cb_t         *pSnoopCB = L7_NULLPTR;
  snoopIntfCfgData_t *pCfg;
  L7_uint32           cbIndex;
  L7_RC_t             rv = L7_SUCCESS;
  snoop_eb_t         *pSnoopEB;

  pSnoopEB = snoopEBGet();
  pSnoopCB = snoopCBFirstGet();
  for (cbIndex = L7_NULL;
       cbIndex < pSnoopEB->maxSnoopInstances; cbIndex++, pSnoopCB++)
  {
    if (snoopMapIntfIsConfigurable(intIfNum, &pCfg, pSnoopCB) == L7_TRUE)
    {
      /* Snooping enabled on interface basis or
         Snooping enabled on vlan basis */
      if (pCfg->intfMode & SNOOP_VLAN_MODE ||
          pSnoopCB->snoopCfgData->snoopVlanMode[vlanId] & SNOOP_VLAN_MODE)
      {
        if (snoopIntfVlanModeApply(intIfNum, vlanId, L7_ENABLE, 
                                   pSnoopCB) != L7_SUCCESS)
        {
          rv = L7_FAILURE;
          LOG_MSG("snoopVlanPortAdd: snoopIntfVlanModeApply(Enable) failed for interface %d vlan %d family %d",
                  intIfNum, vlanId, pSnoopCB->family);
        }
      }

      /* Enable static mcast router attached interface for this (vlan,interface) */
      if (pCfg->intfMcastRtrAttached == L7_ENABLE ||
          L7_VLAN_ISMASKBITSET(pCfg->vlanStaticMcastRtr, vlanId))
      {
        if (snoopVlanMrouterApply(intIfNum, vlanId, L7_ENABLE, pSnoopCB)
                                  != L7_SUCCESS)
        {
          rv = L7_FAILURE;
          LOG_MSG("snoopVlanPortAdd: snoopVlanMrouterApply(Enable) failed for interface %d vlan %d family %d",
                  intIfNum, vlanId, pSnoopCB->family);
        }
      }
    } /* end of interface configuration structure get */
#ifdef L7_MCAST_PACKAGE
    snoopL3McastVlanPortAdd(vlanId, pSnoopCB);
#endif
  } /* end of snoop instances iterations */
  return rv;
}

/*********************************************************************
* @purpose  To process the Callback for VLAN_DELETE_PORT_NOTIFY
*
* @param    vlanId   @b{(input)} VLAN ID
* @param    intIfNum @b{(input)} Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t snoopVlanPortDelete(L7_uint32 vlanId, L7_uint32 intIfNum)
{
  snoop_cb_t         *pSnoopCB = L7_NULLPTR;
  L7_uint32           cbIndex;
  L7_RC_t             rv = L7_SUCCESS;
  snoop_eb_t         *pSnoopEB;

  pSnoopEB = snoopEBGet();
  pSnoopCB = snoopCBFirstGet();
  for (cbIndex = L7_NULL;
       cbIndex < pSnoopEB->maxSnoopInstances; cbIndex++, pSnoopCB++)
  {
    if (snoopIntfVlanModeApply(intIfNum, vlanId, L7_DISABLE, pSnoopCB) 
                               != L7_SUCCESS)
    {
      rv = L7_FAILURE;
      LOG_MSG("snoopVlanPortDelete: snoopIntfVlanModeApply(Disable) failed for interface %d vlan %d family %d",
              intIfNum, vlanId, pSnoopCB->family);
    }

    /* Disable static mcast router attached interface for this (vlan,interface) */
     if (snoopVlanMrouterApply(intIfNum, vlanId, L7_DISABLE, pSnoopCB)
                               != L7_SUCCESS)
     {
       rv = L7_FAILURE;
       LOG_MSG("snoopVlanPortAdd: snoopVlanMrouterApply(Enable) failed for interface %d vlan %d family %d",
               intIfNum, vlanId, pSnoopCB->family);
     }
  } /* end of snoop instances iterations */

  return rv;
}

/*********************************************************************
* @purpose  To process the Callback for VLAN_DELETE_PORT_NOTIFY
*
* @param    vlanId   @b{(input)} VLAN ID
* @param    intIfNum @b{(input)} Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t snoopAllVlanDelete(void)
{
  snoopOperData_t *pData;
  L7_RC_t          rv = L7_SUCCESS, rc = L7_SUCCESS;
  L7_uint32        vlanId;
  L7_uint32        cbIndex;
  snoop_eb_t      *pSnoopEB;
  snoop_cb_t      *pSnoopCB = L7_NULLPTR;

  /* Remove all entries for this vlan id from the snoop Tree and MFDB */
  pSnoopEB = snoopEBGet();
  pSnoopCB = snoopCBFirstGet();
  for (cbIndex = L7_NULL; 
       cbIndex < pSnoopEB->maxSnoopInstances; cbIndex++, pSnoopCB++)
  { 
    pData = snoopOperEntryGet(L7_NULL, pSnoopCB, L7_MATCH_EXACT);
    while (pData)
    {
      vlanId = pData->vlanId;

      /* Remove the Querier operational entry */
     snoopQuerierVlanModeApply(vlanId, L7_DISABLE, pSnoopCB);

      /* Clean up snoopDB */
      if (snoopVlanEntriesRemove(vlanId, pSnoopCB) != L7_SUCCESS)
      {
        rv = L7_FAILURE;
        LOG_MSG("snoopAllVlanDelete: Error while removing entries for vlan %d from snoop tree",
                 vlanId);
      }

     /* Remove the operational entry */
     rc = snoopOperEntryDelete(vlanId);
     if (rc != L7_SUCCESS && rc != L7_NOT_EXIST)
     {
       rv = L7_FAILURE; 
       LOG_MSG("snoopAllVlanDelete: Failed to delete the vlan map index for vlan %d ", 
               vlanId);
     }

      pData = snoopOperEntryGet(L7_NULL, pSnoopCB, L7_MATCH_EXACT);
    }/* End of while of snoop oper entries*/
  } /* End of snoop instances iterations */
  return rv;
}

/*********************************************************************
* @purpose  To process the Callback for L7_FORWARDING
*
* @param    intIfNum @b{(input)} Internal interface number
* @param    vidList @b{(input)}  Array of VLANs in the MSTID
* @param    numVlans @b{(input)} Number of VLANs in the MSTID's VID List
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t snoopDot1sForwarding(L7_uint32 intIfNum, L7_ushort16 *vidList,
                                    L7_uint32 numVlans)
{
  L7_RC_t             rv = L7_SUCCESS;
  L7_uint32           vlanId, i, vlanMode, cbIndex;
  snoop_cb_t         *pSnoopCB = L7_NULLPTR;
  snoopIntfCfgData_t *pCfg;
  snoop_eb_t         *pSnoopEB;

  pSnoopEB = snoopEBGet();
  for (i = 0; i < numVlans; i++)
  {
    vlanId = (L7_uint32)(vidList[i]);
    if ((snoop_dot1qOperVlanMemberGet(vlanId, intIfNum, &vlanMode) == L7_SUCCESS) 
        && (vlanMode == L7_DOT1Q_FIXED) )
    {
      pSnoopCB = snoopCBFirstGet();
      for (cbIndex = L7_NULL;
          cbIndex < pSnoopEB->maxSnoopInstances; cbIndex++, pSnoopCB++)
      {
        if (snoopMapIntfIsConfigurable(intIfNum, &pCfg, pSnoopCB) 
            == L7_TRUE)
        {
          if ((pSnoopCB->snoopCfgData->snoopVlanMode[vlanId] & SNOOP_VLAN_MODE) || 
              (pCfg->intfMode & SNOOP_VLAN_MODE))
          {
            if (snoopIntfVlanModeApply(intIfNum, vlanId, L7_ENABLE,
                                       pSnoopCB) != L7_SUCCESS)
            {
              rv = L7_FAILURE;
              LOG_MSG("snoopDot1sForwarding: snoopIntfVlanModeApply(Enable) failed for interface %d vlan %d",
                      intIfNum, vlanId);
            }
          }
          else
          {
            if (snoopIntfVlanModeApply(intIfNum, vlanId, L7_DISABLE, 
                                       pSnoopCB) != L7_SUCCESS)
            {
              rv = L7_FAILURE;
              LOG_MSG("snoopDot1sForwarding: snoopIntfVlanModeApply(Disable) failed for interface %d vlan %d",
                      intIfNum, vlanId);
            }
          }
        } /* end of interface configuration structure get */
      }/* end of snoop instances iterations */
    } /* end of mstivlans iterations */
  }
  return rv;
}

/*********************************************************************
* @purpose  To process the Callback for L7_NOT_FORWARDING
*
* @param    intIfNum @b{(input)} Internal interface number
* @param    vidList @b{(input)}  Array of VLANs in the MSTID
* @param    numVlans @b{(input)} Number of VLANs in the MSTID's VID List
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t snoopDot1sNotForwarding(L7_uint32 intIfNum, L7_ushort16 *vidList,
                                       L7_uint32 numVlans)
{
  L7_RC_t     rv = L7_SUCCESS;
  L7_uint32   cbIndex, idx;
  snoop_cb_t *pSnoopCB = L7_NULLPTR;
  snoop_eb_t *pSnoopEB;

  pSnoopEB = snoopEBGet();
  for (idx = 0; idx < numVlans; idx++)
  {
    pSnoopCB = snoopCBFirstGet();
    for (cbIndex = L7_NULL; 
         cbIndex < pSnoopEB->maxSnoopInstances; cbIndex++, pSnoopCB++)
    {
      if (snoopIntfVlanModeApply(intIfNum, (L7_uint32)(vidList[idx]), 
                                 L7_DISABLE, pSnoopCB) != L7_SUCCESS)
      {
        rv = L7_FAILURE;
        LOG_MSG("snoopDot1sNotForwarding: snoopIntfVlanModeApply(Disable) failed for interface %d vlan %d family %d",
                intIfNum, vidList[idx], pSnoopCB->family);
      }
    }
  }
  return rv;
}

/*********************************************************************
* @purpose  To process the Callback for L7_DOT1S_EVENT_TOPOLOGY_CHANGE
*
* @param    mstID    @b{(input)} Internal interface number
* @param    vidList  @b{(input)}  Array of VLANs in the MSTID
* @param    numVlans @b{(input)} Number of VLANs in the MSTID's VID List
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t snoopDot1sTCNProcess(L7_uint32 mstID, L7_ushort16 *vidList,
                                    L7_uint32 numVlans)
{
  L7_RC_t          rv = L7_SUCCESS;
  L7_uint32        cbIndex, idx, intIfNum, mode;
  snoop_cb_t      *pSnoopCB = L7_NULLPTR;
  snoop_eb_t      *pSnoopEB;
  snoopOperData_t *pSnoopOperEntry = L7_NULLPTR;

  for (idx = 0; idx < numVlans; idx++)
  { 
    pSnoopEB = snoopEBGet();
    pSnoopCB = snoopCBFirstGet();
    for (cbIndex = L7_NULL; cbIndex < pSnoopEB->maxSnoopInstances; 
         cbIndex++, pSnoopCB++)
    {
      if ((pSnoopOperEntry = snoopOperEntryGet((L7_uint32)vidList[idx], pSnoopCB, 
                                               L7_MATCH_EXACT))!= L7_NULLPTR)
      {
        for (intIfNum = 1; intIfNum <= L7_MAX_INTERFACE_COUNT; intIfNum++)
        {
          if ((L7_INTF_ISMASKBITSET(pSnoopOperEntry->snoopIntfMode, intIfNum))&& 
              (L7_INTF_ISMASKBITSET(pSnoopOperEntry->mcastRtrAttached, intIfNum) == 0))
          { 
            if ((L7_SUCCESS == snoop_dot1qVlanMemberGet(vidList[idx], intIfNum, &mode))
                && (L7_DOT1Q_FIXED == mode))
            {  
              (void)snoopGeneralQuerySend(intIfNum, (L7_uint32)(vidList[idx]),
                                          pSnoopCB, SNOOP_TCN_QUERY); 
            } /* End of vlan participation check */
          } /* End of snoop operational mode check */
        } /* End of interfaces loop */
      } /* Snoop oper entry get */
    } /* End of control block interations */
  }/* End if VID iterations */
  return rv;
}

/*********************************************************************
* @purpose  To process the route add/modify event of L3 Multicast
*
* @param    mcastMacAddr  @b{(input)} Mac Address of the group registered
* @param    mcastSrcAddr  @b{(input)} Mac Address of the source registered
* @param    srcVlan       @b{(input)}  Ingress VLAN Interface
* @param    vlanMask      @b{(input)} vlan Ids of the outgoing vlan
*                                     routing interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t snoopL3McastAddNotifyProcess(L7_inet_addr_t *mcastGroupAddr,
                                            L7_inet_addr_t *mcastSrcAddr,
                                            L7_int32        srcVlan, 
                                            L7_VLAN_MASK_t *vlanMask)
{
  snoopL3InfoData_t *pSnoopL3Entry;
  L7_uchar8          mcastMacAddr[L7_MAC_ADDR_LEN];

  /* check if entry already exists and modify if found */
  if ((pSnoopL3Entry = snoopL3EntryFind(mcastGroupAddr, mcastSrcAddr,
                                        L7_MATCH_EXACT)) != L7_NULLPTR)
  {
    /* Update existing entry */
    memcpy(&pSnoopL3Entry->outVlanRtrIntfMask, vlanMask, 
           sizeof(L7_VLAN_MASK_t));
    pSnoopL3Entry->srcIntfVlan = srcVlan;
  }
  else
  {
    snoopL3EntryAdd(mcastGroupAddr, mcastSrcAddr, srcVlan, vlanMask);
  }
  /* notify the hardware regarding the change */
  snoopMulticastMacFromIpAddr(mcastGroupAddr, mcastMacAddr);
  snoopNotifyL3Mcast(mcastMacAddr, L7_NULL, L7_NULLPTR);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To process the route delete event of L3 Multicast
*
* @param    mcastMacAddr  @b{(input)} Mac Address of the L3 group 
*                                     notified registered
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t snoopL3McastDeleteNotifyProcess(L7_inet_addr_t *mcastGroupAddr,
                                               L7_inet_addr_t *mcastSrcAddr)
{
  return snoopL3EntryDelete(mcastGroupAddr, mcastSrcAddr);
}

#ifdef L7_MCAST_PACKAGE
/*********************************************************************
* @purpose  To process the lage release event
*
* @param    intIfNum   @b{(input)} Interface just released from a LAG
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static  void snoopL3McastLagPortRelease(L7_uint32 intIfNum)
{
  L7_uchar8        macAddr[L7_MAC_ADDR_LEN];
  snoopInfoData_t *pSnoopEntry = L7_NULLPTR;
  L7_uint32        nextVlanId;
  L7_ushort16      shortVid;
  L7_uint32        mode, cbIndex;
  snoop_cb_t      *pSnoopCB = L7_NULLPTR;
  snoop_eb_t      *pSnoopEB = L7_NULLPTR;

  pSnoopEB = snoopEBGet();
  pSnoopCB = snoopCBFirstGet();
  for (cbIndex = L7_NULL; cbIndex < pSnoopEB->maxSnoopInstances;
       cbIndex++, pSnoopCB++)
  {
    pSnoopEntry = snoopEntryFind(macAddr, L7_NULL, pSnoopCB->family,
                                 L7_MATCH_GETNEXT);
    while (pSnoopEntry)
    {
      /*first 2 bytes are vlan id*/
      memcpy((void *)&shortVid,
             (void *)&pSnoopEntry->snoopInfoDataKey.vlanIdMacAddr[0], 2);
      nextVlanId = (L7_uint32)shortVid;

      /*next six bytes are mac address*/
      memcpy((void *)macAddr,
             (void *)&pSnoopEntry->snoopInfoDataKey.vlanIdMacAddr[2],
             L7_MAC_ADDR_LEN);

      if (snoop_dot1qVlanMemberGet(nextVlanId, intIfNum, &mode) == L7_SUCCESS &&
          mode == L7_DOT1Q_FIXED)
      {
        snoopNotifyL3Mcast(macAddr, nextVlanId, &pSnoopEntry->snoopGrpMemberList);
      }
      pSnoopEntry = snoopEntryFind(macAddr, nextVlanId, pSnoopCB->family,
                                   L7_MATCH_GETNEXT);
    }/* Loop thru the snoop L2 entries */
  }/* CB interations */
}

/***************************************************************************
* @purpose  To process the VLAN Port add event and process L2 Mcast database
*
* @param    vlanId   @b{(input)} VLAN ID of the port that was just made
*                                member
* @param    pSnoopCB @b{(input)} Snooping Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
****************************************************************************/
static  void snoopL3McastVlanPortAdd(L7_uint32 vlanId, snoop_cb_t *pSnoopCB)
{
  L7_uchar8        macAddr[L7_MAC_ADDR_LEN];
  snoopInfoData_t *pSnoopEntry = L7_NULLPTR;
  L7_uint32        nextVlanId;
  L7_ushort16      shortVid;

  /* check if it is a routing vlan, or else we need not process the
     event */
  if (dot1qVlanIntfVlanIdToIntIfNum(vlanId, &nextVlanId) != L7_SUCCESS)
  {
    return;
  }

  memset((void *)macAddr, 0x00, L7_MAC_ADDR_LEN);
  
  pSnoopEntry = snoopEntryFind(macAddr, L7_NULL, pSnoopCB->family, 
                               L7_MATCH_GETNEXT);
  while (pSnoopEntry)
  {
    /*first 2 bytes are vlan id*/
    memcpy((void *)&shortVid,
           (void *)&pSnoopEntry->snoopInfoDataKey.vlanIdMacAddr[0], 2);
    nextVlanId = (L7_uint32)shortVid;

    /*next six bytes are mac address*/
    memcpy((void *)macAddr,
           (void *)&pSnoopEntry->snoopInfoDataKey.vlanIdMacAddr[2],
           L7_MAC_ADDR_LEN);

    if (nextVlanId == vlanId)
    {
      snoopNotifyL3Mcast(macAddr, nextVlanId, &pSnoopEntry->snoopGrpMemberList);
      
    }

    pSnoopEntry = snoopEntryFind(macAddr, nextVlanId, pSnoopCB->family, 
                                 L7_MATCH_GETNEXT);
  }
}
#endif /* L7_MCAST_PACKAGE */
