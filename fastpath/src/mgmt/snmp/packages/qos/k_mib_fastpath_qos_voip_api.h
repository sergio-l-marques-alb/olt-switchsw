/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename   k_mib_fastpath_qos_voip_api.h
*
* @purpose    Wrapper functions for Fastpath QOS VOIP MIB
*
* @component  SNMP
*
* @comments
*
* @create     12/3/2007
*
* @author     cpverne
* @end
*
**********************************************************************/

#include "l7_common.h"

L7_RC_t
snmpAgentAutoVoIPModeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);

L7_RC_t
snmpAgentAutoVoIPModeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val);

L7_RC_t
snmpAgentAutoVoIPMinBandwidthGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);

L7_RC_t
snmpAgentAutoVoIPMinBandwidthSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val);

L7_RC_t
snmpAgentAutoVoIPCosQueueGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *CosQueue);

L7_RC_t
snmpAgentAutoVoIPEntryGet(L7_uint32 UnitIndex, L7_uint32 agentAutoVoIPIntfIndex, L7_uint32 *intIfNum);

L7_RC_t
snmpAgentAutoVoIPEntryNextGet(L7_uint32 UnitIndex, 
                              L7_uint32 *agentAutoVoIPIntfIndex, 
                              L7_uint32 *intIfNum);







