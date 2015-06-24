/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   snooping.c
*
* @purpose    Contains definitions to packet processing routines to
*             support multicast snooping
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
#include "l7_product.h"
#include "buff_api.h"
#include "sysnet_api_ipv6.h"
#include "osapi_support.h"
#include "l7utils_inet_addr_api.h"

#include "snooping_defs.h"
#include "snooping_proto.h"
#include "snooping_outcalls.h"
#include "snooping_util.h"
#include "snooping_debug.h"
#include "snooping_api.h"
#include "snooping_db.h"
#include "l7_mgmd_api.h"
#include "dhcp_snooping_api.h"
#include "ptin_mgmd_eventqueue.h"

/* PTin added: IGMP snooping */
#if 1
  #include "snooping_ptin_db.h"
  #include "snooping_ptin_defs.h"
  #include "logger.h" // DFF
  #include "ptin_igmp.h"
  #include "ptin_debug.h"

/* PTin Add: IGMPv3 */
  #if SNOOP_PTIN_IGMPv3_GLOBAL

    #if SNOOP_PTIN_IGMPv3_PROXY

      #include "snooping_ptin_proxytimer.h"

    #endif 


  #endif


extern ptin_debug_pktTimer_t debug_pktTimer;
extern L7_uint64 hapiBroadReceice_igmp_count;
extern L7_uint64 hapiBroadReceice_mld_count;
#endif



/*****************************************************************
* @purpose  Create and send a packet event to MGMD, based on the received IGMP packet
*
* @param    outterVlan @b{ (input) } Packet's outter vlan
* @param    innerVlan @b{ (input) } Packet's inner vlan
* @param    portId @b{ (input) } Port through which the packet was received
* @param    clientId @b{ (input) } Client that sent the packet
* @param    payload @b{ (input) } Packet's payload (including L2)
* @param    payloadLength @b{ (input) } Packet length (including L2)
*
* @returns
* 
* @end
*********************************************************************/
static L7_RC_t mgmdPacketSend(L7_uint16 mcastRootVlan,L7_uint32 portId, L7_uint32 clientId, void* payload, L7_uint32 payloadLength)
{
  PTIN_MGMD_EVENT_t mgmdPcktEvent = {0};
  L7_uint32         ethernetHdrLen;
  L7_uint32         serviceId;

  //Strip L2 header from the packet before sending it to MGMD
  ethernetHdrLen = sysNetDataOffsetGet(payload);
  payload       += ethernetHdrLen;
  payloadLength -= ethernetHdrLen;

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "{");
  #ifdef IGMPASSOC_MULTI_MC_SUPPORTED
  //Determine serviceId
  if ((L7_uint16) -1 == mcastRootVlan)
  {
    serviceId = PTIN_MGMD_SERVICE_ID;
  }
  else
  #endif
  {
    ptin_timer_start(35,"ptin_evc_get_evcIdfromIntVlan");
    if (ptin_evc_get_evcIdfromIntVlan(mcastRootVlan, &serviceId)!=L7_SUCCESS)
    {
      ptin_timer_stop(35);
      LOG_ERR(LOG_CTX_PTIN_IGMP, "No EVC associated to internal vlan %u", mcastRootVlan);
      return L7_FAILURE;
    }
    ptin_timer_stop(35);
  }

  //Create a new MGMD packet event
  ptin_timer_start(36,"ptin_mgmd_event_packet_create");
  if(L7_SUCCESS != ptin_mgmd_event_packet_create(&mgmdPcktEvent, serviceId, portId, clientId, (void*) payload, payloadLength))
  {
    ptin_timer_stop(36);
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to create packet for MGMD");
    
    return L7_ERROR;
  }
  ptin_timer_stop(36);

  ptin_timer_start(37,"ptin_mgmd_eventQueue_tx");
  if(L7_SUCCESS != ptin_mgmd_eventQueue_tx(&mgmdPcktEvent))
  {
    ptin_timer_stop(37);
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to place packet event in MGMD rxQueue");
    return L7_ERROR;
  }
  ptin_timer_stop(37);
 

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Packet Send}");
  return L7_SUCCESS;
}

/* PTin added: IGMPv3 snooping */
#if SNOOP_PTIN_IGMPv3_PROXY

/************************************************************************************************************/
/*PTin Added: MGMD Proxy*/

/*****************************************************************
* @purpose  calculates the max response time from the max response code
*
* @param    max_resp_time @b{ (input) } the maximum response time
*
* @returns
* @notes
*


   If Max Resp Code < 128, Max Resp Time = Max Resp Code

   If Max Resp Code >= 128, Max Resp Code represents a floating-point
   value as follows:

       0 1 2 3 4 5 6 7
      +-+-+-+-+-+-+-+-+
      |1| exp | mant  |
      +-+-+-+-+-+-+-+-+

   Max Resp Time = (mant | 0x10) << (exp + 3)
* @end
*********************************************************************/

static L7_int32  snoopPTinProxy_decode_max_resp_code(L7_uchar8 family, L7_int32 max_resp_code)
{
  L7_int32           max_resp_time = 0;

#if  !SNOOP_PTIN_CISCO_MAX_RESPONSE_CODE_BUG
  if (max_resp_code < 0x80)
  {
    max_resp_time= max_resp_code;
  }
  else
  {
   
    max_resp_time= ((max_resp_code & 0x0F) | 0x10) << (((max_resp_code & 0x70) >>4)+3);
  }
#else
//Since Cisco equiments consider Max Response Code=Max Response Time, we will behave accordingly 
   max_resp_time= max_resp_code;
#endif

  if (family == L7_AF_INET)
  {
    max_resp_time = max_resp_time/10;
  }
  else if (family == L7_AF_INET6)
  {
    max_resp_time = max_resp_time/1000;
  }
  return max_resp_time;
}

/*****************************************************************
* @purpose  calculates the selected delay from the max response time
*
* @param    max_resp_time @b{ (input) } the maximum response time
*
* @returns  the selected delay for a
   response is randomly selected in the range (0, [Max Resp Time]) where
   Max Resp Time is derived from Max Resp Code in the received Query
   message.
* @notes  
*

* @end
*********************************************************************/


static L7_int32 snoopPTinProxy_selected_delay_calculate (L7_int32 maxResponseTime)
{
#if 0
#include"rng_api.h"//MMELO
#include"osapi.h"//MMELO
#endif
  L7_int32 selectedDelay;

#if 0
  extern L7_double64 L7_Random(void);
  L7_double64 randval;

  /* Initialize the random number generator */
  randInitRNG((L7_long32)osapiTimeMillisecondsGet());

  randval =  L7_Random();
#endif
 
  if ((selectedDelay=maxResponseTime/2)<PTIN_IGMP_MIN_UNSOLICITEDREPORTINTERVAL)
    selectedDelay=PTIN_IGMP_DEFAULT_UNSOLICITEDREPORTINTERVAL;
  
 return(selectedDelay);
}

/*End PTin Added: MGMD Proxy*/
/************************************************************************************************************/
#endif 

/*****************************************************************************
* @purpose  Callback function to Copy and process IGMP and PIM control frames
*
* @param    hookId        @b{(input)} The hook location
* @param    bufHandle     @b{(input)} Handle to the frame to be processed
* @param    pduInfo       @b{(input)} Pointer to info about this frame
* @param    continueFunc  @b{(input)} Optional pointer to a continue function
*
* @returns  SYSNET_PDU_RC_COPIED   If frame has been copied;
*                                  continue processing it
* @returns  SYSNET_PDU_RC_IGNORED  If frame has been ignored
*                                  continue processing it
*
* @notes    none
*
* @end
****************************************************************************/
SYSNET_PDU_RC_t snoopIGMPPktIntercept(L7_uint32 hookId,
                                      L7_netBufHandle bufHandle,
                                      sysnet_pdu_info_t *pduInfo,
                                      L7_FUNCPTR_t continueFunc)
{
  L7_uchar8     *data, *protPtr;

  SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "snoopIGMPPktIntercept");

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  protPtr = (data + sysNetDataOffsetGet(data)) + SNOOP_IP_HDR_NEXT_PROTO_OFFSET;

  if (*protPtr == IP_PROT_IGMP || *protPtr == IP_PROT_PIM)
  {
    debug_pktTimer.pkt_intercept_counter++;
    hapiBroadReceice_igmp_count++;

    ptin_timer_start(72,"snoopPacketHandle");
    if (snoopPacketHandle(bufHandle, pduInfo, L7_AF_INET) == L7_SUCCESS)
    {
      ptin_timer_stop(72);
      return SYSNET_PDU_RC_COPIED;
    }
    ptin_timer_stop(72);
  }

  return SYSNET_PDU_RC_IGNORED;
}

/*********************************************************************
* @purpose  Callback funtion to Copy and process MLD control frames
*
* @param    hookId        @b{(input)} The hook location
* @param    bufHandle     @b{(input)} Handle to the frame to be processed
* @param    pduInfo       @b{(input)} Pointer to info about this frame
* @param    continueFunc  @b{(input)} Optional pointer to a continue function
*
* @returns  SYSNET_PDU_RC_COPIED   If frame has been copied;
*                                  continue processing it
* @returns  SYSNET_PDU_RC_IGNORED  If frame has been ignored
*                                  continue processing it
*
* @notes    none
*
* @end
*********************************************************************/
SYSNET_PDU_RC_t snoopMLDPktIntercept(L7_uint32 hookId,
                                     L7_netBufHandle bufHandle,
                                     sysnet_pdu_info_t *pduInfo,
                                     L7_FUNCPTR_t continueFunc)
{
  L7_uchar8          *data, *dataPtr;
  L7_uint32           dataOffSet;
  L7_uchar8          *protoPtr;
  L7_ip6ExtHeader_t   ip6ExtHdr;
  L7_uint32           extLength = 0;
  snoop_eb_t         *pSnoopEB;

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  dataOffSet = sysNetDataOffsetGet(data);

  protoPtr = data + dataOffSet + SNOOP_IP6_HDR_NEXT_PROTO_OFFSET;
  dataPtr = data + dataOffSet + (sizeof(L7_uchar8) * L7_IP6_HEADER_LEN);
  /* Support for MLD packets with IPv6 options header is not supported
     by all hardwares.
  */
  if ((pSnoopEB = snoopEBGet()) == L7_NULLPTR)
  {
    return SYSNET_PDU_RC_IGNORED;
  }

  if (*protoPtr == SNOOP_IP6_IPPROTO_HOPOPTS)
  {
    memcpy(&ip6ExtHdr, dataPtr, sizeof(L7_ip6ExtHeader_t));
    if (ip6ExtHdr.xnext != IP_PROT_ICMPV6)
    {
      return SYSNET_PDU_RC_IGNORED;
    }

    extLength = SNOOP_IP6_HOPBHOP_LEN_GET(ip6ExtHdr.xlen);
    dataPtr   = data + dataOffSet + (sizeof(L7_uchar8) * L7_IP6_HEADER_LEN)
                + extLength;
  }
  else if (*protoPtr != IP_PROT_ICMPV6)
  {
    return SYSNET_PDU_RC_IGNORED;
  }

  hapiBroadReceice_mld_count++;

  if (pSnoopEB->ipv6OptionsSupport == L7_TRUE)
  {

    if (*dataPtr == L7_MLD_MEMBERSHIP_QUERY ||
        *dataPtr == L7_MLD_V1_MEMBERSHIP_REPORT ||
        *dataPtr == L7_MLD_V1_MEMBERSHIP_DONE ||
        *dataPtr == L7_MLD_V2_MEMBERSHIP_REPORT)
    {
      if (snoopPacketHandle(bufHandle, pduInfo, L7_AF_INET6) == L7_SUCCESS)
      {
        return SYSNET_PDU_RC_COPIED;
      }
    } /* End of valid MLD Packet check */
  }
  else
  {
    if ((*protoPtr == IP_PROT_ICMPV6 && 
         (*dataPtr == L7_MLD_MEMBERSHIP_QUERY || 
          *dataPtr == L7_MLD_V1_MEMBERSHIP_REPORT || 
          *dataPtr == L7_MLD_V1_MEMBERSHIP_DONE ||
          *dataPtr == L7_MLD_V2_MEMBERSHIP_REPORT))||
        (*protoPtr == SNOOP_IP6_IPPROTO_HOPOPTS &&
         (*dataPtr == L7_MLD_MEMBERSHIP_QUERY ||
          *dataPtr == L7_MLD_V2_MEMBERSHIP_REPORT))
       )
    {
      if (snoopPacketHandle(bufHandle, pduInfo, L7_AF_INET6) == L7_SUCCESS)
      {
        return SYSNET_PDU_RC_COPIED;
      }
    } /* End of valid MLD Packet check */
  }

  return SYSNET_PDU_RC_IGNORED;
}

/***********************************************************************
* @purpose Function to handle incoming snoop control packets
*
* @param   netBufHandle  @b{(input)}  Buffer handle to the snoop control
*                                     frame received
* @param   pduInfo       @b{(input)}  Pointer to PDU info
* @param   family        @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns L7_SUCCESS  Packet copied into local buffer
* @returns L7_FAILURE  Packet ignored
*
* @notes none
*
* @end
*
***********************************************************************/
L7_RC_t snoopPacketHandle(L7_netBufHandle netBufHandle,
                          sysnet_pdu_info_t *pduInfo,
                          L7_uchar8 family)
{
  snoopPDU_Msg_t     msg;
  L7_uchar8         *data;
  L7_uint32          dataLength;
  L7_uint32          dot1qMode;
  L7_INTF_TYPES_t    sysIntfType;
  L7_RC_t            rc;
  snoop_cb_t        *pSnoopCB      = L7_NULLPTR;  
  L7_uchar8         *igmpPtr       = L7_NULLPTR;
  L7_uint32          client_idx    = (L7_uint32) -1;              /* PTin added: IGMP snooping */
  L7_uint16          mcastRootVlan; /* Internal vlan will be converted to MC root vlan */
  L7_uint8           port_type;
 
  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"{");

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    LOG_FATAL(LOG_CTX_PTIN_IGMP,"Snoop Control Block returned null pointer");
    return L7_FAILURE;
  }

  /* If message queue is not yet created, return error. */
  if (family == L7_AF_INET)
  {
    if (!pSnoopCB->snoopExec->snoopIGMPQueue)
    {
      LOG_FATAL(LOG_CTX_PTIN_IGMP,"IGMP message queue is not yet created");
      return L7_FAILURE; /* In case of failure, the buffer is freed by DTL */
    }
  }
  else
  {
    if (!pSnoopCB->snoopExec->snoopMLDQueue)
    {
      LOG_FATAL(LOG_CTX_PTIN_IGMP,"MLD message queue is not yet created");
      return L7_FAILURE; /* In case of failure, the buffer is freed by DTL */
    }
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP,
            "Packet intercepted vlan %d, innerVlan=%u, intIfNum %u, rx_port=%u",
            pduInfo->vlanId, pduInfo->innerVlanId, pduInfo->intIfNum, pduInfo->rxPort);

  /* Ensure snooping is enabled on the switch */
  if (pSnoopCB->snoopCfgData->snoopAdminMode != L7_ENABLE)
  {
    LOG_NOTICE(LOG_CTX_PTIN_IGMP,"Snoop Admin mode is disabled");
    return L7_FAILURE; /* In case of failure, the buffer is freed by DTL */
  }

  /* Validate intIfNum */
  if ((pduInfo->intIfNum == 0) || (pduInfo->intIfNum >= PTIN_SYSTEM_MAXINTERFACES_PER_GROUP))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid intIfNum %u: out or range (max=%u)", pduInfo->intIfNum, PTIN_SYSTEM_MAXINTERFACES_PER_GROUP-1);
    return L7_FAILURE;
  }

  /* Get interface type */
  if (nimGetIntfType(pduInfo->intIfNum, &sysIntfType) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid interface type");
    return L7_FAILURE;
  }

  /* If not received on CPU interface, verify the incoming interface */
  if (sysIntfType != L7_CPU_INTF)
  {
    /* Verify that the receiving interface is a member of the receiving VLAN */
    if ( (dot1qOperVlanMemberGet(pduInfo->vlanId, pduInfo->intIfNum,
                                 &dot1qMode) != L7_SUCCESS) ||
         (dot1qMode != L7_DOT1Q_FIXED) )
    {
      ptin_igmp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, SNOOP_STAT_FIELD_IGMP_DROPPED);
      return L7_FAILURE;
    }

    /* Verify that the receiving interface is valid */
    if (snoopIntfCanBeEnabled(pduInfo->intIfNum, pduInfo->vlanId) != L7_TRUE)
    {
      ptin_igmp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, SNOOP_STAT_FIELD_IGMP_DROPPED);
      return L7_FAILURE;
    }
  }

  /* Get start and length of incoming frame */
  SYSAPI_NET_MBUF_GET_DATASTART(netBufHandle, data);
  SYSAPI_NET_MBUF_GET_DATALENGTH(netBufHandle, dataLength);

  if (ptin_debug_igmp_packet_trace)
  {    
    L7_uint32 i;
    printf("Rx:PayloadLength:%d\n",dataLength);
    for (i=0;i<dataLength;i++)
      printf("%02x ",data[i]);
    printf("\n");
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP, "DMAC=%02x:%02x:%02x:%02x:%02x:%02x SMAC=%02x:%02x:%02x:%02x:%02x:%02x",
              data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7],data[8],data[9],data[10],data[11]);

#ifdef L7_DHCP_SNOOPING_PACKAGE
#ifdef L7_IPSG_PACKAGE
  L7_enetMacAddr_t   clientMacAddr;
  memcpy(&clientMacAddr.addr, &data[6], sizeof(clientMacAddr));   
  if ( ipsgClientAuthorized(&clientMacAddr, pduInfo->vlanId, pduInfo->intIfNum) != L7_TRUE )
  {
    L7_uchar8 macAddrStr[SNOOP_MAC_STR_LEN];      
    snoopMacToString(clientMacAddr.addr, macAddrStr);

    if(ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Packet Silently Ignored. Client Not Authorized! [intIfNum:%u vlanId:%u macAddrStr:%s]", pduInfo->intIfNum, pduInfo->vlanId, macAddrStr);
    ptin_igmp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, SNOOP_STAT_FIELD_IGMP_DROPPED);
    return L7_FAILURE;
  }
#endif
#endif
  
/*If we support Inband*/
#if PTIN_EVC_INBAND_SUPPORT 
  if (pduInfo->vlanId == PTIN_VLAN_INBAND)
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Packet Silently Ignored. Inband Vlan! [intIfNum:%u vlanId:%u]", pduInfo->intIfNum, pduInfo->vlanId);
    return L7_SUCCESS;
  }
#endif

  /* PTin added: IGMP snooping */
#if 1
   ptin_timer_start(73,"ptin_igmp_clientIntfVlan_validate");
  /*Get Port Type*/   
  rc = ptin_evc_internal_vlan_port_type_get(pduInfo->vlanId, pduInfo->intIfNum, &port_type);
  ptin_timer_stop(73);
  if (rc != L7_SUCCESS)
  {
    L7_enetMacAddr_t   clientMacAddr;
    memcpy(&clientMacAddr.addr, &data[6], sizeof(clientMacAddr));  
    L7_uchar8 macAddrStr[SNOOP_MAC_STR_LEN];      
    snoopMacToString(clientMacAddr.addr, macAddrStr);

    LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Packet Silently Ignored. Failed to Obtain Port Type! [intIfNum:%u vlanId:%u macAddrStr:%s]", pduInfo->intIfNum, pduInfo->vlanId, macAddrStr);
    ptin_igmp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, SNOOP_STAT_FIELD_IGMP_DROPPED);    
    return L7_FAILURE;
  }  

  /* Search for client index if this is a leaf port*/
   if (port_type == PTIN_EVC_INTF_LEAF)
   {
     ptin_timer_start(74,"ptin_igmp_clientIndex_get");
     #if ( PTIN_BOARD_IS_MATRIX )     
     rc = ptin_igmp_clientIndex_get(pduInfo->intIfNum,
                                L7_NULL, L7_NULL,
                                L7_NULL,
                                &client_idx);
     #else
     rc =ptin_igmp_clientIndex_get(pduInfo->intIfNum,
                              pduInfo->vlanId, pduInfo->innerVlanId,
                              &data[L7_MAC_ADDR_LEN],
                              &client_idx);
     #endif
     if (rc != L7_SUCCESS)
     {
       ptin_timer_stop(74);  
       /*Client Does Not Exist*/
       if (rc == L7_NOT_EXIST)
       {
         #ifdef IGMP_DYNAMIC_CLIENTS_SUPPORTED
          LOG_TRACE(LOG_CTX_PTIN_IGMP,"Client Does Not Exist: (intIfNum=%u vlan=%u innerVlanId=%u", rc, pduInfo->intIfNum, pduInfo->vlanId, pduInfo->innerVlanId);  
         #else         
          LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed (rc:%u) to obtain clientId (intIfNum=%u vlan=%u innerVlanId=%u", rc, pduInfo->intIfNum, pduInfo->vlanId, pduInfo->innerVlanId);  
          return L7_FAILURE;
         #endif
       }
       else
       {
         /*Abort Here*/
         ptin_igmp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, SNOOP_STAT_FIELD_IGMP_DROPPED);
         LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed (rc:%u) to obtain clientId  (intIfNum=%u vlan=%u innerVlanId=%u", rc, pduInfo->intIfNum, pduInfo->vlanId, pduInfo->innerVlanId);  
         return L7_FAILURE;
       }
     }
     else /*rc == L7_SUCCESS*/
     {       
       ptin_timer_stop(74);  

       /* Validate client index */
       if (client_idx>=PTIN_IGMP_CLIENTIDX_MAX)
       {
         /*Abort Here*/
         ptin_igmp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, SNOOP_STAT_FIELD_IGMP_DROPPED);
         LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid  clientId:%u : (intIfNum=%u vlan=%u innerVlanId=%u", client_idx, pduInfo->intIfNum, pduInfo->vlanId, pduInfo->innerVlanId);  
         return L7_FAILURE;
       }
       else
       {
         LOG_TRACE(LOG_CTX_PTIN_IGMP,"Obtained  clientId:%u: (intIfNum=%u vlan=%u innerVlanId=%u", client_idx, pduInfo->intIfNum, pduInfo->vlanId, pduInfo->innerVlanId);  
       }
     }
   }  

#ifdef IGMP_DYNAMIC_CLIENTS_SUPPORTED
  /* For leaf interfaces */
  if ( port_type == PTIN_EVC_INTF_LEAF )
  {
    if (client_idx>=PTIN_IGMP_CLIENTIDX_MAX)
    {
      ptin_timer_start(75,"ptin_igmp_dynamic_client_add");
      #if (PTIN_BOARD_IS_MATRIX)
      /* If the client does not exist, it will be created in dynamic mode */
      rc = ptin_igmp_dynamic_client_add(pduInfo->intIfNum,
                                       L7_NULL, L7_NULL,
                                       L7_NULL,
                                       &client_idx);
      #else
      /* For Linecard only: If client was not recognized, add it as dynamic */
      rc = ptin_igmp_dynamic_client_add(pduInfo->intIfNum,
                                       pduInfo->vlanId, pduInfo->innerVlanId,
                                       &data[L7_MAC_ADDR_LEN],
                                       &client_idx);
      #endif

      if (rc != L7_SUCCESS)
      {        
        ptin_timer_stop(75);            
        ptin_igmp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, SNOOP_STAT_FIELD_IGMP_DROPPED);
        LOG_ERR(LOG_CTX_PTIN_IGMP,"intIfNum=%u,vlan=%u innerVlanId:%u are not accepted", pduInfo->intIfNum, pduInfo->vlanId, pduInfo->innerVlanId);        
        return L7_FAILURE;
      }
      else
      {
        if (client_idx>=PTIN_IGMP_CLIENTIDX_MAX)
        {
          ptin_igmp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, SNOOP_STAT_FIELD_IGMP_DROPPED);
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Invalid client_idx:%u (intIfNum=%u,vlan=%u innerVlanId=%u)", client_idx, pduInfo->intIfNum, pduInfo->vlanId, pduInfo->innerVlanId);    
          return L7_FAILURE;
        }
        else
        {
          LOG_TRACE(LOG_CTX_PTIN_IGMP,"Created new  clientId:%u: (intIfNum=%u vlan=%u innerVlanId=%u", client_idx, pduInfo->intIfNum, pduInfo->vlanId, pduInfo->innerVlanId);    
        }
      }
    }
  }
#endif

