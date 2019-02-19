/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_common.h
*
* @purpose   Central header file for the BROAD HAPI MODULE
*
* @component hapi
*
* @comments
*
* @create    12/20/2001
*
* @author    mbaucom
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_BROAD_COMMON_H
#define INCLUDE_BROAD_COMMON_H


#include "l7_common.h"
#include "defaultconfig.h"
#include "log.h"
#include "sysapi_hpc.h"
#include "dapi.h"
#include "dapi_struct.h"

#include "dot1q_api.h"

/* Vendor Driver files */
#include <sal/types.h>
#include <sal/core/alloc.h>
#include <sal/core/spl.h>
#include <sal/appl/pci.h>
#include <sal/appl/sal.h>
#include <sal/appl/config.h>
#include <sal/appl/io.h>
#include <appl/cpudb/cpudb.h>

#include <bcm/error.h>
#include <bcm/filter.h>
#include <bcm/stg.h>
#include <bcm/port.h>
#include <bcm/link.h>

#include "bcm_int/esw/mbcm.h"

#include "broad_cpu_intf.h"
#include "broad_init.h"
#include "broad_port.h" 
#include "broad_stats.h"
#include "broad_system.h"
#include "broad_policy_types.h"
#include "broad_l2_dot1x.h"
#include "broad_l2_voiceVlan.h"

#ifdef L7_METRO_FLEX_PACKAGE
#ifdef L7_DOT1AD_PACKAGE
#include "broad_l2_dot1ad.h"
#endif
#endif

/* Stacking traffic runs on VLAN 4094.
** COS 7 is reserved for stacking control traffic.
**
** On XGS technology it may be possible to use VLAN 4095, however the CHECK_VID macro
** currently prevents us from using this value. 
*/
#define HPC_STACKING_VLAN_ID 4094
#define HPC_STACKING_COS 7

/* Maximum number of ports per broadcom chip.
*/
#define HAPI_BROAD_MAX_PORTS_PER_CPU_UNIT 56

/* Maximum number  of broadcom modules on the same PCI bus.
*/
#define HAPI_BROAD_MAX_CPU_UNITS_PER_BOX   BCM_LOCAL_UNITS_MAX


/* EGRESS COS setting for BPDUs.
*/
#ifdef L7_STACKING_PACKAGE
#define HAPI_BROAD_EGRESS_HIGH_PRIORITY_COS  6
#else
#define HAPI_BROAD_EGRESS_HIGH_PRIORITY_COS  7
#endif

/* EGRESS COS setting for normal priority packets.
*/
#define HAPI_BROAD_EGRESS_NORMAL_PRIORITY_COS (HAPI_BROAD_EGRESS_HIGH_PRIORITY_COS /*- 1*/)   /* PTin modified: for inband packets */

/* INGRESS COS setting for BPDUs. This cos is same as 
** as HAPI_BROAD_EGRESS_NORMAL_PRIORITY_COS as the BPDUs are sent 
** directly to the manager unit without any tunneling. So there 
** shouldn't be any contention. If BPDU's are tunneled on some
** platform, then the cos leveles may have to be adjusted (see hapiBroadCpuRxTxInit).
*/
#define HAPI_BROAD_INGRESS_BPDU_COS           (HAPI_BROAD_EGRESS_HIGH_PRIORITY_COS)

/* COS Setting for other high priority frames.
** This includes protocol messages like RIP/BGP/OSPF. The priority for these frames
** should be less than BPDU priority.
*/
#define HAPI_BROAD_INGRESS_HIGH_PRIORITY_COS  (HAPI_BROAD_INGRESS_BPDU_COS - 1)

/* COS Setting for medium priority frames.
** This includes protocol messages like IGMP, ARPs.
** The priority for these frames should be less than BPDU priority.
*/
#define HAPI_BROAD_INGRESS_MED_PRIORITY_COS  (HAPI_BROAD_INGRESS_HIGH_PRIORITY_COS - 1)

/* COS Setting for low priority frames.
** It should not obstruct BDPU and control packets of protocols. 
** It should not be obstructed by BC, MC and low priortiy packets.
*/
#define HAPI_BROAD_INGRESS_LOW_PRIORITY_COS     (HAPI_BROAD_INGRESS_MED_PRIORITY_COS - 1)
#define HAPI_BROAD_INGRESS_LOWEST_PRIORITY_COS  1

/* The priority to be marked for the packets destined to local routing interface.
** The cos queue is picked based on the corresponding priority. With default dot1p
** mappings, a priority 6 will map the packets on cpu cos-queue 3.
*/
#define HAPI_BROAD_INGRESS_LOCAL_IP_PRIORITY    6

/*
 * #defines and emuns
 */
#define L7_BCMX_OK(rc)     ((((rc) > 0)                 || \
                             ((rc) == BCM_E_NONE)       || \
                             ((rc) == BCM_E_TIMEOUT)    || \
                             ((rc) == BCM_E_BADID)      || \
                             ((rc) == BCM_E_EMPTY)      || \
                             ((rc) == BCM_E_NOT_FOUND)  || \
                             ((rc) == BCM_E_EXISTS)     || \
                             ((rc) == BCM_E_INIT)  || \
                             ((rc) == BCM_E_PORT)) ? L7_TRUE : L7_FALSE)

 /* Reason codes for RX to CPU on RoBo.
    SDK should define these in some common header file,
    but currently these are defined in robo/rx.c */
#define BCM_ROBO_RX_REASON_MIRROR 0x1
                /* Mirroring */
#define BCM_ROBO_RX_REASON_SW_LEARN 0x2
                /* Software SA learning */
#define BCM_ROBO_RX_REASON_SWITCHING 0x4
                /* Normal switching */
