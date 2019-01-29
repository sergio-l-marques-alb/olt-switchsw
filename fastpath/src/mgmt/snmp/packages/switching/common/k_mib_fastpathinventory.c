/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   k_mib_entity.c
*
* @purpose    System-Specific code for Entity MIB - RFC 2737
*
* @component  SNMP
*
* @comments
*
* @create     6/04/2003
*
* @author     cpverne
* @end
*
**********************************************************************/
#include "k_private_base.h"
#include "k_mib_fastpathinventory_api.h"
#include "usmdb_dim_api.h"
#include "usmdb_cardmgr_api.h"
#include "usmdb_cnfgr_api.h"
#include "usmdb_common.h"
#include "usmdb_sim_api.h"
#include "usmdb_unitmgr_api.h"
#include "usmdb_file_api.h"
#include "strlib_base_common.h"

#ifdef L7_NSF_PACKAGE
#include "k_mib_fastpathnsf_api.h"
#endif

#include "unitmgr_api.h"
#include "usmdb_util_api.h"

agentInventorySFSGroup_t*
k_agentInventorySFSGroup_get(int serialNum, ContextInfo *contextInfo,
                               int nominator)
{
  static agentInventorySFSGroup_t agentInventorySFSData;
  L7_BOOL mode;
  
  memset(&agentInventorySFSData,0x0,sizeof(agentInventorySFSData));

  switch (nominator)
  {
  case -1:
  case I_agentInventoryStackUnitNumber:
        if (nominator != -1) break;

  case I_agentInventorySFS:
    if (usmDbUnitMgrStackFirmwareSyncModeGet(&mode) == L7_SUCCESS)
    { 
      if (mode == L7_ENABLE)
      {
        agentInventorySFSData.agentInventorySFS= D_agentInventorySFS_enable;
      }
      else
      {
        agentInventorySFSData.agentInventorySFS= D_agentInventorySFS_disable;
      } 
      SET_VALID(I_agentInventorySFS, agentInventorySFSData.valid);
    }

    if (nominator != -1) break;

  case I_agentInventorySFSAllowDowngrade:
    if (usmDbUnitMgrStackFirmwareSyncAllowDowngradeModeGet(&mode) == L7_SUCCESS)
    {
      if (mode == L7_ENABLE)
      {
        agentInventorySFSData.agentInventorySFSAllowDowngrade = 
                              D_agentInventorySFSAllowDowngrade_enable; 
      }
      else
      {
        agentInventorySFSData.agentInventorySFSAllowDowngrade = 
                              D_agentInventorySFSAllowDowngrade_disable;
      }    
      SET_VALID(I_agentInventorySFSAllowDowngrade, agentInventorySFSData.valid);
    }
    if (nominator != -1) break;

  case I_agentInventorySFSTrap:
    if (usmDbUnitMgrStackFirmwareSyncTrapModeGet(&mode) == L7_SUCCESS)
    {
      if (mode == L7_ENABLE)
      {
        agentInventorySFSData.agentInventorySFSTrap = 
                              D_agentInventorySFSTrap_enable;
      }
      else
      {
        agentInventorySFSData.agentInventorySFSTrap = 
                              D_agentInventorySFSTrap_disable;
      }
      SET_VALID(I_agentInventorySFSTrap, agentInventorySFSData.valid);
    }

    if (nominator != -1) break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentInventorySFSData.valid))
    return(NULL);

  return(&agentInventorySFSData);
 
}



agentInventoryStackGroup_t *
k_agentInventoryStackGroup_get(int serialNum, ContextInfo *contextInfo,
                               int nominator)
{
  static agentInventoryStackGroup_t agentInventoryStackGroupData;

  #ifndef L7_STACKING_PACKAGE
  return(NULL);
  #endif 
  
  switch (nominator)
  {
  case -1:

  case I_agentInventoryStackReplicateSTK:
    agentInventoryStackGroupData.agentInventoryStackReplicateSTK = D_agentInventoryStackReplicateSTK_disable;
    SET_VALID(I_agentInventoryStackReplicateSTK, agentInventoryStackGroupData.valid);
    if (nominator != -1) break;
    
  case I_agentInventoryStackReload:
    agentInventoryStackGroupData.agentInventoryStackReload = D_agentInventoryStackReload_disable;
    SET_VALID(I_agentInventoryStackReload, agentInventoryStackGroupData.valid);
    if (nominator != -1) break;

  case I_agentInventoryStackMaxUnitNumber:
    agentInventoryStackGroupData.agentInventoryStackMaxUnitNumber = L7_MAX_UNITS_PER_STACK;
    SET_VALID(I_agentInventoryStackMaxUnitNumber, agentInventoryStackGroupData.valid);
    if (nominator != -1) break;

  case I_agentInventoryStackReplicateSTKStatus:
    if (snmpAgentInventoryStackReplicateSTKStatusGet(&agentInventoryStackGroupData.agentInventoryStackReplicateSTKStatus) == L7_SUCCESS)
      SET_VALID(I_agentInventoryStackReplicateSTKStatus, agentInventoryStackGroupData.valid);
    if (nominator != -1) break;

  case I_agentInventoryStackSTKname:
    if (snmpAgentInventoryStackSTKnameGet(&agentInventoryStackGroupData.agentInventoryStackSTKname) == L7_SUCCESS)
      SET_VALID(I_agentInventoryStackSTKname, agentInventoryStackGroupData.valid);
    if (nominator != -1) break;
 
  case I_agentInventoryStackActivateSTK:
    agentInventoryStackGroupData.agentInventoryStackActivateSTK = D_agentInventoryStackActivateSTK_disable;
    SET_VALID(I_agentInventoryStackActivateSTK, agentInventoryStackGroupData.valid);
    if (nominator != -1) break;
  
  case I_agentInventoryStackDeleteSTK:
    agentInventoryStackGroupData.agentInventoryStackDeleteSTK = D_agentInventoryStackDeleteSTK_disable;
    SET_VALID(I_agentInventoryStackDeleteSTK, agentInventoryStackGroupData.valid);
    break;
    
  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentInventoryStackGroupData.valid))
    return(NULL);

  return(&agentInventoryStackGroupData);
}

#ifdef SETS

int
k_agentInventorySFSGroup_test(ObjectInfo *object, ObjectSyntax *value,
                               doList_t *dp, ContextInfo *contextInfo)
{
  return NO_ERROR;
}

int k_agentInventorySFSGroup_set
    SR_PROTOTYPE((agentInventorySFSGroup_t *data,
                  ContextInfo *contextInfo,
                  int function))
{

 if(VALID(I_agentInventorySFS,data->valid) &&
    snmpAgentInventoryStackFirmwareSyncModeSet(data->agentInventorySFS)!= L7_SUCCESS)
   return COMMIT_FAILED_ERROR;

 if(VALID(I_agentInventorySFSAllowDowngrade,data->valid) &&
    snmpAgentInventoryStackFirmwareSyncAllowDowngradeModeSet(data->agentInventorySFSAllowDowngrade)!= L7_SUCCESS)
   return COMMIT_FAILED_ERROR;

 if(VALID(I_agentInventorySFSTrap,data->valid) &&
    snmpAgentInventoryStackFirmwareSyncTrapModeSet(data->agentInventorySFSTrap)!= L7_SUCCESS)
   return COMMIT_FAILED_ERROR;

  return NO_ERROR;;
}

