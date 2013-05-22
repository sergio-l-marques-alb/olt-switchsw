/**
 * ptin_packet.c
 *  
 * Implements the Packet capture and process routines
 *
 * Created on: 2013/05/22 
 * Author: Milton Ruas (milton-r-silva@ext.ptinovacao.pt)
 * Notes: 
 */

#include "ptin_evc.h"
#include "ptin_utils.h"
#include "ptin_intf.h"

#include "sysnet_api_ipv4.h"

#define MAX_FLOOD_VLANS   8

void ptin_packet_send(L7_uint32 intIfNum,
                      L7_uint32 vlanId,
                      L7_uint32 innerVID,
                      L7_uchar8 *payload,
                      L7_uint32 payloadLen);


L7_BOOL ptin_packet_debug_enable = L7_FALSE;

void ptin_packet_debug( L7_BOOL enable)
{
  ptin_packet_debug_enable = enable & 1;
}

/*********************************************************************
* @purpose  Intercept incoming ARP packets.
*
* @param    hookId        The hook location
* @param    bufHandle     Handle to the frame to be processed
* @param    *pduInfo      Pointer to info about this frame
* @param    continueFunc  Optional pointer to a continue function
*
* @returns  SYSNET_PDU_RC_CONSUMED  if packet to be forwarded
*           SYSNET_PDU_RC_IGNORED   if packet should continue normal processing
*           SYSNET_PDU_RC_DISCARD   if packet falls through the cracks
*           SYSNET_PDU_RC_COPIED    if broadcast destination MAC
*
* @notes    This function is executed on the DTL thread.
*
* @end
*********************************************************************/
SYSNET_PDU_RC_t ptinArpRecv(L7_uint32 hookId,
                            L7_netBufHandle bufHandle,
                            sysnet_pdu_info_t *pduInfo,
                            L7_FUNCPTR_t continueFunc)
{
  L7_uint i;
  L7_uint32 intIfNum;
  L7_uint16 vlanId;
  L7_uint16 innerVlanId;

  L7_uchar8 *payload;
  L7_uint32 payloadLen;

  ptin_intf_t ptin_intf;
  L7_uint16   evc_idx;
  L7_BOOL     is_stacked;

  L7_uint32       intf;
  NIM_INTF_MASK_t intfList;

  L7_uint16   number_of_vlans;
  L7_uint16   flood_vlan[MAX_FLOOD_VLANS];

  intIfNum    = pduInfo->intIfNum;     /* Source port */
  vlanId      = pduInfo->vlanId;       /* Vlan */
  innerVlanId = pduInfo->innerVlanId;  /* Inner vlan */

  L7_uchar8 bcast_pkt[6]  = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

  if (ptin_packet_debug_enable)
    LOG_TRACE(LOG_CTX_PTIN_EVC,"Packet received at intIfNum=%u with vlanId=%u and innerVlanId=%u",
              intIfNum, vlanId, innerVlanId);

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, payload);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, payloadLen);

  /* Validate vlan id */
  if ( vlanId < PTIN_VLAN_MIN || vlanId > PTIN_VLAN_MAX )
  {
    if (ptin_packet_debug_enable)
      LOG_ERR(LOG_CTX_PTIN_EVC,"Invalid vlanId %u",vlanId);
    return SYSNET_PDU_RC_IGNORED;
  }
  /* Validate inner vlan id */
  if ( innerVlanId == 0 || innerVlanId > 4095 )
  {
    if (ptin_packet_debug_enable)
      LOG_ERR(LOG_CTX_PTIN_EVC,"Invalid innerVlanId %u",innerVlanId);
    return SYSNET_PDU_RC_IGNORED;
  }

  /* Packet should be ARP broadcast */
  if ( memcmp(&payload[0], bcast_pkt, L7_MAC_ADDR_LEN)!=0 )
  {
    if (ptin_packet_debug_enable)
      LOG_ERR(LOG_CTX_PTIN_EVC,"Packet is not broadcast");
    return SYSNET_PDU_RC_IGNORED;
  }
  /* Packet should be ARP type */
  if ( *((L7_uint16 *) &payload[16]) != L7_ETYPE_ARP )
  {
    if (ptin_packet_debug_enable)
      LOG_ERR(LOG_CTX_PTIN_EVC,"Packet is not ARP");
    return SYSNET_PDU_RC_IGNORED;
  }

  /* Get EVC id from vlanId */
  if (ptin_evc_get_evcIdfromIntVlan(vlanId, &evc_idx)!=L7_SUCCESS)
  {
    if (ptin_packet_debug_enable)
      LOG_ERR(LOG_CTX_PTIN_EVC,"vlanId %u is not related to any EVC",vlanId);
    return SYSNET_PDU_RC_IGNORED;
  }

  /* Get stacked status */
  if (ptin_evc_check_is_stacked_fromIntVlan(vlanId, &is_stacked)!=L7_SUCCESS)
  {
    if (ptin_packet_debug_enable)
      LOG_ERR(LOG_CTX_PTIN_EVC,"EVC %u: Error obtaining stacked status.");
    return SYSNET_PDU_RC_IGNORED;
  }
  if ( !is_stacked )
  {
    if (ptin_packet_debug_enable)
      LOG_ERR(LOG_CTX_PTIN_EVC,"EVC %u: EVC not stacked.");
    return SYSNET_PDU_RC_IGNORED;
  }

  /* Get list of leaf interfaces */
  if (ptin_evc_intfType_getList(vlanId, PTIN_EVC_INTF_LEAF, &intfList)!=L7_SUCCESS)
  {
    if (ptin_packet_debug_enable)
      LOG_ERR(LOG_CTX_PTIN_EVC,"EVC %u: Error getting list of leaf interfaces");
    return SYSNET_PDU_RC_IGNORED;
  }

  /* Forward frame to all interfaces in this VLAN with multicast routers attached */
  for (intf = 1; intf <= L7_MAX_INTERFACE_COUNT; intf++)
  {
    /* Skip not active ports */
    if ( !(L7_INTF_ISMASKBITSET(intfList, intf)) )
    {
      continue;
    }

    /* Convert intf to ptin_intf format */
    if ( ptin_intf_intIfNum2ptintf(intf, &ptin_intf)!=L7_SUCCESS)
    {
      if (ptin_packet_debug_enable)
        LOG_ERR(LOG_CTX_PTIN_EVC,"EVC %u: Error converting intIfNum %u to ptin_intf format",intf);
      continue;
    }

    /* Getting list of vlans */
    number_of_vlans = MAX_FLOOD_VLANS;
    if ( ptin_evc_flood_vlan_get(evc_idx, &ptin_intf, innerVlanId,
                                 flood_vlan, L7_NULLPTR, &number_of_vlans) != L7_SUCCESS )
    {
      if (ptin_packet_debug_enable)
        LOG_ERR(LOG_CTX_PTIN_EVC,"EVC %u: Error getting list of vlans for innervlan=%u, ptin_intf=%u/%u",
                evc_idx, innerVlanId, ptin_intf.intf_type, ptin_intf.intf_id);
      continue;
    }

    if (ptin_packet_debug_enable)
      LOG_TRACE(LOG_CTX_PTIN_EVC,"EVC %u: %u vlans obtained for ptin_intf %u/%u, cvlan=%u",
                evc_idx, number_of_vlans, ptin_intf.intf_type, ptin_intf.intf_id, innerVlanId);

    /* Transmit packet to all vlans of this interface */
    for (i=0; i<number_of_vlans; i++)
    {
      if (ptin_packet_debug_enable)
        LOG_TRACE(LOG_CTX_PTIN_EVC,"EVC %u: Sending packet to ptin_port %u/%u, with oVlan=%u",
                  evc_idx, ptin_intf.intf_type, ptin_intf.intf_id, flood_vlan[i]);

      ptin_packet_send(intf, vlanId, flood_vlan[i], payload, payloadLen);
    }
  }

  return SYSNET_PDU_RC_COPIED;
}


