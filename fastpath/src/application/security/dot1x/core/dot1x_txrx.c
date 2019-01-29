/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dot1x_txrx.c
*
* @purpose   dot1x Transmit and Receive PDUs
*
* @component dot1x
*
* @comments  none
*
* @create    04/08/2003
*
* @author    mfiorito
*
* @end
*
**********************************************************************/

#define L7_MAC_EAPOL_PDU

#include "dot1x_include.h"
#include "dot1q_api.h"

static L7_uchar8 temp[DOT1X_MAX_PDU_SIZE];

/**************************************************************************
*
* @purpose   Init txrx component
*
* @param     none
*
* @returns   L7_SUCCESS or L7_FAILURE
*
* @comments  register the receive function with sysnet
*
* @end
*
*************************************************************************/
L7_RC_t dot1xTxRxInit(void)
{
  L7_RC_t rc;
  sysnetNotifyEntry_t snEntry;

  PT_LOG_INFO(LOG_CTX_STARTUP,"Going to register dot1xPduReceive related to type=%u, protocol_type=%u: 0x%08x",
           SYSNET_ETHERTYPE_ENTRY, L7_ETYPE_EAPOL, (L7_uint32) dot1xPduReceive);

  memset(&snEntry, 0x00, sizeof(snEntry));
  /*Register the pdu receive function with sysnet utility*/
  osapiStrncpy(snEntry.funcName, "dot1xPduReceive", sizeof(snEntry.funcName));
  snEntry.notify_pdu_receive = dot1xPduReceive;
  snEntry.type = SYSNET_ETHERTYPE_ENTRY;
  snEntry.u.protocol_type = L7_ETYPE_EAPOL;
  rc = sysNetRegisterPduReceive(&snEntry);
  return rc;
}

/*********************************************************************
* @purpose This routine receives any PAE PDU and sends it to the
*          dot1x message queue and dot1xTask
*
* @param   bufHandle    @b{(input)} handle to the network buffer the PAE PDU is stored in
* @param   bpduInfo     @b{(input)} pointer to sysnet structure which contains the internal
*                                     interface number that the PDU was received on
*
* @returns L7_SUCCESS on a successful operation
* @returns L7_FAILURE for any error
*
* @comments This routine is registered with sysnet ethernet type 0x888E at
*           dot1x init time.
*
* @end
*********************************************************************/
L7_RC_t dot1xPduReceive(L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo)
{
  L7_uchar8 *data, baseMac[L7_MAC_ADDR_LEN];
  L7_uint32  maxFrameSize,dataLength;
  L7_ushort16 protocolType, temp16;
  L7_uint32 vlanId ;
  L7_8021QTag_t       *vlanTagPtr;
  L7_uchar8             capabilities=0;

  /* If admin mode not enabled, don't process frame */
  if (dot1xCfg->dot1xAdminMode != L7_ENABLE)
    return L7_FAILURE;

  /* Ensure we received this frame on a physical port */
  if (dot1xIsValidIntf(pduInfo->intIfNum) != L7_TRUE)
    return L7_FAILURE;

  /* Get base MAC address (could be BIA or LAA) - should match src addr in frame */
  if (simGetSystemIPMacType() == L7_SYSMAC_BIA)
    simGetSystemIPBurnedInMac(baseMac);
  else
    simGetSystemIPLocalAdminMac(baseMac);

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, dataLength);

  /* Determine the protocol type */
  memcpy ((L7_uchar8 *)&protocolType, data+L7_ENET_HDR_SIZE, sizeof(L7_ushort16));
  temp16 = osapiNtohs(protocolType);
  protocolType = temp16;

  if (protocolType == L7_ETYPE_8021Q)
  {
    vlanTagPtr = (L7_8021QTag_t*)(data+L7_ENET_HDR_SIZE);
    vlanId = vlanTagPtr->tci & L7_VLAN_TAG_VLAN_ID_MASK;
    memcpy ((L7_uchar8 *)&protocolType, data+L7_ENET_HDR_SIZE+L7_8021Q_ENCAPS_HDR_SIZE, sizeof(L7_ushort16));
    temp16 = osapiNtohs(protocolType);
    protocolType = temp16;
  }
  else
  {
    (void) dot1qQportsVIDGet(pduInfo->intIfNum, L7_DOT1Q_DEFAULT_VID_INDEX, &vlanId);
  }

  if ( (protocolType == L7_ETYPE_EAPOL) &&
       ( (memcmp(data,L7_EAPOL_PDU_MAC_ADDR.addr,L7_MAC_ADDR_LEN) == 0) ||
         (memcmp(data,baseMac,L7_MAC_ADDR_LEN) == 0) ) )
  {
    L7_enetHeader_t *enetHdr;

    /* If this is a reflected frame don't process it any further */
    enetHdr = (L7_enetHeader_t *)data;

    if (memcmp(enetHdr->src.addr, baseMac, L7_MAC_ADDR_LEN) == 0)
      return L7_FAILURE;

    if (nimGetIntfMaxFrameSize(pduInfo->intIfNum, &maxFrameSize) != L7_SUCCESS)
      return L7_FAILURE;

    if (dot1xIssueCmd(dot1xPduReceived, pduInfo->intIfNum, &bufHandle) != L7_SUCCESS)
    {
      if ( dot1xPortPaeCapabilitiesGet(pduInfo->intIfNum,&capabilities) == L7_FAILURE)
      {
        SYSAPI_NET_MBUF_FREE(bufHandle);
      }
      else if (capabilities == L7_DOT1X_PAE_PORT_AUTH_CAPABLE)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1X_COMPONENT_ID,
                "DOT1X: Problem occured while placing the PDU in the dot1x Queue\r\n");
        /* increment invalid receive counter */
        dot1xPortStats[pduInfo->intIfNum].authInvalidEapolFramesRx++;
        /* free the buffer */
        SYSAPI_NET_MBUF_FREE(bufHandle);
      }
    }

    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/**************************************************************************
* @purpose   Remove any tagging that may be there between the src addr and the lengthType field
*
* @param     bufHandle  @b{(input)} handle to the network buffer the PDU is stored in
*
* @returns   void
*
* @comments  assumes that PDU is meant for dot1x
*
* @end
*************************************************************************/
void dot1xTxRxHeaderTagRemove(L7_netBufHandle bufHandle)
{
  L7_uchar8 *data;
  L7_uint32 dataLength;
  L7_uchar8 *offset;
  L7_uint32 diff;

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, dataLength);
  /*assuming  sysNetPost8021qVlanTagOffsetGet is Endian safe*/
  /*removing VLAN tag if present - assuming E-RIF removed in DTLPDU receive if it was present*/
  offset = sysNetPost8021qVlanTagOffsetGet((L7_char8 *)data);
  if (data != offset)
  {
    diff = (L7_uint32)(offset - data);
    memcpy(temp, offset, dataLength - diff);
    memcpy(data + L7_ENET_HDR_SIZE, temp, dataLength - diff);
    dataLength = dataLength - L7_8021Q_ENCAPS_HDR_SIZE;
    SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, dataLength);
  }

  return;
}

