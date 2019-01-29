/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename vrrp_main.c
*
* @purpose vrrp main implementation
*
* @component vrrp
*
* @comments none
*
* @create 10/15/2001
*
* @author Kavleen Kaur et. al.
*
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/

#define L7_MAC_ENET_BCAST                    /* For broadcast address */

#include "l7_common.h"

#include "nimapi.h"

#include "l3_commdefs.h"
#include "l3_comm_structs.h"
#include "default_cnfgr.h"
#include "l3_default_cnfgr.h"

#include "l7_vrrp_api.h"
#include "vrrp_config.h"

#include "l7_vrrpinclude.h"

#include "sysnet_api.h"
#include "sysnet_api_ipv4.h"
#include "vrrp_util.h"
#include "vrrptask.h"

#include "vrrp_trap_api.h"
#include "trap_layer3_api.h"

#include "l7_ip_api.h"
#include "l3end_api.h"

#define VRRP_IP_PKT_MIN_LEN ( L7_IP_HDR_LEN + VRRP_MIN_PACKET_LENGTH )

#define VRRP_MAX_ERR_LOG_COUNT  500
#define VRRP_LOG_MOD 100

void  *vrrp_Queue;
L7_uchar8 vrrpMulticastAddr[6] = { 0x01,0x00,0x5E,0x00,0x00,0x12};
L7_uint32 vrrpPktLen = VRRP_PACKET_LENGTH;

static L7_uchar8 vrrpTempBuff[VRRP_PACKET_LENGTH] = {0};

/* message queue overflow debug counter */
L7_uint32 vrrpQueueOverflowCtr = 0;
L7_uint32 vrrpQueueIntfChngOverflowCtr = 0;
L7_uint32 vrrpQueueRtEventChngOverflowCtr = 0;
L7_uint32 vrrpBufferPoolId = 0;

osapiTimerDescr_t   *pVRRPAdTimerHolder;
osapiTimerDescr_t   *pVRRPMasterTimerHolder;

extern vrrpRouterBasicConfiguration_t  vrrpBasicConfig_g;
extern void      *VrrpConfigSema;
extern void vrrpDebugPacketRxTrace(L7_uint32 intIfNum, L7_uchar8* pdu);
extern void vrrpDebugPacketTxTrace(L7_uint32 intIfNum, L7_uchar8 *vrrp_buff);


/*********************************************************************
*
* @purpose task to handle all vrrp messages
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

void VRRPDaemonTask()
{
  vrrpMsg_t      msg;


  osapiTaskInitDone(L7_VRRP_TASK_SYNC);

  /* Task Main Body */
  do
  {
    memset(&msg,0x00,sizeof(vrrpMsg_t));
    (void)osapiMessageReceive(vrrp_Queue, (void *)&msg, VRRP_MSG_SIZE, L7_WAIT_FOREVER);
    switch (msg.msg_id)
    {
    case vrrpMsgCnfgr:
        vrrpCnfgrParse(&msg.type.cnfgrCmdData);
        break;
    case (vrrpMsgAdvtTimerTick):
      VRRPAdvertisementSend();
      break;

    case (vrrpMsgMasterCheckTimerTick):
      VRRPMasterTimeCheck();
      break;

    case (vrrpMsgPacketRecv):
      (void)vrrpAdvtPacketProcess(&msg);
      break;

    case (vrrpMsgRtrEvent):
      (void)vrrpMapRoutingEventChangeProcess(msg.type.rtrEvent.intIfNum,
                                            msg.type.rtrEvent.event,
                                            msg.type.rtrEvent.pData,
                                            &(msg.type.rtrEvent.response));
      break;

    case vrrpMsgRestoreEvent:
        (void)vrrpRestoreProcess();
        break;

    case vrrpMsgBestRouteEvent:
        (void)vrrpMapBestRoutesGet();
        break;
    default:
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VRRP_MAP_COMPONENT_ID,
              "vrrpRecvTask: invalid mesage type. %s:%d\n", __FILE__, __LINE__);
      break;
    }
  }while (1);

}




/*********************************************************************
*
* @purpose task to handle sending the appropriate VRRP messages
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
void VRRPAdvertTimerCheck( )
{
    static L7_BOOL logMsgDisplayed = L7_FALSE;
  L7_int32 rc;
  vrrpMsg_t      msg;
    L7_BOOL adminMode;

    if ( (L7_vrrpAdminModeGet(&adminMode) == L7_SUCCESS) && (adminMode == L7_TRUE) )
    {
        if (!vrrp_Queue)
        {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VRRP_MAP_COMPONENT_ID,
                    "error  in VRRPAdvertTimerCheck() \n");
        }
        msg.msg_id = vrrpMsgAdvtTimerTick;

        rc = osapiMessageSend( vrrp_Queue, &msg, VRRP_MSG_SIZE, L7_NO_WAIT, L7_MSG_PRIORITY_NORM );

        if (rc != L7_SUCCESS)
        {
          if (logMsgDisplayed == L7_FALSE)
          {
            logMsgDisplayed = L7_TRUE;
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VRRP_MAP_COMPONENT_ID,
                    "Advt Timer Tick send failed (one or more occurrences).\n");
          }
          vrrpQueueOverflowCtr++;
        }
  }
  osapiTimerAdd ( (void*)VRRPAdvertTimerCheck,L7_NULL,L7_NULL,ADVT_SEND_TIMER_INTERVAL,&pVRRPAdTimerHolder );

}




/*********************************************************************
*
* @purpose task to handle sending the appropriate VRRP messages
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
void VRRPMasterCheck()
{
    static L7_BOOL logMsgDisplayed = L7_FALSE;
  vrrpMsg_t   msg;
  L7_int32 rc;
  L7_BOOL adminMode;

    if ( (L7_vrrpAdminModeGet(&adminMode) == L7_SUCCESS) && (adminMode == L7_TRUE) )
    {
        if (!vrrp_Queue)
        {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VRRP_MAP_COMPONENT_ID,
                    "error  in VRRPMasterCheck() \n");
        }
        msg.msg_id = vrrpMsgMasterCheckTimerTick;
        rc = osapiMessageSend( vrrp_Queue, &msg, VRRP_MSG_SIZE, L7_NO_WAIT, L7_MSG_PRIORITY_NORM );
        if (rc != L7_SUCCESS)
        {
          if (logMsgDisplayed == L7_FALSE)
          {
            logMsgDisplayed = L7_TRUE;
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VRRP_MAP_COMPONENT_ID,
                    "Master Check Timer Tick send failed (one or more occurrences).\n");
          }
          vrrpQueueOverflowCtr++;
        }
  }
  osapiTimerAdd ( (void*)VRRPMasterCheck,L7_NULL,L7_NULL,MASTER_CHECK_TIMER_INTERVAL,&pVRRPMasterTimerHolder );
}


/*********************************************************************
*
* @purpose for sending VRRP messages
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
L7_RC_t VRRPAdvertisementSend()
{
  L7_uchar8 nextVrId,vrID;
  L7_int32 checkAvailability;
  L7_uint32 nextInfNum,infNum;
  vrrpRouterInfo_t* routerInfo;
  L7_clocktime    time;
  L7_int32    secs,nanosecs;
  L7_double64  millisecs_since_lastAdvtSent;
  L7_uint32 i;
  L7_uint32 secsSinceLastGratArp;


  if (L7_vrrpFirstGet(&vrID,&infNum)==L7_FAILURE)
  {
    /* Return success, even though there are no virtual routers configured */
    return L7_SUCCESS;
  }
  do
  {
    osapiSemaTake(VrrpConfigSema, L7_WAIT_FOREVER);
    if (L7_NULLPTR != (routerInfo = (vrrpRouterInfo_t*)
                       L7_vrrpVirtualRouterFetch(vrID,infNum)))
    {
      if ((L7_VRRP_STATE_MASTER == routerInfo->vrrpVirtRouterOperInfo.vr_state))
      {
        osapiClockTimeRaw (&time);
        secs = routerInfo->vrrpVirtRouterOperInfo.lastAdvt_Sendtime.seconds;
        nanosecs = routerInfo->vrrpVirtRouterOperInfo.lastAdvt_Sendtime.nanoseconds;
        millisecs_since_lastAdvtSent = (time.seconds*SECONDS_FACTOR+time.nanoseconds/NANOSECONDS_FACTOR)-
                                       (secs* SECONDS_FACTOR + nanosecs/NANOSECONDS_FACTOR);

        if ( millisecs_since_lastAdvtSent >= routerInfo->vrrpCfgInfo.adver_int*SECONDS_FACTOR)
        {
          if (vrrpAdvtPacketSend(routerInfo,0)==L7_FAILURE)
          {
            osapiSemaGive(VrrpConfigSema);
            return L7_FAILURE;
          }

          routerInfo->vrrpVirtRouterOperInfo.lastAdvt_Sendtime.seconds = time.seconds;
          routerInfo->vrrpVirtRouterOperInfo.lastAdvt_Sendtime.nanoseconds= time.nanoseconds;
        }

        /* The master sends periodic grat ARPs to aid station learning. See RFC 3768
         * section 2.4. */
        secsSinceLastGratArp = time.seconds - routerInfo->vrrpVirtRouterOperInfo.lastGratArpTime;
        if (secsSinceLastGratArp > VRRP_GRAT_ARP_INTERVAL)
        {
          routerInfo->vrrpVirtRouterOperInfo.lastGratArpTime = time.seconds;
          for (i = 0; i< L7_L3_NUM_IP_ADDRS; i++)
          {
            if (routerInfo->vrrpCfgInfo.ipaddress[i] != FD_VRRP_DEFAULT_IP_ADDRESS)
            {
              vrrpGratuitousARPSend(routerInfo->vrrpVirtRouterOperInfo.vmac,
                                    routerInfo->vrrpCfgInfo.ipaddress[i],
                                    infNum);
            }
          }
        }
      }
    }
    osapiSemaGive(VrrpConfigSema);
    checkAvailability=L7_vrrpNextGet(vrID,infNum,&nextVrId,&nextInfNum);
    vrID = nextVrId;
    infNum = nextInfNum;

  }while (checkAvailability==L7_SUCCESS);

  return(L7_SUCCESS);
}