/*********************************************************************
* @purpose  Send a multicast packet on a specified interface and vlan
*
* @param    intIfNum   @b{(input)} Outgoing internal interface number
* @param    vlanId     @b{(input)} VLAN ID
* @param    payload    @b{(input)} Message to be forwarded
* @param    payloadLen @b{(input)} Length of message
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void ptin_packet_send(L7_uint32 intIfNum,
                      L7_uint32 vlanId,
                      L7_uint32 new_vlanId,
                      L7_uchar8 *payload,
                      L7_uint32 payloadLen)
{
  L7_netBufHandle   bufHandle;
  L7_uchar8        *dataStart;
  L7_INTF_TYPES_t   sysIntfType;
  DTL_CMD_TX_INFO_t dtlCmd;


  /* If outgoing interface is CPU interface, don't send it */
  if ( (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS) &&
       (sysIntfType == L7_CPU_INTF) )
  {
    return;
  }

  SYSAPI_NET_MBUF_GET(bufHandle);
  if (bufHandle == L7_NULL)
  {
    return;
  }

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, dataStart);
  memcpy(dataStart, payload, payloadLen);
  SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, payloadLen);

  /* Change outer vlan */
  dataStart[14] &= 0xf0;
  dataStart[14] |= (new_vlanId >> 8) & 0x0f;
  dataStart[15]  = new_vlanId & 0xff;

  memset((L7_uchar8 *)&dtlCmd, 0, sizeof(DTL_CMD_TX_INFO_t));

  dtlCmd.intfNum             = intIfNum;
  dtlCmd.priority            = 1;
  dtlCmd.typeToSend          = DTL_NORMAL_UNICAST;
  dtlCmd.cmdType.L2.domainId = vlanId;
  dtlCmd.cmdType.L2.vlanId   = vlanId;

  dtlPduTransmit (bufHandle, DTL_CMD_TX_L2, &dtlCmd);

  if (ptin_packet_debug_enable)
    LOG_TRACE(LOG_CTX_PTIN_EVC,"Packet transmited to intIfNum=%u", intIfNum);

  return;
}