/**************************************************************************
* @purpose   Transmit a EAPOL EAP Req/Id to Supplicant
*
* @param     intIfNum @b{(input)} internal interface number
*
* @returns   void
*
* @comments
*
* @end
*************************************************************************/
void dot1xTxReqId(L7_uint32 intIfNum, dot1xPortType_t portType)
{
  L7_netBufHandle bufHandle;
  L7_uchar8 *data;
  L7_enetHeader_t *enetHdr;
  L7_enet_encaps_t *encap;
  L7_uchar8 baseMac[L7_MAC_ADDR_LEN];
  L7_eapolPacket_t *eapolPkt;
  L7_eapPacket_t *eapPkt;
  L7_eapRrPacket_t *eapRrPkt;
  L7_uint32 length;
  dot1xLogicalPortInfo_t *logicalPortInfo;

  SYSAPI_NET_MBUF_GET(bufHandle);
  if (bufHandle == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
            "dot1xTxReqId: Out of System buffers."
            " 802.1X cannot process/transmit message due to lack of internal buffers");
    return;
  }

  if (portType == DOT1X_PHYSICAL_PORT)
  {
    DOT1X_EVENT_TRACE(DOT1X_TRACE_API_CALLS,intIfNum,"%s:Physical Port \n",
                        __FUNCTION__);
  }
  else
  {
    DOT1X_EVENT_TRACE(DOT1X_TRACE_API_CALLS,dot1xPhysPortGet(intIfNum),"%s for port - %d\n",
                      __FUNCTION__,intIfNum);

  }
  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);

  /* Set ethernet header */
  enetHdr = (L7_enetHeader_t *)(data);

  /* Get base MAC address (could be BIA or LAA) */
  if (simGetSystemIPMacType() == L7_SYSMAC_BIA)
    simGetSystemIPBurnedInMac(baseMac);
  else
    simGetSystemIPLocalAdminMac(baseMac);

  /* Set dest and source MAC in ethernet header */
  memcpy(enetHdr->dest.addr, L7_EAPOL_PDU_MAC_ADDR.addr, L7_MAC_ADDR_LEN);
  memcpy(enetHdr->src.addr, baseMac, L7_MAC_ADDR_LEN);

  /* Set ethernet type */
  encap = (L7_enet_encaps_t *)((L7_uchar8 *)enetHdr + L7_ENET_HDR_SIZE);
  encap->type = osapiHtons(L7_ETYPE_EAPOL);

  /* Set EAPOL header */
  eapolPkt = (L7_eapolPacket_t *)((L7_uchar8 *)encap + L7_ENET_ENCAPS_HDR_SIZE);
  eapolPkt->protocolVersion = L7_DOT1X_PAE_PORT_PROTOCOL_VERSION_1;
  eapolPkt->packetType = EAPOL_EAPPKT;
  eapolPkt->packetBodyLength = osapiHtons( (L7_ushort16)(sizeof(L7_eapPacket_t) + sizeof(L7_eapRrPacket_t)) );

  /* Set EAP header */
  eapPkt = (L7_eapPacket_t *)((L7_uchar8 *)eapolPkt + sizeof(L7_eapolPacket_t));
  eapPkt->code = EAP_REQUEST;
  if (portType == DOT1X_PHYSICAL_PORT)
  {
    if (dot1xPortInfo[intIfNum].incCurrentId == L7_TRUE)
    {
      dot1xPortInfo[intIfNum].currentId++;
      dot1xPortInfo[intIfNum].incCurrentId = L7_FALSE;
    }
    eapPkt->id = dot1xPortInfo[intIfNum].currentId;
  }
  else
  {
    logicalPortInfo = dot1xLogicalPortInfoGet(intIfNum);
    if(logicalPortInfo)
    {
      eapPkt->id = logicalPortInfo->currentIdL;
     }
  }



  eapPkt->length = osapiHtons( (L7_ushort16)(sizeof(L7_eapPacket_t) + sizeof(L7_eapRrPacket_t)) );

  /* Set EAP Request/Response header */
  eapRrPkt = (L7_eapRrPacket_t *)((L7_uchar8 *)eapPkt + sizeof(L7_eapPacket_t));
  eapRrPkt->type = EAP_RRIDENTITY;

  length = (L7_uint32)( L7_ENET_HDR_SIZE + L7_ENET_ENCAPS_HDR_SIZE + sizeof(L7_eapolPacket_t) +
           sizeof(L7_eapPacket_t) + sizeof(L7_eapRrPacket_t) );
  SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, length);

  dot1xFrameTransmit(intIfNum, bufHandle, portType);

  if (portType == DOT1X_PHYSICAL_PORT)
  {
    dot1xPortStats[intIfNum].authEapReqIdFramesTx++;
  }
  else
  {
    dot1xPortStats[dot1xPhysPortGet(intIfNum)].authEapReqIdFramesTx++;
  }
  return;
}

/**************************************************************************
* @purpose  Transmit a EAPOL EAP Request to Supplicant
*
* @param    logicalPortInfo  @b{(input))  Logical Port Info node
* @param    bufHandle @b{(input)} handle to the network buffer where frame is stored
*
* @returns  void
*
* @comments If the bufHandle is not L7_NULL, it means either 1) we got a request
*           from the authentication server which needs to be put into an EAPOL
*           frame and sent to the supplicant, or 2) we are retransmitting a
*           request that we sent previously.  In this case, we need to make a
*           copy of the buffer in case we need to re-transmit it.
* @comments If the bufHandle is L7_NULL, then we are doing local authentication
*           and need to generate a new MD5 challenge to be sent to the supplicant.
*           In this case, a copy of the buffer will be made in dot1xTxMd5Challenge
*           in case we need to re-transmit it.
*
* @end
*************************************************************************/
void dot1xTxReq(dot1xLogicalPortInfo_t *logicalPortInfo, L7_netBufHandle bufHandle)
{
  L7_uchar8 *dataStart, *suppDataStart;
  L7_uint32 length;
  L7_uint32 physPort = logicalPortInfo->physPort;

  DOT1X_EVENT_TRACE(DOT1X_TRACE_API_CALLS,physPort,"%s for port - %d\n",
                    __FUNCTION__,logicalPortInfo->logicalPortNumber);

  if (bufHandle == L7_NULL)
  {
    dot1xTxMd5Challenge(logicalPortInfo);
  }
  else
  {
    /* Make a copy of buffer for re-transmission */
    SYSAPI_NET_MBUF_GET(logicalPortInfo->suppBufHandle);
    if (logicalPortInfo->suppBufHandle != L7_NULL)
    {
      SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, dataStart);
      SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, length);
      SYSAPI_NET_MBUF_GET_DATASTART(logicalPortInfo->suppBufHandle, suppDataStart);
      memcpy(suppDataStart, dataStart, length);
      SYSAPI_NET_MBUF_SET_DATALENGTH(logicalPortInfo->suppBufHandle, length);

    }
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
              "dot1xTxReq: Out of System buffers."
              " 802.1X cannot process/transmit message due to lack of internal buffers");
    }


    dot1xFrameTransmit(logicalPortInfo->logicalPortNumber, bufHandle,DOT1X_LOGICAL_PORT);
  }

  dot1xPortStats[physPort].authEapReqFramesTx++;

  return;
}

