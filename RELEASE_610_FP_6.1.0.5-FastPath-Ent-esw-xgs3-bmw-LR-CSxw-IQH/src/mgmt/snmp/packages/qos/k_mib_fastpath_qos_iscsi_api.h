/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename k_mib_fastpath_qos_iscsi.h
*
* @purpose  Wrapper functions for FASTPATH iSCSI MIB
*                  
* @component SNMP
*
* @comments 
*
* @create 10/20/2008
*
* @author jeffr
* @end
*
**********************************************************************/

#include "l7_common.h"

L7_RC_t
snmpAgentIscsiEnableGet(L7_uint32 *val);

L7_RC_t
snmpAgentIscsiAgingTimeOutGet(L7_uint32 *val);

L7_RC_t
snmpAgentIscsiQosTypeGet(L7_uint32 *val);

L7_RC_t
snmpAgentIscsiQosVptValueGet(L7_uint32 *val);

L7_RC_t
snmpAgentIscsiQosDscpValueGet(L7_uint32 *val);

L7_RC_t
snmpAgentIscsiQosRemarkGet(L7_uint32 *val);


L7_RC_t
snmpAgentIscsiEnableSet(L7_uint32 val);

L7_RC_t
snmpAgentIscsiAgingTimeOutSet(L7_uint32 val);

L7_RC_t
snmpAgentIscsiQosTypeSet(L7_uint32 val);

L7_RC_t
snmpAgentIscsiQosVptValueSet(L7_uint32 val);

L7_RC_t
snmpAgentIscsiQosDscpValueSet(L7_uint32 val);

L7_RC_t
snmpAgentIscsiQosRemarkSet(L7_uint32 val);

L7_RC_t
snmpAgentIscsiTargetConfigEntryGet(agentIscsiFlowAccelerationTargetConfigEntry_t *agentIscsiFlowAccelerationTargetConfigEntryData, 
                                   L7_int32 nominator);

L7_RC_t
snmpAgentIscsiTargetConfigEntryGetNext(agentIscsiFlowAccelerationTargetConfigEntry_t *agentIscsiFlowAccelerationTargetConfigEntryData, L7_int32 nominator);

L7_RC_t
snmpAgentIscsiFlowAccelerationTargetConfigIndexNextFreeGet(L7_uint32 *val);

L7_RC_t
snmpAgentIscsiTargetConfigEntrySet(agentIscsiFlowAccelerationTargetConfigEntry_t *agentIscsiFlowAccelerationTargetConfigEntryData);

L7_RC_t
snmpAgentIscsiFlowAccelerationSessionEntryGet(agentIscsiFlowAccelerationSessionEntry_t *agentIscsiFlowAccelerationSessionEntryData, 
                                 L7_int32 nominator);

L7_RC_t
snmpAgentIscsiFlowAccelerationSessionEntryGetNext(agentIscsiFlowAccelerationSessionEntry_t *agentIscsiFlowAccelerationSessionEntryData, L7_int32 nominator);

L7_RC_t
snmpAgentIscsiFlowAccelerationConnectionEntryGet(agentIscsiFlowAccelerationConnectionEntry_t *agentIscsiFlowAccelerationConnectionEntryData, 
                                    L7_int32 nominator);

L7_RC_t
snmpAgentIscsiFlowAccelerationConnectionEntryGetNext(agentIscsiFlowAccelerationConnectionEntry_t *agentIscsiFlowAccelerationConnectionEntryData, L7_int32 nominator);
