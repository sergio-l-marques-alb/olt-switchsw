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
#include "dot3ad_api.h"
#include "mirror_api.h"
#include "l7_ip_api.h"

#include "snooping_api.h"
#include "snooping_util.h"
#include "snooping_outcalls.h"
#include "snooping_ctrl.h"
#include "snooping_proto.h"
#include "snooping_db.h"
#include "snooping_debug.h"

#ifdef L7_NSF_PACKAGE
#include "snooping_ckpt.h"
#endif /* L7_NSF_PACKAGE */

#ifdef L7_MCAST_PACKAGE
#include "l7_mcast_api.h"
#endif /* L7_MCAST_PACKAGE */

/* PTin added: IGMP snooping */
#if 1
#include "logger.h"
#include "snooping_defs.h"
#endif

#include "ptin_evc.h"

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
#ifdef L7_MCAST_PACKAGE
/* L3 Mcast Notify processing */
L7_RC_t snoopL3McastAddNotifyProcess(L7_inet_addr_t *mcastGroupAddr,
                                     L7_inet_addr_t *mcastSrcAddr,
                                     L7_int32        srcVlan,
                                     L7_VLAN_MASK_t *vlanMask);
static L7_RC_t snoopL3McastDeleteNotifyProcess(L7_inet_addr_t *mcastGroupAddr,
                                               L7_inet_addr_t *mcastSrcAddr);
static void snoopL3McastModeChangeProcess(L7_uint32 l3Mode);
/* PTin Added: MGMD integration */
#endif

static void snoopMgmdSwitchPortOpenProcess(L7_uint32 serviceId, L7_uint32 portId, L7_uint32 groupAddr, L7_uint32 sourceAddr);
static void snoopMgmdSwitchPortCloseProcess(L7_uint32 serviceId, L7_uint32 portId, L7_uint32 groupAddr, L7_uint32 sourceAddr);

static L7_uchar8 snoopMsgQueueSchedule();


#define SNOOP_SCHEDULE_NONE       0
#define SNOOP_SCHEDULE_IGMP_ONLY  2
#define SNOOP_SCHEDULE_MLD_ONLY   1
#define SNOOP_SCHEDULE_BOTH       3

L7_uchar8 *snoopCnfgrStateNames[SNOOP_PHASE_UNCONFIG_2 + 1] =
{
  "P0", "P1", "P2", "WMU", "P3", "EXE", "U1", "U2"
};

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
//      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
//             "snoopTask: Queues out of sync");
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

    case (snoopNimStartup):
        if (msg.u.snoopNimStartup.startupPhase == NIM_INTERFACE_CREATE_STARTUP)
        {
          snoopNimCreateStartup();
        }
        else if (msg.u.snoopNimStartup.startupPhase == NIM_INTERFACE_ACTIVATE_STARTUP)
        {
          snoopNimActivateStartup();
        }
        else
        {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
                  "Unknown NIM startup event %d", msg.u.snoopNimStartup.startupPhase);
        }
      break;

    case (snoopAdminModeChange):
      if (pSnoopEB->snoopWarmRestart == L7_FALSE || pSnoopEB->activateStartupDone == L7_TRUE)
      {
        /* Enable Snooping Admin Mode */
        (void)snoopAdminModeApply(msg.u.mode, L7_NULL, msg.cbHandle);      /* PTin modified: IGMP snooping */
      }
      break;

    case (snoopIntfModeChange):
      /* Enable Snooping Admin Mode */
      (void)snoopIntfModeApply(msg.intIfNum, msg.u.mode, msg.cbHandle);
      break;

    case (snoopIntfMrouterModeChange):
      /* Interface is made a static multicast router */
      (void)snoopIntfMrouterApply(msg.intIfNum, msg.u.mode, msg.cbHandle);
      break;

    case (snoopVlanChange):
      /* Vlan change notification */
      (void)snoopVlanChangeProcess(&msg.u.vlanData, msg.intIfNum,
                                   msg.vlanEvent);
      break;

    case (snoopVlanModeChange):
      /* Enable VLAN Snooping Mode */
      snoopVlanModeChangeProcess(msg.vlanId, msg.u.mode, msg.cbHandle);
      break;

    case (snoopVlanMrouterModeChange):
     /* To enable/disable static mcast router attached on an interface/vlan */
       snoopVlanMrouterModeChangeProcess(msg.vlanId, msg.intIfNum, msg.u.mode,
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
       SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "snoopTask: Going to process message");
       snoopPacketProcess(&pduMsg);
       break;

    case (snoopFlush):
        /* Clear snoop entries */
        snoopEntriesDoFlush(msg.cbHandle);
      break;

#ifdef L7_MCAST_PACKAGE
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
    case  snoopL3McastModeChange:
          snoopL3McastModeChangeProcess(msg.u.mode);
          break;
#endif /* L7_MCAST_PACKAGE */
     /* Snooping querier messages */
    case snoopQuerierModeChange:
    case snoopQuerierVlanModeChange:
    case snoopQuerierAddressChange:
    case snoopQuerierVlanAddressChange:
    case snoopQuerierNetworkAddressChange:
    case snoopQuerierRtrAddressChange:
    case snoopQuerierVersionChange:
    case snoopQuerierVlanElectionModeChange:
       snoopQuerierProcessMessage(&msg);
       break;

    case snoopHandleNewBackupManager:
#ifdef L7_NSF_PACKAGE
        snoopNewBackupManager();
