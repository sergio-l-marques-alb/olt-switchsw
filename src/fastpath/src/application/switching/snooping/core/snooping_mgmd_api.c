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
#include "logger.h" 
#include "usmdb_snooping_api.h"
#include "l3_addrdefs.h"
#include "comm_mask.h"
#include "ptin_evc.h"
#include "ptin_igmp.h"
#include "ptin_prot_typeb.h"
#include "snooping_util.h"
#include "snooping_proto.h"
#include "snooping_db.h"

#include "ipc.h"
#include "ptin_msghandler.h"
#include "ptin_intf.h"
#include "ptin_cnfgr.h"
#include "ptin_mgmd_inet_defs.h"
#include "ptin_fpga_api.h"
#include "ptin_debug.h"

/* Static Methods */
#if (!PTIN_BOARD_IS_MATRIX && (defined (IGMP_QUERIER_IN_UC_EVC)))
L7_RC_t ptin_mgmd_send_leaf_packet(uint32 portId, L7_uint16 int_ovlan, L7_uint16 int_ivlan, L7_uchar8 *payload, L7_uint32 payloadLength,uchar8 family, L7_uint client_idx);
#endif
#if (PTIN_BOARD_IS_MATRIX || PTIN_BOARD_IS_LINECARD)
static L7_RC_t __matrix_slotid_get(L7_uint8 matrixType, L7_uint8 *slotId);
static L7_RC_t __matrix_ipaddr_get(L7_uint8 matrixType, L7_uint32 *ipAddr);
#endif
#if PTIN_BOARD_IS_LINECARD
static L7_RC_t __remoteslot_mfdbport_sync(L7_uint8 slotId, L7_uint8 admin, L7_uint32 serviceId, L7_uint32 portId, L7_uint32 groupAddr, L7_uint32 sourceAddr, L7_uint8 groupType);
#endif
#if (PTIN_BOARD_IS_MATRIX || PTIN_BOARD_IS_LINECARD)
static L7_RC_t __matrix_mfdbport_sync(L7_uint8 admin, L7_uint8 matrixType, L7_uint32 serviceId, L7_uint32 slotId, L7_uint32 groupAddr, L7_uint32 sourceAddr, L7_uint8 groupType);
#endif

/* Initialization of the external API struct */
ptin_mgmd_externalapi_t mgmd_external_api = {
  .igmp_admin_set              = &snooping_igmp_admin_set,
  .mld_admin_set               = &snooping_mld_admin_set,
  .cos_set                     = &snooping_cos_set,
  .portList_get                = &snooping_portList_get,
  .portType_get                = &snooping_portType_get,
  .channel_serviceid_get       = &snooping_channel_serviceid_get,
  .clientList_get              = &snooping_clientList_get,
#if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT
  .client_resources_available  = &snooping_client_resources_available,
  .client_resources_allocate   = &snooping_client_resources_allocate,
  .client_resources_release    = &snooping_client_resources_release,
#endif
  .port_open                   = &snooping_port_open,
  .port_close                  = &snooping_port_close,
  .tx_packet                   = &snooping_tx_packet,
};

#if (PTIN_BOARD_IS_MATRIX || PTIN_BOARD_IS_LINECARD)
/**
 * Get active/backup matrix slot id.
 * 
 * @param matrixType : Matrix type (1-active; 0-backup)
 * @param slotId     : Variable where the matrix slotId will be written
 * 
 * @return L7_RC_t 
 *  
 * @note When this method is used in a linecard, the matrixType parameter is ignored and the slotId returned always belongs to the active matrix 
 */
L7_RC_t __matrix_slotid_get(L7_uint8 matrixType, L7_uint8 *slotId)
{
#if PTIN_BOARD_IS_MATRIX
  L7_uint8 activeMatrixSlotId;
  L7_uint8 backupMatrixSlotId;
#endif

  if(slotId == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_PROTB, "Invalid context [slotId:%p]", slotId);
    return L7_FAILURE;
  }

#if PTIN_BOARD_IS_MATRIX
  if((matrixType != 1) && (matrixType != 0))
  {
    LOG_ERR(LOG_CTX_PTIN_PROTB, "Invalid matrix type [%u]", matrixType);
    return L7_FAILURE;
  }

  activeMatrixSlotId = ptin_fgpa_matrixActive_slot();
  backupMatrixSlotId = ptin_fgpa_matrixInactive_slot();

  if(matrixType == 1) //Return active matrix slot ID
  {
    *slotId = activeMatrixSlotId;
  }
  else if(matrixType == 0) //Return backup matrix slot ID
  {
    *slotId = backupMatrixSlotId;
  }
#elif PTIN_BOARD_IS_LINECARD
  *slotId = ptin_fgpa_matrixActive_slot();
#endif

  return L7_SUCCESS;
}

/**
 * Get active/backup matrix IP address.
 * 
 * @param matrixType : Matrix type (1-active; 0-backup)
 * @param ipAddr     : Variable where the matrix IP will be written
 * 
 * @return L7_RC_t 
 *  
 * @note When this method is used in a linecard, the matrixType parameter is ignored and the IP address returned always belongs to the active matrix 
 */
L7_RC_t __matrix_ipaddr_get(L7_uint8 matrixType, L7_uint32 *ipAddr)
{
  L7_uint8 matrixSlotId;

  if(ipAddr == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_PROTB, "Invalid context [ipAddr:%p]", ipAddr);
    return L7_FAILURE;
  }

#if PTIN_BOARD_IS_MATRIX
  if((matrixType != 1) && (matrixType != 0))
  {
    LOG_ERR(LOG_CTX_PTIN_PROTB, "Invalid matrix type [%u]", matrixType);
    return L7_FAILURE;
  }
#endif

  if(L7_SUCCESS != __matrix_slotid_get(matrixType, &matrixSlotId))
  {
    LOG_ERR(LOG_CTX_PTIN_PROTB, "Unable to get matrix slot id");
    return L7_FAILURE;
  }

  if(matrixSlotId == PTIN_SYS_MX1_SLOT)
  {
    *ipAddr = IPC_MX_IPADDR_WORKING;
  }
  else
  {
    *ipAddr = IPC_MX_IPADDR_PROTECTION;
  }

  return L7_SUCCESS;
}
#endif

#if PTIN_BOARD_IS_LINECARD
/**
 * Send CCMSG_MGMD_PORT_SYNC message to a remote slot to sync a MGMD MFDB port.
 * 
 * @param slotId     : Protection slot
 * @param admin      : L7_ENABLE/L7_DISABLE
 * @param serviceId  : Service ID
 * @param portId     : Port ID (intfnum)
 * @param groupAddr  : Group IP
 * @param sourceAddr : Source IP
 * @param groupType  : Group type (0-dynamic; 1-static)
 * 
 * @return L7_RC_t 
 */