/**************************************************************************
* @purpose   Transmit a EAPOL EAP Success to Supplicant
*
* @param     intIfNum @b{(input)} internal interface number
*
* @returns   void
*
* @comments
*
* @end
*************************************************************************/
void dot1xTxCannedSuccess(L7_uint32 intIfNum, dot1xPortType_t portType)
{
  L7_netBufHandle bufHandle;
  L7_uchar8 *data;
  L7_enetHeader_t *enetHdr;
  L7_enet_encaps_t *encap;
  L7_uchar8 baseMac[L7_MAC_ADDR_LEN];
  L7_eapolPacket_t *eapolPkt;
  L7_eapPacket_t *eapPkt;
  L7_uint32 length, session;
  dot1xLogicalPortInfo_t *logicalPortInfo;

  DOT1X_EVENT_TRACE(DOT1X_TRACE_API_CALLS,dot1xPhysPortGet(intIfNum),
                    "%s: called for port -%d\n",__FUNCTION__,intIfNum);

  /* Don't send canned success on the probe (mirror dest) port */
  if (mirrorIsDestConfigured(intIfNum, &session) == L7_TRUE)
    return;

  SYSAPI_NET_MBUF_GET(bufHandle);
  if (bufHandle == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
            "dot1xTxReqId: Out of System buffers."
            " 802.1X cannot process/transmit message due to lack of internal buffers");
    return;
  }

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);

  /* Set ethernet header */
  enetHdr = (L7_enetHeader_t *)(data);

  /* Get base MAC address (could be BIA or LAA) */
  if (simGetSystemIPMacType() == L7_SYSMAC_BIA)
    simGetSystemIPBurnedInMac(baseMac);
  else
    simGetSystemIPLocalAdminMac(baseMac);

  /* Set dest and source MAC in ethernet header */
  memcpy(enetHdr->dest.addr, L7_EAPOL_PDU_MAC_ADDR.addr, L7_MAC_ADDR_LEN);
  memcpy(enetHdr->src.addr, baseMac, L7_MAC_ADDR_LEN);

  /* Set ethernet type */
  encap = (L7_enet_encaps_t *)((L7_uchar8 *)enetHdr + L7_ENET_HDR_SIZE);
  encap->type = osapiHtons(L7_ETYPE_EAPOL);

  /* Set EAPOL header */
  eapolPkt = (L7_eapolPacket_t *)((L7_uchar8 *)encap + L7_ENET_ENCAPS_HDR_SIZE);
  eapolPkt->protocolVersion = L7_DOT1X_PAE_PORT_PROTOCOL_VERSION_1;
  eapolPkt->packetType = EAPOL_EAPPKT;
  eapolPkt->packetBodyLength = osapiHtons( (L7_ushort16)(sizeof(L7_eapPacket_t)) );

  /* Set EAP header */
  eapPkt = (L7_eapPacket_t *)((L7_uchar8 *)eapolPkt + sizeof(L7_eapolPacket_t));
  eapPkt->code = EAP_SUCCESS;

  if (portType == DOT1X_PHYSICAL_PORT)
  {
        eapPkt->id = dot1xPortInfo[intIfNum].currentId;
  }
  else
  {
    logicalPortInfo = dot1xLogicalPortInfoGet(intIfNum);
    if(logicalPortInfo)
    {
          eapPkt->id = logicalPortInfo->currentIdL;
    }
  }

  eapPkt->length = osapiHtons( (L7_ushort16) (sizeof(L7_eapPacket_t)) );

  length = (L7_uint32)( L7_ENET_HDR_SIZE + L7_ENET_ENCAPS_HDR_SIZE +
           sizeof(L7_eapolPacket_t) + sizeof(L7_eapPacket_t) );
  SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, length);

   dot1xFrameTransmit(intIfNum, bufHandle, portType);

  return;
}

/**************************************************************************
* @purpose   Transmit a EAPOL EAP Failure to Supplicant
*
* @param     intIfNum @b{(input)} internal interface number
*
* @returns   void
*
* @comments
*
* @end
*************************************************************************/
void dot1xTxCannedFail(L7_uint32 intIfNum, dot1xPortType_t portType)
{
  L7_netBufHandle bufHandle;
  L7_uchar8 *data;
  L7_enetHeader_t *enetHdr;
  L7_enet_encaps_t *encap;
  L7_uchar8 baseMac[L7_MAC_ADDR_LEN];
  L7_eapolPacket_t *eapolPkt;
  L7_eapPacket_t *eapPkt;
  L7_uint32 length;
  dot1xLogicalPortInfo_t *logicalPortInfo;

  DOT1X_EVENT_TRACE(DOT1X_TRACE_API_CALLS,dot1xPhysPortGet(intIfNum),
                    "%s: called for port -%d\n",__FUNCTION__,intIfNum);

  SYSAPI_NET_MBUF_GET(bufHandle);
  if (bufHandle == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
            "dot1xTxCannedFail: Out of System buffers."
            " 802.1X cannot process/transmit message due to lack of internal buffers");
    return;
  }

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);

  /* Set ethernet header */
  enetHdr = (L7_enetHeader_t *)(data);

  /* Get base MAC address (could be BIA or LAA) */
  if (simGetSystemIPMacType() == L7_SYSMAC_BIA)
    simGetSystemIPBurnedInMac(baseMac);
  else
    simGetSystemIPLocalAdminMac(baseMac);

  /* Set dest and source MAC in ethernet header */
  memcpy(enetHdr->dest.addr, L7_EAPOL_PDU_MAC_ADDR.addr, L7_MAC_ADDR_LEN);
  memcpy(enetHdr->src.addr, baseMac, L7_MAC_ADDR_LEN);

  /* Set ethernet type */
  encap = (L7_enet_encaps_t *)((L7_uchar8 *)enetHdr + L7_ENET_HDR_SIZE);
  encap->type = osapiHtons(L7_ETYPE_EAPOL);

  /* Set EAPOL header */
  eapolPkt = (L7_eapolPacket_t *)((L7_uchar8 *)encap + L7_ENET_ENCAPS_HDR_SIZE);
  eapolPkt->protocolVersion = L7_DOT1X_PAE_PORT_PROTOCOL_VERSION_1;
  eapolPkt->packetType = EAPOL_EAPPKT;
  eapolPkt->packetBodyLength = osapiHtons( (L7_ushort16)(sizeof(L7_eapPacket_t)) );

  /* Set EAP header */
  eapPkt = (L7_eapPacket_t *)((L7_uchar8 *)eapolPkt + sizeof(L7_eapolPacket_t));
  eapPkt->code = EAP_FAILURE;

  if (portType == DOT1X_PHYSICAL_PORT)
  {
        eapPkt->id = dot1xPortInfo[intIfNum].currentId;
  }
  else
  {
    logicalPortInfo = dot1xLogicalPortInfoGet(intIfNum);
    if(logicalPortInfo)
    {
          eapPkt->id = logicalPortInfo->currentIdL;
    }
  }

  eapPkt->length = osapiHtons( (L7_ushort16) (sizeof(L7_eapPacket_t)) );

  length = (L7_uint32) (L7_ENET_HDR_SIZE + L7_ENET_ENCAPS_HDR_SIZE +
           sizeof(L7_eapolPacket_t) + sizeof(L7_eapPacket_t) );
  SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, length);

  dot1xFrameTransmit(intIfNum, bufHandle, portType);

  return;
}

