/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  ds_main.c
*
* @purpose   DHCP snooping
*
* @component DHCP snooping
*
* @comments none
*
* @create 3/26/2007
*
* @author Rob Rice (rrice)
*
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "usmdb_common.h"
#include "usmdb_ip_base_api.h"
#include "usmdb_mib_bridge_api.h"
#include "usmdb_util_api.h"
#include "osapi.h"
#include "log.h"
#include "nimapi.h"
#include "comm_mask.h"
#include "trapapi.h"


#include "ds_util.h"
#include "ds_cfg.h"
#include "ds_cnfgr.h"
#include "dhcp_snooping_api.h"
#include "dhcp_snooping_exports.h"
#include "ptin_dhcp.h"

#include "dot1q_api.h"
#include "dot3ad_api.h"
#ifdef L7_ROUTING_PACKAGE
#include "l7_relay_api.h"
#endif

#ifdef L7_DHCPS_PACKAGE
#include "usmdb_dhcps_api.h"
#endif

#ifdef L7_DVLAN_PACKAGE
#include "dvlantag_api.h"
#endif

#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
#include "dot1ad_api.h"
#include "dot1ad_l2tp_api.h"
#endif

/* PTin added: DHCP snooping */
#if 1
#include "ptin_dhcp.h"
#include "ptin_evc.h"
#include "ptin_intf.h"
#include "logger.h"
#endif

extern dsCfgData_t *dsCfgData;
extern dsCnfgrState_t dsCnfgrState;
extern osapiRWLock_t dsCfgRWLock;

extern void *dsMsgQSema;
extern void *Ds_Event_Queue;
extern void *Ds_Packet_Queue;

/* Global status data */
extern dsInfo_t *dsInfo;

/* Array of per interface working data. Indexed on internal interface number */
extern dsIntfInfo_t *dsIntfInfo;
extern osapiTimerDescr_t *dsLeaseTimer;

extern L7_uchar8 DS_DHCP_OPTIONS_COOKIE[];

L7_BOOL downloadFlag=L7_FALSE;
L7_BOOL doubleTaggedTx = L7_TRUE;

L7_uchar8 *vlanEventNames[] = {"Invalid", "ADD", "DELETE PENDING", "DELETE", "ADD PORT",
                               "DELETE PORT", "START TAGGING PORT", "STOP TAGGING PORT",
                               "INITIALIZED", "RESTORE"};

L7_uchar8 *dhcpMsgTypeNames[] = {"UNKNOWN", "DISCOVER", "OFFER", "REQUEST",
                                 "DECLINE", "ACK", "NACK", "RELEASE"};

L7_RC_t dsVlanEventProcess(dot1qNotifyData_t *pVlanData, L7_uint32 intIfNum, L7_uint32 event);

static void dsPeriodicStore();
#ifdef L7_DHCP_L2_RELAY_PACKAGE
static L7_RC_t dsRelayAgentInfoAdd(L7_uint32 intIfNum, L7_uint32 vlanId,
                            L7_ushort16 innerVlanId,
                            L7_uchar8 *frame, L7_uint32 *frameLen);
static L7_RC_t dsRelayAgentInfoRemoveOrGet (L7_uchar8 *frame,
                                            L7_uint32 *frameLen,
                                            L7_dhcp_packet_t *packet,
                                            L7_uint32 dhcpPacketLength,
                                            L7_BOOL optionsRemove,
                                            dsRelayAgentInfo_t *relayAgentInfo);
#endif
void dsFrameVlanTagRemove(L7_uchar8 *frame, L7_ushort16 *frameLen, L7_uchar8 *frameOut);


void dsFrameDoubleVlanTagAdd(L7_uint32 intIfNum, L7_uint32 vlanId, L7_uchar8 *frame,
                       L7_ushort16 *frameLen, L7_ushort16 innerVlanId, L7_uchar8 *frameOut);

L7_BOOL dsL2RelayServiceIsEnabled(L7_uint32 intIfNum, L7_uint32 vlanId,
                                  L7_ushort16 innerVlanId);

L7_BOOL dsSnoopingServiceIsEnabled(L7_uint32 intIfNum, L7_uint32 vlanId,
                                   L7_uint32 rxSVlanId, L7_uchar8 dhcpOp,
                                   L7_uint32 *cVlanId, L7_uint32 *sVlanId);
L7_RC_t dsReplyFrameForward(L7_uint32 intIfNum, L7_uint32 vlanId, L7_uchar8 *macaddr,
                            L7_uchar8 *frame, L7_ushort16 frameLen,
                            L7_ushort16 innerVlanId, L7_uint client_idx);                   /* PTin modified: DHCP snooping */
L7_RC_t dsFrameIntfFilterSend(L7_uint32 intIfNum, L7_ushort16 vlanId,
                       L7_uchar8 *frame, L7_ushort16 frameLen,
                       L7_BOOL requestFlag, L7_ushort16 innerVlanId, L7_uint client_idx);   /* PTin modified: DHCP snooping */

void dsUdpCheckSumCalculate(L7_uchar8 *frame, L7_uint32 *frameLen,
                            L7_BOOL added, L7_ushort16 lenChng);

static L7_RC_t dsv6AddOption9(L7_uchar8 *frame, L7_uint32 *frameLen, L7_uchar8 *dhcpRelayFrame, L7_ushort16 dhcpRelayFrameLen);

static L7_RC_t dsv6AddOption18or37(L7_uint32 intIfNum, L7_uchar8 *frame, L7_uint32 *frameLen, L7_ushort16 vlanId,
                                   L7_ushort16 innerVlanId, L7_uchar8 *macAddr, L7_dhcp6_opttype_t dhcpOp);

static L7_RC_t dsDHCPv6ClientFrameProcess(L7_uint32 intIfNum, L7_ushort16 vlanId, L7_uchar8 *frame, L7_ushort16 innerVlanId,
                                          L7_uint client_idx);

static L7_RC_t dsDHCPv6ServerFrameProcess(L7_uint32 intIfNum, L7_ushort16 vlanId, L7_uchar8 *frame, L7_ushort16 innerVlanId,
                                          L7_uint client_idx);

/*********************************************************************
* @purpose  Main function for the DHCP snooping thread. Read incoming
*           messages (events and DHCP packets) and process accordingly.
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void dhcpSnoopTask(void)
{
  dsEventMsg_t eventMsg;
  dsFrameMsg_t frameMsg;
  L7_RC_t rc;
  L7_uint32 val;

  /* Loop forever, processing incoming messages */
  while (L7_TRUE)
  {
    /* Since we are reading from multiple queues, we cannot wait forever
     * on the message receive from each queue. Use a semaphore to indicate
     * whether any queue has data. Wait until data is available. */
    if (osapiSemaTake(dsMsgQSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
             "Failure taking DHCP snooping message queue semaphore.");
      continue;
    }
    memset(&eventMsg, 0, sizeof(dsEventMsg_t));
    memset(&frameMsg, 0, sizeof(dsFrameMsg_t));

    /* Always try to read from the event queue first. No sense processing
     * DHCP packets with stale state. */
    if (osapiMessageReceive(Ds_Event_Queue, &eventMsg,
                            sizeof(dsEventMsg_t), L7_NO_WAIT) == L7_SUCCESS)
    {
      /* Configuration actions are all done on the UI thread.
       * Synchronize that processing with event processing by
       * taking the same semaphore here. */
      switch (eventMsg.msgType)
      {
        case DS_VLAN_EVENT:
          if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) == L7_SUCCESS)
          {
            dsVlanEventProcess(&eventMsg.dsMsgData.vlanEvent.vlanData,
                               eventMsg.dsMsgData.vlanEvent.intIfNum,
                               eventMsg.dsMsgData.vlanEvent.vlanEventType);
            osapiWriteLockGive(dsCfgRWLock);
          }
          break;

        case DS_CNFGR_INIT:
          if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) == L7_SUCCESS)
          {
            dsCnfgrHandle(&eventMsg.dsMsgData.cmdData);
            osapiWriteLockGive(dsCfgRWLock);
          }
          break;

        case DS_TIMER_EVENT:
          if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) == L7_SUCCESS)
          {
            dsLeaseCheck();
            dsRateLimitCheck();
            if ( downloadFlag == L7_TRUE)
            {
              if ( (((rc = usmDbAgentIpIfAddressGet(0, &val))==L7_SUCCESS) && (val > 0) && (val != FD_SIM_DEFAULT_NETWORK_IP_ADDR)) ||
                  (((rc = usmDbServicePortIPAddrGet(0,&val))==L7_SUCCESS) && (val > 0) && (val != FD_SIM_DEFAULT_SERVPORT_IP_ADDR)) )
              {
                downloadFlag = L7_FALSE;
                dsTftpDownloadInit();
              }
            }
            dsPeriodicStore();

            osapiWriteLockGive(dsCfgRWLock);
          }
          break;
        case DS_CFG_APPLY_EVENT:
          if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) == L7_SUCCESS)
          {
            dstxtCfgEventProcess(eventMsg.dsMsgData.cfgApplyEvent.txtCfgEventType);
            osapiWriteLockGive(dsCfgRWLock);
          }
          break;

        case DS_DHCPS_ACK_EVENT:
          if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) == L7_SUCCESS)
          {
            if (dsBindingIpAddrSet(&eventMsg.dsMsgData.dhcpsEvent.chAddr,
                                osapiNtohl(eventMsg.dsMsgData.dhcpsEvent.ipAddr))
                                  == L7_SUCCESS)
            {
              dsBindingTreeKey_t key;

              dsInfo->debugStats.bindingsAdded++;
              memset(&key, 0x00, sizeof(key));
              memcpy(&key.macAddr.addr, &eventMsg.dsMsgData.dhcpsEvent.chAddr.addr, L7_ENET_MAC_ADDR_LEN);
              key.ipType = L7_AF_INET;
              dsBindingLeaseSet(&key, eventMsg.dsMsgData.dhcpsEvent.leaseTime);
            }
            osapiWriteLockGive(dsCfgRWLock);
          }
          break;
        case DS_DHCPS_NACK_EVENT:
          if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) == L7_SUCCESS)
          {
            dsBindingTreeKey_t key;

            memset(&key, 0x00, sizeof(key));
            memcpy(&key.macAddr.addr, &eventMsg.dsMsgData.dhcpsEvent.chAddr.addr, L7_ENET_MAC_ADDR_LEN);
            key.ipType = L7_AF_INET;
            dsBindingRemove(&key);
            dsInfo->debugStats.bindingsRemoved++;
            osapiWriteLockGive(dsCfgRWLock);
          }
          break;
        case DS_DB_DOWNLOAD_EVENT:
          if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) == L7_SUCCESS)
          {
            if (eventMsg.dsMsgData.tftpEvent.dsDbDownloadStatus == L7_TRUE)
            {
               dsDbRemoteRestore();
               osapiWriteLockGive(dsCfgRWLock);
            }
            else
            {
             L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
                "Downloading of DHCP Snooping bindings failed. It may cause traffic loss."
                " This message appears when downloading of the DHCP Snooping bindings"
                "database fails.");
             osapiWriteLockGive(dsCfgRWLock);
            }

          }
          break;
        case DS_DB_UPLOAD_EVENT:
          /* To handle the failure of downloading the snooping database */
          if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) == L7_SUCCESS)
          {
            if (eventMsg.dsMsgData.tftpEvent.dsDbDownloadStatus == L7_FALSE)
            {
              L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
                "Uploading of DHCP Snooping bindings failed."
                " This message appears when uploading of the DHCP"
                " Snooping bindings database fails.");
               dsInfo->dsDbDataChanged = L7_TRUE;
            }
            osapiWriteLockGive(dsCfgRWLock);
          }
          break;
          case DS_NIM_STARTUP:
            if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) == L7_SUCCESS)
            {
              if (eventMsg.dsMsgData.nimStartupEvent.startupPhase == NIM_INTERFACE_CREATE_STARTUP)
              {
                dsNimCreateStartup();
              }
              else if (eventMsg.dsMsgData.nimStartupEvent.startupPhase == NIM_INTERFACE_ACTIVATE_STARTUP)
              {
                dsNimActivateStartup();
              }
              else
              {
                L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
                        "Unknown NIM startup event %d", eventMsg.dsMsgData.nimStartupEvent.startupPhase);
              }
              osapiWriteLockGive(dsCfgRWLock);
            }
            break;
        case DS_INTF_EVENT:
          if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) == L7_SUCCESS)
          {
            dsIntfChangeProcess(eventMsg.dsMsgData.intfEvent.intIfNum,
                               eventMsg.dsMsgData.intfEvent.event,
                               eventMsg.dsMsgData.intfEvent.correlator);
            osapiWriteLockGive(dsCfgRWLock);
          }
          break;
        default:
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
                 "Unknown event message type received in the DHCP snooping thread.");
          break;
      }
    }

    /* If the event queue was empty, see if a DHCP message has arrived. */
    else if (osapiMessageReceive(Ds_Packet_Queue, &frameMsg,
                                 sizeof(dsFrameMsg_t), L7_NO_WAIT) == L7_SUCCESS)
    {
      if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) == L7_SUCCESS)
      {
        dsFrameProcess(frameMsg.rxIntf, frameMsg.vlanId, frameMsg.frameBuf,
                       frameMsg.frameLen, frameMsg.innerVlanId, frameMsg.client_idx);     /* PTin modified: DHCP snooping */
        osapiWriteLockGive(dsCfgRWLock);
      }
    }

    /* Someone gave the msg queue semaphore, but didn't put a message on
     * either queue. Count it. */
    else
    {
      dsInfo->debugStats.msgRxError++;
    }
  } /* end while true */

  return;
}


/*********************************************************************
* @purpose  Callback function to process VLAN changes.
*
* @param    vlanId     @b{(input)}   VLAN ID
* @param    intIfnum   @b{(input)}   internal interface whose state has changed
* @param    event      @b{(input)}   VLAN event (see vlanNotifyEvent_t for list)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    process on own thread
*
* @end
*********************************************************************/
L7_RC_t dsVlanChangeCallback(dot1qNotifyData_t *vlanDataCopy, L7_uint32 intIfNum,
                             L7_uint32 event)
{
  NIM_INTF_MASK_t portMask;
  L7_uint32 i;
  L7_RC_t rc = L7_SUCCESS;
  dsEventMsg_t msg;
  L7_uint32 vid, vlanId, numVlans = 0;
  L7_uint32 tempNumVlans, vlanIdTemp = L7_NULL;
  dot1qNotifyData_t vlanData;

  if (Ds_Event_Queue == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
           "DHCP snooping event queue is NULL when processing VLAN change.");
    return L7_FAILURE;
  }

  /* Ignore events types we don't care about.  */
  if ((event != VLAN_DELETE_PENDING_NOTIFY) &&
      (event != VLAN_ADD_PORT_NOTIFY) &&
      (event != VLAN_DELETE_PORT_NOTIFY))
  {
    return L7_SUCCESS;
  }

  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  /* Ignore interfaces we don't care about. */
  if (intIfNum && !dsIntfIsValid(intIfNum))
  {
    osapiReadLockGive(dsCfgRWLock);
    return L7_SUCCESS;
  }
  /*make a copy of the vlan mask as we modify the bits of vlans we are not interested in */
  memcpy(&vlanData, vlanDataCopy, sizeof(dot1qNotifyData_t));
  tempNumVlans = vlanData.numVlans;

  for (vid = 1; vid<=L7_VLAN_MAX_MASK_BIT; vid++)
  {
    if (vlanData.numVlans == 1)
    {
      vlanId = vlanData.data.vlanId;
      /* For any continue, we will break out */
      vid = L7_VLAN_MAX_MASK_BIT + 1;
    }
    else
    {
      if (L7_VLAN_ISMASKBITSET(vlanData.data.vlanMask,vid))
      {
         vlanId = vid;
      }
      else
      {
         if (numVlans == tempNumVlans)
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
    /* Ignore events for VLANs where DHCP snooping not enabled. */
    if (!_dsVlanEnableGet(vlanId))
    {
      if (vlanData.numVlans == 1)
       {
         osapiReadLockGive(dsCfgRWLock);
         return L7_SUCCESS;
       }
       else
       {
         /*Reset the bit in the mask and continue */
         L7_VLAN_CLRMASKBIT(vlanData.data.vlanMask, vid);
         tempNumVlans--;
         continue;
       }
    }
    else
    {
      /* reserve a VLAN ID to check there exists only one Snooping enabled VLANID*/
      vlanIdTemp = vlanId;
    }

    /* Only trace VLAN events we care about */
    if (dsCfgData->dsTraceFlags & DS_TRACE_VLAN_EVENTS)
    {
      L7_uchar8 dsTrace[DS_MAX_TRACE_LEN];
      osapiSnprintf(dsTrace, DS_MAX_TRACE_LEN,
                    "DHCP snooping received %d event for VLAN %u ",
                    event, vlanId);
      if (intIfNum)
      {
        L7_uchar8 intfStr[64];
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
        osapiSnprintf(intfStr, 64, "and interface %s.", ifName);
        strncat(dsTrace, intfStr, 64);
      }
      dsTraceWrite(dsTrace);
    }

    /* Have to process the VLAN delete pending on the dot1q thread. If we
     * wait to process on own thread, dot1q won't be able to tell us which
     * ports were members of the VLAN because the VLAN will be gone already! */
    if (event == VLAN_DELETE_PENDING_NOTIFY)
    {
      if (_dsVlanEnableGet(vlanId))
      {
        /* DHCP snooping is enabled on deleted VLAN */
        if (dot1qVlanEgressPortsGet(vlanId, &portMask) == L7_SUCCESS)
        {
          for (i = 1; i < DS_MAX_INTF_COUNT; i++)
          {
            if (L7_INTF_ISMASKBITSET(portMask, i))
            {
              L7_BOOL intfIsSnooping = dsVlanIntfIsSnooping(vlanId,i) /*dsIntfIsSnooping(i)*/;   /* PTin modified: DHCP snooping */
              if (L7_VLAN_ISMASKBITSET ( dsIntfInfo[i].dsVlans,i))
              {
                dsIntfInfo[i].dsNumVlansEnabled--;
                L7_VLAN_CLRMASKBIT(dsIntfInfo[i].dsVlans, i);
                if (intfIsSnooping && !dsVlanIntfIsSnooping(vlanId,i) /*!dsIntfIsSnooping(i)*/)    /* PTin modified: DHCP snooping */
                {
                  dsIntfDisable(i);
                }
              }
            }
          }
        }
      }
    }
    numVlans++;
  }

  vlanData.numVlans = tempNumVlans;
  if (tempNumVlans == 1)
  {
    /* If only one vlan Id with Snooping enabled is found,
       then move that vlanId to the appropriate place holder
       in the vlanData structure. */
    vlanData.data.vlanId = vlanIdTemp;
  }

  if (event != VLAN_DELETE_PENDING_NOTIFY && vlanData.numVlans > 0)
  {
    /* process event on our thread */
    memset( (void *)&msg, 0, sizeof(dsEventMsg_t) );
    msg.msgType = DS_VLAN_EVENT;
    memcpy(&msg.dsMsgData.vlanEvent.vlanData, &vlanData, sizeof(dot1qNotifyData_t));
    msg.dsMsgData.vlanEvent.intIfNum = intIfNum;
    msg.dsMsgData.vlanEvent.vlanEventType = event;

    rc = osapiMessageSend(Ds_Event_Queue, &msg, sizeof(dsEventMsg_t),
                          L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
    if (rc == L7_SUCCESS)
    {
      osapiSemaGive(dsMsgQSema);
    }
    else
    {
      static L7_uint32 lastMsg = 0;
      dsInfo->debugStats.eventMsgTxError++;
      if (osapiUpTimeRaw() > lastMsg)
      {
        lastMsg = osapiUpTimeRaw();
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
                "Failed to queue VLAN event to DHCP snooping thread.");
      }
    }
  }
  osapiReadLockGive(dsCfgRWLock);
  return rc;
}

/*********************************************************************
* @purpose  Process VLAN event caught in dsVlanChangeCallback().
*
* @param    vlanId     @b{(input)}   VLAN ID
* @param    intIfnum   @b{(input)}   internal interface whose state has changed
* @param    event      @b{(input)}   VLAN event (see vlanNotifyEvent_t for list)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Called on DHCP snooping thread.
*
* @end
*********************************************************************/
L7_RC_t dsVlanEventProcess(dot1qNotifyData_t *pVlanData, L7_uint32 intIfNum, L7_uint32 event)
{
  L7_uint32 i, vlanId = 0, numVlans = 0;
  /* Remember port state before VLAN event */
  L7_BOOL intfIsSnooping /*= dsIntfIsSnooping(intIfNum)*/;    /* PTin removed: DHCP snooping */


  for (i = 1; i<=L7_VLAN_MAX_MASK_BIT; i++)
  {
    if (pVlanData->numVlans == 1)
    {
      vlanId = pVlanData->data.vlanId;
      /* For any continue, we will break out */
      i = L7_VLAN_MAX_MASK_BIT + 1;
    }
    else
    {
      if (L7_VLAN_ISMASKBITSET(pVlanData->data.vlanMask,i))
      {
        vlanId = i;
      }
      else
      {
        if (numVlans == pVlanData->numVlans)
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

    intfIsSnooping = dsVlanIntfIsSnooping(vlanId,intIfNum);   /* PTin added: DHCP snooping */

    switch (event)
    {
      case VLAN_ADD_PORT_NOTIFY:
        if (_dsVlanEnableGet(vlanId))
        {
          if (! L7_VLAN_ISMASKBITSET(dsIntfInfo[intIfNum].dsVlans,vlanId))
          {
            /* DHCP snooping enabled on this VLAN. Increment VLAN count on port. */
            dsIntfInfo[intIfNum].dsNumVlansEnabled++;
            if ((dsCfgData->dsGlobalAdminMode == L7_ENABLE) &&
               !intfIsSnooping && dsVlanIntfIsSnooping(vlanId,intIfNum) /*dsIntfIsSnooping(intIfNum)*/)     /* PTin modified: DHCP snooping */
            {
              /* Adding port to the VLAN enabled DHCP snooping on the port. */
              dsIntfEnable(intIfNum);
            }
            L7_VLAN_SETMASKBIT (dsIntfInfo[intIfNum].dsVlans, vlanId);
          }
        }
        break;

      case VLAN_DELETE_PORT_NOTIFY:
        if (_dsVlanEnableGet(vlanId))
        {
          if (L7_VLAN_ISMASKBITSET(dsIntfInfo[intIfNum].dsVlans, vlanId))
          {
            if (dsIntfInfo[intIfNum].dsNumVlansEnabled != 0)
            {
              dsIntfInfo[intIfNum].dsNumVlansEnabled--;
              if (intfIsSnooping && !dsVlanIntfIsSnooping(vlanId,intIfNum) /*!dsIntfIsSnooping(intIfNum)*/)  /* PTin modified: DHCP snooping */
              {
               /* Removing port from VLAN disabled snooping on VLAN. */
               dsIntfDisable(intIfNum);
              }
            }
            else
            {
              L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
              nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
              L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
                    "DHCP snooping VLAN count on interface %s is bad.", ifName);
            }
            L7_VLAN_CLRMASKBIT (dsIntfInfo[intIfNum].dsVlans,vlanId);
          }
        }
        break;

      case VLAN_DELETE_PENDING_NOTIFY:
        /* handled in callback */
        break;

      default:
        /* do nothing */
        break;
    }
    numVlans++;
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Take a peek at incoming IP packets. If DHCP, snoop them.
*
* @param    hookId        The hook location
* @param    bufHandle     Handle to the frame to be processed
* @param    *pduInfo      Pointer to info about this frame
* @param    continueFunc  Optional pointer to a continue function
*
* @returns  SYSNET_PDU_RC_CONSUMED  if frame has been consumed; stop processing it
* @returns  SYSNET_PDU_RC_IGNORED   if frame has been ignored; continue processing it
*
* @notes    don't barf on IP packets with options
*
* @end
*********************************************************************/
SYSNET_PDU_RC_t dsPacketIntercept(L7_uint32 hookId,
                                  L7_netBufHandle bufHandle,
                                  sysnet_pdu_info_t *pduInfo,
                                  L7_FUNCPTR_t continueFunc)
{
  L7_ipHeader_t *ipHeader;
  L7_udp_header_t *udpHeader;
  L7_dhcp_packet_t *dhcpPacket;
  L7_uint32 rc;
  L7_uint32 len, ethHeaderSize;
  L7_uchar8 *data, sysMacAddr[L7_MAC_ADDR_LEN];
  L7_uint32 vlanId, innerVlanId = 0;
#if defined(L7_ROUTING_PACKAGE) || defined(L7_DHCPS_PACKAGE)
  L7_uint32 mode;
#endif
  L7_uint client_idx = (L7_uint)-1;   /* PTin added: DHCP snooping */

  if (ptin_debug_dhcp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_DHCP,"Packet intercepted: intIfNum=%u, vlanId=%u, innerVlanId=%u",
              pduInfo->intIfNum, pduInfo->vlanId, pduInfo->innerVlanId);

  if (ptin_debug_dhcp_snooping || (dsCfgData->dsTraceFlags & DS_TRACE_FRAME_RX))
  {
    L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
    osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                  "(%s)Packet rx'ed at DHCP intercept, intIfNum=%u, vlanId = %d, inner vlanId = %d",
                  __FUNCTION__, pduInfo->intIfNum, pduInfo->vlanId, pduInfo->innerVlanId);
    dsTraceWrite(traceMsg);
    LOG_TRACE(LOG_CTX_PTIN_DHCP,"Packet rx'ed at DHCP intercept, intIfNum=%u, vlanId = %d, inner vlanId = %d",
              pduInfo->intIfNum, pduInfo->vlanId, pduInfo->innerVlanId);
  }

  if (! dsInfo->l2HwUpdateComplete)
  {
    /* DHCP packet received before control plane is ready. Drop such packet */
    dsInfo->debugStats.msgsDroppedControlPlaneNotReady++;
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"DHCP packet received before control plane is ready. Dropping this packet.");
    return SYSNET_PDU_RC_DISCARD;
  }

  /* Shouldn't get packets if DHCP snooping disabled, but if we do, ignore them. */
  if (dsCfgData->dsGlobalAdminMode != L7_ENABLE )
  {
    #ifdef L7_DHCP_L2_RELAY_PACKAGE
    if (dsCfgData->dsL2RelayAdminMode != L7_ENABLE)
    {
      if (ptin_debug_dhcp_snooping || (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_EXTERNAL_CALLS))
      {
        L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
        osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                    "(%s)Packet rx'ed is ignored as neither GBL Snooping nor L2 Relay is enabled at DHCP intercept.",
                      __FUNCTION__);
        dsTraceWrite(traceMsg);
        LOG_TRACE(LOG_CTX_PTIN_DHCP,"Packet rx'ed is ignored as neither GBL Snooping nor L2 Relay is enabled at DHCP intercept.");
      }
      return SYSNET_PDU_RC_IGNORED;
    }
    #else
    if (ptin_debug_dhcp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_DHCP,"Packet ignored");
    return SYSNET_PDU_RC_IGNORED;
    #endif
  }

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, len);
  ethHeaderSize = sysNetDataOffsetGet(data);
  ipHeader = (L7_ipHeader_t*)(data + ethHeaderSize);

  if (((ipHeader->iph_versLen & 0xF0) == (L7_IP_VERSION << 4)) &&
      (ipHeader->iph_prot == IP_PROT_UDP))
  {
    /* If either DHCP snooping or the L2 Relay is not enabled on
       rx interface, ignore packet. */
    if (dsVlanIntfIsSnooping(pduInfo->vlanId,pduInfo->intIfNum) /*dsIntfIsSnooping(pduInfo->intIfNum)*/ == L7_FALSE )   /* PTin modified: DHCP snooping */
    {
      #ifdef L7_DHCP_L2_RELAY_PACKAGE
      if ( _dsVlanIntfL2RelayGet(pduInfo->vlanId,pduInfo->intIfNum) /*_dsIntfL2RelayGet(pduInfo->intIfNum)*/ == L7_FALSE) /* PTin modified: DHCP snooping */
      {
        if (ptin_debug_dhcp_snooping || (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_EXTERNAL_CALLS))
        {
          L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
          osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                      "(%s)Packet rx'ed is ignored as neither INTF Snooping nor L2 Relay is enabled at DHCP intercept.",
                        __FUNCTION__);
          dsTraceWrite(traceMsg);
          LOG_TRACE(LOG_CTX_PTIN_DHCP,"Packet rx'ed is ignored as neither INTF Snooping nor L2 Relay is enabled at DHCP intercept.");
        }
        return SYSNET_PDU_RC_IGNORED;
      }
      #else
      if (ptin_debug_dhcp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_DHCP,"Packet ignored");
      return SYSNET_PDU_RC_IGNORED;
      #endif
    }

    if (((osapiNtohl(ipHeader->iph_src) & L7_CLASS_D_ADDR_NETWORK) == L7_CLASS_D_ADDR_NETWORK) ||
        ((osapiNtohl(ipHeader->iph_src) & L7_CLASS_E_ADDR_NETWORK) == L7_CLASS_E_ADDR_NETWORK))
    {
      /* Illegal src IP in DHCP packet. Ignore it. */
      dsInfo->debugStats.badSrcAddr++;
      ptin_dhcp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, DHCP_STAT_FIELD_RX_INTERCEPTED);
      ptin_dhcp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, DHCP_STAT_FIELD_RX_FILTERED);

      if (ptin_debug_dhcp_snooping || (dsCfgData->dsTraceFlags & DS_TRACE_FRAME_RX))
      {
        L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
        osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                      "(%s) Packet ignored, because of ip source (0x%08x)",
                      __FUNCTION__, osapiNtohl(ipHeader->iph_src));
        dsTraceWrite(traceMsg);
        LOG_TRACE(LOG_CTX_PTIN_DHCP,"Packet ignored, because of ip source (0x%08x)", osapiNtohl(ipHeader->iph_src));
      }

      return SYSNET_PDU_RC_IGNORED;
    }

    udpHeader = (L7_udp_header_t *)((L7_char8 *)ipHeader + dsIpHdrLen(ipHeader));
    if ((osapiNtohs(udpHeader->destPort) == UDP_PORT_DHCP_SERV) ||
        (osapiNtohs(udpHeader->destPort) == UDP_PORT_DHCP_CLNT))
    {
      /* This is used only when the packet comes double tagged.*/
      vlanId = pduInfo->vlanId;
      innerVlanId = pduInfo->innerVlanId;
      if (ptin_debug_dhcp_snooping || (dsCfgData->dsTraceFlags & DS_TRACE_FRAME_RX))
      {
        L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
        osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                      "(%s)Packet rx'ed Dot1q VLAN Id (%d) length(%d)",
                      __FUNCTION__, vlanId, len);
        dsTraceWrite(traceMsg);
        LOG_TRACE(LOG_CTX_PTIN_DHCP,"Packet rx'ed Dot1q VLAN Id (%d) length(%d)", vlanId, len);
      }
      if ( dsRateLimitFilter(pduInfo))
      {
        ptin_dhcp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, DHCP_STAT_FIELD_RX_INTERCEPTED);
        ptin_dhcp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, DHCP_STAT_FIELD_RX_FILTERED);
        SYSAPI_NET_MBUF_FREE(bufHandle);
        if (ptin_debug_dhcp_snooping || (dsCfgData->dsTraceFlags & DS_TRACE_FRAME_RX))
        {
          L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
          osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                        "(%s) Packet rate limited",
                        __FUNCTION__);
          dsTraceWrite(traceMsg);
          LOG_TRACE(LOG_CTX_PTIN_DHCP,"Packet rate limited");
        }
        return SYSNET_PDU_RC_CONSUMED;
      }

      /* PTin added: DHCP snooping */
      /* Only search and validate client for non CXP360G and untrusted interfaces */
      #if ( !PTIN_BOARD_IS_MATRIX )

      ptin_client_id_t client;

      if (!_dsVlanIsIntfRoot(pduInfo->vlanId,pduInfo->intIfNum))
      {
        #if 0
        /* Validate inner vlan */
        if (innerVlanId==0 || innerVlanId>=4095)
        {
          if (ptin_debug_dhcp_snooping)
            LOG_ERR(LOG_CTX_PTIN_DHCP,"Client not referenced! (intIfNum=%u, innerVlanId=%u, intVlanId=%u)",
                  pduInfo->intIfNum, innerVlanId, vlanId);
          ptin_dhcp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, DHCP_STAT_FIELD_RX_INTERCEPTED);
          ptin_dhcp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, DHCP_STAT_FIELD_RX_FILTERED);
          return SYSNET_PDU_RC_IGNORED;
        }
        #endif

        /* Client information */
        memset(&client, 0x00, sizeof(client));
        client.ptin_intf.intf_type = client.ptin_intf.intf_id = 0;
        client.outerVlan = vlanId;
        client.innerVlan = (innerVlanId > 0 && innerVlanId < 4096) ? innerVlanId : 0;
        client.mask  = PTIN_CLIENT_MASK_FIELD_INTF | PTIN_CLIENT_MASK_FIELD_OUTERVLAN;
        client.mask |= (innerVlanId > 0 && innerVlanId < 4096) ? PTIN_CLIENT_MASK_FIELD_INNERVLAN : 0;

        /* Only search for a client, if inner vlan is valid */
        /* Otherwise, use dynamic DHCP */
        #if (PTIN_BOARD_IS_GPON)
        if (innerVlanId > 0 && innerVlanId < 4096)
        #else
        if (1)
        #endif
        {
          /* Find client index, and validate it */
          if (ptin_dhcp_clientIndex_get(pduInfo->intIfNum, vlanId, &client, &client_idx)!=L7_SUCCESS ||
              client_idx>=PTIN_SYSTEM_MAXCLIENTS_PER_DHCP_INSTANCE)
          {
            if (ptin_debug_dhcp_snooping)
              LOG_ERR(LOG_CTX_PTIN_DHCP,"Client not found! (intIfNum=%u, ptin_intf=%u/%u, innerVlanId=%u, intVlanId=%u extOVlan=%u extIVlan=%u)",
                      pduInfo->intIfNum, client.ptin_intf.intf_type, client.ptin_intf.intf_id, client.innerVlan, vlanId, client.outerVlan, client.innerVlan);
            ptin_dhcp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, DHCP_STAT_FIELD_RX_INTERCEPTED);
            ptin_dhcp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, DHCP_STAT_FIELD_RX_FILTERED);
            if (ptin_debug_dhcp_snooping)
              LOG_TRACE(LOG_CTX_PTIN_DHCP,"Packet ignored");
            return SYSNET_PDU_RC_IGNORED;
            //client_idx = (L7_uint)-1;
          }
        }
        else
        {
          client_idx = (L7_uint) -1;
        }
      }
      #endif

      if (ptin_debug_dhcp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_DHCP,"Packet intercepted at intIfNum=%u, oVlan=%u, iVlan=%u",
                  pduInfo->intIfNum, pduInfo->vlanId, pduInfo->innerVlanId);

      rc = dsPacketQueue(data, len, vlanId, pduInfo->intIfNum, innerVlanId, &client_idx);    /* PTin modified: DHCP snooping */

      /* Packet intercepted */
      ptin_dhcp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, client_idx, DHCP_STAT_FIELD_RX_INTERCEPTED);

      if (rc == L7_REQUEST_DENIED) /* DHCP Message got filtered, So Ignore
                                      For further processing */
      {
        dsInfo->debugStats.msgsIntercepted++;
        dsInfo->debugStats.msgsInterceptedIntf[pduInfo->intIfNum]++;
        if (ptin_debug_dhcp_snooping)
          LOG_TRACE(LOG_CTX_PTIN_DHCP,"Incremented DHCP_STAT_FIELD_RX_FILTERED");
        ptin_dhcp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, client_idx, DHCP_STAT_FIELD_RX_FILTERED);
        SYSAPI_NET_MBUF_FREE(bufHandle);
        if (ptin_debug_dhcp_snooping || (dsCfgData->dsTraceFlags & DS_TRACE_FRAME_RX))
        {
          L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
          osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                        "(%s) Packet denied to be stored in packet queue",
                        __FUNCTION__);
          dsTraceWrite(traceMsg);
          LOG_TRACE(LOG_CTX_PTIN_DHCP,"Packet denied to be stored in packet queue");
        }
        return SYSNET_PDU_RC_CONSUMED;
      }
      if (rc == L7_SUCCESS)
      {
        dsInfo->debugStats.msgsIntercepted++;
        dsInfo->debugStats.msgsInterceptedIntf[pduInfo->intIfNum]++;

        /* If the DHCP reply message is destined for the DHCP CLIENT running on the
           switch itself, then ignore the packet.*/
        dhcpPacket = (L7_dhcp_packet_t*)((L7_char8 *)udpHeader + sizeof(L7_udp_header_t));

        simGetSystemIPBurnedInMac(sysMacAddr);
        if (memcmp(dhcpPacket->chaddr, sysMacAddr, L7_MAC_ADDR_LEN) == L7_NULL)
        {
          if (ptin_debug_dhcp_snooping)
            LOG_TRACE(LOG_CTX_PTIN_DHCP,"Packet ignored");
          return SYSNET_PDU_RC_IGNORED;
        }

#ifdef L7_ROUTING_PACKAGE
        if ((ihAdminModeGet(&mode) == L7_SUCCESS) && (mode == L7_ENABLE))
        {
          if (ptin_debug_dhcp_snooping)
            LOG_TRACE(LOG_CTX_PTIN_DHCP,"Packet ignored");
          return SYSNET_PDU_RC_IGNORED;
        }
#endif /* if Relay agent is there */
#ifdef L7_DHCPS_PACKAGE
        if ((usmDbDhcpsAdminModeGet(0, &mode) == L7_SUCCESS) &&
            (mode == L7_ENABLE))
        {
          if (ptin_debug_dhcp_snooping)
            LOG_TRACE(LOG_CTX_PTIN_DHCP,"Packet ignored");
          return SYSNET_PDU_RC_IGNORED;
        }
#endif /* If DHCP server is there */
#if 0 /* PTIN Daniel - Impedia funcionamento se intf de clinte fosse trusted. Nem sequer sei qual era o propósito disto incialmente... o que me deixa algo preocupado */
        if (_dsVlanIntfTrustGet(pduInfo->vlanId,pduInfo->intIfNum) /*_dsIntfTrustGet(pduInfo->intIfNum)*/) /* Trusted port */   /* PTin modified: DHCP snooping */
        {
          if (ptin_debug_dhcp_snooping)
            LOG_TRACE(LOG_CTX_PTIN_DHCP,"Packet ignored");
          return SYSNET_PDU_RC_IGNORED;
        }
#endif
        SYSAPI_NET_MBUF_FREE(bufHandle);
        if (ptin_debug_dhcp_snooping)
          LOG_TRACE(LOG_CTX_PTIN_DHCP,"Packet consumed");
        return SYSNET_PDU_RC_CONSUMED;
      }
    }
    else
    {
      if (ptin_debug_dhcp_snooping || (dsCfgData->dsTraceFlags & DS_TRACE_FRAME_RX))
      {
        L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
        osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                      "(%s) Packet is neither server nor client (%u)",
                      __FUNCTION__, osapiNtohs(udpHeader->destPort));
        dsTraceWrite(traceMsg);
        LOG_TRACE(LOG_CTX_PTIN_DHCP,"Packet is neither server nor client (%u)", osapiNtohs(udpHeader->destPort));
      }
    }
  }
  else
  {
    if (ptin_debug_dhcp_snooping || (dsCfgData->dsTraceFlags & DS_TRACE_FRAME_RX))
    {
      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                    "(%s) Invalid protocol received (%u), or invalid versLen (0x%02x)",
                    __FUNCTION__, ipHeader->iph_prot, ipHeader->iph_versLen);
      dsTraceWrite(traceMsg);
      LOG_TRACE(LOG_CTX_PTIN_DHCP,"Invalid protocol received (%u), or invalid versLen (0x%02x)",ipHeader->iph_prot, ipHeader->iph_versLen);
    }
  }

  if (ptin_debug_dhcp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_DHCP,"Packet ignored");

  return SYSNET_PDU_RC_IGNORED;
}

