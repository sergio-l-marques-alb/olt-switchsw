/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* Name: k_mib_fastpathswitghing.c
*
* Purpose: System-specific code for the fastPathNPNP 201 MIB
*
* Created by: Colin Verne, Soma 04/25/2001
*
* Component:
*
*********************************************************************
*********************************************************************
*
********************************************************************/

#include "snmpapi.h"
#include "k_private_base.h"
#include <inst_lib.h>
#include "k_mib_fastpathswitching_api.h"
#ifdef L7_DVLAN_PACKAGE
#include "k_mib_fastpathdvlan_api.h"
#endif
#include "snmp_util_api.h"
#include "dot3ad_api.h"
#include "user_mgr_apl.h"
#include "comm_mask.h"
#include "usmdb_dim_api.h"
#include "dot1q_exports.h"
#include "dot1s_exports.h"
#include "nim_exports.h"
#include "fdb_exports.h"
#include "snmp_exports.h"
#include "sim_exports.h"
#include "snooping_exports.h"
#include "user_manager_exports.h"
#include "usmdb_1213_api.h"
#include "usmdb_common.h"
#include "usmdb_dai_api.h"
#include "usmdb_dhcp_snooping.h"
#include "usmdb_dot1s_api.h"
#include "usmdb_dot3ad_api.h"
#include "usmdb_dvlantag_api.h"
#include "usmdb_ip_base_api.h"
#include "usmdb_mfdb_api.h"
#include "usmdb_mib_bridge_api.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_mirror_api.h"
#include "usmdb_registry_api.h"
#include "usmdb_sim_api.h"
#include "usmdb_snmp_api.h"
#include "usmdb_snooping_api.h"
#include "usmdb_switch_cpu_api.h"
#include "usmdb_trapmgr_api.h"
#include "usmdb_user_mgmt_api.h"
#include "usmdb_unitmgr_api.h"
#include "usmdb_util_api.h"
#ifdef L7_MGMT_SECURITY_PACKAGE
#include "usmdb_sshc_api.h"
#endif /* L7_MGMT_SECURITY_PACKAGE */
#include "usmdb_link_dependency_api.h"
#ifdef L7_AUTO_INSTALL_PACKAGE
#include "usmdb_auto_install_api.h"
#endif /* L7_AUTO_INSTALL_PACKAGE */
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
#include "usmdb_dhcp6c_api.h"
#endif

/*static int voiceVlanDeviceDebugFlag= L7_FALSE; */

void voiceVlanDeviceDebugFlagSet(int temp)
{
  voiceVlanDeviceDebugFlag= temp;
}

/* Definitions for getting the index into tables which allow repeated duplicate names in their rows */
L7_uint32 lagTempIntIfNum = 0;
L7_uint32 *lagIntIfNum = &lagTempIntIfNum;
L7_uint32 snmpTempTrapReceiverIndex = 0;
L7_uint32 *snmpTrapReceiverIndex = &snmpTempTrapReceiverIndex;

/*
 * The following definitions help in avoiding deletion of an existing row when a new row is created
 * with the same name as the existing one.
*/
L7_BOOL creationErrorAuthenticationGroup = L7_FALSE;
L7_BOOL creationErrorLagConfigGroup = L7_FALSE;
L7_BOOL creationErrorUserConfigGroup = L7_FALSE;
L7_BOOL creationErrorSnmpCommunity = L7_FALSE;
L7_BOOL creationErrorSnmpTrapReceiver = L7_FALSE;

agentInventoryGroup_t *
k_agentInventoryGroup_get(int serialNum, ContextInfo *contextInfo,
                          int nominator)
{
  static agentInventoryGroup_t agentInventoryGroupData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;

    agentInventoryGroupData.agentInventoryManufacturer = MakeOctetString(NULL, 0);
    agentInventoryGroupData.agentInventoryMaintenanceLevel = MakeOctetString(NULL, 0);
    agentInventoryGroupData.agentInventoryPartNumber = MakeOctetString(NULL, 0);
    agentInventoryGroupData.agentInventoryFRUNumber = MakeOctetString(NULL, 0);
    agentInventoryGroupData.agentInventorySerialNumber = MakeOctetString(NULL, 0);
    agentInventoryGroupData.agentInventoryMachineModel = MakeOctetString(NULL, 0);
    agentInventoryGroupData.agentInventoryMachineType = MakeOctetString(NULL, 0);
    agentInventoryGroupData.agentInventorySysDescription = MakeOctetString(NULL, 0);
    agentInventoryGroupData.agentInventoryBurnedInMacAddress = MakeOctetString(NULL, 0);
    agentInventoryGroupData.agentInventoryOperatingSystem = MakeOctetString(NULL, 0);
    agentInventoryGroupData.agentInventoryNetworkProcessingDevice = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(agentInventoryGroupData.valid);

  switch (nominator)
  {
  case -1:
    break;

  case I_agentInventorySysDescription:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDb1213SysDescrGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentInventoryGroupData.agentInventorySysDescription, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentInventorySysDescription, agentInventoryGroupData.valid);
    break;

  case I_agentInventoryMachineType:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbMachineTypeGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentInventoryGroupData.agentInventoryMachineType, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentInventoryMachineType, agentInventoryGroupData.valid);
    break;

  case I_agentInventoryMachineModel:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbMachineModelGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentInventoryGroupData.agentInventoryMachineModel, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentInventoryMachineModel, agentInventoryGroupData.valid);
    break;

  case I_agentInventorySerialNumber:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbSwDevInfoSerialNumGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentInventoryGroupData.agentInventorySerialNumber, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentInventorySerialNumber, agentInventoryGroupData.valid);
    break;

  case I_agentInventoryFRUNumber:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbFRUNumGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentInventoryGroupData.agentInventoryFRUNumber, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentInventoryFRUNumber, agentInventoryGroupData.valid);
    break;

  case I_agentInventoryPartNumber:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbPartNumGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentInventoryGroupData.agentInventoryPartNumber, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentInventoryPartNumber, agentInventoryGroupData.valid);
    break;

  case I_agentInventoryMaintenanceLevel:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbMaintLevelGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentInventoryGroupData.agentInventoryMaintenanceLevel, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentInventoryMaintenanceLevel, agentInventoryGroupData.valid);
    break;

  case I_agentInventoryManufacturer:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbManufacturerGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentInventoryGroupData.agentInventoryManufacturer, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentInventoryManufacturer, agentInventoryGroupData.valid);
    break;

  case I_agentInventoryBurnedInMacAddress:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbSwDevCtrlBurnedInMacAddrGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetString(&agentInventoryGroupData.agentInventoryBurnedInMacAddress, snmp_buffer, L7_MAC_ADDR_LEN) == L7_TRUE))
      SET_VALID(I_agentInventoryBurnedInMacAddress, agentInventoryGroupData.valid);
    break;

  case I_agentInventoryOperatingSystem:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbOperSysGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentInventoryGroupData.agentInventoryOperatingSystem, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentInventoryOperatingSystem, agentInventoryGroupData.valid);
    break;

  case I_agentInventoryNetworkProcessingDevice:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbNPDGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentInventoryGroupData.agentInventoryNetworkProcessingDevice, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentInventoryNetworkProcessingDevice, agentInventoryGroupData.valid);
    break;

  case I_agentInventoryAdditionalPackages:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (snmpAgentInventoryAdditionalPackagesGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentInventoryGroupData.agentInventoryAdditionalPackages, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentInventoryAdditionalPackages, agentInventoryGroupData.valid);
    break;

  case I_agentInventorySoftwareVersion:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbSwVersionGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentInventoryGroupData.agentInventorySoftwareVersion, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentInventorySoftwareVersion, agentInventoryGroupData.valid);
    break;

#ifdef I_agentInventoryHardwareVersion
  case I_agentInventoryHardwareVersion:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbHwVersionGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentInventoryGroupData.agentInventoryHardwareVersion, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentInventoryHardwareVersion, agentInventoryGroupData.valid);
    break;
#endif

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentInventoryGroupData.valid) )
    return(NULL);

  return(&agentInventoryGroupData);
}

agentTrapLogGroup_t *
k_agentTrapLogGroup_get(int serialNum, ContextInfo *contextInfo,
                        int nominator)
{
  static agentTrapLogGroup_t agentTrapLogGroupData;

  ZERO_VALID(agentTrapLogGroupData.valid);

  switch (nominator)
  {
  case -1:
    break;

  case I_agentTrapLogTotal :
    CLR_VALID(I_agentTrapLogTotal, agentTrapLogGroupData.valid);
    if (usmDbInfoTrapsSinceLastResetSwGet(USMDB_UNIT_CURRENT,
                                          &agentTrapLogGroupData.agentTrapLogTotal) == L7_SUCCESS)
      SET_VALID(I_agentTrapLogTotal, agentTrapLogGroupData.valid);
    break;

  case I_agentTrapLogTotalSinceLastViewed :
    CLR_VALID(I_agentTrapLogTotalSinceLastViewed, agentTrapLogGroupData.valid);
    if (usmDbTrapNumUnDisplayedTrapEntriesGet(USMDB_UNIT_CURRENT,
                                              &agentTrapLogGroupData.agentTrapLogTotalSinceLastViewed) == L7_SUCCESS)
      SET_VALID(I_agentTrapLogTotalSinceLastViewed, agentTrapLogGroupData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentTrapLogGroupData.valid))
    return(NULL);

  return(&agentTrapLogGroupData);
}

agentTrapLogEntry_t *
k_agentTrapLogEntry_get(int serialNum, ContextInfo *contextInfo,
                        int nominator,
                        int searchType,
                        SR_INT32 agentTrapLogIndex)
{
  static agentTrapLogEntry_t agentTrapLogEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;

    agentTrapLogEntryData.agentTrapLogSystemTime = MakeOctetString(NULL, 0);
    agentTrapLogEntryData.agentTrapLogTrap = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(agentTrapLogEntryData.valid);
  agentTrapLogEntryData.agentTrapLogIndex = agentTrapLogIndex;
  SET_VALID(I_agentTrapLogIndex, agentTrapLogEntryData.valid);

  if ( (searchType == EXACT) ?
       (snmpAgentTrapLogEntryGet(USMDB_UNIT_CURRENT, agentTrapLogEntryData.agentTrapLogIndex) != L7_SUCCESS) :
       ( (snmpAgentTrapLogEntryGet(USMDB_UNIT_CURRENT, agentTrapLogEntryData.agentTrapLogIndex) != L7_SUCCESS) &&
         (snmpAgentTrapLogEntryNextGet(USMDB_UNIT_CURRENT, &agentTrapLogEntryData.agentTrapLogIndex) != L7_SUCCESS) ) )
  {
    ZERO_VALID(agentTrapLogEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {

  case -1:
  case I_agentTrapLogIndex:
    break;

  case I_agentTrapLogSystemTime :
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (snmpAgentTrapLogSystemTimeGet(USMDB_UNIT_CURRENT, agentTrapLogEntryData.agentTrapLogIndex, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentTrapLogEntryData.agentTrapLogSystemTime, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentTrapLogSystemTime, agentTrapLogEntryData.valid);
    break;

  case I_agentTrapLogTrap :
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (snmpAgentTrapLogLogTrapGet(USMDB_UNIT_CURRENT, agentTrapLogEntryData.agentTrapLogIndex, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentTrapLogEntryData.agentTrapLogTrap, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentTrapLogTrap, agentTrapLogEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentTrapLogEntryData.valid) )
    return(NULL);

  return(&agentTrapLogEntryData);
}

agentSupportedMibEntry_t *
k_agentSupportedMibEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_INT32 agentSupportedMibIndex)
{
  static agentSupportedMibEntry_t agentSupportedMibEntryData;

  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;

    agentSupportedMibEntryData.agentSupportedMibName = MakeOctetString(NULL, 0);
    agentSupportedMibEntryData.agentSupportedMibDescription = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(agentSupportedMibEntryData.valid);

  agentSupportedMibEntryData.agentSupportedMibIndex = agentSupportedMibIndex;
  SET_VALID(I_agentSupportedMibIndex, agentSupportedMibEntryData.valid);

  if ( (searchType == EXACT) ?
       (SnmpSupportedMibTableEntryGet(agentSupportedMibEntryData.agentSupportedMibIndex) != L7_SUCCESS) :
       ( (SnmpSupportedMibTableEntryGet(agentSupportedMibEntryData.agentSupportedMibIndex) != L7_SUCCESS) &&
         (SnmpSupportedMibTableEntryNextGet(&agentSupportedMibEntryData.agentSupportedMibIndex) != L7_SUCCESS) ) )
  {
    ZERO_VALID(agentSupportedMibEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_agentSupportedMibIndex:
    break;

  case I_agentSupportedMibName:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (SnmpSupportedMibTableMibNameGet(agentSupportedMibEntryData.agentSupportedMibIndex, snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromTextExact(&agentSupportedMibEntryData.agentSupportedMibName, snmp_buffer) == L7_TRUE)
      SET_VALID(I_agentSupportedMibName, agentSupportedMibEntryData.valid);
    break;

  case I_agentSupportedMibDescription:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (SnmpSupportedMibTableMibDescriptionGet(agentSupportedMibEntryData.agentSupportedMibIndex, snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromTextExact(&agentSupportedMibEntryData.agentSupportedMibDescription, snmp_buffer) == L7_TRUE)
      SET_VALID(I_agentSupportedMibDescription, agentSupportedMibEntryData.valid);
    break;

  default:
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentSupportedMibEntryData.valid))
    return(NULL);

  return(&agentSupportedMibEntryData);
}

/*******************************************************************/
#ifdef I_agentSwitchCpuProcessMemFree
agentSwitchCpuProcessGroup_t *
k_agentSwitchCpuProcessGroup_get(int serialNum, ContextInfo *contextInfo,
                                 int nominator)
{
   static agentSwitchCpuProcessGroup_t agentSwitchCpuProcessGroupData;
   ZERO_VALID(agentSwitchCpuProcessGroupData.valid);

   /*
    * put your code to retrieve the information here
    */
  switch (nominator)
  {
  case -1:
    break;

  case I_agentSwitchCpuProcessMemFree:
       CLR_VALID(I_agentSwitchCpuProcessMemFree, agentSwitchCpuProcessGroupData.valid);
       if (usmDbInfoSwitchCpuProcessMemFree(USMDB_UNIT_CURRENT,
          &agentSwitchCpuProcessGroupData.agentSwitchCpuProcessMemFree) == L7_SUCCESS)
          SET_VALID(I_agentSwitchCpuProcessMemFree, agentSwitchCpuProcessGroupData.valid);
          break;

  case I_agentSwitchCpuProcessMemAvailable:
       CLR_VALID(I_agentSwitchCpuProcessMemAvailable, agentSwitchCpuProcessGroupData.valid);
       if (usmDbInfoSwitchCpuProcessMemAvailable(USMDB_UNIT_CURRENT,
          &agentSwitchCpuProcessGroupData.agentSwitchCpuProcessMemAvailable) == L7_SUCCESS)
          SET_VALID(I_agentSwitchCpuProcessMemAvailable, agentSwitchCpuProcessGroupData.valid);
          break;
  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentSwitchCpuProcessGroupData.valid))
    return(NULL);

   return(&agentSwitchCpuProcessGroupData);
}
#endif /* I_agentSwitchCpuProcessMemFree */

#ifdef I_agentSwitchCpuProcessIndex
agentSwitchCpuProcessEntry_t *
k_agentSwitchCpuProcessEntry_get(int serialNum, ContextInfo *contextInfo,
                                 int nominator,
                                 int searchType,
                                 SR_INT32 agentSwitchCpuProcessIndex)
{
   static agentSwitchCpuProcessEntry_t agentSwitchCpuProcessEntryData;
   static L7_BOOL firstTime = L7_TRUE;
   L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

   /*
    * put your code to retrieve the information here
    */
   if (firstTime == L7_TRUE)
   {
     firstTime = L7_FALSE;

     agentSwitchCpuProcessEntryData.agentSwitchCpuProcessName = MakeOctetString(NULL, 0);
     agentSwitchCpuProcessEntryData.agentSwitchCpuProcessPercentageUtilization = MakeOctetString(NULL, 0);
   }

   ZERO_VALID(agentSwitchCpuProcessEntryData.valid);
   agentSwitchCpuProcessEntryData.agentSwitchCpuProcessIndex = agentSwitchCpuProcessIndex;
   SET_VALID(I_agentSwitchCpuProcessIndex ,agentSwitchCpuProcessEntryData.valid);


  if ( (searchType == EXACT) )
  {
      if ( snmpAgentSwitchCpuProcessEntryGet(USMDB_UNIT_CURRENT, agentSwitchCpuProcessEntryData.agentSwitchCpuProcessIndex) != L7_SUCCESS )
      {
          ZERO_VALID(agentSwitchCpuProcessEntryData.valid);
          return(NULL);
      }
  }
  else
  {
      if (snmpAgentSwitchCpuProcessEntryGet(USMDB_UNIT_CURRENT, agentSwitchCpuProcessEntryData.agentSwitchCpuProcessIndex) != L7_SUCCESS)
      {
          if (snmpAgentSwitchCpuProcessEntryNextGet(USMDB_UNIT_CURRENT, &agentSwitchCpuProcessEntryData.agentSwitchCpuProcessIndex) != L7_SUCCESS)
          {
              ZERO_VALID(agentSwitchCpuProcessEntryData.valid);
              return(NULL);
          }
      }
  }
  switch (nominator)
  {

  case -1:
  case I_agentSwitchCpuProcessIndex:
    break;

  case I_agentSwitchCpuProcessName:
      bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (snmpAgentSwitchCpuProcessNameGet(USMDB_UNIT_CURRENT, agentSwitchCpuProcessEntryData.agentSwitchCpuProcessIndex, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentSwitchCpuProcessEntryData.agentSwitchCpuProcessName, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentSwitchCpuProcessName, agentSwitchCpuProcessEntryData.valid);
    break;

  case I_agentSwitchCpuProcessPercentageUtilization:
      bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (snmpAgentSwitchCpuProcessPercentageUtilizationGet(USMDB_UNIT_CURRENT, agentSwitchCpuProcessEntryData.agentSwitchCpuProcessIndex, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentSwitchCpuProcessEntryData.agentSwitchCpuProcessPercentageUtilization, snmp_buffer) == L7_TRUE))
        SET_VALID(I_agentSwitchCpuProcessPercentageUtilization, agentSwitchCpuProcessEntryData.valid);
    break;
  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentSwitchCpuProcessEntryData.valid) )
  {
      return(NULL);
  }

  return(&agentSwitchCpuProcessEntryData);
}
#endif /* I_agentSwitchCpuProcessIndex */

agentLoginSessionEntry_t *
k_agentLoginSessionEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_INT32 agentLoginSessionIndex)
{
  static agentLoginSessionEntry_t agentLoginSessionEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_inet_addr_t gaddr;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;

    agentLoginSessionEntryData.agentLoginSessionUserName = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(agentLoginSessionEntryData.valid);
  agentLoginSessionEntryData.agentLoginSessionIndex = agentLoginSessionIndex;
  SET_VALID(I_agentLoginSessionIndex, agentLoginSessionEntryData.valid);

  if ( (searchType == EXACT) ?
       (snmpAgentLoginSessionEntryGet(USMDB_UNIT_CURRENT, agentLoginSessionEntryData.agentLoginSessionIndex) != L7_SUCCESS) :
       ( (snmpAgentLoginSessionEntryGet(USMDB_UNIT_CURRENT, agentLoginSessionEntryData.agentLoginSessionIndex) != L7_SUCCESS) &&
         (snmpAgentLoginSessionEntryNextGet(USMDB_UNIT_CURRENT, &agentLoginSessionEntryData.agentLoginSessionIndex) != L7_SUCCESS) ) )
  {
    ZERO_VALID(agentLoginSessionEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_agentLoginSessionIndex:
    break;

  case I_agentLoginSessionUserName :
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbLoginSessionUserGet(USMDB_UNIT_CURRENT, agentLoginSessionEntryData.agentLoginSessionIndex, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentLoginSessionEntryData.agentLoginSessionUserName, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentLoginSessionUserName, agentLoginSessionEntryData.valid);
    break;

  case I_agentLoginSessionConnectionType :
    if (snmpAgentLoginSessionConnectionTypeGet(USMDB_UNIT_CURRENT, agentLoginSessionEntryData.agentLoginSessionIndex,
                                               &agentLoginSessionEntryData.agentLoginSessionConnectionType) == L7_SUCCESS)
      SET_VALID(I_agentLoginSessionConnectionType, agentLoginSessionEntryData.valid);
    break;

  case I_agentLoginSessionIdleTime :
    if (snmpLoginSessionIdleTimeGet(USMDB_UNIT_CURRENT, agentLoginSessionEntryData.agentLoginSessionIndex,
                                    &agentLoginSessionEntryData.agentLoginSessionIdleTime) == L7_SUCCESS)
      SET_VALID(I_agentLoginSessionIdleTime, agentLoginSessionEntryData.valid);
    break;

  case I_agentLoginSessionSessionTime :
    if (snmpLoginSessionTimeGet(USMDB_UNIT_CURRENT, agentLoginSessionEntryData.agentLoginSessionIndex,
                                &agentLoginSessionEntryData.agentLoginSessionSessionTime) == L7_SUCCESS)
      SET_VALID(I_agentLoginSessionSessionTime, agentLoginSessionEntryData.valid);
    break;

  case I_agentLoginSessionStatus :
    if (snmpAgentLoginSessionStatusGet(USMDB_UNIT_CURRENT, agentLoginSessionEntryData.agentLoginSessionIndex,
                                       &agentLoginSessionEntryData.agentLoginSessionStatus) == L7_SUCCESS)
      SET_VALID(I_agentLoginSessionStatus, agentLoginSessionEntryData.valid);
    break;

  case I_agentLoginSessionInetAddressType :
  case I_agentLoginSessionInetAddress :
    if (usmDbLoginSessionRemoteIpAddrGet(USMDB_UNIT_CURRENT, agentLoginSessionEntryData.agentLoginSessionIndex,
                                         &gaddr) == L7_SUCCESS)
    {
      if(gaddr.family == L7_AF_INET)
      {
        agentLoginSessionEntryData.agentLoginSessionInetAddressType = D_agentLoginSessionInetAddressType_ipv4;
        SafeMakeOctetString(&agentLoginSessionEntryData.agentLoginSessionInetAddress,(L7_uchar8*)&gaddr.addr.ipv4.s_addr, sizeof(gaddr.addr.ipv4.s_addr));
      }
      else if (gaddr.family == L7_AF_INET6)
      {
        agentLoginSessionEntryData.agentLoginSessionInetAddressType = D_agentLoginSessionInetAddressType_ipv6;
        SafeMakeOctetString(&agentLoginSessionEntryData.agentLoginSessionInetAddress, gaddr.addr.ipv6.in6.addr8, sizeof(gaddr.addr.ipv6.in6.addr8));
      }
      else
      {
        agentLoginSessionEntryData.agentLoginSessionInetAddressType = D_agentLoginSessionInetAddressType_unknown;
        SafeMakeOctetString(&agentLoginSessionEntryData.agentLoginSessionInetAddress, NULL, 0);
      }
    }
    else
    {
      agentLoginSessionEntryData.agentLoginSessionInetAddressType = D_agentLoginSessionInetAddressType_unknown;
      SafeMakeOctetString(&agentLoginSessionEntryData.agentLoginSessionInetAddress, NULL, 0);
    }

    SET_VALID(I_agentLoginSessionInetAddressType, agentLoginSessionEntryData.valid);
    SET_VALID(I_agentLoginSessionInetAddress, agentLoginSessionEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentLoginSessionEntryData.valid) )
    return(NULL);

  return(&agentLoginSessionEntryData);
}

#ifdef SETS
int
k_agentLoginSessionEntry_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentLoginSessionEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                               doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentLoginSessionEntry_set_defaults(doList_t *dp)
{
  agentLoginSessionEntry_t *data = (agentLoginSessionEntry_t *) (dp->data);

  if ((data->agentLoginSessionUserName = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }

  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_agentLoginSessionEntry_set(agentLoginSessionEntry_t *data,
                             ContextInfo *contextInfo, int function)
{
  if (VALID(I_agentLoginSessionStatus, data->valid) &&
      snmpAgentLoginSessionStatusSet(USMDB_UNIT_CURRENT, data->agentLoginSessionIndex,
                                     data->agentLoginSessionStatus) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  return NO_ERROR;
}

  #ifdef SR_agentLoginSessionEntry_UNDO
/* add #define SR_agentLoginSessionEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentLoginSessionEntry family.
 */
int
agentLoginSessionEntry_undo(doList_t *doHead, doList_t *doCur,
                            ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentLoginSessionEntry_UNDO */

#endif /* SETS */

agentTelnetConfigGroup_t *
k_agentTelnetConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                             int nominator)
{
  static agentTelnetConfigGroup_t agentTelnetConfigGroupData;

  ZERO_VALID(agentTelnetConfigGroupData.valid);

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
    if (nominator != -1) break;
    /* else pass through */

  case I_agentTelnetLoginTimeout :
    CLR_VALID(I_agentTelnetLoginTimeout, agentTelnetConfigGroupData.valid);
    if (usmDbAgentTelnetTimeoutGet(USMDB_UNIT_CURRENT, &agentTelnetConfigGroupData.agentTelnetLoginTimeout) == L7_SUCCESS)
      SET_VALID(I_agentTelnetLoginTimeout, agentTelnetConfigGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentTelnetMaxSessions :
    CLR_VALID(I_agentTelnetMaxSessions, agentTelnetConfigGroupData.valid);
    if (usmDbAgentTelnetNumSessionsGet(USMDB_UNIT_CURRENT, &agentTelnetConfigGroupData.agentTelnetMaxSessions) == L7_SUCCESS)
      SET_VALID(I_agentTelnetMaxSessions, agentTelnetConfigGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentTelnetAllowNewMode :
    CLR_VALID(I_agentTelnetAllowNewMode, agentTelnetConfigGroupData.valid);
    if (snmpAgentTelnetAllowNewModeGet(USMDB_UNIT_CURRENT, &agentTelnetConfigGroupData.agentTelnetAllowNewMode) == L7_SUCCESS)
      SET_VALID(I_agentTelnetAllowNewMode, agentTelnetConfigGroupData.valid);
    if (nominator != -1) break;

#ifdef I_agentTelnetMgmtPortNum
  case I_agentTelnetMgmtPortNum :
    CLR_VALID(I_agentTelnetMgmtPortNum, agentTelnetConfigGroupData.valid);
    if (usmDbSwDevCtrlTelnetMgmtPortNumGet(USMDB_UNIT_CURRENT, &agentTelnetConfigGroupData.agentTelnetMgmtPortNum) == L7_SUCCESS)
    {
      SET_VALID(I_agentTelnetMgmtPortNum, agentTelnetConfigGroupData.valid);
    }
    if (nominator != -1) break;
#endif /* I_agentTelnetMgmtPortNum */
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentTelnetConfigGroupData.valid) )
    return(NULL);

  return(&agentTelnetConfigGroupData);
}

#ifdef SETS
int
k_agentTelnetConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentTelnetConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                               doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentTelnetConfigGroup_set(agentTelnetConfigGroup_t *data,
                             ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (VALID(I_agentTelnetLoginTimeout, data->valid))
  {
    if(usmDbAgentTelnetTimeoutSet(USMDB_UNIT_CURRENT, data->agentTelnetLoginTimeout) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
    SET_VALID(I_agentTelnetLoginTimeout, tempValid);
    }
  }
  if (VALID(I_agentTelnetMaxSessions, data->valid))
  {
    if(usmDbAgentTelnetNumSessionsSet(USMDB_UNIT_CURRENT, data->agentTelnetMaxSessions) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentTelnetMaxSessions, tempValid);
    }
  }
  if (VALID(I_agentTelnetAllowNewMode, data->valid))
  {
    if(snmpAgentTelnetAllowNewModeSet(USMDB_UNIT_CURRENT, data->agentTelnetAllowNewMode) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;
}

  #ifdef SR_agentTelnetConfigGroup_UNDO
/* add #define SR_agentTelnetConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentTelnetConfigGroup family.
 */
int
agentTelnetConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                            ContextInfo *contextInfo)
{
  agentTelnetConfigGroup_t *data = (agentTelnetConfigGroup_t *) doCur->data;
  agentTelnetConfigGroup_t *undodata = (agentTelnetConfigGroup_t *) doCur->undodata;
  agentTelnetConfigGroup_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentTelnetConfigGroup_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentTelnetConfigGroup_UNDO */

#endif /* SETS */

agentPasswordManagementConfigGroup_t *
k_agentPasswordManagementConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                             int nominator)
{
  static agentPasswordManagementConfigGroup_t agentPasswordManagementConfigGroupData;
  L7_ushort16 val = 0;

  ZERO_VALID(agentPasswordManagementConfigGroupData.valid);

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
    if (nominator != -1) break;
    /* else pass through */

  case I_agentPasswordManagementMinLength :
    CLR_VALID(I_agentPasswordManagementMinLength, agentPasswordManagementConfigGroupData.valid);
    if (usmDbMinPassLengthGet(&val) == L7_SUCCESS)
    {
      agentPasswordManagementConfigGroupData.agentPasswordManagementMinLength = (L7_uint32)val;
      SET_VALID(I_agentPasswordManagementMinLength, agentPasswordManagementConfigGroupData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */

  case I_agentPasswordManagementHistory :
    CLR_VALID(I_agentPasswordManagementHistory, agentPasswordManagementConfigGroupData.valid);
    if (usmDbHistoryLengthGet(&val) == L7_SUCCESS)
    {
      agentPasswordManagementConfigGroupData.agentPasswordManagementHistory = (L7_uint32)val;
      SET_VALID(I_agentPasswordManagementHistory, agentPasswordManagementConfigGroupData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */

  case I_agentPasswordManagementAging :
    CLR_VALID(I_agentPasswordManagementAging, agentPasswordManagementConfigGroupData.valid);
    if (usmDbPasswdAgingValueGet(&val) == L7_SUCCESS)
    {
      agentPasswordManagementConfigGroupData.agentPasswordManagementAging = (L7_uint32)val;
      SET_VALID(I_agentPasswordManagementAging, agentPasswordManagementConfigGroupData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */

  case I_agentPasswordManagementLockAttempts :
    CLR_VALID(I_agentPasswordManagementLockAttempts, agentPasswordManagementConfigGroupData.valid);
    if (usmDbLockoutAttemptsGet(&val) == L7_SUCCESS)
    {
      agentPasswordManagementConfigGroupData.agentPasswordManagementLockAttempts = (L7_uint32)val;
      SET_VALID(I_agentPasswordManagementLockAttempts, agentPasswordManagementConfigGroupData.valid);
    }
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentPasswordManagementConfigGroupData.valid) )
    return(NULL);

  return(&agentPasswordManagementConfigGroupData);
}

#ifdef SETS
int
k_agentPasswordManagementConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentPasswordManagementConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                               doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentPasswordManagementConfigGroup_set(agentPasswordManagementConfigGroup_t *data,
                             ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (VALID(I_agentPasswordManagementMinLength, data->valid))
  {
    if(usmDbMinPasswdSet(USMDB_UNIT_CURRENT, data->agentPasswordManagementMinLength) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
    SET_VALID(I_agentPasswordManagementMinLength, tempValid);
    }
  }
  if (VALID(I_agentPasswordManagementHistory, data->valid))
  {
    if(usmDbPasswdHistoryLengthSet(USMDB_UNIT_CURRENT, data->agentPasswordManagementHistory) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentPasswordManagementHistory, tempValid);
    }
  }
  if (VALID(I_agentPasswordManagementAging, data->valid))
  {
    if(usmDbPasswdAgingSet(USMDB_UNIT_CURRENT, data->agentPasswordManagementAging) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentPasswordManagementAging, tempValid);
    }
  }
  if (VALID(I_agentPasswordManagementLockAttempts, data->valid))
  {
    if(usmDbPasswdsLockoutSet(USMDB_UNIT_CURRENT, data->agentPasswordManagementLockAttempts) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentPasswordManagementLockAttempts, tempValid);
    }
  }

  return NO_ERROR;
}

  #ifdef SR_agentPasswordManagementConfigGroup_UNDO
/* add #define SR_agentPasswordManagementConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentPasswordManagementConfigGroup family.
 */
int
agentPasswordManagementConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                            ContextInfo *contextInfo)
{
  agentPasswordManagementConfigGroup_t *data = (agentPasswordManagementConfigGroup_t *) doCur->data;
  agentPasswordManagementConfigGroup_t *undodata = (agentPasswordManagementConfigGroup_t *) doCur->undodata;
  agentPasswordManagementConfigGroup_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentPasswordManagementConfigGroup_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentPasswordManagementConfigGroup_UNDO */

#endif /* SETS */

agentUserConfigGroup_t *
k_agentUserConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                           int nominator)
{
  static agentUserConfigGroup_t agentUserConfigGroupData;
  static L7_BOOL firstTime = L7_TRUE;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;

    agentUserConfigGroupData.agentUserConfigCreate = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(agentUserConfigGroupData.valid);
  SET_VALID(I_agentUserConfigCreate, agentUserConfigGroupData.valid);

  return(&agentUserConfigGroupData);
}

#ifdef SETS
int
k_agentUserConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentUserConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                             doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentUserConfigGroup_set(agentUserConfigGroup_t *data,
                           ContextInfo *contextInfo, int function)
{
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (VALID(I_agentUserConfigCreate, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentUserConfigCreate->octet_ptr, data->agentUserConfigCreate->length);
    if (snmpAgentUserConfigCreateSet(USMDB_UNIT_CURRENT, snmp_buffer) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      creationErrorUserConfigGroup = L7_TRUE;
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentUserConfigCreate, tempValid);
    }
  }

  return NO_ERROR;
}

  #ifdef SR_agentUserConfigGroup_UNDO
/* add #define SR_agentUserConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentUserConfigGroup family.
 */
int
agentUserConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                          ContextInfo *contextInfo)
{
  agentUserConfigGroup_t *data = (agentUserConfigGroup_t *) doCur->data;

  L7_int32 function = SR_UNKNOWN;
  L7_int32 rc = L7_FAILURE;
  L7_int32 agentUserTempIndex;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  /* Define a table entry for destroying the created row */
  agentUserConfigEntry_t agentUserConfigEntryData;
  agentUserConfigEntry_t *entrysetdata = &agentUserConfigEntryData;

  /* Initialize this table entry */
  agentUserConfigEntryData.agentUserName = MakeOctetString(NULL, 0);
  agentUserConfigEntryData.agentUserPassword = MakeOctetString(NULL, 0);
  agentUserConfigEntryData.agentUserEncryptionPassword = MakeOctetString(NULL, 0);
  agentUserConfigEntryData.agentUserAuthenticationList = MakeOctetString(NULL, 0);
  agentUserConfigEntryData.agentUserPortSecurity = MakeOctetString(NULL, 0);
  ZERO_VALID(agentUserConfigEntryData.valid);

  if ( creationErrorUserConfigGroup == L7_TRUE )
  {
    creationErrorUserConfigGroup = L7_FALSE;
    return NO_ERROR;
  }

  if ( data == NULL )
    return UNDO_FAILED_ERROR;

  bzero(snmp_buffer, SNMP_BUFFER_LEN);
  strncpy(snmp_buffer, data->agentUserConfigCreate->octet_ptr, data->agentUserConfigCreate->length);

  /* Get the table index for the specific user name */
  if ( (rc = userMgrLoginIndexGet(snmp_buffer, &agentUserTempIndex)) == L7_SUCCESS)
  {
    /* we are trying to undo an add to the Table
     * undoing an add, so delete
    */
    agentUserConfigEntryData.agentUserStatus = D_agentUserStatus_destroy;
    agentUserConfigEntryData.agentUserIndex = agentUserTempIndex;
    SET_VALID(I_agentUserStatus, agentUserConfigEntryData.valid);
    function = SR_DELETE;

    /* use the set method for the undo */
    if ((entrysetdata != NULL) && (k_agentUserConfigEntry_set(entrysetdata, contextInfo, function) == NO_ERROR))
      return NO_ERROR;
  }

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentUserConfigGroup_UNDO */

#endif /* SETS */

agentUserConfigEntry_t *
k_agentUserConfigEntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           SR_INT32 agentUserIndex)
{
  static agentUserConfigEntry_t agentUserConfigEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 snmp_buffer_len = SNMP_BUFFER_LEN;
  L7_uint32 temp_unsigned;
  L7_BOOL bool_val;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;

    agentUserConfigEntryData.agentUserName = MakeOctetString(NULL, 0);
    agentUserConfigEntryData.agentUserPassword = MakeOctetString(NULL, 0);
    #if defined(I_agentUserEncryptionPassword)
    agentUserConfigEntryData.agentUserEncryptionPassword = MakeOctetString(NULL, 0);
    #endif
    agentUserConfigEntryData.agentUserAuthenticationList = MakeOctetString(NULL, 0);
    agentUserConfigEntryData.agentUserPortSecurity = MakeOctetString(NULL, 0);
    agentUserConfigEntryData.agentUserPasswordExpireTime = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(agentUserConfigEntryData.valid);
  agentUserConfigEntryData.agentUserIndex = agentUserIndex;
  SET_VALID(I_agentUserIndex, agentUserConfigEntryData.valid);

  if ( (searchType == EXACT) ?
       (snmpAgentUserConfigEntryGet(USMDB_UNIT_CURRENT, agentUserConfigEntryData.agentUserIndex) != L7_SUCCESS) :
       ( (snmpAgentUserConfigEntryGet(USMDB_UNIT_CURRENT, agentUserConfigEntryData.agentUserIndex) != L7_SUCCESS) &&
         (snmpAgentUserConfigEntryNextGet(USMDB_UNIT_CURRENT, &agentUserConfigEntryData.agentUserIndex) != L7_SUCCESS) ) )
  {
    ZERO_VALID(agentUserConfigEntryData.valid);
    return(NULL);
  }

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
  case I_agentUserIndex:
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentUserName :
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbLoginsGet(USMDB_UNIT_CURRENT, agentUserConfigEntryData.agentUserIndex, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentUserConfigEntryData.agentUserName, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentUserName, agentUserConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentUserPassword :
    SET_VALID(I_agentUserPassword, agentUserConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentUserAccessMode :
    if (snmpAgentUserAccessModeGet(USMDB_UNIT_CURRENT, agentUserConfigEntryData.agentUserIndex,
                                   &agentUserConfigEntryData.agentUserAccessMode) == L7_SUCCESS)
      SET_VALID(I_agentUserAccessMode, agentUserConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentUserStatus :
    if (snmpAgentUserStatusGet(USMDB_UNIT_CURRENT, agentUserConfigEntryData.agentUserIndex,
                               &agentUserConfigEntryData.agentUserStatus) == L7_SUCCESS)
      SET_VALID(I_agentUserStatus, agentUserConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */
#if defined(I_agentUserAuthenticationType) && L7_FEAT_SNMP_USER_MAPPING
  case I_agentUserAuthenticationType :
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNMP_COMPONENT_ID, L7_SNMP_USER_AUTHENTICATION_FEATURE_ID) == L7_TRUE &&
        snmpAgentUserAuthenticationTypeGet(USMDB_UNIT_CURRENT, agentUserConfigEntryData.agentUserIndex,
                                           &agentUserConfigEntryData.agentUserAuthenticationType) == L7_SUCCESS)
      SET_VALID(I_agentUserAuthenticationType, agentUserConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */
#endif

#if defined(I_agentUserEncryptionType) && L7_FEAT_SNMP_USER_MAPPING
  case I_agentUserEncryptionType :
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNMP_COMPONENT_ID, L7_SNMP_USER_ENCRYPTION_FEATURE_ID) == L7_TRUE &&
        snmpAgentUserEncryptionTypeGet(USMDB_UNIT_CURRENT, agentUserConfigEntryData.agentUserIndex,
                                       &agentUserConfigEntryData.agentUserEncryptionType) == L7_SUCCESS)
      SET_VALID(I_agentUserEncryptionType, agentUserConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */
#endif

#if defined(I_agentUserEncryptionPassword) && L7_FEAT_SNMP_USER_MAPPING
  case I_agentUserEncryptionPassword :
    SET_VALID(I_agentUserEncryptionPassword, agentUserConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */
#endif

  case I_agentUserAuthenticationList:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (snmpAgentUserAuthenticationListGet(USMDB_UNIT_CURRENT, agentUserConfigEntryData.agentUserIndex, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentUserConfigEntryData.agentUserAuthenticationList, snmp_buffer) == L7_TRUE))
      SET_VALID(nominator, agentUserConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentUserPortSecurity:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (snmpAgentUserPortSecurityGet(USMDB_UNIT_CURRENT, agentUserConfigEntryData.agentUserIndex, snmp_buffer, &snmp_buffer_len) == L7_SUCCESS &&
        (SafeMakeOctetString(&agentUserConfigEntryData.agentUserPortSecurity, snmp_buffer, snmp_buffer_len) == L7_TRUE))
      SET_VALID(nominator, agentUserConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentUserLockoutStatus :
    if (usmDbUserLockStatusGet(USMDB_UNIT_CURRENT, agentUserConfigEntryData.agentUserIndex,
                               &bool_val) == L7_SUCCESS)
    {
      agentUserConfigEntryData.agentUserLockoutStatus = (L7_uint32)bool_val;
      SET_VALID(I_agentUserLockoutStatus, agentUserConfigEntryData.valid);
    }
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentUserPasswordExpireTime :
    if (usmDbPasswordExpireTimeGet(USMDB_UNIT_CURRENT, agentUserConfigEntryData.agentUserIndex,
                                   &temp_unsigned) == L7_SUCCESS &&
        (SafeMakeDateAndTime(&agentUserConfigEntryData.agentUserPasswordExpireTime, temp_unsigned) == L7_TRUE))
      SET_VALID(I_agentUserPasswordExpireTime, agentUserConfigEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentUserConfigEntryData.valid) )
    return(NULL);

  return(&agentUserConfigEntryData);
}

#ifdef SETS
int
k_agentUserConfigEntry_test(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentUserConfigEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                             doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentUserConfigEntry_set_defaults(doList_t *dp)
{
  agentUserConfigEntry_t *data = (agentUserConfigEntry_t *) (dp->data);

  if ((data->agentUserName = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->agentUserPassword = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }

  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_agentUserConfigEntry_set(agentUserConfigEntry_t *data,
                           ContextInfo *contextInfo, int function)
{
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
#if defined(I_agentUserEncryptionType)
  L7_uint32 temp_encryption_type;
#endif

  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if ( snmpAgentUserConfigEntryGet(USMDB_UNIT_CURRENT, data->agentUserIndex) != L7_SUCCESS)
  {
    ZERO_VALID(data->valid);
    return(COMMIT_FAILED_ERROR);
  }

  if (data->agentUserName != NULL)
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentUserName->octet_ptr, data->agentUserName->length);
    if ( VALID(I_agentUserName, data->valid))
    {
      if(snmpAgentUserNameSet(USMDB_UNIT_CURRENT, data->agentUserIndex, snmp_buffer) != L7_SUCCESS )
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
      else
      {
        SET_VALID(I_agentUserName, tempValid);
      }
    }
  }

  if (data->agentUserPassword != NULL)
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentUserPassword->octet_ptr, data->agentUserPassword->length);
    if ( VALID(I_agentUserPassword, data->valid))
    {
      if (usmDbPasswdLengthValid (USMDB_UNIT_CURRENT, snmp_buffer, L7_FALSE) == L7_SUCCESS) 
      {
        if(usmDbPasswordSet(USMDB_UNIT_CURRENT, data->agentUserIndex, snmp_buffer, L7_FALSE) != L7_SUCCESS )
        {
          memcpy(data->valid, tempValid, sizeof(data->valid));
          return(COMMIT_FAILED_ERROR);
        }
        else
        {
          SET_VALID(I_agentUserPassword, tempValid);
        }
      }
      else
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);  
      }
    }
  }

  if ( VALID(I_agentUserAccessMode, data->valid))
  {
    if(snmpAgentUserAccessModeSet(USMDB_UNIT_CURRENT, data->agentUserIndex,
                                  data->agentUserAccessMode) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentUserAccessMode, tempValid);
    }
  }

  if ( VALID(I_agentUserStatus, data->valid))
  {
    if(snmpAgentUserStatusSet(USMDB_UNIT_CURRENT, data->agentUserIndex,
                              data->agentUserStatus) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentUserStatus, tempValid);
    }
  }

#if defined(I_agentUserAuthenticationType) && L7_FEAT_SNMP_USER_MAPPING
  if ( VALID(I_agentUserAuthenticationType, data->valid))
  {
    if((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNMP_COMPONENT_ID, L7_SNMP_USER_ENCRYPTION_FEATURE_ID) != L7_TRUE) ||
         (snmpAgentUserAuthenticationTypeSet(USMDB_UNIT_CURRENT, data->agentUserIndex,
                                            data->agentUserAuthenticationType) != L7_SUCCESS ))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentUserAuthenticationType, tempValid);
    }
  }
#endif
#if defined(I_agentUserEncryptionType) && defined(I_agentUserEncryptionPassword) && L7_FEAT_SNMP_USER_MAPPING
  if (VALID(I_agentUserEncryptionType, data->valid) &&
      !VALID(I_agentUserEncryptionPassword, data->valid))
  {
    if (data->agentUserEncryptionType == D_agentUserEncryptionType_none)
    {
      /* if set to none, assume a blank password */
      bzero(snmp_buffer, SNMP_BUFFER_LEN);
      if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNMP_COMPONENT_ID, L7_SNMP_USER_ENCRYPTION_FEATURE_ID) != L7_TRUE ||
          snmpAgentUserEncryptionSet(USMDB_UNIT_CURRENT,
                                     data->agentUserIndex,
                                     data->agentUserEncryptionType,
                                     snmp_buffer) != L7_SUCCESS )
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
      else
      {
        SET_VALID(I_agentUserEncryptionType, tempValid);
        SET_VALID(I_agentUserEncryptionPassword, tempValid);
      }
    }
    else
    {
      /* if set to DES, no password exists so fail */
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
  }
#endif
#if defined(I_agentUserEncryptionType) && defined(I_agentUserEncryptionPassword) && L7_FEAT_SNMP_USER_MAPPING
  if (!VALID(I_agentUserEncryptionType, data->valid) &&
      VALID(I_agentUserEncryptionPassword, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentUserEncryptionPassword->octet_ptr, data->agentUserEncryptionPassword->length);
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNMP_COMPONENT_ID, L7_SNMP_USER_ENCRYPTION_FEATURE_ID) != L7_TRUE) ||
        (snmpAgentUserEncryptionTypeGet(USMDB_UNIT_CURRENT, data->agentUserIndex, &temp_encryption_type) != L7_SUCCESS) ||
        (temp_encryption_type == D_agentUserEncryptionType_none) ||
        (snmpAgentUserEncryptionSet(USMDB_UNIT_CURRENT,
                                     data->agentUserIndex,
                                     temp_encryption_type,
                                     snmp_buffer) != L7_SUCCESS ))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentUserEncryptionType, tempValid);
      SET_VALID(I_agentUserEncryptionPassword, tempValid);
    }
  }
#endif
#if defined(I_agentUserEncryptionType) && defined(I_agentUserEncryptionPassword) && L7_FEAT_SNMP_USER_MAPPING
  if (VALID(I_agentUserEncryptionType, data->valid) &&
      VALID(I_agentUserEncryptionPassword, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentUserEncryptionPassword->octet_ptr, data->agentUserEncryptionPassword->length);
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNMP_COMPONENT_ID, L7_SNMP_USER_ENCRYPTION_FEATURE_ID) != L7_TRUE ||
        (snmpAgentUserEncryptionSet(USMDB_UNIT_CURRENT,
                                    data->agentUserIndex,
                                    data->agentUserEncryptionType,
                                    snmp_buffer) != L7_SUCCESS ))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentUserEncryptionType, tempValid);
      SET_VALID(I_agentUserEncryptionPassword, tempValid);
    }
  }
#endif

  if (data->agentUserAuthenticationList != NULL)
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentUserAuthenticationList->octet_ptr, data->agentUserAuthenticationList->length);
    if ( VALID(I_agentUserAuthenticationList, data->valid))
    {
      if(snmpAgentUserAuthenticationListSet(USMDB_UNIT_CURRENT, data->agentUserIndex, snmp_buffer) != L7_SUCCESS )
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
      else
      {
        SET_VALID(I_agentUserAuthenticationList, tempValid);
      }
    }
  }

  if (data->agentUserPortSecurity != NULL)
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentUserPortSecurity->octet_ptr, data->agentUserPortSecurity->length);
    if ( VALID(I_agentUserPortSecurity, data->valid))
    {
      if(snmpAgentUserPortSecuritySet(USMDB_UNIT_CURRENT, data->agentUserIndex, snmp_buffer, data->agentUserPortSecurity->length) != L7_SUCCESS )
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
    }
  }

  return NO_ERROR;
}

  #ifdef SR_agentUserConfigEntry_UNDO
/* add #define SR_agentUserConfigEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentUserConfigEntry family.
 */
int
agentUserConfigEntry_undo(doList_t *doHead, doList_t *doCur,
                          ContextInfo *contextInfo)
{
  agentUserConfigEntry_t *data = (agentUserConfigEntry_t *) doCur->data;
  agentUserConfigEntry_t *undodata = (agentUserConfigEntry_t *) doCur->undodata;
  agentUserConfigEntry_t *entrysetdata = NULL;

  L7_int32 function = SR_UNKNOWN;
  L7_int32 rc = L7_FAILURE;
  L7_int32 agentUserTempIndex;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  /* Define a group instance so as to create the deleted row */
  agentUserConfigGroup_t agentUserConfigGroupTempdata;
  agentUserConfigGroup_t *setdata = &agentUserConfigGroupTempdata;

  /* Initialize this group instance */
  agentUserConfigGroupTempdata.agentUserConfigCreate = CloneOctetString(undodata->agentUserName);
  ZERO_VALID(agentUserConfigGroupTempdata.valid);

  /*Copy valid bits from data to undodata */
  if( undodata != NULL && data != NULL )
    memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /*
   * we are either trying to undo a delete, or a modify
   * undoing a delete or modify, replace the original data
   */
  if(data->agentUserStatus ==  D_agentUserStatus_destroy)
  {
    /* check that the entry was actually deleted */
    if (snmpAgentUserConfigEntryGet(USMDB_UNIT_CURRENT,
                                    undodata->agentUserIndex) == L7_SUCCESS)
      return NO_ERROR;

    /* Use the set method for the creation of the row with default values */
    SET_VALID(I_agentUserConfigCreate, agentUserConfigGroupTempdata.valid);
    function = SR_ADD_MODIFY;
    if ((setdata != NULL) && k_agentUserConfigGroup_set(setdata, contextInfo, function) == NO_ERROR)
    {
      bzero(snmp_buffer, SNMP_BUFFER_LEN);
      strncpy(snmp_buffer, undodata->agentUserName->octet_ptr, undodata->agentUserName->length);

      /* Get the table index for the specific user name */
      if ( (rc = userMgrLoginIndexGet(snmp_buffer, &agentUserTempIndex)) == L7_SUCCESS)
      {
        undodata->agentUserIndex = agentUserTempIndex;

        /* set all valid for the setting the previous values to the created row */
        memset(undodata->valid, 0xffff, sizeof(undodata->valid));

        /* Status should not be set again */
        CLR_VALID(I_agentUserStatus, undodata->valid);

        entrysetdata = undodata;

        /* use the set method for the undo */
        if ((entrysetdata != NULL) && (k_agentUserConfigEntry_set(entrysetdata, contextInfo, function) == NO_ERROR))
        {
          free(agentUserConfigGroupTempdata.agentUserConfigCreate);
          return NO_ERROR;
        }
      }
    }
  }
  else
  {
    /* we are trying to undo a modify to the Table */
    if(undodata->agentUserStatus == D_agentUserStatus_notReady
    || undodata->agentUserStatus == D_agentUserStatus_notInService)
    {
      undodata->agentUserStatus = D_agentUserStatus_createAndWait;
    }
    else
    {
      if(undodata->agentUserStatus == D_agentUserStatus_active)
      {
        undodata->agentUserStatus = D_agentUserStatus_createAndGo;
      }
    }

    entrysetdata = undodata;
    function = SR_ADD_MODIFY;

    /* use the set method for the undo */
    if ((entrysetdata != NULL) && (k_agentUserConfigEntry_set(entrysetdata, contextInfo, function) == NO_ERROR))
    {
      free(agentUserConfigGroupTempdata.agentUserConfigCreate);
      return NO_ERROR;
    }
  }

  free(agentUserConfigGroupTempdata.agentUserConfigCreate);
  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentUserConfigEntry_UNDO */

#endif /* SETS */

agentSerialGroup_t *
k_agentSerialGroup_get(int serialNum, ContextInfo *contextInfo,
                       int nominator)
{
  static agentSerialGroup_t agentSerialGroupData;

  ZERO_VALID(agentSerialGroupData.valid);

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
    if (nominator != -1) break;
    /* else pass through */

  case I_agentSerialTimeout :
    if (snmpAgentSerialTimeoutGet(USMDB_UNIT_CURRENT, &agentSerialGroupData.agentSerialTimeout) == L7_SUCCESS)
      SET_VALID(I_agentSerialTimeout, agentSerialGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentSerialBaudrate :
    if (snmpAgentSerialBaudrateGet(USMDB_UNIT_CURRENT, &agentSerialGroupData.agentSerialBaudrate) == L7_SUCCESS)
      SET_VALID(I_agentSerialBaudrate, agentSerialGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentSerialCharacterSize :
    if (snmpAgentSerialCharacterSizeGet( USMDB_UNIT_CURRENT, &agentSerialGroupData.agentSerialCharacterSize) == L7_SUCCESS)
      SET_VALID(I_agentSerialCharacterSize, agentSerialGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentSerialHWFlowControlMode :
    if (snmpAgentSerialHWFlowControlModeGet( USMDB_UNIT_CURRENT, &agentSerialGroupData.agentSerialHWFlowControlMode) == L7_SUCCESS)
      SET_VALID(I_agentSerialHWFlowControlMode, agentSerialGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentSerialStopBits :
    if (snmpAgentSerialStopBitsGet( USMDB_UNIT_CURRENT, &agentSerialGroupData.agentSerialStopBits) == L7_SUCCESS)
      SET_VALID(I_agentSerialStopBits, agentSerialGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentSerialParityType :
    if (snmpAgentSerialParityTypeGet( USMDB_UNIT_CURRENT, &agentSerialGroupData.agentSerialParityType) == L7_SUCCESS)
      SET_VALID(I_agentSerialParityType, agentSerialGroupData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentSerialGroupData.valid) )
    return(NULL);

  return(&agentSerialGroupData);
}

#ifdef SETS
int
k_agentSerialGroup_test(ObjectInfo *object, ObjectSyntax *value,
                        doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentSerialGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                         doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentSerialGroup_set(agentSerialGroup_t *data,
                       ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if ( VALID(I_agentSerialTimeout, data->valid))
  {
    if(snmpAgentSerialTimeoutSet(USMDB_UNIT_CURRENT,
                                 data->agentSerialTimeout) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentSerialTimeout, tempValid);
    }
  }

  if ( VALID(I_agentSerialBaudrate, data->valid))
  {
    if(snmpAgentSerialBaudrateSet(USMDB_UNIT_CURRENT,
                                  data->agentSerialBaudrate) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentSerialBaudrate, tempValid);
    }
  }
/* lvl7_@p0859 start */
#ifdef NOT_SUPPORTED
  if ( VALID(I_agentSerialCharacterSize, data->valid))
  {
    if(snmpAgentSerialCharacterSizeSet(USMDB_UNIT_CURRENT,
                                       data->agentSerialCharacterSize) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentSerialCharacterSize, tempValid);
    }
  }

  if ( VALID(I_agentSerialHWFlowControlMode, data->valid))
  {
    if(snmpAgentSerialHWFlowControlModeSet(USMDB_UNIT_CURRENT,
                                           data->agentSerialHWFlowControlMode) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentSerialHWFlowControl, tempValid);
    }
  }

  if ( VALID(I_agentSerialStopBits, data->valid))
  {
    if(snmpAgentSerialStopBitsSet(USMDB_UNIT_CURRENT,
                                  data->agentSerialStopBits) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentSerialStopBits, tempValid);
    }
  }

  if ( VALID(I_agentSerialParityType, data->valid))
  {
    if(snmpAgentSerialParityTypeSet(USMDB_UNIT_CURRENT,
                                    data->agentSerialParityType) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
  }

#endif /* NOT_SUPPORTED */
/* lvl7_@p0859 end */

  return NO_ERROR;
}

  #ifdef SR_agentSerialGroup_UNDO
/* add #define SR_agentSerialGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentSerialGroup family.
 */
int
agentSerialGroup_undo(doList_t *doHead, doList_t *doCur,
                      ContextInfo *contextInfo)
{
  agentSerialGroup_t *data = (agentSerialGroup_t *) doCur->data;
  agentSerialGroup_t *undodata = (agentSerialGroup_t *) doCur->undodata;
  agentSerialGroup_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL )
    return UNDO_FAILED_ERROR;

  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentSerialGroup_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentSerialGroup_UNDO */

#endif /* SETS */

agentLagConfigGroup_t *
k_agentLagConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                          int nominator)
{
  static agentLagConfigGroup_t agentLagConfigGroupData;
  static L7_BOOL firstTime = L7_TRUE;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;

    agentLagConfigGroupData.agentLagConfigCreate = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(agentLagConfigGroupData.valid);

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentLagConfigCreate:
    SET_VALID(I_agentLagConfigCreate, agentLagConfigGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */
#ifdef I_agentLagConfigGroupHashOption
  case I_agentLagConfigGroupHashOption:
    if (snmpAgentLagConfigGroupHashOptionGet(USMDB_UNIT_CURRENT, 
                                             &(agentLagConfigGroupData.agentLagConfigGroupHashOption)) == L7_SUCCESS)
      SET_VALID(I_agentLagConfigGroupHashOption, agentLagConfigGroupData.valid);
    break;
#endif

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentLagConfigGroupData.valid))
    return(NULL);

  return(&agentLagConfigGroupData);
}

#ifdef SETS
int
k_agentLagConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                           doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentLagConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentLagConfigGroup_set(agentLagConfigGroup_t *data,
                          ContextInfo *contextInfo, int function)
{
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (VALID(I_agentLagConfigCreate, data->valid) && data->agentLagConfigCreate != NULL)
  {
    if (data->agentLagConfigCreate->length == 0)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentLagConfigCreate->octet_ptr, data->agentLagConfigCreate->length);

    if (snmpAgentLagConfigCreateSet(USMDB_UNIT_CURRENT, snmp_buffer, lagIntIfNum) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      creationErrorLagConfigGroup = L7_TRUE;
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentLagConfigCreate, tempValid);
    }
  }
  if (VALID(I_agentLagConfigStaticCapability, data->valid))
  {
    /*This is an obsolete object. Shouldn't receive any SET on it */
    memcpy(data->valid, tempValid, sizeof(data->valid));
    return NO_SUCH_NAME_ERROR;
  }
#ifdef I_agentLagConfigGroupHashOption
  if (VALID(I_agentLagConfigGroupHashOption, data->valid))
  {
    if (snmpAgentLagConfigGroupHashOptionSet(USMDB_UNIT_CURRENT, data->agentLagConfigGroupHashOption) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentLagConfigCreate, tempValid);
    }
  }
#endif

  return NO_ERROR;
}

  #ifdef SR_agentLagConfigGroup_UNDO
/* add #define SR_agentLagConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentLagConfigGroup family.
 */
int
agentLagConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                         ContextInfo *contextInfo)
{
  agentLagConfigGroup_t *data = (agentLagConfigGroup_t *) doCur->data;
  agentLagConfigGroup_t *undodata = (agentLagConfigGroup_t *) doCur->undodata;
  L7_int32 function = SR_UNKNOWN;

  L7_int32 intIfNum;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  /* Define a table entry for destroying the created row */
  agentLagSummaryConfigEntry_t agentLagSummaryConfigEntry;
  agentLagSummaryConfigEntry_t *setdata = &agentLagSummaryConfigEntry;

  /* Initialize this table Entry */
  agentLagSummaryConfigEntry.agentLagSummaryName = MakeOctetString(NULL, 0);
  ZERO_VALID(agentLagSummaryConfigEntry.valid);

  if ( creationErrorLagConfigGroup == L7_TRUE )
  {
    creationErrorLagConfigGroup = L7_FALSE;
    return NO_ERROR;
  }

  if( data == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  if( VALID(I_agentLagConfigCreate, undodata->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    strncpy(snmp_buffer, data->agentLagConfigCreate->octet_ptr, data->agentLagConfigCreate->length);
    snmp_buffer[data->agentLagConfigCreate->length] = '\0'; /* Making sure that we get a NULL Terminated string */

    if ( lagIntIfNum != NULL )
    {
      intIfNum = *lagIntIfNum; /* Internal interface number */
    }
    else
    {
      return UNDO_FAILED_ERROR;
    }

    /* we are trying to undo an add to the Table
     * undoing an add, so delete
    */
    agentLagSummaryConfigEntry.agentLagSummaryStatus = D_agentLagSummaryStatus_destroy;
    SET_VALID(I_agentLagSummaryStatus, agentLagSummaryConfigEntry.valid);
    function = SR_DELETE;

    /* Get the external interface number from the internal interface number */
    usmDbExtIfNumFromIntIfNum(intIfNum, &(agentLagSummaryConfigEntry.agentLagSummaryLagIndex));

    /* use the set method for the undo */
    if ((setdata != NULL) && (k_agentLagSummaryConfigEntry_set(setdata, contextInfo, function) == NO_ERROR))
      return NO_ERROR;
  }

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentLagConfigGroup_UNDO */

#endif /* SETS */

agentLagSummaryConfigEntry_t *
k_agentLagSummaryConfigEntry_get(int serialNum, ContextInfo *contextInfo,
                                 int nominator,
                                 int searchType,
                                 SR_INT32 agentLagSummaryLagIndex)
{
  static agentLagSummaryConfigEntry_t agentLagSummaryConfigEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 intIfNum;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;

    agentLagSummaryConfigEntryData.agentLagSummaryName = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(agentLagSummaryConfigEntryData.valid);
  agentLagSummaryConfigEntryData.agentLagSummaryLagIndex = agentLagSummaryLagIndex;
  SET_VALID(I_agentLagSummaryLagIndex, agentLagSummaryConfigEntryData.valid);

  if ( (searchType == EXACT) ?
       (snmpLagIndexGet(USMDB_UNIT_CURRENT, agentLagSummaryConfigEntryData.agentLagSummaryLagIndex) != L7_SUCCESS) :
       ( (snmpLagIndexGet(USMDB_UNIT_CURRENT, agentLagSummaryConfigEntryData.agentLagSummaryLagIndex) != L7_SUCCESS) &&
         (snmpLagIndexGetNext(USMDB_UNIT_CURRENT, &agentLagSummaryConfigEntryData.agentLagSummaryLagIndex) != L7_SUCCESS) ) )
  {
    ZERO_VALID(agentLagSummaryConfigEntryData.valid);
    return(NULL);
  }

  /* Conversion to internal interface number for usmdb calls */
  usmDbIntIfNumFromExtIfNum(agentLagSummaryConfigEntryData.agentLagSummaryLagIndex, &intIfNum);

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
  case I_agentLagSummaryLagIndex:
    if ( nominator != -1 ) break;
    /* else pass through */

#ifdef NOT_SUPPORTED
  case I_agentLagSummaryFlushTimer :
    if (usmDbDot3adFlushTimerGet(USMDB_UNIT_CURRENT, intIfNum,
                                 &agentLagSummaryConfigEntryData.agentLagSummaryFlushTimer) == L7_SUCCESS)
      SET_VALID(I_agentLagSummaryFlushTimer, agentLagSummaryConfigEntryData.valid);
    if ( nominator != -1 ) break;
#endif /* NOT_SUPPORTED */

  case I_agentLagSummaryName :
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbDot3adNameGet(USMDB_UNIT_CURRENT, intIfNum, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentLagSummaryConfigEntryData.agentLagSummaryName, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentLagSummaryName, agentLagSummaryConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentLagSummaryLinkTrap :
    if (snmpAgentLagSummaryLinkTrapGet(USMDB_UNIT_CURRENT, intIfNum,
                                       &agentLagSummaryConfigEntryData.agentLagSummaryLinkTrap) == L7_SUCCESS)
      SET_VALID(I_agentLagSummaryLinkTrap, agentLagSummaryConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentLagSummaryAdminMode :
    if (snmpAgentLagSummaryAdminModeGet(USMDB_UNIT_CURRENT, intIfNum,
                                        &agentLagSummaryConfigEntryData.agentLagSummaryAdminMode) == L7_SUCCESS)
      SET_VALID(I_agentLagSummaryAdminMode, agentLagSummaryConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentLagSummaryStpMode :
    if (snmpAgentLagSummaryStpModeGet(USMDB_UNIT_CURRENT, intIfNum,
                                      &agentLagSummaryConfigEntryData.agentLagSummaryStpMode) == L7_SUCCESS)
      SET_VALID(I_agentLagSummaryStpMode, agentLagSummaryConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentLagSummaryAddPort :
    /* always 0 */
    agentLagSummaryConfigEntryData.agentLagSummaryAddPort = 0;
    SET_VALID(I_agentLagSummaryAddPort, agentLagSummaryConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentLagSummaryDeletePort :
    /* always 0 */
    agentLagSummaryConfigEntryData.agentLagSummaryDeletePort = 0;
    SET_VALID(I_agentLagSummaryDeletePort, agentLagSummaryConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentLagSummaryStatus :
    /* always active */
    agentLagSummaryConfigEntryData.agentLagSummaryStatus = D_agentLagSummaryStatus_active;
    SET_VALID(I_agentLagSummaryStatus, agentLagSummaryConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentLagSummaryType :
    if (snmpAgentLagSummaryTypeGet(USMDB_UNIT_CURRENT, intIfNum,
                                   &agentLagSummaryConfigEntryData.agentLagSummaryType) == L7_SUCCESS)
      SET_VALID(I_agentLagSummaryType, agentLagSummaryConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentLagSummaryStaticCapability :
    if (snmpAgentLagSummaryStaticCapabilityGet(USMDB_UNIT_CURRENT, intIfNum,
                                               &agentLagSummaryConfigEntryData.agentLagSummaryStaticCapability) == L7_SUCCESS)
      SET_VALID(I_agentLagSummaryStaticCapability, agentLagSummaryConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

#ifdef I_agentLagSummaryHashMode
  case I_agentLagSummaryHashMode :
    if (snmpAgentLagSummaryHashModeGet(USMDB_UNIT_CURRENT, intIfNum,
                                   &agentLagSummaryConfigEntryData.agentLagSummaryHashMode) == L7_SUCCESS)
      SET_VALID(I_agentLagSummaryHashMode, agentLagSummaryConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */
#endif /* I_agentLagSummaryHashMode */

#ifdef I_agentLagSummaryHashOption
  case I_agentLagSummaryHashOption :
    if (snmpAgentLagSummaryHashOptionGet(USMDB_UNIT_CURRENT, intIfNum,
                                               &agentLagSummaryConfigEntryData.agentLagSummaryHashOption) == L7_SUCCESS)
      SET_VALID(I_agentLagSummaryHashOption, agentLagSummaryConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */
#endif

#ifdef I_agentLagSummarySwitchportMode
  case I_agentLagSummarySwitchportMode :
    if (snmpAgentLagSummarySwitchportModeGet(USMDB_UNIT_CURRENT, intIfNum,
                                               &agentLagSummaryConfigEntryData.agentLagSummarySwitchportMode) == L7_SUCCESS)
      SET_VALID(I_agentLagSummarySwitchportMode, agentLagSummaryConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */
#endif

    break;
  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentLagSummaryConfigEntryData.valid) )
    return(NULL);

  return(&agentLagSummaryConfigEntryData);
}

#ifdef SETS
int
k_agentLagSummaryConfigEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                  doList_t *dp, ContextInfo *contextInfo)
{
  return NO_ERROR;
}

int
k_agentLagSummaryConfigEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                   doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentLagSummaryConfigEntry_set_defaults(doList_t *dp)
{
  agentLagSummaryConfigEntry_t *data = (agentLagSummaryConfigEntry_t *) (dp->data);

  if ((data->agentLagSummaryName = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->agentLagSummaryStaticCapability = D_agentLagSummaryStaticCapability_disable;

  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_agentLagSummaryConfigEntry_set(agentLagSummaryConfigEntry_t *data,
                                 ContextInfo *contextInfo, int function)
{
  L7_uint32 intIfNum;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (snmpLagIndexGet(USMDB_UNIT_CURRENT, data->agentLagSummaryLagIndex) != L7_SUCCESS)
  {
    ZERO_VALID(data->valid);
    return COMMIT_FAILED_ERROR;
  }

  /* Conversion to internal interface number for usmDb calls */
  usmDbIntIfNumFromExtIfNum(data->agentLagSummaryLagIndex, &intIfNum);

#ifdef NOT_SUPPORTED
  if (VALID(I_agentLagSummaryFlushTimer, data->valid))
  {
    if(usmDbDot3adFlushTimerSet(USMDB_UNIT_CURRENT, intIfNum,
                               data->agentLagSummaryFlushTimer) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentLagSummaryFlushTimer, tempValid);
    }
  }
#endif /* NOT_SUPPORTED */

  if (VALID(I_agentLagSummaryName, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    strncpy(snmp_buffer, data->agentLagSummaryName->octet_ptr, data->agentLagSummaryName->length);
    if (usmDbDot3adNameSet(USMDB_UNIT_CURRENT, intIfNum, snmp_buffer) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentLagSummaryName, tempValid);
    }
  }

  if (VALID(I_agentLagSummaryLinkTrap, data->valid))
  {
    if(snmpAgentLagSummaryLinkTrapSet(USMDB_UNIT_CURRENT, intIfNum,
                                     data->agentLagSummaryLinkTrap) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentLagSummaryLinkTrap, tempValid);
    }
  }

  if (VALID(I_agentLagSummaryAdminMode, data->valid))
  {
    if(snmpAgentLagSummaryAdminModeSet(USMDB_UNIT_CURRENT, intIfNum,
                                      data->agentLagSummaryAdminMode) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentLagSummaryAdminMode, tempValid);
    }
  }

  if (VALID(I_agentLagSummaryStpMode, data->valid))
  {
    if(snmpAgentLagSummaryStpModeSet(USMDB_UNIT_CURRENT, intIfNum,
                                    data->agentLagSummaryStpMode) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentLagSummaryStpMode, tempValid);
    }
  }

  if (VALID(I_agentLagSummaryAddPort, data->valid))
  {
    if(snmpAgentLagSummaryAddPortSet(USMDB_UNIT_CURRENT, intIfNum,
                                    data->agentLagSummaryAddPort) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentLagSummaryAddPort, tempValid);
    }
  }

  if (VALID(I_agentLagSummaryDeletePort, data->valid))
  {
    if(snmpAgentLagSummaryDeletePortSet(USMDB_UNIT_CURRENT, intIfNum,
                                       data->agentLagSummaryDeletePort) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentLagSummaryDeletePort, tempValid);
    }
  }

  if (VALID(I_agentLagSummaryStatus, data->valid))
  {
    if(snmpAgentLagSummaryStatusSet(USMDB_UNIT_CURRENT, intIfNum,
                                   data->agentLagSummaryStatus) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentLagSummaryStatus, tempValid);
    }
  }

  if (VALID(I_agentLagSummaryStaticCapability, data->valid))
  {
    if(snmpAgentLagSummaryStaticCapabilitySet(USMDB_UNIT_CURRENT, intIfNum,
                                              data->agentLagSummaryStaticCapability) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

#ifdef I_agentLagSummaryHashMode
  if (VALID(I_agentLagSummaryHashMode, data->valid))
  {
    if(snmpAgentLagSummaryHashOptionSet(USMDB_UNIT_CURRENT, intIfNum,
                                              data->agentLagSummaryHashMode) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }
#endif

#ifdef I_agentLagSummaryHashOption
  if (VALID(I_agentLagSummaryHashOption, data->valid))
  {
    if(snmpAgentLagSummaryHashOptionSet(USMDB_UNIT_CURRENT, intIfNum,
                                              data->agentLagSummaryHashOption) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }
#endif

#ifdef I_agentLagSummarySwitchportMode
  if (VALID(I_agentLagSummarySwitchportMode, data->valid))
  {
    if(snmpAgentLagSummarySwitchportModeSet(USMDB_UNIT_CURRENT, intIfNum,
                                              data->agentLagSummarySwitchportMode) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }
#endif

  return NO_ERROR;
}

  #ifdef SR_agentLagSummaryConfigEntry_UNDO
/* add #define SR_agentLagSummaryConfigEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentLagSummaryConfigEntry family.
 */
int
agentLagSummaryConfigEntry_undo(doList_t *doHead, doList_t *doCur,
                                ContextInfo *contextInfo)
{
  agentLagSummaryConfigEntry_t *data = (agentLagSummaryConfigEntry_t *) doCur->data;
  agentLagSummaryConfigEntry_t *undodata = (agentLagSummaryConfigEntry_t *) doCur->undodata;
  agentLagSummaryConfigEntry_t *entrysetdata = NULL;

  L7_int32 function = SR_UNKNOWN;
  L7_int32 intIfNum;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  /* Define a group instance so as to create the deleted row */
  agentLagConfigGroup_t agentLagConfigGroupTempdata;
  agentLagConfigGroup_t *groupsetdata = &agentLagConfigGroupTempdata;

  /* Initialize this group instance */
  agentLagConfigGroupTempdata.agentLagConfigCreate = CloneOctetString(undodata->agentLagSummaryName);
  ZERO_VALID(agentLagConfigGroupTempdata.valid);

  /*Copy valid bits from data to undodata */
  if( undodata != NULL && data != NULL )
    memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /*
   * we are either trying to undo a delete, or a modify
   * undoing a delete or modify, replace the original data
   */
  if(data->agentLagSummaryStatus ==  D_agentLagSummaryStatus_destroy)
  {
    /* check that the entry was actually deleted */
    if (snmpLagIndexGet(USMDB_UNIT_CURRENT,
                        undodata->agentLagSummaryLagIndex) == L7_SUCCESS)
      return NO_ERROR;

    /* Use the set method for the creation of the row with default values */
    SET_VALID(I_agentLagConfigCreate, agentLagConfigGroupTempdata.valid);
    function = SR_ADD_MODIFY;
    if ( (groupsetdata != NULL)&& (k_agentLagConfigGroup_set(groupsetdata, contextInfo, function) == NO_ERROR))
    {
      bzero(snmp_buffer, SNMP_BUFFER_LEN);
      strncpy(snmp_buffer, undodata->agentLagSummaryName->octet_ptr, undodata->agentLagSummaryName->length);

      /* Making sure that we get a NULL Terminated string */
      snmp_buffer[undodata->agentLagSummaryName->length] = '\0';

      intIfNum = *lagIntIfNum; /* Internal interface number */

      /* set all valid for the setting the previous values to the created row */
      memset(undodata->valid, 0xff, sizeof(undodata->valid));

      /* Status should not be set again */
      CLR_VALID(I_agentLagSummaryStatus, undodata->valid);

      /* agent Add Lagport cannot be set to zero(default value) again */
      if( undodata->agentLagSummaryAddPort == 0 )
        CLR_VALID(I_agentLagSummaryAddPort, undodata->valid);

      /* agent Delete Lagport cannot be set to zero(default value) again */
      if( undodata->agentLagSummaryDeletePort == 0 )
        CLR_VALID(I_agentLagSummaryDeletePort, undodata->valid);

      /* Get the external interface number from internal interface number */
      usmDbExtIfNumFromIntIfNum(intIfNum, &(undodata->agentLagSummaryLagIndex));

      entrysetdata = undodata;

      /* use the set method for the undo */
      if ((entrysetdata != NULL) && (k_agentLagSummaryConfigEntry_set(entrysetdata,
                                                                      contextInfo, function) == NO_ERROR))
      {
        free(agentLagConfigGroupTempdata.agentLagConfigCreate);
        return NO_ERROR;
      }
    }
  }
  else
  {
    /* we are trying to undo a modify to the Table */
    if(undodata->agentLagSummaryStatus == D_agentLagSummaryStatus_notReady
    || undodata->agentLagSummaryStatus == D_agentLagSummaryStatus_notInService)
    {
      undodata->agentLagSummaryStatus = D_agentLagSummaryStatus_createAndWait;
    }
    else
    {
      if(undodata->agentLagSummaryStatus == D_agentLagSummaryStatus_active)
      {
        undodata->agentLagSummaryStatus = D_agentLagSummaryStatus_createAndGo;
      }
    }
    entrysetdata = undodata;
    function = SR_ADD_MODIFY;

    /* use the set method for the undo */
    if ((entrysetdata != NULL) && (k_agentLagSummaryConfigEntry_set(entrysetdata, contextInfo, function) == NO_ERROR))
    {
      free(agentLagConfigGroupTempdata.agentLagConfigCreate);
      return NO_ERROR;
    }
  }

  free(agentLagConfigGroupTempdata.agentLagConfigCreate);
  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentLagSummaryConfigEntry_UNDO */

#endif /* SETS */

agentLagDetailedConfigEntry_t *
k_agentLagDetailedConfigEntry_get(int serialNum, ContextInfo *contextInfo,
                                  int nominator,
                                  int searchType,
                                  SR_INT32 agentLagDetailedLagIndex,
                                  SR_INT32 agentLagDetailedIfIndex)
{
  static agentLagDetailedConfigEntry_t agentLagDetailedConfigEntryData;
  L7_uint32 memberIntIfNum;
  static L7_BOOL firstTime = L7_TRUE;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    agentLagDetailedConfigEntryData.agentLagDetailedPortSpeed = MakeOIDFromDot("0.0");
  }

  ZERO_VALID(agentLagDetailedConfigEntryData.valid);
  agentLagDetailedConfigEntryData.agentLagDetailedLagIndex = agentLagDetailedLagIndex;
  SET_VALID(I_agentLagDetailedLagIndex, agentLagDetailedConfigEntryData.valid);
  agentLagDetailedConfigEntryData.agentLagDetailedIfIndex = agentLagDetailedIfIndex;
  SET_VALID(I_agentLagDetailedIfIndex, agentLagDetailedConfigEntryData.valid);

  if ( ( (searchType == EXACT) ?
         (snmpLagDetailedConfigEntryGet(USMDB_UNIT_CURRENT, agentLagDetailedConfigEntryData.agentLagDetailedLagIndex,
                                        agentLagDetailedConfigEntryData.agentLagDetailedIfIndex) != L7_SUCCESS) :
         (snmpLagDetailedConfigEntryGet(USMDB_UNIT_CURRENT, agentLagDetailedConfigEntryData.agentLagDetailedLagIndex,
                                        agentLagDetailedConfigEntryData.agentLagDetailedIfIndex) != L7_SUCCESS) &&
         (snmpLagDetailedConfigEntryNext(USMDB_UNIT_CURRENT, &agentLagDetailedConfigEntryData.agentLagDetailedLagIndex,
                                         &agentLagDetailedConfigEntryData.agentLagDetailedIfIndex) != L7_SUCCESS)) ||
       (usmDbIntIfNumFromExtIfNum(agentLagDetailedConfigEntryData.agentLagDetailedIfIndex, &memberIntIfNum) != L7_SUCCESS))
  {
    ZERO_VALID(agentLagDetailedConfigEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_agentLagDetailedLagIndex:
  case I_agentLagDetailedIfIndex:
    break;

  case I_agentLagDetailedPortSpeed :
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    FreeOID(agentLagDetailedConfigEntryData.agentLagDetailedPortSpeed);
    if (snmpAgentLagDetailedPortSpeedGet(USMDB_UNIT_CURRENT, memberIntIfNum, snmp_buffer) == L7_SUCCESS)
    {
      if ((agentLagDetailedConfigEntryData.agentLagDetailedPortSpeed = MakeOIDFromDot(snmp_buffer)) != NULL)
        SET_VALID(I_agentLagDetailedPortSpeed, agentLagDetailedConfigEntryData.valid);
    }
    else
    {
      agentLagDetailedConfigEntryData.agentLagDetailedPortSpeed = MakeOIDFromDot("0.0");
    }
    break;

  case I_agentLagDetailedPortStatus:
    if (snmpAgentLagDetailedPortStatusGet(USMDB_UNIT_CURRENT, agentLagDetailedConfigEntryData.agentLagDetailedLagIndex,
                                          memberIntIfNum, &agentLagDetailedConfigEntryData.agentLagDetailedPortStatus) == L7_SUCCESS)
        SET_VALID(I_agentLagDetailedPortStatus, agentLagDetailedConfigEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentLagDetailedConfigEntryData.valid) )
    return(NULL);

  return(&agentLagDetailedConfigEntryData);
}


agentNetworkConfigGroup_t *
k_agentNetworkConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                              int nominator)
{
  static agentNetworkConfigGroup_t agentNetworkConfigGroupData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;

    agentNetworkConfigGroupData.agentNetworkBurnedInMacAddress = MakeOctetString(NULL, 0);
    agentNetworkConfigGroupData.agentNetworkLocalAdminMacAddress = MakeOctetString(NULL, 0);
    agentNetworkConfigGroupData.agentNetworkIpv6Gateway = MakeOctetString(NULL, 0);
#ifdef I_agentNetworkDhcp6ClientDuid
    agentNetworkConfigGroupData.agentNetworkDhcp6ClientDuid = MakeOctetString(NULL, 0);
#endif
  }

  ZERO_VALID(agentNetworkConfigGroupData.valid);

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

/* lvl7_@p1261 start */
  switch (nominator)
  {
  case -1:
    if (nominator != -1) break;
    /* else pass through */

  case I_agentNetworkIPAddress :
    if (usmDbAgentIpIfAddressGet(USMDB_UNIT_CURRENT, &agentNetworkConfigGroupData.agentNetworkIPAddress) == L7_SUCCESS)
      SET_VALID(I_agentNetworkIPAddress, agentNetworkConfigGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentNetworkSubnetMask :
    if (usmDbAgentIpIfNetMaskGet(USMDB_UNIT_CURRENT, &agentNetworkConfigGroupData.agentNetworkSubnetMask) == L7_SUCCESS)
      SET_VALID(I_agentNetworkSubnetMask, agentNetworkConfigGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentNetworkDefaultGateway :
    if (usmDbAgentIpIfDefaultRouterGet(USMDB_UNIT_CURRENT, &agentNetworkConfigGroupData.agentNetworkDefaultGateway) == L7_SUCCESS)
      SET_VALID(I_agentNetworkDefaultGateway, agentNetworkConfigGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentNetworkBurnedInMacAddress :
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbSwDevCtrlBurnedInMacAddrGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetString(&agentNetworkConfigGroupData.agentNetworkBurnedInMacAddress, snmp_buffer, L7_MAC_ADDR_LEN) == L7_TRUE))
      SET_VALID(I_agentNetworkBurnedInMacAddress, agentNetworkConfigGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentNetworkLocalAdminMacAddress :
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbSwDevCtrlLocalAdminAddrGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetString(&agentNetworkConfigGroupData.agentNetworkLocalAdminMacAddress, snmp_buffer, L7_MAC_ADDR_LEN) == L7_TRUE))
      SET_VALID(I_agentNetworkLocalAdminMacAddress, agentNetworkConfigGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentNetworkMacAddressType :
    if (snmpAgentNetworkMacAddressTypeGet(USMDB_UNIT_CURRENT, &agentNetworkConfigGroupData.agentNetworkMacAddressType) == L7_SUCCESS)
      SET_VALID(I_agentNetworkMacAddressType, agentNetworkConfigGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentNetworkConfigProtocol :
    if (snmpAgentNetworkConfigProtocolGet(USMDB_UNIT_CURRENT, &agentNetworkConfigGroupData.agentNetworkConfigProtocol) == L7_SUCCESS)
      SET_VALID(I_agentNetworkConfigProtocol, agentNetworkConfigGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentNetworkMgmtVlan :
    if (usmDbMgmtVlanIdGet(USMDB_UNIT_CURRENT, &agentNetworkConfigGroupData.agentNetworkMgmtVlan) == L7_SUCCESS)
      SET_VALID(I_agentNetworkMgmtVlan, agentNetworkConfigGroupData.valid);
    if (nominator != -1)
    break;
  case I_agentNetworkIpv6AdminMode :
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
    if (snmpAgentNetworkIpv6AdminModeGet(USMDB_UNIT_CURRENT, &agentNetworkConfigGroupData.agentNetworkIpv6AdminMode) == L7_SUCCESS)
      SET_VALID(I_agentNetworkIpv6AdminMode, agentNetworkConfigGroupData.valid);
#endif
    if (nominator != -1)
    break;

  case I_agentNetworkIpv6Gateway :
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
    if (snmpAgentNetworkIpv6GatewayGet(USMDB_UNIT_CURRENT,snmp_buffer) == L7_SUCCESS &&
         (SafeMakeOctetString(&agentNetworkConfigGroupData.agentNetworkIpv6Gateway, snmp_buffer, sizeof(L7_in6_addr_t)) == L7_TRUE))
      SET_VALID(I_agentNetworkIpv6Gateway, agentNetworkConfigGroupData.valid);
#endif
    if (nominator != -1)
    break;

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
#ifdef I_agentNetworkIpv6AddressAutoConfig
    case I_agentNetworkIpv6AddressAutoConfig:
      if (snmpAgentNetworkIPv6AutoConfigGet(&agentNetworkConfigGroupData.agentNetworkIpv6AddressAutoConfig) == L7_SUCCESS)
      {
        SET_VALID(I_agentNetworkIpv6AddressAutoConfig, agentNetworkConfigGroupData.valid);
      }
      if (nominator != -1) break;
#endif
      /* else pass through */
#ifdef I_agentNetworkIpv6ConfigProtocol
    case I_agentNetworkIpv6ConfigProtocol:
      if (snmpAgentNetworkIPv6ConfigProtocolGet(&agentNetworkConfigGroupData.agentNetworkIpv6ConfigProtocol) == L7_SUCCESS)
      {
        SET_VALID(I_agentNetworkIpv6ConfigProtocol, agentNetworkConfigGroupData.valid);
      }
      if (nominator != -1) break;
#endif
      /* else pass through */
#ifdef I_agentNetworkDhcp6ClientDuid
    case I_agentNetworkDhcp6ClientDuid:
      memset(snmp_buffer, 0, sizeof(snmp_buffer));
      if (usmDbDhcp6cDuidGet(snmp_buffer) == L7_SUCCESS)
      { 
         if (SafeMakeOctetStringFromTextExact(&agentNetworkConfigGroupData.agentNetworkDhcp6ClientDuid, snmp_buffer) == L7_TRUE)
         {
            SET_VALID(I_agentNetworkDhcp6ClientDuid, agentNetworkConfigGroupData.valid);
         }
      }
      if (nominator != -1) break;
#endif
      /* else pass through */
#endif
      break;
  default:
    /* unknown nominator */
    return(NULL);
    break;
  }
/* lvl7_@p1261 end */

  if ( nominator >= 0 && !VALID(nominator, agentNetworkConfigGroupData.valid) )
    return(NULL);

  return(&agentNetworkConfigGroupData);
}

#ifdef SETS
int
k_agentNetworkConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                               doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentNetworkConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                                doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentNetworkConfigGroup_set(agentNetworkConfigGroup_t *data,
                              ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));
#ifdef L7_ROUTING_PACKAGE
  L7_uint32 intIfNum;
#endif

  if ( VALID(I_agentNetworkConfigProtocol, data->valid))
  {
    if(snmpAgentNetworkConfigProtocolSet(USMDB_UNIT_CURRENT,
                                         data->agentNetworkConfigProtocol) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentNetworkConfigProtocol, tempValid);
    }
  }
/* lvl7_@p1261 start */
  if ( VALID(I_agentNetworkIPAddress, data->valid))
  {
    if(snmpAgentNetworkIPAddressSet(USMDB_UNIT_CURRENT,
                                    data->agentNetworkIPAddress) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentNetworkIPAddress, tempValid);
    }
  }

  if ( VALID(I_agentNetworkSubnetMask, data->valid))
  {
    if(snmpAgentNetworkSubnetMaskSet(USMDB_UNIT_CURRENT,
                                     data->agentNetworkSubnetMask) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentNetworkSubnetMask, tempValid);
    }
  }
/* lvl7_@p1261 end */

  if ( VALID(I_agentNetworkDefaultGateway, data->valid))
  {
    if(snmpAgentNetworkDefaultGatewaySet(USMDB_UNIT_CURRENT,
                                         data->agentNetworkDefaultGateway) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentNetworkDefaultGateway, tempValid);
    }
  }

  if ( VALID(I_agentNetworkLocalAdminMacAddress, data->valid))
  {
    if(snmpAgentNetworkLocalAdminMacAddressSet(USMDB_UNIT_CURRENT,
                                               data->agentNetworkLocalAdminMacAddress->octet_ptr) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentNetworkLocalAdminMacAddress, tempValid);
    }
  }

  #if 0
  if ( VALID(I_agentNetworkMacAddressType, data->valid))
  {
    if(snmpAgentNetworkMacAddressTypeSet(USMDB_UNIT_CURRENT,
                                         data->agentNetworkMacAddressType) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentNetworkMacAddressType, tempValid);
    }
  }
  #endif

  if ( VALID(I_agentNetworkWebMode, data->valid))
  {
    if(snmpAgentNetworkWebModeSet(USMDB_UNIT_CURRENT,
                                  data->agentNetworkWebMode) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentNetworkWebMode, tempValid);
    }
  }

  if ( VALID(I_agentNetworkJavaMode, data->valid))
  {
    if(snmpAgentNetworkJavaModeSet(USMDB_UNIT_CURRENT,
                                   data->agentNetworkJavaMode) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentNetworkJavaMode, tempValid);
    }
  }

  if ( VALID(I_agentNetworkMgmtVlan, data->valid))
  {
    if ((data->agentNetworkMgmtVlan < L7_DOT1Q_MIN_VLAN_ID) ||
        (data->agentNetworkMgmtVlan > L7_DOT1Q_MAX_VLAN_ID))
      return COMMIT_FAILED_ERROR;

#ifdef L7_ROUTING_PACKAGE
    /* if this VLAN is enabled for routing, return a failure */
    if (usmDbIpVlanRtrVlanIdToIntIfNum(0, data->agentNetworkMgmtVlan, &intIfNum) == L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }
#endif

    if(usmDbMgmtVlanIdSet(USMDB_UNIT_CURRENT,
                          data->agentNetworkMgmtVlan) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
  }
   if ( VALID(I_agentNetworkIpv6AdminMode, data->valid ))
   {
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
     if(snmpAgentNetworkIpv6AdminModeSet(USMDB_UNIT_CURRENT,data->agentNetworkIpv6AdminMode) != L7_SUCCESS )
     {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return(COMMIT_FAILED_ERROR);
     }
     else
     {
       SET_VALID(I_agentNetworkIpv6AdminMode, tempValid);
     }
#endif
   }

   if ( VALID(I_agentNetworkIpv6Gateway, data->valid ))
   {

      bzero(snmp_buffer, SNMP_BUFFER_LEN);
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
      memcpy(snmp_buffer, data->agentNetworkIpv6Gateway->octet_ptr, data->agentNetworkIpv6Gateway->length);
      if(snmpAgentNetworkIpv6GatewaySet(USMDB_UNIT_CURRENT,snmp_buffer) != L7_SUCCESS )
      {
         memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
      else
      {
        SET_VALID(I_agentNetworkIpv6Gateway, tempValid);
      }
#endif
   }
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
#ifdef I_agentNetworkIpv6AddressAutoConfig
   if ( VALID (I_agentNetworkIpv6AddressAutoConfig, data->valid ))
   {
     if (snmpAgentNetworkIPv6AutoConfigSet(data->agentNetworkIpv6AddressAutoConfig) != L7_SUCCESS)
     {
        return(COMMIT_FAILED_ERROR);
     }
   }
#endif
#ifdef I_agentNetworkIpv6ConfigProtocol
   if (VALID (I_agentNetworkIpv6ConfigProtocol, data->valid ))
   {
     if (snmpAgentNetworkIPv6ConfigProtocolSet(data->agentNetworkIpv6ConfigProtocol) != L7_SUCCESS)
     {
        return(COMMIT_FAILED_ERROR);
     }
   }
#endif
#endif
  return NO_ERROR;
}

  #ifdef SR_agentNetworkConfigGroup_UNDO
/* add #define SR_agentNetworkConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentNetworkConfigGroup family.
 */
int
agentNetworkConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                             ContextInfo *contextInfo)
{
  agentNetworkConfigGroup_t *data = (agentNetworkConfigGroup_t *) doCur->data;
  agentNetworkConfigGroup_t *undodata = (agentNetworkConfigGroup_t *) doCur->undodata;
  agentNetworkConfigGroup_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentNetworkConfigGroup_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentNetworkConfigGroup_UNDO */

#endif /* SETS */


agentNetworkIpv6AddrEntry_t *
k_agentNetworkIpv6AddrEntry_get(int serialNum, ContextInfo *contextInfo,
                                    int nominator,
                                    int searchType,
                                    OctetString * agentNetworkIpv6AddrPrefix

                       )
{
   #if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
   static agentNetworkIpv6AddrEntry_t agentNetworkIpv6AddrEntryData;
   static L7_BOOL firstTime = L7_TRUE;
   L7_in6_addr_t prefix;

   memset(&prefix, 0, sizeof(L7_in6_addr_t));

   if (firstTime == L7_TRUE)
   {
    agentNetworkIpv6AddrEntryData.agentNetworkIpv6AddrPrefix = MakeOctetString(NULL,0);
    firstTime = L7_FALSE;
   }
   ZERO_VALID(agentNetworkIpv6AddrEntryData.valid);

   memcpy(&prefix , agentNetworkIpv6AddrPrefix->octet_ptr, agentNetworkIpv6AddrPrefix->length);
if (  (searchType == EXACT) ?
             (snmpAgentNetworkIpv6AddrEntryGet(USMDB_UNIT_CURRENT,
                                                   prefix,
                                                   &agentNetworkIpv6AddrEntryData.agentNetworkIpv6AddrPrefixLength,
                                                   agentNetworkIpv6AddrEntryData.agentNetworkIpv6AddrEuiFlag) != L7_SUCCESS) :
             ((snmpAgentNetworkIpv6AddrEntryGet(USMDB_UNIT_CURRENT,
                                                   prefix,
                                                   &agentNetworkIpv6AddrEntryData.agentNetworkIpv6AddrPrefixLength,
                                                   agentNetworkIpv6AddrEntryData.agentNetworkIpv6AddrEuiFlag) != L7_SUCCESS) &&
             (snmpAgentNetworkIpv6AddrEntryNext(USMDB_UNIT_CURRENT,
                                                   &prefix,
                                                   &agentNetworkIpv6AddrEntryData.agentNetworkIpv6AddrPrefixLength,
                                                   &agentNetworkIpv6AddrEntryData.agentNetworkIpv6AddrEuiFlag) != L7_SUCCESS)))

   {
     ZERO_VALID(agentNetworkIpv6AddrEntryData.valid);
     return(NULL);
   }
  if(SafeMakeOctetString(&agentNetworkIpv6AddrEntryData.agentNetworkIpv6AddrPrefix, (L7_uchar8 *)&prefix, sizeof(L7_in6_addr_t)) != L7_TRUE)
  {
    ZERO_VALID(agentNetworkIpv6AddrEntryData.valid);
    return(NULL);
  }
  else
  {
     SET_VALID(I_agentNetworkIpv6AddrPrefix,agentNetworkIpv6AddrEntryData.valid);
  }
  switch(nominator)
  {
    case -1:
    case I_agentNetworkIpv6AddrPrefix:
    /*case I_agentNetworkIpv6AddrPrefixLength:*/
    case I_agentNetworkIpv6AddrEuiFlag:
    if ( nominator != -1 ) break;

     case I_agentServicePortIpv6AddrPrefixLength:
      /*agentServicePortIpv6AddrEntryData.agentServicePortIpv6AddrPrefixLength */
      SET_VALID(I_agentNetworkIpv6AddrPrefixLength, agentNetworkIpv6AddrEntryData.valid);
      if ( nominator != -1 ) break;

    case I_agentNetworkIpv6AddrStatus:
      agentNetworkIpv6AddrEntryData.agentNetworkIpv6AddrStatus = D_agentNetworkIpv6AddrStatus_active;
      SET_VALID(I_agentNetworkIpv6AddrStatus, agentNetworkIpv6AddrEntryData.valid);
      break;
    default:
      return(NULL);
      break;

  }

    if (nominator >= 0 && !VALID(nominator, agentNetworkIpv6AddrEntryData.valid))
    return(NULL);

   return(&agentNetworkIpv6AddrEntryData);
  #endif
return NULL;
}
#ifdef SETS
int
k_agentNetworkIpv6AddrEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                     doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentNetworkIpv6AddrEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                      doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentNetworkIpv6AddrEntry_set_defaults(doList_t *dp)
{
    agentNetworkIpv6AddrEntry_t *data = (agentNetworkIpv6AddrEntry_t *) (dp->data);


    ZERO_VALID(data->valid);
    return NO_ERROR;
}
int
k_agentNetworkIpv6AddrEntry_set(agentNetworkIpv6AddrEntry_t *data,
                                    ContextInfo *contextInfo, int function)
{
   #if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
   L7_BOOL isCreatedNew = L7_FALSE;
   L7_in6_addr_t prefix;
   L7_char8 tempValid[sizeof(data->valid)];
   L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];

   bzero(tempValid, sizeof(tempValid));
   memset(&prefix, 0, sizeof(L7_in6_addr_t));
   prefix = *(L7_in6_addr_t *)data->agentNetworkIpv6AddrPrefix->octet_ptr;

  if( VALID(I_agentNetworkIpv6AddrStatus, data->valid))
  {
   if(data->agentNetworkIpv6AddrStatus == D_agentNetworkIpv6AddrStatus_createAndGo)
   {
   if(
       VALID(I_agentNetworkIpv6AddrPrefixLength, data->valid ) &&
       VALID(I_agentNetworkIpv6AddrEuiFlag, data->valid ))
   {
     bzero(snmp_buffer, SNMP_BUFFER_LEN);
     memcpy(snmp_buffer, data->agentNetworkIpv6AddrPrefix->octet_ptr, data->agentNetworkIpv6AddrPrefix->length);
     if(snmpAgentNetworkIpv6AddrPrefixSet(USMDB_UNIT_CURRENT,
                                              prefix,
                                              data->agentNetworkIpv6AddrPrefixLength,
                                              data->agentNetworkIpv6AddrEuiFlag) != L7_SUCCESS )
     {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return(COMMIT_FAILED_ERROR);
     }
     else
     {
       isCreatedNew = L7_TRUE;
       SET_VALID(I_agentNetworkIpv6AddrPrefix, tempValid);
     }

   }
   }
   else if(data->agentNetworkIpv6AddrStatus == D_agentNetworkIpv6AddrStatus_destroy)
   {

   }
   else if(data->agentNetworkIpv6AddrStatus != D_agentNetworkIpv6AddrStatus_active)
   {
     return COMMIT_FAILED_ERROR;
   }
  }
   return NO_ERROR;

#else
return COMMIT_FAILED_ERROR;
#endif
}
#ifdef SR_agentNetworkIpv6AddrEntry_UNDO
/* add #define SR_agentNetworkIpv6AddrEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentNetworkIpv6AddrEntry family.
 */
int
agentNetworkIpv6AddrEntry_undo(doList_t *doHead, doList_t *doCur,
                                   ContextInfo *contextInfo)
{
  #if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  agentNetworkIpv6AddrEntry_t *data = (agentNetworkIpv6AddrEntry_t *) doCur->data;
  agentNetworkIpv6AddrEntry_t *undodata = (agentNetworkIpv6AddrEntry_t *) doCur->undodata;
  agentNetworkIpv6AddrEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

   /*Copy valid bits from data to undodata */
  if( undodata != NULL && data != NULL )
    memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add, a delete, or a modify */
  if ( undodata == NULL )
  {
    /* undoing an add, so delete */
    data->agentNetworkIpv6AddrStatus = D_agentNetworkIpv6AddrStatus_destroy;
    setdata = data;
    function = SR_DELETE;
  }
  else
  {
    /* undoing an add, so delete */
    if(undodata->agentNetworkIpv6AddrStatus == D_agentNetworkIpv6AddrStatus_notReady
    || undodata->agentNetworkIpv6AddrStatus == D_agentNetworkIpv6AddrStatus_notInService)
    {
      undodata->agentNetworkIpv6AddrStatus = D_agentNetworkIpv6AddrStatus_creatandWait;
    }
    else
    {
       if(undodata->agentNetworkIpv6AddrStatus == D_agentNetworkIpv6AddrStatus_active)
      {
        undodata->agentNetworkIpv6AddrStatus = D_agentNetworkIpv6AddrStatus_createAndGo;
      }
    }

    if (data->agentNetworkIpv6AddrStatus == D_agentNetworkIpv6AddrStatus_destroy)
    {
      /* Setting all valid bits to restore previous values */
      memset(undodata->valid, 0xff, sizeof(undodata->valid));
    }

    setdata = undodata;;
    function = SR_ADD_MODIFY;
  }

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentNetworkIpv6AddrEntry_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;



#endif
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentNetworkIpv6AddrEntry_UNDO */

#endif /* SETS */

#ifdef I_agentNetworkDhcp6ADVERTISEMessagesReceived
agentNetworkStatsGroup_t *
k_agentNetworkStatsGroup_get(int serialNum, ContextInfo *contextInfo,
                             int nominator)
{
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
   static agentNetworkStatsGroup_t agentNetworkStatsGroupData;
   L7_MGMT_PORT_TYPE_t portType;

   portType = L7_MGMT_NETWORKPORT;

   switch (nominator)
   {
     case -1:
       break;
#ifdef I_agentNetworkDhcp6ADVERTISEMessagesReceived
     case I_agentNetworkDhcp6ADVERTISEMessagesReceived:
       if (usmDbDhcp6cAdvertiseReceivedGet(portType,
           &agentNetworkStatsGroupData.agentNetworkDhcp6ADVERTISEMessagesReceived) == L7_SUCCESS)
       {
         SET_VALID(I_agentNetworkDhcp6ADVERTISEMessagesReceived, agentNetworkStatsGroupData.valid);
       }
       if (nominator != -1) break;
       /* else pass through */
#endif
#ifdef I_agentNetworkDhcp6REPLYMessagesReceived
     case I_agentNetworkDhcp6REPLYMessagesReceived:
       if (usmDbDhcp6cReplyReceivedGet(portType,
           &agentNetworkStatsGroupData.agentNetworkDhcp6REPLYMessagesReceived ) == L7_SUCCESS)
       {
         SET_VALID(I_agentNetworkDhcp6REPLYMessagesReceived, agentNetworkStatsGroupData.valid);
       }
       if (nominator != -1) break;
       /* else pass through */
#endif
#ifdef I_agentNetworkDhcp6ADVERTISEMessagesDiscarded
     case I_agentNetworkDhcp6ADVERTISEMessagesDiscarded:
       if (usmDbDhcp6cAdvertiseDiscardedGet(portType,
            &agentNetworkStatsGroupData.agentNetworkDhcp6ADVERTISEMessagesDiscarded ) == L7_SUCCESS)
       {
         SET_VALID(I_agentNetworkDhcp6ADVERTISEMessagesDiscarded, agentNetworkStatsGroupData.valid);
       }
       if (nominator != -1) break;
       /* else pass through */
#endif
#ifdef I_agentNetworkDhcp6REPLYMessagesDiscarded
     case I_agentNetworkDhcp6REPLYMessagesDiscarded:
       if (usmDbDhcp6cReplyDiscardedGet(portType,
          &agentNetworkStatsGroupData.agentNetworkDhcp6REPLYMessagesDiscarded ) == L7_SUCCESS)
       {
         SET_VALID(I_agentNetworkDhcp6REPLYMessagesDiscarded, agentNetworkStatsGroupData.valid);
       }
       if (nominator != -1) break;
       /* else pass through */
#endif
#ifdef I_agentNetworkDhcp6MalformedMessagesReceived
     case I_agentNetworkDhcp6MalformedMessagesReceived:
       if (usmDbDhcp6cMalformedReceivedGet(portType, 
           &agentNetworkStatsGroupData.agentNetworkDhcp6MalformedMessagesReceived ) == L7_SUCCESS)
       {
         SET_VALID(I_agentNetworkDhcp6MalformedMessagesReceived, agentNetworkStatsGroupData.valid);
       }
       if (nominator != -1) break;
       /* else pass through */
#endif
#ifdef I_agentNetworkDhcp6SOLICITMessagesSent
     case I_agentNetworkDhcp6SOLICITMessagesSent:
       if (usmDbDhcp6cSolicitTransmittedGet(portType,
           &agentNetworkStatsGroupData.agentNetworkDhcp6SOLICITMessagesSent ) == L7_SUCCESS)
       {
         SET_VALID(I_agentNetworkDhcp6SOLICITMessagesSent, agentNetworkStatsGroupData.valid);
       }
       if (nominator != -1) break;
       /* else pass through */
#endif
#ifdef I_agentNetworkDhcp6REQUESTMessagesSent
     case I_agentNetworkDhcp6REQUESTMessagesSent:
       if (usmDbDhcp6cRequestTransmittedGet(portType,
           &agentNetworkStatsGroupData.agentNetworkDhcp6REQUESTMessagesSent ) == L7_SUCCESS)
       {
         SET_VALID(I_agentNetworkDhcp6REQUESTMessagesSent, agentNetworkStatsGroupData.valid);
       }
       if (nominator != -1) break;
       /* else pass through */
#endif
#ifdef I_agentNetworkDhcp6RENEWMessagesSent
     case I_agentNetworkDhcp6RENEWMessagesSent:
       if (usmDbDhcp6cRenewTransmittedGet (portType,
           &agentNetworkStatsGroupData.agentNetworkDhcp6RENEWMessagesSent ) == L7_SUCCESS)
       {
         SET_VALID(I_agentNetworkDhcp6RENEWMessagesSent, agentNetworkStatsGroupData.valid);
       }
       if (nominator != -1) break;
       /* else pass through */
#endif
#ifdef I_agentNetworkDhcp6REBINDMessagesSent
     case I_agentNetworkDhcp6REBINDMessagesSent:
       if(usmDbDhcp6cRebindTransmittedGet (portType,
          &agentNetworkStatsGroupData.agentNetworkDhcp6REBINDMessagesSent ) == L7_SUCCESS)
       {
         SET_VALID(I_agentNetworkDhcp6REBINDMessagesSent, agentNetworkStatsGroupData.valid);
       }
       if (nominator != -1) break;
       /* else pass through */
#endif
#ifdef I_agentNetworkDhcp6RELEASEMessagesSent
     case I_agentNetworkDhcp6RELEASEMessagesSent:  
       if (usmDbDhcp6cReleaseTransmittedGet (portType,
            &agentNetworkStatsGroupData.agentNetworkDhcp6RELEASEMessagesSent ) == L7_SUCCESS)
       {
         SET_VALID(I_agentNetworkDhcp6RELEASEMessagesSent, agentNetworkStatsGroupData.valid);
       }
       if (nominator != -1) break;
       /* else pass through */
#endif
#ifdef I_agentNetworkDhcp6StatsReset
     case I_agentNetworkDhcp6StatsReset:
       agentNetworkStatsGroupData.agentNetworkDhcp6StatsReset = D_agentNetworkDhcp6StatsReset_none;
       SET_VALID(I_agentNetworkDhcp6StatsReset, agentNetworkStatsGroupData.valid);
       if (nominator != -1) break;
       /* else pass through */
#endif
     default:
       /* unknown nominator */
       return(NULL);
       break;
   }
   if (nominator >= 0 && !VALID(nominator, agentNetworkStatsGroupData.valid))
   {
     return(NULL);
   }
   return(&agentNetworkStatsGroupData);
#else /* defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE) */
   return(NULL);
#endif /* defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE) */
}
#endif

#ifdef SETS
int
k_agentNetworkStatsGroup_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentNetworkStatsGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                               doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

#ifdef I_agentNetworkDhcp6StatsReset
int
k_agentNetworkStatsGroup_set(agentNetworkStatsGroup_t *data,
                             ContextInfo *contextInfo, int function)
{
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  L7_MGMT_PORT_TYPE_t portType;

  portType = L7_MGMT_NETWORKPORT;

#ifdef I_agentNetworkDhcp6StatsReset
   if (VALID(I_agentNetworkDhcp6StatsReset, data->valid))
   {
      if (data->agentNetworkDhcp6StatsReset == D_agentNetworkDhcp6StatsReset_reset)
      {
        if (usmDbDhcp6cStatisticsClear(portType) != L7_SUCCESS)
        {
          return COMMIT_FAILED_ERROR;
        }
      }
   }
#endif
#else
   return COMMIT_FAILED_ERROR;
#endif
   return NO_ERROR;
}
#endif

#ifdef SR_agentNetworkStatsGroup_UNDO
/* add #define SR_agentNetworkStatsGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentNetworkStatsGroup family.
 */
int
agentNetworkStatsGroup_undo(doList_t *doHead, doList_t *doCur,
                            ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentNetworkStatsGroup_UNDO */
#endif /* SETS */


#ifdef I_agentDhcpClientVendorClassIdMode
agentVendorClassOptionConfigGroup_t *
k_agentVendorClassOptionConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                                   int nominator)
{
   static agentVendorClassOptionConfigGroup_t agentVendorClassOptionConfigGroupData;
   static L7_BOOL firstTime = L7_TRUE;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
  }

   
   ZERO_VALID(agentVendorClassOptionConfigGroupData.valid);
   
   /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
    if (nominator != -1) break;
    /* else pass through */
  case I_agentDhcpClientVendorClassIdMode:
      if (snmpDhcpVendorClassIdModeGet(&agentVendorClassOptionConfigGroupData.agentDhcpClientVendorClassIdMode)
                                      == L7_SUCCESS)
      SET_VALID(I_agentDhcpClientVendorClassIdMode,
                agentVendorClassOptionConfigGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */
  case I_agentDhcpClientVendorClassIdString:
      if(snmpDhcpVendorClassIdStringGet(&agentVendorClassOptionConfigGroupData.agentDhcpClientVendorClassIdString) == L7_SUCCESS)
        SET_VALID(I_agentDhcpClientVendorClassIdString, agentVendorClassOptionConfigGroupData.valid);
      break;
      /* else pass through */
  default:
    /* unknown nominator */
    return(NULL);
    break;
  }
  if ( nominator >= 0 && !VALID(nominator, agentVendorClassOptionConfigGroupData.valid) )
    return(NULL);

   return(&agentVendorClassOptionConfigGroupData);
}

#ifdef SETS
int
k_agentVendorClassOptionConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                    doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentVendorClassOptionConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                                     doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentVendorClassOptionConfigGroup_set(agentVendorClassOptionConfigGroup_t *data,
                                   ContextInfo *contextInfo, int function)
{
  if (VALID(I_agentDhcpClientVendorClassIdMode, data->valid) &&
      snmpDhcpVendorClassIdModeSet(data->agentDhcpClientVendorClassIdMode) != L7_SUCCESS)
  {
      CLR_VALID(I_agentDhcpClientVendorClassIdMode, data->valid);
      return COMMIT_FAILED_ERROR;
  }
  if (VALID(I_agentDhcpClientVendorClassIdString, data->valid) &&
      snmpDhcpVendorClassIdStringSet(data->agentDhcpClientVendorClassIdString) != L7_SUCCESS)
  {
      CLR_VALID(I_agentDhcpClientVendorClassIdString, data->valid);
      return COMMIT_FAILED_ERROR;
  }
  return NO_ERROR;
}
#ifdef SR_agentVendorClassOptionConfigGroup_UNDO
/* add #define SR_agentVendorClassOptionConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentVendorClassOptionConfigGroup family.
 */
int
agentVendorClassOptionConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                                  ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentVendorClassOptionConfigGroup_UNDO */

#endif /* L7_SETS. */
#endif /* I_agentDhcpClientVendorClassIdMode */


agentServicePortConfigGroup_t *
k_agentServicePortConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                                  int nominator)
{
  static agentServicePortConfigGroup_t agentServicePortConfigGroupData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_SERVICE_PORT_PRESENT_ID ) == L7_FALSE )
      return(NULL);

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;

    agentServicePortConfigGroupData.agentServicePortBurnedInMacAddress = MakeOctetString(NULL, 0);
    agentServicePortConfigGroupData.agentServicePortIpv6Gateway = MakeOctetString(NULL, 0);
#ifdef I_agentServicePortDhcp6ClientDuid
    agentServicePortConfigGroupData.agentServicePortDhcp6ClientDuid = MakeOctetString(NULL, 0);
#endif
  }

  ZERO_VALID(agentServicePortConfigGroupData.valid);

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

/* lvl7_@p1261 start */
  switch (nominator)
  {
  case -1:
    if (nominator != -1) break;
    /* else pass through */

  case I_agentServicePortIPAddress :
    if (usmDbServicePortIPAddrGet(USMDB_UNIT_CURRENT, &agentServicePortConfigGroupData.agentServicePortIPAddress) == L7_SUCCESS)
      SET_VALID(I_agentServicePortIPAddress, agentServicePortConfigGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentServicePortSubnetMask :
    if (usmDbServicePortNetMaskGet(USMDB_UNIT_CURRENT, &agentServicePortConfigGroupData.agentServicePortSubnetMask) == L7_SUCCESS)
      SET_VALID(I_agentServicePortSubnetMask, agentServicePortConfigGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentServicePortDefaultGateway :
    if (usmDbServicePortGatewayGet(USMDB_UNIT_CURRENT, &agentServicePortConfigGroupData.agentServicePortDefaultGateway) == L7_SUCCESS)
      SET_VALID(I_agentServicePortDefaultGateway, agentServicePortConfigGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentServicePortBurnedInMacAddress :
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbServPortMacAddressGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetString(&agentServicePortConfigGroupData.agentServicePortBurnedInMacAddress, snmp_buffer, L7_MAC_ADDR_LEN) == L7_TRUE))
      SET_VALID(I_agentServicePortBurnedInMacAddress, agentServicePortConfigGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentServicePortConfigProtocol :
    if (snmpAgentServicePortConfigProtocolGet(USMDB_UNIT_CURRENT, &agentServicePortConfigGroupData.agentServicePortConfigProtocol) == L7_SUCCESS)
      SET_VALID(I_agentServicePortConfigProtocol, agentServicePortConfigGroupData.valid);
    if (nominator != -1)
    break;

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  case I_agentServicePortIpv6AdminMode :
    if (snmpAgentServicePortIpv6AdminModeGet(USMDB_UNIT_CURRENT,&agentServicePortConfigGroupData.agentServicePortIpv6AdminMode) == L7_SUCCESS)
      SET_VALID(I_agentServicePortIpv6AdminMode, agentServicePortConfigGroupData.valid);
    if (nominator != -1) break;
     /* else pass through */
  case I_agentServicePortIpv6Gateway :
    if (snmpAgentServicePortIpv6GatewayGet(USMDB_UNIT_CURRENT,snmp_buffer) == L7_SUCCESS &&
         (SafeMakeOctetString(&agentServicePortConfigGroupData.agentServicePortIpv6Gateway, snmp_buffer, sizeof(L7_in6_addr_t)) == L7_TRUE))
      SET_VALID(I_agentServicePortIpv6Gateway,agentServicePortConfigGroupData.valid);
    if (nominator != -1) break;
     /* else pass through */
#ifdef I_agentServicePortIpv6AddressAutoConfig
   case I_agentServicePortIpv6AddressAutoConfig:
     if (snmpAgentServicePortIPv6AutoConfigGet(&agentServicePortConfigGroupData.agentServicePortIpv6AddressAutoConfig) == L7_SUCCESS)
     {
        SET_VALID(I_agentServicePortIpv6AddressAutoConfig,agentServicePortConfigGroupData.valid);
     }
     if (nominator != -1) break;
#endif
     /* else pass through */
#ifdef I_agentServicePortIpv6ConfigProtocol
   case I_agentServicePortIpv6ConfigProtocol:
     if (snmpAgentServicePortIPv6ConfigProtocolGet(&agentServicePortConfigGroupData.agentServicePortIpv6ConfigProtocol) == L7_SUCCESS)
     {
        SET_VALID (I_agentServicePortIpv6ConfigProtocol, agentServicePortConfigGroupData.valid);
     }
     if (nominator != -1) break;
#endif
     /* else pass through */
#ifdef I_agentServicePortDhcp6ClientDuid
   case I_agentServicePortDhcp6ClientDuid:
      memset(snmp_buffer, 0, sizeof(snmp_buffer));
      if (usmDbDhcp6cDuidGet(snmp_buffer) == L7_SUCCESS)
      {
         if (SafeMakeOctetStringFromTextExact(&agentServicePortConfigGroupData.agentServicePortDhcp6ClientDuid, snmp_buffer) == L7_TRUE)
         {
            SET_VALID(I_agentServicePortDhcp6ClientDuid, agentServicePortConfigGroupData.valid);
         }
      }
      if (nominator != -1) break;
      /* else pass through */
#endif
#endif
  default:
    /* unknown nominator */
    return(NULL);
    break;
  }
/* lvl7_@p1261 end */

  if ( nominator >= 0 && !VALID(nominator, agentServicePortConfigGroupData.valid) )
    return(NULL);

  return(&agentServicePortConfigGroupData);
}

#ifdef SETS
int
k_agentServicePortConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                   doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentServicePortConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                                    doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentServicePortConfigGroup_set(agentServicePortConfigGroup_t *data,
                                  ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  #if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  #endif
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_SERVICE_PORT_PRESENT_ID ) == L7_FALSE )
      return(COMMIT_FAILED_ERROR);

  if ( VALID(I_agentServicePortConfigProtocol, data->valid))
  {
    if(snmpAgentServicePortConfigProtocolSet(USMDB_UNIT_CURRENT,
                                             data->agentServicePortConfigProtocol) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
  }

/* lvl7_@p1261 start */
  if ( VALID(I_agentServicePortIPAddress, data->valid))
  {
    if(snmpAgentServicePortIPAddressSet(USMDB_UNIT_CURRENT,
                                        data->agentServicePortIPAddress) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentServicePortIPAddress, tempValid);
    }
  }

  if ( VALID(I_agentServicePortSubnetMask, data->valid))
  {
    if(snmpAgentServicePortSubnetMaskSet(USMDB_UNIT_CURRENT,
                                         data->agentServicePortSubnetMask) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentServicePortSubnetMask, tempValid);
    }
  }
/* lvl7_@p1261 end */

  if ( VALID(I_agentServicePortDefaultGateway, data->valid))
  {
    if(snmpAgentServicePortDefaultGatewaySet(USMDB_UNIT_CURRENT,
                                             data->agentServicePortDefaultGateway) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentServicePortDefaultGateway, tempValid);
    }
  }
  #if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  if ( VALID(I_agentServicePortIpv6AdminMode, data->valid ))
  {
    if(snmpAgentServicePortIpv6AdminModeSet(USMDB_UNIT_CURRENT,data->agentServicePortIpv6AdminMode) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentServicePortIpv6AdminMode, tempValid);
    }
  }

  if ( VALID(I_agentServicePortIpv6Gateway, data->valid ))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentServicePortIpv6Gateway->octet_ptr, data->agentServicePortIpv6Gateway->length);
    if(snmpAgentServicePortIpv6GatewaySet(USMDB_UNIT_CURRENT,snmp_buffer) != L7_SUCCESS )
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentServicePortIpv6Gateway, tempValid);
    }
  }
#ifdef I_agentServicePortIpv6AddressAutoConfig
  if ( VALID(I_agentServicePortIpv6AddressAutoConfig, data->valid ))
  {
    if (snmpAgentServicePortIPv6AutoConfigSet(data->agentServicePortIpv6AddressAutoConfig) != L7_SUCCESS)
    {
      return(COMMIT_FAILED_ERROR);
    }
  }
#endif
#ifdef I_agentServicePortIpv6ConfigProtocol
  if (VALID(I_agentServicePortIpv6ConfigProtocol, data->valid))
  {
     if (snmpAgentServicePortIPv6ConfigProtocolSet(data->agentServicePortIpv6ConfigProtocol) != L7_SUCCESS)
     {
       return(COMMIT_FAILED_ERROR);
     }
  }
#endif
#endif

  return NO_ERROR;
}

  #ifdef SR_agentServicePortConfigGroup_UNDO
/* add #define SR_agentServicePortConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentServicePortConfigGroup family.
 */
int
agentServicePortConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                                 ContextInfo *contextInfo)
{
  agentServicePortConfigGroup_t *data = (agentServicePortConfigGroup_t *) doCur->data;
  agentServicePortConfigGroup_t *undodata = (agentServicePortConfigGroup_t *) doCur->undodata;
  agentServicePortConfigGroup_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if((setdata != NULL) && (k_agentServicePortConfigGroup_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentServicePortConfigGroup_UNDO */

#endif /* SETS */









agentServicePortIpv6AddrEntry_t *
k_agentServicePortIpv6AddrEntry_get(int serialNum, ContextInfo *contextInfo,
                                    int nominator,
                                    int searchType,
                                    OctetString * agentServicePortIpv6AddrPrefix
                                    )
{
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
   static agentServicePortIpv6AddrEntry_t agentServicePortIpv6AddrEntryData;
   static L7_BOOL firstTime = L7_TRUE;
   L7_in6_addr_t prefix;

   memset(&prefix, 0, sizeof(L7_in6_addr_t));

   if (firstTime == L7_TRUE)
   {
    agentServicePortIpv6AddrEntryData.agentServicePortIpv6AddrPrefix = MakeOctetString(NULL,0);
    firstTime = L7_FALSE;
   }
   ZERO_VALID(agentServicePortIpv6AddrEntryData.valid);

   memcpy(&prefix , agentServicePortIpv6AddrPrefix->octet_ptr, agentServicePortIpv6AddrPrefix->length);


   if (  (searchType == EXACT) ?
             (snmpAgentServicePortIpv6AddrEntryGet(USMDB_UNIT_CURRENT,
                                                   prefix,
                                                   &agentServicePortIpv6AddrEntryData.agentServicePortIpv6AddrPrefixLength,
                                                   agentServicePortIpv6AddrEntryData.agentServicePortIpv6AddrEuiFlag) != L7_SUCCESS) :
             ((snmpAgentServicePortIpv6AddrEntryGet(USMDB_UNIT_CURRENT,
                                                   prefix,
                                                   &agentServicePortIpv6AddrEntryData.agentServicePortIpv6AddrPrefixLength,
                                                   agentServicePortIpv6AddrEntryData.agentServicePortIpv6AddrEuiFlag) != L7_SUCCESS) &&
             (snmpAgentServicePortIpv6AddrEntryNext(USMDB_UNIT_CURRENT,
                                                    &prefix,
                                                    &agentServicePortIpv6AddrEntryData.agentServicePortIpv6AddrPrefixLength,
                                                    &agentServicePortIpv6AddrEntryData.agentServicePortIpv6AddrEuiFlag) != L7_SUCCESS)))

   {
     ZERO_VALID(agentServicePortIpv6AddrEntryData.valid);
     return(NULL);
   }

  if(SafeMakeOctetString(&agentServicePortIpv6AddrEntryData.agentServicePortIpv6AddrPrefix, (L7_uchar8 *)&prefix, sizeof(L7_in6_addr_t)) != L7_TRUE)
  {
    ZERO_VALID(agentServicePortIpv6AddrEntryData.valid);
    return(NULL);
  }
  else
  {
     SET_VALID(I_agentServicePortIpv6AddrPrefix,agentServicePortIpv6AddrEntryData.valid);
  }
  switch(nominator)
  {
    case -1:
    case I_agentServicePortIpv6AddrPrefix:
    case I_agentServicePortIpv6AddrEuiFlag:
    if ( nominator != -1 ) break;

    case I_agentServicePortIpv6AddrPrefixLength:
      SET_VALID(I_agentServicePortIpv6AddrPrefixLength, agentServicePortIpv6AddrEntryData.valid);
      if ( nominator != -1 ) break;

    case I_agentServicePortIpv6AddrStatus:
      agentServicePortIpv6AddrEntryData.agentServicePortIpv6AddrStatus = D_agentServicePortIpv6AddrStatus_active;
      SET_VALID(I_agentServicePortIpv6AddrStatus, agentServicePortIpv6AddrEntryData.valid);
      break;

    default:
      return(NULL);
      break;

  }

    if (nominator >= 0 && !VALID(nominator, agentServicePortIpv6AddrEntryData.valid))
    return(NULL);

   return(&agentServicePortIpv6AddrEntryData);
#endif
return NULL;
}
#ifdef SETS
int
k_agentServicePortIpv6AddrEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                     doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentServicePortIpv6AddrEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                      doList_t *doHead, doList_t *dp)
{

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
#else
return COMMIT_FAILED_ERROR;
#endif
}

int
k_agentServicePortIpv6AddrEntry_set_defaults(doList_t *dp)
{
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
    agentServicePortIpv6AddrEntry_t *data = (agentServicePortIpv6AddrEntry_t *) (dp->data);


    ZERO_VALID(data->valid);

    return NO_ERROR;
#else
return COMMIT_FAILED_ERROR;
#endif
}

int
k_agentServicePortIpv6AddrEntry_set(agentServicePortIpv6AddrEntry_t *data,
                                    ContextInfo *contextInfo, int function)
{
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
   L7_BOOL isCreatedNew = L7_FALSE;
   L7_in6_addr_t prefix;
   L7_char8 tempValid[sizeof(data->valid)];
   L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
   L7_uchar8 str_addr[64];
   bzero(tempValid, sizeof(tempValid));
   memset(&prefix, 0, sizeof(L7_in6_addr_t));
   prefix = *(L7_in6_addr_t *)data->agentServicePortIpv6AddrPrefix->octet_ptr;
   if( VALID(I_agentServicePortIpv6AddrStatus, data->valid))
   {
     if(data->agentServicePortIpv6AddrStatus == D_agentServicePortIpv6AddrStatus_createAndGo)
     {
       if(VALID(I_agentServicePortIpv6AddrPrefixLength, data->valid ) &&
          VALID(I_agentServicePortIpv6AddrEuiFlag, data->valid ))
       {
         bzero(snmp_buffer, SNMP_BUFFER_LEN);
         memcpy(snmp_buffer, data->agentServicePortIpv6AddrPrefix->octet_ptr, data->agentServicePortIpv6AddrPrefix->length);
         if(snmpAgentServicePortIpv6AddrPrefixSet(USMDB_UNIT_CURRENT,
                                              prefix,
                                              data->agentServicePortIpv6AddrPrefixLength,
                                              data->agentServicePortIpv6AddrEuiFlag) != L7_SUCCESS )
         {
           memcpy(data->valid, tempValid, sizeof(data->valid));
           return(COMMIT_FAILED_ERROR);
         }
         else
         {
           isCreatedNew = L7_TRUE;
           SET_VALID(I_agentServicePortIpv6AddrPrefix, tempValid);
         }
       }
     }
     else if(data->agentServicePortIpv6AddrStatus == D_agentServicePortIpv6AddrStatus_destroy)
     {
       bzero(str_addr,64);
       osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)&prefix,str_addr,IPV6_DISP_ADDR_LEN);
       if(usmDbServicePortIPV6PrefixRemove(&prefix,data->agentServicePortIpv6AddrPrefixLength,0)!=L7_SUCCESS)
         return COMMIT_FAILED_ERROR;
     }
     else if(data->agentServicePortIpv6AddrStatus != D_agentServicePortIpv6AddrStatus_active)
     {
       return COMMIT_FAILED_ERROR;
     }
   }

   return NO_ERROR;
#else
   return COMMIT_FAILED_ERROR;
#endif
}

#ifdef SR_agentServicePortIpv6AddrEntry_UNDO
/* add #define SR_agentServicePortIpv6AddrEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentServicePortIpv6AddrEntry family.
 */
int
agentServicePortIpv6AddrEntry_undo(doList_t *doHead, doList_t *doCur,
                                   ContextInfo *contextInfo)
{
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  agentServicePortIpv6AddrEntry_t *data = (agentServicePortIpv6AddrEntry_t *) doCur->data;
  agentServicePortIpv6AddrEntry_t *undodata = (agentServicePortIpv6AddrEntry_t *) doCur->undodata;
  agentServicePortIpv6AddrEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

   /*Copy valid bits from data to undodata */
  if( undodata != NULL && data != NULL )
    memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add, a delete, or a modify */
  if ( undodata == NULL )
  {
    /* undoing an add, so delete */
    data->agentServicePortIpv6AddrStatus = D_agentServicePortIpv6AddrStatus_destroy;
    setdata = data;
    function = SR_DELETE;
  }
  else
  {
    /* undoing an add, so delete */
    if(undodata->agentServicePortIpv6AddrStatus == D_agentServicePortIpv6AddrStatus_notReady
    || undodata->agentServicePortIpv6AddrStatus == D_agentServicePortIpv6AddrStatus_notInService)
    {
      undodata->agentServicePortIpv6AddrStatus = D_agentServicePortIpv6AddrStatus_creatandWait;
    }
    else
    {
       if(undodata->agentServicePortIpv6AddrStatus == D_agentServicePortIpv6AddrStatus_active)
      {
        undodata->agentServicePortIpv6AddrStatus = D_agentServicePortIpv6AddrStatus_createAndGo;
      }
    }

    if (data->agentServicePortIpv6AddrStatus == D_agentServicePortIpv6AddrStatus_destroy)
    {
      /* Setting all valid bits to restore previous values */
      memset(undodata->valid, 0xff, sizeof(undodata->valid));
    }

    setdata = undodata;;
    function = SR_ADD_MODIFY;
  }

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentServicePortIpv6AddrEntry_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;




#endif
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentServicePortIpv6AddrEntry_UNDO */

#endif /* SETS */

#ifdef I_agentServicePortDhcp6ADVERTISEMessagesReceived
agentServicePortStatsGroup_t *
k_agentServicePortStatsGroup_get(int serialNum, ContextInfo *contextInfo,
                                 int nominator)
{
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
   static agentServicePortStatsGroup_t agentServicePortStatsGroupData;
   L7_MGMT_PORT_TYPE_t portType;
   
   portType = L7_MGMT_SERVICEPORT;

   switch (nominator)
   {
     case -1:
       if (nominator != -1) break;
       /* else pass through */
#ifdef I_agentServicePortDhcp6ADVERTISEMessagesReceived
     case I_agentServicePortDhcp6ADVERTISEMessagesReceived:
       if (usmDbDhcp6cAdvertiseReceivedGet (portType, 
          &agentServicePortStatsGroupData.agentServicePortDhcp6ADVERTISEMessagesReceived)== L7_SUCCESS)
       {
         SET_VALID(I_agentServicePortDhcp6ADVERTISEMessagesReceived, agentServicePortStatsGroupData.valid);
       }
       if (nominator != -1) break;
#endif 
       /* else pass through */
#ifdef I_agentServicePortDhcp6REPLYMessagesReceived
     case I_agentServicePortDhcp6REPLYMessagesReceived:
       if (usmDbDhcp6cReplyReceivedGet(portType, 
          &agentServicePortStatsGroupData.agentServicePortDhcp6REPLYMessagesReceived) == L7_SUCCESS)
       {
         SET_VALID(I_agentServicePortDhcp6REPLYMessagesReceived, agentServicePortStatsGroupData.valid);
       }
       if (nominator != -1) break;
#endif
       /* else pass through */
#ifdef I_agentServicePortDhcp6ADVERTISEMessagesDiscarded
     case I_agentServicePortDhcp6ADVERTISEMessagesDiscarded:
       if (usmDbDhcp6cAdvertiseDiscardedGet (portType, 
           &agentServicePortStatsGroupData.agentServicePortDhcp6ADVERTISEMessagesDiscarded) == L7_SUCCESS)
       {
         SET_VALID(I_agentServicePortDhcp6ADVERTISEMessagesDiscarded, agentServicePortStatsGroupData.valid);
       }
       if (nominator != -1) break;
#endif
       /* else pass through */
#ifdef I_agentServicePortDhcp6REPLYMessagesDiscarded
     case I_agentServicePortDhcp6REPLYMessagesDiscarded:
       if (usmDbDhcp6cReplyDiscardedGet (portType, 
           &agentServicePortStatsGroupData.agentServicePortDhcp6REPLYMessagesDiscarded) == L7_SUCCESS)
       {
         SET_VALID(I_agentServicePortDhcp6REPLYMessagesDiscarded, agentServicePortStatsGroupData.valid);
       }
       if (nominator != -1) break;
#endif
       /* else pass through */
#ifdef I_agentServicePortDhcp6MalformedMessagesReceived
     case I_agentServicePortDhcp6MalformedMessagesReceived:
       if (usmDbDhcp6cMalformedReceivedGet (portType,
          &agentServicePortStatsGroupData.agentServicePortDhcp6MalformedMessagesReceived) == L7_SUCCESS)
       {
         SET_VALID(I_agentServicePortDhcp6MalformedMessagesReceived, agentServicePortStatsGroupData.valid); 
       }
       if (nominator != -1) break;
#endif
       /* else pass through */
#ifdef I_agentServicePortDhcp6SOLICITMessagesSent
     case I_agentServicePortDhcp6SOLICITMessagesSent:
       if (usmDbDhcp6cSolicitTransmittedGet (portType, 
           &agentServicePortStatsGroupData.agentServicePortDhcp6SOLICITMessagesSent) == L7_SUCCESS)
       {
         SET_VALID(I_agentServicePortDhcp6SOLICITMessagesSent, agentServicePortStatsGroupData.valid);
       }
       if (nominator != -1) break;
#endif
       /* else pass through */
#ifdef I_agentServicePortDhcp6REQUESTMessagesSent
     case I_agentServicePortDhcp6REQUESTMessagesSent:
       if (usmDbDhcp6cRequestTransmittedGet (portType, 
           &agentServicePortStatsGroupData.agentServicePortDhcp6REQUESTMessagesSent)== L7_SUCCESS)
       {
         SET_VALID(I_agentServicePortDhcp6REQUESTMessagesSent, agentServicePortStatsGroupData.valid);
       }
       if (nominator != -1) break;
#endif
       /* else pass through */
#ifdef I_agentServicePortDhcp6RENEWMessagesSent
     case I_agentServicePortDhcp6RENEWMessagesSent:
       if (usmDbDhcp6cRenewTransmittedGet(portType, 
           &agentServicePortStatsGroupData.agentServicePortDhcp6RENEWMessagesSent) == L7_SUCCESS)
       {
         SET_VALID(I_agentServicePortDhcp6RENEWMessagesSent, agentServicePortStatsGroupData.valid);
       }
       if (nominator != -1) break;
#endif
       /* else pass through */
#ifdef I_agentServicePortDhcp6REBINDMessagesSent
     case I_agentServicePortDhcp6REBINDMessagesSent:
       if (usmDbDhcp6cRebindTransmittedGet(portType, 
           &agentServicePortStatsGroupData.agentServicePortDhcp6REBINDMessagesSent) == L7_SUCCESS)
       {
         SET_VALID(I_agentServicePortDhcp6REBINDMessagesSent, agentServicePortStatsGroupData.valid);
       }
       if (nominator != -1) break;
#endif
       /* else pass through */
#ifdef I_agentServicePortDhcp6RELEASEMessagesSent
     case I_agentServicePortDhcp6RELEASEMessagesSent:
       if (usmDbDhcp6cReleaseTransmittedGet (portType,
           &agentServicePortStatsGroupData.agentServicePortDhcp6RELEASEMessagesSent) == L7_SUCCESS)
       {
         SET_VALID(I_agentServicePortDhcp6RELEASEMessagesSent, agentServicePortStatsGroupData.valid);
       }
       if (nominator != -1) break;
#endif
       /* else pass through */
#ifdef I_agentServicePortDhcp6StatsReset
     case I_agentServicePortDhcp6StatsReset:
       agentServicePortStatsGroupData.agentServicePortDhcp6StatsReset = D_agentServicePortDhcp6StatsReset_none;
       SET_VALID(I_agentServicePortDhcp6StatsReset, agentServicePortStatsGroupData.valid);
       if (nominator != -1) break;
#endif
       /* else pass through */

     default:
       /* unknown nominator */
      return(NULL);
   }
   if (nominator >= 0 && !VALID(nominator, agentServicePortStatsGroupData.valid))
   {
     return(NULL);
   }
   return(&agentServicePortStatsGroupData);
#else /* defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE) */
   return(NULL);
#endif /* defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE) */
}
#endif

#ifdef SETS
int
k_agentServicePortStatsGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                  doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentServicePortStatsGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                                   doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

#ifdef I_agentServicePortDhcp6StatsReset
int
k_agentServicePortStatsGroup_set(agentServicePortStatsGroup_t *data,
                                 ContextInfo *contextInfo, int function)
{
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  L7_MGMT_PORT_TYPE_t portType;

  portType = L7_MGMT_SERVICEPORT;
#ifdef I_agentServicePortDhcp6StatsReset
   if (VALID(I_agentServicePortDhcp6StatsReset, data->valid))
   {
      if (data->agentServicePortDhcp6StatsReset == D_agentServicePortDhcp6StatsReset_reset)
      {
        if (usmDbDhcp6cStatisticsClear(portType) != L7_SUCCESS)
        {
          return COMMIT_FAILED_ERROR;
        }
      }
   }
#endif
#else
   return COMMIT_FAILED_ERROR;
#endif
   return NO_ERROR;
}
#endif

#ifdef SR_agentServicePortStatsGroup_UNDO
/* add #define SR_agentServicePortStatsGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentServicePortStatsGroup family.
 */
int
agentServicePortStatsGroup_undo(doList_t *doHead, doList_t *doCur,
                                ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentServicePortStatsGroup_UNDO */

#endif /* SETS */

#ifdef I_agentSnmpCommunityCreate
agentSnmpConfigGroup_t *
k_agentSnmpConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                           int nominator)
{
  static agentSnmpConfigGroup_t agentSnmpConfigGroupData;
  static L7_BOOL firstTime = L7_TRUE;

  if (firstTime == L7_TRUE)
  {
    agentSnmpConfigGroupData.agentSnmpCommunityCreate = MakeOctetString(NULL, 0);
    agentSnmpConfigGroupData.agentSnmpTrapReceiverCreate = MakeOctetString(NULL, 0);
    firstTime = L7_FALSE;
  }

  ZERO_VALID(agentSnmpConfigGroupData.valid);
  SET_VALID(I_agentSnmpCommunityCreate, agentSnmpConfigGroupData.valid);
  SET_VALID(I_agentSnmpTrapReceiverCreate, agentSnmpConfigGroupData.valid);

  return(&agentSnmpConfigGroupData);
}

#ifdef SETS
int
k_agentSnmpConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentSnmpConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                             doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentSnmpConfigGroup_set(agentSnmpConfigGroup_t *data,
                           ContextInfo *contextInfo, int function)
{
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));


  if (VALID(I_agentSnmpCommunityCreate, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentSnmpCommunityCreate->octet_ptr, data->agentSnmpCommunityCreate->length);
    if (snmpAgentSnmpCommunityCreateSet(USMDB_UNIT_CURRENT, snmp_buffer) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      creationErrorSnmpCommunity = L7_TRUE;
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentSnmpCommunityCreate, tempValid);
    }
  }

  if (VALID(I_agentSnmpTrapReceiverCreate, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentSnmpTrapReceiverCreate->octet_ptr, data->agentSnmpTrapReceiverCreate->length);
    if (snmpAgentSnmpTrapReceiverCreateSet(USMDB_UNIT_CURRENT, snmp_buffer, snmpTrapReceiverIndex) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      creationErrorSnmpTrapReceiver = L7_TRUE;
      return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;
}

  #ifdef SR_agentSnmpConfigGroup_UNDO
/* add #define SR_agentSnmpConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentSnmpConfigGroup family.
 */
int
agentSnmpConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                          ContextInfo *contextInfo)
{
  agentSnmpConfigGroup_t *data = (agentSnmpConfigGroup_t *) doCur->data;

  L7_int32 function = SR_UNKNOWN;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_int32 rc = L7_FAILURE;
  L7_int32 tempInt;
  L7_int32 agentSnmpCommunityTempIndex = 0; /* Temporary Index */
  L7_int32 agentSnmpTrapReceiverTempIndex; /* Temporary Index */

  /* Definition for Implementation of Tables created from leaf objects outside the table */
  snmpCfgData_t snmpCfgData;

  /* Define an Snmp Community table Entry*/
  agentSnmpCommunityConfigEntry_t agentSnmpCommunityConfigEntryData;
  agentSnmpCommunityConfigEntry_t *snmpCommunityConfigEntrysetdata = &agentSnmpCommunityConfigEntryData;

  /* Define an Snmp Trap Receiver Entry */
  agentSnmpTrapReceiverConfigEntry_t agentSnmpTrapReceiverConfigEntryData;
  agentSnmpTrapReceiverConfigEntry_t *trapReceiverEntrysetdata = &agentSnmpTrapReceiverConfigEntryData;

  /* Initialize the Snmp Community Entry */
  agentSnmpCommunityConfigEntryData.agentSnmpCommunityName = MakeOctetString(NULL, 0);
  ZERO_VALID(agentSnmpCommunityConfigEntryData.valid);

  /* Initialize the Snmp Trap Receiver Entry */
  agentSnmpTrapReceiverConfigEntryData.agentSnmpTrapReceiverCommunityName = MakeOctetString(NULL, 0);
  ZERO_VALID(agentSnmpTrapReceiverConfigEntryData.valid);

  if ( creationErrorSnmpCommunity == L7_TRUE )
  {
    creationErrorSnmpCommunity = L7_FALSE;
    CLR_VALID(I_agentSnmpCommunityCreate, data->valid);
  }

  if ( creationErrorSnmpTrapReceiver == L7_TRUE )
  {
    creationErrorSnmpTrapReceiver = L7_FALSE;
    CLR_VALID(I_agentSnmpTrapReceiverCreate, data->valid);
  }

  if( data == NULL )
    return UNDO_FAILED_ERROR;

  if( VALID(I_agentSnmpCommunityCreate, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    strncpy(snmp_buffer, data->agentSnmpCommunityCreate->octet_ptr, data->agentSnmpCommunityCreate->length);

    /* Making sure that we get a NULL Terminated string */
    snmp_buffer[data->agentSnmpCommunityCreate->length] = '\0';

    /* Get the snmp community index from the community name of the created row */
    for (tempInt = 0; tempInt < L7_MAX_SNMP_COMM; tempInt++)
    {
      if (strcmp( snmpCfgData.snmpComm[tempInt].agentCommunityName, snmp_buffer) == L7_SUCCESS)
      {
        rc = L7_SUCCESS;
        agentSnmpCommunityTempIndex = tempInt;
        break;
      }
    }
    if (rc != L7_SUCCESS)
      return UNDO_FAILED_ERROR;

    /* we are trying to undo an add to the Table
     * undoing an add, so delete
    */
    agentSnmpCommunityConfigEntryData.agentSnmpCommunityStatus = D_agentSnmpCommunityStatus_destroy;
    agentSnmpCommunityConfigEntryData.agentSnmpCommunityIndex = agentSnmpCommunityTempIndex;
    SET_VALID(I_agentSnmpCommunityStatus, agentSnmpCommunityConfigEntryData.valid);
    function = SR_DELETE;

    /* use the set method for the undo */
    if ((snmpCommunityConfigEntrysetdata == NULL) ||
          k_agentSnmpCommunityConfigEntry_set(snmpCommunityConfigEntrysetdata,contextInfo, function) != NO_ERROR)
      return UNDO_FAILED_ERROR;
  }
  if( VALID(I_agentSnmpTrapReceiverCreate, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    strncpy(snmp_buffer, data->agentSnmpTrapReceiverCreate->octet_ptr, data->agentSnmpTrapReceiverCreate->length);

    /*Make sure that we get a NULL Terminated string */
    snmp_buffer[data->agentSnmpTrapReceiverCreate->length] = '\0';

    if ( snmpTrapReceiverIndex != NULL )
    {
      /* Newly created row's index */
      agentSnmpTrapReceiverTempIndex = *snmpTrapReceiverIndex;
    }
    else
    {
      return UNDO_FAILED_ERROR;
    }

    /* we are trying to undo an add to the Table
     * undoing an add, so delete
    */
    agentSnmpTrapReceiverConfigEntryData.agentSnmpTrapReceiverStatus = D_agentSnmpTrapReceiverStatus_destroy;
    agentSnmpTrapReceiverConfigEntryData.agentSnmpTrapReceiverIndex = agentSnmpTrapReceiverTempIndex;
    SET_VALID(I_agentSnmpTrapReceiverStatus, agentSnmpTrapReceiverConfigEntryData.valid);
    function = SR_DELETE;

    /* use the set method for the undo */
    if ((trapReceiverEntrysetdata == NULL) ||
           k_agentSnmpTrapReceiverConfigEntry_set(trapReceiverEntrysetdata, contextInfo, function) != NO_ERROR)
      return UNDO_FAILED_ERROR;
  }

  return NO_ERROR;
}
  #endif /* SR_agentSnmpConfigGroup_UNDO */

#endif /* SETS */
#endif /* I_agentSnmpCommunityCreate */

#ifdef I_agentSnmpCommunityIndex
agentSnmpCommunityConfigEntry_t *
k_agentSnmpCommunityConfigEntry_get(int serialNum, ContextInfo *contextInfo,
                                    int nominator,
                                    int searchType,
                                    SR_INT32 agentSnmpCommunityIndex)
{
  static agentSnmpCommunityConfigEntry_t agentSnmpCommunityConfigEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;

    agentSnmpCommunityConfigEntryData.agentSnmpCommunityName = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(agentSnmpCommunityConfigEntryData.valid);
  agentSnmpCommunityConfigEntryData.agentSnmpCommunityIndex = agentSnmpCommunityIndex;
  SET_VALID(I_agentSnmpCommunityIndex, agentSnmpCommunityConfigEntryData.valid);

  if ( (searchType == EXACT) ?
       (usmDbSnmpCommunityEntryGet(USMDB_UNIT_CURRENT, agentSnmpCommunityConfigEntryData.agentSnmpCommunityIndex) != L7_SUCCESS) :
       ( (usmDbSnmpCommunityEntryGet(USMDB_UNIT_CURRENT, agentSnmpCommunityConfigEntryData.agentSnmpCommunityIndex) != L7_SUCCESS) &&
         (usmDbSnmpCommunityEntryNext(USMDB_UNIT_CURRENT, &agentSnmpCommunityConfigEntryData.agentSnmpCommunityIndex) != L7_SUCCESS) ) )
  {
    ZERO_VALID(agentSnmpCommunityConfigEntryData.valid);
    return(NULL);
  }

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentSnmpCommunityName :
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbSnmpCommunityNameGet(USMDB_UNIT_CURRENT, agentSnmpCommunityConfigEntryData.agentSnmpCommunityIndex, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentSnmpCommunityConfigEntryData.agentSnmpCommunityName, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentSnmpCommunityName, agentSnmpCommunityConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentSnmpCommunityIPAddress :
    if (usmDbSnmpCommunityIpAddrGet(USMDB_UNIT_CURRENT, agentSnmpCommunityConfigEntryData.agentSnmpCommunityIndex,
                                    &agentSnmpCommunityConfigEntryData.agentSnmpCommunityIPAddress) == L7_SUCCESS)
      SET_VALID(I_agentSnmpCommunityIPAddress, agentSnmpCommunityConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentSnmpCommunityIPMask :
    if (usmDbSnmpCommunityIpMaskGet(USMDB_UNIT_CURRENT, agentSnmpCommunityConfigEntryData.agentSnmpCommunityIndex,
                                    &agentSnmpCommunityConfigEntryData.agentSnmpCommunityIPMask) == L7_SUCCESS)
      SET_VALID(I_agentSnmpCommunityIPMask, agentSnmpCommunityConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentSnmpCommunityAccessMode :
    if (snmpAgentSnmpCommunityAccessModeGet(USMDB_UNIT_CURRENT, agentSnmpCommunityConfigEntryData.agentSnmpCommunityIndex,
                                            &agentSnmpCommunityConfigEntryData.agentSnmpCommunityAccessMode) == L7_SUCCESS)
      SET_VALID(I_agentSnmpCommunityAccessMode, agentSnmpCommunityConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentSnmpCommunityStatus :
    if (snmpAgentSnmpCommunityStatusGet(USMDB_UNIT_CURRENT, agentSnmpCommunityConfigEntryData.agentSnmpCommunityIndex,
                                        &agentSnmpCommunityConfigEntryData.agentSnmpCommunityStatus) == L7_SUCCESS)
      SET_VALID(I_agentSnmpCommunityStatus, agentSnmpCommunityConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentSnmpCommunityIndex:
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentSnmpCommunityConfigEntryData.valid) )
    return(NULL);

  return(&agentSnmpCommunityConfigEntryData);
}

#ifdef SETS
int
k_agentSnmpCommunityConfigEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                     doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentSnmpCommunityConfigEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                      doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentSnmpCommunityConfigEntry_set_defaults(doList_t *dp)
{
  agentSnmpCommunityConfigEntry_t *data = (agentSnmpCommunityConfigEntry_t *) (dp->data);

  if ((data->agentSnmpCommunityName = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }

  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_agentSnmpCommunityConfigEntry_set(agentSnmpCommunityConfigEntry_t *data,
                                    ContextInfo *contextInfo, int function)
{
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (usmDbSnmpCommunityEntryGet(USMDB_UNIT_CURRENT, data->agentSnmpCommunityIndex) != L7_SUCCESS)
  {
    ZERO_VALID(data->valid);
    return(COMMIT_FAILED_ERROR);
  }

  if (data->agentSnmpCommunityName != NULL)
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentSnmpCommunityName->octet_ptr, data->agentSnmpCommunityName->length);
    if ( VALID(I_agentSnmpCommunityName, data->valid))
    {
      if( usmDbSnmpCommunityNameSet(USMDB_UNIT_CURRENT, data->agentSnmpCommunityIndex,
                                   snmp_buffer) != L7_SUCCESS )
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
      else
      {
        SET_VALID(I_agentSnmpCommunityName, tempValid);
      }
    }
  }

  if ( VALID(I_agentSnmpCommunityIPAddress, data->valid))
  {
    if( usmDbSnmpCommunityIpAddrSet(USMDB_UNIT_CURRENT, data->agentSnmpCommunityIndex,
                                   data->agentSnmpCommunityIPAddress) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentSnmpCommunityIPAddress, tempValid);
    }
  }

  if ( VALID(I_agentSnmpCommunityIPMask, data->valid))
  {
    if( usmDbSnmpCommunityIpMaskSet(USMDB_UNIT_CURRENT, data->agentSnmpCommunityIndex,
                                   data->agentSnmpCommunityIPMask) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentSnmpCommunityIPMask, tempValid);
    }
  }

  if ( VALID(I_agentSnmpCommunityAccessMode, data->valid))
  {
    if( snmpAgentSnmpCommunityAccessModeSet(USMDB_UNIT_CURRENT, data->agentSnmpCommunityIndex,
                                           data->agentSnmpCommunityAccessMode) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentSnmpCommunityAccessMode, tempValid);
    }
  }

  if ( VALID(I_agentSnmpCommunityStatus, data->valid))
  {
    if( snmpAgentSnmpCommunityStatusSet(USMDB_UNIT_CURRENT, data->agentSnmpCommunityIndex,
                                       data->agentSnmpCommunityStatus) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
  }

  return NO_ERROR;
}

  #ifdef SR_agentSnmpCommunityConfigEntry_UNDO
/* add #define SR_agentSnmpCommunityConfigEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentSnmpCommunityConfigEntry family.
 */
int
agentSnmpCommunityConfigEntry_undo(doList_t *doHead, doList_t *doCur,
                                   ContextInfo *contextInfo)
{
  agentSnmpCommunityConfigEntry_t *data = (agentSnmpCommunityConfigEntry_t *) doCur->data;
  agentSnmpCommunityConfigEntry_t *undodata = (agentSnmpCommunityConfigEntry_t *) doCur->undodata;
  agentSnmpCommunityConfigEntry_t *entrysetdata = NULL;

  L7_int32 function = SR_UNKNOWN;
  L7_int32 rc = L7_FAILURE;
  L7_int32 agentSnmpCommunityTempIndex = 0; /* Temporary Index */
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_int32 tempInt;

  /* Definition for Implementation of Tables created from leaf objects outside the table */
  snmpCfgData_t snmpCfgData;

  /* Define a group instance so as to create the deleted row */
  agentSnmpConfigGroup_t agentSnmpConfigGroupTempdata;
  agentSnmpConfigGroup_t *setdata = &agentSnmpConfigGroupTempdata;

  /* Initialize this group instance */
  agentSnmpConfigGroupTempdata.agentSnmpCommunityCreate = CloneOctetString(undodata->agentSnmpCommunityName);
  ZERO_VALID(agentSnmpConfigGroupTempdata.valid);

  /*Copy valid bits from data to undodata */
  if( undodata != NULL && data != NULL )
    memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /*
   * we are either trying to undo a delete, or a modify
   * undoing a delete or modify, replace the original data
   */
  if(data->agentSnmpCommunityStatus ==  D_agentSnmpCommunityStatus_destroy)
  {
        /* check that the entry was actually deleted */
    if (usmDbSnmpCommunityEntryGet(USMDB_UNIT_CURRENT,
                                   undodata->agentSnmpCommunityIndex) == L7_SUCCESS)
      return NO_ERROR;

    /* Use the set method for the creation of the row with default values */
    SET_VALID(I_agentSnmpCommunityCreate, agentSnmpConfigGroupTempdata.valid);
    function = SR_ADD_MODIFY;
    if ((setdata != NULL) && k_agentSnmpConfigGroup_set(setdata, contextInfo, function) == NO_ERROR)
    {
      bzero(snmp_buffer, SNMP_BUFFER_LEN);
      strncpy(snmp_buffer, undodata->agentSnmpCommunityName->octet_ptr,
                                              undodata->agentSnmpCommunityName->length);

      /* Making sure that we get a NULL Terminated string */
      snmp_buffer[undodata->agentSnmpCommunityName->length] = '\0';

      /* Get the snmp community index from the community name of the created row */
      for (tempInt = 0; tempInt < L7_MAX_SNMP_COMM; tempInt++)
      {
        if (strcmp( snmpCfgData.snmpComm[tempInt].agentCommunityName, snmp_buffer) == L7_SUCCESS)
        {
          rc = L7_SUCCESS;
          agentSnmpCommunityTempIndex = tempInt;
          break;
        }
      }
      if (rc != L7_SUCCESS)
        return UNDO_FAILED_ERROR;

      undodata->agentSnmpCommunityIndex = agentSnmpCommunityTempIndex;

      /* set all valid for the setting the previous values to the created row */
      memset(undodata->valid, 0xff, sizeof(undodata->valid));

      /* Status should not be set again */
      CLR_VALID(I_agentSnmpCommunityStatus, undodata->valid);

      entrysetdata = undodata;

      /* use the set method for the undo */
      if ((entrysetdata != NULL)
           && k_agentSnmpCommunityConfigEntry_set(entrysetdata,
                                                  contextInfo, function) == NO_ERROR)
      {
        free(agentSnmpConfigGroupTempdata.agentSnmpCommunityCreate);
        return NO_ERROR;
      }
    }
  }
  else
  {
    /* we are trying to undo a modify to the Table */
    if(undodata->agentSnmpCommunityStatus == D_agentSnmpCommunityStatus_notInService)
    {
      undodata->agentSnmpCommunityStatus = D_agentSnmpCommunityStatus_config;
    }
    else
    {
      if(undodata->agentSnmpCommunityStatus == D_agentSnmpCommunityStatus_active)
      {
        undodata->agentSnmpCommunityStatus = D_agentSnmpCommunityStatus_config;
      }
    }
    function = SR_ADD_MODIFY;
    entrysetdata = undodata;

    /* use the set method for the undo */
    if ((entrysetdata != NULL) && (k_agentSnmpCommunityConfigEntry_set(entrysetdata, contextInfo, function) == NO_ERROR))
    {
      free(agentSnmpConfigGroupTempdata.agentSnmpCommunityCreate);
      return NO_ERROR;
    }
  }

  free(agentSnmpConfigGroupTempdata.agentSnmpCommunityCreate);
  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentSnmpCommunityConfigEntry_UNDO */

#endif /* SETS */
#endif /* I_agentSnmpCommunityIndex */


#ifdef I_agentSnmpTrapReceiverIndex
agentSnmpTrapReceiverConfigEntry_t *
k_agentSnmpTrapReceiverConfigEntry_get(int serialNum, ContextInfo *contextInfo,
                                       int nominator,
                                       int searchType,
                                       SR_INT32 agentSnmpTrapReceiverIndex)
{
  static agentSnmpTrapReceiverConfigEntry_t agentSnmpTrapReceiverConfigEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;

    agentSnmpTrapReceiverConfigEntryData.agentSnmpTrapReceiverCommunityName = MakeOctetString(NULL, 0);
    agentSnmpTrapReceiverConfigEntryData.agentSnmpTrapReceiverIpv6Address = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(agentSnmpTrapReceiverConfigEntryData.valid);
  agentSnmpTrapReceiverConfigEntryData.agentSnmpTrapReceiverIndex = agentSnmpTrapReceiverIndex;
  SET_VALID(I_agentSnmpCommunityIndex, agentSnmpTrapReceiverConfigEntryData.valid);

  if ( (searchType == EXACT) ?
       (usmDbTrapManagerEntryIpGet (USMDB_UNIT_CURRENT, agentSnmpTrapReceiverConfigEntryData.agentSnmpTrapReceiverIndex) != L7_SUCCESS) :
       ( (usmDbTrapManagerEntryIpGet (USMDB_UNIT_CURRENT, agentSnmpTrapReceiverConfigEntryData.agentSnmpTrapReceiverIndex) != L7_SUCCESS) &&
         (usmDbTrapManagerEntryIpNext (USMDB_UNIT_CURRENT, &agentSnmpTrapReceiverConfigEntryData.agentSnmpTrapReceiverIndex) != L7_SUCCESS) ) )
    return(NULL);

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
  case I_agentSnmpTrapReceiverIndex:
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentSnmpTrapReceiverCommunityName :
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbTrapManagerCommIpGet(USMDB_UNIT_CURRENT, agentSnmpTrapReceiverConfigEntryData.agentSnmpTrapReceiverIndex, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentSnmpTrapReceiverConfigEntryData.agentSnmpTrapReceiverCommunityName, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentSnmpTrapReceiverCommunityName, agentSnmpTrapReceiverConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentSnmpTrapReceiverIPAddress :
    if (usmDbTrapManagerIpAddrGet(USMDB_UNIT_CURRENT, agentSnmpTrapReceiverConfigEntryData.agentSnmpTrapReceiverIndex,
                                  &agentSnmpTrapReceiverConfigEntryData.agentSnmpTrapReceiverIPAddress) == L7_SUCCESS)
      SET_VALID(I_agentSnmpTrapReceiverIPAddress, agentSnmpTrapReceiverConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentSnmpTrapReceiverStatus :
    if (snmpAgentSnmpTrapReceiverStatusGet(USMDB_UNIT_CURRENT, agentSnmpTrapReceiverConfigEntryData.agentSnmpTrapReceiverIndex,
                                           &agentSnmpTrapReceiverConfigEntryData.agentSnmpTrapReceiverStatus) == L7_SUCCESS)
      SET_VALID(I_agentSnmpTrapReceiverStatus, agentSnmpTrapReceiverConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentSnmpTrapReceiverVersion :
    if (snmpAgentSnmpTrapReceiverVersionGet(USMDB_UNIT_CURRENT, agentSnmpTrapReceiverConfigEntryData.agentSnmpTrapReceiverIndex,
                                            &agentSnmpTrapReceiverConfigEntryData.agentSnmpTrapReceiverVersion) == L7_SUCCESS)
      SET_VALID(I_agentSnmpTrapReceiverVersion, agentSnmpTrapReceiverConfigEntryData.valid);
   if ( nominator != -1 )
    break;
  case I_agentSnmpTrapReceiverIpv6Address :
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
    memset(snmp_buffer,0x00, SNMP_BUFFER_LEN);
    if ((snmpAgentSnmpTrapReceiverIpv6AddressGet(USMDB_UNIT_CURRENT,
                                                 agentSnmpTrapReceiverConfigEntryData.agentSnmpTrapReceiverIndex,
                                                 snmp_buffer) == L7_SUCCESS) && 
        (SafeMakeOctetString(&agentSnmpTrapReceiverConfigEntryData.agentSnmpTrapReceiverIpv6Address, snmp_buffer,
         sizeof(L7_in6_addr_t)) == L7_TRUE))
    SET_VALID(I_agentSnmpTrapReceiverIpv6Address,agentSnmpTrapReceiverConfigEntryData.valid);
  break;
#endif
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentSnmpTrapReceiverConfigEntryData.valid) )
    return(NULL);

  return(&agentSnmpTrapReceiverConfigEntryData);
}

#ifdef SETS
int
k_agentSnmpTrapReceiverConfigEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                        doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentSnmpTrapReceiverConfigEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                         doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentSnmpTrapReceiverConfigEntry_set_defaults(doList_t *dp)
{
  agentSnmpTrapReceiverConfigEntry_t *data = (agentSnmpTrapReceiverConfigEntry_t *) (dp->data);

  if ((data->agentSnmpTrapReceiverCommunityName = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }

  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_agentSnmpTrapReceiverConfigEntry_set(agentSnmpTrapReceiverConfigEntry_t *data,
                                       ContextInfo *contextInfo, int function)
{
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if ( VALID(I_agentSnmpTrapReceiverCommunityName, data->valid) &&
       data->agentSnmpTrapReceiverCommunityName != NULL )
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentSnmpTrapReceiverCommunityName->octet_ptr, data->agentSnmpTrapReceiverCommunityName->length);
    if ( usmDbTrapManagerCommIpSet(USMDB_UNIT_CURRENT,
                                   data->agentSnmpTrapReceiverIndex,
                                   snmp_buffer) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentSnmpTrapReceiverCommunityName, tempValid);
    }
  }

  if ( VALID(I_agentSnmpTrapReceiverIPAddress, data->valid))
  {
    if( usmDbTrapManagerIpAddrRawSet(USMDB_UNIT_CURRENT,
                                    data->agentSnmpTrapReceiverIndex,
                                    data->agentSnmpTrapReceiverIPAddress) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentSnmpTrapReceiverIPAddress, tempValid);
    }
  }

  if ( VALID(I_agentSnmpTrapReceiverStatus, data->valid))
  {
    if( snmpAgentSnmpTrapReceiverStatusSet(USMDB_UNIT_CURRENT,
                                          data->agentSnmpTrapReceiverIndex,
                                          data->agentSnmpTrapReceiverStatus) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentSnmpTrapReceiverStatus, tempValid);
    }
  }

  if ( VALID(I_agentSnmpTrapReceiverVersion, data->valid))
  {
    if( snmpAgentSnmpTrapReceiverVersionSet(USMDB_UNIT_CURRENT,
                                    data->agentSnmpTrapReceiverIndex,
                                    data->agentSnmpTrapReceiverVersion) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentSnmpTrapReceiverVersion, tempValid);
    }
  }

  #if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  if ( VALID(I_agentSnmpTrapReceiverIpv6Address, data->valid))
  {
    memset(snmp_buffer,0x00,SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentSnmpTrapReceiverIpv6Address->octet_ptr,data->agentSnmpTrapReceiverIpv6Address->length);
    if( snmpAgentSnmpTrapReceiverIpv6AddressSet(USMDB_UNIT_CURRENT,
                                           data->agentSnmpTrapReceiverIndex,
                                           snmp_buffer) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentSnmpTrapReceiverIpv6Address, tempValid);
    }
  }
#endif


  return NO_ERROR;
}

  #ifdef SR_agentSnmpTrapReceiverConfigEntry_UNDO
/* add #define SR_agentSnmpTrapReceiverConfigEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentSnmpTrapReceiverConfigEntry family.
 */
int
agentSnmpTrapReceiverConfigEntry_undo(doList_t *doHead, doList_t *doCur,
                                      ContextInfo *contextInfo)
{
  agentSnmpTrapReceiverConfigEntry_t *data = (agentSnmpTrapReceiverConfigEntry_t *) doCur->data;
  agentSnmpTrapReceiverConfigEntry_t *undodata = (agentSnmpTrapReceiverConfigEntry_t *) doCur->undodata;
  agentSnmpTrapReceiverConfigEntry_t *entrysetdata = NULL;

  L7_int32 function = SR_UNKNOWN;
  L7_int32 agentSnmpTrapReceiverTempIndex; /* Temporary Index */
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  /* Define a group instance so as to create the deleted row */
  agentSnmpConfigGroup_t agentSnmpConfigGroupTempdata;
  agentSnmpConfigGroup_t *groupsetdata = &agentSnmpConfigGroupTempdata;

  /* Initialize this group instance */
  agentSnmpConfigGroupTempdata.agentSnmpTrapReceiverCreate = CloneOctetString(undodata->agentSnmpTrapReceiverCommunityName);
  ZERO_VALID(agentSnmpConfigGroupTempdata.valid);

  /*Copy valid bits from data to undodata */
  if( undodata != NULL && data != NULL )
    memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /*
   * we are either trying to undo a delete, or a modify
   * undoing a delete or modify, replace the original data
   */
  if(data->agentSnmpTrapReceiverStatus ==  D_agentSnmpTrapReceiverStatus_destroy)
  {
    /* check that the entry was actually deleted */
    if (usmDbTrapManagerEntryIpGet(USMDB_UNIT_CURRENT,
                                   undodata->agentSnmpTrapReceiverIndex) == L7_SUCCESS)
      return NO_ERROR;

    /* Use the set method for the creation of the row with default values */
    SET_VALID(I_agentSnmpTrapReceiverCreate, agentSnmpConfigGroupTempdata.valid);
    function = SR_ADD_MODIFY;
    if ((groupsetdata != NULL) && k_agentSnmpConfigGroup_set(groupsetdata, contextInfo, function) == NO_ERROR)
    {
      bzero(snmp_buffer, SNMP_BUFFER_LEN);
      strncpy(snmp_buffer, undodata->agentSnmpTrapReceiverCommunityName->octet_ptr,
                                               undodata->agentSnmpTrapReceiverCommunityName->length);

      /* Making sure that we get a NULL Terminated string */
      snmp_buffer[undodata->agentSnmpTrapReceiverCommunityName->length] = '\0';

      if ( snmpTrapReceiverIndex != NULL )
      {
        /* Newly created row's index */
        agentSnmpTrapReceiverTempIndex = *snmpTrapReceiverIndex;
      }
      else
      {
        return UNDO_FAILED_ERROR;
      }

      undodata->agentSnmpTrapReceiverIndex = agentSnmpTrapReceiverTempIndex;

      /* set all valid for the setting the previous values to the created row */
      memset(undodata->valid, 0xff, sizeof(undodata->valid));

      /* Status should not be set again */
      CLR_VALID(I_agentSnmpTrapReceiverStatus, undodata->valid);

      entrysetdata = undodata;

      /* use the set method for the undo */
      if ((entrysetdata != NULL) &&
                    k_agentSnmpTrapReceiverConfigEntry_set(entrysetdata,
                                                           contextInfo, function) == NO_ERROR)
        return NO_ERROR;
    }
  }
  else
  {
    /* we are trying to undo a modify to the Table */
    if(undodata->agentSnmpTrapReceiverStatus == D_agentSnmpTrapReceiverStatus_notInService)
    {
      undodata->agentSnmpTrapReceiverStatus = D_agentSnmpTrapReceiverStatus_config;
    }
    else
    {
      if(undodata->agentSnmpTrapReceiverStatus == D_agentSnmpTrapReceiverStatus_active)
      {
        undodata->agentSnmpTrapReceiverStatus = D_agentSnmpTrapReceiverStatus_config;
      }
    }
    entrysetdata = undodata;
    function = SR_ADD_MODIFY;

    /* use the set method for the undo */
    if ((entrysetdata != NULL)
             && k_agentSnmpTrapReceiverConfigEntry_set(entrysetdata,
                                                       contextInfo, function) == NO_ERROR)
      return NO_ERROR;
  }

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentSnmpTrapReceiverConfigEntry_UNDO */

#endif /* SETS */
#endif /* I_agentSnmpTrapReceiverIndex */

agentSnmpTrapFlagsConfigGroup_t *
k_agentSnmpTrapFlagsConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                                    int nominator)
{
  static agentSnmpTrapFlagsConfigGroup_t agentSnmpTrapFlagsConfigGroupData;

  ZERO_VALID(agentSnmpTrapFlagsConfigGroupData.valid);

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
    if (nominator != -1) break;
    /* else pass through */

  case I_agentSnmpAuthenticationTrapFlag :
    CLR_VALID(I_agentSnmpAuthenticationTrapFlag, agentSnmpTrapFlagsConfigGroupData.valid);
    if (snmpAgentSnmpAuthenticationTrapFlagGet(USMDB_UNIT_CURRENT, &agentSnmpTrapFlagsConfigGroupData.agentSnmpAuthenticationTrapFlag) == L7_SUCCESS)
      SET_VALID(I_agentSnmpAuthenticationTrapFlag, agentSnmpTrapFlagsConfigGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentSnmpLinkUpDownTrapFlag :
    CLR_VALID(I_agentSnmpLinkUpDownTrapFlag, agentSnmpTrapFlagsConfigGroupData.valid);
    if (snmpAgentSnmpLinkUpDownTrapFlagGet(USMDB_UNIT_CURRENT, &agentSnmpTrapFlagsConfigGroupData.agentSnmpLinkUpDownTrapFlag) == L7_SUCCESS)
      SET_VALID(I_agentSnmpLinkUpDownTrapFlag, agentSnmpTrapFlagsConfigGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentSnmpMultipleUsersTrapFlag :
    CLR_VALID(I_agentSnmpMultipleUsersTrapFlag, agentSnmpTrapFlagsConfigGroupData.valid);
    if (snmpAgentSnmpMultipleUsersTrapFlagGet(USMDB_UNIT_CURRENT, &agentSnmpTrapFlagsConfigGroupData.agentSnmpMultipleUsersTrapFlag) == L7_SUCCESS)
      SET_VALID(I_agentSnmpMultipleUsersTrapFlag, agentSnmpTrapFlagsConfigGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentSnmpSpanningTreeTrapFlag :
    CLR_VALID(I_agentSnmpSpanningTreeTrapFlag, agentSnmpTrapFlagsConfigGroupData.valid);
    if (snmpAgentSnmpSpanningTreeTrapFlagGet(USMDB_UNIT_CURRENT, &agentSnmpTrapFlagsConfigGroupData.agentSnmpSpanningTreeTrapFlag) == L7_SUCCESS)
      SET_VALID(I_agentSnmpSpanningTreeTrapFlag, agentSnmpTrapFlagsConfigGroupData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentSnmpTrapFlagsConfigGroupData.valid) )
    return(NULL);

  return(&agentSnmpTrapFlagsConfigGroupData);
}

#ifdef SETS
int
k_agentSnmpTrapFlagsConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                     doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentSnmpTrapFlagsConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                                      doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentSnmpTrapFlagsConfigGroup_set(agentSnmpTrapFlagsConfigGroup_t *data,
                                    ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if ( VALID(I_agentSnmpAuthenticationTrapFlag, data->valid))
  {
    if(snmpAgentSnmpAuthenticationTrapFlagSet(USMDB_UNIT_CURRENT,
                                              data->agentSnmpAuthenticationTrapFlag) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentSnmpAuthenticationTrapFlag, tempValid);
    }
  }

  if ( VALID(I_agentSnmpLinkUpDownTrapFlag, data->valid))
  {
    if( snmpAgentSnmpLinkUpDownTrapFlagSet(USMDB_UNIT_CURRENT,
                                          data->agentSnmpLinkUpDownTrapFlag) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentSnmpLinkUpDownTrapFlag, tempValid);
    }
  }

  if ( VALID(I_agentSnmpMultipleUsersTrapFlag, data->valid))
  {
    if( snmpAgentSnmpMultipleUsersTrapFlagSet(USMDB_UNIT_CURRENT,
                                             data->agentSnmpMultipleUsersTrapFlag) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentSnmpMultipleUsersTrapFlag, tempValid);
    }
  }

  if ( VALID(I_agentSnmpSpanningTreeTrapFlag, data->valid))
  {
    if(snmpAgentSnmpSpanningTreeTrapFlagSet(USMDB_UNIT_CURRENT,
                                            data->agentSnmpSpanningTreeTrapFlag) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentSnmpSpanningTreeTrapFlag, tempValid);
    }
  }

  return NO_ERROR;
}

  #ifdef SR_agentSnmpTrapFlagsConfigGroup_UNDO
/* add #define SR_agentSnmpTrapFlagsConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentSnmpTrapFlagsConfigGroup family.
 */
int
agentSnmpTrapFlagsConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                                   ContextInfo *contextInfo)
{
  agentSnmpTrapFlagsConfigGroup_t *data = (agentSnmpTrapFlagsConfigGroup_t *) doCur->data;
  agentSnmpTrapFlagsConfigGroup_t *undodata = (agentSnmpTrapFlagsConfigGroup_t *) doCur->undodata;
  agentSnmpTrapFlagsConfigGroup_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentSnmpTrapFlagsConfigGroup_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentSnmpTrapFlagsConfigGroup_UNDO */

#endif /* SETS */

agentSpanningTreeConfigGroup_t *
k_agentSpanningTreeConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                                   int nominator)
{
  static agentSpanningTreeConfigGroup_t agentSpanningTreeConfigGroupData;
  ZERO_VALID(agentSpanningTreeConfigGroupData.valid);

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
    if (nominator != -1) break;
    /* else pass through */

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentSpanningTreeConfigGroupData.valid) )
    return(NULL);

  return(&agentSpanningTreeConfigGroupData);
}

#ifdef SETS
int
k_agentSpanningTreeConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                    doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentSpanningTreeConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                                     doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentSpanningTreeConfigGroup_set(agentSpanningTreeConfigGroup_t *data,
                                   ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  return COMMIT_FAILED_ERROR;
}

  #ifdef SR_agentSpanningTreeConfigGroup_UNDO
/* add #define SR_agentSpanningTreeConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentSpanningTreeConfigGroup family.
 */
int
agentSpanningTreeConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                                  ContextInfo *contextInfo)
{
  agentSpanningTreeConfigGroup_t *data = (agentSpanningTreeConfigGroup_t *) doCur->data;
  agentSpanningTreeConfigGroup_t *undodata = (agentSpanningTreeConfigGroup_t *) doCur->undodata;
  agentSpanningTreeConfigGroup_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentSpanningTreeConfigGroup_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentSpanningTreeConfigGroup_UNDO */

#endif /* SETS */

agentStpSwitchConfigGroup_t *
k_agentStpSwitchConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                                int nominator)
{
  static agentStpSwitchConfigGroup_t agentStpSwitchConfigGroupData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1S_COMPONENT_ID, L7_DOT1S_FEATURE_ID) == L7_FALSE)
  {
    return(NULL);
  }

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;

    agentStpSwitchConfigGroupData.agentStpConfigDigestKey = MakeOctetString(NULL, 0);
    agentStpSwitchConfigGroupData.agentStpConfigName = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(agentStpSwitchConfigGroupData.valid);

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpConfigDigestKey:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbDot1sConfigDigestKeyGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentStpSwitchConfigGroupData.agentStpConfigDigestKey, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentStpConfigDigestKey, agentStpSwitchConfigGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpConfigFormatSelector:
    if (usmDbDot1sConfigFormatSelectorGet(USMDB_UNIT_CURRENT, &agentStpSwitchConfigGroupData.agentStpConfigFormatSelector) == L7_SUCCESS)
      SET_VALID(I_agentStpConfigFormatSelector, agentStpSwitchConfigGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpConfigName:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbDot1sConfigNameGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentStpSwitchConfigGroupData.agentStpConfigName, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentStpConfigName, agentStpSwitchConfigGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpConfigRevision:
    if (usmDbDot1sConfigRevisionGet(USMDB_UNIT_CURRENT, &agentStpSwitchConfigGroupData.agentStpConfigRevision) == L7_SUCCESS)
      SET_VALID(I_agentStpConfigRevision, agentStpSwitchConfigGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpForceVersion:
    if (snmpAgentStpForceVersionGet(USMDB_UNIT_CURRENT, &agentStpSwitchConfigGroupData.agentStpForceVersion) == L7_SUCCESS)
      SET_VALID(I_agentStpForceVersion, agentStpSwitchConfigGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpAdminMode:
    if (snmpAgentStpAdminModeGet(USMDB_UNIT_CURRENT, &agentStpSwitchConfigGroupData.agentStpAdminMode) == L7_SUCCESS)
      SET_VALID(I_agentStpAdminMode, agentStpSwitchConfigGroupData.valid);
    break;

  case I_agentStpBpduGuardMode:
    if (snmpAgentStpBpduGuardModeGet(USMDB_UNIT_CURRENT, &agentStpSwitchConfigGroupData.agentStpBpduGuardMode) == L7_SUCCESS)
      SET_VALID(I_agentStpBpduGuardMode, agentStpSwitchConfigGroupData.valid);
    break;

  case I_agentStpBpduFilterDefault:
    if (snmpAgentStpBpduFilterDefaultGet(USMDB_UNIT_CURRENT, &agentStpSwitchConfigGroupData.agentStpBpduFilterDefault) == L7_SUCCESS)
      SET_VALID(I_agentStpBpduFilterDefault, agentStpSwitchConfigGroupData.valid);
    break;


  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentStpSwitchConfigGroupData.valid) )
    return(NULL);

  return(&agentStpSwitchConfigGroupData);
}

#ifdef SETS
int
k_agentStpSwitchConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                 doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentStpSwitchConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                                  doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentStpSwitchConfigGroup_set(agentStpSwitchConfigGroup_t *data,
                                ContextInfo *contextInfo, int function)
{
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1S_COMPONENT_ID, L7_DOT1S_FEATURE_ID) == L7_FALSE)
  {
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentStpConfigName, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentStpConfigName->octet_ptr, data->agentStpConfigName->length);
    if (snmpAgentStpConfigNameSet(USMDB_UNIT_CURRENT, snmp_buffer) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentStpConfigName, tempValid);
    }
  }

  if (VALID(I_agentStpConfigRevision, data->valid))
  {
    if( usmDbDot1sConfigRevisionSet(USMDB_UNIT_CURRENT, data->agentStpConfigRevision) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentStpConfigRevision, tempValid);
    }
  }

  if (VALID(I_agentStpForceVersion, data->valid))
  {
    if(snmpAgentStpForceVersionSet(USMDB_UNIT_CURRENT, data->agentStpForceVersion) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentStpForceVersion, tempValid);
    }
  }

  if (VALID(I_agentStpAdminMode, data->valid))
  {
    if( snmpAgentStpAdminModeSet(USMDB_UNIT_CURRENT, data->agentStpAdminMode) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

  if (VALID(I_agentStpBpduGuardMode, data->valid))
  {
    if( snmpAgentStpBpduGuardModeSet(USMDB_UNIT_CURRENT, data->agentStpBpduGuardMode) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

  if (VALID(I_agentStpBpduFilterDefault, data->valid))
  {
    if( snmpAgentStpBpduFilterDefaultSet(USMDB_UNIT_CURRENT, data->agentStpBpduFilterDefault) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;
}

#ifdef SR_agentStpSwitchConfigGroup_UNDO
/* add #define SR_agentStpSwitchConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentStpSwitchConfigGroup family.
 */
int
agentStpSwitchConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                               ContextInfo *contextInfo)
{
  agentStpSwitchConfigGroup_t *data = (agentStpSwitchConfigGroup_t *) doCur->data;
  agentStpSwitchConfigGroup_t *undodata = (agentStpSwitchConfigGroup_t *) doCur->undodata;
  agentStpSwitchConfigGroup_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentStpSwitchConfigGroup_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
#endif /* SR_agentStpSwitchConfigGroup_UNDO */

#endif /* SETS */

agentStpPortEntry_t *
k_agentStpPortEntry_get(int serialNum, ContextInfo *contextInfo,
                        int nominator,
                        int searchType,
                        SR_INT32 ifIndex)
{
  static agentStpPortEntry_t agentStpPortEntryData;
  L7_uint32 intIfNum;
  usmDbTimeSpec_t temp_timeSpec;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1S_COMPONENT_ID, L7_DOT1S_FEATURE_ID) == L7_FALSE)
  {
    return(NULL);
  }

  ZERO_VALID(agentStpPortEntryData.valid);

  agentStpPortEntryData.ifIndex = ifIndex;
  SET_VALID(I_agentStpPortEntryIndex_ifIndex, agentStpPortEntryData.valid);

  if ( ( (searchType == EXACT) ?
         (snmpAgentStpPortGet(USMDB_UNIT_CURRENT, agentStpPortEntryData.ifIndex) != L7_SUCCESS) :
         (snmpAgentStpPortGet(USMDB_UNIT_CURRENT, agentStpPortEntryData.ifIndex) != L7_SUCCESS) &&
         (snmpAgentStpPortNextGet(USMDB_UNIT_CURRENT, &agentStpPortEntryData.ifIndex) != L7_SUCCESS)) ||
       (usmDbIntIfNumFromExtIfNum(agentStpPortEntryData.ifIndex, &intIfNum) != L7_SUCCESS))
  {
    ZERO_VALID(agentStpPortEntryData.valid);
    return(NULL);
  }

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpPortState:
    if (snmpAgentStpPortStateGet(USMDB_UNIT_CURRENT, intIfNum, &agentStpPortEntryData.agentStpPortState) == L7_SUCCESS)
      SET_VALID(I_agentStpPortState, agentStpPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpPortStatsMstpBpduRx:
    if (usmDbDot1sPortStatsMSTPBPDUsReceivedGet(USMDB_UNIT_CURRENT, intIfNum, &agentStpPortEntryData.agentStpPortStatsMstpBpduRx) == L7_SUCCESS)
      SET_VALID(I_agentStpPortStatsMstpBpduRx, agentStpPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpPortStatsMstpBpduTx:
    if (usmDbDot1sPortStatsMSTPBPDUsSentGet(USMDB_UNIT_CURRENT, intIfNum, &agentStpPortEntryData.agentStpPortStatsMstpBpduTx) == L7_SUCCESS)
      SET_VALID(I_agentStpPortStatsMstpBpduTx, agentStpPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpPortStatsRstpBpduRx:
    if (usmDbDot1sPortStatsRSTPBPDUsReceivedGet(USMDB_UNIT_CURRENT, intIfNum, &agentStpPortEntryData.agentStpPortStatsRstpBpduRx) == L7_SUCCESS)
      SET_VALID(I_agentStpPortStatsRstpBpduRx, agentStpPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpPortStatsRstpBpduTx:
    if (usmDbDot1sPortStatsRSTPBPDUsSentGet(USMDB_UNIT_CURRENT, intIfNum, &agentStpPortEntryData.agentStpPortStatsRstpBpduTx) == L7_SUCCESS)
      SET_VALID(I_agentStpPortStatsRstpBpduTx, agentStpPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpPortStatsStpBpduRx:
    if (usmDbDot1sPortStatsSTPBPDUsReceivedGet(USMDB_UNIT_CURRENT, intIfNum, &agentStpPortEntryData.agentStpPortStatsStpBpduRx) == L7_SUCCESS)
      SET_VALID(I_agentStpPortStatsStpBpduRx, agentStpPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpPortStatsStpBpduTx:
    if (usmDbDot1sPortStatsSTPBPDUsSentGet(USMDB_UNIT_CURRENT, intIfNum, &agentStpPortEntryData.agentStpPortStatsStpBpduTx) == L7_SUCCESS)
      SET_VALID(I_agentStpPortStatsStpBpduTx, agentStpPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpPortUpTime:
    if (usmDbDot1sPortUpTimeGet(USMDB_UNIT_CURRENT, intIfNum, &temp_timeSpec) == L7_SUCCESS)
    {
      agentStpPortEntryData.agentStpPortUpTime  = temp_timeSpec.seconds;
      agentStpPortEntryData.agentStpPortUpTime += temp_timeSpec.minutes * SECONDS_PER_MINUTE;
      agentStpPortEntryData.agentStpPortUpTime += temp_timeSpec.hours * SECONDS_PER_HOUR;
      agentStpPortEntryData.agentStpPortUpTime += temp_timeSpec.days * SECONDS_PER_DAY;

      agentStpPortEntryData.agentStpPortUpTime  *= SNMP_HUNDRED;
      SET_VALID(I_agentStpPortUpTime, agentStpPortEntryData.valid);
    }
    if ( nominator != -1) break;
    /* else pass through */

  case I_agentStpPortMigrationCheck:
    agentStpPortEntryData.agentStpPortMigrationCheck = D_agentStpPortMigrationCheck_false;
    SET_VALID(I_agentStpPortMigrationCheck, agentStpPortEntryData.valid);
    if ( nominator != -1) break;
    /* else pass through */

  

  case I_agentStpPortEntryIndex_ifIndex:
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentStpPortEntryData.valid) )
    return(NULL);

   return(&agentStpPortEntryData);
}

#ifdef SETS
int
k_agentStpPortEntry_test(ObjectInfo *object, ObjectSyntax *value,
                         doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentStpPortEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                          doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentStpPortEntry_set_defaults(doList_t *dp)
{
    agentStpPortEntry_t *data = (agentStpPortEntry_t *) (dp->data);

    data->agentStpPortStatsMstpBpduRx = (SR_UINT32) 0;
    data->agentStpPortStatsMstpBpduTx = (SR_UINT32) 0;
    data->agentStpPortStatsRstpBpduRx = (SR_UINT32) 0;
    data->agentStpPortStatsRstpBpduTx = (SR_UINT32) 0;
    data->agentStpPortStatsStpBpduRx = (SR_UINT32) 0;
    data->agentStpPortStatsStpBpduTx = (SR_UINT32) 0;

    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentStpPortEntry_set(agentStpPortEntry_t *data,
                        ContextInfo *contextInfo, int function)
{
  L7_uint32 intIfNum;
  L7_RC_t rc;

  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1S_COMPONENT_ID, L7_DOT1S_FEATURE_ID) == L7_FALSE)
  {
    return COMMIT_FAILED_ERROR;
  }

  /* Conversion to internal interface number for snmp call */
  rc = usmDbIntIfNumFromExtIfNum(data->ifIndex, &intIfNum);
  if (rc == L7_SUCCESS)
  {
    if (VALID(I_agentStpPortState, data->valid))
    {
      if(snmpAgentStpPortStateSet(USMDB_UNIT_CURRENT, intIfNum, data->agentStpPortState) != L7_SUCCESS)
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
      else
      {
        CLR_VALID(I_agentStpPortState, tempValid);
      }
    }

    if (VALID(I_agentStpPortMigrationCheck, data->valid))
    {
      if( data->agentStpPortMigrationCheck == D_agentStpPortMigrationCheck_true &&
        usmDbDot1sPortForceMigrationCheck(USMDB_UNIT_CURRENT, intIfNum) != L7_SUCCESS)
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
    }

    

    return NO_ERROR;
  }

  return COMMIT_FAILED_ERROR;
}

#ifdef SR_agentStpPortEntry_UNDO
/* add #define SR_agentStpPortEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentStpPortEntry family.
 */
int
agentStpPortEntry_undo(doList_t *doHead, doList_t *doCur,
                       ContextInfo *contextInfo)
{
  agentStpPortEntry_t *data = (agentStpPortEntry_t *) doCur->data;
  agentStpPortEntry_t *undodata = (agentStpPortEntry_t *) doCur->undodata;
  agentStpPortEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentStpPortEntry_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
#endif /* SR_agentStpPortEntry_UNDO */

#endif /* SETS */

agentStpCstConfigGroup_t *
k_agentStpCstConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                             int nominator)
{
  static agentStpCstConfigGroup_t agentStpCstConfigGroupData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 rootIdlength;
  L7_ushort16 bridgePriority = 0;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1S_COMPONENT_ID, L7_DOT1S_FEATURE_ID) == L7_FALSE)
  {
    return(NULL);
  }

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;

    agentStpCstConfigGroupData.agentStpCstRegionalRootId = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(agentStpCstConfigGroupData.valid);

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpCstHelloTime:
    if (usmDbDot1sCistHelloTimeGet(USMDB_UNIT_CURRENT, &agentStpCstConfigGroupData.agentStpCstHelloTime) == L7_SUCCESS)
      SET_VALID(I_agentStpCstHelloTime, agentStpCstConfigGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpCstMaxAge:
    if (usmDbDot1sCistMaxAgeGet(USMDB_UNIT_CURRENT, &agentStpCstConfigGroupData.agentStpCstMaxAge) == L7_SUCCESS)
      SET_VALID(I_agentStpCstMaxAge, agentStpCstConfigGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpCstRegionalRootId:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbDot1sCistRegionalRootIDGet(USMDB_UNIT_CURRENT, snmp_buffer, &rootIdlength) == L7_SUCCESS &&
        (SafeMakeOctetString(&agentStpCstConfigGroupData.agentStpCstRegionalRootId, snmp_buffer, rootIdlength) == L7_TRUE))
      SET_VALID(I_agentStpCstRegionalRootId, agentStpCstConfigGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpCstRegionalRootPathCost:
    if (usmDbDot1sCistRegionalRootPathCostGet(USMDB_UNIT_CURRENT, &agentStpCstConfigGroupData.agentStpCstRegionalRootPathCost) == L7_SUCCESS)
      SET_VALID(I_agentStpCstRegionalRootPathCost, agentStpCstConfigGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpCstRootFwdDelay:
    if (usmDbDot1sCistRootFwdDelayGet(USMDB_UNIT_CURRENT, &agentStpCstConfigGroupData.agentStpCstRootFwdDelay) == L7_SUCCESS)
      SET_VALID(I_agentStpCstRootFwdDelay, agentStpCstConfigGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpCstBridgeFwdDelay:
    if (usmDbDot1sCistBridgeFwdDelayGet(USMDB_UNIT_CURRENT, &agentStpCstConfigGroupData.agentStpCstBridgeFwdDelay) == L7_SUCCESS)
      SET_VALID(I_agentStpCstBridgeFwdDelay, agentStpCstConfigGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpCstBridgeHelloTime:
    if (usmDbDot1sCistBridgeHelloTimeGet(USMDB_UNIT_CURRENT, &agentStpCstConfigGroupData.agentStpCstBridgeHelloTime) == L7_SUCCESS)
      SET_VALID(I_agentStpCstBridgeHelloTime, agentStpCstConfigGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpCstBridgeHoldTime:
    if (usmDbDot1sCistBridgeHoldTimeGet(USMDB_UNIT_CURRENT, &agentStpCstConfigGroupData.agentStpCstBridgeHoldTime) == L7_SUCCESS)
      SET_VALID(I_agentStpCstBridgeHoldTime, agentStpCstConfigGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpCstBridgeMaxAge:
    if (usmDbDot1sCistBridgeMaxAgeGet(USMDB_UNIT_CURRENT, &agentStpCstConfigGroupData.agentStpCstBridgeMaxAge) == L7_SUCCESS)
      SET_VALID(I_agentStpCstBridgeMaxAge, agentStpCstConfigGroupData.valid);
     break;

  case I_agentStpCstBridgeMaxHops:
    if (usmDbDot1sBridgeMaxHopGet(USMDB_UNIT_CURRENT, &agentStpCstConfigGroupData.agentStpCstBridgeMaxHops) == L7_SUCCESS)
      SET_VALID(I_agentStpCstBridgeMaxHops, agentStpCstConfigGroupData.valid);
     break;

  case I_agentStpCstBridgePriority:
    if (usmDbDot1sMstiBridgePriorityGet(USMDB_UNIT_CURRENT, DOT1S_CIST_ID, &bridgePriority) == L7_SUCCESS)
    {
      agentStpCstConfigGroupData.agentStpCstBridgePriority = (L7_uint32)bridgePriority;
      SET_VALID(I_agentStpCstBridgePriority, agentStpCstConfigGroupData.valid);
    }
    break;

  case I_agentStpCstBridgeHoldCount:
     if (usmDbDot1sBridgeTxHoldCountGet(USMDB_UNIT_CURRENT,&agentStpCstConfigGroupData.agentStpCstBridgeHoldCount)==L7_SUCCESS)
       SET_VALID(I_agentStpCstBridgeHoldCount, agentStpCstConfigGroupData.valid);
     break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentStpCstConfigGroupData.valid) )
    return(NULL);

  return(&agentStpCstConfigGroupData);
}

#ifdef SETS
int
k_agentStpCstConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentStpCstConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                               doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentStpCstConfigGroup_set(agentStpCstConfigGroup_t *data,
                             ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1S_COMPONENT_ID, L7_DOT1S_FEATURE_ID) == L7_FALSE)
  {
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentStpCstBridgeFwdDelay, data->valid))
  {
    if( usmDbDot1sCistBridgeFwdDelaySet(USMDB_UNIT_CURRENT, data->agentStpCstBridgeFwdDelay) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentStpCstBridgeFwdDelay, tempValid);
    }
  }

  if (VALID(I_agentStpCstBridgeMaxAge, data->valid))
  {
    if( usmDbDot1sCistBridgeMaxAgeSet(USMDB_UNIT_CURRENT, data->agentStpCstBridgeMaxAge) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

  if (VALID(I_agentStpCstBridgeMaxHops, data->valid))
  {
    if (usmDbDot1sBridgeMaxHopSet(USMDB_UNIT_CURRENT, data->agentStpCstBridgeMaxHops) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

  if (VALID(I_agentStpCstBridgePriority, data->valid))
  {
    if (usmDbDot1sMstiBridgePrioritySet(USMDB_UNIT_CURRENT, DOT1S_CIST_ID, (L7_uint32 *)&(data->agentStpCstBridgePriority)) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

  /* hold count*/
  if (VALID(I_agentStpCstBridgeHoldCount, data->valid))
  {
    if (usmDbDot1sBridgeTxHoldCountSet(USMDB_UNIT_CURRENT, data->agentStpCstBridgeHoldCount) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;
}

#ifdef SR_agentStpCstConfigGroup_UNDO
/* add #define SR_agentStpCstConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentStpCstConfigGroup family.
 */
int
agentStpCstConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                            ContextInfo *contextInfo)
{
  agentStpCstConfigGroup_t *data = (agentStpCstConfigGroup_t *) doCur->data;
  agentStpCstConfigGroup_t *undodata = (agentStpCstConfigGroup_t *) doCur->undodata;
  agentStpCstConfigGroup_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentStpCstConfigGroup_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
#endif /* SR_agentStpCstConfigGroup_UNDO */

#endif /* SETS */

agentStpCstPortEntry_t *
k_agentStpCstPortEntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           SR_INT32 ifIndex)
{
  static agentStpCstPortEntry_t agentStpCstPortEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 snmp_buffer_len = SNMP_BUFFER_LEN;
  L7_uint32 bridgeIdLength;
  L7_uint32 intIfNum;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1S_COMPONENT_ID, L7_DOT1S_FEATURE_ID) == L7_FALSE)
  {
    return(NULL);
  }

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;

    agentStpCstPortEntryData.agentStpCstPortId = MakeOctetString(NULL, 0);
    agentStpCstPortEntryData.agentStpCstDesignatedBridgeId = MakeOctetString(NULL, 0);
    agentStpCstPortEntryData.agentStpCstDesignatedPortId = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(agentStpCstPortEntryData.valid);

  agentStpCstPortEntryData.ifIndex = ifIndex;
  SET_VALID(I_agentStpCstPortEntryIndex_ifIndex, agentStpCstPortEntryData.valid);

  if ( ( (searchType == EXACT) ?
         (snmpAgentStpCstPortGet(USMDB_UNIT_CURRENT, agentStpCstPortEntryData.ifIndex) != L7_SUCCESS) :
         (snmpAgentStpCstPortGet(USMDB_UNIT_CURRENT, agentStpCstPortEntryData.ifIndex) != L7_SUCCESS) &&
         (snmpAgentStpCstPortNextGet(USMDB_UNIT_CURRENT, &agentStpCstPortEntryData.ifIndex) != L7_SUCCESS)) ||
       (usmDbIntIfNumFromExtIfNum(agentStpCstPortEntryData.ifIndex, &intIfNum) != L7_SUCCESS))
  {
    ZERO_VALID(agentStpCstPortEntryData.valid);
    return(NULL);
  }

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpCstPortOperEdge:
    if (snmpAgentStpCstPortOperEdgeGet(USMDB_UNIT_CURRENT, intIfNum, &agentStpCstPortEntryData.agentStpCstPortOperEdge) == L7_SUCCESS)
      SET_VALID(I_agentStpCstPortOperEdge, agentStpCstPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpCstPortOperPointToPoint:
    if (snmpAgentStpCstPortOperPointToPointGet(USMDB_UNIT_CURRENT, intIfNum, &agentStpCstPortEntryData.agentStpCstPortOperPointToPoint) == L7_SUCCESS)
      SET_VALID(I_agentStpCstPortOperPointToPoint, agentStpCstPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpCstPortTopologyChangeAck:
    if (snmpAgentStpCstPortTopologyChangeAckGet(USMDB_UNIT_CURRENT, intIfNum, &agentStpCstPortEntryData.agentStpCstPortTopologyChangeAck) == L7_SUCCESS)
      SET_VALID(I_agentStpCstPortTopologyChangeAck, agentStpCstPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpCstPortEdge:
    if (snmpAgentStpCstPortEdgeGet(USMDB_UNIT_CURRENT, intIfNum, &agentStpCstPortEntryData.agentStpCstPortEdge) == L7_SUCCESS)
      SET_VALID(I_agentStpCstPortEdge, agentStpCstPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpCstPortForwardingState:
    if (snmpAgentStpMstPortForwardingStateGet(USMDB_UNIT_CURRENT, L7_DOT1S_CIST_INSTANCE, intIfNum,
                                              &agentStpCstPortEntryData.agentStpCstPortForwardingState) == L7_SUCCESS)
      SET_VALID(I_agentStpCstPortForwardingState, agentStpCstPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpCstPortId:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (snmpAgentStpMstPortIDGet(USMDB_UNIT_CURRENT, L7_DOT1S_CIST_INSTANCE, intIfNum, snmp_buffer, snmp_buffer_len) == L7_SUCCESS &&
        (SafeMakeOctetString(&agentStpCstPortEntryData.agentStpCstPortId, snmp_buffer, 2) == L7_TRUE))
      SET_VALID(I_agentStpCstPortId, agentStpCstPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpCstPortPathCost:
    if (usmDbDot1sMstiPortPathCostGet(USMDB_UNIT_CURRENT, L7_DOT1S_CIST_INSTANCE, intIfNum,
                                      &agentStpCstPortEntryData.agentStpCstPortPathCost) == L7_SUCCESS)
      SET_VALID(I_agentStpCstPortPathCost, agentStpCstPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpCstPortPriority:
    if (usmDbDot1sMstiPortPriorityGet(USMDB_UNIT_CURRENT, L7_DOT1S_CIST_INSTANCE, intIfNum,
                                      &agentStpCstPortEntryData.agentStpCstPortPriority) == L7_SUCCESS)
      SET_VALID(I_agentStpCstPortPriority, agentStpCstPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpCstDesignatedBridgeId:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbDot1sMstiDesignatedBridgeIDGet(USMDB_UNIT_CURRENT, L7_DOT1S_CIST_INSTANCE, intIfNum, snmp_buffer, &bridgeIdLength) == L7_SUCCESS &&
        (SafeMakeOctetString(&agentStpCstPortEntryData.agentStpCstDesignatedBridgeId, snmp_buffer, bridgeIdLength) == L7_TRUE))
      SET_VALID(I_agentStpCstDesignatedBridgeId, agentStpCstPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpCstDesignatedCost:
    if (usmDbDot1sMstiDesignatedCostGet(USMDB_UNIT_CURRENT, L7_DOT1S_CIST_INSTANCE, intIfNum, &agentStpCstPortEntryData.agentStpCstDesignatedCost) == L7_SUCCESS)
      SET_VALID(I_agentStpCstDesignatedCost, agentStpCstPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpCstDesignatedPortId:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (snmpAgentStpMstDesignatedPortIDGet(USMDB_UNIT_CURRENT, L7_DOT1S_CIST_INSTANCE, intIfNum, snmp_buffer, snmp_buffer_len) == L7_SUCCESS &&
        (SafeMakeOctetString(&agentStpCstPortEntryData.agentStpCstDesignatedPortId, snmp_buffer, 2) == L7_TRUE))
      SET_VALID(I_agentStpCstDesignatedPortId, agentStpCstPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpCstExtPortPathCost:
    if (usmDbDot1sCistPortExternalPathCostGet(USMDB_UNIT_CURRENT, intIfNum,
                                              &agentStpCstPortEntryData.agentStpCstExtPortPathCost) == L7_SUCCESS)
      SET_VALID(I_agentStpCstExtPortPathCost, agentStpCstPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpCstPortBpduGuardEffect:
    if (snmpAgentStpCstPortBpduGuardEffectGet(USMDB_UNIT_CURRENT, intIfNum, &agentStpCstPortEntryData.agentStpCstPortBpduGuardEffect) == L7_SUCCESS)
      SET_VALID(I_agentStpCstPortBpduGuardEffect, agentStpCstPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpCstPortBpduFilter:
    if (snmpAgentStpCstPortBpduFilterGet(USMDB_UNIT_CURRENT, intIfNum, &agentStpCstPortEntryData.agentStpCstPortBpduFilter) == L7_SUCCESS)
      SET_VALID(I_agentStpCstPortBpduFilter, agentStpCstPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpCstPortBpduFlood:
    if (snmpAgentStpCstPortBpduFloodGet(USMDB_UNIT_CURRENT, intIfNum, &agentStpCstPortEntryData.agentStpCstPortBpduFlood) == L7_SUCCESS)
      SET_VALID(I_agentStpCstPortBpduFlood, agentStpCstPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpCstPortAutoEdge:
    if (snmpAgentStpCstPortAutoEdgeGet(USMDB_UNIT_CURRENT, intIfNum, &agentStpCstPortEntryData.agentStpCstPortAutoEdge) == L7_SUCCESS)
      SET_VALID(I_agentStpCstPortAutoEdge, agentStpCstPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpCstPortRootGuard:
    if (snmpAgentStpCstPortRootGuardGet(USMDB_UNIT_CURRENT, intIfNum, &agentStpCstPortEntryData.agentStpCstPortRootGuard) == L7_SUCCESS)
      SET_VALID(I_agentStpCstPortRootGuard, agentStpCstPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */
  case I_agentStpCstPortLoopGuard:
    if (snmpAgentStpCstPortLoopGuardGet(USMDB_UNIT_CURRENT, intIfNum, &agentStpCstPortEntryData.agentStpCstPortLoopGuard) == L7_SUCCESS)
      SET_VALID(I_agentStpCstPortLoopGuard, agentStpCstPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */
  case I_agentStpCstPortTCNGuard:
    if (snmpAgentStpCstPortTCNGuardGet(USMDB_UNIT_CURRENT, intIfNum, &agentStpCstPortEntryData.agentStpCstPortTCNGuard) == L7_SUCCESS)
      SET_VALID(I_agentStpCstPortTCNGuard, agentStpCstPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpCstPortEntryIndex_ifIndex:
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentStpCstPortEntryData.valid) )
    return(NULL);

   return(&agentStpCstPortEntryData);
}

#ifdef SETS
int
k_agentStpCstPortEntry_test(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentStpCstPortEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                             doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentStpCstPortEntry_set_defaults(doList_t *dp)
{
    agentStpCstPortEntry_t *data = (agentStpCstPortEntry_t *) (dp->data);

    if ((data->agentStpCstPortId = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->agentStpCstDesignatedBridgeId = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->agentStpCstDesignatedPortId = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentStpCstPortEntry_set(agentStpCstPortEntry_t *data,
                           ContextInfo *contextInfo, int function)
{
  L7_uint32 intIfNum;
  L7_RC_t rc;

  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1S_COMPONENT_ID, L7_DOT1S_FEATURE_ID) == L7_FALSE)
  {
    return COMMIT_FAILED_ERROR;
  }

  /* Conversion to internal interface number for snmp/usmdb calls */
  rc = usmDbIntIfNumFromExtIfNum(data->ifIndex, &intIfNum);
  if (rc == L7_SUCCESS)
  {
    if (VALID(I_agentStpCstPortEdge, data->valid))
    {
      if(snmpAgentStpCstPortEdgeSet(USMDB_UNIT_CURRENT, intIfNum, data->agentStpCstPortEdge) != L7_SUCCESS)
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
      else
      {
        SET_VALID(I_agentStpCstPortEdge, tempValid);
      }
    }

    if (VALID(I_agentStpCstPortPathCost, data->valid))
    {
      if( usmDbDot1sMstiPortPathCostSet(USMDB_UNIT_CURRENT, L7_DOT1S_CIST_INSTANCE, intIfNum, data->agentStpCstPortPathCost) != L7_SUCCESS)
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
      else
      {
        SET_VALID(I_agentStpCstPortPathCost, tempValid);
      }
    }

    if (VALID(I_agentStpCstPortPriority, data->valid))
    {
      if( usmDbDot1sMstiPortPrioritySet(USMDB_UNIT_CURRENT, L7_DOT1S_CIST_INSTANCE, intIfNum, &data->agentStpCstPortPriority) != L7_SUCCESS)
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
    }

    if (VALID(I_agentStpCstExtPortPathCost, data->valid))
    {
      if (usmDbDot1sCistPortExternalPathCostSet(USMDB_UNIT_CURRENT, intIfNum, data->agentStpCstExtPortPathCost) != L7_SUCCESS)
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
      else
      {
        SET_VALID(I_agentStpCstExtPortPathCost, tempValid);
      }
    }

    if (VALID(I_agentStpCstPortBpduFilter, data->valid))
    {
      if(snmpAgentStpCstPortBpduFilterSet(USMDB_UNIT_CURRENT, intIfNum, data->agentStpCstPortBpduFilter) != L7_SUCCESS)
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
      else
      {
        SET_VALID(I_agentStpCstPortBpduFilter, tempValid);
      }
    }

    if (VALID(I_agentStpCstPortBpduFlood, data->valid))
    {
      if(snmpAgentStpCstPortBpduFloodSet(USMDB_UNIT_CURRENT, intIfNum, data->agentStpCstPortBpduFlood) != L7_SUCCESS)
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
      else
      {
        SET_VALID(I_agentStpCstPortBpduFlood, tempValid);
      }
    }

    if (VALID(I_agentStpCstPortAutoEdge, data->valid))
    {
      if(snmpAgentStpCstPortAutoEdgeSet(USMDB_UNIT_CURRENT, intIfNum, data->agentStpCstPortAutoEdge) != L7_SUCCESS)
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
      else
      {
        SET_VALID(I_agentStpCstPortAutoEdge, tempValid);
      }
    }

    if (VALID(I_agentStpCstPortRootGuard, data->valid))
    {
      if(snmpAgentStpCstPortRootGuardSet(USMDB_UNIT_CURRENT, intIfNum, data->agentStpCstPortRootGuard) != L7_SUCCESS)
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
      else
      {
        SET_VALID(I_agentStpCstPortRootGuard, tempValid);
      }
    }

    if (VALID(I_agentStpCstPortLoopGuard, data->valid))
    {
      if(snmpAgentStpCstPortLoopGuardSet(USMDB_UNIT_CURRENT, intIfNum, data->agentStpCstPortLoopGuard) != L7_SUCCESS)
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
      else
      {
        SET_VALID(I_agentStpCstPortLoopGuard, tempValid);
      }
    }

    if (VALID(I_agentStpCstPortTCNGuard, data->valid))
    {
      if(snmpAgentStpCstPortTCNGuardSet(USMDB_UNIT_CURRENT, intIfNum, data->agentStpCstPortTCNGuard) != L7_SUCCESS)
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
      else
      {
        SET_VALID(I_agentStpCstPortTCNGuard, tempValid);
      }
    }
    return NO_ERROR;
  }

  return COMMIT_FAILED_ERROR;
}

#ifdef SR_agentStpCstPortEntry_UNDO
/* add #define SR_agentStpCstPortEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentStpCstPortEntry family.
 */
int
agentStpCstPortEntry_undo(doList_t *doHead, doList_t *doCur,
                          ContextInfo *contextInfo)
{
  agentStpCstPortEntry_t *data = (agentStpCstPortEntry_t *) doCur->data;
  agentStpCstPortEntry_t *undodata = (agentStpCstPortEntry_t *) doCur->undodata;
  agentStpCstPortEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL)
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentStpCstPortEntry_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
#endif /* SR_agentStpCstPortEntry_UNDO */

#endif /* SETS */

agentStpMstEntry_t *
k_agentStpMstEntry_get(int serialNum, ContextInfo *contextInfo,
                       int nominator,
                       int searchType,
                       SR_UINT32 agentStpMstId)
{
  static agentStpMstEntry_t agentStpMstEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_ushort16 priority;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 bridgeIdLength;
  L7_uint32 rootIdLength;
  usmDbTimeSpec_t temp_timespec;
  L7_ushort16 temp_short;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1S_COMPONENT_ID, L7_DOT1S_FEATURE_ID) == L7_FALSE)
  {
    return(NULL);
  }

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;

    agentStpMstEntryData.agentStpMstBridgeIdentifier = MakeOctetString(NULL, 0);
    agentStpMstEntryData.agentStpMstDesignatedRootId = MakeOctetString(NULL, 0);
    agentStpMstEntryData.agentStpMstRootPortId = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(agentStpMstEntryData.valid);

  agentStpMstEntryData.agentStpMstId = agentStpMstId;
  SET_VALID(I_agentStpMstId, agentStpMstEntryData.valid);

 if ( (searchType == EXACT) ?
       usmDbDot1sInstanceGet(USMDB_UNIT_CURRENT, agentStpMstEntryData.agentStpMstId) != L7_SUCCESS :
       (usmDbDot1sInstanceGet(USMDB_UNIT_CURRENT, agentStpMstEntryData.agentStpMstId) != L7_SUCCESS &&
        usmDbDot1sInstanceNextGet(USMDB_UNIT_CURRENT, agentStpMstEntryData.agentStpMstId,
        &agentStpMstEntryData.agentStpMstId) != L7_SUCCESS) )
  {
    ZERO_VALID(agentStpMstEntryData.valid);
    return(NULL);
  }

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
  case I_agentStpMstId:
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpMstBridgePriority:
    if (usmDbDot1sMstiBridgePriorityGet(USMDB_UNIT_CURRENT, agentStpMstEntryData.agentStpMstId, &priority) == L7_SUCCESS)
    {
      agentStpMstEntryData.agentStpMstBridgePriority = (L7_uint32)priority;
      SET_VALID(I_agentStpMstBridgePriority, agentStpMstEntryData.valid);
    }
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpMstBridgeIdentifier:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbDot1sMstiBridgeIdentifierGet(USMDB_UNIT_CURRENT, agentStpMstEntryData.agentStpMstId, snmp_buffer, &bridgeIdLength) == L7_SUCCESS &&
        (SafeMakeOctetString(&agentStpMstEntryData.agentStpMstBridgeIdentifier, snmp_buffer, bridgeIdLength) == L7_TRUE))
      SET_VALID(I_agentStpMstBridgeIdentifier, agentStpMstEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpMstDesignatedRootId:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbDot1sMstiDesignatedRootIDGet(USMDB_UNIT_CURRENT, agentStpMstEntryData.agentStpMstId, snmp_buffer, &rootIdLength) == L7_SUCCESS &&
        (SafeMakeOctetString(&agentStpMstEntryData.agentStpMstDesignatedRootId, snmp_buffer, rootIdLength) == L7_TRUE))
      SET_VALID(I_agentStpMstDesignatedRootId, agentStpMstEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpMstRootPathCost:
    if (usmDbDot1sMstiRootPathCostGet(USMDB_UNIT_CURRENT, agentStpMstEntryData.agentStpMstId, &agentStpMstEntryData.agentStpMstRootPathCost) == L7_SUCCESS)
      SET_VALID(I_agentStpMstRootPathCost, agentStpMstEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpMstRootPortId:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (snmpAgentStpMstRootPortIDGet(USMDB_UNIT_CURRENT, agentStpMstEntryData.agentStpMstId, &temp_short) == L7_SUCCESS &&
        (SafeMakeOctetString(&agentStpMstEntryData.agentStpMstRootPortId, (char*)&temp_short, 2) == L7_TRUE))
      SET_VALID(I_agentStpMstRootPortId, agentStpMstEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpMstTimeSinceTopologyChange:
    if (usmDbDot1sMstiTimeSinceTopologyChangeGet(USMDB_UNIT_CURRENT, agentStpMstEntryData.agentStpMstId, &temp_timespec) == L7_SUCCESS)
    {
      agentStpMstEntryData.agentStpMstTimeSinceTopologyChange  = temp_timespec.seconds;
      agentStpMstEntryData.agentStpMstTimeSinceTopologyChange += temp_timespec.minutes * SECONDS_PER_MINUTE;
      agentStpMstEntryData.agentStpMstTimeSinceTopologyChange += temp_timespec.hours * SECONDS_PER_HOUR;
      agentStpMstEntryData.agentStpMstTimeSinceTopologyChange += temp_timespec.days * SECONDS_PER_DAY;

      agentStpMstEntryData.agentStpMstTimeSinceTopologyChange *= SNMP_HUNDRED;
      SET_VALID(I_agentStpMstTimeSinceTopologyChange, agentStpMstEntryData.valid);
    }
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpMstTopologyChangeCount:
    if (usmDbDot1sMstiTopologyChangeCountGet(USMDB_UNIT_CURRENT, agentStpMstEntryData.agentStpMstId, &agentStpMstEntryData.agentStpMstTopologyChangeCount) == L7_SUCCESS)
      SET_VALID(I_agentStpMstTopologyChangeCount, agentStpMstEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpMstTopologyChangeParm:
    if (snmpAgentStpMstTopologyChangeParmGet(USMDB_UNIT_CURRENT, agentStpMstEntryData.agentStpMstId, &agentStpMstEntryData.agentStpMstTopologyChangeParm) == L7_SUCCESS)
      SET_VALID(I_agentStpMstTopologyChangeParm, agentStpMstEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpMstRowStatus:
    /* if entry shows up, it's active */
    agentStpMstEntryData.agentStpMstRowStatus = D_agentStpMstRowStatus_active;
    SET_VALID(I_agentStpMstRowStatus, agentStpMstEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentStpMstEntryData.valid) )
    return(NULL);

   return(&agentStpMstEntryData);
}

#ifdef SETS
int
k_agentStpMstEntry_test(ObjectInfo *object, ObjectSyntax *value,
                        doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentStpMstEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                         doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentStpMstEntry_set_defaults(doList_t *dp)
{
    agentStpMstEntry_t *data = (agentStpMstEntry_t *) (dp->data);

    if ((data->agentStpMstBridgeIdentifier = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->agentStpMstDesignatedRootId = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->agentStpMstRootPortId = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->agentStpMstTopologyChangeCount = (SR_UINT32) 0;

    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentStpMstEntry_set(agentStpMstEntry_t *data,
                       ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1S_COMPONENT_ID, L7_DOT1S_FEATURE_ID) == L7_FALSE)
  {
    return COMMIT_FAILED_ERROR;
  }

  if (data->agentStpMstId == L7_NULL)
    return COMMIT_FAILED_ERROR;

  if (usmDbDot1sInstanceGet(USMDB_UNIT_CURRENT, data->agentStpMstId) != L7_SUCCESS)
  {
    /* create a new entry */
    if (VALID(I_agentStpMstRowStatus, data->valid))
    {
      if((data->agentStpMstRowStatus == D_agentStpMstRowStatus_createAndGo) &&
        (usmDbDot1sMstiCreate(USMDB_UNIT_CURRENT, data->agentStpMstId) != L7_SUCCESS))
      {
        /* don't set the status */
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
      else
      {
        SET_VALID(I_agentStpMstRowStatus, tempValid);
      }
    }
    else
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return COMMIT_FAILED_ERROR;
    }
  }

  if (VALID(I_agentStpMstBridgePriority, data->valid))  {
    if((usmDbDot1sMstiBridgePrioritySet(USMDB_UNIT_CURRENT, data->agentStpMstId,
                                                          &data->agentStpMstBridgePriority) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentStpMstBridgePriority, tempValid);
    }
  }

  if ((data->agentStpMstRowStatus == D_agentStpMstRowStatus_destroy)
      &&  (VALID(I_agentStpMstRowStatus, data->valid)))
  {
      if (usmDbDot1sInstanceGet(USMDB_UNIT_CURRENT, data->agentStpMstId) == L7_SUCCESS)
  {
    /* ignore if set to active */
    /* if set to destroy and destroy fails, return failure */
    if ((data->agentStpMstRowStatus != D_agentStpMstRowStatus_active)
              && ((usmDbDot1sMstiDelete(USMDB_UNIT_CURRENT, data->agentStpMstId) != L7_SUCCESS)))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }
      else
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
  }
  return NO_ERROR;
}

#ifdef SR_agentStpMstEntry_UNDO
/* add #define SR_agentStpMstEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentStpMstEntry family.
 */
int
agentStpMstEntry_undo(doList_t *doHead, doList_t *doCur,
                      ContextInfo *contextInfo)
{
  agentStpMstEntry_t *data = (agentStpMstEntry_t *) doCur->data;
  agentStpMstEntry_t *undodata = (agentStpMstEntry_t *) doCur->undodata;
  agentStpMstEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*Copy valid bits from data to undodata */
  if( undodata != NULL && data != NULL )
    memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add, a delete, or a modify */
  if ( undodata == NULL )
  {
    /* undoing an add, so delete */
    data->agentStpMstRowStatus = D_agentStpMstRowStatus_destroy;
    setdata = data;
    function = SR_DELETE;
  }
  else
  {
    /* undoing a delete or modify, replace the original data */
    if(undodata->agentStpMstRowStatus == D_agentStpMstRowStatus_notReady
    || undodata->agentStpMstRowStatus == D_agentStpMstRowStatus_notInService)
    {
      undodata->agentStpMstRowStatus = D_agentStpMstRowStatus_createAndWait;
    }
    else
    {
      if(undodata->agentStpMstRowStatus == D_agentStpMstRowStatus_active)
      {
        undodata->agentStpMstRowStatus = D_agentStpMstRowStatus_createAndGo;
      }
    }

    if (data->agentStpMstRowStatus == D_agentStpMstRowStatus_destroy)
    {
      /* Setting all valid bits to restore previous values */
      memset(undodata->valid, 0xff, sizeof(undodata->valid));
    }

    setdata = undodata;
    function = SR_ADD_MODIFY;
  }

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentStpMstEntry_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
#endif /* SR_agentStpMstEntry_UNDO */

#endif /* SETS */

agentStpMstPortEntry_t *
k_agentStpMstPortEntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           SR_UINT32 agentStpMstId,
                           SR_INT32 ifIndex)
{
  static agentStpMstPortEntry_t agentStpMstPortEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 snmp_buffer_len = SNMP_BUFFER_LEN;
  L7_uint32 bridgeIdLength;
  L7_uint32 intIfNum;
  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1S_COMPONENT_ID, L7_DOT1S_FEATURE_ID) == L7_FALSE)
  {
    return(NULL);
  }

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;

    agentStpMstPortEntryData.agentStpMstPortId = MakeOctetString(NULL, 0);
    agentStpMstPortEntryData.agentStpMstDesignatedBridgeId = MakeOctetString(NULL, 0);
    agentStpMstPortEntryData.agentStpMstDesignatedPortId = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(agentStpMstPortEntryData.valid);

  agentStpMstPortEntryData.agentStpMstId = agentStpMstId;
  SET_VALID(I_agentStpMstPortEntryIndex_agentStpMstId, agentStpMstPortEntryData.valid);
  agentStpMstPortEntryData.ifIndex = ifIndex;
  SET_VALID(I_agentStpMstPortEntryIndex_ifIndex, agentStpMstPortEntryData.valid);

  if ( ( (searchType == EXACT) ?
         (snmpAgentStpMstPortGet(USMDB_UNIT_CURRENT, agentStpMstPortEntryData.agentStpMstId, agentStpMstPortEntryData.ifIndex) != L7_SUCCESS) :
         (snmpAgentStpMstPortGet(USMDB_UNIT_CURRENT, agentStpMstPortEntryData.agentStpMstId, agentStpMstPortEntryData.ifIndex) != L7_SUCCESS) &&
         (snmpAgentStpMstPortNextGet(USMDB_UNIT_CURRENT, agentStpMstPortEntryData.agentStpMstId,
                                     &agentStpMstPortEntryData.agentStpMstId, &agentStpMstPortEntryData.ifIndex) != L7_SUCCESS)) ||
       (usmDbIntIfNumFromExtIfNum(agentStpMstPortEntryData.ifIndex, &intIfNum) != L7_SUCCESS))
  {
    ZERO_VALID(agentStpMstPortEntryData.valid);
    return(NULL);
  }

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpMstPortForwardingState:
    if (snmpAgentStpMstPortForwardingStateGet(USMDB_UNIT_CURRENT, agentStpMstPortEntryData.agentStpMstId, intIfNum,
                                              &agentStpMstPortEntryData.agentStpMstPortForwardingState) == L7_SUCCESS)
      SET_VALID(I_agentStpMstPortForwardingState, agentStpMstPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpMstPortId:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (snmpAgentStpMstPortIDGet(USMDB_UNIT_CURRENT, agentStpMstPortEntryData.agentStpMstId, intIfNum, snmp_buffer, snmp_buffer_len) == L7_SUCCESS &&
        (SafeMakeOctetString(&agentStpMstPortEntryData.agentStpMstPortId, snmp_buffer, 2) == L7_TRUE))
      SET_VALID(I_agentStpMstPortId, agentStpMstPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpMstPortPathCost:
    if (usmDbDot1sMstiPortPathCostGet(USMDB_UNIT_CURRENT, agentStpMstPortEntryData.agentStpMstId, intIfNum,
                                      &agentStpMstPortEntryData.agentStpMstPortPathCost) == L7_SUCCESS)
      SET_VALID(I_agentStpMstPortPathCost, agentStpMstPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpMstPortPriority:
    if (usmDbDot1sMstiPortPriorityGet(USMDB_UNIT_CURRENT, agentStpMstPortEntryData.agentStpMstId, intIfNum,
                                      &agentStpMstPortEntryData.agentStpMstPortPriority) == L7_SUCCESS)
      SET_VALID(I_agentStpMstPortPriority, agentStpMstPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpMstDesignatedBridgeId:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbDot1sMstiDesignatedBridgeIDGet(USMDB_UNIT_CURRENT, agentStpMstPortEntryData.agentStpMstId, intIfNum, snmp_buffer, &bridgeIdLength) == L7_SUCCESS &&
        (SafeMakeOctetString(&agentStpMstPortEntryData.agentStpMstDesignatedBridgeId, snmp_buffer, bridgeIdLength) == L7_TRUE))
      SET_VALID(I_agentStpMstDesignatedBridgeId, agentStpMstPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpMstDesignatedCost:
    if (usmDbDot1sMstiDesignatedCostGet(USMDB_UNIT_CURRENT, agentStpMstPortEntryData.agentStpMstId, intIfNum, &agentStpMstPortEntryData.agentStpMstDesignatedCost) == L7_SUCCESS)
      SET_VALID(I_agentStpMstDesignatedCost, agentStpMstPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpMstDesignatedPortId:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (snmpAgentStpMstDesignatedPortIDGet(USMDB_UNIT_CURRENT, agentStpMstPortEntryData.agentStpMstId, intIfNum, snmp_buffer, snmp_buffer_len) == L7_SUCCESS &&
        (SafeMakeOctetString(&agentStpMstPortEntryData.agentStpMstDesignatedPortId, snmp_buffer, 2) == L7_TRUE))
      SET_VALID(I_agentStpMstDesignatedPortId, agentStpMstPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */
  case I_agentStpMstPortLoopInconsistentState:
    if (snmpAgentStpMstPortLoopInconsistentStateGet(USMDB_UNIT_CURRENT, agentStpMstPortEntryData.agentStpMstId, intIfNum,
                                                    &agentStpMstPortEntryData.agentStpMstPortLoopInconsistentState) == L7_SUCCESS)
      SET_VALID(I_agentStpMstPortLoopInconsistentState, agentStpMstPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpMstPortTransitionsIntoLoopInconsistentState:
    if (usmDbDot1sPortStatsTransitionsIntoLoopInconsistentStateGet(USMDB_UNIT_CURRENT, agentStpMstPortEntryData.agentStpMstId, intIfNum,
                                                    &agentStpMstPortEntryData.agentStpMstPortTransitionsIntoLoopInconsistentState) == L7_SUCCESS)
      SET_VALID(I_agentStpMstPortTransitionsIntoLoopInconsistentState, agentStpMstPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpMstPortTransitionsOutOfLoopInconsistentState:
    if (usmDbDot1sPortStatsTransitionsOutOfLoopInconsistentStateGet(USMDB_UNIT_CURRENT, agentStpMstPortEntryData.agentStpMstId, intIfNum,
                                                    &agentStpMstPortEntryData.agentStpMstPortTransitionsOutOfLoopInconsistentState) == L7_SUCCESS)
      SET_VALID(I_agentStpMstPortTransitionsOutOfLoopInconsistentState, agentStpMstPortEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */
  case I_agentStpMstPortEntryIndex_agentStpMstId:
  case I_agentStpMstPortEntryIndex_ifIndex:
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentStpMstPortEntryData.valid) )
    return(NULL);

   return(&agentStpMstPortEntryData);
}

#ifdef SETS
int
k_agentStpMstPortEntry_test(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentStpMstPortEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                             doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentStpMstPortEntry_set_defaults(doList_t *dp)
{
    agentStpMstPortEntry_t *data = (agentStpMstPortEntry_t *) (dp->data);

    if ((data->agentStpMstPortId = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->agentStpMstDesignatedBridgeId = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->agentStpMstDesignatedPortId = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentStpMstPortEntry_set(agentStpMstPortEntry_t *data,
                           ContextInfo *contextInfo, int function)
{
  L7_uint32 intIfNum;
  L7_RC_t rc;

  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1S_COMPONENT_ID, L7_DOT1S_FEATURE_ID) == L7_FALSE)
  {
    return COMMIT_FAILED_ERROR;
  }

  /* Conversion to internal interface number for usmdb calls */
  rc = usmDbIntIfNumFromExtIfNum(data->ifIndex, &intIfNum);
  if (rc == L7_SUCCESS)
  {
    if (VALID(I_agentStpMstPortPathCost, data->valid))
    {
      if( usmDbDot1sMstiPortPathCostSet(USMDB_UNIT_CURRENT, data->agentStpMstId, intIfNum, data->agentStpMstPortPathCost) != L7_SUCCESS)
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
      else
      {
        SET_VALID(I_agentStpMstPortPathCost, tempValid);
      }
    }

    if (VALID(I_agentStpMstPortPriority, data->valid))
    {
      if( usmDbDot1sMstiPortPrioritySet(USMDB_UNIT_CURRENT, data->agentStpMstId, intIfNum, &data->agentStpMstPortPriority) != L7_SUCCESS)
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
    }
    return NO_ERROR;
  }

  return COMMIT_FAILED_ERROR;
}

#ifdef SR_agentStpMstPortEntry_UNDO
/* add #define SR_agentStpMstPortEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentStpMstPortEntry family.
 */
int
agentStpMstPortEntry_undo(doList_t *doHead, doList_t *doCur,
                          ContextInfo *contextInfo)
{
  agentStpMstPortEntry_t *data = (agentStpMstPortEntry_t *) doCur->data;
  agentStpMstPortEntry_t *undodata = (agentStpMstPortEntry_t *) doCur->undodata;
  agentStpMstPortEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL)
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentStpMstPortEntry_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
#endif /* SR_agentStpMstPortEntry_UNDO */

#endif /* SETS */

agentStpMstVlanEntry_t *
k_agentStpMstVlanEntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           SR_UINT32 agentStpMstId,
                           SR_UINT32 dot1qVlanIndex)
{
  static agentStpMstVlanEntry_t agentStpMstVlanEntryData;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1S_COMPONENT_ID, L7_DOT1S_FEATURE_ID) == L7_FALSE)
  {
    return(NULL);
  }

  ZERO_VALID(agentStpMstVlanEntryData.valid);

  agentStpMstVlanEntryData.agentStpMstId = agentStpMstId;
  SET_VALID(I_agentStpMstVlanEntryIndex_agentStpMstId, agentStpMstVlanEntryData.valid);
  agentStpMstVlanEntryData.dot1qVlanIndex = dot1qVlanIndex;
  SET_VALID(I_agentStpMstVlanEntryIndex_dot1qVlanIndex, agentStpMstVlanEntryData.valid);

  if ( (searchType == EXACT) ?
       usmDbDot1sMstiVIDGet(USMDB_UNIT_CURRENT, agentStpMstVlanEntryData.agentStpMstId, agentStpMstVlanEntryData.dot1qVlanIndex) != L7_SUCCESS :
       (usmDbDot1sMstiVIDGet(USMDB_UNIT_CURRENT, agentStpMstVlanEntryData.agentStpMstId, agentStpMstVlanEntryData.dot1qVlanIndex) != L7_SUCCESS &&
        usmDbDot1sInstanceVlanNextGet(USMDB_UNIT_CURRENT, agentStpMstVlanEntryData.agentStpMstId, &agentStpMstVlanEntryData.agentStpMstId,
                                      agentStpMstVlanEntryData.dot1qVlanIndex, &agentStpMstVlanEntryData.dot1qVlanIndex) != L7_SUCCESS) )
  {
    ZERO_VALID(agentStpMstVlanEntryData.valid);
    return(NULL);
  }

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpMstVlanRowStatus:
    /* if entry shows up, it's active */
    agentStpMstVlanEntryData.agentStpMstVlanRowStatus = D_agentStpMstVlanRowStatus_active;
    SET_VALID(I_agentStpMstVlanRowStatus, agentStpMstVlanEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentStpMstVlanEntryIndex_agentStpMstId:
  case I_agentStpMstVlanEntryIndex_dot1qVlanIndex:
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentStpMstVlanEntryData.valid) )
    return(NULL);

   return(&agentStpMstVlanEntryData);
}

#ifdef SETS
int
k_agentStpMstVlanEntry_test(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentStpMstVlanEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                             doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentStpMstVlanEntry_set_defaults(doList_t *dp)
{
    agentStpMstVlanEntry_t *data = (agentStpMstVlanEntry_t *) (dp->data);


    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentStpMstVlanEntry_set(agentStpMstVlanEntry_t *data,
                           ContextInfo *contextInfo, int function)
{
   L7_BOOL isNewRowCreated = L7_FALSE;
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1S_COMPONENT_ID, L7_DOT1S_FEATURE_ID) == L7_FALSE)
  {
    return COMMIT_FAILED_ERROR;
  }

  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_DOT1S_COMPONENT_ID) == L7_FALSE)
    return COMMIT_FAILED_ERROR;

  if ( (data->agentStpMstId == L7_NULL) || (data->dot1qVlanIndex == L7_NULL) )
    return COMMIT_FAILED_ERROR;

  if (usmDbDot1sMstiVIDGet(USMDB_UNIT_CURRENT, data->agentStpMstId, data->dot1qVlanIndex) != L7_SUCCESS)
  {
    /* add vlan to this entry */
    if (VALID(I_agentStpMstVlanRowStatus, data->valid))
    {
      if( data->agentStpMstVlanRowStatus == D_agentStpMstVlanRowStatus_createAndGo &&
        usmDbDot1sMstiVlanAdd(USMDB_UNIT_CURRENT, data->agentStpMstId, data->dot1qVlanIndex) != L7_SUCCESS)
      {
        /* don't set the status */
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
      else
      {
        SET_VALID(I_agentStpMstVlanRowStatus, tempValid);
        isNewRowCreated = L7_TRUE;
      }
    }
  }

  if (VALID(I_agentStpMstVlanRowStatus, data->valid) &&
      !( data->agentStpMstVlanRowStatus == D_agentStpMstVlanRowStatus_createAndGo))
  {
    /* ignore if set to active */
    /* if set to destroy and destroy fails, return failure */
    if (data->agentStpMstVlanRowStatus != D_agentStpMstVlanRowStatus_active && (isNewRowCreated != L7_TRUE)
        && (data->agentStpMstVlanRowStatus != D_agentStpMstVlanRowStatus_destroy ||
         usmDbDot1sMstiVlanRemove(USMDB_UNIT_CURRENT, data->agentStpMstId, data->dot1qVlanIndex) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;
}

#ifdef SR_agentStpMstVlanEntry_UNDO
/* add #define SR_agentStpMstVlanEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentStpMstVlanEntry family.
 */
int
agentStpMstVlanEntry_undo(doList_t *doHead, doList_t *doCur,
                          ContextInfo *contextInfo)
{
  agentStpMstVlanEntry_t *data = (agentStpMstVlanEntry_t *) doCur->data;
  agentStpMstVlanEntry_t *undodata = (agentStpMstVlanEntry_t *) doCur->undodata;
  agentStpMstVlanEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*Copy valid bits from data to undodata */
  if( undodata != NULL && data != NULL )
    memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add, a delete, or a modify */
  if ( undodata == NULL )
  {
    /* undoing an add, so delete */
    data->agentStpMstVlanRowStatus = D_agentStpMstVlanRowStatus_destroy;
    setdata = data;
    function = SR_DELETE;
  }
  else
  {
    /* undoing a delete or modify, replace the original data */
    if(undodata->agentStpMstVlanRowStatus == D_agentStpMstVlanRowStatus_notReady
    || undodata->agentStpMstVlanRowStatus == D_agentStpMstVlanRowStatus_notInService)
    {
        undodata->agentStpMstVlanRowStatus = D_agentStpMstVlanRowStatus_createAndWait;
    }
    else
    {
      if(undodata->agentStpMstVlanRowStatus == D_agentStpMstVlanRowStatus_active)
      {
        undodata->agentStpMstVlanRowStatus = D_agentStpMstVlanRowStatus_createAndGo;
      }
      setdata = undodata;
      function = SR_ADD_MODIFY;
    }
  }

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentStpMstVlanEntry_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
#endif /* SR_agentStpMstVlanEntry_UNDO */

#endif /* SETS */

agentSwitchStormControlGroup_t *
k_agentSwitchStormControlGroup_get(int serialNum, ContextInfo *contextInfo,
                             int nominator)
{
  static agentSwitchStormControlGroup_t agentSwitchStormControlGroupData;

  ZERO_VALID(agentSwitchStormControlGroupData.valid);

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
    if (nominator != -1) break;
    /* else pass through */

  case I_agentSwitchBroadcastControlMode :
    CLR_VALID(I_agentSwitchBroadcastControlMode, agentSwitchStormControlGroupData.valid);
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID, L7_POLICY_BCAST_CONTROL_FEATURE_ID) == L7_TRUE)
    {
    if (snmpAgentSwitchBroadcastControlModeGet(USMDB_UNIT_CURRENT,
                                                  &agentSwitchStormControlGroupData.agentSwitchBroadcastControlMode) == L7_SUCCESS)
         SET_VALID(I_agentSwitchBroadcastControlMode, agentSwitchStormControlGroupData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */
  case I_agentSwitchMulticastControlMode :
    CLR_VALID(I_agentSwitchMulticastControlMode, agentSwitchStormControlGroupData.valid);
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID, L7_POLICY_MCAST_CONTROL_FEATURE_ID) == L7_TRUE)
    {
       if (snmpAgentSwitchMulticastControlModeGet(USMDB_UNIT_CURRENT,
                                                  &agentSwitchStormControlGroupData.agentSwitchMulticastControlMode) == L7_SUCCESS)
         SET_VALID(I_agentSwitchMulticastControlMode, agentSwitchStormControlGroupData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */
  case I_agentSwitchUnicastControlMode :
    CLR_VALID(I_agentSwitchUnicastControlMode, agentSwitchStormControlGroupData.valid);
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID, L7_POLICY_UCAST_CONTROL_FEATURE_ID) == L7_TRUE)
    {
       if (snmpAgentSwitchUnicastControlModeGet(USMDB_UNIT_CURRENT,
                                                  &agentSwitchStormControlGroupData.agentSwitchUnicastControlMode) == L7_SUCCESS)
         SET_VALID(I_agentSwitchUnicastControlMode, agentSwitchStormControlGroupData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */

  case I_agentSwitchBroadcastControlThreshold :
    CLR_VALID(I_agentSwitchBroadcastControlThreshold, agentSwitchStormControlGroupData.valid);
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID, L7_POLICY_BCAST_CONTROL_FEATURE_ID) == L7_TRUE)
    {
       if (snmpAgentSwitchBroadcastControlThresholdGet(USMDB_UNIT_CURRENT,
                                                  &agentSwitchStormControlGroupData.agentSwitchBroadcastControlThreshold) == L7_SUCCESS)
         SET_VALID(I_agentSwitchBroadcastControlThreshold, agentSwitchStormControlGroupData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */
  case I_agentSwitchMulticastControlThreshold :
    CLR_VALID(I_agentSwitchMulticastControlThreshold, agentSwitchStormControlGroupData.valid);
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID, L7_POLICY_MCAST_CONTROL_FEATURE_ID) == L7_TRUE)
    {
       if (snmpAgentSwitchMulticastControlThresholdGet(USMDB_UNIT_CURRENT,
                                                  &agentSwitchStormControlGroupData.agentSwitchMulticastControlThreshold) == L7_SUCCESS)
         SET_VALID(I_agentSwitchMulticastControlThreshold, agentSwitchStormControlGroupData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */
  case I_agentSwitchUnicastControlThreshold :
    CLR_VALID(I_agentSwitchUnicastControlThreshold, agentSwitchStormControlGroupData.valid);
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID, L7_POLICY_UCAST_CONTROL_FEATURE_ID) == L7_TRUE)
    {
       if (snmpAgentSwitchUnicastControlThresholdGet(USMDB_UNIT_CURRENT,
                                                  &agentSwitchStormControlGroupData.agentSwitchUnicastControlThreshold) == L7_SUCCESS)
         SET_VALID(I_agentSwitchUnicastControlThreshold, agentSwitchStormControlGroupData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */

  case I_agentSwitchBroadcastControlThresholdUnit :
    CLR_VALID(I_agentSwitchBroadcastControlThresholdUnit, agentSwitchStormControlGroupData.valid);
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID, L7_POLICY_BCAST_CONTROL_FEATURE_ID) == L7_TRUE)
    {                      
       if (snmpAgentSwitchBroadcastControlThresholdUnitGet(USMDB_UNIT_CURRENT, 
                                                  &agentSwitchStormControlGroupData.agentSwitchBroadcastControlThresholdUnit) == L7_SUCCESS)
         SET_VALID(I_agentSwitchBroadcastControlThresholdUnit, agentSwitchStormControlGroupData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */
  case I_agentSwitchMulticastControlThresholdUnit :
    CLR_VALID(I_agentSwitchMulticastControlThresholdUnit, agentSwitchStormControlGroupData.valid);
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID, L7_POLICY_MCAST_CONTROL_FEATURE_ID) == L7_TRUE)
    {                      
       if (snmpAgentSwitchMulticastControlThresholdUnitGet(USMDB_UNIT_CURRENT, 
                                                  &agentSwitchStormControlGroupData.agentSwitchMulticastControlThresholdUnit) == L7_SUCCESS)
         SET_VALID(I_agentSwitchMulticastControlThresholdUnit, agentSwitchStormControlGroupData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */
  case I_agentSwitchUnicastControlThresholdUnit :
    CLR_VALID(I_agentSwitchUnicastControlThresholdUnit, agentSwitchStormControlGroupData.valid);
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID, L7_POLICY_UCAST_CONTROL_FEATURE_ID) == L7_TRUE)
    {                      
       if (snmpAgentSwitchUnicastControlThresholdUnitGet(USMDB_UNIT_CURRENT, 
                                                  &agentSwitchStormControlGroupData.agentSwitchUnicastControlThresholdUnit) == L7_SUCCESS)
         SET_VALID(I_agentSwitchUnicastControlThresholdUnit, agentSwitchStormControlGroupData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */

  case I_agentSwitchDot3FlowControlMode :
    CLR_VALID(I_agentSwitchDot3FlowControlMode, agentSwitchStormControlGroupData.valid);
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_FLOW_CONTROL_FEATURE_ID) == L7_FALSE)
    {
      if (snmpAgentSwitchDot3FlowControlModeGet(USMDB_UNIT_CURRENT,
                                                &agentSwitchStormControlGroupData.agentSwitchDot3FlowControlMode) == L7_SUCCESS)
        SET_VALID(I_agentSwitchDot3FlowControlMode, agentSwitchStormControlGroupData.valid);
    }
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentSwitchStormControlGroupData.valid) )
    return(NULL);

  return(&agentSwitchStormControlGroupData);
}

#ifdef SETS
int
k_agentSwitchStormControlGroup_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{
  ZERO_VALID(((agentSwitchStormControlGroup_t *) (dp->data))->valid);
  return NO_ERROR;
}

int
k_agentSwitchStormControlGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                               doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentSwitchStormControlGroup_set(agentSwitchStormControlGroup_t *data,
                             ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if ( VALID(I_agentSwitchBroadcastControlMode, data->valid))
  {
    if(( usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID, L7_POLICY_BCAST_CONTROL_FEATURE_ID) == L7_FALSE ||
         snmpAgentSwitchBroadcastControlModeSet(USMDB_UNIT_CURRENT,
                                            data->agentSwitchBroadcastControlMode) != L7_SUCCESS ))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentSwitchBroadcastControlMode, tempValid);
    }
  }
  if ( VALID(I_agentSwitchMulticastControlMode, data->valid))
  {
    if(( usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID, L7_POLICY_MCAST_CONTROL_FEATURE_ID) == L7_FALSE ||
         snmpAgentSwitchMulticastControlModeSet(USMDB_UNIT_CURRENT,
                                            data->agentSwitchMulticastControlMode) != L7_SUCCESS ))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentSwitchMulticastControlMode, tempValid);
    }
  }
  if ( VALID(I_agentSwitchUnicastControlMode, data->valid))
  {
    if(( usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID, L7_POLICY_UCAST_CONTROL_FEATURE_ID) == L7_FALSE ||
         snmpAgentSwitchUnicastControlModeSet(USMDB_UNIT_CURRENT,
                                            data->agentSwitchUnicastControlMode) != L7_SUCCESS ))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentSwitchUnicastControlMode, tempValid);
    }
  }


  if ( VALID(I_agentSwitchBroadcastControlThreshold, data->valid))
  {
    if(( usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID, L7_POLICY_BCAST_CONTROL_FEATURE_ID) == L7_FALSE ||
         snmpAgentSwitchBroadcastControlThresholdSet(USMDB_UNIT_CURRENT,
                                               data->agentSwitchBroadcastControlThreshold) != L7_SUCCESS ))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentSwitchBroadcastControlThreshold, tempValid);
    }
  }
  if ( VALID(I_agentSwitchMulticastControlThreshold, data->valid))
  {
    if(( usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID, L7_POLICY_MCAST_CONTROL_FEATURE_ID) == L7_FALSE ||
         snmpAgentSwitchMulticastControlThresholdSet(USMDB_UNIT_CURRENT,
                                             data->agentSwitchMulticastControlThreshold) != L7_SUCCESS ))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentSwitchMulticastControlThreshold, tempValid);
    }
  }
  if ( VALID(I_agentSwitchUnicastControlThreshold, data->valid))
  {
    if(( usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID, L7_POLICY_UCAST_CONTROL_FEATURE_ID) == L7_FALSE ||
         snmpAgentSwitchUnicastControlThresholdSet(USMDB_UNIT_CURRENT,
                                              data->agentSwitchUnicastControlThreshold) != L7_SUCCESS ))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentSwitchUnicastControlThreshold, tempValid);
    }
  }

  if ( VALID(I_agentSwitchBroadcastControlThresholdUnit, data->valid)) 
  {
    if(( usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID, L7_POLICY_BCAST_CONTROL_FEATURE_ID) == L7_FALSE || 
         snmpAgentSwitchBroadcastControlThresholdUnitSet(USMDB_UNIT_CURRENT, 
                                              data->agentSwitchBroadcastControlThresholdUnit) != L7_SUCCESS ))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentSwitchBroadcastControlThresholdUnit, tempValid);
    }
  }

  if ( VALID(I_agentSwitchMulticastControlThresholdUnit, data->valid)) 
  {
    if(( usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID, L7_POLICY_MCAST_CONTROL_FEATURE_ID) == L7_FALSE || 
         snmpAgentSwitchMulticastControlThresholdUnitSet(USMDB_UNIT_CURRENT, 
                                              data->agentSwitchMulticastControlThresholdUnit) != L7_SUCCESS ))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentSwitchMulticastControlThresholdUnit, tempValid);
    }
  }

  if ( VALID(I_agentSwitchUnicastControlThresholdUnit, data->valid)) 
  {
    if(( usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID, L7_POLICY_UCAST_CONTROL_FEATURE_ID) == L7_FALSE || 
         snmpAgentSwitchUnicastControlThresholdUnitSet(USMDB_UNIT_CURRENT, 
                                              data->agentSwitchUnicastControlThresholdUnit) != L7_SUCCESS ))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentSwitchUnicastControlThresholdUnit, tempValid);
    }
  }

  if ( VALID(I_agentSwitchDot3FlowControlMode, data->valid))
  {
    if(( usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_FLOW_CONTROL_FEATURE_ID) == L7_TRUE ||
       snmpAgentSwitchDot3FlowControlModeSet(USMDB_UNIT_CURRENT,
                                               data->agentSwitchDot3FlowControlMode) != L7_SUCCESS ))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
  }

  return NO_ERROR;
}

  #ifdef SR_agentSwitchStormControlGroup_UNDO
/* add #define SR_agentSwitchConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentSwitchConfigGroup family.
 */
int
agentSwitchStormControlGroup_undo(doList_t *doHead, doList_t *doCur,
                            ContextInfo *contextInfo)
{
  agentSwitchStormControlGroup_t *data = (agentSwitchStormControlGroup_t *) doCur->data;
  agentSwitchStormControlGroup_t *undodata = (agentSwitchStormControlGroup_t *) doCur->undodata;
  agentSwitchStormControlGroup_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentSwitchStormControlGroup_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentSwitchConfigGroup_UNDO */

#endif /* SETS */

agentSwitchAddressAgingTimeoutEntry_t *
k_agentSwitchAddressAgingTimeoutEntry_get(int serialNum, ContextInfo *contextInfo,
                                          int nominator,
                                          int searchType,
                                          SR_UINT32 dot1qFdbId)
{
  static agentSwitchAddressAgingTimeoutEntry_t agentSwitchAddressAgingTimeoutEntryData;
  L7_FDB_TYPE_t vlType;

  /* Ensure IVL is supported */
  if ( (usmDbFDBTypeOfVLGet(USMDB_UNIT_CURRENT, &vlType) != L7_SUCCESS) ||
       (vlType != L7_IVL) )
  {
    ZERO_VALID(agentSwitchAddressAgingTimeoutEntryData.valid);
    return(NULL);
  }

  agentSwitchAddressAgingTimeoutEntryData.dot1qFdbId = dot1qFdbId;
  SET_VALID(I_agentSwitchAddressAgingTimeoutEntryIndex_dot1qFdbId,
            agentSwitchAddressAgingTimeoutEntryData.valid);

  if ( (searchType == EXACT) ?
       (usmDbFdbIdGet(USMDB_UNIT_CURRENT, agentSwitchAddressAgingTimeoutEntryData.dot1qFdbId) != L7_SUCCESS) :
       ( (usmDbFdbIdGet(USMDB_UNIT_CURRENT, agentSwitchAddressAgingTimeoutEntryData.dot1qFdbId) != L7_SUCCESS) &&
         (usmDbFdbIdNextGet(USMDB_UNIT_CURRENT, agentSwitchAddressAgingTimeoutEntryData.dot1qFdbId, &agentSwitchAddressAgingTimeoutEntryData.dot1qFdbId) != L7_SUCCESS) ) )
  {
    ZERO_VALID(agentSwitchAddressAgingTimeoutEntryData.valid);
    return(NULL);
  }

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
  case I_agentSwitchAddressAgingTimeoutEntryIndex_dot1qFdbId:
    if (nominator != -1) break;
    /* else pass through */

  case I_agentSwitchAddressAgingTimeout :
    CLR_VALID(I_agentSwitchAddressAgingTimeout, agentSwitchAddressAgingTimeoutEntryData.valid);
    if (usmDbFDBAddressAgingTimeoutGet(USMDB_UNIT_CURRENT,
                                       agentSwitchAddressAgingTimeoutEntryData.dot1qFdbId,
                                       &agentSwitchAddressAgingTimeoutEntryData.agentSwitchAddressAgingTimeout) == L7_SUCCESS)
      SET_VALID(I_agentSwitchAddressAgingTimeout, agentSwitchAddressAgingTimeoutEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentSwitchAddressAgingTimeoutEntryData.valid) )
    return(NULL);

  return(&agentSwitchAddressAgingTimeoutEntryData);
}

#ifdef SETS
int
k_agentSwitchAddressAgingTimeoutEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                           doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentSwitchAddressAgingTimeoutEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                            doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentSwitchAddressAgingTimeoutEntry_set_defaults(doList_t *dp)
{
  agentSwitchAddressAgingTimeoutEntry_t *data = (agentSwitchAddressAgingTimeoutEntry_t *) (dp->data);

  data->agentSwitchAddressAgingTimeout = 300;

  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_agentSwitchAddressAgingTimeoutEntry_set(agentSwitchAddressAgingTimeoutEntry_t *data,
                                          ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (usmDbFdbIdGet(USMDB_UNIT_CURRENT, data->dot1qFdbId) != L7_SUCCESS)
  {
    ZERO_VALID(data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentSwitchAddressAgingTimeout, data->valid))
  {
    if( usmDbFDBAddressAgingTimeoutSet(USMDB_UNIT_CURRENT, data->dot1qFdbId,
                                     data->agentSwitchAddressAgingTimeout) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;
}

  #ifdef SR_agentSwitchAddressAgingTimeoutEntry_UNDO
/* add #define SR_agentSwitchAddressAgingTimeoutEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentSwitchAddressAgingTimeoutEntry family.
 */
int
agentSwitchAddressAgingTimeoutEntry_undo(doList_t *doHead, doList_t *doCur,
                                         ContextInfo *contextInfo)
{
  agentSwitchAddressAgingTimeoutEntry_t *data = (agentSwitchAddressAgingTimeoutEntry_t *) doCur->data;
  agentSwitchAddressAgingTimeoutEntry_t *undodata = (agentSwitchAddressAgingTimeoutEntry_t *) doCur->undodata;
  agentSwitchAddressAgingTimeoutEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentSwitchAddressAgingTimeoutEntry_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentSwitchAddressAgingTimeoutEntry_UNDO */

#endif /* SETS */


agentSwitchSnoopingCfgEntry_t*
k_agentSwitchSnoopingCfgEntry_get(int serialNum, ContextInfo *contextInfo,
                                 int nominator,
                                 int searchType,
                                 SR_INT32 family)
{
  static agentSwitchSnoopingCfgEntry_t agentSwitchSnoopingGroupData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uchar8 tmpFamily;
  L7_uint32 snmp_buffer_len = SNMP_BUFFER_LEN;

  tmpFamily = (L7_uchar8)family;

  ZERO_VALID(agentSwitchSnoopingGroupData.valid);

  agentSwitchSnoopingGroupData.agentSwitchSnoopingProtocol = family;
  SET_VALID(I_agentSwitchSnoopingProtocol, agentSwitchSnoopingGroupData.valid);

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;

    agentSwitchSnoopingGroupData.agentSwitchSnoopingPortMask = MakeOctetString(NULL, 0);
  }

  /* Validating the row-entries for GET and GETNEXT operations */
  if (searchType == EXACT)
  {
    if(usmDbSnoopProtocolGet(tmpFamily)
                             != L7_TRUE)
    {
      ZERO_VALID(agentSwitchSnoopingGroupData.valid);
      return(NULL);
    }

  }
  else if (searchType == NEXT)
  {
    if(usmDbSnoopProtocolGet(tmpFamily) != L7_TRUE)
    {
      if(usmDbSnoopProtocolNextGet(tmpFamily, &tmpFamily)
                                  != L7_TRUE)
      {
       ZERO_VALID(agentSwitchSnoopingGroupData.valid);
        return(NULL);
      }
    }
    agentSwitchSnoopingGroupData.agentSwitchSnoopingProtocol = tmpFamily;
  }


  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
    if ( nominator != -1) break;
    /* else pass through */

  case I_agentSwitchSnoopingProtocol:
    agentSwitchSnoopingGroupData.agentSwitchSnoopingProtocol = tmpFamily;
    SET_VALID(I_agentSwitchSnoopingProtocol, agentSwitchSnoopingGroupData.valid);
    if (nominator != -1) break;

  case I_agentSwitchSnoopingAdminMode:
    if (snmpAgentSwitchSnoopingAdminModeGet(USMDB_UNIT_CURRENT,
                                            &agentSwitchSnoopingGroupData.agentSwitchSnoopingAdminMode,
                                            agentSwitchSnoopingGroupData.agentSwitchSnoopingProtocol)
                                            == L7_SUCCESS)
      SET_VALID(I_agentSwitchSnoopingAdminMode, agentSwitchSnoopingGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentSwitchSnoopingPortMask:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbSnoopIntfEnabledMaskGet(USMDB_UNIT_CURRENT, snmp_buffer, &snmp_buffer_len,
                                     agentSwitchSnoopingGroupData.agentSwitchSnoopingProtocol) == L7_SUCCESS &&
        SafeMakeOctetString(&agentSwitchSnoopingGroupData.agentSwitchSnoopingPortMask, snmp_buffer, snmp_buffer_len) != L7_ERROR)
      SET_VALID(I_agentSwitchSnoopingPortMask, agentSwitchSnoopingGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentSwitchSnoopingMulticastControlFramesProcessed:
    if (usmDbSnoopControlFramesProcessed(USMDB_UNIT_CURRENT,
                                         &agentSwitchSnoopingGroupData.agentSwitchSnoopingMulticastControlFramesProcessed,
                                         agentSwitchSnoopingGroupData.agentSwitchSnoopingProtocol)
                                         == L7_SUCCESS)
      SET_VALID(I_agentSwitchSnoopingMulticastControlFramesProcessed, agentSwitchSnoopingGroupData.valid);
    break;

  default:
    /* unknown nominator value */
    return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, agentSwitchSnoopingGroupData.valid))
    return(NULL);

  return (&agentSwitchSnoopingGroupData);
}

#ifdef SETS
int
k_agentSwitchSnoopingCfgEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                    doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentSwitchSnoopingCfgEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                     doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentSwitchSnoopingCfgEntry_set(agentSwitchSnoopingCfgEntry_t *data,
                                   ContextInfo *contextInfo, int function)
{
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 snmp_buffer_len;
  L7_uchar8 family = L7_AF_INET;
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));
  family = (L7_uchar8)data->agentSwitchSnoopingProtocol;

  /*
  * Check if MLD Snooping feature is supported.
  */
  if (family == L7_AF_INET6) 
  {
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNOOPING_COMPONENT_ID,
                                 L7_MLD_SNOOPING_FEATURE_SUPPORTED) == L7_FALSE)
    {
      return COMMIT_FAILED_ERROR;
    }
  }

  if (VALID(I_agentSwitchSnoopingAdminMode, data->valid))
  {
    if( snmpAgentSwitchSnoopingAdminModeSet(USMDB_UNIT_CURRENT,
                                            data->agentSwitchSnoopingAdminMode,
                                            family) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentSwitchSnoopingAdminMode, tempValid);
    }
  }

  if (VALID(I_agentSwitchSnoopingPortMask, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentSwitchSnoopingPortMask->octet_ptr, data->agentSwitchSnoopingPortMask->length);
    snmp_buffer_len = data->agentSwitchSnoopingPortMask->length;

    if (usmDbSnoopIntfEnabledMaskSet(USMDB_UNIT_CURRENT, snmp_buffer, &snmp_buffer_len, family) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;
}

#ifdef SR_agentSwitchSnoopingCfgEntry_UNDO
/* add #define SR_agentSwitchSnoopingCfgEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentSwitchSnoopingCfgEntry family.
 */
int
agentSwitchSnoopingCfgEntry_undo(doList_t *doHead, doList_t *doCur,
                                  ContextInfo *contextInfo)
{
  agentSwitchSnoopingCfgEntry_t *data = (agentSwitchSnoopingCfgEntry_t *) doCur->data;
  agentSwitchSnoopingCfgEntry_t *undodata = (agentSwitchSnoopingCfgEntry_t *) doCur->undodata;
  agentSwitchSnoopingCfgEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentSwitchSnoopingCfgEntry_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
#endif /* SR_agentSwitchSnoopingGroup_UNDO */

#endif /* SETS */

agentSwitchMFDBGroup_t *
k_agentSwitchMFDBGroup_get(int serialNum, ContextInfo *contextInfo,
                           int nominator)
{
  static agentSwitchMFDBGroup_t agentSwitchMFDBGroupData;

  ZERO_VALID(agentSwitchMFDBGroupData.valid);

  switch (nominator)
  {
  case I_agentSwitchMFDBMaxTableEntries:
    if (usmDbMfdbMaxTableEntriesGet(USMDB_UNIT_CURRENT,
                                 &agentSwitchMFDBGroupData.agentSwitchMFDBMaxTableEntries) == L7_SUCCESS)
      SET_VALID(I_agentSwitchMFDBMaxTableEntries, agentSwitchMFDBGroupData.valid);
    break;

  case I_agentSwitchMFDBMostEntriesUsed:
    if (usmDbMfdbMostEntriesGet(USMDB_UNIT_CURRENT,
                                &agentSwitchMFDBGroupData.agentSwitchMFDBMostEntriesUsed) == L7_SUCCESS)
      SET_VALID(I_agentSwitchMFDBMostEntriesUsed, agentSwitchMFDBGroupData.valid);
    break;

  case I_agentSwitchMFDBCurrentEntries:
    if (usmDbMfdbCurrEntriesGet(USMDB_UNIT_CURRENT,
                                &agentSwitchMFDBGroupData.agentSwitchMFDBCurrentEntries) == L7_SUCCESS)
      SET_VALID(I_agentSwitchMFDBCurrentEntries, agentSwitchMFDBGroupData.valid);
    break;

  default:
    break;
  }

  if (nominator && !VALID(nominator, agentSwitchMFDBGroupData.valid))
    return(NULL);

  return(&agentSwitchMFDBGroupData);
}

agentSwitchMFDBEntry_t *
k_agentSwitchMFDBEntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           SR_UINT32 agentSwitchMFDBVlanId,
                           OctetString * agentSwitchMFDBMacAddress,
                           SR_INT32 agentSwitchMFDBProtocolType)
{
  static agentSwitchMFDBEntry_t agentSwitchMFDBEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer_MacAddress[SNMP_BUFFER_LEN];
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 snmp_buffer_len=0;

  if (firstTime == L7_TRUE)
  {
   firstTime = L7_FALSE;

   agentSwitchMFDBEntryData.agentSwitchMFDBMacAddress = MakeOctetString(NULL, 0);
   agentSwitchMFDBEntryData.agentSwitchMFDBDescription = MakeOctetString(NULL, 0);
   agentSwitchMFDBEntryData.agentSwitchMFDBForwardingPortMask = MakeOctetString(NULL, 0);
   agentSwitchMFDBEntryData.agentSwitchMFDBFilteringPortMask = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(agentSwitchMFDBEntryData.valid);

  agentSwitchMFDBEntryData.agentSwitchMFDBVlanId = agentSwitchMFDBVlanId;
  SET_VALID(I_agentSwitchMFDBVlanId, agentSwitchMFDBEntryData.valid);

  bzero(snmp_buffer_MacAddress, SNMP_BUFFER_LEN);
  memcpy(snmp_buffer_MacAddress, agentSwitchMFDBMacAddress->octet_ptr, agentSwitchMFDBMacAddress->length);

  agentSwitchMFDBEntryData.agentSwitchMFDBProtocolType = agentSwitchMFDBProtocolType;
  SET_VALID(I_agentSwitchMFDBProtocolType, agentSwitchMFDBEntryData.valid);

  if ( (searchType == EXACT) ?
      (snmpAgentSwitchMFDBEntryGet(USMDB_UNIT_CURRENT,
                                   agentSwitchMFDBEntryData.agentSwitchMFDBVlanId,
                                   snmp_buffer_MacAddress,
                                   agentSwitchMFDBEntryData.agentSwitchMFDBProtocolType) != L7_SUCCESS) :
      ((snmpAgentSwitchMFDBEntryGet(USMDB_UNIT_CURRENT,
                                    agentSwitchMFDBEntryData.agentSwitchMFDBVlanId,
                                    snmp_buffer_MacAddress,
                                    agentSwitchMFDBEntryData.agentSwitchMFDBProtocolType) != L7_SUCCESS) &&
       snmpAgentSwitchMFDBEntryNextGet(USMDB_UNIT_CURRENT,
                                       &agentSwitchMFDBEntryData.agentSwitchMFDBVlanId,
                                       snmp_buffer_MacAddress,
                                       &agentSwitchMFDBEntryData.agentSwitchMFDBProtocolType) != L7_SUCCESS))
    return(NULL);

  /* copy the new MacAddress to the outgoing data structure, return failure if it fails */
  if (SafeMakeOctetString(&agentSwitchMFDBEntryData.agentSwitchMFDBMacAddress, snmp_buffer_MacAddress, L7_MAC_ADDR_LEN) != L7_TRUE)
    return(NULL);

  SET_VALID(I_agentSwitchMFDBMacAddress, agentSwitchMFDBEntryData.valid);

  switch (nominator)
  {
  case -1:
  case I_agentSwitchMFDBVlanId:
  case I_agentSwitchMFDBMacAddress:
  case I_agentSwitchMFDBProtocolType:
    break;

  case I_agentSwitchMFDBType:
    if (snmpAgentSwitchMFDBTypeGet(USMDB_UNIT_CURRENT,
                                   agentSwitchMFDBEntryData.agentSwitchMFDBVlanId,
                                   snmp_buffer_MacAddress,
                                   agentSwitchMFDBEntryData.agentSwitchMFDBProtocolType,
                                   &agentSwitchMFDBEntryData.agentSwitchMFDBType) == L7_SUCCESS)
      SET_VALID(I_agentSwitchMFDBType, agentSwitchMFDBEntryData.valid);
    break;

  case I_agentSwitchMFDBDescription:
    if ((snmpAgentSwitchMFDBDescriptionGet(USMDB_UNIT_CURRENT,
                                           agentSwitchMFDBEntryData.agentSwitchMFDBVlanId,
                                           snmp_buffer_MacAddress,
                                           agentSwitchMFDBEntryData.agentSwitchMFDBProtocolType,
                                           snmp_buffer) == L7_SUCCESS) &&
        (SafeMakeOctetStringFromTextExact(&agentSwitchMFDBEntryData.agentSwitchMFDBDescription, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentSwitchMFDBDescription, agentSwitchMFDBEntryData.valid);
    break;

  case I_agentSwitchMFDBForwardingPortMask:
    if ((snmpAgentSwitchMFDBForwardingPortMaskGet(USMDB_UNIT_CURRENT,
                                                  agentSwitchMFDBEntryData.agentSwitchMFDBVlanId,
                                                  snmp_buffer_MacAddress,
                                                  agentSwitchMFDBEntryData.agentSwitchMFDBProtocolType,
                                                  snmp_buffer,
                                                  &snmp_buffer_len) == L7_SUCCESS) &&
        (SafeMakeOctetString(&agentSwitchMFDBEntryData.agentSwitchMFDBForwardingPortMask, snmp_buffer, snmp_buffer_len) == L7_TRUE))
      SET_VALID(I_agentSwitchMFDBForwardingPortMask, agentSwitchMFDBEntryData.valid);
    break;

  case I_agentSwitchMFDBFilteringPortMask:
    if ((snmpAgentSwitchMFDBFilteringPortMaskGet(USMDB_UNIT_CURRENT,
                                                 agentSwitchMFDBEntryData.agentSwitchMFDBVlanId,
                                                 snmp_buffer_MacAddress,
                                                 agentSwitchMFDBEntryData.agentSwitchMFDBProtocolType,
                                                 snmp_buffer,
                                                 &snmp_buffer_len) == L7_SUCCESS) &&
        (SafeMakeOctetString(&agentSwitchMFDBEntryData.agentSwitchMFDBFilteringPortMask, snmp_buffer, snmp_buffer_len) == L7_TRUE))
      SET_VALID(I_agentSwitchMFDBFilteringPortMask, agentSwitchMFDBEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentSwitchMFDBEntryData.valid))
    return(NULL);

  return(&agentSwitchMFDBEntryData);
}

agentSwitchMFDBSummaryEntry_t *
k_agentSwitchMFDBSummaryEntry_get(int serialNum, ContextInfo *contextInfo,
                                  int nominator,
                                  int searchType,
                                  SR_UINT32 agentSwitchMFDBSummaryVlanId,
                                  OctetString * agentSwitchMFDBSummaryMacAddress)
{
  static agentSwitchMFDBSummaryEntry_t agentSwitchMFDBSummaryEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer_MacAddress[SNMP_BUFFER_LEN];
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 snmp_buffer_len;



  if (firstTime == L7_TRUE)
  {
   firstTime = L7_FALSE;

   agentSwitchMFDBSummaryEntryData.agentSwitchMFDBSummaryMacAddress = MakeOctetString(NULL, 0);
   agentSwitchMFDBSummaryEntryData.agentSwitchMFDBSummaryForwardingPortMask = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(agentSwitchMFDBSummaryEntryData.valid);

  agentSwitchMFDBSummaryEntryData.agentSwitchMFDBSummaryVlanId = agentSwitchMFDBSummaryVlanId;
  SET_VALID(I_agentSwitchMFDBSummaryVlanId, agentSwitchMFDBSummaryEntryData.valid);

  bzero(snmp_buffer_MacAddress, SNMP_BUFFER_LEN);
  memcpy(snmp_buffer_MacAddress, agentSwitchMFDBSummaryMacAddress->octet_ptr, agentSwitchMFDBSummaryMacAddress->length);

  if ( (searchType == EXACT) ?
      (snmpAgentSwitchMFDBSummaryEntryGet(USMDB_UNIT_CURRENT,
                                          agentSwitchMFDBSummaryEntryData.agentSwitchMFDBSummaryVlanId,
                                          snmp_buffer_MacAddress) != L7_SUCCESS) :
      ((snmpAgentSwitchMFDBSummaryEntryGet(USMDB_UNIT_CURRENT,
                                           agentSwitchMFDBSummaryEntryData.agentSwitchMFDBSummaryVlanId,
                                           snmp_buffer_MacAddress) != L7_SUCCESS) &&
       snmpAgentSwitchMFDBSummaryEntryNextGet(USMDB_UNIT_CURRENT,
                                              &agentSwitchMFDBSummaryEntryData.agentSwitchMFDBSummaryVlanId,
                                              snmp_buffer_MacAddress) != L7_SUCCESS))
    return(NULL);

  /* copy the new MacAddress to the outgoing data structure, return failure if it fails */
  if (SafeMakeOctetString(&agentSwitchMFDBSummaryEntryData.agentSwitchMFDBSummaryMacAddress, snmp_buffer_MacAddress, L7_MAC_ADDR_LEN) != L7_TRUE)
    return(NULL);

  SET_VALID(I_agentSwitchMFDBSummaryMacAddress, agentSwitchMFDBSummaryEntryData.valid);

  switch (nominator)
  {
  case -1:
  case I_agentSwitchMFDBSummaryVlanId:
  case I_agentSwitchMFDBSummaryMacAddress:
    break;

  case I_agentSwitchMFDBSummaryForwardingPortMask:
    if ((snmpAgentSwitchMFDBSummaryForwardingPortMaskGet(USMDB_UNIT_CURRENT,
                                                         agentSwitchMFDBSummaryEntryData.agentSwitchMFDBSummaryVlanId,
                                                         snmp_buffer_MacAddress,
                                                         snmp_buffer,
                                                         &snmp_buffer_len) == L7_SUCCESS) &&
        (SafeMakeOctetString(&agentSwitchMFDBSummaryEntryData.agentSwitchMFDBSummaryForwardingPortMask, snmp_buffer, snmp_buffer_len) == L7_TRUE))
      SET_VALID(I_agentSwitchMFDBSummaryForwardingPortMask, agentSwitchMFDBSummaryEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentSwitchMFDBSummaryEntryData.valid))
    return(NULL);

  return(&agentSwitchMFDBSummaryEntryData);
}


agentSwitchSnoopingIntfEntry_t *
k_agentSwitchSnoopingIntfEntry_get(int serialNum, ContextInfo *contextInfo,
                                       int nominator,
                                       int searchType,
                                       SR_INT32 ifIndex,
                                       SR_INT32 family)
{
  static L7_BOOL firstTime = L7_TRUE;
  static agentSwitchSnoopingIntfEntry_t agentSwitchSnoopingIntfEntryData;
  L7_uint32 intIfNum;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 snmp_buffer_len = SNMP_BUFFER_LEN;

  ZERO_VALID(agentSwitchSnoopingIntfEntryData.valid);
  bzero(snmp_buffer, sizeof(snmp_buffer));
  agentSwitchSnoopingIntfEntryData.ifIndex = ifIndex;
  agentSwitchSnoopingIntfEntryData.agentSwitchSnoopingProtocol = family;
  SET_VALID(I_agentSwitchSnoopingIntfEntryIndex_ifIndex, agentSwitchSnoopingIntfEntryData.valid);
  SET_VALID(I_agentSwitchSnoopingIntfEntryIndex_agentSwitchSnoopingProtocol,
            agentSwitchSnoopingIntfEntryData.valid);

  if (firstTime == L7_TRUE)
  {
    agentSwitchSnoopingIntfEntryData.agentSwitchSnoopingIntfVlanIDs = MakeOctetString(NULL, 0);
    firstTime = L7_FALSE;
  }

  /* Validating the row-entries for GET and GETNEXT operations */
  if (searchType == EXACT)
  {
    if(snmpSnoopIntfValidate(USMDB_UNIT_CURRENT, agentSwitchSnoopingIntfEntryData.ifIndex,
                             agentSwitchSnoopingIntfEntryData.agentSwitchSnoopingProtocol) != L7_SUCCESS)
    {
      ZERO_VALID(agentSwitchSnoopingIntfEntryData.valid);
      return(NULL);
    }

  }
  else if (searchType == NEXT)
  {
    if(snmpSnoopIntfValidate(USMDB_UNIT_CURRENT, agentSwitchSnoopingIntfEntryData.ifIndex,
                             agentSwitchSnoopingIntfEntryData.agentSwitchSnoopingProtocol) != L7_SUCCESS)
    {
      if(snmpSnoopNextIntfValidate(USMDB_UNIT_CURRENT, agentSwitchSnoopingIntfEntryData.ifIndex,
                                   &agentSwitchSnoopingIntfEntryData.ifIndex,
                                   agentSwitchSnoopingIntfEntryData.agentSwitchSnoopingProtocol,
                                   &agentSwitchSnoopingIntfEntryData.agentSwitchSnoopingProtocol) != L7_SUCCESS)
      {
        ZERO_VALID(agentSwitchSnoopingIntfEntryData.valid);
        return(NULL);
      }
    }
  }

  usmDbIntIfNumFromExtIfNum(agentSwitchSnoopingIntfEntryData.ifIndex, &intIfNum);

  switch (nominator)
  {
  case -1:
    if ( nominator != -1) break;
    /* else pass through */

  case I_agentSwitchSnoopingIntfIndex:
    agentSwitchSnoopingIntfEntryData.agentSwitchSnoopingIntfIndex = intIfNum;
    SET_VALID(I_agentSwitchSnoopingIntfIndex, agentSwitchSnoopingIntfEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentSwitchSnoopingIntfAdminMode:
    if (snmpAgentSwitchSnoopingIntfAdminModeGet(USMDB_UNIT_CURRENT, intIfNum,
                                             &agentSwitchSnoopingIntfEntryData.agentSwitchSnoopingIntfAdminMode,
                                              agentSwitchSnoopingIntfEntryData.agentSwitchSnoopingProtocol) == L7_SUCCESS)
      SET_VALID(I_agentSwitchSnoopingIntfAdminMode, agentSwitchSnoopingIntfEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentSwitchSnoopingIntfGroupMembershipInterval:
    if (usmDbSnoopIntfGroupMembershipIntervalGet(USMDB_UNIT_CURRENT, intIfNum,
                   &agentSwitchSnoopingIntfEntryData.agentSwitchSnoopingIntfGroupMembershipInterval,
                    agentSwitchSnoopingIntfEntryData.agentSwitchSnoopingProtocol) == L7_SUCCESS)
      SET_VALID(I_agentSwitchSnoopingIntfGroupMembershipInterval, agentSwitchSnoopingIntfEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentSwitchSnoopingIntfMaxResponseTime:
    if (usmDbSnoopIntfResponseTimeGet(USMDB_UNIT_CURRENT, intIfNum,
                                  &agentSwitchSnoopingIntfEntryData.agentSwitchSnoopingIntfMaxResponseTime,
                                  agentSwitchSnoopingIntfEntryData.agentSwitchSnoopingProtocol) == L7_SUCCESS)
      SET_VALID(I_agentSwitchSnoopingIntfMaxResponseTime, agentSwitchSnoopingIntfEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentSwitchSnoopingIntfMRPExpirationTime:
    if (usmDbSnoopIntfMcastRtrExpiryTimeGet(USMDB_UNIT_CURRENT, intIfNum,
                                        &agentSwitchSnoopingIntfEntryData.agentSwitchSnoopingIntfMRPExpirationTime,
                                         agentSwitchSnoopingIntfEntryData.agentSwitchSnoopingProtocol) == L7_SUCCESS)
      SET_VALID(I_agentSwitchSnoopingIntfMRPExpirationTime, agentSwitchSnoopingIntfEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentSwitchSnoopingIntfFastLeaveAdminMode:
    if (snmpAgentSwitchSnoopingIntfFastLeaveAdminModeGet(USMDB_UNIT_CURRENT, intIfNum,
                                             &agentSwitchSnoopingIntfEntryData.agentSwitchSnoopingIntfFastLeaveAdminMode,
                                              agentSwitchSnoopingIntfEntryData.agentSwitchSnoopingProtocol) == L7_SUCCESS)
      SET_VALID(I_agentSwitchSnoopingIntfFastLeaveAdminMode, agentSwitchSnoopingIntfEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentSwitchSnoopingIntfMulticastRouterMode:
    if (snmpAgentSwitchSnoopingIntfMulticastRouterModeGet(USMDB_UNIT_CURRENT, intIfNum,
                                             &agentSwitchSnoopingIntfEntryData.agentSwitchSnoopingIntfMulticastRouterMode,
                                              agentSwitchSnoopingIntfEntryData.agentSwitchSnoopingProtocol) == L7_SUCCESS)
      SET_VALID(I_agentSwitchSnoopingIntfMulticastRouterMode, agentSwitchSnoopingIntfEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentSwitchSnoopingIntfVlanIDs:
      bzero(snmp_buffer, SNMP_BUFFER_LEN);
      if (snmpAgentSwitchSnoopingIntfVlanIDsGet(USMDB_UNIT_CURRENT, intIfNum, snmp_buffer, &snmp_buffer_len) == L7_SUCCESS &&
          SafeMakeOctetString(&agentSwitchSnoopingIntfEntryData.agentSwitchSnoopingIntfVlanIDs, snmp_buffer, snmp_buffer_len) == L7_TRUE)
        SET_VALID(I_agentSwitchSnoopingIntfVlanIDs, agentSwitchSnoopingIntfEntryData.valid);
      break;

  default:
    /* unknown nominator value */
    return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, agentSwitchSnoopingIntfEntryData.valid))
    return(NULL);

  return (&agentSwitchSnoopingIntfEntryData);

}

#ifdef SETS
int
k_agentSwitchSnoopingIntfEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                        doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentSwitchSnoopingIntfEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                         doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentSwitchSnoopingIntfEntry_set_defaults(doList_t *dp)
{
    agentSwitchSnoopingIntfEntry_t *data = (agentSwitchSnoopingIntfEntry_t *) (dp->data);

    data->agentSwitchSnoopingIntfAdminMode = D_agentSwitchSnoopingIntfAdminMode_disable;
    data->agentSwitchSnoopingIntfGroupMembershipInterval = 260;
    data->agentSwitchSnoopingIntfMaxResponseTime = 10;
    data->agentSwitchSnoopingIntfMRPExpirationTime = 0;
    data->agentSwitchSnoopingIntfFastLeaveAdminMode = D_agentSwitchSnoopingIntfFastLeaveAdminMode_disable;
    data->agentSwitchSnoopingIntfMulticastRouterMode = D_agentSwitchSnoopingIntfMulticastRouterMode_disable;
    if ((data->agentSwitchSnoopingIntfVlanIDs = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;

    }

    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentSwitchSnoopingIntfEntry_set(agentSwitchSnoopingIntfEntry_t *data,
                                       ContextInfo *contextInfo, int function)
{
  /* L7_char8 snmp_buffer[SNMP_BUFFER_LEN];    */
  L7_uint32 intIfNum;
  L7_RC_t rc;
  L7_uchar8 family = L7_AF_INET;

  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  /* Conversion to internal interface number for snmp call */
 family = (L7_uchar8) data->agentSwitchSnoopingProtocol;
 rc = usmDbIntIfNumFromExtIfNum(data->ifIndex, &intIfNum);
 if (rc == L7_SUCCESS)
 {

  if (VALID(I_agentSwitchSnoopingIntfAdminMode, data->valid))
  {
    if( snmpAgentSwitchSnoopingIntfAdminModeSet(USMDB_UNIT_CURRENT, intIfNum,
                                              data->agentSwitchSnoopingIntfAdminMode,
                                              family) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentSwitchSnoopingIntfAdminMode, tempValid);
    }
  }

  if (VALID(I_agentSwitchSnoopingIntfGroupMembershipInterval, data->valid))
  {
    if( usmDbSnoopIntfGroupMembershipIntervalSet(USMDB_UNIT_CURRENT, intIfNum,
                             data->agentSwitchSnoopingIntfGroupMembershipInterval,
                              family) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentSwitchSnoopingIntfGroupMembershipInterval, tempValid);
    }
  }

  if (VALID(I_agentSwitchSnoopingIntfMaxResponseTime, data->valid))
  {
    if( usmDbSnoopIntfResponseTimeSet(USMDB_UNIT_CURRENT, intIfNum,
                                data->agentSwitchSnoopingIntfMaxResponseTime,
                                family) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentSwitchSnoopingIntfMaxResponseTime, tempValid);
    }
  }

  if (VALID(I_agentSwitchSnoopingIntfMRPExpirationTime, data->valid))
  {
    if( usmDbSnoopIntfMcastRtrExpiryTimeSet(USMDB_UNIT_CURRENT, intIfNum,
                                      data->agentSwitchSnoopingIntfMRPExpirationTime,
                                      family) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentSwitchSnoopingIntfMRPExpirationTime, tempValid);
    }
  }

  if (VALID(I_agentSwitchSnoopingIntfFastLeaveAdminMode, data->valid))
  {
    if( snmpAgentSwitchSnoopingIntfFastLeaveAdminModeSet(USMDB_UNIT_CURRENT, intIfNum,
                                              data->agentSwitchSnoopingIntfFastLeaveAdminMode,
                                              family) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentSwitchSnoopingIntfFastLeaveAdminMode, tempValid);
    }
  }

  if (VALID(I_agentSwitchSnoopingIntfMulticastRouterMode, data->valid))
  {
    if( snmpAgentSwitchSnoopingIntfMulticastRouterModeSet(USMDB_UNIT_CURRENT, intIfNum,
                                              data->agentSwitchSnoopingIntfMulticastRouterMode,
                                              family) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentSwitchSnoopingIntfMulticastRouterMode, tempValid);
    }
  }

 }
  return NO_ERROR;

}

#ifdef SR_agentSwitchSnoopingIntfEntry_UNDO
/* add #define SR_agentSwitchSnoopingIntfEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentSwitchSnoopingIntfEntry family.
 */
int
agentSwitchSnoopingIntfEntry_undo(doList_t *doHead, doList_t *doCur,
                                      ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentSwitchSnoopingIntfEntry_UNDO */

#endif /* SETS */

agentSwitchSnoopingVlanEntry_t *
k_agentSwitchSnoopingVlanEntry_get(int serialNum, ContextInfo *contextInfo,
                                   int nominator,
                                   int searchType,
                                   SR_UINT32 dot1qVlanIndex,
                                   SR_INT32 family)
{
  static agentSwitchSnoopingVlanEntry_t agentSwitchSnoopingVlanEntryData;
  char snmp_buffer[SNMP_BUFFER_LEN];
  L7_ushort16 groupMembershipInterval;
  L7_ushort16 responseTime;
  L7_ushort16 mcrtExpiryTime;

  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_DOT1Q_COMPONENT_ID ) == L7_FALSE )
    return(NULL);


  ZERO_VALID(agentSwitchSnoopingVlanEntryData.valid);
  bzero(snmp_buffer, sizeof(snmp_buffer));
  agentSwitchSnoopingVlanEntryData.dot1qVlanIndex = dot1qVlanIndex;
  agentSwitchSnoopingVlanEntryData.agentSwitchSnoopingProtocol = family;
  SET_VALID(I_agentSwitchSnoopingVlanEntryIndex_dot1qVlanIndex, agentSwitchSnoopingVlanEntryData.valid);
  SET_VALID(I_agentSwitchSnoopingVlanEntryIndex_agentSwitchSnoopingProtocol, agentSwitchSnoopingVlanEntryData.valid);

  /* Validating the row-entries for GET and GETNEXT operations */
  if (searchType == EXACT)
  {
    if(snmpSnoopVlanGet(USMDB_UNIT_CURRENT, agentSwitchSnoopingVlanEntryData.dot1qVlanIndex,
                        agentSwitchSnoopingVlanEntryData.agentSwitchSnoopingProtocol)
       != L7_SUCCESS)
    {
      ZERO_VALID(agentSwitchSnoopingVlanEntryData.valid);
      return(NULL);
    }
  }
  else if (searchType == NEXT)
  {
    if(snmpSnoopVlanGet(USMDB_UNIT_CURRENT, agentSwitchSnoopingVlanEntryData.dot1qVlanIndex,
                        agentSwitchSnoopingVlanEntryData.agentSwitchSnoopingProtocol) != L7_SUCCESS)
    {
      if(snmpSnoopVlanGetNext(USMDB_UNIT_CURRENT, &agentSwitchSnoopingVlanEntryData.dot1qVlanIndex,
                              agentSwitchSnoopingVlanEntryData.agentSwitchSnoopingProtocol,
                              &agentSwitchSnoopingVlanEntryData.agentSwitchSnoopingProtocol)
         != L7_SUCCESS)
      {
        ZERO_VALID(agentSwitchSnoopingVlanEntryData.valid);
        return(NULL);
      }
    }
  }

  switch (nominator)
  {
    case -1:
      if ( nominator != -1) break;
      /* else pass through */

    case I_agentSwitchSnoopingVlanAdminMode:
      if (usmDbSnoopVlanModeGet(USMDB_UNIT_CURRENT, agentSwitchSnoopingVlanEntryData.dot1qVlanIndex,                 &agentSwitchSnoopingVlanEntryData.agentSwitchSnoopingVlanAdminMode,
                                agentSwitchSnoopingVlanEntryData.agentSwitchSnoopingProtocol) == L7_SUCCESS)
      {
        SET_VALID(I_agentSwitchSnoopingVlanAdminMode, agentSwitchSnoopingVlanEntryData.valid);
      }
      if (nominator != -1) break;
      /* else pass through */

    case I_agentSwitchSnoopingVlanGroupMembershipInterval:
      if (usmDbSnoopVlanGroupMembershipIntervalGet(USMDB_UNIT_CURRENT, agentSwitchSnoopingVlanEntryData.dot1qVlanIndex, &groupMembershipInterval,
                                                   agentSwitchSnoopingVlanEntryData.agentSwitchSnoopingProtocol) == L7_SUCCESS)
      {
        SET_VALID(I_agentSwitchSnoopingVlanGroupMembershipInterval, agentSwitchSnoopingVlanEntryData.valid);
        agentSwitchSnoopingVlanEntryData.agentSwitchSnoopingVlanGroupMembershipInterval = groupMembershipInterval;
      }
      if (nominator != -1) break;
      /* else pass through */

    case I_agentSwitchSnoopingVlanMaxResponseTime:
      if (usmDbSnoopVlanMaximumResponseTimeGet(USMDB_UNIT_CURRENT, agentSwitchSnoopingVlanEntryData.dot1qVlanIndex, &responseTime,
                                               agentSwitchSnoopingVlanEntryData.agentSwitchSnoopingProtocol) == L7_SUCCESS)
      {
        SET_VALID(I_agentSwitchSnoopingVlanMaxResponseTime, agentSwitchSnoopingVlanEntryData.valid);
        agentSwitchSnoopingVlanEntryData.agentSwitchSnoopingVlanMaxResponseTime = responseTime;
      }
      if (nominator != -1) break;
      /* else pass through */
    case I_agentSwitchSnoopingVlanFastLeaveAdminMode:
      if (usmDbSnoopVlanFastLeaveModeGet(USMDB_UNIT_CURRENT, agentSwitchSnoopingVlanEntryData.dot1qVlanIndex, (L7_uint32 *)&agentSwitchSnoopingVlanEntryData.agentSwitchSnoopingVlanFastLeaveAdminMode,
                                         agentSwitchSnoopingVlanEntryData.agentSwitchSnoopingProtocol) == L7_SUCCESS)
      {
        SET_VALID(I_agentSwitchSnoopingVlanFastLeaveAdminMode, agentSwitchSnoopingVlanEntryData.valid);
      }
      if (nominator != -1) break;
      /* else pass through */

    case I_agentSwitchSnoopingVlanMRPExpirationTime:
      if(snmpSnoopVlanMcastRtrExpiryTimeGet(USMDB_UNIT_CURRENT,
                                            agentSwitchSnoopingVlanEntryData.dot1qVlanIndex, &mcrtExpiryTime,
                                            agentSwitchSnoopingVlanEntryData.agentSwitchSnoopingProtocol) == L7_SUCCESS)
      {
        SET_VALID(I_agentSwitchSnoopingVlanMRPExpirationTime, agentSwitchSnoopingVlanEntryData.valid);
        agentSwitchSnoopingVlanEntryData.agentSwitchSnoopingVlanMRPExpirationTime = mcrtExpiryTime;
      }
      break;

    default:
      /* unknown nominator value */
      return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, agentSwitchSnoopingVlanEntryData.valid))
    return(NULL);

  return (&agentSwitchSnoopingVlanEntryData);
}

#ifdef SETS
int
k_agentSwitchSnoopingVlanEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                        doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentSwitchSnoopingVlanEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                         doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentSwitchSnoopingVlanEntry_set_defaults(doList_t *dp)
{
    agentSwitchSnoopingVlanEntry_t *data = (agentSwitchSnoopingVlanEntry_t *) (dp->data);

    data->agentSwitchSnoopingVlanAdminMode = D_agentSwitchSnoopingVlanAdminMode_disable;
    data->agentSwitchSnoopingVlanGroupMembershipInterval = 260;
    data->agentSwitchSnoopingVlanMaxResponseTime = 10;
    data->agentSwitchSnoopingVlanFastLeaveAdminMode = D_agentSwitchSnoopingVlanFastLeaveAdminMode_disable;
    data->agentSwitchSnoopingVlanMRPExpirationTime = 0;

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentSwitchSnoopingVlanEntry_set(agentSwitchSnoopingVlanEntry_t *data,
                                       ContextInfo *contextInfo, int function)
{

  L7_ushort16 groupMembershipInterval;
  L7_ushort16 responseTime;
  L7_ushort16 mcrtExpiryTime;
  L7_uchar8 family = L7_AF_INET;
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));
#if 0
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_DOT1Q_COMPONENT_ID ) == L7_FALSE )
      return(COMMIT_FAILED_ERROR);
#endif
  family = (L7_uchar8)data->agentSwitchSnoopingProtocol;
  if (VALID(I_agentSwitchSnoopingVlanAdminMode, data->valid))
  {
    if( usmDbSnoopVlanModeSet(USMDB_UNIT_CURRENT, data->dot1qVlanIndex,
                                              data->agentSwitchSnoopingVlanAdminMode, family) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentSwitchSnoopingVlanAdminMode, tempValid);
    }
  }

  if (VALID(I_agentSwitchSnoopingVlanGroupMembershipInterval, data->valid))
  {
    groupMembershipInterval = data->agentSwitchSnoopingVlanGroupMembershipInterval;
    if( usmDbSnoopVlanGroupMembershipIntervalSet(USMDB_UNIT_CURRENT, data->dot1qVlanIndex,
                                              groupMembershipInterval, family) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentSwitchSnoopingVlanGroupMembershipInterval, tempValid);
    }
  }

  if (VALID(I_agentSwitchSnoopingVlanMaxResponseTime, data->valid))
  {
    responseTime = data->agentSwitchSnoopingVlanMaxResponseTime;
    if( usmDbSnoopVlanMaximumResponseTimeSet(USMDB_UNIT_CURRENT, data->dot1qVlanIndex,
                                              responseTime, family) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentSwitchSnoopingVlanMaxResponseTime, tempValid);
    }
  }

  if (VALID(I_agentSwitchSnoopingVlanFastLeaveAdminMode, data->valid))
  {
    if( usmDbSnoopVlanFastLeaveModeSet(USMDB_UNIT_CURRENT, data->dot1qVlanIndex,
                                             data->agentSwitchSnoopingVlanFastLeaveAdminMode,
                                       family) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentSwitchSnoopingVlanFastLeaveAdminMode, tempValid);
    }
  }

  if (VALID(I_agentSwitchSnoopingVlanMRPExpirationTime, data->valid))
  {
    mcrtExpiryTime = data->agentSwitchSnoopingVlanMRPExpirationTime;
    if (snmpSnoopVlanMcastRtrExpiryTimeSet(USMDB_UNIT_CURRENT, data->dot1qVlanIndex,
                                           mcrtExpiryTime, family)
        != L7_SUCCESS)
    {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
    }
    else
    {
        SET_VALID(I_agentSwitchSnoopingVlanMRPExpirationTime, tempValid);
    }
  }

  return NO_ERROR;
}

#ifdef SR_agentSwitchSnoopingVlanEntry_UNDO
/* add #define SR_agentSwitchSnoopingVlanEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentSwitchSnoopingVlanEntry family.
 */
int
agentSwitchSnoopingVlanEntry_undo(doList_t *doHead, doList_t *doCur,
                                      ContextInfo *contextInfo)
{
  agentSwitchSnoopingVlanEntry_t *data = (agentSwitchSnoopingVlanEntry_t *) doCur->data;
  agentSwitchSnoopingVlanEntry_t *undodata = (agentSwitchSnoopingVlanEntry_t *) doCur->undodata;
  agentSwitchSnoopingVlanEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

 /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data->valid == NULL || undodata == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;


  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentSwitchSnoopingVlanEntry_set(setdata, contextInfo, function) == NO_ERROR))
      return NO_ERROR;
  return UNDO_FAILED_ERROR;
}
#endif /* SR_agentSwitchSnoopingVlanEntry_UNDO */

#endif /* SETS */


agentSwitchVlanStaticMrouterEntry_t *
k_agentSwitchVlanStaticMrouterEntry_get(int serialNum, ContextInfo *contextInfo,
                                            int nominator,
                                            int searchType,
                                            SR_INT32 ifIndex,
                                            SR_UINT32 dot1qVlanIndex,
                                            SR_INT32 family)
{
   static agentSwitchVlanStaticMrouterEntry_t agentSwitchVlanStaticMrouterEntryData;
   L7_uchar8 tmpFamily;
   L7_uint32 intIfIndex;
   ZERO_VALID(agentSwitchVlanStaticMrouterEntryData.valid);
   agentSwitchVlanStaticMrouterEntryData.ifIndex = ifIndex;
   agentSwitchVlanStaticMrouterEntryData.dot1qVlanIndex = dot1qVlanIndex;
   agentSwitchVlanStaticMrouterEntryData.agentSwitchSnoopingProtocol = family;
   SET_VALID(I_agentSwitchVlanStaticMrouterEntryIndex_ifIndex, agentSwitchVlanStaticMrouterEntryData.valid);
   SET_VALID(I_agentSwitchVlanStaticMrouterEntryIndex_dot1qVlanIndex, agentSwitchVlanStaticMrouterEntryData.valid);
   SET_VALID(I_agentSwitchVlanStaticMrouterEntryIndex_agentSwitchSnoopingProtocol, agentSwitchVlanStaticMrouterEntryData.valid);
   tmpFamily = (L7_uchar8) family;

  /* Validating the row-entries for GET and GETNEXT operations */
  if (searchType == EXACT)
  {
    if (snmpIntfVlanValidate(USMDB_UNIT_CURRENT,agentSwitchVlanStaticMrouterEntryData.ifIndex,agentSwitchVlanStaticMrouterEntryData.dot1qVlanIndex,
                             tmpFamily) != L7_SUCCESS )
    {
      ZERO_VALID(agentSwitchVlanStaticMrouterEntryData.valid);
      return(NULL);
    }

    if (usmDbSnoopProtocolGet(tmpFamily) == L7_FALSE)
    {
      ZERO_VALID(agentSwitchVlanStaticMrouterEntryData.valid);
      return(NULL);
    }
  }
  else if (searchType == NEXT)
  {

    if (snmpIntfVlanValidate(USMDB_UNIT_CURRENT,agentSwitchVlanStaticMrouterEntryData.ifIndex,agentSwitchVlanStaticMrouterEntryData.dot1qVlanIndex,
                             tmpFamily) != L7_SUCCESS)
    {
      if (snmpIntfVlanNextGet(USMDB_UNIT_CURRENT,
                              &agentSwitchVlanStaticMrouterEntryData.ifIndex, &agentSwitchVlanStaticMrouterEntryData.dot1qVlanIndex,
                              &tmpFamily) != L7_SUCCESS )
      {
        ZERO_VALID(agentSwitchVlanStaticMrouterEntryData.valid);
        return(NULL);
      }
    }
  }
agentSwitchVlanStaticMrouterEntryData.agentSwitchSnoopingProtocol = tmpFamily;
if( usmDbIntIfNumFromExtIfNum(agentSwitchVlanStaticMrouterEntryData.ifIndex, &intIfIndex) != L7_SUCCESS )
{
  ZERO_VALID(agentSwitchVlanStaticMrouterEntryData.valid);
  return(NULL);
}

  switch (nominator)
    {
    case -1:
    case I_agentSwitchVlanStaticMrouterAdminMode:
      if (snmpAgentSwitchVlanStaticMrouterAdminModeGet(USMDB_UNIT_CURRENT, intIfIndex, agentSwitchVlanStaticMrouterEntryData.dot1qVlanIndex,
                                                       &agentSwitchVlanStaticMrouterEntryData.agentSwitchVlanStaticMrouterAdminMode,
                                                       tmpFamily) == L7_SUCCESS)
      {
        SET_VALID(I_agentSwitchVlanStaticMrouterAdminMode, agentSwitchVlanStaticMrouterEntryData.valid);
      }
      break;

    default:
      break;
    }

  if (nominator >= 0 && !VALID(nominator, agentSwitchVlanStaticMrouterEntryData.valid))
    return(NULL);

   return(&agentSwitchVlanStaticMrouterEntryData);
}

#ifdef SETS
int
k_agentSwitchVlanStaticMrouterEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                             doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}
int
k_agentSwitchVlanStaticMrouterEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                              doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentSwitchVlanStaticMrouterEntry_set_defaults(doList_t *dp)
{
    agentSwitchVlanStaticMrouterEntry_t *data = (agentSwitchVlanStaticMrouterEntry_t *) (dp->data);

    data->agentSwitchVlanStaticMrouterAdminMode = D_agentSwitchVlanStaticMrouterAdminMode_disable;

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentSwitchVlanStaticMrouterEntry_set(agentSwitchVlanStaticMrouterEntry_t *data,
                                            ContextInfo *contextInfo, int function)
{
  L7_uint32 intIfIndex;

  if ( usmDbPhysicalExtIfNumberCheck(USMDB_UNIT_CURRENT, data->ifIndex) != L7_SUCCESS ||
        usmDbIntIfNumFromExtIfNum(data->ifIndex, &intIfIndex) != L7_SUCCESS )
    {
      ZERO_VALID(data->valid);
      return(COMMIT_FAILED_ERROR);
    }

  if (VALID(I_agentSwitchVlanStaticMrouterAdminMode, data->valid))
  {
    if(snmpAgentSwitchVlanStaticMrouterAdminModeSet(USMDB_UNIT_CURRENT, intIfIndex, data->dot1qVlanIndex,
                       data->agentSwitchVlanStaticMrouterAdminMode) != L7_SUCCESS)
      {
        CLR_VALID(I_agentSwitchVlanStaticMrouterAdminMode, data->valid);
        return COMMIT_FAILED_ERROR;
      }
  }

  return NO_ERROR ;
}

#ifdef SR_agentSwitchVlanStaticMrouterEntry_UNDO
/* add #define SR_agentSwitchVlanStaticMrouterEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentSwitchVlanStaticMrouterEntry family.
 */
int
agentSwitchVlanStaticMrouterEntry_undo(doList_t *doHead, doList_t *doCur,
                                           ContextInfo *contextInfo)
{
  agentSwitchVlanStaticMrouterEntry_t *data = (agentSwitchVlanStaticMrouterEntry_t *) doCur->data;
  agentSwitchVlanStaticMrouterEntry_t *undodata =
    (agentSwitchVlanStaticMrouterEntry_t *) doCur->undodata;
  agentSwitchVlanStaticMrouterEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

 /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data->valid == NULL || undodata == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;


  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentSwitchVlanStaticMrouterEntry_set(setdata, contextInfo, function)
      == NO_ERROR))
      return NO_ERROR;
  return UNDO_FAILED_ERROR;
}
#endif /* SR_agentSwitchVlanStaticMrouterEntry_UNDO */

#endif /* SETS */

/****************************************
   Snooping Querier
 *****************************************/
agentSwitchSnoopingQuerierCfgEntry_t *
k_agentSwitchSnoopingQuerierCfgEntry_get(int serialNum, ContextInfo *contextInfo,
                                         int nominator,
                                         int searchType,
                                         SR_INT32 agentSwitchSnoopingProtocol)
{

   static agentSwitchSnoopingQuerierCfgEntry_t agentSwitchSnoopingQuerierCfgEntryData;
   static L7_BOOL firstTime = L7_TRUE;
   L7_uchar8 tmpFamily;
   L7_inet_addr_t address;
   tmpFamily = (L7_uchar8)agentSwitchSnoopingProtocol;
   ZERO_VALID(agentSwitchSnoopingQuerierCfgEntryData.valid);
   inetAddressZeroSet(tmpFamily,&address); 
  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    agentSwitchSnoopingQuerierCfgEntryData.agentSwitchSnoopingQuerierAddress
                                                    = MakeOctetString(NULL, 0);
  }

  /* Validating the row-entries for GET and GETNEXT operations */
  if (searchType == EXACT)
  {
    if(usmDbSnoopProtocolGet(tmpFamily)
                             != L7_TRUE)
    {
      ZERO_VALID(agentSwitchSnoopingQuerierCfgEntryData.valid);
      return(NULL);
    }
  }
  else if (searchType == NEXT)
  {
    if(usmDbSnoopProtocolGet(tmpFamily) != L7_TRUE)
    {
      if(usmDbSnoopProtocolNextGet(tmpFamily, &tmpFamily)
                                  != L7_TRUE)
      {
        ZERO_VALID(agentSwitchSnoopingQuerierCfgEntryData.valid);
        return(NULL);
      }
    }
  }

   agentSwitchSnoopingQuerierCfgEntryData.agentSwitchSnoopingProtocol = tmpFamily;
   SET_VALID(I_agentSwitchSnoopingQuerierCfgEntryIndex_agentSwitchSnoopingProtocol, \
             agentSwitchSnoopingQuerierCfgEntryData.valid);
  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */
  switch (nominator)
  {
      case -1:
        if ( nominator != -1) break;
        /* else pass through */
          /* pass through */
      case I_agentSwitchSnoopingQuerierAdminMode:
      if (snmpAgentSwitchSnoopingQuerierAdminModeGet(&agentSwitchSnoopingQuerierCfgEntryData.agentSwitchSnoopingQuerierAdminMode,
                                                    tmpFamily) == L7_SUCCESS)
      {
        SET_VALID(I_agentSwitchSnoopingQuerierAdminMode, agentSwitchSnoopingQuerierCfgEntryData.valid);

      }
       if (nominator != -1) break;
      /* else pass through */

  case I_agentSwitchSnoopingQuerierVersion:
      if (usmDbSnoopQuerierVersionGet(&agentSwitchSnoopingQuerierCfgEntryData.agentSwitchSnoopingQuerierVersion,
                                       tmpFamily) == L7_SUCCESS)
        SET_VALID(I_agentSwitchSnoopingQuerierVersion, agentSwitchSnoopingQuerierCfgEntryData.valid);
       if (nominator != -1) break;
      /* else pass through */
  
  case I_agentSwitchSnoopingQuerierAddress:

        if(tmpFamily==L7_AF_INET)
          {
            if(usmDbSnoopQuerierAddressGet(&address.addr.ipv4.s_addr, tmpFamily) == L7_SUCCESS)
            {
             
             if(SafeMakeOctetString(&agentSwitchSnoopingQuerierCfgEntryData.agentSwitchSnoopingQuerierAddress,(L7_uchar8*)
                          &(address.addr.ipv4.s_addr), sizeof(address.addr.ipv4.s_addr))==L7_TRUE)
             {
              SET_VALID(I_agentSwitchSnoopingQuerierAddress, agentSwitchSnoopingQuerierCfgEntryData.valid);
             
            }  
          }
        }   
        else if(tmpFamily==L7_AF_INET6)
         {
            if(usmDbSnoopQuerierAddressGet(&address.addr.ipv6, tmpFamily) == L7_SUCCESS)
            {

             if( SafeMakeOctetString(&agentSwitchSnoopingQuerierCfgEntryData.agentSwitchSnoopingQuerierAddress,address.addr.ipv6.in6.addr8,
                                                 sizeof(address.addr.ipv6.in6.addr8))==L7_TRUE)
                  SET_VALID(I_agentSwitchSnoopingQuerierAddress, agentSwitchSnoopingQuerierCfgEntryData.valid);
            }
         }
     
       if (nominator != -1) break;
      /* else pass through */

  case I_agentSwitchSnoopingQuerierQueryInterval:
      if (usmDbSnoopQuerierQueryIntervalGet(&agentSwitchSnoopingQuerierCfgEntryData.agentSwitchSnoopingQuerierQueryInterval,
                                             tmpFamily) == L7_SUCCESS)
        SET_VALID(I_agentSwitchSnoopingQuerierQueryInterval, agentSwitchSnoopingQuerierCfgEntryData.valid);
       if (nominator != -1) break;
      /* else pass through */

  case I_agentSwitchSnoopingQuerierExpiryInterval:
      if (usmDbSnoopQuerierExpiryIntervalGet(&agentSwitchSnoopingQuerierCfgEntryData.agentSwitchSnoopingQuerierExpiryInterval,
                                             tmpFamily) == L7_SUCCESS)
        SET_VALID(I_agentSwitchSnoopingQuerierExpiryInterval, agentSwitchSnoopingQuerierCfgEntryData.valid);
        break;
      /* else pass through */

  default:
    /* unknown nominator value */
    return(NULL);
  }

  if ( nominator >= 0 &&
      !VALID(nominator, agentSwitchSnoopingQuerierCfgEntryData.valid))
  {
    return(NULL);
  }

  return(&agentSwitchSnoopingQuerierCfgEntryData);
}

#ifdef SETS
int
k_agentSwitchSnoopingQuerierCfgEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                          doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentSwitchSnoopingQuerierCfgEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                           doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentSwitchSnoopingQuerierCfgEntry_set_defaults(doList_t *dp)
{
    agentSwitchSnoopingQuerierCfgEntry_t *data = (agentSwitchSnoopingQuerierCfgEntry_t *) (dp->data);

    data->agentSwitchSnoopingQuerierAdminMode = D_agentSwitchSnoopingQuerierAdminMode_disable;
    if ((data->agentSwitchSnoopingQuerierAddress = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->agentSwitchSnoopingQuerierQueryInterval = 60;
    data->agentSwitchSnoopingQuerierExpiryInterval = 60;

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentSwitchSnoopingQuerierCfgEntry_set(agentSwitchSnoopingQuerierCfgEntry_t *data,
                                         ContextInfo *contextInfo, int function)
{
 
  L7_uchar8 family = L7_AF_INET;
  L7_char8 tempValid[sizeof(data->valid)];
  L7_uchar8 ipAddr[SNMP_BUFFER_LEN],address[SNMP_BUFFER_LEN];

  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  bzero(tempValid, sizeof(tempValid));
  bzero(ipAddr,SNMP_BUFFER_LEN);  
  
  family = (L7_uchar8)data->agentSwitchSnoopingProtocol;
  /* Querier Admin Mode */
  if (VALID(I_agentSwitchSnoopingQuerierAdminMode, data->valid))
  {
    if( snmpAgentSwitchSnoopingQuerierAdminModeSet(data->agentSwitchSnoopingQuerierAdminMode,
                                                   family) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentSwitchSnoopingQuerierAdminMode, tempValid);
    }
  }

  /* Querier Version */
  if (VALID(I_agentSwitchSnoopingQuerierVersion, data->valid))
  {
    if(usmDbSnoopQuerierVersionSet(data->agentSwitchSnoopingQuerierVersion,
                                   family) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentSwitchSnoopingQuerierVersion, tempValid);
    }
  }
  /* Querier Query Interval */
  if (VALID(I_agentSwitchSnoopingQuerierQueryInterval, data->valid))
  {
    if(usmDbSnoopQuerierQueryIntervalSet(data->agentSwitchSnoopingQuerierQueryInterval,
                                         family) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentSwitchSnoopingQuerierQueryInterval, tempValid);
    }
  }
  /* Querier Expiry Interval */
  if (VALID(I_agentSwitchSnoopingQuerierExpiryInterval, data->valid))
  {
    if(usmDbSnoopQuerierExpiryIntervalSet(data->agentSwitchSnoopingQuerierExpiryInterval,
                                         family) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentSwitchSnoopingQuerierExpiryInterval, tempValid);
    }
  }

  /* Querier Address */
  if (VALID(I_agentSwitchSnoopingQuerierAddress, data->valid))
  {  
     memcpy(address,data->agentSwitchSnoopingQuerierAddress->octet_ptr,data->agentSwitchSnoopingQuerierAddress->length);
     
     if (osapiInetPton(family, address, ipAddr) == L7_SUCCESS)
     {
      if (usmDbSnoopQuerierAddressSet(ipAddr, family) == L7_SUCCESS)
      {
       SET_VALID(I_agentSwitchSnoopingQuerierAddress, tempValid);
       return NO_ERROR;
      }
    }

    memcpy(data->valid, tempValid, sizeof(data->valid));
    return COMMIT_FAILED_ERROR;

    
 }

  return NO_ERROR;
}

#ifdef SR_agentSwitchSnoopingQuerierCfgEntry_UNDO
/* add #define SR_agentSwitchSnoopingQuerierCfgEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentSwitchSnoopingQuerierCfgEntry family.
 */
int
agentSwitchSnoopingQuerierCfgEntry_undo(doList_t *doHead, doList_t *doCur,
                                        ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentSwitchSnoopingQuerierCfgEntry_UNDO */

#endif /* SETS */
agentSwitchSnoopingQuerierVlanEntry_t *
k_agentSwitchSnoopingQuerierVlanEntry_get(int serialNum, ContextInfo *contextInfo,
                                          int nominator,
                                          int searchType,
                                          SR_UINT32 dot1qVlanIndex,
                                          SR_INT32 agentSwitchSnoopingProtocol)
{
   static agentSwitchSnoopingQuerierVlanEntry_t agentSwitchSnoopingQuerierVlanEntryData;
   static L7_BOOL firstTime = L7_TRUE;
   L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN], family;
   L7_uint32 vlanId;

   if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_DOT1Q_COMPONENT_ID ) == L7_FALSE )
      return(NULL);

   if (firstTime == L7_TRUE)
   {
     firstTime = L7_FALSE;
     agentSwitchSnoopingQuerierVlanEntryData.agentSwitchSnoopingQuerierVlanAddress
                                                     = MakeOctetString(NULL, 0);
     agentSwitchSnoopingQuerierVlanEntryData.agentSwitchSnoopingQuerierLastQuerierAddress
                                                     = MakeOctetString(NULL, 0);
   }

   ZERO_VALID(agentSwitchSnoopingQuerierVlanEntryData.valid);
   bzero(snmp_buffer, sizeof(snmp_buffer));
   agentSwitchSnoopingQuerierVlanEntryData.dot1qVlanIndex = dot1qVlanIndex;
   agentSwitchSnoopingQuerierVlanEntryData.agentSwitchSnoopingProtocol = agentSwitchSnoopingProtocol;

   SET_VALID(I_agentSwitchSnoopingQuerierVlanEntryIndex_dot1qVlanIndex,\
              agentSwitchSnoopingQuerierVlanEntryData.valid);

   SET_VALID(I_agentSwitchSnoopingQuerierVlanEntryIndex_agentSwitchSnoopingProtocol,\
             agentSwitchSnoopingQuerierVlanEntryData.valid);

   /* Validating the row-entries for GET and GETNEXT operations */
   if (searchType == EXACT)
   {
     if(snmpSnoopVlanGet(USMDB_UNIT_CURRENT, agentSwitchSnoopingQuerierVlanEntryData.dot1qVlanIndex,
                         agentSwitchSnoopingQuerierVlanEntryData.agentSwitchSnoopingProtocol)
                         != L7_SUCCESS)
     {
       ZERO_VALID(agentSwitchSnoopingQuerierVlanEntryData.valid);
       return(NULL);
     }
   }
   else if (searchType == NEXT)
   {
      if(snmpSnoopVlanGet(USMDB_UNIT_CURRENT, agentSwitchSnoopingQuerierVlanEntryData.dot1qVlanIndex,
                          agentSwitchSnoopingQuerierVlanEntryData.agentSwitchSnoopingProtocol) != L7_SUCCESS)
      {
        if(snmpSnoopVlanGetNext(USMDB_UNIT_CURRENT, &agentSwitchSnoopingQuerierVlanEntryData.dot1qVlanIndex,
                                agentSwitchSnoopingQuerierVlanEntryData.agentSwitchSnoopingProtocol,
                                &agentSwitchSnoopingQuerierVlanEntryData.agentSwitchSnoopingProtocol)
                                != L7_SUCCESS)
        {
          ZERO_VALID(agentSwitchSnoopingQuerierVlanEntryData.valid);
          return(NULL);
        }
      }
   }

  vlanId = agentSwitchSnoopingQuerierVlanEntryData.dot1qVlanIndex;
  family = agentSwitchSnoopingQuerierVlanEntryData.agentSwitchSnoopingProtocol;

  switch (nominator)
  {
    case -1:
      if ( nominator != -1) break;
        /* else pass through */

    case I_agentSwitchSnoopingQuerierVlanAdminMode:
      if (snmpAgentSwitchSnoopingQuerierVlanModeGet(vlanId,
          &agentSwitchSnoopingQuerierVlanEntryData.agentSwitchSnoopingQuerierVlanAdminMode,
          family) == L7_SUCCESS)
      {
         SET_VALID(I_agentSwitchSnoopingQuerierVlanAdminMode, agentSwitchSnoopingQuerierVlanEntryData.valid);
      }
      if (nominator != -1) break;
      /* else pass through */
    case I_agentSwitchSnoopingQuerierVlanOperMode:
      if (snmpAgentSwitchSnoopingQuerierVlanOperModeGet(vlanId,
           &agentSwitchSnoopingQuerierVlanEntryData.agentSwitchSnoopingQuerierVlanOperMode,
                                                    family) == L7_SUCCESS)
      {
         SET_VALID(I_agentSwitchSnoopingQuerierVlanOperMode, agentSwitchSnoopingQuerierVlanEntryData.valid);
      }
      if (nominator != -1) break;
      /* else pass through */

    case I_agentSwitchSnoopingQuerierElectionParticipateMode:
      if (snmpAgentSwitchSnoopingQuerierVlanElectionModeGet(vlanId,
           &agentSwitchSnoopingQuerierVlanEntryData.agentSwitchSnoopingQuerierElectionParticipateMode,
           family) == L7_SUCCESS)
      {
         SET_VALID(I_agentSwitchSnoopingQuerierElectionParticipateMode, agentSwitchSnoopingQuerierVlanEntryData.valid);
      }
      if (nominator != -1) break;
    /* else pass through */

    case I_agentSwitchSnoopingQuerierVlanAddress:
      if (usmDbSnoopQuerierVlanAddressGet(vlanId, snmp_buffer, family) == L7_SUCCESS
          && SafeMakeOctetString(&agentSwitchSnoopingQuerierVlanEntryData.agentSwitchSnoopingQuerierVlanAddress,
                                 snmp_buffer, (family == L7_AF_INET)? L7_IP_ADDR_LEN : L7_IP6_ADDR_LEN)
                                 == L7_TRUE)
        SET_VALID(I_agentSwitchSnoopingQuerierVlanAddress, \
                  agentSwitchSnoopingQuerierVlanEntryData.valid);
      if (nominator != -1) break;
      /* else pass through */

    case I_agentSwitchSnoopingQuerierOperVersion:
      if (usmDbSnoopQuerierOperVersionGet(vlanId,
           &agentSwitchSnoopingQuerierVlanEntryData.agentSwitchSnoopingQuerierOperVersion,
           family) == L7_SUCCESS)
      {
         SET_VALID(I_agentSwitchSnoopingQuerierOperVersion, agentSwitchSnoopingQuerierVlanEntryData.valid);
      }
      if (nominator != -1) break;
      /* else pass through */

    case I_agentSwitchSnoopingQuerierOperMaxResponseTime:
      if (usmDbSnoopQuerierOperMaxRespTimeGet(vlanId,
           &agentSwitchSnoopingQuerierVlanEntryData.agentSwitchSnoopingQuerierOperMaxResponseTime,
           family) == L7_SUCCESS)
      {
         SET_VALID(I_agentSwitchSnoopingQuerierOperMaxResponseTime, agentSwitchSnoopingQuerierVlanEntryData.valid);
      }
      if (nominator != -1) break;
      /* else pass through */

    case I_agentSwitchSnoopingQuerierLastQuerierAddress:
      if (usmDbSnoopQuerierLastQuerierAddressGet(vlanId, snmp_buffer, family) == L7_SUCCESS
          && SafeMakeOctetString(&agentSwitchSnoopingQuerierVlanEntryData.agentSwitchSnoopingQuerierLastQuerierAddress,
                                 snmp_buffer, (family == L7_AF_INET)? L7_IP_ADDR_LEN : L7_IP6_ADDR_LEN)
                                 == L7_TRUE)
        SET_VALID(I_agentSwitchSnoopingQuerierLastQuerierAddress, \
                  agentSwitchSnoopingQuerierVlanEntryData.valid);
      if (nominator != -1) break;
      /* else pass through */
    case I_agentSwitchSnoopingQuerierLastQuerierVersion:
      if (usmDbSnoopQuerierLastQuerierVersionGet(vlanId,
           &agentSwitchSnoopingQuerierVlanEntryData.agentSwitchSnoopingQuerierLastQuerierVersion,
           family) == L7_SUCCESS)
      {
        SET_VALID(I_agentSwitchSnoopingQuerierLastQuerierVersion, agentSwitchSnoopingQuerierVlanEntryData.valid);
      }
      break;
    default:
    /* unknown nominator value */
    return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, agentSwitchSnoopingQuerierVlanEntryData.valid))
    return(NULL);

  return (&agentSwitchSnoopingQuerierVlanEntryData);
}

#ifdef SETS
int
k_agentSwitchSnoopingQuerierVlanEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                           doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentSwitchSnoopingQuerierVlanEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                            doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}
int
k_agentSwitchSnoopingQuerierVlanEntry_set_defaults(doList_t *dp)
{
    agentSwitchSnoopingQuerierVlanEntry_t *data = (agentSwitchSnoopingQuerierVlanEntry_t *) (dp->data);

    data->agentSwitchSnoopingQuerierVlanAdminMode = D_agentSwitchSnoopingQuerierVlanAdminMode_disable;
    data->agentSwitchSnoopingQuerierVlanOperMode = D_agentSwitchSnoopingQuerierVlanOperMode_disabled;
    data->agentSwitchSnoopingQuerierElectionParticipateMode = D_agentSwitchSnoopingQuerierElectionParticipateMode_disable;
    if ((data->agentSwitchSnoopingQuerierVlanAddress = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->agentSwitchSnoopingQuerierLastQuerierAddress = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentSwitchSnoopingQuerierVlanEntry_set(agentSwitchSnoopingQuerierVlanEntry_t *data,
                                          ContextInfo *contextInfo, int function)
{
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uchar8 family = L7_AF_INET;
  L7_char8 tempValid[sizeof(data->valid)];
  L7_uint32 vlanId;
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  bzero(tempValid, sizeof(tempValid));
  family = (L7_uchar8)data->agentSwitchSnoopingProtocol;
  vlanId = data->dot1qVlanIndex;

  /* Querier VLAN Admin Mode */
  if (VALID(I_agentSwitchSnoopingQuerierVlanAdminMode, data->valid))
  {
    if(snmpAgentSwitchSnoopingQuerierVlanModeSet(vlanId, data->agentSwitchSnoopingQuerierVlanAdminMode,
                                                 family) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentSwitchSnoopingQuerierVlanAdminMode, tempValid);
    }
  }

  /* Querier Election Moden */
  if (VALID(I_agentSwitchSnoopingQuerierElectionParticipateMode, data->valid))
  {
    if(snmpAgentSwitchSnoopingQuerierVlanElectionModeSet(vlanId,
                                                         data->agentSwitchSnoopingQuerierElectionParticipateMode,
                                                         family) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentSwitchSnoopingQuerierElectionParticipateMode, tempValid);
    }
  }

  /* Querier Vlan Address */
  if (VALID(I_agentSwitchSnoopingQuerierVlanAddress, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentSwitchSnoopingQuerierVlanAddress->octet_ptr,
           data->agentSwitchSnoopingQuerierVlanAddress->length);

    if (usmDbSnoopQuerierVlanAddressSet(vlanId, snmp_buffer, family) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentSwitchSnoopingQuerierVlanAddress, tempValid);
    }
  }
  return NO_ERROR;
}

#ifdef SR_agentSwitchSnoopingQuerierVlanEntry_UNDO
/* add #define SR_agentSwitchSnoopingQuerierVlanEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentSwitchSnoopingQuerierVlanEntry family.
 */
int
agentSwitchSnoopingQuerierVlanEntry_undo(doList_t *doHead, doList_t *doCur,
                                         ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentSwitchSnoopingQuerierVlanEntry_UNDO */

#endif /* SETS */


/* End of Snooping Querier */
#ifdef I_agentSwitchVoiceVLANAdminMode
agentSwitchVoiceVLANGroup_t *
k_agentSwitchVoiceVLANGroup_get(int serialNum, ContextInfo *contextInfo,
                                   int nominator)
{
  static agentSwitchVoiceVLANGroup_t agentSwitchVoiceVLANGroupData;

  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_VOICE_VLAN_COMPONENT_ID ) == L7_FALSE )
      return(NULL);

  ZERO_VALID(agentSwitchVoiceVLANGroupData.valid);

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
    if ( nominator != -1) break;
    /* else pass through */

  case I_agentSwitchVoiceVLANAdminMode:
    if (snmpAgentSwitchVoiceVLANAdminModeGet(USMDB_UNIT_CURRENT,
                                             &agentSwitchVoiceVLANGroupData.agentSwitchVoiceVLANAdminMode) == L7_SUCCESS)
      SET_VALID(I_agentSwitchVoiceVLANAdminMode, agentSwitchVoiceVLANGroupData.valid);
    break;

  default:
    /* unknown nominator value */
    return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, agentSwitchVoiceVLANGroupData.valid))
    return(NULL);

  return (&agentSwitchVoiceVLANGroupData);
}

#ifdef SETS
int
k_agentSwitchVoiceVLANGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                    doList_t *dp, ContextInfo *contextInfo)
{
    return NO_ERROR;
}

int
k_agentSwitchVoiceVLANGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                                     doList_t *doHead, doList_t *dp)
{
    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentSwitchVoiceVLANGroup_set(agentSwitchVoiceVLANGroup_t *data,
                                   ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_VOICE_VLAN_COMPONENT_ID ) == L7_FALSE )
      return(COMMIT_FAILED_ERROR);

  if (VALID(I_agentSwitchVoiceVLANAdminMode, data->valid))
  {
    if ( snmpAgentSwitchVoiceVLANAdminModeSet(USMDB_UNIT_CURRENT,
                                              data->agentSwitchVoiceVLANAdminMode) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentSwitchVoiceVLANAdminMode, tempValid);
    }
  }

  return NO_ERROR;
}

#ifdef SR_agentSwitchVoiceVLANGroup_UNDO
/* add #define SR_agentSwitchVoiceVLANGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentSwitchVoiceVLANGroup family.
 */
int
agentSwitchVoiceVLANGroup_undo(doList_t *doHead, doList_t *doCur,
                                  ContextInfo *contextInfo)
{
  agentSwitchVoiceVLANGroup_t *data = (agentSwitchVoiceVLANGroup_t *) doCur->data;
  agentSwitchVoiceVLANGroup_t *undodata = (agentSwitchVoiceVLANGroup_t *) doCur->undodata;
  agentSwitchVoiceVLANGroup_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentSwitchVoiceVLANGroup_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
#endif /* SR_agentSwitchVoiceVLANGroup_UNDO */

#endif /* SETS */

#endif /* I_agentSwitchVoiceVLANAdminMode */

/*#ifdef I_agentSwitchVoiceVlanDeviceEntry*/
agentSwitchVoiceVlanDeviceEntry_t *
k_agentSwitchVoiceVlanDeviceEntry_get(int serialNum, ContextInfo *contextInfo,
                                      int nominator,
                                      int searchType,
                                      SR_INT32 agentSwitchVoiceVlanInterfaceNum,
                                      OctetString * agentSwitchVoiceVlanDeviceMacAddress)
{

   static agentSwitchVoiceVlanDeviceEntry_t agentSwitchVoiceVlanDeviceEntryData;
   static L7_BOOL firstTime = L7_TRUE;
   static L7_BOOL firstCall = L7_FALSE;
   /* L7_char8 snmp_buffer_MacAddress[SNMP_BUFFER_LEN];*/
  /* L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
   L7_uint32 snmp_buffer_len;*/
   L7_char8 mac_buffer[SNMP_BUFFER_LEN];
   L7_char8 name_buffer[SNMP_BUFFER_LEN];
   L7_enetMacAddr_t macAddr;

   /*
    * put your code to retrieve the information here
    */
   if(voiceVlanDeviceDebugFlag== L7_TRUE)
     printf("\n In %s Interface:%d Mac_addr:%s",__FUNCTION__, agentSwitchVoiceVlanInterfaceNum, agentSwitchVoiceVlanDeviceMacAddress->octet_ptr);
   if (firstTime == L7_TRUE)
  {
   firstTime = L7_FALSE;
   agentSwitchVoiceVlanDeviceEntryData.agentSwitchVoiceVlanDeviceMacAddress = MakeOctetString(NULL, 0);
   agentSwitchVoiceVlanDeviceEntryData.agentSwitchVoiceVlanInterfaceNum = 0;   
  }

  if (firstCall == L7_TRUE)
  {
   firstCall = L7_FALSE;
   SafeMakeOctetString(&agentSwitchVoiceVlanDeviceEntryData.agentSwitchVoiceVlanDeviceMacAddress, NULL, 0);
  }

  ZERO_VALID(agentSwitchVoiceVlanDeviceEntryData.valid);
  bzero(name_buffer, SNMP_BUFFER_LEN);
  bzero(mac_buffer, SNMP_BUFFER_LEN);
  memcpy(name_buffer, agentSwitchVoiceVlanDeviceMacAddress->octet_ptr,
                     agentSwitchVoiceVlanDeviceMacAddress->length);

  if(! snmpConvertMacToString(name_buffer, mac_buffer))
  {
    ZERO_VALID(agentSwitchVoiceVlanDeviceEntryData.valid);
    return (NULL);
  }

  agentSwitchVoiceVlanDeviceEntryData.agentSwitchVoiceVlanInterfaceNum = agentSwitchVoiceVlanInterfaceNum;
  SET_VALID(I_agentSwitchVoiceVlanInterfaceNum, agentSwitchVoiceVlanDeviceEntryData.valid);

 /* bzero(snmp_buffer_MacAddress, SNMP_BUFFER_LEN);
  memcpy(snmp_buffer_MacAddress, agentSwitchVoiceVlanDeviceMacAddress->octet_ptr, agentSwitchVoiceVlanDeviceMacAddress->length);*/
  snmpConvertStringToMac ( mac_buffer,macAddr.addr);

  if ( (searchType == EXACT) ?
      (snmpAgentSwitchVoiceVlanDeviceEntryGet(USMDB_UNIT_CURRENT,
                                          agentSwitchVoiceVlanDeviceEntryData.agentSwitchVoiceVlanInterfaceNum,
                                          macAddr.addr) != L7_SUCCESS) :
      ((snmpAgentSwitchVoiceVlanDeviceEntryGet(USMDB_UNIT_CURRENT,
                                           agentSwitchVoiceVlanDeviceEntryData.agentSwitchVoiceVlanInterfaceNum,
                                           macAddr.addr) != L7_SUCCESS) &&
       snmpAgentSwitchVoiceVlanDeviceEntryNextGet(USMDB_UNIT_CURRENT,
                                              &agentSwitchVoiceVlanDeviceEntryData.agentSwitchVoiceVlanInterfaceNum,
                                              macAddr.addr,firstCall) != L7_SUCCESS))
  {
         firstCall = L7_TRUE;
         return(NULL);
  }
 

  /* copy the new MacAddress to the outgoing data structure, return failure if it fails */
 /* if (SafeMakeOctetString(&agentSwitchVoiceVlanDeviceEntryData.agentSwitchVoiceVlanDeviceMacAddress, snmp_buffer_MacAddress, L7_MAC_ADDR_LEN) != L7_TRUE)
    return(NULL);*/
   firstCall = L7_FALSE;
   bzero(name_buffer, SNMP_BUFFER_LEN);

   snmpConvertMacToString (macAddr.addr,mac_buffer);
    
    if(snmpConvertStringToMac(mac_buffer, name_buffer) != L7_SUCCESS)
    {
      ZERO_VALID(agentSwitchVoiceVlanDeviceEntryData.valid);
      return (NULL);
    }
    if(SafeMakeOctetString(&(agentSwitchVoiceVlanDeviceEntryData.agentSwitchVoiceVlanDeviceMacAddress ),
                            name_buffer, L7_ENET_MAC_ADDR_LEN) == L7_TRUE)
    {
      SET_VALID(I_agentSwitchVoiceVlanDeviceMacAddress, agentSwitchVoiceVlanDeviceEntryData.valid);
    }
    else
    {
      ZERO_VALID(agentSwitchVoiceVlanDeviceEntryData.valid);
      return (NULL);
    }

  /*SET_VALID(I_agentSwitchVoiceVlanDeviceMacAddress, agentSwitchVoiceVlanDeviceEntryData.valid);*/

  switch (nominator)
  {
  case -1:
  case I_agentSwitchVoiceVlanInterfaceNum:
  case I_agentSwitchVoiceVlanDeviceMacAddress:
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentSwitchVoiceVlanDeviceEntryData.valid))
    return(NULL);
  
   return(&agentSwitchVoiceVlanDeviceEntryData);

}
/*#endif */

agentSwitchProtectedPortEntry_t *
k_agentSwitchProtectedPortEntry_get(int serialNum, ContextInfo *contextInfo,
                                    int nominator,
                                    int searchType,
                                    SR_INT32 agentSwitchProtectedPortGroupId)
{
   static agentSwitchProtectedPortEntry_t agentSwitchProtectedPortEntryData;
   static L7_BOOL firstTime = L7_TRUE;
   L7_char8 nameBuf[L7_PROTECTED_PORT_GROUP_NAME_SIZE];
   L7_char8 portListBuf[L7_INTF_INDICES];

   if (firstTime == L7_TRUE)
   {
     firstTime = L7_FALSE;
     agentSwitchProtectedPortEntryData.agentSwitchProtectedPortGroupName =
       MakeOctetString(NULL, 0);
     agentSwitchProtectedPortEntryData.agentSwitchProtectedPortPortList =
       MakeOctetString(NULL, 0);
   }

   ZERO_VALID(agentSwitchProtectedPortEntryData.valid);

   agentSwitchProtectedPortEntryData.agentSwitchProtectedPortGroupId =
         agentSwitchProtectedPortGroupId;
   SET_VALID(I_agentSwitchProtectedPortGroupId,
             agentSwitchProtectedPortEntryData.valid);

   if(snmpSwitchProtectedPortEntryGet(
       agentSwitchProtectedPortEntryData.agentSwitchProtectedPortGroupId) !=
     L7_SUCCESS)
   {
     if((searchType == EXACT) ||
        (snmpSwitchProtectedPortEntryGetNext(
          &agentSwitchProtectedPortEntryData.agentSwitchProtectedPortGroupId) !=
        L7_SUCCESS))
     {
       ZERO_VALID(agentSwitchProtectedPortEntryData.valid);
       return(NULL);
     }
   }
   switch(nominator)
   {
     case -1:
     case I_agentSwitchProtectedPortGroupId:
            if (nominator != -1 ) break;
         /* else pass through */
     case I_agentSwitchProtectedPortGroupName:
       memset(nameBuf, 0x00, L7_PROTECTED_PORT_GROUP_NAME_SIZE);
       if((snmpSwitchProtectedPortGroupNameGet(
            agentSwitchProtectedPortEntryData.agentSwitchProtectedPortGroupId,
            nameBuf) == L7_SUCCESS) &&
          (SafeMakeOctetStringFromText(
           &agentSwitchProtectedPortEntryData.agentSwitchProtectedPortGroupName,
           nameBuf) ==  L7_TRUE))
       {

         SET_VALID(I_agentSwitchProtectedPortGroupName,
                   agentSwitchProtectedPortEntryData.valid);
       }
       if (nominator != -1 ) break;
         /* else pass through */
     case I_agentSwitchProtectedPortPortList:
       memset(portListBuf, 0x00, L7_INTF_INDICES);
       if((snmpSwitchProtectedPortPortListGet(
            agentSwitchProtectedPortEntryData.agentSwitchProtectedPortGroupId,
            portListBuf) == L7_SUCCESS) &&
          (SafeMakeOctetString(
            &agentSwitchProtectedPortEntryData.agentSwitchProtectedPortPortList,
            portListBuf, L7_INTF_INDICES) ==  L7_TRUE))
       {
         SET_VALID(I_agentSwitchProtectedPortPortList,
                   agentSwitchProtectedPortEntryData.valid);
       }
       break;
     default:
       return(NULL);
       break;
   }
   if (nominator >= 0 &&
       !VALID(nominator, agentSwitchProtectedPortEntryData.valid))
   {
     return(NULL);
   }
   return(&agentSwitchProtectedPortEntryData);
}

#ifdef SETS
int
k_agentSwitchProtectedPortEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                     doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentSwitchProtectedPortEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                      doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentSwitchProtectedPortEntry_set_defaults(doList_t *dp)
{
    agentSwitchProtectedPortEntry_t *data =
      (agentSwitchProtectedPortEntry_t *) (dp->data);

    if ((data->agentSwitchProtectedPortGroupName =
           MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->agentSwitchProtectedPortPortList =
           MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentSwitchProtectedPortEntry_set(agentSwitchProtectedPortEntry_t *data,
                                    ContextInfo *contextInfo, int function)
{
   L7_char8 tempValid[sizeof(data->valid)];
   L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
   memset(tempValid, 0, sizeof(tempValid));
   if(VALID(I_agentSwitchProtectedPortGroupName,data->valid))
   {
     memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
     memcpy(snmp_buffer, data->agentSwitchProtectedPortGroupName->octet_ptr,
            data->agentSwitchProtectedPortGroupName->length);
     if(snmpSwitchProtectedPortGroupNameSet(
          data->agentSwitchProtectedPortGroupId, snmp_buffer) != L7_SUCCESS)
     {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return COMMIT_FAILED_ERROR;
     }
     else
     {
       SET_VALID(I_agentSwitchProtectedPortGroupName, tempValid);
     }
   }
   if(VALID(I_agentSwitchProtectedPortPortList,data->valid))
   {
     memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
     memcpy(snmp_buffer, data->agentSwitchProtectedPortPortList->octet_ptr,
            data->agentSwitchProtectedPortPortList->length);
     if(snmpSwitchProtectedPortPortListSet(
          data->agentSwitchProtectedPortGroupId, snmp_buffer) != L7_SUCCESS)
     {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return COMMIT_FAILED_ERROR;
     }
   }
   return NO_ERROR;
}

#ifdef SR_agentSwitchProtectedPortEntry_UNDO
/* add #define SR_agentSwitchProtectedPortEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentSwitchProtectedPortEntry family.
 */
int
agentSwitchProtectedPortEntry_undo(doList_t *doHead, doList_t *doCur,
                                   ContextInfo *contextInfo)
{
  agentSwitchProtectedPortEntry_t *data = (agentSwitchProtectedPortEntry_t *) doCur->data;
  agentSwitchProtectedPortEntry_t *undodata = (agentSwitchProtectedPortEntry_t *) doCur->undodata;
  agentSwitchProtectedPortEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) &&
      (k_agentSwitchProtectedPortEntry_set(setdata, contextInfo,
                       function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
#endif /* SR_agentSwitchProtectedPortEntry_UNDO */

#endif /* SETS */

#ifdef L7_DAI_PACKAGE

agentDaiConfigGroup_t *
k_agentDaiConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                          int nominator)
{
  static agentDaiConfigGroup_t agentDaiConfigGroupData;

  ZERO_VALID(agentDaiConfigGroupData.valid);

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
    if (nominator != -1) break;
    /* else pass through */

  case I_agentDaiSrcMacValidate :
    if (snmpDaiSrcMacValidateGet(&agentDaiConfigGroupData.agentDaiSrcMacValidate) == L7_SUCCESS)
      SET_VALID(I_agentDaiSrcMacValidate, agentDaiConfigGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentDaiDstMacValidate :
    if (snmpDaiDstMacValidateGet(&agentDaiConfigGroupData.agentDaiDstMacValidate) == L7_SUCCESS)
      SET_VALID(I_agentDaiDstMacValidate, agentDaiConfigGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentDaiIPValidate :
    if (snmpDaiIPValidateGet(&agentDaiConfigGroupData.agentDaiIPValidate) == L7_SUCCESS)
      SET_VALID(I_agentDaiIPValidate, agentDaiConfigGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentDaiStatsReset :
    agentDaiConfigGroupData.agentDaiStatsReset = D_agentDaiStatsReset_none;
    SET_VALID(I_agentDaiStatsReset, agentDaiConfigGroupData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentDaiConfigGroupData.valid) )
    return(NULL);

  return(&agentDaiConfigGroupData);
}

#ifdef SETS
int
k_agentDaiConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                           doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentDaiConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value, 
                            doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentDaiConfigGroup_set(agentDaiConfigGroup_t *data,
                          ContextInfo *contextInfo, int function)
{
  if (VALID(I_agentDaiSrcMacValidate, data->valid) &&
      snmpDaiSrcMacValidateSet(data->agentDaiSrcMacValidate) != L7_SUCCESS)
  {
      CLR_VALID(I_agentDaiSrcMacValidate, data->valid);
      return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentDaiDstMacValidate, data->valid) &&
      snmpDaiDstMacValidateSet(data->agentDaiDstMacValidate)!= L7_SUCCESS)
  {
    CLR_VALID(I_agentDaiDstMacValidate, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentDaiIPValidate, data->valid) &&
      snmpDaiIPValidateSet(data->agentDaiIPValidate) != L7_SUCCESS)
  {
      CLR_VALID(I_agentDaiIPValidate, data->valid);
      return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentDaiStatsReset, data->valid) &&
      (data->agentDaiStatsReset == D_agentDaiStatsReset_reset) &&
      usmDbDaiVlanStatsClear() != L7_SUCCESS)
  {
      CLR_VALID(I_agentDaiStatsReset, data->valid);
      return COMMIT_FAILED_ERROR;
  }
 
  return NO_ERROR;
}

#ifdef SR_agentDaiConfigGroup_UNDO
/* add #define SR_agentDaiConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentDaiConfigGroup family.
 */
int
agentDaiConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                         ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentDaiConfigGroup_UNDO */

#endif /* SETS */

agentDaiVlanConfigEntry_t *
k_agentDaiVlanConfigEntry_get(int serialNum, ContextInfo *contextInfo,
                              int nominator,
                              int searchType,
                              SR_UINT32 agentDaiVlanIndex)
{
  static agentDaiVlanConfigEntry_t agentDaiVlanConfigEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    agentDaiVlanConfigEntryData.agentDaiVlanArpAclName =
             MakeOctetString(NULL, 0);
  }

  ZERO_VALID(agentDaiVlanConfigEntryData.valid);
  agentDaiVlanConfigEntryData.agentDaiVlanIndex = agentDaiVlanIndex;
  SET_VALID(I_agentDaiVlanIndex, agentDaiVlanConfigEntryData.valid);

  if ((searchType == EXACT) ?
      (snmpDaiVlanGet(USMDB_UNIT_CURRENT,
                       agentDaiVlanConfigEntryData.agentDaiVlanIndex) != L7_SUCCESS) :
      ((snmpDaiVlanGet(USMDB_UNIT_CURRENT,
                       agentDaiVlanConfigEntryData.agentDaiVlanIndex) != L7_SUCCESS) &&
       (snmpDaiVlanNextGet(USMDB_UNIT_CURRENT,
                           &agentDaiVlanConfigEntryData.agentDaiVlanIndex) != L7_SUCCESS)))
  {
    ZERO_VALID(agentDaiVlanConfigEntryData.valid);
    return (NULL);
  }

  switch (nominator)
  {
    case -1:
    case I_agentDaiVlanIndex:
      if (nominator != -1) break;
      /* else pass through */

    case I_agentDaiVlanDynArpInspEnable:
      if(snmpDaiVlanDaiEnableGet(agentDaiVlanConfigEntryData.agentDaiVlanIndex,
                                 &agentDaiVlanConfigEntryData.agentDaiVlanDynArpInspEnable) == L7_SUCCESS)
        SET_VALID(I_agentDaiVlanDynArpInspEnable, agentDaiVlanConfigEntryData.valid);
      if (nominator != -1) break;
      /* else pass through */

    case I_agentDaiVlanLoggingEnable:
      if(snmpDaiVlanLoggingEnableGet(agentDaiVlanConfigEntryData.agentDaiVlanIndex,
                                     &agentDaiVlanConfigEntryData.agentDaiVlanLoggingEnable) == L7_SUCCESS)
        SET_VALID(I_agentDaiVlanLoggingEnable, agentDaiVlanConfigEntryData.valid);
      if (nominator != -1) break;
      /* else pass through */

    case I_agentDaiVlanArpAclName:
      memset(snmp_buffer, 0, (size_t)SNMP_BUFFER_LEN);
      if((usmDbDaiVlanArpAclGet(agentDaiVlanConfigEntryData.agentDaiVlanIndex,
                                snmp_buffer) == L7_SUCCESS) &&
         (SafeMakeOctetString(&agentDaiVlanConfigEntryData.agentDaiVlanArpAclName,
                              snmp_buffer,
                              strlen((char *)snmp_buffer)) == L7_TRUE))
        SET_VALID(I_agentDaiVlanArpAclName, agentDaiVlanConfigEntryData.valid);
      if (nominator != -1) break;
      /* else pass through */

    case I_agentDaiVlanArpAclStaticFlag: 
      if(snmpDaiVlanArpAclStaticFlagGet(agentDaiVlanConfigEntryData.agentDaiVlanIndex,
                                        &agentDaiVlanConfigEntryData.agentDaiVlanArpAclStaticFlag)
                                        == L7_SUCCESS)
        SET_VALID(I_agentDaiVlanArpAclStaticFlag, agentDaiVlanConfigEntryData.valid);
      break;

    default:
      return (NULL);
      break;
  }

  if (nominator >= 0 && !VALID(nominator, agentDaiVlanConfigEntryData.valid))
    return(NULL);

  return(&agentDaiVlanConfigEntryData);
}

#ifdef SETS
int
k_agentDaiVlanConfigEntry_test(ObjectInfo *object, ObjectSyntax *value,
                               doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentDaiVlanConfigEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentDaiVlanConfigEntry_set_defaults(doList_t *dp)
{
    agentDaiVlanConfigEntry_t *data = (agentDaiVlanConfigEntry_t *) (dp->data);

    if ((data->agentDaiVlanArpAclName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentDaiVlanConfigEntry_set(agentDaiVlanConfigEntry_t *data,
                              ContextInfo *contextInfo, int function)
{
  L7_uint32 vlanId;
  L7_char8  snmp_buffer[SNMP_BUFFER_LEN];

  vlanId = data->agentDaiVlanIndex;
  if(snmpDaiVlanGet(USMDB_UNIT_CURRENT, vlanId) != L7_SUCCESS)
  {
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentDaiVlanDynArpInspEnable, data->valid) &&
      snmpDaiVlanDaiEnableSet(vlanId, data->agentDaiVlanDynArpInspEnable) != L7_SUCCESS)
  {
      CLR_VALID(I_agentDaiVlanDynArpInspEnable, data->valid);
      return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentDaiVlanLoggingEnable, data->valid) &&
      snmpDaiVlanLoggingEnableSet(vlanId, data->agentDaiVlanLoggingEnable)!= L7_SUCCESS)
  {
    CLR_VALID(I_agentDaiVlanLoggingEnable, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentDaiVlanArpAclName, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentDaiVlanArpAclName->octet_ptr,
           data->agentDaiVlanArpAclName->length);
    if (data->agentDaiVlanArpAclName->length == 0)
    {
      if(usmDbDaiVlanArpAclGet(vlanId, snmp_buffer) == L7_SUCCESS)
      {
        if(osapiStrncmp(snmp_buffer, "",
                  L7_ARP_ACL_NAME_LEN_MAX+1) != 0)
        {
          if (usmDbDaiVlanARPAclRangeSet(snmp_buffer,vlanId, vlanId, L7_DISABLE, L7_DISABLE) != L7_SUCCESS)
          {
            CLR_VALID(I_agentDaiVlanArpAclName, data->valid);
            return COMMIT_FAILED_ERROR;
          }
        }
      }
    }
    else if(usmDbDaiVlanArpAclSet(vlanId, snmp_buffer) != L7_SUCCESS)
    {
      CLR_VALID(I_agentDaiVlanArpAclName, data->valid);
      return COMMIT_FAILED_ERROR;
    }
  }

  if (VALID(I_agentDaiVlanArpAclStaticFlag, data->valid) &&
      snmpDaiVlanArpAclStaticFlagSet(vlanId, data->agentDaiVlanArpAclStaticFlag) != L7_SUCCESS)
  {
      CLR_VALID(I_agentDaiVlanArpAclStaticFlag, data->valid);
      return COMMIT_FAILED_ERROR;
  }

  return NO_ERROR;
}

#ifdef SR_agentDaiVlanConfigEntry_UNDO
/* add #define SR_agentDaiVlanConfigEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentDaiVlanConfigEntry family.
 */
int
agentDaiVlanConfigEntry_undo(doList_t *doHead, doList_t *doCur,
                             ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentDaiVlanConfigEntry_UNDO */

#endif /* SETS */

agentDaiVlanStatsEntry_t *
k_agentDaiVlanStatsEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_UINT32 agentDaiVlanStatsIndex)
{
  static agentDaiVlanStatsEntry_t agentDaiVlanStatsEntryData;
  daiVlanStats_t stats;

  ZERO_VALID(agentDaiVlanStatsEntryData.valid);
  agentDaiVlanStatsEntryData.agentDaiVlanStatsIndex = agentDaiVlanStatsIndex;
  SET_VALID(I_agentDaiVlanStatsIndex, agentDaiVlanStatsEntryData.valid);

  if (((searchType == EXACT) ?
       (snmpDaiVlanGet(USMDB_UNIT_CURRENT,
                        agentDaiVlanStatsEntryData.agentDaiVlanStatsIndex) != L7_SUCCESS) :
       ((snmpDaiVlanGet(USMDB_UNIT_CURRENT,
                        agentDaiVlanStatsEntryData.agentDaiVlanStatsIndex) != L7_SUCCESS) &&
        (snmpDaiVlanNextGet(USMDB_UNIT_CURRENT,
                            &agentDaiVlanStatsEntryData.agentDaiVlanStatsIndex) != L7_SUCCESS))) ||
      (usmDbDaiVlanStatsGet(agentDaiVlanStatsEntryData.agentDaiVlanStatsIndex,
                            &stats) != L7_SUCCESS))
  {
    ZERO_VALID(agentDaiVlanStatsEntryData.valid);
    return (NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_agentDaiVlanStatsIndex:
    if (nominator != -1) break;
    /* else pass through */

  case I_agentDaiVlanPktsForwarded :
    agentDaiVlanStatsEntryData.agentDaiVlanPktsForwarded = stats.forwarded;
    SET_VALID(I_agentDaiVlanPktsForwarded, agentDaiVlanStatsEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */
    
  case I_agentDaiVlanPktsDropped   :
    agentDaiVlanStatsEntryData.agentDaiVlanPktsDropped = stats.dropped;
    SET_VALID(I_agentDaiVlanPktsDropped, agentDaiVlanStatsEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */
    
  case I_agentDaiVlanDhcpDrops     :
    agentDaiVlanStatsEntryData.agentDaiVlanDhcpDrops = stats.dhcpDrops;
    SET_VALID(I_agentDaiVlanDhcpDrops, agentDaiVlanStatsEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */
    
  case I_agentDaiVlanDhcpPermits   :
    agentDaiVlanStatsEntryData.agentDaiVlanDhcpPermits = stats.dhcpPermits;
    SET_VALID(I_agentDaiVlanDhcpPermits, agentDaiVlanStatsEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */
    
  case I_agentDaiVlanAclDrops      :
    agentDaiVlanStatsEntryData.agentDaiVlanAclDrops = stats.aclDrops;
    SET_VALID(I_agentDaiVlanAclDrops, agentDaiVlanStatsEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */
    
  case I_agentDaiVlanAclPermits    :
    agentDaiVlanStatsEntryData.agentDaiVlanAclPermits = stats.aclPermits;
    SET_VALID(I_agentDaiVlanAclPermits, agentDaiVlanStatsEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */
    
  case I_agentDaiVlanSrcMacFailures:
    agentDaiVlanStatsEntryData.agentDaiVlanSrcMacFailures = stats.sMacFailures;
    SET_VALID(I_agentDaiVlanSrcMacFailures, agentDaiVlanStatsEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */
    
  case I_agentDaiVlanDstMacFailures:
    agentDaiVlanStatsEntryData.agentDaiVlanDstMacFailures = stats.dMacFailures;
    SET_VALID(I_agentDaiVlanDstMacFailures, agentDaiVlanStatsEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */
    
  case I_agentDaiVlanIpValidFailures:
    agentDaiVlanStatsEntryData.agentDaiVlanIpValidFailures = stats.ipValidFailures;
    SET_VALID(I_agentDaiVlanIpValidFailures, agentDaiVlanStatsEntryData.valid);
    break;
    
  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentDaiVlanStatsEntryData.valid) )
    return(NULL);

  return(&agentDaiVlanStatsEntryData);
}

agentDaiIfConfigEntry_t *
k_agentDaiIfConfigEntry_get(int serialNum, ContextInfo *contextInfo,
                            int nominator,
                            int searchType,
                            SR_INT32 ifIndex)
{
  static agentDaiIfConfigEntry_t agentDaiIfConfigEntryData;
  L7_uint32 intIfNum;

  ZERO_VALID(agentDaiIfConfigEntryData.valid);
  agentDaiIfConfigEntryData.ifIndex = ifIndex;
  SET_VALID(I_agentDaiIfConfigEntryIndex_ifIndex, agentDaiIfConfigEntryData.valid);

  if(((searchType == EXACT) ?
      (snmpDaiIntfGet(agentDaiIfConfigEntryData.ifIndex) != L7_SUCCESS) :
      ((snmpDaiIntfGet(agentDaiIfConfigEntryData.ifIndex) != L7_SUCCESS) &&
       (snmpDaiIntfNextGet(agentDaiIfConfigEntryData.ifIndex, &agentDaiIfConfigEntryData.ifIndex) != L7_SUCCESS))) ||
     (usmDbIntIfNumFromExtIfNum(agentDaiIfConfigEntryData.ifIndex, &intIfNum) != L7_SUCCESS))
  {
    ZERO_VALID(agentDaiIfConfigEntryData.valid);
    return (NULL);
  }
   
  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
  case I_agentDaiIfConfigEntryIndex_ifIndex:
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentDaiIfTrustEnable:
    if (snmpDaiPortTrustModeGet(intIfNum, &agentDaiIfConfigEntryData.agentDaiIfTrustEnable) == L7_SUCCESS)
      SET_VALID(I_agentDaiIfTrustEnable, agentDaiIfConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentDaiIfRateLimit:
    if (usmDbDaiIntfRateLimitGet(intIfNum, &agentDaiIfConfigEntryData.agentDaiIfRateLimit) == L7_SUCCESS)
      SET_VALID(I_agentDaiIfRateLimit, agentDaiIfConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentDaiIfBurstInterval:
    if (usmDbDaiIntfBurstIntervalGet(intIfNum, &agentDaiIfConfigEntryData.agentDaiIfBurstInterval) == L7_SUCCESS)
      SET_VALID(I_agentDaiIfBurstInterval, agentDaiIfConfigEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentDaiIfConfigEntryData.valid) )
    return(NULL);

  return(&agentDaiIfConfigEntryData);
}

#ifdef SETS
int
k_agentDaiIfConfigEntry_test(ObjectInfo *object, ObjectSyntax *value,
                             doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentDaiIfConfigEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                              doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentDaiIfConfigEntry_set_defaults(doList_t *dp)
{
    agentDaiIfConfigEntry_t *data = (agentDaiIfConfigEntry_t *) (dp->data);

    data->agentDaiIfRateLimit = 15;
    data->agentDaiIfBurstInterval = 1;

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentDaiIfConfigEntry_set(agentDaiIfConfigEntry_t *data,
                            ContextInfo *contextInfo, int function)
{
  L7_uint32 intIfNum;
  L7_RC_t rc;

  /* Conversion to internal interface number for snmp call */
  rc = usmDbIntIfNumFromExtIfNum(data->ifIndex, &intIfNum);
  if (rc == L7_SUCCESS)
  {
    if (VALID(I_agentDaiIfTrustEnable, data->valid))
    {
      if(snmpDaiPortTrustModeSet(intIfNum, data->agentDaiIfTrustEnable) != L7_SUCCESS)
      {
        CLR_VALID(I_agentDaiIfTrustEnable, data->valid);
        return COMMIT_FAILED_ERROR;
      }
    }
    if (VALID(I_agentDaiIfRateLimit, data->valid))
    {
      if(usmDbDaiIntfRateLimitSet(intIfNum, data->agentDaiIfRateLimit) != L7_SUCCESS)
      {
        CLR_VALID(I_agentDaiIfRateLimit, data->valid);
        return COMMIT_FAILED_ERROR;
      }
    }
    if (VALID(I_agentDaiIfBurstInterval, data->valid))
    {
      if(usmDbDaiIntfBurstIntervalSet(intIfNum, data->agentDaiIfBurstInterval) != L7_SUCCESS)
      {
        CLR_VALID(I_agentDaiIfBurstInterval, data->valid);
        return COMMIT_FAILED_ERROR;
      }
    }
    return NO_ERROR;
  }

  return COMMIT_FAILED_ERROR;
}

#ifdef SR_agentDaiIfConfigEntry_UNDO
/* add #define SR_agentDaiIfConfigEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentDaiIfConfigEntry family.
 */
int
agentDaiIfConfigEntry_undo(doList_t *doHead, doList_t *doCur,
                           ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentDaiIfConfigEntry_UNDO */

#endif /* SETS */

agentArpAclEntry_t *
k_agentArpAclEntry_get(int serialNum, ContextInfo *contextInfo,
                       int nominator,
                       int searchType,
                       OctetString * agentArpAclName)
{
  static agentArpAclEntry_t agentArpAclEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 name_buffer[SNMP_BUFFER_LEN];
  
  if(firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    agentArpAclEntryData.agentArpAclName = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(agentArpAclEntryData.valid);
  bzero(name_buffer, SNMP_BUFFER_LEN);
  memcpy(name_buffer, agentArpAclName->octet_ptr,
                     agentArpAclName->length);

  if((searchType == EXACT) ?
     (usmDbArpAclGet(name_buffer) != L7_SUCCESS) :
     ((usmDbArpAclGet(name_buffer) != L7_SUCCESS) &&
      (usmDbArpAclNextGet(name_buffer, name_buffer) != L7_SUCCESS)))
  {
    ZERO_VALID(agentArpAclEntryData.valid);
    return (NULL);
  }
  else
  {
    if(SafeMakeOctetStringFromTextExact(&(agentArpAclEntryData.agentArpAclName),
                                        name_buffer) == L7_TRUE)
    {
      SET_VALID(I_agentArpAclName, agentArpAclEntryData.valid);
    }
    else
    {
      ZERO_VALID(agentArpAclEntryData.valid);
      return (NULL);
    }
  }

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
  case I_agentArpAclName:
    if (nominator != -1) break;
    /* else pass through */

  case I_agentArpAclRowStatus:
    agentArpAclEntryData.agentArpAclRowStatus = D_agentArpAclRowStatus_active;
    SET_VALID(I_agentArpAclRowStatus, agentArpAclEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentArpAclEntryData.valid) )
    return(NULL);

  return(&agentArpAclEntryData);
}

#ifdef SETS
int
k_agentArpAclEntry_test(ObjectInfo *object, ObjectSyntax *value,
                        doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentArpAclEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                         doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentArpAclEntry_set_defaults(doList_t *dp)
{
    agentArpAclEntry_t *data = (agentArpAclEntry_t *) (dp->data);


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentArpAclEntry_set(agentArpAclEntry_t *data,
                       ContextInfo *contextInfo, int function)
{
  char snmp_buffer[SNMP_BUFFER_LEN];

  if (VALID(I_agentArpAclRowStatus, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentArpAclName->octet_ptr,
                        data->agentArpAclName->length);
    if (snmpArpAclRowStatusSet(snmp_buffer,
                               data->agentArpAclRowStatus) != L7_SUCCESS)
    {
      CLR_VALID(I_agentArpAclRowStatus, data->valid);
      return COMMIT_FAILED_ERROR;
    }
  }
  return NO_ERROR;
}

#ifdef SR_agentArpAclEntry_UNDO
/* add #define SR_agentArpAclEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentArpAclEntry family.
 */
int
agentArpAclEntry_undo(doList_t *doHead, doList_t *doCur,
                      ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentArpAclEntry_UNDO */

#endif /* SETS */

agentArpAclRuleEntry_t *
k_agentArpAclRuleEntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           OctetString * agentArpAclName,
                           SR_UINT32 agentArpAclRuleMatchSenderIpAddr,
                           OctetString * agentArpAclRuleMatchSenderMacAddr)
{
  static agentArpAclRuleEntry_t agentArpAclRuleEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 name_buffer[SNMP_BUFFER_LEN];
  L7_char8 mac_buffer[SNMP_BUFFER_LEN];
  
  if(firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    agentArpAclRuleEntryData.agentArpAclName = MakeOctetString(NULL, 0);
    agentArpAclRuleEntryData.agentArpAclRuleMatchSenderMacAddr = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(agentArpAclRuleEntryData.valid);
  bzero(name_buffer, SNMP_BUFFER_LEN);
  bzero(mac_buffer, SNMP_BUFFER_LEN);
  memcpy(name_buffer, agentArpAclRuleMatchSenderMacAddr->octet_ptr,
                     agentArpAclRuleMatchSenderMacAddr->length);

  if(! snmpConvertMacToString(name_buffer, mac_buffer))
  {
    ZERO_VALID(agentArpAclRuleEntryData.valid);
    return (NULL);
  }
  bzero(name_buffer, SNMP_BUFFER_LEN);
  memcpy(name_buffer, agentArpAclName->octet_ptr,
                      agentArpAclName->length);
  agentArpAclRuleEntryData.agentArpAclRuleMatchSenderIpAddr = agentArpAclRuleMatchSenderIpAddr;
  SET_VALID(I_agentArpAclRuleMatchSenderIpAddr, agentArpAclRuleEntryData.valid);

  if((searchType == EXACT) ?
     (usmDbArpAclRuleGet(name_buffer, agentArpAclRuleMatchSenderIpAddr, mac_buffer) != L7_SUCCESS) :
     ((usmDbArpAclRuleGet(name_buffer, agentArpAclRuleMatchSenderIpAddr, mac_buffer) != L7_SUCCESS) &&
      (usmDbArpAclRuleNextGet(name_buffer, agentArpAclRuleMatchSenderIpAddr, mac_buffer,
                              name_buffer,
                              &agentArpAclRuleEntryData.agentArpAclRuleMatchSenderIpAddr,
                              mac_buffer) != L7_SUCCESS)))
  {
    ZERO_VALID(agentArpAclRuleEntryData.valid);
    return (NULL);
  }
  else
  {
    if(SafeMakeOctetStringFromTextExact(&(agentArpAclRuleEntryData.agentArpAclName),
                                        name_buffer) == L7_TRUE)
    {
      SET_VALID(I_agentArpAclRuleEntryIndex_agentArpAclName, agentArpAclRuleEntryData.valid);
    }
    else
    {
      ZERO_VALID(agentArpAclRuleEntryData.valid);
      return (NULL);
    }
    bzero(name_buffer, SNMP_BUFFER_LEN);
    if(snmpConvertStringToMac(mac_buffer, name_buffer) != L7_SUCCESS)
    {
      ZERO_VALID(agentArpAclRuleEntryData.valid);
      return (NULL);
    }
    if(SafeMakeOctetString(&(agentArpAclRuleEntryData.agentArpAclRuleMatchSenderMacAddr),
                            name_buffer, L7_ENET_MAC_ADDR_LEN) == L7_TRUE)
    {
      SET_VALID(I_agentArpAclRuleMatchSenderMacAddr, agentArpAclRuleEntryData.valid);
    }
    else
    {
      ZERO_VALID(agentArpAclRuleEntryData.valid);
      return (NULL);
    }
  }
                         
  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
  case I_agentArpAclRuleEntryIndex_agentArpAclName:
  case I_agentArpAclRuleMatchSenderIpAddr:
  case I_agentArpAclRuleMatchSenderMacAddr:
    if (nominator != -1) break;
    /* else pass through */

  case I_agentArpAclRuleRowStatus:
    agentArpAclRuleEntryData.agentArpAclRuleRowStatus = D_agentArpAclRuleRowStatus_active;
    SET_VALID(I_agentArpAclRuleRowStatus, agentArpAclRuleEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentArpAclRuleEntryData.valid) )
    return(NULL);
  return(&agentArpAclRuleEntryData);
}

#ifdef SETS
int
k_agentArpAclRuleEntry_test(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentArpAclRuleEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                             doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentArpAclRuleEntry_set_defaults(doList_t *dp)
{
    agentArpAclRuleEntry_t *data = (agentArpAclRuleEntry_t *) (dp->data);


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentArpAclRuleEntry_set(agentArpAclRuleEntry_t *data,
                           ContextInfo *contextInfo, int function)
{
  char snmp_buffer[SNMP_BUFFER_LEN];
  char mac_buffer[SNMP_BUFFER_LEN];

  if (VALID(I_agentArpAclRuleRowStatus, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    bzero(mac_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentArpAclRuleMatchSenderMacAddr->octet_ptr,
                       data->agentArpAclRuleMatchSenderMacAddr->length);
    if(! snmpConvertMacToString(snmp_buffer, mac_buffer))
    {
      CLR_VALID(I_agentArpAclRuleRowStatus, data->valid);
      return COMMIT_FAILED_ERROR;
    }
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentArpAclName->octet_ptr,
                        data->agentArpAclName->length);
    if (snmpArpAclRuleRowStatusSet(snmp_buffer, data->agentArpAclRuleMatchSenderIpAddr,
                                   mac_buffer,
                                   data->agentArpAclRuleRowStatus) != L7_SUCCESS)
    {
      CLR_VALID(I_agentArpAclRuleRowStatus, data->valid);
      return COMMIT_FAILED_ERROR;
    }
  }
  return NO_ERROR;
}

#ifdef SR_agentArpAclRuleEntry_UNDO
/* add #define SR_agentArpAclRuleEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentArpAclRuleEntry family.
 */
int
agentArpAclRuleEntry_undo(doList_t *doHead, doList_t *doCur,
                          ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentArpAclRuleEntry_UNDO */

#endif /* SETS */

#endif /* L7_DAI_PACKAGE */

agentTransferUploadGroup_t *
k_agentTransferUploadGroup_get(int serialNum, ContextInfo *contextInfo,
                               int nominator)
{
  static agentTransferUploadGroup_t agentTransferUploadGroupData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  
  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;

    agentTransferUploadGroupData.agentTransferUploadPath = MakeOctetString(NULL, 0);
    agentTransferUploadGroupData.agentTransferUploadFilename = MakeOctetString(NULL, 0);
    agentTransferUploadGroupData.agentTransferUploadServerAddress = MakeOctetString(NULL, 0);
    agentTransferUploadGroupData.agentTransferUploadUsername = MakeOctetString(NULL, 0);
    agentTransferUploadGroupData.agentTransferUploadPassword  = MakeOctetString(NULL, 0);
  }

   ZERO_VALID(agentTransferUploadGroupData.valid); 

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
    
  case I_agentTransferUploadMode :
    CLR_VALID(I_agentTransferUploadMode, agentTransferUploadGroupData.valid);
    if (snmpAgentTransferUploadModeGet(USMDB_UNIT_CURRENT,
                                       &agentTransferUploadGroupData.agentTransferUploadMode) == L7_SUCCESS)
      SET_VALID(I_agentTransferUploadMode, agentTransferUploadGroupData.valid);


    if (nominator != -1) break;
    /* else pass through */

  case I_agentTransferUploadServerAddressType:
    CLR_VALID(I_agentTransferUploadServerAddressType, agentTransferUploadGroupData.valid);
    if (snmpAgentTransferUploadServerAddressTypeGet(USMDB_UNIT_CURRENT,
                 &agentTransferUploadGroupData.agentTransferUploadServerAddressType) == L7_SUCCESS)
      SET_VALID(I_agentTransferUploadServerAddressType, agentTransferUploadGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentTransferUploadServerAddress:
    CLR_VALID(I_agentTransferUploadServerAddress, agentTransferUploadGroupData.valid);
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (snmpAgentTransferServerAddressGet(USMDB_UNIT_CURRENT, snmp_buffer, sizeof(snmp_buffer)) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentTransferUploadGroupData.agentTransferUploadServerAddress, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentTransferUploadServerAddress, agentTransferUploadGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentTransferUploadPath :
    CLR_VALID(I_agentTransferUploadPath, agentTransferUploadGroupData.valid);
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
/* lvl7_@p0772 start */
    if (usmDbTransferFilePathRemoteGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentTransferUploadGroupData.agentTransferUploadPath, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentTransferUploadPath, agentTransferUploadGroupData.valid);
/* lvl7_@p0772 end */
    if (nominator != -1) break;
    /* else pass through */

#ifdef L7_MGMT_SECURITY_PACKAGE
  case I_agentTransferUploadUsername :
    CLR_VALID(I_agentTransferUploadUsername, agentTransferUploadGroupData.valid);
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
/* lvl7_@p0772 start */
    if (usmDbSshcTransferRemoteUsernameGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentTransferUploadGroupData.agentTransferUploadUsername, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentTransferUploadUsername, agentTransferUploadGroupData.valid);
/* lvl7_@p0772 end */
    if (nominator != -1) break;
    /* else pass through */
#endif /* L7_MGMT_SECURITY_PACKAGE */

  case I_agentTransferUploadFilename :
    CLR_VALID(I_agentTransferUploadFilename, agentTransferUploadGroupData.valid);
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
/* lvl7_@p0772 start */
    if (usmDbTransferFileNameRemoteGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentTransferUploadGroupData.agentTransferUploadFilename, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentTransferUploadFilename, agentTransferUploadGroupData.valid);
/* lvl7_@p0772 end */
    if (nominator != -1) break;
    /* else pass through */

  case I_agentTransferUploadImagename :
    CLR_VALID(I_agentTransferUploadImagename, agentTransferUploadGroupData.valid);
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
/* lvl7_@p0772 start */
    if (usmDbTransferFileNameLocalGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS)
    {
      if (strcmp(snmp_buffer, USMDB_IMAGE1_NAME) == 0)
      {
        agentTransferUploadGroupData.agentTransferUploadImagename = D_agentTransferUploadImagename_image1;
      }
      else if (strcmp(snmp_buffer, USMDB_IMAGE2_NAME) == 0)
      {
        agentTransferUploadGroupData.agentTransferUploadImagename = D_agentTransferUploadImagename_image2;
      }
#ifdef D_agentTransferUploadImagename_unknown
      else
      {
        agentTransferUploadGroupData.agentTransferUploadImagename = D_agentTransferUploadImagename_unknown;
      }
#endif

      SET_VALID(I_agentTransferUploadImagename, agentTransferUploadGroupData.valid);
    }
/* lvl7_@p0772 end */
    if (nominator != -1) break;
    /* else pass through */

  case I_agentTransferUploadDataType :
    CLR_VALID(I_agentTransferUploadDataType, agentTransferUploadGroupData.valid);
    if (snmpAgentTransferUploadDataTypeGet(USMDB_UNIT_CURRENT,
                                           &agentTransferUploadGroupData.agentTransferUploadDataType) == L7_SUCCESS)
      SET_VALID(I_agentTransferUploadDataType, agentTransferUploadGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentTransferUploadStart :
    SET_VALID(I_agentTransferUploadStart, agentTransferUploadGroupData.valid);
    agentTransferUploadGroupData.agentTransferUploadStart = D_agentTransferUploadStart_disable;
    if (nominator != -1) break;
    /* else pass through */

  case I_agentTransferUploadStatus :
    CLR_VALID(I_agentTransferUploadStatus, agentTransferUploadGroupData.valid);
    if (snmpAgentTransferUploadStatusGet(USMDB_UNIT_CURRENT,
                                         &agentTransferUploadGroupData.agentTransferUploadStatus) == L7_SUCCESS)
      SET_VALID(I_agentTransferUploadStatus, agentTransferUploadGroupData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentTransferUploadGroupData.valid) )
    return(NULL);

  return(&agentTransferUploadGroupData);
}

#ifdef SETS
int
k_agentTransferUploadGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentTransferUploadGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                                 doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentTransferUploadGroup_set(agentTransferUploadGroup_t *data,
                               ContextInfo *contextInfo, int function)
{
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));


  if ( VALID(I_agentTransferUploadMode, data->valid))
  {
    if( snmpAgentTransferUploadModeSet(USMDB_UNIT_CURRENT,
                                      data->agentTransferUploadMode) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentTransferUploadMode, tempValid);
    }
  }

  if ( VALID(I_agentTransferUploadServerAddressType, data->valid))
  {
    if( snmpAgentTransferUploadServerAddressTypeSet(USMDB_UNIT_CURRENT,
                                              data->agentTransferUploadServerAddressType)
                                              != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentTransferUploadServerAddressType, tempValid);
    }
  }

  if (VALID(I_agentTransferUploadServerAddress, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    strncpy(snmp_buffer, data->agentTransferUploadServerAddress->octet_ptr,
            data->agentTransferUploadServerAddress->length);
    if (snmpAgentTransferServerAddressSet(USMDB_UNIT_CURRENT,
                                          snmp_buffer) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    SET_VALID(I_agentTransferUploadServerAddress, tempValid);
  }

  if (data->agentTransferUploadPath != NULL)
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentTransferUploadPath->octet_ptr, data->agentTransferUploadPath->length);
    if ( VALID(I_agentTransferUploadPath, data->valid))
    {
      if( snmpAgentTransferFilePathSet(USMDB_UNIT_CURRENT, snmp_buffer) != L7_SUCCESS )
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
      else
      {
        SET_VALID(I_agentTransferUploadPath, tempValid);
      }
    }
  }

  if (VALID(I_agentTransferUploadFilename, data->valid) && data->agentTransferUploadFilename != NULL)
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentTransferUploadFilename->octet_ptr, data->agentTransferUploadFilename->length);
    if (snmpAgentTransferFileNameSet(USMDB_UNIT_CURRENT, snmp_buffer) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentTransferUploadFilename, tempValid);
    }
  }

  if (VALID(I_agentTransferUploadImagename, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);

    if(data->agentTransferUploadImagename == D_agentTransferUploadImagename_image1)
    {
      usmDbTransferFileNameLocalSet(USMDB_UNIT_CURRENT, USMDB_IMAGE1_NAME);
    }
    else if (data->agentTransferUploadImagename == D_agentTransferUploadImagename_image2)
    {
      usmDbTransferFileNameLocalSet(USMDB_UNIT_CURRENT, USMDB_IMAGE2_NAME);
    }
    else
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }

    SET_VALID(I_agentTransferUploadImagename, tempValid);

  }

  if ( VALID(I_agentTransferUploadDataType, data->valid))
  {
    if( snmpAgentTransferUploadDataTypeSet(USMDB_UNIT_CURRENT,
                                          data->agentTransferUploadDataType) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentTransferUploadDataType, tempValid);
    }
  }

#ifdef L7_MGMT_SECURITY_PACKAGE
  if (VALID(I_agentTransferUploadUsername, data->valid) && (data->agentTransferUploadUsername != NULL))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentTransferUploadUsername->octet_ptr, data->agentTransferUploadUsername->length);
    if (usmDbSshcTransferRemoteUsernameSet(USMDB_UNIT_CURRENT, snmp_buffer) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentTransferUploadUsername, tempValid);
    }
  }
  if (VALID(I_agentTransferUploadPassword, data->valid) && (data->agentTransferUploadPassword != NULL))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentTransferUploadPassword->octet_ptr, data->agentTransferUploadPassword->length);
    if (usmDbSshcTransferRemotePasswordSet(USMDB_UNIT_CURRENT, snmp_buffer) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentTransferUploadPassword, tempValid);
    }
  }
#endif /* L7_MGMT_SECURITY_PACKAGE */

  if ( VALID(I_agentTransferUploadStart, data->valid))
  {
    if( snmpAgentTransferUploadStartSet(USMDB_UNIT_CURRENT,
                                       data->agentTransferUploadStart) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
  }

  return NO_ERROR;
}


  #ifdef SR_agentTransferUploadGroup_UNDO
/* add #define SR_agentTransferUploadGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentTransferUploadGroup family.
 */
int
agentTransferUploadGroup_undo(doList_t *doHead, doList_t *doCur,
                              ContextInfo *contextInfo)
{
  agentTransferUploadGroup_t *data = (agentTransferUploadGroup_t *) doCur->data;
  agentTransferUploadGroup_t *undodata = (agentTransferUploadGroup_t *) doCur->undodata;
  agentTransferUploadGroup_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentTransferUploadGroup_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentTransferUploadGroup_UNDO */

#endif /* SETS */

agentTransferDownloadGroup_t *
k_agentTransferDownloadGroup_get(int serialNum, ContextInfo *contextInfo,
                                 int nominator)
{
  static agentTransferDownloadGroup_t agentTransferDownloadGroupData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;

    agentTransferDownloadGroupData.agentTransferDownloadPath = MakeOctetString(NULL, 0);
    agentTransferDownloadGroupData.agentTransferDownloadFilename = MakeOctetString(NULL, 0);
    agentTransferDownloadGroupData.agentTransferDownloadServerAddress = MakeOctetString(NULL, 0);
     agentTransferDownloadGroupData.agentTransferDownloadUsername = MakeOctetString(NULL, 0);
    agentTransferDownloadGroupData.agentTransferDownloadPassword  = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(agentTransferDownloadGroupData.valid);

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
    
  case I_agentTransferDownloadMode :
    if (snmpAgentTransferDownloadModeGet(USMDB_UNIT_CURRENT,
                                         &agentTransferDownloadGroupData.agentTransferDownloadMode) == L7_SUCCESS)
      SET_VALID(I_agentTransferDownloadMode, agentTransferDownloadGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentTransferDownloadServerAddressType:
    CLR_VALID(I_agentTransferDownloadServerAddressType, agentTransferDownloadGroupData.valid);
    if (snmpAgentTransferDownloadServerAddressTypeGet(USMDB_UNIT_CURRENT,
                 &agentTransferDownloadGroupData.agentTransferDownloadServerAddressType)
                                                                          == L7_SUCCESS)
      SET_VALID(I_agentTransferDownloadServerAddressType, agentTransferDownloadGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentTransferDownloadServerAddress:
    CLR_VALID(I_agentTransferDownloadServerAddress, agentTransferDownloadGroupData.valid);
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (snmpAgentTransferServerAddressGet(USMDB_UNIT_CURRENT, snmp_buffer, sizeof(snmp_buffer)) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentTransferDownloadGroupData.agentTransferDownloadServerAddress, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentTransferDownloadServerAddress, agentTransferDownloadGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentTransferDownloadPath :
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
/* lvl7_@p0772 start */
    if (usmDbTransferFilePathRemoteGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentTransferDownloadGroupData.agentTransferDownloadPath, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentTransferDownloadPath, agentTransferDownloadGroupData.valid);
/* lvl7_@p0772 endt */
    if ( nominator != -1 ) break;
    /* else pass through */

#ifdef L7_MGMT_SECURITY_PACKAGE
  case I_agentTransferDownloadUsername :
    CLR_VALID(I_agentTransferDownloadUsername, agentTransferDownloadGroupData.valid);
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
/* lvl7_@p0772 start */
    if (usmDbSshcTransferRemoteUsernameGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentTransferDownloadGroupData.agentTransferDownloadUsername, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentTransferDownloadUsername, agentTransferDownloadGroupData.valid);
/* lvl7_@p0772 end */
    if (nominator != -1) break;
    /* else pass through */
#endif /* L7_MGMT_SECURITY_PACKAGE */

  case I_agentTransferDownloadFilename :
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
/* lvl7_@p0772 start */
    if (usmDbTransferFileNameRemoteGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentTransferDownloadGroupData.agentTransferDownloadFilename, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentTransferDownloadFilename, agentTransferDownloadGroupData.valid);
/* lvl7_@p0772 end */
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentTransferDownloadImagename :
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
/* lvl7_@p0772 start */
    if (usmDbTransferFileNameLocalGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS)
    {
      if(strcmp(snmp_buffer, USMDB_IMAGE1_NAME) == 0)
      {
        agentTransferDownloadGroupData.agentTransferDownloadImagename = D_agentTransferDownloadImagename_image1;
      }
      else if (strcmp(snmp_buffer, USMDB_IMAGE2_NAME) == 0)
      {
        agentTransferDownloadGroupData.agentTransferDownloadImagename = D_agentTransferDownloadImagename_image2;
      }
#ifdef D_agentTransferDownloadImagename_unknown
      else
      {
        agentTransferDownloadGroupData.agentTransferDownloadImagename = D_agentTransferDownloadImagename_unknown;
      }
#endif

      SET_VALID(I_agentTransferDownloadImagename, agentTransferDownloadGroupData.valid);
    }

    /* lvl7_@p0772 end */
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentTransferDownloadDataType :
    if (snmpAgentTransferDownloadDataTypeGet(USMDB_UNIT_CURRENT,
                                             &agentTransferDownloadGroupData.agentTransferDownloadDataType) == L7_SUCCESS)
      SET_VALID(I_agentTransferDownloadDataType, agentTransferDownloadGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentTransferDownloadStart:
    agentTransferDownloadGroupData.agentTransferDownloadStart = D_agentTransferDownloadStart_disable;
    SET_VALID(I_agentTransferDownloadStart, agentTransferDownloadGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentTransferDownloadStatus :
    CLR_VALID(I_agentTransferDownloadStatus, agentTransferDownloadGroupData.valid);
    if (snmpAgentTransferDownloadStatusGet(USMDB_UNIT_CURRENT,
                                           &agentTransferDownloadGroupData.agentTransferDownloadStatus) == L7_SUCCESS)
      SET_VALID(I_agentTransferDownloadStatus, agentTransferDownloadGroupData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentTransferDownloadGroupData.valid) )
    return(NULL);

  return(&agentTransferDownloadGroupData);
}

#ifdef SETS
int
k_agentTransferDownloadGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                  doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentTransferDownloadGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                                   doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentTransferDownloadGroup_set(agentTransferDownloadGroup_t *data,
                                 ContextInfo *contextInfo, int function)
{
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if ( VALID(I_agentTransferDownloadMode, data->valid))
  {
    if( snmpAgentTransferDownloadModeSet(USMDB_UNIT_CURRENT,
                                        data->agentTransferDownloadMode) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentTransferDownloadMode, tempValid);
    }
  }

  if ( VALID(I_agentTransferDownloadServerAddressType, data->valid))
  {
    
    if( snmpAgentTransferDownloadServerAddressTypeSet(USMDB_UNIT_CURRENT,
                                              data->agentTransferDownloadServerAddressType)
                                              != L7_SUCCESS )
    {
      
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentTransferDownloadServerAddressType, tempValid);
    }
  }

  if (VALID(I_agentTransferDownloadServerAddress, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    strncpy(snmp_buffer, data->agentTransferDownloadServerAddress->octet_ptr,
            data->agentTransferDownloadServerAddress->length);
    if (snmpAgentTransferServerAddressSet(USMDB_UNIT_CURRENT,
                                          snmp_buffer) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    SET_VALID(I_agentTransferDownloadServerAddress, tempValid);
  }

  if (VALID(I_agentTransferDownloadPath, data->valid) && (data->agentTransferDownloadPath != NULL))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentTransferDownloadPath->octet_ptr, data->agentTransferDownloadPath->length);
    if (snmpAgentTransferFilePathSet(USMDB_UNIT_CURRENT, snmp_buffer) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentTransferDownloadPath, tempValid);
    }
  }

  if (VALID(I_agentTransferDownloadFilename, data->valid) && (data->agentTransferDownloadFilename != NULL))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentTransferDownloadFilename->octet_ptr, data->agentTransferDownloadFilename->length);
    if (snmpAgentTransferFileNameSet(USMDB_UNIT_CURRENT, snmp_buffer) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentTransferDownloadFilename, tempValid);
    }
  }

  if (VALID(I_agentTransferDownloadImagename, data->valid) && (data->agentTransferDownloadImagename != 0))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);

    if(data->agentTransferDownloadImagename == D_agentTransferDownloadImagename_image1)
    {
      usmDbTransferFileNameLocalSet(USMDB_UNIT_CURRENT, USMDB_IMAGE1_NAME);
    }
    else if (data->agentTransferDownloadImagename == D_agentTransferDownloadImagename_image2)
    {
      usmDbTransferFileNameLocalSet(USMDB_UNIT_CURRENT, USMDB_IMAGE2_NAME);
    }
    else
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }

    SET_VALID(I_agentTransferDownloadFilename, tempValid);

  }


  if ( VALID(I_agentTransferDownloadDataType, data->valid))
  {
    if( snmpAgentTransferDownloadDataTypeSet(USMDB_UNIT_CURRENT,
                                            data->agentTransferDownloadDataType) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentTransferDownloadDataType, tempValid);
    }
  }

#ifdef L7_MGMT_SECURITY_PACKAGE
  if (VALID(I_agentTransferDownloadUsername, data->valid) && (data->agentTransferDownloadUsername != NULL))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentTransferDownloadUsername->octet_ptr, data->agentTransferDownloadUsername->length);
    if (usmDbSshcTransferRemoteUsernameSet(USMDB_UNIT_CURRENT, snmp_buffer) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentTransferDownloadUsername, tempValid);
    }
  }
  if (VALID(I_agentTransferDownloadPassword, data->valid) && (data->agentTransferDownloadPassword != NULL))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentTransferDownloadPassword->octet_ptr, data->agentTransferDownloadPassword->length);
    if (usmDbSshcTransferRemotePasswordSet(USMDB_UNIT_CURRENT, snmp_buffer) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentTransferDownloadPassword, tempValid);
    }
  }
#endif /* L7_MGMT_SECURITY_PACKAGE */

  if ( VALID(I_agentTransferDownloadStart, data->valid))
  {
    if( snmpAgentTransferDownloadStartSet(USMDB_UNIT_CURRENT,
                                         data->agentTransferDownloadStart) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
  }

  return NO_ERROR;
}
  #ifdef SR_agentTransferDownloadGroup_UNDO
/* add #define SR_agentTransferDownloadGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentTransferDownloadGroup family.
 */
int
agentTransferDownloadGroup_undo(doList_t *doHead, doList_t *doCur,
                                ContextInfo *contextInfo)
{
  agentTransferDownloadGroup_t *data = (agentTransferDownloadGroup_t *) doCur->data;
  agentTransferDownloadGroup_t *undodata = (agentTransferDownloadGroup_t *) doCur->undodata;
  agentTransferDownloadGroup_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentTransferDownloadGroup_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentTransferDownloadGroup_UNDO */

#endif /* SETS */

/* switchimage starts */

agentImageConfigGroup_t *
k_agentImageConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                              int nominator)
{
static agentImageConfigGroup_t agentImageConfigGroupData;
static L7_BOOL firstTime = L7_TRUE;
L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
L7_uint32 unitId;

   usmDbUnitMgrNumberGet(&unitId);

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;

    agentImageConfigGroupData.agentImage1 = MakeOctetString(NULL, 0);
    agentImageConfigGroupData.agentImage2 = MakeOctetString(NULL, 0);
    agentImageConfigGroupData.agentActiveImage = MakeOctetString(NULL,0);
    agentImageConfigGroupData.agentNextActiveImage = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(agentImageConfigGroupData.valid);

  switch (nominator)
  {
  case -1:
    if (nominator != -1) break;
    /* else pass through */
  case I_agentImage1 :
    CLR_VALID(I_agentImage1, agentImageConfigGroupData.valid);
    bzero(snmp_buffer, SNMP_BUFFER_LEN);

    if (usmDbImageVersionGet(unitId, USMDB_IMAGE1_NAME, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentImageConfigGroupData.agentImage1, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentImage1, agentImageConfigGroupData.valid);
    if (nominator != -1) break;

  case I_agentImage2 :
    CLR_VALID(I_agentImage2, agentImageConfigGroupData.valid);
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbImageVersionGet(unitId, USMDB_IMAGE2_NAME, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentImageConfigGroupData.agentImage2, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentImage2, agentImageConfigGroupData.valid);
    if (nominator != -1) break;

  case I_agentActiveImage :
    CLR_VALID(I_agentActiveImage, agentImageConfigGroupData.valid);
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbActiveImageNameGet(unitId, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentImageConfigGroupData.agentActiveImage, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentActiveImage, agentImageConfigGroupData.valid);
    if (nominator != -1) break;
  case I_agentNextActiveImage :
    CLR_VALID(I_agentNextActiveImage, agentImageConfigGroupData.valid);
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbActivatedImageNameGet(unitId, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact(&agentImageConfigGroupData.agentNextActiveImage, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentNextActiveImage, agentImageConfigGroupData.valid);
    break;
  default:
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentImageConfigGroupData.valid) )
    return(NULL);

  return(&agentImageConfigGroupData);

}

#ifdef SETS
int
k_agentImageConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                             doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentImageConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentImageConfigGroup_set(agentImageConfigGroup_t *data,
                            ContextInfo *contextInfo, int function)
{
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (VALID(I_agentNextActiveImage, data->valid) && data->agentNextActiveImage != NULL)
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentNextActiveImage->octet_ptr, data->agentNextActiveImage->length);
    if (usmDbImageActivate(USMDB_UNIT_CURRENT, snmp_buffer) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentNextActiveImage, tempValid);
      return NO_ERROR;
    }
  }

  return COMMIT_FAILED_ERROR;
}

#ifdef SR_agentImageConfigGroup_UNDO
/* add #define SR_agentImageConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentImageConfigGroup family.
 */
int
agentImageConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                           ContextInfo *contextInfo)
{
  agentImageConfigGroup_t *data = (agentImageConfigGroup_t *) doCur->data;
  agentImageConfigGroup_t *undodata = (agentImageConfigGroup_t *) doCur->undodata;
  agentImageConfigGroup_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentImageConfigGroup_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
#endif /* SR_agentImageConfigGroup_UNDO */

#endif /* SETS */

/* switchimage ends */

agentPortMirroringGroup_t *
k_agentPortMirroringGroup_get(int serialNum, ContextInfo *contextInfo,
                              int nominator)
{
#ifdef OLD_CODE
  static agentPortMirroringGroup_t agentPortMirroringGroupData;

  ZERO_VALID(agentPortMirroringGroupData.valid);

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
    if (nominator != -1) break;
    /* else pass through */

  case I_agentMirroredPortIfIndex :
    if (snmpAgentMirroredPortIfIndexGet(USMDB_UNIT_CURRENT,
                                        &agentPortMirroringGroupData.agentMirroredPortIfIndex) == L7_SUCCESS)
      SET_VALID(I_agentMirroredPortIfIndex, agentPortMirroringGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentProbePortIfIndex :
    if (snmpAgentProbePortIfIndexGet(USMDB_UNIT_CURRENT,
                                     &agentPortMirroringGroupData.agentProbePortIfIndex) == L7_SUCCESS)
      SET_VALID(I_agentProbePortIfIndex, agentPortMirroringGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentPortMirroringMode :
    if (snmpAgentPortMirroringModeGet(USMDB_UNIT_CURRENT,
                                      &agentPortMirroringGroupData.agentPortMirroringMode) == L7_SUCCESS)
      SET_VALID(I_agentPortMirroringMode, agentPortMirroringGroupData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentPortMirroringGroupData.valid) )
    return(NULL);

  return(&agentPortMirroringGroupData);
#else
  return(NULL);
#endif
}

#ifdef SETS
int
k_agentPortMirroringGroup_test(ObjectInfo *object, ObjectSyntax *value,
                               doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentPortMirroringGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                                doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentPortMirroringGroup_set(agentPortMirroringGroup_t *data,
                              ContextInfo *contextInfo, int function)
{
#ifdef OLD_CODE
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (VALID(I_agentMirroredPortIfIndex, data->valid))
  {
    if( snmpAgentMirroredPortIfIndexSet(USMDB_UNIT_CURRENT, data->agentMirroredPortIfIndex) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentMirroredPortIfIndex, tempValid);
    }
  }

  if (VALID(I_agentProbePortIfIndex, data->valid))
  {
    if( snmpAgentProbePortIfIndexSet(USMDB_UNIT_CURRENT, data->agentProbePortIfIndex) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentProbePortIfIndex, tempValid);
    }
  }

  if (VALID(I_agentPortMirroringMode, data->valid))
  {
    if( snmpAgentPortMirroringModeSet(USMDB_UNIT_CURRENT, data->agentPortMirroringMode) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;
#else
  return COMMIT_FAILED_ERROR;
#endif
}

  #ifdef SR_agentPortMirroringGroup_UNDO
/* add #define SR_agentPortMirroringGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentPortMirroringGroup family.
 */
int
agentPortMirroringGroup_undo(doList_t *doHead, doList_t *doCur,
                             ContextInfo *contextInfo)
{
  agentPortMirroringGroup_t *data = (agentPortMirroringGroup_t *) doCur->data;
  agentPortMirroringGroup_t *undodata = (agentPortMirroringGroup_t *) doCur->undodata;
  agentPortMirroringGroup_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentPortMirroringGroup_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentPortMirroringGroup_UNDO */

#endif /* SETS */

agentPortMirrorEntry_t *
k_agentPortMirrorEntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           SR_UINT32 agentPortMirrorSessionNum)
{
  static agentPortMirrorEntry_t agentPortMirrorEntryData;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 snmp_buffer_len=0;



  agentPortMirrorEntryData.agentPortMirrorSessionNum = agentPortMirrorSessionNum;
  SET_VALID(I_agentPortMirrorSessionNum, agentPortMirrorEntryData.valid);

  if ((searchType == EXACT) ?
     (snmpAgentPortMirrorEntryGet(agentPortMirrorEntryData.agentPortMirrorSessionNum) != L7_SUCCESS) :
     ((snmpAgentPortMirrorEntryGet(agentPortMirrorEntryData.agentPortMirrorSessionNum) != L7_SUCCESS) &&
      (snmpAgentPortMirrorEntryNextGet(&agentPortMirrorEntryData.agentPortMirrorSessionNum) != L7_SUCCESS)))
    return(NULL);

  switch (nominator)
  {
  case -1:
  case I_agentPortMirrorSessionNum:
    if (nominator != -1)
      break;

  case I_agentPortMirrorDestinationPort:
    if (usmDbSwPortMonitorDestPortGet(USMDB_UNIT_CURRENT,
                                      agentPortMirrorEntryData.agentPortMirrorSessionNum,
                                      &agentPortMirrorEntryData.agentPortMirrorDestinationPort) == L7_SUCCESS)
      SET_VALID(I_agentPortMirrorDestinationPort, agentPortMirrorEntryData.valid);

    if (nominator != -1)
      break;


  case I_agentPortMirrorSourcePortMask:
    if (snmpAgentPortMirrorSourcePortMaskGet(agentPortMirrorEntryData.agentPortMirrorSessionNum,
                                             snmp_buffer, &snmp_buffer_len) == L7_SUCCESS)
    {
      if (SafeMakeOctetString(&agentPortMirrorEntryData.agentPortMirrorSourcePortMask,
                              snmp_buffer, snmp_buffer_len) == L7_TRUE)
        SET_VALID(I_agentPortMirrorSourcePortMask, agentPortMirrorEntryData.valid);
    }

    if (nominator != -1)
      break;

  case I_agentPortMirrorAdminMode:
    if (snmpAgentPortMirrorAdminModeGet(agentPortMirrorEntryData.agentPortMirrorSessionNum,
                                        &agentPortMirrorEntryData.agentPortMirrorAdminMode) == L7_SUCCESS)
      SET_VALID(I_agentPortMirrorAdminMode, agentPortMirrorEntryData.valid);

    break;

  default:
    /* unknown nominator */
    return(NULL);
  }

  if (nominator != -1 && !VALID(nominator, agentPortMirrorEntryData.valid))
    return(NULL);
  return(&agentPortMirrorEntryData);
}

#ifdef SETS
int
k_agentPortMirrorEntry_test(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentPortMirrorEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                             doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentPortMirrorEntry_set_defaults(doList_t *dp)
{
    agentPortMirrorEntry_t *data = (agentPortMirrorEntry_t *) (dp->data);

    if ((data->agentPortMirrorSourcePortMask = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentPortMirrorEntry_set(agentPortMirrorEntry_t *data,
                           ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (VALID(I_agentPortMirrorDestinationPort, data->valid) &&
      snmpAgentPortMirrorProbePortSet(data->agentPortMirrorSessionNum,
                                    data->agentPortMirrorDestinationPort) != L7_SUCCESS)
  {
    memcpy(data->valid, tempValid, sizeof(data->valid));
    return COMMIT_FAILED_ERROR;
  }
  else
  {
    SET_VALID(I_agentMirroredPortIfIndex, tempValid);
  }

  if (VALID(I_agentPortMirrorSourcePortMask, data->valid) &&
      snmpAgentPortMirrorSourcePortMaskSet(data->agentPortMirrorSessionNum,
                                           data->agentPortMirrorSourcePortMask->octet_ptr,
                                           data->agentPortMirrorSourcePortMask->length) != L7_SUCCESS)
  {
    memcpy(data->valid, tempValid, sizeof(data->valid));
    return COMMIT_FAILED_ERROR;
  }
  else
  {
    SET_VALID(I_agentPortMirrorSourcePortMask, tempValid);
  }

  if (VALID(I_agentPortMirrorAdminMode, data->valid) &&
      snmpAgentPortMirrorAdminModeSet(data->agentPortMirrorSessionNum,
                                      data->agentPortMirrorAdminMode) != L7_SUCCESS)
  {
    memcpy(data->valid, tempValid, sizeof(data->valid));
    return COMMIT_FAILED_ERROR;
  }
  else
  {
    SET_VALID(I_agentPortMirrorAdminMode, tempValid);
  }

  return NO_ERROR;
}

#ifdef SR_agentPortMirrorEntry_UNDO
/* add #define SR_agentPortMirrorEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentPortMirrorEntry family.
 */
int
agentPortMirrorEntry_undo(doList_t *doHead, doList_t *doCur,
                          ContextInfo *contextInfo)
{
  agentPortMirrorEntry_t *data = (agentPortMirrorEntry_t *) doCur->data;
  agentPortMirrorEntry_t *undodata = (agentPortMirrorEntry_t *) doCur->undodata;
  agentPortMirrorEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentPortMirrorEntry_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
#endif /* SR_agentPortMirrorEntry_UNDO */

#endif /* SETS */

agentPortMirrorTypeEntry_t *
k_agentPortMirrorTypeEntry_get(int serialNum, ContextInfo *contextInfo,
                               int nominator,
                               int searchType,
                               SR_UINT32 agentPortMirrorSessionNum,
                               SR_UINT32 agentPortMirrorTypeSourcePort)
{
   static agentPortMirrorTypeEntry_t agentPortMirrorTypeEntryData;

   agentPortMirrorTypeEntryData.agentPortMirrorSessionNum = agentPortMirrorSessionNum;
   SET_VALID(I_agentPortMirrorTypeEntryIndex_agentPortMirrorSessionNum, agentPortMirrorTypeEntryData.valid);
   agentPortMirrorTypeEntryData.agentPortMirrorTypeSourcePort = agentPortMirrorTypeSourcePort;
   SET_VALID(I_agentPortMirrorTypeSourcePort,
             agentPortMirrorTypeEntryData.valid);

   if ((searchType == EXACT) ?
      (snmpAgentPortMirrorTypeEntryGet(agentPortMirrorTypeEntryData.agentPortMirrorSessionNum,
                                       agentPortMirrorTypeEntryData.agentPortMirrorTypeSourcePort) != L7_SUCCESS) :
      ((snmpAgentPortMirrorTypeEntryGet(agentPortMirrorTypeEntryData.agentPortMirrorSessionNum,
                                        agentPortMirrorTypeEntryData.agentPortMirrorTypeSourcePort) != L7_SUCCESS) &&
      (snmpAgentPortMirrorTypeEntryGetNext(&agentPortMirrorTypeEntryData.agentPortMirrorSessionNum,
                                           &agentPortMirrorTypeEntryData.agentPortMirrorTypeSourcePort) != L7_SUCCESS)))
   {
     return (NULL);
   }

   switch(nominator)
   {
     case -1:
     case I_agentPortMirrorTypeSourcePort:
       if(nominator != -1)
         break;
     case I_agentPortMirrorTypeType:
       if(snmpAgentPortMirrorTypeTypeGet(agentPortMirrorTypeEntryData.agentPortMirrorSessionNum,
                                         agentPortMirrorTypeEntryData.agentPortMirrorTypeSourcePort,
                                         &agentPortMirrorTypeEntryData.agentPortMirrorTypeType) == L7_SUCCESS)
       {
         SET_VALID(I_agentPortMirrorTypeType, agentPortMirrorTypeEntryData.valid);
       }
       break;
     default:
       /* unknown nominator */
       return(NULL);
   }

   if (nominator != -1 && !VALID(nominator, agentPortMirrorTypeEntryData.valid))
     return (NULL);
   return(&agentPortMirrorTypeEntryData);
}

#ifdef SETS
int
k_agentPortMirrorTypeEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentPortMirrorTypeEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                 doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentPortMirrorTypeEntry_set_defaults(doList_t *dp)
{
    agentPortMirrorTypeEntry_t *data = (agentPortMirrorTypeEntry_t *) (dp->data);
    data->agentPortMirrorTypeType = D_agentPortMirrorTypeType_txrx;
    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentPortMirrorTypeEntry_set(agentPortMirrorTypeEntry_t *data,
                               ContextInfo *contextInfo, int function)
{
   L7_char8 tempValid[sizeof(data->valid)];
   memset(tempValid, 0, sizeof(tempValid));

   if(VALID(I_agentPortMirrorTypeType, data->valid) &&
     (snmpAgentPortMirrorTypeTypeSet(data->agentPortMirrorSessionNum,
                                     data->agentPortMirrorTypeSourcePort,
                                     data->agentPortMirrorTypeType) != L7_SUCCESS))
   {
     memcpy(data->valid, tempValid, sizeof(data->valid));
     return COMMIT_FAILED_ERROR;
   }
   return NO_ERROR;
}

#ifdef SR_agentPortMirrorTypeEntry_UNDO
/* add #define SR_agentPortMirrorTypeEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentPortMirrorTypeEntry family.
 */
int
agentPortMirrorTypeEntry_undo(doList_t *doHead, doList_t *doCur,
                              ContextInfo *contextInfo)
{
   agentPortMirrorTypeEntry_t *data = (agentPortMirrorTypeEntry_t *) doCur->data;
   agentPortMirrorTypeEntry_t *undodata = (agentPortMirrorTypeEntry_t *) doCur->undodata;
   agentPortMirrorTypeEntry_t *setdata = NULL;
   L7_int32 function = SR_UNKNOWN;

   /*
    * Modifications for UNDO Feature
    * Setting valid bits of undodata same as that for data
    */
   if ( data == NULL || undodata == NULL )
     return UNDO_FAILED_ERROR;
   memcpy(undodata->valid,data->valid,sizeof(data->valid));

   /* undoing a modify, replace the original data */
   setdata = undodata;
   function = SR_ADD_MODIFY;

   /* use the set method for the undo */
  if ((setdata != NULL) &&
      (k_agentPortMirrorTypeEntry_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentPortMirrorTypeEntry_UNDO */

#endif /* SETS */

agentDot3adAggPortEntry_t *
k_agentDot3adAggPortEntry_get(int serialNum, ContextInfo *contextInfo,
                              int nominator,
                              int searchType,
                              SR_INT32 agentDot3adAggPort)
{
  static agentDot3adAggPortEntry_t agentDot3adAggPortEntryData;

  ZERO_VALID(agentDot3adAggPortEntryData.valid);
  agentDot3adAggPortEntryData.agentDot3adAggPort = agentDot3adAggPort;
  SET_VALID(I_agentDot3adAggPort, agentDot3adAggPortEntryData.valid);

  if ( ( searchType == EXACT ? ( usmDbPhysicalIntIfNumberCheck(USMDB_UNIT_CURRENT, agentDot3adAggPortEntryData.agentDot3adAggPort) != L7_SUCCESS ) :
         ( usmDbPhysicalIntIfNumberCheck(USMDB_UNIT_CURRENT, agentDot3adAggPortEntryData.agentDot3adAggPort) != L7_SUCCESS &&
           usmDbGetNextPhysicalIntIfNumber(agentDot3adAggPortEntryData.agentDot3adAggPort, &agentDot3adAggPortEntryData.agentDot3adAggPort) != L7_SUCCESS ) ) )
  {
    ZERO_VALID(agentDot3adAggPortEntryData.valid);
    return(NULL);
  }

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
  case I_agentDot3adAggPort:
    if (nominator != -1) break;
    /* else pass through */

  case I_agentDot3adAggPortLACPMode :
    if (snmpDot3adAggPortLacpModeGet(USMDB_UNIT_CURRENT,
                                     agentDot3adAggPortEntryData.agentDot3adAggPort,
                                     &agentDot3adAggPortEntryData.agentDot3adAggPortLACPMode) == L7_SUCCESS)
      SET_VALID(I_agentDot3adAggPortLACPMode, agentDot3adAggPortEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentDot3adAggPortEntryData.valid) )
    return(NULL);

  return(&agentDot3adAggPortEntryData);
}

#ifdef SETS
int
k_agentDot3adAggPortEntry_test(ObjectInfo *object, ObjectSyntax *value,
                               doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentDot3adAggPortEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentDot3adAggPortEntry_set_defaults(doList_t *dp)
{
  agentDot3adAggPortEntry_t *data = (agentDot3adAggPortEntry_t *) (dp->data);


  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_agentDot3adAggPortEntry_set(agentDot3adAggPortEntry_t *data,
                              ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (usmDbPhysicalIntIfNumberCheck(USMDB_UNIT_CURRENT, data->agentDot3adAggPort) != L7_SUCCESS)
  {
    ZERO_VALID(data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentDot3adAggPortLACPMode, data->valid))
  {
    if( snmpDot3adAggPortLacpModeSet(USMDB_UNIT_CURRENT,
                                   data->agentDot3adAggPort,
                                   data->agentDot3adAggPortLACPMode) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentDot3adAggPortLACPMode, tempValid);
    }
  }

  return NO_ERROR;
}

  #ifdef SR_agentDot3adAggPortEntry_UNDO
/* add #define SR_agentDot3adAggPortEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentDot3adAggPortEntry family.
 */
int
agentDot3adAggPortEntry_undo(doList_t *doHead, doList_t *doCur,
                             ContextInfo *contextInfo)
{
  agentDot3adAggPortEntry_t *data = (agentDot3adAggPortEntry_t *) doCur->data;
  agentDot3adAggPortEntry_t *undodata = (agentDot3adAggPortEntry_t *) doCur->undodata;
  agentDot3adAggPortEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentDot3adAggPortEntry_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentDot3adAggPortEntry_UNDO */

#endif /* SETS */


agentPortConfigEntry_t *
k_agentPortConfigEntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           SR_INT32 agentPortDot1dBasePort)
{
  static agentPortConfigEntry_t agentPortConfigEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
#if L7_FEAT_SF10GBT
  L7_uchar8 fwRevision[4];
  L7_uchar8 fwPartNumber[8];
#endif  

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;

    agentPortConfigEntryData.agentPortDefaultType = MakeOIDFromDot("0.0");
    agentPortConfigEntryData.agentPortType = MakeOIDFromDot("0.0");
    #ifdef I_agentPortTransceiverFwPartNumber
    agentPortConfigEntryData.agentPortTransceiverFwPartNumber = MakeOctetString(NULL,0);
    #endif
    #ifdef I_agentPortTransceiverFwRevision
    agentPortConfigEntryData.agentPortTransceiverFwRevision = MakeOctetString(NULL,0);
    #endif

  }

  ZERO_VALID(agentPortConfigEntryData.valid);
  agentPortConfigEntryData.agentPortDot1dBasePort = agentPortDot1dBasePort;
  SET_VALID(I_agentPortDot1dBasePort, agentPortConfigEntryData.valid);

  if ( ( searchType == EXACT ? ( usmDbPhysicalIntIfNumberCheck(USMDB_UNIT_CURRENT, agentPortConfigEntryData.agentPortDot1dBasePort) != L7_SUCCESS ) :
         ( usmDbPhysicalIntIfNumberCheck(USMDB_UNIT_CURRENT, agentPortConfigEntryData.agentPortDot1dBasePort) != L7_SUCCESS &&
           usmDbGetNextPhysicalIntIfNumber(agentPortConfigEntryData.agentPortDot1dBasePort, &agentPortConfigEntryData.agentPortDot1dBasePort) != L7_SUCCESS ) ) )
  {
    ZERO_VALID(agentPortConfigEntryData.valid);
    return(NULL);
  }

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */
  
  switch (nominator)
  {
  case -1:
  case I_agentPortDot1dBasePort:
    if (nominator != -1) break;
    /* else pass through */

  case I_agentPortIfIndex :
    if (usmDbExtIfNumFromIntIfNum(agentPortConfigEntryData.agentPortDot1dBasePort,
                                  &agentPortConfigEntryData.agentPortIfIndex) == L7_SUCCESS)
      SET_VALID(I_agentPortIfIndex, agentPortConfigEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentPortIanaType :
    if (snmpAgentPortIanaTypeGet(USMDB_UNIT_CURRENT, agentPortConfigEntryData.agentPortDot1dBasePort,
                                 &agentPortConfigEntryData.agentPortIanaType) == L7_SUCCESS)
      SET_VALID(I_agentPortIanaType, agentPortConfigEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentPortSTPMode:
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1S_COMPONENT_ID, L7_DOT1S_FEATURE_ID) == L7_FALSE)
    {
      /* legacy code is not present */
    }
    if (nominator != -1) break;
    /* else pass through */

  case I_agentPortSTPState:
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1S_COMPONENT_ID, L7_DOT1S_FEATURE_ID) == L7_FALSE)
    {
      /* legacy code is not present */
    }
    if (nominator != -1) break;
    /* else pass through */

  case I_agentPortAdminMode:
    if (snmpAgentPortAdminModeGet(USMDB_UNIT_CURRENT, agentPortConfigEntryData.agentPortDot1dBasePort,
                                  &agentPortConfigEntryData.agentPortAdminMode) == L7_SUCCESS)
      SET_VALID(I_agentPortAdminMode, agentPortConfigEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

#ifdef NOT_SUPPORTED
  case I_agentPortPhysicalMode:
    if (snmpAgentPortPhysicalModeGet(USMDB_UNIT_CURRENT, agentPortConfigEntryData.agentPortDot1dBasePort,
                                     &agentPortConfigEntryData.agentPortPhysicalMode) == L7_SUCCESS)
      SET_VALID(I_agentPortPhysicalMode, agentPortConfigEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */
#endif /* NOT_SUPPORTED */

#ifdef NOT_SUPPORTED
  case I_agentPortDuplexMode:
    if (snmpAgentPortDuplexModeGet(USMDB_UNIT_CURRENT, agentPortConfigEntryData.agentPortDot1dBasePort,
                                   &agentPortConfigEntryData.agentPortDuplexMode) == L7_SUCCESS)
      SET_VALID(I_agentPortDuplexMode, agentPortConfigEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */
#endif /* NOT_SUPPORTED */

#ifdef NOT_SUPPORTED
  case I_agentPortPhysicalStatus:
    if (snmpAgentPortPhysicalStatusGet(USMDB_UNIT_CURRENT, agentPortConfigEntryData.agentPortDot1dBasePort,
                                       &agentPortConfigEntryData.agentPortPhysicalStatus) == L7_SUCCESS)
      SET_VALID(I_agentPortPhysicalStatus, agentPortConfigEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */
#endif /* NOT_SUPPORTED */

  case I_agentPortLinkTrapMode:
    if (snmpAgentPortLinkTrapModeGet(USMDB_UNIT_CURRENT, agentPortConfigEntryData.agentPortDot1dBasePort,
                                     &agentPortConfigEntryData.agentPortLinkTrapMode) == L7_SUCCESS)
      SET_VALID(I_agentPortLinkTrapMode, agentPortConfigEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentPortClearStats:
    agentPortConfigEntryData.agentPortClearStats = D_agentPortClearStats_disable;
    SET_VALID(I_agentPortClearStats, agentPortConfigEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentPortDefaultType:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    FreeOID(agentPortConfigEntryData.agentPortDefaultType);
    if (snmpAgentPortDefaultTypeGet(USMDB_UNIT_CURRENT, agentPortConfigEntryData.agentPortDot1dBasePort,
                                    snmp_buffer) == L7_SUCCESS)
    {
      if ((agentPortConfigEntryData.agentPortDefaultType = MakeOIDFromDot(snmp_buffer)) != NULL)
        SET_VALID(I_agentPortDefaultType, agentPortConfigEntryData.valid);
    }
    else
    {
      agentPortConfigEntryData.agentPortDefaultType = MakeOIDFromDot("0.0");
    }
    if (nominator != -1) break;
    /* else pass through */

  case I_agentPortType:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    FreeOID(agentPortConfigEntryData.agentPortType);
    if (snmpAgentPortTypeGet(USMDB_UNIT_CURRENT, agentPortConfigEntryData.agentPortDot1dBasePort,
                             snmp_buffer) == L7_SUCCESS)
    {
      if ((agentPortConfigEntryData.agentPortType = MakeOIDFromDot(snmp_buffer)) != NULL)
        SET_VALID(I_agentPortType, agentPortConfigEntryData.valid);
    }
    else
    {
      agentPortConfigEntryData.agentPortType = MakeOIDFromDot("0.0");
    }
    if (nominator != -1) break;
    /* else pass through */

  case I_agentPortAutoNegAdminStatus :
    if (snmpAgentPortAutoNegAdminStatusGet(USMDB_UNIT_CURRENT, agentPortConfigEntryData.agentPortDot1dBasePort,
                                           &agentPortConfigEntryData.agentPortAutoNegAdminStatus) == L7_SUCCESS)
      SET_VALID(I_agentPortAutoNegAdminStatus, agentPortConfigEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentPortDot3FlowControlMode :
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID,
                                 L7_POLICY_PORT_FLOW_CONTROL_FEATURE_ID) == L7_TRUE)
    {
      if (snmpAgentPortDot3FlowControlModeGet(USMDB_UNIT_CURRENT, agentPortConfigEntryData.agentPortDot1dBasePort,
                                              &agentPortConfigEntryData.agentPortDot3FlowControlMode) == L7_SUCCESS)
        SET_VALID(I_agentPortDot3FlowControlMode, agentPortConfigEntryData.valid);
    }
    else
    {
      CLR_VALID(I_agentPortDot3FlowControlMode, agentPortConfigEntryData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */

#ifdef L7_DVLAN_PACKAGE
  case I_agentPortDVlanTagMode:
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_FEATURE_SUPPORTED) == L7_TRUE &&
        snmpAgentPortDVlanTagModeGet(USMDB_UNIT_CURRENT, agentPortConfigEntryData.agentPortDot1dBasePort,
                                     &agentPortConfigEntryData.agentPortDVlanTagMode) == L7_SUCCESS)
      SET_VALID(I_agentPortDVlanTagMode, agentPortConfigEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentPortDVlanTagEthertype:
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_ETHERTYPE_PER_INTERFACE_FEATURE_ID) == L7_TRUE &&
        usmDbDvlantagIntfEthertypeGet(USMDB_UNIT_CURRENT, agentPortConfigEntryData.agentPortDot1dBasePort,
                                   &agentPortConfigEntryData.agentPortDVlanTagEthertype, 0) == L7_SUCCESS)
      SET_VALID(I_agentPortDVlanTagEthertype, agentPortConfigEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentPortDVlanTagCustomerId:
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_CUSTID_FEATURE_ID) == L7_TRUE &&
        usmDbDvlantagIntfCustIdGet(USMDB_UNIT_CURRENT, agentPortConfigEntryData.agentPortDot1dBasePort,
                                   &agentPortConfigEntryData.agentPortDVlanTagCustomerId) == L7_SUCCESS)
      SET_VALID(I_agentPortDVlanTagCustomerId, agentPortConfigEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */
#endif

  case I_agentPortMaxFrameSizeLimit :
     if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_NIM_COMPONENT_ID, L7_NIM_JUMBOFRAMES_FEATURE_ID) == L7_TRUE &&
         snmpAgentPortMaxFrameSizeGet(agentPortConfigEntryData.agentPortDot1dBasePort,
                                    &agentPortConfigEntryData.agentPortMaxFrameSizeLimit) == L7_SUCCESS)
      SET_VALID(I_agentPortMaxFrameSizeLimit, agentPortConfigEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentPortMaxFrameSize :

     if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_NIM_COMPONENT_ID, L7_NIM_JUMBOFRAMES_FEATURE_ID) == L7_TRUE &&
          snmpAgentPortConfigMaxFrameSizeGet(agentPortConfigEntryData.agentPortDot1dBasePort,
                                    &agentPortConfigEntryData.agentPortMaxFrameSize) == L7_SUCCESS)
      SET_VALID(I_agentPortMaxFrameSize, agentPortConfigEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */
  case I_agentPortBroadcastControlMode :
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_BCAST_CONTROL_FEATURE_ID) == L7_TRUE)
    {
       if (snmpAgentPortBroadcastControlModeGet(agentPortConfigEntryData.agentPortDot1dBasePort,
                                               &agentPortConfigEntryData.agentPortBroadcastControlMode) == L7_SUCCESS)
         SET_VALID(I_agentPortBroadcastControlMode, agentPortConfigEntryData.valid);
       else
       {
         CLR_VALID(I_agentPortBroadcastControlMode, agentPortConfigEntryData.valid);
       }
    }
    if (nominator != -1) break;
    /* else pass through */
  case I_agentPortMulticastControlMode :

    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_MCAST_CONTROL_FEATURE_ID) == L7_TRUE)
    {
       if (snmpAgentPortMulticastControlModeGet(agentPortConfigEntryData.agentPortDot1dBasePort,
                                               &agentPortConfigEntryData.agentPortMulticastControlMode) == L7_SUCCESS)
         SET_VALID(I_agentPortMulticastControlMode, agentPortConfigEntryData.valid);
       else
       {
         CLR_VALID(I_agentPortMulticastControlMode, agentPortConfigEntryData.valid);
       }
    }
    if (nominator != -1) break;
    /* else pass through */
  case I_agentPortUnicastControlMode :

    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_UCAST_CONTROL_FEATURE_ID) == L7_TRUE)
    {
       if (snmpAgentPortUnicastControlModeGet(agentPortConfigEntryData.agentPortDot1dBasePort,
                                               &agentPortConfigEntryData.agentPortUnicastControlMode) == L7_SUCCESS)
         SET_VALID(I_agentPortUnicastControlMode, agentPortConfigEntryData.valid);
       else
       {
         CLR_VALID(I_agentPortUnicastControlMode, agentPortConfigEntryData.valid);
       }
    }
    if (nominator != -1) break;
    /* else pass through */

  case I_agentPortBroadcastControlThreshold :
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_BCAST_CONTROL_FEATURE_ID) == L7_TRUE)
    {
       if (snmpAgentPortBroadcastControlThresholdGet(agentPortConfigEntryData.agentPortDot1dBasePort,
                                               &agentPortConfigEntryData.agentPortBroadcastControlThreshold) == L7_SUCCESS)
         SET_VALID(I_agentPortBroadcastControlThreshold, agentPortConfigEntryData.valid);
       else
       {
         CLR_VALID(I_agentPortBroadcastControlThreshold, agentPortConfigEntryData.valid);
       }
    }
    if (nominator != -1) break;
    /* else pass through */
  case I_agentPortMulticastControlThreshold :
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_MCAST_CONTROL_FEATURE_ID) == L7_TRUE)
    {
       if (snmpAgentPortMulticastControlThresholdGet(agentPortConfigEntryData.agentPortDot1dBasePort,
                                               &agentPortConfigEntryData.agentPortMulticastControlThreshold) == L7_SUCCESS)
         SET_VALID(I_agentPortMulticastControlThreshold, agentPortConfigEntryData.valid);
       else
       {
         CLR_VALID(I_agentPortMulticastControlThreshold, agentPortConfigEntryData.valid);
       }
    }
    if (nominator != -1) break;
    /* else pass through */
  case I_agentPortUnicastControlThreshold :
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_UCAST_CONTROL_FEATURE_ID) == L7_TRUE)
    {
       if (snmpAgentPortUnicastControlThresholdGet(agentPortConfigEntryData.agentPortDot1dBasePort,
                                               &agentPortConfigEntryData.agentPortUnicastControlThreshold) == L7_SUCCESS)
         SET_VALID(I_agentPortUnicastControlThreshold, agentPortConfigEntryData.valid);
       else
       {
         CLR_VALID(I_agentPortUnicastControlThreshold, agentPortConfigEntryData.valid);
       }
    }
    if (nominator != -1) break;
    /* else pass through */
  case I_agentPortBroadcastControlThresholdUnit :  
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_BCAST_CONTROL_FEATURE_ID) == L7_TRUE)
    {                      
       if (snmpAgentPortBroadcastControlThresholdUnitGet(agentPortConfigEntryData.agentPortDot1dBasePort,
                                               &agentPortConfigEntryData.agentPortBroadcastControlThresholdUnit) == L7_SUCCESS)
         SET_VALID(I_agentPortBroadcastControlThresholdUnit, agentPortConfigEntryData.valid);
       else
       {
         CLR_VALID(I_agentPortBroadcastControlThresholdUnit, agentPortConfigEntryData.valid);
       }
    }
    if (nominator != -1) break;
    /* else pass through */
  case I_agentPortMulticastControlThresholdUnit :  
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_MCAST_CONTROL_FEATURE_ID) == L7_TRUE)
    {                      
       if (snmpAgentPortMulticastControlThresholdUnitGet(agentPortConfigEntryData.agentPortDot1dBasePort,
                                               &agentPortConfigEntryData.agentPortMulticastControlThresholdUnit) == L7_SUCCESS)
         SET_VALID(I_agentPortMulticastControlThresholdUnit, agentPortConfigEntryData.valid);
       else
       {
         CLR_VALID(I_agentPortMulticastControlThresholdUnit, agentPortConfigEntryData.valid);
       }
    }
    if (nominator != -1) break;
    /* else pass through */
  case I_agentPortUnicastControlThresholdUnit :  
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID, L7_POLICY_PORT_UCAST_CONTROL_FEATURE_ID) == L7_TRUE)
    {                      
       if (snmpAgentPortUnicastControlThresholdUnitGet(agentPortConfigEntryData.agentPortDot1dBasePort,
                                               &agentPortConfigEntryData.agentPortUnicastControlThresholdUnit) == L7_SUCCESS)
         SET_VALID(I_agentPortUnicastControlThresholdUnit, agentPortConfigEntryData.valid);
       else
       {
         CLR_VALID(I_agentPortUnicastControlThresholdUnit, agentPortConfigEntryData.valid);
       }
    }
    if (nominator != -1) break;
    /* else pass through */

#ifdef I_agentPortSwitchportMode
  case I_agentPortSwitchportMode :
    if (snmpAgentPortSwitchportModeGet(USMDB_UNIT_CURRENT,agentPortConfigEntryData.agentPortDot1dBasePort,
                                       &agentPortConfigEntryData.agentPortSwitchportMode) == L7_SUCCESS)
      SET_VALID(I_agentPortSwitchportMode, agentPortConfigEntryData.valid);
    else
    {
      CLR_VALID(I_agentPortSwitchportMode, agentPortConfigEntryData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */
#endif /* I_agentPortSwitchportMode */

/*#ifdef I_agentPortVoiceVlanMode*/
  case I_agentPortVoiceVlanMode :
    if (snmpAgentPortVoiceVlanModeGet(USMDB_UNIT_CURRENT, agentPortConfigEntryData.agentPortDot1dBasePort,
                                       &agentPortConfigEntryData.agentPortVoiceVlanMode) == L7_SUCCESS)
      SET_VALID(I_agentPortVoiceVlanMode, agentPortConfigEntryData.valid);
    else
    {
      CLR_VALID(I_agentPortVoiceVlanMode, agentPortConfigEntryData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */
/*#endif*/ /* I_agentPortVoiceVlanMode */

/*#ifdef I_agentPortVoiceVlanID*/
  case I_agentPortVoiceVlanID :
    if (snmpAgentPortVoiceVlanIDGet(USMDB_UNIT_CURRENT, agentPortConfigEntryData.agentPortDot1dBasePort,
                                           &agentPortConfigEntryData.agentPortVoiceVlanID) == L7_SUCCESS)
      SET_VALID(I_agentPortVoiceVlanID, agentPortConfigEntryData.valid);
    else
    {
      CLR_VALID(I_agentPortVoiceVlanID, agentPortConfigEntryData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */
/*#endif *//* I_agentPortVoiceVlanID */

/*#ifdef I_agentPortVoiceVlanPriority*/
  case I_agentPortVoiceVlanPriority :
    if (snmpAgentPortVoiceVlanPriorityGet(USMDB_UNIT_CURRENT, agentPortConfigEntryData.agentPortDot1dBasePort,
                                           &agentPortConfigEntryData.agentPortVoiceVlanPriority) == L7_SUCCESS)
      SET_VALID(I_agentPortVoiceVlanPriority, agentPortConfigEntryData.valid);
    else
    {
      CLR_VALID(I_agentPortVoiceVlanPriority, agentPortConfigEntryData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */
/*#endif*/ /* I_agentPortVoiceVlanPriority */

/*#ifdef I_agentPortVoiceVlanDataPriorityMode*/
  case I_agentPortVoiceVlanDataPriorityMode :
    if (snmpAgentPortVoiceVlanDataPriorityModeGet(USMDB_UNIT_CURRENT, agentPortConfigEntryData.agentPortDot1dBasePort,
                                       &agentPortConfigEntryData.agentPortVoiceVlanDataPriorityMode) == L7_SUCCESS)
      SET_VALID(I_agentPortVoiceVlanDataPriorityMode, agentPortConfigEntryData.valid);
    else
    {
      CLR_VALID(I_agentPortVoiceVlanDataPriorityMode, agentPortConfigEntryData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */
/*#endif*/ /* I_agentPortVoiceVlanDataPriorityMode */

/*#ifdef I_agentPortVoiceVlanUntagged*/
  case I_agentPortVoiceVlanUntagged :
    if (snmpAgentPortVoiceVlanUntaggedGet(USMDB_UNIT_CURRENT, agentPortConfigEntryData.agentPortDot1dBasePort,
                                       &agentPortConfigEntryData.agentPortVoiceVlanUntagged) == L7_SUCCESS)
      SET_VALID(I_agentPortVoiceVlanUntagged, agentPortConfigEntryData.valid);
    else
    {
      CLR_VALID(I_agentPortVoiceVlanUntagged, agentPortConfigEntryData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */
/*#endif*/ /* I_agentPortVoiceVlanUntagged */

/*#ifdef I_agentPortVoiceVlanNoneMode*/
  case I_agentPortVoiceVlanNoneMode :
    if (snmpAgentPortVoiceVlanNoneModeGet(USMDB_UNIT_CURRENT, agentPortConfigEntryData.agentPortDot1dBasePort,
                                       &agentPortConfigEntryData.agentPortVoiceVlanNoneMode) == L7_SUCCESS)
      SET_VALID(I_agentPortVoiceVlanNoneMode, agentPortConfigEntryData.valid);
    else
    {
      CLR_VALID(I_agentPortVoiceVlanNoneMode, agentPortConfigEntryData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */
/*#endif*/ /* I_agentPortVoiceVlanNoneMode*/

/*#ifdef I_agentPortVoiceVlanDSCP*/
  case I_agentPortVoiceVlanDSCP :
    if (snmpAgentPortVoiceVlanDSCPGet(USMDB_UNIT_CURRENT, agentPortConfigEntryData.agentPortDot1dBasePort,
                                       &agentPortConfigEntryData.agentPortVoiceVlanDSCP) == L7_SUCCESS)
      SET_VALID(I_agentPortVoiceVlanDSCP, agentPortConfigEntryData.valid);
    else
    {
      CLR_VALID(I_agentPortVoiceVlanDSCP, agentPortConfigEntryData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */
/*#endif*/ /* I_agentPortVoiceVlanDSCP */

/*#ifdef I_agentPortVoiceVlanAuthMode*/
  case I_agentPortVoiceVlanAuthMode :
    if (snmpAgentPortVoiceVlanAuthModeGet(USMDB_UNIT_CURRENT, agentPortConfigEntryData.agentPortDot1dBasePort,
                                       &agentPortConfigEntryData.agentPortVoiceVlanAuthMode) == L7_SUCCESS)
      SET_VALID(I_agentPortVoiceVlanAuthMode, agentPortConfigEntryData.valid);
    else
    {
      CLR_VALID(I_agentPortVoiceVlanAuthMode, agentPortConfigEntryData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */
/*#endif*/ /* I_agentPortVoiceVlanAuthMode */

/*#ifdef I_agentPortVoiceVlanOperationalStatus*/
  case I_agentPortVoiceVlanOperationalStatus :
    if (snmpAgentPortVoiceVlanOperationalStatusGet(USMDB_UNIT_CURRENT, agentPortConfigEntryData.agentPortDot1dBasePort,
                                       &agentPortConfigEntryData.agentPortVoiceVlanOperationalStatus) == L7_SUCCESS)
      SET_VALID(I_agentPortVoiceVlanOperationalStatus, agentPortConfigEntryData.valid);
    else
    {
      CLR_VALID(I_agentPortVoiceVlanOperationalStatus, agentPortConfigEntryData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */
/*#endif*/ /* I_agentPortVoiceVlanOperationalStatus */

#ifdef I_agentPortDot3FlowControlOperStatus
  case I_agentPortDot3FlowControlOperStatus :
    if (snmpAgentPortDot3FlowControlOperStatusGet(USMDB_UNIT_CURRENT, agentPortConfigEntryData.agentPortDot1dBasePort,
                                                  &agentPortConfigEntryData.agentPortDot3FlowControlOperStatus) == L7_SUCCESS)
        SET_VALID(I_agentPortDot3FlowControlOperStatus, agentPortConfigEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */
#endif

#if L7_FEAT_SF10GBT
  case I_agentPortTransceiverFwPartNumber :
    if ( (snmpAgentPortGetFirmwareRevision(agentPortConfigEntryData.agentPortDot1dBasePort,
            fwPartNumber, fwRevision) == L7_SUCCESS) 
         && (SafeMakeOctetString(&agentPortConfigEntryData.agentPortTransceiverFwPartNumber, 
                                 fwPartNumber, 8) == L7_TRUE))
    {
      SET_VALID(I_agentPortTransceiverFwPartNumber, agentPortConfigEntryData.valid);
    } else
    {
      CLR_VALID(I_agentPortTransceiverFwPartNumber, agentPortConfigEntryData.valid);
    }
    if ( nominator != -1 ) break;
    
  case I_agentPortTransceiverFwRevision :
    if ( (snmpAgentPortGetFirmwareRevision(
          agentPortConfigEntryData.agentPortDot1dBasePort,
            fwPartNumber, fwRevision) == L7_SUCCESS) 
         && (SafeMakeOctetString(&agentPortConfigEntryData.agentPortTransceiverFwRevision, 
                                 fwRevision, 4) == L7_TRUE))
    {
      SET_VALID(I_agentPortTransceiverFwRevision, agentPortConfigEntryData.valid);
    } else
    {
      CLR_VALID(I_agentPortTransceiverFwRevision, agentPortConfigEntryData.valid);
    }
    if ( nominator != -1 ) break;
#endif /* L7_FEAT_SF10GBT */
    break;
    
  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentPortConfigEntryData.valid) )
    return(NULL);

  return(&agentPortConfigEntryData);
}

#ifdef SETS
int
k_agentPortConfigEntry_test(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *dp, ContextInfo *contextInfo)
{

  ZERO_VALID(((agentPortConfigEntry_t *) (dp->data))->valid);
  return NO_ERROR;
}

int
k_agentPortConfigEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                             doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentPortConfigEntry_set_defaults(doList_t *dp)
{
  agentPortConfigEntry_t *data = (agentPortConfigEntry_t *) (dp->data);

  if ((data->agentPortDefaultType = MakeOIDFromDot("0.0")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }

  if ((data->agentPortType = MakeOIDFromDot("0.0")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }

  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_agentPortConfigEntry_set(agentPortConfigEntry_t *data,
                           ContextInfo *contextInfo, int function)
{
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];

  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (usmDbPhysicalIntIfNumberCheck(USMDB_UNIT_CURRENT, data->agentPortDot1dBasePort) != L7_SUCCESS)
  {
    ZERO_VALID(data->valid);
    return COMMIT_FAILED_ERROR;
  }

  data->agentPortDot1dBasePort = data->agentPortDot1dBasePort;

  if (VALID(I_agentPortSTPMode, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1S_COMPONENT_ID, L7_DOT1S_FEATURE_ID) != L7_FALSE))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentPortSTPMode, tempValid);
    }
  }

  if (VALID(I_agentPortAdminMode, data->valid))
  {
    if( snmpAgentPortAdminModeSet(USMDB_UNIT_CURRENT, data->agentPortDot1dBasePort,
                                data->agentPortAdminMode) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentPortAdminMode, tempValid);
    }
  }

#ifdef NOT_SUPPORTED
  if (VALID(I_agentPortPhysicalMode, data->valid))
  {
    if( snmpAgentPortPhysicalModeSet(USMDB_UNIT_CURRENT, data->agentPortDot1dBasePort,
                                   data->agentPortPhysicalMode) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentPortPhysicalMode, tempValid);
    }
  }
#endif /* NOT_SUPPORTED */


#ifdef NOT_SUPPORTED
  if (VALID(I_agentPortDuplexMode, data->valid))
  {
    if( snmpAgentPortDuplexModeSet(USMDB_UNIT_CURRENT, data->agentPortDot1dBasePort,
                                 data->agentPortDuplexMode) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentPortDuplexMode, tempValid);
    }
  }
#endif /* NOT_SUPPORTED */

  if (VALID(I_agentPortLinkTrapMode, data->valid))
  {
    if( snmpAgentPortLinkTrapModeSet(USMDB_UNIT_CURRENT, data->agentPortDot1dBasePort,
                                   data->agentPortLinkTrapMode) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentPortLinkTrapMode, tempValid);
    }
  }

  if ( VALID(I_agentPortClearStats, data->valid))
  {
    if( snmpAgentPortClearStatsSet(USMDB_UNIT_CURRENT, data->agentPortDot1dBasePort,
                                  data->agentPortClearStats) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentPortClearStats, tempValid);
    }
  }

  if ( VALID(I_agentPortDefaultType, data->valid) )
  {
    if((MakeDotFromOID(data->agentPortDefaultType, snmp_buffer) != 0) ||
         (snmpAgentPortDefaultTypeSet(USMDB_UNIT_CURRENT, data->agentPortDot1dBasePort, snmp_buffer) != L7_SUCCESS) )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentPortDefaultType, tempValid);
    }
  }

  if ( VALID(I_agentPortAutoNegAdminStatus, data->valid))
  {
    if( snmpAgentPortAutoNegAdminStatusSet(USMDB_UNIT_CURRENT, data->agentPortDot1dBasePort,
                                          data->agentPortAutoNegAdminStatus) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentPortAutoNegAdminStatus, tempValid);
    }
  }

  if ( VALID(I_agentPortDot3FlowControlMode, data->valid))
  {
    if(((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID,
                                 L7_POLICY_PORT_FLOW_CONTROL_FEATURE_ID) == L7_FALSE) ||
       snmpAgentPortDot3FlowControlModeSet(USMDB_UNIT_CURRENT, data->agentPortDot1dBasePort,
                                           data->agentPortDot3FlowControlMode) != L7_SUCCESS ))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentPortDot3FlowControlMode, tempValid);
    }
  }

#ifdef L7_DVLAN_PACKAGE
  if (VALID(I_agentPortDVlanTagMode, data->valid))
  {
    if((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_FEATURE_SUPPORTED) == L7_FALSE ||
      snmpAgentPortDVlanTagModeSet(USMDB_UNIT_CURRENT, data->agentPortDot1dBasePort,
                                    data->agentPortDVlanTagMode) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentPortDVlanTagMode, tempValid);
    }
  }

  if (VALID(I_agentPortDVlanTagEthertype, data->valid))
  {
    if((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_ETHERTYPE_PER_INTERFACE_FEATURE_ID) == L7_FALSE ||
        usmDbDvlantagIntfEthertypeSet(USMDB_UNIT_CURRENT, data->agentPortDot1dBasePort,
                                      data->agentPortDVlanTagEthertype, L7_TRUE) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentPortDVlanTagEthertype, tempValid);
    }
  }

  if (VALID(I_agentPortDVlanTagCustomerId, data->valid))
  {
    if((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_CUSTID_FEATURE_ID) == L7_FALSE ||
      usmDbDvlantagIntfCustIdSet(USMDB_UNIT_CURRENT, data->agentPortDot1dBasePort,
                                  data->agentPortDVlanTagCustomerId) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentPortDVlanTagCustomerId, tempValid);
    }
  }
#endif

  if ( VALID(I_agentPortMaxFrameSize, data->valid))
  {

    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_NIM_COMPONENT_ID, L7_NIM_JUMBOFRAMES_FEATURE_ID) != L7_TRUE ||
          snmpAgentPortConfigMaxFrameSizeSet(data->agentPortDot1dBasePort,
                                             data->agentPortMaxFrameSize) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

  if ( VALID(I_agentPortBroadcastControlMode, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID,
                                 L7_POLICY_PORT_BCAST_CONTROL_FEATURE_ID) == L7_FALSE ||
        snmpAgentPortBroadcastControlModeSet(data->agentPortDot1dBasePort,
                                                 data->agentPortBroadcastControlMode) != L7_SUCCESS))
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return(COMMIT_FAILED_ERROR);
    }
    else
    {
       SET_VALID(I_agentPortBroadcastControlMode, tempValid);
    }
  }
  if ( VALID(I_agentPortMulticastControlMode, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID,
                                 L7_POLICY_PORT_MCAST_CONTROL_FEATURE_ID) == L7_FALSE ||
        snmpAgentPortMulticastControlModeSet(data->agentPortDot1dBasePort,
                                                 data->agentPortMulticastControlMode) != L7_SUCCESS))
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return(COMMIT_FAILED_ERROR);
    }
    else
    {
       SET_VALID(I_agentPortMulticastControlMode, tempValid);
    }
  }
  if ( VALID(I_agentPortUnicastControlMode, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID,
                                 L7_POLICY_PORT_UCAST_CONTROL_FEATURE_ID) == L7_FALSE ||
        snmpAgentPortUnicastControlModeSet(data->agentPortDot1dBasePort,
                                                 data->agentPortUnicastControlMode) != L7_SUCCESS))
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return(COMMIT_FAILED_ERROR);
    }
    else
    {
       SET_VALID(I_agentPortUnicastControlMode, tempValid);
    }
  }


  if ( VALID(I_agentPortBroadcastControlThreshold, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID,
                                 L7_POLICY_PORT_BCAST_CONTROL_FEATURE_ID) == L7_FALSE ||
        snmpAgentPortBroadcastControlThresholdSet(data->agentPortDot1dBasePort,
                                                 data->agentPortBroadcastControlThreshold) != L7_SUCCESS))
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return(COMMIT_FAILED_ERROR);
    }
    else
    {
       SET_VALID(I_agentPortBroadcastControlThreshold, tempValid);
    }
  }
  if ( VALID(I_agentPortMulticastControlThreshold, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID,
                                 L7_POLICY_PORT_MCAST_CONTROL_FEATURE_ID) == L7_FALSE ||
        snmpAgentPortMulticastControlThresholdSet(data->agentPortDot1dBasePort,
                                                 data->agentPortMulticastControlThreshold) != L7_SUCCESS))
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return(COMMIT_FAILED_ERROR);
    }
    else
    {
       SET_VALID(I_agentPortMulticastControlThreshold, tempValid);
    }
  }
  if ( VALID(I_agentPortUnicastControlThreshold, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID,
                                 L7_POLICY_PORT_UCAST_CONTROL_FEATURE_ID) == L7_FALSE ||
        snmpAgentPortUnicastControlThresholdSet(data->agentPortDot1dBasePort,
                                                 data->agentPortUnicastControlThreshold) != L7_SUCCESS))
    {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
    }
    else
    {
       SET_VALID(I_agentPortUnicastControlThreshold, tempValid);
    }
  }

  if ( VALID(I_agentPortBroadcastControlThresholdUnit, data->valid)) 
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID, 
                                 L7_POLICY_PORT_BCAST_CONTROL_FEATURE_ID) == L7_FALSE ||
        snmpAgentPortBroadcastControlThresholdUnitSet(data->agentPortDot1dBasePort, 
                                                 data->agentPortBroadcastControlThresholdUnit) != L7_SUCCESS))
    {                      
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
    }
    else
    {
       SET_VALID(I_agentPortBroadcastControlThresholdUnit, tempValid);
    }
  }
  if ( VALID(I_agentPortMulticastControlThresholdUnit, data->valid)) 
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID, 
                                 L7_POLICY_PORT_MCAST_CONTROL_FEATURE_ID) == L7_FALSE ||
        snmpAgentPortMulticastControlThresholdUnitSet(data->agentPortDot1dBasePort, 
                                                 data->agentPortMulticastControlThresholdUnit) != L7_SUCCESS))
    {                      
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
    }
    else
    {
       SET_VALID(I_agentPortMulticastControlThresholdUnit, tempValid);
    }
  }
  if ( VALID(I_agentPortUnicastControlThresholdUnit, data->valid)) 
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_POLICY_COMPONENT_ID, 
                                 L7_POLICY_PORT_UCAST_CONTROL_FEATURE_ID) == L7_FALSE ||
        snmpAgentPortUnicastControlThresholdUnitSet(data->agentPortDot1dBasePort, 
                                                 data->agentPortUnicastControlThresholdUnit) != L7_SUCCESS))
    {                      
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
    }
    else
    {
       SET_VALID(I_agentPortUnicastControlThresholdUnit, tempValid);
    }
  }

#ifdef I_agentPortSwitchportMode
   if (VALID(I_agentPortSwitchportMode,data->valid))
   {
      if (snmpAgentPortSwitchportModeSet(USMDB_UNIT_CURRENT,data->agentPortDot1dBasePort,
                                         data->agentPortSwitchportMode) == L7_SUCCESS)
      {
          SET_VALID(I_agentPortSwitchportMode, tempValid);
      }
      else
      {
        memcpy (data->valid,tempValid,sizeof(data->valid));
        /*CLR_VALID(I_agentPortSwitchportMode, agentPortConfigEntryData.valid);*/
        return(COMMIT_FAILED_ERROR);
      }
   }
#endif /* I_agentPortSwitchportMode */

/*#ifdef I_agentPortVoiceVlanMode*/
   if (VALID(I_agentPortVoiceVlanMode,data->valid))
   {
      if (snmpAgentPortVoiceVlanModeSet(USMDB_UNIT_CURRENT,data->agentPortDot1dBasePort,
                                         data->agentPortVoiceVlanMode) == L7_SUCCESS)
      {
          SET_VALID(I_agentPortVoiceVlanMode, tempValid);
      }
      else
      {
        memcpy (data->valid,tempValid,sizeof(data->valid));
        /*CLR_VALID(I_agentPortVoiceVlanMode, agentPortConfigEntryData.valid);*/
        return(COMMIT_FAILED_ERROR);
      }
   }
/*#endif*/ /* I_agentPortVoiceVlanMode */

  
/*#ifdef I_agentPortVoiceVlanID*/
   if (VALID(I_agentPortVoiceVlanID,data->valid))
   {
      if (snmpAgentPortVoiceVlanIDSet(USMDB_UNIT_CURRENT, data->agentPortDot1dBasePort,
                                             data->agentPortVoiceVlanID) == L7_SUCCESS)
      {
          SET_VALID(I_agentPortVoiceVlanID, tempValid);
      }
      else
      {
        memcpy (data->valid,tempValid,sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
   }
   
/*#endif*/ /* I_agentPortVoiceVlanID */

/*#ifdef I_agentPortVoiceVlanPriority*/
  if (VALID(I_agentPortVoiceVlanPriority,data->valid))
  {
      if (snmpAgentPortVoiceVlanPrioritySet(USMDB_UNIT_CURRENT, data->agentPortDot1dBasePort,
                                             data->agentPortVoiceVlanPriority) == L7_SUCCESS)
      {
          SET_VALID(I_agentPortVoiceVlanPriority, data->valid);
      }
      else
      {
         memcpy (data->valid,tempValid,sizeof(data->valid));
         return(COMMIT_FAILED_ERROR);
        /*CLR_VALID(I_agentPortVoiceVlanPriority, agentPortConfigEntryData.valid);*/
      }
  }
/*#endif*/
/*#ifdef I_agentPortVoiceVlanDataPriorityMode*/
  if (VALID(I_agentPortVoiceVlanDataPriorityMode,data->valid))
  {
      if (snmpAgentPortVoiceVlanDataPriorityModeSet(USMDB_UNIT_CURRENT, data->agentPortDot1dBasePort,
                                         data->agentPortVoiceVlanDataPriorityMode) == L7_SUCCESS)
      {
          SET_VALID(I_agentPortVoiceVlanDataPriorityMode, data->valid);
      }
      else
      {
         memcpy (data->valid,tempValid,sizeof(data->valid));
         return(COMMIT_FAILED_ERROR);
        /*CLR_VALID(I_agentPortVoiceVlanDataPriorityMode, agentPortConfigEntryData.valid);*/
      }
  }
   
/*#endif*/ /* I_agentPortVoiceVlanDataPriorityMode */

/*#ifdef I_agentPortVoiceVlanUntagged*/
  if (VALID(I_agentPortVoiceVlanUntagged,data->valid))
  {
      if (snmpAgentPortVoiceVlanUntaggedSet(USMDB_UNIT_CURRENT, data->agentPortDot1dBasePort,
                                         data->agentPortVoiceVlanUntagged) == L7_SUCCESS)
      {
          SET_VALID(I_agentPortVoiceVlanUntagged, data->valid);
      }
      else
      {
         memcpy (data->valid,tempValid,sizeof(data->valid));
         return(COMMIT_FAILED_ERROR);
        
      }
  }
   
/*#endif*/ /* I_agentPortVoiceVlanUntagged */

/*#ifdef I_agentPortVoiceVlanNoneMode*/
  if (VALID(I_agentPortVoiceVlanNoneMode,data->valid))
  {
      if (snmpAgentPortVoiceVlanNoneModeSet(USMDB_UNIT_CURRENT, data->agentPortDot1dBasePort,
                                         data->agentPortVoiceVlanNoneMode) == L7_SUCCESS)
      {
          SET_VALID(I_agentPortVoiceVlanNoneMode, data->valid);
      }
      else
      {
         memcpy (data->valid,tempValid,sizeof(data->valid));
         return(COMMIT_FAILED_ERROR);
        
      }
  }
   
/*#endif*/ /* I_agentPortVoiceVlanNoneMode */

/*#ifdef I_agentPortVoiceVlanDSCP*/
  if (VALID(I_agentPortVoiceVlanDSCP,data->valid))
  {
      if (snmpAgentPortVoiceVlanDSCPSet(USMDB_UNIT_CURRENT, data->agentPortDot1dBasePort,
                                         data->agentPortVoiceVlanDSCP) == L7_SUCCESS)
      {
          SET_VALID(I_agentPortVoiceVlanDSCP, data->valid);
      }
      else
      {
         memcpy (data->valid,tempValid,sizeof(data->valid));
         return(COMMIT_FAILED_ERROR);
        /*CLR_VALID(I_agentPortVoiceVlanDSCP, agentPortConfigEntryData.valid);*/
      }
  }
   
/*#endif*/ /* I_agentPortVoiceVlanDSCP */

/*#ifdef I_agentPortVoiceVlanAuthMode*/
  if (VALID(I_agentPortVoiceVlanAuthMode,data->valid))
  {
      if (snmpAgentPortVoiceVlanAuthModeSet(USMDB_UNIT_CURRENT, data->agentPortDot1dBasePort,
                                         data->agentPortVoiceVlanAuthMode) == L7_SUCCESS)
      {
          SET_VALID(I_agentPortVoiceVlanAuthMode, data->valid);
      }
      else
      {
         memcpy (data->valid,tempValid,sizeof(data->valid));
         return(COMMIT_FAILED_ERROR);
        
      }
  }
   
/*#endif*/ /* I_agentPortVoiceVlanAuthMode */
  return NO_ERROR;
}

  #ifdef SR_agentPortConfigEntry_UNDO
/* add #define SR_agentPortConfigEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentPortConfigEntry family.
 */
int
agentPortConfigEntry_undo(doList_t *doHead, doList_t *doCur,
                          ContextInfo *contextInfo)
{
  agentPortConfigEntry_t *data = (agentPortConfigEntry_t *) doCur->data;
  agentPortConfigEntry_t *undodata = (agentPortConfigEntry_t *) doCur->undodata;
  agentPortConfigEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentPortConfigEntry_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentPortConfigEntry_UNDO */

#endif /* SETS */


agentAuthenticationGroup_t *
k_agentAuthenticationGroup_get(int serialNum, ContextInfo *contextInfo,
                               int nominator)
{
  static agentAuthenticationGroup_t agentAuthenticationGroupData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  if (firstTime == L7_TRUE)
  {
    agentAuthenticationGroupData.agentAuthenticationListCreate = MakeOctetString(NULL, 0);
    agentAuthenticationGroupData.agentUserConfigDefaultAuthenticationList = MakeOctetString(NULL, 0);
    firstTime = L7_FALSE;
  }

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentAuthenticationListCreate:
    /* always valid, return empty string */
    SET_VALID(nominator, agentAuthenticationGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentUserConfigDefaultAuthenticationList:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbAPLNonConfiguredUserGet(USMDB_UNIT_CURRENT, L7_USER_MGR_COMPONENT_ID,
                                     snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromTextExact(&agentAuthenticationGroupData.agentUserConfigDefaultAuthenticationList,
                                         snmp_buffer) == L7_TRUE)
      SET_VALID(nominator, agentAuthenticationGroupData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentAuthenticationGroupData.valid))
    return(NULL);

  return(&agentAuthenticationGroupData);
}

#ifdef SETS
int
k_agentAuthenticationGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentAuthenticationGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                                 doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentAuthenticationGroup_set(agentAuthenticationGroup_t *data,
                               ContextInfo *contextInfo, int function)
{
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (VALID(I_agentAuthenticationListCreate, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentAuthenticationListCreate->octet_ptr,
           data->agentAuthenticationListCreate->length);
    if (usmDbAPLCreate(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, snmp_buffer) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      creationErrorAuthenticationGroup = L7_TRUE;
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentAuthenticationListCreate, tempValid);
    }
  }

  if (VALID(I_agentUserConfigDefaultAuthenticationList, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentUserConfigDefaultAuthenticationList->octet_ptr,
           data->agentUserConfigDefaultAuthenticationList->length);
    if (usmDbAPLNonConfiguredUserSet(USMDB_UNIT_CURRENT, L7_USER_MGR_COMPONENT_ID,
                                     snmp_buffer) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }
  return NO_ERROR;
}

#ifdef SR_agentAuthenticationGroup_UNDO
/* add #define SR_agentAuthenticationGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentAuthenticationGroup family.
 */
int
agentAuthenticationGroup_undo(doList_t *doHead, doList_t *doCur,
                              ContextInfo *contextInfo)
{
  agentAuthenticationGroup_t *data = (agentAuthenticationGroup_t *) doCur->data;
  agentAuthenticationGroup_t *undodata = (agentAuthenticationGroup_t *) doCur->undodata;
  agentAuthenticationGroup_t *groupsetdata = NULL;

  L7_int32 function = SR_UNKNOWN;
  L7_int32 agentAuthenticationListTempIndex; /* Temporary Index */
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_int32 rc = L7_FAILURE;

  /* Define a table entry for destroying the created row */
  agentAuthenticationListEntry_t agentAuthenticationListEntryData;
  agentAuthenticationListEntry_t *entrysetdata = &agentAuthenticationListEntryData;

  /* Initialize this table entry */
  agentAuthenticationListEntryData.agentAuthenticationListName = MakeOctetString(NULL, 0);
  ZERO_VALID(agentAuthenticationListEntryData.valid);

  if ( creationErrorAuthenticationGroup == L7_TRUE )
  {
    creationErrorAuthenticationGroup = L7_FALSE;
    free(agentAuthenticationListEntryData.agentAuthenticationListName);
    return NO_ERROR;
  }

  /*Copy valid bits from data to undodata */
  if( undodata == NULL || data == NULL )
  {
    free(agentAuthenticationListEntryData.agentAuthenticationListName);
    return UNDO_FAILED_ERROR;
  }
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  if( VALID(I_agentAuthenticationListCreate, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    strncpy(snmp_buffer, data->agentAuthenticationListCreate->octet_ptr,
                                        data->agentAuthenticationListCreate->length);

    /* Get the Authentication List index from the authentication list name */
    if ((rc = userMgrAPLIndexGet(snmp_buffer, &agentAuthenticationListTempIndex)) == L7_FAILURE)
    {
      free(agentAuthenticationListEntryData.agentAuthenticationListName);
      return UNDO_FAILED_ERROR;
    }

    /* we are trying to undo an add to the Table
     * undoing an add, so delete
    */
    agentAuthenticationListEntryData.agentAuthenticationListStatus = D_agentAuthenticationListStatus_destroy;
    agentAuthenticationListEntryData.agentAuthenticationListIndex = agentAuthenticationListTempIndex;
    SET_VALID(I_agentAuthenticationListStatus, agentAuthenticationListEntryData.valid);
    function = SR_DELETE;

    /* use the set method for the undo */
    if ((entrysetdata != NULL) && (k_agentAuthenticationListEntry_set(entrysetdata,
                                                                      contextInfo, function) == NO_ERROR))
    {
      free(agentAuthenticationListEntryData.agentAuthenticationListName);
      return NO_ERROR;
    }
  }
  if (VALID(I_agentUserConfigDefaultAuthenticationList, data->valid))
  {
     /* undoing a modify, replace the original data */
      groupsetdata = undodata;
      function = SR_ADD_MODIFY;

      /* use the set method for the undo */
      if ((groupsetdata != NULL) && (k_agentAuthenticationGroup_set(groupsetdata, contextInfo, function) == NO_ERROR))
      {
        free(agentAuthenticationListEntryData.agentAuthenticationListName);
        return NO_ERROR;
      }
  }

  free(agentAuthenticationListEntryData.agentAuthenticationListName);
  return UNDO_FAILED_ERROR;
}
#endif /* SR_agentAuthenticationGroup_UNDO */

#endif /* SETS */

agentAuthenticationListEntry_t *
k_agentAuthenticationListEntry_get(int serialNum, ContextInfo *contextInfo,
                                   int nominator,
                                   int searchType,
                                   SR_UINT32 agentAuthenticationListIndex)
{
  static agentAuthenticationListEntry_t agentAuthenticationListEntryData;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  agentAuthenticationListEntryData.agentAuthenticationListIndex = agentAuthenticationListIndex;
  SET_VALID(I_agentAuthenticationListIndex, agentAuthenticationListEntryData.valid);

  if ((searchType == EXACT) ?
      (snmpAgentAuthenticationListEntryGet(USMDB_UNIT_CURRENT,
                                           agentAuthenticationListEntryData.agentAuthenticationListIndex) != L7_SUCCESS) :
      ((snmpAgentAuthenticationListEntryGet(USMDB_UNIT_CURRENT,
                                            agentAuthenticationListEntryData.agentAuthenticationListIndex) != L7_SUCCESS) &&
       (snmpAgentAuthenticationListEntryNextGet(USMDB_UNIT_CURRENT,
                                                &agentAuthenticationListEntryData.agentAuthenticationListIndex) != L7_SUCCESS)))
  {
    return(NULL);
  }

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
  case I_agentAuthenticationListIndex:
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentAuthenticationListName:
    if (usmDbAPLListGetIndex(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN,
                             agentAuthenticationListEntryData.agentAuthenticationListIndex,
                             snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromText(&agentAuthenticationListEntryData.agentAuthenticationListName, snmp_buffer) == L7_TRUE)
      SET_VALID(nominator, agentAuthenticationListEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentAuthenticationListMethod1:
    if (snmpAgentAuthenticationListMethod1Get(USMDB_UNIT_CURRENT,
                                              agentAuthenticationListEntryData.agentAuthenticationListIndex,
                                              &agentAuthenticationListEntryData.agentAuthenticationListMethod1) == L7_SUCCESS)
      SET_VALID(nominator, agentAuthenticationListEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentAuthenticationListMethod2:
    if (snmpAgentAuthenticationListMethod2Get(USMDB_UNIT_CURRENT,
                                              agentAuthenticationListEntryData.agentAuthenticationListIndex,
                                              &agentAuthenticationListEntryData.agentAuthenticationListMethod2) == L7_SUCCESS)
      SET_VALID(nominator, agentAuthenticationListEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentAuthenticationListMethod3:
    if (snmpAgentAuthenticationListMethod3Get(USMDB_UNIT_CURRENT,
                                              agentAuthenticationListEntryData.agentAuthenticationListIndex,
                                              &agentAuthenticationListEntryData.agentAuthenticationListMethod3) == L7_SUCCESS)
      SET_VALID(nominator, agentAuthenticationListEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentAuthenticationListStatus:
    /* always active */
    agentAuthenticationListEntryData.agentAuthenticationListStatus = D_agentAuthenticationListStatus_active;
    SET_VALID(nominator, agentAuthenticationListEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentAuthenticationListMethod4:
    if (snmpAgentAuthenticationListMethod4Get(USMDB_UNIT_CURRENT,
                                              agentAuthenticationListEntryData.agentAuthenticationListIndex,
                                              &agentAuthenticationListEntryData.agentAuthenticationListMethod4) == L7_SUCCESS)
      SET_VALID(nominator, agentAuthenticationListEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentAuthenticationListEntryData.valid))
    return(NULL);

  return(&agentAuthenticationListEntryData);
}

#ifdef SETS
int
k_agentAuthenticationListEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                    doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentAuthenticationListEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                     doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentAuthenticationListEntry_set_defaults(doList_t *dp)
{
    agentAuthenticationListEntry_t *data = (agentAuthenticationListEntry_t *) (dp->data);

    if ((data->agentAuthenticationListName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentAuthenticationListEntry_set(agentAuthenticationListEntry_t *data,
                                   ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (VALID(I_agentAuthenticationListMethod1, data->valid))
  {
    if( snmpAgentAuthenticationListMethod1Set(USMDB_UNIT_CURRENT,
                                            data->agentAuthenticationListIndex,
                                            data->agentAuthenticationListMethod1) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentAuthenticationListMethod1, tempValid);
    }
  }

  if (VALID(I_agentAuthenticationListMethod2, data->valid))
  {
    if( snmpAgentAuthenticationListMethod2Set(USMDB_UNIT_CURRENT,
                                            data->agentAuthenticationListIndex,
                                            data->agentAuthenticationListMethod2) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentAuthenticationListMethod2, tempValid);
    }
  }

  if (VALID(I_agentAuthenticationListMethod3, data->valid))
  {
    if( snmpAgentAuthenticationListMethod3Set(USMDB_UNIT_CURRENT,
                                            data->agentAuthenticationListIndex,
                                            data->agentAuthenticationListMethod3) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentAuthenticationListMethod3, tempValid);
    }
  }

  if (VALID(I_agentAuthenticationListStatus, data->valid))
  {
    if( snmpAgentAuthenticationListStatusSet(USMDB_UNIT_CURRENT,
                                           data->agentAuthenticationListIndex,
                                           data->agentAuthenticationListStatus) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentAuthenticationListStatus, tempValid);
    }
  }

  if (VALID(I_agentAuthenticationListMethod4, data->valid))
  {
    if( snmpAgentAuthenticationListMethod4Set(USMDB_UNIT_CURRENT,
                                            data->agentAuthenticationListIndex,
                                            data->agentAuthenticationListMethod4) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;
}

#ifdef SR_agentAuthenticationListEntry_UNDO
/* add #define SR_agentAuthenticationListEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentAuthenticationListEntry family.
 */
int
agentAuthenticationListEntry_undo(doList_t *doHead, doList_t *doCur,
                                  ContextInfo *contextInfo)
{
  agentAuthenticationListEntry_t *data = (agentAuthenticationListEntry_t *) doCur->data;
  agentAuthenticationListEntry_t *undodata = (agentAuthenticationListEntry_t *) doCur->undodata;
  agentAuthenticationListEntry_t *entrysetdata = NULL;

  L7_int32 function = SR_UNKNOWN;
  L7_int32 rc = L7_FAILURE;
  L7_int32 agentAuthenticationListTempIndex;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  /* Define a group instance so as to create the deleted row */
  agentAuthenticationGroup_t agentAuthenticationGroupTempdata;
  agentAuthenticationGroup_t *setdata = &agentAuthenticationGroupTempdata;

  /* Initialize this group instance */
  agentAuthenticationGroupTempdata.agentAuthenticationListCreate = CloneOctetString(undodata->agentAuthenticationListName);
  ZERO_VALID(agentAuthenticationGroupTempdata.valid);


  /*Copy valid bits from data to undodata */
  if( undodata != NULL && data != NULL )
    memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /*
   * we are either trying to undo a delete, or a modify
   * undoing a delete or modify, replace the original data
   */
  if(data->agentAuthenticationListStatus ==  D_agentAuthenticationListStatus_destroy)
  {
    /* check that the entry was actually deleted */
    if (snmpAgentAuthenticationListEntryGet(USMDB_UNIT_CURRENT,
                                            undodata->agentAuthenticationListIndex) == L7_SUCCESS)
      return NO_ERROR;

    /* Use the set method for the creation of the row with default values */
    SET_VALID(I_agentAuthenticationListCreate, agentAuthenticationGroupTempdata.valid);
    function = SR_ADD_MODIFY;
    if ((setdata != NULL) && k_agentAuthenticationGroup_set(setdata, contextInfo, function) == NO_ERROR)
    {
      bzero(snmp_buffer, SNMP_BUFFER_LEN);
      strncpy(snmp_buffer, undodata->agentAuthenticationListName->octet_ptr, undodata->agentAuthenticationListName->length);

      /* Get the Authentication List index from the authentication list name */
      if ( (rc = userMgrAPLIndexGet(snmp_buffer, &agentAuthenticationListTempIndex)) == L7_SUCCESS)
      {
        undodata->agentAuthenticationListIndex = agentAuthenticationListTempIndex;

        /* set all valid for the setting the previous values to the created row */
        memset(undodata->valid, 0xff, sizeof(undodata->valid));

        /* Status should not be set again */
        CLR_VALID(I_agentAuthenticationListStatus, undodata->valid);

        entrysetdata = undodata;

        /* use the set method for the undo */
        if ((entrysetdata != NULL) && k_agentAuthenticationListEntry_set(entrysetdata,
                                                                         contextInfo, function) == NO_ERROR)
        {
          free(agentAuthenticationGroupTempdata.agentAuthenticationListCreate);
          return NO_ERROR;
        }
      }
    }
  }
  else
  {
    /* we are trying to undo a modify to the Table */
    if(undodata->agentAuthenticationListStatus == D_agentAuthenticationListStatus_notReady
    || undodata->agentAuthenticationListStatus == D_agentAuthenticationListStatus_notInService)
    {
      undodata->agentAuthenticationListStatus = D_agentAuthenticationListStatus_createAndWait;
    }
    else
    {
      if(undodata->agentAuthenticationListStatus == D_agentAuthenticationListStatus_active)
      {
        undodata->agentAuthenticationListStatus = D_agentAuthenticationListStatus_createAndGo;
      }
    }
    entrysetdata = undodata;
    function = SR_ADD_MODIFY;

    /* use the set method for the undo */
    if ((entrysetdata != NULL) && k_agentAuthenticationListEntry_set(entrysetdata,
                                                                     contextInfo, function) == NO_ERROR) \
    {
      free(agentAuthenticationGroupTempdata.agentAuthenticationListCreate);
      return NO_ERROR;
    }
  }

  free(agentAuthenticationGroupTempdata.agentAuthenticationListCreate);
  return UNDO_FAILED_ERROR;
}
#endif /* SR_agentAuthenticationListEntry_UNDO */

#endif /* SETS */


agentClassOfServicePortEntry_t *
k_agentClassOfServicePortEntry_get(int serialNum, ContextInfo *contextInfo,
                                   int nominator,
                                   int searchType,
                                   SR_INT32 ifIndex,
                                   SR_INT32 agentClassOfServicePortPriority)
{
   static agentClassOfServicePortEntry_t agentClassOfServicePortEntryData;
   L7_uint32 intIfNum;

   if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1P_COMPONENT_ID,
                   L7_DOT1P_USER_PRIORITY_PER_INTERFACE_FEATURE_ID) != L7_TRUE)
   {
     return(NULL);
   }

   ZERO_VALID(agentClassOfServicePortEntryData.valid);
   agentClassOfServicePortEntryData.ifIndex = ifIndex;
   SET_VALID(I_agentClassOfServicePortEntryIndex_ifIndex, agentClassOfServicePortEntryData.valid);
   agentClassOfServicePortEntryData.agentClassOfServicePortPriority = agentClassOfServicePortPriority;
   SET_VALID(I_agentClassOfServicePortPriority, agentClassOfServicePortEntryData.valid);


   if (((searchType == EXACT) ?
        (snmpAgentClassOfServicePortGet(USMDB_UNIT_CURRENT, agentClassOfServicePortEntryData.ifIndex,
                       agentClassOfServicePortEntryData.agentClassOfServicePortPriority) != L7_SUCCESS) :
        (snmpAgentClassOfServicePortGet(USMDB_UNIT_CURRENT, agentClassOfServicePortEntryData.ifIndex,
                       agentClassOfServicePortEntryData.agentClassOfServicePortPriority) != L7_SUCCESS) &&
        (snmpAgentClassOfServicePortNextGet(USMDB_UNIT_CURRENT, &agentClassOfServicePortEntryData.ifIndex,
                       &agentClassOfServicePortEntryData.agentClassOfServicePortPriority) != L7_SUCCESS)) ||
        (usmDbIntIfNumFromExtIfNum(agentClassOfServicePortEntryData.ifIndex, &intIfNum) != L7_SUCCESS))
   {
     ZERO_VALID(agentClassOfServicePortEntryData.valid);
     return(NULL);
   }

/*
 * if ( nominator != -1 ) condition is added to all the case statements
 * to support the undo functionality for getting all the values.
 */

   switch (nominator)
   {
   case -1:
   case I_agentClassOfServicePortEntryIndex_ifIndex:
   case I_agentClassOfServicePortPriority:
     if ( nominator != -1 ) break;
     /* else pass through */

   case I_agentClassOfServicePortClass:
     if (usmDbDot1dTrafficClassGet(USMDB_UNIT_CURRENT, intIfNum,
                   agentClassOfServicePortEntryData.agentClassOfServicePortPriority,
                   &agentClassOfServicePortEntryData.agentClassOfServicePortClass) == L7_SUCCESS)
       SET_VALID(I_agentClassOfServicePortClass, agentClassOfServicePortEntryData.valid);
     break;

   default:
     /* unknown nominator */
     return(NULL);
     break;
   }

   if (nominator >= 0 && !VALID(nominator, agentClassOfServicePortEntryData.valid))
     return(NULL);

   return(&agentClassOfServicePortEntryData);
}

#ifdef SETS
int
k_agentClassOfServicePortEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                    doList_t *dp, ContextInfo *contextInfo)
{
    return NO_ERROR;
}

int
k_agentClassOfServicePortEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                     doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentClassOfServicePortEntry_set_defaults(doList_t *dp)
{
    agentClassOfServicePortEntry_t *data = (agentClassOfServicePortEntry_t *) (dp->data);


    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentClassOfServicePortEntry_set(agentClassOfServicePortEntry_t *data,
                                   ContextInfo *contextInfo, int function)
{
  L7_uint32 intIfNum;

  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  /* Conversion to internal interface number for usmdb calls */
  if (usmDbIntIfNumFromExtIfNum(data->ifIndex, &intIfNum) != L7_SUCCESS)
  {
    ZERO_VALID(data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (usmDbDot1dTrafficClassEntryGet(USMDB_UNIT_CURRENT, intIfNum,
                            data->agentClassOfServicePortPriority) != L7_SUCCESS ||
      usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1P_COMPONENT_ID,
                               L7_DOT1P_FEATURE_SUPPORTED) != L7_TRUE ||
      usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1P_COMPONENT_ID,
                               L7_DOT1P_USER_PRIORITY_PER_INTERFACE_FEATURE_ID) != L7_TRUE)
  {
    ZERO_VALID(data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentClassOfServicePortClass, data->valid))
  {
    if (usmDbDot1dTrafficClassSet(USMDB_UNIT_CURRENT, intIfNum,
                                  data->agentClassOfServicePortPriority,
                                  data->agentClassOfServicePortClass) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;
}

#ifdef SR_agentClassOfServicePortEntry_UNDO
/* add #define SR_agentClassOfServicePortEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentClassOfServicePortEntry family.
 */
int
agentClassOfServicePortEntry_undo(doList_t *doHead, doList_t *doCur,
                                  ContextInfo *contextInfo)
{
   agentClassOfServicePortEntry_t *data = (agentClassOfServicePortEntry_t *) doCur->data;
   agentClassOfServicePortEntry_t *undodata = (agentClassOfServicePortEntry_t *) doCur->undodata;
   agentClassOfServicePortEntry_t *setdata = NULL;
   L7_uint32 function = SR_UNKNOWN;

   /*
    * Modifications for UNDO Feature
    * Setting valid bits of undodata same as that for data
    */
   if ( data->valid == NULL )
     return UNDO_FAILED_ERROR;
   memcpy(undodata->valid,data->valid,sizeof(data->valid));

   /* we are either trying to undo an add or a delete which is not possible */
   if ( undodata == NULL )
     return NO_ERROR;

   /* undoing a modify, replace the original data */
   setdata = undodata;
   function = SR_ADD_MODIFY;

   /* use the set method for the undo */
   if ((setdata != NULL) && k_agentClassOfServicePortEntry_set(setdata, contextInfo, function) == NO_ERROR)
   {
     return NO_ERROR;
   }

   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentClassOfServicePortEntry_UNDO */

#endif /* SETS */


agentSystemGroup_t *
k_agentSystemGroup_get(int serialNum, ContextInfo *contextInfo,
                       int nominator)
{
  static agentSystemGroup_t agentSystemGroupData;

  ZERO_VALID(agentSystemGroupData.valid);

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentSaveConfig:
    agentSystemGroupData.agentSaveConfig = D_agentSaveConfig_disable;
    SET_VALID(I_agentSaveConfig, agentSystemGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentSaveConfigStatus:
    CLR_VALID(I_agentSaveConfigStatus, agentSystemGroupData.valid);
    if (snmpAgentSaveConfigStatusGet(USMDB_UNIT_CURRENT,
                                           &agentSystemGroupData.agentSaveConfigStatus) == L7_SUCCESS)
      SET_VALID(I_agentSaveConfigStatus, agentSystemGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentClearConfig:
    agentSystemGroupData.agentClearConfig = D_agentClearConfig_disable;
    SET_VALID(I_agentClearConfig, agentSystemGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentClearLags:
    agentSystemGroupData.agentClearLags = D_agentClearLags_disable;
    SET_VALID(I_agentClearLags, agentSystemGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentClearLoginSessions:
    agentSystemGroupData.agentClearLoginSessions = D_agentClearLoginSessions_disable;
    SET_VALID(I_agentClearLoginSessions, agentSystemGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentClearPasswords:
    agentSystemGroupData.agentClearPasswords = D_agentClearPasswords_disable;
    SET_VALID(I_agentClearPasswords, agentSystemGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentClearPortStats:
    agentSystemGroupData.agentClearPortStats = D_agentClearPortStats_disable;
    SET_VALID(I_agentClearPortStats, agentSystemGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentClearSwitchStats:
    agentSystemGroupData.agentClearSwitchStats = D_agentClearSwitchStats_disable;
    SET_VALID(I_agentClearSwitchStats, agentSystemGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentClearTrapLog:
    agentSystemGroupData.agentClearTrapLog = D_agentClearTrapLog_disable;
    SET_VALID(I_agentClearTrapLog, agentSystemGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentClearVlan:
    agentSystemGroupData.agentClearVlan = D_agentClearVlan_disable;
    SET_VALID(I_agentClearVlan, agentSystemGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

#ifdef I_agentStartupConfigErase
  case I_agentStartupConfigErase :
    agentSystemGroupData.agentStartupConfigErase = 0;
    SET_VALID(I_agentStartupConfigErase, agentSystemGroupData.valid);
    if ( nominator != -1 ) break;
#endif

  case I_agentResetSystem:
    if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_STACKING_COMPONENT_ID) != L7_TRUE)
    {
      agentSystemGroupData.agentResetSystem = D_agentResetSystem_disable;
      SET_VALID(I_agentResetSystem, agentSystemGroupData.valid);
    }
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentSystemGroupData.valid) )
    return(NULL);

  return(&agentSystemGroupData);
}

#ifdef SETS
int
k_agentSystemGroup_test(ObjectInfo *object, ObjectSyntax *value,
                        doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentSystemGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                         doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentSystemGroup_set(agentSystemGroup_t *data,
                       ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if ( VALID(I_agentSaveConfig, data->valid))
  {
    if( snmpAgentSaveConfigSet(USMDB_UNIT_CURRENT,
                              data->agentSaveConfig) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentSaveConfig, tempValid);
    }
  }

  if ( VALID(I_agentClearConfig, data->valid))
  {
    if( snmpAgentClearConfigSet(USMDB_UNIT_CURRENT,
                               data->agentClearConfig) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentClearConfig, tempValid);
    }
  }

  if ( VALID(I_agentClearLags, data->valid))
  {
    if( snmpAgentClearLagsSet(USMDB_UNIT_CURRENT,
                             data->agentClearLags) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentClearLags, tempValid);
    }
  }

  if ( VALID(I_agentClearLoginSessions, data->valid))
  {
    if( snmpAgentClearLoginSessionsSet(USMDB_UNIT_CURRENT,
                                      data->agentClearLoginSessions) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentClearLoginSessions, tempValid);
    }
  }

  if ( VALID(I_agentClearPasswords, data->valid))
  {
    if( snmpAgentClearPasswordsSet(USMDB_UNIT_CURRENT,
                                  data->agentClearPasswords) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentClearPasswords, tempValid);
    }
  }

  if ( VALID(I_agentClearPortStats, data->valid) &&
       snmpAgentClearPortStatsSet(USMDB_UNIT_CURRENT,
                                  data->agentClearPortStats) != L7_SUCCESS )
  {
    memcpy(data->valid, tempValid, sizeof(data->valid));
    return(COMMIT_FAILED_ERROR);
  }
  else
  {
    SET_VALID(I_agentClearPortStats, tempValid);
  }

  if ( VALID(I_agentClearSwitchStats, data->valid))
  {
    if( snmpAgentClearSwitchStatsSet(USMDB_UNIT_CURRENT,
                                    data->agentClearSwitchStats) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentClearSwitchStats, tempValid);
    }
  }

  if ( VALID(I_agentClearTrapLog, data->valid))
  {
    if( snmpAgentClearTrapLogSet(USMDB_UNIT_CURRENT,
                                data->agentClearTrapLog) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentClearTrapLog, tempValid);
    }
  }

  if ( VALID(I_agentClearVlan, data->valid))
  {
    if( snmpAgentClearVlanSet(USMDB_UNIT_CURRENT,
                             data->agentClearVlan) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentClearVlan, tempValid);
    }
  }

  if ( VALID(I_agentResetSystem, data->valid))
  {
    if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_STACKING_COMPONENT_ID) != L7_TRUE)
    {
      if( snmpAgentResetSystemSet(USMDB_UNIT_CURRENT,
                                  data->agentResetSystem) != L7_SUCCESS )
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
    }
    else
    {
      return(COMMIT_FAILED_ERROR);
    }
  }

#ifdef I_agentStartupConfigErase
  if(VALID(I_agentStartupConfigErase, data->valid))
  {
    if(data->agentStartupConfigErase == L7_TRUE)
    {

      if(usmdbEraseStartupConfig() != L7_SUCCESS)
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
    }
  }
#endif

  return NO_ERROR;
}

  #ifdef SR_agentSystemGroup_UNDO
/* add #define SR_agentSystemGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentSystemGroup family.
 */
int
agentSystemGroup_undo(doList_t *doHead, doList_t *doCur,
                      ContextInfo *contextInfo)
{
  agentSystemGroup_t *data = (agentSystemGroup_t *) doCur->data;
  agentSystemGroup_t *undodata = (agentSystemGroup_t *) doCur->undodata;
  agentSystemGroup_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentSystemGroup_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentSystemGroup_UNDO */

#endif /* SETS */

agentCableTesterGroup_t *
k_agentCableTesterGroup_get(int serialNum, ContextInfo *contextInfo,
                                   int nominator)
{
  static agentCableTesterGroup_t agentCableTesterGroupData;

  ZERO_VALID(agentCableTesterGroupData.valid);

  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_CABLE_TEST_COMPONENT_ID) != L7_TRUE)
    return(NULL);

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentCableTesterStatus:
    if (snmpAgentCableTesterStatusGet(&agentCableTesterGroupData.agentCableTesterStatus) == L7_SUCCESS)
      SET_VALID(nominator, agentCableTesterGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentCableTesterIfIndex:
    if (snmpAgentCableTesterIfIndexGet(&agentCableTesterGroupData.agentCableTesterIfIndex) == L7_SUCCESS)
      SET_VALID(nominator, agentCableTesterGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentCableTesterCableStatus:
    if (snmpAgentCableTesterCableStatusGet(&agentCableTesterGroupData.agentCableTesterCableStatus) == L7_SUCCESS)
      SET_VALID(nominator, agentCableTesterGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentCableTesterMinimumCableLength:
    if (SnmpCableTesterMinLengthGet(&agentCableTesterGroupData.agentCableTesterMinimumCableLength) == L7_SUCCESS)
      SET_VALID(nominator, agentCableTesterGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentCableTesterMaximumCableLength:
    if (SnmpCableTesterMaxLengthGet(&agentCableTesterGroupData.agentCableTesterMaximumCableLength) == L7_SUCCESS)
      SET_VALID(nominator, agentCableTesterGroupData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentCableTesterCableFailureLocation:
    if (SnmpCableTesterFailLocationGet(&agentCableTesterGroupData.agentCableTesterCableFailureLocation) == L7_SUCCESS)
      SET_VALID(nominator, agentCableTesterGroupData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentCableTesterGroupData.valid))
    return(NULL);

  return(&agentCableTesterGroupData);
}

#ifdef SETS
int
k_agentCableTesterGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                    doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentCableTesterGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                                     doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentCableTesterGroup_set(agentCableTesterGroup_t *data,
                                   ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_CABLE_TEST_COMPONENT_ID) != L7_TRUE)
    return(COMMIT_FAILED_ERROR);

  if (VALID(I_agentCableTesterStatus, data->valid))
  {
    if( snmpAgentCableTesterStatusSet(data->agentCableTesterStatus) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentCableTesterStatus, tempValid);
    }
  }

  if (VALID(I_agentCableTesterIfIndex, data->valid))
  {
    if( snmpAgentCableTesterIfIndexSet(data->agentCableTesterIfIndex) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;
}

#ifdef SR_agentCableTesterGroup_UNDO
/* add #define SR_agentCableTesterGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentCableTesterGroup family.
 */
int
agentCableTesterGroup_undo(doList_t *doHead, doList_t *doCur,
                                  ContextInfo *contextInfo)
{
  agentCableTesterGroup_t *data = (agentCableTesterGroup_t *) doCur->data;
  agentCableTesterGroup_t *undodata = (agentCableTesterGroup_t *) doCur->undodata;
  agentCableTesterGroup_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL)
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentCableTesterGroup_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
#endif /* SR_agentCableTesterGroup_UNDO */

#endif /* SETS */

#ifdef L7_DHCP_FILTER_PACKAGE

agentDhcpFilteringGroup_t *
k_agentDhcpFilteringGroup_get(int serialNum, ContextInfo *contextInfo,
                                                     int nominator)
{
  static agentDhcpFilteringGroup_t agentDhcpFilteringGroupData;

  ZERO_VALID(agentDhcpFilteringGroupData.valid);

  switch (nominator)
  {
    case -1:
      if ( nominator != -1 ) break;
      /* else pass through */

    case I_agentDhcpFilteringAdminMode:
      if (snmpAgentDhcpFilteringAdminModeGet(&agentDhcpFilteringGroupData.agentDhcpFilteringAdminMode)
                                                        == L7_SUCCESS)
      SET_VALID(I_agentDhcpFilteringAdminMode,agentDhcpFilteringGroupData.valid);
      break;

    default:
      /* unknown nominator */
      return(NULL);
      break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentDhcpFilteringGroupData.valid) )
        return(NULL);

  return (&agentDhcpFilteringGroupData);
}

#ifdef SETS
int
k_agentDhcpFilteringGroup_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{
  return NO_ERROR;
}

int
k_agentDhcpFilteringGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                               doList_t *doHead, doList_t *dp)
{
  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentDhcpFilteringGroup_set(agentDhcpFilteringGroup_t *data,
                             ContextInfo *contextInfo, int function)
{
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (VALID(I_agentDhcpFilteringAdminMode, data->valid))
  {
    if( snmpAgentDhcpFilteringAdminModeSet(data->agentDhcpFilteringAdminMode) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;;
}

#ifdef SR_agentDhcpFilteringGroup_UNDO
/* add #define SR_agentDhcpFilteringGroup_UNDO in sitedefs.h to
 *  *  * include the undo routine for the agentDhcpFilteringGroup family.
 *   *   */
int
agentDhcpFilteringGroup_undo(doList_t *doHead, doList_t *doCur,
                                ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
#endif /* SR_agentDhcpFilteringGroup_UNDO */
#endif /* SETS */

agentDhcpFilteringPortConfigEntry_t *
k_agentDhcpFilteringPortConfigEntry_get(int serialNum, ContextInfo *contextInfo,
                                       int nominator, int searchType,
                                               SR_INT32 ifIndex)
{
  static agentDhcpFilteringPortConfigEntry_t agentDhcpFilteringPortConfigEntryData;
  L7_uint32 intIfNum;
  /*
   *    ** put your code to retrieve the information here
   *    **/
  ZERO_VALID(agentDhcpFilteringPortConfigEntryData.valid);
  agentDhcpFilteringPortConfigEntryData.ifIndex = ifIndex;

  SET_VALID(I_agentDhcpFilteringPortConfigEntryIndex_ifIndex,
          agentDhcpFilteringPortConfigEntryData.valid);

  if(((searchType == EXACT)
    ?(snmpDhcpFilterIntfGet(agentDhcpFilteringPortConfigEntryData.ifIndex) != L7_SUCCESS)
        :(snmpDhcpFilterIntfGet(agentDhcpFilteringPortConfigEntryData.ifIndex) != L7_SUCCESS &&
      snmpNextDhcpFilterIntfGet(agentDhcpFilteringPortConfigEntryData.ifIndex,
                          &agentDhcpFilteringPortConfigEntryData.ifIndex)
                     != L7_SUCCESS)))
  {
    ZERO_VALID(agentDhcpFilteringPortConfigEntryData.valid);
    return(NULL);
  }

  usmDbIntIfNumFromExtIfNum(agentDhcpFilteringPortConfigEntryData.ifIndex, &intIfNum);

  switch (nominator)
  {
    case -1:
    case I_agentDhcpFilteringPortConfigEntryIndex_ifIndex :
      if ( nominator != -1 ) break;
        /* else pass through */

    case I_agentDhcpFilteringPortTrustedMode :
      if(snmpDhcpFilteringPortTrustedModeGet(intIfNum,
              &agentDhcpFilteringPortConfigEntryData.agentDhcpFilteringPortTrustedMode)
                  == L7_SUCCESS)
      {
          SET_VALID(I_agentDhcpFilteringPortTrustedMode,
            agentDhcpFilteringPortConfigEntryData.valid);
      }
        break;
    default:
        /* unknown nominator */
        return(NULL);
      break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentDhcpFilteringPortConfigEntryData.valid) )
                    return(NULL);
  return(&agentDhcpFilteringPortConfigEntryData);
}

#ifdef SETS
int
k_agentDhcpFilteringPortConfigEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                        doList_t *dp, ContextInfo *contextInfo)
{
  return NO_ERROR;
}

int
k_agentDhcpFilteringPortConfigEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                         doList_t *doHead, doList_t *dp)
{
  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentDhcpFilteringPortConfigEntry_set_defaults(doList_t *dp)
{
  agentDhcpFilteringPortConfigEntry_t *data =
      (agentDhcpFilteringPortConfigEntry_t *) (dp->data);

  data->agentDhcpFilteringPortTrustedMode = D_agentDhcpFilteringPortTrustedMode_untrusted;

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_agentDhcpFilteringPortConfigEntry_set(agentDhcpFilteringPortConfigEntry_t *data,
                                         ContextInfo *contextInfo, int function)
{
  L7_uint32 intIfNum;
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (usmDbIntIfNumFromExtIfNum(data->ifIndex,&intIfNum) != L7_SUCCESS)
                   return (COMMIT_FAILED_ERROR);
  if (VALID(I_agentDhcpFilteringPortTrustedMode, data->valid))
  {
    if(snmpDhcpFilteringPortTrustedModeSet(intIfNum,
                              data->agentDhcpFilteringPortTrustedMode)
                        != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      CLR_VALID(I_agentDhcpFilteringPortTrustedMode, tempValid);
    }
  }

  return NO_ERROR;
}

#ifdef SR_agentDhcpFilteringPortConfigEntry_UNDO
/* add #define SR_agentDhcpFilteringPortConfigEntry_UNDO in sitedefs.h to
 *  *  * include the undo routine for the agentDhcpFilteringPortConfigEntry family.
 *   *   */
int
agentDhcpFilteringPortConfigEntry_undo(doList_t *doHead, doList_t *doCur,
                              ContextInfo *contextInfo)
{
  agentDhcpFilteringPortConfigEntry_t *data = (agentDhcpFilteringPortConfigEntry_t *)doCur->data;
  agentDhcpFilteringPortConfigEntry_t *undodata = (agentDhcpFilteringPortConfigEntry_t *)doCur->undodata;
  agentDhcpFilteringPortConfigEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   *** Modifications for UNDO Feature
   *** Setting valid bits of undodata same as that for data
   ***/
  if ( data == NULL || undodata == NULL )
                return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  if ((setdata!= NULL) &&
      (k_agentDhcpFilteringPortConfigEntry_set(setdata, contextInfo, function)
                                                     == NO_ERROR ))
        return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
#endif /* SR_agentDhcpFilteringPortConfigEntry_UNDO */

#endif /* SETS */

#endif /* L7_DHCP_FILTER_PACKAGE */



agentHTTPConfigGroup_t *
k_agentHTTPConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                          int nominator)
{
  static agentHTTPConfigGroup_t agentHTTPConfigGroupData;

  ZERO_VALID(agentHTTPConfigGroupData.valid);

  switch (nominator)
  {
  case -1:
    if (nominator != -1) break;
    /* else pass through */

  case I_agentHTTPWebMode :
    if (snmpAgentNetworkWebModeGet(USMDB_UNIT_CURRENT, &agentHTTPConfigGroupData.agentHTTPWebMode) == L7_SUCCESS)
      SET_VALID(I_agentHTTPWebMode, agentHTTPConfigGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentHTTPJavaMode :
    if (snmpAgentNetworkJavaModeGet(USMDB_UNIT_CURRENT, &agentHTTPConfigGroupData.agentHTTPJavaMode) == L7_SUCCESS)
      SET_VALID(I_agentHTTPJavaMode, agentHTTPConfigGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentHTTPMaxSessions:
    if (usmDbCliWebHttpNumSessionsGet(&agentHTTPConfigGroupData.agentHTTPMaxSessions) == L7_SUCCESS)
      SET_VALID(I_agentHTTPMaxSessions, agentHTTPConfigGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentHTTPHardTimeout:
    if (usmDbCliWebHttpSessionHardTimeOutGet(&agentHTTPConfigGroupData.agentHTTPHardTimeout) == L7_SUCCESS)
      SET_VALID(I_agentHTTPHardTimeout, agentHTTPConfigGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentHTTPSoftTimeout:
    if (usmDbCliWebHttpSessionSoftTimeOutGet(&agentHTTPConfigGroupData.agentHTTPSoftTimeout) == L7_SUCCESS)
      SET_VALID(I_agentHTTPSoftTimeout, agentHTTPConfigGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentHTTPConfigGroupData.valid))
    return (NULL);

  return(&agentHTTPConfigGroupData);
}

#ifdef SETS
int
k_agentHTTPConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                           doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentHTTPConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentHTTPConfigGroup_set(agentHTTPConfigGroup_t *data,
                          ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if ( VALID(I_agentHTTPWebMode, data->valid))
  {
    if(snmpAgentNetworkWebModeSet(USMDB_UNIT_CURRENT,
                                  data->agentHTTPWebMode) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentHTTPWebMode, tempValid);
    }
  }

  if ( VALID(I_agentHTTPJavaMode, data->valid))
  {
    if(snmpAgentNetworkJavaModeSet(USMDB_UNIT_CURRENT,
                                   data->agentHTTPJavaMode) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentHTTPJavaMode, tempValid);
    }
  }

  if ( VALID(I_agentHTTPMaxSessions, data->valid))
  {
    if(usmDbCliWebHttpNumSessionsSet(data->agentHTTPMaxSessions) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentHTTPMaxSessions, tempValid);
    }
  }

  if ( VALID(I_agentHTTPHardTimeout, data->valid))
  {
    if(usmDbCliWebHttpSessionHardTimeOutSet(data->agentHTTPHardTimeout) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentHTTPHardTimeout, tempValid);
    }
  }

  if ( VALID(I_agentHTTPSoftTimeout, data->valid))
  {
    if(usmDbCliWebHttpSessionSoftTimeOutSet(data->agentHTTPSoftTimeout) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentHTTPSoftTimeout, tempValid);
    }
  }

  return NO_ERROR;
}

#ifdef SR_agentHTTPConfigGroup_UNDO
/* add #define SR_agentHTTPConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentHTTPConfigGroup family.
 */
int
agentHTTPConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                         ContextInfo *contextInfo)
{
  agentHTTPConfigGroup_t *data = (agentHTTPConfigGroup_t *) doCur->data;
  agentHTTPConfigGroup_t *undodata = (agentHTTPConfigGroup_t *) doCur->undodata;
  agentHTTPConfigGroup_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentHTTPConfigGroup_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
#endif /* SR_agentHTTPConfigGroup_UNDO */

#endif /* SETS */

#ifdef I_agentDhcpSnoopingAdminMode

agentDhcpSnoopingConfigGroup_t *
k_agentDhcpSnoopingConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                                   int nominator)
{
   static agentDhcpSnoopingConfigGroup_t agentDhcpSnoopingConfigGroupData;
#ifdef L7_IPSG_PACKAGE
   L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
#endif
   static L7_BOOL firstTime = L7_TRUE;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
#ifdef L7_IPSG_PACKAGE
    agentDhcpSnoopingConfigGroupData.agentDhcpSnoopingRemoteFileName =
             MakeOctetString(NULL, 0);
#endif
  }

   
   ZERO_VALID(agentDhcpSnoopingConfigGroupData.valid);
   
   /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
    if (nominator != -1) break;
    /* else pass through */
  case I_agentDhcpSnoopingAdminMode:
      if (snmpDhcpSnoopingAdminModeGet(&agentDhcpSnoopingConfigGroupData.agentDhcpSnoopingAdminMode)
                                      == L7_SUCCESS)
      SET_VALID(I_agentDhcpSnoopingAdminMode,
                agentDhcpSnoopingConfigGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */
  case I_agentDhcpSnoopingVerifyMac:
      if (snmpDhcpSnoopingVerifyMacGet(&agentDhcpSnoopingConfigGroupData.agentDhcpSnoopingVerifyMac)
                                      == L7_SUCCESS)
      SET_VALID(I_agentDhcpSnoopingVerifyMac,
                agentDhcpSnoopingConfigGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */
#ifdef L7_IPSG_PACKAGE
  case I_agentDhcpSnoopingRemoteFileName:
    if((cnfgrIsFeaturePresent(L7_DHCP_SNOOPING_COMPONENT_ID,
                              L7_DHCP_SNOOPING_DB_PERSISTENCY)) != L7_TRUE)
    {
      return (NULL);
    }
    memset(snmp_buffer, 0, (size_t)SNMP_BUFFER_LEN);
    if ( (usmDbDsDbFileNameGet(snmp_buffer) == L7_SUCCESS) &&
         (SafeMakeOctetString (&agentDhcpSnoopingConfigGroupData.agentDhcpSnoopingRemoteFileName,
                               snmp_buffer,
                               strlen((char *)snmp_buffer)) == L7_TRUE)
       )
    {
      SET_VALID (I_agentDhcpSnoopingRemoteFileName, agentDhcpSnoopingConfigGroupData.valid);
    }
    if (nominator != -1) break;
  case I_agentDhcpSnoopingRemoteIpAddr:
    if((cnfgrIsFeaturePresent(L7_DHCP_SNOOPING_COMPONENT_ID,
                              L7_DHCP_SNOOPING_DB_PERSISTENCY)) != L7_TRUE)
    {
      return (NULL);
    }
   if ( usmDbDsDbIpAddrGet(&agentDhcpSnoopingConfigGroupData.agentDhcpSnoopingRemoteIpAddr)
        ==L7_SUCCESS)
   {

     SET_VALID (I_agentDhcpSnoopingRemoteIpAddr, agentDhcpSnoopingConfigGroupData.valid);
   }
    if (nominator != -1) break;

  case I_agentDhcpSnoopingStoreInterval:
    if((cnfgrIsFeaturePresent(L7_DHCP_SNOOPING_COMPONENT_ID,
                              L7_DHCP_SNOOPING_DB_PERSISTENCY)) != L7_TRUE)
    {
      return (NULL);
    }
    if ( usmDbDsDbStoreIntervalGet( &agentDhcpSnoopingConfigGroupData.agentDhcpSnoopingStoreInterval)==
         L7_SUCCESS)
    {
       SET_VALID (I_agentDhcpSnoopingStoreInterval, agentDhcpSnoopingConfigGroupData.valid);
    }
    if (nominator != -1) break;
#endif
  case I_agentDhcpSnoopingStatsReset :
    agentDhcpSnoopingConfigGroupData.agentDhcpSnoopingStatsReset =
                                              D_agentDhcpSnoopingStatsReset_none;
    SET_VALID(I_agentDhcpSnoopingStatsReset, agentDhcpSnoopingConfigGroupData.valid);
  break;
  default:
    /* unknown nominator */
    return(NULL);
    break;
  }
  if ( nominator >= 0 && !VALID(nominator, agentDhcpSnoopingConfigGroupData.valid) )
    return(NULL);

   return(&agentDhcpSnoopingConfigGroupData);
}

#ifdef SETS
int
k_agentDhcpSnoopingConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                    doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentDhcpSnoopingConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                                     doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentDhcpSnoopingConfigGroup_set(agentDhcpSnoopingConfigGroup_t *data,
                                   ContextInfo *contextInfo, int function)
{
#ifdef L7_IPSG_PACKAGE
  L7_char8  snmp_buffer[SNMP_BUFFER_LEN];
#endif

  if (VALID(I_agentDhcpSnoopingAdminMode, data->valid) &&
      snmpDhcpSnoopingAdminModeSet(data->agentDhcpSnoopingAdminMode) != L7_SUCCESS)
  {
      CLR_VALID(I_agentDhcpSnoopingAdminMode, data->valid);
      return COMMIT_FAILED_ERROR;
  }
  if (VALID(I_agentDhcpSnoopingVerifyMac, data->valid) &&
      snmpDhcpSnoopingVerifyMacSet(data->agentDhcpSnoopingVerifyMac) != L7_SUCCESS)
  {
      CLR_VALID(I_agentDhcpSnoopingAdminMode, data->valid);
      return COMMIT_FAILED_ERROR;
  }
  
  if (VALID(I_agentDhcpSnoopingStatsReset, data->valid) &&
      (data->agentDhcpSnoopingStatsReset == D_agentDhcpSnoopingStatsReset_reset) &&
      usmDbDsIntfStatsClear () != L7_SUCCESS)
  {
      CLR_VALID(I_agentDhcpSnoopingStatsReset, data->valid);
      return COMMIT_FAILED_ERROR;
  }
#ifdef L7_IPSG_PACKAGE
  if ( VALID(I_agentDhcpSnoopingRemoteIpAddr, data->valid))
  {
     if((cnfgrIsFeaturePresent(L7_DHCP_SNOOPING_COMPONENT_ID,
                               L7_DHCP_SNOOPING_DB_PERSISTENCY)) != L7_TRUE)
     {
       return COMMIT_FAILED_ERROR;
     }
     if (usmDbDsDbIpAddrSet(data->agentDhcpSnoopingRemoteIpAddr) != L7_SUCCESS)
     {
       CLR_VALID(I_agentDhcpSnoopingRemoteIpAddr,data->valid);
       return COMMIT_FAILED_ERROR;
     }
  }
  if ( VALID(I_agentDhcpSnoopingRemoteFileName, data->valid))
  {
     if((cnfgrIsFeaturePresent(L7_DHCP_SNOOPING_COMPONENT_ID,
                               L7_DHCP_SNOOPING_DB_PERSISTENCY)) != L7_TRUE)
     {
       return COMMIT_FAILED_ERROR;
     }
     bzero(snmp_buffer, SNMP_BUFFER_LEN);
     memcpy(snmp_buffer,data->agentDhcpSnoopingRemoteFileName->octet_ptr,
                        data->agentDhcpSnoopingRemoteFileName->length);
     if (usmDbDsDbFileNameSet (snmp_buffer)!= L7_SUCCESS)
     {
       CLR_VALID(I_agentDhcpSnoopingRemoteFileName, data->valid);
       return COMMIT_FAILED_ERROR;
     }
  }
  if ( VALID(I_agentDhcpSnoopingStoreInterval,data->valid))
  {
     if((cnfgrIsFeaturePresent(L7_DHCP_SNOOPING_COMPONENT_ID,
                               L7_DHCP_SNOOPING_DB_PERSISTENCY)) != L7_TRUE)
     {
       return COMMIT_FAILED_ERROR;
     }
     if (usmDbDsDbStoreIntervalSet (data->agentDhcpSnoopingStoreInterval) != L7_SUCCESS)
     {
       CLR_VALID (I_agentDhcpSnoopingStoreInterval,data->valid);
       return COMMIT_FAILED_ERROR;
     }
  }
#endif

   return NO_ERROR;
}
#ifdef SR_agentDhcpSnoopingConfigGroup_UNDO
/* add #define SR_agentDhcpSnoopingConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentDhcpSnoopingConfigGroup family.
 */
int
agentDhcpSnoopingConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                                  ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentDhcpSnoopingConfigGroup_UNDO */

#endif /* SETS */
#endif /* I_agentDhcpSnoopingAdminMode */

#ifdef I_agentDhcpSnoopingVlanIndex
agentDhcpSnoopingVlanConfigEntry_t *
k_agentDhcpSnoopingVlanConfigEntry_get(int serialNum, ContextInfo *contextInfo,
                                       int nominator,
                                       int searchType,
                                       SR_UINT32 agentDhcpSnoopingVlanIndex)
{
   static agentDhcpSnoopingVlanConfigEntry_t agentDhcpSnoopingVlanConfigEntryData;

  ZERO_VALID(agentDhcpSnoopingVlanConfigEntryData.valid);
  agentDhcpSnoopingVlanConfigEntryData.agentDhcpSnoopingVlanIndex =
                                      agentDhcpSnoopingVlanIndex;
  SET_VALID(I_agentDhcpSnoopingVlanIndex, agentDhcpSnoopingVlanConfigEntryData.valid);

  if ((searchType == EXACT) ?
      (snmpDhcpSnoopingVlanGet(USMDB_UNIT_CURRENT,
                       agentDhcpSnoopingVlanConfigEntryData.agentDhcpSnoopingVlanIndex) != L7_SUCCESS)        :
      ((snmpDhcpSnoopingVlanGet(USMDB_UNIT_CURRENT,
                agentDhcpSnoopingVlanConfigEntryData.agentDhcpSnoopingVlanIndex) != L7_SUCCESS)&&
       (snmpDhcpSnoopingVlanNextGet(USMDB_UNIT_CURRENT,
              &agentDhcpSnoopingVlanConfigEntryData.agentDhcpSnoopingVlanIndex) != L7_SUCCESS)))
  {
    ZERO_VALID(agentDhcpSnoopingVlanConfigEntryData.valid);
    return (NULL);
  }

  switch (nominator)
  {
    case -1:
    case I_agentDhcpSnoopingVlanIndex:
      if (nominator != -1) break;
      /* else pass through */

    case I_agentDhcpSnoopingVlanEnable:
      if(snmpDhcpSnoopingVlanEnableGet(agentDhcpSnoopingVlanConfigEntryData.agentDhcpSnoopingVlanIndex,
                   &agentDhcpSnoopingVlanConfigEntryData.agentDhcpSnoopingVlanEnable) == L7_SUCCESS)
        SET_VALID(I_agentDhcpSnoopingVlanEnable, agentDhcpSnoopingVlanConfigEntryData.valid);
      break;
      /* else pass through */
    default:
      return (NULL);
      break;
  }

  if (nominator >= 0 && !VALID(nominator, agentDhcpSnoopingVlanConfigEntryData.valid))
  {
    
    return(NULL);
  }

  return(&agentDhcpSnoopingVlanConfigEntryData);
}

#ifdef SETS
int
k_agentDhcpSnoopingVlanConfigEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                        doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentDhcpSnoopingVlanConfigEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                         doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentDhcpSnoopingVlanConfigEntry_set_defaults(doList_t *dp)
{
    agentDhcpSnoopingVlanConfigEntry_t *data = (agentDhcpSnoopingVlanConfigEntry_t *) (dp->data);


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentDhcpSnoopingVlanConfigEntry_set(agentDhcpSnoopingVlanConfigEntry_t *data,
                                       ContextInfo *contextInfo, int function)
{

  L7_uint32 vlanId;


  vlanId = data->agentDhcpSnoopingVlanIndex;
  if(snmpDhcpSnoopingVlanGet(USMDB_UNIT_CURRENT, vlanId) != L7_SUCCESS)
  {
    CLR_VALID(I_agentDhcpSnoopingVlanEnable, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentDhcpSnoopingVlanEnable, data->valid) &&
      snmpDhcpSnoopingVlanEnableSet(vlanId, data->agentDhcpSnoopingVlanEnable) != L7_SUCCESS)
  {
      CLR_VALID(I_agentDhcpSnoopingVlanEnable, data->valid);
      return COMMIT_FAILED_ERROR;
  }
  return NO_ERROR;
}
#ifdef SR_agentDhcpSnoopingVlanConfigEntry_UNDO
/* add #define SR_agentDhcpSnoopingVlanConfigEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentDhcpSnoopingVlanConfigEntry family.
 */
int
agentDhcpSnoopingVlanConfigEntry_undo(doList_t *doHead, doList_t *doCur,
                                      ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentDhcpSnoopingVlanConfigEntry_UNDO */

#endif /* SETS */
#endif /* I_agentDhcpSnoopingVlanIndex */

#ifdef I_agentDhcpSnoopingIfConfigEntryIndex_ifIndex
agentDhcpSnoopingIfConfigEntry_t *
k_agentDhcpSnoopingIfConfigEntry_get(int serialNum, ContextInfo *contextInfo,
                                     int nominator,
                                     int searchType,
                                     SR_INT32 ifIndex)
{
  static agentDhcpSnoopingIfConfigEntry_t agentDhcpSnoopingConfigEntryData;
  L7_uint32 intIfNum;

  ZERO_VALID(agentDhcpSnoopingConfigEntryData.valid);
  agentDhcpSnoopingConfigEntryData.ifIndex = ifIndex;
  SET_VALID(I_agentDhcpSnoopingIfConfigEntryIndex_ifIndex, agentDhcpSnoopingConfigEntryData.valid);

  if(((searchType == EXACT) ?
      (snmpDhcpSnoopingIntfGet(agentDhcpSnoopingConfigEntryData.ifIndex) != L7_SUCCESS) :
      ((snmpDhcpSnoopingIntfGet(agentDhcpSnoopingConfigEntryData.ifIndex) != L7_SUCCESS) &&
       (snmpDhcpSnoopingIntfNextGet(agentDhcpSnoopingConfigEntryData.ifIndex, &agentDhcpSnoopingConfigEntryData.ifIndex) != L7_SUCCESS))) ||
     (usmDbIntIfNumFromExtIfNum(agentDhcpSnoopingConfigEntryData.ifIndex, &intIfNum) != L7_SUCCESS))
  {
    ZERO_VALID(agentDhcpSnoopingConfigEntryData.valid);
    return (NULL);
  }

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
  case I_agentDhcpSnoopingIfConfigEntryIndex_ifIndex:
    if ( nominator != -1 ) break;
    /* else pass through */
  case I_agentDhcpSnoopingIfTrustEnable:
    if (snmpDhcpSnoopingPortTrustModeGet(intIfNum, &agentDhcpSnoopingConfigEntryData.agentDhcpSnoopingIfTrustEnable) == L7_SUCCESS)
      SET_VALID(I_agentDhcpSnoopingIfTrustEnable, agentDhcpSnoopingConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentDhcpSnoopingIfLogEnable:
    if (snmpDhcpSnoopingPortLogModeGet(intIfNum, &agentDhcpSnoopingConfigEntryData.agentDhcpSnoopingIfLogEnable) == L7_SUCCESS)
      SET_VALID(I_agentDhcpSnoopingIfLogEnable, agentDhcpSnoopingConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentDhcpSnoopingIfRateLimit:
    if (usmDbDsIntfRateLimitGet(intIfNum, &agentDhcpSnoopingConfigEntryData.agentDhcpSnoopingIfRateLimit) == L7_SUCCESS)
      SET_VALID(I_agentDhcpSnoopingIfRateLimit, agentDhcpSnoopingConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentDhcpSnoopingIfBurstInterval:
    if (usmDbDsIntfBurstIntervalGet(intIfNum,
                                 &agentDhcpSnoopingConfigEntryData.agentDhcpSnoopingIfBurstInterval)
                               == L7_SUCCESS)
      SET_VALID(I_agentDhcpSnoopingIfBurstInterval, agentDhcpSnoopingConfigEntryData.valid);
    break;


  default:
    /* unknown nominator */
    return(NULL);
    break;
  }
  if ( nominator >= 0 && !VALID(nominator, agentDhcpSnoopingConfigEntryData.valid) )
    return(NULL);

  return(&agentDhcpSnoopingConfigEntryData);

}

#ifdef SETS
int
k_agentDhcpSnoopingIfConfigEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                      doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentDhcpSnoopingIfConfigEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                       doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentDhcpSnoopingIfConfigEntry_set_defaults(doList_t *dp)
{
    agentDhcpSnoopingIfConfigEntry_t *data = (agentDhcpSnoopingIfConfigEntry_t *) (dp->data);


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}
int
k_agentDhcpSnoopingIfConfigEntry_set(agentDhcpSnoopingIfConfigEntry_t *data,
                                     ContextInfo *contextInfo, int function)
{

  L7_uint32 intIfNum;
  L7_RC_t rc;

  /* Conversion to internal interface number for snmp call */
  rc = usmDbIntIfNumFromExtIfNum(data->ifIndex, &intIfNum);
  if (rc == L7_SUCCESS)
  {
    if (VALID(I_agentDhcpSnoopingIfTrustEnable, data->valid))
    {
      if(snmpDhcpSnoopingPortTrustModeSet(intIfNum,
                        data->agentDhcpSnoopingIfTrustEnable) != L7_SUCCESS)
      {
        CLR_VALID(I_agentDhcpSnoopingIfTrustEnable, data->valid);
        return COMMIT_FAILED_ERROR;
      }
    }
    if (VALID(I_agentDhcpSnoopingIfLogEnable, data->valid))
    {
      if (snmpDhcpSnoopingPortLogSet (intIfNum,
                      data->agentDhcpSnoopingIfLogEnable) != L7_SUCCESS)
      {
        CLR_VALID(I_agentDhcpSnoopingIfLogEnable, data->valid);
        return COMMIT_FAILED_ERROR;
      }
    }
    if (VALID(I_agentDhcpSnoopingIfRateLimit, data->valid))
    {
      if(usmDbDsIntfRateLimitSet(intIfNum, data->agentDhcpSnoopingIfRateLimit) != L7_SUCCESS)
      {
        CLR_VALID(I_agentDhcpSnoopingIfRateLimit, data->valid);
        return COMMIT_FAILED_ERROR;
      }
    }
    if (VALID(I_agentDhcpSnoopingIfBurstInterval, data->valid))
    {
      if(usmDbDsIntfBurstIntervalSet(intIfNum, data->agentDhcpSnoopingIfBurstInterval) != L7_SUCCESS)
      {
        CLR_VALID(I_agentDhcpSnoopingIfBurstInterval, data->valid);
        return COMMIT_FAILED_ERROR;
      }
    }

    return NO_ERROR;
  }
  
  return COMMIT_FAILED_ERROR;
}
#ifdef SR_agentDhcpSnoopingIfConfigEntry_UNDO
/* add #define SR_agentDhcpSnoopingIfConfigEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentDhcpSnoopingIfConfigEntry family.
 */
int
agentDhcpSnoopingIfConfigEntry_undo(doList_t *doHead, doList_t *doCur,
                                    ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentDhcpSnoopingIfConfigEntry_UNDO */

#endif /* SETS */
#endif /* I_agentDhcpSnoopingIfConfigEntryIndex_ifIndex */

#ifdef I_agentDhcpSnoopingStatsEntryIndex_ifIndex
agentDhcpSnoopingStatsEntry_t *
k_agentDhcpSnoopingStatsEntry_get(int serialNum, ContextInfo *contextInfo,
                                  int nominator,
                                  int searchType,
                                  SR_INT32 ifIndex)
{
   static agentDhcpSnoopingStatsEntry_t agentDhcpSnoopingStatsEntryData;
   dhcpSnoopIntfStats_t stats;
   L7_uint32 intIfNum;

   ZERO_VALID (agentDhcpSnoopingStatsEntryData.valid);
   agentDhcpSnoopingStatsEntryData.ifIndex = ifIndex;
   SET_VALID (I_agentDhcpSnoopingStatsEntryIndex_ifIndex,
                             agentDhcpSnoopingStatsEntryData.valid);


  if(((searchType == EXACT) ?
      (snmpDhcpSnoopingIntfGet(agentDhcpSnoopingStatsEntryData.ifIndex) != L7_SUCCESS) :
      ((snmpDhcpSnoopingIntfGet(agentDhcpSnoopingStatsEntryData.ifIndex) != L7_SUCCESS) &&
       (snmpDhcpSnoopingIntfNextGet(agentDhcpSnoopingStatsEntryData.ifIndex, &agentDhcpSnoopingStatsEntryData.ifIndex) != L7_SUCCESS))) ||
     (usmDbIntIfNumFromExtIfNum(agentDhcpSnoopingStatsEntryData.ifIndex, &intIfNum) != L7_SUCCESS))
  {
    ZERO_VALID(agentDhcpSnoopingStatsEntryData.valid);
    return (NULL);
  }
  if ( usmDbDsIntfStatsGet (intIfNum, &stats) != L7_SUCCESS)
  {
    ZERO_VALID(agentDhcpSnoopingStatsEntryData.valid);
    return (NULL);
  }
  switch (nominator)
  {
  case -1:
  case I_agentDhcpSnoopingStatsEntryIndex_ifIndex:
    if ( nominator != -1 ) break;
    /* else pass through */
  case I_agentDhcpSnoopingMacVerifyFailures:
    agentDhcpSnoopingStatsEntryData.agentDhcpSnoopingMacVerifyFailures =
                                     stats.macVerify;
    SET_VALID ( I_agentDhcpSnoopingMacVerifyFailures,
                       agentDhcpSnoopingStatsEntryData.valid);
    if (nominator != -1) break;
  case I_agentDhcpSnoopingInvalidClientMessages:
    agentDhcpSnoopingStatsEntryData.agentDhcpSnoopingInvalidClientMessages =
                                             stats.intfMismatch;
       SET_VALID ( I_agentDhcpSnoopingInvalidClientMessages,
                       agentDhcpSnoopingStatsEntryData.valid);
         if (nominator != -1) break;
  case I_agentDhcpSnoopingInvalidServerMessages:
    agentDhcpSnoopingStatsEntryData.agentDhcpSnoopingInvalidServerMessages =
                                           stats.untrustedSvrMsg;
     SET_VALID ( I_agentDhcpSnoopingInvalidServerMessages,
                         agentDhcpSnoopingStatsEntryData.valid);
      break;
   default:
      return(NULL);
     break;
  }
   if ( nominator >= 0 && !VALID(nominator,agentDhcpSnoopingStatsEntryData.valid))
      return(NULL);
   return(&agentDhcpSnoopingStatsEntryData);

}
#endif /* I_agentDhcpSnoopingStatsEntryIndex_ifIndex */
                                

#ifdef I_agentIpsgIfConfigEntryIndex_ifIndex
agentIpsgIfConfigEntry_t *
k_agentIpsgIfConfigEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_INT32 ifIndex)
{

  static agentIpsgIfConfigEntry_t agentIpsgIfConfigEntryData;
  L7_uint32 intIfNum;

  ZERO_VALID(agentIpsgIfConfigEntryData.valid);
  agentIpsgIfConfigEntryData.ifIndex = ifIndex;
  SET_VALID(I_agentIpsgIfConfigEntryIndex_ifIndex, agentIpsgIfConfigEntryData.valid);

  if((cnfgrIsFeaturePresent(L7_DHCP_SNOOPING_COMPONENT_ID,
                            L7_DHCP_SNOOPING_IPSG)) != L7_TRUE)
  {
    return (NULL);
  }

  if(((searchType == EXACT) ?
      (snmpDhcpSnoopingIntfGet(agentIpsgIfConfigEntryData.ifIndex) != L7_SUCCESS) :
      ((snmpDhcpSnoopingIntfGet(agentIpsgIfConfigEntryData.ifIndex) != L7_SUCCESS) &&
       (snmpDhcpSnoopingIntfNextGet(agentIpsgIfConfigEntryData.ifIndex, &agentIpsgIfConfigEntryData.ifIndex) != L7_SUCCESS))) ||
     (usmDbIntIfNumFromExtIfNum(agentIpsgIfConfigEntryData.ifIndex, &intIfNum) != L7_SUCCESS))
  {
    ZERO_VALID(agentIpsgIfConfigEntryData.valid);
    return (NULL);
  }
  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
  case I_agentIpsgIfConfigEntryIndex_ifIndex:
    if ( nominator != -1 ) break;
    /* else pass through */
  case I_agentIpsgIfVerifySource:
    if (snmpIpsgIfVerifySourceGet(intIfNum, &agentIpsgIfConfigEntryData.agentIpsgIfVerifySource) == L7_SUCCESS)
      SET_VALID(I_agentIpsgIfVerifySource, agentIpsgIfConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentIpsgIfPortSecurity:
    if (snmpIpsgIfPortSecurityGet(intIfNum, &agentIpsgIfConfigEntryData.agentIpsgIfPortSecurity) == L7_SUCCESS)
      SET_VALID(I_agentIpsgIfPortSecurity, agentIpsgIfConfigEntryData.valid);
     break;
    /* else pass through */
  default:
    /* unknown nominator */
    return(NULL);
    break;
  }
  if ( nominator >= 0 && !VALID(nominator, agentIpsgIfConfigEntryData.valid) )
    return(NULL);

  return(&agentIpsgIfConfigEntryData);
}

#ifdef SETS
int
k_agentIpsgIfConfigEntry_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentIpsgIfConfigEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                               doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentIpsgIfConfigEntry_set_defaults(doList_t *dp)
{
    agentIpsgIfConfigEntry_t *data = (agentIpsgIfConfigEntry_t *) (dp->data);


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentIpsgIfConfigEntry_set(agentIpsgIfConfigEntry_t *data,
                             ContextInfo *contextInfo, int function)
{
  L7_uint32 intIfNum;
  L7_RC_t rc;

  if((cnfgrIsFeaturePresent(L7_DHCP_SNOOPING_COMPONENT_ID,
                            L7_DHCP_SNOOPING_IPSG)) != L7_TRUE)
  {
    return COMMIT_FAILED_ERROR;
  }

  /* Conversion to internal interface number for snmp call */
  rc = usmDbIntIfNumFromExtIfNum(data->ifIndex, &intIfNum);
  if (rc == L7_SUCCESS)
  {
    if ( (VALID(I_agentIpsgIfVerifySource, data->valid)) &&
         (VALID(I_agentIpsgIfPortSecurity, data->valid))
        )
    {
      if( snmpIpsgIfVerifySourceSet(intIfNum,
                        data->agentIpsgIfVerifySource,
                        data->agentIpsgIfPortSecurity) != L7_SUCCESS)
      {
        CLR_VALID(I_agentIpsgIfVerifySource, data->valid);
        CLR_VALID(I_agentIpsgIfPortSecurity, data->valid);
        return COMMIT_FAILED_ERROR;
      }
    }
    return NO_ERROR;
  }
  return COMMIT_FAILED_ERROR;
 
}

#if 0
agentDhcpSnoopingStatsEntry_t *
k_agentDhcpSnoopingStatsEntry_get(int serialNum, ContextInfo *contextInfo,
                                  int nominator,
                                  int searchType,
                                  SR_INT32 ifIndex)
{
   static agentDhcpSnoopingStatsEntry_t agentDhcpSnoopingStatsEntryData;
   dhcpSnoopIntfStats_t stats;
   L7_uint32 intIfNum;

   ZERO_VALID (agentDhcpSnoopingStatsEntryData.valid);
   agentDhcpSnoopingStatsEntryData.ifIndex = ifIndex;
   SET_VALID (I_agentDhcpSnoopingStatsEntryIndex_ifIndex,
                             agentDhcpSnoopingStatsEntryData.valid);


  if(((searchType == EXACT) ?
      (snmpDhcpSnoopingIntfGet(agentDhcpSnoopingStatsEntryData.ifIndex) != L7_SUCCESS) :
      ((snmpDhcpSnoopingIntfGet(agentDhcpSnoopingStatsEntryData.ifIndex) != L7_SUCCESS) &&
       (snmpDhcpSnoopingIntfNextGet(agentDhcpSnoopingStatsEntryData.ifIndex, &agentDhcpSnoopingStatsEntryData.ifIndex) != L7_SUCCESS))) ||
     (usmDbIntIfNumFromExtIfNum(agentDhcpSnoopingStatsEntryData.ifIndex, &intIfNum) != L7_SUCCESS))
  {
    ZERO_VALID(agentDhcpSnoopingStatsEntryData.valid);
    return (NULL);
  }
  if ( usmDbDsIntfStatsGet (intIfNum, &stats) != L7_SUCCESS)
  {
    ZERO_VALID(agentDhcpSnoopingStatsEntryData.valid);
    return (NULL);
  }

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */
  switch (nominator)
  {
  case -1:
  case I_agentDhcpSnoopingStatsEntryIndex_ifIndex:
    if ( nominator != -1 ) break;
    /* else pass through */
  case I_agentDhcpSnoopingMacVerifyFailures:
    agentDhcpSnoopingStatsEntryData.agentDhcpSnoopingMacVerifyFailures =
                                     stats.macVerify;
    SET_VALID ( I_agentDhcpSnoopingMacVerifyFailures, 
                       agentDhcpSnoopingStatsEntryData.valid);
    if (nominator != -1) break;
  case I_agentDhcpSnoopingInvalidClientMessages:
    agentDhcpSnoopingStatsEntryData.agentDhcpSnoopingInvalidClientMessages = 
                                             stats.intfMismatch;  
       SET_VALID ( I_agentDhcpSnoopingInvalidClientMessages,
                       agentDhcpSnoopingStatsEntryData.valid);
         if (nominator != -1) break;
  case I_agentDhcpSnoopingInvalidServerMessages:
    agentDhcpSnoopingStatsEntryData.agentDhcpSnoopingInvalidServerMessages =
                                           stats.untrustedSvrMsg;
     SET_VALID ( I_agentDhcpSnoopingInvalidServerMessages, 
                         agentDhcpSnoopingStatsEntryData.valid);
      break;
   default:
      return(NULL);
     break; 
  }
   if ( nominator >= 0 && !VALID(nominator,agentDhcpSnoopingStatsEntryData.valid))
      return(NULL);
   return(&agentDhcpSnoopingStatsEntryData);

}
#endif

#ifdef SR_agentIpsgIfConfigEntry_UNDO
/* add #define SR_agentIpsgIfConfigEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentIpsgIfConfigEntry family.
 */
int
agentIpsgIfConfigEntry_undo(doList_t *doHead, doList_t *doCur,
                            ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentIpsgIfConfigEntry_UNDO */

#endif  /* SETS */
#endif /* I_agentIpsgIfConfigEntryIndex_ifIndex */

#ifdef I_agentStaticIpsgBindingIfIndex
agentStaticIpsgBinding_t *
k_agentStaticIpsgBinding_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_INT32 agentStaticIpsgBindingIfIndex,
                             SR_UINT32 agentStaticIpsgBindingVlanId,
                             OctetString * agentStaticIpsgBindingMacAddr,
                             SR_UINT32 agentStaticIpsgBindingIpAddr)
{

  static agentStaticIpsgBinding_t agentStaticIpsgBindingData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 mac_buffer[SNMP_BUFFER_LEN];
  L7_char8 name_buffer[SNMP_BUFFER_LEN];
  L7_enetMacAddr_t macAddr;
  static L7_ushort16 vlanIndex;
  L7_uint32 entryType;

  if((cnfgrIsFeaturePresent(L7_DHCP_SNOOPING_COMPONENT_ID,
                            L7_DHCP_SNOOPING_IPSG)) != L7_TRUE)
  {
    return (NULL);
  }

  if(firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    agentStaticIpsgBindingData.agentStaticIpsgBindingMacAddr = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(agentStaticIpsgBindingData.valid);
  bzero(name_buffer, SNMP_BUFFER_LEN);
  bzero(mac_buffer, SNMP_BUFFER_LEN);
  memcpy(name_buffer, agentStaticIpsgBindingMacAddr->octet_ptr,
                     agentStaticIpsgBindingMacAddr->length);

  if(! snmpConvertMacToString(name_buffer, mac_buffer))
  {
    ZERO_VALID(agentStaticIpsgBindingData.valid);
    return (NULL);
  }

  agentStaticIpsgBindingData.agentStaticIpsgBindingIfIndex = agentStaticIpsgBindingIfIndex;
  SET_VALID(I_agentStaticIpsgBindingIfIndex, agentStaticIpsgBindingData.valid);

  agentStaticIpsgBindingData.agentStaticIpsgBindingVlanId = agentStaticIpsgBindingVlanId;
  SET_VALID(I_agentStaticIpsgBindingVlanId , agentStaticIpsgBindingData.valid);

  agentStaticIpsgBindingData.agentStaticIpsgBindingIpAddr = agentStaticIpsgBindingIpAddr;
  SET_VALID(I_agentStaticIpsgBindingIpAddr, agentStaticIpsgBindingData.valid);

  snmpConvertStringToMac ( mac_buffer,macAddr.addr);

  entryType = IPSG_ENTRY_STATIC;

  if((searchType == EXACT) ?
     (usmDbIpsgBindingGetByType(&agentStaticIpsgBindingData.agentStaticIpsgBindingIfIndex, 
                          &vlanIndex,
                          &agentStaticIpsgBindingData.agentStaticIpsgBindingIpAddr,
                          &macAddr, entryType) != L7_SUCCESS) :
     (( usmDbIpsgBindingGetByType(&agentStaticIpsgBindingData.agentStaticIpsgBindingIfIndex, 
                          &vlanIndex,
                          &agentStaticIpsgBindingData.agentStaticIpsgBindingIpAddr,
                          &macAddr, entryType) != L7_SUCCESS) &&
      (usmDbIpsgBindingGetNextByType(&agentStaticIpsgBindingData.agentStaticIpsgBindingIfIndex, 
                          &vlanIndex,
                          &agentStaticIpsgBindingData.agentStaticIpsgBindingIpAddr,
                          &macAddr, entryType) != L7_SUCCESS)))
  {
    ZERO_VALID(agentStaticIpsgBindingData.valid);
    return (NULL);
  }
  else
  {

   agentStaticIpsgBindingData.agentStaticIpsgBindingVlanId = vlanIndex;

    bzero(name_buffer, SNMP_BUFFER_LEN);

    snmpConvertMacToString (macAddr.addr,mac_buffer);
    
    if(snmpConvertStringToMac(mac_buffer, name_buffer) != L7_SUCCESS)
    {
      ZERO_VALID(agentStaticIpsgBindingData.valid);
      return (NULL);
    }
    if(SafeMakeOctetString(&( agentStaticIpsgBindingData.agentStaticIpsgBindingMacAddr),
                            name_buffer, L7_ENET_MAC_ADDR_LEN) == L7_TRUE)
    {
      SET_VALID(I_agentStaticIpsgBindingMacAddr, agentStaticIpsgBindingData.valid);
    }
    else
    {
      ZERO_VALID(agentStaticIpsgBindingData.valid);
      return (NULL);
    }
  }

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
  case I_agentStaticIpsgBindingIfIndex:
  case I_agentStaticIpsgBindingVlanId:
  case I_agentStaticIpsgBindingIpAddr:
  case I_agentStaticIpsgBindingMacAddr:
    if (nominator != -1) break;
    /* else pass through */

  case I_agentStaticIpsgBindingRowStatus:
    agentStaticIpsgBindingData.agentStaticIpsgBindingRowStatus = D_agentStaticIpsgBindingRowStatus_active;
    SET_VALID(I_agentStaticIpsgBindingRowStatus, agentStaticIpsgBindingData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentStaticIpsgBindingData.valid) )
    return(NULL);
  return(&agentStaticIpsgBindingData);

}

#ifdef SETS
int
k_agentStaticIpsgBinding_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentStaticIpsgBinding_ready(ObjectInfo *object, ObjectSyntax *value,
                               doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentStaticIpsgBinding_set_defaults(doList_t *dp)
{
    agentStaticIpsgBinding_t *data = (agentStaticIpsgBinding_t *) (dp->data);


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentStaticIpsgBinding_set(agentStaticIpsgBinding_t *data,
                             ContextInfo *contextInfo, int function)
{
  char snmp_buffer[SNMP_BUFFER_LEN];
  char mac_buffer[SNMP_BUFFER_LEN];

  if((cnfgrIsFeaturePresent(L7_DHCP_SNOOPING_COMPONENT_ID,
                            L7_DHCP_SNOOPING_IPSG)) != L7_TRUE)
  {
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentStaticIpsgBindingRowStatus, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    bzero(mac_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentStaticIpsgBindingMacAddr->octet_ptr,
                       data->agentStaticIpsgBindingMacAddr->length);
    if(! snmpConvertMacToString(snmp_buffer, mac_buffer))
    {
      CLR_VALID(I_agentStaticIpsgBindingRowStatus, data->valid);
      return COMMIT_FAILED_ERROR;
    }
   
    if (snmpStaticIpsgBindingRowStatusSet(data->agentStaticIpsgBindingIfIndex,
                                          data->agentStaticIpsgBindingVlanId,
                                          mac_buffer,
                                          data->agentStaticIpsgBindingIpAddr,
                                          data->agentStaticIpsgBindingRowStatus) != L7_SUCCESS)
    {
      CLR_VALID(I_agentStaticIpsgBindingRowStatus, data->valid);
      return COMMIT_FAILED_ERROR;
    }
  }
  return NO_ERROR;
}

#ifdef SR_agentStaticIpsgBinding_UNDO
/* add #define SR_agentStaticIpsgBinding_UNDO in sitedefs.h to
 * include the undo routine for the agentStaticIpsgBinding family.
 */
int
agentStaticIpsgBinding_undo(doList_t *doHead, doList_t *doCur,
                            ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentStaticIpsgBinding_UNDO */

#endif /* SETS */
#endif /* I_agentStaticIpsgBindingIfIndex */

#ifdef I_agentDynamicIpsgBindingIfIndex
agentDynamicIpsgBinding_t *
k_agentDynamicIpsgBinding_get(int serialNum, ContextInfo *contextInfo,
                              int nominator,
                              int searchType,
                              SR_INT32 agentDynamicIpsgBindingIfIndex,
                              SR_UINT32 agentDynamicIpsgBindingVlanId,
                              OctetString * agentDynamicIpsgBindingMacAddr,
                              SR_UINT32 agentDynamicIpsgBindingIpAddr)
{


  static agentDynamicIpsgBinding_t agentDynamicIpsgBindingData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 mac_buffer[SNMP_BUFFER_LEN];
  L7_char8 name_buffer[SNMP_BUFFER_LEN];
  L7_enetMacAddr_t macAddr;
  static L7_ushort16 vlanIndex;
  L7_uint32 entryType;

  if((cnfgrIsFeaturePresent(L7_DHCP_SNOOPING_COMPONENT_ID,
                            L7_DHCP_SNOOPING_IPSG)) != L7_TRUE)
  {
    return (NULL);
  }

  if(firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    agentDynamicIpsgBindingData.agentDynamicIpsgBindingMacAddr = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(agentDynamicIpsgBindingData.valid);
  bzero(name_buffer, SNMP_BUFFER_LEN);
  bzero(mac_buffer, SNMP_BUFFER_LEN);
  memcpy(name_buffer, agentDynamicIpsgBindingMacAddr->octet_ptr,
                     agentDynamicIpsgBindingMacAddr->length);

  if(! snmpConvertMacToString(name_buffer, mac_buffer))
  {
    ZERO_VALID(agentDynamicIpsgBindingData.valid);
    return (NULL);
  }
  agentDynamicIpsgBindingData.agentDynamicIpsgBindingIfIndex = agentDynamicIpsgBindingIfIndex;
  SET_VALID(I_agentDynamicIpsgBindingIfIndex, agentDynamicIpsgBindingData.valid);

  agentDynamicIpsgBindingData.agentDynamicIpsgBindingVlanId = agentDynamicIpsgBindingVlanId;
  SET_VALID(I_agentDynamicIpsgBindingVlanId , agentDynamicIpsgBindingData.valid);

  agentDynamicIpsgBindingData.agentDynamicIpsgBindingIpAddr = agentDynamicIpsgBindingIpAddr;
  SET_VALID(I_agentDynamicIpsgBindingIpAddr, agentDynamicIpsgBindingData.valid);

  snmpConvertStringToMac ( mac_buffer,macAddr.addr);

  entryType = IPSG_ENTRY_DYNAMIC;
  
  if((searchType == EXACT) ?
     (usmDbIpsgBindingGetByType(&agentDynamicIpsgBindingData.agentDynamicIpsgBindingIfIndex,
                          &vlanIndex,
                          &agentDynamicIpsgBindingData.agentDynamicIpsgBindingIpAddr,
                          &macAddr, entryType) != L7_SUCCESS) :
     (( usmDbIpsgBindingGetByType(&agentDynamicIpsgBindingData.agentDynamicIpsgBindingIfIndex,
                          &vlanIndex,
                          &agentDynamicIpsgBindingData.agentDynamicIpsgBindingIpAddr,
                          &macAddr, entryType) != L7_SUCCESS) &&
      (usmDbIpsgBindingGetNextByType(&agentDynamicIpsgBindingData.agentDynamicIpsgBindingIfIndex,
                          &vlanIndex,
                          &agentDynamicIpsgBindingData.agentDynamicIpsgBindingIpAddr,
                          &macAddr, entryType) != L7_SUCCESS)))
  {
    ZERO_VALID(agentDynamicIpsgBindingData.valid);
    return (NULL);
  }
  else
  {

   agentDynamicIpsgBindingData.agentDynamicIpsgBindingVlanId = vlanIndex;

    bzero(name_buffer, SNMP_BUFFER_LEN);
    bzero(mac_buffer, SNMP_BUFFER_LEN);

    snmpConvertMacToString (macAddr.addr,mac_buffer);

    if(snmpConvertStringToMac(mac_buffer, name_buffer) != L7_SUCCESS)
    {
      ZERO_VALID(agentDynamicIpsgBindingData.valid);
      return (NULL);
    }
    if(SafeMakeOctetString(&( agentDynamicIpsgBindingData.agentDynamicIpsgBindingMacAddr),
                            name_buffer, L7_ENET_MAC_ADDR_LEN) == L7_TRUE)
    {
      SET_VALID(I_agentDynamicIpsgBindingMacAddr, agentDynamicIpsgBindingData.valid);
    }
    else
    {
      ZERO_VALID(agentDynamicIpsgBindingData.valid);
      return (NULL);
    }
  }

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
  case I_agentDynamicIpsgBindingIfIndex:
  case I_agentDynamicIpsgBindingVlanId:
  case I_agentDynamicIpsgBindingIpAddr:
  case I_agentDynamicIpsgBindingMacAddr:
   break;
  
  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentDynamicIpsgBindingData.valid) )
    return(NULL);
  return(&agentDynamicIpsgBindingData);


}
#endif /* I_agentDynamicIpsgBindingIfIndex */

#ifdef I_agentStaticDsBindingMacAddr
agentStaticDsBinding_t *
k_agentStaticDsBinding_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           OctetString * agentStaticDsBindingMacAddr)
{
  static agentStaticDsBinding_t agentStaticDsBindingData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 mac_buffer[SNMP_BUFFER_LEN];
  L7_char8 name_buffer[SNMP_BUFFER_LEN];
  L7_enetMacAddr_t macAddr;
  L7_uint32 entryType;
  dhcpSnoopBinding_t binding;

  if(firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    agentStaticDsBindingData.agentStaticDsBindingMacAddr = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(agentStaticDsBindingData.valid);
  bzero(name_buffer, SNMP_BUFFER_LEN);
  bzero(mac_buffer, SNMP_BUFFER_LEN);
  memcpy(name_buffer, agentStaticDsBindingMacAddr->octet_ptr,
                     agentStaticDsBindingMacAddr->length);

  if(! snmpConvertMacToString(name_buffer, mac_buffer))
  {
    ZERO_VALID(agentStaticDsBindingData.valid);
    return (NULL);
  }

  snmpConvertStringToMac ( mac_buffer,macAddr.addr);

  entryType = DS_BINDING_STATIC;

  memset (&binding, '\0', sizeof(dhcpSnoopBinding_t));
  memcpy(binding.macAddr,macAddr.addr, L7_MAC_ADDR_LEN); 


  if((searchType == EXACT) ?
     (usmDbDsBindingGetByType(&binding,entryType) != L7_SUCCESS) :
     (( usmDbDsBindingGetByType(&binding,entryType) != L7_SUCCESS) &&
      (usmDbDsBindingGetNextByType(&binding,entryType) != L7_SUCCESS)))
  {
    ZERO_VALID(agentStaticDsBindingData.valid);
    return (NULL);
  }
  else
  {
   agentStaticDsBindingData.agentStaticDsBindingIfIndex = binding.intIfNum;
   agentStaticDsBindingData.agentStaticDsBindingVlanId = binding.vlanId;
   agentStaticDsBindingData.agentStaticDsBindingIpAddr = binding.ipAddr;

   SET_VALID(I_agentStaticDsBindingIfIndex, agentStaticDsBindingData.valid);
   SET_VALID(I_agentStaticDsBindingVlanId , agentStaticDsBindingData.valid);
   SET_VALID(I_agentStaticDsBindingIpAddr, agentStaticDsBindingData.valid);

    bzero(name_buffer, SNMP_BUFFER_LEN);
    bzero(mac_buffer, SNMP_BUFFER_LEN);

    snmpConvertMacToString (binding.macAddr,mac_buffer); 

    if(snmpConvertStringToMac(mac_buffer, name_buffer) != L7_SUCCESS)
    {
      ZERO_VALID(agentStaticDsBindingData.valid);
      return (NULL);
    }
    if(SafeMakeOctetString(&( agentStaticDsBindingData.agentStaticDsBindingMacAddr),
                            name_buffer, L7_ENET_MAC_ADDR_LEN) == L7_TRUE)
    {
      SET_VALID(I_agentStaticDsBindingMacAddr, agentStaticDsBindingData.valid);
    }
    else
    {
      ZERO_VALID(agentStaticDsBindingData.valid);
      return (NULL);
    }
  }

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
  case I_agentStaticDsBindingIfIndex:
  case I_agentStaticDsBindingVlanId:
  case I_agentStaticDsBindingIpAddr:
  case I_agentStaticDsBindingMacAddr:
     break;
    /* else pass through */

  case I_agentStaticDsBindingRowStatus:
    agentStaticDsBindingData.agentStaticDsBindingRowStatus = D_agentStaticDsBindingRowStatus_active;
    SET_VALID(I_agentStaticDsBindingRowStatus, agentStaticDsBindingData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentStaticDsBindingData.valid) )
    return(NULL);
  return(&agentStaticDsBindingData);

}

#ifdef SETS
int
k_agentStaticDsBinding_test(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentStaticDsBinding_ready(ObjectInfo *object, ObjectSyntax *value,
                             doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}


int
k_agentStaticDsBinding_set_defaults(doList_t *dp)
{
    agentStaticDsBinding_t *data = (agentStaticDsBinding_t *) (dp->data);


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentStaticDsBinding_set(agentStaticDsBinding_t *data,
                           ContextInfo *contextInfo, int function)
{
   char snmp_buffer[SNMP_BUFFER_LEN];
   char mac_buffer[SNMP_BUFFER_LEN];

  if (VALID(I_agentStaticDsBindingRowStatus, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    bzero(mac_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentStaticDsBindingMacAddr->octet_ptr,
                       data->agentStaticDsBindingMacAddr->length);
    if(! snmpConvertMacToString(snmp_buffer, mac_buffer))
    {
      CLR_VALID(I_agentStaticDsBindingRowStatus, data->valid);
      return COMMIT_FAILED_ERROR;
    }

    if (snmpStaticDsBindingRowStatusSet(mac_buffer,data->agentStaticDsBindingIpAddr,
                                        data->agentStaticDsBindingVlanId,
                                        data->agentStaticDsBindingIfIndex,
                                        data->agentStaticDsBindingRowStatus) != L7_SUCCESS)
    {
      CLR_VALID(I_agentStaticDsBindingRowStatus, data->valid);
      return COMMIT_FAILED_ERROR;
    }
  }
  return NO_ERROR;
}


#ifdef SR_agentStaticDsBinding_UNDO
/* add #define SR_agentStaticDsBinding_UNDO in sitedefs.h to
 * include the undo routine for the agentStaticDsBinding family.
 */
int
agentStaticDsBinding_undo(doList_t *doHead, doList_t *doCur,
                          ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentStaticDsBinding_UNDO */

#endif /* SETS */
#endif /* I_agentStaticDsBindingMacAddr */

#ifdef I_agentDynamicDsBindingMacAddr
agentDynamicDsBinding_t *
k_agentDynamicDsBinding_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           OctetString * agentDynamicDsBindingMacAddr)
{
  static agentDynamicDsBinding_t agentDynamicDsBindingData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 mac_buffer[SNMP_BUFFER_LEN];
  L7_char8 name_buffer[SNMP_BUFFER_LEN];
  L7_enetMacAddr_t macAddr;
  L7_uint32 entryType;
  dhcpSnoopBinding_t binding;

  if(firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    agentDynamicDsBindingData.agentDynamicDsBindingMacAddr = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(agentDynamicDsBindingData.valid);
  bzero(name_buffer, SNMP_BUFFER_LEN);
  bzero(mac_buffer, SNMP_BUFFER_LEN);
  memcpy(name_buffer, agentDynamicDsBindingMacAddr->octet_ptr,
                     agentDynamicDsBindingMacAddr->length);

  if(! snmpConvertMacToString(name_buffer, mac_buffer))
  {
    ZERO_VALID(agentDynamicDsBindingData.valid);
    return (NULL);
  }

 /*  agentDynamicDsBindingData.agentDynamicDsBindingIfIndex = agentDynamicDsBindingIfIndex;
  SET_VALID(I_agentDynamicDsBindingIfIndex, agentDynamicDsBindingData.valid);

  agentDynamicDsBindingData.agentDynamicDsBindingVlanId = agentDynamicDsBindingVlanId;
  SET_VALID(I_agentDynamicDsBindingVlanId , agentDynamicDsBindingData.valid);

  agentDynamicDsBindingData.agentDynamicDsBindingIpAddr = agentDynamicDsBindingIpAddr;
  SET_VALID(I_agentDynamicDsBindingIpAddr, agentDynamicDsBindingData.valid); */

  snmpConvertStringToMac ( mac_buffer,macAddr.addr);

  entryType = DS_BINDING_DYNAMIC;
  memcpy(binding.macAddr, macAddr.addr, L7_MAC_ADDR_LEN);

  if((searchType == EXACT) ?
     (usmDbDsBindingGetByType(&binding,entryType) != L7_SUCCESS) :
     (( usmDbDsBindingGetByType(&binding,entryType) != L7_SUCCESS) &&
      (usmDbDsBindingGetNextByType(&binding,entryType) != L7_SUCCESS)))
  {
    ZERO_VALID(agentDynamicDsBindingData.valid);
    return (NULL);
  }
  else
  {
   agentDynamicDsBindingData.agentDynamicDsBindingIfIndex = binding.intIfNum;
   agentDynamicDsBindingData.agentDynamicDsBindingVlanId = binding.vlanId;
   agentDynamicDsBindingData.agentDynamicDsBindingIpAddr = binding.ipAddr;
   /* convert remaining time to timeticks (100th of a second) as it is returned in seconds */
   agentDynamicDsBindingData.agentDynamicDsBindingLeaseRemainingTime = binding.remLease * 100;

   SET_VALID(I_agentDynamicDsBindingIfIndex, agentDynamicDsBindingData.valid);
   SET_VALID(I_agentDynamicDsBindingVlanId , agentDynamicDsBindingData.valid);
   SET_VALID(I_agentDynamicDsBindingIpAddr, agentDynamicDsBindingData.valid); 
   SET_VALID(I_agentDynamicDsBindingLeaseRemainingTime, agentDynamicDsBindingData.valid); 

    bzero(name_buffer, SNMP_BUFFER_LEN);
    bzero(mac_buffer, SNMP_BUFFER_LEN);

    snmpConvertMacToString (binding.macAddr,mac_buffer);

    if(snmpConvertStringToMac(mac_buffer, name_buffer) != L7_SUCCESS)
    {
      ZERO_VALID(agentDynamicDsBindingData.valid);
      return (NULL);
    }
    if(SafeMakeOctetString(&( agentDynamicDsBindingData.agentDynamicDsBindingMacAddr),
                            name_buffer, L7_ENET_MAC_ADDR_LEN) == L7_TRUE)
    {
      SET_VALID(I_agentDynamicDsBindingMacAddr, agentDynamicDsBindingData.valid);
    }
    else
    {
      ZERO_VALID(agentDynamicDsBindingData.valid);
      return (NULL);
    }
  }

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
  case I_agentDynamicDsBindingIfIndex:
  case I_agentDynamicDsBindingVlanId:
  case I_agentDynamicDsBindingIpAddr:
  case I_agentDynamicDsBindingMacAddr:
  case I_agentDynamicDsBindingLeaseRemainingTime:
  break;

 

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentDynamicDsBindingData.valid) )
    return(NULL);
  return(&agentDynamicDsBindingData);

}
#endif /* I_agentDynamicDsBindingMacAddr */
#ifdef L7_DHCP_L2_RELAY_PACKAGE
/*********************** DHCP L2 RELAY APIs ****************************/
#ifdef I_agentDhcpL2RelayAdminMode
agentDhcpL2RelayConfigGroup_t *
k_agentDhcpL2RelayConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                                   int nominator)
{
   static agentDhcpL2RelayConfigGroup_t agentDhcpL2RelayConfigGroupData;
   static L7_BOOL firstTime = L7_TRUE;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
  }

   
   ZERO_VALID(agentDhcpL2RelayConfigGroupData.valid);
   
   /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
    if (nominator != -1) break;
    /* else pass through */
  case I_agentDhcpL2RelayAdminMode:
      if (snmpDhcpL2RelayAdminModeGet(&agentDhcpL2RelayConfigGroupData.agentDhcpL2RelayAdminMode)
                                      == L7_SUCCESS)
      SET_VALID(I_agentDhcpL2RelayAdminMode,
                agentDhcpL2RelayConfigGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */
  case I_agentDhcpSnoopingStatsReset :
    agentDhcpL2RelayConfigGroupData.agentDhcpL2RelayStatsReset =
                                              D_agentDhcpL2RelayStatsReset_none;
    SET_VALID(I_agentDhcpL2RelayStatsReset, agentDhcpL2RelayConfigGroupData.valid);
  break;
  default:
    /* unknown nominator */
    return(NULL);
    break;
  }
  if ( nominator >= 0 && !VALID(nominator, agentDhcpL2RelayConfigGroupData.valid) )
    return(NULL);

   return(&agentDhcpL2RelayConfigGroupData);
}

#ifdef SETS
int
k_agentDhcpL2RelayConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                    doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentDhcpL2RelayConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                                     doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentDhcpL2RelayConfigGroup_set(agentDhcpL2RelayConfigGroup_t *data,
                                   ContextInfo *contextInfo, int function)
{
  if (VALID(I_agentDhcpL2RelayAdminMode, data->valid) &&
      snmpDhcpL2RelayAdminModeSet(data->agentDhcpL2RelayAdminMode) != L7_SUCCESS)
  {
      CLR_VALID(I_agentDhcpL2RelayAdminMode, data->valid);
      return COMMIT_FAILED_ERROR;
  }
  if (VALID(I_agentDhcpL2RelayStatsReset, data->valid) &&
      (data->agentDhcpL2RelayStatsReset == D_agentDhcpL2RelayStatsReset_reset) &&
      snmpDhcpL2RelayIntfStatsClear() != L7_SUCCESS)
  {
      CLR_VALID(I_agentDhcpL2RelayStatsReset, data->valid);
      return COMMIT_FAILED_ERROR;
  }
  return NO_ERROR;
}
#ifdef SR_agentDhcpL2RelayConfigGroup_UNDO
/* add #define SR_agentDhcpL2RelayConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentDhcpL2RelayConfigGroup family.
 */
int
agentDhcpL2RelayConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                                  ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentDhcpL2RelayConfigGroup_UNDO */

#endif /* L7_SETS. */

agentDhcpL2RelayIfConfigEntry_t *
k_agentDhcpL2RelayIfConfigEntry_get(int serialNum, ContextInfo *contextInfo,
                                     int nominator,
                                     int searchType,
                                     SR_INT32 ifIndex)
{
  static agentDhcpL2RelayIfConfigEntry_t agentDhcpL2RelayConfigEntryData;
  L7_uint32 intIfNum;

  ZERO_VALID(agentDhcpL2RelayConfigEntryData.valid);
  agentDhcpL2RelayConfigEntryData.ifIndex = ifIndex;
  SET_VALID(I_agentDhcpL2RelayIfConfigEntryIndex_ifIndex, agentDhcpL2RelayConfigEntryData.valid);

  if(((searchType == EXACT) ?
      (snmpDhcpL2RelayIntfGet(agentDhcpL2RelayConfigEntryData.ifIndex) != L7_SUCCESS) :
      ((snmpDhcpL2RelayIntfGet(agentDhcpL2RelayConfigEntryData.ifIndex) != L7_SUCCESS) &&
       (snmpDhcpL2RelayIntfNextGet(agentDhcpL2RelayConfigEntryData.ifIndex, &agentDhcpL2RelayConfigEntryData.ifIndex) != L7_SUCCESS))) ||
     (usmDbIntIfNumFromExtIfNum(agentDhcpL2RelayConfigEntryData.ifIndex, &intIfNum) != L7_SUCCESS))
  {
    ZERO_VALID(agentDhcpL2RelayConfigEntryData.valid);
    return (NULL);
  }

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
  case I_agentDhcpL2RelayIfConfigEntryIndex_ifIndex:
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentDhcpL2RelayIfEnable:
    if (snmpDhcpL2RelayPortModeGet(intIfNum, &agentDhcpL2RelayConfigEntryData.agentDhcpL2RelayIfEnable) == L7_SUCCESS)
      SET_VALID(I_agentDhcpL2RelayIfEnable, agentDhcpL2RelayConfigEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentDhcpL2RelayIfTrustEnable:
    if (snmpDhcpL2RelayPortTrustModeGet(intIfNum, &agentDhcpL2RelayConfigEntryData.agentDhcpL2RelayIfTrustEnable) == L7_SUCCESS)
      SET_VALID(I_agentDhcpL2RelayIfTrustEnable, agentDhcpL2RelayConfigEntryData.valid);
    break;
    /* else pass through */

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }
  if ( nominator >= 0 && !VALID(nominator, agentDhcpL2RelayConfigEntryData.valid) )
    return(NULL);

  return(&agentDhcpL2RelayConfigEntryData);

}

#ifdef SETS
int
k_agentDhcpL2RelayIfConfigEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                      doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentDhcpL2RelayIfConfigEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                       doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentDhcpL2RelayIfConfigEntry_set_defaults(doList_t *dp)
{
    agentDhcpL2RelayIfConfigEntry_t *data = (agentDhcpL2RelayIfConfigEntry_t *) (dp->data);


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}
int
k_agentDhcpL2RelayIfConfigEntry_set(agentDhcpL2RelayIfConfigEntry_t *data,
                                     ContextInfo *contextInfo, int function)
{

  L7_uint32 intIfNum;
  L7_RC_t rc;

  /* Conversion to internal interface number for snmp call */
  rc = usmDbIntIfNumFromExtIfNum(data->ifIndex, &intIfNum);
  if (rc == L7_SUCCESS)
  {
    if (VALID(I_agentDhcpL2RelayIfEnable, data->valid))
    {
      if (snmpDhcpL2RelayPortModeSet (intIfNum,
                      data->agentDhcpL2RelayIfEnable) != L7_SUCCESS)
      {
        CLR_VALID(I_agentDhcpL2RelayIfEnable, data->valid);
        return COMMIT_FAILED_ERROR;
      }
    }
    if (VALID(I_agentDhcpL2RelayIfTrustEnable, data->valid))
    {
      if(snmpDhcpL2RelayPortTrustModeSet(intIfNum,
                        data->agentDhcpL2RelayIfTrustEnable) != L7_SUCCESS)
      {
        CLR_VALID(I_agentDhcpL2RelayIfTrustEnable, data->valid);
        return COMMIT_FAILED_ERROR;
      }
    }
    return NO_ERROR;
  }
  
  return COMMIT_FAILED_ERROR;
}
#ifdef SR_agentDhcpL2RelayIfConfigEntry_UNDO
/* add #define SR_agentDhcpL2RelayIfConfigEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentDhcpL2RelayIfConfigEntry family.
 */
int
agentDhcpL2RelayIfConfigEntry_undo(doList_t *doHead, doList_t *doCur,
                                    ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentDhcpL2RelayIfConfigEntry_UNDO */

#endif /* SETS */

agentDhcpL2RelayVlanConfigEntry_t *
k_agentDhcpL2RelayVlanConfigEntry_get(int serialNum, ContextInfo *contextInfo,
                                       int nominator,
                                       int searchType,
                                       SR_UINT32 agentDhcpL2RelayVlanIndex)
{
   static agentDhcpL2RelayVlanConfigEntry_t agentDhcpL2RelayVlanConfigEntryData;

  ZERO_VALID(agentDhcpL2RelayVlanConfigEntryData.valid);
  agentDhcpL2RelayVlanConfigEntryData.agentDhcpL2RelayVlanIndex =
                                      agentDhcpL2RelayVlanIndex;
  SET_VALID(I_agentDhcpL2RelayVlanIndex, agentDhcpL2RelayVlanConfigEntryData.valid);

  if ((searchType == EXACT) ?
      (snmpDhcpL2RelayVlanGet(USMDB_UNIT_CURRENT,
                       agentDhcpL2RelayVlanConfigEntryData.agentDhcpL2RelayVlanIndex) != L7_SUCCESS)        :
      ((snmpDhcpL2RelayVlanGet(USMDB_UNIT_CURRENT,
                agentDhcpL2RelayVlanConfigEntryData.agentDhcpL2RelayVlanIndex) != L7_SUCCESS)&&
       (snmpDhcpL2RelayVlanNextGet(USMDB_UNIT_CURRENT,
              &agentDhcpL2RelayVlanConfigEntryData.agentDhcpL2RelayVlanIndex) != L7_SUCCESS)))
  {
    ZERO_VALID(agentDhcpL2RelayVlanConfigEntryData.valid);
    return (NULL);
  }

  switch (nominator)
  {
    case -1:
    case I_agentDhcpL2RelayVlanIndex:
      if (nominator != -1) break;
      /* else pass through */

  case I_agentDhcpL2RelayVlanEnable:
      if(snmpDhcpL2RelayVlanEnableGet(agentDhcpL2RelayVlanConfigEntryData.agentDhcpL2RelayVlanIndex,
                   &agentDhcpL2RelayVlanConfigEntryData.agentDhcpL2RelayVlanEnable) == L7_SUCCESS)
        SET_VALID(I_agentDhcpL2RelayVlanEnable, agentDhcpL2RelayVlanConfigEntryData.valid);
      if ( nominator != -1 ) break;
      /* else pass through */

  case I_agentDhcpL2RelayCircuitIdVlanEnable:
      if(snmpDhcpL2RelayCIdVlanEnableGet(agentDhcpL2RelayVlanConfigEntryData.agentDhcpL2RelayVlanIndex,
                   &agentDhcpL2RelayVlanConfigEntryData.agentDhcpL2RelayCircuitIdVlanEnable) == L7_SUCCESS)
        SET_VALID(I_agentDhcpL2RelayCircuitIdVlanEnable, agentDhcpL2RelayVlanConfigEntryData.valid);
      if ( nominator != -1 ) break;
      /* else pass through */

  case I_agentDhcpL2RelayRemoteIdVlanEnable:
      if(snmpDhcpL2RelayRIdVlanEnableGet(agentDhcpL2RelayVlanConfigEntryData.agentDhcpL2RelayVlanIndex,
                   &agentDhcpL2RelayVlanConfigEntryData.agentDhcpL2RelayRemoteIdVlanEnable) == L7_SUCCESS)
        SET_VALID(I_agentDhcpL2RelayRemoteIdVlanEnable, agentDhcpL2RelayVlanConfigEntryData.valid);
      break;
      /* else pass through */

    default:
      return (NULL);
      break;
  }

  if (nominator >= 0 && !VALID(nominator, agentDhcpL2RelayVlanConfigEntryData.valid))
  {
    
    return(NULL);
  }

  return(&agentDhcpL2RelayVlanConfigEntryData);
}

#ifdef SETS
int
k_agentDhcpL2RelayVlanConfigEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                        doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentDhcpL2RelayVlanConfigEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                         doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentDhcpL2RelayVlanConfigEntry_set_defaults(doList_t *dp)
{
    agentDhcpL2RelayVlanConfigEntry_t *data = (agentDhcpL2RelayVlanConfigEntry_t *) (dp->data);


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentDhcpL2RelayVlanConfigEntry_set(agentDhcpL2RelayVlanConfigEntry_t *data,
                                       ContextInfo *contextInfo, int function)
{

  L7_uint32 vlanId;


  vlanId = data->agentDhcpL2RelayVlanIndex;
  if(snmpDhcpL2RelayVlanGet(USMDB_UNIT_CURRENT, vlanId) != L7_SUCCESS)
  {
    CLR_VALID(I_agentDhcpL2RelayVlanEnable, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentDhcpL2RelayVlanEnable, data->valid) &&
      snmpDhcpL2RelayVlanEnableSet(vlanId, data->agentDhcpL2RelayVlanEnable) != L7_SUCCESS)
  {
      CLR_VALID(I_agentDhcpL2RelayVlanEnable, data->valid);
      return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentDhcpL2RelayCircuitIdVlanEnable, data->valid) &&
      snmpDhcpL2RelayCIdVlanEnableSet(vlanId, data->agentDhcpL2RelayCircuitIdVlanEnable) != L7_SUCCESS)
  {
      CLR_VALID(I_agentDhcpL2RelayCircuitIdVlanEnable, data->valid);
      return COMMIT_FAILED_ERROR;
  }
  if (VALID(I_agentDhcpL2RelayRemoteIdVlanEnable, data->valid) &&
      snmpDhcpL2RelayRIdVlanEnableSet(vlanId, data->agentDhcpL2RelayRemoteIdVlanEnable) != L7_SUCCESS)
  {
      CLR_VALID(I_agentDhcpL2RelayRemoteIdVlanEnable, data->valid);
      return COMMIT_FAILED_ERROR;
  }
  return NO_ERROR;
}
#ifdef SR_agentDhcpL2RelayVlanConfigEntry_UNDO
/* add #define SR_agentDhcpL2RelayVlanConfigEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentDhcpL2RelayVlanConfigEntry family.
 */
int
agentDhcpL2RelayVlanConfigEntry_undo(doList_t *doHead, doList_t *doCur,
                                      ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentDhcpL2RelayVlanConfigEntry_UNDO */

#endif /* SETS */

agentDhcpL2RelayStatsEntry_t *
k_agentDhcpL2RelayStatsEntry_get(int serialNum, ContextInfo *contextInfo,
                                  int nominator,
                                  int searchType,
                                  SR_INT32 ifIndex)
{
   static agentDhcpL2RelayStatsEntry_t agentDhcpL2RelayStatsEntryData;
   dsL2RelayIntfStats_t stats;
   L7_uint32 intIfNum;

   ZERO_VALID (agentDhcpL2RelayStatsEntryData.valid);
   agentDhcpL2RelayStatsEntryData.ifIndex = ifIndex;
   SET_VALID (I_agentDhcpL2RelayStatsEntryIndex_ifIndex,
                             agentDhcpL2RelayStatsEntryData.valid);


  if(((searchType == EXACT) ?
      (snmpDhcpL2RelayIntfGet(agentDhcpL2RelayStatsEntryData.ifIndex) != L7_SUCCESS) :
      ((snmpDhcpL2RelayIntfGet(agentDhcpL2RelayStatsEntryData.ifIndex) != L7_SUCCESS) &&
       (snmpDhcpL2RelayIntfNextGet(agentDhcpL2RelayStatsEntryData.ifIndex, &agentDhcpL2RelayStatsEntryData.ifIndex) != L7_SUCCESS))) ||
     (usmDbIntIfNumFromExtIfNum(agentDhcpL2RelayStatsEntryData.ifIndex, &intIfNum) != L7_SUCCESS))
  {
    ZERO_VALID(agentDhcpL2RelayStatsEntryData.valid);
    return (NULL);
  }
  if ( usmDbDsL2RelayIntfStatsGet (intIfNum, &stats) != L7_SUCCESS)
  {
    ZERO_VALID(agentDhcpL2RelayStatsEntryData.valid);
    return (NULL);
  }
  switch (nominator)
  {
  case -1:
  case I_agentDhcpL2RelayStatsEntryIndex_ifIndex:
    if ( nominator != -1 ) break;
    /* else pass through */
  case I_agentDhcpL2RelayUntrustedSrvrMsgsWithOptn82:
    agentDhcpL2RelayStatsEntryData.agentDhcpL2RelayUntrustedSrvrMsgsWithOptn82 =
                                     stats.untrustedSrvMsgsWithOpt82;
    SET_VALID ( I_agentDhcpL2RelayUntrustedSrvrMsgsWithOptn82,
                       agentDhcpL2RelayStatsEntryData.valid);
    if (nominator != -1) break;
  case I_agentDhcpL2RelayUntrustedClntMsgsWithOptn82:
    agentDhcpL2RelayStatsEntryData.agentDhcpL2RelayUntrustedClntMsgsWithOptn82 =
                                             stats.untrustedCliMsgsWithOpt82;
       SET_VALID ( I_agentDhcpL2RelayUntrustedClntMsgsWithOptn82,
                       agentDhcpL2RelayStatsEntryData.valid);
         if (nominator != -1) break;
  case I_agentDhcpL2RelayTrustedSrvrMsgsWithoutOptn82:
    agentDhcpL2RelayStatsEntryData.agentDhcpL2RelayTrustedSrvrMsgsWithoutOptn82 =
                                           stats.trustedSrvMsgsWithoutOpt82;
     SET_VALID ( I_agentDhcpL2RelayTrustedSrvrMsgsWithoutOptn82,
                         agentDhcpL2RelayStatsEntryData.valid);
     if (nominator != -1) break;
  case I_agentDhcpL2RelayTrustedClntMsgsWithoutOptn82:
    agentDhcpL2RelayStatsEntryData.agentDhcpL2RelayTrustedClntMsgsWithoutOptn82 =
                                           stats.untrustedCliMsgsWithOpt82;
     SET_VALID ( I_agentDhcpL2RelayTrustedClntMsgsWithoutOptn82,
                         agentDhcpL2RelayStatsEntryData.valid);
      break;
   default:
      return(NULL);
     break;
  }
   if ( nominator >= 0 && !VALID(nominator,agentDhcpL2RelayStatsEntryData.valid))
      return(NULL);
   return(&agentDhcpL2RelayStatsEntryData);

}
#endif /* I_agentDhcpL2RelayAdminMode */
#ifdef I_agentDhcpL2RelaySubscriptionIntfIndex

agentDhcpL2RelaySubscriptionConfigEntry_t *
k_agentDhcpL2RelaySubscriptionConfigEntry_get(int serialNum, ContextInfo *contextInfo,
                                          int nominator,
                                          int searchType,
                                          SR_UINT32 agentDhcpL2RelaySubscriptionIntfIndex,
                                          OctetString * agentDhcpL2RelaySubscriptionName)
{
  static agentDhcpL2RelaySubscriptionConfigEntry_t agentDhcpL2RelaySubscriptionConfigEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 intfIndex;
  L7_uchar8 subscName[SNMP_BUFFER_LEN];
  L7_uchar8 eoStr = L7_EOS;

  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_DOT1Q_COMPONENT_ID ) == L7_FALSE )
    return(NULL);

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    /*agentDhcpL2RelaySubscriptionConfigEntryData.agentDhcpL2RelaySubscriptionIntfIndex 
    = L7_NULL;*/
    agentDhcpL2RelaySubscriptionConfigEntryData.agentDhcpL2RelaySubscriptionName
    = MakeOctetString(NULL, 0);
    agentDhcpL2RelaySubscriptionConfigEntryData.agentDhcpL2RelaySubscriptionEnable = L7_DISABLE;
    agentDhcpL2RelaySubscriptionConfigEntryData.agentDhcpL2RelayCircuitIdSubscriptionEnable = L7_DISABLE;
    agentDhcpL2RelaySubscriptionConfigEntryData.agentDhcpL2RelayRemoteIdSubscriptionEnable 
    = MakeOctetString(NULL, 0);

  }
  ZERO_VALID(agentDhcpL2RelaySubscriptionConfigEntryData.valid);
  bzero(snmp_buffer, sizeof(snmp_buffer));
  agentDhcpL2RelaySubscriptionConfigEntryData.agentDhcpL2RelaySubscriptionIntfIndex = 
    agentDhcpL2RelaySubscriptionIntfIndex;
  osapiStrncpy(agentDhcpL2RelaySubscriptionConfigEntryData.agentDhcpL2RelaySubscriptionName->octet_ptr,
         agentDhcpL2RelaySubscriptionName->octet_ptr, agentDhcpL2RelaySubscriptionName->length);
  agentDhcpL2RelaySubscriptionConfigEntryData.agentDhcpL2RelaySubscriptionName->length =
         agentDhcpL2RelaySubscriptionName->length;

  SET_VALID(I_agentDhcpL2RelaySubscriptionIntfIndex,\
            agentDhcpL2RelaySubscriptionConfigEntryData.valid);

  SET_VALID(I_agentDhcpL2RelaySubscriptionName,\
            agentDhcpL2RelaySubscriptionConfigEntryData.valid);
  /* Validating the row-entries for GET and GETNEXT operations */
  if (searchType == EXACT)
  {
    if ((snmpDhcpL2RelaySubscriptionEntryGet(
                                           agentDhcpL2RelaySubscriptionConfigEntryData.agentDhcpL2RelaySubscriptionIntfIndex,
                                           &agentDhcpL2RelaySubscriptionConfigEntryData.agentDhcpL2RelaySubscriptionName))
        != L7_SUCCESS)
    {
      ZERO_VALID(agentDhcpL2RelaySubscriptionConfigEntryData.valid);
      return(NULL);
    }
  }
  else if (searchType == NEXT)
  {
    if (snmpDhcpL2RelaySubscriptionEntryGet(agentDhcpL2RelaySubscriptionConfigEntryData.agentDhcpL2RelaySubscriptionIntfIndex,
                                            &agentDhcpL2RelaySubscriptionConfigEntryData.agentDhcpL2RelaySubscriptionName)
        != L7_SUCCESS)
    {
      if (snmpDhcpL2RelaySubscriptionEntryNextGet(
                                                 &agentDhcpL2RelaySubscriptionConfigEntryData.agentDhcpL2RelaySubscriptionIntfIndex,
                                                 &agentDhcpL2RelaySubscriptionConfigEntryData.agentDhcpL2RelaySubscriptionName)
          != L7_SUCCESS)
      {
        ZERO_VALID(agentDhcpL2RelaySubscriptionConfigEntryData.valid);
        return(NULL);
      }
    }
  }

  osapiStrncpy(subscName, &eoStr, L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX);
  intfIndex = agentDhcpL2RelaySubscriptionConfigEntryData.agentDhcpL2RelaySubscriptionIntfIndex;
  osapiStrncpy(subscName, agentDhcpL2RelaySubscriptionConfigEntryData.agentDhcpL2RelaySubscriptionName->octet_ptr,
         agentDhcpL2RelaySubscriptionConfigEntryData.agentDhcpL2RelaySubscriptionName->length );

  switch (nominator)
  {
  case -1:
  case I_agentDhcpL2RelaySubscriptionIntfIndex:
  case I_agentDhcpL2RelaySubscriptionName:
          if ( nominator != -1 ) break;

  case I_agentDhcpL2RelaySubscriptionRowStatus:
        agentDhcpL2RelaySubscriptionConfigEntryData.agentDhcpL2RelaySubscriptionRowStatus =
                D_agentDhcpL2RelaySubscriptionRowStatus_active;
        SET_VALID(I_agentDhcpL2RelaySubscriptionRowStatus,
                              agentDhcpL2RelaySubscriptionConfigEntryData.valid);
      if (nominator != -1) break;

  case I_agentDhcpL2RelaySubscriptionEnable:
    if (snmpDhcpL2RelaySubscriptionModeGet(intfIndex, subscName,
                                           &agentDhcpL2RelaySubscriptionConfigEntryData.agentDhcpL2RelaySubscriptionEnable) == L7_SUCCESS)
    {
      SET_VALID(I_agentDhcpL2RelaySubscriptionEnable, agentDhcpL2RelaySubscriptionConfigEntryData.valid);
    }

    if (nominator != -1) break;
    /* else pass through */

  case I_agentDhcpL2RelayCircuitIdSubscriptionEnable:
    if (snmpDhcpL2RelayCircuitIdSubscriptionModeGet(intfIndex, subscName,
                                                    &agentDhcpL2RelaySubscriptionConfigEntryData.agentDhcpL2RelayCircuitIdSubscriptionEnable) == L7_SUCCESS)
    {
      SET_VALID(I_agentDhcpL2RelayCircuitIdSubscriptionEnable, agentDhcpL2RelaySubscriptionConfigEntryData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */

  case I_agentDhcpL2RelayRemoteIdSubscriptionEnable:
    if (snmpDhcpL2RelayRemoteIdSubscriptionModeGet(intfIndex, subscName,
                                                   &agentDhcpL2RelaySubscriptionConfigEntryData.agentDhcpL2RelayRemoteIdSubscriptionEnable) == L7_SUCCESS)
    {
      SET_VALID(I_agentDhcpL2RelayRemoteIdSubscriptionEnable, agentDhcpL2RelaySubscriptionConfigEntryData.valid);
    }
    break;
  default:
    /* unknown nominator value */
    return(NULL);
  }


  if (nominator >= 0 && !VALID(nominator, agentDhcpL2RelaySubscriptionConfigEntryData.valid))
  {
    return(NULL);
  }

  return(&agentDhcpL2RelaySubscriptionConfigEntryData);
}
#ifdef SETS
int
k_agentDhcpL2RelaySubscriptionConfigEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                           doList_t *dp, ContextInfo *contextInfo)
{
    return NO_ERROR;
}

int
k_agentDhcpL2RelaySubscriptionConfigEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                            doList_t *doHead, doList_t *dp)
{
    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}
int
k_agentDhcpL2RelaySubscriptionConfigEntry_set_defaults(doList_t *dp)
{
    agentDhcpL2RelaySubscriptionConfigEntry_t *data = (agentDhcpL2RelaySubscriptionConfigEntry_t *) (dp->data);

    data->agentDhcpL2RelaySubscriptionEnable = D_agentDhcpL2RelaySubscriptionEnable_disable;
    data->agentDhcpL2RelayCircuitIdSubscriptionEnable = D_agentDhcpL2RelayCircuitIdSubscriptionEnable_disable;
    if ((data->agentDhcpL2RelayRemoteIdSubscriptionEnable = MakeOctetStringFromText("")) == 0)
    {
      return RESOURCE_UNAVAILABLE_ERROR;
    }
    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentDhcpL2RelaySubscriptionConfigEntry_set(agentDhcpL2RelaySubscriptionConfigEntry_t *data,
                                          ContextInfo *contextInfo, int function)
{
  L7_char8 tempValid[sizeof(data->valid)];
  L7_uint32 intfIndex;
  L7_uchar8 subscName[L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX+1];
  L7_uchar8 eoStr = L7_EOS;
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */

  bzero(tempValid, sizeof(tempValid));
  osapiStrncpy(subscName, &eoStr, L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX);

  intfIndex = data->agentDhcpL2RelaySubscriptionIntfIndex;
  osapiStrncpy(subscName, data->agentDhcpL2RelaySubscriptionName->octet_ptr,
         data->agentDhcpL2RelaySubscriptionName->length);

  if (VALID(I_agentDhcpL2RelaySubscriptionRowStatus, data->valid) &&
           data->agentDhcpL2RelaySubscriptionRowStatus == D_agentDhcpL2RelaySubscriptionRowStatus_createAndGo)
  {
    if(!VALID(I_agentDhcpL2RelaySubscriptionEnable, data->valid) &&
       !VALID(I_agentDhcpL2RelayCircuitIdSubscriptionEnable, data->valid) &&
       !VALID(I_agentDhcpL2RelayRemoteIdSubscriptionEnable, data->valid))
    {
      /* If none of the params are enbaled then the entry is not created.*/
      return COMMIT_FAILED_ERROR;
    }
  }

  if (VALID(I_agentDhcpL2RelaySubscriptionEnable, data->valid))
  {
    if (snmpDhcpL2RelaySubscriptionModeSet(intfIndex, subscName, 
                                           data->agentDhcpL2RelaySubscriptionEnable) != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }
  }

  if (VALID(I_agentDhcpL2RelayCircuitIdSubscriptionEnable, data->valid))
  {
    if (snmpDhcpL2RelayCircuitIdSubscriptionModeSet(intfIndex, subscName, 
                                                    data->agentDhcpL2RelayCircuitIdSubscriptionEnable) != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }
  }

  if (VALID(I_agentDhcpL2RelayRemoteIdSubscriptionEnable, data->valid))
  {
    if (snmpDhcpL2RelayRemoteIdSubscriptionModeSet(intfIndex, subscName, 
                                                   data->agentDhcpL2RelayRemoteIdSubscriptionEnable) != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }
  }
  return NO_ERROR;
}

#ifdef SR_agentDhcpL2RelaySubscriptionConfigEntry_UNDO
/* add #define SR_agentDhcpL2RelaySubscriptionConfigEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentDhcpL2RelaySubscriptionConfigEntry family.
 */
int
agentDhcpL2RelaySubscriptionConfigEntry_undo(doList_t *doHead, doList_t *doCur,
                                         ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentDhcpL2RelaySubscriptionConfigEntry_UNDO */

#endif /* SETS */

#endif /* I_agentDhcpL2RelaySubscriptionIntfIndex */

#endif /* L7_DHCP_L2_RELAY_PACKAGE */

#ifdef L7_AUTO_INSTALL_PACKAGE

#ifdef I_agentAutoinstallMode
agentAutoInstallConfigGroup_t *
k_agentAutoInstallConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                          int nominator)
{
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_BOOL mode;
  L7_uint32 retryCnt = 0;
  static agentAutoInstallConfigGroup_t agentAutoInstallConfigGroupData;

  ZERO_VALID(agentAutoInstallConfigGroupData.valid);

  switch (nominator)
  {
  case -1:
    if (nominator != -1) break;
    /* else pass through */

  case I_agentAutoinstallMode :
    if(usmdbAutoInstallStartStopGet(&mode) == L7_SUCCESS)
    {
      if(mode == L7_TRUE)
      {
        agentAutoInstallConfigGroupData.agentAutoinstallMode = L7_ENABLE;
      }
      else
      {
        agentAutoInstallConfigGroupData.agentAutoinstallMode = L7_DIAG_DISABLE;
      }
      SET_VALID(I_agentAutoinstallMode, agentAutoInstallConfigGroupData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */

  case I_agentAutoinstallAutosaveMode :
    if(usmdbAutoInstallAutoSaveGet(&mode) == L7_SUCCESS)
    {
      if(mode == L7_TRUE)
      {
        agentAutoInstallConfigGroupData.agentAutoinstallAutosaveMode = L7_ENABLE;
      }
      else
      {
        agentAutoInstallConfigGroupData.agentAutoinstallAutosaveMode = L7_DIAG_DISABLE;
      }
      SET_VALID(I_agentAutoinstallAutosaveMode, agentAutoInstallConfigGroupData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */

  case I_agentAutoinstallUnicastRetryCount :
    if(usmdbAutoInstallAutoSaveGet(&retryCnt) == L7_SUCCESS)
    {
      agentAutoInstallConfigGroupData.agentAutoinstallUnicastRetryCount = retryCnt;
      SET_VALID(I_agentAutoinstallUnicastRetryCount, agentAutoInstallConfigGroupData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */

  case I_agentAutoinstallStatus :
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if ((L7_SUCCESS == usmdbAutoInstallCurrentStatusGet(&snmp_buffer[0])) &&
       (SafeMakeOctetStringFromText(&agentAutoInstallConfigGroupData.agentAutoinstallStatus, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentAutoinstallStatus, agentAutoInstallConfigGroupData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentAutoInstallConfigGroupData.valid))
    return (NULL);

  return(&agentAutoInstallConfigGroupData);
}

int
k_agentAutoInstallConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                        doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentAutoInstallConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                                                doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentAutoInstallConfigGroup_set(agentAutoInstallConfigGroup_t *data,
                                        ContextInfo *contextInfo, int function)
{
  L7_BOOL mode;

  if(VALID(I_agentAutoinstallMode, data->valid))
  {
    if(data->agentAutoinstallMode == L7_DIAG_DISABLE)
    {
      mode = L7_FALSE;
    }
    else
    {
      mode = L7_TRUE;
    }
    if(usmdbAutoInstallStartStopSet(mode) != L7_SUCCESS)
    {
       return(COMMIT_FAILED_ERROR);
    }
  }
  if(VALID(I_agentAutoinstallAutosaveMode, data->valid))
  {
    if(data->agentAutoinstallAutosaveMode == L7_DIAG_DISABLE)
    {
      mode = L7_FALSE;
    }
    else
    {
      mode = L7_TRUE;
    }
    if(usmdbAutoInstallAutoSaveSet(mode) != L7_SUCCESS)
    {
       return(COMMIT_FAILED_ERROR);
    }
  }

  if(VALID(I_agentAutoinstallUnicastRetryCount, data->valid))
  {
    if(usmdbAutoInstallFileDownLoadRetryCountSet(data->agentAutoinstallUnicastRetryCount)
       != L7_SUCCESS)
    {
      return(COMMIT_FAILED_ERROR);
    }
  }
  return NO_ERROR;
}
#endif /* I_agentAutoinstallMode */

#endif /* L7_AUTO_INSTALL_PACKAGE */



#ifdef I_agentLinkDependencyGroupId
/* Code For Link Dependency Starts */
agentLinkDependencyGroupEntry_t *k_agentLinkDependencyGroupEntry_get( int serialNum,
                                                                      ContextInfo *contextInfo,
                                                                      int nominator,
                                                                      int searchType,
                                                                      SR_INT32 agentLinkDependencyGroupId )
{
  static agentLinkDependencyGroupEntry_t agentLinkDependencyGroupEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_uint32      emptyMembers;
  L7_uint32      emptyDependencies;
  L7_INTF_MASK_t members;
  L7_INTF_MASK_t dependencies;
  L7_RC_t        rc;

  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_LINK_DEPENDENCY_COMPONENT_ID) == L7_FALSE)
  {
    return (NULL);
  }

  if (firstTime == L7_TRUE)
  {
    agentLinkDependencyGroupEntryData.agentLinkDependencyGroupMemberPortMask    = MakeOctetString(NULL, 0);
    agentLinkDependencyGroupEntryData.agentLinkDependencyGroupDependsOnPortMask = MakeOctetString(NULL, 0);

    firstTime = L7_FALSE;
  }

  ZERO_VALID(agentLinkDependencyGroupEntryData.valid);

  if ( EXACT == searchType )
  {
    rc = usmDbLinkDependencyGroupGet( agentLinkDependencyGroupId, &members, &dependencies );
  }
  else
  {
    do
    {
      rc = usmDbLinkDependencyGroupGet( agentLinkDependencyGroupId, &members, &dependencies );
      if ( L7_SUCCESS == rc )
      {
        break;
      }
      agentLinkDependencyGroupId++;
    } while ( agentLinkDependencyGroupId <= L7_LINK_DEPENDENCY_LAST_GROUP );
  }

  if ( L7_SUCCESS != rc )
  {
    ZERO_VALID( agentLinkDependencyGroupEntryData.valid );
    return( NULL );
  }

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
    case -1:
    case I_agentLinkDependencyGroupId:
    case I_agentLinkDependencyGroupStatus:
    case I_agentLinkDependencyGroupMemberPortMask:
    case I_agentLinkDependencyGroupDependsOnPortMask:
      L7_INTF_MASKREV( members );
      L7_INTF_MASKREV( dependencies );

      agentLinkDependencyGroupEntryData.agentLinkDependencyGroupId = agentLinkDependencyGroupId;
      agentLinkDependencyGroupEntryData.agentLinkDependencyGroupStatus = D_agentLinkDependencyGroupStatus_active;
      SafeMakeOctetString( &agentLinkDependencyGroupEntryData.agentLinkDependencyGroupMemberPortMask,
                           members.value,
                           L7_INTF_INDICES );
      SafeMakeOctetString( &agentLinkDependencyGroupEntryData.agentLinkDependencyGroupDependsOnPortMask,
                           dependencies.value,
                           L7_INTF_INDICES );

      SET_VALID( I_agentLinkDependencyGroupId,                 agentLinkDependencyGroupEntryData.valid);
      L7_INTF_NONZEROMASK( members,      emptyMembers );
      if ( 0 == emptyMembers )
      {
        agentLinkDependencyGroupEntryData.agentLinkDependencyGroupStatus = D_agentLinkDependencyGroupStatus_notReady;
      }
      else
      {
        SET_VALID( I_agentLinkDependencyGroupMemberPortMask,     agentLinkDependencyGroupEntryData.valid );
      }

      L7_INTF_NONZEROMASK( dependencies, emptyDependencies );
      if ( 0 == emptyDependencies )
      {
        agentLinkDependencyGroupEntryData.agentLinkDependencyGroupStatus = D_agentLinkDependencyGroupStatus_notReady;
      }
      else
      {
        SET_VALID( I_agentLinkDependencyGroupDependsOnPortMask,  agentLinkDependencyGroupEntryData.valid );
      }
      SET_VALID( I_agentLinkDependencyGroupStatus,             agentLinkDependencyGroupEntryData.valid );
      break;

    default:
      return(NULL);
      break;
  }

  if (nominator >= 0 && !VALID(nominator, agentLinkDependencyGroupEntryData.valid))
  {
    return(NULL);
  }

  return(&agentLinkDependencyGroupEntryData);
}

#ifdef SETS
int
k_agentLinkDependencyGroupEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                     doList_t *dp, ContextInfo *contextInfo)
{
  agentLinkDependencyGroupEntry_t *agentLinkDependencyGroupEntry = (agentLinkDependencyGroupEntry_t *)(dp->data);

  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_LINK_DEPENDENCY_COMPONENT_ID) == L7_FALSE)
  {
    return NO_SUCH_NAME_ERROR;
  }

  if ( L7_TRUE != usmDbLinkDependencyIsValidGroup( agentLinkDependencyGroupEntry->agentLinkDependencyGroupId ) )
  {
    return NO_CREATION_ERROR;
  }

  switch (object->nominator)
  {
#ifdef I_agentLinkDependencyGroupStatus
    case I_agentLinkDependencyGroupStatus:

      switch (value->sl_value)
      {
        case D_agentLinkDependencyGroupStatus_destroy:
          break;
        default:
          return WRONG_VALUE_ERROR;
      }
      break;
#endif /* I_agentLinkDependencyGroupStatus */

#ifdef I_agentLinkDependencyGroupMemberPortMask
    case I_agentLinkDependencyGroupMemberPortMask:
      break;
#endif /* I_agentLinkDependencyGroupMemberPortMask */

#ifdef I_agentLinkDependencyGroupDependsOnPortMask
    case I_agentLinkDependencyGroupDependsOnPortMask:
      break;
#endif /* I_agentLinkDependencyGroupDependsOnPortMask */

    default:
      return GEN_ERROR;
  }

  return NO_ERROR;
}

int
k_agentLinkDependencyGroupEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                      doList_t *doHead, doList_t *dp)
{
  if ( L7_FALSE == usmDbComponentPresentCheck( USMDB_UNIT_CURRENT, L7_LINK_DEPENDENCY_COMPONENT_ID ) )
  {
    return NO_SUCH_NAME_ERROR;
  }

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentLinkDependencyGroupEntry_set_defaults(doList_t *dp)
{
  if ( L7_FALSE == usmDbComponentPresentCheck( USMDB_UNIT_CURRENT, L7_LINK_DEPENDENCY_COMPONENT_ID ) )
  {
    return NO_SUCH_NAME_ERROR;
  }

  return NO_ERROR;
}

int
k_agentLinkDependencyGroupEntry_set(agentLinkDependencyGroupEntry_t *data,
                                    ContextInfo *contextInfo, int function)
{
  L7_INTF_MASK_t members;
  L7_INTF_MASK_t dependencies;
  L7_uint32      length;
  L7_RC_t        rc;

  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only partially successful
   */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  memset( &members,      0, sizeof( members ) );
  memset( &dependencies, 0, sizeof( dependencies ) );

  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_LINK_DEPENDENCY_COMPONENT_ID) == L7_FALSE)
  {
    return NO_SUCH_NAME_ERROR;
  }

  rc = usmDbLinkDependencyGroupGet( data->agentLinkDependencyGroupId, &members, &dependencies );

  if ( ( VALID( I_agentLinkDependencyGroupStatus, data->valid ) ) &&
       ( D_agentLinkDependencyGroupStatus_destroy == data->agentLinkDependencyGroupStatus ) )
  {
    if ( ( L7_SUCCESS == rc ) &&
         ( L7_SUCCESS != usmDbLinkDependencyGroupClear(data->agentLinkDependencyGroupId) ) )
    {
      ZERO_VALID(data->valid);
      return COMMIT_FAILED_ERROR;
    }

    return NO_ERROR;
  }

  if ( ( !VALID( I_agentLinkDependencyGroupMemberPortMask,    data->valid ) ) &&
       ( !VALID( I_agentLinkDependencyGroupDependsOnPortMask, data->valid ) ) )
  {
    ZERO_VALID(data->valid);
    return INCONSISTENT_NAME_ERROR;
  }

  if ( VALID( I_agentLinkDependencyGroupMemberPortMask, data->valid ) )
  {
    memset( &members, 0, sizeof( members ) );

    length = data->agentLinkDependencyGroupMemberPortMask->length;
    if ( length > L7_INTF_INDICES )
    {
      length = L7_INTF_INDICES;
    }
    memcpy( &members, data->agentLinkDependencyGroupMemberPortMask->octet_ptr, length );
    L7_INTF_MASKREV( members );
    SET_VALID( I_agentLinkDependencyGroupMemberPortMask, tempValid );
  }

  if ( VALID( I_agentLinkDependencyGroupDependsOnPortMask, data->valid ) )
  {
    memset( &dependencies, 0, sizeof( dependencies ) );

    length = data->agentLinkDependencyGroupDependsOnPortMask->length;
    if ( length > L7_INTF_INDICES )
    {
      length = L7_INTF_INDICES;
    }
    memcpy( &dependencies, data->agentLinkDependencyGroupDependsOnPortMask->octet_ptr, length );
    L7_INTF_MASKREV( dependencies );
    SET_VALID( I_agentLinkDependencyGroupDependsOnPortMask, tempValid );
  }

  if ( L7_SUCCESS != usmDbLinkDependencyGroupSet( data->agentLinkDependencyGroupId, &members, &dependencies ) )
  {
    ZERO_VALID(data->valid);
    return COMMIT_FAILED_ERROR;
  }

  memcpy( data->valid, tempValid, sizeof( data->valid ) );

  return NO_ERROR;
}

#ifdef SR_agentLinkDependencyGroupEntry_UNDO
/* add #define SR_agentLinkDependencyGroupEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentLinkDependencyGroupEntry family.
 */
int
agentLinkDependencyGroupEntry_undo(doList_t *doHead, doList_t *doCur,
                                   ContextInfo *contextInfo)
{
  agentLinkDependencyGroupEntry_t *data     = (agentLinkDependencyGroupEntry_t *)doCur->data;
  agentLinkDependencyGroupEntry_t *undodata = (agentLinkDependencyGroupEntry_t *)doCur->undodata;

  if ( NULL == data )
  {
    return UNDO_FAILED_ERROR;
  }

  if ( NULL == undodata )
  {
    usmDbLinkDependencyGroupClear(data->agentLinkDependencyGroupId);
    return NO_ERROR;
  }

  memcpy( undodata->valid, data->valid, sizeof( data->valid ) );

  if ( NO_ERROR == k_agentLinkDependencyGroupEntry_set( undodata, contextInfo, SR_ADD_MODIFY ) )
  {
    return( NO_ERROR );
  }

  return UNDO_FAILED_ERROR;
}
#endif /* SR_agentLinkDependencyGroupEntry_UNDO */

#endif /* SETS */

#endif /* I_agentLinkDependencyGroupId */
