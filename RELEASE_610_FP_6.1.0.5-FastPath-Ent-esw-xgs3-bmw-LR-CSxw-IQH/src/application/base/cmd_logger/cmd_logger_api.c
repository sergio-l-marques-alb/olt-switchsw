
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  cmd_logger_api.c
*
* @purpose   command logger api functions
*
* @component cmdLogger component
*
* @comments  none
*
* @create    25-Oct-2004
*
* @author    gaunce
*
* @end
*             
**********************************************************************/

#include "cmd_logger_include.h"
#include "log.h"

extern cmdLoggerCfgData_t cmdLoggerCfgData;


/*********************************************************************
* @purpose  Returns the Command Logger Admin Mode 
*
* @param    void  
*
* @returns  mode  Command Logger Admin Mode
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 
cmdLoggerAdminModeGet(void)
{
  return(cmdLoggerCfgData.adminMode);
}

/*********************************************************************
* @purpose  Sets the Command Logger Admin Mode
*
* @param    void  
*
* @returns  L7_SUCCESS
*
* @notes    none 
*       
* @end
*********************************************************************/
void 
cmdLoggerAdminModeSet(L7_uint32 mode)
{
  if (cmdLoggerCfgData.adminMode != mode)
  {
    cmdLoggerCfgData.adminMode = mode;
    cmdLoggerCfgData.cfgHdr.dataChanged = L7_TRUE;
  }
}


/*********************************************************************
* @purpose  Add a Command Logging Entry 
*
* @param    L7_char8  *strHistory
*
* returns   nothing
*
* @notes
*
* @end
*********************************************************************/
void cmdLoggerEntryAdd(L7_char8 *strHistory)
{
  if (cmdLoggerCfgData.adminMode == L7_ENABLE)
  {
    L7_LOGNC(cmdLoggerCfgData.severity, strHistory);
  }
}

/*********************************************************************
* @purpose  Returns the Command Logger Web Admin Mode
*
* @param    void
*
* @returns  mode  Command Logger Web Admin Mode
*
* @notes    none
* 
* @end
*********************************************************************/
L7_uint32
cmdLoggerWebAdminModeGet(void)
{
  return(cmdLoggerCfgData.webAdminMode);
} 
    
/*********************************************************************
* @purpose  Sets the Command Logger Web Admin Mode
*
* @param    void
*
* @returns  L7_SUCCESS
* 
* @notes    none
*           
* @end
*********************************************************************/
void
cmdLoggerWebAdminModeSet(L7_uint32 mode)
{
  if (cmdLoggerCfgData.webAdminMode != mode)
  {
    cmdLoggerCfgData.webAdminMode = mode;
    cmdLoggerCfgData.cfgHdr.dataChanged = L7_TRUE;
  }
} 

/*********************************************************************
* @purpose  Add a Web Session Logging Entry
*
* @param    L7_char8  *strHistory
*
* returns   nothing
*
* @notes
*
* @end
*********************************************************************/
void cmdLoggerWebEntryAdd(L7_char8 *strHistory)
{
  if (cmdLoggerCfgData.webAdminMode == L7_ENABLE)
  {
    L7_LOGNC(cmdLoggerCfgData.webSeverity , strHistory);
  }
}

/*********************************************************************
* @purpose  Returns the Command Logger SNMP Admin Mode
*
* @param    void
*
* @returns  mode  Command Logger SNMP Admin Mode
*
* @notes    none
* 
* @end
*********************************************************************/
L7_uint32
cmdLoggerSnmpAdminModeGet(void)
{
  return(cmdLoggerCfgData.snmpAdminMode);
}

/*********************************************************************
* @purpose  Sets the Command Logger Snmp Admin Mode
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes    none
* 
* @end
*********************************************************************/
void
cmdLoggerSnmpAdminModeSet(L7_uint32 mode)
{
  if (cmdLoggerCfgData.snmpAdminMode != mode)
  {
    cmdLoggerCfgData.snmpAdminMode = mode;
    cmdLoggerCfgData.cfgHdr.dataChanged = L7_TRUE;
  }
}

/*********************************************************************
* @purpose  Add a Snmp Command Logging Entry
*
* @param    L7_char8  *strHistory
*
* returns   nothing
*
* @notes
*
* @end
*********************************************************************/
void cmdLoggerSnmpEntryAdd(L7_char8 *strHistory)
{
  if (cmdLoggerCfgData.snmpAdminMode == L7_ENABLE)
  {
    L7_LOGNC(cmdLoggerCfgData.snmpSeverity, strHistory);
  }
}