#define BCM_ROBO_RX_REASON_PROTOCOL_TERMINATION 0x8
                /* Protocol Termination */
#define BCM_ROBO_RX_REASON_PROTOCOL_SNOOP 0x10
                 /* Protocol Snooping */
#define BCM_ROBO_RX_REASON_EXCEPTION_FLOOD 0x20
                 /* Exception porcessing or flooding */

/* trick to enable printing of special information on command */
/* a debug command should be used in order to set the value after boot */
#ifdef GLOBAL_HAPI_DEBUG_PRINTF
L7_uint32 hapiDriverPrintfFlag = SYSAPI_LOGGING_HAPI;
#else
extern L7_uint32 hapiDriverPrintfFlag;
#endif


#define L2_BSR_TIME_INTERVAL       1

/* Packets per time quantum to allow 20% based on 64 byte frames */
/* (14881 pps * 0.20) */
#define L2_BSR_10MBPS_PKTS_PER_INTERVAL      (2976)

/* Packets per time quantum to allow 5% based on 64 byte frames */
/* (148810 pps * 0.05) */
#define L2_BSR_100MBPS_PKTS_PER_INTERVAL        (7440)

/* Packets per time quantum to allow 5% based on 64 byte frames */
/* (1488100 pps * 0.05) */
#define L2_BSR_1000MBPS_PKTS_PER_INTERVAL       (74405)

/* Cosq WRR weights */
#define COSQ0_WEIGHT 1
#define COSQ1_WEIGHT 2
#define COSQ2_WEIGHT 3
#define COSQ3_WEIGHT 4
#define COSQ4_WEIGHT 5
#define COSQ5_WEIGHT 6
#define COSQ6_WEIGHT 7
#define COSQ7_WEIGHT 8

/* Need to add one for the instance 0 which does not count in the protocol's instances */
#define HAPI_MAX_MULTIPLE_STP_INSTANCES     (L7_MAX_MULTIPLE_STP_INSTANCES + 1)

#define HAPI_BROAD_MAX_PORTS_PER_SWC 32
#define HAPI_BROAD_MAX_SWCM_PER_UNIT 5
#define HAPI_BROAD_MAX_CPU_PER_STACK 2
#define HAPI_BROAD_MAX_PORTS_PER_MODID 32

/*ON XGS2 mode-dvlan tunnel FROM THE CLI MADE THE PORT AN ACCESS (CUSTOMER) PORT. 
  THIS IMPLEMENTATION HAS SINCE BEEN CHANGED FOR XGS3 DEVICES. THE SAME COMMAND FROM THE CLI
  MAKES THE PORT AN UPLINK (SERVICE PROVIDER) PORT. THIS IS CONSISTENT WITH THE WAY MARVEL AND
  CISCO WORK. TO FORCE THE HARDWARE TO BEHAVE WITH THE OLD XGS2 IMPLEMENTATION SET THIS FLAG TO 1*/
#define HAPI_BROAD_DTAG_XGS2 0

/*Double VLAN Tagging modes*/
#define HAPI_BROAD_DTAG_MODE_NONE 0
/* Broadcom Uplink port*/
#define HAPI_BROAD_DTAG_MODE_INTERNAL 1
/* Broadcom Access port*/
#define HAPI_BROAD_DTAG_MODE_EXTERNAL 2


/* Maximum number of multicast routes.
*/
#define L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL  L7_MULTICAST_FIB_MAX_ENTRIES

/* This macro sets a bit corresponding to the VLAN in the VLAN membership
** mask. 
*/
#define BROAD_VLAN_MEMBER_SET(usp,vid,dapi_g) \
            (((BROAD_PORT_t*)HAPI_PORT_GET((usp),(dapi_g)))->vlan_member.value[(vid) / 8] |=  \
              (1 << ((vid) % 8) ))

/* This macro clears a bit corresponding to the VLAN in the VLAN membership
** mask. 
*/
#define BROAD_VLAN_MEMBER_CLEAR(usp,vid,dapi_g) \
            (((BROAD_PORT_t*)HAPI_PORT_GET((usp),(dapi_g)))->vlan_member.value[(vid) / 8] &=  \
              (~(1 << ((vid) % 8) )))

/* This macro returns a non-zero value if the port is a member of the specified VLAN.
*/
#define BROAD_IS_VLAN_MEMBER(usp,vid,dapi_g) \
            (((BROAD_PORT_t*)HAPI_PORT_GET((usp),(dapi_g)))->vlan_member.value[(vid) / 8] & \
              (1 << ((vid) % 8) ))

/* This macro sets a bit corresponding to the VLAN in the tagging enebaled
** mask. 
*/
#define BROAD_VLAN_TAGGING_SET(usp,vid,dapi_g) \
            (((BROAD_PORT_t*)HAPI_PORT_GET((usp),(dapi_g)))->vlan_tagged.value[(vid) / 8] |=  \
              (1 << ((vid) % 8) ))

/* This macro clears a bit corresponding to the VLAN in the tagging enabled
** mask. 
*/
#define BROAD_VLAN_TAGGING_CLEAR(usp,vid,dapi_g) \
            (((BROAD_PORT_t*)HAPI_PORT_GET((usp),(dapi_g)))->vlan_tagged.value[(vid) / 8] &=  \
              (~(1 << ((vid) % 8) )))

/* This macro returns a non-zero value if tagging is enabled on the VLAN.
*/
#define BROAD_IS_VLAN_TAGGING(usp,vid,dapi_g) \
            (((BROAD_PORT_t*)HAPI_PORT_GET((usp),(dapi_g)))->vlan_tagged.value[(vid) / 8] & \
              (1 << ((vid) % 8) ))