/*********************************************************************
* @purpose  Take a peek at incoming IPv6 packets. If DHCPv6, snoop them.
*
* @param    hookId        The hook location
* @param    bufHandle     Handle to the frame to be processed
* @param    *pduInfo      Pointer to info about this frame
* @param    continueFunc  Optional pointer to a continue function
*
* @returns  SYSNET_PDU_RC_CONSUMED  if frame has been consumed; stop processing it
* @returns  SYSNET_PDU_RC_IGNORED   if frame has been ignored; continue processing it
*
* @notes    don't barf on IP packets with options
*
* @end
*********************************************************************/
SYSNET_PDU_RC_t dsv6PacketIntercept(L7_uint32 hookId,
                                    L7_netBufHandle bufHandle,
                                    sysnet_pdu_info_t *pduInfo,
                                    L7_FUNCPTR_t continueFunc)
{
  L7_ip6Header_t *ipv6Header;
  L7_udp_header_t *udpHeader;
//  L7_dhcp6_packet_t *dhcpPacket;
  L7_uint32 rc;
  L7_uint32 len, ethHeaderSize;
  L7_uchar8 *data;
  L7_uint32 vlanId, innerVlanId = 0;
#if defined(L7_ROUTING_PACKAGE) || defined(L7_DHCPS_PACKAGE)
  L7_uint32 mode;
#endif
  L7_uint client_idx = (L7_uint)-1;   /* PTin added: DHCP snooping */

  if (ptin_debug_dhcp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_DHCP,"Packet intercepted: intIfNum=%u, vlanId=%u, innerVlanId=%u",
              pduInfo->intIfNum, pduInfo->vlanId, pduInfo->innerVlanId);

  if (ptin_debug_dhcp_snooping || (dsCfgData->dsTraceFlags & DS_TRACE_FRAME_RX))
  {
    L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
    osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                  "(%s)Packet rx'ed at DHCP intercept, intIfNum=%u, vlanId = %d, inner vlanId = %d",
                  __FUNCTION__, pduInfo->intIfNum, pduInfo->vlanId, pduInfo->innerVlanId);
    dsTraceWrite(traceMsg);
    LOG_TRACE(LOG_CTX_PTIN_DHCP,"Packet rx'ed at DHCP intercept, intIfNum=%u, vlanId = %d, inner vlanId = %d",
              pduInfo->intIfNum, pduInfo->vlanId, pduInfo->innerVlanId);
  }

  if (! dsInfo->l2HwUpdateComplete)
  {
    /* DHCP packet received before control plane is ready. Drop such packet */
    dsInfo->debugStats.msgsDroppedControlPlaneNotReady++;
    return SYSNET_PDU_RC_DISCARD;
  }

  /* Shouldn't get packets if DHCP snooping disabled, but if we do, ignore them. */
  if (dsCfgData->dsGlobalAdminMode != L7_ENABLE )
  {
    #ifdef L7_DHCP_L2_RELAY_PACKAGE
    if (dsCfgData->dsL2RelayAdminMode != L7_ENABLE)
    {
      if (ptin_debug_dhcp_snooping || (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_EXTERNAL_CALLS))
      {
        L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
        osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                    "(%s)Packet rx'ed is ignored as neither GBL Snooping nor L2 Relay is enabled at DHCP intercept.",
                      __FUNCTION__);
        dsTraceWrite(traceMsg);
        LOG_TRACE(LOG_CTX_PTIN_DHCP,"Packet rx'ed is ignored as neither GBL Snooping nor L2 Relay is enabled at DHCP intercept.");
      }
      return SYSNET_PDU_RC_IGNORED;
    }
    #else
    return SYSNET_PDU_RC_IGNORED;
    #endif
  }

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, len);
  ethHeaderSize = sysNetDataOffsetGet(data);
  ipv6Header = (L7_ip6Header_t*)(data + ethHeaderSize);

  if (((osapiNtohl(ipv6Header->ver_class_flow) & 0xF0000000) == (L7_IP6_VERSION << 28)) &&
      (ipv6Header->next == IP_PROT_UDP))
  {
    /* If either DHCP snooping or the L2 Relay is not enabled on
       rx interface, ignore packet. */
    if (dsVlanIntfIsSnooping(pduInfo->vlanId,pduInfo->intIfNum) /*dsIntfIsSnooping(pduInfo->intIfNum)*/ == L7_FALSE )   /* PTin modified: DHCP snooping */
    {
      #ifdef L7_DHCP_L2_RELAY_PACKAGE
      if ( _dsVlanIntfL2RelayGet(pduInfo->vlanId,pduInfo->intIfNum) /*_dsIntfL2RelayGet(pduInfo->intIfNum)*/ == L7_FALSE) /* PTin modified: DHCP snooping */
      {
        if (ptin_debug_dhcp_snooping || (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_EXTERNAL_CALLS))
        {
          L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
          osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                      "(%s)Packet rx'ed is ignored as neither INTF Snooping nor L2 Relay is enabled at DHCP intercept.",
                        __FUNCTION__);
          dsTraceWrite(traceMsg);
          LOG_TRACE(LOG_CTX_PTIN_DHCP,"Packet rx'ed is ignored as neither INTF Snooping nor L2 Relay is enabled at DHCP intercept.");
        }
        return SYSNET_PDU_RC_IGNORED;
      }
      #else
      return SYSNET_PDU_RC_IGNORED;
      #endif
    }

    udpHeader = (L7_udp_header_t *)((L7_char8 *)ipv6Header + L7_IP6_HEADER_LEN);
    if ((osapiNtohs(udpHeader->destPort) == UDP_PORT_DHCP6_SERV) ||
        (osapiNtohs(udpHeader->destPort) == UDP_PORT_DHCP6_CLNT))
    {
      /* This is used only when the packet comes double tagged.*/
      vlanId = pduInfo->vlanId;
      innerVlanId = pduInfo->innerVlanId;
      if (ptin_debug_dhcp_snooping || (dsCfgData->dsTraceFlags & DS_TRACE_FRAME_RX))
      {
        L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
        osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                      "(%s)Packet rx'ed Dot1q VLAN Id (%d) length(%d)",
                      __FUNCTION__, vlanId, len);
        dsTraceWrite(traceMsg);
        LOG_TRACE(LOG_CTX_PTIN_DHCP,"Packet rx'ed Dot1q VLAN Id (%d) length(%d)", vlanId, len);
      }
      if ( dsRateLimitFilter(pduInfo))
      {
        ptin_dhcp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, DHCP_STAT_FIELD_RX_INTERCEPTED);
        ptin_dhcp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, DHCP_STAT_FIELD_RX_FILTERED);
        SYSAPI_NET_MBUF_FREE(bufHandle);
        return SYSNET_PDU_RC_CONSUMED;
      }

      /* PTin added: DHCP snooping */
      /* Only search and validate client for non CXP360G and untrusted interfaces */
      #if ( !PTIN_BOARD_IS_MATRIX )

      ptin_client_id_t client;

      if (!_dsVlanIsIntfRoot(pduInfo->vlanId,pduInfo->intIfNum))
      {
        #if 0
        /* Validate inner vlan */
        if (innerVlanId==0 || innerVlanId>=4095)
        {
          if (ptin_debug_dhcp_snooping)
            LOG_ERR(LOG_CTX_PTIN_DHCP,"Client not referenced! (intIfNum=%u, innerVlanId=%u, intVlanId=%u)",
                  pduInfo->intIfNum, innerVlanId, vlanId);
          ptin_dhcp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, DHCP_STAT_FIELD_RX_INTERCEPTED);
          ptin_dhcp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, DHCP_STAT_FIELD_RX_FILTERED);
          return SYSNET_PDU_RC_IGNORED;
        }
        #endif

        /* Client information */
        client.ptin_intf.intf_type = client.ptin_intf.intf_id = 0;
        client.outerVlan = vlanId;
        client.innerVlan = (innerVlanId > 0 && innerVlanId < 4096) ? innerVlanId : 0;
        client.mask  = PTIN_CLIENT_MASK_FIELD_INTF | PTIN_CLIENT_MASK_FIELD_OUTERVLAN;
        client.mask |= (innerVlanId > 0 && innerVlanId < 4096) ? PTIN_CLIENT_MASK_FIELD_INNERVLAN : 0;

        /* Only search for a client, if inner vlan is valid */
        /* Otherwise, use dynamic DHCP */
        #if (PTIN_BOARD_IS_GPON)
        if (innerVlanId > 0 && innerVlanId < 4096)
        #else
        if (1)
        #endif
        {
          /* Find client index, and validate it */
          if (ptin_dhcp_clientIndex_get(pduInfo->intIfNum, vlanId, &client, &client_idx)!=L7_SUCCESS ||
              client_idx>=PTIN_SYSTEM_MAXCLIENTS_PER_DHCP_INSTANCE)
          {
            if (ptin_debug_dhcp_snooping)
              LOG_ERR(LOG_CTX_PTIN_DHCP,"Client not found! (intIfNum=%u, ptin_intf=%u/%u, innerVlanId=%u, intVlanId=%u)",
                      pduInfo->intIfNum, client.ptin_intf.intf_type,client.ptin_intf.intf_id, client.innerVlan, vlanId);
            ptin_dhcp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, DHCP_STAT_FIELD_RX_INTERCEPTED);
            ptin_dhcp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, DHCP_STAT_FIELD_RX_FILTERED);
            if (ptin_debug_dhcp_snooping)
              LOG_ERR(LOG_CTX_PTIN_DHCP,"Packet ignored.");
            return SYSNET_PDU_RC_IGNORED;
            //client_idx = (L7_uint)-1;
          }
        }
        else
        {
          client_idx = (L7_uint) -1;
        }
      }
      #endif

      if (ptin_debug_dhcp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_DHCP,"Packet intercepted at intIfNum=%u, oVlan=%u, iVlan=%u",
                  pduInfo->intIfNum, pduInfo->vlanId, pduInfo->innerVlanId);

      rc = dsPacketQueue(data, len, vlanId, pduInfo->intIfNum, innerVlanId, &client_idx);    /* PTin modified: DHCP snooping */

      /* Packet intercepted */
      ptin_dhcp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, client_idx, DHCP_STAT_FIELD_RX_INTERCEPTED);

      if (rc == L7_REQUEST_DENIED) /* DHCP Message got filtered, So Ignore
                                      For further processing */
      {
        dsInfo->debugStats.msgsIntercepted++;
        dsInfo->debugStats.msgsInterceptedIntf[pduInfo->intIfNum]++;
        if (ptin_debug_dhcp_snooping)
          LOG_TRACE(LOG_CTX_PTIN_DHCP,"Incremented DHCP_STAT_FIELD_RX_FILTERED");
        ptin_dhcp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, client_idx, DHCP_STAT_FIELD_RX_FILTERED);
        SYSAPI_NET_MBUF_FREE(bufHandle);
        return SYSNET_PDU_RC_CONSUMED;
      }
      if (rc == L7_SUCCESS)
      {
        dsInfo->debugStats.msgsIntercepted++;
        dsInfo->debugStats.msgsInterceptedIntf[pduInfo->intIfNum]++;

#if 0 /* PTin removed - DHCPv6 */
        /* If the DHCP reply message is destined for the DHCP CLIENT running on the
           switch itself, then ignore the packet.*/
        dhcpPacket = (L7_dhcp6_packet_t*)((L7_char8 *)udpHeader + sizeof(L7_udp_header_t));

        simGetSystemIPBurnedInMac(sysMacAddr);
        if (memcmp(dhcpPacket->chaddr, sysMacAddr, L7_MAC_ADDR_LEN) == L7_NULL)
        {
          return SYSNET_PDU_RC_IGNORED;
        }
#endif

#ifdef L7_ROUTING_PACKAGE
        if ((ihAdminModeGet(&mode) == L7_SUCCESS) && (mode == L7_ENABLE))
        {
          return SYSNET_PDU_RC_IGNORED;
        }
#endif /* if Relay agent is there */
#ifdef L7_DHCPS_PACKAGE
        if ((usmDbDhcpsAdminModeGet(0, &mode) == L7_SUCCESS) &&
            (mode == L7_ENABLE))
        {
          return SYSNET_PDU_RC_IGNORED;
        }
#endif /* If DHCP server is there */
        if (_dsVlanIsIntfRoot(pduInfo->vlanId,pduInfo->intIfNum) /*_dsIntfTrustGet(pduInfo->intIfNum)*/) /* Trusted port */   /* PTin modified: DHCP snooping */
        {
          return SYSNET_PDU_RC_IGNORED;
        }
        SYSAPI_NET_MBUF_FREE(bufHandle);
        return SYSNET_PDU_RC_CONSUMED;
      }
    }
    else
    {
      if (ptin_debug_dhcp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_DHCP,"DHCP Packet is not server nor client");
    }
  }

  if (ptin_debug_dhcp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_DHCP,"Packet is not DHCP");

  return SYSNET_PDU_RC_IGNORED;
}

/*********************************************************************
* @purpose  Queue a DHCP packet for processing on our own thread
*
*
* @param    ethHeader    @b{(input)} ethernet frame
* @param    dataLen      @b{(input)} length of ethernet frame
* @param    vlanId       @b{(input)} Vlan ID
* @param    intIfNum     @b{(input)} port where packet arrived
* @param    innerVlanId  @b{(input)} port where packet arrived
*
* @returns  L7_SUCCESS
*
* @notes    vlanId represents the existing VLANID if single tagged,
*                  represents the outer tag if double tagged.
*           innerVlanId represents the inner tag if double tagged.
*
* @end
*********************************************************************/
L7_RC_t dsPacketQueue(L7_uchar8 *ethHeader, L7_uint32 dataLen,
                      L7_ushort16 vlanId, L7_uint32 intIfNum,
                      L7_ushort16 innerVlanId, L7_uint *client_idx)    /* PTin modified: DHCP snooping && DHCPv6 */
{
    dsFrameMsg_t dsFrameMsg;

    L7_udp_header_t *udp_header;
    L7_dhcp_packet_t *dhcpPacket;
    L7_ushort16 ipPktLen;
    L7_ushort16 dhcpPktLen;
    L7_ushort16 ethHdrLen;
    L7_ipHeader_t *ipHeader;
    L7_ushort16 ipHdrLen;
    L7_uchar8 ipVersion;

    ethHdrLen = sysNetDataOffsetGet(ethHeader);
    ipVersion = (*(L7_uchar8*)(ethHeader + ethHdrLen) & 0xF0) >> 4;

   if (ipVersion != L7_IP6_VERSION)
   {
      ipHeader = (L7_ipHeader_t*) (ethHeader + ethHdrLen);
      ipHdrLen = dsIpHdrLen(ipHeader);

      /* Filter the DHCP messages based on rules */

      udp_header = (L7_udp_header_t *) ((L7_char8 *) ipHeader + ipHdrLen);
      dhcpPacket = (L7_dhcp_packet_t*) ((L7_char8 *) udp_header + sizeof(L7_udp_header_t));
      ipPktLen = osapiNtohs(ipHeader->iph_len);
      dhcpPktLen = ipPktLen - ipHdrLen - sizeof(L7_udp_header_t);

      if (ptin_debug_dhcp_snooping)
        LOG_DEBUG(LOG_CTX_PTIN_DHCP, "Packet %s received at intIfNum=%u, oVlan=%u, iVlan=%u, MAC=%02x:%02x:%02x:%02x:%02x:%02x",
              dhcpMsgTypeNames[dsPacketType(dhcpPacket, dhcpPktLen)], intIfNum, vlanId, innerVlanId, dhcpPacket->chaddr[0], dhcpPacket->chaddr[1], dhcpPacket->chaddr[2], dhcpPacket->chaddr[3], dhcpPacket->chaddr[4], dhcpPacket->chaddr[5]);

      if (dsCfgData->dsTraceFlags & DS_TRACE_FRAME_RX)
      {
         L7_uchar8 dsTrace[DS_MAX_TRACE_LEN];
         L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
         L7_dhcp_pkt_type_t dhcpPktType = dsPacketType(dhcpPacket, dhcpPktLen);
         
         memset(ifName, 0, sizeof(ifName));
         nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
         osapiSnprintf(dsTrace, DS_MAX_TRACE_LEN, "(%s)DHCP snooping received %s on interface %s in VLAN %u.", __FUNCTION__, dhcpMsgTypeNames[dhcpPktType], ifName, vlanId);
         dsTraceWrite(dsTrace);
      }
      if (dsCfgData->dsTraceFlags & DS_TRACE_FRAME_RX_DETAIL)
      {
         dsLogEthernetHeader((L7_enetHeader_t*) ethHeader, DS_TRACE_CONSOLE);
         dsLogIpHeader(ipHeader, DS_TRACE_CONSOLE);
         dsLogDhcpPacket(dhcpPacket, DS_TRACE_CONSOLE);
      }

      /* Filter DHCP packet based on security rules */
      if (dsFrameFilter(intIfNum, vlanId, ethHeader, ipHeader, innerVlanId, client_idx)) /* PTin modified: DHCP snooping */
      {
         dsInfo->debugStats.msgsFiltered++;

         if (ptin_debug_dhcp_snooping)
            LOG_TRACE(LOG_CTX_PTIN_DHCP, "Incremented DHCP_STAT_FIELD_RX_FILTERED");
         //ptin_dhcp_stat_increment_field(intIfNum, vlanId, *client_idx, DHCP_STAT_FIELD_RX_FILTERED);
         return L7_REQUEST_DENIED;
      }
   }
   dsInfo->debugStats.msgsReceived++;
   ptin_dhcp_stat_increment_field(intIfNum, vlanId, *client_idx, DHCP_STAT_FIELD_RX);

   if (dsCfgData->dsTraceFlags & DS_TRACE_FRAME_RX)
   {
     L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
     osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                   "(%s)Packet frameLen = %d ",__FUNCTION__, dataLen );
     dsTraceWrite(traceMsg);
   }

   memcpy(&dsFrameMsg.frameBuf, ethHeader, dataLen);
   dsFrameMsg.rxIntf = intIfNum;
   dsFrameMsg.vlanId = vlanId;
   dsFrameMsg.frameLen = dataLen;
   /* Useful only when processing DHCP replies in Metro networks. */
   dsFrameMsg.innerVlanId = innerVlanId;
   /* PTin added: DHCP snooping */
   dsFrameMsg.client_idx  = *client_idx;

   if (osapiMessageSend(Ds_Packet_Queue, &dsFrameMsg, sizeof(dsFrameMsg_t), L7_NO_WAIT,
                         L7_MSG_PRIORITY_NORM) == L7_SUCCESS)
   {
    osapiSemaGive(dsMsgQSema);
   }
   else
   {
     /* This may be fairly normal, so don't log. DHCP should recover. */
     dsInfo->debugStats.frameMsgTxError++;
   }
   return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process a DHCP packet on DHCP snooping thread