L7_RC_t __remoteslot_mfdbport_sync(L7_uint8 slotId, L7_uint8 admin, L7_uint32 serviceId, L7_uint32 portId, L7_uint32 groupAddr, L7_uint32 sourceAddr, L7_uint8 groupType)
{
  msg_HwMgmdPortSync mgmdPortSync = {0};
  L7_uint32          protectionSlotIp = 0xC0A8C800; //192.168.200.X

  /* Determine protection slot/ip/interface */
  protectionSlotIp |= (slotId+1) & 0x000000FF;

  /* Fill the sync structure */
  mgmdPortSync.SlotId     = slotId;
  mgmdPortSync.admin      = admin;
  mgmdPortSync.serviceId  = serviceId;
  mgmdPortSync.portId     = portId;
  mgmdPortSync.groupAddr  = groupAddr;
  mgmdPortSync.sourceAddr = sourceAddr;
  mgmdPortSync.groupType  = groupType;

  LOG_TRACE(LOG_CTX_PTIN_PROTB, "Sending message to card %08X(%u) to set port %u admin to %u for group %08X/%08X", protectionSlotIp, slotId, portId, admin, groupAddr, sourceAddr);

  /* Send the mfdb port configurations to the remote slot */
  if (send_ipc_message(IPC_HW_FASTPATH_PORT, protectionSlotIp, CCMSG_MGMD_PORT_SYNC, (char *)(&mgmdPortSync), NULL, sizeof(mgmdPortSync), NULL) < 0)
  {
    LOG_ERR(LOG_CTX_PTIN_PROTB, "Failed to sync MGMD between active and protection interface");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}
#endif

#if (PTIN_BOARD_IS_MATRIX || PTIN_BOARD_IS_LINECARD)
/**
 * Send CCMSG_MGMD_PORT_SYNC message to a matrix (active or backup) to open/close a port in the MFDB.
 * 
 * @param admin      : L7_ENABLE/L7_DISABLE
 * @param matrixType : Matrix type (1-active; 0-backup)
 * @param serviceId  : Service ID
 * @param slotId     : Slot ID
 * @param groupAddr  : Group IP
 * @param sourceAddr : Source IP
 * @param groupType  : Group type (0-dynamic; 1-static)
 * 
 * @return L7_RC_t 
 */
L7_RC_t __matrix_mfdbport_sync(L7_uint8 admin, L7_uint8 matrixType, L7_uint32 serviceId, L7_uint32 portId, L7_uint32 groupAddr, L7_uint32 sourceAddr, L7_uint8 groupType)
{
  msg_HwMgmdPortSync mgmdPortSync = {0};
  L7_uint32          matrixIpAddr = 0;
  L7_uint8           matrixSlotId;

  /* Determine active/backup matrix slotId and IP address */
  if(L7_SUCCESS != __matrix_slotid_get(matrixType, &matrixSlotId))
  {
    LOG_ERR(LOG_CTX_PTIN_PROTB, "Unable to get matrix slot ID");
    return L7_FAILURE;
  }
  if(L7_SUCCESS != __matrix_ipaddr_get(matrixType, &matrixIpAddr))
  {
    LOG_ERR(LOG_CTX_PTIN_PROTB, "Unable to get matrix IP address");
    return L7_FAILURE;
  }

  /* Fill the sync structure */
  mgmdPortSync.SlotId     = matrixSlotId;
  mgmdPortSync.admin      = admin;
  mgmdPortSync.serviceId  = serviceId;
  mgmdPortSync.portId     = portId;
  mgmdPortSync.groupAddr  = groupAddr;
  mgmdPortSync.sourceAddr = sourceAddr;
  mgmdPortSync.groupType  = groupType;

  LOG_TRACE(LOG_CTX_PTIN_PROTB, "Sending message to matrix %08X(%u) to set port %u admin to %u for group %08X/%08X", matrixIpAddr, matrixSlotId, portId, admin, groupAddr, sourceAddr);

  /* Send the mfdb port configurations to the remote slot */
  if (send_ipc_message(IPC_HW_FASTPATH_PORT, matrixIpAddr, CCMSG_MGMD_PORT_SYNC, (char *)(&mgmdPortSync), NULL, sizeof(mgmdPortSync), NULL) < 0)
  {
    LOG_ERR(LOG_CTX_PTIN_PROTB, "Failed to sync MGMD between active and protection interface");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}
#endif

unsigned int snooping_igmp_admin_set(unsigned char admin)
{
  LOG_TRACE(LOG_CTX_PTIN_IGMP, "Context [admin:%u]", admin);

  // Snooping global activation
  if (L7_SUCCESS != usmDbSnoopAdminModeSet(1, admin, L7_AF_INET))  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error with usmDbSnoopAdminModeSet");
    return FAILURE;
  }
  
  return SUCCESS;
}

unsigned int snooping_mld_admin_set(unsigned char admin)
{
  LOG_TRACE(LOG_CTX_PTIN_IGMP, "Context [admin:%u]", admin);

  // Snooping global activation
  if (L7_SUCCESS != usmDbSnoopAdminModeSet(1, admin, L7_AF_INET6))  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error with usmDbSnoopAdminModeSet");
    return FAILURE;
  }

  return SUCCESS;
}

unsigned int snooping_cos_set(unsigned char cos)
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

unsigned int snooping_portList_get(unsigned int serviceId, ptin_mgmd_port_type_t portType, PTIN_MGMD_PORT_MASK_t *portList)
{
  L7_INTF_MASK_t interfaceBitmap = {{0}};
  L7_uint16      mcastRootVlan;
  L7_RC_t        res = SUCCESS;  

  LOG_TRACE(LOG_CTX_PTIN_IGMP, "Context [serviceId:%u portType:%u portList:%p]", serviceId, portType, portList);

  if( SUCCESS != ptin_evc_intRootVlan_get(serviceId, &mcastRootVlan))
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Unable to get mcastRootVlan from serviceId");
    memcpy(portList, &interfaceBitmap.value, PTIN_MGMD_PORT_MASK_INDICES*sizeof(uchar8));
    return NOT_EXIST;
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
    memcpy(portList, &interfaceBitmap.value, PTIN_MGMD_PORT_MASK_INDICES*sizeof(uchar8));
    return NOT_SUPPORTED;
  }

#if 0
  L7_int32       i;
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
#endif   

  if(SUCCESS != res)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Unable to get port information");
    return FAILURE;
  }

  memcpy(portList, &interfaceBitmap.value, PTIN_MGMD_PORT_MASK_INDICES*sizeof(uchar8));

  return SUCCESS;
}

unsigned int snooping_portType_get(unsigned int serviceId, unsigned int portId, ptin_mgmd_port_type_t *portType)
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
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Port is root");
    *portType = PTIN_MGMD_PORT_TYPE_ROOT;
  }
  else if(SUCCESS == ptin_igmp_clientIntfVlan_validate(portId, mcastRootVlan))
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Port is leaf");
    *portType = PTIN_MGMD_PORT_TYPE_LEAF;
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Unknown port type");
    return FAILURE;
  }

  return SUCCESS;
}

unsigned int snooping_channel_serviceid_get(unsigned int groupAddr, unsigned int sourceAddr, unsigned int *serviceId)
{
  LOG_TRACE(LOG_CTX_PTIN_IGMP, "Context [groupAddr:%08X sourceAddr:%08X serviceId:%p]", groupAddr, sourceAddr, serviceId);

  if(serviceId == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Abnormal context [serviceId:%p]", serviceId);
    return FAILURE;
  }

#if (!PTIN_BOARD_IS_MATRIX) //Conversion from IGMP service to the Multicast service is only performed in the linecards
{
  L7_inet_addr_t groupInetAddr;
  L7_inet_addr_t sourceInetAddr;
  L7_uint16      mcastRootVlan;

  /* Get multicast root vlan */
  inetAddressSet(L7_AF_INET, &groupAddr,  &groupInetAddr);
  inetAddressSet(L7_AF_INET, &sourceAddr, &sourceInetAddr);
  if (ptin_igmp_McastRootVlan_get(&groupInetAddr, &sourceInetAddr, (L7_uint16)-1, &mcastRootVlan)==L7_SUCCESS)
  {
    if (ptin_evc_get_evcIdfromIntVlan(mcastRootVlan, serviceId)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "No EVC associated to internal vlan %u", mcastRootVlan);
      return L7_FAILURE;
    }
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Found serviceID %u associated to the pair {groupAddr,sourceAddr}={%08X,%08X}", *serviceId, groupAddr, sourceAddr);
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Unable to determine serviceID associated to the pair {groupAddr,sourceAddr}={%08X,%08X}", groupAddr, sourceAddr);
    return L7_FAILURE;
  }
}
#endif //(!PTIN_BOARD_IS_MATRIX)

  return SUCCESS;
}

