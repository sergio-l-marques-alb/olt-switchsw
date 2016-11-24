/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
* @filename  isdp_control.c
*
* @purpose   isdp Control file
*
* @component isdp
*
* @comments
*
* @create    9/11/2007
*
* @author    dgaryachy
*
* @end
*
**********************************************************************/

#include "isdp_include.h"
#include "buff_api.h"

extern void               *isdpTaskSyncSema;
extern void               *isdpProcessQueue;
extern void               *isdpPduQueue;
extern isdpIntfInfoData_t *isdpIntfTbl;
extern osapiTimerDescr_t  *isdpBaseTimer;
extern isdpCfg_t          *isdpCfg;

static L7_uint32          isdpSendPacketTimer = 0;

/*********************************************************************
* @purpose  isdp task which serves the request queue
*
* @param
*
* @returns  void
*
* @comments User-interface writes and LACPDUs are serviced off
*           of the isdp_queue
*
* @end
*********************************************************************/
void isdpTask()
{
  isdpMsg_t msg;
  L7_RC_t   rc = L7_SUCCESS;

  memset(&msg, 0x00, sizeof(isdpMsg_t));

  rc = osapiTaskInitDone(L7_ISDP_TASK_SYNC);

  for (;;)
  {
     /* Since we are reading from multiple queues, we cannot wait forever
     * on the message receive from each queue. Rather than sleep between
     * queue reads, use a semaphore to indicate whether any queue has
     * data. Wait until data is available. */

    if (osapiSemaTake(isdpTaskSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
        L7_LOG(L7_LOG_SEVERITY_ERROR, L7_ISDP_COMPONENT_ID,
               "Failure taking ISDP message queue semaphore.");
        continue;
    }

    memset(&msg, 0x00, sizeof(isdpMsg_t));

    /* Always try to read from the main ISDP queue first. */
    rc = osapiMessageReceive(isdpProcessQueue, &msg,
                             sizeof(isdpMsg_t), L7_NO_WAIT);

    if (rc != L7_SUCCESS)
    {
        /* The main queue is empty. See if a pdu event is queued. */
        rc = osapiMessageReceive(isdpPduQueue, &msg,
                                 sizeof(isdpMsg_t), L7_NO_WAIT);
    }

    if (rc == L7_SUCCESS)
    {
      (void)isdpDispatchCmd(msg);
    }
    else
    {
      LOG_MSG("isdp message recv failed: event %d, intIfNum %d\n",
              msg.intIfNum, msg.event);
    }
  }
}

/*********************************************************************
* @purpose  Route the event to a handling function and grab the parms
*
* @param    msg  @b{(input)} isdp message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
void isdpDispatchCmd(isdpMsg_t msg)
{
  isdpDebugEventTrace(msg);

  switch (msg.event)
  {
  case ISDP_INTF_EVENT:
    (void) isdpIntfChangeProcess(msg.intIfNum, msg.data.status);
  break;

  case ISDP_PDU_EVENT:
    (void)isdpPduReceiveProcess(msg.intIfNum, msg.data.bufHandle);
  break;

  case ISDP_TIMER_EVENT:
    (void)isdpTimerProcess();
  break;

  case ISDP_CNFGR_EVENT:
    isdpCnfgrParse(&msg.data.CmdData);
  break;

  default:
    LOG_MSG("Unrecognized event %d\n", msg.event);

  }/*end switch (msg.event)*/
}

