/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    dot1dgarp.c
* @purpose     GARP Wrapper functions
* @component   GARP
* @comments    none
* @create      02/23/2001
* @author      Hassan
* @author      djohnson, 5/3/04, partial rewrite
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "defaultconfig.h"
#include "sysapi.h"
#include "osapi.h"
#include "dot3ad_api.h"
#include "string.h"
#include "log.h"
#include "garpapi.h"
#include "garpcfg.h"
#include "garpctlblk.h"
#include "dot1dgarp.h"
#include "dot1s_api.h"
#include "garp.h"
#include "gvrapi.h"
#include "gmrapi.h"
#include "gidapi.h"
#include "gipapi.h"
#include "gmrp_api.h"
#include "dot1q_api.h"
#include "usmdb_util_api.h"
#include "simapi.h"
#include "garp_leaveall_timer.h"
#include "garp_dot1q_api.h"
#include "garp_dot1q_util.h"
#include "garp_timer.h"
#include "component_mask.h"
#include "garp_debug.h"
#include "mfdb_api.h"
#include "nimapi.h"

gvrpMac_t *gvrpMac;
gvrpMac_t *gmrpMac;
void *garpTaskSyncSema;
GarpInfo_t garpInfo;
extern osapiRWLock_t garpCfgRWLock;
extern garpCfg_t     *garpCfg;

extern gmrpInstance_t *gmrpInstance;
extern GARPCBptr GARPCB;
extern gmrpInfo_t *gmrpInfo;
extern garpCnfgrState_t garpCnfgrState;

extern void garpParseCnfgr(L7_CNFGR_CMD_DATA_t *pCmdData);
extern L7_RC_t gipDeletePortGIPInstance(L7_uint32 instanceID,L7_uint32 portNumber);
extern L7_RC_t gipInsertPortGIPInstance(L7_uint32 instanceID, L7_uint32 portNumber);
extern L7_RC_t gipCreateGIPInstance(L7_uint32 instanceID);
extern L7_RC_t gipDeleteGIPInstance(L7_uint32 instanceID);

extern void garpTraceQueueMsgSend(GarpPacket *msg);
extern void garpTraceQueueMsgRecv(GarpPacket *msg);
extern gvrpInfo_t *gvrpInfo;

static void garpVlanAddProcess(L7_uint32 vlanId);
static L7_RC_t garpVlanPortChangeProcess(L7_uint32 intIfNum, L7_uint32 vlanId);
static L7_RC_t garpVlanChangeProcess(dot1qNotifyData_t *vlanData, L7_uint32 intIfNum, L7_uint32 event,L7_ushort16 gvrp_event);
static L7_uint32 garpDot1sChangeProcess(L7_uint32 mstID, L7_uint32 intIfNum, L7_uint32 event);
static void garpIntfAcquireReleaseCheck(L7_uint32 intIfNum, L7_PORT_EVENTS_t event,
                                        L7_INTF_ACQUIRE_ACTIONS_t *action,
                                        L7_COMPONENT_IDS_t *componentID);

void garpIntfEventActiveProcess(L7_uint32 intIfNum);
static void garpIntfEventInactiveProcess(L7_uint32 intIfNum);
/****************************************************************************/
/* Local persistent GARP and GVRP variables.  There is a single GARP        */
/* database used by all apps and a single instance of GVRP.  GMRP           */
/* instances are created/destroyed dynamically as VLANs are                 */
/* registered/deregistered.                                                 */
/****************************************************************************/
static void *garp_app;
Gvr  *gvrp_app;
void        *garpQueue;
void        *garpPduQueue;

static L7_uchar8 garpTempBuff[garp_max_GPDU_len] = {0};

/* need this file to link in other functions from garb_debug.c */
extern void garpDumbFunction(void);

failed_to_enqueue_timers_t failed_timer[L7_MAX_INTERFACE_COUNT+1];

extern L7_BOOL delete_gvrp_attribute(Gvr *app, Vlan_id vid);

extern void gid_gmr_leaveall_timer_expired(L7_uint32 vid,
                                           Gid *my_port, Gmr *gmr_inst);
extern L7_uint32 GenerateLeaveAllTimeoutValue(L7_uint32 leaveall_timeout);

extern void garpExpireGVRPLeaveAllTimer(L7_uint32 port_no);
extern void garpExpireGMRPLeaveAllTimers(L7_uint32 port_no);
static void garpTimerProcess();
static L7_RC_t garpIntfChangeProcess(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator);
static void garpGenericAttributeProcess(GARPCommand  *cmd);


extern void gid_leaveall(Gid *);


/* 3 Bytes For LLC Header:  DSAP = 0x42, LSAP = 0x42, Control = 0x03, and
 * 2 Bytes For Protocol Id: 0x00 0x01 */
static L7_uchar8 GARP_802_2_LLC_HEADER_AND_PROTOCOL[5] =
{
  0x42, 0x42, 0x03, 0x00, 0x01
};

extern L7_uint32 getNumberOfGVDEntries();
extern L7_BOOL gvd_delete_entry(L7_uint32 delete_at_index);

/*********************************************************************
* @purpose  check if this is a garp valid port
*
* @param    port    port to bechecked
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @notes
*
* @end
*********************************************************************/
static L7_BOOL garpValidPort(GARPPort port)
{
  L7_BOOL returnVal = L7_FALSE;

  /* If port is within range, return True  */
  if ((port >= 0) && (port < L7_MAX_INTERFACE_COUNT))
  {
    if ((dot3adIsLagMember(port) == L7_FALSE) ||
        (dot3adIsLagConfigured(port) == L7_TRUE))
    {
      returnVal = L7_TRUE;
    }
  }

  return returnVal;
}

/*********************************************************************
* @purpose  notify garp a link state is changed
*
* @param    GARPSpanState    link state
* @param    GARPPort         port
*
* @returns  GARP_SUCCESS
* @returns  GARP_FAILURE
*
* @notes
*
* @end
*********************************************************************/
static GARPStatus garpSpanState(GARPSpanState state, GARPPort port)
{
  GarpPacket msg;
  GARPStatus returnVal = GARP_SUCCESS;

  /*  if the queue is created and the port is valid */
  if ( (garpQueue != L7_NULL) &&  garpValidPort(port) )
  {
    msg.msgId = (state ? G_PORT_CONN : G_PORT_DISCONN);
    msg.port  = port;

    garpTraceQueueMsgSend(&msg);

    if (osapiMessageSend(garpQueue, (void *)&msg, sizeof(GarpPacket), L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM) == L7_ERROR)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_GARP_COMPONENT_ID,
          "garpSpanState: QUEUE SEND FAILURE: port= %d, state = %s."
          " The garpQueue is full, logs specifics of the message content"
          " like internal interface number, type of message etc.",
              port, state ? "G_PORT_CONN" : "G_PORT_DISCONN");
    }
    else
    {
        osapiSemaGive(garpInfo.msgQSema);
        returnVal = GARP_SUCCESS;
    }
  }

  return returnVal;
}

/*********************************************************************
* @purpose  Tell GARP that interface changed state.
*
* @param    GARPSpanState    link state
* @param    GARPPort         port
*
* @returns  GARP_SUCCESS
* @returns  GARP_FAILURE
*
* @notes
*
* @end
*********************************************************************/
static GARPStatus garpIfStateChange(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator)
{

  NIM_EVENT_COMPLETE_INFO_t status;
  GarpPacket msg;
  GARPStatus returnVal = GARP_FAILURE;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  if (garpIsValidIntf(intIfNum) != L7_TRUE)
  {

      status.intIfNum    = intIfNum;
      status.component   = L7_GARP_COMPONENT_ID;
      status.event       = event;
      status.correlator  = correlator;
      status.response.rc = L7_SUCCESS;
      nimEventStatusCallback(status);
      return L7_SUCCESS;
  }


  if (garpQueue != L7_NULL)
  {
    msg.msgId = G_INTF_STATE_CHANGE;
    msg.port  = intIfNum;
    msg.msgData.nim.nim_correlator = correlator;
    msg.msgData.nim.nim_event = event;

    garpTraceQueueMsgSend(&msg);

    if (osapiMessageSend(garpQueue,
                         (void *)&msg,
                         sizeof(GarpPacket),
                         L7_WAIT_FOREVER,
                         L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
    {

        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_GARP_COMPONENT_ID,
            "garpIfStateChange: QUEUE SEND FAILURE: intf= %s, event = %d, correlator = %d."
            " The garpQueue is full, logs specifics of the message content like"
            " internal interface number, type of message etc.", ifName, event, correlator);
        L7_LOG_ERROR(intIfNum);
    }
    else
    {
        osapiSemaGive(garpInfo.msgQSema);
        returnVal = GARP_SUCCESS;
    }
  }

  return returnVal;
}

/*********************************************************************
* @purpose  connect a port
*
* @param    port    port to be connected
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
static void connect_port(GARPPort port)
{
  L7_uint32 unit;


  GARP_TRACE_INTERNAL_EVENTS("connect_port:  %d\n", port);

  unit = simGetThisUnit();

  /* ask GIP to connect this port */
  gip_connect_port_to_active_topology(&gvrp_app->g, (L7_uint32)port);
  if ((dot1sModeGet() == L7_DISABLE) ||
      (dot1sMstiPortStateGet(DOT1S_CIST_ID, (L7_uint32)port) == L7_DOT1S_MANUAL_FWD))

  {
    gipInsertPortGIPInstance(DOT1S_CIST_ID, (L7_uint32)port);
  }

  /* GMRP connect for each VLAN */
  gmrpConnectPort(port);
}

/*********************************************************************
* @purpose  disconnect a port
*
* @param    port     port to be diconnected
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
static void disconnect_port( GARPPort port)
{
  L7_uint32 unit;


  GARP_TRACE_INTERNAL_EVENTS("disconnect_port:  %d\n", port);

  unit = simGetThisUnit();

  /* ask GIP to disconnect this port */
  gip_disconnect_port_from_active_tolpolgy(&gvrp_app->g, (L7_uint32)port);
  if ((dot1sModeGet() == L7_DISABLE))
  {
      gipDeletePortGIPInstance(DOT1S_CIST_ID, (L7_uint32)port);
  }

  /* GMRP disconnect */
  gmrpDisconnectPort(port);
}

