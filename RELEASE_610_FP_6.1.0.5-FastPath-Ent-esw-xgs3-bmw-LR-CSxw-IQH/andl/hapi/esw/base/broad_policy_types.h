
#ifndef BROAD_POLICY_TYPES_H
#define BROAD_POLICY_TYPES_H

#include "l7_common.h"

L7_uint32 hapiBroadMaxPortsPerBcmUnit();
L7_uint32 hapiBroadMaxBcmUnitsPerCpu();
L7_uint32 hapiBroadMaxSystemPolicies();
L7_uint32 hapiBroadMaxPolicies();
L7_uint32 hapiBroadMaxPoliciesPerBcmUnit();
L7_uint32 hapiBroadMaxRules();

#define BROAD_MAX_BCM_UNITS_PER_CPU      hapiBroadMaxBcmUnitsPerCpu()
#define BROAD_MAX_BCM_PORTS_PER_BCM_UNIT hapiBroadMaxPortsPerBcmUnit()

/* Multiple policies can be applied to a port, e.g. COSQ, ACL, etc. */
#define BROAD_MAX_POLICIES               hapiBroadMaxPolicies()

/* Limit the size of policies that USL can sync to reduce memory consumption. */
/* Accomodate System policies, static MAC filters, and VLAN policies */
#define BROAD_MAX_SYSTEM_POLICIES        hapiBroadMaxSystemPolicies()
#define BROAD_MAX_POLICIES_PER_BCM_UNIT  hapiBroadMaxPoliciesPerBcmUnit()
#define BROAD_MAX_RULES_PER_POLICY       (L7_ACL_MAX_RULES_PER_MULTILIST+4)  /* +4 for implict deny all */
#define BROAD_MAX_RULES_PER_SMALL_POLICY  10 /* This is an arbitrary number, 
                                                but we want to keep it as low as possible to 
                                                minimize memory allocation. This number is used to 
                                                keep the USL table as small as possible, since most
                                                system policies require a small number of rules. 
                                                Only VLAN ACLs require the full capacity defined by
                                                BROAD_MAX_RULES_PER_POLICY. */
#define BROAD_MAX_SYSTEM_RULES           32 /* This is an arbitrary number, 
                                               but we want to keep it as low as possible to 
                                               minimize memory allocation.  This number is meant to 
                                               reflect actual system policy rules, not including
                                               VLAN ACL rules. */
#define BROAD_MAX_RULES                  hapiBroadMaxRules()

#define FIELD_MASK_ALL                0    /* don't cares */
#define FIELD_MASK_NONE             (~0)   /* care */

/* Handles */

typedef unsigned int BROAD_POLICY_t;
#define BROAD_POLICY_INVALID        (~0)

typedef unsigned int BROAD_POLICY_RULE_t;
#define BROAD_POLICY_RULE_INVALID   (~0)

typedef struct
{
  L7_BOOL   inUse;
  BROAD_POLICY_RULE_t rule;
  L7_uint32 instance;

} BROAD_POLICY_RULE_LIST_t;

/* Policy Types */
/* Stack ports do not get any policies by default. Instead, to apply a policy to
 * a stack port it is necessary to specifically apply the policy to the corresponding
 * bcmx_lport(s).
 */
typedef unsigned char BROAD_POLICY_TYPE_t;
                                         /* Precedence           Default Port Application         */
#define BROAD_POLICY_TYPE_PORT         0 /*  High                 none - selectively apply/remove */
#define BROAD_POLICY_TYPE_IPSG         1 /*  Medium               none - selectively apply/remove */
#define BROAD_POLICY_TYPE_VLAN         2 /*  Medium               all Eth (non-stacking) ports    */
#define BROAD_POLICY_TYPE_ISCSI        3 /*  Medium               all Eth (non-stacking) ports    */
#define BROAD_POLICY_TYPE_SYSTEM_PORT  4 /*  Low                  none - selectively apply/remove */
#define BROAD_POLICY_TYPE_SYSTEM       5 /*  Low                  all Eth (non-stacking) ports    */
#define BROAD_POLICY_TYPE_COSQ         6 /*  Lowest               none - selectively apply/remove */
#define BROAD_POLICY_TYPE_DVLAN        7 /*  High                 none - selectively apply/remove */
#define BROAD_POLICY_TYPE_DOT1AD_SNOOP 8 /*  High               all Eth (non-stacking) ports    */ 
#define BROAD_POLICY_TYPE_SYS_EXT      9 /*  PTin added: Policies */
#define BROAD_POLICY_TYPE_LAST         10