/*********************************************************************
*
* @purpose for checking Virtual router master state
*
* @param void
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t VRRPMasterTimeCheck()
{
  L7_uchar8 nextVrId, vrID;
  L7_int32 checkAvailability;
  L7_uint32 nextInfNum,infNum;
  L7_double64 skewTime;
  vrrpRouterInfo_t* routerInfo ;
  L7_clocktime    time, vr_uptime;
  L7_double64     lastPacket_time, elapsed_upTime,preemptdelay_time;
  L7_BOOL preempt_state ;

  if (L7_vrrpFirstGet(&vrID,&infNum)==L7_FAILURE)
  {
    /* Return success, even though there are no virtual routers configured */
    return L7_SUCCESS;
  }
  do
  {
    osapiSemaTake(VrrpConfigSema, L7_WAIT_FOREVER);
    preempt_state = L7_FALSE;

    if (L7_NULL != (routerInfo = (vrrpRouterInfo_t*)L7_vrrpVirtualRouterFetch(vrID,infNum)))
    {
      if (L7_VRRP_STATE_BACKUP == routerInfo->vrrpVirtRouterOperInfo.vr_state)
      {
        skewTime= ((256 - (L7_double64)(routerInfo->vrrpVirtRouterOperInfo.oper_priority))/256)*SECONDS_FACTOR;
        osapiClockTimeRaw (&time);

        vr_uptime=routerInfo->vrrpVirtRouterOperInfo.vrrp_uptime;
        lastPacket_time = (time.seconds*SECONDS_FACTOR+time.nanoseconds/NANOSECONDS_FACTOR) -
                          (routerInfo->vrrpVirtRouterOperInfo.last_packet_recvtime.seconds*SECONDS_FACTOR+
                           routerInfo->vrrpVirtRouterOperInfo.last_packet_recvtime.nanoseconds/NANOSECONDS_FACTOR);
        if(routerInfo->vrrpVirtRouterOperInfo.preempt_startTime.seconds != 0)
        {
         preemptdelay_time = (time.seconds*SECONDS_FACTOR+time.nanoseconds/NANOSECONDS_FACTOR) -
                        (routerInfo->vrrpVirtRouterOperInfo.preempt_startTime.seconds*SECONDS_FACTOR+
                         routerInfo->vrrpVirtRouterOperInfo.preempt_startTime.nanoseconds/NANOSECONDS_FACTOR);
         if(preemptdelay_time > (L7_double64)((routerInfo->vrrpCfgInfo.preempt_delay)*SECONDS_FACTOR))
         {
          preempt_state = L7_TRUE;
          routerInfo->vrrpVirtRouterOperInfo.preempt_startTime.seconds = 0;
          routerInfo->vrrpVirtRouterOperInfo.preempt_startTime.nanoseconds = 0;
         }
        }
        if (routerInfo->vrrpVirtRouterOperInfo.zerp_priority_pkt_rcvd_flag==L7_TRUE)
        {
          if ( lastPacket_time > skewTime)
          {
            (routerInfo->vrrpVirtRouterOperInfo.num_time_become_master)++;

            routerInfo->vrrpVirtRouterOperInfo.zerp_priority_pkt_rcvd_flag=L7_FALSE;

            if (VRRPTransitToMaster(routerInfo, routerInfo->vrrpVirtRouterOperInfo.vmac,
                                    infNum) != L7_SUCCESS)
            {
              osapiSemaGive(VrrpConfigSema);
              return L7_FAILURE;
            }
          }
        }
        else
        {
          elapsed_upTime = (time.seconds*SECONDS_FACTOR + time.nanoseconds/NANOSECONDS_FACTOR) -
                           (vr_uptime.seconds*SECONDS_FACTOR+vr_uptime.nanoseconds/NANOSECONDS_FACTOR);

          if (((routerInfo->vrrpVirtRouterOperInfo.last_packet_recvtime.seconds == 0) &&
               (elapsed_upTime> (L7_double64)(3*(routerInfo->vrrpCfgInfo.adver_int)*SECONDS_FACTOR + skewTime)))
              ||
              ((routerInfo->vrrpVirtRouterOperInfo.last_packet_recvtime.seconds != 0) &&
               (lastPacket_time > (L7_double64)(3*(routerInfo->vrrpVirtRouterOperInfo.master_advt_int)*SECONDS_FACTOR + skewTime)))

              ||
              (preempt_state == L7_TRUE))
          {
            (routerInfo->vrrpVirtRouterOperInfo.num_time_become_master)++;

            if (VRRPTransitToMaster(routerInfo,routerInfo->vrrpVirtRouterOperInfo.vmac,
                                    infNum) != L7_SUCCESS)
            {
              osapiSemaGive(VrrpConfigSema);
              return L7_FAILURE;
            }
          }
        }
      }
    }

    osapiSemaGive(VrrpConfigSema);
    checkAvailability=L7_vrrpNextGet(vrID,infNum,&nextVrId,&nextInfNum);
    vrID = nextVrId;
    infNum = nextInfNum;


  }
  while (checkAvailability == L7_SUCCESS);

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose display current VRRP message queue overflow counter
*
* @param void
*
* @returns void
*
* @notes for engineering debug use only
*
* @end
*
*********************************************************************/
void vrrpQueueOverflowShow(void)
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "VRRP_queue overflow count = %u\n", vrrpQueueOverflowCtr);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "VRRP_queue intf change overflow count = %u\n", vrrpQueueIntfChngOverflowCtr);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "VRRP_queue routing event change overflow count = %u\n", vrrpQueueRtEventChngOverflowCtr);
}

