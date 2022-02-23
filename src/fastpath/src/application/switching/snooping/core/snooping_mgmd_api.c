/*********************************************************************
*
* (C) Copyright PT Inova��o S.A. 2013-2013
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
#include "snooping_util.h"
#include "snooping_proto.h"
#include "snooping_db.h"
#include "snooping_api.h"
#include "ptin_intf.h"
#include "ptin_cnfgr.h"
#include "ptin_mgmd_inet_defs.h"
#include "ptin_debug.h"
#if (PTIN_BOARD_IS_LINECARD || PTIN_BOARD_IS_STANDALONE)
  #include "ptin_prot_typeb.h"
#endif
#if (PTIN_BOARD_IS_MATRIX || PTIN_BOARD_IS_LINECARD)
  #include "ptin_fpga_api.h"  
  #include "ptin_msghandler.h"
  #include "ptin_msg.h"
#endif

/* Static Methods */
#if (!PTIN_BOARD_IS_MATRIX && (defined (IGMP_QUERIER_IN_UC_EVC)))
L7_RC_t ptin_mgmd_send_leaf_packet(uint32 portId, L7_uint16 int_ovlan, L7_uint16 int_ivlan, L7_uchar8 *payload, L7_uint32 payloadLength,uchar8 family, L7_uint client_idx, L7_uint32 param);
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
  .port_resources_available    = &snooping_port_resources_available,
  .port_resources_allocate     = &snooping_port_resources_allocate,
  .port_resources_release      = &snooping_port_resources_release,
#endif
  .port_open                   = &snooping_port_open,
  .port_close                  = &snooping_port_close,
  .tx_packet                   = &snooping_tx_packet,
};

unsigned int snooping_igmp_admin_set(unsigned char admin)
{
  L7_BOOL adminMode = admin;

  PT_LOG_TRACE(LOG_CTX_IGMP, "Context [admin:%u]", admin);

  if ( L7_SUCCESS != usmDbSnoopAdminModeGet(1, &adminMode, L7_AF_INET))
  {
    PT_LOG_ERR(LOG_CTX_IGMP,"Error with usmDbSnoopAdminModeGet");
    return FAILURE;
  }

  if (adminMode == admin)
  {
    PT_LOG_NOTICE(LOG_CTX_IGMP,"usmDbSnoopAdminModeSet is already :%u", admin);
    return L7_SUCCESS;
  }

  // Snooping global activation
  if (L7_SUCCESS != usmDbSnoopAdminModeSet(1, admin, L7_AF_INET))  {
    PT_LOG_ERR(LOG_CTX_IGMP,"Error with usmDbSnoopAdminModeSet");
    return FAILURE;
  }
  
  return SUCCESS;
}

unsigned int snooping_mld_admin_set(unsigned char admin)
{
  PT_LOG_TRACE(LOG_CTX_IGMP, "Context [admin:%u]", admin);

  // Snooping global activation
  if (L7_SUCCESS != usmDbSnoopAdminModeSet(1, admin, L7_AF_INET6))  {
    PT_LOG_ERR(LOG_CTX_IGMP,"Error with usmDbSnoopAdminModeSet");
    return FAILURE;
  }

  return SUCCESS;
}

unsigned int snooping_cos_set(unsigned char cos)
{
  PT_LOG_TRACE(LOG_CTX_IGMP, "Context [cos:%u]", cos);

  // Attrib IGMP packets priority
  if (L7_SUCCESS != usmDbSnoopPrioModeSet(1, cos, L7_AF_INET))
  {
    PT_LOG_ERR(LOG_CTX_IGMP,"Error with usmDbSnoopPrioModeSet");
    return FAILURE;
  }

  return SUCCESS;
}

unsigned int snooping_portList_get(unsigned int serviceId, ptin_mgmd_port_type_t portType, PTIN_MGMD_PORT_MASK_t *portList, unsigned int *noOfPorts)
{
  ptin_port_bmp_t interfaceBitmap;
  L7_uint32      noOfInterfaces = 0;
  L7_uint16      mcastRootVlan;
  L7_RC_t        res = SUCCESS;  

  //PT_LOG_TRACE(LOG_CTX_IGMP, "Context [serviceId:%u portType:%u portList:%p noOfPorts:%p]", serviceId, portType, portList, noOfPorts);

  if ( portList == L7_NULLPTR || noOfPorts == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_IGMP, "Context [serviceId:%u portType:%u portList:%p noOfPorts:%p]", serviceId, portType, portList, noOfPorts);
    return FAILURE;
  }

  *noOfPorts = 0;

  memset(&interfaceBitmap, 0x00, sizeof(interfaceBitmap));

  if( SUCCESS != ptin_evc_intRootVlan_get(serviceId, &mcastRootVlan))
  {
    PT_LOG_ERR(LOG_CTX_IGMP,"Unable to get mcastRootVlan from serviceId:%u", serviceId);
    memcpy(portList, &interfaceBitmap, sizeof(*portList));
    return NOT_EXIST;
  } 

  /* Request portList to FP */
  if(PTIN_MGMD_PORT_TYPE_LEAF == portType)
  {
    res = ptin_igmp_clientPtinport_getList(mcastRootVlan, &interfaceBitmap, &noOfInterfaces);
  }
  else if(PTIN_MGMD_PORT_TYPE_ROOT == portType)
  {
    res = ptin_igmp_rootptinPort_getList(mcastRootVlan, &interfaceBitmap, &noOfInterfaces);
  }
  else
  {   
    PT_LOG_ERR(LOG_CTX_IGMP,"Unknown port type");
    memcpy(portList, &interfaceBitmap, sizeof(*portList));
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
    PT_LOG_ERR(LOG_CTX_IGMP,"Unable to get port information");
    return FAILURE;
  }

  memcpy(portList, &interfaceBitmap, sizeof(*portList));
  *noOfPorts = noOfInterfaces;

  return SUCCESS;
}

unsigned int snooping_portType_get(unsigned int serviceId, unsigned int portId, ptin_mgmd_port_type_t *portType)
{
  L7_uint8 port_type;
  L7_uint32 ptin_port = portId-1;
#ifdef ONE_MULTICAST_VLAN_RING_SUPPORT
  L7_uint8 port_type_igmp;
  L7_uint8 rc;

#endif //ONE_MULTICAST_VLAN_RING_SUPPORT

  if (SUCCESS != ptin_evc_port_type_get(serviceId, ptin_port, &port_type))
  {
    PT_LOG_ERR(LOG_CTX_IGMP,"Unknown port type");
    return FAILURE;
  }

#ifdef ONE_MULTICAST_VLAN_RING_SUPPORT
  rc = ptin_igmp_port_type_get(ptin_port, &port_type_igmp);

  if (rc == L7_SUCCESS)
  {
    PT_LOG_DEBUG(LOG_CTX_IGMP, "Port is %d ",port_type_igmp);
  }
  else
  {
    PT_LOG_DEBUG(LOG_CTX_IGMP, "Port is LAG ");
    return L7_FALSE;
  }
#endif //ONE_MULTICAST_VLAN_RING_SUPPORT

  if (port_type == PTIN_EVC_INTF_LEAF)
  {
    *portType = PTIN_MGMD_PORT_TYPE_LEAF;
  }
  else
  {
    if (port_type == PTIN_EVC_INTF_ROOT)
    {
#ifdef ONE_MULTICAST_VLAN_RING_SUPPORT
      if(port_type_igmp == PTIN_IGMP_PORT_CLIENT)
      {
        *portType = PTIN_MGMD_PORT_TYPE_LEAF;
      }
      else
      {
        *portType = PTIN_MGMD_PORT_TYPE_ROOT;
      }
#else 
      *portType = PTIN_MGMD_PORT_TYPE_ROOT;
#endif //ONE_MULTICAST_VLAN_RING_SUPPORT
    }
    else
    {
      PT_LOG_ERR(LOG_CTX_IGMP,"Unknown port type");
      return FAILURE;
    }
  }
  return SUCCESS;
}

unsigned int snooping_channel_serviceid_get(unsigned int portId, unsigned int clientId, unsigned int groupAddr, unsigned int sourceAddr, unsigned int *serviceId)
{
  PT_LOG_TRACE(LOG_CTX_IGMP, "Context [groupAddr:%08X sourceAddr:%08X serviceId:%p]", groupAddr, sourceAddr, serviceId);

  if(serviceId == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_IGMP, "Abnormal context [serviceId:%p]", serviceId);
    return FAILURE;
  }

#if (!PTIN_BOARD_IS_MATRIX) //Conversion from IGMP service to the Multicast service is only performed in the linecards
{
  L7_inet_addr_t groupInetAddr;
  L7_inet_addr_t sourceInetAddr;
  L7_uint16      intVlan         = (L7_uint16) -1;
  L7_BOOL        isLeafPort      = L7_TRUE;
  L7_uint32      ptin_port = portId-1;

  /* Get multicast root vlan */
  inetAddressSet(L7_AF_INET, &groupAddr,  &groupInetAddr);
  inetAddressSet(L7_AF_INET, &sourceAddr, &sourceInetAddr);
  
  if (ptin_igmp_mcast_evc_id_get(intVlan, ptin_port, isLeafPort, clientId, &groupInetAddr, &sourceInetAddr, serviceId) == L7_SUCCESS)
  {
    if(L7_TRUE != ptin_evc_is_intf_leaf(*serviceId, ptin_port))
    {
      PT_LOG_ERR(LOG_CTX_IGMP,"This is not a leaf Port (ptin_port=%u, serviceId=%u)", ptin_port, *serviceId);    
      return L7_FAILURE;
    }    
    PT_LOG_TRACE(LOG_CTX_IGMP,"Found serviceID %u associated to the pair {groupAddr,sourceAddr}={%08X,%08X}", *serviceId, groupAddr, sourceAddr);
  }
  else
  {
    PT_LOG_DEBUG(LOG_CTX_IGMP,"Unable to determine serviceID associated to the pair {groupAddr,sourceAddr}={%08X,%08X}", groupAddr, sourceAddr);
    return L7_FAILURE;
  }
}
#endif //(!PTIN_BOARD_IS_MATRIX)

  return SUCCESS;
}