/* IP Types */

#define BROAD_IP_TYPE_IPV4    0x01  
#define BROAD_IP_TYPE_IPV6    0x02
#define BROAD_IP_TYPE_NONIP   0x04


/* VLAN formats */

#define BROAD_VLAN_FORMAT_STAG        0x01
#define BROAD_VLAN_FORMAT_NOSTAG      0x02
#define BROAD_VLAN_FORMAT_CTAG        0x04
#define BROAD_VLAN_FORMAT_NOCTAG      0x08
#define BROAD_VLAN_FORMAT_STAG_ZERO   0x10
#define BROAD_VLAN_FORMAT_CTAG_ZERO   0x20
#define BROAD_VLAN_FORMAT_UNTAG       0x40


/* Lookup status */

#define BROAD_LOOKUPSTATUS_DOS_ATTACK_PKT         0x8000
#define BROAD_LOOKUPSTATUS_UNRESOLVED_SA          0x4000
#define BROAD_LOOKUPSTATUS_LPM_HIT                0x2000
#define BROAD_LOOKUPSTATUS_STARGV_HIT             0x1000
#define BROAD_LOOKUPSTATUS_L3_DST_HIT             0x0800
#define BROAD_LOOKUPSTATUS_L3_UC_SRC_HIT          0x0400
#define BROAD_LOOKUPSTATUS_L2_USER_ENTRY_HIT      0x0200
#define BROAD_LOOKUPSTATUS_L2_TABLE_DST_L3        0x0100
#define BROAD_LOOKUPSTATUS_L2_DST_HIT             0x0080
#define BROAD_LOOKUPSTATUS_L2_SRC_STATIC          0x0040
#define BROAD_LOOKUPSTATUS_L2_SRC_HIT             0x0020
#define BROAD_LOOKUPSTATUS_INGRESS_SPG_STATE_MASK 0x0018
#define BROAD_LOOKUPSTATUS_INGRESS_SPG_STATE_DIS  0x0000 /* {            */            
#define BROAD_LOOKUPSTATUS_INGRESS_SPG_STATE_BLK  0x0008 /*  only one of */ 
#define BROAD_LOOKUPSTATUS_INGRESS_SPG_STATE_LRN  0x0010 /*  these four  */  
#define BROAD_LOOKUPSTATUS_INGRESS_SPG_STATE_FWD  0x0018 /* }            */            
#define BROAD_LOOKUPSTATUS_FB_VLAN_ID_VALID       0x0004
#define BROAD_LOOKUPSTATUS_VXLT_HIT               0x0002
#define BROAD_LOOKUPSTATUS_TUNNEL_HIT             0x0001

/* Policy Fields */

