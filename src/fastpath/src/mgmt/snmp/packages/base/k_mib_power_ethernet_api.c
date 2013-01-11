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

#include "l7_common.h"
#include "poe_exports.h"
#include "usmdb_trapmgr_api.h"
#include "usmdb_util_api.h"
#include "usmdb_poe_api.h"
#include "k_private_base.h"
#include "k_mib_power_ethernet_api.h"

L7_RC_t
snmpPethPsePortEntryGet(L7_uint32 UnitIndex, L7_uint32 pethPsePortGroupIndex, L7_uint32 pethPsePortIndex, L7_uint32 *intIfNum, L7_uint32 nominator)
{
  L7_uint32 temp_val = 0;

  if (pethPsePortGroupIndex == 1 &&
      usmDbIntIfNumFromExtIfNum(pethPsePortIndex, intIfNum) == L7_SUCCESS &&
      usmDbPoeIsValidIntf(*intIfNum) == L7_SUCCESS)
  {
    /*
     * Power Classification is only valid when delivering power.
     */
    if ((nominator == I_pethPsePortPowerClassifications) &&
        ((L7_SUCCESS != usmDbPethPsePortDetectionStatusGet(UnitIndex, *intIfNum, &temp_val)) ||
         (L7_POE_PORT_DETECTION_STATUS_DELIVERING_POWER != temp_val)))
    {
      return L7_FAILURE;
    }
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

L7_RC_t
snmpPethPsePortEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *pethPsePortGroupIndex, L7_uint32 *pethPsePortIndex, L7_uint32 *intIfNum, L7_uint32 nominator)
{
  /* only GroupIndex of 1 is supported */
  if (*pethPsePortGroupIndex > 1)
    return L7_FAILURE;

  /* if GroupIndex is less than 1, setup for next valid index,interface */
  if (*pethPsePortGroupIndex < 1)
  {
    *pethPsePortGroupIndex = 1;
    *pethPsePortIndex = 0;
  }

  do
  {
    if (snmpPethPsePortEntryGet(UnitIndex, *pethPsePortGroupIndex, *pethPsePortIndex, intIfNum, nominator) == L7_SUCCESS)
    {
      return L7_SUCCESS;
    }
  }
  while (usmDbNextPhysicalExtIfNumberGet(UnitIndex, *pethPsePortIndex, pethPsePortIndex) == L7_SUCCESS);

  /* no more valid interfaces found */
  return L7_FAILURE;
}


L7_RC_t
snmpPethPsePortAdminEnableGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val = 0;

  rc = usmDbPethPsePortAdminEnableGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_pethPsePortAdminEnable_true;
      break;
    case L7_DISABLE:
      *val = D_pethPsePortAdminEnable_false;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpPethPsePortAdminEnableSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  switch (val)
  {
  case D_pethPsePortAdminEnable_true:
    temp_val = L7_ENABLE;
    break;
  case D_pethPsePortAdminEnable_false:
    temp_val = L7_DISABLE;
    break;
  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbPethPsePortAdminEnableSet(UnitIndex, intIfNum, temp_val);
  }

  return rc;
}

