/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsmProto.c
*
* @purpose Contains functions for all PIMSM send/receive API.
*
* @component pimsm
*
* @comments
*
* @create 01/01/2006
*
* @author vmurali/dsatyanarayana
* @end
*
**********************************************************************/
#include "l7_ip_api.h"
#include "pim_defs.h"
#include "pimsmmacros.h"
#include "l3_addrdefs.h"
#include "pimsmdefs.h"
#include "pimsmmain.h"
#include "pimsmproto.h"
#include "pimsmtimer.h"
#include "pimsmneighbor.h"
#include "pimsmcontrol.h"
#include "l7_pimsm_api.h"
#include "l7_mcast_api.h"
#include "heap_api.h"
#include "mcast_wrap.h"
#include "pimsmbsr.h"
#include "pimsmmrt.h"
#include "pimsmcache.h"
#include "pimsmwrap.h"
#include "l3_addrdefs.h"
#include "mcast_wrap.h"
#include "intf_bitset.h"
#include "buff_api.h"
#include "pimsm_map_debug.h"

/******************************************************************************
*
* @purpose  Sendout pimsm packet through Socket
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pDestAddr   @b{(input)}  destination address
* @param    pSrcAddr    @b{(input)}  source address
* @param    msgType     @b{(input)}  pimsm message type
* @param    pPayload    @b{(input)}  payload after pim header
* @param    datalen     @b{(input)}  payload length
* @param    rtrIfNum    @b{(input)}  outgoing router interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
L7_RC_t  pimsmPacketSend(pimsmCB_t *pimsmCb,
                         L7_inet_addr_t * pDestAddr,
                         L7_inet_addr_t *pSrcAddr,
                         L7_uint32 msgType,
                         L7_uchar8 *pPayload, L7_int32 datalen,
                         L7_uint32 rtrIfNum)
{
  mcastSendPkt_t pimsmCtrlPkt;
  L7_int32 checksum = 0, len, ttl;
  pim_header_t *pimsmHdr;
  L7_uchar8 *cp;

  if(pimsmCb->isPimsmEnabled != L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_ERROR,
                "PIM-SM not initialized ");
    return L7_FAILURE;
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_DEBUG, "Enter");
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_DEBUG, "msgType = %d", msgType);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_DEBUG, "pDestAddr = ", pDestAddr);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_DEBUG, "pPayload = %p", pPayload);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_DEBUG, "datalen = %d", datalen);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_DEBUG, "rtrIfNum = %d", rtrIfNum);


  len = (msgType == PIMSM_REGISTER)? 8: datalen;
  pimsmHdr = (pim_header_t*)pPayload;
  pimsmHdr->pim_cksum = 0;

  if(pimsmCb->family == L7_AF_INET)
  {
    checksum = inetChecksum((void*)pPayload, len);
  }
  else if(pimsmCb->family == L7_AF_INET6)
  {
    cp = pimsmCb->pseudoIpv6Hdr;

    MCAST_PUT_DATA(pSrcAddr->addr.ipv6.in6.addr8,16, cp);
    MCAST_PUT_DATA(pDestAddr->addr.ipv6.in6.addr8,16,cp);
    MCAST_PUT_LONG(len, cp);

    MCAST_PUT_BYTE(0, cp);
    MCAST_PUT_BYTE(0, cp);
    MCAST_PUT_BYTE(0, cp);
    MCAST_PUT_BYTE(IP_PROT_PIM, cp);

    /* append the PIM pData */
    /*PIMSM_TRACE_PKT(PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_NOTICE, pPayload,len);*/
    MCAST_PUT_DATA(pPayload,len,cp);

    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_DEBUG,"Len is %d",cp - pimsmCb->pseudoIpv6Hdr);

    /*PIMSM_TRACE_PKT(PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_NOTICE,pimsmCb->pseudoIpv6Hdr ,cp - pimsmCb->pseudoIpv6Hdr);*/
    checksum = inetChecksum((void*)pimsmCb->pseudoIpv6Hdr,cp - pimsmCb->pseudoIpv6Hdr);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_DEBUG,"Checksum is %p\r\n",checksum);

  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_DEBUG, "checksum = 0x%x", checksum);

  pimsmHdr->pim_cksum = checksum;

  memset(&pimsmCtrlPkt, 0, sizeof(mcastSendPkt_t));

  ttl = PIMSM_DEFAULT_MCAST_TTL;
  if (L7_TRUE != inetIsInMulticast(pDestAddr))
  {
   ttl = PIMSM_DEFAULT_UCAST_TTL;
  }
  inetCopy(&pimsmCtrlPkt.srcAddr, pSrcAddr);
  inetCopy(&pimsmCtrlPkt.destAddr, pDestAddr);
  pimsmCtrlPkt.family = pimsmCb->family;
  pimsmCtrlPkt.rtrIfNum = rtrIfNum;
  pimsmCtrlPkt.ttl = ttl;
  pimsmCtrlPkt.protoType = PIM_IP_PROTO_NUM;
  pimsmCtrlPkt.flags = 0;
  pimsmCtrlPkt.sockFd = pimsmCb->sockFd;
  pimsmCtrlPkt.payLoad = (L7_uchar8 *)pimsmHdr;
  pimsmCtrlPkt.length = datalen;
  pimsmCtrlPkt.rtrAlert = L7_FALSE;

  if(L7_SUCCESS != mcastMapPacketSend(&pimsmCtrlPkt))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_ERROR,"Couldn't send the PIMSM packet");
    return L7_FAILURE;
  }
  pimsmDebugPacketRxTxTrace(pimsmCb->family, L7_FALSE, pimsmCtrlPkt.rtrIfNum,
                            &pimsmCtrlPkt.srcAddr, &pimsmCtrlPkt.destAddr,
                            pimsmCtrlPkt.payLoad, pimsmCtrlPkt.length);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_NOTICE, "Sending PIM Packet on rtrIfNum (%d)...",
            rtrIfNum);
  PIMSM_TRACE_PKT(PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_NOTICE, (L7_uchar8 *)pimsmHdr, datalen);

  pimsmStatsIncrement(pimsmCb, rtrIfNum, msgType, PIMSM_TX);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_DEBUG, "Exit");
  return(L7_SUCCESS);
}
/******************************************************************************
*
* @purpose  Send PIM Hello pkt
*
* @param    pimsmCb     @b{(input)}  control block
* @param    rtrIfNum    @b{(input)}  outgoing router interface number
* @param    intIfNum    @b{(input)}  outgoing internal interface number
* @param    pIntfAddr   @b{(input)}  interface address
* @param    holdtime    @b{(input)}  holdtime
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
L7_RC_t pimsmHelloSend(pimsmCB_t *pimsmCb,L7_uint32 rtrIfNum,
      L7_inet_addr_t *pIntfAddr, L7_short16 holdtime)
{

  L7_uchar8   *pDataStart;
  L7_char8 *pData, *numAddrListPtr, *pDataTmp;
  L7_uint32 datalen, numSecondaryAddr = PIMSM_MAX_NBR_SECONDARY_ADDR;
  L7_inet_addr_t secondaryAddrList[PIMSM_MAX_NBR_SECONDARY_ADDR], *pSecAddr;
  L7_uint32  ipPrefixLen[16];
  L7_uint32 ii = 1, intIfNum;
  L7_ushort16 trackingPropagationDelay = 0;

  L7_uint32 drPriority = 0;
  L7_uint32 genId;
  pimsmInterfaceEntry_t  * pIntfEntry = L7_NULLPTR;
  L7_RC_t rc;

  if((pimsmCb == (pimsmCB_t * )L7_NULLPTR))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_ERROR, "Invalid Input");
    PIMSM_TRACE(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_DEBUG, "pimsmCb  = %p", pimsmCb);
    return L7_FAILURE;
  }
  rc = pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_ERROR,"IntfEntry is NULLPTR");
    return L7_FAILURE;
  }
  if (mcastIpMapRtrIntfToIntIfNum(pimsmCb->family,
                                     rtrIfNum, &intIfNum) != L7_SUCCESS)
  {
     PIMSM_TRACE(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_MAX,
      "mcastIpMapRtrIntfToIntIfNum() failed for rtrIfNum = %d", rtrIfNum);
     return L7_FAILURE;
  }

  rc = pimsmMapInterfaceDRPriorityGet(pimsmCb->family, intIfNum, &drPriority);
  if(rc!= L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_ERROR,
                "pimsmMapInterfaceDRPriorityGet() failed! rtrIfNum = %d",
                rtrIfNum);
    return L7_FAILURE;
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_DEBUG, "drPriority = %d", drPriority);
  pDataStart = pimsmCb->pktTxBuf;
  memset(pDataStart , 0 , PIMSM_PKT_SIZE_MAX);
  pData = pDataStart;

  /* Prepare the PIM packet */
  MCAST_PUT_BYTE(((PIM_PROTOCOL_VERSION << 4) | (PIMSM_HELLO & 0x0f)), pData);
  MCAST_PUT_BYTE(0, pData);
  MCAST_PUT_SHORT(0, pData);


  PIMSM_TRACE(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_DEBUG, "holdtime = %d", holdtime);
  MCAST_PUT_SHORT(PIMSM_MESSAGE_HELLO_HOLDTIME, pData);
  MCAST_PUT_SHORT(PIMSM_MESSAGE_HELLO_HOLDTIME_LENGTH, pData);
  MCAST_PUT_SHORT(holdtime, pData);


  MCAST_PUT_SHORT(PIMSM_MESSAGE_HELLO_DR_PRIORITY, pData);
  MCAST_PUT_SHORT(PIMSM_MESSAGE_HELLO_DR_PRIORITY_LENGTH, pData);
  MCAST_PUT_LONG(drPriority, pData);

  genId = pIntfEntry->pimsmInterfaceGenerationIDValue;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_DEBUG, "genId = %d", genId);
  MCAST_PUT_SHORT(PIMSM_MESSAGE_HELLO_GENERATION_ID, pData);
  MCAST_PUT_SHORT(PIMSM_MESSAGE_HELLO_GENERATION_ID_LENGTH, pData);
  MCAST_PUT_LONG(genId, pData);

  /* Put the LAN Prune Dealy option */
  MCAST_PUT_SHORT(PIMSM_MESSAGE_HELLO_LAN_PRUNE_DELAY,pData);
  MCAST_PUT_SHORT(PIMSM_MESSAGE_HELLO_LAN_PRUNE_DELAY_LENGTH,pData);
  trackingPropagationDelay = PIMSM_DEFAULT_PROPAGATION_DELAY_MSECS;
  trackingPropagationDelay |= PIMSM_MESSAGE_HELLO_TRACKING_BIT_OFFSET;
  MCAST_PUT_SHORT(trackingPropagationDelay,pData);
  MCAST_PUT_SHORT(PIMSM_DEFAULT_OVERRIDE_INTERVAL_MSECS,pData);

  /* Put the address list option, if secondary addresses are present
     for that interface*/

  if (pimsmCb->family == L7_AF_INET6)
  {
     pSecAddr = secondaryAddrList;
     mcastIpMapRtrIntfIpAddressesGet(pimsmCb->family, rtrIfNum,
         &numSecondaryAddr, pSecAddr, ipPrefixLen);
     if (numSecondaryAddr > 1)
     {
       MCAST_PUT_SHORT(PIMSM_MESSAGE_HELLO_ADDRESS_LIST, pData);
      numAddrListPtr = pData;
      MCAST_PUT_SHORT(0, pData);
       pDataTmp = pData;
       while (ii < PIMSM_MAX_NBR_SECONDARY_ADDR && ii < numSecondaryAddr)
       {
         PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_INFO,
               " sending sec addr = ",
               &secondaryAddrList[ii]);
         PIM_PUT_EUADDR_INET(&secondaryAddrList[ii],pData);
         ii++;
       }
      MCAST_PUT_SHORT(pData - pDataTmp, numAddrListPtr);
      MCAST_UNUSED_PARAM (numAddrListPtr);
     }
  }


  datalen = pData - (L7_char8 *)pDataStart;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_DEBUG, "pDataStart = 0x%p", pDataStart);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_DEBUG, "pData = 0x%p", pData);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_DEBUG, "datalen = %d", datalen);
  rc = pimsmPacketSend(pimsmCb, &pimsmCb->allPimRoutersGrpAddr, pIntfAddr,
                       (PIMSM_HELLO),
                       pDataStart, datalen,  rtrIfNum);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_ERROR,
                "pimsmPacketSend() failed! ");
    return L7_FAILURE;
  }

  return(L7_SUCCESS);
}


/******************************************************************************
*
* @purpose  Send PIM Register pkt
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pGrpAddr    @b{(input)}  group address
* @param    regFlags    @b{(input)}  register flags to set
* @param    pIPPkt      @b{(input)}  start of ip pkt header
* @param    ipPktLen    @b{(input)}  ip pkt length
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
L7_RC_t  pimsmRegisterSend(pimsmCB_t *pimsmCb, L7_inet_addr_t * pGrpAddr,
                  L7_uint32 regFlags,
                  L7_char8 *pIPPkt, L7_uint32 ipPktLen)
{

  L7_uint32 datalen =0;
  L7_uint32 rtrIfNum;
  L7_inet_addr_t  rpAddr;
  L7_uchar8 *pDataStart;
  L7_uchar8 *pData;
  L7_RC_t rc;
  L7_BOOL retVal;
  L7_inet_addr_t srcAddr;
  pimsmStarStarRpNode_t *   pStarStarRpNode = L7_NULLPTR;

  if((pimsmCb == (pimsmCB_t * )L7_NULLPTR) ||
     (pGrpAddr == (L7_inet_addr_t * )L7_NULLPTR) ||
     (ipPktLen > 0 && pIPPkt == (L7_char8 * )L7_NULLPTR))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM, PIMSM_TRACE_ERROR, "Invalid Input");
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM, PIMSM_TRACE_DEBUG, "pimsmCb  = %p", pimsmCb);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM, PIMSM_TRACE_DEBUG, "pGrpAddr = %p", pGrpAddr);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM, PIMSM_TRACE_DEBUG, "pIPPkt = %p", pIPPkt);
    return L7_FAILURE;
  }
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM, PIMSM_TRACE_DEBUG, "pGrpAddr :",pGrpAddr);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM, PIMSM_TRACE_DEBUG, "regFlags = 0x%x", regFlags);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM, PIMSM_TRACE_DEBUG, "pIPPkt = %p", pIPPkt);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM, PIMSM_TRACE_DEBUG, "ipPktLen = %d", ipPktLen);

  retVal= pimsmMapIsInSsmRange(pimsmCb->family, pGrpAddr);
  if(retVal == L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM, PIMSM_TRACE_DEBUG,
                "pGrpAddr is in SSM Range, dont send Reg-msg");
    return L7_FAILURE;
  }
  pDataStart = pimsmCb->pktTxBuf;
  memset(pDataStart , 0 , PIMSM_PKT_SIZE_MAX);

  pData = pDataStart;
  /* Prepare the PIM packet */
  MCAST_PUT_BYTE(((PIM_PROTOCOL_VERSION << 4) | (PIMSM_REGISTER & 0x0f)), pData);
  MCAST_PUT_BYTE(0, pData);
  MCAST_PUT_SHORT(0, pData);

  regFlags = osapiHtonl(regFlags);
  MCAST_PUT_LONG(regFlags, pData);
  /* include the ip header */
  if(ipPktLen > 0)
  {
    memcpy(pData, pIPPkt, ipPktLen);
  }
  datalen = (pData - pDataStart) + ipPktLen;
  if(pimsmRpAddressGet(pimsmCb, pGrpAddr, &rpAddr)!= L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM, PIMSM_TRACE_ERROR, "RP not found" );
    return L7_FAILURE;
  }

  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM, PIMSM_TRACE_DEBUG, "rpAddr :",&rpAddr);
  rc = pimsmStarStarRpFind( pimsmCb, &rpAddr, &pStarStarRpNode );
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM, PIMSM_TRACE_ERROR,
                "pimsmStarStarRpFind() failed! ");
    return L7_FAILURE ;
  }
  rtrIfNum = pStarStarRpNode->pimsmStarStarRpEntry.pimsmStarStarRpRPFIfIndex;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM, PIMSM_TRACE_INFO, "rtrIfNum = %d", rtrIfNum);
  if(rtrIfNum == PIMSM_REGISTER_INTF_NUM)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM, PIMSM_TRACE_INFO,
                "I am RP for this pGrpAddr, dont send Reg-Msg");
    return L7_FAILURE;
  }

  rc = mcastIpMapGlobalIpAddressGet(pimsmCb->family, rtrIfNum, &srcAddr);

  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM, PIMSM_TRACE_ERROR,
                "mcastIpMapGlobalIpAddressGet() failed rtrIfNum = %d", rtrIfNum);
    return L7_FAILURE;
  }
  rc = pimsmPacketSend(pimsmCb, &rpAddr, &srcAddr, PIMSM_REGISTER,
                       pDataStart, datalen, rtrIfNum);

  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM, PIMSM_TRACE_ERROR,
                "pimsmPacketSend() failed! ");
    return L7_FAILURE;
  }

  return(L7_SUCCESS);
}

/******************************************************************************
*
* @purpose  Send PIM Null-Register pkt
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pSrcAddr    @b{(input)}  source address
* @param    pGrpAddr    @b{(input)}  group address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
L7_RC_t pimsmNullRegisterSend(pimsmCB_t *pimsmCb,
                              L7_inet_addr_t * pSrcAddr,
                              L7_inet_addr_t * pGrpAddr)
{
  L7_uint32     regFlags = 0, ipPktLen = 0;
#define PIMSM_MAX_IP_HDR_LEN 50  /*ipv4 = 20 and ipv6 = 40 & pim-hdr= 4 */
  L7_uchar8 pIPPkt[PIMSM_MAX_IP_HDR_LEN];

  memset(pIPPkt , 0 , PIMSM_MAX_IP_HDR_LEN);

  regFlags |= PIMSM_MESSAGE_REGISTER_NULL_REGISTER_BIT;
  pimsmIpHdrFrame(pimsmCb->family,  pIPPkt , pSrcAddr, pGrpAddr, &ipPktLen);
  pimsmRegisterSend(pimsmCb, pGrpAddr, regFlags, pIPPkt, ipPktLen);
  return(L7_SUCCESS);
}
/******************************************************************************
*
* @purpose  Send Register-Stop pkt.
*
* @param    pimsmCb       @b{(input)}  control block
* @param      rtrIfNum      @b{(input)}  router interface number
            pRegDestAddr  @b{(input)}  register pkt destination address
            pInnerGrpAddr @b{(input)}  encoded group address
            pInnerSrcAddr @b{(input)}  encoded source address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
******************************************************************************/
L7_RC_t pimsmRegisterStopSend(pimsmCB_t *pimsmCb,
         L7_uint32 rtrIfNum, L7_inet_addr_t * pRegDestAddr,
         L7_inet_addr_t * pInnerGrpAddr, L7_inet_addr_t * pInnerSrcAddr)
{
  L7_uchar8 *pDataStart;
  L7_uchar8 *pData;
  L7_uchar8 grpMaskLen =0;
  L7_uint32 datalen =0;
  L7_RC_t rc;
  L7_inet_addr_t srcAddr;

  if((pimsmCb == (pimsmCB_t * )L7_NULLPTR) ||
     (pRegDestAddr == (L7_inet_addr_t * )L7_NULLPTR) ||
     (pInnerGrpAddr == (L7_inet_addr_t * )L7_NULLPTR) ||
     (pInnerSrcAddr == (L7_inet_addr_t * )L7_NULLPTR))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM, PIMSM_TRACE_ERROR, "Invalid Input");
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM, PIMSM_TRACE_INFO, "pimsmCb  = %p", pimsmCb);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM, PIMSM_TRACE_INFO, "rtrIfNum = %d", rtrIfNum);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM, PIMSM_TRACE_INFO, "pRegDestAddr = %p", pRegDestAddr);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM, PIMSM_TRACE_INFO, "pInnerGrpAddr = %p", pInnerGrpAddr);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM, PIMSM_TRACE_INFO, "pInnerSrcAddr = %p", pInnerSrcAddr);
    return L7_FAILURE;
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM, PIMSM_TRACE_INFO, "rtrIfNum = %d", rtrIfNum);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM, PIMSM_TRACE_INFO, "pRegDestAddr :",pRegDestAddr);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM, PIMSM_TRACE_INFO, "pInnerGrpAddr :",pInnerGrpAddr);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM, PIMSM_TRACE_INFO, "pInnerSrcAddr :",pInnerSrcAddr);

  pDataStart = pimsmCb->pktTxBuf;
  memset(pDataStart , 0 , PIMSM_PKT_SIZE_MAX);
  pData = (L7_char8 *)pDataStart;
  /* Prepare the PIM packet */
  MCAST_PUT_BYTE(((PIM_PROTOCOL_VERSION << 4) | (PIMSM_REGISTER_STOP & 0x0f)), pData);
  MCAST_PUT_BYTE(0, pData);
  MCAST_PUT_SHORT(0, pData);

  pimGrpMaskLenGet(pInnerGrpAddr->family, &grpMaskLen);
  PIM_PUT_EGADDR_INET(pInnerGrpAddr, grpMaskLen, 0, pData);
  PIM_PUT_EUADDR_INET(pInnerSrcAddr, pData);
  datalen = pData - pDataStart;

  rc = mcastIpMapGlobalIpAddressGet(pimsmCb->family, rtrIfNum, &srcAddr);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM, PIMSM_TRACE_ERROR,
                "mcastIpMapGlobalIpAddressGet() failed rtrIfNum = %d", rtrIfNum);
    return L7_FAILURE;
  }
  rc = pimsmPacketSend(pimsmCb, pRegDestAddr, &srcAddr,
                       (PIMSM_REGISTER_STOP),
                       pDataStart, datalen, rtrIfNum);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM, PIMSM_TRACE_ERROR,
                "pimsmPacketSend() failed! ");
    return L7_FAILURE;
  }
  return(L7_SUCCESS);
}



/******************************************************************************
*
* @purpose  Send Assert Pkt
*
* @param    pimsmCb        @b{(input)}  control block
* @param    pSrcAddr         @b{(input)}  encoded source address
            pGrpAddr         @b{(input)}  encoded group address
            rtrIfNum         @b{(input)}  outgoing router interface number
            localMetric      @b{(input)}  metric value
            localPref      @b{(input)}  preference value

*
* @returns  L7_BOOL
*
* @comments     none
*
* @end
*
******************************************************************************/
L7_RC_t  pimsmAssertSend(pimsmCB_t *pimsmCb, L7_inet_addr_t *pSrcAddr,
                         L7_inet_addr_t *pGrpAddr, L7_uint32  rtrIfNum,
                         L7_uint32 localPref, L7_uint32 localMetric)
{


  L7_uchar8 *pDataStart;
  L7_uchar8 *pData;
  L7_uint32 datalen;
  L7_uchar8 grpMaskLen =0;
  L7_RC_t rc;
  pimsmInterfaceEntry_t * pIntfEntry;

  if((pimsmCb == (pimsmCB_t * )L7_NULLPTR) ||
     (pSrcAddr == (L7_inet_addr_t * )L7_NULLPTR) ||
     (pGrpAddr == (L7_inet_addr_t * )L7_NULLPTR))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR, "Invalid Input");
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "pimsmCb  = %p", pimsmCb);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "pSrcAddr = %p", pSrcAddr);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "pGrpAddr = %p", pGrpAddr);
    return L7_FAILURE;
  }

  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "pSrcAddr :",pSrcAddr);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "pGrpAddr :",pGrpAddr);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "rtrIfNum = %d", rtrIfNum);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "localPref = %d", localPref);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "localMetric = %d", localMetric);

  /* Don't send assert if the outgoing interface a tunnel or register vif */
  if(rtrIfNum == PIMSM_REGISTER_INTF_NUM)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR, "Error! rtrIfNum is register-interface");
    return(L7_FAILURE);
  }

  rc = pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,"Interface is not up");
    return L7_FAILURE;
  }
  pDataStart = pimsmCb->pktTxBuf;
  memset(pDataStart , 0 , PIMSM_PKT_SIZE_MAX);

  pData = pDataStart;
  /* Prepare the PIM packet */
  MCAST_PUT_BYTE(((PIM_PROTOCOL_VERSION << 4) | (PIMSM_ASSERT & 0x0f)), pData);
  MCAST_PUT_BYTE(0, pData);
  MCAST_PUT_SHORT(0, pData);

  pimGrpMaskLenGet(pGrpAddr->family, &grpMaskLen);

  PIM_PUT_EGADDR_INET(pGrpAddr, grpMaskLen, 0, pData);
  PIM_PUT_EUADDR_INET(pSrcAddr, pData);
  MCAST_PUT_LONG(localPref, pData);
  MCAST_PUT_LONG(localMetric, pData);
  datalen = pData - pDataStart;
  rc = pimsmPacketSend(pimsmCb, &pimsmCb->allPimRoutersGrpAddr,
                        &pIntfEntry->pimsmInterfaceAddr,
                       (PIMSM_ASSERT),
                       pDataStart, datalen, rtrIfNum);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,
                "pimsmPacketSend() failed! ");
    return L7_FAILURE;
  }
  return(L7_SUCCESS);
}

/******************************************************************************
*
* @purpose  Send PIM Assert Cancel pkt
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pSrcAddr      @b{(input)}  encoded source address
            pGrpAddr      @b{(input)}  encoded group address
            rtrIfNum      @b{(input)}  outgoing router interface number

*
* @returns  L7_BOOL
*
* @comments     none
*
* @end
*
******************************************************************************/
L7_RC_t pimsmAssertCancelSend(pimsmCB_t *pimsmCb, L7_inet_addr_t * pSrcAddr,
                              L7_inet_addr_t * pGrpAddr, L7_uint32  rtrIfNum)
{

  return pimsmAssertSend(pimsmCb, pSrcAddr, pGrpAddr, rtrIfNum,
                         PIMSM_DEFAULT_ASSERT_PREFERENCE |
                         PIMSM_ASSERT_RPT_BIT,
                         PIMSM_DEFAULT_ASSERT_METRIC);
}
/******************************************************************************
*
* @purpose  Send PIM  join/prune pkt immediately
*
* @param    pimsmCb       @b{(input)}  control block
* @param    pJPData          @b{(input)}  join/prune data to sendout
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
L7_RC_t pimsmJoinPruneImdtSend(pimsmCB_t *pimsmCb,
                               pimsmSendJoinPruneData_t * pJPData)
{

  L7_uchar8 *pData;
  L7_uchar8 *pDataStart;
  L7_uint32  datalen =0;
  L7_inet_addr_t  allMcastGrpAddrH;
  L7_inet_addr_t  *pNbrAddr;
  L7_RC_t rc;
  L7_uchar8 maskLen =0;
  pimsmInterfaceEntry_t *pIntfEntry = L7_NULLPTR;

  if((pimsmCb == (pimsmCB_t * )L7_NULLPTR) ||
     (pJPData == (pimsmSendJoinPruneData_t * )L7_NULLPTR))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR, "Invalid Input");
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_DEBUG, "pimsmCb  = %p", pimsmCb);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_DEBUG, "pJPData  = %p", pJPData);
    return L7_FAILURE;
  }

  /* Get the Interface Address */
  if (pimsmIntfEntryGet (pimsmCb, pJPData->rtrIfNum, &pIntfEntry) != L7_SUCCESS)
  {
    PIMSM_TRACE (PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                 "intfEntry is NULL for rtrIfNum = %d", pJPData->rtrIfNum);
    return L7_FAILURE;
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_DEBUG, "isStarStarRpFlag = %d",
              pJPData->isStarStarRpFlag);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_DEBUG, "neighborAddr : ",
                   pJPData->pNbrAddr);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_DEBUG, "holdtime = %d",
              pJPData->holdtime);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_DEBUG, "srcAddr :",
                   pJPData->pSrcAddr);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_DEBUG, "srcAddrMaskLen = %d",
              pJPData->srcAddrMaskLen);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_DEBUG, "grpAddr :",
                   pJPData->pGrpAddr);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_DEBUG, "grpAddrMaskLen = %d",
              pJPData->grpAddrMaskLen);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_DEBUG, "addrFlags = 0x%x",
              pJPData->addrFlags);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_DEBUG, "rtrIfNum = %d",
              pJPData->rtrIfNum);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_DEBUG, "joinOrPruneFlag = %d",
              pJPData->joinOrPruneFlag);

  if(pJPData->rtrIfNum == PIMSM_REGISTER_INTF_NUM)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
      "Error! rtrIfNum is register-interface");
    return(L7_FAILURE);
  }

  pDataStart = pimsmCb->pktTxBuf;
  memset(pDataStart , 0 , PIMSM_PKT_SIZE_MAX);

  pData = pDataStart;
  /* Prepare the PIM packet */
  MCAST_PUT_BYTE(((PIM_PROTOCOL_VERSION << 4) | (PIMSM_JOIN_PRUNE & 0x0f)), pData);
  MCAST_PUT_BYTE(0, pData);
  MCAST_PUT_SHORT(0, pData);

  /*Copy the family, type, and neighbor address */
  pNbrAddr = pJPData->pNbrAddr;
  PIM_PUT_EUADDR_INET(pNbrAddr, pData);
  MCAST_PUT_BYTE(0, pData);          /* Reserved */
  MCAST_PUT_BYTE(1, pData);          /* numgroups */
  MCAST_PUT_SHORT(pJPData->holdtime, pData);

  if(pJPData->isStarStarRpFlag == L7_TRUE)
  {
    pimsmStarStarRpMaskLenGet(pimsmCb->family,&maskLen);
    inetAllMcastGroupAddressInit(pimsmCb->family, &allMcastGrpAddrH);
    PIM_PUT_EGADDR_INET(&allMcastGrpAddrH, maskLen, 0, pData);
  }
  else if(pJPData->isStarStarRpFlag == L7_FALSE)
  {
    PIM_PUT_EGADDR_INET(pJPData->pGrpAddr,
                        pJPData->grpAddrMaskLen, 0, pData);
  }
  else
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                "Error: isStarStarRpFlag is not set correctly");
    return(L7_FAILURE);
  }
  if(pJPData->joinOrPruneFlag == L7_TRUE)
  {
    MCAST_PUT_SHORT(1, pData);/* Join Addr num*/
    MCAST_PUT_SHORT(0, pData);/* Prune Addr num*/
  }
  else if(pJPData->joinOrPruneFlag == L7_FALSE)
  {
    MCAST_PUT_SHORT(0, pData);/* Join Addr num*/
    MCAST_PUT_SHORT(1, pData);/* Prune Addr num */
  }
  else
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                "Error: joinOrPruneFlag is not set correctly");
    return(L7_FAILURE);
  }

  pJPData->addrFlags |= PIMSM_ADDR_S_BIT;   /* Mandatory for PIMv2 */
  PIM_PUT_ESADDR_INET(pJPData->pSrcAddr,
                      pJPData->srcAddrMaskLen,
                      pJPData->addrFlags, pData);
  datalen = pData - pDataStart;
  rc = pimsmPacketSend(pimsmCb, &pimsmCb->allPimRoutersGrpAddr,
                       &pIntfEntry->pimsmInterfaceAddr,
                       PIMSM_JOIN_PRUNE,
                       pDataStart, datalen, pJPData->rtrIfNum);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                "pimsmPacketSend() failed! ");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}