/*********************************************************************
* @purpose  Look up which application this packet belongs to
*
* @param    pPkt         pointer to the packet
*
* @returns  GARP_GVRP_APP = 0,
* @returns  GARP_GMRP_APP,
* @returns  UNDEFINED_GARP_APP
*
* @notes    Determines if given packet is a GPDU, and if so,
*           determines its application destination.
*
* @end
*********************************************************************/
static GARPApplication garp_packet_lookup( L7_uchar8 *pPkt )
{
    GARPApplication app_id = UNDEFINED_GARP_APP;
    L7_uint32       offset = 0;


    /* set proper application ID for GPDU */
    if (GarpPktIsGvrpPDU(pPkt))
    {
        app_id = GARP_GVRP_APP;
    }
    else if (GarpPktIsGmrpPDU(pPkt))
    {
        app_id = GARP_GMRP_APP;
    }

    /* if the packet has a valid GPDU MAC address */
    if (app_id != UNDEFINED_GARP_APP)
    {
        /* if the packet is tagged */
        offset = GarpPktIsTagged(pPkt);

        /* Validate the LLC Header (3 bytes) and Protocol Id (2 bytes),
         * i.e. make sure that the 5-byte sequence starting from
         * pPkt[14+offset] is  0x42 0x42 0x03 0x00 0x01
         */
        if (memcmp((void *)&(pPkt[14+offset]),
                   (void *)&(GARP_802_2_LLC_HEADER_AND_PROTOCOL[0]), 5) != 0)
        {
            app_id = UNDEFINED_GARP_APP;
        }
    }

    return app_id;
}

/*********************************************************************
* @purpose  Copy a receive buffer to a local buffer
*
* @param    GARPRcvCBPtr received buffer
* @param    L7_uchar8    pointer to buffer ( to copy the received buffer in)
* @param    L7_uint32    received buffer length
*
* @returns  GARP_SUCCESS
* @returns  GARP_FAILURE
*
* @notes
*
* @end
*********************************************************************/
GARPStatus GarpRcbToBuffCopy(L7_uchar8 *data, L7_uchar8 * pBuff, L7_uint32 bufLen)
{
  /* copy data into the given buffer */
  if (bufLen <= garp_max_GPDU_len)
  {
    memcpy(pBuff,data,bufLen);
  }
  else
  {
    /* if the length is greater than the max, */
    /* copy the max - 2 bytes.                */
    /* the last 2 bytes are left to be NULL   */
    /* for saftey                             */
    memcpy(pBuff,data,garp_max_GPDU_len-2);
  }

  return (pBuff != L7_NULL) ? GARP_SUCCESS : GARP_FAILURE;
}

/*********************************************************************
* @purpose  Processes a received GPDU.
*
* @param    pGPDU         pointer to the packet
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void process_pdu_rx(GarpPduPacket *garpPkt)
{
  GARPApplication  app_id;
  L7_uchar8        *pxgTempBuff;
  L7_uint32        port;
  L7_uchar8        *data;
  L7_uint32        dataLen;
  L7_uint32        offset = 0;
  L7_uint32        vlanId = 0;
  L7_uint32        index = 0;
  L7_uint32        interfaceMode=0;

  /* Debug - total PDUs received by GARP task */
  garpInfo.infoCounters.garpPDUsReceived++;

  /* get the data start */
  SYSAPI_NET_MBUF_GET_DATASTART(garpPkt->msgData.pdu.bufHandle, data);

  /* get the data length */
  SYSAPI_NET_MBUF_GET_DATALENGTH(garpPkt->msgData.pdu.bufHandle, dataLen);

 if (((GARP_PHASE_EXECUTE == garpCnfgrState) ||
      (GARP_PHASE_INIT_3  == garpCnfgrState)) &&
      ((garpGetGvrpEnabled() == L7_TRUE) ||
       (garpGetGmrpEnabled() == L7_TRUE)))
  {
    /* reset the buffer */
    memset(garpTempBuff, 0x00, garp_max_GPDU_len);

    /* get the GRCB data (actual GPDU) */
    pxgTempBuff = (L7_uchar8 *)&garpTempBuff[0];

    /*  extract the GPDU from the RCB */
    if ((GarpRcbToBuffCopy(data, pxgTempBuff, dataLen)) == GARP_SUCCESS)
    {
      /* get the port number */
      port = garpPkt->port;

      /* get the vlan ID */
      vlanId = garpPkt->msgData.pdu.vlan_id;

      /* get the application from this GPDU */
      app_id =  garp_packet_lookup(pxgTempBuff);

      if (app_id != UNDEFINED_GARP_APP)
      {
        /* if the packet is tagged AND the LSAP, PROT_ID and TPID are GPDU */

        offset = GarpPktIsTagged(data);

        if (app_id == GARP_GVRP_APP)
        {

          /* Debug */
          garpInfo.infoCounters.gvrpPDUsReceived++;

          memcpy(gvrpMac[port].mac_addr, &pxgTempBuff[6], L7_MAC_ADDR_LEN);

          /* check if gvrp is enabled , before processing the GVRP packet*/
          if (garpGetGvrpEnabled() == L7_TRUE)
          {
            /* check gvrp interface mode on the specified interface */
            if (garpIntfCfgGet(port,&interfaceMode,GARP_GVRP_APP)==L7_SUCCESS)
            {
              if(interfaceMode==L7_ENABLE)
                gvr_rcv(gvrp_app, port, (void *)(pxgTempBuff+17+offset));
            }
          }
        }
        else if (app_id == GARP_GMRP_APP)
        {
            L7_BOOL     process_gmrp_packet;
            L7_BOOL     gmrp_database_is_full;
            L7_BOOL     new_vlan_entry_sought;


            L7_int32    gmrpEntries=0;

            process_gmrp_packet = L7_FALSE;
            new_vlan_entry_sought = L7_FALSE;

            /* Debug */
            garpInfo.infoCounters.gmrpPDUsReceived++;

            /* Is gmrp enabled globally and on the interface? */
            if (garpGetGmrpEnabled() == L7_TRUE)
            {
            /* check gvrp interface mode on the specified interface */
            if (garpIntfCfgGet(port,&interfaceMode,GARP_GMRP_APP)==L7_SUCCESS)
            {
              if(interfaceMode==L7_ENABLE)
              {
                  process_gmrp_packet = L7_TRUE;
              }
            }

           /*--------------------------------------------------------------*/
           /*  Determine if database is full:

              Gmr structures are not filled in for a VLAN until the
              the VLAN is seen in a packet. There is no need to request
              an entry if the GMRP database is full.

              New multicast addresses (and new VLANs) should not be
              added to the database if the database is full.  However,
              messages for currently registered addresses must be processed

              The packet must be parsed to determine if there is an attempt
              to register a new multicast address, so that check will be
              made in a later routine.
              */
            /*--------------------------------------------------------------*/
            if ( process_gmrp_packet == L7_TRUE)
            {

                gmrp_database_is_full = L7_TRUE;
                if (garpInfo.infoCounters.gmrpRegistationCount < L7_MAX_GROUP_REGISTRATION_ENTRIES)
                {
                    /* Extra sanity check: No need to process a new entry
                       if MFDB database is full from other components. */
                    if (mfdbCurrEntriesGet(&gmrpEntries) == L7_SUCCESS)
                    {
                        if (gmrpEntries < L7_MFDB_MAX_MAC_ENTRIES)
                        {
                            gmrp_database_is_full = L7_FALSE;
                        }
                    }
                }


                /*  Determine if this is a newly seen VLAN for GMRP */
                new_vlan_entry_sought       = L7_TRUE;
                if (gmrpInstanceIndexFind(vlanId,&index) == L7_SUCCESS)
                {
                    /* GMRP already knows about this VLAN */
                    new_vlan_entry_sought = L7_FALSE;

                }

                /*--------------------------------------------*/
                /* Create a GMR entry for the newly seen VLAN */
                /*--------------------------------------------*/
                if (new_vlan_entry_sought == L7_TRUE)
                {
                    if (gmrp_database_is_full == L7_TRUE)
                    {
                        process_gmrp_packet = L7_FALSE;
                    }
                    else
                    {
                        /* Query dot1q for existence of VLAN and create a Gmr entry */
                        if (dot1qVlanCheckValid(vlanId) == L7_SUCCESS)
                        {
                            if (gmrpCreateGmr(vlanId)!= L7_SUCCESS)
                            {
                                /* Failure reasons are logged in underlying routines */
                                process_gmrp_packet = L7_FALSE;
                            }
                            else
                            {
                                /* Query dot1q for each of the member ports */

                                L7_RC_t     rc;
                                GARPCommand cmd;
                                L7_uint32   searchIntIfNum;
                                L7_uint32   vlanStatus;

                                rc = nimFirstValidIntfNumber(&searchIntIfNum);
                                while (rc == L7_SUCCESS)
                                {
                                    if (dot1qIntfIsValid(searchIntIfNum) == L7_TRUE)
                                    {
                                        if ( (dot1qOperVlanMemberGet(vlanId, searchIntIfNum,
                                                                     &vlanStatus) == L7_SUCCESS)
                                             && (vlanStatus == L7_DOT1Q_FIXED) )
                                        {

                                            if (GarpBuildCommand(&cmd, GARP_GMRP_APP, GARP_ENABLE_PORT,
                                                               GARP_GMRP_ATTRIBUTE, L7_NULL, searchIntIfNum, vlanId)
                                                               == GARP_SUCCESS)
                                            {
                                                garpIntfGMRPModeProcess(cmd.port, L7_ENABLE, &cmd);

                                            }
                                        }
                                    }
                                  rc = nimNextValidIntfNumber(searchIntIfNum, &searchIntIfNum);
                                } /* while nimNextValidIntfNumber */
                              }  /* else gmrpCreateGmr  was successful */
                            }  /* dot1qVlanCheckValid */

                        }  /* else gmrp_database_is_full is not full */
                    }  /* new_entry_sought is false */
                } /* check for database full, create Gmr entry for VLAN */

            }  /* GMRP enabled*/


            /* increment the number of received pkts
               Increment should occur before the first opportunity to discard*/
            gmrpInfo->received[port]++;

            /* Ensure that a gmrp instance exists */
            if (gmrpInstanceIndexFind(vlanId,&index) != L7_SUCCESS)
            {
                process_gmrp_packet = L7_FALSE;

            }

            if ( process_gmrp_packet == L7_TRUE)
            {
                /* find the right GMRP instance and then pass it to gmr_rcv */
                memcpy(gmrpMac[port].mac_addr, &pxgTempBuff[6], L7_MAC_ADDR_LEN);
                gmr_rcv(gmrpInstance[index].gmr, port, (void*)(pxgTempBuff+17+offset));
            }
            else
            {
                garpInfo.infoCounters.gmrpRxPdusNotProcessed++;
            }
        }
      }
    }
  }

  /* free up the RCB and return to caller */
  SYSAPI_NET_MBUF_FREE(garpPkt->msgData.pdu.bufHandle);
}