#endif /* L7_NSF_PACKAGE */
        break;
    case snoopActivateStartupDone:
        break;

    case snoopMgmdSwitchPortOpen:
        snoopMgmdSwitchPortOpenProcess(pduMsg.vlanId, pduMsg.intIfNum, pduMsg.groupAddress, pduMsg.sourceAddress);
        break;

    case snoopMgmdSwitchPortClose:
        snoopMgmdSwitchPortCloseProcess(pduMsg.vlanId, pduMsg.intIfNum, pduMsg.groupAddress, pduMsg.sourceAddress);
        break;

    default:
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopTask(): invalid message type:%d.", msg.msgId);
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
L7_RC_t snoopAdminModeApply(L7_uint32 adminMode, L7_uint16 vlan_id /* PTin added: IGMP snooping */, snoop_cb_t *pSnoopCB)
{
  L7_uint32             intIfNum, cfgIndex, mode;
  L7_uint32             vlanId;
  nimConfigID_t         configIdNull;
  L7_RC_t               rc;
  snoopIntfCfgData_t   *pCfg = L7_NULLPTR;
  snoopOperData_t      *pSnoopOperEntry  = L7_NULLPTR;

  if (adminMode == L7_ENABLE)
  {
    /* Fill in the Vlan ID map by getting all the currently configured VLANs */

    /* Apply Snooping Mode in hardware */
    if (pSnoopCB->family == L7_AF_INET)
    {

      if (snoopIGMPAdminModeApply(L7_ENABLE, vlan_id) != L7_SUCCESS)               /* PTin modified: IGMP snooping */
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
               "snoopAdminModeApply (Enable): snoopIGMPAdminModeApply failed");
        return L7_FAILURE;
      }

    }
    else if (pSnoopCB->family == L7_AF_INET6)
    {
      if (snoopMLDAdminModeApply(L7_ENABLE) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
               "snoopAdminModeApply (Enable): snoopMLDAdminModeApply failed");
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

            if ( (dot1qOperVlanMemberGet(vlanId, intIfNum, &mode) == L7_SUCCESS)
                  &&(mode == L7_DOT1Q_FIXED))
            {
              /* Snoop operational mode */
              if (pCfg->intfMode & SNOOP_VLAN_MODE ||
                  pSnoopCB->snoopCfgData->snoopVlanMode[vlanId] & SNOOP_VLAN_MODE)
              {
                if (snoopIntfVlanModeApply(intIfNum, vlanId, L7_ENABLE, pSnoopCB) != L7_SUCCESS)
                {
                  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
                  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

                  L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
                         "snoopAdminModeApply: Failed to apply mode intf %s vlan %d family %d",
                          ifName, vlanId, pSnoopCB->family);
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
          if (snoopQuerierVlanModeApply(vlanId,
                                        L7_ENABLE, pSnoopCB) != L7_SUCCESS)
          {
            L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
                   "snoopAdminModeApply: Failed to enabled querier "
                      "vlan mode vlan %d family %d", vlanId, pSnoopCB->family);
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
      if (snoopIGMPAdminModeApply(L7_DISABLE, vlan_id) != L7_SUCCESS)              /* PTin modified: IGMP snooping */
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
               "snoopAdminModeApply (Disable): snoopIGMPAdminModeApply failed");
        return L7_FAILURE;
      }
    }
    else if (pSnoopCB->family == L7_AF_INET6)
    {
      if (snoopMLDAdminModeApply(L7_DISABLE) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
               "snoopAdminModeApply (Disable): snoopMLDAdminModeApply failed");
        return L7_FAILURE;
      }
    }
    else
    {
      return L7_FAILURE;
    }

    if (vlan_id == 0)
    {
    #ifdef L7_NSF_PACKAGE
      snoopCheckpointCBFlushCallback(pSnoopCB->family);
    #endif /* L7_NSF_PACKAGE */

      /* Flush all Snooping entries */
      /* PTin modified: IGMP */
      snoopEntriesDoFlush(pSnoopCB);
    }
    else
    {
      snoopEntryDoFlush(vlan_id,pSnoopCB);
    }

    #if 0
    #ifdef L7_NSF_PACKAGE
    snoopCheckpointCBFlushCallback(pSnoopCB->family);
    #endif /* L7_NSF_PACKAGE */

    /* Flush all Snooping entries */
    snoopEntriesDoFlush(pSnoopCB);
    #endif

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

          #ifdef L7_MCAST_PACKAGE
          snoopVlanNotifyL3Mcast(vlanId, L7_FALSE);
          #endif /* L7_MCAST_PACKAGE */
          if (snoopQuerierVlanModeApply(vlanId, L7_DISABLE, pSnoopCB) != L7_SUCCESS)
          {
            L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
                   "snoopAdminModeApply (Disable): Failed to enabled querier vlan mode vlan %d family %d",
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

  if (dot1qVIDListMaskGet(intIfNum, &vidMask) != L7_SUCCESS)
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
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
               "snoopIntfModeApply: Failed to apply mode intf %s vlan %d family %d",
                ifName, vlanId, pSnoopCB->family);
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
#ifdef L7_MCAST_PACKAGE
  L7_BOOL result;