/******************************************************************************
*
* @purpose  allocate buffers to build join/prune message
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pJPMsgBuff  @b{(output)} buffer to build join/prune message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t
pimsmJPWorkingBuffGet(pimsmCB_t *pimsmCb, pimsmJPMsgStoreBuff_t *pJPMsgBuff)
{
  memset(pJPMsgBuff, 0, sizeof(pimsmJPMsgStoreBuff_t));

  if ((pJPMsgBuff->pJPMsg =
        PIMSM_ALLOC (pimsmCb->family ,(PIMSM_MAX_JP_MSG_SIZE )))
                 == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                "Unable to allocate buffer from jpMsgBufferId");
    pJPMsgBuff->pJPMsg = L7_NULLPTR;
    return L7_FAILURE;
  }
  if ((pJPMsgBuff->pJoinList =
        PIMSM_ALLOC (pimsmCb->family,
                    ((pimJoinPruneMaxAddrsInListGet(pimsmCb->family)) * sizeof(pim_encod_src_addr_t))))
                 == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                "Unable to allocate buffer from jpAddrListBufferId");
    PIMSM_FREE (pimsmCb->family, (void*) pJPMsgBuff->pJPMsg);
    return L7_FAILURE;
  }

  if ((pJPMsgBuff->pPruneList =
        PIMSM_ALLOC (pimsmCb->family,
                    ((pimJoinPruneMaxAddrsInListGet(pimsmCb->family)) * sizeof(pim_encod_src_addr_t))))
                 == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                "Unable to allocate buffer from jpAddrListBufferId");
    PIMSM_FREE (pimsmCb->family, (void*) pJPMsgBuff->pJPMsg);
    PIMSM_FREE (pimsmCb->family, (void*) pJPMsgBuff->pJoinList);
    return L7_FAILURE;
  }

  if ((pJPMsgBuff->pRpJoinList =
        PIMSM_ALLOC (pimsmCb->family,
                    ((pimJoinPruneMaxAddrsInListGet(pimsmCb->family)) * sizeof(pim_encod_src_addr_t))))
                 == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                "Unable to allocate buffer from jpAddrListBufferId");
    PIMSM_FREE (pimsmCb->family, (void*) pJPMsgBuff->pJPMsg);
    PIMSM_FREE (pimsmCb->family, (void*) pJPMsgBuff->pJoinList);
    PIMSM_FREE (pimsmCb->family, (void*) pJPMsgBuff->pPruneList);
    return L7_FAILURE;
  }
  if ((pJPMsgBuff->pRpPruneList =
        PIMSM_ALLOC (pimsmCb->family,
                    ((pimJoinPruneMaxAddrsInListGet(pimsmCb->family)) * sizeof(pim_encod_src_addr_t))))
                 == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                "Unable to allocate buffer from jpAddrListBufferId");
    PIMSM_FREE (pimsmCb->family, (void*) pJPMsgBuff->pJPMsg);
    PIMSM_FREE (pimsmCb->family, (void*) pJPMsgBuff->pJoinList);
    PIMSM_FREE (pimsmCb->family, (void*) pJPMsgBuff->pPruneList);
    PIMSM_FREE (pimsmCb->family, (void*) pJPMsgBuff->pRpJoinList);
    return L7_FAILURE;
  }

  inetAddressZeroSet(pimsmCb->family, &pJPMsgBuff->currGrpAddr);
  pJPMsgBuff->buildJPMsgUse = L7_TRUE;
  return L7_SUCCESS;
}

/******************************************************************************
*
* @purpose de-allocate buffers used to build join/prune message
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pNbrEntry   @b{(input)}  neighbor entry
*
* @returns  void
*
* @comments     none
*
* @end
*
******************************************************************************/
void
pimsmJPWorkingBuffReturn(pimsmCB_t *pimsmCb, pimsmNeighborEntry_t *pNbrEntry)
{
  pimsmJPMsgStoreBuff_t *pJPMsgBuff = &pNbrEntry->pimsmBuildJPMsg;

  if( pJPMsgBuff->buildJPMsgUse != L7_TRUE)
  {
   return;
  }
  if (pJPMsgBuff->pJPMsg != L7_NULLPTR)
  {
   PIMSM_FREE (pimsmCb->family, (void*) pJPMsgBuff->pJPMsg);
  }
  if (pJPMsgBuff->pJoinList != L7_NULLPTR)
  {
    PIMSM_FREE (pimsmCb->family, (void*) pJPMsgBuff->pJoinList);
  }
  if (pJPMsgBuff->pPruneList != L7_NULLPTR)
  {
   PIMSM_FREE (pimsmCb->family, (void*) pJPMsgBuff->pPruneList);
  }
  if (pJPMsgBuff->pRpJoinList != L7_NULLPTR)
  {
    PIMSM_FREE (pimsmCb->family, (void*) pJPMsgBuff->pRpJoinList);
  }
  if (pJPMsgBuff->pRpPruneList != L7_NULLPTR)
  {
    PIMSM_FREE (pimsmCb->family, (void*) pJPMsgBuff->pRpPruneList);
  }

  memset(pJPMsgBuff, 0, sizeof(pimsmJPMsgStoreBuff_t));

  inetAddressZeroSet(pimsmCb->family, &pJPMsgBuff->currGrpAddr);

  pJPMsgBuff->buildJPMsgUse = L7_FALSE;
}
/******************************************************************************
*
* @purpose  Send PIM  join/prune pkt
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pNbrEntry   @b{(input)}  neighbor entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t pimsmJPMsgSend(pimsmCB_t *pimsmCb,
                              pimsmNeighborEntry_t *pNbrEntry)
{

  L7_uchar8 *pDataStart;
  L7_uchar8 *pData;
  L7_uint32  datalen, rtrIfNum;
  L7_RC_t rc;
  pimsmInterfaceEntry_t *pIntfEntry = L7_NULLPTR;

  pDataStart = pimsmCb->pktTxBuf;
  memset(pDataStart , 0 , PIMSM_PKT_SIZE_MAX);
  pData = pDataStart;
  /* Prepare the PIM packet */
  MCAST_PUT_BYTE(((PIM_PROTOCOL_VERSION << 4) | (PIMSM_JOIN_PRUNE & 0x0f)), pData);
  MCAST_PUT_BYTE(0, pData);
  MCAST_PUT_SHORT(0, pData);

  rtrIfNum = pNbrEntry->pimsmNeighborIfIndex;
  datalen = pNbrEntry->pimsmBuildJPMsg.jpMsgSize;
  memcpy(pData, pNbrEntry->pimsmBuildJPMsg.pJPMsg, datalen);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_DEBUG,
                "pimsmBuildJPMsg.pJPMsg Length = %d", datalen);
  rc = pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry);
  if (rc  != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                "intfEntry is NULL for rtrIfNum = %d", rtrIfNum);
    pimsmJPWorkingBuffReturn(pimsmCb, pNbrEntry);
    return L7_FAILURE;
  }
  rc = pimsmPacketSend(pimsmCb, &pimsmCb->allPimRoutersGrpAddr,
                        &pIntfEntry->pimsmInterfaceAddr,
                       (PIMSM_JOIN_PRUNE),
                       pDataStart, datalen + PIM_HEARDER_SIZE, rtrIfNum);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                "pimsmPacketSend() failed! ");
    pimsmJPWorkingBuffReturn(pimsmCb, pNbrEntry);
    return L7_FAILURE;
  }

  pimsmJPWorkingBuffReturn(pimsmCb, pNbrEntry);
  return L7_SUCCESS;
}
/******************************************************************************
*
* @purpose  Pack (*,*,RP) fields in join/prune message
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pJPMsgBuff  @b{(input)} buffer to build join/prune message
*
* @returns  void
*
* @comments     none
*
* @end
*
******************************************************************************/
static void pimsmStarStarRpMsgPack(pimsmCB_t *pimsmCb, pimsmJPMsgStoreBuff_t *pJPMsgBuff)
{
  L7_uchar8  maskLen = 0;
  L7_inet_addr_t  allMcastGrpAddrH;
  L7_uchar8 *pData;

  if((pJPMsgBuff->numRpJoinAddrs + pJPMsgBuff->numRpPruneAddrs) > 0)
  {

    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                  "pJPMsgBuff->pJPMsg = %p pJPMsgBuff->jpMsgSize = %d",
                  pJPMsgBuff->pJPMsg, pJPMsgBuff->jpMsgSize);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                  "pJPMsgBuff->currGrpAddr = ",
                  &pJPMsgBuff->currGrpAddr);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                  "pJPMsgBuff->currGrpMaskLen = %d",
                  pJPMsgBuff->currGrpMaskLen);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                  "pJPMsgBuff->joinListSize = %d",
                  pJPMsgBuff->joinListSize);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                  "pJPMsgBuff->numJoinAddrs = %d",
                  pJPMsgBuff->numJoinAddrs);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                  "pJPMsgBuff->pruneListSize = %d",
                  pJPMsgBuff->pruneListSize);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                  "pJPMsgBuff->numPruneAddrs = %d",
                  pJPMsgBuff->numPruneAddrs);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                  "*pJPMsgBuff->pNumGrps = %d",
                  *pJPMsgBuff->pNumGrps);

    /* Add the (*,*,RP) at the beginning */
    pData = pJPMsgBuff->pJPMsg + pJPMsgBuff->jpMsgSize;
    pimsmStarStarRpMaskLenGet(pimsmCb->family,&maskLen);
    inetAllMcastGroupAddressInit(pimsmCb->family, &allMcastGrpAddrH);
    PIM_PUT_EGADDR_INET(&allMcastGrpAddrH, maskLen, 0, pData);
    MCAST_PUT_SHORT(pJPMsgBuff->numRpJoinAddrs, pData);
    MCAST_PUT_SHORT(pJPMsgBuff->numRpPruneAddrs, pData);
    memcpy(pData, pJPMsgBuff->pRpJoinList, pJPMsgBuff->rpJoinListSize);
    pData += pJPMsgBuff->rpJoinListSize;
    memcpy(pData, pJPMsgBuff->pRpPruneList, pJPMsgBuff->rpPruneListSize);
    pData += pJPMsgBuff->rpPruneListSize;
    pJPMsgBuff->jpMsgSize = (pData - pJPMsgBuff->pJPMsg);
    pJPMsgBuff->rpJoinListSize = 0;
    pJPMsgBuff->numRpJoinAddrs = 0;
    pJPMsgBuff->rpPruneListSize = 0;
    pJPMsgBuff->numRpPruneAddrs = 0;
    (*pJPMsgBuff->pNumGrps)++;
  }
}
/******************************************************************************
*
* @purpose   Pack all fields in join/prune message
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pNbrEntry   @b{(input)}  neighbor entry
*
* @returns  void
*
* @comments     none
*
* @end
*
* TODO: XXX: Currently, the (*,*,RP) stuff goes at the end of the
* Join/Prune message. However, this particular implementation of PIM
* processes the Join/Prune messages faster if (*,*,RP) is at the beginning.
* Modify some of the functions below such that the
* outgoing messages place (*,*,RP) at the beginning, not at the end.
*
******************************************************************************/
static void
pimsmJoinPruneMsgPack(pimsmCB_t *pimsmCb, pimsmNeighborEntry_t *pNbrEntry)
{
  pimsmJPMsgStoreBuff_t *pJPMsgBuff;
  L7_uchar8 *pData;

  pJPMsgBuff = &pNbrEntry->pimsmBuildJPMsg;
  if(pJPMsgBuff->buildJPMsgUse != L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_DEBUG,
                "buildJPMsgUse not in use");
    return;
  }
  if(inetIsAddressZero(&pJPMsgBuff->currGrpAddr) == L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_DEBUG,
                "currGrpAddr is not set");
    return;
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                "pJPMsgBuff->pJPMsg = %p pJPMsgBuff->jpMsgSize = %d",
                pJPMsgBuff->pJPMsg, pJPMsgBuff->jpMsgSize);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                "pJPMsgBuff->currGrpAddr = ",
                &pJPMsgBuff->currGrpAddr);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                "pJPMsgBuff->currGrpMaskLen = %d",
                pJPMsgBuff->currGrpMaskLen);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                "pJPMsgBuff->joinListSize = %d",
                pJPMsgBuff->joinListSize);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                "pJPMsgBuff->numJoinAddrs = %d",
                pJPMsgBuff->numJoinAddrs);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                "pJPMsgBuff->pruneListSize = %d",
                pJPMsgBuff->pruneListSize);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                "pJPMsgBuff->numPruneAddrs = %d",
                pJPMsgBuff->numPruneAddrs);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                "*pJPMsgBuff->pNumGrps = %d",
                *pJPMsgBuff->pNumGrps);


  pData = pJPMsgBuff->pJPMsg + pJPMsgBuff->jpMsgSize;
  PIM_PUT_EGADDR_INET(&pJPMsgBuff->currGrpAddr,
   pJPMsgBuff->currGrpMaskLen, 0, pData);
  MCAST_PUT_SHORT(pJPMsgBuff->numJoinAddrs, pData);
  MCAST_PUT_SHORT(pJPMsgBuff->numPruneAddrs, pData);
  memcpy(pData, pJPMsgBuff->pJoinList, pJPMsgBuff->joinListSize);
  pData += pJPMsgBuff->joinListSize;
  memcpy(pData, pJPMsgBuff->pPruneList, pJPMsgBuff->pruneListSize);
  pData += pJPMsgBuff->pruneListSize;
  pJPMsgBuff->jpMsgSize = (pData - pJPMsgBuff->pJPMsg);
  inetAddressZeroSet(pimsmCb ->family, &pJPMsgBuff->currGrpAddr);
  pJPMsgBuff->currGrpMaskLen = 0;
  pJPMsgBuff->joinListSize = 0;
  pJPMsgBuff->numJoinAddrs = 0;
  pJPMsgBuff->pruneListSize = 0;
  pJPMsgBuff->numPruneAddrs = 0;
  (*pJPMsgBuff->pNumGrps)++;
/*  if (*pJPMsgBuff->pNumGrps > PIMSM_MAX_JP_NUM_GROUPS)
  {
    pimsmStarStarRpMsgPack(pimsmCb, pJPMsgBuff);
    if(pimsmJPMsgSend(pimsmCb, pNbrEntry)!= L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
          "pimsmJPMsgSend() Failed");
      return ;
    }
  }
*/
}

/******************************************************************************
*
* @purpose  Pack and send join/prune message
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pNbrEntry   @b{(input)}  neighbor entry
*
* @returns  void
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t
pimsmJoinPruneMsgPackAndSend(pimsmCB_t *pimsmCb,
         pimsmNeighborEntry_t *pNbrEntry)
{
  pimsmJPMsgStoreBuff_t *pJPMsgBuff;

  if((pNbrEntry == (pimsmNeighborEntry_t *)L7_NULLPTR)||
     pNbrEntry->pimsmBuildJPMsg.buildJPMsgUse != L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_MAX,
                "pNbrEntry is L7_NULLPTR or buildJPMsgUse is FALSE");
    return L7_FAILURE;
  }
  pJPMsgBuff = &pNbrEntry->pimsmBuildJPMsg;
  pimsmStarStarRpMsgPack(pimsmCb, pJPMsgBuff);
  pimsmJoinPruneMsgPack(pimsmCb, pNbrEntry);
  if(pimsmJPMsgSend(pimsmCb, pNbrEntry)!= L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
        "pimsmJPMsgSend() Failed");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}


/******************************************************************************
*
* @purpose   Prepare to send PIM join/prune pkt
*
* @param    pimsmCb     @b{(input)}  control block
* @param    sgSendJoinPruneData               @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     Use This API if bundling join/prune is required.
            The Logic is very implementation specific.

*
* @end
*
******************************************************************************/
static L7_RC_t pimsmJoinPruneBundleSend(pimsmCB_t *pimsmCb,
            pimsmNeighborEntry_t *pNbrEntry,
            pimsmSendJoinPruneData_t * pJPData)
{
  pimsmJPMsgStoreBuff_t *pJPMsgBuff;
  L7_uchar8 *pData;
  L7_BOOL isStarStarRpFlag = pJPData->isStarStarRpFlag;
  L7_ushort16 holdtime = pJPData->holdtime;
  L7_inet_addr_t *pGrpAddr= pJPData->pGrpAddr;
  L7_uchar8 grpAddrMaskLen= pJPData->grpAddrMaskLen;
  L7_inet_addr_t * pSrcAddr= pJPData->pSrcAddr;
  L7_uchar8 srcAddrMaskLen= pJPData->srcAddrMaskLen;
  L7_ushort16 addrFlags = pJPData->addrFlags;
  L7_BOOL joinOrPruneFlag = pJPData->joinOrPruneFlag;
  L7_uint32 joinPruneMsgSize=0, maxJPListSize =0;



  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "isStarStarRpFlag = %d",
              pJPData->isStarStarRpFlag);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "neighborAddr : ",
                   pJPData->pNbrAddr);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "holdtime = %d",
              pJPData->holdtime);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "srcAddr :",
                   pJPData->pSrcAddr);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "srcAddrMaskLen = %d",
              pJPData->srcAddrMaskLen);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "grpAddr :",
                   pJPData->pGrpAddr);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "grpAddrMaskLen = %d",
              pJPData->grpAddrMaskLen);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "addrFlags = 0x%x",
              pJPData->addrFlags);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "rtrIfNum = %d",
              pJPData->rtrIfNum);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "joinOrPruneFlag = %d",
              pJPData->joinOrPruneFlag);

  pJPMsgBuff = &pNbrEntry->pimsmBuildJPMsg;

  if(pimJoinPruneMsgLenGet(pimsmCb->family,&joinPruneMsgSize)
            != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
        "pimJoinPruneMsgLenGet() Failed");
    return L7_FAILURE;
  }
  maxJPListSize =  (pimJoinPruneMaxAddrsInListGet(pimsmCb->family)) * joinPruneMsgSize;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
     "maxJPListSize = %d", maxJPListSize);
  if(pJPMsgBuff->buildJPMsgUse == L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                  "pJPMsgBuff->jpMsgSize = %d",
                  pJPMsgBuff->jpMsgSize);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                  "pJPMsgBuff->joinListSize = %d",
                  pJPMsgBuff->joinListSize);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                  "pJPMsgBuff->pruneListSize = %d",
                  pJPMsgBuff->pruneListSize);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                  "pJPMsgBuff->rpJoinListSize = %d",
                  pJPMsgBuff->rpJoinListSize);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                  "pJPMsgBuff->rpPruneListSize = %d",
                  pJPMsgBuff->rpPruneListSize);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "JP Msg Buff= %p",pJPMsgBuff);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "JP Msg Buff Build Msg= %p",pJPMsgBuff->pJPMsg);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "grpAddr in JP MsgBuff :",&pJPMsgBuff->currGrpAddr);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "holdtime in JP msgBuff= %d",pJPMsgBuff->holdtime);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "GrpMaskLen in JP msgBuff= %d",pJPMsgBuff->currGrpMaskLen);

    if(((pJPMsgBuff->jpMsgSize + pJPMsgBuff->joinListSize +
        pJPMsgBuff->pruneListSize + pJPMsgBuff->rpJoinListSize +
        pJPMsgBuff->rpPruneListSize + 24 + 20) >=
        (PIMSM_MAX_JP_MSG_SIZE- joinPruneMsgSize))
       || (pJPMsgBuff->joinListSize >=
            (maxJPListSize -1))
       || (pJPMsgBuff->pruneListSize >=
            (maxJPListSize-1))
       || (pJPMsgBuff->rpJoinListSize >=
            (maxJPListSize-1))
       || (pJPMsgBuff->rpPruneListSize >=
            (maxJPListSize-1)))
    {
      /* TODO:  If the list is getting too large, must
       * be careful with the fragmentation.
       */
      if(pimsmJoinPruneMsgPackAndSend(pimsmCb, pNbrEntry)!= L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
            "pimsmJoinPruneMsgPackAndSend() Failed");
        return L7_FAILURE;
      }
    }

    if(isStarStarRpFlag == L7_FALSE &&
      ((PIMSM_INET_IS_ADDR_EQUAL(&pJPMsgBuff->currGrpAddr,pGrpAddr) == L7_FALSE)
       || (pJPMsgBuff->currGrpMaskLen != grpAddrMaskLen)
       || (pJPMsgBuff->holdtime != holdtime)))
    {
      pimsmJoinPruneMsgPack(pimsmCb, pNbrEntry);
    }
  }

  if (pJPMsgBuff->buildJPMsgUse != L7_TRUE)
  {
   if( pimsmJPWorkingBuffGet(pimsmCb, &pNbrEntry->pimsmBuildJPMsg)
         != L7_SUCCESS)
    {
       PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
         "Unable to allocate buffers for JPMsg");
       return L7_FAILURE;
    }
    pData = pJPMsgBuff->pJPMsg;
    PIM_PUT_EUADDR_INET(&pNbrEntry->pimsmNeighborAddrList.pimsmPrimaryAddress, pData);
    MCAST_PUT_BYTE(0, pData);          /* Reserved */
    pJPMsgBuff->pNumGrps = pData++;  /* The pointer for numgroups */
    *(pJPMsgBuff->pNumGrps) = 0;        /* Zero groups */
    MCAST_PUT_SHORT(holdtime, pData);
    pJPMsgBuff->holdtime = holdtime;
    pJPMsgBuff->jpMsgSize = pData - pJPMsgBuff->pJPMsg;
  }

  if(isStarStarRpFlag != L7_TRUE)
  {
    inetCopy(&pJPMsgBuff->currGrpAddr, pGrpAddr);
    pJPMsgBuff->currGrpMaskLen = grpAddrMaskLen;
  }
  else
  {
    inetAddressZeroSet(pimsmCb ->family, &pJPMsgBuff->currGrpAddr);
    pJPMsgBuff->currGrpMaskLen = 0;
  }

  switch(joinOrPruneFlag)
  {
    case L7_TRUE:
      if(isStarStarRpFlag == L7_TRUE)
      {
        pData = pJPMsgBuff->pRpJoinList + pJPMsgBuff->rpJoinListSize;
      }
      else
      {
        pData = pJPMsgBuff->pJoinList + pJPMsgBuff->joinListSize;
      }
      break;
    case L7_FALSE:
      if(isStarStarRpFlag == L7_TRUE)
      {
        pData = pJPMsgBuff->pRpPruneList + pJPMsgBuff->rpPruneListSize;
      }
      else
      {
        pData = pJPMsgBuff->pPruneList + pJPMsgBuff->pruneListSize;
      }
      break;
    default:
      return(L7_FAILURE);
  }

  addrFlags |= PIMSM_ADDR_S_BIT;   /* Mandatory for PIMv2 */
  PIM_PUT_ESADDR_INET(pSrcAddr, srcAddrMaskLen, addrFlags, pData);

  switch(joinOrPruneFlag)
  {
    case L7_TRUE:
      if(isStarStarRpFlag == L7_TRUE)
      {
        pJPMsgBuff->rpJoinListSize = pData - pJPMsgBuff->pRpJoinList;
        pJPMsgBuff->numRpJoinAddrs++;
      }
      else
      {
        pJPMsgBuff->joinListSize = pData - pJPMsgBuff->pJoinList;
        pJPMsgBuff->numJoinAddrs++;
      }
      break;
    case L7_FALSE:
      if(isStarStarRpFlag == L7_TRUE)
      {
        pJPMsgBuff->rpPruneListSize = pData - pJPMsgBuff->pRpPruneList;
        pJPMsgBuff->numRpPruneAddrs++;
      }
      else
      {
        pJPMsgBuff->pruneListSize = pData - pJPMsgBuff->pPruneList;
        pJPMsgBuff->numPruneAddrs++;
      }
      break;
    default:
      return(L7_FAILURE);
  }

  return L7_SUCCESS;
}
/******************************************************************************
*
* @purpose  Prepare to send PIM join/prune pkt
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pJPData          @b{(input)}  join/prune data to sendout
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
L7_RC_t pimsmJoinPruneSend(pimsmCB_t *pimsmCb,
                           pimsmSendJoinPruneData_t * pJPData)
{

  L7_RC_t rc;
  pimsmNeighborEntry_t *pNbrEntry;
  pimsmInterfaceEntry_t  * pIntfEntry = L7_NULLPTR;

  if((pimsmCb == (pimsmCB_t * )L7_NULLPTR) ||
     (pJPData == (pimsmSendJoinPruneData_t * )L7_NULLPTR))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR, "Invalid Input");
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_DEBUG, "pimsmCb  = %p", pimsmCb);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_DEBUG, "pJPData  = %p", pJPData);
    return L7_FAILURE;
  }

  if(pJPData->rtrIfNum == PIMSM_REGISTER_INTF_NUM)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR, "Error! rtrIfNum is register-interface");
    return(L7_FAILURE);
  }

  if(pimsmIntfEntryGet( pimsmCb, pJPData->rtrIfNum, &pIntfEntry )
     !=L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,"IntfEntry is NULLPTR");
    return L7_FAILURE;
  }

  if (pJPData->sendImmdFlag != L7_TRUE)
  {
      pJPData->sendImmdFlag = pimsmIAmRP(pimsmCb, pJPData->pGrpAddr);
  }
  if(pJPData->pruneEchoFlag == L7_TRUE || pJPData->sendImmdFlag == L7_TRUE)
  {
    pimsmJoinPruneImdtSend(pimsmCb,pJPData);
  }
  else
  {
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,"Neighbor Addr is:",
      pJPData->pNbrAddr);
    rc = pimsmNeighborFind(pimsmCb, pIntfEntry, pJPData->pNbrAddr,
                           &pNbrEntry);
    if(rc != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,"Upstream Neighbor not found");
      return L7_FAILURE;
    }

    rc = pimsmJoinPruneBundleSend(pimsmCb,pNbrEntry, pJPData);
    if(rc != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
            "pimsmJoinPruneBundleSend() failed");
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose
*
* @param        pParam  @b{(input)} timer handle
*
* @returns   void
*
* @comments
*
* @end
******************************************************************************/
void pimsmJPBundleTimerExpiryHandler(void *pParam)
{
  L7_uint32 rtrIfNum;
  pimsmTimerData_t *pTimerData;
  pimsmCB_t *pimsmCb = L7_NULLPTR;
  pimsmInterfaceEntry_t *   pIntfEntry = L7_NULLPTR;
  pimsmNeighborEntry_t *    pCurrentNbrEntry;
  L7_RC_t rc;
  L7_int32      handle = (L7_int32)pParam;

  pTimerData = (pimsmTimerData_t*)handleListNodeRetrieve(handle);
  if(L7_NULLPTR == pTimerData)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,"Invalid input parameter");
    return;
  }

  pimsmCb = pTimerData->pimsmCb;
  if(L7_NULLPTR == pimsmCb)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,"Control Block is NULL");
    return;
  }
  if (pimsmCb->pimsmJPBundleTimer == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_PIMSM_MAP_COMPONENT_ID,
            "PIMSM pimsmJPBundleTimer is NULL, But Still Expired");
    return;
  }
  pimsmCb->pimsmJPBundleTimer = L7_NULLPTR;
  for(rtrIfNum = 1; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
  {
    if(pimsmIntfEntryGet( pimsmCb, rtrIfNum, &pIntfEntry )!= L7_SUCCESS)
    {
      continue;
    }
    rc = pimsmNeighborGetFirst( pimsmCb, pIntfEntry, &pCurrentNbrEntry );
    while(rc == L7_SUCCESS)
    {
      if(pimsmJoinPruneMsgPackAndSend(pimsmCb, pCurrentNbrEntry)!= L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_DEBUG,
            "pimsmJoinPruneMsgPackAndSend() Failed");
      }
      rc = pimsmNeighborNextGet( pimsmCb, pIntfEntry, pCurrentNbrEntry,
         &pCurrentNbrEntry );
    }

  }

  if (pimsmUtilAppTimerSet (pimsmCb, pimsmJPBundleTimerExpiryHandler,
                            (void*)pimsmCb->pimsmJPBundleTimerHandle,
                            PIMSM_DEFAULT_JP_BUNDLE_TIME,
                            &(pimsmCb->pimsmJPBundleTimer),
                            "SM-JPB2")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR, "pimsmJPBundleTimer appTimerAdd Failed");
    return;
  }
}

