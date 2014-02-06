/*********************************************************************
*
* (C) Copyright PT Inovação S.A. 2013-2013
*
**********************************************************************
*
* @create    17/01/2014
*
* @author    Daniel Filipe Figueira
*
**********************************************************************/
#include "snooping_mgmd_api.h"
#include "ptin_globaldefs.h"
#include "logger.h" 
#include "usmdb_snooping_api.h"
#include "l3_addrdefs.h"
#include "comm_mask.h"
#include "ptin_evc.h"
#include "snooping_util.h"
#include "snooping_proto.h"
#include "snooping_db.h"

ptin_mgmd_externalapi_t mgmd_external_api = {
  .igmp_admin_set=snooping_igmp_admin_set,
  .mld_admin_set=snooping_mld_admin_set,
  .cos_set=snooping_cos_set,
  .portList_get=snooping_portList_get,
  .portType_get=snooping_portType_get,
  .clientList_get=snooping_clientList_get,
  .port_open=snooping_port_open,
  .port_close=snooping_port_close,
  .tx_packet=snooping_tx_packet,
  };

RC_t snooping_igmp_admin_set(uint8 admin)
{
  LOG_TRACE(LOG_CTX_PTIN_IGMP, "Context [admin:%u]", admin);

  // Snooping global activation
  if (L7_SUCCESS != usmDbSnoopAdminModeSet( 1, admin, L7_AF_INET))  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error with usmDbSnoopAdminModeSet");
    return FAILURE;
  }

  return SUCCESS;
}

RC_t snooping_mld_admin_set(uint8 admin)
{
  LOG_TRACE(LOG_CTX_PTIN_IGMP, "Context [admin:%u]", admin);

  // Snooping global activation
  if (L7_SUCCESS != usmDbSnoopAdminModeSet( 1, admin, L7_AF_INET6))  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error with usmDbSnoopAdminModeSet");
    return FAILURE;
  }

  return SUCCESS;
}

RC_t snooping_cos_set(uint8 cos)
{
  LOG_TRACE(LOG_CTX_PTIN_IGMP, "Context [cos:%u]", cos);

  // Attrib IGMP packets priority
  if (L7_SUCCESS != usmDbSnoopPrioModeSet(1, cos, L7_AF_INET))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error with usmDbSnoopPrioModeSet");
    return FAILURE;
  }

  return SUCCESS;
}

RC_t snooping_portList_get(uint32 serviceId, ptin_mgmd_port_type_t portType, PTIN_MGMD_PORT_MASK_t *portList)
{
  L7_INTF_MASK_t interfaceBitmap = {{0}};
  L7_uint16      mcastRootVlan;
  L7_RC_t        res = SUCCESS;
  L7_int32       i;

  LOG_TRACE(LOG_CTX_PTIN_IGMP, "Context [serviceId:%u portType:%u portList:%p]", serviceId, portType, portList);

  if( SUCCESS != ptin_evc_intRootVlan_get(serviceId, &mcastRootVlan))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Unable to get mcastRootVlan from serviceId");
    return FAILURE;
  } 

  /* Request portList to FP */
  if(PTIN_MGMD_PORT_TYPE_LEAF == portType)
  {
    res = ptin_igmp_clientIntfs_getList(mcastRootVlan, &interfaceBitmap);
  }
  else if(PTIN_MGMD_PORT_TYPE_ROOT == portType)
  {
    res = ptin_igmp_rootIntfs_getList(mcastRootVlan, &interfaceBitmap);
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Unknown port type");
    return FAILURE;
  }

  /* We need to shift the bitmap returned by FP to the left by 1 position to ensure compatibility with MGMD */
  for(i=0; i < L7_INTF_INDICES; ++i)
  {
    L7_uchar8 current_byte;

    current_byte = interfaceBitmap.value[i];

    //'OR' the "lost" bit in the previous position if this is not the first index of the array
    if(i != 0) 
    {
      L7_uchar8 lost_bit;

      lost_bit = (current_byte & 0x80) >> 7;
      interfaceBitmap.value[i-1] |= lost_bit;
    }

    interfaceBitmap.value[i] = current_byte << 1;
  }

  if(SUCCESS != res)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Unable to get port information");
    return FAILURE;
  }

  memcpy(portList, &interfaceBitmap.value, PTIN_MGMD_PORT_MASK_INDICES*sizeof(uchar8));

  return SUCCESS;
}