/*********************************************************************
* @purpose  To compute the check sum of the packet
* @param    addr    @b{(input)}      Pointer to the data on which check sum needs to be computed
* @param    len     @b{(input)}      Length of the data
* @param    csum    @b{(input)}      csum field
* @returns  L7_ushort16              computed check sum

* @notes    none
*
* @end
*********************************************************************/

L7_ushort16 vrrpCheckSum( L7_ushort16 *addr, L7_ushort16 len, L7_ushort16 csum)
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
    sum += osapiHtons(*(L7_uchar8 *)w << 8);

  sum = (sum >> 16) + (sum & 0xffff);     /* add hi 16 to low 16 */
  sum += (sum >> 16);                     /* add carry */
  answer = ~sum;                          /* truncate to 16 bits */

  return(answer);
}





/*********************************************************************
* @purpose  To build IP Packet
* @param    routerInfo    @b{(input)}      Pointer to the vrrp structure
* @param    vrrp_buff     @b{(input)}      data buffer
* @returns  L7_SUCCESS or L7_FAILURE

* @notes    none
*
* @end
*********************************************************************/
L7_RC_t vrrpDataBuild(vrrpRouterInfo_t* routerInfo,L7_uchar8* vrrp_buff,int zeroPriorityFlag)
{
  vrrpPkt_t *packet = (vrrpPkt_t *)vrrp_buff;
  L7_uint32 numCfgIpAddr = 0;
  L7_uint32 count, length;


  packet->version_type = (VRRP_PROTO_VERSION<<4) | VRRP_PKT_ADV;
  packet->vrid         = routerInfo->vrrpCfgInfo.vrid;
  if (zeroPriorityFlag == 0)
    packet->priority     = routerInfo->vrrpVirtRouterOperInfo.oper_priority;
  else
    packet->priority     = 0;

  packet->auth_type  = (L7_uchar8)routerInfo->vrrpCfgInfo.auth_types;/* enum converted to char*/
  packet->adver_int    = routerInfo->vrrpCfgInfo.adver_int;

  for (count = 0; count < L7_L3_NUM_IP_ADDRS; count++)
  {
    if ( routerInfo->vrrpCfgInfo.ipaddress[count] != L7_NULL_IP_ADDR)
    {
      packet->ipaddress[numCfgIpAddr] = routerInfo->vrrpCfgInfo.ipaddress[count];
      numCfgIpAddr = numCfgIpAddr + 1;
    }
  }

  packet->naddr  = numCfgIpAddr;

  if (packet->auth_type == L7_AUTH_TYPE_SIMPLE_PASSWORD)
  {
    bzero((char *)packet->auth_data,VRRP_AUTH_DATA_LEN);
    /* Copy the authentication data at the end of IP address list */
    memcpy((L7_uchar8*)&(packet->ipaddress[numCfgIpAddr]), routerInfo->vrrpCfgInfo.auth_data,VRRP_AUTH_DATA_LEN);
  }
  length = sizeof(vrrpPkt_t) - (sizeof(L7_IP_ADDR_t) * (L7_L3_NUM_IP_ADDRS - numCfgIpAddr));
  packet->chksum   = 0;
  packet->chksum   = vrrpCheckSum( (L7_ushort16*)packet, length, 0);

  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  To build IP Packet
* @param    routerInfo    @b{(input)}      Pointer to the vrrp structure
* @param    vrrp_buff     @b{(input)}      data buffer
* @returns  L7_SUCCESS or L7_FAILURE

* @notes    none
*
* @end
*********************************************************************/
L7_RC_t vrrpIPBuild(vrrpRouterInfo_t* routerInfo,L7_uchar8* vrrp_buff)
{
  L7_uint32 length;
  L7_uint32 count, numCfgIpAddr, pktSize;
  static L7_uint32  iph_ident;
  L7_ipHeader_t * ip = (L7_ipHeader_t *)(vrrp_buff);
  iph_ident=0;

  ip->iph_versLen =(L7_IP_VERSION<<4) | L7_IP_HDR_VER_LEN ;
  length= L7_IP_HDR_LEN;
  ip->iph_tos = 0;
  ip->iph_ident = osapiHtons(++iph_ident);
  ip->iph_flags_frag  = 0;
  ip->iph_ttl = IP_VRRP_TTL;
  ip->iph_prot  = IP_PROT_VRRP;
  /* This is always the primary IP address on the local interface */
  ip->iph_src = osapiHtonl(routerInfo->vrrpVirtRouterOperInfo.localAddr.ipAddr);

  ip->iph_dst = osapiHtonl(L7_IP_VRRP_ADDR);
  ip->iph_csum = 0;

  numCfgIpAddr = 0;
  for (count = 0; count < L7_L3_NUM_IP_ADDRS; count++)
  {
    if ( routerInfo->vrrpCfgInfo.ipaddress[count] != L7_NULL_IP_ADDR)
    {
      numCfgIpAddr = numCfgIpAddr + 1;
    }
  }

  /* Calculate the packet size */
  /* Packet size = Maximum Pkt Size - ( Max Allowed Ip Addrs - Cfgr Ip Addrs) * sizeof(IPAddr))*/
  pktSize = VRRP_PACKET_LENGTH - (sizeof(L7_IP_ADDR_t) * (L7_L3_NUM_IP_ADDRS - numCfgIpAddr));

  ip->iph_len = pktSize;
  ip->iph_len = osapiHtons(ip->iph_len);

  ip->iph_csum  = vrrpCheckSum( (L7_ushort16*)ip, length, 0 );

  return L7_SUCCESS;
}




/*********************************************************************
* @purpose  To build whole VRRP Packet
* @param    routerInfo    @b{(input)}      Pointer to the vrrp structure
* @param    vrrp_buff     @b{(input)}      data buffer
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t vrrpPktBuild(vrrpRouterInfo_t* routerInfo,L7_uchar8* vrrp_buff,int zeroPriorityFlag)
{
  if (vrrpIPBuild(routerInfo,vrrp_buff)==L7_FAILURE)
  {
    return L7_FAILURE;
  }
  vrrp_buff += sizeof(L7_ipHeader_t);
  if (vrrpDataBuild(routerInfo,vrrp_buff,zeroPriorityFlag)==L7_FAILURE)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;

}

/*********************************************************************
*
* @purpose to handle Enqueuing of VRRP messages
*
* @param    bufHandle   @b{(input)} buffer handle to the bpdu received
* @param    intIfNum    @b{(input)} internal interface number
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t vrrpPacketProcess(L7_netBufHandle netBufHandle,L7_uint32 intIfNum)
{
  vrrpMsg_t msg;
  L7_int32 rc;
  L7_uchar8 *data;
  L7_uint32 dataLength;
  L7_uint32 pdu_data_offset;
  L7_uint32 pdu_data_len;
  static L7_uint32 vrrp_err_log_count = 0;

  bzero( (L7_uchar8 *)&msg, sizeof(msg));
  msg.msg_id = vrrpMsgPacketRecv;
  msg.type.intfNumPktRcvd = intIfNum;

  /* get the data start */
  SYSAPI_NET_MBUF_SET_LOC(netBufHandle, MBUF_LOC_VRRP_RX);
  SYSAPI_NET_MBUF_GET_DATASTART(netBufHandle, data);
  /* get the data length */
  SYSAPI_NET_MBUF_GET_DATALENGTH(netBufHandle, dataLength);

  pdu_data_offset = sysNetDataOffsetGet(data);

  pdu_data_len = dataLength - pdu_data_offset;

  /* Right now we support L7_L3_NUM_IP_ADDRS ( = 32 ) addresses that may be
   * configured on a routing interface. If we receive VRRP packets with
   * "Count of IP addrs" and the subseqent IP addresses more than 32,
   * we shall drop it */
  if ((pdu_data_len < VRRP_IP_PKT_MIN_LEN) || (pdu_data_len > VRRP_PACKET_LENGTH))
  {
    /* The error count variable is to avoid flooding of the LOG messages in case
     * of the packet flood, with the above check failing */
    if(vrrp_err_log_count == 0)
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_VRRP_MAP_COMPONENT_ID,
              "VRRP packet of size %d dropped. Min VRRP packet size is %d; "
              "max VRRP packet size is %d",
               pdu_data_len, VRRP_IP_PKT_MIN_LEN, VRRP_PACKET_LENGTH);

    vrrp_err_log_count++;
    if(vrrp_err_log_count == VRRP_MAX_ERR_LOG_COUNT)
      vrrp_err_log_count = 0;

    return L7_FAILURE;
  }

  rc = bufferPoolAllocate(vrrpBufferPoolId, &(msg.vrrpMessage));
  if (rc != L7_SUCCESS)
    return rc;

  memcpy(msg.vrrpMessage, (data+pdu_data_offset), pdu_data_len);
  msg.messageLen = pdu_data_len;

  rc = osapiMessageSend( vrrp_Queue, &msg, VRRP_MSG_SIZE, L7_NO_WAIT, L7_MSG_PRIORITY_NORM );
  if (rc != L7_SUCCESS)
  {
    bufferPoolFree(vrrpBufferPoolId, msg.vrrpMessage);
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VRRP_MAP_COMPONENT_ID,
            "VRRP Packet Enqueue failed.");
  }
  return rc;
}