/*********************************************************************
* @purpose  Put message to process queue
*
* @param    msg  @b{(input)} isdp message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t isdpProcessMsgQueue(isdpMsg_t msg)
{
  L7_RC_t rc = L7_SUCCESS;

  if (isdpProcessQueue == L7_NULLPTR)
  {
    LOG_MSG("isdpProcessQueue is not initialized\n");
    return L7_FAILURE;
  }

  rc = osapiMessageSend(isdpProcessQueue, &msg, sizeof(isdpMsg_t),
      L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM);

  if(rc != L7_SUCCESS)
  {
    LOG_MSG("osapiMessageSend failed\n");
    return rc;
  }

  rc = osapiSemaGive(isdpTaskSyncSema);
  if(rc != L7_SUCCESS)
  {
    LOG_MSG("Failed to give msgQueue semaphore\n");
  }

  return rc;
}

/*********************************************************************
* @purpose  Put message to pdu queue
*
* @param    msg  @b{(input)} isdp message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t isdpPduReceiveQueue( L7_uint32 intIfNum, L7_netBufHandle bufHandle)
{
  L7_RC_t   rc = L7_SUCCESS;
  isdpMsg_t msg;

  if (isdpPduQueue == L7_NULLPTR)
  {
    LOG_MSG("isdpPduQueue is not initialized\n");
    return L7_FAILURE;
  }

  memset( (void *)&msg, 0, sizeof(isdpMsg_t) );

  msg.event = ISDP_PDU_EVENT;
  msg.intIfNum = intIfNum;
  msg.data.bufHandle = bufHandle;

  rc = osapiMessageSend( isdpPduQueue, &msg, sizeof(isdpMsg_t),
      L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM);

  if(rc != L7_SUCCESS)
  {
    LOG_MSG("osapiMessageSend failed\n");
    return rc;
  }

  rc = osapiSemaGive(isdpTaskSyncSema);
  if(rc != L7_SUCCESS)
  {
    LOG_MSG("Failed to give msgQueue semaphore\n");
  }

  return rc;
}

/*********************************************************************
* @purpose  This is the routine which receives the PDU and passes it to the classifier
*      with the relevant event for the prx state machine.
*
* @param     intIfNum   @b{(input)} Interface that this PDU was received on
* @param    bufHandle  @b{(input)} buffer handle to the PDU
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments It also queues up a event for the bdm state machine for bdmpduRcvd
*           This routine is responsible for freing up the buffer pool before
*      any possible return path.
*
* @end
*********************************************************************/
L7_RC_t isdpPduReceiveProcess(L7_uint32 intIfNum, L7_netBufHandle bufHandle)
{
  L7_RC_t           rc         = L7_SUCCESS;
  L7_uchar8         *data      = L7_NULLPTR;
  L7_uint32         length     = 0;
  L7_uint32         offset     = 0;
  isdpIntfCfgData_t *pCfg      = L7_NULLPTR;
  isdpPacket_t      isdpPacket;
  isdpEntry_t       isdpEntry;

  memset(&isdpEntry, 0x00, sizeof(isdpEntry_t));
  memset(&isdpPacket, 0x00, sizeof(isdpPacket_t));

  if (isdpIsValidIntf(intIfNum) != L7_TRUE)
  {
    SYSAPI_NET_MBUF_FREE(bufHandle);
    LOG_MSG("Interface %d isn't valid\n", intIfNum);
    return L7_FAILURE;
  }

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, length);
  offset = sysNetDataOffsetGet(data);

  memcpy(isdpPacket.ether.src.addr, data + L7_MAC_ADDR_LEN, L7_MAC_ADDR_LEN);
  memcpy(&isdpPacket.llc.length, data + sizeof(L7_enetHeader_t), sizeof(L7_ushort16));

  if(isdpPacket.llc.length + sizeof(isdpPacket.llc.length) -
     sizeof(L7_802_encaps_t) != length - offset)
  {
    SYSAPI_NET_MBUF_FREE(bufHandle);
    isdpTrafficInvalidFormatPduReceivedInc(intIfNum);
    return L7_FAILURE;
  }

  rc = isdpDataPduRead(intIfNum, data + offset, length - offset, &isdpPacket);
  SYSAPI_NET_MBUF_FREE(bufHandle);

  if(rc != L7_SUCCESS)
  {
    isdpIpAddrListRelease(&isdpPacket.ipAddressList);
    return rc;
  }

  if(isdpPacket.isdp_header.version == ISDP_VERSION_1)
  {
    if(isdpIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
    {
      isdpIntfTbl[intIfNum].version1Received = L7_TRUE;
    }
    isdpTrafficV1PduReceivedInc(intIfNum);
  }
  else if(isdpPacket.isdp_header.version == ISDP_VERSION_2)
  {
    isdpTrafficV2PduReceivedInc(intIfNum);
  }

  isdpTrafficPduReceivedInc(intIfNum);
  isdpDebugPacketRxTrace(intIfNum, &isdpPacket);

  rc = isdpFillEntry(&isdpEntry, &isdpPacket, intIfNum);

  if(rc != L7_SUCCESS)
  {
    LOG_MSG("isdpFillEntry failed: intIfNum %d\n", intIfNum);
    isdpIpAddrListRelease(&isdpPacket.ipAddressList);
    return rc;
  }

  rc = isdpEntryAdd(&isdpEntry);
  if(rc != L7_SUCCESS)
  {
    LOG_MSG("isdpEntryAdd failed: intIfNum %d\n", intIfNum);
    isdpIpAddrListRelease(&isdpEntry.ipAddressList);
  }

  return rc;
}

