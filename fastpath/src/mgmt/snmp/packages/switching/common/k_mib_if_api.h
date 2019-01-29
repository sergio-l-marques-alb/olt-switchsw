
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename k_mib_if_api.h
*
* @purpose SNMP specific value conversion
*
* @component SNMP
*
* @comments This file is included at the top of the k_mib_if.c
*
* @create 04/09/2001
*
* @author cpverne
*
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/

#ifndef __K_MIB_IF_API_H
#define __K_MIB_IF_API_H

#include "k_private_base.h"
#include "snmpapi.h"

L7_RC_t 
snmpIfSpeedGet(L7_uint32 UnitIndex, L7_uint32 interface, SR_UINT32 *val);

L7_RC_t 
snmpIfHighSpeedGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

L7_RC_t 
snmpIfOperStatusGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

L7_RC_t 
snmpIfAdminStatusGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

L7_RC_t 
snmpIfAdminStatusSet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 val);

L7_RC_t 
snmpIfPromiscuousModeGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

L7_RC_t 
snmpIfPromiscuousModeSet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 val);

L7_RC_t 
snmpIfConnectorPresentGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

L7_RC_t 
snmpIfLastChangeGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

L7_RC_t 
snmpIfLinkUpDownTrapEnableGet(L7_uint32 UnitIndex, L7_int32 interface, L7_int32 *val);

L7_RC_t 
snmpIfLinkUpDownTrapEnableSet(L7_uint32 UnitIndex, L7_int32 interface, L7_int32 val);

#endif /*__K_MIB_IF_API_H*/