L7_RC_t
snmpPethPsePortDetectionStatusGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val = 0;

  rc = usmDbPethPsePortDetectionStatusGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_POE_PORT_DETECTION_STATUS_DISABLED: /* POE_DISABLED */
      *val = D_pethPsePortDetectionStatus_disabled;
      break;
    case L7_POE_PORT_DETECTION_STATUS_SEARCHING: /* POE_SEARCHING */
      *val = D_pethPsePortDetectionStatus_searching;
      break;
    case L7_POE_PORT_DETECTION_STATUS_DELIVERING_POWER: /* POE_DELIVERING_POWER */
      *val = D_pethPsePortDetectionStatus_deliveringPower;
      break;
    case L7_POE_PORT_DETECTION_STATUS_REQUESTING_POWER: /* REQUESTING_POWER*/
    case L7_POE_PORT_DETECTION_STATUS_FAULT: /* POE_FAULT */
      *val = D_pethPsePortDetectionStatus_fault;
      break;
    case L7_POE_PORT_DETECTION_STATUS_TEST: /* POE_TEST */
      *val = D_pethPsePortDetectionStatus_test;
      break;
    case L7_POE_PORT_DETECTION_STATUS_OTHER_FAULT: /* POE_OTHER_FAULT */
      *val = D_pethPsePortDetectionStatus_otherFault;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpPethPsePortPowerPriorityGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val = 0;

  rc = usmDbPethPsePortPowerPriorityGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_POE_PRIORITY_CRITICAL: /* POE_CRITICAL */
      *val = D_pethPsePortPowerPriority_critical;
      break;
    case L7_POE_PRIORITY_HIGH: /* POE_HIGH */
      *val = D_pethPsePortPowerPriority_high;
      break;
    case L7_POE_PRIORITY_LOW: /* POE_LOW */
      *val = D_pethPsePortPowerPriority_low;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpPethPsePortPowerPrioritySet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  switch (val)
  {
  case D_pethPsePortPowerPriority_critical:
    temp_val = L7_POE_PRIORITY_CRITICAL; /* POE_CRITICAL */
    break;
  case D_pethPsePortPowerPriority_high:
    temp_val = L7_POE_PRIORITY_HIGH; /* POE_HIGH */
    break;
  case D_pethPsePortPowerPriority_low:
    temp_val = L7_POE_PRIORITY_LOW; /* POE_LOW */
    break;
  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbPethPsePortPowerPrioritySet(UnitIndex, intIfNum, temp_val);
  }

  return rc;
}

L7_RC_t
snmpPethPsePortPowerClassificationsGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val = 0;

  rc = usmDbPethPsePortPowerClassificationsGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
      case L7_POE_PORT_POWER_CLASS0: /* POE_CLASS0 */
        *val = D_pethPsePortPowerClassifications_class0;
        break;
      case L7_POE_PORT_POWER_CLASS1: /* POE_CLASS1 */
        *val = D_pethPsePortPowerClassifications_class1;
        break;
      case L7_POE_PORT_POWER_CLASS2: /* POE_CLASS2 */
        *val = D_pethPsePortPowerClassifications_class2;
        break;
      case L7_POE_PORT_POWER_CLASS3: /* POE_CLASS3 */
        *val = D_pethPsePortPowerClassifications_class3;
        break;
      case L7_POE_PORT_POWER_CLASS4: /* POE_CLASS4 */
        *val = D_pethPsePortPowerClassifications_class4;
        break;
      default:
        /* unknown value */
        rc = L7_FAILURE;
        break;
    }
  }

  return rc;
}

L7_RC_t
snmpPethPsePortPowerPairsControlAbilityGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val = 0;

  rc = usmDbPethPsePortPowerPairsControlAbilityGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_pethPsePortPowerPairsControlAbility_true;
      break;
    case L7_DISABLE:
      *val = D_pethPsePortPowerPairsControlAbility_false;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }
  return rc;
}

L7_RC_t
snmpPethPsePortPowerPairsGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val = 0;

  rc = usmDbPethPsePortPowerPairsGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_POE_PORT_POWER_PAIRS_SIGNAL:
      *val = D_pethPsePortPowerPairs_signal;
      break;
    case L7_POE_PORT_POWER_PAIRS_SPARE:
      *val = D_pethPsePortPowerPairs_spare;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }
  return rc;
}

L7_RC_t
snmpPethPsePortPowerPairsSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val)
{
  switch (val)
  {
  case  D_pethPsePortPowerPairs_signal:
    val = L7_POE_PORT_POWER_PAIRS_SIGNAL;
    break;
  case D_pethPsePortPowerPairs_spare:
    val = L7_POE_PORT_POWER_PAIRS_SPARE;
    break;
  default:
    /* unknown value */
    return L7_FAILURE;
    break;
  }
  return usmDbPethPsePortPowerPairsSet(UnitIndex, intIfNum, val);
}