unsigned int snooping_clientList_get(unsigned int serviceId, unsigned int portId, PTIN_MGMD_CLIENT_MASK_t *clientList, unsigned int *noOfClients)
{
  LOG_TRACE(LOG_CTX_PTIN_IGMP, "Context [serviceId:%u portId:%u clientList:%p]", serviceId, portId, clientList);

  memset(clientList->value, 0x00, PTIN_MGMD_CLIENT_BITMAP_SIZE * sizeof(uint8));
  
#if (!PTIN_BOARD_IS_MATRIX) //Since we do not expose any counters for the packets sent from the MX to the LC it does not make sense to increment them on the MGMD module
  if(ptin_igmp_groupclients_bmp_get(serviceId, portId, clientList->value, noOfClients)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Failed to obtain client bitmap [serviceId:%u portId:%u clientList:%p]", serviceId, portId, clientList);
    return FAILURE;
  }
#else
  *noOfClients=0;
#endif

  return SUCCESS;
}

/*Admission Control Feature*/
#if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT

unsigned int snooping_client_resources_available(unsigned int serviceId, unsigned int portId, unsigned int clientId, unsigned int groupAddr, unsigned int sourceAddr)
{
  L7_inet_addr_t inetGroupAddr;
  L7_uint32      channelBandwidth;
  L7_uint32      ptin_port;
  L7_RC_t        rc;

  LOG_TRACE(LOG_CTX_PTIN_IGMP, "Context [serviceId:%u portId:%u groupAddr:0x%08x sourceAddr:0x%08x]", serviceId, portId, groupAddr, sourceAddr);
  
  if (clientId == (L7_uint32) -1)
  {
    return L7_TRUE;
  }
  if (L7_SUCCESS != ptin_intf_intIfNum2port(portId, &ptin_port))
  {
    return L7_FALSE;
  }

  inetAddressSet(L7_AF_INET, &groupAddr , &inetGroupAddr);

  ptin_timer_start(60,"ptin_igmp_channel_bandwidth_get");
  channelBandwidth = ptin_igmp_channel_bandwidth_get(&inetGroupAddr);
  ptin_timer_stop(60);

  if (ptin_debug_igmp_snooping)
  {   
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Channel Bandwidth:%u kbps", channelBandwidth); 
  }
  
#if 0
  ptin_timer_start(61,"ptin_igmp_port_resources_available");
  rc = ptin_igmp_port_resources_available(ptin_port, channelBandwidth);
  ptin_timer_stop(61);

  if (rc != L7_SUCCESS)
  {
    return L7_FALSE;
  }
#endif

  ptin_timer_start(62,"ptin_igmp_multicast_service_resources_available");
  rc = ptin_igmp_multicast_service_resources_available(ptin_port, clientId,serviceId,channelBandwidth);
  ptin_timer_stop(62);

  if (rc != L7_SUCCESS)
  {
    return L7_FALSE;
  }

  ptin_timer_start(63,"ptin_igmp_client_resources_available");
  rc = ptin_igmp_client_resources_available(ptin_port, clientId, channelBandwidth);
  ptin_timer_stop(63);
  return (rc == L7_SUCCESS) ;
}

unsigned int snooping_client_resources_allocate(unsigned int serviceId, unsigned int portId, unsigned int clientId, unsigned int groupAddr, unsigned int sourceAddr)
{
  L7_inet_addr_t inetGroupAddr;
  L7_uint32      channelBandwidth;
  L7_uint32      ptin_port;
  L7_RC_t        rc;
  
  if (L7_SUCCESS != ptin_intf_intIfNum2port(portId, &ptin_port))
  {
    return L7_FAILURE;
  }

  inetAddressSet(L7_AF_INET, &groupAddr , &inetGroupAddr);

  ptin_timer_start(60,"ptin_igmp_channel_bandwidth_get");
  channelBandwidth = ptin_igmp_channel_bandwidth_get(&inetGroupAddr);
  ptin_timer_stop(60);

  if (ptin_debug_igmp_snooping)
  {   
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Channel Bandwidth:%u kbps", channelBandwidth); 
  }

#if 0
  ptin_timer_start(64,"ptin_igmp_port_resources_allocate");
  rc = ptin_igmp_port_resources_allocate(ptin_port, channelBandwidth);
  ptin_timer_stop(64);

  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
#endif
  ptin_timer_start(65,"ptin_igmp_multicast_service_resources_allocate");
  rc = ptin_igmp_multicast_service_resources_allocate(ptin_port, clientId,serviceId,channelBandwidth);
  ptin_timer_stop(65);

  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  ptin_timer_start(66,"ptin_igmp_client_resources_allocate");
  rc = ptin_igmp_client_resources_allocate(ptin_port, clientId, channelBandwidth);
  ptin_timer_stop(66);
  return rc;
}

unsigned int snooping_client_resources_release(unsigned int serviceId, unsigned int portId, unsigned int clientId, unsigned int groupAddr, unsigned int sourceAddr)
{
  L7_inet_addr_t inetGroupAddr;
  L7_uint32      channelBandwidth;
  L7_uint32      ptin_port;
  L7_RC_t        rc;
  
  if (L7_SUCCESS != ptin_intf_intIfNum2port(portId, &ptin_port))
  {
    return L7_FAILURE;
  }

  inetAddressSet(L7_AF_INET, &groupAddr , &inetGroupAddr);

  ptin_timer_start(60,"ptin_igmp_channel_bandwidth_get");
  channelBandwidth = ptin_igmp_channel_bandwidth_get(&inetGroupAddr);
  ptin_timer_stop(60);

  if (ptin_debug_igmp_snooping)
  {   
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Channel Bandwidth:%u kbps", channelBandwidth); 
  }

#if 0
  ptin_timer_start(67,"ptin_igmp_port_resources_release");
  rc = ptin_igmp_port_resources_release(ptin_port, channelBandwidth);
  ptin_timer_stop(67);

  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
#endif

  ptin_timer_start(68,"ptin_igmp_multicast_service_resources_release");
  rc = ptin_igmp_multicast_service_resources_release(ptin_port, clientId,serviceId,channelBandwidth);
  ptin_timer_stop(68);

  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  ptin_timer_start(69,"ptin_igmp_client_resources_release");
  rc = ptin_igmp_client_resources_release(ptin_port, clientId, channelBandwidth);
  ptin_timer_stop(69);
  return rc;
}

#endif //End Admission Control