*
* @param    intIfNum @b{(input)} receive interface
* @param    vlanId   @b{(input)} VLAN ID
* @param    frame    @b{(input)} ethernet frame
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsFrameProcess(L7_uint32 intIfNum, L7_ushort16 vlanId,
                       L7_uchar8 *frame, L7_uint32 frameLen,
                       L7_ushort16 innerVlanId, L7_uint client_idx)
{
  L7_ushort16 ethHdrLen;
  L7_uchar8 ipVersion;
  L7_RC_t ret = L7_SUCCESS;

  ethHdrLen = sysNetDataOffsetGet(frame);
  ipVersion = (0xF0 & *(L7_uchar8*)(frame + ethHdrLen)) >> 4 ;

  if(L7_IP_VERSION == ipVersion)
  {
     ret = dsDHCPv4FrameProcess(intIfNum, vlanId, frame, frameLen, innerVlanId, client_idx);
  }
  else if(L7_IP6_VERSION == ipVersion)
  {
     ret = dsDHCPv6FrameProcess(intIfNum, vlanId, frame, frameLen, innerVlanId, client_idx);
  }

  return ret;
}

/*********************************************************************
* @purpose  Process a DHCP packet on DHCP snooping thread
*
* @param    intIfNum @b{(input)} receive interface
* @param    vlanId   @b{(input)} VLAN ID
* @param    frame    @b{(input)} ethernet frame
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsDHCPv4FrameProcess(L7_uint32 intIfNum, L7_ushort16 vlanId,
                       L7_uchar8 *frame, L7_uint32 frameLen,
                       L7_ushort16 innerVlanId, L7_uint client_idx)     /* PTin modified: DHCP snooping */
{
  L7_udp_header_t    *udp_header;
  L7_dhcp_packet_t   *dhcpPacket;
  L7_ushort16         ipPktLen;
  L7_ushort16         dhcpPktLen;
  L7_ushort16         ethHdrLen;
  L7_ipHeader_t      *ipHeader;
  L7_ushort16         ipHdrLen;
  L7_RC_t             rc = L7_FAILURE;
#ifdef L7_DHCP_L2_RELAY_PACKAGE
  dsRelayAgentInfo_t  relayAgentInfo;
  L7_ushort16         vlanIdFwd = 0;
#endif
  dhcpSnoopBinding_t  dhcp_binding;
  L7_enetHeader_t    *mac_header = 0;
  L7_uint32           relayOptIntIfNum = 0;
  L7_uchar8           broadcast_flag;  
  
  memset(&dhcp_binding, 0x00, sizeof(dhcp_binding));   

  ethHdrLen  = sysNetDataOffsetGet(frame);
  ipHeader   = (L7_ipHeader_t*)(frame + ethHdrLen);
  ipHdrLen   = dsIpHdrLen(ipHeader);
  udp_header = (L7_udp_header_t *)((L7_char8 *)ipHeader + ipHdrLen);
  dhcpPacket = (L7_dhcp_packet_t*)((L7_char8 *)udp_header + sizeof(L7_udp_header_t));
  ipPktLen   = osapiNtohs(ipHeader->iph_len);
  dhcpPktLen = ipPktLen - ipHdrLen - sizeof(L7_udp_header_t);
  if (ptin_debug_dhcp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_DHCP, "Packet frameLen = %d, initial UDP length = %d ", frameLen, osapiNtohs(udp_header->length) );

  /* Update Binding database only when DHCP Snooping is enabled */
  if ((dsCfgData->dsGlobalAdminMode == L7_ENABLE) && (dsVlanIntfIsSnooping(vlanId,intIfNum) == L7_TRUE))
  {
    if (ptin_debug_dhcp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_DHCP, "intIfNum %u, vlanId=%u valid", intIfNum, vlanId);

    if ((dhcpPacket->op == L7_DHCP_BOOTP_REPLY) || (dhcpPacket->op == L7_DHCP_BOOTP_REQUEST)) 
    {
      /* Search for this client before the binding is extracted because the entry in this table will be removed if a NACK/DECLINE is received */
      if(dhcpPacket->op == L7_DHCP_BOOTP_REPLY)
      {
        if (ptin_debug_dhcp_snooping)
          LOG_TRACE(LOG_CTX_PTIN_DHCP, "L7_DHCP_BOOTP_REPLY");

        memset(&dhcp_binding, 0, sizeof(dhcpSnoopBinding_t));
        mac_header = (L7_enetHeader_t*) frame;
        memcpy(&dhcp_binding.key.macAddr, dhcpPacket->chaddr, L7_ENET_MAC_ADDR_LEN);
        dhcp_binding.key.ipType = L7_AF_INET;
        if (L7_SUCCESS != dsBindingFind(&dhcp_binding, L7_MATCH_EXACT))
        {
          if (ptin_debug_dhcp_snooping)
            LOG_TRACE(LOG_CTX_PTIN_DHCP, "DHCP Relay-Agent: Received server reply for an unknown client");
          return L7_SUCCESS;
        }
      }

      /* Update bindings database. If this is a client message, yiaddr will be 0.
       * But we want to enter a temporary binding so we can learn the port where
       * the client resides. Then when the server responds, we'll add yiaddr to
       * the binding and be able to forward the reply to the client. */
      rc = dsBindingExtract(intIfNum, vlanId, innerVlanId, dhcpPacket, dhcpPktLen);    
      if ((dhcpPacket->op == L7_DHCP_BOOTP_REQUEST) && (rc == L7_REQUEST_DENIED))
      {
        if (ptin_debug_dhcp_snooping)
          LOG_TRACE(LOG_CTX_PTIN_DHCP, "L7_DHCP_BOOTP_REQUEST: Request denied");

        /* Static binding exists for client on untrusted interface. Consider
         * this a configuration error. Drop client messages. Logic does let
         * through server messages to client on untrusted interface, but if
         * we drop the client messages in this case, we should never see
         * server messages. Allow server messages in this case so that
         * a client on a trusted port can get a DHCP address, even if a static
         * binding is configured for that client. */
        dsInfo->debugStats.msgDroppedBindingExists++;
        return rc;
      }
    }
  }

#ifdef  L7_DHCP_L2_RELAY_PACKAGE
  /* Do Option-82 processing if L2 Relay is enabled*/
  if ((dsCfgData->dsL2RelayAdminMode == L7_ENABLE) && (_dsVlanIntfL2RelayGet(vlanId,intIfNum) == L7_TRUE))    
  {
    /* all filterations for server replies are done even before the
       frame is posted to DHCP task. So the server frame here is
       expected to be valid. If the packet has come with Option-82
       it needs to removed before relaying back to the client.*/
    if (dhcpPacket->op == L7_DHCP_BOOTP_REPLY)
    {
      if (ptin_debug_dhcp_snooping)
      {
        LOG_TRACE(LOG_CTX_PTIN_DHCP, "L7_DHCP_BOOTP_REPLY - Packet frameLen = %d after Option-82 Removal from DHCP Reply", frameLen);
      }

      /* Get this client's information (if we don't already have it) */
      if(dhcp_binding.ipFamily == 0)
      {
         memset(&dhcp_binding, 0, sizeof(dhcpSnoopBinding_t));
         mac_header = (L7_enetHeader_t*) frame;
         memcpy(&dhcp_binding.key.macAddr, dhcpPacket->chaddr, L7_ENET_MAC_ADDR_LEN);
         dhcp_binding.key.ipType = L7_AF_INET;
         if (L7_SUCCESS != dsBindingFind(&dhcp_binding, L7_MATCH_EXACT))
         {
           if (ptin_debug_dhcp_snooping)
             LOG_TRACE(LOG_CTX_PTIN_DHCP, "DHCP Relay-Agent: Received server reply for an unknown client");
           return L7_SUCCESS;
         }
      }
      relayOptIntIfNum = dhcp_binding.intIfNum;

      /* Remove relay agent info if we added it during the client request */
      memset(&relayAgentInfo, 0, sizeof(dsRelayAgentInfo_t));
      if(dhcp_binding.flags & DHCP_FLAGS_BIT_CLIENTREQUEST_RELAYOP_ADDED)
      {
         if (dsRelayAgentInfoRemoveOrGet(frame, &frameLen, dhcpPacket, dhcpPktLen, L7_TRUE, &relayAgentInfo) != L7_SUCCESS)
         {
            if (ptin_debug_dhcp_snooping)
               LOG_ERR(LOG_CTX_PTIN_DHCP, "DHCP Relay-Agent: Error while removing relay agent info");
            return L7_FAILURE;
         }
      }
      else
      {
         if (dsRelayAgentInfoRemoveOrGet(frame, &frameLen, dhcpPacket, dhcpPktLen, L7_FALSE, &relayAgentInfo) != L7_SUCCESS)
         {
            if (ptin_debug_dhcp_snooping)
               LOG_ERR(LOG_CTX_PTIN_DHCP, "DHCP Relay-Agent: Error while getting relay agent info");
            return L7_FAILURE;
         }
      }
    }
    /* all filterations for client requests are done even before the
       frame is posted to DHCP task. So the client frame here may or may not
       have Option-82. Add Option-82 to the frame, if it does not have, before
       forwarding to the server.*/
    else if (dhcpPacket->op == L7_DHCP_BOOTP_REQUEST)
    {
      L7_BOOL isActiveOp82;

      if (ptin_debug_dhcp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_DHCP, "L7_DHCP_BOOTP_REQUEST");

      if (dsL2RelayServiceIsEnabled(intIfNum, vlanId, innerVlanId) == L7_FALSE)
      {
         if (ptin_debug_dhcp_snooping)
            LOG_ERR(LOG_CTX_PTIN_DHCP, "DHCP L2 Relay is not enabled [intIfNum:%u vlanId:%u innerVlanId:%u].", intIfNum, vlanId, innerVlanId);
         return L7_FAILURE;
      }

      /* Client requests received with giaddr different from 0 should be forwarded witout modifications */
      if(dhcpPacket->giaddr == L7_NULL)
      {
         if (ptin_dhcp_client_options_get(intIfNum, vlanId, innerVlanId, &isActiveOp82, L7_NULLPTR, L7_NULLPTR) != L7_SUCCESS)
         {
           if (ptin_debug_dhcp_snooping)
              LOG_ERR(LOG_CTX_PTIN_DHCP, "Unable to get DHCP client options [intIfNum:%u vlanId:%u innerVlanId:%u]", intIfNum, vlanId, innerVlanId);
           return L7_FAILURE;
         }

         if (isActiveOp82)
         {
           L7_RC_t rcRelayAgentInfoAdd;

           if (ptin_debug_dhcp_snooping)
             LOG_TRACE(LOG_CTX_PTIN_DHCP, "Is Active option 82");

           /* This function adds Option-82 only if it does not already exists.*/
           if ((rcRelayAgentInfoAdd = dsRelayAgentInfoAdd(intIfNum, vlanId, innerVlanId, frame, &frameLen)) == L7_FAILURE)
           {
             if (ptin_debug_dhcp_snooping)
                LOG_ERR(LOG_CTX_PTIN_DHCP, "Failed to add DHCP Option-82 for Client request on SVLAN %d", vlanIdFwd);
             return L7_FAILURE;
           }
           ptin_dhcp_stat_increment_field(intIfNum, vlanId, client_idx, DHCP_STAT_FIELD_RX_SERVER_REPLIES_WITH_OPTION82);

           /* Set the relayop_added bit so we know, when processing the server response, that it was us who added the relay option */
           LOG_TRACE(LOG_CTX_PTIN_DHCP, "dsRelayAgentInfoAdd returned %u", rcRelayAgentInfoAdd);
           if(rcRelayAgentInfoAdd == L7_SUCCESS)
           {
              dsBindingTreeKey_t key;

              memset(&key, 0x00, sizeof(key));
              memcpy(&key.macAddr.addr, &dhcpPacket->chaddr, L7_ENET_MAC_ADDR_LEN);
              key.ipType = L7_AF_INET;
              dhcp_binding.flags |= DHCP_FLAGS_BIT_CLIENTREQUEST_RELAYOP_ADDED;
              dsBindingFlagsUpdate(&key, dhcp_binding.flags);
           }

           if (ptin_debug_dhcp_snooping)
             LOG_TRACE(LOG_CTX_PTIN_DHCP, "Packet frameLen = %d after Option-82 addition", frameLen);
         }
      }

      /* Check if broadcast flag should be modified */
      if ( (ptin_dhcp_flags_get(vlanId, &broadcast_flag) == L7_SUCCESS) && (broadcast_flag != DHCP_BOOTP_FLAG_NONE) )
      {
        if (ptin_debug_dhcp_snooping)
          LOG_TRACE(LOG_CTX_PTIN_DHCP, "Broadcast_flag = 0x%x", broadcast_flag);

        if (broadcast_flag == DHCP_BOOTP_FLAG_BROADCAST)
        {
          dhcpPacket->flags |= osapiHtons(0x8000); /* Force broadcast flag */
        }
        else
        {
          dhcpPacket->flags &= osapiHtons((L7_uint16) ~0x8000); /* Clear broadcast flag */
        }

        /* Need to recalculate UDP checksum. */
        dsUdpCheckSumCalculate(frame, &frameLen, L7_TRUE, 0);

        if (ptin_debug_dhcp_snooping)
          LOG_TRACE(LOG_CTX_PTIN_DHCP,"Broadcast flag changed (%u)",broadcast_flag);
      }
    }
  }
#endif //L7_DHCP_L2_RELAY_PACKAGE

  LOG_TRACE(LOG_CTX_PTIN_DHCP, "Forwarding packet from intIfNum %u to %u (vlanId=%u, innerVlanId=%u)", intIfNum, relayOptIntIfNum, vlanId, innerVlanId);
  if (dsFrameForward(intIfNum, vlanId, frame, frameLen, innerVlanId, client_idx, relayOptIntIfNum) == L7_SUCCESS)
  {
    dsInfo->debugStats.msgsForwarded++;
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP, "Unable to forward DHCP packet [intIfNum:%u vlanId:%u frame:%p frameLen:%u innerVlanId:%u client_idx:%u relayOptIntIfNum:%u]",
            intIfNum, vlanId, frame, frameLen, innerVlanId, client_idx, relayOptIntIfNum);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process a DHCP packet on DHCP snooping thread
*
* @param    intIfNum @b{(input)} receive interface
* @param    vlanId   @b{(input)} VLAN ID
* @param    frame    @b{(input)} ethernet frame
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsDHCPv6FrameProcess(L7_uint32 intIfNum, L7_ushort16 vlanId,
                             L7_uchar8 *frame, L7_uint32 frameLen,
                             L7_ushort16 innerVlanId, L7_uint client_idx)     /* PTin modified: DHCP snooping */
{
   L7_uchar8 *dhcp_header_ptr;

   dhcp_header_ptr = frame + sysNetDataOffsetGet(frame) + L7_IP6_HEADER_LEN + sizeof(L7_udp_header_t);

   LOG_DEBUG(LOG_CTX_PTIN_DHCP, "Received DHCPv6 message type[%u] [intIfNum:%u clientIdx:%u vlanId:%u innerVlanId:%u]", *dhcp_header_ptr, intIfNum, client_idx, vlanId, innerVlanId);

   switch(*dhcp_header_ptr)
   {
      case L7_DHCP6_ADVERTISE:
      case L7_DHCP6_REPLY:
      {
         if (ptin_debug_dhcp_snooping)
           LOG_WARNING(LOG_CTX_PTIN_DHCP, "Silently ignoring invalid message received: msg-type (%d)", *dhcp_header_ptr);
         return L7_SUCCESS;
      }
      case L7_DHCP6_RELAY_REPL:
      {
         dsDHCPv6ServerFrameProcess(intIfNum, vlanId, frame, innerVlanId, client_idx);

         break;
      }
      case L7_DHCP6_SOLICIT:
      case L7_DHCP6_REQUEST:
      case L7_DHCP6_CONFIRM:
      case L7_DHCP6_RENEW:
      case L7_DHCP6_REBIND:
      case L7_DHCP6_RELEASE:
      case L7_DHCP6_DECLINE:
      case L7_DHCP6_RECONFIGURE:
      case L7_DHCP6_INFORMATION_REQUEST:
      case L7_DHCP6_RELAY_FORW:
      {
         dsDHCPv6ClientFrameProcess(intIfNum, vlanId, frame, innerVlanId, client_idx);

         break;
      }
      default:
        if (ptin_debug_dhcp_snooping)
          LOG_WARNING(LOG_CTX_PTIN_DHCP, "Invalid DHCPv6 Message received: unknown msg-type %u", *dhcp_header_ptr);
   }

   return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process a DHCP packet on DHCP snooping thread
*
* @param    intIfNum @b{(input)} receive interface
* @param    vlanId   @b{(input)} VLAN ID
* @param    frame    @b{(input)} ethernet frame
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsDHCPv6ClientFrameProcess(L7_uint32 intIfNum, L7_ushort16 vlanId, L7_uchar8 *frame, L7_ushort16 innerVlanId, L7_uint client_idx)
{
   L7_uchar8 frame_copy[DS_DHCP_PACKET_SIZE_MAX] = { 0 }, *ipv6_copy_header_ptr, *udp_copy_header_ptr, *dhcp_copy_header_ptr;
   L7_uchar8 *eth_header_ptr, *ipv6_header_ptr, *udp_header_ptr, *dhcp_header_ptr;
   L7_dhcp6_relay_agent_packet_t relay_agent_header = { 0 };
   L7_uint32 frame_copy_len;
   L7_BOOL isActiveOp37, isActiveOp18;
   L7_ip6Header_t *ipv6_header, *ipv6_copy_header;
   L7_udp_header_t *udp_header, *udp_copy_header;
   L7_inet_addr_t client_ip_addr = { 0 };
   L7_enetMacAddr_t client_mac_addr;
   L7_enetHeader_t *mac_header = 0;
   L7_ushort16 ethHdrLen;
   L7_uint8 dhcp_msg_type;

   LOG_DEBUG(LOG_CTX_PTIN_DHCP, "DHCP Relay-Agent: Processing client request");

   //Check if the port through which the message was received is valid
   if (_dsVlanIsIntfRoot(vlanId,intIfNum))
   {
      if (ptin_debug_dhcp_snooping)
        LOG_WARNING(LOG_CTX_PTIN_DHCP, "DHCPv6 Relay-Agent: Discarded client request received on root port");
      ptin_dhcp_stat_increment_field(intIfNum, vlanId, client_idx, DHCP_STAT_FIELD_RX_CLIENT_PKTS_ON_TRUSTED_INTF);
      return L7_FAILURE;
   }

   //Parse the received frame
   eth_header_ptr    = frame;
   mac_header        = (L7_enetHeader_t*) frame;
   ethHdrLen         = sysNetDataOffsetGet(frame);
   ipv6_header_ptr   = eth_header_ptr + ethHdrLen;
   ipv6_header       = (L7_ip6Header_t*) ipv6_header_ptr;
   udp_header_ptr    = ipv6_header_ptr + L7_IP6_HEADER_LEN;
   udp_header        = (L7_udp_header_t *) udp_header_ptr;
   dhcp_header_ptr   = udp_header_ptr + sizeof(L7_udp_header_t);
   dhcp_msg_type     = *((L7_uint8*)dhcp_header_ptr); //@note(DFF): Do NOT convert this field to HOST! It is a single byte.

   //Copy received frame up to the end of the UDP header
   memcpy(frame_copy, frame, ethHdrLen + L7_IP6_HEADER_LEN + sizeof(L7_udp_header_t));
   ipv6_copy_header_ptr = frame_copy + ethHdrLen;
   ipv6_copy_header     = (L7_ip6Header_t*) ipv6_copy_header_ptr;
   udp_copy_header_ptr  = ipv6_copy_header_ptr + L7_IP6_HEADER_LEN;
   udp_copy_header      = (L7_udp_header_t*) udp_copy_header_ptr;
   dhcp_copy_header_ptr = udp_copy_header_ptr + sizeof(L7_udp_header_t);
   frame_copy_len       = ethHdrLen + L7_IP6_HEADER_LEN + sizeof(L7_udp_header_t);

   //Make sure that the reported UDP.length is at least the minimum size possible
   if(osapiNtohs(udp_header->length) < (sizeof(L7_udp_header_t) + sizeof(L7_dhcp6_packet_t)))
   {
      if (ptin_debug_dhcp_snooping)
        LOG_ERR(LOG_CTX_PTIN_DHCP, "DHCPv6 Relay-Agent: Broken frame received, ignoring (invalid UDP.length).");
      return L7_SUCCESS;
   }

   //If the port through which the message was received is configured as untrusted and the packet is a RELAY-FORW, drop it
   if((dhcp_msg_type==L7_DHCP6_RELAY_FORW) && (!_dsVlanIntfTrustGet(vlanId,intIfNum)))
   {
      if (ptin_debug_dhcp_snooping)
        LOG_ERR(LOG_CTX_PTIN_DHCP, "DHCPv6 Relay-Agent: Discarded RELAY-FORW message received on untrusted port");
      return L7_SUCCESS;
   }

   //Get DHCP Options for this client
   inetAddressZeroSet(L7_AF_INET6, &client_ip_addr);
   memcpy(&client_mac_addr, mac_header->src.addr, L7_ENET_MAC_ADDR_LEN);
   if (ptin_dhcp_client_options_get(intIfNum, vlanId, innerVlanId, L7_NULLPTR, &isActiveOp37, &isActiveOp18) != L7_SUCCESS)
   {
      LOG_WARNING(LOG_CTX_PTIN_DHCP, "DHCPv6 Relay-Agent: Unknown client [intIfNum:%u vlanId:%u innerVlanId:%u]", intIfNum, vlanId, innerVlanId);
      return L7_FAILURE;
   }
   LOG_TRACE(LOG_CTX_PTIN_DHCP, "DHCPv6 Relay-Agent: Determined client options [op18:%u op37:%u]", isActiveOp18, isActiveOp37);
   

   //@note (Daniel): Currently, statistics for DHCP are broken. They are to be rewritten in v4.0.
   ptin_dhcp_stat_increment_field(intIfNum, vlanId, client_idx, DHCP_STAT_FIELD_RX_CLIENT_REQUESTS_WITHOUT_OPTIONS);

   //Create a new Relay-Agent message. If the received msg is a 'L7_DHCP6_RELAY_FORW', increase hop_count
   relay_agent_header.msg_type  = L7_DHCP6_RELAY_FORW;
   relay_agent_header.hop_count = (dhcp_msg_type==L7_DHCP6_RELAY_FORW) ? (*(dhcp_header_ptr+1)+1) : (0);
   LOG_TRACE(LOG_CTX_PTIN_DHCP, "DHCPv6 Relay-Agent: Setting hopCount to %u", relay_agent_header.hop_count);
   memcpy(relay_agent_header.peer_address, ipv6_header->src, IPV6_ADDRESS_LEN);
   memcpy(dhcp_copy_header_ptr, &relay_agent_header, sizeof(L7_dhcp6_relay_agent_packet_t));
   frame_copy_len += sizeof(L7_dhcp6_relay_agent_packet_t);

   //Add DHCP-Relay option (option 9)
   LOG_TRACE(LOG_CTX_PTIN_DHCP, "DHCPv6 Relay-Agent: Adding option 9");
   if (L7_SUCCESS != dsv6AddOption9(frame_copy, &frame_copy_len, dhcp_header_ptr, osapiNtohs(udp_header->length) - sizeof(L7_dhcp6_packet_t) - L7_FCS_LEN))
   {
      LOG_WARNING(LOG_CTX_PTIN_DHCP, "DHCP Relay-Agent: Unable to add op9 to DHCP frame");
      return L7_FAILURE;
   }

   //Add Interface-id option (option 18)
   if (isActiveOp18)
   {
      if (L7_SUCCESS != dsv6AddOption18or37(intIfNum, frame_copy, &frame_copy_len, vlanId, innerVlanId, client_mac_addr.addr, L7_DHCP6_OPT_INTERFACE_ID))
      {
         if (ptin_debug_dhcp_snooping)
           LOG_ERR(LOG_CTX_PTIN_DHCP, "DHCP Relay-Agent: Error adding op18 to DHCP frame");
         return L7_FAILURE;
      }
      LOG_TRACE(LOG_CTX_PTIN_DHCP, "DHCPv6 Relay-Agent: Added op18 to DHCP frame");
   }

   //Add Interface-id option (option 37)
   if (isActiveOp37)
   {
      if (L7_SUCCESS != dsv6AddOption18or37(intIfNum, frame_copy, &frame_copy_len, vlanId, innerVlanId, client_mac_addr.addr, L7_DHCP6_OPT_REMOTE_ID))
      {
         if (ptin_debug_dhcp_snooping)
           LOG_ERR(LOG_CTX_PTIN_DHCP, "DHCP Relay-Agent: Error adding op37 to DHCP frame");
         return L7_FAILURE;
      }
      LOG_TRACE(LOG_CTX_PTIN_DHCP, "DHCPv6 Relay-Agent: Added op37 to DHCP frame");
   }

   //Update the UDP and IPv6 headers
   udp_copy_header->sourcePort = osapiHtons(547);
   udp_copy_header->length     = ipv6_copy_header->paylen = osapiHtons(frame_copy_len - ethHdrLen - L7_IP6_HEADER_LEN);
   dsUdpCheckSumCalculate(frame_copy, &frame_copy_len, L7_TRUE, 0);

   //Send the new DHCP message to the server
   LOG_TRACE(LOG_CTX_PTIN_DHCP, "DHCPv6 Relay-Agent: Sending packet [intfNum:%u vlanId:%u innerVlan:%u client:%u]", intIfNum, vlanId, innerVlanId, client_idx);
   if (L7_SUCCESS != dsFrameFlood(intIfNum, vlanId, frame_copy, frame_copy_len, L7_TRUE, innerVlanId, client_idx))
   {
      if (ptin_debug_dhcp_snooping)
        LOG_ERR(LOG_CTX_PTIN_DHCP, "DHCP Relay-Agent: Error sending DHCPv6 message");
      return L7_FAILURE;
   }

   //Add or update an existing entry in the binding table
   dsv6BindingAdd(DS_BINDING_TENTATIVE, &client_mac_addr, client_ip_addr, vlanId, innerVlanId, intIfNum);
   dsv6LeaseStatusUpdate(&client_mac_addr, *(L7_uint8*)dhcp_header_ptr);

   return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process a DHCP packet on DHCP snooping thread
*
* @param    intIfNum @b{(input)} receive interface
* @param    vlanId   @b{(input)} VLAN ID
* @param    frame    @b{(input)} ethernet frame
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsDHCPv6ServerFrameProcess(L7_uint32 intIfNum, L7_ushort16 vlanId, L7_uchar8 *frame, L7_ushort16 innerVlanId, L7_uint client_idx)
{
   L7_uchar8 frame_copy[DS_DHCP_PACKET_SIZE_MAX] = { 0 }, *ipv6_copy_header_ptr, *udp_copy_header_ptr, *dhcp_copy_header_ptr;
   L7_uchar8 *eth_header_ptr, *ipv6_header_ptr, *udp_header_ptr, *dhcp_header_ptr, *relay_op_header_ptr;
   L7_uchar8 *op_interfaceid_ptr = 0, *op_relaymsg_ptr = 0, *op_remoteid_ptr = 0;
   L7_BOOL isActiveOp37, isActiveOp18;
   L7_inet_addr_t link_addr = {0}, client_ip_addr = {0};
   L7_uint32 frame_len, frame_copy_len, lease_time = 0;
   L7_dhcp6_relay_agent_packet_t *relay_agent_header;
   L7_ip6Header_t *ipv6_header, *ipv6_copy_header;
   L7_udp_header_t *udp_header, *udp_copy_header;
   L7_dhcp6_option_packet_t *dhcp_op_header = 0;
   L7_enetMacAddr_t client_mac_addr;
   L7_enetHeader_t *mac_header = 0;
   dhcpSnoopBinding_t dhcp_binding;
   L7_ushort16 ethHdrLen;
   L7_uint8  ethPrty, *frameEthPrty;

   LOG_DEBUG(LOG_CTX_PTIN_DHCP, "DHCP Relay-Agent: Received server reply");

   //Check if the port through which the message was received is valid
   if (_dsVlanIsIntfRoot(vlanId,intIfNum) == L7_FALSE)
   {
      if (ptin_debug_dhcp_snooping)
        LOG_WARNING(LOG_CTX_PTIN_DHCP, "DHCP Relay-Agent: Received server reply on untrusted port");
      ptin_dhcp_stat_increment_field(intIfNum, vlanId, client_idx, DHCP_STAT_FIELD_RX_SERVER_PKTS_ON_UNTRUSTED_INTF);
      return L7_FAILURE;
   }

   //Parse the received frame
   eth_header_ptr     = frame;
   mac_header         = (L7_enetHeader_t*) frame;
   ethHdrLen          = sysNetDataOffsetGet(frame);
   ipv6_header_ptr    = eth_header_ptr + ethHdrLen;
   ipv6_header        = (L7_ip6Header_t*) ipv6_header_ptr;
   udp_header_ptr     = ipv6_header_ptr + L7_IP6_HEADER_LEN;
   udp_header         = (L7_udp_header_t *) udp_header_ptr;
   dhcp_header_ptr    = udp_header_ptr + sizeof(L7_udp_header_t);
   relay_agent_header = (L7_dhcp6_relay_agent_packet_t*) dhcp_header_ptr;

   //Copy received frame up to the end of the UDP header
   memcpy(frame_copy, frame, ethHdrLen + L7_IP6_HEADER_LEN + sizeof(L7_udp_header_t));
   ipv6_copy_header_ptr = frame_copy + ethHdrLen;
   ipv6_copy_header     = (L7_ip6Header_t*) ipv6_copy_header_ptr;
   udp_copy_header_ptr  = ipv6_copy_header_ptr + L7_IP6_HEADER_LEN;
   udp_copy_header      = (L7_udp_header_t*) udp_copy_header_ptr;
   dhcp_copy_header_ptr = udp_copy_header_ptr + sizeof(L7_udp_header_t);
   frame_copy_len       = ethHdrLen + L7_IP6_HEADER_LEN + sizeof(L7_udp_header_t);

   //Get client interface from DHCP binding table
   memset(&dhcp_binding,         0,                     sizeof(dhcpSnoopBinding_t));
   memcpy(&client_mac_addr,      mac_header->dest.addr, L7_ENET_MAC_ADDR_LEN);
   memcpy(&dhcp_binding.key.macAddr, mac_header->dest.addr, L7_ENET_MAC_ADDR_LEN);
   dhcp_binding.key.ipType = L7_AF_INET6;
   if (L7_SUCCESS == dsBindingFind(&dhcp_binding, L7_MATCH_EXACT))
   {
      intIfNum = dhcp_binding.intIfNum;
   }
   //Get DHCP Options for this client
   if (ptin_dhcp_client_options_get(intIfNum, dhcp_binding.vlanId, dhcp_binding.innerVlanId, L7_NULLPTR, &isActiveOp37, &isActiveOp18) != L7_SUCCESS)
   {
      if (ptin_debug_dhcp_snooping)
        LOG_ERR(LOG_CTX_PTIN_DHCP, "DHCP Relay-Agent: Unable to get client[intfNum:%u oVlan:%u iVlan:%u] options", intIfNum, dhcp_binding.vlanId, dhcp_binding.innerVlanId);
      return L7_FAILURE;
   }
   //If the service is unstacked (client_idx==-1) then we have to determine the client_idx through the inner_vlan in the Binding Table
   if(client_idx == -1)
   {
     ptin_client_id_t client;

     client.ptin_intf.intf_type = client.ptin_intf.intf_id = 0;
     client.outerVlan = dhcp_binding.vlanId;
     client.innerVlan = dhcp_binding.innerVlanId;
     client.mask  = PTIN_CLIENT_MASK_FIELD_INTF | PTIN_CLIENT_MASK_FIELD_OUTERVLAN;
     client.mask |= (dhcp_binding.innerVlanId !=0 ) ? PTIN_CLIENT_MASK_FIELD_INNERVLAN : 0;

     if (ptin_dhcp_clientIndex_get(intIfNum, vlanId, &client, &client_idx)!=L7_SUCCESS)
     {
       if (ptin_debug_dhcp_snooping)
         LOG_ERR(LOG_CTX_PTIN_DHCP,"Client not found! (intIfNum=%u, innerVlanId=%u, intVlanId=%u)",
                 intIfNum, dhcp_binding.innerVlanId, vlanId);
       ptin_dhcp_stat_increment_field(intIfNum, vlanId, (L7_uint32)-1, DHCP_STAT_FIELD_RX_INTERCEPTED);
       ptin_dhcp_stat_increment_field(intIfNum, vlanId, (L7_uint32)-1, DHCP_STAT_FIELD_RX_FILTERED);
       return L7_SUCCESS;
     }
   }

   //Make sure that the reported UDP.length is at least the minimum size possible
   if(osapiNtohs(udp_header->length) < (sizeof(L7_udp_header_t) + sizeof(L7_dhcp6_packet_t)))
   {
      if (ptin_debug_dhcp_snooping)
        LOG_ERR(LOG_CTX_PTIN_DHCP, "DHCPv6 Relay-Agent: Broken frame received, ignoring (invalid UDP.length).");
      return L7_SUCCESS;
   }

   //Parse options in which we are interested. The remaining are ignored
   relay_op_header_ptr = dhcp_header_ptr + sizeof(L7_dhcp6_relay_agent_packet_t);
   frame_len           = osapiNtohs(udp_header->length) - sizeof(L7_udp_header_t) - sizeof(L7_dhcp6_relay_agent_packet_t);
   while (frame_len > 0)
   {
     dhcp_op_header = (L7_dhcp6_option_packet_t*) relay_op_header_ptr;

     //Check for an invalid length
     if ((osapiNtohs(dhcp_op_header->option_len) == 0) || 
         (frame_len < (sizeof(L7_dhcp6_option_packet_t) + osapiNtohs(dhcp_op_header->option_len))))
     {
        LOG_ERR(LOG_CTX_PTIN_DHCP, "DHCP Relay-Agent: Received message with an invalid frame length %d/%d", frame_len, sizeof(L7_dhcp6_option_packet_t) + osapiNtohs(dhcp_op_header->option_len));
        return L7_SUCCESS;
     }

     switch (osapiNtohs(dhcp_op_header->option_code))
     {
        case L7_DHCP6_OPT_INTERFACE_ID:
        {
           op_interfaceid_ptr   = relay_op_header_ptr;
           frame_len           -= sizeof(L7_dhcp6_option_packet_t) + osapiNtohs(dhcp_op_header->option_len);
           relay_op_header_ptr += sizeof(L7_dhcp6_option_packet_t) + osapiNtohs(dhcp_op_header->option_len);
           break;
        }
        case L7_DHCP6_OPT_REMOTE_ID:
        {
           op_remoteid_ptr      = relay_op_header_ptr;
           frame_len           -= sizeof(L7_dhcp6_option_packet_t) + osapiNtohs(dhcp_op_header->option_len);
           relay_op_header_ptr += sizeof(L7_dhcp6_option_packet_t) + osapiNtohs(dhcp_op_header->option_len);
           break;
        }
        case L7_DHCP6_OPT_RELAY_MSG:
        {
           L7_dhcp6_relay_agent_packet_t *relay_message;

           //Find the first relay message option and save a pointer to it
           if(op_relaymsg_ptr == L7_NULLPTR)
           {
              op_relaymsg_ptr   = relay_op_header_ptr; 
           }
           frame_len           -= sizeof(L7_dhcp6_option_packet_t);
           relay_op_header_ptr += sizeof(L7_dhcp6_option_packet_t);
           relay_message        = (L7_dhcp6_relay_agent_packet_t*)relay_op_header_ptr;

           if(relay_message->msg_type == L7_DHCP6_RELAY_REPL) //@note(DFF): Do NOT convert this field to HOST! It is a single byte.
           {
              frame_len           -= sizeof(L7_dhcp6_relay_agent_packet_t);
              relay_op_header_ptr += sizeof(L7_dhcp6_relay_agent_packet_t);
           }
           else
           {
              frame_len           -= sizeof(L7_dhcp6_option_packet_t);
              relay_op_header_ptr += sizeof(L7_dhcp6_option_packet_t);
           }

           break;
        }        
        case L7_DHCP6_OPT_IA_NA:
        {
           frame_len           -= sizeof(L7_dhcp6_option_packet_t) + 3*sizeof(L7_uint32);
           relay_op_header_ptr += sizeof(L7_dhcp6_option_packet_t) + 3*sizeof(L7_uint32);
           break;
        }
        case L7_DHCP6_OPT_IA_TA:
        {
           frame_len           -= sizeof(L7_dhcp6_option_packet_t) + sizeof(L7_uint32);
           relay_op_header_ptr += sizeof(L7_dhcp6_option_packet_t) + sizeof(L7_uint32);
           break;
        }
        case L7_DHCP6_OPT_IAADDR:
        {
           inetAddressSet(L7_AF_INET6, relay_op_header_ptr + sizeof(L7_dhcp6_option_packet_t), &client_ip_addr);
           lease_time           = osapiNtohl(*(L7_uint32*) (relay_op_header_ptr + sizeof(L7_dhcp6_option_packet_t) + IPV6_ADDRESS_LEN + sizeof(L7_int32)));
           frame_len           -= sizeof(L7_dhcp6_option_packet_t) + osapiNtohs(dhcp_op_header->option_len);
           relay_op_header_ptr += sizeof(L7_dhcp6_option_packet_t) + osapiNtohs(dhcp_op_header->option_len);
           break;
        }
        default:
           frame_len           -= sizeof(L7_dhcp6_option_packet_t) + osapiNtohs(dhcp_op_header->option_len);
           relay_op_header_ptr += sizeof(L7_dhcp6_option_packet_t) + osapiNtohs(dhcp_op_header->option_len);
           break; 
     }
   }

   //Check if the server reply did not have any options
   if(!op_interfaceid_ptr && !op_remoteid_ptr)
   {
      ptin_dhcp_stat_increment_field(intIfNum, vlanId, client_idx, DHCP_STAT_FIELD_RX_SERVER_REPLIES_WITHOUT_OPTIONS);
   }

   if(!op_relaymsg_ptr)
   {
      if (ptin_debug_dhcp_snooping)
        LOG_WARNING(LOG_CTX_PTIN_DHCP, "DHCP Relay-Agent: Received DHCPv6 message missing mandatory relay message option");
      return L7_SUCCESS;
   }

   //Validate the DIP and link-addr fields on the received message
   inetAddressSet(L7_AF_INET6, relay_agent_header->link_address, &link_addr);
   if(!inetIsInAddressAny(&link_addr) || 0 != memcmp(ipv6_header->dst, relay_agent_header->peer_address, IPV6_ADDRESS_LEN))
   {
      if (ptin_debug_dhcp_snooping)
        LOG_WARNING(LOG_CTX_PTIN_DHCP, "DHCP Relay-Agent: Received DHCPv6 message with invalid fields");
      return L7_SUCCESS;
   }

   //Change ethernet priority bit
   if (ptin_dhcp_ethPrty_get(vlanId, &ethPrty) != L7_SUCCESS)
   {
     if (ptin_debug_dhcp_snooping)
       LOG_ERR(LOG_CTX_PTIN_DHCP, "Unable to get ethernet priority");
     return L7_FAILURE;
   }
   frameEthPrty  = (L7_uint8*)(frame_copy + 2*sizeof(L7_enetMacAddr_t) + sizeof(L7_ushort16));
   *frameEthPrty &= 0x1F; //Reset p-bit
   *frameEthPrty |= ((0x7 & ethPrty) << 5); //Set p-bit

   //Create a new DHCPv6 message
   memcpy(dhcp_copy_header_ptr, op_relaymsg_ptr + sizeof(L7_dhcp6_option_packet_t), osapiNtohs(*(L7_uint16*)(op_relaymsg_ptr + sizeof(L7_uint16))));
   frame_copy_len += osapiNtohs(*(L7_uint16*)(op_relaymsg_ptr + sizeof(L7_uint16)));

   //Update the UDP and IPv6 headers
   if(relay_agent_header->hop_count==0) //Set UDP destination port based on weather we are directly connected to the client or not
   {
      udp_copy_header->destPort = osapiHtons(546);
   }
   else
   {
      udp_copy_header->destPort = osapiHtons(547);
   }
   udp_copy_header->length = ipv6_copy_header->paylen = osapiHtons(frame_copy_len - ethHdrLen - L7_IP6_HEADER_LEN);
   dsUdpCheckSumCalculate(frame_copy, &frame_copy_len, L7_TRUE, 0);

   //Send the new DHCP message to the client
   LOG_TRACE(LOG_CTX_PTIN_DHCP, "DHCPv6 Relay-Agent: Sending packet [intfNum:%u vlanId:%u innerVlan:%u client:%u]", intIfNum, vlanId, innerVlanId, client_idx);
   if (L7_SUCCESS != dsFrameIntfFilterSend(intIfNum, vlanId, frame_copy, frame_copy_len, L7_TRUE, innerVlanId, client_idx))
   {
      if (ptin_debug_dhcp_snooping)
        LOG_ERR(LOG_CTX_PTIN_DHCP, "DHCP Relay-Agent: Error sending DHCPv6 message");
      return L7_SUCCESS;
   }

   //Increment server tx counters
   ptin_dhcp_stat_increment_field(intIfNum, vlanId, client_idx, DHCP_STAT_FIELD_TX_SERVER_REPLIES_WITHOUT_OPTIONS);
   ptin_dhcp_stat_increment_field(intIfNum, vlanId, client_idx, DHCP_STAT_FIELD_TX_FORWARDED);

   //Remove the entry in the binding table if the client has previously sent a release
   if(dhcp_binding.leaseStatus == DS_LEASESTATUS_V6_RELEASE)
   {
     dsBindingTreeKey_t key;

     memset(&key, 0x00, sizeof(key));
     memcpy(&key.macAddr.addr, &client_mac_addr.addr, L7_ENET_MAC_ADDR_LEN);
     key.ipType = L7_AF_INET6;
     dsBindingRemove(&key);
   }
   else
   {
     dsBindingTreeKey_t key;

     //Add a new dynamic entry in the binding table
     memset(&key, 0x00, sizeof(key));
     memcpy(&key.macAddr.addr, &client_mac_addr.addr, L7_ENET_MAC_ADDR_LEN);
     key.ipType = L7_AF_INET6;
     dsv6BindingIpAddrSet(&client_mac_addr, client_ip_addr);
     dsBindingLeaseSet(&key, lease_time);
     dsv6LeaseStatusUpdate(&client_mac_addr, *(L7_uint8*)(op_relaymsg_ptr + sizeof(L7_dhcp6_option_packet_t)));
   }

   return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Add a new DHCPv6 option (op. 18)
*
* @param    frame             DHCPv6 frame
* @param    frameLen          Pointer to current length of the DHCPv6 frame
* @param    dhcpRelayFrame    Original DHCPv6 header received from the client
* @param    dhcpRelayFrameLen Original DHCPv6 header length
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t dsv6AddOption9(L7_uchar8 *frame, L7_uint32 *frameLen, L7_uchar8 *dhcpRelayFrame, L7_ushort16 dhcpRelayFrameLen)
{
   L7_dhcp6_option_packet_t dhcp_op_dhcp_relay_be = { 0 };

   dhcp_op_dhcp_relay_be.option_code = osapiHtons(L7_DHCP6_OPT_RELAY_MSG);
   dhcp_op_dhcp_relay_be.option_len  = osapiHtons(dhcpRelayFrameLen);

   memcpy(frame + *frameLen, &dhcp_op_dhcp_relay_be, sizeof(L7_dhcp6_option_packet_t)); //Copy Relay-message option header
   *frameLen += sizeof(L7_dhcp6_option_packet_t);
   memcpy(frame + *frameLen, dhcpRelayFrame, dhcpRelayFrameLen); //Copy DHCP-relay-message
   *frameLen += dhcpRelayFrameLen;

   return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Add a new DHCPv6 option (op. 18)
*
* @param    intIfNum    Interface through which the message was received
* @param    frame       DHCPv6 frame
* @param    frameLen    Pointer to current length of the DHCPv6 frame
* @param    dhcpOp      DHCPv6 option to add
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t dsv6AddOption18or37(L7_uint32 intIfNum, L7_uchar8 *frame, L7_uint32 *frameLen, L7_ushort16 vlanId,
                            L7_ushort16 innerVlanId, L7_uchar8 *macAddr, L7_dhcp6_opttype_t dhcpOp)
{
   L7_char8 circuit_id[DS_MAX_REMOTE_ID_STRING], remote_id[DS_MAX_REMOTE_ID_STRING];
   L7_dhcp6_option_packet_t dhcp_op_dhcp_relay_be = { 0 };

   if (ptin_dhcp_stringIds_get(intIfNum, vlanId, innerVlanId , macAddr, circuit_id, remote_id) != L7_SUCCESS)
   {
      return L7_FAILURE;
   }

   dhcp_op_dhcp_relay_be.option_code = osapiHtons(dhcpOp);
   if(L7_DHCP6_OPT_INTERFACE_ID == dhcpOp)
   {
      dhcp_op_dhcp_relay_be.option_len = osapiHtons(strlen(circuit_id));
   }
   else if(L7_DHCP6_OPT_REMOTE_ID == dhcpOp)
   {
      dhcp_op_dhcp_relay_be.option_len = osapiHtons(sizeof(L7_uint32) + strlen(remote_id)); //VendorId + RemoteId
   }
   memcpy(frame + *frameLen, &dhcp_op_dhcp_relay_be, sizeof(L7_dhcp6_option_packet_t)); //Copy Relay-message option header
   *frameLen += sizeof(L7_dhcp6_option_packet_t);

   if(L7_DHCP6_OPT_INTERFACE_ID == dhcpOp)
   {
      if(!strlen(circuit_id))
      {
        if (ptin_debug_dhcp_snooping)
          LOG_ERR(LOG_CTX_PTIN_DHCP, "DHCP Relay-Agent: Invalid circuit-id string (string length is 0)", circuit_id);
        return L7_FAILURE;
      }
      if (ptin_debug_dhcp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_DHCP, "DHCP Relay-Agent: Adding interface-id '%s'", circuit_id);
      memcpy(frame + *frameLen, circuit_id, strlen(circuit_id)); //Copy circuit-id string
      *frameLen += strlen(circuit_id);
   }
   else if(L7_DHCP6_OPT_REMOTE_ID == dhcpOp)
   {
     L7_uint32 enterprise_number;

     //Add PTin enterprise number
     enterprise_number = PTIN_ENTERPRISE_NUMBER;
     memcpy(frame + *frameLen, &enterprise_number, sizeof(L7_uint32));
     *frameLen += sizeof(L7_uint32);

      if(!strlen(remote_id))
      {
        if (ptin_debug_dhcp_snooping)
          LOG_ERR(LOG_CTX_PTIN_DHCP, "DHCP Relay-Agent: Invalid remote-id string (string length is 0)", remote_id);
        return L7_FAILURE;
      }
      if (ptin_debug_dhcp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_DHCP, "DHCP Relay-Agent: Adding remote-id '%s'", remote_id);
      memcpy(frame + *frameLen, remote_id, strlen(remote_id)); //Copy remote-id string
      *frameLen += strlen(remote_id);
   }

   return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Apply security filtering rules to received DHCP packet.
*
* @param    intIfNum @b{(input)} receive interface
* @param    vlanId   @b{(input)} VLAN ID
* @param    frame    @b{(input)} ethernet frame
* @param    ipHeader @b{(input)} IP header inside frame
*
* @returns  L7_TRUE if frame is filtered
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL dsFrameFilter(L7_uint32 intIfNum, L7_ushort16 vlanId,
                      L7_uchar8 *frame, L7_ipHeader_t *ipHeader,
                      L7_ushort16 innerVlanId, L7_uint *client_idx)      /* PTin modified: DHCP snooping */
{
  /* Discard server packets received on untrusted ports */
  if (dsFilterServerMessage(intIfNum, vlanId, frame, ipHeader, innerVlanId, client_idx))    /* PTin modified: DHCP snooping */
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Packet dropped here: server filter");
    return L7_TRUE;
  }

  /* Discard certain client messages based on rx interface */
  if (dsFilterClientMessage(intIfNum, vlanId, frame, ipHeader, innerVlanId, client_idx))    /* PTin modified: DHCP snooping */
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Packet dropped here: client filter");
     return L7_TRUE;
  }

  /* Verify that the source MAC matches the client hw address */
  if (dsFilterVerifyMac(intIfNum, vlanId, frame, ipHeader))
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Packet dropped here: verify MAC");
    return L7_TRUE;
  }

  return L7_FALSE;
}
/*********************************************************************
* @purpose  Adds Double DVLAN tag with specified SVLANID.
*
* @param    intIfNum  @b{(input)} the received interface
* @param    vlanId    @b{(output)} the outer VLANId to be added.
* @param    frame     @b{(input)} Pointer to DHCP frame
* @param    frameLen  @b{(inout)} Length of the frame
* @param    innerVlanId   @b{(output)} the inner VLANId to be added.
* @param    frameOut  @b{(output)} The output frame.
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dsFrameDoubleVlanTagAdd(L7_uint32 intIfNum, L7_uint32 vlanId, L7_uchar8 *frame,
                       L7_ushort16 *frameLen, L7_ushort16 innerVlanId, L7_uchar8 *frameOut)
{
  L7_uchar8 *frameTx = frameOut, *frameTempPtr= L7_NULLPTR;
  L7_8021QTag_t cVlanTag, *sVlanTag = L7_NULLPTR;
  L7_ushort16 dvlanEthTypeTemp = 0x0;
#ifdef L7_DVLAN_PACKAGE
  L7_uint32  dvlanEthType = 0x0000;
#endif


  if (dsCfgData->dsTraceFlags & DS_TRACE_FRAME_TX)
  {
    L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
    memset(traceMsg, 0, DS_MAX_TRACE_LEN);
    osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                  "(%s)The given frame len : %d, cVlanId = %d, sVlanId = %d",
                  __FUNCTION__, *frameLen, innerVlanId, vlanId);
    dsTraceWrite(traceMsg);
  }

  frameTempPtr = frameTx;

#ifdef L7_DVLAN_PACKAGE
  if (usmDbComponentPresentCheck(simGetThisUnit(), L7_DVLANTAG_COMPONENT_ID) == L7_TRUE)
  {
    if (dvlantagIntfEthertypeGet(intIfNum , &dvlanEthType, 0) != L7_SUCCESS)
    {
      if (dsCfgData->dsTraceFlags & DS_TRACE_FRAME_TX)
      {
        L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
        memset(traceMsg, 0, DS_MAX_TRACE_LEN);
        osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                      "(%s)Failed to get given DVLAN ethernet type = %x",__FUNCTION__, dvlanEthType);
        dsTraceWrite(traceMsg);
      }
      return;
    }
    dvlanEthTypeTemp = dvlanEthType;
  }
#endif

  if (innerVlanId != L7_NULL)
  {
    /* This may need to be updated with proper priority*/
    memset(&cVlanTag, 0, sizeof(L7_8021QTag_t));
    cVlanTag.tci = innerVlanId;
    cVlanTag.tpid = FD_DVLANTAG_802_1Q_ETHERTYPE;
  }

  memset(frameTx, 0, DS_DHCP_PACKET_SIZE_MAX);
  memcpy(frameTx, frame, L7_ENET_HDR_SIZE) ;
  frameTx = frameTx + L7_ENET_HDR_SIZE;
  sVlanTag = (L7_8021QTag_t *)(frame + L7_ENET_HDR_SIZE);
  memcpy(frameTx, &dvlanEthTypeTemp, sizeof(L7_ushort16));
  frameTx = frameTx + sizeof(L7_ushort16);
  memcpy(frameTx, &(sVlanTag->tci), sizeof(L7_ushort16));
  frameTx = frameTx + sizeof(L7_ushort16);

  if (innerVlanId != L7_NULL)
  {
    memcpy(frameTx,  &cVlanTag, sizeof(L7_8021QTag_t));
    frameTx = frameTx + sizeof(L7_8021QTag_t);
  }
  memcpy(frameTx, frame + L7_ENET_HDR_SIZE + sizeof(L7_8021QTag_t),
         (*frameLen - (L7_ENET_HDR_SIZE + sizeof(L7_8021QTag_t))));
  if (innerVlanId != L7_NULL)
  {
    *frameLen = *frameLen + sizeof(L7_8021QTag_t);
  }
  return;

}

/*********************************************************************
* @purpose  Removes Dot1q VLAN tag and gets the VLAN Id.
*
* @param    frame     @b{(input)} Pointer to DHCP frame
* @param    frameLen  @b{(inout)} Length of the frame
* @param    frameOut  @b{(output)} The output frame.
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dsFrameVlanTagRemove(L7_uchar8 *frame, L7_ushort16 *frameLen,
                          L7_uchar8 *frameOut)
{
  L7_uchar8 *frameTx = frameOut, *frameTxTemp = L7_NULLPTR;
  L7_ushort16 innerTagOffset = 0;

  if (dsCfgData->dsTraceFlags & DS_TRACE_FRAME_TX)
  {
    L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
    memset(traceMsg, 0, DS_MAX_TRACE_LEN);
    osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                  "(%s)The given frame len : %d", __FUNCTION__, *frameLen);
    dsTraceWrite(traceMsg);
  }
  frameTxTemp = frameTx;

  memset(frameTx, 0, DS_DHCP_PACKET_SIZE_MAX);
  memcpy(frameTx, frame, sizeof(L7_enetHeader_t));
  innerTagOffset = sizeof(L7_enetHeader_t) + sizeof(L7_8021QTag_t);
  memcpy((frameTx + sizeof(L7_enetHeader_t)),
         (frame + innerTagOffset), ((*frameLen) - sizeof(L7_8021QTag_t)));
  *frameLen = *frameLen - sizeof(L7_8021QTag_t);

  return;
}
/*********************************************************************
* @purpose  Adds Single Outer DVLAN tag with specified SVLANID.
*
* @param    intIfNum  @b{(input)} the received interface
* @param    frame     @b{(input)} Pointer to DHCP frame
* @param    frameLen  @b{(inout)} Length of the frame
* @param    sVlanId   @b{(output)} the outer VLANId to be added.
* @param    frameOut  @b{(output)} The output frame.
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dsFrameSVlanTagAdd(L7_uint32 intIfNum, L7_uchar8 *frame, L7_ushort16 *frameLen,
                        L7_ushort16 sVlanId, L7_uchar8 *frameOut)
{
  L7_uchar8 *frameTx = frameOut, *frameTxTemp = L7_NULLPTR;
  L7_ushort16 dvlanEthType=0x00;
#ifdef L7_DVLAN_PACKAGE
    L7_uint32 dEthType = 0x0000;
#endif

  if (dsCfgData->dsTraceFlags & DS_TRACE_FRAME_TX)
  {
    L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
    memset(traceMsg, 0, DS_MAX_TRACE_LEN);
    osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                  "(%s)The given frame len : %d", __FUNCTION__, *frameLen);
    dsTraceWrite(traceMsg);
  }
  frameTxTemp = frameTx;

#ifdef L7_DVLAN_PACKAGE
  if (usmDbComponentPresentCheck(simGetThisUnit(), L7_DVLANTAG_COMPONENT_ID) == L7_TRUE)
  {
    if (dvlantagIntfEthertypeGet(intIfNum , &dEthType, 0) != L7_SUCCESS)
    {
      if (dsCfgData->dsTraceFlags & DS_TRACE_FRAME_TX)
      {
        L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
        memset(traceMsg, 0, DS_MAX_TRACE_LEN);
        osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                      "(%s)Failed to get given DVLAN ethernet type = %x",__FUNCTION__, dvlanEthType);
        dsTraceWrite(traceMsg);
      }
      return;
    }
    dvlanEthType = dEthType;
  }
  else
  {
    if (dsCfgData->dsTraceFlags & DS_TRACE_FRAME_TX)
    {
      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
      memset(traceMsg, 0, DS_MAX_TRACE_LEN);
      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                    "(%s)Failed to get given DVLAN ethernet type, as DVLAN component is not present",
                   __FUNCTION__);
      dsTraceWrite(traceMsg);
    }
    return;
  }
#endif

  memset(frameTx, 0, DS_DHCP_PACKET_SIZE_MAX);
  memcpy(frameTx, frame, L7_ENET_HDR_SIZE) ;
  frameTx = frameTx + L7_ENET_HDR_SIZE;
  memcpy(frameTx, &dvlanEthType, sizeof(L7_ushort16));
  frameTx = frameTx + sizeof(L7_ushort16);
  memcpy(frameTx, &sVlanId, sizeof(L7_ushort16));
  frameTx = frameTx + sizeof(L7_ushort16);

  memcpy(frameTx,
         frame + L7_ENET_HDR_SIZE + sizeof(L7_8021QTag_t),
        (*frameLen - (L7_ENET_HDR_SIZE + sizeof(L7_8021QTag_t))));

  return;
}
/*********************************************************************
* @purpose  Adds inner DOT1Q tag with specified VLANID.
*
* @param    frame     @b{(input)} Pointer to DHCP frame
* @param    frameLen  @b{(inout)} Length of the frame
* @param    cVlanId   @b{(output)} the inner VLANId to be inserted.
* @param    frameOut  @b{(output)} The output frame.
*
* @returns  none
*
* @notes    In Triumph devices,application gets single
*           outer tagged frame.This function modifies the
*           incoming single outer tagged frame as double tagged frame
*           by inserting inner vlan id.Length of the frame will be
*           incremented by 4 bytes.
*
*
* @end
*********************************************************************/
void dsFrameCVlanTagInsert(L7_uchar8 *frame, L7_ushort16 *frameLen,
                        L7_ushort16 cVlanId, L7_uchar8 *frameOut)
{
  L7_uchar8 *frameTx = frameOut;
  L7_8021QTag_t *cVlanTag;

  if ((*frameLen + sizeof(L7_8021QTag_t)) > DS_DHCP_PACKET_SIZE_MAX)
  {
    if (dsCfgData->dsTraceFlags & DS_TRACE_FRAME_TX)
    {
      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
      memset(traceMsg, 0, DS_MAX_TRACE_LEN);
      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                   "(%s)Max length frame arrived with len:%d, cvlanid = %d", __FUNCTION__,
                    *frameLen, cVlanId);
      dsTraceWrite(traceMsg);
    }
    return;
  }

  if (dsCfgData->dsTraceFlags & DS_TRACE_FRAME_TX)
  {
    L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
    memset(traceMsg, 0, DS_MAX_TRACE_LEN);
    osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                  "(%s)The given frame len : %d, cvlanid = %d", __FUNCTION__,
                   *frameLen, cVlanId);
    dsTraceWrite(traceMsg);
  }

  frameTx = frameTx + L7_ENET_HDR_SIZE+sizeof(L7_8021QTag_t);
  cVlanTag = (L7_8021QTag_t *)frameTx;
  memcpy(frameTx+sizeof(L7_8021QTag_t), frameTx,
         *frameLen - (L7_ENET_HDR_SIZE+sizeof(L7_8021QTag_t)));

  cVlanTag->tpid = FD_DVLANTAG_802_1Q_ETHERTYPE;
  cVlanTag->tci = cVlanId;

  *frameLen = *frameLen + sizeof(L7_8021QTag_t);

   return;
}
/*********************************************************************
* @purpose  Adds inner DOT1Q tag with specified VLANID.
*
* @param    frame     @b{(input)} Pointer to DHCP frame
* @param    frameLen  @b{(inout)} Length of the frame
* @param    cVlanId   @b{(output)} the inner VLANId to be added.
* @param    frameOut  @b{(output)} The output frame.
*
* @returns  none
*
* @notes    In Triumph devices,application gets single
*           outer tagged frame.This function modifies the
*           incoming single outer tagged frame as single inner
*           tagged frame. Length of the frame will not change in this
*           case.
*
*
* @end
*********************************************************************/
void dsFrameCVlanTagAdd(L7_uchar8 *frame, L7_ushort16 *frameLen,
                        L7_ushort16 cVlanId, L7_uchar8 *frameOut)
{
  L7_uchar8 *frameTx = frameOut, *frameTxTemp = L7_NULLPTR;
  L7_8021QTag_t *cVlanTag;

  if (dsCfgData->dsTraceFlags & DS_TRACE_FRAME_TX)
  {
    L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
    memset(traceMsg, 0, DS_MAX_TRACE_LEN);
    osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                  "(%s)The given frame len : %d, cvlanid = %d", __FUNCTION__,
                   *frameLen, cVlanId);
    dsTraceWrite(traceMsg);
  }
  frameTxTemp = frameTx;

  memset(frameTx, 0, DS_DHCP_PACKET_SIZE_MAX);
  memcpy(frameTx, frame, L7_ENET_HDR_SIZE) ;
  frameTx = frameTx + L7_ENET_HDR_SIZE;
  cVlanTag = (L7_8021QTag_t *)(frame + L7_ENET_HDR_SIZE);
  if (cVlanTag->tpid == FD_DVLANTAG_802_1Q_ETHERTYPE)
  {
    cVlanTag = (L7_8021QTag_t *)(frameTx);
    cVlanTag->tpid = FD_DVLANTAG_802_1Q_ETHERTYPE;
    cVlanTag->tci = cVlanId;
    frameTx = frameTx + sizeof(L7_8021QTag_t);
    memcpy(frameTx, frame + L7_ENET_HDR_SIZE + sizeof(L7_8021QTag_t),
           (*frameLen - (L7_ENET_HDR_SIZE + sizeof(L7_8021QTag_t))));
    return;
  }
  else
  {
    memcpy(frameTx, frame + L7_ENET_HDR_SIZE,
           sizeof(L7_8021QTag_t));
    frameTx = frameTx + sizeof(L7_8021QTag_t);
    cVlanTag = (L7_8021QTag_t *)(frameTx);
    if (cVlanTag->tpid == FD_DVLANTAG_802_1Q_ETHERTYPE)
    {
      cVlanTag->tci = cVlanId;
      cVlanTag->tpid = FD_DVLANTAG_802_1Q_ETHERTYPE;
      frameTx = frameTx + sizeof(L7_8021QTag_t);
      memcpy(frameTx, frame + L7_ENET_HDR_SIZE + sizeof(L7_8021QTag_t),
             (*frameLen - (L7_ENET_HDR_SIZE + sizeof(L7_8021QTag_t))));
      return;
    }
  }
  return;
}
#ifdef L7_DHCP_L2_RELAY_PACKAGE
/*********************************************************************
* @purpose  Calculates UDP checksum for modified DHCP packet.
*
* @param    frame     @b{(input)} Pointer to DHCP frame
* @param    frameLen  @b{(inout)} Length of the frame
* @param    added     @b{(input)} the bytes added or deleted.
* @param    lenChng   @b{(input)} number of the bytes added or deleted.
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void dsUdpCheckSumCalculate(L7_uchar8 *frame, L7_uint32 *frameLen,
                            L7_BOOL added, L7_ushort16 lenChng)
{
   L7_ushort16 ethHdrLen = sysNetDataOffsetGet(frame);
   L7_uchar8 ipVersion = (0xF0 & *(L7_uchar8*) (frame + ethHdrLen)) >> 4;

   if (L7_IP_VERSION == ipVersion)
   {
      L7_ipHeader_t *ipHeader = (L7_ipHeader_t*) (frame + ethHdrLen);
      L7_udp_header_t *udp_header;
      L7_ushort16 ipHdrLen = dsIpHdrLen(ipHeader), proto = IP_PROT_UDP, udpLen = 0;
      L7_uchar8 *psuedoHdr = dsInfo->pktBuff, *tempPtr = L7_NULLPTR;

      if ((dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CLIENT) || (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_SERVER))
      {
         L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
         osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN, "(%s)The length changed (%d) is %s for UDP checksum calculation.", __FUNCTION__, lenChng, added == L7_TRUE ? "added" : "removed");
         dsTraceWrite(traceMsg);
      }

      /* Calculate the UDP checksum.*/
      udp_header = (L7_udp_header_t *) ((L7_char8 *) ipHeader + ipHdrLen);
      udp_header->checksum = 0x0000;

      if ((added == L7_TRUE))
      {
         udp_header->length = osapiHtons(osapiNtohs(udp_header->length) + lenChng);
         *frameLen = *frameLen + lenChng;
      }
      else
      {
         udp_header->length = osapiHtons(osapiNtohs(udp_header->length) - lenChng);
         *frameLen = *frameLen - lenChng;
      }
      udpLen = udp_header->length;
      proto  = osapiHtons(proto);

      memset(psuedoHdr, 0, DS_DHCP_PACKET_SIZE_MAX);
      tempPtr = psuedoHdr; /* To store start of the formed psuedoHdr.*/
      memcpy(psuedoHdr, &(ipHeader->iph_src), 4);
      psuedoHdr += 4;
      memcpy(psuedoHdr, &(ipHeader->iph_dst), 4);
      psuedoHdr += 4;
      memcpy(psuedoHdr, &proto, 2);
      psuedoHdr += 2;
      memcpy(psuedoHdr, &udpLen, 2);
      psuedoHdr += 2;

      memcpy(psuedoHdr, udp_header, osapiNtohs(udp_header->length));
      udp_header->checksum = osapiHtons((L7_uint16) inetChecksum(tempPtr, ((osapiNtohs(udp_header->length)) + 12)));
      /* Reset the packet buffer after usage.*/
      psuedoHdr = dsInfo->pktBuff;
      memset(psuedoHdr, 0, DS_DHCP_PACKET_SIZE_MAX);

      /* Now calculate the IP checksum.*/
      if ((added == L7_TRUE))
      {
         ipHeader->iph_len = osapiHtons(osapiNtohs(ipHeader->iph_len) + lenChng);
      }
      else
      {
         ipHeader->iph_len = osapiHtons(osapiNtohs(ipHeader->iph_len) - lenChng);
      }
      ipHeader->iph_csum = 0x0000;
      ipHeader->iph_csum = osapiHtons((L7_uint16) inetChecksum(ipHeader, ipHdrLen));

      if ((dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CLIENT) || (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_SERVER))
      {
         L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
         osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN, "(%s)The new UDP length (%d), UDP chksum (%x) IP-len (%d) IP- chksum (%x).", __FUNCTION__, osapiNtohs(udp_header->length), osapiNtohs(udp_header->checksum), osapiNtohs(ipHeader->iph_len), osapiNtohs(ipHeader->iph_csum));
         dsTraceWrite(traceMsg);
      }
   }
   else
   {
      L7_ip6Header_t *ipHeader = (L7_ip6Header_t*) (frame + ethHdrLen);
      L7_udp_header_t *udp_header = (L7_udp_header_t *) ((L7_char8 *) ipHeader + L7_IP6_HEADER_LEN);
      L7_uint32 udpLen = 0, proto = IP_PROT_UDP;
      L7_uchar8 *psuedoHdr = dsInfo->pktBuff, *tempPtr = L7_NULLPTR;

      /* Calculate the UDP checksum.*/
      udp_header->checksum = 0x0000;

      udpLen = udp_header->length;
      proto  = osapiHtonl(proto);

      memset(psuedoHdr, 0, DS_DHCP_PACKET_SIZE_MAX);
      tempPtr = psuedoHdr; /* To store start of the formed psuedoHdr.*/
      memcpy(psuedoHdr, &(ipHeader->src), 16);
      psuedoHdr += 16;
      memcpy(psuedoHdr, &(ipHeader->dst), 16);
      psuedoHdr += 16;
      memcpy(psuedoHdr, &udpLen, 4);
      psuedoHdr += 4;
      memcpy(psuedoHdr, &proto, 4);
      psuedoHdr += 4;

      memcpy(psuedoHdr, udp_header, osapiNtohs(udp_header->length));
      udp_header->checksum = osapiHtons((L7_uint16) inetChecksum(tempPtr, (osapiNtohs(udp_header->length) + 40)));

      /* Reset the packet buffer after usage.*/
      psuedoHdr = dsInfo->pktBuff;
      memset(psuedoHdr, 0, DS_DHCP_PACKET_SIZE_MAX);
   }
}
#endif
/*********************************************************************
* @purpose  Forwards a DHCP reply packet based on DHCP Snooping
*           tentative entry.
*
* @param    intIfNum   @b{(input)} receive interface
* @param    vlanId     @b{(input)} VLAN ID
* @param    macaddr    @b{(input)} MAC address to which the packet
*                                  needs to be forwarded.
* @param    frame        @b{(input)} ethernet frame
* @param    frameLen     @b{(input)} ethernet frame length,
*                                    incl eth header (bytes)
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t dsReplyFrameForward(L7_uint32 intIfNum, L7_uint32 vlanId, L7_uchar8 *macaddr,
                            L7_uchar8 *frame, L7_ushort16 frameLen,
                            L7_ushort16 innerVlanId, L7_uint client_idx)      /* PTin modified: DHCP snooping */
{
  L7_uint32       mode = 0;
  L7_ushort16 vlanIdFwd = vlanId;
  dhcpSnoopBinding_t dsBinding;

  memset(&dsBinding,'\0', sizeof(dhcpSnoopBinding_t));
  memcpy(dsBinding.key.macAddr, macaddr, L7_ENET_MAC_ADDR_LEN);
  dsBinding.key.ipType = L7_AF_INET;

  if (dsCfgData->dsTraceFlags & DS_TRACE_FRAME_TX)
  {
      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                    "(%s) Given params to get snoop entry are intf (%d) vlanId (%d)"
                    " MAC(%2x:%2x:%2x:%2x:%2x:%2x)",__FUNCTION__, intIfNum, vlanIdFwd,
                    macaddr[0], macaddr[1],macaddr[2],macaddr[3], macaddr[4], macaddr[5]);
      dsTraceWrite(traceMsg);
  }

  if (dsBindingFind(&dsBinding, L7_MATCH_EXACT) != L7_SUCCESS)
  {
    if (dsCfgData->dsTraceFlags & DS_TRACE_FRAME_TX)
    {
      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                    "(%s)Failed to get snoop entry for "
                    " MAC(%2x:%2x:%2x:%2x:%2x:%2x)", __FUNCTION__,
                    macaddr[0], macaddr[1],macaddr[2],macaddr[3], macaddr[4], macaddr[5]);
      dsTraceWrite(traceMsg);
    }
  }
  else
  {
    /* Forward based on learnt MAC entry.*/
    if ( (dsBinding.intIfNum == intIfNum) ||
         (dsBinding.vlanId   != vlanIdFwd)
       )
    {
      return L7_FAILURE;
    } /*Perform egress filtering before sending out the server reply */
    if( (dot1qOperVlanMemberGet(vlanId,dsBinding.intIfNum,&mode)==L7_SUCCESS)&&
             (mode == L7_DOT1Q_FORBIDDEN)
           )
    {
      /* Returning success rather failure to avoid the further flooding in the VLAN
         as we already know that this MAC address is on this port */
      if (dsCfgData->dsTraceFlags & DS_TRACE_FRAME_TX)
      {
        L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
        osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                    "(%s)Egress filtering for "
                    " MAC(%2x:%2x:%2x:%2x:%2x:%2x) on Vlan -%d", __FUNCTION__,
                    macaddr[0], macaddr[1],macaddr[2],macaddr[3], macaddr[4], macaddr[5],vlanId);
         dsTraceWrite(traceMsg);
      }
      return L7_SUCCESS;
    }
    else if (mode)
    {
      if (dsFrameIntfFilterSend(dsBinding.intIfNum, vlanIdFwd, frame, frameLen,
                         L7_FALSE, innerVlanId, client_idx) == L7_SUCCESS)        /* PTin modified: DHCP snooping */
      {
        dsInfo->debugStats.serverOption82Tx++;
        ptin_dhcp_stat_increment_field(dsBinding.intIfNum, vlanId, client_idx, DHCP_STAT_FIELD_TX_SERVER_REPLIES_WITHOUT_OPTIONS);
        ptin_dhcp_stat_increment_field(dsBinding.intIfNum, vlanId, client_idx, DHCP_STAT_FIELD_TX_FORWARDED);
        return L7_SUCCESS;
      }
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Checks if DHCP Snooping is enabled for a given VLANID
*
* @param    intIfNum   @b{(input)} receive interface
* @param    vlanId     @b{(input)} VLAN ID
* @param    rxSVlanId  @b{(input)} received SVLAN ID (applicable only in
*                                   Metro for Reply packets coming from NNI)
* @param    dhcpOp     @b{(input)}  DHCP opcode (REQUEST/REPLY)
* @param    cVlanId    @b{(input)} CVLAN ID (applicable only in Metro,
*                                 remark CVId specified for matched subscription)
* @param    sVlanId    @b{(output)} SVLAN ID (applicable only in Metro,
*                                 service SVId specified for matched subscription)
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    In addition Snooping enable status, this API also returns the
*           SVID and remark CVID in case of Metro builds which are used in
*           forwarding request packets.
*           This also assumes the rxSVlanId has the S-Tag VLANID in case of
*           replies returned by DHCP server on NNI port.
*
* @end
*********************************************************************/
L7_BOOL dsSnoopingServiceIsEnabled(L7_uint32 intIfNum, L7_uint32 vlanId,
                                   L7_uint32 rxSVlanId, L7_uchar8 dhcpOp,
                                   L7_uint32 *cVlanId, L7_uint32 *sVlanId)
{
#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
  if (dhcpOp == L7_DHCP_BOOTP_REQUEST)
  {

    *sVlanId = L7_NULL;
#if 0
    if (dot1adServiceIntfSvidMapGet(intIfNum, vlanId, subscriptionName,
                                    sVlanId, cVlanId) != L7_SUCCESS)
#endif
    {
      L7_uchar8 dsTrace[DS_MAX_TRACE_LEN];
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      osapiSnprintf(dsTrace, DS_MAX_TRACE_LEN,
                    "DHCP L2 Relay failed to get subscription for Iface %s with VLAN %u",
                    ifName, vlanId);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID, dsTrace);
      return L7_FALSE;
    }
  }
  else if (dhcpOp == L7_DHCP_BOOTP_REPLY)
  {
    *sVlanId = rxSVlanId;
  }
#else
  *sVlanId = vlanId;
#endif
  return _dsVlanEnableGet(*sVlanId);
}
#ifdef L7_DHCP_L2_RELAY_PACKAGE
/*********************************************************************
* @purpose  Checks if DHCP L2 rleay is enabled for a given VLANID
*
* @param    intIfNum    @b{(input)} receive interface
* @param    vlanId      @b{(input)} VLAN ID (SVLANID if double tagged)
* @param    innerVlanId @b{(input)} CVLAN ID (applicable only in Metro)
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    This API assumes the vlanId (SVLANID) and innerVlanId (CVLANID)
*           to already contain the translated values based on DOT1AD subscriptions.
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL dsL2RelayServiceIsEnabled(L7_uint32 intIfNum, L7_uint32 vlanId,
                                  L7_ushort16 innerVlanId)
{
#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
  L7_uchar8 subscriptionName[L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX + 1];
  dsL2RelaySubscriptionCfg_t *subsCfg;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  if (dot1adSubscriptionNameGet(intIfNum, vlanId, innerVlanId, subscriptionName)
                               != L7_SUCCESS)
  {
    L7_uchar8 dsTrace[DS_MAX_TRACE_LEN];
    osapiSnprintf(dsTrace, DS_MAX_TRACE_LEN,
                  "DHCP L2 Relay failed to get subscription for Iface %s with VLAN %u, innerVlanId = %d",
                  ifName, vlanId, innerVlanId);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID, dsTrace);
    return L7_FALSE;
  }
  else
  {
    if (dsL2RelaySubscriptionEntryGet(intIfNum, subscriptionName, L7_TRUE,
                                      &subsCfg) != L7_SUCCESS)
    {
      L7_uchar8 dsTrace[DS_MAX_TRACE_LEN];
      osapiSnprintf(dsTrace, DS_MAX_TRACE_LEN,
                    "(%s)DHCP L2 Relay failed to get subscription name entry for Iface %d, %s with VLAN %u and subsc = %s",
                    __FUNCTION__, intIfNum, ifName, vlanId, subscriptionName);
      dsTraceWrite(dsTrace);
    }
    else
    {
      if (subsCfg->l2relay != L7_ENABLE)
      {
        if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CLIENT)
        {
          L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
          osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                        "(%s)DHCP L2 Relay subscription "
                        "does not have L2 Relay enabled on intf = %d, %s,"
                        " vlanId = %d, subscName = %s", __FUNCTION__,
                           intIfNum, ifName, vlanId, subscriptionName);
          dsTraceWrite(traceMsg);
        }
        return L7_FALSE;
      }
      return L7_TRUE;
    }
  }
#endif

  if ((vlanId != L7_NULL) && (_dsVlanL2RelayGet(vlanId) == L7_TRUE))
  {
    return L7_TRUE;
  }
  return L7_FALSE;
}

void dsL2RelayRelayAgentInfoOptionCfgGet(L7_uint32 intIfNum, L7_uint32 vlanId, L7_ushort16 innerVlanId, L7_uchar8 *macAddr,
                                         L7_BOOL *cIdFlag, L7_uchar8 *circuitIdStr,                     /* PTin modified: DHCP snooping */
                                         L7_BOOL *rIdFlag, L7_uchar8 *remoteIdStr )
{
#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
  dsL2RelaySubscriptionCfg_t *subsCfg;
  L7_uchar8 subscriptionName[L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX + 1];
  L7_uchar8 endOfStr = FD_DHCP_L2RELAY_REMOTE_ID_VLAN_MODE;
  L7_RC_t rc = L7_SUCCESS;
  DOT1AD_INTFERFACE_TYPE_t intfType;

  if (dot1adInterfaceTypeGet(intIfNum, &intfType) != L7_SUCCESS)
  {
    return;
  }
  /* The function calls for get matching subscription in DOT1AD is different UNI 
     and NNI interfaces. Since REQUESTs are transmitted from UNI and REPLYs are 
     expected from NNI, find matching subscription Names differs.   */
  if (intfType == DOT1AD_INTFERFACE_TYPE_UNI || intfType == DOT1AD_INTFERFACE_TYPE_UNI_P)
  {
    rc = dot1adSubscriptionNameGet(intIfNum, vlanId, innerVlanId, subscriptionName);
  }
  else
  {
    rc = dot1adNniSubscriptionNameGet(intIfNum, vlanId, innerVlanId, subscriptionName);
  }
  /* Ignore the packet if the SVLAN Id for the received packet VlandId
     does not have DHCP L2 Relay enabled, though the packet is eligible
     for forwarding based on above receive-side checks. */
  if (rc != L7_SUCCESS)
  {
    L7_uchar8 dsTrace[DS_MAX_TRACE_LEN];
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    osapiSnprintf(dsTrace, DS_MAX_TRACE_LEN,
                  "DHCP L2 Relay failed to get service VLAN for Iface %s with VLAN %u and innerVlanId = %d",
                  ifName, vlanId, innerVlanId);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID, dsTrace);
    return;
  }
  else
  {
    if (dsL2RelaySubscriptionEntryGet(intIfNum, subscriptionName, L7_TRUE,
                                      &subsCfg) != L7_SUCCESS)
    {
      L7_uchar8 dsTrace[DS_MAX_TRACE_LEN];
      osapiSnprintf(dsTrace, DS_MAX_TRACE_LEN,
                    "(%s)DHCP L2 Relay failed to get subscription entry for Iface %d with VLAN %u",
                    __FUNCTION__, intIfNum, vlanId);
      dsTraceWrite(dsTrace);
      *cIdFlag = L7_FALSE;
      *rIdFlag = L7_FALSE;
    }
    else
    {
      if (subsCfg->circuitId == L7_ENABLE)
      {
        *cIdFlag = L7_TRUE;
      }
      if (osapiStrncmp(subsCfg->remoteId, &endOfStr, sizeof(L7_uchar8)) != L7_NULL)
      {
        *rIdFlag = L7_TRUE;
        osapiStrncpy(remoteIdStr, subsCfg->remoteId, DS_MAX_REMOTE_ID_STRING);
      }
      return;
    }
  }
