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
  LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Context [admin:%u]", admin);

  // Snooping global activation
  if (L7_SUCCESS != usmDbSnoopAdminModeSet( 1, admin, L7_AF_INET))  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error with usmDbSnoopAdminModeSet");
    return FAILURE;
  }

  return SUCCESS;
}

RC_t snooping_mld_admin_set(uint8 admin)
{
  LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Context [admin:%u]", admin);

  // Snooping global activation
  if (L7_SUCCESS != usmDbSnoopAdminModeSet( 1, admin, L7_AF_INET6))  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error with usmDbSnoopAdminModeSet");
    return FAILURE;
  }

  return SUCCESS;
}

RC_t snooping_cos_set(uint8 cos)
{
  LOG_ERR(LOG_CTX_PTIN_IGMP, "Context [cos:%u]", cos);

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
  LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Context [serviceId:%u portType:%u portList:%p]", serviceId, portType, portList);

  memset(portList, 0x00, sizeof(PTIN_MGMD_PORT_MASK_t));

  return SUCCESS;
}

RC_t snooping_portType_get(uint32 serviceId, uint32 portId, ptin_mgmd_port_type_t *portType)
{
  LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Context [serviceId:%u portId:%u portType:%u]", serviceId, portId, *portType);

  *portType = PTIN_MGMD_PORT_TYPE_LEAF; //Hardcoded for now..

  return SUCCESS;
}

RC_t snooping_clientList_get(uint32 serviceId, uint32 portId, uint8 *clientList)
{
  LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Context [serviceId:%u portId:%u clientList:%p]", serviceId, portId, clientList);

  return SUCCESS;
}

RC_t snooping_port_open(uint32 serviceId, uint32 portId, uint32 groupAddr, uint32 sourceAddr, BOOL isStatic)
{
  LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Context [serviceId:%u portId:%u groupAddr:%08X sourceAddr:%08X isStatic:%u]", serviceId, portId, groupAddr, sourceAddr, isStatic);

  return SUCCESS;
}

RC_t snooping_port_close(uint32 serviceId, uint32 portId, uint32 groupAddr, uint32 sourceAddr)
{
  LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Context [serviceId:%u portId:%u groupAddr:%08X sourceAddr:%08X]", serviceId, portId, groupAddr, sourceAddr);

  return SUCCESS;
}

RC_t snooping_tx_packet(uchar8 *payLoad, uint32 payloadLength, uint32 serviceId, uint32 portId, uint32 clientId, uchar8 family)
{
  LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Context [payLoad:%p payloadLength:%u serviceId:%u portId:%u clientId:%u family:%u]", payLoad, payloadLength, serviceId, portId, clientId, family);

  return SUCCESS;
}

