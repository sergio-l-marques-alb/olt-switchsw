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
#ifndef SNOOPING_MGMD_API_H
#define SNOOPING_MGMD_API_H

#include "ptin_mgmd_api.h"
#include "ptin_mgmd_defs.h"

RC_t snooping_igmp_admin_set(uint8 admin);
RC_t snooping_mld_admin_set(uint8 admin);

RC_t snooping_cos_set(uint8 admin);

RC_t snooping_portList_get(uint32 serviceId, ptin_mgmd_port_type_t portType, PTIN_MGMD_PORT_MASK_t *portList);
RC_t snooping_portType_get(uint32 serviceId, uint32 portId, ptin_mgmd_port_type_t *portType);

RC_t snooping_clientList_get(uint32 serviceId, uint32 portId, uint8 *clientList);

RC_t snooping_port_open(uint32 serviceId, uint32 portId, uint32 groupAddr, uint32 sourceAddr, BOOL isStatic);
RC_t snooping_port_close(uint32 serviceId, uint32 portId, uint32 groupAddr, uint32 sourceAddr);

RC_t snooping_tx_packet(uchar8 *payLoad, uint32 payloadLength, uint32 serviceId, uint32 portId, uint32 clientId, uchar8 family);

#endif /* SNOOPING_MGMD_API_H */

