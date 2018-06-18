/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
* @filename k_mib_fastpathlogging_api.c
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

#include "l7_common.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"

#include "usmdb_log_api.h"
#include "k_mib_fastpathlogging_api.h"
#include "k_private_base.h"
#include "usmdb_cmd_logger_api.h"
#include "log_exports.h"

L7_RC_t snmpAgentLogInMemoryAdminStatusGet(L7_uint32 *val)
{
  L7_RC_t rc;
  L7_ADMIN_MODE_t temp_val;

  rc = usmDbLogInMemoryAdminStatusGet(USMDB_UNIT_CURRENT, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ADMIN_MODE_DISABLE:
      *val = D_agentLogInMemoryAdminStatus_disable;
      break;
    case L7_ADMIN_MODE_ENABLE:
      *val = D_agentLogInMemoryAdminStatus_enable;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }
      
  return rc;
}

L7_RC_t snmpAgentLogInMemoryAdminStatusSet(L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_ADMIN_MODE_t temp_val = 0;

  switch (val)
  {
  case D_agentLogInMemoryAdminStatus_disable:
    temp_val = L7_ADMIN_MODE_DISABLE;
    break;
  case D_agentLogInMemoryAdminStatus_enable:
    temp_val = L7_ADMIN_MODE_ENABLE;
    break;
  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }
  
  if (rc == L7_SUCCESS)
  {
    rc = usmDbLogInMemoryAdminStatusSet(USMDB_UNIT_CURRENT, temp_val);
  }

  if (rc == L7_ALREADY_CONFIGURED)
  {
    rc = L7_SUCCESS;
  }

  return rc;
}


L7_RC_t snmpAgentLogInMemoryBehaviorGet(L7_uint32 *val)
{
  L7_RC_t rc;
  L7_LOG_FULL_BEHAVIOR_t temp_val;

  rc = usmDbLogInMemoryBehaviorGet(USMDB_UNIT_CURRENT, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_LOG_WRAP_BEHAVIOR:
      *val = D_agentLogInMemoryBehavior_wrap;
      break;
    case L7_LOG_STOP_ON_FULL_BEHAVIOR:
      *val = D_agentLogInMemoryBehavior_stop_on_full;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }
      
  return rc;
}

L7_RC_t snmpAgentLogInMemoryBehaviorSet(L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_LOG_FULL_BEHAVIOR_t temp_val = 0;

  switch (val)
  {
  case D_agentLogInMemoryBehavior_wrap:
    temp_val = L7_LOG_WRAP_BEHAVIOR;
    break;
  case D_agentLogInMemoryBehavior_stop_on_full:
    temp_val = L7_LOG_STOP_ON_FULL_BEHAVIOR;
    break;
  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbLogInMemoryBehaviorSet(USMDB_UNIT_CURRENT, temp_val);
  }
      
  if (rc == L7_ALREADY_CONFIGURED)
  {
    rc = L7_SUCCESS;
  }

  return rc;
}

/***************************************************************************************/

L7_RC_t snmpAgentLogConsoleAdminStatusGet(L7_uint32 *val)
{
  L7_RC_t rc;
  L7_ADMIN_MODE_t temp_val;

  rc = usmDbLogConsoleAdminStatusGet(USMDB_UNIT_CURRENT, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ADMIN_MODE_DISABLE:
      *val = D_agentLogConsoleAdminStatus_disable;
      break;
    case L7_ADMIN_MODE_ENABLE:
      *val = D_agentLogConsoleAdminStatus_enable;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }
      
  return rc;
}

L7_RC_t snmpAgentLogConsoleAdminStatusSet(L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_ADMIN_MODE_t temp_val = 0;

  switch (val)
  {
  case D_agentLogConsoleAdminStatus_disable:
    temp_val = L7_ADMIN_MODE_DISABLE;
    break;
  case D_agentLogConsoleAdminStatus_enable:
    temp_val = L7_ADMIN_MODE_ENABLE;
    break;
  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }
  
  if (rc == L7_SUCCESS)
  {
    rc = usmDbLogConsoleAdminStatusSet(USMDB_UNIT_CURRENT, temp_val);
  }
      
  if (rc == L7_ALREADY_CONFIGURED)
  {
    rc = L7_SUCCESS;
  }

  return rc;
}

