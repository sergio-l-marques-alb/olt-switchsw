/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsmdefs.h
*
* @purpose  includes all common stuff for entire pimsm component.
*
* @component pimsm
*
* @comments
*
* @create 01/01/2006
*
* @author dsatyanarayana
* @end
*
**********************************************************************/
#ifndef _PIMSMDEFS_H_
#define _PIMSMDEFS_H_
#include <stdlib.h> /* rand() */
#include "rtip.h"
#include "l7_common.h"
#include "l3_commdefs.h"
#include "l7_common_l3.h"
#include "l7_pimsm_api.h"

#include "intf_bitset.h"
#include "pimsmdebug.h"

/* Maximum buff size to send or receive packet */
#define PIMSM_PKT_SIZE_MAX                   L7_MULTICAST_MAX_IP_MTU  

#include "mcast_wrap.h"
#include "pimsmmain.h"
#include "pimsmproto.h"
#include "l7handle_api.h"
#define PIMSM_RANDOM()                       rand()
#define PIMSM_DEFAULT_MCAST_TTL              1
#define PIMSM_DEFAULT_UCAST_TTL              64
#define PIMSM_AVL_TREE_TYPE                  0x10


/* Tree sizes dependent on l3_platform.h */
#define PIMSM_S_G_IPV4_TBL_SIZE_TOTAL          platIpv4McastRoutesMaxGet()
#define PIMSM_S_G_IPV6_TBL_SIZE_TOTAL          platIpv6McastRoutesMaxGet()
#define PIMSM_S_G_RPT_IPV4_TBL_SIZE_TOTAL      platIpv4McastRoutesMaxGet()
#define PIMSM_S_G_RPT_IPV6_TBL_SIZE_TOTAL      platIpv6McastRoutesMaxGet()
#define PIMSM_STAR_G_IPV4_TBL_SIZE_TOTAL       platIpv4McastRoutesMaxGet()
#define PIMSM_STAR_G_IPV6_TBL_SIZE_TOTAL       platIpv6McastRoutesMaxGet()
#define PIMSM_STAR_STAR_RP_TBL_SIZE_TOTAL      L7_PIMSM_MAX_RP_GRP_ENTRIES
#define PIMSM_RP_GRP_ENTRIES_MAX               L7_PIMSM_MAX_RP_GRP_ENTRIES
#define PIMSM_CAND_RP_GROUPS_MAX               L7_PIMSM_MAX_CAND_RP_NUM

/* Implementation specific constants to have upper limits */
#define PIMSM_BSR_FRAGMENT_RP_ENTRIES_MAX    L7_PIMSM_MAX_RP_GRP_ENTRIES
#define PIMSM_BSR_FRAGMENT_GRP_ENTRIES_MAX   L7_PIMSM_MAX_RP_GRP_ENTRIES
#define PIMSM_BSR_RP_NODES_MAX               L7_PIMSM_MAX_RP_GRP_ENTRIES
#define PIMSM_CANDIDATE_RP_GRP_MAX           L7_PIMSM_MAX_CAND_RP_NUM
#define PIMSM_BSM_PKT_FRAGMENT_COUNT_MAX      1
#define PIMSM_BSR_SCOPE_NODES_MAX            L7_PIMSM_MAX_PER_SCOPE_BSR_NODES
#define PIMSM_MFC_WRONG_IF_RATE_LIMIT_THRESHOLD  10

#define PIMSM_MAX_NBR                        L7_PIMSM_MAX_NBR 
/* some constants derived from  l3_mcast_commdefs.h*/
#define PIMSM_BEST_ROUTE_CHANGES_MAX         L7_PIM_MAX_BEST_ROUTE_CHANGES
#define PIMSM_DEFAULT_HELLO_PERIOD           L7_PIMSM_INTERFACE_HELLO_INTERVAL
#define PIMSM_DEFAULT_JOIN_PRUNE_PERIOD      L7_PIMSM_INTERFACE_JOIN_PRUNE_INTERVAL 


#define PIMSM_NUM_OPTIMAL_OUTGOING_INTERFACES  L7_PIM_NUM_OPTIMAL_OUTGOING_INTERFACES

#define PIMSM_MAX_KERNEL_CACHE_ENTRIES_PER_STAR_G_ENTRY 256


#define PIMSM_MAX_JP_PACKETS        MCAST_MAX_INTERFACES

/*
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |PIM Ver| Type  |   Reserved    |           Checksum            |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |        Upstream Neighbor Address (Encoded-Unicast format)     |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |  Reserved     | Num groups    |          Holdtime             |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |         Multicast Group Address 1 (Encoded-Group format)      |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |   Number of Joined Sources    |   Number of Pruned Sources    |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


 #define PIMSM_MAX_JP_MSG_SIZE    \
   (L7_MULTICAST_MAX_IP_MTU - 4 - 16 - 4 - 
         PIMSM_MAX_JP_NUM_GROUPS * (20 - 4 - PIMSM_MAX_ADDRS_IN_LIST * 20))
    20 = sizeof(pim_encod_src_addr_t) , 20 = sizeof(pim_encod_grp_addr_t) 
      16 = sizeof(pim_encod_uni_addr_t)
     4 = two shorts fro numJoins and numPrunes
     4 = pim header

 */
