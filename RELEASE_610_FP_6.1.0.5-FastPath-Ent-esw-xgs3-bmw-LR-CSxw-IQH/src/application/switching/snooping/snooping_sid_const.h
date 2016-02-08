/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   snooping_sid.h
*
* @purpose    Contians constants for Snooping
*
* @component  Snooping
*
* @comments   none
*
* @create     07/02/2003
*
* @author     markl
* @end
*
**********************************************************************/
#ifndef INCLUDE_SNOOPING_SID_CONST_H
#define INCLUDE_SNOOPING_SID_CONST_H


/*
***********************************************************************
*                          CONSTANTS
***********************************************************************
*/

enum 
{
  FD_CNFGR_SNOOPING_DEFAULT_STACK_SIZE     = L7_DEFAULT_STACK_SIZE,
  FD_CNFGR_SNOOPING_DEFAULT_TASK_SLICE     = L7_DEFAULT_TASK_SLICE,
  FD_CNFGR_SNOOPING_DEFAULT_TASK_PRI       = L7_TASK_PRIORITY_LEVEL(L7_MEDIUM_TASK_PRIORITY)      /* PTin modified */
};


#endif /* INCLUDE_SNOOPING_SID_CONST_H */