L7_RC_t
snmpPethPsePortMPSAbsentCounterGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val)
{
  return usmDbPethPsePortMPSAbsentCounterGet(UnitIndex, intIfNum, val);
}

L7_RC_t
snmpPethPsePortInvalidSignatureCounter(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val)
{
  return usmDbPethPsePortInvalidSignatureCounterGet(UnitIndex, intIfNum, val);
}

L7_RC_t
snmpPethPsePortPowerDeniedCounter(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val)
{
  return usmDbPethPsePortPowerDeniedCounterGet(UnitIndex, intIfNum, val);
}

L7_RC_t
snmpPethPsePortOverLoadCounter(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val)
{
  return usmDbPethPsePortOverLoadCounterGet(UnitIndex, intIfNum, val);
}

L7_RC_t
snmpPethPsePortShortCounter(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val)
{
  return usmDbPethPsePortShortCounterGet(UnitIndex, intIfNum, val);
}

L7_RC_t
snmpPethPsePortTypeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uchar8 *string)
{
  return usmDbPethPsePortTypeStringGet(UnitIndex, intIfNum, string);
}

L7_RC_t
snmpPethPsePortTypeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uchar8 *string)
{
  return usmDbPethPsePortTypeStringSet(UnitIndex, intIfNum, string);
}

L7_RC_t
snmpAgentPethPowerLimitTypeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val = 0;

  rc = usmDbPethPsePortViolationTypeGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_POE_LIMIT_TYPE_DOT3AF:
      *val = D_agentPethPowerLimitType_dot3af;
      break;
    case L7_POE_LIMIT_TYPE_USER:
      *val = D_agentPethPowerLimitType_user;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }
  return rc;
}

L7_RC_t
snmpAgentPethPowerLimitTypeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val)
{
  switch (val)
  {
  case D_agentPethPowerLimitType_dot3af:
    val = L7_POE_LIMIT_TYPE_DOT3AF;
    break;
  case  D_agentPethPowerLimitType_user:
    val = L7_POE_LIMIT_TYPE_USER;
    break;
  default:
    /* unknown value */
    return L7_FAILURE;
    break;
  }
  return usmDbPethPsePortViolationTypeSet(UnitIndex, intIfNum, val);
}

L7_RC_t
snmpAgentPethHighPowerEnableGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val = 0;

  rc = usmDbPethPsePortHighPowerModeGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentPethHighPowerEnable_true;
      break;
    case L7_DISABLE:
      *val = D_agentPethHighPowerEnable_false;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }
  return rc;
}

L7_RC_t
snmpAgentPethHighPowerEnableSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val)
{
    switch (val)
    {
    case D_agentPethHighPowerEnable_true:
      val = L7_ENABLE;
      break;
    case  D_agentPethHighPowerEnable_false:
      val = L7_DISABLE;
      break;
    default:
      /* unknown value */
      return L7_FAILURE;
      break;
    }
    return usmDbPethPsePortHighPowerModeSet(UnitIndex, intIfNum, val);
}

L7_RC_t
snmpAgentPethPowerDetectionTypeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val = 0;

  rc = usmDbPethPsePortDetectionModeGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_POE_DETECTION_INVALID:
      *val = D_agentPethPowerDetectionType_none;
      break;
    case L7_POE_DETECTION_LEGACY:
      *val = D_agentPethPowerDetectionType_legacy;
      break;
    case L7_POE_DETECTION_4PT_DOT3AF:
      *val = D_agentPethPowerDetectionType_fourPtdot3afonly;
      break;
    case L7_POE_DETECTION_4PT_DOT3AF_LEG:
      *val = D_agentPethPowerDetectionType_fourPtdot3afandlegacy;
      break;
    case L7_POE_DETECTION_2PT_DOT3AF:
      *val = D_agentPethPowerDetectionType_twoPtdot3afonly;
      break;
    case L7_POE_DETECTION_2PT_DOT3AF_LEG:
      *val = D_agentPethPowerDetectionType_twoPtdot3afandlegacy;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }
  return rc;
}