unsigned int snooping_port_open(unsigned int serviceId, unsigned int portId, unsigned int groupAddr, unsigned int sourceAddr, unsigned char isStatic)
{
  L7_RC_t        rc = L7_SUCCESS;
  snoop_cb_t     *pSnoopCB = L7_NULLPTR;
  snoopPDU_Msg_t msg;
  snoop_eb_t     *pSnoopEB = L7_NULLPTR;

  /*
   * We were forced to implement this method asynchronous from MGMD as the SDK crashes if the mfdb request is made by the MGMD thread. 
   * The SDK exits in an assert that checks for the in_interrupt() method. As no solution was found, an alternative method was implemented. 
   * Instead of directly calling mfdb, MGMD will place a request in the snooping queue, which will eventually be processed. 
   */

  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Context [serviceId:%u portId:%u groupAddr:%08X sourceAddr:%08X isStatic:%u]", serviceId, portId, groupAddr, sourceAddr, isStatic);

  /* Get Snoop Execution Block and Control Block */
  pSnoopEB = snoopEBGet();
  if ((pSnoopCB = snoopCBGet(L7_AF_INET)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* Fill the message */
  memset((L7_uchar8 *)&msg, 0, sizeof(msg));
  msg.msgId         = snoopMgmdSwitchPortOpen;
  msg.intIfNum      = portId;
  msg.vlanId        = serviceId;
  msg.groupAddress  = groupAddr;
  msg.sourceAddress = sourceAddr;
  msg.client_idx    = isStatic; 
  msg.cbHandle      = pSnoopCB;

  
  /* Send a Port_Open event to the FP */
  LOG_TRACE(LOG_CTX_PTIN_IGMP, "Sending request to FP to open a port on the switch");
  if(L7_SUCCESS == (rc = osapiMessageSend(pSnoopCB->snoopExec->snoopIGMPQueue, &msg, SNOOP_PDU_MSG_SIZE, L7_NO_WAIT, L7_MSG_PRIORITY_NORM)))
  {
    if (osapiSemaGive(pSnoopEB->snoopMsgQSema) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to unlock snooping's queue semaphore");
      return L7_FAILURE;
    }
  }

  /*
   * Sync MFDB ports to the protection type-b linecard and backup matrix. 
   * However, do this only for dynamic ports! Static ports are already sent to those cards by the management layer. 
   */
  if(isStatic != L7_TRUE)
  {
#if PTIN_BOARD_IS_MATRIX
    /* Sync the status of this switch port on the backup backup matrix, if it exists */
    if(ptin_fgpa_mx_is_matrixactive())
    {
      __matrix_mfdbport_sync(L7_ENABLE, 0, serviceId, portId, groupAddr, sourceAddr, isStatic);
    }
#elif PTIN_BOARD_IS_LINECARD
    ptin_prottypeb_intf_config_t protTypebIntfConfig = {0};

    /* Sync the status of this switch port on the backup type-b protection port, if it exists */
    ptin_prottypeb_intf_config_get(portId, &protTypebIntfConfig);
    if(protTypebIntfConfig.status == L7_ENABLE)
    {
      __remoteslot_mfdbport_sync(protTypebIntfConfig.pairSlotId, L7_ENABLE, serviceId, protTypebIntfConfig.pairIntfNum, groupAddr, sourceAddr, isStatic);
      __matrix_mfdbport_sync(L7_ENABLE, 1, serviceId, protTypebIntfConfig.pairSlotId, groupAddr, sourceAddr, isStatic);
    }
#endif
  }

  return rc;
}

unsigned int snooping_port_close(unsigned int serviceId, unsigned int portId, unsigned int groupAddr, unsigned int sourceAddr)
{
  L7_RC_t        rc = L7_SUCCESS;
  snoop_cb_t     *pSnoopCB = L7_NULLPTR;
  snoopPDU_Msg_t msg;
  snoop_eb_t     *pSnoopEB = L7_NULLPTR;

  /*
   * We were forced to implement this method asynchronous from MGMD as the SDK crashes if the mfdb request is made by the MGMD thread. 
   * The SDK exits in an assert that checks for the in_interrupt() method. As no solution was found, an alternative method was implemented. 
   * Instead of directly calling mfdb, MGMD will place a request in the snooping queue, which will eventually be processed. 
   */

  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Context [serviceId:%u portId:%u groupAddr:%08X sourceAddr:%08X]", serviceId, portId, groupAddr, sourceAddr);

  /*In L2 we do not support forwarding multicast packets based on the Source Address. 
    To support IGMPv3 protocol we only close the ports if the Source Address is equal to 0x0000.
    If not we ignore the request*/
  if(sourceAddr != PTIN_MGMD_ANY_IPv4_HOST)
  {
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Ignoring Port Close Request!");
    return rc;
  }

  /* Get Snoop Execution Block and Control Block */
  pSnoopEB = snoopEBGet();
  if ((pSnoopCB = snoopCBGet(L7_AF_INET)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* Fill the message */
  memset((L7_uchar8 *)&msg, 0, sizeof(msg));
  msg.msgId         = snoopMgmdSwitchPortClose;
  msg.intIfNum      = portId;
  msg.vlanId        = serviceId;
  msg.groupAddress  = groupAddr;
  msg.sourceAddress = sourceAddr;
  msg.cbHandle      = pSnoopCB;

  /* Send a Port_Close event to the FP */
  LOG_TRACE(LOG_CTX_PTIN_IGMP, "Sending request to FP to close a port");
  if(L7_SUCCESS == (rc = osapiMessageSend(pSnoopCB->snoopExec->snoopIGMPQueue, &msg, SNOOP_PDU_MSG_SIZE, L7_NO_WAIT, L7_MSG_PRIORITY_NORM)))
  {
    if (osapiSemaGive(pSnoopEB->snoopMsgQSema) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to unlock snooping's queue semaphore");
      return L7_FAILURE;
    }
  }

#if PTIN_BOARD_IS_MATRIX
  /* Sync the status of this switch port on the backup backup matrix, if it exists */
  if(ptin_fgpa_mx_is_matrixactive())
  {
    __matrix_mfdbport_sync(L7_DISABLE, 0, serviceId, portId, groupAddr, sourceAddr, L7_FALSE);
  }
#elif PTIN_BOARD_IS_LINECARD
  ptin_prottypeb_intf_config_t protTypebIntfConfig = {0};

  /* Sync the status of this switch port on the backup type-b protection port, if it exists */
  ptin_prottypeb_intf_config_get(portId, &protTypebIntfConfig);
  if(protTypebIntfConfig.status == L7_ENABLE)
  {
    __remoteslot_mfdbport_sync(protTypebIntfConfig.pairSlotId, L7_DISABLE, serviceId, protTypebIntfConfig.pairIntfNum, groupAddr, sourceAddr, L7_FALSE);
    __matrix_mfdbport_sync(L7_DISABLE, 1, serviceId, protTypebIntfConfig.pairSlotId, groupAddr, sourceAddr, L7_FALSE);
  }
#endif

  return rc;
}

unsigned int snooping_tx_packet(unsigned char *payload, unsigned int payloadLength, unsigned int serviceId, unsigned int portId, unsigned int clientId, unsigned char family)
{
  L7_uint16             shortVal;
  L7_uchar8             srcMac[L7_MAC_ADDR_LEN];
  L7_uchar8             destMac[L7_MAC_ADDR_LEN];
  L7_uchar8             packet[L7_MAX_FRAME_SIZE];
  L7_uchar8            *dataPtr;
  L7_uint32             packetLength = payloadLength;
  L7_uint32             dstIpAddr;
  L7_inet_addr_t        destIp;
  L7_uint32             activeState;  
  L7_uint16             int_ovlan; 
  L7_uint16             int_ivlan    = 0; 
  ptin_IgmpProxyCfg_t   igmpCfg;
   
  LOG_TRACE(LOG_CTX_PTIN_IGMP, "Context [payLoad:%p payloadLength:%u serviceId:%u portId:%u clientId:%u family:%u]", payload, payloadLength, serviceId, portId, clientId, family);

#if PTIN_BOARD_IS_MATRIX
  /* Do nothing for slave matrix */
  if (!ptin_fgpa_mx_is_matrixactive())
  {
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP,"Silently ignoring packet transmission. I'm a Slave Matrix [portId=%u serviceId=%u]",portId, serviceId );
    return SUCCESS;
  }
#else
#if PTIN_BOARD_IS_LINECARD
  ptin_prottypeb_intf_config_t protTypebIntfConfig = {0};

  /* Get  the protection status of this switch port */
  ptin_prottypeb_intf_config_get(portId, &protTypebIntfConfig);
  if( protTypebIntfConfig.intfRole != PROT_TYPEB_ROLE_NONE &&  protTypebIntfConfig.status != L7_ENABLE)
  {
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP,"Silently ignoring packet transmission. I'm a Protection Port [portId=%u serviceId=%u]",portId, serviceId );
    return SUCCESS;
  }
#endif
#endif

  //Ignore if the port has link down
  if ( (nimGetIntfActiveState(portId, &activeState) != L7_SUCCESS) || (activeState != L7_ACTIVE) )
  {
    if (ptin_debug_igmp_snooping)
      LOG_NOTICE(LOG_CTX_PTIN_IGMP,"Silently ignoring packet transmission. Outgoing interface [portId=%u serviceId=%u] is down!",portId,serviceId );    
    return SUCCESS;
  }

  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get__snooping_old(&igmpCfg) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations");        
  }
  //Workaround to support Group Specific Queries; IPv6 is not complaint with this approach!       
  #if (!PTIN_BOARD_IS_MATRIX && (defined (IGMP_QUERIER_IN_UC_EVC)))
  ptin_mgmd_port_type_t portType;
  L7_uint32             groupAddress;  
  if (snooping_portType_get(serviceId, portId, &portType) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Unable to get port type from int_ovlan [%u] portId [%u]",serviceId,portId);
    return FAILURE;
  }
  //Get Group Address
  groupAddress=*((L7_uint32*) (payload+28));   
  //We only get the intRootVLAN here for the General Query and for the Membership Reports
  //For Group Specific Queries we use the  ptin_mgmd_send_leaf_packet to obtain the intRootVLAN
  if ( portType == PTIN_MGMD_PORT_TYPE_ROOT || groupAddress==0x00)
  #endif
  {   
    //Get outter internal vlan
    if( SUCCESS != ptin_evc_intRootVlan_get(serviceId, &int_ovlan))
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Unable to get mcastRootVlan from serviceId");
      return FAILURE;
    }
  }
  #if (!PTIN_BOARD_IS_MATRIX && (defined (IGMP_QUERIER_IN_UC_EVC)))
  else
  {
    if (clientId != (unsigned int) -1)
    {
      L7_uint32 ptin_port;    

      /* Convert to ptin_port format */
      if (ptin_intf_intIfNum2port(portId, &ptin_port) != L7_SUCCESS || ptin_port >= PTIN_SYSTEM_N_INTERF)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Cannot convert intIfNum %u to ptin_port format", portId);
        return L7_FAILURE;
      }

      if( igmp_intVlan_from_clientId_get(ptin_port, clientId, &int_ovlan) != L7_SUCCESS  
          || int_ovlan<PTIN_VLAN_MIN || int_ovlan>PTIN_VLAN_MAX)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Cannot obtain int_ovlan (%u) from client id:%u", int_ovlan, clientId);              
        return L7_FAILURE;
      }
      if (ptin_debug_igmp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_IGMP,"Obtained int_ovlan (%u) from client id:%u", int_ovlan, clientId);  
    }     
  }
  #endif

  //Get destination MAC from destIpAddr
  dstIpAddr = *((L7_uint32*) (payload+16));
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
  dataPtr = packet;
  memset(packet, 0x00, L7_MAX_FRAME_SIZE * sizeof(L7_uchar8));
  SNOOP_PUT_DATA(destMac, L7_MAC_ADDR_LEN, dataPtr);    // 6 bytes
  packetLength += L7_MAC_ADDR_LEN;
  SNOOP_PUT_DATA(srcMac, L7_MAC_ADDR_LEN, dataPtr);    // 6 bytes
  packetLength += L7_MAC_ADDR_LEN;

  //OuterVlan
  shortVal = L7_ETYPE_8021Q;
  SNOOP_PUT_SHORT(shortVal, dataPtr);                   // 2 bytes
  packetLength += 2;
  shortVal = ((igmpCfg.igmp_cos & 0x07)<<13) | (int_ovlan & 0x0fff);
  SNOOP_PUT_SHORT(shortVal, dataPtr);                   // 2 bytes
  packetLength += 2;

  //IP Ether type
  shortVal = L7_ETYPE_IP;
  SNOOP_PUT_SHORT(shortVal, dataPtr);                   // 2 bytes
  packetLength += 2;

  //Copy the L3 and above payload to the packet buffer
  memcpy(dataPtr, payload, payloadLength * sizeof(uchar8));

  #if (!PTIN_BOARD_IS_MATRIX && (defined (IGMP_QUERIER_IN_UC_EVC)))
  if ( portType == PTIN_MGMD_PORT_TYPE_ROOT )
  #endif
  {  
    //Send packet
    snoopPacketSend(portId, int_ovlan, int_ivlan, packet, packetLength, family, clientId);
  }
  #if (!PTIN_BOARD_IS_MATRIX && (defined (IGMP_QUERIER_IN_UC_EVC)))
  else //To support sending one Membership Query Message per ONU 
  {
    if (groupAddress != 0x0 ) //Membership Group or Group and Source Specific Query Message
    { 
      if (clientId == (unsigned int) -1)//No Client Id is provided
      {        
        mgmdQueryInstances_t *mgmdQueryInstancesPtr=L7_NULLPTR;
      
        L7_uint32             mgmdNumberOfQueryInstances;
        L7_uint32             numberOfQueriesSent=0;

        mgmdQueryInstancesPtr = ptin_mgmd_query_instances_get(&mgmdNumberOfQueryInstances);
        if ((mgmdNumberOfQueryInstances>0 && mgmdQueryInstancesPtr==L7_NULLPTR) || mgmdNumberOfQueryInstances>=PTIN_SYSTEM_N_EVCS)
        {
          LOG_WARNING(LOG_CTX_PTIN_IGMP,"Either mgmdNumberOfQueryInstances [%u] >= PTIN_SYSTEM_N_EVCS [%u] or mgmdQueryInstances=%p",mgmdNumberOfQueryInstances,PTIN_SYSTEM_N_EVCS,mgmdQueryInstancesPtr);
          mgmdNumberOfQueryInstances=0;
          return SUCCESS;
        }

        if (ptin_debug_igmp_snooping)
          LOG_TRACE(LOG_CTX_PTIN_IGMP,"Going to send %u Group Specific Queries",mgmdNumberOfQueryInstances);
      
        while(mgmdQueryInstancesPtr!=L7_NULLPTR)
        {        
          if (mgmdQueryInstancesPtr->inUse==L7_TRUE)
          {
            ++numberOfQueriesSent;
            //Get outter internal vlan
            if( SUCCESS != ptin_evc_intRootVlan_get(mgmdQueryInstancesPtr->UcastEvcId, &int_ovlan))
            {
              LOG_ERR(LOG_CTX_PTIN_IGMP,"Unable to get mcastRootVlan from serviceId");
              return FAILURE;
            }          
            ptin_mgmd_send_leaf_packet(portId, int_ovlan, int_ivlan, packet, packetLength, family, clientId);
          }
          if(numberOfQueriesSent>=mgmdNumberOfQueryInstances)
          {          
            break;
          }
          mgmdQueryInstancesPtr++;     
        }
      }
      else//Client Id is provided
      {
        //Send packet
        snoopPacketSend(portId, int_ovlan, int_ivlan, packet, packetLength, family, clientId);
      }
    }
    else //General Query
    {
      ptin_mgmd_send_leaf_packet(portId, int_ovlan, int_ivlan, packet, packetLength, family, clientId);
    }
  }
  #endif
  
  return SUCCESS;
}