unsigned int snooping_clientList_get(unsigned int serviceId, unsigned int portId, PTIN_MGMD_CLIENT_MASK_t *clientList, unsigned int *noOfClients)
{
#if (!PTIN_BOARD_IS_MATRIX)
  L7_uint32    ptin_port = portId-1;
#endif
  //PT_LOG_TRACE(LOG_CTX_IGMP, "Context [serviceId:%u portId:%u clientList:%p noOfClients:%p]", serviceId, portId, clientList, noOfClients);

  memset(clientList->value, 0x00, PTIN_MGMD_CLIENT_BITMAP_SIZE * sizeof(uint8));
  
#if (!PTIN_BOARD_IS_MATRIX) //Since we do not expose any counters for the packets sent from the MX to the LC it does not make sense to increment them on the MGMD module
  ptin_timer_start(72,"ptin_igmp_groupclients_bmp_get");
  if(ptin_igmp_groupclients_bmp_get(serviceId, ptin_port, clientList->value, noOfClients)!=L7_SUCCESS)
  {
    ptin_timer_stop(72);
    PT_LOG_ERR(LOG_CTX_IGMP,"Failed to obtain client bitmap [serviceId:%u ptin_port:%u clientList:%p]", serviceId, ptin_port, clientList);
    return SUCCESS;
  }
  ptin_timer_stop(72);
#else
  *noOfClients=0;
#endif

  return SUCCESS;
}

/*Admission Control Feature*/
#if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT

unsigned int snooping_port_resources_available(unsigned int serviceId, unsigned int portId, unsigned int groupAddr, unsigned int sourceAddr)
{
  L7_uint32      channelBandwidth;
  L7_uint32      ptin_port = portId - 1;
  L7_RC_t        rc;
  L7_uint16      int_ovlan;

  PT_LOG_TRACE(LOG_CTX_IGMP, "Context [serviceId:%u portId:%u groupAddr:0x%08x sourceAddr:0x%08x]", serviceId, portId, groupAddr, sourceAddr);
 
  if (L7_FALSE == ptin_igmp_proxy_admission_control_get())
  {
    PT_LOG_NOTICE(LOG_CTX_IGMP, "Admission Control Feature is Disabled!");
    return TRUE;
  }

  if (portId == 0)
  {
    PT_LOG_DEBUG(LOG_CTX_IGMP, "Ignoring Request [serviceId:%u portId:%u groupAddr:0x%08x sourceAddr:0x%08x]", serviceId, portId, groupAddr, sourceAddr);
    return L7_TRUE;
  }

  //Get outter internal vlan
  if( SUCCESS != ptin_evc_intRootVlan_get(serviceId, &int_ovlan))
  {
    PT_LOG_ERR(LOG_CTX_IGMP,"Unable to get mcastRootVlan from serviceId");
    return FAILURE;
  }

  if (L7_TRUE == ptin_igmp_proxy_bandwidth_control_get() && sourceAddr != 0)
  {
    L7_inet_addr_t inetGroupAddr;
    L7_inet_addr_t inetSourceAddr;

    inetAddressSet(L7_AF_INET, &groupAddr , &inetGroupAddr);
    inetAddressSet(L7_AF_INET, &sourceAddr , &inetSourceAddr);

    ptin_timer_start(60,"ptin_igmp_channel_bandwidth_get");
    if ( L7_SUCCESS != ptin_igmp_channel_bandwidth_get(serviceId, &inetGroupAddr, &inetSourceAddr, &channelBandwidth))
    {
      ptin_timer_stop(60);
      PT_LOG_ERR(LOG_CTX_IGMP, "Failed to obtain channel bandwidth [serviceId:%u portId:%u groupAddr:0x%08x sourceAddr:0x%08x]", serviceId, portId, groupAddr, sourceAddr);
      return L7_FALSE;
    }
    ptin_timer_stop(60);

    if (ptin_debug_igmp_snooping)
    {   
      PT_LOG_TRACE(LOG_CTX_IGMP,"Channel Bandwidth:%u kbps", channelBandwidth); 
    }
  }
  else
  {
    channelBandwidth = 0;
  }

  ptin_timer_start(61,"ptin_igmp_port_resources_available");
  rc = ptin_igmp_port_resources_available(ptin_port, channelBandwidth);
  ptin_timer_stop(61);

  return (rc == L7_SUCCESS) ;
}

unsigned int snooping_port_resources_allocate(unsigned int serviceId, unsigned int portId, unsigned int groupAddr, unsigned int sourceAddr)
{  
  L7_uint32      channelBandwidth;
  L7_uint32      ptin_port = portId - 1;
  L7_RC_t        rc;
  L7_uint16      int_ovlan;
    
  PT_LOG_TRACE(LOG_CTX_IGMP, "Context [serviceId:%u portId:%u groupAddr:0x%08x sourceAddr:0x%08x ]", serviceId, portId, groupAddr, sourceAddr);

  if (L7_FALSE == ptin_igmp_proxy_admission_control_get())
  {
    PT_LOG_NOTICE(LOG_CTX_IGMP, "Admission Control Feature is Disabled!");
    return SUCCESS;
  }

  if (portId == 0)
  {
    PT_LOG_DEBUG(LOG_CTX_IGMP, "Ignoring Request [serviceId:%u portId:%u groupAddr:0x%08x sourceAddr:0x%08x]", serviceId, portId, groupAddr, sourceAddr);
    return L7_SUCCESS;
  }

  //Get outter internal vlan
  if( SUCCESS != ptin_evc_intRootVlan_get(serviceId, &int_ovlan))
  {
    PT_LOG_ERR(LOG_CTX_IGMP,"Unable to get mcastRootVlan from serviceId");
    return FAILURE;
  }

  if (L7_TRUE == ptin_igmp_proxy_bandwidth_control_get())
  {
    L7_inet_addr_t inetGroupAddr;
    L7_inet_addr_t inetSourceAddr;

    inetAddressSet(L7_AF_INET, &groupAddr , &inetGroupAddr);
    inetAddressSet(L7_AF_INET, &sourceAddr , &inetSourceAddr);

    ptin_timer_start(60,"ptin_igmp_channel_bandwidth_get");
    if ( L7_SUCCESS != ptin_igmp_channel_bandwidth_get(serviceId, &inetGroupAddr, &inetSourceAddr, &channelBandwidth))
    {
      ptin_timer_stop(60);
      PT_LOG_ERR(LOG_CTX_IGMP, "Failed to obtain channel bandwidth [serviceId:%u portId:%u groupAddr:0x%08x sourceAddr:0x%08x]", serviceId, portId, groupAddr, sourceAddr);
      return L7_FALSE;
    }
    ptin_timer_stop(60);

    if (ptin_debug_igmp_snooping)
    {   
      PT_LOG_TRACE(LOG_CTX_IGMP,"Channel Bandwidth:%u kbps", channelBandwidth); 
    }
  }
  else
  {
    channelBandwidth = 0;
  }

  ptin_timer_start(62,"ptin_igmp_port_resources_allocate");
  rc = ptin_igmp_port_resources_allocate(ptin_port, channelBandwidth);
  ptin_timer_stop(62);

  return rc;
}

unsigned int snooping_port_resources_release(unsigned int serviceId, unsigned int portId, unsigned int groupAddr, unsigned int sourceAddr)
{  
  L7_uint32      channelBandwidth;
  L7_uint32      ptin_port = portId - 1;
  L7_uint16      int_ovlan;
  L7_RC_t        rc;
  
  PT_LOG_TRACE(LOG_CTX_IGMP, "Context [serviceId:%u portId:%u groupAddr:0x%08x sourceAddr:0x%08x ]", serviceId, portId, groupAddr, sourceAddr);

  if (L7_FALSE == ptin_igmp_proxy_admission_control_get())
  {
    PT_LOG_NOTICE(LOG_CTX_IGMP, "Admission Control Feature is Disabled!");
    return SUCCESS;
  }

  if (portId == 0)
  {
    PT_LOG_DEBUG(LOG_CTX_IGMP, "Ignoring Request [serviceId:%u portId:%u groupAddr:0x%08x sourceAddr:0x%08x]", serviceId, portId, groupAddr, sourceAddr);
    return L7_SUCCESS;
  }

  //Get outter internal vlan
  if( SUCCESS != ptin_evc_intRootVlan_get(serviceId, &int_ovlan))
  {
    PT_LOG_ERR(LOG_CTX_IGMP,"Unable to get mcastRootVlan from serviceId");
    return FAILURE;
  }

  if (L7_TRUE == ptin_igmp_proxy_bandwidth_control_get())
  {
    L7_inet_addr_t inetGroupAddr;
    L7_inet_addr_t inetSourceAddr;

    inetAddressSet(L7_AF_INET, &groupAddr , &inetGroupAddr);
    inetAddressSet(L7_AF_INET, &sourceAddr , &inetSourceAddr);

    ptin_timer_start(60,"ptin_igmp_channel_bandwidth_get");
    if ( L7_SUCCESS != ptin_igmp_channel_bandwidth_get(serviceId, &inetGroupAddr, &inetSourceAddr, &channelBandwidth))
    {
      ptin_timer_stop(60);
      PT_LOG_ERR(LOG_CTX_IGMP, "Failed to obtain channel bandwidth [serviceId:%u portId:%u groupAddr:0x%08x sourceAddr:0x%08x]", serviceId, portId, groupAddr, sourceAddr);
      return L7_FALSE;
    }
    ptin_timer_stop(60);

    if (ptin_debug_igmp_snooping)
    {   
      PT_LOG_TRACE(LOG_CTX_IGMP,"Channel Bandwidth:%u kbps", channelBandwidth); 
    }
  }
  else
  {
    channelBandwidth = 0;
  }

  ptin_timer_start(63,"ptin_igmp_port_resources_release");
  rc = ptin_igmp_port_resources_release(ptin_port, channelBandwidth);
  ptin_timer_stop(63);

  return rc;
}