L7_RC_t
snmpAgentPethPowerDetectionTypeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val)
{
    switch (val)
    {
    case D_agentPethPowerDetectionType_none:
      val = L7_POE_DETECTION_INVALID;
      break;
    case D_agentPethPowerDetectionType_legacy:
      val = L7_POE_DETECTION_LEGACY;
      break;
    case  D_agentPethPowerDetectionType_fourPtdot3afonly:
      val = L7_POE_DETECTION_4PT_DOT3AF;
      break;
    case  D_agentPethPowerDetectionType_fourPtdot3afandlegacy:
      val = L7_POE_DETECTION_4PT_DOT3AF_LEG;
      break;
    case D_agentPethPowerDetectionType_twoPtdot3afonly:
      val = L7_POE_DETECTION_2PT_DOT3AF;
      break;
    case  D_agentPethPowerDetectionType_twoPtdot3afandlegacy:
      val = L7_POE_DETECTION_2PT_DOT3AF_LEG;
      break;
    default:
      /* unknown value */
      return L7_FAILURE;
      break;
    }
    return usmDbPethPsePortDetectionModeSet(UnitIndex, intIfNum, val);
}

L7_RC_t
snmpAgentPethFaultStatusGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val = 0;

  rc = usmDbPethPsePortErrorCodeGet(intIfNum, &temp_val);
  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_POE_ERROR_NONE:
      *val = D_agentPethFaultStatus_none;
      break;
    case L7_POE_ERROR_MPS_ABSENT:
      *val = D_agentPethFaultStatus_mpsAbsent;
      break;
    case L7_POE_ERROR_SHORT:
      *val = D_agentPethFaultStatus_short;
      break;
    case L7_POE_ERROR_OVERLOAD:
      *val = D_agentPethFaultStatus_overload;
      break;
    case L7_POE_ERROR_POWER_DENIED:
      *val = D_agentPethFaultStatus_powerDenied;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }
  return rc;
}

L7_RC_t
snmpAgentPethPortReset(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val)
{
   if (val == D_agentPethPortReset_reset)
   {
     return usmDbPethPsePortReset(UnitIndex, intIfNum);
   }
   return L7_FAILURE;
}

/**********************************************************************/

L7_RC_t
snmpPethMainPseEntryGet(L7_uint32 UnitIndex, L7_uint32 pethMainPseGroupIndex)
{
  if (pethMainPseGroupIndex == 1)
    return L7_SUCCESS;

  return L7_FAILURE;
}

L7_RC_t
snmpPethMainPseEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *pethMainPseGroupIndex)
{
  /* only GroupIndex of 1 is supported */
  if (*pethMainPseGroupIndex > 1)
    return L7_FAILURE;

  /* if GroupIndex is less than 1, setup for next valid index */
  if (*pethMainPseGroupIndex < 1)
  {
    *pethMainPseGroupIndex = 1;
  }

  return L7_SUCCESS;
}

L7_RC_t
snmpPethMainPseOperStatusGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val = 0;

  rc = usmDbPethPseMainOperStatusGet(UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_POE_SYSTEM_OPER_STATUS_ON: /* POE_SYSTEM_ON */
      *val = D_pethMainPseOperStatus_on;
      break;
    case L7_POE_SYSTEM_OPER_STATUS_OFF: /* POE_SYSTEM_OFF */
      *val = D_pethMainPseOperStatus_off;
      break;
    case L7_POE_SYSTEM_OPER_STATUS_FAULTY: /* POE_SYSTEM_FAULTY */
      *val = D_pethMainPseOperStatus_faulty;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

#ifdef I_agentPethPsePowerManagementMode
L7_RC_t
snmpAgentPethPsePowerManagementModeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val = 0;

  rc = usmDbPethPseMainPowerMgmtModeGet(UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_POE_POWER_MGMT_INVALID:
      *val = D_agentPethPsePowerManagementMode_none;
      break;
    case L7_POE_POWER_MGMT_STATIC:
      *val = D_agentPethPsePowerManagementMode_dynamic;
      break;
    case L7_POE_POWER_MGMT_DYNAMIC:
      *val = D_agentPethPsePowerManagementMode_static;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }
  return rc;
}