#ifdef IGMPASSOC_MULTI_MC_SUPPORTED 
  L7_uchar8       *buffPtr          = L7_NULLPTR;
  L7_uint16        ipHdrLen         = 0;   
  L7_inet_addr_t   groupAddr;
  L7_inet_addr_t   sourceAddr;
  L7_uint16        noOfGroupRecords = 1;
  L7_uint16        noOfSources;
  char             groupAddrStr[IPV6_DISP_ADDR_LEN]={}; 

  /* Set Group Address to Zero*/
  inetAddressZeroSet(family, &groupAddr);

  /*Set Source Address to Zero*/
  inetAddressZeroSet(family, &sourceAddr);

  /* IGMP */
  if (pSnoopCB->family == family)
  {
    /* Validate minimum size of packet */
    if (dataLength < L7_ENET_HDR_SIZE + L7_ENET_HDR_TYPE_LEN_SIZE + L7_IP_HDR_LEN + SNOOP_IGMPv1v2_HEADER_LENGTH)
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Received pkt is too small %d",dataLength);
      ptin_igmp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, client_idx, SNOOP_STAT_FIELD_IGMP_RECEIVED_INVALID);
      return L7_FAILURE;
    }

    /* Extract source and group address from packet */

    /* Point to the start of ethernet payload */
    buffPtr = (L7_uchar8 *)(data + sysNetDataOffsetGet(data));

    ipHdrLen = (buffPtr[0] & 0x0f)*4;
    if ( ipHdrLen < L7_IP_HDR_LEN)
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "IP Header Len is invalid %d",ipHdrLen);
      ptin_igmp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, client_idx, SNOOP_STAT_FIELD_IGMP_RECEIVED_INVALID);
      return L7_FAILURE;
    }
    if ((L7_ENET_HDR_SIZE + L7_ENET_HDR_TYPE_LEN_SIZE + ipHdrLen + SNOOP_IGMPv1v2_HEADER_LENGTH) > dataLength)
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP, "IP Header Len is too big (%u) for the packet length (%u)", ipHdrLen, dataLength);
      ptin_igmp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, client_idx, SNOOP_STAT_FIELD_IGMP_RECEIVED_INVALID);
      return L7_FAILURE;
    }

    /* Starting of IGMP header */
    igmpPtr = (L7_uchar8 *) &buffPtr[ipHdrLen];

    /* Group address */
    /* For V1/V2 and query messages, the group address is always located at the same place */
    if (igmpPtr[0] == L7_IGMP_MEMBERSHIP_QUERY ||
        igmpPtr[0] == L7_IGMP_V1_MEMBERSHIP_REPORT ||
        igmpPtr[0] == L7_IGMP_V2_MEMBERSHIP_REPORT ||
        igmpPtr[0] == L7_IGMP_V2_LEAVE_GROUP)
    {      
      /*Get Multicast Group Address*/         
      groupAddr.addr.ipv4.s_addr = *(L7_uint32 *) ((L7_uint8 *) &igmpPtr[4]);
      
      //Convert to Little Endian
      groupAddr.addr.ipv4.s_addr = osapiNtohl(groupAddr.addr.ipv4.s_addr);
    }
    else if (igmpPtr[0] == L7_IGMP_V3_MEMBERSHIP_REPORT)
    {
      /*Get Number of Group Records*/
      noOfGroupRecords = osapiNtohs(*((L7_uint16 *) &igmpPtr[6]));
      
      
      if ( noOfGroupRecords > 0 )
      { 
        /*Get Multicast Group Address of First Group Record*/         
        groupAddr.addr.ipv4.s_addr = *(L7_uint32 *) ((L7_uint8 *) &igmpPtr[12]);

        //Convert to Little Endian
        groupAddr.addr.ipv4.s_addr = osapiNtohl(groupAddr.addr.ipv4.s_addr);

        /*Get Number of Sources of First Group Address*/
        noOfSources = osapiNtohs(*((L7_uint16 *) &igmpPtr[10]));    

        if (noOfSources == 0)
        {
          //AnySource
        }
        else
        {
          /*Get Source  Address of First Group Record*/         
          sourceAddr.addr.ipv4.s_addr = *(L7_uint32 *) ((L7_uint8 *) &igmpPtr[16]);

          //Convert to Little Endian
          sourceAddr.addr.ipv4.s_addr = osapiNtohl(sourceAddr.addr.ipv4.s_addr);
        }        
      }
      else
      {
        LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Number of Group Records:%u [vlan=%u innerVlan=%u client_idx]: Packet Silently ignored...",
                noOfGroupRecords, pduInfo->vlanId, pduInfo->innerVlanId, client_idx);  
        ptin_igmp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, client_idx, SNOOP_STAT_FIELD_IGMP_RECEIVED_INVALID);
        return L7_FAILURE;
      }      
    }
    else
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Protocol Not Supported :%u [vlan=%u innerVlan=%u client_idx]", igmpPtr[0], pduInfo->vlanId, pduInfo->innerVlanId, client_idx);
      ptin_igmp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, client_idx, SNOOP_STAT_FIELD_IGMP_RECEIVED_INVALID);
      return L7_NOT_SUPPORTED;
    }
     
    /*RFC5771 - Local Network Control Block (224.0.0.0 - 224.0.0.255 (224.0.0/24)) 
    The range of addresses between 224.0.0.0 and 224.0.0.255, inclusive, is reserved for the use of routing protocols and other low-level topology discovery or maintenance protocols, such as gateway discovery
    and group membership reporting.  Multicast routers should not forward any multicast datagram with destination addresses in this range, regardless of its TTL.*/    
    if(groupAddr.addr.ipv4.s_addr >= L7_IP_MCAST_BASE_ADDR && groupAddr.addr.ipv4.s_addr <= L7_IP_MAX_LOCAL_MULTICAST )    
    {
      if(ptin_debug_igmp_snooping)
        LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Multicast Group Address is Reserved for Protocol use [vlan=%u innerVlan=%u client_idx=%u grpAddr=%s]. Packet Silently ignored...",
                pduInfo->vlanId, pduInfo->innerVlanId, client_idx, inetAddrPrint(&groupAddr,groupAddrStr));  
      if(igmpPtr!=L7_NULLPTR)
        ptin_igmp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, client_idx, snoopPacketType2IGMPStatField(igmpPtr[0],SNOOP_STAT_FIELD_VALID_RX));
      else
      {      
        ptin_igmp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, client_idx, SNOOP_STAT_FIELD_IGMP_RECEIVED_VALID);    
      }      
      return L7_SUCCESS;
    }
  }
  else
  {
    LOG_NOTICE(LOG_CTX_PTIN_IGMP, "IPv6 not supported yet!");
    ptin_igmp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, client_idx, SNOOP_STAT_FIELD_IGMP_RECEIVED_INVALID);
    return L7_FAILURE;
  }

  ptin_timer_start(76,"ptin_igmp_McastRootVlan_get");
  /* Get multicast root vlan */  
  if ( ptin_igmp_McastRootVlan_get(pduInfo->vlanId, pduInfo->intIfNum, (port_type == PTIN_EVC_INTF_LEAF), client_idx, &groupAddr, &sourceAddr, &mcastRootVlan) == L7_SUCCESS )
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Vlan=%u will be converted to %u (grpAddr=%s)",
              pduInfo->vlanId, mcastRootVlan, inetAddrPrint(&groupAddr,groupAddrStr));
    ptin_timer_stop(76);
  }
  else 
  {
    ptin_timer_stop(76);
    if (noOfGroupRecords == 1 || igmpPtr[0] != L7_IGMP_V3_MEMBERSHIP_REPORT)
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Can't get McastRootVlan for vlan=%u (grpAddr=%s). Packet Silently ignored...",
                pduInfo->vlanId, inetAddrPrint(&groupAddr,groupAddrStr));    
      if(igmpPtr!=L7_NULLPTR)
        ptin_igmp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, client_idx, snoopPacketType2IGMPStatField(igmpPtr[0],SNOOP_STAT_FIELD_DROPPED_RX));
      else
      {      
        ptin_igmp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, client_idx, SNOOP_STAT_FIELD_IGMP_DROPPED);    
      }        
      return L7_FAILURE;
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Can't get McastRootVlan for vlan=%u (grpAddr=%s): Packet with more group records left:%u",
                pduInfo->vlanId, inetAddrPrint(&groupAddr,groupAddrStr), noOfGroupRecords);    
      mcastRootVlan = (L7_uint16) -1;
    }
  }
#else
  ptin_timer_start(76,"ptin_igmp_McastRootVlan_get");
  /* !IGMPASSOC_MULTI_MC_SUPPORTED */
  if (ptin_igmp_McastRootVlan_get(pduInfo->vlanId, &mcastRootVlan)==L7_SUCCESS)
  {
    ptin_timer_stop(76);
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Vlan=%u will be converted to %u",pduInfo->vlanId,mcastRootVlan);
  }
  else
  {
    ptin_timer_stop(76);
    LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Can't get McastRootVlan for vlan=%u. Packet Silently ignored...",pduInfo->vlanId);
    if(igmpPtr!=L7_NULLPTR)
      ptin_igmp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, client_idx, snoopPacketType2IGMPStatField(igmpPtr[0],SNOOP_STAT_FIELD_DROPPED_RX));
    else
    {      
      ptin_igmp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, client_idx, SNOOP_STAT_FIELD_IGMP_DROPPED);    
    }          
    return L7_FAILURE;
  }
#endif

  /* Validate interface and Multicast root vlan */
  if (
      #ifdef IGMPASSOC_MULTI_MC_SUPPORTED
      mcastRootVlan != (L7_uint16) -1 &&
      #endif
      ptin_igmp_intfVlan_validate(pduInfo->intIfNum, mcastRootVlan)!=L7_SUCCESS)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP,"intIfNum=%u,vlan=%u are not accepted",pduInfo->intIfNum,pduInfo->vlanId);
    if(igmpPtr!=L7_NULLPTR)
      ptin_igmp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, client_idx, snoopPacketType2IGMPStatField(igmpPtr[0],SNOOP_STAT_FIELD_DROPPED_RX));
    else
    {      
      ptin_igmp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, client_idx, SNOOP_STAT_FIELD_IGMP_DROPPED);    
    }        
    return L7_FAILURE;
  }

  /* Update Vlan ID of the root MC service vlan */
  #ifdef IGMPASSOC_MULTI_MC_SUPPORTED
  if (igmpPtr[0] == L7_IGMP_V3_MEMBERSHIP_REPORT)
  {
    pduInfo->vlanId = PTIN_MGMD_SERVICE_ID;
  }
  else
  #endif  
  { 
    pduInfo->vlanId = mcastRootVlan;
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Packet intercepted at intIfNum=%u, oVlan=%u, iVlan=%u",
            pduInfo->intIfNum, mcastRootVlan, pduInfo->innerVlanId);

  /* Change Internal vlan with the MC root vlan inside message */
  if ((*(L7_ushort16 *)&data[12] == 0x8100) ||
      (*(L7_ushort16 *)&data[12] == 0x88A8) ||
      (*(L7_ushort16 *)&data[12] == 0x9100))
  {
    data[14] &= 0xf0;
    data[14] |= (msg.vlanId>>8) & 0x0f;
    data[15]  =  msg.vlanId & 0xff;
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"vlan changed inside packet");
  }
#endif

  memset((L7_uchar8 *)&msg, 0, sizeof(msg));
  msg.msgId    = snoopPDUMsgRecv;
  msg.intIfNum = pduInfo->intIfNum;
  msg.innerVlanId = pduInfo->innerVlanId;
  msg.vlanId   = /*osapiNtohl already done by hapiRx function*/(pduInfo->vlanId);
  msg.cbHandle = pSnoopCB;
  msg.client_idx = client_idx;          /* PTin added: IGMP snooping */

  /* Put a copy of the incoming frame into one of Snooping's buffer pools.
   * This will allow other components to process the original MBUF
   */
  if (dataLength <= SNOOP_SMALL_BUFFER_SIZE)
  {
    rc = bufferPoolAllocate(pSnoopCB->snoopExec->snoopSmallBufferPoolId,
                            &(msg.snoopBuffer));
    msg.snoopBufferPoolId = pSnoopCB->snoopExec->snoopSmallBufferPoolId;
  }
  else if (dataLength <= SNOOP_MED_BUFFER_SIZE)
  {
    rc = bufferPoolAllocate(pSnoopCB->snoopExec->snoopMedBufferPoolId,
                            &(msg.snoopBuffer));
    msg.snoopBufferPoolId = pSnoopCB->snoopExec->snoopMedBufferPoolId;
  }
  else
  {
    rc = bufferPoolAllocate(pSnoopCB->snoopExec->snoopLargeBufferPoolId,
                            &(msg.snoopBuffer));
    msg.snoopBufferPoolId = pSnoopCB->snoopExec->snoopLargeBufferPoolId;
  }

  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Insufficient buffers");
    if(igmpPtr!=L7_NULLPTR)
    {
      ptin_igmp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, client_idx, snoopPacketType2IGMPStatField(igmpPtr[0],SNOOP_STAT_FIELD_DROPPED_RX));
    }
    else
    {
      ptin_igmp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, client_idx, SNOOP_STAT_FIELD_IGMP_DROPPED);    
    }

    //ptin_igmp_dynamic_client_flush(pduInfo->vlanId,client_idx);
    return L7_FAILURE;
  }

  if(ptin_debug_igmp_snooping)
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to send message to queue");

    L7_int32 n_msg = -1;
    if (osapiMsgQueueGetNumMsgs(pSnoopCB->snoopExec->snoopIGMPQueue, &n_msg)==L7_SUCCESS)
    {
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Size of IGMP queue = %u messages",n_msg);
    }
    else
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Error reading IGMP queue size");
    }
  }

  memcpy(msg.snoopBuffer, data, dataLength);
  msg.dataLength = dataLength;

#if PTIN_SNOOP_USE_MGMD
  /* Send packet to MGMD */
  ptin_timer_start(34,"mgmdPacketSend");
  if(L7_SUCCESS != (rc = mgmdPacketSend(mcastRootVlan, msg.intIfNum, client_idx, (void*) msg.snoopBuffer, msg.dataLength)))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to send packet to MGMD");
  }
  ptin_timer_stop(34);
#else
  if (pSnoopCB->family == L7_AF_INET)
  {
    rc = osapiMessageSend(pSnoopCB->snoopExec->snoopIGMPQueue,
                          &msg, SNOOP_PDU_MSG_SIZE, L7_NO_WAIT,
                          L7_MSG_PRIORITY_NORM);
  }
  else
  {
    rc = osapiMessageSend(pSnoopCB->snoopExec->snoopMLDQueue,
                          &msg, SNOOP_PDU_MSG_SIZE, L7_NO_WAIT,
                          L7_MSG_PRIORITY_NORM);
  }

  if (rc == L7_SUCCESS)
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Message sent to queue");
    if (osapiSemaGive(pSnoopCB->snoopExec->snoopMsgQSema) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to give msgQueue semaphore");
    }
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to post a %s message to queue",pSnoopCB->family==L7_AF_INET?"IGMP":"MLD");
  }
#endif

  bufferPoolFree(msg.snoopBufferPoolId, msg.snoopBuffer);

  if (ptin_debug_igmp_snooping)
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"}");

  return L7_SUCCESS;
}
/*************************************************************************
* @purpose Parse and place the information in a data structure for
*          further processing
*
* @param   msg          @b{(input)}  Pointer to received snoop control
*                                    message
* @param   mcastPacket  @b{(output)} Pointer to data structure used
*                                    internally for processing
*
* @returns L7_SUCCESS  Valid packet
* @returns L7_FAILURE  Packet to be dropped
*
* @notes none
*
* @end
*
*************************************************************************/
static L7_RC_t snoopPacketParse(snoopPDU_Msg_t *msg,
                                mgmdSnoopControlPkt_t *mcastPacket)
{
  L7_uint32      /*intfMode,*/ xtenHdrLen;
  L7_uint32      messageLen, ipHdrLen/*,  vlanMode*/;
  L7_INTF_TYPES_t sysIntfType;
  L7_uchar8     *data, byteVal, *startPtr;
  L7_uchar8     *pEtype;
  L7_ushort16    protocolType;
  L7_uchar8     *buffPtr;
  L7_ipHeader_t  ip_header;
  L7_ip6Header_t ip6_header;
  L7_in6_addr_t  ipv6Addr;
  L7_uint32      lenIcmpData;
  static ipv6pkt_t ipv6pkt; /* packet with pseudo header for checksum
                             calculation */

  /* Determine the protocol type and verify that it is type IP.
  ** Also, strip any VLAN tag.  The tag will be added on egress if necessary.
  */
  data       = msg->snoopBuffer;
  messageLen = msg->dataLength;
  pEtype     = msg->snoopBuffer + L7_ENET_HDR_SIZE;

  /* Copy the dmac of the frame for future validation */
  memcpy(mcastPacket->destMac, msg->snoopBuffer, L7_MAC_ADDR_LEN);

  SNOOP_GET_SHORT(protocolType, pEtype);
  if (protocolType == L7_ETYPE_8021Q)
  {
#ifndef L7_METRO_FLEX_PACKAGE
    L7_int32 index;

    /* This is a VLAN tagged Frame */
    /* Strip the VLAN header */
    for (index = L7_ENET_HDR_SIZE-1; index >= 0; index--)
    {
      /* Shift the Destination MAC Address and the Source MAC Address by 4 bytes.
      ** These 4 bytes are the VLAN header.
      */
      *(msg->snoopBuffer + index + L7_8021Q_ENCAPS_HDR_SIZE) = *(msg->snoopBuffer + index);
    }
    messageLen -= L7_8021Q_ENCAPS_HDR_SIZE;
    data = msg->snoopBuffer + L7_8021Q_ENCAPS_HDR_SIZE;
    pEtype = data + L7_ENET_HDR_SIZE; /* The Ethernet SA DA fields are moved */
#else
    /* The VLAN TAG is not stripped in this case */
    pEtype = data + L7_ENET_HDR_SIZE + L7_8021Q_ENCAPS_HDR_SIZE;
#endif
    SNOOP_GET_SHORT(protocolType, pEtype);
  }
  SNOOP_UNUSED_PARAM(pEtype);
  /* Verify that this is an IPv4/IPv6 frame */
  if (!((protocolType == L7_ETYPE_IP   && msg->cbHandle->family == L7_AF_INET)||
        (protocolType == L7_ETYPE_IPV6 && msg->cbHandle->family == L7_AF_INET6))
     )
  {
    snoopDebugPacketRxTrace(mcastPacket, SNOOP_PKT_DROP_BAD_ETYPE);
    return L7_FAILURE;
  }

  memset(mcastPacket->payLoad, 0x00, L7_MAX_FRAME_SIZE * sizeof(L7_uchar8));
  if (messageLen > L7_MAX_FRAME_SIZE)
  {
    messageLen = L7_MAX_FRAME_SIZE;
  }

  memcpy(mcastPacket->payLoad, data, messageLen);
  mcastPacket->length   = messageLen;

  /* now operate only on the local buffer */
  data = mcastPacket->payLoad;

  /* If received on CPU interface, flood it within the VLAN */
  if ( (nimGetIntfType(msg->intIfNum, &sysIntfType) == L7_SUCCESS) &&
       (sysIntfType == L7_CPU_INTF) )
  {
    snoopDebugPacketRxTrace(mcastPacket, SNOOP_PKT_DROP_RX_CPU);
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "Flooding packet...");
    snoopPacketFlood(mcastPacket);
    return L7_FAILURE;
  }

  /* Flood the packet if snooping is not enabled in either the VLAN or on the
     interface */
  if (msg->vlanId < L7_DOT1Q_MIN_VLAN_ID || msg->vlanId > L7_DOT1Q_MAX_VLAN_ID)
  {
    snoopDebugPacketRxTrace(mcastPacket, SNOOP_PKT_DROP_BAD_VLAN);
    return L7_FAILURE;
  }
  else
  {
    /* PTin removed */
#if 0
    vlanMode = (mcastPacket->cbHandle->snoopCfgData->snoopVlanMode[msg->vlanId] & SNOOP_VLAN_MODE) 
               ? L7_ENABLE : L7_DISABLE ;
    if (vlanMode != L7_ENABLE)
    {
      if (snoopIntfModeGet(msg->intIfNum, msg->vlanId, &intfMode,
                           mcastPacket->cbHandle->family) == L7_SUCCESS)
      {
        if (intfMode != L7_ENABLE)
        {
          SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "Flooding packet...");
          snoopPacketFlood(mcastPacket);
          return L7_FAILURE;
        }
      }
      else
      {
        return L7_FAILURE;
      } /* End of else of intf mode get */
    } /* End of VLAN Mode check */
#endif
  }/* End of VLAN Mode Get */

  /* Point to the start of ethernet payload */
  buffPtr = (L7_uchar8 *)(data + sysNetDataOffsetGet(data));
  /* IGMP */
  if (mcastPacket->cbHandle->family == L7_AF_INET)
  {
    if (mcastPacket->length < L7_ENET_HDR_SIZE +
        L7_ENET_HDR_TYPE_LEN_SIZE +
        L7_IP_HDR_LEN + SNOOP_IGMPv1v2_HEADER_LENGTH)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "Received pkt is too small %d",\
                  mcastPacket->length);
      return L7_FAILURE;
    }

    startPtr = buffPtr;
    SNOOP_GET_BYTE(ip_header.iph_versLen, buffPtr);
    SNOOP_GET_BYTE(ip_header.iph_tos, buffPtr);
    SNOOP_GET_SHORT(ip_header.iph_len, buffPtr);
    SNOOP_GET_SHORT(ip_header.iph_ident, buffPtr);
    SNOOP_GET_SHORT(ip_header.iph_flags_frag, buffPtr);
    SNOOP_GET_BYTE(ip_header.iph_ttl, buffPtr);
    SNOOP_GET_BYTE(ip_header.iph_prot, buffPtr);
    SNOOP_GET_SHORT(ip_header.iph_csum, buffPtr);
    SNOOP_GET_ADDR(&ip_header.iph_src, buffPtr);
    SNOOP_GET_ADDR(&ip_header.iph_dst, buffPtr);

    /* Required as per RFC 3376*/
    if (ip_header.iph_ttl != SNOOP_TTL_VALID_VALUE)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "Received pkt with TTL other than one %d",\
                  ip_header.iph_ttl);
      return L7_FAILURE;
    }

    /* Save tos field Required as per RFC 3376*/
    mcastPacket->tosByte = ip_header.iph_tos;

    /* Calculate the IP header length, including any IP options */
    ipHdrLen = ((ip_header.iph_versLen & 0x0f) * 4);

    /*check for router alert option  Required as per RFC 3376**/
    if ((ipHdrLen > L7_IP_HDR_LEN) &&  (*buffPtr ==IGMP_IP_ROUTER_ALERT_TYPE))
    {
      mcastPacket->routerAlert =L7_TRUE;
    }
    else
    {
      mcastPacket->routerAlert =L7_FALSE;
    }

    buffPtr += (ipHdrLen - L7_IP_HDR_LEN);
    mcastPacket->ip_payload = buffPtr;

    inetAddressSet(L7_AF_INET, &ip_header.iph_src, &(mcastPacket->srcAddr));
    inetAddressSet(L7_AF_INET, &ip_header.iph_dst, &(mcastPacket->destAddr));

    if ((L7_INET_IS_ADDR_BROADCAST(&(mcastPacket->srcAddr))) ||
        (L7_INET_IS_ADDR_EXPERIMENTAL(&(mcastPacket->srcAddr))) ||
        inetIsInMulticast(&(mcastPacket->srcAddr)) == L7_TRUE)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "Invalid src IP address");
      return L7_FAILURE;
    }

    mcastPacket->msgType = ip_header.iph_prot;

    if (mcastPacket->msgType == IP_PROT_IGMP)
    {
      /* Validate IP total length value */
      if ((ip_header.iph_len - ipHdrLen) < SNOOP_IGMPv1v2_HEADER_LENGTH)
      {
        snoopDebugPacketRxTrace(mcastPacket, SNOOP_PKT_DROP_BAD_IPHDR_LEN);
        return L7_FAILURE;
      }

      /* Verify IGMP checksum */
      if (snoopCheckSum((L7_ushort16 *)buffPtr, ip_header.iph_len - ipHdrLen,
                        0) != 0)
      {
        snoopDebugPacketRxTrace(mcastPacket, SNOOP_PKT_DROP_BAD_IGMP_CKSUM);
        return L7_FAILURE;
      }
    }
    mcastPacket->ip_payload_length = ip_header.iph_len - ipHdrLen;

    /* Validate IP header checksum */
    if (snoopCheckSum((L7_ushort16 *)startPtr, ipHdrLen, 0) != 0)
    {
      snoopDebugPacketRxTrace(mcastPacket, SNOOP_PKT_DROP_BAD_IP_CKSUM);
      return L7_FAILURE;
    }

    /* Verify that this is an IP multicast frame */
    /*    snoopValidatePacket(); */
    if ((ip_header.iph_dst & L7_CLASS_D_ADDR_NETWORK) !=
        L7_CLASS_D_ADDR_NETWORK)
    {
      snoopDebugPacketRxTrace(mcastPacket, SNOOP_PKT_DROP_BAD_IPV4_DEST);
      return L7_FAILURE;
    }

    /* Should not receive a frame destined to <= 224.0.0.0 */
    if (ip_header.iph_dst <= L7_IP_MCAST_BASE_ADDR)
    {
      snoopDebugPacketRxTrace(mcastPacket, SNOOP_PKT_DROP_BAD_IPV4_DEST);
      return L7_FAILURE;
    }
  }
  else /* MLD */
  {
    L7_uchar8 tempArr[L7_MAC_ADDR_LEN - 2] = { 0, 0, 0, 0};

    /* Check for invalid IPv6 MCAST address */
    if (memcmp(mcastPacket->destMac+2, tempArr, sizeof(tempArr)) == 0)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
      return L7_FAILURE;
    }

    if (mcastPacket->length < L7_ENET_HDR_SIZE +
        L7_ENET_HDR_TYPE_LEN_SIZE +
        L7_IP6_HEADER_LEN + SNOOP_MLDV1_HEADER_LENGTH)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET6, "Received pkt is too small %d",\
                  mcastPacket->length);
      return L7_FAILURE;
    }

    SNOOP_GET_LONG(ip6_header.ver_class_flow, buffPtr);
    SNOOP_GET_SHORT(ip6_header.paylen, buffPtr);
    SNOOP_GET_BYTE(ip6_header.next, buffPtr);
    SNOOP_GET_BYTE(ip6_header.hoplim, buffPtr);

    /* Not received with an IPv6 hop limit as 1, discard.
       RFC3810 section 5  */
    if (ip6_header.hoplim != SNOOP_TTL_VALID_VALUE)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET6, "Received pkt with hop limit other than one %d",\
                  ip6_header.hoplim);
      return L7_FAILURE;
    }

    SNOOP_GET_ADDR6(&ipv6Addr, buffPtr);
    if (!L7_IP6_IS_ADDR_LINK_LOCAL(&ipv6Addr))
    {
      /* Not received with an IPv6 link local address.
         Discard RFC3810 5.1.14, 5.2.13 */
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET6,
                  "Not received with an IPv6 link local address");
      return L7_FAILURE;
    }

    inetAddressSet(L7_AF_INET6, &ipv6Addr, &(mcastPacket->srcAddr));
    SNOOP_GET_ADDR6(&ipv6Addr, buffPtr);
    inetAddressSet(L7_AF_INET6, &ipv6Addr, &(mcastPacket->destAddr));

    xtenHdrLen = 0;
    mcastPacket->routerAlert = L7_FALSE;
    if (ip6_header.next == SNOOP_IP6_IPPROTO_HOPOPTS)
    {
      SNOOP_GET_BYTE(ip6_header.next, buffPtr); /* Next Header */
      SNOOP_GET_BYTE(byteVal, buffPtr); /* Xtension hdr length */
      xtenHdrLen = SNOOP_IP6_HOPBHOP_LEN_GET(byteVal);
      buffPtr += xtenHdrLen - 2;
      mcastPacket->routerAlert = L7_TRUE;
    }

    if (ip6_header.next == IP_PROT_ICMPV6)
    {
      mcastPacket->ip_payload = buffPtr;
      SNOOP_GET_BYTE(mcastPacket->msgType, buffPtr);
      SNOOP_UNUSED_PARAM(buffPtr);
      /* Verify ICMPv6 checksum */
      inetAddressGet(L7_AF_INET6, &(mcastPacket->srcAddr), &(ipv6pkt.srcAddr));
      inetAddressGet(L7_AF_INET6, &(mcastPacket->destAddr), &(ipv6pkt.dstAddr));
      lenIcmpData = ip6_header.paylen - xtenHdrLen;
      /*datalen should be in big endian for snoopcheckSum to succeed*/
      if (snoopGetEndianess() == SNOOP_LITTLE_ENDIAN)
      {
        ipv6pkt.dataLen = osapiNtohl(lenIcmpData);
      }
      else
      {
        ipv6pkt.dataLen = lenIcmpData;
      }

      ipv6pkt.zero[0] = 0;
      ipv6pkt.zero[1] = 0;
      ipv6pkt.zero[2] = 0;
      ipv6pkt.nxtHdr  = IP_PROT_ICMPV6;

      memcpy(&(ipv6pkt.icmpv6pkt), mcastPacket->ip_payload, lenIcmpData);
      if (snoopCheckSum ((L7_ushort16 *)&ipv6pkt,
                         (L7_IP6_HEADER_LEN + lenIcmpData), 0)!=0)
      {
        snoopDebugPacketRxTrace(mcastPacket, SNOOP_PKT_DROP_BAD_ICMP_CKSUM);
        return L7_FAILURE;
      }

      mcastPacket->ip_payload_length = ip6_header.paylen - xtenHdrLen;
    }
    else
    {
      /* Not an ICMPv6 packet */
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET6, "Invalid next header %d",\
                  ip6_header.next);
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Process incoming snoop control packets
*
* @param   msg       @b{(input)}  Pointer to received snoop control
*                                 message
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t snoopPacketProcess(snoopPDU_Msg_t *msg)
{
  static mgmdSnoopControlPkt_t mcastPacket;  /* A place holder for received
                                                packet */
  L7_NIM_QUERY_DATA_t          nimQueryData;
  L7_uchar8                   *buffPtr;
  L7_uchar8                    igmpType;
  L7_uchar8                    msgType=0;
  L7_uint32                    ipv4Addr;
  L7_RC_t                      rc=L7_NO_VALUE;
  snoopOperData_t             *pSnoopOperEntry;
  L7_inet_addr_t               addr;
  ptin_IgmpProxyCfg_t          igmpCfg;
  //L7_BOOL                      checkForTOS = L7_FALSE;

  L7_int16  snoopStatIgmpField=-1;

#if 0
  if (msg->client_idx == (L7_uint32) -1)
  {
    /* Apenas se a interface e LEAF */
    if (ptin_igmp_clientIntfVlan_validate(msg->intIfNum,msg->vlanId)==L7_SUCCESS)
    {
#if ( PTIN_BOARD_IS_MATRIX )
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, msg->cbHandle->family, "snoopPacketHandle: Going to add dynamically a client");

      /* If the client does not exist, it will be created in dynamic mode */
      if (ptin_igmp_dynamic_client_add(msg->intIfNum,
                                       msg->vlanId, msg->innerVlanId,
                                       &msg->snoopBuffer[L7_MAC_ADDR_LEN],
                                       &msg->client_idx) != L7_SUCCESS)
      {
        msg->client_idx = (L7_uint) -1;
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, msg->cbHandle->family, "snoopPacketHandle: intIfNum=%u,vlan=%u are not accepted",msg->intIfNum,msg->vlanId);
      }
#else
      if (ptin_debug_igmp_snooping)
        LOG_ERR(LOG_CTX_PTIN_IGMP, "No dynamic clients allowed for leafs of linecards");
      return L7_FAILURE;
#endif
    }
  }