/**************************************************************************
* @purpose   Transmit an EAP Request MD5 challenge to Supplicant
*
* @param     logicalPortInfo  @b{(input))  Logical Port Info node
*
* @returns   void
*
* @comments
*
* @end
*************************************************************************/
void dot1xTxMd5Challenge(dot1xLogicalPortInfo_t *logicalPortInfo)
{
  L7_netBufHandle bufHandle;
  L7_uchar8 *data, *suppDataStart, *challenge;
  L7_enetHeader_t *enetHdr;
  L7_enet_encaps_t *encap;
  L7_uchar8 baseMac[L7_MAC_ADDR_LEN];
  L7_eapolPacket_t *eapolPkt;
  L7_eapPacket_t *eapPkt;
  L7_eapRrPacket_t *eapRrPkt;
  L7_uint32 length;

  DOT1X_EVENT_TRACE(DOT1X_TRACE_API_CALLS,dot1xPhysPortGet(logicalPortInfo->logicalPortNumber),
                    "%s: called for port -%d\n",__FUNCTION__,logicalPortInfo->logicalPortNumber);

  SYSAPI_NET_MBUF_GET(bufHandle);
  if (bufHandle == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
            "dot1xTxMd5Challenge: Out of System buffers."
            " 802.1X cannot process/transmit message due to lack of internal buffers");
    return;
  }

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);

  /* Set ethernet header */
  enetHdr = (L7_enetHeader_t *)(data);

  /* Get base MAC address (could be BIA or LAA) */
  if (simGetSystemIPMacType() == L7_SYSMAC_BIA)
    simGetSystemIPBurnedInMac(baseMac);
  else
    simGetSystemIPLocalAdminMac(baseMac);

  /* Set dest and source MAC in ethernet header */
  memcpy(enetHdr->dest.addr, L7_EAPOL_PDU_MAC_ADDR.addr, L7_MAC_ADDR_LEN);
  memcpy(enetHdr->src.addr, baseMac, L7_MAC_ADDR_LEN);

  /* Set ethernet type */
  encap = (L7_enet_encaps_t *)((L7_uchar8 *)enetHdr + L7_ENET_HDR_SIZE);
  encap->type = osapiHtons(L7_ETYPE_EAPOL);

  /* Set EAPOL header */
  eapolPkt = (L7_eapolPacket_t *)((L7_uchar8 *)encap + L7_ENET_ENCAPS_HDR_SIZE);
  eapolPkt->protocolVersion = L7_DOT1X_PAE_PORT_PROTOCOL_VERSION_1;
  eapolPkt->packetType = EAPOL_EAPPKT;
  eapolPkt->packetBodyLength = osapiHtons(sizeof(L7_eapPacket_t) + sizeof(L7_eapRrPacket_t) +
                                          1 + DOT1X_CHALLENGE_LEN + L7_MAC_ADDR_LEN);

  /* Set EAP header */
  eapPkt = (L7_eapPacket_t *)((L7_uchar8 *)eapolPkt + sizeof(L7_eapolPacket_t));
  eapPkt->code = EAP_REQUEST;
  eapPkt->id = ++logicalPortInfo->currentIdL;
  eapPkt->length = osapiHtons(sizeof(L7_eapPacket_t) + sizeof(L7_eapRrPacket_t) +
                              1 + DOT1X_CHALLENGE_LEN + L7_MAC_ADDR_LEN);

  /* Set EAP Request/Response header */
  eapRrPkt = (L7_eapRrPacket_t *)((L7_uchar8 *)eapPkt + sizeof(L7_eapPacket_t));
  eapRrPkt->type = EAP_RRMD5;

  challenge = ((L7_uchar8 *)eapRrPkt) + sizeof(L7_eapRrPacket_t);
  *challenge = DOT1X_CHALLENGE_LEN;
  challenge++;
  dot1xLocalAuthChallengeGenerate(logicalPortInfo->dot1xChallenge, DOT1X_CHALLENGE_LEN);
  memcpy(challenge, logicalPortInfo->dot1xChallenge, DOT1X_CHALLENGE_LEN);
  challenge += DOT1X_CHALLENGE_LEN;
  memcpy(challenge, baseMac, L7_MAC_ADDR_LEN);

  length = L7_ENET_HDR_SIZE + L7_ENET_ENCAPS_HDR_SIZE + sizeof(L7_eapolPacket_t) +
           sizeof(L7_eapPacket_t) + sizeof(L7_eapRrPacket_t) +
           1 + DOT1X_CHALLENGE_LEN + L7_MAC_ADDR_LEN;
  SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, length);

  /* Make a copy of buffer for re-transmission */
  SYSAPI_NET_MBUF_GET(logicalPortInfo->suppBufHandle);
  if (logicalPortInfo->suppBufHandle != L7_NULL)
  {
    SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
    SYSAPI_NET_MBUF_GET_DATASTART(logicalPortInfo->suppBufHandle, suppDataStart);
    memcpy(suppDataStart, data, length); /* length calculated above */
    SYSAPI_NET_MBUF_SET_DATALENGTH(logicalPortInfo->suppBufHandle, length);
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
            "dot1xTxMd5Challenge: Out of System buffers."
            " 802.1X cannot process/transmit message due to lack of internal buffers");
  }

  dot1xFrameTransmit(logicalPortInfo->logicalPortNumber, bufHandle,DOT1X_LOGICAL_PORT);

  return;
}

