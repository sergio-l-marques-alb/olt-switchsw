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

bcm_field_qualify_t l3l4Qset[] =    /* IPv4 six-tuple => 160b (+2b: pfq2_ip_has_options) */
{
    bcmFieldQualifyInterfaceClassPort,  /* 16b */
    bcmFieldQualifyInVPort,             /* 16b */
    bcmFieldQualifySrcIp,               /* 32b */
    bcmFieldQualifyDstIp,               /* 32b */
    bcmFieldQualifyIpProtocol,          /*  8b */
    bcmFieldQualifyL4SrcPort,           /* 16b */
    bcmFieldQualifyL4DstPort,           /* 16b */
    bcmFieldQualifyDSCP,                /*  8b */
    bcmFieldQualifyL2Format,            /*  3b */
    bcmFieldQualifyVlanFormat,          /*  4b */
    bcmFieldQualifyIpType,              /*  7b */
    bcmFieldQualifyStageIngress
};

#define l3l4QsetSize (sizeof(l3l4Qset) / sizeof(bcm_field_qualify_t))

bcm_field_qualify_t l2l3SrcQset[] =    /* l2/3 src => 160b (+2b: pfq2_ip_has_options) */
{
    bcmFieldQualifyInterfaceClassPort,  /* 16b */
    bcmFieldQualifyInVPort,             /* 16b */
    bcmFieldQualifySrcMac,              /* 48b */
    bcmFieldQualifySrcIp,               /* 32b */
    bcmFieldQualifyEtherType,           /* 16b */
    bcmFieldQualifyOuterVlan,           /* 16b */
    bcmFieldQualifyL2Format,            /*  3b */
    bcmFieldQualifyVlanFormat,          /*  4b */
    bcmFieldQualifyIpType,              /*  7b */
    bcmFieldQualifyStageIngress
};

#define l2l3SrcQsetSize (sizeof(l2l3SrcQset) / sizeof(bcm_field_qualify_t))

