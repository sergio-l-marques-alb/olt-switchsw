/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   k_mib_fastpathinventory_api.h
*
* @purpose    Wrapper functions for Fastpath Inventory MIB
*
* @component  SNMP
*
* @comments
*
* @create     6/24/2003
*
* @author     cpverne
* @end
*
**********************************************************************/

#ifndef K_MIB_FASTPATHINVENTORY_API_H
#define K_MIB_FASTPATHINVENTORY_API_H

/* Begin Function Prototypes */

/****************************************************************************************/

L7_RC_t snmpAgentInventoryStackReplicateSTKSet(L7_uint32 val);

L7_RC_t snmpAgentInventoryStackReloadSet(L7_uint32 val);

L7_RC_t snmpAgentInventoryStackSTKnameGet(L7_uint32 *val);

L7_RC_t snmpAgentInventoryStackSTKnameSet(L7_uint32 val);

L7_RC_t snmpAgentInventoryStackReplicateSTKStatusGet(L7_uint32 *val);

L7_RC_t snmpAgentInventoryStackActivateSTKSet(L7_uint32 val);

L7_RC_t snmpAgentInventoryStackDeleteSTKSet(L7_uint32 val);

/****************************************************************************************/

L7_RC_t snmpAgentInventorySupportedUnitExpectedCodeVerGet(L7_uint32 agentInventoryUnitNumber, L7_char8 *buf);

/****************************************************************************************/
L7_RC_t snmpAgentInventoryUnitEntryGet(L7_uint32 agentInventoryUnitNumber);

L7_RC_t snmpAgentInventoryUnitEntryNextGet(L7_uint32 *agentInventoryUnitNumber);

L7_RC_t snmpAgentInventoryUnitSupportedUnitIndexGet(L7_uint32 agentInventoryUnitNumber, L7_uint32 *val);

L7_RC_t snmpAgentInventoryUnitMgmtAdminGet(L7_uint32 agentInventoryUnitNumber, L7_uint32 *val);

L7_RC_t snmpAgentInventoryUnitHWMgmtPrefGet(L7_uint32 agentInventoryUnitNumber, L7_uint32 *val);

L7_RC_t snmpAgentInventoryUnitHWMgmtPrefValueGet(L7_uint32 agentInventoryUnitNumber, L7_uint32 *val);

L7_RC_t snmpAgentInventoryUnitAdminMgmtPrefGet(L7_uint32 agentInventoryUnitNumber, L7_uint32 *val);

L7_RC_t snmpAgentInventoryUnitAdminMgmtPrefSet(L7_uint32 agentInventoryUnitNumber, L7_uint32 val);

L7_RC_t snmpAgentInventoryUnitAdminMgmtPrefValueGet(L7_uint32 agentInventoryUnitNumber, L7_uint32 *val);

L7_RC_t snmpAgentInventoryUnitAdminMgmtPrefValueSet(L7_uint32 agentInventoryUnitNumber, L7_uint32 val);

L7_RC_t snmpAgentInventoryUnitMgmtAdminSet(L7_uint32 agentInventoryUnitNumber, L7_uint32 val);

L7_RC_t snmpAgentInventoryUnitStatusGet(L7_uint32 agentInventoryUnitNumber, L7_uint32 *val);

L7_RC_t snmpAgentInventoryUnitDetectedCodeVerGet(L7_uint32 agentInventoryUnitNumber, L7_char8 *buf);

L7_RC_t snmpAgentInventoryUnitDetectedCodeInFlashVerGet(L7_uint32 agentInventoryUnitNumber, L7_char8 *buf);

L7_RC_t snmpAgentInventoryUnitUpTimeGet(L7_uint32 agentInventoryUnitNumber, L7_uint32 *val);

L7_RC_t snmpAgentInventoryUnitReplicateSTKSet(L7_uint32 agentInventoryUnitNumber, L7_uint32 val);

L7_RC_t snmpAgentInventoryUnitReplicateSTKStatusGet(L7_uint32 agentInventoryUnitNumber, L7_uint32 *val);

L7_RC_t snmpAgentInventoryUnitReloadSet(L7_uint32 agentInventoryUnitNumber, L7_uint32 val);

L7_RC_t snmpAgentInventoryUnitRowStatusSet(L7_uint32 agentInventoryUnitNumber, L7_uint32 val);

