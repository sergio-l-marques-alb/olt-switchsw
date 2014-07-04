/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: dtl_pdu_landd.c
*
* Purpose: This file contains the functions that receives or send
*          protocol data units from/to the lan device driver.
*
* Component: Device Transformation Layer (DTL)
*
* Commnets:
*
* Created by: Shekhar Kalyanam 3/14/2001
*
*********************************************************************/
/*************************************************************

*************************************************************/





/*
**********************************************************************
*                           HEADER FILES
**********************************************************************
*/

#define DTL_PDU_LANDD_GLOBALS               /* Enable global space   */
#include "dtlinclude.h"

#include <l3end_api.h>
#include <sysapi.h>
#include <sysnet_api.h>
#include <sysnet_api_ipv4.h>
#include <sysnet_api_ipv6.h>
#include <simapi.h>
#include "dapi_struct.h"
#include "l7_socket.h"
#include "logger.h"
#include "ptin_fpga_api.h"

extern ptin_debug_pktTimer_t debug_pktTimer;

extern L7_BOOL dtlNetInitDone;

/* My parent to adapt    */

#if DTLCTRL_COMPONENT_PDU_LANDD

/* PTin added: packet trap */
#if 1
L7_BOOL pdu_receive_debug = 0;

void pdu_receive_debug_enable(L7_BOOL enable)
{
  pdu_receive_debug = enable;
}

static L7_uint8  ptin_debug_dtl = 0;
inline void ptin_debug_dtl_set(L7_uint8 enable){ptin_debug_dtl = enable;};
#endif


/*
**********************************************************************
*                           PRIVATE FUNCTIONS PROTOTYPES
**********************************************************************
*/


/*
**********************************************************************
*                           API FUNCTIONS
**********************************************************************
*/

/*********************************************************************
* @purpose  Receive the PDU puts it in a message queue and releases the interrupt thread
*
* @param    *ddUSP         @b{(input)} Pointer to the Unit Slot Port Structure
* @param    family         @b{(input)} A DAPI_FAMILY_t family
* @param    cmd            @b{(input)} A DAPI_CMD_t command
* @param    *dapiEventInfo @b{(input)} Pointer to Dapi event
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlPduReceiveCallback(DAPI_USP_t *ddusp,
                                    DAPI_FAMILY_t family,
                                    DAPI_CMD_t cmd,
                                    DAPI_EVENT_t event,
                                    void *dapiEventInfo)
{

  L7_RC_t mrc = L7_SUCCESS;
  L7_RC_t dr= L7_SUCCESS;
  L7_ushort16 restoredtlMsgTail;
  L7_ushort16 savedTail;
  DAPI_FRAME_CMD_t *dei;

  if (pdu_receive_debug)
    LOG_TRACE(LOG_CTX_PTIN_DTL,"Packet received here");

  dei = (DAPI_FRAME_CMD_t *)dapiEventInfo;

  if (dtlNetInitDone != L7_TRUE)
  {
    SYSAPI_NET_MBUF_FREE(dei->cmdData.receive.frameHdl);
    dr = L7_SUCCESS;
    return dr;
  }

  debug_pktTimer.pkt_dtl_counter++;

  /*enter critical section*/
  dtlEnterCS();

  if (dtlMsgQueue && ((dtlMsgTail+1)%DTL_MSG_COUNT != dtlMsgHead))
  {
    restoredtlMsgTail= dtlMsgTail;
    dtlMsgTail = (dtlMsgTail+1)%DTL_MSG_COUNT;
    savedTail = dtlMsgTail;
    /*exit critical section*/
    dtlExitCS();

    memcpy(&(dtlMsg[savedTail].cmdData.dapiFrameEventInfo),dapiEventInfo,sizeof(DAPI_FRAME_CMD_t));
    memcpy(&(dtlMsg[savedTail].ddusp),ddusp,sizeof(DAPI_USP_t));

    dtlMsg[savedTail].cmdFamilyType = family;
    dtlMsg[savedTail].cmdType = cmd;
    dtlMsg[savedTail].eventType = event;

    if (pdu_receive_debug)
      LOG_TRACE(LOG_CTX_PTIN_DTL,"Sending packet to dtlMsgQueue queue");

    if ((mrc = osapiMessageSend(dtlMsgQueue, (void *)&savedTail, DTL_MSG_SIZE,L7_NO_WAIT, L7_MSG_PRIORITY_NORM))
        != L7_SUCCESS)
    {
      dtlEnterCS();
      dtlMsgTail = restoredtlMsgTail;
      dtlExitCS();

      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DTL_COMPONENT_ID,
              "DTL: pdu receive msg send failed, mrc=%d\n", mrc);
      dr = L7_FAILURE;
    }
  }
  else
  {
    /*exit critical section*/
    dtlExitCS();
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DTL_COMPONENT_ID,
            "DTL: dtl pdu receive msg queue full , mrc=%d\n", mrc);
    dr = L7_FAILURE;
  }

  return dr;
}


