/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @file     usmdb_cmd_logger_api.h
*
* @purpose   Command Logger API declarations
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

#ifndef __USMDB_CMD_LOGGER_API_H__
#define __USMDB_CMD_LOGGER_API_H__

#include "l7_common.h"
#include "usmdb_log_api.h"
#include "osapi.h"
#include "default_cnfgr.h"

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
L7_RC_t usmDbCommandLogger(L7_uint32 unit, L7_char8 *strHistory);

/*********************************************************************
* @purpose  Validate the current user configuration and Log CLI commands
*
* @param    L7_uint32  unit
* @param    L7_char8   *strHistory
* @param    L7_uint32  sessionId
*
* returns   L7_SUCCESS
*
* returns   L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbCommandLoggerSession(L7_uint32 unit, L7_char8 *strHistory, L7_uint32 sessionId);
/***** Declarations for the USMDB calls for command logger *****/ 

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
L7_RC_t usmDbCmdLoggerAdminModeGet(L7_uint32 *adminMode);

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
L7_RC_t usmDbCmdLoggerAdminModeSet(L7_uint32 adminMode);

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
L7_RC_t usmDbCmdLoggerEntryAdd(L7_char8 *strHistory);

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
L7_RC_t usmDbCmdLoggerWebAdminModeGet(L7_uint32 *webAdminMode);

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
L7_RC_t usmDbCmdLoggerWebAdminModeSet(L7_uint32 webAdminMode);

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
L7_RC_t usmDbCmdLoggerWebEntryAdd(L7_char8 *strHistory);

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
L7_RC_t usmDbCmdLoggerSnmpAdminModeGet(L7_uint32 *snmpAdminMode);

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
L7_RC_t usmDbCmdLoggerSnmpAdminModeSet(L7_uint32 snmpAdminMode);

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
L7_RC_t usmDbCmdLoggerSnmpEntryAdd(L7_char8 *strHistory);

/*********************************************************************
* @purpose  Returns the Switch Auditing Admin Mode 
*
* @param   L7_uint32 *mode (output) - the switch auditing admin mode
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbCmdLoggerAuditAdminModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Sets the Switch Auditing Admin Mode
*
* @param    L7_uint32  mode  - New switch auditing admin mode 
*
* returns   L7_SUCCESS
*           L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbCmdLoggerAuditAdminModeSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Add Auditing Entry 
*
* @param    L7_char8  *strEntry - message string
*
* @param    L7_uint32 component - component ID
*
* returns   nothing
*
* @notes
*
* @end
*********************************************************************/
void usmDbCmdLoggerAuditEntryAdd(L7_uint32 component, L7_char8 *strEntry);

#endif