L7_RC_t snmpAgentLogConsoleSeverityFilterGet(L7_uint32 *val)
{
  L7_RC_t rc;
  L7_LOG_SEVERITY_t temp_val;

  rc = usmDbLogConsoleSeverityFilterGet(USMDB_UNIT_CURRENT, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_LOG_SEVERITY_EMERGENCY:
      *val = D_agentLogConsoleSeverityFilter_emergency;
      break;
    case L7_LOG_SEVERITY_ALERT:
      *val = D_agentLogConsoleSeverityFilter_alert;
      break;
    case L7_LOG_SEVERITY_CRITICAL:
      *val = D_agentLogConsoleSeverityFilter_critical;
      break;
    case L7_LOG_SEVERITY_ERROR:
      *val = D_agentLogConsoleSeverityFilter_error;
      break;
    case L7_LOG_SEVERITY_WARNING:
      *val = D_agentLogConsoleSeverityFilter_warning;
      break;
    case L7_LOG_SEVERITY_NOTICE:
      *val = D_agentLogConsoleSeverityFilter_notice;
      break;
    case L7_LOG_SEVERITY_INFO:
      *val = D_agentLogConsoleSeverityFilter_informational;
      break;
    case L7_LOG_SEVERITY_DEBUG:
      *val = D_agentLogConsoleSeverityFilter_debug;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }
      
  return rc;
}

L7_RC_t snmpAgentLogConsoleSeverityFilterSet(L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_LOG_SEVERITY_t temp_val = 0;

  switch (val)
  {
  case D_agentLogConsoleSeverityFilter_emergency:
    temp_val = L7_LOG_SEVERITY_EMERGENCY;
    break;
  case D_agentLogConsoleSeverityFilter_alert:
    temp_val = L7_LOG_SEVERITY_ALERT;
    break;
  case D_agentLogConsoleSeverityFilter_critical:
    temp_val = L7_LOG_SEVERITY_CRITICAL;
    break;
  case D_agentLogConsoleSeverityFilter_error:
    temp_val = L7_LOG_SEVERITY_ERROR;
    break;
  case D_agentLogConsoleSeverityFilter_warning:
    temp_val = L7_LOG_SEVERITY_WARNING;
    break;
  case D_agentLogConsoleSeverityFilter_notice:
    temp_val = L7_LOG_SEVERITY_NOTICE;
    break;
  case D_agentLogConsoleSeverityFilter_informational:
    temp_val = L7_LOG_SEVERITY_INFO;
    break;
  case D_agentLogConsoleSeverityFilter_debug:
    temp_val = L7_LOG_SEVERITY_DEBUG;
    break;
  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbLogConsoleSeverityFilterSet(USMDB_UNIT_CURRENT, temp_val);
  }
      
  if (rc == L7_ALREADY_CONFIGURED)
  {
    rc = L7_SUCCESS;
  }

  return rc;
}

/***************************************************************************************/

L7_RC_t snmpAgentLogPersistentAdminStatusGet(L7_uint32 *val)
{
  L7_RC_t rc;
  L7_ADMIN_MODE_t temp_val;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_LOG_COMPONENT_ID, L7_LOG_PERSISTENT_FEATURE_ID) != L7_TRUE)
  {
     return L7_FAILURE;
  }

  rc = usmDbLogPersistentAdminStatusGet(USMDB_UNIT_CURRENT, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ADMIN_MODE_DISABLE:
      *val = D_agentLogPersistentAdminStatus_disable;
      break;
    case L7_ADMIN_MODE_ENABLE:
      *val = D_agentLogPersistentAdminStatus_enable;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }
      
  return rc;
}

L7_RC_t snmpAgentLogPersistentAdminStatusSet(L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_ADMIN_MODE_t temp_val = 0;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_LOG_COMPONENT_ID, L7_LOG_PERSISTENT_FEATURE_ID) != L7_TRUE)
  {
     return L7_FAILURE;
  }

  switch (val)
  {
  case D_agentLogPersistentAdminStatus_disable:
    temp_val = L7_ADMIN_MODE_DISABLE;
    break;
  case D_agentLogPersistentAdminStatus_enable:
    temp_val = L7_ADMIN_MODE_ENABLE;
    break;
  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }
  
  if (rc == L7_SUCCESS)
  {
    rc = usmDbLogPersistentAdminStatusSet(USMDB_UNIT_CURRENT, temp_val);
  }
      
  if (rc == L7_ALREADY_CONFIGURED)
  {
    rc = L7_SUCCESS;
  }

  return rc;
}

