/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
* @filename k_mib_fastpathlogging_api.h
*
* @purpose SNMP specific value conversion for FASTPATH Logging MIB
*
* @component SNMP
*
* @create 02/17/2004
*
* @author cpverne
*
* @end
*
**********************************************************************/

#ifndef __K_MIB_FASTPATHLOGGING_API_H__
#define __K_MIB_FASTPATHLOGGING_API_H__


L7_RC_t snmpAgentLogInMemoryAdminStatusGet(L7_uint32 *agentLogInMemoryAdminStatus);

L7_RC_t snmpAgentLogInMemoryAdminStatusSet(L7_uint32 agentLogInMemoryAdminStatus);

L7_RC_t snmpAgentLogInMemoryBehaviorGet(L7_uint32 *agentLogInMemoryBehavior);

L7_RC_t snmpAgentLogInMemoryBehaviorSet(L7_uint32 agentLogInMemoryBehavior);

/***************************************************************************************/

L7_RC_t snmpAgentLogConsoleAdminStatusGet(L7_uint32 *agentLogConsoleAdminStatus);

L7_RC_t snmpAgentLogConsoleAdminStatusSet(L7_uint32 agentLogConsoleAdminStatus);

L7_RC_t snmpAgentLogConsoleSeverityFilterGet(L7_uint32 *agentLogConsoleSeverityFilter);

L7_RC_t snmpAgentLogConsoleSeverityFilterSet(L7_uint32 agentLogConsoleSeverityFilter);


/***************************************************************************************/

L7_RC_t snmpAgentLogPersistentAdminStatusGet(L7_uint32 *agentLogPersistentAdminStatus);

L7_RC_t snmpAgentLogPersistentSeverityFilterGet(L7_uint32 *agentLogPersistentSeverityFilter);

L7_RC_t snmpAgentLogPersistentAdminStatusSet(L7_uint32 agentLogPersistentAdminStatus);

L7_RC_t snmpAgentLogPersistentSeverityFilterSet(L7_uint32 agentLogPersistentSeverityFilter);

/***************************************************************************************/

L7_RC_t snmpAgentLogSyslogAdminStatusGet(L7_uint32 *agentLogSyslogAdminStatus);

L7_RC_t snmpAgentLogSyslogLocalPortGet(L7_uint32 *agentLogSyslogLocalPort);

L7_RC_t snmpAgentLogSyslogMaxHostsGet(L7_uint32 *agentLogSyslogMaxHosts);

L7_RC_t snmpAgentLogSyslogAdminStatusSet(L7_uint32 agentLogSyslogAdminStatus);

L7_RC_t snmpAgentLogSyslogLocalPortSet(L7_uint32 agentLogSyslogLocalPort);

/***************************************************************************************/

L7_RC_t snmpAgentLogSyslogHostEntryGet(L7_uint32 agentLogHostTableIndex);

L7_RC_t snmpAgentLogSyslogHostEntryNextGet(L7_uint32 *agentLogHostTableIndex);

L7_RC_t snmpAgentLogHostTableAddressTypeGet(L7_uint32 agentLogHostTableIndex, L7_uint32 *agentLogHostTableAddressType);

L7_RC_t snmpAgentLogHostTableAddressTypeSet(L7_uint32 agentLogHostTableIndex, L7_uint32 agentLogHostTableAddressType);

L7_RC_t snmpAgentLogHostTableSeverityFilterGet(L7_uint32 agentLogHostTableIndex, L7_uint32 *agentLogHostTableSeverityFilter);

L7_RC_t snmpAgentLogHostTableSeverityFilterSet(L7_uint32 agentLogHostTableIndex, L7_uint32 agentLogHostTableSeverityFilter);

L7_RC_t snmpAgentLogHostTableRowStatusGet(L7_uint32 agentLogHostTableIndex, L7_uint32 *agentLogHostTableRowStatus);

L7_RC_t snmpAgentLogHostTableRowStatusSet(L7_uint32 agentLogHostTableIndex, L7_uint32 agentLogHostTableRowStatus);

/***************************************************************************************/

L7_RC_t snmpAgentLogCliCommandsAdminStatusGet(L7_uint32 *CliCommandsAdminStatus );

L7_RC_t snmpAgentLogCliCommandsAdminStatusSet (L7_uint32 CliCommandsAdminStatus);

L7_RC_t snmpAgentLogHostIpAddressTypeGet(L7_uint32 index,
                                         L7_int32 *addressType);

L7_RC_t snmpAgentLogHostIpAddressTypeSet(L7_uint32 index,
                                         L7_int32 addressType);
/***************************************************************************************/

#endif /* __K_MIB_FASTPATHLOGGING_API_H__ */
