/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename k_mib_power_ethernet_api.c
*
* @purpose SNMP specific value conversion for POE MIB
*
* @component POE
*
* @comments
*
* @create 11/12/2003
*
* @author cpverne
*
* @end
*
**********************************************************************/

#ifndef _K_MIB_POWER_ETHERNET_API_H
#define _K_MIB_POWER_ETHERNET_API_H

#include "l7_common.h"

L7_RC_t
snmpPethPsePortEntryGet(L7_uint32 UnitIndex, L7_uint32 pethPsePortGroupIndex, L7_uint32 pethPsePortIndex, L7_uint32 *intIfNum, L7_uint32 nominator);

L7_RC_t
snmpPethPsePortEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *pethPsePortGroupIndex, L7_uint32 *pethPsePortIndex, L7_uint32 *intIfNum, L7_uint32 nominator);

L7_RC_t
snmpPethPsePortAdminEnableGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);

L7_RC_t
snmpPethPsePortAdminEnableSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val);

L7_RC_t
snmpPethPsePortDetectionStatusGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);

L7_RC_t
snmpPethPsePortPowerPriorityGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);

L7_RC_t
snmpPethPsePortPowerPrioritySet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val);

L7_RC_t
snmpPethPsePortPowerClassificationsGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);

L7_RC_t
snmpPethPsePortPowerPairsControlAbilityGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);

L7_RC_t
snmpPethPsePortPowerPairsGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);

L7_RC_t
snmpPethPsePortPowerPairsSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val);

L7_RC_t
snmpPethPsePortMPSAbsentCounterGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);

L7_RC_t
snmpPethPsePortInvalidSignatureCounter(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);

L7_RC_t
snmpPethPsePortPowerDeniedCounter(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);

L7_RC_t
snmpPethPsePortOverLoadCounter(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);

L7_RC_t
snmpPethPsePortShortCounter(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);

L7_RC_t
snmpPethPsePortTypeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uchar8 *string);

L7_RC_t
snmpPethPsePortTypeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uchar8 *string);

L7_RC_t
snmpAgentPethPowerLimitTypeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);

L7_RC_t
snmpAgentPethPowerLimitTypeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val);

L7_RC_t
snmpAgentPethHighPowerEnableGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);

L7_RC_t
snmpAgentPethHighPowerEnableSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val);

L7_RC_t
snmpAgentPethPowerDetectionTypeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);

L7_RC_t
snmpAgentPethPowerDetectionTypeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val);

L7_RC_t
snmpAgentPethFaultStatusGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);

L7_RC_t
snmpAgentPethPortReset(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val);
/**********************************************************************/

L7_RC_t
snmpPethMainPseEntryGet(L7_uint32 UnitIndex, L7_uint32 pethMainPseGroupIndex);

L7_RC_t
snmpPethMainPseEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *pethMainPseGroupIndex);

L7_RC_t
snmpPethMainPseOperStatusGet(L7_uint32 UnitIndex, L7_uint32 *val);

L7_RC_t
snmpAgentPethPsePowerManagementModeGet(L7_uint32 UnitIndex, L7_uint32 *val);

L7_RC_t
snmpPethPsePowerManagementModeSet(L7_uint32 UnitIndex, L7_uint32 val);

L7_RC_t
snmpAgentPethPseAutoResetEnableGet(L7_uint32 UnitIndex, L7_uint32 *val);

L7_RC_t
snmpPethPseAutoResetEnableSet(L7_uint32 UnitIndex, L7_uint32 val);

L7_RC_t
snmpPethMainPseLegacyGet(L7_uint32 UnitIndex, L7_uint32 *val);

L7_RC_t
snmpPethMainPseLegacySet(L7_uint32 UnitIndex, L7_uint32 val);


/**********************************************************************/

L7_RC_t
snmpPethNotificationControlEntryGet(L7_uint32 UnitIndex, L7_uint32 pethNotificationControlGroupIndex);

L7_RC_t
snmpPethNotificationControlEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *pethNotificationControlGroupIndex);

L7_RC_t
snmpPethNotificationControlEnableGet(L7_uint32 UnitIndex, L7_uint32 *val);

L7_RC_t
snmpPethNotificationControlEnableSet(L7_uint32 UnitIndex, L7_uint32 val);

#endif /* _K_MIB_POWER_ETHERNET_H */
