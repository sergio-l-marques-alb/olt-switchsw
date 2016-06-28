/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename l7_resources.h
*
* @purpose  This file defines parameters which vary based on available resource
*           constraints (memory etc.). 
*           These parameters should be internal to the component and not 
*           externally visible to the user.
*
* @component 
*
* @comments 
*          
*         
*
* @created 03/15/2007
*
* @author akulkarni 
* @end
*
**********************************************************************/

#ifndef  __INC_L7_RESOURCES_H__
#define  __INC_L7_RESOURCES_H__



/*********************************************************************
**  Start of Message Queue Count defines 
**********************************************************************/
#define DEFAULT_MSG_COUNT                                 1024
#define DEFAULT_MSG_COUNT_MED                             2048
#define DEFAULT_MSG_COUNT_HIGH                            3072

#define DEFAULT_HELPER_MSG_COUNT                          (3*L7_MAX_PHYSICAL_PORTS_PER_UNIT)

#define USER_MGR_MSG_COUNT                        DEFAULT_MSG_COUNT_MED
#define FD_DOT1X_MSG_COUNT                        DEFAULT_MSG_COUNT_MED
#define SNOOP_MSG_COUNT                           DEFAULT_MSG_COUNT_MED
#define DOT3AD_SID_MSG_COUNT                      DEFAULT_MSG_COUNT
#define LLDP_MSG_COUNT		                      DEFAULT_MSG_COUNT
#define PML_MSG_COUNT                             3
#define FD_CNFGR_RADIUS_MSG_COUNT                 DEFAULT_MSG_COUNT
#define FD_CNFGR_TACACS_MSG_COUNT                 DEFAULT_MSG_COUNT
#define DHCPSMAP_QUEUE_MAX_MSG_COUNT                        64
#define DTL_MSG_COUNT                             (4*1024)
#define DTL_ADDR_MSG_COUNT                        L7_MAX_FDB_MAC_ENTRIES
#define HAPI_BROAD_LAG_ASYNC_CMD_QUEUE_SIZE       (L7_MAX_NUM_LAG_INTF * \
                                                   L7_MAX_MEMBERS_PER_LAG * 4)
/* the number of messages GARP queue can hold */
#define GARP_MSG_COUNT                  (L7_MAX_VLANS + (L7_MAX_INTERFACE_COUNT * 2))
/* the number of messages GARP PDU queue can hold */
#define GARP_PDU_MSG_COUNT                        (L7_MAX_INTERFACE_COUNT * 2)
#define DOT1Q_MSG_COUNT                       (4*L7_MAX_VLANS + DOT1Q_INTF_MAX_COUNT)

#define DOT1S_MSG_COUNT                           DEFAULT_MSG_COUNT_HIGH
#define DOT1AD_MSG_COUNT                       (L7_MAX_INTERFACE_COUNT)
#ifdef L7_AUTO_INSTALL_PACKAGE
#define AUTO_INSTALL_MSG_COUNT                    32
#endif

#define L7_TRAP_MSG_QUEUE_SIZE                    512

#define ISDP_MSG_COUNT                            (3*L7_MAX_PORT_COUNT)

#define BOXS_MSG_COUNT                            12

/* This queue is only used if the L7_DOT1S_ASYNC_STATE_SET is set to true
*/
#define DOT1S_STATE_CHANGE_QUEUE_SIZE (  \
        ((L7_MAX_PORT_COUNT + L7_MAX_NUM_LAG_INTF) * \
        (L7_MAX_MULTIPLE_STP_INSTANCES + 1))*4)
/*********************************************************************
**  end of Message Queue Count defines 
**********************************************************************/


/*********************************************************************
**  Start of Buffer pool defines 
**********************************************************************/
#define DOT1S_MAX_BPDU_BUF_COUNT			            (2*L7_MAX_PORT_COUNT)

#define DOT3AD_NUM_BUFFERS		                      (L7_MAX_NUM_DYNAMIC_LAG * \
                                                         L7_MAX_MEMBERS_PER_LAG * 4)

#define SNOOP_NUM_LARGE_BUFFERS                             10
#define SNOOP_NUM_MED_BUFFERS                               300
#define SNOOP_NUM_SMALL_BUFFERS             (2*((L7_MAX_GROUP_REGISTRATION_ENTRIES*5)/4))
#define SNOOP_SMALL_BUFFER_SIZE                            512
#define SNOOP_MED_BUFFER_SIZE                              2048

#define L7_LLDP_REM_MGMT_ADDR_BUFFERS                      100
#define L7_LLDP_REM_UNKNOWN_TLV_BUFFERS                    100
#define L7_LLDP_REM_ORG_DEF_INFO_BUFFERS                   100

