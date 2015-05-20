/*********************************************************************
* <pre>
* LL   VV  VV LL   7777777  (C) Copyright LVL7 Systems 2002-2006
* LL   VV  VV LL   7   77   All Rights Reserved.
* LL   VV  VV LL      77
* LL    VVVV  LL     77
* LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
* </pre>
**********************************************************************
*
* @filename   macal_sid_const.h
*
* @purpose    Contians constants for Management Access Control and Administration List
*
* @component  Management Access Control and Administration List
*
* @comments   none
*
* @create     05/05/2005
*
* @author     stamboli
* @end
*
**********************************************************************/
#ifndef INCLUDE_MACAL_SID_CONST_H
#define INCLUDE_MACAL_SID_CONST_H


/*
***********************************************************************
*                          CONSTANTS
***********************************************************************
*/

enum 
{
  FD_CNFGR_MACAL_DEFAULT_STACK_SIZE     = L7_DEFAULT_STACK_SIZE,
  FD_CNFGR_MACAL_DEFAULT_TASK_SLICE     = L7_DEFAULT_TASK_SLICE,
  FD_CNFGR_MACAL_DEFAULT_TASK_PRI       = L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)
};


#endif /* INCLUDE_MACAL_SID_CONST_H */

