/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   sid_sid.h
*
* @purpose    Contians constants for sshd
*
* @component  cnfgr
*
* @comments   none
*
* @create     10/13/2003
*
* @author     mfiorito
* @end
*
**********************************************************************/
#ifndef INCLUDE_SSHD_SID_CONST_H
#define INCLUDE_SSHD_SID_CONST_H

#include "l7_product.h"


/*
***********************************************************************
*                          CONSTANTS
***********************************************************************
*/

enum
{
  FD_CNFGR_SSHD_DEFAULT_STACK_SIZE     = L7_DEFAULT_STACK_SIZE,
  FD_CNFGR_SSHD_DEFAULT_TASK_SLICE     = L7_DEFAULT_TASK_SLICE,
  FD_CNFGR_SSHD_DEFAULT_TASK_PRI       = L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)
};


#endif /* INCLUDE_SSHD_SID_CONST_H */