#define PIMSM_MAX_JP_MSG_SIZE         PIM_MAX_JP_MSG_SIZE

/*#define PIMSM_MAX_JP_NUM_GROUPS     (255 - L7_PIMSM_MAX_RP_GRP_ENTRIES)*/
/*#define PIMSM_MAX_JP_NUM_GROUPS     5*/
/* 255 is because numgroups in jp-msg is 8-bytes field */

#define PIMSM_MAX_PROTOCOL_PACKETS    MCAST_MAX_INTERFACES 
#define PIMSM_MAX_IPV4_CACHE_ENTRIES  (PIMSM_STAR_G_IPV4_TBL_SIZE_TOTAL * 10)
#define PIMSM_MAX_IPV6_CACHE_ENTRIES  (PIMSM_STAR_G_IPV6_TBL_SIZE_TOTAL * 10)
/* Section 4.11.  [Page 128]*/
/* Propagation_delay_default =  0.5 secs    */
#define PIMSM_DEFAULT_PROPAGATION_DELAY_MSECS          500 /* the time period is in millisecs */

/*  t_override_default  = 2.5 secs */
#define PIMSM_DEFAULT_OVERRIDE_INTERVAL_MSECS             2500/* the time period is in millisecs */ 

#define PIMSM_DEFAULT_JP_OVERRIDE_INTERVAL           3 
#define PIMSM_DEFAULT_TRIGGERED_HELLO_DELAY       5
#define PIMSM_DEFAULT_ASSERT_OVERRIDE_INTERVAL    3 
#define PIMSM_DEFAULT_ASSERT_TIME                 180
#define PIMSM_DEFAULT_REGISTER_SUPPRESSION_TIME   60
/* Used to send NULL-REGISTER pkt*/
#define PIMSM_DEFAULT_REGISTER_PROBE_TIME         10
#define PIMSM_DEFAULT_KEEPALIVE_PERIOD            210
#define PIMSM_DEFAULT_RP_KEEPALIVE_PERIOD   \
   (3 * PIMSM_DEFAULT_REGISTER_SUPPRESSION_TIME + PIMSM_DEFAULT_REGISTER_PROBE_TIME)
/* t_periodic as per draft-11*/
#define PIMSM_DEFAULT_ASSERT_OVERRIDE_INTERVAL    3
/* RFC 3.5.1.2 compliance */
#define PIMSM_DEFAULT_ASSERT_PREFERENCE            0x7fffffff   
#define PIMSM_DEFAULT_ASSERT_METRIC               0xffffffff
#define PIMSM_ASSERT_RPT_BIT                       0x80000000

/*(excluding pim header and data) */
#define PIMSM_REGISTER_HEARDER_SIZE              4 
/*(excluding pim header and option data) */
#define PIMSM_HELLO_HEARDER_SIZE                 4 

/* number of list entries used for both  GRP/RP and RP/GRP mapping */
#define PIMSM_RP_GRP_ADDR_LIST_NODES_MAX    (PIMSM_RP_GRP_ENTRIES_MAX * 2) 

/* all the timer values below are in seconds */
#define PIMSM_DEFAULT_BOOTSTRAP_PERIOD          60        
#define PIMSM_DEFAULT_BOOTSTRAP_TIMEOUT         130      
#define PIMSM_DEFAULT_BOOTSTRAP_SZ_TIMEOUT      1300
#define PIMSM_DEFAULT_CAND_RP_ADV_PERIOD        60
#define PIMSM_DEFAULT_CAND_RP_HOLDTIME          150

#define PIMSM_DEFAULT_CAND_RP_ADV_PRIORITY      192

/* Following macros should be used in flags in (S,G) or (*.G) or (*,*,RP) 
    Node struct & Not anywhere else*/
#define PIMSM_ADDED_TO_MFC       0x0001  /* a mirror for mfc  */
#define PIMSM_NEW                0x0002  /* new created routing entry    */
#define PIMSM_NULL_OIF           0x0004  /* oif is null  */
#define PIMSM_NODE_DELETE        0x0008  /* mark the node for deletion*/
/* Encoded-Source address bits */
#define PIMSM_ADDR_RP_BIT 0x1
#define PIMSM_ADDR_WC_BIT 0x2
#define PIMSM_ADDR_S_BIT  0x4


/* 
 * CAUTION: Always check for == L7_TRUE or == L7_FALSE
            Don't check for  != L7_TRUE or != L7_FALSE
            because of -1 being other return value.
 */
#define PIMSM_INET_IS_ADDR_EQUAL(xaddr, yaddr)      \
    (((inetIsAddressZero((xaddr)) == L7_TRUE) ||      \
    (inetIsAddressZero((yaddr)) == L7_TRUE)) ? -1 :   \
    L7_INET_IS_ADDR_EQUAL((xaddr), (yaddr)))
#endif /* _PIMSMDEFS_H_ */
