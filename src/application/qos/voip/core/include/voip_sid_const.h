/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   voip_sid_const.h
*
* @purpose    Contians constants for VOIP
*
* @component  cnfgr
*
* @comments   none
*
* @create     05/12/2007
*
* @author     aprashant
* @end
*
**********************************************************************/
#ifndef INCLUDE_VOIP_SID_CONST_H
#define INCLUDE_VOIP_SID_CONST_H


/*
***********************************************************************
*                          CONSTANTS
***********************************************************************
*/

enum
{
  FD_CNFGR_VOIP_DEFAULT_STACK_SIZE     = L7_DEFAULT_STACK_SIZE,
  FD_CNFGR_VOIP_DEFAULT_TASK_SLICE     = L7_DEFAULT_TASK_SLICE,
  FD_CNFGR_VOIP_DEFAULT_TASK_PRI       = L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)
};

#endif /* INCLUDE_VOIP_SID_CONST_H */
