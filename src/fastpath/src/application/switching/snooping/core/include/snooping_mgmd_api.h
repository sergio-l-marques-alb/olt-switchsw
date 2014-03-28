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

unsigned int snooping_igmp_admin_set(unsigned char admin);
unsigned int snooping_mld_admin_set(unsigned char admin);

unsigned int snooping_cos_set(unsigned char cos);

unsigned int snooping_portList_get(unsigned int serviceId, ptin_mgmd_port_type_t portType, PTIN_MGMD_PORT_MASK_t *portList);
unsigned int snooping_portType_get(unsigned int serviceId, unsigned int portId, ptin_mgmd_port_type_t *portType);

unsigned int snooping_clientList_get(unsigned int serviceId, unsigned int portId, PTIN_MGMD_CLIENT_MASK_t *clientList);

unsigned int snooping_port_open(unsigned int serviceId, unsigned int portId, unsigned int groupAddr, unsigned int sourceAddr, unsigned char isStatic);
unsigned int snooping_port_close(unsigned int serviceId, unsigned int portId, unsigned int groupAddr, unsigned int sourceAddr);

unsigned int snooping_tx_packet(unsigned char *payload, unsigned int payloadLength, unsigned int serviceId, unsigned int portId, unsigned int clientId, unsigned char family);

#endif /* SNOOPING_MGMD_API_H */