#endif

  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get__snooping_old(&igmpCfg) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations");
    return L7_FAILURE;
  }

  memset(&mcastPacket, 0x00, sizeof(mgmdSnoopControlPkt_t));
  mcastPacket.cbHandle = msg->cbHandle;
  mcastPacket.intIfNum = msg->intIfNum;
  mcastPacket.vlanId   = msg->vlanId;
  mcastPacket.innerVlanId = msg->innerVlanId;
  mcastPacket.client_idx = msg->client_idx;       /* PTin added: IGMP snooping */

  /*Check to see whether the interface is still attached or whether it has been
    detached/deleted by the time this packet is up for processing (now)*/
  memset((void *)&nimQueryData,0,sizeof(nimQueryData));
  nimQueryData.request  = L7_NIM_QRY_RQST_STATE;
  nimQueryData.intIfNum = msg->intIfNum;

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Packet copied... OuterVlan %d InnerVlan:%d intIfNum %d",
            mcastPacket.vlanId, mcastPacket.innerVlanId, mcastPacket.intIfNum);

  if (nimIntfQuery(&nimQueryData) != L7_SUCCESS ||
      (nimQueryData.data.state != L7_INTF_ATTACHED &&
       nimQueryData.data.state != L7_INTF_ATTACHING))
  {
    /* Free the buffer */
    bufferPoolFree(msg->snoopBufferPoolId, msg->snoopBuffer);
    snoopDebugPacketRxTrace(&mcastPacket, SNOOP_PKT_DROP_NOT_READY);
    if (mcastPacket.ip_payload!=L7_NULLPTR)
      ptin_igmp_stat_increment_field(mcastPacket.intIfNum, mcastPacket.vlanId, mcastPacket.client_idx, snoopPacketType2IGMPStatField(mcastPacket.ip_payload[0],SNOOP_STAT_FIELD_DROPPED_RX));
    else
    {
      ptin_igmp_stat_increment_field(mcastPacket.intIfNum, mcastPacket.vlanId, mcastPacket.client_idx, SNOOP_STAT_FIELD_IGMP_DROPPED);
    }

    


    //ptin_igmp_dynamic_client_flush(mcastPacket.vlanId, mcastPacket.client_idx);
    return L7_SUCCESS;
  }

  /* Validate incoming VLAN ID */
  if ((pSnoopOperEntry = snoopOperEntryGet(msg->vlanId, msg->cbHandle, L7_MATCH_EXACT))
      == L7_NULLPTR)
  {
    /* Free the buffer */
    bufferPoolFree(msg->snoopBufferPoolId, msg->snoopBuffer);
    snoopDebugPacketRxTrace(&mcastPacket, SNOOP_PKT_DROP_BAD_VLAN);
    if (mcastPacket.ip_payload!=L7_NULLPTR)
    {
      ptin_igmp_stat_increment_field(mcastPacket.intIfNum, mcastPacket.vlanId, mcastPacket.client_idx, snoopPacketType2IGMPStatField(mcastPacket.ip_payload[0],SNOOP_STAT_FIELD_DROPPED_RX));
    }
    else
    {
      ptin_igmp_stat_increment_field(mcastPacket.intIfNum, mcastPacket.vlanId, mcastPacket.client_idx, SNOOP_STAT_FIELD_IGMP_DROPPED);
    }

    
    //ptin_igmp_dynamic_client_flush(mcastPacket.vlanId, mcastPacket.client_idx);
    return L7_SUCCESS;
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Packet is valid");

  /*------------------------------------*/
  /*   Process the buffer               */
  /*------------------------------------*/
  if (snoopPacketParse(msg, &mcastPacket) != L7_SUCCESS)
  {
    /* Free the buffer */
    bufferPoolFree(msg->snoopBufferPoolId, msg->snoopBuffer);
    if (mcastPacket.ip_payload!=L7_NULLPTR)
    {
      ptin_igmp_stat_increment_field(mcastPacket.intIfNum, mcastPacket.vlanId, mcastPacket.client_idx, snoopPacketType2IGMPStatField(mcastPacket.ip_payload[0],SNOOP_STAT_FIELD_DROPPED_RX));
    }
    else
    {
      ptin_igmp_stat_increment_field(mcastPacket.intIfNum, mcastPacket.vlanId, mcastPacket.client_idx, SNOOP_STAT_FIELD_IGMP_DROPPED);
    }
    //ptin_igmp_dynamic_client_flush(mcastPacket.vlanId, mcastPacket.client_idx);
    return L7_SUCCESS;
  }
  /* Free the buffer */
  bufferPoolFree(msg->snoopBufferPoolId, msg->snoopBuffer);
  snoopDebugPacketRxTrace(&mcastPacket, 0);
  /* IPv4 Multicast Control Packet */
  if (mcastPacket.cbHandle->family == L7_AF_INET)
  {
    if (mcastPacket.msgType == IP_PROT_IGMP)
    {
      /* Point to the start of IP Payload location in the payload */
      buffPtr = mcastPacket.ip_payload;
      SNOOP_GET_BYTE(igmpType, buffPtr); /* Get the IGMP Type */

      SNOOP_UNUSED_PARAM(buffPtr);  

      /* Validate total length value */
      if ((mcastPacket.ip_payload[0]==L7_IGMP_V3_MEMBERSHIP_REPORT && mcastPacket.length<IGMP_V3_PKT_MIN_LENGTH) || 
        ( (mcastPacket.ip_payload[0]==L7_IGMP_MEMBERSHIP_QUERY  || mcastPacket.ip_payload[0]==L7_IGMP_V2_MEMBERSHIP_REPORT || mcastPacket.ip_payload[0]==L7_IGMP_V1_MEMBERSHIP_REPORT) && (mcastPacket.length < IGMP_PKT_MIN_LENGTH)) )
        {
          ptin_igmp_stat_increment_field(mcastPacket.intIfNum, mcastPacket.vlanId, mcastPacket.client_idx, snoopPacketType2IGMPStatField(mcastPacket.ip_payload[0],SNOOP_STAT_FIELD_INVALID_RX));
          LOG_DEBUG(LOG_CTX_PTIN_IGMP,"IGMP Packet Length is invalid =%d",mcastPacket.length);
          return L7_FAILURE;;      
        }

      /* "Reference: RFC3376 - Section 4. Do not accept IGMPv3 Report or Query
       *  messages without the IP TOS set to 0xC0."
       *  However this check is performed only if IGMP is configured to.
       */
#if 0
      if (checkForTOS == L7_TRUE)
      {
        if (((mcastPacket.tosByte) & (SNOOP_IP_TOS)) != SNOOP_IP_TOS)
        {
          LOG_DEBUG (LOG_CTX_PTIN_IGMP, "IGMPv3 Message received with Invalid ToS - 0x%x, "
                     "Discarding it.\n", message->ipTypeOfService);
          return;
        }
      }
#endif
      if (ptin_debug_igmp_packet_trace)
      {    
        L7_uint32 i;
        printf("Rx:PayloadLength:%d\n",mcastPacket.length);
        for (i=0;i<mcastPacket.length;i++)
          printf("%02x ",mcastPacket.payLoad[i]);
        printf("\n");
      }

      if (mcastPacket.ip_payload[0]==L7_IGMP_MEMBERSHIP_QUERY)
      {
        /*Group Address*/
        if (mcastPacket.ip_payload[4]!=0 || mcastPacket.ip_payload[5]!=0 || mcastPacket.ip_payload[6]!=0 || mcastPacket.ip_payload[7]!=0)
        {
          /*Number of Sources*/
          if (mcastPacket.ip_payload[11]!=0 || mcastPacket.ip_payload[10]!=0)
            msgType=L7_IGMP_MEMBERSHIP_GROUP_AND_SOURCE_SCPECIFC_QUERY;
          else
            msgType=L7_IGMP_MEMBERSHIP_GROUP_SPECIFIC_QUERY;        
        }
        else
           msgType=mcastPacket.ip_payload[0];
      }
      else
      {
        msgType=mcastPacket.ip_payload[0];
      }

      switch (igmpType)
      {
      case L7_IGMP_MEMBERSHIP_QUERY:
        if (igmpCfg.networkVersion==2)
        {
          LOG_DEBUG(LOG_CTX_PTIN_IGMP,"MEMBERSHIP_QUERY: intIfNum=%u, oVlan=%u, iVlan=%u, DMAC=%02x:%02x:%02x:%02x:%02x:%02x SMAC=%02x:%02x:%02x:%02x:%02x:%02x",
                    mcastPacket.intIfNum, mcastPacket.vlanId, mcastPacket.innerVlanId,
                    mcastPacket.payLoad[0],mcastPacket.payLoad[1],mcastPacket.payLoad[2],mcastPacket.payLoad[3],mcastPacket.destMac[4],mcastPacket.payLoad[5],
                    mcastPacket.payLoad[6],mcastPacket.payLoad[7],mcastPacket.payLoad[8],mcastPacket.payLoad[9],mcastPacket.payLoad[10],mcastPacket.payLoad[11]);              
          rc = snoopMgmdMembershipQueryProcess(&mcastPacket);
        }
        else if (igmpCfg.networkVersion==3)
        {
          LOG_DEBUG(LOG_CTX_PTIN_IGMP,"MEMBERSHIP_QUERYv3: intIfNum=%u, oVlan=%u, iVlan=%u, DMAC=%02x:%02x:%02x:%02x:%02x:%02x SMAC=%02x:%02x:%02x:%02x:%02x:%02x",
                    mcastPacket.intIfNum, mcastPacket.vlanId, mcastPacket.innerVlanId,
                    mcastPacket.payLoad[0],mcastPacket.payLoad[1],mcastPacket.payLoad[2],mcastPacket.payLoad[3],mcastPacket.destMac[4],mcastPacket.payLoad[5],
                    mcastPacket.payLoad[6],mcastPacket.payLoad[7],mcastPacket.payLoad[8],mcastPacket.payLoad[9],mcastPacket.payLoad[10],mcastPacket.payLoad[11]);
          ptin_timer_start(34,"snoopMgmdSrcSpecificMembershipQueryProcess");
          rc = snoopMgmdSrcSpecificMembershipQueryProcess(&mcastPacket);
          ptin_timer_stop(34);
        }
        break;      
      case L7_IGMP_V3_MEMBERSHIP_REPORT:          
        if (igmpCfg.networkVersion==3)
        {
          LOG_DEBUG(LOG_CTX_PTIN_IGMP,"MEMBERSHIP_REPORTv3: intIfNum=%u, oVlan=%u, iVlan=%u, DMAC=%02x:%02x:%02x:%02x:%02x:%02x SMAC=%02x:%02x:%02x:%02x:%02x:%02x",
                    mcastPacket.intIfNum,mcastPacket.vlanId,mcastPacket.innerVlanId,
                    mcastPacket.payLoad[0],mcastPacket.payLoad[1],mcastPacket.payLoad[2],mcastPacket.payLoad[3],mcastPacket.payLoad[4],mcastPacket.payLoad[5],
                    mcastPacket.payLoad[6],mcastPacket.payLoad[7],mcastPacket.payLoad[8],mcastPacket.payLoad[9],mcastPacket.payLoad[10],mcastPacket.payLoad[11]);
          ptin_timer_start(35,"snoopMgmdSrcSpecificMembershipReportProcess");
          rc = snoopMgmdSrcSpecificMembershipReportProcess(&mcastPacket);
          ptin_timer_stop(35);                
          break;
        }
        else if (igmpCfg.networkVersion==2)
          LOG_WARNING(LOG_CTX_PTIN_IGMP,"MEMBERSHIP_REPORTv3 Rec'd: Operating on v2 Only!");

      case L7_IGMP_V1_MEMBERSHIP_REPORT:
      case L7_IGMP_V2_MEMBERSHIP_REPORT:
        LOG_DEBUG(LOG_CTX_PTIN_IGMP,"MEMBERSHIP_REPORT: intIfNum=%u, oVlan=%u, iVlan=%u, DMAC=%02x:%02x:%02x:%02x:%02x:%02x SMAC=%02x:%02x:%02x:%02x:%02x:%02x",
                  mcastPacket.intIfNum,mcastPacket.vlanId,mcastPacket.innerVlanId,
                  mcastPacket.payLoad[0],mcastPacket.payLoad[1],mcastPacket.payLoad[2],mcastPacket.payLoad[3],mcastPacket.payLoad[4],mcastPacket.payLoad[5],
                  mcastPacket.payLoad[6],mcastPacket.payLoad[7],mcastPacket.payLoad[8],mcastPacket.payLoad[9],mcastPacket.payLoad[10],mcastPacket.payLoad[11]);
        ptin_timer_start(0,"snoopPacketProcess-snoopMgmdMembershipReportProcess");
        rc = snoopMgmdMembershipReportProcess(&mcastPacket);
        ptin_timer_stop(0);
        break;
      case L7_IGMP_V2_LEAVE_GROUP:
        LOG_DEBUG(LOG_CTX_PTIN_IGMP,"LEAVE_GROUP: intIfNum=%u, oVlan=%u, iVlan=%u, DMAC=%02x:%02x:%02x:%02x:%02x:%02x SMAC=%02x:%02x:%02x:%02x:%02x:%02x",
                  mcastPacket.intIfNum,mcastPacket.vlanId,mcastPacket.innerVlanId,
                  mcastPacket.payLoad[0],mcastPacket.payLoad[1],mcastPacket.payLoad[2],mcastPacket.payLoad[3],mcastPacket.payLoad[4],mcastPacket.payLoad[5],
                  mcastPacket.payLoad[6],mcastPacket.payLoad[7],mcastPacket.payLoad[8],mcastPacket.payLoad[9],mcastPacket.payLoad[10],mcastPacket.payLoad[11]);
        rc = snoopMgmdLeaveGroupProcess(&mcastPacket);
        break;

      case L7_IGMP_DVMRP:
        ipv4Addr = L7_IP_ALL_DVMRP_RTRS_ADDR;
        inetAddressSet(L7_AF_INET, &ipv4Addr , &addr);
        rc = snoopPimv1v2DvmrpProcess(&mcastPacket, &addr, IP_PROT_IGMP);
        break;
      case L7_IGMP_PIM_V1:
        ipv4Addr = L7_IP_ALL_PIM_RTRS_ADDR;
        inetAddressSet(L7_AF_INET, &ipv4Addr , &addr);
        rc = snoopPimv1v2DvmrpProcess(&mcastPacket, &addr, IP_PROT_IGMP);
        break;

      default:
        /* Per the IGMP Snooping draft, an IGMP Snooping switch MUST forward all unrecognized
           IGMP messages... so, flood on all ports in the VLAN except the incoming intf */
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "Flooding packet...");
        rc = snoopPacketFlood(&mcastPacket);
        break;
      }              
    }
    else
    {
      inetAddressGet(L7_AF_INET, &mcastPacket.destAddr, &ipv4Addr);
      /* Non-IGMP/multicast data frame */
      /* Check to see if the dest IP addr falls in the 224.0.0.X range. */
      if ( (ipv4Addr > L7_IP_MCAST_BASE_ADDR) &&
           (ipv4Addr < L7_IP_MAX_LOCAL_MULTICAST) )
      {
        /* If this is a PIMv2 packet, it is handled the same as an IGMP Query barring the
           query packet parsing.
         * This will flood the packet on all ports */
        if ((mcastPacket.msgType == IP_PROT_PIM) &&
            ipv4Addr == L7_IP_ALL_PIM_RTRS_ADDR)
        {
          inetAddressSet(L7_AF_INET, &ipv4Addr , &addr);
          rc = snoopPimv1v2DvmrpProcess(&mcastPacket, &addr, IP_PROT_PIM);
        }
        else
        {
          /* Multicast data frame in the reserved range */
          SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "Flooding packet...");
          rc = snoopPacketFlood(&mcastPacket);
        }
      }
      else
      {
        /* Unknown packet type flood on the VLAN */
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "Flooding packet...");
        rc = snoopPacketFlood(&mcastPacket);
      }
    } /* end of non igmp packet processing */
  } /* End of IPv4 multicast control packet processing */
  else /* IPv6 Multicast Control Packet */
  {
    switch (mcastPacket.msgType)
    {
    case L7_MLD_MEMBERSHIP_QUERY: /* Query */
      rc = snoopMgmdMembershipQueryProcess(&mcastPacket);
      break;

    case L7_MLD_V1_MEMBERSHIP_REPORT: /* Report */
      rc = snoopMgmdMembershipReportProcess(&mcastPacket);
      break;

    case L7_MLD_V2_MEMBERSHIP_REPORT:
      rc = L7_SUCCESS; //snoopMgmdSrcSpecificMembershipReportProcess(&mcastPacket);
      break;

    case L7_MLD_V1_MEMBERSHIP_DONE: /* Done */
      rc = snoopMgmdLeaveGroupProcess(&mcastPacket);
      break;

    default:
      /* Snooping switch MUST forward all unrecognized
        messages... so, flood on all ports in the VLAN except
        the incoming intf */
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "Flooding packet...");
      rc = snoopPacketFlood(&mcastPacket);
      break;
    } /* end of switch case */
  }/* end of IPv6 Multicast Control Packet processing */

  /* PTin added: IGMP snooping */
#if 1
  if (rc==L7_SUCCESS)
    debug_pktTimer.pktOk_counter++;
  else
    debug_pktTimer.pktEr_counter++;

  debug_pktTimer.time_end = osapiTimeMillisecondsGet();
#endif


  /*If we do update each type of message we do not need to also update the global conters: GlobalIGMPCounter=Sum IGMPv3+IGMPv2+Query*/
#if 0
  if (rc==L7_SUCCESS)
    ptin_igmp_stat_increment_field(mcastPacket.intIfNum, mcastPacket.vlanId, mcastPacket.client_idx, SNOOP_STAT_FIELD_IGMP_RECEIVED_VALID);
  else if (rc==L7_FAILURE) /*Invalid Packet Type*/
    ptin_igmp_stat_increment_field(mcastPacket.intIfNum, mcastPacket.vlanId, mcastPacket.client_idx, SNOOP_STAT_FIELD_IGMP_RECEIVED_INVALID);
  else /*if (rc==L7_ERROR)*/
    ptin_igmp_stat_increment_field(mcastPacket.intIfNum, mcastPacket.vlanId, mcastPacket.client_idx, SNOOP_STAT_FIELD_IGMP_DROPPED);
#else
  if (rc==L7_SUCCESS)
    snoopStatIgmpField=SNOOP_STAT_FIELD_VALID_RX;
  else if (rc==L7_FAILURE) /*Invalid Packet Type*/
    snoopStatIgmpField=SNOOP_STAT_FIELD_INVALID_RX;
  else /*if (rc==L7_ERROR)*/
    snoopStatIgmpField=SNOOP_STAT_FIELD_DROPPED_RX;               

    ptin_igmp_stat_increment_field(mcastPacket.intIfNum, mcastPacket.vlanId, mcastPacket.client_idx, snoopPacketType2IGMPStatField(/*mcastPacket.msgType*/msgType,snoopStatIgmpField));
#endif


  /* If client is dynamic, and no channels are associated, flush it */
  //ptin_igmp_dynamic_client_flush(mcastPacket.vlanId, mcastPacket.client_idx);

  return rc;
}

/* PTin removed: IGMP snooping */
#if 0
/*********************************************************************
* @purpose Process IGMP/MLD Membership query message
*
* @param   mcastPacket  @b{(input)} Pointer to data structure to hold
*                                   received packet
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t snoopMgmdMembershipQueryProcess(mgmdSnoopControlPkt_t *mcastPacket)
{
  snoop_cb_t     *pSnoopCB = L7_NULLPTR;
  L7_RC_t         rc = L7_SUCCESS;
  L7_uchar8      *dataPtr = L7_NULL;
  L7_uint32       ipv4Addr;
  L7_in6_addr_t   ipv6Addr;
  L7_mgmdMsg_t    mgmdMsg;
  L7_uchar8       byteVal;
  L7_uchar8       dmac[L7_MAC_ADDR_LEN];
  L7_BOOL         generalQry = L7_FALSE;

  /* Get Snoop Control Block */
  pSnoopCB = mcastPacket->cbHandle;
  /* If the src IP is non-zero, add this interface to the multicast router list
     and to all snooping entries */

  /* Set pointer to IGMP message */
  dataPtr = mcastPacket->ip_payload;

  if (mcastPacket->cbHandle->family == L7_AF_INET) /* IGMP Message */
  {
    memset(&mgmdMsg, 0x00, sizeof(L7_mgmdMsg_t));
    SNOOP_GET_BYTE(mgmdMsg.mgmdType, dataPtr);         /* Version/Type */
    SNOOP_GET_BYTE(mgmdMsg.igmpMaxRespTime, dataPtr);  /* Max response time */
    SNOOP_GET_SHORT(mgmdMsg.mgmdChecksum, dataPtr);    /* Checksum */
    SNOOP_GET_ADDR(&ipv4Addr, dataPtr);    /* Group Address */
    SNOOP_UNUSED_PARAM(dataPtr);
    inetAddressSet(L7_AF_INET, &ipv4Addr, &mgmdMsg.mgmdGroupAddr);
  }
  else /* MLD Message */
  {
    memset(&mgmdMsg, 0x00, sizeof(L7_mgmdMsg_t));
    SNOOP_GET_BYTE(mgmdMsg.mgmdType, dataPtr);         /* Version/Type */
    SNOOP_GET_BYTE(byteVal, dataPtr);  /* Max response time */
    SNOOP_UNUSED_PARAM(byteVal);
    SNOOP_GET_SHORT(mgmdMsg.mgmdChecksum, dataPtr);    /* Checksum */
    SNOOP_GET_SHORT(mgmdMsg.mldMaxRespTime, dataPtr);  /* Max response time */
    SNOOP_GET_SHORT(mgmdMsg.mgmdReserved, dataPtr);               /* rserved */

    SNOOP_GET_ADDR6(&ipv6Addr, dataPtr); /* Group Address */
    inetAddressSet(L7_AF_INET6, &ipv6Addr, &mgmdMsg.mgmdGroupAddr);
  }

  if (mcastPacket->cbHandle->family == L7_AF_INET6)
  {
    /* Check if it is general query address or group specific */
    if (inetIsAddressZero(&mgmdMsg.mgmdGroupAddr) == L7_TRUE)
    {
      L7_uchar8 mldQryAddr[L7_IP6_ADDR_LEN];
      L7_uchar8 ipBuf[L7_IP6_ADDR_LEN];

      memset(mldQryAddr, 0x00, L7_IP6_ADDR_LEN);
      osapiInetPton(L7_AF_INET6, SNOOP_IP6_ALL_HOSTS_ADDR, mldQryAddr);

      /* Check if it is equal to the all hosts address FF02::1 */
      inetAddressGet(L7_AF_INET6, &mcastPacket->destAddr, ipBuf);
      if (memcmp(ipBuf, mldQryAddr, L7_IP6_ADDR_LEN) != 0)
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
        return L7_FAILURE;
      }
      generalQry = L7_TRUE;
    }
    else /* Should be group specific query */
    {
      if (inetIsInMulticast(&mgmdMsg.mgmdGroupAddr) == L7_TRUE)
      {
        if (L7_INET_ADDR_COMPARE(&mcastPacket->destAddr, &mgmdMsg.mgmdGroupAddr) != 0)
        {
          SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
          return L7_FAILURE;
        }
      }
      else
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
        return L7_FAILURE;
      }
    }
  }
  else
  {
    /* Check if it is general query address or group specific */
    if (inetIsAddressZero(&mgmdMsg.mgmdGroupAddr) == L7_TRUE)
    {
      /* Check if IPv4 destination address is same as 224.0.0.1 */
      inetAddressGet(mcastPacket->cbHandle->family, &mcastPacket->destAddr,
                     &ipv4Addr);
      if (ipv4Addr != L7_IP_ALL_HOSTS_ADDR)
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
        return L7_FAILURE;
      }
    }
    else /* Should be group specific query */
    {
      if (inetIsInMulticast(&mgmdMsg.mgmdGroupAddr) == L7_TRUE)
      {
        if (L7_INET_ADDR_COMPARE(&mcastPacket->destAddr, &mgmdMsg.mgmdGroupAddr) != 0)
        {
          SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
          return L7_FAILURE;
        }
      }
      else
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
        return L7_FAILURE;
      }
    }
  }

  memset(dmac, 0x00, sizeof(dmac));
  /* Validate the group address being reported */
  snoopMulticastMacFromIpAddr(&mcastPacket->destAddr, dmac);
  if (snoopMacAddrCheck(dmac, mcastPacket->cbHandle->family) != L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
    return L7_FAILURE;
  }
  /* Match the destination mac and the mac resloved from ipv4 dest addr */
  if (memcmp(mcastPacket->destMac, dmac, L7_MAC_ADDR_LEN) != 0)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
    return L7_FAILURE;
  }

  if (inetIsAddressZero(&mcastPacket->srcAddr) != L7_TRUE)
  {
    /* If ipv6 options header is not supported in hardware.
       In such case we only process MLDv2 General queries with hop options */
    if (mcastPacket->cbHandle->family == L7_AF_INET6 &&
        mcastPacket->cbHandle->snoopExec->ipv6OptionsSupport == L7_FALSE &&
        mcastPacket->routerAlert == L7_TRUE)
    {
      if (mcastPacket->ip_payload_length > SNOOP_MLDV1_HEADER_LENGTH &&
          generalQry == L7_TRUE)
      {
        if ((rc = snoopQuerierQueryProcess(mcastPacket)) == L7_SUCCESS)
        {
          /* PTin removed: IGMP snooping */
#if 0
          snoopMcastRtrListAdd(mcastPacket->vlanId, mcastPacket->intIfNum, 
                               SNOOP_DYNAMIC, mcastPacket->cbHandle);
#endif
        }
      }
      else if (generalQry == L7_FALSE)
      {
        /* Do not flood group specific queries as they are switched in 
           hardware and are not copied to CPU. If we flood then two copies
           are created for the packet. */
        rc = L7_FAILURE;
      }
    }
    else
    {
      if ((rc = snoopQuerierQueryProcess(mcastPacket)) == L7_SUCCESS)
      {
        /* PTin removed: IGMP snooping */
#if 0
        snoopMcastRtrListAdd(mcastPacket->vlanId, mcastPacket->intIfNum, 
                             SNOOP_DYNAMIC, mcastPacket->cbHandle);
#endif
      }
    }
  }
  /* Forward membership query on all interfaces on this VLAN, except the
     incoming interface */
  if (rc == L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "Flooding packet...");
    snoopPacketFlood(mcastPacket);
  }

  pSnoopCB->counters.controlFramesProcessed++;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Process IGMPv1/IGMPv2/MLDv1/MLDv2 Group Membership Reports