#endif /* L7_MCAST_PACKAGE */

  if (pSnoopCB->snoopCfgData->snoopAdminMode != L7_ENABLE)
  {
    return L7_SUCCESS;
  }

  if (snoopIsValidIntf(intIfNum) != L7_TRUE)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
           "snoopIntfVlanModeApply: snoopIsValidIntf failed for interface %s",
             ifName);
    return L7_FAILURE;
  }

  if ((pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_EXACT))
                       == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (mode == L7_ENABLE)
  {
    /* If interface is already enabled in this VLAN, don't enable again */
    if (L7_INTF_ISMASKBITSET(pSnoopOperEntry->snoopIntfMode, intIfNum))
    {
      return L7_SUCCESS;
    }

    if (snoopIntfCanBeEnabled(intIfNum, vlanId) == L7_TRUE)
    {
#ifdef L7_MCAST_PACKAGE
      L7_INTF_NONZEROMASK(pSnoopOperEntry->snoopIntfMode, result);
      if (result == L7_FALSE)
      {
        snoopVlanNotifyL3Mcast(vlanId, L7_TRUE);
      }
#endif /* L7_MCAST_PACKAGE */
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
#ifdef L7_MCAST_PACKAGE
    L7_INTF_NONZEROMASK(pSnoopOperEntry->snoopIntfMode, result);
    if (result == L7_FALSE)
    {
      snoopVlanNotifyL3Mcast(vlanId, L7_FALSE);
    }
#endif /* L7_MCAST_PACKAGE */

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

  if (dot1qVIDListMaskGet(intIfNum, &vidMask) != L7_SUCCESS)
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
  /* PTin removed: IGMP snooping */
  #if 0
  L7_uint32            intIfNum, vlanStatus;
  L7_RC_t              rc;
  snoopIntfCfgData_t  *pCfg;
  L7_BOOL              intfFlag = L7_FALSE;
  snoopOperData_t     *pSnoopOperEntry;

  rc = nimFirstValidIntfNumber(&intIfNum);
  while (rc == L7_SUCCESS)
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

      if ((dot1qOperVlanMemberGet(vlanId, intIfNum, &vlanStatus) == L7_SUCCESS)
          && (vlanStatus == L7_DOT1Q_FIXED))
      {
        (void)snoopIntfVlanModeApply(intIfNum, vlanId, mode, pSnoopCB);
      }
    }
    rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
  }
  #endif

  /* PTin added: IGMP snooping */
  if (snoopAdminModeApply(mode, vlanId, pSnoopCB)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error configuring to %u vlan %u trapping",mode,vlanId);
  }
  else
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Success configuring to %u vlan %u trapping",mode,vlanId);
  }

  /* Enable Snooping Querier if the vlan is now ready for query processing */
  if (snoopQuerierVlanModeApply(vlanId, mode, pSnoopCB)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error configuring to %u vlan %u querier",mode,vlanId);
  }
  else
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Success configuring to %u vlan %u querier",mode,vlanId);
  }

  /* PTin removed: IGMP snooping */
  #if 0
  /* If no interface is using the operational information, de-initialize it */
  if (mode == L7_DISABLE && intfFlag == L7_FALSE)
  {
    if ((pSnoopOperEntry = snoopOperEntryGet(vlanId,pSnoopCB, L7_MATCH_EXACT)))
    {
      snoopOperEntryDeInit(pSnoopOperEntry);
    }
  }
  #endif
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

  if ( (dot1qOperVlanMemberGet(vlanId, intIfNum, &vlanStatus) == L7_SUCCESS) &&
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
      if ( (dot1qOperVlanMemberGet(vlanId, intIfNum, &mode) == L7_SUCCESS)
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
          L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
          nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
                 "snoopApplyIntfConfigData: Failed to apply configuration intf %s vlan %d family %d",
                  ifName, vlanId, pSnoopCB->family);
          return L7_FAILURE;
        }
      }
      else
      {
        if (snoopIntfVlanModeApply(intIfNum, vlanId, L7_DISABLE, pSnoopCB) != L7_SUCCESS)
        {
          L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
          nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
                 "snoopApplyIntfConfigData: Failed to apply configuration intf %s vlan %d family %d",
                  ifName, vlanId, pSnoopCB->family);
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
          L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
          nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
                 "snoopApplyIntfConfigData: Failed to apply static mrtr configuration intf %s vlan %d family %d",
                  ifName, vlanId, pSnoopCB->family);
          return L7_FAILURE;
        }
      }
      else
      {
        if (snoopVlanMrouterApply(intIfNum, vlanId, L7_DISABLE, pSnoopCB)
                                  != L7_SUCCESS)
        {
          L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
          nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
                 "snoopApplyIntfConfigData: Failed to apply static mrtr configuration intf %s vlan %d family %d",
                  ifName, vlanId, pSnoopCB->family);
          return L7_FAILURE;
        }
      }
      pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_GETNEXT);
    } /* End of VLAN iterations */
  } /* End of interface config get */
  return rc;
}

/*********************************************************************
* @purpose  Handle NIM startup callback
*
* @param    startupPhase    Activate/Create
*
* @returns  void
*
* @notes    Transfer to snooping thread
*
* @end
*********************************************************************/
void snoopIntfStartupCallback(NIM_STARTUP_PHASE_t startupPhase)
{
  snoopMgmtMsg_t  msg;
  snoop_eb_t     *pSnoopEB = L7_NULL;
  L7_RC_t         rc;

  pSnoopEB = snoopEBGet();

  if (pSnoopEB->snoopQueue == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
            "Received NIM startup %d callback while in init state %s",
            startupPhase, snoopCnfgrStateNames[pSnoopEB->snoopCnfgrState]);
    return;
  }

  if (pSnoopEB->snoopQueue != L7_NULL)
  {
    memset(&msg, 0, sizeof(snoopMgmtMsg_t));
    msg.msgId    = snoopNimStartup;
    msg.u.snoopNimStartup.startupPhase = startupPhase;

    rc = osapiMessageSend(pSnoopEB->snoopQueue, &msg, SNOOP_MSG_SIZE,
                          L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM);

    if (rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_SNOOPING_COMPONENT_ID,
              "Failed to send NIM startup phase %d to Snooping thread",
              startupPhase);
    }
    else
    {
      if (osapiSemaGive(pSnoopEB->snoopMsgQSema) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
               "snoopIntfStartupCallback: Failed to give msgQueue semaphore\n");
      }
    }
  }

  return;
}