/* The number of small buffers and the size of each is based upon the limits 
** of the pending request queue and the data structures that can be queued 
** to it. One each for the authentication server and accounting server.
**
** Number = 2 servers * (256 pending requests + 256 request infos) = 1024 
** Size = max of (.... or sizeof request info) = 376
*/
#define RADIUS_SM_BUF_NUM                                 1024
/* The number of large buffers and the size of each is based upon the limits
** of the pending request queue and the maximum expected size of a RADIUS 
** packet to be received in a response plus a secret.
**
** Number = 256 pending requests = 256
** Size = RADIUS packet + Secret = 4102
*/
#define RADIUS_LG_BUF_NUM                                  256

#define DAPI_DEBUG_NUM_BUFFER_MSGS                         512

/*********************************************************************
**   End of Buffer pool defines. 
**********************************************************************/



/*********************************************************************
**  Start of Misc. base paramters 
**********************************************************************/
/* Network Buffers */
/* The maximum network buffers depend upon reserved buffers
   If you are changing one check if you need to change the other
*/
/* PTin modified */
#if 0
#define L7_MAX_NETWORK_BUFF_PER_BOX                        246
#define L7_MBUF_RESERVED_TX_BUFFERS                         32
#define L7_MBUF_RESERVED_RX_HI_PRIO_BUFFERS                 16
#define L7_MBUF_RESERVED_RX_MID0_PRIO_BUFFERS               16
#define L7_MBUF_RESERVED_RX_MID1_PRIO_BUFFERS               64
#define L7_MBUF_RESERVED_RX_MID2_PRIO_BUFFERS               64
#else
#define L7_MAX_NETWORK_BUFF_PER_BOX                       1024
#define L7_MBUF_RESERVED_TX_BUFFERS                        128
#define L7_MBUF_RESERVED_RX_HI_PRIO_BUFFERS                 64
#define L7_MBUF_RESERVED_RX_MID0_PRIO_BUFFERS               64
#define L7_MBUF_RESERVED_RX_MID1_PRIO_BUFFERS               64
#define L7_MBUF_RESERVED_RX_MID2_PRIO_BUFFERS               64
//#define L7_MAX_NETWORK_BUFF_PER_BOX                        768
//#define L7_MBUF_RESERVED_TX_BUFFERS                         64
//#define L7_MBUF_RESERVED_RX_HI_PRIO_BUFFERS                 64
//#define L7_MBUF_RESERVED_RX_MID0_PRIO_BUFFERS               64
//#define L7_MBUF_RESERVED_RX_MID1_PRIO_BUFFERS               64
//#define L7_MBUF_RESERVED_RX_MID2_PRIO_BUFFERS               64
#endif

/* OS task dispatching parameters.
*/
#define L7_DEFAULT_STACK_SIZE                           (1024 * 32)
#define L7_DEFAULT_TASK_SLICE                                1
#define L7_DEFAULT_TASK_PRIORITY                           255
#define L7_MEDIUM_TASK_PRIORITY                            100
#define L7_TASK_PRIORITY_LEVEL(x)                           (x)
/* Maximum number of records in the buffered log. */
#define L7_LOG_IN_MEMORY_LOG_COUNT                         128
#define OSAPI_MAX_TIMERS                                  4096

/* Maximum number of BCM devices in a single FASTPATH unit */
#define L7_MAX_BCM_DEVICES_PER_UNIT                          8
/* Maximum number of ports per BCM device */
#define L7_MAX_BCM_PORTS_PER_DEVICE                         64

/*********************************************************************
**  End of Misc. base paramters 
**********************************************************************/



/*********************************************************************
**  Start of Misc. L2 paramters 
**********************************************************************/
/* Number of Querier vlans: This constant can be used to control max number of VLANs
                            enabled for querier based on requirement */
#define SNOOP_QUERIER_VLAN_MAX                      (L7_MAX_VLANS/16)
/*********************************************************************
**  End of Misc. L2 paramters 
**********************************************************************/




/*********************************************************************
**  Start of Misc. L3 paramters 
**********************************************************************/
/* Maximum number of static routes is arbitrarily chosen */
#define FD_RTR_MAX_STATIC_ROUTES                            64

#define    FD_RTR6_MAX_STATIC_ROUTES                        64 
/* Max static ARPs is largest conceivable for any platform, but is not 
 * necessarily the max supported on a given platform.  This value is only
 * used to give the config array a consistent, platform-independent number 
 * of entries to facilitate config file migration.
 *
 * See L7_IP_ARP_CACHE_STATIC_MAX for the maximum number of configurable 
 * static ARPs supported on a give platform.
 */
