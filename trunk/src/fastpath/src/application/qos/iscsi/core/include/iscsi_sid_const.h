/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   iscsi_sid_const.h
*
* @purpose    Contians constants for ISCSI
*
* @component  cnfgr
*
* @comments   none
*
* @create     04/18/2008
* @end
*
**********************************************************************/
#ifndef INCLUDE_ISCSI_SID_CONST_H
#define INCLUDE_ISCSI_SID_CONST_H


/*
***********************************************************************
*                          CONSTANTS
***********************************************************************
*/

enum
{
  FD_CNFGR_ISCSI_DEFAULT_STACK_SIZE     = L7_DEFAULT_STACK_SIZE,
  FD_CNFGR_ISCSI_DEFAULT_TASK_SLICE     = L7_DEFAULT_TASK_SLICE,
  FD_CNFGR_ISCSI_DEFAULT_TASK_PRI       = L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)
};

#endif /* INCLUDE_ISCSI_SID_CONST_H */