#if (!PTIN_BOARD_IS_MATRIX && (defined (IGMP_QUERIER_IN_UC_EVC)))
L7_RC_t ptin_mgmd_send_leaf_packet(uint32 portId, L7_uint16 int_ovlan, L7_uint16 int_ivlan, L7_uchar8 *payload, L7_uint32 payloadLength,uchar8 family, L7_uint client_idx)
{
  ptin_HwEthEvcFlow_t   clientFlow;
  L7_RC_t               rc;
  L7_uchar8             packet[L7_MAX_FRAME_SIZE];
  L7_uint32             packetLength;

  /* To get the first client */
  memset(&clientFlow, 0x00, sizeof(clientFlow));
  do
  {
    //Copy the payload  to the packet buffer
    memcpy(packet, payload, payloadLength);
    packetLength=payloadLength;

    #if (defined IGMP_QUERIER_IN_UC_EVC)
    {
      /* First client/flow */
       rc = ptin_evc_vlan_client_next(int_ovlan, portId, &clientFlow, &clientFlow);

      /* Internal vlans */
      int_ivlan = clientFlow.int_ivid;

      if (ptin_debug_igmp_snooping)
        LOG_TRACE(LOG_CTX_PTIN_IGMP,"rc=%d", rc);

      /* if success, use next cvlan */
      if (rc == L7_SUCCESS)
      {
        /* If this client is not an IGMP client, goto next one */
        if (!(clientFlow.flags & PTIN_EVC_MASK_IGMP_PROTOCOL))
          continue;

#if 0//Client Id is -1 for packets sent to the leaf ports
        /* Get client index */
        if (clientFlow.int_ivid != 0)
        {
          /* Get related client index */
          if (ptin_igmp_clientIndex_get(portId, int_ovlan, int_ivlan, L7_NULLPTR, &client_idx)!=L7_SUCCESS)
          {
            client_idx = (L7_uint) -1;
          }
          if (ptin_debug_igmp_snooping)
          {
            LOG_TRACE(LOG_CTX_PTIN_IGMP,"Packet will be transmited for client cvlan=%u (client_idx=%u) in intIfNum=%u (intVlan=%u)",
                      int_ivlan, client_idx, portId, int_ovlan);
          }
        }
#endif
      }
      /* If clients are not supported, used null inner vlan */
      else if ( rc == L7_NOT_SUPPORTED )
      {
        int_ivlan = 0;
        client_idx = (L7_uint)-1;
        if (ptin_debug_igmp_snooping)
        {
          LOG_TRACE(LOG_CTX_PTIN_IGMP,"Packet will be transmited for intIfNum=%u (intVlan=%u)", portId, int_ovlan);
        }
      }
      else
      {
        /* An error ocurred */
        if (ptin_debug_igmp_snooping)
        {
          LOG_TRACE(LOG_CTX_PTIN_IGMP,"No more transmissions for intIfNum=%u (intVlan=%u), rc=%u", portId, int_ovlan, rc);
        }
        break;
      }
    }
    #else
    {
      /* Standard querier, with no inner vlan (clients querier not supported) */
      int_ivlan = 0;
      rc = L7_NOT_SUPPORTED;
    }
    #endif
    /* Only transmit, if IGMP flag is active */
    if (rc==L7_NOT_SUPPORTED || clientFlow.flags & PTIN_EVC_MASK_IGMP_PROTOCOL)
    {
      //Send packet
      snoopPacketSend(portId, int_ovlan, int_ivlan, packet, packetLength, family, client_idx);
    }
  } while (rc==L7_SUCCESS);   /* Next client? */  
  return rc;
}
#endif

