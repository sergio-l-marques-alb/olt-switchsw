/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2010
*
**********************************************************************
*
* @filename  broad_group_sqset.c
*
* @component hapi
*
* @create    1/27/2010
*
* @author    colinw
*
* @end
*
**********************************************************************/
#include "flex.h"
#include "broad_group_sqset.h"

/* Super QSet Definitions */
/* Only qualifiers that were explicitly requested can be used in rule
 * definitions. Therefore, we define super qsets to encompass as many
 * fields as possible to maximize the utility of each group that is
 * created.
 *
 * Each super qset definition must include qualification on
 * 1) PacketFormat in order to match untagged, single tagged, etc.
 * 2) InPorts (PBM) if it is required to qualify on ingress.
 */

bcm_field_qualify_t l2SvtQset[] =   /* single VLAN tag */
{
    bcmFieldQualifyInPorts,
    bcmFieldQualifyDstMac,
    bcmFieldQualifySrcMac,
    bcmFieldQualifyEtherType,
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifyL2Format,
    bcmFieldQualifyIpType,
    bcmFieldQualifyStageIngress
};

#define l2SvtQsetSize (sizeof(l2SvtQset) / sizeof(bcm_field_qualify_t))

/* This qset is the same as l2SvtQset, but includes additional
   qualifiers from LookupStatus. 
   This is necessary on some platforms that don't use systemQset 
   for system policies (e.g. Hawkeye). */
bcm_field_qualify_t l2SvtLookupStatusQset[] =   /* single VLAN tag */
{
    bcmFieldQualifyInPorts,
    bcmFieldQualifyDstMac,
    bcmFieldQualifySrcMac,
    bcmFieldQualifyEtherType,
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifyL2Format,
    bcmFieldQualifyIpType,
    bcmFieldQualifyL2StationMove,
    bcmFieldQualifyL3DestRouteHit,
    bcmFieldQualifyL3DestHostHit,
    bcmFieldQualifyIngressStpState,
    bcmFieldQualifyStageIngress
};

#define l2SvtLookupStatusQsetSize (sizeof(l2SvtLookupStatusQset) / sizeof(bcm_field_qualify_t))

bcm_field_qualify_t l3l4Qset[] =    /* IPv4 six-tuple  */
{
    bcmFieldQualifyInPorts,
    bcmFieldQualifySrcIp,
    bcmFieldQualifyDstIp,
    bcmFieldQualifyIpProtocol,
    bcmFieldQualifyL4SrcPort,
    bcmFieldQualifyL4DstPort,
    bcmFieldQualifyDSCP,
    bcmFieldQualifyL2Format,
    bcmFieldQualifyVlanFormat,
    bcmFieldQualifyIpType,
    bcmFieldQualifyStageIngress
};

#define l3l4QsetSize (sizeof(l3l4Qset) / sizeof(bcm_field_qualify_t))

bcm_field_qualify_t l2l3SrcQset[] =    /* l2/3 src */
{
    bcmFieldQualifyInPorts,
    bcmFieldQualifySrcMac,
    bcmFieldQualifySrcIp,
    bcmFieldQualifyEtherType,
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifyL2Format,
    bcmFieldQualifyVlanFormat,
    bcmFieldQualifyIpType,
    bcmFieldQualifyStageIngress
};

#define l2l3SrcQsetSize (sizeof(l2l3SrcQset) / sizeof(bcm_field_qualify_t))

bcm_field_qualify_t l2l3DstQset[] =    /* l2/3 dst */
{
    bcmFieldQualifyInPorts,
    bcmFieldQualifyDstMac,
    bcmFieldQualifyDstIp,
    bcmFieldQualifyEtherType,
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifyL2Format,
    bcmFieldQualifyVlanFormat,
    bcmFieldQualifyIpType,
    bcmFieldQualifyStageIngress
};

#define l2l3DstQsetSize (sizeof(l2l3DstQset) / sizeof(bcm_field_qualify_t))

/* SQSet used for double wide mode policies */
bcm_field_qualify_t l2l3l4Qset[] =    /* l2/l3/l4 */
{
    bcmFieldQualifyInPorts,
    bcmFieldQualifySrcMac,
    bcmFieldQualifyDstMac,
    bcmFieldQualifySrcIp,
    bcmFieldQualifyDstIp,
    bcmFieldQualifyEtherType,
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifyInnerVlan,
    bcmFieldQualifyL2Format,
    bcmFieldQualifyVlanFormat,
    bcmFieldQualifyIpType,
    bcmFieldQualifyIpProtocol,
    bcmFieldQualifyL4SrcPort,
    bcmFieldQualifyL4DstPort,
    bcmFieldQualifyDSCP,
    bcmFieldQualifyL2StationMove,
    bcmFieldQualifyL3DestRouteHit,
    bcmFieldQualifyL3DestHostHit,
    bcmFieldQualifyIngressStpState,
    bcmFieldQualifyStageIngress, 
    bcmFieldQualifyTcpControl
};

#define l2l3l4QsetSize (sizeof(l2l3l4Qset) / sizeof(bcm_field_qualify_t))