/* The following are the hardware routines */
/* This macro sets a bit corresponding to the VLAN in the VLAN membership
** mask. 
*/
#define BROAD_HW_VLAN_MEMBER_SET(usp,vid,dapi_g) \
            (((BROAD_PORT_t*)HAPI_PORT_GET((usp),(dapi_g)))->hw_vlan_member.value[(vid) / 8] |=  \
              (1 << ((vid) % 8) ))

/* This macro clears a bit corresponding to the VLAN in the VLAN membership
** mask. 
*/
#define BROAD_HW_VLAN_MEMBER_CLEAR(usp,vid,dapi_g) \
            (((BROAD_PORT_t*)HAPI_PORT_GET((usp),(dapi_g)))->hw_vlan_member.value[(vid) / 8] &=  \
              (~(1 << ((vid) % 8) )))

/* This macro returns a non-zero value if the port is a member of the specified VLAN.
*/
#define BROAD_IS_HW_VLAN_MEMBER(usp,vid,dapi_g) \
            (((BROAD_PORT_t*)HAPI_PORT_GET((usp),(dapi_g)))->hw_vlan_member.value[(vid) / 8] & \
              (1 << ((vid) % 8) ))

/* This macro sets a bit corresponding to the VLAN in the tagging enebaled
** mask. 
*/
#define BROAD_HW_VLAN_TAGGING_SET(usp,vid,dapi_g) \
            (((BROAD_PORT_t*)HAPI_PORT_GET((usp),(dapi_g)))->hw_vlan_tagged.value[(vid) / 8] |=  \
              (1 << ((vid) % 8) ))

/* This macro clears a bit corresponding to the VLAN in the tagging enabled
** mask. 
*/
#define BROAD_HW_VLAN_TAGGING_CLEAR(usp,vid,dapi_g) \
            (((BROAD_PORT_t*)HAPI_PORT_GET((usp),(dapi_g)))->hw_vlan_tagged.value[(vid) / 8] &=  \
              (~(1 << ((vid) % 8) )))

/* This macro returns a non-zero value if tagging is enabled on the VLAN.
*/
#define BROAD_IS_HW_VLAN_TAGGING(usp,vid,dapi_g) \
            (((BROAD_PORT_t*)HAPI_PORT_GET((usp),(dapi_g)))->hw_vlan_tagged.value[(vid) / 8] & \
              (1 << ((vid) % 8) ))



#define HAPI_BROAD_INVALID_L3_INTF_ID 0xFFFFFFFF

#define HAPI_BROAD_USP_TO_UPORT(_usp,_uport)  (_uport) = (bcmx_uport_t) (((_usp)->unit << 24) | ((_usp)->slot << 16) | (_usp)->port)
#define HAPI_BROAD_UPORT_TO_USP(_uport,_usp)  do {                                       \
                                              (_usp)->unit = (((L7_uint32)_uport) >> 24) & 0xFF;     \
                                              (_usp)->slot = (((L7_uint32)_uport) >> 16) & 0xFF;     \
                                              (_usp)->port = (((L7_uint32)_uport))       & 0xFFFF;   \
                                            } while (0)
#define HAPI_BROAD_LPORT_TO_USP(_lport,_usp)  do {                                       \
                                              bcmx_uport_t _uport = BCMX_UPORT_GET(_lport);       \
                                              (_usp)->unit = (((L7_uint32)_uport) >> 24) & 0xFF;     \
                                              (_usp)->slot = (((L7_uint32)_uport) >> 16) & 0xFF;     \
                                              (_usp)->port = (((L7_uint32)_uport))       & 0xFFFF;   \
                                            } while (0)

/* maximum voip profile supported 
*/
#define L7_MAX_VOIP_PROFILES 1

/* Timer value (in msec) for the hapiBroadStackCardPluginsFinishTimer */
#define HAPI_BROAD_STACK_CARD_PLUGINS_FINISH_TIMER (10 * 1000)

/*
 * Structures
 */
typedef struct BROAD_SYSTEM_s BROAD_SYSTEM_t;