/*********************************************************************
* @purpose  Removes all GVRP entries (static, dynamic and defeault
*           VLANs) from GVRP database.
*
* @returns  void
*
* @notes    This function is called when GVRP is administratively
*           disabled from the command line ("no set gvrp adminmode"
*           command is issued) in order to remove all VLAN entries
*           from GVRP database.
*           Dynamic entries are removed from Dot1Q susbsystem
*           by dot1qGVRPEnable() function.
*
* @end
*********************************************************************/
void static Remove_All_GVRP_Entries()
{
    L7_int32 numOfGVDEntries;
    Gid       *my_port, *first_port;
    L7_uint32 first_port_no;


    /* get the number of entries in GVRP database */
    numOfGVDEntries = (L7_int32)getNumberOfGVDEntries();

    first_port = gvrp_app->g.gid;

    if ((Gid *)L7_NULL == first_port)
    {
        return;
    }

    my_port       = first_port;
    first_port_no = first_port->port_no;


    do
    {
        my_port->transmit_leaveall  = L7_FALSE;
        my_port->cstart_join_timer  = L7_FALSE;
        my_port->cstart_leave_timer = L7_FALSE;




        gid_leaveall(my_port);

        if (my_port->application->last_gid_used > 0)
        {
            gid_leave_timer_expired(my_port->port_no);
        }

        my_port = my_port->next_in_connected_ring;

    } while (first_port_no != my_port->port_no);

}

/*********************************************************************
* @purpose  Deletes all instances of GMRP on all ports of the device.
*
* @returns  void
*
* @notes    This function resembles garpLeaveAllUpdateGMRPTimers().
*           It is called when GMRP is administratively disabled from
*           the command line ("no set gmrp adminmode" command is
*           issued) in order to remove all instances of GMRP on all
*           ports.
*           For every instance of GMRP, this function acts as if
*           the corresponding GMRP Leave All timer has expired.
*           The Leave All message doesn't need to be sent, though,
*           so we  set my_port->transmit_leaveall to L7_FALSE before
*           processing this pseudo GMRP Leave All timer expiration
*           event.
*
* @end
*********************************************************************/
void static Remove_All_GMRP_Instances()
{
    L7_uint32  nextVlanId, vlanId, index;
    Gid        *this_port;
    Gmr        *gmrpApp;
    Gid        *my_port;

    vlanId     = 0;
    nextVlanId = 0;
    index      = 0;

    while (L7_SUCCESS == dot1qNextVlanGet(vlanId, &nextVlanId))
    {
        if (L7_SUCCESS == gmrpInstanceIndexFind(nextVlanId, &index))
        {
            gmrpApp   = gmrpInstance[index].gmr;
            my_port   = gmrpApp->g.gid;
            this_port = my_port;

            if (L7_NULL != (Gid *)my_port)
            {
                do
                {
                    /* we are disabling GMRP administratively;
                     * here, we are "expiring" all GMRP LeaveAll timers on
                     * all GMRP instances
                     */
                    if ((L7_TRUE == my_port->is_enabled) &&
                        (L7_TRUE == my_port->is_connected))
                    {
                        /* process the GMRP Leave All timer, but
                         * don't send the GMRP Leave All PDU */
                        my_port->transmit_leaveall = L7_FALSE;

                        gid_gmr_leaveall_timer_expired(nextVlanId, my_port, gmrpApp);

                        /* generate a new random value for the GMRP Leave All
                         * timer for the next time when GMRP is enabled */
                        my_port->gmrp_leaveall_left =
                            GenerateLeaveAllTimeoutValue(my_port->leaveall_timeout);
                    }

                    my_port = my_port->next_in_port_ring;

                } while (this_port != my_port);
            }

            (void) gmrpDestroyGmr(nextVlanId);
        }

        vlanId = nextVlanId;
    }

    return;
}

/*********************************************************************
* @purpose    Processes a GARP command
*
* @param      GARPCommand* cmd, reference to a single GARP command
*
* @returns      void
*
* @notes
*
* @end
*********************************************************************/
void process_command(GARPCommand *cmd)
{

  L7_BOOL status;

  /* check for valid GARP application */
  if ( (cmd->app == GARP_GVRP_APP) || (cmd->app == GARP_GMRP_APP) )
  {

    garpTraceProcessCommand(cmd);

    /* begin processing of a GARPCommandNameTable type command */
    switch (cmd->command)
    {
      case GARP_JOIN:
      case GARP_LEAVE:
      case GARP_DELETE_ATTRIBUTE:
      case GARP_NORMAL_PARTICIPANT:
      case GARP_NON_PARTICIPANT:
      case GARP_NORMAL_REGISTRATION:
      case GARP_REGISTRATION_FIXED:
      case GARP_REGISTRATION_FORBIDDEN:

        garpGenericAttributeProcess(cmd);
        break;

    case GARP_ENABLE_PORT:

        if (cmd->port != L7_ALL_INTERFACES)
        {
          /* Acquired interfaces  */
          if (garpIntfAcquiredGet(cmd->port, &status) == L7_SUCCESS)
          {
              if (status  == L7_TRUE)
              {
                  break;
              }

          }
        }

        /* Process the command */
        if (cmd->app == GARP_GVRP_APP)
        {
          garpIntfGVRPModeEnableProcess(cmd->port, (void*)cmd);

        }
        else if (cmd->app == GARP_GMRP_APP)
        {
            garpIntfGMRPModeProcess(cmd->port, L7_ENABLE, (void*)cmd);
        }
        break;
    case GARP_DISABLE_PORT:

       if(cmd->port != L7_ALL_INTERFACES)
       {
         /* Acquired interfaces  */
         if (garpIntfAcquiredGet(cmd->port, &status) == L7_SUCCESS)
         {
            if (status  == L7_TRUE)
            {
                break;
            }
         }

       }

        /* Process the command */
      if (cmd->app == GARP_GVRP_APP)
      {

        garpIntfGVRPModeDisableProcess(cmd->port, (void*)cmd);

      }
      else if (cmd->app == GARP_GMRP_APP)
      {

        garpIntfGMRPModeProcess(cmd->port, L7_DISABLE, (void*)cmd);
      }
      break;


      case GARP_ENABLE:
          (void)garpGarpModeProcess(cmd->app, GARP_ENABLE);
          break;

    case GARP_DISABLE:

          (void)garpGarpModeProcess(cmd->app, GARP_DISABLE);

          break;

      case GARP_JOIN_TIME:
      {
        garpJoinTimeProcess(cmd->port, *(L7_uint32 *)&cmd->data);

      }
      break;

      case GARP_LEAVE_TIME:
      {

        garpLeaveTimeProcess(cmd->port, *(L7_uint32 *)&cmd->data);

      }
      break;

      case GARP_LEAVEALL_TIME:
      {

        garpLeaveAllTimeProcess(cmd->port, *(L7_uint32 *)&cmd->data);

      }
      break;

      case GVRP_VLAN_CREATION_FORBID:
        {
          setVlanCreationForbid(gvrp_app->g.gid, cmd->port, *(L7_BOOL *)&cmd->data);
        }
        break;

      case GVRP_VLAN_REGISTRATION_FORBID:
        {
          setVlanRegistrationForbid(gvrp_app->g.gid, cmd->port, *(L7_BOOL *)&cmd->data);
        }
        break;

      default:
        break;

    } /* switch(cmd) */

  } /* endif */

  return;
}

/*********************************************************************
* @purpose  Send a message to the Garp_Task to process the incomming packet.
*
* @param    bufHandle   buffer handle to the bpdu received
* @param    intIfNum    interface number
*
* @returns  L7_SUCCESS  if the message was put on the queue
* @returns  L7_FAILURE  if the message was not put on the queue
*
* @notes    void
*
* @end
*********************************************************************/
L7_RC_t GarpSendPDU(L7_netBufHandle bufHandle, L7_uint32 intIfNum, L7_uint32 vlan_id)
{
  GarpPduPacket msg;
  GARPApplication garp_app;
  L7_RC_t    rc ;
  L7_uchar8        *data;
  L7_uint32        dataLen;
  L7_uint32  interfaceMode=0;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  /* initialize*/
  garp_app= UNDEFINED_GARP_APP;
  rc = L7_ERROR;

  if (garpPduQueue != L7_NULL)
  {
    msg.msgId     = G_PDU;
    msg.port      = intIfNum;
    msg.msgData.pdu.bufHandle = bufHandle;
    msg.msgData.pdu.vlan_id   = vlan_id;

    /* check if gvrp or gmrp mode is enabled before sending data */
    /* get the data start */
    SYSAPI_NET_MBUF_GET_DATASTART(msg.msgData.pdu.bufHandle, data);

    /* get the data length */
    SYSAPI_NET_MBUF_GET_DATALENGTH(msg.msgData.pdu.bufHandle, dataLen);

    garp_app=garp_packet_lookup(data);
    if (((garp_app==GARP_GVRP_APP) && (garpGetGvrpEnabled()==L7_ENABLE))||
          ((garp_app==GARP_GMRP_APP) && (garpGetGmrpEnabled()==L7_ENABLE)))
    {

       /* check interface mode */
       if ((garpIntfCfgGet(intIfNum,&interfaceMode,garp_app)== L7_SUCCESS) &&
            (interfaceMode == L7_ENABLE))
       {
         /*garpTraceQueueMsgSend(&msg);*/

         /* PDUs are sent as L7_NO_WAIT because it is not critical to the system
            if a packet is dropped */
         rc = osapiMessageSend(garpPduQueue, (void*)&msg, sizeof(GarpPduPacket), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);

         if (rc != L7_SUCCESS)
         {
           L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_GARP_COMPONENT_ID,
               "GarpSendPDU: QUEUE SEND FAILURE: intf= %s, vlan_id = %d, bufHandle = %d."
               " The garpPduQueue is full, logs specific of the GPDU, internal interface "
               "number, vlan id, buffer handle etc.", ifName, vlan_id, bufHandle);
         }
         else
         {
             osapiSemaGive(garpInfo.msgQSema);
         }
       }  /*check interface mode */

      }/* check gvrp and gmrp mode */
      else if ((garp_app == UNDEFINED_GARP_APP) && (GarpPktIsGvrpPDU(data) == L7_TRUE))
      {
         /* increment the error statistic */
         gvrpInfo->garpErrorStats[intIfNum].invalidProtocolId++;
      }

  }


  /* ALT - If rc is L7_ERROR then DTL will take care of freeing the
  **       network buffer.
  */
  return rc;
}