/******************************************************************************
* @purpose
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pData       @b{(input)}  start of neighbor addresses
* @param    optionLen   @b{(input)}  total option length
* @param    pParams     @b{(output)}  hello params
*
* @returns   void
*
* @comments
*
* @end
******************************************************************************/
static void pimsmSecondaryAddrListGet(pimsmCB_t *pimsmCb,L7_uchar8 *pData,
                        L7_ushort16   optionLen, pimsmHelloParams_t *pParams)
{
  pim_encod_uni_addr_t nbrAddr;
  L7_uchar8 i = 0, *pNbrList;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_INFO, "Entry option len = %d",optionLen);

  pNbrList = (L7_uchar8 *)pData;
  while (i<PIMSM_MAX_NBR_SECONDARY_ADDR && (pNbrList - pData) < optionLen)
  {
     memset(&nbrAddr, 0, sizeof(pim_encod_uni_addr_t));
     PIM_GET_EUADDR_INET(&nbrAddr, pNbrList);
     inetAddressSet(nbrAddr.addr_family, &nbrAddr.addr, &pParams->addrList[i]);
     PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_INFO, "extracted addr = ",&pParams->addrList[i]);
    i++;
  }
}
/******************************************************************************
*
* @purpose  Parse the hello message
*
* @param    pimsmCb     @b{(input)}  control block
* @param    rtrIfNum    @b{(input)}  router interface number
* @param    pPimHeader  @b{(input)}  start of pim header
* @param    pimPktLen     @b{(input)}  pim packet length
* @param    pParams      @b{(output)}  hello params
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_BOOL
pimsmHellpParseParams(pimsmCB_t *pimsmCb, L7_uint32 rtrIfNum,
            L7_char8 *pPimHeader,
            L7_uint32 pimPktLen, pimsmHelloParams_t *pParams)
{
  L7_char8 *pPimHelloMsg;
  L7_char8 *pData;
  L7_ushort16 optionType;
  L7_ushort16 optionLen;
  L7_uint32 optionTotalLen;
  L7_uint32  datalen,i;
  L7_BOOL holdtimeOptionPresent = L7_FALSE;
  pPimHelloMsg = (L7_char8 *)(pPimHeader + PIM_HEARDER_SIZE);

  datalen = pimPktLen - PIM_HEARDER_SIZE;

  memset(pParams, 0 , sizeof(pimsmHelloParams_t));

  for (i=0;i<PIMSM_MAX_NBR_SECONDARY_ADDR;i++)
  {
    inetAddressZeroSet(pimsmCb->family, &pParams->addrList[i]);
  }

  while(datalen > 0)
  {
    /* Ignore any data if shorter than (pim_hello header) */
    pData = pPimHelloMsg;
    MCAST_GET_SHORT(optionType, pData);
    MCAST_GET_SHORT(optionLen, pData);
    switch(optionType)
    {
      case PIMSM_MESSAGE_HELLO_HOLDTIME:
        if(PIMSM_MESSAGE_HELLO_HOLDTIME_LENGTH != optionLen)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_ERROR,"wrong option len optionType = %d",
                      optionType);
          return(L7_FALSE);
        }
        MCAST_GET_SHORT(pParams->holdtime, pData);
        holdtimeOptionPresent = L7_TRUE;
        break;
      case PIMSM_MESSAGE_HELLO_DR_PRIORITY:
        if(PIMSM_MESSAGE_HELLO_DR_PRIORITY_LENGTH != optionLen)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_ERROR,
                      "wrong option len optionType = %d option_len = %d",
                      optionType, optionLen);
          return(L7_FALSE);
        }
        MCAST_GET_LONG(pParams->drPriority, pData);
        break;
      case PIMSM_MESSAGE_HELLO_GENERATION_ID:
        if(PIMSM_MESSAGE_HELLO_GENERATION_ID_LENGTH != optionLen)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_ERROR,
                      "wrong option len optionType = %d option_len = %d",
                      optionType, optionLen);
          return(L7_FALSE);
        }
        MCAST_GET_LONG(pParams->genId, pData);
        break;

      case PIMSM_MESSAGE_HELLO_ADDRESS_LIST:
        if (optionLen <= 0)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_ERROR,
                      "wrong option len optionType = %d option_len = %d",
                      optionType, optionLen);
          return(L7_FALSE);
        }
        pimsmSecondaryAddrListGet(pimsmCb,pData,optionLen,pParams);
        pData = pData + optionLen;
        break;

      case PIMSM_MESSAGE_HELLO_LAN_PRUNE_DELAY:
        if(PIMSM_MESSAGE_HELLO_LAN_PRUNE_DELAY_LENGTH != optionLen)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_ERROR,
                      "wrong option len optionType = %d option_len = %d",
                      optionType, optionLen);
          return(L7_FALSE);
        }
        pParams->LANPruneDelayPresent = L7_TRUE;
        MCAST_GET_SHORT(pParams->propagationDelay, pData);
        if ((pParams->propagationDelay & PIMSM_MESSAGE_HELLO_TRACKING_BIT_OFFSET)
                 != L7_NULL)
        {
          pParams->trackingSupportPresent = L7_TRUE;
          pParams->propagationDelay &= ~PIMSM_MESSAGE_HELLO_TRACKING_BIT_OFFSET;
        }
        MCAST_GET_SHORT(pParams->overrideIntvl, pData);
        break;

      default:
        PIMSM_TRACE(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_DEBUG,
                    "Ignoring Unknown optionType = %d option_len = %d",
                    optionType, optionLen);
        /* Ignore any unknown options */
        break;
    }

    /* Move to the next option */
    optionTotalLen = (PIMSM_HELLO_HEARDER_SIZE + optionLen);
    datalen -= optionTotalLen;
    pPimHelloMsg += optionTotalLen;
  }/* end of while */

  if(holdtimeOptionPresent == L7_FALSE)
  {
    /* Holdtime options was not send in hello-msg */
    if(pimsmInterfaceHelloHoldtimeGet(pimsmCb, rtrIfNum, &pParams->holdtime)
       != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_ERROR,
                  "pimsmInterfaceHelloHoldtimeGet() failed! rtrIfNum = %d",
                  rtrIfNum);
      return L7_FALSE;
    }
  }
  return L7_TRUE;
}


/******************************************************************************
*
* @purpose  Receive Hello-pkts processing
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pSrcAddr    @b{(input)}  ip source address
* @param    pDestAddr   @b{(input)}  ip destination address
* @param    pPimHeader  @b{(input)}  start of pim header
* @param    pimPktLen     @b{(input)}  pim packet length
* @param    rtrIfNum    @b{(input)}  incoming router interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t pimsmHelloRecv(pimsmCB_t *pimsmCb,L7_inet_addr_t *pSrcAddr,
                              L7_inet_addr_t *pDestAddr,
                              L7_char8 *pPimHeader, L7_uint32 pimPktLen,
                              L7_uint32 rtrIfNum)
{
  pimsmInterfaceEntry_t *pIntfEntry = L7_NULLPTR;
  pimsmNeighborEntry_t  *pNbrEntry;

  pimsmHelloParams_t helloParams;
  L7_RC_t rc;

  /*
   * Either a local vif or somehow received PIMSM_HELLO from
   * non-directly connected router. Ignore it else get interface number
   */
  PIMSM_TRACE(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_DEBUG, "rtrIfNum = %d", rtrIfNum);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_DEBUG, "pSrcAddr :", pSrcAddr);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_DEBUG, "pDestAddr :", pDestAddr);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_DEBUG, "length = %d", pimPktLen);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_DEBUG, "payLoad = %p", pPimHeader);

  if (mcastIpMapIsRtrIntfUnnumbered(pimsmCb->family,rtrIfNum) != L7_TRUE)
  {
    if (inetIsDirectlyConnected(pSrcAddr, rtrIfNum)== L7_FALSE)
    {
      /* Message from non-directly connected router.  */
      PIMSM_TRACE(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_ERROR,
                  "Ignoring PIMSM_HELLO from non-neighbor router:");
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_INFO, "Address :",pSrcAddr);
      return(L7_FAILURE);
    }
  }

  if(pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_ERROR,"PIMSM is not Enabled on intf = %d",
                rtrIfNum);
    return L7_FAILURE;
  }
  /* Get the parameters from the message. Return if error. */
  if(pimsmHellpParseParams(pimsmCb, rtrIfNum, pPimHeader,
                           pimPktLen, &helloParams) == L7_FALSE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_ERROR,"Error parsing hello-msg on rtrIfNum = %d",
                rtrIfNum);
    return L7_FAILURE;
  }
  rc = pimsmNeighborFind(pimsmCb, pIntfEntry, pSrcAddr, &pNbrEntry);
  if(rc != L7_SUCCESS)
  {
    /* neighbor not found */
    PIMSM_TRACE(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_DEBUG,"neighbor not found intf = %d",
                rtrIfNum);
    /* No entry found so this is a new neighbor.
   Create a new entry for it.*/
    rc = pimsmNeighborCreate(pimsmCb, pSrcAddr, pIntfEntry, &helloParams);
    if(rc != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_ERROR,"pimsmNeighborCreate() failed intf = %d",
                  rtrIfNum);
      return L7_FAILURE;
    }
    /* Since a new neighbour has come up, let it know your existence */
    rc = pimsmHelloSend(pimsmCb, rtrIfNum,
         &pIntfEntry->pimsmInterfaceAddr,
         pIntfEntry->pimsmInterfaceHelloHoldtime);
    if(rc != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_ERROR,"pimsmHelloSend() failed intf = %d",
                  rtrIfNum);
      return L7_FAILURE;
    }
    /* Unicast the BSM when a new neighbor comes up */
    pimsmBsrUnicastBSM(pimsmCb,pSrcAddr,rtrIfNum);
  }
  else
  {
    /* neighbor found */
    PIMSM_TRACE(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_DEBUG,"neighbor  found intf = %d",
                rtrIfNum);
    rc = pimsmNeighborUpdate(pimsmCb, pIntfEntry, pNbrEntry, &helloParams);
    if(rc != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_HELLO_RX_TX, PIMSM_TRACE_ERROR,"pimsmNeighborUpdate() failed intf = %d",
                  rtrIfNum);
      return L7_FAILURE;
    }
  }
  return(L7_SUCCESS);
}

/******************************************************************************
* @purpose   part of register-pkt processing
*
* @param    pimsmCb        @b{(input)}  control block
* @param      pSGNode         @b{(input)}  (s,g) node
            pRegSrcAddr      @b{(input)}  register-stop pkt ip source address
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t pimsmRegisterStopProcessing(pimsmCB_t *pimsmCb,
                                           pimsmSGNode_t * pSGNode,
                                           L7_inet_addr_t *pRegSrcAddr)
{
  pimsmPerSGRegisterEventInfo_t   registerEventInfo;
#ifdef PIMSM_TBD /*Remove later*/
  pimsmSGEntry_t  * pSGEntry;
  L7_inet_addr_t rpAddr;
  L7_RC_t rc;
  L7_inet_addr_t * pSrcAddr;

  /* XXX: not in the spec: check if the PIMSM_REGISTER_STOP originator is
   * really the RP
   */
  pSGEntry = &pSGNode->pimsmSGEntry;
  rc = pimsmRpAddressGet(pimsmCb, &pSGEntry->pimsmSGGrpAddress, &rpAddr);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_ERROR,
                "pimsmRpAddressGet failed ");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "pGrpAddr :",
      &pSGEntry->pimsmSGGrpAddress);
    return L7_FAILURE;
  }
  if(PIMSM_INET_IS_ADDR_EQUAL(&rpAddr, pRegSrcAddr) == L7_FALSE)
  {
    /* The RP address doesn't match. Ignore. */
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_ERROR,
                "RP address doesn't match ");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "recv RP Addr :",pRegSrcAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "my_rp_addr :",&rpAddr);
    return L7_FAILURE;
  }

#endif /*PIMSM_TBD */
  memset(&registerEventInfo, 0 , sizeof(pimsmPerSGRegisterEventInfo_t));
  registerEventInfo.eventType = PIMSM_REG_PER_S_G_SM_EVENT_RECV_REG_STOP;

  pimsmPerSGRegisterExecute(pimsmCb, pSGNode, &registerEventInfo);
  if (pimsmSGMFCUpdate(pimsmCb,pSGNode, MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM, PIMSM_TRACE_ERROR,
      "Failed to update (S,G) exact");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/******************************************************************************
*
* @purpose  Receive Register-Stop pkt processing.
*
* @param    pimsmCb        @b{(input)}  control block
* @param    pRegSrcAddr    @b{(input)}  ip source address
* @param    pRegDestAddr   @b{(input)}  ip destination address
* @param    pPimHeader     @b{(input)}  start of pim header
* @param    pimPktLen        @b{(input)}  pim packet length
* @param    rtrIfNum       @b{(input)}  incoming router interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t pimsmRegisterStopRecv(pimsmCB_t *pimsmCb,
                L7_inet_addr_t *pRegSrcAddr,
                L7_inet_addr_t *pRegDestAddr,
                L7_char8 *pPimHeader, L7_uint32 pimPktLen,
                L7_uint32 rtrIfNum)
{
  pim_encod_grp_addr_t encodGrpAddr;
  pim_encod_uni_addr_t encodSrcAddr;
  L7_inet_addr_t grpAddr, srcAddr;
  L7_char8 *pData;
  pimsmSGNode_t   * pSGNode = L7_NULLPTR;
  L7_RC_t rc;
  pimsmSGEntry_t *pSGEntry;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "rtrIfNum = %d", rtrIfNum);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "pRegSrcAddr :", pRegSrcAddr);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "pRegDestAddr :", pRegDestAddr);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "length = %d", pimPktLen);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "payLoad = %p", pPimHeader);

  memset(&encodGrpAddr, 0 ,sizeof(pim_encod_grp_addr_t));
  memset(&encodSrcAddr, 0 ,sizeof(pim_encod_uni_addr_t));

  pData = (pPimHeader + PIM_HEARDER_SIZE);
  PIM_GET_EGADDR_INET(&encodGrpAddr, pData);
  PIM_GET_EUADDR_INET(&encodSrcAddr, pData);

  MCAST_UNUSED_PARAM (pData);

  inetAddressSet(encodGrpAddr.addr_family,
                 &(encodGrpAddr.addr),&grpAddr);

  inetAddressSet(encodSrcAddr.addr_family,
                 &(encodSrcAddr.addr),&srcAddr);
  if(inetIsAddressZero(&srcAddr)!= L7_TRUE)
  {
    /* Search for (S,G) entry */
    rc = pimsmSGFind(pimsmCb, &grpAddr, &srcAddr, &pSGNode);
    if(rc != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_ERROR,
                  "pimsmSGFind failed");
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "pGrpAddr :",&grpAddr);
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "pSrcAddr :",&srcAddr);
      return L7_FAILURE;
    }
    pimsmRegisterStopProcessing(pimsmCb,pSGNode,pRegSrcAddr);
  }
  else
  {
    /* Source address == 0 */
    rc = pimsmSGFirstGet(pimsmCb,&pSGNode);
    while(rc == L7_SUCCESS)
    {
      pSGEntry = &pSGNode->pimsmSGEntry;
      if(PIMSM_INET_IS_ADDR_EQUAL(&pSGEntry->pimsmSGGrpAddress,
                               &grpAddr) == L7_TRUE)
      {
        pimsmRegisterStopProcessing(pimsmCb,pSGNode,pRegSrcAddr);
      }
      rc = pimsmSGNextGet(pimsmCb,pSGNode, &pSGNode);
    }
  }

  return(L7_SUCCESS);
}

/******************************************************************************
*
* @purpose  Receive Register pkt processing (s,g) part.
*
* @param    pimsmCb           @b{(input)}  control block
* @param    pRegSrcAddr       @b{(input)}  ip source address
* @param    pRegDestAddr      @b{(input)}  ip destination address
* @param    pIPPkt            @b{(input)}  start of ip pkt
* @param    ipLen               @b{(input)}  ip pkt length
* @param    rtrIfNum          @b{(input)}  incoming router interface number
* @param    pInnerSrcAddr     @b{(input)}  encoded source address
* @param    pInnerGrpAddr     @b{(input)}  encoded group address
* @param      borderBit         @b{(input)}  border bit
* @param      nullRegisterBit   @b{(input)}  null-register bit
* @param      pSGNode            @b{(input)}  (s,g) node

* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*


packet_arrives_on_rp_tunnel( pkt ) {
    if( outer.dst is not one of my addresses ) {
        drop the packet silently.
        # Note: this may be a spoofing attempt
    }
    if( I_am_RP(G) AND outer.dst == RP(G) ) {
          sentRegisterStop = FALSE;
          if ( register.borderbit == TRUE ) {
               if ( PMBR(S,G) == unknown ) {
                    PMBR(S,G) = outer.src
               } else if ( outer.src != PMBR(S,G) ) {
                    send Register-Stop(S,G) to outer.src
                    drop the packet silently.
               }
          }
          if ( SPTbit(S,G) OR
           ( SwitchToSptDesired(S,G) AND ( inherited_olist(S,G) == NULL ))) {
            send Register-Stop(S,G) to outer.src
            sentRegisterStop = TRUE;
          }
          if ( SPTbit(S,G) OR SwitchToSptDesired(S,G) ) {
               if ( sentRegisterStop == TRUE ) {
                    restart KeepaliveTimer(S,G) to RP_Keepalive_Period;
               } else {
                    restart KeepaliveTimer(S,G) to Keepalive_Period;
               }
          }
          if( !SPTbit(S,G) AND ! pkt.NullRegisterBit ) {
               decapsulate and forward the inner packet to
               inherited_olist(S,G,rpt) # Note (+)
          }
    } else {
        send Register-Stop(S,G) to outer.src
        # Note (*)
    }
}


outer.dst is the IP destination address of the encapsulating header.

outer.src is the IP source address of the encapsulating header, i.e.,
the DR's address.

* @end
*
******************************************************************************/
static L7_RC_t pimsmRegisterSGEntryProcessing(pimsmCB_t *pimsmCb,
                L7_inet_addr_t *pRegSrcAddr, L7_inet_addr_t *pRegDestAddr,
                L7_char8 *pIPPkt, L7_uint32 ipLen, L7_uint32 rtrIfNum,
                L7_inet_addr_t *pInnerSrcAddr, L7_inet_addr_t *pInnerGrpAddr,
                L7_uint32 borderBit, L7_uint32 nullRegisterBit, L7_uchar8 ipTOS,
                pimsmSGNode_t   *pSGNode)
{

  L7_BOOL     sentRegisterStop = L7_FALSE, sptDesired= L7_FALSE,
  sptBit= L7_FALSE, oifNull= L7_FALSE;
  L7_uint32 timeout;
  interface_bitset_t  oif;
  L7_RC_t rc;
  pimsmSGEntry_t    *pSGEntry;
  L7_inet_addr_t  *pPMBRAddr;
  pimsmSGRptNode_t * pSGRptNode = L7_NULLPTR;

  pSGEntry = &pSGNode->pimsmSGEntry;

  sentRegisterStop = L7_FALSE;
  if(borderBit)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO,
                "border bit is set ");
    pPMBRAddr = &pSGEntry->pimsmSGRPRegisterPMBRAddress;
    if(inetIsAddressZero(pPMBRAddr) == L7_TRUE)
    {
      inetCopy(pPMBRAddr, pRegSrcAddr);
      PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO,
                  "PMBR addr is set to ");
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "pRegSrcAddr :",pRegSrcAddr);
    }
    else if(PIMSM_INET_IS_ADDR_EQUAL(pPMBRAddr, pRegSrcAddr) == L7_FALSE)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO,
                  "received PMBR addr is different ");
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "pRegSrcAddr :",pRegSrcAddr);
      pimsmRegisterStopSend(pimsmCb, rtrIfNum, pRegSrcAddr,
                            pInnerGrpAddr, pInnerSrcAddr);
      return L7_FAILURE;
    }
  }
  sptBit = pSGEntry->pimsmSGSPTBit;
  if(sptBit != L7_TRUE)
  {
    sptDesired = pimsmSwitchToSptDesired(pimsmCb, pInnerSrcAddr,
                                         pInnerGrpAddr, rtrIfNum);
    memset(&oif, 0, sizeof(interface_bitset_t));
    rc = pimsmSGInhrtdOlist(pimsmCb, pSGNode, &oif);
    if(rc != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_ERROR,
                  "pimsmSGInhrtdOlist failed ");
      return L7_FAILURE;
    }
    BITX_IS_EMPTY(&oif, oifNull);
  }
  if((sptBit == L7_TRUE) ||
     ((sptDesired == L7_TRUE) && (oifNull == L7_TRUE)))
  {
    pimsmRegisterStopSend(pimsmCb, rtrIfNum, pRegSrcAddr,
                          pInnerGrpAddr, pInnerSrcAddr);
    sentRegisterStop = L7_TRUE;
  }
  if((sptBit == L7_TRUE) ||
     (sptDesired == L7_TRUE))
  {
    if(sentRegisterStop == L7_TRUE)
    {
      timeout = PIMSM_DEFAULT_RP_KEEPALIVE_PERIOD;
    }
    else
    {
      timeout = PIMSM_DEFAULT_KEEPALIVE_PERIOD;
    }
    pimsmRestartKeepaliveTimer(pimsmCb, pSGNode, timeout);
  }
  if((sptBit != L7_TRUE) &&
     (!nullRegisterBit))
  {
    memset(&oif, 0, sizeof(interface_bitset_t));
    /* decapsulate and forward */
    rc = pimsmSGRptFind(pimsmCb, pInnerGrpAddr, pInnerSrcAddr, &pSGRptNode);
    if(rc == L7_SUCCESS)
    {
      rc = pimsmSGRptInhrtdOlist(pimsmCb, pSGRptNode, &oif);
      if(rc == L7_SUCCESS)
      {
        BITX_IS_EMPTY(&oif, oifNull);
        if ((oifNull == L7_TRUE) &&
            (pSGRptNode->pimsmSGRptEntry.pimsmSGRptRPFIfIndex == PIMSM_REGISTER_INTF_NUM))
        {
           pimsmRegisterStopSend(pimsmCb, rtrIfNum, pRegSrcAddr,
                              pInnerGrpAddr, pInnerSrcAddr);
        }
        else
        {
           pimsmDecapsulateAndSend(pimsmCb->family, pimsmCb->sockFd, &oif, pIPPkt,
                            ipLen, ipTOS);
        }
      }
      else
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_ERROR,
                    "pimsmSGRptInhrtdOlist failed ");
      }
    }
    else
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_ERROR,
                  "pimsmSGRptFind failed ");
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "pGrpAddr :",pInnerGrpAddr);
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "pSrcAddr :",pInnerSrcAddr);
    }
    /* Since we have a (S,G) entry, we would like to switchover
      quickly to form the SPT tree, rather than waiting for register-threshold
      to take effect. At present register-threshold does not make sence
      for (S,G) switchover */
    /*if (pSGEntry->pimsmSGRPFIfIndex == PIMSM_REGISTER_INTF_NUM)*/
    {
      pimsmSwitchToShortestPath(pimsmCb, pInnerSrcAddr, pInnerGrpAddr,
         L7_NULLPTR);
    }

  }
  return(L7_SUCCESS);
}


/******************************************************************************
*
* @purpose  Receive Register pkt processing (s,g,rpt) part.
*
* @param    pimsmCb           @b{(input)}  control block
* @param    pRegSrcAddr       @b{(input)}  ip source address
* @param    pRegDestAddr      @b{(input)}  ip destination address
* @param    pIPPkt            @b{(input)}  start of ip pkt
* @param    ipLen               @b{(input)}  ip pkt length
* @param    rtrIfNum          @b{(input)}  incoming router interface number
* @param    pInnerSrcAddr     @b{(input)}  encoded source address
* @param    pInnerGrpAddr     @b{(input)}  encoded group address
* @param      borderBit         @b{(input)}  border bit
* @param      nullRegisterBit   @b{(input)}  null-register bit
* @param      pSGRptNode         @b{(input)}  (s,g,rpt) node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t pimsmRegisterSGRptEntryProcessing(pimsmCB_t *pimsmCb,
                L7_inet_addr_t *pRegSrcAddr, L7_inet_addr_t *pRegDestAddr,
                L7_char8 *pIPPkt, L7_uint32 ipLen, L7_uint32 rtrIfNum,
                L7_inet_addr_t *pInnerSrcAddr, L7_inet_addr_t *pInnerGrpAddr,
                L7_uint32 borderBit, L7_uint32 nullRegisterBit,
                pimsmSGRptNode_t   *pSGRptNode)
{
    /*TODO: Something missing
      Since we have a (S,G,rpt) entry, we would like to switchover
      quickly to form the SPT tree, rather than waiting for register-threshold
      to take effect. At present register-threshold does not make sence
      for (S,G,rpt) switchover */

  pimsmRegisterStopSend(pimsmCb, rtrIfNum, pRegSrcAddr,
                          pInnerGrpAddr, pInnerSrcAddr);
  pimsmSwitchToShortestPath(pimsmCb, pInnerSrcAddr, pInnerGrpAddr, L7_NULLPTR);
  return(L7_SUCCESS);
}