int
k_agentInventorySFSGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                                doList_t *doHead, doList_t *dp)
{
  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentInventoryStackGroup_test(ObjectInfo *object, ObjectSyntax *value,
                               doList_t *dp, ContextInfo *contextInfo)
{
  return NO_ERROR;
}

int
k_agentInventoryStackGroup_ready(ObjectInfo *object, ObjectSyntax *value, 
                                doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentInventoryStackGroup_set(agentInventoryStackGroup_t *data,
                              ContextInfo *contextInfo, int function)
{
#ifdef L7_STACKING_PACKAGE
  L7_char8 fileName[SNMP_BUFFER_LEN];
  L7_char8 auditMsg[SNMP_BUFFER_LEN];

  memset(fileName, L7_NULL, sizeof(fileName));
  memset(auditMsg, L7_NULL, sizeof(auditMsg));

  if (VALID(I_agentInventoryStackSTKname, data->valid) &&
      snmpAgentInventoryStackSTKnameSet(data->agentInventoryStackSTKname) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;
  
  if (VALID(I_agentInventoryStackReplicateSTK, data->valid) &&
      snmpAgentInventoryStackReplicateSTKSet(data->agentInventoryStackReplicateSTK) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentInventoryStackActivateSTK, data->valid) &&
      snmpAgentInventoryStackActivateSTKSet(data->agentInventoryStackActivateSTK) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentInventoryStackDeleteSTK, data->valid))
  { 
    if (snmpAgentInventoryStackDeleteSTKSet(data->agentInventoryStackDeleteSTK) != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }
    
    if (data->agentInventoryStackDeleteSTK == D_agentInventoryUnitDeleteSTK_enable)
    {
      usmDbTransferFileNameLocalGet(USMDB_UNIT_CURRENT, fileName);
      osapiSnprintf(auditMsg, sizeof(auditMsg),
                    pStrInfo_base_Auditing_FileDelete, fileName);
      SnmpCmdLoggerAuditEntryAdd(contextInfo, auditMsg);
    }
  }

  if (VALID(I_agentInventoryStackReload, data->valid) && 
      snmpAgentInventoryStackReloadSet(data->agentInventoryStackReload) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  return NO_ERROR;
#else
  return(COMMIT_FAILED_ERROR);
#endif
}

#ifdef SR_agentInventoryStackGroup_UNDO
/* add #define SR_agentInventoryStackGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentInventoryStackGroup family.
 */
int
agentInventoryStackGroup_undo(doList_t *doHead, doList_t *doCur,
                             ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentInventoryStackGroup_UNDO */

#endif /* SETS */

agentInventorySupportedUnitEntry_t *
k_agentInventorySupportedUnitEntry_get(int serialNum, ContextInfo *contextInfo,
                                       int nominator,
                                       int searchType,
                                       SR_UINT32 agentInventorySupportedUnitIndex)
{
  static agentInventorySupportedUnitEntry_t agentInventorySupportedUnitEntryData;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 unitType = 0;
  
  #ifndef L7_STACKING_PACKAGE
  return(NULL);
  #endif
  
  ZERO_VALID(agentInventorySupportedUnitEntryData.valid);
  
  agentInventorySupportedUnitEntryData.agentInventorySupportedUnitIndex = agentInventorySupportedUnitIndex;
  SET_VALID(I_agentInventorySupportedUnitIndex, agentInventorySupportedUnitEntryData.valid);
  
  if ((searchType == EXACT) ?
      (usmDbUnitMgrSupportedUnitIdGet(agentInventorySupportedUnitEntryData.agentInventorySupportedUnitIndex, &unitType) != L7_SUCCESS) :
      ((usmDbUnitMgrSupportedUnitIdGet(agentInventorySupportedUnitEntryData.agentInventorySupportedUnitIndex, &unitType) != L7_SUCCESS) &&
       (usmDbUnitMgrSupportedUnitIdNextGet(&agentInventorySupportedUnitEntryData.agentInventorySupportedUnitIndex, &unitType) != L7_SUCCESS)))
    return(NULL);
  
  switch (nominator)
  {
  case -1:
  case I_agentInventorySupportedUnitIndex:
    if (nominator != -1) break;

  case I_agentInventorySupportedUnitModelIdentifier:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbUnitMgrSupportedModelGet(agentInventorySupportedUnitEntryData.agentInventorySupportedUnitIndex,
                                      snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromText(&agentInventorySupportedUnitEntryData.agentInventorySupportedUnitModelIdentifier,
                                    snmp_buffer) == L7_TRUE)
      SET_VALID(I_agentInventorySupportedUnitModelIdentifier, agentInventorySupportedUnitEntryData.valid);
    if (nominator != -1) break;

  case I_agentInventorySupportedUnitDescription:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbUnitMgrSupportedDescriptionGet(agentInventorySupportedUnitEntryData.agentInventorySupportedUnitIndex,
                                            snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromText(&agentInventorySupportedUnitEntryData.agentInventorySupportedUnitDescription,
                                    snmp_buffer) == L7_TRUE)
      SET_VALID(I_agentInventorySupportedUnitDescription, agentInventorySupportedUnitEntryData.valid);
    if (nominator != -1) break;

  case I_agentInventorySupportedUnitExpectedCodeVer:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (snmpAgentInventorySupportedUnitExpectedCodeVerGet(agentInventorySupportedUnitEntryData.agentInventorySupportedUnitIndex,
                                                          snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromText(&agentInventorySupportedUnitEntryData.agentInventorySupportedUnitExpectedCodeVer,
                                    snmp_buffer) == L7_TRUE)
      SET_VALID(I_agentInventorySupportedUnitExpectedCodeVer, agentInventorySupportedUnitEntryData.valid);
    if (nominator != -1) break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentInventorySupportedUnitEntryData.valid))
    return(NULL);

  return(&agentInventorySupportedUnitEntryData);
}

#ifdef SETS
int
k_agentInventorySupportedUnitEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                        doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentInventorySupportedUnitEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                         doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentInventorySupportedUnitEntry_set_defaults(doList_t *dp)
{
    agentInventorySupportedUnitEntry_t *data = (agentInventorySupportedUnitEntry_t *) (dp->data);

    if ((data->agentInventorySupportedUnitModelIdentifier = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->agentInventorySupportedUnitDescription = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->agentInventorySupportedUnitExpectedCodeVer = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentInventorySupportedUnitEntry_set(agentInventorySupportedUnitEntry_t *data,
                                       ContextInfo *contextInfo, int function)
{
  L7_uint32 temp_uint32;

  #ifndef L7_STACKING_PACKAGE
  return COMMIT_FAILED_ERROR;
  #endif
  
  if (usmDbUnitMgrSupportedUnitIdGet(data->agentInventorySupportedUnitIndex, &temp_uint32) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  return NO_ERROR;
}

#ifdef SR_agentInventorySupportedUnitEntry_UNDO
/* add #define SR_agentInventorySupportedUnitEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentInventorySupportedUnitEntry family.
 */
int
agentInventorySupportedUnitEntry_undo(doList_t *doHead, doList_t *doCur,
                                      ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentInventorySupportedUnitEntry_UNDO */

#endif /* SETS */

agentInventoryUnitEntry_t *
k_agentInventoryUnitEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_UINT32 agentInventoryUnitNumber)
{
  static agentInventoryUnitEntry_t agentInventoryUnitEntryData;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  static L7_BOOL firstTime = L7_TRUE;
 
  L7_uint32 unitTypeIndex = 0;  
  L7_char8 image1FileName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 image2FileName[L7_CLI_MAX_STRING_LENGTH];
 
  #ifndef L7_STACKING_PACKAGE
  return(NULL);
  #endif
  
  if (firstTime == L7_TRUE)
  {
    agentInventoryUnitEntryData.agentInventoryUnitDetectedCodeVer = MakeOctetString(NULL,0);
    agentInventoryUnitEntryData.agentInventoryUnitDetectedCodeInFlashVer = MakeOctetString(NULL,0);
    agentInventoryUnitEntryData.agentInventoryUnitDescription = MakeOctetString(NULL,0);
    agentInventoryUnitEntryData.agentInventoryUnitImage1Version = MakeOctetString(NULL,0);
    agentInventoryUnitEntryData.agentInventoryUnitImage2Version = MakeOctetString(NULL,0);
    agentInventoryUnitEntryData.agentInventoryUnitSTKname = 2;
    firstTime = L7_FALSE;
  }
  
  ZERO_VALID(agentInventoryUnitEntryData.valid);
  
  agentInventoryUnitEntryData.agentInventoryUnitNumber = agentInventoryUnitNumber;
  SET_VALID(I_agentInventoryUnitNumber, agentInventoryUnitEntryData.valid);
  
  if ((searchType == EXACT) ?
      (snmpAgentInventoryUnitEntryGet(agentInventoryUnitEntryData.agentInventoryUnitNumber) != L7_SUCCESS) :
      ((snmpAgentInventoryUnitEntryGet(agentInventoryUnitEntryData.agentInventoryUnitNumber) != L7_SUCCESS) &&
       (snmpAgentInventoryUnitEntryNextGet(&agentInventoryUnitEntryData.agentInventoryUnitNumber) != L7_SUCCESS)))
    return(NULL);
  
  switch (nominator)
  {
  case -1:
  case I_agentInventoryUnitNumber:
    if (nominator != -1) break;

  case I_agentInventoryUnitAssignNumber:
    agentInventoryUnitEntryData.agentInventoryUnitAssignNumber = 0;
    SET_VALID(I_agentInventoryUnitAssignNumber, agentInventoryUnitEntryData.valid);
    if (nominator != -1) break;

  case I_agentInventoryUnitType:
    if (usmDbUnitMgrUnitTypeGet(agentInventoryUnitEntryData.agentInventoryUnitNumber,
                                &agentInventoryUnitEntryData.agentInventoryUnitType) == L7_SUCCESS)
      SET_VALID(I_agentInventoryUnitType, agentInventoryUnitEntryData.valid);
    if (nominator != -1) break;

  case I_agentInventoryUnitSupportedUnitIndex:
    if (snmpAgentInventoryUnitSupportedUnitIndexGet(agentInventoryUnitEntryData.agentInventoryUnitNumber,
                                                    &agentInventoryUnitEntryData.agentInventoryUnitSupportedUnitIndex) == L7_SUCCESS)
      SET_VALID(I_agentInventoryUnitSupportedUnitIndex, agentInventoryUnitEntryData.valid);
    if (nominator != -1) break;

  case I_agentInventoryUnitMgmtAdmin:
    if (snmpAgentInventoryUnitMgmtAdminGet(agentInventoryUnitEntryData.agentInventoryUnitNumber,
                                           &agentInventoryUnitEntryData.agentInventoryUnitMgmtAdmin) == L7_SUCCESS)
      SET_VALID(I_agentInventoryUnitMgmtAdmin, agentInventoryUnitEntryData.valid);
    if (nominator != -1) break;

  case I_agentInventoryUnitHWMgmtPref:
    if (snmpAgentInventoryUnitHWMgmtPrefGet(agentInventoryUnitEntryData.agentInventoryUnitNumber,
                                            &agentInventoryUnitEntryData.agentInventoryUnitHWMgmtPref) == L7_SUCCESS)
      SET_VALID(I_agentInventoryUnitHWMgmtPref, agentInventoryUnitEntryData.valid);
    if (nominator != -1) break;

  case I_agentInventoryUnitHWMgmtPrefValue:
    if (snmpAgentInventoryUnitHWMgmtPrefValueGet(agentInventoryUnitEntryData.agentInventoryUnitNumber,
                                                 &agentInventoryUnitEntryData.agentInventoryUnitHWMgmtPrefValue) == L7_SUCCESS)
      SET_VALID(I_agentInventoryUnitHWMgmtPrefValue, agentInventoryUnitEntryData.valid);
    if (nominator != -1) break;

  case I_agentInventoryUnitAdminMgmtPref:
    if (snmpAgentInventoryUnitAdminMgmtPrefGet(agentInventoryUnitEntryData.agentInventoryUnitNumber,
                                               &agentInventoryUnitEntryData.agentInventoryUnitAdminMgmtPref) == L7_SUCCESS)
      SET_VALID(I_agentInventoryUnitAdminMgmtPref, agentInventoryUnitEntryData.valid);
    if (nominator != -1) break;

  case I_agentInventoryUnitAdminMgmtPrefValue:
    if (snmpAgentInventoryUnitAdminMgmtPrefValueGet(agentInventoryUnitEntryData.agentInventoryUnitNumber,
                                                    &agentInventoryUnitEntryData.agentInventoryUnitAdminMgmtPrefValue) == L7_SUCCESS)
      SET_VALID(I_agentInventoryUnitAdminMgmtPrefValue, agentInventoryUnitEntryData.valid);
    if (nominator != -1) break;

  case I_agentInventoryUnitStatus:
    if (snmpAgentInventoryUnitStatusGet(agentInventoryUnitEntryData.agentInventoryUnitNumber,
                                       &agentInventoryUnitEntryData.agentInventoryUnitStatus) == L7_SUCCESS)
      SET_VALID(I_agentInventoryUnitStatus, agentInventoryUnitEntryData.valid);
    if (nominator != -1) break;

  case I_agentInventoryUnitDetectedCodeVer:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (snmpAgentInventoryUnitDetectedCodeVerGet(agentInventoryUnitEntryData.agentInventoryUnitNumber,
                                                snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromText(&agentInventoryUnitEntryData.agentInventoryUnitDetectedCodeVer,
                                    snmp_buffer) == L7_TRUE)
      SET_VALID(I_agentInventoryUnitDetectedCodeVer, agentInventoryUnitEntryData.valid);
    if (nominator != -1) break;

  case I_agentInventoryUnitDetectedCodeInFlashVer:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (snmpAgentInventoryUnitDetectedCodeInFlashVerGet(agentInventoryUnitEntryData.agentInventoryUnitNumber,
                                                       snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromText(&agentInventoryUnitEntryData.agentInventoryUnitDetectedCodeInFlashVer,
                                    snmp_buffer) == L7_TRUE)
      SET_VALID(I_agentInventoryUnitDetectedCodeInFlashVer, agentInventoryUnitEntryData.valid);
    if (nominator != -1) break;

  case I_agentInventoryUnitUpTime:
    if (snmpAgentInventoryUnitUpTimeGet(agentInventoryUnitEntryData.agentInventoryUnitNumber,
                                       &agentInventoryUnitEntryData.agentInventoryUnitUpTime) == L7_SUCCESS)
      SET_VALID(I_agentInventoryUnitUpTime, agentInventoryUnitEntryData.valid);
    if (nominator != -1) break;

  case I_agentInventoryUnitDescription:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if ((usmDbUnitMgrUnitTypeIdIndexGet(agentInventoryUnitEntryData.agentInventoryUnitNumber, &unitTypeIndex) == L7_SUCCESS)
           && usmDbUnitMgrSupportedDescriptionGet(unitTypeIndex,
                                         snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromText(&agentInventoryUnitEntryData.agentInventoryUnitDescription,
                                    snmp_buffer) == L7_TRUE)
      SET_VALID(I_agentInventoryUnitDescription, agentInventoryUnitEntryData.valid);
    if (nominator != -1) break;

  case I_agentInventoryUnitImage1Version  :
    
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
        
    if (usmDbImageFileNameGet(0, image1FileName) != L7_SUCCESS)
    {
      strcpy(snmp_buffer, "no image");
    }
    else
    {
      if (usmDbImageVersionGet(agentInventoryUnitEntryData.agentInventoryUnitNumber, 
            image1FileName, snmp_buffer) != L7_SUCCESS)
      {
        strcpy(snmp_buffer, "no image");
      }
    }
    
    if (SafeMakeOctetStringFromText(&agentInventoryUnitEntryData.agentInventoryUnitImage1Version,
                                    snmp_buffer) == L7_TRUE)
    {
      SET_VALID(I_agentInventoryUnitImage1Version, agentInventoryUnitEntryData.valid);
    }
    
    if (nominator != -1) break;

  case I_agentInventoryUnitImage2Version  :
    
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
        
    if (usmDbImageFileNameGet(1, image2FileName) != L7_SUCCESS)
    {
      strcpy(snmp_buffer, "no image");
    }
    else
    {
      if (usmDbImageVersionGet(agentInventoryUnitEntryData.agentInventoryUnitNumber, 
            image2FileName, snmp_buffer) != L7_SUCCESS)
      {
        strcpy(snmp_buffer, "no image");
      }
    }
    
    if (SafeMakeOctetStringFromText(&agentInventoryUnitEntryData.agentInventoryUnitImage2Version,
                                    snmp_buffer) == L7_TRUE)
    {
      SET_VALID(I_agentInventoryUnitImage2Version, agentInventoryUnitEntryData.valid);
    }
    
    if (nominator != -1) break;

  case I_agentInventoryUnitSTKname  :
    
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
        
    if (usmDbTransferFileNameLocalGet(USMDB_UNIT_CURRENT, snmp_buffer) != L7_SUCCESS)
    {
      usmDbImageFileNameGet(0, snmp_buffer);
    }

    usmDbImageFileNameGet(1, image2FileName);
    if(strcmp(snmp_buffer, image2FileName) == 0)
    {
      agentInventoryUnitEntryData.agentInventoryUnitSTKname = D_agentInventoryUnitSTKname_image2;
    }
    else
    {
      agentInventoryUnitEntryData.agentInventoryUnitSTKname = D_agentInventoryUnitSTKname_image1;
    }
    
    SET_VALID(I_agentInventoryUnitSTKname, agentInventoryUnitEntryData.valid);
        
    if (nominator != -1) break;


  case I_agentInventoryUnitReplicateSTK:
    agentInventoryUnitEntryData.agentInventoryUnitReplicateSTK = D_agentInventoryUnitReplicateSTK_disable;
    SET_VALID(I_agentInventoryUnitReplicateSTK, agentInventoryUnitEntryData.valid);
    if (nominator != -1) break;

  case I_agentInventoryUnitActivateSTK:
    agentInventoryUnitEntryData.agentInventoryUnitActivateSTK = D_agentInventoryUnitActivateSTK_disable;
    SET_VALID(I_agentInventoryUnitActivateSTK, agentInventoryUnitEntryData.valid);
    if (nominator != -1) break;

  case I_agentInventoryUnitDeleteSTK:
    agentInventoryUnitEntryData.agentInventoryUnitDeleteSTK = D_agentInventoryUnitDeleteSTK_disable;
    SET_VALID(I_agentInventoryUnitDeleteSTK, agentInventoryUnitEntryData.valid);
    if (nominator != -1) break;    
    
  case I_agentInventoryUnitReload:
    agentInventoryUnitEntryData.agentInventoryUnitReload = D_agentInventoryUnitReload_disable;
    SET_VALID(I_agentInventoryUnitReload, agentInventoryUnitEntryData.valid);
    if (nominator != -1) break;

  case I_agentInventoryUnitRowStatus:
    agentInventoryUnitEntryData.agentInventoryUnitRowStatus = D_agentInventoryUnitRowStatus_active;
    SET_VALID(I_agentInventoryUnitRowStatus, agentInventoryUnitEntryData.valid);
    if (nominator != -1) break;

  case I_agentInventoryUnitSerialNumber:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbUnitMgrSerialNumberGet(agentInventoryUnitEntryData.agentInventoryUnitNumber,
                                    snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromText(&agentInventoryUnitEntryData.agentInventoryUnitSerialNumber,
                                    snmp_buffer) == L7_TRUE)
      SET_VALID(I_agentInventoryUnitSerialNumber, agentInventoryUnitEntryData.valid);
    if (nominator != -1) break;

  case I_agentInventoryUnitStandby:
    if (snmpAgentInventoryUnitStandbyGet(agentInventoryUnitEntryData.agentInventoryUnitNumber,
                                         &agentInventoryUnitEntryData.agentInventoryUnitStandby) == L7_SUCCESS)
      SET_VALID(I_agentInventoryUnitStandby, agentInventoryUnitEntryData.valid);
    if (nominator != -1) break;

#ifdef I_agentInventoryUnitReplicateSTKStatus
  case I_agentInventoryUnitReplicateSTKStatus:
    if ( L7_SUCCESS == snmpAgentInventoryUnitReplicateSTKStatusGet(agentInventoryUnitEntryData.agentInventoryUnitNumber,
                                                                   &agentInventoryUnitEntryData.agentInventoryUnitReplicateSTKStatus ) )
    {
      SET_VALID(I_agentInventoryUnitReplicateSTKStatus, agentInventoryUnitEntryData.valid);
    }
    if (nominator != -1) break;
#endif /* I_agentInventoryUnitReplicateSTKStatus */

#ifdef L7_NSF_PACKAGE
#ifdef I_agentNsfUnitSupport
  case I_agentNsfUnitSupport:
    if ( L7_SUCCESS == snmpAgentNsfUnitSupportGet(agentInventoryUnitEntryData.agentInventoryUnitNumber,
                                                  &agentInventoryUnitEntryData.agentNsfUnitSupport ) )
    {
      SET_VALID(I_agentNsfUnitSupport, agentInventoryUnitEntryData.valid);
    }
    if (nominator != -1) break;
#endif /* I_agentInventoryUnitNsfSupport */
#endif /* L7_NSF_PACKAGE */

    break;

case I_agentInventoryUnitSFSTransferStatus:
     if(L7_SUCCESS == snmpAgentInventoryUnitSFSTransferStatusGet(
                      agentInventoryUnitEntryData.agentInventoryUnitNumber,
                      &agentInventoryUnitEntryData.agentInventoryUnitSFSTransferStatus))
     {
        SET_VALID(I_agentInventoryUnitSFSTransferStatus,agentInventoryUnitEntryData.valid);
     } 
     if(nominator != -1) break;

case I_agentInventoryUnitSFSLastAttemptStatus:
     if(L7_SUCCESS == snmpAgentInventoryUnitSFSLastAttemptStatusGet(
                      agentInventoryUnitEntryData.agentInventoryUnitNumber,
                      &agentInventoryUnitEntryData.agentInventoryUnitSFSLastAttemptStatus))
     {
        SET_VALID(I_agentInventoryUnitSFSLastAttemptStatus,agentInventoryUnitEntryData.valid);
     }
     if(nominator != -1) break;

 
  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentInventoryUnitEntryData.valid))
    return(NULL);

  return(&agentInventoryUnitEntryData);
}

#ifdef SETS
int
k_agentInventoryUnitEntry_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentInventoryUnitEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                               doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentInventoryUnitEntry_set_defaults(doList_t *dp)
{
    agentInventoryUnitEntry_t *data = (agentInventoryUnitEntry_t *) (dp->data);

    if ((data->agentInventoryUnitDetectedCodeVer = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->agentInventoryUnitDetectedCodeInFlashVer = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentInventoryUnitEntry_set(agentInventoryUnitEntry_t *data,
                             ContextInfo *contextInfo, int function)
{
  L7_uint32 sfs_status;

  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_char8 auditMsg[SNMP_BUFFER_LEN];

  memset(snmp_buffer, L7_NULL, sizeof(snmp_buffer));
  memset(auditMsg, L7_NULL, sizeof(auditMsg));

  #ifndef L7_STACKING_PACKAGE
  return COMMIT_FAILED_ERROR;
  #endif

  if (snmpAgentInventoryUnitEntryGet(data->agentInventoryUnitNumber) != L7_SUCCESS)
  {
    /* unit doesn't exist, attempt creation... unit type is required for creation */
    if (VALID(I_agentInventoryUnitRowStatus, data->valid) &&
        VALID(I_agentInventoryUnitSupportedUnitIndex, data->valid) &&
        (data->agentInventoryUnitRowStatus == D_agentInventoryUnitRowStatus_createAndGo ||
         data->agentInventoryUnitRowStatus == D_agentInventoryUnitRowStatus_createAndWait) &&
        usmDbUnitMgrCreateUnit(data->agentInventoryUnitNumber, data->agentInventoryUnitSupportedUnitIndex) == L7_SUCCESS)
    {
      /* unit created, clear valid bits and set other parameters */
      CLR_VALID(I_agentInventoryUnitRowStatus, data->valid);
      CLR_VALID(I_agentInventoryUnitSupportedUnitIndex, data->valid);
    }
    else
    {
      return COMMIT_FAILED_ERROR;
    }
  }
  else if (VALID(I_agentInventoryUnitSupportedUnitIndex, data->valid))
  {
    /* Cannot change unit index after unit has been created */
    return COMMIT_FAILED_ERROR;
  }

if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_STACKING_COMPONENT_ID,
    L7_STACKING_SFS_FEATURE_ID) == L7_TRUE)
{

  if(VALID(I_agentInventoryUnitAssignNumber,data->valid))
  {
    if(usmDbUnitMgrStackFirmwareSyncStatusGet(&sfs_status) == L7_SUCCESS && 
       sfs_status == L7_SFS_SWITCH_STATUS_IN_PROGRESS)
    {
      return COMMIT_FAILED_ERROR;
    }
  } 
}


  if (VALID(I_agentInventoryUnitAssignNumber, data->valid) &&
      (data->agentInventoryUnitAssignNumber < 1 ||
       data->agentInventoryUnitAssignNumber > L7_MAX_UNITS_PER_STACK ||
       usmDbUnitMgrUnitNumberChange(data->agentInventoryUnitNumber, 
                                    data->agentInventoryUnitAssignNumber) != L7_SUCCESS))
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentInventoryUnitMgmtAdmin, data->valid) &&
      snmpAgentInventoryUnitMgmtAdminSet(data->agentInventoryUnitNumber, 
                                         data->agentInventoryUnitMgmtAdmin) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentInventoryUnitAdminMgmtPref, data->valid) &&
      snmpAgentInventoryUnitAdminMgmtPrefSet(data->agentInventoryUnitNumber, 
                                             data->agentInventoryUnitAdminMgmtPref) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentInventoryUnitAdminMgmtPrefValue, data->valid) &&
      snmpAgentInventoryUnitAdminMgmtPrefValueSet(data->agentInventoryUnitNumber, 
                                                  data->agentInventoryUnitAdminMgmtPrefValue) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentInventoryUnitDescription, data->valid))
  {
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentInventoryUnitSTKname, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    
    if(data->agentInventoryUnitSTKname == D_agentInventoryUnitSTKname_image1)
    {
      usmDbImageFileNameGet(0, snmp_buffer);
    }
    else
    {
      usmDbImageFileNameGet(1, snmp_buffer);
    }

    if (usmDbTransferFileNameLocalSet(USMDB_UNIT_CURRENT, snmp_buffer) != L7_SUCCESS)
      return COMMIT_FAILED_ERROR;
  }  

  if (VALID(I_agentInventoryUnitReplicateSTK, data->valid) &&
      snmpAgentInventoryUnitReplicateSTKSet(data->agentInventoryUnitNumber, 
                                            data->agentInventoryUnitReplicateSTK) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;


  if (VALID(I_agentInventoryUnitActivateSTK, data->valid) &&
      (data->agentInventoryUnitActivateSTK == D_agentInventoryUnitActivateSTK_enable))
  {
      
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbTransferFileNameLocalGet(USMDB_UNIT_CURRENT, snmp_buffer) != L7_SUCCESS)
      return COMMIT_FAILED_ERROR;

    if (usmDbImageActivate(data->agentInventoryUnitNumber, snmp_buffer,
                           L7_FALSE) != L7_SUCCESS)
      return COMMIT_FAILED_ERROR;        
  }  

  if (VALID(I_agentInventoryUnitDeleteSTK, data->valid) &&
      (data->agentInventoryUnitDeleteSTK == D_agentInventoryUnitDeleteSTK_enable))
  {
      
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    
    if (usmDbTransferFileNameLocalGet(USMDB_UNIT_CURRENT, snmp_buffer) != L7_SUCCESS)
      return COMMIT_FAILED_ERROR;

    if (usmDbFileDelete(data->agentInventoryUnitNumber, snmp_buffer) != L7_SUCCESS)
      return COMMIT_FAILED_ERROR;

    if (data->agentInventoryUnitDeleteSTK == D_agentInventoryUnitDeleteSTK_enable)
    {
      osapiSnprintf(auditMsg, sizeof(auditMsg),
                    pStrInfo_base_Auditing_FileDelete, snmp_buffer);
      SnmpCmdLoggerAuditEntryAdd(contextInfo, auditMsg);
    }
  }  

  
  if (VALID(I_agentInventoryUnitReload, data->valid))
  {
    if ((data->agentInventoryUnitNumber == L7_NULL) || 
        (data->agentInventoryUnitNumber > L7_MAX_UNITS_PER_STACK))
      return COMMIT_FAILED_ERROR;

    if (snmpAgentInventoryUnitReloadSet(data->agentInventoryUnitNumber, 
                                        data->agentInventoryUnitReload) != L7_SUCCESS)
      return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentInventoryUnitRowStatus, data->valid) &&
      snmpAgentInventoryUnitRowStatusSet(data->agentInventoryUnitNumber, 
                                         data->agentInventoryUnitRowStatus) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentInventoryUnitStandby, data->valid) &&
      snmpAgentInventoryUnitStandbySet(data->agentInventoryUnitNumber, 
                                       data->agentInventoryUnitStandby) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  return NO_ERROR;
}

#ifdef SR_agentInventoryUnitEntry_UNDO
/* add #define SR_agentInventoryUnitEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentInventoryUnitEntry family.
 */
int
agentInventoryUnitEntry_undo(doList_t *doHead, doList_t *doCur,
                            ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentInventoryUnitEntry_UNDO */

#endif /* SETS */

agentInventorySlotEntry_t *
k_agentInventorySlotEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_UINT32 agentInventoryUnitNumber,
                             SR_UINT32 agentInventorySlotNumber)
{
  static agentInventorySlotEntry_t agentInventorySlotEntryData;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 snmp_buffer_len = SNMP_BUFFER_LEN;
  static L7_BOOL firstTime = L7_TRUE;

  if (firstTime == L7_TRUE)
  {
    agentInventorySlotEntryData.agentInventorySlotCapabilities = MakeOctetString(NULL,0);
    firstTime = L7_FALSE;
  }
  
  ZERO_VALID(agentInventorySlotEntryData.valid);
  
  agentInventorySlotEntryData.agentInventoryUnitNumber = agentInventoryUnitNumber;
  SET_VALID(I_agentInventorySlotEntryIndex_agentInventoryUnitNumber, agentInventorySlotEntryData.valid);
  
  agentInventorySlotEntryData.agentInventorySlotNumber = agentInventorySlotNumber;
  SET_VALID(I_agentInventorySlotNumber, agentInventorySlotEntryData.valid);
  
  if ((searchType == EXACT) ?
      (snmpAgentInventorySlotEntryGet(agentInventorySlotEntryData.agentInventoryUnitNumber, 
                                      agentInventorySlotEntryData.agentInventorySlotNumber) != L7_SUCCESS) :
      ((snmpAgentInventorySlotEntryGet(agentInventorySlotEntryData.agentInventoryUnitNumber, 
                                       agentInventorySlotEntryData.agentInventorySlotNumber) != L7_SUCCESS) &&
       (snmpAgentInventorySlotEntryNextGet(&agentInventorySlotEntryData.agentInventoryUnitNumber, 
                                           &agentInventorySlotEntryData.agentInventorySlotNumber) != L7_SUCCESS)))
    return(NULL);
  
  switch (nominator)
  {
  case -1:
  case I_agentInventorySlotEntryIndex_agentInventoryUnitNumber:
  case I_agentInventorySlotNumber:
    if (nominator != -1) break;

  case I_agentInventorySlotStatus:
    if (snmpAgentInventorySlotStatusGet(agentInventorySlotEntryData.agentInventoryUnitNumber,
                                        agentInventorySlotEntryData.agentInventorySlotNumber,
                                        &agentInventorySlotEntryData.agentInventorySlotStatus) == L7_SUCCESS)
      SET_VALID(I_agentInventorySlotStatus, agentInventorySlotEntryData.valid);
    if (nominator != -1) break;

  case I_agentInventorySlotPowerMode:
    if (snmpAgentInventorySlotPowerModeGet(agentInventorySlotEntryData.agentInventoryUnitNumber,
                                           agentInventorySlotEntryData.agentInventorySlotNumber,
                                           &agentInventorySlotEntryData.agentInventorySlotPowerMode) == L7_SUCCESS)
      SET_VALID(I_agentInventorySlotPowerMode, agentInventorySlotEntryData.valid);
    if (nominator != -1) break;

  case I_agentInventorySlotAdminMode:
    if (snmpAgentInventorySlotAdminModeGet(agentInventorySlotEntryData.agentInventoryUnitNumber,
                                           agentInventorySlotEntryData.agentInventorySlotNumber,
                                           &agentInventorySlotEntryData.agentInventorySlotAdminMode) == L7_SUCCESS)
      SET_VALID(I_agentInventorySlotAdminMode, agentInventorySlotEntryData.valid);
    if (nominator != -1) break;

  case I_agentInventorySlotInsertedCardType:
    if (usmDbCardInsertedCardTypeGet(agentInventorySlotEntryData.agentInventoryUnitNumber,
                                     agentInventorySlotEntryData.agentInventorySlotNumber,
                                     &agentInventorySlotEntryData.agentInventorySlotInsertedCardType) == L7_SUCCESS)
      SET_VALID(I_agentInventorySlotInsertedCardType, agentInventorySlotEntryData.valid);
    if (nominator != -1) break;

  case I_agentInventorySlotConfiguredCardType:
    if (usmDbCardConfiguredCardTypeGet(agentInventorySlotEntryData.agentInventoryUnitNumber,
                                       agentInventorySlotEntryData.agentInventorySlotNumber,
                                       &agentInventorySlotEntryData.agentInventorySlotConfiguredCardType) == L7_SUCCESS)
      SET_VALID(I_agentInventorySlotConfiguredCardType, agentInventorySlotEntryData.valid);
    if (nominator != -1) break;

  case I_agentInventorySlotCapabilities:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (snmpAgentInventorySlotCapabilitiesGet(agentInventorySlotEntryData.agentInventoryUnitNumber,
                                              agentInventorySlotEntryData.agentInventorySlotNumber,
                                              snmp_buffer, &snmp_buffer_len) == L7_SUCCESS &&
        SafeMakeOctetString(&agentInventorySlotEntryData.agentInventorySlotCapabilities, snmp_buffer, snmp_buffer_len) == L7_TRUE)
      SET_VALID(I_agentInventorySlotCapabilities, agentInventorySlotEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }


  if (nominator >= 0 && !VALID(nominator, agentInventorySlotEntryData.valid))
    return(NULL);
  return(&agentInventorySlotEntryData);
}

#ifdef SETS
int
k_agentInventorySlotEntry_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentInventorySlotEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                               doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentInventorySlotEntry_set_defaults(doList_t *dp)
{
    agentInventorySlotEntry_t *data = (agentInventorySlotEntry_t *) (dp->data);

    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentInventorySlotEntry_set(agentInventorySlotEntry_t *data,
                             ContextInfo *contextInfo, int function)
{

   return COMMIT_FAILED_ERROR;
}

#ifdef SR_agentInventorySlotEntry_UNDO
/* add #define SR_agentInventorySlotEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentInventorySlotEntry family.
 */
int
agentInventorySlotEntry_undo(doList_t *doHead, doList_t *doCur,
                            ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentInventorySlotEntry_UNDO */

#endif /* SETS */

agentInventoryCardTypeEntry_t *
k_agentInventoryCardTypeEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_UINT32 agentInventoryCardIndex)
{
  static agentInventoryCardTypeEntry_t agentInventoryCardTypeEntryData;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  static L7_BOOL firstTime = L7_TRUE;

  if (firstTime == L7_TRUE)
  {
    agentInventoryCardTypeEntryData.agentInventoryCardModelIdentifier = MakeOctetString(NULL, 0);
    agentInventoryCardTypeEntryData.agentInventoryCardDescription = MakeOctetString(NULL, 0);
    firstTime = L7_FALSE;
  }

  ZERO_VALID(agentInventoryCardTypeEntryData.valid);
  agentInventoryCardTypeEntryData.agentInventoryCardIndex = agentInventoryCardIndex;
  SET_VALID(I_agentInventoryCardIndex, agentInventoryCardTypeEntryData.valid);

  if ((searchType == EXACT) ?
      (usmDbCardSupportedIndexCheck(agentInventoryCardTypeEntryData.agentInventoryCardIndex) != L7_SUCCESS) :
      ((usmDbCardSupportedIndexCheck(agentInventoryCardTypeEntryData.agentInventoryCardIndex) != L7_SUCCESS) &&
       (usmDbCardSupportedIndexNextGet(&agentInventoryCardTypeEntryData.agentInventoryCardIndex) != L7_SUCCESS)))
  {
    return (NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_agentInventoryCardIndex:
    if (nominator != -1) break;

  case I_agentInventoryCardType:
    if (usmDbCardSupportedTypeIdGet(agentInventoryCardTypeEntryData.agentInventoryCardIndex,
                                    &agentInventoryCardTypeEntryData.agentInventoryCardType) == L7_SUCCESS)
      SET_VALID(I_agentInventoryCardType, agentInventoryCardTypeEntryData.valid);
    if (nominator != -1) break;

  case I_agentInventoryCardModelIdentifier:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbCardSupportedCardModelIdGet(agentInventoryCardTypeEntryData.agentInventoryCardIndex,
                                         snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromTextExact(&agentInventoryCardTypeEntryData.agentInventoryCardModelIdentifier, snmp_buffer) == L7_TRUE)
      SET_VALID(I_agentInventoryCardModelIdentifier, agentInventoryCardTypeEntryData.valid);
    if (nominator != -1) break;
  
  case I_agentInventoryCardDescription:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbCardSupportedCardDescriptionGet(agentInventoryCardTypeEntryData.agentInventoryCardIndex,
                                             snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromTextExact(&agentInventoryCardTypeEntryData.agentInventoryCardDescription, snmp_buffer) == L7_TRUE)
      SET_VALID(I_agentInventoryCardDescription, agentInventoryCardTypeEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentInventoryCardTypeEntryData.valid))
    return(NULL);

  return(&agentInventoryCardTypeEntryData);
}

#ifdef SETS
int
k_agentInventoryCardTypeEntry_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentInventoryCardTypeEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                               doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentInventoryCardTypeEntry_set_defaults(doList_t *dp)
{
    agentInventoryCardTypeEntry_t *data = (agentInventoryCardTypeEntry_t *) (dp->data);

    if ((data->agentInventoryCardModelIdentifier = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->agentInventoryCardDescription = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentInventoryCardTypeEntry_set(agentInventoryCardTypeEntry_t *data,
                             ContextInfo *contextInfo, int function)
{

   return COMMIT_FAILED_ERROR;
}

#ifdef SR_agentInventoryCardTypeEntry_UNDO
/* add #define SR_agentInventoryCardTypeEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentInventoryCardTypeEntry family.
 */
int
agentInventoryCardTypeEntry_undo(doList_t *doHead, doList_t *doCur,
                            ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentInventoryCardTypeEntry_UNDO */

#endif /* SETS */

agentInventoryComponentEntry_t *
k_agentInventoryComponentEntry_get(int serialNum, ContextInfo *contextInfo,
                                   int nominator,
                                   int searchType,
                                   SR_UINT32 agentInventoryComponentIndex)
{
  static agentInventoryComponentEntry_t agentInventoryComponentEntryData;
  L7_uint32 componentId;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  
  ZERO_VALID(agentInventoryComponentEntryData.valid);
     
  agentInventoryComponentEntryData.agentInventoryComponentIndex = agentInventoryComponentIndex;
  SET_VALID(I_agentInventoryComponentIndex, agentInventoryComponentEntryData.valid);
  
  if ((searchType == EXACT) ?
     (usmDbComponentIndexGet(agentInventoryComponentEntryData.agentInventoryComponentIndex, 
                             &componentId) != L7_SUCCESS) :
     ((usmDbComponentIndexGet(agentInventoryComponentEntryData.agentInventoryComponentIndex, 
                              &componentId) != L7_SUCCESS) &&
      (usmDbComponentIndexNextGet(&agentInventoryComponentEntryData.agentInventoryComponentIndex, 
                                  &componentId) != L7_SUCCESS)))
    return(NULL);
  
  switch (nominator)
  {
  case I_agentInventoryComponentIndex:
    if (nominator != -1) break;

  case I_agentInventoryComponentMnemonic:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbComponentMnemonicGet(componentId, snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromTextExact(&agentInventoryComponentEntryData.agentInventoryComponentMnemonic,
                                         snmp_buffer) == L7_TRUE)
      SET_VALID(I_agentInventoryComponentMnemonic, agentInventoryComponentEntryData.valid);
    if (nominator != -1) break;
      
  case I_agentInventoryComponentName:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbComponentNameGet(componentId, snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromTextExact(&agentInventoryComponentEntryData.agentInventoryComponentName,
                                         snmp_buffer) == L7_TRUE)
      SET_VALID(I_agentInventoryComponentName, agentInventoryComponentEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator != 0 && !VALID(nominator, agentInventoryComponentEntryData.valid))
    return(NULL);

  return(&agentInventoryComponentEntryData);
}

agentInventoryStackPortGroup_t *
k_agentInventoryStackPortGroup_get(int serialNum, ContextInfo *contextInfo,
                                   int nominator)
{
  static agentInventoryStackPortGroup_t agentInventoryStackPortGroupData;

  switch (nominator)
  {
  case -1:
  case I_agentInventoryStackPortIpTelephonyQOSSupport:
    if (snmpAgentInventoryStackPortIpTelephonyQOSSupportGet(&agentInventoryStackPortGroupData.agentInventoryStackPortIpTelephonyQOSSupport) == L7_SUCCESS)
      SET_VALID(I_agentInventoryStackPortIpTelephonyQOSSupport, agentInventoryStackPortGroupData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentInventoryStackPortGroupData.valid))
    return(NULL);

  return(&agentInventoryStackPortGroupData);
}

#ifdef SETS
int
k_agentInventoryStackPortGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                    doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentInventoryStackPortGroup_ready(ObjectInfo *object, ObjectSyntax *value, 
                                     doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentInventoryStackPortGroup_set(agentInventoryStackPortGroup_t *data,
                                   ContextInfo *contextInfo, int function)
{
  if (VALID(I_agentInventoryStackPortIpTelephonyQOSSupport, data->valid) &&
      snmpAgentInventoryStackPortIpTelephonyQOSSupportSet(data->agentInventoryStackPortIpTelephonyQOSSupport) != L7_SUCCESS)
  {
    return COMMIT_FAILED_ERROR;
  }

  return NO_ERROR;
}

#ifdef SR_agentInventoryStackPortGroup_UNDO
/* add #define SR_agentInventoryStackPortGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentInventoryStackPortGroup family.
 */
int
agentInventoryStackPortGroup_undo(doList_t *doHead, doList_t *doCur,
                                  ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentInventoryStackPortGroup_UNDO */

#endif /* SETS */

agentInventoryStackPortEntry_t *
k_agentInventoryStackPortEntry_get(int serialNum, ContextInfo *contextInfo,
                                   int nominator,
                                   int searchType,
                                   SR_UINT32 agentInventoryStackPortIndex)
{
  static agentInventoryStackPortEntry_t agentInventoryStackPortEntryData;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  ZERO_VALID(agentInventoryStackPortEntryData.valid);
  agentInventoryStackPortEntryData.agentInventoryStackPortIndex = agentInventoryStackPortIndex;
  SET_VALID(I_agentInventoryStackPortIndex, agentInventoryStackPortEntryData.valid);

  if ((searchType == EXACT) ?
      (snmpAgentInventoryStackPortEntryGet(agentInventoryStackPortEntryData.agentInventoryStackPortIndex) != L7_SUCCESS) :
      (snmpAgentInventoryStackPortEntryGet(agentInventoryStackPortEntryData.agentInventoryStackPortIndex) != L7_SUCCESS &&
       snmpAgentInventoryStackPortEntryNextGet(&agentInventoryStackPortEntryData.agentInventoryStackPortIndex) != L7_SUCCESS))
  {
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_agentInventoryStackPortIndex:
    if (nominator != -1) break;

  case I_agentInventoryStackPortUnit:
    if (snmpAgentInventoryStackPortUnitGet(agentInventoryStackPortEntryData.agentInventoryStackPortIndex,
                                           &agentInventoryStackPortEntryData.agentInventoryStackPortUnit) == L7_SUCCESS)
    {
      SET_VALID(I_agentInventoryStackPortUnit, agentInventoryStackPortEntryData.valid);
    }
    if (nominator != -1) break;

  case I_agentInventoryStackPortTag:
    memset(snmp_buffer, 0, SNMP_BUFFER_LEN);

    if (snmpAgentInventoryStackPortTagGet(agentInventoryStackPortEntryData.agentInventoryStackPortIndex,
                                          snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromTextExact(&agentInventoryStackPortEntryData.agentInventoryStackPortTag,
                                         snmp_buffer) == L7_TRUE)
    {
      SET_VALID(I_agentInventoryStackPortTag, agentInventoryStackPortEntryData.valid);
    }
    if (nominator != -1) break;

  case I_agentInventoryStackPortConfiguredStackMode:
    if (snmpAgentInventoryStackPortConfiguredStackModeGet(agentInventoryStackPortEntryData.agentInventoryStackPortIndex,
                                                          &agentInventoryStackPortEntryData.agentInventoryStackPortConfiguredStackMode) == L7_SUCCESS)
    {
      SET_VALID(I_agentInventoryStackPortConfiguredStackMode, agentInventoryStackPortEntryData.valid);
    }
    if (nominator != -1) break;

  case I_agentInventoryStackPortRunningStackMode:
    if (snmpAgentInventoryStackPortRunningStackModeGet(agentInventoryStackPortEntryData.agentInventoryStackPortIndex,
                                                       &agentInventoryStackPortEntryData.agentInventoryStackPortRunningStackMode) == L7_SUCCESS)
    {
      SET_VALID(I_agentInventoryStackPortRunningStackMode, agentInventoryStackPortEntryData.valid);
    }
    if (nominator != -1) break;

  case I_agentInventoryStackPortLinkStatus:
    if (snmpAgentInventoryStackPortLinkStatusGet(agentInventoryStackPortEntryData.agentInventoryStackPortIndex,
                                                 &agentInventoryStackPortEntryData.agentInventoryStackPortLinkStatus) == L7_SUCCESS)
    {
      SET_VALID(I_agentInventoryStackPortLinkStatus, agentInventoryStackPortEntryData.valid);
    }
    if (nominator != -1) break;

  case I_agentInventoryStackPortLinkSpeed:
    if (snmpAgentInventoryStackPortLinkSpeedGet(agentInventoryStackPortEntryData.agentInventoryStackPortIndex,
                                                &agentInventoryStackPortEntryData.agentInventoryStackPortLinkSpeed) == L7_SUCCESS)
    {
      SET_VALID(I_agentInventoryStackPortLinkSpeed, agentInventoryStackPortEntryData.valid);
    }
    if (nominator != -1) break;

  case I_agentInventoryStackPortDataRate:
    if (snmpAgentInventoryStackPortDataRateGet(agentInventoryStackPortEntryData.agentInventoryStackPortIndex,
                                               &agentInventoryStackPortEntryData.agentInventoryStackPortDataRate) == L7_SUCCESS)
    {
      SET_VALID(I_agentInventoryStackPortDataRate, agentInventoryStackPortEntryData.valid);
    }
    if (nominator != -1) break;

  case I_agentInventoryStackPortErrorRate:
    if (snmpAgentInventoryStackPortErrorRateGet(agentInventoryStackPortEntryData.agentInventoryStackPortIndex,
                                                &agentInventoryStackPortEntryData.agentInventoryStackPortErrorRate) == L7_SUCCESS)
    {
      SET_VALID(I_agentInventoryStackPortErrorRate, agentInventoryStackPortEntryData.valid);
    }
    if (nominator != -1) break;

  case I_agentInventoryStackPortTotalErrors:
    if (snmpAgentInventoryStackPortTotalErrorsGet(agentInventoryStackPortEntryData.agentInventoryStackPortIndex,
                                                  &agentInventoryStackPortEntryData.agentInventoryStackPortTotalErrors) == L7_SUCCESS)
    {
      SET_VALID(I_agentInventoryStackPortTotalErrors, agentInventoryStackPortEntryData.valid);
    }
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentInventoryStackPortEntryData.valid))
    return(NULL);

  return(&agentInventoryStackPortEntryData);
}

#ifdef SETS
int
k_agentInventoryStackPortEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                    doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentInventoryStackPortEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                     doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentInventoryStackPortEntry_set_defaults(doList_t *dp)
{
    agentInventoryStackPortEntry_t *data = (agentInventoryStackPortEntry_t *) (dp->data);

    if ((data->agentInventoryStackPortTag = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->agentInventoryStackPortDataRate = (SR_UINT32) 0;
    data->agentInventoryStackPortErrorRate = (SR_UINT32) 0;
    data->agentInventoryStackPortTotalErrors = (SR_UINT32) 0;

    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentInventoryStackPortEntry_set(agentInventoryStackPortEntry_t *data,
                                   ContextInfo *contextInfo, int function)
{
  if (VALID(I_agentInventoryStackPortConfiguredStackMode, data->valid) &&
      snmpAgentInventoryStackPortConfiguredStackModeSet(data->agentInventoryStackPortIndex,
                                                        data->agentInventoryStackPortConfiguredStackMode) != L7_SUCCESS)
  {
    return COMMIT_FAILED_ERROR;
  }

  return NO_ERROR;
}

#ifdef SR_agentInventoryStackPortEntry_UNDO
/* add #define SR_agentInventoryStackPortEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentInventoryStackPortEntry family.
 */
int
agentInventoryStackPortEntry_undo(doList_t *doHead, doList_t *doCur,
                                  ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentInventoryStackPortEntry_UNDO */

#endif /* SETS */
