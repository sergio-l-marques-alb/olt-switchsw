/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
* @filename  isdp_txrx.c
*
* @purpose   ISDP Transmit and Receive PDUs
*
* @component isdp
*
* @comments
*
* @create    12/11/2007
*
* @author    dgaryachy
*
* @end
*
**********************************************************************/

#define L7_MAC_ISDP_PDU

#include "isdp_include.h"
#include "buff_api.h"
#include "isdp_api.h"
#include "l7_ip_api.h"
#include "rlim_api.h"
#include "simapi.h"
#include "voice_vlan_api.h"

extern isdpCfg_t         *isdpCfg;
extern osapiTimerDescr_t *isdpBaseTimer;

static const L7_uchar8 isdpOrganizationCode[] = {0x00, 0x00, 0x0c};

/*********************************************************************
* @purpose  Transmit a PDU (ISDP)
*
*
* @param    paddedBufHandle  @b{(input)} handle to the packet buffer
* @param    intIfNum      @b{(input)} interface through which to transmit
*
* @returns  L7_SUCCESS  on a successful tranmission
* @returns  L7_FAILURE  for any error
*
* @comments This routine just transmits the packet.
*           Appropriate counters are incremented.
*
* @end
*********************************************************************/
L7_RC_t isdpPduTransmit(L7_netBufHandle paddedBufHandle,
                        L7_uint32 intIfNum)
{
  L7_netBufHandle   bufHandle = L7_NULL;
  L7_RC_t           rc        = L7_SUCCESS;
  DTL_CMD_TX_INFO_t dtlCmdInfo;

  bufHandle =  isdpTxRxNetworkConvert(paddedBufHandle, intIfNum);
  if (bufHandle == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "isdpPduTransmit(): could not get a buffer\n");
    return L7_FAILURE;
  }

  /* send it back to DTL for transmit
   * assuming FCS is redone as a lower layer
   */
  bzero((char *)&dtlCmdInfo,(L7_int32)sizeof(DTL_CMD_TX_INFO_t));
  dtlCmdInfo.intfNum = intIfNum;
  dtlCmdInfo.priority = 1;
  dtlCmdInfo.typeToSend = DTL_L2RAW_UNICAST;

  rc = dtlPduTransmit( bufHandle, DTL_CMD_TX_L2, &dtlCmdInfo);
  return rc;
}

/**************************************************************************
* @purpose   Remove any tagging that may be there between the src addr and the lengthType field
*
* @param     bufHandle    @b{(input)} handle to the network buffer the PDU is stored in
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  assumes that PDU is meant for isdp
*
* @end
*************************************************************************/
L7_RC_t isdpTxRxHeaderTagRemove(L7_netBufHandle bufHandle)
{
  L7_uchar8 *data;
  L7_uint32 dataLength;
  L7_uchar8 *offset;
  L7_uint32 diff;
  L7_uchar8 *temp;

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, dataLength);
  /* assuming  sysNetPost8021qVlanTagOffsetGet is Endian safe
   * removing VLAN tag if present - assuming E-RIF removed in
   * DTLPDU receive if it was present
   */
  offset = sysNetPost8021qVlanTagOffsetGet((L7_char8 *)data);
  if (data != offset)
  {
    diff = (L7_uint32)(offset - data);
    /*Dynamically allocate the required memory insted of constant ISDP_MAX_PDU_SIZE
      Because some times CDP packets are more than ISDP_MAX_PDU_SIZE */
    temp = (L7_uchar8*) osapiMalloc (L7_ISDP_COMPONENT_ID, (dataLength - diff));
    memset(temp, 0, (dataLength - diff));
    memcpy(temp, offset, dataLength - diff);
    memcpy(data + L7_ENET_HDR_SIZE, temp, dataLength - diff);
    dataLength = dataLength - L7_8021Q_ENCAPS_HDR_SIZE;
    SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, dataLength);
    /*Free the allocated memory after use*/
    osapiFree(L7_ISDP_COMPONENT_ID, temp);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This routine receives PDU (ISDP) and sends
