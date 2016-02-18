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
  #if 0
  bcmFieldQualifyDrop,          /* PTin added: FP */
  bcmFieldQualifySrcTrunk,      /* PTin added: FP */
  #endif
  bcmFieldQualifyVlanFormat,    /* PTin added: FP */
  #if 0
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
bcm_field_qualify_t systemQsetPTin[] =  /* System requirement */
{
  bcmFieldQualifyInPorts,
  bcmFieldQualifySrcMac,
  bcmFieldQualifyDstMac,
  bcmFieldQualifyOuterVlan,
  bcmFieldQualifyInnerVlan,     /* PTin added: FP */
  bcmFieldQualifyL2StationMove,
  bcmFieldQualifyEtherType,
  bcmFieldQualifyIpType,
  bcmFieldQualifyDrop,          /* PTin added: FP */
  bcmFieldQualifySrcTrunk,      /* PTin added: FP */
  bcmFieldQualifyVlanFormat,    /* PTin added: FP */
  bcmFieldQualifyDstIp,         /* PTin added: FP */

#if (PTIN_BOARD == PTIN_BOARD_CXO160G || \
     PTIN_BOARD == PTIN_BOARD_TA48GE ||  \
     PTIN_BOARD == PTIN_BOARD_OLT1T0)
  bcmFieldQualifyIntPriority,   /* PTin added: FP */
#endif
/* PTin added: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  bcmFieldQualifyDstClassField, /* PTin added: FP */
  bcmFieldQualifySrcClassField, /* PTin added: FP */
#else
  bcmFieldQualifyLookupClass0,
#endif

  bcmFieldQualifyStageIngress
};
#define systemQsetPTinSize (sizeof(systemQsetPTin) / sizeof(bcm_field_qualify_t))
#endif


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


/* The following sqset is used on FB2 to allow classifications on the 
   CLASS_ID determined via the LOOKUP engine. Compared to ipv6L3L4Qset,
   this qset requires removal of the Ethertype. */
bcm_field_qualify_t ipv6L3L4ClassIdQset[] =  /* includes VLAN ID */
{
    bcmFieldQualifyInPorts,
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
/* PTin modified: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
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

    bcmFieldQualifyStageLookup
};

#define llpfQsetLookupSize (sizeof(llpfQsetLookup) / sizeof(bcm_field_qualify_t))


/* Sqsets for system policies. */
super_qset_definition_t systemQsetTriumph2Def    = {systemQsetTriumph2,    systemQsetTriumph2Size,    0, 0};

/* PTin added: ICAP */
#if 1
super_qset_definition_t systemQsetPTinDef        = {systemQsetPTin,        systemQsetPTinSize,    0, 0};
#endif

/* Sqsets for other policies. */
/* Singlewide */
super_qset_definition_t l2l3SrcQsetDef           = {l2l3SrcQset,           l2l3SrcQsetSize,           0, 0};
super_qset_definition_t l2l3DstQsetDef           = {l2l3DstQset,           l2l3DstQsetSize,           0, 0};
super_qset_definition_t vlanl3QsetDef            = {vlanl3Qset,            vlanl3QsetSize,            0, 0};
super_qset_definition_t iscsiQsetDef             = {iscsiQset,             iscsiQsetSize,             
                                                    iscsiCustomQset,       iscsiCustomQsetSize};

/* Doublewide */
super_qset_definition_t l2l3l4Xgs4ClassIdQsetDef = {l2l3l4Xgs4ClassIdQset, l2l3l4Xgs4ClassIdQsetSize, 0, 0};
super_qset_definition_t ipv6SrcL4ClassIdQsetDef  = {ipv6SrcL4ClassIdQset,  ipv6SrcL4ClassIdQsetSize,  0, 0};
super_qset_definition_t ipv6DstL4ClassIdQsetDef  = {ipv6DstL4ClassIdQset,  ipv6DstL4ClassIdQsetSize,  0, 0};

/* Quadwide */
super_qset_definition_t ipv6L3L4ClassIdQsetDef   = {ipv6L3L4ClassIdQset,   ipv6L3L4ClassIdQsetSize,   0, 0};

/* Sqsets for EFP */
super_qset_definition_t l2QsetEgressDef          = {l2QsetEgress,          l2QsetEgressSize,          0, 0};
super_qset_definition_t l3l4QsetEgressDef        = {l3l4QsetEgress,        l3l4QsetEgressSize,        0, 0};
super_qset_definition_t ipv6L3L4QsetEgressDef    = {ipv6L3L4QsetEgress,    ipv6L3L4QsetEgressSize,    0, 0};

/* Sqsets for VFP */
super_qset_definition_t l2l3l4QsetLookupDef      = {l2l3l4QsetLookup,      l2l3l4QsetLookupSize,      0, 0};
super_qset_definition_t dot1adQsetLookupDef      = {dot1adQsetLookup,      dot1adQsetLookupSize,      0, 0};
super_qset_definition_t llpfQsetLookupDef        = {llpfQsetLookup,        llpfQsetLookupSize,        0, 0};