L7_RC_t snmpAgentLogPersistentSeverityFilterGet(L7_uint32 *val)
{
  L7_RC_t rc;
  L7_LOG_SEVERITY_t temp_val;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_LOG_COMPONENT_ID, L7_LOG_PERSISTENT_FEATURE_ID) != L7_TRUE)
  {
     return L7_FAILURE;
  }

  rc = usmDbLogPersistentSeverityFilterGet(USMDB_UNIT_CURRENT, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_LOG_SEVERITY_EMERGENCY:
      *val = D_agentLogPersistentSeverityFilter_emergency;
      break;
    case L7_LOG_SEVERITY_ALERT:
      *val = D_agentLogPersistentSeverityFilter_alert;
      break;
    case L7_LOG_SEVERITY_CRITICAL:
      *val = D_agentLogPersistentSeverityFilter_critical;
      break;
    case L7_LOG_SEVERITY_ERROR:
      *val = D_agentLogPersistentSeverityFilter_error;
      break;
    case L7_LOG_SEVERITY_WARNING:
      *val = D_agentLogPersistentSeverityFilter_warning;
      break;
    case L7_LOG_SEVERITY_NOTICE:
      *val = D_agentLogPersistentSeverityFilter_notice;
      break;
    case L7_LOG_SEVERITY_INFO:
      *val = D_agentLogPersistentSeverityFilter_informational;
      break;
    case L7_LOG_SEVERITY_DEBUG:
      *val = D_agentLogPersistentSeverityFilter_debug;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }
      
  return rc;
}

L7_RC_t snmpAgentLogPersistentSeverityFilterSet(L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_LOG_SEVERITY_t temp_val = 0;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_LOG_COMPONENT_ID, L7_LOG_PERSISTENT_FEATURE_ID) != L7_TRUE)
  {
     return L7_FAILURE;
  }

  switch (val)
  {
  case D_agentLogPersistentSeverityFilter_emergency:
    temp_val = L7_LOG_SEVERITY_EMERGENCY;
    break;
  case D_agentLogPersistentSeverityFilter_alert:
    temp_val = L7_LOG_SEVERITY_ALERT;
    break;
  case D_agentLogPersistentSeverityFilter_critical:
    temp_val = L7_LOG_SEVERITY_CRITICAL;
    break;
  case D_agentLogPersistentSeverityFilter_error:
    temp_val = L7_LOG_SEVERITY_ERROR;
    break;
  case D_agentLogPersistentSeverityFilter_warning:
    temp_val = L7_LOG_SEVERITY_WARNING;
    break;
  case D_agentLogPersistentSeverityFilter_notice:
    temp_val = L7_LOG_SEVERITY_NOTICE;
    break;
  case D_agentLogPersistentSeverityFilter_informational:
    temp_val = L7_LOG_SEVERITY_INFO;
    break;
  case D_agentLogPersistentSeverityFilter_debug:
    temp_val = L7_LOG_SEVERITY_DEBUG;
    break;
  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbLogPersistentSeverityFilterSet(USMDB_UNIT_CURRENT, temp_val);
  }
      
  if (rc == L7_ALREADY_CONFIGURED)
  {
    rc = L7_SUCCESS;
  }

  return rc;
}

/***************************************************************************************/

L7_RC_t snmpAgentLogSyslogAdminStatusGet(L7_uint32 *val)
{
  L7_RC_t rc;
  L7_ADMIN_MODE_t temp_val;

  rc = usmDbLogSyslogAdminStatusGet(USMDB_UNIT_CURRENT, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ADMIN_MODE_DISABLE:
      *val = D_agentLogSyslogAdminStatus_disable;
      break;
    case L7_ADMIN_MODE_ENABLE:
      *val = D_agentLogSyslogAdminStatus_enable;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }
      
  return rc;
}