/*********************************************************************
* @purpose  Transmit a frame
*
* @param    intIfNum   @b{(input)} outgoing internal interface number
* @param    bufHandle  @b{(input)} handle to the network buffer where frame is stored
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void dot1xFrameTransmit(L7_uint32 intIfNum, L7_netBufHandle bufHandle,dot1xPortType_t portType)
{
  L7_uchar8 *data,tmpMac[L7_ENET_MAC_ADDR_LEN];
  DTL_CMD_TX_INFO_t  dtlCmd;

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);

  bzero((L7_uchar8 *)&dtlCmd,sizeof(DTL_CMD_TX_INFO_t));
  if (portType == DOT1X_LOGICAL_PORT)
  {
    dot1xLogicalPortInfo_t *logicalPortInfo;
    logicalPortInfo = dot1xLogicalPortInfoGet(intIfNum);
    if(logicalPortInfo == L7_NULLPTR || logicalPortInfo->inUse == L7_FALSE)
    {
      SYSAPI_NET_MBUF_FREE(bufHandle);
      return;
    }
    memset(tmpMac,0,L7_ENET_MAC_ADDR_LEN);
    if(memcmp(logicalPortInfo->suppMacAddr.addr,tmpMac,L7_ENET_MAC_ADDR_LEN)!=0)
      memcpy(data, logicalPortInfo->suppMacAddr.addr, L7_ENET_MAC_ADDR_LEN);
    dtlCmd.intfNum = logicalPortInfo->physPort;
    dtlCmd.cmdType.L2.domainId = logicalPortInfo->vlanId;

  }
  else
  {
    dtlCmd.intfNum = intIfNum;
    (void) dot1qQportsVIDGet(intIfNum, L7_DOT1Q_DEFAULT_VID_INDEX,
                      &(dtlCmd.cmdType.L2.domainId));
  }
  dtlCmd.priority = 1;
  dtlCmd.typeToSend = DTL_L2RAW_UNICAST;


  dot1xDebugPacketTxTrace(dtlCmd.intfNum ,bufHandle);

  if (dtlPduTransmit (bufHandle, DTL_CMD_TX_L2, &dtlCmd) == L7_SUCCESS)
    dot1xPortStats[dtlCmd.intfNum].authEapolFramesTx++;
  else
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1X_COMPONENT_ID,
            "dot1xFrameTransmit: dtlPduTransmit failed\n");

  return;
}

/**************************************************************************
* @purpose   Convert the packet from network byte order (Big endian) to
*            host byte order
*
* @param     bufHandle  @b{(input)} the handle to the dot1x PDU
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  Applies to PAE (dot1x) PDUs only
*
* @end
*************************************************************************/
L7_RC_t dot1xTxRxHostConvert(L7_netBufHandle bufHandle)
{
  L7_uchar8 *data;
  L7_enetHeader_t *enetHdr;
  L7_enet_encaps_t *encap;
  L7_eapolPacket_t *eapolPkt;
  L7_eapPacket_t *eapPkt;
  L7_eapolKeyPacket_t *eapolKeyPkt;
  L7_ushort16 temp16;

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  enetHdr = (L7_enetHeader_t *)data;

  encap = (L7_enet_encaps_t *)((L7_uchar8 *)enetHdr + L7_ENET_HDR_SIZE);
  temp16 = osapiNtohs(encap->type);
  memcpy(&encap->type, &temp16, sizeof(L7_ushort16));

  if (encap->type != L7_ETYPE_EAPOL)
  {
      printf("\n Encap type :%02X02X",encap->type);
      return L7_FAILURE;
  }

  eapolPkt = (L7_eapolPacket_t *)((L7_uchar8 *)encap + L7_ENET_ENCAPS_HDR_SIZE);
  temp16 = osapiNtohs(eapolPkt->packetBodyLength);
  memcpy(&eapolPkt->packetBodyLength, &temp16, sizeof(L7_ushort16));

  if (eapolPkt->packetType == EAPOL_EAPPKT)
  {
    eapPkt = (L7_eapPacket_t *)((L7_uchar8 *)eapolPkt + sizeof(L7_eapolPacket_t));
    temp16 = osapiNtohs(eapPkt->length);
    memcpy(&eapPkt->length, &temp16, sizeof(L7_ushort16));
  }
  else if (eapolPkt->packetType == EAPOL_KEY)
  {
    eapolKeyPkt = (L7_eapolKeyPacket_t *)((L7_uchar8 *)eapolPkt + sizeof(L7_eapolPacket_t));
    temp16 = osapiNtohs(eapolKeyPkt->keyLength);
    memcpy(&eapolKeyPkt->keyLength, &temp16, sizeof(L7_ushort16));
  }

  return L7_SUCCESS;
}

/**************************************************************************
* @purpose   Send Response frame to Server
*
* @param     lIntIfNum   @b{(input)} Logical internal interface number
* @param     bufHandle  @b{(input)} the handle to the dot1x PDU
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*************************************************************************/
L7_RC_t dot1xSendRespToServer(L7_uint32 lIntIfNum, L7_netBufHandle bufHandle)
{
  L7_uchar8 *data;
  L7_eapPacket_t *eapPkt = L7_NULLPTR;

  DOT1X_EVENT_TRACE(DOT1X_TRACE_API_CALLS,dot1xPhysPortGet(lIntIfNum),
                    "%s: called for port -%d\n",__FUNCTION__,lIntIfNum);

  if (bufHandle != L7_NULL)
  {
    SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);

    eapPkt = (L7_eapPacket_t *)(data + L7_ENET_HDR_SIZE + L7_ENET_ENCAPS_HDR_SIZE + sizeof(L7_eapolPacket_t));
  }


  if (dot1xRadiusAccessRequestSend(lIntIfNum, (L7_uchar8 *)eapPkt) != L7_SUCCESS)
  {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
            "dot1xSendRespToServer: dot1xRadiusAccessRequestSend failed\n"
            " Failed sending message to RADIUS server");
        DOT1X_EVENT_TRACE(DOT1X_TRACE_API_CALLS,dot1xPhysPortGet(lIntIfNum),
                    "%s: dot1xRadiusAccessRequestSend failed for port -%d\n",__FUNCTION__,lIntIfNum);
  }

  if (bufHandle != L7_NULL)
  {
    SYSAPI_NET_MBUF_FREE(bufHandle);
  }

  return L7_SUCCESS;
}

/**************************************************************************
* @purpose   Send an EAP Response/Notification to dot1x client
*
* @param     intIfNum       @b{(input)} Logical interface number
* @param     *radiusPayload @b{(input)} pointer to radius payload
* @param     payloadLen     @b{(input)} payload length
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*************************************************************************/