/*********************************************************************
* @purpose  Do NIM create startup
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes    Snooping has no configuration at this point
*
* @end
*********************************************************************/
L7_RC_t snoopNimCreateStartup(void)
{
  L7_RC_t          rc, rc2;
  L7_uint32        intIfNum;
  snoop_eb_t      *pSnoopEB = L7_NULL;
  snoop_cb_t      *pSnoopCB = L7_NULL;
  L7_uint32        vlanId, cbIndex;

  SNOOP_TRACE(SNOOP_DEBUG_CHECKPOINT, 0, "Snooping Create Startup beginning");
  pSnoopEB = snoopEBGet();

  rc = nimFirstValidIntfNumber(&intIfNum);

  while (rc == L7_SUCCESS)
  {
    if (snoopIsReady() == L7_TRUE)
    {
      /* General snoop interface create */

      if (snoopIsValidIntf(intIfNum) == L7_TRUE)
      {
        rc2 = snoopIntfCreate(intIfNum);
      }
      else if (dot1qVlanIntfIntIfNumToVlanId(intIfNum, &vlanId) == L7_SUCCESS)
      { /* It is a vlan routing interface */
        L7_uint32 l3Mode;

        L7_VLAN_SETMASKBIT(pSnoopEB->routingVlanVidMask, vlanId);
        pSnoopCB = snoopCBFirstGet();
        for (cbIndex = L7_NULL;
             cbIndex < pSnoopEB->maxSnoopInstances; cbIndex++, pSnoopCB++)
        {
          l3Mode = L7_DISABLE;
#ifdef L7_MCAST_PACKAGE
          if (mcastMapMcastIsOperational(pSnoopCB->family) == L7_TRUE)
          {
            l3Mode = L7_ENABLE;
          }
#endif /* L7_MCAST_PACKAGE */
          if (l3Mode == L7_ENABLE)
          {
          /* Disable snooping querier on newly created vlan routing interface */
          (void)snoopQuerierVlanModeApply(vlanId, L7_DISABLE, pSnoopCB);
        }
      }
    }
    }

    rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
  }

  /* Register with NIM to receive port CREATE and DELETE events */
  PORTEVENT_SETMASKBIT(pSnoopEB->nimEventMask, L7_CREATE);
  PORTEVENT_SETMASKBIT(pSnoopEB->nimEventMask, L7_DELETE);
  nimRegisterIntfEvents(L7_SNOOPING_COMPONENT_ID, pSnoopEB->nimEventMask);

  nimStartupEventDone(L7_SNOOPING_COMPONENT_ID);

  pSnoopEB->createStartupDone = L7_TRUE;

  SNOOP_TRACE(SNOOP_DEBUG_CHECKPOINT, 0, "Snooping Create Startup done");
  L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_SNOOPING_COMPONENT_ID,
            "Snooping Create Startup done");

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  See if port for Snooping has been acquired
*
* @param    void
*
* @returns  L7_TRUE if port has been acquired for another purpose
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL snoopPortAcquired(L7_uint32 intIfNum, L7_uint32 *pRoutingEnabled,
                          L7_BOOL *pLagAcquired, L7_BOOL *pProbeSetup)
{
  L7_uint32    routingEnabled = L7_DISABLE;
  L7_BOOL      lagAcquired = L7_FALSE, probeSetup = L7_FALSE,
               portAcquired = L7_FALSE;

  probeSetup = mirrorIsActiveProbePort(intIfNum);
  lagAcquired = dot3adIsLagActiveMember(intIfNum);
  (void) ipMapRtrIntfModeGet(intIfNum, &routingEnabled);

  if (pRoutingEnabled != L7_NULLPTR)
  {
      *pRoutingEnabled = routingEnabled;
  }

  if (pLagAcquired != L7_NULLPTR)
  {
      *pLagAcquired = lagAcquired;
  }

  if (pProbeSetup != L7_NULLPTR)
  {
      *pProbeSetup = probeSetup;
  }

  if ((probeSetup == L7_TRUE) || (lagAcquired == L7_TRUE) ||
      (routingEnabled == L7_ENABLE))
  {
      portAcquired = L7_TRUE;
  }

  return portAcquired;
}

/*********************************************************************
* @purpose  Update snooping port acquisition list for interface
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void snoopPortUpdateAcquisition(L7_uint32 intIfNum, L7_uint32 routingEnabled,
                                L7_BOOL lagAcquired, L7_BOOL probeSetup)
{
    snoop_eb_t      *pSnoopEB = L7_NULL;

    pSnoopEB = snoopEBGet();

    /* update acquired list */
    if (probeSetup == L7_TRUE)
    {
        COMPONENT_ACQ_SETMASKBIT(pSnoopEB->snoopIntfInfo[intIfNum].acquiredList, L7_PORT_MIRROR_COMPONENT_ID);
    }
    else
    {
        COMPONENT_ACQ_CLRMASKBIT(pSnoopEB->snoopIntfInfo[intIfNum].acquiredList, L7_PORT_MIRROR_COMPONENT_ID);
    }

    if (lagAcquired == L7_TRUE)
    {
        COMPONENT_ACQ_SETMASKBIT(pSnoopEB->snoopIntfInfo[intIfNum].acquiredList, L7_DOT3AD_COMPONENT_ID);
    }
    else
    {
        COMPONENT_ACQ_CLRMASKBIT(pSnoopEB->snoopIntfInfo[intIfNum].acquiredList, L7_DOT3AD_COMPONENT_ID);
    }

    if (routingEnabled == L7_ENABLE)
    {
        COMPONENT_ACQ_SETMASKBIT(pSnoopEB->snoopIntfInfo[intIfNum].acquiredList, L7_IP_MAP_COMPONENT_ID);
    }
    else
    {
        COMPONENT_ACQ_CLRMASKBIT(pSnoopEB->snoopIntfInfo[intIfNum].acquiredList, L7_IP_MAP_COMPONENT_ID);
    }

    return;
}