*
* @param   mcastPacket  @b{(input)} Pointer to data structure to hold
*                                   received packet
*
* @returns L7_SUCCESS
* @returns L7_FAILURE  Invalid packet
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t snoopMgmdMembershipReportProcess(mgmdSnoopControlPkt_t *mcastPacket)
{
  L7_uchar8       *dataPtr;
  L7_BOOL          fwdFlag = L7_TRUE;
  L7_mgmdMsg_t     mgmdMsg;
  L7_uchar8        dmac[L7_MAC_ADDR_LEN], byteVal;
  L7_RC_t          rc = L7_SUCCESS;
  L7_uint32        ipv4Addr;
  snoop_cb_t      *pSnoopCB = L7_NULLPTR;
  L7_in6_addr_t    ipv6Addr;

  /* Set pointer to IGMP message */
  dataPtr = mcastPacket->ip_payload;

  /* Get Snoop Control Block */
  pSnoopCB = mcastPacket->cbHandle;

  if (mcastPacket->cbHandle->family == L7_AF_INET) /* IGMP Message */
  {
    memset(&mgmdMsg, 0x00, sizeof(L7_mgmdMsg_t));
    SNOOP_GET_BYTE(mgmdMsg.mgmdType, dataPtr);         /* Version/Type */
    SNOOP_GET_BYTE(mgmdMsg.igmpMaxRespTime, dataPtr);  /* Max response time */
    SNOOP_GET_SHORT(mgmdMsg.mgmdChecksum, dataPtr);    /* Checksum */
    SNOOP_GET_ADDR(&ipv4Addr, dataPtr);    /* Group Address */
    SNOOP_UNUSED_PARAM(dataPtr);
    inetAddressSet(L7_AF_INET, &ipv4Addr, &mgmdMsg.mgmdGroupAddr);

    if (mgmdMsg.mgmdType == L7_IGMP_V2_MEMBERSHIP_REPORT &&
        pSnoopCB->snoopCfgData->igmpv3_tos_rtr_alert_check == L7_TRUE)
    {
      if (mcastPacket->routerAlert != L7_TRUE)
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
        return L7_FAILURE;
      }
    }
  }
  else /* MLD Message */
  {
    memset(&mgmdMsg, 0x00, sizeof(L7_mgmdMsg_t));
    SNOOP_GET_BYTE(mgmdMsg.mgmdType, dataPtr);         /* Version/Type */
    SNOOP_GET_BYTE(byteVal, dataPtr);  /* Max response time */
    SNOOP_UNUSED_PARAM(byteVal);
    SNOOP_GET_SHORT(mgmdMsg.mgmdChecksum, dataPtr);    /* Checksum */
    SNOOP_GET_SHORT(mgmdMsg.mldMaxRespTime, dataPtr);  /* Max response time */
    SNOOP_GET_SHORT(mgmdMsg.mgmdReserved, dataPtr);               /* rserved */

    SNOOP_GET_ADDR6(&ipv6Addr, dataPtr); /* Group Address */
    inetAddressSet(L7_AF_INET6, &ipv6Addr, &mgmdMsg.mgmdGroupAddr);
  }

  if (L7_INET_ADDR_COMPARE(&mcastPacket->destAddr, &mgmdMsg.mgmdGroupAddr) != 0)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
    return L7_FAILURE;
  }

  memset(dmac, 0x00, sizeof(dmac));
  /* Validate the group address being reported */
  snoopMulticastMacFromIpAddr(&mgmdMsg.mgmdGroupAddr, dmac);
  if (snoopMacAddrCheck(dmac, mcastPacket->cbHandle->family) != L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
    return L7_FAILURE;
  }

  /* Match the destination mac and the mac resloved from ipv4 dest addr */
  if (memcmp(mcastPacket->destMac, dmac, L7_MAC_ADDR_LEN) != 0)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
    return L7_FAILURE;
  }

  /* Check to see if the group address is in the 224.0.0.X range.  If it is, don't
     process it... just forward to all ports in the vlan except incoming, otherwise
     it to the table */
  fwdFlag = inetIsLinkLocalMulticastAddress(&mgmdMsg.mgmdGroupAddr);

  if (fwdFlag == L7_FALSE)
  {
    /* Does an entry with the same MAC addr and VLAN ID already exist? */
    if (snoopEntryFind(dmac, mcastPacket->vlanId, mcastPacket->cbHandle->family,
                       L7_MATCH_EXACT) == L7_NULLPTR)
    {
      /* Entry does not exist... give back the semaphore and create new entry */
      rc = snoopEntryCreate(dmac, mcastPacket->vlanId, mcastPacket->cbHandle->family);
      if (rc == L7_SUCCESS)
      {
        /* Add all multicast router interfaces to this entry */
        snoopAllMcastRtrIntfsAdd(dmac, mcastPacket->vlanId,
                                 mcastPacket->intIfNum, mcastPacket->cbHandle);
      }
      else
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopEntryCreate failed. Table full\n");
      }
    }

    /* If the entry already existed or it was just successfully created, add the interface */
    if (rc == L7_SUCCESS)
    {
      rc = snoopIntfAdd(dmac, mcastPacket->vlanId, mcastPacket->intIfNum,
                        SNOOP_GROUP_MEMBERSHIP, mcastPacket->cbHandle);
      if (rc != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
                "snoopMgmdMembershipReportProcess: snoopIntfAdd failed");
      }
    }
    /* Forward membership report to multicast routers we've detected */
    snoopPacketRtrIntfsForward(mcastPacket);
  }
  else
  {
    /* Flood it in the VLAN as it is for link local multicast address */
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "Flooding packet...");
    snoopPacketFlood(mcastPacket);
  }


  pSnoopCB->counters.controlFramesProcessed++;
  return rc;
}

/****************************************************************************
* @purpose Process IGMPv3/MLDv2 Group Membership Report
*
* @param   mcastPacket  @b{(input)} Pointer to data structure to hold
*                                   received packet
*
* @returns L7_SUCCESS
* @returns L7_FAILURE, if invalid group address
*
* @notes none
*
* @end
*
*****************************************************************************/
L7_RC_t snoopMgmdSrcSpecificMembershipReportProcess(mgmdSnoopControlPkt_t
                                                    *mcastPacket)
{
  L7_uchar8               dmac[L7_MAC_ADDR_LEN], *dataPtr, recType, auxDataLen;
  L7_RC_t                 rc = L7_SUCCESS;
  snoopInfoData_t        *snoopEntry;
  L7_uint32               ipv4Addr, timerValue, intfFastLeaveMode;
  L7_BOOL   fastLeaveMode;
  L7_ushort16             noOfGroups, noOfSources;
  L7_inet_addr_t          groupAddr;
  L7_BOOL                 fwdFlag = L7_FALSE, floodFlag = L7_TRUE;
  snoop_cb_t             *pSnoopCB = L7_NULLPTR;
  L7_uchar8               ipBuf[16];

  /* Set pointer to IGMPv3 Membership Report */
  dataPtr = mcastPacket->ip_payload + MGMD_REPORT_NUM_SOURCES_OFFSET;
  SNOOP_GET_SHORT(noOfGroups, dataPtr);
  /* Data ptr will point to the start if group records */

  /* Get Snoop Control Block */
  pSnoopCB = mcastPacket->cbHandle;

  if (mcastPacket->cbHandle->family == L7_AF_INET)
  {
    /* IGMP */
    inetAddressGet(L7_AF_INET, &mcastPacket->destAddr, &ipv4Addr);
    if (ipv4Addr != L7_IP_IGMPV3_REPORT_ADDR)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "Invalid destination IPv4 address\n");
      return L7_FAILURE;
    }

    if (pSnoopCB->snoopCfgData->igmpv3_tos_rtr_alert_check == L7_TRUE)
    {
      if (mcastPacket->tosByte != SNOOP_TOS_VALID_VALUE)
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
        return L7_FAILURE;
      }

      if (mcastPacket->routerAlert != L7_TRUE)
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
        return L7_FAILURE;
      }
    }

  }
  else
  {
    L7_uchar8 mldv3ReportAddr[L7_IP6_ADDR_LEN];

    memset(mldv3ReportAddr, 0x00, L7_IP6_ADDR_LEN);
    osapiInetPton(L7_AF_INET6, SNOOP_IP6_MLDV2_REPORT_ADDR, mldv3ReportAddr);

    /* MLD */
    inetAddressGet(L7_AF_INET6, &mcastPacket->destAddr, ipBuf);
    if (memcmp(ipBuf, mldv3ReportAddr, L7_IP6_ADDR_LEN) != 0)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET6, "Invalid destination IPv6 address\n");
      return L7_FAILURE;
    }
  }

  /* Validate the dmac and the destination Ipv4 address */
  memset(dmac, 0x00, sizeof(dmac));
  snoopMulticastMacFromIpAddr(&mcastPacket->destAddr, dmac);
  if (memcmp(mcastPacket->destMac, dmac, L7_MAC_ADDR_LEN) != 0)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
    return L7_FAILURE;
  }

  while (noOfGroups > 0)
  {
    /* IGMP Membership records */
    SNOOP_GET_BYTE(recType, dataPtr); /* Record type */
    SNOOP_GET_BYTE(auxDataLen, dataPtr); /* AuxData Len */
    SNOOP_GET_SHORT(noOfSources, dataPtr); /* Number of sources */
    if (mcastPacket->cbHandle->family == L7_AF_INET)
    {
      SNOOP_GET_ADDR(&ipv4Addr, dataPtr);
      inetAddressSet(L7_AF_INET, &ipv4Addr, &groupAddr);
    }
    else
    {
      /* IPv6 MCAST Address */
      SNOOP_GET_ADDR6(ipBuf, dataPtr);
      inetAddressSet(L7_AF_INET6, ipBuf, &groupAddr);
    }

    if (inetIsInMulticast(&groupAddr) == L7_TRUE)
    {
      memset(dmac, 0x00, sizeof(dmac));
      /* Validate the group address being reported */
      snoopMulticastMacFromIpAddr(&groupAddr, dmac);
      if (snoopMacAddrCheck(dmac, mcastPacket->cbHandle->family) != L7_SUCCESS)
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, pSnoopCB->family, "Invalid destination multicast mac\n");
        return L7_FAILURE;
      }
      fwdFlag = inetIsLinkLocalMulticastAddress(&groupAddr);
    }
    else
    {
      fwdFlag = L7_TRUE;
    }

    if (fwdFlag == L7_FALSE)
    {
      floodFlag = L7_FALSE;
      if ((noOfSources == 0) && (recType == L7_IGMP_CHANGE_TO_INCLUDE_MODE))
      {
        /* Treat TO_IN( {} ) like an IGMPv2 Leave / MLDv1 Done*/

        /* Does an entry with the same MAC addr and VLAN ID already exist? */
        snoopEntry = snoopEntryFind(dmac, mcastPacket->vlanId,
                                    mcastPacket->cbHandle->family, L7_MATCH_EXACT);
        if (snoopEntry)
        {
          fastLeaveMode = (mcastPacket->cbHandle->snoopCfgData->snoopVlanMode[mcastPacket->vlanId] &
                           SNOOP_VLAN_FAST_LEAVE_MODE) ? L7_ENABLE : L7_DISABLE;

          if (snoopIntfFastLeaveAdminModeGet(mcastPacket->intIfNum, &intfFastLeaveMode,
                                             mcastPacket->cbHandle->family) != L7_SUCCESS)
          {
            intfFastLeaveMode = L7_DISABLE;
          }

          /* check whether the interface or the VLAN has fast-leave admin mode enabled */
          if ((intfFastLeaveMode == L7_ENABLE) ||(fastLeaveMode == L7_ENABLE))
          {
            if (L7_INTF_ISMASKBITSET(snoopEntry->snoopGrpMemberList,
                                     mcastPacket->intIfNum))
            {
              if (snoopIntfRemove(dmac, mcastPacket->vlanId,
                                  mcastPacket->intIfNum,SNOOP_GROUP_MEMBERSHIP,
                                  mcastPacket->cbHandle) != L7_SUCCESS)
              {
                L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
                        "snoopMgmdV3MembershipReportProcess: Failed to remove group membership");
              }
            }
          }
          else
          {
            if (snoop_is_timer_running(snoopEntry, mcastPacket->intIfNum,
                                       mcastPacket->vlanId, SNOOP_GROUP_MEMBERSHIP,
                                       mcastPacket->cbHandle) == L7_TRUE)
            {
              if (snoopQuerySend(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->cbHandle,
                                 SNOOP_LEAVE_PROCESS_QUERY, L7_NULLPTR) != L7_SUCCESS)
              {
                L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
                        "snoopMgmdV3MembershipReportProcess: Failed to send group specific query");
              }
            }

            timerValue = snoopCheckPrecedenceParamGet(mcastPacket->vlanId,mcastPacket->intIfNum,
                                                      SNOOP_PARAM_MAX_RESPONSE_TIME,
                                                      snoopEntry->snoopInfoDataKey.family);
            if (L7_INTF_ISMASKBITSET(snoopEntry->snoopGrpMemberList,
                                     mcastPacket->intIfNum))
            {
              snoopTimerUpdate(snoopEntry, mcastPacket->intIfNum,
                               mcastPacket->vlanId,
                               SNOOP_MAX_RESPONSE, timerValue,
                               mcastPacket->cbHandle);
            } /* End of group membership check */
            /* Entry exists... give back semaphore, set timers and transmit query */
          }
        }
      }
      else
      {

        /* Does an entry with the same MAC addr and VLAN ID already exist? */
        if (snoopEntryFind(dmac, mcastPacket->vlanId, mcastPacket->cbHandle->family,
                           L7_MATCH_EXACT) == L7_NULLPTR)
        {
          /* Entry does not exist... give back the semaphore and create new entry */
          rc = snoopEntryCreate(dmac, mcastPacket->vlanId, mcastPacket->cbHandle->family);
          if (rc == L7_SUCCESS)
          {
            /* Add all multicast router interfaces to this entry */
            snoopAllMcastRtrIntfsAdd(dmac, mcastPacket->vlanId,
                                     mcastPacket->intIfNum,
                                     mcastPacket->cbHandle);
          }
          else
          {
            SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopEntryCreate failed. Table full\n");
          }
        }

        /* If the entry already existed or it was just successfully created, add the interface */
        if (rc == L7_SUCCESS)
        {
          rc = snoopIntfAdd(dmac, mcastPacket->vlanId, mcastPacket->intIfNum,
                            SNOOP_GROUP_MEMBERSHIP, mcastPacket->cbHandle);
        }
      }/* End of record type check */
    } /* end of if process packet */

    /* Point to the next record */
    /* RFC 3376 4.2.6, RFC 3810 5.2.6 */
    if (mcastPacket->cbHandle->family == L7_AF_INET)
    {
      dataPtr += ((auxDataLen * 4) + (noOfSources * sizeof(L7_in_addr_t)));
    }
    else
    {
      dataPtr += ((auxDataLen * 4) + (noOfSources * sizeof(L7_in6_addr_t)));
    }

    /* Check if it is a malformed packet */
    if ((dataPtr - mcastPacket->ip_payload) > mcastPacket->ip_payload_length)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"Dropped malformed packet\n");
      return L7_FAILURE;
    }

    noOfGroups -= 1;
  } /* end of while loop */

  /* Forward membership report to multicast routers we've detected */
  if (floodFlag == L7_FALSE)
  {
    snoopPacketRtrIntfsForward(mcastPacket);
  }
  else
  {
    /* Flood it in the VLAN as it is for link local multicast address */
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "Flooding packet...");
    snoopPacketFlood(mcastPacket);
  }

  pSnoopCB->counters.controlFramesProcessed++;
  return rc;
}
/*********************************************************************
* @purpose Process IGMPv2 Leave/ MLDv1 Done Group message
*
* @param   mcastPacket  @b{(input)} Pointer to data structure to hold
*                                   received packet
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t snoopMgmdLeaveGroupProcess(mgmdSnoopControlPkt_t *mcastPacket)
{

  L7_uchar8        dmac[L7_MAC_ADDR_LEN], *dataPtr, byteVal;
  L7_uchar8        ipBuf[L7_IP6_ADDR_LEN];
  snoopInfoData_t *snoopEntry;
  L7_RC_t          rc = L7_SUCCESS;
  L7_uint32        ipv4Addr;
  L7_BOOL  fastLeaveMode;
  L7_uint32        timerValue, intfFastLeaveMode;
  snoop_cb_t      *pSnoopCB = L7_NULLPTR;
  L7_mgmdMsg_t     mgmdMsg;
  L7_in6_addr_t    ipv6Addr;
  L7_BOOL          fwdFlag = L7_TRUE;

  /* Get Snoop Control Block */
  pSnoopCB = mcastPacket->cbHandle;

  /* Set pointer to IGMP message */
  dataPtr = mcastPacket->ip_payload;

  if (mcastPacket->cbHandle->family == L7_AF_INET) /* IGMP Message */
  {
    memset(&mgmdMsg, 0x00, sizeof(L7_mgmdMsg_t));
    SNOOP_GET_BYTE(mgmdMsg.mgmdType, dataPtr);         /* Version/Type */
    SNOOP_GET_BYTE(mgmdMsg.igmpMaxRespTime, dataPtr);  /* Max response time */
    SNOOP_GET_SHORT(mgmdMsg.mgmdChecksum, dataPtr);    /* Checksum */
    SNOOP_GET_ADDR(&ipv4Addr, dataPtr);    /* Group Address */
    SNOOP_UNUSED_PARAM(dataPtr);
    inetAddressSet(L7_AF_INET, &ipv4Addr, &mgmdMsg.mgmdGroupAddr);

    if (pSnoopCB->snoopCfgData->igmpv3_tos_rtr_alert_check == L7_TRUE)
    {
      if (mcastPacket->routerAlert != L7_TRUE)
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
        return L7_FAILURE;
      }
    }
  }
  else /* MLD Message */
  {
    memset(&mgmdMsg, 0x00, sizeof(L7_mgmdMsg_t));
    SNOOP_GET_BYTE(mgmdMsg.mgmdType, dataPtr);         /* Version/Type */
    SNOOP_GET_BYTE(byteVal, dataPtr);                  /* Code */
    SNOOP_UNUSED_PARAM(byteVal);
    SNOOP_GET_SHORT(mgmdMsg.mgmdChecksum, dataPtr);    /* Checksum */
    SNOOP_GET_SHORT(mgmdMsg.mldMaxRespTime, dataPtr);  /* Max response time */
    SNOOP_GET_SHORT(mgmdMsg.mgmdReserved, dataPtr);

    SNOOP_GET_ADDR6(&ipv6Addr, dataPtr); /* Group Address */
    inetAddressSet(L7_AF_INET6, &ipv6Addr, &mgmdMsg.mgmdGroupAddr);
  }

  if (pSnoopCB->family == L7_AF_INET)
  {
    /* Check if IPv4 destination address is same as 224.0.0.2 */
    inetAddressGet(mcastPacket->cbHandle->family, &mcastPacket->destAddr,
                   &ipv4Addr);
    if (ipv4Addr != L7_IP_ALL_RTRS_ADDR)
    {
      if (L7_INET_ADDR_COMPARE(&mcastPacket->destAddr, &mgmdMsg.mgmdGroupAddr) != 0)
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
        return L7_FAILURE;
      }
    }
  }
  else
  {
    L7_uchar8 mldv3ReportAddr[L7_IP6_ADDR_LEN];

    memset(mldv3ReportAddr, 0x00, L7_IP6_ADDR_LEN);
    osapiInetPton(L7_AF_INET6, SNOOP_IP6_ALL_ROUTERS_ADDR, mldv3ReportAddr);

    /* MLD */
    inetAddressGet(L7_AF_INET6, &mcastPacket->destAddr, ipBuf);
    if (memcmp(ipBuf, mldv3ReportAddr, L7_IP6_ADDR_LEN) != 0)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET6, "Invalid destination IPv6 address\n");
      return L7_FAILURE;
    }
  }

  /* check if it is a leave for a valid mcast group address */
  if (inetIsInMulticast(&mgmdMsg.mgmdGroupAddr) == L7_FALSE)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
    return L7_FAILURE;
  }

  /* check if frame dmac is same as IP Dest Addr equivalent dmac */
  memset(dmac, 0x00, sizeof(dmac));
  snoopMulticastMacFromIpAddr(&mcastPacket->destAddr, dmac);
  if (memcmp(mcastPacket->destMac, dmac, L7_MAC_ADDR_LEN) != 0)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
    return L7_FAILURE;
  }

  memset(dmac, 0x00, sizeof(dmac));
  /* Validate the group address being reported */
  snoopMulticastMacFromIpAddr(&mgmdMsg.mgmdGroupAddr, dmac);
  if (snoopMacAddrCheck(dmac, mcastPacket->cbHandle->family) != L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
    return L7_FAILURE;
  }


  fwdFlag = inetIsLinkLocalMulticastAddress(&mgmdMsg.mgmdGroupAddr);

  if (fwdFlag == L7_FALSE)
  {

    /* Does an entry with the same MAC addr and VLAN ID already exist? */
    snoopEntry = snoopEntryFind(dmac, mcastPacket->vlanId,
                                mcastPacket->cbHandle->family, L7_MATCH_EXACT);

    if (snoopEntry)
    {
      fastLeaveMode = (mcastPacket->cbHandle->snoopCfgData->snoopVlanMode[mcastPacket->vlanId] &
                       SNOOP_VLAN_FAST_LEAVE_MODE) ? L7_ENABLE : L7_DISABLE;
      if (snoopIntfFastLeaveAdminModeGet(mcastPacket->intIfNum, &intfFastLeaveMode,
                                         mcastPacket->cbHandle->family) != L7_SUCCESS)
      {
        intfFastLeaveMode = L7_DISABLE;
      }

      /* check whether the interface or the VLAN has
       * fast-leave admin mode enabled
       */
      if ((intfFastLeaveMode == L7_ENABLE) ||(fastLeaveMode == L7_ENABLE))
      {
        if (L7_INTF_ISMASKBITSET(snoopEntry->snoopGrpMemberList,
                                 mcastPacket->intIfNum))
        {
          if (snoopIntfRemove(dmac, mcastPacket->vlanId, mcastPacket->intIfNum,
                              SNOOP_GROUP_MEMBERSHIP, mcastPacket->cbHandle)
              != L7_SUCCESS)
          {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
                    "snoopMgmdLeaveGroupProcess: Failed to remove group membership");
          }
        }
      }
      else
      {
        if (snoop_is_timer_running(snoopEntry, mcastPacket->intIfNum,
                                   mcastPacket->vlanId, SNOOP_GROUP_MEMBERSHIP,
                                   mcastPacket->cbHandle) == L7_TRUE)
        {
          if (snoopQuerySend(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->cbHandle,
                             SNOOP_LEAVE_PROCESS_QUERY, L7_NULLPTR) != L7_SUCCESS)
          {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
                    "snoopMgmdLeaveGroupProcess: Failed to send group specific query");
          }
        }

        /* Entry exists... give back semaphore, set timers and transmit query */
        timerValue = snoopCheckPrecedenceParamGet(mcastPacket->vlanId,
                                                  mcastPacket->intIfNum,
                                                  SNOOP_PARAM_MAX_RESPONSE_TIME,
                                                  snoopEntry->snoopInfoDataKey.family);
        if (L7_INTF_ISMASKBITSET(snoopEntry->snoopGrpMemberList,
                                 mcastPacket->intIfNum))
        {
          snoopTimerUpdate(snoopEntry, mcastPacket->intIfNum, mcastPacket->vlanId,
                           SNOOP_MAX_RESPONSE, timerValue, mcastPacket->cbHandle);

        }/* End of membership check */
      }
    }
  } /* end of group address validity check */

  if (fwdFlag == L7_FALSE)
    snoopPacketRtrIntfsForward(mcastPacket);
  else
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "Flooding packet...");
    snoopPacketFlood(mcastPacket);
  }

  /* Update stats */
  pSnoopCB->counters.controlFramesProcessed++;
  return rc;
}
#endif

//static L7_RC_t igmp_packet_respond(mgmdSnoopControlPkt_t *mcastPacket);
static L7_RC_t igmp_packet_general_query_respond(mgmdSnoopControlPkt_t *mcastPacket, L7_inet_addr_t *groupIP);

#if 0
static L7_RC_t igmp_generate_packet_and_send(L7_uint32 vlan, L7_uint8 type, L7_uint32 groupIP);
#endif

