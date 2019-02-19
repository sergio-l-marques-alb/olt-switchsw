/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   k_mib_fastpathinventory_api.c
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

#include "snmpapi.h"
#include "k_private_base.h"
#include "k_mib_fastpathinventory_api.h"
#include "usmdb_cda_api.h"
#include "transfer_exports.h"
#include "usmdb_cardmgr_api.h"
#include "usmdb_common.h"
#include "usmdb_sim_api.h"
#include "usmdb_spm_api.h"
#include "usmdb_unitmgr_api.h"
#include "usmdb_dim_api.h"
#include "usmdb_file_api.h"
#include <ctype.h>

/* Begin Function Definitions: k_mib_fastpathinventory_api.h */

/****************************************************************************************/

L7_RC_t snmpAgentInventoryStackReplicateSTKSet(L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 snmp_buffer[128];

  switch (val)
  {
  case D_agentInventoryStackReplicateSTK_disable:
    /* no-op */
    break;

  case D_agentInventoryStackReplicateSTK_enable:
    rc = usmDbTransferFileNameLocalGet(USMDB_UNIT_CURRENT, snmp_buffer);
    rc = usmdbCdaCodeUpdateNoBlock(L7_USMDB_CDA_CDA_ALL_UNITS, snmp_buffer, snmp_buffer);
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  return rc;
}

L7_RC_t snmpAgentInventoryStackReloadSet(L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentInventoryStackReload_enable:
    rc = usmDbUnitMgrResetSystem();
    break;

  case D_agentInventoryStackReload_disable:
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  return rc;
}

L7_RC_t snmpAgentInventoryStackReplicateSTKStatusGet(L7_uint32 *val)
{
  L7_RC_t rc = L7_SUCCESS;
  usmDbCdaCodeLoadStatus_t temp_val;

  rc = usmdbCdaUpdateStatusGet(START_DOWNLOAD, &temp_val);

  switch (temp_val)
  {
  case L7_USMDB_CDA_IN_PROGRESS:
    *val = D_agentInventoryStackReplicateSTKStatus_inProgress;
    break;
  case L7_USMDB_CDA_NOT_IN_PROGRESS:
    *val = D_agentInventoryStackReplicateSTKStatus_notInProgress;
    break;
  case L7_USMDB_CDA_FINISHED_WITH_SUCCESS:
    *val = D_agentInventoryStackReplicateSTKStatus_finishedWithSuccess;
    break;
  case L7_USMDB_CDA_FINISHED_WITH_ERROR:
    *val = D_agentInventoryStackReplicateSTKStatus_finishedWithError;
    break;
  default:
    rc = L7_FAILURE;
    *val = 0;
    break;
  }

  return rc;
}

L7_RC_t snmpAgentInventoryStackSTKnameGet(L7_uint32 *val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uchar8 localName[L7_MAX_FILENAME+1];

  memset(localName, 0, sizeof(localName));
  rc = usmDbTransferFileNameLocalGet(USMDB_UNIT_CURRENT, localName);

  if (rc == L7_SUCCESS)
  {
    if (strcmp(localName, USMDB_IMAGE1_NAME) == 0)
    {
      *val = D_agentInventoryStackSTKname_image1;
    }
    else if (strcmp(localName, USMDB_IMAGE2_NAME) == 0)
    {
      *val = D_agentInventoryStackSTKname_image2;
    }
    else
    {
      *val = D_agentInventoryStackSTKname_unconfigured;
    }
  }
  else
  {
    *val = D_agentInventoryStackSTKname_unconfigured;
  }

  return L7_SUCCESS;
}

L7_RC_t snmpAgentInventoryStackSTKnameSet(L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 curVal;


  /* succeed if set to current value */
  if (snmpAgentInventoryStackSTKnameGet(&curVal) == L7_SUCCESS && curVal == val)
  {
    return L7_SUCCESS;
  }

  switch (val)
  {
  case D_agentInventoryStackSTKname_image1:
    rc = usmDbTransferFileNameLocalSet(USMDB_UNIT_CURRENT, USMDB_IMAGE1_NAME);
    break;

  case D_agentInventoryStackSTKname_image2:
    rc = usmDbTransferFileNameLocalSet(USMDB_UNIT_CURRENT, USMDB_IMAGE2_NAME);
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  return rc;
}

L7_RC_t snmpAgentInventoryStackActivateSTKSet(L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 snmp_buffer[128];

  switch (val)
  {
  case D_agentInventoryStackActivateSTK_enable:
    rc = usmDbTransferFileNameLocalGet(USMDB_UNIT_CURRENT, snmp_buffer);
    rc = usmDbImageActivate(USMDB_UNIT_CURRENT, snmp_buffer);
    break;

  case D_agentInventoryStackActivateSTK_disable:
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  return rc;
}

L7_RC_t snmpAgentInventoryStackDeleteSTKSet(L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 snmp_buffer[128];

  switch (val)
  {
  case D_agentInventoryStackDeleteSTK_enable:
    rc = usmDbTransferFileNameLocalGet(USMDB_UNIT_CURRENT, snmp_buffer);
    rc = usmDbFileDelete(USMDB_UNIT_CURRENT, snmp_buffer);
    break;

  case D_agentInventoryStackDeleteSTK_disable:
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  return rc;
}
  

/****************************************************************************************/

L7_RC_t snmpAgentInventorySupportedUnitExpectedCodeVerGet(L7_uint32 agentInventoryUnitNumber, L7_char8 *buf)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbUnitMgrSupportedCodeTargetIdGet(agentInventoryUnitNumber, &temp_val);

  if (rc == L7_SUCCESS)
  {
    sprintf(buf, "%d.%d.%d.%d", ((temp_val >> 3*8) & 0xFF), ((temp_val >> 2*8) & 0xFF), ((temp_val >> 1*8) & 0xFF), (temp_val & 0xFF));
  }

  return rc;
}

/****************************************************************************************/

L7_RC_t
snmpAgentInventoryUnitEntryGet(L7_uint32 agentInventoryUnitNumber)
{
  L7_uint32 temp_type;
#ifdef OLD_CODE
  L7_uint32 temp_unit = 0;

  if (usmDbUnitMgrStackMemberGetFirst(&temp_unit) == L7_SUCCESS)
  {
    /* search for the unit in the stack */
    do
    {
      if (temp_unit == agentInventoryUnitNumber)
        return L7_SUCCESS;
    }
    while (usmDbUnitMgrStackMemberGetNext(temp_unit, &temp_unit) == L7_SUCCESS);
  }

  /* could not find the unit in the stack */
  return L7_FAILURE;
#endif /* OLD_CODE */
  
  return usmDbUnitMgrUnitTypeGet(agentInventoryUnitNumber, &temp_type);
}

L7_RC_t
snmpAgentInventoryUnitEntryNextGet(L7_uint32 *agentInventoryUnitNumber)
{
#ifdef OLD_CODE
  L7_uint32 temp_unit = 0;

  if (usmDbUnitMgrStackMemberGetFirst(&temp_unit) == L7_SUCCESS)
  {
    /* search for the next unit in the stack */
    do
    {
      if (temp_unit > *agentInventoryUnitNumber)
      {
        *agentInventoryUnitNumber = temp_unit;
        return L7_SUCCESS;
      }
    }
    while (usmDbUnitMgrStackMemberGetNext(temp_unit, &temp_unit) == L7_SUCCESS);
  }

  /* could not find annother unit in the stack */
  return L7_FAILURE;
#endif

  return usmDbUnitMgrStackMemberGetNext(*agentInventoryUnitNumber, agentInventoryUnitNumber);
}

L7_RC_t snmpAgentInventoryUnitSupportedUnitIndexGet(L7_uint32 agentInventoryUnitNumber, L7_uint32 *val)
{
  L7_uint32 unit_type;
  L7_uint32 temp_type;
  L7_uint32 temp_index = 0;

  if (usmDbUnitMgrUnitTypeGet(agentInventoryUnitNumber, &unit_type) == L7_SUCCESS)
  {
    while (usmDbUnitMgrSupportedUnitIdNextGet(&temp_index, &temp_type) == L7_SUCCESS)
    {
      if (temp_type == unit_type)
      {
        *val = temp_index;
        return L7_SUCCESS;
      }
    }
    *val = 0;
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

L7_RC_t snmpAgentInventoryUnitMgmtAdminGet(L7_uint32 agentInventoryUnitNumber, L7_uint32 *val)
{
  L7_RC_t rc;
  usmDbUnitMgrMgmtFlag_t temp_val;

  rc = usmDbUnitMgrIsMgmtUnit(agentInventoryUnitNumber, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_UNITMGR_MGMT_ENABLED:
      *val = D_agentInventoryUnitMgmtAdmin_mgmtUnit;
      break;
    case L7_USMDB_UNITMGR_MGMT_DISABLED:
      *val = D_agentInventoryUnitMgmtAdmin_stackUnit;
      break;
    case L7_USMDB_UNITMGR_MGMT_UNASSIGNED:
      *val = D_agentInventoryUnitMgmtAdmin_mgmtUnassigned;
      break;
    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t snmpAgentInventoryUnitHWMgmtPrefGet(L7_uint32 agentInventoryUnitNumber, L7_uint32 *val)
{
  L7_RC_t rc;
  usmDbUnitMgrMgmtPref_t temp_val;

  rc = usmDbUnitMgrHwMgmtPrefGet(agentInventoryUnitNumber, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_UNITMGR_MGMTPREF_DISABLED:
      *val = D_agentInventoryUnitHWMgmtPref_disabled;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_1:
    case L7_USMDB_UNITMGR_MGMTPREF_2:
    case L7_USMDB_UNITMGR_MGMTPREF_3:
    case L7_USMDB_UNITMGR_MGMTPREF_4:
    case L7_USMDB_UNITMGR_MGMTPREF_5:
    case L7_USMDB_UNITMGR_MGMTPREF_6:
    case L7_USMDB_UNITMGR_MGMTPREF_7:
    case L7_USMDB_UNITMGR_MGMTPREF_8:
    case L7_USMDB_UNITMGR_MGMTPREF_9:
    case L7_USMDB_UNITMGR_MGMTPREF_10:
    case L7_USMDB_UNITMGR_MGMTPREF_11:
    case L7_USMDB_UNITMGR_MGMTPREF_12:
    case L7_USMDB_UNITMGR_MGMTPREF_13:
    case L7_USMDB_UNITMGR_MGMTPREF_14:
    case L7_USMDB_UNITMGR_MGMTPREF_15:
      *val = D_agentInventoryUnitHWMgmtPref_assigned;
      break;
    case L7_USMDB_UNITMGR_MGMTFUNC_UNASSIGNED:
      *val = D_agentInventoryUnitHWMgmtPref_unsassigned;
      break;
    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t snmpAgentInventoryUnitHWMgmtPrefValueGet(L7_uint32 agentInventoryUnitNumber, L7_uint32 *val)
{
  L7_RC_t rc;
  usmDbUnitMgrMgmtPref_t temp_val;

  rc = usmDbUnitMgrHwMgmtPrefGet(agentInventoryUnitNumber, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_UNITMGR_MGMTPREF_1:
      *val = 1;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_2:
      *val = 2;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_3:
      *val = 3;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_4:
      *val = 4;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_5:
      *val = 5;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_6:
      *val = 6;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_7:
      *val = 7;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_8:
      *val = 8;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_9:
      *val = 9;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_10:
      *val = 10;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_11:
      *val = 11;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_12:
      *val = 12;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_13:
      *val = 13;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_14:
      *val = 14;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_15:
      *val = 15;
      break;
    default:
      *val = 0;
      break;
    }
  }

  return rc;
}

L7_RC_t snmpAgentInventoryUnitAdminMgmtPrefGet(L7_uint32 agentInventoryUnitNumber, L7_uint32 *val)
{
  L7_RC_t rc;
  usmDbUnitMgrMgmtPref_t temp_val;

  rc = usmDbUnitMgrAdminMgmtPrefGet(agentInventoryUnitNumber, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_UNITMGR_MGMTPREF_DISABLED:
      *val = D_agentInventoryUnitAdminMgmtPref_disabled;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_1:
    case L7_USMDB_UNITMGR_MGMTPREF_2:
    case L7_USMDB_UNITMGR_MGMTPREF_3:
    case L7_USMDB_UNITMGR_MGMTPREF_4:
    case L7_USMDB_UNITMGR_MGMTPREF_5:
    case L7_USMDB_UNITMGR_MGMTPREF_6:
    case L7_USMDB_UNITMGR_MGMTPREF_7:
    case L7_USMDB_UNITMGR_MGMTPREF_8:
    case L7_USMDB_UNITMGR_MGMTPREF_9:
    case L7_USMDB_UNITMGR_MGMTPREF_10:
    case L7_USMDB_UNITMGR_MGMTPREF_11:
    case L7_USMDB_UNITMGR_MGMTPREF_12:
    case L7_USMDB_UNITMGR_MGMTPREF_13:
    case L7_USMDB_UNITMGR_MGMTPREF_14:
    case L7_USMDB_UNITMGR_MGMTPREF_15:
      *val = D_agentInventoryUnitAdminMgmtPref_assigned;
      break;
    case L7_USMDB_UNITMGR_MGMTFUNC_UNASSIGNED:
      *val = D_agentInventoryUnitAdminMgmtPref_unsassigned;
      break;
    }
  }

  return rc;
}

L7_RC_t snmpAgentInventoryUnitAdminMgmtPrefSet(L7_uint32 agentInventoryUnitNumber, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  usmDbUnitMgrMgmtPref_t temp_val = 0;

  switch (val)
  {
  case D_agentInventoryUnitAdminMgmtPref_disabled:
    temp_val = L7_USMDB_UNITMGR_MGMTPREF_DISABLED;
    break;
  case D_agentInventoryUnitAdminMgmtPref_unsassigned:
    temp_val = L7_USMDB_UNITMGR_MGMTFUNC_UNASSIGNED;
    break;
  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbUnitMgrAdminMgmtPrefSet(agentInventoryUnitNumber, temp_val);
  }

  return rc;
}

L7_RC_t snmpAgentInventoryUnitAdminMgmtPrefValueGet(L7_uint32 agentInventoryUnitNumber, L7_uint32 *val)
{
  L7_RC_t rc;
  usmDbUnitMgrMgmtPref_t temp_val;

  rc = usmDbUnitMgrAdminMgmtPrefGet(agentInventoryUnitNumber, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_UNITMGR_MGMTPREF_1:
      *val = 1;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_2:
      *val = 2;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_3:
      *val = 3;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_4:
      *val = 4;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_5:
      *val = 5;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_6:
      *val = 6;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_7:
      *val = 7;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_8:
      *val = 8;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_9:
      *val = 9;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_10:
      *val = 10;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_11:
      *val = 11;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_12:
      *val = 12;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_13:
      *val = 13;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_14:
      *val = 14;
      break;
    case L7_USMDB_UNITMGR_MGMTPREF_15:
      *val = 15;
      break;
    default:
      *val = 0;
      break;
    }
  }

  return rc;
}

L7_RC_t snmpAgentInventoryUnitAdminMgmtPrefValueSet(L7_uint32 agentInventoryUnitNumber, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  usmDbUnitMgrMgmtPref_t temp_val = 0;

  switch (val)
  {
  case 1:
    temp_val = L7_USMDB_UNITMGR_MGMTPREF_1;
    break;
  case 2:
    temp_val = L7_USMDB_UNITMGR_MGMTPREF_2;
    break;
  case 3:
    temp_val = L7_USMDB_UNITMGR_MGMTPREF_3;
    break;
  case 4:
    temp_val = L7_USMDB_UNITMGR_MGMTPREF_4;
    break;
  case 5:
    temp_val = L7_USMDB_UNITMGR_MGMTPREF_5;
    break;
  case 6:
    temp_val = L7_USMDB_UNITMGR_MGMTPREF_6;
    break;
  case 7:
    temp_val = L7_USMDB_UNITMGR_MGMTPREF_7;
    break;
  case 8:
    temp_val = L7_USMDB_UNITMGR_MGMTPREF_8;
    break;
  case 9:
    temp_val = L7_USMDB_UNITMGR_MGMTPREF_9;
    break;
  case 10:
    temp_val = L7_USMDB_UNITMGR_MGMTPREF_10;
    break;
  case 11:
    temp_val = L7_USMDB_UNITMGR_MGMTPREF_11;
    break;
  case 12:
    temp_val = L7_USMDB_UNITMGR_MGMTPREF_12;
    break;
  case 13:
    temp_val = L7_USMDB_UNITMGR_MGMTPREF_13;
    break;
  case 14:
    temp_val = L7_USMDB_UNITMGR_MGMTPREF_14;
    break;
  case 15:
    temp_val = L7_USMDB_UNITMGR_MGMTPREF_15;
    break;
  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbUnitMgrAdminMgmtPrefSet(agentInventoryUnitNumber, temp_val);
  }

  return rc;
}

L7_RC_t snmpAgentInventoryUnitMgmtAdminSet(L7_uint32 agentInventoryUnitNumber, L7_uint32 val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = snmpAgentInventoryUnitMgmtAdminGet(agentInventoryUnitNumber, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (val)
    {
    case D_agentInventoryUnitMgmtAdmin_mgmtUnit:
      if (temp_val == D_agentInventoryUnitMgmtAdmin_mgmtUnit)
        return L7_SUCCESS;
      break;

    case D_agentInventoryUnitMgmtAdmin_stackUnit:
    case D_agentInventoryUnitMgmtAdmin_mgmtUnassigned:
    default:
      rc = L7_FAILURE;
      break;
    }

    if (rc == L7_SUCCESS)
      rc = usmDbUnitMgrTransferMgmt(agentInventoryUnitNumber);
  }

  return rc;
}

L7_RC_t snmpAgentInventoryUnitStatusGet(L7_uint32 agentInventoryUnitNumber, L7_uint32 *val)
{
  L7_RC_t rc;
  usmDbUnitMgrStatus_t temp_val;

  rc = usmDbUnitMgrUnitStatusGet(agentInventoryUnitNumber, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_USMDB_UNITMGR_UNIT_OK:
      *val = D_agentInventoryUnitStatus_ok;
      break;
    case L7_USMDB_UNITMGR_UNIT_UNSUPPORTED:
      *val = D_agentInventoryUnitStatus_unsupported;
      break;
    case L7_USMDB_UNITMGR_CODE_MISMATCH:
      *val = D_agentInventoryUnitStatus_codeMismatch;
      break;
    case L7_USMDB_UNITMGR_CFG_MISMATCH:
      *val = D_agentInventoryUnitStatus_configMismatch;
      break;
    case L7_USMDB_UNITMGR_UNIT_NOT_PRESENT:
      *val = D_agentInventoryUnitStatus_notPresent;
      break;
    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t snmpAgentInventoryUnitDetectedCodeVerGet(L7_uint32 agentInventoryUnitNumber, L7_char8 *buf)
{
  L7_RC_t rc;
  usmDbCodeVersion_t temp_val;

  rc = usmDbUnitMgrDetectCodeVerRunningGet(agentInventoryUnitNumber, &temp_val);

  if (rc == L7_SUCCESS)
  {
    if (isalpha(temp_val.rel))
    {
      sprintf(buf, "%c.%d.%d.%d", temp_val.rel, temp_val.ver, temp_val.maint_level, temp_val.build_num);
    }
    else
    {
      sprintf(buf, "%d.%d.%d.%d", temp_val.rel, temp_val.ver, temp_val.maint_level, temp_val.build_num);
    }
  }

  return rc;
}

L7_RC_t snmpAgentInventoryUnitDetectedCodeInFlashVerGet(L7_uint32 agentInventoryUnitNumber, L7_char8 *buf)
{
  L7_RC_t rc;
  usmDbCodeVersion_t temp_val;

  rc = usmDbUnitMgrDetectCodeVerFlashGet(agentInventoryUnitNumber, &temp_val);

  if (rc == L7_SUCCESS)
  {
    if (isalpha(temp_val.rel))
    {
      sprintf(buf, "%c.%d.%d.%d", temp_val.rel, temp_val.ver, temp_val.maint_level, temp_val.build_num);
    }
    else
    {
      sprintf(buf, "%d.%d.%d.%d", temp_val.rel, temp_val.ver, temp_val.maint_level, temp_val.build_num);
    }
  }

  return rc;
}

L7_RC_t snmpAgentInventoryUnitUpTimeGet(L7_uint32 agentInventoryUnitNumber, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbUnitMgrUnitUpTimeGet(agentInventoryUnitNumber, &temp_val);

  if (rc == L7_SUCCESS)
  {
    /* convert from seconds to timeticks */
    *val = temp_val * SNMP_HUNDRED;
  }
  
  return rc;
}

L7_RC_t snmpAgentInventoryUnitReplicateSTKSet(L7_uint32 agentInventoryUnitNumber, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 snmp_buffer[128];

  switch (val)
  {
  case D_agentInventoryUnitReplicateSTK_disable:
    /* no-op */
    break;

  case D_agentInventoryUnitReplicateSTK_enable:
    rc = usmDbTransferFileNameLocalGet(USMDB_UNIT_CURRENT, snmp_buffer);
    rc = usmdbCdaCodeUpdateNoBlock(agentInventoryUnitNumber, snmp_buffer, snmp_buffer);
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  return rc;
}

#ifdef I_agentInventoryUnitReplicateSTKStatus
L7_RC_t snmpAgentInventoryUnitReplicateSTKStatusGet(L7_uint32 agentInventoryUnitNumber, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_char8 snmp_buffer[200];
  L7_uint32 unit;
  L7_uint32 mode;

  rc = usmDbUnitMgrNumberGet(&unit);
  if (L7_SUCCESS == rc)
  {
    /* local file copy */
    if (agentInventoryUnitNumber == unit)
    {
      if (L7_TRUE == usmDbTransferInProgressGet(unit))
      {
        usmDbTransferModeGet(unit, &mode);
        if (L7_TRANSFER_LOCAL == mode)
        {
          *val = D_agentInventoryUnitReplicateSTKStatus_inProgress;
        }
        else                    /* Some other file transfer is in progress */
        {
          *val = D_agentInventoryUnitReplicateSTKStatus_notInProgress;
        }
      }
      else
      {
        usmDbTransferResultGet(unit, val, snmp_buffer);
        switch (*val)
        {
          case L7_OTHER_FILE_TYPE_FAILED:
            *val = D_agentInventoryUnitReplicateSTKStatus_finishedWithError;
            break;

          case L7_TRANSFER_SUCCESS:
            *val = D_agentInventoryUnitReplicateSTKStatus_finishedWithSuccess;
            break;

          default:
            *val = D_agentInventoryUnitReplicateSTKStatus_notInProgress;
            break;
        }
      }
    }
    else
    {
      rc = snmpAgentInventoryStackReplicateSTKStatusGet(val);
    }
  }

  return rc;
}
#endif /* I_agentInventoryUnitReplicateSTKStatus */

L7_RC_t snmpAgentInventoryUnitReloadSet(L7_uint32 agentInventoryUnitNumber, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentInventoryUnitReload_enable:
    rc = usmDbUnitMgrResetUnit(agentInventoryUnitNumber);
    break;

  case D_agentInventoryUnitReload_disable:
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  return rc;
}

L7_RC_t snmpAgentInventoryUnitRowStatusSet(L7_uint32 agentInventoryUnitNumber, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;

  switch (val)
  {
  case D_agentInventoryUnitRowStatus_destroy:
    rc = usmDbUnitMgrDeleteUnit(agentInventoryUnitNumber);
    break;

  default:
    /* unknown or unallowed value */
    break;
  }

  return rc;
}

L7_RC_t snmpAgentInventoryUnitStandbyGet(L7_uint32 agentInventoryUnitNumber, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 opr_standby_unit = 0;
  L7_uint32 cfg_standby_unit = 0;
  L7_BOOL mode = L7_FALSE;

  rc = usmDbUnitMgrStandbyGet(&opr_standby_unit, &mode);

  if (rc == L7_SUCCESS)
  {
    rc = usmDbUnitMgrCfgStandbyGet(&cfg_standby_unit);

    if (rc == L7_SUCCESS)
    {
      if (cfg_standby_unit == agentInventoryUnitNumber)
      {
        *val = D_agentInventoryUnitStandby_standby_cfg;
      }
      else if (opr_standby_unit == agentInventoryUnitNumber)
      {
        *val = D_agentInventoryUnitStandby_standby_opr;
      }
      else
      {
        *val = D_agentInventoryUnitStandby_unassigned;
      }
    }
  }

  return rc;
}


L7_RC_t snmpAgentInventoryUnitStandbySet(L7_uint32 agentInventoryUnitNumber, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 mgmt_unit = 0;
  L7_uint32 standby_status = 0;

  /* Verify that the unit being set isn't the management unit */
  rc = usmDbUnitMgrMgrNumberGet(&mgmt_unit);
  if (rc == L7_SUCCESS && mgmt_unit != agentInventoryUnitNumber)
  {
    switch (val)
    {
      case D_agentInventoryUnitStandby_standby_cfg:
        rc = usmDbUnitMgrStandbySet(agentInventoryUnitNumber, L7_TRUE);
        break;
      case D_agentInventoryUnitStandby_standby_opr:
        rc = L7_FAILURE;
        break;
      case D_agentInventoryUnitStandby_unassigned:
        /* Verify that the unit being set unassigned is currently the operational or configured standby unit */
        rc = snmpAgentInventoryUnitStandbyGet(agentInventoryUnitNumber, &standby_status);
        if (rc == L7_SUCCESS && (standby_status == D_agentInventoryUnitStandby_standby_cfg || 
                                 standby_status == D_agentInventoryUnitStandby_standby_opr))
        {
          rc = usmDbUnitMgrStandbySet(agentInventoryUnitNumber, L7_FALSE);
        }
        else
        {
          rc = L7_FAILURE;
        }
        break;
      default:
        rc = L7_FAILURE;
        break;
    }
  }
  else
  {
    rc = L7_FAILURE;
  }
  return rc;
}

/****************************************************************************************/

L7_RC_t snmpAgentInventorySlotEntryGet(L7_uint32 agentInventoryUnitNumber, L7_uint32 agentInventorySlotNumber)
{
  L7_uint32 unit_id;
  L7_uint32 ptrSlots[L7_MAX_PHYSICAL_SLOTS_PER_UNIT];
  L7_uint32 numSlots;
  L7_uint32 i;

  if (usmDbUnitMgrUnitTypeGet(agentInventoryUnitNumber, &unit_id) == L7_SUCCESS)
  {
    if (usmDbUnitDbEntrySlotsGet(unit_id, &numSlots, ptrSlots) == L7_SUCCESS)
    {
      for (i = 0; i < numSlots; i++)
      {
        if (ptrSlots[i] == agentInventorySlotNumber)
        {
          return L7_SUCCESS;
        }
      }
    }
  }

  return L7_FAILURE;
}


L7_RC_t snmpAgentInventorySlotEntryNextGet(L7_uint32 *agentInventoryUnitNumber, L7_uint32 *agentInventorySlotNumber)
{
  L7_uint32 unit_id;
  L7_uint32 ptrSlots[L7_MAX_PHYSICAL_SLOTS_PER_UNIT];
  L7_uint32 numSlots;
  L7_uint32 i;
  L7_BOOL matchFirst = L7_FALSE;

  do
  {
    if (usmDbUnitMgrUnitTypeGet(*agentInventoryUnitNumber, &unit_id) == L7_SUCCESS)
    {
      if (usmDbUnitDbEntrySlotsGet(unit_id, &numSlots, ptrSlots) == L7_SUCCESS)
      {
        for (i = 0; i < numSlots; i++)
        {
          /* if matchFirst is true, then match on anything greater or equal to agentInventorySlotNumber */
          if ((matchFirst == L7_TRUE) ? (ptrSlots[i] >= *agentInventorySlotNumber) : (ptrSlots[i] > *agentInventorySlotNumber))
          {
            *agentInventorySlotNumber = ptrSlots[i];
            return L7_SUCCESS;
          }
        }
      }
    }

    /* look on the next unit */
    matchFirst = L7_TRUE;
    *agentInventorySlotNumber = 0;
  }
  while (snmpAgentInventoryUnitEntryNextGet(agentInventoryUnitNumber) == L7_SUCCESS);

  return L7_FAILURE;
}


L7_RC_t snmpAgentInventorySlotStatusGet(L7_uint32 agentInventoryUnitNumber, L7_uint32 agentInventorySlotNumber, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_BOOL temp_val;

  rc = usmDbSlotIsFullGet(agentInventoryUnitNumber, agentInventorySlotNumber, &temp_val);

  *val = D_agentInventorySlotStatus_error;

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *val = D_agentInventorySlotStatus_full;
      break;
  
    case L7_FALSE:
      *val = D_agentInventorySlotStatus_empty;
      break;
  
    default:
      break;
    }
  }

  return L7_SUCCESS;
}

L7_RC_t snmpAgentInventorySlotPowerModeGet(L7_uint32 agentInventoryUnitNumber, L7_uint32 agentInventorySlotNumber, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbSlotPowerModeGet(agentInventoryUnitNumber, agentInventorySlotNumber, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentInventorySlotPowerMode_enable;
      break;
    case L7_DISABLE:
      *val = D_agentInventorySlotPowerMode_disable;
      break;
    default:
      rc = L7_FAILURE;
      break;
    }
  }
  
  return rc;
}

L7_RC_t snmpAgentInventorySlotAdminModeGet(L7_uint32 agentInventoryUnitNumber, L7_uint32 agentInventorySlotNumber, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbSlotAdminModeGet(agentInventoryUnitNumber, agentInventorySlotNumber, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentInventorySlotAdminMode_enable;
      break;
    case L7_DISABLE:
      *val = D_agentInventorySlotAdminMode_disable;
      break;
    default:
      rc = L7_FAILURE;
      break;
    }
  }
  
  return rc;
}


L7_RC_t snmpAgentInventorySlotCapabilitiesGet(L7_uint32 agentInventoryUnitNumber, L7_uint32 agentInventorySlotNumber, L7_char8 *buf, L7_uint32 *buf_len)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  *buf_len = 1;

  rc = usmDbSlotSupportedPluggableGet(agentInventoryUnitNumber, agentInventorySlotNumber, &temp_val);

  if ((rc == L7_SUCCESS) && (temp_val == L7_TRUE))
  {
    buf[0] |= (1 << D_agentInventorySlotCapabilities_pluggable);
  }

  rc = usmDbSlotSupportedPowerdownGet(agentInventoryUnitNumber, agentInventorySlotNumber, &temp_val);

  if ((rc == L7_SUCCESS) && (temp_val == L7_TRUE))
  {
    buf[0] |= (1 << D_agentInventorySlotCapabilities_power_down);
  }

  return L7_SUCCESS;
}

/****************************************************************************************/

L7_RC_t snmpAgentInventoryStackPortIpTelephonyQOSSupportGet(L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val = L7_FALSE;

  rc = usmdbSpmFpsConfigQosModeGet(&temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *val = D_agentInventoryStackPortIpTelephonyQOSSupport_enable;
      break;
  
    case L7_FALSE:
      *val = D_agentInventoryStackPortIpTelephonyQOSSupport_disable;
      break;
  
    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t snmpAgentInventoryStackPortIpTelephonyQOSSupportSet(L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = L7_FALSE;

  switch (val)
  {
  case D_agentInventoryStackPortIpTelephonyQOSSupport_enable:
    temp_val = L7_TRUE;
    break;

  case D_agentInventoryStackPortIpTelephonyQOSSupport_disable:
    temp_val = L7_FALSE;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmdbSpmFpsConfigQosModeSet(temp_val);
  }

  return rc;
}

/****************************************************************************************/

L7_RC_t snmpAgentInventoryStackPortEntryGet(L7_uint32 agentInventoryStackPortIndex)
{
  SPM_STACK_PORT_ENTRY_t temp_entry;

  return usmdbSpmStackingCapablePortGet(agentInventoryStackPortIndex, &temp_entry);
}

L7_RC_t snmpAgentInventoryStackPortEntryNextGet(L7_uint32 *agentInventoryStackPortIndex)
{
  L7_RC_t rc = L7_SUCCESS;
  SPM_STACK_PORT_ENTRY_t temp_entry;

  /* clear data in temp_entry */
  memset(&temp_entry, 0, sizeof(temp_entry));

  temp_entry.snmp_index = *agentInventoryStackPortIndex;

  rc = usmdbSpmStackingCapablePortNextGet(&temp_entry);

  if (rc == L7_SUCCESS)
  {
    *agentInventoryStackPortIndex = temp_entry.snmp_index;
  }

  return rc;
}

L7_RC_t snmpAgentInventoryStackPortUnitGet(L7_uint32 agentInventoryStackPortIndex, L7_uint32 *val)
{
  L7_RC_t rc = L7_SUCCESS;
  SPM_STACK_PORT_ENTRY_t temp_entry;

  rc = usmdbSpmStackingCapablePortGet(agentInventoryStackPortIndex, &temp_entry);

  if (rc == L7_SUCCESS)
  {
    *val = temp_entry.unit;
  }

  return rc;
}

L7_RC_t snmpAgentInventoryStackPortTagGet(L7_uint32 agentInventoryStackPortIndex, L7_char8 *buffer)
{
  L7_RC_t rc = L7_SUCCESS;
  SPM_STACK_PORT_ENTRY_t temp_entry;

  rc = usmdbSpmStackingCapablePortGet(agentInventoryStackPortIndex, &temp_entry);

  if (rc == L7_SUCCESS)
  {
    strcpy(buffer, temp_entry.port_tag);
  }

  return rc;
}

L7_RC_t snmpAgentInventoryStackPortConfiguredStackModeGet(L7_uint32 agentInventoryStackPortIndex, L7_uint32 *val)
{
  L7_RC_t rc=L7_FAILURE;
  SPM_STACK_PORT_ENTRY_t temp_entry;

  rc = usmdbSpmStackingCapablePortGet(agentInventoryStackPortIndex, &temp_entry);

  if (rc == L7_SUCCESS && temp_entry.fps_port == L7_TRUE)
    {
      switch (temp_entry.port_info.config_mode)
      {
      case L7_FALSE:
        *val = D_agentInventoryStackPortConfiguredStackMode_ethernet;
        break;
  
      case L7_TRUE:
        *val = D_agentInventoryStackPortConfiguredStackMode_stack;
        break;
  
      default:
        /* unknown case */
        rc = L7_FAILURE;
        break;
      }
   }
 else
   rc=L7_FAILURE;

  return rc;
}

L7_RC_t snmpAgentInventoryStackPortConfiguredStackModeSet(L7_uint32 agentInventoryStackPortIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  SPM_STACK_PORT_ENTRY_t temp_entry;
  L7_uint32 temp_val = 0;

  /* get the current status of this port */
  rc = usmdbSpmStackingCapablePortGet(agentInventoryStackPortIndex, &temp_entry);
  
  if (rc == L7_SUCCESS)
  {
    if (temp_entry.fps_port == L7_TRUE)
    {
      switch (val)
      {
      case D_agentInventoryStackPortConfiguredStackMode_ethernet:
        temp_val = L7_FALSE;
        break;
  
      case D_agentInventoryStackPortConfiguredStackMode_stack:
        temp_val = L7_TRUE;
        break;
  
      default:
        /* unknown case */
        rc = L7_FAILURE;
        break;
      }
    }
    else
    {
      /* Non-FPS ports can never be configured */
      rc = L7_FAILURE;
    }
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmdbSpmFpsConfigStackingModeSet(agentInventoryStackPortIndex, temp_val);
  }
  
  return rc;
}

L7_RC_t snmpAgentInventoryStackPortRunningStackModeGet(L7_uint32 agentInventoryStackPortIndex, L7_uint32 *val)
{
  L7_RC_t rc = L7_SUCCESS;
  SPM_STACK_PORT_ENTRY_t temp_entry;

  rc = usmdbSpmStackingCapablePortGet(agentInventoryStackPortIndex, &temp_entry);

  if (rc == L7_SUCCESS)
  {
    if (temp_entry.fps_port == L7_TRUE)
    {
      switch (temp_entry.port_info.runtime_mode)
      {
      case 0:
        *val = D_agentInventoryStackPortRunningStackMode_ethernet;
        break;
  
      case 1:
        *val = D_agentInventoryStackPortRunningStackMode_stack;
        break;
  
      default:
        /* unknown case */
        rc = L7_FAILURE;
        break;
      }
    }
    else
    {
      /* Non-FPS ports are always stack ports */
      *val = D_agentInventoryStackPortRunningStackMode_stack;
    }
  }

  return rc;
}

L7_RC_t snmpAgentInventoryStackPortLinkStatusGet(L7_uint32 agentInventoryStackPortIndex, L7_uint32 *val)
{
  L7_RC_t rc = L7_SUCCESS;
  SPM_STACK_PORT_ENTRY_t temp_entry;

  rc = usmdbSpmStackingCapablePortGet(agentInventoryStackPortIndex, &temp_entry);

  if (rc == L7_SUCCESS)
  {
    switch (temp_entry.port_info.link_status)
    {
    case L7_TRUE:
      *val = D_agentInventoryStackPortLinkStatus_up;
      break;

    case L7_FALSE:
      *val = D_agentInventoryStackPortLinkStatus_down;
      break;

    default:
      /* unknown case */
      return L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t snmpAgentInventoryStackPortLinkSpeedGet(L7_uint32 agentInventoryStackPortIndex, L7_uint32 *val)
{
  L7_RC_t rc = L7_SUCCESS;
  SPM_STACK_PORT_ENTRY_t temp_entry;

  rc = usmdbSpmStackingCapablePortGet(agentInventoryStackPortIndex, &temp_entry);

  if (rc == L7_SUCCESS)
  {
    *val = temp_entry.port_info.speed;
  }

  return rc;
}

L7_RC_t snmpAgentInventoryStackPortDataRateGet(L7_uint32 agentInventoryStackPortIndex, L7_uint32 *val)
{
  L7_RC_t rc = L7_SUCCESS;
  SPM_STACK_PORT_ENTRY_t temp_entry;

  rc = usmdbSpmStackingCapablePortGet(agentInventoryStackPortIndex, &temp_entry);

  if (rc == L7_SUCCESS)
  {
    *val = temp_entry.port_info.rx_data_rate + temp_entry.port_info.tx_data_rate;
  }

  return rc;
}

L7_RC_t snmpAgentInventoryStackPortErrorRateGet(L7_uint32 agentInventoryStackPortIndex, L7_uint32 *val)
{
  L7_RC_t rc = L7_SUCCESS;
  SPM_STACK_PORT_ENTRY_t temp_entry;

  rc = usmdbSpmStackingCapablePortGet(agentInventoryStackPortIndex, &temp_entry);

  if (rc == L7_SUCCESS)
  {
    *val = temp_entry.port_info.rx_error_rate + temp_entry.port_info.tx_error_rate;
  }

  return rc;
}

L7_RC_t snmpAgentInventoryStackPortTotalErrorsGet(L7_uint32 agentInventoryStackPortIndex, L7_uint32 *val)
{
  L7_RC_t rc = L7_SUCCESS;
  SPM_STACK_PORT_ENTRY_t temp_entry;

  rc = usmdbSpmStackingCapablePortGet(agentInventoryStackPortIndex, &temp_entry);

  if (rc == L7_SUCCESS)
  {
    *val = temp_entry.port_info.rx_total_errors + temp_entry.port_info.tx_total_errors;
  }

  return rc;
}

/* End Function Definitions */


