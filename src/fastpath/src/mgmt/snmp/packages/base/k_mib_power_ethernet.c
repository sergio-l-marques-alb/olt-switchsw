/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename k_mib_power_ethernet.c
*
* @purpose  system specific code for draft-ietf-hubmib-power-ethernet-mib-08.txt
*
* @component POE
*
* @create  11/12/2003
*
* @author  cpverne
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "poe_exports.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"
#include "usmdb_poe_api.h"
#include "k_private_base.h"
#include "k_mib_power_ethernet_api.h"

pethPsePortEntry_t *
k_pethPsePortEntry_get(int serialNum, ContextInfo *contextInfo,
                       int nominator,
                       int searchType,
                       SR_INT32 pethPsePortGroupIndex,
                       SR_INT32 pethPsePortIndex)
{
  static pethPsePortEntry_t pethPsePortEntryData;
  L7_uint32 intIfNum;
  static L7_BOOL firstTime = L7_TRUE;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];

  memset(snmp_buffer, 0x00, SNMP_BUFFER_LEN);

  /* check if supported on this platform */
  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POE_COMPONENT_ID, L7_POE_FEATURE_ID) != L7_TRUE)
  {
    return(NULL);
  }

  if (firstTime == L7_TRUE)
  {
    /* Initialize complex variables to satisfy Clone_ function */
    pethPsePortEntryData.pethPsePortType = MakeOctetString(NULL, 0);
    firstTime = L7_FALSE;
  }

  ZERO_VALID(pethPsePortEntryData.valid);

  pethPsePortEntryData.pethPsePortGroupIndex = pethPsePortGroupIndex;
  SET_VALID(I_pethPsePortGroupIndex, pethPsePortEntryData.valid);

  pethPsePortEntryData.pethPsePortIndex = pethPsePortIndex;
  SET_VALID(I_pethPsePortIndex, pethPsePortEntryData.valid);

  if ((searchType == EXACT) ?
      (snmpPethPsePortEntryGet(USMDB_UNIT_CURRENT,
                               pethPsePortEntryData.pethPsePortGroupIndex,
                               pethPsePortEntryData.pethPsePortIndex,
                               &intIfNum, nominator) != L7_SUCCESS) :
      ((snmpPethPsePortEntryGet(USMDB_UNIT_CURRENT,
                                pethPsePortEntryData.pethPsePortGroupIndex,
                                pethPsePortEntryData.pethPsePortIndex,
                                &intIfNum, nominator) != L7_SUCCESS) &&
       (snmpPethPsePortEntryNextGet(USMDB_UNIT_CURRENT,
                                    &pethPsePortEntryData.pethPsePortGroupIndex,
                                    &pethPsePortEntryData.pethPsePortIndex,
                                    &intIfNum, nominator) != L7_SUCCESS)))
  {
    return(NULL);
  }

  switch (nominator)
  {

    case -1:
    case I_pethPsePortGroupIndex:
    case I_pethPsePortIndex:
      if (nominator != -1) break;

    case I_pethPsePortAdminEnable:
      if (snmpPethPsePortAdminEnableGet(USMDB_UNIT_CURRENT, intIfNum,
                                        &pethPsePortEntryData.pethPsePortAdminEnable) == L7_SUCCESS)
        SET_VALID(I_pethPsePortAdminEnable, pethPsePortEntryData.valid);
      if (nominator != -1) break;

    case I_pethPsePortPowerPairsControlAbility:
      if (snmpPethPsePortPowerPairsControlAbilityGet(USMDB_UNIT_CURRENT, intIfNum,
                                                     &pethPsePortEntryData.pethPsePortPowerPairsControlAbility)
          == L7_SUCCESS)
        SET_VALID(I_pethPsePortPowerPairsControlAbility, pethPsePortEntryData.valid);
      if (nominator != -1) break;

    case I_pethPsePortPowerPairs:
      if (snmpPethPsePortPowerPairsGet(USMDB_UNIT_CURRENT, intIfNum,
                                       &pethPsePortEntryData.pethPsePortPowerPairs)
          == L7_SUCCESS)
        SET_VALID(I_pethPsePortPowerPairs, pethPsePortEntryData.valid);
      if (nominator != -1) break;

    case I_pethPsePortDetectionStatus:
      if (snmpPethPsePortDetectionStatusGet(USMDB_UNIT_CURRENT, intIfNum, &pethPsePortEntryData.pethPsePortDetectionStatus) == L7_SUCCESS)
        SET_VALID(I_pethPsePortDetectionStatus, pethPsePortEntryData.valid);
      if (nominator != -1) break;

    case I_pethPsePortPowerPriority:
      if (snmpPethPsePortPowerPriorityGet(USMDB_UNIT_CURRENT, intIfNum, &pethPsePortEntryData.pethPsePortPowerPriority) == L7_SUCCESS)
        SET_VALID(I_pethPsePortPowerPriority, pethPsePortEntryData.valid);
      if (nominator != -1) break;

    case I_pethPsePortMPSAbsentCounter:
      if (snmpPethPsePortMPSAbsentCounterGet(USMDB_UNIT_CURRENT, intIfNum,
                                             &pethPsePortEntryData.pethPsePortMPSAbsentCounter)
          == L7_SUCCESS)
        SET_VALID(I_pethPsePortMPSAbsentCounter, pethPsePortEntryData.valid);
      if (nominator != -1) break;

    case I_pethPsePortType:
      if (snmpPethPsePortTypeGet(USMDB_UNIT_CURRENT, intIfNum, snmp_buffer) == L7_SUCCESS &&
          SafeMakeOctetStringFromText(& pethPsePortEntryData.pethPsePortType, snmp_buffer) == L7_TRUE)
        SET_VALID(I_pethPsePortType, pethPsePortEntryData.valid);
      if (nominator != -1) break;

    case I_pethPsePortPowerClassifications:
      if (snmpPethPsePortPowerClassificationsGet(USMDB_UNIT_CURRENT, intIfNum,
                                                 &pethPsePortEntryData.pethPsePortPowerClassifications) == L7_SUCCESS)
        SET_VALID(I_pethPsePortPowerClassifications, pethPsePortEntryData.valid);
      if (nominator != -1) break;

    case I_pethPsePortInvalidSignatureCounter:
      if (snmpPethPsePortInvalidSignatureCounter(USMDB_UNIT_CURRENT, intIfNum,
                                                 &pethPsePortEntryData.pethPsePortInvalidSignatureCounter)
          == L7_SUCCESS)
        SET_VALID(I_pethPsePortInvalidSignatureCounter, pethPsePortEntryData.valid);
      if (nominator != -1) break;

    case I_pethPsePortPowerDeniedCounter:
      if (snmpPethPsePortPowerDeniedCounter(USMDB_UNIT_CURRENT, intIfNum,
                                            &pethPsePortEntryData.pethPsePortPowerDeniedCounter)
          == L7_SUCCESS)
        SET_VALID(I_pethPsePortPowerDeniedCounter, pethPsePortEntryData.valid);
      if (nominator != -1) break;

    case I_pethPsePortOverLoadCounter:
      if (snmpPethPsePortOverLoadCounter(USMDB_UNIT_CURRENT, intIfNum,
                                         &pethPsePortEntryData.pethPsePortOverLoadCounter)
          == L7_SUCCESS)
        SET_VALID(I_pethPsePortOverLoadCounter, pethPsePortEntryData.valid);
      if (nominator != -1) break;

    case I_pethPsePortShortCounter:
      if (snmpPethPsePortShortCounter(USMDB_UNIT_CURRENT, intIfNum,
                                      &pethPsePortEntryData.pethPsePortShortCounter)
          == L7_SUCCESS)
        SET_VALID(I_pethPsePortShortCounter, pethPsePortEntryData.valid);
      if (nominator != -1) break;

#ifdef I_agentPethPowerLimit
    case I_agentPethPowerLimit:
      if (usmDbPethPsePortCurPowerLimitGet(USMDB_UNIT_CURRENT, intIfNum, &pethPsePortEntryData.agentPethPowerLimit) == L7_SUCCESS)
        SET_VALID(I_agentPethPowerLimit, pethPsePortEntryData.valid);
      if (nominator != -1) break;
#endif

#ifdef I_agentPethOutputPower
    case I_agentPethOutputPower:
      if (usmDbPethPsePortPowerOutputGet(intIfNum, &pethPsePortEntryData.agentPethOutputPower) == L7_SUCCESS)
        SET_VALID(I_agentPethOutputPower, pethPsePortEntryData.valid);
      if (nominator != -1) break;
#endif
#ifdef I_agentPethOutputCurrent
    case I_agentPethOutputCurrent:
      if (usmDbPethPsePortCurrentOutputGet(USMDB_UNIT_CURRENT, intIfNum, &pethPsePortEntryData.agentPethOutputCurrent) == L7_SUCCESS)
        SET_VALID(I_agentPethOutputCurrent, pethPsePortEntryData.valid);
      if (nominator != -1) break;
#endif

#ifdef I_agentPethOutputVolts
    case I_agentPethOutputVolts:
      if (usmDbPethPsePortVoltageOutputGet(USMDB_UNIT_CURRENT, intIfNum, &pethPsePortEntryData.agentPethOutputVolts) == L7_SUCCESS)
        SET_VALID(I_agentPethOutputVolts, pethPsePortEntryData.valid);
      if (nominator != -1) break;
#endif

#ifdef I_agentPethTemperature
    case I_agentPethTemperature:
      if (usmDbPethPsePortTemperatureGet(USMDB_UNIT_CURRENT, intIfNum, &pethPsePortEntryData.agentPethTemperature) == L7_SUCCESS)
        SET_VALID(I_agentPethTemperature, pethPsePortEntryData.valid);
      if (nominator != -1) break;
#endif

#ifdef I_agentPethPowerLimitType
    case I_agentPethPowerLimitType:
      if (snmpAgentPethPowerLimitTypeGet(USMDB_UNIT_CURRENT, intIfNum, &pethPsePortEntryData.agentPethPowerLimitType) == L7_SUCCESS)
        SET_VALID(I_agentPethPowerLimitType, pethPsePortEntryData.valid);
      if (nominator != -1) break;
#endif

#ifdef I_agentPethHighPowerEnable
    case I_agentPethHighPowerEnable:
      if (snmpAgentPethHighPowerEnableGet(USMDB_UNIT_CURRENT, intIfNum, &pethPsePortEntryData.agentPethHighPowerEnable) == L7_SUCCESS)
        SET_VALID(I_agentPethHighPowerEnable, pethPsePortEntryData.valid);
      if (nominator != -1) break;
#endif

#ifdef I_agentPethPowerDetectionType
    case I_agentPethPowerDetectionType:
      if (snmpAgentPethPowerDetectionTypeGet(USMDB_UNIT_CURRENT, intIfNum, &pethPsePortEntryData.agentPethPowerDetectionType) == L7_SUCCESS)
        SET_VALID(I_agentPethPowerDetectionType, pethPsePortEntryData.valid);
      if (nominator != -1) break;
#endif

#ifdef I_agentPethFaultStatus
    case I_agentPethFaultStatus:
      if (snmpAgentPethFaultStatusGet(USMDB_UNIT_CURRENT, intIfNum, &pethPsePortEntryData.agentPethFaultStatus) == L7_SUCCESS)
        SET_VALID(I_agentPethFaultStatus, pethPsePortEntryData.valid);
      if (nominator != -1) break;
#endif

#ifdef I_agentPethPortReset
    case I_agentPethPortReset:
      pethPsePortEntryData.agentPethPortReset = D_agentPethPortReset_none;
      SET_VALID(I_agentPethPortReset, pethPsePortEntryData.valid);
      if (nominator != -1) break;
#endif

      break;                            /* Do *not* pass through to the default case. */

    default:
      /* Unknown nominator */
      return(NULL);
  }

  if (nominator > 0 && !VALID(nominator, pethPsePortEntryData.valid))
  {
    return(NULL);
  }

  return(&pethPsePortEntryData);
}