typedef enum
{
    BROAD_FIELD_MACDA,
    BROAD_FIELD_MACSA,
    BROAD_FIELD_ETHTYPE,
    BROAD_FIELD_OVID,  /* Outer VLAN Id - for SVT or DVT packets */
    BROAD_FIELD_IVID,  /* Inner VLAN Id - only for DVT packets   */
    BROAD_FIELD_DSCP,  /* DSCP or TOS */
    BROAD_FIELD_PROTO,
    BROAD_FIELD_SIP,
    BROAD_FIELD_DIP,
    BROAD_FIELD_SPORT,
    BROAD_FIELD_DPORT,
    BROAD_FIELD_IP6_HOPLIMIT,
    BROAD_FIELD_IP6_NEXTHEADER,
    BROAD_FIELD_LOOKUP_STATUS,
    BROAD_FIELD_IP6_SRC,
    BROAD_FIELD_IP6_DST,
    BROAD_FIELD_IP6_FLOWLABEL,
    BROAD_FIELD_IP6_TRAFFIC_CLASS,
    BROAD_FIELD_ICMP_MSG_TYPE,
    BROAD_FIELD_CLASS_ID,     /* For use in the IFP, determined by VFP results */
    BROAD_FIELD_L2_CLASS_ID,  /* For use in the IFP, determined by L2X results */
    BROAD_FIELD_ISCSI_OPCODE,
    BROAD_FIELD_ISCSI_OPCODE_TCP_OPTIONS, /* Use this field for iSCSI clients that default to 12 bytes of TCP options (e.g. Linux). */
    BROAD_FIELD_TCP_CONTROL,
    BROAD_FIELD_VLAN_FORMAT,
    BROAD_FIELD_IP_TYPE,
    BROAD_FIELD_INPORTS,          /* PTin added */
    BROAD_FIELD_DROP,             /* PTin added */
    BROAD_FIELD_L2_STATION_MOVE,  /* PTin added */
    BROAD_FIELD_L2_SRC_HIT,       /* PTin added */
 /* ... add new fields here ... */
    BROAD_FIELD_LAST,

    /* special bit fields */
    BROAD_FIELD_OCOS = BROAD_FIELD_LAST,  /* Outer CoS - part of Outer VID */
    BROAD_FIELD_ICOS,                     /* Inner CoS - part of Inner VID */
    BROAD_FIELD_SPECIAL_LAST
}
BROAD_POLICY_FIELD_t;

#define BROAD_FIELD_NONE        0
#define BROAD_FIELD_SPECIFIED   1


typedef unsigned char BROAD_POLICY_RULE_PRIORITY_t;
#define BROAD_POLICY_RULE_PRIORITY_LOWEST  0
#define BROAD_POLICY_RULE_PRIORITY_DEFAULT 1
#define BROAD_POLICY_RULE_PRIORITY_HIGHEST 2

/* Policy Actions */
typedef enum
{
 /* Action                         Param0    Param1  Param2   Description                              */
    BROAD_ACTION_SOFT_DROP,     /* n/a       n/a     n/a      drop (affects switched traffic only)     */
    BROAD_ACTION_HARD_DROP,     /* n/a       n/a     n/a      drop all (overrides all other rules)     */
    BROAD_ACTION_PERMIT,        /* n/a       n/a     n/a      permit unless another rule drops         */
    BROAD_ACTION_REDIRECT,      /* unit      slot    port     override switching decision              */
    BROAD_ACTION_MIRROR,        /* unit      slot    port     mirror to lport                          */
    BROAD_ACTION_TRAP_TO_CPU,   /* n/a       n/a     n/a      unconditional trap to cpu, no switching  */
    BROAD_ACTION_COPY_TO_CPU,   /* n/a       n/a     n/a      copy to cpu in addition to switching (1) */
    BROAD_ACTION_SET_COSQ,      /* cosq      n/a     n/a      set cos queue of switched traffic only   */
    BROAD_ACTION_SET_DSCP,      /* dscp      n/a     n/a      set ip dscp in l3 packet                 */
    BROAD_ACTION_SET_TOS,       /* tos       n/a     n/a      set tos in l3 packet                     */
    BROAD_ACTION_SET_USERPRIO,  /* userprio  n/a     n/a      set dot1p in l2 packet                   */
    BROAD_ACTION_SET_DROPPREC,  /* conf      exceed  non-conf set color for different meter output     */
    BROAD_ACTION_SET_OUTER_VID, /* vid       n/a     n/a      set the outer VLAN ID (LOOKUP and EGRESS only) */
    BROAD_ACTION_SET_INNER_VID, /* vid       n/a     n/a      set the inner VLAN ID (EGRESS only)      */
    BROAD_ACTION_ADD_OUTER_VID, /* vid       n/a     n/a      add a new outer VLAN ID (LOOKUP only)    */
    BROAD_ACTION_SET_CLASS_ID,  /* class id  n/a     n/a      set a class ID to be used by IFP (LOOKUP only) */
    BROAD_ACTION_SET_REASON_CODE,/* reason   n/a     n/a      set a reason for RX */
    BROAD_ACTION_SET_USERPRIO_AS_COS2, /* n/a n/a n/a Copy inner TAG priority as dot1p priority in 
                                                  outgoing L2 frame */
 /* etc... */
    BROAD_ACTION_SET_COSQ_AS_PKTPRIO, /* PTin added */
    BROAD_ACTION_REDIRECT_TRUNK,      /* PTin added */
    BROAD_ACTION_LAST

    /* Notes:
     * 1) SET_COSQ only assigns priority to switched traffic, not traffic copied to CPU.
     *    To send traffic to CPU with specified priority it is necessary to TRAP_TO_CPU.
     */
}
BROAD_POLICY_ACTION_t;