/*********************************************************************
* @purpose Process IGMP/MLD Membership query message
*
* @param   mcastPacket  @b{(input)} Pointer to data structure to hold
*                                   received packet
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t snoopMgmdMembershipQueryProcess(mgmdSnoopControlPkt_t *mcastPacket)
{
  snoop_cb_t     *pSnoopCB = L7_NULLPTR;
//L7_RC_t         rc = L7_SUCCESS;
  L7_uchar8      *dataPtr = L7_NULL;
  L7_uint32       ipv4Addr;
  L7_in6_addr_t   ipv6Addr;
  L7_mgmdMsg_t    mgmdMsg;
  L7_uchar8       byteVal;
  L7_uchar8       dmac[L7_MAC_ADDR_LEN];
  L7_BOOL         generalQry = L7_FALSE;


  SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipQueryProcess");

  /* Interface must be root */
  if (ptin_igmp_rootIntfVlan_validate(mcastPacket->intIfNum, mcastPacket->vlanId)!=L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdMembershipQueryProcess: This is not a root interface (intIfNum=%u)!",mcastPacket->intIfNum);
    return L7_FAILURE;
  }

  /* Get Snoop Control Block */
  pSnoopCB = mcastPacket->cbHandle;
  /* If the src IP is non-zero, add this interface to the multicast router list
     and to all snooping entries */

  /* Set pointer to IGMP message */
  dataPtr = mcastPacket->ip_payload;

  if (mcastPacket->cbHandle->family == L7_AF_INET) /* IGMP Message */
  {
    memset(&mgmdMsg, 0x00, sizeof(L7_mgmdMsg_t));
    SNOOP_GET_BYTE(mgmdMsg.mgmdType, dataPtr);         /* Version/Type */
    SNOOP_GET_BYTE(mgmdMsg.igmpMaxRespTime, dataPtr);  /* Max response time */
    SNOOP_GET_SHORT(mgmdMsg.mgmdChecksum, dataPtr);    /* Checksum */
    SNOOP_GET_ADDR(&ipv4Addr, dataPtr);    /* Group Address */
    SNOOP_UNUSED_PARAM(dataPtr);
    inetAddressSet(L7_AF_INET, &ipv4Addr, &mgmdMsg.mgmdGroupAddr);
  }
  else /* MLD Message */
  {
    memset(&mgmdMsg, 0x00, sizeof(L7_mgmdMsg_t));
    SNOOP_GET_BYTE(mgmdMsg.mgmdType, dataPtr);         /* Version/Type */
    SNOOP_GET_BYTE(byteVal, dataPtr);  /* Max response time */
    SNOOP_UNUSED_PARAM(byteVal);
    SNOOP_GET_SHORT(mgmdMsg.mgmdChecksum, dataPtr);    /* Checksum */
    SNOOP_GET_SHORT(mgmdMsg.mldMaxRespTime, dataPtr);  /* Max response time */
    SNOOP_GET_SHORT(mgmdMsg.mgmdReserved, dataPtr);               /* rserved */

    SNOOP_GET_ADDR6(&ipv6Addr, dataPtr); /* Group Address */
    inetAddressSet(L7_AF_INET6, &ipv6Addr, &mgmdMsg.mgmdGroupAddr);
  }

  if (mcastPacket->cbHandle->family == L7_AF_INET6)
  {
    /* Check if it is general query address or group specific */
    if (inetIsAddressZero(&mgmdMsg.mgmdGroupAddr) == L7_TRUE)
    {
      L7_uchar8 mldQryAddr[L7_IP6_ADDR_LEN];
      L7_uchar8 ipBuf[L7_IP6_ADDR_LEN];

      memset(mldQryAddr, 0x00, L7_IP6_ADDR_LEN);
      osapiInetPton(L7_AF_INET6, SNOOP_IP6_ALL_HOSTS_ADDR, mldQryAddr);

      /* Check if it is equal to the all hosts address FF02::1 */
      inetAddressGet(L7_AF_INET6, &mcastPacket->destAddr, ipBuf);
      if (memcmp(ipBuf, mldQryAddr, L7_IP6_ADDR_LEN) != 0)
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipQueryProcess: Invalid Packet");
        return L7_FAILURE;
      }
      generalQry = L7_TRUE;
//    ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_GENERAL_QUERY_VALID_RX);
    }
    else /* Should be group specific query */
    {
      if (inetIsInMulticast(&mgmdMsg.mgmdGroupAddr) == L7_TRUE)
      {
        if (L7_INET_ADDR_COMPARE(&mcastPacket->destAddr, &mgmdMsg.mgmdGroupAddr) != 0)
        {
          SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipQueryProcess: Invalid Packet");
          return L7_FAILURE;
        }
      }
      else
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipQueryProcess: Invalid Packet");
        return L7_FAILURE;
      }      
    }
  }
  else
  {
    /* Check if it is general query address or group specific */
    if (inetIsAddressZero(&mgmdMsg.mgmdGroupAddr) == L7_TRUE)
    {
      /* Check if IPv4 destination address is same as 224.0.0.1 */
      inetAddressGet(mcastPacket->cbHandle->family, &mcastPacket->destAddr,
                     &ipv4Addr);
      if (ipv4Addr != L7_IP_ALL_HOSTS_ADDR)
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipQueryProcess: Invalid Packet");
        return L7_FAILURE;
      }
//    ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_GENERAL_QUERY_VALID_RX);
    }
    else /* Should be group specific query */
    {
      if (inetIsInMulticast(&mgmdMsg.mgmdGroupAddr) == L7_TRUE)
      {
        if (L7_INET_ADDR_COMPARE(&mcastPacket->destAddr, &mgmdMsg.mgmdGroupAddr) != 0)
        {
          SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipQueryProcess: Invalid Packet");
          return L7_FAILURE;
        }
      }
      else
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipQueryProcess: Invalid Packet");
        return L7_FAILURE;
      }
//    ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_TOTAL_RX);
    }
  }

  memset(dmac, 0x00, sizeof(dmac));
  /* Validate the group address being reported */
  snoopMulticastMacFromIpAddr(&mcastPacket->destAddr, dmac);
  if (snoopMacAddrCheck(dmac, mcastPacket->cbHandle->family) != L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipQueryProcess: Invalid Packet");
    return L7_FAILURE;
  }
  /* Match the destination mac and the mac resloved from ipv4 dest addr */
  if (memcmp(mcastPacket->destMac, dmac, L7_MAC_ADDR_LEN) != 0)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipQueryProcess: Invalid Packet");
    return L7_FAILURE;
  }

#if 0
  if (inetIsAddressZero(&mcastPacket->srcAddr) != L7_TRUE)
  {
    /* If ipv6 options header is not supported in hardware.
       In such case we only process MLDv2 General queries with hop options */
    if (mcastPacket->cbHandle->family == L7_AF_INET6 &&
        mcastPacket->cbHandle->snoopExec->ipv6OptionsSupport == L7_FALSE &&
        mcastPacket->routerAlert == L7_TRUE)
    {
      if (mcastPacket->ip_payload_length > SNOOP_MLDV1_HEADER_LENGTH &&
          generalQry == L7_TRUE)
      {
        if ((rc = snoopQuerierQueryProcess(mcastPacket)) == L7_SUCCESS)
        {
          /* PTin removed: IGMP snooping */
          snoopMcastRtrListAdd(mcastPacket->vlanId, mcastPacket->intIfNum, 
                               SNOOP_DYNAMIC, mcastPacket->cbHandle);
        }
      }
      else if (generalQry == L7_FALSE)
      {
        /* Do not flood group specific queries as they are switched in 
           hardware and are not copied to CPU. If we flood then two copies
           are created for the packet. */
        rc = L7_FAILURE;
      }
    }
    else
    {
      if ((rc = snoopQuerierQueryProcess(mcastPacket)) == L7_SUCCESS)
      {
        /* PTin removed: IGMP snooping */
        snoopMcastRtrListAdd(mcastPacket->vlanId, mcastPacket->intIfNum, 
                             SNOOP_DYNAMIC, mcastPacket->cbHandle);
      }
    }
  }
#endif

  pSnoopCB->counters.controlFramesProcessed++;

  if (igmp_packet_general_query_respond(mcastPacket, &mgmdMsg.mgmdGroupAddr)!=L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipQueryProcess: Error responding with channel list");
    return L7_FAILURE;
  }
  SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipQueryProcess: Query was processed");

#if 0
  /* Forward membership query on all interfaces on this VLAN, except the
     incoming interface */
  if (rc == L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "snoopMgmdMembershipQueryProcess: Flooding packet...");
    snoopPacketFlood(mcastPacket);
  }
#endif

  return L7_SUCCESS;
}



#if SNOOP_PTIN_IGMPv3_PROXY 
/*********************************************************************
* @purpose Process IGMPv3/MLDv2 Membership query message
*
* @param   mcastPacket  @b{(input)} Pointer to data structure to hold
*                                   received packet
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t snoopMgmdSrcSpecificMembershipQueryProcess(mgmdSnoopControlPkt_t *mcastPacket)
{

  snoop_cb_t                  *pSnoopCB ;
  snoop_eb_t                  *pSnoopEB ;

  snoopPTinL3InfoData_t *      avlTreeEntry=L7_NULLPTR;
  ptin_IgmpProxyCfg_t          igmpCfg;
  L7_uchar8                   *dataPtr = L7_NULL;
  L7_uint32                    ipv4Addr, incomingVersion = 0,timeout=0;
//L7_in6_addr_t                ipv6Addr;
//L7_mgmdQueryMsg_t            mgmdMsg;
  L7_uchar8                    byteVal,robustnessVariable=PTIN_IGMP_DEFAULT_ROBUSTNESS;
  L7_ushort16                  maxRespCode=0,recdChecksum;

  L7_uchar8                    dmac[L7_MAC_ADDR_LEN];                 
  L7_char8                     queryType=-1/*,mRouterVersion=-1*/;
  char                         debug_buf[IPV6_DISP_ADDR_LEN];

  L7_uint16                    selectedDelay = 0,maxRespTime=0,noOfSources=0,sourceIdx;  

  void *ptr=L7_NULLPTR;

  L7_BOOL   isInterface=L7_FALSE;
  L7_BOOL   sendReport=L7_FALSE;

  L7_inet_addr_t  groupAddr,sourceList[PTIN_IGMP_DEFAULT_MAX_SOURCES_PER_GROUP_RECORD]={};  

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Membership Query Message Rec'd");

  /* Interface must be root */
  if (ptin_igmp_rootIntfVlan_validate(mcastPacket->intIfNum, mcastPacket->vlanId)!=L7_SUCCESS)
  {
//  ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_IGMP_DROPPED);
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"This is not a root interface (intIfNum=%u)! Packet silently discarded.",mcastPacket->intIfNum);
    return L7_ERROR;
  }


  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get__snooping_old(&igmpCfg) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations");        
  }
  else
  {    
    robustnessVariable=igmpCfg.host.robustness;
  }

  /* Get Snoop Control Block */
  pSnoopCB = mcastPacket->cbHandle;
  /* If the src IP is non-zero, add this interface to the multicast router list
     and to all snooping entries */

  /* Set pointer to IGMP message */
  dataPtr = mcastPacket->ip_payload;

  if ((pSnoopEB= snoopEBGet())== L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to snoopEBGet()");
    return L7_ERROR;
  }

  /*Let us verify if we do have any MGMD Host*/
  if (avlTreeCount(&pSnoopEB->snoopPTinL3AvlTree)==0)
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Membership Query Packet silently ignored: We do not have active MGMD Hosts");
    return L7_SUCCESS;
  }
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
  memset(dmac, 0x00, sizeof(dmac));
  /* Validate the group address being reported */
  snoopMulticastMacFromIpAddr(&mcastPacket->destAddr, dmac);
  if (snoopMacAddrCheck(dmac, mcastPacket->cbHandle->family) != L7_SUCCESS)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "Invalid Packet, packet silently discarded");
    return L7_FAILURE;
  }
  /* Match the destination mac and the mac resloved from ipv4 dest addr */
  if (memcmp(mcastPacket->destMac, dmac, L7_MAC_ADDR_LEN) != 0)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "Invalid Packet, packet silently discarded");
    return L7_FAILURE;
  }
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

  if (mcastPacket->cbHandle->family == L7_AF_INET) /* IGMP Message */
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Dst Addr %s",inetAddrPrint(&mcastPacket->destAddr,debug_buf));

    //memset(&mgmdMsg, 0x00, sizeof(L7_mgmdQueryMsg_t));

    SNOOP_GET_BYTE(byteVal, dataPtr);         /* Version/Type */

    SNOOP_GET_BYTE(maxRespCode, dataPtr);  /*Max Response Code - 8 Bits IGMP*/
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Max Resp Code=%u",maxRespCode);
    
    SNOOP_GET_SHORT(recdChecksum, dataPtr);    /* Checksum */
    SNOOP_GET_ADDR(&ipv4Addr, dataPtr);    /* Group Address */

    inetAddressSet(L7_AF_INET, &ipv4Addr, &groupAddr);
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Group Addr %s",inetAddrPrint(&groupAddr,debug_buf));


    if (mcastPacket->ip_payload==L7_NULLPTR ||  mcastPacket->ip_payload_length==0)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Either the IP payload is a null pointer or the ip payload length is 0");
      return L7_ERROR;      
    }

    if (mcastPacket->ip_payload_length > SNOOP_IGMPv1v2_HEADER_LENGTH)
    {

#if 1//igmpv3_tos_rtr_alert_check
      if (pSnoopCB->snoopCfgData->igmpv3_tos_rtr_alert_check == L7_TRUE)
      {
        if (mcastPacket->tosByte != SNOOP_TOS_VALID_VALUE)
        {
          ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_GENERIC_QUERY_INVALID_RX);
          LOG_WARNING(LOG_CTX_PTIN_IGMP,"Packet rec'd with TOS invalid, packet silently discarded");
          return L7_FAILURE;
        }

        if (mcastPacket->routerAlert != L7_TRUE)
        {
          LOG_WARNING(LOG_CTX_PTIN_IGMP,"Packet rec'd with Router Alert Option not Active, packet silently discarded");
          return L7_FAILURE;
        }
      }
#endif

    /*4.1.6. QRV (Querier’s Robustness Variable)*/
    /*...in which case the receivers use the default [Robustness Variable] value specified in
    section 8.1 or a statically configured value. */
      SNOOP_GET_BYTE(byteVal, dataPtr);  /*Resv+SFlag+QRV - 4 bits + 1 bit + 3 bits*/  
#if 0       
      robustnessVariable = byteVal & 0x07;    
      if (robustnessVariable<PTIN_MIN_ROBUSTNESS_VARIABLE)
      {
        LOG_WARNING(LOG_CTX_PTIN_IGMP,"Invalid robustness Variable, packet silently discarded");
        return L7_FAILURE;
      }
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Robustness Variable=%u",robustnessVariable);  
#else
      byteVal = byteVal & 0x07;    
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Robustness Variable=%u",byteVal);  
#endif
        
      SNOOP_GET_BYTE(byteVal, dataPtr);  /* QQIC */
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"QQIC=%u",byteVal);
      SNOOP_GET_SHORT(noOfSources, dataPtr);  /* Number of sources */
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"noOfSources=%u",noOfSources);
      SNOOP_UNUSED_PARAM(dataPtr);
      if (mcastPacket->ip_payload_length !=
          (SNOOP_IGMPV3_HEADER_MIN_LENGTH +
           noOfSources * sizeof(L7_in_addr_t)))
      {
        LOG_WARNING(LOG_CTX_PTIN_IGMP,"Invalid IGMPv2 Membership Query Message Length %u, packet silently discarded", mcastPacket->ip_payload_length);
        return L7_FAILURE;
      }
      incomingVersion = SNOOP_IGMP_VERSION_3;
    }
    else if (maxRespCode == 0)
    {
      if (mcastPacket->ip_payload_length != SNOOP_IGMPv1v2_HEADER_LENGTH)
      {
        LOG_WARNING(LOG_CTX_PTIN_IGMP,"Invalid IGMPv1 Membership Query Message Length: %u, packet silently discarded", mcastPacket->ip_payload_length);
        return L7_FAILURE;
      }
      incomingVersion = SNOOP_IGMP_VERSION_1;
    }
    else
    {
      if (mcastPacket->ip_payload_length != SNOOP_IGMPv1v2_HEADER_LENGTH)
      {
        LOG_WARNING(LOG_CTX_PTIN_IGMP,"Invalid IGMPv2 Membership Query Message Length: %u, packet silently discarded", mcastPacket->ip_payload_length);
        return L7_FAILURE;
      }
#if 1//igmpv3_tos_rtr_alert_check
      if (pSnoopCB->snoopCfgData->igmpv3_tos_rtr_alert_check == L7_TRUE)
      {
        if (mcastPacket->routerAlert != L7_TRUE)
        {
          LOG_WARNING(LOG_CTX_PTIN_IGMP,"IGMPv2 Membership Query Message rec'd with Router Alert Option not Active, packet silently discarded");
          return L7_FAILURE;
        }
      }
#endif
      incomingVersion = SNOOP_IGMP_VERSION_2;
    }
  }/* Is IGMP pkt check */
#if 0//Snooping
  else /* MLD Message */
  {
    memset(&mgmdMsg, 0x00, sizeof(L7_mgmdMsg_t));
    SNOOP_GET_BYTE(mgmdMsg.mgmdType, dataPtr);         /* Version/Type */
    SNOOP_GET_BYTE(byteVal, dataPtr);  /* Max response time */
    SNOOP_UNUSED_PARAM(byteVal);
    SNOOP_GET_SHORT(mgmdMsg.mgmdChecksum, dataPtr);    /* Checksum */
    SNOOP_GET_SHORT(mgmdMsg.mldMaxRespTime, dataPtr);  /* Max response time */
    SNOOP_GET_SHORT(mgmdMsg.mgmdReserved, dataPtr);               /* rserved */

    SNOOP_GET_ADDR6(&ipv6Addr, dataPtr); /* Group Address */
    inetAddressSet(L7_AF_INET6, &ipv6Addr, &mgmdMsg.mgmdGroupAddr);
  }
#else//Proxy
  else /* MLD Message */
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"IPv6 currently not supported, packet silently discarded");
    return L7_SUCCESS;
#if 0 //We have disable IPv6 processing
    memset(&mgmdMsg, 0x00, sizeof(L7_mgmdQueryMsg_t));
    SNOOP_GET_BYTE(mgmdMsg.mgmdType, dataPtr);   /* Version/Type */
    SNOOP_GET_BYTE(byteVal, dataPtr);  /* Code */
    SNOOP_UNUSED_PARAM(byteVal);
    SNOOP_GET_SHORT(mgmdMsg.mgmdChecksum, dataPtr);    /* Checksum */
    SNOOP_GET_SHORT(maxRespCode, dataPtr);  /* Max response time */
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Max Resp Code=%d",maxRespCode);
    SNOOP_GET_SHORT(mgmdMsg.mgmdReserved, dataPtr);/* rserved */

    SNOOP_GET_ADDR6(&ipv6Addr, dataPtr); /* Group Address */
    inetAddressSet(L7_AF_INET6, &ipv6Addr, &groupAddr);
    if (mcastPacket->ip_payload_length > SNOOP_MLDV1_HEADER_LENGTH) /* MIN MLD qry length */
    {
      SNOOP_GET_BYTE(mgmdMsg.qqic, dataPtr);  /* QQIC */
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"mgmdMsg.qqic=%d",mgmdMsg.qqic);
      SNOOP_GET_SHORT(noOfSources, dataPtr);  /* Number of sources */
      SNOOP_UNUSED_PARAM(dataPtr);
      if (mcastPacket->ip_payload_length !=
          (noOfSources * sizeof(L7_in6_addr_t) + SNOOP_MLDV2_HEADER_MIN_LENGTH))
      {
        LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Illegal MLDv2 packet length = %d", mcastPacket->ip_payload_length);
        return L7_FAILURE;
      }
      incomingVersion = SNOOP_MLD_VERSION_2;
    }
    else
    {
      if (mcastPacket->ip_payload_length != SNOOP_MLDV1_HEADER_LENGTH)
      {
        LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Illegal MLDv1 packet length = %d", mcastPacket->ip_payload_length);
        return L7_FAILURE;
      }
      incomingVersion = SNOOP_MLD_VERSION_1;
    }
#endif
  }/* End of MLD Message check */
#endif

  /*This only makes sense for IGMPv2 and IGMPv3*/
  if(incomingVersion==SNOOP_IGMP_VERSION_2 || incomingVersion==SNOOP_IGMP_VERSION_3)
  {
    /* As the packet has the max-respons-time in 1/10 of secs, convert it to seconds
        for further processing */
    maxRespTime = snoopPTinProxy_decode_max_resp_code(mcastPacket->cbHandle->family,maxRespCode);
    if (maxRespTime==0)
    {
      LOG_WARNING(LOG_CTX_PTIN_IGMP,"Max Response Time equal to zero, packet silently discarded");
      return L7_FAILURE;
    }
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Max Response Time=%u",maxRespTime);
    /* Calculate the Selected delay */
    selectedDelay = snoopPTinProxy_selected_delay_calculate((L7_int32) maxRespTime); 
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Selected Delay=%d",selectedDelay);
  }
  

  if (mcastPacket->cbHandle->family == L7_AF_INET)/*IPv4*/
  {
    switch (incomingVersion)
    {
    case SNOOP_IGMP_VERSION_1:
      {
#if 0
        /* Check if it is general query address or group specific */
        if (inetIsAddressZero(&groupAddr) == L7_TRUE)
        {
          /* Check if IPv4 destination address is same as 224.0.0.1 */
          inetAddressGet(mcastPacket->cbHandle->family, &mcastPacket->destAddr,
                         &ipv4Addr);
          if (ipv4Addr != L7_IP_ALL_HOSTS_ADDR)
          {
            LOG_DEBUG(LOG_CTX_PTIN_IGMP,"snoopMgmdSrcSpecificMembershipQueryProcess: IPv4 dest addr %s!=224.0.0.1",snoopPTinIPv4AddrPrint(mcastPacket->destAddr,debug_buf));
            return L7_FAILURE;
          }
          LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopMgmdSrcSpecificMembershipQueryProcess: IGMPv1 General Query" );
          ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_GENERAL_QUERIES_RECEIVED);
        }
        else /* Should be group specific query */
        {
          if (inetIsInMulticast(&groupAddr) == L7_TRUE)
          {
            if (L7_INET_ADDR_COMPARE(&mcastPacket->destAddr, &groupAddr) != 0)
            {
              LOG_DEBUG(LOG_CTX_PTIN_IGMP,"snoopMgmdSrcSpecificMembershipQueryProcess:  Ipv4 dst addr != Group Addr %s!=%s"\,
                        snoopPTinIPv4AddrPrint(mcastPacket->destAddr,debug_buf),snoopPTinIPv4AddrPrint(groupAddr,debug_buf));
              return L7_FAILURE;
            }
          }
          else
          {
            LOG_DEBUG(LOG_CTX_PTIN_IGMP,"snoopMgmdSrcSpecificMembershipQueryProcess: Multicast Ipv4 Group Addr Invalid =%s"\,
                      ,snoopPTinIPv4AddrPrint(groupAddr,debug_buf));
            return L7_FAILURE;
          }
          LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopMgmdSrcSpecificMembershipQueryProcess: IGMPv1 Group Specific Query )";
                   ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_SPECIFIC_QUERIES_RECEIVED);

                   }
                   }
#else
        LOG_WARNING(LOG_CTX_PTIN_IGMP,"IGMPv1 Query Rec'd, Packet Silently Ignored");
        return L7_NOT_SUPPORTED;
#endif
        break;
      }
    case SNOOP_IGMP_VERSION_2:
      {
#if 1
        /* Check if it is general query address or group specific */
        if (inetIsAddressZero(&groupAddr) == L7_TRUE)
        {
          /* Check if IPv4 destination address is same as 224.0.0.1 */
          inetAddressGet(mcastPacket->cbHandle->family, &mcastPacket->destAddr,
                         &ipv4Addr);
          if (ipv4Addr != L7_IP_ALL_HOSTS_ADDR)
          {
            LOG_WARNING(LOG_CTX_PTIN_IGMP,"Invalid IGMPv2 General Query Rec'd:: IPv4 dest addr %s!=224.0.0.1, packet silently discarded",inetAddrPrint(&mcastPacket->destAddr,debug_buf));
            pSnoopCB->counters.controlFramesProcessed++;                    
            return L7_FAILURE;
          }
          LOG_TRACE(LOG_CTX_PTIN_IGMP,"IGMPv2 General Query Rec'd");    
          queryType=L7_IGMP_MEMBERSHIP_QUERY;      
        }
        else /* Should be group specific query */
        {
          if (inetIsInMulticast(&groupAddr) == L7_TRUE)
          {
            if (L7_INET_ADDR_COMPARE(&mcastPacket->destAddr, &groupAddr) != 0)
            {
              LOG_WARNING(LOG_CTX_PTIN_IGMP,"Invalid IGMPv2 Group Specific Query Rec'd: IPv4 dst addr != Group Addr %s!=%s, packet silently discarded",
                          inetAddrPrint(&mcastPacket->destAddr,debug_buf),inetAddrPrint(&groupAddr,debug_buf));
              pSnoopCB->counters.controlFramesProcessed++;                    
              return L7_FAILURE;
            }
          }
          else
          {
            LOG_WARNING(LOG_CTX_PTIN_IGMP,"Invalid IGMPv2 Group Specific Query Rec'd: Multicast IPv4 Group Addr Invalid =%s, packet silently discarded",
                        inetAddrPrint(&groupAddr,debug_buf));
            pSnoopCB->counters.controlFramesProcessed++;                    
            return L7_FAILURE;
          }

          /*Let us verify if this group is registered by any IGMPv3 Host*/            
          if ((avlTreeEntry=snoopPTinL3EntryFind(mcastPacket->vlanId,&groupAddr,L7_MATCH_EXACT))==L7_NULLPTR || 
              avlTreeEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].active==L7_FALSE ||
              snoopPTinZeroClients(avlTreeEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].clients)!=L7_ALREADY_CONFIGURED)
          {
            LOG_TRACE(LOG_CTX_PTIN_IGMP,"Failed to find group for which grp-query is rx'ed: %s. Packet silently ignored.",inetAddrPrint(&groupAddr,debug_buf));
            return L7_SUCCESS;
          }
          LOG_TRACE(LOG_CTX_PTIN_IGMP,"IGMPv2 Group Specific Query Rec'd");
          queryType=L7_IGMP_MEMBERSHIP_GROUP_SPECIFIC_QUERY;  
        }
#else
        LOG_WARNING(LOG_CTX_PTIN_IGMP,"IGMPv2 Query Rec'd, Packet Silently Ignored");
        return L7_NOT_IMPLEMENTED_YET;