L7_RC_t snmpAgentInventoryUnitStandbyGet(L7_uint32 agentInventoryUnitNumber, L7_uint32 *val);

L7_RC_t snmpAgentInventoryUnitStandbySet(L7_uint32 agentInventoryUnitNumber, L7_uint32 val);

/****************************************************************************************/

L7_RC_t snmpAgentInventorySlotEntryGet(L7_uint32 agentInventoryUnitNumber, L7_uint32 agentInventorySlotNumber);

L7_RC_t snmpAgentInventorySlotEntryNextGet(L7_uint32 *agentInventoryUnitNumber, L7_uint32 *agentInventorySlotNumber);

L7_RC_t snmpAgentInventorySlotStatusGet(L7_uint32 agentInventoryUnitNumber, L7_uint32 agentInventorySlotNumber, L7_uint32 *val);

L7_RC_t snmpAgentInventorySlotPowerModeGet(L7_uint32 agentInventoryUnitNumber, L7_uint32 agentInventorySlotNumber, L7_uint32 *val);

L7_RC_t snmpAgentInventorySlotAdminModeGet(L7_uint32 agentInventoryUnitNumber, L7_uint32 agentInventorySlotNumber, L7_uint32 *val);

L7_RC_t snmpAgentInventorySlotCapabilitiesGet(L7_uint32 agentInventoryUnitNumber, L7_uint32 agentInventorySlotNumber, L7_char8 *buf, L7_uint32 *buf_len);

/****************************************************************************************/

L7_RC_t snmpAgentInventoryStackPortIpTelephonyQOSSupportGet(L7_uint32 *val);

L7_RC_t snmpAgentInventoryStackPortIpTelephonyQOSSupportSet(L7_uint32 val);

/****************************************************************************************/

L7_RC_t snmpAgentInventoryStackPortEntryGet(L7_uint32 agentInventoryStackPortIndex);

L7_RC_t snmpAgentInventoryStackPortEntryNextGet(L7_uint32 *agentInventoryStackPortIndex);

L7_RC_t snmpAgentInventoryStackPortUnitGet(L7_uint32 agentInventoryStackPortIndex, L7_uint32 *val);

L7_RC_t snmpAgentInventoryStackPortTagGet(L7_uint32 agentInventoryStackPortIndex, L7_char8 *buffer);

L7_RC_t snmpAgentInventoryStackPortConfiguredStackModeGet(L7_uint32 agentInventoryStackPortIndex, L7_uint32 *val);

L7_RC_t snmpAgentInventoryStackPortConfiguredStackModeSet(L7_uint32 agentInventoryStackPortIndex, L7_uint32 val);

L7_RC_t snmpAgentInventoryStackPortRunningStackModeGet(L7_uint32 agentInventoryStackPortIndex, L7_uint32 *val);

L7_RC_t snmpAgentInventoryStackPortLinkStatusGet(L7_uint32 agentInventoryStackPortIndex, L7_uint32 *val);

L7_RC_t snmpAgentInventoryStackPortLinkSpeedGet(L7_uint32 agentInventoryStackPortIndex, L7_uint32 *val);

L7_RC_t snmpAgentInventoryStackPortDataRateGet(L7_uint32 agentInventoryStackPortIndex, L7_uint32 *val);

L7_RC_t snmpAgentInventoryStackPortErrorRateGet(L7_uint32 agentInventoryStackPortIndex, L7_uint32 *val);

L7_RC_t snmpAgentInventoryStackPortTotalErrorsGet(L7_uint32 agentInventoryStackPortIndex, L7_uint32 *val);

L7_RC_t snmpAgentInventoryStackFirmwareSyncModeSet(L7_uint32 val);
L7_RC_t snmpAgentInventoryStackFirmwareSyncAllowDowngradeModeSet(L7_uint32 val);
L7_RC_t snmpAgentInventoryStackFirmwareSyncTrapModeSet(L7_uint32 val);
L7_RC_t snmpAgentInventoryUnitSFSLastAttemptStatusGet(L7_uint32 unit,L7_uint32* status);
L7_RC_t snmpAgentInventoryUnitSFSTransferStatusGet(L7_uint32 unit,L7_uint32* status);

/* End Function Prototypes */
#endif /* K_MIB_FASTPATHINVENTORY_API_H */