/*******************************************************************************
*
* @structures BROAD_PORT_t
*
* @purpose    Maintain BROAD Port Data
*
* @end
*
*******************************************************************************/
typedef struct
{
  bcmx_lport_t            bcmx_lport;
  int                     bcm_unit;
  int                     bcm_port;
  int                     bcm_modid;

  L7_BOOL                 front_panel_stack;  

  DAPI_PORT_LOOPBACK_t        loopback_mode;
  L7_int32                    hw_lb_mode;
  L7_uint32                   autonegotiationAbilityMask;
  /* This flag indicates that the port is a LAG.
  */
  L7_BOOL                 port_is_lag;

  /* Cached Speed and Duplex */
  DAPI_PORT_SPEED_t       speed;
  DAPI_PORT_DUPLEX_t      duplex;
  L7_BOOL                 pause;      /* configuration for pause autonegotiation */
  L7_BOOL                 link_up;
  L7_BOOL                 rx_pause;   /* cached info on whether RX pause negotiated on link */
  L7_BOOL                 tx_pause;   /* cached info on whether TX pause negotiated on link */
  /* Cached value of whether the port's link up is from SFP module */
  L7_BOOL                 sfpLink; 


  /* Spanning-tree states */
  L7_int32                hw_stp_state;
  DAPI_PORT_STP_STATE_t   stp_state;
  bcm_stg_stp_t           hw_dot1s_state[HAPI_MAX_MULTIPLE_STP_INSTANCES];
  DAPI_PORT_DOT1S_STATE_t dot1s_state[HAPI_MAX_MULTIPLE_STP_INSTANCES];
  L7_BOOL                 bpduFilterMode;
  L7_BOOL                 bpduFloodMode;

  L7_BOOL                 l2FlushInProgress;
  L7_uint32               l2FlushReq; /* Number of flushes issued on this port */
  void                   *l2FlushSyncSem; /* Semaphore to synchronize setting port CML */

  /* Dot1x data */
  HAPI_DOT1X_PORT_t       dot1x;


  /* VLAN Membership mask */
  L7_VLAN_MASK_t          vlan_member;
  /* Tagged Membership mask. */
  L7_VLAN_MASK_t          vlan_tagged;
  /* Vlan Membership and tagging status in hardware. */
  L7_VLAN_MASK_t          hw_vlan_member;
  L7_VLAN_MASK_t          hw_vlan_tagged;

  /* Protocol-based VLAN membership list. The table is indexed by 
     protocol and contains VLAN IDs. If VLAN is zero then the port 
     does not classify the protocol, otherwise the port classifies 
     the protocol to the specified VLAN.
  */
  L7_ushort16             pbvlan_table[DAPI_QVLAN_NUM_OF_PROTOCOL_ENTRIES];

  L7_ushort16             pvid;
  L7_BOOL                 ingressFilteringEnabled;
  L7_BOOL                 acceptFrameType;
  L7_BOOL                 gvrpEnabled;
  L7_BOOL                 gmrpEnabled;
  L7_ulong32              priority;
  L7_BOOL                 locked; 
  L7_uint32               dtag_mode;
  L7_BOOL                 dhcpSnoopingEnabled;
  L7_BOOL                 dhcpSnoopingTrusted;
  L7_BOOL                 dynamicArpInspectionTrusted;
  L7_BOOL                 ipsgEnabled;

  /* Captive Portal Mode */
  L7_BOOL                     cpEnabled;
  L7_BOOL                     cpBlocked;


  /* dot1p to traffic class mapping table and policy id */
  L7_uchar8               dot1pMap[L7_DOT1P_MAX_PRIORITY+1];
  BROAD_POLICY_t          dot1pPolicy;
  BROAD_POLICY_t          voiceVlanPolicy;
  BROAD_POLICY_t          voipPolicy;
  void                   *qos;
  /* Used for EFM-OAM  */
  L7_BOOL                 dot3ahRemLbEnabled; 
  L7_BOOL                 dot3ahEnabled;
  BROAD_POLICY_t          dot3ahRlbPduPolicy;
  BROAD_POLICY_t          dot3ahRlbDataPolicy;

  /*BROAD_POLICY_t              voiceVlanPolicy;*/
  HAPI_VOICE_VLAN_PORT_t      voiceVlanPort;

  /* vlanId used for this port when it is enabled for routing.
  */
  L7_ushort16             port_based_routing_vlanid;

  /* Time to live limit for multicast frames on this interface.
  */
  L7_uchar8               multicast_ttl_limit;
  L7_uint32               l3_interface_id;
  L7_uint32              *vrrp_interface_id;
  void                   *routing_if_sync_sem;
  /* This semaphore is used to wait for L3 table sync 
     before deleting routing interfaces.
  */
  L7_BOOL                 routing_if_delete_pending;

   /* WLAN L2 tunnel info */
   void  *pWlanTunnel;
   

  union
  {
    struct
    {
      char  dummy[16];
    } cpu;

    struct
    {
      /* Flag indicating whether this port is administratively enabled.
      */
      L7_BOOL           admin_enabled;

      /* Flag indicating that fiber mode selection on combo PHYs id disabled.
      ** The fiber selection is disabled when auto-negotiation is disabled. This
      ** limitation is in place because Fastpath application doesn't handle media
      ** type configuration.
      */
      L7_BOOL           fiber_mode_disabled;

      L7_BOOL           acceptLLDPDU;
      L7_BOOL           acceptLAPDU;
      L7_BOOL           isMemberOfLag;
	  L7_BOOL           isLagAddPending;
      DAPI_USP_t        lagUsp;
      L7_uint32         phyCapability;
      void             *phySemaphore;
    } physical;

    struct
    {
      L7_uint32         numMembers;
      L7_uint32         tgid;
      L7_uint32         lastValidTgid;
      L7_BOOL           isolatePending;
      L7_uint32         hashMode;

    } lag;

    void  *pTunnel; /* Pointer to L3 internal tunnel struct */

  } hapiModeparm;
#ifdef L7_METRO_FLEX_PACKAGE
#ifdef L7_DOT1AD_PACKAGE
/* Dot1ad params in Hapi */
  dot1adIntfCfg_t     hapiDot1adintfCfg;
  dot1adDebugIntfStats_t  hapiDot1adIntfStats;
#endif
#endif
} BROAD_PORT_t;


/*******************************************************************************
*
* @structures BROAD_SYSTEM_t
*
* @purpose    Store HAPI BROAD System data
*
* @end
*
*******************************************************************************/
struct BROAD_SYSTEM_s
{
  void                  *qos;
  DAPI_USP_t             uspMap[HAPI_BROAD_MAX_CPU_PER_STACK][HAPI_BROAD_MAX_SWCM_PER_UNIT][HAPI_BROAD_MAX_PORTS_PER_SWC];

  L7_short16             stg_instNumber[L7_MAX_VLAN_ID + 1]; /* use vlan ID as index to get the instNumber */

  L7_enetMacAddr_t       bridgeMacAddr;
  L7_ushort16            mgmtVlanId;
  BROAD_POLICY_t         mgmtPolicy;
#ifdef L7_DOT1AG_PACKAGE
  BROAD_POLICY_t         dot1agPolicy;
#endif

  /* system policies for prioritizing traffic to CPU port */
  BROAD_POLICY_t         sysId1, sysId2;
  BROAD_POLICY_t         dhcpSnoopingPolicyId[120][2];          /* Trap DHCPS/DHCPC traffic and rate limit */
  BROAD_POLICY_t         dhcpDefaultPolicyId[2];             /* If snooping is disabled, just raise */
  BROAD_POLICY_t         dot1xViolationPolicyId;