*      it to the isdp msg Q and task
*
* @param    bufHandle    @b{(input)} handle to the network buffer the pdu is stored in
* @param    pduInfo     @b{(input)} pointer to sysnet structure which contains the internal
*                    interface number that the Pdu was received on
*
* @returns  L7_SUCCESS  on a successful operation
* @returns  L7_FAILURE  for any error
*
* @comments This routine is registered with sysnet for Des MAC address of
*      01-00-0c-cc-cc-cc at isdp init time.
*
* @end
*********************************************************************/
L7_RC_t isdpPduReceive(L7_netBufHandle bufHandle,
                       sysnet_pdu_info_t *pduInfo)
{
  L7_uchar8         *data     = L7_NULLPTR;
  isdpIntfCfgData_t *pCfg     = L7_NULLPTR;
  L7_802_encaps_t   *encap    = L7_NULLPTR;
  L7_ushort16       ethertype = 0;

  if(isdpTxRxHeaderTagRemove(bufHandle) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "Couldn't remove header %d\n", pduInfo->intIfNum);
    return L7_FAILURE;
  }

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  encap = (L7_802_encaps_t *)(data + L7_ENET_HDR_SIZE);

  ethertype = osapiNtohs(encap->type);

  if (memcmp(data, L7_ENET_ISDP_MULTICAST_MAC_ADDR.addr, L7_MAC_ADDR_LEN) != 0 ||
      ethertype != L7_ETYPE_ISDP)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(pduInfo->intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "Foreign pdu on interface %s\n", ifName);
    return L7_FAILURE;
  }

  if ( (isdpCfg->cfg.globalCfgData.mode != L7_ENABLE) ||
       (isdpIsValidIntf(pduInfo->intIfNum) != L7_TRUE) ||
       (isdpIntfIsConfigurable(pduInfo->intIfNum, &pCfg) != L7_TRUE) ||
       (pCfg->mode != L7_ENABLE) )
  {
    isdpDebugTrace(ISDP_DBG_FLAG_PduGroup,
                   "%s: LINE %d: interface %d isn't configured for rx \n",
                   __FUNCTION__, __LINE__, pduInfo->intIfNum);

    return L7_FAILURE;
  }

  return isdpPduReceiveQueue(pduInfo->intIfNum, bufHandle);
}

/**************************************************************************
* @purpose   Init txrx component
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  register the receive function with sysnet
*
*
* @end
*************************************************************************/
L7_RC_t isdpTxRxInit(void)
{
  L7_RC_t             rc      = L7_SUCCESS;
  sysnetNotifyEntry_t snEntry;

  PT_LOG_INFO(LOG_CTX_STARTUP,"Going to register isdpPduReceive related to type=%u, protocol_type=%u: 0x%08x",
           SYSNET_ETHERTYPE_ENTRY, L7_ETYPE_ISDP, (L7_uint32) isdpPduReceive);

  memset(&snEntry, 0, sizeof(snEntry));

  /*Register the pdu receive function with sysnet utility*/
  osapiStrncpy(snEntry.funcName, "isdpPduReceive", sizeof(snEntry.funcName));
  snEntry.notify_pdu_receive = isdpPduReceive;
  snEntry.type = SYSNET_ETHERTYPE_ENTRY;
  snEntry.u.protocol_type = L7_ETYPE_ISDP;
  rc = sysNetRegisterPduReceive(&snEntry);

  if (isdpBaseTimer == L7_NULLPTR)
  {
    osapiTimerAdd((void*)isdpTimerExpired, 0, 0, ISDP_BASE_TIMEOUT, &isdpBaseTimer);
  }

  return rc;
}

/**************************************************************************
* @purpose   Deinit txrx component
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  unregister the receive function with sysnet
*
*
* @end
*************************************************************************/
L7_RC_t isdpTxRxDeinit(void)
{
  L7_RC_t             rc      = L7_SUCCESS;
  sysnetNotifyEntry_t snEntry;

  memset(&snEntry, 0, sizeof(sysnetNotifyEntry_t));

  /*Register the pdu receive function with sysnet utility*/
  osapiStrncpy(snEntry.funcName, "isdpPduReceive", sizeof(snEntry.funcName));
  snEntry.notify_pdu_receive = isdpPduReceive;
  snEntry.type = SYSNET_ETHERTYPE_ENTRY;
  snEntry.u.protocol_type = L7_ETYPE_ISDP;
  rc = sysNetDeregisterPduReceive(&snEntry);

  if (isdpBaseTimer != L7_NULLPTR)
  {
    osapiTimerFree(isdpBaseTimer);
    isdpBaseTimer = L7_NULLPTR;
  }

  return rc;
}