#endif
        break;          
      }
    case SNOOP_IGMP_VERSION_3:
      {           
        /* Check if it is general query address or group specific */
        if (inetIsAddressZero(&groupAddr) == L7_TRUE)
        {          
          if (noOfSources!=0)
          {
            LOG_WARNING(LOG_CTX_PTIN_IGMP,"Invalid IGMPv3 General Query Rec'd: MGroupAddr=0 & NSources=%d, packet silently discarded",noOfSources);
            pSnoopCB->counters.controlFramesProcessed++;    
            return L7_FAILURE;
          }
          /* Check if IPv4 destination address is same as 224.0.0.1 */
          inetAddressGet(mcastPacket->cbHandle->family, &mcastPacket->destAddr,
                         &ipv4Addr);
          if (ipv4Addr != L7_IP_ALL_HOSTS_ADDR)
          {
            LOG_WARNING(LOG_CTX_PTIN_IGMP,"Invalid IGMPv3 General Query Rec'd: IPv4 dest addr %s!=224.0.0.1, packet silently discarded",inetAddrPrint(&mcastPacket->destAddr,debug_buf));
            pSnoopCB->counters.controlFramesProcessed++;                    
            return L7_FAILURE;
          }
          LOG_DEBUG(LOG_CTX_PTIN_IGMP,"IGMPv3 General Query Rec'd" );
          queryType=L7_IGMP_MEMBERSHIP_QUERY;            
        }
        else /* Should be group or group & source specific query */
        {
          if (inetIsInMulticast(&groupAddr) == L7_TRUE)
          {
            if (L7_INET_ADDR_COMPARE(&mcastPacket->destAddr, &groupAddr) != 0)
            {
              LOG_WARNING(LOG_CTX_PTIN_IGMP,"Invalid IGMPv3 Group Specific Query Rec'd: Ipv4 dst addr != Group Addr - %s!=%s, packet silently discarded",
                          inetAddrPrint(&mcastPacket->destAddr,debug_buf),inetAddrPrint(&groupAddr,debug_buf));
              pSnoopCB->counters.controlFramesProcessed++;                    
              return L7_FAILURE;
            }
            if (noOfSources==0)
            {
              LOG_DEBUG(LOG_CTX_PTIN_IGMP,"IGMPv3 Group Specific Query Rec'd");
              queryType=L7_IGMP_MEMBERSHIP_GROUP_SPECIFIC_QUERY;              
            }
            else
            {
              if (noOfSources>PTIN_IGMP_DEFAULT_MAX_SOURCES_PER_GROUP_RECORD)
              {
                LOG_WARNING(LOG_CTX_PTIN_IGMP,"noOfSources higher than maximum allowed value changing Group and Source Query to Group Specific Query %u>%u",noOfSources,PTIN_IGMP_DEFAULT_MAX_SOURCES_PER_GROUP_RECORD); 
                noOfSources=0;
                queryType=L7_IGMP_MEMBERSHIP_GROUP_SPECIFIC_QUERY;  
              }
              else
              {
                LOG_DEBUG(LOG_CTX_PTIN_IGMP,"IGMPv3 Group & Source Specific Query Rec'd");              
                queryType=L7_IGMP_MEMBERSHIP_GROUP_AND_SOURCE_SCPECIFC_QUERY;             
              }
            }
          }
          else
          {
            LOG_WARNING(LOG_CTX_PTIN_IGMP,"Invalid IGMPv3 Group Specific Query Rec'd: Multicast Ipv4 Group Addr Invalid =%s, packet silently discarded",
                        inetAddrPrint(&groupAddr,debug_buf));
            pSnoopCB->counters.controlFramesProcessed++;                    
            return L7_FAILURE;
          }

          /*Let us verify if this group is registered by any IGMPv3 Host*/            
          if ((avlTreeEntry=snoopPTinL3EntryFind(mcastPacket->vlanId,&groupAddr,L7_MATCH_EXACT))==L7_NULLPTR || 
              avlTreeEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].active==L7_FALSE ||
              snoopPTinZeroClients(avlTreeEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].clients)!=L7_ALREADY_CONFIGURED)
          {
            LOG_TRACE(LOG_CTX_PTIN_IGMP,"Failed to find group for which grp-query is rx'ed: %s. Packet silently ignored.",inetAddrPrint(&groupAddr,debug_buf));
            return L7_SUCCESS;
          }
        }
        break;
      }
    }
  }
  else/*IPv6*/
  {
#if 0
    /* Check if it is general query address or group specific */
    if (inetIsAddressZero(&groupAddr) == L7_TRUE)
    {
      L7_uchar8 mldQryAddr[L7_IP6_ADDR_LEN];
      L7_uchar8 ipBuf[L7_IP6_ADDR_LEN];

      memset(mldQryAddr, 0x00, L7_IP6_ADDR_LEN);
      osapiInetPton(L7_AF_INET6, SNOOP_IP6_ALL_HOSTS_ADDR, mldQryAddr);

      /* Check if it is equal to the all hosts address FF02::1 */
      inetAddressGet(L7_AF_INET6, &mcastPacket->destAddr, ipBuf);
      if (memcmp(ipBuf, mldQryAddr, L7_IP6_ADDR_LEN) != 0)
      {
        LOG_WARNING(LOG_CTX_PTIN_IGMP,"Invalid Packet");
        return L7_FAILURE;
      }
      queryType=SNOOP_PTIN_GENERAL_QUERY;
      ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_GENERAL_QUERY_VALID_RX);
    }
    else /* Should be group specific query */
    {
      if (inetIsInMulticast(&groupAddr) == L7_TRUE)
      {
        if (L7_INET_ADDR_COMPARE(&mcastPacket->destAddr, &groupAddr) != 0)
        {
          SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
          return L7_FAILURE;
        }

      }
      else
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
        return L7_FAILURE;
      }
      ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_TOTAL_RX);
    }
#endif
  }

  if (mcastPacket->cbHandle->family == L7_AF_INET)
  {
    /* Add new source */
    for (sourceIdx=0;sourceIdx<noOfSources;sourceIdx++)
    {
      memset(&sourceList[sourceIdx], 0x00, sizeof(L7_inet_addr_t));             
      {
        SNOOP_GET_ADDR(&ipv4Addr, dataPtr);
        inetAddressSet(L7_AF_INET, &ipv4Addr, &sourceList[sourceIdx]);
        if (inetIpAddressValidityCheck(L7_AF_INET,&sourceList[sourceIdx])!=L7_SUCCESS)
        {
          LOG_WARNING(LOG_CTX_PTIN_IGMP, "Invalid Source IP Address %s. Packet silently ignored.",inetAddrPrint(&sourceList[sourceIdx], debug_buf));
          return L7_FAILURE;
        }
      }
    }
  }
  else
  {
#if 0
    /* Add new source */
    for (sourceIdx=0;sourceIdx<noOfSources;sourceIdx++)
    {
      memset(&sourceList[sourceIdx], 0x00, sizeof(L7_inet_addr_t));
      /* IPv6 MCAST Address */
      SNOOP_GET_ADDR6(&ipv6Addr, dataPtr);
      inetAddressSet(L7_AF_INET6, &ipv6Addr, &sourceList[sourceIdx]);

      if (inetIpAddressValidityCheck(L7_AF_INET,&sourceList[sourceIdx])!=L7_SUCCESS)
      {
        LOG_TRACE(LOG_CTX_PTIN_IGMP, "Invalid Source IP Address %s",inetAddrPrint(&sourceList[sourceIdx], debug_buf));
        return L7_FAILURE;
      }
    }
#endif
  }    

  switch (queryType)
  {
  case L7_IGMP_MEMBERSHIP_QUERY:
    {      
      ptr=snoopPTinGeneralQueryProcess(mcastPacket->vlanId, SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM, selectedDelay, &sendReport, &timeout,robustnessVariable);
//    snoopStatIgmpField=SNOOP_STAT_FIELD_GENERAL_QUERY_VALID_RX;
      isInterface=L7_TRUE;      
      break;
    }

  case L7_IGMP_MEMBERSHIP_GROUP_SPECIFIC_QUERY :
    {
      ptr=snoopPTinGroupSpecifcQueryProcess(avlTreeEntry, SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM,selectedDelay, &sendReport, &timeout,robustnessVariable);      
//    snoopStatIgmpField=SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_VALID_RX;
      break;
    }
  case L7_IGMP_MEMBERSHIP_GROUP_AND_SOURCE_SCPECIFC_QUERY:
    {
      ptr=snoopPTinGroupSourceSpecifcQueryProcess(avlTreeEntry, SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM, noOfSources, sourceList, selectedDelay, &sendReport, &timeout,robustnessVariable);
//    snoopStatIgmpField=SNOOP_STAT_FIELD_GROUP_AND_SOURCE_SPECIFIC_QUERY_VALID_RX;
      break;
    }
  }

  if (ptr !=L7_NULLPTR && sendReport==L7_TRUE && timeout>=PTIN_IGMP_MIN_UNSOLICITEDREPORTINTERVAL)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Scheduling Membership Report Message with timeout: %u ",timeout); 

    if (snoopPTinScheduleReportMessage(mcastPacket->vlanId,&groupAddr,queryType,selectedDelay,isInterface,1, ptr,robustnessVariable)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed snoopPTinReportSchedule()");
      return L7_FAILURE;
    }
  }
  else if (ptr ==L7_NULLPTR && sendReport==L7_TRUE)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "sendReport Flag is equal to L7_TRUE, while groupPtr=L7_NULLPTR");      
  }
  else if (timeout<PTIN_IGMP_MIN_UNSOLICITEDREPORTINTERVAL)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "Selected delay smaller than allowed value (%u<1)",timeout);
  }
  else
  {
    LOG_NOTICE(LOG_CTX_PTIN_IGMP, "sendReport Flag is equal to L7_FALSE");      
  }


  pSnoopCB->counters.controlFramesProcessed++;  
  LOG_TRACE(LOG_CTX_PTIN_IGMP, "snoopMgmdSrcSpecificMembershipQueryProcess: Query was processed");         

//ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, snoopStatIgmpField);

  return L7_SUCCESS;
}

/*End MGMD Proxy*/
/************************************************************************************************************/
#endif

/*********************************************************************
* @purpose Process IGMPv1/IGMPv2/MLDv1/MLDv2 Group Membership Reports
*
* @param   mcastPacket  @b{(input)} Pointer to data structure to hold
*                                   received packet
*
* @returns L7_SUCCESS
* @returns L7_FAILURE  Invalid packet
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t snoopMgmdMembershipReportProcess(mgmdSnoopControlPkt_t *mcastPacket)
{
  L7_uchar8       *dataPtr;
  L7_mgmdMsg_t     mgmdMsg;
  L7_BOOL          fwdFlag = L7_FALSE;
  L7_uchar8        dmac[L7_MAC_ADDR_LEN], byteVal;
  L7_RC_t          rc = L7_SUCCESS;
  L7_uint32        ipv4Addr;
  snoop_cb_t      *pSnoopCB = L7_NULLPTR;
  L7_in6_addr_t    ipv6Addr;
#if 0
  snoopInfoData_t *snoopEntry;
  ptin_McastClient_id_t clientId;
  L7_uint               client_index;
#endif

  ptin_timer_start(1,"snoopMgmdMembershipReportProcess-start");

  SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipReportProcess");

  /* Interface must be client */
  if (ptin_igmp_clientIntfVlan_validate(mcastPacket->intIfNum, mcastPacket->vlanId)!=L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdMembershipReportProcess: This is not a client interface (intIfNum=%u)!",mcastPacket->intIfNum);
    return L7_FAILURE;
  }

  /* Set pointer to IGMP message */
  dataPtr = mcastPacket->ip_payload;

  /* Get Snoop Control Block */
  pSnoopCB = mcastPacket->cbHandle;

  if (mcastPacket->cbHandle->family == L7_AF_INET) /* IGMP Message */
  {
    memset(&mgmdMsg, 0x00, sizeof(L7_mgmdMsg_t));
    SNOOP_GET_BYTE(mgmdMsg.mgmdType, dataPtr);         /* Version/Type */
    SNOOP_GET_BYTE(mgmdMsg.igmpMaxRespTime, dataPtr);  /* Max response time */
    SNOOP_GET_SHORT(mgmdMsg.mgmdChecksum, dataPtr);    /* Checksum */
    SNOOP_GET_ADDR(&ipv4Addr, dataPtr);    /* Group Address */
    SNOOP_UNUSED_PARAM(dataPtr);
    inetAddressSet(L7_AF_INET, &ipv4Addr, &mgmdMsg.mgmdGroupAddr);

    if (mgmdMsg.mgmdType == L7_IGMP_V2_MEMBERSHIP_REPORT &&
        pSnoopCB->snoopCfgData->igmpv3_tos_rtr_alert_check == L7_TRUE)
    {
      if (mcastPacket->routerAlert != L7_TRUE)
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipReportProcess: Invalid Packet");
        return L7_FAILURE;
      }
    }
  }
  else
  {
    /* MLD Message */
    memset(&mgmdMsg, 0x00, sizeof(L7_mgmdMsg_t));
    SNOOP_GET_BYTE(mgmdMsg.mgmdType, dataPtr);         /* Version/Type */
    SNOOP_GET_BYTE(byteVal, dataPtr);  /* Max response time */
    SNOOP_UNUSED_PARAM(byteVal);
    SNOOP_GET_SHORT(mgmdMsg.mgmdChecksum, dataPtr);    /* Checksum */
    SNOOP_GET_SHORT(mgmdMsg.mldMaxRespTime, dataPtr);  /* Max response time */
    SNOOP_GET_SHORT(mgmdMsg.mgmdReserved, dataPtr);               /* rserved */

    SNOOP_GET_ADDR6(&ipv6Addr, dataPtr); /* Group Address */
    inetAddressSet(L7_AF_INET6, &ipv6Addr, &mgmdMsg.mgmdGroupAddr);
  }

  if (L7_INET_ADDR_COMPARE(&mcastPacket->destAddr, &mgmdMsg.mgmdGroupAddr) != 0)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipReportProcess: Invalid Packet");
    return L7_FAILURE;
  }

  memset(dmac, 0x00, sizeof(dmac));
  /* Validate the group address being reported */
  snoopMulticastMacFromIpAddr(&mgmdMsg.mgmdGroupAddr, dmac);
  if (snoopMacAddrCheck(dmac, mcastPacket->cbHandle->family) != L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
    return L7_FAILURE;
  }

  /* Match the destination mac and the mac resloved from ipv4 dest addr */
  if (memcmp(mcastPacket->destMac, dmac, L7_MAC_ADDR_LEN) != 0)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipReportProcess: Invalid Packet");
    return L7_FAILURE;
  }

#if 0
  /* Client information */
  clientId.ptin_intf.intf_type = clientId.ptin_intf.intf_id = 0;
  clientId.outerVlan = mcastPacket->vlanId;
  clientId.innerVlan = mcastPacket->innerVlanId;
  clientId.ipv4_addr = 0;
  memcpy(clientId.macAddr,dmac,sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
  clientId.mask = MCAST_CLIENT_MASK_FIELD_INTF | MCAST_CLIENT_MASK_FIELD_INNERVLAN;
  if (ptin_igmp_clientIndex_get(mcastPacket->intIfNum, mcastPacket->vlanId, &clientId, &client_index)!=L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdMembershipReportProcess: ptin_igmp_clientIndex_get failed");
    return L7_FAILURE;
  }
#else
  if (mcastPacket->client_idx>=PTIN_SYSTEM_IGMP_MAXCLIENTS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdMembershipReportProcess: Client not provided");
    return L7_FAILURE;
  }
#endif

  pSnoopCB->counters.controlFramesProcessed++;

  /* Check to see if the group address is in the 224.0.0.X range.  If it is, don't
     process it... just forward to all ports in the vlan except incoming, otherwise
     it to the table */
  if (inetIsLinkLocalMulticastAddress(&mgmdMsg.mgmdGroupAddr))
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipReportProcess: Packet is in the 224.0.0.x range... don't process it!");
    ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_JOINS_RECEIVED_INVALID);
    return L7_SUCCESS;
  }

#if 0
  /* Does an entry with the same MAC addr and VLAN ID already exist? */
  if ((snoopEntry=snoopEntryFind(dmac, mcastPacket->vlanId, mcastPacket->cbHandle->family, L7_MATCH_EXACT)) == L7_NULLPTR)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipReportProcess: snoop entry does not exist");
    /* Entry does not exist... give back the semaphore and create new entry */
    rc = snoopEntryCreate(dmac, mcastPacket->vlanId, mcastPacket->cbHandle->family,L7_FALSE);
    if (rc != L7_SUCCESS)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipReportProcess: snoopEntryCreate failed. Table full");
      return L7_FAILURE;
    }
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipReportProcess: snoop entry successfully created!");

    if ((snoopEntry=snoopEntryFind(dmac, mcastPacket->vlanId, mcastPacket->cbHandle->family, L7_MATCH_EXACT)) == L7_NULLPTR)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipReportProcess: snoopEntryFind failed.");
      return L7_FAILURE;
    }
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipReportProcess: snoop entry now exists");
  }

  /* Only add interfaces, for dynamic entries */
  if (!snoopEntry->staticGroup)
  {
    /* If the entry already existed or it was just successfully created, add the interface */
    rc = snoopIntfAdd(dmac, mcastPacket->vlanId, mcastPacket->intIfNum,
                      SNOOP_GROUP_MEMBERSHIP, mcastPacket->cbHandle);
    if (rc != L7_SUCCESS)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdMembershipReportProcess: snoopIntfAdd failed");
      return L7_FAILURE;
    }

    /* Check if channel exists... mark to forward join to network */
    if (!snoopChannelExist(snoopEntry,&mgmdMsg.mgmdGroupAddr))
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipReportProcess: Message will be sent to network");
      fwdFlag = L7_TRUE;
    }
    else
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdMembershipReportProcess: channel already exists");
    }

    rc = snoopChannelIntfAdd(snoopEntry,mcastPacket->intIfNum,&mgmdMsg.mgmdGroupAddr);
    if (rc != L7_SUCCESS)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdMembershipReportProcess: snoopChannelIntfAdd failed");
      return L7_FAILURE;
    }
  }
  /* If is a static group, check if this interface is part of the group */
  else
  {
    if (!snoopEntry->port_list[mcastPacket->intIfNum].active)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdMembershipReportProcess: This interface is not part of the static group");
      return L7_FAILURE;
    }
    /* Check if channel exists... It should exist for static entries */
    if (!snoopChannelExist(snoopEntry,&mgmdMsg.mgmdGroupAddr))
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipReportProcess: Static channel does not exist");
      return L7_FAILURE;
    }
  }

  /* Add client */
  if (snoopChannelClientAdd(snoopEntry,mcastPacket->intIfNum,&mgmdMsg.mgmdGroupAddr,client_index)!=L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdMembershipReportProcess: snoopChannelClientAdd failed");
    return L7_FAILURE;
  }
  SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdMembershipReportProcess: client added");
#endif

  ptin_timer_stop(1);

  ptin_timer_start(2,"snoopMgmdMembershipReportProcess-snoop_client_add_procedure");
  /* Add client */
  if ((rc=snoop_client_add_procedure(dmac, mcastPacket->vlanId,
                                     &mgmdMsg.mgmdGroupAddr,
                                     mcastPacket->client_idx,
                                     mcastPacket->intIfNum,
                                     &fwdFlag))!=L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdMembershipReportProcess: snoop_client_add_procedure failed");
  }
  ptin_timer_stop(2);

#if 0
  /* Only if IP channel is new, we forward the JOIN packet */
  if (fwdFlag)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipReportProcess: Forwarding message to network");
    /* Forward membership report to multicast routers we've detected */
    igmp_packet_respond(mcastPacket);
  }
#endif


  if (rc==L7_SUCCESS)
  {
    /* Restart client timer */
    ptin_igmp_client_timer_start(mcastPacket->intIfNum, mcastPacket->client_idx);
//  ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_JOINS_RECEIVED_SUCCESS);
  }
  else
  {
//  ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_JOINS_RECEIVED_FAILED);
  }

  return rc;
}


L7_uint8 snoopRecordType2IGMPStatField(L7_uint8 recordType,L7_uint8 fieldType)
{

  switch (recordType)
  {
  case L7_IGMP_MODE_IS_INCLUDE:
    switch (fieldType)
    {
    case SNOOP_STAT_FIELD_TX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_TX;   
    case SNOOP_STAT_FIELD_TOTAL_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_TOTAL_RX;   
    case SNOOP_STAT_FIELD_VALID_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_VALID_RX;   
    case SNOOP_STAT_FIELD_INVALID_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_INVALID_RX;   
    case SNOOP_STAT_FIELD_DROPPED_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_DROPPED_RX;   
    default:
      return SNOOP_STAT_FIELD_ALL;
    }

  case L7_IGMP_MODE_IS_EXCLUDE:
    switch (fieldType)
    {
    case SNOOP_STAT_FIELD_TX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_TX;   
    case SNOOP_STAT_FIELD_TOTAL_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_TOTAL_RX;   
    case SNOOP_STAT_FIELD_VALID_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_VALID_RX;   
    case SNOOP_STAT_FIELD_INVALID_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_INVALID_RX;   
    case SNOOP_STAT_FIELD_DROPPED_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_DROPPED_RX;   
    default:
      return SNOOP_STAT_FIELD_ALL;
    }

  case L7_IGMP_ALLOW_NEW_SOURCES:
    switch (fieldType)
    {
    case SNOOP_STAT_FIELD_TX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_TX;   
    case SNOOP_STAT_FIELD_TOTAL_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_TOTAL_RX;   
    case SNOOP_STAT_FIELD_VALID_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_VALID_RX;   
    case SNOOP_STAT_FIELD_INVALID_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_INVALID_RX;   
    case SNOOP_STAT_FIELD_DROPPED_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_DROPPED_RX;   
    default:
      return SNOOP_STAT_FIELD_ALL;
    }

  case L7_IGMP_BLOCK_OLD_SOURCES:
    switch (fieldType)
    {
    case SNOOP_STAT_FIELD_TX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_TX;   
    case SNOOP_STAT_FIELD_TOTAL_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_TOTAL_RX;   
    case SNOOP_STAT_FIELD_VALID_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_VALID_RX;   
    case SNOOP_STAT_FIELD_INVALID_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_INVALID_RX;   
    case SNOOP_STAT_FIELD_DROPPED_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_DROPPED_RX;   
    default:
      return SNOOP_STAT_FIELD_ALL;
    }

  case L7_IGMP_CHANGE_TO_INCLUDE_MODE:
    switch (fieldType)
    {
    case SNOOP_STAT_FIELD_TX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_TX;   
    case SNOOP_STAT_FIELD_TOTAL_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_TOTAL_RX;   
    case SNOOP_STAT_FIELD_VALID_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_VALID_RX;   
    case SNOOP_STAT_FIELD_INVALID_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_INVALID_RX;   
    case SNOOP_STAT_FIELD_DROPPED_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_DROPPED_RX;   
    default:
      return SNOOP_STAT_FIELD_ALL;
    }

  case L7_IGMP_CHANGE_TO_EXCLUDE_MODE:
    switch (fieldType)
    {
    case SNOOP_STAT_FIELD_TX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_TX;   
    case SNOOP_STAT_FIELD_TOTAL_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_TOTAL_RX;   
    case SNOOP_STAT_FIELD_VALID_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_VALID_RX;   
    case SNOOP_STAT_FIELD_INVALID_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_INVALID_RX;   
    case SNOOP_STAT_FIELD_DROPPED_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_DROPPED_RX;   
    default:
      return SNOOP_STAT_FIELD_ALL;
    }
  default:
    return SNOOP_STAT_FIELD_ALL;
  }
}
#if SNOOP_PTIN_IGMPv3_ROUTER

L7_inet_addr_t      sourceList[PTIN_IGMP_DEFAULT_MAX_SOURCES_PER_GROUP_RECORD]={};  
/****************************************************************************
* @purpose Process IGMPv3/MLDv2 Group Membership Report
*
* @param   mcastPacket  @b{(input)} Pointer to data structure to hold
*                                   received packet
*
* @returns L7_SUCCESS
* @returns L7_FAILURE, if invalid group address
*
* @notes none
*
* @end
*
*****************************************************************************/
L7_RC_t snoopMgmdSrcSpecificMembershipReportProcess(mgmdSnoopControlPkt_t
                                                    *mcastPacket)
{
  L7_uchar8               dmac[L7_MAC_ADDR_LEN], *dataPtr, recType, auxDataLen,recordType,recTypeAux;
  L7_RC_t                 rc = L7_SUCCESS;
  snoopPTinL3InfoData_t  *snoopEntry=L7_NULLPTR;  
  L7_uint32               ipv4Addr;
  L7_ushort16             noOfGroups, noOfSources,noOfSourcesTmp,numberOfClients=0;
  L7_inet_addr_t          groupAddr,firstGroupAddr;
  L7_BOOL                 fwdFlag = L7_FALSE, floodFlag = L7_TRUE;
  snoop_cb_t             *pSnoopCB;
  L7_uchar8               ipBuf[16];

  L7_uchar8     *dataPtrTmp;
  snoopPTinProxyInterface_t* interfacePtr;  
  snoopPTinProxyGroup_t* groupPtr;
  snoopPTinProxyGroup_t *firstGroupPtr=L7_NULLPTR;
  L7_uint32 /*internalVlanId=0,*/i,vlanId=mcastPacket->vlanId/*,previousVlanId=mcastPacket->vlanId*/;
  char                debug_buf[IPV6_DISP_ADDR_LEN];

  L7_uint32 noOfRecords=0, dummy=0,totalRecords=0;
  L7_BOOL newEntry=L7_FALSE,flagNewGroup=L7_FALSE,flagAddClient=L7_FALSE,flagRemoveClient=L7_FALSE;  

//L7_uint16 mcastRootVlanId;

  ptin_IgmpProxyCfg_t igmpCfg;
  L7_BOOL   firstGroup=L7_FALSE;

//L7_int16      snoopStatIgmpField=-1;

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Membership Report Message Rec'd");

  /* Set pointer to IGMPv3 Membership Report */
  dataPtr = mcastPacket->ip_payload + MGMD_REPORT_NUM_SOURCES_OFFSET;
  SNOOP_GET_SHORT(noOfGroups, dataPtr);

  /* Data ptr will point to the start if group records */
  LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Number of Group records, %u",noOfGroups);

  /* Get Snoop Control Block */
  pSnoopCB = mcastPacket->cbHandle;
  if (pSnoopCB==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "pSnoopCB==L7_NULLPTR");
    return L7_ERROR;
  }

  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get__snooping_old(&igmpCfg) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations, going to use default values!");
    igmpCfg.host.robustness=PTIN_IGMP_DEFAULT_ROBUSTNESS;
  }


 /*We need to fix this in the near future: since it can be used to prevent DOS attacks*/
  if (noOfGroups>igmpCfg.host.max_records_per_report || noOfGroups==0)
  {
    /* Data ptr will point to the start if group records */
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"Number of Group records higher than max allowed value: %u>%u, dropping the packet",noOfGroups,PTIN_IGMP_MAX_RECORDS_PER_REPORT);
    return L7_FAILURE;
  }

  if (mcastPacket->cbHandle->family == L7_AF_INET)
  {
    /* IGMP */
    inetAddressGet(L7_AF_INET, &mcastPacket->destAddr, &ipv4Addr);
    if (ipv4Addr != L7_IP_IGMPV3_REPORT_ADDR)
    {
      LOG_TRACE(LOG_CTX_PTIN_IGMP, "Invalid destination IPv4 address");
      return L7_FAILURE;
    }

    if (pSnoopCB->snoopCfgData->igmpv3_tos_rtr_alert_check == L7_TRUE)
    {
      if (mcastPacket->tosByte != SNOOP_TOS_VALID_VALUE)
      {
        LOG_TRACE(LOG_CTX_PTIN_IGMP,  "Invalid Packet");
        return L7_FAILURE;
      }

      if (mcastPacket->routerAlert != L7_TRUE)
      {
        LOG_TRACE(LOG_CTX_PTIN_IGMP, "Invalid Packet");
        return L7_FAILURE;
      }
    }
  }
  else
  {
#if 1
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"MLD currently not supported yet, packet silently discarded");
    return L7_SUCCESS;
#else
    L7_uchar8 mldv3ReportAddr[L7_IP6_ADDR_LEN];

    memset(mldv3ReportAddr, 0x00, L7_IP6_ADDR_LEN);
    osapiInetPton(L7_AF_INET6, SNOOP_IP6_MLDV2_REPORT_ADDR, mldv3ReportAddr);

    /* MLD */
    inetAddressGet(L7_AF_INET6, &mcastPacket->destAddr, ipBuf);
    if (memcmp(ipBuf, mldv3ReportAddr, L7_IP6_ADDR_LEN) != 0)
    {
      LOG_WARNING(LOG_CTX_PTIN_IGMP, "Invalid destination IPv6 address");
      return L7_FAILURE;
    }