/*********************************************************************
* @purpose  Install or un install Hooks for Garp.
*
* @param    state    garp state
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
static void garp_toggle(GarpStateTable state)
{
  /* If Request is ENABLE */
  if (state == ENABLE_GARP)
  {
    /* ENABLE GARP Interface */
    (void)GarpSpanInstallHook(garpSpanState);
    GARPCB->GarpCB_intfStateChange = garpIfStateChange;
  }
  else
  {
    /* DISABLE OFF GARP Interface */
    (void)GarpSpanUnInstallHook();
    GARPCB->GarpCB_intfStateChange = L7_NULL;
  }

  return;
}

/*********************************************************************
* @purpose  Initialize GARP applications
*
* @param    None
*
* @returns  GARP_SUCCESS
* @returns  GARP_FAILURE
*
* @notes
*
* @end
*********************************************************************/
static GARPStatus init_garp_apps(void)
{
  GARPStatus returnVal = GARP_FAILURE;

  /* GVRP */
  if ((gvr_create_gvr(GARP_GVRP_APP, &garp_app)) == L7_TRUE)
  {
    gvrp_app = (Gvr*)garp_app;

    /* Enable GARP PDU reception, don't do the interface yet */
    garp_toggle(ENABLE_GARP);

    returnVal = GARP_SUCCESS;
  }

  return returnVal;
}

/*********************************************************************
* @purpose  create and start the GARP Task.
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void garpCPTask(void)
{
  GarpPacket msg;
  GarpPduPacket pduMsg;
  L7_ushort16 i;
  L7_RC_t   status;

  garpDumbFunction();

  for (i=0; i<=L7_MAX_INTERFACE_COUNT; i++)
  {
      failed_timer[i].gvrp_join_in = L7_FALSE;
  }

  /* create the main GARP queue */

  garpInfo.msgQSema = osapiSemaCCreate (OSAPI_SEM_Q_FIFO, 0);
  if (garpInfo.msgQSema == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_GARP_COMPONENT_ID,
            "garpCPTask: Failed to create msgQSema, exiting.\n");
    return;
  }

  garpQueue     = (void*)osapiMsgQueueCreate(GARP_QUEUE, GARP_MSG_COUNT, sizeof(GarpPacket));
  garpPduQueue = (void*)osapiMsgQueueCreate(GARP_PDU_QUEUE, GARP_PDU_MSG_COUNT, sizeof(GarpPduPacket));

  if ( (garpQueue == L7_NULL) && (garpPduQueue == L7_NULL) )
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_GARP_COMPONENT_ID,
            "garpCPTask: Failed to create garpQueue or garpPduQueue , exiting.\n");
  }
  else
  {
    /* Initialize GARP applications */
    if (init_garp_apps() == GARP_SUCCESS)
    {
      /* Post that GARP CCP is ready for command and GPDU processing */
      GARPCB->GarpCB_garpReady = L7_TRUE;
      (*GARPCB->GarpCB_garpInitDone)();

      /* main task loop */
      while (1)
      {
        /* Since we are reading from multiple queues, we cannot wait forever
         * on the message receive from each queue. Rather than sleep between
         * queue reads, use a semaphore to indicate whether any queue has
         * data. Wait until data is available. */

         if (osapiSemaTake(garpInfo.msgQSema, L7_WAIT_FOREVER) != L7_SUCCESS)
         {
             L7_LOGF(L7_LOG_SEVERITY_INFO, L7_GARP_COMPONENT_ID,
                     "Failure taking GARP message queue semaphore");
             continue;
         }

          memset((void*)&msg, 0, sizeof(GarpPacket));
          memset((void*)&pduMsg, 0, sizeof(GarpPduPacket));

          /* Always try to read from the main processing queue first. This queue
             contains all control messages for the system. */

          status = osapiMessageReceive(garpQueue, &msg, sizeof(GarpPacket), L7_NO_WAIT);

          if (status != L7_SUCCESS)
          {
              /* If the processing queue was empty, see if a PDU needs processing */
              status = osapiMessageReceive(garpPduQueue, &pduMsg, sizeof(GarpPduPacket), L7_NO_WAIT);
              if (status == L7_SUCCESS && pduMsg.msgId == G_PDU)
              {
                msg.msgId = G_PDU;
              }
          }

          if (status != L7_SUCCESS)
          {
              L7_LOGF(L7_LOG_SEVERITY_INFO, L7_GARP_COMPONENT_ID,
                      "Bad status %d from garpQueue or garpPduQueue\n", status);
              continue;
          }

        osapiSemaTake(garpTaskSyncSema, L7_WAIT_FOREVER);

        /* Allow for debug */
        garpInfo.msgsRecvdOnQueue++;


        /* Trace the percentage usage of the queue.
           Ideally this would be done when sending a
           messsage to the queue. However, the garp
           component has many places where messages
           are sent to the queue, so it is done on receive */
        garpTraceMsgQueueUsage();

        /* Trace the acutal message */
        garpTraceQueueMsgRecv(&msg);


        /* Process message */

        switch (msg.msgId)
        {
          case G_COMMAND:
            /* GARP command (GARPCommandNameTable) */
            process_command(&msg.msgData.command);
            break;


          /* G_PORT_CONN and G_PORT_DISCONN are obsolete,
             the intended functionality is handled by G_DOT1S_NOTIFY */
          case G_PORT_CONN:
            /* pass through */
          case G_PORT_DISCONN:
            break;

          case G_PDU:
            process_pdu_rx(&pduMsg);
            break;

          case G_DOT1S_NOTIFY:
            garpDot1sChangeProcess(msg.msgData.dot1s.mstID, msg.port,
                                  msg.msgData.dot1s.event);
            break;

          case G_TIMER:
            /* Process popped timers */
            garpTimerProcess();

            break;


          case G_LEAVEALL_TIMER:
              /* Process popped timers */
              garpLeaveAllTimerProcess();
              break;

          case G_INTF_STATE_CHANGE:
            garpIntfChangeProcess(msg.port, msg.msgData.nim.nim_event,
                                  msg.msgData.nim.nim_correlator);
            break;

          case G_NIM_STARTUP:
            if (msg.msgData.startup.startupPhase == NIM_INTERFACE_CREATE_STARTUP)
            {
              garpNimCreateStartup();
            }
            else if (msg.msgData.startup.startupPhase == NIM_INTERFACE_ACTIVATE_STARTUP)
            {
              garpNimActivateStartup();
            }
            else
            {
              L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_GARP_COMPONENT_ID,
                      "Unknown NIM startup event %d", msg.msgData.startup.startupPhase);
            }
            break;

          case G_CNFGR:
            garpParseCnfgr(&(msg.msgData.CmdData));
            break;


          case G_VLAN_NOTIFY:
            garpVlanChangeProcess(&msg.msgData.dot1q.vlanData, msg.port,
                                  msg.msgData.dot1q.event,msg.msgData.dot1q.gvrp_event);
            break;


          default:
            break;

        }

        (void)osapiSemaGive(garpTaskSyncSema);

      } /* while(1) */
    }

    else
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_GARP_COMPONENT_ID,
              "garpCPTask: Failed to initialize Garp, exiting.\n");
    }
  }

  return;
}

/*********************************************************************
* @purpose  Tell Garp the link status has changed
*
* @param    intIfNum        interface number
* @param    event           event
* @param    correlator      correlator for event
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t garpIntfChangeProcess(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator)
{
  L7_RC_t rc = L7_SUCCESS;
  NIM_EVENT_COMPLETE_INFO_t status;
  L7_INTF_TYPES_t   sysIntfType;
  GARPCommand       cmd;
  L7_uint32         prevAcquired, currAcquired;
  L7_uint32         acqAction, componentId;
  L7_uint32         mode;

  memset( (void *)&cmd, 0, sizeof(cmd));

  /*  If Garp exist  */
  if (GARPCB != L7_NULL)
  {
    /*   Can the Command be issued?  */
    if ((GarpIsReady() == L7_TRUE) &&
        (GARPCB->GarpCB_linkStateChange != L7_NULL) &&
        (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS) &&
        (garpIsValidIntfType(sysIntfType) == L7_TRUE))
    {
      switch (event)
      {
      case L7_CREATE:
        /* General garp interface create */
        (void)garpIntfCreate(intIfNum);
        (void)garpStatsIntfCreate(intIfNum);

        /* gmrp interface create */
        gmrpIntfCreate(intIfNum);

        break;

      case L7_ATTACH:
        rc = garpApplyIntfConfigData(intIfNum, GARP_APPLY_CONFIG_NORMAL);
        break;

      case L7_DETACH:
        /* A DETACH may occur without an active port ever becoming inactive.
           Handle this case */

        garpIntfEventInactiveProcess(intIfNum);

        /* Now, process the DETACH event */
        cmd.port = intIfNum;
        cmd.command = GARP_DISABLE_PORT;
        cmd.app =  GARP_GVRP_APP;
        cmd.vlan_id = 0;

        process_command(&cmd);

        cmd.app =  GARP_GMRP_APP;
        process_command(&cmd);

        break;

      case L7_DELETE:
        (void) garpIntfDelete (intIfNum);
        (void) gmrpIntfDelete(intIfNum);
        break;


      case L7_INACTIVE:
        garpIntfEventInactiveProcess(intIfNum);
        break;

      case L7_ACTIVE:
        garpIntfEventActiveProcess(intIfNum);
        break;



      default:

        /* Determine if the interface is to be acquired or released by the component */
        garpIntfAcquireReleaseCheck(intIfNum, event,&acqAction, &componentId);

        if (acqAction == L7_INTF_ACQ_ACTION_ACQUIRE)
        {

            COMPONENT_ACQ_NONZEROMASK(garpInfo.acquiredList[intIfNum],
                                      prevAcquired);
            /* If the interface was not previously acquired,
               disable the protocol if it is currently enabled. */
            if (prevAcquired == L7_FALSE)
            {
                cmd.port = intIfNum;
                cmd.command = GARP_DISABLE_PORT;
                cmd.vlan_id = 0;

                garpQportsGVRPCfgGet(intIfNum, &mode);
                if ( mode==L7_ENABLE)
                {
                    cmd.app =  GARP_GVRP_APP;
                    process_command(&cmd);
                }

                garpQportsGMRPCfgGet(intIfNum, &mode);
                if ( mode==L7_ENABLE)
                {
                    cmd.app =  GARP_GMRP_APP;
                    process_command(&cmd);
                }

            }
            /* update aquired list */
            COMPONENT_ACQ_SETMASKBIT(garpInfo.acquiredList[intIfNum], componentId);

            rc = L7_SUCCESS;

        }
        else if (acqAction == L7_INTF_ACQ_ACTION_RELEASE)
        {
            /* The interface may participate in the component participation */
            /* update aquired list */
            COMPONENT_ACQ_CLRMASKBIT(garpInfo.acquiredList[intIfNum], componentId);
            COMPONENT_ACQ_NONZEROMASK(garpInfo.acquiredList[intIfNum],currAcquired);
            if (currAcquired == L7_FALSE)
            {
                (void)garpApplyIntfGvrpMode(intIfNum);
                (void)garpApplyIntfGmrpMode(intIfNum);
            }
            rc = L7_SUCCESS;
        }
        else
            /* not an event of local interest */
            rc = L7_SUCCESS;

        /* break */
        break;
      }
    }
  }

  /* Notify NIM */

  status.intIfNum    = intIfNum;
  status.component   = L7_GARP_COMPONENT_ID;
  status.event       = event;
  status.correlator  = correlator;
  status.response.rc = rc;
  nimEventStatusCallback(status);

  return rc;
}