#define FD_RTR_MAX_STATIC_ARP_ENTRIES                      256

/*********************************************************************
**  End of Misc. L3 paramters 
**********************************************************************/

/*********************************************************************
**  Start of Misc. Mgmt paramters 
**********************************************************************/
#define SCROLL_BUF_LINES                                   1024

/* Add 10 to RMON max interface count in order to pass ANVL tests.
   Note: Need to verify how much saving we get from tweaking this.Hence removing them  */
#define L7_MAX_RMON_INTERFACE_COUNT                    (L7_MAX_PORT_COUNT \
                                                        + L7_MAX_NUM_LAG_INTF + 10)
#define L7_RMON_ENTRY_NUM                                    3
/*********************************************************************
**  End of Misc. Mgmt paramters 
**********************************************************************/

#define DAPI_TRACE_ENTRY_MAX         (1024)
#define DAPI_TRACE_ENTRY_SIZE_MAX    (128)

#define SNOOP_MAX_MRTR_TIMERS       (L7_MAX_VLANS)

#define FD_SIM_NETWORK_CONFIG_MODE          L7_SYSCONFIG_MODE_NONE
#define FD_SIM_SERVPORT_CONFIG_MODE         L7_SYSCONFIG_MODE_DHCP

/* Product specific default addresses for network and service port */
#define PROD_SIM_DEFAULT_SERVPORT_IP_ADDR   0x00000000   /* 0.0.0.0 */
#define PROD_SIM_DEFAULT_SERVPORT_MASK      0x00000000   /* 0.0.0.0 */
#define PROD_SIM_DEFAULT_SERVPORT_GW        0x00000000   /* 0.0.0.0 */

#define PROD_SIM_DEFAULT_NETWORK_IP_ADDR    0x00000000   /* 0.0.0.0 */
#define PROD_SIM_DEFAULT_NETWORK_MASK       0x00000000   /* 0.0.0.0 */
#define PROD_SIM_DEFAULT_NETWORK_GW         0x00000000   /* 0.0.0.0 */

#define NIM_TRACE_ENTRY_MAX         (1024)
#define NIM_TRACE_ENTRY_SIZE_MAX    (32)

#define L7_TLV_TRACE_ENTRY_MAX        128
#define L7_TLV_TRACE_ENTRY_SIZE_MAX   20

#define DOT1X_TRACE_ENTRY_MAX         (256)
#define DOT1X_TRACE_ENTRY_SIZE_MAX    (16)
#define DOT3AD_TRACE_ENTRY_MAX        (256)
#define DOT3AD_TRACE_ENTRY_SIZE_MAX   (32)

#define DOT3AH_TRACE_ENTRY_MAX         (256)
#define DOT3AH_TRACE_ENTRY_SIZE_MAX    (16)



#if 0
#define DOT1S_TRACE_ENTRY_MAX         (256)
#define DOT1S_TRACE_ENTRY_SIZE_MAX    (16)
#endif

#define L7_MAX_BUFFER_POOLS             128

#define TRACEROUTE_MAX_SESSIONS         16

#define TRAPMGR_TABLE_SIZE              256

#define L7_DOT1S_STACK_SIZE             L7_DEFAULT_STACK_SIZE + (1024 * 32)

#define L7_ISDP_STACK_SIZE              L7_DEFAULT_STACK_SIZE

#define  L7_CMGR_CARD_INSERT_REMOVE_QUEUE_MSG_COUNT       256

/* Memory profiler support */
#define L7_MEMORY_PROFILER_ENABLE

/* Dtl end driver IP stack buffer config */
#define DTL_CL_SMALL_NUM          500
#define DTL_CL_MEDIUM_NUM         100
#define DTL_CL_LARGE_NUM          10


#define DTL_CL_NUM			      (DTL_CL_SMALL_NUM + DTL_CL_MEDIUM_NUM + DTL_CL_LARGE_NUM)
#define DTL_MBLK_NUM			  (DTL_CL_NUM)

/* L3 end driver IP stack buffer config */
#define L3END_CL_SMALL_NUM          500
#define L3END_CL_MEDIUM_NUM         100
#define L3END_CL_LARGE_NUM          10


#define L3END_CL_NUM			      (L3END_CL_SMALL_NUM + L3END_CL_MEDIUM_NUM + L3END_CL_LARGE_NUM)
#define L3END_MBLK_NUM			      (L3END_CL_NUM)

#endif /* __INC_L7_RESOURCES_H__*/