  BROAD_POLICY_t         dynamicArpInspectUntrustedPolicyId;
  BROAD_POLICY_t         ipsgPolicyId;

#if defined(FEAT_METRO_CPE_V1_0)
  BROAD_POLICY_t         ipv6SnoopId;
#endif
#ifdef L7_METRO_PACKAGE
#ifdef L7_DOT3AH_PACKAGE
  BROAD_POLICY_t         dot3ahPolicyId;
  BROAD_POLICY_t         dot3ahPolicyPortId;  
  BROAD_POLICY_t         dot3ahPolicyLimit;
  BROAD_POLICY_t         dot3ahPolicyRemLb;
#endif
#endif

#ifdef L7_IPV6_PACKAGE
  BROAD_POLICY_t         Ip6SysId;
#endif

#ifdef L7_STACKING_PACKAGE
  BROAD_POLICY_t         fpsSysId; /* XGS2 FPS policy */
#endif
  L7_BOOL               dot1xMode;
  BROAD_POLICY_t         voiceCdpSysId;
  BROAD_POLICY_t         voiceDhcpSysId;
};


typedef struct 
{
  bcmx_lport_t     bcmx_lport;
  L7_uint32        vlanID;
  DAPI_USP_t       usp;

  L7_uchar8        *pkt_data;
  L7_uint32        user_data_size;
  L7_uchar8        proto_byte;

  L7_uint32        sendFrame;
  L7_uint32        sendBpdu;

  L7_uint32        dtag_mode;

} BROAD_TX_BPDU_MSG_t;
#ifdef L7_METRO_FLEX_PACKAGE
#ifdef L7_DOT1AD_PACKAGE
         /* Following structure is for L2 Protocol Tunneling */
typedef struct 
{
  bcmx_lport_t   bcmx_lport;
  L7_uint32        vlanID;
  L7_ushort16      dot1adSVlanId;
  L7_ushort16      dot1adRemarkCVlanId;
  DAPI_USP_t    usp;

  L7_uchar8       *pkt_data;
  L7_uint32        user_data_size;
  L7_uchar8       proto_byte;
  L7_uchar8       rx_untagged;
  L7_uint32        sendFrame;
  L7_uint32        sendBpdu;
  BROAD_PORT_t          *hapiPortPtr;
  L7_BOOL         tunnel;     /* tells whether to tunnel or detunnel */

} BROAD_TX_PDU_MSG_t;
#endif
#endif



typedef struct 
{
  DAPI_USP_t       usp;
  DAPI_FRAME_CMD_t cmdInfo;
  bcm_rx_reasons_t reasons;
  L7_uchar8        rx_untagged;
  L7_uint32        cos;

} BROAD_PKT_RX_MSG_t;
/* Functions */

typedef enum
{
  BROAD_FLUSH_BY_PORT = 0,
  BROAD_FLUSH_BY_VLAN = 1

} BROAD_FLUSH_TYPE_t;

typedef struct
{
  L7_uint32           bcmx_lport;
  L7_uint32           tgid;
  L7_BOOL             port_is_lag;
  L7_uint32           vlanID;
  BROAD_FLUSH_TYPE_t  flushtype;
  L7_BOOL             disableLearning;  /* PTin added */

} BROAD_L2ADDR_FLUSH_t;

typedef enum
{
  BROAD_TX_PRIO_HIGH = 0,
  BROAD_TX_PRIO_NORMAL,
  BROAD_TX_PRIO_LAST
} BROAD_TX_PRIO_t;


/* Driver init */

/* Get PCI Configuration of Device */
pci_dev_t* sysconfGetPciDevice(L7_int32 unit);

/* Start the LED Processor by downloading the program */
void bcmLedProcStart(L7_int32 unit);

/* Initialize the driver.
*/
L7_RC_t hpcBroadInit();

int
systemInit(int unit);

/* Packet allocation*/
void* hapiBroadPacketAlloc(L7_int32 unit, L7_int32 size, L7_uint32 flags);

/* Packet free */
void hapiBroadPacketFree(L7_int32 unit, void* ptr);

/* Get the USP of a port from the bcom's (unit, port) set */
void hapiBroadUspGet(L7_uint32 cpuId, L7_int32 unit, L7_int32 port, DAPI_t *dapi_g, DAPI_USP_t *usp);
void hapiBroadLogicalUspGet(L7_int32 unit, L7_int32 port, DAPI_t *dapi_g, DAPI_USP_t *usp);

void hapiBroadCpuTxRxInit(DAPI_t *dapi_g);
void hapiBroadReceiveTask(L7_uint32 numArgs, DAPI_t *dapi_g);
void hapiBroadBpduTxTask(L7_uint32 numArgs, DAPI_t *dapi_g);
#ifdef L7_METRO_FLEX_PACKAGE
#ifdef L7_DOT1AD_PACKAGE
void hapiBroadPduTransmitTask(L7_uint32 numArgs, DAPI_t *dapi_g);
L7_RC_t hapiBroadRxProtoSnoopModify(BROAD_PKT_RX_MSG_t *pktRxMsg,DAPI_t *dapi_g);
#endif
#endif
L7_BOOL hapiBroadStrataRx(BROAD_PKT_RX_MSG_t *pktRxMsg, DAPI_t *dapi_g);
L7_BOOL hapiBroadXgsRx(BROAD_PKT_RX_MSG_t *pktRxMsg, DAPI_t *dapi_g);
L7_BOOL hapiBroadRxMacSaIsL3Intf(L7_uchar8 *macSa, DAPI_t *dapi_g);

L7_RC_t hapiBroadDrvInit(L7_ulong32 cardId, DAPI_t *dapi_g);