/*********************************************************************
* @purpose  Do NIM activate startup
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t snoopNimActivateStartup(void)
{
  L7_RC_t          rc, rc2;
  L7_uint32        intIfNum;
  L7_INTF_STATES_t intIfState;
  L7_uint32        routingEnabled;
  L7_BOOL          lagAcquired, probeSetup, portAcquired;
  L7_uint32        activeState = L7_INACTIVE;
  snoop_eb_t      *pSnoopEB = L7_NULL;

  SNOOP_TRACE(SNOOP_DEBUG_CHECKPOINT, 0, "Snooping Activate Startup beginning");

  pSnoopEB = snoopEBGet();
  rc = nimFirstValidIntfNumber(&intIfNum);

  while (rc == L7_SUCCESS)
  {
    intIfState = nimGetIntfState(intIfNum);

    switch (intIfState)
    {
      case L7_INTF_UNINITIALIZED:
      case L7_INTF_CREATING:
      case L7_INTF_CREATED: /* Already receiving this event since startup callback */
      case L7_INTF_DELETING: /* Already receiving this event since startup callback */
      case L7_INTF_ATTACHING:
      case L7_INTF_DETACHING: /* Since interface attach was never processed, nothing to do here */
        break;

      case L7_INTF_ATTACHED:
        if (snoopIsValidIntf(intIfNum) != L7_TRUE)
        {
            break;
        }

        /*
        Determine remaining intf state
        */

        portAcquired = snoopPortAcquired(intIfNum, &routingEnabled, &lagAcquired, &probeSetup);
        snoopPortUpdateAcquisition(intIfNum, routingEnabled, lagAcquired, probeSetup);

        if (nimGetIntfActiveState(intIfNum, &activeState) != L7_SUCCESS)
        {
            /*
            Can phase really change here?  It was checked as part of
            nimFirstValidIntfNumber()
            */

            break;
        }

        if ((activeState == L7_ACTIVE) && (portAcquired == L7_FALSE))
        {
          rc2 = snoopIntfActive(intIfNum);
        }

        break;

      default:
        L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_SNOOPING_COMPONENT_ID,
                  "Received unknown NIM intIfState %d", intIfState);
        break;
    }

    rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
  }

  /* Now ask NIM to send any future changes for these event types */
  PORTEVENT_SETMASKBIT(pSnoopEB->nimEventMask, L7_INACTIVE);
  PORTEVENT_SETMASKBIT(pSnoopEB->nimEventMask, L7_ACTIVE);
  PORTEVENT_SETMASKBIT(pSnoopEB->nimEventMask, L7_CREATE);
  PORTEVENT_SETMASKBIT(pSnoopEB->nimEventMask, L7_ATTACH);
  PORTEVENT_SETMASKBIT(pSnoopEB->nimEventMask, L7_DELETE);
  PORTEVENT_SETMASKBIT(pSnoopEB->nimEventMask, L7_DETACH);
  PORTEVENT_SETMASKBIT(pSnoopEB->nimEventMask, L7_DOWN);
  PORTEVENT_SETMASKBIT(pSnoopEB->nimEventMask, L7_LAG_ACQUIRE);
  PORTEVENT_SETMASKBIT(pSnoopEB->nimEventMask, L7_LAG_RELEASE);
  PORTEVENT_SETMASKBIT(pSnoopEB->nimEventMask, L7_PROBE_SETUP);
  PORTEVENT_SETMASKBIT(pSnoopEB->nimEventMask, L7_PROBE_TEARDOWN);
  PORTEVENT_SETMASKBIT(pSnoopEB->nimEventMask, L7_PORT_ROUTING_ENABLED);
  PORTEVENT_SETMASKBIT(pSnoopEB->nimEventMask, L7_PORT_ROUTING_DISABLED);

  nimRegisterIntfEvents(L7_SNOOPING_COMPONENT_ID, pSnoopEB->nimEventMask);

  nimStartupEventDone(L7_SNOOPING_COMPONENT_ID);

  /*
  Do hardware update here
  */

  /* If this is a warm restart, apply checkpointed data. */
  if (pSnoopEB->snoopWarmRestart == L7_TRUE)
  {
#ifdef L7_NSF_PACKAGE
    snoopCheckpointDataApply();
#endif /* L7_NSF_PACKAGE */
  }

  pSnoopEB->activateStartupDone = L7_TRUE;

  cnfgrApiComponentHwUpdateDone(L7_SNOOPING_COMPONENT_ID, L7_CNFGR_HW_APPLY_CONFIG);
  SNOOP_TRACE(SNOOP_DEBUG_CHECKPOINT, 0, "Snooping Activate Startup done");
  L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_SNOOPING_COMPONENT_ID,
            "Snooping Activate Startup done");

  return L7_SUCCESS;
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
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopIntfChangeCallback: Failed to give msgQueue semaphore\n");
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

  switch (event)
  {
    case VLAN_ADD_NOTIFY:
    case VLAN_DELETE_PENDING_NOTIFY:
    case VLAN_ADD_PORT_NOTIFY:
    case VLAN_DELETE_PORT_NOTIFY:
    case VLAN_INITIALIZED_NOTIFY:
    case VLAN_RESTORE_NOTIFY:
      break;
    default:
      return L7_SUCCESS;
  }

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
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopVlanChangeCallback: Failed to give msgQueue semaphore\n");
    }
  }
  else
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
           "snoopVlanChangeCallback: Failed to post message event %d intf %s\n",
            event, ifName);
  }
  return L7_SUCCESS;
}

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
static L7_RC_t snoopVlanChangeProcess(dot1qNotifyData_t *vlanData,
                                      L7_uint32 intIfNum,
                                      L7_uint32 event)
{
  L7_RC_t     rc = L7_SUCCESS;
  L7_uint32      i = 0, vlanId = 0, numVlans = 0;
  snoop_eb_t      *pSnoopEB = L7_NULL;

  pSnoopEB = snoopEBGet();

  if ((pSnoopEB->activateStartupDone != L7_TRUE) &&
      (event != VLAN_ADD_NOTIFY))
  {
    return L7_SUCCESS;
  }

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
                /* Already taken care of all the bits in the
                mask so break out of for loop */
                break;
            }
            else
            {
                /* Vlan is not set check for the next bit
                since there are more bits that are set*/
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
          SNOOP_TRACE(SNOOP_DEBUG_QUERIER, 0,
                      "Process VLAN_ADD_NOTIFY event: Added OperEntry "
                      "VLAN %d, add querier", vlanId);
          snoopQuerierVlanAdd(vlanId);
        }
        break;

      case VLAN_DELETE_PENDING_NOTIFY:
        /* About to delete a VLAN */
          (void)snoopQuerierVlanDelete(vlanId);
         rc = snoopVlanDelete(vlanId);
        break;

      case VLAN_ADD_PORT_NOTIFY:
         rc = snoopVlanPortAdd(vlanId, intIfNum);
        /* Port added to a VLAN */
        break;

      case VLAN_DELETE_PORT_NOTIFY:
        /* Port removed from a VLAN */
        rc = snoopVlanPortDelete(vlanId, intIfNum);
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
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopDot1sIntfChangeCallback: Failed to give msgQueue semaphore\n");
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
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
           "snoopNetworkAddrChanged: Invalid address change event received\n");
    return L7_SUCCESS;
  }
  msg.cbHandle = pSnoopCB;

  if (osapiMessageSend(pSnoopCB->snoopExec->snoopQueue, &msg, SNOOP_MSG_SIZE,
                       L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
           "snoopNetworkAddrChanged: Failed to post a message to queue\n");
  }
  else
  {
    if (osapiSemaGive(pSnoopCB->snoopExec->snoopMsgQSema) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopNetworkAddrChanged: Failed to give msgQueue semaphore\n");
    }
  }
  return L7_SUCCESS;
}
#ifdef L7_ROUTING_PACKAGE
/*********************************************************************
* @purpose  Posts message to vendor code to specify the event of
*           change in routing vlan ip address
*
* @param    intIfNum   @b{(input)} Interface number
* @param    event      @b{(input)} Event type
* @param    pData   @b{(input)} Data
* @param    pEventInfo   @b{(input)} Event Info

* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t snoopRoutingEventCallback(L7_uint32 intIfNum, L7_uint32 event,
                                  void *pData,
                                  ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo)
{
  snoopMgmtMsg_t  msg;
  snoop_cb_t     *pSnoopCB;
  L7_uint32       vlanId;
  ASYNC_EVENT_COMPLETE_INFO_t completionData;

  if ((dot1qVlanIntfIntIfNumToVlanId(intIfNum, &vlanId) == L7_SUCCESS) &&
      (event == L7_RTR_INTF_ENABLE ||
       event == L7_RTR_INTF_DISABLE))
  {
   /* Apply the config change */
    memset(&msg, 0, sizeof(snoopMgmtMsg_t) );
    msg.msgId    = snoopQuerierRtrAddressChange;

    if ((pSnoopCB = snoopCBGet(L7_AF_INET)) == L7_NULLPTR)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopRoutingEventCallback: Invalid address change event received\n");
      return L7_SUCCESS;
    }
    msg.cbHandle = pSnoopCB;
    msg.vlanId = vlanId;

    if (osapiMessageSend(pSnoopCB->snoopExec->snoopQueue, &msg, SNOOP_MSG_SIZE,
                         L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopRoutingEventCallback: Failed to post a message to queue\n");
    }
    else
    {
      if (osapiSemaGive(pSnoopCB->snoopExec->snoopMsgQSema) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
               "snoopRoutingEventCallback: Failed to give msgQueue semaphore\n");
      }
    }
  }/* End of events check */

  memset(&completionData, 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
  completionData.componentId = L7_IPRT_SNOOPING;
  if (pEventInfo != L7_NULLPTR)
  {
    completionData.correlator  = pEventInfo->correlator;
    completionData.handlerId   = pEventInfo->handlerId;
  }
  completionData.async_rc.rc = L7_SUCCESS;
  completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;
  /* Return event completion status  */
  asyncEventCompleteTally(&completionData);
  return L7_SUCCESS;
}
#endif
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
  L7_int32       rc = L7_SUCCESS;
  snoopTimerParams_t msg;
  snoop_eb_t    *pSnoopEB = L7_NULLPTR;

  pSnoopEB = snoopEBGet();
  msg.timerCBHandle = (L7_uint32)ptrData;