unsigned int snooping_client_resources_available(unsigned int serviceId, unsigned int portId, unsigned int clientId, unsigned int groupAddr, unsigned int sourceAddr, PTIN_MGMD_CLIENT_MASK_t *clientList, unsigned int noOfClients)
{ 
  L7_uint32      channelBandwidth; 
  L7_uint32      ptin_port = portId - 1;
  L7_uint16      int_ovlan;
  L7_RC_t        rc;

  PT_LOG_TRACE(LOG_CTX_IGMP, "Context [serviceId:%u portId:%u clientId:%u groupAddr:0x%08x sourceAddr:0x%08x noOfClients:%u]", serviceId, portId, clientId, groupAddr, sourceAddr, noOfClients);
  
  if (L7_FALSE == ptin_igmp_proxy_admission_control_get())
  {
    PT_LOG_NOTICE(LOG_CTX_IGMP, "Admission Control Feature is Disabled!");
    return TRUE;
  }

  if (portId == 0 || clientId == (L7_uint32) -1)
  {
    PT_LOG_DEBUG(LOG_CTX_IGMP, "Ignoring Request [serviceId:%u portId:%u groupAddr:0x%08x sourceAddr:0x%08x]", serviceId, portId, groupAddr, sourceAddr);
    return L7_TRUE;
  }

  //Get outter internal vlan
  if( SUCCESS != ptin_evc_intRootVlan_get(serviceId, &int_ovlan))
  {
    PT_LOG_ERR(LOG_CTX_IGMP,"Unable to get mcastRootVlan from serviceId");
    return FAILURE;
  }

  if ( noOfClients != 0)     
  {
    ptin_timer_start(70,"ptin_igmp_admission_control_verify_the_presence_of_other_clients");
    if (L7_ALREADY_CONFIGURED == ptin_igmp_admission_control_verify_the_presence_of_other_clients(ptin_port, clientId, clientList->value))
    {
      ptin_timer_stop(70);
      return L7_TRUE;
    }
     ptin_timer_stop(70);
  }

  if (L7_TRUE == ptin_igmp_proxy_bandwidth_control_get() && sourceAddr != 0)
  {
    L7_inet_addr_t inetGroupAddr;
    L7_inet_addr_t inetSourceAddr;

    inetAddressSet(L7_AF_INET, &groupAddr , &inetGroupAddr);
    inetAddressSet(L7_AF_INET, &sourceAddr , &inetSourceAddr);

    ptin_timer_start(60,"ptin_igmp_channel_bandwidth_get");
    if ( L7_SUCCESS != ptin_igmp_channel_bandwidth_get(serviceId, &inetGroupAddr, &inetSourceAddr, &channelBandwidth))
    {
      ptin_timer_stop(60);
      PT_LOG_ERR(LOG_CTX_IGMP, "Failed to obtain channel bandwidth [serviceId:%u portId:%u groupAddr:0x%08x sourceAddr:0x%08x]", serviceId, portId, groupAddr, sourceAddr);
      return L7_FALSE;
    }
    ptin_timer_stop(60);

    if (ptin_debug_igmp_snooping)
    {   
      PT_LOG_TRACE(LOG_CTX_IGMP,"Channel Bandwidth:%u kbps", channelBandwidth); 
    }
  }
  else
  {
    channelBandwidth = 0;
  }

  ptin_timer_start(64,"ptin_igmp_client_resources_available");
  rc = ptin_igmp_client_resources_available(ptin_port, clientId, channelBandwidth);
  ptin_timer_stop(64);

  if (rc != L7_SUCCESS)
  {
    return L7_FALSE;
  }

#if PTIN_BOARD_IS_ACTIVETH
  if ( noOfClients != 0)     
  {
    ptin_timer_start(71,"ptin_igmp_admission_control_verify_the_presence_of_other_groupclients");
    if (L7_ALREADY_CONFIGURED == ptin_igmp_admission_control_verify_the_presence_of_other_groupclients(ptin_port, clientId, clientList->value))
    {
      ptin_timer_stop(71);
      return L7_TRUE;
    }
     ptin_timer_stop(71);
  }
#endif

  ptin_timer_start(67,"ptin_igmp_multicast_service_resources_available");
  rc = ptin_igmp_multicast_service_resources_available(ptin_port, clientId,serviceId,channelBandwidth);
  ptin_timer_stop(67);
    
  return (rc == L7_SUCCESS) ;
}

unsigned int snooping_client_resources_allocate(unsigned int serviceId, unsigned int portId, unsigned int clientId, unsigned int groupAddr, unsigned int sourceAddr, PTIN_MGMD_CLIENT_MASK_t *clientList, unsigned int noOfClients)
{ 
  L7_uint32      channelBandwidth;
  L7_uint32      ptin_port = portId - 1;
  L7_RC_t        rc;
  L7_uint16      int_ovlan;
  
  PT_LOG_TRACE(LOG_CTX_IGMP, "Context [serviceId:%u portId:%u clientId:%u groupAddr:0x%08x sourceAddr:0x%08x noOfClients:%u]", serviceId, portId, clientId, groupAddr, sourceAddr, noOfClients);

  if (L7_FALSE == ptin_igmp_proxy_admission_control_get())
  {
    PT_LOG_NOTICE(LOG_CTX_IGMP, "Admission Control Feature is Disabled!");
    return SUCCESS;
  }

  if (portId == 0 || clientId == (L7_uint32) -1)
  {
    PT_LOG_DEBUG(LOG_CTX_IGMP, "Ignoring Request [serviceId:%u portId:%u groupAddr:0x%08x sourceAddr:0x%08x]", serviceId, portId, groupAddr, sourceAddr);
    return L7_SUCCESS;
  }

  //Get outter internal vlan
  if( SUCCESS != ptin_evc_intRootVlan_get(serviceId, &int_ovlan))
  {
    PT_LOG_ERR(LOG_CTX_IGMP,"Unable to get mcastRootVlan from serviceId");
    return FAILURE;
  }

  if ( noOfClients != 0)     
  {
    ptin_timer_start(70,"ptin_igmp_admission_control_verify_the_presence_of_other_clients");
    if (L7_ALREADY_CONFIGURED == ptin_igmp_admission_control_verify_the_presence_of_other_clients(ptin_port, clientId, clientList->value))
    {
      ptin_timer_stop(70);
      return L7_SUCCESS;
    }
     ptin_timer_stop(70);
  }

  if (L7_TRUE == ptin_igmp_proxy_bandwidth_control_get())
  {
    L7_inet_addr_t inetGroupAddr;
    L7_inet_addr_t inetSourceAddr;

    inetAddressSet(L7_AF_INET, &groupAddr , &inetGroupAddr);
    inetAddressSet(L7_AF_INET, &sourceAddr , &inetSourceAddr);

    ptin_timer_start(60,"ptin_igmp_channel_bandwidth_get");
    if ( L7_SUCCESS != ptin_igmp_channel_bandwidth_get(serviceId, &inetGroupAddr, &inetSourceAddr, &channelBandwidth))
    {
      ptin_timer_stop(60);
      PT_LOG_ERR(LOG_CTX_IGMP, "Failed to obtain channel bandwidth [serviceId:%u portId:%u groupAddr:0x%08x sourceAddr:0x%08x]", serviceId, portId, groupAddr, sourceAddr);
      return L7_FALSE;
    }
    ptin_timer_stop(60);

    if (ptin_debug_igmp_snooping)
    {   
      PT_LOG_TRACE(LOG_CTX_IGMP,"Channel Bandwidth:%u kbps", channelBandwidth); 
    }
  }
  else
  {
    channelBandwidth = 0;
  }
  
  ptin_timer_start(65,"ptin_igmp_client_resources_allocate");
  rc = ptin_igmp_client_resources_allocate(ptin_port, clientId, channelBandwidth);
  ptin_timer_stop(65);

  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

#if PTIN_BOARD_IS_ACTIVETH
  if ( noOfClients != 0)     
  {
    ptin_timer_start(71,"ptin_igmp_admission_control_verify_the_presence_of_other_groupclients");
    if (L7_ALREADY_CONFIGURED == ptin_igmp_admission_control_verify_the_presence_of_other_groupclients(ptin_port, clientId, clientList->value))
    {
      ptin_timer_stop(71);
      return L7_SUCCESS;
    }
     ptin_timer_stop(71);
  }
#endif

  ptin_timer_start(68,"ptin_igmp_multicast_service_resources_allocate");
  rc = ptin_igmp_multicast_service_resources_allocate(ptin_port, clientId,serviceId,channelBandwidth);
  ptin_timer_stop(68);
    
  return rc;
}