#endif
  }

  /* Validate the dmac and the destination Ipv4 address */
  memset(dmac, 0x00, sizeof(dmac));
  snoopMulticastMacFromIpAddr(&mcastPacket->destAddr, dmac);
  if (memcmp(mcastPacket->destMac, dmac, L7_MAC_ADDR_LEN) != 0)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "Invalid Packet");
    return L7_FAILURE;
  }

  /* Validate client index */
  if (mcastPacket->client_idx >= PTIN_SYSTEM_IGMP_MAXCLIENTS)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "Client not provided  :%d",mcastPacket->client_idx);
    return L7_FAILURE;
  }

  /* Check if it is a malformed packet */
  if ((dataPtr - mcastPacket->ip_payload) > mcastPacket->ip_payload_length)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"Dropped malformed packet :%d",mcastPacket->cbHandle->family);
    return L7_FAILURE;
  }

  while (noOfGroups > 0)
  {
    /* IGMP Membership records */
    SNOOP_GET_BYTE(recType, dataPtr); /* Record type */ 

    if (recType<=0 || recType>=MGMD_GROUP_REPORT_TYPE_MAX)
    {
      ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_INVALID_RX);
      return L7_SUCCESS;
    }
    recTypeAux=recType;/*Saved to be used later on*/
    recordType=recType;
    ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, snoopRecordType2IGMPStatField(recType,SNOOP_STAT_FIELD_TOTAL_RX));     
    SNOOP_GET_BYTE(auxDataLen, dataPtr); /* AuxData Len */
    SNOOP_GET_SHORT(noOfSources, dataPtr); /* Number of sources */   
    memset(&groupAddr, 0x00, sizeof(groupAddr));
    if (mcastPacket->cbHandle->family == L7_AF_INET)
    {
      SNOOP_GET_ADDR(&ipv4Addr, dataPtr);     
      inetAddressSet(L7_AF_INET, &ipv4Addr, &groupAddr);      
    }
#if 0
    else
    {
      /* IPv6 MCAST Address */
      SNOOP_GET_ADDR6(ipBuf, dataPtr);
      inetAddressSet(L7_AF_INET6, ipBuf, &groupAddr);
    }
#endif

    if (inetIsInMulticast(&groupAddr) == L7_TRUE)
    {
      memset(dmac, 0x00, sizeof(dmac));
      /* Validate the group address being reported */
      snoopMulticastMacFromIpAddr(&groupAddr, dmac);
      if (snoopMacAddrCheck(dmac, mcastPacket->cbHandle->family) != L7_SUCCESS)
      {
        ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, snoopRecordType2IGMPStatField(recType,SNOOP_STAT_FIELD_INVALID_RX)); 
        LOG_WARNING(LOG_CTX_PTIN_IGMP,"Invalid destination multicast mac");
        return L7_FAILURE;
      }
      fwdFlag = inetIsLinkLocalMulticastAddress(&groupAddr);
    }
    else
    {
      fwdFlag = L7_TRUE;
    }

    LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Group Record Rec'd (recordType=%u, numberOfSources=%u, groupAddr=%s, fwdFlag=%u)",recType,noOfSources,inetAddrPrint(&groupAddr, debug_buf),fwdFlag);

    if (fwdFlag == L7_FALSE)
    {
#if 0 //Fixme       
//The sourceAddr field is currently not used by ptin_igmp_McastRootVlan_get()
      L7_inet_addr_t sourceAddr;
      sourceAddr.addr.ipv4.s_addr=0;
      sourceAddr.family = L7_AF_INET;         

      if (ptin_igmp_McastRootVlan_get((L7_inet_addr_t *) &groupAddr, (L7_inet_addr_t *) &sourceAddr, internalVlanId, &mcastRootVlanId) !=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP,
                "snoopPacketHandle: Can't get McastRootVlan for vlan=%u (grpAddr=%s srcAddr=%s)",
                mcastPacket->vlanId, inetAddrPrint(&groupAddr,debug_buf) , inetAddrPrint(&sourceAddr,debug_buf));
        return L7_FAILURE;
      }
      vlanId=(L7_uint32) mcastRootVlanId;
#endif        


      /*If we receive a Block{} or an Allow{} without sources we silently discard the packet*/
      if (noOfSources==0 && (recType==L7_IGMP_ALLOW_NEW_SOURCES || recType==L7_IGMP_BLOCK_OLD_SOURCES))
      {
        ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, snoopRecordType2IGMPStatField(recType,SNOOP_STAT_FIELD_INVALID_RX));
        LOG_NOTICE(LOG_CTX_PTIN_IGMP,"Group Record Type %u without any source address, packet silently discarded",recType);
        return L7_SUCCESS;
      }

      /*If number of sources per record is higher than PTIN_IGMP_DEFAULT_MAX_SOURCES_PER_GROUP_RECORD we do not processs, instead we consider only the group address and the record-type*/
      if (noOfSources>PTIN_IGMP_DEFAULT_MAX_SOURCES_PER_GROUP_RECORD /*|| noOfSources+snoopEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].numberOfSources>PTIN_IGMP_DEFAULT_MAX_SOURCES_PER_GROUP_RECORD*/)
      {
        if (recType==L7_IGMP_ALLOW_NEW_SOURCES || recType==L7_IGMP_MODE_IS_INCLUDE || recType==L7_IGMP_CHANGE_TO_INCLUDE_MODE)
        {
          recordType=L7_IGMP_CHANGE_TO_EXCLUDE_MODE;
          recTypeAux=L7_IGMP_CHANGE_TO_EXCLUDE_MODE;
        }
        /*This condition is not tottaly correct for IS_EX & TO_EX, if the filter mode is already on exclude mode for that specific client*/
        else if (recType==L7_IGMP_BLOCK_OLD_SOURCES || recType==L7_IGMP_MODE_IS_EXCLUDE || recType==L7_IGMP_CHANGE_TO_EXCLUDE_MODE)
        {
          recordType=L7_IGMP_CHANGE_TO_INCLUDE_MODE;
          recTypeAux=L7_IGMP_CHANGE_TO_INCLUDE_MODE;
        }
        noOfSources=0;                        

        LOG_WARNING(LOG_CTX_PTIN_IGMP, "Group Record (groupAddr:%s recordType:%u) has more than 64 sources (noofSources: %u), changing the record type to :%u without with zero sources",inetAddrPrint(&groupAddr, debug_buf),recTypeAux,noOfSources,recType);          
      }

      /*Since we add the group record before processing the packet, we need to have some logic to determine the record type*/
      else if (recType==L7_IGMP_MODE_IS_EXCLUDE || 
               (recType==L7_IGMP_CHANGE_TO_EXCLUDE_MODE /*&& snoopEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].filtermode==PTIN_SNOOP_FILTERMODE_EXCLUDE*/))
      {
        recordType=L7_IGMP_CHANGE_TO_EXCLUDE_MODE;
        recTypeAux=L7_IGMP_CHANGE_TO_EXCLUDE_MODE;

        noOfSources=0;                                          
      }
      else if (recType==L7_IGMP_MODE_IS_INCLUDE)
      {
        recordType=L7_IGMP_ALLOW_NEW_SOURCES;
        recTypeAux=L7_IGMP_ALLOW_NEW_SOURCES;   
      }
      else
        recordType=recType;

      if (mcastPacket->cbHandle->family == L7_AF_INET)
      {
        /* Add new source */
        for (i=0;i<noOfSources;i++)
        {
          memset(&sourceList[i], 0x00, sizeof(L7_inet_addr_t));             
          {
            SNOOP_GET_ADDR(&ipv4Addr, dataPtr);
            inetAddressSet(L7_AF_INET, &ipv4Addr, &sourceList[i]);
            if (inetIpAddressValidityCheck(L7_AF_INET,&sourceList[i])!=L7_SUCCESS)
            {
              ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, snoopRecordType2IGMPStatField(recType,SNOOP_STAT_FIELD_INVALID_RX)); 
              LOG_TRACE(LOG_CTX_PTIN_IGMP, "Invalid Source IP Address %s",inetAddrPrint(&sourceList[i], debug_buf));
              return L7_FAILURE;
            }
          }
        }
      }
      else
      {
        /* Add new source */
        for (i=0;i<noOfSources;i++)
        {
          memset(&sourceList[i], 0x00, sizeof(L7_inet_addr_t));
          /* IPv6 MCAST Address */
          SNOOP_GET_ADDR6(ipBuf, dataPtr);
          inetAddressSet(L7_AF_INET6, ipBuf, &sourceList[i]);

          if (inetIpAddressValidityCheck(L7_AF_INET6,&sourceList[i])!=L7_SUCCESS)
          {
            ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, snoopRecordType2IGMPStatField(recType,SNOOP_STAT_FIELD_INVALID_RX)); 
            LOG_TRACE(LOG_CTX_PTIN_IGMP, "Invalid Source IP Address %s",inetAddrPrint(&sourceList[i], debug_buf));
            return L7_FAILURE;
          }
        }
      }       

      /* Create new entry in AVL tree for VLAN+IP if necessary */
      if (L7_NULLPTR == (snoopEntry = snoopPTinL3EntryFind(vlanId, &groupAddr, L7_MATCH_EXACT)))
      {
        if (L7_SUCCESS != snoopPTinL3EntryAdd(vlanId,&groupAddr))
        {
          ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, snoopRecordType2IGMPStatField(recType,SNOOP_STAT_FIELD_DROPPED_RX)); 
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to Add L3 Entry");
          return L7_ERROR;
        }
        else
        {
          flagNewGroup=L7_TRUE;
          LOG_TRACE(LOG_CTX_PTIN_IGMP, "snoopPTinL3EntryAdd(%04X,%u)",groupAddr.addr.ipv4.s_addr,mcastPacket->vlanId);
        }
        if (L7_NULLPTR == (snoopEntry = snoopPTinL3EntryFind(vlanId, &groupAddr, L7_MATCH_EXACT)))
        {
          ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, snoopRecordType2IGMPStatField(recType,SNOOP_STAT_FIELD_DROPPED_RX)); 
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to Add&Find L3 Entry");
          return L7_ERROR;
        }
      }
      else
      {
        numberOfClients=snoopEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].numberOfClients;

      }

      /* If Leaf interface is not used, initialize it */
      if (snoopEntry->interfaces[mcastPacket->intIfNum].active == L7_FALSE)
      {
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Initializing leaf interface idx: %u", mcastPacket->intIfNum);
        snoopPTinInitializeInterface(&snoopEntry->interfaces[mcastPacket->intIfNum],vlanId,&groupAddr,mcastPacket->intIfNum);            
      }
      noOfRecords=0;
      /* If root interface is not used, initialize it */
      if (snoopEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].active == L7_FALSE)
      {
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Initializing root interface idx: %u", SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM);
        snoopPTinInitializeInterface(&snoopEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM],vlanId,&groupAddr,SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM);
      }

      if ( (interfacePtr=snoopPTinProxyInterfaceAdd(vlanId)) ==L7_NULLPTR)
      {
        ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, snoopRecordType2IGMPStatField(recType,SNOOP_STAT_FIELD_DROPPED_RX));          
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinProxyInterfaceAdd()");
        return L7_ERROR;
      }

      if ((groupPtr=snoopPTinGroupRecordAdd(interfacePtr,recordType,&snoopEntry->snoopPTinL3InfoDataKey.mcastGroupAddr,&newEntry,igmpCfg.host.robustness ))==L7_NULLPTR)
      {
        ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, snoopRecordType2IGMPStatField(recType,SNOOP_STAT_FIELD_DROPPED_RX)); 
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordGroupAdd()");
        return L7_ERROR;
      }

      //Saving Group Record Ptr          
      if (noOfSources==0 && firstGroup==L7_FALSE)
      {
        firstGroupPtr=groupPtr;
      }

      if (mcastPacket->intIfNum==SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM)
      {
        ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, snoopRecordType2IGMPStatField(recType,SNOOP_STAT_FIELD_DROPPED_RX)); 
        LOG_ERR(LOG_CTX_PTIN_IGMP, "mcastPacket->intIfNum=0");
        return L7_ERROR;
      }

      floodFlag = L7_FALSE;        

      dataPtrTmp=dataPtr;     
      noOfSourcesTmp=noOfSources;            

      switch (recTypeAux)
      {
      case L7_IGMP_MODE_IS_INCLUDE:
        {           
          LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Going to process MRM for Leaf Interface");
          rc=snoopPTinMembershipReportIsIncludeProcess(snoopEntry, mcastPacket->intIfNum, mcastPacket->client_idx, noOfSourcesTmp, sourceList,&dummy, groupPtr);          
          if (rc!=L7_SUCCESS)
          {
            LOG_ERR(LOG_CTX_PTIN_IGMP, "Leaf Interface: snoopPTinMembershipReportIsIncludeProcess()");              
//          return rc;
          }
          LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Going to process MRM for Root Interface");
          rc=snoopPTinMembershipReportIsIncludeProcess(snoopEntry, SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM, mcastPacket->intIfNum, noOfSources, sourceList,&noOfRecords, groupPtr);
          if (rc!=L7_SUCCESS)
          {
            LOG_ERR(LOG_CTX_PTIN_IGMP, "Root Interface: snoopPTinMembershipReportIsIncludeProcess()");              
//          return rc;
          }
          break;
        }
      case L7_IGMP_MODE_IS_EXCLUDE:
        {            
          LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Going to process MRM for Leaf Interface");
          rc=snoopPTinMembershipReportIsExcludeProcess(snoopEntry, mcastPacket->intIfNum, mcastPacket->client_idx, noOfSourcesTmp, sourceList,&dummy, groupPtr);          
          if (rc!=L7_SUCCESS)
          {
            LOG_ERR(LOG_CTX_PTIN_IGMP, "Leaf: snoopPTinMembershipReportIsExcludeProcess()");              
//          return rc;
          }
          LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Going to process MRM for Root Interface");
          rc=snoopPTinMembershipReportIsExcludeProcess(snoopEntry, SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM, mcastPacket->intIfNum, noOfSources, sourceList,&noOfRecords, groupPtr);
          if (rc!=L7_SUCCESS)
          {
            LOG_ERR(LOG_CTX_PTIN_IGMP, "Root: snoopPTinMembershipReportIsExcludeProcess()");              
//          return rc;
          }
          break;
        }
      case L7_IGMP_CHANGE_TO_INCLUDE_MODE:
        {            
          LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Going to process MRM for Leaf Interface");
          rc=snoopPTinMembershipReportToIncludeProcess(snoopEntry, mcastPacket->intIfNum, mcastPacket->client_idx, noOfSourcesTmp, sourceList,&dummy, groupPtr);          
          if (rc!=L7_SUCCESS)
          {
            LOG_ERR(LOG_CTX_PTIN_IGMP, "Leaf: snoopPTinMembershipReportIsIncludeProcess()");              
//          return rc;
          }
          LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Going to process MRM for Root Interface");
          rc=snoopPTinMembershipReportToIncludeProcess(snoopEntry, SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM, mcastPacket->intIfNum, noOfSources, sourceList,&noOfRecords, groupPtr);
          if (rc!=L7_SUCCESS)
          {
            LOG_ERR(LOG_CTX_PTIN_IGMP, "Root: snoopPTinMembershipReportIsIncludeProcess()");              
//          return rc;
          }
          break;
        }
      case L7_IGMP_CHANGE_TO_EXCLUDE_MODE:
        {            
          LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Going to process MRM for Leaf Interface");
          rc=snoopPTinMembershipReportToExcludeProcess(snoopEntry, mcastPacket->intIfNum, mcastPacket->client_idx, noOfSourcesTmp, sourceList,&dummy, groupPtr);          
          if (rc!=L7_SUCCESS)
          {
            LOG_ERR(LOG_CTX_PTIN_IGMP, "Leaf: snoopPTinMembershipReportToExcludeProcess()");              
//          return rc;
          }
          LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Going to process MRM for Root Interface");
          rc=snoopPTinMembershipReportToExcludeProcess(snoopEntry, SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM, mcastPacket->intIfNum, noOfSources, sourceList,&noOfRecords, groupPtr);
          if (rc!=L7_SUCCESS)
          {
            LOG_ERR(LOG_CTX_PTIN_IGMP, "Root: snoopPTinMembershipReportToExcludeProcess()");              
//          return rc;
          }
          break;
        }
      case L7_IGMP_ALLOW_NEW_SOURCES:
        {            
          LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Going to process MRM for Leaf Interface");
          rc=snoopPTinMembershipReportAllowProcess(snoopEntry, mcastPacket->intIfNum, mcastPacket->client_idx, noOfSourcesTmp, sourceList,&dummy, groupPtr);          
          if (rc!=L7_SUCCESS)
          {
            LOG_ERR(LOG_CTX_PTIN_IGMP, "Leaf: snoopPTinMembershipReportAllowProcess()");              
//          return rc;
          }
          LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Going to process MRM for Root Interface");            
          rc=snoopPTinMembershipReportAllowProcess(snoopEntry, SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM, mcastPacket->intIfNum, noOfSources, sourceList,&noOfRecords, groupPtr);
          if (rc!=L7_SUCCESS)
          {
            LOG_ERR(LOG_CTX_PTIN_IGMP, "Root: snoopPTinMembershipReportAllowProcess()");              
//          return rc;
          }
          break;
        }
      case L7_IGMP_BLOCK_OLD_SOURCES:
        {            
          LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Going to process MRM for Leaf Interface");                         
          rc=snoopPTinMembershipReportBlockProcess(snoopEntry, mcastPacket->intIfNum, mcastPacket->client_idx, noOfSourcesTmp, sourceList,&dummy, groupPtr);          
          if (rc!=L7_SUCCESS)
          {
            LOG_ERR(LOG_CTX_PTIN_IGMP, "Leaf: snoopPTinMembershipReportBlockProcess()");
//          return rc;
          }
          LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Going to process MRM for Root Interface");
          rc=snoopPTinMembershipReportBlockProcess(snoopEntry, SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM, mcastPacket->intIfNum, noOfSources, sourceList,&noOfRecords, groupPtr);
          if (rc!=L7_SUCCESS)
          {
            LOG_ERR(LOG_CTX_PTIN_IGMP, "Root: snoopPTinMembershipReportBlockProcess()");              
//          return rc;
          }
          break;
        }
      default:
        {
          LOG_WARNING(LOG_CTX_PTIN_IGMP, "Unknown record type. Silently ignored");        
        }
      }

      if (rc!=L7_SUCCESS)
      {
        ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, snoopRecordType2IGMPStatField(recType,SNOOP_STAT_FIELD_DROPPED_RX));         
        if (snoopPTinGroupRecordRemove(interfacePtr,&snoopEntry->snoopPTinL3InfoDataKey.mcastGroupAddr,recordType)!=L7_SUCCESS)
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordRemove()");
          return L7_ERROR;
        }
        return rc;
      }
      else
        ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, snoopRecordType2IGMPStatField(recType,SNOOP_STAT_FIELD_VALID_RX)); 


      if (noOfRecords>0)
      {
        //Saving First GroupAddr and GroupPtr
        if (firstGroup==L7_FALSE)
        {
          firstGroupAddr=groupAddr;
          firstGroupPtr=groupPtr;
          firstGroup=L7_TRUE;      
        }
        totalRecords=totalRecords+noOfRecords;    

#if 0 
        if (groupPtr->numberOfSources>PTIN_IGMP_DEFAULT_MAX_SOURCES_PER_GROUP_RECORD)
        {
          if (snoopPTinGroupRecordRemove(interfacePtr, &snoopEntry->snoopPTinL3InfoDataKey.mcastGroupAddr,recType)!=L7_SUCCESS)
          {
            LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordRemove()");
            return L7_ERROR;
          }
          if (recordType==L7_IGMP_ALLOW_NEW_SOURCES)
          {
            if ((groupPtr=snoopPTinGroupRecordAdd(interfacePtr,L7_IGMP_CHANGE_TO_EXCLUDE_MODE,&snoopEntry->snoopPTinL3InfoDataKey.mcastGroupAddr,&newEntry,igmpCfg.host.robustness ))==L7_NULLPTR)
            {
              ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, snoopRecordType2IGMPStatField(recTypeAux,SNOOP_STAT_FIELD_DROPPED_RX)); 
              LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordGroupAdd()");
              return L7_ERROR;
            }
          }
#if 0
          else /*if (recordType==L7_IGMP_BLOCK_OLD_SOURCES) */
          {
            if ((groupPtr=snoopPTinGroupRecordAdd(interfacePtr,L7_IGMP_CHANGE_TO_INCLUDE_MODE,&snoopEntry->snoopPTinL3InfoDataKey.mcastGroupAddr,&newEntry,igmpCfg.host.robustness ))==L7_NULLPTR)
            {
              ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, snoopRecordType2IGMPStatField(recTypeAux,SNOOP_STAT_FIELD_DROPPED_RX)); 
              LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordGroupAdd()");
              return L7_ERROR;
            }
          }
#endif
          noOfRecords=0;
        }
#endif
      }
      else if (newEntry==L7_TRUE)
      {
        if (snoopPTinGroupRecordRemove(interfacePtr, &snoopEntry->snoopPTinL3InfoDataKey.mcastGroupAddr,recordType)!=L7_SUCCESS)
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopPTinGroupRecordRemove()");
          return L7_ERROR;
        }
      }

#if 0
      //We need to check if this group as the same vlanId as the previous group. If not we schedule a new Report
      if (previousVlanId!=vlanId)
      {
        previousVlanId=vlanId;
        firstGroup=L7_FALSE;

        if (noOfRecords>0)
        {
          LOG_TRACE(LOG_CTX_PTIN_IGMP, "Schedule Membership Report Message");
          if (snoopPTinReportSchedule(vlanId,&firstGroupAddr,SNOOP_PTIN_MEMBERSHIP_REPORT,0,L7_FALSE,totalRecords, firstGroupPtr)!=L7_SUCCESS)
          {
            LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed snoopPTinReportSchedule()");
            return L7_FAILURE;
          }
          totalRecords=0;
        }
      }
#endif

#if 0
      LOG_NOTICE(LOG_CTX_PTIN_IGMP,"Going to open this L2 Port (interfaceIdx:%u vlanId:%u groupAddr:%s)",mcastPacket->intIfNum,mcastPacket->vlanId,inetAddrPrint(&groupAddr, debug_buf));
      /*Open L2 Port on Switch*/
      if (snoopGroupIntfAdd(vlanId,&groupAddr, mcastPacket->intIfNum)!=L7_SUCCESS)
      {
        ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, snoopRecordType2IGMPStatField(recTypeAux,SNOOP_STAT_FIELD_DROPPED_RX)); 
        LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to snoopGroupIntfAdd()");
        return L7_ERROR;
      }
#endif      
    } /* end of if process packet */

    /* Point to the next record */
    /* RFC 3376 4.2.6, RFC 3810 5.2.6 */
    if (mcastPacket->cbHandle->family == L7_AF_INET)
    {
      dataPtr += (auxDataLen * 4);
    }
    else
    {
      dataPtr += (auxDataLen * 4);
    }    

    if (flagNewGroup==L7_TRUE && snoopEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].numberOfClients>0)
    {
      ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx,SNOOP_STAT_FIELD_ACTIVE_GROUPS);
      flagNewGroup=L7_FALSE;
      if (flagAddClient==L7_FALSE)
        flagAddClient=L7_TRUE;
    }
    else if (snoopEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].numberOfClients==0)
    {
      ptin_igmp_stat_decrement_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx,SNOOP_STAT_FIELD_ACTIVE_GROUPS);
    }
    else if (numberOfClients<snoopEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].numberOfClients)
    {
      if (flagAddClient==L7_FALSE)
        flagAddClient=L7_TRUE;
    }
    else if (numberOfClients>snoopEntry->interfaces[SNOOP_PTIN_PROXY_ROOT_INTERFACE_NUM].numberOfClients)
    {
      if (flagRemoveClient==L7_FALSE)
        flagRemoveClient=L7_TRUE;
    }

    noOfGroups -= 1;
  } /* end of while loop */

  if (totalRecords>0)
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP, "Schedule Membership Report Message");
    if (snoopPTinScheduleReportMessage(vlanId,&firstGroupAddr,L7_IGMP_V3_MEMBERSHIP_REPORT,0,L7_FALSE,totalRecords, firstGroupPtr,igmpCfg.host.robustness)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed snoopPTinReportSchedule()");
      return L7_ERROR;
    }
  }

#if 0
  /* Forward membership report to multicast routers we've detected */
  if (floodFlag == L7_FALSE)
  {
    snoopPacketRtrIntfsForward(mcastPacket, L7_IGMP_V3_MEMBERSHIP_REPORT);
  }
  else
  {
    /* Flood it in the VLAN as it is for link local multicast address */
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Flooding packet...");
    snoopPacketFlood(mcastPacket);
  }
#endif

  if (flagAddClient!=flagRemoveClient)
  {
    if (flagAddClient==L7_TRUE)
      ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx,SNOOP_STAT_FIELD_ACTIVE_CLIENTS);
    else
      ptin_igmp_stat_decrement_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx,SNOOP_STAT_FIELD_ACTIVE_CLIENTS);
  }

  pSnoopCB->counters.controlFramesProcessed++; 

  return rc;
}
#endif