bcm_field_qualify_t l2l3DstQset[] =    /* l2/3 dst => 158b */
{
    bcmFieldQualifyInterfaceClassPort,
    bcmFieldQualifyInVPort,
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
bcm_field_qualify_t l2l3l4Xgs4ClassIdQset[] =    /* l2/l3/l4 => 308b */
{
    bcmFieldQualifyInterfaceClassPort,  /* 16b */
    bcmFieldQualifyInVPort,             /* 16b */
    bcmFieldQualifySrcMac,              /* 48b */
    bcmFieldQualifyDstMac,              /* 48b */
    bcmFieldQualifySrcIp,               /* 32b */
    bcmFieldQualifyDstIp,               /* 32b */
    bcmFieldQualifyOuterVlan,           /* 16b */
    bcmFieldQualifyInnerVlan,           /* 16b */
    bcmFieldQualifyEtherType,           /* 16b */
    bcmFieldQualifyL2Format,            /*  3b */
    bcmFieldQualifyVlanFormat,          /*  4b */
    bcmFieldQualifyIpType,              /*  7b */
    bcmFieldQualifyIpProtocol,          /*  8b */
    bcmFieldQualifyL4SrcPort,           /* 16b */
    bcmFieldQualifyL4DstPort,           /* 16b */
    bcmFieldQualifyDSCP,                /*  8b */
    /*bcmFieldQualifyL2StationMove,
    bcmFieldQualifyL3DestRouteHit,
    bcmFieldQualifyL3DestHostHit,*/
    bcmFieldQualifyStageIngress
};

#define l2l3l4Xgs4ClassIdQsetSize (sizeof(l2l3l4Xgs4ClassIdQset) / sizeof(bcm_field_qualify_t))

/* VLAN + L3 super qset is used to support VLAN ACLs for L3/4 packets.
 * Note that it does not qualify on InPorts, so it will be applied to
 * all ports.
 */
bcm_field_qualify_t vlanl3Qset[] =    /* vlan/l3 => 160b */
{
    bcmFieldQualifyOuterVlan,           /* 16b */
    bcmFieldQualifyInnerVlan,           /* 16b */
    bcmFieldQualifySrcIp,               /* 32b */
    bcmFieldQualifyDstIp,               /* 32b */
    bcmFieldQualifyIpProtocol,          /*  8b */
    bcmFieldQualifyL4SrcPort,           /* 16b */
    bcmFieldQualifyL4DstPort,           /* 16b */
    bcmFieldQualifyDSCP,                /*  8b */
    bcmFieldQualifyL2Format,            /*  3b */
    bcmFieldQualifyVlanFormat,          /*  4b */
    bcmFieldQualifyIpType,              /*  7b */
    bcmFieldQualifyStageIngress
};

#define vlanl3QsetSize (sizeof(vlanl3Qset) / sizeof(bcm_field_qualify_t))


/* Note that Triumph2 doesn't support bcmFieldQualifyTunnelTerminated as
   a qualifier in the IFP. So this sqset excludes 
   bcmFieldQualifyTunnelTerminated compared to systemQsetDouble[],
   and adds bcmFieldQualifyTunnelType and bcmFieldQualifyLoopbackType. */
bcm_field_qualify_t systemQsetArad[] =  /* System requirement => 278b */
{
  bcmFieldQualifyInterfaceClassPort,    /* 16b */
  bcmFieldQualifySrcPort,               /* 16b */
  bcmFieldQualifyDstPort,               /* 19b */
  bcmFieldQualifyInVPort,               /* 16b */
  bcmFieldQualifyOutVPort,              /* 16b */
  bcmFieldQualifyDstMulticastGroup,     /* 19b = bcmFieldQualifyDstPort */
  bcmFieldQualifySrcMac,                /* 48b */
  bcmFieldQualifyDstMac,                /* 48b */
  bcmFieldQualifyOuterVlan,             /* 16b */
  bcmFieldQualifyInnerVlan,             /* 16b */
  bcmFieldQualifyL4SrcPort,             /* 16b */
  bcmFieldQualifyL4DstPort,             /* 16b */
  bcmFieldQualifyEtherType,             /* 16b */
  bcmFieldQualifyIpProtocol,            /*  8b */
  bcmFieldQualifyIpType,                /*  7b */
  bcmFieldQualifyVlanFormat,            /*  4b */

  bcmFieldQualifyStageIngress
};
#define systemQsetAradSize (sizeof(systemQsetArad) / sizeof(bcm_field_qualify_t))

/* PTin added: ICAP */
#if 1
bcm_field_qualify_t systemQsetPTin[] =  /* System requirement => 160b */
{
  //bcmFieldQualifyInterfaceClassPort,    /* 16b */
  bcmFieldQualifyInVPort,               /* 16b */
  //bcmFieldQualifySrcMac,                /* 48b */
  bcmFieldQualifyDstMac,                /* 48b */
  bcmFieldQualifyOuterVlan,             /* 16b */
  bcmFieldQualifyInnerVlan,             /* 16b */
  bcmFieldQualifyL2StationMove,         /*  1b */
  bcmFieldQualifyEtherType,             /* 16b */
  bcmFieldQualifyIpType,                /*  7b */
  //bcmFieldQualifyDrop,                  /* 19b */
  bcmFieldQualifyVlanFormat,            /*  4b */
  bcmFieldQualifyDstIp,                 /* 32b */
  bcmFieldQualifyIntPriority,           /*  3b */
  bcmFieldQualifyStageIngress
};
#define systemQsetPTinSize (sizeof(systemQsetPTin) / sizeof(bcm_field_qualify_t))
#endif

/* Too big */
#if 0
bcm_field_qualify_t ipv6L3L4Qset[] =  /* includes VLAN ID */
{
    bcmFieldQualifyInterfaceClassPort,  /* 16b */
    bcmFieldQualifySrcPort,             /* 16b */
    bcmFieldQualifyInVPort,             /* 16b */
    bcmFieldQualifyEtherType,           /* 16b */
    bcmFieldQualifyOuterVlan,           /* 16b */
    bcmFieldQualifySrcIp6,              /* 128b */
    bcmFieldQualifyDstIp6,              /* 128b */
    bcmFieldQualifyIp6TrafficClass,     /*  8b */
    bcmFieldQualifyIp6FlowLabel,        /* 20b */
    bcmFieldQualifyIp6NextHeader,       /*  8b */
    bcmFieldQualifyIp6HopLimit,         /*  8b */
    bcmFieldQualifyL4SrcPort,           /* 16b */    /* also used for ICMP Msg Type */
    bcmFieldQualifyL4DstPort,           /* 16b */
    bcmFieldQualifyL2StationMove,       /*  1b */
    /*bcmFieldQualifyL3DestRouteHit,
    bcmFieldQualifyL3DestHostHit,
    bcmFieldQualifyIngressStpState,*/
    bcmFieldQualifyL2Format,            /*  3b */
    bcmFieldQualifyVlanFormat,          /*  4b */
    bcmFieldQualifyIpType,              /*  7b */
    bcmFieldQualifyStageIngress
};

#define ipv6L3L4QsetSize (sizeof(ipv6L3L4Qset) / sizeof(bcm_field_qualify_t))
#endif

bcm_field_qualify_t ipv6SrcL4Qset[] =  /* includes VLAN ID => 280b */
{
    bcmFieldQualifyInterfaceClassPort,  /* 16b */
    bcmFieldQualifySrcPort,             /* 16b */
    bcmFieldQualifyInVPort,             /* 16b */
    bcmFieldQualifyOuterVlan,           /* 16b */
    bcmFieldQualifyInnerVlan,           /* 16b */
    bcmFieldQualifySrcIp6,              /* 128b */
    bcmFieldQualifyIp6TrafficClass,     /*  8b */
    bcmFieldQualifyIp6FlowLabel,        /* 20b */
    bcmFieldQualifyIp6NextHeader,       /*  8b */
    bcmFieldQualifyIp6HopLimit,         /*  8b */
    bcmFieldQualifyL4SrcPort,           /* 16b */
    bcmFieldQualifyL4DstPort,           /* 16b */
    bcmFieldQualifyL2StationMove,       /*  1b */
    /*bcmFieldQualifyL3DestRouteHit,
    bcmFieldQualifyL3DestHostHit,
    bcmFieldQualifyIngressStpState,*/
    bcmFieldQualifyIpType,              /*  7b */
    bcmFieldQualifyStageIngress
};

#define ipv6SrcL4QsetSize (sizeof(ipv6SrcL4Qset) / sizeof(bcm_field_qualify_t))

bcm_field_qualify_t ipv6DstL4Qset[] =  /* includes VLAN ID => 280b */
{
    bcmFieldQualifyInterfaceClassPort,  /* 16b */
    bcmFieldQualifySrcPort,             /* 16b */
    bcmFieldQualifyInVPort,             /* 16b */
    bcmFieldQualifyOuterVlan,           /* 16b */
    bcmFieldQualifyInnerVlan,           /* 16b */
    bcmFieldQualifyDstIp6,              /* 128b */
    bcmFieldQualifyIp6TrafficClass,     /*  8b */
    bcmFieldQualifyIp6FlowLabel,        /* 20b */
    bcmFieldQualifyIp6NextHeader,       /*  8b */
    bcmFieldQualifyIp6HopLimit,         /*  8b */
    bcmFieldQualifyL4SrcPort,           /* 16b */
    bcmFieldQualifyL4DstPort,           /* 16b */
    bcmFieldQualifyL2StationMove,       /*  1b */
    /*bcmFieldQualifyL3DestRouteHit,
    bcmFieldQualifyL3DestHostHit,
    bcmFieldQualifyIngressStpState,*/
    bcmFieldQualifyIpType,              /*  7b */
    bcmFieldQualifyStageIngress
};

#define ipv6DstL4QsetSize (sizeof(ipv6DstL4Qset) / sizeof(bcm_field_qualify_t))

/* SQSet used for single wide mode policies */
bcm_field_qualify_t l2QsetEgress[] =    /* l2 => 153b */
{
    //bcmFieldQualifyInterfaceClassPort,  /* 32b */
    //bcmFieldQualifyDstPort,             /* 16b */
    bcmFieldQualifyOutVPort,            /* 24b */
    bcmFieldQualifyDstClassField,       /*  8b */
    bcmFieldQualifySrcMac,              /* 48b */
    bcmFieldQualifyDstMac,              /* 48b */
    bcmFieldQualifyOuterVlan,           /* 16b */
    bcmFieldQualifyVlanFormat,          /*  5b */
    bcmFieldQualifyIntPriority,         /*  4b */
    bcmFieldQualifyStageEgress
};
#define l2QsetEgressSize (sizeof(l2QsetEgress) / sizeof(bcm_field_qualify_t))

bcm_field_qualify_t l3l4QsetEgress[] =    /* l3/l4 => 153b */
{
    //bcmFieldQualifyInterfaceClassPort,  /* 32b */
    //bcmFieldQualifyDstPort,             /* 16b */
    bcmFieldQualifyOutVPort,            /* 24b */
    bcmFieldQualifyDstClassField,       /*  8b */
    //bcmFieldQualifySrcClassField,       /*  8b */
    bcmFieldQualifySrcIp,               /* 32b */
    bcmFieldQualifyDstIp,               /* 32b */
    //bcmFieldQualifyOuterVlan,           /* 16b */
    bcmFieldQualifyIpProtocol,          /*  8b */
    bcmFieldQualifyL4SrcPort,           /* 16b */
    bcmFieldQualifyL4DstPort,           /* 16b */
    bcmFieldQualifyDSCP,                /*  8b */
    bcmFieldQualifyIpType,              /*  4b */
    //bcmFieldQualifyVlanFormat,          /*  5b */
    //bcmFieldQualifyIntPriority,         /*  4b */
    bcmFieldQualifyStageEgress
};
#define l3l4QsetEgressSize (sizeof(l3l4QsetEgress) / sizeof(bcm_field_qualify_t))


bcm_field_action_t destl2AsetIngress[] =  /* System requirement => 38b */
{
  bcmFieldActionVportNew,           /* 17b */
  bcmFieldActionVSwitchNew,         /* 17b */
  bcmFieldActionPrioIntNew,         /*  4b */
};
#define destl2AsetIngressSize (sizeof(destl2AsetIngress) / sizeof(bcm_field_action_t))

bcm_field_action_t destl3AsetIngress[] =  /* System requirement => 37b */
{
  bcmFieldActionIncomingIpIfSet,    /* 13b */
  bcmFieldActionVSwitchNew,         /* 17b */
  bcmFieldActionPrioIntNew,         /*  4b */
  bcmFieldActionDropPrecedence,     /*  3b */
};
#define destl3AsetIngressSize (sizeof(destl3AsetIngress) / sizeof(bcm_field_action_t))


bcm_field_action_t redirectAsetIngress[] =  /* System requirement => 37b */
{
  bcmFieldActionRedirect,           /* 20b */
  bcmFieldActionRedirectMcast,      /* 20b = bcmFieldActionRedirect */
  bcmFieldActionDrop,               /* 20b = bcmFieldActionRedirect */
  bcmFieldActionMirrorIngress,      /*  5b */
  bcmFieldActionVlanActionSetNew,   /*  7b */
  bcmFieldActionOuterVlanPrioNew    /*  5b */
};
#define redirectAsetIngressSize (sizeof(redirectAsetIngress) / sizeof(bcm_field_action_t))

bcm_field_action_t snoopTrapAsetIngress[] =  /* System requirement => 30b */
{
  bcmFieldActionSnoop,              /* 11b */
  bcmFieldActionTrapReduced,        /* 12b */
  bcmFieldActionPrioIntNew,         /*  4b */
  bcmFieldActionDropPrecedence,     /*  3b */
};
#define snoopTrapAsetIngressSize (sizeof(snoopTrapAsetIngress) / sizeof(bcm_field_action_t))

bcm_field_action_t voqAsetIngress[] =  /* System requirement => 36b */
{
  bcmFieldActionISQ,                /* 20b */
  bcmFieldActionVSQ,                /*  9b */
  bcmFieldActionPrioIntNew,         /*  4b */
  bcmFieldActionDropPrecedence,     /*  3b */
};
#define voqAsetIngressSize (sizeof(voqAsetIngress) / sizeof(bcm_field_action_t))

bcm_field_action_t policerAsetIngress[] =  /* System requirement => 36b */
{
  bcmFieldActionPolicerLevel0,      /* 17b */
  bcmFieldActionVlanActionSetNew,   /*  7b */
  bcmFieldActionOuterVlanPrioNew,   /*  5b */
  bcmFieldActionPrioIntNew,         /*  4b */
  bcmFieldActionDropPrecedence,     /*  3b */
};
#define policerAsetIngressSize (sizeof(policerAsetIngress) / sizeof(bcm_field_action_t))

bcm_field_action_t statAsetIngress[] =  /* System requirement => 36b */
{
  bcmFieldActionStat0,              /* 18b */
  bcmFieldActionStatTag,            /*  9b */
  bcmFieldActionVSQ,                /*  9b */
};
#define policerAsetIngressSize (sizeof(policerAsetIngress) / sizeof(bcm_field_action_t))


bcm_field_action_t systemAsetArad[] =  /* System requirement => 39b */
{
  bcmFieldActionTrapReduced,        /* 12b */
  bcmFieldActionRedirect,           /* 20b */
  bcmFieldActionDrop,               /* 20b = bcmFieldActionRedirect */
  bcmFieldActionPrioIntNew,         /*  4b */
  bcmFieldActionDropPrecedence,     /*  3b */
};
#define systemAsetAradSize (sizeof(systemAsetArad) / sizeof(bcm_field_action_t))

bcm_field_action_t systemAsetEgress[] =  /* System requirement => 36b */
{
  bcmFieldActionRedirect,
  bcmFieldActionDrop,
  bcmFieldActionMirrorEgress,
};
#define systemAsetEgressSize (sizeof(systemAsetEgress) / sizeof(bcm_field_action_t))



/* Sqsets for system policies. */
//super_xset_definition_t systemQsetDoubleDef = {systemQsetDouble,      systemQsetDoubleSize,      0, 0};

super_xset_definition_t systemXsetAradDef = {systemQsetArad,    systemQsetAradSize, 0, 0, systemAsetArad,    systemAsetAradSize};

super_xset_definition_t systemQsetPTinDef        = {systemQsetPTin,        systemQsetPTinSize,    0, 0, systemAsetArad,    systemAsetAradSize}; /*Y*/

/* Sqsets for other policies. */
/* Singlewide */
super_xset_definition_t l3l4QsetDef              = {l3l4Qset,              l3l4QsetSize,              0, 0, systemAsetArad,    systemAsetAradSize}; /*Y*/
super_xset_definition_t l2l3SrcQsetDef           = {l2l3SrcQset,           l2l3SrcQsetSize,           0, 0, systemAsetArad,    systemAsetAradSize}; /*Y*/
super_xset_definition_t l2l3DstQsetDef           = {l2l3DstQset,           l2l3DstQsetSize,           0, 0, systemAsetArad,    systemAsetAradSize}; /*Y*/
super_xset_definition_t vlanl3QsetDef            = {vlanl3Qset,            vlanl3QsetSize,            0, 0, systemAsetArad,    systemAsetAradSize}; /*Y*/

/* Doublewide */
super_xset_definition_t l2l3l4Xgs4ClassIdQsetDef = {l2l3l4Xgs4ClassIdQset, l2l3l4Xgs4ClassIdQsetSize, 0, 0, systemAsetArad,    systemAsetAradSize}; /*Y*/
super_xset_definition_t ipv6SrcL4QsetDef         = {ipv6SrcL4Qset,         ipv6SrcL4QsetSize,         0, 0, systemAsetArad,    systemAsetAradSize}; /*Y*/
super_xset_definition_t ipv6DstL4QsetDef         = {ipv6DstL4Qset,         ipv6DstL4QsetSize,         0, 0, systemAsetArad,    systemAsetAradSize}; /*Y*/

/* Quadwide */
//super_xset_definition_t ipv6L3L4QsetDef          = {ipv6L3L4Qset,          ipv6L3L4QsetSize,          0, 0, systemAsetArad,    systemAsetAradSize}; /*Y*/

/* Sqsets for EFP */
super_xset_definition_t l2QsetEgressDef          = {l2QsetEgress,          l2QsetEgressSize,          0, 0, systemAsetEgress,    systemAsetEgressSize}; /*Y*/
super_xset_definition_t l3l4QsetEgressDef        = {l3l4QsetEgress,        l3l4QsetEgressSize,        0, 0, systemAsetEgress,    systemAsetEgressSize}; /*Y*/


