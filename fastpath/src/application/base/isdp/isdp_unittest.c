/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
* @filename isdp_unittest.c
*
* @purpose isdp unit test file
*
* @component isdp
*
* @comments none
*
* @create 06/02/2008
*
* @author dgaryachy
*
* @end
*
**********************************************************************/

#define L7_MAC_ISDP_PDU

#include "isdp_include.h"
#include "isdp_api.h"
#include "isdp_debug_api.h"
#include "support_api.h"
#include "isdp_unittest.h"
#include "simapi.h"

L7_RC_t isdpUnitTestPduReceive(L7_uint32 intIfNum, L7_netBufHandle bufHandle)
{
  L7_RC_t rc = L7_SUCCESS;
  isdpEntry_t isdpEntry;
  L7_uchar8 *data = L7_NULLPTR;
  L7_uint32  length = 0;
  L7_uint32  offset = 0;
  isdpPacket_t isdpPacket;

  memset(&isdpEntry, 0x00, sizeof(isdpEntry_t));
  memset(&isdpPacket, 0x00, sizeof(isdpPacket_t));

  if (isdpIsValidIntf(intIfNum) != L7_TRUE)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_DebugGroup,
                   "%s: LINE %d: interface %d isn't valid \n",
                   __FUNCTION__, __LINE__, intIfNum);

    return L7_FAILURE;
  }

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, length);
  offset = sysNetDataOffsetGet(data);

  memcpy(isdpPacket.ether.src.addr, data + L7_MAC_ADDR_LEN, L7_MAC_ADDR_LEN);

  rc = isdpDataPduRead(intIfNum, data + offset, length - offset, &isdpPacket);
  isdpDebugPacketRxTrace(intIfNum, &isdpPacket);

  SYSAPI_NET_MBUF_FREE(bufHandle);

  if(rc == L7_FAILURE)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_DebugGroup,
                   "%s: LINE %d: failed on interface %d\n",
                   __FUNCTION__, __LINE__, intIfNum);

    return rc;
  }

  rc = isdpFillEntry(&isdpEntry, &isdpPacket, intIfNum);

  if(rc == L7_FAILURE)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_DebugGroup,
                   "%s: LINE %d: failed on interface %d\n",
                   __FUNCTION__, __LINE__, intIfNum);
    return rc;
  }

  rc = isdpEntryAdd(&isdpEntry);

  return rc;
}

L7_RC_t isdpUnitTestPacketTransmit(L7_netBufHandle paddedBufHandle,
              L7_uint32 intIfNum)
{
  L7_netBufHandle bufHandle = L7_NULL;
  L7_RC_t rc = L7_SUCCESS;

  bufHandle =  isdpTxRxNetworkConvert(paddedBufHandle, intIfNum);
  if (bufHandle == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ISDP_COMPONENT_ID,
            "%s: could not get a buffer\n", __FUNCTION__);
    return L7_FAILURE;
  }

  rc = isdpUnitTestPduReceive(intIfNum, bufHandle);

  return rc;
}

L7_RC_t isdpUnitTestAddNeighbors(L7_uint32 number, L7_char8 holdtime)
{
  L7_uint32 i = 0;
  isdpEntryKey_t key;
  L7_RC_t rc = L7_SUCCESS;
  L7_RC_t rcNim = L7_SUCCESS;
  isdpEntry_t* pEntry = L7_NULLPTR;
  L7_uint32 intIfNum = 0;
  L7_uint32 sentNumber = 0;
  L7_netBufHandle bufHandle = L7_NULL;

  memset((void *)&key, 0x00, sizeof(isdpEntryKey_t));

  sysapiPrintf("%s: Adding %d neighbor entries\n", __FUNCTION__, number);

  rcNim = nimFirstValidIntfNumber(&intIfNum);

  for(i = 1; i < number + 1; i++)
  {
    key.intIfNum = intIfNum;
    osapiSnprintf(key.deviceId, L7_ISDP_DEVICE_ID_LEN, "%d", i);

    rc = isdpUnitTestPacketCreate(key.intIfNum, key.deviceId, holdtime,
                                  L7_ETYPE_ISDP, 2, 0, &bufHandle);
    rc = isdpUnitTestPacketTransmit(bufHandle, intIfNum);

    if(rc == L7_SUCCESS)
    {
      pEntry = isdpEntryFind(key);
      if(pEntry == L7_NULL)
      {
        sysapiPrintf("%s: couldn't find\n", __FUNCTION__);
        return L7_FAILURE;
      }
      sentNumber++;
    }

    rcNim = nimNextValidIntfNumber(intIfNum, &intIfNum);
    if(rcNim == L7_FAILURE)
    {
      intIfNum = 0;
      rcNim = nimFirstValidIntfNumber(&intIfNum);
    }
  }

  sysapiPrintf("%s: succeded, added %d neighbors\n", __FUNCTION__, sentNumber);
  return L7_SUCCESS;
}

