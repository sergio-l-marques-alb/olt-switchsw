/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   dot1x_sid_const.h
*
* @purpose    Contians constants for dot1x
*
* @component  cnfgr
*
* @comments   none
*
* @create     08/07/2003
*
* @author     mfiorito
* @end
*
**********************************************************************/
#ifndef INCLUDE_DOT1X_SID_CONST_H
#define INCLUDE_DOT1X_SID_CONST_H


/*
***********************************************************************
*                          CONSTANTS
***********************************************************************
*/

enum 
{
  FD_CNFGR_DOT1X_DEFAULT_STACK_SIZE     = L7_DEFAULT_STACK_SIZE,
  FD_CNFGR_DOT1X_DEFAULT_TASK_SLICE     = L7_DEFAULT_TASK_SLICE,
  FD_CNFGR_DOT1X_DEFAULT_TASK_PRI       = L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)
};


#endif /* INCLUDE_DOT1X_SID_CONST_H */