/*********************************************************************
*
* @purpose to handle received VRRP messages
*
* @param    msg        @b{(input)}     Pointer to vrrpMsg_t structure
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t vrrpAdvtPacketProcess(vrrpMsg_t *msg)
{
  vrrpRouterInfo_t* routerInfo;
  L7_uchar8 priority;
  L7_ipHeader_t *ip_hdr;
  vrrpPkt_t  *vrrp_pkt;
  L7_uint32 intIfNum;
  L7_uint32 mode;
  L7_uchar8 *pVrrpTempBuff;

  L7_clocktime recv_time;
  /*------------------------------------*/
  /*   Process the buffer               */
  /*------------------------------------*/
  intIfNum  = msg->type.intfNumPktRcvd;


  /* reset the buffer */
  memset(vrrpTempBuff,0x00,VRRP_PACKET_LENGTH);

  /*  Get the data  */
  pVrrpTempBuff = (L7_uchar8 *)&vrrpTempBuff[0];
  memcpy(pVrrpTempBuff, msg->vrrpMessage, msg->messageLen);

  ip_hdr = (L7_ipHeader_t*)pVrrpTempBuff;
  vrrp_pkt = (vrrpPkt_t *)((L7_char8 *)ip_hdr + sizeof(L7_ipHeader_t));

  /* Free the buffer */
  bufferPoolFree(vrrpBufferPoolId, msg->vrrpMessage);


  /*----------------------------------------------*/
  /*   Check validity of handling the packet      */
  /*----------------------------------------------*/


  L7_vrrpAdminModeGet(&mode);
  if (mode != L7_TRUE)
  {
    return L7_FAILURE;
  }
  vrrpDebugPacketRxTrace( intIfNum, pVrrpTempBuff);

  if (vrrpPacketValidate(ip_hdr,vrrp_pkt,intIfNum,msg->messageLen) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(VrrpConfigSema, L7_WAIT_FOREVER);

  if (NULL!= (routerInfo =
              (vrrpRouterInfo_t*)L7_vrrpVirtualRouterFetch(vrrp_pkt->vrid,intIfNum)))
  {
    (routerInfo->vrrpVirtRouterOperInfo.advert_received)++;
    priority = routerInfo->vrrpVirtRouterOperInfo.oper_priority;

    osapiClockTimeRaw (&recv_time);

    /* If we are the default master for this VRID, don't process this advt packet */
    if (priority == 255)
    {
      /* Per RFC 2338, if we are master and we get a zero-priority
         VRRP advertisement, we need to send an advertisement and
         reset the advertisement timer */
      if (vrrp_pkt->priority == 0)
      {
        (routerInfo->vrrpVirtRouterOperInfo.pkt_rcvd_zero_priority)++;
        vrrpAdvtPacketSend(routerInfo, 0);
        routerInfo->vrrpVirtRouterOperInfo.lastAdvt_Sendtime.seconds = recv_time.seconds;
        routerInfo->vrrpVirtRouterOperInfo.lastAdvt_Sendtime.nanoseconds= recv_time.nanoseconds;
      }
      osapiSemaGive(VrrpConfigSema);
      return L7_FAILURE;
    }

    if (routerInfo->vrrpCfgInfo.preempt_mode == L7_TRUE)    /* prempt=TRUE */
    {
      if (routerInfo->vrrpVirtRouterOperInfo.vr_state == L7_VRRP_STATE_BACKUP)     /* BACKUP STATE */
      {
        /* If we get a zero-priority advertisement that's from the master,
         * update the last received time, set a flag and increment stat.
         */
        if (vrrp_pkt->priority == 0 &&
            osapiNtohl(ip_hdr->iph_src) == routerInfo->vrrpVirtRouterOperInfo.masterIpAddr)
        {
          routerInfo->vrrpVirtRouterOperInfo.last_packet_recvtime.seconds = recv_time.seconds;
          routerInfo->vrrpVirtRouterOperInfo.last_packet_recvtime.nanoseconds = recv_time.nanoseconds;
          routerInfo->vrrpVirtRouterOperInfo.zerp_priority_pkt_rcvd_flag = L7_TRUE;
          (routerInfo->vrrpVirtRouterOperInfo.pkt_rcvd_zero_priority)++;
        }
        else
        {
          if (vrrp_pkt->priority >= priority)
          {
            routerInfo->vrrpVirtRouterOperInfo.last_packet_recvtime.seconds = recv_time.seconds;
            routerInfo->vrrpVirtRouterOperInfo.last_packet_recvtime.nanoseconds = recv_time.nanoseconds;
            routerInfo->vrrpVirtRouterOperInfo.masterIpAddr = osapiNtohl(ip_hdr->iph_src);
            routerInfo->vrrpVirtRouterOperInfo.preempt_startTime.seconds = 0;
            routerInfo->vrrpVirtRouterOperInfo.preempt_startTime.nanoseconds = 0;
            if (routerInfo->vrrpCfgInfo.timer_learn == L7_TRUE)
            {
             routerInfo->vrrpVirtRouterOperInfo.master_advt_int = vrrp_pkt->adver_int;
            }
          }
          else
          {
           if(routerInfo->vrrpVirtRouterOperInfo.preempt_delay_config)
           {
            routerInfo->vrrpVirtRouterOperInfo.masterIpAddr = osapiNtohl(ip_hdr->iph_src);
            routerInfo->vrrpVirtRouterOperInfo.preempt_startTime.seconds = recv_time.seconds;
            routerInfo->vrrpVirtRouterOperInfo.preempt_startTime.nanoseconds = recv_time.nanoseconds;
           }
          }
        }
      }
      else /* MASTER STATE */
      {
        if ( (vrrp_pkt-> priority > priority) ||
             ((vrrp_pkt-> priority == priority)
              && (osapiNtohl(ip_hdr->iph_src) >
                  routerInfo->vrrpVirtRouterOperInfo.localAddr.ipAddr)))
        {
          routerInfo->vrrpVirtRouterOperInfo.last_packet_recvtime.seconds = recv_time.seconds;
          routerInfo->vrrpVirtRouterOperInfo.last_packet_recvtime.nanoseconds = recv_time.nanoseconds;
          routerInfo->vrrpVirtRouterOperInfo.masterIpAddr = osapiNtohl(ip_hdr->iph_src);

          if (routerInfo->vrrpCfgInfo.timer_learn == L7_TRUE)
          {
           routerInfo->vrrpVirtRouterOperInfo.master_advt_int = vrrp_pkt->adver_int;
          }

          if ( VRRPTransitToBackup(routerInfo,intIfNum) != L7_SUCCESS)
          {
            L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
            nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
            L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_VRRP_MAP_COMPONENT_ID,
                    "Failed to move to backup state for VR %d on interface %s.",
                    routerInfo->vrrpCfgInfo.vrid, ifName);
            osapiSemaGive(VrrpConfigSema);
            return L7_FAILURE;
          }
        }
        else
        {
          if (vrrp_pkt->priority == 0)
          {
            /* Per RFC 2338, if we are master and we get a zero-priority
               VRRP advertisement, we need to send an advertisement and
               reset the advertisement timer */
            (routerInfo->vrrpVirtRouterOperInfo.pkt_rcvd_zero_priority)++;
            vrrpAdvtPacketSend(routerInfo, 0);
            routerInfo->vrrpVirtRouterOperInfo.lastAdvt_Sendtime.seconds = recv_time.seconds;
            routerInfo->vrrpVirtRouterOperInfo.lastAdvt_Sendtime.nanoseconds= recv_time.nanoseconds;
          }
        }
      }
    }
    else    /* prempt=FALSE */
    {
      if (routerInfo->vrrpVirtRouterOperInfo.vr_state == L7_VRRP_STATE_BACKUP)
      {
        routerInfo->vrrpVirtRouterOperInfo.last_packet_recvtime.seconds = recv_time.seconds;
        routerInfo->vrrpVirtRouterOperInfo.last_packet_recvtime.nanoseconds = recv_time.nanoseconds;
        routerInfo->vrrpVirtRouterOperInfo.masterIpAddr = osapiNtohl(ip_hdr->iph_src);
      }
      else
      {

        if ( (vrrp_pkt-> priority > priority)
               || ((vrrp_pkt-> priority == priority)
                   && (osapiNtohl(ip_hdr->iph_src) >
                       routerInfo->vrrpVirtRouterOperInfo.localAddr.ipAddr)))
        {
          routerInfo->vrrpVirtRouterOperInfo.last_packet_recvtime.seconds = recv_time.seconds;
          routerInfo->vrrpVirtRouterOperInfo.last_packet_recvtime.nanoseconds = recv_time.nanoseconds;
          routerInfo->vrrpVirtRouterOperInfo.masterIpAddr = osapiNtohl(ip_hdr->iph_src);

          if (VRRPTransitToBackup(routerInfo,intIfNum) != L7_SUCCESS)
          {
            L7_uchar8 ifName[L7_NIM_IFNAME_SIZE+1];
            nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
            L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_VRRP_MAP_COMPONENT_ID,
                    "Failed to move to backup state for VR %d on interface %s.",
                    routerInfo->vrrpCfgInfo.vrid, ifName);
            osapiSemaGive(VrrpConfigSema);
            return L7_FAILURE;
          }
        }
      }

     if (routerInfo->vrrpCfgInfo.timer_learn == L7_TRUE)
     {
       routerInfo->vrrpVirtRouterOperInfo.master_advt_int = vrrp_pkt->adver_int;
     }
    }
   }
  osapiSemaGive(VrrpConfigSema);
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose to validate received VRRP messages
*
* @param    ip_hdr    @b{(input)}      IP Header of the received data
* @param    vrrp_pkt     @b{(input)}      VRRP data received
* @param    intIfNum    @b{(input)}      Interface on which the Packet was received
* @param    recv_pktlen  @b{(input)}    Length of received message
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t vrrpPacketValidate(L7_ipHeader_t *ip_hdr, vrrpPkt_t *vrrp_pkt,
                           L7_uint32 intIfNum, L7_uint32 recv_pktlen)
{
  L7_AUTH_TYPES_t  authType;
  vrrpRouterInfo_t* routerInfo;
  L7_uchar8 authKey[L7_VRRP_MAX_AUTH_DATA];
  L7_ushort16 ret_csum;
  L7_ushort16 vrrpPktHdrSize;
  L7_BOOL addr_match;
  L7_ushort16 i, j;

  /* First verify the vrid, whether the packet is meant for this router*/
  if (L7_vrrpOperEntryGet(vrrp_pkt->vrid, intIfNum)== L7_FAILURE)
  {
    L7_vrrpVridErrorIncr();
    return L7_FAILURE;
  }

  osapiSemaTake(VrrpConfigSema, L7_WAIT_FOREVER);
  if (NULL!= (routerInfo =
              (vrrpRouterInfo_t*) L7_vrrpVirtualRouterFetch(vrrp_pkt->vrid,intIfNum)))
  {

    if ((routerInfo->vrrpVirtRouterOperInfo.vr_state != L7_VRRP_STATE_MASTER) &&
        (routerInfo->vrrpVirtRouterOperInfo.vr_state != L7_VRRP_STATE_BACKUP))
    {
      osapiSemaGive(VrrpConfigSema);
      return L7_FAILURE;
    }

    /* Verify IP packet length */
    if (osapiNtohs(ip_hdr->iph_len) < L7_IP_HDR_LEN + VRRP_MIN_PACKET_LENGTH)
    {
      {
        (routerInfo->vrrpVirtRouterOperInfo.pkt_rcvd_invalid_length)++;
        osapiSemaGive(VrrpConfigSema);
        return L7_FAILURE;
      }
    }

    /* Verify TTL */
    if (ip_hdr->iph_ttl != IP_VRRP_TTL)
    {
      (routerInfo->vrrpVirtRouterOperInfo.pkt_ttl_incorrect)++;
      osapiSemaGive(VrrpConfigSema);
      return L7_FAILURE;
    }

    /* Verify VRRP Version */
    if (vrrp_pkt->version_type >> 4 != VRRP_PROTO_VERSION)
    {
      L7_vrrpRouterVerErrorIncr();
      osapiSemaGive(VrrpConfigSema);
      return L7_FAILURE;

    }

    if (( vrrp_pkt->version_type & 0x0f) != VRRP_PKT_ADV)
    {
      (routerInfo->vrrpVirtRouterOperInfo.pkt_rcvd_invalid_type)++;
      osapiSemaGive(VrrpConfigSema);
      return L7_FAILURE;
    }

    /* Verify that the Recv packet length is greater than or equal to
     the minimum vrrp packet length */
    if ( (recv_pktlen - L7_IP_HDR_LEN) <= VRRP_MIN_PACKET_LENGTH)
    {
      (routerInfo->vrrpVirtRouterOperInfo.pkt_rcvd_invalid_length)++;
      osapiSemaGive(VrrpConfigSema);
      return L7_FAILURE;
    }

    vrrpPktHdrSize = sizeof(vrrpPkt_t) -
                    (L7_L3_NUM_IP_ADDRS - vrrp_pkt->naddr) * sizeof(L7_IP_ADDR_t);
    /* Verify the checksum */
    ret_csum = vrrpCheckSum( (L7_ushort16*)vrrp_pkt, vrrpPktHdrSize, 0);
    if (ret_csum != 0)
    {
      L7_vrrpChecksumErrorIncr();
      osapiSemaGive(VrrpConfigSema);
      return L7_FAILURE;
    }

    /* Verify the Adver Interval */
   if (routerInfo->vrrpCfgInfo.timer_learn != L7_TRUE)
   {
    if (vrrp_pkt->adver_int != routerInfo->vrrpCfgInfo.adver_int)
    {
      (routerInfo->vrrpVirtRouterOperInfo.pkt_advert_interval_error)++;
      osapiSemaGive(VrrpConfigSema);
      return L7_FAILURE;
    }
   }

    /* Check the Authentication Type */
    if ((vrrp_pkt->auth_type != L7_AUTH_TYPE_NONE) && (vrrp_pkt->auth_type != L7_AUTH_TYPE_SIMPLE_PASSWORD))
    {
       if (vrrpBasicConfig_g.notif_flag == L7_ENABLE)
       {

         vrrpMapTrapAuthFailureTrap(vrrp_pkt->ipaddress[0],
                                 VRRP_TRAP_INVALID_AUTH_TYPE);
        }
      (routerInfo->vrrpVirtRouterOperInfo.pkt_rcvd_invalid_auth_type)++;
      osapiSemaGive(VrrpConfigSema);
      return L7_FAILURE;
    }

    authType = routerInfo->vrrpCfgInfo.auth_types;
    if (vrrp_pkt->auth_type != authType)
    {
      if (vrrpBasicConfig_g.notif_flag == L7_ENABLE)
      {
        vrrpMapTrapAuthFailureTrap(vrrp_pkt->ipaddress[0],
                                     VRRP_TRAP_AUTH_TYPE_MISMATCH);
      }
      (routerInfo->vrrpVirtRouterOperInfo.pkt_rcvd_mismatched_auth_type)++;
      osapiSemaGive(VrrpConfigSema);
      return L7_FAILURE;
    }

    /* Perform authentication */
    if (vrrp_pkt->auth_type == L7_AUTH_TYPE_SIMPLE_PASSWORD)
    {
      memcpy(authKey, routerInfo->vrrpCfgInfo.auth_data,VRRP_AUTH_DATA_LEN);

      /*
      We can't really use the auth_data field of vrrp_pkt, because
      the number of IP addresses will most likely not be the max
      */

      if (memcmp(&(vrrp_pkt->ipaddress[vrrp_pkt->naddr]),
                 authKey, L7_VRRP_MAX_AUTH_DATA) != 0)
      {
         if (vrrpBasicConfig_g.notif_flag == L7_ENABLE)
         {

            vrrpMapTrapAuthFailureTrap(vrrp_pkt->ipaddress[0],
                                   VRRP_TRAP_AUTH_FAILURE);
         }
        (routerInfo->vrrpVirtRouterOperInfo.pkt_auth_failure)++;
        osapiSemaGive(VrrpConfigSema);
        return L7_FAILURE;
      }
    }

    if (vrrp_pkt->naddr != routerInfo->vrrpVirtRouterOperInfo.ipAddressCount)
    {
      /* I'd rather not log a message here because the condition is likely to
       * persist and fill the log. But w/o a log message, user is not notified
       * of the configuration error. So at least rate limit. */
      routerInfo->vrrpVirtRouterOperInfo.pkt_addr_count++;
      if (routerInfo->vrrpVirtRouterOperInfo.pkt_addr_count % VRRP_LOG_MOD == 1)
      {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE+1];
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_VRRP_MAP_COMPONENT_ID,
                "Dropping packet received for VR %d on interface %s that has %d VR IP address%s. "
                "%d address%s are configured locally.",
                routerInfo->vrrpCfgInfo.vrid, ifName, vrrp_pkt->naddr,
                (vrrp_pkt->naddr == 1) ? "" : "es",
                routerInfo->vrrpVirtRouterOperInfo.ipAddressCount,
                (routerInfo->vrrpVirtRouterOperInfo.ipAddressCount == 1) ? "" : "es");
      }
      osapiSemaGive(VrrpConfigSema);
      return L7_FAILURE;
    }

    /* Whether in backup or master state, make sure the IP address in the VRRP
       packet matches the IP address  that was configured for this VRID */
    for (i = 0; i < vrrp_pkt->naddr; i++)
    {
      addr_match = L7_FALSE;
      for (j = 0; j < L7_L3_NUM_IP_ADDRS; j++)
      {
        if (osapiNtohl(vrrp_pkt->ipaddress[i]) == routerInfo->vrrpCfgInfo.ipaddress[j])
        {
          addr_match = L7_TRUE;
          break;
        }
      }
      if (addr_match != L7_TRUE)
      {
        (routerInfo->vrrpVirtRouterOperInfo.pkt_rcvd_invalid_ip_address_list)++;
        if (routerInfo->vrrpVirtRouterOperInfo.pkt_rcvd_invalid_ip_address_list % VRRP_LOG_MOD == 1)
        {
          L7_uchar8 ifName[L7_NIM_IFNAME_SIZE+1];
          L7_uchar8 ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
          osapiInetNtoa(osapiNtohl(vrrp_pkt->ipaddress[i]), ipAddrStr);
          nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_VRRP_MAP_COMPONENT_ID,
                  "Dropping packet received for VR %d on interface %s that has VR IP address %s. "
                  "This VR address is not configured locally.",
                  routerInfo->vrrpCfgInfo.vrid, ifName, ipAddrStr);
        }
        osapiSemaGive(VrrpConfigSema);
        return L7_FAILURE;
      }
    }
  }
  else
  {
    osapiSemaGive(VrrpConfigSema);
    return L7_FAILURE;
  }

  osapiSemaGive(VrrpConfigSema);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To change the state to master