#endif

  /* PTin replaced: DHCP snooping */
  #if 1
  *cIdFlag = L7_FALSE;
  *rIdFlag = L7_FALSE;

  if (ptin_dhcp_stringIds_get(intIfNum,vlanId,innerVlanId,macAddr,circuitIdStr,remoteIdStr)==L7_SUCCESS)
  {
    if (*circuitIdStr!='\0')  *cIdFlag = L7_TRUE;
    if (*remoteIdStr !='\0')  *rIdFlag = L7_TRUE;

    if (ptin_debug_dhcp_snooping)
    {
      LOG_TRACE(LOG_CTX_PTIN_DHCP,"CircuitId=\"%s\"\r\n",circuitIdStr);
      LOG_TRACE(LOG_CTX_PTIN_DHCP,"remoteId =\"%s\"\r\n",remoteIdStr);
    }
  }
  #else
  if (_dsVlanL2RelayCircuitIdGet(vlanId))
  {
    *cIdFlag = L7_TRUE;
  }
  if (_dsVlanL2RelayRemoteIdGet(vlanId))
  {
    *rIdFlag = L7_TRUE;
    osapiStrncpy(remoteIdStr, dsCfgData->dsL2RelayRemoteId[vlanId],
                 DS_MAX_REMOTE_ID_STRING);
  }
  return;
  #endif
}
/*********************************************************************
* @purpose  Add DHCP Relay Agent Information Option to DHCP.
*
* @param    frame     @b{(input)} Pointer to DHCP frame
* @param    intIfNum  @b{(inout)} internal interface number on which frame
*                                 is received.
* @param    vlanId   @b{(input)}  VLAN Id
* @param    frameLen  @b{(inout)} Length od the frame.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ALREADY_CONFIGURED If the packet already has op82
*
* @notes    The packet is added with Option-82 only if the packet does not
*           already has Option-82.
*
* @end
*********************************************************************/
static
L7_RC_t dsRelayAgentInfoAdd(L7_uint32 intIfNum, L7_uint32 vlanId,
                            L7_ushort16 innerVlanId,
                            L7_uchar8 *frame, L7_uint32 *frameLen)
{

  L7_uint32 pktSize;
  L7_BOOL   stopProcessing = L7_FALSE;
  L7_BOOL   DHCP_PKT       = L7_FALSE;
  L7_uchar8 *dhcpOption, *subOption, *maxPktLen;
  nimUSP_t   nimUsp;
  L7_BOOL    cidFlag = L7_FALSE;
  L7_BOOL    ridFlag = L7_FALSE;
  L7_uchar8  optLen_cId, circuitIdStr[DS_MAX_REMOTE_ID_STRING];   /* PTin added: DHCP snooping */
  L7_uchar8  optLen_rId, remoteIdStr[DS_MAX_REMOTE_ID_STRING];
  L7_udp_header_t *udp_header;
  L7_dhcp_packet_t *pDhcpPacket;
  L7_ushort16 ipPktLen;
  L7_ushort16 dhcpPacketLength;
  L7_ushort16 ethHdrLen = sysNetDataOffsetGet(frame);
  L7_ipHeader_t *ipHeader = (L7_ipHeader_t*)(frame + ethHdrLen);
  L7_ushort16 ipHdrLen = dsIpHdrLen(ipHeader);

  udp_header = (L7_udp_header_t *)((L7_char8 *)ipHeader + ipHdrLen);
  pDhcpPacket = (L7_dhcp_packet_t*)((L7_char8 *)udp_header + sizeof(L7_udp_header_t));
  ipPktLen = osapiNtohs(ipHeader->iph_len);
  dhcpPacketLength = ipPktLen - ipHdrLen - sizeof(L7_udp_header_t);

  dsL2RelayRelayAgentInfoOptionCfgGet(intIfNum, vlanId, innerVlanId, &frame[L7_MAC_ADDR_LEN],
                                      &cidFlag, circuitIdStr,       /* PTin modified: DHCP snooping */
                                      &ridFlag, remoteIdStr );
  if ( cidFlag == L7_FALSE && ridFlag == L7_FALSE)
  {
    /* Our device has nothing to do as we are not configured for
       addition of Option-82.*/
    if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CLIENT)
    {
      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                    "(%s)Failed to add DHCP Option-82 for Client request as "
                    "none of the suboptions is enabled on intIfNum %d", __FUNCTION__, intIfNum);
      dsTraceWrite(traceMsg);
    }
    return L7_SUCCESS;
  }


  /* Intializing nimUSP. */
  memset (&nimUsp, L7_NULL, sizeof(nimUSP_t));
  pktSize = dhcpPacketLength;

  /*if condition fails, it'a DHCP Packet, so Add Agent options */
  if(memcmp ((pDhcpPacket+ 1), DS_DHCP_OPTIONS_COOKIE, L7_DHCP_MAGIC_COOKIE_LEN))
  {
    if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CLIENT)
    {
      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                    "(%s)Failed to add Option-82 for Client request as it "
                    "does not have expected Cookie", __FUNCTION__);
      dsTraceWrite(traceMsg);
    }
    return L7_SUCCESS;
  }

  maxPktLen = (L7_uchar8*)(pDhcpPacket) + pktSize; /*length of the REQUEST Packet */
  dhcpOption = (L7_uchar8 *)( pDhcpPacket + 1 );
  dhcpOption = dhcpOption + L7_DHCP_MAGIC_COOKIE_LEN;

  while((dhcpOption < maxPktLen) && (!stopProcessing) )
  {
      switch(*dhcpOption)
      {
      /* Skip zeroes padded by client ... */
      case DHO_PAD:
          ++dhcpOption;
          continue;

          /* if message type is there(53) , it's a DHCP packet. */
      case DHO_DHCP_MESSAGE_TYPE:
          DHCP_PKT = L7_TRUE;
          dhcpOption += dhcpOption [DHCP_OPTION_LENGTH_OFFSET] + DHCP_OPTION_CONTENT_OFFSET;
          break;

          /* Quit immediately if we hit an End option. */
      case DHO_END:
          stopProcessing = L7_TRUE; /* while loop breaks */
          break;

      case DHCP_RELAY_AGENT_INFO_OPTION:
          /* Relay agent option  shouldn't be there before
          the DHCP packet type, if it is there, leave it as is. */
          if (!DHCP_PKT)
          {
            return L7_FAILURE;
          }
          /* There's already a Relay Agent Information option
          in this packet. So we are not going to modify it or add more.*/
          return L7_ALREADY_CONFIGURED;

      default:
          dhcpOption += dhcpOption [DHCP_OPTION_LENGTH_OFFSET] + DHCP_OPTION_CONTENT_OFFSET;
          break;
      }
  }
  if(!DHCP_PKT)
  {
    if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CLIENT)
    {
      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                    "(%s)Failed to add Option-82 for Client request as it "
                    "is invalid DHCP packet(no msg type option)", __FUNCTION__);
      dsTraceWrite(traceMsg);
    }
    return L7_SUCCESS;
  }

  /* end of the packet after parsing */
  subOption = dhcpOption;

  /* setting Relay Agent Information option. */
  *subOption++ = DHCP_RELAY_AGENT_INFO_OPTION;
  *subOption++ = 0;

  if (cidFlag == L7_TRUE)
  {
    /* Copy  the Circuit Id Sub option specifier 1... */
    *subOption++ = DHCP_RELAY_AGENT_CIRCUIT_ID_SUBOPTION;
    /* PTin replaced: DHCP snooping */
    #if 1
    optLen_cId = strlen(circuitIdStr);
    *subOption++ = optLen_cId;
    osapiStrncpy(subOption, circuitIdStr, optLen_cId);
    subOption += optLen_cId;
    #else
    *subOption++ = DHCP_RELAY_AGENT_CIRCUIT_ID_LENGTH;
    if (nimGetUnitSlotPort(intIfNum, &nimUsp) == L7_SUCCESS)
    {
      *subOption++ = nimUsp.unit;
      *subOption++ = nimUsp.slot;
      *subOption++ = (L7_uchar8)nimUsp.port;
    }
    #endif

    if (ptin_debug_dhcp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_DHCP,"CircuitId=\"%s\"\r\n",circuitIdStr);
  }
  if (ridFlag == L7_TRUE)
  {
    /* Copy  the Remote Id Sub option specifier 1... */
    *subOption++ = DHCP_RELAY_AGENT_REMOTE_ID_SUBOPTION;
    optLen_rId = strlen(remoteIdStr);
    *subOption++ = optLen_rId;

    osapiStrncpy(subOption, remoteIdStr, optLen_rId);
    subOption += optLen_rId;

    if (ptin_debug_dhcp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_DHCP,"RemoteId=\"%s\"\r\n",remoteIdStr);
  }


  dhcpOption [DHCP_OPTION_LENGTH_OFFSET] = subOption - dhcpOption -
                                             DHCP_OPTION_CONTENT_OFFSET;
                                 /* storing length of Relay Agent option */

  /* Deposit an END token. */
  *subOption++ = DHO_END;
  /* Need to recalculate UDP checksum. */
  dsUdpCheckSumCalculate(frame, frameLen, L7_TRUE, (dhcpOption [DHCP_OPTION_LENGTH_OFFSET] +
                                          DHCP_OPTION_CONTENT_OFFSET));
  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Removes/Gets the DHCP relay agent options when sending to