/*********************************************************************
* @purpose Process IGMPv2 Leave/ MLDv1 Done Group message
*
* @param   mcastPacket  @b{(input)} Pointer to data structure to hold
*                                   received packet
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t snoopMgmdLeaveGroupProcess(mgmdSnoopControlPkt_t *mcastPacket)
{

  L7_uchar8        dmac[L7_MAC_ADDR_LEN], *dataPtr, byteVal;
  L7_uchar8        ipBuf[L7_IP6_ADDR_LEN];
  //L7_RC_t          rc = L7_SUCCESS;
  L7_uint32        ipv4Addr;
  //L7_BOOL  fastLeaveMode;
  //L7_uint32        timerValue, intfFastLeaveMode;
  snoop_cb_t      *pSnoopCB = L7_NULLPTR;
  L7_mgmdMsg_t     mgmdMsg;
  L7_in6_addr_t    ipv6Addr;
  L7_BOOL          fwdFlag = L7_FALSE;
#if 0
  snoopInfoData_t *snoopEntry;
  ptin_McastClient_id_t clientId;
  L7_uint               client_index;
#endif

  SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdLeaveGroupProcess");

  /* Interface must be client */
  if (ptin_igmp_clientIntfVlan_validate(mcastPacket->intIfNum, mcastPacket->vlanId)!=L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdLeaveGroupProcess: This is not a client interface (intIfNum=%u)!",mcastPacket->intIfNum);
    return L7_FAILURE;
  }

  /* Get Snoop Control Block */
  pSnoopCB = mcastPacket->cbHandle;

  /* Set pointer to IGMP message */
  dataPtr = mcastPacket->ip_payload;

  if (mcastPacket->cbHandle->family == L7_AF_INET) /* IGMP Message */
  {
    memset(&mgmdMsg, 0x00, sizeof(L7_mgmdMsg_t));
    SNOOP_GET_BYTE(mgmdMsg.mgmdType, dataPtr);         /* Version/Type */
    SNOOP_GET_BYTE(mgmdMsg.igmpMaxRespTime, dataPtr);  /* Max response time */
    SNOOP_GET_SHORT(mgmdMsg.mgmdChecksum, dataPtr);    /* Checksum */
    SNOOP_GET_ADDR(&ipv4Addr, dataPtr);    /* Group Address */
    SNOOP_UNUSED_PARAM(dataPtr);
    inetAddressSet(L7_AF_INET, &ipv4Addr, &mgmdMsg.mgmdGroupAddr);

    if (pSnoopCB->snoopCfgData->igmpv3_tos_rtr_alert_check == L7_TRUE)
    {
      if (mcastPacket->routerAlert != L7_TRUE)
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdLeaveGroupProcess: Invalid Packet");
        return L7_FAILURE;
      }
    }
  }
  else /* MLD Message */
  {
    memset(&mgmdMsg, 0x00, sizeof(L7_mgmdMsg_t));
    SNOOP_GET_BYTE(mgmdMsg.mgmdType, dataPtr);         /* Version/Type */
    SNOOP_GET_BYTE(byteVal, dataPtr);                  /* Code */
    SNOOP_UNUSED_PARAM(byteVal);
    SNOOP_GET_SHORT(mgmdMsg.mgmdChecksum, dataPtr);    /* Checksum */
    SNOOP_GET_SHORT(mgmdMsg.mldMaxRespTime, dataPtr);  /* Max response time */
    SNOOP_GET_SHORT(mgmdMsg.mgmdReserved, dataPtr);

    SNOOP_GET_ADDR6(&ipv6Addr, dataPtr); /* Group Address */
    inetAddressSet(L7_AF_INET6, &ipv6Addr, &mgmdMsg.mgmdGroupAddr);
  }

  if (pSnoopCB->family == L7_AF_INET)
  {
    /* Check if IPv4 destination address is same as 224.0.0.2 */
    inetAddressGet(mcastPacket->cbHandle->family, &mcastPacket->destAddr,
                   &ipv4Addr);
    if (ipv4Addr != L7_IP_ALL_RTRS_ADDR)
    {
      if (L7_INET_ADDR_COMPARE(&mcastPacket->destAddr, &mgmdMsg.mgmdGroupAddr) != 0)
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdLeaveGroupProcess: Invalid Packet");
        return L7_FAILURE;
      }
    }
  }
  else
  {
    L7_uchar8 mldv3ReportAddr[L7_IP6_ADDR_LEN];

    memset(mldv3ReportAddr, 0x00, L7_IP6_ADDR_LEN);
    osapiInetPton(L7_AF_INET6, SNOOP_IP6_ALL_ROUTERS_ADDR, mldv3ReportAddr);

    /* MLD */
    inetAddressGet(L7_AF_INET6, &mcastPacket->destAddr, ipBuf);
    if (memcmp(ipBuf, mldv3ReportAddr, L7_IP6_ADDR_LEN) != 0)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET6, "snoopMgmdLeaveGroupProcess: Invalid destination IPv6 address");
      return L7_FAILURE;
    }
  }

  /* check if it is a leave for a valid mcast group address */
  if (inetIsInMulticast(&mgmdMsg.mgmdGroupAddr) == L7_FALSE)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdLeaveGroupProcess: Invalid Packet");
    return L7_FAILURE;
  }

  /* check if frame dmac is same as IP Dest Addr equivalent dmac */
  memset(dmac, 0x00, sizeof(dmac));
  snoopMulticastMacFromIpAddr(&mcastPacket->destAddr, dmac);
  if (memcmp(mcastPacket->destMac, dmac, L7_MAC_ADDR_LEN) != 0)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdLeaveGroupProcess: Invalid Packet");
    return L7_FAILURE;
  }

  memset(dmac, 0x00, sizeof(dmac));
  /* Validate the group address being reported */
  snoopMulticastMacFromIpAddr(&mgmdMsg.mgmdGroupAddr, dmac);
  if (snoopMacAddrCheck(dmac, mcastPacket->cbHandle->family) != L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdLeaveGroupProcess: Invalid Packet");
    return L7_FAILURE;
  }

  if (inetIsLinkLocalMulticastAddress(&mgmdMsg.mgmdGroupAddr))
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,
                "snoopMgmdLeaveGroupProcess: Packet is in the 224.0.0.x range... don't process it!");
    return L7_SUCCESS;
  }

#if 0
  /* Client information */
  clientId.ptin_intf.intf_type = clientId.ptin_intf.intf_id = 0;
  clientId.outerVlan = mcastPacket->vlanId;
  clientId.innerVlan = mcastPacket->innerVlanId;
  clientId.ipv4_addr = 0;
  memcpy(clientId.macAddr,dmac,sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
  clientId.mask = MCAST_CLIENT_MASK_FIELD_INTF | MCAST_CLIENT_MASK_FIELD_INNERVLAN;
  if (ptin_igmp_clientIndex_get(mcastPacket->intIfNum, mcastPacket->vlanId, &clientId, &client_index)!=L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,
                "snoopMgmdLeaveGroupProcess: ptin_igmp_clientIndex_get failed");
    return L7_FAILURE;
  }
#else
  if (mcastPacket->client_idx>=PTIN_SYSTEM_IGMP_MAXCLIENTS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"Client not provided");
    return L7_FAILURE;
  }
#endif

  /* Update stats */
  pSnoopCB->counters.controlFramesProcessed++;

#if 0 //This is performed by the snoopPacketProcess() routine.
  ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_LEAVES_RECEIVED);
#endif

#if 0
  /* Does an entry with the same MAC addr and VLAN ID already exist? */
  snoopEntry = snoopEntryFind(dmac, mcastPacket->vlanId,
                              mcastPacket->cbHandle->family, L7_MATCH_EXACT);
  if ( snoopEntry == L7_NULLPTR )
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,
                "snoopMgmdLeaveGroupProcess: snoopEntryFind failed!");
    return L7_FAILURE;
  }

  /* If interface is not active for this group, do nothing */
  if (!snoopEntry->port_list[mcastPacket->intIfNum].active)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,
                "snoopMgmdLeaveGroupProcess: Interface %u is not active.",mcastPacket->intIfNum);
    return L7_FAILURE;
  }

  /* Verify if this channel exists */
  if (!snoopChannelExist(snoopEntry,&mgmdMsg.mgmdGroupAddr))
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,
                "snoopMgmdLeaveGroupProcess: This channel does not exist");
    return L7_SUCCESS;
  }

  /* Is this client consuming this channel? If not, there is nothing to be done */
  if (!snoopChannelClientExists(snoopEntry,&mgmdMsg.mgmdGroupAddr,client_index))
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdLeaveGroupProcess: This client does not exist!");
    return L7_SUCCESS;
  }

  /* Remove client */
  if (snoopChannelClientRemove(snoopEntry,mcastPacket->intIfNum,&mgmdMsg.mgmdGroupAddr,client_index)!=L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,
                "snoopMgmdLeaveGroupProcess: snoopChannelClientRemove failed");
    return L7_FAILURE;
  }
  SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdLeaveGroupProcess: client removed");

  /* Only send leave and remove interface, for dynamic entries */
  if (!snoopEntry->staticGroup)
  {
    /* Procedures when no one is watching a channel in a particular interface... */
    /* Is there no clients for this channel+interface? */
    if (snoopChannelClientsIntfNone(snoopEntry,mcastPacket->intIfNum,&mgmdMsg.mgmdGroupAddr))
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdLeaveGroupProcess: No clients for this channel and interface");
      /* Remove interface from this channel */
      if (snoopChannelIntfRemove(snoopEntry,mcastPacket->intIfNum,&mgmdMsg.mgmdGroupAddr)!=L7_SUCCESS)
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,
                    "snoopMgmdLeaveGroupProcess: snoopChannelIntfRemove failed");
        return L7_FAILURE;
      }
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdLeaveGroupProcess: interface removed for this interface");

      /* If channel was removed, send LEAVE to network */
      if (!snoopChannelExist(snoopEntry,&mgmdMsg.mgmdGroupAddr))
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdLeaveGroupProcess: Message will be forward to network");
        fwdFlag = L7_TRUE;
      }

      /* If there is no channels for this interface, remove interface from group */
      if (snoopChannelsIntfNone(snoopEntry,mcastPacket->intIfNum))
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdLeaveGroupProcess: No channels for this interface");
        if (snoopIntfRemove(dmac, mcastPacket->vlanId, mcastPacket->intIfNum,
                            SNOOP_GROUP_MEMBERSHIP, mcastPacket->cbHandle) != L7_SUCCESS)
        {
          SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,
                      "snoopMgmdLeaveGroupProcess: Failed to remove group membership");
          return L7_FAILURE;
        }
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdLeaveGroupProcess: interface was removed from group");
      }
    }
  }
#endif

  /* Add client */
  if (snoop_client_remove_procedure(dmac, mcastPacket->vlanId,
                                    &mgmdMsg.mgmdGroupAddr,
                                    mcastPacket->client_idx,
                                    mcastPacket->intIfNum,
                                    &fwdFlag)!=L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdLeaveGroupProcess: snoop_client_remove_procedure failed");
    return L7_FAILURE;
  }

#if 0
  // Only if IP channel was removed, we forward the LEAVE packet */
  if (fwdFlag)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdLeaveGroupProcess: Forwarding message to network");
    igmp_packet_respond(mcastPacket);
  }
#endif

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose Process IGMP PIMv1 / DVMRP message
*
* @param   mcastPacket  @b{(input)} Pointer to data structure to hold
*                                   received packet
* @param   grpAddr      @b{(input)} Multicast Group address
* @param   ipProtType   @b{(input)} IP Protocol payload type
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   PIMv1,DVMRP control messages are of type IGMP.  So, process them
*          by adding the incoming interface to the list of multicast
*          routers and then forward the frame to interfaces with
*          multicast routers attached. We do this to identify the
*          multicast routers currently in non-querier state. For PIMv2
*          packets forwarding is done in hardware, hence only snoop them.
*
* @end
*
*********************************************************************/
L7_RC_t snoopPimv1v2DvmrpProcess(mgmdSnoopControlPkt_t *mcastPacket,
                                 L7_inet_addr_t *grpAddr,
                                 L7_uint32 ipProtType)
{
  snoop_cb_t *pSnoopCB = L7_NULLPTR;

  /* Verify this frame was sent to the the "all routers" address */

  pSnoopCB = mcastPacket->cbHandle;

  if (L7_INET_ADDR_COMPARE(&mcastPacket->destAddr, grpAddr) == 0)
  {
    /* PTin removed: IGMP snooping */
#if 0
    /* Add this interface to mcast router list and to all snooping entries */
    snoopMcastRtrListAdd(mcastPacket->vlanId, mcastPacket->intIfNum,
                         SNOOP_DYNAMIC, mcastPacket->cbHandle);
#endif
  }

  /* Forward on all interfaces on this VLAN, except the incoming interface only
     if the packet is of type IGMP. Other packets are forwarded by silicon*/
  pSnoopCB->counters.controlFramesProcessed++;
  if (ipProtType == IP_PROT_IGMP)
    return snoopPacketFlood(mcastPacket);
  else
    return L7_SUCCESS;
}


typedef struct
{
  L7_uchar8   type;
  L7_uchar8   responseTime;
  L7_ushort16 chksum;
  L7_uint32   groupIP;
} __attribute__ ((packed)) struct_IGMPv2;

typedef struct
{
  L7_uchar8   type;
  L7_uchar8   reserved1;
  L7_ushort16 chksum;
  L7_ushort16 reserved2;
  L7_ushort16 number_of_group_records;
  L7_uchar8   recType;
  L7_uchar8   auxDataLength;
  L7_ushort16 number_of_sources;
  L7_uint32   multicastAddr;
} __attribute__ ((packed)) struct_IGMPv3;

/*********************************************************************
* @purpose  Build IGMP Message
*
* @param    vlanId    @b{(input)}  Vlan id
* @param    type      @b{(input)}  IGMP Packet type
* @param    version   @b{(input)}  IGMP Version
* @param    groupAddr @b{(input)}  IGMP Group address
* @param    buffer    @b{(output)} Buffer to hold the packet
* @param    length    @b{(input)}  Message size
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
static L7_RC_t snoopIGMPFrameBuildv2(L7_uint32      vlanId,
                                     L7_uchar8       type,
                                     L7_uint32       version,
                                     L7_inet_addr_t *groupAddr,
                                     L7_uchar8      *buffer,
                                     L7_uint32      *length,
                                     snoop_cb_t     *pSnoopCB)
{
  L7_uchar8      *dataPtr, *tempPtr, *startPtr;
  L7_uchar8       baseMac[L7_MAC_ADDR_LEN], byteVal;
  L7_uchar8       destMac[L7_MAC_ADDR_LEN];
  L7_ushort16     shortVal;
  static L7_ushort16 iph_ident = 1;
  L7_inet_addr_t  destIp;
  L7_uint32       ipv4Addr;
  L7_uint32       frame_length = IGMP_FRAME_SIZE;

  dataPtr = buffer;

  // Determine Destination IP
  switch (type)
  {
  case L7_IGMP_V1_MEMBERSHIP_REPORT:
  case L7_IGMP_V2_MEMBERSHIP_REPORT:
    memcpy(&destIp,groupAddr,sizeof(L7_inet_addr_t));
    break;

  case L7_IGMP_V2_LEAVE_GROUP:
    destIp.family = L7_AF_INET;
    destIp.addr.ipv4.s_addr = L7_IP_ALL_RTRS_ADDR;
    break;

  case L7_IGMP_MEMBERSHIP_QUERY:
    destIp.family = L7_AF_INET;
    destIp.addr.ipv4.s_addr = L7_IP_ALL_HOSTS_ADDR;
    break;

  case L7_IGMP_V3_MEMBERSHIP_REPORT:
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "snoopIGMPFrameBuildv2: L7_IGMP_V3_MEMBERSHIP_REPORT not supported yet");
    return L7_FAILURE;

  default:
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "snoopIGMPFrameBuildv2: Message type (%u) not supported",type);
    return L7_FAILURE;
  }

  /* Validate the group address being reported */
  snoopMulticastMacFromIpAddr(&destIp, destMac);
  /* Validate MAC address */
  if (snoopMacAddrCheck(destMac, L7_AF_INET) != L7_SUCCESS)
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
  SNOOP_PUT_DATA(destMac, L7_MAC_ADDR_LEN, dataPtr);    // 6 bytes
  SNOOP_PUT_DATA(baseMac, L7_MAC_ADDR_LEN, dataPtr);    // 6 bytes

  /* OuterVlan */
  shortVal = L7_ETYPE_8021Q;
  SNOOP_PUT_SHORT(shortVal, dataPtr);                   // 2 bytes
  shortVal = ((pSnoopCB->snoopCfgData->snoopAdminIGMPPrio & 0x07)<<13) | (vlanId & 0x0fff);
  SNOOP_PUT_SHORT(shortVal, dataPtr);                   // 2 bytes

  /* IP Ether type */
  shortVal = L7_ETYPE_IP;
  SNOOP_PUT_SHORT(shortVal, dataPtr);                   // 2 bytes

  /* Start of IP Header */
  startPtr = dataPtr; 
  /* IP Version */
  byteVal    = (L7_IP_VERSION<<4) | L7_IP_HDR_VER_LEN;
  SNOOP_PUT_BYTE(byteVal, dataPtr);                     // 1 byte

  /* TOS */
  byteVal     = SNOOP_IP_TOS;
  SNOOP_PUT_BYTE(byteVal, dataPtr);                     // 1 byte
  /* Payload Length */
  if (version > SNOOP_IGMP_VERSION_2)
  {
    shortVal = L7_IP_HDR_LEN + SNOOP_IGMPV3_HEADER_MIN_LENGTH;
  }
  else
  {
    shortVal = L7_IP_HDR_LEN + SNOOP_IGMPv1v2_HEADER_LENGTH;
  }
  SNOOP_PUT_SHORT(shortVal, dataPtr);                   // 2 bytes

  /* Identified */
  shortVal = iph_ident++;
  SNOOP_PUT_SHORT(shortVal, dataPtr);                   // 2 bytes

  /* Fragment flags */
  shortVal = 0;
  SNOOP_PUT_SHORT(shortVal, dataPtr);                   // 2 bytes
  /* TTL */
  byteVal = SNOOP_IP_TTL;
  SNOOP_PUT_BYTE(byteVal, dataPtr);                     // 1 byte
  /* Protocol */
  byteVal = IGMP_PROT;
  SNOOP_PUT_BYTE(byteVal, dataPtr);                     // 1 byte
  /* Checksum = 0*/
  shortVal = 0;
  tempPtr = dataPtr;
  SNOOP_PUT_SHORT(shortVal, dataPtr);                   // 2 bytes

  /* Source Address - Snooping Switches send it with null source address */
  //if (inetAddressGet(L7_AF_INET, &pSnoopCB->snoopCfgData->snoopQuerierCfgData.snoopQuerierAddress, &ipv4Addr)!=L7_SUCCESS)  {
  if (inetAddressGet(L7_AF_INET, &pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].snoopVlanQuerierCfgData.snoopQuerierVlanAddress, &ipv4Addr)!=L7_SUCCESS)
  {
    ipv4Addr = 0;
  }
  memcpy(dataPtr, &ipv4Addr, L7_IP_ADDR_LEN); dataPtr += L7_IP_ADDR_LEN;  // 4 bytes
  /* Destination Address */
  inetAddressGet(L7_AF_INET, &destIp, &ipv4Addr);
  memcpy(dataPtr, &ipv4Addr, L7_IP_ADDR_LEN); dataPtr += L7_IP_ADDR_LEN;  // 4 bytes

  shortVal = snoopCheckSum((L7_ushort16 *)startPtr, L7_IP_HDR_LEN, 0);
  SNOOP_PUT_SHORT(shortVal, tempPtr);                   // 2 bytes
  SNOOP_UNUSED_PARAM(tempPtr);
  /* End of IP Header */

  /* Start IGMP Header */
  startPtr = dataPtr;

  /* IGMP Type */
  byteVal = type;
  SNOOP_PUT_BYTE(byteVal, dataPtr);                     // 1 byte
  /* Max response code */
  byteVal = 0;
  SNOOP_PUT_BYTE(byteVal, dataPtr);                     // 1 byte 
  /* Checksum = 0*/
  tempPtr = dataPtr;
  shortVal = 0;
  SNOOP_PUT_SHORT(shortVal, dataPtr);                   // 2 bytes

  if (type==L7_IGMP_V1_MEMBERSHIP_REPORT ||
      type==L7_IGMP_V2_MEMBERSHIP_REPORT ||
      type==L7_IGMP_V2_LEAVE_GROUP ||
      type==L7_IGMP_MEMBERSHIP_QUERY)
  {
    // Group Address
    inetAddressGet(L7_AF_INET, groupAddr, &ipv4Addr);
    memcpy(dataPtr, &ipv4Addr, L7_IP_ADDR_LEN);
    dataPtr += L7_IP_ADDR_LEN;                       // 4 bytes  

    SNOOP_UNUSED_PARAM(dataPtr);

    shortVal = snoopCheckSum((L7_ushort16 *)startPtr, SNOOP_IGMPv1v2_HEADER_LENGTH, 0);
    SNOOP_PUT_SHORT(shortVal, tempPtr); /* Copy the calculated checksum
                                          to stored checksum ptr */
    SNOOP_UNUSED_PARAM(tempPtr);

    frame_length = IGMP_FRAME_SIZE;
  }
  else
  {
    return L7_FAILURE;
  }

  // Return length of packet
  if (length!=L7_NULLPTR)  *length = frame_length;
  return L7_SUCCESS;
}

/**
 * Rebuild packet structure
 * 
 * @param mcastPacket : Packet info
 * @param groupAddr   : Group address (IP channel) to use
 * @param type        : IGMP Message type
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t igmp_rebuild_packet(mgmdSnoopControlPkt_t *mcastPacket, L7_inet_addr_t *groupAddr, L7_uchar8 type)
{
  L7_uchar8       *dataStart;
  L7_uint32        version, frameLength = 0, vlanId;
  L7_RC_t          rc = L7_SUCCESS;
  snoop_cb_t      *pSnoopCB = L7_NULLPTR;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(mcastPacket->cbHandle->family)) == L7_NULLPTR)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "igmp_rebuild_packet: Error getting pSnoopCB");
    return L7_FAILURE;
  }

  /* Get vlan id */
  vlanId  = mcastPacket->vlanId;

  /* USe always version 2 */
  version = SNOOP_IGMP_VERSION_2; // /*(type==SNOOP_IGMP_QUERY) ? igmp_querier_version :*/ pSnoopCB->snoopCfgData->snoopQuerierCfgData.snoopQuerierVersion;

  /* Datastart points to the beginning of data */
  dataStart = mcastPacket->payLoad;

  /* Only allow IPv4 for now */
  if (pSnoopCB->family != L7_AF_INET)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "igmp_rebuild_packet: Not IPv4 packet");
    return L7_FAILURE;
  }

  /* Build packet */
  rc = snoopIGMPFrameBuildv2(vlanId, type, version, groupAddr, dataStart, &frameLength, pSnoopCB);

  /* Everything went went well? */
  if (rc != L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "igmp_rebuild_packet: Something went wrong");
    return L7_FAILURE;
  }

  /* Update frame length */
  mcastPacket->length = frameLength;

  return rc;
}

#if 0
static L7_RC_t igmp_packet_respond(mgmdSnoopControlPkt_t *mcastPacket)
{
  L7_inet_addr_t         groupAddr;
  L7_uchar8              type;
  L7_RC_t                rc = L7_SUCCESS;
  L7_uint32              intf, vlan, innervlan;

  intf      = mcastPacket->intIfNum;
  vlan      = mcastPacket->vlanId;
  innervlan = mcastPacket->innerVlanId;

  // get message type
  type = mcastPacket->ip_payload[0];

  // Get Group channel
  groupAddr.family           = L7_AF_INET;
  groupAddr.addr.ipv4.s_addr = ((struct_IGMPv2 *) mcastPacket->ip_payload)->groupIP;

  // Build answer packet
  if ((rc=igmp_rebuild_packet(mcastPacket, &groupAddr, type))==L7_SUCCESS)
  {
    // Send Packet to mrouter ports
    if ((rc=snoopPacketRtrIntfsForward(mcastPacket))==L7_SUCCESS)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "igmp_packet_respond: Packet transmitted to router interfaces");
    }
    else
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "igmp_packet_respond: Error transmitting to router interfaces");
    }
  }
  else
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "igmp_packet_respond: Error rebuilding packet");
  }

  mcastPacket->intIfNum    = intf;
  mcastPacket->vlanId      = vlan;
  mcastPacket->innerVlanId = innervlan;

  return rc;
}
#endif

static L7_RC_t igmp_packet_general_query_respond(mgmdSnoopControlPkt_t *mcastPacket, L7_inet_addr_t *groupIP)
{
  L7_uint16             channel;
  L7_uint16             number_of_channels;
  ptin_igmpChannelInfo_t channel_list[1024];
  L7_uint               intf, vlanId, client_vlan;

  intf        = mcastPacket->intIfNum;
  vlanId      = mcastPacket->vlanId;
  client_vlan = mcastPacket->innerVlanId;

  /* Initialize number of channels */
  number_of_channels = 0;

  if (groupIP->family != L7_AF_INET)
    return L7_FAILURE;

  if (groupIP->addr.ipv4.s_addr != L7_NULL)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"Respostas ao Specific query (0x%08X) (UNI Vlan=%u)",groupIP->addr.ipv4.s_addr, vlanId);

    /* If this IP exists, send join */

    if (!snoopChannelExist4VlanId(vlanId,groupIP,L7_NULLPTR))
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"Canal 0x%08X existe!", groupIP->addr.ipv4.s_addr);
      number_of_channels = 1;
      memcpy(&channel_list[0], groupIP, sizeof(L7_inet_addr_t) );
    }
    else
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"Canal 0x%08X NAO existe!", groupIP->addr.ipv4.s_addr);
    }
  }
  else
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"UNI Vlan=%u",vlanId);

    /* Note: the vlan that is supplied to the query is the uniVlan and not the MC one!
       The client_vlan must be ignored in this case */
    number_of_channels = L7_MAX_GROUP_REGISTRATION_ENTRIES;
    snoopChannelsListGet(vlanId,(L7_uint16)-1,channel_list,&number_of_channels);
  }

  /* Send channels reports */
  for (channel=0; channel<number_of_channels; channel++)
  {
    /* Prepare packet */
    if (igmp_rebuild_packet(mcastPacket, &channel_list[channel].groupAddr, L7_IGMP_V2_MEMBERSHIP_REPORT) == L7_SUCCESS)
    {
      /* Send packet to all mrouter ports */
      snoopPacketRtrIntfsForward(mcastPacket, L7_IGMP_V2_MEMBERSHIP_REPORT);
    }
    else
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"Error transmitting Query #%u",channel);
    }
  }

  SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"Respostas ao Query=%u",number_of_channels);

  mcastPacket->intIfNum    = intf;
  mcastPacket->vlanId      = vlanId;
  mcastPacket->innerVlanId = client_vlan;

  return L7_SUCCESS;
}

/**
 * Generate an IGMP packet from scratch, and send it to network 
 * interfaces 
 * 
 * @param vlan    : Internal Vlan
 * @param type    : IGMP packet type
 * @param groupIP : IP channel
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t igmp_generate_packet_and_send(L7_uint32 vlan, L7_uint8 type, L7_inet_addr_t *channel)
{
  L7_RC_t                rc = L7_SUCCESS;
  mgmdSnoopControlPkt_t  mcastPacket;

  /* Validate arguments */
  if (vlan<PTIN_VLAN_MIN || vlan>PTIN_VLAN_MAX || channel==L7_NULLPTR)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "igmp_generate_packet_and_send: Invalid arguments");
  }
  /* Valkidate family */
  if (channel->family!=L7_AF_INET)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "igmp_generate_packet_and_send: IP family not supported");
  }

  // Initialize mcastPacket structure
  memset(&mcastPacket,0x00,sizeof(mgmdSnoopControlPkt_t));
  mcastPacket.cbHandle    = snoopCBGet(L7_AF_INET);
  mcastPacket.vlanId      = vlan;
  mcastPacket.innerVlanId = 0;
  mcastPacket.client_idx  = (L7_uint32)-1;
  mcastPacket.msgType     = IP_PROT_IGMP;
  mcastPacket.srcAddr.family            = L7_AF_INET;
  mcastPacket.srcAddr.addr.ipv4.s_addr  = L7_NULL_IP_ADDR;
  mcastPacket.destAddr.family           = L7_AF_INET;
  mcastPacket.destAddr.addr.ipv4.s_addr = L7_NULL_IP_ADDR;

  // Build answer packet
  ptin_timer_start(27,"igmp_generate_packet_and_send-igmp_rebuild_packet");
  rc=igmp_rebuild_packet(&mcastPacket, channel, type);
  ptin_timer_stop(27);
  if (rc==L7_SUCCESS)
  {
    // Send Packet to mrouter ports
    ptin_timer_start(28,"igmp_generate_packet_and_send-snoopPacketRtrIntfsForward");
    rc=snoopPacketRtrIntfsForward(&mcastPacket, type);
    ptin_timer_stop(28);
    if (rc==L7_SUCCESS)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "igmp_generate_packet_and_send: Packet transmitted to router interfaces");
    }
    else
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "igmp_generate_packet_and_send: Error transmitting to router interfaces");
    }
  }
  else
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "igmp_generate_packet_and_send: Error rebuilding packet");
  }

  return rc;
}
