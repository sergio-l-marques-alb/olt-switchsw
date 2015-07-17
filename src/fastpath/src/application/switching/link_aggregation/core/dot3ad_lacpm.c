/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename dot3ad_lacpm.c                        
*
* @purpose LACPM- Link Aggregation Control Parser Multiplexer
*
* @component Link aggregation Group dot3ad (IEEE 802.3ad)
*
* @notes  This component is responsible to parse control frames coming into LACP
*         LACPDU and Marker Frames. It is also responsible to multiplex
*         the LACPDU and Marker Response Frames out to the lower layers 
*
* @create 8/8/2001
*
* @author skalyanam
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#include "dot3ad_include.h"
#include "sysnet_api.h"
#include "osapi.h"
#include "osapi_support.h"
#include "buff_api.h"
#include "dot3ad_debug.h"
#include "dot3ad_helper.h"
#include "dot3ad_transport.h"
#include "unitmgr_api.h"
#if 1
/* PTin added: SSM packet processing */
#include "ptin_ssm.h"
#endif

#ifdef DOT3AD_COMPONENT_LACPM

extern L7_BOOL dot3adCnfgrIsHelperPresent();
extern dot3ad_system_t dot3adSystem;
extern dot3ad_stats_t  dot3ad_stats[L7_MAX_INTERFACE_COUNT];
extern const L7_uchar8 dot3adSlowProtocolsMulticastMacAddr[L7_MAC_ADDR_LEN];                /* Tbl 43B-1 */
extern L7_uint32 dot3adBufferPoolId;
extern dot3adOperPort_t dot3adOperPort[L7_MAX_PORT_COUNT + 1];
extern L7_uint32 dot3adSwitchoverInProgress;
/**************************************************************************
*
* @purpose   This function determines whether the interface is 
*            a valid candidate for participation in the LAG.
*            The port must be in full duplex and same speed as other
*            LAG members.
*
* @param     intIfNum     the internal interface number
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes  
*
* @end
**************************************************************************/
static L7_RC_t dot3adPortDuplexSpeedTest (L7_uint32 intIfNum)
{
  dot3ad_agg_t *a;
  dot3ad_port_t *p;
  L7_uint32 i,firstPort;
  
  p = dot3adPortIntfFind(intIfNum);
  if (p == L7_NULLPTR)
  {
      return L7_FAILURE;
  }
     /* is the aggregator valid */
  a = dot3adAggIntfFind(p->actorPortWaitSelectedAggId);
  if ((a == L7_NULLPTR) || (a->inuse == L7_FALSE) || (a->adminMode == L7_DISABLE))
  {
    return L7_FAILURE;
  }

 /* If this port is already an active member then we don't need to do
  ** the link tests.
  */
  for (i = 0; i < a->activeNumMembers; i++)
  {
    if (intIfNum == a->aggActivePortList[i])
    {
      return L7_SUCCESS;
    }
  }
  
  if (dot3adOperPort[intIfNum].fullDuplex == L7_FALSE)
  {
    return L7_FAILURE;
  }

  /* does the speed match */
  if (a->activeNumMembers > 0)
  {
    firstPort = a->aggActivePortList[0];
    if (dot3adOperPort[firstPort].linkSpeed != dot3adOperPort[intIfNum].linkSpeed)
    {
      return L7_FAILURE;
    }
  }


  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose   Receives a PDU from DTL, it parses the PDU and 
*
* @param     intIfNum     the internal interface number the PDU was received in
* @param     bufHandle    handle to the network buffer the PDU is stored in      
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes  determines whether it is a LACPDU or a Marker Response PDU 
*            if it is a LACPDU it puts the received notification in the LAC queue if it is 
*            a Marker Response PDU it sends it to the APM/MR module
*            This routine is registerd with sysnet to received all slow protocol mucticast 
*            destined PDUs 
*
* @end
*
*************************************************************************/
L7_RC_t dot3adPduReceive(L7_netBufHandle bufHandle,sysnet_pdu_info_t *pduInfo )
{
  L7_uchar8 *data;
  L7_ushort16 lengthType;
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 intIfNum;
  L7_INTF_TYPES_t sysIntfType;
  dot3ad_port_t *p=L7_NULLPTR;
  dot3ad_agg_t *agg=L7_NULLPTR;
  L7_uchar8 * buffer = L7_NULLPTR;
  L7_uint32 dataLength = 0;

#if 1
  /* PTin added: SSM packet processing */
  if (ssmPDUReceive(bufHandle,pduInfo)==L7_SUCCESS)
  {
    return L7_SUCCESS;
  }
#endif

  intIfNum = pduInfo->intIfNum;
  rc = nimGetIntfType(intIfNum, &sysIntfType);

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  /*check if this PDU is meant for dot3ad by checking the DA*/

  if (memcmp(data,dot3adSlowProtocolsMulticastMacAddr,L7_MAC_ADDR_LEN) == 0 &&
      sysIntfType == L7_PHYSICAL_INTF)
  {
    /* If port is not full duplex or port speed is mismatched then
    ** don't accept the PDU on this port.
    */
    rc = dot3adPortDuplexSpeedTest (intIfNum);
    if (rc != L7_SUCCESS) 
    {
        SYSAPI_NET_MBUF_FREE(bufHandle);
        return L7_SUCCESS;
    }

    /*remove any header tagging*/
    rc = dot3adLacpmHeaderTagRemove(bufHandle);
    /*accomodate for byte order*/
    rc = dot3adLacpmHostConvert(bufHandle);

    SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
    /*check to see if the PDU carries the slow protocols type field of 0x8809*/
    memcpy(&lengthType,data+DOT3AD_LENGTHTYPE_OFFSET,2);  
    if (lengthType == DOT3AD_SLOW_PROTOCOL_TYPE)
    {

      if (data[DOT3AD_SUBTYPE_OFFSET] == DOT3AD_LACP_SUBTYPE)
      {
        /*increment counter*/
        dot3ad_stats[intIfNum].LACPDUsRx++;
        /* add check that aggregator of the port is not static and LACPDU need not be dropped*/
        p= dot3adPortIntfFind(intIfNum);
        if (p == L7_NULLPTR)
        {
          /* error ! LACPDU has been received on a port that does belong to lag */
          /* since this is a callback function consume packet and return success */
          SYSAPI_NET_MBUF_FREE(bufHandle);
          return L7_SUCCESS;
        }/* port not found */

        /* find aggregator of given interface */
        agg = dot3adAggKeyFind(p->actorOperPortKey);
        if (agg == L7_NULLPTR)
        {
          /* error ! LACPDU has been received on a port that does belong to lag */
          /* since this is a callback function consume packet and return success */
          SYSAPI_NET_MBUF_FREE(bufHandle);
          return L7_SUCCESS;

        }
        if (agg->isStatic == L7_FALSE)
        {
            
            SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, dataLength);

            if ((bufferPoolAllocate(dot3adBufferPoolId , &buffer) != L7_SUCCESS) ||
                (dataLength > DOT3AD_BUFFER_SIZE))
            {
              /* increment  appropriate statistics */
              dot3ad_stats[intIfNum].RxLACPDUsDropped++;
              /* consume packet and return success*/
              SYSAPI_NET_MBUF_FREE(bufHandle);
              return L7_SUCCESS;
            }
            /* We have a good buffer, copy the incoming mbuf into our buffer */
            memcpy(buffer, data, dataLength);

            /* Now free the mbuf */
            SYSAPI_NET_MBUF_FREE(bufHandle);

            /*send msg to LAC queue with lacpPduRx and bufHandle and the port 
            it was received in*/
            rc = LACIssueCmd(LACPDU_RECEIVE, intIfNum, (void*) buffer);
            if (L7_ERROR == rc || L7_FAILURE == rc)
            {
              bufferPoolFree(dot3adBufferPoolId,  buffer);
              /* By returning a success we ensure that this mbuf is not freed 
                 again by sysnet*/
              L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT3AD_COMPONENT_ID,
                      "Error in processing lag pdu %d", rc);
            
              rc = L7_SUCCESS;
            }
         }
         else
         {
           /* increment  appropriate statistics */
             dot3ad_stats[intIfNum].RxLACPDUsDropped++;
           /* consume packet and return success*/
             SYSAPI_NET_MBUF_FREE(bufHandle);
             return L7_SUCCESS;
         } /* mode == static*/
      }
      else if (data[DOT3AD_SUBTYPE_OFFSET] == DOT3AD_MARKER_SUBTYPE)
      {
        /*increment counter for marker pdu received*/
        dot3ad_stats[intIfNum].MarkerPDUsRx++;

        /*call the Marker Responder with bufHandle and the port it was received in*/
        rc = dot3adMarkerResponder(intIfNum, bufHandle);
      }
      else if (data[DOT3AD_SUBTYPE_OFFSET]==DOT3AD_ILLEGAL_SUBTYPE
               ||data[DOT3AD_SUBTYPE_OFFSET]>=DOT3AD_ILLEGAL__SUBTYPE_MIN)
      { /*increment counter for illegal packet*/

        dot3ad_stats[intIfNum].IllegalRx++;
        SYSAPI_NET_MBUF_FREE(bufHandle);
        return L7_SUCCESS;

      }
      else
      {
        dot3ad_stats[intIfNum].UnknownRx++;
        SYSAPI_NET_MBUF_FREE(bufHandle);
        return L7_SUCCESS;

      }
    }
    else
    {
      dot3ad_stats[intIfNum].UnknownRx++;
      SYSAPI_NET_MBUF_FREE(bufHandle);
      return L7_SUCCESS;
    }

    /*the Mbuffer is freed by the lacpreceive machine for LACPDU*/
    /*and for marker response pdu the same buffer is sent back to the lower layer*/
    /*and is freed after the pdu is sent*/
    return rc;
  }

  else
  {
    return L7_FAILURE;
  }
}