#if 0
  if ((pSnoopEB = (snoop_eb_t *)ptrData) == L7_NULLPTR)
    return;
#endif
#if (!PTIN_SNOOP_USE_MGMD)
  rc = osapiMessageSend(pSnoopEB->snoopTimerQueue, &msg, SNOOP_TIMER_MSG_SIZE, L7_NO_WAIT,
                        L7_MSG_PRIORITY_NORM);
#endif
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
           "snoopTimer(): Snooping timer tick send failed\n");
  }
  else
  {
    if (osapiSemaGive(pSnoopEB->snoopMsgQSema) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopTimerExpiryHdlr: MsgQSema give failed");
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
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
           "snoopIntfChangeProcess: Received an interface change callback while not ready to receive it");
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
      else /* (event == L7_PORT_ROUTING_DISABLED) */
      {
        componentID = L7_IP_MAP_COMPONENT_ID;
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
        else /* (event == L7_PORT_ROUTING_ENABLED) */
        {
          componentID = L7_IP_MAP_COMPONENT_ID;
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
#if 1 /* Bypass MFDB port removal when MGMD is active and we receive a link down event */
    break;
#endif
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
    else if (dot1qVlanIntfIntIfNumToVlanId(intIfNum, &vlanId) == L7_SUCCESS)
    { /* It is a vlan routing interface */
        L7_uint32 l3Mode;

      L7_VLAN_SETMASKBIT(pSnoopEB->routingVlanVidMask, vlanId);
      pSnoopCB = snoopCBFirstGet();
      for (cbIndex = L7_NULL;
           cbIndex < pSnoopEB->maxSnoopInstances; cbIndex++, pSnoopCB++)
      {
       /* Disable snooping querier on newly created vlan routing interface if
          layer 3 mcast mode is enabled */
        l3Mode = L7_DISABLE;
#ifdef L7_MCAST_PACKAGE
        if (mcastMapMcastIsOperational(pSnoopCB->family) == L7_TRUE)
        {
          l3Mode = L7_ENABLE;
        }
#endif /* L7_MCAST_PACKAGE */
        if (l3Mode == L7_ENABLE)
        {
          (void)snoopQuerierVlanModeApply(vlanId, L7_DISABLE, pSnoopCB);
        }
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
    rc = snoopIntfDelete(intIfNum);
    if (dot1qVlanIntfIntIfNumToVlanId(intIfNum, &vlanId) == L7_SUCCESS)
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

  if (pSnoopCB->snoopExec->snoopIntfMapTbl[intIfNum] != 0)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
            "Interface structure already exists for interface %d, %s when L7_CREATE event received",
            intIfNum, ifName);
    /*
    return L7_SUCCESS;
    */
  }

  for (cid = 1; cid < L7_IGMP_SNOOPING_MAX_INTF; cid++)
  {
    /* The index table is common for MLD and IGMP */
    if (NIM_CONFIG_ID_IS_EQUAL(&(pSnoopCB->snoopCfgData->snoopIntfCfgData[cid].configId),
                               &configId))
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_SNOOPING_COMPONENT_ID,
              "snoopIntfCreate: Found config ID for interface %d, %s, family %d cbIndex %d\n",
              intIfNum, ifName, pSnoopCB->family, pSnoopCB->cbIndex);
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
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SNOOPING_COMPONENT_ID,
                "snoopIntfCreate Failure for snoopMapIntfConfigEntryGet for interface %d, %s family %d cbIndex %d\n",
                intIfNum, ifName, pSnoopCB->family, pSnoopCB->cbIndex);
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
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopIntfAttach: Failed to attach intf %s family %d\n",
              ifName, pSnoopCB->family);
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
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
               "snoopIntfDetach: Failed to detach intf %s family %d\n",
                 ifName, pSnoopCB->family);
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
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
               "snoopIntfDetach: Failed to detach intf %s family %d\n",
                  ifName, pSnoopCB->family);
         return L7_FAILURE;
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
      dot1qVIDListMaskGet(intIfNum,&vidMask);
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
              L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
              nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

              rv = L7_FAILURE;
              L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
                     "snoopIntfActive: Mcast router Apply failed for interface %s vlan %d family %d",
                      ifName, vlanId, pSnoopCB->family);
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
      dot1qVIDListMaskGet(intIfNum,&vidMask);
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
            L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
            nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

            rv = L7_FAILURE;
            L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
                   "snoopIntfInActive: interface vlan mode apply failed for interface %s vlan %d family %d",
                    ifName, vlanId, pSnoopCB->family);
          }
        }

        /* Remove this interface from the static mcast router list */
        if (L7_INTF_ISMASKBITSET(pSnoopOperEntry->mcastRtrAttached, intIfNum))
        {
          rc = snoopVlanMrouterApply(intIfNum, vlanId, L7_DISABLE,
                                     pSnoopCB);
          if (rc != L7_SUCCESS)
          {
            L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
            nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

            rv = L7_FAILURE;
            L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
                   "snoopIntfInActive: Mcast router Apply failed for interface %s vlan %d family %d",
                    ifName, vlanId, pSnoopCB->family);
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
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopVlanDelete: Error while removing entries for vlan %d from snoop tree",
                vlanId);
    }
  }

  /* Remove the operational entry */
  rc_2 = snoopOperEntryDelete(vlanId);
  if (rc_2 != L7_SUCCESS && rc_2 != L7_NOT_EXIST)
  {/* NOT_EXIST can happen during clear config, clear vlan */
    rc = L7_FAILURE;
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
           "snoopVlanDelete: Failed to delete the vlan map index for vlan %d ",
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
      if (pCfg->intfMode & SNOOP_VLAN_MODE || ((vlanId <= L7_DOT1Q_MAX_VLAN_ID) &&
          (pSnoopCB->snoopCfgData->snoopVlanMode[vlanId] & SNOOP_VLAN_MODE)))
      {
        if (snoopIntfVlanModeApply(intIfNum, vlanId, L7_ENABLE,
                                   pSnoopCB) != L7_SUCCESS)
        {
          L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
          nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

          rv = L7_FAILURE;
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
                 "snoopVlanPortAdd: snoopIntfVlanModeApply(Enable) failed for interface %s vlan %d family %d",
                  ifName, vlanId, pSnoopCB->family);
        }
      }

      /* Enable static mcast router attached interface for this (vlan,interface) */
      if (pCfg->intfMcastRtrAttached == L7_ENABLE ||
          L7_VLAN_ISMASKBITSET(pCfg->vlanStaticMcastRtr, vlanId))
      {
        if (snoopVlanMrouterApply(intIfNum, vlanId, L7_ENABLE, pSnoopCB)
                                  != L7_SUCCESS)
        {
          L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
          nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

          rv = L7_FAILURE;
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
                 "snoopVlanPortAdd: snoopVlanMrouterApply(Enable) failed for interface %s vlan %d family %d",
                  ifName, vlanId, pSnoopCB->family);
        }
      }
    } /* end of interface configuration structure get */
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
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      rv = L7_FAILURE;
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopVlanPortDelete: snoopIntfVlanModeApply(Disable) failed for interface %s vlan %d family %d",
              ifName, vlanId, pSnoopCB->family);
    }

    /* Disable static mcast router attached interface for this (vlan,interface) */
     if (snoopVlanMrouterApply(intIfNum, vlanId, L7_DISABLE, pSnoopCB)
                               != L7_SUCCESS)
     {
       L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
       nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

       rv = L7_FAILURE;
       L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
              "snoopVlanPortAdd: snoopVlanMrouterApply(Enable) failed for interface %s vlan %d family %d",
               ifName, vlanId, pSnoopCB->family);
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
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
               "snoopAllVlanDelete: Error while removing entries for vlan %d from snoop tree",
                 vlanId);
      }

     /* Remove the operational entry */
     rc = snoopOperEntryDelete(vlanId);
     if (rc != L7_SUCCESS && rc != L7_NOT_EXIST)
     {
       rv = L7_FAILURE;
       L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
              "snoopAllVlanDelete: Failed to delete the vlan map index for vlan %d ",
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
    if ((dot1qOperVlanMemberGet(vlanId, intIfNum, &vlanMode) == L7_SUCCESS)
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
              L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
              nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

              rv = L7_FAILURE;
              L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
                     "snoopDot1sForwarding: snoopIntfVlanModeApply(Enable) failed for interface %s vlan %d",
                      ifName, vlanId);
            }
          }
          else
          {
            if (snoopIntfVlanModeApply(intIfNum, vlanId, L7_DISABLE,
                                       pSnoopCB) != L7_SUCCESS)
            {
              L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
              nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

              rv = L7_FAILURE;
              L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
                     "snoopDot1sForwarding: snoopIntfVlanModeApply(Disable) failed for interface %s vlan %d",
                      ifName, vlanId);
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
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

        rv = L7_FAILURE;
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
               "snoopDot1sNotForwarding: snoopIntfVlanModeApply(Disable) failed for interface %s vlan %d family %d",
                ifName, vidList[idx], pSnoopCB->family);
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
            if ((L7_SUCCESS == dot1qVlanMemberGet(vidList[idx], intIfNum, &mode))
                && (L7_DOT1Q_FIXED == mode))
            {
              (void)snoopQuerySend(intIfNum, (L7_uint32)(vidList[idx]),
                                          pSnoopCB, SNOOP_TCN_QUERY, L7_NULLPTR);
            } /* End of vlan participation check */
          } /* End of snoop operational mode check */
        } /* End of interfaces loop */
      } /* Snoop oper entry get */
    } /* End of control block interations */
  }/* End if VID iterations */
  return rv;
}