/*********************************************************************
* @purpose  Receive the PDU
*
* @param    *ddUSP         @b{(input)} Pointer to the Unit Slot Port Structure
* @param    family         @b{(input)} A DAPI_FAMILY_t family
* @param    cmd            @b{(input)} A DAPI_CMD_t command
* @param    *dapiEventInfo @b{(input)} Pointer to Dapi event
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    provide traffic parameterization
*
* @end
*********************************************************************/
L7_RC_t dtlPduReceive(DAPI_USP_t *ddusp,
                      DAPI_FAMILY_t family,
                      DAPI_CMD_t cmd,
                      DAPI_EVENT_t event,
                      void *dapiEventInfo)
{

  DAPI_FRAME_CMD_t *dei;
  char *data;
  nimUSP_t usp;
  L7_uint32 intIfNum;
  L7_RC_t rc = L7_FAILURE;
  sysnet_pdu_info_t pduInfo;

  if (pdu_receive_debug)
    LOG_TRACE(LOG_CTX_PTIN_DTL,"I have received a packet here");

  debug_pktTimer.pkt_dtl2_counter++;

  dei = (DAPI_FRAME_CMD_t *)dapiEventInfo;

  SYSAPI_NET_MBUF_SET_LOC(dei->cmdData.receive.frameHdl, MBUF_LOC_PDU_RX);
  SYSAPI_NET_MBUF_GET_DATASTART(dei->cmdData.receive.frameHdl, data);

  usp.unit = ddusp->unit;
  usp.slot = ddusp->slot;
  usp.port = ddusp->port + 1;

  if (nimGetIntIfNumFromUSP(&usp, &intIfNum) != L7_SUCCESS)
  {
    SYSAPI_NET_MBUF_FREE (dei->cmdData.receive.frameHdl);
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
              "Received PDU with invalid usp %d/%d/%u.",
              usp.unit, usp.slot, usp.port);
    return L7_FAILURE;
  }
  bzero((L7_uchar8 *)&pduInfo, sizeof(sysnet_pdu_info_t));
  pduInfo.intIfNum = intIfNum;
  pduInfo.vlanId = dei->cmdData.receive.vlanID;
  pduInfo.innerVlanId = dei->cmdData.receive.innerVlanId;

  if (pdu_receive_debug)
    LOG_TRACE(LOG_CTX_PTIN_DTL,"...");

  if (sysNetHas8021qVlanERIF(data) == L7_TRUE)
  {
      SYSAPI_NET_MBUF_FREE (dei->cmdData.receive.frameHdl);
      rc = L7_FAILURE;
  }
  else
  {
    if (pdu_receive_debug)
      LOG_TRACE(LOG_CTX_PTIN_DTL,"Going to call sysNetNotifyPduReceive");

    rc = sysNetNotifyPduReceive (dei->cmdData.receive.frameHdl, &pduInfo);
  }

  if (rc == L7_FAILURE)
  {
    dtlStatsIncrement(intIfNum,L7_PLATFORM_CTR_RX_DISCARDS);

  }
  return rc;
}