/**************************************************************************
*
* @purpose   Receives a Marker Response PDU  
*
* @param     intIfNum     the internal interface number the PDU was received in
* @param     bufHandle    handle to the network buffer the PDU is stored in      
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes  it changes the subtype from marker information to marker information response
*            and send it through the same interface it arrived in 
*
* @end
*
*************************************************************************/
L7_RC_t dot3adMarkerResponder(L7_uint32 intIfNum, L7_netBufHandle bufHandle)
{
  L7_uchar8 *data;
  L7_uchar8  macAddr[6];
  L7_RC_t rc;

  if (nimGetIntfAddress(intIfNum,L7_NULL,macAddr)== L7_SUCCESS)
  {
    SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
    /*set the sourceaddr as this interfaces local mac address*/
    memcpy(data+6,macAddr,L7_MAC_ADDR_LEN);
    /*set the TVL_Type as marker Response Information 0x02*/
    memset(data+DOT3AD_TVLTYPE_OFFSET,DOT3AD_MARKER_RESP_INFO,1);
    /*set the data length in the netMbuf structure*/
    SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, DOT3AD_MKRESP_DATALENGTH);

    rc = dot3adLacpmTransmit(intIfNum,bufHandle);

    return rc;
  }
  else
    return L7_FAILURE;
}