#ifdef L7_MCAST_PACKAGE
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
L7_RC_t snoopL3McastAddNotifyProcess(L7_inet_addr_t *mcastGroupAddr,
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

/*********************************************************************
* @purpose  Propogate MCAST Event notifications to snooping task
*
* @param    event       @b{(input)} Event type
* @param    pData       @b{(input)} Data
* @param    pEventInfo  @b{(input)} Event Info
*
* @returns  L7_SUCCESS
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snoopMcastEventCallback(L7_uint32 event, void *pData,
                                ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo)
{
  snoopMgmtMsg_t               msg;
  snoop_eb_t                  *pSnoopEB = L7_NULLPTR;
  ASYNC_EVENT_COMPLETE_INFO_t  completionData;

  pSnoopEB = snoopEBGet();

  if (event == MCAST_RTR_ADMIN_MODE_ENABLED ||
      event == MCAST_RTR_ADMIN_MODE_DISABLE_PENDING)
  {
    /* Apply the config change */
    memset(&msg, 0, sizeof(snoopMgmtMsg_t) );
    msg.msgId   = snoopL3McastModeChange;
    msg.u.mode  = (event == MCAST_RTR_ADMIN_MODE_ENABLED) ? L7_ENABLE : L7_DISABLE;

    if (osapiMessageSend(pSnoopEB->snoopQueue, &msg, SNOOP_MSG_SIZE,
                         L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopMcastEventCallback: Failed to post a message to queue\n");
    }
    else
    {
      if (osapiSemaGive(pSnoopEB->snoopMsgQSema) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
               "snoopMcastEventCallback: Failed to give msgQueue semaphore\n");
      }
    }
  }

  memset(&completionData, 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
  completionData.componentId     = L7_MRP_SNOOPING;
  if (pEventInfo != L7_NULLPTR)
  {
  completionData.correlator      = pEventInfo->correlator;
  completionData.handlerId       = pEventInfo->handlerId;
  }
  completionData.async_rc.rc     = L7_SUCCESS;
  completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;
  /* Return event completion status  */
  asyncEventCompleteTally(&completionData);

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Process L3 multicast operational mode to enable/disable
*           snooping querier operationaly.
*
* @param    mode       @b{(input)} IP multicast operational mode
*
* @returns  L7_SUCCESS
*
* @comments none
*
* @end
*********************************************************************/
static void snoopL3McastModeChangeProcess(L7_uint32 l3Mode)
{
  L7_uint32   cbIndex;
  snoop_cb_t *pSnoopCB = L7_NULLPTR;
  L7_uint32   querierMode, vlanId;
  snoop_eb_t      *pSnoopEB = L7_NULLPTR;

  pSnoopEB = snoopEBGet();

  querierMode = (l3Mode == L7_ENABLE) ? L7_DISABLE: L7_ENABLE;
  pSnoopCB = snoopCBFirstGet();
  for (cbIndex = L7_NULL;
       cbIndex < pSnoopEB->maxSnoopInstances; cbIndex++, pSnoopCB++)
  {
    for (vlanId = 1; vlanId <= L7_DOT1Q_MAX_VLAN_ID; vlanId++)
    {
      if (L7_VLAN_ISMASKBITSET(pSnoopEB->routingVlanVidMask,vlanId))
      {
        if (snoopQuerierVlanModeApply(vlanId, querierMode, pSnoopCB) != L7_SUCCESS)
        {
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
                   "snoopL3McastModeChangeProcess: Failed to set querier vlan mode vlan %d family %d mode %d",
                   vlanId, pSnoopCB->family, querierMode);
        }
      }/* Routing VLAN check */
    }/* VLAN iterations */
  }/* CB iterations */
}
#endif /* L7_MCAST_PACKAGE */