void hapiBroadAddrMacUpdate(void *bcmx_l2addr, int insert, DAPI_t *dapi_g);

void hapiBroadAddrMacFrameLearn(bcm_pkt_t *bcm_pkt, DAPI_t *dapi_g);

void hapiBroadL3McastPortVlanAddNotify (DAPI_USP_t *usp, L7_ushort16 vlan_id, DAPI_t *dapi_g);

void hapiBroadL3McastPortVlanRemoveNotify (DAPI_USP_t *usp, L7_ushort16 vlan_id, DAPI_t *dapi_g);

L7_RC_t hapiBroadL3McastRPF(L7_netBufHandle frameHdl, L7_ushort16 vlanID, DAPI_USP_t *usp,
                            DAPI_t *dapi_g);
L7_RC_t hapiBroadConfigIgmpFilter(L7_BOOL enableFilter, L7_uint16 vlanId, L7_uint8 cos, L7_BOOL switchFrame, DAPI_USP_t *usp, DAPI_t *dapi_g);
L7_RC_t hapiBroadConfigMldFilter(L7_BOOL enableFilter,DAPI_t *dapi_g);
L7_RC_t hapiBroadConfigDhcpFilter(L7_BOOL enableFilter, L7_uint16 vlanId, DAPI_USP_t *usp, DAPI_t *dapi_g);

/* PTin added */
L7_uint16 hapiBroadPtinIgmp_numberOfAvailableVlans(void);
L7_uint16 hapiBroadPtinDhcp_numberOfAvailableVlans(void);
/* PTin end */

L7_RC_t hapiBroadTgidToUspConvert(L7_uint32 tgid, DAPI_USP_t *usp, DAPI_t *dapi_g);
void hapiBroadLportToTgidUspConvert(L7_uint32 lport, DAPI_USP_t *usp, DAPI_t *dapi_g);
L7_RC_t hapiBroadCosCommitDot1pParams(BROAD_PORT_t *hapiPortPtr, L7_uchar8 *dot1pMap);

L7_RC_t hapiBroadCosSetDot1pParams(DAPI_USP_t *usp, L7_uchar8 dot1p, L7_uchar8 cosq, DAPI_t *dapi_g);

L7_RC_t hapiBroadCosChangeTrustMode(BROAD_PORT_t *hapiPortPtr,L7_uint32 newTrustMode);


L7_RC_t hapiBroadQvlanMcastFloodModeSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);
L7_RC_t hapiBroadSwitchConfigDos(DAPI_t *dapi_g);
/*********************************************************************
*
* @purpose Install the MAC filter for the system MAC address.
*
* @param   dapi_g   - DAPI driver.
* @param   new_vlan_id - VLAN ID for the new MAC address. 
*                        If set to zero then new filter is not installed.
*
* @param   new_mac_addr - MAC Address for this filter.
*
* @returns void
*
* @notes   
*
* @end
*
*********************************************************************/
void hapiBroadFfpSysMacInstall (DAPI_t      *dapi_g,
                                L7_ushort16  new_vlan_id,
                                L7_uchar8   *new_mac_addr);

#ifdef L7_DOT1AG_PACKAGE
/*********************************************************************
*
* @purpose Install DOT1AG filter to trap cfm packets with system MAC address.
*
* @param   dapi_g   - DAPI driver.
*
* @param   new_mac_addr - MAC Address for this filter.
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void hapiBroadDot1agPolicyInstall (DAPI_t      *dapi_g,
                                   L7_uchar8   *new_mac_addr);
#endif
/*********************************************************************
* @purpose  Apply default configuration to the hardware.
*           This function is invoked during IPL and every time that 
*           the hardware is reset with bcm_clear().
*
* @param    void
*                                       
* @returns  none
*
* @comments    
*       
* @end
*********************************************************************/
void hpcHardwareDefaultConfigApply(void);

/*********************************************************************
* @purpose  Resets the Driver routing tables.
*
* @param    void
*                                       
* @returns  L7_SUCCESS successfully reset the driver
* @returns  L7_FAILURE problem occurred while resetting the driver
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t hpcHardwareTablesReset (void);

/*********************************************************************
* @purpose  Enables ASF mode on all local BCM units.
*
* @param    void
*                                       
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t hpcHardwareDriverAsfEnable(void);

/*********************************************************************
* @purpose  Resets the Driver to a known state
*
* @param    void
*                                       
* @returns  L7_SUCCESS successfully reset the driver
* @returns  L7_FAILURE problem occurred while resetting the driver
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t hpcHardwareDriverReset(void);


/**************************************************************************
*
* @purpose  Returns whether software learning is enabled. 
*
* @param    none
*
* @returns  L7_TRUE  - Software learning is enabled.
* @returns  L7_FALSE - Hardware learning is enabled.
*
* @notes 
*
* @end
*
*************************************************************************/
L7_BOOL hpcSoftwareLearningEnabled(void);

/*********************************************************************
* @purpose Determine whether this code is running on Draco/Tucana.
*
* @param   none
*
* @returns L7_TRUE - Code is running on XGSII devices
* @returns L7_FALSE - Code is not running on XGSII devices
*
* @notes   This function is primarily used in common CPU RX code.
*
* @end
*********************************************************************/
L7_BOOL hapiBroadXGS2Check (void);
/*********************************************************************
* @purpose Determine whether this code is running on Hawkeye.
*
* @param   none
*
* @returns L7_TRUE - Code is running on Hawkeye
* @returns L7_FALSE - Code is not running on Hawkeye
*
* @notes   Downside: Any new Hawkeye platform needs to be added here.
*
* @end
*********************************************************************/
L7_BOOL hapiBroadHawkeyeCheck (void);