RC_t snooping_portType_get(uint32 serviceId, uint32 portId, ptin_mgmd_port_type_t *portType)
{
  L7_uint16 mcastRootVlan;

  LOG_TRACE(LOG_CTX_PTIN_IGMP, "Context [serviceId:%u portId:%u portType:%u]", serviceId, portId, *portType);

  if( SUCCESS != ptin_evc_intRootVlan_get(serviceId, &mcastRootVlan))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Unable to get mcastRootVlan from serviceId");
    return FAILURE;
  } 

  if(SUCCESS == ptin_igmp_rootIntfVlan_validate(portId, mcastRootVlan))
  {
    LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Port is root");
    *portType = PTIN_MGMD_PORT_TYPE_ROOT;
  }
  else if(SUCCESS == ptin_igmp_clientIntfVlan_validate(portId, mcastRootVlan))
  {
    LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Port is leaf");
    *portType = PTIN_MGMD_PORT_TYPE_LEAF;
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Unknown port type");
    return FAILURE;
  }

  return SUCCESS;
}

RC_t snooping_clientList_get(uint32 serviceId, uint32 portId, uint8 *clientList)
{
  LOG_TRACE(LOG_CTX_PTIN_IGMP, "Context [serviceId:%u portId:%u clientList:%p]", serviceId, portId, clientList);

  return SUCCESS;
}

RC_t snooping_port_open(uint32 serviceId, uint32 portId, uint32 groupAddr, uint32 sourceAddr, BOOL isStatic)
{
  L7_RC_t        rc = L7_SUCCESS;
  snoop_cb_t     *pSnoopCB = L7_NULLPTR;
  snoopPDU_Msg_t msg;

  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Context [serviceId:%u portId:%u groupAddr:%08X sourceAddr:%08X isStatic:%u]", serviceId, portId, groupAddr, sourceAddr, isStatic);

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(L7_AF_INET)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* Fill the message */
  memset((L7_uchar8 *)&msg, 0, sizeof(msg));
  msg.msgId        = snoopMgmdSwitchPortOpen;
  msg.intIfNum     = portId;
  msg.vlanId       = serviceId;
  msg.groupAddress = groupAddr;
  msg.groupAddress = sourceAddr;
  msg.cbHandle     = pSnoopCB;

  /* Send a Port_Open event to the FP */
  LOG_TRACE(LOG_CTX_PTIN_IGMP, "Sending request to FP to open a new port on the switch");
  rc = osapiMessageSend(pSnoopCB->snoopExec->snoopIGMPQueue, &msg, SNOOP_PDU_MSG_SIZE, L7_NO_WAIT, L7_MSG_PRIORITY_NORM);

  return rc;
}

RC_t snooping_port_close(uint32 serviceId, uint32 portId, uint32 groupAddr, uint32 sourceAddr)
{
  L7_RC_t        rc = L7_SUCCESS;
  snoop_cb_t     *pSnoopCB = L7_NULLPTR;
  snoopPDU_Msg_t msg;

  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Context [serviceId:%u portId:%u groupAddr:%08X sourceAddr:%08X]", serviceId, portId, groupAddr, sourceAddr);

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(L7_AF_INET)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* Fill the message */
  memset((L7_uchar8 *)&msg, 0, sizeof(msg));
  msg.msgId        = snoopMgmdSwitchPortClose;
  msg.intIfNum     = portId;
  msg.vlanId       = serviceId;
  msg.groupAddress = groupAddr;
  msg.groupAddress = sourceAddr;
  msg.cbHandle     = pSnoopCB;

  /* Send a Port_Open event to the FP */
  LOG_TRACE(LOG_CTX_PTIN_IGMP, "Sending request to FP to open a new port on the switch");
  rc = osapiMessageSend(pSnoopCB->snoopExec->snoopIGMPQueue, &msg, SNOOP_PDU_MSG_SIZE, L7_NO_WAIT, L7_MSG_PRIORITY_NORM);

  return rc;
}