unsigned int snooping_client_resources_release(unsigned int serviceId, unsigned int portId, unsigned int clientId, unsigned int groupAddr, unsigned int sourceAddr, PTIN_MGMD_CLIENT_MASK_t *clientList, unsigned int noOfClients)
{  
  L7_uint32      channelBandwidth;
  L7_uint32      ptin_port = portId - 1;
  L7_RC_t        rc;
  L7_uint16      int_ovlan;
  
  PT_LOG_TRACE(LOG_CTX_IGMP, "Context [serviceId:%u portId:%u clientId:%u groupAddr:0x%08x sourceAddr:0x%08x noOfClients:%u]", serviceId, portId, clientId, groupAddr, sourceAddr, noOfClients);

  if (L7_FALSE == ptin_igmp_proxy_admission_control_get())
  {
    PT_LOG_NOTICE(LOG_CTX_IGMP, "Admission Control Feature is Disabled!");
    return SUCCESS;
  }

  if (portId == 0 || clientId == (L7_uint32) -1)
  {
    PT_LOG_DEBUG(LOG_CTX_IGMP, "Ignoring Request [serviceId:%u portId:%u groupAddr:0x%08x sourceAddr:0x%08x]", serviceId, portId, groupAddr, sourceAddr);
    return L7_SUCCESS;
  }

  //Get outter internal vlan
  if( SUCCESS != ptin_evc_intRootVlan_get(serviceId, &int_ovlan))
  {
    PT_LOG_ERR(LOG_CTX_IGMP,"Unable to get mcastRootVlan from serviceId");
    return FAILURE;
  }

  if ( noOfClients != 0)     
  {
    ptin_timer_start(70,"ptin_igmp_admission_control_verify_the_presence_of_other_clients");
    if (L7_ALREADY_CONFIGURED == ptin_igmp_admission_control_verify_the_presence_of_other_clients(ptin_port, clientId, clientList->value))
    {
      ptin_timer_stop(70);
      return L7_SUCCESS;
    }
    ptin_timer_stop(70);
  }

  if (L7_TRUE == ptin_igmp_proxy_bandwidth_control_get())
  {
    L7_inet_addr_t inetGroupAddr;
    L7_inet_addr_t inetSourceAddr;

    inetAddressSet(L7_AF_INET, &groupAddr , &inetGroupAddr);
    inetAddressSet(L7_AF_INET, &sourceAddr , &inetSourceAddr);

    ptin_timer_start(60,"ptin_igmp_channel_bandwidth_get");
    if ( L7_SUCCESS != ptin_igmp_channel_bandwidth_get(serviceId, &inetGroupAddr, &inetSourceAddr, &channelBandwidth))
    {
      ptin_timer_stop(60);
      PT_LOG_ERR(LOG_CTX_IGMP, "Failed to obtain channel bandwidth [serviceId:%u portId:%u groupAddr:0x%08x sourceAddr:0x%08x]", serviceId, portId, groupAddr, sourceAddr);
      return L7_FALSE;
    }
    ptin_timer_stop(60);

    if (ptin_debug_igmp_snooping)
    {   
      PT_LOG_TRACE(LOG_CTX_IGMP,"Channel Bandwidth:%u kbps", channelBandwidth); 
    }
  }
  else
  {
    channelBandwidth = 0;
  }

  ptin_timer_start(66,"ptin_igmp_client_resources_release");
  rc = ptin_igmp_client_resources_release(ptin_port, clientId, channelBandwidth);
  ptin_timer_stop(66);

  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

#if PTIN_BOARD_IS_ACTIVETH
  if ( noOfClients != 0)     
  {
    ptin_timer_start(71,"ptin_igmp_admission_control_verify_the_presence_of_other_groupclients");
    if (L7_ALREADY_CONFIGURED == ptin_igmp_admission_control_verify_the_presence_of_other_groupclients(ptin_port, clientId, clientList->value))
    {
      ptin_timer_stop(71);
      return L7_SUCCESS;
    }
     ptin_timer_stop(71);
  }
#endif

  ptin_timer_start(69,"ptin_igmp_multicast_service_resources_release");
  rc = ptin_igmp_multicast_service_resources_release(ptin_port, clientId,serviceId,channelBandwidth);
  ptin_timer_stop(69);
    
  return rc;
}

#endif //End Admission Control

unsigned int snooping_port_open(unsigned int serviceId, unsigned int portId, unsigned int ipv4GroupAddr, unsigned int ipv4SourceAddr, unsigned char isStatic)
{
  L7_inet_addr_t groupAddr;
  L7_inet_addr_t sourceAddr;
  L7_BOOL        isProtection = L7_FALSE;
  L7_uint32      ptin_port = portId - 1;

  inetAddressSet(L7_AF_INET, &ipv4GroupAddr, &groupAddr);
  inetAddressSet(L7_AF_INET, &ipv4SourceAddr, &sourceAddr);

  /*
   * We were forced to implement this method asynchronous from MGMD as the SDK crashes if the mfdb request is made by the MGMD thread. 
   * The SDK exits in an assert that checks for the in_interrupt() method. As no solution was found, an alternative method was implemented. 
   * Instead of directly calling mfdb, MGMD will place a request in the snooping queue, which will eventually be processed. 
   */
  return snoopPortOpen(serviceId, ptin_port, &groupAddr, &sourceAddr, isStatic, isProtection);
  
}

unsigned int snooping_port_close(unsigned int serviceId, unsigned int portId, unsigned int ipv4GroupAddr, unsigned int ipv4SourceAddr)
{
  L7_inet_addr_t groupAddr;
  L7_inet_addr_t sourceAddr;
  L7_BOOL        isProtection = L7_FALSE;
  L7_uint32      ptin_port = portId - 1;

  inetAddressSet(L7_AF_INET, &ipv4GroupAddr, &groupAddr);
  inetAddressSet(L7_AF_INET, &ipv4SourceAddr, &sourceAddr);

  /*
   * We were forced to implement this method asynchronous from MGMD as the SDK crashes if the mfdb request is made by the MGMD thread. 
   * The SDK exits in an assert that checks for the in_interrupt() method. As no solution was found, an alternative method was implemented. 
   * Instead of directly calling mfdb, MGMD will place a request in the snooping queue, which will eventually be processed. 
   */
  return snoopPortClose(serviceId, ptin_port, &groupAddr, &sourceAddr, isProtection);
}

unsigned int snooping_tx_packet(unsigned char *payload, unsigned int payloadLength, unsigned int serviceId, unsigned int portId, unsigned int clientId, unsigned char family, unsigned int onuId)
{
#if PTIN_BOARD_IS_MATRIX
  L7_uint16             shortVal;
  L7_uchar8             srcMac[L7_MAC_ADDR_LEN];
  L7_uchar8             destMac[L7_MAC_ADDR_LEN];
  L7_uchar8             packet[L7_MAX_FRAME_SIZE];
  L7_uchar8            *dataPtr;
  L7_uint32             packetLength = payloadLength;
  L7_uint32             dstIpAddr, intIfNum;
  L7_uchar8            *destIpPtr;
  L7_inet_addr_t        destIp;
  L7_uint32             activeState;  
  L7_uint16             int_ovlan; 
  L7_uint16             int_ivlan    = 0; 
  ptin_IgmpProxyCfg_t   igmpCfg; 
  L7_uint32             ptin_port = portId-1;
  L7_RC_t               rc;
   
  PT_LOG_TRACE(LOG_CTX_IGMP, "Context [payLoad:%p payloadLength:%u serviceId:%u portId:%u clientId:%u family:%u]", payload, payloadLength, serviceId, portId, clientId, family);

  rc = ptin_intf_port2intIfNum(ptin_port, &intIfNum);
  if (rc != L7_SUCCESS)
  {
     PT_LOG_ERR(LOG_CTX_IGMP, "Error getting intIfNum");
     return FAILURE;
  }

#if 1 //ndef ONE_MULTICAST_VLAN_RING_SUPPORT
  /* Do nothing for slave matrix */
  if (!ptin_fpga_mx_is_matrixactive_rt())
  {
    if (ptin_debug_igmp_snooping)
      PT_LOG_NOTICE(LOG_CTX_IGMP,"Silently ignoring packet transmission. I'm a Slave Matrix [portId=%u serviceId=%u]",portId, serviceId);
    return SUCCESS;
  }
#endif //ONE_MULTICAST_VLAN_RING_SUPPORT

  //Ignore if the port has link down
  if ( (nimGetIntfActiveState(intIfNum, &activeState) != L7_SUCCESS) || (activeState != L7_ACTIVE) )
  {
    if (ptin_debug_igmp_snooping)
      PT_LOG_NOTICE(LOG_CTX_IGMP,"Silently ignoring packet transmission. Outgoing interface [portId=%u serviceId=%u] is down!",intIfNum,serviceId);    
    return SUCCESS;
  }

  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get__snooping_old(&igmpCfg) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_IGMP, "Error getting IGMP Proxy configurations");        
  }
  //Workaround to support Group Specific Queries; IPv6 is not complaint with this approach!       
  ptin_mgmd_port_type_t portType;
  L7_uint32             groupAddress;
#ifdef ONE_MULTICAST_VLAN_RING_SUPPORT
  L7_uint8              isDynamic; 
#endif //ONE_MULTICAST_VLAN_RING_SUPPORT  

  /* portID (mgmd) is equal to ptin_port + 1*/ 
  if (snooping_portType_get(serviceId, portId, &portType) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_IGMP,"Unable to get port type from int_ovlan [%u] portId [%u]",serviceId,portId);
    return FAILURE;
  }

#ifdef ONE_MULTICAST_VLAN_RING_SUPPORT
  ptin_igmp_port_is_Dynamic(ptin_port, &isDynamic);

  PT_LOG_TRACE(LOG_CTX_IGMP,"RING: port is Dynamic %d ",isDynamic);

#endif //ONE_MULTICAST_VLAN_RING_SUPPORT
  //Get Group Address
  destIpPtr = (payload+28);
  SNOOP_GET_ADDR(&groupAddress, destIpPtr); 
  
  //We only get the intRootVLAN here for the General Query and for the Membership Reports
  //For Group Specific Queries we use the  ptin_mgmd_send_leaf_packet to obtain the intRootVLAN
  if(1)
  {  
    //Get outter internal vlan
    if( SUCCESS != ptin_evc_intRootVlan_get(serviceId, &int_ovlan))
    {
      PT_LOG_ERR(LOG_CTX_IGMP,"Unable to get mcastRootVlan from serviceId");
      return FAILURE;
    }
  }
#ifdef ONE_MULTICAST_VLAN_RING_SUPPORT
  else if( portType == PTIN_MGMD_PORT_TYPE_LEAF && isDynamic)
  {
    PT_LOG_TRACE(LOG_CTX_IGMP,"portType == PTIN_MGMD_PORT_TYPE_LEAF && isDynamic");
     //Get outter internal vlan
    if( SUCCESS != ptin_evc_intRootVlan_get(serviceId, &int_ovlan))
    {
      PT_LOG_TRACE(LOG_CTX_IGMP,"Unable to get mcastRootVlan from serviceId");
      return FAILURE;
    }
  }
#endif //ONE_MULTICAST_VLAN_RING_SUPPORT

  //Get destination MAC from destIpAddr
  destIpPtr = (payload+16);
  SNOOP_GET_ADDR(&dstIpAddr, destIpPtr);  
  
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

#ifdef ONE_MULTICAST_VLAN_RING_SUPPORT
  L7_uint8 local_router_port_id;
 if ( portType == PTIN_MGMD_PORT_TYPE_ROOT /* LRP */ ||
     (portType == PTIN_MGMD_PORT_TYPE_LEAF && isDynamic && (ptin_igmp_get_local_router_port(&local_router_port_id) == L7_SUCCESS)) ||/* Portas de uplink client */ 
     (portId >= PTIN_SYSTEM_N_LOCAL_PORTS && portId < PTIN_SYSTEM_N_INTERF) )