#ifdef SETS
int
k_pethPsePortEntry_test(ObjectInfo *object, ObjectSyntax *value,
                        doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_pethPsePortEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                         doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_pethPsePortEntry_set_defaults(doList_t *dp)
{
    pethPsePortEntry_t *data = (pethPsePortEntry_t *) (dp->data);

    data->pethPsePortMPSAbsentCounter = (SR_UINT32) 0;
    if ((data->pethPsePortType = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->pethPsePortInvalidSignatureCounter = (SR_UINT32) 0;
    data->pethPsePortPowerDeniedCounter = (SR_UINT32) 0;
    data->pethPsePortOverLoadCounter = (SR_UINT32) 0;
    data->pethPsePortShortCounter = (SR_UINT32) 0;

    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_pethPsePortEntry_set(pethPsePortEntry_t *data,
                       ContextInfo *contextInfo, int function)
{
  L7_uint32 intIfNum;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];


  /* check if supported on this platform */
  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POE_COMPONENT_ID, L7_POE_FEATURE_ID) != L7_TRUE)
    return(COMMIT_FAILED_ERROR);

  if (snmpPethPsePortEntryGet(USMDB_UNIT_CURRENT, data->pethPsePortGroupIndex, data->pethPsePortIndex, &intIfNum, -1) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  /* PoE admin mode */
  if (VALID(I_pethPsePortAdminEnable, data->valid) &&
      snmpPethPsePortAdminEnableSet(USMDB_UNIT_CURRENT, intIfNum, data->pethPsePortAdminEnable) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  /* power priority */
  if (VALID(I_pethPsePortPowerPriority, data->valid) &&
      snmpPethPsePortPowerPrioritySet(USMDB_UNIT_CURRENT, intIfNum, data->pethPsePortPowerPriority) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  /* Power pairs */
  if (VALID(I_pethPsePortPowerPairs, data->valid) &&
      snmpPethPsePortPowerPairsSet(USMDB_UNIT_CURRENT, intIfNum, data->pethPsePortPowerPairs) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  /* Port Type*/
  if (VALID(I_pethPsePortType, data->valid))
  {
      bzero(snmp_buffer, SNMP_BUFFER_LEN);
      memcpy(snmp_buffer, data->pethPsePortType->octet_ptr, data->pethPsePortType->length);
      if (snmpPethPsePortTypeSet(USMDB_UNIT_CURRENT, intIfNum, snmp_buffer)
                                != L7_SUCCESS) {
          return(COMMIT_FAILED_ERROR);
      }
  }

  /* Port power limit */
#ifdef I_agentPethPowerLimit
  if (VALID(I_agentPethPowerLimit, data->valid) &&
      usmDbPethPsePortCurPowerLimitSet(USMDB_UNIT_CURRENT, intIfNum, data->agentPethPowerLimit) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;
#endif /* I_agentPethOutputPower */

  /* port power limit type */
#ifdef I_agentPethPowerLimitType
  if (VALID(I_agentPethPowerLimitType, data->valid) &&
      snmpAgentPethPowerLimitTypeSet(USMDB_UNIT_CURRENT, intIfNum, data->agentPethPowerLimitType) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;
#endif

  /* port high power mode */
#ifdef I_agentPethHighPowerEnable
  if (VALID(I_agentPethHighPowerEnable, data->valid) &&
      snmpAgentPethHighPowerEnableSet(USMDB_UNIT_CURRENT, intIfNum, data->agentPethHighPowerEnable) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;
#endif

  /* port poe detection mode */
#ifdef I_agentPethPowerDetectionType
  if (VALID(I_agentPethPowerDetectionType, data->valid) &&
      snmpAgentPethPowerDetectionTypeSet(USMDB_UNIT_CURRENT, intIfNum, data->agentPethPowerDetectionType) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;
#endif

  /* poe reset */
#ifdef I_agentPethPortReset
  if (VALID(I_agentPethPortReset, data->valid) &&
      snmpAgentPethPortReset(USMDB_UNIT_CURRENT, intIfNum, data->agentPethPortReset) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;
#endif
  return NO_ERROR;
}

#ifdef SR_pethPsePortEntry_UNDO
/* add #define SR_pethPsePortEntry_UNDO in sitedefs.h to
 * include the undo routine for the pethPsePortEntry family.
 */
int
pethPsePortEntry_undo(doList_t *doHead, doList_t *doCur,
                      ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_pethPsePortEntry_UNDO */

#endif /* SETS */

pethMainPseEntry_t *
k_pethMainPseEntry_get(int serialNum, ContextInfo *contextInfo,
                       int nominator,
                       int searchType,
                       SR_INT32 pethMainPseGroupIndex)
{
  static pethMainPseEntry_t pethMainPseEntryData;

  /* check if supported on this platform */
  if (usmDbFeaturePresentCheck(pethMainPseGroupIndex, L7_POE_COMPONENT_ID, L7_POE_FEATURE_ID) != L7_TRUE)
    return(NULL);

  ZERO_VALID(pethMainPseEntryData.valid);

  pethMainPseEntryData.pethMainPseGroupIndex = pethMainPseGroupIndex;
  SET_VALID(I_pethMainPseGroupIndex, pethMainPseEntryData.valid);

  if ((searchType == EXACT) ?
      (snmpPethMainPseEntryGet(pethMainPseGroupIndex,
                               pethMainPseEntryData.pethMainPseGroupIndex) != L7_SUCCESS) :
      ((snmpPethMainPseEntryGet(pethMainPseGroupIndex,
                               pethMainPseEntryData.pethMainPseGroupIndex) != L7_SUCCESS) ||
       (snmpPethMainPseEntryNextGet(pethMainPseGroupIndex,
                                    &pethMainPseEntryData.pethMainPseGroupIndex) != L7_SUCCESS)))
    return(NULL);

  switch (nominator)
  {
  case -1:
  case I_pethMainPseGroupIndex:
    if (nominator != -1) break;

  case I_pethMainPsePower:
    if (usmDbPethPseMainPowerGet(pethMainPseEntryData.pethMainPseGroupIndex, &pethMainPseEntryData.pethMainPsePower) == L7_SUCCESS)
      SET_VALID(I_pethMainPsePower, pethMainPseEntryData.valid);
    if (nominator != -1) break;

  case I_pethMainPseOperStatus:
    if (snmpPethMainPseOperStatusGet(pethMainPseEntryData.pethMainPseGroupIndex, &pethMainPseEntryData.pethMainPseOperStatus) == L7_SUCCESS)
      SET_VALID(I_pethMainPseOperStatus, pethMainPseEntryData.valid);
    if (nominator != -1) break;

  case I_pethMainPseConsumptionPower:
    if (usmDbPethPseMainConsumptionPowerGet(pethMainPseEntryData.pethMainPseGroupIndex, &pethMainPseEntryData.pethMainPseConsumptionPower) == L7_SUCCESS)
      SET_VALID(I_pethMainPseConsumptionPower, pethMainPseEntryData.valid);
    if (nominator != -1) break;

  case I_pethMainPseUsageThreshold:
    if (usmDbPethPseMainUsageThresholdGet(pethMainPseEntryData.pethMainPseGroupIndex, &pethMainPseEntryData.pethMainPseUsageThreshold) == L7_SUCCESS)
      SET_VALID(I_pethMainPseUsageThreshold, pethMainPseEntryData.valid);
    if (nominator != -1) break;

#ifdef I_agentPethPsePowerManagementMode
  case I_agentPethPsePowerManagementMode:
    if (snmpAgentPethPsePowerManagementModeGet(pethMainPseEntryData.pethMainPseGroupIndex,
                                               &pethMainPseEntryData.agentPethPsePowerManagementMode)
                                                == L7_SUCCESS)
      SET_VALID(I_agentPethPsePowerManagementMode, pethMainPseEntryData.valid);
    if (nominator != -1) break;
#endif /* I_agentPethPsePowerManagementMode */

#ifdef I_agentPethPseAutoResetEnable
  case I_agentPethPseAutoResetEnable:
    if (snmpAgentPethPseAutoResetEnableGet(pethMainPseEntryData.pethMainPseGroupIndex,
                                           &pethMainPseEntryData.agentPethPseAutoResetEnable)
                                            == L7_SUCCESS)
      SET_VALID(I_agentPethPseAutoResetEnable, pethMainPseEntryData.valid);
    if (nominator != -1) break;
#endif /* I_agentPethPseAutoResetEnable */

#ifdef I_agentPethMainPseLegacy
  case I_agentPethMainPseLegacy:
    if (snmpPethMainPseLegacyGet(pethMainPseEntryData.pethMainPseGroupIndex,
                                 &pethMainPseEntryData.agentPethMainPseLegacy) == L7_SUCCESS)
    {
        SET_VALID(I_agentPethMainPseLegacy, pethMainPseEntryData.valid);
    }
    if (nominator != -1) break;
#endif
    break;

  default:
    /* Unknown nominator */
    return(NULL);
  }

  if (nominator > 0 && !VALID(nominator, pethMainPseEntryData.valid))
    return(NULL);

  return(&pethMainPseEntryData);
}

#ifdef SETS
int
k_pethMainPseEntry_test(ObjectInfo *object, ObjectSyntax *value,
                        doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_pethMainPseEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                         doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_pethMainPseEntry_set_defaults(doList_t *dp)
{
    pethMainPseEntry_t *data = (pethMainPseEntry_t *) (dp->data);


    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_pethMainPseEntry_set(pethMainPseEntry_t *data,
                       ContextInfo *contextInfo, int function)
{
  /* check if supported on this platform */
  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POE_COMPONENT_ID, L7_POE_FEATURE_ID) != L7_TRUE)
    return(COMMIT_FAILED_ERROR);

  if (snmpPethMainPseEntryGet(USMDB_UNIT_CURRENT,
                              data->pethMainPseGroupIndex) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if (VALID(I_pethMainPseUsageThreshold, data->valid) &&
      usmDbPethPseMainUsageThresholdSet(data->pethMainPseGroupIndex,
                                        data->pethMainPseUsageThreshold) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

#ifdef I_agentPethPsePowerManagementMode
  if (VALID(I_agentPethPsePowerManagementMode, data->valid) &&
      snmpPethPsePowerManagementModeSet(data->pethMainPseGroupIndex,
                                        data->agentPethPsePowerManagementMode) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;
#endif /* I_agentPethPsePowerManagementMode */

#ifdef I_agentPethPseAutoResetEnable
  if (VALID(I_agentPethPseAutoResetEnable, data->valid) &&
      snmpPethPseAutoResetEnableSet(data->pethMainPseGroupIndex, data->agentPethPseAutoResetEnable) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;
#endif /* I_agentPethPseAutoResetEnable */

#ifdef I_agentPethMainPseLegacy
  if (VALID(I_agentPethMainPseLegacy, data->valid) &&
      snmpPethMainPseLegacySet(data->pethMainPseGroupIndex, data->agentPethMainPseLegacy) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;
#endif

  return NO_ERROR;
}

#ifdef SR_pethMainPseEntry_UNDO
/* add #define SR_pethMainPseEntry_UNDO in sitedefs.h to
 * include the undo routine for the pethMainPseEntry family.
 */
int
pethMainPseEntry_undo(doList_t *doHead, doList_t *doCur,
                      ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_pethMainPseEntry_UNDO */

#endif /* SETS */

pethNotificationControlEntry_t *
k_pethNotificationControlEntry_get(int serialNum, ContextInfo *contextInfo,
                                   int nominator,
                                   int searchType,
                                   SR_INT32 pethNotificationControlGroupIndex)
{
  static pethNotificationControlEntry_t pethNotificationControlEntryData;

  /* check if supported on this platform */
  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POE_COMPONENT_ID, L7_POE_FEATURE_ID) != L7_TRUE)
    return(NULL);

  pethNotificationControlEntryData.pethNotificationControlGroupIndex = pethNotificationControlGroupIndex;
  SET_VALID(I_pethNotificationControlGroupIndex, pethNotificationControlEntryData.valid);

  if ((searchType == EXACT) ?
      (snmpPethNotificationControlEntryGet(USMDB_UNIT_CURRENT,
                                           pethNotificationControlEntryData.pethNotificationControlGroupIndex) != L7_SUCCESS) :
      ((snmpPethNotificationControlEntryGet(USMDB_UNIT_CURRENT,
                                            pethNotificationControlEntryData.pethNotificationControlGroupIndex) != L7_SUCCESS) ||
       (snmpPethNotificationControlEntryNextGet(USMDB_UNIT_CURRENT,
                                                &pethNotificationControlEntryData.pethNotificationControlGroupIndex) != L7_SUCCESS)))
    return(NULL);

  switch (nominator)
  {
  case -1:
  case I_pethNotificationControlGroupIndex:
    break;

  case I_pethNotificationControlEnable:
    if (snmpPethNotificationControlEnableGet(USMDB_UNIT_CURRENT,
                                             &pethNotificationControlEntryData.pethNotificationControlEnable) == L7_SUCCESS)
      SET_VALID(I_pethNotificationControlEnable, pethNotificationControlEntryData.valid);
    break;

  default:
    /* Unknown nominator */
    return(NULL);
  }

  if (nominator > 0 && !VALID(nominator, pethNotificationControlEntryData.valid))
    return(NULL);

  return(&pethNotificationControlEntryData);
}

#ifdef SETS
int
k_pethNotificationControlEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                    doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_pethNotificationControlEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                     doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_pethNotificationControlEntry_set_defaults(doList_t *dp)
{
    pethNotificationControlEntry_t *data = (pethNotificationControlEntry_t *) (dp->data);


    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_pethNotificationControlEntry_set(pethNotificationControlEntry_t *data,
                                   ContextInfo *contextInfo, int function)
{
  /* check if supported on this platform */
  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POE_COMPONENT_ID, L7_POE_FEATURE_ID) != L7_TRUE)
    return(COMMIT_FAILED_ERROR);

  if (snmpPethNotificationControlEntryGet(USMDB_UNIT_CURRENT,
                                          data->pethNotificationControlGroupIndex) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if (VALID(I_pethNotificationControlEnable, data->valid) &&
      snmpPethNotificationControlEnableSet(USMDB_UNIT_CURRENT, data->pethNotificationControlEnable) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  return NO_ERROR;
}

#ifdef SR_pethNotificationControlEntry_UNDO
/* add #define SR_pethNotificationControlEntry_UNDO in sitedefs.h to
 * include the undo routine for the pethNotificationControlEntry family.
 */
int
pethNotificationControlEntry_undo(doList_t *doHead, doList_t *doCur,
                                  ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_pethNotificationControlEntry_UNDO */

#endif /* SETS */