static msg_SnoopSyncReply_t snoopSyncReply[IPCLIB_MAX_MSGSIZE/sizeof(msg_SnoopSyncReply_t)];

#if PTIN_BOARD_IS_MATRIX 
L7_RC_t ptin_snoop_sync_mx_process_request(L7_uint16 vlanId, L7_uint32 groupAddr)
{
  L7_uint32                maxNumberOfSnoopEntries  = IPCLIB_MAX_MSGSIZE/sizeof(msg_SnoopSyncReply_t); //IPC buffer size / struct size
  L7_uint32                numberOfSnoopEntries     = avlTreeCount(&(snoopEBGet()->snoopAvlTree));  
  L7_uint32                intIfNum;
  L7_uint16                internalRootVlan = (L7_uint16) -1;
  L7_uint16                internalRootVlanTmp;
  L7_uint32                serviceId        = 0;//Invalid Extended Service Id
  L7_uint32                channel;
  
  snoopInfoData_t         *snoopInfoData;
  snoopInfoDataKey_t       snoopInfoDataKey;
  snoopInfoDataKey_t      *snoopInfoDataKeyPtr;

  L7_uint32                ipAddr;
  
  if(!ptin_fgpa_mx_is_matrixactive())//I'm Protection
  { 
    LOG_NOTICE(LOG_CTX_PTIN_PROTB, "Silently Ignoring Snoop Sync Request. I'm a protection Matrix!");
    return L7_SUCCESS;
  }
  else
  {
    /* MX board IP address */
    ipAddr = IPC_MX_IPADDR;
  }

  if (numberOfSnoopEntries==0)
  {
    LOG_NOTICE(LOG_CTX_PTIN_PROTB, "Silently Ignoring Snoop Sync Request. Snoop Table is Empty");
    return L7_SUCCESS;
  }
  else
  {
    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_PROTB,"Max Number of MAC MFDB Entries:%u",L7_MFDB_MAX_MAC_ENTRIES);
    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_PROTB,"Number of Existing Snoop Entries (%u) | Maximum number of snoop entries (%u)",numberOfSnoopEntries,maxNumberOfSnoopEntries);

    //Initialize SnoopSyncReply Structure
    if(numberOfSnoopEntries<maxNumberOfSnoopEntries)
    {
      memset(snoopSyncReply, 0x00, numberOfSnoopEntries*sizeof(msg_SnoopSyncReply_t));
    }
    else
    {      
      memset(snoopSyncReply, 0x00, maxNumberOfSnoopEntries*sizeof(msg_SnoopSyncReply_t));
    }    
    numberOfSnoopEntries=0;
  }
   
  /*First Snoop Entry*/
  if(vlanId==0 && groupAddr==0)
  {
    memset(&snoopInfoDataKey,0x00,sizeof(snoopInfoDataKey_t));
  }
  else
  {
    /*IPv4 Support*/
    snoopInfoDataKey.family=L7_AF_INET;

    snoopInfoDataKey.vlanIdMacAddr[0]                       = (L7_uchar8) ((vlanId>>8) & 0xFF);
    snoopInfoDataKey.vlanIdMacAddr[1]                       = (L7_uchar8) (vlanId & 0xFF);
    snoopInfoDataKey.vlanIdMacAddr[L7_FDB_IVL_ID_LEN+0] = 0x01;
    snoopInfoDataKey.vlanIdMacAddr[L7_FDB_IVL_ID_LEN+1] = 0x00;
    snoopInfoDataKey.vlanIdMacAddr[L7_FDB_IVL_ID_LEN+2] = 0x5E;
    snoopInfoDataKey.vlanIdMacAddr[L7_FDB_IVL_ID_LEN+3] = (L7_uchar8) ((groupAddr>>16) & 0x7F);
    snoopInfoDataKey.vlanIdMacAddr[L7_FDB_IVL_ID_LEN+4] = (L7_uchar8) ((groupAddr>> 8) & 0xFF);
    snoopInfoDataKey.vlanIdMacAddr[L7_FDB_IVL_ID_LEN+5] = (L7_uchar8) ((groupAddr) & 0xFF);
  }
   
  while ( ( snoopInfoData = (snoopInfoData_t *) avlSearchLVL7( &(snoopEBGet()->snoopAvlTree), (void *) &snoopInfoDataKey, AVL_NEXT) ) != L7_NULL && 
          numberOfSnoopEntries<maxNumberOfSnoopEntries)
  {
    /* Prepare next key */
    memcpy( &snoopInfoDataKey, &snoopInfoData->snoopInfoDataKey, sizeof(snoopInfoDataKey_t) );

    snoopInfoDataKeyPtr  = &snoopInfoData->snoopInfoDataKey;    

    internalRootVlanTmp = (L7_uint16) snoopInfoDataKeyPtr->vlanIdMacAddr[0]<<8 | (L7_uint16) snoopInfoDataKeyPtr->vlanIdMacAddr[1];

    if( internalRootVlan != internalRootVlanTmp)
    {
      internalRootVlan = internalRootVlanTmp;

      if (ptin_evc_get_evcIdfromIntVlan(internalRootVlan, &serviceId) != L7_SUCCESS)
      {
        LOG_WARNING(LOG_CTX_PTIN_IGMP,"  Failed to obtain extended service Id from internal root vlan [internalRootVlan=%u]",internalRootVlan);
        continue;
      }
    }
    
    if (ptin_debug_igmp_snooping)
    {
       /* Global information */    
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"  Family=%s     Vlan=%-4u MAC=%02x:%02x:%02x:%02x:%02x:%02x   %s",
           ((snoopInfoDataKeyPtr->family==L7_AF_INET) ? "IPv4" : "IPv6"),
           internalRootVlan,
           snoopInfoDataKeyPtr->vlanIdMacAddr[L7_FDB_IVL_ID_LEN+0],
           snoopInfoDataKeyPtr->vlanIdMacAddr[L7_FDB_IVL_ID_LEN+1],
           snoopInfoDataKeyPtr->vlanIdMacAddr[L7_FDB_IVL_ID_LEN+2],
           snoopInfoDataKeyPtr->vlanIdMacAddr[L7_FDB_IVL_ID_LEN+3],
           snoopInfoDataKeyPtr->vlanIdMacAddr[L7_FDB_IVL_ID_LEN+4],
           snoopInfoDataKeyPtr->vlanIdMacAddr[L7_FDB_IVL_ID_LEN+5],
           ((snoopInfoData->staticGroup) ? "Static" : "Dynamic"));    
      /* Ports information */
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"  IntfNUm Ports information:");
      for (intIfNum = 1; intIfNum <= L7_MAX_INTERFACE_COUNT; intIfNum++)
      {
        if (L7_INTF_ISMASKBITSET(snoopInfoData->snoopGrpMemberList,intIfNum))
        {
          LOG_TRACE(LOG_CTX_PTIN_IGMP," %u",intIfNum);
        }
      }
    }

    /* Channels information */
    if (ptin_debug_igmp_snooping)
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"  Channels information:");
    for (channel=0; channel<SNOOP_MAX_CHANNELS_PER_SNOOP_ENTRY; channel++)
    {
      //Only Sync Active Channels
      if (!snoopInfoData->channel_list[channel].active)  continue;

      //Only Sync Dynamic Channels
      if (snoopInfoData->staticGroup == L7_FALSE) continue;

      snoopSyncReply[numberOfSnoopEntries].serviceId           = serviceId;      
      snoopSyncReply[numberOfSnoopEntries].groupAddr           = snoopInfoData->channel_list[channel].ipAddr;
      snoopSyncReply[numberOfSnoopEntries].isStatic            = snoopInfoData->staticGroup;
      snoopSyncReply[numberOfSnoopEntries].numberOfActivePorts = snoopInfoData->global.number_of_ports;
     
      memcpy(&snoopSyncReply[numberOfSnoopEntries].snoopGrpMemberList, &snoopInfoData->snoopGrpMemberList, sizeof(snoopInfoData->snoopGrpMemberList)); 

      if (ptin_debug_igmp_snooping)
      {
        LOG_TRACE(LOG_CTX_PTIN_IGMP,"    Channel#%-2u:       IpAddr=%u.%u.%u.%u",channel,
             (snoopSyncReply[numberOfSnoopEntries].groupAddr>>24) & 0xff,
             (snoopSyncReply[numberOfSnoopEntries].groupAddr>>16) & 0xff,
             (snoopSyncReply[numberOfSnoopEntries].groupAddr>>8) & 0xff,
             snoopSyncReply[numberOfSnoopEntries].groupAddr & 0xff);            
      }
      /* Next Snoop entry */
      numberOfSnoopEntries++;       
    }
  }

  if (numberOfSnoopEntries>0)
  {    
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Sending a Snoop Sync Reply Message to ipAddr:%08X with %u snoop Entries  to sync the protection matrix",ipAddr, numberOfSnoopEntries);
    if(vlanId==0 && groupAddr==0)
    {
      LOG_DEBUG(LOG_CTX_PTIN_MSG, "Remaining Snoop Entries to be Sync:%u",avlTreeCount(&(snoopEBGet()->snoopAvlTree))-numberOfSnoopEntries);     
    }
    
    /*Send the snoop sync request to the protection matrix */  
    if (send_ipc_message(IPC_HW_FASTPATH_PORT, ipAddr, CCMSG_MGMD_SNOOP_SYNC_REPLY, (char *)(&snoopSyncReply), NULL, numberOfSnoopEntries*sizeof(msg_SnoopSyncReply_t), NULL) < 0)
    {
      LOG_ERR(LOG_CTX_PTIN_PROTB, "Failed to send Snoop Sync Reply Message");
      return L7_FAILURE;
    }
  }
  else
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "No more Snoop Entries Remaining to be Sync");    
  }

  return L7_SUCCESS; 
}