/*********************************************************************
* @purpose Determine whether this code is running on Raptor.
*
* @param   none
*
* @returns L7_TRUE - Code is running on Raptor devices
* @returns L7_FALSE - Code is not running on Raptor devices
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL hapiBroadRaptorCheck (void);

/*********************************************************************
* @purpose Determine whether this code is running on Ravan.
*
* @param   none
*
* @returns L7_TRUE - Code is running on Ravan devices
* @returns L7_FALSE - Code is not running on Ravan devices
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL hapiBroadRavenCheck (void);

/*********************************************************************
* @purpose Determine whether this code is running on Robo.
*
* @param   none
*
* @returns L7_TRUE - Code is running on Robo devices
* @returns L7_FALSE - Code is not running on Robo devices
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL  hapiBroadRoboCheck (void);


/*********************************************************************
* @purpose Determine whether this code is running on RoboVariant.
*
* @param   none
*
* @returns L7_TRUE - Code is running on Rbob variant devices
* @returns L7_FALSE - Code is not running on Robo variant devices
*
* @notes
*
* @end
*********************************************************************/
L7_int32 hapiBroadRoboVariantCheck (void);

/*********************************************************************
* @purpose Determine whether this code is running on FB2.
*
* @param   none
*
* @returns L7_TRUE - Code is running on FB2s
* @returns L7_FALSE - Code is not running on FB2s
*
* @notes   Downside: Any new FB2 platform needs to be added here.
*
* @end
*********************************************************************/
L7_BOOL hapiBroadFB2Check (void);

/*********************************************************************
* @purpose Determine whether this code is running on platforms with MLD
*          issue
*
* @param   none
*
* @returns L7_TRUE - Code is running on platforms having MLD hardware bug
* @returns L7_FALSE - Code is not running on platforms with MLD bug.
*
* @notes   This function is primarily used in common CPU RX code.
*          See GNATS 15333 and Hardware errata docs.
*
* @end
*********************************************************************/
L7_BOOL hapiBroadMldHwIssueCheck (void);

/*********************************************************************
* @purpose Determine whether this code is running on Triumph.
*
* @param   none
*
* @returns L7_TRUE - Code is running on Triumph
* @returns L7_FALSE - Code is not running on Triumph
*
* @notes   Downside: Any new Triumph platform needs to be added here.
*
* @end
*********************************************************************/
L7_BOOL hapiBroadTriumphCheck (void);

L7_RC_t hapiBroadMapDbCreate(void);
L7_RC_t hapiBroadMapDbCpuUnitEntryAdd(int unit, cpudb_key_t *cpuKey, int cpuunit);
L7_RC_t hapiBroadMapDbCpuUnitEntryDel(int unit);
L7_RC_t hapiBroadMapDbPortEntryAdd(int unit, bcm_port_t port, bcmx_lport_t lport);
L7_RC_t hapiBroadMapDbEntryGet(cpudb_key_t *cpuKey, int cpuunit, bcm_port_t port, int *unit, bcmx_lport_t *lport);
bcmx_uport_t lvl7_uport_create_callback(bcmx_lport_t lport, int unit, bcm_port_t port, uint32 flags);
void hapiBroadDebugBcmxMapDump(void);

void hapiBroadModidModportToLportSet (int mod_id, int mod_port, int lport);
void hapiBroadModidModportToLportGet (int mod_id, int mod_port, int *lport);

void hapiBroadFfpIpAddrSync (void);
void hpcBroadRediscover(int priority);

/*********************************************************************
* @purpose  Detect stacking port link state changes.
*
* @param    void
*                                       
* @returns  none
*
* @comments    
*       
* @end
*********************************************************************/
void hpcStackPortLinkStatusCallback (
                      int unit,
                      bcm_port_t port,
                      bcm_port_info_t *info);

/*********************************************************************
* @purpose  Determine whether specified BCM port is a stack port.
*
* @param    bcm_unit  - BCM unit number of the port.
* @param    bcm_port  - BCM port number of the port.
*
* @returns  L7_TRUE - BCM port is used for stacking.
* @returns  L7_FALSE - BCM port is not used for stacking.
*
* @notes
*
* @end
*
*********************************************************************/
L7_BOOL hpcIsBcmPortStacking (L7_uint32 bcm_unit, L7_uint32 bcm_port);

/*********************************************************************
* @purpose  Enable/Disable the stack ports. 
*
* @param    L7_BOOL  enable
*                                       
* @returns  none
*
* @comments    
*       
* @end
*********************************************************************/
void hpcStackPortEnable (L7_BOOL enable);


/*********************************************************************
* @purpose  Qualifies BCMX filter with non-stack ports.
*
* @param    void
*                                       
* @returns  none
*
* @comments This function is called for BCMX filters that are installed   
* @comments on all ports in the box. For stand-alone boxes and HiGig
* @comments stackable systems the filter is not changed. 
* @comments For the front-panel stacked systems the filter is qualified
* @comments with front panel ports which are not configured for stacking.
*       
* @end
*********************************************************************/
void hpcBcmxFilterStackPortRemove(bcm_filterid_t *bcmx_filter);

/*********************************************************************
*
* @purpose Re-initialize multicast global data.
*
* @param   none
*
* @returns none
*
* @notes
*
* @end
*
*********************************************************************/
void hapiBroadL3McastReInit (void);

/*********************************************************************
*
* @purpose Updates the L3 routing table to match the L2 state.
*          The updates may include MAC address moves, LAG membership 
*          changes and tagging changes.
*
* @param  none
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void hapiBroadRouteUpdateL3Tables(void);

/*********************************************************************
*
* @purpose Waits for all pending group add/delete commands to finish.
*
* @param  none
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void hapiBroadL3McastWait (void);

/*********************************************************************
*
* @purpose Send a flush request to the Flush task
*
* @param   flushReq  @b{(input}} The port or vlan to flush
*
* @returns void
*
* @end
*
*********************************************************************/
void hapiBroadL2FlushRequest(BROAD_L2ADDR_FLUSH_t flushReq);