/*********************************************************************
* @purpose  Process the interface event L7_ACTIVE
*
* @param    intIfNum      @b{(input)} internal interface number
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void garpIntfEventActiveProcess(L7_uint32 intIfNum)
{
    connect_port(intIfNum);
}

/*********************************************************************
* @purpose  Process the interface event L7_INACTIVE
*
* @param    intIfNum      @b{(input)} internal interface number
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
static void garpIntfEventInactiveProcess(L7_uint32 intIfNum)
{

    /* de-register all dynamic VLANs from the port; this needs to be
     * done before calling disconnect_port() so that GVRP could send
     * LeaveEmpty messages (if there are any dynamic VLANS registered
     * on msg.port) from other connected ports */
    garpExpireGVRPLeaveAllTimer((L7_uint32)(intIfNum));

    /* also, de-register all dynamic entries from all instances
     * of GMRP on the current port */
    garpExpireGMRPLeaveAllTimers((L7_uint32)(intIfNum));

    /* and finally, GARP disconnect port (span) */
    disconnect_port(intIfNum);

}

/*********************************************************************
* @purpose  Checks to see if the interface should be acquired or released
*           as a result of the event notification
*
* @param    intIfNum      @b{(input)} internal interface number
* @param    event         @b{(input)} NIM event
* @param    action        @b{(input)} action to take
* @param    componentID   @b{(input)} component to associate with the action
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void garpIntfAcquireReleaseCheck(L7_uint32 intIfNum, L7_PORT_EVENTS_t event,
                                   L7_INTF_ACQUIRE_ACTIONS_t *action,
                                   L7_COMPONENT_IDS_t *componentID)
{
  switch (event)
  {
  case L7_LAG_ACQUIRE:

      *action = L7_INTF_ACQ_ACTION_ACQUIRE;
      *componentID = L7_DOT3AD_COMPONENT_ID;

      break;

  case L7_PROBE_SETUP:

      *action = L7_INTF_ACQ_ACTION_ACQUIRE;
      *componentID = L7_PORT_MIRROR_COMPONENT_ID;

      break;

  case L7_PORT_ROUTING_ENABLED:

      *action = L7_INTF_ACQ_ACTION_ACQUIRE;
      *componentID = L7_IP_MAP_COMPONENT_ID;

      break;

  case L7_LAG_RELEASE:

      *action = L7_INTF_ACQ_ACTION_RELEASE;
      *componentID = L7_DOT3AD_COMPONENT_ID;

      break;

  case L7_PROBE_TEARDOWN:

      *action = L7_INTF_ACQ_ACTION_RELEASE;
      *componentID = L7_PORT_MIRROR_COMPONENT_ID;

      break;


  case L7_PORT_ROUTING_DISABLED:

      *action = L7_INTF_ACQ_ACTION_RELEASE;
      *componentID = L7_IP_MAP_COMPONENT_ID;

      break;



  default:
    /* not an event of local interest */
      *action = L7_INTF_ACQ_ACTION_NONE;
      *componentID = L7_FIRST_COMPONENT_ID;

      break;
  } /* endswitch */
}




/*********************************************************************
* @purpose  Processes GARP-related event initiated by Dot1Q
*
* @param (in)    vlanId    Virtual LAN Id
* @param (in)    intIfNum  Interface Number
* @param (in)    event
*
* @returns  L7_SUCCESS  or L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t garpVlanChangeProcess(dot1qNotifyData_t *vlanData, L7_uint32 intIfNum, L7_uint32 event,L7_ushort16 gvrp_event)
{
    L7_RC_t     rc;
    GARPCommand cmd;
    L7_uint32   status;
    L7_uint32   mode;
    L7_uint32   index;
    L7_uint32 i = 0, vlanId = 0, numVlans = 0;
    rc = L7_SUCCESS;
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

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
              /* "vlan <VLAN Id>" command issued */
              (void) garpVlanAddProcess(vlanId);

              /*--------------*/
              /* Process GMRP */
              /*--------------*/
              /* GMRP only processes VLAN adds as needed.
                 This is done on the reception of PDUs.
                 This allows memory usage to be limited
                 to only that required to hold actual
                 GMRP entries */
              break;

          case VLAN_DELETE_PENDING_NOTIFY:
                /*--------------*/
               /* Process GVRP */
               /*--------------*/
               if(gvrp_event == 1)
               {
                 if (GarpBuildCommand(&cmd, GARP_GVRP_APP, GARP_DELETE_ATTRIBUTE,
                                       GARP_GVRP_VLAN_ATTRIBUTE, (GARPAttrValue)(&vlanId),
                                       L7_NULL, L7_NULL) == GARP_SUCCESS)
                  {
                      garpGenericAttributeProcess(&cmd);
                  }
                  else
                  {
                      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_GARP_COMPONENT_ID,
                              "GARP VLAN process: Cannot process vlan callback for vlanId %d, intf %s, event %d\n",
                              vlanId, ifName, event);
                  }
               }

              /*--------------*/
              /* Process GMRP */
              /*--------------*/
               if (gvrp_event == 0)
               {
                 (void) gmrpDestroyGmr(vlanId);
               }
              break;



          case VLAN_DELETE_NOTIFY:
              /* "no vlan" command issued */
              /* vlan delete pending notify already processed for static vlans */
               /* no processing required for dynamic vlans */
              break;


          case VLAN_ADD_PORT_NOTIFY:
              /* "vlan participation include/exclude/auto" command issued */
              if ((GARP_GVRP_IS_ENABLED != L7_TRUE) &&
                  (GARP_GMRP_IS_ENABLED != L7_TRUE))
                  break;


              if (garpIsValidIntf(intIfNum) != L7_TRUE)
                  break;


              /* Acquired interfaces  */
              if (garpIntfAcquiredGet(intIfNum, &status) == L7_SUCCESS)
              {
                  if (status  == L7_TRUE)
                  {
                      break;
                  }
              }


              /*--------------*/
              /* Process GVRP */
              /*--------------*/

               (void)garpVlanPortChangeProcess(intIfNum,vlanId);

              /*--------------*/
              /* Process GMRP */
              /*--------------*/

              /*check if gmrp is enabled for the switch and this interface*/
              /*add port into the gid associated with this vlan*/
              /*the spanning tree state this is taken care by the GARP*/
              /*by invoking the connect to active topology*/

              garpGMRPEnableGet(&mode);
              garpQportsGMRPCfgGet(intIfNum, &status);
              if (( mode==L7_ENABLE) && ( status==L7_ENABLE) )
              {
                 /* Prior to release K, this path invoked
                    garpIntfGMRPModeProcess, which invokes the
                    DTL call and allocated memory.  As of release
                    K, memory allocation is restricted to only when
                    GARP neneeds it, but the dtl call is still required
                    for the driver to send GMRP frames to the CPU

                    The effect of this placement is to re-enable the port
                    in DTL each time a port is added to a VLAN.  The driver
                    fails to send GMRP PDUs to the CPU without this, so this
                    call will remain. */

                  (void)dtlDot1dGMRPEnable(intIfNum);

                  /* Process only if a GMRP instance exists for this VLAN */
                  if (gmrpInstanceIndexFind(vlanId,&index) == L7_SUCCESS)
                  {

                      if (GarpBuildCommand(&cmd, GARP_GMRP_APP, GARP_ENABLE_PORT,
                                         GARP_GMRP_ATTRIBUTE, L7_NULL, intIfNum, vlanId)
                                         == GARP_SUCCESS)
                      {
                          gmr_proc_command(&cmd);
                      }
                      else
                      {
                          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_GARP_COMPONENT_ID,
                                  "GMRP VLAN process: Cannot process vlan callback for vlanId %d, intf %s, event %d\n",
                                  vlanId, ifName, event);
                      }

                  }  /* gmrpInstanceFind */

              }

              break;

          case VLAN_DELETE_PORT_NOTIFY:
              /* "vlan participation include/exclude/auto" command issued */

              if ((GARP_GVRP_IS_ENABLED != L7_TRUE) &&
                  (GARP_GMRP_IS_ENABLED != L7_TRUE))
                  break;


              if (garpIsValidIntf(intIfNum) != L7_TRUE)
                  break;


              /* Acquired interfaces  */
              if (garpIntfAcquiredGet(intIfNum, &status) == L7_SUCCESS)
              {
                  if (status  == L7_TRUE)
                  {
                      break;
                  }
              }


              /*--------------*/
              /* Process GVRP */
              /*--------------*/

              (void)garpVlanPortChangeProcess(intIfNum,vlanId);

              /*--------------*/
              /* Process GMRP */
              /*--------------*/

              /*check if gmrp is enabled for the switch and this interface*/
              /*delete port from the gid associated with this vlan*/

              garpGMRPEnableGet(&mode);
              garpQportsGMRPCfgGet(intIfNum, &status);
              if (( mode==L7_ENABLE) && ( status==L7_ENABLE) )
              {
                  /* Purposely:  No DTL call is being made to disable GMRP on
                     the interface here */

                  /* Process only if a GMRP instance exists for this VLAN */
                  if (gmrpInstanceIndexFind(vlanId,&index) == L7_SUCCESS)
                  {

                      if (GarpBuildCommand(&cmd, GARP_GMRP_APP, GARP_DISABLE_PORT,
                                         GARP_GMRP_ATTRIBUTE, L7_NULL, intIfNum, vlanId)
                                         == GARP_SUCCESS)
                      {
                          gmr_proc_command(&cmd);

                      }
                      else
                      {
                          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_GARP_COMPONENT_ID,
                                  "GMRP VLAN process: Cannot process vlan change callback for vlanId %d, intf %s, event %d\n",
                                  vlanId, ifName, event);
                      }

                  }
              }

              break;

    case VLAN_RESTORE_NOTIFY:
            (void)osapiWriteLockTake(garpCfgRWLock, L7_WAIT_FOREVER);

            garpCfg->gvrp_enabled = L7_FALSE;
            garpCfg->gmrp_enabled = L7_FALSE;
            garpCfg->hdr.dataChanged = L7_TRUE;


            (void)osapiWriteLockGive(garpCfgRWLock);

              (void)garpGarpModeProcess(GARP_GVRP_APP, GARP_DISABLE);
              (void)garpGarpModeProcess(GARP_GMRP_APP, GARP_DISABLE);
              break;

          default:
              break;
      }
      numVlans++;
    }

    return rc;
}