#endif                
  { 
    ptin_timer_start(31,"snoopPacketSend"); 
    //Send packet
    snoopPacketSend(ptin_port, int_ovlan, int_ivlan, packet, packetLength, family, clientId);
    ptin_timer_stop(31);       
  }
  return SUCCESS;
#else
  L7_uint16             shortVal;
  L7_uchar8             srcMac[L7_MAC_ADDR_LEN];
  L7_uchar8             destMac[L7_MAC_ADDR_LEN];
  L7_uchar8             packet[L7_MAX_FRAME_SIZE];
  L7_uchar8            *dataPtr;
  L7_uint32             packetLength = payloadLength;
  L7_uint32             dstIpAddr, intIfNum;
  L7_uchar8            *destIpPtr;
  L7_inet_addr_t        destIp;
  L7_uint32             activeState;
  L7_uint16             int_ovlan;
  L7_uint16             int_ivlan    = 0;
  L7_uint32             ptin_port    = portId-1;
  ptin_IgmpProxyCfg_t   igmpCfg;
  ptin_mgmd_port_type_t portType;
  L7_uint32             groupAddress;
  L7_RC_t               rc;
#ifdef ONE_MULTICAST_VLAN_RING_SUPPORT
  L7_uint8              isDynamic;
#endif //ONE_MULTICAST_VLAN_RING_SUPPORT

  PT_LOG_TRACE(LOG_CTX_IGMP, "Context [payLoad:%p payloadLength:%u serviceId:%u portId:%u clientId:%u family:%u]", payload, payloadLength, serviceId, portId, clientId, family);

#if (PTIN_BOARD_IS_LINECARD || PTIN_BOARD_IS_STANDALONE)
  {
    ptin_prottypeb_intf_config_t protTypebIntfConfig = {0};

    /* Get the protection status of this switch port */
    /* FIXME TC16SXG: intIfNum->ptin_port */
    ptin_prottypeb_intf_config_get(ptin_port, &protTypebIntfConfig);
    if( protTypebIntfConfig.intfRole != PROT_TYPEB_ROLE_NONE &&  protTypebIntfConfig.status != L7_ENABLE)
    {
      if (ptin_debug_igmp_snooping)
        PT_LOG_NOTICE(LOG_CTX_IGMP,"Silently ignoring packet transmission. I'm a Protection Port [ptin_port=%u serviceId=%u]",ptin_port, serviceId );
      return SUCCESS;
    }
  }
#else
  #error "Not Implemented Yet"
#endif

  rc = ptin_intf_port2intIfNum(ptin_port, &intIfNum);
  if (rc != L7_SUCCESS)
  {
     PT_LOG_ERR(LOG_CTX_IGMP, "Error getting intIfNum");
     return FAILURE;
  }

  //Ignore if the port has link down
  if ( (nimGetIntfActiveState(intIfNum, &activeState) != L7_SUCCESS) || (activeState != L7_ACTIVE) )
  {
    if (ptin_debug_igmp_snooping)
      PT_LOG_NOTICE(LOG_CTX_IGMP,"Silently ignoring packet transmission. Outgoing interface [intIfNum=%u serviceId=%u] is down!",intIfNum,serviceId );
    return SUCCESS;
  }

  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get__snooping_old(&igmpCfg) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_IGMP, "Error getting IGMP Proxy configurations");
  }
  //Workaround to support Group Specific Queries; IPv6 is not complaint with this approach!
#if (!PTIN_BOARD_IS_MATRIX && (defined (IGMP_QUERIER_IN_UC_EVC)))
  if (ptin_port >= PTIN_SYSTEM_N_PONS)
  {
    portType = PTIN_MGMD_PORT_TYPE_ROOT;
  }
  else
  {
    portType = PTIN_MGMD_PORT_TYPE_LEAF;
  }

#ifdef ONE_MULTICAST_VLAN_RING_SUPPORT
  ptin_igmp_port_is_Dynamic(ptin_port, &isDynamic);

  PT_LOG_TRACE(LOG_CTX_IGMP,"RING: port is Dynamic %d ",isDynamic);

#endif //ONE_MULTICAST_VLAN_RING_SUPPORT
  //Get Group Address
  destIpPtr = (payload+28);
  SNOOP_GET_ADDR(&groupAddress, destIpPtr);

  //We only get the intRootVLAN here for the General Query and for the Membership Reports
  //For Group Specific Queries we use the  ptin_mgmd_send_leaf_packet to obtain the intRootVLAN
  if ( (portType == PTIN_MGMD_PORT_TYPE_ROOT || groupAddress==0x00) )
#endif
  {
    //Get outter internal vlan
    if( SUCCESS != ptin_evc_intRootVlan_get(serviceId, &int_ovlan))
    {
      PT_LOG_ERR(LOG_CTX_IGMP,"Unable to get mcastRootVlan from serviceId");
      return FAILURE;
    }
  }
#ifdef ONE_MULTICAST_VLAN_RING_SUPPORT
  else if( portType == PTIN_MGMD_PORT_TYPE_LEAF && isDynamic)
  {
    PT_LOG_TRACE(LOG_CTX_IGMP,"portType == PTIN_MGMD_PORT_TYPE_LEAF && isDynamic");
     //Get outter internal vlan
    if( SUCCESS != ptin_evc_intRootVlan_get(serviceId, &int_ovlan))
    {
      PT_LOG_TRACE(LOG_CTX_IGMP,"Unable to get mcastRootVlan from serviceId");
      return FAILURE;
    }
  }
#endif //ONE_MULTICAST_VLAN_RING_SUPPORT
  #if (!PTIN_BOARD_IS_MATRIX && (defined (IGMP_QUERIER_IN_UC_EVC)))
  else
  {
    if (clientId != (unsigned int) -1)
    {
      /* Convert to ptin_port format */
      if (ptin_port >= PTIN_SYSTEM_N_INTERF)
      {
        PT_LOG_ERR(LOG_CTX_IGMP,"Cannot convert intIfNum %u to ptin_port format", ptin_port);
        return L7_FAILURE;
      }

      if( igmp_intVlan_from_clientId_get(ptin_port, clientId, &int_ovlan) != L7_SUCCESS
          || int_ovlan<PTIN_VLAN_MIN || int_ovlan>PTIN_VLAN_MAX)
      {
        PT_LOG_ERR(LOG_CTX_IGMP,"Cannot obtain int_ovlan (%u) from client id:%u", int_ovlan, clientId);
        return L7_FAILURE;
      }
      if (ptin_debug_igmp_snooping)
        PT_LOG_TRACE(LOG_CTX_IGMP,"Obtained int_ovlan (%u) from client id:%u", int_ovlan, clientId);
    }
  }
  #endif

  //Get destination MAC from destIpAddr
  destIpPtr = (payload+16);
  SNOOP_GET_ADDR(&dstIpAddr, destIpPtr);

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
#ifdef ONE_MULTICAST_VLAN_RING_SUPPORT
  //if ( portType == PTIN_MGMD_PORT_TYPE_ROOT  || ( portType == PTIN_MGMD_PORT_TYPE_LEAF && isDynamic))
  L7_uint8 local_router_port_id;
  if ( portType == PTIN_MGMD_PORT_TYPE_ROOT /* LRP */ || 
       (portType == PTIN_MGMD_PORT_TYPE_LEAF && isDynamic && (ptin_igmp_get_local_router_port(&local_router_port_id) == L7_SUCCESS)/* Portas de uplink client */) )
#else
  if ( portType == PTIN_MGMD_PORT_TYPE_ROOT )
#endif
  #endif
  {
    ptin_timer_start(31,"snoopPacketSend");
    //Send packet
    snoopPacketSend(ptin_port, int_ovlan, int_ivlan, packet, packetLength, family, clientId);
    ptin_timer_stop(31);
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
          PT_LOG_WARN(LOG_CTX_IGMP,"Either mgmdNumberOfQueryInstances [%u] >= PTIN_SYSTEM_N_EVCS [%u] or mgmdQueryInstances=%p",mgmdNumberOfQueryInstances,PTIN_SYSTEM_N_EVCS,mgmdQueryInstancesPtr);
          mgmdNumberOfQueryInstances=0;
          return SUCCESS;
        }

        if (ptin_debug_igmp_snooping)
          PT_LOG_TRACE(LOG_CTX_IGMP,"Going to send %u Group Specific Queries",mgmdNumberOfQueryInstances);

        while(mgmdQueryInstancesPtr!=L7_NULLPTR)
        {
          if (mgmdQueryInstancesPtr->inUse==L7_TRUE)
          {
            ++numberOfQueriesSent;
            //Get outter internal vlan

            if( SUCCESS != ptin_evc_intRootVlan_get(mgmdQueryInstancesPtr->UcastEvcId, &int_ovlan))
            {
              PT_LOG_ERR(LOG_CTX_IGMP,"Unable to get mcastRootVlan from serviceId");
              return FAILURE;
            }
            ptin_mgmd_send_leaf_packet(portId, int_ovlan, int_ivlan, packet, packetLength, family, clientId, onuId);
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
        ptin_timer_start(31,"snoopPacketSend");
        //Send packet
        snoopPacketSend(ptin_port, int_ovlan, int_ivlan, packet, packetLength, family, clientId);
        ptin_timer_stop(31);
      }
    }
    else //General Query
    {
      ptin_mgmd_send_leaf_packet(portId, int_ovlan, int_ivlan, packet, packetLength, family, clientId, onuId);
    }
  }
  #endif

  return SUCCESS;
#endif
}

