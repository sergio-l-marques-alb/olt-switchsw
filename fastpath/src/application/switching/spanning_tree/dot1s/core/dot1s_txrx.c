/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  dot1s_txrx.c
*
* @purpose   Multiple Spanning tree Transmit and Receive BPDUs
*
* @component dot1s
*
* @comments
*
* @create    10/14/2002
*
* @author    skalyanam
*
* @end
*
**********************************************************************/
#include "dot1s_include.h"
#include "buff_api.h"
#include "dot1s_helper.h"
#include "dot1s_transport.h"
#include "log.h"

extern  DOT1S_BRIDGE_t              *dot1sBridge;
static  L7_uchar8 temp[sizeof (DOT1S_MSTP_ENCAPS_t)];
static const L7_uchar8 dot1sBpduMulticastMacAddr[L7_MAC_ADDR_LEN] =
{0x01, 0x80, 0xC2, 0x00, 0x00, 0x00};
extern dot1sDeregister_t dot1sDeregister;
extern dot1sCnfgrState_t dot1sCnfgrState;
L7_uint32 dot1sBPDUBufferPoolId = 0;


/*********************************************************************
* @purpose  Transmit a BPDU (STP,RSTP or MSTP)
*
*
* @param    paddedBufHandle @b{(input)} handle to the packet buffer
* @param    intIfNum        @b{(input)} interface through which to transmit
*
* @returns  L7_SUCCESS  on a successful tranmission
* @returns  L7_FAILURE  for any error
*
* @comments This routine just transmits the packet be it STP RSTP or MSTP.
*           Appropriate counters are incremented.
*
* @end
*********************************************************************/
L7_RC_t dot1sBpduTransmit(L7_netBufHandle paddedBufHandle,
                          L7_uint32 intIfNum)
{

  L7_netBufHandle bufHandle;
  L7_RC_t rc;
  DTL_CMD_TX_INFO_t dtlCmdInfo;
  DOT1S_BYTE_MSTP_ENCAPS_t *pdu;
  L7_uchar8 *data;
  L7_uint32 pduLen;
  L7_uint32 portIndex = dot1sPortIndexFromIntfNumGet(intIfNum);

  bufHandle =  dot1sTxRxNetworkConvert(paddedBufHandle);
  if (bufHandle == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DOT1S_COMPONENT_ID,
           "dot1sBpduTransmit(): could not get a buffer."
           " Out of system buffers");
    return L7_FAILURE;
  }

  DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_HELPER_DETAIL,
          "%s: Opportunity to transmit from intf %d \n", __FUNCTION__, intIfNum);
  /* send it back to DTL for transmit
   * assuming FCS is redone as a lower layer
   */
  if (dot1sHelperFuncTable.dot1sNotifyHelperPduStart)
  {
    SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
    pdu = (DOT1S_BYTE_MSTP_ENCAPS_t *)data;
    SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle,pduLen);
    rc = dot1sHelperFuncTable.dot1sNotifyHelperPduStart(pdu, intIfNum, pduLen);
    SYSAPI_NET_MBUF_FREE(bufHandle);
  }
  else
  {
  bzero((char *)&dtlCmdInfo,(L7_int32)sizeof(DTL_CMD_TX_INFO_t));
  dtlCmdInfo.intfNum= intIfNum;
  dtlCmdInfo.priority = 1;
  dtlCmdInfo.typeToSend = DTL_L2RAW_UNICAST;
  rc = dtlPduTransmit( bufHandle, DTL_CMD_TX_L2, &dtlCmdInfo);
  }
  if (rc != L7_SUCCESS)
  {
    /*increment txdiscards counter*/
    dot1sPortStats[portIndex].txDiscards++;
  }
  return rc;

}
/*********************************************************************
* @purpose  This routine receives any BPDU (STP, RSTP or MSTP) and sends
*           it to the dot1s msg Q and task
*
* @param    bufHandle    @b{(input)} handle to the network buffer the BPDU is stored in
* @param    bpduInfo     @b{(input)} pointer to sysnet structure which contains the internal
*                                     interface number that the BPDU was received on
*
* @returns  L7_SUCCESS  on a successful operation
* @returns  L7_FAILURE  for any error
*
* @comments This routine is registered with sysnet for Des MAC address of
*           01:80:C2:00:00:00 at dot1s init time.
*
* @end
*********************************************************************/
L7_RC_t dot1sBpduReceive(L7_netBufHandle bufHandle,
                         sysnet_pdu_info_t *bpduInfo)
{
  L7_uchar8         *data;
  L7_RC_t           rc = L7_FAILURE;
  L7_uint32         intIfNum;
  L7_INTF_TYPES_t       sysIntfType;
  L7_uchar8         *buff= L7_NULLPTR;
  L7_uint32 portIndex;


  if (dot1sDeregister.dot1sSysnetBpduRegister == L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
            "dot1sBpduReceive is deregistered\n");
    return L7_FAILURE;
  }
  if (!(DOT1S_IS_READY))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
            "Received a vlan callback while outside the EXECUTE state");
    return L7_FAILURE;
  }

  if (DOT1S_DEBUG_COMMON(DOT1S_DEBUG_RCVD_BPDU))
  {

    DOT1S_DEBUG_MSG("%s: Port %d Received BPDU at %d (msec) \n",__FUNCTION__,
                    bpduInfo->intIfNum,osapiUpTimeMillisecondsGet());
  }


  intIfNum = bpduInfo->intIfNum;
  rc = nimGetIntfType(intIfNum, &sysIntfType);


  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  /*check if this PDU is meant for dot1s by checking the DA*/

  if (memcmp(data,dot1sBpduMulticastMacAddr,L7_MAC_ADDR_LEN) == 0 &&
      (sysIntfType == L7_PHYSICAL_INTF || sysIntfType == L7_LAG_INTF))
  {
      portIndex = dot1sPortIndexFromIntfNumGet(intIfNum);
    /*remove any header tagging*/
   rc = dot1sTxRxHeaderTagRemove(bufHandle);

    /*Accomodate for Endianess*/
    rc = dot1sTxRxHostConvert(bufHandle, &buff);

    dot1sDebugPacketRxTrace(intIfNum,(DOT1S_MSTP_ENCAPS_t *)buff);

    if (rc == L7_SUCCESS)
    {
      rc = dot1sIssueCmd(dot1sBpduReceived, bpduInfo->intIfNum, L7_NULL, buff);
      if (L7_SUCCESS != rc)
      {
          /*free the buffer*/
        bufferPoolFree(dot1sBPDUBufferPoolId, buff);
        dot1sPortStats[portIndex].rxDiscards++;
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
                "dot1sBpduReceive(): Unable to put BPDU on Msg Queue\n");
      }
    }
    else
    {
      dot1sPortStats[portIndex].rxDiscards++;
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
              "dot1sBpduReceive(): cannot get buffer from buff pool\n");
    }
    /* We need to return success always to tell the sysnet we consumed the BPDU */
    return L7_SUCCESS;
  }
  return L7_FAILURE;

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
L7_RC_t dot1sTxRxInit(void)
{
  L7_RC_t rc;
  sysnetNotifyEntry_t snEntry;

  /*Register the pdu receive function with sysnet utility*/
  strcpy(snEntry.funcName, "dot1sPduReceive");
  snEntry.notify_pdu_receive = dot1sBpduReceive;
  snEntry.type = SYSNET_MAC_ENTRY;
  memcpy(snEntry.u.macAddr, dot1sBpduMulticastMacAddr, L7_MAC_ADDR_LEN);
  rc = sysNetRegisterPduReceive(&snEntry);
  return rc;
}
/*********************************************************************
* @purpose  Transmits a TCN BPDU
*
* @param    p   @b{(input)}   pointer to port entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sTcnTx(DOT1S_PORT_COMMON_t *p)
{
  DOT1S_MSTP_ENCAPS_t   *pdu;
  L7_uchar8             *pduStart;
  L7_netBufHandle       bufHandle;
  L7_RC_t rc;

  /* if the txCount is greater than the bridgeTxHoldCount
   * return without transmitting
   */
  if (p->txCount > dot1sBridge->TxHoldCount)
  {
    return L7_SUCCESS;
  }

  SYSAPI_NET_MBUF_GET(bufHandle);
  if (bufHandle == L7_NULL)
  {
    return L7_FAILURE;
  }
  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, pduStart);
  pdu = (DOT1S_MSTP_ENCAPS_t *) pduStart;
  memset((void *)pdu, 0x00, sizeof(*pdu));

  memcpy(pdu->destMac, dot1sBpduMulticastMacAddr, L7_MAC_ADDR_LEN);
  rc = dot1sIhIntfAddrGet(p->portNum,L7_NULL,pdu->srcMac);


  pdu->packetLength = 0x07;
  pdu->logicalLinkCtrl[0] = 0x42;
  pdu->logicalLinkCtrl[1] = 0x42;
  pdu->logicalLinkCtrl[2] = 0x03;

  pdu->protocolIdentifier = 0x0000;
  pdu->bdpuType = 0x80;
  pdu->protocolVersion = 0x00;

  /*call dot1sTransmitBPDU*/
  dot1sDebugPacketTxTrace(p->portNum,pdu);
  rc = dot1sBpduTransmit(bufHandle, p->portNum);

  return rc;
}