/*********************************************************************
* @purpose  initialize multiple GIP data strucyure.
*
* @param    mstId    spanning tree ID
* @param    mstId    spanning tree ID
* @param    mstId    spanning tree ID
*
* @returns  L7_FAILURE
* @returns  L7_SUCCESS
*
* @notes   initialize the multiple GIP instance data structure
*
*
* @end
*********************************************************************/
L7_uint32 garpDot1sChangeProcess(L7_uint32 mstID, L7_uint32 intIfNum, L7_uint32 event)
{
  L7_RC_t rc = L7_SUCCESS;

  switch (event)
  {
    /* The follwing two case statements need to be L7_DOT1S_EVENT_FORWARDING and
     * L7_DOT1S_EVENT_NOT_FORWARDING. This routine is registered with dot1s and not NIM.
     * dot1s does not generate the L7_ACTIVE and L7_INACTIVE events, NIM
     * does that. The active status of the port is checked before a GARP PDU
     * is transmitted and a GARP PDU is never received on an INACTIVE interface,
     * thus the active status of the port is honored.
     */

    case L7_DOT1S_EVENT_FORWARDING:
      if (gipIsPortBelongToInstance(mstID,intIfNum) == L7_FAILURE)
      {
        rc = gipInsertPortGIPInstance(mstID, intIfNum);
      }
      /*--------------*/
      /* Process GMRP */
      /*--------------*/
      gmrpDot1sConnectPort(mstID, intIfNum);

      break;

    case L7_DOT1S_EVENT_NOT_FORWARDING:
      rc = gipDeletePortGIPInstance(mstID,intIfNum);

      /*--------------*/
      /* Process GMRP */
      /*--------------*/

      /* de-register all dynamic VLANs from the port */
      garpExpireGVRPLeaveAllTimer(intIfNum);

      /* also, de-register all dynamic entries from all instances
       * of GMRP on the current port */
      garpExpireGMRPLeaveAllTimers(intIfNum);

      gmrpDot1sDisconnectPort(mstID,intIfNum);
      break;

      break;

    case L7_DOT1S_EVENT_INSTANCE_CREATE:
      rc = gipCreateGIPInstance(mstID);
      break;

    case L7_DOT1S_EVENT_INSTANCE_DELETE:
      rc = gipDeleteGIPInstance(mstID);
      break;

    default:
      break;
  }

  return rc;
}
/*********************************************************************
* @purpose  Process popped GARP timers
*
* @param    void
*
* @returns  void
*
* @notes    This routine processes all popped timers and removes them from
*           the popped timers queue.
*
* @end
*********************************************************************/
static void garpTimerProcess()
{
    garpTimerDescr_t *pTimer;
    GARP_TIMER_TYPE_t timerType;
    L7_uint32       vid;
    L7_uint32       port_no;
    L7_uint32       gvrpMode;
    garpTimerList_t *pTimerList;

    /* Process timers only in phase 3 or execute state.*/
    if ((garpCnfgrState == GARP_PHASE_INIT_3) ||
        (garpCnfgrState == GARP_PHASE_EXECUTE))
    {

        /*--------------------------------------------------------*/
        /* Move timers from pending to popped list, as applicable */
        /*--------------------------------------------------------*/
        garpTimerAction();

        /*--------------------------*/
        /*  Process popped timers   */
        /*--------------------------*/
        pTimerList   = &garpTimers.garpTimersPoppedList ;

        while ((pTimer = garpTimerListDeleteFirst(pTimerList)) != L7_NULL)
        {
            timerType = pTimer->timerType;
            vid       = pTimer->vid;
            port_no   = pTimer->port_no;

            memset(pTimer, 0x0, sizeof(garpTimerDescr_t));


            switch (timerType)
            {
                case GARP_TIMER_GVRP_LEAVE:
                    if ((garpGVRPEnableGet(&gvrpMode) == L7_SUCCESS) && (gvrpMode==L7_TRUE))
                    {
                      gid_leave_timer_expired(port_no);
                    }
                    break;

                case GARP_TIMER_GMRP_LEAVE:
                   gid_gmr_leave_timer_expired(vid, port_no);
                   break;

                case GARP_TIMER_GVRP_JOIN:
                    gid_join_timer_expired(port_no);
                    break;

                case GARP_TIMER_GMRP_JOIN:
                    gid_gmr_join_timer_expired(vid, port_no);
                    break;

                default:
                    break;

            }    /* switch (pTimer) */


        }   /* while list is not empty */
    }

    return;
}

/*********************************************************************
* @purpose  Create Garp
*
* @param    GarpCBInst    Garp Control Block instance
*
* @returns  GARP_SUCCESS
* @returns  GARP_FAILURE
*
* @notes
*
* @end
*********************************************************************/
GARPStatus GarpCreate( void *GarpCBInst)
{
  GARPStatus returnVal;
  GARPCBptr  garpPtr = (GARPCBptr)GarpCBInst;

  /* setup GARP control block variables */
  garpPtr->GarpCB_gvrpSupported = L7_TRUE;
  garpPtr->GarpCB_gmrpSupported = L7_TRUE;

  /* create the semaphore to protect our data between the garpCPTask and others */
  garpTaskSyncSema = L7_NULLPTR;
  garpTaskSyncSema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (garpTaskSyncSema == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_GARP_COMPONENT_ID,
            "GarpCreate: garpTaskSyncSema not created.");
    return GARP_FAILURE;
  }

  /* make returnVal success by default; we'll change it in case of failure */
  returnVal = GARP_SUCCESS;

  /* Create the Garp Central Control Point (GCCP) task  */
  garpPtr->GarpCB_garpCCPTask = osapiTaskCreate(GARP_CCP_TASK,
                                                garpCPTask,
                                                0, 0,
                                                GARP_STACK_SIZE_DEFAULT,
                                                GARP_CCP_TASK_PRI,
                                                GARP_DEFAULT_TASK_SLICE);
  if (garpPtr->GarpCB_garpCCPTask == L7_ERROR)
  {
     returnVal = GARP_FAILURE;
     L7_LOGF(L7_LOG_SEVERITY_INFO, L7_GARP_COMPONENT_ID,
             "GarpCreate(): unable to create GCCP task.\n");
  }

  return returnVal;
}