L7_RC_t snmpAgentLogSyslogAdminStatusSet(L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_ADMIN_MODE_t temp_val = 0;

  switch (val)
  {
  case D_agentLogSyslogAdminStatus_disable:
    temp_val = L7_ADMIN_MODE_DISABLE;
    break;
  case D_agentLogSyslogAdminStatus_enable:
    temp_val = L7_ADMIN_MODE_ENABLE;
    break;
  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }
  
  if (rc == L7_SUCCESS)
  {
    rc = usmDbLogSyslogAdminStatusSet(USMDB_UNIT_CURRENT, temp_val);
  }
      
  if (rc == L7_ALREADY_CONFIGURED)
  {
    rc = L7_SUCCESS;
  }

  return rc;
}


/***************************************************************************************/

L7_RC_t snmpAgentLogSyslogHostEntryGet(L7_uint32 agentLogHostTableIndex)
{
  L7_ROW_STATUS_t temp_val;

  if (usmDbLogHostTableRowStatusGet(USMDB_UNIT_CURRENT, agentLogHostTableIndex, &temp_val) == L7_SUCCESS)
  {
    /* check if it's an invalid entry */
    if (temp_val == L7_ROW_STATUS_INVALID ||
        temp_val == L7_ROW_STATUS_DESTROY)
      return L7_FAILURE;

    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

L7_RC_t snmpAgentLogSyslogHostEntryNextGet(L7_uint32 *agentLogHostTableIndex)
{
  L7_ROW_STATUS_t temp_val;
  L7_uint32 temp_index = *agentLogHostTableIndex;

  if (temp_index < 1)
    temp_index = 1;

  while (temp_index <= L7_LOG_MAX_HOSTS)
  {
    if  (usmDbLogHostTableRowStatusGet(USMDB_UNIT_CURRENT, temp_index, &temp_val) == L7_SUCCESS)
    {
      /* check if it's a valid entry */
      if (temp_val != L7_ROW_STATUS_INVALID &&
          temp_val != L7_ROW_STATUS_DESTROY)
      {
        *agentLogHostTableIndex = temp_index;

        return L7_SUCCESS;
      }
    }
    temp_index++;
  }

  return L7_FAILURE;
}

L7_RC_t snmpAgentLogHostTableSeverityFilterGet(L7_uint32 agentLogHostTableIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_LOG_SEVERITY_t temp_val;

  rc = usmDbLogHostTableSeverityGet(USMDB_UNIT_CURRENT, agentLogHostTableIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_LOG_SEVERITY_EMERGENCY:
      *val = D_agentLogHostTableSeverityFilter_emergency;
      break;
    case L7_LOG_SEVERITY_ALERT:
      *val = D_agentLogHostTableSeverityFilter_alert;
      break;
    case L7_LOG_SEVERITY_CRITICAL:
      *val = D_agentLogHostTableSeverityFilter_critical;
      break;
    case L7_LOG_SEVERITY_ERROR:
      *val = D_agentLogHostTableSeverityFilter_error;
      break;
    case L7_LOG_SEVERITY_WARNING:
      *val = D_agentLogHostTableSeverityFilter_warning;
      break;
    case L7_LOG_SEVERITY_NOTICE:
      *val = D_agentLogHostTableSeverityFilter_notice;
      break;
    case L7_LOG_SEVERITY_INFO:
      *val = D_agentLogHostTableSeverityFilter_informational;
      break;
    case L7_LOG_SEVERITY_DEBUG:
      *val = D_agentLogHostTableSeverityFilter_debug;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }
      
  return rc;
}

L7_RC_t snmpAgentLogHostTableSeverityFilterSet(L7_uint32 agentLogHostTableIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_LOG_SEVERITY_t temp_val = 0;

  switch (val)
  {
  case D_agentLogHostTableSeverityFilter_emergency:
    temp_val = L7_LOG_SEVERITY_EMERGENCY;
    break;
  case D_agentLogHostTableSeverityFilter_alert:
    temp_val = L7_LOG_SEVERITY_ALERT;
    break;
  case D_agentLogHostTableSeverityFilter_critical:
    temp_val = L7_LOG_SEVERITY_CRITICAL;
    break;
  case D_agentLogHostTableSeverityFilter_error:
    temp_val = L7_LOG_SEVERITY_ERROR;
    break;
  case D_agentLogHostTableSeverityFilter_warning:
    temp_val = L7_LOG_SEVERITY_WARNING;
    break;
  case D_agentLogHostTableSeverityFilter_notice:
    temp_val = L7_LOG_SEVERITY_NOTICE;
    break;
  case D_agentLogHostTableSeverityFilter_informational:
    temp_val = L7_LOG_SEVERITY_INFO;
    break;
  case D_agentLogHostTableSeverityFilter_debug:
    temp_val = L7_LOG_SEVERITY_DEBUG;
    break;
  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbLogHostTableSeveritySet(USMDB_UNIT_CURRENT, agentLogHostTableIndex, temp_val);
  }
      
  if (rc == L7_ALREADY_CONFIGURED)
  {
    rc = L7_SUCCESS;
  }

  return rc;
}



L7_RC_t snmpAgentLogHostTableRowStatusGet(L7_uint32 agentLogHostTableIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_ROW_STATUS_t temp_val;
  
  rc = usmDbLogHostTableRowStatusGet(USMDB_UNIT_CURRENT, agentLogHostTableIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ROW_STATUS_ACTIVE:
      *val = D_agentLogHostTableRowStatus_active;
      break;
    case L7_ROW_STATUS_NOT_IN_SERVICE:
      *val = D_agentLogHostTableRowStatus_notInService;
      break;
    case L7_ROW_STATUS_NOT_READY:
      *val = D_agentLogHostTableRowStatus_notReady;
      break;
    case L7_ROW_STATUS_CREATE_AND_GO:
      *val = D_agentLogHostTableRowStatus_createAndGo;
      break;
    case L7_ROW_STATUS_CREATE_AND_WAIT:
      *val = D_agentLogHostTableRowStatus_createAndWait;
      break;
    case L7_ROW_STATUS_DESTROY:
      *val = D_agentLogHostTableRowStatus_destroy;
      break;
    default:
      /* unknown value */
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t snmpAgentLogHostTableRowStatusSet(L7_uint32 agentLogHostTableIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_ROW_STATUS_t temp_val = 0;
  
  switch (val)
  {
  case D_agentLogHostTableRowStatus_active:
    temp_val = L7_ROW_STATUS_ACTIVE;
    break;
  case D_agentLogHostTableRowStatus_notInService:
    temp_val = L7_ROW_STATUS_NOT_IN_SERVICE;
    break;
  case D_agentLogHostTableRowStatus_notReady:
    temp_val = L7_ROW_STATUS_NOT_READY;
    break;
  case D_agentLogHostTableRowStatus_createAndGo:
    temp_val = L7_ROW_STATUS_CREATE_AND_GO;
    break;
  case D_agentLogHostTableRowStatus_createAndWait:
    temp_val = L7_ROW_STATUS_CREATE_AND_WAIT;
    break;
  case D_agentLogHostTableRowStatus_destroy:
    temp_val = L7_ROW_STATUS_DESTROY;
    break;
  default:
    /* unknown value */
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbLogHostTableRowStatusSet(USMDB_UNIT_CURRENT, agentLogHostTableIndex, temp_val);
  }

  return rc;
}


L7_RC_t snmpAgentLogCliCommandsAdminStatusGet(L7_uint32 *CliCommandsAdminStatus )
{

   L7_RC_t rc = L7_FAILURE;
   L7_uint32 temp_val = 0;

   rc = usmDbCmdLoggerAdminModeGet(&temp_val);

   if(rc == L7_SUCCESS)
   {
     switch (temp_val)
     {
     case L7_ENABLE:
       *CliCommandsAdminStatus = D_agentLogCliCommandsAdminStatus_enable;
       break;

     case L7_DISABLE:
       *CliCommandsAdminStatus = D_agentLogCliCommandsAdminStatus_disable;
       break;

     default:
       rc = L7_FAILURE;
       break;
     }
   }

   return rc;
}

L7_RC_t snmpAgentLogCliCommandsAdminStatusSet (L7_uint32 CliCommandsAdminStatus)
{
   L7_RC_t rc = L7_SUCCESS;
   L7_uint32 temp_val = 0;

   switch (CliCommandsAdminStatus)
   {
   case D_agentLogCliCommandsAdminStatus_enable:
     temp_val = L7_ENABLE;
     break;

   case D_agentLogCliCommandsAdminStatus_disable:
     temp_val = L7_DISABLE;
     break;

   default:
     rc = L7_FAILURE;
     break;
   }

   if(rc == L7_SUCCESS)
   {
     rc = usmDbCmdLoggerAdminModeSet(temp_val);
   }

   return rc;
}
#if 0
L7_RC_t snmpAgentLogWebAdminStatusGet(L7_uint32 *WebLoggingAdminStatus )
{

   L7_RC_t rc = L7_FAILURE;
   L7_uint32 temp_val = 0;

   rc = usmDbCmdLoggerWebAdminModeGet(&temp_val);

   if(rc == L7_SUCCESS)
   {
     switch (temp_val)
     {
     case L7_ENABLE:
       *WebLoggingAdminStatus = D_agentLogWebAdminStatus_enable;
       break;

     case L7_DISABLE:
       *WebLoggingAdminStatus = D_agentLogWebAdminStatus_disable;
       break;

     default:
       rc = L7_FAILURE;
       break;
     }
   }

   return rc;
}

L7_RC_t snmpAgentLogWebAdminStatusSet (L7_uint32 WebLoggingAdminStatus)
{
   L7_RC_t rc = L7_SUCCESS;
   L7_uint32 temp_val = 0;

   switch (WebLoggingAdminStatus)
   {
   case D_agentLogWebAdminStatus_enable:
     temp_val = L7_ENABLE;
     break;

   case D_agentLogWebAdminStatus_disable:
     temp_val = L7_DISABLE;
     break;

   default:
     rc = L7_FAILURE;
     break;
   }

   if(rc == L7_SUCCESS)
   {
     rc = usmDbCmdLoggerWebAdminModeSet(temp_val);
   }

   return rc;
}

L7_RC_t snmpAgentLogSnmpAdminStatusGet(L7_uint32 *SnmpLoggingAdminStatus )
{

   L7_RC_t rc = L7_FAILURE;
   L7_uint32 temp_val = 0;

   rc = usmDbCmdLoggerSnmpAdminModeGet(&temp_val);

   if(rc == L7_SUCCESS)
   {
     switch (temp_val)
     {
     case L7_ENABLE:
       *SnmpLoggingAdminStatus = D_agentLogSnmpAdminStatus_enable;
       break;

     case L7_DISABLE:
       *SnmpLoggingAdminStatus = D_agentLogSnmpAdminStatus_disable;
       break;

     default:
       rc = L7_FAILURE;
       break;
     }
   }

   return rc;
}

L7_RC_t snmpAgentLogSnmpAdminStatusSet (L7_uint32 SnmpLoggingAdminStatus)
{
   L7_RC_t rc = L7_SUCCESS;
   L7_uint32 temp_val = 0;

   switch (SnmpLoggingAdminStatus)
   {
   case D_agentLogWebAdminStatus_enable:
     temp_val = L7_ENABLE;
     break;

   case D_agentLogWebAdminStatus_disable:
     temp_val = L7_DISABLE;
     break;

   default:
     rc = L7_FAILURE;
     break;
   }

   if(rc == L7_SUCCESS)
   {
     rc = usmDbCmdLoggerSnmpAdminModeSet(temp_val);
   }

   return rc;
}

L7_RC_t snmpAgentLogAuditAdminStatusGet(L7_uint32 *AuditAdminStatus )
{

   L7_RC_t rc = L7_FAILURE;
   L7_uint32 temp_val = 0;

   rc = usmDbCmdLoggerAuditAdminModeGet(&temp_val);

   if(rc == L7_SUCCESS)
   {
     switch (temp_val)
     {
     case L7_ENABLE:
       *AuditAdminStatus = D_agentLogAuditAdminStatus_enable;
       break;

     case L7_DISABLE:
       *AuditAdminStatus = D_agentLogAuditAdminStatus_disable;
       break;

     default:
       rc = L7_FAILURE;
       break;
     }
   }

   return rc;
}

L7_RC_t snmpAgentLogAuditAdminStatusSet (L7_uint32 AuditAdminStatus)
{
   L7_RC_t rc = L7_SUCCESS;
   L7_uint32 temp_val = 0;

   switch (AuditAdminStatus)
   {
   case D_agentLogAuditAdminStatus_enable:
     temp_val = L7_ENABLE;
     break;

   case D_agentLogAuditAdminStatus_disable:
     temp_val = L7_DISABLE;
     break;

   default:
     rc = L7_FAILURE;
     break;
   }

   if(rc == L7_SUCCESS)
   {
     rc = usmDbCmdLoggerAuditAdminModeSet(temp_val);
   }

   return rc;
}
#endif

L7_RC_t snmpAgentLogHostIpAddressTypeGet(L7_uint32 index,
                                         L7_int32 *addressType) 
{

   L7_RC_t rc = L7_FAILURE;
   L7_IP_ADDRESS_TYPE_t temp_val = 0;

   rc = usmDbLogHostTableAddressTypeGet(USMDB_UNIT_CURRENT, index, &temp_val);

   if(rc == L7_SUCCESS)
   {
     switch (temp_val)
     {
     case L7_IP_ADDRESS_TYPE_IPV4:
       *addressType = D_agentLogHostTableIpAddressType_ipv4;
       break;

     case L7_IP_ADDRESS_TYPE_DNS:
       *addressType = D_agentLogHostTableIpAddressType_dns;
       break;

     default:
       rc = L7_FAILURE;
       break;
     }
   }

   return rc;
}

L7_RC_t snmpAgentLogHostIpAddressTypeSet(L7_uint32 index,
                                         L7_int32 addressType) 
{

   L7_RC_t rc = L7_SUCCESS;
   L7_IP_ADDRESS_TYPE_t temp_val = 0;

   switch (addressType)
   {
     case D_agentLogHostTableIpAddressType_ipv4:
       temp_val = L7_IP_ADDRESS_TYPE_IPV4;
       break;

     case D_agentLogHostTableIpAddressType_dns:
       temp_val = L7_IP_ADDRESS_TYPE_DNS ;
       break;

     default:
       rc = L7_FAILURE;
       break;
   }

   if(rc == L7_SUCCESS)
   {
     rc = usmDbLogHostTableAddressTypeSet(USMDB_UNIT_CURRENT, index, temp_val);
   }

   return rc;
}

L7_RC_t snmpAgentLogEmailSubjectMessageTypeGet(L7_uint32 index,
                                               L7_uint32 msgType)
{
  if ((msgType == L7_LOG_EMAIL_ALERT_NON_URGENT) || (msgType == L7_LOG_EMAIL_ALERT_URGENT))
  {
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

L7_RC_t snmpAgentLogEmailSubjectMessageTypeNextGet(L7_uint32 index,
                                                  L7_uint32 *msgType)
{
  L7_uint32 i;

  for(i = *msgType; i <= 2; i++)
  {
    if (snmpAgentLogEmailSubjectMessageTypeGet(index, i) == L7_SUCCESS)
    {
      *msgType = i;
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

L7_RC_t snmpLogEmailAlertMailServerAddressGet(L7_uint32 index,
                                                   L7_uint32 type,
                                                   L7_char8  *mailServerAddr)
{
  return usmDbLogEmailAlertMailServerAddressGet(index, type, mailServerAddr);

}



L7_RC_t snmpLogEmailAlertNextMailServerAddressGet(L7_uint32 index,
                                                   L7_uint32 *type,
                                                   L7_char8 * mailServerAddr)
{
  if (strlen(mailServerAddr)== 0)
  {
    return usmDbLogEmailAlertFirstMailServerAddressGet(index, type, mailServerAddr);
  }
  else
  {
    return usmDbLogEmailAlertNextMailServerAddressGet(index, type, mailServerAddr);
  }
}
L7_RC_t snmpLogEmailAlertToAddrGet(L7_uint32 index,
                                   L7_uint32 type,
                                   L7_char8 * toAddr)
{
 return usmDbLogEmailAlertToAddrFind(type, toAddr);
}
L7_RC_t snmpLogEmailAlertNextToAddrGet(L7_uint32 index,
                                   L7_uint32 * type,
                                   L7_char8 * toAddr)
{
 return usmDbLogEmailAlertNextToAddrGet(index, type, toAddr);
}
/***************************************************************************************/