/*********************************************************************
* @purpose  Get the address from tlv and insert to address list
*
* @param    *ipAddressList @b((input)) ip address list
* @param    address_tlv    @b((input)) address tlv
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
static L7_RC_t isdpParseAddressTlv(isdpIpAddressBuffer_t **pIpAddressList,
                                   isdpTlvAddressByte_t *address_tlv)
{
  L7_RC_t   rc    = L7_SUCCESS;
  L7_uint32 temp  = 0;

  memcpy(&temp, address_tlv->address, sizeof(L7_uint32));
  rc = isdpIpAddrPutList(pIpAddressList, osapiNtohl(temp));

  return rc;
}

/*********************************************************************
* @purpose  Parse buffer with Tlvs
*
* @param    tlv_type     @b((input)) device Id
* @param    value_length     @b((input)) device Id
* @param    *deviceId     @b((input)) device Id
* @param    address_tlv   @b((input)) address tlv
*
* @returns  void
*
* @end
*********************************************************************/
static void isdpParseTlv(L7_ushort16 tlv_type, L7_ushort16 value_length,
                            isdpPacket_t* isdpPacket, char* value)
{
  L7_uint32 temp           = 0;
  L7_uint32 i              = 0;
  L7_uint32 strLength      = 0;
  L7_uint32 addressNumber  = 0;
  L7_RC_t   rcAddr         = L7_SUCCESS;

  switch (tlv_type)
  {
  case ISDP_DEVICE_ID_TLV_TYPE:
    strLength = value_length < L7_ISDP_DEVICE_ID_LEN ? value_length + 1 : L7_ISDP_DEVICE_ID_LEN;
    osapiStrncpySafe(isdpPacket->isdp_device_id, value, strLength);
    break;

  case ISDP_ADDRESS_TLV_TYPE:
    memcpy(&addressNumber,value,sizeof(addressNumber)); 
    addressNumber = osapiNtohl(addressNumber);
    value += sizeof(L7_uint32);

    for(i = 0; i < addressNumber; i++)
    {
      rcAddr = isdpParseAddressTlv(&isdpPacket->ipAddressList, (isdpTlvAddressByte_t*) value);
      if(rcAddr == L7_SUCCESS)
      {
        isdpPacket->isdp_address_number++;
      }

      value += sizeof(isdpTlvAddressByte_t);
    }
    break;

  case ISDP_PORT_ID_TLV_TYPE:
    strLength = value_length < L7_ISDP_PORT_ID_LEN ? value_length + 1 : L7_ISDP_PORT_ID_LEN;
    osapiStrncpySafe(isdpPacket->isdp_port_id, value, strLength);
    break;

  case ISDP_CAPABILITIES_TLV_TYPE:
    memcpy(&temp,value,sizeof(temp)); 
    isdpPacket->isdp_capabilities =  osapiNtohl(temp);
    break;

  case ISDP_VERSION_TLV_TYPE:
    strLength = value_length < L7_ISDP_VERSION_LEN ? value_length + 1 : L7_ISDP_VERSION_LEN;
    osapiStrncpySafe(isdpPacket->isdp_version, value, strLength);
    break;

  case ISDP_PLATFORM_TLV_TYPE:
    strLength = value_length < L7_ISDP_PLATFORM_LEN ? value_length + 1 : L7_ISDP_PLATFORM_LEN;
    osapiStrncpySafe(isdpPacket->isdp_platform, value, strLength);
    break;

  default:
    isdpDebugTrace(ISDP_DBG_FLAG_PduGroup,
                   "%s: LINE %d: not supported TLV type %d\n",
                   __FUNCTION__, __LINE__, tlv_type);
    break;
  }
}