/**************************************************************************
*
* @purpose   Notify the helper about stopping the transmission of the PDU 
*
* @param     intIfNum     the internal interface number the PDU is being sent
*
* @returns   L7_SUCCESS or L7_FAILURE
*
* @notes     
*
* @end
*
*************************************************************************/
L7_RC_t dot3adLacpmNotifyHelperPduStop(L7_uint32 intIfNum)
{
  dot3adHelperMsg_t *localMsg = L7_NULLPTR;
  dot3ad_port_t *p;

  p = dot3adPortIntfFind(intIfNum);
  if (p == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  osapiSemaTake(dot3adTransSema,L7_WAIT_FOREVER);
  localMsg = &(dot3adTransportPduDb[intIfNum]);

  localMsg->msgType = DOT3AD_HELPER_PDU_STOP;

  memset(&(localMsg->pdu), 0, sizeof(dot3ad_pdu_t));
  localMsg->interval = 0;
  localMsg->intIfNum = intIfNum;
  if(nimGetUnitSlotPort(intIfNum, &localMsg->usp) != L7_SUCCESS)
  {
    osapiSemaGive(dot3adTransSema);
    return L7_FAILURE;
  } 

  L7_INTF_SETMASKBIT(pduChangedMask, intIfNum);
  osapiSemaGive(dot3adTransSyncSema);

  osapiSemaGive(dot3adTransSema);

  return L7_SUCCESS;


}

/**************************************************************************
*
* @purpose   Notify the helper about the transmission of the PDU 
*
* @param     intIfNum     the internal interface number the PDU is to be sent through
* @param     dot3ad_pdu_t *pdu to be transmit to the helper      
*
* @returns   L7_SUCCESS or L7_FAILURE
*
* @notes     This function checks with the shared memory if the PDU to be transmit 
*            is different from the stored or if the interval is different from the stored
*            value. If it is, it will overwrite the stored pdu /interval with the new 
*            information and notify the dot3ad transport task which is responsible to send 
*            the PDU to the helper.
*
* @end
*
*************************************************************************/
L7_RC_t dot3adLacpmNotifyHelperPduStart(dot3ad_pdu_t *pdu, L7_uint32 intIfNum)
{
  dot3adHelperMsg_t *localMsg = L7_NULLPTR;
  L7_uint32 pduComp;
  L7_uint32 interval;
  dot3ad_port_t *p;

  p = dot3adPortIntfFind(intIfNum);
  if (p == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  interval = (dot3adPerMachineIsFast(p) == L7_TRUE) ? 
      (DOT3AD_FAST_PER_TIME * 1000) : (DOT3AD_SLOW_PER_TIME * 1000);


  DOT3AD_DEBUG_NSF_PRINT_DEBUG(DOT3AD_DEBUG_NSF_TRANSPORT, 
                 "Transmit opportunity on interface(%d) interval %d \n",
                  intIfNum, interval);

  osapiSemaTake(dot3adTransSema,L7_WAIT_FOREVER);
  localMsg = &(dot3adTransportPduDb[intIfNum]);

  pduComp = memcmp(&(localMsg->pdu), pdu, sizeof(dot3ad_pdu_t));

  if (pduComp != 0 || localMsg->interval != interval)
  {

     memcpy(&(localMsg->pdu), pdu, sizeof(dot3ad_pdu_t));
     localMsg->interval = interval;
     localMsg->intIfNum = intIfNum;
     localMsg->msgType = DOT3AD_HELPER_PDU_START;

     if(nimGetUnitSlotPort(intIfNum, &localMsg->usp) != L7_SUCCESS)
     {
       osapiSemaGive(dot3adTransSema);
       return L7_FAILURE;
     }  

     L7_INTF_SETMASKBIT(pduChangedMask, intIfNum);
     DOT3AD_DEBUG_NSF_PRINT_DEBUG(DOT3AD_DEBUG_NSF_TRANSPORT, 
                    "Data available for on interface (%d)  \n", intIfNum);
     
     osapiSemaGive(dot3adTransSyncSema);

  }
  
  osapiSemaGive(dot3adTransSema);

  return L7_SUCCESS;

}

/**************************************************************************
*
* @purpose   Notify the helper about modifying the timer interval 
*
* @param     intIfNum     the internal interface number the PDU is being sent
*
* @returns   L7_SUCCESS or L7_FAILURE
*
* @notes     
*
* @end
*
*************************************************************************/
L7_RC_t dot3adLacpmNotifyHelperPduModify(L7_uint32 intIfNum, L7_uint32 interval)
{
  dot3adHelperMsg_t *localMsg = L7_NULLPTR;
  dot3ad_port_t *p;

  p = dot3adPortIntfFind(intIfNum);
  if (p == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  osapiSemaTake(dot3adTransSema,L7_WAIT_FOREVER);
  localMsg = &(dot3adTransportPduDb[intIfNum]);

  localMsg->msgType = DOT3AD_HELPER_PDU_MODIFY;

  localMsg->interval = interval;
  localMsg->intIfNum = intIfNum;

  if(nimGetUnitSlotPort(intIfNum, &localMsg->usp) != L7_SUCCESS)
  {
    osapiSemaGive(dot3adTransSema);
    return L7_FAILURE;
  }  

  L7_INTF_SETMASKBIT(pduChangedMask, intIfNum);
  osapiSemaGive(dot3adTransSyncSema);

  osapiSemaGive(dot3adTransSema);

  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose   Transmit a pdu to DTL  
*
* @param     intIfNum     the internal interface number the PDU is to be sent through
* @param     bufHandle    handle to the network buffer the PDU is stored in      
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes     transmits both LACPDU and Marker Response pdu
*         
*
* @end
*
*************************************************************************/
L7_RC_t dot3adLacpmTransmit(L7_uint32 intIfNum, L7_uint32 bufHandle)
{
  L7_RC_t rc;
  L7_NIM_QUERY_DATA_t nimQueryData;
  L7_uchar8 *data;
  dot3ad_pdu_t *pdu;
  L7_uint32 subType, tlvType;
  DTL_CMD_TX_INFO_t dtlCmdInfo;
 
  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  pdu = (dot3ad_pdu_t *)data;
  subType = pdu->subType;
  tlvType = pdu->tvlTypeActor;

  nimQueryData.intIfNum = intIfNum;
  nimQueryData.request = L7_NIM_QRY_RQST_STATE;

  if (nimIntfQuery(&nimQueryData) != L7_SUCCESS)
  {
    /* should never get here */
    return L7_FAILURE;
  }
  else if ((nimQueryData.data.state == L7_INTF_ATTACHING) ||
           (nimQueryData.data.state == L7_INTF_DETACHING) ||
           (nimQueryData.data.state == L7_INTF_ATTACHED))
  {
    /* only talk to the hardware when the hardware is valid */

    /* If port is not full duplex or port speed is mismatched then
    ** don't send PDU on this port.
    */
    rc = dot3adPortDuplexSpeedTest (intIfNum);
    if (rc != L7_SUCCESS)
    {
      SYSAPI_NET_MBUF_FREE(bufHandle);
      return L7_SUCCESS;
    }

    /*accomodate byte order*/
    rc = dot3adLacpmNetworkConvert(pdu);

    /* If the interface is local or if nsf is not supported transmit directly
       otherwise call notify the helper and let it take care of further transmissions.
    */
    if (dot3adCnfgrIsHelperPresent() == L7_TRUE)
    {
      dot3adLacpmNotifyHelperPduStart(pdu, intIfNum);
      SYSAPI_NET_MBUF_FREE(bufHandle);
    }
    else
    {
        /*send it back to DTL for transmit*/
      bzero((char *)&dtlCmdInfo,(L7_int32)sizeof(DTL_CMD_TX_INFO_t));
      dtlCmdInfo.intfNum= intIfNum;
      dtlCmdInfo.priority = 1;
      dtlCmdInfo.typeToSend = DTL_L2RAW_UNICAST;

      rc = dtlPduTransmit( bufHandle, DTL_CMD_TX_L2, &dtlCmdInfo);
    }
  }
  else
  {
    /* HW not present, free the buffer and return success */
    SYSAPI_NET_MBUF_FREE(bufHandle);
    return L7_SUCCESS;
  }

  if (rc == L7_SUCCESS)
  {
      /*increment a counter here*/
    if (subType == DOT3AD_LACP_SUBTYPE)
    {
      dot3ad_stats[intIfNum].LACPDUsTx++;
    }
    else if (subType == DOT3AD_MARKER_SUBTYPE)
    {
      if (tlvType == DOT3AD_MARKER_RESP_INFO)
      {
        dot3ad_stats[intIfNum].MarkerResponsePDUsTx++;
      }
      else if (tlvType == DOT3AD_MARKER_INFO)
      {
        dot3ad_stats[intIfNum].MarkerPDUsTx++;

      }
    }
  }

  return rc;
}

/**************************************************************************
*
* @purpose   Init the LACPM  
*
* @returns   L7_SUCCESS or L7_FAILURE 
*
* @notes     register the receive function with sysnet
*         
*
* @end
*
*************************************************************************/
L7_RC_t dot3adLacpmInit(void)
{
  L7_RC_t rc;
  sysnetNotifyEntry_t snEntry;

  LOG_INFO(LOG_CTX_STARTUP,"Going to register dot3adPduReceive related to type=%u: 0x%08x",
           SYSNET_MAC_ENTRY, (L7_uint32) dot3adPduReceive);

  /*Register the pdu receive function with sysnet utility*/
  strcpy(snEntry.funcName, "dot3adPduReceive");
  snEntry.notify_pdu_receive = dot3adPduReceive;
  snEntry.type = SYSNET_MAC_ENTRY;
  memcpy(snEntry.u.macAddr, dot3adSlowProtocolsMulticastMacAddr, L7_MAC_ADDR_LEN);
  rc = sysNetRegisterPduReceive(&snEntry); 
  return rc;
}
/**************************************************************************
*
* @purpose   Remove any tagging that may be there between the src addr and the lengthType field  
*
* @param     bufHandle    handle to the network buffer the PDU is stored in      
*
* @returns   L7_SUCCESS or L7_FAILURE 
*
* @notes     assumes that PDU is meant for dot3ad
*         
*
* @end
*
*************************************************************************/
L7_RC_t dot3adLacpmHeaderTagRemove(L7_netBufHandle bufHandle)
{
  L7_uchar8 *data;
  L7_uchar8 *strippedData;
  L7_int32 dataLength;
  L7_int32 offset = 0;

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, dataLength);

  offset = sysNetDataOffsetGet(data);
/*subtract two bytes as we need the lenghtType field*/
/*subtract 12 bytes for the mac addrrs*/
  offset = offset - L7_ENET_HDR_SIZE - 2;

  if (offset > 0)
  {
    /* adjust frame start and length */
    strippedData = &data[offset];
    SYSAPI_NET_MBUF_SET_DATASTART(bufHandle, strippedData);
    SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, dataLength - offset);

    /* move the data */
    for (offset = (L7_ENET_HDR_SIZE - 1); offset >= 0; offset--) 
      strippedData[offset] = data[offset];
  }
  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose   Convert the packet from network byte order (Big endianess)
*            to host byte order
*
* @param     bufHandle  the handle to the LACPDU
*
* @returns   L7_SUCCESS or L7_FAILURE 
*
* @notes     Applies to LACPDU only
*         
*
* @end
*
*************************************************************************/
L7_RC_t dot3adLacpmHostConvert(L7_netBufHandle bufHandle)
{
  L7_uchar8 *data;
  dot3ad_pdu_t  *pdu;
  L7_ushort16   temp16;

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  pdu = (dot3ad_pdu_t *)data;

  /*do network to host on the following fields of the pdu*/
  /*lengthType*/
  temp16 = osapiNtohs(pdu->lengthType);
  memcpy(&pdu->lengthType, &temp16, sizeof(L7_ushort16));
  /*actorSysPri*/
  temp16 = osapiNtohs(pdu->actorSysPri);
  memcpy(&pdu->actorSysPri, &temp16, sizeof(L7_ushort16));
  /*actorKey*/
  temp16 = osapiNtohs(pdu->actorKey);
  memcpy(&pdu->actorKey, &temp16, sizeof(L7_ushort16));
  /*actorPortPri*/
  temp16 = osapiNtohs(pdu->actorPortPri);
  memcpy(&pdu->actorPortPri, &temp16, sizeof(L7_ushort16));
  /*actorPort*/
  temp16 = osapiNtohs(pdu->actorPort);
  memcpy(&pdu->actorPort, &temp16, sizeof(L7_ushort16));
  /*partnerSysPri*/
  temp16 = osapiNtohs(pdu->partnerSysPri);
  memcpy(&pdu->partnerSysPri, &temp16, sizeof(L7_ushort16));
  /*partnerKey*/
  temp16 = osapiNtohs(pdu->partnerKey);
  memcpy(&pdu->partnerKey, &temp16, sizeof(L7_ushort16));
  /*partnerPortPri*/
  temp16 = osapiNtohs(pdu->partnerPortPri);
  memcpy(&pdu->partnerPortPri, &temp16, sizeof(L7_ushort16));
  /*partnerPort*/
  temp16 = osapiNtohs(pdu->partnerPort);
  memcpy(&pdu->partnerPort, &temp16, sizeof(L7_ushort16));
  /*collectorMaxDelay*/
  temp16 = osapiNtohs(pdu->collectorMaxDelay);
  memcpy(&pdu->collectorMaxDelay, &temp16, sizeof(L7_ushort16));


  return L7_SUCCESS;
}
/**************************************************************************
*
* @purpose   Convert the packet from host byte order
*            to network byte order (Big endianess)
*
* @param     bufHandle  the handle to the LACPDU
*
* @returns   L7_SUCCESS or L7_FAILURE 
*
* @notes     Applies to LACPDU only
*         
*
* @end
*
*************************************************************************/
L7_RC_t dot3adLacpmNetworkConvert(dot3ad_pdu_t * pdu)
{
  L7_ushort16   temp16;



  /*do host to network on the following fields of the pdu*/
  /*lengthType*/
  temp16 = osapiHtons(pdu->lengthType);
  memcpy(&pdu->lengthType, &temp16, sizeof(L7_ushort16));
  /*actorSysPri*/
  temp16 = osapiHtons(pdu->actorSysPri);
  memcpy(&pdu->actorSysPri, &temp16, sizeof(L7_ushort16));
  /*actorKey*/
  temp16 = osapiHtons(pdu->actorKey);
  memcpy(&pdu->actorKey, &temp16, sizeof(L7_ushort16));
  /*actorPortPri*/
  temp16 = osapiHtons(pdu->actorPortPri);
  memcpy(&pdu->actorPortPri, &temp16, sizeof(L7_ushort16));
  /*actorPort*/
  temp16 = osapiHtons(pdu->actorPort);
  memcpy(&pdu->actorPort, &temp16, sizeof(L7_ushort16));
  /*partnerSysPri*/
  temp16 = osapiHtons(pdu->partnerSysPri);
  memcpy(&pdu->partnerSysPri, &temp16, sizeof(L7_ushort16));
  /*partnerKey*/
  temp16 = osapiHtons(pdu->partnerKey);
  memcpy(&pdu->partnerKey, &temp16, sizeof(L7_ushort16));
  /*partnerPortPri*/
  temp16 = osapiHtons(pdu->partnerPortPri);
  memcpy(&pdu->partnerPortPri, &temp16, sizeof(L7_ushort16));
  /*partnerPort*/
  temp16 = osapiHtons(pdu->partnerPort);
  memcpy(&pdu->partnerPort, &temp16, sizeof(L7_ushort16));
  /*collectorMaxDelay*/
  temp16 = osapiHtons(pdu->collectorMaxDelay);
  memcpy(&pdu->collectorMaxDelay, &temp16, sizeof(L7_ushort16));


  return L7_SUCCESS;
}






#endif /*DOT3AD_COMPONENET_LACPM*/