*
* @param    routerInfo    @b{(input)}      Pointer to the vrrp structure
* @param    vmac[L7_MAC_ADDR_LEN]    @b{(input)}   array for VMAC
* @param    infNumber    @b{(input)}               interface number
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t VRRPTransitToMaster(vrrpRouterInfo_t *routerInfo,
                            L7_uchar8 vmac[L7_MAC_ADDR_LEN], L7_uint32 intIfNum)
{
  NIM_HANDLE_t nimHandle;
  L7_uint32 ipAddrCount;
  L7_uint32 ipMask[L7_L3_NUM_IP_ADDRS];
  L7_clocktime gratArpTime;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE+1];

  routerInfo->vrrpVirtRouterOperInfo.vr_state = L7_VRRP_STATE_MASTER;
  routerInfo->vrrpVirtRouterOperInfo.master_advt_int = routerInfo->vrrpCfgInfo.adver_int;

  if (routerInfo->vrrpVirtRouterOperInfo.oper_priority != 255)
  {
    /* Priority 255 means we are the ip address owner.
     * no need to explicitly tell that vr ip to the stack */
    memset(ipMask, 0, sizeof(ipMask));
    for(ipAddrCount = 0; ipAddrCount< L7_L3_NUM_IP_ADDRS; ipAddrCount++)
    {
      if (routerInfo->vrrpCfgInfo.ipaddress[ipAddrCount] != FD_VRRP_DEFAULT_IP_ADDRESS)
      {
        (void)ipMapRtrIntfSubnetMaskGet(intIfNum, routerInfo->vrrpCfgInfo.ipaddress[ipAddrCount],
                                        &ipMask[ipAddrCount]);
      }
    }

    osapiVrrpMasterVrrpAddressAdd(intIfNum, routerInfo->vrrpCfgInfo.vrid,
                                  routerInfo->vrrpCfgInfo.ipaddress,
                                  ipMask);
  }

  if (L7_vrrpMgmtVmacAddrAdd(intIfNum, vmac) != L7_SUCCESS)
  {
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_VRRP_MAP_COMPONENT_ID,
            "Failed to add virtual router MAC address to L2FDB for VR %d on "
            "interface %s.", routerInfo->vrrpCfgInfo.vrid, ifName);
    return L7_FAILURE;
  }

  /* Hardware should be updated first when we are becoming Master.
   * This would add the L2 MAC entry corresponding to VRID in the hardware
   */
  if (dtlIpv4VrrpVridAdd(intIfNum,routerInfo->vrrpCfgInfo.vrid,
                         routerInfo->vrrpCfgInfo.ipaddress[0])!=L7_SUCCESS)
  {
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_VRRP_MAP_COMPONENT_ID,
            "Failed to add virtual router %d on interface %s to hardware.",
            routerInfo->vrrpCfgInfo.vrid, ifName);
    L7_vrrpMgmtVmacAddrDel(intIfNum, vmac);
    return L7_FAILURE;
  }

  if(vrrpMapIntfChangeNimNotify(intIfNum, L7_VRRP_TO_MASTER,
                                L7_NULLPTR, &nimHandle) == L7_FAILURE)
  {
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_VRRP_MAP_COMPONENT_ID,
            "Failed to notify NIM that virtual router %d has become master "
            "on interface %s.", routerInfo->vrrpCfgInfo.vrid, ifName);
    return L7_FAILURE;
  }

  /* Send the first VRRP advertisement after becoming Master */
  if (vrrpAdvtPacketSend(routerInfo,0)==L7_FAILURE)
  {
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_VRRP_MAP_COMPONENT_ID,
            "Failed to send first VRRP advertisement after becoming master "
            "for VR %d on interface %s.", routerInfo->vrrpCfgInfo.vrid, ifName);
    return L7_FAILURE;
  }

  /* Send the first gratuitous ARP after becoming Master
   * for the VRIP address with VR Mac */
  osapiClockTimeRaw (&gratArpTime);
  routerInfo->vrrpVirtRouterOperInfo.lastGratArpTime = gratArpTime.seconds;
  for(ipAddrCount = 0; ipAddrCount< L7_L3_NUM_IP_ADDRS; ipAddrCount++)
  {
    if (routerInfo->vrrpCfgInfo.ipaddress[ipAddrCount] != FD_VRRP_DEFAULT_IP_ADDRESS)
    {
      /* Not catastrophic if grat ARP doesn't go. Continue on. */
      vrrpGratuitousARPSend(routerInfo->vrrpVirtRouterOperInfo.vmac,
                                routerInfo->vrrpCfgInfo.ipaddress[ipAddrCount],
                                intIfNum);
    }
  }

  /* continue if this fails */
  if (vrrpBasicConfig_g.notif_flag == L7_ENABLE)
  {
    vrrpMapTrapNewMasterTrap(routerInfo->vrrpCfgInfo.ipaddress[0]);
  }

  routerInfo->vrrpVirtRouterOperInfo.masterIpAddr =
    routerInfo->vrrpVirtRouterOperInfo.localAddr.ipAddr;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To change the state to backup
