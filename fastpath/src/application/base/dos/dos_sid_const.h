/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   dos_sid_const.h
*
* @purpose    Contians constants for DOS
*
* @component  cnfgr
*
* @comments   none
*
* @create     04/13/2007
*
* @author     aprashant
* @end
*
**********************************************************************/
#ifndef INCLUDE_DOS_SID_CONST_H
#define INCLUDE_DOS_SID_CONST_H


/*
***********************************************************************
*                          CONSTANTS
***********************************************************************
*/

enum
{
  FD_CNFGR_DOS_DEFAULT_STACK_SIZE     = L7_DEFAULT_STACK_SIZE,
  FD_CNFGR_DOS_DEFAULT_TASK_SLICE     = L7_DEFAULT_TASK_SLICE,
  FD_CNFGR_DOS_DEFAULT_TASK_PRI       = L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)
};

#endif /* INCLUDE_DOS_SID_CONST_H */
