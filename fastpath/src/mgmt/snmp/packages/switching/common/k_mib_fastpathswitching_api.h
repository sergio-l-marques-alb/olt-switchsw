/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename k_mib_fastpathswitching_api.h
*
* @purpose SNMP specific value conversion
*
* @component SNMP
*
* @create 04/26/2001
*
* @author soma, cpverne
*
* @end
*
**********************************************************************/

#include <l7_common.h>
#include "cli_web_exports.h"
#include "dot1q_exports.h"
#include "dot1s_exports.h"
#include "snmp_exports.h"
#include "user_manager_exports.h"
#include "dot1x_auth_serv_exports.h"

#include <defaultconfig.h>
#include <sysapi.h>
#include "snmp_ct_api.h"
#include "user_mgr_api.h"
#include "osapi_support.h"
#include "usmdb_nim_api.h"
#include "usmdb_common.h"
#include "usmdb_dot1q_api.h"
#include "usmdb_dot1s_api.h"
#include "usmdb_ip_base_api.h"
#include "usmdb_mfdb_api.h"
#include "usmdb_mib_bridge_api.h"
#include "usmdb_mirror_api.h"
#include "usmdb_policy_api.h"
#include "usmdb_port_user.h"
#include "usmdb_sim_api.h"
#include "usmdb_snmp_api.h"
#include "usmdb_snooping_api.h"
#include "usmdb_sshd_api.h"
#include "usmdb_switch_cpu_api.h"
#include "usmdb_trapmgr_api.h"
#include "usmdb_util_api.h"
#include "usmdb_voice_vlan_api.h"
#include "dot1q_api.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_protected_port_api.h"
#include "usmdb_dot3ad_api.h"
#include "compdefs.h"
#include "usmdb_user_mgmt_api.h"
#include "snooping_exports.h"
#include "dot3ad_exports.h"
#include "usmdb_dhcp_client.h"
#include "sshd_exports.h"
#include "sim_exports.h"
#include "config_script_api.h"
#include "usmdb_dim_api.h"
#include "usmdb_dns_client_api.h"
#include "usmdb_sdm.h"

#ifdef L7_DAI_PACKAGE
#include "usmdb_dai_api.h"
#endif

#ifdef L7_DHCP_SNOOPING_PACKAGE
#include "usmdb_dhcp_snooping.h"
#endif

#include "unitmgr_api.h"
#include "usmdb_unitmgr_api.h"
static int voiceVlanDeviceDebugFlag= L7_FALSE;
static L7_BOOL snmpAPLAuthMethodCheckSetIsAllowed(L7_ACCESS_LINE_t line, L7_ACCESS_LEVEL_t access, L7_char8 *aplName, L7_uint32 order, L7_USER_MGR_AUTH_METHOD_t method);

static L7_RC_t snmpAgentAuthListMethodModifyDependencyCheck(L7_ACCESS_LINE_t line,
                                                            L7_ACCESS_LEVEL_t access,
                                                            L7_char8 *aplName,
                                                            L7_uint32 order,
                                                            L7_USER_MGR_AUTH_METHOD_t method);
L7_RC_t snmpConvertStringToMac(L7_uchar8 *macStr, L7_uchar8 *macAddr);

L7_RC_t
snmpAgentInventoryAdditionalPackagesGet ( L7_uint32 UnitIndex, L7_char8 *buf)
{
  L7_char8 packageBuffer[] =

#ifdef SNMP_BGP
    "BGP-4"
  #ifdef SNMP_QOS
    ", "
  #endif /* SNMP_QOS */
#endif /* SNMP_BGP */

#ifdef SNMP_QOS
    "QOS"
  #ifdef SNMP_IP_MCAST
    ", "
  #endif /* SNMP_IP_MCAST */
#endif /* SNMP_QOS */

#ifdef SNMP_IP_MCAST
    "IP Multicast"
  #ifdef SNMP_IPV6
    ", "
  #endif /* SNMP_IPV6 */
#endif /* SNMP_IP_MCAST */

#ifdef SNMP_IPV6

    "IPv6"
  #ifdef L7_STACKING_PACKAGE
    ", "
  #endif /* L7_STACKING_PACKAGE */
#endif
#ifndef L7_CHASSIS

#ifdef L7_STACKING_PACKAGE
    "Stacking"
#endif /* L7_STACKING_PACKAGE */
#endif
    "";

  strcpy(buf, packageBuffer);
  return L7_SUCCESS;
}

#ifdef I_agentPortSpeedDuplexStatus 
typedef enum
{
  L7_SNMP_PORT_CAPABILITY_DUPLEX_AUTO = (0X80 >> D_agentPortSpeedDuplexStatus_auto_duplex),
  L7_SNMP_PORT_CAPABILITY_DUPLEX_HALF = (0x80 >> D_agentPortSpeedDuplexStatus_half_duplex), 
  L7_SNMP_PORT_CAPABILITY_DUPLEX_FULL = (0x80 >> D_agentPortSpeedDuplexStatus_full_duplex),
  L7_SNMP_PORT_CAPABILITY_SPEED_AUTO  = (0x80 >> D_agentPortSpeedDuplexStatus_speed_auto),
  L7_SNMP_PORT_CAPABILITY_SPEED_10    = (0x80 >> D_agentPortSpeedDuplexStatus_speed_10mbit),
  L7_SNMP_PORT_CAPABILITY_SPEED_100   = (0x80 >> D_agentPortSpeedDuplexStatus_speed_100mbit),
  L7_SNMP_PORT_CAPABILITY_SPEED_1000  = (0x80 >> D_agentPortSpeedDuplexStatus_speed_1gbit),
  L7_SNMP_PORT_CAPABILITY_SPEED_10G   = (0x80 >> D_agentPortSpeedDuplexStatus_speed_10gbit)
}L7_SNMP_PORT_SPEEDS_t;
#endif

/**************************************************************************************************************/
L7_RC_t
snmpAgentTrapLogEntryGet ( L7_uint32 UnitIndex, L7_uint32 logIndex )
{
  usmDbTrapLogEntry_t trapLogEntry;
  L7_RC_t rc;

  /* try to get the entry */
  rc = usmDbTrapLogEntryGet(UnitIndex, USMDB_USER_DISPLAY, logIndex, &trapLogEntry);

  return rc;
}

L7_RC_t
snmpAgentTrapLogEntryNextGet ( L7_uint32 UnitIndex, L7_uint32 *logIndex )
{
  /* incriment to the next value, and see if it's available */
  *logIndex += 1;

  return snmpAgentTrapLogEntryGet(UnitIndex, *logIndex);
}

L7_RC_t
snmpAgentTrapLogSystemTimeGet ( L7_uint32 UnitIndex, L7_int32 logIndex, L7_char8 *buf)
{
  usmDbTrapLogEntry_t trapLogEntry;
  L7_RC_t rc;

  rc = usmDbTrapLogEntryGet(UnitIndex, USMDB_USER_DISPLAY, logIndex, &trapLogEntry);
  if (rc == L7_SUCCESS)
  {
    strcpy(buf, trapLogEntry.timestamp);
  }

  return rc;
}

L7_RC_t
snmpAgentTrapLogLogTrapGet ( L7_uint32 UnitIndex, L7_int32 logIndex, L7_char8 *buf)
{
  usmDbTrapLogEntry_t trapLogEntry;
  L7_RC_t rc;

  rc = usmDbTrapLogEntryGet(UnitIndex, USMDB_USER_DISPLAY, logIndex, &trapLogEntry);
  if (rc == L7_SUCCESS)
  {
    strcpy(buf, trapLogEntry.message);
  }

  return rc;
}

L7_RC_t snmpCpuProcessRisingThresholdSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  if ((val < 0) || (val > FD_SIM_DEFAULT_CPU_UTIL_MAX_THRESHOLD))
  {
    return L7_FAILURE;
  }

  if (val == L7_NULL)
  {
    if ((usmdbCpuUtilMonitorParamSet(UnitIndex, SIM_CPU_UTIL_MONITOR_FALLING_PERIOD_PARAM, val) != L7_SUCCESS) ||
        (usmdbCpuUtilMonitorParamSet(UnitIndex, SIM_CPU_UTIL_MONITOR_FALLING_THRESHOLD_PARAM, val) != L7_SUCCESS) ||
        (usmdbCpuUtilMonitorParamSet(UnitIndex, SIM_CPU_UTIL_MONITOR_RISING_THRESHOLD_PARAM, val) != L7_SUCCESS) ||
        (usmdbCpuUtilMonitorParamSet(UnitIndex, SIM_CPU_UTIL_MONITOR_RISING_PERIOD_PARAM, val) != L7_SUCCESS))
    {
      return L7_FAILURE;
    }
  }
  else
  {
    if (usmdbCpuUtilMonitorParamSet(UnitIndex, SIM_CPU_UTIL_MONITOR_RISING_THRESHOLD_PARAM, val) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}

L7_RC_t snmpCpuProcessRisingThresholdIntervalSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  if ((val < 0) || (val > FD_SIM_DEFAULT_CPU_UTIL_MAX_PERIOD) ||
      ((val % FD_SIM_DEFAULT_CPU_UTIL_MIN_PERIOD) != 0))
  {
    return L7_FAILURE;
  }

  if (val == L7_NULL)
  {
    if ((usmdbCpuUtilMonitorParamSet(UnitIndex, SIM_CPU_UTIL_MONITOR_FALLING_PERIOD_PARAM, L7_NULL) != L7_SUCCESS) ||
        (usmdbCpuUtilMonitorParamSet(UnitIndex, SIM_CPU_UTIL_MONITOR_FALLING_THRESHOLD_PARAM, L7_NULL) != L7_SUCCESS) ||
        (usmdbCpuUtilMonitorParamSet(UnitIndex, SIM_CPU_UTIL_MONITOR_RISING_THRESHOLD_PARAM, L7_NULL) != L7_SUCCESS) ||
        (usmdbCpuUtilMonitorParamSet(UnitIndex, SIM_CPU_UTIL_MONITOR_RISING_PERIOD_PARAM, L7_NULL) != L7_SUCCESS))
    {
      return L7_FAILURE;
    }
  }
  else
  {
    if (usmdbCpuUtilMonitorParamSet(UnitIndex, SIM_CPU_UTIL_MONITOR_RISING_PERIOD_PARAM, val) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}

L7_RC_t snmpCpuProcessFallingThresholdSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_uint32 tmpVal;

  if ((val < 0) || (val > FD_SIM_DEFAULT_CPU_UTIL_MAX_THRESHOLD))
  {
    return L7_FAILURE;
  }

  if (usmdbCpuUtilMonitorParamGet(UnitIndex, SIM_CPU_UTIL_MONITOR_RISING_THRESHOLD_PARAM, &tmpVal) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  else
  {
    if (val == L7_NULL)
    {
      if (tmpVal > L7_NULL)
      {
        return L7_FAILURE;
      }
    }
    else
    {
      if (usmdbCpuUtilMonitorParamSet(UnitIndex, SIM_CPU_UTIL_MONITOR_FALLING_THRESHOLD_PARAM, val) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
    }
  }

  return L7_SUCCESS;
}

L7_RC_t snmpCpuProcessFallingThresholdIntervalSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_uint32 tmpVal;

  if ((val < 0) || (val > FD_SIM_DEFAULT_CPU_UTIL_MAX_PERIOD) ||
      ((val % FD_SIM_DEFAULT_CPU_UTIL_MIN_PERIOD) != 0))
  {
    return L7_FAILURE;
  }

  if (usmdbCpuUtilMonitorParamGet(UnitIndex, SIM_CPU_UTIL_MONITOR_RISING_PERIOD_PARAM, &tmpVal) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  else
  {
    if (val == L7_NULL)
    {
      if (tmpVal > L7_NULL)
      {
        return L7_FAILURE;
      }
      else 
      {
        if (usmdbCpuUtilMonitorParamSet(UnitIndex, SIM_CPU_UTIL_MONITOR_FALLING_PERIOD_PARAM, val) != L7_SUCCESS)
        {
          return L7_FAILURE;
        }
      }
    }
    else
    {
      if (usmdbCpuUtilMonitorParamSet(UnitIndex, SIM_CPU_UTIL_MONITOR_FALLING_PERIOD_PARAM, val) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
    }
  }

  return L7_SUCCESS;
}

/**************************************************************************************************************/
#ifdef I_agentSwitchCpuProcessIndex
L7_RC_t
snmpAgentSwitchCpuProcessEntryGet( L7_uint32 UnitIndex, L7_uint32 processIndex, 
                                   L7_char8 *processIdString, L7_uint32 processIdSize )
{
   return usmDbTaskIdGet(UnitIndex, processIndex, processIdString, processIdSize );
}

L7_RC_t
snmpAgentSwitchCpuProcessEntryNextGet( L7_uint32 UnitIndex, L7_uint32 *processIndex, 
                                       L7_char8 *processIdString, L7_uint32 processIdSize )
{
  /* incriment to the next value, and see if it's available */
  *processIndex+= 1;

  return snmpAgentSwitchCpuProcessEntryGet(UnitIndex, *processIndex, processIdString, processIdSize);
}
#endif /* I_agentSwitchCpuProcessIndex */

#ifdef I_agentSwitchCpuProcessName
L7_RC_t
snmpAgentSwitchCpuProcessNameGet( L7_uint32 UnitIndex, L7_int32 processIndex, L7_char8 *buf)
{
  usmDbProcessEntry_t dumpEntry;
  L7_RC_t rc;

  rc = usmDbSwitchCpuProcessEntryGet(UnitIndex, processIndex, &dumpEntry);
  if (rc == L7_SUCCESS)
  {
    strcpy(buf, dumpEntry.taskName);
  }

  return rc;
}
#endif /* I_agentSwitchCpuProcessName */

#ifdef I_agentSwitchCpuProcessPercentageUtilization
L7_RC_t
snmpAgentSwitchCpuProcessPercentageUtilizationGet( L7_uint32 UnitIndex, L7_int32 processIndex, L7_char8 *buf)
{
  usmDbProcessEntry_t dumpEntry;
  L7_RC_t rc;

  rc = usmDbSwitchCpuProcessEntryGet(UnitIndex, processIndex, &dumpEntry);
  if (rc == L7_SUCCESS)
  {
    strcpy(buf, dumpEntry.taskPercentString);
  }

  return rc;
}
#endif /* I_agentSwitchCpuProcessPercentageUtilization */

/**************************************************************************************************************/

L7_RC_t
snmpAgentLoginSessionEntryGet ( L7_uint32 UnitIndex, L7_uint32 loginIndex)
{
  L7_BOOL validLogin = L7_FALSE;

  /*old SSH TELNET TAG _TOTAL_CONNECTIONS*/
  if (loginIndex >= 0)
  {
    usmDbLoginSessionValidEntry(UnitIndex, loginIndex, &validLogin);
    if (validLogin == L7_TRUE)
      return L7_SUCCESS;
  }

  return L7_FAILURE;
}


L7_RC_t
snmpAgentLoginSessionEntryNextGet ( L7_uint32 UnitIndex, L7_uint32 *loginIndex)
{
  L7_BOOL validLogin = L7_FALSE;

  if (*loginIndex < 0)
    *loginIndex = 0;

  /*old SSH TELNET TAG _TOTAL_CONNECTIONS*/
  do
  {
    usmDbLoginSessionValidEntry(UnitIndex, *loginIndex, &validLogin);
    if (validLogin == L7_TRUE)
      return L7_SUCCESS;
    *loginIndex = *loginIndex + 1;
  } while (*loginIndex < (FD_CLI_DEFAULT_MAX_CONNECTIONS + 1));

  return L7_FAILURE;
}

L7_RC_t
snmpAgentLoginSessionConnectionTypeGet ( L7_uint32 UnitIndex, L7_int32 loginIndex, L7_uint32 *val)
{
  L7_uint32 temp_val=0;
  L7_RC_t rc;

  rc = usmDbLoginSessionTypeGet(UnitIndex, loginIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_LOGIN_TYPE_SERIAL:
      *val = D_agentLoginSessionConnectionType_serial;
      break;
    case L7_LOGIN_TYPE_TELNET:
      *val = D_agentLoginSessionConnectionType_telnet;
      break;
    case L7_LOGIN_TYPE_SSH:
      *val = D_agentLoginSessionConnectionType_ssh;
      break;
    case L7_LOGIN_TYPE_HTTP:
      *val = D_agentLoginSessionConnectionType_http;
      break;
    case L7_LOGIN_TYPE_HTTPS:
      *val = D_agentLoginSessionConnectionType_https;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpLoginSessionIdleTimeGet ( L7_uint32 UnitIndex, L7_int32 loginIndex, L7_uint32 *val)
{
  L7_RC_t rc;

  rc = usmDbLoginSessionIdleTimeGet(UnitIndex, loginIndex, val);

  if (rc == L7_SUCCESS)
  {
    *val = *val * 100;
  }

  return rc;
}

L7_RC_t
snmpLoginSessionTimeGet ( L7_uint32 UnitIndex, L7_int32 loginIndex, L7_uint32 *val)
{
  L7_RC_t rc;

  rc = usmDbLoginSessionTimeGet(UnitIndex, loginIndex, val);

  if (rc == L7_SUCCESS)
  {
    *val = *val * 100;
  }

  return rc;
}

L7_RC_t
snmpAgentLoginSessionStatusGet ( L7_uint32 UnitIndex, L7_int32 loginIndex, L7_uint32 *val)
{
  *val = D_agentLoginSessionStatus_active;
  return L7_SUCCESS;
}

L7_RC_t
snmpAgentLoginSessionStatusSet ( L7_uint32 UnitIndex, L7_int32 loginIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;

  switch (val)
  {
  case D_agentLoginSessionStatus_active:
    rc = L7_SUCCESS;
    break;

  case D_agentLoginSessionStatus_destroy:
    rc = usmDbLoginSessionResetConnectionSet(UnitIndex, loginIndex);
    break;

  default:
    rc = L7_FAILURE;
    break;
  }
  return L7_SUCCESS;
}

/**************************************************************************************************************/

L7_RC_t
snmpAgentTelnetAllowNewModeGet ( L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbAgentTelnetNewSessionsGet(UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *val = D_agentTelnetAllowNewMode_enable;
      break;

    case L7_FALSE:
      *val = D_agentTelnetAllowNewMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentTelnetAllowNewModeSet ( L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentTelnetAllowNewMode_enable:
    temp_val = L7_TRUE;
    break;

  case D_agentTelnetAllowNewMode_disable:
    temp_val = L7_FALSE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbAgentTelnetNewSessionsSet(UnitIndex, temp_val);
  }

  return rc;
}

/************************************************************************************************************/

/*
 * Function to retrieve the next valid Exclude keyword from the Application
 */
L7_RC_t snmpAgentUserMgrPasswdExcludeKeywordNextGet(L7_char8 *keyword, L7_uint32 maxLen)
{
  L7_uint32 len=0;

  if(keyword[0] != 0)
  {
    len = osapiStrnlen(keyword, maxLen);
    keyword[len-1] = keyword[len-1]-1;
    if(usmDbUserMgrPasswdIsExcludeKeywordExist(keyword) != L7_SUCCESS)
    {
      return L7_FAILURE;
    } 
  }

  if(usmDbUserMgrPasswdExcludeKeywordNextGet(keyword) != L7_SUCCESS)
  { 
    return L7_FAILURE;
  }
  return L7_SUCCESS; 
}


/**************************************************************************************************************/

L7_RC_t
snmpAgentUserConfigCreateSet ( L7_uint32 UnitIndex, L7_char8 *buf)
{
  L7_RC_t rc;
  L7_uint32 tempInt;
  L7_char8 temp_buf[SNMP_BUFFER_LEN];

  /* check name for correct characters and length */
  if (usmDbStringAlphaNumericCheck(buf) != L7_SUCCESS ||
      strlen(buf) >= L7_LOGIN_SIZE)
    return L7_FAILURE;

  /* do not let a user with user name "default" created*/
  if (usmDbStringCaseInsensitiveCompare(L7_USER_MGR_DEFAULT_USER_STRING, buf)
                                                                == L7_SUCCESS)
      return L7_FAILURE;

  /* check all users for an existing entry by that name */
  for (tempInt = 0; tempInt < L7_MAX_LOGINS; tempInt++)
  {
    bzero(temp_buf, SNMP_BUFFER_LEN);
    rc = usmDbLoginsGet(USMDB_UNIT_CURRENT, tempInt, temp_buf);
    if (usmDbStringCaseInsensitiveCompare(temp_buf, buf) == L7_SUCCESS)
      return L7_FAILURE;
  }

  for (tempInt = 0; tempInt < L7_MAX_LOGINS; tempInt++)
  {
    rc = usmDbLoginsGet(USMDB_UNIT_CURRENT, tempInt, temp_buf);

    if (strcmp(temp_buf, "") == 0)
    {
      rc = usmDbLoginsSet(USMDB_UNIT_CURRENT, tempInt, buf);
      rc = usmDbPasswordSet(USMDB_UNIT_CURRENT, tempInt, "", L7_FALSE);
      rc = usmDbLoginStatusSet(USMDB_UNIT_CURRENT, tempInt, L7_ENABLE);
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

L7_RC_t
snmpAgentUserConfigEntryGet ( L7_uint32 UnitIndex, L7_int32 userIndex)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  if (usmDbLoginsGet(UnitIndex, userIndex, buf) == L7_SUCCESS && buf[0] != 0)
    return L7_SUCCESS;
  return L7_FAILURE;
}

L7_RC_t
snmpAgentUserConfigEntryNextGet ( L7_uint32 UnitIndex, L7_int32 *userIndex)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  if (*userIndex < 0)
    *userIndex = 0;

  while (*userIndex < L7_MAX_LOGINS)
  {
    if (usmDbLoginsGet(UnitIndex, *userIndex, buf) == L7_SUCCESS && buf[0] != 0)
      return L7_SUCCESS;

    /* increment */
    *userIndex = *userIndex + 1;
  }
  return L7_FAILURE;
}

L7_RC_t
snmpAgentUserAccessModeGet ( L7_uint32 UnitIndex, L7_uint32 userIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbUserAccessLevelGet ( UnitIndex, userIndex, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_LOGIN_ACCESS_READ_ONLY:
      *val = D_agentUserAccessMode_read;
      break;

    case L7_LOGIN_ACCESS_READ_WRITE:
      *val = D_agentUserAccessMode_write;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentUserStatusGet ( L7_uint32 UnitIndex, L7_uint32 UserIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbLoginStatusGet ( UnitIndex, UserIndex, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentUserStatus_active;
      break;

    case L7_DISABLE:
      *val = D_agentUserStatus_destroy;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

#if L7_FEAT_SNMP_USER_MAPPING
L7_RC_t
snmpAgentUserAuthenticationTypeGet ( L7_uint32 UnitIndex, L7_uint32 userIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = userMgrLoginUserAuthenticationGet ( userIndex, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_SNMP_USER_AUTH_PROTO_NONE:
      *val = D_agentUserAuthenticationType_none;
      break;

    case L7_SNMP_USER_AUTH_PROTO_HMACMD5:
      *val = D_agentUserAuthenticationType_hmacmd5;
      break;

    case L7_SNMP_USER_AUTH_PROTO_HMACSHA:
      *val = D_agentUserAuthenticationType_hmacsha;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentUserEncryptionTypeGet ( L7_uint32 UnitIndex, L7_uint32 userIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = userMgrLoginUserEncryptionGet ( userIndex, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_SNMP_USER_PRIV_PROTO_NONE:
      *val = D_agentUserEncryptionType_none;
      break;

    case L7_SNMP_USER_PRIV_PROTO_DES:
      *val = D_agentUserEncryptionType_des;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}
#endif /* L7_FEAT_SNMP_USER_MAPPING */

L7_RC_t
snmpAgentUserNameSet ( L7_uint32 UnitIndex, L7_int32 userIndex, L7_char8 *buf )
{
  L7_RC_t rc;
  L7_uint32 tempInt;
  L7_char8 temp_buf[SNMP_BUFFER_LEN];

  /* check name for correct characters and length */
  if (usmDbStringAlphaNumericCheck(buf) != L7_SUCCESS ||
      strlen(buf) >= L7_LOGIN_SIZE)
    return L7_FAILURE;

  /* do not let a user with user name "default" created*/
  if (usmDbStringCaseInsensitiveCompare(L7_USER_MGR_DEFAULT_USER_STRING, buf)
                                                                == L7_SUCCESS)
      return L7_FAILURE;

  /* check all users for an existing entry by that name */
  for (tempInt = 0; tempInt < L7_MAX_LOGINS; tempInt++)
  {
    if (tempInt != userIndex)
    {
      bzero(temp_buf, SNMP_BUFFER_LEN);
      rc = usmDbLoginsGet(USMDB_UNIT_CURRENT, tempInt, temp_buf);
      if (usmDbStringCaseInsensitiveCompare(temp_buf, buf) == L7_SUCCESS)
        return L7_FAILURE;
    }
  }

  return usmDbLoginsSet(UnitIndex, userIndex, buf);
}

L7_RC_t
snmpAgentUserAccessModeSet ( L7_uint32 UnitIndex, L7_uint32 userIndex, L7_int32 val )
{
  L7_uint32 currentVal;

  /* return success only if it's the same value */
  if (snmpAgentUserAccessModeGet(UnitIndex, userIndex, &currentVal) == L7_SUCCESS)
  {
    if (currentVal == val)
      return L7_SUCCESS;

    switch (val)
    {
      case D_agentUserAccessMode_read:
        val = L7_LOGIN_ACCESS_READ_ONLY;
        break;
     
      case D_agentUserAccessMode_write:
        val = L7_LOGIN_ACCESS_READ_WRITE;
        break;
     
      default:
        return L7_FAILURE;
    }
    return (usmDbUserAccessLevelSet(UnitIndex, userIndex,  val));
  }

  /* return failure if changed */
  return L7_FAILURE;
}

L7_RC_t
snmpAgentUserStatusSet ( L7_uint32 UnitIndex, L7_uint32 UserIndex, L7_int32 val )
{
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentUserStatus_active:
    rc = L7_SUCCESS;
    break;

  case D_agentUserStatus_destroy:
    rc = usmDbLoginsDelete( UnitIndex, UserIndex );
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  return rc;
}

#if L7_FEAT_SNMP_USER_MAPPING
L7_RC_t
snmpAgentUserAuthenticationTypeSet ( L7_uint32 UnitIndex, L7_uint32 UserIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentUserAuthenticationType_none:
    temp_val = L7_SNMP_USER_AUTH_PROTO_NONE;
    break;

  case D_agentUserAuthenticationType_hmacmd5:
    temp_val = L7_SNMP_USER_AUTH_PROTO_HMACMD5;
    break;

  case D_agentUserAuthenticationType_hmacsha:
    temp_val = L7_SNMP_USER_AUTH_PROTO_HMACSHA;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = userMgrLoginUserAuthenticationSet ( UserIndex, temp_val );
  }

  return rc;
}

L7_RC_t
snmpAgentUserEncryptionSet ( L7_uint32 UnitIndex, L7_uint32 UserIndex, L7_int32 val, L7_uchar8 *buf)
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentUserEncryptionType_none:
    temp_val = L7_SNMP_USER_PRIV_PROTO_NONE;
    break;

  case D_agentUserEncryptionType_des:
    temp_val = L7_SNMP_USER_PRIV_PROTO_DES;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = userMgrLoginUserEncryptionSet ( UserIndex, temp_val, buf );
  }

  return rc;
}
#endif /* L7_FEAT_SNMP_USER_MAPPING */


/**************************************************************************************************************/

L7_RC_t
snmpAgentLagConfigCreateSet(L7_uint32 UnitIndex, L7_char8 *buf, L7_uint32 *lagIntIfNum)
{
  L7_uint32 intIfNum;
  L7_uint32 member[L7_MAX_MEMBERS_PER_LAG]; /* internal interface numbers of lag members */

  /* Set all member values to zero, allowing us to create a lag without any members */
  /* Members will be created at a later stage using the add lag members command */
  bzero((L7_char8 *)member, sizeof(L7_uint32)*L7_MAX_MEMBERS_PER_LAG);

  if (buf[0] != 0)
  {
    /*create lag with 0 members*/
    if (usmDbDot3adCreateSet(UnitIndex,
                             buf,
                             FD_DOT3AD_ADMIN_MODE,
                             FD_DOT3AD_LINK_TRAP_MODE,
                             0,
                             USMDB_FD_DOT3AD_HASH_MODE,
                             member,
                             &intIfNum) == L7_SUCCESS )
    {
      /* Get the lag interface number for the row created for use in the UNDO function */
      *lagIntIfNum = intIfNum;
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

L7_RC_t
snmpLagIndexGet(L7_uint32 UnitIndex, L7_uint32 index)
{
  L7_uint32 intIfNum;

  if (usmDbIntIfNumFromExtIfNum(index, &intIfNum) != L7_SUCCESS)
    return L7_FAILURE;
  if (usmDbDot3adIsConfigured(UnitIndex, intIfNum) == L7_TRUE)
    return L7_SUCCESS;

  return L7_FAILURE;
}

L7_RC_t
snmpLagIndexGetNext(L7_uint32 UnitIndex, L7_uint32 *index)
{
  L7_uint32 tmpIntIfNum = 0;
  L7_uint32 tmpExtIfNum = 0;

  /* iterate through interface numbers */
  while (usmDbDot3adAggEntryGetNext(UnitIndex, tmpIntIfNum, &tmpIntIfNum) == L7_SUCCESS)
  {
    /* convert internal interface number to external interface number */
    if (usmDbExtIfNumFromIntIfNum(tmpIntIfNum, &tmpExtIfNum) == L7_SUCCESS)
    {
      /* if really the next ext interface number */
      if (tmpExtIfNum > *index)
      {
        *index = tmpExtIfNum;
        return L7_SUCCESS;
      }
    }
  }

  return L7_FAILURE;
}

L7_RC_t
snmpAgentLagSummaryLinkTrapGet ( L7_uint32 UnitIndex, L7_uint32 lagIntf, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbDot3adLinkTrapGet ( UnitIndex, lagIntf, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentLagSummaryLinkTrap_enable;
      break;

    case L7_DISABLE:
      *val = D_agentLagSummaryLinkTrap_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }
  return rc;
}

L7_RC_t
snmpAgentLagSummaryAdminModeGet ( L7_uint32 UnitIndex, L7_int32 lagIntf, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbDot3adAdminModeGet ( UnitIndex, lagIntf, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentLagSummaryAdminMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentLagSummaryAdminMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }
  return rc;
}

L7_RC_t
snmpAgentLagSummaryStpModeGet ( L7_uint32 UnitIndex, L7_int32 lagIntf, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbDot1sForceVersionGet(UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case DOT1S_FORCE_VERSION_DOT1D:
      *val = D_agentLagSummaryStpMode_dot1d;
      break;

    case DOT1S_FORCE_VERSION_DOT1W:
      *val = D_agentLagSummaryStpMode_fast;
      break;

    case DOT1S_FORCE_VERSION_DOT1S:
      *val = D_agentLagSummaryStpMode_dot1s;
      break;

    default:
      *val = D_agentLagSummaryStpMode_off;
      break;
    }
  }
  return rc;
}

L7_RC_t
snmpAgentLagSummaryTypeGet ( L7_uint32 UnitIndex, L7_int32 lagIndex, L7_int32 *val )
{
  L7_RC_t rc;
  L7_BOOL temp_bool = L7_FALSE;

  rc = usmDbDot3adIsStaticLag(UnitIndex, lagIndex, &temp_bool);

  if (rc == L7_SUCCESS)
  {
    switch (temp_bool)
    {
    case L7_TRUE:
      *val = D_agentLagSummaryType_static;
      break;
    case L7_FALSE:
      *val = D_agentLagSummaryType_dynamic;
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
snmpAgentLagSummaryStaticCapabilityGet ( L7_uint32 UnitIndex, L7_int32 lagIndex, L7_int32 *val )
{
  L7_RC_t rc;
  L7_BOOL temp_bool = L7_FALSE;

  /* - Need to make sure the usmdb API to use in this condition */
  rc = usmDbDot3adIsStaticLag(UnitIndex, lagIndex, &temp_bool);

  if (rc == L7_SUCCESS)
  {
    switch (temp_bool)
    {
    case L7_TRUE:
      *val = D_agentLagSummaryStaticCapability_enable;
      break;
    case L7_FALSE:
      *val = D_agentLagSummaryStaticCapability_disable;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

#ifdef I_agentLagSummaryHashMode
L7_RC_t
snmpAgentLagSummaryHashModeGet ( L7_uint32 UnitIndex, L7_int32 lagIndex, L7_int32 *val )
{
  L7_RC_t rc;

  /* - Need to make sure the usmdb API to use in this condition */
  rc = usmDbDot3adLagHashModeGet (UnitIndex, lagIndex, val);


  return rc;
}
#endif /* I_agentLagSummaryHashMode */

#ifdef I_agentLagSummarySwitchportMode
L7_RC_t
snmpAgentLagSummarySwitchportModeGet ( L7_uint32 UnitIndex, L7_int32 lagIndex, L7_int32 *val )
{
  L7_RC_t rc;
  L7_uint32 tempVal = 0;

  /* - Need to make sure the usmdb API to use in this condition */
  rc = usmDbDot1qSwPortModeGet(UnitIndex, lagIndex, &tempVal );
  if (rc == L7_SUCCESS)
  {
    switch (tempVal)
    {
      case DOT1Q_SWPORT_MODE_GENERAL:
        *val = D_agentLagSummarySwitchportMode_general;
        break;
      case DOT1Q_SWPORT_MODE_ACCESS:
        *val = D_agentLagSummarySwitchportMode_access;
        break;
      case DOT1Q_SWPORT_MODE_TRUNK:
        *val = D_agentLagSummarySwitchportMode_trunk;
        break;
      default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}
#endif /* I_agentLagSummarySwitchportMode */

L7_RC_t
snmpAgentLagSummaryHashOptionGet ( L7_uint32 UnitIndex, L7_int32 lagIndex, L7_int32 *val )
{
  L7_RC_t   rc = L7_FAILURE;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  if ( usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_INTF_FEATURE_ID) == L7_TRUE)
  {
    rc = usmDbDot3adLagHashModeGet(UnitIndex, lagIndex, val);
  }
  return rc;
}

L7_RC_t
snmpAgentLagConfigGroupHashOptionGet(L7_uint32 UnitIndex, L7_int32 *val)
{
  L7_RC_t   rc = L7_FAILURE;

  if ( usmDbFeaturePresentCheck(UnitIndex, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_GLOBAL_FEATURE_ID) == L7_TRUE)
  {
    rc = usmDbDot3adSystemHashModeGet(UnitIndex, val);
  }
  return rc;
}

L7_RC_t
snmpAgentLagConfigGroupHashOptionSet ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL temp_bool = L7_FALSE;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  if (usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_GLOBAL_FEATURE_ID) == L7_FALSE)
  {
    return L7_FAILURE;
  }

  switch (val)
  {
    case L7_DOT3AD_HASHING_MODE_SA_VLAN:
    temp_bool = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_SA_VLAN_ETYPE_INTF_FEATURE_ID);
    if (temp_bool != L7_TRUE)
     temp_bool = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_SA_FEATURE_ID);
    break;
    case L7_DOT3AD_HASHING_MODE_DA_VLAN:
    temp_bool = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_DA_VLAN_ETYPE_INTF_FEATURE_ID);
    if (temp_bool != L7_TRUE)
     temp_bool = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_DA_FEATURE_ID);
    break;
    case L7_DOT3AD_HASHING_MODE_SDA_VLAN:
    temp_bool = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_SADA_VLAN_ETYPE_INTF_FEATURE_ID);
    if (temp_bool != L7_TRUE)
     temp_bool = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_SADA_FEATURE_ID);
    break;
    case L7_DOT3AD_HASHING_MODE_SIP_SPORT:
    temp_bool = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_SIP_SPORT_FEATURE_ID);
    break;
    case L7_DOT3AD_HASHING_MODE_DIP_DPORT:
    temp_bool = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_DIP_DPORT_FEATURE_ID);
    break;
    case L7_DOT3AD_HASHING_MODE_SDIP_DPORT:
    temp_bool = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_SIP_DIP_SPORT_DPORT_FEATURE_ID);
    break;
    case L7_DOT3AD_HASHING_MODE_ENHANCED:
    temp_bool = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_ENHANCED_FEATURE_ID);
    break;
    default:
    return L7_FAILURE;
    break;
  }

  if (temp_bool == L7_TRUE)
  {
    rc = usmDbDot3adSystemHashModeSet(UnitIndex, val);
  }

  return rc;
}

L7_RC_t
snmpAgentLagSummaryLinkTrapSet  ( L7_uint32 UnitIndex, L7_int32 lagIntf, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentLagSummaryLinkTrap_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentLagSummaryLinkTrap_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDot3adLinkTrapSet(UnitIndex, lagIntf, temp_val);
  }

  return rc;
}

L7_RC_t
snmpAgentLagSummaryAdminModeSet ( L7_uint32 UnitIndex, L7_int32 lagIntf, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentLagSummaryAdminMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentLagSummaryAdminMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDot3adAdminModeSet(UnitIndex, lagIntf, temp_val);
  }

  return rc;
}

L7_RC_t
snmpAgentLagSummaryStpModeSet ( L7_uint32 UnitIndex, L7_int32 lagIntf, L7_int32 val )
{
  L7_BOOL temp_bool = L7_FALSE;
  L7_RC_t rc = L7_SUCCESS;

    switch (val)
    {
    case D_agentLagSummaryStpMode_dot1s:
      temp_bool = L7_TRUE;
      break;

    case D_agentLagSummaryStpMode_off:
      temp_bool = L7_FALSE;
      break;

    default:
      rc = L7_FAILURE;
    }

    if (rc == L7_SUCCESS)
    {
      rc = usmDbDot1sPortStateSet(UnitIndex, lagIntf, temp_bool);
    }

  return rc;
}

L7_RC_t
snmpAgentLagSummaryAddPortSet ( L7_uint32 UnitIndex, L7_int32 lagIntf, L7_int32 val )
{
  L7_RC_t rc = L7_SUCCESS;

  L7_uint32 memberIntf;
  L7_uint32 memberSpeed;
  L7_BOOL flag = L7_FALSE;
  L7_uint32 lacpMode;

  if (val == 0)
  {
    return L7_FAILURE;
  }

  rc = usmDbIntIfNumFromExtIfNum(val, &memberIntf);
  if (rc == L7_SUCCESS)
  {
    if(usmDbDot3adIsStaticLag(UnitIndex, lagIntf, &flag) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    if(flag != L7_TRUE)
    {
      /*This LAG is dynamically maintained. Need to verify the LACP mode */
      if((usmDbDot3adAggPortLacpModeGet(UnitIndex, memberIntf,
                                        &lacpMode) != L7_SUCCESS) ||
         (lacpMode == L7_DISABLE))
      {
        return L7_FAILURE;
      }
    }
/* lvl7_@p1493 start */
    rc = usmDbIfSpeedGet(UnitIndex, memberIntf, &memberSpeed);
    if (rc == L7_SUCCESS)
    {
      /* member port must be configured to Full duplex to be added to a lag */
      switch (memberSpeed)
      {
      case L7_PORTCTRL_PORTSPEED_FULL_10T:
      case L7_PORTCTRL_PORTSPEED_FULL_100TX:
      case L7_PORTCTRL_PORTSPEED_FULL_100FX:
      case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
      case L7_PORTCTRL_PORTSPEED_FULL_2P5FX:    /* PTin added: Speed 2.5G */
      case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
      case L7_PORTCTRL_PORTSPEED_FULL_40G_KR4:  /* PTin added: Speed 40G */
      case L7_PORTCTRL_PORTSPEED_FULL_100G_BKP: /* PTin added: Speed 100G */
      case L7_PORTCTRL_PORTSPEED_UNKNOWN: /* preconfigured or link-down port */
        /* let lag determine if the port speed and type are correct for this lag */
        rc = usmDbDot3adMemberAddSet(UnitIndex, lagIntf, memberIntf);
        break;
      default:
        rc = L7_FAILURE;
        break;
      }
    }
/* lvl7_@p1493 end */
  }

  return rc;
}

L7_RC_t
snmpAgentLagSummaryDeletePortSet ( L7_uint32 UnitIndex, L7_int32 lagIntf, L7_int32 val )
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 toDelete;

  /* lagintf in inter, val in ext */
  rc = usmDbIntIfNumFromExtIfNum(val, &toDelete);
  if (rc == L7_SUCCESS)
  {
    rc = usmDbDot3adMemberDeleteSet(UnitIndex, lagIntf, toDelete);
  }

  return rc;
}

L7_RC_t
snmpAgentLagSummaryStatusSet ( L7_uint32 UnitIndex, L7_int32 lagIntf, L7_int32 val )
{
  L7_RC_t rc;

  switch (val)
  {
  case D_agentLagSummaryStatus_active:
    rc = L7_SUCCESS;
    break;

  case D_agentLagSummaryStatus_destroy:
    rc = usmDbDot3adRemoveSet(UnitIndex, lagIntf);
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  return rc;
}

L7_RC_t
snmpAgentLagSummaryStaticCapabilitySet ( L7_uint32 UnitIndex, L7_int32 lagIntf, L7_int32 val )
{
  L7_BOOL temp_bool = L7_FALSE;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentLagSummaryStaticCapability_enable:
    temp_bool = L7_TRUE;
    break;

  case D_agentLagSummaryStaticCapability_disable:
    temp_bool = L7_FALSE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDot3adLagStaticModeSet(UnitIndex, lagIntf, temp_bool);
  }

  return rc;
}

L7_RC_t
snmpAgentLagSummaryHashOptionSet ( L7_uint32 UnitIndex, L7_int32 lagIntf, L7_int32 val )
{
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL temp_bool = L7_FALSE;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  if (usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_INTF_FEATURE_ID) == L7_FALSE)
  {
    return L7_FAILURE;
  }

  switch (val)
  {
    case L7_DOT3AD_HASHING_MODE_SA_VLAN:
    temp_bool = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_SA_VLAN_ETYPE_INTF_FEATURE_ID);
    break;
    case L7_DOT3AD_HASHING_MODE_DA_VLAN:
    temp_bool = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_DA_VLAN_ETYPE_INTF_FEATURE_ID);
    break;
    case L7_DOT3AD_HASHING_MODE_SDA_VLAN:
    temp_bool = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_SADA_VLAN_ETYPE_INTF_FEATURE_ID);
    break;
    case L7_DOT3AD_HASHING_MODE_SIP_SPORT:
    temp_bool = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_SIP_SPORT_FEATURE_ID);
    break;
    case L7_DOT3AD_HASHING_MODE_DIP_DPORT:
    temp_bool = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_DIP_DPORT_FEATURE_ID);
    break;
    case L7_DOT3AD_HASHING_MODE_SDIP_DPORT:
    temp_bool = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_SIP_DIP_SPORT_DPORT_FEATURE_ID);
    break;
    case L7_DOT3AD_HASHING_MODE_ENHANCED:
    temp_bool = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_ENHANCED_FEATURE_ID);
    break;
    default:
    return L7_FAILURE;
    break;
  }

  if (temp_bool == L7_TRUE)
  {
    rc = usmDbDot3adLagHashModeSet(UnitIndex, lagIntf, val);
  }

  return rc;
}

#ifdef I_agentLagSummarySwitchportMode
L7_RC_t
snmpAgentLagSummarySwitchportModeSet ( L7_uint32 UnitIndex, L7_int32 lagIntf, L7_int32 val )
{
  L7_RC_t rc = L7_FAILURE;
  L7_int32 swportMode;
  NIM_INTF_MASK_t intfMask;
  memset(&intfMask, 0, sizeof(intfMask));
  NIM_INTF_SETMASKBIT(intfMask,lagIntf);
  switch (val)
  {
    case D_agentLagSummarySwitchportMode_general:
      swportMode = DOT1Q_SWPORT_MODE_GENERAL;
      break;
    case D_agentLagSummarySwitchportMode_access:
      swportMode = DOT1Q_SWPORT_MODE_ACCESS;
      break;
    case D_agentLagSummarySwitchportMode_trunk:
      swportMode = DOT1Q_SWPORT_MODE_TRUNK;
      break;
    default:
    /* unknown value */
    return L7_FAILURE;
  }

  rc = usmDbDot1qSwPortModeSet(UnitIndex, &intfMask, swportMode);

  return rc;
}
#endif


/**************************************************************************************************************/
L7_RC_t
snmpLagMemberIfIndexGet(L7_uint32 UnitIndex, L7_uint32 lagIndex, L7_uint32 ifIndex)
{
  L7_uint32 lagIntIfNum, memberIntIfNum;

  if ((usmDbIntIfNumFromExtIfNum(lagIndex, &lagIntIfNum) == L7_SUCCESS) &&
      (usmDbIntIfNumFromExtIfNum(ifIndex, &memberIntIfNum) == L7_SUCCESS) &&
      (usmDbDot3adMemberCheck(UnitIndex, lagIntIfNum, memberIntIfNum) == L7_SUCCESS))
    return L7_SUCCESS;

  return L7_FAILURE;
}

L7_RC_t
snmpLagMemberIfIndexGetNext(L7_uint32 UnitIndex, L7_uint32 lagIndex, L7_uint32 *ifIndex)
{
  while (usmDbGetNextVisibleExtIfNumber(*ifIndex, ifIndex) == L7_SUCCESS)
  {
    if (snmpLagMemberIfIndexGet(UnitIndex, lagIndex, *ifIndex) == L7_SUCCESS)
      return L7_SUCCESS;
  }
  return L7_FAILURE;
}


L7_RC_t
snmpLagDetailedConfigEntryGet ( L7_uint32 UnitIndex, L7_uint32 lagIndex, L7_uint32 ifIndex)
{
  if (snmpLagIndexGet(UnitIndex, lagIndex) == L7_SUCCESS &&
      snmpLagMemberIfIndexGet(UnitIndex, lagIndex, ifIndex) == L7_SUCCESS)
    return L7_SUCCESS;

  return L7_FAILURE;
}


L7_RC_t
snmpLagDetailedConfigEntryNext ( L7_uint32 UnitIndex, L7_uint32 *lagIndex, L7_uint32 *ifIndex)
{
  /* try to get the next member for this lag*/
  if (snmpLagMemberIfIndexGetNext(UnitIndex, *lagIndex, ifIndex) == L7_SUCCESS)
    return L7_SUCCESS;

  /* If there are no more members in the current lag loop through available lags */
  while (snmpLagIndexGetNext(UnitIndex, lagIndex) == L7_SUCCESS)
  {
    *ifIndex = 0;
    if (snmpLagMemberIfIndexGetNext(UnitIndex, *lagIndex, ifIndex) == L7_SUCCESS)
      return L7_SUCCESS;
  }

  /* no more lags */
  return L7_FAILURE;
}


L7_RC_t
snmpAgentLagDetailedPortSpeedGet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uchar8 *buf)
{
  L7_uint32 ifSpeed;
  L7_uint32 ifConnectorType;
  L7_RC_t rc;

  rc = usmDbIfSpeedGet(UnitIndex, intIfIndex, &ifSpeed);

  if (rc == L7_SUCCESS)
  {
    switch (ifSpeed)
    {
    case L7_PORTCTRL_PORTSPEED_HALF_10T:
      strcpy(buf, "dot3MauType10BaseTHD");
      break;

    case L7_PORTCTRL_PORTSPEED_FULL_10T:
      strcpy(buf, "dot3MauType10BaseTFD");
      break;

    case L7_PORTCTRL_PORTSPEED_HALF_100TX:
      strcpy(buf, "dot3MauType100BaseTXHD");
      break;

    case L7_PORTCTRL_PORTSPEED_FULL_100TX:
      strcpy(buf, "dot3MauType100BaseTXFD");
      break;

    case L7_PORTCTRL_PORTSPEED_FULL_100FX:
      strcpy(buf, "dot3MauType100BaseFXFD");
      break;

    case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
      rc = usmDbIntfConnectorTypeGet(intIfIndex, &ifConnectorType);

      if (rc == L7_SUCCESS)
      {
        switch (ifConnectorType)
        {
        case L7_MTRJ:
          strcpy(buf, "dot3MauType1000BaseSXFD");
          break;

        case L7_RJ45:
          strcpy(buf, "dot3MauType1000BaseTFD");
          break;

        default:
          strcpy(buf, "dot3MauType1000BaseXFD");
          break;
        }
      }
      else
      {
        /* if connector type is unknown, return the default */
        strcpy(buf, "dot3MauType1000BaseXFD");
        rc = L7_SUCCESS;
      }

      break;

    /* PTin NOTE (2.5G)
     * There is no ethernet physical medium defined for 2.5G! This speed
     * appeared with SGMII and there is not IEEE standard definition. So there
     * is no point in referencing any physical medium over here.
     */

    case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
      strcpy(buf, "dot3MauType10GigBaseX");
      break;

    default:
      strcpy(buf, "0.0");
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentLagDetailedPortStatusGet(L7_uint32 UnitIndex, L7_uint32 lagIndex, L7_uint32 intIfIndex, L7_uint32 *val)
{
  L7_RC_t rc;

  rc = usmDbDot3adIsActiveMember(UnitIndex, intIfIndex);

  if (rc == L7_SUCCESS)
  {
    *val = D_agentLagDetailedPortStatus_active;
  }
  else
  {
    *val = D_agentLagDetailedPortStatus_inactive;
  }

  return L7_SUCCESS;
}

/**************************************************************************************************************/
/* lvl7_@p2115 start */
/* lvl7_@p1261 start */
L7_RC_t
snmpAgentNetworkIPAddressSet( L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 netMask = 0;
  L7_uint32 gateway;
  L7_uint32 sysCfgPotocol;
  L7_uint32 errorNum;

  /* protocol must be set to none to be configured */
  usmDbAgentBasicConfigNetworkConfigProtocolDesiredGet(UnitIndex, &sysCfgPotocol);
  if (sysCfgPotocol != L7_SYSCONFIG_MODE_NONE)
  {
      return L7_FAILURE;
  }

  if ( (usmDbAgentIpIfNetMaskGet(UnitIndex, &netMask) == L7_SUCCESS) &&
       (usmDbAgentIpIfDefaultRouterGet(UnitIndex, &gateway) == L7_SUCCESS) )
  {
    if (netMask == 0)
      netMask = L7_SNMP_DEFAULT_NETMASK;
    if (usmDbIpInfoValidate(val, netMask, gateway) == L7_SUCCESS)
      rc = usmDbSystemIPAndNetMaskSet(UnitIndex, val, netMask, &errorNum);
  }

  return rc;
}

L7_RC_t
snmpAgentNetworkSubnetMaskSet( L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 ipAddr;
  L7_uint32 gateway;
  L7_uint32 sysCfgPotocol;
  L7_uint32 errorNum;

  /* protocol must be set to none to be configured */
  usmDbAgentBasicConfigNetworkConfigProtocolDesiredGet(UnitIndex, &sysCfgPotocol);
  if (sysCfgPotocol != L7_SYSCONFIG_MODE_NONE)
  {
      return L7_FAILURE;
  }

  if ( (usmDbAgentIpIfAddressGet(UnitIndex, &ipAddr) == L7_SUCCESS) &&
       (usmDbAgentIpIfDefaultRouterGet(UnitIndex, &gateway) == L7_SUCCESS) )
  {
    if (usmDbIpInfoValidate(ipAddr, val, gateway) == L7_SUCCESS)
      rc = usmDbSystemIPAndNetMaskSet(UnitIndex, ipAddr, val, &errorNum);
  }

  return rc;
}
/* lvl7_@p1261 end */

L7_RC_t
snmpAgentNetworkDefaultGatewaySet( L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 ipAddr  = 0;
  L7_uint32 netMask = 0;
  L7_uint32 gateway = 0;
  L7_uint32 sysCfgPotocol = 0;
  L7_uint32 temp_errorNum = 0;

  /* protocol must be set to none to be configured */
  usmDbAgentBasicConfigNetworkConfigProtocolDesiredGet(UnitIndex, &sysCfgPotocol);
  if (sysCfgPotocol != L7_SYSCONFIG_MODE_NONE)
  {
      return L7_FAILURE;
  }

  if ( (usmDbAgentIpIfAddressGet(UnitIndex, &ipAddr) == L7_SUCCESS) &&
       (usmDbAgentIpIfNetMaskGet(UnitIndex, &netMask) == L7_SUCCESS) &&
       (usmDbAgentIpIfDefaultRouterGet(UnitIndex, &gateway) == L7_SUCCESS))
  {
    /* if the ip and netmask are valid in the new gateway, first clear the gateway, reset the ip and netmask, and then set the new gateway */
    if (usmDbIpInfoValidate(ipAddr, netMask, val) == L7_SUCCESS)
    {
      if (usmDbAgentIpIfDefaultRouterSet(UnitIndex,0) == L7_SUCCESS)
      {
        if (usmDbSystemIPAndNetMaskSet(UnitIndex, ipAddr, netMask, &temp_errorNum) == L7_SUCCESS)
          rc = usmDbAgentIpIfDefaultRouterSet(UnitIndex, val);
      }
      else
      {
        /* reset the gateway back */
        (void)usmDbAgentIpIfDefaultRouterSet(UnitIndex,gateway);
      }
    }
  }

  return rc;
}
/* lvl7_@2115 end */

L7_RC_t
snmpAgentNetworkMacAddressTypeGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  /* some platforms does not require LOCAL-MAC-TYPE, may always use BURNED-IN MAC Type */
  if (usmDbFeaturePresentCheck(UnitIndex, L7_SIM_COMPONENT_ID, L7_SIM_ALLOW_LOCAL_MAC_USAGE_FEATURE_ID) != L7_TRUE)
  {
    *val = D_agentNetworkMacAddressType_burned_in;
    return L7_SUCCESS;
  }

  rc = usmDbSwDevCtrlMacAddrTypeGet ( UnitIndex, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_SYSMAC_BIA:
      *val = D_agentNetworkMacAddressType_burned_in;
      break;

    case L7_SYSMAC_LAA:
      *val = D_agentNetworkMacAddressType_local;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}


L7_RC_t
snmpAgentNetworkConfigProtocolGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbAgentBasicConfigNetworkConfigProtocolDesiredGet ( UnitIndex, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_SYSCONFIG_MODE_NONE:
      *val = D_agentNetworkConfigProtocol_none;
      break;

    case L7_SYSCONFIG_MODE_BOOTP:
      *val = D_agentNetworkConfigProtocol_bootp;
      break;
    case L7_SYSCONFIG_MODE_DHCP:
      *val = D_agentNetworkConfigProtocol_dhcp;
      break;
    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentNetworkWebModeGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbSwDevCtrlWebMgmtModeGet ( UnitIndex, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentNetworkWebMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentNetworkWebMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentNetworkJavaModeGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbWebJavaModeGet ( UnitIndex, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentNetworkJavaMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentNetworkJavaMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

/* lvl7_@p0862 start */
L7_RC_t
snmpAgentNetworkLocalAdminMacAddressSet (L7_uint32 UnitIndex, L7_uchar8 *buf)
{
  /* some platforms does not require LOCAL-MAC-TYPE, may always use BURNED-IN MAC Type */
  if (usmDbFeaturePresentCheck(UnitIndex, L7_SIM_COMPONENT_ID, L7_SIM_ALLOW_LOCAL_MAC_USAGE_FEATURE_ID) != L7_TRUE)
    return L7_FAILURE;

  /* bit 1 of byte 0 must be a 1 and bit 0 of byte 1 must be 0*/
  if (buf[0] & 0x2 && !(buf[0] & 0x1))
  {
    return usmDbSwDevCtrlLocalAdminAddrSet(UnitIndex, buf);
  }

  return L7_FAILURE;
}
/* lvl7_@p0862 end */


L7_RC_t
snmpAgentNetworkMacAddressTypeSet ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentNetworkMacAddressType_burned_in:
    temp_val = L7_SYSMAC_BIA;
    break;

/* lvl7_@p0863 start */
  case D_agentNetworkMacAddressType_local:
    temp_val = L7_SYSMAC_LAA;
    break;
/* lvl7_@p0863 end */

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    if (usmDbFeaturePresentCheck(UnitIndex, L7_SIM_COMPONENT_ID, L7_SIM_ALLOW_LOCAL_MAC_USAGE_FEATURE_ID) == L7_TRUE)
      rc = usmDbSwDevCtrlMacAddrTypeSet (UnitIndex, temp_val);
    else /* some platforms does not require LOCAL-MAC-TYPE, may always use BURNED-IN MAC Type */
      rc = L7_FAILURE;
  }

  return rc;
}

L7_RC_t
snmpAgentNetworkConfigProtocolSet ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0, oldVal;
  L7_RC_t rc = L7_SUCCESS;

  usmDbAgentBasicConfigNetworkConfigProtocolDesiredGet(UnitIndex, &oldVal);

  usmDbAgentIpIfDefaultRouterSet(UnitIndex, 0);
  usmDbAgentIpIfAddressSet(UnitIndex, 0);
  usmDbAgentIpIfNetMaskSet(UnitIndex, 0);

  switch (val)
  {
  case  D_agentNetworkConfigProtocol_none:
    temp_val = L7_SYSCONFIG_MODE_NONE;
    break;

  case D_agentNetworkConfigProtocol_bootp:
    temp_val = L7_SYSCONFIG_MODE_BOOTP;
    break;

  case D_agentNetworkConfigProtocol_dhcp:
      temp_val = L7_SYSCONFIG_MODE_DHCP;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbAgentBasicConfigProtocolDesiredSet ( UnitIndex, temp_val );

    if (oldVal == L7_SYSCONFIG_MODE_BOOTP)
    {
      usmDbBootpTaskReInit(USMDB_UNIT_CURRENT);
    }
  }

  return rc;
}

L7_RC_t
snmpAgentNetworkWebModeSet ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentNetworkWebMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentNetworkWebMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbSwDevCtrlWebMgmtModeSet ( UnitIndex, temp_val );
  }

  return rc;
}

L7_RC_t
snmpAgentNetworkJavaModeSet ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentNetworkJavaMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentNetworkJavaMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbWebJavaModeSet ( UnitIndex, temp_val );
  }

  return rc;
}

/**************************************************************************************************************/

/* lvl7_@p2115 start */
/* lvl7_@p1261 start */
L7_RC_t
snmpAgentServicePortIPAddressSet( L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 netMask = 0;
  L7_uint32 gateway;
  L7_uint32 sysCfgPotocol;
  L7_uint32 errorNum;

  /* protocol must be set to none to be configured */
  usmDbAgentBasicConfigServPortConfigProtocolDesiredGet(UnitIndex, &sysCfgPotocol);
  if (sysCfgPotocol != L7_SYSCONFIG_MODE_NONE)
  {
      return L7_FAILURE;
  }

  if ( (usmDbServicePortNetMaskGet(UnitIndex, &netMask) == L7_SUCCESS) &&
       (usmDbServicePortGatewayGet(UnitIndex, &gateway) == L7_SUCCESS) )
  {
    if (netMask == 0)
      netMask = L7_SNMP_DEFAULT_NETMASK;
    if (usmDbIpInfoValidate(val, netMask, gateway) == L7_SUCCESS)
      rc = usmDbServicePortIPAndNetMaskSet(UnitIndex, val, netMask, &errorNum);
  }

  return rc;
}

L7_RC_t
snmpAgentServicePortSubnetMaskSet( L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 ipAddr;
  L7_uint32 gateway;
  L7_uint32 sysCfgPotocol;
  L7_uint32 errorNum;

  /* protocol must be set to none to be configured */
  usmDbAgentBasicConfigServPortConfigProtocolDesiredGet(UnitIndex, &sysCfgPotocol);
  if (sysCfgPotocol != L7_SYSCONFIG_MODE_NONE)
  {
      return L7_FAILURE;
  }

  if ( (usmDbServicePortConfiguredIpAddrGet(&ipAddr) == L7_SUCCESS) &&
       (usmDbServicePortConfiguredGatewayGet(&gateway) == L7_SUCCESS) )
  {
    if (usmDbIpInfoValidate(ipAddr, val, gateway) == L7_SUCCESS)
      rc = usmDbServicePortIPAndNetMaskSet(UnitIndex, ipAddr, val, &errorNum);
  }

  return rc;
}
/* lvl7_@p1261 end */

L7_RC_t
snmpAgentServicePortDefaultGatewaySet( L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 ipAddr  = 0;
  L7_uint32 netMask = 0;
  L7_uint32 gateway = 0;
  L7_uint32 sysCfgPotocol = 0;
  L7_uint32 temp_errorNum = 0;

  /* protocol must be set to none to be configured */
  usmDbAgentBasicConfigServPortConfigProtocolDesiredGet(UnitIndex, &sysCfgPotocol);
  if (sysCfgPotocol != L7_SYSCONFIG_MODE_NONE)
  {
      return L7_FAILURE;
  }

  if ( (usmDbServicePortConfiguredIpAddrGet(&ipAddr) == L7_SUCCESS) &&
       (usmDbServicePortConfiguredNetMaskGet(&netMask) == L7_SUCCESS) &&
       (usmDbServicePortConfiguredGatewayGet(&gateway) == L7_SUCCESS))
  {
    /* if the ip and netmask are valid in the new gateway, first clear the gateway, 
     * reset the ip and netmask, and then set the new gateway */
    if (usmDbIpInfoValidate(ipAddr, netMask, val) == L7_SUCCESS)
    {
      if (usmDbServicePortGatewaySet(UnitIndex,0) == L7_SUCCESS)
      {
        if (usmDbServicePortIPAndNetMaskSet(UnitIndex, ipAddr, netMask, &temp_errorNum) == L7_SUCCESS)
          rc = usmDbServicePortGatewaySet(UnitIndex, val);
      }
      else
      {
        /* reset the gateway back */
        (void)usmDbServicePortGatewaySet(UnitIndex,gateway);
      }
    }
  }

  return rc;
}

/* lvl7_@p2115 start */

L7_RC_t
snmpAgentServicePortConfigProtocolGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbAgentBasicConfigServPortConfigProtocolDesiredGet ( UnitIndex, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_SYSCONFIG_MODE_NONE:
      *val = D_agentServicePortConfigProtocol_none;
      break;

    case L7_SYSCONFIG_MODE_BOOTP:
      *val = D_agentServicePortConfigProtocol_bootp;
      break;
    case L7_SYSCONFIG_MODE_DHCP:
      *val = D_agentServicePortConfigProtocol_dhcp;
      break;
    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}


L7_RC_t
snmpAgentServicePortConfigProtocolSet  ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0, oldVal;
  L7_RC_t rc = L7_SUCCESS;

  usmDbAgentBasicConfigServPortConfigProtocolDesiredGet(UnitIndex, &oldVal);

  usmDbServicePortGatewaySet(UnitIndex, 0);
  usmDbServicePortIPAddrSet(UnitIndex, 0);
  usmDbServicePortNetMaskSet(UnitIndex, 0);

  switch (val)
  {
  case D_agentServicePortConfigProtocol_none:
    temp_val = L7_SYSCONFIG_MODE_NONE;
    break;

  case D_agentServicePortConfigProtocol_bootp:
    temp_val = L7_SYSCONFIG_MODE_BOOTP;
    break;

  case D_agentServicePortConfigProtocol_dhcp:
      temp_val = L7_SYSCONFIG_MODE_DHCP;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbAgentBasicConfigServPortProtocolDesiredSet ( UnitIndex, temp_val );

    if (oldVal == L7_SYSCONFIG_MODE_BOOTP)
    {
        usmDbBootpTaskReInit(USMDB_UNIT_CURRENT);
    }
  }

  return rc;
}

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
L7_RC_t
snmpAgentNetworkIpv6AdminModeGet(L7_uint32 UnitIndex, L7_int32 *val)
{
  L7_uint32 temp_val;
  temp_val = usmDbAgentIpIfIPV6AdminModeGet();

  switch(temp_val)
  {
    case L7_ENABLE:
      *val = D_agentNetworkIpv6AdminMode_enabled;
      break;

    case L7_DISABLE:
      *val = D_agentNetworkIpv6AdminMode_disabled;
      break;

    default:
      *val = D_agentNetworkIpv6AdminMode_enabled;
      break;
  }
  return L7_SUCCESS;
}

L7_RC_t
snmpAgentNetworkIpv6GatewayGet( L7_uint32 UnitIndex, L7_char8 *gateway_buff)
{
  L7_in6_addr_t gateway;
  L7_uchar8     str_addr[64];

  if(usmDbAgentIpIfIPV6GatewayGet(&gateway)==L7_SUCCESS)
  {
    osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)&gateway, str_addr,IPV6_DISP_ADDR_LEN);
    sprintf(gateway_buff,"%s",str_addr);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}


L7_RC_t
snmpAgentNetworkIpv6AdminModeSet( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;
  switch (val)
  {
    case D_agentNetworkIpv6AdminMode_enabled:
      temp_val = L7_ENABLE;
      break;

    case D_agentNetworkIpv6AdminMode_disabled:
      temp_val = L7_DISABLE;
      break;

    default:
      rc = L7_FAILURE;
      break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbAgentIpIfIPV6AdminModeSet( temp_val );
  }
  return rc;
}

L7_RC_t
snmpAgentNetworkIpv6GatewaySet(L7_uint32 UnitIndex, L7_char8 *gateway_buff)
{
  L7_RC_t rc = L7_FAILURE;
  L7_in6_addr_t gateway;

  if(osapiInetPton(L7_AF_INET6, gateway_buff, (L7_uchar8 *)&gateway)==L7_SUCCESS)
  {
    rc=(usmDbAgentIpIfIPV6GatewaySet(&gateway));
  }
  return rc;
}

#ifdef I_agentNetworkIpv6AddressAutoConfig
L7_RC_t
snmpAgentNetworkIPv6AutoConfigGet(L7_int32 *val )
{
  L7_uint32 autoConfig;
  L7_RC_t rc = L7_FAILURE;

  rc = usmDbNetworkIPv6AddrAutoConfigGet(&autoConfig);
  if (rc == L7_SUCCESS)
  {
     if (autoConfig == L7_ENABLE)
     {
        *val = D_agentNetworkIpv6AddressAutoConfig_enable;
     }
     else
     {
        *val = D_agentNetworkIpv6AddressAutoConfig_disable;
     }
  }
  return rc;
}

L7_RC_t
snmpAgentNetworkIPv6AutoConfigSet(L7_int32 val )
{
  L7_uint32 protocol, servAutoConfig, networkProtocol, temp_val;
  L7_RC_t rc = L7_SUCCESS;

  (void) usmDbAgentBasicConfigServPortIPv6ConfigProtocolGet(USMDB_UNIT_CURRENT,&protocol);
  (void) usmDbServPortIPv6AddrAutoConfigGet(&servAutoConfig);
  (void) usmDbAgentBasicConfigNetworkIPv6ConfigProtocolGet(USMDB_UNIT_CURRENT,&networkProtocol);


  switch (val)
  {
    case D_agentNetworkIpv6AddressAutoConfig_disable:
      temp_val = L7_DISABLE;
      break;

    case D_agentNetworkIpv6AddressAutoConfig_enable:
      /* DHCP can only be set for either the service port or network port, not both */
       if ((servAutoConfig != L7_ENABLE) && (networkProtocol!= L7_SYSCONFIG_MODE_DHCP)
           && (protocol != L7_SYSCONFIG_MODE_DHCP))
       {
          temp_val = L7_ENABLE;
       }
       else
       {
         rc = L7_FAILURE;
       }
      break;
   default:
     rc = L7_FAILURE;
  }
  if (rc == L7_SUCCESS)
  {
     rc = usmDbNetworkIPv6AddrAutoConfigSet(temp_val);
  }
  return rc;
}
#endif

#ifdef I_agentNetworkIpv6ConfigProtocol
L7_RC_t
snmpAgentNetworkIPv6ConfigProtocolGet(L7_int32 *val )
{
  L7_uint32 protocol;
  L7_RC_t rc = L7_SUCCESS;
  rc = usmDbAgentBasicConfigNetworkIPv6ConfigProtocolGet(USMDB_UNIT_CURRENT, &protocol);
  if (rc == L7_SUCCESS)
  {
    if (protocol == L7_SYSCONFIG_MODE_NONE)
    {
      *val = D_agentNetworkIpv6ConfigProtocol_none;
    }
    if (protocol == L7_SYSCONFIG_MODE_DHCP)
    {
      *val = D_agentNetworkIpv6ConfigProtocol_dhcp;
    }
  }
  return rc;
}

L7_RC_t
snmpAgentNetworkIPv6ConfigProtocolSet(L7_int32 val )
{
  L7_uint32 protocol, servAutoConfig, autoConfig, temp_val;
  L7_RC_t rc = L7_SUCCESS;

  (void) usmDbAgentBasicConfigServPortIPv6ConfigProtocolGet(USMDB_UNIT_CURRENT,&protocol);
  (void) usmDbServPortIPv6AddrAutoConfigGet(&servAutoConfig);
  (void) usmDbNetworkIPv6AddrAutoConfigGet(&autoConfig);

  switch (val)
  {
  case D_agentNetworkIpv6ConfigProtocol_none:
    temp_val = L7_SYSCONFIG_MODE_NONE;
    break;

  case D_agentNetworkIpv6ConfigProtocol_dhcp:
    /* DHCP can only be set for either the service port or network port, not both */
    if ((servAutoConfig != L7_ENABLE) && (autoConfig != L7_ENABLE) && (protocol != L7_SYSCONFIG_MODE_DHCP))
    {
      temp_val = L7_SYSCONFIG_MODE_DHCP;
    }
    else
    {
      rc = L7_FAILURE;
    }
    break;

  default:
    rc = L7_FAILURE;
  }
  if (rc == L7_SUCCESS)
  {
     rc = usmDbAgentBasicIPv6ConfigProtocolSet(USMDB_UNIT_CURRENT, temp_val);
  }
  return rc;
}
#endif

/**************************************************************************************/
L7_RC_t
snmpAgentServicePortIpv6AdminModeGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;

  temp_val = usmDbServicePortIPV6AdminModeGet();
  switch (temp_val)
  {
    case L7_ENABLE:
      *val = D_agentServicePortIpv6AdminMode_enabled;
      break;

    case L7_DISABLE:
      *val = D_agentServicePortIpv6AdminMode_disabled;
      break;

    default:
      *val = D_agentServicePortIpv6AdminMode_enabled;
      break;
  }
  return L7_SUCCESS;
}

L7_RC_t
snmpAgentServicePortIpv6GatewayGet( L7_uint32 UnitIndex, L7_char8 *gateway_buff)
{
  L7_in6_addr_t gateway;
  L7_uchar8     str_addr[64];

  if(usmDbServicePortIPV6GatewayGet(&gateway)==L7_SUCCESS)
  {
    osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)&gateway, str_addr,IPV6_DISP_ADDR_LEN);
    sprintf(gateway_buff,"%s",str_addr);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

L7_RC_t
snmpAgentServicePortIpv6AdminModeSet( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;
  switch (val)
  {
    case D_agentServicePortIpv6AdminMode_enabled:
      temp_val = L7_ENABLE;
      break;

    case D_agentServicePortIpv6AdminMode_disabled:
      temp_val = L7_DISABLE;
      break;

    default:
      rc = L7_FAILURE;
      break;
  }
  if (rc == L7_SUCCESS)
  {
    rc = usmDbServicePortIPV6AdminModeSet( temp_val );
  }
  return rc;
}

L7_RC_t
snmpAgentServicePortIpv6GatewaySet(L7_uint32 UnitIndex, L7_char8 *gateway_buff)
{
  L7_RC_t rc = L7_FAILURE;
  L7_in6_addr_t gateway;

  if(osapiInetPton(L7_AF_INET6, gateway_buff, (L7_uchar8 *)&gateway)==L7_SUCCESS)
  {
    rc = usmDbServicePortIPV6GatewaySet(&gateway);
  }
  return rc;
}

#ifdef I_agentServicePortIpv6AddressAutoConfig
L7_RC_t
snmpAgentServicePortIPv6AutoConfigGet(L7_int32 *val )
{
  L7_uint32 autoConfig;
  L7_RC_t rc = L7_FAILURE;

  rc = usmDbServPortIPv6AddrAutoConfigGet(&autoConfig);
  if (rc == L7_SUCCESS)
  {
    switch (autoConfig)
    {
      case  L7_DISABLE:
        *val = D_agentServicePortIpv6AddressAutoConfig_disable;
        break;

      case L7_ENABLE:
        *val = D_agentServicePortIpv6AddressAutoConfig_enable;
        break;

      default:
        rc = L7_FAILURE;
        break;
    }
  }
  return rc;
}

L7_RC_t
snmpAgentServicePortIPv6AutoConfigSet(L7_int32 val )
{
  L7_uint32 protocol, autoConfig, networkProtocol, temp_val;
  L7_RC_t rc = L7_SUCCESS;

  (void) usmDbAgentBasicConfigServPortIPv6ConfigProtocolGet(USMDB_UNIT_CURRENT,&protocol);
  (void) usmDbNetworkIPv6AddrAutoConfigGet(&autoConfig);
  (void) usmDbAgentBasicConfigNetworkIPv6ConfigProtocolGet(USMDB_UNIT_CURRENT,&networkProtocol);


  switch (val)
  {
    case D_agentServicePortIpv6AddressAutoConfig_disable:
      temp_val = L7_DISABLE;
      break;

    case D_agentServicePortIpv6AddressAutoConfig_enable:
      /* DHCP can only be set for either the service port or network port, not both */
       if ((autoConfig != L7_ENABLE) && (networkProtocol!= L7_SYSCONFIG_MODE_DHCP)
           && (protocol != L7_SYSCONFIG_MODE_DHCP))
       {
          temp_val = L7_ENABLE;
       }
       else
       {
         rc = L7_FAILURE;
       }
      break;
   default:
     rc = L7_FAILURE;
  }
  if (rc == L7_SUCCESS)
  {
     rc = usmDbServPortIPv6AddrAutoConfigSet(temp_val);
  }
  return rc;
}
#endif

#ifdef I_agentServicePortIpv6ConfigProtocol
L7_RC_t
snmpAgentServicePortIPv6ConfigProtocolGet(L7_int32 *val )
{
  L7_uint32 protocol;
  L7_RC_t rc = L7_FAILURE;
  rc = usmDbAgentBasicConfigServPortIPv6ConfigProtocolGet(USMDB_UNIT_CURRENT, &protocol);
  if (rc == L7_SUCCESS)
  {
    switch (protocol)
    {
      case L7_SYSCONFIG_MODE_NONE:
        *val = D_agentServicePortIpv6ConfigProtocol_none;
         break;
      case L7_SYSCONFIG_MODE_DHCP:
        *val = D_agentServicePortIpv6ConfigProtocol_dhcp;
        break;
      default:
        rc = L7_FAILURE;
        break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentServicePortIPv6ConfigProtocolSet(L7_int32 val )
{
  L7_uint32 protocol, servAutoConfig, autoConfig, temp_val;
  L7_RC_t rc = L7_SUCCESS;

  (void) usmDbAgentBasicConfigNetworkIPv6ConfigProtocolGet(USMDB_UNIT_CURRENT,&protocol);
  (void) usmDbServPortIPv6AddrAutoConfigGet(&servAutoConfig);
  (void) usmDbNetworkIPv6AddrAutoConfigGet(&autoConfig);

  switch (val)
  {
  case D_agentServicePortIpv6ConfigProtocol_none:
    temp_val = L7_SYSCONFIG_MODE_NONE;
    break;

  case D_agentServicePortIpv6ConfigProtocol_dhcp:
    /* DHCP can only be set for either the service port or network port, not both */
    if ((servAutoConfig != L7_ENABLE) && (autoConfig != L7_ENABLE) && (protocol != L7_SYSCONFIG_MODE_DHCP))
    {
      temp_val = L7_SYSCONFIG_MODE_DHCP;
    }
    else
    {
      rc = L7_FAILURE;
    }
    break;

  default:
    rc = L7_FAILURE;
  }
  if (rc == L7_SUCCESS)
  {
     rc = usmDbAgentBasicConfigServPortIPv6ProtocolSet(USMDB_UNIT_CURRENT, temp_val);
  }
  return rc;
}
#endif

L7_RC_t
snmpSortServicePortIpv6AddressIndices(L7_in6_prefix_t in[], L7_uint32 out[], L7_uint32 numAddrs)
{
  L7_uint32 i, j, temp;

  for(i = 0; i<numAddrs; i++)
    out[i]=i;
  for(i = 0; i<numAddrs; i++)
  {
    for(j=0; j<numAddrs-(i+1); j++)
    {
      if(memcmp(&in[out[j]], &in[out[j+1]], sizeof(L7_in6_addr_t)) > 0)
      {
        /*swap ths indexes*/
        temp = out[j+1];
        out[j+1] = out[j];
        out[j] = temp;
      }
    }
  }
  return L7_SUCCESS;
}

L7_RC_t
snmpAgentServicePortIpv6AddrEntryGet(L7_uint32 UnitIndex,L7_in6_addr_t prefix,L7_uint32 *prefixLen,L7_uint32 euiFlag)
{
  L7_in6_prefix_t ipv6Prefixes[L7_RTR6_MAX_INTF_ADDRS+1];
  L7_uint32 numAddr = L7_RTR6_MAX_INTF_ADDRS+1;
  L7_uint32 index[L7_RTR6_MAX_INTF_ADDRS+1];
  int i,val;
  L7_RC_t rc=L7_FAILURE;

  rc = usmDbServicePortIPV6AddrsGet( ipv6Prefixes, &numAddr);
  if(rc == L7_SUCCESS)
  {

    snmpSortServicePortIpv6AddressIndices( ipv6Prefixes, index, numAddr);
    for(i = 0; i<numAddr; i++)
    {
      val = memcmp(&prefix, &ipv6Prefixes[index[i]].in6Addr, sizeof(L7_in6_addr_t));
      if(val == 0)
      {
        *prefixLen = ipv6Prefixes[index[i]].in6PrefixLen;
        return L7_SUCCESS;
      }
    }
  }
  return L7_FAILURE;
}

L7_RC_t
snmpAgentServicePortIpv6AddrEntryNext(L7_uint32 UnitIndex,L7_in6_addr_t *prefix,L7_uint32 *prefixLen,L7_uint32 *euiFlag)
{
  L7_in6_prefix_t ipv6Prefixes[L7_RTR6_MAX_INTF_ADDRS+1];
  L7_uint32 numAddr = L7_RTR6_MAX_INTF_ADDRS+1;
  L7_uint32 index[L7_RTR6_MAX_INTF_ADDRS+1];
  int i,val;
   int j;
  L7_char8 str_addr[64];


  if(usmDbServicePortIPV6AddrsGet( ipv6Prefixes, &numAddr) == L7_SUCCESS)
  {
   for(j=0;j<numAddr;j++)
   {
    memset(str_addr,0x00,sizeof(str_addr));
     osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)prefix,str_addr,IPV6_DISP_ADDR_LEN);
   }

    if(numAddr == 0)
      return L7_FAILURE;

    snmpSortServicePortIpv6AddressIndices( ipv6Prefixes, index, numAddr);
    for(i = 0; i<numAddr; i++)
    {
      val = memcmp(prefix, &ipv6Prefixes[index[i]].in6Addr, sizeof(L7_in6_addr_t));
      if(val < 0)
      {
        memcpy(prefix, &ipv6Prefixes[index[i]].in6Addr, sizeof(L7_in6_addr_t));
        *prefixLen = ipv6Prefixes[index[i]].in6PrefixLen;
        return L7_SUCCESS;
      }
    }
  }
  return L7_FAILURE;
}


/** table set **/
L7_RC_t
snmpAgentServicePortIpv6AddrPrefixSet(L7_uint32 UnitIndex, L7_in6_addr_t ip6Addr,L7_int32 pLen, L7_int32 euiFlag)
{
  L7_RC_t rc;
  L7_uint32 temp_val=0;
  switch(euiFlag)
  {
    case D_agentServicePortIpv6AddrEuiFlag_enabled:
      temp_val |= L7_RTR6_ADDR_EUI64;;
      break;

    case D_agentServicePortIpv6AddrEuiFlag_disabled:
      temp_val = 0;
      break;

    default:
      temp_val = 0;
      break;
  }
  rc = usmDbServicePortIPV6PrefixAdd(&ip6Addr, pLen,temp_val);
  return rc;
}

L7_RC_t
snmpAgentNetworkIpv6AddrEntryGet(L7_uint32 UnitIndex,L7_in6_addr_t prefix,L7_uint32 *prefixLen,L7_uint32 euiFlag)
{
  L7_in6_prefix_t ipv6Prefixes[L7_RTR6_MAX_INTF_ADDRS+1];
  L7_uint32 numAddr = L7_RTR6_MAX_INTF_ADDRS+1;
  L7_uint32 index[L7_RTR6_MAX_INTF_ADDRS+1];
  int i,val;
  L7_RC_t rc=L7_FAILURE;

  rc = usmDbAgentIpIfIPV6AddrsGet( ipv6Prefixes, &numAddr);
  if(rc == L7_SUCCESS)
  {
    snmpSortServicePortIpv6AddressIndices( ipv6Prefixes, index, numAddr);
    for(i = 0; i<numAddr; i++)
    {

      val = memcmp(&prefix, &ipv6Prefixes[index[i]].in6Addr, sizeof(L7_in6_addr_t));
      if(val == 0)
      {
        *prefixLen = ipv6Prefixes[index[i]].in6PrefixLen;
        return L7_SUCCESS;
      }
    }
  }
  return L7_FAILURE;
}

L7_RC_t
snmpAgentNetworkIpv6AddrEntryNext(L7_uint32 UnitIndex,L7_in6_addr_t *prefix,L7_uint32 *prefixLen,L7_uint32 *euiFlag)
{
  L7_in6_prefix_t ipv6Prefixes[L7_RTR6_MAX_INTF_ADDRS+1];
  L7_uint32 numAddr = L7_RTR6_MAX_INTF_ADDRS+1;
  L7_uint32 index[L7_RTR6_MAX_INTF_ADDRS+1];
  int i,val;
   int j;
  L7_char8 str_addr[64];


  if(usmDbAgentIpIfIPV6AddrsGet( ipv6Prefixes, &numAddr) == L7_SUCCESS)
  {
   for(j=0;j<numAddr;j++)
   {
    memset(str_addr,0x00,sizeof(str_addr));
     osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)prefix,str_addr,IPV6_DISP_ADDR_LEN);
   }

    if(numAddr == 0)
      return L7_FAILURE;

    snmpSortServicePortIpv6AddressIndices( ipv6Prefixes, index, numAddr);
    for(i = 0; i<numAddr; i++)
    {
      val = memcmp(prefix, &ipv6Prefixes[index[i]].in6Addr, sizeof(L7_in6_addr_t));
      if(val < 0)
      {
        memcpy(prefix, &ipv6Prefixes[index[i]].in6Addr, sizeof(L7_in6_addr_t));
        *prefixLen = ipv6Prefixes[index[i]].in6PrefixLen;
        return L7_SUCCESS;
      }
    }
  }
  return L7_FAILURE;
}


L7_RC_t
snmpAgentNetworkIpv6AddrPrefixSet(L7_uint32 UnitIndex, L7_in6_addr_t ip6Addr,L7_int32 pLen, L7_int32 euiFlag)
{
  L7_RC_t rc;
  L7_uint32 temp_val=0;

  switch(euiFlag)
  {
    case D_agentNetworkIpv6AddrEuiFlag_enabled:
      temp_val |= L7_RTR6_ADDR_EUI64;;
      break;

    case D_agentNetworkIpv6AddrEuiFlag_disabled:
      temp_val = 0;
      break;

    default:
      temp_val = 0;
      return L7_FAILURE;
  }
  rc = usmDbAgentIpIfIPV6PrefixAdd(&ip6Addr, pLen,temp_val);
  return rc;
}
#endif


#ifdef I_agentDhcpClientVendorClassIdMode
L7_RC_t snmpDhcpVendorClassIdModeGet(L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val;
  rc  = usmdbDhcpVendorClassOptionAdminModeGet(&temp_val);
  if (L7_SUCCESS == rc)
  {
    switch (temp_val)
    {
      case L7_ENABLE:
        *val = D_agentDhcpClientVendorClassIdMode_true;
        rc = L7_SUCCESS;
        break;
      case L7_DISABLE:
        *val = D_agentDhcpClientVendorClassIdMode_false;
        rc = L7_SUCCESS;
        break;
      default:
        rc = L7_FAILURE;
        break;
    }
  }

  return rc;
}

L7_RC_t snmpDhcpVendorClassIdModeSet(L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val = L7_FALSE;
  switch(val)
  {
    case D_agentDhcpClientVendorClassIdMode_true:
      temp_val = L7_ENABLE;
      rc = L7_SUCCESS;
      break;
    case D_agentDhcpClientVendorClassIdMode_false:
      temp_val = L7_DISABLE;
      rc = L7_SUCCESS;
      break;
    default:
      rc = L7_FAILURE;
      return rc;
      break;
  }

  rc = usmdbDhcpVendorClassOptionAdminModeSet(temp_val, L7_TRUE);
  return rc;
}
#endif


#ifdef I_agentDhcpClientVendorClassIdString
L7_RC_t snmpDhcpVendorClassIdStringGet(OctetString **os_ptr)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8  snmpBuff[SNMP_BUFFER_LEN];

  bzero(snmpBuff, SNMP_BUFFER_LEN);
  rc  = usmdbDhcpVendorClassOptionStringGet(snmpBuff);
  if (L7_SUCCESS == rc)
  {
    if (SafeMakeOctetStringFromTextExact(os_ptr, snmpBuff) == L7_TRUE)
    {
      return L7_SUCCESS;
    }
  }
  return rc;
}

L7_RC_t snmpDhcpVendorClassIdStringSet(OctetString *os_ptr)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8  snmpBuff[SNMP_BUFFER_LEN]; 

  bzero(snmpBuff, SNMP_BUFFER_LEN);

  memcpy(snmpBuff, os_ptr->octet_ptr, os_ptr->length);

  rc  = usmdbDhcpVendorClassOptionStringSet(snmpBuff, L7_TRUE);
  if (L7_SUCCESS == rc)
  {
    return L7_SUCCESS;
  }
  return rc;
}
#endif

/**************************************************************************************************************/

#ifdef I_agentSnmpCommunityAccessMode
L7_RC_t
snmpAgentSnmpCommunityAccessModeGet ( L7_uint32 UnitIndex, L7_int32 commIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbSnmpCommunityAccessLevelGet ( UnitIndex, commIndex, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_SNMP_COMMUNITY_ACCESS_LEVEL_READ_ONLY:
      *val = D_agentSnmpCommunityAccessMode_read_only;
      break;

    case L7_SNMP_COMMUNITY_ACCESS_LEVEL_READ_WRITE:
      *val = D_agentSnmpCommunityAccessMode_read_write;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentSnmpCommunityAccessModeSet  ( L7_uint32 UnitIndex, L7_int32 commIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSnmpCommunityAccessMode_read_only:
    temp_val = L7_SNMP_COMMUNITY_ACCESS_LEVEL_READ_ONLY;
    break;

  case D_agentSnmpCommunityAccessMode_read_write:
    temp_val = L7_SNMP_COMMUNITY_ACCESS_LEVEL_READ_WRITE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbSnmpCommunityAccessLevelSet ( UnitIndex, commIndex, temp_val );
  }

  return rc;
}
#endif /* I_agentSnmpCommunityAccessMode */

#ifdef I_agentSnmpCommunityStatus
L7_RC_t
snmpAgentSnmpCommunityStatusGet ( L7_uint32 UnitIndex, L7_int32 commIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbSnmpCommunityStatusGet ( UnitIndex, commIndex, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_SNMP_COMMUNITY_STATUS_VALID:
      *val = D_agentSnmpCommunityStatus_active;
      break;

    case L7_SNMP_COMMUNITY_STATUS_INVALID:
      *val = D_agentSnmpCommunityStatus_notInService;
      break;

    case L7_SNMP_COMMUNITY_STATUS_DELETE:
      *val = D_agentSnmpCommunityStatus_destroy;
      break;

    case L7_SNMP_COMMUNITY_STATUS_CONFIG:
      *val = D_agentSnmpCommunityStatus_config;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentSnmpCommunityStatusSet ( L7_uint32 UnitIndex, L7_int32 commIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSnmpCommunityStatus_active:
    temp_val = L7_SNMP_COMMUNITY_STATUS_VALID;
    break;

  case D_agentSnmpCommunityStatus_notInService:
    temp_val = L7_SNMP_COMMUNITY_STATUS_INVALID;
    break;

  case D_agentSnmpCommunityStatus_destroy:
    temp_val = L7_SNMP_COMMUNITY_STATUS_DELETE;
    break;

  case D_agentSnmpCommunityStatus_config:
    temp_val = L7_SNMP_COMMUNITY_STATUS_CONFIG;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbSnmpCommunityStatusSet ( UnitIndex, commIndex, temp_val );
  }

  return rc;
}
#endif /* I_agentSnmpCommunityStatus */

/**************************************************************************************************************/

#ifdef I_agentSnmpTrapReceiverStatus
L7_RC_t
snmpAgentSnmpTrapReceiverStatusGet ( L7_uint32 UnitIndex, L7_int32 trapIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbTrapManagerStatusIpGet ( UnitIndex, trapIndex, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_SNMP_TRAP_MGR_STATUS_VALID:
      *val = D_agentSnmpTrapReceiverStatus_active;
      break;

    case L7_SNMP_TRAP_MGR_STATUS_INVALID:
      *val = D_agentSnmpTrapReceiverStatus_notInService;
      break;

    case L7_SNMP_TRAP_MGR_STATUS_DELETE:
      *val = D_agentSnmpTrapReceiverStatus_destroy;
      break;

    case L7_SNMP_TRAP_MGR_STATUS_CONFIG:
      *val = D_agentSnmpTrapReceiverStatus_config;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentSnmpTrapReceiverStatusSet  ( L7_uint32 UnitIndex, L7_int32 trapIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_uint32 ipval;
  L7_char8 ip[16];
  L7_RC_t rc = L7_SUCCESS;
  #if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
  L7_in6_addr_t ipv6_address;
  L7_char8 buf1[L7_CLI_MAX_STRING_LENGTH ];
  /*L7_BOOL ipv6AddrGreater = L7_FALSE;*/
  memset(&ipv6_address, 0, sizeof(L7_in6_addr_t));
  #endif

  memset (ip, 0,sizeof(ip));
  switch (val)
  {
  case D_agentSnmpTrapReceiverStatus_active:
    temp_val = L7_SNMP_TRAP_MGR_STATUS_VALID;
    break;

  case D_agentSnmpTrapReceiverStatus_notInService:
    temp_val = L7_SNMP_TRAP_MGR_STATUS_INVALID;
    break;

  case D_agentSnmpTrapReceiverStatus_destroy:
    temp_val = L7_SNMP_TRAP_MGR_STATUS_DELETE;
    break;

  case D_agentSnmpTrapReceiverStatus_config:
    temp_val = L7_SNMP_TRAP_MGR_STATUS_CONFIG;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    if(temp_val == L7_SNMP_TRAP_MGR_STATUS_DELETE)
    {
      if(usmDbTrapManagerIpAddrGet(UnitIndex, trapIndex, &ipval) == L7_SUCCESS) 
      {
        if(usmDbInetNtoa(ipval, ip) == L7_SUCCESS)
        {
           if((strcmp(ip, "0.0.0.0") != 0) )
           {
             rc = usmDbTrapManagerStatusIpSet ( UnitIndex, trapIndex, L7_SNMP_TRAP_MGR_STATUS_INVALID);
           }
        }
        else
        {
          rc = L7_FAILURE;
        }
      }
      #if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
      else if (usmDbTrapManagerIPv6AddrGet(UnitIndex, trapIndex, &ipv6_address) == L7_SUCCESS)
      {
         if(osapiInetNtop(L7_AF_INET6, (char *)&ipv6_address, buf1, sizeof(buf1)) == L7_SUCCESS)
         {
           if(strcmp(buf1, "::") !=0)
           {
              rc = usmDbTrapManagerStatusIpSet ( UnitIndex, trapIndex, L7_SNMP_TRAP_MGR_STATUS_INVALID);
           }
         }
         else
         {
           rc = L7_FAILURE;
         }
      }
      #endif
      else
      {
         rc = L7_FAILURE;      
      }
    }
    if (rc == L7_SUCCESS)
    {
    rc = usmDbTrapManagerStatusIpSet ( UnitIndex, trapIndex, temp_val );
  }
  }

  return rc;
}
#endif /* I_agentSnmpTrapReceiverStatus */

/**************************************************************************************************************/

#ifdef I_agentSnmpTrapReceiverVersion
L7_RC_t
snmpAgentSnmpTrapReceiverVersionGet ( L7_uint32 UnitIndex, L7_int32 trapIndex, L7_int32 *val )
{
  snmpTrapVer_t temp_val;
  L7_RC_t rc;

  rc = usmDbTrapManagerVersionGet ( UnitIndex, trapIndex, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_SNMP_TRAP_VER_SNMPV1:
      *val = D_agentSnmpTrapReceiverVersion_snmpv1;
      break;

    case L7_SNMP_TRAP_VER_SNMPV2C:
      *val = D_agentSnmpTrapReceiverVersion_snmpv2c;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentSnmpTrapReceiverVersionSet  ( L7_uint32 UnitIndex, L7_int32 trapIndex, L7_int32 val )
{
  snmpTrapVer_t temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSnmpTrapReceiverVersion_snmpv1:
    temp_val = L7_SNMP_TRAP_VER_SNMPV1;
    break;

  case D_agentSnmpTrapReceiverVersion_snmpv2c:
    temp_val = L7_SNMP_TRAP_VER_SNMPV2C;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbTrapManagerVersionSet ( UnitIndex, trapIndex, temp_val );
  }

  return rc;
}
#endif /* I_agentSnmpTrapReceiverVersion */

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
L7_RC_t
snmpAgentSnmpTrapReceiverIpv6AddressGet(L7_uint32 UnitIndex, L7_int32 trapIndex, L7_char8 *snmp_buffer)
{
  L7_in6_addr_t gateway;
  L7_uchar8     str_addr[64];

  memset(str_addr, 0x00, sizeof(str_addr));
  memset(&gateway , 0, sizeof(L7_in6_addr_t));

  if(usmDbTrapManagerIPv6AddrGet(UnitIndex, trapIndex, &gateway)==L7_SUCCESS)
  {
    if(osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)&gateway, str_addr,sizeof(str_addr)) != L7_NULL)
    {
      if(strcmp(str_addr, "::") !=0)
        sprintf(snmp_buffer,"%s",str_addr);
      return L7_SUCCESS;
    }
    else
      return L7_FAILURE;
  }
  else
    return L7_FAILURE;
}

L7_RC_t
snmpAgentSnmpTrapReceiverIpv6AddressSet(L7_uint32 UnitIndex, L7_int32 trapIndex, L7_char8 *snmp_buffer )
{
  L7_RC_t rc = L7_SUCCESS;
  L7_in6_addr_t ipv6addr;
  if(osapiInetPton(L7_AF_INET6, snmp_buffer, (L7_uchar8 *)&ipv6addr)==L7_SUCCESS)
    rc=usmDbTrapManagerIPv6AddrSet(UnitIndex, trapIndex, &ipv6addr);
  else
    rc = L7_FAILURE;
  return rc;
}

#endif

#ifdef I_agentSnmpTrapReceiverAddressType
L7_RC_t
snmpAgentSnmpTrapReceiverAddressTypeGet ( L7_uint32 UnitIndex, L7_uint32 trapIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbTrapManagerIpAddrTypeGet ( UnitIndex, trapIndex, &temp_val );
  
  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_SNMP_TRAP_MGR_ADDR_TYPE_IPv4:
      *val = D_agentSnmpTrapReceiverAddressType_ipv4;
      break;

    case L7_SNMP_TRAP_MGR_ADDR_TYPE_IPv6:
      *val = D_agentSnmpTrapReceiverAddressType_ipv6;
      break;

    case L7_SNMP_TRAP_MGR_ADDR_TYPE_DNSv4:
      *val = D_agentSnmpTrapReceiverAddressType_dns_v4;
      break;

    case L7_SNMP_TRAP_MGR_ADDR_TYPE_DNSv6:
      *val = D_agentSnmpTrapReceiverAddressType_dns_v6;
      break;

    default:
      *val = 0;
      rc = L7_SUCCESS;
    } 
  }
  
  return rc;
}

L7_RC_t
snmpAgentSnmpTrapReceiverAddressTypeSet  ( L7_uint32 UnitIndex, L7_int32 trapIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSnmpTrapReceiverAddressType_ipv4:
    temp_val = L7_SNMP_TRAP_MGR_ADDR_TYPE_IPv4;
    break;

  case D_agentSnmpTrapReceiverAddressType_ipv6:
    temp_val = L7_SNMP_TRAP_MGR_ADDR_TYPE_IPv6;
    break;

  case D_agentSnmpTrapReceiverAddressType_dns_v4:
    temp_val = L7_SNMP_TRAP_MGR_ADDR_TYPE_DNSv4;
    break;

  case D_agentSnmpTrapReceiverAddressType_dns_v6:
    temp_val = L7_SNMP_TRAP_MGR_ADDR_TYPE_DNSv6;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbTrapManagerIpAddrTypeSet(UnitIndex, trapIndex, temp_val);
  }

  return rc;
}
#endif /* I_agentSnmpTrapReceiverAddressType */

#ifdef I_agentSnmpTrapReceiverAddress
L7_RC_t
snmpAgentSnmpTrapReceiverAddressGet(L7_uint32 UnitIndex, L7_int32 trapIndex, L7_char8 *snmp_buffer)
{
  L7_uint32 ipaddrtype, ipaddr;
  L7_RC_t rc = L7_SUCCESS;
  
  rc = snmpAgentSnmpTrapReceiverAddressTypeGet(UnitIndex, trapIndex, &ipaddrtype);
  
  if(rc == L7_SUCCESS)
  {
    if((ipaddrtype == D_agentSnmpTrapReceiverAddressType_dns_v4) || (ipaddrtype == D_agentSnmpTrapReceiverAddressType_ipv4))
    {
      rc = usmDbTrapManagerIpAddrGet(UnitIndex, trapIndex, &ipaddr);
      osapiInetNtoa(ipaddr, snmp_buffer);
    }
    #if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
    else if((ipaddrtype == D_agentSnmpTrapReceiverAddressType_dns_v6) || (ipaddrtype == D_agentSnmpTrapReceiverAddressType_ipv6))
    {
      rc = snmpAgentSnmpTrapReceiverIpv6AddressGet(UnitIndex, trapIndex, snmp_buffer);
    }
    #endif
  }
  return rc;
}

L7_RC_t
snmpAgentSnmpTrapReceiverAddressSet(L7_uint32 UnitIndex, L7_int32 trapIndex, L7_char8 *snmp_buffer )
{
  L7_RC_t rc = L7_SUCCESS;
  L7_in6_addr_t ipv6addr;  
  L7_inet_addr_t destIp;
  L7_char8 domainName[L7_DNS_DOMAIN_NAME_SIZE_MAX];  
  dnsClientLookupStatus_t status =  DNS_LOOKUP_STATUS_FAILURE;
  L7_uint32 ipaddrtype, ipv4addr;
  L7_inet_addr_t inetAddr;

  rc = snmpAgentSnmpTrapReceiverAddressTypeGet(UnitIndex, trapIndex, &ipaddrtype);
  
  if(rc == L7_SUCCESS)
  {
    if(ipaddrtype == D_agentSnmpTrapReceiverAddressType_dns_v4)
    {
      inetAddressReset(&inetAddr);
        rc = usmDbDNSClientInetNameLookup(L7_AF_INET, snmp_buffer, &status, domainName, &inetAddr);
        /*  If the DNS lookup fails */
        if (rc == L7_SUCCESS)
        {
          inetAddrHtop(&inetAddr, snmp_buffer);
          if (usmDbInetAton(snmp_buffer, &ipv4addr) == L7_SUCCESS || (ipv4addr != 0))
          {
            rc = usmDbTrapManagerIpAddrSet(UnitIndex, trapIndex, snmp_buffer);
          }
        }
    }
    else if(ipaddrtype == D_agentSnmpTrapReceiverAddressType_dns_v6)
    {
      inetAddressZeroSet(L7_AF_INET6, &destIp);
      if(usmDbDNSClientInetNameLookup(L7_AF_INET6, snmp_buffer, &status,
          domainName, &destIp) != L7_SUCCESS)
      {
        rc = L7_FAILURE;
      }
      if (inet6AddressGet(&destIp, &ipv6addr) != L7_SUCCESS)
      {
        rc = L7_FAILURE;
      }
      rc=usmDbTrapManagerIPv6AddrSet(UnitIndex, trapIndex, &ipv6addr);
    }
    else if(ipaddrtype == D_agentSnmpTrapReceiverAddressType_ipv4)
    {
      if (usmDbInetAton(snmp_buffer, &ipv4addr) == L7_SUCCESS || (ipv4addr != 0))
      {
        rc = usmDbTrapManagerIpAddrSet(UnitIndex, trapIndex, snmp_buffer);
      }
    }
    #if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
    else if(ipaddrtype == D_agentSnmpTrapReceiverAddressType_ipv6)
    {
      rc = snmpAgentSnmpTrapReceiverIpv6AddressSet(UnitIndex, trapIndex, snmp_buffer);
    }
    #endif
  }
  return rc;
}

#endif /* I_agentSnmpTrapReceiverAddress */

/**************************************************************************************************************/


L7_RC_t
snmpAgentSnmpAuthenticationTrapFlagGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbTrapAuthSwGet ( UnitIndex, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSnmpAuthenticationTrapFlag_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSnmpAuthenticationTrapFlag_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}


L7_RC_t
snmpAgentSnmpLinkUpDownTrapFlagGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbTrapLinkStatusSwGet ( UnitIndex, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSnmpLinkUpDownTrapFlag_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSnmpLinkUpDownTrapFlag_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}


L7_RC_t
snmpAgentSnmpMultipleUsersTrapFlagGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbTrapMultiUsersSwGet ( UnitIndex, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSnmpMultipleUsersTrapFlag_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSnmpMultipleUsersTrapFlag_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}


L7_RC_t
snmpAgentSnmpSpanningTreeTrapFlagGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbTrapSpanningTreeSwGet ( UnitIndex, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSnmpSpanningTreeTrapFlag_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSnmpSpanningTreeTrapFlag_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}


L7_RC_t
snmpAgentSnmpAuthenticationTrapFlagSet  ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSnmpAuthenticationTrapFlag_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSnmpAuthenticationTrapFlag_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbTrapAuthSwSet ( UnitIndex, temp_val );
  }

  return rc;
}


L7_RC_t
snmpAgentSnmpLinkUpDownTrapFlagSet  ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSnmpLinkUpDownTrapFlag_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSnmpLinkUpDownTrapFlag_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbTrapLinkStatusSwSet ( UnitIndex, temp_val );
  }

  return rc;
}


L7_RC_t
snmpAgentSnmpMultipleUsersTrapFlagSet  ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSnmpMultipleUsersTrapFlag_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSnmpMultipleUsersTrapFlag_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbTrapMultiUsersSwSet ( UnitIndex, temp_val );
  }

  return rc;
}


L7_RC_t
snmpAgentSnmpSpanningTreeTrapFlagSet  ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSnmpSpanningTreeTrapFlag_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSnmpSpanningTreeTrapFlag_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbTrapSpanningTreeSwSet ( UnitIndex, temp_val );
  }

  return rc;
}

L7_RC_t
snmpAgentSpanningTreeModeGet ( L7_uint32 unitIndex, L7_int32 *val )
{
  L7_BOOL dot1asMode = L7_FALSE;
  L7_RC_t rc = L7_FAILURE;

  rc = usmDbDot1sModeGet(unitIndex, &dot1asMode);

  if (rc == L7_SUCCESS)
  {
    switch (dot1asMode)
    {
      case L7_TRUE:
        *val = D_agentSpanningTreeMode_enable;
        break;

      case L7_FALSE:
        *val = D_agentSpanningTreeMode_disable;
        break;

      default:
        *val = 0;
        rc = L7_FAILURE;
    }
  }
  return rc;
}

L7_RC_t
snmpAgentSpanningTreeModeSet ( L7_uint32 unitIndex, L7_int32 val )
{
  L7_BOOL dot1asMode = L7_FALSE;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
    case D_agentSpanningTreeMode_enable:
      dot1asMode = L7_TRUE;
      break;

    case D_agentSpanningTreeMode_disable:
      dot1asMode = L7_FALSE;
      break;

    default:
      rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
      rc = usmDbDot1sModeSet(unitIndex, dot1asMode);
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchBroadcastControlModeGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbSwDevCtrlBcastStormModeGet ( UnitIndex, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSwitchBroadcastControlMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSwitchBroadcastControlMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}


L7_RC_t
snmpAgentSwitchMulticastControlModeGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbSwDevCtrlMcastStormModeGet ( UnitIndex, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSwitchMulticastControlMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSwitchMulticastControlMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchUnicastControlModeGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbSwDevCtrlUcastStormModeGet ( UnitIndex, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSwitchUnicastControlMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSwitchUnicastControlMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchBroadcastControlThresholdGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_RC_t rc;
  L7_RATE_UNIT_t temp_val;

  rc = usmDbSwDevCtrlBcastStormThresholdGet ( UnitIndex, val, &temp_val );

  return rc;
}


L7_RC_t
snmpAgentSwitchMulticastControlThresholdGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_RC_t rc;
  L7_RATE_UNIT_t temp_val;

  rc = usmDbSwDevCtrlMcastStormThresholdGet ( UnitIndex, val, &temp_val );

  return rc;
}


L7_RC_t
snmpAgentSwitchUnicastControlThresholdGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_RC_t rc;
  L7_RATE_UNIT_t temp_val;

  rc = usmDbSwDevCtrlUcastStormThresholdGet ( UnitIndex, val, &temp_val );

  return rc;
}

L7_RC_t
snmpAgentSwitchBroadcastControlThresholdUnitGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_RC_t rc;
  L7_uint32 temp_val;
  L7_RATE_UNIT_t rate_unit;

  rc = usmDbSwDevCtrlBcastStormThresholdGet ( UnitIndex, &temp_val, &rate_unit );

  if (rc == L7_SUCCESS)
  {
     switch (rate_unit)
     {
     case L7_RATE_UNIT_PERCENT:
         *val = D_agentSwitchBroadcastControlThresholdUnit_percent;
         break;

     case L7_RATE_UNIT_PPS:
         *val = D_agentSwitchBroadcastControlThresholdUnit_pps;
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
snmpAgentSwitchMulticastControlThresholdUnitGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_RC_t rc;
  L7_uint32 temp_val;
  L7_RATE_UNIT_t rate_unit;

  rc = usmDbSwDevCtrlMcastStormThresholdGet ( UnitIndex, &temp_val, &rate_unit );

  if (rc == L7_SUCCESS)
  {
     switch (rate_unit)
     {
     case L7_RATE_UNIT_PERCENT:
         *val = D_agentSwitchMulticastControlThresholdUnit_percent;
         break;

     case L7_RATE_UNIT_PPS:
         *val = D_agentSwitchMulticastControlThresholdUnit_pps;
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
snmpAgentSwitchUnicastControlThresholdUnitGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_RC_t rc;
  L7_uint32 temp_val;
  L7_RATE_UNIT_t rate_unit;

  rc = usmDbSwDevCtrlUcastStormThresholdGet ( UnitIndex, &temp_val, &rate_unit );

  if (rc == L7_SUCCESS)
  {
     switch (rate_unit)
     {
     case L7_RATE_UNIT_PERCENT:
         *val = D_agentSwitchUnicastControlThresholdUnit_percent;
         break;

     case L7_RATE_UNIT_PPS:
         *val = D_agentSwitchUnicastControlThresholdUnit_pps;
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
snmpAgentSwitchDot3FlowControlModeGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbSwDevCtrlFlowControlModeGet ( UnitIndex, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSwitchDot3FlowControlMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSwitchDot3FlowControlMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}


L7_RC_t
snmpAgentSwitchBroadcastControlModeSet  ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSwitchBroadcastControlMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSwitchBroadcastControlMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbSwDevCtrlBcastStormModeSet ( UnitIndex, temp_val);
  }

  return rc;
}


L7_RC_t
snmpAgentSwitchMulticastControlModeSet  ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSwitchMulticastControlMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSwitchMulticastControlMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbSwDevCtrlMcastStormModeSet ( UnitIndex, temp_val);
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchUnicastControlModeSet  ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSwitchUnicastControlMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSwitchUnicastControlMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbSwDevCtrlUcastStormModeSet ( UnitIndex, temp_val);
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchBroadcastControlThresholdSet  ( L7_uint32 UnitIndex, L7_uint32 val )
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val, rate_min, rate_max;
  L7_RATE_UNIT_t rate_unit;

  rc = usmDbSwDevCtrlBcastStormThresholdGet (UnitIndex, &temp_val, &rate_unit);

  if (rc == L7_SUCCESS)
  {
      if (rate_unit == L7_RATE_UNIT_PERCENT)
      {
         rate_min = L7_STORMCONTROL_LEVEL_MIN;
         rate_max = L7_STORMCONTROL_LEVEL_MAX;
      }
      else if (rate_unit == L7_RATE_UNIT_PPS)
      {
         rate_min = L7_STORMCONTROL_RATE_MIN;
         rate_max = L7_STORMCONTROL_RATE_MAX;
      }
      else
      {
         return L7_FAILURE;
      }
      if ((val < rate_min) || (val > rate_max))
      {
         return L7_FAILURE;
      }
  }

  rc = usmDbSwDevCtrlBcastStormThresholdSet ( UnitIndex, val, rate_unit);

  return rc;
}

L7_RC_t
snmpAgentSwitchMulticastControlThresholdSet  ( L7_uint32 UnitIndex, L7_uint32 val )
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val, rate_min, rate_max;
  L7_RATE_UNIT_t rate_unit;

  rc = usmDbSwDevCtrlMcastStormThresholdGet (UnitIndex, &temp_val, &rate_unit);

  if (rc == L7_SUCCESS)
  {
      if (rate_unit == L7_RATE_UNIT_PERCENT)
      {
         rate_min = L7_STORMCONTROL_LEVEL_MIN;
         rate_max = L7_STORMCONTROL_LEVEL_MAX;
      }
      else if (rate_unit == L7_RATE_UNIT_PPS)
      {
         rate_min = L7_STORMCONTROL_RATE_MIN;
         rate_max = L7_STORMCONTROL_RATE_MAX;
      }
      else
      {
         return L7_FAILURE;
      }
      if ((val < rate_min) || (val > rate_max))
      {
         return L7_FAILURE;
      }
  }

  rc = usmDbSwDevCtrlMcastStormThresholdSet ( UnitIndex, val, rate_unit);

  return rc;
}

L7_RC_t
snmpAgentSwitchUnicastControlThresholdSet  ( L7_uint32 UnitIndex, L7_uint32 val )
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val, rate_min, rate_max;
  L7_RATE_UNIT_t rate_unit;

  rc = usmDbSwDevCtrlUcastStormThresholdGet (UnitIndex, &temp_val, &rate_unit);

  if (rc == L7_SUCCESS)
  {
      if (rate_unit == L7_RATE_UNIT_PERCENT)
      {
         rate_min = L7_STORMCONTROL_LEVEL_MIN;
         rate_max = L7_STORMCONTROL_LEVEL_MAX;
      }
      else if (rate_unit == L7_RATE_UNIT_PPS)
      {
         rate_min = L7_STORMCONTROL_RATE_MIN;
         rate_max = L7_STORMCONTROL_RATE_MAX;
      }
      else
      {
         return L7_FAILURE;
      }
      if ((val < rate_min) || (val > rate_max))
      {
         return L7_FAILURE;
      }
  }

  rc = usmDbSwDevCtrlUcastStormThresholdSet ( UnitIndex, val, rate_unit);

  return rc;
}


L7_RC_t
snmpAgentSwitchBroadcastControlThresholdUnitSet  ( L7_uint32 UnitIndex, L7_uint32 val )
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 rate=0;
  L7_uint32 temp_val;
  L7_RATE_UNIT_t rate_unit=0;
  L7_RATE_UNIT_t unit_current;

  rc = usmDbSwDevCtrlBcastStormThresholdGet (UnitIndex, &temp_val, &unit_current);

  switch (val)
  {
  case D_agentSwitchBroadcastControlThresholdUnit_percent:
    rate_unit = L7_RATE_UNIT_PERCENT;
    break;

  case D_agentSwitchBroadcastControlThresholdUnit_pps:
    rate_unit = L7_RATE_UNIT_PPS;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS && rate_unit != unit_current)
  {
      rc = usmDbSwDevCtrlBcastStormThresholdSet ( UnitIndex, rate, rate_unit);
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchMulticastControlThresholdUnitSet  ( L7_uint32 UnitIndex, L7_uint32 val )
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 rate=0;
  L7_uint32 temp_val;
  L7_RATE_UNIT_t rate_unit=0;
  L7_RATE_UNIT_t unit_current;

  rc = usmDbSwDevCtrlMcastStormThresholdGet (UnitIndex, &temp_val, &unit_current);

  switch (val)
  {
  case D_agentSwitchMulticastControlThresholdUnit_percent:
    rate_unit = L7_RATE_UNIT_PERCENT;
    break;

  case D_agentSwitchMulticastControlThresholdUnit_pps:
    rate_unit = L7_RATE_UNIT_PPS;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS && rate_unit != unit_current)
  {
      rc = usmDbSwDevCtrlMcastStormThresholdSet ( UnitIndex, rate, rate_unit);
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchUnicastControlThresholdUnitSet  ( L7_uint32 UnitIndex, L7_uint32 val )
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 rate=0;
  L7_uint32 temp_val;
  L7_RATE_UNIT_t rate_unit=0;
  L7_RATE_UNIT_t unit_current;

  rc = usmDbSwDevCtrlUcastStormThresholdGet (UnitIndex, &temp_val, &unit_current);

  switch (val)
  {
  case D_agentSwitchUnicastControlThresholdUnit_percent:
    rate_unit = L7_RATE_UNIT_PERCENT;
    break;

  case D_agentSwitchUnicastControlThresholdUnit_pps:
    rate_unit = L7_RATE_UNIT_PPS;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS && rate_unit != unit_current)
  {
      rc = usmDbSwDevCtrlUcastStormThresholdSet ( UnitIndex, rate, rate_unit);
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchDot3FlowControlModeSet  ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSwitchDot3FlowControlMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSwitchDot3FlowControlMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbSwDevCtrlFlowControlModeSet ( UnitIndex, temp_val );
  }

  return rc;
}


/**************************************************************************************************************/
L7_RC_t
snmpAgentSwitchSnoopingAdminModeGet ( L7_uint32 UnitIndex, L7_uint32 *val, L7_uchar8 family)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbSnoopAdminModeGet(UnitIndex, &temp_val, family);

  if (rc == L7_SUCCESS)
  {
    switch(temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSwitchSnoopingAdminMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSwitchSnoopingAdminMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }
  return rc;
}

L7_RC_t
snmpAgentSwitchSnoopingAdminModeSet ( L7_uint32 UnitIndex, L7_uint32 val, L7_uchar8 family)
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch(val)
  {
  case D_agentSwitchSnoopingAdminMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSwitchSnoopingAdminMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbSnoopAdminModeSet(UnitIndex, temp_val, family);
  }
  return rc;
}
/**************************************************************************************************************/
L7_RC_t
snmpAgentSwitchSnoopingQuerierAdminModeGet (L7_uint32 *val, L7_uchar8 family)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbSnoopQuerierAdminModeGet(&temp_val, family);

  if (rc == L7_SUCCESS)
  {
    switch(temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSwitchSnoopingQuerierAdminMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSwitchSnoopingQuerierAdminMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }
  return rc;
}
L7_RC_t
snmpAgentSwitchSnoopingQuerierAdminModeSet (L7_uint32 val, L7_uchar8 family)
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch(val)
  {
  case D_agentSwitchSnoopingQuerierAdminMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSwitchSnoopingQuerierAdminMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbSnoopQuerierAdminModeSet(temp_val, family);
  }
  return rc;
}
L7_RC_t
snmpAgentSwitchSnoopingQuerierVlanModeGet (L7_uint32 vlanId,
                                           L7_uint32 *val, L7_uchar8 family)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbSnoopQuerierVlanModeGet(vlanId, &temp_val, family);

  if (rc == L7_SUCCESS)
  {
    switch(temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSwitchSnoopingQuerierVlanAdminMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSwitchSnoopingQuerierVlanAdminMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }
  return rc;
}
L7_RC_t
snmpAgentSwitchSnoopingQuerierVlanOperModeGet (L7_uint32 vlanId,
                                           L7_uint32 *val, L7_uchar8 family)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbSnoopQuerierOperStateGet(vlanId, &temp_val, family);

  if (rc == L7_SUCCESS)
  {
    switch(temp_val)
    {
    case L7_SNOOP_QUERIER_DISABLED:
      *val = D_agentSwitchSnoopingQuerierVlanOperMode_disabled;
      break;
    case L7_SNOOP_QUERIER_QUERIER:
      *val = D_agentSwitchSnoopingQuerierVlanOperMode_querier;
      break;
    case L7_SNOOP_QUERIER_NON_QUERIER:
      *val = D_agentSwitchSnoopingQuerierVlanOperMode_non_querier;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }
  return rc;
}
L7_RC_t
snmpAgentSwitchSnoopingQuerierVlanElectionModeGet (L7_uint32 vlanId,
                                                   L7_uint32 *val, L7_uchar8 family)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbSnoopQuerierVlanElectionModeGet(vlanId, &temp_val, family);

  if (rc == L7_SUCCESS)
  {
    switch(temp_val)
    {
    case L7_DISABLE:
      *val = D_agentSwitchSnoopingQuerierElectionParticipateMode_disable;
      break;
    case L7_ENABLE:
      *val = D_agentSwitchSnoopingQuerierElectionParticipateMode_enable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }
  return rc;
}

L7_RC_t
snmpAgentSwitchSnoopingQuerierVlanModeSet(L7_uint32 vlanId,
                                          L7_uint32 val, L7_uchar8 family)
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch(val)
  {
  case D_agentSwitchSnoopingQuerierVlanAdminMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSwitchSnoopingQuerierVlanAdminMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbSnoopQuerierVlanModeSet(vlanId, temp_val, family);
  }
  return rc;
}
L7_RC_t
snmpAgentSwitchSnoopingQuerierVlanElectionModeSet(L7_uint32 vlanId,
                                                  L7_uint32 val, L7_uchar8 family)
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch(val)
  {
  case D_agentSwitchSnoopingQuerierElectionParticipateMode_disable:
    temp_val = L7_DISABLE;
    break;

  case D_agentSwitchSnoopingQuerierElectionParticipateMode_enable:
    temp_val = L7_ENABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbSnoopQuerierVlanElectionModeSet(vlanId, temp_val, family);
  }
  return rc;
}

L7_RC_t
snmpAgentSwitchSnoopingQuerierOperMaxResponseTimeGet(L7_uint32 vlanId,
                                                     L7_int32  *maxRespTime,
                                                     L7_uchar8 family)
{
  L7_uint32 vlanMode  = L7_NULL;
  L7_uint32 operState = L7_NULL;

  if (usmDbSnoopQuerierVlanModeGet(vlanId, &vlanMode, family) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (L7_ENABLE == vlanMode)
  {
    /* Operational State */
    if (usmDbSnoopQuerierOperStateGet(vlanId, &operState, family) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    if (L7_SNOOP_QUERIER_QUERIER == operState)
    {
      if (usmDbSnoopQuerierOperMaxRespTimeGet(vlanId, maxRespTime, family) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      return L7_SUCCESS;
    }
  }

  /* (-1) - indicates that parameter is not configured.*/
  *maxRespTime = -1;
  return L7_SUCCESS;
}

L7_RC_t
snmpAgentSwitchSnoopingQuerierLastQuerierAddressGet(L7_uint32 vlanId,
                                                    L7_uchar8  *addr,
                                                    L7_uchar8 family)
{
  L7_uint32 vlanMode  = L7_NULL;
  L7_uint32 operState = L7_NULL;

  if (usmDbSnoopQuerierVlanModeGet(vlanId, &vlanMode, family) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (L7_ENABLE == vlanMode)
  {
    /* Operational State */
    if (usmDbSnoopQuerierOperStateGet(vlanId, &operState, family) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    if (L7_SNOOP_QUERIER_QUERIER == operState)
    {
      if (usmDbSnoopQuerierLastQuerierAddressGet(vlanId, addr, family) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
    }
  }

  /*If querier on such vlan/family isn't configured -
   field should be leaved empty.*/
  return L7_SUCCESS;
}

L7_RC_t
snmpAgentSwitchSnoopingQuerierLastQuerierVersionGet(L7_uint32 vlanId,
                                                   L7_int32  *qVer,
                                                   L7_uchar8 family)
{
  L7_uint32 vlanMode  = L7_NULL;
  L7_uint32 operState = L7_NULL;

  if (usmDbSnoopQuerierVlanModeGet(vlanId, &vlanMode, family) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (L7_ENABLE == vlanMode)
  {
    /* Operational State */
    if (usmDbSnoopQuerierOperStateGet(vlanId, &operState, family) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    if (L7_SNOOP_QUERIER_QUERIER == operState)
    {
      if (usmDbSnoopQuerierLastQuerierVersionGet(vlanId, qVer, family) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      return L7_SUCCESS;
    }
  }

  /* (-1) - indicates that parameter is not configured.*/
  *qVer = -1;
  return L7_SUCCESS;
}

/**************************************************************************************************************/
L7_RC_t
snmpAgentSwitchMFDBEntryGet ( L7_uint32 UnitIndex, L7_uint32 vlanId, L7_uchar8 *macAddress, L7_uint32 protocolType )
{
  L7_RC_t rc;
  L7_uint32 temp_protocolType = 0;
  L7_uchar8 vidMac[L7_MFDB_KEY_SIZE];

  rc = usmDbEntryVidMacCombine(vlanId, macAddress, vidMac);

  if (rc == L7_SUCCESS)
  {
    switch (protocolType)
    {
    case D_agentSwitchMFDBProtocolType_static:
      temp_protocolType = L7_MFDB_PROTOCOL_STATIC;
      break;

    case D_agentSwitchMFDBProtocolType_gmrp:
      temp_protocolType = L7_MFDB_PROTOCOL_GMRP;
      break;

    case D_agentSwitchMFDBProtocolType_igmp:
      temp_protocolType = L7_MFDB_PROTOCOL_IGMP;
      break;

    case D_agentSwitchMFDBProtocolType_mld:
      temp_protocolType = L7_MFDB_PROTOCOL_MLD;
      break;

    default:
      rc = L7_FAILURE;
      break;
    }
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbMfdbEntryProtocolGetExact(UnitIndex, vidMac, temp_protocolType);
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchMFDBEntryNextGet ( L7_uint32 UnitIndex, L7_uint32 *vlanId, L7_uchar8 *macAddress, L7_uint32 *protocolType )
{
  L7_RC_t rc;
  L7_uint32 temp_protocolType;
  L7_uchar8 vidMac[L7_MFDB_KEY_SIZE];

  rc = usmDbEntryVidMacCombine(*vlanId, macAddress, vidMac);

  if (rc == L7_SUCCESS)
  {
    if (*protocolType < D_agentSwitchMFDBProtocolType_static)
    {
      temp_protocolType = L7_NULL;
    }
    else if (*protocolType > D_agentSwitchMFDBProtocolType_mld)
    {
      temp_protocolType = L7_MFDB_PROTOCOL_MLD;
    }
    else
    {
      switch (*protocolType)
      {
      case D_agentSwitchMFDBProtocolType_static:
        temp_protocolType = L7_MFDB_PROTOCOL_STATIC;
        break;

      case D_agentSwitchMFDBProtocolType_gmrp:
        temp_protocolType = L7_MFDB_PROTOCOL_GMRP;
        break;

      case D_agentSwitchMFDBProtocolType_igmp:
        temp_protocolType = L7_MFDB_PROTOCOL_IGMP;
        break;

      case D_agentSwitchMFDBProtocolType_mld:
        temp_protocolType = L7_MFDB_PROTOCOL_MLD;
        break;

      default:
        /* should never reach this point */
        rc = L7_FAILURE;
        break;
      }
    }
  }

  if (rc == L7_SUCCESS)
  {
    /* loop until no more or successful translation */
    while ((rc = usmDbMfdbEntryProtocolGetNext(UnitIndex, vidMac, &temp_protocolType)) == L7_SUCCESS)
    {
      switch (temp_protocolType)
      {
      case L7_MFDB_PROTOCOL_STATIC:
        *protocolType = D_agentSwitchMFDBProtocolType_static;
        break;

      case L7_MFDB_PROTOCOL_GMRP:
        *protocolType = D_agentSwitchMFDBProtocolType_gmrp;
        break;

      case L7_MFDB_PROTOCOL_IGMP:
        *protocolType = D_agentSwitchMFDBProtocolType_igmp;
        break;

      case L7_MFDB_PROTOCOL_MLD:
        *protocolType = D_agentSwitchMFDBProtocolType_mld;
        break;

      default:
        rc = L7_FAILURE;
        break;
      }

      if (rc == L7_SUCCESS && usmDbEntryVidMacSeparate(vidMac, vlanId, macAddress) == L7_SUCCESS)
      {
        break;
      }
    }
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchMFDBEntryInfoGet ( L7_uint32 UnitIndex, L7_uint32 vlanId, L7_uchar8 *macAddress, L7_uint32 protocolType, usmdbMfdbUserInfo_t *info )
{
  L7_RC_t rc;
  L7_uint32 temp_protocolType = 0;
  L7_uchar8 vidMac[L7_MFDB_KEY_SIZE];

  rc = usmDbEntryVidMacCombine(vlanId, macAddress, vidMac);

  if (rc == L7_SUCCESS)
  {
    switch (protocolType)
    {
    case D_agentSwitchMFDBProtocolType_static:
      temp_protocolType = L7_MFDB_PROTOCOL_STATIC;
      break;

    case D_agentSwitchMFDBProtocolType_gmrp:
      temp_protocolType = L7_MFDB_PROTOCOL_GMRP;
      break;

    case D_agentSwitchMFDBProtocolType_igmp:
      temp_protocolType = L7_MFDB_PROTOCOL_IGMP;
      break;

    case D_agentSwitchMFDBProtocolType_mld:
      temp_protocolType = L7_MFDB_PROTOCOL_MLD;
      break;

    default:
      rc = L7_FAILURE;
      break;
    }
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbMfdbEntryProtocolInfoGet(UnitIndex, vidMac, temp_protocolType, info);
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchMFDBTypeGet ( L7_uint32 UnitIndex, L7_uint32 vlanId, L7_uchar8 *macAddress, L7_uint32 protocolType, L7_uint32 *val )
{
  L7_RC_t rc;
  usmdbMfdbUserInfo_t info;

  rc = snmpAgentSwitchMFDBEntryInfoGet(UnitIndex, vlanId, macAddress, protocolType, &info);

  if (rc == L7_SUCCESS)
  {
    switch (info.usmdbMfdbType)
    {
    case L7_MFDB_TYPE_STATIC:
      *val = D_agentSwitchMFDBType_static;
      break;

    case L7_MFDB_TYPE_DYNAMIC:
      *val = D_agentSwitchMFDBType_dynamic;
      break;

    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchMFDBDescriptionGet ( L7_uint32 UnitIndex, L7_uint32 vlanId, L7_uchar8 *macAddress, L7_uint32 protocolType, L7_uchar8 *buf )
{
  L7_RC_t rc = L7_FAILURE;
  usmdbMfdbUserInfo_t info;

  if (snmpAgentSwitchMFDBEntryInfoGet(UnitIndex, vlanId, macAddress, protocolType, &info) == L7_SUCCESS)
  {
    strcpy(buf, info.usmdbMfdbDescr);
    rc = L7_SUCCESS;
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchMFDBForwardingPortMaskGet ( L7_uint32 UnitIndex, L7_uint32 vlanId, L7_uchar8 *macAddress, L7_uint32 protocolType, L7_uchar8 *buf, L7_uint32 *buf_length )
{
  L7_RC_t rc = L7_FAILURE;
  usmdbMfdbUserInfo_t info;


  if (snmpAgentSwitchMFDBEntryInfoGet(UnitIndex, vlanId, macAddress, protocolType, &info) == L7_SUCCESS &&
      usmDbReverseMask(info.usmdbMfdbFwdMask, &info.usmdbMfdbFwdMask) == L7_SUCCESS)
  {
    memcpy(buf, info.usmdbMfdbFwdMask.value, L7_INTF_INDICES);
    *buf_length = L7_INTF_INDICES;
    rc = L7_SUCCESS;
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchMFDBFilteringPortMaskGet ( L7_uint32 UnitIndex, L7_uint32 vlanId, L7_uchar8 *macAddress, L7_uint32 protocolType, L7_uchar8 *buf, L7_uint32 *buf_length )
{
  L7_RC_t rc = L7_FAILURE;
  usmdbMfdbUserInfo_t info;

  if (snmpAgentSwitchMFDBEntryInfoGet(UnitIndex, vlanId, macAddress, protocolType, &info) == L7_SUCCESS &&
      usmDbReverseMask(info.usmdbMfdbFwdMask, &info.usmdbMfdbFwdMask) == L7_SUCCESS)
  {
    memcpy(buf, info.usmdbMfdbFltMask.value, L7_INTF_INDICES);
    *buf_length = L7_INTF_INDICES;
    rc = L7_SUCCESS;
  }

  return rc;
}

/**************************************************************************************************************/

L7_RC_t
snmpAgentSwitchMFDBSummaryEntryGet ( L7_uint32 UnitIndex, L7_uint32 vlanId, L7_uchar8 *macAddress )
{
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 vidMac[L7_MFDB_KEY_SIZE];
  L7_INTF_MASK_t absMask;

  if (usmDbEntryVidMacCombine(vlanId, macAddress, vidMac) == L7_SUCCESS &&
      usmDbMfdbEntryFwdIntfGetNext(UnitIndex, vidMac, L7_MATCH_EXACT, &absMask) == L7_SUCCESS)
  {
    rc = L7_SUCCESS;
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchMFDBSummaryEntryNextGet ( L7_uint32 UnitIndex, L7_uint32 *vlanId, L7_uchar8 *macAddress )
{
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 vidMac[L7_MFDB_KEY_SIZE];
  L7_INTF_MASK_t absMask;

  if (usmDbEntryVidMacCombine(*vlanId, macAddress, vidMac) == L7_SUCCESS &&
      usmDbMfdbEntryFwdIntfGetNext(UnitIndex, vidMac, L7_MATCH_GETNEXT, &absMask) == L7_SUCCESS &&
      usmDbEntryVidMacSeparate(vidMac, vlanId, macAddress) == L7_SUCCESS)
  {
    rc = L7_SUCCESS;
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchMFDBSummaryForwardingPortMaskGet ( L7_uint32 UnitIndex, L7_uint32 vlanId, L7_uchar8 *macAddress, L7_uchar8 *buf, L7_uint32 *buf_length )
{
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 vidMac[L7_MFDB_KEY_SIZE];
  L7_INTF_MASK_t absMask;

  if (usmDbEntryVidMacCombine(vlanId, macAddress, vidMac) == L7_SUCCESS &&
      usmDbMfdbEntryFwdIntfGetNext(UnitIndex, vidMac, L7_MATCH_EXACT, &absMask) == L7_SUCCESS &&
      usmDbReverseMask(absMask, &absMask) == L7_SUCCESS)
  {
    memcpy(buf, absMask.value, L7_INTF_INDICES);
    *buf_length = L7_INTF_INDICES;
    rc = L7_SUCCESS;
  }

  return rc;
}



/**************************************************************************************************************/
L7_RC_t
snmpAgentTransferUploadModeGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbTransferModeGet ( UnitIndex, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRANSFER_TFTP:
      *val = D_agentTransferUploadMode_tftp;
      break;

    case L7_TRANSFER_XMODEM:
      *val = D_agentTransferUploadMode_xmodem;
      break;

#ifdef _L7_OS_LINUX_
    case L7_TRANSFER_YMODEM:
      *val = D_agentTransferUploadMode_ymodem;
      break;

    case L7_TRANSFER_ZMODEM:
      *val = D_agentTransferUploadMode_zmodem;
      break;
#endif /* _L7_OS_LINUX_ */

    case L7_TRANSFER_SFTP:
      *val = D_agentTransferUploadMode_sftp;
      break;

    case L7_TRANSFER_SCP:
      *val = D_agentTransferUploadMode_scp;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}


L7_RC_t
snmpAgentTransferUploadDataTypeGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbTransferUploadFileTypeGet ( UnitIndex, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
#ifdef D_agentTransferUploadDataType_config
    case L7_FILE_TYPE_CONFIG:
      *val = D_agentTransferUploadDataType_config;
      break;
#endif

#ifdef D_agentTransferUploadDataType_errorlog
    case L7_FILE_TYPE_ERRORLOG:
      *val = D_agentTransferUploadDataType_errorlog;
      break;
#endif

#ifdef D_agentTransferUploadDataType_messagelog
    case L7_FILE_TYPE_SYSTEM_MSGLOG:
      *val = D_agentTransferUploadDataType_messagelog;
      break;
#endif

#ifdef D_agentTransferUploadDataType_traplog
    case L7_FILE_TYPE_TRAP_LOG:
      *val = D_agentTransferUploadDataType_traplog;
      break;
#endif

#ifdef D_agentTransferUploadDataType_clibanner
    case L7_FILE_TYPE_CLI_BANNER:
      *val = D_agentTransferUploadDataType_clibanner;
       break;
#endif

#ifdef D_agentTransferUploadDataType_code
    case L7_FILE_TYPE_CODE:
      *val = D_agentTransferUploadDataType_code;
       break;
#endif

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}


L7_RC_t
snmpAgentTransferUploadStartGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  /* always disabled: trigger value */
  *val = D_agentTransferUploadStart_disable;
  return L7_SUCCESS;
}


L7_RC_t
snmpAgentTransferUploadModeSet  ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentTransferUploadMode_tftp:
    temp_val = L7_TRANSFER_TFTP;
    break;
  case D_agentTransferUploadMode_xmodem:
    temp_val = L7_TRANSFER_XMODEM;
    break;

#ifdef _L7_OS_LINUX_
  case D_agentTransferUploadMode_ymodem:
    temp_val = L7_TRANSFER_YMODEM;
    break;
  case D_agentTransferUploadMode_zmodem:
    temp_val = L7_TRANSFER_ZMODEM;
    break;
#endif /* _L7_OS_LINUX_ */

#ifdef L7_MGMT_SECURITY_PACKAGE
  case D_agentTransferUploadMode_sftp:
    if (usmDbFeaturePresentCheck(UnitIndex,L7_FLEX_SSHD_COMPONENT_ID,
                                 L7_SSHD_SECURE_TRANSFER_FEATURE_ID) == L7_TRUE)
    {
      temp_val = L7_TRANSFER_SFTP;
    }
    else
    {
      rc = L7_FAILURE;
    }
    break;

  case D_agentTransferUploadMode_scp:
    if (usmDbFeaturePresentCheck(UnitIndex,L7_FLEX_SSHD_COMPONENT_ID,
                                 L7_SSHD_SECURE_TRANSFER_FEATURE_ID) == L7_TRUE)
    {
      temp_val = L7_TRANSFER_SCP;
    }
    else
    {
      rc = L7_FAILURE;
    }
    break;
#endif /* L7_MGMT_SECURITY_PACKAGE */
  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbTransferModeSet ( UnitIndex, temp_val );
  }

  return rc;
}

L7_RC_t
snmpAgentTransferUploadServerAddressTypeSet( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
    case D_agentTransferUploadServerAddressType_ipv4:
      temp_val = L7_AF_INET;  /* check if we need to define enum for this */
      break;

    case D_agentTransferUploadServerAddressType_ipv6:
      temp_val = L7_AF_INET6;
      break;

    default:
      rc = L7_FAILURE;
      break;
  }
  if ( rc == L7_SUCCESS)
  {
    rc = usmDbTransferServerAddressTypeSet(UnitIndex, temp_val);
  }
  return rc;
}

L7_RC_t
snmpAgentTransferDownloadServerAddressTypeSet( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
    case D_agentTransferDownloadServerAddressType_ipv4:
      temp_val = L7_AF_INET;  /* check if we need to define enum for this */
      break;

    case D_agentTransferDownloadServerAddressType_ipv6:
      temp_val = L7_AF_INET6;
      break;

    default:
      rc = L7_FAILURE;
      break;
  }
  if ( rc == L7_SUCCESS)
  {
    rc = usmDbTransferServerAddressTypeSet(UnitIndex, temp_val);
  }
  return rc;
}

L7_RC_t
snmpAgentTransferDownloadServerAddressTypeGet( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t   rc;

  rc = usmDbTransferServerAddressTypeGet(UnitIndex, &temp_val);
  if ( rc == L7_SUCCESS )
  {
    switch (temp_val)
    {
      case L7_AF_INET:
        *val = D_agentTransferDownloadServerAddressType_ipv4;
      break;

      case L7_AF_INET6:
        *val = D_agentTransferDownloadServerAddressType_ipv6;
        break;

      default:
        *val = D_agentTransferDownloadServerAddressType_unknown;
        break;
    }
  }
  return rc;
}

L7_RC_t
snmpAgentTransferUploadServerAddressTypeGet( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t   rc;

  rc = usmDbTransferServerAddressTypeGet(UnitIndex, &temp_val);
  if ( rc == L7_SUCCESS )
  {
    switch (temp_val)
    {
      case L7_AF_INET:
        *val = D_agentTransferUploadServerAddressType_ipv4;
      break;

      case L7_AF_INET6:
        *val = D_agentTransferUploadServerAddressType_ipv6;
        break;

      default:
        *val = D_agentTransferUploadServerAddressType_unknown;
        break;
    }
  }
  return rc;
}

L7_RC_t
snmpAgentTransferServerAddressSet( L7_uint32 UnitIndex, L7_char8 *buf )
{
  L7_inet_addr_t inetAddr;
  L7_RC_t   rc;

  rc = usmDbParseInetAddrFromStr(buf, &inetAddr);
  if (rc == L7_SUCCESS)
  {
    rc = usmDbTransferServerAddressSet(UnitIndex, (L7_uchar8 *)&inetAddr.addr);
  }

  return rc;
}

L7_RC_t
snmpAgentTransferServerAddressGet( L7_uint32 UnitIndex, L7_char8 *buf, L7_uint32 bufSize )
{
  L7_inet_addr_t inetAddr;
  L7_uint32 family;
  L7_uchar8 *temp_ptr;
  L7_RC_t   rc;

  rc = usmDbTransferServerAddressGet(UnitIndex, (L7_uchar8 *)&inetAddr.addr);
  if (rc == L7_SUCCESS)
  {
    rc = usmDbTransferServerAddressTypeGet(UnitIndex, &family);
    if (rc == L7_SUCCESS)
    {
      temp_ptr = osapiInetNtop(family, (L7_uchar8 *)&inetAddr.addr, (L7_uchar8 *)buf, bufSize);
      if (temp_ptr != (L7_uchar8 *)buf)
      {
        rc = L7_FAILURE;
      }
    }
  }

  return rc;
}

L7_RC_t
snmpAgentTransferFilePathSet( L7_uint32 UnitIndex, L7_char8 *buf )
{
  L7_RC_t rc;

  rc = usmDbStringAlphaNumericPlusCheck(buf, "\\/_-", L7_NULLPTR);
  if ( rc == L7_SUCCESS)
  {
    rc = usmDbTransferFilePathRemoteSet(UnitIndex, buf);
  }
  return rc;
}

L7_RC_t
snmpAgentTransferFileNameSet( L7_uint32 UnitIndex, L7_char8 *buf )
{
  L7_RC_t rc;
  L7_uint32 fileType;

  rc = usmDbStringAlphaNumericPlusCheck(buf, "._-", L7_NULLPTR);
  if ( rc == L7_SUCCESS)
  {
    rc = usmDbTransferUploadFileTypeGet(UnitIndex,&fileType);
    if (rc == L7_SUCCESS)
    {
      if (fileType == L7_FILE_TYPE_CONFIG_SCRIPT)
      {
        rc = setConfigScriptSourceFileName(buf);
        rc = usmDbTransferFileNameRemoteSet(UnitIndex, buf);

      }
      else if (fileType == L7_FILE_TYPE_CODE)
      {
        return L7_FAILURE;
      }
      else
      {
        rc = usmDbTransferFileNameLocalSet(UnitIndex, buf);
        rc = usmDbTransferFileNameRemoteSet(UnitIndex, buf);
      }
    }
  }
  return rc;
}

L7_RC_t
snmpAgentTransferUploadDataTypeSet  ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 buf[DIM_MAX_FILENAME_SIZE];

  switch (val)
  {
#ifdef D_agentTransferUploadDataType_config
  case D_agentTransferUploadDataType_config:
    temp_val = L7_FILE_TYPE_CONFIG_SCRIPT;
    break;
#endif

#ifdef D_agentTransferUploadDataType_operationallog
  case D_agentTransferUploadDataType_operationallog:
    temp_val = L7_FILE_TYPE_OPERATIONAL_LOG;
    break;
#endif

#ifdef D_agentTransferUploadDataType_startuplog
  case D_agentTransferUploadDataType_startuplog:
    temp_val = L7_FILE_TYPE_STARTUP_LOG;
    break;
#endif

#ifdef D_agentTransferUploadDataType_errorlog
  case D_agentTransferUploadDataType_errorlog:
    temp_val = L7_FILE_TYPE_ERRORLOG;
    break;
#endif

#ifdef D_agentTransferUploadDataType_messagelog
  case D_agentTransferUploadDataType_messagelog:
    temp_val = L7_FILE_TYPE_SYSTEM_MSGLOG;
    break;
#endif

#ifdef D_agentTransferUploadDataType_traplog
  case D_agentTransferUploadDataType_traplog:
    temp_val = L7_FILE_TYPE_TRAP_LOG;
    break;
#endif

#ifdef D_agentTransferUploadDataType_clibanner
  case D_agentTransferUploadDataType_clibanner:
    temp_val = L7_FILE_TYPE_CLI_BANNER;
    break;
#endif

#ifdef D_agentTransferUploadDataType_code
  case D_agentTransferUploadDataType_code:
    temp_val = L7_FILE_TYPE_CODE;
    break;
#endif

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbTransferUploadFileTypeSet ( UnitIndex, temp_val );
  }

  if (rc == L7_SUCCESS && temp_val == L7_FILE_TYPE_CODE)
  {
    /* Need active image name of the manager unit */
    rc = usmDbUnitMgrNumberGet(&UnitIndex);
    if(rc == L7_SUCCESS)
    {
      rc = usmDbActiveImageNameGet(UnitIndex,buf);
      if(rc == L7_SUCCESS)
      {
        if(usmDbImageExists(buf) == L7_TRUE)
        {
          rc = usmDbTransferFileNameLocalSet(UnitIndex, buf);
          rc = usmDbTransferFileNameRemoteSet(UnitIndex, buf);
        }
      }
    }
  }

  return rc;
}


L7_RC_t
snmpAgentTransferUploadStartSet  ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_RC_t rc = L7_FAILURE;
  L7_int32 temp_val;

  if (val == D_agentTransferUploadStart_enable)
  {
    /* prevent SNMP from initiating an XMODEM transfer */
    if (usmDbTransferModeGet(UnitIndex, &temp_val) == L7_SUCCESS &&
        temp_val != L7_TRANSFER_XMODEM)
    {
      rc = usmDbTransferUpStartSet(UnitIndex);
    }
  }

  return rc;
}


L7_RC_t
snmpAgentTransferUploadStatusGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;

  if (usmDbTransferModeGet(UnitIndex, &temp_val) == L7_SUCCESS)
  {
    if (usmDbTransferUploadResultCodeGet(UnitIndex, &temp_val) == L7_SUCCESS)
    {
      switch (temp_val)
      {
      case L7_TRANSFER_CODE_NONE:
        *val = D_agentTransferUploadStatus_notInitiated;
        break;

      case L7_TRANSFER_CODE_STARTING:
        *val = D_agentTransferUploadStatus_transferStarting;
        break;

      case L7_TRANSFER_CODE_ERROR_STARTING:
        *val = D_agentTransferUploadStatus_errorStarting;
        break;

      case L7_TRANSFER_CODE_WRONG_FILE_TYPE:
        *val = D_agentTransferUploadStatus_wrongFileType;
        break;

      case L7_TRANSFER_CODE_UPDATE_CONFIG:
        *val = D_agentTransferUploadStatus_updatingConfig;
        break;

      case L7_TRANSFER_CODE_INVALID_CONFIG:
        *val = D_agentTransferUploadStatus_invalidConfigFile;
        break;

      case L7_TRANSFER_CODE_WRITING_FLASH:
        *val = D_agentTransferUploadStatus_writingToFlash;
        break;

      case L7_TRANSFER_CODE_FLASH_FAILED:
        *val = D_agentTransferUploadStatus_failureWritingToFlash;
        break;

      case L7_TRANSFER_CODE_CHECKING_CRC:
        *val = D_agentTransferUploadStatus_checkingCRC;
        break;

      case L7_TRANSFER_CODE_CRC_FAILURE:
        *val = D_agentTransferUploadStatus_failedCRC;
        break;

      case L7_TRANSFER_CODE_UNKNOWN_DIRECTION:
        *val = D_agentTransferUploadStatus_unknownDirection;
        break;

      case L7_TRANSFER_CODE_SUCCESS:
        *val = D_agentTransferUploadStatus_transferSuccessful;
        break;

      case L7_TRANSFER_CODE_FAILURE:
        *val = D_agentTransferUploadStatus_transferFailed;
        break;

      default:
        *val = 0;
        return L7_FAILURE;
        break;
      }
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}


/**************************************************************************************************************/

L7_RC_t
snmpAgentTransferDownloadModeGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbTransferModeGet ( UnitIndex, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {

    case L7_TRANSFER_TFTP:
      *val = D_agentTransferDownloadMode_tftp;
      break;

    case L7_TRANSFER_SFTP:
      *val = D_agentTransferDownloadMode_sftp;
      break;

    case L7_TRANSFER_SCP:
      *val = D_agentTransferDownloadMode_scp;
      break;

    case L7_TRANSFER_XMODEM:
      *val = D_agentTransferDownloadMode_xmodem;
      break;

#ifdef _L7_OS_LINUX_
    case L7_TRANSFER_YMODEM:
      *val = D_agentTransferDownloadMode_ymodem;
      break;

    case L7_TRANSFER_ZMODEM:
      *val = D_agentTransferDownloadMode_zmodem;
      break;
#endif /* _L7_OS_LINUX_ */

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}


L7_RC_t
snmpAgentTransferDownloadDataTypeGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbTransferDownloadFileTypeGet ( UnitIndex, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_FILE_TYPE_CODE:
      *val = D_agentTransferDownloadDataType_code;
      break;

    case L7_FILE_TYPE_CONFIG:
      *val = D_agentTransferDownloadDataType_config;
      break;

    case L7_FILE_TYPE_SSHKEY_RSA1:
      *val = D_agentTransferDownloadDataType_sshkey_rsa1;
      break;

    case L7_FILE_TYPE_SSHKEY_RSA2:
      *val = D_agentTransferDownloadDataType_sshkey_rsa2;
      break;

    case L7_FILE_TYPE_SSHKEY_DSA:
      *val = D_agentTransferDownloadDataType_sshkey_dsa;
      break;

    case L7_FILE_TYPE_SSLPEM_ROOT:
      *val = D_agentTransferDownloadDataType_sslpem_root;
      break;

#ifdef D_agentTransferDownloadDataType_tr069_acs_sslpem_root
    case L7_FILE_TYPE_TR069_ACS_SSLPEM_ROOT:
      *val = D_agentTransferDownloadDataType_tr069_acs_sslpem_root;
      break;
#endif

#ifdef D_agentTransferDownloadDataType_tr069_client_ssl_private_key
    case L7_FILE_TYPE_TR069_CLIENT_SSLPRIV_KEY:
      *val = D_agentTransferDownloadDataType_tr069_client_ssl_private_key;
      break;
#endif

#ifdef D_agentTransferDownloadDataType_tr069_client_ssl_cert
    case L7_FILE_TYPE_TR069_CLIENT_SSL_CERT:
      *val = D_agentTransferDownloadDataType_tr069_client_ssl_cert;
      break;
#endif

    case L7_FILE_TYPE_SSLPEM_SERVER:
      *val = D_agentTransferDownloadDataType_sslpem_server;
      break;

    case L7_FILE_TYPE_SSLPEM_DHWEAK:
      *val = D_agentTransferDownloadDataType_sslpem_dhweak;
      break;

    case L7_FILE_TYPE_SSLPEM_DHSTRONG:
      *val = D_agentTransferDownloadDataType_sslpem_dhstrong;
      break;

    case L7_FILE_TYPE_CLI_BANNER:
      *val = D_agentTransferDownloadDataType_clibanner;
      break;

    case L7_FILE_TYPE_KERNEL:
      *val = D_agentTransferDownloadDataType_kernel;
      break;

#ifdef D_agentTransferDownloadDataType_ias_users
    case L7_FILE_TYPE_IAS_USERS:
      *val = D_agentTransferDownloadDataType_ias_users;
      break;
#endif

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}


L7_RC_t
snmpAgentTransferDownloadStartGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  *val = D_agentTransferDownloadStart_disable;
  return L7_SUCCESS;
}


L7_RC_t
snmpAgentTransferDownloadStatusGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;

  if (usmDbTransferModeGet(UnitIndex, &temp_val) == L7_SUCCESS)
  {
    if (usmDbTransferDownloadResultCodeGet(UnitIndex, &temp_val) == L7_SUCCESS)
    {
      switch (temp_val)
      {
      case L7_TRANSFER_CODE_NONE:
        *val = D_agentTransferDownloadStatus_notInitiated;
        break;

      case L7_TRANSFER_CODE_STARTING:
        *val = D_agentTransferDownloadStatus_transferStarting;
        break;

      case L7_TRANSFER_CODE_ERROR_STARTING:
        *val = D_agentTransferDownloadStatus_errorStarting;
        break;

      case L7_TRANSFER_CODE_WRONG_FILE_TYPE:
        *val = D_agentTransferDownloadStatus_wrongFileType;
        break;

      case L7_TRANSFER_CODE_UPDATE_CONFIG:
        *val = D_agentTransferDownloadStatus_updatingConfig;
        break;

      case L7_TRANSFER_CODE_INVALID_CONFIG:
        *val = D_agentTransferDownloadStatus_invalidConfigFile;
        break;

      case L7_TRANSFER_CODE_WRITING_FLASH:
        *val = D_agentTransferDownloadStatus_writingToFlash;
        break;

      case L7_TRANSFER_CODE_FLASH_FAILED:
        *val = D_agentTransferDownloadStatus_failureWritingToFlash;
        break;

      case L7_TRANSFER_CODE_CHECKING_CRC:
        *val = D_agentTransferDownloadStatus_checkingCRC;
        break;

      case L7_TRANSFER_CODE_CRC_FAILURE:
        *val = D_agentTransferDownloadStatus_failedCRC;
        break;

      case L7_TRANSFER_CODE_UNKNOWN_DIRECTION:
        *val = D_agentTransferDownloadStatus_unknownDirection;
        break;

      case L7_TRANSFER_CODE_SUCCESS:
        *val = D_agentTransferDownloadStatus_transferSuccessful;
        break;

      case L7_TRANSFER_CODE_FAILURE:
        *val = D_agentTransferDownloadStatus_transferFailed;
        break;

      default:
        *val = 0;
        return L7_FAILURE;
        break;
      }
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}



L7_RC_t
snmpAgentTransferDownloadModeSet  ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentTransferDownloadMode_tftp:
    temp_val = L7_TRANSFER_TFTP;
    break;

#ifdef L7_MGMT_SECURITY_PACKAGE
  case D_agentTransferDownloadMode_sftp:
    if (usmDbFeaturePresentCheck(UnitIndex,L7_FLEX_SSHD_COMPONENT_ID,
                                 L7_SSHD_SECURE_TRANSFER_FEATURE_ID) == L7_TRUE)
    {
      temp_val = L7_TRANSFER_SFTP;
    }
    else
    {
      rc = L7_FAILURE;
    }
    break;

  case D_agentTransferDownloadMode_scp:
    if (usmDbFeaturePresentCheck(UnitIndex,L7_FLEX_SSHD_COMPONENT_ID,
                                 L7_SSHD_SECURE_TRANSFER_FEATURE_ID) == L7_TRUE)
    {
      temp_val = L7_TRANSFER_SCP;
    }
    else
    {
      rc = L7_FAILURE;
    }
    break;
#endif /* L7_MGMT_SECURITY_PACKAGE */

  case D_agentTransferDownloadMode_xmodem:
    temp_val = L7_TRANSFER_XMODEM;
    break;

#ifdef _L7_OS_LINUX_
  case D_agentTransferDownloadMode_ymodem:
    temp_val = L7_TRANSFER_YMODEM;
    break;

  case D_agentTransferDownloadMode_zmodem:
    temp_val = L7_TRANSFER_ZMODEM;
    break;
#endif /* _L7_OS_LINUX_ */

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbTransferModeSet ( UnitIndex, temp_val );
  }

  return rc;
}


L7_RC_t
snmpAgentTransferDownloadDataTypeSet  ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentTransferDownloadDataType_code:
    temp_val = L7_FILE_TYPE_CODE;
    break;

  case D_agentTransferDownloadDataType_config:
    temp_val = L7_FILE_TYPE_CONFIG;
    break;

  case D_agentTransferDownloadDataType_sshkey_rsa1:
    temp_val = L7_FILE_TYPE_SSHKEY_RSA1;
    break;

  case D_agentTransferDownloadDataType_sshkey_rsa2:
    temp_val = L7_FILE_TYPE_SSHKEY_RSA2;
    break;

  case D_agentTransferDownloadDataType_sshkey_dsa:
    temp_val = L7_FILE_TYPE_SSHKEY_DSA;
    break;

  case D_agentTransferDownloadDataType_sslpem_root:
    temp_val = L7_FILE_TYPE_SSLPEM_ROOT;
    break;

#ifdef D_agentTransferDownloadDataType_tr069_acs_sslpem_root
  case D_agentTransferDownloadDataType_tr069_acs_sslpem_root:
    temp_val = L7_FILE_TYPE_TR069_ACS_SSLPEM_ROOT;
    break;
#endif

#ifdef D_agentTransferDownloadDataType_tr069_client_ssl_private_key
  case D_agentTransferDownloadDataType_tr069_client_ssl_private_key:
    temp_val = L7_FILE_TYPE_TR069_ACS_SSLPEM_ROOT;
    break;
#endif

#ifdef D_agentTransferDownloadDataType_tr069_client_ssl_cert
  case D_agentTransferDownloadDataType_tr069_client_ssl_cert:
    temp_val = L7_FILE_TYPE_TR069_ACS_SSLPEM_ROOT;
    break;
#endif

  case D_agentTransferDownloadDataType_sslpem_server:
    temp_val = L7_FILE_TYPE_SSLPEM_SERVER;
    break;

  case D_agentTransferDownloadDataType_sslpem_dhweak:
    temp_val = L7_FILE_TYPE_SSLPEM_DHWEAK;
    break;

  case D_agentTransferDownloadDataType_sslpem_dhstrong:
    temp_val = L7_FILE_TYPE_SSLPEM_DHSTRONG;
    break;

  case D_agentTransferDownloadDataType_clibanner:
    temp_val = L7_FILE_TYPE_CLI_BANNER;
    break;

  case D_agentTransferDownloadDataType_kernel:
  #if defined(_L7_OS_LINUX_) && defined(L7_SDK_EXCLUDES_KERNEL)
    temp_val = L7_FILE_TYPE_KERNEL;
  #else
    rc=L7_FAILURE;
  #endif
    break;
  
#ifdef D_agentTransferDownloadDataType_ias_users
  case D_agentTransferDownloadDataType_ias_users:
    temp_val = L7_FILE_TYPE_IAS_USERS;
    break;
#endif

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbTransferDownloadFileTypeSet ( UnitIndex, temp_val );
  }

  return rc;
}


L7_RC_t
snmpAgentTransferDownloadStartSet  ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_RC_t rc = L7_FAILURE;
  L7_int32 temp_val;

  if (val == D_agentTransferDownloadStart_enable)
  {
    /* prevent SNMP from initiating a non TFTP transfer */
    if (usmDbTransferModeGet(UnitIndex, &temp_val) == L7_SUCCESS &&
        (temp_val == L7_TRANSFER_TFTP || temp_val == L7_TRANSFER_SFTP || temp_val == L7_TRANSFER_SCP))
    {
      /* When downloading SSH key files */
      if (usmDbTransferDownloadFileTypeGet(UnitIndex, &temp_val) == L7_SUCCESS &&
          (temp_val == L7_FILE_TYPE_SSHKEY_RSA1 ||
           temp_val == L7_FILE_TYPE_SSHKEY_RSA2 ||
           temp_val == L7_FILE_TYPE_SSHKEY_DSA))
      {
#ifdef L7_MGMT_SECURITY_PACKAGE
        /* check that SSH Admin Mode is disabled, and number of sessions is 0 */
        if (usmDbsshdAdminModeGet(UnitIndex, &temp_val) == L7_SUCCESS &&
            temp_val == L7_DISABLE &&
            usmDbsshdNumSessionsGet(UnitIndex, &temp_val) == L7_SUCCESS &&
            temp_val == 0)
#endif /* L7_MGMT_SECURITY_PACKAGE */
          /* allow transfer to start */
          rc = usmDbTransferDownStartSet(UnitIndex);
      }
      else
      {
        rc = usmDbTransferDownStartSet(UnitIndex);
      }
    }
  }

  return rc;
}

/**************************************************************************************************************/
#ifdef OLD_CODE
L7_RC_t
snmpAgentMirroredPortIfIndexGet ( L7_uint32 UnitIndex, L7_uint32 *index )
{
  L7_uint32 intIfNum;

  if (usmDbSwPortMonitorSourcePortGet(UnitIndex, &intIfNum) == L7_SUCCESS)
  {
    if (intIfNum == 0)
    {
      *index = 0;
      return L7_SUCCESS;
    }
    return usmDbExtIfNumFromIntIfNum(intIfNum, index);
  }

  return L7_FAILURE;
}

L7_RC_t
snmpAgentProbePortIfIndexGet ( L7_uint32 UnitIndex, L7_uint32 *index )
{
  L7_uint32 intIfNum;

  if (usmDbSwPortMonitorDestPortGet(UnitIndex, &intIfNum) == L7_SUCCESS)
  {
    if (intIfNum == 0)
    {
      *index = 0;
      return L7_SUCCESS;
    }
    return usmDbExtIfNumFromIntIfNum(intIfNum, index);
  }

  return L7_FAILURE;
}

L7_RC_t
snmpAgentPortMirroringModeGet( L7_uint32 UnitIndex, L7_uint32 *portMonitorMode )
{
  L7_uint32 mode;
  L7_RC_t rc;

  rc = usmDbSwPortMonitorModeGet(UnitIndex, &mode);
  if (rc == L7_SUCCESS)
  {
    switch (mode)
    {
    case L7_ENABLE:
      *portMonitorMode = D_agentPortMirroringMode_enable;
      break;

    case L7_DISABLE:
      *portMonitorMode = D_agentPortMirroringMode_disable;
      break;

    default:
      *portMonitorMode = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentMirroredPortIfIndexSet ( L7_uint32 UnitIndex, L7_uint32 index )
{
  L7_uint32 intIfNum;

  if (usmDbIntIfNumFromExtIfNum(index, &intIfNum) == L7_SUCCESS)
  {
    return usmDbSwPortMonitorSourcePortSet(UnitIndex, intIfNum);
  }

  return L7_FAILURE;
}

L7_RC_t
snmpAgentProbePortIfIndexSet ( L7_uint32 UnitIndex, L7_uint32 index )
{
  L7_uint32 intIfNum;

  if (usmDbIntIfNumFromExtIfNum(index, &intIfNum) == L7_SUCCESS)
  {
    return usmDbSwPortMonitorDestPortSet(UnitIndex, intIfNum);
  }

  return L7_FAILURE;
}

L7_RC_t
snmpAgentPortMirroringModeSet( L7_uint32 UnitIndex, L7_uint32 portMonitorMode )
{
  L7_uint32 mode;
  L7_RC_t rc = L7_SUCCESS;

  switch (portMonitorMode)
  {
  case D_agentPortMirroringMode_enable:
    mode = L7_ENABLE;
    break;

  case D_agentPortMirroringMode_disable:
    mode = L7_DISABLE;
    break;

  case D_agentPortMirroringMode_delete:
    if (usmDbSwPortMonitorModeGet(UnitIndex, &mode) == L7_SUCCESS && mode == L7_DISABLE)
      return usmDbSwPortMonitorConfigRemove(UnitIndex);
    else
      return L7_FAILURE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
    rc = usmDbSwPortMonitorModeSet(UnitIndex, mode);

  return rc;
}
#endif

L7_RC_t
snmpAgentPortMirrorEntryGet( L7_uint32 agentPortMirrorSessionNum )
{
  L7_uint32 sessionCount;

  sessionCount = usmDbSwPortMonitorSessionCount(USMDB_UNIT_CURRENT);

  if (agentPortMirrorSessionNum >= 1 &&
      agentPortMirrorSessionNum <= sessionCount)
    return L7_SUCCESS;

  return L7_FAILURE;
}

L7_RC_t
snmpAgentPortMirrorEntryNextGet( L7_uint32 *agentPortMirrorSessionNum )
{
  /* test for the next session number */
  *agentPortMirrorSessionNum = *agentPortMirrorSessionNum + 1;

  return snmpAgentPortMirrorEntryGet(*agentPortMirrorSessionNum);
}


L7_RC_t
snmpAgentPortMirrorSourcePortMaskGet ( L7_uint32 agentPortMirrorSessionNum, L7_char8 *buf, L7_uint32 *buf_len )
{
  L7_RC_t rc;
  L7_INTF_MASK_t temp_val;

  rc = usmDbSwPortMonitorSourcePortsGet(USMDB_UNIT_CURRENT, agentPortMirrorSessionNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    /* reverse the bits from internal mode to SNMP mode */
    L7_INTF_MASKREV(temp_val);
    memcpy(buf, temp_val.value, L7_INTF_INDICES);
    *buf_len = L7_INTF_INDICES;
  }

  return rc;
}

L7_RC_t
snmpAgentPortMirrorSourcePortMaskSet ( L7_uint32 agentPortMirrorSessionNum, L7_char8 *buf, L7_uint32 buf_len )
{
  L7_RC_t rc;
  L7_INTF_MASK_t temp_val;
  L7_INTF_MASK_t current_val;
  L7_uint32 k, j;

  if (buf_len > L7_INTF_INDICES)
    return L7_FAILURE;

  bzero(temp_val.value, L7_INTF_INDICES);
  memcpy(temp_val.value, buf, buf_len);

  /* reverse the bits from the SNMP mode to internal mode */
  L7_INTF_MASKREV(temp_val);

  rc = usmDbSwPortMonitorSourcePortsGet(USMDB_UNIT_CURRENT, agentPortMirrorSessionNum, &current_val);

  if (rc == L7_SUCCESS)
  {
    for (k = 0; k < L7_INTF_INDICES && k < buf_len; k++)
    {
      /* interfaces 1 to 8 within a byte */
      for (j = 1; j <= 8; j++)
      {
        /* check if bits are different */
        /* kth byte, jth interface */
        if (L7_INTF_ISMASKBITSET(temp_val, 8*k + j) !=
            L7_INTF_ISMASKBITSET(current_val, 8*k + j))
        {
          if (L7_INTF_ISMASKBITSET(temp_val, 8*k + j))
          {
            /* add port */
            /* change usmDb call to reflect direction per source port */
            if (usmDbSwPortMonitorSourcePortAdd(USMDB_UNIT_CURRENT, agentPortMirrorSessionNum, 8*k + j,L7_MIRROR_BIDIRECTIONAL) != L7_SUCCESS)
              return L7_FAILURE;
          }
          else
          {
            /* remove port */
            if (usmDbSwPortMonitorSourcePortRemove(USMDB_UNIT_CURRENT, agentPortMirrorSessionNum, 8*k + j) != L7_SUCCESS)
              return L7_FAILURE;
          }
        }
      }
    }
  }

  return rc;
}

L7_RC_t
snmpAgentPortMirrorAdminModeGet( L7_uint32 agentPortMirrorSessionNum, L7_uint32 *val )
{
  L7_RC_t rc;
  L7_BOOL temp_val;

  rc = usmDbSwPortMonitorModeGet(USMDB_UNIT_CURRENT, agentPortMirrorSessionNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *val = D_agentPortMirrorAdminMode_enable;
      break;

    case L7_FALSE:
      *val = D_agentPortMirrorAdminMode_disable;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentPortMirrorAdminModeSet( L7_uint32 agentPortMirrorSessionNum, L7_uint32 val )
{
  L7_RC_t rc = L7_SUCCESS;
  L7_BOOL temp_val = 0;

  switch (val)
  {
  case D_agentPortMirrorAdminMode_enable:
    temp_val = L7_TRUE;
    break;

  case D_agentPortMirrorAdminMode_disable:
    temp_val = L7_FALSE;
    break;

  case D_agentPortMirrorAdminMode_delete:
    /* if deleting */
    return usmDbSwPortMonitorSessionRemove(USMDB_UNIT_CURRENT, agentPortMirrorSessionNum);

  default:
    /* unknown value */
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbSwPortMonitorModeSet(USMDB_UNIT_CURRENT, agentPortMirrorSessionNum, temp_val);
  }

  return rc;
}

L7_RC_t
snmpAgentPortMirrorProbePortSet( L7_uint32 agentPortMirrorSessionNum, L7_uint32 destIfNum )
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 currDest;
  L7_uint32 sessionNum;

  rc=usmDbSwPortMonitorDestPortGet(USMDB_UNIT_CURRENT,agentPortMirrorSessionNum,&currDest);
  if (rc == L7_SUCCESS)
  {
    if (currDest !=0)
    {

      if (currDest != destIfNum)
      {
        if ((usmDbSwPortMonitorIsValidDestPort(destIfNum) != L7_TRUE) ||
            (usmDbSwPortMonitorIsDestConfigured(USMDB_UNIT_CURRENT,destIfNum,&sessionNum) == L7_TRUE) ||
            (usmDbSwPortMonitorIsSrcConfigured(USMDB_UNIT_CURRENT,destIfNum) == L7_TRUE))
          {

            /*failed probe port checks */
            rc= L7_FAILURE;
          }
          else
          {
            /* remove probe port and then reset it*/
            rc=usmDbSwPortMonitorDestPortRemove(USMDB_UNIT_CURRENT,agentPortMirrorSessionNum);
            if (rc == L7_SUCCESS)
              rc = usmDbSwPortMonitorDestPortSet(USMDB_UNIT_CURRENT, agentPortMirrorSessionNum, destIfNum);
            if (rc != L7_SUCCESS)
            {
              rc= usmDbSwPortMonitorDestPortSet(USMDB_UNIT_CURRENT,agentPortMirrorSessionNum,currDest);
            }

          }
      }
    }
    else
      rc = usmDbSwPortMonitorDestPortSet(USMDB_UNIT_CURRENT, agentPortMirrorSessionNum, destIfNum);
  }
  else
    rc = usmDbSwPortMonitorDestPortSet(USMDB_UNIT_CURRENT, agentPortMirrorSessionNum, destIfNum);

  return rc;
}

L7_RC_t
snmpAgentPortMirrorTypeEntryGet(L7_uint32 sessionNum, L7_uint32 srcIfNum)
{
  L7_uint32      listSrcPorts[L7_MAX_INTERFACE_COUNT + 1];
  L7_uint32      numPorts;
  L7_INTF_MASK_t srcIfMask;
  L7_uint32      index = 0;
  L7_uint32      intSrcIfNum;

  if(usmDbIntIfNumFromExtIfNum(srcIfNum, &intSrcIfNum) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if(usmDbSwPortMonitorSourcePortsGet(USMDB_UNIT_CURRENT, sessionNum,
                                      &srcIfMask) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if(usmDbConvertMaskToList(&srcIfMask, listSrcPorts, &numPorts) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (numPorts < 1)
  {
    return L7_FAILURE;
  }

  for (index = 1; index <= numPorts; index++)
  {
    if(listSrcPorts[index] == intSrcIfNum)
      return L7_SUCCESS;
    if(listSrcPorts[index] > intSrcIfNum)
      break;
  }
  return L7_FAILURE;
}

L7_RC_t
snmpAgentPortMirrorSourceNextGet(L7_uint32 sessionNum, L7_uint32 *srcIfNum)
{
  L7_uint32      listSrcPorts[L7_MAX_INTERFACE_COUNT + 1];
  L7_uint32      numPorts;
  L7_INTF_MASK_t srcIfMask;
  L7_uint32      index = 0;
  L7_uint32      intSrcIfNum;

  if(*srcIfNum == 0)
  {
    intSrcIfNum = 0;
  }
  else if(usmDbIntIfNumFromExtIfNum(*srcIfNum, &intSrcIfNum) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if(usmDbSwPortMonitorSourcePortsGet(USMDB_UNIT_CURRENT, sessionNum,
                                      &srcIfMask) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if(usmDbConvertMaskToList(&srcIfMask, listSrcPorts, &numPorts) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (numPorts < 1)
  {
    return L7_FAILURE;
  }

  for (index = 1; index <= numPorts; index++)
  {
    if(listSrcPorts[index] > intSrcIfNum)
    {
      intSrcIfNum = listSrcPorts[index];
      return usmDbExtIfNumFromIntIfNum(intSrcIfNum, srcIfNum);
    }
  }
  return L7_FAILURE;
}

L7_RC_t
snmpAgentPortMirrorTypeEntryGetNext(L7_uint32 *sessionNum, L7_uint32 *srcIfNum)
{
  if((snmpAgentPortMirrorEntryGet(*sessionNum) == L7_SUCCESS) &&
    (snmpAgentPortMirrorSourceNextGet(*sessionNum, srcIfNum) == L7_SUCCESS))
  {
    return L7_SUCCESS;
  }
  *srcIfNum = 0;
  while(snmpAgentPortMirrorEntryNextGet(sessionNum) == L7_SUCCESS)
  {
    if(snmpAgentPortMirrorSourceNextGet(*sessionNum, srcIfNum) == L7_SUCCESS)
    {
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

L7_RC_t
snmpAgentPortMirrorTypeTypeGet(L7_uint32 sessionNum, L7_uint32 srcIfNum,
                               L7_uint32 *portMirrorType)
{
  L7_uint32      intSrcIfNum;
  L7_MIRROR_DIRECTION_t probeType;

  if(usmDbIntIfNumFromExtIfNum(srcIfNum, &intSrcIfNum) != L7_SUCCESS)
    return L7_FAILURE;
  if(usmDbMirrorSourcePortDirectionGet(sessionNum, intSrcIfNum,
                                       &probeType) != L7_SUCCESS)
    return L7_FAILURE;
  switch(probeType)
  {
    case L7_MIRROR_BIDIRECTIONAL:
      *portMirrorType = D_agentPortMirrorTypeType_txrx;
      break;
    case L7_MIRROR_INGRESS:
      *portMirrorType = D_agentPortMirrorTypeType_rx;
      break;
    case L7_MIRROR_EGRESS:
      *portMirrorType = D_agentPortMirrorTypeType_tx;
      break;
    default:
      return L7_FAILURE;
  }
  return L7_SUCCESS;
}

L7_RC_t
snmpAgentPortMirrorTypeTypeSet(L7_uint32 sessionNum, L7_uint32 srcIfNum, L7_uint32 portMirrorType)
{
  L7_uint32      intSrcIfNum;
  L7_MIRROR_DIRECTION_t probeType;

  if(snmpAgentPortMirrorTypeEntryGet(sessionNum, srcIfNum) != L7_SUCCESS)
    return L7_FAILURE;
  if(usmDbIntIfNumFromExtIfNum(srcIfNum, &intSrcIfNum) != L7_SUCCESS)
    return L7_FAILURE;
  switch(portMirrorType)
  {
    case D_agentPortMirrorTypeType_tx:
      probeType = L7_MIRROR_EGRESS;
      break;
    case D_agentPortMirrorTypeType_rx:
      probeType = L7_MIRROR_INGRESS;
      break;
    case D_agentPortMirrorTypeType_txrx:
      probeType = L7_MIRROR_BIDIRECTIONAL;
      break;
    default:
      return L7_FAILURE;
  }
  if((usmDbSwPortMonitorSourcePortRemove(USMDB_UNIT_CURRENT, sessionNum,
                                         intSrcIfNum) == L7_SUCCESS) &&
    (usmDbSwPortMonitorSourcePortAdd(USMDB_UNIT_CURRENT, sessionNum,
                                     intSrcIfNum, probeType) == L7_SUCCESS))
  {
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/**************************************************************************************************************/

L7_RC_t
snmpDot3adAggPortLacpModeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *mode)
{
  L7_uint32 lacpMode;
  L7_RC_t rc;

  rc = usmDbDot3adAggPortLacpModeGet(UnitIndex, intIfNum, &lacpMode);
  if (rc == L7_SUCCESS)
  {
    if (lacpMode == L7_ENABLE)
      *mode = D_agentDot3adAggPortLACPMode_enable;
    else if (lacpMode == L7_DISABLE)
      *mode = D_agentDot3adAggPortLACPMode_disable;
    else
      rc = L7_FAILURE;
  }

  return rc;
}

L7_RC_t
snmpDot3adAggPortLacpModeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 mode)
{
  L7_uint32 lacpMode;

  if (mode == D_agentDot3adAggPortLACPMode_enable)
    lacpMode = L7_ENABLE;
  else if (mode == D_agentDot3adAggPortLACPMode_disable)
    lacpMode = L7_DISABLE;
  else
    return L7_FAILURE;

  return usmDbDot3adAggPortLacpModeSet(UnitIndex, intIfNum, lacpMode);
}

/**************************************************************************************************************/

L7_RC_t
snmpAgentPortIanaTypeGet ( L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  /* lvl7_@p0662 start */
  rc = usmDbIfTypeGet ( UnitIndex, intIfIndex, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_IANA_ETHERNET:
    case L7_IANA_FAST_ETHERNET:
    case L7_IANA_FAST_ETHERNET_FX:
    case L7_IANA_GIGABIT_ETHERNET:
    case L7_IANA_2G5_ETHERNET:      /* PTin added: Speed 2.5G */
    case L7_IANA_10G_ETHERNET:
    case L7_IANA_40G_ETHERNET:      /* PTin added: Speed 40G */
    case L7_IANA_100G_ETHERNET:     /* PTin added: Speed 100G */
      *val = D_agentPortIanaType_ethernetCsmacd;
      break;
    case L7_IANA_AAL5:
      *val = D_agentPortIanaType_aal5;
      break;
    case L7_IANA_PROP_VIRTUAL:
      *val = D_agentPortIanaType_propVirtual;
      break;
    case L7_IANA_L2_VLAN:
      *val = D_agentPortIanaType_l2vlan;
      break;
    case L7_IANA_LAG_DESC:
      *val = D_agentPortIanaType_ieee8023adLag;
      break;
    default:
      *val = D_agentPortIanaType_other;
    }
  }
  return rc;
  /* lvl7_@p0662 end */
}

L7_RC_t
snmpAgentPortAdminModeGet ( L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbIfAdminStateGet ( UnitIndex, intIfIndex, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentPortAdminMode_enable;
      break;

    case L7_DISABLE:
    case L7_DIAG_DISABLE:
      *val = D_agentPortAdminMode_disable;
      break;
    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

/* lvl7_@p1953 start */
#ifdef NOT_SUPPORTED
L7_RC_t
snmpAgentPortPhysicalModeGet ( L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbIfSpeedGet ( UnitIndex, intIfIndex, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_PORTCTRL_PORTSPEED_AUTO_NEG:
      *val = D_agentPortPhysicalMode_auto_negotiate;
      break;

    case L7_PORTCTRL_PORTSPEED_HALF_10T:
      *val = D_agentPortPhysicalMode_half_10;
      break;

    case L7_PORTCTRL_PORTSPEED_FULL_10T:
      *val = D_agentPortPhysicalMode_full_10;
      break;

    case L7_PORTCTRL_PORTSPEED_HALF_100TX:
      *val = D_agentPortPhysicalMode_half_100;
      break;

    case L7_PORTCTRL_PORTSPEED_FULL_100TX:
      *val = D_agentPortPhysicalMode_full_100;
      break;

    case L7_PORTCTRL_PORTSPEED_FULL_100FX:
      *val = D_agentPortPhysicalMode_full_100fx;
      break;

    case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
      *val = D_agentPortPhysicalMode_full_1000sx;
      break;

    /* PTin NOTE (2.5G)
     * This piece of code is not compiled... so there is no point on adding any
     * 2.5G reference here.
     */

    case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
      *val = D_agentPortPhysicalMode_full_10gsx;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }
  return rc;
}
#endif /* NOT_SUPPORTED */
/* lvl7_@p1953 end */

L7_RC_t
snmpAgentPortDefaultTypeGet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uchar8 *buf)
{
  L7_uint32 ifSpeed;

  L7_RC_t rc;
  rc = usmDbIfDefaultSpeedGet(UnitIndex, intIfIndex, &ifSpeed);
  if (rc == L7_SUCCESS)
  {
    switch (ifSpeed)
    {
    case L7_PORTCTRL_PORTSPEED_HALF_10T:
      strcpy(buf, "dot3MauType10BaseTHD");
      break;

    case L7_PORTCTRL_PORTSPEED_FULL_10T:
      strcpy(buf, "dot3MauType10BaseTFD");
      break;

    case L7_PORTCTRL_PORTSPEED_HALF_100TX:
      strcpy(buf, "dot3MauType100BaseTXHD");
      break;

    case L7_PORTCTRL_PORTSPEED_FULL_100TX:
      strcpy(buf, "dot3MauType100BaseTXFD");
      break;

    case L7_PORTCTRL_PORTSPEED_FULL_100FX:
      strcpy(buf, "dot3MauType100BaseFXFD");
      break;


    default:
      strcpy(buf, "0.0");
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentPortTypeGet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uchar8 *buf)
{
  L7_uint32 ifSpeed;
  L7_uint32 ifConnectorType;
  L7_RC_t rc;

  rc = usmDbIfSpeedStatusGet(UnitIndex, intIfIndex, &ifSpeed);

  if (rc == L7_SUCCESS)
  {
    switch (ifSpeed)
    {
    case L7_PORTCTRL_PORTSPEED_HALF_10T:
      strcpy(buf, "dot3MauType10BaseTHD");
      break;

    case L7_PORTCTRL_PORTSPEED_FULL_10T:
      strcpy(buf, "dot3MauType10BaseTFD");
      break;

    case L7_PORTCTRL_PORTSPEED_HALF_100TX:
      strcpy(buf, "dot3MauType100BaseTXHD");
      break;

    case L7_PORTCTRL_PORTSPEED_FULL_100TX:
      strcpy(buf, "dot3MauType100BaseTXFD");
      break;

    case L7_PORTCTRL_PORTSPEED_FULL_100FX:
      strcpy(buf, "dot3MauType100BaseFXFD");
      break;

    case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
      rc = usmDbIntfConnectorTypeGet(intIfIndex, &ifConnectorType);

      if (rc == L7_SUCCESS)
      {
        switch (ifConnectorType)
        {
        case L7_MTRJ:
          strcpy(buf, "dot3MauType1000BaseSXFD");
          break;

        case L7_RJ45:
          strcpy(buf, "dot3MauType1000BaseTFD");
          break;

        default:
          strcpy(buf, "dot3MauType1000BaseXFD");
          break;
        }
      }
      else
      {
        /* if connector type is unknown, return the default */
        strcpy(buf, "dot3MauType1000BaseXFD");
        rc = L7_SUCCESS;
      }

      break;

    /* PTin NOTE (2.5G)
     * There is no ethernet physical medium defined for 2.5G! This speed
     * appeared with SGMII and there is not IEEE standard definition. So there
     * is no point in referencing any physical medium over here.
     */

    case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
      strcpy(buf, "dot3MauType10GigBaseX");
      break;

    default:
      strcpy(buf, "0.0");
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentPortAutoNegAdminStatusGet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_int32 *status)
{
  L7_uint32 adminStatus;
  L7_RC_t rc;

  rc = usmDbIfAutoNegAdminStatusGet(UnitIndex, intIfIndex, &adminStatus);
  if (rc == L7_SUCCESS)
  {
    if (adminStatus == L7_ENABLE)
      *status = D_agentPortAutoNegAdminStatus_enable;
    else if (adminStatus == L7_DISABLE)
      *status = D_agentPortAutoNegAdminStatus_disable;
    else
      rc = L7_FAILURE;
  }
  return rc;
}


L7_RC_t
snmpAgentPortDot3FlowControlModeGet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_int32 *status)
{
  L7_uint32 mode;
  L7_RC_t rc;

  rc = usmDbIfFlowCtrlModeGet(UnitIndex, intIfIndex, &mode);
  if (rc == L7_SUCCESS)
  {
    if (mode == L7_ENABLE)
      *status = D_agentPortDot3FlowControlMode_enable;
    else if (mode == L7_DISABLE)
      *status = D_agentPortDot3FlowControlMode_disable;
    else
      rc = L7_FAILURE;
  }
  return rc;
}

#ifdef L7_DHCP_FILTER_PACKAGE

L7_RC_t
snmpDhcpFilterIntfGet(L7_uint32 extIfNum)
{
  L7_uint32 intIfNum;

  if (usmDbIntIfNumFromExtIfNum(extIfNum, &intIfNum) == L7_SUCCESS)
  {
     if (usmDbDhcpFilterIsValidIntf(intIfNum) == L7_TRUE)
        return L7_SUCCESS;
     else
        return L7_FAILURE;
  }

  return L7_FAILURE;
}

L7_RC_t
snmpNextDhcpFilterIntfGet(L7_uint32 extIfNum, L7_uint32 *nextExtIfNum )
{
  L7_uint32 intIfNum;

  /* check to see if this is a visible external interface number */
  while (usmDbGetNextVisibleExtIfNumber(extIfNum, nextExtIfNum) == L7_SUCCESS)
  {
    if (usmDbIntIfNumFromExtIfNum(*nextExtIfNum, &intIfNum) == L7_SUCCESS)
    {
      if ( usmDbDhcpFilterIsValidIntf(intIfNum) == L7_TRUE)
        return L7_SUCCESS;
      else
        extIfNum = *nextExtIfNum;
    }
  }
  return L7_FAILURE;
}

L7_RC_t snmpDhcpFilteringPortTrustedModeGet(L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val;
  rc  = usmDbDhcpFilterIntfModeGet(intIfNum, &temp_val);
  if (L7_SUCCESS == rc)
  {
    switch (temp_val)
    {
      case L7_ENABLE:
        *val = D_agentDhcpFilteringPortTrustedMode_trusted;
        rc = L7_SUCCESS;
        break;
      case L7_DISABLE:
        *val = D_agentDhcpFilteringPortTrustedMode_untrusted;
        rc = L7_SUCCESS;
        break;
      default:
        rc = L7_FAILURE;
        break;
    }
  }

  return rc;
}

L7_RC_t snmpDhcpFilteringPortTrustedModeSet(L7_uint32 intIfNum, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val = L7_DISABLE;
  /*the modes should be somethng like L7_TRUSTED, L7_UNTRUSTED
   **but adhoc they will be L7_ENABLE,L7_DISABLE (since usmDb APIs are like that
   **/
  switch(val)
  {
    case D_agentDhcpFilteringPortTrustedMode_trusted:
      temp_val = L7_ENABLE;
      rc = L7_SUCCESS;
      break;
    case D_agentDhcpFilteringPortTrustedMode_untrusted:
      temp_val = L7_DISABLE;
      rc = L7_SUCCESS;
      break;
    default:
      rc = L7_FAILURE;
      return rc;
      break;
  }

  rc = usmDbDhcpFilterIntfModeSet(intIfNum,temp_val);
  return rc;
}

L7_RC_t snmpAgentDhcpFilteringAdminModeGet(L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val;

  rc = usmDbDhcpFilterAdminModeGet(&temp_val);
  if(rc == L7_SUCCESS)
  {
    switch(temp_val)
    {
      case L7_ENABLE:
        *val = D_agentDhcpFilteringAdminMode_enable;
        break;
      case L7_DISABLE:
        *val = D_agentDhcpFilteringAdminMode_disable;
        break;
      default:
        rc = L7_FAILURE;
    }
  }
  return rc;
}

L7_RC_t snmpAgentDhcpFilteringAdminModeSet(L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val;
  switch(val)
  {
    case D_agentDhcpFilteringAdminMode_enable:
      temp_val = L7_ENABLE;
      rc = L7_SUCCESS;
      break;
    case D_agentDhcpFilteringAdminMode_disable:
      temp_val = L7_DISABLE;
      rc = L7_SUCCESS;
      break;
    default:
      rc = L7_FAILURE;
      return rc;
  }
  rc = usmDbDhcpFilterAdminModeSet(temp_val);
  return rc;
}
#endif /* L7_DHCP_FILTER_PACKAGE */
#ifdef L7_DAI_PACKAGE
/****************************************/
/* SNMP APIs for Dynamic ARP Inspection */
/****************************************/
L7_RC_t snmpDaiSrcMacValidateGet(L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL temp_val;
  rc  = usmDbDaiVerifySMacGet(&temp_val);
  if (L7_SUCCESS == rc)
  {
    switch (temp_val)
    {
      case L7_TRUE:
        *val = D_agentDaiSrcMacValidate_true;
        rc = L7_SUCCESS;
        break;
      case L7_FALSE:
        *val = D_agentDaiSrcMacValidate_false;
        rc = L7_SUCCESS;
        break;
      default:
        rc = L7_FAILURE;
        break;
    }
  }

  return rc;
}

L7_RC_t snmpDaiSrcMacValidateSet(L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL temp_val = L7_FALSE;
  switch(val)
  {
    case D_agentDaiSrcMacValidate_true:
      temp_val = L7_TRUE;
      rc = L7_SUCCESS;
      break;
    case D_agentDaiSrcMacValidate_false:
      temp_val = L7_FALSE;
      rc = L7_SUCCESS;
      break;
    default:
      rc = L7_FAILURE;
      return rc;
      break;
  }

  rc = usmDbDaiVerifySMacSet(temp_val);
  return rc;
}

L7_RC_t snmpDaiDstMacValidateGet(L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL temp_val;
  rc  = usmDbDaiVerifyDMacGet(&temp_val);
  if (L7_SUCCESS == rc)
  {
    switch (temp_val)
    {
      case L7_TRUE:
        *val = D_agentDaiDstMacValidate_true;
        rc = L7_SUCCESS;
        break;
      case L7_FALSE:
        *val = D_agentDaiDstMacValidate_false;
        rc = L7_SUCCESS;
        break;
      default:
        rc = L7_FAILURE;
        break;
    }
  }

  return rc;
}

L7_RC_t snmpDaiDstMacValidateSet(L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL temp_val = L7_FALSE;
  switch(val)
  {
    case D_agentDaiDstMacValidate_true:
      temp_val = L7_TRUE;
      rc = L7_SUCCESS;
      break;
    case D_agentDaiDstMacValidate_false:
      temp_val = L7_FALSE;
      rc = L7_SUCCESS;
      break;
    default:
      rc = L7_FAILURE;
      return rc;
      break;
  }

  rc = usmDbDaiVerifyDMacSet(temp_val);
  return rc;
}

L7_RC_t snmpDaiIPValidateGet(L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL temp_val;
  rc  = usmDbDaiVerifyIPGet(&temp_val);
  if (L7_SUCCESS == rc)
  {
    switch (temp_val)
    {
      case L7_TRUE:
        *val = D_agentDaiIPValidate_true;
        rc = L7_SUCCESS;
        break;
      case L7_FALSE:
        *val = D_agentDaiIPValidate_false;
        rc = L7_SUCCESS;
        break;
      default:
        rc = L7_FAILURE;
        break;
    }
  }

  return rc;
}

L7_RC_t snmpDaiIPValidateSet(L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL temp_val = L7_FALSE;
  switch(val)
  {
    case D_agentDaiIPValidate_true:
      temp_val = L7_TRUE;
      rc = L7_SUCCESS;
      break;
    case D_agentDaiIPValidate_false:
      temp_val = L7_FALSE;
      rc = L7_SUCCESS;
      break;
    default:
      rc = L7_FAILURE;
      return rc;
      break;
  }

  rc = usmDbDaiVerifyIPSet(temp_val);
  return rc;
}
L7_RC_t snmpDaiVlanGet(L7_uint32 unit, L7_uint32 vlanId)
{

 if((vlanId < L7_DOT1Q_MIN_VLAN_ID) ||
    (vlanId > L7_DOT1Q_MAX_VLAN_ID))
   return L7_FAILURE;
 else
   return L7_SUCCESS;
}

L7_RC_t snmpDaiVlanNextGet(L7_uint32 unit, L7_uint32 *vlanId)
{
  return usmDbDaiVlanNextGet(*vlanId, vlanId);
}

L7_RC_t snmpDaiVlanDaiEnableGet(L7_uint32 vlanId, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val;
  rc  = usmDbDaiVlanEnableGet(vlanId, &temp_val);
  if (L7_SUCCESS == rc)
  {
    switch (temp_val)
    {
      case L7_ENABLE:
        *val = D_agentDaiVlanDynArpInspEnable_true;
        rc = L7_SUCCESS;
        break;
      case L7_DISABLE:
        *val = D_agentDaiVlanDynArpInspEnable_false;
        rc = L7_SUCCESS;
        break;
      default:
        rc = L7_FAILURE;
        break;
    }
  }

  return rc;
}

L7_RC_t snmpDaiVlanDaiEnableSet(L7_uint32 vlanId, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val = L7_DISABLE;
  switch(val)
  {
    case D_agentDaiVlanDynArpInspEnable_true:
      temp_val = L7_ENABLE;
      rc = L7_SUCCESS;
      break;
    case D_agentDaiVlanDynArpInspEnable_false:
      temp_val = L7_DISABLE;
      rc = L7_SUCCESS;
      break;
    default:
      rc = L7_FAILURE;
      return rc;
      break;
  }

  rc = usmDbDaiVlanEnableSet(vlanId,temp_val);
  return rc;
}

L7_RC_t snmpDaiVlanLoggingEnableGet(L7_uint32 vlanId, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val;
  rc  = usmDbDaiVlanLoggingEnableGet(vlanId, &temp_val);
  if (L7_SUCCESS == rc)
  {
    switch (temp_val)
    {
      case L7_ENABLE:
        *val = D_agentDaiVlanLoggingEnable_true;
        rc = L7_SUCCESS;
        break;
      case L7_DISABLE:
        *val = D_agentDaiVlanLoggingEnable_false;
        rc = L7_SUCCESS;
        break;
      default:
        rc = L7_FAILURE;
        break;
    }
  }

  return rc;
}

L7_RC_t snmpDaiVlanLoggingEnableSet(L7_uint32 vlanId, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val = L7_DISABLE;
  switch(val)
  {
    case D_agentDaiVlanLoggingEnable_true:
      temp_val = L7_ENABLE;
      rc = L7_SUCCESS;
      break;
    case D_agentDaiVlanLoggingEnable_false:
      temp_val = L7_DISABLE;
      rc = L7_SUCCESS;
      break;
    default:
      rc = L7_FAILURE;
      return rc;
      break;
  }

  rc = usmDbDaiVlanLoggingEnableSet(vlanId,temp_val);
  return rc;
}

L7_RC_t snmpDaiVlanArpAclStaticFlagGet(L7_uint32 vlanId, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val;
  rc  = usmDbDaiVlanArpAclStaticFlagGet(vlanId, &temp_val);
  if (L7_SUCCESS == rc)
  {
    switch (temp_val)
    {
      case L7_ENABLE:
        *val = D_agentDaiVlanArpAclStaticFlag_true;
        rc = L7_SUCCESS;
        break;
      case L7_DISABLE:
        *val = D_agentDaiVlanArpAclStaticFlag_false;
        rc = L7_SUCCESS;
        break;
      default:
        rc = L7_FAILURE;
        break;
    }
  }

  return rc;
}

L7_RC_t snmpDaiVlanArpAclStaticFlagSet(L7_uint32 vlanId, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val = L7_DISABLE;
  switch(val)
  {
    case D_agentDaiVlanArpAclStaticFlag_true:
      temp_val = L7_ENABLE;
      rc = L7_SUCCESS;
      break;
    case D_agentDaiVlanArpAclStaticFlag_false:
      temp_val = L7_DISABLE;
      rc = L7_SUCCESS;
      break;
    default:
      rc = L7_FAILURE;
      return rc;
      break;
  }

  rc = usmDbDaiVlanArpAclStaticFlagSet(vlanId,temp_val);
  return rc;
}

L7_RC_t
snmpDaiIntfGet(L7_uint32 extIfNum)
{
  L7_uint32 intIfNum;

  if (usmDbIntIfNumFromExtIfNum(extIfNum, &intIfNum) == L7_SUCCESS)
  {
     if (usmDbDaiIsValidIntf(intIfNum) == L7_TRUE)
        return L7_SUCCESS;
     else
        return L7_FAILURE;
  }

  return L7_FAILURE;
}

L7_RC_t
snmpDaiIntfNextGet(L7_uint32 extIfNum, L7_uint32 *nextExtIfNum)
{
  L7_uint32 intIfNum;

  /* check to see if this is a visible external interface number */
  while (usmDbGetNextVisibleExtIfNumber(extIfNum, nextExtIfNum) == L7_SUCCESS)
  {
    if (usmDbIntIfNumFromExtIfNum(*nextExtIfNum, &intIfNum) == L7_SUCCESS)
    {
      if (usmDbDaiIsValidIntf(intIfNum) == L7_TRUE)
        return L7_SUCCESS;
      else
        extIfNum = *nextExtIfNum;
    }
  }
  return L7_FAILURE;
}

L7_RC_t snmpDaiPortTrustModeGet(L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val;
  rc  = usmDbDaiIntfTrustGet(intIfNum, &temp_val);
  if (L7_SUCCESS == rc)
  {
    switch (temp_val)
    {
      case L7_ENABLE:
        *val = D_agentDaiIfTrustEnable_true;
        rc = L7_SUCCESS;
        break;
      case L7_DISABLE:
        *val = D_agentDaiIfTrustEnable_false;
        rc = L7_SUCCESS;
        break;
      default:
        rc = L7_FAILURE;
        break;
    }
  }

  return rc;
}

L7_RC_t snmpDaiPortTrustModeSet(L7_uint32 intIfNum, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val = L7_DISABLE;
  switch(val)
  {
    case D_agentDaiIfTrustEnable_true:
      temp_val = L7_ENABLE;
      rc = L7_SUCCESS;
      break;
    case D_agentDaiIfTrustEnable_false:
      temp_val = L7_DISABLE;
      rc = L7_SUCCESS;
      break;
    default:
      rc = L7_FAILURE;
      return rc;
      break;
  }

  rc = usmDbDaiIntfTrustSet(intIfNum,temp_val);
  return rc;
}

L7_RC_t snmpArpAclRowStatusSet(L7_char8 *aclName, L7_uint32 rowStatus)
{
  L7_RC_t rc = L7_SUCCESS;

  switch(rowStatus)
  {
  case D_agentArpAclRowStatus_active:
    rc = usmDbArpAclGet(aclName);
    break;

  case D_agentArpAclRowStatus_createAndGo:
    rc = usmDbArpAclCreate(aclName);
    break;
  case D_agentArpAclRowStatus_destroy:
    rc = usmDbArpAclDelete(aclName);
    break;

  default:
    rc = L7_FAILURE;
  }

  return rc;
}

L7_RC_t snmpArpAclRuleRowStatusSet(L7_char8 *aclName, L7_uint32 ipAddr,
                                   L7_char8 *mac_buffer, L7_uint32 rowStatus)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_enetMacAddr_t macAddr;

  snmpConvertStringToMac (mac_buffer, macAddr.addr);

  switch(rowStatus)
  {
  case D_agentArpAclRuleRowStatus_active:
    rc = usmDbArpAclRuleGet(aclName, ipAddr, &macAddr);
    break;

  case D_agentArpAclRuleRowStatus_createAndGo:
    rc = usmDbArpAclRuleAdd(aclName, ipAddr, &macAddr);
    break;
  case D_agentArpAclRuleRowStatus_destroy:
    rc = usmDbArpAclRuleDelete(aclName, ipAddr, &macAddr);
    break;

  default:
    rc = L7_FAILURE;
  }

  return rc;
}

#endif  /* L7_DAI_PACKAGE */

L7_RC_t
snmpSwitchAddrConflictDetectStatusGet(L7_int32 *val)
{
  L7_BOOL temp_val;
  L7_RC_t rc;

  rc = usmDbIPAddrConflictDetectStatusGet(&temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *val = D_agentSwitchAddressConflictDetectionStatus_true;
      break;

    case L7_FALSE:
      *val = D_agentSwitchAddressConflictDetectionStatus_false;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpSwitchLastConflictReportedTimeGet(L7_uint32 *val)
{
  L7_RC_t rc;

  rc = usmDbIPAddrConflictLastDetectTimeGet(val);

  if (rc == L7_SUCCESS)
  {
    *val = *val * 100;
  }

  return rc;
}

/*********************************************************************
* @purpose  Convert a MAC address in string format to hex.
*
* @param    macAddr  @b((input)) MAC string to be converted
* @param    mac      @b((output)) In hex format
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t snmpConvertStringToMac(L7_uchar8 *macStr, L7_uchar8 *macAddr)
{
  L7_uint32 i, j, digit_count = 0;
  L7_uchar8 mac_address[20];   /* more than enough */

  if (strlen(macStr) != 17)
  {      /* test string length */
    return L7_FAILURE;
  }

  for ( i=0,j=0; i<17; i++,j++ )
  {
    digit_count++;
    switch ( macStr[i] )
    {
    case '0':
      mac_address[j]=0x0;
      break;
    case '1':
      mac_address[j]=0x1;
      break;
    case '2':
      mac_address[j]=0x2;
      break;
    case '3':
      mac_address[j]=0x3;
      break;
    case '4':
      mac_address[j]=0x4;
      break;
    case '5':
      mac_address[j]=0x5;
      break;
    case '6':
      mac_address[j]=0x6;
      break;
    case '7':
      mac_address[j]=0x7;
      break;
    case '8':
      mac_address[j]=0x8;
      break;
    case '9':
      mac_address[j]=0x9;
      break;
    case 'a':
    case 'A':
      mac_address[j]=0xA;
      break;
    case 'b':
    case 'B':
      mac_address[j]=0xB;
      break;
    case 'c':
    case 'C':
      mac_address[j]=0xC;
      break;
    case 'd':
    case 'D':
      mac_address[j]=0xD;
      break;
    case 'e':
    case 'E':
      mac_address[j]=0xE;
      break;
    case 'f':
    case 'F':
      mac_address[j]=0xF;
      break;
    case ':':
      if (digit_count != 3)
      {    /* if more or less than 2 digits return false */
        return L7_FAILURE;
      }
      j--;
      digit_count=0;
      break;
    default:
      return L7_FAILURE;
      break;
    }
  }
  for ( i = 0; i < 6; i++ )
  {
    macAddr[i] = ( (mac_address[(i*2)] << 4) + mac_address[(i*2)+1] );
  }
  return L7_SUCCESS;
}

L7_RC_t
snmpSwitchProtectedPortEntryGet(L7_uint32 agentSwitchProtectedPortGroupId)
{
  if(agentSwitchProtectedPortGroupId >= 0 &&
     agentSwitchProtectedPortGroupId < L7_PROTECTED_PORT_MAX_GROUPS)
  {
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

L7_RC_t
snmpSwitchProtectedPortEntryGetNext(L7_uint32 *agentSwitchProtectedPortGroupId)
{
  (*agentSwitchProtectedPortGroupId)++;
  if(*agentSwitchProtectedPortGroupId < L7_PROTECTED_PORT_MAX_GROUPS)
  {
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

L7_RC_t
snmpSwitchProtectedPortGroupNameGet(L7_uint32 agentSwitchProtectedPortGroupId,
                                    L7_char8 *agentSwitchProtectedPortGroupName)
{
  return usmdbProtectedPortGroupNameGet(USMDB_UNIT_CURRENT,
                                        agentSwitchProtectedPortGroupId,
                                        agentSwitchProtectedPortGroupName);
}

L7_RC_t
snmpSwitchProtectedPortGroupNameSet(L7_uint32 agentSwitchProtectedPortGroupId,
                                    L7_char8 *agentSwitchProtectedPortGroupName)
{
  return usmdbProtectedPortGroupNameSet(USMDB_UNIT_CURRENT,
                                        agentSwitchProtectedPortGroupId,
                                        agentSwitchProtectedPortGroupName);
}

L7_RC_t
snmpSwitchProtectedPortPortListGet(L7_uint32 agentSwitchProtectedPortGroupId,
                                    L7_char8 *agentSwitchProtectedPortPortList)
{
  L7_INTF_MASK_t temp;
  if(usmdbProtectedPortGroupIntfMaskGet(USMDB_UNIT_CURRENT,
                                        agentSwitchProtectedPortGroupId,
                                        &temp) == L7_SUCCESS)
  {
    memcpy(agentSwitchProtectedPortPortList, temp.value, L7_INTF_INDICES);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

L7_RC_t
snmpSwitchProtectedPortPortListSet(L7_uint32 agentSwitchProtectedPortGroupId,
                                   L7_char8 *agentSwitchProtectedPortPortList)
{
  L7_RC_t rc;
  L7_uint32 intIfNum;
  L7_INTF_MASK_t newIfMask;
  L7_INTF_MASK_t oldIfMask;

  memset(&newIfMask, 0x00, L7_INTF_INDICES);
  memset(&oldIfMask, 0x00, L7_INTF_INDICES);
  memcpy(newIfMask.value, agentSwitchProtectedPortPortList, L7_INTF_INDICES);
  if(usmdbProtectedPortGroupIntfMaskGet(USMDB_UNIT_CURRENT,
                                        agentSwitchProtectedPortGroupId,
                                        &oldIfMask) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  L7_INTF_MASKEXOREQ(newIfMask,oldIfMask);
  rc = usmDbIntIfNumTypeFirstGet(USMDB_UNIT_CURRENT, USM_PHYSICAL_INTF, 0,
                                 &intIfNum);
  while(rc == L7_SUCCESS)
  {
    if (L7_INTF_ISMASKBITSET(newIfMask,intIfNum))
    {
      if(L7_INTF_ISMASKBITSET(oldIfMask, intIfNum))
      {
        /*Already protected, need to delete it */
        if (usmdbProtectedPortGroupIntfDelete(USMDB_UNIT_CURRENT,
                                              agentSwitchProtectedPortGroupId,
                                              intIfNum) != L7_SUCCESS)
        {
          return L7_FAILURE;
        }
      }
      else
      {
        /* Newly added, so need to add to protected list */
        if (usmdbProtectedPortGroupIntfAdd(USMDB_UNIT_CURRENT,
                                           agentSwitchProtectedPortGroupId,
                                           intIfNum) != L7_SUCCESS)
        {
          return L7_FAILURE;
        }
      }
    }
    rc = usmDbIntIfNumTypeNextGet(USMDB_UNIT_CURRENT,USM_PHYSICAL_INTF, 0,
                                  intIfNum, &intIfNum);
  }
  return L7_SUCCESS;
}

L7_RC_t
snmpAgentPortMaxFrameSizeGet(L7_uint32 UnitIndex,  L7_uint32 *maxFrameSize)
{
  return usmDbIfMaxFrameSizeGet(UnitIndex, maxFrameSize);
}


L7_RC_t
snmpAgentPortConfigMaxFrameSizeGet(L7_uint32 UnitIndex,  L7_uint32 *maxFrameSize)
{
  return usmDbIfConfigMaxFrameSizeGet(UnitIndex, maxFrameSize);
}

L7_RC_t
snmpAgentPortConfigMaxFrameSizeSet(L7_uint32 UnitIndex,  L7_uint32 maxFrameSize)
{
  return usmDbIfConfigMaxFrameSizeSet(UnitIndex, maxFrameSize);
}


L7_RC_t
snmpAgentPortBroadcastControlModeGet ( L7_uint32 interface, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbSwDevCtrlBcastStormModeIntfGet ( interface, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentPortBroadcastControlMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentPortBroadcastControlMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentPortMulticastControlModeGet ( L7_uint32 interface, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbSwDevCtrlMcastStormModeIntfGet ( interface, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentPortMulticastControlMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentPortMulticastControlMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentPortUnicastControlModeGet ( L7_uint32 interface, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbSwDevCtrlUcastStormModeIntfGet ( interface, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentPortUnicastControlMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentPortUnicastControlMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentPortBroadcastControlThresholdGet ( L7_uint32 interface, L7_int32 *val )
{
  L7_RC_t rc;
  L7_RATE_UNIT_t temp_val;

  rc = usmDbSwDevCtrlBcastStormThresholdIntfGet ( interface, val, &temp_val );

  return rc;
}


L7_RC_t
snmpAgentPortMulticastControlThresholdGet ( L7_uint32 interface, L7_int32 *val )
{
  L7_RC_t rc;
  L7_RATE_UNIT_t temp_val;

  rc = usmDbSwDevCtrlMcastStormThresholdIntfGet ( interface, val, &temp_val );

  return rc;
}


L7_RC_t
snmpAgentPortUnicastControlThresholdGet ( L7_uint32 interface, L7_int32 *val )
{
  L7_RC_t rc;
  L7_RATE_UNIT_t temp_val;

  rc = usmDbSwDevCtrlUcastStormThresholdIntfGet ( interface, val, &temp_val );

  return rc;
}

L7_RC_t
snmpAgentPortBroadcastControlThresholdUnitGet ( L7_uint32 interface, L7_int32 *val )
{
  L7_RC_t rc;
  L7_uint32 temp_val;
  L7_RATE_UNIT_t rate_unit;

  rc = usmDbSwDevCtrlBcastStormThresholdIntfGet ( interface, &temp_val, &rate_unit );

  if (rc == L7_SUCCESS)
  {
     switch (rate_unit)
     {
     case L7_RATE_UNIT_PERCENT:
         *val = D_agentPortBroadcastControlThresholdUnit_percent;
         break;

     case L7_RATE_UNIT_PPS:
         *val = D_agentPortBroadcastControlThresholdUnit_pps;
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
snmpAgentPortMulticastControlThresholdUnitGet ( L7_uint32 interface, L7_int32 *val )
{
  L7_RC_t rc;
  L7_uint32 temp_val;
  L7_RATE_UNIT_t rate_unit;

  rc = usmDbSwDevCtrlMcastStormThresholdIntfGet ( interface, &temp_val, &rate_unit );

  if (rc == L7_SUCCESS)
  {
     switch (rate_unit)
     {
     case L7_RATE_UNIT_PERCENT:
         *val = D_agentPortMulticastControlThresholdUnit_percent;
         break;

     case L7_RATE_UNIT_PPS:
         *val = D_agentPortMulticastControlThresholdUnit_pps;
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
snmpAgentPortUnicastControlThresholdUnitGet ( L7_uint32 interface, L7_int32 *val )
{
  L7_RC_t rc;
  L7_uint32 temp_val;
  L7_RATE_UNIT_t rate_unit;

  rc = usmDbSwDevCtrlUcastStormThresholdIntfGet ( interface, &temp_val, &rate_unit );

  if (rc == L7_SUCCESS)
  {
     switch (rate_unit)
     {
     case L7_RATE_UNIT_PERCENT:
         *val = D_agentPortUnicastControlThresholdUnit_percent;
         break;

     case L7_RATE_UNIT_PPS:
         *val = D_agentPortUnicastControlThresholdUnit_pps;
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
snmpAgentPortBroadcastControlModeSet  ( L7_uint32 interface, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentPortBroadcastControlMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentPortBroadcastControlMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbSwDevCtrlBcastStormModeIntfSet ( interface, temp_val);
  }

  return rc;
}

L7_RC_t
snmpAgentPortMulticastControlModeSet  ( L7_uint32 interface, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentPortMulticastControlMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentPortMulticastControlMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbSwDevCtrlMcastStormModeIntfSet ( interface, temp_val);
  }

  return rc;
}

L7_RC_t
snmpAgentPortUnicastControlModeSet  ( L7_uint32 interface, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentPortUnicastControlMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentPortUnicastControlMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbSwDevCtrlUcastStormModeIntfSet ( interface, temp_val);
  }

  return rc;
}

L7_RC_t
snmpAgentPortBroadcastControlThresholdSet  ( L7_uint32 interface, L7_int32 val )
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val, rate_min, rate_max;
  L7_RATE_UNIT_t rate_unit;

  rc = usmDbSwDevCtrlBcastStormThresholdIntfGet (interface, &temp_val, &rate_unit);

  if (rc == L7_SUCCESS)
  {
      if (rate_unit == L7_RATE_UNIT_PERCENT)
      {
         rate_min = L7_STORMCONTROL_LEVEL_MIN;
         rate_max = L7_STORMCONTROL_LEVEL_MAX;
      }
      else if (rate_unit == L7_RATE_UNIT_PPS)
      {
         rate_min = L7_STORMCONTROL_RATE_MIN;
         rate_max = L7_STORMCONTROL_RATE_MAX;
      }
      else
      {
         return L7_FAILURE;
      }
      if ((val < rate_min) || (val > rate_max))
      {
         return L7_FAILURE;
      }
  }

  rc = usmDbSwDevCtrlBcastStormThresholdIntfSet ( interface, val, rate_unit);

  return rc;
}


L7_RC_t
snmpAgentPortMulticastControlThresholdSet  ( L7_uint32 interface, L7_int32 val )
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val, rate_min, rate_max;
  L7_RATE_UNIT_t rate_unit;

  rc = usmDbSwDevCtrlMcastStormThresholdIntfGet (interface, &temp_val, &rate_unit);

  if (rc == L7_SUCCESS)
  {
      if (rate_unit == L7_RATE_UNIT_PERCENT)
      {
         rate_min = L7_STORMCONTROL_LEVEL_MIN;
         rate_max = L7_STORMCONTROL_LEVEL_MAX;
      }
      else if (rate_unit == L7_RATE_UNIT_PPS)
      {
         rate_min = L7_STORMCONTROL_RATE_MIN;
         rate_max = L7_STORMCONTROL_RATE_MAX;
      }
      else
      {
         return L7_FAILURE;
      }
      if ((val < rate_min) || (val > rate_max))
      {
         return L7_FAILURE;
      }
  }

  rc = usmDbSwDevCtrlMcastStormThresholdIntfSet ( interface, val, rate_unit);

  return rc;
}

L7_RC_t
snmpAgentPortUnicastControlThresholdSet  ( L7_uint32 interface, L7_int32 val )
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val, rate_min, rate_max;
  L7_RATE_UNIT_t rate_unit;

  rc = usmDbSwDevCtrlUcastStormThresholdIntfGet (interface, &temp_val, &rate_unit);

  if (rc == L7_SUCCESS)
  {
      if (rate_unit == L7_RATE_UNIT_PERCENT)
      {
         rate_min = L7_STORMCONTROL_LEVEL_MIN;
         rate_max = L7_STORMCONTROL_LEVEL_MAX;
      }
      else if (rate_unit == L7_RATE_UNIT_PPS)
      {
         rate_min = L7_STORMCONTROL_RATE_MIN;
         rate_max = L7_STORMCONTROL_RATE_MAX;
      }
      else
      {
         return L7_FAILURE;
      }
      if ((val < rate_min) || (val > rate_max))
      {
         return L7_FAILURE;
      }
  }

  rc = usmDbSwDevCtrlUcastStormThresholdIntfSet ( interface, val, rate_unit);

  return rc;
}

L7_RC_t
snmpAgentPortBroadcastControlThresholdUnitSet  ( L7_uint32 interface, L7_uint32 val )
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 rate=0;
  L7_uint32 temp_val;
  L7_RATE_UNIT_t rate_unit=0;
  L7_RATE_UNIT_t unit_current;

  rc = usmDbSwDevCtrlBcastStormThresholdIntfGet (interface, &temp_val, &unit_current);

  switch (val)
  {
  case D_agentPortBroadcastControlThresholdUnit_percent:
    rate_unit = L7_RATE_UNIT_PERCENT;
    break;

  case D_agentPortBroadcastControlThresholdUnit_pps:
    rate_unit = L7_RATE_UNIT_PPS;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS && rate_unit != unit_current)
  {
      rc = usmDbSwDevCtrlBcastStormThresholdIntfSet ( interface, rate, rate_unit);
  }

  return rc;
}

L7_RC_t
snmpAgentPortMulticastControlThresholdUnitSet  ( L7_uint32 interface, L7_uint32 val )
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 rate=0;
  L7_uint32 temp_val;
  L7_RATE_UNIT_t rate_unit=0;
  L7_RATE_UNIT_t unit_current;

  rc = usmDbSwDevCtrlMcastStormThresholdIntfGet (interface, &temp_val, &unit_current);

  switch (val)
  {
  case D_agentPortMulticastControlThresholdUnit_percent:
    rate_unit = L7_RATE_UNIT_PERCENT;
    break;

  case D_agentPortMulticastControlThresholdUnit_pps:
    rate_unit = L7_RATE_UNIT_PPS;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS && rate_unit != unit_current)
  {
      rc = usmDbSwDevCtrlMcastStormThresholdIntfSet ( interface, rate, rate_unit);
  }

  return rc;
}

L7_RC_t
snmpAgentPortUnicastControlThresholdUnitSet  ( L7_uint32 interface, L7_uint32 val )
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 rate=0;
  L7_uint32 temp_val;
  L7_RATE_UNIT_t rate_unit=0;
  L7_RATE_UNIT_t unit_current;

  rc = usmDbSwDevCtrlUcastStormThresholdIntfGet (interface, &temp_val, &unit_current);

  switch (val)
  {
  case D_agentPortUnicastControlThresholdUnit_percent:
    rate_unit = L7_RATE_UNIT_PERCENT;
    break;

  case D_agentPortUnicastControlThresholdUnit_pps:
    rate_unit = L7_RATE_UNIT_PPS;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS && rate_unit != unit_current)
  {
      rc = usmDbSwDevCtrlUcastStormThresholdIntfSet ( interface, rate, rate_unit);
  }

  return rc;
}


/* lvl7_@p0769 start */
#ifdef NOT_SUPPORTED
L7_RC_t
snmpAgentPortDuplexModeGet ( L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbIfSpeedGet ( UnitIndex, intIfIndex, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_PORTCTRL_PORTSPEED_AUTO_NEG:
      *val = D_agentPortDuplexMode_auto_negotiate;
      break;

    case L7_PORTCTRL_PORTSPEED_HALF_10T:
    case L7_PORTCTRL_PORTSPEED_HALF_100TX:
      *val = D_agentPortDuplexMode_half_duplex;
      break;

    case L7_PORTCTRL_PORTSPEED_FULL_10T:
    case L7_PORTCTRL_PORTSPEED_FULL_100TX:
    case L7_PORTCTRL_PORTSPEED_FULL_100FX:
    case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
    case L7_PORTCTRL_PORTSPEED_FULL_2P5FX:    /* PTin added: Speed 2.5G */
    case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
    case L7_PORTCTRL_PORTSPEED_FULL_40G_KR4:  /* PTin added: Speed 40G */
    case L7_PORTCTRL_PORTSPEED_FULL_100G_BKP: /* PTin added: Speed 100G */
      *val = D_agentPortDuplexMode_full_duplex;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}
#endif /* NOT_SUPPORTED */
/* lvl7_@p0769 end */

/* lvl7_@p1953 start */
#ifdef NOT_SUPPORTED
L7_RC_t
snmpAgentPortPhysicalStatusGet ( L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbIfSpeedStatusGet ( UnitIndex, intIfIndex, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_PORTCTRL_PORTSPEED_AUTO_NEG:
      *val = D_agentPortPhysicalStatus_auto_negotiate;
      break;

    case L7_PORTCTRL_PORTSPEED_HALF_10T:
      *val = D_agentPortPhysicalStatus_half_10;
      break;

    case L7_PORTCTRL_PORTSPEED_FULL_10T:
      *val = D_agentPortPhysicalStatus_full_10;
      break;

    case L7_PORTCTRL_PORTSPEED_HALF_100TX:
      *val = D_agentPortPhysicalStatus_half_100;
      break;

    case L7_PORTCTRL_PORTSPEED_FULL_100TX:
      *val = D_agentPortPhysicalStatus_full_100;
      break;

    case L7_PORTCTRL_PORTSPEED_FULL_100FX:
      *val = D_agentPortPhysicalStatus_full_100fx;
      break;

    case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
      *val = D_agentPortPhysicalStatus_full_1000sx;
      break;

    /* PTin NOTE (2.5G)
     * This piece of code is not compiled... so there is no point on adding any
     * 2.5G reference here.
     */

    case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
      *val = D_agentPortPhysicalStatus_full_10gsx;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}
#endif /* NOT_SUPPORTED */
/* lvl7_@p1953 end */

L7_RC_t
snmpAgentPortLinkTrapModeGet ( L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbIfLinkUpDownTrapEnableGet ( UnitIndex, intIfIndex, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentPortLinkTrapMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentPortLinkTrapMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentPortAdminModeSet ( L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentPortAdminMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentPortAdminMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbIfAdminStateSet ( UnitIndex, intIfIndex, temp_val );
  }

  return rc;
}

/* lvl7_@p1953 start */
#ifdef NOT_SUPPORTED
L7_RC_t
snmpAgentPortPhysicalModeSet ( L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_int32 val )
{
  L7_uint32 temp_val;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentPortPhysicalMode_auto_negotiate:
    temp_val = L7_PORTCTRL_PORTSPEED_AUTO_NEG;
    break;

  case D_agentPortPhysicalMode_half_100:
    temp_val = L7_PORTCTRL_PORTSPEED_HALF_100TX;
    break;

  case D_agentPortPhysicalMode_full_100:
    temp_val = L7_PORTCTRL_PORTSPEED_FULL_100TX;
    break;

  case D_agentPortPhysicalMode_half_10:
    temp_val = L7_PORTCTRL_PORTSPEED_HALF_10T;
    break;

  case D_agentPortPhysicalMode_full_10:
    temp_val = L7_PORTCTRL_PORTSPEED_FULL_10T;
    break;

  case D_agentPortPhysicalMode_full_100fx:
    temp_val = L7_PORTCTRL_PORTSPEED_FULL_100FX;
    break;

  default:
    rc = L7_FAILURE;
  }


  if (rc == L7_SUCCESS)
  {
    rc = usmDbIfSpeedSet ( UnitIndex, intIfIndex, temp_val );
  }

  return rc;
}
#endif /* NOT_SUPPORTED */
/* lvl7_@p1953 end */

L7_RC_t
snmpAgentPortDefaultTypeSet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uchar8 *buf)
{
  L7_uint32 ifSpeed = 0;
  L7_RC_t rc = L7_SUCCESS;

  if (strcmp(buf, "dot3MauType100BaseTXFD") == 0)
  {
    ifSpeed = L7_PORTCTRL_PORTSPEED_FULL_100TX;
  }
  else if (strcmp(buf, "dot3MauType100BaseTXHD") == 0)
  {
    ifSpeed = L7_PORTCTRL_PORTSPEED_HALF_100TX;
  }
  else if (strcmp(buf, "dot3MauType10BaseTFD") == 0)
  {
    ifSpeed = L7_PORTCTRL_PORTSPEED_FULL_10T;
  }
  else if (strcmp(buf, "dot3MauType10BaseTHD") == 0)
  {
    ifSpeed = L7_PORTCTRL_PORTSPEED_HALF_10T;
  }
  else if (strcmp(buf, "dot3MauType100BaseFXFD") == 0)
  {
    ifSpeed = L7_PORTCTRL_PORTSPEED_FULL_100FX;
  }



  else
  {
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
    rc = usmDbIfDefaultSpeedSet(UnitIndex, intIfIndex, ifSpeed);

  return rc;
}

L7_RC_t
snmpAgentPortAutoNegAdminStatusSet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uint32 val)
{
  L7_uint32 adminStatus;
  L7_RC_t   rc;

  if (val == D_agentPortAutoNegAdminStatus_enable)
    adminStatus = L7_ENABLE;
  else if (val == D_agentPortAutoNegAdminStatus_disable)
    adminStatus = L7_DISABLE;
  else
    return L7_FAILURE;

  if (adminStatus == L7_ENABLE)
  {
    rc = usmDbIfAutoNegoStatusCapabilitiesSet(intIfIndex, L7_PORT_NEGO_CAPABILITY_ALL);
  }
  else
  {
    rc = usmDbIfAutoNegoStatusCapabilitiesSet(intIfIndex, 0);
  }

  return rc;
}

L7_RC_t
snmpAgentPortDot3FlowControlModeSet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_int32 val )
{
  L7_uint32 temp_val;

  if (val == D_agentPortDot3FlowControlMode_enable)
    temp_val = L7_ENABLE;
  else if (val == D_agentPortDot3FlowControlMode_disable)
    temp_val = L7_DISABLE;
  else
    return L7_FAILURE;
  return usmDbIfFlowCtrlModeSet(UnitIndex, intIfIndex, temp_val);
}

/* lvl7_@p0769 start */
#ifdef NOT_SUPPORTED
L7_RC_t
snmpAgentPortDuplexModeSet ( L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_int32 val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbIfSpeedGet ( UnitIndex, intIfIndex, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (val)
    {
    case D_agentPortDuplexMode_auto_negotiate:
      temp_val = L7_PORTCTRL_PORTSPEED_AUTO_NEG;
      break;

    case D_agentPortDuplexMode_half_duplex:
      switch (temp_val)
      {
      case L7_PORTCTRL_PORTSPEED_FULL_10T:
        temp_val = L7_PORTCTRL_PORTSPEED_HALF_10T;
        break;

      case L7_PORTCTRL_PORTSPEED_FULL_100TX:
        temp_val = L7_PORTCTRL_PORTSPEED_HALF_100TX;
        break;

      case L7_PORTCTRL_PORTSPEED_HALF_10T:
      case L7_PORTCTRL_PORTSPEED_HALF_100TX:
        return rc;

      default:
        rc = L7_FAILURE;
      }
      break;

    case D_agentPortDuplexMode_full_duplex:
      switch (temp_val)
      {
      case L7_PORTCTRL_PORTSPEED_HALF_10T:
        temp_val = L7_PORTCTRL_PORTSPEED_FULL_10T;
        break;

      case L7_PORTCTRL_PORTSPEED_HALF_100TX:
        temp_val = L7_PORTCTRL_PORTSPEED_FULL_100TX;
        break;

      case L7_PORTCTRL_PORTSPEED_FULL_10T:
      case L7_PORTCTRL_PORTSPEED_FULL_100TX:
      case L7_PORTCTRL_PORTSPEED_FULL_100FX:
      case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
      case L7_PORTCTRL_PORTSPEED_FULL_2P5FX:    /* PTin added: Speed 2.5G */
      case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
      case L7_PORTCTRL_PORTSPEED_FULL_40G_KR4:  /* PTin added: Speed 40G */
      case L7_PORTCTRL_PORTSPEED_FULL_100G_BKP: /* PTin added: Speed 100G */
        return rc;

      default:
        rc = L7_FAILURE;
      }
      break;

    default:
      rc = L7_FAILURE;
    }
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbIfSpeedSet ( UnitIndex, intIfIndex, temp_val );
  }

  return rc;
}
#endif /* NOT_SUPPORTED */
/* lvl7_@p0769 end */


L7_RC_t
snmpAgentPortLinkTrapModeSet ( L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentPortLinkTrapMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentPortLinkTrapMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbIfLinkUpDownTrapEnableSet ( UnitIndex, intIfIndex, temp_val );
  }

  return rc;
}


L7_RC_t
snmpAgentPortClearStatsSet ( L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_int32 val )
{
  L7_RC_t rc = L7_FAILURE;

  switch (val)
  {
  case D_agentPortClearStats_enable:
    rc = usmDbIntfStatReset(UnitIndex, intIfIndex);
    break;

  case D_agentPortClearStats_disable:
    rc = L7_SUCCESS;
    break;

  default:
    rc = L7_FAILURE;
  }

  return rc;
}

/**************************************************************************************************************/


L7_RC_t
snmpAgentSnmpCommunityCreateSet ( L7_uint32 UnitIndex, L7_char8 *buf )
{
  L7_uint32 index, status;
  L7_RC_t rc;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];

  for (index = 0; index < L7_MAX_SNMP_COMM; index++)
  {
    rc = usmDbSnmpCommunityStatusGet(USMDB_UNIT_CURRENT, index, &status);
    if (rc == L7_SUCCESS && status != L7_SNMP_TRAP_MGR_STATUS_DELETE)
    {
      rc = usmDbSnmpCommunityNameGet(USMDB_UNIT_CURRENT, index, snmp_buffer);
      if (rc == L7_SUCCESS && strcmp(snmp_buffer, buf) == 0)
      {
        /* Community already exists */
        return L7_FAILURE;
      }
    }
  }

  for (index = 0; index < L7_MAX_SNMP_COMM; index++)
  {
    rc = usmDbSnmpCommunityStatusGet(USMDB_UNIT_CURRENT, index, &status);
    if (rc == L7_SUCCESS && status == L7_SNMP_TRAP_MGR_STATUS_DELETE)
    {
      rc = usmDbSnmpCommunityStatusSet(USMDB_UNIT_CURRENT, index, L7_SNMP_COMMUNITY_STATUS_CONFIG);
      rc = usmDbSnmpCommunityNameSet(USMDB_UNIT_CURRENT, index, buf);
      rc = usmDbSnmpCommunityIpAddrSet(USMDB_UNIT_CURRENT, index, 0);
      rc = usmDbSnmpCommunityIpMaskSet(USMDB_UNIT_CURRENT, index, 0);
      rc = usmDbSnmpCommunityAccessLevelSet(USMDB_UNIT_CURRENT, index, L7_SNMP_COMMUNITY_ACCESS_LEVEL_READ_ONLY);
      rc = usmDbSnmpCommunityOwnerSet(USMDB_UNIT_CURRENT, index, "");
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}


L7_RC_t
snmpAgentSnmpTrapReceiverCreateSet ( L7_uint32 UnitIndex, L7_char8 *buf, L7_uint32 *snmpTrapReceiverIndex)
{
  L7_uint32 index, status;
  L7_RC_t rc;

  for (index = 0; index < L7_MAX_SNMP_COMM; index++)
  {
    rc = usmDbTrapManagerStatusIpGet(USMDB_UNIT_CURRENT, index, &status);
    if (rc == L7_SUCCESS && status == L7_SNMP_TRAP_MGR_STATUS_DELETE)
    {
      rc = usmDbTrapManagerStatusIpSet(USMDB_UNIT_CURRENT, index, L7_SNMP_TRAP_MGR_STATUS_CONFIG);
      rc = usmDbTrapManagerCommIpSet(USMDB_UNIT_CURRENT, index, buf);
      rc = usmDbTrapManagerIpAddrRawSet(USMDB_UNIT_CURRENT, index, 0);
      *snmpTrapReceiverIndex = index;
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}


/**************************************************************************************************************/

L7_RC_t
snmpAgentSerialTimeoutGet ( L7_uint32 UnitIndex, L7_uint32 *val )
{
  return usmDbSerialTimeOutGet(UnitIndex, val);
}


L7_RC_t
snmpAgentSerialBaudrateGet ( L7_uint32 UnitIndex, L7_uint32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbAgentBasicConfigSerialBaudRateGet ( UnitIndex, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_BAUDRATE_1200:
      *val = D_agentSerialBaudrate_baud_1200;
      break;

    case L7_BAUDRATE_2400:
      *val = D_agentSerialBaudrate_baud_2400;
      break;

    case L7_BAUDRATE_4800:
      *val = D_agentSerialBaudrate_baud_4800;
      break;

    case L7_BAUDRATE_9600:
      *val = D_agentSerialBaudrate_baud_9600;
      break;

    case L7_BAUDRATE_19200:
      *val = D_agentSerialBaudrate_baud_19200;
      break;

    case L7_BAUDRATE_38400:
      *val = D_agentSerialBaudrate_baud_38400;
      break;

    case L7_BAUDRATE_57600:
      *val = D_agentSerialBaudrate_baud_57600;
      break;

    case L7_BAUDRATE_115200:
      *val = D_agentSerialBaudrate_baud_115200;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}


L7_RC_t
snmpAgentSerialCharacterSizeGet ( L7_uint32 UnitIndex, L7_uint32 *val )
{
  return usmDbSerialPortParmGet(UnitIndex, SYSAPISERIALPORT_CHARSIZE, val);
}


L7_RC_t
snmpAgentSerialHWFlowControlModeGet ( L7_uint32 UnitIndex, L7_uint32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbSerialPortParmGet(UnitIndex, SYSAPISERIALPORT_FLOWCONTROL, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSerialHWFlowControlMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSerialHWFlowControlMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentSerialStopBitsGet ( L7_uint32 UnitIndex, L7_uint32 *val )
{
  return usmDbSerialPortParmGet(UnitIndex, SYSAPISERIALPORT_STOPBITS, val);
}

L7_RC_t
snmpAgentSerialParityTypeGet ( L7_uint32 UnitIndex, L7_uint32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbSerialPortParmGet(UnitIndex, SYSAPISERIALPORT_PARITYTYPE, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_PARITY_EVEN:
      *val = D_agentSerialParityType_even;
      break;

    case L7_PARITY_ODD:
      *val = D_agentSerialParityType_odd;
      break;

    case L7_PARITY_NONE:
      *val = D_agentSerialParityType_none;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}


L7_RC_t
snmpAgentSerialTimeoutSet ( L7_uint32 UnitIndex, L7_uint32 val )
{
  return usmDbSerialTimeOutSet(UnitIndex, val);
}


L7_RC_t
snmpAgentSerialBaudrateSet ( L7_uint32 UnitIndex, L7_uint32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSerialBaudrate_baud_1200:
    temp_val = L7_BAUDRATE_1200;
    break;

  case D_agentSerialBaudrate_baud_2400:
    temp_val = L7_BAUDRATE_2400;
    break;

  case D_agentSerialBaudrate_baud_4800:
    temp_val = L7_BAUDRATE_4800;
    break;

  case D_agentSerialBaudrate_baud_9600:
    temp_val = L7_BAUDRATE_9600;
    break;

  case D_agentSerialBaudrate_baud_19200:
    temp_val = L7_BAUDRATE_19200;
    break;

  case D_agentSerialBaudrate_baud_38400:
    temp_val = L7_BAUDRATE_38400;
    break;

  case D_agentSerialBaudrate_baud_57600:
    temp_val = L7_BAUDRATE_57600;
    break;

  case D_agentSerialBaudrate_baud_115200:
    temp_val = L7_BAUDRATE_115200;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbAgentBasicConfigSerialBaudRateSet ( UnitIndex, temp_val );
  }

  return rc;
}

/* lvl7_@p0859 start */
#ifdef NOT_SUPPORTED
L7_RC_t
snmpAgentSerialCharacterSizeSet  ( L7_uint32 UnitIndex, L7_uint32 val )
{
  return usmDbSerialPortParmSet(UnitIndex, SYSAPISERIALPORT_CHARSIZE, val);
}


L7_RC_t
snmpAgentSerialHWFlowControlModeSet  ( L7_uint32 UnitIndex, L7_uint32 val )
{
  L7_uint32 temp_val;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSerialHWFlowControlMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSerialHWFlowControlMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbSerialPortParmSet(UnitIndex, SYSAPISERIALPORT_FLOWCONTROL, temp_val);;
  }

  return rc;
}


L7_RC_t
snmpAgentSerialStopBitsSet  ( L7_uint32 UnitIndex, L7_uint32 val )
{
  return usmDbSerialPortParmSet(UnitIndex, SYSAPISERIALPORT_STOPBITS, val);
}


L7_RC_t
snmpAgentSerialParityTypeSet  ( L7_uint32 UnitIndex, L7_uint32 val )
{
  L7_uint32 temp_val;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSerialParityType_even:
    temp_val = L7_PARITY_EVEN;
    break;

  case D_agentSerialParityType_odd:
    temp_val = L7_PARITY_ODD;
    break;

  case D_agentSerialParityType_none:
    temp_val = L7_PARITY_NONE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbSerialPortParmSet(UnitIndex, SYSAPISERIALPORT_PARITYTYPE, temp_val);
  }

  return rc;
}
#endif /* NOT_SUPPORTED */
/* lvl7_@p0859 end */


/**************************************************************************************************************/

L7_RC_t
snmpAgentSaveConfigSet ( L7_uint32 UnitIndex, L7_int32 val )
{
  switch (val)
  {
  case D_agentSaveConfig_enable:
    (void)sysapiWriteConfigToFlashStart();
    break;

  case D_agentSaveConfig_disable:
  default:
    break;
  }

  return L7_SUCCESS;
}

L7_RC_t
snmpAgentSaveConfigStatusGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;

  if (usmDbSaveConfigResultCodeGet(UnitIndex, &temp_val) == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_SAVE_CONFIG_CODE_NONE:
      *val = D_agentSaveConfigStatus_notInitiated;
      break;

    case L7_SAVE_CONFIG_CODE_STARTING:
      *val = D_agentSaveConfigStatus_savingInProcess;
      break;

    case L7_SAVE_CONFIG_CODE_SUCCESS:
      *val = D_agentSaveConfigStatus_savingComplete;
      break;

    case L7_SAVE_CONFIG_CODE_FAILURE:
      *val = D_agentSaveConfigStatus_savingFailed;
      break;

    default:
      *val = 0;
      return L7_FAILURE;
      break;
    }
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

L7_RC_t
snmpAgentClearConfigSet ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 sfs_status;

  switch (val)
  {
  case D_agentClearConfig_enable:
    /* lvl7_@p0687 start */
    /* ignore any return code, assume success */
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT,L7_FLEX_STACKING_COMPONENT_ID,
                                 L7_STACKING_SFS_FEATURE_ID) == L7_TRUE)
    {
      rc = usmDbUnitMgrStackFirmwareSyncStatusGet(&sfs_status);
      if ((rc == L7_SUCCESS) && (sfs_status == L7_SFS_SWITCH_STATUS_IN_PROGRESS))
      {
        return L7_FAILURE;
      }
    }

    usmDbResetConfigActionSet(UnitIndex, val);
    rc = L7_SUCCESS;
    /* lvl7_@p0687 end */
    break;

  case D_agentClearConfig_disable:
    rc = L7_SUCCESS;
  }

  return rc;
}


L7_RC_t
snmpAgentClearLagsSet ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 intIfNum = 0;
  L7_RC_t rc = L7_FAILURE;

  switch (val)
  {
  case D_agentClearLags_enable:
    while (usmDbDot3adAggEntryGetNext(UnitIndex, intIfNum, &intIfNum) == L7_SUCCESS)
    {
      usmDbDot3adRemoveSet(UnitIndex, intIfNum);
    }
    rc = L7_SUCCESS;
    break;

  case D_agentClearLags_disable:
    rc = L7_SUCCESS;
  }

  return rc;
}


L7_RC_t
snmpAgentClearLoginSessionsSet ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_RC_t rc = L7_FAILURE;

  switch (val)
  {
  case D_agentClearLoginSessions_enable:
    rc = usmDbLoginSessionResetConnectionAllSet(UnitIndex);
    break;

  case D_agentClearLoginSessions_disable:
    rc = L7_SUCCESS;
  }

  return rc;
}


L7_RC_t
snmpAgentClearPasswordsSet ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 tempInt;
  L7_char8 buf[1];
  L7_RC_t rc = L7_FAILURE;

  switch (val)
  {
  case D_agentClearPasswords_enable:
    tempInt = 0;
    buf[0] = L7_NULL;
    for (; tempInt < L7_MAX_LOGINS; tempInt++)
      usmDbPasswordSet(UnitIndex, tempInt, buf, L7_FALSE);
    rc = L7_SUCCESS;
    break;

  case D_agentClearPasswords_disable:
    rc = L7_SUCCESS;
  }

  return rc;
}


L7_RC_t
snmpAgentClearPortStatsSet ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 intIfIndex;

  switch (val)
  {
  case D_agentClearPortStats_enable:
    rc = usmDbValidIntIfNumFirstGet(&intIfIndex);
    while (rc == L7_SUCCESS)
    {
      rc = usmDbIntfStatReset(UnitIndex, intIfIndex);
      rc = usmDbValidIntIfNumNext(intIfIndex, &intIfIndex);
    }
    rc = L7_SUCCESS;
    break;

  case D_agentClearPortStats_disable:
    rc = L7_SUCCESS;
  }

  return rc;
}


L7_RC_t
snmpAgentClearSwitchStatsSet ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 intIfIndex;

  switch (val)
  {
  case D_agentClearSwitchStats_enable:
    /* get switch interface number and clear stats for it */
    rc = usmDbMgtSwitchintIfNumGet(UnitIndex, &intIfIndex);
    if (rc == L7_SUCCESS)
      rc = usmDbIntfStatReset(UnitIndex, intIfIndex);
    break;

  case D_agentClearPortStats_disable:
    rc = L7_SUCCESS;
  }

  return rc;
}


L7_RC_t
snmpAgentClearTrapLogSet ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_RC_t rc = L7_FAILURE;

  switch (val)
  {
  case D_agentClearTrapLog_enable:
    rc = usmDbCtrlClearTrapLogSwSet(UnitIndex);
    break;

  case D_agentClearTrapLog_disable:
    rc = L7_SUCCESS;
  }

  return rc;
}


L7_RC_t
snmpAgentClearVlanSet ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_RC_t rc = L7_FAILURE;

  switch (val)
  {
  case D_agentClearVlan_enable:
    rc = usmDbVlanResetToDefaultSet(UnitIndex, val);
    break;

  case D_agentClearVlan_disable:
    rc = L7_SUCCESS;
  }

  return rc;
}


L7_RC_t
snmpAgentResetSystemSet ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_RC_t rc = L7_FAILURE;

  switch (val)
  {
  case D_agentResetSystem_enable:
    rc = usmDbSwDevCtrlResetSet(UnitIndex, val);
    break;

  case D_agentResetSystem_disable:
    rc = L7_SUCCESS;
  }

  return rc;
}

/**************************************************************************************************************/

L7_RC_t
snmpAgentCableTesterStatusGet ( L7_uint32 *val )
{
  L7_RC_t rc;
  snmpCtStatus_t temp_status;

  rc = SnmpCableTesterStatusGet(&temp_status);

  if (rc == L7_SUCCESS)
  {
    switch (temp_status)
    {
    case SNMP_CT_STATUS_ACTIVE:
      *val = D_agentCableTesterStatus_active;
      break;

    case SNMP_CT_STATUS_FAILURE:
      *val = D_agentCableTesterStatus_failure;
      break;

    case SNMP_CT_STATUS_SUCCESS:
      *val = D_agentCableTesterStatus_success;
      break;

    case SNMP_CT_STATUS_UNINITIALIZED:
      *val = D_agentCableTesterStatus_uninitialized;
      break;

    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentCableTesterIfIndexGet ( L7_uint32 *val )
{
  L7_RC_t rc;
  L7_uint32 temp_val = 0;

  rc = SnmpCableTesterPortIndexGet(&temp_val);

  if (rc == L7_SUCCESS && temp_val != SNMP_CT_PORTINDEX_DEFAULT)
  {
    /* perform internal to external conversion */
    rc = usmDbExtIfNumFromIntIfNum(temp_val, val);
  }

  return rc;
}

L7_RC_t
snmpAgentCableTesterCableStatusGet ( L7_uint32 *val )
{
  L7_RC_t rc;
  snmpCtCableStatus_t temp_status;

  rc = SnmpCableTesterCableStatusGet(&temp_status);

  if (rc == L7_SUCCESS)
  {
    switch (temp_status)
    {
    case SNMP_CT_CABLE_STATUS_NORMAL:
      *val = D_agentCableTesterCableStatus_normal;
      break;

    case SNMP_CT_CABLE_STATUS_OPEN:
      *val = D_agentCableTesterCableStatus_open;
      break;

    case SNMP_CT_CABLE_STATUS_SHORT:
      *val = D_agentCableTesterCableStatus_short;
      break;

    case SNMP_CT_CABLE_STATUS_UNKNOWN:
      *val = D_agentCableTesterCableStatus_unknown;
      break;

    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentCableTesterStatusSet ( L7_uint32 val )
{
  L7_RC_t rc = L7_SUCCESS;
  snmpCtStatus_t temp_status = 0;

  switch (val)
  {
  case D_agentCableTesterStatus_active:
    temp_status = SNMP_CT_STATUS_ACTIVE;
    break;

  case D_agentCableTesterStatus_failure:
    temp_status = SNMP_CT_STATUS_FAILURE;
    break;

  case D_agentCableTesterStatus_success:
    temp_status = SNMP_CT_STATUS_SUCCESS;
    break;

  case D_agentCableTesterStatus_uninitialized:
    temp_status = SNMP_CT_STATUS_UNINITIALIZED;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = SnmpCableTesterStatusSet(temp_status);
  }

  return rc;
}

L7_RC_t
snmpAgentCableTesterIfIndexSet ( L7_uint32 val )
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  /* perform external to internal conversion */
  rc = usmDbIntIfNumFromExtIfNum(val, &temp_val);

  if (rc == L7_SUCCESS)
  {
    rc = SnmpCableTesterPortIndexSet(temp_val);
  }

  return rc;
}

/**************************************************************************************************************/

L7_RC_t
snmpAgentStpAdminModeGet(L7_uint32 UnitIndex, L7_int32 *adminMode)
{
  L7_BOOL temp_mode;
  L7_RC_t rc;

  rc = usmDbDot1sModeGet(UnitIndex, &temp_mode);

  if (rc == L7_SUCCESS)
  {
    switch (temp_mode)
    {
    case L7_TRUE:
      *adminMode = D_agentStpAdminMode_enable;
      break;

    case L7_FALSE:
      *adminMode = D_agentStpAdminMode_disable;
      break;

    default:
      *adminMode = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentStpAdminModeSet(L7_uint32 UnitIndex, L7_int32 adminMode)
{
  L7_BOOL temp_mode = L7_FALSE;
  L7_RC_t rc = L7_SUCCESS;

  switch (adminMode)
  {
  case D_agentStpAdminMode_enable:
    temp_mode = L7_TRUE;
    break;

  case D_agentStpAdminMode_disable:
    temp_mode = L7_FALSE;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
    rc = usmDbDot1sModeSet(UnitIndex, temp_mode);

  return rc;
}

L7_RC_t
snmpAgentStpBpduGuardModeGet(L7_uint32 UnitIndex, L7_int32 *mode)
{
  L7_BOOL temp_mode;
  L7_RC_t rc;

  rc = usmDbDot1sBpduGuardGet(UnitIndex, &temp_mode);

  if (rc == L7_SUCCESS)
  {
    switch (temp_mode)
    {
    case L7_TRUE:
      *mode = D_agentStpBpduGuardMode_enable;
      break;

    case L7_FALSE:
      *mode = D_agentStpBpduGuardMode_disable;
      break;

    default:
      *mode = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentStpBpduGuardModeSet(L7_uint32 UnitIndex, L7_int32 mode)
{
  L7_BOOL temp_mode = L7_FALSE;
  L7_RC_t rc = L7_SUCCESS;

  switch (mode)
  {
  case D_agentStpBpduGuardMode_enable:
    temp_mode = L7_TRUE;
    break;

  case D_agentStpBpduGuardMode_disable:
    temp_mode = L7_FALSE;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
    rc = usmDbDot1sBpduGuardSet(UnitIndex, temp_mode);

  return rc;
}

L7_RC_t
snmpAgentStpBpduFilterDefaultGet(L7_uint32 UnitIndex, L7_int32 *mode)
{
  L7_BOOL temp_mode;
  L7_RC_t rc;

  rc = usmDbDot1sBpduFilterGet(UnitIndex, &temp_mode);

  if (rc == L7_SUCCESS)
  {
    switch (temp_mode)
    {
    case L7_TRUE:
      *mode = D_agentStpBpduFilterDefault_enable;
      break;

    case L7_FALSE:
      *mode = D_agentStpBpduFilterDefault_disable;
      break;

    default:
      *mode = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentStpBpduFilterDefaultSet(L7_uint32 UnitIndex, L7_int32 mode)
{
  L7_BOOL temp_mode = L7_FALSE;
  L7_RC_t rc = L7_SUCCESS;

  switch (mode)
  {
  case D_agentStpBpduFilterDefault_enable:
    temp_mode = L7_TRUE;
    break;

  case D_agentStpBpduFilterDefault_disable:
    temp_mode = L7_FALSE;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
    rc = usmDbDot1sBpduFilterSet(UnitIndex, temp_mode);

  return rc;
}



L7_RC_t
snmpAgentStpConfigNameSet(L7_uint32 UnitIndex, L7_char8 *buf)
{
  L7_RC_t rc;

  /* check name for correct characters */
  if (usmDbStringAlphaNumericCheck(buf) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  rc = usmDbDot1sConfigNameSet(UnitIndex, buf);

  return rc;
}

L7_RC_t
snmpAgentStpForceVersionGet(L7_uint32 UnitIndex, L7_int32 *forceVersion)
{
  L7_uint32 temp_ver;
  L7_RC_t rc = L7_SUCCESS;

  rc = usmDbDot1sForceVersionGet(UnitIndex, &temp_ver);

  if (rc == L7_SUCCESS)
  {
    switch (temp_ver)
    {
    case DOT1S_FORCE_VERSION_DOT1D:
      *forceVersion = D_agentStpForceVersion_dot1d;
      break;

    case DOT1S_FORCE_VERSION_DOT1W:
      *forceVersion = D_agentStpForceVersion_dot1w;
      break;

    case DOT1S_FORCE_VERSION_DOT1S:
      *forceVersion = D_agentStpForceVersion_dot1s;
      break;

    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentStpForceVersionSet(L7_uint32 UnitIndex, L7_int32 forceVersion)
{
  DOT1S_FORCE_VERSION_t temp_ver = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (forceVersion)
  {
  case D_agentStpForceVersion_dot1d:
    temp_ver = DOT1S_FORCE_VERSION_DOT1D;
    break;

  case D_agentStpForceVersion_dot1w:
    temp_ver = DOT1S_FORCE_VERSION_DOT1W;
    break;

  case D_agentStpForceVersion_dot1s:
    temp_ver = DOT1S_FORCE_VERSION_DOT1S;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
    rc = usmDbDot1sForceVersionSet(UnitIndex, temp_ver);

  return rc;
}

/**************************************************************************************************************/

L7_RC_t
snmpAgentStpPortGet(L7_uint32 UnitIndex, L7_uint32 stpPort)
{
  L7_uint32 cistId = DOT1S_CIST_ID;
  L7_uint32 intIfNum;

  /* check to see if this is a visible external interface number */
  if (usmDbVisibleExtIfNumberCheck(UnitIndex, stpPort) == L7_SUCCESS)
  {
    if (usmDbIntIfNumFromExtIfNum(stpPort, &intIfNum) == L7_SUCCESS)
    {
      return usmDbDot1sMstiPortGet(UnitIndex, cistId, intIfNum);
    }
  }

  return L7_FAILURE;
}

L7_RC_t
snmpAgentStpPortNextGet(L7_uint32 UnitIndex, L7_uint32 *stpPort)
{
  L7_uint32 cistId = DOT1S_CIST_ID;
  L7_uint32 intIfNum = 0;
  L7_uint32 extIfNum = 0;

  while (usmDbDot1sPortNextGet(UnitIndex, cistId, intIfNum, &intIfNum) == L7_SUCCESS)
  {
    /* convert internal interface number to external interface number */
    if (usmDbExtIfNumFromIntIfNum(intIfNum, &extIfNum) == L7_SUCCESS)
    {
      if (extIfNum > *stpPort)
      {
        /* check to see if this is a visible external interface number */
        if (usmDbVisibleExtIfNumberCheck(UnitIndex, extIfNum) == L7_SUCCESS)
        {
          *stpPort = extIfNum;
          return L7_SUCCESS;
        }
      }
    }
  }

  return L7_FAILURE;
}

L7_RC_t
snmpAgentStpPortStateGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *portState)
{
  L7_BOOL temp_state;
  L7_RC_t rc;

  rc = usmDbDot1sPortStateGet(UnitIndex, intIfNum, &temp_state);

  if (rc == L7_SUCCESS)
  {
    switch (temp_state)
    {
    case L7_TRUE:
      *portState = D_agentStpPortState_enable;
      break;

    case L7_FALSE:
      *portState = D_agentStpPortState_disable;
      break;

    default:
      *portState = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentStpPortStateSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 portState)
{
  L7_BOOL temp_state = L7_FALSE;
  L7_RC_t rc = L7_SUCCESS;

  switch (portState)
  {
  case D_agentStpPortState_enable:
    temp_state = L7_TRUE;
    break;

  case D_agentStpPortState_disable:
    temp_state = L7_FALSE;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
    rc = usmDbDot1sPortStateSet(UnitIndex, intIfNum, temp_state);

  return rc;
}

/**************************************************************************************************************/
L7_RC_t
snmpAgentStpCstPortGet(L7_uint32 UnitIndex, L7_uint32 cstPort)
{
  L7_uint32 cistId = DOT1S_CIST_ID;
  L7_uint32 intIfNum;


  /* check to see if this is a visible external interface number */
  if (usmDbVisibleExtIfNumberCheck(UnitIndex, cstPort) == L7_SUCCESS)
  {
    if (usmDbIntIfNumFromExtIfNum(cstPort, &intIfNum) == L7_SUCCESS)
    {
      return usmDbDot1sMstiPortGet(UnitIndex, cistId, intIfNum);
    }
  }

  return L7_FAILURE;
}

L7_RC_t
snmpAgentStpCstPortNextGet(L7_uint32 UnitIndex, L7_uint32 *cstPort)
{
  L7_uint32 cistId = DOT1S_CIST_ID;
  L7_uint32 intIfNum = 0;
  L7_uint32 extIfNum = 0;

  while (usmDbDot1sPortNextGet(UnitIndex, cistId, intIfNum, &intIfNum) == L7_SUCCESS)
  {
    /* convert internal interface number to external interface number */
    if (usmDbExtIfNumFromIntIfNum(intIfNum, &extIfNum) == L7_SUCCESS)
    {
      if (extIfNum > *cstPort)
      {
        /* check to see if this is a visible external interface number */
        if (usmDbVisibleExtIfNumberCheck(UnitIndex, extIfNum) == L7_SUCCESS)
        {
          *cstPort = extIfNum;
          return L7_SUCCESS;
        }
      }
    }
  }

  return L7_FAILURE;
}

L7_RC_t
snmpAgentStpCstPortOperEdgeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbDot1sCistPortOperEdgeGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentStpCstPortOperEdge_enable;
      break;

    case L7_DISABLE:
      *val = D_agentStpCstPortOperEdge_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentStpCstPortOperPointToPointGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val)
{
  L7_BOOL temp_val;
  L7_RC_t rc;

  rc = usmDbDot1sCistPortOperPointToPointGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *val = D_agentStpCstPortOperPointToPoint_true;
      break;

    case L7_FALSE:
      *val = D_agentStpCstPortOperPointToPoint_false;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentStpCstPortTopologyChangeAckGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val)
{
  L7_BOOL temp_val;
  L7_RC_t rc;

  rc = usmDbDot1sCistPortTopologyChangeAckGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *val = D_agentStpCstPortTopologyChangeAck_true;
      break;

    case L7_FALSE:
      *val = D_agentStpCstPortTopologyChangeAck_false;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentStpCstPortEdgeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbDot1sCistPortEdgeGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentStpCstPortEdge_enable;
      break;

    case L7_DISABLE:
      *val = D_agentStpCstPortEdge_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentStpCstPortEdgeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 val)
{
  L7_BOOL temp_val = L7_FALSE;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentStpCstPortEdge_enable:
    temp_val = L7_TRUE;
    break;

  case D_agentStpCstPortEdge_disable:
    temp_val = L7_FALSE;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
    rc = usmDbDot1sCistPortEdgeSet(UnitIndex, intIfNum, temp_val);

  return rc;
}

/* BPDU Filter Effect */

L7_RC_t
snmpAgentStpCstPortBpduGuardEffectGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val)
{
  L7_BOOL temp_val;
  L7_RC_t rc;

  rc = usmDbDot1sBpduGuardEffectGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *val = D_agentStpCstPortBpduGuardEffect_enable;
      break;

    case L7_FALSE:
      *val = D_agentStpCstPortBpduGuardEffect_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

/* BPDU Filter*/
L7_RC_t
snmpAgentStpCstPortBpduFilterGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val)
{
  L7_BOOL temp_val;
  L7_RC_t rc;

  rc = usmDbDot1sIntfBpduFilterGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *val = D_agentStpCstPortBpduFilter_enable;
      break;

    case L7_FALSE:
      *val = D_agentStpCstPortBpduFilter_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentStpCstPortBpduFilterSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 val)
{
  L7_BOOL temp_val = L7_FALSE;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentStpCstPortBpduFilter_enable:
    temp_val = L7_TRUE;
    break;

  case D_agentStpCstPortBpduFilter_disable:
    temp_val = L7_FALSE;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
    rc = usmDbDot1sIntfBpduFilterSet(UnitIndex, intIfNum, temp_val);

  return rc;
}


/* BPDU Flood*/
L7_RC_t
snmpAgentStpCstPortBpduFloodGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val)
{
  L7_BOOL temp_val;
  L7_RC_t rc;

  rc = usmDbDot1sIntfBpduFloodGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *val = D_agentStpCstPortBpduFlood_enable;
      break;

    case L7_FALSE:
      *val = D_agentStpCstPortBpduFlood_disable;
      break;

    default:
      *val = D_agentStpCstPortBpduFlood_disable;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentStpCstPortBpduFloodSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 val)
{
  L7_BOOL temp_val = L7_FALSE;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentStpCstPortBpduFlood_enable:
    temp_val = L7_TRUE;
    break;

  case D_agentStpCstPortBpduFlood_disable:
    temp_val = L7_FALSE;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
    rc = usmDbDot1sIntfBpduFloodSet(UnitIndex, intIfNum, temp_val);

  return rc;
}



/* Auto Edge*/
L7_RC_t
snmpAgentStpCstPortAutoEdgeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbDot1sPortAutoEdgeGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentStpCstPortAutoEdge_enable;
      break;

    case L7_DISABLE:
      *val = D_agentStpCstPortAutoEdge_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentStpCstPortAutoEdgeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 val)
{
  L7_BOOL temp_val = L7_FALSE;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentStpCstPortAutoEdge_enable:
    temp_val = L7_TRUE;
    break;

  case D_agentStpCstPortAutoEdge_disable:
    temp_val = L7_FALSE;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
    rc = usmDbDot1sPortAutoEdgeSet(UnitIndex, intIfNum, temp_val);

  return rc;
}

/* Root Guard*/
L7_RC_t
snmpAgentStpCstPortRootGuardGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbDot1sPortRestrictedRoleGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentStpCstPortRootGuard_enable;
      break;

    case L7_DISABLE:
      *val = D_agentStpCstPortRootGuard_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentStpCstPortRootGuardSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 val)
{
  L7_BOOL temp_val = L7_FALSE;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentStpCstPortRootGuard_enable:
    temp_val = L7_TRUE;
    break;

  case D_agentStpCstPortRootGuard_disable:
    temp_val = L7_FALSE;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
    rc = usmDbDot1sPortRestrictedRoleSet(UnitIndex, intIfNum, temp_val);

  return rc;
}

/* Loop Guard*/
L7_RC_t
snmpAgentStpCstPortLoopGuardGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbDot1sPortLoopGuardGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentStpCstPortLoopGuard_enable;
      break;

    case L7_DISABLE:
      *val = D_agentStpCstPortLoopGuard_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentStpCstPortLoopGuardSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 val)
{
  L7_BOOL temp_val = L7_FALSE;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentStpCstPortLoopGuard_enable:
    temp_val = L7_TRUE;
    break;

  case D_agentStpCstPortLoopGuard_disable:
    temp_val = L7_FALSE;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
    rc = usmDbDot1sPortLoopGuardSet(UnitIndex, intIfNum, temp_val);

  return rc;
}

/* TCN Guard*/
L7_RC_t
snmpAgentStpCstPortTCNGuardGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbDot1sPortRestrictedTcnGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentStpCstPortTCNGuard_enable;
      break;

    case L7_DISABLE:
      *val = D_agentStpCstPortTCNGuard_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentStpCstPortTCNGuardSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 val)
{
  L7_BOOL temp_val = L7_FALSE;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentStpCstPortTCNGuard_enable:
    temp_val = L7_TRUE;
    break;

  case D_agentStpCstPortTCNGuard_disable:
    temp_val = L7_FALSE;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
    rc = usmDbDot1sPortRestrictedTcnSet(UnitIndex, intIfNum, temp_val);

  return rc;
}


/**************************************************************************************************************/

L7_RC_t
snmpAgentStpMstRootPortIDGet(L7_uint32 UnitIndex, L7_uint32 mstId, L7_ushort16 *val)
{
  L7_uint32 temp_port;
  L7_RC_t rc;

  rc = usmDbDot1sMstiRootPortIDGet(UnitIndex, mstId, &temp_port);
  if (rc == L7_SUCCESS)
  {
    *val = osapiHtons((L7_ushort16)temp_port);
  }

  return rc;
}

L7_RC_t
snmpAgentStpMstTopologyChangeParmGet(L7_uint32 UnitIndex, L7_uint32 mstId, L7_uint32 *parm)
{
  L7_BOOL temp_parm;
  L7_RC_t rc;

  rc = usmDbDot1sMstiTopologyChangeParmGet(UnitIndex, mstId, &temp_parm);

  if (rc == L7_SUCCESS)
  {
    switch (temp_parm)
    {
    case L7_TRUE:
      *parm = D_agentStpMstTopologyChangeParm_true;
      break;

    case L7_FALSE:
      *parm = D_agentStpMstTopologyChangeParm_false;
      break;

    default:
      *parm = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

/**************************************************************************************************************/

L7_RC_t
snmpAgentStpMstPortGet(L7_uint32 UnitIndex, L7_uint32 mstId, L7_uint32 mstPort)
{
  L7_uint32 intIfNum;

  /* check to see if this is a visible external interface number */
  if (usmDbVisibleExtIfNumberCheck(UnitIndex, mstPort) == L7_SUCCESS)
  {
    if (usmDbIntIfNumFromExtIfNum(mstPort, &intIfNum) == L7_SUCCESS)
    {
      return usmDbDot1sMstiPortGet(UnitIndex, mstId, intIfNum);
    }
  }

  return L7_FAILURE;
}

L7_RC_t
snmpAgentStpMstPortNextGet(L7_uint32 UnitIndex, L7_uint32 mstId, L7_uint32 *nextMstId, L7_uint32 *mstPort)
{
  L7_uint32 intIfNum = 0;
  L7_uint32 extIfNum = 0;
  L7_uint32 temp_mstId;

  temp_mstId = mstId;
  while (usmDbDot1sMstiPortNextGet(UnitIndex, mstId, &mstId, intIfNum, &intIfNum) == L7_SUCCESS)
  {
    if (temp_mstId < mstId)
    {
      if (usmDbExtIfNumFromIntIfNum(intIfNum, &extIfNum) == L7_SUCCESS)
      {
        /* check to see if this is a visible external interface number */
        if (usmDbVisibleExtIfNumberCheck(UnitIndex, extIfNum) == L7_SUCCESS)
        {
          *mstPort = extIfNum;
          *nextMstId = mstId;
          return L7_SUCCESS;
        }
      }
    }
    else
    {
      /* convert internal interface number to external interface number */
      if (usmDbExtIfNumFromIntIfNum(intIfNum, &extIfNum) == L7_SUCCESS)
      {
        if (extIfNum > *mstPort)
        {
          /* check to see if this is a visible external interface number */
          if (usmDbVisibleExtIfNumberCheck(UnitIndex, extIfNum) == L7_SUCCESS)
          {
            *mstPort = extIfNum;
            *nextMstId = mstId;
            return L7_SUCCESS;
          }
        }
      }
    }
  }

  return L7_FAILURE;
}

L7_RC_t
snmpAgentStpMstPortForwardingStateGet(L7_uint32 UnitIndex, L7_uint32 mstId, L7_uint32 intIfNum, L7_int32 *fwdState)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbDot1sMstiPortForwardingStateGet(UnitIndex, mstId, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_DOT1S_DISCARDING:
      *fwdState = D_agentStpMstPortForwardingState_discarding;
      break;

    case L7_DOT1S_LEARNING:
      *fwdState = D_agentStpMstPortForwardingState_learning;
      break;

    case L7_DOT1S_FORWARDING:
      *fwdState = D_agentStpMstPortForwardingState_forwarding;
      break;

    case L7_DOT1S_DISABLED:
      *fwdState = D_agentStpMstPortForwardingState_disabled;
      break;

    case L7_DOT1S_MANUAL_FWD:
      *fwdState = D_agentStpMstPortForwardingState_manualFwd;
      break;

    case L7_DOT1S_NOT_PARTICIPATE:
      *fwdState = D_agentStpMstPortForwardingState_notParticipate;
      break;

    default:
      *fwdState = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentStpMstPortIDGet(L7_uint32 UnitIndex, L7_uint32 mstId, L7_uint32 intIfNum, L7_char8 *buf, L7_uint32 bufLen)
{
  L7_uint32 temp_port;
  L7_ushort16 port;
  L7_RC_t rc;

  rc = usmDbDot1sMstiPortIDGet(UnitIndex, mstId, intIfNum, &temp_port);

  if (rc == L7_SUCCESS)
  {
    port = (L7_ushort16)temp_port;
    port = osapiHtons(port);
    memcpy(buf, (void*)&port, bufLen);
  }

  return rc;
}

L7_RC_t
snmpAgentStpMstDesignatedPortIDGet(L7_uint32 UnitIndex, L7_uint32 mstId, L7_uint32 intIfNum, L7_char8 *buf, L7_uint32 bufLen)
{
  L7_ushort16 temp_port;
  L7_RC_t rc;

  rc = usmDbDot1sMstiDesignatedPortIDGet(UnitIndex, mstId, intIfNum, &temp_port);

  if (rc == L7_SUCCESS)
  {
    temp_port = osapiHtons(temp_port);
    memcpy(buf, (void*)&temp_port, bufLen);
  }

  return rc;
}

L7_RC_t
snmpAgentStpMstPortLoopInconsistentStateGet(L7_uint32 UnitIndex, L7_uint32 mstId, L7_uint32 intIfNum, L7_int32 *loopState)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbDot1sMstiPortLoopInconsistentStateGet(UnitIndex, mstId, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
     case L7_FALSE:
       *loopState = D_agentStpMstPortLoopInconsistentState_false;
       break;

     case L7_TRUE:
       *loopState = D_agentStpMstPortLoopInconsistentState_true;
       break;

     default:
       *loopState = 0;
       rc = L7_FAILURE;
       break;
    }
  }

  return rc;
}

L7_RC_t
snmpDot1sInstanceVlanNextGet(L7_uint32 UnitIndex, L7_uint32 mstID, L7_uint32 VID,
                             L7_uint32 *nextMstID, L7_uint32 *nextVID)
{
  if (mstID == DOT1S_CIST_INDEX)
  {
    mstID++;
  }

  return usmDbDot1sInstanceVlanNextGet(UnitIndex, mstID, nextMstID, VID, nextVID);
}

/**************************************************************************************************************/

L7_RC_t
snmpAgentUserAuthenticationListGet(L7_uint32 UnitIndex, L7_uint32 userIndex, L7_char8 *buf)
{
  L7_RC_t rc;
  L7_uchar8 userName[SNMP_BUFFER_LEN];

  rc = usmDbLoginsGet(UnitIndex, userIndex, userName);

  if (rc == L7_SUCCESS)
  {
    rc = usmDbAPLUserGet(UnitIndex, userName, L7_USER_MGR_COMPONENT_ID, buf);
  }

  return rc;
}

L7_RC_t
snmpAgentUserAuthenticationListSet(L7_uint32 UnitIndex, L7_uint32 userIndex, L7_char8 *buf)
{
  L7_RC_t rc;
  L7_uchar8 userName[SNMP_BUFFER_LEN];

  rc = usmDbLoginsGet(UnitIndex, userIndex, userName);

  if (rc == L7_SUCCESS)
  {
    rc = usmDbAPLUserSet(UnitIndex, userName, L7_USER_MGR_COMPONENT_ID, buf);
  }

  return rc;
}
/* 1 based port mask, index starts at high order order bit */
#define PORTMASK_ZERO_VALUE(pm)     (memset(pm, 0,    sizeof(pm)))
#define PORTMASK_SET_ALL_VALUE(pm)  (memset(pm, 0xff, sizeof(pm)))
#define PORTMASK_SET_VALUE(n,pm)    (pm[((n)-1)/8] |=  (1 << (7-(((n)-1) % 8))))
#define PORTMASK_CLR_VALUE(n,pm)    (pm[((n)-1)/8] &= ~(1 << (7-(((n)-1) % 8))))
#define PORTMASK_GET_VALUE(n,pm)    (pm[((n)-1)/8] &   (1 << (7-(((n)-1) % 8))))
#define PORTMASK_LENGTH(n)          (((n)-1)/8 + ((((n)-1)%8) ? 1 : 0))
#define PORTMASK_MAX_INDEX(len)     ((len)*8)

L7_RC_t
snmpAgentUserPortSecurityGet(L7_uint32 UnitIndex, L7_uint32 userIndex, L7_char8 *buf, L7_uint32 *buf_len)
{
  L7_uint32 intIfNum;
  L7_BOOL temp_bool;

  *buf_len = PORTMASK_LENGTH(L7_MAX_PORT_COUNT);

  for (intIfNum = 1; intIfNum <= L7_MAX_PORT_COUNT; intIfNum++)
  {
    if (nimCheckIfNumber(intIfNum) == L7_SUCCESS)
    {
      if (usmDbPortUserAccessGetIndex(UnitIndex, intIfNum, userIndex, &temp_bool) == L7_SUCCESS &&
          temp_bool == L7_TRUE)
      {
        PORTMASK_SET_VALUE(intIfNum, buf);
      }
    }
  }
  *buf_len = PORTMASK_LENGTH(intIfNum-1);

  return L7_SUCCESS;
}

L7_RC_t
snmpAgentUserPortSecuritySet(L7_uint32 UnitIndex, L7_uint32 userIndex, L7_char8 *buf, L7_uint32 buf_len)
{
  L7_RC_t rc;
  L7_uint32 intIfNum;
  L7_BOOL temp_bool, noMoreIntf = L7_FALSE;
  L7_uchar8 userName[SNMP_BUFFER_LEN];

  rc = usmDbLoginsGet(UnitIndex, userIndex, userName);

  if (rc == L7_SUCCESS)
  {
    rc = usmDbIntIfNumTypeFirstGet(UnitIndex, USM_PHYSICAL_INTF, 0, &intIfNum);

    while (rc == L7_SUCCESS)
    {
      temp_bool = PORTMASK_GET_VALUE(intIfNum, buf) ? L7_TRUE : L7_FALSE;

      if (usmDbPortUserAccessSet(UnitIndex, intIfNum, userName, temp_bool) != L7_SUCCESS)
      {
        rc = L7_FAILURE;
        break;
      }

      if ((rc = usmDbIntIfNumTypeNextGet(UnitIndex, USM_PHYSICAL_INTF, 0,
                                         intIfNum, &intIfNum)) != L7_SUCCESS)
        noMoreIntf = L7_TRUE;
    }

    if (noMoreIntf == L7_TRUE)
      rc = L7_SUCCESS;
  }

  return rc;
}


L7_RC_t
oldsnmpAgentUserPortSecurityGet(L7_uint32 UnitIndex, L7_uint32 userIndex, L7_char8 *buf, L7_uint32 *buf_len)
{
  L7_uint32 intIfNum;
  L7_BOOL temp_bool;

  L7_uint32 i; /* index into string */
  L7_uint32 j; /* index into character */

  *buf_len = (L7_MAX_INTERFACE_COUNT / (sizeof(L7_char8) * 8)) + 1;

  for (intIfNum = 1; intIfNum < L7_MAX_INTERFACE_COUNT; intIfNum++)
  {
    i = intIfNum / (sizeof(L7_char8) * 8);
    j = ((sizeof(L7_char8) * 8) - 1) - (intIfNum % (sizeof(L7_char8) * 8));

    if (i < SNMP_BUFFER_LEN)
    {
      if (usmDbPortUserAccessGetIndex(UnitIndex, intIfNum, userIndex, &temp_bool) == L7_SUCCESS &&
          temp_bool == L7_TRUE)
      {
        buf[i] = buf[i] | (1 << j);
      }
      else
      {
        buf[i] = buf[i] & ~(1 << j);
      }
    }
  }

  return L7_SUCCESS;
}

L7_RC_t
oldsnmpAgentUserPortSecuritySet(L7_uint32 UnitIndex, L7_uint32 userIndex, L7_char8 *buf, L7_uint32 buf_len)
{
  L7_RC_t rc;
  L7_uint32 intIfNum;
  L7_BOOL temp_bool;

  L7_uint32 i; /* index into string */
  L7_uint32 j; /* index into character */

  L7_uchar8 userName[SNMP_BUFFER_LEN];

  rc = usmDbLoginsGet(UnitIndex, userIndex, userName);

  if (rc == L7_SUCCESS)
  {
    for (i = 0; i < buf_len; i++)
    {
      for (j = 1; j < (sizeof(L7_char8) * 8); j++)
      {
        intIfNum = (i * (sizeof(L7_char8) * 8)) + j;
        if (intIfNum > L7_MAX_INTERFACE_COUNT)
          return L7_FAILURE;

        temp_bool  = (buf[i] & (1 << (8-j))) ? L7_TRUE : L7_FALSE;

        if (usmDbPortUserAccessSet(UnitIndex, intIfNum, userName, temp_bool) != L7_SUCCESS)
        {
          return L7_FAILURE;
        }
      }
    }
  }

  return rc;
}


L7_RC_t
snmpAgentAuthenticationListEntryGet(L7_uint32 UnitIndex, L7_uint32 aplIndex)
{
  L7_RC_t rc;
  L7_char8 aplName[SNMP_BUFFER_LEN];

  rc = usmDbAPLListGetIndex(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplIndex, aplName);

  return rc;
}

L7_RC_t
snmpAgentAuthenticationListEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *aplIndex)
{
  L7_RC_t rc;

  /* try the next entry */
  *aplIndex += 1;

  rc = snmpAgentAuthenticationListEntryGet(UnitIndex, *aplIndex);

  return rc;
}

static L7_BOOL snmpAPLAuthMethodCheckSetIsAllowed(L7_ACCESS_LINE_t line, L7_ACCESS_LEVEL_t access, L7_char8 *aplName, L7_uint32 order, L7_USER_MGR_AUTH_METHOD_t method)
{
  L7_USER_MGR_AUTH_METHOD_t currentMethod;

  L7_uint32 nextOrder;
  L7_RC_t rc;

  /* Get the current method of the order and if it is same, return L7_SUCCESS*/
  rc = usmDbAPLAuthMethodGet(line, access, aplName, order, &currentMethod);
  if (rc != L7_SUCCESS)
  {
    return L7_FALSE;
  }

  if (method == currentMethod)
  {
    return L7_TRUE;
  }

  /* Traverse through all methods and check whether given method is allowed to configure */
  for (nextOrder = 0; nextOrder < L7_MAX_AUTH_METHODS; nextOrder++)
  {
    if (nextOrder == order)
    {
      continue;
    }

    /* Get the next method */
    rc = usmDbAPLAuthMethodGet(line, access, aplName, nextOrder, &currentMethod);
    if (rc != L7_SUCCESS)
    {
      return L7_FALSE;
    }

    /* Duplicate method check. */
    if ((method == currentMethod) && (method != L7_AUTH_METHOD_UNDEFINED))
      return L7_FALSE;

    switch (method)
    {
      case L7_AUTH_METHOD_RADIUS :
      case L7_AUTH_METHOD_TACACS :
        if (nextOrder < order)
        {
          if(currentMethod == L7_AUTH_METHOD_LOCAL || currentMethod == L7_AUTH_METHOD_REJECT || currentMethod == L7_AUTH_METHOD_ENABLE || currentMethod == L7_AUTH_METHOD_LINE ||  currentMethod == L7_AUTH_METHOD_NONE ||  currentMethod == L7_AUTH_METHOD_UNDEFINED)
            return L7_FALSE;
        }
        break;

      case L7_AUTH_METHOD_LOCAL:
      case L7_AUTH_METHOD_LINE:
      case L7_AUTH_METHOD_ENABLE:
      case L7_AUTH_METHOD_NONE:
        if (nextOrder > order)
        {
          /* Non-server based should be the last method */
          if(currentMethod != L7_AUTH_METHOD_UNDEFINED)
            return L7_FALSE;
        }
        else
        {
          if(!(currentMethod == L7_AUTH_METHOD_RADIUS || currentMethod == L7_AUTH_METHOD_TACACS))
            return L7_FALSE;
        }
        break;

      case L7_AUTH_METHOD_UNDEFINED:      
        if (nextOrder > order)
        {
          /* Non-server based should be the last method */
          if(currentMethod != L7_AUTH_METHOD_UNDEFINED)
            return L7_FALSE;
        }
        else
        {
          if(currentMethod == L7_AUTH_METHOD_UNDEFINED)
            return L7_FALSE;
        }
        break;

        default:
        break; /* Do nothing. SNMP supports above methods only */
    }
  }

  if (snmpAgentAuthListMethodModifyDependencyCheck(line, access, aplName, order, method) != L7_TRUE)
      return L7_FALSE;

  return L7_TRUE;
}
/* Do not permit to change/ remove local or enable method from login auth list which is already 
   applied to console*/
/* Do not permit to change/Remove enable method from enable auth list which is already applied 
   to console*/ 
static L7_RC_t snmpAgentAuthListMethodModifyDependencyCheck(L7_ACCESS_LINE_t line, 
                                                            L7_ACCESS_LEVEL_t access, 
                                                            L7_char8 *aplName, 
                                                            L7_uint32 order, 
                                                            L7_USER_MGR_AUTH_METHOD_t method)
{
  L7_USER_MGR_AUTH_METHOD_t currentMethod;

  L7_RC_t rc;
  L7_uchar8 currentAplName[L7_MAX_AUTHENTICATIONLIST_NAME_SIZE+1];

  /* Get the current method of the order and if it is same, return L7_SUCCESS*/
  rc = usmDbAPLAuthMethodGet(line, access, aplName, order, &currentMethod);
  if (rc != L7_SUCCESS)
  {
    return L7_FALSE;
  }

  if (method == currentMethod)
  {
    return L7_TRUE;
  }

  if (usmDbAPLLineGet(ACCESS_LINE_CONSOLE, ACCESS_LEVEL_LOGIN, currentAplName) != L7_SUCCESS) 
  {
    return L7_FALSE;
  }
  if (strcmp(currentAplName, aplName) ==0)
  {
    if (currentMethod == L7_AUTH_METHOD_LOCAL || currentMethod == L7_AUTH_METHOD_ENABLE)
    {
      if ((method != L7_AUTH_METHOD_LOCAL) && (method != L7_AUTH_METHOD_ENABLE))
      {
        return L7_FALSE;
      }
    } 
  } /* if (strcmp(currentAplName, aplName) ==0)*/  
  return L7_TRUE;
}

/************************************** CHECK API END *******************************************/

L7_RC_t
snmpAgentAuthenticationListMethod1Get(L7_uint32 UnitIndex, L7_uint32 aplIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_USER_MGR_AUTH_METHOD_t temp_val;
  L7_char8 aplName[SNMP_BUFFER_LEN];

  rc = usmDbAPLListGetIndex(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplIndex, aplName);

  if (rc == L7_SUCCESS)
  {
    rc = usmDbAPLAuthMethodGet(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 0, &temp_val);

    if (rc == L7_SUCCESS)
    {
      switch (temp_val)
      {
      case L7_AUTH_METHOD_UNDEFINED:
        *val = D_agentAuthenticationListMethod1_undefined;
        break;

      case L7_AUTH_METHOD_LOCAL:
        *val = D_agentAuthenticationListMethod1_local;
        break;

      case L7_AUTH_METHOD_RADIUS:
        *val = D_agentAuthenticationListMethod1_radius;
        break;

      case L7_AUTH_METHOD_LINE:
        *val = D_agentAuthenticationListMethod1_line;
        break;

      case L7_AUTH_METHOD_ENABLE:
        *val = D_agentAuthenticationListMethod1_enable;
        break;

      case L7_AUTH_METHOD_NONE:
        *val = D_agentAuthenticationListMethod1_none;
        break;

      case L7_AUTH_METHOD_TACACS:
        *val = D_agentAuthenticationListMethod1_tacacs;
        break;
      
#ifdef D_agentAuthenticationListMethod1_ias
      case L7_AUTH_METHOD_IAS:
        *val = D_agentAuthenticationListMethod1_ias;
        break;
#endif

      default:
        *val = 0;
        rc = L7_FAILURE;
        break;
      }
    }
  }

  return rc;
}

L7_RC_t
snmpAgentAuthenticationListMethod1Set(L7_uint32 UnitIndex, L7_uint32 aplIndex, L7_uint32 val)
{
  L7_RC_t rc;
  L7_USER_MGR_AUTH_METHOD_t temp_val = 0;
  L7_char8 aplName[SNMP_BUFFER_LEN];

  rc = usmDbAPLListGetIndex(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplIndex, aplName);

  if (rc == L7_SUCCESS)
  {
    switch (val)
    {
    case D_agentAuthenticationListMethod1_undefined:
      if (snmpAPLAuthMethodCheckSetIsAllowed(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 0, L7_AUTH_METHOD_UNDEFINED) != L7_TRUE)
        return L7_FAILURE;
      temp_val = L7_AUTH_METHOD_UNDEFINED;
      break;

    case D_agentAuthenticationListMethod1_local:
      if (snmpAPLAuthMethodCheckSetIsAllowed(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 0, L7_AUTH_METHOD_LOCAL) != L7_TRUE)
        return L7_FAILURE;
      temp_val = L7_AUTH_METHOD_LOCAL;
      break;

    case D_agentAuthenticationListMethod1_radius:
      if (snmpAPLAuthMethodCheckSetIsAllowed(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 0, L7_AUTH_METHOD_RADIUS) != L7_TRUE)
        return L7_FAILURE;
      temp_val = L7_AUTH_METHOD_RADIUS;
      break;

    case D_agentAuthenticationListMethod1_enable:
      if (snmpAPLAuthMethodCheckSetIsAllowed(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 0, L7_AUTH_METHOD_ENABLE) != L7_TRUE)
        return L7_FAILURE;
      temp_val = L7_AUTH_METHOD_ENABLE;
      break;

    case D_agentAuthenticationListMethod1_none:
      if (snmpAPLAuthMethodCheckSetIsAllowed(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 0, L7_AUTH_METHOD_NONE) != L7_TRUE)
        return L7_FAILURE;
      temp_val = L7_AUTH_METHOD_NONE;
      break;

    case D_agentAuthenticationListMethod1_line:
      if (snmpAPLAuthMethodCheckSetIsAllowed(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 0, L7_AUTH_METHOD_LINE) != L7_TRUE)
        return L7_FAILURE;
      temp_val = L7_AUTH_METHOD_LINE;
      break;

    case D_agentAuthenticationListMethod1_tacacs:
      if (snmpAPLAuthMethodCheckSetIsAllowed(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 0, L7_AUTH_METHOD_TACACS) != L7_TRUE)
        return L7_FAILURE;
      temp_val = L7_AUTH_METHOD_TACACS;
      break;

#ifdef D_agentAuthenticationListMethod1_ias
    case D_agentAuthenticationListMethod1_ias:
      temp_val = L7_AUTH_METHOD_IAS;
      break;
#endif

    default:
      rc = L7_FAILURE;
      break;
    }

    if (rc == L7_SUCCESS)
    {
      rc = usmDbAPLAuthMethodSet(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 0, temp_val);
    }
  }

  return rc;
}

L7_RC_t
snmpAgentAuthenticationListMethod2Get(L7_uint32 UnitIndex, L7_uint32 aplIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_USER_MGR_AUTH_METHOD_t temp_val;
  L7_char8 aplName[SNMP_BUFFER_LEN];

  rc = usmDbAPLListGetIndex(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplIndex, aplName);

  if (rc == L7_SUCCESS)
  {
    rc = usmDbAPLAuthMethodGet(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 1, &temp_val);

    if (rc == L7_SUCCESS)
    {
      switch (temp_val)
      {
      case L7_AUTH_METHOD_UNDEFINED:
        *val = D_agentAuthenticationListMethod2_undefined;
        break;

      case L7_AUTH_METHOD_LOCAL:
        *val = D_agentAuthenticationListMethod2_local;
        break;

      case L7_AUTH_METHOD_RADIUS:
        *val = D_agentAuthenticationListMethod2_radius;
        break;

      case L7_AUTH_METHOD_LINE:
        *val = D_agentAuthenticationListMethod2_line;
        break;

      case L7_AUTH_METHOD_ENABLE:
        *val = D_agentAuthenticationListMethod2_enable;
        break;

      case L7_AUTH_METHOD_NONE:
        *val = D_agentAuthenticationListMethod2_none;
        break;

      case L7_AUTH_METHOD_TACACS:
        *val = D_agentAuthenticationListMethod2_tacacs;
        break;

#ifdef D_agentAuthenticationListMethod2_ias      
      case L7_AUTH_METHOD_IAS:
        *val = D_agentAuthenticationListMethod2_ias;
        break;
#endif
      default:
        *val = 0;
        rc = L7_FAILURE;
        break;
      }
    }
  }

  return rc;
}

L7_RC_t
snmpAgentAuthenticationListMethod2Set(L7_uint32 UnitIndex, L7_uint32 aplIndex, L7_uint32 val)
{
  L7_RC_t rc;
  L7_USER_MGR_AUTH_METHOD_t temp_val = 0;
  L7_char8 aplName[SNMP_BUFFER_LEN];

  rc = usmDbAPLListGetIndex(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplIndex, aplName);

  if (rc == L7_SUCCESS)
  {
    switch (val)
    {
    case D_agentAuthenticationListMethod2_undefined:
      if (snmpAPLAuthMethodCheckSetIsAllowed(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 1, L7_AUTH_METHOD_UNDEFINED) != L7_TRUE)
        return L7_FAILURE;
      temp_val = L7_AUTH_METHOD_UNDEFINED;
      break;

    case D_agentAuthenticationListMethod2_local:
      if (snmpAPLAuthMethodCheckSetIsAllowed(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 1, L7_AUTH_METHOD_LOCAL) != L7_TRUE)
        return L7_FAILURE;
      temp_val = L7_AUTH_METHOD_LOCAL;
      break;

    case D_agentAuthenticationListMethod2_radius:
      if (snmpAPLAuthMethodCheckSetIsAllowed(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 1, L7_AUTH_METHOD_RADIUS) != L7_TRUE)
        return L7_FAILURE;
      temp_val = L7_AUTH_METHOD_RADIUS;
      break;

    case D_agentAuthenticationListMethod2_enable:
      if (snmpAPLAuthMethodCheckSetIsAllowed(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 1, L7_AUTH_METHOD_ENABLE) != L7_TRUE)
        return L7_FAILURE;
      temp_val = L7_AUTH_METHOD_ENABLE;
      break;

    case D_agentAuthenticationListMethod2_none:
      if (snmpAPLAuthMethodCheckSetIsAllowed(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 1, L7_AUTH_METHOD_NONE) != L7_TRUE)
        return L7_FAILURE;
      temp_val = L7_AUTH_METHOD_NONE;
      break;

    case D_agentAuthenticationListMethod2_line:
      if (snmpAPLAuthMethodCheckSetIsAllowed(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 1, L7_AUTH_METHOD_LINE) != L7_TRUE)
        return L7_FAILURE;
      temp_val = L7_AUTH_METHOD_LINE;
      break;

    case D_agentAuthenticationListMethod2_tacacs:
      if (snmpAPLAuthMethodCheckSetIsAllowed(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 1, L7_AUTH_METHOD_TACACS) != L7_TRUE)
        return L7_FAILURE;
      temp_val = L7_AUTH_METHOD_TACACS;
      break;

#ifdef D_agentAuthenticationListMethod2_ias      
    case D_agentAuthenticationListMethod2_ias:
      temp_val = L7_AUTH_METHOD_IAS;
      break;
#endif

    default:
      rc = L7_FAILURE;
      break;
    }

    if (rc == L7_SUCCESS)
    {
      rc = usmDbAPLAuthMethodSet(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 1, temp_val);
    }
  }

  return rc;
}

L7_RC_t
snmpAgentAuthenticationListMethod3Get(L7_uint32 UnitIndex, L7_uint32 aplIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_USER_MGR_AUTH_METHOD_t temp_val;
  L7_char8 aplName[SNMP_BUFFER_LEN];

  rc = usmDbAPLListGetIndex(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplIndex, aplName);

  if (rc == L7_SUCCESS)
  {
    rc = usmDbAPLAuthMethodGet(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 2, &temp_val);

    if (rc == L7_SUCCESS)
    {
      switch (temp_val)
      {
      case L7_AUTH_METHOD_UNDEFINED:
        *val = D_agentAuthenticationListMethod3_undefined;
        break;

      case L7_AUTH_METHOD_LOCAL:
        *val = D_agentAuthenticationListMethod3_local;
        break;

      case L7_AUTH_METHOD_RADIUS:
        *val = D_agentAuthenticationListMethod3_radius;
        break;

      case L7_AUTH_METHOD_TACACS:
        *val = D_agentAuthenticationListMethod3_tacacs;
        break;

#ifdef D_agentAuthenticationListMethod3_ias      
      case L7_AUTH_METHOD_IAS:
        *val = D_agentAuthenticationListMethod3_ias;
        break;
#endif

      case L7_AUTH_METHOD_LINE:
        *val = D_agentAuthenticationListMethod3_line;
        break;

      case L7_AUTH_METHOD_ENABLE:
        *val = D_agentAuthenticationListMethod3_enable;
        break;

      case L7_AUTH_METHOD_NONE:
        *val = D_agentAuthenticationListMethod3_none;
        break;

      default:
        *val = 0;
        rc = L7_FAILURE;
        break;
      }
    }
  }

  return rc;
}

L7_RC_t
snmpAgentAuthenticationListMethod3Set(L7_uint32 UnitIndex, L7_uint32 aplIndex, L7_uint32 val)
{
  L7_RC_t rc;
  L7_USER_MGR_AUTH_METHOD_t temp_val = 0;
  L7_char8 aplName[SNMP_BUFFER_LEN];

  rc = usmDbAPLListGetIndex(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplIndex, aplName);

  if (rc == L7_SUCCESS)
  {
    switch (val)
    {
    case D_agentAuthenticationListMethod3_undefined:
      if (snmpAPLAuthMethodCheckSetIsAllowed(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 2, L7_AUTH_METHOD_UNDEFINED) != L7_TRUE)
        return L7_FAILURE;
      temp_val = L7_AUTH_METHOD_UNDEFINED;
      break;

    case D_agentAuthenticationListMethod3_local:
      if (snmpAPLAuthMethodCheckSetIsAllowed(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 2, L7_AUTH_METHOD_LOCAL) != L7_TRUE)
        return L7_FAILURE;
      temp_val = L7_AUTH_METHOD_LOCAL;
      break;

    case D_agentAuthenticationListMethod3_radius:
      if (snmpAPLAuthMethodCheckSetIsAllowed(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 2, L7_AUTH_METHOD_RADIUS) != L7_TRUE)
        return L7_FAILURE;
      temp_val = L7_AUTH_METHOD_RADIUS;
      break;
    case D_agentAuthenticationListMethod3_enable:
      if (snmpAPLAuthMethodCheckSetIsAllowed(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 2, L7_AUTH_METHOD_ENABLE) != L7_TRUE)
        return L7_FAILURE;
      temp_val = L7_AUTH_METHOD_ENABLE;
      break;

    case D_agentAuthenticationListMethod3_none:
      if (snmpAPLAuthMethodCheckSetIsAllowed(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 2, L7_AUTH_METHOD_NONE) != L7_TRUE)
        return L7_FAILURE;
      temp_val = L7_AUTH_METHOD_NONE;
      break;

    case D_agentAuthenticationListMethod3_line:
      if (snmpAPLAuthMethodCheckSetIsAllowed(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 2, L7_AUTH_METHOD_LINE) != L7_TRUE)
        return L7_FAILURE;
      temp_val = L7_AUTH_METHOD_LINE;
      break;

    case D_agentAuthenticationListMethod3_tacacs:
      if (snmpAPLAuthMethodCheckSetIsAllowed(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 2, L7_AUTH_METHOD_TACACS) != L7_TRUE)
        return L7_FAILURE;
      temp_val = L7_AUTH_METHOD_TACACS;
      break;

#ifdef D_agentAuthenticationListMethod3_ias      
    case D_agentAuthenticationListMethod3_ias:
      temp_val = L7_AUTH_METHOD_IAS;
      break;
#endif

    default:
      rc = L7_FAILURE;
      break;
    }

    if (rc == L7_SUCCESS)
    {
      rc = usmDbAPLAuthMethodSet(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 2, temp_val);
    }
  }

  return rc;
}

L7_RC_t
snmpAgentAuthenticationListMethod4Get(L7_uint32 UnitIndex, L7_uint32 aplIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_USER_MGR_AUTH_METHOD_t temp_val;
  L7_char8 aplName[SNMP_BUFFER_LEN];

  rc = usmDbAPLListGetIndex(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplIndex, aplName);

  if (rc == L7_SUCCESS)
  {
    rc = usmDbAPLAuthMethodGet(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 3, &temp_val);

    if (rc == L7_SUCCESS)
    {
      switch (temp_val)
      {

      case L7_AUTH_METHOD_UNDEFINED:
        *val = D_agentAuthenticationListMethod4_undefined;
        break;
      case L7_AUTH_METHOD_LOCAL:
        *val = D_agentAuthenticationListMethod4_local;
        break;

      case L7_AUTH_METHOD_RADIUS:
        *val = D_agentAuthenticationListMethod4_radius;
        break;

      case L7_AUTH_METHOD_TACACS:
        *val = D_agentAuthenticationListMethod4_tacacs;
        break;

#ifdef D_agentAuthenticationListMethod4_ias      
      case L7_AUTH_METHOD_IAS:
        *val = D_agentAuthenticationListMethod4_ias;
        break;
#endif

      case L7_AUTH_METHOD_LINE:
        *val = D_agentAuthenticationListMethod4_line;
        break;

      case L7_AUTH_METHOD_ENABLE:
        *val = D_agentAuthenticationListMethod4_enable;
        break;

      case L7_AUTH_METHOD_NONE:
        *val = D_agentAuthenticationListMethod4_none;
        break;

      default:
        *val = 0;
        rc = L7_FAILURE;
        break;
      }
    }
  }

  return rc;
}

L7_RC_t
snmpAgentAuthenticationListMethod4Set(L7_uint32 UnitIndex, L7_uint32 aplIndex, L7_uint32 val)
{
  L7_RC_t rc;
  L7_USER_MGR_AUTH_METHOD_t temp_val = 0;
  L7_char8 aplName[SNMP_BUFFER_LEN];

  rc = usmDbAPLListGetIndex(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplIndex, aplName);

  if (rc == L7_SUCCESS)
  {
    switch (val)
    {
    case D_agentAuthenticationListMethod4_undefined:
      if (snmpAPLAuthMethodCheckSetIsAllowed(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 3, L7_AUTH_METHOD_UNDEFINED) != L7_TRUE)
        return L7_FAILURE;
      temp_val = L7_AUTH_METHOD_UNDEFINED;
      break;

    case D_agentAuthenticationListMethod4_local:
      if (snmpAPLAuthMethodCheckSetIsAllowed(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 3, L7_AUTH_METHOD_LOCAL) != L7_TRUE)
        return L7_FAILURE;
      temp_val = L7_AUTH_METHOD_LOCAL;
      break;

    case D_agentAuthenticationListMethod4_radius:
      if (snmpAPLAuthMethodCheckSetIsAllowed(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 3, L7_AUTH_METHOD_RADIUS) != L7_TRUE)
        return L7_FAILURE;
      temp_val = L7_AUTH_METHOD_RADIUS;
      break;

    case D_agentAuthenticationListMethod4_enable:
      if (snmpAPLAuthMethodCheckSetIsAllowed(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 3, L7_AUTH_METHOD_ENABLE) != L7_TRUE)
        return L7_FAILURE;
      temp_val = L7_AUTH_METHOD_ENABLE;
      break;

    case D_agentAuthenticationListMethod4_none:
      if (snmpAPLAuthMethodCheckSetIsAllowed(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 3, L7_AUTH_METHOD_NONE) != L7_TRUE)
        return L7_FAILURE;
      temp_val = L7_AUTH_METHOD_NONE;
      break;

    case D_agentAuthenticationListMethod4_line:
      if (snmpAPLAuthMethodCheckSetIsAllowed(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 3, L7_AUTH_METHOD_LINE) != L7_TRUE)
        return L7_FAILURE;
      temp_val = L7_AUTH_METHOD_LINE;

    case D_agentAuthenticationListMethod4_tacacs:
      if (snmpAPLAuthMethodCheckSetIsAllowed(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 3, L7_AUTH_METHOD_TACACS) != L7_TRUE)
        return L7_FAILURE;
      temp_val = L7_AUTH_METHOD_TACACS;
      break;

#ifdef D_agentAuthenticationListMethod4_ias      
    case D_agentAuthenticationListMethod4_ias:
      temp_val = L7_AUTH_METHOD_IAS;
      break;
#endif

    default:
      rc = L7_FAILURE;
      break;
    }

    if (rc == L7_SUCCESS)
    {
      rc = usmDbAPLAuthMethodSet(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 3, temp_val);
    }
  }

  return rc;
}

L7_RC_t
snmpAgentAuthenticationListMethod5Get(L7_uint32 UnitIndex, L7_uint32 aplIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_USER_MGR_AUTH_METHOD_t temp_val;
  L7_char8 aplName[SNMP_BUFFER_LEN];

  rc = usmDbAPLListGetIndex(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplIndex, aplName);

  if (rc == L7_SUCCESS)
  {
    rc = usmDbAPLAuthMethodGet(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 4, &temp_val);

    if (rc == L7_SUCCESS)
    {
      switch (temp_val)
      {
#ifdef D_agentAuthenticationListMethod5_undefined
      case L7_AUTH_METHOD_UNDEFINED:
        *val = D_agentAuthenticationListMethod5_undefined;
        break;
#endif

#ifdef D_agentAuthenticationListMethod5_local
      case L7_AUTH_METHOD_LOCAL:
        *val = D_agentAuthenticationListMethod5_local;
        break;
#endif

#ifdef D_agentAuthenticationListMethod5_radius
      case L7_AUTH_METHOD_RADIUS:
        *val = D_agentAuthenticationListMethod5_radius;
        break;
#endif

#ifdef D_agentAuthenticationListMethod5_reject
      case L7_AUTH_METHOD_REJECT:
        *val = D_agentAuthenticationListMethod5_reject;
        break;
#endif

#ifdef D_agentAuthenticationListMethod5_tacacs
      case L7_AUTH_METHOD_TACACS:
        *val = D_agentAuthenticationListMethod5_tacacs;
        break;
#endif

#ifdef D_agentAuthenticationListMethod5_ias      
      case L7_AUTH_METHOD_IAS:
        *val = D_agentAuthenticationListMethod5_ias;
        break;
#endif
      default:
        *val = 0;
        rc = L7_FAILURE;
        break;
      }
    }
  }

  return rc;
}

L7_RC_t
snmpAgentAuthenticationListMethod5Set(L7_uint32 UnitIndex, L7_uint32 aplIndex, L7_uint32 val)
{
  L7_RC_t rc;
  L7_USER_MGR_AUTH_METHOD_t temp_val = 0;
  L7_char8 aplName[SNMP_BUFFER_LEN];

  rc = usmDbAPLListGetIndex(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplIndex, aplName);

  if (rc == L7_SUCCESS)
  {
    switch (val)
    {
#ifdef D_agentAuthenticationListMethod5_undefined
    case D_agentAuthenticationListMethod5_undefined:
      temp_val = L7_AUTH_METHOD_UNDEFINED;
      break;
#endif

#ifdef D_agentAuthenticationListMethod5_local
    case D_agentAuthenticationListMethod5_local:
      temp_val = L7_AUTH_METHOD_LOCAL;
      break;
#endif

#ifdef D_agentAuthenticationListMethod5_radius
    case D_agentAuthenticationListMethod5_radius:
      temp_val = L7_AUTH_METHOD_RADIUS;
      break;
#endif

#ifdef D_agentAuthenticationListMethod5_reject
    case D_agentAuthenticationListMethod5_reject:
      temp_val = L7_AUTH_METHOD_REJECT;
      break;
#endif

#ifdef D_agentAuthenticationListMethod5_tacacs
    case D_agentAuthenticationListMethod5_tacacs:
      temp_val = L7_AUTH_METHOD_TACACS;
      break;
#endif

#ifdef D_agentAuthenticationListMethod5_ias      
    case D_agentAuthenticationListMethod5_ias:
      temp_val = L7_AUTH_METHOD_IAS;
      break;
#endif

    default:
      temp_val = -1;
      break;
    }

    if (temp_val > -1)
    {
      rc = usmDbAPLAuthMethodSet(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName, 4, temp_val);
    }
  }

  return rc;
}

L7_RC_t
snmpAgentAuthenticationListStatusSet(L7_uint32 UnitIndex, L7_uint32 aplIndex, L7_uint32 val)
{
  L7_RC_t rc;
  L7_char8 aplName[SNMP_BUFFER_LEN];

  rc = usmDbAPLListGetIndex(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplIndex, aplName);

  if (rc == L7_SUCCESS)
  {
    switch (val)
    {
    case D_agentAuthenticationListStatus_active:
      break;

    case D_agentAuthenticationListStatus_destroy:
      rc = usmDbAPLDelete(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, aplName);
      break;

    default:
      /* unknown or invalid value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

/**************************************************************************************************************/

L7_RC_t
snmpAgentClassOfServicePortGet(L7_uint32 UnitIndex, L7_uint32 port, L7_uint32 priority)
{
  L7_uint32 intIfNum;

  /* check to see if this is a visible external interface number */
  if (usmDbVisibleExtIfNumberCheck(UnitIndex, port) == L7_SUCCESS)
  {
    if (usmDbIntIfNumFromExtIfNum(port, &intIfNum) == L7_SUCCESS)
    {
      return usmDbDot1dTrafficClassEntryGet(UnitIndex, intIfNum, priority);
    }
  }

  return L7_FAILURE;
}

L7_RC_t
snmpAgentClassOfServicePortNextGet(L7_uint32 UnitIndex, L7_uint32 *port, L7_uint32 *priority)
{
  L7_uint32 intIfNum = 0;
  L7_uint32 extIfNum = 0;
  L7_uint32 temp_priority;

  temp_priority = *priority;

  (void) usmDbIntIfNumFromExtIfNum(*port, &intIfNum);

  while (usmDbDot1dTrafficClassEntryNextGet(UnitIndex, &intIfNum, &temp_priority) == L7_SUCCESS)
  {
    if (temp_priority < *priority)
    {
      if (usmDbExtIfNumFromIntIfNum(intIfNum, &extIfNum) == L7_SUCCESS)
      {
        /* check to see if this is a visible external interface number */
        if (usmDbVisibleExtIfNumberCheck(UnitIndex, extIfNum) == L7_SUCCESS)
        {
          *port = extIfNum;
          *priority = temp_priority;
          return L7_SUCCESS;
        }
      }
    }

    else
    {
      /* convert internal interface number to external interface number */
      if (usmDbExtIfNumFromIntIfNum(intIfNum, &extIfNum) == L7_SUCCESS)
      {
        if (extIfNum > *port)
        {
          /* check to see if this is a visible external interface number */
          if (usmDbVisibleExtIfNumberCheck(UnitIndex, extIfNum) == L7_SUCCESS)
          {
            *port = extIfNum;
            *priority = temp_priority;
            return L7_SUCCESS;
          }
        }
      }
    }
  }

  return L7_FAILURE;
}

/**************************************************************************************************************/

L7_RC_t
snmpIntfValidate(L7_uint32 UnitIndex, L7_uint32 extIfNum)
{

  L7_uint32 intIfNum;

  /* check to see if this is a visible external interface number */
  if (usmDbVisibleExtIfNumberCheck(UnitIndex, extIfNum) == L7_SUCCESS)
  {
    return usmDbIntIfNumFromExtIfNum(extIfNum, &intIfNum);
  }

  return L7_FAILURE;
}

/**************************************************************************************************************/

L7_RC_t
snmpNextIntfValidate(L7_uint32 UnitIndex, L7_uint32 extIfNum, L7_uint32 *nextExtIfNum )
{

  L7_uint32 intIfNum;


 /* check to see if this is a visible external interface number */
 if (usmDbGetNextVisibleExtIfNumber(extIfNum, nextExtIfNum) == L7_SUCCESS)
    {
      return usmDbIntIfNumFromExtIfNum(*nextExtIfNum, &intIfNum);
    }

  return L7_FAILURE;
}
/**************************************************************************************************************/

L7_RC_t
snmpSnoopIntfValidate(L7_uint32 UnitIndex, L7_uint32 extIfNum, L7_uchar8 family)
{

  L7_uint32 intIfNum;
  L7_RC_t   rc = L7_FAILURE;

  /* check to see if this is a visible external interface number */
  if (usmDbVisibleExtIfNumberCheck(UnitIndex, extIfNum) == L7_SUCCESS)
  {
    rc = usmDbIntIfNumFromExtIfNum(extIfNum, &intIfNum);
  }

  if (rc == L7_SUCCESS)
  {
    if (usmDbSnoopProtocolGet(family) == L7_TRUE)
      return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/**************************************************************************************************************/

L7_RC_t
snmpSnoopNextIntfValidate(L7_uint32 UnitIndex, L7_uint32 extIfNum, L7_uint32 *nextExtIfNum ,
                          L7_uint32 family, L7_uint32 *nextFamily)
{

  L7_uint32 intIfNum;
  L7_uchar8 tmpFamily;

  tmpFamily = (L7_uchar8)family;

  if (usmDbVisibleExtIfNumberCheck(UnitIndex, extIfNum) == L7_SUCCESS)

  {
    /* go to next valid family */
    if (usmDbSnoopProtocolNextGet(tmpFamily, &tmpFamily) == L7_TRUE)
    {
      *nextExtIfNum = extIfNum;
      *nextFamily = tmpFamily;
       return L7_SUCCESS;
    }
  }

 /* get the first family and then get the next valid interface */
 if (usmDbSnoopProtocolNextGet(0, &tmpFamily) == L7_TRUE)
 {
   *nextFamily = tmpFamily;
   /* check to see if this is a visible external interface number */
   if (usmDbGetNextVisibleExtIfNumber(extIfNum, nextExtIfNum) == L7_SUCCESS)
    {
      return usmDbIntIfNumFromExtIfNum(*nextExtIfNum, &intIfNum);
    }
 }
  *nextFamily = 0;
  return L7_FAILURE;
}
/**************************************************************************************************************/

L7_RC_t
snmpAgentSwitchSnoopingIntfAdminModeGet ( L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val,
                                          L7_uchar8 family)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbSnoopIntfModeGet(UnitIndex, intIfNum, &temp_val, family);

  if (rc == L7_SUCCESS)
  {
    switch(temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSwitchSnoopingIntfAdminMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSwitchSnoopingIntfAdminMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }
  return rc;
}

/**************************************************************************************************************/

L7_RC_t
snmpAgentSwitchSnoopingIntfAdminModeSet ( L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val,
                                          L7_uchar8 family)
{
  L7_uint32 temp_val = L7_DISABLE;
  L7_RC_t rc = L7_SUCCESS;

  switch(val)
  {
  case D_agentSwitchSnoopingIntfAdminMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSwitchSnoopingIntfAdminMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbSnoopIntfModeSet(UnitIndex, intIfNum, temp_val, family);
  }
  return rc;
}

/**************************************************************************************************************/

L7_RC_t
snmpAgentSwitchSnoopingIntfFastLeaveAdminModeGet ( L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val,
                                                   L7_uchar8 family)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbSnoopIntfFastLeaveAdminModeGet(UnitIndex, intIfNum, &temp_val, family);

  if (rc == L7_SUCCESS)
  {
    switch(temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSwitchSnoopingIntfFastLeaveAdminMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSwitchSnoopingIntfFastLeaveAdminMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }
  return rc;
}

/**************************************************************************************************************/

L7_RC_t
snmpAgentSwitchSnoopingIntfFastLeaveAdminModeSet ( L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val,
                                                   L7_uchar8 family)
{
  L7_uint32 temp_val = L7_DISABLE;
  L7_RC_t rc = L7_SUCCESS;

  switch(val)
  {
  case D_agentSwitchSnoopingIntfFastLeaveAdminMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSwitchSnoopingIntfFastLeaveAdminMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbSnoopIntfFastLeaveAdminModeSet(UnitIndex, intIfNum, temp_val, family);
  }
  return rc;
}

/**************************************************************************************************************/
L7_RC_t
snmpAgentSwitchSnoopingIntfMulticastRouterModeGet ( L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val,
                                                    L7_uchar8 family)
{
  L7_uint32 cfgIntfMode = L7_DISABLE, operIntfMode = L7_DISABLE;
  L7_BOOL mcastRtrEnabled = L7_FALSE;
  L7_uint32 vlanId;

  (void)usmDbSnoopIntfMrouterStatusGet(UnitIndex, intIfNum, &cfgIntfMode, family);
  if (cfgIntfMode == L7_ENABLE)
    *val = D_agentSwitchSnoopingIntfMulticastRouterMode_enable;
  else
  {
    mcastRtrEnabled = L7_FALSE;
    for (vlanId = 1 ; vlanId <= L7_DOT1Q_MAX_VLAN_ID ; vlanId++)
    {
      operIntfMode = L7_DISABLE;
      (void)usmDbSnoopIntfMrouterGet(UnitIndex, intIfNum, vlanId, &operIntfMode, family);
      if (operIntfMode == L7_ENABLE)
      {
        mcastRtrEnabled = L7_TRUE;
        break;
      }
    }
    switch(mcastRtrEnabled)
    {
    case L7_TRUE:
      *val = D_agentSwitchSnoopingIntfMulticastRouterMode_enable;
      break;
    case L7_FALSE:
      *val = D_agentSwitchSnoopingIntfMulticastRouterMode_disable;
      break;
    }
  }
  return L7_SUCCESS;
}

/**************************************************************************************************************/

L7_RC_t
snmpAgentSwitchSnoopingIntfMulticastRouterModeSet ( L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val,
                                                    L7_uchar8 family)
{
  L7_uint32 temp_val = L7_DISABLE;
  L7_RC_t rc = L7_SUCCESS;

  switch(val)
  {
  case D_agentSwitchSnoopingIntfMulticastRouterMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSwitchSnoopingIntfMulticastRouterMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbSnoopIntfMrouterSet(UnitIndex, intIfNum, temp_val, family);
  }
  return rc;
}

/**************************************************************************************************************/

L7_RC_t
snmpAgentSwitchSnoopingIntfVlanIDsGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                          L7_uchar8 *VlanIDList, L7_uint32 *VlanIDList_len)
{
  L7_uint32 i, j, largestVlan = 0, numOfVlans = 0;
  static dot1qVidList_t list[L7_MAX_VLANS+1];

  /* Get a list of VLANs an interface is a member of */
  if (usmDbVlanVIDListGet(UnitIndex, intIfNum, (L7_uint32 *)list, &numOfVlans) != L7_SUCCESS)
    return L7_FAILURE;

  memset(VlanIDList, 0, *VlanIDList_len);

  /* By calling the above usmDb call, we get an array of VlanIds,
     which we need to convert to a bitmask */
  for (i = 0; i < numOfVlans; i++)
  {
    /* Keep track of the largest Vlan in the buffer */
    if (largestVlan < list[i].vid)
    {
      largestVlan = list[i].vid;

      /* Check and see if this will fall outside the buffer */
      if (largestVlan > *VlanIDList_len*8)
        return L7_FAILURE;
    }

    if (list[i].vid > 0)
    {
      j = ((list[i].vid-1)/8);
      VlanIDList[j] |= (1 << (8-(list[i].vid%8)));
    }
  }

  *VlanIDList_len = ((largestVlan-1)/8)+1;

  return L7_SUCCESS;
}

/*********************************************************************/


/**************************************************************************************************************/

L7_RC_t snmpSnoopVlanGet(L7_uint32 unit, L7_uint32 vlanId, L7_uint32 family)
{
 L7_uchar8 tmpFamily;

 tmpFamily = (L7_uchar8)family;

 if((usmDbSnoopVlanCheckValid(vlanId) != L7_SUCCESS) ||
    (usmDbSnoopProtocolGet(tmpFamily) == L7_FALSE)
    )
   return L7_FAILURE;
 else
   return L7_SUCCESS;
}

L7_RC_t snmpSnoopVlanGetNext(L7_uint32 unit, L7_uint32 *vlanId,
                             L7_uint32 family, L7_uint32 *nextFamily)
{
  L7_uchar8 tmpFamily = '\0';

  tmpFamily = (L7_uchar8)family;
  /*if first time call*/
  if (L7_NULL == *vlanId)
  {
    if (usmDbSnoopVlanNextGet(*vlanId, vlanId) != L7_SUCCESS) 
    {
      *vlanId     = L7_NULL;
      *nextFamily = L7_NULL;
      return L7_FAILURE;
  }

  if (usmDbSnoopProtocolNextGet(L7_NULL, &tmpFamily) != L7_TRUE)
  {
    *nextFamily = L7_NULL;
    return L7_FAILURE;
  }

  *nextFamily = tmpFamily;
  }
  else
  {
    if (usmDbSnoopProtocolNextGet(tmpFamily, &tmpFamily) != L7_TRUE)
    {
      if (usmDbSnoopProtocolNextGet(L7_NULL, &tmpFamily) != L7_TRUE)
      {
        *nextFamily = L7_NULL;
        return L7_FAILURE;
      }

      if (usmDbSnoopVlanNextGet(*vlanId, vlanId) != L7_SUCCESS) 
      {
        *vlanId     = L7_NULL;
        *nextFamily = L7_NULL;
        return L7_FAILURE;
      }
    }
  }

  *nextFamily = tmpFamily;
  return L7_SUCCESS;
}

L7_RC_t snmpIntfVlanValidate(L7_uint32 unit, L7_uint32 extIfNum, L7_uint32 vlanId,
                             L7_uchar8 family)
{
  L7_uint32 intIfNum;

  /* check to see if this is a valid routing interface */
  if (usmDbIntIfNumFromExtIfNum(extIfNum, &intIfNum) == L7_SUCCESS &&
      usmDbPhysicalIntIfNumberCheck(unit, intIfNum) == L7_SUCCESS &&
      usmDbSnoopVlanCheckValid(vlanId) == L7_SUCCESS)
  {
    if (usmDbSnoopProtocolGet(family) == L7_TRUE)
    {
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

L7_RC_t snmpIntfVlanNextGet(L7_uint32 unit, L7_uint32 *extIfNum, L7_uint32 *vlanId,
                            L7_uchar8 *family)
{

  if (usmDbSnoopProtocolNextGet(*family, family) != L7_TRUE)
  {
    if (usmDbSnoopProtocolNextGet(0, family) != L7_TRUE)
    {
      return L7_FAILURE;
    }
  }
  else if (snmpIntfVlanValidate(unit, *extIfNum, *vlanId, *family)
                                == L7_SUCCESS)
  {
    return L7_SUCCESS;
  }

  if (usmDbSnoopVlanNextGet(*vlanId, vlanId) != L7_SUCCESS)
  {
     if (usmDbSnoopVlanNextGet(L7_NULL, vlanId) != L7_SUCCESS)
     {
       return L7_SUCCESS;
     }
  }
  else if (snmpIntfVlanValidate(unit, *extIfNum, *vlanId, *family)
                           == L7_SUCCESS)
  {
    return L7_SUCCESS;
  }

 while (nimGetNextExtIfNumber(*extIfNum, extIfNum) == L7_SUCCESS)
  {
    /* check to see if this is a valid routing interface */
    if (snmpIntfVlanValidate(unit, *extIfNum, *vlanId, *family) == L7_SUCCESS)
       {
         return L7_SUCCESS;
       }
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Obtain the Static Mrouter Admin Mode
*
* @param    unit                      Unit Id
* @param    intIfNum                  Exernal Interface Number
* @param    vlanId                    Vlan Id
* @param    mode                      Static Mrouter Admin Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes   none
* @end
*********************************************************************/
L7_RC_t snmpAgentSwitchVlanStaticMrouterAdminModeGet(L7_uint32 unit, L7_uint32 intIfNum,
                                                     L7_uint32 vlanId, L7_uint32 *mode,
                                                     L7_uchar8 family)
{
  L7_uchar8 cfgVlanMode = L7_DISABLE;
  L7_uint32 operVlanMode = L7_DISABLE;

  (void) usmDbsnoopIntfApiVlanStaticMcastRtrGet(unit, intIfNum, vlanId, &cfgVlanMode, family);
  (void) usmDbSnoopIntfMrouterGet(unit, intIfNum, vlanId, &operVlanMode, family);

  if ((cfgVlanMode == L7_ENABLE) || (operVlanMode == L7_ENABLE))
    *mode = D_agentSwitchVlanStaticMrouterAdminMode_enable;
  else
    *mode = D_agentSwitchVlanStaticMrouterAdminMode_disable;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the Static Mrouter Admin Mode
*
* @param    unit                      Unit Id
* @param    intIfNum                  Exernal Interface Number
* @param    vlanId                    Vlan Id
* @param    mode                      Static Mrouter Admin Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes   none
* @end
*********************************************************************/
L7_RC_t snmpAgentSwitchVlanStaticMrouterAdminModeSet(L7_uint32 unit, L7_uint32 intIfNum, L7_uint32 vlanId,
                                                         L7_uint32 mode, L7_uchar8 family)
{
  L7_RC_t rc = L7_SUCCESS;

  L7_uchar8 temp_val = 0;

  switch (mode)
    {
    case D_agentSwitchVlanStaticMrouterAdminMode_enable:
      temp_val = L7_ENABLE;
      break;

    case D_agentSwitchVlanStaticMrouterAdminMode_disable:
      temp_val = L7_DISABLE;
      break;

    default:
      rc = L7_FAILURE;
    }


  if (rc == L7_SUCCESS)
    {
      rc = usmDbsnoopIntfApiVlanStaticMcastRtrSet(unit, intIfNum, vlanId, temp_val, family);
    }

  return(rc);
}

/*********************************************************************
* @purpose  Get the  Snooping Multicast Router Expiry Time for a VLAN
*
* @param    unit                      Unit Id
* @param    vlanId                    Vlan Id
* @param    mcrtExpiryTime            The Mcast Router Expiry Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes   none
* @end
*********************************************************************/
L7_RC_t snmpSnoopVlanMcastRtrExpiryTimeGet(L7_uint32 unit, L7_uint32 vlanId, L7_ushort16 *mcrtExpiryTime,
                                           L7_uchar8 family)
{
  return usmDbSnoopVlanMcastRtrExpiryTimeGet(unit, vlanId, mcrtExpiryTime, family);
}

/*********************************************************************
* @purpose  Set the  Snooping Multicast Router Expiry Time for a VLAN
*
* @param    unit                      Unit Id
* @param    vlanId                    Vlan Id
* @param    mcrtExpiryTime            The Mcast Router Expiry Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes   none
* @end
*********************************************************************/
L7_RC_t snmpSnoopVlanMcastRtrExpiryTimeSet(L7_uint32 unit, L7_uint32 vlanId,
                                           L7_ushort16 mcrtExpiryTime, L7_uchar8 family)
{
  return usmDbSnoopVlanMcastRtrExpiryTimeSet(unit, vlanId, mcrtExpiryTime, family);
}

#ifdef L7_DHCP_SNOOPING_PACKAGE

L7_RC_t snmpDhcpSnoopingAdminModeGet(L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL temp_val;
  rc  = usmDbDsAdminModeGet(&temp_val);
  if (L7_SUCCESS == rc)
  {
    switch (temp_val)
    {
      case L7_TRUE:
        *val = D_agentDhcpSnoopingAdminMode_true;
        rc = L7_SUCCESS;
        break;
      case L7_FALSE:
        *val = D_agentDhcpSnoopingAdminMode_false;
        rc = L7_SUCCESS;
        break;
      default:
        rc = L7_FAILURE;
        break;
    }
  }

  return rc;
}
L7_RC_t snmpDhcpSnoopingVerifyMacGet(L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL temp_val;
  rc  = usmDbDsVerifyMacGet(&temp_val);
  if (L7_SUCCESS == rc)
  {
    switch (temp_val)
    {
      case L7_TRUE:
        *val = D_agentDhcpSnoopingVerifyMac_true;
        rc = L7_SUCCESS;
        break;
      case L7_FALSE:
        *val = D_agentDhcpSnoopingVerifyMac_false;
        rc = L7_SUCCESS;
        break;
      default:
        rc = L7_FAILURE;
        break;
    }
  }

  return rc;
}

L7_RC_t snmpDhcpSnoopingAdminModeSet(L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL temp_val = L7_FALSE;
  switch(val)
  {
    case D_agentDhcpSnoopingAdminMode_true:
      temp_val = L7_TRUE;
      rc = L7_SUCCESS;
      break;
    case D_agentDhcpSnoopingAdminMode_false:
      temp_val = L7_FALSE;
      rc = L7_SUCCESS;
      break;
    default:
      rc = L7_FAILURE;
      return rc;
      break;
  }

  rc = usmDbDsAdminModeSet(temp_val);
  return rc;
}

L7_RC_t snmpDhcpSnoopingVerifyMacSet(L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL temp_val = L7_FALSE;
  switch(val)
  {
    case D_agentDhcpSnoopingVerifyMac_true:
      temp_val = L7_TRUE;
      rc = L7_SUCCESS;
      break;
    case D_agentDhcpSnoopingVerifyMac_false:
      temp_val = L7_FALSE;
      rc = L7_SUCCESS;
      break;
    default:
      rc = L7_FAILURE;
      return rc;
      break;
  }

  rc = usmDbDsVerifyMacSet(temp_val);
  return rc;
}

L7_RC_t snmpDhcpSnoopingVlanGet(L7_uint32 unit, L7_uint32 vlanId)
{

 if((vlanId < L7_DOT1Q_MIN_VLAN_ID) ||
    (vlanId > L7_DOT1Q_MAX_VLAN_ID))
   return L7_FAILURE;
 else
   return L7_SUCCESS;
}

L7_RC_t snmpDhcpSnoopingVlanNextGet(L7_uint32 unit, L7_uint32 *vlanId)
{

  if ( ( (*vlanId+1) < L7_DOT1Q_MIN_VLAN_ID) ||
       ( (*vlanId+1) > L7_DOT1Q_MAX_VLAN_ID)
     )
   return L7_FAILURE;

  *vlanId = *vlanId +1;

  return L7_SUCCESS;

}

L7_RC_t snmpDhcpSnoopingVlanEnableGet(L7_uint32 vlanId, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val;
  rc  = usmDbDsVlanConfigGet(vlanId, &temp_val);
  if (L7_SUCCESS == rc)
  {
    switch (temp_val)
    {
      case L7_ENABLE:
        *val = D_agentDhcpSnoopingVlanEnable_true;
              rc = L7_SUCCESS;
              break;
      case L7_DISABLE:
              *val = D_agentDhcpSnoopingVlanEnable_false;
              rc = L7_SUCCESS;
        break;
      default:
              rc = L7_FAILURE;
        break;
    }
  }

  return rc;
}

L7_RC_t snmpDhcpSnoopingVlanEnableSet(L7_uint32 vlanId, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val = L7_DISABLE;
  switch(val)
  {
    case D_agentDhcpSnoopingVlanEnable_true:
      temp_val = L7_ENABLE;
      rc = L7_SUCCESS;
      break;
    case D_agentDhcpSnoopingVlanEnable_false:
      temp_val = L7_DISABLE;
      rc = L7_SUCCESS;
      break;
    default:
      rc = L7_FAILURE;
      return rc;
      break;
  }

  rc = usmDbDsVlanConfigSet(vlanId,vlanId,temp_val);
  return rc;
}

L7_RC_t
snmpDhcpSnoopingIntfGet(L7_uint32 extIfNum)
{
  L7_uint32 intIfNum;

  if (usmDbIntIfNumFromExtIfNum(extIfNum, &intIfNum) == L7_SUCCESS)
  {
     if (usmDbDsIntfIsValid(intIfNum) == L7_TRUE)
        return L7_SUCCESS;
     else
        return L7_FAILURE;
  }

  return L7_FAILURE;
}

L7_RC_t
snmpDhcpSnoopingIntfNextGet(L7_uint32 extIfNum, L7_uint32 *nextExtIfNum)
{
  L7_uint32 intIfNum;

  /* check to see if this is a visible external interface number */
  while (usmDbGetNextVisibleExtIfNumber(extIfNum, nextExtIfNum) == L7_SUCCESS)
  {
    if (usmDbIntIfNumFromExtIfNum(*nextExtIfNum, &intIfNum) == L7_SUCCESS)
    {
      if (usmDbDsIntfIsValid(intIfNum) == L7_TRUE)
        return L7_SUCCESS;
      else
        extIfNum = *nextExtIfNum;
    }
  }
  return L7_FAILURE;
}


L7_RC_t snmpDhcpSnoopingPortTrustModeGet(L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val;
  rc  = usmDbDsIntfTrustGet(intIfNum, &temp_val);
  if (L7_SUCCESS == rc)
  {
    switch (temp_val)
    {
      case L7_ENABLE:
        *val = D_agentDhcpSnoopingIfTrustEnable_true;
         rc = L7_SUCCESS;
         break;
      case L7_DISABLE:
        *val = D_agentDhcpSnoopingIfTrustEnable_false;
         rc = L7_SUCCESS;
        break;
      default:
        rc = L7_FAILURE;
        break;
    }
  }

  return rc;
}

L7_RC_t snmpDhcpSnoopingPortLogModeGet(L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val;
  rc  = usmDbDsIntfLogInvalidGet(intIfNum, &temp_val);
  if (L7_SUCCESS == rc)
  {
    switch (temp_val)
    {
      case L7_ENABLE:
        *val = D_agentDhcpSnoopingIfLogEnable_true;
              rc = L7_SUCCESS;
              break;
      case L7_DISABLE:
              *val = D_agentDhcpSnoopingIfLogEnable_false;
              rc = L7_SUCCESS;
        break;
      default:
              rc = L7_FAILURE;
        break;
    }
  }

  return rc;
}

L7_RC_t snmpDhcpSnoopingPortTrustModeSet(L7_uint32 intIfNum, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val = L7_DISABLE;

  switch(val)
  {
    case D_agentDhcpSnoopingIfTrustEnable_true:
      temp_val = L7_ENABLE;
      rc = L7_SUCCESS;
      break;
    case D_agentDhcpSnoopingIfTrustEnable_false:
      temp_val = L7_DISABLE;
      rc = L7_SUCCESS;
      break;
    default:
      rc = L7_FAILURE;
      return rc;
      break;
  }

  rc = usmDbDsIntfTrustSet(intIfNum,temp_val);
  return rc;
}

L7_RC_t snmpDhcpSnoopingPortLogSet(L7_uint32 intIfNum, L7_uint32 val)
{

  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val = L7_DISABLE;

  switch(val)
  {
    case D_agentDhcpSnoopingIfLogEnable_true:
      temp_val = L7_ENABLE;
      rc = L7_SUCCESS;
      break;
    case D_agentDhcpSnoopingIfLogEnable_false:
      temp_val = L7_DISABLE;
      rc = L7_SUCCESS;
      break;
    default:
      rc = L7_FAILURE;
      return rc;
      break;
  }

  rc = usmDbDsIntfLogInvalidSet(intIfNum,temp_val);
  return rc;
}

#ifdef L7_IPSG_PACKAGE

L7_RC_t snmpIpsgIfVerifySourceGet(L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_BOOL verifyIp, verifyMac;
  L7_RC_t rc = usmDbIpsgVerifySourceGet(intIfNum, &verifyIp, &verifyMac);
  if (L7_SUCCESS == rc)
  {
    *val = verifyIp ? D_agentIpsgIfVerifySource_true : D_agentIpsgIfVerifySource_false;
  }
  return rc;
}

L7_RC_t snmpIpsgIfPortSecurityGet(L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_BOOL verifyIp, verifyMac;
  L7_RC_t rc = usmDbIpsgVerifySourceGet(intIfNum, &verifyIp, &verifyMac);
  if (L7_SUCCESS == rc)
  {
    *val = verifyMac ? D_agentIpsgIfPortSecurity_true : D_agentIpsgIfPortSecurity_false;
  }
  return rc;
}

/*********************************************************************
* @purpose  Enable or disable IPSG on an interface.
*
* @param    intIfNum       @b((input)) internal interface number
* @param    ipsgAdminMode  @b((input)) whether IPSG verifies the source 
*                                      IP address on this interface
*
* @returns  L7_SUCCESS
*           L7_NOT_SUPPORTED, if IPSG is not supported on this 
*                             type of interface
*           L7_FAILURE, other failures
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snmpIpsgIfVerifySourceSet(L7_uint32 intIfNum, L7_uint32 ipsgAdminMode)
{
  L7_BOOL verifyIp;

  verifyIp = (ipsgAdminMode == D_agentIpsgIfVerifySource_true) ? L7_TRUE : L7_FALSE;
  
  return usmDbIpsgVerifySourceSet(intIfNum, verifyIp, L7_FALSE);
}

/*********************************************************************
* @purpose  Enable or disable IPSG port security on an interface.
*
* @param    intIfNum   @b((input)) internal interface number
* @param    psAdminMode         @b((input)) whether IPSG verifies the source 
*                                   IP address on this interface
*
* @returns  L7_SUCCESS
*           L7_NOT_SUPPORTED, if IPSG is not supported on this 
*                             type of interface
*           L7_DEPENDENCY_NOT_MET, if attempting to enable IPSG port security
*                                  without enabling IPSG
*           L7_REQUEST_DENIED, if attempting to disable IPSG port security
*                              while leaving IPSG enabled
*           L7_FAILURE, other failures
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snmpIpsgIfPortSecuritySet(L7_uint32 intIfNum, L7_uint32 psAdminMode)
{
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL verifyIp, verifyMac;

  /* Get current value for IPSG admin mode */
  rc = usmDbIpsgVerifySourceGet(intIfNum, &verifyIp, &verifyMac);
  if (rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  verifyMac = (psAdminMode == D_agentIpsgIfPortSecurity_true) ? L7_TRUE : L7_FALSE;
  
  return usmDbIpsgVerifySourceSet(intIfNum, verifyIp, verifyMac);
}

L7_RC_t snmpStaticIpsgBindingRowStatusSet(L7_uint32 ifIndex,
                                          L7_uint32 vlanId,
                                          L7_char8 *mac_buffer,
                                          L7_uint32  ipAddr,
                                          L7_uint32 rowStatus)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_enetMacAddr_t macAddr;
  L7_uint32 entryType;
  L7_ushort16 vlanIndex=vlanId;


  snmpConvertStringToMac ( mac_buffer,macAddr.addr);

  switch(rowStatus)
  {
  case D_agentStaticIpsgBindingRowStatus_active:
    rc = usmDbIpsgBindingGet(&ifIndex,&vlanIndex,&ipAddr,&macAddr, &entryType);
    if (entryType == IPSG_ENTRY_DYNAMIC)
    rc = L7_FAILURE;
    break;

  case D_agentStaticIpsgBindingRowStatus_createAndGo:
    rc = usmDbIpsgStaticEntryAdd(ifIndex,vlanIndex,&macAddr,ipAddr);
    break;
  case D_agentStaticIpsgBindingRowStatus_destroy:
    rc = usmDbIpsgStaticEntryRemove(ifIndex,vlanIndex,&macAddr,ipAddr);
    break;

  default:
    rc = L7_FAILURE;
  }

  return rc;
}
#endif

L7_RC_t snmpStaticDsBindingRowStatusSet(  L7_char8 *mac_buffer,
                                          L7_uint32  ipAddr,
                                          L7_uint32 vlanId,
                                          L7_uint32 ifIndex,
                                          L7_uint32 rowStatus)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_enetMacAddr_t macAddr;
  dhcpSnoopBinding_t binding;


  snmpConvertStringToMac ( mac_buffer,macAddr.addr);
  memcpy(&binding.macAddr, &macAddr, L7_MAC_ADDR_LEN);

  switch(rowStatus)
  {
  case D_agentStaticDsBindingRowStatus_active:
    rc = usmDbDsBindingGetByType(&binding,DS_BINDING_STATIC);
    break;

  case D_agentStaticDsBindingRowStatus_createAndGo:
    rc = usmDbDsStaticBindingAdd(&macAddr, ipAddr, vlanId, 0 /* PTin modified: DHCP */, ifIndex);
    break;
  case D_agentStaticDsBindingRowStatus_destroy:
    rc = usmDbDsStaticBindingRemove(&macAddr);
    break;

  default:
    rc = L7_FAILURE;
  }

  return rc;
}
#ifdef L7_DHCP_L2_RELAY_PACKAGE
/********************  DHCP L2 Relay *******************************/

#ifdef I_agentDhcpL2RelayAdminMode
L7_RC_t snmpDhcpL2RelayAdminModeGet(L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val;
  rc  = usmDbDsL2RelayAdminModeGet(&temp_val);
  if (L7_SUCCESS == rc)
  {
    switch (temp_val)
    {
      case L7_ENABLE:
        *val = D_agentDhcpL2RelayAdminMode_true;
        rc = L7_SUCCESS;
        break;
      case L7_DISABLE:
        *val = D_agentDhcpL2RelayAdminMode_false;
        rc = L7_SUCCESS;
        break;
      default:
        rc = L7_FAILURE;
        break;
    }
  }

  return rc;
}

L7_RC_t snmpDhcpL2RelayAdminModeSet(L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val = L7_FALSE;
  switch(val)
  {
    case D_agentDhcpL2RelayAdminMode_true:
      temp_val = L7_ENABLE;
      rc = L7_SUCCESS;
      break;
    case D_agentDhcpL2RelayAdminMode_false:
      temp_val = L7_DISABLE;
      rc = L7_SUCCESS;
      break;
    default:
      rc = L7_FAILURE;
      return rc;
      break;
  }

  rc = usmDbDsL2RelayAdminModeSet(temp_val);
  return rc;
}
#endif

L7_RC_t snmpDhcpL2RelayIntfStatsClear()
{
  L7_uint32 intfIndex;
  for (intfIndex = 1; intfIndex < DS_MAX_INTF_COUNT; intfIndex++)
  {
    if (usmDbDsL2RelayIntfStatsClear(intfIndex) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}
L7_RC_t
snmpDhcpL2RelayIntfGet(L7_uint32 extIfNum)
{
  L7_uint32 intIfNum;

  if (usmDbIntIfNumFromExtIfNum(extIfNum, &intIfNum) == L7_SUCCESS)
  {
     if (usmDbDsIntfIsValid(intIfNum) == L7_TRUE)
        return L7_SUCCESS;
     else
        return L7_FAILURE;
  }

  return L7_FAILURE;
}

L7_RC_t
snmpDhcpL2RelayIntfNextGet(L7_uint32 extIfNum, L7_uint32 *nextExtIfNum)
{
  L7_uint32 intIfNum;

  /* check to see if this is a visible external interface number */
  while (usmDbGetNextVisibleExtIfNumber(extIfNum, nextExtIfNum) == L7_SUCCESS)
  {
    if (usmDbIntIfNumFromExtIfNum(*nextExtIfNum, &intIfNum) == L7_SUCCESS)
    {
      if (usmDbDsIntfIsValid(intIfNum) == L7_TRUE)
        return L7_SUCCESS;
      else
        extIfNum = *nextExtIfNum;
    }
  }
  return L7_FAILURE;
}

#ifdef I_agentDhcpL2RelayIfEnable
L7_RC_t snmpDhcpL2RelayPortModeGet(L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val;
  rc  = usmDbDsL2RelayIntfModeGet(intIfNum, &temp_val);
  if (L7_SUCCESS == rc)
  {
    switch (temp_val)
    {
      case L7_ENABLE:
        *val = D_agentDhcpL2RelayIfEnable_true;
              rc = L7_SUCCESS;
              break;
      case L7_DISABLE:
              *val = D_agentDhcpL2RelayIfEnable_false;
              rc = L7_SUCCESS;
        break;
      default:
              rc = L7_FAILURE;
        break;
    }
  }
  return rc;
}

L7_RC_t snmpDhcpL2RelayPortModeSet(L7_uint32 extIfNum, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val = L7_FALSE;

  switch(val)
  {
    case D_agentDhcpL2RelayIfEnable_true:
      temp_val = L7_ENABLE;
      rc = L7_SUCCESS;
      break;
    case D_agentDhcpL2RelayIfEnable_false:
      temp_val = L7_DISABLE;
      rc = L7_SUCCESS;
      break;
    default:
      rc = L7_FAILURE;
      return rc;
      break;
  }
  rc = usmDbDsL2RelayIntfModeSet(extIfNum, temp_val);
  return rc;
}
#endif


#ifdef I_agentDhcpL2RelayIfTrustEnable
L7_RC_t snmpDhcpL2RelayPortTrustModeGet(L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL temp_val;

  rc  = usmDbDsL2RelayIntfTrustGet(intIfNum, &temp_val);
  if (L7_SUCCESS == rc)
  {
    switch (temp_val)
    {
      case L7_TRUE:
        *val = D_agentDhcpL2RelayIfTrustEnable_true;
         rc = L7_SUCCESS;
         break;
      case L7_FALSE:
        *val = D_agentDhcpL2RelayIfTrustEnable_false;
         rc = L7_SUCCESS;
        break;
      default:
        rc = L7_FAILURE;
        break;
    }
  }
  return rc;
}

L7_RC_t snmpDhcpL2RelayPortTrustModeSet(L7_uint32 extIfNum, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL temp_val = L7_FALSE;

  switch(val)
  {
    case D_agentDhcpL2RelayIfTrustEnable_true:
      temp_val = L7_TRUE;
      rc = L7_SUCCESS;
      break;
    case D_agentDhcpL2RelayIfTrustEnable_false:
      temp_val = L7_FALSE;
      rc = L7_SUCCESS;
      break;
    default:
      rc = L7_FAILURE;
      return rc;
      break;
  }
  rc = usmDbDsL2RelayIntfTrustSet(extIfNum, temp_val);
  return rc;
}
#endif


L7_RC_t snmpDhcpL2RelayVlanGet(L7_uint32 unit, L7_uint32 vlanId)
{

 if((vlanId < L7_DOT1Q_MIN_VLAN_ID) ||
    (vlanId > L7_DOT1Q_MAX_VLAN_ID))
   return L7_FAILURE;
 else
   return L7_SUCCESS;
}

L7_RC_t snmpDhcpL2RelayVlanNextGet(L7_uint32 unit, L7_uint32 *vlanId)
{

  if ( ( (*vlanId+1) < L7_DOT1Q_MIN_VLAN_ID) ||
       ( (*vlanId+1) > L7_DOT1Q_MAX_VLAN_ID)
     )
   return L7_FAILURE;

  *vlanId = *vlanId +1;

  return L7_SUCCESS;

}
#ifdef I_agentDhcpL2RelayVlanEnable
L7_RC_t snmpDhcpL2RelayVlanEnableGet(L7_uint32 vlanNum, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val;
  rc  = usmDbDsL2RelayVlanModeGet(vlanNum, &temp_val);
  if (L7_SUCCESS == rc)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentDhcpL2RelayVlanEnable_true;
      rc = L7_SUCCESS;
      break;
    case L7_DISABLE:
      *val = D_agentDhcpL2RelayVlanEnable_false;
      rc = L7_SUCCESS;
      break;
    default:
      rc = L7_FAILURE;
      break;
    }
  }
  return rc;
}

L7_RC_t snmpDhcpL2RelayVlanEnableSet(L7_uint32 vlanNum, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val = L7_FALSE;

  switch(val)
  {
    case D_agentDhcpL2RelayVlanEnable_true:
      temp_val = L7_ENABLE;
      rc = L7_SUCCESS;
      break;
    case D_agentDhcpL2RelayVlanEnable_false:
      temp_val = L7_DISABLE;
      rc = L7_SUCCESS;
      break;
    default:
      rc = L7_FAILURE;
      return rc;
      break;
  }

  rc = usmDbDsL2RelayVlanModeSet(vlanNum, vlanNum, temp_val);
  return rc;
}
#endif


#ifdef I_agentDhcpL2RelayCircuitIdVlanEnable
L7_RC_t snmpDhcpL2RelayCIdVlanEnableGet(L7_uint32 vlanNum, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val;
  rc  = usmDbDsL2RelayCircuitIdGet(vlanNum, &temp_val);

  if (L7_SUCCESS == rc)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentDhcpL2RelayCircuitIdVlanEnable_true;
      rc = L7_SUCCESS;
      break;
    case L7_DISABLE:
      *val = D_agentDhcpL2RelayCircuitIdVlanEnable_false;
      rc = L7_SUCCESS;
      break;
    default:
      rc = L7_FAILURE;
      break;
    }
  }
  return rc;
}

L7_RC_t snmpDhcpL2RelayCIdVlanEnableSet(L7_uint32 vlanNum, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val = L7_FALSE;

  switch(val)
  {
    case D_agentDhcpL2RelayCircuitIdVlanEnable_true:
      temp_val = L7_ENABLE;
      rc = L7_SUCCESS;
      break;
    case D_agentDhcpL2RelayCircuitIdVlanEnable_false:
      temp_val = L7_DISABLE;
      rc = L7_SUCCESS;
      break;
    default:
      rc = L7_FAILURE;
      return rc;
      break;
  }

  rc = usmDbDsL2RelayCircuitIdSet(vlanNum, vlanNum, temp_val);

  return rc;
}
#endif

#ifdef I_agentDhcpL2RelayRemoteIdVlanEnable
L7_RC_t snmpDhcpL2RelayRIdVlanEnableGet(L7_uint32 vlanNum, OctetString **os_ptr)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8  snmpBuff[SNMP_BUFFER_LEN];

  bzero(snmpBuff, SNMP_BUFFER_LEN);
  rc  = usmDbDsL2RelayRemoteIdGet(vlanNum, snmpBuff);
  if (L7_SUCCESS == rc)
  {
    if (SafeMakeOctetStringFromTextExact(os_ptr, snmpBuff) == L7_TRUE)
    {
      return L7_SUCCESS;
    }
  }
  return rc;
}

L7_RC_t snmpDhcpL2RelayRIdVlanEnableSet(L7_uint32 vlanNum, OctetString *os_ptr)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8  snmpBuff[SNMP_BUFFER_LEN], eosStr = L7_EOS;

  bzero(snmpBuff, SNMP_BUFFER_LEN);

  memcpy(snmpBuff, os_ptr->octet_ptr, os_ptr->length);

  if (osapiStrncmp(snmpBuff, &eosStr, sizeof(L7_uchar8)) == L7_NULL)
  {
    return L7_FAILURE;
  }
  rc  = usmDbDsL2RelayRemoteIdSet(vlanNum, vlanNum, snmpBuff);

  if (L7_SUCCESS == rc)
  {
    return L7_SUCCESS;
  }
  return rc;
}
#endif

#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
L7_RC_t snmpDhcpL2RelaySubscriptionEntryGet(L7_uint32 ifIndex, OctetString **os_ptr)
{
  L7_uchar8  snmpBuff[L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX+1];

  bzero(snmpBuff, L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX);
  memcpy(snmpBuff, (*os_ptr)->octet_ptr, (*os_ptr)->length);

  if (usmdbDsL2RelayIntfSubscriptionGet(ifIndex, snmpBuff) == L7_SUCCESS)
  {
    memcpy((*os_ptr)->octet_ptr, snmpBuff, strlen(snmpBuff));
    (*os_ptr)->length = strlen(snmpBuff);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}
L7_RC_t snmpDhcpL2RelaySubscriptionEntryNextGet(L7_uint32 *ifIndex, OctetString **os_ptr)
{
  L7_uchar8  snmpBuff[L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX+1];

  bzero(snmpBuff, L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX);
  memcpy(snmpBuff, (*os_ptr)->octet_ptr, (*os_ptr)->length);
  if (usmdbDsL2RelayIntfSubscriptionNextGet(ifIndex, snmpBuff) == L7_SUCCESS)
  {
    memcpy((*os_ptr)->octet_ptr, snmpBuff, strlen(snmpBuff));
    (*os_ptr)->length = strlen(snmpBuff);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}
L7_RC_t snmpDhcpL2RelaySubscriptionModeGet(L7_uint32 ifIndex, L7_uchar8 *subscName,
                                           L7_uint32 *mode)
{
  return usmdbDsL2RelaySubscripionModeGet(ifIndex, subscName, mode);

}
L7_RC_t snmpDhcpL2RelaySubscriptionModeSet(L7_uint32 ifIndex, L7_uchar8 *subscName,
                                           L7_uint32 mode)
{
  return usmdbDsL2RelaySubscripionModeSet(ifIndex, subscName, mode);

}
L7_RC_t snmpDhcpL2RelayCircuitIdSubscriptionModeGet(L7_uint32 ifIndex, L7_uchar8 *subscName,
                                           L7_uint32 *mode)
{
  return usmdbDsL2RelaySubscripionCircuitIdModeGet(ifIndex, subscName, mode);

}
L7_RC_t snmpDhcpL2RelayCircuitIdSubscriptionModeSet(L7_uint32 ifIndex, L7_uchar8 *subscName,
                                           L7_uint32 mode)
{
  return usmdbDsL2RelaySubscripionCircuitIdModeSet(ifIndex, subscName, mode);

}
L7_RC_t snmpDhcpL2RelayRemoteIdSubscriptionModeGet(L7_uint32 ifIndex, L7_uchar8 *subscName,
                                                   OctetString **os_ptr)
{
  L7_uchar8  snmpBuff[DS_MAX_REMOTE_ID_STRING];

  bzero(snmpBuff, DS_MAX_REMOTE_ID_STRING);

  if (usmdbDsL2RelaySubscripionRemoteIdModeGet(ifIndex, subscName, snmpBuff) == L7_SUCCESS)
  {
    if (SafeMakeOctetStringFromTextExact(os_ptr, snmpBuff) == L7_TRUE)
    {
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}
L7_RC_t snmpDhcpL2RelayRemoteIdSubscriptionModeSet(L7_uint32 ifIndex, L7_uchar8 *subscName,
                                           OctetString *os_ptr)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8  snmpBuff[DS_MAX_REMOTE_ID_STRING], eosStr = L7_EOS;

  bzero(snmpBuff, DS_MAX_REMOTE_ID_STRING);

  osapiStrncpy(snmpBuff, os_ptr->octet_ptr, os_ptr->length);

  if (osapiStrncmp(snmpBuff, &eosStr, sizeof(L7_uchar8)) == L7_NULL)
  {
    return L7_FAILURE;
  }
  rc  = usmdbDsL2RelaySubscripionRemoteIdModeSet(ifIndex, subscName, snmpBuff);

  if (L7_SUCCESS == rc)
  {
    return L7_SUCCESS;
  }
  return rc;
}
#endif /* Metro && Dot1ad package.*/
#endif /* L7_DHCP_L2_RELAY_PACKAGE */
#endif /* L7_DHCP_SNOOPING_PACKAGE */
/*********************************************************************
* @purpose  Convert a MAC address to a string.
*
* @param    macAddr    @b((input)) MAC address to be converted
* @param    macAddrStr @b((output)) In string format
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL snmpConvertMacToString(L7_uchar8 *mac, L7_uchar8 *macBuf)
{
  int val = 0;

  if(macBuf == NULL || mac == NULL)
  {
    return L7_FALSE;
  }

  val = sprintf(macBuf,"%02x:%02x:%02x:%02x:%02x:%02x",mac[0],mac[1],
               mac[2],mac[3],mac[4],mac[5]);
  macBuf[17]='\0';
  if(val < 17)
  {
    return L7_FALSE; /* On error reading */
  }
  return L7_TRUE;
}


/**************************************************************************************************************/

#ifdef I_agentPortSwitchportMode
L7_RC_t
snmpAgentPortSwitchportModeGet (L7_uint32 unit, L7_uint32 interface, L7_uint32 *val )
{
  L7_RC_t rc;
  L7_uint32 tempVal = 0;

  /* - Need to make sure the usmdb API to use in this condition */
  rc = usmDbDot1qSwPortModeGet(unit, interface, &tempVal );
  if (rc == L7_SUCCESS)
  {
    switch (tempVal)
    {
      case DOT1Q_SWPORT_MODE_GENERAL:
        *val = D_agentPortSwitchportMode_general;
        break;
      case DOT1Q_SWPORT_MODE_ACCESS:
        *val = D_agentPortSwitchportMode_access;
        break;
      case DOT1Q_SWPORT_MODE_TRUNK:
        *val = D_agentPortSwitchportMode_trunk;
        break;
      default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Set the Voice VLAN Admin Mode for the interface
*
* @param    unit           Unit Id
* @param    interface      interface
* @param    val            Voice VLAN Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes   none
* @end
*********************************************************************/
L7_RC_t snmpAgentPortSwitchportModeSet( L7_uint32 UnitIndex, L7_uint32 interface, L7_int32 val )
{
  L7_RC_t rc = L7_FAILURE;
  L7_int32 swportMode;
  NIM_INTF_MASK_t intfMask;

  memset(&intfMask, 0, sizeof(intfMask));

  NIM_INTF_SETMASKBIT(intfMask,interface);
  switch (val)
  {
    case D_agentPortSwitchportMode_general:
      swportMode = DOT1Q_SWPORT_MODE_GENERAL;
      break;
    case D_agentPortSwitchportMode_access:
      swportMode = DOT1Q_SWPORT_MODE_ACCESS;
      break;
    case D_agentPortSwitchportMode_trunk:
      swportMode = DOT1Q_SWPORT_MODE_TRUNK;
      break;
    default:
    /* unknown value */
      return L7_FAILURE;
  }

  rc = usmDbDot1qSwPortModeSet(UnitIndex, &intfMask, swportMode);

  return rc;
}
#endif

#ifdef I_agentSwitchVoiceVLANAdminMode
/*********************************************************************
* @purpose  Get the Voice VLAN Admin Mode for the switch
*
* @param    unit           Unit Id
* @param    val            Voice VLAN Admin Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes   none
* @end
*********************************************************************/
L7_RC_t snmpAgentSwitchVoiceVLANAdminModeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmdbVoiceVlanAdminModeGet(UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch(temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSwitchVoiceVLANAdminMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSwitchVoiceVLANAdminMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }
  return rc;
}

/*********************************************************************
* @purpose  Set the Voice VLAN Admin Mode for the switch
*
* @param    unit           Unit Id
* @param    val            Voice VLAN Admin Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes   none
* @end
*********************************************************************/
L7_RC_t snmpAgentSwitchVoiceVLANAdminModeSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch(val)
  {
  case D_agentSwitchVoiceVLANAdminMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSwitchVoiceVLANAdminMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmdbVoiceVlanAdminModeSet(UnitIndex, temp_val);
  }
  return rc;
}
#endif /* I_agentSwitchVoiceVLANAdminMode */

/*#ifdef I_agentSwitchVoiceVlanDeviceEntry*/
/*********************************************************************
* @purpose  Get the Voice VLAN Device Entry for the given interface
*           and mac address
*
* @param    unit           Unit Id
* @param    intIfNum       Interface Number
* @param    mac_addr       device mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes   none
* @end
*********************************************************************/
L7_RC_t snmpAgentSwitchVoiceVlanDeviceEntryGet ( L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uchar8 *macAddress )
{

  L7_RC_t rc1= L7_FAILURE;
  L7_uchar8 deviceMacAddr[L7_MAC_ADDR_LEN];

  if(voiceVlanDeviceDebugFlag== L7_TRUE)
    printf("\n 1. %s: Interface : %d, first Mac_address:%2x:%2x:%2x:%2x:%2x:%2x",__FUNCTION__,intIfNum,
                                                              macAddress[0],
                                                              macAddress[1],
                                                              macAddress[2],
                                                              macAddress[3],
                                                              macAddress[4],
                                                              macAddress[5]);
  memset(deviceMacAddr,0,sizeof(deviceMacAddr));
  memcpy(deviceMacAddr,macAddress,sizeof(deviceMacAddr));

  if (usmDbVisibleExtIfNumberCheck(UnitIndex, intIfNum) == L7_SUCCESS)

  {
     if (usmdbVoiceVlanPortDeviceInfoFirstGet(UnitIndex,intIfNum,deviceMacAddr) == L7_SUCCESS )
     {
       if (memcmp(deviceMacAddr,macAddress,sizeof(deviceMacAddr))== 0)
       {
         if(voiceVlanDeviceDebugFlag== L7_TRUE)
          printf("\n %s: Returning success",__FUNCTION__);
          return L7_SUCCESS;
       }
     }
     else
     {
       rc1=usmdbVoiceVlanPortDeviceInfoNextGet(UnitIndex,intIfNum,deviceMacAddr);
        while(rc1==L7_SUCCESS)
        {
          if (memcmp(deviceMacAddr,macAddress,sizeof(deviceMacAddr))== 0)
            {
              if(voiceVlanDeviceDebugFlag== L7_TRUE)
                printf("\n %s: Returning success",__FUNCTION__);
              return L7_SUCCESS;
            }
          rc1 = usmdbVoiceVlanPortDeviceInfoNextGet(UnitIndex,intIfNum,deviceMacAddr);
        }
     }
  }

  if(voiceVlanDeviceDebugFlag== L7_TRUE)
    printf("\n %s: Returning L7_FAILURE",__FUNCTION__);
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the next Voice VLAN Device Entry for the given interface
* @param    unit           Unit Id
* @param    intIfNum       Interface Number
* @param    mac_addr       device mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes   none
* @end
*********************************************************************/
L7_RC_t snmpAgentSwitchVoiceVlanDeviceEntryNextGet ( L7_uint32 UnitIndex, L7_uint32 *intIfNum, L7_uchar8 *macAddress,L7_BOOL first_time )
{
  L7_RC_t rc1= L7_FAILURE;
  L7_uchar8 deviceMacAddr[L7_MAC_ADDR_LEN],tmpMacAddr[L7_MAC_ADDR_LEN];
  L7_uint32 interface=0,retVal,nextInterface;
  static L7_uint32 oldIntf;
  static L7_uchar8 oldMacAddr[L7_MAC_ADDR_LEN];
  /*static L7_BOOL first_time = L7_TRUE;*/

  if(voiceVlanDeviceDebugFlag== L7_TRUE)
    printf("\n 1. %s: Interface : %d, first Mac_address:%2x:%2x:%2x:%2x:%2x:%2x",__FUNCTION__,*intIfNum,
                                                              macAddress[0],
                                                              macAddress[1],
                                                              macAddress[2],
                                                              macAddress[3],
                                                              macAddress[4],
                                                              macAddress[5]);

  if (first_time == L7_TRUE)
  {
    oldIntf =0;
    memset(oldMacAddr,0,L7_MAC_ADDR_LEN);
    interface = 1;
    first_time = L7_FALSE;
  }
  else
  {
      interface = oldIntf;
  }

    while (interface != 0)
    {
      if(voiceVlanDeviceDebugFlag== L7_TRUE)
        printf("\n 1. %s: Interface : %d, Mac_address:%2x:%2x:%2x:%2x:%2x:%2x",__FUNCTION__,oldIntf,
                                                              oldMacAddr[0],
                                                              oldMacAddr[1],
                                                              oldMacAddr[2],
                                                              oldMacAddr[3],
                                                              oldMacAddr[4],
                                                              oldMacAddr[5]);
      if (usmDbVisibleInterfaceCheck(UnitIndex, interface, &retVal) == L7_SUCCESS)
      {
          memset(deviceMacAddr,0,L7_MAC_ADDR_LEN);
          rc1= usmdbVoiceVlanPortDeviceInfoFirstGet(UnitIndex,interface,deviceMacAddr);

          if(voiceVlanDeviceDebugFlag== L7_TRUE)
            printf("\n 3. %s: Interface : %d, first Mac_address:%2x:%2x:%2x:%2x:%2x:%2x",__FUNCTION__,interface,
                                                              deviceMacAddr[0],
                                                              deviceMacAddr[1],
                                                              deviceMacAddr[2],
                                                              deviceMacAddr[3],
                                                              deviceMacAddr[4],
                                                              deviceMacAddr[5]);
          if (rc1==L7_SUCCESS)
          {
             memset(tmpMacAddr,0,L7_MAC_ADDR_LEN);

             if(memcmp(oldMacAddr,tmpMacAddr,sizeof(tmpMacAddr))== 0)
             {
                *intIfNum = interface;
                memcpy(macAddress,deviceMacAddr,sizeof(deviceMacAddr));
                oldIntf =interface;
                memcpy(oldMacAddr,deviceMacAddr,sizeof(deviceMacAddr));
                if(voiceVlanDeviceDebugFlag== L7_TRUE)
                  printf("\n 4. %s:REturning  Interface : %d, first Mac_address:%2x:%2x:%2x:%2x:%2x:%2x",__FUNCTION__,oldIntf,
                                                              oldMacAddr[0],
                                                              oldMacAddr[1],
                                                              oldMacAddr[2],
                                                              oldMacAddr[3],
                                                              oldMacAddr[4],
                                                              oldMacAddr[5]);
                return L7_SUCCESS;
             }
             else
             {
                /*find the next device address*/

                 while(rc1==L7_SUCCESS)
                 {
                   if (memcmp(oldMacAddr,deviceMacAddr,sizeof(deviceMacAddr))==0)
                   {
                     rc1 = usmdbVoiceVlanPortDeviceInfoNextGet(UnitIndex,interface,deviceMacAddr);
                     if (rc1 == L7_SUCCESS)
                     {
                       *intIfNum = interface;
                       memcpy(macAddress,deviceMacAddr,sizeof(deviceMacAddr));
                       oldIntf =interface;
                       memcpy(oldMacAddr,deviceMacAddr,sizeof(deviceMacAddr));
                       /* printf("\n 6. %s:Returning  Interface : %d, first Mac_address:%2x:%2x:%2x:%2x:%2x",__FUNCTION__,*intIfNum,
                                                              macAddress[0],
                                                              macAddress[1],
                                                              macAddress[2],
                                                              macAddress[3],
                                                              macAddress[4],
                                                              macAddress[5]);*/
                       return L7_SUCCESS;
                     }
                   }

                   rc1 = usmdbVoiceVlanPortDeviceInfoNextGet(UnitIndex,interface,deviceMacAddr);
                 }/*while DeviceInfoNextGet = success*/
               }

             }



      }
      if ( usmDbValidIntIfNumNext(interface, &nextInterface) == L7_SUCCESS )
      {
              interface = nextInterface;
              memset(oldMacAddr,0,L7_MAC_ADDR_LEN);
      }
      else
      {
             oldIntf = interface;
             interface = 0;
      }
    }/*while interface !=0*/

    if(voiceVlanDeviceDebugFlag== L7_TRUE)
     printf("\n %s: returning L7_FAILURE",__FUNCTION__);
    *intIfNum =0;
    memset(macAddress,0,sizeof(deviceMacAddr));
    /*first_time = L7_TRUE;*/
    return L7_FAILURE;

}
/*#endif */
#ifdef I_agentPortVoiceVlanMode
/*********************************************************************
* @purpose  Get the Voice VLAN Admin Mode for the interface
*
* @param    unit           Unit Id
* @param    interface      interface
* @param    val            Voice VLAN Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes   none
* @end
*********************************************************************/
L7_RC_t snmpAgentPortVoiceVlanModeGet( L7_uint32 UnitIndex, L7_uint32 interface, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmdbVoiceVlanPortAdminModeGet ( UnitIndex, interface, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case VOICE_VLAN_CONFIG_VLAN_ID:
      *val = D_agentPortVoiceVlanMode_vlanid;
      break;

    case VOICE_VLAN_CONFIG_DOT1P:
      *val = D_agentPortVoiceVlanMode_dot1p;
      break;

    case VOICE_VLAN_CONFIG_NONE:
      *val = D_agentPortVoiceVlanMode_none;
      break;

    case VOICE_VLAN_CONFIG_UNTAGGED:
      *val = D_agentPortVoiceVlanMode_untagged;
      break;

    case VOICE_VLAN_CONFIG_DISABLE:
      *val = D_agentPortVoiceVlanMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Set the Voice VLAN Admin Mode for the interface
*
* @param    unit           Unit Id
* @param    interface      interface
* @param    val            Voice VLAN Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes   none
* @end
*********************************************************************/
L7_RC_t snmpAgentPortVoiceVlanModeSet( L7_uint32 UnitIndex, L7_uint32 interface, L7_int32 val )
{
  L7_uint32 type_val = 0;
  VOICE_VLAN_CONFIG_TYPE_t type;
  L7_RC_t rc;

  switch (val)
  {
    case D_agentPortVoiceVlanMode_vlanid:
      type = VOICE_VLAN_CONFIG_VLAN_ID;
      type_val = FD_VOICE_VLAN_VLAN_ID;
      break;

    case D_agentPortVoiceVlanMode_dot1p:
      type = VOICE_VLAN_CONFIG_DOT1P;
      type_val = FD_VOICE_VLAN_PRIORITY;
      break;

    case D_agentPortVoiceVlanMode_none:
      type = VOICE_VLAN_CONFIG_NONE;
      break;

    case D_agentPortVoiceVlanMode_untagged:
      type = VOICE_VLAN_CONFIG_UNTAGGED;
      break;

    case D_agentPortVoiceVlanMode_disable:
      type = VOICE_VLAN_CONFIG_DISABLE;
      break;

    default:
      return L7_FAILURE;
  }

  rc = usmdbvoiceVlanPortAdminModeSet ( UnitIndex, interface, type, type_val );

  return rc;
}
#endif /* I_agentPortVoiceVlanMode */

#ifdef I_agentPortVoiceVlanID
/*********************************************************************
* @purpose  Get the Voice VLAN ID for the interface
*
* @param    unit           Unit Id
* @param    interface      interface
* @param    val            Voice VLAN ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes   none
* @end
*********************************************************************/
L7_RC_t snmpAgentPortVoiceVlanIDGet( L7_uint32 UnitIndex, L7_uint32 interface, L7_int32 *val )
{
  return (usmdbVoiceVlanPortVlanIdGet ( UnitIndex, interface, val ));
}

/*********************************************************************
* @purpose  Set the Voice VLAN ID for the interface
*
* @param    unit           Unit Id
* @param    interface      interface
* @param    val            Voice VLAN ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes   none
* @end
*********************************************************************/
L7_RC_t snmpAgentPortVoiceVlanIDSet( L7_uint32 UnitIndex, L7_uint32 interface, L7_int32 val )
{
  return ( usmdbVoiceVlanPortVlanIdSet ( UnitIndex, interface, val ));
}
#endif /* I_agentPortVoiceVlanID */

#ifdef I_agentPortVoiceVlanPriority
/*********************************************************************
* @purpose  Get the Voice VLAN Priority for the interface
*
* @param    unit           Unit Id
* @param    interface      interface
* @param    val            Voice VLAN Priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes   none
* @end
*********************************************************************/
L7_RC_t snmpAgentPortVoiceVlanPriorityGet( L7_uint32 UnitIndex, L7_uint32 interface, L7_int32 *val )
{
  return (usmdbVoiceVlanPortVlanPriorityGet ( UnitIndex, interface, val ));
}

/*********************************************************************
* @purpose  Set the Voice VLAN Priority for the interface
*
* @param    unit           Unit Id
* @param    interface      interface
* @param    val            Voice VLAN Priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes   none
* @end
*********************************************************************/
L7_RC_t snmpAgentPortVoiceVlanPrioritySet( L7_uint32 UnitIndex, L7_uint32 interface, L7_int32 val )
{
  L7_uint32 mode;
  L7_RC_t rc;

  /* Voice mode can be set on on port in general mode */
  rc = usmDbDot1qSwPortModeGet(UnitIndex, interface, &mode);
  if (rc == L7_SUCCESS && mode != DOT1Q_SWPORT_MODE_GENERAL)
  {
    return L7_ERROR;
  }

  return ( usmdbVoiceVlanPortVlanPrioritySet ( UnitIndex, interface, val ));
}
#endif /* I_agentPortVoiceVlanPriority */

#ifdef I_agentPortVoiceVlanDataPriorityMode
/*********************************************************************
* @purpose  Get the Voice VLAN Data Priority for the interface
*
* @param    unit           Unit Id
* @param    interface      interface
* @param    val            Voice VLAN Data Priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes   none
* @end
*********************************************************************/
L7_RC_t snmpAgentPortVoiceVlanDataPriorityModeGet( L7_uint32 UnitIndex,
                                        L7_uint32 interface,
                                        L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmdbVoiceVlanPortCosOverrideGet ( UnitIndex, interface, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch(temp_val)
    {
    case L7_FALSE:
      *val = D_agentPortVoiceVlanDataPriorityMode_trust;
      break;

    case L7_TRUE:
      *val = D_agentPortVoiceVlanDataPriorityMode_untrust;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Set the Voice VLAN Data Priority for the interface
*
* @param    unit           Unit Id
* @param    interface      interface
* @param    val            Voice VLAN Data Priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes   none
* @end
*********************************************************************/
L7_RC_t snmpAgentPortVoiceVlanDataPriorityModeSet( L7_uint32 UnitIndex,
                                        L7_uint32 interface,
                                        L7_int32 val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  switch(val)
  {
  case D_agentPortVoiceVlanDataPriorityMode_trust:
    temp_val = L7_FALSE;
    break;

  case D_agentPortVoiceVlanDataPriorityMode_untrust:
    temp_val = L7_TRUE;
    break;

  default:
    return L7_FAILURE;
  }

  {
    L7_uint32 mode;

    /* Voice mode can be set on on port in general mode */
    rc = usmDbDot1qSwPortModeGet(UnitIndex, interface, &mode);
    if (rc == L7_SUCCESS && mode != DOT1Q_SWPORT_MODE_GENERAL && temp_val == L7_TRUE)
    {
      return L7_ERROR;
    }
  }

  rc = usmdbVoiceVlanPortCosOverrideSet ( UnitIndex, interface, temp_val );

  return rc;
}
#endif /* I_agentPortVoiceVlanDataPriorityMode */

#ifdef I_agentPortVoiceVlanUntagged
/*********************************************************************
* @purpose  Get the Voice VLAN Untagged mode for the interface
*
* @param    unit           Unit Id
* @param    interface      interface
* @param    val            Voice VLAN Data Priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes   none
* @end
*********************************************************************/
L7_RC_t snmpAgentPortVoiceVlanUntaggedGet( L7_uint32 UnitIndex,
                                        L7_uint32 interface,
                                        L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmdbVoiceVlanPortUntagGet(UnitIndex, interface, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch(temp_val)
    {
    case L7_FALSE:
      *val = D_agentPortVoiceVlanUntagged_false;
      break;

    case L7_TRUE:
      *val = D_agentPortVoiceVlanUntagged_true;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }
  return rc;
}

/*********************************************************************
* @purpose  Set the Voice VLAN Untagged mode for the interface
*
* @param    unit           Unit Id
* @param    interface      interface
* @param    val            Voice VLAN Data Priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes   none
* @end
*********************************************************************/
L7_RC_t snmpAgentPortVoiceVlanUntaggedSet( L7_uint32 UnitIndex,
                                        L7_uint32 interface,
                                        L7_int32 val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  switch(val)
  {
  case D_agentPortVoiceVlanUntagged_false:
    temp_val = L7_FALSE;
    break;

  case D_agentPortVoiceVlanUntagged_true:
    temp_val = L7_TRUE;
    break;

  default:
    return L7_FAILURE;
  }
  {
    L7_uint32 mode;

    /* Voice mode can be set on on port in general mode */
    rc = usmDbDot1qSwPortModeGet(UnitIndex, interface, &mode);
    if (rc == L7_SUCCESS && mode != DOT1Q_SWPORT_MODE_GENERAL && temp_val == L7_TRUE)
    {
      return L7_ERROR;
    }
  }

  rc = usmdbVoiceVlanPortUntagSet(UnitIndex,interface,temp_val);
  return rc;
}
#endif

#ifdef I_agentPortVoiceVlanNoneMode
/*********************************************************************
* @purpose  Get the Voice VLAN None Mode for the interface
*
* @param    unit           Unit Id
* @param    interface      interface
* @param    val            Voice VLAN None Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes   none
* @end
*********************************************************************/
L7_RC_t snmpAgentPortVoiceVlanNoneModeGet( L7_uint32 UnitIndex,
                                        L7_uint32 interface,
                                        L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmdbVoiceVlanPortNoneGet(UnitIndex, interface, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch(temp_val)
    {
    case L7_FALSE:
      *val = D_agentPortVoiceVlanNoneMode_false;
      break;

    case L7_TRUE:
      *val = D_agentPortVoiceVlanNoneMode_true;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Set the Voice VLAN None Mode for the interface
*
* @param    unit           Unit Id
* @param    interface      interface
* @param    val            Voice VLAN Data Priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes   none
* @end
*********************************************************************/
L7_RC_t snmpAgentPortVoiceVlanNoneModeSet( L7_uint32 UnitIndex,
                                        L7_uint32 interface,
                                        L7_int32 val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  switch(val)
  {
  case D_agentPortVoiceVlanNoneMode_false:
    temp_val = L7_FALSE;
    break;

  case D_agentPortVoiceVlanNoneMode_true:
    temp_val = L7_TRUE;
    break;

  default:
    return L7_FAILURE;
  }
  {
    L7_uint32 mode;

    /* Voice mode can be set on on port in general mode */
    rc = usmDbDot1qSwPortModeGet(UnitIndex, interface, &mode);
    if (rc == L7_SUCCESS && mode != DOT1Q_SWPORT_MODE_GENERAL && temp_val == L7_TRUE)
    {
      return L7_ERROR;
    }
  }

  rc = usmdbVoiceVlanPortNoneSet(UnitIndex,interface,temp_val);
  return rc;
}
#endif

#ifdef I_agentPortVoiceVlanDSCP
/*********************************************************************
* @purpose  Get the Voice VLAN Data Priority for the interface
*
* @param    unit           Unit Id
* @param    interface      interface
* @param    val            Voice VLAN Data Priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes   none
* @end
*********************************************************************/
L7_RC_t snmpAgentPortVoiceVlanDSCPGet( L7_uint32 UnitIndex,
                                        L7_uint32 interface,
                                        L7_int32 *val )
{
  L7_RC_t rc;

  rc = usmdbVoiceVlanPortDscpGet(UnitIndex, interface, val );

  return rc;
}

/*********************************************************************
* @purpose  Set the Voice VLAN Data Priority for the interface
*
* @param    unit           Unit Id
* @param    interface      interface
* @param    val            Voice VLAN Data Priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes   none
* @end
*********************************************************************/
L7_RC_t snmpAgentPortVoiceVlanDSCPSet( L7_uint32 UnitIndex,
                                        L7_uint32 interface,
                                        L7_int32 val )
{
  L7_RC_t rc;

  {
    L7_uint32 mode;

    /* Voice mode can be set on on port in general mode */
    rc = usmDbDot1qSwPortModeGet(UnitIndex, interface, &mode);
    if (rc == L7_SUCCESS && mode != DOT1Q_SWPORT_MODE_GENERAL && val != FD_VOICE_VLAN_DSCP)
    {
      return L7_ERROR;
    }
  }

  rc = usmdbVoiceVlanPortDscpSet(UnitIndex,interface,val);
  return rc;
}
#endif


#ifdef I_agentPortVoiceVlanAuthMode
/*********************************************************************
* @purpose  Get the Voice VLAN Auth Mode for the interface
*
* @param    unit           Unit Id
* @param    interface      interface
* @param    val            Voice VLAN Auth Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes   none
* @end
*********************************************************************/
L7_RC_t snmpAgentPortVoiceVlanAuthModeGet(L7_uint32 UnitIndex,
                                          L7_uint32 interface,
                                          L7_int32 *val)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmdbVoiceVlanPortAuthGet(UnitIndex, interface, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
      case L7_ENABLE:
        *val = D_agentPortVoiceVlanAuthMode_enable;
        break;

      case L7_DISABLE:
        *val = D_agentPortVoiceVlanAuthMode_disable;
        break;

      default:
        rc = L7_FAILURE;
        break;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Set the Voice VLAN None Mode for the interface
*
* @param    unit           Unit Id
* @param    interface      interface
* @param    val            Voice VLAN Data Priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes   none
* @end
*********************************************************************/
L7_RC_t snmpAgentPortVoiceVlanAuthModeSet(L7_uint32 UnitIndex,
                                          L7_uint32 interface,
                                          L7_int32 val)
{
  L7_uint32 temp_val;
  L7_uint32 mode;

  switch (val)
  {
    case D_agentPortVoiceVlanAuthMode_disable:
      temp_val = L7_DISABLE;
      break;

    case D_agentPortVoiceVlanAuthMode_enable:
      temp_val = L7_ENABLE;
      break;

    default:
      return L7_FAILURE;
  }

  /* Voice mode can be set on on port in general mode */
  if (((L7_SUCCESS != usmDbDot1qSwPortModeGet(UnitIndex, interface, &mode)) ||
       (DOT1Q_SWPORT_MODE_GENERAL != mode)) &&
      (L7_DISABLE == temp_val))
  {
    return L7_ERROR;
  }

  return usmdbVoiceVlanPortAuthSet(UnitIndex, interface, temp_val);
}
#endif

#ifdef I_agentPortVoiceVlanOperationalStatus
/*********************************************************************
* @purpose  Set the Voice VLAN Operational Status for the interface
*
* @param    unit           Unit Id
* @param    interface      interface
* @param    val            Voice VLAN Operational Status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes   none
* @end
*********************************************************************/
L7_RC_t snmpAgentPortVoiceVlanOperationalStatusGet( L7_uint32 UnitIndex,
                                             L7_uint32 interface,
                                             L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmdbVoiceVlanPorStatusGet ( UnitIndex, interface, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch(temp_val)
    {
    case L7_TRUE:
      *val = D_agentPortVoiceVlanOperationalStatus_enabled;
      break;

    case L7_FALSE:
      *val = D_agentPortVoiceVlanOperationalStatus_disabled;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}
#endif /* I_agentPortVoiceVlanOperationalStatus */

#ifdef I_agentPortDot3FlowControlOperStatus
L7_RC_t
snmpAgentPortDot3FlowControlOperStatusGet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_int32 *status)
{
  L7_BOOL rx_mode, tx_mode;
  L7_RC_t rc;

  rc = usmDbIfPauseStatusGet(UnitIndex, intIfIndex, &rx_mode, &tx_mode);
  if (rc == L7_SUCCESS)
  {
    if (rx_mode == L7_TRUE || tx_mode == L7_TRUE)
    {
      *status = D_agentPortDot3FlowControlOperStatus_active;
    }
    else
    {
      *status = D_agentPortDot3FlowControlOperStatus_inactive;
    }
  }
  return rc;
}
#endif /* I_agentPortDot3FlowControlOperStatus */

#if L7_FEAT_SF10GBT

/*********************************************************************
* @purpose  Get the prot firmware revision and part number
*
* @param    unit           Unit Id
* @param    interface      interface
* @param    fwPartNumber   Firmware part number
* @param    fwRevision     Firmware revision
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes   none
* @end
*********************************************************************/
L7_RC_t snmpAgentPortGetFirmwareRevision( L7_uint32 interface,
                                        L7_uchar8 *fwPartNumber,
                                        L7_uchar8 *fwRevision)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 UnitIndex, SlotIndex,Index;
  L7_uint32 numOfPorts, val;

  memset(fwRevision,0,4);
  memset(fwPartNumber,0,8);
  
  /* take into account zero-based numbering of units obtained by USMDB_UNIT_CURRENT*/
  if (usmDbUnitSlotPortGet(interface,&UnitIndex,&SlotIndex,&Index) == L7_SUCCESS)
  {
    usmDbPortsPerBoxGet(UnitIndex,&numOfPorts);
    /* Checking for 10G non-stacking ports*/
    if ( (usmDbIfTypeGet( UnitIndex, interface, &val) == L7_SUCCESS)
         && (val == L7_IANA_10G_ETHERNET)
         && ((Index == numOfPorts - 1) || (Index == numOfPorts)) )

    {
        rc = usmDbIfTransceiverFwRevisionGet(UnitIndex, interface, fwRevision, fwPartNumber);
    }
    if (rc != L7_SUCCESS)
    {
      memset(fwRevision,0xFF,4);
      memset(fwPartNumber,0xFF,8);
      rc = L7_SUCCESS;
    }
  }
  return rc;
}

#endif /* #if L7_FEAT_SF10GBT */

#ifdef I_agentPortSpeedDuplexStatus 
/*********************************************************************
* @purpose  Get the port speed and duplex parameters
*
* @param    interface             interface
* @param    speedDuplexStatus     speedDuplexStatus
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes   none
* @end
*********************************************************************/
L7_RC_t snmpAgentPortSpeedDuplexGet(L7_uint32 unit, L7_uint32 intIfNum, L7_uchar8 *speedDuplexStatus)
{
  L7_uint32 intfSpeed;
  L7_uint32 duplex_val;

  if (usmDbIfSpeedGet(unit, intIfNum, &intfSpeed) == L7_SUCCESS)
  {
    if(intfSpeed == L7_PORTCTRL_PORTSPEED_FULL_1000SX)
    {
      *speedDuplexStatus |= L7_SNMP_PORT_CAPABILITY_DUPLEX_FULL;
      *speedDuplexStatus |= L7_SNMP_PORT_CAPABILITY_SPEED_1000;
    }
    else if ((intfSpeed == L7_PORTCTRL_PORTSPEED_FULL_100TX) || 
             (intfSpeed == L7_PORTCTRL_PORTSPEED_FULL_100FX))
    {
      *speedDuplexStatus |= L7_SNMP_PORT_CAPABILITY_DUPLEX_FULL;
      *speedDuplexStatus |= L7_SNMP_PORT_CAPABILITY_SPEED_100;
    }
    else if (intfSpeed == L7_PORTCTRL_PORTSPEED_HALF_100TX)
    {
      *speedDuplexStatus |= L7_SNMP_PORT_CAPABILITY_DUPLEX_HALF;
      *speedDuplexStatus |= L7_SNMP_PORT_CAPABILITY_SPEED_100;
    }
    else if (intfSpeed == L7_PORTCTRL_PORTSPEED_FULL_10T)
    {
      *speedDuplexStatus |= L7_SNMP_PORT_CAPABILITY_DUPLEX_FULL;
      *speedDuplexStatus |= L7_SNMP_PORT_CAPABILITY_SPEED_10;
    }
    else if (intfSpeed == L7_PORTCTRL_PORTSPEED_HALF_10T)
    {
      *speedDuplexStatus |= L7_SNMP_PORT_CAPABILITY_DUPLEX_HALF;
      *speedDuplexStatus |= L7_SNMP_PORT_CAPABILITY_SPEED_10;
    }
  }
  else 
  {
    return L7_FAILURE;
  }


  if ((usmDbIfDuplexGet(intIfNum, &duplex_val)) == L7_SUCCESS)  
  {
    if (duplex_val == L7_PORT_CAPABILITY_DUPLEX_AUTO)
    {
      *speedDuplexStatus |= L7_SNMP_PORT_CAPABILITY_DUPLEX_AUTO;
    }
  }
  else 
  {
    return L7_FAILURE;
  }

  if (usmDbIsIntfSpeedAuto(intIfNum) == L7_TRUE)
  {
    *speedDuplexStatus |= L7_SNMP_PORT_CAPABILITY_SPEED_AUTO;
  }
  
  return L7_SUCCESS;   
}

/*********************************************************************
* @purpose  Set the port speed and duplex parameters
*
* @param    unit                  stack manager unit index
* @param    interface             interface
* @param    speedDuplexStatus     speedDuplexStatus
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes   none
* @end
*********************************************************************/
L7_RC_t snmpAgentPortSpeedDuplexSet(L7_uint32 unit, L7_uint32 intIfNum, L7_uchar8 *buf)
{
  L7_uint32 intfStatus;
  L7_BOOL autoneg_mode = L7_FALSE;
  L7_uint32 autoneg_val, val;
  L7_uint32 duplex_val = 0, speed_val = 0;
  
  if (usmDbDot3adIsMember(unit, intIfNum) == L7_SUCCESS)
  {
    return L7_ERROR;
  }

  if (usmDbIfAutoNegAdminStatusGet(unit, intIfNum, &val) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (buf[0] & L7_SNMP_PORT_CAPABILITY_DUPLEX_AUTO)
  {
    duplex_val |= L7_PORT_CAPABILITY_DUPLEX_AUTO; 
    buf[0] &= ~(L7_SNMP_PORT_CAPABILITY_DUPLEX_AUTO);
  }
  if (buf[0] & L7_SNMP_PORT_CAPABILITY_DUPLEX_HALF)
  {
    duplex_val |= L7_PORT_CAPABILITY_DUPLEX_HALF;
    buf[0] &= ~(L7_SNMP_PORT_CAPABILITY_DUPLEX_HALF);
  }
  if (buf[0] & L7_SNMP_PORT_CAPABILITY_DUPLEX_FULL)
  {
    duplex_val |= L7_PORT_CAPABILITY_DUPLEX_FULL;
    buf[0] &= ~(L7_SNMP_PORT_CAPABILITY_DUPLEX_FULL);
  }
  if (buf[0] & L7_SNMP_PORT_CAPABILITY_SPEED_AUTO)
  {
    speed_val |= L7_PORT_CAPABILITY_SPEED_AUTO;
    buf[0] &= ~(L7_SNMP_PORT_CAPABILITY_SPEED_AUTO);
  }
  if (buf[0] & L7_SNMP_PORT_CAPABILITY_SPEED_10)
  {
    speed_val |= L7_PORT_CAPABILITY_SPEED_10;
    buf[0] &= ~(L7_SNMP_PORT_CAPABILITY_SPEED_10);
  }
  if (buf[0] & L7_SNMP_PORT_CAPABILITY_SPEED_100)
  {
    speed_val |= L7_PORT_CAPABILITY_SPEED_100;
    buf[0] &= ~(L7_SNMP_PORT_CAPABILITY_SPEED_100);
  }
  if (buf[0] & (L7_SNMP_PORT_CAPABILITY_SPEED_1000))
  {
    speed_val |= L7_PORT_CAPABILITY_SPEED_1000;
    buf[0] &= ~(L7_SNMP_PORT_CAPABILITY_SPEED_1000);
  }
  if (buf[0] & L7_SNMP_PORT_CAPABILITY_SPEED_10G)
  {
    speed_val |= L7_PORT_CAPABILITY_SPEED_10G;
    buf[0] &= ~(L7_SNMP_PORT_CAPABILITY_SPEED_10G);
  }

  if (buf[0] != 0x00)
  {
    return L7_FAILURE;
  }

  if ((duplex_val == 0) && (speed_val == 0))
  {
    return L7_FAILURE;
  }

  if(duplex_val != 0)
  {
    if (usmDbNimIntfSpeedGet(intIfNum, &intfStatus) != L7_SUCCESS)
    {
      return L7_ERROR;
    }
    
    if (usmDbIsIntfSpeedAuto(intIfNum) == L7_TRUE)
    {
      autoneg_mode = L7_TRUE;
    }
 
    intfStatus |= duplex_val;

    if ((duplex_val) &  L7_PORT_CAPABILITY_DUPLEX_AUTO)
    {
      autoneg_mode = L7_TRUE;
    }
    if (autoneg_mode == L7_TRUE)
    {
     /* set autonegotiation capabilities and as well as interface speed value.
      Even if one fails the operation fails*/
      if (usmDbIntfAutoNegValueGet(intfStatus, intIfNum, &autoneg_val) != L7_SUCCESS)
      {
        return L7_ERROR;
      }

      if ((usmDbIfSpeedSet(unit, intIfNum, intfStatus) != L7_SUCCESS) ||
          (usmDbIfAutoNegoStatusCapabilitiesSet(intIfNum, autoneg_val) != L7_SUCCESS))
      {
        return L7_FAILURE;
      }
    }
    else
    {
      if (val == L7_ENABLE)
      {
        autoneg_val = 0;
        if ((usmDbIfSpeedSet(unit, intIfNum, intfStatus) != L7_SUCCESS) ||
            (usmDbIfAutoNegoStatusCapabilitiesSet(intIfNum, autoneg_val) != L7_SUCCESS))
        {
          return L7_FAILURE;
        }
      }
      else if (usmDbIfSpeedSet(unit, intIfNum, intfStatus) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
    }

  }

  if (speed_val != 0)
  {
    if ((usmDbIfDuplexGet(intIfNum, &intfStatus)) != L7_SUCCESS)
    {
      return L7_ERROR;
    }

    if (intfStatus == L7_PORT_CAPABILITY_DUPLEX_AUTO)
    {
      autoneg_mode = L7_TRUE;
    }
  
    intfStatus |= speed_val;

    if ((speed_val) & L7_SNMP_PORT_CAPABILITY_SPEED_AUTO)
    {
      autoneg_mode = L7_TRUE;
    } 
  
    if (autoneg_mode == L7_TRUE)
    {
     /* set autonegotiation capabilities and as well as interface speed value.
      Even if one fails the operation fails*/
      if (usmDbIntfAutoNegValueGet(intfStatus, intIfNum, &autoneg_val) != L7_SUCCESS)
      {
        return L7_ERROR;
      }

      if ((usmDbIfSpeedSet(unit, intIfNum, intfStatus) != L7_SUCCESS) ||
          (usmDbIfAutoNegoStatusCapabilitiesSet(intIfNum, autoneg_val) != L7_SUCCESS))
      {
        return L7_FAILURE;
      }
    }
    else
    {
      if (val == L7_ENABLE)
      {
        autoneg_val = 0;
        if ((usmDbIfSpeedSet(unit, intIfNum, intfStatus) != L7_SUCCESS) ||
            (usmDbIfAutoNegoStatusCapabilitiesSet(intIfNum, autoneg_val) != L7_SUCCESS))
        {    
          return L7_FAILURE;
        }
      }
      else if (usmDbIfSpeedSet(unit, intIfNum, intfStatus) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
    }
  }
  return L7_SUCCESS;
}
#endif
#ifdef I_agentIASUserIndex
/*********************************************************************
*
* @purpose  Create a user entry in Dot1x user database     
*          
* @param    buf @b((input)) user name
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE
*
* @comments
*
* @notes 
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t
snmpAgentIASUserConfigCreateSet (L7_char8 *buf)
{
  L7_RC_t rc;
  L7_uint32 tempInt;
  L7_char8 temp_buf[SNMP_BUFFER_LEN];

  /* check name for correct characters and length */
  if (usmDbStringAlphaNumericCheck(buf) != L7_SUCCESS ||
      strlen(buf) >= L7_LOGIN_SIZE)
    return L7_FAILURE;

  /* check all users for an existing entry by that name */
  for (tempInt = 0; tempInt < L7_MAX_IAS_USERS; tempInt++)
  {
    bzero(temp_buf, SNMP_BUFFER_LEN);
    rc = usmDbUserMgrIASUserDBUserNameGet(tempInt, temp_buf, sizeof(temp_buf));
    if (usmDbStringCaseInsensitiveCompare(temp_buf, buf) == L7_SUCCESS)
      return L7_FAILURE;
  }

  for (tempInt = 0; tempInt < L7_MAX_IAS_USERS; tempInt++)
  {
    rc = usmDbUserMgrIASUserDBUserNameGet(tempInt, temp_buf, sizeof(temp_buf));

    if (strcmp(temp_buf, "") == 0)
    {
      rc = usmDbUserMgrIASUserDBUserNameSet(tempInt, buf);
      rc = usmDbUserMgrIASUserDBUserPasswordSet(tempInt, "", L7_FALSE);
      rc = usmDbUserMgrIASUserDBUserStatusSet(tempInt, L7_ENABLE);
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Get the dot1x user entry status
*          
* @param    UserIndex @b((input)) user entry index
* @param    val @b((output)) user entry status
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE
*
* @comments
*
* @notes 
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t
snmpAgentIASUserStatusGet (L7_uint32 UserIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbUserMgrIASUserDBUserStatusGet (UserIndex, &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentIASUserStatus_active;
      break;

    case L7_DISABLE:
      *val = D_agentIASUserStatus_destroy;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Set the dot1x user entry status
*          
* @param    UserIndex @b((input)) user entry index
* @param    val @b((input)) user entry status
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE
*
* @comments
*
* @notes 
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t
snmpAgentIASUserStatusSet (L7_uint32 UserIndex, L7_int32 val )
{
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentIASUserStatus_active:
    rc = L7_SUCCESS;
    break;

  case D_agentIASUserStatus_destroy:
    rc = usmDbUserMgrIASUserDBUserDelete(UserIndex );
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  verify if the user index is free    
*          
* @param    userIndex @b((input)) user entry index
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE
*
* @comments
*
* @notes 
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t
snmpAgentIASUserConfigEntryGet (L7_int32 userIndex)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  if (usmDbUserMgrIASUserDBUserNameGet(userIndex, buf, sizeof(buf)) == L7_SUCCESS && buf[0] != 0)
    return L7_SUCCESS;
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Get the next available user index    
*          
* @param    userIndex @b((output))  user entry index
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE
*
* @comments
*
* @notes 
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t
snmpAgentIASUserConfigEntryNextGet (L7_int32 *userIndex)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  if (*userIndex < 0)
    *userIndex = 0;

  while (*userIndex < L7_MAX_IAS_USERS)
  {
    if (usmDbUserMgrIASUserDBUserNameGet(*userIndex, buf, sizeof(buf)) == L7_SUCCESS && buf[0] != 0)
      return L7_SUCCESS;

    /* increment */
    *userIndex = *userIndex + 1;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Get the user name at the given index    
*          
* @param    userIndex @b((input)) user entry index
* @param    buf @b((output)) user name
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE
*
* @comments
*
* @notes 
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t
snmpAgentIASUserNameSet (L7_int32 userIndex, L7_char8 *buf )
{
  L7_RC_t rc;
  L7_uint32 tempInt;
  L7_char8 temp_buf[SNMP_BUFFER_LEN];

  /* check name for correct characters and length */
  if (usmDbStringAlphaNumericCheck(buf) != L7_SUCCESS ||
      strlen(buf) >= L7_LOGIN_SIZE)
    return L7_FAILURE;

  /* check all users for an existing entry by that name */
  for (tempInt = 0; tempInt < L7_MAX_IAS_USERS; tempInt++)
  {
    if (tempInt != userIndex)
    {
      bzero(temp_buf, SNMP_BUFFER_LEN);
      rc = usmDbUserMgrIASUserDBUserNameGet(tempInt, temp_buf, sizeof(temp_buf));
      if (usmDbStringCaseInsensitiveCompare(temp_buf, buf) == L7_SUCCESS)
        return L7_FAILURE;
    }
  }

  return usmDbUserMgrIASUserDBUserNameSet(userIndex, buf);
}
#endif

#ifdef L7_ROUTING_PACKAGE
/*********************************************************************
* @purpose  Set the template ID of the next active SDM template.
*
* @param    templateId @b{(input)}  template ID
*
* @returns  L7_SUCCESS
*           L7_FAILURE if template ID not stored persistently
*
* @notes    This API is only called on the management unit. 
*           Immediately saves the value persistently on a local unit. If
*           stacking, push the template ID to other members of the stack,
*           so that they can also store persistently.
*
*           If templateId is Default, deletes the next active 
*           template ID from persistent storage.
*
* @end
*********************************************************************/
L7_RC_t
snmpSdmNextActiveTemplateSet(L7_uint32 templateId)
{
  if ((templateId == D_agentSdmPreferNextTemplate_ipv4DataCenter) &&
      (usmDbSdmTemplateSupported(templateId) != L7_TRUE))
  {
    return L7_FAILURE;
  }
  return usmDbSdmNextActiveTemplateSet(templateId);
}

/*********************************************************************
*
* @purpose Determine if a given SDM template is supported in this build.
*          
* @param   templateId  @b{(input)}    template ID
* 
* @returns L7_TRUE or L7_FALSE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_BOOL
snmpSdmTemplateIdGet(L7_uint32 templateId)
{
  return usmDbSdmTemplateSupported(templateId);
}
#endif /* L7_ROUTING_PACKAGE */