*           server.
*
* @param    frame            @b{(input)} Pointer to DHCP frame
* @param    frameLen         @b{(inout)} Length of the frame
* @param    packet           @b{(input)} Pointer to Dhcp Packet
* @param    dhcpPacketLength @b{(input)} length of the Dhcp Packet
* @param    optionsRemove    @b{(input)} Remove/Get the options.
* @param    relayAgentInfo   @b{(output)} Relay structure having to
*                                         extract Relay agent option info.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsRelayAgentInfoRemoveOrGet (L7_uchar8 *frame,
                                     L7_uint32 *frameLen,
                                     L7_dhcp_packet_t   *packet,
                                     L7_uint32          dhcpPacketLength,
                                     L7_BOOL            optionsRemove,
                                     dsRelayAgentInfo_t *relayAgentInfo)
{
  L7_BOOL DHCP_PACKET = L7_FALSE;
  L7_uchar8 *dhcpOption, *maxPktLen;
  L7_uchar8 *relayOffset = L7_NULLPTR, *relayEnd;
  L7_uchar8 optLen = 0, relayLen = 0;
  L7_ushort16 pktSize;
  L7_BOOL Agent_Option_OK = L7_FALSE;
  L7_BOOL stop_processing = L7_FALSE;
  L7_uint32 passOverLen = 0;

  /* If there's no cookie, it's a bootp packet, so we should just
     forward it unchanged. */
  if (memcmp ((packet + 1), DS_DHCP_OPTIONS_COOKIE, L7_DHCP_MAGIC_COOKIE_LEN))
  {
    if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_SERVER)
    {
      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                    "(%s)Failed to %s DHCP Option-82 for server reply as "
                    "it does not have expected cookie.", __FUNCTION__,
                    optionsRemove ? "remove":"get");
      dsTraceWrite(traceMsg);
    }
    if (ptin_debug_dhcp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_DHCP,"There's no cookie...");
    return L7_SUCCESS;
  }

  pktSize = dhcpPacketLength;
  maxPktLen = (L7_uchar8*)(packet) + pktSize;

  dhcpOption = (L7_uchar8 *)(packet + 1);
  dhcpOption = dhcpOption + L7_DHCP_MAGIC_COOKIE_LEN;

  while ( (dhcpOption < maxPktLen) && (!stop_processing) )
  {
    switch (*dhcpOption)
    {
    /*  */
    case DHO_PAD:
      ++dhcpOption;
      continue;

      /* If we see a message type, it's a DHCP packet. */
    case DHO_DHCP_MESSAGE_TYPE:
      DHCP_PACKET = L7_TRUE;
      /*goto skip;*/
      /*replace skip with the following */
      dhcpOption += dhcpOption [DHCP_OPTION_LENGTH_OFFSET] + DHCP_OPTION_CONTENT_OFFSET;
      break;

      /* Quit immediately if we come across End option. */
    case DHO_END:
      stop_processing = L7_TRUE; /* while loop breaks */
      break;

    case DHCP_RELAY_AGENT_INFO_OPTION:
      /* We shouldn't see a relay agent option in a
         packet before we've seen the DHCP packet type,
         but if we do, we have to leave it alone. */

      if (!DHCP_PACKET)
      {
        if (ptin_debug_dhcp_snooping)
          LOG_TRACE(LOG_CTX_PTIN_DHCP,"Not a DHCP packet");
        return L7_FAILURE;
      }
      stop_processing = L7_TRUE; /* while loop breaks */
      relayOffset = dhcpOption;
      Agent_Option_OK = L7_TRUE;
      break;


    default:
      dhcpOption += dhcpOption [DHCP_OPTION_LENGTH_OFFSET] + DHCP_OPTION_CONTENT_OFFSET;
      break;
    }
  }

  /* If it's not a DHCP packet, Relay it as is */
  if (!DHCP_PACKET)
  {
    if ((dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_SERVER) ||
        (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CLIENT))
    {
      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                    "(%s)Failed to %s Option-82 as it is invalid DHCP packet(no msg type option)",
                    __FUNCTION__, optionsRemove ? "remove":"get");
      dsTraceWrite(traceMsg);
    }
    if (ptin_debug_dhcp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_DHCP,"Not a DHCP packet");
    return L7_SUCCESS;
  }

  /* If the relay agent option itself is not found for it to remove,
     return success.*/
  if (!Agent_Option_OK)
  {
    if ((dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_SERVER) ||
        (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CLIENT))
    {
      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                    "(%s)Failed to %s Option-82 as the "
                    "DHCP packet does not have Relay options.",
                    __FUNCTION__, optionsRemove ? "remove":"get");
      dsTraceWrite(traceMsg);
    }
    if (ptin_debug_dhcp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_DHCP,"DHCP packet does not have Relay options");
    return L7_SUCCESS;
  }

  /* 
     RFC 3046 - 2.0
   
     The length N of the DHCP Agent Information Option shall include all bytes of the sub-option
     code/length/value tuples.  Since at least one sub-option must be defined, the minimum Relay
     Agent Information length is two (2).
  */
  if((*(relayOffset + 1)) < DHCP_OPTION_CONTENT_OFFSET)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_ERR(LOG_CTX_PTIN_DHCP, "Invalid DHCP Agent Information Option length [%u]", *(relayOffset + 1));
    return L7_FAILURE;
  }

  /* If the relay agent option is present but the expected or
     valid suboptions are not present, then drop the packet. */
  /* Parse the relay agent suboptions. */
  relayLen = *(relayOffset + 1);
  relayEnd = relayOffset + relayLen + DHCP_OPTION_CONTENT_OFFSET;
  passOverLen = relayEnd - (L7_uchar8 *)packet;
  relayOffset += DHCP_OPTION_CONTENT_OFFSET;
  while ((relayOffset < relayEnd) && (*relayOffset != DHO_END))
  {
    if (ptin_debug_dhcp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_DHCP,"Suboption detected? %u (relayOffset=%u)",*relayOffset,relayOffset);

    switch (*relayOffset)
    {
    case DHCP_RELAY_AGENT_CIRCUIT_ID_SUBOPTION:
      {
        optLen = *(relayOffset + 1);

        if (ptin_debug_dhcp_snooping)
          LOG_TRACE(LOG_CTX_PTIN_DHCP,"optLen=%u",optLen);

        relayOffset += DHCP_OPTION_CONTENT_OFFSET;

        /* Evaluate Opt Length */
        if ((relayOffset+optLen)>relayEnd)
        {
          if (ptin_debug_dhcp_snooping)
            LOG_TRACE(LOG_CTX_PTIN_DHCP,"optLen is too big");
          return L7_FAILURE;
        }

        /* PTin removed: DHCP snooping */
        #if 0
        if (optLen != DHCP_RELAY_AGENT_CIRCUIT_ID_LENGTH)
        {
          /* Not expected circuit-id option format.*/
          if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_SERVER)
          {
            L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
            osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                          "(%s)Failed to %s Option-82 for server reply as the "
                          "DHCP packet does not have expected circuit-id length.",
                          __FUNCTION__, optionsRemove ? "remove":"get");
            dsTraceWrite(traceMsg);
          }
          if (optionsRemove == L7_TRUE)
          {
            return L7_FAILURE;
          }
        }
        #endif

        relayAgentInfo->circuitIdFlag = L7_TRUE;

        // PTin replaced: DHCP snooping
        #if 0
        L7_int board_slot;
        L7_int unit, slot, port, ind;

        board_slot = 0;

        // Search for the sequence " eth "
        for (ind=0; ind<=(optLen-5-3) && strncmp(&relayOffset[ind]," eth ",5)!=0; ind++ );
        if (ind>optLen-5-3)
        {
          if (ptin_debug_dhcp_snooping)
            LOG_TRACE(LOG_CTX_PTIN_DHCP,"Invalid circuit id length");
          return L7_FAILURE;
        }
        ind+=5;

        unit = 1;
        relayAgentInfo->usp.unit = unit;
        #if ( PTIN_BOARD_IS_MATRIX )
        if (sscanf(&relayOffset[ind],"%*2d-%1d%2d",&slot,&port)!=2)
        {
          if (ptin_debug_dhcp_snooping)
            LOG_TRACE(LOG_CTX_PTIN_DHCP,"Failure here");
          slot = 0;
          port = 0;
        }
        relayAgentInfo->usp.slot = slot;
        relayAgentInfo->usp.port = port+1;
        relayAgentInfo->board_slot = 1;
        #elif ( PTIN_BOARD_IS_LINECARD )
        if (sscanf(&relayOffset[ind],"%2d-%*1d%*2d/%1d%2d",&board_slot,&slot,&port)!=3)
        {
          if (ptin_debug_dhcp_snooping)
            LOG_TRACE(LOG_CTX_PTIN_DHCP,"Failure here");
          slot = 0;
          port = 0;
          board_slot = 0;
        }
        relayAgentInfo->usp.slot = slot;
        relayAgentInfo->usp.port = port+1;
        relayAgentInfo->board_slot = board_slot;
        #else
        if (sscanf(&relayOffset[ind],"%d/%d",&slot,&port)!=2)
         {
          if (ptin_debug_dhcp_snooping)
            LOG_TRACE(LOG_CTX_PTIN_DHCP,"Failure here");
          slot = 0;
          port = 0;
         }
        relayAgentInfo->usp.slot = slot-1;
        relayAgentInfo->usp.port = port+1;
        relayAgentInfo->board_slot = 0;
        #endif
        
        if (ptin_debug_dhcp_snooping)
          LOG_TRACE(LOG_CTX_PTIN_DHCP,"{unit,slot,port}={%d,%d,%d}\r\n",unit,slot,port);

        #endif
        relayOffset += optLen;
        #if 0
        relayAgentInfo->usp.unit = *relayOffset++;
        relayAgentInfo->usp.slot = *relayOffset++;
        relayAgentInfo->usp.port = *relayOffset++;
        #endif
        #if 0
        if (nimGetIntIfNumFromUSP(&relayAgentInfo->usp, &relayAgentInfo->intIfNum) != L7_SUCCESS)
        {
          /* The received data does not represent a valid interface.*/
          if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_SERVER)
          {
            L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
            osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                          "(%s)Failed to %s Option-82 for server reply as the "
                          "DHCP packet does not have valid circuit-id interface number(%d/%d/%d).", __FUNCTION__,
                          optionsRemove ? "remove":"get", relayAgentInfo->usp.unit, relayAgentInfo->usp.slot,
                          relayAgentInfo->usp.port);
            dsTraceWrite(traceMsg);
          }
          if (optionsRemove == L7_TRUE)
          {
            if (ptin_debug_dhcp_snooping)
              LOG_TRACE(LOG_CTX_PTIN_DHCP,"Failure here");
            return L7_FAILURE;
          }
          if (ptin_debug_dhcp_snooping)
            LOG_TRACE(LOG_CTX_PTIN_DHCP,"Success here");
          return L7_SUCCESS;
        }
        #endif
      }
      break;
    case DHCP_RELAY_AGENT_REMOTE_ID_SUBOPTION:
      {
        optLen = *(relayOffset + 1);
        relayOffset += DHCP_OPTION_CONTENT_OFFSET;

        /* Evaluate Opt Length */
        if ((relayOffset+optLen)>relayEnd)
        {
          if (ptin_debug_dhcp_snooping)
            LOG_TRACE(LOG_CTX_PTIN_DHCP,"optLen is too big");
          return L7_FAILURE;
        }

        relayAgentInfo->remoteIdFlag = L7_TRUE;
        osapiStrncpy(relayAgentInfo->remoteId, relayOffset, optLen);
        relayOffset += optLen;
      }
      break;
    default:
      {
        /* found some unexpected/undefined Option-82 sub-option.*/
        if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_SERVER)
        {
          L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
          osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                        "(%s)Failed to %s Option-82 for server reply as the "
                        "DHCP packet have an unexpected relay suboption.",
                        __FUNCTION__, optionsRemove ? "remove":"get");
          dsTraceWrite(traceMsg);
        }
        if (optionsRemove == L7_TRUE)
        {
          if (ptin_debug_dhcp_snooping)
            LOG_TRACE(LOG_CTX_PTIN_DHCP,"Failure here");
          return L7_FAILURE;
        }
        if (ptin_debug_dhcp_snooping)
          LOG_TRACE(LOG_CTX_PTIN_DHCP,"Success here");
        return L7_SUCCESS;
      }
    }
  }

  if (optionsRemove == L7_TRUE)
  {
    L7_uchar8 *tempBuff = dsInfo->pktBuff;
    memset(tempBuff, 0 , DS_DHCP_PACKET_SIZE_MAX);
    memcpy(tempBuff, relayEnd, (dhcpPacketLength - passOverLen));
    memcpy(dhcpOption, tempBuff, (dhcpPacketLength - passOverLen));
    memset(tempBuff, 0 , DS_DHCP_PACKET_SIZE_MAX);
    if (frameLen != L7_NULLPTR)
    {
      dsUdpCheckSumCalculate(frame, frameLen, L7_FALSE, (relayEnd - dhcpOption));
    }
  }
  if (ptin_debug_dhcp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_DHCP,"Success here");

  return L7_SUCCESS;
}
#endif
/*********************************************************************
* @purpose  Filter server messages received on untrusted ports.
*
* @param    intIfNum @b{(input)} receive interface
* @param    vlanId   @b{(input)} VLAN ID
* @param    frame    @b{(input)} ethernet frame
* @param    ipHeader @b{(input)} IP header inside frame
*
* @returns  L7_TRUE if frame is filtered
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL dsFilterServerMessage(L7_uint32 intIfNum, L7_ushort16 vlanId,
                              L7_uchar8 *frame, L7_ipHeader_t *ipHeader,
                              L7_ushort16 innerVlanId, L7_uint *client_idx)      /* PTin modified: DHCP snooping */
{
  L7_udp_header_t *udp_header;
  L7_dhcp_packet_t *dhcpPacket;
  L7_ushort16 ipHdrLen = dsIpHdrLen(ipHeader);
  #ifdef L7_DHCP_L2_RELAY_PACKAGE
  L7_ushort16 ipPktLen;
  L7_ushort16 dhcpPktLen;
  dsRelayAgentInfo_t relayAgentInfo;
  L7_BOOL relayFlag = L7_FALSE;
  L7_BOOL cIdFlag = L7_FALSE, rIdFlag = L7_FALSE;
  L7_uchar8 circuitIdStr[DS_MAX_REMOTE_ID_STRING];        /* PTin added: DHCP snooping */
  L7_uchar8 remoteIdStr[DS_MAX_REMOTE_ID_STRING];
  L7_uchar8 sysMacAddr[L7_MAC_ADDR_LEN];
  L7_RC_t   result = L7_SUCCESS;    /* PTin added: DHCP snooping */
  dhcpSnoopBinding_t dhcp_binding;

  ipPktLen = osapiNtohs(ipHeader->iph_len);
  dhcpPktLen = ipPktLen - ipHdrLen - sizeof(L7_udp_header_t);
  #endif

  udp_header = (L7_udp_header_t *)((L7_char8 *)ipHeader + ipHdrLen);
  dhcpPacket = (L7_dhcp_packet_t*)((L7_char8 *)udp_header + sizeof(L7_udp_header_t));

  if (dhcpPacket->op != L7_DHCP_BOOTP_REPLY)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_DHCP,"Not a L7_DHCP_BOOTP_REPLY");
    /* Packet not under interest of current function.*/
    return L7_FALSE;
  }
  /* Do the following binding database processing checks only when DHCP
     Snooping operational on that interface. */
  if (dsCfgData->dsGlobalAdminMode == L7_ENABLE &&
      dsVlanIntfIsSnooping(vlanId,intIfNum) /*dsIntfIsSnooping(intIfNum)*/ == L7_TRUE       /* PTin modified: DHCP snooping */
      /* && _dsVlanEnableGet(vlanId) == L7_TRUE*/)                                          /* PTin removed: DHCP snooping */
  {
    if (_dsVlanIsIntfRoot(vlanId,intIfNum) /*_dsIntfTrustGet(intIfNum)*/ == L7_FALSE)     /* PTin modified: DHCP snooping */
    {
      dsIntfInfo[intIfNum].dsIntfStats.untrustedSvrMsg++;
      if (_dsIntfLogInvalidGet(intIfNum))
      {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];

        static L7_uint32 lastMsg = 0;

        if (osapiUpTimeRaw() > lastMsg)
        {
          lastMsg = osapiUpTimeRaw();
          nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCP_SNOOPING_COMPONENT_ID,
                  "DHCP snooping dropping DHCP server message received on untrusted "
                  "interface %s on vlan %u."
                    " This message appears when DHCP Snooping untrusted port drops the"
                    " DHCP Server message.", ifName, vlanId);

          dsLogEthernetHeader((L7_enetHeader_t*)frame, DS_TRACE_LOG);
          dsLogIpHeader(ipHeader, DS_TRACE_LOG);
          dsLogDhcpPacket(dhcpPacket, DS_TRACE_LOG);
        }
      }
      if (ptin_debug_dhcp_snooping)
        LOG_ERR(LOG_CTX_PTIN_DHCP,"DHCP packet dropped here: DHCP snooping dropping DHCP server message received on untrusted interface");
      ptin_dhcp_stat_increment_field(intIfNum, vlanId, *client_idx, DHCP_STAT_FIELD_RX_SERVER_PKTS_ON_UNTRUSTED_INTF);
      return L7_TRUE;
    }
  }

  #ifdef L7_DHCP_L2_RELAY_PACKAGE
  /* If the reponse packets are destined to the switch itself (i.e those are
     responses for the DHCP client operating in the FASTPATH switch). Expecting
     Option-82 on Reply packets for our client does not make any sense. Hence
     Option-82 checks are relaxed for packets destined to FASTPATH DHCP client.*/
  simGetSystemIPBurnedInMac(sysMacAddr);
  if (memcmp(dhcpPacket->chaddr, sysMacAddr, L7_MAC_ADDR_LEN) == L7_NULL)
  {
    if (ptin_debug_dhcp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_DHCP,"Invalid MAC");
    return L7_FALSE;
  }

  /* Check if L2 Relay of Option-82 filters are needed.*/
  if (dsCfgData->dsL2RelayAdminMode == L7_ENABLE &&
      _dsVlanIntfL2RelayGet(vlanId,intIfNum) /*_dsIntfL2RelayGet(intIfNum)*/ == L7_TRUE)    /* PTin modified: DHCP snooping */
  {
    /* Get the Option-82 params,if present from the server reply. */
    relayFlag = L7_FALSE;
    memset(&relayAgentInfo, 0 , sizeof(dsRelayAgentInfo_t));

    /* PTin replaced: DHCP snooping */
    #if 1
    if ((result=dsRelayAgentInfoRemoveOrGet(frame, L7_NULLPTR, dhcpPacket, dhcpPktLen, L7_FALSE, &relayAgentInfo))==L7_SUCCESS)
    #else
    if (dsRelayAgentInfoRemoveOrGet(frame, L7_NULLPTR, dhcpPacket, dhcpPktLen,
                                    L7_FALSE, &relayAgentInfo) == L7_SUCCESS)
    #endif
    {
#if 1 /* PTin Added: Flexible circuit-id */
      L7_enetHeader_t *mac_header = 0;

      mac_header = (L7_enetHeader_t*) frame;

      memset(&dhcp_binding, 0, sizeof(dhcpSnoopBinding_t));
      memcpy(&dhcp_binding.key.macAddr, dhcpPacket->chaddr, L7_ENET_MAC_ADDR_LEN);
      dhcp_binding.key.ipType = L7_AF_INET;
      if (L7_SUCCESS != dsBindingFind(&dhcp_binding, L7_MATCH_EXACT))
      {
        if (ptin_debug_dhcp_snooping)
          LOG_WARNING(LOG_CTX_PTIN_DHCP, "DHCP Relay-Agent: Received server reply for a client (%02X:%02X:%02X:%02X:%02X:%02X) that is not in the binding table", 
                    dhcp_binding.key.macAddr[0], dhcp_binding.key.macAddr[1], dhcp_binding.key.macAddr[2], dhcp_binding.key.macAddr[3], dhcp_binding.key.macAddr[4], dhcp_binding.key.macAddr[5]);
        return L7_TRUE;
      }
#endif
      /* PTin added: DHCP snooping */
      #if 1
      /* Recalculate client index according the circuit id, if the packet came from root interfaces */
      if (_dsVlanIntfL2RelayTrustGet(vlanId,intIfNum)
          //&& relayAgentInfo.circuitIdFlag
          )
      {
        L7_uint client_index;
        ptin_client_id_t client;

        /* Client information */
        client.ptin_intf.intf_type = client.ptin_intf.intf_id = 0;
        client.outerVlan = dhcp_binding.vlanId;
        client.innerVlan = dhcp_binding.innerVlanId;
        client.mask  = PTIN_CLIENT_MASK_FIELD_INTF | PTIN_CLIENT_MASK_FIELD_OUTERVLAN;
        client.mask |= (dhcp_binding.innerVlanId != 0) ? PTIN_CLIENT_MASK_FIELD_INNERVLAN : 0;

        #if (PTIN_BOARD_IS_GPON)
        if (dhcp_binding.innerVlanId!=0)
        #else
        if (1)
        #endif
        {
          /* Find client index, and validate it */
        #if 1 /* PTin modified: flexible circuit-id */
          if (ptin_dhcp_clientIndex_get(dhcp_binding.intIfNum, vlanId, &client, &client_index)==L7_SUCCESS &&
        #else
          if (ptin_dhcp_clientIndex_get(relayAgentInfo.intIfNum, vlanId, &client, &client_index)==L7_SUCCESS &&
        #endif
              client_index<PTIN_SYSTEM_MAXCLIENTS_PER_DHCP_INSTANCE)
          {
            *client_idx = client_index;   /* Update to new client index */
            if (ptin_debug_dhcp_snooping)
              LOG_TRACE(LOG_CTX_PTIN_DHCP,"Client Id redefined to %u",*client_idx);
          }
        }
      }
      #endif

      if (relayAgentInfo.circuitIdFlag == L7_TRUE ||
          relayAgentInfo.remoteIdFlag == L7_TRUE)
      {
        if (ptin_debug_dhcp_snooping)
          LOG_TRACE(LOG_CTX_PTIN_DHCP,"Relay Flag is 1");
        relayFlag = L7_TRUE;
      }
    }

    if ((_dsVlanIntfL2RelayTrustGet(vlanId,intIfNum) /*_dsIntfL2RelayTrustGet(intIfNum)*/ == L7_TRUE)   /* PTin modified: DHCP snooping */
         && (relayFlag == L7_FALSE))
    {
      /* The packet is put for filtering as the packet might not have Option-82,
         which is expected on L2 Relay trusted interfaces. So drop the packet.*/
      dsIntfInfo[intIfNum].dsIntfStats.trustedServerFramesWithoutOption82++;
      ptin_dhcp_stat_increment_field(intIfNum, vlanId, *client_idx, DHCP_STAT_FIELD_RX_SERVER_REPLIES_WITHOUT_OPTIONS);
#if 0 /* PTin removed: DHCPv6 */
      ptin_dhcp_stat_increment_field(intIfNum, vlanId, *client_idx, DHCP_STAT_FIELD_RX_SERVER_REPLIES_WITHOUT_OPTIONS);
#endif
//    DHCP_L2RELAY_LOG("DHCP L2 Relay dropping server msg without Option-82 rx'ed on L2Relay trusted",
//                       intIfNum, vlanId, (L7_enetHeader_t *)frame, ipHeader, dhcpPacket, DS_TRACE_LOG);
#if 0 /* PTin removed: flexible circuit-id */
      if (ptin_debug_dhcp_snooping)
        LOG_ERR(LOG_CTX_PTIN_DHCP,"DHCP packet dropped here: DHCP L2 Relay dropping server msg without Option-82 rx'ed on L2Relay trusted");
      return L7_TRUE;
#else
      return L7_FALSE;
#endif
    }
    else if ((_dsVlanIntfL2RelayTrustGet(vlanId,intIfNum) /*_dsIntfL2RelayTrustGet(intIfNum)*/ == L7_FALSE)   /* PTin modified: DHCP snooping */
             && (relayFlag == L7_TRUE))
    {
      /* The packet is put for filtering as the packet have Option-82,
         which is not expected on L2 Relay untrusted interfaces. So drop the packet.*/
      dsIntfInfo[intIfNum].dsIntfStats.untrustedServerFramesWithOption82++;
      ptin_dhcp_stat_increment_field(intIfNum, vlanId, *client_idx, DHCP_STAT_FIELD_RX_SERVER_PKTS_ON_UNTRUSTED_INTF);
      DHCP_L2RELAY_LOG("DHCP L2 Relay dropping server msg with Option-82 rx'ed on L2Relay untrusted",
                         intIfNum, vlanId, (L7_enetHeader_t *)frame, ipHeader, dhcpPacket, DS_TRACE_LOG);
      if (ptin_debug_dhcp_snooping)
        LOG_ERR(LOG_CTX_PTIN_DHCP,"DHCP packet dropped here: DHCP L2 Relay dropping server msg with Option-82 rx'ed on L2Relay untrusted");
      return L7_TRUE;
    }
    /* PTin replaced: DHCP snooping */
    #if 1
      if (result!=L7_SUCCESS)
      {
        if (ptin_debug_dhcp_snooping)
          LOG_ERR(LOG_CTX_PTIN_DHCP,"DHCP packet dropped here");
        return L7_TRUE;
      }

      /* Validate destination slot (only for TOLT8G) */
#if 0 /* PTin removed: flexible circuit-id */
      #if ( PTIN_BOARD_IS_LINECARD )
      L7_uint8  board_slot;             /* PTin added: DHCP snooping */
      /* For trusted interfaces, check if destination board_slot matches the current one.
         If not, discard packet */
      if (_dsVlanIntfL2RelayTrustGet(vlanId,intIfNum))
      {
        if (ptin_intf_slot_get(&board_slot)!=L7_SUCCESS || board_slot!=relayAgentInfo.board_slot)
        {
          if (ptin_debug_dhcp_snooping)
            LOG_ERR(LOG_CTX_PTIN_DHCP,"Destination board (%u) does not match this one (%u)... discarding packet!",
                    relayAgentInfo.board_slot, board_slot);
          return L7_TRUE;
        }
        if (ptin_debug_dhcp_snooping)
          LOG_TRACE(LOG_CTX_PTIN_DHCP,"Destination board matches this one (%u)... yeah!",board_slot);
      }
      #endif
#endif
    #else
    else
    #endif
    {
      dsInfo->debugStats.serverOption82Rx++;
      ptin_dhcp_stat_increment_field( intIfNum, vlanId, *client_idx,
                                      DHCP_STAT_FIELD_RX_SERVER_REPLIES_WITH_OPTION82);
      dsL2RelayRelayAgentInfoOptionCfgGet(dhcp_binding.intIfNum,
                                          vlanId, dhcp_binding.innerVlanId, &frame[L7_MAC_ADDR_LEN],
                                          &cIdFlag, circuitIdStr,                                           /* PTin modified: DHCP snooping */
                                          &rIdFlag, remoteIdStr );
      if ((_dsVlanIntfL2RelayTrustGet(vlanId,intIfNum) /*_dsIntfL2RelayTrustGet(intIfNum)*/ == L7_TRUE))    /* PTin modified: DHCP snooping */
      {
#if 0 /* PTIN Removed: Optional DHCP Op.82 */
        if (cIdFlag == L7_TRUE && relayAgentInfo.circuitIdFlag != L7_TRUE)
        {
          /* Filter the packet as the expected Circuit Id is not seen in the
             server reply.*/
          DHCP_L2RELAY_LOG("DHCP L2 Relay dropping server msg without expected"
                           "circuit-id rx'ed on trusted", intIfNum, vlanId,
                           (L7_enetHeader_t *)frame, ipHeader, dhcpPacket,
                           DS_TRACE_LOG);
          if (ptin_debug_dhcp_snooping)
            LOG_ERR(LOG_CTX_PTIN_DHCP,"DHCP packet dropped here: DHCP L2 Relay dropping server msg without expected circuit-id rx'ed on trusted");
          return L7_TRUE;
        }
        if (rIdFlag == L7_TRUE && relayAgentInfo.remoteIdFlag != L7_TRUE)
        {
          /* Filter the packet as the expected Remote Id is not seen in the
             server reply.*/
          DHCP_L2RELAY_LOG("DHCP L2 Relay dropping server msg without expected"
                           "remote-id rx'ed on trusted", intIfNum, vlanId,
                           (L7_enetHeader_t *)frame, ipHeader, dhcpPacket,
                           DS_TRACE_LOG);
          if (ptin_debug_dhcp_snooping)
            LOG_ERR(LOG_CTX_PTIN_DHCP,"DHCP packet dropped here: DHCP L2 Relay dropping server msg without expected remote-id rx'ed on trusted");
          return L7_TRUE;
        }
#endif
      }
      if (ptin_debug_dhcp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_DHCP,"No discard");
      return L7_FALSE;
    }
  }
  #endif

  if (ptin_debug_dhcp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_DHCP,"No discard");

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Filter DHCPRELEASE or DHCPDECLINE message if received on
*           interface other than that specified in bindings DB for this
*           client.
*
* @param    intIfNum @b{(input)} receive interface
* @param    vlanId   @b{(input)} VLAN ID
* @param    frame    @b{(input)} ethernet frame
* @param    ipHeader @b{(input)} IP header inside frame
*
* @returns  L7_TRUE if frame is filtered
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL dsFilterClientMessage(L7_uint32 intIfNum, L7_ushort16 vlanId,
                              L7_uchar8 *frame, L7_ipHeader_t *ipHeader,
                              L7_ushort16 innerVlanId, L7_uint *client_idx)      /* PTin modified: DHCP snooping */
{
  L7_udp_header_t *udp_header;
  L7_dhcp_packet_t *dhcpPacket;
  L7_dhcp_pkt_type_t dhcpPktType;
  L7_uint32 ipPktLen;
  L7_uint32 dhcpPktLen;
  L7_ushort16 ipHdrLen;
  dhcpSnoopBinding_t dsBinding;
  L7_enetMacAddr_t *srcMacAddr;
#ifdef L7_DHCP_L2_RELAY_PACKAGE
  dsRelayAgentInfo_t relayAgentInfo;
  L7_BOOL relayFlag = L7_FALSE;
#endif
  L7_RC_t result = L7_SUCCESS;    /* PTin added: DHCP snooping */

  ipHdrLen = dsIpHdrLen(ipHeader);
  ipPktLen = osapiNtohs(ipHeader->iph_len);
  udp_header = (L7_udp_header_t*)((L7_char8 *)ipHeader + ipHdrLen);
  dhcpPacket = (L7_dhcp_packet_t*)((L7_char8 *)udp_header + sizeof(L7_udp_header_t));
  dhcpPktLen = ipPktLen - ipHdrLen - sizeof(L7_udp_header_t);

  if (dhcpPacket->op != L7_DHCP_BOOTP_REQUEST)
  {
    /* Packet not under interest of current function.*/
    return L7_FALSE;
  }

#ifdef L7_DHCP_L2_RELAY_PACKAGE
  if (dsCfgData->dsL2RelayAdminMode == L7_ENABLE &&
      _dsVlanIntfL2RelayGet(vlanId,intIfNum) /*_dsIntfL2RelayGet(intIfNum)*/ == L7_TRUE)    /* PTin modified: DHCP snooping */
  {
    /* Filter out any packets with non-zero giaddr representing L3 full
       Relay agent. (Ref. R-103 from document TR-101.)*/
    if (dhcpPacket->giaddr != L7_NULL)
    {
      DHCP_L2RELAY_LOG("DHCP L2 Relay dropping client msg with non-zero giadddr",
                        intIfNum, vlanId, (L7_enetHeader_t *)frame, ipHeader, dhcpPacket,
                          DS_TRACE_LOG);
      return L7_TRUE;
    }

    /* Extract the DHCP Option-82 if exists in the packet.*/
    relayFlag = L7_FALSE;
    memset(&relayAgentInfo, 0 , sizeof(dsRelayAgentInfo_t));
    /* PTin replaced: DHCP snooping */
    #if 1
    if ((result=dsRelayAgentInfoRemoveOrGet(frame, L7_NULLPTR, dhcpPacket, dhcpPktLen, L7_FALSE, &relayAgentInfo))==L7_SUCCESS)
    #else
    if (dsRelayAgentInfoRemoveOrGet(frame, L7_NULLPTR, dhcpPacket, dhcpPktLen,
                                    L7_FALSE, &relayAgentInfo) == L7_SUCCESS)
    #endif
    {
      if (relayAgentInfo.circuitIdFlag == L7_TRUE ||
          relayAgentInfo.remoteIdFlag == L7_TRUE)
      {
        if (ptin_debug_dhcp_snooping)
          LOG_ERR(LOG_CTX_PTIN_DHCP,"Relay Flag is 1");
        relayFlag = L7_TRUE;
      }
    }

    if (relayFlag == L7_TRUE)
    {
      dsInfo->debugStats.clientOption82Rx++;
      if (ptin_debug_dhcp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_DHCP,"Incremented DHCP_STAT_FIELD_RX_CLIENT_REQUESTS_WITH_OPTION82");
#if 0 /* PTin removed: DHCPv6 */
      ptin_dhcp_stat_increment_field(intIfNum, vlanId, *client_idx, DHCP_STAT_FIELD_RX_CLIENT_REQUESTS_WITH_OPTION82);
#endif
    }
    else
    {
      if (ptin_debug_dhcp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_DHCP,"Incremented DHCP_STAT_FIELD_RX_CLIENT_REQUESTS_WITHOUT_OPTION82");
      ptin_dhcp_stat_increment_field(intIfNum, vlanId, *client_idx, DHCP_STAT_FIELD_RX_CLIENT_REQUESTS_WITHOUT_OPTIONS);
    }

    /* If giaadr is 0, we MUST drop packets with the relay option recv on untrusted interfaces. */
    if(dhcpPacket->giaddr == 0)
    {
       if ((relayFlag == L7_TRUE) && (_dsVlanIntfL2RelayTrustGet(vlanId,intIfNum) == L7_FALSE))   /* PTin modified: DHCP snooping */
       {
         dsIntfInfo[intIfNum].dsIntfStats.untrustedClientFramesWithOption82++;
         ptin_dhcp_stat_increment_field(intIfNum, vlanId, *client_idx, DHCP_STAT_FIELD_RX_CLIENT_PKTS_WITHOPS_ON_UNTRUSTED_INTF);
         DHCP_L2RELAY_LOG("DHCP L2 Relay dropping client msg with Option-82 and giaddr==0 "
                            "rx'ed on L2 Relay untrusted", intIfNum, vlanId,
                            (L7_enetHeader_t *)frame, ipHeader, dhcpPacket,
                             DS_TRACE_LOG);
         if (ptin_debug_dhcp_snooping)
           LOG_ERR(LOG_CTX_PTIN_DHCP,"DHCP packet dropped here: DHCP L2 Relay dropping client msg with Option-82 and giaddr==0 rx'ed on L2 Relay untrusted");
         return L7_TRUE;
       }
    }

    /* PTin added: DHCP snooping */
    #if 1
    if (result!=L7_SUCCESS)
    {
      if (ptin_debug_dhcp_snooping)
        LOG_ERR(LOG_CTX_PTIN_DHCP,"DHCP packet dropped here");
      return L7_TRUE;
    }
    #endif

    /* The vlanId and innerVlanId are already translated values.*/
    if (dsL2RelayServiceIsEnabled(intIfNum, vlanId, innerVlanId) != L7_TRUE)
    {
      DHCP_L2RELAY_LOG("DHCP L2 Relay dropping client msg whose SVLAN "
                         "does not have L2 Relay enabled",
                         intIfNum, vlanId, (L7_enetHeader_t *) frame,
                         ipHeader, dhcpPacket, DS_TRACE_CONSOLE);
      if (ptin_debug_dhcp_snooping)
        LOG_ERR(LOG_CTX_PTIN_DHCP,"DHCP packet dropped here: DHCP L2 Relay dropping client msg whose SVLAN does not have L2 Relay enabled");
      return L7_TRUE;
    }
  }
#endif

  /* Do the following binding database processing checks only when DHCP
     Snooping operational on that interface. */
  if (dsCfgData->dsGlobalAdminMode == L7_ENABLE &&
      dsVlanIntfIsSnooping(vlanId,intIfNum) /*dsIntfIsSnooping(intIfNum)*/ == L7_TRUE         /* PTin modified: DHCP snooping */
     /* && _dsVlanEnableGet(vlanId) == L7_TRUE*/ )                                            /* PTin removed: DHCP snooping */

  {
    if (_dsVlanIsIntfRoot(vlanId,intIfNum) /*_dsIntfTrustGet(intIfNum)*/ == L7_TRUE)        /* PTin modified: DHCP snooping */
    {
      /* If the interface is trusted Snooping interface, then the database
         related validations are not needed as the binding database is updated
         only for untrusted interfaces. */
      return L7_FALSE;
    }
    dhcpPktType = dsPacketType(dhcpPacket, dhcpPktLen);
    if ((dhcpPktType == L7_DHCP_RELEASE) || (dhcpPktType == L7_DHCP_DECLINE))
    {
      /* Verify that client still on same interface as its binding */
      srcMacAddr = (L7_enetMacAddr_t*) (frame + L7_ENET_MAC_ADDR_LEN);
      if (!dsCfgData->dsVerifyMac)
      {
        memcpy(dsBinding.key.macAddr, dhcpPacket->chaddr, L7_ENET_MAC_ADDR_LEN);
      }
      else
      {
        memcpy(dsBinding.key.macAddr, srcMacAddr->addr, L7_ENET_MAC_ADDR_LEN);
      }
      dsBinding.key.ipType = L7_AF_INET;

      if (dsBindingFind(&dsBinding, L7_MATCH_EXACT) != L7_SUCCESS)
      {
        dsInfo->debugStats.releaseDeclineUnkClient++;

        if (_dsIntfLogInvalidGet(intIfNum))
        {
          L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
          L7_uchar8 srcMacStr[DS_MAC_STR_LEN + 1];
          static L7_uint32 lastMsg = 0;
          if (osapiUpTimeRaw() > lastMsg)
          {
            lastMsg = osapiUpTimeRaw();
            nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
            dsMacToString(srcMacAddr->addr, srcMacStr);
            L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCP_SNOOPING_COMPONENT_ID,
                    "DHCP snooping dropping %s received on "
                    "interface %s, vlan %u, from %s. Station not in bindings table.",
                      " This message appears when DHCP Snooping untrusted port  drops "
                      "DHCP Decline or DHCP release messages for a non-existing entry "
                      "in SNOOPING table.",
                    dhcpMsgTypeNames[dhcpPktType], ifName, vlanId, srcMacStr);
            dsLogEthernetHeader((L7_enetHeader_t*) frame, DS_TRACE_LOG);
            dsLogIpHeader(ipHeader, DS_TRACE_LOG);
            dsLogDhcpPacket(dhcpPacket, DS_TRACE_LOG);
          }
        }
        if (ptin_debug_dhcp_snooping)
          LOG_ERR(LOG_CTX_PTIN_DHCP,"DHCP packet dropped here: Client should have a binding before sending a RELEASE or DECLINE. Drop msg.");
        /* Client should have a binding before sending a RELEASE or DECLINE. Drop msg. */
        return L7_TRUE;
      }
      if ((dsBinding.intIfNum != intIfNum) || (dsBinding.vlanId != vlanId))
      {

       if (!dsCfgData->dsVerifyMac) /* to handle the relay agent case */
       {
         return L7_FALSE;
       }

        dsIntfInfo[intIfNum].dsIntfStats.intfMismatch++;

        if (_dsIntfLogInvalidGet(intIfNum))
        {
          L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
          L7_uchar8 bindIfName[L7_NIM_IFNAME_SIZE + 1];
          L7_uchar8 srcMacStr[DS_MAC_STR_LEN + 1];
          static L7_uint32 lastMsg = 0;

          if (osapiUpTimeRaw() > lastMsg)
          {
            lastMsg = osapiUpTimeRaw();
            nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
            nimGetIntfName(dsBinding.intIfNum, L7_SYSNAME, bindIfName);
            dsMacToString(srcMacAddr->addr, srcMacStr);
            L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCP_SNOOPING_COMPONENT_ID,
                    "DHCP snooping dropping %s received on "
                    "interface %s, vlan %u, from client %s.",
                    dhcpMsgTypeNames[dhcpPktType], ifName, vlanId, srcMacStr);
            L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCP_SNOOPING_COMPONENT_ID,
                    "Client previously bound to interface %s, vlan %u."
           " This message appears when DHCP Snooping untrusted"
                    " message drops the DHCP Decline or Release messages ",
                    bindIfName, dsBinding.vlanId);
            dsLogEthernetHeader((L7_enetHeader_t*) frame, DS_TRACE_LOG);
            dsLogIpHeader(ipHeader, DS_TRACE_LOG);
            dsLogDhcpPacket(dhcpPacket, DS_TRACE_LOG);
          }
        }
        if (ptin_debug_dhcp_snooping)
          LOG_ERR(LOG_CTX_PTIN_DHCP,"DHCP packet dropped here");
        return L7_TRUE;    /* drop msg */
      }
    }

  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Filter client messages whose source MAC address does not match