L7_RC_t dot1xRadiusSendNotification(L7_uint32 intIfNum,
                                    L7_uchar8 *radiusPayload,
                                    L7_uint32 payloadLen)
{
   L7_uint32 length = 0;
  L7_BOOL done = L7_FALSE;
  L7_enetHeader_t *enetHdr;
  L7_enet_encaps_t *encap;
  L7_eapolPacket_t *eapolPkt;
  L7_eapPacket_t   *eapBuf;
  L7_BOOL replyMess = L7_FALSE;
  L7_eapPacket_t *eapPkt;
  L7_netBufHandle bufHandle_new;
  L7_uchar8 *data;
  L7_uchar8 baseMac[L7_ENET_MAC_ADDR_LEN];
  L7_radiusAttr_t *radiusAttr;
  L7_eapRrPacket_t *eapRrPkt;
  L7_uint32 templength = 0;
  dot1xLogicalPortInfo_t *logicalPortInfo;
  logicalPortInfo = dot1xLogicalPortInfoGet(intIfNum);

  DOT1X_EVENT_TRACE(DOT1X_TRACE_API_CALLS,dot1xPhysPortGet(intIfNum),
                    "%s: called for port -%d\n",__FUNCTION__,intIfNum);

  if(logicalPortInfo == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  SYSAPI_NET_MBUF_GET(bufHandle_new);
  if (bufHandle_new == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
            "dot1xTxReqId: Out of System buffers."
            " 802.1X cannot process/transmit message due to lack of internal buffers");
    return L7_FAILURE;
  }
  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle_new, data);

  /* Set ethernet header */
  enetHdr = (L7_enetHeader_t *)(data);

  /* Get base MAC address (could be BIA or LAA) */
  if (simGetSystemIPMacType() == L7_SYSMAC_BIA)
    simGetSystemIPBurnedInMac(baseMac);
  else
    simGetSystemIPLocalAdminMac(baseMac);
   /* Set dest and source MAC in ethernet header */

   memcpy(enetHdr->dest.addr, L7_EAPOL_PDU_MAC_ADDR.addr, L7_MAC_ADDR_LEN);
   memcpy(enetHdr->src.addr, baseMac, L7_MAC_ADDR_LEN);

  /* Set ethernet type */
  /* Set ethernet type */
  encap = (L7_enet_encaps_t *)((L7_uchar8 *)enetHdr + L7_ENET_HDR_SIZE);
  encap->type = osapiHtons(L7_ETYPE_EAPOL);

  /* Set EAPOL header */
  eapolPkt = (L7_eapolPacket_t *)((L7_uchar8 *)encap +  L7_ENET_ENCAPS_HDR_SIZE);
  eapolPkt->protocolVersion =   L7_DOT1X_PAE_PORT_PROTOCOL_VERSION_1;
  eapolPkt->packetType = EAPOL_EAPPKT;

  eapPkt = (L7_eapPacket_t *)((L7_uchar8 *)eapolPkt + sizeof(L7_eapolPacket_t));
  eapBuf = (L7_eapPacket_t *)eapPkt;
  radiusAttr = (L7_radiusAttr_t *)radiusPayload;
  eapBuf->code = EAP_REQUEST;
  eapBuf->id = logicalPortInfo->currentIdL;
  while (done == L7_FALSE)
  {
    /* Copy the data from the radius server from attribute 18 construct the data
    for the EAP response and send this to supplicant */
    if (radiusAttr->type == RADIUS_ATTR_TYPE_REPLY_MESSAGE)
    {
      /* Set EAP header */
      memcpy((L7_uchar8 *)eapBuf + sizeof(L7_eapPacket_t)+ sizeof(L7_eapRrPacket_t)+templength,
                                 (L7_uchar8 *)radiusAttr + sizeof(L7_radiusAttr_t),
                                     radiusAttr->length - sizeof(L7_radiusAttr_t));
      templength = templength + radiusAttr->length - sizeof(L7_radiusAttr_t);
      eapBuf->length = sizeof(L7_uchar8) *2 + sizeof(L7_ushort16) +
                       templength +  sizeof(L7_eapRrPacket_t);
      length = eapBuf->length;
      /* Set EAP Request/Response header */
      eapRrPkt = (L7_eapRrPacket_t *)((L7_uchar8 *)eapPkt + sizeof(L7_eapPacket_t));
      eapRrPkt->type = EAP_RRNOTIF;
      replyMess = L7_TRUE;
    } /* end if */

    payloadLen -= radiusAttr->length;

    if (payloadLen <= (L7_uint32)sizeof(L7_radiusAttr_t))
    {
      done = L7_TRUE;
    }
    else
     {
      radiusAttr = (L7_radiusAttr_t *)((L7_uchar8 *)radiusAttr + radiusAttr->length);
    }

    if (radiusAttr->length <= 0)
      done = L7_TRUE;

  } /* end while */
  if(replyMess == L7_FALSE)
  {
    SYSAPI_NET_MBUF_FREE(bufHandle_new);
    return L7_FAILURE;
  }
  eapolPkt->packetBodyLength = length;
  length = (L7_uint32) (L7_ENET_HDR_SIZE +     L7_ENET_ENCAPS_HDR_SIZE +
                sizeof(L7_eapolPacket_t) + eapolPkt->packetBodyLength );
  SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle_new, length);
  dot1xFrameTransmit(intIfNum, bufHandle_new,DOT1X_LOGICAL_PORT);
  return L7_SUCCESS;

}

/**************************************************************************
* @purpose   Check whether to transmit a EAPOL EAP Req/Id to Supplicant
*
* @param     intIfNum @b{(input)} internal interface number
*
* @returns   L7_SUCCESS if ReqId should be sent, else L7_FAILURE
*
* @comments
*
* @end
*************************************************************************/
L7_RC_t dot1xTxReqIdCheck(L7_uint32 intIfNum)
{
  if (dot1xPortInfo[intIfNum].portControlMode != L7_DOT1X_PORT_AUTO &&
      dot1xPortInfo[intIfNum].numUsers >= dot1xPortInfo[intIfNum].maxUsers)
    return L7_FAILURE;

  if (dot1xPortInfo[intIfNum].incCurrentId == L7_TRUE)
    dot1xPortInfo[intIfNum].currentId++;

  dot1xPortInfo[intIfNum].incCurrentId = L7_FALSE;

  /*dot1xPortInfo[intIfNum].txWhenPort = dot1xPortInfo[intIfNum].txPeriod;*/

  return L7_SUCCESS;
}

/* Supplicant RX/TX functions */

