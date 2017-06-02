/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   lldp_sid.h
*
* @purpose    Contians constants for 802.1AB
*
* @component  cnfgr
*
* @comments   none
*
* @create     02/01/2005
*
* @author     dfowler
* @end
*
**********************************************************************/
#ifndef INCLUDE_LLDP_SID_CONST_H
#define INCLUDE_LLDP_SID_CONST_H


/*
***********************************************************************
*                          CONSTANTS
***********************************************************************
*/

enum
{
  FD_CNFGR_LLDP_DEFAULT_STACK_SIZE     = L7_DEFAULT_STACK_SIZE,
  FD_CNFGR_LLDP_DEFAULT_TASK_SLICE     = L7_DEFAULT_TASK_SLICE,
  FD_CNFGR_LLDP_DEFAULT_TASK_PRI       = L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)
};

#endif /* INCLUDE_LLDP_SID_CONST_H */
