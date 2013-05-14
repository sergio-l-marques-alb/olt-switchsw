/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename sslt_sid_const.h
*
* @purpose Contains constants for SSL Tunnel
*
* @component sslt
*
* @comments none
*
* @create 10/22/2003
*
* @author spetriccione
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_SSLT_SID_CONST_H
#define INCLUDE_SSLT_SID_CONST_H

/*
***********************************************************************
*                          CONSTANTS
***********************************************************************
*/

enum 
{
  FD_CNFGR_SSLT_DEFAULT_STACK_SIZE     = L7_DEFAULT_STACK_SIZE,
  FD_CNFGR_SSLT_DEFAULT_TASK_SLICE     = L7_DEFAULT_TASK_SLICE,
  FD_CNFGR_SSLT_DEFAULT_TASK_PRI       = L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)
};


#endif /* INCLUDE_SSLT_SID_CONST_H */