/*********************************************************************
* @purpose  Enable GVRP for an interface
*
* @param (in) intIfNum  physical or logical interface to be configured
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all interfaces
* @param (in) status    L7_ENABLE  or L7_DISABLE
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t garpIntfGVRPModeEnableProcess(L7_uint32 intIfNum, GARPCommand *cmd)
{
  L7_uint32 i,start,end;
  L7_uint32 aqstatus;
  GARPCommand buildCmd;
  L7_uint32 VlanId, nextVlanId;
  L7_uint32 type, mode;
  L7_uint32 command;


  if ((intIfNum > L7_ALL_INTERFACES) || (intIfNum < 1))
  {
    return L7_NOT_EXISTS;
  }

  if (intIfNum == L7_ALL_INTERFACES)
  {
    start = 1;
    end = L7_MAX_INTERFACE_COUNT;
  }
  else
  {
    start = intIfNum;
    end = intIfNum+1;
  }


  for (i=start;i<end;i++)
  {

      /* if the interface has been acquired, don't implement the change */
      /* Besides, acquired interfaces will already be operationally disabled */
      if (garpIntfAcquiredGet(i, &aqstatus) == L7_SUCCESS)
      {
          if (aqstatus == L7_TRUE)
          {
              continue;
          }

      }

      /* Skip the port in case unit is not ready. Check if the port is attached */
      if (garpIntfAttachedGet(i) == L7_FALSE)
      {
         continue;
      }

      /* Enable the port */
      cmd->port = i;
      gvr_proc_command(gvrp_app, (void*)cmd);

      /* Process VLAN membership */

      VlanId     = 0;
      nextVlanId = 0;
      type       = 0;

      while(L7_SUCCESS == dot1qNextVlanGet(VlanId, &nextVlanId))
      {
          VlanId = nextVlanId;

           /* If the vlan is static or dot1x created vlan then its fine. */
          if ( (L7_SUCCESS == dot1qVlanMemberAndTypeGet(nextVlanId, i, &mode, &type)) &&
               ((type != L7_DOT1Q_TYPE_GVRP_REGISTERED) && (type != L7_DOT1Q_TYPE_WS_L2_TNNL_REGISTERED) &&
                (type != L7_DOT1Q_TYPE_IP_INTERNAL_VLAN)) )
          {
              if(type == L7_DOT1Q_TYPE_DOT1X_REGISTERED)
              { 
                      mode = L7_DOT1Q_NORMAL_REGISTRATION;
              } 
      
              switch (mode)
              {
                  case L7_DOT1Q_FIXED:
                      command = GARP_REGISTRATION_FIXED;
                      break;

                  case L7_DOT1Q_FORBIDDEN:
                      command = GARP_REGISTRATION_FORBIDDEN;
                      break;

                  case L7_DOT1Q_NORMAL_REGISTRATION:
                      command = GARP_NORMAL_REGISTRATION;
                      break;

                  default:
                      command = GARP_UNDEFINED_COMMAND;
                      break;
              }

              if (GARP_UNDEFINED_COMMAND != command)
              {
                  if ( GarpBuildCommand(&buildCmd, GARP_GVRP_APP, command, GARP_GVRP_VLAN_ATTRIBUTE,
                                        (GARPAttrValue)(&nextVlanId),i, L7_NULL) == GARP_SUCCESS)
                  {
                      gvr_proc_command(gvrp_app, (void*)&buildCmd);
                  }

              }

          } /* if (L7_SUCCESS == dot1qVlanMemberGet (nextvid, intIfNum, &val)) */

      } /* while(L7_SUCCESS == dot1qNextVlanGet(vlanId, &nextVlanId)) */


    } /* for i= start */

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Enable or Disable GVRP for an interface
*
* @param (in) intIfNum  physical or logical interface to be configured
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all interfaces
* @param (in) status    L7_ENABLE  or L7_DISABLE
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t garpIntfGVRPModeDisableProcess(L7_uint32 intIfNum, GARPCommand *cmd)
{
  L7_uint32 i,start,end;
  L7_uint32 aqstatus;


  if ((intIfNum > L7_ALL_INTERFACES) || (intIfNum < 1))
  {
    return L7_NOT_EXISTS;
  }

  if (intIfNum == L7_ALL_INTERFACES)
  {
    start = 1;
    end = L7_MAX_INTERFACE_COUNT;
  }
  else
  {
    start = intIfNum;
    end = intIfNum+1;
  }

  for (i=start;i<end;i++)
  {

    /* if the interface has been acquired, don't implement the change */
    /* Besides, acquired interfaces will already be operationally disabled */
    if (garpIntfAcquiredGet(i, &aqstatus) == L7_SUCCESS)
    {
        if (aqstatus == L7_TRUE)
        {
            continue;
        }

        cmd->port = i;
        gvr_proc_command(gvrp_app, (void*)cmd);
    }


  }

  return(L7_SUCCESS);
}


/*********************************************************************
* @purpose  Enable or Disable GMRP for an interface
*
* @param (in) intIfNum  physical or logical interface to be configured
*                       or L7_ALL_INTERFACES to apply configuration
*                       to all interfaces
* @param (in) status    L7_ENABLE  or L7_DISABLE
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes    This routine causes memory to be allocated for a VLAN and port, if a non-null
*           VLAN ID is passed. Thus, it must only be invoke with a non-null VLAN ID if the
*           vlan is needed for a valid GMR registration.  Otherwise, memory is wasted.
*
* @end
*********************************************************************/
L7_RC_t garpIntfGMRPModeProcess(L7_uint32 intIfNum, L7_uint32 status, GARPCommand *cmd)
{
  L7_uint32 i = 0;
  L7_uint32 vlanId, nextVlanId;
  L7_uint32 aqstatus;
  L7_uint32 index;
  L7_uint32 gmrpMode;
  NIM_INTF_MASK_t maskBuf, reverseMaskBuf;
  L7_uint32  maskSize, numPorts;
  L7_uint32 intfList[L7_MAX_INTERFACE_COUNT + 1];

  if ((intIfNum > L7_ALL_INTERFACES) || (intIfNum < 1))
  {
    return L7_NOT_EXISTS;
  }

  vlanId = 0;
  while(dot1qNextVlanGet(vlanId, &nextVlanId) == L7_SUCCESS)
  {

    cmd->vlan_id = nextVlanId;

    if(intIfNum == L7_ALL_INTERFACES)
    {

      memset((L7_uchar8 *)&reverseMaskBuf, 0, sizeof(reverseMaskBuf));
      if (dot1qVlanCurrentEgressPortsGet(cmd->vlan_id,
            (NIM_INTF_MASK_t *)&reverseMaskBuf, &maskSize) != L7_SUCCESS)
        return L7_FAILURE;

      /* Since dot1q returns the mask reversed for SNMP, we need to reverse it again */
      memset((L7_uchar8 *)&maskBuf, 0, sizeof(maskBuf));
      nimReverseMask(reverseMaskBuf, &maskBuf);

      /* Convert mask to a list of interfaces */
      memset((L7_uchar8 *)intfList, 0, sizeof(intfList));
      if (nimMaskToList(&maskBuf, intfList, &numPorts) != L7_SUCCESS)
        return L7_FAILURE;

      for (i=0; i <numPorts; i++)
      {

        /* Besides, acquired interfaces will already be operationally disabled */
        if (garpIntfAcquiredGet(intfList[i], &aqstatus) == L7_SUCCESS)
        {
          if (aqstatus  == L7_TRUE)
          {
            continue;
          }

      }

      /* If enabling GMRP on a port, do NOT automatically allocate
         resource to VLANs. These resources are only to be applied
         when a GMRP pdu is arrives associated with a VLAN.

         If disabling GMRP on a port, release all resources.

       */

        if(status == L7_ENABLE)
      {
          cmd->port = intfList[i];

          /* Enable GMRP in DTL */
          (void)dtlDot1dGMRPEnable(intfList[i]);
          if (gmrpInstanceIndexFind(cmd->vlan_id,&index) == L7_SUCCESS)
          {
            gmr_proc_command((void*)cmd);
          }

        }
        else if(status == L7_DISABLE)
        {

          cmd->port = intfList[i];

          /* Disable GMRP in DTL */
          (void)dtlDot1dGMRPDisable(intfList[i]);

          if((garpGMRPEnableGet(&gmrpMode)== L7_SUCCESS) && (gmrpMode==L7_FALSE))
            return L7_SUCCESS;

          gmr_proc_command((void*)cmd);
        }
      }
    }

    else
    {
      /* Besides, acquired interfaces will already be operationally disabled */
      if (intIfNum > L7_MAX_INTERFACE_COUNT)
      {
        return L7_NOT_EXISTS;
      }

      if (garpIntfAcquiredGet(intIfNum, &aqstatus) == L7_SUCCESS)
      {
        if (aqstatus  == L7_TRUE)
        {
          return L7_FAILURE;
        }

      }

      if ( status == L7_ENABLE)
      {

        cmd->port = intIfNum;

        (void)dtlDot1dGMRPEnable(intIfNum);
        if (gmrpInstanceIndexFind(cmd->vlan_id,&index) == L7_SUCCESS)
        {
          gmr_proc_command((void*)cmd);
        }

      }
      else if(status == L7_DISABLE)
      {

        cmd->port = intIfNum;

        (void)dtlDot1dGMRPDisable(intIfNum);

          /* if gmrp mode is disabled, the leaveall timer expiry should not take effect */
          if ((garpGMRPEnableGet(&gmrpMode) == L7_SUCCESS) && (gmrpMode==L7_FALSE))
             return L7_SUCCESS;

        gmr_proc_command((void*)cmd);
      }
    }

    vlanId = nextVlanId;
  }
  return(L7_SUCCESS);
}



/*********************************************************************
* @purpose Process generic attributes
*
* @param    *cmd @b{(input)} Pointer to GARPCommand structure
*
* @returns  void
*
* @notes    Invoked for L7_DETACH
*
* @end
*********************************************************************/
void garpGenericAttributeProcess(GARPCommand  *cmd)
{

    L7_uint32 vlanId;
    L7_uint32 nextVlanId;

    if (cmd->app == GARP_GVRP_APP)
    {
      gvr_proc_command(gvrp_app, (void*)cmd);
    }
    else if (cmd->app == GARP_GMRP_APP)
    {
      if(cmd->vlan_id != L7_NULL)
      {
        gmr_proc_command((void*)cmd);
      }
      else
      {
        /* this command needs to run on every instance of gmrp */
        vlanId = 0;
        while(dot1qNextVlanGet(vlanId, &nextVlanId) == L7_SUCCESS)
        {
          cmd->vlan_id = nextVlanId;
          gmr_proc_command((void*)cmd);
          vlanId = nextVlanId;
        }
      }
    }

}