#define BROAD_CONFORMING_ACTION_IS_SPECIFIED(_actionPtr, _action)    (_actionPtr->actions[BROAD_POLICY_ACTION_CONFORMING]    & (1 << _action))
#define BROAD_EXCEEDING_ACTION_IS_SPECIFIED(_actionPtr, _action)     (_actionPtr->actions[BROAD_POLICY_ACTION_EXCEEDING]     & (1 << _action))
#define BROAD_NONCONFORMING_ACTION_IS_SPECIFIED(_actionPtr, _action) (_actionPtr->actions[BROAD_POLICY_ACTION_NONCONFORMING] & (1 << _action))
#define BROAD_ACTION_IS_SPECIFIED(_actionPtr, _action)               (BROAD_CONFORMING_ACTION_IS_SPECIFIED(_actionPtr, _action) ||\
                                                                      BROAD_EXCEEDING_ACTION_IS_SPECIFIED(_actionPtr, _action)  ||\
                                                                      BROAD_NONCONFORMING_ACTION_IS_SPECIFIED(_actionPtr, _action))

typedef enum
{
  BROAD_POLICY_ACTION_CONFORMING,    /* applies to conforming traffic */    
  BROAD_POLICY_ACTION_EXCEEDING,     /* applies to exceeding traffic */     
  BROAD_POLICY_ACTION_NONCONFORMING, /* applies to non-conforming traffic */
  BROAD_POLICY_ACTION_LAST
} BROAD_POLICY_ACTION_SCOPE_t;

typedef struct
{
    L7_uint32               actions[BROAD_POLICY_ACTION_LAST];    /* Bitmap of BROAD_POLICY_ACTION_t */

    struct
    {
      /* Redirect action and mirror action are mutually exclusive,
         so they can share these fields. */
      DAPI_USP_t usp;       /* For mirror/redirect */
      int modid, modport;   /* For mirror/redirect */
      int trunkid;          /* PTin added: For redirect_trunk */

      L7_uint32 cpu_modid;
      L7_uint32 cpu_modport;

      L7_uchar8 set_cosq[BROAD_POLICY_ACTION_LAST];
      L7_uchar8 set_dscp[BROAD_POLICY_ACTION_LAST];
      L7_uchar8 set_tos[BROAD_POLICY_ACTION_LAST];
      L7_uchar8 set_userprio[BROAD_POLICY_ACTION_LAST];

      struct
      {
        L7_uchar8 conforming;
        L7_uchar8 exceeding;
        L7_uchar8 nonconforming;
      } set_dropprec;

      L7_ushort16 set_ovid;
      L7_ushort16 set_ivid;
      L7_ushort16 add_ovid;
      L7_uchar8   set_class_id;
      L7_uchar8   set_reason;

    } parms;
}
BROAD_ACTION_ENTRY_t;

/* Drop Precedence */
typedef enum
{
    BROAD_COLOR_GREEN,
    BROAD_COLOR_YELLOW,
    BROAD_COLOR_RED,
    BROAD_COLOR_LAST
}
BROAD_DROP_PRECEDENCE_t;

/* Metering Definitions */

typedef enum
{
    BROAD_METER_COLOR_BLIND,
    BROAD_METER_COLOR_AWARE
}
BROAD_METER_COLOR_MODE_t;