#if (!PTIN_BOARD_IS_MATRIX && (defined (IGMP_QUERIER_IN_UC_EVC)))
L7_RC_t ptin_mgmd_send_leaf_packet(uint32 portId, L7_uint16 int_ovlan, L7_uint16 int_ivlan, L7_uchar8 *payload, L7_uint32 payloadLength,uchar8 family, L7_uint client_idx, L7_uint32 onuId)
{
  ptin_HwEthEvcFlow_t   clientFlow;
  L7_RC_t               rc;
  L7_uchar8             packet[L7_MAX_FRAME_SIZE];
  L7_uint32             packetLength, ptin_port;

  ptin_port = portId -1;
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
       rc = ptin_evc_vlan_client_next(int_ovlan, ptin_port, &clientFlow, &clientFlow);
       if (ptin_debug_igmp_snooping)
       {
         PT_LOG_TRACE(LOG_CTX_IGMP,"onuId=%d", onuId);
         PT_LOG_TRACE(LOG_CTX_IGMP,"clientFlow.onuId=%d", clientFlow.onuId);
       }
       if (ptin_debug_igmp_snooping)
       {
         PT_LOG_TRACE(LOG_CTX_IGMP,"onuId=%d", onuId);
         PT_LOG_TRACE(LOG_CTX_IGMP,"clientFlow.onuId=%d", clientFlow.onuId);
       }

       if ( (onuId != (L7_uint32) -1) && onuId != clientFlow.onuId ) 
       {
         continue;
       }
       
      /* Internal vlans */
      int_ivlan = clientFlow.int_ivid;

      if (ptin_debug_igmp_snooping)
        PT_LOG_TRACE(LOG_CTX_IGMP,"rc=%d", rc);

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
            PT_LOG_TRACE(LOG_CTX_IGMP,"Packet will be transmited for client cvlan=%u (client_idx=%u) in intIfNum=%u (intVlan=%u)",
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
          PT_LOG_TRACE(LOG_CTX_IGMP, "Packet will be transmited for ptin_port=%u (intVlan=%u)", ptin_port, int_ovlan);
        }
      }
      else
      {
        /* An error ocurred */
        if (ptin_debug_igmp_snooping)
        {
          PT_LOG_TRACE(LOG_CTX_IGMP,"No more transmissions for ptin_port=%u (intVlan=%u), rc=%u", ptin_port, int_ovlan, rc);
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
      ptin_timer_start(31,"snoopPacketSend"); 
      //Send packet
      snoopPacketSend(ptin_port, int_ovlan, int_ivlan, packet, packetLength, family, client_idx);
      ptin_timer_stop(31);
    }
  } while (rc==L7_SUCCESS);   /* Next client? */  
  return rc;
}
#endif

#if (PTIN_BOARD_IS_MATRIX || PTIN_BOARD_IS_LINECARD)
static msg_SnoopSyncReply_t snoopSyncReply[IPCLIB_MAX_MSGSIZE/sizeof(msg_SnoopSyncReply_t)];