/*********************************************************************
*
* @purpose Set the Ingress Filtering mode
*
* @param   lport  - broadcom lport
* @param   val    - True, enabled
*                 - False, disabled
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadVlanIngressFilterSet(bcmx_lport_t lport,L7_BOOL val);

/*********************************************************************
*
* @purpose Set the Egress Filtering mode
*
* @param   lport  - broadcom lport
* @param   val    - True, enabled
*                 - False, disabled
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadVlanEgressFilterSet(bcmx_lport_t lport, L7_BOOL val, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Set the tpid on a port
*
* @param   lport  - broadcom lport
* @param   val    - True, enabled
*                 - False, disabled
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPortTpidSet(bcmx_lport_t lport, L7_ushort16 val, DAPI_t *dapi_g);

/*********************************************************************
* @purpose Stub function to point to SDK assert to log error
*
* @end
*********************************************************************/
void hapiBroadAssert(const L7_char8 *expr, const L7_char8 *file, L7_uint32 line);

/*********************************************************************
* @purpose  Modify MMU to give more cells to CPU CoS queues
*
* @param    unit - bcm unit number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments CPU CoS queues 4-7 will be given more cells
*           To handle bursts at these COS
*
* @end
*********************************************************************/
L7_RC_t hapiBroadMmuConfigModify(L7_uint32 unit);


/*********************************************************************
*
* @purpose Enable/Disable learning on a port or trunk during flush operation.
*
* @param   portInfo  - Learn mode of lport/tgid to be changed
* @param   learnMode - L7_ENABLE: Enable learning
*                      L7_DISABLE: Disable learning
*
* @returns L7_RC_t    result
*
* @notes  In stackable environment we can run into scenarios where the
*         native unit finishes the flush quickly, re-learns new 
*         addresses and notifies the manager of the new addresses before
*         the management unit has executed the particular port flush. So
*         management unit will loose all the newly learned port mac-addresses
*         after the flush, causing L2 tables to go out of sync. 
*         With this approach, learning is disabled on the port until the management
*         unit finishes the flush. This should keep the management unit in 
*         sync with other units. There is still a chance that non-management
*         units may go out of sync...
*         
*         
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadFlushL2LearnModeSet (BROAD_L2ADDR_FLUSH_t portInfo, L7_uint32 learnMode);

/*********************************************************************
*
* @purpose Notification from CMGR to the driver about a card plug-in
*
*
* @returns L7_RC_t result
*
* @notes   CARD_CMD_PLUGIN_START is issued before the L7_ATTACH for the card is issued
*          to NIM.
*          CARD_CMD_PLUGIN_FINISH is issued after NIM/Application has finished L7_ATTACH
*          processing.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadCardPluginNotify (DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Notification from CMGR to the driver about a card plug-in
*
*
* @returns L7_RC_t result
*
* @notes   CARD_CMD_UNPLUG_START is issued before the L7_DETACH for the card is issued
*          to NIM/Application.
*          CARD_CMD_UNPLUG_FINISH is issued after NIM/Application has finished L7_DETACH
*          processing.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadCardUnplugNotify (DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Sets the admin mode for all the ports in a card
*
* @param   unit        Unit number of the card
* @param   slot        Slot number of the card
* @param   forceMode           L7_TRUE: Apply the 'forcedAdminMode' value to
*                              all the ports in the card.
*                              L7_FALSE: Apply the application issued adminMode
*                              (stored in hapiPort structure) value.
* @param   forcedAdminMode     Valid only if 'forceMode' is L7_TRUE.
*                              L7_TRUE: Enable the ports.
*                              L7_FALSE: Disable the ports.
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   Uses customx port infra-structure to send the message to the 
*          appropriate unit. The port number in the customx call is
*          dummy parameter.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemCardPortsAdminModeSet(L7_uint32 unit, L7_uint32 slot, 
                                             L7_BOOL forceMode, L7_BOOL forcedAdminMode,
                                             DAPI_t *dapi_g);


/*********************************************************************
*
* @purpose Enable flow control and set the mac if necessary
* 
* @returns Broadcom driver defined
*
* @end
*
*********************************************************************/
extern int hapiBroadFlowControlModeSet(L7_uint32 mode,L7_uchar8 *mac);

/*********************************************************************
*
* @purpose Map a bcm_unit to its mod id, if appropriate
*
* @param   unit         The bcm unit to get the modid for
* @param   modid        (OUT) The modid the unit belongs to
*
* @returns BCM_E_XXX
*
* @end
*
*********************************************************************/
extern int hapiBroadBcmUnitToModid(int bcm_unit, int *modid);


/*********************************************************************
*
* @purpose Enable sdk tracing to go to the dapiTrace
*
* @param   val         1 to enable logging to the dapiTrace, 0 to log to syslog
*
* @returns NONE
*
* @end
*
*********************************************************************/
extern void hapiBroadDebugBcmTrace(int val);

/*********************************************************************
*
* @purpose Enable sdk tracing to go to the screen
*
* @param   val         1 to print to the screen instead of log, 0 for normal
*
* @returns NONE
*
* @end
*
*********************************************************************/
extern void hapiBroadDebugBcmPrint(int val);

/*********************************************************************
*
* @purpose Map a bcm_unit to its mod id, if appropriate
*
* @param   flags      The debug flags as defined by the SDK
* @param   format     Printf format string
* @param   args       variable arg list
*
* @returns BCM_E_XXX
*
* @end
*
*********************************************************************/
extern int hapiBroadCmPrint(uint32 flags, const char *format, va_list args);

#endif