/*********************************************************************
* @purpose  Put ip address to isdp packet
*
* @param    ipAddress  @b{(input)} ip address
* @param    pdu        @b{(input)} pointer to isdp packet
*
*
* @comments
*
* @end
*********************************************************************/
static void isdpTxRxPutIpAddress(L7_uint32 ipAddress, isdpPacket_t *pdu)
{
  L7_RC_t rc = L7_SUCCESS;

  if(ipAddress != 0 && pdu != L7_NULLPTR)
  {
    rc = isdpIpAddrPutList(&pdu->ipAddressList, ipAddress);
    if(rc == L7_SUCCESS)
    {
      pdu->isdp_address_number++;
      pdu->llc.length += sizeof(isdpTlvAddressByte_t);
    }
  }
}

/*********************************************************************
* @purpose  Transmits a isdp PDU
*
* @param    intIfNum   @b{(input)} interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t isdpTcnTx(L7_uint32 intIfNum, L7_uchar8 version)
{
  isdpPacket_t        *pdu      = L7_NULLPTR;
  L7_uchar8           *pduStart = L7_NULLPTR;
  L7_enetHeader_t     *enetHdr  = L7_NULLPTR;
  L7_netBufHandle     bufHandle = L7_NULL;
  L7_RC_t             rc        = L7_SUCCESS;
  L7_uchar8           ifName[L7_NIM_IFNAME_SIZE + 1];
#ifdef L7_ROUTING_PACKAGE
  L7_IP_ADDR_t        ipAddress = 0;
  L7_IP_MASK_t        mask      = 0;
  L7_uint32           vlanId    = L7_NULL;
  L7_uint32           vlanIntIfNum = L7_NULL;
  L7_BOOL             vlanParticipate = L7_TRUE;
  L7_uint32           mode            = L7_NULL;
  L7_VLAN_MASK_t vidMask;
  memset(&vidMask, L7_NULL, sizeof(L7_VLAN_MASK_t));
#endif /* L7_ROUTING_PACKAGE */
  isdpEntry_t         *pEntry = L7_NULLPTR;
  isdpEntryKey_t      key;

  memset(ifName, 0, sizeof(ifName));

  SYSAPI_NET_MBUF_GET(bufHandle);
  if (bufHandle == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "bufHandle is NULL\n");
    return L7_FAILURE;
  }

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, pduStart);
  pdu = (isdpPacket_t *) pduStart;
  memset((void *)pdu, 0x00, ISDP_MAX_PDU_SIZE);

  enetHdr = (L7_enetHeader_t *)pduStart;
  memcpy(enetHdr->dest.addr, L7_ENET_ISDP_MULTICAST_MAC_ADDR.addr, L7_MAC_ADDR_LEN);
  rc = nimGetIntfAddress(intIfNum, L7_NULL, enetHdr->src.addr);

  pdu->llc.length = (L7_ushort16)(sizeof(isdpHeader_t) +
                    sizeof(L7_802_encaps_t) -
                    sizeof(pdu->llc.length));

  pdu->llc.dsap = ISDP_DSAP_SNAP;
  pdu->llc.ssap = ISDP_SSAP_SNAP;
  pdu->llc.cntl = ISDP_UNNUMBERED_FRAME;
  memcpy(pdu->llc.org, isdpOrganizationCode, sizeof(isdpOrganizationCode));
  pdu->llc.type = osapiHtons(L7_ETYPE_ISDP);

  pdu->isdp_header.holdTime = isdpCfg->cfg.globalCfgData.holdTime;
  pdu->isdp_header.version = version;

  isdpDeviceIdGet(pdu->isdp_device_id);
  pdu->llc.length += strlen(pdu->isdp_device_id);

  pdu->isdp_address_number = 0;
  pdu->llc.length += sizeof(L7_uint32);

  isdpTxRxPutIpAddress(simGetServPortIPAddr(), pdu);
  isdpTxRxPutIpAddress(simGetSystemIPAddr(), pdu);