*
* @param    infNumber    @b{(input)}     Interface number
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t VRRPTransitToBackup(vrrpRouterInfo_t* routerInfo, L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_SUCCESS;
  NIM_HANDLE_t nimHandle;

  L7_vrrpState_t saveState = routerInfo->vrrpVirtRouterOperInfo.vr_state;

  routerInfo->vrrpVirtRouterOperInfo.vr_state = L7_VRRP_STATE_BACKUP;

  /* We only need to do a VRID delete call to DTL if we are transitioning from Master state */
  if (saveState == L7_VRRP_STATE_MASTER)
  {
    osapiVrrpMasterVrrpAddressDelete(intIfNum, routerInfo->vrrpCfgInfo.vrid,
                                     routerInfo->vrrpCfgInfo.ipaddress);

    if(vrrpMapIntfChangeNimNotify(intIfNum, L7_VRRP_FROM_MASTER,
                                  L7_NULLPTR, &nimHandle) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    rc = dtlIpv4VrrpVridDelete(intIfNum,routerInfo->vrrpCfgInfo.vrid);

    L7_vrrpMgmtVmacAddrDel(intIfNum, routerInfo->vrrpVirtRouterOperInfo.vmac);
  }

  return rc;
}

/*********************************************************************
* @purpose  To shutdown the vrrp master
*
* @param    routerInfo    @b{(input)}      Pointer to the vrrp structure
*
* @returns  L7_SUCCESS or L7_FAILURE

* @notes    none
*
* @end
*********************************************************************/
L7_RC_t VRRPShutdownVirtualRouter(vrrpRouterInfo_t* routerInfo, L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_SUCCESS;
  NIM_HANDLE_t nimHandle;

  if (routerInfo->vrrpVirtRouterOperInfo.vr_state == L7_VRRP_STATE_MASTER)
  {
    if (vrrpAdvtPacketSend(routerInfo,1)==L7_FAILURE)    /*Sending Zero Priority Advertisement Packet*/
    {
      return L7_FAILURE;
    }

    osapiVrrpMasterVrrpAddressDelete(intIfNum, routerInfo->vrrpCfgInfo.vrid,
                                     routerInfo->vrrpCfgInfo.ipaddress);

    (routerInfo->vrrpVirtRouterOperInfo.pkt_sent_zero_priority)++;
    routerInfo->vrrpVirtRouterOperInfo.vr_state = L7_VRRP_STATE_INIT;

      if(vrrpMapIntfChangeNimNotify(intIfNum, L7_VRRP_FROM_MASTER,
                           L7_NULLPTR, &nimHandle) == L7_FAILURE)
    {
      return L7_FAILURE;
    }

    rc = dtlIpv4VrrpVridDelete(intIfNum,routerInfo->vrrpCfgInfo.vrid);

    L7_vrrpMgmtVmacAddrDel(intIfNum, routerInfo->vrrpVirtRouterOperInfo.vmac);

  }
  return rc;
}



/*********************************************************************
* @purpose  To send the gratuitous arp for the IP address passed
*
* @param    vmac[L7_MAC_ADDR_LEN]    @b{(input)}   array for VMAC
* @param    sourceIP     @b{(input)}               source IP address
* @param    infNumber    @b{(input)}               interface number
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t vrrpGratuitousARPSend(L7_uchar8 vmac[L7_MAC_ADDR_LEN],
                              L7_uint32 sourceIP,L7_uint32 infNumber)
{

  L7_ether_arp_t  arpHdr;
  L7_netBufHandle pBufHandle;
  L7_uchar8 vrrpBuffer[sizeof(L7_ether_arp_t)];
  L7_ushort16 etype;
  L7_ushort16 flag = 0;
  L7_uchar8 eth_dhost[L7_MAC_ADDR_LEN];
  L7_char8 *vrrpData;

  L7_IP_ADDR_t targetIP,srcIP;

  bzero(vrrpBuffer,sizeof(L7_ether_arp_t));

  /*ARP Header*/
  memcpy(&srcIP, &sourceIP, sizeof(srcIP));
  memcpy(&targetIP, &sourceIP, sizeof(targetIP));
  memcpy( eth_dhost , &(L7_ENET_BCAST_MAC_ADDR) ,L7_MAC_ADDR_LEN );

  arpHdr.arp_hrd = osapiHtons(L7_ARPHRD_ETHER);     /* hardware address type */
  arpHdr.arp_pro = osapiHtons(L7_ETYPE_IP);     /* protocol address type */
  arpHdr.arp_hln = L7_MAC_ADDR_LEN;       /* hardware address length */
  arpHdr.arp_pln = VRRP_ARP_ADDR_LEN;       /* protocol address length */
  arpHdr.arp_op  = osapiHtons(L7_ARPOP_REQUEST);   /* opcode command */

  memcpy(arpHdr.arp_sha, vmac, L7_MAC_ADDR_LEN);  /* sender hardware address */
  srcIP = osapiHtonl(srcIP);
  memcpy(arpHdr.arp_spa, &srcIP, sizeof(L7_IP_ADDR_t));  /* sender protocol (IP) address */
  memset(arpHdr.arp_tha, 0,L7_MAC_ADDR_LEN);
  targetIP = osapiHtonl(targetIP);
  memcpy(arpHdr.arp_tpa, &targetIP, sizeof(L7_IP_ADDR_t));  /* target protocol (IP) address */

  memcpy(vrrpBuffer, &arpHdr, sizeof(L7_ether_arp_t));

  flag |= L7_LL_ENCAP_ENET;
  flag |= L7_LL_USE_L3_SRC_MAC;
  etype=L7_ETYPE_ARP;
  if (sysNetPduHdrEncapsulate(eth_dhost,infNumber,&flag,sizeof(L7_ether_arp_t),
                              0,etype ,&pBufHandle,vrrpBuffer )
      != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /*Gratuitous ARP is sent using the virtual mac as the source address*/
  SYSAPI_NET_MBUF_GET_DATASTART(pBufHandle, vrrpData);
  vrrpData += L7_MAC_ADDR_LEN;
  memcpy(vrrpData, vmac, L7_MAC_ADDR_LEN);

  if (ipmRouterIfBufSend(infNumber,pBufHandle)==L7_FAILURE)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  To send the adverisement packet
*
* @param    routerInfo    @b{(input)}      Pointer to the vrrp structure
*
* @returns  L7_SUCCESS or L7_FAILURE
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t vrrpAdvtPacketSend(vrrpRouterInfo_t* routerInfo,int zeroPriorityFlag)
{
  L7_uchar8 vrrpBuffer[L7_IP_HDR_LEN+sizeof(vrrpPkt_t)];
  L7_ushort16 etype;
  L7_ushort16 flag = 0;
  L7_netBufHandle pBufHandle;
  L7_char8 *vrrpData;
  L7_uint32 datalen;
  L7_uint32 pktSize;
  L7_char8  *tempHeader;

  memset(vrrpBuffer, 0x00, sizeof(vrrpBuffer));
  tempHeader = vrrpBuffer;
  if ( vrrpPktBuild(routerInfo,vrrpBuffer,zeroPriorityFlag)==L7_FAILURE)
  {
    return L7_FAILURE;
  }

  /* Calculate the packet size */
  /* Packet size = Maximum Pkt Size - ( Max Allowed Ip Addrs - Cfgr Ip Addrs) * sizeof(IPAddr))*/
  pktSize = VRRP_PACKET_LENGTH - (sizeof(L7_IP_ADDR_t) * (L7_L3_NUM_IP_ADDRS - ((vrrpPkt_t *) (vrrpBuffer + sizeof(L7_ipHeader_t)))->naddr));

  flag |= L7_LL_ENCAP_ENET;
  flag |= L7_LL_USE_L3_SRC_MAC;
  etype=L7_ETYPE_IP;

  if (sysNetPduHdrEncapsulate(vrrpMulticastAddr,
                              routerInfo->vrrpVirtRouterOperInfo.intIfNum,
                              &flag,pktSize,0,etype,
                              &pBufHandle,vrrpBuffer )
      != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  SYSAPI_NET_MBUF_GET_DATALENGTH(pBufHandle, datalen);
  if (datalen < L7_EFFECTIVE_MIN_ENET_PKT_SIZE)
  {
    SYSAPI_NET_MBUF_GET_DATASTART(pBufHandle, vrrpData);
    memset(vrrpData + datalen, 0 , L7_EFFECTIVE_MIN_ENET_PKT_SIZE - datalen);
    datalen = L7_EFFECTIVE_MIN_ENET_PKT_SIZE;
    SYSAPI_NET_MBUF_SET_DATALENGTH(pBufHandle, datalen);
  }

  SYSAPI_NET_MBUF_GET_DATASTART(pBufHandle, vrrpData);
  vrrpData += L7_MAC_ADDR_LEN;
  memcpy(vrrpData,routerInfo->vrrpVirtRouterOperInfo.vmac, L7_MAC_ADDR_LEN);
  vrrpDebugPacketTxTrace(routerInfo->vrrpVirtRouterOperInfo.intIfNum, vrrpBuffer);
  if (ipmRouterIfBufSend(routerInfo->vrrpVirtRouterOperInfo.intIfNum,
                         pBufHandle)==L7_FAILURE)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
