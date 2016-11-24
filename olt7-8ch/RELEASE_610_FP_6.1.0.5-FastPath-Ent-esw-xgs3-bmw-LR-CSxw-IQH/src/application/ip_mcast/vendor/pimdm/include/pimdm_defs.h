/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_defs.h
*
* @purpose     
*
* @component  PIM-DM
*
* @comments   none
*
* @create      
*
* @author     Krishna Mohan CS
* @end
*
**********************************************************************/

#ifndef _PIMDM_DEFS_H
#define _PIMDM_DEFS_H

/*******************************************************************************
**                             Includes                                       **
*******************************************************************************/
#include "l3_addrdefs.h"
#include "l3_mcast_commdefs.h"
#include "l3_mcast_defaultconfig.h"
#include "mcast_bitset.h"
#include "pim_defs.h"

/*******************************************************************************
**                        Implementation Specific Definitions                 **
*******************************************************************************/

/* Scalable parameters */
#define PIMDM_MAX_NEIGHBORS               L7_PIMDM_MAX_NBR

#define PIMDM_MRT_MAX_IPV4_ROUTE_ENTRIES  L7_PIMDM_MAX_MRT_IPV4_TABLE_SIZE
#define PIMDM_MRT_MAX_IPV6_ROUTE_ENTRIES  L7_PIMDM_MAX_MRT_IPV6_TABLE_SIZE

#define PIMDM_MAX_LOCAL_MEMB_ENTRIES      (L7_MGMD_GROUPS_MAX_ENTRIES + 1)
#define PIMDM_MAX_INTERFACES              MCAST_MAX_INTERFACES
#define PIMDM_INTF_BIT_SIZE               MCAST_BITX_NUM_BITS(PIMDM_MAX_INTERFACES+1)
#define PIMDM_NUM_OPTIMAL_OUTGOING_INTERFACES L7_PIM_NUM_OPTIMAL_OUTGOING_INTERFACES

/* Protocol Specific parameters */
#define PIMDM_MAX_ADMIN_SCOPE_ENTRIES   L7_MCAST_MAX_ADMINSCOPE_ENTRIES
#define PIMDM_MAX_DBG_MSG_SIZE          256
#define PIMDM_MAX_DBG_ADDR_SIZE         IPV6_DISP_ADDR_LEN
#define PIMDM_MRT_TREE_TYPE             0x10
#define PIMDM_MAX_PDU                   L7_MULTICAST_MAX_IP_MTU
#define PIMDM_MRT_ENTRY_HOLDTIME        210 /* in seconds */  
#define PIMDM_MAX_BEST_ROUTE_CHANGES    L7_PIM_MAX_BEST_ROUTE_CHANGES
#define PIMDM_MFC_WRONG_IF_RATE_LIMIT_THRESHOLD  50

/*******************************************************************************
**                        Protocol Specific Definitions                       **
*******************************************************************************/
/* PIM-DM Protocol Version Related Definitions */
#define PIMDM_MRT_ENTRY_TIMER_TIMEOUT     210 /* in seconds */
#define PIMDM_PRUNE_LIMIT_TIMER_TIMEOUT   210 /* in seconds */
#define PIMDM_GRAFT_RETRY_TIMER_TIMEOUT   3 /* in seconds */  
#define PIMDM_PRUNE_HOLD_TIME             60 /* in seconds TBD: Think of a better value*/ 
#define PIMDM_DEFAULT_REFRESH_INTERVAL    60 /* in seconds */     
#define PIMDM_DEFAULT_SOURCE_LIFETIME     210 /* in seconds */   
#define PIMDM_ASSERT_METRIC_INFINITY      0xFFFFFFFF 
#define PIMDM_DEFAULT_ASSERT_TIMEOUT      180 /* in seconds */ 
#define PIMDM_ASSERT_OVERRIDE_DELAY       10

#define PIMDM_INFINITE_HOLDTIME           0xFFFF
#define PIMDM_DEFAULT_CTRL_PKT_TTL        1  /* Default TTL in PIM-DM Control
                                              * Packets */
/* PIM-DM Hello Message Processing Related Definitions */
#define PIMDM_STATEREFRESH_VERSION        1    /* State refresh version supported */
#define PIMDM_HELLO_PKT_VER_TYPE          0x20 /* Section 4.7.1  of RFC3973 */

#define PIMDM_HELLO_OPTION_HOLDTIME         1  /* Section 4.7.5.1 of RFC3973 */
#define PIMDM_HELLO_OPTION_LANPRUNEDELAY    2  /* Section 4.7.5.2 of RFC3973 */
#define PIMDM_HELLO_OPTION_GENID            20  /* Section 4.7.5.3 of RFC3973 */
#define PIMDM_HELLO_OPTION_STATEREFRESH     21  /* Section 4.7.5.4 of RFC3973 */
#define PIMDM_HELLO_OPTIONLEN_HOLDTIME      2  /* Section 4.7.5.1 of RFC3973 */
#define PIMDM_HELLO_OPTIONLEN_LANPRUNEDELAY 4  /* Section 4.7.5.2 of RFC3973 */
#define PIMDM_HELLO_OPTIONLEN_GENID         4  /* Section 4.7.5.3 of RFC3973 */
#define PIMDM_HELLO_OPTIONLEN_STATEREFRESH  4  /* Section 4.7.5.4 of RFC3973 */

/*******************************************************************************
**                        Data Structure Definitions                          **
*******************************************************************************/
/* None */
#endif /* _PIMDM_DEFS_H */