*           the client hardware address in the DHCP packet.
*
* @param    intIfNum @b{(input)} receive interface
* @param    vlanId   @b{(input)} VLAN ID
* @param    frame    @b{(input)} ethernet frame
* @param    ipHeader @b{(input)} IP header inside frame
*
* @returns  L7_TRUE if frame is filtered
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL dsFilterVerifyMac(L7_uint32 intIfNum, L7_ushort16 vlanId,
                          L7_uchar8 *frame, L7_ipHeader_t *ipHeader)
{
  L7_udp_header_t *udp_header;
  L7_dhcp_packet_t *dhcpPacket;
  L7_enetMacAddr_t* srcMacAddr;
  L7_ushort16 ipHdrLen = dsIpHdrLen(ipHeader);

  /* Only verify MAC if configured to do so. */
  if (!dsCfgData->dsVerifyMac)
    return L7_FALSE;

  udp_header = (L7_udp_header_t*)((L7_char8 *)ipHeader + ipHdrLen);
  dhcpPacket = (L7_dhcp_packet_t*)((L7_char8 *)udp_header + sizeof(L7_udp_header_t));

  if ((dhcpPacket->op == L7_DHCP_BOOTP_REQUEST) &&
      (_dsVlanIsIntfRoot(vlanId,intIfNum) /*_dsIntfTrustGet(intIfNum)*/ == L7_FALSE))     /* PTin modified: DHCP snooping */
  {
    /* client message on untrusted interface */
    srcMacAddr = (L7_enetMacAddr_t*) (frame + L7_ENET_MAC_ADDR_LEN);
    if (memcmp(srcMacAddr->addr, dhcpPacket->chaddr, L7_ENET_MAC_ADDR_LEN) != 0)
    {
      dsIntfInfo[intIfNum].dsIntfStats.macVerify++;
      if (_dsIntfLogInvalidGet(intIfNum))
      {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        L7_uchar8 srcMacStr[DS_MAC_STR_LEN + 1];
        L7_uchar8 clientHwStr[DS_MAC_STR_LEN + 1];
        static L7_uint32 lastMsg = 0;
        if (osapiUpTimeRaw() > lastMsg)
        {
          lastMsg = osapiUpTimeRaw();
          dsMacToString(srcMacAddr->addr, srcMacStr);
          dsMacToString(dhcpPacket->chaddr, clientHwStr);
          nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCP_SNOOPING_COMPONENT_ID,
                  "DHCP snooping dropping client message received on interface %s, vlan %u.",
                  ifName, vlanId);
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCP_SNOOPING_COMPONENT_ID,
                  "Source MAC address %s does not match client hardware address %s."
                  " This message appears when DHCP Snooping drops the source mac "
                  "address doest not match the clinet Hardware address.",
                  srcMacStr, clientHwStr);
          dsLogEthernetHeader((L7_enetHeader_t*) frame, DS_TRACE_LOG);
          dsLogIpHeader(ipHeader, DS_TRACE_LOG);
          dsLogDhcpPacket(dhcpPacket, DS_TRACE_LOG);
        }
      }
      return L7_TRUE;
    }
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Extract binding info from a DHCP server packet.
*
* @param    intIfNum   @b{(input)} receive interface
* @param    vlanId     @b{(input)} VLAN ID
* @param    dhcpPacket @b{(input)} DHCP packet
* @param    pktLen     @b{(input)} DHCP packet length (bytes)
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    Use DHCP DISCOVER and REQUEST messages to learn the port where the
*           client is attached. Use DHCP ACK messages to learn
*           the MAC to IP binding. A DECLINE, RELEASE, or NAK removes binding.
*
* @end
*********************************************************************/
L7_RC_t dsBindingExtract(L7_uint32 intIfNum, L7_ushort16 vlanId, L7_ushort16 innerVlanId, /* PTin modified: DHCP */
                         L7_dhcp_packet_t *dhcpPacket, L7_uint32 pktLen)
{
  L7_dhcp_pkt_type_t dhcpPktType;
  L7_uint32 yiaddr = 0;     /* client's IP address */
  L7_enetMacAddr_t chaddr;
  L7_uint32 leaseTime;
  dhcpSnoopBinding_t dsBinding;

  memcpy(&chaddr, dhcpPacket->chaddr, L7_ENET_MAC_ADDR_LEN);

  /* If a static entry exists with DHCP message mac address
     don't process it. Log the message. */

  memcpy(dsBinding.key.macAddr, chaddr.addr, L7_ENET_MAC_ADDR_LEN);
  dsBinding.key.ipType = L7_AF_INET;
  if (dsBindingFind(&dsBinding, L7_MATCH_EXACT) == L7_SUCCESS)
  {
     if ( dsBinding.bindingType == DS_BINDING_STATIC)
     {
       return L7_REQUEST_DENIED;
     }
  }

  dhcpPktType = dsPacketType(dhcpPacket, pktLen);
  switch (dhcpPktType)
  {
    case L7_DHCP_DISCOVER:
      if (ptin_debug_dhcp_snooping)
        LOG_DEBUG(LOG_CTX_PTIN_DHCP,"Processing DISCOVER from intIfNum=%u, oVlan=%u, iVlan=%u, MAC=%02x:%02x:%02x:%02x:%02x:%02x",
                  intIfNum, vlanId, innerVlanId,
                  dsBinding.key.macAddr[0],dsBinding.key.macAddr[1],dsBinding.key.macAddr[2],dsBinding.key.macAddr[3],dsBinding.key.macAddr[4],dsBinding.key.macAddr[5]);

      /* learn client port. Can only learn client port from a client message.
       * We look at DISCOVER so we have a binding if client
       * sends DECLINE. */
      if (dsBindingAdd(DS_BINDING_TENTATIVE, &chaddr, yiaddr, vlanId, innerVlanId /*PTin modified: DHCP */,
                       intIfNum) != L7_SUCCESS)
      {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        L7_uchar8 clientHwStr[DS_MAC_STR_LEN + 1];
        static L7_uint32 lastMsg = 0;

        if (ptin_debug_dhcp_snooping)
          LOG_ERR(LOG_CTX_PTIN_DHCP, "Unable to add new entry to the binding table [chaddr:%02X:%02X:%02X:%02X:%02X:%02X yiaddr:%08X, vlanId:%u innerVlanId:%u intIfNum:%u]", 
                  dsBinding.key.macAddr[0], dsBinding.key.macAddr[1], dsBinding.key.macAddr[2], dsBinding.key.macAddr[3], dsBinding.key.macAddr[4], dsBinding.key.macAddr[5], 
                  yiaddr, vlanId, innerVlanId, intIfNum);

        dsInfo->debugStats.bindingAddFail++;
        if (osapiUpTimeRaw() > lastMsg)
        {
          lastMsg = osapiUpTimeRaw();
          dsMacToString(chaddr.addr, clientHwStr);
          nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
                  "Failed to add binding {%s, %s, vlan %u} from DISCOVER.",
                  clientHwStr, ifName, vlanId);
        }
        return L7_FAILURE;
      }
      if (ptin_debug_dhcp_snooping)
        LOG_DEBUG(LOG_CTX_PTIN_DHCP, "Successfully added new entry to the binding table [chaddr:%02X:%02X:%02X:%02X:%02X:%02X yiaddr:%08X, vlanId:%u innerVlanId:%u intIfNum:%u]",
                  dsBinding.key.macAddr[0], dsBinding.key.macAddr[1], dsBinding.key.macAddr[2], dsBinding.key.macAddr[3], dsBinding.key.macAddr[4], dsBinding.key.macAddr[5], 
                  yiaddr, vlanId, innerVlanId, intIfNum);

      break;

    case L7_DHCP_REQUEST:
      if (ptin_debug_dhcp_snooping)
        LOG_DEBUG(LOG_CTX_PTIN_DHCP,"Processing REQUEST from intIfNum=%u, oVlan=%u, iVlan=%u, MAC=%02x:%02x:%02x:%02x:%02x:%02x",
                  intIfNum, vlanId, innerVlanId,
                  dsBinding.key.macAddr[0],dsBinding.key.macAddr[1],dsBinding.key.macAddr[2],dsBinding.key.macAddr[3],dsBinding.key.macAddr[4],dsBinding.key.macAddr[5]);

      /* If client is renewing existing binding, leave it. But allow client to
       * move to a new VLAN and port. */
      memcpy(dsBinding.key.macAddr, chaddr.addr, L7_ENET_MAC_ADDR_LEN);
      dsBinding.key.ipType = L7_AF_INET;
      if (dsBindingFind(&dsBinding, L7_MATCH_EXACT) == L7_SUCCESS)
      {
        if ((dsBinding.vlanId != vlanId) || (dsBinding.intIfNum != intIfNum))
        {
          /* keep IP address */
          dsBindingAdd(DS_BINDING_TENTATIVE, &chaddr, dsBinding.ipAddr, vlanId, innerVlanId /*PTin modified: DHCP */, intIfNum);
        }
        else
        {
        }
      }
      else
      {
        /* Create new binding from REQUEST */
        if (dsBindingAdd(DS_BINDING_TENTATIVE, &chaddr, yiaddr, vlanId, innerVlanId /*PTin modified: DHCP */,
                         intIfNum) != L7_SUCCESS)
        {
          L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
          L7_uchar8 clientHwStr[DS_MAC_STR_LEN + 1];
          static L7_uint32 lastMsg = 0;

          dsInfo->debugStats.bindingAddFail++;
          if (osapiUpTimeRaw() > lastMsg)
          {
            lastMsg = osapiUpTimeRaw();
            dsMacToString(chaddr.addr, clientHwStr);
            nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
            L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
                    "Failed to add binding {%s, %s, vlan %u} from REQUEST.",
                    clientHwStr, ifName, vlanId);
          }
          return L7_FAILURE;
        }
      }
      break;

    case L7_DHCP_ACK:
    {
         if (ptin_debug_dhcp_snooping)
           LOG_DEBUG(LOG_CTX_PTIN_DHCP,"Processing ACK from intIfNum=%u, oVlan=%u, iVlan=%u, MAC=%02x:%02x:%02x:%02x:%02x:%02x",
                     intIfNum, vlanId, innerVlanId,
                     dsBinding.key.macAddr[0],dsBinding.key.macAddr[1],dsBinding.key.macAddr[2],dsBinding.key.macAddr[3],dsBinding.key.macAddr[4],dsBinding.key.macAddr[5]);

         /* Setting the IP addr on the binding will fail if client is on a
          * trusted port or a port not enabled for DHCP snooping. This is ok. */
         /* memcpy to avoid data-alignment problems */
         memcpy(&yiaddr, &(dhcpPacket->yiaddr), sizeof(yiaddr));
         yiaddr = osapiNtohl(yiaddr);
         if ( yiaddr == 0)
         {
           /* This is the ACK for the dhcp_offer,IGNORE it */
           break;
         }
         if (dsBindingIpAddrSet(&chaddr, yiaddr) == L7_SUCCESS)
         {
           dsInfo->debugStats.bindingsAdded++;
         }
         /* Extract lease time */
         dsBindingTreeKey_t key;
         memset(&key, 0x00, sizeof(key));
         memcpy(&key.macAddr.addr, &chaddr.addr, L7_ENET_MAC_ADDR_LEN);
         key.ipType = L7_AF_INET;
         leaseTime = dsLeaseTimeGet(dhcpPacket, pktLen);
         dsBindingLeaseSet(&key, leaseTime);
         break;
    }
    case L7_DHCP_DECLINE:
    case L7_DHCP_RELEASE:
    case L7_DHCP_NACK:
    {
         if (ptin_debug_dhcp_snooping)
           LOG_DEBUG(LOG_CTX_PTIN_DHCP,"Processing DECLINE / RELEASE / NACK from intIfNum=%u, oVlan=%u, iVlan=%u, MAC=%02x:%02x:%02x:%02x:%02x:%02x",
                     intIfNum, vlanId, innerVlanId,
                     dsBinding.key.macAddr[0],dsBinding.key.macAddr[1],dsBinding.key.macAddr[2],dsBinding.key.macAddr[3],dsBinding.key.macAddr[4],dsBinding.key.macAddr[5]);

         /* Client has refused or given up lease, or server has refused. Remove binding.
          * A failure removing binding could be ok. The client might be on a trusted
          * port or a port not enabled for DHCP snooping. */
         dsBindingTreeKey_t key;
         memset(&key, 0x00, sizeof(key));
         memcpy(&key.macAddr.addr, &chaddr.addr, L7_ENET_MAC_ADDR_LEN);
         key.ipType = L7_AF_INET;
         if (dsBindingRemove(&key) == L7_SUCCESS)
         {
           dsInfo->debugStats.bindingsRemoved++;
         }
         break;
    }
    default:
      if (ptin_debug_dhcp_snooping)
        LOG_DEBUG(LOG_CTX_PTIN_DHCP,"Ignoring OFFER from intIfNum=%u, oVlan=%u, iVlan=%u, MAC=%02x:%02x:%02x:%02x:%02x:%02x",
                  intIfNum, vlanId, innerVlanId,
                  dsBinding.key.macAddr[0],dsBinding.key.macAddr[1],dsBinding.key.macAddr[2],dsBinding.key.macAddr[3],dsBinding.key.macAddr[4],dsBinding.key.macAddr[5]);
      /* just ignore OFFER messages */
      break;
  }

  //Update the lease status of the binding table entry. Ignore the return code because the entry might not exist if a release was received.
  dsv4LeaseStatusUpdate(&chaddr, dhcpPktType);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Forward a valid DHCP client packet.