#if PTIN_SYSTEM_IGMP_L3_MULTICAST_FORWARD
#if PTIN_BOARD_IS_MATRIX 
L7_RC_t ptin_snoop_l3_sync_mx_process_request(L7_uint16 vlanId, L7_inet_addr_t *groupAddr, L7_inet_addr_t *sourceAddr)
{
  L7_uint32                   maxNumberOfSnoopEntries  = IPCLIB_MAX_MSGSIZE/sizeof(msg_SnoopSyncReply_t); //IPC buffer size / struct size
  L7_uint32                   numberOfSnoopEntries     = avlTreeCount(&(snoopEBGet()->snoopChannelAvlTree));    
  L7_uint16                   internalRootVlan = (L7_uint16) -1;  
  L7_uint32                   serviceId        = 0;//Invalid Extended Service Id  
  snoopChannelInfoData_t     *snoopChannelInfoData;
  snoopChannelInfoDataKey_t   snoopChannelInfoDataKey;
  snoopChannelInfoDataKey_t  *snoopChannelInfoDataKeyPtr;
  L7_BOOL                     firstEntry = L7_FALSE;
  char                        groupAddrStr[IPV6_DISP_ADDR_LEN]={};
  char                        sourceAddrStr[IPV6_DISP_ADDR_LEN]={};
  L7_uint32                   ipAddr;
  
  if(!ptin_fpga_mx_is_matrixactive_rt())//I'm Standby Matrix
  { 
    PT_LOG_NOTICE(LOG_CTX_IGMP, "Silently Ignoring Snoop Sync Request. I'm a standby Matrix!");
    return L7_SUCCESS;
  }
  else
  {
    /* Standby MX board IP address */
    ipAddr = IPC_MX_PAIR_IPADDR;
  }

  if (numberOfSnoopEntries==0)
  {
    PT_LOG_NOTICE(LOG_CTX_IGMP, "Silently Ignoring Snoop Sync Request. L3 Multicast Table is Empty");
    return L7_SUCCESS;
  }
  else
  {
    if (ptin_debug_igmp_snooping)
      PT_LOG_TRACE(LOG_CTX_IGMP,"Max Number of L3 Multicast Entries:%u",PTIN_SYSTEM_IGMP_L3_MULTICAST_MAX_ENTRIES);
    if (ptin_debug_igmp_snooping)
      PT_LOG_TRACE(LOG_CTX_IGMP,"Number of Existing Snoop Entries (%u) | Maximum number of snoop entries (%u)",numberOfSnoopEntries,maxNumberOfSnoopEntries);

    //Initialize SnoopSyncReply Structure
    if(numberOfSnoopEntries < maxNumberOfSnoopEntries)
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
  if(vlanId==0 && inetIsAddressZero(groupAddr) && inetIsAddressZero( sourceAddr))
  {
    firstEntry = L7_TRUE;
    memset(&snoopChannelInfoDataKey,0x00,sizeof(snoopInfoDataKey_t));
  }
  else
  {
    snoopChannelInfoDataKey.vlanId = vlanId;
    memcpy(&snoopChannelInfoDataKey.groupAddr, groupAddr, sizeof(snoopChannelInfoDataKey.groupAddr));
    memcpy(&snoopChannelInfoDataKey.sourceAddr, sourceAddr, sizeof(snoopChannelInfoDataKey.sourceAddr));
  }
   
  while ( ( snoopChannelInfoData = (snoopChannelInfoData_t *) avlSearchLVL7( &(snoopEBGet()->snoopChannelAvlTree), (void *) &snoopChannelInfoDataKey, AVL_NEXT) ) != L7_NULL && 
          numberOfSnoopEntries<maxNumberOfSnoopEntries)
  {
    /* Prepare next key */
    memcpy( &snoopChannelInfoDataKey, &snoopChannelInfoData->snoopChannelInfoDataKey, sizeof(snoopChannelInfoDataKey) );

    snoopChannelInfoDataKeyPtr  = &snoopChannelInfoData->snoopChannelInfoDataKey;    
    if (ptin_debug_igmp_snooping)
    {
      inetAddrPrint(&snoopChannelInfoDataKeyPtr->groupAddr, groupAddrStr);
      inetAddrPrint(&snoopChannelInfoDataKeyPtr->sourceAddr, sourceAddrStr);
      PT_LOG_TRACE(LOG_CTX_IGMP,"snoopChannelEntry: vlanId:%u groupAddr:%s sourceAddr:%s", snoopChannelInfoDataKeyPtr->vlanId, groupAddrStr, sourceAddrStr);
    }

    /*If this is a static entry move to the next entry*/
    if ( (snoopChannelInfoData->flags  & SNOOP_CHANNEL_ENTRY_IS_STATIC) == SNOOP_CHANNEL_ENTRY_IS_STATIC)
      continue;

    if( internalRootVlan != snoopChannelInfoDataKeyPtr->vlanId)
    {
      internalRootVlan = snoopChannelInfoDataKeyPtr->vlanId;

      if (ptin_evc_get_evcIdfromIntVlan(internalRootVlan, &serviceId) != L7_SUCCESS)
      {
        PT_LOG_WARN(LOG_CTX_IGMP,"  Failed to obtain extended service Id from internal root vlan [internalRootVlan=%u]",internalRootVlan);
        continue;
      }
    }

    snoopSyncReply[numberOfSnoopEntries].serviceId = serviceId;
    fp_to_ptin_ip_notation(&snoopChannelInfoDataKeyPtr->groupAddr, &snoopSyncReply[numberOfSnoopEntries].groupAddr);
    fp_to_ptin_ip_notation(&snoopChannelInfoDataKeyPtr->sourceAddr, &snoopSyncReply[numberOfSnoopEntries].sourceAddr);
    
    if ( (snoopChannelInfoData->flags & SNOOP_CHANNEL_ENTRY_IS_STATIC) == SNOOP_CHANNEL_ENTRY_IS_STATIC)
      snoopSyncReply[numberOfSnoopEntries].isStatic = L7_TRUE;
    
    snoopSyncReply[numberOfSnoopEntries].numberOfActivePorts = snoopChannelInfoData->noOfInterfaces;
    {
        NIM_INTF_MASK_t nmask;
        ptin_intf_portbmp2intIfNumMask(&snoopChannelInfoData->channelPtinPortMask, &nmask);
        memcpy(&snoopSyncReply[numberOfSnoopEntries].intIfNum_mask, &nmask, sizeof(snoopSyncReply[numberOfSnoopEntries].intIfNum_mask)); 
    }
   
    /* Next Snoop entry */
    numberOfSnoopEntries++;       
  }

  if (numberOfSnoopEntries>0)
  {    
    PT_LOG_INFO(LOG_CTX_IGMP, "Sending a Snoop Sync Reply Message to ipAddr:%08X (%u) with %u snoop Entries  to sync the standby matrix",ipAddr, MX_PAIR_SLOT_ID, numberOfSnoopEntries);
    if(firstEntry)
    {
      PT_LOG_DEBUG(LOG_CTX_IGMP, "Remaining Snoop Entries to be Sync:%u",avlTreeCount(&(snoopEBGet()->snoopChannelAvlTree))-numberOfSnoopEntries);     
    }
    
    /*Send the snoop sync request to the protection matrix */  
    if (send_ipc_message(IPC_HW_FASTPATH_PORT, ipAddr, CCMSG_MGMD_SNOOP_SYNC_REPLY,
                         (char *)(&snoopSyncReply), NULL,
                         numberOfSnoopEntries*sizeof(msg_SnoopSyncReply_t), NULL) != 0)
    {
      PT_LOG_ERR(LOG_CTX_IGMP, "Failed to send Snoop Sync Reply Message");
      return L7_FAILURE;
    }
  }
  else
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "No more Snoop Entries Remaining to be Sync");    
  }

  return L7_SUCCESS; 
}
#else//!PTIN_BOARD_IS_MATRIX 
L7_RC_t ptin_snoop_l3_sync_port_process_request(L7_uint16 vlanId, L7_inet_addr_t *groupAddr, L7_inet_addr_t *sourceAddr, L7_uint32 portId)
{
  L7_uint32                       maxNumberOfSnoopEntries  = IPCLIB_MAX_MSGSIZE/sizeof(msg_SnoopSyncReply_t); //IPC buffer size / struct size
  L7_uint32                       numberOfSnoopEntries     = avlTreeCount(&(snoopEBGet()->snoopChannelAvlTree));    
  L7_uint16                       internalRootVlan = (L7_uint16) -1;  
  L7_uint32                       serviceId        = 0;//Invalid Extended Service Id    
  snoopChannelInfoData_t         *snoopChannelInfoData;
  snoopChannelInfoDataKey_t       snoopChannelInfoDataKey;
  snoopChannelInfoDataKey_t      *snoopChannelInfoDataKeyPtr;  
  char                            groupAddrStr[IPV6_DISP_ADDR_LEN]={};
  char                            sourceAddrStr[IPV6_DISP_ADDR_LEN]={};
  L7_uint32                       ipAddr;                               
  ptin_prottypeb_intf_config_t    protTypebIntfConfig = {0};
  L7_uint32                       ptin_port = portId -1;
  
  /* Get the configuration of this portId for the Type B Scheme Protection */
  ptin_prottypeb_intf_config_get(ptin_port, &protTypebIntfConfig);   

  if(protTypebIntfConfig.status == L7_ENABLE) //I'm Working
  {
    #if PTIN_BOARD_IS_STANDALONE
    ipAddr = simGetIpcIpAddr();
    #else
    /* Determine the IP address of the protection port/slot */    
    if (L7_SUCCESS != ptin_fpga_slot_ip_addr_get(protTypebIntfConfig.pairSlotId, &ipAddr))
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Failed to obtain ipAddress of slotId:%u", protTypebIntfConfig.pairSlotId);
      return L7_FAILURE;
    }
    #endif
  }
  else
  { //I'm Standby
    PT_LOG_NOTICE(LOG_CTX_IGMP, "Silently Ignoring Snoop Sync Request. I'm a standby Port");
    return L7_SUCCESS;
  }

  if (numberOfSnoopEntries==0)
  {
    PT_LOG_NOTICE(LOG_CTX_IGMP, "Silently Ignoring Snoop Sync Request. L3 Multicast Table is Empty");
    return L7_SUCCESS;
  }
  else
  {
    PT_LOG_TRACE(LOG_CTX_IGMP,"Max Number of L3 Multicast Entries:%u",PTIN_SYSTEM_IGMP_L3_MULTICAST_MAX_ENTRIES);

    PT_LOG_TRACE(LOG_CTX_IGMP,"Number of Existing Snoop Entries (%u) | Maximum number of snoop entries (%u)",numberOfSnoopEntries,maxNumberOfSnoopEntries);
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
  if(vlanId==0 && inetIsAddressZero(groupAddr) && inetIsAddressZero( sourceAddr))
  {   
    memset(&snoopChannelInfoDataKey,0x00,sizeof(snoopChannelInfoDataKey));
  }
  else
  {
    snoopChannelInfoDataKey.vlanId = vlanId;
    memcpy(&snoopChannelInfoDataKey.groupAddr, groupAddr, sizeof(snoopChannelInfoDataKey.groupAddr));
    memcpy(&snoopChannelInfoDataKey.sourceAddr, sourceAddr, sizeof(snoopChannelInfoDataKey.sourceAddr));
  }
   
  while ( ( snoopChannelInfoData = (snoopChannelInfoData_t *) avlSearchLVL7( &(snoopEBGet()->snoopChannelAvlTree), (void *) &snoopChannelInfoDataKey, AVL_NEXT) ) != L7_NULL && 
          numberOfSnoopEntries<maxNumberOfSnoopEntries)
  {
    /* Prepare next key */
    memcpy( &snoopChannelInfoDataKey, &snoopChannelInfoData->snoopChannelInfoDataKey, sizeof(snoopChannelInfoDataKey) );

    snoopChannelInfoDataKeyPtr  = &snoopChannelInfoData->snoopChannelInfoDataKey;    

    if (PTINPORT_BITMAP_IS_SET(snoopChannelInfoData->channelPtinPortMask, ptin_port))
    {
      /*If this is a static entry move to the next entry*/
      if ( (snoopChannelInfoData->flags & SNOOP_CHANNEL_ENTRY_IS_STATIC) == SNOOP_CHANNEL_ENTRY_IS_STATIC)
        continue;

      if (internalRootVlan != snoopChannelInfoDataKeyPtr->vlanId)
      {
        internalRootVlan = snoopChannelInfoDataKeyPtr->vlanId;

        if (ptin_evc_get_evcIdfromIntVlan(internalRootVlan, &serviceId) != L7_SUCCESS)
        {
          PT_LOG_WARN(LOG_CTX_IGMP,"  Failed to obtain extended service Id from internal root vlan [internalRootVlan=%u]",internalRootVlan);
          continue;
        }
      }

      if (ptin_debug_igmp_snooping)
      {
        inetAddrPrint(&snoopChannelInfoDataKeyPtr->groupAddr, groupAddrStr);
        inetAddrPrint(&snoopChannelInfoDataKeyPtr->sourceAddr, sourceAddrStr);
        PT_LOG_TRACE(LOG_CTX_IGMP,"snoopChannelEntry: vlanId:%u groupAddr:%s sourceAddr:%s", snoopChannelInfoDataKeyPtr->vlanId, groupAddrStr, sourceAddrStr);
      }

      snoopSyncReply[numberOfSnoopEntries].serviceId = serviceId;
      fp_to_ptin_ip_notation(&snoopChannelInfoDataKeyPtr->groupAddr, &snoopSyncReply[numberOfSnoopEntries].groupAddr);
      fp_to_ptin_ip_notation(&snoopChannelInfoDataKeyPtr->sourceAddr, &snoopSyncReply[numberOfSnoopEntries].sourceAddr);
      /* FIXME TC16SXG: intIfNum->ptin_port */
      snoopSyncReply[numberOfSnoopEntries].portId = protTypebIntfConfig.pairPtinPort;
    
      if ( (snoopChannelInfoData->flags & SNOOP_CHANNEL_ENTRY_IS_STATIC) == SNOOP_CHANNEL_ENTRY_IS_STATIC)
        snoopSyncReply[numberOfSnoopEntries].isStatic = L7_TRUE;

      
      /* Next Snoop entry */
      numberOfSnoopEntries++;       
    }    
  }

  if (numberOfSnoopEntries>0)
  {    
    PT_LOG_DEBUG(LOG_CTX_IGMP, "Sending a Snoop Sync Reply Message ipAddr:%08X with %u Snoop Entries  to sync slot/port:%u/%u",
                 ipAddr, numberOfSnoopEntries, protTypebIntfConfig.pairSlotId, protTypebIntfConfig.pairPtinPort);
    /*Send the snoop sync request to the protection matrix */  
    if (send_ipc_message(IPC_HW_FASTPATH_PORT, ipAddr, CCMSG_MGMD_SNOOP_SYNC_REPLY,
                         (char *)(&snoopSyncReply), NULL,
                         numberOfSnoopEntries*sizeof(msg_SnoopSyncReply_t), NULL) != 0)
    {
      PT_LOG_ERR(LOG_CTX_IGMP, "Failed to send Snoop Sync Reply Message");
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS; 
}
#endif//!PTIN_BOARD_IS_MATRIX 

#else//!PTIN_SYSTEM_IGMP_L3_MULTICAST_FORWARD
#if PTIN_BOARD_IS_MATRIX 
L7_RC_t ptin_snoop_sync_mx_process_request(L7_uint16 vlanId, L7_uint32 groupAddr)
{
  L7_uint32                        maxNumberOfSnoopEntries  = IPCLIB_MAX_MSGSIZE/sizeof(msg_SnoopSyncReply_t); //IPC buffer size / struct size
  L7_uint32                        numberOfSnoopEntries     = avlTreeCount(&(snoopEBGet()->snoopAvlTree));  
  L7_uint32                        intIfNum;
  L7_uint16                        internalRootVlan = (L7_uint16) -1;
  L7_uint16                        internalRootVlanTmp;
  L7_uint32                        serviceId        = 0;//Invalid Extended Service Id
  L7_uint32                        channel;
  
  snoopInfoData_t                 *snoopInfoData;
  snoopInfoDataKey_t               snoopInfoDataKey;
  snoopInfoDataKey_t              *snoopInfoDataKeyPtr;

  L7_uint32                        ipAddr;
  
  if(!ptin_fpga_mx_is_matrixactive_rt())//I'm a Standby Matrix
  { 
    PT_LOG_NOTICE(LOG_CTX_PROTB, "Silently Ignoring Snoop Sync Request. I'm a standby Matrix!");
    return L7_SUCCESS;
  }
  else
  {
    /* Standby MX board IP address */
    ipAddr = IPC_MX_PAIR_IPADDR;
  }

  if (numberOfSnoopEntries==0)
  {
    PT_LOG_NOTICE(LOG_CTX_PROTB, "Silently Ignoring Snoop Sync Request. L3 Multicast Table is Empty");
    return L7_SUCCESS;
  }
  else
  {
    if (ptin_debug_igmp_snooping)
      PT_LOG_TRACE(LOG_CTX_PROTB,"Max Number of MAC MFDB Entries:%u",L7_MFDB_MAX_MAC_ENTRIES);
    if (ptin_debug_igmp_snooping)
      PT_LOG_TRACE(LOG_CTX_PROTB,"Number of Existing Snoop Entries (%u) | Maximum number of snoop entries (%u)",numberOfSnoopEntries,maxNumberOfSnoopEntries);

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
        PT_LOG_WARN(LOG_CTX_IGMP,"  Failed to obtain extended service Id from internal root vlan [internalRootVlan=%u]",internalRootVlan);
        continue;
      }
    }
    
    if (ptin_debug_igmp_snooping)
    {
       /* Global information */    
      PT_LOG_TRACE(LOG_CTX_IGMP,"  Family=%s     Vlan=%-4u MAC=%02x:%02x:%02x:%02x:%02x:%02x   %s",
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
      PT_LOG_TRACE(LOG_CTX_IGMP,"  IntfNUm Ports information:");
      for (intIfNum = 1; intIfNum <= L7_MAX_INTERFACE_COUNT; intIfNum++)
      {
        if (L7_INTF_ISMASKBITSET(snoopInfoData->snoopGrpMemberList,intIfNum))
        {
          PT_LOG_TRACE(LOG_CTX_IGMP," %u",intIfNum);
        }
      }
    }

    /* Channels information */
    if (ptin_debug_igmp_snooping)
      PT_LOG_TRACE(LOG_CTX_IGMP,"  Channels information:");
    for (channel=0; channel<SNOOP_MAX_CHANNELS_PER_SNOOP_ENTRY; channel++)
    {
      //Only Sync Active Channels
      if (!snoopInfoData->channel_list[channel].active)  continue;
#if 0 //All Entries Added by Mgmd are Static!
      //Only Sync Dynamic Channels
      if (snoopInfoData->staticGroup == L7_FALSE) continue;
#endif

      snoopSyncReply[numberOfSnoopEntries].serviceId           = serviceId;
      snoopSyncReply[numberOfSnoopEntries].groupAddr           = snoopInfoData->channel_list[channel].ipAddr;
      snoopSyncReply[numberOfSnoopEntries].isStatic            = snoopInfoData->staticGroup;
      snoopSyncReply[numberOfSnoopEntries].numberOfActivePorts = snoopInfoData->channel_list[channel].number_of_ports;
     
      memcpy(&snoopSyncReply[numberOfSnoopEntries].intIfNum_mask, &snoopInfoData->channel_list[channel].intIfNum_mask, sizeof(snoopSyncReply[numberOfSnoopEntries].intIfNum_mask)); 

      if (ptin_debug_igmp_snooping)
      {
        PT_LOG_TRACE(LOG_CTX_IGMP,"    Channel#%-2u:       IpAddr=%u.%u.%u.%u",channel,
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
    PT_LOG_INFO(LOG_CTX_MSG, "Sending a Snoop Sync Reply Message to ipAddr:%08X (%u) with %u snoop Entries  to sync the protection matrix",ipAddr, MX_PAIR_SLOTID, numberOfSnoopEntries);
    if(vlanId==0 && groupAddr==0)
    {
      PT_LOG_DEBUG(LOG_CTX_MSG, "Remaining Snoop Entries to be Sync:%u",avlTreeCount(&(snoopEBGet()->snoopAvlTree))-numberOfSnoopEntries);     
    }
    
    /*Send the snoop sync request to the protection matrix */  
    if (send_ipc_message(IPC_HW_FASTPATH_PORT, ipAddr, CCMSG_MGMD_SNOOP_SYNC_REPLY,
                         (char *)(&snoopSyncReply), NULL,
                         numberOfSnoopEntries*sizeof(msg_SnoopSyncReply_t), NULL) != 0)
    {
      PT_LOG_ERR(LOG_CTX_PROTB, "Failed to send Snoop Sync Reply Message");
      return L7_FAILURE;
    }
  }
  else
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "No more Snoop Entries Remaining to be Sync");    
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
  L7_uint32                      ptin_port = portId -1;

  snoopInfoData_t               *snoopInfoData;
  snoopInfoDataKey_t             snoopInfoDataKey;
  snoopInfoDataKey_t            *snoopInfoDataKeyPtr;
  L7_uint32                      ipAddr;  
                             
  ptin_prottypeb_intf_config_t   protTypebIntfConfig = {0};
  
  /* Get the configuration of this portId for the Type B Scheme Protection */
  ptin_prottypeb_intf_config_get(ptin_port, &protTypebIntfConfig);   

  if(protTypebIntfConfig.status == L7_ENABLE) //I'm Working
  {
    /* Determine the IP address of the protection port/slot */   
    if (L7_SUCCESS != ptin_fpga_slot_ip_addr_get(protTypebIntfConfig.pairSlotId, &ipAddr))
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Failed to obtain ipAddress of slotId:%u", protTypebIntfConfig.pairSlotId);
      return L7_FAILURE;
    }
  }
  else
  { //I'm Protection
    PT_LOG_NOTICE(LOG_CTX_PROTB, "Silently Ignoring Snoop Sync Request. I'm a Protection Port");
    return L7_SUCCESS;
  }

  if (numberOfSnoopEntries==0)
  {
    PT_LOG_NOTICE(LOG_CTX_PROTB, "Silently Ignoring Snoop Sync Request. Snoop Table is Empty");
    return L7_SUCCESS;
  }
  else
  {
    PT_LOG_TRACE(LOG_CTX_PROTB,"Max Number of MAC MFDB Entries:%u",L7_MFDB_MAX_MAC_ENTRIES);

    PT_LOG_TRACE(LOG_CTX_PROTB,"Number of Existing Snoop Entries (%u) | Maximum number of snoop entries (%u)",numberOfSnoopEntries,maxNumberOfSnoopEntries);
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
          PT_LOG_WARN(LOG_CTX_IGMP,"  Failed to obtain extended service Id from internal root vlan [internalRootVlan=%u]",internalRootVlan);
          continue;
        }
      }

      /* Global information */    
      PT_LOG_TRACE(LOG_CTX_PROTB,"  Family=%s     Vlan=%-4u MAC=%02x:%02x:%02x:%02x:%02x:%02x   %s",
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
      PT_LOG_TRACE(LOG_CTX_PROTB,"  Channels information:");
      for (channel=0; channel<SNOOP_MAX_CHANNELS_PER_SNOOP_ENTRY; channel++)
      {
        //Only Sync Active Channels
        if (!snoopInfoData->channel_list[channel].active)  continue;

        //Only Sync Dynamic Channels
        if (snoopInfoData->staticGroup == L7_FALSE) continue;

        snoopSyncReply[numberOfSnoopEntries].serviceId           = serviceId;
        snoopSyncReply[numberOfSnoopEntries].groupAddr           = snoopInfoData->channel_list[channel].ipAddr;
        snoopSyncReply[numberOfSnoopEntries].isStatic            = snoopInfoData->staticGroup);
        snoopSyncReply[numberOfSnoopEntries].portId              = protTypebIntfConfig.pairIntfNum;

        PT_LOG_TRACE(LOG_CTX_PROTB,"    Channel#%-2u:       IpAddr=%u.%u.%u.%u",channel,
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
    
    PT_LOG_DEBUG(LOG_CTX_MSG, "Sending a Snoop Sync Reply Message ipAddr:%08X with %u Snoop Entries  to sync slot/port:%u/%u",ipAddr, numberOfSnoopEntries, protTypebIntfConfig.pairSlotId, protTypebIntfConfig.pairIntfNum);
    /*Send the snoop sync request to the protection matrix */  
    if (send_ipc_message(IPC_HW_FASTPATH_PORT, ipAddr, CCMSG_MGMD_SNOOP_SYNC_REPLY,
                         (char *)(&snoopSyncReply), NULL,
                         numberOfSnoopEntries*sizeof(msg_SnoopSyncReply_t), NULL) != 0)
    {
      PT_LOG_ERR(LOG_CTX_PROTB, "Failed to send Snoop Sync Reply Message");
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS; 
}
#endif//!PTIN_BOARD_IS_MATRIX 
#endif//!PTIN_SYSTEM_IGMP_L3_MULTICAST_FORWARD
#endif//(PTIN_BOARD_IS_MATRIX || PTIN_BOARD_IS_LINECARD)

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
  PTIN_MGMD_CTRL_MGMD_STATUS_t mgmdStatus; 

  /* Create and send a PTIN_MGMD_EVENT_CTRL_STATUS_GET event to MGMD */
  ptin_mgmd_event_ctrl_create(&inEventMsg, PTIN_MGMD_EVENT_CTRL_STATUS_GET, rand(), 0, ptinMgmdTxQueueId, (void*) &mgmdStatus, (uint32) sizeof(PTIN_MGMD_CTRL_MGMD_STATUS_t));
  ptin_mgmd_sendCtrlEvent(&inEventMsg, &outEventMsg);

  /* Parse the received reply */
  PT_LOG_DEBUG(LOG_CTX_IGMP, "MGMD replied");
  ptin_mgmd_event_ctrl_parse(&outEventMsg, &ctrlResMsg);
  PT_LOG_DEBUG(LOG_CTX_IGMP,  "  CTRL Msg Code: %08X", ctrlResMsg.msgCode);
  PT_LOG_DEBUG(LOG_CTX_IGMP,  "  CTRL Msg Id  : %08X", ctrlResMsg.msgId);
  PT_LOG_DEBUG(LOG_CTX_IGMP,  "  CTRL Res     : %u",   ctrlResMsg.res);
 
  /* Copy the response contents to igmpProxy */
  if(sizeof(PTIN_MGMD_CTRL_MGMD_STATUS_t) != ctrlResMsg.dataLength)
  {
    PT_LOG_ERR(LOG_CTX_IGMP,"Unexpected size in the MGMD response [dataLength:%u/%zu]",
               ctrlResMsg.dataLength, sizeof(PTIN_MGMD_CTRL_MGMD_STATUS_t));
    return L7_FAILURE;
  }
  else
  {
    memcpy(&mgmdStatus, ctrlResMsg.data, ctrlResMsg.dataLength);
    PT_LOG_DEBUG(LOG_CTX_IGMP,  "  Mgmd Status  : %s",   mgmdStatus.mgmdStatus == PTIN_MGMD_STATUS_WORKING ? "Alive":"Dead");
  }

  return ctrlResMsg.res;
}


