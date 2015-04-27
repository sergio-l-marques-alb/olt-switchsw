/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   dns_client_const.h
*
* @purpose    Contains constants for DNS Client
*
* @component  DNS Client
*
* @comments   none
*
* @create     02/28/2005
*
* @author     dfowler
* @end
*
**********************************************************************/
#ifndef INCLUDE_DNS_CLIENT_SID_CONST_H
#define INCLUDE_DNS_CLIENT_SID_CONST_H


/*
***********************************************************************
*                          CONSTANTS
***********************************************************************
*/

enum 
{
  FD_CNFGR_DNS_CLIENT_DEFAULT_STACK_SIZE     = L7_DEFAULT_STACK_SIZE,
  FD_CNFGR_DNS_CLIENT_DEFAULT_TASK_SLICE     = L7_DEFAULT_TASK_SLICE,
  FD_CNFGR_DNS_CLIENT_DEFAULT_TASK_PRI       = L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)
};


#endif /* INCLUDE_DNS_CLIENT_SID_CONST_H */