/******************************************************************************
*
* @purpose  Receive Register pkt processing (*,G) part.
*
* @param    pimsmCb           @b{(input)}  control block
* @param    pRegSrcAddr       @b{(input)}  ip source address
* @param    pRegDestAddr      @b{(input)}  ip destination address
* @param    pIPPkt            @b{(input)}  start of ip pkt
* @param    ipLen               @b{(input)}  ip pkt length
* @param    rtrIfNum          @b{(input)}  incoming router interface number
* @param    pInnerSrcAddr     @b{(input)}  encoded source address
* @param    pInnerGrpAddr     @b{(input)}  encoded group address
* @param      borderBit         @b{(input)}  border bit
* @param      nullRegisterBit   @b{(input)}  null-register bit
* @param      pSGRptNode         @b{(input)}  (*,g) node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t pimsmRegisterStarGEntryProcessing(pimsmCB_t *pimsmCb,
             L7_inet_addr_t *pRegSrcAddr, L7_inet_addr_t *pRegDestAddr,
             L7_char8 *pIPPkt, L7_uint32 ipLen, L7_uint32 rtrIfNum,
             L7_inet_addr_t *pInnerSrcAddr, L7_inet_addr_t *pInnerGrpAddr,
             L7_uint32 borderBit, L7_uint32 nullRegisterBit, L7_uchar8 ipTOS,
             pimsmStarGNode_t   *pStarGNode)
{

  L7_BOOL oifNull;
  interface_bitset_t  oif;
  L7_RC_t rc;
  pimsmSGEntry_t    *pSGEntry = L7_NULLPTR;
  pimsmSGNode_t   *pSGNode= L7_NULLPTR;
  pimsmSendJoinPruneData_t pJPData;
  L7_uchar8 maskLen =0;
  L7_uint32 rtrIfNumSG;

  if(borderBit)
  {
    /* Create (S,G) state. The oifs will be the copied from the
     * existing (*,G) or (*,*,RP) entry. */
    rc = pimsmSGNodeCreate(pimsmCb, pInnerSrcAddr, pInnerGrpAddr, &pSGNode,L7_TRUE);
    if(rc != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_ERROR,
                  "pimsmSGNodeCreate failed");
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "pGrpAddr :",pInnerGrpAddr);
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "pSrcAddr :",pInnerSrcAddr);
      return L7_FAILURE;
    }
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO,
                "border bit is set ");
    inetCopy(&pSGEntry->pimsmSGRPRegisterPMBRAddress, pRegSrcAddr);
    memset(&pJPData, 0, sizeof(pimsmSendJoinPruneData_t));
    rtrIfNumSG = pSGNode->pimsmSGEntry.pimsmSGRPFIfIndex;
    if(pimsmInterfaceJoinPruneHoldtimeGet(pimsmCb, rtrIfNumSG,
                                       &pJPData.holdtime)
                                             != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,
        PIMSM_TRACE_ERROR,"pimsmInterfaceJoinPruneHoldtimeGet failed");
      return L7_FAILURE;
    }
    pJPData.pSrcAddr = pRegSrcAddr;
    pimSrcMaskLenGet(pimsmCb->family,&maskLen);
    pJPData.srcAddrMaskLen = maskLen;
    pJPData.pGrpAddr = pRegDestAddr;
    pimGrpMaskLenGet(pimsmCb->family,&maskLen);
    pJPData.grpAddrMaskLen = maskLen;
    pJPData.addrFlags = 0;
    pJPData.rtrIfNum = rtrIfNumSG;
    pJPData.joinOrPruneFlag = L7_TRUE;
    pJPData.pNbrAddr = &pSGNode->pimsmSGEntry.pimsmSGUpstreamNeighbor;
    if(pimsmJoinPruneSend(pimsmCb, &pJPData) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_ERROR,"Error sending Join/Prune Message");
    }
  }
  else
  {
    rc = pimsmStarGImdtOlist(pimsmCb, pStarGNode, &oif);
    if(rc != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_ERROR,
                  "pimsmStarGImdtOlist failed ");
      return L7_FAILURE;
    }
    BITX_IS_EMPTY(&oif, oifNull);
    if(oifNull == L7_TRUE)
    {
      pimsmRegisterStopSend(pimsmCb, rtrIfNum, pRegSrcAddr,
                            pInnerGrpAddr, pInnerSrcAddr);
      PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO,
                  "(*,G) oif is empty ");
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "pInnerGrpAddr :",pInnerGrpAddr);
      return L7_FAILURE;
    }
    else if(!nullRegisterBit)
    {
      pimsmStarGKernelCacheCountUpdate(pimsmCb, pStarGNode,
                                  pInnerSrcAddr, ipLen);
      if( pimsmCb->pimsmRegRateBytes == 0)
      {
         pimsmSwitchToShortestPath(pimsmCb, pInnerSrcAddr, pInnerGrpAddr,
            pStarGNode);
      }
      pimsmDecapsulateAndSend(pimsmCb->family, pimsmCb->sockFd, &oif, pIPPkt,
                              ipLen, ipTOS);
    }
  }
  return(L7_SUCCESS);
}
/******************************************************************************
*
* @purpose  Receive Register pkt processing (*,*,RP) part.
*
* @param    pimsmCb           @b{(input)}  control block
* @param    pRegSrcAddr       @b{(input)}  ip source address
* @param    pRegDestAddr      @b{(input)}  ip destination address
* @param    pIPPkt            @b{(input)}  start of ip pkt
* @param    ipLen               @b{(input)}  ip pkt length
* @param    rtrIfNum          @b{(input)}  incoming router interface number
* @param    pInnerSrcAddr     @b{(input)}  encoded source address
* @param    pInnerGrpAddr     @b{(input)}  encoded group address
* @param      borderBit         @b{(input)}  border bit
* @param      nullRegisterBit   @b{(input)}  null-register bit
* @param      pSGRptNode         @b{(input)}  (*,*,RP) node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t pimsmRegisterStarStarRpEntryProcessing(pimsmCB_t *pimsmCb,
               L7_inet_addr_t *pRegSrcAddr, L7_inet_addr_t *pRegDestAddr,
               L7_char8 *pIPPkt, L7_uint32 ipLen, L7_uint32 rtrIfNum,
               L7_inet_addr_t *pInnerSrcAddr, L7_inet_addr_t *pInnerGrpAddr,
               L7_uint32 borderBit, L7_uint32 nullRegisterBit, L7_uchar8 ipTOS,
               pimsmStarStarRpNode_t   *pStarStarRpNode)
{
  L7_BOOL oifNull;
  L7_uchar8 maskLen =0;
  interface_bitset_t  oif;
  L7_RC_t rc;
  pimsmSGEntry_t    *pSGEntry = L7_NULLPTR;
  pimsmSGNode_t   *pSGNode = L7_NULLPTR;
  pimsmSendJoinPruneData_t pJPData;
  pimsmStarGNode_t   *pStarGNode = L7_NULLPTR;
  L7_uint32 rtrIfNumSG;

  if(borderBit)
  {
    /* Create (S,G) state. The oifs will be the copied from the
     * existing (*,G) or (*,*,RP) entry. */
    rc = pimsmSGNodeCreate(pimsmCb, pInnerSrcAddr, pInnerGrpAddr, &pSGNode,L7_TRUE);
    if(rc != L7_SUCCESS ||
       pSGNode == (pimsmSGNode_t *) L7_NULLPTR)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_ERROR,
                  "pimsmSGNodeCreate failed ");
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "pGrpAddr :",pInnerGrpAddr);
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "pSrcAddr:",pInnerSrcAddr);
      return L7_FAILURE;
    }
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO,
                "border bit is set ");
    inetCopy(&pSGEntry->pimsmSGRPRegisterPMBRAddress, pRegSrcAddr);
    memset(&pJPData, 0, sizeof(pimsmSendJoinPruneData_t));
    rtrIfNumSG = pSGNode->pimsmSGEntry.pimsmSGRPFIfIndex;
    if(pimsmInterfaceJoinPruneHoldtimeGet(pimsmCb, rtrIfNumSG,
                                       &pJPData.holdtime)
                                             != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,
        PIMSM_TRACE_ERROR,"pimsmInterfaceJoinPruneHoldtimeGet failed");
      return L7_FAILURE;
    }
    pJPData.pSrcAddr = pRegSrcAddr;
    pimSrcMaskLenGet(pimsmCb->family,&maskLen);
    pJPData.srcAddrMaskLen = maskLen;
    pJPData.pGrpAddr= pRegDestAddr;
    pimGrpMaskLenGet(pimsmCb->family,&maskLen);
    pJPData.grpAddrMaskLen = maskLen;
    pJPData.addrFlags = 0;
    pJPData.rtrIfNum = rtrIfNumSG;
    pJPData.pNbrAddr = &pSGNode->pimsmSGEntry.pimsmSGUpstreamNeighbor;
    pJPData.joinOrPruneFlag = L7_TRUE;
    rc = pimsmJoinPruneSend(pimsmCb, &pJPData);
    if(rc != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_ERROR,"Error sending Join/Prune Message");
    }
  }
  else
  {
    /* TODO: No idea what to create : either (S,G) or (*,G).
     * The choice below is (*,G)
     */
    rc = pimsmStarGNodeCreate(pimsmCb, pInnerGrpAddr, &pStarGNode);
    if(rc != L7_SUCCESS ||
       pStarGNode == (pimsmStarGNode_t *) L7_NULLPTR)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_ERROR,
                  "pimsmStarGNodeCreate failed ");
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "pInnerGrpAddr :",pInnerGrpAddr);
      return L7_FAILURE;
    }

   /* TODO : let this be there, later remove if not needed*/
    rc = pimsmStarGImdtOlist(pimsmCb, pStarGNode, &oif);
    if(rc != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_ERROR,
                  "pimsmStarGImdtOlist failed ");
      return L7_FAILURE;
    }
    BITX_IS_EMPTY(&oif, oifNull);
    if(oifNull == L7_TRUE)
    {
      pimsmRegisterStopSend(pimsmCb, rtrIfNum, pRegSrcAddr,
                            pInnerGrpAddr, pInnerSrcAddr);
      PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO,
                  "(*,G) oif is empty ");
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "pInnerGrpAddr :",pInnerGrpAddr);
      return L7_FAILURE;
    }
    if(!nullRegisterBit)
    {
      pimsmStarGKernelCacheCountUpdate(pimsmCb, pStarGNode,
                                  pInnerSrcAddr, ipLen);
      pimsmDecapsulateAndSend(pimsmCb->family, pimsmCb->sockFd, &oif, pIPPkt,
                              ipLen, ipTOS);
    }
  }
  return(L7_SUCCESS);
}
/******************************************************************************
*
* @purpose  Receive Register pkt processing.
*
* @param    pimsmCb           @b{(input)}  control block
* @param    pRegSrcAddr       @b{(input)}  ip source address
* @param    pRegDestAddr      @b{(input)}  ip destination address
* @param    pPimHeader        @b{(input)}  start of pim pkt
* @param    pimPktLen         @b{(input)}  pim pkt length
* @param    rtrIfNum          @b{(input)}  incoming router interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t pimsmRegisterRecv(pimsmCB_t *pimsmCb,L7_inet_addr_t *pRegSrcAddr,
                                 L7_inet_addr_t *pRegDestAddr,
                                 L7_char8 *pPimHeader, L7_uint32 pimPktLen,
                                 L7_uint32 rtrIfNum, L7_uchar8 ipTypeOfService)
{
  L7_inet_addr_t innerSrcAddr ,  innerGrpAddr;
  L7_inet_addr_t  srcAddr, destAddr;
  L7_char8  *pData;
  L7_uint32 borderBit, nullRegisterBit;
  pimsmSGNode_t   * pSGNode= L7_NULLPTR;
  pimsmSGRptNode_t * pSGRptNode = L7_NULLPTR;
  pimsmStarGNode_t    * pStarGNode= L7_NULLPTR;
  pimsmStarStarRpNode_t   * pStarStarRpNode= L7_NULLPTR;
  L7_uint32 regFlags;
  L7_RC_t rc;
  pimsmLongestMatch_t pimsmMRTNode;
  L7_inet_addr_t  rpAddr;
  L7_uint32 pktLen = 0;
  L7_BOOL retVal;


  PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "rtrIfNum = %d", rtrIfNum);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "pRegSrcAddr :", pRegSrcAddr);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "pRegDestAddr :", pRegDestAddr);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "length = %d", pimPktLen);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "payLoad = %p", pPimHeader);

  pData = (L7_char8 *)(pPimHeader + PIM_HEARDER_SIZE);

  MCAST_GET_LONG(regFlags, pData);
  regFlags = osapiNtohl(regFlags);
  borderBit = regFlags & PIMSM_MESSAGE_REGISTER_BORDER_BIT;

  nullRegisterBit = regFlags & PIMSM_MESSAGE_REGISTER_NULL_REGISTER_BIT;


  /* Message length validation.    */
  rc = pimsmMessageLengthValidate(pimsmCb->family, pimPktLen);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_ERROR, "Message length validation failed");
    return L7_FAILURE;
  }

  if(mcastIpMapUnnumberedIsLocalAddress(pRegDestAddr, L7_NULLPTR) != L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_ERROR, "Register msg is not targetted to me.");
    return(L7_FAILURE);
  }

  if(pimsmSourceAndGroupAddressGet(pimsmCb->family,
                                   (L7_uchar8 *)pData,
                                   &srcAddr,&destAddr,
                                   &pktLen) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_ERROR,"pimsmSourceAndGroupAddressGet() failed");
    return L7_FAILURE;
  }

  inetAddrNtoh(&srcAddr, &innerSrcAddr);
  inetAddrNtoh(&destAddr, &innerGrpAddr);

  /*
   * innerSrcAddr and innerGrpAddr must be valid IP unicast and multicast
   * address respectively. .
   */

  if(inetIsValidHostAddress(&innerSrcAddr) == L7_FALSE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_ERROR,
                "Inner source address of register message is invalid:");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "innerSrcAddr :",&innerSrcAddr);
    pimsmRegisterStopSend(pimsmCb, rtrIfNum, pRegSrcAddr, &innerGrpAddr,
      &innerSrcAddr);
    return L7_FAILURE;
  }
  if(inetIsInMulticast(&innerGrpAddr) == L7_FALSE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_ERROR,
                "Inner pGrpAddr address of register message is invalid");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "innerGrpAddr :",&innerGrpAddr);
    pimsmRegisterStopSend(pimsmCb, rtrIfNum, pRegSrcAddr, &innerGrpAddr,
      &innerSrcAddr);
    return L7_FAILURE;
  }

  /*  Don't process register frames in the SSM range */
  retVal = pimsmMapIsInSsmRange(pimsmCb->family, &innerGrpAddr);
  if(retVal == L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_ERROR,"SSM Range Addr. ");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "innerGrpAddr :",&innerGrpAddr);
    pimsmRegisterStopSend(pimsmCb, rtrIfNum, pRegSrcAddr, &innerGrpAddr,
      &innerSrcAddr);
    return L7_FAILURE;
  }

  rc = pimsmRpAddressGet(pimsmCb, &innerGrpAddr, &rpAddr);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_ERROR,
                "pimsmRpAddressGet failed  ");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "innerGrpAddr :",&innerGrpAddr);
    pimsmRegisterStopSend(pimsmCb, rtrIfNum, pRegSrcAddr, &innerGrpAddr,
      &innerSrcAddr);
    return L7_FAILURE;
  }

  /* Following implements the comments

    if( I_am_RP(G) AND outer.dst == RP(G) ) {
           ....

     else {
          send Register-Stop(S,G) to outer.src
          # Note (*)
      }

  */
  if((mcastIpMapUnnumberedIsLocalAddress(&rpAddr, L7_NULLPTR) != L7_TRUE) ||
     (L7_INET_IS_ADDR_EQUAL(&rpAddr, pRegDestAddr) != L7_TRUE))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_ERROR,
                "I am not RP or recv'ed RP is not RP for the pGrpAddr ");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "innerGrpAddr :",&innerGrpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, " recv'ed RP Addr :",pRegDestAddr);
    pimsmRegisterStopSend(pimsmCb, rtrIfNum, pRegSrcAddr, &innerGrpAddr,
      &innerSrcAddr);
    return L7_FAILURE;
  }

  rc = pimsmMRTFind(pimsmCb, &innerGrpAddr, &innerSrcAddr, &pimsmMRTNode);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_ERROR,
                "pimsmMRTFind failed");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "innerGrpAddr :",&innerGrpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "innerSrcAddr :",&innerSrcAddr);
    pimsmRegisterStopSend(pimsmCb, rtrIfNum, pRegSrcAddr, &innerGrpAddr,
      &innerSrcAddr);
    return L7_FAILURE;
  }

  switch(pimsmMRTNode.entryType)
  {
    case PIMSM_ENTRY_TYPE_S_G:
      pSGNode = (pimsmSGNode_t*)pimsmMRTNode.mrtNode;
      rc = pimsmRegisterSGEntryProcessing(pimsmCb, pRegSrcAddr, pRegDestAddr,
             pData, pktLen, rtrIfNum, &innerSrcAddr, &innerGrpAddr,
             borderBit, nullRegisterBit, ipTypeOfService, pSGNode);
      break;
    case PIMSM_ENTRY_TYPE_S_G_RPT:
      pSGRptNode = (pimsmSGRptNode_t*)pimsmMRTNode.mrtNode;
      rc = pimsmRegisterSGRptEntryProcessing(pimsmCb, pRegSrcAddr, pRegDestAddr,
             pData, pktLen, rtrIfNum, &innerSrcAddr, &innerGrpAddr,
             borderBit, nullRegisterBit, pSGRptNode);
      break;
    case PIMSM_ENTRY_TYPE_STAR_G:
      pStarGNode = (pimsmStarGNode_t*)pimsmMRTNode.mrtNode;
      rc = pimsmRegisterStarGEntryProcessing(pimsmCb, pRegSrcAddr, pRegDestAddr,
             pData, pktLen, rtrIfNum, &innerSrcAddr, &innerGrpAddr,
             borderBit, nullRegisterBit, ipTypeOfService, pStarGNode);
      break;
    case PIMSM_ENTRY_TYPE_STAR_STAR_RP:
      pStarStarRpNode = (pimsmStarStarRpNode_t*)pimsmMRTNode.mrtNode;
      rc = pimsmRegisterStarStarRpEntryProcessing(pimsmCb, pRegSrcAddr, pRegDestAddr,
             pData, pktLen, rtrIfNum, &innerSrcAddr, &innerGrpAddr,
             borderBit, nullRegisterBit, ipTypeOfService, pStarStarRpNode);
      break;
    case PIMSM_ENTRY_TYPE_NONE:
    default:
      PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_ERROR,"no entry exists for group =");
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "innerGrpAddr :",&innerGrpAddr);
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "innerSrcAddr :",&innerSrcAddr);
      pimsmRegisterStopSend(pimsmCb, rtrIfNum, pRegSrcAddr, &innerGrpAddr,
         &innerSrcAddr);
      rc = L7_FAILURE;
  }
  return rc;
}

/******************************************************************************
*
* @purpose  Receive join/prune pkt upstream (*,*,RP) processing part
*
* @param    pimsmCb         @b{(input)}  control block
* @param    pData           @b{(input)} join/prune msg part start
* @param    pGrpAddr        @b{(input)}  group address
* @param    holdtime        @b{(input)} holdtime
* @param    numJoinSrcs     @b{(input)} number of joined sources in the msg
* @param    numPruneSrcs    @b{(input)} number of pruned sources in the msg
* @param    adminScopedGrp  @b{(input)} is adminscoped group
* @param    rtrIfNum        @b{(input)} incoming router interface number
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t pimsmUpStrmStarStarRpProcessing(pimsmCB_t *pimsmCb,
         L7_char8 *pData,
         L7_inet_addr_t *pGrpAddr, L7_short16 holdtime,
         L7_short16 numJoinSrcs, L7_short16 numPruneSrcs,
         L7_BOOL adminScopedGrp,
         L7_uint32 rtrIfNum)
{

  L7_char8 srcFlags = L7_NULL;
  L7_BOOL joinDesired;
  pim_encod_src_addr_t  encodSrcAddr;
  pimsmStarStarRpNode_t   * pStarStarRpNode= L7_NULLPTR;
  pimsmUpStrmStarStarRPEventInfo_t upStrmStarStarRPEventInfo;
  L7_RC_t rc;
  L7_inet_addr_t rpAddr;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Entry");

  memset(&encodSrcAddr, 0, sizeof(pim_encod_src_addr_t));
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
              "Start Processing Join Part");
  /* (*,*,RP) Join */
  while(numJoinSrcs--)
  {
    /* ignore all joins for administratively scoped groups */
    if(adminScopedGrp == L7_TRUE)
    {
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
         "Ignoring AdminScope addr :",pGrpAddr);
      break;
    }

    PIM_GET_ESADDR_INET(&encodSrcAddr, pData);
    inetAddressSet(encodSrcAddr.addr_family,
                   &(encodSrcAddr.addr),&rpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "rpAddr :",&rpAddr);

    if(inetIsValidHostAddress(&rpAddr) == L7_FALSE)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Invalid host addr");
      continue;
    }
    srcFlags = encodSrcAddr.flags;
    /*inetMaskLenToMask(encodSrcAddr.masklen, s_mask); -Remove*/

    if((srcFlags & PIMSM_ADDR_RP_BIT) &&
       (srcFlags & PIMSM_ADDR_WC_BIT))
    {
      /* (*,*,RP) see join*/
      PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Received (*,*,RP) join");
      rc = pimsmStarStarRpFind(pimsmCb, &rpAddr, &pStarStarRpNode);
      if(rc != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                    "pimsmStarStarRpFind failed ");
        continue;
      }
      if(pStarStarRpNode->pimsmStarStarRpEntry.pimsmStarStarRpRPFIfIndex ==
         rtrIfNum)
      {
        memset(&upStrmStarStarRPEventInfo, 0 ,
               sizeof(pimsmUpStrmStarStarRPEventInfo_t));
        upStrmStarStarRPEventInfo.eventType =
        PIMSM_UPSTRM_STAR_STAR_RP_SM_EVENT_SEE_JOIN;
        upStrmStarStarRPEventInfo.holdtime = holdtime;
        pimsmUpStrmStarStarRpExecute(pimsmCb, pStarStarRpNode,
                                     &upStrmStarStarRPEventInfo);
      }
      memset(&upStrmStarStarRPEventInfo, 0 ,
             sizeof(pimsmUpStrmStarStarRPEventInfo_t));
      joinDesired = pimsmStarStarRpJoinDesired(pimsmCb, pStarStarRpNode);
      if(joinDesired == L7_TRUE)
      {
        upStrmStarStarRPEventInfo.eventType =
        PIMSM_UPSTRM_STAR_STAR_RP_SM_EVENT_JOIN_DESIRED_TRUE;

      }
      else
      {
        upStrmStarStarRPEventInfo.eventType =
        PIMSM_UPSTRM_STAR_STAR_RP_SM_EVENT_JOIN_DESIRED_FALSE;
      }
      pimsmUpStrmStarStarRpExecute(pimsmCb,
                                   pStarStarRpNode,
                                   &upStrmStarStarRPEventInfo);
    }
  } /* numJoinSrcs */
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
              "Start Processing Prune Part");
  while(numPruneSrcs--)
  {
    /* either suppress the local (*,*,RP) prunes or override the prunes by
     * sending (*,*,RP) and/or (*,G) and/or (S,G) Join.
     */
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Received (*,*,RP) prune");
    PIM_GET_ESADDR_INET(&encodSrcAddr, pData);
    inetAddressSet(encodSrcAddr.addr_family,
                   &(encodSrcAddr.addr),&rpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "rpAddr :",&rpAddr);
    if(inetIsValidHostAddress(&rpAddr) == L7_FALSE)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Invalid host addr ");
      continue;
    }
    srcFlags = encodSrcAddr.flags;
    /*inetMaskLenToMask(encodSrcAddr.masklen, s_mask); -Remove*/
    if((srcFlags & PIMSM_ADDR_RP_BIT) &&
       (srcFlags & PIMSM_ADDR_WC_BIT))/* This is the RP address. */
    {
      rc = pimsmStarStarRpFind(pimsmCb, &rpAddr, &pStarStarRpNode);
      if(rc != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                    "pimsmStarStarRpFind failed - (*,*,RP) not found");
        continue;
      }
      if(pStarStarRpNode->pimsmStarStarRpEntry.pimsmStarStarRpRPFIfIndex ==
         rtrIfNum)
      {
        memset(&upStrmStarStarRPEventInfo, 0 ,
               sizeof(pimsmUpStrmStarStarRPEventInfo_t));
        upStrmStarStarRPEventInfo.eventType =
        PIMSM_UPSTRM_STAR_STAR_RP_SM_EVENT_SEE_PRUNE;
        pimsmUpStrmStarStarRpExecute(pimsmCb, pStarStarRpNode,
                                     &upStrmStarStarRPEventInfo);
      }
    }
  } /* numPruneSrcs */
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Exit");
  return L7_SUCCESS;
}

/******************************************************************************
*
* @purpose Receive join/prune pkt upstream (*,G) Join processing part
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pSrcAddr    @b{(input)}  source address
* @param    pGrpAddr    @b{(input)}  group address
* @param    holdtime    @b{(input)} holdtime
* @param    rtrIfNum    @b{(input)} incoming router interface number
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t pimsmUpStrmStarGJoinProcessing(pimsmCB_t *pimsmCb,
           L7_inet_addr_t *pSrcAddr, L7_inet_addr_t *pGrpAddr,
           L7_short16 holdtime,
           L7_uint32 rtrIfNum)
{
  pimsmStarGNode_t        * pStarGNode= L7_NULLPTR;
  pimsmStarGEntry_t *pStarGEntry;
  pimsmUpStrmStarGEventInfo_t  upStrmStarGEventInfo;
  L7_RC_t rc;
  L7_BOOL joinDesired;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Entry");

  /* (*,G) see JOIN  (toward the RP) */
  rc = pimsmStarGFind(pimsmCb, pGrpAddr, &pStarGNode);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                "pimsmStarGFind failed ");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pGrpAddr :",pGrpAddr);
    return L7_FAILURE;
  }
  pStarGEntry = &pStarGNode->pimsmStarGEntry;

  if(pStarGEntry->pimsmStarGRPFIfIndex == rtrIfNum)
  {
    memset(&upStrmStarGEventInfo, 0 ,
           sizeof(pimsmUpStrmStarGEventInfo_t));
    upStrmStarGEventInfo.eventType =
    PIMSM_UPSTRM_STAR_G_SM_EVENT_SEE_JOIN;
    upStrmStarGEventInfo.holdtime = holdtime;
    pimsmUpStrmStarGExecute(pimsmCb,
                            pStarGNode,
                            &upStrmStarGEventInfo);
  }
  memset(&upStrmStarGEventInfo, 0 ,
         sizeof(pimsmUpStrmStarGEventInfo_t));

  joinDesired = pimsmStarGJoinDesired(pimsmCb, pStarGNode);
  if(joinDesired == L7_TRUE)
  {
    upStrmStarGEventInfo.eventType =
    PIMSM_UPSTRM_STAR_G_SM_EVENT_JOIN_DESIRED_TRUE;
  }
  else
  {
    upStrmStarGEventInfo.eventType =
    PIMSM_UPSTRM_STAR_G_SM_EVENT_JOIN_DESIRED_FALSE;
  }
  pimsmUpStrmStarGExecute(pimsmCb, pStarGNode,
                          &upStrmStarGEventInfo);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Exit");
  return L7_SUCCESS;
}