void isdpUnitTestDatabasePositiveTests()
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 holdtime = 200;
  L7_uint32 neighborsNum = 10;

  rc = isdpUnitTestAddNeighbors(neighborsNum, holdtime);

  if(rc != L7_SUCCESS)
  {
    sysapiPrintf("%s: failed\n", __FUNCTION__);
    return;
  }

  sysapiPrintf("%s: succeded\n", __FUNCTION__);
}

void isdpUnitTestDatabaseBoundaryTests()
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 holdtime = 200;

  rc = isdpUnitTestAddNeighbors(L7_ISDP_MAX_NEIGHBORS, holdtime);
  if(rc != L7_SUCCESS)
  {
    sysapiPrintf("%s: failed\n", __FUNCTION__);
    return;
  }

  sysapiPrintf("%s: succeded\n", __FUNCTION__);
}

void isdpUnitTestDatabaseNegativeTests()
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 holdtime = 200;

  rc = isdpUnitTestAddNeighbors(L7_ISDP_MAX_NEIGHBORS + 1, holdtime);
  if(rc == L7_SUCCESS)
  {
    sysapiPrintf("%s: failed\n", __FUNCTION__);
    return;
  }

  sysapiPrintf("%s: succeded\n", __FUNCTION__);
}

void isdpUnitTestApi()
{
  L7_char8 deviceIdFormatCpb = L7_NULL;
  L7_uint32 deviceIdFormat = 0;
  L7_uint32 mode = L7_DISABLE;
  L7_uint32 intfMode = L7_DISABLE;
  L7_uint32 timer = 0;
  L7_uint32 holdTimeInt = 0;
  L7_uint32 v2Mode = L7_DISABLE;
  L7_uint32 intIfNum = 0;
  L7_char8 deviceId[L7_ISDP_DEVICE_ID_LEN];
  L7_char8 platform[L7_ISDP_PLATFORM_LEN];
  L7_char8 version[L7_ISDP_VERSION_LEN];
  L7_char8 portId[L7_ISDP_PORT_ID_LEN];
  L7_uint32 capabilities = 0;
  L7_uint32 address = 0;
  L7_uchar8 holdTime = 0;
  L7_char8 protoVersion = 0;
  L7_uint32 lastChangeTime = 0;
  L7_uint32 txTraceMode = 0;
  L7_uint32 rxTraceMode = 0;
  L7_uint32 eventsTraceMode = 0;

  memset(deviceId, 0x00, sizeof(deviceId));
  memset(platform, 0x00, sizeof(platform));
  memset(version, 0x00, sizeof(version));
  memset(portId, 0x00, sizeof(portId));

  isdpDeviceIdFormatCapabilityGet(&deviceIdFormatCpb);
  sysapiPrintf("%s: LINE %d: isdpDeviceIdFormatCapabilityGet success\n",
               __FUNCTION__, __LINE__);

  isdpDeviceIdFormatCapabilityGet(0);
  sysapiPrintf("%s: LINE %d: isdpDeviceIdFormatCapabilityGet success\n",
               __FUNCTION__, __LINE__);

  isdpDeviceIdFormatGet(&deviceIdFormat);
  sysapiPrintf("%s: LINE %d: isdpDeviceIdFormatGet success\n",
               __FUNCTION__, __LINE__);

  isdpDeviceIdFormatGet(0);
  sysapiPrintf("%s: LINE %d: isdpDeviceIdFormatGet success\n",
               __FUNCTION__, __LINE__);

  isdpModeSet(mode);
  sysapiPrintf("%s: LINE %d: isdpModeSet success\n", __FUNCTION__, __LINE__);

  isdpModeGet(&mode);
  sysapiPrintf("%s: LINE %d: isdpModeGet success\n", __FUNCTION__, __LINE__);

  isdpModeGet(0);
  sysapiPrintf("%s: LINE %d: isdpModeGet success\n", __FUNCTION__, __LINE__);

  isdpTimerSet(timer);
  sysapiPrintf("%s: LINE %d: isdpTimerSet success\n", __FUNCTION__, __LINE__);

  isdpTimerGet(&timer);
  sysapiPrintf("%s: LINE %d: isdpTimerGet success\n", __FUNCTION__, __LINE__);

  isdpTimerGet(0);
  sysapiPrintf("%s: LINE %d: isdpTimerGet success\n", __FUNCTION__, __LINE__);

  isdpHoldTimeSet(holdTimeInt);
  sysapiPrintf("%s: LINE %d: isdpHoldTimeSet success\n", __FUNCTION__, __LINE__);

  isdpHoldTimeGet(&holdTimeInt);
  sysapiPrintf("%s: LINE %d: isdpHoldTimeGet success\n", __FUNCTION__, __LINE__);

  isdpHoldTimeGet(0);
  sysapiPrintf("%s: LINE %d: isdpHoldTimeGet success\n", __FUNCTION__, __LINE__);

  isdpV2ModeSet(v2Mode);
  sysapiPrintf("%s: LINE %d: isdpV2ModeSet success\n", __FUNCTION__, __LINE__);

  isdpV2ModeGet(&v2Mode);
  sysapiPrintf("%s: LINE %d: isdpV2ModeGet success\n", __FUNCTION__, __LINE__);

  isdpV2ModeGet(0);
  sysapiPrintf("%s: LINE %d: isdpV2ModeGet success\n", __FUNCTION__, __LINE__);

  isdpIntfModeSet(intIfNum, intfMode);
  sysapiPrintf("%s: LINE %d: isdpIntfModeSet success\n", __FUNCTION__, __LINE__);

  isdpIntfModeSet(0, 0);
  sysapiPrintf("%s: LINE %d: isdpIntfModeSet success\n", __FUNCTION__, __LINE__);

  isdpIntfModeSet(0, 0);
  sysapiPrintf("%s: LINE %d: isdpIntfModeSet success\n", __FUNCTION__, __LINE__);

  isdpIntfModeGet(intIfNum, &intfMode);
  sysapiPrintf("%s: LINE %d: isdpIntfModeGet success\n", __FUNCTION__, __LINE__);

  isdpIntfModeGet(0, 0);
  sysapiPrintf("%s: LINE %d: isdpIntfModeGet success\n", __FUNCTION__, __LINE__);

  isdpTrafficCountersClear();
  sysapiPrintf("%s: LINE %d: isdpTrafficCountersClear success\n",
                __FUNCTION__, __LINE__);

  isdpNeighborTableClear();
  sysapiPrintf("%s: LINE %d: isdpNeighborTableClear success\n",
               __FUNCTION__, __LINE__);

  isdpIntfNeighborGetNext(intIfNum, deviceId, deviceId);
  sysapiPrintf("%s: LINE %d: isdpIntfNeighborGetNext success\n",
               __FUNCTION__, __LINE__);

  isdpIntfNeighborGetNext(0, 0, 0);
  sysapiPrintf("%s: LINE %d: isdpIntfNeighborGetNext success\n",
               __FUNCTION__, __LINE__);

  isdpDeviceIdNeighborGetNext(intIfNum, deviceId, &intIfNum, deviceId);
  sysapiPrintf("%s: LINE %d: isdpDeviceIdNeighborGetNext success\n",
               __FUNCTION__, __LINE__);

  isdpDeviceIdNeighborGetNext(0, 0, 0, 0);
  sysapiPrintf("%s: LINE %d: isdpDeviceIdNeighborGetNext success\n",
               __FUNCTION__, __LINE__);

  isdpNeighborGetNext(intIfNum, deviceId, &intIfNum, deviceId);
  sysapiPrintf("%s: LINE %d: isdpNeighborGetNext success\n",
               __FUNCTION__, __LINE__);

  isdpNeighborGetNext(0, 0, 0, 0);
  sysapiPrintf("%s: LINE %d: isdpNeighborGetNext success\n",
               __FUNCTION__, __LINE__);

  isdpIntfNeighborPlatformGet(intIfNum, deviceId, platform);
  sysapiPrintf("%s: LINE %d: isdpIntfNeighborPlatformGet success\n",
               __FUNCTION__, __LINE__);

  isdpIntfNeighborPlatformGet(0, 0, 0);
  sysapiPrintf("%s: LINE %d: isdpIntfNeighborPlatformGet success\n",
               __FUNCTION__, __LINE__);

  isdpIntfNeighborVersionGet(intIfNum, deviceId, version);
  sysapiPrintf("%s: LINE %d: isdpIntfNeighborVersionGet success\n",
               __FUNCTION__, __LINE__);

  isdpIntfNeighborVersionGet(0, 0, 0);
  sysapiPrintf("%s: LINE %d: isdpIntfNeighborVersionGet success\n",
               __FUNCTION__, __LINE__);

  isdpIntfNeighborPortIdGet(intIfNum, deviceId, portId);
  sysapiPrintf("%s: LINE %d: isdpIntfNeighborPortIdGet success\n",
               __FUNCTION__, __LINE__);

  isdpIntfNeighborPortIdGet(0, 0, 0);
  sysapiPrintf("%s: LINE %d: isdpIntfNeighborPortIdGet success\n",
               __FUNCTION__, __LINE__);

  isdpIntfNeighborAddressGetNext(intIfNum, deviceId, address, &address);
  sysapiPrintf("%s: LINE %d: isdpNeighborAddressGetNext success\n",
               __FUNCTION__, __LINE__);

  isdpIntfNeighborAddressGetNext(0, 0, 0, 0);
  sysapiPrintf("%s: LINE %d: isdpNeighborAddressGetNext success\n",
               __FUNCTION__, __LINE__);

  isdpIntfNeighborCapabilitiesGet(intIfNum, deviceId, &capabilities);
  sysapiPrintf("%s: LINE %d: isdpIntfNeighborCapabilitiesGet success\n",
               __FUNCTION__, __LINE__);

  isdpIntfNeighborCapabilitiesGet(0, 0, 0);
  sysapiPrintf("%s: LINE %d: isdpIntfNeighborCapabilitiesGet success\n",
               __FUNCTION__, __LINE__);

  isdpIntfNeighborHoldTimeGet(intIfNum, deviceId, &holdTime);
  sysapiPrintf("%s: LINE %d: isdpIntfNeighborHoldTimeGet success\n",
               __FUNCTION__, __LINE__);

  isdpIntfNeighborHoldTimeGet(0, 0, 0);
  sysapiPrintf("%s: LINE %d: isdpIntfNeighborHoldTimeGet success\n",
               __FUNCTION__, __LINE__);

  isdpIntfNeighborProtocolVersionGet(intIfNum, deviceId, &protoVersion);
  sysapiPrintf("%s: LINE %d: isdpIntfNeighborProtocolVersionGet success\n",
               __FUNCTION__, __LINE__);

  isdpIntfNeighborProtocolVersionGet(0, 0, 0);
  sysapiPrintf("%s: LINE %d: isdpIntfNeighborProtocolVersionGet success\n",
               __FUNCTION__, __LINE__);

  isdpIntfNeighborLastChangeTimeGet(intIfNum, deviceId, &lastChangeTime);
  sysapiPrintf("%s: LINE %d: isdpIntfNeighborLastChangeTimeGet success\n",
               __FUNCTION__, __LINE__);

  isdpIntfNeighborLastChangeTimeGet(0, 0, 0);
  sysapiPrintf("%s: LINE %d: isdpIntfNeighborLastChangeTimeGet success\n",
               __FUNCTION__, __LINE__);

  isdpTrafficPduReceivedGet();
  sysapiPrintf("%s: LINE %d: isdpTrafficPduReceivedGet success\n",
               __FUNCTION__, __LINE__);

  isdpTrafficV1PduReceivedGet();
  sysapiPrintf("%s: LINE %d: isdpTrafficV1PduReceivedGet success\n",
               __FUNCTION__, __LINE__);

  isdpTrafficV2PduReceivedGet();
  sysapiPrintf("%s: LINE %d: isdpTrafficV2PduReceivedGet success\n",
               __FUNCTION__, __LINE__);

  isdpTrafficBadHeaderPduReceivedGet();
  sysapiPrintf("%s: LINE %d: isdpTrafficBadHeaderPduReceivedGet success\n",
               __FUNCTION__, __LINE__);

  isdpTrafficChkSumErrorPduReceivedGet();
  sysapiPrintf("%s: LINE %d: isdpTrafficChkSumErrorPduReceivedGet success\n",
               __FUNCTION__, __LINE__);

  isdpTrafficInvalidFormatPduReceivedGet();
  sysapiPrintf("%s: LINE %d: isdpTrafficInvalidFormatPduReceivedGet success\n",
               __FUNCTION__, __LINE__);

  isdpTrafficPduTransmitGet();
  sysapiPrintf("%s: LINE %d: isdpTrafficPduTransmitGet success\n",
               __FUNCTION__, __LINE__);

  isdpTrafficV1PduTransmitGet();
  sysapiPrintf("%s: LINE %d: isdpTrafficV1PduTransmitGet success\n",
               __FUNCTION__, __LINE__);

  isdpTrafficV2PduTransmitGet();
  sysapiPrintf("%s: LINE %d: isdpTrafficV2PduTransmitGet success\n",
               __FUNCTION__, __LINE__);

  isdpTrafficFailurePduTransmitGet();
  sysapiPrintf("%s: LINE %d: isdpTrafficFailurePduTransmitGet success\n",
               __FUNCTION__, __LINE__);

  isdpTrafficTableFullGet();
  sysapiPrintf("%s: LINE %d: isdpTrafficTableFullGet success\n",
               __FUNCTION__, __LINE__);

  isdpNeighborsTableLastChangeTimeGet();
  sysapiPrintf("%s: LINE %d: isdpNeighborsTableLastChangeTimeGet success\n",
               __FUNCTION__, __LINE__);

  isdpDebugTracePacketTxModeSet(txTraceMode);
  sysapiPrintf("%s: LINE %d: isdpDebugTracePacketTxModeSet success\n",
               __FUNCTION__, __LINE__);

  isdpDebugTracePacketTxModeGet(&txTraceMode);
  sysapiPrintf("%s: LINE %d: isdpDebugTracePacketTxModeGet success\n",
               __FUNCTION__, __LINE__);

  isdpDebugTracePacketTxModeGet(0);
  sysapiPrintf("%s: LINE %d: isdpDebugTracePacketTxModeGet success\n",
               __FUNCTION__, __LINE__);

  isdpDebugTracePacketRxModeSet(rxTraceMode);
  sysapiPrintf("%s: LINE %d: isdpDebugTracePacketRxModeSet success\n",
               __FUNCTION__, __LINE__);

  isdpDebugTracePacketRxModeGet(&rxTraceMode);
  sysapiPrintf("%s: LINE %d: isdpDebugTracePacketRxModeGet success\n",
               __FUNCTION__, __LINE__);

  isdpDebugTracePacketRxModeGet(0);
  sysapiPrintf("%s: LINE %d: isdpDebugTracePacketRxModeGet success\n",
               __FUNCTION__, __LINE__);

  isdpDebugTraceEventsModeSet(eventsTraceMode);
  sysapiPrintf("%s: LINE %d: isdpDebugTraceEventsModeSet success\n",
               __FUNCTION__, __LINE__);

  isdpDebugTraceEventsModeGet(&eventsTraceMode);
  sysapiPrintf("%s: LINE %d: isdpDebugTraceEventsModeGet success\n",
               __FUNCTION__, __LINE__);

  isdpDebugTraceEventsModeGet(0);
  sysapiPrintf("%s: LINE %d: isdpDebugTraceEventsModeGet success\n",
               __FUNCTION__, __LINE__);

  sysapiPrintf("%s: succeded\n", __FUNCTION__);
}