typedef struct
{
    L7_uint64 in_prof;   /* green/yellow */
    L7_uint64 out_prof;  /* red */
}
BROAD_METER_STATS_t;

typedef struct
{
    L7_uint32                cir, cbs;
    L7_uint32                pir, pbs;
    BROAD_METER_COLOR_MODE_t colorMode;
    int                      ffpmtr_id; /* ffp requires meter id */
    bcm_port_t               port; /* ffp requires port */
}
BROAD_METER_ENTRY_t;

/* Counter Definitions */

typedef enum
{
    BROAD_COUNT_PACKETS,   /* matching packet counter */
    BROAD_COUNT_BYTES      /* matching byte counter   */
}
BROAD_COUNTER_MODE_t;

typedef struct
{
    L7_uint64 count;
}
BROAD_COUNTER_STATS_t;

typedef struct
{
    BROAD_COUNTER_MODE_t mode;
    int                  ffpcntr_id; /* ffp requires counter id */
    bcm_port_t           port; /* ffp requires port */
}
BROAD_COUNTER_ENTRY_t;

typedef struct
{
    L7_BOOL   meter;  /* indicates if meter or counter stat */
    
    union
    {
        BROAD_METER_STATS_t   meter;
        BROAD_COUNTER_STATS_t counter;
    }
    statMode;
}
BROAD_POLICY_STATS_t;

/* Define all the field types */

#define BROAD_FIELD_MACDA_SIZE                     6
#define BROAD_FIELD_MACSA_SIZE                     6
#define BROAD_FIELD_ETHTYPE_SIZE                   2
#define BROAD_FIELD_OVID_SIZE                      2
#define BROAD_FIELD_IVID_SIZE                      2
#define BROAD_FIELD_DSCP_SIZE                      1
#define BROAD_FIELD_PROTO_SIZE                     1
#define BROAD_FIELD_SIP_SIZE                       4
#define BROAD_FIELD_DIP_SIZE                       4
#define BROAD_FIELD_SPORT_SIZE                     2
#define BROAD_FIELD_DPORT_SIZE                     2
#define BROAD_FIELD_IP6_HOPLIMIT_SIZE              1
#define BROAD_FIELD_IP6_NEXTHEADER_SIZE            1
#define BROAD_FIELD_LOOKUP_STATUS_SIZE             2
#define BROAD_FIELD_IP6_SRC_SIZE                   16
#define BROAD_FIELD_IP6_DST_SIZE                   16
#define BROAD_FIELD_IP6_FLOWLABEL_SIZE             4
#define BROAD_FIELD_IP6_TRAFFIC_CLASS_SIZE         1
#define BROAD_FIELD_ICMP_MSG_TYPE_SIZE             1
#define BROAD_FIELD_CLASS_ID_SIZE                  1
#define BROAD_FIELD_TCP_CONTROL_SIZE               1
#define BROAD_FIELD_L2_CLASS_ID_SIZE               1
#define BROAD_FIELD_ISCSI_OPCODE_SIZE              1
#define BROAD_FIELD_ISCSI_OPCODE_TCP_OPTIONS_SIZE  1
#define BROAD_FIELD_VLAN_FORMAT_SIZE               1
#define BROAD_FIELD_IP_TYPE_SIZE                   1
#define BROAD_FIELD_INPORTS_SIZE                   sizeof(bcm_pbmp_t)   /* PTin added */
#define BROAD_FIELD_DROP_SIZE                      1    /* PTin added */
#define BROAD_FIELD_L2_STATION_MOVE_SIZE           1    /* PTin added */
#define BROAD_FIELD_L2_SRC_HIT_SIZE                1    /* PTin added */