#ifdef L7_ROUTING_PACKAGE
  rc = ipMapRtrIntfIpAddressGet(intIfNum, &ipAddress, &mask);
  if(rc == L7_SUCCESS)
  {
    isdpTxRxPutIpAddress(ipAddress, pdu);
  }

  do
  {
    rc = dot1qVIDListMaskGet(intIfNum, &vidMask);
    if(rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_ISDP_COMPONENT_ID, "dot1qVIDListMaskGet failed");
      break;
    } 

    L7_VLAN_NONZEROMASK(vidMask, vlanParticipate);

    if(vlanParticipate == L7_FALSE)
    {
      break;
    }

    while(dot1qNextVlanGet(vlanId, &vlanId) == L7_SUCCESS &&
          pdu->llc.length + sizeof(isdpTlvAddress_t) < ISDP_MAX_PDU_SIZE)
    {
      rc = dot1qOperVlanMemberGet(vlanId, intIfNum, &mode);
      if (rc != L7_SUCCESS || mode != L7_DOT1Q_FIXED)
      {
        continue;
      }
      
      rc = dot1qVlanIntfVlanIdToIntIfNum(vlanId, &vlanIntIfNum);
      if(rc != L7_SUCCESS)
      {
        continue;
      }
      
      rc = ipMapRtrIntfCfgIpAddressGet(vlanIntIfNum, &ipAddress, &mask);
      if(rc != L7_SUCCESS || ipAddress == L7_NULL)
      {
        continue;
      }

      isdpTxRxPutIpAddress(ipAddress, pdu);
    }
  } while(L7_FALSE);

#ifdef L7_RLIM_PACKAGE
  rc = rlimTunnelLocalIp4AddrGet(intIfNum, &ipAddress);
  if(rc == L7_SUCCESS)
  {
    isdpTxRxPutIpAddress(ipAddress, pdu);
  }
#endif /* L7_RLIM_PACKAGE */
#endif /* L7_ROUTING_PACKAGE */

  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
  osapiStrncpy(pdu->isdp_port_id, ifName, L7_ISDP_PORT_ID_LEN);

  pdu->llc.length += strlen(pdu->isdp_port_id);

  if(cnfgrIsFeaturePresent(L7_LAYER3_COMPONENT_ID,  0))
    pdu->isdp_capabilities = ISDP_CAP_ROUTER_BITMASK;
  else
    pdu->isdp_capabilities = ISDP_CAP_SWITCH_BITMASK;

  pdu->llc.length += sizeof(pdu->isdp_capabilities);

  sysapiRegistryGet(SW_VERSION, STR_ENTRY, (void*) pdu->isdp_version);
  pdu->llc.length += strlen(pdu->isdp_version);

  sysapiRegistryGet(MACHINE_MODEL, STR_ENTRY, (void*) pdu->isdp_platform);
  pdu->llc.length += strlen(pdu->isdp_platform);

  pdu->llc.length += 6 * sizeof(isdpTlvTypeLength_t);

  /* Search for a neighbor's entry for this interface */
  memset(&key, 0, sizeof(key));
  key.intIfNum = intIfNum - 1;          /* Start with the previous interface */
  pEntry = (isdpEntry_t *)&key;
  do
  {
    pEntry = isdpEntryGetNext(pEntry->key);
    if (L7_NULLPTR == pEntry)
    {
      break;
    }

    if (pEntry->key.intIfNum < intIfNum)
    {
      continue;
    }

    if (pEntry->key.intIfNum == intIfNum)
    {
      /*
       * Found an entry for this interface.  Now check with voice VLAN to see if there is
       * a voice VLAN for this device.
       */
      if (L7_SUCCESS == voiceVlanPortDeviceVlanGet(intIfNum, pEntry->mac.addr, &pdu->isdp_voip_vlan))
      {
        /* Found a voice VLAN, no need to search further */
        pdu->llc.length += 7;
        break;
      }
    }
  } while (pEntry->key.intIfNum == intIfNum);

  rc = isdpPduTransmit(bufHandle, intIfNum);
  if (rc == L7_SUCCESS)
  {
    isdpDebugPacketTxTrace(intIfNum, pdu);
    isdpTrafficPduTransmitInc(intIfNum);
    if(pdu->isdp_header.version == ISDP_VERSION_1)
    {
      isdpTrafficV1PduTransmitInc(intIfNum);
    }
    else if(pdu->isdp_header.version == ISDP_VERSION_2)
    {
      isdpTrafficV2PduTransmitInc(intIfNum);
    }
  }
  else
  {
    isdpTrafficFailurePduTransmitInc(intIfNum);
  }

  return rc;
}

