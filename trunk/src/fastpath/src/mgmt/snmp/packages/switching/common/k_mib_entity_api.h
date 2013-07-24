/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   k_mib_entity_api.h
*
* @purpose    Wrapper functions for Entity MIB - RFC 2737
*
* @component  SNMP
*
* @comments
*
* @create     6/10/2003
*
* @author     cpverne
* @end
*
**********************************************************************/
#include "l7_common.h"

#ifndef K_MIB_ENTITY_API_H
#define K_MIB_ENTITY_API_H

/* Begin Function Prototypes */
L7_RC_t
snmpEntPhysicalEntryGet ( entPhysicalEntry_t *entPhysicalEntryData, L7_int32 nominator);

L7_RC_t
snmpEntPhysicalEntryNextGet ( entPhysicalEntry_t *entPhysicalEntryData, L7_int32 nominator);


L7_RC_t snmpEntPhysicalClassGet(L7_uint32 physicalIndex, L7_uint32 *val);


L7_RC_t snmpEntPhysicalIsFRUGet(L7_uint32 physicalIndex, L7_uint32 *val);


L7_RC_t snmpEntLastChangeTimeGet(L7_uint32 *val);

/* End Function Prototypes */

#endif /* K_MIB_ENTITY_API_H */