typedef struct 
{
  L7_uint32 flags;

  struct      /* PTin modified from union to struct */
  {
    struct
    {
      /* MAC SA and MAC DA are included in the union here because currently
         none of the chips can include these qualifiers along w/ IPv6 
         qualifiers. Including them here consumes less memory. If future chips
         allow MAC SA and MAC DA w/ IPv6 qualifiers, then these should be
         moved outside of the union. */
      struct 
      {
        L7_uchar8  value[BROAD_FIELD_MACDA_SIZE];
        L7_uchar8  mask[BROAD_FIELD_MACDA_SIZE];
      } fieldMacDa;

      struct 
      {
        L7_uchar8  value[BROAD_FIELD_MACSA_SIZE];
        L7_uchar8  mask[BROAD_FIELD_MACSA_SIZE];
      } fieldMacSa;

      struct 
      {
        L7_uchar8  value[BROAD_FIELD_SIP_SIZE];
        L7_uchar8  mask[BROAD_FIELD_SIP_SIZE];
      } fieldSip;

      struct 
      {
        L7_uchar8  value[BROAD_FIELD_DIP_SIZE];
        L7_uchar8  mask[BROAD_FIELD_DIP_SIZE];
      } fieldDip;

      struct 
      {
        L7_uchar8  value[BROAD_FIELD_DSCP_SIZE];
        L7_uchar8  mask[BROAD_FIELD_DSCP_SIZE];
      } fieldDscp;

    } l2Ipv4;

    struct
    {
      struct 
      {
        L7_uchar8  value[BROAD_FIELD_IP6_SRC_SIZE];
        L7_uchar8  mask[BROAD_FIELD_IP6_SRC_SIZE];
      } fieldIp6Src;

      struct 
      {
        L7_uchar8  value[BROAD_FIELD_IP6_DST_SIZE];
        L7_uchar8  mask[BROAD_FIELD_IP6_DST_SIZE];
      } fieldIp6Dst;

    } Ipv6;
  } u;

  struct 
  {
    L7_uchar8  value[BROAD_FIELD_ETHTYPE_SIZE];
  } fieldEthtype;

  struct 
  {
    L7_uchar8  value[BROAD_FIELD_OVID_SIZE];
    L7_uchar8  mask[BROAD_FIELD_OVID_SIZE];
  } fieldOvid;

  struct 
  {
    L7_uchar8  value[BROAD_FIELD_IVID_SIZE];
    L7_uchar8  mask[BROAD_FIELD_IVID_SIZE];
  } fieldIvid;

  struct 
  {
    L7_uchar8  value[BROAD_FIELD_PROTO_SIZE];
    L7_uchar8  mask[BROAD_FIELD_PROTO_SIZE];
  } fieldProto;

  struct 
  {
    L7_uchar8  value[BROAD_FIELD_SPORT_SIZE];
    L7_uchar8  mask[BROAD_FIELD_SPORT_SIZE];
  } fieldSport;

  struct 
  {
    L7_uchar8  value[BROAD_FIELD_DPORT_SIZE];
    L7_uchar8  mask[BROAD_FIELD_DPORT_SIZE];
  } fieldDport;

  struct 
  {
    L7_uchar8  value[BROAD_FIELD_IP6_HOPLIMIT_SIZE];
  } fieldIp6Hoplimit;

  struct 
  {
    L7_uchar8  value[BROAD_FIELD_IP6_NEXTHEADER_SIZE];
    L7_uchar8  mask[BROAD_FIELD_IP6_NEXTHEADER_SIZE];
  } fieldIp6Nextheader;

  struct 
  {
    L7_uchar8  value[BROAD_FIELD_LOOKUP_STATUS_SIZE];
    L7_uchar8  mask[BROAD_FIELD_LOOKUP_STATUS_SIZE];
  } fieldLookupStatus;

  struct 
  {
    L7_uchar8  value[BROAD_FIELD_IP6_FLOWLABEL_SIZE];
    L7_uchar8  mask[BROAD_FIELD_IP6_FLOWLABEL_SIZE];
  } fieldIp6Flowlabel;

  struct 
  {
    L7_uchar8  value[BROAD_FIELD_IP6_TRAFFIC_CLASS_SIZE];
    L7_uchar8  mask[BROAD_FIELD_IP6_TRAFFIC_CLASS_SIZE];
  } fieldIp6TrafficClass;

  struct 
  {
    L7_uchar8  value[BROAD_FIELD_ICMP_MSG_TYPE_SIZE];
    L7_uchar8  mask[BROAD_FIELD_ICMP_MSG_TYPE_SIZE];
  } fieldIcmpMsgType;

  struct 
  {
    L7_uchar8  value[BROAD_FIELD_CLASS_ID_SIZE];
  } fieldClassId;
  
  struct 
  {
    L7_uchar8  value[BROAD_FIELD_TCP_CONTROL_SIZE];
    L7_uchar8  mask[BROAD_FIELD_TCP_CONTROL_SIZE];
  } fieldTCPControl;


  struct 
  {
    L7_uchar8  value[BROAD_FIELD_L2_CLASS_ID_SIZE];
  } fieldL2ClassId;

  struct 
  {
    L7_uchar8  value[BROAD_FIELD_ISCSI_OPCODE_SIZE];
    L7_uchar8  mask[BROAD_FIELD_ISCSI_OPCODE_SIZE];
  } fieldIscsiOpcode;

  struct 
  {
    L7_uchar8  value[BROAD_FIELD_VLAN_FORMAT_SIZE];
  } fieldVlanFormat;

  struct 
  {
    L7_uchar8  value[BROAD_FIELD_IP_TYPE_SIZE];
  } fieldIpType;

    // PTin added
  struct 
  {
    L7_uchar8  value[BROAD_FIELD_INPORTS_SIZE];
    L7_uchar8  mask[BROAD_FIELD_INPORTS_SIZE];
  } fieldInports;

  struct 
  {
    L7_uchar8  value[BROAD_FIELD_DROP_SIZE];
  } fieldDrop;

  struct 
  {
    L7_uchar8  value[BROAD_FIELD_L2_STATION_MOVE_SIZE];
  } fieldL2StationMove;

  struct 
  {
    L7_uchar8  value[BROAD_FIELD_L2_SRC_HIT_SIZE];
  } fieldL2SrcHit;
  // PTin end

} BROAD_FIELD_ENTRY_t;