/**************************************************************************
* @purpose   Transmit a EAPOL Start message to the Authenticator
*
* @param     intIfNum @b{(input)} internal interface number
*
* @returns   void
*
* @comments
*
* @end
*************************************************************************/
void dot1xTxStart(L7_uint32 intIfNum)
{
  L7_netBufHandle bufHandle;
  L7_uchar8 *data;
  L7_enetHeader_t *enetHdr;
  L7_enet_encaps_t *encap;
  L7_uchar8 baseMac[L7_MAC_ADDR_LEN];
  L7_eapolPacket_t *eapolPkt;
  L7_uint32 length;


  SYSAPI_NET_MBUF_GET(bufHandle);
  if (bufHandle == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1X_COMPONENT_ID,
            "dot1xTxStart: Out of System buffers\n");
    return;
  }

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);

  /* Set ethernet header */
  enetHdr = (L7_enetHeader_t *)(data);

  /* Get base MAC address (could be BIA or LAA) */
  if (simGetSystemIPMacType() == L7_SYSMAC_BIA)
    simGetSystemIPBurnedInMac(baseMac);
  else
    simGetSystemIPLocalAdminMac(baseMac);

  /* Set dest and source MAC in ethernet header */
  memcpy(enetHdr->dest.addr, L7_EAPOL_PDU_MAC_ADDR.addr, L7_MAC_ADDR_LEN);
  memcpy(enetHdr->src.addr, baseMac, L7_MAC_ADDR_LEN);

  /* Set ethernet type */
  encap = (L7_enet_encaps_t *)((L7_uchar8 *)enetHdr + L7_ENET_HDR_SIZE);
  encap->type = osapiHtons(L7_ETYPE_EAPOL);

  /* Set EAPOL header */
  eapolPkt = (L7_eapolPacket_t *)((L7_uchar8 *)encap + L7_ENET_ENCAPS_HDR_SIZE);
  eapolPkt->protocolVersion = L7_DOT1X_PAE_PORT_PROTOCOL_VERSION_1;
  eapolPkt->packetType = EAPOL_START;
  eapolPkt->packetBodyLength = osapiHtons(0);

  length = (L7_uint32)( L7_ENET_HDR_SIZE + L7_ENET_ENCAPS_HDR_SIZE +
           sizeof(L7_eapolPacket_t));
  SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, length);
  dot1xPortStats[intIfNum].suppEapolFramesTx++;
  dot1xPortStats[intIfNum].suppEapolStartFramesTx++;
  dot1xFrameTransmit(intIfNum, bufHandle, DOT1X_PHYSICAL_PORT);
  return;
}