L7_RC_t isdpUnitTestPacketCreate(L7_uint32 intIfNum, L7_char8* deviceId,
                                 L7_char8 holdTime, L7_short16 etype,
                                 L7_char8 version, L7_uint32 addToLlcLength,
                                 L7_netBufHandle *pBufHandle)
{
  isdpPacket_t        *pdu      = L7_NULLPTR;
  L7_uchar8           *pduStart = L7_NULLPTR;
  L7_RC_t             rc        = L7_SUCCESS;
  L7_uchar8           ifName[L7_NIM_IFNAME_SIZE + 1];

  memset(ifName, 0 , sizeof(ifName));

  SYSAPI_NET_MBUF_GET(*pBufHandle);
  if (*pBufHandle == L7_NULL)
  {
    isdpDebugTrace(ISDP_DBG_FLAG_PduGroup,
                   "%s: LINE %d: bufHandle is NULL, interface %d\n",
                   __FUNCTION__, __LINE__, intIfNum);

    return L7_FAILURE;
  }

  SYSAPI_NET_MBUF_GET_DATASTART(*pBufHandle, pduStart);
  pdu = (isdpPacket_t *) pduStart;
  memset((void *)pdu, 0x00, ISDP_MAX_PDU_SIZE);

  memcpy(pdu->ether.dest.addr, L7_ENET_ISDP_MULTICAST_MAC_ADDR.addr, L7_MAC_ADDR_LEN);
  rc = nimGetIntfAddress(intIfNum, L7_NULL, pdu->ether.src.addr);

  pdu->llc.length = (L7_ushort16)(sizeof(isdpHeader_t) +
                    sizeof(L7_802_encaps_t) -
                    sizeof(pdu->llc.length));

  pdu->llc.dsap = ISDP_DSAP_SNAP;
  pdu->llc.ssap = ISDP_SSAP_SNAP;
  pdu->llc.cntl = ISDP_UNNUMBERED_FRAME;
  pdu->llc.type = osapiHtons(etype);

  pdu->isdp_header.holdTime = holdTime;
  pdu->isdp_header.version = version;

  osapiStrncpy(pdu->isdp_device_id,  deviceId, L7_ISDP_DEVICE_ID_LEN);
  pdu->llc.length += strlen(pdu->isdp_device_id);

  pdu->isdp_address_number = 2;
  pdu->llc.length += sizeof(L7_uint32);

  isdpIpAddrPutList(&pdu->ipAddressList, simGetServPortIPAddr());
  pdu->llc.length += sizeof(isdpTlvAddress_t);

  isdpIpAddrPutList(&pdu->ipAddressList, simGetServPortIPAddr());
  pdu->llc.length += sizeof(isdpTlvAddress_t);

  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
  osapiStrncpy(pdu->isdp_port_id, ifName, L7_ISDP_PORT_ID_LEN);

  pdu->llc.length += strlen(pdu->isdp_port_id);

  if(cnfgrIsFeaturePresent(L7_LAYER3_COMPONENT_ID,  0))
    pdu->isdp_capabilities = ISDP_CAP_ROUTER_BITMASK;
  else
    pdu->isdp_capabilities = ISDP_CAP_SWITCH_BITMASK;

  pdu->llc.length += sizeof(pdu->isdp_capabilities);

  sysapiRegistryGet(MAINT_LEVEL, STR_ENTRY, (void*) pdu->isdp_version);
  pdu->llc.length += strlen(pdu->isdp_version);

  sysapiRegistryGet(MACHINE_MODEL, STR_ENTRY, (void*) pdu->isdp_platform);
  pdu->llc.length += strlen(pdu->isdp_platform);

  pdu->llc.length += 6 * sizeof(isdpTlvTypeLength_t) + addToLlcLength;

  isdpDebugTrace(ISDP_DBG_FLAG_PduGroup,
                   "%s: LINE %d: llc.length = %d, interface %d\n",
                   __FUNCTION__, __LINE__, pdu->llc.length, intIfNum);

  return rc;
}

L7_RC_t isdpUnitTestSend(L7_uint32 intIfNum, L7_uint32 number, L7_short16 etype,
                         L7_char8 version, L7_char8 holdtime, L7_uint32 addToLlcLength)
{
  L7_uint32 i = 0;
  isdpEntryKey_t key;
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 sentNumber = 0;
  L7_uchar8 deviceId[L7_ISDP_DEVICE_ID_LEN];
  L7_netBufHandle bufHandle = L7_NULL;

  memset((void *)&key, 0x00, sizeof(isdpEntryKey_t));

  sysapiPrintf("%s: Sending %d isdp packets\n", __FUNCTION__, number);

  for(i = 1; i < number + 1; i++)
  {
    osapiSnprintf(deviceId, L7_ISDP_DEVICE_ID_LEN, "%d", i);

    rc = isdpUnitTestPacketCreate(intIfNum, deviceId, holdtime, etype,
                                  version, addToLlcLength, &bufHandle);

    rc = isdpPduTransmit(bufHandle, intIfNum);
    if(rc != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    sentNumber++;
  }

  sysapiPrintf("%s: succeded\n", __FUNCTION__);
  return L7_SUCCESS;
}