/* Policy Rule Entry */
#define BROAD_METER_SPECIFIED    0x01
#define BROAD_METER_SHARED       0x02    /* aggregate meter */
#define BROAD_COUNTER_SPECIFIED  0x04
#define BROAD_COUNTER_SHARED     0x08    /* aggregate counter */

typedef struct
{ 
    L7_uchar8                     ruleFlags;
    BROAD_POLICY_RULE_PRIORITY_t  priority;
    BROAD_FIELD_ENTRY_t           fieldInfo;
    BROAD_ACTION_ENTRY_t          actionInfo;

    /* Counters and meters are mutually exclusive. */
    union
    {
      struct
      {
        BROAD_METER_ENTRY_t           meterInfo;
      } meter;

      struct
      {
        BROAD_COUNTER_ENTRY_t         counterInfo;
      } counter;
    } u;
    L7_uint32                     meterSrcEntry;  /* src rule number for shared meters/counters */
}
BROAD_POLICY_RULE_ENTRY_t;

typedef struct
{
    L7_int32                      unit;
    L7_int32                      vfpRuleId;
    L7_uint32                     ingressPort;
    L7_BOOL                       upStreamLearn;
    L7_BOOL                       downStreamLearn;
    L7_int32                      portList[4];
    L7_uchar8                     listSize;
    L7_uchar8                     ruleFlags;
    BROAD_POLICY_RULE_PRIORITY_t  priority;
    BROAD_FIELD_ENTRY_t           fieldInfo;
    BROAD_ACTION_ENTRY_t          actionInfo;

    /* Counters and meters are mutually exclusive. */
    union
    {
      struct
      {
        BROAD_METER_ENTRY_t           meterInfo;
      } meter;

      struct
      {
        BROAD_COUNTER_ENTRY_t         counterInfo;
      } counter;
    } u;
    L7_uint32                     meterSrcEntry;  /* src rule number for shared meters/counters */
}bcm_dot1ad_rule_entry_t;