/**************************************************************************
* @purpose   Transmit a EAPOL EAP Response to Authenticator
*
* @param     intIfNum @b{(input)} internal interface number
*
* @returns   void
*
* @comments
*
* @end
*************************************************************************/
void dot1xTxSuppRsp(L7_uint32 intIfNum)
{
  L7_netBufHandle bufHandle;
  L7_uchar8 *data;
  L7_enetHeader_t *enetHdr;
  L7_enet_encaps_t *encap;
  L7_uchar8 baseMac[L7_MAC_ADDR_LEN];
  L7_eapolPacket_t *eapolPkt;
  L7_eapPacket_t *eapPkt;
  L7_eapRrPacket_t *eapRrPkt;
  L7_uint32 length=0;
  L7_uchar8 *userName;
  L7_uchar8  answer[DOT1X_MD5_LEN+1], *responseData;
  L7_uint32  responseDataLen;
  dot1xPortCfg_t *pCfg;
  L7_uint32 index;
  L7_uchar8 userPw[L7_PASSWORD_SIZE];
  L7_uchar8 defaultUser[DOT1X_MD5_LEN+2];
  L7_BOOL usrExists = L7_FALSE;
  L7_BOOL usrIndex = 0;

  SYSAPI_NET_MBUF_GET(bufHandle);
  if (bufHandle == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1X_COMPONENT_ID,
            "dot1xTxReqId: Out of System buffers\n");
    return;
  }

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);

  /* Set ethernet header */
  enetHdr = (L7_enetHeader_t *)(data);

  /* Get base MAC address (could be BIA or LAA) */
  if (simGetSystemIPMacType() == L7_SYSMAC_BIA)
    simGetSystemIPBurnedInMac(baseMac);
  else
    simGetSystemIPLocalAdminMac(baseMac);

  /* Set dest and source MAC in ethernet header */
  memcpy(enetHdr->dest.addr, L7_EAPOL_PDU_MAC_ADDR.addr, L7_MAC_ADDR_LEN);
  memcpy(enetHdr->src.addr, baseMac, L7_MAC_ADDR_LEN);

  /* Set ethernet type */
  encap = (L7_enet_encaps_t *)((L7_uchar8 *)enetHdr + L7_ENET_HDR_SIZE);
  encap->type = osapiHtons(L7_ETYPE_EAPOL);

  /* Set EAPOL header */
  eapolPkt = (L7_eapolPacket_t *)((L7_uchar8 *)encap + L7_ENET_ENCAPS_HDR_SIZE);
  eapolPkt->protocolVersion = L7_DOT1X_PAE_PORT_PROTOCOL_VERSION_1;
  eapolPkt->packetType = EAPOL_EAPPKT;
  eapolPkt->packetBodyLength = osapiHtons( (L7_ushort16)(sizeof(L7_eapPacket_t) + sizeof(L7_eapRrPacket_t)) );

  /* Set EAP header */
  eapPkt = (L7_eapPacket_t *)((L7_uchar8 *)eapolPkt + sizeof(L7_eapolPacket_t));
  eapPkt->code = EAP_RESPONSE;
  eapPkt->id = dot1xSupplicantPortInfo[intIfNum].currentId;
  /* Set EAP Request/Response header */
  eapRrPkt = (L7_eapRrPacket_t *)((L7_uchar8 *)eapPkt + sizeof(L7_eapPacket_t));

  if (dot1xIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    return ;
  }
  /* Verify whether the user exists or not */

  if (pCfg->supplicantPortCfg.dot1xSupplicantUserNameLen > 0)
  {
    if (userMgrLoginIndexGet((L7_char8 *)pCfg->supplicantPortCfg.dot1xSupplicantUserName,
                           &usrIndex) == L7_SUCCESS)
    {
      usrExists = L7_TRUE;
    }

  }

  /* Send Identity response to the AUthenticator */
  if (dot1xSupplicantPortInfo[intIfNum].requestType == EAP_RRIDENTITY)
  {
    eapRrPkt->type = EAP_RRIDENTITY;
    userName = ((L7_uchar8 *)eapRrPkt) + sizeof(L7_eapRrPacket_t);

    if ( (pCfg->supplicantPortCfg.dot1xSupplicantUserNameLen > 0)
         &&
         (usrExists == L7_TRUE)
       )
    {

     eapolPkt->packetBodyLength = osapiHtons( (L7_ushort16)(sizeof(L7_eapPacket_t) + sizeof(L7_eapRrPacket_t)+
                                  pCfg->supplicantPortCfg.dot1xSupplicantUserNameLen) );
      eapPkt->length = eapolPkt->packetBodyLength;
      memset (userName,'\0', pCfg->supplicantPortCfg.dot1xSupplicantUserNameLen+1);
      memcpy (userName,
              pCfg->supplicantPortCfg.dot1xSupplicantUserName,
              pCfg->supplicantPortCfg.dot1xSupplicantUserNameLen);

      length = (L7_uint32)( L7_ENET_HDR_SIZE + L7_ENET_ENCAPS_HDR_SIZE + sizeof(L7_eapolPacket_t) +
          sizeof(L7_eapPacket_t) + sizeof(L7_eapRrPacket_t) + pCfg->supplicantPortCfg.dot1xSupplicantUserNameLen);
    }
    else
    {
       osapiSnprintf(defaultUser, sizeof(defaultUser), "%02X:%02X:%02X:%02X:%02X:%02X",
                  baseMac[0],baseMac[1],baseMac[2],baseMac[3],baseMac[4],baseMac[5]);

      eapolPkt->packetBodyLength = osapiHtons( (L7_ushort16)(sizeof(L7_eapPacket_t) +
                                   sizeof(L7_eapRrPacket_t)+sizeof(defaultUser)) );
      eapPkt->length =  eapolPkt->packetBodyLength ;

      memcpy (userName, defaultUser, sizeof(defaultUser));
      length = (L7_uint32)( L7_ENET_HDR_SIZE + L7_ENET_ENCAPS_HDR_SIZE + sizeof(L7_eapolPacket_t) +
           sizeof(L7_eapPacket_t) + sizeof(L7_eapRrPacket_t) +sizeof(defaultUser));
    }
    dot1xPortStats[intIfNum].suppEapRespIdFramesTx++;
  }
  /* Send challange response to the AUthenticator */
  else if (dot1xSupplicantPortInfo[intIfNum].requestType == EAP_RRMD5)
  {
    eapolPkt->packetBodyLength = osapiHtons( (L7_ushort16)(sizeof(L7_eapPacket_t) +
                                  sizeof(L7_eapRrPacket_t)+DOT1X_MD5_LEN+1));
    eapPkt->length =  eapolPkt->packetBodyLength ;
    eapRrPkt->type = EAP_RRMD5;
    userName = ((L7_uchar8 *)eapRrPkt) + sizeof(L7_eapRrPacket_t);

    if ( (pCfg->supplicantPortCfg.dot1xSupplicantUserNameLen > 0) &&
         (usrExists == L7_TRUE)
       )
    {
      if (userMgrLoginIndexGet(pCfg->supplicantPortCfg.dot1xSupplicantUserName,&index) == L7_SUCCESS)
      {
        bzero(userPw, L7_PASSWORD_SIZE);
        if (userMgrLoginUserPasswordClearTextGet(index,
                                  (L7_char8 *)userPw) != L7_SUCCESS)
        {
          return;
        }
        responseDataLen = 1 + strlen((L7_char8 *)userPw) + dot1xSupplicantPortInfo[intIfNum].challangeLen;
      }
      else
      {
        return;
      }
    }
    else
    {
      /* Need to define default value for this */
      responseDataLen = 1 + strlen(FD_DOT1X_PASS_TEXT) + dot1xSupplicantPortInfo[intIfNum].challangeLen;
      eapolPkt->packetBodyLength = osapiHtons( (L7_ushort16)(sizeof(L7_eapPacket_t) +
                                   sizeof(L7_eapRrPacket_t)+DOT1X_MD5_LEN+1));
      eapPkt->length =  eapolPkt->packetBodyLength ;
    }
    responseData = (L7_uchar8 *)osapiMalloc(L7_DOT1X_COMPONENT_ID, responseDataLen);
    if (responseData == L7_NULLPTR)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1X_COMPONENT_ID,
              "dot1xLocalAuthMd5ResponseValidate: couldn't allocate responseData\n");
      return;
    }

    bzero(responseData, responseDataLen);
    responseData[0] = dot1xSupplicantPortInfo[intIfNum].currentId;
    if ( (pCfg->supplicantPortCfg.dot1xSupplicantUserNameLen > 0) &&
         (usrExists == L7_TRUE)
       )
    {
      memcpy(&responseData[1],userPw, strlen((L7_char8 *)userPw));
      memcpy(&responseData[1+strlen((L7_char8 *)userPw)],dot1xSupplicantPortInfo[intIfNum].dot1xChallange,
              dot1xSupplicantPortInfo[intIfNum].challangeLen);
    }
    else
    {
      memcpy(&responseData[1], FD_DOT1X_PASS_TEXT, strlen(FD_DOT1X_PASS_TEXT));
      memcpy(&responseData[1+strlen(FD_DOT1X_PASS_TEXT)], dot1xSupplicantPortInfo[intIfNum].dot1xChallange
                           ,dot1xSupplicantPortInfo[intIfNum].challangeLen);
    }
    *answer = DOT1X_MD5_LEN;
    dot1xLocalMd5Calc(responseData, responseDataLen, answer+1);
    osapiFree(L7_DOT1X_COMPONENT_ID, responseData);
    memcpy (userName, answer,DOT1X_MD5_LEN+1);
    length = (L7_uint32)( L7_ENET_HDR_SIZE + L7_ENET_ENCAPS_HDR_SIZE + sizeof(L7_eapolPacket_t) +
           sizeof(L7_eapPacket_t) + sizeof(L7_eapRrPacket_t) +DOT1X_MD5_LEN+1);
    dot1xPortStats[intIfNum].suppEapResponseFramesTx++;
  }
  else if (dot1xSupplicantPortInfo[intIfNum].requestType == EAP_RRNAK)
  {
    eapPkt->length = osapiHtons( (L7_ushort16)(sizeof(L7_eapPacket_t) + sizeof(L7_eapRrPacket_t))+1);
    eapRrPkt->type = EAP_RRNAK;
    responseData = ((L7_uchar8 *)eapRrPkt) + sizeof(L7_eapRrPacket_t)+1;
    *responseData = 0;

    length = (L7_uint32)( L7_ENET_HDR_SIZE + L7_ENET_ENCAPS_HDR_SIZE + sizeof(L7_eapolPacket_t) +
           sizeof(L7_eapPacket_t) + sizeof(L7_eapRrPacket_t) +1);
    dot1xPortStats[intIfNum].suppEapResponseFramesTx++;

  }

  SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, length);
  dot1xPortStats[intIfNum].suppEapolFramesTx++;
  dot1xFrameTransmit(intIfNum, bufHandle,DOT1X_PHYSICAL_PORT);
  return;
}