L7_RC_t
snmpPethPsePowerManagementModeSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  switch (val)
  {
    case D_agentPethPsePowerManagementMode_none:
      val = L7_POE_POWER_MGMT_INVALID;
      break;
    case D_agentPethPsePowerManagementMode_dynamic:
      val = L7_POE_POWER_MGMT_STATIC;
      break;
    case D_agentPethPsePowerManagementMode_static:
      val = L7_POE_POWER_MGMT_DYNAMIC;
      break;
    default:
      /* unknown value */
      return L7_FAILURE;
      break;
    }
  return usmDbPethPseMainPowerMgmtModeSet(UnitIndex, val);
}
#endif /* I_agentPethPsePowerManagementMode */

#ifdef I_agentPethPseAutoResetEnable
L7_RC_t
snmpAgentPethPseAutoResetEnableGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val = 0;

  rc = usmDbPethPseMainResetAutoGet(UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentPethPseAutoResetEnable_true;
      break;
    case L7_DISABLE:
      *val = D_agentPethPseAutoResetEnable_false;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }
  return rc;
}

L7_RC_t
snmpPethPseAutoResetEnableSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  switch (val)
  {
    case D_agentPethPseAutoResetEnable_true:
      val = L7_ENABLE;
      break;
    case D_agentPethPseAutoResetEnable_false:
      val = L7_DISABLE;
      break;
    default:
      /* unknown value */
      return L7_FAILURE;
      break;
    }
  return usmDbPethPseMainResetAutoSet(UnitIndex, val);
}
#endif /* I_agentPethPseAutoResetEnable */

L7_RC_t
snmpPethMainPseLegacyGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val = 0;

  rc = usmDbPethPseMainDetectionModeGet(UnitIndex, &temp_val);
  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_POE_DETECTION_4PT_DOT3AF_LEG:
      *val = D_agentPethMainPseLegacy_true;
      break;
    default:
      *val = D_agentPethMainPseLegacy_false;
      break;
    }
  }
  return rc;
}

L7_RC_t
snmpPethMainPseLegacySet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  switch (val)
  {
  case D_agentPethMainPseLegacy_true:
    temp_val = L7_POE_DETECTION_4PT_DOT3AF_LEG;
    break;
  case D_agentPethMainPseLegacy_false:
    temp_val = FD_POE_DETECTION;
    break;
  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbPethPseMainDetectionModeSet(L7_ALL_UNITS, temp_val);
  }

  return rc;

}

/**********************************************************************/

L7_RC_t
snmpPethNotificationControlEntryGet(L7_uint32 UnitIndex, L7_uint32 pethNotificationControlGroupIndex)
{
  if (pethNotificationControlGroupIndex == 1)
    return L7_SUCCESS;

  return L7_FAILURE;
}

L7_RC_t
snmpPethNotificationControlEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *pethNotificationControlGroupIndex)
{
  /* only GroupIndex of 1 is supported */
  if (*pethNotificationControlGroupIndex > 1)
    return L7_FAILURE;

  /* if GroupIndex is less than 1, setup for next valid index */
  if (*pethNotificationControlGroupIndex < 1)
  {
    *pethNotificationControlGroupIndex = 1;
  }

  return L7_SUCCESS;
}

L7_RC_t
snmpPethNotificationControlEnableGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val = 0;

  rc = usmDbTrapPoeGet(UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_pethNotificationControlEnable_true;
      break;
    case L7_DISABLE:
      *val = D_pethNotificationControlEnable_false;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpPethNotificationControlEnableSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  switch (val)
  {
  case D_pethNotificationControlEnable_true:
    temp_val = L7_ENABLE;
    break;
  case D_pethNotificationControlEnable_false:
    temp_val = L7_DISABLE;
    break;
  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbTrapPoeSet(UnitIndex, temp_val);
  }

  return rc;
}