*
* @param    intIfNum     @b{(input)} receive interface
* @param    vlanId       @b{(input)} VLAN ID
* @param    frame        @b{(input)} ethernet frame
* @param    frameLen     @b{(input)} ethernet frame length,
*                                    incl eth header (bytes)
* @param    intfIfNumFwd @b{(input)} Iface on which server reply needs to
*                                    be forwarded based on Option-82.
*                                    Applicable only for DHCP replies only.
* @param    sVlanId      @b{(input)} Service VLAN on which client requests
*                                    need to be forwarded if L2 relay is enabled.
*                                    Applicable only for DHCP requests only.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    For DHCP snooping, all client  and server messages
*           are forwarded here (both for trusted and untrusted).
*           The Option-82 is already added or removed based on
*           L2 relay configuration by the caller itself.
*           But if L2 relay is enabled, the server messages are
*           forwarded only to the client based on option-82 content
*           i.e. which is here specified in 'intfIfNumFwd'. Even the
*           ports for which the client messages need to be forwarded
*           is specified by 'sVlanId' if L2 Relay is operational and
*           'vlanId' if only DHCP Snooping is operational.
*
*
* @end
*********************************************************************/
L7_RC_t dsFrameForward(L7_uint32 intIfNum, L7_ushort16 vlanId,
                       L7_uchar8 *frame, L7_ushort16 frameLen,
                       L7_ushort16 innerVlanId, L7_uint client_idx,     /* PTin modified: DHCP snooping */
                       L7_uint32 relayOptIntIfNum)
{
  L7_udp_header_t *udp_header;
  L7_dhcp_packet_t *dhcpPacket;
  L7_ushort16 ethHdrLen = sysNetDataOffsetGet(frame);
  L7_ipHeader_t *ipHeader = (L7_ipHeader_t*)(frame + ethHdrLen);
  L7_ushort16 ipHdrLen = dsIpHdrLen(ipHeader);
  L7_BOOL requestFlag = L7_FALSE;
  L7_uint8 ethPrty, *frameEthPrty;

  udp_header = (L7_udp_header_t *)((L7_char8 *)ipHeader + ipHdrLen);
  dhcpPacket = (L7_dhcp_packet_t*)((L7_char8 *)udp_header + sizeof(L7_udp_header_t));

  if (dsCfgData->dsTraceFlags & DS_TRACE_FRAME_TX)
  {
    L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
    osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                  "(%s)Frame forward inputs for DHCP %s are: intIfNum(%d), vlanId(%d), innerVlanId(%d)"
                  "relayOptIntIfNum(%d), frameLen(%d) ", __FUNCTION__,
                  (dhcpPacket->op == L7_DHCP_BOOTP_REQUEST) ? "request":"reply",
                  intIfNum, vlanId, innerVlanId, relayOptIntIfNum, frameLen);
    dsTraceWrite(traceMsg);
  }
  /* If server reply with L2relay enabled,forward only to client
     interface from Option-82 sub-option Circuit-Id*/
  if (dhcpPacket->op == L7_DHCP_BOOTP_REPLY)
  {
    requestFlag = L7_FALSE;
    if (relayOptIntIfNum != L7_NULL)
    {
       //Change ethernet priority bit
      if (ptin_dhcp_ethPrty_get(vlanId, &ethPrty) != L7_SUCCESS)
      {
         if (ptin_debug_dhcp_snooping)
           LOG_ERR(LOG_CTX_PTIN_DHCP, "Unable to get ethernet priority");
         return L7_FAILURE;
      }
      frameEthPrty  = (L7_uint8*)(frame + 2*sizeof(L7_enetMacAddr_t) + sizeof(L7_ushort16));
      *frameEthPrty &= 0x1F; //Reset p-bit
      *frameEthPrty |= ((0x7 & ethPrty) << 5); //Set p-bit

      /* PTin modified: DHCP snooping */
      if (dsFrameIntfFilterSend(relayOptIntIfNum, vlanId, frame, frameLen, L7_FALSE, innerVlanId, client_idx) == L7_SUCCESS)
      {
        dsInfo->debugStats.serverOption82Tx++;
        ptin_dhcp_stat_increment_field(relayOptIntIfNum, vlanId, client_idx, DHCP_STAT_FIELD_TX_SERVER_REPLIES_WITHOUT_OPTIONS);
        ptin_dhcp_stat_increment_field(relayOptIntIfNum, vlanId, client_idx, DHCP_STAT_FIELD_TX_FORWARDED);
        return L7_SUCCESS;
      }
      return L7_FAILURE;
    }
    else
    {
       //Change ethernet priority bit
      if (ptin_dhcp_ethPrty_get(vlanId, &ethPrty) != L7_SUCCESS)
      {
         if (ptin_debug_dhcp_snooping)
           LOG_ERR(LOG_CTX_PTIN_DHCP, "Unable to get ethernet priority");
         return L7_FAILURE;
      }
      frameEthPrty  = (L7_uint8*)(frame + 2*sizeof(L7_enetMacAddr_t) + sizeof(L7_ushort16));
      *frameEthPrty &= 0x1F; //Reset p-bit
      *frameEthPrty |= ((0x7 & ethPrty) << 5); //Set p-bit

      /* If there is no Circuit-id information in the Reply pakcets,
         Forward the DHCP replies to the interface based on the DHCP Snooping
         binding for the client. */
      if (dsReplyFrameForward(intIfNum, vlanId, dhcpPacket->chaddr, frame,
                              frameLen, innerVlanId, client_idx) != L7_SUCCESS)     /* PTin modified: DHCP snooping */
      {
        if (dsCfgData->dsTraceFlags & DS_TRACE_FRAME_TX)
        {
          L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
          osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                        "(%s)Failed to get DHCP snooping binding for MAC(%s)",
                        __FUNCTION__, dhcpPacket->chaddr);
          dsTraceWrite(traceMsg);
        }
      }
      else
      {
        return L7_SUCCESS;
      }
    }
  }
  else
  {
    requestFlag = L7_TRUE;
  }
  if (dsFrameFlood(intIfNum, vlanId, frame, frameLen,
                   requestFlag, innerVlanId, client_idx) == L7_SUCCESS)     /* PTin modified: DHCP snooping */
  {
    #ifdef L7_DHCP_L2_RELAY_PACKAGE
    if (dsCfgData->dsL2RelayAdminMode == L7_ENABLE &&
        dhcpPacket->op == L7_DHCP_BOOTP_REQUEST)
    {
      dsInfo->debugStats.clientOption82Tx++;
    }
    #endif
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/***********************************************************************
* @purpose Flood DHCP packet from client toward server
*
* @param    intIfNum   @b{(input)} receive interface
* @param    vlanId     @b{(input)} VLAN ID
* @param    frame      @b{(input)} ethernet frame
* @param    frameLen   @b{(input)} ethernet frame length, incl eth header (bytes)
* @param    requestFlag @b{(input)} TRUE for DHCP requests (i.e., from client)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   Send on ports in VLAN except untrusted
*
* @end
*
***********************************************************************/
L7_RC_t dsFrameFlood(L7_uint32 intIfNum, L7_ushort16 vlanId,
                     L7_uchar8 *frame, L7_ushort16 frameLen,
                     L7_BOOL requestFlag, L7_ushort16 innerVlanId, L7_uint client_idx)
{
  NIM_INTF_MASK_t portMask;
  L7_uint32 i;
  L7_RC_t rc = L7_SUCCESS;

  if (ptin_debug_dhcp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_DHCP, "Going to flood packet (src_intIfNum %u, vlanId=%u, innerVlanId=%u)", intIfNum, vlanId, innerVlanId);

  /* PTin modified: DHCP snooping */
  #if 1
  /* Only send to trusted ports */
  if (ptin_dhcp_intfTrusted_getList(vlanId, &portMask) == L7_SUCCESS)
  #else
  if (dot1qVlanEgressPortsGet(vlanId, &portMask) == L7_SUCCESS)
  #endif
  {
    for (i = 1; i < DS_MAX_INTF_COUNT; i++)
    {
      if (i == intIfNum)
      {
        continue;
      }

      /* The API dot1qVlanEgressPortsGet returns both LAG interface as
         well as its particpating interfaces. So exclude the particpiating
         interfaces so that the underlying layer takes care of forwarding
         to participating interfaces. */
      if (dot3adIsLagActiveMember(i) == L7_TRUE)
      {
        continue;
      }

      if (L7_INTF_ISMASKBITSET(portMask, i))
      {
        if (ptin_debug_dhcp_snooping)
          LOG_TRACE(LOG_CTX_PTIN_DHCP, "Going to flood to intIfNum %u", i);

        /* This port is in the packet's VLAN.
         * If we are not snooping this VLAN,
         *   send packet on every member port.
         * If we are snooping the packet's VLAN,
         *   send only on trusted ports. Reason:  If this is a client message,
         *   we only need to flood on ports where
         *   a server might be, and servers have to be behind trusted ports.
         *   If this is a server message, then there must not be a binding
         *   for the client. If there had been a binding, we would have already
         *   sent the message just on the port identified in the binding. Since
         *   there is no binding, we will flood in the VLAN. However, we don't
         *   send on untrusted ports, hoping that the client will send another
         *   request and trigger creation of a binding. Letting the client get
         *   a DHCP address on an untrusted port with no binding could cause
         *   problems for the client if IPSG or DAI are enabled on the port. */
        if (!_dsVlanEnableGet(vlanId) || _dsVlanIsIntfRoot(vlanId,i) /*_dsIntfTrustGet(i)*/)    /* PTin modified: DHCP snooping */
        {
          L7_uint8  ethPrty;
          L7_uint8  *frameEthPrty;

          if (ptin_debug_dhcp_snooping)
            LOG_TRACE(LOG_CTX_PTIN_DHCP, "I am here");

          //Change ethernet priority bit
          if (ptin_dhcp_ethPrty_get(vlanId, &ethPrty) != L7_SUCCESS)
          {
            if (ptin_debug_dhcp_snooping)
              LOG_ERR(LOG_CTX_PTIN_DHCP, "Unable to get ethernet priority");
            return L7_FAILURE;
          }
          frameEthPrty  = (L7_uint8*)(frame + 2*sizeof(L7_enetMacAddr_t) + sizeof(L7_ushort16));
          *frameEthPrty &= 0x1F; //Reset p-bit
          *frameEthPrty |= ((0x7 & ethPrty) << 5); //Set p-bit

          if (dsFrameIntfFilterSend(i, vlanId, frame, frameLen,
                         requestFlag, innerVlanId, client_idx) != L7_SUCCESS)                     /* PTin modified: DHCP snooping */
          {
            ptin_dhcp_stat_increment_field(intIfNum, vlanId, client_idx, DHCP_STAT_FIELD_TX_FAILED);
            rc = L7_FAILURE;
          }
          /* PTin added: DHCP snooping */
          #if 1
          else
          {
             L7_BOOL isActiveOp82, isActiveOp37, isActiveOp18;
             L7_ushort16 ethHdrLen = sysNetDataOffsetGet(frame);
             L7_uchar8 ipVersion = (0xF0 & *(L7_uchar8*)(frame + ethHdrLen)) >> 4 ;

             if (ptin_dhcp_client_options_get(intIfNum, vlanId, innerVlanId, &isActiveOp82, &isActiveOp37, &isActiveOp18) != L7_SUCCESS)
             {
                if (ptin_debug_dhcp_snooping)
                  LOG_ERR(LOG_CTX_PTIN_DHCP, "Unable to get DHCP options");
                return L7_FAILURE;
             }

             if(L7_IP_VERSION == ipVersion)
             {
                if(isActiveOp82)
                {
                   ptin_dhcp_stat_increment_field(i, vlanId, client_idx, DHCP_STAT_FIELD_TX_CLIENT_REQUESTS_WITH_OPTION82);
                }
                if(!isActiveOp82)
                {
                   ptin_dhcp_stat_increment_field(i, vlanId, client_idx, DHCP_STAT_FIELD_TX_CLIENT_REQUESTS_WITHOUT_OPTIONS);
                }
             }
             else
             {
                if (isActiveOp37)
                {
                   ptin_dhcp_stat_increment_field(i, vlanId, client_idx, DHCP_STAT_FIELD_TX_CLIENT_REQUESTS_WITH_OPTION37);
                }
                if (isActiveOp18)
                {
                   ptin_dhcp_stat_increment_field(i, vlanId, client_idx, DHCP_STAT_FIELD_TX_CLIENT_REQUESTS_WITH_OPTION18);
                }
                if ( (!isActiveOp18) && (!isActiveOp37) )
                {
                   ptin_dhcp_stat_increment_field(i, vlanId, client_idx, DHCP_STAT_FIELD_TX_CLIENT_REQUESTS_WITHOUT_OPTIONS);
                }
             }

             ptin_dhcp_stat_increment_field(i, vlanId, client_idx, DHCP_STAT_FIELD_TX_FORWARDED);
          }
          #endif
        }
      }
    }
  }
  return rc;
}
/***********************************************************************
* @purpose Filter and forward DHCP packet on specified interface.
*
* @param    intIfNum   @b{(input)} receive interface
* @param    vlanId     @b{(input)} VLAN ID
* @param    frame      @b{(input)} ethernet frame
* @param    frameLen   @b{(input)} ethernet frame length, incl eth header (bytes)
* @param    requestFlag@b{(input)} DHCP/BOOTP Request or Reply.
* @param    innerVlanId@b{(input)} inner VLAN ID, incase packet
*                                  received is double tagged.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   This API is place holder to filter packets based on the destination
*          port (interface on which packet is being sent) rules. And ofcourse,
*          it is DOT1AD subscription rules which help the purpose in Metro
*          builds.
*
* @end
*
***********************************************************************/
L7_RC_t dsFrameIntfFilterSend(L7_uint32 intIfNum, L7_ushort16 vlanId,
                       L7_uchar8 *frame, L7_ushort16 frameLen,
                       L7_BOOL requestFlag, L7_ushort16 innerVlanId, L7_uint client_idx)      /* PTin modified: DHCP snooping */
{
#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
  DOT1AD_INTFERFACE_TYPE_t intfType;
  L7_uchar8 *frameTemp = dsInfo->pktBuff;
  L7_DOT1AD_SVCTYPE_t svType;

  if ((dsCfgData->dsL2RelayAdminMode != L7_ENABLE) ||
      (_dsVlanIntfL2RelayGet(vlanId,intIfNum) /*_dsIntfL2RelayGet(intIfNum)*/ != L7_TRUE))    /* PTin modified: DHCP snooping */
  {
    /* L2 relay not either not enabled globally or on the rx interface.
     * Assume packet got this far because DHCP snooping is enabled */
    return dsFrameSend(intIfNum, vlanId, innerVlanId, client_idx, frame, frameLen);           /* PTin modified: DHCP snooping */
  }

  if (dot1adInterfaceTypeGet(intIfNum, &intfType) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (DOT1AD_INTFERFACE_TYPE_SWITCHPORT == intfType)
  {
    return dsFrameSend(intIfNum,vlanId,innerVlanId,client_idx,frame,frameLen);                /* PTin modified: DHCP snooping */
  }

  if (dot1adServiceTypeGet(vlanId, &svType) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (intfType == DOT1AD_INTFERFACE_TYPE_NNI)
  {
    dsFrameDoubleVlanTagAdd(intIfNum,vlanId,frame, &frameLen, innerVlanId, frameTemp);
    if (dsFrameSend(intIfNum, vlanId, innerVlanId, client_idx, frameTemp, frameLen) != L7_SUCCESS)    /* PTin modified: DHCP snooping */
    {
      return L7_FAILURE;
    }
    /* Reset the control block frame to reuse.*/
    memset (frameTemp, 0, DS_DHCP_PACKET_SIZE_MAX);
  }
  else if (intfType == DOT1AD_INTFERFACE_TYPE_UNI ||
           intfType == DOT1AD_INTFERFACE_TYPE_UNI_P ||
           intfType == DOT1AD_INTFERFACE_TYPE_UNI_S)
  {
    dot1adSubscriptionStruct_t *subscrEntry = L7_NULLPTR;
    L7_uint32                  searchVlanId = innerVlanId;
    L7_uint32                  matchType = L7_MATCH_EXACT;
    L7_BOOL                    foundEntry = L7_FALSE;

    if (svType == L7_DOT1AD_SVCTYPE_ETREE)
    {
      if (innerVlanId != L7_DOT1AD_ETREE_RESERVED_INNER_VLAN)
      {
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

    if (dot1adSubscriptionEntryTreeServiceTypeSearch(intIfNum,vlanId,
                                             searchVlanId, matchType,
                                         &subscrEntry) != L7_SUCCESS)
    {
      searchVlanId = 0;
      matchType = L7_MATCH_EXACT; /* Try with vlanId + 0 exact combination */

      if (dot1adSubscriptionEntryTreeServiceTypeSearch(intIfNum,vlanId,
                                               searchVlanId, matchType,
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
              break;
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
    else
    {
      foundEntry = L7_TRUE;
    }
    if (foundEntry == L7_TRUE)
    {
       if (requestFlag == L7_TRUE &&
           (svType == L7_DOT1AD_SVCTYPE_ELINE ||
            svType == L7_DOT1AD_SVCTYPE_ETREE))
       {
         if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_SERVER)
         {
           L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
           memset(traceMsg, 0, DS_MAX_TRACE_LEN);
           osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                         "(%s)Returning as the request pkts are not to be "
                "sent on other UNIs for e-line and e-tree",__FUNCTION__);
           dsTraceWrite(traceMsg);
         }
         return L7_SUCCESS;
       }
       /* Whether its request or reply , send based on the UNI dot1ad rule.*/
       if (dot1adIsFieldConfigured(subscrEntry->configMask, DOT1AD_PKTTYPE) == L7_TRUE)
       {
         if (subscrEntry->pktType == L7_DOT1AD_PKTTYPE_UNTAGGED)
         {
           memcpy (frameTemp, frame, frameLen);
           /* dsFrameVlanTagRemove(frame, &frameLen, frameTemp); */
           vlanId = L7_NULL;
         }
         else if (subscrEntry->pktType == L7_DOT1AD_PKTTYPE_TAGGED)
         {
           if (dot1adIsFieldConfigured(subscrEntry->configMask, DOT1AD_MATCH_SVID) == L7_TRUE)
           {
             dsFrameSVlanTagAdd(intIfNum, frame, &frameLen, subscrEntry->svidToMatch, frameTemp);
             if (dot1adIsFieldConfigured(subscrEntry->configMask, DOT1AD_MATCH_CVID) == L7_TRUE)
             {
               dsFrameCVlanTagInsert(frameTemp, &frameLen,subscrEntry->cvidToMatch,frameTemp);
             }
             else if(innerVlanId)
             {
               dsFrameCVlanTagInsert(frameTemp, &frameLen,innerVlanId,frameTemp);
             }
           }
           else if (dot1adIsFieldConfigured(subscrEntry->configMask, DOT1AD_MATCH_CVID) == L7_TRUE)
           {
             dsFrameCVlanTagAdd(frame, &frameLen, subscrEntry->cvidToMatch, frameTemp);
           }
         }
       }
       else
       {
         /* Send pkt as it comes by removing S-tag.*/
         if (innerVlanId == L7_NULL)
         {
           memcpy (frameTemp, frame, frameLen);
           vlanId = L7_NULL;
         }
         else
         {
           dsFrameCVlanTagAdd(frame, &frameLen, innerVlanId, frameTemp);
         }
       }
      return dsFrameSend(intIfNum, vlanId, innerVlanId, client_idx, frameTemp, frameLen);     /* PTin modified: DHCP snooping */
    }
    else
    {
      if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_SERVER)
      {
        L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
        memset(traceMsg, 0, DS_MAX_TRACE_LEN);
        osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                      "(%s) Failed to get subsc entry with intf = %d, svid = %d, cvid = %d", __FUNCTION__,
             intIfNum, vlanId, innerVlanId);
        dsTraceWrite(traceMsg);
      }
    }
  }
  return L7_SUCCESS;
#else
  return dsFrameSend(intIfNum,vlanId,innerVlanId,client_idx,frame,frameLen);      /* PTin modified: DHCP snooping */
#endif
}
/***********************************************************************
* @purpose Send a DHCP packet on a given interface
*
* @param    intIfNum   @b{(input)} outgoing interface
* @param    vlanId     @b{(input)} VLAN ID
* @param    frame      @b{(input)} ethernet frame
* @param    frameLen   @b{(input)} ethernet frame length, incl eth header (bytes)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   Send on all trusted ports in VLAN
*
* @end
*
***********************************************************************/
L7_RC_t dsFrameSend(L7_uint32 intIfNum, L7_ushort16 vlanId,
                    L7_ushort16 innerVlanId, L7_uint client_idx,    /* PTin modified: DHCP snooping */
                    L7_uchar8 *frame, L7_ushort16 frameLen)
{
  L7_netBufHandle   bufHandle;
  L7_uchar8        *dataStart;
  L7_INTF_TYPES_t   sysIntfType;
  //L7_BOOL           is_vlan_stacked;

  if (ptin_debug_dhcp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_DHCP, "Going to transmit packet to intIfNum %u, vlanId=%u, innerVlanId=%u", intIfNum, vlanId, innerVlanId);

  /* If outgoing interface is CPU interface, don't send it */
  if ((nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS) &&
      (sysIntfType == L7_CPU_INTF))
  {
    return L7_SUCCESS;
  }

  if (dsCfgData->dsTraceFlags & DS_TRACE_FRAME_TX)
  {
    L7_uchar8 dsTrace[DS_MAX_TRACE_LEN];
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    osapiSnprintf(dsTrace, DS_MAX_TRACE_LEN,
                  "(%s)DHCP snooping forwarding %u byte frame on interface %s in VLAN %u.",
                  __FUNCTION__, frameLen, ifName, vlanId);
    dsTraceWrite(dsTrace);
  }

  SYSAPI_NET_MBUF_GET(bufHandle);
  if (bufHandle == L7_NULL)
  {
    /* Don't bother logging this. mbuf alloc failures happen occasionally. DHCP
     * should recover. */
    dsInfo->debugStats.txFail++;
    ptin_dhcp_stat_increment_field(intIfNum, vlanId, client_idx, DHCP_STAT_FIELD_TX_FAILED);
    return L7_FAILURE;
  }

  /* PTin added: DHCP snooping */
  #if 1
  L7_uint16 extOVlan = vlanId;
  L7_uint16 extIVlan = 0;
  //L7_int i;

  if (ptin_debug_dhcp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_DHCP, "Going to transmit packet to intIfNum %u, vlanId=%u, innerVlanId=%u", intIfNum, vlanId, innerVlanId);

  /* Extract external outer and inner vlan for this tx interface */
  if (ptin_dhcp_extVlans_get(intIfNum, vlanId, innerVlanId, client_idx, &extOVlan,&extIVlan) == L7_SUCCESS)
  {
    /* Modify outer vlan */
    if (vlanId!=extOVlan)
    {
      frame[14] &= 0xf0;
      frame[14] |= ((extOVlan>>8) & 0x0f);
      frame[15]  = extOVlan & 0xff;
      //vlanId = extOVlan;
    }
    /* Add inner vlan when there exists, and if vlan belongs to a stacked EVC */
    if (extIVlan!=0)
    {
      //for (i=frameLen-1; i>=16; i--)  frame[i+4] = frame[i];
            /* No inner tag? */
      if (osapiNtohs(*((L7_uint16 *) &frame[16])) != 0x8100 &&
          osapiNtohs(*((L7_uint16 *) &frame[16])) != 0x88A8 &&
          osapiNtohs(*((L7_uint16 *) &frame[16])) != 0x9100)
      {
        memmove(&frame[20],&frame[16],frameLen);
        frame[16] = 0x81;
        frame[17] = 0x00;
        frameLen += 4;
      }
      frame[18] = (frame[14] & 0xe0) | ((extIVlan>>8) & 0x0f);
      frame[19] = extIVlan & 0xff;
      //innerVlanId = extIVlan;
    }
  }
  #endif

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, dataStart);
  memcpy(dataStart, frame, frameLen);
  SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, frameLen);

  if (dsCfgData->dsTraceFlags & DS_TRACE_FRAME_TX)
  {
    int row, column;
    L7_uchar8 *pkt = frame;

    printf("\n===================\n");
    printf("======DHCP PKT=====\n");
    printf("===================\n");
    for (row = 0; row < 5; row++)
    {
      printf("%04x ", row * 16);
      for (column = 0; column < 16; column++)
      {
        printf("%2.2x ",pkt[row*16 + column]);
      }
      printf("\n");
    }
    printf("===================\n");
  }

  if (ptin_debug_dhcp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_DHCP, "Ready to transmit packet to intIfNum %u, vlanId=%u, innerVlanId=%u", intIfNum, vlanId, innerVlanId);

  if (dtlIpBufSend(intIfNum, vlanId, bufHandle) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DHCP_SNOOPING_COMPONENT_ID,
            " Failed to send DHCP pkt on intf (%s) and vlanID (%d)",
              ifName, vlanId);
    return L7_FAILURE;
  }

  if (ptin_debug_dhcp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_DHCP,"Packet transmited to intIfNum=%u, with oVlan=%u (intVlan=%u), iVlan=%u",
              intIfNum, extOVlan, vlanId, extIVlan);

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Reset the received packet count at the start of
*           new 1 second interval on each interface.
*           And count the consecutive intervals with rate_limit hit
*
* @param    none
*
* @returns  void
*
* @notes    This function gets called every 1 second
*
* @end
*********************************************************************/
void dsRateLimitCheck(void)
{
  L7_uint32 i;

  for (i = 1; i < DS_MAX_INTF_COUNT; i++)
  {
    if((dsCfgData->dsIntfCfg[i].rateLimitCfg.rate_limit != L7_DS_RATE_LIMIT_NONE) &&
       (dsIntfInfo[i].pktRxCount >= (L7_uint32) dsCfgData->dsIntfCfg[i].rateLimitCfg.rate_limit))
    {
      dsIntfInfo[i].consecutiveInterval++;
    }
    else
    {
      dsIntfInfo[i].consecutiveInterval = 1;
    }
    dsIntfInfo[i].pktRxCount = 0;
  }

  /* reschedule the timer */
  osapiTimerAdd((void *)dsTimerCallback, L7_NULL, L7_NULL,
                DS_LEASE_TIMER_INTERVAL, &dsLeaseTimer);
}

/*********************************************************************
* @purpose  Start the DS timer
*
* @param    none
*
* @returns  L7_SUCCESS
*           L7_FAILURE if timer already running
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsTimerStart(void)
{
  if (dsLeaseTimer)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
            "Attempted to start the DHCP snooping lease timer while timer "
            "already running.");
    return L7_FAILURE;
  }

    /* reschedule the timer */
  osapiTimerAdd((void *)dsTimerCallback, L7_NULL, L7_NULL,
                DS_LEASE_TIMER_INTERVAL, &dsLeaseTimer);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check if the configured rate limit is exceeded on this
*           interface
*
* @param    pduInfo  @b{(input)} pointer to pdu info structure
*                                which stores intIfNum and vlanId
*
* @returns  L7_TRUE  - pkt rate > rate limit
*                     (or) the interface is down
*
* @returns  L7_FALSE  - pkt rate < rate limit
*                       or if the rate limit is none
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL dsRateLimitFilter(sysnet_pdu_info_t *pduInfo)
{
  L7_uint32 linkState;

  /* We shall directly ask NIM in this intercept function to know the
   * exact status of the interface in the h/w. This will ensure,
   * we drop those packets received when the interface is
   * being made error-disabled */
  if(! ((nimGetIntfLinkState(pduInfo->intIfNum, &linkState) == L7_SUCCESS) &&
        (linkState == L7_UP)) )
  {
    return L7_TRUE;
  }

  if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FALSE;    /* don't risk disabling interface for this */
  }

  /* Enforce the rate limit check on the packets received on this interface */
  if(dsCfgData->dsIntfCfg[pduInfo->intIfNum].rateLimitCfg.rate_limit
      != L7_DS_RATE_LIMIT_NONE)
  {
    dsIntfInfo[pduInfo->intIfNum].pktRxCount++;

    /* PTin removed: DHCP snooping */
    #if 0
    if((dsIntfInfo[pduInfo->intIfNum].consecutiveInterval >=
        dsCfgData->dsIntfCfg[pduInfo->intIfNum].rateLimitCfg.burst_interval) &&
       (dsIntfInfo[pduInfo->intIfNum].pktRxCount >=
        (L7_uint32) dsCfgData->dsIntfCfg[pduInfo->intIfNum].rateLimitCfg.rate_limit))
    {
      /* Error disable this interface by calling NIM API.
       * User has to explicitly do "no shutdown"
       * to bring the interface up */
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(pduInfo->intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCP_SNOOPING_COMPONENT_ID,
              "DHCP Snooping has diagnostically disabled interface %s. "
              "The incoming DHCP message rate on this interface exceeded the rate "
              "limit of %d pps (burst interval %d).",
              ifName, dsCfgData->dsIntfCfg[pduInfo->intIfNum].rateLimitCfg.rate_limit,
              dsCfgData->dsIntfCfg[pduInfo->intIfNum].rateLimitCfg.burst_interval);

      dsIntfInfo[pduInfo->intIfNum].pktRxCount = 0;
      /* Disable the i/f in h/w */
      nimSetIntfAdminState(pduInfo->intIfNum, L7_DIAG_DISABLE);

      /* Raise a trap for the error disabling event */
       trapMgrDsIntfErrorDisabledTrap(pduInfo->intIfNum);

      osapiWriteLockGive(dsCfgRWLock);
      return L7_TRUE;
    }
    #endif
  }

  osapiWriteLockGive(dsCfgRWLock);
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Check if the configured time is expired to store the
*           DHCP Snooping database
*
*
* @notes   It's a static function and invokes the tftp functions.
*
* @end
*********************************************************************/


static void dsPeriodicStore()
{
  static int i = 0;

  /* Try to store into remote db or to local db only when the
   * persistency feature is present */
  if(cnfgrIsFeaturePresent(L7_DHCP_SNOOPING_COMPONENT_ID,
                           L7_DHCP_SNOOPING_DB_PERSISTENCY) == L7_TRUE)
  {
    if(i == dsCfgData->dsDbStoreInterval)
    {
      i=0;
      /* if (_dsBindingsCount()) */
      if (dsInfo->dsDbDataChanged == L7_TRUE)
      {
        if (dsCfgData->dsDbIpAddr) /* Save the bindings in a remote machine */
        {
          dsTftpUploadInit();
        }
        else /* Save the bindings in local machine */
        {
          dsDbLocalSave();
        }
        dsInfo->dsDbDataChanged = L7_FALSE;
      }
    }
    else if (i > dsCfgData->dsDbStoreInterval)
    {
      i=0;
    }
    else
    {
      i++;
    }
  }
}

void setdoubleTaggedTx(L7_BOOL neW)
{
  doubleTaggedTx = neW;
}