/******************************************************************************
*
* @purpose  Receive join/prune pkt upstream (S,G) Join processing part
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pSrcAddr    @b{(input)}  source address
* @param    pGrpAddr    @b{(input)}  group address
* @param    holdtime    @b{(input)} holdtime
* @param    rtrIfNum    @b{(input)} incoming router interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t pimsmUpStrmSGJoinProcessing(pimsmCB_t *pimsmCb,
         L7_inet_addr_t *pSrcAddr, L7_inet_addr_t *pGrpAddr,
         L7_short16 holdtime, L7_uint32 rtrIfNum)
{
  pimsmSGNode_t       * pSGNode= L7_NULLPTR;
  pimsmUpStrmSGEventInfo_t upStrmSGEventInfo;
  L7_RC_t rc;
  L7_BOOL joinDesired;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Entry");
  rc = pimsmSGFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGNode);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                "pimsmSGFind failed");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pGrpAddr :",pGrpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pSrcAddr :",pSrcAddr);
    return L7_FAILURE;
  }
  if(pSGNode->pimsmSGEntry.pimsmSGRPFIfIndex == rtrIfNum)
  {
    memset(&upStrmSGEventInfo, 0 ,
           sizeof(pimsmUpStrmSGEventInfo_t));
    upStrmSGEventInfo.eventType
    = PIMSM_UPSTRM_S_G_SM_EVENT_SEE_JOIN;
    upStrmSGEventInfo.holdtime = holdtime;
    pimsmUpStrmSGExecute(pimsmCb, pSGNode, &upStrmSGEventInfo);
  }
  memset(&upStrmSGEventInfo, 0 ,
         sizeof(pimsmUpStrmSGEventInfo_t));
  joinDesired = pimsmSGJoinDesired(pimsmCb, pSGNode);
  if(joinDesired == L7_TRUE)
  {
    upStrmSGEventInfo.eventType
    = PIMSM_UPSTRM_S_G_SM_EVENT_JOIN_DESIRED_TRUE;
  }
  else
  {
    upStrmSGEventInfo.eventType
    = PIMSM_UPSTRM_S_G_SM_EVENT_JOIN_DESIRED_FALSE;
  }
  pimsmUpStrmSGExecute(pimsmCb, pSGNode, &upStrmSGEventInfo);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Exit");
  return L7_SUCCESS;
}

/******************************************************************************
*
* @purpose  Receive join/prune pkt upstream (S,G,rpt) Join processing part
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pSrcAddr    @b{(input)}  source address
* @param    pGrpAddr    @b{(input)}  group address
* @param    holdtime    @b{(input)} holdtime
* @param    rtrIfNum    @b{(input)} incoming router interface number
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t pimsmUpStrmSGRptJoinProcessing(pimsmCB_t *pimsmCb,
             L7_inet_addr_t *pSrcAddr, L7_inet_addr_t *pGrpAddr,
             L7_short16 holdtime,
             L7_uint32 rtrIfNum)
{
  pimsmSGRptNode_t *    pSGRptNode= L7_NULLPTR;
  pimsmUpStrmSGRptEventInfo_t upStrmSGRptEventInfo;
  L7_RC_t rc;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Entry");
  rc = pimsmSGRptFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGRptNode);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                "pimsmSGRptFind failed ");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pGrpAddr :",pGrpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pSrcAddr :",pSrcAddr);
    return L7_FAILURE;
  }
  if(pSGRptNode->pimsmSGRptEntry.pimsmSGRptRPFIfIndex == rtrIfNum)
  {
    memset(&upStrmSGRptEventInfo, 0 ,
           sizeof(pimsmUpStrmSGRptEventInfo_t));
    upStrmSGRptEventInfo.eventType =
    PIMSM_UPSTRM_S_G_RPT_SM_EVENT_SEE_JOIN;
    upStrmSGRptEventInfo.holdtime = holdtime;
    pimsmUpStrmSGRptExecute(pimsmCb, pSGRptNode,
                            &upStrmSGRptEventInfo);
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Exit");
  return L7_SUCCESS;
}

/******************************************************************************
*
* @purpose  Receive join/prune pkt upstream (*,G) Prune processing part
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pSrcAddr    @b{(input)}  source address
* @param    pGrpAddr    @b{(input)}  group address
* @param    rtrIfNum    @b{(input)} incoming router interface number
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t pimsmUpStrmStarGPruneProcessing(pimsmCB_t *pimsmCb,
           L7_inet_addr_t *pSrcAddr, L7_inet_addr_t *pGrpAddr,
           L7_uint32 rtrIfNum)
{
  pimsmStarGNode_t        * pStarGNode= L7_NULLPTR;
  pimsmStarGEntry_t *pStarGEntry;
  pimsmUpStrmStarGEventInfo_t  upStrmStarGEventInfo;
  L7_RC_t rc;
  pimsmSGNode_t       * pSGNode = L7_NULLPTR;
  pimsmUpStrmSGEventInfo_t upStrmSGEventInfo;
  L7_BOOL couldAssert;
  pimsmPerIntfStarGAssertEventInfo_t  starGAssertEventInfo;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Entry");

  rc = pimsmStarGFind(pimsmCb, pGrpAddr, &pStarGNode);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                "pimsmStarGFind failed ");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pGrpAddr :",pGrpAddr);
    return L7_FAILURE;
  }
  pStarGEntry = &pStarGNode->pimsmStarGEntry;

  if(pStarGEntry->pimsmStarGRPFIfIndex == rtrIfNum)
  {
    memset(&upStrmStarGEventInfo, 0 ,
           sizeof(pimsmUpStrmStarGEventInfo_t));
    upStrmStarGEventInfo.eventType =
    PIMSM_UPSTRM_STAR_G_SM_EVENT_SEE_PRUNE;
    pimsmUpStrmStarGExecute(pimsmCb,
                            pStarGNode,
                            &upStrmStarGEventInfo);
  }
  memset(&starGAssertEventInfo, 0, sizeof(pimsmPerIntfStarGAssertEventInfo_t));
  starGAssertEventInfo.rtrIfNum = rtrIfNum;
  couldAssert = pimsmStarGICouldAssert(pimsmCb, pStarGNode, rtrIfNum);
  if(couldAssert == L7_FALSE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
       "(couldAssert == false)");
    starGAssertEventInfo.eventType =
    PIMSM_ASSERT_STAR_G_SM_EVENT_COULD_ASSERT_FALSE;
    inetAddressZeroSet(pimsmCb->family,&starGAssertEventInfo.srcAddr);
    pimsmPerIntfStarGAssertExecute(pimsmCb, pStarGNode,&starGAssertEventInfo);
  }
  else
  {
     PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
       "(couldAssert == True) ");
  }
  rc = pimsmSGFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGNode);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                "pimsmSGFind failed");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pGrpAddr :",pGrpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pSrcAddr :",pSrcAddr);
    return L7_FAILURE;
  }
  if(pSGNode->pimsmSGEntry.pimsmSGRPFIfIndex == rtrIfNum)
  {
    memset(&upStrmSGEventInfo, 0 ,
           sizeof(pimsmUpStrmSGEventInfo_t));
    upStrmSGEventInfo.eventType
    = PIMSM_UPSTRM_S_G_SM_EVENT_SEE_PRUNE_STAR_G;
    pimsmUpStrmSGExecute(pimsmCb, pSGNode, &upStrmSGEventInfo);
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Exit");
  return L7_SUCCESS;
}

/******************************************************************************
*
* @purpose Receive join/prune pkt upstream (S,G) Prune processing part
*
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pSrcAddr    @b{(input)}  source address
* @param    pGrpAddr    @b{(input)}  group address
* @param    rtrIfNum    @b{(input)} incoming router interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t pimsmUpStrmSGPruneProcessing(pimsmCB_t *pimsmCb,
                L7_inet_addr_t *pSrcAddr, L7_inet_addr_t *pGrpAddr,
                L7_uint32 rtrIfNum)
{
  pimsmSGNode_t       * pSGNode= L7_NULLPTR;
  pimsmUpStrmSGEventInfo_t upStrmSGEventInfo;
  L7_RC_t rc;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Entry");
  rc = pimsmSGFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGNode);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                "pimsmSGFind failed");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pGrpAddr :",pGrpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pSrcAddr :",pSrcAddr);
    return L7_FAILURE;
  }
  if(pSGNode->pimsmSGEntry.pimsmSGRPFIfIndex == rtrIfNum)
  {
    memset(&upStrmSGEventInfo, 0 ,
           sizeof(pimsmUpStrmSGEventInfo_t));
    upStrmSGEventInfo.eventType
    = PIMSM_UPSTRM_S_G_SM_EVENT_SEE_PRUNE_S_G;
    pimsmUpStrmSGExecute(pimsmCb, pSGNode, &upStrmSGEventInfo);
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Exit");
  return L7_SUCCESS;
}

/******************************************************************************
*
* @purpose  Receive join/prune pkt upstream (S,G,rpt) Prune processing part
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pSrcAddr    @b{(input)}  source address
* @param    pGrpAddr    @b{(input)}  group address
* @param    rtrIfNum    @b{(input)} incoming router interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t pimsmUpStrmSGRptPruneProcessing(pimsmCB_t *pimsmCb,
                L7_inet_addr_t *pSrcAddr, L7_inet_addr_t *pGrpAddr,
                L7_uint32 rtrIfNum)
{
  pimsmSGRptNode_t *    pSGRptNode= L7_NULLPTR;
  pimsmUpStrmSGRptEventInfo_t upStrmSGRptEventInfo;
  L7_RC_t rc;
  pimsmSGNode_t       * pSGNode= L7_NULLPTR;
  pimsmUpStrmSGEventInfo_t upStrmSGEventInfo;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Entry");

  rc = pimsmSGRptFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGRptNode);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                "pimsmSGRptFind failed");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pGrpAddr :",pGrpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pSrcAddr :",pSrcAddr);
    return L7_FAILURE;
  }
  if(pSGRptNode->pimsmSGRptEntry.pimsmSGRptRPFIfIndex == rtrIfNum)
  {
    memset(&upStrmSGRptEventInfo, 0 ,
           sizeof(pimsmUpStrmSGRptEventInfo_t));
    upStrmSGRptEventInfo.eventType =
    PIMSM_UPSTRM_S_G_RPT_SM_EVENT_SEE_PRUNE;
    pimsmUpStrmSGRptExecute(pimsmCb, pSGRptNode,
                            &upStrmSGRptEventInfo);
  }
  rc = pimsmSGFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGNode);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                "pimsmSGFind failed");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pGrpAddr :",pGrpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pSrcAddr :",pSrcAddr);
    return L7_FAILURE;
  }
  if(pSGNode->pimsmSGEntry.pimsmSGRPFIfIndex == rtrIfNum)
  {
    memset(&upStrmSGEventInfo, 0 ,
           sizeof(pimsmUpStrmSGEventInfo_t));
    upStrmSGEventInfo.eventType
    = PIMSM_UPSTRM_S_G_SM_EVENT_SEE_PRUNE_S_G_RPT;
    pimsmUpStrmSGExecute(pimsmCb, pSGNode, &upStrmSGEventInfo);
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Exit");
  return L7_SUCCESS;
}

/******************************************************************************
*
* @purpose  Receive join/prune pkt downstream (*,*,RP) processing part
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pData           @b{(input)} join/prune msg part start
* @param    pGrpAddr        @b{(input)}  group address
* @param    rtrIfNum        @b{(input)} incoming router interface number
* @param    holdtime        @b{(input)} holdtime
* @param    numJoinSrcs     @b{(input)} number of joined sources in the msg
* @param    numPruneSrcs    @b{(input)} number of pruned sources in the msg
* @param    adminScopedGrp  @b{(input)} is adminscoped group
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t pimsmDnStrmStarStarRpProcessing(pimsmCB_t *pimsmCb,
   L7_char8 *pData,
   L7_inet_addr_t *pGrpAddr,  L7_uint32 rtrIfNum, L7_short16 holdtime,
   L7_short16 numJoinSrcs, L7_short16 numPruneSrcs,
   L7_BOOL adminScopedGrp)
{
  pim_encod_src_addr_t  encodSrcAddr;
  pimsmStarStarRpNode_t   * pStarStarRpNode = L7_NULLPTR;
  pimsmDnStrmPerIntfStarStarRPEventInfo_t dnStrmStarStarRpEventInfo;
  pimsmUpStrmStarStarRPEventInfo_t upStrmStarStarRPEventInfo;
  L7_RC_t rc;
  L7_inet_addr_t rpAddr, grpAddrH;
  L7_BOOL retVal,joinDesired;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Entry");

  memset(&encodSrcAddr, 0 ,sizeof(pim_encod_uni_addr_t));
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
              "Start Processing Join Part");
  /* (*,*,RP) found */
  while(numJoinSrcs--)
  {
    /* ignore all joins for administratively scoped groups */
    if(adminScopedGrp == L7_TRUE)
    {
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                       "Ignoring AdminScope addr = :",pGrpAddr);
      break;
    }

    PIM_GET_ESADDR_INET(&encodSrcAddr, pData);
    inetAddressSet(encodSrcAddr.addr_family,
                   &(encodSrcAddr.addr),&rpAddr);
    /*inetMaskLenToMask(encodSrcAddr.masklen, s_mask); -Remove*/

    if(inetIsValidHostAddress(&rpAddr) == L7_FALSE)
    {
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                       "Invalid host addr =  :",&rpAddr);
      continue;
    }
    /* Don't create (*,*,RP) for SSM range */
    inetAddrNtoh(pGrpAddr, &grpAddrH);\
    retVal = pimsmMapIsInSsmRange(pimsmCb->family, &grpAddrH);
    if(retVal == L7_TRUE)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                  "IN SSM RANGE ");
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pGrpAddr :",&grpAddrH);
      continue;
    }
    rc = pimsmStarStarRpNodeCreate(pimsmCb, &rpAddr, &pStarStarRpNode);
    if(rc != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                  "pimsmStarStarRpNodeCreate failed \n");
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "rpAddr :",&rpAddr);
      continue;
    }
    /*delete negative entries from MFC*/
    if((pStarStarRpNode->flags & PIMSM_NULL_OIF) != L7_NULL)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION,  PIMSM_TRACE_ERROR,
                   "deleting MFC entry" );
      if (pimsmStarStarRpMFCUpdate(pimsmCb,pStarStarRpNode, MFC_DELETE_ENTRY, L7_FALSE)
          != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
             "Failed to delete (*,*,RP) from MFC");
      }
      else
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
          "Negative (*,*,RP) MFC Entry deleted");
      }
    }
    pimsmStarGNegativeDelete(pimsmCb,
      &pStarStarRpNode->pimsmStarStarRpEntry.pimsmStarStarRpRPAddress);
    rc = pimsmStarStarRpIEntryCreate(pimsmCb, pStarStarRpNode, rtrIfNum);
    if(rc != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                  "pimsmStarStarRpIEntryCreate failed - rtrIfNum = %d\n",
                  rtrIfNum); continue;
    }
    memset(&dnStrmStarStarRpEventInfo, 0 ,
           sizeof(pimsmDnStrmPerIntfStarStarRPEventInfo_t));
    dnStrmStarStarRpEventInfo.eventType =
    PIMSM_DNSTRM_STAR_STAR_RP_SM_EVENT_RECV_JOIN;
    dnStrmStarStarRpEventInfo.rtrIfNum = rtrIfNum;
    dnStrmStarStarRpEventInfo.holdtime = holdtime;
    pimsmDnStrmPerIntfStarStarRPExecute(pimsmCb, pStarStarRpNode,
                                          &dnStrmStarStarRpEventInfo);

    if(pimsmStarStarRpMFCUpdate(pimsmCb,pStarStarRpNode, MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,"pimsmStarStarRpMFCUpdate failed");
      continue;
    }
    memset(&upStrmStarStarRPEventInfo, 0 ,
           sizeof(pimsmUpStrmStarStarRPEventInfo_t));
    joinDesired = pimsmStarStarRpJoinDesired(pimsmCb, pStarStarRpNode);
    if (joinDesired == L7_TRUE)
    {
      upStrmStarStarRPEventInfo.eventType =
      PIMSM_UPSTRM_STAR_STAR_RP_SM_EVENT_JOIN_DESIRED_TRUE;

    }
    else
    {
      upStrmStarStarRPEventInfo.eventType =
      PIMSM_UPSTRM_STAR_STAR_RP_SM_EVENT_JOIN_DESIRED_FALSE;
    }
    pimsmUpStrmStarStarRpExecute(pimsmCb,
                                 pStarStarRpNode, &upStrmStarStarRPEventInfo);
    pimsmStarGTreeStarStarRpJoinRecvUpdate(pimsmCb, &rpAddr, rtrIfNum);
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
              "Start Processing Prune Part");
  while(numPruneSrcs--)
  {
    PIM_GET_ESADDR_INET(&encodSrcAddr, pData);
    inetAddressSet(encodSrcAddr.addr_family,
                   &(encodSrcAddr.addr),&rpAddr);
    if(inetIsValidHostAddress(&rpAddr) == L7_FALSE)
    {
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Invalid host addr  :",&rpAddr);
      continue;
    }
    rc = pimsmStarStarRpFind(pimsmCb, &rpAddr, &pStarStarRpNode);
    if(rc != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                  "pimsmStarStarRpFind failed ");
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "rpAddr :",&rpAddr);
      continue;
    }
    rc = pimsmStarStarRpIEntryCreate(pimsmCb, pStarStarRpNode, rtrIfNum);
    if(rc != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                  "pimsmStarStarRpIEntryCreate failed - rtrIfNum = %d",
                  rtrIfNum);
      continue;
    }

    memset(&dnStrmStarStarRpEventInfo, 0 ,
           sizeof(pimsmDnStrmPerIntfStarStarRPEventInfo_t));
    dnStrmStarStarRpEventInfo.eventType =
    PIMSM_DNSTRM_STAR_STAR_RP_SM_EVENT_RECV_PRUNE;
    dnStrmStarStarRpEventInfo.rtrIfNum = rtrIfNum;
    pimsmDnStrmPerIntfStarStarRPExecute(pimsmCb, pStarStarRpNode,
                                          &dnStrmStarStarRpEventInfo);


    memset(&upStrmStarStarRPEventInfo, 0 ,
           sizeof(pimsmUpStrmStarStarRPEventInfo_t));
    joinDesired = pimsmStarStarRpJoinDesired(pimsmCb, pStarStarRpNode);
    if (joinDesired == L7_TRUE)
    {
      upStrmStarStarRPEventInfo.eventType =
      PIMSM_UPSTRM_STAR_STAR_RP_SM_EVENT_JOIN_DESIRED_TRUE;

    }
    else
    {
      upStrmStarStarRPEventInfo.eventType =
      PIMSM_UPSTRM_STAR_STAR_RP_SM_EVENT_JOIN_DESIRED_FALSE;
    }
    pimsmUpStrmStarStarRpExecute(pimsmCb,
                                 pStarStarRpNode,
                                 &upStrmStarStarRPEventInfo);
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Exit");
  return L7_SUCCESS;
}

/******************************************************************************
*
* @purpose  Receive join/prune pkt downstream (*,G) Join processing part
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pSrcAddr    @b{(input)}  source address
* @param    pGrpAddr    @b{(input)}  group address
* @param    rtrIfNum    @b{(input)} incoming router interface number
* @param    holdtime    @b{(input)} holdtime
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t pimsmDnStrmStarGJoinProcessing(pimsmCB_t *pimsmCb,
       L7_inet_addr_t *pSrcAddr, L7_inet_addr_t *pGrpAddr,
       L7_uint32 rtrIfNum,
       L7_short16 holdtime)
{
  pimsmStarGNode_t        * pStarGNode = L7_NULLPTR;
  L7_RC_t rc;
  pimsmDnStrmPerIntfStarGEventInfo_t dnStrmPerIntfStarGEventInfo;
  pimsmUpStrmStarGEventInfo_t  upStrmStarGEventInfo;
  L7_BOOL joinDesired;
  pimsmPerIntfStarGAssertEventInfo_t  starGAssertEventInfo;


  rc = pimsmStarGNodeCreate(pimsmCb, pGrpAddr, &pStarGNode);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                "pimsmStarGNodeCreate failed");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pGrpAddr :",pGrpAddr);
    return L7_FAILURE;
  }
  if (mcastMapProtocolIsAdminScopedAddress(pimsmCb->family,
                                           pStarGNode->pimsmStarGEntry.pimsmStarGRPFIfIndex,
                                           pGrpAddr) == L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,
                "admin-scope configured on Upstream ");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO, "pGrpAddr :",pGrpAddr);
    return L7_FAILURE;
  }

  /*delete negative entries from MFC*/
  if((pStarGNode->flags & PIMSM_NULL_OIF) != L7_NULL)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION,  PIMSM_TRACE_ERROR,
                 "deleting MFC entry" );

    if (pimsmStarGMFCUpdate(pimsmCb,pStarGNode, MFC_DELETE_ENTRY, L7_FALSE)
        != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
           "Failed to delete (*,G) from MFC");
    }
    else
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
        "Negative (*,G) MFC Entry deleted");
    }
  }
  /*Note that (S,G) and (S,G,rpt) MFC updates are done below,
    Hence no need to delete negative entries*/
  /*pimsmSGNegativeDelete(pimsmCb,
      &pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress); */
  pimsmSGRptNegativeDelete(pimsmCb,
      &pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress);
  pimsmStarStarRpNegativeDelete (pimsmCb,
      &pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress);
  rc = pimsmStarGIEntryCreate(pimsmCb, pStarGNode, rtrIfNum);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                "pimsmStarGIEntryCreate failed - rtrIfNum = %d ", rtrIfNum);
    return L7_FAILURE;
  }

  memset(&dnStrmPerIntfStarGEventInfo, 0 ,
         sizeof(pimsmDnStrmPerIntfStarGEventInfo_t));
  dnStrmPerIntfStarGEventInfo.eventType
  = PIMSM_DNSTRM_STAR_G_SM_EVENT_RECV_JOIN;
  dnStrmPerIntfStarGEventInfo.rtrIfNum  = rtrIfNum;
  dnStrmPerIntfStarGEventInfo.holdtime = holdtime;
  pimsmDnStrmPerIntfStarGExecute(pimsmCb, pStarGNode,
                                 &dnStrmPerIntfStarGEventInfo);
  memset(&upStrmStarGEventInfo,0,sizeof(upStrmStarGEventInfo));
  joinDesired = pimsmStarGJoinDesired(pimsmCb, pStarGNode);
  if (joinDesired == L7_TRUE)
  {
    upStrmStarGEventInfo.eventType =
    PIMSM_UPSTRM_STAR_G_SM_EVENT_JOIN_DESIRED_TRUE;
  }
  else
  {
    upStrmStarGEventInfo.eventType =
    PIMSM_UPSTRM_STAR_G_SM_EVENT_JOIN_DESIRED_FALSE;
  }
  pimsmUpStrmStarGExecute(pimsmCb, pStarGNode,
                          &upStrmStarGEventInfo);

  memset(&starGAssertEventInfo, 0, sizeof(pimsmPerIntfStarGAssertEventInfo_t));
  starGAssertEventInfo.rtrIfNum = rtrIfNum;
  starGAssertEventInfo.eventType =
  PIMSM_ASSERT_STAR_G_SM_EVENT_RECV_JOIN;
  inetAddressZeroSet(pimsmCb->family,&starGAssertEventInfo.srcAddr);
  pimsmPerIntfStarGAssertExecute(pimsmCb, pStarGNode,&starGAssertEventInfo);
  if(pimsmStarGMFCUpdate(pimsmCb,pStarGNode,
                   MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,"Failed to update MFC");
    return L7_FAILURE;
  }
  rc = pimsmSGRptStarGJoinRecvUpdate(pimsmCb,pGrpAddr,rtrIfNum,holdtime);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
    "pimsmSGRptStarGJoinRecvUpdate failed");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pGrpAddr :",pGrpAddr);
    return L7_FAILURE;
  }
  if(pimsmSGTreeJoinRecvEntryUpdate(pimsmCb,pGrpAddr,rtrIfNum) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,"pimsmSGTreeJoinRecvEntryUpdate failed");
    return L7_FAILURE;
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Exit");
  return L7_SUCCESS;
}

/******************************************************************************
*
* @purpose  Receive join/prune pkt downstream (S,G) Join processing part
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pSrcAddr    @b{(input)}  source address
* @param    pGrpAddr    @b{(input)}  group address
* @param    rtrIfNum    @b{(input)} incoming router interface number
* @param    holdtime    @b{(input)} holdtime
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t pimsmDnStrmSGJoinProcessing(pimsmCB_t *pimsmCb,
     L7_inet_addr_t *pSrcAddr, L7_inet_addr_t *pGrpAddr,
     L7_uint32 rtrIfNum,
     L7_short16 holdtime)
{
  pimsmSGNode_t       * pSGNode= L7_NULLPTR;
  pimsmDnStrmPerIntfSGEventInfo_t dnStrmPerIntfSGEventInfo;
  L7_RC_t rc;
  L7_uint32 rtrIfNumSrc;
  pimsmUpStrmSGEventInfo_t upStrmSGEventInfo;
  L7_BOOL joinDesired;
  pimsmPerIntfSGAssertEventInfo_t sgAssertEventInfo;
  L7_ushort16       sgRptFlags = 0;
  pimsmSGRptNode_t  *pSGRptNode = L7_NULLPTR;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Entry");

  /* Ignore this (S,G) Join if the rtrIfNum is same as the pSrcAddr IF */
  if((pimsmRPFInterfaceGet(pimsmCb, pSrcAddr,&rtrIfNumSrc) ==L7_SUCCESS) &&
     rtrIfNumSrc == rtrIfNum)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                "Ignore this (S,G) Join ");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pGrpAddr :",pGrpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pSrcAddr :",pSrcAddr);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR, "rtrIfNum = %d", rtrIfNum);
   /*Return Check is not required as the above condition could be met while testing assert topology */
    /*return L7_FAILURE;*/
  }
  if (mcastMapProtocolIsAdminScopedAddress(pimsmCb->family,
                                           rtrIfNumSrc,
                                           pGrpAddr) == L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,
                "admin-scope configured on Upstream ");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO, "pGrpAddr :",pGrpAddr);
    return L7_FAILURE;
  }

  rc = pimsmSGNodeCreate(pimsmCb, pSrcAddr, pGrpAddr, &pSGNode,L7_FALSE);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                "pimsmSGNodeCreate failed ");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pGrpAddr :",pGrpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pSrcAddr :",pSrcAddr);
    return L7_FAILURE;
  }

  if (pimsmSGRptFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGRptNode) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                "pimsmSGRptFind failed ");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pGrpAddr :",pGrpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pSrcAddr :",pSrcAddr);

  }
  else
  {
    sgRptFlags = pSGRptNode->flags;
  /*delete negative entries from MFC*/
   if((sgRptFlags & PIMSM_NULL_OIF) != L7_NULL)
   {
     PIMSM_TRACE(PIMSM_DEBUG_BIT_MFC_INTERACTION,  PIMSM_TRACE_ERROR,
                  "deleting MFC entry" );
     pimsmSGRptMFCUpdate(pimsmCb, pSGRptNode, MFC_DELETE_ENTRY, L7_FALSE);
   }
  }
  rc = pimsmSGIEntryCreate(pimsmCb, pSGNode, rtrIfNum);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                "pimsmSGIEntryCreate failed - rtrIfNum = %d", rtrIfNum);
    return L7_FAILURE;
  }
  memset(&dnStrmPerIntfSGEventInfo, 0 ,
         sizeof(pimsmDnStrmPerIntfSGEventInfo_t));
  dnStrmPerIntfSGEventInfo.eventType =
  PIMSM_DNSTRM_S_G_SM_EVENT_RECV_JOIN;
  dnStrmPerIntfSGEventInfo.rtrIfNum  = rtrIfNum;
  dnStrmPerIntfSGEventInfo.holdtime = holdtime;
  pimsmDnStrmPerIntfSGExecute(pimsmCb, pSGNode, &dnStrmPerIntfSGEventInfo);
  memset(&upStrmSGEventInfo,0,sizeof(pimsmUpStrmSGEventInfo_t));
  joinDesired = pimsmSGJoinDesired(pimsmCb, pSGNode);
  if (joinDesired == L7_TRUE)
  {
    upStrmSGEventInfo.eventType
    = PIMSM_UPSTRM_S_G_SM_EVENT_JOIN_DESIRED_TRUE;
  }
  else
  {
    upStrmSGEventInfo.eventType
    = PIMSM_UPSTRM_S_G_SM_EVENT_JOIN_DESIRED_FALSE;
  }
  pimsmUpStrmSGExecute(pimsmCb, pSGNode, &upStrmSGEventInfo);

  memset(&sgAssertEventInfo, 0, sizeof(pimsmPerIntfSGAssertEventInfo_t));
  sgAssertEventInfo.rtrIfNum = rtrIfNum;
  sgAssertEventInfo.eventType = PIMSM_ASSERT_S_G_SM_EVENT_RECV_JOIN;
  pimsmPerIntfSGAssertExecute(pimsmCb, pSGNode,&sgAssertEventInfo);

  /*Update only if entry exists in MFC */
  if(pimsmSGMFCUpdate(pimsmCb,pSGNode, MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,"pimsmSGMFCUpdate failed");
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Exit");
  return L7_SUCCESS;
}

