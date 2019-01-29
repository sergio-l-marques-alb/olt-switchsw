/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   ipmap_sid.h
*
* @purpose    Contians constants for Igmp Snooping
*
* @component  ipmap
*
* @comments   none
*
* @create     07/02/2003
*
* @author     markl
* @end
*
**********************************************************************/
#ifndef INCLUDE_IPMAP_SID_CONST_H
#define INCLUDE_IPMAP_SID_CONST_H


/*
***********************************************************************
*                          CONSTANTS
***********************************************************************
*/

enum 
{
  FD_IP_FORWARDING_DEFAULT_STACK_SIZE     = (L7_L3_DEFAULT_STACK_SIZE * 2),
  FD_IP_FORWARDING_DEFAULT_TASK_SLICE     = L7_L3_DEFAULT_TASK_SLICE,
  FD_IP_FORWARDING_DEFAULT_TASK_PRI       = L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY) 
};

enum
{
  FD_IP_PROCESSING_DEFAULT_STACK_SIZE     = (L7_L3_DEFAULT_STACK_SIZE * 2),
  FD_IP_PROCESSING_DEFAULT_TASK_SLICE     = L7_L3_DEFAULT_TASK_SLICE,
  FD_IP_PROCESSING_DEFAULT_TASK_PRI       = L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY) 
};

enum
{
  FD_IP_ARP_DEFAULT_STACK_SIZE     = L7_L3_DEFAULT_STACK_SIZE,
  FD_IP_ARP_DEFAULT_TASK_SLICE     = L7_L3_DEFAULT_TASK_SLICE,
  FD_IP_ARP_DEFAULT_TASK_PRI       = L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)
};

#endif /* INCLUDE_IPMAP_SID_CONST_H */