/*********************************************************************
* @purpose  Put TLV type and length to pdu
*
* @param    type      @b{(input)}   type of TLV
* @param    length    @b{(input)}   length of TLV
* @param    pdu       @b{(output)}  pdu buffer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t isdpTxRxTlvTypeLengthPut(L7_ushort16 type, L7_ushort16 length, L7_uchar8* pdu)
{
  L7_ushort16 tempConversion  = 0;

  tempConversion = osapiHtons(type);
  memcpy(pdu,&tempConversion,sizeof(type));
  tempConversion = osapiHtons(length);
  memcpy((pdu+sizeof(type)),&tempConversion,sizeof(length));

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Put VoIP VLAN Reply in PDU
*
* @param    pdu       @b{(input)}   isdp packet
* @param    data_ptr  @b{(output)}  pointer to pdu buffer pointer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t isdpTxRxTlvVoipVlanReplyPut(isdpPacket_t *pdu,
                                           L7_uchar8    **data_ptr)
{
  L7_ushort16 vlanId;

  if (0 != pdu->isdp_voip_vlan)
  {
    isdpTxRxTlvTypeLengthPut(ISDP_VOIP_VLAN_REPLY_TLV_TYPE, 7, *data_ptr);

    *data_ptr += sizeof(isdpTlvTypeLength_t);

    **data_ptr = 1;
    *data_ptr += 1;

    vlanId = osapiHtons(pdu->isdp_voip_vlan);
    memcpy(*data_ptr, &vlanId, sizeof(pdu->isdp_voip_vlan));
    *data_ptr += sizeof(pdu->isdp_voip_vlan);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Put TLV string value to pdu
*
* @param    type      @b{(input)}   type of TLV
* @param    string    @b{(input)}   value of TLV
* @param    data_ptr  @b{(output)}  pointer to pdu buffer pointer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t isdpTxRxTlvStringValuePut(L7_ushort16 type, L7_uchar8* string,
                                         L7_uchar8** data_ptr)
{
  L7_uint32 string_length = 0;

  string_length = strlen(string);

  isdpTxRxTlvTypeLengthPut(type, string_length +
                           sizeof(isdpTlvTypeLength_t), *data_ptr);

  *data_ptr += sizeof(isdpTlvTypeLength_t);

  memcpy(*data_ptr, string, string_length);
  *data_ptr += string_length;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Put TLV address value to pdu
*
* @param    pdu       @b{(input)}   isdp packet
* @param    data_ptr  @b{(output)}  pointer to pdu buffer pointer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t isdpTxRxTlvAddressValuePut(isdpPacket_t *pdu,
                                          L7_uchar8    **data_ptr)
{
  L7_uint32   temp32        = 0;
  L7_ushort16 temp16        = 0;
  L7_uint32   address       = 0;
  L7_uint32   i             = 1;
  L7_RC_t     addrRc        = L7_SUCCESS;

  isdpTxRxTlvTypeLengthPut(ISDP_ADDRESS_TLV_TYPE,
                           sizeof(isdpTlvAddressByte_t) * pdu->isdp_address_number +
                           sizeof(isdpTlvTypeLength_t) + sizeof(L7_uint32),
                           *data_ptr);

  *data_ptr += sizeof(isdpTlvTypeLength_t);

  temp32 = osapiHtonl(pdu->isdp_address_number);
  memcpy(*data_ptr, &temp32, sizeof(temp32));
  *data_ptr += sizeof(temp32);

  for(i = 1; i < pdu->isdp_address_number + 1; i++)
  {
    addrRc = isdpIpAddrListIndexGet(pdu->ipAddressList, i, &address);
    if(addrRc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
              "Couldn't get address\n");
      return addrRc;
    }

    **data_ptr = ISDP_NLPID;
    *data_ptr += 1;

    **data_ptr = ISDP_PROTO_LEN;
    *data_ptr += 1;

    **data_ptr = ISDP_IP;
    *data_ptr += 1;

    temp16 = osapiHtons(L7_IP_ADDR_LEN);
    memcpy(*data_ptr, &temp16, sizeof(temp16));
    *data_ptr += sizeof(temp16);

    temp32 = osapiHtonl(address);
    memcpy(*data_ptr, &temp32, sizeof(temp32));
    *data_ptr += sizeof(temp32);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Put capabilities TLV value to pdu
*
* @param    data_ptr  @b{(output)}  pointer to pdu buffer pointer
* @param    pdu       @b{(input)}  isdp packet
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t isdpTxRxTlvCapabilitiesValuePut(L7_uchar8** data_ptr,
                                               isdpPacket_t* pdu)
{
  L7_uint32 temp = 0;

  isdpTxRxTlvTypeLengthPut(ISDP_CAPABILITIES_TLV_TYPE, sizeof(pdu->isdp_capabilities) +
      sizeof(isdpTlvTypeLength_t), *data_ptr);
  *data_ptr += sizeof(isdpTlvTypeLength_t);

  temp = osapiHtonl(pdu->isdp_capabilities);
  memcpy(*data_ptr, &temp, sizeof(pdu->isdp_capabilities));
  *data_ptr += sizeof(pdu->isdp_capabilities);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Put TLVs to pdu
*
* @param    data_ptr  @b{(output)}  pointer to pdu buffer
* @param    pdu       @b{(input)}  isdp packet
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t isdpTxRxTlvsPut(L7_uchar8* data_ptr, isdpPacket_t* pdu)
{
  isdpTxRxTlvStringValuePut(ISDP_DEVICE_ID_TLV_TYPE, pdu->isdp_device_id, &data_ptr);

  isdpTxRxTlvStringValuePut(ISDP_PLATFORM_TLV_TYPE, pdu->isdp_platform, &data_ptr);

  isdpTxRxTlvStringValuePut(ISDP_PORT_ID_TLV_TYPE, pdu->isdp_port_id, &data_ptr);

  isdpTxRxTlvCapabilitiesValuePut(&data_ptr, pdu);

  isdpTxRxTlvStringValuePut(ISDP_VERSION_TLV_TYPE, pdu->isdp_version, &data_ptr);

  isdpTxRxTlvAddressValuePut(pdu, &data_ptr);

  isdpIpAddrListRelease(&pdu->ipAddressList);

  isdpTxRxTlvVoipVlanReplyPut(pdu, &data_ptr);

  return L7_SUCCESS;
}

/**************************************************************************
* @purpose   Convert the packet from host byte order (Little endianess) to
*       network byte order (Big endianess)
*
* @param   bufHandle  @b{(input)} the handle to the LACPDU
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*************************************************************************/
L7_netBufHandle isdpTxRxNetworkConvert(L7_netBufHandle bufHandle,
                                       L7_uint32 intIfNum)
{
  L7_uchar8             *data              = L7_NULLPTR;
  L7_uchar8             *unpaddedData      = L7_NULLPTR;
  L7_uchar8             *data_ptr          = L7_NULLPTR;
  isdpPacket_t          *pdu               = L7_NULLPTR;
  isdpPacket_t          *unpaddedPdu       = L7_NULLPTR;
  L7_netBufHandle       unpaddedBufHandle  = L7_NULL;
  L7_uint32             packetLength       = 0;

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  pdu = (isdpPacket_t *)data;

  /*get a new network buffer*/

  SYSAPI_NET_MBUF_GET(unpaddedBufHandle);
  if (unpaddedBufHandle == L7_NULL)
  {
    /*free up the padded buffer*/
    SYSAPI_NET_MBUF_FREE(bufHandle);

    return L7_NULL;
  }

  SYSAPI_NET_MBUF_GET_DATASTART(unpaddedBufHandle, unpaddedData);
  unpaddedPdu = (isdpPacket_t *) unpaddedData;
  data_ptr = unpaddedData;

  memset((void *)unpaddedPdu, 0x00, ISDP_MAX_PDU_SIZE);

  memcpy(unpaddedPdu->ether.dest.addr, pdu->ether.dest.addr, L7_MAC_ADDR_LEN);
  memcpy(unpaddedPdu->ether.src.addr, pdu->ether.src.addr, L7_MAC_ADDR_LEN);
  unpaddedPdu->llc.length = osapiHtons(pdu->llc.length);
  memcpy(&unpaddedPdu->llc, &pdu->llc, sizeof(L7_802_encaps_t));
  unpaddedPdu->isdp_header.version = pdu->isdp_header.version;
  unpaddedPdu->isdp_header.holdTime = pdu->isdp_header.holdTime;

  data_ptr += sizeof(L7_enetHeader_t) + sizeof(L7_802_encaps_t) +
  sizeof(isdpHeader_t) ;

  isdpTxRxTlvsPut(data_ptr, pdu);

  if(pdu->isdp_header.version == ISDP_VERSION_1)
  {
    isdpChecksum(&unpaddedPdu->isdp_header,
                 unpaddedPdu->llc.length -
                 sizeof(L7_802_encaps_t) +
                 sizeof(unpaddedPdu->llc.length),
                 &unpaddedPdu->isdp_header.checksum);
  }
  else  /* ISDP_VERSION_2 */
  {
    isdpV2CheckSum(&unpaddedPdu->isdp_header,
                   unpaddedPdu->llc.length -
                   sizeof(L7_802_encaps_t) +
                   sizeof(unpaddedPdu->llc.length),
                   &unpaddedPdu->isdp_header.checksum);
  }
  
  packetLength = unpaddedPdu->llc.length +
                 sizeof(L7_enetHeader_t) +
                 sizeof(unpaddedPdu->llc.length);

  SYSAPI_NET_MBUF_SET_DATALENGTH(unpaddedBufHandle, packetLength);
  /*free up the padded buffer*/
  SYSAPI_NET_MBUF_FREE(bufHandle);

  return unpaddedBufHandle;
}

