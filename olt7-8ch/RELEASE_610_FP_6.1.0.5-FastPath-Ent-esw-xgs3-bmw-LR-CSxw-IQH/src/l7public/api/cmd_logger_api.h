/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  cmd_logger_api.h
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

/*************************************************************
                    
*************************************************************/



#ifndef CMD_LOGGER_API_H
#define CMD_LOGGER_API_H

#include "l7_common.h"

/* Begin Function Prototypes */

/*********************************************************************
* @purpose  Returns the Unit's Command Logger Mode 
*
* @param    void  
*
* @returns  mode  Command Logger Mode
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 
cmdLoggerAdminModeGet(void);

/*********************************************************************
* @purpose  Sets the Unit's Command Logger Mode  
*
* @param    mode  Command Logger Mode  
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
void 
cmdLoggerAdminModeSet(L7_uint32 mode);



/*********************************************************************
* @purpose  Add a Command Logger Entry  
*
* @param    strHistory  String  
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
void 
cmdLoggerEntryAdd(L7_char8 *strHistory);

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
cmdLoggerWebAdminModeGet(void);

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
cmdLoggerWebAdminModeSet(L7_uint32 mode);

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
void cmdLoggerWebEntryAdd(L7_char8 *strHistory);

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
cmdLoggerSnmpAdminModeGet(void);

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
cmdLoggerSnmpAdminModeSet(L7_uint32 mode);

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
void cmdLoggerSnmpEntryAdd(L7_char8 *strHistory);

/* End Function Prototypes */
#endif /* CMD_LOGGER_API_ */