static void snoopMgmdSwitchPortOpenProcess(L7_uint32 serviceId, L7_uint32 portId, L7_uint32 groupAddr, L7_uint32 sourceAddr)
{
  L7_uint16      mcastRootVlan;
  L7_inet_addr_t groupIp;

  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Received request to open a new port on the switch [serviceId:%u portId:%u groupAddr:%08X sourceAddr:%08X]", serviceId, portId, groupAddr, sourceAddr);

  if( L7_SUCCESS != ptin_evc_intRootVlan_get(serviceId, &mcastRootVlan))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to get mcastRootVlan from serviceId");
  }

  inetAddressSet(L7_AF_INET, &groupAddr, &groupIp);
  if(L7_SUCCESS != snoopGroupIntfAdd(mcastRootVlan, &groupIp, portId, L7_TRUE))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to open port on switch for intVlan:%u groupAddr:%08X intfNum:%u", mcastRootVlan, groupIp.addr.ipv4.s_addr, portId);
  }
}

static void snoopMgmdSwitchPortCloseProcess(L7_uint32 serviceId, L7_uint32 portId, L7_uint32 groupAddr, L7_uint32 sourceAddr)
{
  L7_uint16      mcastRootVlan;
  L7_inet_addr_t groupIp;

  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Received request to close an existing port on the switch [serviceId:%u portId:%u groupAddr:%08X sourceAddr:%08X]", serviceId, portId, groupAddr, sourceAddr);

  if( L7_SUCCESS != ptin_evc_intRootVlan_get(serviceId, &mcastRootVlan))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to get mcastRootVlan from serviceId");
  }

  inetAddressSet(L7_AF_INET, &groupAddr, &groupIp);
  if(L7_SUCCESS != snoopGroupIntfRemove(mcastRootVlan, &groupIp, portId))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to close port on switch for intVlan:%u groupAddr:%08X intfNum:%u", mcastRootVlan, groupIp.addr.ipv4.s_addr, portId);
  }
}