/******************************************************************************
*
* @purpose  Receive join/prune pkt downstream (S,G,rpt) Join processing part
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pSrcAddr    @b{(input)}  source address
* @param    pGrpAddr    @b{(input)}  group address
* @param    rtrIfNum    @b{(input)} incoming router interface number
* @param    holdtime    @b{(input)} holdtime
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t pimsmDnStrmSGRptJoinProcessing(pimsmCB_t *pimsmCb,
     L7_inet_addr_t *pSrcAddr, L7_inet_addr_t *pGrpAddr,
     L7_uint32 rtrIfNum,
     L7_short16 holdtime)
{
  pimsmSGRptNode_t *    pSGRptNode = L7_NULLPTR;
  pimsmDnStrmPerIntfSGRptEventInfo_t dnStrmPerIntfSGRptEventInfo;
  L7_RC_t rc;
  pimsmUpStrmSGRptEventInfo_t upStrmSGRptEventInfo;
  L7_BOOL pruneDesired;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Entry");

  rc = pimsmSGRptFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGRptNode);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                "pimsmSGRptNodeCreate failed ");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pGrpAddr :",pGrpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pSrcAddr :",pSrcAddr);
    return L7_FAILURE;
  }
  if (mcastMapProtocolIsAdminScopedAddress(pimsmCb->family,
                                           pSGRptNode->pimsmSGRptEntry.pimsmSGRptRPFIfIndex,
                                           pGrpAddr) == L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,
                "admin-scope configured on Upstream ");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO, "pGrpAddr :",pGrpAddr);
    return L7_FAILURE;
  }

  if (pSGRptNode->pimsmSGRptIEntry[rtrIfNum] == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,"(S,G,I) Entry does not exist ");
   return L7_FAILURE;
  }
  memset(&dnStrmPerIntfSGRptEventInfo, 0 ,
         sizeof(pimsmDnStrmPerIntfSGRptEventInfo_t));
  dnStrmPerIntfSGRptEventInfo.eventType =
  PIMSM_DNSTRM_S_G_RPT_SM_EVENT_RECV_JOIN_SG_RPT;
  dnStrmPerIntfSGRptEventInfo.rtrIfNum  = rtrIfNum;
  dnStrmPerIntfSGRptEventInfo.holdtime = holdtime;
  pimsmDnStrmPerIntfSGRptExecute(pimsmCb, pSGRptNode,
      &dnStrmPerIntfSGRptEventInfo);

  if(pimsmSGRptMFCUpdate(pimsmCb,pSGRptNode, MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,"MFC Updation failed");
    return L7_FAILURE;
  }

  memset(&upStrmSGRptEventInfo,0,sizeof(pimsmUpStrmSGRptEventInfo_t));
  pruneDesired = pimsmSGRptPruneDesired(pimsmCb,pSGRptNode);
  if(pruneDesired ==  L7_TRUE)
  {
    upStrmSGRptEventInfo.eventType
          = PIMSM_UPSTRM_S_G_RPT_SM_EVENT_PRUNE_DESIRED_TRUE;
  }
  else
  {
    upStrmSGRptEventInfo.eventType
          = PIMSM_UPSTRM_S_G_RPT_SM_EVENT_PRUNE_DESIRED_FALSE;
  }
  pimsmUpStrmSGRptExecute(pimsmCb,pSGRptNode,&upStrmSGRptEventInfo);

  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Exit");
  return L7_SUCCESS;
}


/******************************************************************************
*
* @purpose  Receive join/prune pkt downstream (*,G) Prune processing part
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pSrcAddr    @b{(input)}  source address
* @param    pGrpAddr    @b{(input)}  group address
* @param    rtrIfNum    @b{(input)} incoming router interface number
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t pimsmDnStrmStarGPruneProcessing(pimsmCB_t *pimsmCb,
         L7_inet_addr_t *pSrcAddr, L7_inet_addr_t *pGrpAddr,
         L7_uint32 rtrIfNum)
{
  pimsmStarGNode_t        * pStarGNode= L7_NULLPTR;
  L7_RC_t rc;
  pimsmDnStrmPerIntfStarGEventInfo_t  dnStrmPerIntfStarGEventInfo;
  pimsmStarGEntry_t *pStarGEntry;
  L7_inet_addr_t *pRpAddr;
  pimsmStarStarRpNode_t   * pStarStarRpNode = L7_NULLPTR;
  L7_BOOL newStarGEntryFlag = L7_FALSE;
  pimsmUpStrmStarGEventInfo_t  upStrmStarGEventInfo;
  L7_BOOL joinDesired;
  L7_BOOL couldAssert;
  pimsmPerIntfStarGAssertEventInfo_t  starGAssertEventInfo;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Entry");

  rc = pimsmStarGFind(pimsmCb, pGrpAddr, &pStarGNode);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                "pimsmStarGFind failed ");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pGrpAddr :",pGrpAddr);
    rc = pimsmStarStarRpFind(pimsmCb, pSrcAddr, &pStarStarRpNode);
    if(rc != L7_SUCCESS ||
       pStarStarRpNode == (pimsmStarStarRpNode_t *) L7_NULLPTR)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                  "pimsmStarStarRpFind failed ");
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "RP Addr  :",pSrcAddr);
      return L7_FAILURE;
    }
    rc = pimsmStarGNodeCreate(pimsmCb, pGrpAddr, &pStarGNode);
    if(rc != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                  "pimsmStarGNodeCreate failed ");
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pGrpAddr :",pGrpAddr);
      return L7_FAILURE;
    }
    newStarGEntryFlag = L7_TRUE;
  }

  if(newStarGEntryFlag != L7_TRUE)
  {
    pStarGEntry = &pStarGNode->pimsmStarGEntry;
    pRpAddr = &pStarGEntry->pimsmStarGRPAddress;
    if(PIMSM_INET_IS_ADDR_EQUAL(pSrcAddr, pRpAddr) == L7_FALSE)
    {
      /* The RP address doesn't match. Ignore. */
      PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                  "RP address doesn't match ");
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "msg_rp_addr :",pSrcAddr);
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "my_rp_addr :",pRpAddr);
      return L7_FAILURE;
    }
  }
  if (pStarGNode->pimsmStarGIEntry[rtrIfNum] == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,"(*,G,I) Entry does not exist ");
   return L7_FAILURE;
  }

  memset(&dnStrmPerIntfStarGEventInfo, 0 ,
         sizeof(pimsmDnStrmPerIntfStarGEventInfo_t));
  dnStrmPerIntfStarGEventInfo.eventType =
  PIMSM_DNSTRM_STAR_G_SM_EVENT_RECV_PRUNE;
  dnStrmPerIntfStarGEventInfo.rtrIfNum = rtrIfNum;

  pimsmDnStrmPerIntfStarGExecute(pimsmCb, pStarGNode,
                                 &dnStrmPerIntfStarGEventInfo);

  memset(&upStrmStarGEventInfo,0,sizeof(upStrmStarGEventInfo));
  joinDesired = pimsmStarGJoinDesired(pimsmCb, pStarGNode);
  if (joinDesired == L7_TRUE)
  {
    /*upStrmStarGEventInfo.eventType =
    PIMSM_UPSTRM_STAR_G_SM_EVENT_JOIN_DESIRED_TRUE;*/
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
       "(joinDesired == True) ");
  }
  else
  {
    upStrmStarGEventInfo.eventType =
    PIMSM_UPSTRM_STAR_G_SM_EVENT_JOIN_DESIRED_FALSE;
    pimsmUpStrmStarGExecute(pimsmCb, pStarGNode,
                          &upStrmStarGEventInfo);
  }

  memset(&starGAssertEventInfo, 0, sizeof(pimsmPerIntfStarGAssertEventInfo_t));
  starGAssertEventInfo.rtrIfNum = rtrIfNum;
  couldAssert = pimsmStarGICouldAssert(pimsmCb, pStarGNode, rtrIfNum);
  if(couldAssert == L7_FALSE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
       "(couldAssert == false)");
    starGAssertEventInfo.eventType =
    PIMSM_ASSERT_STAR_G_SM_EVENT_COULD_ASSERT_FALSE;
    inetAddressZeroSet(pimsmCb->family,&starGAssertEventInfo.srcAddr);
    pimsmPerIntfStarGAssertExecute(pimsmCb, pStarGNode,&starGAssertEventInfo);
  }
  else
  {
     PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
       "(couldAssert == True) ");
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Exit");
  return L7_SUCCESS;
}

/******************************************************************************
*
* @purpose  Receive join/prune pkt downstream (S,G) Prune processing part
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pSrcAddr    @b{(input)}  source address
* @param    pGrpAddr    @b{(input)}  group address
* @param    rtrIfNum    @b{(input)} incoming router interface number
* @param    holdtime    @b{(input)} holdtime
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t pimsmDnStrmSGPruneProcessing(pimsmCB_t *pimsmCb,
         L7_inet_addr_t *pSrcAddr, L7_inet_addr_t *pGrpAddr,
         L7_uint32 rtrIfNum,L7_short16 holdtime)
{
  pimsmSGNode_t       * pSGNode= L7_NULLPTR;
  pimsmSGRptNode_t *pSGRptNode = L7_NULLPTR;
  L7_RC_t rc;
  pimsmDnStrmPerIntfSGEventInfo_t dnStrmPerIntfSGEventInfo;
  pimsmUpStrmSGEventInfo_t upStrmSGEventInfo;
  pimsmUpStrmSGRptEventInfo_t upStrmSGRptEventInfo;
  L7_BOOL pruneDesired;
  L7_BOOL couldAssert;
  pimsmPerIntfSGAssertEventInfo_t  sgAssertEventInfo;

  interface_bitset_t    sgInhrtdOif;
  L7_BOOL result= L7_FALSE;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Entry");
  rc = pimsmSGFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGNode);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                "pimsmSGFind failed ");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pGrpAddr :",pGrpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pSrcAddr :",pSrcAddr);
    return L7_FAILURE;
  }
  if (pSGNode->pimsmSGIEntry[rtrIfNum] == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,"(S,G,I) Entry does not exist ");
   return L7_FAILURE;
  }
  memset(&dnStrmPerIntfSGEventInfo, 0 ,
         sizeof(pimsmDnStrmPerIntfSGEventInfo_t));
  dnStrmPerIntfSGEventInfo.eventType =
  PIMSM_DNSTRM_S_G_SM_EVENT_RECV_PRUNE;
  dnStrmPerIntfSGEventInfo.rtrIfNum = rtrIfNum;

  pimsmDnStrmPerIntfSGExecute(pimsmCb, pSGNode,
                              &dnStrmPerIntfSGEventInfo);

  memset(&sgAssertEventInfo, 0, sizeof(pimsmPerIntfSGAssertEventInfo_t));
  sgAssertEventInfo.rtrIfNum = rtrIfNum;
  couldAssert = pimsmSGICouldAssert(pimsmCb, pSGNode, rtrIfNum);
  if(couldAssert == L7_FALSE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
       "(couldAssert == false)");
    sgAssertEventInfo.eventType =
    PIMSM_ASSERT_S_G_SM_EVENT_COULD_ASSERT_FALSE;
    pimsmPerIntfSGAssertExecute(pimsmCb, pSGNode,&sgAssertEventInfo);
  }
  else
  {
     PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
       "(couldAssert == True) ");
  }

  rc = pimsmSGRptFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGRptNode);

  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                "pimsmSGRptFind failed ");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pGrpAddr :",pGrpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pSrcAddr :",pSrcAddr);
    return L7_FAILURE;
  }
  rc = pimsmSGRptIEntryCreate(pimsmCb, pSGRptNode, rtrIfNum);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                "pimsmSGRptIEntryCreate failed - rtrIfNum = %d", rtrIfNum);
    return L7_FAILURE;
  }
  memset(&upStrmSGRptEventInfo,0,sizeof(pimsmUpStrmSGRptEventInfo_t));
  pruneDesired = pimsmSGRptPruneDesired(pimsmCb,pSGRptNode);
  if (pruneDesired ==  L7_TRUE)
  {
    upStrmSGRptEventInfo.eventType
    = PIMSM_UPSTRM_S_G_RPT_SM_EVENT_PRUNE_DESIRED_TRUE;
  }
  else
  {
    upStrmSGRptEventInfo.eventType
    = PIMSM_UPSTRM_S_G_RPT_SM_EVENT_PRUNE_DESIRED_FALSE;
  }
  pimsmUpStrmSGRptExecute(pimsmCb,pSGRptNode,&upStrmSGRptEventInfo);

  memset( &sgInhrtdOif, 0, sizeof( interface_bitset_t ) );
  pimsmSGInhrtdOlist( pimsmCb, pSGNode, &sgInhrtdOif );
  BITX_RESET(&sgInhrtdOif,rtrIfNum);
  BITX_IS_EMPTY(&sgInhrtdOif,result);
  memset(&upStrmSGEventInfo,0,sizeof(pimsmUpStrmSGEventInfo_t));
  /*joinDesired = pimsmSGJoinDesired(pimsmCb,pSGNode);
  if (joinDesired == L7_TRUE)*/
  if (result != L7_TRUE)
  {
    /*upStrmSGEventInfo.eventType
    = PIMSM_UPSTRM_S_G_SM_EVENT_JOIN_DESIRED_TRUE;*/
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
       "(joinDesired == True) ");
  }
  else
  {
    upStrmSGEventInfo.eventType
    = PIMSM_UPSTRM_S_G_SM_EVENT_JOIN_DESIRED_FALSE;
    pimsmUpStrmSGExecute(pimsmCb,pSGNode, &upStrmSGEventInfo);
  }


  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Exit");
  return L7_SUCCESS;
}

/******************************************************************************
*
* @purpose  Receive join/prune pkt downstream (S,G,rpt) EOM processing part
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pGrpAddr    @b{(input)}  group address
* @param    rtrIfNum    @b{(input)} incoming router interface number
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t pimsmDnStrmSGRptEOMEventPost(pimsmCB_t *pimsmCb,
      L7_inet_addr_t *pGrpAddr, L7_uint32 rtrIfNum)
{
  pimsmSGRptNode_t         * pSGRptNode;
  pimsmSGRptEntry_t *       pSGRptEntry;
  L7_RC_t rc, rc2;
  pimsmDnStrmPerIntfSGRptEventInfo_t dnStrmPerIntfSGRptEventInfo;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Entry");
  rc = pimsmSGRptFirstGet( pimsmCb, &pSGRptNode );
  while(rc == L7_SUCCESS)
  {
    pSGRptEntry = &pSGRptNode->pimsmSGRptEntry;
    if(PIMSM_INET_IS_ADDR_EQUAL(&pSGRptEntry->pimsmSGRptGrpAddress, pGrpAddr) == L7_TRUE)
    {
      rc2 = pimsmSGRptIEntryCreate(pimsmCb, pSGRptNode, rtrIfNum);
      if(rc2 != L7_SUCCESS)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                    "pimsmSGRptIEntryCreate failed - rtrIfNum = %d", rtrIfNum);
        return L7_FAILURE;
      }
      memset(&dnStrmPerIntfSGRptEventInfo, 0 ,
             sizeof(pimsmDnStrmPerIntfSGRptEventInfo_t));
      dnStrmPerIntfSGRptEventInfo.eventType =
      PIMSM_DNSTRM_S_G_RPT_SM_EVENT_END_OF_MSG;
      dnStrmPerIntfSGRptEventInfo.rtrIfNum = rtrIfNum;

      pimsmDnStrmPerIntfSGRptExecute(pimsmCb, pSGRptNode,
                                     &dnStrmPerIntfSGRptEventInfo);
    }
     rc = pimsmSGRptNextGet( pimsmCb, pSGRptNode, &pSGRptNode );
  }


  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Exit");
  return L7_SUCCESS;
}
/******************************************************************************
*
* @purpose  Receive join/prune pkt downstream (S,G,rpt) Prune processing part
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pSrcAddr    @b{(input)}  source address
* @param    pGrpAddr    @b{(input)}  group address
* @param    rtrIfNum    @b{(input)} incoming router interface number
* @param    holdtime    @b{(input)} holdtime
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t pimsmDnStrmSGRptPruneProcessing(pimsmCB_t *pimsmCb,
   L7_inet_addr_t *pSrcAddr, L7_inet_addr_t *pGrpAddr,
   L7_uint32 rtrIfNum,L7_short16 holdtime)
{
  pimsmSGRptNode_t *    pSGRptNode= L7_NULLPTR;
  L7_RC_t rc;
  pimsmDnStrmPerIntfSGRptEventInfo_t dnStrmPerIntfSGRptEventInfo;
  pimsmUpStrmSGRptEventInfo_t upStrmSGRptEventInfo;
  L7_BOOL pruneDesired;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Entry");

  rc = pimsmSGRptNodeCreate(pimsmCb, pSrcAddr, pGrpAddr, &pSGRptNode);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                "pimsmSGRptNodeCreate failed ");
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pGrpAddr :",pGrpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pSrcAddr :",pSrcAddr);
    return L7_FAILURE;
  }
  rc = pimsmSGRptIEntryCreate(pimsmCb, pSGRptNode, rtrIfNum);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                "pimsmSGRptIEntryCreate failed - rtrIfNum = %d", rtrIfNum);
    return L7_FAILURE;
  }
  memset(&dnStrmPerIntfSGRptEventInfo, 0 ,
         sizeof(pimsmDnStrmPerIntfSGRptEventInfo_t));
  dnStrmPerIntfSGRptEventInfo.eventType =
  PIMSM_DNSTRM_S_G_RPT_SM_EVENT_RECV_PRUNE_SG_RPT;
  dnStrmPerIntfSGRptEventInfo.rtrIfNum  = rtrIfNum;
  dnStrmPerIntfSGRptEventInfo.holdtime = holdtime;
  pimsmDnStrmPerIntfSGRptExecute(pimsmCb, pSGRptNode,
    &dnStrmPerIntfSGRptEventInfo);

  memset(&upStrmSGRptEventInfo,0,sizeof(pimsmUpStrmSGRptEventInfo_t));
  pruneDesired = pimsmSGRptPruneDesired(pimsmCb,pSGRptNode);
  if(pruneDesired ==  L7_TRUE)
  {
    upStrmSGRptEventInfo.eventType
      = PIMSM_UPSTRM_S_G_RPT_SM_EVENT_PRUNE_DESIRED_TRUE;
    pimsmUpStrmSGRptExecute(pimsmCb,pSGRptNode,&upStrmSGRptEventInfo);
  }
  else
  {
    /*upStrmSGRptEventInfo.eventType
      = PIMSM_UPSTRM_S_G_RPT_SM_EVENT_PRUNE_DESIRED_FALSE;*/
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
       "(pruneDesired == False) ");
  }

  if(pimsmSGRptMFCUpdate(pimsmCb,pSGRptNode, MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
      "MFC Updation failed");
    return L7_FAILURE;
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Exit");
  return L7_SUCCESS;
}

/******************************************************************************
*
* @purpose  Receive join/prune pkt non-target processing part
*
* @param    pimsmCb     @b{(input)}  control block
* @param    rtrIfNum    @b{(input)} incoming router interface number
* @param    pIntfEntry  @b{(input)} interface entry
* @param    pData       @b{(input)} join/prune msg start
* @param    numGroups   @b{(input)} number of groups in the msg
* @param    holdtime    @b{(input)} holdtime
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t pimsmJoinPruneNonTargetProcessing(pimsmCB_t *pimsmCb,
         L7_uint32 rtrIfNum, pimsmInterfaceEntry_t  *pIntfEntry,
         L7_char8 *pData, L7_char8 numGroups, L7_short16 holdtime)
{

  L7_short16 numJoinSrcs;
  L7_short16 numPruneSrcs;
  L7_char8 srcFlags = L7_NULL;
  pim_encod_grp_addr_t  encodGrpAddr;
  pim_encod_src_addr_t  encodSrcAddr;
  L7_inet_addr_t srcAddr, grpAddr, allMcastGrpAddrH, grpAddrH;
  L7_uchar8 maskLen;
  L7_BOOL adminScopedGrp = L7_FALSE;
  L7_char8 *pDataJoinsStart;
  L7_char8 *pDataPrunesStart;
  L7_inet_addr_t rpAddr;
  L7_BOOL ignoreGrp;
  L7_short16 numJoinSrcsTmp;
  L7_BOOL retVal = L7_FALSE;
  L7_uint32 joinPruneMsgSize=0;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Entry");
  memset(&encodGrpAddr, 0 ,sizeof(pim_encod_grp_addr_t));
  memset(&encodSrcAddr, 0 ,sizeof(pim_encod_uni_addr_t));

  pimJoinPruneMsgLenGet(pimsmCb->family,&joinPruneMsgSize);
  while(numGroups--)
  {
    PIM_GET_EGADDR_INET(&encodGrpAddr, pData);
    MCAST_GET_SHORT(numJoinSrcs, pData);
    MCAST_GET_SHORT(numPruneSrcs, pData);
    /*inetMaskLenToMask(encodGrpAddr.masklen, g_mask); -Remove*/
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "numJoinSrcs = %d", numJoinSrcs);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "numPruneSrcs = %d", numPruneSrcs);

    inetAddressSet(encodGrpAddr.addr_family,
                   &(encodGrpAddr.addr),&grpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Recv'ed encodGrpAddr :",&grpAddr);
    adminScopedGrp = mcastMapProtocolIsAdminScopedAddress(pimsmCb->family,
                                                          rtrIfNum, &grpAddr);
    inetAddrNtoh(&grpAddr, &grpAddrH);
    if(inetIsInMulticast(&grpAddrH) ==L7_FALSE)
    {
      pData += (numJoinSrcs + numPruneSrcs) * joinPruneMsgSize;
      PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                  "encodGrpAddr is NOT in Multicast Addr Range ");
      continue; /* Ignore this grpAddr and jump to the next */
    }

    inetAllMcastGroupAddressInit(pimsmCb->family,&allMcastGrpAddrH);
    pimsmStarStarRpMaskLenGet(pimsmCb->family,&maskLen);
    if((L7_INET_IS_ADDR_EQUAL(&grpAddrH, &allMcastGrpAddrH) == L7_TRUE) &&
       (encodGrpAddr.masklen == maskLen))
    {
      /* Upstream (*,*,RP) Join suppression */
      pimsmUpStrmStarStarRpProcessing(pimsmCb, pData, &grpAddr,
              holdtime, numJoinSrcs, numPruneSrcs, adminScopedGrp, rtrIfNum);
      continue;
    }
    retVal= pimsmMapIsInSsmRange(pimsmCb->family, &grpAddr);
    if(retVal == L7_FALSE)
    {
     if(pimsmRpAddressGet(pimsmCb, &grpAddr,  &rpAddr) != L7_SUCCESS)
     {
       PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX,  PIMSM_TRACE_ERROR, "RP not found" );
     }

     if(inetIsAddressZero(&rpAddr) == L7_TRUE)
     {
       PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                   "No RP found for this grpAddr-addr");
       PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "grpAddr-addr :",&grpAddr);
       pData += (numJoinSrcs + numPruneSrcs) * joinPruneMsgSize;
       continue;
     }
     PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "rpAddr = ", &rpAddr);
    }
    pDataJoinsStart = pData;
    pDataPrunesStart = pData + numJoinSrcs * joinPruneMsgSize;

    /*  If the RP address in the Join message is different from
    * the local match, ignore the whole group.     */
    ignoreGrp = L7_FALSE;
    numJoinSrcsTmp = numJoinSrcs;
    while(numJoinSrcsTmp--)
    {
      /* ignore all joins for administratively scoped groups */
      if(adminScopedGrp == L7_TRUE)
      {
        PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                         "Ignoring AdminScope addr :",&grpAddr);
        break;
      }
      PIM_GET_ESADDR_INET(&encodSrcAddr, pData);
      inetAddressSet(encodSrcAddr.addr_family,
                     &(encodSrcAddr.addr),&srcAddr);
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "srcAddr = ", &srcAddr);
      if(inetIsValidHostAddress(&srcAddr) == L7_FALSE)
      {
        PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                         "Invalid host addr :",&srcAddr);
        continue;
      }
      if(!(encodSrcAddr.flags & PIMSM_ADDR_RP_BIT)
         && (encodSrcAddr.flags & PIMSM_ADDR_WC_BIT))
      {
        PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                         "Invalid host addr Bits WC=1 RPT=0:",&srcAddr);
        continue;
      }
      if((encodSrcAddr.flags & PIMSM_ADDR_RP_BIT)
         && (encodSrcAddr.flags & PIMSM_ADDR_WC_BIT))
      {
        /* This target is the RP address, i.e. (*,G) Join.
         * Check if the RP-mapping is consistent */
        PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                    "Received (*,G) join");
        if(retVal == L7_FALSE)
        {
         if(PIMSM_INET_IS_ADDR_EQUAL(&rpAddr, &srcAddr) == L7_FALSE)
         {
           ignoreGrp = L7_TRUE;
           PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                       "Rp addr in msg is different from local match");
           break;
         }
        }
      }
    }
    /* If the RP address in the Join message is different from
     * the local match, ignore the whole group.  */
    if(ignoreGrp == L7_TRUE)
    {
      pData +=
      (numJoinSrcsTmp + numPruneSrcs) * joinPruneMsgSize;
      continue;
    }

    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                "Start Processing Join Part");
    pData = pDataJoinsStart;
    while(numJoinSrcs--)
    {
      /* ignore all joins for administratively scoped groups */
      if(adminScopedGrp == L7_TRUE)
      {
        PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                         "Ignoring AdminScope addr  :",&grpAddr);
        break;
      }
      PIM_GET_ESADDR_INET(&encodSrcAddr, pData);
      inetAddressSet(encodSrcAddr.addr_family,
                     &(encodSrcAddr.addr),&srcAddr);
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "srcAddr = ", &srcAddr);
      if(inetIsValidHostAddress(&srcAddr) == L7_FALSE)
      {
        PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                         "Invalid host addr :",&srcAddr);
        continue;
      }
      srcFlags = encodSrcAddr.flags;
      /*inetMaskLenToMask(encodSrcAddr.masklen, s_mask); -Remove*/

      if((srcFlags & PIMSM_ADDR_RP_BIT) &&
         (srcFlags & PIMSM_ADDR_WC_BIT))
      {
        pimsmUpStrmStarGJoinProcessing(pimsmCb, &srcAddr, &grpAddr,holdtime, rtrIfNum);
      } /* End of (*,G) Join suppression */
      else if((srcFlags & PIMSM_ADDR_RP_BIT) &&
              (!(srcFlags & PIMSM_ADDR_WC_BIT)))
      {
        pimsmUpStrmSGRptJoinProcessing(pimsmCb, &srcAddr, &grpAddr,holdtime, rtrIfNum);
      }
      else if((!(srcFlags & PIMSM_ADDR_RP_BIT)) &&
              (!(srcFlags & PIMSM_ADDR_WC_BIT)))
      {
        /* this is (S,G) Join */
        pimsmUpStrmSGJoinProcessing(pimsmCb, &srcAddr, &grpAddr,holdtime, rtrIfNum);
      }
    }
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                "Start Processing Prune Part");
    pData = pDataPrunesStart;
    /* Prunes suppression */
    while(numPruneSrcs--)
    {
      PIM_GET_ESADDR_INET(&encodSrcAddr, pData);
      inetAddressSet(encodSrcAddr.addr_family,
                     &(encodSrcAddr.addr),&srcAddr);
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "srcAddr = ", &srcAddr);
      if(inetIsValidHostAddress(&srcAddr) == L7_FALSE)
      {
        PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Invalid host addr :",&srcAddr);
        continue;
      }
      srcFlags = encodSrcAddr.flags;
      /*inetMaskLenToMask(encodSrcAddr.masklen, s_mask); -Remove*/
      if((srcFlags & PIMSM_ADDR_RP_BIT) &&
         (srcFlags & PIMSM_ADDR_WC_BIT))
      {
        pimsmUpStrmStarGPruneProcessing(pimsmCb, &srcAddr, &grpAddr, rtrIfNum);
      }
      else if((srcFlags & PIMSM_ADDR_RP_BIT) &&
              (!(srcFlags & PIMSM_ADDR_WC_BIT)))
      {
        pimsmUpStrmSGRptPruneProcessing(pimsmCb, &srcAddr, &grpAddr, rtrIfNum);
      }
      else if((!(srcFlags & PIMSM_ADDR_RP_BIT)) &&
              (!(srcFlags & PIMSM_ADDR_WC_BIT)))
      {
        pimsmUpStrmSGPruneProcessing(pimsmCb, &srcAddr, &grpAddr, rtrIfNum);
      }
    }  /* while (numPruneSrcs--) */
  }  /* while (numGroups--) */
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Exit");
  return L7_SUCCESS;
}