/*********************************************************************
* @purpose  Calclulate ISDP checksum
*
* @param    data      @b{(input)}  data
* @param    length    @b{(input)}  data length
* @param    checksum  @b{(output)}  checksum
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This is different from IP checksum slightly
*           Eventually this checksum algorithm should be used in Tx side
*           also
*
* @end
*********************************************************************/
L7_RC_t isdpV2CheckSum(void* data, size_t length, L7_ushort16* checksum)
{
  register       L7_long32   sum = 0;
  register const L7_ushort16 *d  = (const L7_ushort16 *)data;
  L7_uchar8 *lastByte = (L7_uchar8*)data;

  L7_uchar8 covtLastByteIntoShort[2]= {0,0};
  L7_ushort16 convertedLastShort    = 0;

  if (data == L7_NULLPTR || checksum == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "One of parameters is equal to NULL\n");
    return L7_FAILURE;
  }

  while (length > 1)
  {
    sum += *d++;
    length -= 2;
  }
  if (length > 0)
  {
    lastByte = lastByte + (d- (L7_ushort16 *)data)*2;

    if ( *lastByte & 0x80)
    {
      covtLastByteIntoShort[1]= *lastByte - 1;
      covtLastByteIntoShort[0]= 0xff;
    }
    else
    {
      covtLastByteIntoShort[1]= *lastByte;
      covtLastByteIntoShort[0]= 0x0;
    }
    memcpy((char*)&convertedLastShort, covtLastByteIntoShort, sizeof(convertedLastShort));
  }

  sum = sum+ convertedLastShort;

  /*  Fold 32-bit sum to 16 bits */
  while (sum>>16)
    sum = (sum & 0xffff) + (sum >> 16);

  *checksum = ~sum;

  return L7_SUCCESS;
}

L7_RC_t isdpChecksum(void* data, size_t length, L7_ushort16* checksum)
{
  register       L7_long32   sum = 0;
  register const L7_ushort16 *d  = (const L7_ushort16 *)data;
  L7_uchar8 *leftOverByte = (L7_uchar8*)data;

  if (data == L7_NULLPTR || checksum == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "One of parameters is equal to NULL\n");
    return L7_FAILURE;
  }

  while (length > 1)
  {
    sum += *d++;
    length -= 2;
    leftOverByte += 2;
  }
  if (length > 0)
  {
    sum += *leftOverByte;
  }

  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  *checksum = ~sum;

  return L7_SUCCESS;
}