#else//!PTIN_BOARD_IS_MATRIX 
L7_RC_t ptin_snoop_sync_port_process_request(L7_uint16 vlanId, L7_uint32 groupAddr, L7_uint32 portId)
{
  L7_uint32                      maxNumberOfSnoopEntries  = IPCLIB_MAX_MSGSIZE/sizeof(msg_SnoopSyncReply_t); //IPC buffer size / struct size
  L7_uint32                      numberOfSnoopEntries     = avlTreeCount(&(snoopEBGet()->snoopAvlTree));  
  L7_uint32                      channel;
  L7_uint16                      internalRootVlan = (L7_uint16) -1;
  L7_uint16                      internalRootVlanTmp;
  L7_uint32                      serviceId        = 0;//Invalid Extended Service Id
    
  snoopInfoData_t               *snoopInfoData;
  snoopInfoDataKey_t             snoopInfoDataKey;
  snoopInfoDataKey_t            *snoopInfoDataKeyPtr;
  L7_uint32                      ipAddr;  
                             
  ptin_prottypeb_intf_config_t   protTypebIntfConfig = {0};
  
  /* Get the configuration of this portId for the Type B Scheme Protection */
  ptin_prottypeb_intf_config_get(portId, &protTypebIntfConfig);   

  if(protTypebIntfConfig.status == L7_ENABLE) //I'm Working
  {
    /* Determine the IP address of the protection port/slot */
    ipAddr = 0xC0A8C800 /*192.168.200.X*/ | ((protTypebIntfConfig.pairSlotId+1) & 0x000000FF); 
  }
  else
  { //I'm Protection
    LOG_NOTICE(LOG_CTX_PTIN_PROTB, "Silently Ignoring Snoop Sync Request. I'm a Protection Port");
    return L7_SUCCESS;
  }

  if (numberOfSnoopEntries==0)
  {
    LOG_NOTICE(LOG_CTX_PTIN_PROTB, "Silently Ignoring Snoop Sync Request. Snoop Table is Empty");
    return L7_SUCCESS;
  }
  else
  {
    LOG_TRACE(LOG_CTX_PTIN_PROTB,"Max Number of MAC MFDB Entries:%u",L7_MFDB_MAX_MAC_ENTRIES);

    LOG_TRACE(LOG_CTX_PTIN_PROTB,"Number of Existing Snoop Entries (%u) | Maximum number of snoop entries (%u)",numberOfSnoopEntries,maxNumberOfSnoopEntries);
    //Initialize SnoopSyncReply Structure
    if(numberOfSnoopEntries<maxNumberOfSnoopEntries)
    {
      memset(snoopSyncReply, 0x00, numberOfSnoopEntries*sizeof(msg_SnoopSyncReply_t));
    }
    else
    {      
      memset(snoopSyncReply, 0x00, maxNumberOfSnoopEntries*sizeof(msg_SnoopSyncReply_t));
    }    
    numberOfSnoopEntries=0;
  }
  
  

  /*First Snoop Entry*/
  if(vlanId==0 && groupAddr==0)
  {
    memset(&snoopInfoDataKey,0x00,sizeof(snoopInfoDataKey_t));
  }
  else
  {
    /*IPv4 Support*/
    snoopInfoDataKey.family=L7_AF_INET;

    snoopInfoDataKey.vlanIdMacAddr[0]                       = (L7_uchar8) ((vlanId>>8) & 0xFF);
    snoopInfoDataKey.vlanIdMacAddr[1]                       = (L7_uchar8) (vlanId & 0xFF);
    snoopInfoDataKey.vlanIdMacAddr[L7_FDB_IVL_ID_LEN+0] = 0x01;
    snoopInfoDataKey.vlanIdMacAddr[L7_FDB_IVL_ID_LEN+1] = 0x00;
    snoopInfoDataKey.vlanIdMacAddr[L7_FDB_IVL_ID_LEN+2] = 0x5E;
    snoopInfoDataKey.vlanIdMacAddr[L7_FDB_IVL_ID_LEN+3] = (L7_uchar8) ((groupAddr>>16) & 0x7F);
    snoopInfoDataKey.vlanIdMacAddr[L7_FDB_IVL_ID_LEN+4] = (L7_uchar8) ((groupAddr>> 8) & 0xFF);
    snoopInfoDataKey.vlanIdMacAddr[L7_FDB_IVL_ID_LEN+5] = (L7_uchar8) ((groupAddr) & 0xFF);
  }
   
  while ( ( snoopInfoData = (snoopInfoData_t *) avlSearchLVL7( &(snoopEBGet()->snoopAvlTree), (void *) &snoopInfoDataKey, AVL_NEXT) ) != L7_NULL && 
          numberOfSnoopEntries<maxNumberOfSnoopEntries)
  {
    /* Prepare next key */
    memcpy( &snoopInfoDataKey, &snoopInfoData->snoopInfoDataKey, sizeof(snoopInfoDataKey_t) );

    snoopInfoDataKeyPtr  = &snoopInfoData->snoopInfoDataKey;    

    if (L7_INTF_ISMASKBITSET(snoopInfoData->snoopGrpMemberList,portId))
    {
      internalRootVlanTmp = (L7_uint16) snoopInfoDataKeyPtr->vlanIdMacAddr[0]<<8 | (L7_uint16) snoopInfoDataKeyPtr->vlanIdMacAddr[1];

      if (internalRootVlan != internalRootVlanTmp)
      {
        internalRootVlan = internalRootVlanTmp;

        if (ptin_evc_get_evcIdfromIntVlan(internalRootVlan, &serviceId) != L7_SUCCESS)
        {
          LOG_WARNING(LOG_CTX_PTIN_IGMP,"  Failed to obtain extended service Id from internal root vlan [internalRootVlan=%u]",internalRootVlan);
          continue;
        }
      }

      /* Global information */    
      LOG_TRACE(LOG_CTX_PTIN_PROTB,"  Family=%s     Vlan=%-4u MAC=%02x:%02x:%02x:%02x:%02x:%02x   %s",
             ((snoopInfoDataKeyPtr->family==L7_AF_INET) ? "IPv4" : "IPv6"),
             internalRootVlan,
             snoopInfoDataKeyPtr->vlanIdMacAddr[L7_FDB_IVL_ID_LEN+0],
             snoopInfoDataKeyPtr->vlanIdMacAddr[L7_FDB_IVL_ID_LEN+1],
             snoopInfoDataKeyPtr->vlanIdMacAddr[L7_FDB_IVL_ID_LEN+2],
             snoopInfoDataKeyPtr->vlanIdMacAddr[L7_FDB_IVL_ID_LEN+3],
             snoopInfoDataKeyPtr->vlanIdMacAddr[L7_FDB_IVL_ID_LEN+4],
             snoopInfoDataKeyPtr->vlanIdMacAddr[L7_FDB_IVL_ID_LEN+5],
             ((snoopInfoData->staticGroup) ? "Static" : "Dynamic"));               
      
      /* Channels information */
      LOG_TRACE(LOG_CTX_PTIN_PROTB,"  Channels information:");
      for (channel=0; channel<SNOOP_MAX_CHANNELS_PER_SNOOP_ENTRY; channel++)
      {
        //Only Sync Active Channels
        if (!snoopInfoData->channel_list[channel].active)  continue;

        //Only Sync Dynamic Channels
        if (snoopInfoData->staticGroup == L7_FALSE) continue;

        snoopSyncReply[numberOfSnoopEntries].serviceId           = serviceId;      
        snoopSyncReply[numberOfSnoopEntries].groupAddr           = snoopInfoData->channel_list[channel].ipAddr;
        snoopSyncReply[numberOfSnoopEntries].isStatic            = snoopInfoData->staticGroup;    
        snoopSyncReply[numberOfSnoopEntries].portId              = protTypebIntfConfig.pairIntfNum;

        LOG_TRACE(LOG_CTX_PTIN_PROTB,"    Channel#%-2u:       IpAddr=%u.%u.%u.%u",channel,
               (snoopSyncReply[numberOfSnoopEntries].groupAddr>>24) & 0xff,
               (snoopSyncReply[numberOfSnoopEntries].groupAddr>>16) & 0xff,
               (snoopSyncReply[numberOfSnoopEntries].groupAddr>>8) & 0xff,
               snoopSyncReply[numberOfSnoopEntries].groupAddr & 0xff);            
          /* Next Snoop entry */
        numberOfSnoopEntries++;       
      }
    }
  }

  if (numberOfSnoopEntries>0)
  {
    
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Sending a Snoop Sync Reply Message ipAddr:%08X with %u Snoop Entries  to sync slot/port:%u/%u",ipAddr, numberOfSnoopEntries, protTypebIntfConfig.pairSlotId, protTypebIntfConfig.pairIntfNum);
    /*Send the snoop sync request to the protection matrix */  
    if (send_ipc_message(IPC_HW_FASTPATH_PORT, ipAddr, CCMSG_MGMD_SNOOP_SYNC_REPLY, (char *)(&snoopSyncReply), NULL, numberOfSnoopEntries*sizeof(msg_SnoopSyncReply_t), NULL) < 0)
    {
      LOG_ERR(LOG_CTX_PTIN_PROTB, "Failed to send Snoop Sync Reply Message");
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS; 
}

#endif//!PTIN_BOARD_IS_MATRIX 

/**
* @purpose This method is used to verify  if the
*          Mgmd Lib is alive or not
*
* @return RC_t
*
* @notes If Mgmd is alive L7_SUCCESS is returned. If not it is
*        returned an error code (e.g. L7_FAILURE).
*/
L7_RC_t ptin_igmp_mgmd_status_get(void)
{
  PTIN_MGMD_EVENT_t            inEventMsg  = {0}, outEventMsg = {0};
  PTIN_MGMD_EVENT_CTRL_t       ctrlResMsg  = {0};
  PTIN_MGMD_CTRL_MGMD_STATUS_t mgmdStatus  = {0};;

  /* Create and send a PTIN_MGMD_EVENT_CTRL_STATUS_GET event to MGMD */
  ptin_mgmd_event_ctrl_create(&inEventMsg, PTIN_MGMD_EVENT_CTRL_STATUS_GET, rand(), 0, ptinMgmdTxQueueId, (void*) &mgmdStatus, (uint32) sizeof(PTIN_MGMD_CTRL_MGMD_STATUS_t));
  ptin_mgmd_sendCtrlEvent(&inEventMsg, &outEventMsg);

  /* Parse the received reply */
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "MGMD replied");
  ptin_mgmd_event_ctrl_parse(&outEventMsg, &ctrlResMsg);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Msg Code: %08X", ctrlResMsg.msgCode);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Msg Id  : %08X", ctrlResMsg.msgId);
  LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  CTRL Res     : %u",   ctrlResMsg.res);
 
  /* Copy the response contents to igmpProxy */
  if(sizeof(PTIN_MGMD_CTRL_MGMD_STATUS_t) != ctrlResMsg.dataLength)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Unexpected size in the MGMD response [dataLength:%u/%u]", ctrlResMsg.dataLength, sizeof(PTIN_MGMD_CTRL_MGMD_STATUS_t));
    return L7_FAILURE;
  }
  else
  {
    memcpy(&mgmdStatus, ctrlResMsg.data, ctrlResMsg.dataLength);
    LOG_DEBUG(LOG_CTX_PTIN_IGMP,  "  Mgmd Status  : %s",   mgmdStatus.mgmdStatus == PTIN_MGMD_STATUS_WORKING ? "Alive":"Dead");
  }

  return ctrlResMsg.res;
}
