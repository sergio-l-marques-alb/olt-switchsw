/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   pml_sid.h
*
* @purpose    Contians constants for Port MAC Locking
*
* @component  cnfgr
*
* @comments   none
*
* @create     05/21/2004
*
* @author     colinw
* @end
*
**********************************************************************/
#ifndef INCLUDE_PML_SID_CONST_H
#define INCLUDE_PML_SID_CONST_H


/*
***********************************************************************
*                          CONSTANTS
***********************************************************************
*/

enum 
{
  FD_CNFGR_PML_DEFAULT_STACK_SIZE     = L7_DEFAULT_STACK_SIZE,
  FD_CNFGR_PML_DEFAULT_TASK_SLICE     = L7_DEFAULT_TASK_SLICE,
  FD_CNFGR_PML_DEFAULT_TASK_PRI       = L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)
};


#endif /* INCLUDE_PML_SID_CONST_H */