/*********************************************************************
* @purpose  Apply GARP application Mode
*
* @param    application  @b{(input)} GARP_GVRP_APP or  GARP_GMRP_APP
* @param    mode         @b{(input)} GARP_ENABLE or GARP_DISABLE
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Invoked for L7_DETACH
*
* @end
*********************************************************************/
L7_RC_t garpGarpModeProcess(GARPApplication application, L7_uint32 mode)
{
  L7_uint32 index;
  NIM_INTF_MASK_t maskBuf, reverseMaskBuf;
  L7_uint32  maskSize, numPorts;
  L7_uint32 intfList[L7_MAX_INTERFACE_COUNT + 1];
  L7_uint32 status;
  GARPCommand cmd;
  L7_uint32 aqstatus,i;

    /* Validity checks */
    if ( (application != GARP_GVRP_APP ) && (application != GARP_GMRP_APP) )
    {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_GARP_COMPONENT_ID,
                "unknown application: %d\n", application);
        return L7_FAILURE;
    }

    if ( (mode != GARP_ENABLE ) && (mode != GARP_DISABLE) )
    {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_GARP_COMPONENT_ID,
                "unknown mode: %d\n", mode);
        return L7_FAILURE;
    }


    /* Apply mode change */
    if (mode == GARP_ENABLE)
    {
        if (garpTimerActivateProcess() != L7_SUCCESS)
        {
            /* Error is logged inside garpGarpTimerActivateProcess */
            return L7_FAILURE;
        }
        if ((application == GARP_GVRP_APP) && (GARP_GVRP_IS_ENABLED == L7_FALSE))
        {
          GARP_GVRP_IS_ENABLED = L7_TRUE;
          setGvrpApp(L7_TRUE);
        }
        else if ((application == GARP_GMRP_APP) && (GARP_GMRP_IS_ENABLED == L7_FALSE))
        {
          GARP_GMRP_IS_ENABLED = L7_TRUE;
          setGmrpApp(L7_TRUE);
        }

        memset((L7_uchar8 *)&reverseMaskBuf, 0, sizeof(reverseMaskBuf));

        if (dot1qVlanCurrentEgressPortsGet(L7_DOT1Q_DEFAULT_VLAN,
              (NIM_INTF_MASK_t *)&reverseMaskBuf, &maskSize)
            != L7_SUCCESS)
          return L7_FAILURE;

        /* Since dot1q returns the mask reversed for SNMP, we need to
         * reverse it again */
        memset((L7_uchar8 *)&maskBuf, 0, sizeof(maskBuf));
        nimReverseMask(reverseMaskBuf, &maskBuf);

        /* Convert mask to a list of interfaces */
        memset((L7_uchar8 *)intfList, 0,sizeof(intfList));
        if (nimMaskToList(&maskBuf, intfList,&numPorts) != L7_SUCCESS)
          return L7_FAILURE;

        for (i=0; i <numPorts; i++)
        {
          /* Besides,acquired  interfaces will already be
           * operationally disabled */
          if(garpIntfAcquiredGet(intfList[i],&aqstatus)== L7_SUCCESS)
          {
            if(aqstatus == L7_TRUE)
            {
              continue;
            }

          }
          garpQportsGMRPCfgGet(intfList[i], &status);
          if(status==L7_ENABLE)
          {
            (void)dtlDot1dGMRPEnable(intfList[i]);
            if (gmrpInstanceIndexFind(L7_DOT1Q_DEFAULT_VLAN,&index) == L7_SUCCESS)
            {
              if(GarpBuildCommand(&cmd,GARP_GMRP_APP,GARP_ENABLE_PORT,
                    GARP_GMRP_ATTRIBUTE,L7_NULL,
                    intfList[i],L7_DOT1Q_DEFAULT_VLAN)==  GARP_SUCCESS)
              {
                gmr_proc_command(&cmd);
              }
            }
          }

        }
    } /* mode == GARP_ENABLE */
    else
    if (mode == GARP_DISABLE)
    {

/*        if ((cmd->app == GARP_GVRP_APP) && (GARP_GVRP_IS_ENABLED == L7_TRUE))*/
          if (GARP_GVRP_APP == application)
        {
            /* remove all VLAN entries from GVRP database */
            Remove_All_GVRP_Entries();

            GARP_GVRP_IS_ENABLED = L7_FALSE;
            setGvrpApp(L7_FALSE);
            if (garpTimerDeactivateProcess(application) != L7_SUCCESS)
            {
              /* Error is logged inside garpGarpTimerActivateProcess */
              return L7_FAILURE;
            }
        }
/*        else if ((cmd->app == GARP_GMRP_APP) && (GARP_GMRP_IS_ENABLED == L7_TRUE))*/
        else if (GARP_GMRP_APP == application)
        {
            /* remove all instances of GMRP on all ports */
            Remove_All_GMRP_Instances();

            GARP_GMRP_IS_ENABLED = L7_FALSE;
            setGmrpApp(L7_FALSE);


            /* We don't call the garpTimerDeactivateProcess here because
             * Remove_All_GMRP_Instances eventually calls gmr_destroy_gmr which eventually calls
             * gid_gestroy_gid which removes all the running timers for gmrp
             */
        }


    } /* mode == GARP_DISABLE */

    return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Process Join Time modification
*
* @param (in) intIfNum  intIfNum
* @param (in) join_time join time value
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void garpJoinTimeProcess(L7_uint32 intIfNum, L7_uint32 join_time)
{

    L7_uint32  nextVlanId, vlanId, index;
    Gid        *gmrpGid;
    Gmr        *gmrpApp;

    setJoinTime(gvrp_app->g.gid, intIfNum, join_time);

    /* this command needs to run on Every instance of gmrp */
    vlanId = 0;
    while(dot1qNextVlanGet(vlanId,&nextVlanId) == L7_SUCCESS)
    {
      if (gmrpInstanceIndexFind(nextVlanId, &index) != L7_SUCCESS)
        break;

      gmrpApp = gmrpInstance[index].gmr;
      gmrpGid = gmrpApp->g.gid;
      setJoinTime(gmrpGid, intIfNum, join_time);
      vlanId = nextVlanId;
    }
}



/*********************************************************************
* @purpose  Process Leave Time modification
*
* @param (in) intIfNum  intIfNum
* @param (in) join_time join time value
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void garpLeaveTimeProcess(L7_uint32 intIfNum, L7_uint32 leave_time)
{

    L7_uint32  nextVlanId, vlanId, index;
    Gid        *gmrpGid;
    Gmr        *gmrpApp;

    setLeaveTime(gvrp_app->g.gid, intIfNum, leave_time);


    /* this command needs to run on every instance of gmrp */
    vlanId = 0;
    while(dot1qNextVlanGet(vlanId,&nextVlanId) == L7_SUCCESS)
    {
      if (gmrpInstanceIndexFind(nextVlanId, &index) != L7_SUCCESS)
        break;

      gmrpApp = gmrpInstance[index].gmr;
      gmrpGid = gmrpApp->g.gid;
      setLeaveTime(gmrpGid, intIfNum, leave_time);
      vlanId = nextVlanId;
    }
}



/*********************************************************************
* @purpose  Process Leave All Time modification
*
* @param (in) intIfNum  intIfNum
* @param (in) join_time join time value
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void garpLeaveAllTimeProcess(L7_uint32 intIfNum, L7_uint32 leaveall_time)
{

    L7_uint32  nextVlanId, vlanId, index;
    Gid        *gmrpGid;
    Gmr        *gmrpApp;

    setLeaveAllTime(gvrp_app->g.gid, intIfNum, leaveall_time);

    /* this command needs to run on every instance of gmrp */
    vlanId = 0;
    while(dot1qNextVlanGet(vlanId,&nextVlanId) == L7_SUCCESS)
    {
      if (gmrpInstanceIndexFind(nextVlanId, &index) != L7_SUCCESS)
        break;

      gmrpApp = gmrpInstance[index].gmr;
      gmrpGid = gmrpApp->g.gid;
      setLeaveAllTime(gmrpGid, intIfNum, leaveall_time);
      vlanId = nextVlanId;
    }
}



/*********************************************************************
* @purpose  Process VLAN add notification
*
* @param (in) vlanId  VLAN ID
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void garpVlanAddProcess(L7_uint32 vlanId)
{

    GARPCommand cmd;
    L7_uint32            i, type = 0;
    L7_RC_t              return_value = L7_FAILURE;
    L7_uint32 index = 0;

    if ((L7_TRUE != nimPhaseStatusCheck()) ||
        (L7_TRUE != garpGetGvrpEnabled()))
    {
        /* Silent return if GVRP is not enabled. In fact, previous checks
           should have taken care of this */
        return;
    }

    return_value = dot1qVlanStatusGet(vlanId, &type);

   /* get the type of a newly-created VLAN; it
    * should be either Default or Static */
    if ((L7_SUCCESS != return_value) || (L7_DOT1Q_DYNAMIC == type))
    {
        /* Dynamic VLANs are not added again */
        return;
    }

    for (i=1; i < L7_ALL_INTERFACES; i++)
    {
        if (L7_SUCCESS == nimCheckIfNumber(i))
        {
            if (L7_TRUE == garpGVRPIntfEnableGet(i))
            {

                if ( GarpBuildCommand(&cmd, GARP_GVRP_APP, GARP_NORMAL_REGISTRATION,
                                      GARP_GVRP_VLAN_ATTRIBUTE, (GARPAttrValue)(&vlanId),
                                      i, L7_NULL) == GARP_SUCCESS)
                {
                    gvr_proc_command(gvrp_app, &cmd);
                }

            }
        }
    }

    if(vlanId == L7_DOT1Q_DEFAULT_VLAN)
    {
      if(L7_FAILURE == gmrpInstanceIndexFind(vlanId, &index))
      {
        gmrpCreateGmr(vlanId);
        }
    }

    return;
}




/*********************************************************************
* @purpose  Process VLAN Port Change notification
*
* @param (in) intIfNum  internal interface number
* @param (in) vlanId    VLAN ID
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t garpVlanPortChangeProcess(L7_uint32 intIfNum, L7_uint32 vlanId)
{
    L7_uint32            type, mode;
    L7_RC_t              return_value;

    GARPCommand cmd;
    GARPCommandNameTable command;

    type = 0;
    mode = 0;
    return_value = L7_FAILURE;

    if ((L7_TRUE != nimPhaseStatusCheck()) &&
        (L7_TRUE != garpGetGvrpEnabled()))
    {
        return return_value;
    }

    if (L7_SUCCESS == nimCheckIfNumber(intIfNum))
    {
        if (L7_TRUE == garpGVRPIntfEnableGet(intIfNum))
            {
                if (L7_SUCCESS == dot1qVlanMemberAndTypeGet(vlanId,
                                                        intIfNum, &mode, &type))
                {
                    if(type == L7_DOT1Q_TYPE_DOT1X_REGISTERED) 
                       mode = L7_DOT1Q_NORMAL_REGISTRATION;

                    switch (mode)
                    {
                        case L7_DOT1Q_FIXED:
                            command = GARP_REGISTRATION_FIXED;
                            break;

                        case L7_DOT1Q_FORBIDDEN:
                            command = GARP_REGISTRATION_FORBIDDEN;
                            break;

                        case L7_DOT1Q_NORMAL_REGISTRATION:
                            command = GARP_NORMAL_REGISTRATION;
                            break;

                        default:
                            command = GARP_UNDEFINED_COMMAND;
                            break;
                    }

                    if ((L7_DOT1Q_DYNAMIC != type) &&
                        (GARP_UNDEFINED_COMMAND != command))
                    {


                        if ( GarpBuildCommand(&cmd, GARP_GVRP_APP, command,
                                              GARP_GVRP_VLAN_ATTRIBUTE,
                                          (GARPAttrValue)(&vlanId),intIfNum,
                                              L7_NULL) == GARP_SUCCESS)
                        {
                            gvr_proc_command(gvrp_app, &cmd);
                        }
                    }

                } /* if (L7_SUCCESS == dot1qVlanMemberAndTypeGet */

        } /* if (L7_TRUE == garpGVRPIntfEnableGet(intIfNum)) */

    } /* if (L7_SUCCESS == nimCheckIfNumber(intIfNum)) */


  return L7_SUCCESS;
}