/******************************************************************************
*
* @purpose  Receive join/prune pkt target processing part
*
* @param    pimsmCb     @b{(input)} control block
* @param    rtrIfNum    @b{(input)} incoming router interface number
* @param    pIntfEntry  @b{(input)} interface entry
* @param    pData       @b{(input)} join/prune msg start
* @param    numGroups   @b{(input)} number of groups in the msg
* @param    holdtime    @b{(input)} holdtime
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t pimsmJoinPruneTargetProcessing(pimsmCB_t *pimsmCb,
   L7_uint32 rtrIfNum, pimsmInterfaceEntry_t  *pIntfEntry,
   L7_char8 *pData, L7_char8 numGroups, L7_short16 holdtime)
{
  L7_char8 srcFlags = L7_NULL;
  pim_encod_grp_addr_t  encodGrpAddr;
  pim_encod_src_addr_t  encodSrcAddr;
  L7_inet_addr_t srcAddr, grpAddr, allMcastGrpAddrH, grpAddrH;
  L7_uchar8 maskLen;
  L7_BOOL adminScopedGrp = L7_FALSE;
  L7_BOOL tmpState = L7_FALSE ;
  L7_inet_addr_t rpAddr;
  L7_BOOL ignoreGrp;
  L7_short16 numJoinSrcsTmp;
  L7_short16 numJoinSrcs;
  L7_short16 numPruneSrcs;
  L7_char8 *pDataJoinsStart;
  L7_char8 *pDataPrunesStart;
  L7_BOOL retVal = L7_FALSE;
  L7_uint32 joinPruneMsgSize=0;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Entry");

  memset(&encodGrpAddr, 0 ,sizeof(pim_encod_grp_addr_t));
  memset(&encodSrcAddr, 0 ,sizeof(pim_encod_uni_addr_t));

  pimJoinPruneMsgLenGet(pimsmCb->family,&joinPruneMsgSize);

  /*
   * Start processing the groups. If this is (*,*,RP), skip it, but process
   * it at the end.
   */
  while(numGroups--)
  {
    PIM_GET_EGADDR_INET(&encodGrpAddr, pData);
    MCAST_GET_SHORT(numJoinSrcs, pData);
    MCAST_GET_SHORT(numPruneSrcs, pData);
    /*inetMaskLenToMask(encodGrpAddr.masklen, g_mask);-Remove*/
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "numJoinSrcs = %d", numJoinSrcs);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "numPruneSrcs = %d", numPruneSrcs);
    inetAddressSet(encodGrpAddr.addr_family,
                   &(encodGrpAddr.addr),&grpAddr);
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Recv'ed encodGrpAddr :",&grpAddr);
    adminScopedGrp = mcastMapProtocolIsAdminScopedAddress(pimsmCb->family,
                                                          rtrIfNum, &grpAddr);
    inetAddrNtoh(&grpAddr, &grpAddrH);
    if(inetIsInMulticast(&grpAddrH) ==L7_FALSE)
    {
      pData += (numJoinSrcs + numPruneSrcs) * joinPruneMsgSize;
      PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                  "encodGrpAddr is NOT in Multicast Addr Range ");
      continue; /* Ignore this group and jump to the next */
    }

    inetAllMcastGroupAddressInit(pimsmCb->family,&allMcastGrpAddrH);
    pimsmStarStarRpMaskLenGet(pimsmCb->family,&maskLen);
    if((L7_INET_IS_ADDR_EQUAL(&grpAddrH, &allMcastGrpAddrH) == L7_TRUE) &&
       (encodGrpAddr.masklen == maskLen))
    {
      pimsmDnStrmStarStarRpProcessing(pimsmCb, pData, &grpAddr,
                                      rtrIfNum, holdtime, numJoinSrcs, numPruneSrcs, adminScopedGrp);
      continue;
    }
    retVal= pimsmMapIsInSsmRange(pimsmCb->family, &grpAddr);
    if(retVal == L7_FALSE)
    {
     if(pimsmRpAddressGet(pimsmCb, &grpAddr,  &rpAddr) != L7_SUCCESS)
     {
       PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX,  PIMSM_TRACE_ERROR, "RP not found" );
       continue;
     }

     if(inetIsAddressZero(&rpAddr) == L7_TRUE)
     {
       PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                   "No RP found for this group-addr");
       PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "group-addr :",&grpAddr);
       pData += (numJoinSrcs + numPruneSrcs) * joinPruneMsgSize;
       continue;
     }
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "rpAddr = ", &rpAddr);
    }
    pDataJoinsStart = pData;
    pDataPrunesStart = pData + numJoinSrcs * joinPruneMsgSize;

    /*  If the RP address in the Join message is different from
    * the local match, ignore the whole group.     */
    ignoreGrp = L7_FALSE;
    numJoinSrcsTmp = numJoinSrcs;
    while(numJoinSrcsTmp--)
    {
      /* ignore all joins for administratively scoped groups */
      if(adminScopedGrp == L7_TRUE)
      {
        PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                         "Ignoring AdminScope addr :",&grpAddr);
        break;
      }
      PIM_GET_ESADDR_INET(&encodSrcAddr, pData);
      inetAddressSet(encodSrcAddr.addr_family,
                     &(encodSrcAddr.addr),&srcAddr);
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "srcAddr = ", &srcAddr);
      if(inetIsValidHostAddress(&srcAddr) == L7_FALSE)
      {
        PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                         "Invalid host addr :",&srcAddr);
        continue;
      }

      if(!(encodSrcAddr.flags & PIMSM_ADDR_RP_BIT)
         && (encodSrcAddr.flags & PIMSM_ADDR_WC_BIT))
      {
        PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                         "Invalid host addr Bits WC=1 RPT=0:",&srcAddr);
        continue;
      }
      if((encodSrcAddr.flags & PIMSM_ADDR_RP_BIT)
         && (encodSrcAddr.flags & PIMSM_ADDR_WC_BIT))
      {
        /* This target is the RP address, i.e. (*,G) Join.
         * Check if the RP-mapping is consistent */
        PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                    "Received (*,G) join");
         if(retVal == L7_FALSE)
         {
          if(PIMSM_INET_IS_ADDR_EQUAL(&rpAddr, &srcAddr) == L7_FALSE)
          {
            ignoreGrp = L7_TRUE;
            PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                        "Rp addr in msg is different from local match");
            break;
          }
         }
      }
    }
    /* If the RP address in the Join message is different from
     * the local match, ignore the whole group.  */
    if(ignoreGrp == L7_TRUE)
    {
      pData +=
      (numJoinSrcsTmp + numPruneSrcs) * joinPruneMsgSize;
      continue;
    }



    /* Jump back to the Join part and process it */
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                "Start Processing Join Part");
    pData = pDataJoinsStart;
    while(numJoinSrcs--)
    {
      /* ignore all joins for administratively scoped groups */
      if(adminScopedGrp == L7_TRUE)
      {
        PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                         "Ignoring AdminScope addr  :",&grpAddr);
        break;
      }
      PIM_GET_ESADDR_INET(&encodSrcAddr, pData);
      inetAddressSet(encodSrcAddr.addr_family,
                     &(encodSrcAddr.addr),&srcAddr);
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "srcAddr = ", &srcAddr);
      if(inetIsValidHostAddress(&srcAddr) == L7_FALSE)
      {
        PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                         "Invalid host addr :",&srcAddr);
        continue;
      }
      srcFlags = encodSrcAddr.flags;
      /*inetMaskLenToMask(encodSrcAddr.masklen, s_mask); */

      if((srcFlags & PIMSM_ADDR_RP_BIT) &&
         (srcFlags & PIMSM_ADDR_WC_BIT))
      {
        tmpState = L7_TRUE;
        pimsmDnStrmStarGJoinProcessing(pimsmCb, &srcAddr, &grpAddr,
                                       rtrIfNum, holdtime);
      } /* End of (*,G) Join suppression */
      else if((srcFlags & PIMSM_ADDR_RP_BIT) &&
              (!(srcFlags & PIMSM_ADDR_WC_BIT)))
      {
        pimsmDnStrmSGRptJoinProcessing(pimsmCb, &srcAddr, &grpAddr,
                                       rtrIfNum, holdtime);
      }
      else if((!(srcFlags & PIMSM_ADDR_RP_BIT)) &&
              (!(srcFlags & PIMSM_ADDR_WC_BIT)))
      {
        /* this is (S,G) Join */
        pimsmDnStrmSGJoinProcessing(pimsmCb, &srcAddr, &grpAddr,
                                    rtrIfNum, holdtime);
      }
    } /* while(numJoinSrcs--) */

    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                "Start Processing Prune Part");

    pData = pDataPrunesStart;
    while(numPruneSrcs--)
    {
      PIM_GET_ESADDR_INET(&encodSrcAddr, pData);
      inetAddressSet(encodSrcAddr.addr_family,
                     &(encodSrcAddr.addr),&srcAddr);
      if(inetIsValidHostAddress(&srcAddr) == L7_FALSE)
      {
        PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Invalid host addr :",&srcAddr);
        continue;
      }
      srcFlags = encodSrcAddr.flags;
      if((srcFlags & PIMSM_ADDR_RP_BIT) &&
         (srcFlags & PIMSM_ADDR_WC_BIT))
      {
        pimsmDnStrmStarGPruneProcessing(pimsmCb, &srcAddr, &grpAddr,
                                        rtrIfNum);
      }
      else if((srcFlags & PIMSM_ADDR_RP_BIT) &&
              (!(srcFlags & PIMSM_ADDR_WC_BIT)))
      {
        tmpState = L7_FALSE ;
        pimsmDnStrmSGRptPruneProcessing(pimsmCb, &srcAddr, &grpAddr,
                                        rtrIfNum,holdtime);
      }
      else if((!(srcFlags & PIMSM_ADDR_RP_BIT)) &&
              (!(srcFlags & PIMSM_ADDR_WC_BIT)))
      {
        pimsmDnStrmSGPruneProcessing(pimsmCb, &srcAddr, &grpAddr,
                                     rtrIfNum,holdtime);
      }
    } /* while(numPruneSrcs--) */
    /* End of (S,G) and (*,G) Prune handling */
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO,
                "tmpState = %d", tmpState);
    if(tmpState == L7_TRUE)
    {
      pimsmDnStrmSGRptEOMEventPost(pimsmCb, &grpAddr, rtrIfNum);
    }
  } /* for all groups */
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Exit");
  return L7_SUCCESS;
}

/******************************************************************************
*
* @purpose  Receive join/prune pkt processing.
*
* @param    pimsmCb           @b{(input)}  control block
* @param    pSrcAddr          @b{(input)}  ip source address
* @param    pDestAddr         @b{(input)}  ip destination address
* @param    pPimHeader        @b{(input)}  start of pim pkt
* @param    pimPktLen         @b{(input)}  pim pkt length
* @param    rtrIfNum          @b{(input)}  incoming router interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t pimsmJoinPruneRecv(pimsmCB_t *pimsmCb,L7_inet_addr_t *pSrcAddr,
                                  L7_inet_addr_t *pDestAddr,
                                  L7_char8 *pPimHeader, L7_uint32 pimPktLen,
                                  L7_uint32 rtrIfNum)
{
  pimsmInterfaceEntry_t       *pIntfEntry = L7_NULLPTR;
  pim_encod_uni_addr_t encodTargetAddr;
  L7_RC_t rc;
  L7_char8 *pData;
  L7_char8 numGroups;
  L7_char8 reserved;
  L7_short16 holdtime;
  L7_inet_addr_t targetAddr;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "rtrIfNum = %d", rtrIfNum);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pSrcAddr :", pSrcAddr);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pDestAddr :", pDestAddr);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "length = %d", pimPktLen);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "payLoad = %p", pPimHeader);

  memset(&encodTargetAddr, 0 ,sizeof(pim_encod_grp_addr_t));

  if(inetIsAllPimRouterAddress(pDestAddr) == L7_FALSE)
  {
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Wrong dest Addr:",pDestAddr);
    return(L7_FAILURE);
  }
  /* Either a local vif or somehow received PIMSM_JOIN_PRUNE from
   * non-directly connected router. Ignore it.
   */

  if (mcastIpMapIsRtrIntfUnnumbered(pimsmCb->family,rtrIfNum) != L7_TRUE)
  {
    if (inetIsDirectlyConnected(pSrcAddr, rtrIfNum)== L7_FALSE)
    {
      /* Message from non-directly connected router.  */
      PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                  "Ignoring PIMSM_JP from non-neighbor router:");
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "pSrcAddr :",pSrcAddr);
      return(L7_FAILURE);
    }
  }


  if(pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                "PIMSM is not Enabled on intf = %d",
                rtrIfNum);
    return L7_FAILURE;
  }

  rc = pimsmNeighborFind(pimsmCb, pIntfEntry, pSrcAddr, L7_NULLPTR);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                "wrongly recv join/prune on intf = %d",
                rtrIfNum);
    return L7_FAILURE;
  }

  /* sanity check for the minimum length */
  if(pimsmCb->family == L7_AF_INET)
  {
    if(pimPktLen < PIMSM_JOIN_PRUNE_MINLEN_IPV4)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                  "sanity check failed for join/prune msg");
      return L7_FAILURE;
    }
  }
  else if(pimsmCb->family == L7_AF_INET6)
  {
    if(pimPktLen < PIMSM_JOIN_PRUNE_MINLEN_IPV6)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                  "sanity check failed for join/prune msg");
      return L7_FAILURE;
    }
  }
  else
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                "Error ! Wrong family = %d", pimsmCb->family);
    return L7_FAILURE;
  }
  pData = (L7_char8 *)(pPimHeader + PIM_HEARDER_SIZE);

  /* Get the target address */
  PIM_GET_EUADDR_INET(&encodTargetAddr, pData);
  inetAddressSet(encodTargetAddr.addr_family,
                 &(encodTargetAddr.addr),&targetAddr);

  MCAST_GET_BYTE(reserved, pData);
  MCAST_UNUSED_PARAM (reserved);

  MCAST_GET_BYTE(numGroups, pData);
  if(numGroups == 0)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,
                "No indication for groups in the message");
    return(L7_FAILURE);
  }

  MCAST_GET_SHORT(holdtime, pData);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "holdtime : %d",holdtime);
  if(inetUnNumberedIsLocalAddress(&targetAddr, rtrIfNum) != L7_TRUE)
  {
    /********* if I am not the target of the join message **********/
    rc = pimsmNeighborFind(pimsmCb, pIntfEntry,
                           &targetAddr, L7_NULLPTR);
    if(rc != L7_SUCCESS)
    {
      /* neighbor not found */
      PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_ERROR,"neighbor not found intf = %d",
                  rtrIfNum);
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Target addr :",&targetAddr);
      return(L7_FAILURE);
    }
    rc = pimsmJoinPruneNonTargetProcessing(pimsmCb, rtrIfNum, pIntfEntry,
                                             pData, numGroups, holdtime);
  }
  else
  {
    /* I am the target of this join, so process the message */
    rc = pimsmJoinPruneTargetProcessing(pimsmCb, rtrIfNum, pIntfEntry,
                                          pData, numGroups, holdtime);
  }

  PIMSM_TRACE(PIMSM_DEBUG_BIT_JOIN_RX_TX, PIMSM_TRACE_INFO, "Exit");
  return rc;
}


/******************************************************************************
*
* @purpose Find the longest active entry, either (*,G) or (S,G) or (S,G,rpt)
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pGrpAddr    @b{(input)}  group address
* @param    rtrIfNum    @b{(input)}  router interface number
* @param      pParam      @b{(output)} MRT entry node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t pimsmLongestActiveStarGEntryMatch(pimsmCB_t *pimsmCb,
      L7_inet_addr_t *pGrpAddr,  L7_uint32 rtrIfNum, pimsmLongestMatch_t *pParam)
{
  pimsmStarGNode_t    * pStarGNode = L7_NULLPTR;
  pimsmStarStarRpNode_t * pStarStarRpNode = L7_NULLPTR ;
  L7_RC_t rc;
  L7_inet_addr_t * rpAddr = L7_NULLPTR, grpRpAddr;

  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "group :",pGrpAddr);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "rtrIfNum = %d", rtrIfNum);
  rc = pimsmStarGFind(pimsmCb, pGrpAddr, &pStarGNode);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,"pimsmStarGFind() failed ");
    rc = pimsmRpAddressGet(pimsmCb,pGrpAddr, &grpRpAddr);
    if(rc != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,"no RP exists for group ");
      return L7_FAILURE;
    }
    PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "rpAddr :",&grpRpAddr);
    rc = pimsmStarStarRpFind(pimsmCb, &grpRpAddr, &pStarStarRpNode);
    if(rc != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,"pimsmStarStarRpFind() failed");
      return L7_FAILURE;
    }
    pParam->entryType = PIMSM_ENTRY_TYPE_STAR_STAR_RP;
    pParam->mrtNode = (void *)pStarStarRpNode;
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,"(*,*,RP) found");
    return L7_SUCCESS;
  }
  else
  {
    if(pStarGNode->pimsmStarGEntry.pimsmStarGRPFIfIndex != rtrIfNum)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO," RPFIfIndex != rtrIfNum");
      if((pStarGNode->flags & PIMSM_ADDED_TO_MFC) !=L7_NULL )
      {
        pParam->entryType = PIMSM_ENTRY_TYPE_STAR_G;
        pParam->mrtNode = (void *)pStarGNode;
        PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,"(*,G) found");
        return L7_SUCCESS;
      }
      else
      {
        rpAddr = &pStarGNode->pimsmStarGEntry.pimsmStarGRPAddress;
        PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "rpAddr :",rpAddr);
        rc = pimsmStarStarRpFind(pimsmCb, rpAddr, &pStarStarRpNode);
        if(rc != L7_SUCCESS)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,"pimsmStarStarRpFind() failed");
          return L7_FAILURE;
        }
        pParam->entryType = PIMSM_ENTRY_TYPE_STAR_STAR_RP;
        pParam->mrtNode = (void *)pStarStarRpNode;
        PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,"(*,*,RP) found");
        return L7_SUCCESS;
        }
    }
    else
    {
      pParam->entryType = PIMSM_ENTRY_TYPE_STAR_G;
      pParam->mrtNode = (void *)pStarGNode;
      PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,"same incoming (*,G) found");
      return L7_SUCCESS;
    }
  }
}


/******************************************************************************
*
* @purpose Find the longest active entry, either
         (*,*,RP) or (*,G) or (S,G) or (S,G,rpt).
*
* @param    pimsmCb     @b{(input)}  control block
* @param    pSrcAddr    @b{(input)}  source address
* @param    pGrpAddr    @b{(input)}  group address
* @param    rtrIfNum    @b{(input)}  router interface number
* @param      pParam      @b{(output)} MRT entry node
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t pimsmLongestActiveMRTEntryMatch(pimsmCB_t *pimsmCb,
         L7_inet_addr_t *pSrcAddr, L7_inet_addr_t *pGrpAddr, L7_uint32 rtrIfNum,
         pimsmLongestMatch_t *pParam)
{
  pimsmSGNode_t    * pSGNode = L7_NULLPTR;
  pimsmSGRptNode_t * pSGRptNode = L7_NULLPTR ;
  L7_RC_t rc;

  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "srcAddr :",pSrcAddr);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "group :",pGrpAddr);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "rtrIfNum = %d", rtrIfNum);

  rc = pimsmSGFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGNode);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,"pimsmSGFind() failed");
    rc = pimsmSGRptFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGRptNode);
    if(rc != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,"pimsmSGRptFind() failed");
      return pimsmLongestActiveStarGEntryMatch(pimsmCb, pGrpAddr,
         rtrIfNum, pParam);
    }
    if(pSGRptNode->pimsmSGRptEntry.pimsmSGRptRPFIfIndex != rtrIfNum)
    {
      if((pSGRptNode->flags & PIMSM_ADDED_TO_MFC) !=L7_NULL )
      {
        pParam->entryType = PIMSM_ENTRY_TYPE_S_G_RPT;
        pParam->mrtNode = (void *)pSGRptNode;
        PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,"(S,G,rpt) found");
        return L7_SUCCESS;
      }
      else
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,"not active (S,G,rpt) found");
        return pimsmLongestActiveStarGEntryMatch(pimsmCb, pGrpAddr,
         rtrIfNum, pParam);
      }
    }
    else
    {
      pParam->entryType = PIMSM_ENTRY_TYPE_S_G_RPT;
      pParam->mrtNode = (void *)pSGRptNode;
      PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,"same incoming (S,G,rpt) found");
      return L7_SUCCESS;
    }
  }
  else
  {
    if(pSGNode->pimsmSGEntry.pimsmSGRPFIfIndex != rtrIfNum)
    {
      if((pSGNode->flags & PIMSM_ADDED_TO_MFC) !=L7_NULL )
      {
        pParam->entryType = PIMSM_ENTRY_TYPE_S_G;
        pParam->mrtNode = (void *)pSGNode;
        PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,"(S,G) found");
        return L7_SUCCESS;
      }
      else
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,"not active (S,G) found");
        return pimsmLongestActiveStarGEntryMatch(pimsmCb, pGrpAddr,
         rtrIfNum, pParam);
      }
    }
    else
    {
      pParam->entryType = PIMSM_ENTRY_TYPE_S_G;
      pParam->mrtNode = (void *)pSGNode;
      PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,"same incoming (S,G) found");
      return L7_SUCCESS;
    }
  }
}

/******************************************************************************
*
* @purpose Receive Assert pkt (*,G) processing part.
*
* @param    pimsmCb     @b{(input)}  control block
* @param      pStarGNode  @b{(input)}  (*,G) node
* @param      msgPref     @b{(input)}  assert preference in the msg
* @param      msgMetric   @b{(input)}  assert metric in the msg
* @param      pMsgSrcAddr @b{(input)}  ip source address in the msg
* @param      rtrIfNum    @b{(input)}  incoming router interface number
* @param      pSrcAddr    @b{(input)}  encoded source address in the msg
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t pimsmStarGAssertTrigger(pimsmCB_t *pimsmCb,
        pimsmStarGNode_t  *pStarGNode,
        L7_uint32   msgPref, L7_uint32   msgMetric,
        L7_inet_addr_t * pMsgSrcAddr,
        L7_uint32   rtrIfNum,
        L7_inet_addr_t * pSrcAddr)
{
  pimsmStarGEntry_t  *pStarGEntry;
  L7_BOOL     localWins, currAssertWinnerWins, currWinner, couldAssert;
  pimsmStarGIEntry_t  * pStarGIEntry = L7_NULLPTR;
  pimsmPerIntfStarGAssertEventInfo_t  starGAssertEventInfo;
  L7_uint32 localMetric = L7_NULL, localPref= L7_NULL;
  pimsmUpStrmStarGEventInfo_t  upStrmStarGEventInfo;
  L7_BOOL joinDesired;
  L7_inet_addr_t  *pWinnerAddr;
  pimsmInterfaceEntry_t * pIntfEntry;

  pStarGEntry = &pStarGNode->pimsmStarGEntry;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO," rtrIfNum= %d", rtrIfNum);

  pStarGIEntry = pStarGNode->pimsmStarGIEntry[rtrIfNum];
  if(pStarGIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,"pStarGIEntry is not present");
    return L7_FAILURE;
  }
  memset(&upStrmStarGEventInfo, 0, sizeof(pimsmUpStrmStarGEventInfo_t));

  memset(&starGAssertEventInfo, 0, sizeof(pimsmPerIntfStarGAssertEventInfo_t));
  starGAssertEventInfo.rtrIfNum = rtrIfNum;
  inetCopy(&starGAssertEventInfo.srcAddr, pSrcAddr);
  starGAssertEventInfo.eventType =
           PIMSM_ASSERT_STAR_G_SM_EVENT_MAX;

  couldAssert = pimsmStarGICouldAssert(pimsmCb, pStarGNode, rtrIfNum);
  if(couldAssert == L7_TRUE)
  {
    localMetric = pStarGEntry->pimsmStarGRPFRouteMetric;
    localPref  = pStarGEntry->pimsmStarGRPFRouteMetricPref | PIMSM_ASSERT_RPT_BIT;
    if(pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM,  PIMSM_TRACE_ERROR,
        "IntfEntry is not available for rtrIfNum(%d)", rtrIfNum);
      return L7_FAILURE;
    }

    localWins = pimsmCompareMetrics(localPref, localMetric,
           &pIntfEntry->pimsmInterfaceAddr, msgPref, msgMetric, pMsgSrcAddr);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,"pStarGIEntry is present");
    if(inetIsAddressZero(&pStarGIEntry->pimsmStarGIAssertWinnerAddress)
       != L7_TRUE)
    {
      currAssertWinnerWins = pimsmCompareMetrics(
         pStarGIEntry->pimsmStarGIAssertWinnerMetricPref,
          pStarGIEntry->pimsmStarGIAssertWinnerMetric,
          &pStarGIEntry->pimsmStarGIAssertWinnerAddress,
          msgPref, msgMetric, pMsgSrcAddr);
      currWinner = PIMSM_INET_IS_ADDR_EQUAL(pMsgSrcAddr,
          &pStarGIEntry->pimsmStarGIAssertWinnerAddress);

      if(currWinner == L7_TRUE)
      {
        if(currAssertWinnerWins == L7_TRUE && localWins == L7_TRUE)
        {
          starGAssertEventInfo.eventType =
          PIMSM_ASSERT_STAR_G_SM_EVENT_RECV_INFR_ASSERT_FROM_CURR_WINNER;
          PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,
          "Recv inferior Assert from current winner");
        }
        else
        {
          starGAssertEventInfo.eventType =
          PIMSM_ASSERT_STAR_G_SM_EVENT_RECV_ACCEPT_ASSERT_FROM_CURR_WINNER;
          inetCopy(&starGAssertEventInfo.winnerAddress, pMsgSrcAddr);
          starGAssertEventInfo.winnerMetricPref = msgPref;
          starGAssertEventInfo.winnerMetric = msgMetric;
          PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,
          "Recv acceptable Assert from current winner");
        }
      }
      else
      {
        if(currAssertWinnerWins == L7_FALSE)
        {
          starGAssertEventInfo.eventType =
          PIMSM_ASSERT_STAR_G_SM_EVENT_RECV_PREF_ASSERT;
          inetCopy(&starGAssertEventInfo.winnerAddress, pMsgSrcAddr);
          starGAssertEventInfo.winnerMetricPref = msgPref;
          starGAssertEventInfo.winnerMetric = msgMetric;
          PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,
            "Recv preferred Assert");
        }
        else
        {
          starGAssertEventInfo.eventType =
           PIMSM_ASSERT_STAR_G_SM_EVENT_RECV_INFR_ASSERT;
          PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,
            "Recv inferior Assert");
        }
      }
    }
    else
    {
      if(localWins == L7_TRUE)
      {
        starGAssertEventInfo.eventType =
        PIMSM_ASSERT_STAR_G_SM_EVENT_RECV_INFR_ASSERT_COULD_ASSERT;
        PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,
        "(couldAssert == True)");
      }
      else
      {
        if(pimsmStarGIAssertTrackingDesired(pimsmCb, pStarGNode,rtrIfNum)
           == L7_TRUE)
         {
         starGAssertEventInfo.eventType =
         PIMSM_ASSERT_STAR_G_SM_EVENT_RECV_ACCEPT_ASSERT;
          PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,
            "Recv acceptable Assert & (assert_tracking_desired == true)");
         }
         else
         {
           PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,
           "Error : (localWins == L7_FALSE) && (assert_tracking_desired == false)");
         }
      }
    }
    if (starGAssertEventInfo.eventType ==
             PIMSM_ASSERT_STAR_G_SM_EVENT_MAX)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,
          "Error : Wrong eventType for (*,G) Assert");
      return L7_FAILURE;
    }
    inetAddressZeroSet(pimsmCb->family,&starGAssertEventInfo.srcAddr);
    pimsmPerIntfStarGAssertExecute(pimsmCb, pStarGNode,&starGAssertEventInfo);
    memset(&upStrmStarGEventInfo, 0 ,
           sizeof(pimsmUpStrmStarGEventInfo_t));

    joinDesired = pimsmStarGJoinDesired(pimsmCb, pStarGNode);
    if(joinDesired == L7_TRUE)
    {
     /*Do not send Join .Only send a prune to the upstream if assert loser and oif is NULL */
      PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,
          "(*.G) Join Desired is TRUE.It is not LOSER");
    }
    else
    {
      upStrmStarGEventInfo.eventType =
       PIMSM_UPSTRM_STAR_G_SM_EVENT_JOIN_DESIRED_FALSE;
      PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,
          "(*,G) Join Desired is FALSE.It is LOSER");

      pimsmUpStrmStarGExecute(pimsmCb, pStarGNode,
                              &upStrmStarGEventInfo);


    }
    if (pimsmStarGMFCUpdate(pimsmCb,pStarGNode,MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,
        "Failed to update (*,G)");
      return L7_FAILURE;
    }
  }
  else if(pStarGEntry->pimsmStarGRPFIfIndex == rtrIfNum)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,"Recv assert on incoming");
    /* Assert received on incoming */
    localPref = pStarGIEntry->pimsmStarGIAssertWinnerMetricPref;
    localMetric = pStarGIEntry->pimsmStarGIAssertWinnerMetric;
    pWinnerAddr = &pStarGIEntry->pimsmStarGIAssertWinnerAddress;
    localWins = pimsmCompareMetrics(localPref, localMetric,
          pWinnerAddr, msgPref, msgMetric, pMsgSrcAddr);
    if(localWins == L7_TRUE)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,"local wins");
      return L7_SUCCESS;
    }
    pStarGIEntry->pimsmStarGIAssertWinnerMetric = msgMetric;
    pStarGIEntry->pimsmStarGIAssertWinnerMetricPref =msgPref;
    inetCopy(&pStarGIEntry->pimsmStarGIAssertWinnerAddress,pMsgSrcAddr);
    if((PIMSM_INET_IS_ADDR_EQUAL(&pStarGEntry->pimsmStarGUpstreamNeighbor,
         pMsgSrcAddr) == L7_FALSE))
    {
      inetCopy(&pStarGEntry->pimsmStarGUpstreamNeighbor,pMsgSrcAddr);
      /* UPSTRM_STAR_G_RPF_CHANGED_DUE_TO_ASSERT */
       PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,
             "upstrm RPF changed due to assert");
      upStrmStarGEventInfo.eventType =
      PIMSM_UPSTRM_STAR_G_SM_EVENT_RPF_CHANGED_DUE_TO_ASSERT;
      pimsmUpStrmStarGExecute(pimsmCb, pStarGNode, &upStrmStarGEventInfo);

    }
  }
  return L7_SUCCESS;
}
/******************************************************************************
*
* @purpose Receive Assert pkt (S,G) processing part.
*
* @param    pimsmCb     @b{(input)}  control block
* @param      pStarGNode  @b{(input)}  (S,G) node
* @param      msgPref     @b{(input)}  assert preference in the msg
* @param      msgMetric   @b{(input)}  assert metric in the msg
* @param      pMsgSrcAddr @b{(input)}  ip source address in the msg
* @param      rtrIfNum    @b{(input)}  incoming router interface number
* @param      rptBitSet   @b{(input)}  rpt-bit in the msg
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t pimsmSGAssertTrigger(pimsmCB_t *pimsmCb, pimsmSGNode_t  *pSGNode,
                                    L7_uint32   msgPref, L7_uint32   msgMetric,
                                    L7_inet_addr_t * pMsgSrcAddr,
                                    L7_uint32   rtrIfNum,
                                    L7_uint32 rptBitSet)
{
  pimsmSGEntry_t  *pSGEntry;
  L7_BOOL     localWins, currAssertWinnerWins, currWinner, couldAssert;
  pimsmSGIEntry_t  * pSGIEntry = L7_NULLPTR;
  pimsmPerIntfSGAssertEventInfo_t  sgAssertEventInfo;
  L7_uint32 localMetric = L7_NULL, localPref= L7_NULL;
  pimsmUpStrmSGEventInfo_t  upStrmSGEventInfo;
  L7_inet_addr_t  *pWinnerAddr;
  L7_BOOL joinDesired;
  pimsmInterfaceEntry_t * pIntfEntry;

  pSGEntry = &pSGNode->pimsmSGEntry;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO," rtrIfNum= %d", rtrIfNum);

  pSGIEntry = pSGNode->pimsmSGIEntry[rtrIfNum];
  if(pSGIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,"pSGIEntry is not present");
    return L7_FAILURE;
  }
  memset(&upStrmSGEventInfo, 0, sizeof(pimsmUpStrmSGEventInfo_t));

  memset(&sgAssertEventInfo, 0, sizeof(pimsmPerIntfSGAssertEventInfo_t));
  sgAssertEventInfo.rtrIfNum = rtrIfNum;
  sgAssertEventInfo.eventType =
             PIMSM_ASSERT_S_G_SM_EVENT_MAX;
  msgPref = msgPref | rptBitSet;
  couldAssert = pimsmSGICouldAssert(pimsmCb, pSGNode, rtrIfNum);
  if(couldAssert == L7_TRUE)
  {
    localMetric = pSGEntry->pimsmSGRPFRouteMetric;
    localPref = pSGEntry->pimsmSGRPFRouteMetricPref;

    if(pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM,  PIMSM_TRACE_ERROR,
        "IntfEntry is not available for rtrIfNum(%d)", rtrIfNum);
      return L7_FAILURE;
    }
    localWins = pimsmCompareMetrics(localPref, localMetric,
           &pIntfEntry->pimsmInterfaceAddr, msgPref, msgMetric, pMsgSrcAddr);

    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,"pSGIEntry is present");
    if(inetIsAddressZero(&pSGIEntry->pimsmSGIAssertWinnerAddress)
       != L7_TRUE)
    {
      currAssertWinnerWins =
         pimsmCompareMetrics(pSGIEntry->pimsmSGIAssertWinnerMetricPref,
                                      pSGIEntry->pimsmSGIAssertWinnerMetric,
                                      &pSGIEntry->pimsmSGIAssertWinnerAddress,
                                      msgPref, msgMetric, pMsgSrcAddr);

      currWinner = PIMSM_INET_IS_ADDR_EQUAL(pMsgSrcAddr,
                                        &pSGIEntry->pimsmSGIAssertWinnerAddress);
      if(currWinner == L7_TRUE)
      {
        if(currAssertWinnerWins == L7_TRUE && localWins == L7_TRUE)
        {
          sgAssertEventInfo.eventType =
          PIMSM_ASSERT_S_G_SM_EVENT_RECV_INFR_ASSERT_FROM_CURR_WINNER;
          PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,
          "Recv inferior Assert from current winner");
        }
        else
        {
          sgAssertEventInfo.eventType =
          PIMSM_ASSERT_S_G_SM_EVENT_RECV_ACCEPT_ASSERT_FROM_CURR_WINNER;
          inetCopy(&sgAssertEventInfo.winnerAddress, pMsgSrcAddr);
          sgAssertEventInfo.winnerMetricPref = msgPref;
          sgAssertEventInfo.winnerMetric = msgMetric;
          PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,
          "Recv acceptable Assert from current winner");
        }
      }
      else
      {
       if(currAssertWinnerWins == L7_FALSE)
       {
         sgAssertEventInfo.eventType =
         PIMSM_ASSERT_S_G_SM_EVENT_RECV_PREF_ASSERT;
         inetCopy(&sgAssertEventInfo.winnerAddress, pMsgSrcAddr);
         sgAssertEventInfo.winnerMetricPref = msgPref ;
         sgAssertEventInfo.winnerMetric = msgMetric;
         PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,
           "Recv preferred Assert");
       }
       else
       {
         sgAssertEventInfo.eventType =
         PIMSM_ASSERT_S_G_SM_EVENT_RECV_INFR_ASSERT;
         PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,
           "Recv inferior Assert");
       }
      }
    }
    else
    {
       if (rptBitSet && (couldAssert == L7_TRUE))
      {
         sgAssertEventInfo.eventType =
          PIMSM_ASSERT_S_G_SM_EVENT_RECV_ASSERT_RPT;
           PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,
             "(couldAssert == True) & RPTbit is set");
      }
      else
      {
        if(!rptBitSet)
        {
          if(localWins == L7_TRUE)
          {
           if(couldAssert == L7_TRUE)
           {
             sgAssertEventInfo.eventType =
             PIMSM_ASSERT_S_G_SM_EVENT_RECV_INFR_ASSERT_COULD_ASSERT;
              PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,
             "(couldAssert == True)");
           }
           else
           {
              PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,
             "Error : (localWins == L7_TRUE) && (couldAssert == false) ");
           }
          }
          else
          {
             if(pimsmSGIAssertTrackingDesired(pimsmCb, pSGNode,rtrIfNum)
               == L7_TRUE)
             {
               sgAssertEventInfo.eventType =
               PIMSM_ASSERT_S_G_SM_EVENT_RECV_ACCEPT_ASSERT;
                PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,
                  "Recv acceptable Assert & (assert_tracking_desired == true)");
             }
             else
             {
                PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,
               "Error : (localWins == L7_FALSE) && (assert_tracking_desired  == false) ");
             }
          }
        }
        else
        {
           PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,
          "Error : RPTbit is set");
        }
      }
    }
    if (sgAssertEventInfo.eventType ==
             PIMSM_ASSERT_S_G_SM_EVENT_MAX)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,
          "Error : Wrong eventType for (S,G) Assert");
      return L7_FAILURE;
    }
    pimsmPerIntfSGAssertExecute(pimsmCb, pSGNode,&sgAssertEventInfo);

    memset(&upStrmSGEventInfo,0,sizeof(pimsmUpStrmSGEventInfo_t));
    joinDesired = pimsmSGJoinDesired(pimsmCb, pSGNode);
    if (joinDesired == L7_TRUE)
    {
      /*Do Not Do Anything .Send a prune to upstream only if We are assert loser and oif is NULL*/
      PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,
          "(S,G) Join Desired is TRUE.It is not LOSER");
    }
    else
    {
         upStrmSGEventInfo.eventType
          = PIMSM_UPSTRM_S_G_SM_EVENT_JOIN_DESIRED_FALSE;
         PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,
          "(S,G) Join Desired is FALSE.It is LOSER");
         pimsmUpStrmSGExecute(pimsmCb, pSGNode, &upStrmSGEventInfo);
    }
    if (pimsmSGMFCUpdate(pimsmCb,pSGNode, MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,
        "Failed to update (S,G) exact");
      return L7_FAILURE;
    }
  }
  else if(pSGEntry->pimsmSGRPFIfIndex == rtrIfNum)
  {
    /* Assert received on incoming */
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,"Recv assert on incoming");
    localPref = pSGIEntry->pimsmSGIAssertWinnerMetricPref;
    localMetric = pSGIEntry->pimsmSGIAssertWinnerMetric;
    pWinnerAddr = &pSGIEntry->pimsmSGIAssertWinnerAddress;
    localWins = pimsmCompareMetrics(localPref, localMetric,
           pWinnerAddr, msgPref, msgMetric, pMsgSrcAddr);
    if(localWins == L7_TRUE)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,"local wins");
      return L7_SUCCESS;
    }
    pSGIEntry->pimsmSGIAssertWinnerMetric = msgMetric;
    pSGIEntry->pimsmSGIAssertWinnerMetricPref =msgPref;
    inetCopy(&pSGIEntry->pimsmSGIAssertWinnerAddress,pMsgSrcAddr);
    if((PIMSM_INET_IS_ADDR_EQUAL(&pSGEntry->pimsmSGUpstreamNeighbor,
         pMsgSrcAddr) == L7_FALSE))
    {
      inetCopy(&pSGEntry->pimsmSGUpstreamNeighbor,pMsgSrcAddr);
      /* UPSTRM_S_G_RPF_CHANGED_DUE_TO_ASSERT */
      PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO,
             "upstrm RPF changed due to assert");
      upStrmSGEventInfo.eventType =
      PIMSM_UPSTRM_S_G_SM_EVENT_RPF_CHANGED_DUE_TO_ASSERT;
      pimsmUpStrmSGExecute(pimsmCb, pSGNode, &upStrmSGEventInfo);
    }
  }
  return L7_SUCCESS;
}