/*********************************************************************
* @purpose  Parse a received PDU and
*
* @param    intIfNum    @b((input))  interface number
* @param    *data       @b((input))  pointer to the pdu received
* @param    length      @b((input))  length of pdu
* @param    *isdpPacket @b((output)) isdp packet
*
* @returns  L7_SUCCESS, PDU parsed and stored successfully
* @returns  L7_FAILURE, error parsing PDU
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t isdpDataPduRead(L7_uint32 intIfNum, L7_uchar8 *data, L7_uint32 length,
                        isdpPacket_t* isdpPacket)
{
  L7_RC_t                     rc               = L7_SUCCESS;
  isdpHeader_t                *pduHeader       = L7_NULLPTR;
  L7_ushort16                 tlv_type         = 0;
  L7_ushort16                 tlv_length       = 0;
  L7_uchar8                   *data_ptr        = L7_NULLPTR;
  L7_ushort16                 checksum         = 0;
  L7_ushort16                 checkV2Sum       = 0;

  if(data == L7_NULLPTR || isdpPacket == L7_NULLPTR)
  {
    LOG_MSG("Null pointers as parameters! \n");
    return L7_FAILURE;
  }

  pduHeader = (isdpHeader_t*) data;
  data_ptr = data;

  isdpPacket->isdp_header.checksum = pduHeader->checksum;
  pduHeader->checksum = 0;
  isdpChecksum(data, length, &checksum);
  isdpV2CheckSum(data, length, &checkV2Sum);
  if( (checksum   != isdpPacket->isdp_header.checksum) && 
      (checkV2Sum != isdpPacket->isdp_header.checksum) ) 
    
  {
    isdpTrafficChkSumErrorPduReceivedInc(intIfNum);
    return L7_FAILURE;
  }

  isdpPacket->isdp_header.version = pduHeader->version;
  if(isdpPacket->isdp_header.version != ISDP_VERSION_1 &&
     isdpPacket->isdp_header.version != ISDP_VERSION_2)
  {
    isdpTrafficBadHeaderPduReceivedInc(intIfNum);
    return L7_FAILURE;
  }

  isdpPacket->isdp_header.holdTime = pduHeader->holdTime;

  data_ptr += sizeof(isdpHeader_t);

  while (data_ptr < data + length)
  {
    memcpy(&tlv_type,data_ptr, sizeof(tlv_type));
    tlv_type = osapiNtohs(tlv_type);
    memcpy(&tlv_length,(data_ptr+sizeof(tlv_type)), sizeof(tlv_length));
    tlv_length = osapiNtohs(tlv_length);


    if( (tlv_length < sizeof(isdpTlvTypeLength_t)) ||
        (tlv_length + data_ptr > data + length) )
    {
      isdpTrafficInvalidFormatPduReceivedInc(intIfNum);
      return L7_FAILURE;
    }

    isdpParseTlv(tlv_type, tlv_length - sizeof(isdpTlvTypeLength_t),
                 isdpPacket, data_ptr + sizeof(isdpTlvTypeLength_t));

    data_ptr += tlv_length;
  }

  return rc;
}

/**************************************************************************
*
* @purpose    actions to be performed when the periodic timer expires
*
* @param    arg1    @b((input))  first argument
* @param    arg2    @b((input))  second argument
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @end
*
*************************************************************************/
L7_RC_t isdpTimerExpired(L7_uint32 arg1, L7_uint32 arg2)
{
  L7_RC_t   rc = L7_SUCCESS;
  isdpMsg_t msg;

  osapiTimerAdd((void*)isdpTimerExpired, 0, 0, ISDP_BASE_TIMEOUT, &isdpBaseTimer);

  memset(&msg, 0, sizeof(isdpMsg_t));

  msg.event = ISDP_TIMER_EVENT;
  rc = isdpProcessMsgQueue(msg);

  return rc;
}

