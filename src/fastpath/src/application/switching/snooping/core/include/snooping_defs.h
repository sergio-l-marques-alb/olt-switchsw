/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   snooping_defs.h
*
* @purpose    Contains platform specific parameters
*
* @component  Snooping
*
* @comments   The constants defined in this file are to modify the
*             maximum number timers supported. Changes in these 
*             constant can affect the memory usage.
*
* @create     07-Dec-2006
*
* @author     drajendra
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_SNOOPING_DEFS_H
#define INCLUDE_SNOOPING_DEFS_H

/* Set MGMD integration */
#define PTIN_SNOOP_USE_MGMD 1

/* Message Queues */
#define SNOOP_QUEUE           "snoop_Queue"
#define SNOOP_TIMER_QUEUE     "snoop_Timer_Queue"
#define SNOOP_MLD_PDU_QUEUE   "snoop_MLD_PDU_Queue"
#define SNOOP_IGMP_PDU_QUEUE  "snoop_IGMP_PDU_Queue"


/* Group Membership Timers */
#define SNOOP_MAX_GRP_TIMERS        (L7_MAX_GROUP_REGISTRATION_ENTRIES * \
                                     L7_IGMP_SNOOPING_MAX_INTF)
#define SNOOP_NUM_GRP_TIMERS       (SNOOP_MAX_GRP_TIMERS/16)

/* MRTR Timers */    
/* definition of SNOOP_MAX_MRTR_TIMERS is moved to l7_resources.h */
                                      
#define SNOOP_NUM_MRTR_TIMERS       (SNOOP_MAX_MRTR_TIMERS)                                      


/* Querier timers */       /* Other Querier Timers + Number of Querier vlans */
#define SNOOP_MAX_QUERIER_TIMERS    (L7_MAX_VLANS + SNOOP_QUERIER_VLAN_MAX)

#define SNOOP_NUM_QUERIER_TIMERS    (SNOOP_MAX_QUERIER_TIMERS)
                                      
#define SNOOP_NUM_TIMERS_PER_INSTANCE  SNOOP_NUM_MRTR_TIMERS+ \
                                       SNOOP_NUM_QUERIER_TIMERS

#define SNOOP_TIMER_INTERVAL         1000  /* milliseconds */

#define SNOOP_INSTANCES_MAX          2

#define SNOOP_ENTRY_TIMER_BLOCKS_COUNT    L7_MAX_GROUP_REGISTRATION_ENTRIES

#endif /* INCLUDE_SNOOPING_DEFS_H */