/*********************************************************************
* @purpose  Transmits the PDU
*
* @param    bufHandle   @b{(input)}  Handle to buffer
* @param    dtlCmd      @b{(input)}  Type of command
* @param    *dtlCmdInfo @b{(input)}  Pointer to DTL transmit command info structure
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    provide traffic parameterization
*
* @end
*********************************************************************/
L7_RC_t dtlPduTransmit( L7_netBufHandle bufHandle,
                        DTL_CMD_t dtlCmd,
                        DTL_CMD_TX_INFO_t *dtlCmdInfo )
{
  DAPI_USP_t       ddUsp;
  nimUSP_t         usp;
  DAPI_FRAME_CMD_t sendData;
  L7_uint32        activeState;  
  L7_RC_t dr;

  /* PTin added: Default flags */
  sendData.cmdData.send.flags = 0;

#if PTIN_BOARD_IS_MATRIX  
  /* Do nothing for slave matrix */
  if (!ptin_fgpa_mx_is_matrixactive())
  {
    if (ptin_debug_dtl)
      LOG_NOTICE(LOG_CTX_PTIN_DTL,"Silently ignoring packet transmission [intfNum:%u]. I'm a Slave Matrix",dtlCmdInfo->intfNum);
    SYSAPI_NET_MBUF_FREE(bufHandle);
    return L7_SUCCESS;
  }
#endif

  //Ignore if the port has link down (only consider valid interfaces)
  if ( (nimCheckIfNumber(dtlCmdInfo->intfNum) == L7_SUCCESS) &&
       ((nimGetIntfActiveState(dtlCmdInfo->intfNum, &activeState) != L7_SUCCESS) || (activeState != L7_ACTIVE)) )
  {
    if (ptin_debug_dtl)
      LOG_NOTICE(LOG_CTX_PTIN_DTL,"Silently ignoring packet transmission. Outgoing interface [intIfNum=%u] is down!",dtlCmdInfo->intfNum);    
    SYSAPI_NET_MBUF_FREE(bufHandle);
    return L7_SUCCESS;
  }

  switch (dtlCmd)
  {
  case DTL_CMD_TX_L2:
    {      

      if (dtlCmdInfo->typeToSend == DTL_VLAN_MULTICAST || dtlCmdInfo->typeToSend == DTL_L2RAW_VLAN_MULTICAST)
      {
        /* send to all ports */
        if (dtlCmdInfo->typeToSend == DTL_L2RAW_VLAN_MULTICAST)
          sendData.cmdData.send.type = DAPI_FRAME_TYPE_NO_L2_EGRESS_MCAST_DOMAIN;
        else
          sendData.cmdData.send.type = DAPI_FRAME_TYPE_MCAST_DOMAIN;

        dr = dapiCpuUspGet (&ddUsp);
        
        if (dr != L7_SUCCESS)
        {
          SYSAPI_NET_MBUF_FREE(bufHandle);
          return L7_FAILURE;
        }
      }
      else if (nimGetUnitSlotPort(dtlCmdInfo->intfNum, &usp) != L7_SUCCESS)
      {
        SYSAPI_NET_MBUF_FREE(bufHandle);
        return L7_FAILURE;
      }
      else
      {
        ddUsp.unit = usp.unit;
        ddUsp.slot = usp.slot;
        ddUsp.port = usp.port - 1;

        if (dtlCmdInfo->typeToSend == DTL_L2RAW_UNICAST)
          sendData.cmdData.send.type = DAPI_FRAME_TYPE_NO_L2_EGRESS_DATA_TO_PORT;
        else {
          sendData.cmdData.send.type = DAPI_FRAME_TYPE_DATA_TO_PORT;
          sendData.cmdData.send.flags = dtlCmdInfo->cmdType.L2.flags;    /* PTIN added: PTP Timestamp */
        }
      }

      sendData.cmdData.send.frameHdl = bufHandle;
      sendData.cmdData.send.priority = dtlCmdInfo->priority;
      sendData.cmdData.send.vlanID = dtlCmdInfo->cmdType.L2.domainId;


      break;
    }/*case DTL_CMD_TX_L2*/
  default:
    SYSAPI_NET_MBUF_FREE(bufHandle);
    return L7_FAILURE;
  }/*switch dtlCmd*/

  SYSAPI_NET_MBUF_SET_LOC(bufHandle, MBUF_LOC_PDU_TX);

  dr = dapiCtl(&ddUsp, DAPI_CMD_FRAME_SEND, &sendData);

  if (dr == L7_SUCCESS)
  {   
    return L7_SUCCESS;
  }
  else
  {
    dtlStatsIncrement(dtlCmdInfo->intfNum,L7_PLATFORM_CTR_TX_DISCARDS);
    return L7_FAILURE;
  }
}

