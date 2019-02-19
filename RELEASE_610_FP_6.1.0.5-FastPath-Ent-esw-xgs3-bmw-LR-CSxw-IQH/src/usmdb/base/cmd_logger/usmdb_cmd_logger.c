/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @file     usmdb_cmd_logger.c
*
* @purpose   USMDB Command Logger definitions
*
* @component USMDB
*
* @comments
*
* create 2/27/2007
*
* author nshrivastav
*
**********************************************************************/

#include "l7_common.h"
#include "usmdb_cmd_logger_api.h"
#include "cmd_logger_api.h"

/*********************************************************************
* @purpose  Return the Command Logger Admin Mode for the Switch
*
* @param   UnitIndex  (input)    the unit for this operation
* @param   *adminMode (output)   the command logger admin mode
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbCmdLoggerAdminModeGet(L7_uint32 *adminMode)
{
  *adminMode = cmdLoggerAdminModeGet();
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the Command Logger Admin Mode for the Switch
*
* @param   UnitIndex  (input)    the unit for this operation
* @param   adminMode (output)    the command logger admin mode
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbCmdLoggerAdminModeSet(L7_uint32 adminMode)
{
  cmdLoggerAdminModeSet(adminMode);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Validate the current user configuration and Log CLI commands
*
* @param    L7_uint32 unit
* @param    L7_char8  *strHistory
*
* returns   L7_SUCCESS
*
* returns   L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbCmdLoggerEntryAdd(L7_char8 *strHistory)
{
  cmdLoggerEntryAdd(strHistory);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Return the Command Logger Web Admin Mode for the Switch
*
* @param   UnitIndex  (input)    the unit for this operation
* @param   *webAdminMode (output)   the command logger admin mode
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbCmdLoggerWebAdminModeGet(L7_uint32 *webAdminMode)
{
  *webAdminMode = cmdLoggerWebAdminModeGet();
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the Command Logger Web Admin Mode for the Switch
*
* @param   UnitIndex  (input)    the unit for this operation
* @param   webAdminMode (output)    the command logger admin mode
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbCmdLoggerWebAdminModeSet(L7_uint32 webAdminMode)
{
  cmdLoggerWebAdminModeSet(webAdminMode);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Validate the current user configuration and Log Web session
*           logins and logouts
*
* @param    L7_uint32 unit
* @param    L7_char8  *strHistory
*
* returns   L7_SUCCESS
*
* returns   L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbCmdLoggerWebEntryAdd(L7_char8 *strHistory)
{
  cmdLoggerWebEntryAdd(strHistory);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Return the Command Logger Snmp Admin Mode for the Switch
*
* @param   UnitIndex  (input)    the unit for this operation
* @param   *snmpAdminMode (output)   the command logger admin mode
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbCmdLoggerSnmpAdminModeGet(L7_uint32 *snmpAdminMode)
{
  *snmpAdminMode = cmdLoggerSnmpAdminModeGet();
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the Command Logger Web Admin Mode for the Switch
*
* @param   UnitIndex  (input)    the unit for this operation
* @param   snmpAdminMode (output)    the command logger admin mode
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbCmdLoggerSnmpAdminModeSet(L7_uint32 snmpAdminMode)
{
  cmdLoggerSnmpAdminModeSet(snmpAdminMode);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Validate the current user configuration and Log Snmp login
*           and set commands
*
* @param    L7_uint32 unit
* @param    L7_char8  *strHistory
*
* returns   L7_SUCCESS
*
* returns   L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbCmdLoggerSnmpEntryAdd(L7_char8 *strHistory)
{
  cmdLoggerSnmpEntryAdd(strHistory);
  return L7_SUCCESS;
}