/*********************************************************************
* @purpose  Transmits a configuration BPDU
*
* @param    p   @b{(input)}   pointer to port entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Using the designatedPortPriority vector as opposed to the
*           port priority vector. IEEE 802.1D draft 4. Eventually this
*           change will make it the IEEE 802.1s
*
* @end
*********************************************************************/
L7_RC_t dot1sConfigTx(DOT1S_PORT_COMMON_t *p)
{
  DOT1S_MSTP_ENCAPS_t   *pdu;
  L7_uchar8             *pduStart;
  L7_netBufHandle       bufHandle;
  L7_RC_t rc;
  L7_uint32 portIndex = dot1sPortIndexFromIntfNumGet(p->portNum);

  /* if the txCount is greater than the bridgeTxHoldCount
   * return without transmitting
   */
  if (p->txCount > dot1sBridge->TxHoldCount)
  {
    return L7_SUCCESS;
  }

  SYSAPI_NET_MBUF_GET(bufHandle);
  if (bufHandle == L7_NULL)
  {
    return L7_FAILURE;
  }

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, pduStart);
  pdu = (DOT1S_MSTP_ENCAPS_t *) pduStart;
  memset((void *)pdu, 0x00, sizeof(*pdu));

  memcpy(pdu->destMac, dot1sBpduMulticastMacAddr, L7_MAC_ADDR_LEN);
  rc = dot1sIhIntfAddrGet(p->portNum,L7_NULL,pdu->srcMac);


  pdu->packetLength = 0x26;
  pdu->logicalLinkCtrl[0] = 0x42;
  pdu->logicalLinkCtrl[1] = 0x42;
  pdu->logicalLinkCtrl[2] = 0x03;

  pdu->protocolIdentifier = 0x0000;
  pdu->bdpuType = 0x00;
  pdu->protocolVersion = 0x00;

  pdu->cistRootId.priInstId = p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedPriority.rootId.priInstId;
  memcpy((char *)&pdu->cistRootId.macAddr,
         (char *)&p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedPriority.rootId.macAddr,
         L7_MAC_ADDR_LEN);

  pdu->cistExtRootPathCost =  p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedPriority.extRootPathCost;

  pdu->cistRegRootId.priInstId = dot1sInstance->cist.BridgeIdentifier.priInstId;
  memcpy((char *)&pdu->cistRegRootId.macAddr,
         (char *)&dot1sInstance->cist.BridgeIdentifier.macAddr,
         L7_MAC_ADDR_LEN);

  pdu->cistPortId = p->portInstInfo[DOT1S_CIST_INDEX].portId;

  pdu->msgAge = (L7_ushort16)p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedTimes.msgAge;
  pdu->maxAge = (L7_ushort16)p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedTimes.maxAge;
  pdu->fwdDelay = (L7_ushort16)p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedTimes.fwdDelay;
  pdu->helloTime = (L7_ushort16)p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedTimes.helloTime;

  if (p->portInstInfo[DOT1S_CIST_INDEX].tcWhile != 0)
  {
    pdu->cistFlags |= DOT1S_TC_FLAG_MASK;
  }
  if (p->tcAck == L7_TRUE)
  {
    pdu->cistFlags |= DOT1S_TCNACK_FLAG_MASK;
  }
  else
  {
    pdu->cistFlags &= ~DOT1S_TCNACK_FLAG_MASK;
  }



  /*call dot1sTransmitBPDU*/
  rc = dot1sBpduTransmit(bufHandle, p->portNum);
  if (rc == L7_SUCCESS)
  {
    /*increment txstp counter*/
    dot1sPortStats[portIndex].stpTx++;
  }
  return rc;
}
/*********************************************************************
* @purpose  Transmits a MST BPDU
*
* @param    p    @b{(input)}   pointer to port entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Using the designatedPortPriority vector as opposed to the
*           port priority vector. IEEE 802.1D draft 4. Eventually this
*           change will make it the IEEE 802.1s
*
* @end
*********************************************************************/
L7_RC_t dot1sMstpTx(DOT1S_PORT_COMMON_t *p)
{
  DOT1S_MSTP_ENCAPS_t   *pdu;
  L7_uchar8             *pduStart;
  L7_netBufHandle       bufHandle;
  DOT1S_BRIDGE_t        *bridge;
  L7_uchar8             mask;
  L7_uint32             i, msg = 0;
  L7_RC_t rc;
  L7_uint32 portIndex = dot1sPortIndexFromIntfNumGet(p->portNum);

  /* if the txCount is greater than the bridgeTxHoldCount
   * return without transmitting
   */
  if (p->txCount > dot1sBridge->TxHoldCount)
  {
    return L7_SUCCESS;
  }

  bridge = dot1sBridgeParmsFind();
  if (bridge == L7_NULLPTR)
  {
    return L7_FAILURE;
  }


  SYSAPI_NET_MBUF_GET(bufHandle);
  if (bufHandle == L7_NULL)
  {
    return L7_FAILURE;
  }
  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, pduStart);
  pdu = (DOT1S_MSTP_ENCAPS_t *) pduStart;
  memset((void *)pdu, 0x00, sizeof(*pdu));

  memcpy(pdu->destMac, dot1sBpduMulticastMacAddr, L7_MAC_ADDR_LEN);
  rc = dot1sIhIntfAddrGet(p->portNum,L7_NULL,pdu->srcMac);


  pdu->logicalLinkCtrl[0] = 0x42;
  pdu->logicalLinkCtrl[1] = 0x42;
  pdu->logicalLinkCtrl[2] = 0x03;
  pdu->protocolIdentifier = 0x0000;
  pdu->bdpuType = 0x02;



  pdu->cistRootId.priInstId = p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedPriority.rootId.priInstId;
  memcpy((char *)&pdu->cistRootId.macAddr,
         (char *)&p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedPriority.rootId.macAddr,
         L7_MAC_ADDR_LEN);

  pdu->cistExtRootPathCost =  p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedPriority.extRootPathCost;

  pdu->cistRegRootId.priInstId = p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedPriority.regRootId.priInstId;
  memcpy((char *)&pdu->cistRegRootId.macAddr,
         (char *)&p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedPriority.regRootId.macAddr,
         L7_MAC_ADDR_LEN);

  pdu->cistPortId = p->portInstInfo[DOT1S_CIST_INDEX].portId;

  pdu->msgAge = (L7_ushort16)p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedTimes.msgAge;
  pdu->maxAge = (L7_ushort16)p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedTimes.maxAge;
  pdu->fwdDelay = (L7_ushort16)p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedTimes.fwdDelay;
  pdu->helloTime = (L7_ushort16)p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedTimes.helloTime;

  pdu->ver1Len = 0x00;

  pdu->cistFlags = 0x00;

  switch (p->portInstInfo[DOT1S_CIST_INDEX].role)
  {
    case ROLE_MASTER:
      mask = DOT1S_PORT_ROLE_MASTER_MASK;
      break;

    case ROLE_ROOT:
      mask = DOT1S_PORT_ROLE_ROOT_MASK;
      break;

    case ROLE_DESIGNATED:
      mask = DOT1S_PORT_ROLE_DESIG_MASK;
      break;

    case ROLE_ALTERNATE:
    case ROLE_BACKUP:
      mask = DOT1S_PORT_ROLE_ALT_BCK_MASK;
      break;

    case ROLE_DISABLED:
    default:
      /* the cist port role can either be one of the above only
       * free the mbuf and return failure
       */
      SYSAPI_NET_MBUF_FREE(bufHandle);
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
              "dot1sMstpTx(): CIST Role Disabled.Port(%d) Inst(%d)\n",p->portNum,DOT1S_CIST_INDEX);
      return L7_FAILURE;
      break;

  }/*end switch (p->portInstInfo[DOT1S_CIST_INDEX].role)*/


  pdu->cistFlags |= mask;

  /* The txMstp() (IEEE 802.1Q-2003 13.26.22) procedure requires the agreement flag
     value to be sent as 'agreed'. However the description of agree variable
     (IEEE 802.1Q-2003 13.24.1 & 13.24.2)  and in the explanation of Proposal
     -Agreement protocolthe spec suggests the use of 'agree' flag for the same.
     Based on email communque from IEEE 802.1s body and logical deuduction,
     choosing the use of agree flag.
  */

  if (p->portInstInfo[DOT1S_CIST_INDEX].agree == L7_TRUE)
  {
    pdu->cistFlags |= DOT1S_AGREEMENT_FLAG_MASK;
  }
  if (p->portInstInfo[DOT1S_CIST_INDEX].proposing == L7_TRUE)
  {
    pdu->cistFlags |= DOT1S_PROPOSAL_FLAG_MASK;
  }
  if (p->portInstInfo[DOT1S_CIST_INDEX].tcWhile != 0)
  {
    pdu->cistFlags |= DOT1S_TC_FLAG_MASK;
  }
  if (p->portInstInfo[DOT1S_CIST_INDEX].learning == L7_TRUE)
  {
    pdu->cistFlags |= DOT1S_LEARNING_FLAG_MASK;
  }
  if (p->portInstInfo[DOT1S_CIST_INDEX].forwarding == L7_TRUE)
  {
    pdu->cistFlags |= DOT1S_FORWARDING_FLAG_MASK;
  }



  if (bridge->ForceVersion < 3)
  {
    pdu->packetLength = 0x27;
    pdu->protocolVersion = 0x02;

    /*call dot1sTransmitBPDU*/
    dot1sDebugPacketTxTrace(p->portNum,pdu);
    rc = dot1sBpduTransmit(bufHandle, p->portNum);
    if (rc == L7_SUCCESS)
    {
      /*increment txrstp counter*/
      dot1sPortStats[portIndex].rstpTx++;
    }
  }
  else
  {
    pdu->packetLength = 0x27+0x42;/*for every msti msg add 0x10*/
    pdu->protocolVersion = 0x03;

    pdu->ver3Len = 64;/*for every msti msg add 16*/

    /*fill the mst parms here */
    memcpy((char *)&pdu->mstConfigId,
           (char *)&bridge->MstConfigId.formatSelector,
           DOT1S_BPDU_CONFIG_ID_LEN);

    /*pdu->cistIntRootPathCost = dot1sInstance->cist.cistRootPriority.intRootPathCost;*/
    pdu->cistIntRootPathCost = p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedPriority.intRootPathCost;

    pdu->cistBridgeId.priInstId = dot1sInstance->cist.BridgeIdentifier.priInstId;
    memcpy((char *)&pdu->cistBridgeId.macAddr,
         (char *)&dot1sInstance->cist.BridgeIdentifier.macAddr,
         L7_MAC_ADDR_LEN);

    pdu->cistRemHops = (L7_uchar8)p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedTimes.remHops;

    msg = 0;

    for (i = 1; i <= L7_MAX_MULTIPLE_STP_INSTANCES; i++)
    {
      if (dot1sInstanceMap[i].inUse == L7_TRUE)
      {
        pdu->packetLength += sizeof(DOT1S_BYTE_MSTI_CFG_MSG_t);
        pdu->ver3Len += sizeof(DOT1S_BYTE_MSTI_CFG_MSG_t);

        pdu->mstiMsg[msg].mstiFlags = 0x00;

        switch (p->portInstInfo[i].role)
        {
          case ROLE_MASTER:
            mask = DOT1S_PORT_ROLE_MASTER_MASK;
            break;

          case ROLE_ROOT:
            mask = DOT1S_PORT_ROLE_ROOT_MASK;
            break;

          case ROLE_DESIGNATED:
            mask = DOT1S_PORT_ROLE_DESIG_MASK;
            break;

          case ROLE_ALTERNATE:
          case ROLE_BACKUP:
            mask = DOT1S_PORT_ROLE_ALT_BCK_MASK;
            break;

          case ROLE_DISABLED:
          default:
            /*the port role can either be one of the above only*/
            /*free the mbuf and return failure*/
            SYSAPI_NET_MBUF_FREE(bufHandle);
            return L7_FAILURE;
            break;

        }/*end switch (p->portInstInfo[i].role)*/
        /***********************************/
        /*compute master flag for this port*/
        (void)dot1sMstiMasterFlagCompute(p, i);
        /***********************************/
        pdu->mstiMsg[msg].mstiFlags |= mask;

        /* The txMstp() (IEEE 802.1Q-2003 13.26.22) procedure requires the agreement
           flag value to be sent as 'agreed'. However the description of agree variable
           (IEEE 802.1Q-2003 13.24.1 & 13.24.2)  and in the explanation of Proposal
           -Agreement protocolthe spec suggests the use of 'agree' flag for the same.
           Based on email communique from IEEE 802.1s body and logical deuduction,
           choosing the use of agree flag.
        */


        if (p->portInstInfo[i].agree == L7_TRUE)
        {
          pdu->mstiMsg[msg].mstiFlags |= DOT1S_AGREEMENT_FLAG_MASK;
        }
        if (p->portInstInfo[i].proposing == L7_TRUE)
        {
          pdu->mstiMsg[msg].mstiFlags |= DOT1S_PROPOSAL_FLAG_MASK;
        }
        if (p->portInstInfo[i].tcWhile != 0)
        {
          pdu->mstiMsg[msg].mstiFlags |= DOT1S_TC_FLAG_MASK;
        }
        if (p->portInstInfo[i].learning == L7_TRUE)
        {
          pdu->mstiMsg[msg].mstiFlags |= DOT1S_LEARNING_FLAG_MASK;
        }
        if (p->portInstInfo[i].forwarding == L7_TRUE)
        {
          pdu->mstiMsg[msg].mstiFlags |= DOT1S_FORWARDING_FLAG_MASK;
        }
        if (p->portInstInfo[i].inst.msti.mstiMaster == L7_TRUE)
        {
          pdu->mstiMsg[msg].mstiFlags |= DOT1S_MASTER_FLAG_MASK;
        }

        pdu->mstiMsg[msg].mstiRegRootId.priInstId =
          p->portInstInfo[i].inst.msti.mstiDesignatedPriority.regRootId.priInstId;
        memcpy((char *)&pdu->mstiMsg[msg].mstiRegRootId.macAddr,
               (char *)&p->portInstInfo[i].inst.msti.mstiDesignatedPriority.regRootId.macAddr,
               L7_MAC_ADDR_LEN);
        /*pdu->mstiMsg[msg].mstiIntRootPathCost =
          dot1sInstance->msti[i].mstiRootPriority.intRootPathCost;*/
        pdu->mstiMsg[msg].mstiIntRootPathCost =
          p->portInstInfo[i].inst.msti.mstiDesignatedPriority.intRootPathCost;


        pdu->mstiMsg[msg].mstiBridgePriority = (L7_uchar8)((dot1sInstance->msti[i].BridgeIdentifier.priInstId >> 12) << 4);
        pdu->mstiMsg[msg].mstiPortPriority = (L7_uchar8)((p->portInstInfo[i].portId >> 12)<<4);
        pdu->mstiMsg[msg].mstiRemHops = (L7_uchar8)(p->portInstInfo[i].inst.msti.mstiDesignatedTimes.remHops);

        msg++;
      }/*end if (dot1sInstanceMap[i].inUse == L7_TRUE)*/
    }/*end for (i = 1; i ....*/

    /*call dot1sTransmitBpdu*/
    dot1sDebugPacketTxTrace(p->portNum,pdu);
    rc = dot1sBpduTransmit(bufHandle, p->portNum);
    if (rc == L7_SUCCESS)
    {
      /*increment txmstp counter*/
      dot1sPortStats[portIndex].mstpTx++;
    }
  }

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
* @comments  assumes that PDU is meant for dot1s
*
* @end
*************************************************************************/
L7_RC_t dot1sTxRxHeaderTagRemove(L7_netBufHandle bufHandle)
{
  L7_uchar8 *data;
  L7_uint32 dataLength;
  L7_uchar8 *offset;
  L7_uint32 diff;



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
      memcpy(temp, offset, dataLength - diff);
      memcpy(data + L7_ENET_HDR_SIZE, temp, dataLength - diff);
      dataLength = dataLength - L7_8021Q_ENCAPS_HDR_SIZE;
      SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, dataLength);
  }

  return L7_SUCCESS;
}
/**************************************************************************
* @purpose   Convert the packet from host byte order (Little endianess) to
*            network byte order (Big endianess)
*
* @param     bufHandle  @b{(input)} the handle to the LACPDU
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*************************************************************************/
L7_netBufHandle dot1sTxRxNetworkConvert(L7_netBufHandle bufHandle)
{
  L7_uchar8             *data, *unpaddedData;
  DOT1S_MSTP_ENCAPS_t   *pdu;
  DOT1S_BYTE_MSTP_ENCAPS_t *unpaddedPdu;
  L7_ushort16           temp16;
  L7_uint32             temp32;
  L7_BOOL               tcnBpdu = L7_FALSE;
  L7_BOOL               configBpdu = L7_FALSE;
  L7_BOOL               rstpBpdu = L7_FALSE;
  L7_BOOL               mstpBpdu = L7_FALSE;
  L7_uint32             numMstiMsgs, i;
  L7_netBufHandle       unpaddedBufHandle;




  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  pdu = (DOT1S_MSTP_ENCAPS_t *)data;

  /*get a new network buffer*/

  SYSAPI_NET_MBUF_GET(unpaddedBufHandle);
  if (unpaddedBufHandle == L7_NULL)
  {
    /*free up the padded buffer*/
    SYSAPI_NET_MBUF_FREE(bufHandle);

    return L7_NULL;
  }
  SYSAPI_NET_MBUF_GET_DATASTART(unpaddedBufHandle, unpaddedData);
  unpaddedPdu = (DOT1S_BYTE_MSTP_ENCAPS_t *) unpaddedData;

  memset((void *)unpaddedPdu, 0x00, sizeof(*unpaddedPdu));

  tcnBpdu = dot1sIsTcnBPDU(pdu);
  configBpdu = dot1sIsConfigBPDU(pdu);
  rstpBpdu = dot1sIsRstBPDU(pdu);
  mstpBpdu = dot1sIsMstBPDU(pdu);

  /*now fill in the internal structure in the correct allignment*/
  memcpy(&unpaddedPdu->destMac[0],
         &pdu->destMac[0],
         DOT1S_BPDU_PROTOCOL_ID_START);
  memcpy((char *)&unpaddedPdu->protocolIdentifier,
         (char *)&pdu->protocolIdentifier,
         DOT1S_BPDU_CISTROOTID_START);

  /* do network to host on the following fields of the pdu
   * packetLength
   */
  temp16 = osapiHtons(pdu->packetLength);
  memcpy(&unpaddedPdu->packetLength, &temp16, sizeof(L7_ushort16));
  /*protocolIdentifier*/
  temp16 = osapiHtons(pdu->protocolIdentifier);
  memcpy(&unpaddedPdu->protocolIdentifier, &temp16, sizeof(L7_ushort16));

  if (tcnBpdu == L7_FALSE)
  {
    memcpy((char *)&unpaddedPdu->cistRootId,
           (char *)&pdu->cistRootId,
           DOT1S_BPDU_FWD_VER1_START);

   /*cistRootId.priInstId*/
    temp16 = osapiHtons(pdu->cistRootId.priInstId);
    memcpy(&unpaddedPdu->cistRootId[0], &temp16, sizeof(L7_ushort16));
    /*cistExtRootPathCost*/
    temp32 = (L7_uint32)osapiHtonl(pdu->cistExtRootPathCost);
    memcpy(&unpaddedPdu->cistExtRootPathCost, &temp32, sizeof(L7_uint32));
    /*cistRegRootId.priInstId*/
    temp16 = osapiHtons(pdu->cistRegRootId.priInstId);
    memcpy(&unpaddedPdu->cistRegRootId[0], &temp16, sizeof(L7_ushort16));
    /*cistPortId*/
    temp16 = osapiHtons(pdu->cistPortId);
    memcpy(&unpaddedPdu->cistPortId, &temp16, sizeof(L7_ushort16));
    /*msgAge*/
    temp16 = osapiHtons(pdu->msgAge);
    memcpy(&unpaddedPdu->msgAge, &temp16, sizeof(L7_ushort16));
    /*maxAge*/
    temp16 = osapiHtons(pdu->maxAge);
    memcpy(&unpaddedPdu->maxAge, &temp16, sizeof(L7_ushort16));
    /*helloTime*/
    temp16 = osapiHtons(pdu->helloTime);
    memcpy(&unpaddedPdu->helloTime, &temp16, sizeof(L7_ushort16));
    /*fwdDelay*/
    temp16 = osapiHtons(pdu->fwdDelay);
    memcpy(&unpaddedPdu->fwdDelay, &temp16, sizeof(L7_ushort16));

    if (configBpdu == L7_FALSE)
    {
      memcpy(&unpaddedPdu->ver1Len, &pdu->ver1Len, DOT1S_BPDU_VER1_LENGTH);

      if (rstpBpdu == L7_FALSE)
      {
        memcpy((char *)&unpaddedPdu->ver1Len,
               (char *)&pdu->ver1Len,
               DOT1S_BPDU_VER1_LENGTH);
        memcpy((char *)&unpaddedPdu->ver3Len,
               (char *)&pdu->ver3Len,
               DOT1S_BPDU_CISTINTROOTPATH_START);

        /*ver3Len*/
        temp16 = osapiHtons(pdu->ver3Len);
        memcpy(&unpaddedPdu->ver3Len, &temp16, sizeof(L7_ushort16));

        memcpy((char *)&unpaddedPdu->cistIntRootPathCost,
               (char *)&pdu->cistIntRootPathCost,
               DOT1S_BPDU_MSTI_MSG_START);

        /*cistIntRootPathCost*/
        temp32 = (L7_uint32)osapiHtonl(pdu->cistIntRootPathCost);
        memcpy(&unpaddedPdu->cistIntRootPathCost, &temp32, sizeof(L7_uint32));
        /*cistBridgeId.priInstId*/
        temp16 = osapiHtons(pdu->cistBridgeId.priInstId);
        memcpy(&unpaddedPdu->cistBridgeId[0], &temp16, sizeof(L7_ushort16));

        numMstiMsgs = pdu->ver3Len/sizeof(DOT1S_BYTE_MSTI_CFG_MSG_t);
        if (numMstiMsgs > 4)
        {
          numMstiMsgs = numMstiMsgs - 4;
          if (numMstiMsgs > 0)
          {
            for (i=1;i<=numMstiMsgs;i++)
            {
              memcpy((char *)&unpaddedPdu->mstiMsg[i-1].mstiFlags,
                     (char *)&pdu->mstiMsg[i-1].mstiFlags,
                     sizeof(L7_uchar8));

              memcpy((char *)&unpaddedPdu->mstiMsg[i-1].mstiRegRootId,
                     (char *)&pdu->mstiMsg[i-1].mstiRegRootId,
                     sizeof(DOT1S_BRIDGEID_t));

              memcpy((char *)&unpaddedPdu->mstiMsg[i-1].mstiIntRootPathCost,
                     (char *)&pdu->mstiMsg[i-1].mstiIntRootPathCost,
                     DOT1S_BPDU_MSTI_REMHOPS_END);


              /*mstiRegRootId.priInstId*/
              temp16 = osapiHtons(pdu->mstiMsg[i-1].mstiRegRootId.priInstId);
              memcpy(&unpaddedPdu->mstiMsg[i-1].mstiRegRootId, &temp16, sizeof(L7_ushort16));
              /*mstiIntRootPathCost*/
              temp32 = (L7_uint32)osapiHtonl(pdu->mstiMsg[i-1].mstiIntRootPathCost);
              memcpy(&unpaddedPdu->mstiMsg[i-1].mstiIntRootPathCost, &temp32, sizeof(L7_uint32));
            }
          }/*end if (numMstiMsgs > 0)*/
        }/*end if (numMstiMsgs > 4)*/
      }/*end if (rstpBpdu == L7_FALSE)*/
    }/*end if (configBpdu == L7_FALSE)*/
  }/*end if (tcnBpdu == L7_FALSE)*/

  SYSAPI_NET_MBUF_SET_DATALENGTH(unpaddedBufHandle, (pdu->packetLength + 12 +2));

  /*free up the padded buffer*/
  SYSAPI_NET_MBUF_FREE(bufHandle);
  return unpaddedBufHandle;
}
/**************************************************************************
* @purpose   Convert the packet from network byte order (Big endianess) to
*            host byte order (Little endianess)
*
* @param     bufHandle  @b{(input)} the handle to the LACPDU
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*************************************************************************/
L7_RC_t dot1sTxRxHostConvert(L7_netBufHandle bufHandle, L7_uchar8 ** buff)
{
  L7_uchar8                 *data;
  DOT1S_BYTE_MSTP_ENCAPS_t  *bpdu;
  L7_ushort16               temp16;
  L7_uint32                 temp32;
  L7_BOOL                   tcnBpdu = L7_FALSE;
  L7_BOOL                   configBpdu = L7_FALSE;
  L7_BOOL                   rstpBpdu = L7_FALSE;
  L7_BOOL                   mstpBpdu = L7_FALSE;
  L7_uint32                 numMstiMsgs, i;
  DOT1S_MSTP_ENCAPS_t       *pdu;
  L7_uchar8                 *pduStart;
  L7_uint32                 dataLen;
  L7_RC_t                   rc;


  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, dataLen);
  bpdu = (DOT1S_BYTE_MSTP_ENCAPS_t *)data;

  /*get a new buffer which will accomodate for the misalignment of this bpdu*/

  rc = bufferPoolAllocate(dot1sBPDUBufferPoolId, buff);



  if (rc != L7_SUCCESS)
  {
    /*free the net buf we received from the network*/
    SYSAPI_NET_MBUF_FREE(bufHandle);
    return L7_FAILURE;
  }

  pduStart = *buff ;



  pdu = (DOT1S_MSTP_ENCAPS_t *)pduStart;
  memset((void *)pdu, 0x00, sizeof(*pdu));

  /*now fill in the internal structure in the correct allignment*/
  memcpy((char *)pdu->destMac,
         (char *)bpdu->destMac,
         DOT1S_BPDU_PROTOCOL_ID_START);
  memcpy((char *)&pdu->protocolIdentifier,
         (char *)bpdu->protocolIdentifier,
         DOT1S_BPDU_CISTROOTID_START);

  /*do network to host on the following fields of the pdu*/
  /*packetLength*/
  temp16 = osapiNtohs(pdu->packetLength);
  memcpy(&pdu->packetLength, &temp16, sizeof(L7_ushort16));
  /*protocolIdentifier*/
  temp16 = osapiNtohs(pdu->protocolIdentifier);
  memcpy(&pdu->protocolIdentifier, &temp16, sizeof(L7_ushort16));
  tcnBpdu = dot1sIsTcnBPDU(pdu);
  if (tcnBpdu == L7_FALSE)
  {
    configBpdu = dot1sIsConfigBPDU(pdu);

    memcpy((char *)&pdu->cistRootId,
           (char *)bpdu->cistRootId,
           DOT1S_BPDU_FWD_VER1_START);

    /*cistRootId.priInstId*/
    temp16 = osapiNtohs(pdu->cistRootId.priInstId);
    memcpy(&pdu->cistRootId.priInstId, &temp16, sizeof(L7_ushort16));
    /*cistExtRootPathCost*/
    temp32 = (L7_uint32)osapiNtohl(pdu->cistExtRootPathCost);
    memcpy(&pdu->cistExtRootPathCost, &temp32, sizeof(L7_uint32));
    /*cistRegRootId.priInstId*/
    temp16 = osapiNtohs(pdu->cistRegRootId.priInstId);
    memcpy(&pdu->cistRegRootId.priInstId, &temp16, sizeof(L7_ushort16));
    /*cistPortId*/
    temp16 = osapiNtohs(pdu->cistPortId);
    memcpy(&pdu->cistPortId, &temp16, sizeof(L7_ushort16));
    /*msgAge*/
    temp16 = osapiNtohs(pdu->msgAge);
    memcpy(&pdu->msgAge, &temp16, sizeof(L7_ushort16));
    /*maxAge*/
    temp16 = osapiNtohs(pdu->maxAge);
    memcpy(&pdu->maxAge, &temp16, sizeof(L7_ushort16));
    /*helloTime*/
    temp16 = osapiNtohs(pdu->helloTime);
    memcpy(&pdu->helloTime, &temp16, sizeof(L7_ushort16));
    /*fwdDelay*/
    temp16 = osapiNtohs(pdu->fwdDelay);
    memcpy(&pdu->fwdDelay, &temp16, sizeof(L7_ushort16));

    if (configBpdu == L7_FALSE)
    {
      memcpy((char *)&pdu->ver1Len,
             (char *)&bpdu->ver1Len,
             DOT1S_BPDU_VER1_LENGTH);
      memcpy((char *)&pdu->ver3Len,
             (char *)&bpdu->ver3Len,
             DOT1S_BPDU_CISTINTROOTPATH_START);

      /*ver3Len*/
      temp16 = osapiNtohs(pdu->ver3Len);
      memcpy(&pdu->ver3Len, &temp16, sizeof(L7_ushort16));
      rstpBpdu = dot1sIsRstBPDU(pdu);

      if (rstpBpdu == L7_FALSE)
      {
        memcpy((char *)&pdu->cistIntRootPathCost,
               (char *)&bpdu->cistIntRootPathCost,
               DOT1S_BPDU_MSTI_MSG_START);
        /*cistIntRootPathCost*/
        temp32 = (L7_uint32)osapiNtohl(pdu->cistIntRootPathCost);
        memcpy(&pdu->cistIntRootPathCost, &temp32, sizeof(L7_uint32));
        /*cistBridgeId.priInstId*/
        temp16 = osapiNtohs(pdu->cistBridgeId.priInstId);
        memcpy(&pdu->cistBridgeId.priInstId, &temp16, sizeof(L7_ushort16));

        mstpBpdu = dot1sIsMstBPDU(pdu);
        if (mstpBpdu == L7_TRUE)
        {
           numMstiMsgs = pdu->ver3Len/sizeof(DOT1S_BYTE_MSTI_CFG_MSG_t);
           if (numMstiMsgs > 4)
           {
             numMstiMsgs = numMstiMsgs - 4;

             if (numMstiMsgs > 0)
             {
               for (i=1;i<=numMstiMsgs;i++)
               {
                 memcpy((char *)&pdu->mstiMsg[i-1].mstiFlags,
                        (char *)&bpdu->mstiMsg[i-1].mstiFlags,
                        sizeof(L7_uchar8));

                 memcpy((char *)&pdu->mstiMsg[i-1].mstiRegRootId,
                        (char *)&bpdu->mstiMsg[i-1].mstiRegRootId,
                        sizeof(DOT1S_BRIDGEID_t));

                 memcpy((char*)&pdu->mstiMsg[i-1].mstiIntRootPathCost,
                        (char *)&bpdu->mstiMsg[i-1].mstiIntRootPathCost,
                        DOT1S_BPDU_MSTI_REMHOPS_END);

                 /*mstiRegRootId.priInstId*/
                 temp16 = osapiNtohs(pdu->mstiMsg[i-1].mstiRegRootId.priInstId);
                 memcpy(&pdu->mstiMsg[i-1].mstiRegRootId.priInstId, &temp16, sizeof(L7_ushort16));
                 /*mstiIntRootPathCost*/
                 temp32 = (L7_uint32)osapiNtohl(pdu->mstiMsg[i-1].mstiIntRootPathCost);
                 memcpy(&pdu->mstiMsg[i-1].mstiIntRootPathCost, &temp32, sizeof(L7_uint32));
               }
             }/*end if (numMstiMsgs > 0)*/
           }/*end if (numMstiMsgs > 4)*/
        }/*end if (mstpBpdu == L7_TRUE)*/
        else
        {
          /*free the net buf we received from the network*/
          SYSAPI_NET_MBUF_FREE(bufHandle);
          /*free the padded netbuf*/
          bufferPoolFree(dot1sBPDUBufferPoolId, *buff);
          return L7_FAILURE;

        }
      }/*end if (rstpBpdu == L7_FALSE)*/
    }/*end if (configBpdu == L7_FALSE)*/
  }/*end if (tcnBpdu == L7_FALSE)*/
  /*free the net buf we received from the network*/
  SYSAPI_NET_MBUF_FREE(bufHandle);

  /*set the padded data length*/
  return L7_SUCCESS;
}