/**************************************************************************
* @purpose  Send isdp PDUs.
*
*
* @returns  L7_SUCCESS
*
*
* @end
*************************************************************************/
L7_RC_t isdpPduSend(void)
{
  L7_RC_t           rcNim    = L7_SUCCESS;
  L7_uint32         intIfNum = 0;
  isdpIntfCfgData_t *pCfg    = L7_NULLPTR;

  if (isdpCfg->cfg.globalCfgData.mode == L7_DISABLE)
  {
    LOG_MSG("Isdp is disabled, timer should be deactivated! \n");
    return L7_FAILURE;
  }

  rcNim = nimFirstValidIntfNumber(&intIfNum);

  while (rcNim == L7_SUCCESS)
  {
    if (isdpIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    {
      rcNim = nimNextValidIntfNumber(intIfNum, &intIfNum);
      continue;
    }

    if (isdpIntfTbl[intIfNum].active == L7_FALSE ||
        isdpIntfTbl[intIfNum].intfAcquired == L7_TRUE)
    {
      rcNim = nimNextValidIntfNumber(intIfNum, &intIfNum);
      continue;
    }

    if (isdpCfg->cfg.intfCfgData[intIfNum].mode == L7_ENABLE)
    {
      if(isdpCfg->cfg.globalCfgData.v2Mode == L7_ENABLE)
      {
        isdpTcnTx(intIfNum, ISDP_VERSION_2);
        if(isdpIntfTbl[intIfNum].version1Received == L7_TRUE)
        {
          isdpTcnTx(intIfNum, ISDP_VERSION_1);
        }
      }
      else
      {
         isdpTcnTx(intIfNum, ISDP_VERSION_1);
      }
    }

    rcNim = nimNextValidIntfNumber(intIfNum, &intIfNum);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process timer event
*
*
* @returns  void
*
* @notes    This function is called every timer interval
*
* @end
*********************************************************************/
L7_RC_t isdpTimerProcess(void)
{
  isdpEntryKey_t key;
  isdpEntry_t    *pEntry = L7_NULLPTR;

  memset(&key, 0, sizeof(key));

  pEntry = isdpEntryGetNext(key);

  while(pEntry != L7_NULLPTR)
  {
    if(pEntry->holdTime > 1)
    {
      pEntry->holdTime--;
      pEntry = isdpEntryGetNext(pEntry->key);
    }
    else
    {
      isdpEntryDelete(pEntry->key);
      pEntry = isdpEntryGetNext(key);
    }
  }

  if(isdpSendPacketTimer > isdpCfg->cfg.globalCfgData.timer - 1)
  {
    isdpPduSend();
    isdpSendPacketTimer = 0;
  }
  else
  {
    isdpSendPacketTimer = isdpSendPacketTimer + 1;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Apply isdp config data
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t isdpCtlApplyConfigData(void)
{
  if (isdpCfg->cfg.globalCfgData.mode == L7_DISABLE)
  {
    return L7_SUCCESS;
  }

  return isdpModeApply(isdpCfg->cfg.globalCfgData.mode);
}