/* Policy Entry */
typedef unsigned char BROAD_POLICY_STAGE_t;
#define BROAD_POLICY_STAGE_LOOKUP  0
#define BROAD_POLICY_STAGE_INGRESS 1
#define BROAD_POLICY_STAGE_EGRESS  2
#define BROAD_POLICY_STAGE_COUNT   3  /* Must be last */

#define BROAD_POLICY_COMMITTED             1    /* policy has been committed to hardware */
#define BROAD_POLICY_CHANGED               2    /* policy has changed since being committed */
#define BROAD_POLICY_FPS                   4    /* policy applies to FPS ports too */
#define BROAD_POLICY_EGRESS_ON_INGRESS     8    /* policy performs egress processing on ingress stage */

typedef struct
{
  BROAD_POLICY_TYPE_t        policyType;
  BROAD_POLICY_STAGE_t       policyStage;
  unsigned char              policyFlags;
#if (BROAD_MAX_RULES_PER_POLICY < 256)
  unsigned char              ruleCount;
#else
  unsigned short             ruleCount;
#endif
  BROAD_POLICY_RULE_ENTRY_t  ruleInfo[BROAD_MAX_RULES_PER_POLICY];
}
BROAD_POLICY_ENTRY_t;

/* The following structure is mostly used by USL to store the majority of system
   policies, which require less rules than VLAN ACLs. */
typedef struct
{
  BROAD_POLICY_TYPE_t        policyType;
  BROAD_POLICY_STAGE_t       policyStage;
  unsigned char              policyFlags;
#if (BROAD_MAX_RULES_PER_POLICY < 256)
  unsigned char              ruleCount;
#else
  unsigned short             ruleCount;
#endif
  BROAD_POLICY_RULE_ENTRY_t  ruleInfo[BROAD_MAX_RULES_PER_SMALL_POLICY];
}
BROAD_POLICY_SMALL_ENTRY_t;

/* Policy Custom Types */

typedef enum
{
    BROAD_CUSTOM_POLICY_CREATE,     /* SET */
    BROAD_CUSTOM_POLICY_DESTROY,    /* SET */
    BROAD_CUSTOM_POLICY_CHECK,      /* GET */
    BROAD_CUSTOM_POLICY_APPLY,      /* SET */
    BROAD_CUSTOM_POLICY_APPLY_ALL,  /* SET */
    BROAD_CUSTOM_POLICY_REMOVE,     /* SET */
    BROAD_CUSTOM_POLICY_REMOVE_ALL, /* SET */
    BROAD_CUSTOM_POLICY_STATS,      /* GET */
}
BROAD_POLICY_CUSTOM_CMD_t;

#define BROAD_POLICY_NONE       0
#define BROAD_POLICY_FIRST      1    /* for fragmenting policy messages */
#define BROAD_POLICY_MIDDLE     2    /* for fragmenting policy messages */
#define BROAD_POLICY_LAST       4    /* for fragmenting policy messages */
#define BROAD_POLICY_COMPRESSED 8    /* for compressing policy data */

typedef struct
{
    BROAD_POLICY_CUSTOM_CMD_t    policyCmd;
    int                          policyFlags;
    BROAD_POLICY_t               policyId;
    int                          policySize;
    union
    {
        BROAD_POLICY_ENTRY_t     policyInfo; /* BROAD_CUSTOM_POLICY_CREATE */
    }
    cmdData;
}
BROAD_POLICY_CUSTOM_DATA_t;

typedef struct
{
  int rpcFlags;
  int rpcRulesIncluded;
  union
  {
    struct
    {
      BROAD_POLICY_CUSTOM_CMD_t policyCmd;
      BROAD_POLICY_t            policyId;
      int                       policySize;
      BROAD_POLICY_TYPE_t       policyType;
      int                       policyFlags;
      BROAD_POLICY_STAGE_t      policyStage;
      int                       ruleCount;
    } policy;
    struct
    {
      BROAD_POLICY_RULE_ENTRY_t firstRule;
    } rules;
  } rpcData;
}
BROAD_POLICY_RPC_DATA_t;

#endif /* BROAD_POLICY_TYPES_H */