bcm_field_qualify_t l2l3l4SrcMacGroupQset[] =    /* l2/l3/l4 */
{
    bcmFieldQualifyInPorts,
    bcmFieldQualifySrcMac,
    bcmFieldQualifyDstMac,
    bcmFieldQualifySrcIp,
    bcmFieldQualifyDstIp,
    bcmFieldQualifyEtherType,
    bcmFieldQualifyOuterVlan,
/* PTin modified: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
    bcmFieldQualifySrcClassL2,
#else
    bcmFieldQualifySrcMacGroup,
#endif
    bcmFieldQualifyL2Format,
    bcmFieldQualifyVlanFormat,
    bcmFieldQualifyIpType,
    bcmFieldQualifyIpProtocol,
    bcmFieldQualifyL4SrcPort,
    bcmFieldQualifyL4DstPort,
    bcmFieldQualifyDSCP,
    bcmFieldQualifyL2StationMove,
    bcmFieldQualifyIngressStpState,
    bcmFieldQualifyStageIngress, 
    bcmFieldQualifyTcpControl
};

#define l2l3l4SrcMacGroupQsetSize (sizeof(l2l3l4SrcMacGroupQset) / sizeof(bcm_field_qualify_t))

/* The following sqset is used on FB2 to allow classifications on the 
   CLASS_ID determined via the LOOKUP engine. Compared to l2l3l4Qset,
   this qset requires removal of the InnerVlan. */
bcm_field_qualify_t l2l3l4ClassIdQset[] =    /* l2/l3/l4 */
{
    bcmFieldQualifyInPorts,
    bcmFieldQualifySrcMac,
    bcmFieldQualifyDstMac,
    bcmFieldQualifySrcIp,
    bcmFieldQualifyDstIp,
    bcmFieldQualifyEtherType,
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifyL2Format,
    bcmFieldQualifyVlanFormat,
    bcmFieldQualifyIpType,
    bcmFieldQualifyIpProtocol,
    bcmFieldQualifyL4SrcPort,
    bcmFieldQualifyL4DstPort,
    bcmFieldQualifyDSCP,
    bcmFieldQualifyL2StationMove,
    bcmFieldQualifyL3DestRouteHit,
    bcmFieldQualifyL3DestHostHit,
    bcmFieldQualifyIngressStpState,
/* PTin modified: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
    bcmFieldQualifySrcClassField,
    bcmFieldQualifyDstClassField,
#else
    bcmFieldQualifyLookupClass0,
#endif
/* PTin modified: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
    bcmFieldQualifySrcClassL2,
#else
    bcmFieldQualifySrcMacGroup,
#endif
    bcmFieldQualifyStageIngress
};

#define l2l3l4ClassIdQsetSize (sizeof(l2l3l4ClassIdQset) / sizeof(bcm_field_qualify_t))

/* The following sqset is used on Triumph to allow classifications on the
   CLASS_ID determined via the LOOKUP engine. Compared to l2l3l4ClassIdQset,
   this qset requires the InnerVlan. 
*/
bcm_field_qualify_t l2l3l4Xgs4ClassIdQset[] =    /* l2/l3/l4 */
{
    bcmFieldQualifyInPorts,
    bcmFieldQualifySrcMac,
    bcmFieldQualifyDstMac,
    bcmFieldQualifySrcIp,
    bcmFieldQualifyDstIp,
    bcmFieldQualifyEtherType,
    bcmFieldQualifyInnerVlan,
    bcmFieldQualifyOuterVlan,
    /*bcmFieldQualifyL2Format,*/
    bcmFieldQualifyVlanFormat,
    bcmFieldQualifyIpType,
    bcmFieldQualifyIpProtocol,
    bcmFieldQualifyL4SrcPort,
    bcmFieldQualifyL4DstPort,
    bcmFieldQualifyDSCP,
#if 0
    bcmFieldQualifyL2StationMove,
    bcmFieldQualifyL3DestRouteHit,
    bcmFieldQualifyL3DestHostHit,
    bcmFieldQualifyIngressStpState,
/* PTin modified: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
    bcmFieldQualifySrcClassField,
    bcmFieldQualifyDstClassField,
#else
    bcmFieldQualifyLookupClass0,
#endif
/* PTin modified: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
    bcmFieldQualifySrcClassL2,
#else
    bcmFieldQualifySrcMacGroup,
#endif
#endif
  /* PTin added */
#if (PTIN_BOARD != PTIN_BOARD_CXO640G)
  bcmFieldQualifySrcIp6,
  bcmFieldQualifyDstIp6,
  bcmFieldQualifyIp6TrafficClass,
  bcmFieldQualifyIp6FlowLabel,
  bcmFieldQualifyIp6HopLimit,
#endif

  bcmFieldQualifyStageIngress
};


#define l2l3l4Xgs4ClassIdQsetSize (sizeof(l2l3l4Xgs4ClassIdQset) / sizeof(bcm_field_qualify_t))

/* VLAN + L3 super qset is used to support VLAN ACLs for L3/4 packets.
 * Note that it does not qualify on InPorts, so it will be applied to
 * all ports.
 */
bcm_field_qualify_t vlanl3Qset[] =    /* vlan/l3 */
{
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifySrcIp,
    bcmFieldQualifyDstIp,
    bcmFieldQualifyIpProtocol,
    bcmFieldQualifyL4SrcPort,
    bcmFieldQualifyL4DstPort,
    bcmFieldQualifyDSCP,
    bcmFieldQualifyL2Format,
    bcmFieldQualifyVlanFormat,
    bcmFieldQualifyIpType,
    bcmFieldQualifyStageIngress
};

#define vlanl3QsetSize (sizeof(vlanl3Qset) / sizeof(bcm_field_qualify_t))

/* We use this qset on chips that support doublewide mode (e.g. Helix+, FB_B0, FB2, Triumph, etc.). */
bcm_field_qualify_t systemQsetDouble[] =  /* System requirement */
{
    bcmFieldQualifyInPorts,
    bcmFieldQualifyDstMac,
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifyL4SrcPort,
    bcmFieldQualifyL4DstPort,
    bcmFieldQualifyEtherType,
    bcmFieldQualifyIpProtocol,
    bcmFieldQualifyTtl,
    bcmFieldQualifyL2StationMove,
#ifdef L7_IPV6_PACKAGE
    bcmFieldQualifyL3DestRouteHit,
    bcmFieldQualifyL3DestHostHit,
#endif
    bcmFieldQualifyIngressStpState,
    bcmFieldQualifyTunnelTerminated,
    bcmFieldQualifyIpType,
    bcmFieldQualifyStageIngress
};

#define systemQsetDoubleSize (sizeof(systemQsetDouble) / sizeof(bcm_field_qualify_t))

bcm_field_qualify_t systemQset[] =  /* System requirement */
{
    bcmFieldQualifyInPorts,
    bcmFieldQualifyL2StationMove,
    bcmFieldQualifyL3DestRouteHit,
    bcmFieldQualifyL3DestHostHit,
    bcmFieldQualifyIngressStpState,
    bcmFieldQualifyTunnelTerminated,
    bcmFieldQualifyStageIngress
};

#define systemQsetSize (sizeof(systemQset) / sizeof(bcm_field_qualify_t))

/* Note that Triumph2 doesn't support bcmFieldQualifyTunnelTerminated as
   a qualifier in the IFP. So this sqset excludes 
   bcmFieldQualifyTunnelTerminated compared to systemQsetDouble[],
   and adds bcmFieldQualifyTunnelType and bcmFieldQualifyLoopbackType. */
bcm_field_qualify_t systemQsetTriumph2[] =  /* System requirement */
{
  bcmFieldQualifyInPorts,
  bcmFieldQualifySrcMac,
  bcmFieldQualifyDstMac,
  bcmFieldQualifyOuterVlan,
  bcmFieldQualifyInnerVlan,     /* PTin added: FP */
  bcmFieldQualifyL4SrcPort,
  bcmFieldQualifyL4DstPort,
  bcmFieldQualifyEtherType,
  bcmFieldQualifyIpProtocol,
  bcmFieldQualifyTtl,
  bcmFieldQualifyL2StationMove,
#ifdef L7_IPV6_PACKAGE
  bcmFieldQualifyL3DestRouteHit,
  bcmFieldQualifyL3DestHostHit,
#endif
  bcmFieldQualifyIngressStpState,
  bcmFieldQualifyIpType,
  bcmFieldQualifyVlanFormat,    /* PTin added: FP */
  bcmFieldQualifyDSCP,
  #if 0
  bcmFieldQualifyDrop,          /* PTin added: FP */
  bcmFieldQualifySrcTrunk,      /* PTin added: FP */
  bcmFieldQualifyDstIp,         /* PTin added: FP */
  bcmFieldQualifyL2SrcHit,      /* PTin added: FP */
  bcmFieldQualifyL2DestHit,     /* PTin added: FP */
  #endif

#ifdef L7_IPV6_PACKAGE
  bcmFieldQualifyTunnelType,
  bcmFieldQualifyLoopbackType,
#endif
  bcmFieldQualifyStageIngress
};
#define systemQsetTriumph2Size (sizeof(systemQsetTriumph2) / sizeof(bcm_field_qualify_t))

/* PTin added: ICAP */
#if 1
bcm_field_qualify_t systemQsetVlanQoS[] =
{
  bcmFieldQualifyInPorts,
  bcmFieldQualifyOuterVlan,
  bcmFieldQualifyEtherType,
  bcmFieldQualifyDSCP,
/* PTin modified: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  bcmFieldQualifySrcClassField,
#else
  bcmFieldQualifyLookupClass0,
#endif

  bcmFieldQualifyStageIngress
};

#define systemQsetVlanQoSSize (sizeof(systemQsetVlanQoS) / sizeof(bcm_field_qualify_t))


bcm_field_qualify_t systemQsetPTin[] =  /* System requirement */
{
  bcmFieldQualifyInPorts,
#if (PTIN_BOARD != PTIN_BOARD_TG16G)
  bcmFieldQualifySrcTrunk,      /* PTin added: FP */
#endif
  bcmFieldQualifyOuterVlan,
  bcmFieldQualifyInnerVlan,     /* PTin added: FP */
  bcmFieldQualifyDrop,          /* PTin added: FP */

#if (PTIN_BOARD == PTIN_BOARD_CXO160G || \
     PTIN_BOARD == PTIN_BOARD_TA48GE  || \
     PTIN_BOARD == PTIN_BOARD_OLT1T0)
  bcmFieldQualifyIntPriority,   /* PTin added: FP */
#else
 /* PTin added: SDK 6.3.0 */
 #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  bcmFieldQualifyDstClassField, /* PTin added: FP */
 #else
  bcmFieldQualifyLookupClass0,
 #endif
#endif

  bcmFieldQualifyStageIngress
};
#define systemQsetPTinSize (sizeof(systemQsetPTin) / sizeof(bcm_field_qualify_t))

bcm_field_qualify_t systemQsetStats[] =  /* System requirement */
{
  bcmFieldQualifyInPorts,
  bcmFieldQualifySrcTrunk,      /* PTin added: FP */
  bcmFieldQualifySrcMac,
  bcmFieldQualifyDstMac,
  bcmFieldQualifyOuterVlan,
  bcmFieldQualifyInnerVlan,     /* PTin added: FP */
  bcmFieldQualifyEtherType,
  bcmFieldQualifyIpType,
  bcmFieldQualifyDrop,          /* PTin added: FP */
  bcmFieldQualifyVlanFormat,    /* PTin added: FP */
  bcmFieldQualifyDstIp,         /* PTin added: FP */

  bcmFieldQualifyStageIngress
};
#define systemQsetStatsSize (sizeof(systemQsetStats) / sizeof(bcm_field_qualify_t))
#endif

custom_field_qualify_t systemCustomQset[] =  /* System requirement */
{
    BROAD_SYSTEM_UDF,        /* ETHTYPE, PROTO, L4DEST, Tunneled IPV6 Next Header */
};

#define systemCustomQsetSize (sizeof(systemCustomQset) / sizeof(custom_field_qualify_t))

bcm_field_qualify_t iscsiQset[] =  /* iSCSI requirement */
{
    bcmFieldQualifyInPorts,
    bcmFieldQualifyL2StationMove,
    bcmFieldQualifyL3DestRouteHit,
    bcmFieldQualifyL3DestHostHit,
    bcmFieldQualifyIngressStpState,
    bcmFieldQualifyStageIngress
};

#define iscsiQsetSize (sizeof(iscsiQset) / sizeof(bcm_field_qualify_t))

custom_field_qualify_t iscsiCustomQset[] =  /* iSCSI requirement */
{
    BROAD_ISCSI_UDF          /* DSTIP, L4SRC/DEST, iSCSI opCode, iSCSI opCode w/ TCP options */
};

#define iscsiCustomQsetSize (sizeof(iscsiCustomQset) / sizeof(custom_field_qualify_t))

/* ipv6NdQsetScorpion also includes OuterVlan as a qualifier. This allows us to
   fit the COS policies in the same slice as the IPv6 system policies. */
bcm_field_qualify_t ipv6NdQsetScorpion[] =   /* req'd by system policies */
{
    bcmFieldQualifyInPorts,
    bcmFieldQualifyIp6HopLimit,    /* IP6_HOPLIMIT   */
    bcmFieldQualifyIp6NextHeader,  /* IP6_NEXTHEADER   */
    bcmFieldQualifyIpType,
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifyStageIngress
};

#define ipv6NdQsetScorpionSize (sizeof(ipv6NdQsetScorpion) / sizeof(bcm_field_qualify_t))

bcm_field_qualify_t ipv6NdQset[] =   /* req'd by system policies */
{
    bcmFieldQualifyInPorts,
    bcmFieldQualifyIp6HopLimit,    /* IP6_HOPLIMIT   */
    bcmFieldQualifyIp6NextHeader,  /* IP6_NEXTHEADER   */
    bcmFieldQualifyIpType,
    bcmFieldQualifyStageIngress
};

#define ipv6NdQsetSize (sizeof(ipv6NdQset) / sizeof(bcm_field_qualify_t))

bcm_field_qualify_t ipv6L3L4Qset[] =  /* includes VLAN ID */
{
    bcmFieldQualifyInPorts,
    bcmFieldQualifyEtherType,
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifySrcIp6,          
    bcmFieldQualifyDstIp6,          
    bcmFieldQualifyIp6TrafficClass,          
    bcmFieldQualifyIp6FlowLabel,          
    bcmFieldQualifyIp6NextHeader,          
    bcmFieldQualifyIp6HopLimit,          
    bcmFieldQualifyL4SrcPort,    /* also used for ICMP Msg Type */
    bcmFieldQualifyL4DstPort,
    bcmFieldQualifyL2StationMove,
    bcmFieldQualifyL3DestRouteHit,
    bcmFieldQualifyL3DestHostHit,
    bcmFieldQualifyIngressStpState,
    bcmFieldQualifyL2Format,
    bcmFieldQualifyVlanFormat,
    bcmFieldQualifyIpType,
    bcmFieldQualifyStageIngress
};

#define ipv6L3L4QsetSize (sizeof(ipv6L3L4Qset) / sizeof(bcm_field_qualify_t))

/* The following sqset is used on FB2 to allow classifications on the 
   CLASS_ID determined via the LOOKUP engine. Compared to ipv6L3L4Qset,
   this qset requires removal of the Ethertype. */
bcm_field_qualify_t ipv6L3L4ClassIdQset[] =  /* includes VLAN ID */
{
    bcmFieldQualifyInPorts,
    /*bcmFieldQualifySrcMac,*/        /* PTin added: IPv6 ACL rules */
    /*bcmFieldQualifyDstMac,*/        /* PTin added: IPv6 ACL rules */
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifyInnerVlan,     /* PTin added: IPv6 ACL rules */
    bcmFieldQualifyVlanFormat,    /* PTin added: IPv6 ACL rules */
    bcmFieldQualifyEtherType,     /* PTin added: IPv6 ACL rules */
    bcmFieldQualifyIpType,        /* PTin added: IPv6 ACL rules */
    bcmFieldQualifySrcIp6,          
    bcmFieldQualifyDstIp6,          
    bcmFieldQualifyIp6TrafficClass,
    bcmFieldQualifyIp6FlowLabel,          
    bcmFieldQualifyIp6NextHeader,          
    bcmFieldQualifyL4SrcPort,    /* also used for ICMP Msg Type */
    bcmFieldQualifyL4DstPort,
#if 0
    bcmFieldQualifyIp6HopLimit,
    bcmFieldQualifyL2StationMove,
    bcmFieldQualifyL3DestRouteHit,
    bcmFieldQualifyL3DestHostHit,
    bcmFieldQualifyIngressStpState,
    bcmFieldQualifyL2Format,
    bcmFieldQualifyVlanFormat,
    bcmFieldQualifyIpType,
/* PTin modified: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
    bcmFieldQualifySrcClassField,
    bcmFieldQualifyDstClassField,
#else
    bcmFieldQualifyLookupClass0,
#endif
/* PTin modified: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
    bcmFieldQualifySrcClassL2,
#else
    bcmFieldQualifySrcMacGroup,
#endif
#endif
    bcmFieldQualifyStageIngress
};

#define ipv6L3L4ClassIdQsetSize (sizeof(ipv6L3L4ClassIdQset) / sizeof(bcm_field_qualify_t))

bcm_field_qualify_t ipv6SrcL4ClassIdQset[] =  /* includes VLAN ID */
{
    bcmFieldQualifyInPorts,
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifySrcIp6,          
    bcmFieldQualifyIp6NextHeader,          
    bcmFieldQualifyL4SrcPort,
    bcmFieldQualifyL4DstPort,
    bcmFieldQualifyL2StationMove,
    bcmFieldQualifyL3DestRouteHit,
    bcmFieldQualifyL3DestHostHit,
    bcmFieldQualifyIngressStpState,
    bcmFieldQualifyL2Format,
    bcmFieldQualifyVlanFormat,
    bcmFieldQualifyIpType,
/* PTin modified: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
    bcmFieldQualifySrcClassField,
    bcmFieldQualifyDstClassField,
#else
    bcmFieldQualifyLookupClass0,
#endif
/* PTin modified: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
    bcmFieldQualifySrcClassL2,
#else
    bcmFieldQualifySrcMacGroup,
#endif
    bcmFieldQualifyStageIngress
};

#define ipv6SrcL4ClassIdQsetSize (sizeof(ipv6SrcL4ClassIdQset) / sizeof(bcm_field_qualify_t))

bcm_field_qualify_t ipv6DstL4ClassIdQset[] =  /* includes VLAN ID */
{
    bcmFieldQualifyInPorts,
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifyDstIp6,          
    bcmFieldQualifyIp6NextHeader,          
    bcmFieldQualifyL4SrcPort,
    bcmFieldQualifyL4DstPort,
    bcmFieldQualifyL2StationMove,
    bcmFieldQualifyL3DestRouteHit,
    bcmFieldQualifyL3DestHostHit,
    bcmFieldQualifyIngressStpState,
    bcmFieldQualifyL2Format,
    bcmFieldQualifyVlanFormat,
    bcmFieldQualifyIpType,
/* PTin modified: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
    bcmFieldQualifySrcClassField,
    bcmFieldQualifyDstClassField,
#else
    bcmFieldQualifyLookupClass0,
#endif
/* PTin modified: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
    bcmFieldQualifySrcClassL2,
#else
    bcmFieldQualifySrcMacGroup,
#endif
    bcmFieldQualifyStageIngress
};

#define ipv6DstL4ClassIdQsetSize (sizeof(ipv6DstL4ClassIdQset) / sizeof(bcm_field_qualify_t))

bcm_field_qualify_t ipv6SrcL4Qset[] =  /* includes VLAN ID */
{
    bcmFieldQualifyInPorts,
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifySrcIp6,          
    bcmFieldQualifyIp6TrafficClass,          
    bcmFieldQualifyIp6FlowLabel,          
    bcmFieldQualifyIp6NextHeader,          
    bcmFieldQualifyIp6HopLimit,          
    bcmFieldQualifyL4SrcPort,
    bcmFieldQualifyL4DstPort,
    bcmFieldQualifyL2StationMove,
    bcmFieldQualifyL3DestRouteHit,
    bcmFieldQualifyL3DestHostHit,
    bcmFieldQualifyIngressStpState,
    bcmFieldQualifyIpType,
    bcmFieldQualifyStageIngress
};

#define ipv6SrcL4QsetSize (sizeof(ipv6SrcL4Qset) / sizeof(bcm_field_qualify_t))

bcm_field_qualify_t ipv6DstL4Qset[] =  /* includes VLAN ID */
{
    bcmFieldQualifyInPorts,
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifyDstIp6,          
    bcmFieldQualifyIp6TrafficClass,          
    bcmFieldQualifyIp6FlowLabel,          
    bcmFieldQualifyIp6NextHeader,          
    bcmFieldQualifyIp6HopLimit,          
    bcmFieldQualifyL4SrcPort,
    bcmFieldQualifyL4DstPort,
    bcmFieldQualifyL2StationMove,
    bcmFieldQualifyL3DestRouteHit,
    bcmFieldQualifyL3DestHostHit,
    bcmFieldQualifyIngressStpState,
    bcmFieldQualifyIpType,
    bcmFieldQualifyStageIngress
};

#define ipv6DstL4QsetSize (sizeof(ipv6DstL4Qset) / sizeof(bcm_field_qualify_t))

/* PTin added: ECAP */
/* SQSet used for single wide mode policies */
bcm_field_qualify_t ptinQsetEgress[] =    /* l2 */
{
    bcmFieldQualifyDstMac,
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifyInnerVlanId,
/* PTin modified: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
    bcmFieldQualifyInterfaceClassPort,
#else
    bcmFieldQualifyPortClass,
#endif
    bcmFieldQualifyOutPort,
    bcmFieldQualifyVlanFormat,
    bcmFieldQualifyEtherType,
    bcmFieldQualifyIntPriority,
    bcmFieldQualifyStageEgress
};
#define ptinQsetEgressSize (sizeof(ptinQsetEgress) / sizeof(bcm_field_qualify_t))

/* SQSet used for single wide mode policies */
bcm_field_qualify_t l2QsetEgress[] =    /* l2 */
{
    bcmFieldQualifySrcMac,
    bcmFieldQualifyDstMac,
    bcmFieldQualifyEtherType,
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifyInnerVlanId,
/* PTin modified: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
    bcmFieldQualifyInterfaceClassPort,
#else
    bcmFieldQualifyPortClass,
#endif
    bcmFieldQualifyOutPort,
    bcmFieldQualifyVlanFormat,
    bcmFieldQualifyDstIp,         /* PTin added: FP */
    bcmFieldQualifyDrop,          /* PTin added: FP */
    bcmFieldQualifyIntPriority,   /* PTin added: FP */
    bcmFieldQualifyStageEgress
};
#define l2QsetEgressSize (sizeof(l2QsetEgress) / sizeof(bcm_field_qualify_t))

bcm_field_qualify_t l3l4QsetEgress[] =    /* l3/l4 */
{
    bcmFieldQualifySrcIp,
    bcmFieldQualifyDstIp,
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifyIpProtocol,
    bcmFieldQualifyL4SrcPort,
    bcmFieldQualifyL4DstPort,
    bcmFieldQualifyDSCP,
/* PTin modified: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
    bcmFieldQualifyInterfaceClassPort,
#else
    bcmFieldQualifyPortClass,
#endif
    bcmFieldQualifyOutPort,
    bcmFieldQualifyIp4,
    bcmFieldQualifyVlanFormat,
    bcmFieldQualifyDrop,          /* PTin added: FP */
    bcmFieldQualifyIntPriority,   /* PTin added: FP */
    bcmFieldQualifyStageEgress
};
#define l3l4QsetEgressSize (sizeof(l3l4QsetEgress) / sizeof(bcm_field_qualify_t))

/* SQSet used for double wide mode policies */
bcm_field_qualify_t ipv6L3L4QsetEgress[] =  /* includes VLAN ID */
{
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifySrcIp6,          
    bcmFieldQualifyDstIp6,          
    bcmFieldQualifyTos,          
    bcmFieldQualifyIp6NextHeader,          
    bcmFieldQualifyL4SrcPort,     /* also used for ICMP Msg Type */
    bcmFieldQualifyL4DstPort,
/* PTin modified: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
    bcmFieldQualifyInterfaceClassPort,
#else
    bcmFieldQualifyPortClass,
#endif
    bcmFieldQualifyOutPort,
    bcmFieldQualifyIp6,
    bcmFieldQualifyDrop,          /* PTin added: FP */
    bcmFieldQualifyIntPriority,   /* PTin added: FP */
    bcmFieldQualifyStageEgress
};
#define ipv6L3L4QsetEgressSize (sizeof(ipv6L3L4QsetEgress) / sizeof(bcm_field_qualify_t))

/* SQSet used for double wide mode policies */
bcm_field_qualify_t l2l3l4QsetLookup[] =    /* l2/l3 */
{
    bcmFieldQualifyInPort,
/* PTin modified: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
    bcmFieldQualifyInterfaceClassPort,
#else
    bcmFieldQualifyPortClass,
#endif
    bcmFieldQualifySrcMac,
    bcmFieldQualifyDstMac,        /* PTin added: Destination MAC */
    bcmFieldQualifySrcIp,
    bcmFieldQualifyDstIp,
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifyInnerVlan,     /* PTin added: Inner Vlan */
    bcmFieldQualifyIpProtocol,
    bcmFieldQualifyDSCP,
    bcmFieldQualifyL4DstPort,
    bcmFieldQualifyL2Format,
    bcmFieldQualifyVlanFormat,
    bcmFieldQualifyIpType,
    bcmFieldQualifySnap,          /* PTin added: snap */
    bcmFieldQualifyStageLookup
};
#define l2l3l4QsetLookupSize (sizeof(l2l3l4QsetLookup) / sizeof(bcm_field_qualify_t))

/* PTin added: VCAP */
/* SQSet used for single wide mode policies */
bcm_field_qualify_t portVlanQsetLookup[] =    /* l2/l3 */
{
    bcmFieldQualifyInPort,
/* PTin modified: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
    bcmFieldQualifyInterfaceClassPort,
#else
    bcmFieldQualifyPortClass,
#endif
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifyVlanFormat,
    bcmFieldQualifyStageLookup
};
#define portVlanQsetLookupSize (sizeof(portVlanQsetLookup) / sizeof(bcm_field_qualify_t))

/******************************************************************************
 * DOT1AD specific QSET with vlan format and both VLANs                       *
 *****************************************************************************/
bcm_field_qualify_t dot1adQsetLookup[] =    /* dot1ad specific qset */
{                                                  
    bcmFieldQualifyInPort,
/* PTin modified: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
    bcmFieldQualifyInterfaceClassPort,
#else
    bcmFieldQualifyPortClass,
#endif
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifyInnerVlan,
    bcmFieldQualifyL2Format,
    bcmFieldQualifyVlanFormat,
    bcmFieldQualifyIpType,
    bcmFieldQualifyStageLookup
};

#define dot1adQsetLookupSize (sizeof(dot1adQsetLookup) / sizeof(bcm_field_qualify_t))

/* This sqset is shared by LLPF and IPSG. */
static bcm_field_qualify_t llpfQsetLookup[] =    /* llpf specific qset */
{                                                  
    bcmFieldQualifyInPort,
/* PTin modified: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
    bcmFieldQualifyInterfaceClassPort,
#else
    bcmFieldQualifyPortClass,
#endif
    bcmFieldQualifySrcMac,
    bcmFieldQualifyDstMac,
    bcmFieldQualifySrcIp,
    bcmFieldQualifyIpProtocol,
    bcmFieldQualifyL4DstPort,
    bcmFieldQualifyL2Format,
    bcmFieldQualifyIpType,
    bcmFieldQualifySnap,
    bcmFieldQualifyOuterVlan,

    bcmFieldQualifySrcIp6,  /* PTin added: FP */
    //bcmFieldQualifyDstIp,  /* PTin added: FP */
    //bcmFieldQualifyDstIp6,  /* PTin added: FP */
    bcmFieldQualifyEtherType,  /* PTin added: FP */

    bcmFieldQualifyStageLookup
};

#define llpfQsetLookupSize (sizeof(llpfQsetLookup) / sizeof(bcm_field_qualify_t))

bcm_field_qualify_t ipv6L3L4QsetLookup[] =  /* includes VLAN ID */
{
    bcmFieldQualifyInPort,
/* PTin modified: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
    bcmFieldQualifyInterfaceClassPort,
#else
    bcmFieldQualifyPortClass,
#endif
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifySrcMac,
    bcmFieldQualifyDstMac,
    bcmFieldQualifyEtherType,
    bcmFieldQualifySrcIp6,          
    bcmFieldQualifyDSCP,          
    bcmFieldQualifyIpProtocol,
    bcmFieldQualifyL4SrcPort,    /* also used for ICMP Msg Type */
    bcmFieldQualifyL4DstPort,
    bcmFieldQualifyL2Format,
    bcmFieldQualifyVlanFormat,
    bcmFieldQualifyIpType,
    bcmFieldQualifyStageLookup
};
#define ipv6L3L4QsetLookupSize (sizeof(ipv6L3L4QsetLookup) / sizeof(bcm_field_qualify_t))

/* Sqsets for system policies. */
super_qset_definition_t systemQsetDoubleDef      = {systemQsetDouble,      systemQsetDoubleSize,      0, 0};
super_qset_definition_t systemQsetDef            = {systemQset,            systemQsetSize,            
                                                    systemCustomQset,      systemCustomQsetSize};
super_qset_definition_t systemQsetTriumph2Def    = {systemQsetTriumph2,    systemQsetTriumph2Size,    0, 0};
super_qset_definition_t ipv6NdQsetDef            = {ipv6NdQset,            ipv6NdQsetSize,            0, 0};
super_qset_definition_t ipv6NdQsetScorpionDef    = {ipv6NdQsetScorpion,    ipv6NdQsetScorpionSize,    0, 0};

/* PTin added: ICAP */
#if 1
super_qset_definition_t systemQsetVlanQoSDef     = {systemQsetVlanQoS,     systemQsetVlanQoSSize, 0, 0};
super_qset_definition_t systemQsetPTinDef        = {systemQsetPTin,        systemQsetPTinSize,    0, 0};
super_qset_definition_t systemQsetStatsDef       = {systemQsetStats,       systemQsetStatsSize,   0, 0};
#endif

/* Sqsets for other policies. */
/* Singlewide */
super_qset_definition_t l2SvtQsetDef             = {l2SvtQset,             l2SvtQsetSize,             0, 0};
super_qset_definition_t l2SvtLookupStatusQsetDef = {l2SvtLookupStatusQset, l2SvtLookupStatusQsetSize, 0, 0};
super_qset_definition_t l3l4QsetDef              = {l3l4Qset,              l3l4QsetSize,              0, 0};
super_qset_definition_t l2l3SrcQsetDef           = {l2l3SrcQset,           l2l3SrcQsetSize,           0, 0};
super_qset_definition_t l2l3DstQsetDef           = {l2l3DstQset,           l2l3DstQsetSize,           0, 0};
super_qset_definition_t vlanl3QsetDef            = {vlanl3Qset,            vlanl3QsetSize,            0, 0};
super_qset_definition_t iscsiQsetDef             = {iscsiQset,             iscsiQsetSize,             
                                                    iscsiCustomQset,       iscsiCustomQsetSize};

/* Doublewide */
super_qset_definition_t l2l3l4QsetDef            = {l2l3l4Qset,            l2l3l4QsetSize,            0, 0};
super_qset_definition_t l2l3l4SrcMacGroupQsetDef = {l2l3l4SrcMacGroupQset, l2l3l4SrcMacGroupQsetSize, 0, 0};
super_qset_definition_t l2l3l4ClassIdQsetDef     = {l2l3l4ClassIdQset,     l2l3l4ClassIdQsetSize,     0, 0};
super_qset_definition_t l2l3l4Xgs4ClassIdQsetDef = {l2l3l4Xgs4ClassIdQset, l2l3l4Xgs4ClassIdQsetSize, 0, 0};
super_qset_definition_t ipv6SrcL4ClassIdQsetDef  = {ipv6SrcL4ClassIdQset,  ipv6SrcL4ClassIdQsetSize,  0, 0};
super_qset_definition_t ipv6DstL4ClassIdQsetDef  = {ipv6DstL4ClassIdQset,  ipv6DstL4ClassIdQsetSize,  0, 0};
super_qset_definition_t ipv6SrcL4QsetDef         = {ipv6SrcL4Qset,         ipv6SrcL4QsetSize,         0, 0};
super_qset_definition_t ipv6DstL4QsetDef         = {ipv6DstL4Qset,         ipv6DstL4QsetSize,         0, 0};

/* Quadwide */
super_qset_definition_t ipv6L3L4QsetDef          = {ipv6L3L4Qset,          ipv6L3L4QsetSize,          0, 0};
super_qset_definition_t ipv6L3L4ClassIdQsetDef   = {ipv6L3L4ClassIdQset,   ipv6L3L4ClassIdQsetSize,   0, 0};

/* Sqsets for EFP */
super_qset_definition_t ptinQsetEgressDef        = {ptinQsetEgress,        ptinQsetEgressSize,        0, 0};  /* PTin added: ECAP */
super_qset_definition_t l2QsetEgressDef          = {l2QsetEgress,          l2QsetEgressSize,          0, 0};
super_qset_definition_t l3l4QsetEgressDef        = {l3l4QsetEgress,        l3l4QsetEgressSize,        0, 0};
super_qset_definition_t ipv6L3L4QsetEgressDef    = {ipv6L3L4QsetEgress,    ipv6L3L4QsetEgressSize,    0, 0};

/* Sqsets for VFP */
super_qset_definition_t l2l3l4QsetLookupDef      = {l2l3l4QsetLookup,      l2l3l4QsetLookupSize,      0, 0};
super_qset_definition_t portVlanQsetLookupDef    = {portVlanQsetLookup,    portVlanQsetLookupSize,    0, 0};  /* PTin added: VCAP */
super_qset_definition_t dot1adQsetLookupDef      = {dot1adQsetLookup,      dot1adQsetLookupSize,      0, 0};
super_qset_definition_t llpfQsetLookupDef        = {llpfQsetLookup,        llpfQsetLookupSize,        0, 0};
super_qset_definition_t ipv6L3L4QsetLookupDef    = {ipv6L3L4QsetLookup,    ipv6L3L4QsetLookupSize,    0, 0};