/******************************************************************************
*
* @purpose  Receive Assert pkt processing.
*
* @param    pimsmCb           @b{(input)}  control block
* @param    pSrcAddr          @b{(input)}  ip source address
* @param    pDestAddr         @b{(input)}  ip destination address
* @param    pPimHeader        @b{(input)}  start of pim pkt
* @param    pimPktLen         @b{(input)}  pim pkt length
* @param    rtrIfNum          @b{(input)}  incoming router interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
static L7_RC_t pimsmAssertRecv(pimsmCB_t *pimsmCb,L7_inet_addr_t *pSrcAddr,
                               L7_inet_addr_t *pDestAddr,
                               L7_char8 *pPimHeader, L7_uint32 pimPktLen,
                               L7_uint32 rtrIfNum)
{
  pimsmInterfaceEntry_t * pIntfEntry= L7_NULLPTR;
  pim_encod_uni_addr_t encodSrcAddr;
  pim_encod_grp_addr_t encodGrpAddr;
  L7_inet_addr_t srcAddr, grpAddr;
  L7_uchar8  *pData;
  L7_RC_t rc;
  L7_uint32 assertPref;
  L7_uint32 assertMetric;
  L7_uint32 assertRptBit;
  pimsmLongestMatch_t param;
  pimsmStarGNode_t    * pStarGNode = L7_NULLPTR;
  pimsmSGNode_t    * pSGNode = L7_NULLPTR;
  pimsmSGRptNode_t * pSGRptNode = L7_NULLPTR ;
  /*
   * Either a local vif or somehow received PIMSM_HELLO from
   * non-directly connected router. Ignore it else get interface number
   */

  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "rtrIfNum = %d", rtrIfNum);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "pSrcAddr :", pSrcAddr);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "pDestAddr :", pDestAddr);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "length = %d", pimPktLen);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "payLoad = %p", pPimHeader);

  if (mcastIpMapIsRtrIntfUnnumbered(pimsmCb->family,rtrIfNum) != L7_TRUE)
  {
    if (inetIsDirectlyConnected(pSrcAddr, rtrIfNum)== L7_FALSE)
    {
      /* Message from non-directly connected router.  */
      PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,
                  "Ignoring PIMSM_HELLO from non-neighbor router:");
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "pSrcAddr :",pSrcAddr);
      return(L7_FAILURE);
    }
  }

  if(pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,"PIMSM is not Enabled on intf = %d",
                rtrIfNum);
    return L7_FAILURE;
  }

  rc = pimsmNeighborFind(pimsmCb, pIntfEntry, pSrcAddr, L7_NULLPTR);
  if(rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,"wrongly recv assert on intf = %d",
                rtrIfNum);
    return L7_FAILURE;
  }

  pData = (L7_uchar8 *)(pPimHeader + PIM_HEARDER_SIZE);

  /* Get the group and srcAddr addresses */
  PIM_GET_EGADDR_INET(&encodGrpAddr, pData);
  PIM_GET_EUADDR_INET(&encodSrcAddr, pData);

  /* Get the metric related info */
  MCAST_GET_LONG(assertPref, pData);
  MCAST_GET_LONG(assertMetric, pData);
  assertRptBit = assertPref & PIMSM_ASSERT_RPT_BIT;

  MCAST_UNUSED_PARAM (pData);

  inetAddressSet(pimsmCb->family, (void*)&(encodSrcAddr.addr) ,&srcAddr);
  inetAddressSet(pimsmCb->family, (void*)&(encodGrpAddr.addr) ,&grpAddr);

  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "msg srcAddr :", &srcAddr);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "msg grpAddr :", &grpAddr);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "assertPref = 0x%x", assertPref);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "assertMetric = 0x%x", assertMetric);
  /* If the Assert arrived on the  outgoing interface ,
     Find the longest  "active" entry, i.e. the one with a kernel mirror */

  /* If the Assert arrived on the incoming interface of an existing (S,G),
  (*,G), or (*,*,RP) entry, the Assert is processed as follows.  If the
  Assert message does not match the entry, exactly, it is ignored; i.e,
  longest-match is not used in this case. */

  rc = pimsmLongestActiveMRTEntryMatch(pimsmCb, &srcAddr, &grpAddr,
      rtrIfNum, &param);
  if(rc !=L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,"no MRT entry exists");
    return L7_FAILURE;
  }
  switch(param.entryType)
  {
    case PIMSM_ENTRY_TYPE_STAR_G:
      pStarGNode = (pimsmStarGNode_t*)param.mrtNode;
      if(pStarGNode == (pimsmStarGNode_t *)L7_NULLPTR)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,"No routing entry. Ignore the assert");
        return L7_FAILURE;
      }
      if((pStarGNode->pimsmStarGEntry.pimsmStarGRPFIfIndex != rtrIfNum) &&
         ((pStarGNode->flags & PIMSM_ADDED_TO_MFC) ==L7_NULL ))
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,"No active entry. Ignore the assert");
        return L7_FAILURE;
      }
      if(assertRptBit)
      {
        pimsmStarGAssertTrigger(pimsmCb, pStarGNode, assertPref,
                                assertMetric, pSrcAddr, rtrIfNum, &srcAddr);
      }
      else
      {
        /* RFC 3.5.2 -- create (S,G)RPT-bit entry
        */
        /*
        With reference to section 3.5.2 in RFC2362, It says If we have an
        active matching entry as (*, G) then create a (S, G) Rpt-bit entry
        and try to forward the assert message. But this way router with (*,G)
        or (S,G)Rpt-bit entry will be always loser as Router with (S,G) entry
        has better metric than Router with (S,G)Rpt-bit entry.
        Hence Router with (S,G)Rpt-bit will never gain the winner status.
        If you create a (S, G) entry instead if (S, G) Rpt-bit entry. It again
        get the winner-status. */
        rc = pimsmSGNodeCreate(pimsmCb,&srcAddr, &grpAddr, &pSGNode,L7_TRUE);
        if(rc !=L7_SUCCESS)
        {
          PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,"cant add entry for");
          PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "group :",&grpAddr);
          PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "srcAddr :",&srcAddr);
          return L7_FAILURE;
        }
        pimsmSGAssertTrigger(pimsmCb, pSGNode, assertPref,
                             assertMetric, pSrcAddr, rtrIfNum, assertRptBit);
      }
      break;
    case PIMSM_ENTRY_TYPE_S_G:
      pSGNode = (pimsmSGNode_t*)param.mrtNode;
      if(pSGNode == (pimsmSGNode_t *)L7_NULLPTR)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,"No routing entry. Ignore the assert");
        return L7_FAILURE;
      }
      if((pSGNode->pimsmSGEntry.pimsmSGRPFIfIndex != rtrIfNum) &&
         ((pSGNode->flags & PIMSM_ADDED_TO_MFC) ==L7_NULL ))
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,"No active entry. Ignore the assert");
        return L7_FAILURE;
      }
      pimsmSGAssertTrigger(pimsmCb, pSGNode, assertPref,
                             assertMetric, pSrcAddr, rtrIfNum, assertRptBit);
      break;
    case PIMSM_ENTRY_TYPE_S_G_RPT:
      pSGRptNode = (pimsmSGRptNode_t*)param.mrtNode;
      if(pSGRptNode == (pimsmSGRptNode_t *)L7_NULLPTR)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,"No routing entry. Ignore the assert");
        return L7_FAILURE;
      }
      if((pSGRptNode->pimsmSGRptEntry.pimsmSGRptRPFIfIndex != rtrIfNum) &&
         ((pSGRptNode->flags & PIMSM_ADDED_TO_MFC) ==L7_NULL ))
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,"No active entry. Ignore the assert");
        return L7_FAILURE;
      }
      PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,"(S,G,rpt) entry - Ignore the assert");
      break;
    case PIMSM_ENTRY_TYPE_STAR_STAR_RP:
    case PIMSM_ENTRY_TYPE_NONE:
    default:
      PIMSM_TRACE(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_ERROR,"no entry exists for");
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "group :",&grpAddr);
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_ASSERT_FSM, PIMSM_TRACE_INFO, "srcAddr :",&srcAddr);
      return L7_FAILURE;
  }
  return(L7_SUCCESS);
}

/******************************************************************************
*
* @purpose  Receive PIM Control pkt Processing.
*
* @param    pimsmCb           @b{(input)}  control block
* @param    pSrcAddrN         @b{(input)}  ip source address
* @param    pDestAddrN        @b{(input)}  ip destination address
* @param    pPimHeader        @b{(input)}  start of pim pkt
* @param    pimPktLen         @b{(input)}  pim pkt length
* @param    rtrIfNum          @b{(input)}  incoming router interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     none
*
* @end
*
******************************************************************************/
L7_RC_t pimsmCtrlPktRecv(pimsmCB_t *pimsmCb,L7_inet_addr_t *pSrcAddrN,
                         L7_inet_addr_t *pDestAddrN,
                         L7_char8 *pPimHeader, L7_uint32 pimPktLen,
                         L7_uint32 rtrIfNum, L7_uchar8 ipTypeOfService)
{
  L7_char8 *pData;
  L7_uchar8 *cp;
  L7_char8 cc;
  L7_int32 type = 0, version = 0, reserved = 0,len=0;
  L7_inet_addr_t srcAddr, destAddr;
  L7_ushort16 pktChkSum, calChkSum;

  pimsmDebugPacketRxTxTrace(pimsmCb->family, L7_TRUE, rtrIfNum,
                            pSrcAddrN, pDestAddrN, pPimHeader, pimPktLen);


  if(pimsmCb->isPimsmEnabled != L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR,
         "PIMSM: Received PIM Packet but PIMSM is not enabled\n");
    return L7_FAILURE;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_NOTICE, "Received PIM Packet on rtrIfNum (%d)...",
      rtrIfNum);
  PIMSM_TRACE_PKT(PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_NOTICE, pPimHeader, pimPktLen);
  if(pimsmIntfEntryGet(pimsmCb, rtrIfNum, L7_NULLPTR) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR,"PIMSM: Received PIM Packet"
      "but PIMSM is not Enabled on interface (%d)\n", rtrIfNum);
    pimsmStatsIncrement(pimsmCb, rtrIfNum, PIMSM_MAX_TYPE, PIMSM_RX);
    return L7_FAILURE;
  }

  inetAddrNtoh(pSrcAddrN, &srcAddr);
  inetAddrNtoh(pDestAddrN, &destAddr);

  /*Processing PIM header */
  pData = (L7_char8 *)pPimHeader;

  MCAST_GET_BYTE (cc, pData);
  version = cc >> 4;
  type = cc & 0x0f;
  MCAST_GET_BYTE (reserved, pData);
  MCAST_GET_SHORT (pktChkSum, pData);
  MCAST_UNUSED_PARAM (pData);

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_DEBUG, "recv checksum  = 0x%x", pktChkSum);
  if(version != PIM_PROTOCOL_VERSION)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR,
      "PIMSM: Received PIMSM Packet with wrong version (%d)\n", version);
    pimsmStatsIncrement(pimsmCb, rtrIfNum, PIMSM_MAX_TYPE, PIMSM_RX);
    return L7_FAILURE;
  }

  if(reserved)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR,
      "PIMSM: Received PIMSM Packet with non-zero reserved field\n");
    pimsmStatsIncrement(pimsmCb, rtrIfNum, PIMSM_MAX_TYPE, PIMSM_RX);
    return L7_FAILURE;
  }

  if(type >= PIMSM_MAX_TYPE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR,
      "PIMSM: Received PIMSM Packet with wrong type (%d)\n", type);
    pimsmStatsIncrement(pimsmCb, rtrIfNum, PIMSM_MAX_TYPE, PIMSM_RX);
    return L7_FAILURE;
  }

  if ( (type == PIMSM_HELLO) ||
      (type == PIMSM_JOIN_PRUNE) ||
      (type == PIMSM_ASSERT))
  {
   /* should be ALL-PIM-ROUTERs */
   if(inetIsAllPimRouterAddress(&destAddr) == L7_FALSE)
   {
     PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_INFO,
           "PIMSM: Received PIMSM Packet with wrong dest Addr:",&destAddr);
     pimsmStatsIncrement(pimsmCb, rtrIfNum, PIMSM_MAX_TYPE, PIMSM_RX);
     return(L7_FAILURE);
   }
  }
  else if ((type == PIMSM_REGISTER) ||
      (type == PIMSM_REGISTER_STOP) ||
      (type == PIMSM_CAND_RP_ADV))
  {
   /* should be valid uni-cast address */
   if(inetIsValidHostAddress(&destAddr) == L7_FALSE)
   {
     PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_INFO,
           "PIMSM: Received PIMSM Packet with wrong dest Addr:",&destAddr);
     pimsmStatsIncrement(pimsmCb, rtrIfNum, PIMSM_MAX_TYPE, PIMSM_RX);
     return(L7_FAILURE);
   }
  }
 if (pimsmCb->family == L7_AF_INET)
 {
  if(type == PIMSM_REGISTER)
  {
    /*
     * For PIM_REGISTER the checksum does not include the inner IP packet.
     * However, some older routers (Cisco) might calculate the checksum
     * over the whole packet. Hence, verify the checksum over the
     * first 8 bytes, and if it fails, then over the whole Register.
     */
     calChkSum = inetChecksum(pPimHeader, 8);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_DEBUG, "calc ipv4 checksum (1) = 0x%x", calChkSum);
    if(calChkSum != 0)
    {
      calChkSum = inetChecksum(pPimHeader, pimPktLen);
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_DEBUG, "calc ipv4 checksum (2) = 0x%x", calChkSum);
      if(calChkSum != 0)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR,
         "PIMSM: Received PIMSM Packet with wrong checksum (%d)\n", calChkSum);
        pimsmStatsIncrement(pimsmCb, rtrIfNum, PIMSM_MAX_TYPE, PIMSM_RX);
        return L7_FAILURE;
      }
    }
  } else {
     calChkSum = inetChecksum(pPimHeader, pimPktLen);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_DEBUG, "calc ipv4 checksum (3) = 0x%x", calChkSum);
    if(calChkSum != 0)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR,
         "PIMSM: Received PIMSM Packet with wrong checksum (%d)\n", calChkSum);
      pimsmStatsIncrement(pimsmCb, rtrIfNum, PIMSM_MAX_TYPE, PIMSM_RX);
      return L7_FAILURE;
    }
  }
 }
  else if(pimsmCb->family == L7_AF_INET6)
  {
    len = (type == PIMSM_REGISTER)? 8: pimPktLen;

    cp = pimsmCb->pseudoIpv6Hdr;
    MCAST_PUT_DATA(&srcAddr.addr.ipv6.in6.addr8,16, cp);
    MCAST_PUT_DATA(&destAddr.addr.ipv6.in6.addr8,16,cp);
    MCAST_PUT_LONG(len, cp);

    MCAST_PUT_BYTE(0, cp);
    MCAST_PUT_BYTE(0, cp);
    MCAST_PUT_BYTE(0, cp);
    MCAST_PUT_BYTE(IP_PROT_PIM, cp);

    /* append the PIM pData */
    MCAST_PUT_DATA(pPimHeader,len,cp);

    calChkSum = inetChecksum((void*)pimsmCb->pseudoIpv6Hdr, cp - pimsmCb->pseudoIpv6Hdr);
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC, PIMSM_TRACE_DEBUG,"Checksum is %p\r\n",calChkSum);

    if(calChkSum != 0)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR,
         "PIMSM: Received PIMSM Packet with wrong checksum (%d)\n", calChkSum);
      pimsmStatsIncrement(pimsmCb, rtrIfNum, PIMSM_MAX_TYPE, PIMSM_RX);
      return L7_FAILURE;
    }
  }
    if(inetUnNumberedIsLocalAddress(&srcAddr, rtrIfNum) != L7_FALSE)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_INFO, "My own packet, came back to me. Hmm");
      if(type != PIMSM_HELLO)
      {
        PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR,
         "PIMSM: Received our own PIMSM Packet.\n");
        pimsmStatsIncrement(pimsmCb, rtrIfNum, PIMSM_MAX_TYPE, PIMSM_RX);
        return L7_FAILURE;
      }
    }

  if(rtrIfNum == PIMSM_REGISTER_INTF_NUM)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR,
      "PIMSM: Received PIMSM Packet with wrong rtrIfNum (%d)\n", rtrIfNum);
    pimsmStatsIncrement(pimsmCb, rtrIfNum, PIMSM_MAX_TYPE, PIMSM_RX);
    return(L7_FAILURE);
  }
  pimsmStatsIncrement(pimsmCb, rtrIfNum, type, PIMSM_RX);

  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_DEBUG, "Msg type = %d", type);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_DEBUG, "rtrIfNum = %d", rtrIfNum);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_DEBUG, "srcAddr :", &srcAddr);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_DEBUG, "destAddr :", &destAddr);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_DEBUG, "length = %d", pimPktLen);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_DEBUG, "payLoad = %p", pPimHeader);


  switch(type)
  {
    case PIMSM_HELLO:
      pimsmHelloRecv(pimsmCb, &srcAddr, &destAddr, pPimHeader,
         pimPktLen, rtrIfNum);
      break;
    case PIMSM_JOIN_PRUNE:
      pimsmJoinPruneRecv(pimsmCb, &srcAddr, &destAddr, pPimHeader,
         pimPktLen, rtrIfNum);
      break;
    case PIMSM_ASSERT:
      pimsmAssertRecv(pimsmCb, &srcAddr, &destAddr, pPimHeader,
         pimPktLen, rtrIfNum);
      break;
    case PIMSM_REGISTER:
      pimsmRegisterRecv(pimsmCb, &srcAddr, &destAddr, pPimHeader,
         pimPktLen, rtrIfNum, ipTypeOfService);
      break;
    case PIMSM_REGISTER_STOP:
      pimsmRegisterStopRecv(pimsmCb, &srcAddr, &destAddr, pPimHeader,
         pimPktLen, rtrIfNum);
      break;
    case PIMSM_CAND_RP_ADV:
      pimsmBsrCandRpAdvRecv(pimsmCb, &srcAddr, &destAddr, pPimHeader,
         pimPktLen, rtrIfNum);
      break;
    case PIMSM_BOOTSTRAP:
      pimsmBsrBootstrapRecv(pimsmCb, &srcAddr, &destAddr, pPimHeader,
         pimPktLen, rtrIfNum);
      break;
    default:
      PIMSM_TRACE(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_ERROR,
         "PIMSM: Ignoring unknown PIMSM Packet with type (%d)\n", type);
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_INFO, "srcAddr :",&srcAddr);
      PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_MISC,  PIMSM_TRACE_INFO, "destAddr :",&destAddr);
      break;
  }
  return L7_SUCCESS;
}