RC_t snooping_tx_packet(uchar8 *payload, uint32 payloadLength, uint32 serviceId, uint32 portId, uint32 clientId, uchar8 family)
{
  L7_uint16 shortVal, mcastRootVlan;
  L7_uchar8 srcMac[L7_MAC_ADDR_LEN];
  L7_uchar8 destMac[L7_MAC_ADDR_LEN];
  L7_uchar8 packet[L7_MAX_FRAME_SIZE];
  L7_uchar8 *dataPtr = packet;
  L7_uint32 packetLength = payloadLength;
  L7_uint32 dstIpAddr;
  L7_inet_addr_t destIp;

  LOG_TRACE(LOG_CTX_PTIN_IGMP, "Context [payLoad:%p payloadLength:%u serviceId:%u portId:%u clientId:%u family:%u]", payload, payloadLength, serviceId, portId, clientId, family);

  memset(packet, 0x00, L7_MAX_FRAME_SIZE * sizeof(L7_uchar8));

  //Get outter internal vlan
  if( SUCCESS != ptin_evc_intRootVlan_get(serviceId, &mcastRootVlan))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Unable to get mcastRootVlan from serviceId");
    return FAILURE;
  }

  //Get destination MAC from destIpAddr
  dstIpAddr = (L7_uint32) *(payload+16);
  inetAddressSet(L7_AF_INET, &dstIpAddr, &destIp);
  snoopMulticastMacFromIpAddr(&destIp, destMac);

  //Get base MAC address (could be BIA or LAA) and use it as src MAC */
  if (simGetSystemIPMacType() == L7_SYSMAC_BIA)
  {
    simGetSystemIPBurnedInMac(srcMac);
  }
  else
  {
    simGetSystemIPLocalAdminMac(srcMac);
  }

  //Set source and dest MAC in ethernet header
  SNOOP_PUT_DATA(destMac, L7_MAC_ADDR_LEN, dataPtr);    // 6 bytes
  packetLength += L7_MAC_ADDR_LEN;
  SNOOP_PUT_DATA(srcMac, L7_MAC_ADDR_LEN, dataPtr);    // 6 bytes
  packetLength += L7_MAC_ADDR_LEN;

  //OuterVlan
  shortVal = L7_ETYPE_8021Q;
  SNOOP_PUT_SHORT(shortVal, dataPtr);                   // 2 bytes
  packetLength += 2;
  shortVal = ((5 & 0x07)<<13) | (mcastRootVlan & 0x0fff);
  SNOOP_PUT_SHORT(shortVal, dataPtr);                   // 2 bytes
  packetLength += 2;

  //IP Ether type
  shortVal = L7_ETYPE_IP;
  SNOOP_PUT_SHORT(shortVal, dataPtr);                   // 2 bytes
  packetLength += 2;

  //Copy the L3 and above payload to the packet buffer
  memcpy(dataPtr, payload, payloadLength * sizeof(uchar8));

  //Send packet
  LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Sending portId:%u mcastRootVlan:%u packet:%p packetLength:%u family:%u clientId:%u", portId, mcastRootVlan, packet, packetLength, family, clientId);
  snoopPacketSend(portId, mcastRootVlan, 0, packet, packetLength, family, clientId);

  return SUCCESS;
}