/*********************************************************************
* @purpose  Receives ARP packets
*
* @param    netBufHandle    @b{(input)} Handle to buffer
* @param    sysnet_pdu_info_t *pduInfo   pointer to pdu info structure
*                                        which stores intIfNum and vlanId
*
*
* @returns  L7_SUCCESS  - Frame has been consumed.
* @returns  L7_FAILURE  - Frame has not been consumed.
* @returns  L7_ERROR  - Frame has not been consumed.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlArpRecvIP(L7_netBufHandle netBufHandle,
                     sysnet_pdu_info_t *pduInfo)
{
  L7_RC_t rc;

  /* Call interceptors who are interested in ARP frames.  If L7_TRUE is returned,
  ** the frame was either discarded or consumed, which means that the network buffer
  ** has been freed by the intercept call, or will be freed by the consumer.
  */
  /* There is a reason these are both here. They are not duplicates. In routing builds,
   * SYSNET_INET_ARP_IN only sees ARP packets received on routing interfaces. DAI and
   * captive portal need to see ARP packets received on L2 interfaces, too. So
   * SYSNET_INET_RECV_ARP_IN is added as an intercept point that sees both. Need to
   * have this intercept point in switching builds, too. The order of these two
   * intercept points matches their order in routing builds. */
  if (SYSNET_PDU_INTERCEPT(L7_AF_INET, SYSNET_INET_RECV_ARP_IN, netBufHandle, pduInfo,
                           L7_NULLPTR, L7_NULLPTR) == L7_TRUE)
  {
    return L7_SUCCESS;
  }

  if (SYSNET_PDU_INTERCEPT(L7_AF_INET, SYSNET_INET_ARP_IN, netBufHandle, pduInfo,
                           L7_NULLPTR, L7_NULLPTR) == L7_TRUE)
  {
    return L7_SUCCESS;
  }

  /* For builds without routing support the ipmRouterIfFrameProcess always
  ** returns an error.
  */
  rc = ipmRouterIfFrameProcess (netBufHandle, pduInfo);
  if (rc != L7_SUCCESS)
  { /*Management vlan id is set to 1 and only management packets are sent to the cpu*/
    if (pduInfo->vlanId==simMgmtVlanIdGet())
      rc = dtlARPProtoRecv(netBufHandle, pduInfo);
    else
      rc = L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
* @purpose  Receives IP packets
*
* @param    netBufHandle    @b{(input)} Handle to buffer
* @param    sysnet_pdu_info_t *pduInfo   pointer to pdu info structure
*                                        which stores intIfNum and vlanId
*
*
* @returns  L7_SUCCESS  - Frame has been consumed.
* @returns  L7_FAILURE  - Frame has not been consumed.
* @returns  L7_ERROR  - Frame has not been consumed.
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t dtlRecvIP(L7_netBufHandle netBufHandle,
                  sysnet_pdu_info_t *pduInfo)
{
  L7_RC_t rc;

  /* Call interceptors who are interested in all incoming IP frames.  (This would
  ** include IGMP Snooping in a Switching build.)  If L7_TRUE is returned, the frame
  ** was either discarded or consumed, which means that the network buffer has been
  ** freed by the intercept call, or will be freed by the consumer.
  */
  if (SYSNET_PDU_INTERCEPT(L7_AF_INET, SYSNET_INET_RECV_IN, netBufHandle, pduInfo,
                           L7_NULLPTR, L7_NULLPTR) == L7_TRUE)
    return L7_SUCCESS;

  /* For builds without routing support the ipmRouterIfFrameProcess always
  ** returns an error.
  */

  rc = ipmRouterIfFrameProcess (netBufHandle, pduInfo);
  if (rc != L7_SUCCESS)
  {
    if (pduInfo->vlanId == simMgmtVlanIdGet())
      rc = dtlIPProtoRecv(netBufHandle, pduInfo);
    else
      rc = L7_FAILURE;
  }

  return rc;
}
/*********************************************************************
* @purpose  Receives IP packets
*
* @param    netBufHandle    @b{(input)} Handle to buffer
* @param    sysnet_pdu_info_t *pduInfo   pointer to pdu info structure
*                                        which stores intIfNum and vlanId
*
*
* @returns  L7_SUCCESS  - Frame has been consumed.
* @returns  L7_FAILURE  - Frame has not been consumed.
* @returns  L7_ERROR  - Frame has not been consumed.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlRecvIP6(L7_netBufHandle netBufHandle,
                  sysnet_pdu_info_t *pduInfo)
{
  L7_RC_t rc;
  L7_uchar8        macStart[2] = {0x33, 0x33}; /* IPv6 Multicast mac address */
  L7_ip6Header_t  *ip6_header;
  L7_uchar8       *data;
  L7_uint32        offset;

  /* Call interceptors who are interested in all incoming IP frames.  (This would
  ** include MLD Snooping in a Switching build.)  If L7_TRUE is returned, the frame
  ** was either discarded or consumed, which means that the network buffer has been
  ** freed by the intercept call, or will be freed by the consumer.
  */

  SYSAPI_NET_MBUF_GET_DATASTART(netBufHandle, data);
  if (memcmp(data, macStart, 2) == 0)
  {
    offset = sysNetDataOffsetGet(data);
    ip6_header = (L7_ip6Header_t *)(data + offset);

    if(L7_IP6_IS_ADDR_MULTICAST(ip6_header->dst))
    {
      if (SYSNET_PDU_INTERCEPT(L7_AF_INET6, SYSNET_INET6_MCAST_IN, netBufHandle, pduInfo,
                               L7_NULLPTR, L7_NULLPTR) == L7_TRUE)
      {
         return L7_SUCCESS;
      }
    } /* End of is IPv6 multicast addr check */
  }/* End of MAC DA Check */
  else
   {
      if (SYSNET_PDU_INTERCEPT(L7_AF_INET6, SYSNET_INET6_RECV_IN, netBufHandle, pduInfo,
            L7_NULLPTR, L7_NULLPTR) == L7_TRUE)
      {
         return L7_SUCCESS;
      }
   }


  if (pduInfo->vlanId == simMgmtVlanIdGet())
  {
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
    rc = dtlIPProtoRecv(netBufHandle, pduInfo);
#else
    rc = L7_FAILURE;
#endif
  }
   else
   {
      rc = L7_FAILURE;
   }

  return rc;
}
/*********************************************************************
* @purpose  Loopbacks a pdu to a specified interface
*
* @param    destIntf        @b{(input)} the interface number the pdu should be received in
* @param    netBufHandle    @b{(input)} Handle to buffer
*
*
* @returns  L7_SUCCESS  - Frame has been consumed.
* @returns  L7_FAILURE  - Frame has not been consumed.
*
* @notes    This is a routine just for testing. The pdu IS NOT sent to the lower
*           layers hence there is no egress from the device. This routine just
*           simulates that a particular pdu has been received on a particular interface
*
* @end
*********************************************************************/
L7_RC_t dtlPduLoopback(L7_uint32 destIntf, L7_netBufHandle bufHandle)
{
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  L7_RC_t dr;
  L7_RC_t rc;
  DAPI_FAMILY_t family = DAPI_FAMILY_FRAME;
  DAPI_CMD_t cmd = DAPI_CMD_FRAME_UNSOLICITED_EVENT;
  DAPI_EVENT_t event = DAPI_EVENT_FRAME_RX;
  DAPI_FRAME_CMD_t dapiEventInfo;

  if (pdu_receive_debug)
    LOG_TRACE(LOG_CTX_PTIN_DTL,"Received packet here!");

  if ((rc = nimGetUnitSlotPort(destIntf, &usp)) != L7_SUCCESS)
      return rc;

  ddUsp.unit = usp.unit;
  ddUsp.slot = usp.slot;
  ddUsp.port = usp.port - 1;

  dapiEventInfo.cmdData.receive.frameHdl = bufHandle;
  dapiEventInfo.cmdData.receive.vlanID = simMgmtVlanIdGet();
  dapiEventInfo.cmdData.receive.type = 0;

  if (pdu_receive_debug)
    LOG_TRACE(LOG_CTX_PTIN_DTL,"Calling dtlPduReceiveCallback function");

  dr = dtlPduReceiveCallback(&ddUsp, family, cmd, event, &dapiEventInfo);
  if (dr==L7_FAILURE)
    rc= L7_FAILURE;
  else
    rc = L7_SUCCESS;
  return rc;
}
#endif /*DTLCTRL_COMPONENT_LANDD*/
