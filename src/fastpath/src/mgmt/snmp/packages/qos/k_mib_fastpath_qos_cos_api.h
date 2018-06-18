/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename   k_mib_fastpath_qos_cos_api.h
*
* @purpose    Wrapper functions for Fastpath QOS COS MIB
*
* @component  SNMP
*
* @comments
*
* @create     5/04/2004
*
* @author     cpverne
* @end
*
**********************************************************************/

#include "l7_common.h"

L7_RC_t
snmpAgentCosMapIntfTrustModeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);

L7_RC_t
snmpAgentCosMapIntfTrustModeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val);

/**********************************************************************/
L7_RC_t
snmpAgentCosMapIpPrecEntryGet(L7_uint32 UnitIndex, L7_uint32 agentCosMapIpPrecIntfIndex, L7_uint32 agentCosMapIpPrecValue, L7_uint32 *intIfNum);

L7_RC_t
snmpAgentCosMapIpPrecEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *agentCosMapIpPrecIntfIndex, L7_uint32 *agentCosMapIpPrecValue, L7_uint32 *intIfNum);

/**********************************************************************/

L7_RC_t
snmpAgentCosMapIpDscpEntryGet(L7_uint32 UnitIndex, L7_uint32 agentCosMapIpDscpIntfIndex, L7_uint32 agentCosMapIpDscpValue, L7_uint32 *intIfNum);

L7_RC_t
snmpAgentCosMapIpDscpEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *agentCosMapIpDscpIntfIndex, L7_uint32 *agentCosMapIpDscpValue, L7_uint32 *intIfNum);

/**********************************************************************/

L7_RC_t
snmpAgentCosMapIntfTrustEntryGet(L7_uint32 UnitIndex, L7_uint32 agentCosMapIntfTrustIntfIndex, L7_uint32 *intIfNum);

L7_RC_t
snmpAgentCosMapIntfTrustEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *agentCosMapIntfTrustIntfIndex, L7_uint32 *intIfNum);

/**********************************************************************/

L7_RC_t
snmpAgentCosQueueControlEntryGet(L7_uint32 UnitIndex, L7_uint32 agentCosQueueIntfIndex, L7_uint32 *intIfNum);

L7_RC_t
snmpAgentCosQueueControlEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *agentCosQueueIntfIndex, L7_uint32 *intIfNum);

L7_RC_t
snmpAgentCosQueueMgmtTypeIntfGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);

L7_RC_t
snmpAgentCosQueueMgmtTypeIntfSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val);

L7_RC_t
snmpAgentCosQueueDefaultsRestoreSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val);

/**********************************************************************/

L7_RC_t
snmpAgentCosQueueEntryGet(L7_uint32 UnitIndex, L7_uint32 agentCosQueueIntfIndex, L7_uint32 agentCosQueueIndex, L7_uint32 *intIfNum);

L7_RC_t
snmpAgentCosQueueEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *agentCosQueueIntfIndex, L7_uint32 *agentCosQueueIndex, L7_uint32 *intIfNum);

L7_RC_t
snmpAgentCosQueueSchedulerTypeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 agentCosQueueIndex, L7_uint32 *val);

L7_RC_t
snmpAgentCosQueueSchedulerTypeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 agentCosQueueIndex, L7_uint32 val);

L7_RC_t
snmpAgentCosQueueMinBandwidthGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 agentCosQueueIndex, L7_uint32 *val);

L7_RC_t
snmpAgentCosQueueMinBandwidthSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 agentCosQueueIndex, L7_uint32 val);

L7_RC_t
snmpAgentCosQueueMaxBandwidthGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 agentCosQueueIndex, L7_uint32 *val);

L7_RC_t
snmpAgentCosQueueMaxBandwidthSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 agentCosQueueIndex, L7_uint32 val);

L7_RC_t
snmpAgentCosQueueMgmtTypeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 agentCosQueueIndex, L7_uint32 *val);

L7_RC_t
snmpAgentCosQueueMgmtTypeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 agentCosQueueIndex, L7_uint32 val);

/**********************************************************************/

L7_RC_t
snmpAgentCosQueueMgmtEntryGet(L7_uint32 UnitIndex, L7_uint32 agentCosQueueIntfIndex, L7_uint32 agentCosQueueIndex, L7_uint32 agentCosQueueDropPrecIndex, L7_uint32 *intIfNum);

L7_RC_t
snmpAgentCosQueueMgmtEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *agentCosQueueIntfIndex, L7_uint32 *agentCosQueueIndex, L7_uint32 *agentCosQueueDropPrecIndex, L7_uint32 *intIfNum);

L7_RC_t
snmpAgentCosQueueMgmtTailDropThresholdGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 agentCosQueueIndex, L7_uint32 agentCosQueueDropPrecIndex, L7_uint32 *val);

L7_RC_t
snmpAgentCosQueueMgmtTailDropThresholdSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 agentCosQueueIndex, L7_uint32 agentCosQueueDropPrecIndex, L7_uint32 val);

L7_RC_t
snmpAgentCosQueueMgmtWredMinThresholdGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 agentCosQueueIndex, L7_uint32 agentCosQueueDropPrecIndex, L7_uint32 *val);

L7_RC_t
snmpAgentCosQueueMgmtWredMinThresholdSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 agentCosQueueIndex, L7_uint32 agentCosQueueDropPrecIndex, L7_uint32 val);

L7_RC_t
snmpAgentCosQueueMgmtWredMaxThresholdGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 agentCosQueueIndex, L7_uint32 agentCosQueueDropPrecIndex, L7_uint32 *val);

L7_RC_t
snmpAgentCosQueueMgmtWredMaxThresholdSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 agentCosQueueIndex, L7_uint32 agentCosQueueDropPrecIndex, L7_uint32 val);

L7_RC_t
snmpAgentCosQueueMgmtWredDropProbabilityGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 agentCosQueueIndex, L7_uint32 agentCosQueueDropPrecIndex, L7_uint32 *val);

L7_RC_t
snmpAgentCosQueueMgmtWredDropProbabilitySet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 agentCosQueueIndex, L7_uint32 agentCosQueueDropPrecIndex, L7_uint32 val);

