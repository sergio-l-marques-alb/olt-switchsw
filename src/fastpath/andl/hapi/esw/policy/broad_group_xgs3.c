/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_group_xgs3.c
*
* This file implements XGS III specific policy code for the local unit.
*
* @component hapi
*
* @create    3/18/2005
*
* @author    robp
*
* @end
*
**********************************************************************/

#include "broad_group_xgs3.h"
#include "bcm/field.h"
#include "bcm/policer.h"    /* PTin added: policer */
#include "ibde.h"
#include "sal/core/libc.h"
#include "osapi_support.h"
#include "broad_group_sqset.h"
#include "broad_group_shuffle.h"
#include "platform_config.h"
#include "logger.h"         /* PTin added: for debug purposes */

/* used for the Higig B0 workaround */
#include <soc/drv.h>
#include <bcm_int/control.h>

extern L7_int32 hpcBroadMasterCpuModPortGet(L7_int32 *modid, L7_int32 *cpuport);


#define MASK_NONE    (~0)
#define MASK_ALL     (0)

#define CHECK_UNIT(u)    {if (u >= SOC_MAX_NUM_DEVICES) LOG_ERROR(u);}

/* Field Map */
/* This table maps the BROAD_FIELD_t enum to BCM API. */

static bcm_field_qualify_t field_map[BROAD_FIELD_LAST] =
{
    bcmFieldQualifyDstMac,     /* MACDA   */
    bcmFieldQualifySrcMac,     /* MACSA   */
    bcmFieldQualifyEtherType,  /* ETHTYPE */
    bcmFieldQualifyOuterVlan,  /* OVID    */
    bcmFieldQualifyInnerVlan,  /* IVID    */
    bcmFieldQualifyDSCP,       /* DSCP    */
    bcmFieldQualifyIpProtocol, /* PROTO   */
    bcmFieldQualifySrcIp,      /* SIP     */
    bcmFieldQualifyDstIp,      /* DIP     */
    bcmFieldQualifyL4SrcPort,  /* SPORT   */
    bcmFieldQualifyL4DstPort,  /* DPORT   */
    bcmFieldQualifyIp6HopLimit,    /* IP6_HOPLIMIT   */
    bcmFieldQualifyIp6NextHeader,  /* IP6_NEXTHEADER   */
    bcmFieldQualifyLookupStatus,   /* LOOKUP_STATUS */
    bcmFieldQualifySrcIp6,         /* Source IPv6 Address */
    bcmFieldQualifyDstIp6,         /* Destination IPv6 Address */
    bcmFieldQualifyIp6FlowLabel,   /* IPv6 Flow Label */
    bcmFieldQualifyIp6TrafficClass,/* IPv6 Traffic Class */
    customFieldQualifyIcmpMsgType, /* ICMP Message Type   */
    //bcmFieldQualifyLookupClass0,    /* Class ID from VFP, to be used in IFP */
    bcmFieldQualifySrcMacGroup,     /* Class ID from L2X, to be used in IFP */
    customFieldQualifyUdf1,         /* iSCSI PDU opCode field */
    customFieldQualifyUdf1,         /* iSCSI PDU opCode field, w/ TCP options */
    bcmFieldQualifyTcpControl,
    bcmFieldQualifyVlanFormat,     /* VLAN Format */
    bcmFieldQualifyL2Format,       /* L2 Header Format */
    bcmFieldQualifySnap,           /* SNAP Header */ 
    bcmFieldQualifyIpType,         /* IP Type */
    bcmFieldQualifyInPorts,        /* InPorts, PTin added: FP */
    bcmFieldQualifyOutPort,        /* OutPort, PTin added: FP */
    bcmFieldQualifySrcTrunk,       /* SrcTrunk, PTin added: FP */
    bcmFieldQualifyPortClass,      /* PortClass, PTin added: FP */
    bcmFieldQualifyDrop,           /* Drop, PTin added: FP */
    //bcmFieldQualifyL2StationMove   /* L2 Station move, PTin added: FP */
};

/* Action Map */
/* This table maps the BROAD_ACTION_t enum to BCM API types. Keep in mind that the same
 * params (0/1/2) are passed to all actions, so they must have compatible params or be
 * specified in separate actions.
 */

#define PROFILE_ACTION_INVALID    bcmFieldActionCount
#define PROFILE_ACTION_NONE       (PROFILE_ACTION_INVALID+1)

#define ACTIONS_PER_MAP_ENTRY     4

typedef struct
{
    bcm_field_action_t green[ACTIONS_PER_MAP_ENTRY];
    bcm_field_action_t yellow[ACTIONS_PER_MAP_ENTRY];
    bcm_field_action_t red[ACTIONS_PER_MAP_ENTRY];
}
action_map_entry_t;

static action_map_entry_t xgs4_ingress_set_cosq_action_map =
    /* SET_COSQ */
    {
      /* PTin modified: CoS */
        { bcmFieldActionGpPrioPktAndIntNew /*bcmFieldActionGpPrioIntNew*/, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
        { bcmFieldActionYpPrioPktAndIntNew /*bcmFieldActionYpPrioIntNew*/, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
        { bcmFieldActionRpPrioPktAndIntNew /*bcmFieldActionRpPrioIntNew*/, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
    };

static action_map_entry_t xgs4_ingress_set_userprio_action_map =
    /* SET_USERPRIO */
    {
        { bcmFieldActionGpPrioPktNew, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
        { bcmFieldActionYpPrioPktNew, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
        { bcmFieldActionRpPrioPktNew, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
    };

static action_map_entry_t xgs4_ingress_set_userprio_as_cos2_action_map =
    /* SET_USERPRIO_AS_COS2 */
    {
        { bcmFieldActionGpPrioPktCopy, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
        { bcmFieldActionYpPrioPktCopy, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
        { bcmFieldActionRpPrioPktCopy, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
    };

static action_map_entry_t xgs4_lookup_copy_to_cpu_action_map =
    /* COPY_TO_CPU */
    {
        { bcmFieldActionCopyToCpu, bcmFieldActionCosQCpuNew,   PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
        { PROFILE_ACTION_INVALID,  PROFILE_ACTION_INVALID,     PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID,  PROFILE_ACTION_INVALID,     PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    };

static action_map_entry_t ingress_action_map[BROAD_ACTION_LAST] =
{
    /* SOFT_DROP - do not switch */
    {
        { bcmFieldActionGpDrop, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE},
        { bcmFieldActionYpDrop, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE},
        { bcmFieldActionRpDrop, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE}
    },
    /* HARD_DROP - override all other rules */
    {
        { bcmFieldActionGpDrop, bcmFieldActionGpCopyToCpuCancel, bcmFieldActionRedirectCancel, PROFILE_ACTION_NONE},
        { bcmFieldActionYpDrop, bcmFieldActionYpCopyToCpuCancel, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE},
        { bcmFieldActionRpDrop, bcmFieldActionRpCopyToCpuCancel, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE}
    },
    /* PERMIT - default behavior */
    {
        { bcmFieldActionGpDropCancel, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
        { bcmFieldActionYpDropCancel, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
        { bcmFieldActionRpDropCancel, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE}
    },
    /* REDIRECT */
    {
        { bcmFieldActionRedirect, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* MIRROR */
    {
        { bcmFieldActionMirrorIngress, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
        { PROFILE_ACTION_INVALID,      PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID,      PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* TRAP_TO_CPU */
    {
        { bcmFieldActionRedirect, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* COPY_TO_CPU */
    {
        { bcmFieldActionCopyToCpu, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
        { PROFILE_ACTION_INVALID,  PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID,  PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_COSQ */
    {
        /* PTin modified: QOS */
        { bcmFieldActionPrioPktAndIntNew /*bcmFieldActionPrioIntNew*/, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
        { PROFILE_ACTION_INVALID,   PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID,   PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_DSCP */
    {
        { bcmFieldActionGpDscpNew, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE},
        { bcmFieldActionYpDscpNew, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE},
        { bcmFieldActionRpDscpNew, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE}
    },
    /* SET_TOS */
    {
        { bcmFieldActionGpDscpNew, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE},
        { bcmFieldActionYpDscpNew, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE},
        { bcmFieldActionRpDscpNew, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE}
    },
    /* SET_USERPRIO */
    {
        { bcmFieldActionPrioPktNew, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
        { PROFILE_ACTION_INVALID,   PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID,   PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_DROPPREC */
    {
        { bcmFieldActionGpDropPrecedence, bcmFieldActionYpDropPrecedence, bcmFieldActionRpDropPrecedence, PROFILE_ACTION_NONE},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_OUTER_VID */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_INNER_VID */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* ADD_OUTER_VID */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* ADD_INNER_VID */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* DO_NOT_LEARN */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_CLASS_ID */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_REASON_CODE */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_USERPRIO_AS_INNER_DOT1P*/
    {
        { bcmFieldActionPrioPktCopy, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
};

static action_map_entry_t lookup_action_map[BROAD_ACTION_LAST] =
{
    /* SOFT_DROP - do not switch */
    {
        { bcmFieldActionDrop,     PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* HARD_DROP - override all other rules */
    {
        { bcmFieldActionDrop,     PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* PERMIT - default behavior */
    {
        { bcmFieldActionDropCancel, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
        { PROFILE_ACTION_INVALID,   PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID,   PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* REDIRECT */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* MIRROR */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* TRAP_TO_CPU */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* COPY_TO_CPU */
    {
        { bcmFieldActionCopyToCpu, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
        { PROFILE_ACTION_INVALID,  PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID,  PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_COSQ */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_DSCP */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_TOS */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_USERPRIO */
    {
        { bcmFieldActionOuterVlanPrioNew, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
        { PROFILE_ACTION_INVALID,         PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID,         PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_DROPPREC */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_OUTER_VID */
    {
        { bcmFieldActionOuterVlanNew, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
        { PROFILE_ACTION_INVALID,     PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID,     PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_INNER_VID */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* ADD_OUTER_VID */
    {
        { bcmFieldActionOuterVlanAdd,  PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
        { PROFILE_ACTION_INVALID,      PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID,      PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* ADD_INNER_VID */
    {
        { bcmFieldActionInnerVlanAdd, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
        { PROFILE_ACTION_INVALID,     PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID,     PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* DO_NOT_LEARN */
    {
        { bcmFieldActionDoNotLearn, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
        { PROFILE_ACTION_INVALID,   PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID,   PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_CLASS_ID */
    {
        { bcmFieldActionClassSet, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_REASON_CODE*/
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID , PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_USERPRIO_AS_INNER_DOT1P*/
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID , PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },

};

static action_map_entry_t egress_action_map[BROAD_ACTION_LAST] =
{
    /* SOFT_DROP - do not switch */
    {
        { bcmFieldActionGpDrop, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE},
        { bcmFieldActionYpDrop, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE},
        { bcmFieldActionRpDrop, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE}
    },
    /* HARD_DROP - override all other rules */
    {
        { bcmFieldActionGpDrop, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE},
        { bcmFieldActionYpDrop, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE},
        { bcmFieldActionRpDrop, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE}
    },
    /* PERMIT - default behavior */
    {
        { bcmFieldActionGpDropCancel, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE},
        { bcmFieldActionYpDropCancel, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE},
        { bcmFieldActionRpDropCancel, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE}
    },
    /* REDIRECT */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* MIRROR */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* TRAP_TO_CPU */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* COPY_TO_CPU */
    {
        { PROFILE_ACTION_INVALID,  PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID,  PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID,  PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_COSQ */
    {
        { PROFILE_ACTION_INVALID,   PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID,   PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID,   PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_DSCP */
    {
        { bcmFieldActionGpDscpNew, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE},
        { bcmFieldActionYpDscpNew, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE},
        { bcmFieldActionRpDscpNew, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE}
    },
    /* SET_TOS */
    {
        { bcmFieldActionGpDscpNew, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE},
        { bcmFieldActionYpDscpNew, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE},
        { bcmFieldActionRpDscpNew, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE}
    },
    /* SET_USERPRIO */
    {
        { bcmFieldActionGpOuterVlanPrioNew, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE},
        { bcmFieldActionYpOuterVlanPrioNew, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE},
        { bcmFieldActionRpOuterVlanPrioNew, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE}
    },
    /* SET_DROPPREC */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_OUTER_VID */
    {
        { bcmFieldActionOuterVlanNew, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
        { PROFILE_ACTION_INVALID,     PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID,     PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_INNER_VID */
    {
        { bcmFieldActionInnerVlanNew, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
        { PROFILE_ACTION_INVALID,     PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID,     PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* ADD_OUTER_VID */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* ADD_INNER_VID */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* DO_NOT_LEARN */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_CLASS_ID */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_REASON_CODE*/
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_USERPRIO_AS_INNER_DOT1P*/
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },

};

/* Drop Precedence (Color) Map */
static uint32 color_map[BROAD_COLOR_LAST] = 
{
    BCM_FIELD_COLOR_GREEN,
    BCM_FIELD_COLOR_YELLOW,
    BCM_FIELD_COLOR_RED
};

static int policy_udf_id[SOC_MAX_NUM_DEVICES];

/* System policies require combination of L2/3/4 fields that are not supported
 * by default, so UDF is required. However, even the UDF is not wide enough
 * to support all system policies in one group. The combination of EthType,
 * Proto and L4 Dst Port is the best we can do. The Super Qset that includes
 * these UDF values can be reused by other policies as needed.
 */
static bcm_field_udf_t sys0_ethType[SOC_MAX_NUM_DEVICES];
static bcm_field_udf_t sys0_ipProto[SOC_MAX_NUM_DEVICES];
static bcm_field_udf_t sys0_l4DstPort[SOC_MAX_NUM_DEVICES];
static bcm_field_udf_t sys0_tunIpv6NextHdr[SOC_MAX_NUM_DEVICES];

/* iSCSI policies require combination of L3/4 fields that are not supported
 * by default, so UDF is required.  The Super Qset that includes
 * these UDF values can be reused by other policies as needed.
 */
static bcm_field_udf_t iscsi_dstIpAddr[SOC_MAX_NUM_DEVICES];
static bcm_field_udf_t iscsi_l4SrcDstPort[SOC_MAX_NUM_DEVICES];
static bcm_field_udf_t iscsi_opCode[SOC_MAX_NUM_DEVICES];
static bcm_field_udf_t iscsi_opCodeTcpOptions[SOC_MAX_NUM_DEVICES];


#define SUPER_QSET_TABLE_SIZE  32    /* total number of super qsets */

static super_qset_entry_t super_qset_table[SOC_MAX_NUM_DEVICES][SUPER_QSET_TABLE_SIZE];

static group_table_t *group_table[SOC_MAX_NUM_DEVICES][BROAD_POLICY_STAGE_COUNT];
static int            group_table_size[SOC_MAX_NUM_DEVICES][BROAD_POLICY_STAGE_COUNT];

#define CHECK_GROUP(u,s,g)  {if ((u>=SOC_MAX_NUM_DEVICES)||(s>=BROAD_POLICY_STAGE_COUNT)||(g>=group_table_size[u][s])) LOG_ERROR(g);}

/* Mapping from BROAD_ENTRY_t to bcm_field_entry_t */
#define BROAD_ENTRY_TO_BCM_ENTRY(entry)   ((bcm_field_entry_t)entry)
#define BCM_ENTRY_TO_BROAD_ENTRY(eid)     ((BROAD_ENTRY_t)eid)

/* Group Allocation Definitions */

typedef struct
{
    int highPrio;     /* highest priority */
    int lowPrio;      /* lowest priority  */
}
group_alloc_table_t;

/* PTin modified: policer */
static group_alloc_table_t group_alloc_table[SOC_MAX_NUM_DEVICES][BROAD_POLICY_STAGE_COUNT][ALLOC_BLOCK_MAX /*ALLOC_BLOCK_HIGH+1*/];

static int _policy_group_lookupstatus_qualify(int unit, bcm_field_entry_t eid, L7_uint32 data, L7_uint32 mask);

/* Utility Functions */

/* Checks if wide mode policies are supported */
static L7_BOOL _policy_supports_wide_mode(int unit)
{
  if (soc_feature(unit, soc_feature_field_wide))
  {
    return L7_TRUE;
  }
  return L7_FALSE;
}

/* Checks if intraslice doublewide mode policies are supported */
static L7_BOOL _policy_supports_intraslice_doublewide_mode(int unit)
{
  if (soc_feature(unit, soc_feature_field_intraslice_double_wide))
  {
    return L7_TRUE;
  }
  return L7_FALSE;
}

int _policy_set_subset(bcm_field_qset_t q1, custom_field_qset_t custom_q1, 
                       bcm_field_qset_t q2, custom_field_qset_t custom_q2)
{
    int  i;

    for (i = 0; i < bcmFieldQualifyCount; i++)
    {
        if (BCM_FIELD_QSET_TEST(q1,i) && !BCM_FIELD_QSET_TEST(q2,i))
            return BCM_E_FAIL;
    }

    for (i = customFieldQualifyFirst; i < customFieldQualifyLast; i++)
    {
        if (CUSTOM_FIELD_QSET_TEST(custom_q1,i) && !CUSTOM_FIELD_QSET_TEST(custom_q2,i))
            return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}

void _policy_set_union(bcm_field_qset_t q1, bcm_field_qset_t *q2)
{
    int  i;

    for (i = 0; i < bcmFieldQualifyCount; i++)
    {
        if (BCM_FIELD_QSET_TEST(q1,i))
            BCM_FIELD_QSET_ADD(*q2,i);
    }
}

/* Super Qset Functions */

int _policy_super_qset_find_match(int                  unit, 
                                  BROAD_POLICY_TYPE_t  type, 
                                  sqsetWidth_t         qsetWidth,
                                  bcm_field_qset_t     qset, 
                                  custom_field_qset_t  customQset, 
                                  int                 *idx)
{
  int  i, /*j, */rv;

//printf("qset      : ");
//for (i=0; i<_SHR_BITDCLSIZE(BCM_FIELD_QUALIFY_MAX); i++)
//{
//  printf("%08X ",qset.w[i]);
//}
//printf("\r\n");

  for (i = 0; i < SUPER_QSET_TABLE_SIZE; i++)
  {
    if (super_qset_table[unit][i].flags & SUPER_QSET_USED)
    {
      bcm_field_qset_t qsetFull;

      if ((super_qset_table[unit][i].applicablePolicyTypes & (1 << type)) == 0)
      {
        /* If this sqset isn't applicable for this type of policy, continue. */
        continue;
      }

      if (super_qset_table[unit][i].sqsetWidth != qsetWidth)
      {
        /* If this sqset doesn't have the proper width for this policy, continue. */
        continue;
      }

      /* consider the UDF when searching for a super qset */
      BCM_FIELD_QSET_INIT(qsetFull);
      _policy_set_union(super_qset_table[unit][i].qsetAgg, &qsetFull);
      _policy_set_union(super_qset_table[unit][i].qsetUdf, &qsetFull);

//    printf("qsetFull  : ");
//    for (j=0; j<_SHR_BITDCLSIZE(BCM_FIELD_QUALIFY_MAX); j++)
//    {
//      printf("%08X ",qsetFull.w[j]);
//    }
//    printf("\r\n");

      rv = _policy_set_subset(qset, customQset, qsetFull, super_qset_table[unit][i].customQset);
      if (BCM_E_NONE == rv)
      {
        *idx = i;
        return BCM_E_NONE;
      }
    }
  }

  return BCM_E_FAIL;
}

static int _policy_super_qset_find_free(int unit, int *idx)
{
    int  i;

    for (i = 0; i < SUPER_QSET_TABLE_SIZE; i++)
    {
        if (!(super_qset_table[unit][i].flags & SUPER_QSET_USED))
        {
            *idx = i;
            return BCM_E_NONE;
        }
    }

    return BCM_E_FAIL;
}

static int _policy_super_qset_add_udf(int unit, bcm_field_qualify_t udf)
{
    int                  rv = BCM_E_NONE;
  int qual_id;
  bcm_field_data_qualifier_t data_qualifier;
  bcm_field_data_packet_format_t packet_format;


    CHECK_UNIT(unit);

    if (BROAD_SYSTEM_UDF == udf)
    {
        /* add UDF for EtherType */
    qual_id = policy_udf_id[unit]++;
    bcm_field_data_qualifier_t_init(&data_qualifier);
    data_qualifier.qual_id = qual_id;
    data_qualifier.flags |= BCM_FIELD_DATA_QUALIFIER_WITH_ID;
    data_qualifier.offset_base = bcmFieldDataOffsetBasePacketStart;
    data_qualifier.offset = 12;
    data_qualifier.length = 2;
    rv = bcm_field_data_qualifier_create(unit, &data_qualifier);
    if (BCM_E_NONE != rv)
      return rv;

    bcm_field_data_packet_format_t_init(&packet_format);
    packet_format.relative_offset = 0;
    packet_format.l2              = BCM_FIELD_DATA_FORMAT_L2_ETH_II;
    packet_format.vlan_tag        = BCM_FIELD_DATA_FORMAT_VLAN_NO_TAG;
    packet_format.outer_ip        = BCM_FIELD_DATA_FORMAT_IP4;
    packet_format.inner_ip        = BCM_FIELD_DATA_FORMAT_IP_NONE;
    packet_format.tunnel          = BCM_FIELD_DATA_FORMAT_TUNNEL_NONE;
    packet_format.mpls            = BCM_FIELD_DATA_FORMAT_MPLS_ANY;
    rv = bcm_field_data_qualifier_packet_format_add(unit, qual_id, &packet_format);
    if (BCM_E_NONE != rv)
      return rv;

    bcm_field_data_packet_format_t_init(&packet_format);
    packet_format.relative_offset = 0;
    packet_format.l2              = BCM_FIELD_DATA_FORMAT_L2_ETH_II;
    packet_format.vlan_tag        = BCM_FIELD_DATA_FORMAT_VLAN_NO_TAG;
    packet_format.outer_ip        = BCM_FIELD_DATA_FORMAT_IP_NONE;
    packet_format.inner_ip        = BCM_FIELD_DATA_FORMAT_IP_NONE;
    packet_format.tunnel          = BCM_FIELD_DATA_FORMAT_TUNNEL_NONE;
    packet_format.mpls            = BCM_FIELD_DATA_FORMAT_MPLS_ANY;
    rv = bcm_field_data_qualifier_packet_format_add(unit, qual_id, &packet_format);
    if (BCM_E_NONE != rv)
      return rv;

    bcm_field_data_packet_format_t_init(&packet_format);
    packet_format.relative_offset = 0;
    packet_format.l2              = BCM_FIELD_DATA_FORMAT_L2_ETH_II;
    packet_format.vlan_tag        = BCM_FIELD_DATA_FORMAT_VLAN_NO_TAG;
    packet_format.outer_ip        = BCM_FIELD_DATA_FORMAT_IP4;
    packet_format.inner_ip        = BCM_FIELD_DATA_FORMAT_IP6;
    packet_format.tunnel          = BCM_FIELD_DATA_FORMAT_TUNNEL_IP_IN_IP;
    packet_format.mpls            = BCM_FIELD_DATA_FORMAT_MPLS_ANY;
    rv = bcm_field_data_qualifier_packet_format_add(unit, qual_id, &packet_format);
    if (BCM_E_NONE != rv)
      return rv;

    bcm_field_data_packet_format_t_init(&packet_format);
    packet_format.relative_offset = 4;
    packet_format.l2              = BCM_FIELD_DATA_FORMAT_L2_ETH_II;
    packet_format.vlan_tag        = BCM_FIELD_DATA_FORMAT_VLAN_SINGLE_TAGGED;
    packet_format.outer_ip        = BCM_FIELD_DATA_FORMAT_IP4;
    packet_format.inner_ip        = BCM_FIELD_DATA_FORMAT_IP_NONE;
    packet_format.tunnel          = BCM_FIELD_DATA_FORMAT_TUNNEL_NONE;
    packet_format.mpls            = BCM_FIELD_DATA_FORMAT_MPLS_ANY;
    rv = bcm_field_data_qualifier_packet_format_add(unit, qual_id, &packet_format);
    if (BCM_E_NONE != rv)
      return rv;

    bcm_field_data_packet_format_t_init(&packet_format);
    packet_format.relative_offset = 4;
    packet_format.l2              = BCM_FIELD_DATA_FORMAT_L2_ETH_II;
    packet_format.vlan_tag        = BCM_FIELD_DATA_FORMAT_VLAN_SINGLE_TAGGED;
    packet_format.outer_ip        = BCM_FIELD_DATA_FORMAT_IP_NONE;
    packet_format.inner_ip        = BCM_FIELD_DATA_FORMAT_IP_NONE;
    packet_format.tunnel          = BCM_FIELD_DATA_FORMAT_TUNNEL_NONE;
    packet_format.mpls            = BCM_FIELD_DATA_FORMAT_MPLS_ANY;
    rv = bcm_field_data_qualifier_packet_format_add(unit, qual_id, &packet_format);
    if (BCM_E_NONE != rv)
      return rv;

    bcm_field_data_packet_format_t_init(&packet_format);
    packet_format.relative_offset = 4;
    packet_format.l2              = BCM_FIELD_DATA_FORMAT_L2_ETH_II;
    packet_format.vlan_tag        = BCM_FIELD_DATA_FORMAT_VLAN_SINGLE_TAGGED;
    packet_format.outer_ip        = BCM_FIELD_DATA_FORMAT_IP4;
    packet_format.inner_ip        = BCM_FIELD_DATA_FORMAT_IP6;
    packet_format.tunnel          = BCM_FIELD_DATA_FORMAT_TUNNEL_IP_IN_IP;
    packet_format.mpls            = BCM_FIELD_DATA_FORMAT_MPLS_ANY;
    rv = bcm_field_data_qualifier_packet_format_add(unit, qual_id, &packet_format);
    if (BCM_E_NONE != rv)
      return rv;

    sys0_ethType[unit] = qual_id;

        /* add UDF for IP Proto */
    qual_id = policy_udf_id[unit]++;
    bcm_field_data_qualifier_t_init(&data_qualifier);
    data_qualifier.qual_id = qual_id;
    data_qualifier.flags |= BCM_FIELD_DATA_QUALIFIER_WITH_ID;
    data_qualifier.offset_base = bcmFieldDataOffsetBaseOuterL3Header;
    data_qualifier.offset = 9;
    data_qualifier.length = 1;
    rv = bcm_field_data_qualifier_create(unit, &data_qualifier);
    if (BCM_E_NONE != rv)
      return rv;

    bcm_field_data_packet_format_t_init(&packet_format);
    packet_format.relative_offset = 0;
    packet_format.l2              = BCM_FIELD_DATA_FORMAT_L2_ETH_II;
    packet_format.vlan_tag        = BCM_FIELD_DATA_FORMAT_VLAN_NO_TAG;
    packet_format.outer_ip        = BCM_FIELD_DATA_FORMAT_IP4;
    packet_format.inner_ip        = BCM_FIELD_DATA_FORMAT_IP_NONE;
    packet_format.tunnel          = BCM_FIELD_DATA_FORMAT_TUNNEL_NONE;
    packet_format.mpls            = BCM_FIELD_DATA_FORMAT_MPLS_ANY;
    rv = bcm_field_data_qualifier_packet_format_add(unit, qual_id, &packet_format);
    if (BCM_E_NONE != rv)
      return rv;

    bcm_field_data_packet_format_t_init(&packet_format);
    packet_format.relative_offset = 0;
    packet_format.l2              = BCM_FIELD_DATA_FORMAT_L2_ETH_II;
    packet_format.vlan_tag        = BCM_FIELD_DATA_FORMAT_VLAN_NO_TAG;
    packet_format.outer_ip        = BCM_FIELD_DATA_FORMAT_IP4;
    packet_format.inner_ip        = BCM_FIELD_DATA_FORMAT_IP6;
    packet_format.tunnel          = BCM_FIELD_DATA_FORMAT_TUNNEL_IP_IN_IP;
    packet_format.mpls            = BCM_FIELD_DATA_FORMAT_MPLS_ANY;
    rv = bcm_field_data_qualifier_packet_format_add(unit, qual_id, &packet_format);
    if (BCM_E_NONE != rv)
      return rv;

    bcm_field_data_packet_format_t_init(&packet_format);
    packet_format.relative_offset = 0;
    packet_format.l2              = BCM_FIELD_DATA_FORMAT_L2_ETH_II;
    packet_format.vlan_tag        = BCM_FIELD_DATA_FORMAT_VLAN_SINGLE_TAGGED;
    packet_format.outer_ip        = BCM_FIELD_DATA_FORMAT_IP4;
    packet_format.inner_ip        = BCM_FIELD_DATA_FORMAT_IP_NONE;
    packet_format.tunnel          = BCM_FIELD_DATA_FORMAT_TUNNEL_NONE;
    packet_format.mpls            = BCM_FIELD_DATA_FORMAT_MPLS_ANY;
    rv = bcm_field_data_qualifier_packet_format_add(unit, qual_id, &packet_format);
    if (BCM_E_NONE != rv)
      return rv;

    bcm_field_data_packet_format_t_init(&packet_format);
    packet_format.relative_offset = 0;
    packet_format.l2              = BCM_FIELD_DATA_FORMAT_L2_ETH_II;
    packet_format.vlan_tag        = BCM_FIELD_DATA_FORMAT_VLAN_SINGLE_TAGGED;
    packet_format.outer_ip        = BCM_FIELD_DATA_FORMAT_IP4;
    packet_format.inner_ip        = BCM_FIELD_DATA_FORMAT_IP6;
    packet_format.tunnel          = BCM_FIELD_DATA_FORMAT_TUNNEL_IP_IN_IP;
    packet_format.mpls            = BCM_FIELD_DATA_FORMAT_MPLS_ANY;
    rv = bcm_field_data_qualifier_packet_format_add(unit, qual_id, &packet_format);
    if (BCM_E_NONE != rv)
      return rv;

    sys0_ipProto[unit] = qual_id;

    /* add UDF for L4 Src/Dst Port */
    qual_id = policy_udf_id[unit]++;
    bcm_field_data_qualifier_t_init(&data_qualifier);
    data_qualifier.qual_id = qual_id;
    data_qualifier.flags |= BCM_FIELD_DATA_QUALIFIER_WITH_ID;
    data_qualifier.flags |= BCM_FIELD_DATA_QUALIFIER_OFFSET_IP4_OPTIONS_ADJUST;
    data_qualifier.offset_base = bcmFieldDataOffsetBaseOuterL4Header;
    data_qualifier.offset = 0;
    data_qualifier.length = 4;
    rv = bcm_field_data_qualifier_create(unit, &data_qualifier);
    if (rv == BCM_E_UNAVAIL)
    {
      /* This chip probably doesn't support the IP option adjustment... try again without it. */
      data_qualifier.flags &= ~(BCM_FIELD_DATA_QUALIFIER_OFFSET_IP4_OPTIONS_ADJUST);
      rv = bcm_field_data_qualifier_create(unit, &data_qualifier);
    }
    if (BCM_E_NONE != rv)
      return rv;

    bcm_field_data_packet_format_t_init(&packet_format);
    packet_format.relative_offset = 0;
    packet_format.l2              = BCM_FIELD_DATA_FORMAT_L2_ETH_II;
    packet_format.vlan_tag        = BCM_FIELD_DATA_FORMAT_VLAN_NO_TAG;
    packet_format.outer_ip        = BCM_FIELD_DATA_FORMAT_IP4;
    packet_format.inner_ip        = BCM_FIELD_DATA_FORMAT_IP_NONE;
    packet_format.tunnel          = BCM_FIELD_DATA_FORMAT_TUNNEL_NONE;
    packet_format.mpls            = BCM_FIELD_DATA_FORMAT_MPLS_ANY;
    rv = bcm_field_data_qualifier_packet_format_add(unit, qual_id, &packet_format);
    if (BCM_E_NONE != rv)
      return rv;

    bcm_field_data_packet_format_t_init(&packet_format);
    packet_format.relative_offset = 0;
    packet_format.l2              = BCM_FIELD_DATA_FORMAT_L2_ETH_II;
    packet_format.vlan_tag        = BCM_FIELD_DATA_FORMAT_VLAN_SINGLE_TAGGED;
    packet_format.outer_ip        = BCM_FIELD_DATA_FORMAT_IP4;
    packet_format.inner_ip        = BCM_FIELD_DATA_FORMAT_IP_NONE;
    packet_format.tunnel          = BCM_FIELD_DATA_FORMAT_TUNNEL_NONE;
    packet_format.mpls            = BCM_FIELD_DATA_FORMAT_MPLS_ANY;
    rv = bcm_field_data_qualifier_packet_format_add(unit, qual_id, &packet_format);
    if (BCM_E_NONE != rv)
      return rv;

    sys0_l4DstPort[unit] = qual_id;

        /* add UDF for tunneled IPv6 (over IPv4) Next Header */
    qual_id = policy_udf_id[unit]++;
    bcm_field_data_qualifier_t_init(&data_qualifier);
    data_qualifier.qual_id = qual_id;
    data_qualifier.flags |= BCM_FIELD_DATA_QUALIFIER_WITH_ID;
    data_qualifier.offset_base = bcmFieldDataOffsetBaseInnerL3Header;
    data_qualifier.offset = 6;
    data_qualifier.length = 1;
    rv = bcm_field_data_qualifier_create(unit, &data_qualifier);
    if (BCM_E_NONE != rv)
      return rv;

    bcm_field_data_packet_format_t_init(&packet_format);
    packet_format.relative_offset = 0;
    packet_format.l2              = BCM_FIELD_DATA_FORMAT_L2_ETH_II;
    packet_format.vlan_tag        = BCM_FIELD_DATA_FORMAT_VLAN_NO_TAG;
    packet_format.outer_ip        = BCM_FIELD_DATA_FORMAT_IP4;
    packet_format.inner_ip        = BCM_FIELD_DATA_FORMAT_IP6;
    packet_format.tunnel          = BCM_FIELD_DATA_FORMAT_TUNNEL_IP_IN_IP;
    packet_format.mpls            = BCM_FIELD_DATA_FORMAT_MPLS_ANY;
    rv = bcm_field_data_qualifier_packet_format_add(unit, qual_id, &packet_format);
    if (BCM_E_NONE != rv)
      return rv;

    bcm_field_data_packet_format_t_init(&packet_format);
    packet_format.relative_offset = 0;
    packet_format.l2              = BCM_FIELD_DATA_FORMAT_L2_ETH_II;
    packet_format.vlan_tag        = BCM_FIELD_DATA_FORMAT_VLAN_SINGLE_TAGGED;
    packet_format.outer_ip        = BCM_FIELD_DATA_FORMAT_IP4;
    packet_format.inner_ip        = BCM_FIELD_DATA_FORMAT_IP6;
    packet_format.tunnel          = BCM_FIELD_DATA_FORMAT_TUNNEL_IP_IN_IP;
    packet_format.mpls            = BCM_FIELD_DATA_FORMAT_MPLS_ANY;
    rv = bcm_field_data_qualifier_packet_format_add(unit, qual_id, &packet_format);
    if (BCM_E_NONE != rv)
      return rv;

    sys0_tunIpv6NextHdr[unit] = qual_id;
    } 
    else if (BROAD_ISCSI_UDF == udf)
    {
        /* add UDF for Destination IPv4 address */
    qual_id = policy_udf_id[unit]++;
    bcm_field_data_qualifier_t_init(&data_qualifier);
    data_qualifier.qual_id = qual_id;
    data_qualifier.flags |= BCM_FIELD_DATA_QUALIFIER_WITH_ID;
    data_qualifier.offset_base = bcmFieldDataOffsetBaseOuterL3Header;
    data_qualifier.offset = 16;
    data_qualifier.length = 4;
    rv = bcm_field_data_qualifier_create(unit, &data_qualifier);
    if (BCM_E_NONE != rv)
      return rv;

    bcm_field_data_packet_format_t_init(&packet_format);
    packet_format.relative_offset = 0;
    packet_format.l2              = BCM_FIELD_DATA_FORMAT_L2_ETH_II;
    packet_format.vlan_tag        = BCM_FIELD_DATA_FORMAT_VLAN_NO_TAG;
    packet_format.outer_ip        = BCM_FIELD_DATA_FORMAT_IP4;
    packet_format.inner_ip        = BCM_FIELD_DATA_FORMAT_IP_NONE;
    packet_format.tunnel          = BCM_FIELD_DATA_FORMAT_TUNNEL_NONE;
    packet_format.mpls            = BCM_FIELD_DATA_FORMAT_MPLS_ANY;
    rv = bcm_field_data_qualifier_packet_format_add(unit, qual_id, &packet_format);
    if (BCM_E_NONE != rv)
      return rv;

    bcm_field_data_packet_format_t_init(&packet_format);
    packet_format.relative_offset = 0;
    packet_format.l2              = BCM_FIELD_DATA_FORMAT_L2_ETH_II;
    packet_format.vlan_tag        = BCM_FIELD_DATA_FORMAT_VLAN_SINGLE_TAGGED;
    packet_format.outer_ip        = BCM_FIELD_DATA_FORMAT_IP4;
    packet_format.inner_ip        = BCM_FIELD_DATA_FORMAT_IP_NONE;
    packet_format.tunnel          = BCM_FIELD_DATA_FORMAT_TUNNEL_NONE;
    packet_format.mpls            = BCM_FIELD_DATA_FORMAT_MPLS_ANY;
    rv = bcm_field_data_qualifier_packet_format_add(unit, qual_id, &packet_format);
    if (BCM_E_NONE != rv)
      return rv;

    iscsi_dstIpAddr[unit] = qual_id;

    /* add UDF for L4 Src/Dst Port */
    qual_id = policy_udf_id[unit]++;
    bcm_field_data_qualifier_t_init(&data_qualifier);
    data_qualifier.qual_id = qual_id;
    data_qualifier.flags |= BCM_FIELD_DATA_QUALIFIER_WITH_ID;
    data_qualifier.flags |= BCM_FIELD_DATA_QUALIFIER_OFFSET_IP4_OPTIONS_ADJUST;
    data_qualifier.offset_base = bcmFieldDataOffsetBaseOuterL4Header;
    data_qualifier.offset = 0;
    data_qualifier.length = 4;
    rv = bcm_field_data_qualifier_create(unit, &data_qualifier);
    if (rv == BCM_E_UNAVAIL)
    {
      /* This chip probably doesn't support the IP option adjustment... try again without it. */
      data_qualifier.flags &= ~(BCM_FIELD_DATA_QUALIFIER_OFFSET_IP4_OPTIONS_ADJUST);
      rv = bcm_field_data_qualifier_create(unit, &data_qualifier);
    }
    if (BCM_E_NONE != rv)
      return rv;

    bcm_field_data_packet_format_t_init(&packet_format);
    packet_format.relative_offset = 0;
    packet_format.l2              = BCM_FIELD_DATA_FORMAT_L2_ETH_II;
    packet_format.vlan_tag        = BCM_FIELD_DATA_FORMAT_VLAN_NO_TAG;
    packet_format.outer_ip        = BCM_FIELD_DATA_FORMAT_IP4;
    packet_format.inner_ip        = BCM_FIELD_DATA_FORMAT_IP_NONE;
    packet_format.tunnel          = BCM_FIELD_DATA_FORMAT_TUNNEL_NONE;
    packet_format.mpls            = BCM_FIELD_DATA_FORMAT_MPLS_ANY;
    rv = bcm_field_data_qualifier_packet_format_add(unit, qual_id, &packet_format);
    if (BCM_E_NONE != rv)
      return rv;

    bcm_field_data_packet_format_t_init(&packet_format);
    packet_format.relative_offset = 0;
    packet_format.l2              = BCM_FIELD_DATA_FORMAT_L2_ETH_II;
    packet_format.vlan_tag        = BCM_FIELD_DATA_FORMAT_VLAN_SINGLE_TAGGED;
    packet_format.outer_ip        = BCM_FIELD_DATA_FORMAT_IP4;
    packet_format.inner_ip        = BCM_FIELD_DATA_FORMAT_IP_NONE;
    packet_format.tunnel          = BCM_FIELD_DATA_FORMAT_TUNNEL_NONE;
    packet_format.mpls            = BCM_FIELD_DATA_FORMAT_MPLS_ANY;
    rv = bcm_field_data_qualifier_packet_format_add(unit, qual_id, &packet_format);
    if (BCM_E_NONE != rv)
      return rv;

    iscsi_l4SrcDstPort[unit] = qual_id;

        /* add UDF for iSCSI PDU opcode field (assumes no TCP options). */
    qual_id = policy_udf_id[unit]++;
    bcm_field_data_qualifier_t_init(&data_qualifier);
    data_qualifier.qual_id = qual_id;
    data_qualifier.flags |= BCM_FIELD_DATA_QUALIFIER_WITH_ID;
    data_qualifier.flags |= BCM_FIELD_DATA_QUALIFIER_OFFSET_IP4_OPTIONS_ADJUST;
    data_qualifier.offset_base = bcmFieldDataOffsetBaseOuterL4Header;
    data_qualifier.offset = 20;
    data_qualifier.length = 1;
    rv = bcm_field_data_qualifier_create(unit, &data_qualifier);
    if (rv == BCM_E_UNAVAIL)
    {
      /* This chip probably doesn't support the IP option adjustment... try again without it. */
      data_qualifier.flags &= ~(BCM_FIELD_DATA_QUALIFIER_OFFSET_IP4_OPTIONS_ADJUST);
      rv = bcm_field_data_qualifier_create(unit, &data_qualifier);
    }
    if (BCM_E_NONE != rv)
      return rv;

    bcm_field_data_packet_format_t_init(&packet_format);
    packet_format.relative_offset = 0;
    packet_format.l2              = BCM_FIELD_DATA_FORMAT_L2_ETH_II;
    packet_format.vlan_tag        = BCM_FIELD_DATA_FORMAT_VLAN_NO_TAG;
    packet_format.outer_ip        = BCM_FIELD_DATA_FORMAT_IP4;
    packet_format.inner_ip        = BCM_FIELD_DATA_FORMAT_IP_NONE;
    packet_format.tunnel          = BCM_FIELD_DATA_FORMAT_TUNNEL_NONE;
    packet_format.mpls            = BCM_FIELD_DATA_FORMAT_MPLS_ANY;
    rv = bcm_field_data_qualifier_packet_format_add(unit, qual_id, &packet_format);
    if (BCM_E_NONE != rv)
      return rv;

    bcm_field_data_packet_format_t_init(&packet_format);
    packet_format.relative_offset = 0;
    packet_format.l2              = BCM_FIELD_DATA_FORMAT_L2_ETH_II;
    packet_format.vlan_tag        = BCM_FIELD_DATA_FORMAT_VLAN_SINGLE_TAGGED;
    packet_format.outer_ip        = BCM_FIELD_DATA_FORMAT_IP4;
    packet_format.inner_ip        = BCM_FIELD_DATA_FORMAT_IP_NONE;
    packet_format.tunnel          = BCM_FIELD_DATA_FORMAT_TUNNEL_NONE;
    packet_format.mpls            = BCM_FIELD_DATA_FORMAT_MPLS_ANY;
    rv = bcm_field_data_qualifier_packet_format_add(unit, qual_id, &packet_format);
    if (BCM_E_NONE != rv)
      return rv;

    iscsi_opCode[unit] = qual_id;

        /* add UDF for iSCSI PDU opcode field; include 12 bytes of TCP options (typical of Linux iSCSI clients). */
    qual_id = policy_udf_id[unit]++;
    bcm_field_data_qualifier_t_init(&data_qualifier);
    data_qualifier.qual_id = qual_id;
    data_qualifier.flags |= BCM_FIELD_DATA_QUALIFIER_WITH_ID;
    data_qualifier.flags |= BCM_FIELD_DATA_QUALIFIER_OFFSET_IP4_OPTIONS_ADJUST;
    data_qualifier.offset_base = bcmFieldDataOffsetBaseOuterL4Header;
    data_qualifier.offset = 32;
    data_qualifier.length = 1;
    rv = bcm_field_data_qualifier_create(unit, &data_qualifier);
    if (rv == BCM_E_UNAVAIL)
    {
      /* This chip probably doesn't support the IP option adjustment... try again without it. */
      data_qualifier.flags &= ~(BCM_FIELD_DATA_QUALIFIER_OFFSET_IP4_OPTIONS_ADJUST);
      rv = bcm_field_data_qualifier_create(unit, &data_qualifier);
    }
    if (BCM_E_NONE != rv)
      return rv;

    bcm_field_data_packet_format_t_init(&packet_format);
    packet_format.relative_offset = 0;
    packet_format.l2              = BCM_FIELD_DATA_FORMAT_L2_ETH_II;
    packet_format.vlan_tag        = BCM_FIELD_DATA_FORMAT_VLAN_NO_TAG;
    packet_format.outer_ip        = BCM_FIELD_DATA_FORMAT_IP4;
    packet_format.inner_ip        = BCM_FIELD_DATA_FORMAT_IP_NONE;
    packet_format.tunnel          = BCM_FIELD_DATA_FORMAT_TUNNEL_NONE;
    packet_format.mpls            = BCM_FIELD_DATA_FORMAT_MPLS_ANY;
    rv = bcm_field_data_qualifier_packet_format_add(unit, qual_id, &packet_format);
    if (BCM_E_NONE != rv)
      return rv;

    bcm_field_data_packet_format_t_init(&packet_format);
    packet_format.relative_offset = 0;
    packet_format.l2              = BCM_FIELD_DATA_FORMAT_L2_ETH_II;
    packet_format.vlan_tag        = BCM_FIELD_DATA_FORMAT_VLAN_SINGLE_TAGGED;
    packet_format.outer_ip        = BCM_FIELD_DATA_FORMAT_IP4;
    packet_format.inner_ip        = BCM_FIELD_DATA_FORMAT_IP_NONE;
    packet_format.tunnel          = BCM_FIELD_DATA_FORMAT_TUNNEL_NONE;
    packet_format.mpls            = BCM_FIELD_DATA_FORMAT_MPLS_ANY;
    rv = bcm_field_data_qualifier_packet_format_add(unit, qual_id, &packet_format);
    if (BCM_E_NONE != rv)
      return rv;

    iscsi_opCodeTcpOptions[unit] = qual_id;
    } 
    else
    {
        rv = BCM_E_NOT_FOUND;
    }

    return rv;
}

static int _policy_super_qset_add(int                      unit,
                                  super_qset_definition_t *sqset_def,
                                  L7_BOOL                 *applicablePolicyTypes)
{
    int                      i;
    int                      rv = BCM_E_NONE;
    bcm_field_qset_t         qset1;
    super_qset_entry_t      *qsetPtr;
    bcm_field_qualify_t     *q1;
    L7_uint32                q1Size;
    custom_field_qualify_t  *q2;
    L7_uint32                q2Size;
    L7_uint32                gid;

    CHECK_UNIT(unit);

    q1     = sqset_def->standardQualifiers;
    q1Size = sqset_def->standardQualifiersCount;
    q2     = sqset_def->customQualifiers;
    q2Size = sqset_def->customQualifiersCount;

    rv = _policy_super_qset_find_free(unit, &i);
    if (BCM_E_NONE != rv)
    {
      LOG_ERROR(rv);  /* Need to increase SUPER_QSET_TABLE_SIZE. */
      return rv;
    }

    qsetPtr = &super_qset_table[unit][i];

    qsetPtr->flags = SUPER_QSET_USED;

    if(BROAD_POLICY_TYPE_LAST > 32)
    {
      /* We can only accommodate 32 bits in the applicablePolicyTypes mask... 
      need update the structure to have an additional mask. */
      LOG_ERROR(0);
    }

    qsetPtr->applicablePolicyTypes = 0;
    for (i = 0; i < BROAD_POLICY_TYPE_LAST; i++)
    {
      if (applicablePolicyTypes[i] == L7_TRUE)
      {
        qsetPtr->applicablePolicyTypes |= (1 << i);
      }
    }

    BCM_FIELD_QSET_INIT(qset1);

    /* initialize the 1st component qset */
    for (i = 0; i < q1Size; i++)
        BCM_FIELD_QSET_ADD(qset1, q1[i]);

    /* initialize the custom qset */
    CUSTOM_FIELD_QSET_INIT(qsetPtr->customQset);
    for (i = 0; i < q2Size; i++)
        CUSTOM_FIELD_QSET_ADD(qsetPtr->customQset, q2[i]);

    BCM_FIELD_QSET_INIT(qsetPtr->qset1);
    BCM_FIELD_QSET_INIT(qsetPtr->qsetAgg);
    BCM_FIELD_QSET_INIT(qsetPtr->qsetUdf);

    _policy_set_union(qset1, &qsetPtr->qset1);
    _policy_set_union(qset1, &qsetPtr->qsetAgg);

    /* handle UDF fields in super qsets */
    for (i = 0; i < q2Size; i++)
    {
        if (BROAD_SYSTEM_UDF == q2[i])
        {
            rv = bcm_field_qset_data_qualifier_add(unit, &qsetPtr->qsetAgg, sys0_ethType[unit]);
            if (BCM_E_NONE != rv)
                return rv;

            rv = bcm_field_qset_data_qualifier_add(unit, &qsetPtr->qsetAgg, sys0_ipProto[unit]);
            if (BCM_E_NONE != rv)
                return rv;

            rv = bcm_field_qset_data_qualifier_add(unit, &qsetPtr->qsetAgg, sys0_l4DstPort[unit]);
            if (BCM_E_NONE != rv)
                return rv;

            rv = bcm_field_qset_data_qualifier_add(unit, &qsetPtr->qsetAgg, sys0_tunIpv6NextHdr[unit]);
            if (BCM_E_NONE != rv)
                return rv;

            /* update the UDF qset with the new fields */
            BCM_FIELD_QSET_ADD(qsetPtr->qsetUdf, bcmFieldQualifyEtherType);
            BCM_FIELD_QSET_ADD(qsetPtr->qsetUdf, bcmFieldQualifyIpProtocol);
            BCM_FIELD_QSET_ADD(qsetPtr->qsetUdf, bcmFieldQualifyL4SrcPort);
            BCM_FIELD_QSET_ADD(qsetPtr->qsetUdf, bcmFieldQualifyL4DstPort);
            BCM_FIELD_QSET_ADD(qsetPtr->qsetUdf, bcmFieldQualifyIp6NextHeader); /* Applied only in 6over4 case */

            qsetPtr->udfId = BROAD_SYSTEM_UDF;
        }
        else if (BROAD_ISCSI_UDF == q2[i])
        {
          rv = bcm_field_qset_data_qualifier_add(unit, &qsetPtr->qsetAgg, iscsi_dstIpAddr[unit]);
            if (BCM_E_NONE != rv)
                return rv;

          rv = bcm_field_qset_data_qualifier_add(unit, &qsetPtr->qsetAgg, iscsi_l4SrcDstPort[unit]);
            if (BCM_E_NONE != rv)
                return rv;

          rv = bcm_field_qset_data_qualifier_add(unit, &qsetPtr->qsetAgg, iscsi_opCode[unit]);
            if (BCM_E_NONE != rv)
                return rv;

            rv = bcm_field_qset_data_qualifier_add(unit, &qsetPtr->qsetAgg, iscsi_opCodeTcpOptions[unit]);
            if (BCM_E_NONE != rv)
                return rv;

            /* update the UDF qset with the new fields */
            BCM_FIELD_QSET_ADD(qsetPtr->qsetUdf, bcmFieldQualifyDstIp);
            BCM_FIELD_QSET_ADD(qsetPtr->qsetUdf, bcmFieldQualifyL4SrcPort);
            BCM_FIELD_QSET_ADD(qsetPtr->qsetUdf, bcmFieldQualifyL4DstPort);
            CUSTOM_FIELD_QSET_ADD(qsetPtr->customQset, customFieldQualifyIscsiOpcode);
            CUSTOM_FIELD_QSET_ADD(qsetPtr->customQset, customFieldQualifyIscsiOpcodeTcpOptions);

            qsetPtr->udfId = BROAD_ISCSI_UDF;
        }
    }

    /* Temporarily create a group using this qset, then check w/ the SDK to determine how many slices it needs. */
    do
    {
      rv = bcm_field_group_create_mode(unit, qsetPtr->qsetAgg, 0, bcmFieldGroupModeAuto, &gid);
      if (rv != BCM_E_NONE)
      {
        break;
      }

      rv = bcm_field_group_status_get(unit, gid, &qsetPtr->status);
      if (rv != BCM_E_NONE)
      {
        break;
      }

      _policy_group_status_to_sqset_width(&qsetPtr->status, &qsetPtr->sqsetWidth);

      (void)bcm_field_group_destroy(unit, gid);

    } while (0);

    if (rv != BCM_E_NONE)
    {
      /* If there were any errors, clean up the sqset table entry. */
      memset(qsetPtr, 0, sizeof(*qsetPtr));
    }

    return rv;
}

static int _policy_udf_init(int unit)
{
    int rv;

    policy_udf_id[unit] = 0;

    rv = _policy_super_qset_add_udf(unit, BROAD_SYSTEM_UDF);
    if (BCM_E_NONE == rv)
    {
      rv = _policy_super_qset_add_udf(unit, BROAD_ISCSI_UDF);
    }

    return rv;
}

static int _policy_super_qset_init_vfp(int unit)
{
  L7_BOOL applicable_policy_types[BROAD_POLICY_TYPE_LAST];

  if(policy_stage_supported(unit, BROAD_POLICY_STAGE_LOOKUP))
  {
    memset(applicable_policy_types, 0, sizeof(applicable_policy_types));
    applicable_policy_types[BROAD_POLICY_TYPE_LLPF] = L7_TRUE;
    applicable_policy_types[BROAD_POLICY_TYPE_IPSG] = L7_TRUE;
    _policy_super_qset_add(unit, &llpfQsetLookupDef, applicable_policy_types);

    memset(applicable_policy_types, 0, sizeof(applicable_policy_types));
    applicable_policy_types[BROAD_POLICY_TYPE_PORT] = L7_TRUE;
    applicable_policy_types[BROAD_POLICY_TYPE_IPSG] = L7_TRUE;

    /* The following qsets use intra-slice doublewide mode, so the number of rules is cut in half. */
    _policy_super_qset_add(unit, &l2l3l4QsetLookupDef, applicable_policy_types);

    _policy_super_qset_add(unit, &ipv6L3L4QsetLookupDef, applicable_policy_types);

    memset(applicable_policy_types, 0, sizeof(applicable_policy_types));
    applicable_policy_types[BROAD_POLICY_TYPE_DOT1AD] = L7_TRUE;

    _policy_super_qset_add(unit, &dot1adQsetLookupDef, applicable_policy_types);
  }

  return BCM_E_NONE;
}

static int _policy_super_qset_init_ifp(int unit)
{
  L7_BOOL applicable_policy_types[BROAD_POLICY_TYPE_LAST];

  if (_policy_supports_wide_mode(unit))
  {
    memset(applicable_policy_types, 0, sizeof(applicable_policy_types));
    applicable_policy_types[BROAD_POLICY_TYPE_PORT] = L7_TRUE;
    applicable_policy_types[BROAD_POLICY_TYPE_VLAN] = L7_TRUE;

    if (policy_stage_supported(unit, BROAD_POLICY_STAGE_LOOKUP))
    {
      // PTin added: new switch => SOC_IS_VALKYRIE2
      if ( SOC_IS_TRIUMPH2(unit) ||
           SOC_IS_TRIUMPH(unit)  ||
           SOC_IS_APOLLO(unit)   ||
           SOC_IS_ENDURO(unit)   ||
           SOC_IS_SCORPION(unit) ||
           SOC_IS_VALKYRIE2(unit)||
           SOC_IS_TRIDENT(unit)     /* PTin added: new switch BCM56843 */
         )
      {
        _policy_super_qset_add(unit, &l2l3l4Xgs4ClassIdQsetDef, applicable_policy_types);
      }
      else
      {
        _policy_super_qset_add(unit, &l2l3l4ClassIdQsetDef, applicable_policy_types);
      }
      _policy_super_qset_add(unit, &ipv6L3L4ClassIdQsetDef,  applicable_policy_types);
      _policy_super_qset_add(unit, &ipv6SrcL4ClassIdQsetDef, applicable_policy_types);
      _policy_super_qset_add(unit, &ipv6DstL4ClassIdQsetDef, applicable_policy_types);
    }
    else
    {
      if (soc_feature(unit, soc_feature_src_mac_group))
      {
        _policy_super_qset_add(unit, &l2l3l4SrcMacGroupQsetDef, applicable_policy_types);
      }
      else
      {
        _policy_super_qset_add(unit, &l2l3l4QsetDef, applicable_policy_types);
      }
      _policy_super_qset_add(unit, &ipv6L3L4QsetDef,  applicable_policy_types);
      _policy_super_qset_add(unit, &ipv6SrcL4QsetDef, applicable_policy_types);
      _policy_super_qset_add(unit, &ipv6DstL4QsetDef, applicable_policy_types);
    }
  }

  if (SOC_IS_HAWKEYE(unit))
  {
    memset(applicable_policy_types, 0, sizeof(applicable_policy_types));
    applicable_policy_types[BROAD_POLICY_TYPE_PORT]        = L7_TRUE;
    applicable_policy_types[BROAD_POLICY_TYPE_VLAN]        = L7_TRUE;
    applicable_policy_types[BROAD_POLICY_TYPE_SYSTEM]      = L7_TRUE;
    applicable_policy_types[BROAD_POLICY_TYPE_PTIN]        = L7_TRUE;   /* PTin added: policer */
    applicable_policy_types[BROAD_POLICY_TYPE_STAT_EVC]    = L7_TRUE;   /* PTin added: stats */
    applicable_policy_types[BROAD_POLICY_TYPE_STAT_CLIENT] = L7_TRUE;   /* PTin added: stats */
    applicable_policy_types[BROAD_POLICY_TYPE_SYSTEM_PORT] = L7_TRUE;
    applicable_policy_types[BROAD_POLICY_TYPE_COSQ]        = L7_TRUE;

    _policy_super_qset_add(unit, &l2SvtLookupStatusQsetDef, applicable_policy_types);
  }

  memset(applicable_policy_types, 0, sizeof(applicable_policy_types));
  applicable_policy_types[BROAD_POLICY_TYPE_SYSTEM]      = L7_TRUE;
  applicable_policy_types[BROAD_POLICY_TYPE_SYSTEM_PORT] = L7_TRUE;
  applicable_policy_types[BROAD_POLICY_TYPE_COSQ]        = L7_TRUE;
  applicable_policy_types[BROAD_POLICY_TYPE_PTIN]        = L7_TRUE;   /* PTin added: policer */
  applicable_policy_types[BROAD_POLICY_TYPE_STAT_EVC]    = L7_TRUE;   /* PTin added: stats */
  applicable_policy_types[BROAD_POLICY_TYPE_STAT_CLIENT] = L7_TRUE;   /* PTin added: stats */

  // PTin added: new switch => SOC_IS_VALKYRIE2 + SOC_IS_TRIDENT
  if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || SOC_IS_ENDURO(unit) || SOC_IS_VALKYRIE2(unit) || SOC_IS_TRIDENT(unit))
  {
    if (SOC_IS_TRIDENT(unit))
      LOG_WARNING(LOG_CTX_MISC, "Using systemQsetTriumph2Def for TRIDENT family!");

    /* Doublewide mode. */
    _policy_super_qset_add(unit, &systemQsetTriumph2Def, applicable_policy_types);
  }
  else if (SOC_IS_SCORPION(unit))
  {
    _policy_super_qset_add(unit, &systemQsetDef, applicable_policy_types);
    _policy_super_qset_add(unit, &ipv6NdQsetScorpionDef, applicable_policy_types);
  }
  else if (_policy_supports_wide_mode(unit))
  {
    /* Doublewide mode. */
    _policy_super_qset_add(unit, &systemQsetDoubleDef, applicable_policy_types);
  }
  else
  {
    _policy_super_qset_add(unit, &systemQsetDef, applicable_policy_types);
    _policy_super_qset_add(unit, &ipv6NdQsetDef, applicable_policy_types);
  }

  /* The following sqset is used for user policies and in some cases, 
     system policies. */
  memset(applicable_policy_types, 0, sizeof(applicable_policy_types));
  applicable_policy_types[BROAD_POLICY_TYPE_PORT]        = L7_TRUE;
  applicable_policy_types[BROAD_POLICY_TYPE_VLAN]        = L7_TRUE;
  if (!_policy_supports_wide_mode(unit) || SOC_IS_SCORPION(unit))
  {
    applicable_policy_types[BROAD_POLICY_TYPE_SYSTEM]      = L7_TRUE;
    applicable_policy_types[BROAD_POLICY_TYPE_PTIN]        = L7_TRUE;   /* PTin added: policer */
    applicable_policy_types[BROAD_POLICY_TYPE_STAT_EVC]    = L7_TRUE;   /* PTin added: stats */
    applicable_policy_types[BROAD_POLICY_TYPE_STAT_CLIENT] = L7_TRUE;   /* PTin added: stats */
    applicable_policy_types[BROAD_POLICY_TYPE_SYSTEM_PORT] = L7_TRUE;
    applicable_policy_types[BROAD_POLICY_TYPE_COSQ]        = L7_TRUE;
  }
  _policy_super_qset_add(unit, &l2SvtQsetDef, applicable_policy_types);

  /* The following sqset is used for iSCSI control rules. */
  memset(applicable_policy_types, 0, sizeof(applicable_policy_types));
  applicable_policy_types[BROAD_POLICY_TYPE_ISCSI] = L7_TRUE;

  _policy_super_qset_add(unit, &iscsiQsetDef, applicable_policy_types);

  /* The following sqset is used for iSCSI sessions but may also be used for user
     policies on devices that do not support doublewide mode. */
  memset(applicable_policy_types, 0, sizeof(applicable_policy_types));
  applicable_policy_types[BROAD_POLICY_TYPE_ISCSI] = L7_TRUE;
  applicable_policy_types[BROAD_POLICY_TYPE_PORT]  = L7_TRUE;
  _policy_super_qset_add(unit, &l3l4QsetDef, applicable_policy_types);

  /* The following sqsets are used only for user policies. */
  memset(applicable_policy_types, 0, sizeof(applicable_policy_types));
  applicable_policy_types[BROAD_POLICY_TYPE_VLAN]        = L7_TRUE;
  _policy_super_qset_add(unit, &vlanl3QsetDef,      applicable_policy_types);

  /* The following sqset is used for IPSG policies but may also be used for user
     policies on devices that do not support doublewide mode. */
  memset(applicable_policy_types, 0, sizeof(applicable_policy_types));
#if L7_FEAT_IPSG_ON_IFP
  applicable_policy_types[BROAD_POLICY_TYPE_IPSG] = L7_TRUE;
#endif
  applicable_policy_types[BROAD_POLICY_TYPE_PORT] = L7_TRUE;
  applicable_policy_types[BROAD_POLICY_TYPE_VLAN] = L7_TRUE;
  _policy_super_qset_add(unit, &l2l3SrcQsetDef, applicable_policy_types);

  /* The following sqsets are used only for user policies. */
  memset(applicable_policy_types, 0, sizeof(applicable_policy_types));
  applicable_policy_types[BROAD_POLICY_TYPE_PORT]        = L7_TRUE;
  applicable_policy_types[BROAD_POLICY_TYPE_VLAN]        = L7_TRUE;

  _policy_super_qset_add(unit, &l2l3DstQsetDef,     applicable_policy_types);

  return BCM_E_NONE;
}

static int _policy_super_qset_init_efp(int unit)
{
  L7_BOOL applicable_policy_types[BROAD_POLICY_TYPE_LAST];

  if(policy_stage_supported(unit, BROAD_POLICY_STAGE_EGRESS))
  {
    memset(applicable_policy_types, 0, sizeof(applicable_policy_types));
    applicable_policy_types[BROAD_POLICY_TYPE_SYSTEM]      = L7_TRUE;
    applicable_policy_types[BROAD_POLICY_TYPE_PORT]        = L7_TRUE;
    applicable_policy_types[BROAD_POLICY_TYPE_VLAN]        = L7_TRUE;
    applicable_policy_types[BROAD_POLICY_TYPE_STAT_EVC]    = L7_TRUE;   /* PTin added: stats */
    applicable_policy_types[BROAD_POLICY_TYPE_STAT_CLIENT] = L7_TRUE;   /* PTin added: stats */

    //printf("%s(%d)\r\n",__FUNCTION__,__LINE__);
    _policy_super_qset_add(unit, &l2QsetEgressDef, applicable_policy_types);

    //printf("%s(%d)\r\n",__FUNCTION__,__LINE__);
    _policy_super_qset_add(unit, &l3l4QsetEgressDef, applicable_policy_types);

    //printf("%s(%d)\r\n",__FUNCTION__,__LINE__);
    _policy_super_qset_add(unit, &ipv6L3L4QsetEgressDef, applicable_policy_types);
  }

  return BCM_E_NONE;
}

static int _policy_super_qset_init(int unit)
{
  int  i, j;

  for (i = 0; i < SUPER_QSET_TABLE_SIZE; i++)
  {
    super_qset_table[unit][i].flags = SUPER_QSET_NONE;
  }

  _policy_super_qset_init_vfp(unit);
  _policy_super_qset_init_ifp(unit);
  _policy_super_qset_init_efp(unit);

  for (i = 0; i < SUPER_QSET_TABLE_SIZE; i++)
  {
    printf("super_qset_table %-2u (width=%u, flags=0x%08x): ",i,super_qset_table[unit][i].sqsetWidth,super_qset_table[unit][i].flags);
    for (j = 0; j < _SHR_BITDCLSIZE(BCM_FIELD_QUALIFY_MAX); j++)
    {
      printf("%08X ",super_qset_table[unit][i].qsetAgg.w[j]);
    }
    printf("\r\n");
  }

  return BCM_E_NONE;
}

static int _policy_action_map_init(int unit)
{
  // PTin added: new switch => SOC_IS_VALKYRIE2
  if ((SOC_IS_TR_VL(unit)) ||
      (SOC_IS_SCORPION(unit)) || 
      (SOC_IS_TRIUMPH2(unit)) || 
      (SOC_IS_APOLLO(unit)) ||
      (SOC_IS_ENDURO(unit)) ||
      (SOC_IS_VALKYRIE2(unit)) ||
      (SOC_IS_TRIDENT(unit)))   /* PTin added: new switch BCM56843 */
  {
    /* Modify action maps for certain actions. */
    memcpy(&ingress_action_map[BROAD_ACTION_SET_COSQ],     &xgs4_ingress_set_cosq_action_map,     sizeof(action_map_entry_t));
    memcpy(&ingress_action_map[BROAD_ACTION_SET_USERPRIO], &xgs4_ingress_set_userprio_action_map, sizeof(action_map_entry_t));

    memcpy(&ingress_action_map[BROAD_ACTION_SET_USERPRIO_AS_COS2], 
           &xgs4_ingress_set_userprio_as_cos2_action_map,
           sizeof(action_map_entry_t));

    memcpy(&lookup_action_map[BROAD_ACTION_COPY_TO_CPU], &xgs4_lookup_copy_to_cpu_action_map, sizeof(action_map_entry_t));
  }

  return BCM_E_NONE;
}

/* Group Allocation Functions */

void _policy_group_alloc_type(BROAD_POLICY_TYPE_t type, group_alloc_block_t *block, group_alloc_dir_t *dir)
{
    /* System and CoS Queue policies grow from low to high.
     * VLAN policies grow from low to high.
     * Port/Trunk policies grow from high to low.
     */
    switch (type)
    {
    case BROAD_POLICY_TYPE_VLAN:
    case BROAD_POLICY_TYPE_ISCSI:
    case BROAD_POLICY_TYPE_IPSG:
    case BROAD_POLICY_TYPE_LLPF:
        *block = ALLOC_BLOCK_MEDIUM;
        *dir   = ALLOC_LOW_TO_HIGH;
        break;
    case BROAD_POLICY_TYPE_PORT:
    case BROAD_POLICY_TYPE_DOT1AD:
        *block = ALLOC_BLOCK_HIGH;
        *dir   = ALLOC_HIGH_TO_LOW;
        break;
    case BROAD_POLICY_TYPE_SYSTEM_PORT:
    case BROAD_POLICY_TYPE_SYSTEM:
        *block = ALLOC_BLOCK_LOW;
        *dir   = ALLOC_HIGH_TO_LOW;
        break;
    /* PTin added: policer and stats */
    case BROAD_POLICY_TYPE_PTIN:
      *block = ALLOC_BLOCK_PTIN;
      *dir   = ALLOC_LOW_TO_HIGH;
      break;
    case BROAD_POLICY_TYPE_STAT_EVC:
        *block = ALLOC_BLOCK_STATS_EVC;
        *dir   = ALLOC_HIGH_TO_LOW;
        break;
    case BROAD_POLICY_TYPE_STAT_CLIENT:
        *block = ALLOC_BLOCK_STATS_CLIENT;
        *dir   = ALLOC_HIGH_TO_LOW;
        break;
    /* PTin end */
    case BROAD_POLICY_TYPE_COSQ:
        *block = ALLOC_BLOCK_LOW;
        *dir   = ALLOC_LOW_TO_HIGH;
        break;
    default:
        LOG_ERROR(type);
        break;
    }
}

L7_BOOL _policy_group_types_compatible(int unit, BROAD_POLICY_TYPE_t group1_type, BROAD_POLICY_TYPE_t group2_type)
{
  L7_BOOL groupTypesCompatible = L7_TRUE;

  if (group1_type != group2_type)
  {
    /* There is a special case here... SYSTEM policies, SYSTEM_PORT, and COSQ
       policies may share a group. The exception is for Helix, where we must
       ensure that COSQ policies are kept separate from SYSTEM and SYSTEM_PORT
       policies. The other exception is for Scorpion packages that do not
       include the IPv6 package (This is to ensure the COS policies go
       into a lower priority slice than the L3 policies (which use a UDF
       and cannot accommodate the COS rules)).
       Additionally, LLPF policies may share a group w/ IPSG policies, as we
       are guaranteed that a packet would never hit both an LLPF and IPSG rule. */
    if (group1_type == BROAD_POLICY_TYPE_SYSTEM_PORT)
    {
      if ((group2_type != BROAD_POLICY_TYPE_SYSTEM) &&
          (group2_type != BROAD_POLICY_TYPE_PTIN) &&        /* PTin added: policer */
          (group2_type != BROAD_POLICY_TYPE_STAT_EVC) &&    /* PTin added: stats */
          (group2_type != BROAD_POLICY_TYPE_STAT_CLIENT) && /* PTin added: stats */
          (group2_type != BROAD_POLICY_TYPE_COSQ))
      {
        groupTypesCompatible = L7_FALSE;
      }
      else if ((group2_type == BROAD_POLICY_TYPE_COSQ) && SOC_IS_HELIX1(unit))
      {
        groupTypesCompatible = L7_FALSE;
      }
#ifndef L7_IPV6_PACKAGE
      else if ((group2_type == BROAD_POLICY_TYPE_COSQ) && SOC_IS_SCORPION(unit))
      {
        groupTypesCompatible = L7_FALSE;
      }
#endif
    }
    else if (group1_type == BROAD_POLICY_TYPE_SYSTEM ||
             group1_type == BROAD_POLICY_TYPE_PTIN ||       /* PTin modified: policer */
             group1_type == BROAD_POLICY_TYPE_STAT_EVC ||   /* PTin modified: stats */
             group1_type == BROAD_POLICY_TYPE_STAT_CLIENT ) /* PTin modified: stats */
    {
      if ((group2_type != BROAD_POLICY_TYPE_SYSTEM_PORT) &&
          (group2_type != BROAD_POLICY_TYPE_COSQ) &&
          (group2_type != BROAD_POLICY_TYPE_STAT_EVC) &&  /* PTin modified: stats */
          (group2_type != BROAD_POLICY_TYPE_STAT_CLIENT)) /* PTin modified: stats */
      {
        groupTypesCompatible = L7_FALSE;
      }
      else if ((group2_type == BROAD_POLICY_TYPE_COSQ) && SOC_IS_HELIX1(unit))
      {
        groupTypesCompatible = L7_FALSE;
      }
#ifndef L7_IPV6_PACKAGE
      else if ((group2_type == BROAD_POLICY_TYPE_COSQ) && SOC_IS_SCORPION(unit))
      {
        groupTypesCompatible = L7_FALSE;
      }
#endif
    }
    else if (group1_type == BROAD_POLICY_TYPE_COSQ)
    {
      if (((group2_type != BROAD_POLICY_TYPE_SYSTEM) &&
           (group2_type != BROAD_POLICY_TYPE_PTIN) &&         /* PTin added: policer */
           (group2_type != BROAD_POLICY_TYPE_STAT_EVC) &&     /* PTin added: stats */
           (group2_type != BROAD_POLICY_TYPE_STAT_CLIENT) &&  /* PTin added: stats */
           (group2_type != BROAD_POLICY_TYPE_SYSTEM_PORT)) ||
          SOC_IS_HELIX1(unit))
      {
        groupTypesCompatible = L7_FALSE;
      }
#ifndef L7_IPV6_PACKAGE
      else if (SOC_IS_SCORPION(unit))
      {
        groupTypesCompatible = L7_FALSE;
      }
#endif
    }
    else if (group1_type == BROAD_POLICY_TYPE_IPSG)
    {
      if (group2_type != BROAD_POLICY_TYPE_LLPF)
      {
        groupTypesCompatible = L7_FALSE;
      }
    }
    else if (group1_type == BROAD_POLICY_TYPE_LLPF)
    {
      if (group2_type != BROAD_POLICY_TYPE_IPSG)
      {
        groupTypesCompatible = L7_FALSE;
      }
    }
    else
    {
      groupTypesCompatible = L7_FALSE;
    }
  }

  return groupTypesCompatible;
}

static int _policy_group_find_first(int                  unit, 
                                    BROAD_POLICY_STAGE_t policyStage,
                                    BROAD_POLICY_TYPE_t  type, 
                                    BROAD_GROUP_t       *group)
{
  group_alloc_block_t block, used_block;
  group_alloc_dir_t   dir,   used_dir;
  group_table_t      *groupPtr;
  super_qset_entry_t  sqsetInfo;

  _policy_group_alloc_type(type, &block, &dir);

  if (ALLOC_HIGH_TO_LOW == dir)
  {
    *group = group_alloc_table[unit][policyStage][block].highPrio;
  }
  else
  {
    *group = group_alloc_table[unit][policyStage][block].lowPrio;
  }

  groupPtr = &group_table[unit][policyStage][*group];
  if (groupPtr->flags & GROUP_USED)
  {
    /* Set group to the first slice of the group. */
    _policy_sqset_get(unit, groupPtr->sqset, &sqsetInfo);
    *group -= *group % sqsetInfo.status.slice_width_physical;
    groupPtr = &group_table[unit][policyStage][*group];

    /* make sure that the block that this group belongs to matches
       the block requested */
    if (_policy_group_types_compatible(unit, type, groupPtr->type) == L7_FALSE)
    {
      return BCM_E_FAIL;
    }
    _policy_group_alloc_type(groupPtr->type, &used_block, &used_dir);
    if (block != used_block)
    {
      return BCM_E_FAIL;
    }
  }

  return BCM_E_NONE;
}

static int _policy_group_decrement(int                   unit,
                                   BROAD_POLICY_STAGE_t  policyStage,
                                   BROAD_GROUP_t        *group, 
                                   group_alloc_block_t   block, 
                                   BROAD_POLICY_TYPE_t   type)
{
  group_alloc_block_t used_block;
  group_alloc_dir_t   used_dir;
  group_table_t      *groupPtr;
  super_qset_entry_t  sqsetInfo;

  /* If the group is within range of this block and it is in use
     and it is multi-slice, set the group to the first slice. */
  if ((*group >= group_alloc_table[unit][policyStage][block].lowPrio) &&
      (*group <= group_alloc_table[unit][policyStage][block].highPrio))
  {
    groupPtr = &group_table[unit][policyStage][*group];
    if (groupPtr->flags & GROUP_USED)
    {
      /* Set group to the first slice of the group. */
      _policy_sqset_get(unit, groupPtr->sqset, &sqsetInfo);
      *group -= *group % sqsetInfo.status.slice_width_physical;
    }
  }

  if ((*group - 1) >= group_alloc_table[unit][policyStage][block].lowPrio)
  {
    *group = *group - 1;

    groupPtr = &group_table[unit][policyStage][*group];
    if (groupPtr->flags & GROUP_USED)
    {
      /* Set group to the first slice of the group. */
      _policy_sqset_get(unit, groupPtr->sqset, &sqsetInfo);
      *group -= *group % sqsetInfo.status.slice_width_physical;
      groupPtr = &group_table[unit][policyStage][*group];

      /* make sure that the block that this group belongs to matches
         the block requested */
      if (_policy_group_types_compatible(unit, type, groupPtr->type) == L7_FALSE)
      {
        return BCM_E_FAIL;
      }
      _policy_group_alloc_type(groupPtr->type, &used_block, &used_dir);
      if (block != used_block)
      {
        return BCM_E_FAIL;
      }
    }

    return BCM_E_NONE;
  }

  return BCM_E_FAIL;
}

static int _policy_group_increment(int                   unit, 
                                   BROAD_POLICY_STAGE_t  policyStage,
                                   BROAD_GROUP_t        *group, 
                                   group_alloc_block_t   block, 
                                   BROAD_POLICY_TYPE_t   type)
{
  group_alloc_block_t used_block;
  group_alloc_dir_t   used_dir;
  group_table_t      *groupPtr;
  super_qset_entry_t  sqsetInfo;

  /* If the group is within range of this block and it is in use
     and it is multi-slice, set the group to the last slice. */
  if ((*group >= group_alloc_table[unit][policyStage][block].lowPrio) &&
      (*group <= group_alloc_table[unit][policyStage][block].highPrio))
  {
    groupPtr = &group_table[unit][policyStage][*group];
    if (groupPtr->flags & GROUP_USED)
    {
      /* Set group to the last slice of the group. */
      _policy_sqset_get(unit, groupPtr->sqset, &sqsetInfo);
      *group += sqsetInfo.status.slice_width_physical - 1;
    }
  }

  while ((*group + 1) <= group_alloc_table[unit][policyStage][block].highPrio)
  {
    *group = *group + 1;

    groupPtr = &group_table[unit][policyStage][*group];
    if (groupPtr->flags & GROUP_USED)
    {
      /* make sure that the block that this group belongs to matches
         the block requested */
      if (_policy_group_types_compatible(unit, type, groupPtr->type) == L7_FALSE)
      {
        return BCM_E_FAIL;
      }
      _policy_group_alloc_type(groupPtr->type, &used_block, &used_dir);
      if (block != used_block)
      {
        return BCM_E_FAIL;
      }
    }

    return BCM_E_NONE;
  }

  return BCM_E_FAIL;
}

static int _policy_group_find_next(int                  unit, 
                                   BROAD_POLICY_STAGE_t policyStage, 
                                   BROAD_POLICY_TYPE_t  type, 
                                   BROAD_GROUP_t       *group)
{
  group_alloc_block_t block;
  group_alloc_dir_t   dir;
  int retval = BCM_E_FAIL;

  _policy_group_alloc_type(type, &block, &dir);

  /* Do not allow the high and low groups to co-mingle. */
  if (ALLOC_HIGH_TO_LOW == dir)
  {
    retval = _policy_group_decrement(unit, policyStage, group, block, type);
  }
  else
  {
    retval = _policy_group_increment(unit, policyStage, group, block, type);
  }

  return retval;
}

static int _policy_field_to_bcm_field(BROAD_POLICY_FIELD_t  field,
                                      BROAD_POLICY_STAGE_t  policyStage,
                                      char                 *value,
                                      char                 *mask,
                                      bcm_field_qualify_t  *bcm_field,
                                      int                   req_ethtype)
{
    L7_ushort16 temp16;

    /* ETHTYPE is a special case so check if it is explicitly required;
     * otherwise, we can just use IpType.
     */
    if ((0 == req_ethtype) && (BROAD_FIELD_ETHTYPE == field))
    {
       if (policyStage == BROAD_POLICY_STAGE_EGRESS)
       {
         if (*((uint16*)value) == 0x86dd)
         {
            *bcm_field = bcmFieldQualifyIp6;
         }
         else if (*((uint16*)value) == 0x0800)
         {
            *bcm_field = bcmFieldQualifyIp4;
         }
        }
        else
        {
          *bcm_field = bcmFieldQualifyIpType; 
        }
    }
    else if ((field == BROAD_FIELD_IVID) && (policyStage == BROAD_POLICY_STAGE_EGRESS))
    {
      /* The EFP supports innerVlanId, but not innerVlan. */
      memcpy(&temp16, mask, sizeof(temp16));
      if ((temp16 & 0xF000) == 0)
      {
        *bcm_field = bcmFieldQualifyInnerVlanId;
      }
      else
      {
        return BCM_E_PARAM;
      }
    }
    else if (field < BROAD_FIELD_LAST)
    {
        /* map user-specified field to bcm field */
         *bcm_field = field_map[field];
              
    }
    else
    {
      return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

int _policy_group_calc_qset(int                             unit,
                            BROAD_POLICY_ENTRY_t           *entryPtr, 
                            policy_resource_requirements_t *resourceReq)
{
    int    f;
    int    rv;
    BROAD_POLICY_RULE_ENTRY_t *rulePtr;

    resourceReq->ruleCount    = entryPtr->ruleCount;
    resourceReq->counterCount = 0;
    resourceReq->meterCount   = 0;

    /* Determine if ETHTYPE is required in this QSET, or if IpType can be used instead. */
    BCM_FIELD_QSET_INIT(resourceReq->qsetAgg);
    CUSTOM_FIELD_QSET_INIT(resourceReq->customQset);
    resourceReq->requiresEtype = 0;

    rulePtr = entryPtr->ruleInfo;
    while (rulePtr != L7_NULL)
    {
      if (hapiBroadPolicyFieldFlagsGet(&(rulePtr->fieldInfo), BROAD_FIELD_ETHTYPE) == BROAD_FIELD_SPECIFIED)
      {
        uint16 ethtype = *((uint16*)hapiBroadPolicyFieldValuePtr(&(rulePtr->fieldInfo), BROAD_FIELD_ETHTYPE));
        if ((0x0800 != ethtype) && (0x86DD != ethtype))
        {
          resourceReq->requiresEtype = 1;
        }
      }

      /* Determine how many counters/meters are required for the policy. */
      if (rulePtr->ruleFlags & BROAD_METER_SPECIFIED)
      {
        /* meters use a counter as well */
        (resourceReq->counterCount)++;
        (resourceReq->meterCount)++;
      }
      else if (rulePtr->ruleFlags & BROAD_COUNTER_SPECIFIED)
      {
        (resourceReq->counterCount)++;
      }

      rulePtr = rulePtr->next;
    }

    if (entryPtr->strictEthTypes)
    {
      resourceReq->requiresEtype = 1;
    }

    /* Create a qset containing each field specified in each rule. */
    rulePtr = entryPtr->ruleInfo;
    while (rulePtr != L7_NULL)
    {
      for (f = 0; f < BROAD_FIELD_LAST; f++)
      {
          if (hapiBroadPolicyFieldFlagsGet(&(rulePtr->fieldInfo), f) == BROAD_FIELD_SPECIFIED)
          {
              char                *value;
              char                *mask;
              bcm_field_qualify_t  bcm_field = 0;
              L7_ushort16          temp16;

              value = (char*)hapiBroadPolicyFieldValuePtr(&(rulePtr->fieldInfo), f);
              mask  = (char*)hapiBroadPolicyFieldMaskPtr(&(rulePtr->fieldInfo), f);

              switch (f)
              {
              /* custom fields go here */
              case BROAD_FIELD_ICMP_MSG_TYPE:
                /* Use L4 src port for ICMP Msg Type. We can do this because
                   XGS3 just treats the L4 src port field as the first two
                   bytes following the IP header. */
                BCM_FIELD_QSET_ADD(resourceReq->qsetAgg,bcmFieldQualifyL4SrcPort);
                break;

              case BROAD_FIELD_ISCSI_OPCODE:
                CUSTOM_FIELD_QSET_ADD(resourceReq->customQset, customFieldQualifyIscsiOpcode);
                break;

              case BROAD_FIELD_ISCSI_OPCODE_TCP_OPTIONS:
                CUSTOM_FIELD_QSET_ADD(resourceReq->customQset, customFieldQualifyIscsiOpcodeTcpOptions);
                break;

              case BROAD_FIELD_LOOKUP_STATUS:
                memcpy(&temp16, mask, sizeof(L7_ushort16));
                if (temp16 & BROAD_LOOKUPSTATUS_DOS_ATTACK_PKT)
                  BCM_FIELD_QSET_ADD(resourceReq->qsetAgg, bcmFieldQualifyDosAttack);

                if (temp16 & BROAD_LOOKUPSTATUS_UNRESOLVED_SA)
                  BCM_FIELD_QSET_ADD(resourceReq->qsetAgg, bcmFieldQualifyL2StationMove);

                if (temp16 & BROAD_LOOKUPSTATUS_LPM_HIT)
                  BCM_FIELD_QSET_ADD(resourceReq->qsetAgg, bcmFieldQualifyL3DestRouteHit);

                if (temp16 & BROAD_LOOKUPSTATUS_STARGV_HIT)
                  BCM_FIELD_QSET_ADD(resourceReq->qsetAgg, bcmFieldQualifyIpmcStarGroupHit);

                if (temp16 & BROAD_LOOKUPSTATUS_L3_DST_HIT)
                  BCM_FIELD_QSET_ADD(resourceReq->qsetAgg, bcmFieldQualifyL3DestHostHit);

                if (temp16 & BROAD_LOOKUPSTATUS_L3_UC_SRC_HIT)
                  BCM_FIELD_QSET_ADD(resourceReq->qsetAgg, bcmFieldQualifyL3SrcHostHit);

                if (temp16 & BROAD_LOOKUPSTATUS_L2_USER_ENTRY_HIT)
                  BCM_FIELD_QSET_ADD(resourceReq->qsetAgg, bcmFieldQualifyL2CacheHit);

                if (temp16 & BROAD_LOOKUPSTATUS_L2_TABLE_DST_L3)
                  BCM_FIELD_QSET_ADD(resourceReq->qsetAgg, bcmFieldQualifyL3Routable);

                if (temp16 & BROAD_LOOKUPSTATUS_L2_DST_HIT)
                  BCM_FIELD_QSET_ADD(resourceReq->qsetAgg, bcmFieldQualifyL2DestHit);

                if (temp16 & BROAD_LOOKUPSTATUS_L2_SRC_STATIC)
                  BCM_FIELD_QSET_ADD(resourceReq->qsetAgg, bcmFieldQualifyL2SrcStatic);

                if (temp16 & BROAD_LOOKUPSTATUS_L2_SRC_HIT)
                  BCM_FIELD_QSET_ADD(resourceReq->qsetAgg, bcmFieldQualifyL2SrcHit);

                if (temp16 & BROAD_LOOKUPSTATUS_INGRESS_SPG_STATE_MASK)
                  BCM_FIELD_QSET_ADD(resourceReq->qsetAgg, bcmFieldQualifyIngressStpState);

                if (temp16 & BROAD_LOOKUPSTATUS_FB_VLAN_ID_VALID)
                  BCM_FIELD_QSET_ADD(resourceReq->qsetAgg, bcmFieldQualifyForwardingVlanValid); /* ?? */

                if (temp16 & BROAD_LOOKUPSTATUS_VXLT_HIT)
                  BCM_FIELD_QSET_ADD(resourceReq->qsetAgg, bcmFieldQualifyVlanTranslationHit);

                if (temp16 & BROAD_LOOKUPSTATUS_TUNNEL_HIT)
                {
                  // PTin added: new switch => SOC_IS_VALKYRIE2
                  if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || SOC_IS_ENDURO(unit) || SOC_IS_VALKYRIE2(unit))
                  {
                    /* TunnelTerminated not supported in IFP, so check for TunnelType instead. */
                    BCM_FIELD_QSET_ADD(resourceReq->qsetAgg, bcmFieldQualifyTunnelType);
                  }
                  else
                  {
                    BCM_FIELD_QSET_ADD(resourceReq->qsetAgg, bcmFieldQualifyTunnelTerminated);
                  }
                }

                break;

              default:
                rv = _policy_field_to_bcm_field(f, entryPtr->policyStage, (char*)value, (char*)mask, &bcm_field, resourceReq->requiresEtype);
                if (BCM_E_NONE != rv)
                {
                  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
                      sysapiPrintf("- _policy_field_to_bcm_field failed rv %d; field f %d; policyStage %d\n", rv, f, entryPtr->policyStage);

                  return rv;
                }

                BCM_FIELD_QSET_ADD(resourceReq->qsetAgg,bcm_field);
                break;
              }
          }
      }

      rulePtr = rulePtr->next;
    }

    switch (entryPtr->policyStage)
    {
    case BROAD_POLICY_STAGE_LOOKUP:
      BCM_FIELD_QSET_ADD(resourceReq->qsetAgg,bcmFieldQualifyStageLookup);
      break;
    case BROAD_POLICY_STAGE_INGRESS:
      BCM_FIELD_QSET_ADD(resourceReq->qsetAgg,bcmFieldQualifyStageIngress);
      break;
    case BROAD_POLICY_STAGE_EGRESS:
      BCM_FIELD_QSET_ADD(resourceReq->qsetAgg,bcmFieldQualifyStageEgress);
      break;
    default:
      return BCM_E_CONFIG;
    }

    switch (entryPtr->policyType)
    {
    case BROAD_POLICY_TYPE_PORT:
    case BROAD_POLICY_TYPE_DOT1AD:
    case BROAD_POLICY_TYPE_LLPF:
    case BROAD_POLICY_TYPE_IPSG:
    case BROAD_POLICY_TYPE_SYSTEM_PORT:
    case BROAD_POLICY_TYPE_COSQ:
      if (entryPtr->policyStage == BROAD_POLICY_STAGE_LOOKUP)
      {
        BCM_FIELD_QSET_ADD(resourceReq->qsetAgg, bcmFieldQualifyInPort);
        BCM_FIELD_QSET_ADD(resourceReq->qsetAgg, bcmFieldQualifyPortClass);
      }
      else if (entryPtr->policyStage == BROAD_POLICY_STAGE_INGRESS)
      {
        BCM_FIELD_QSET_ADD(resourceReq->qsetAgg, bcmFieldQualifyInPorts);
      }
      else if (entryPtr->policyStage == BROAD_POLICY_STAGE_EGRESS)
      {
        BCM_FIELD_QSET_ADD(resourceReq->qsetAgg, bcmFieldQualifyOutPort);
        BCM_FIELD_QSET_ADD(resourceReq->qsetAgg, bcmFieldQualifyPortClass);
      }
      break;

    default:
      break;
    }

    return BCM_E_NONE;
}

static int _policy_group_resource_check(int unit, BROAD_POLICY_TYPE_t policyType, policy_resource_requirements_t *resourceReq, int gid)
{
  int                        rv;
  bcm_field_group_status_t   stat;
  int                        sqset;
  super_qset_entry_t         sqsetInfo;
  sqsetWidth_t               groupSqsetWidth;

  rv = _policy_minimal_sqset_get(unit, policyType, resourceReq, &sqset);
  if (BCM_E_NONE != rv)
    return rv;

  _policy_sqset_get(unit, sqset, &sqsetInfo);

  rv = bcm_field_group_status_get(unit, gid, &stat);
  if (BCM_E_NONE != rv)
    return rv;

  _policy_group_status_to_sqset_width(&stat, &groupSqsetWidth);

  /* If this policy would require expansion of a group that is wider than the 
     policy needs, don't allow this to occur. The policy can be created w/ the
     minimal sqset later on. */
  if (sqsetInfo.sqsetWidth < groupSqsetWidth)
  {
    if ((stat.entry_count > 0) && (stat.entry_count % stat.natural_depth == 0))
    {
      /* Don't allow the group to expand. */
      stat.entries_free = 0;
    }
    else
    {
      stat.entries_free = stat.natural_depth - (stat.entry_count % stat.natural_depth); 
    }
    if ((stat.counter_count > 0) && (stat.counter_count % stat.natural_depth == 0))
    {
      /* Don't allow the group to expand. */
      stat.counters_free = 0;
    }
    else
    {
      stat.counters_free = stat.natural_depth - (stat.counter_count % stat.natural_depth); 
    }
    if ((stat.meter_count > 0) && (stat.meter_count % stat.natural_depth == 0))
    {
      /* Don't allow the group to expand. */
      stat.meters_free = 0;
    }
    else
    {
      stat.meters_free = stat.natural_depth - (stat.meter_count % stat.natural_depth); 
    }
  }
  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
  {
    sysapiPrintf("  Entries free/needed %d/%d, counters free/needed %d/%d, meters free/needed %d/%d\n",
           stat.entries_free,  resourceReq->ruleCount, 
           stat.counters_free, resourceReq->counterCount, 
           stat.meters_free,   resourceReq->meterCount);
  }

  if ((stat.entries_free  >= resourceReq->ruleCount) &&
      (stat.counters_free >= resourceReq->counterCount) &&
      (stat.meters_free   >= resourceReq->meterCount))
  {
    rv = BCM_E_NONE;
  }
  else
  {
    rv = BCM_E_RESOURCE;
  }

  return rv;
}

static int _policy_group_find_group(int                             unit,
                                    BROAD_POLICY_ENTRY_t           *entryPtr,
                                    policy_resource_requirements_t *resourceReq,
                                    BROAD_GROUP_t                  *group)
{
  int          rv;
  sqsetWidth_t qsetWidth;
  int          groupEfpUsingIfp, policyEfpUsingIfp;

  debug_print_qset(&resourceReq->qsetAgg);

  /* Find an existing group that can satisfy the policy requirements. */
  for (qsetWidth = sqsetWidthFirst; qsetWidth < sqsetWidthLast; qsetWidth++)
  {
    rv = _policy_group_find_first(unit, entryPtr->policyStage, entryPtr->policyType, group);
    while (BCM_E_NONE == rv)
    {
      bcm_field_qset_t         qset;
      group_table_t           *groupPtr;

      groupPtr = &group_table[unit][entryPtr->policyStage][*group];

      if (groupPtr->flags & GROUP_USED)
      {
        if (super_qset_table[unit][groupPtr->sqset].sqsetWidth == qsetWidth)
        {
          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
            sysapiPrintf("- examining group %d ", *group);

          /* Insure the group has a suitable qset and enough free entries, counters, et al. */
          BCM_FIELD_QSET_INIT(qset);
          _policy_set_union(super_qset_table[unit][groupPtr->sqset].qsetAgg, &qset);
          _policy_set_union(super_qset_table[unit][groupPtr->sqset].qsetUdf, &qset);

          rv = _policy_set_subset(resourceReq->qsetAgg, resourceReq->customQset, qset, super_qset_table[unit][groupPtr->sqset].customQset);
          if (BCM_E_NONE == rv)
          {
            rv = _policy_group_resource_check(unit, entryPtr->policyType, resourceReq, groupPtr->gid);
            if (rv == BCM_E_NONE)
            {
              /* ensure that policies using IFP for EFP only go in groups using IFP for EFP */
              groupEfpUsingIfp  = (groupPtr->flags & GROUP_EFP_ON_IFP)                     ? L7_TRUE : L7_FALSE;
              policyEfpUsingIfp = (entryPtr->policyFlags & BROAD_POLICY_EGRESS_ON_INGRESS) ? L7_TRUE : L7_FALSE;

              if (groupEfpUsingIfp == policyEfpUsingIfp)
              {
                /* reuse existing group */
                if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
                  sysapiPrintf("- reuse group %d\n", *group);

                return BCM_E_NONE;
              }
            }
          }
        }
      }

      rv = _policy_group_find_next(unit, entryPtr->policyStage, entryPtr->policyType, group);
    }
  }

  return rv;
}

static int _policy_group_alloc_group(int                             unit,
                                     BROAD_POLICY_ENTRY_t           *entryPtr,
                                     policy_resource_requirements_t *resourceReq,
                                     BROAD_GROUP_t                  *group)
{
  int                    rv = BCM_E_FAIL;
  int                    sqset;
  int                    i;
  group_table_t         *groupPtr;
  int                    gid = 0;
  sqsetWidth_t           qsetWidth;
  super_qset_entry_t     sqsetInfo;

  /* make sure that only one group can do EFP on IFP */
  if (entryPtr->policyFlags & BROAD_POLICY_EGRESS_ON_INGRESS)
  {
    for (i = 0; i < group_table_size[unit][entryPtr->policyStage]; i++)
    {
      if ((group_table[unit][entryPtr->policyStage][i].flags & (GROUP_USED | GROUP_EFP_ON_IFP)) == (GROUP_USED | GROUP_EFP_ON_IFP))
      {
        return BCM_E_FAIL;
      }
    }
  }

  debug_print_qset(&resourceReq->qsetAgg);

  for (qsetWidth = sqsetWidthFirst; qsetWidth < sqsetWidthLast; qsetWidth++)
  {
    /* create all new groups based upon a super qset */
    rv = _policy_super_qset_find_match(unit, 
                                       entryPtr->policyType, 
                                       qsetWidth, 
                                       resourceReq->qsetAgg, 
                                       resourceReq->customQset, 
                                       &sqset);
    if (BCM_E_NONE != rv)
    {
      continue;
    }
  
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
      sysapiPrintf("- using super qset %d\n", sqset);

    _policy_sqset_get(unit, sqset, &sqsetInfo);

    /* Try to find a group priority that we can use to create this group. */
    rv = _policy_group_find_first(unit, entryPtr->policyStage, entryPtr->policyType, group);
    while (BCM_E_NONE == rv)
    {
      /* Enforce physical boundary conditions. */
      if (*group % sqsetInfo.status.slice_width_physical == 0)
      {
        if (!(group_table[unit][entryPtr->policyStage][*group].flags & GROUP_USED))
        {
          rv = bcm_field_group_create_mode(unit, sqsetInfo.qsetAgg, *group, bcmFieldGroupModeAuto, &gid);

          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
            sysapiPrintf("bcm_field_group_create() returned %d\n", rv);
    
          if (rv == BCM_E_NONE)
          {
            break;
          }
        }
      }
  
      rv = _policy_group_find_next(unit, entryPtr->policyStage, entryPtr->policyType, group);
    }
  
    if (rv == BCM_E_NONE)
    {
      rv = _policy_group_resource_check(unit, entryPtr->policyType, resourceReq, gid);
      if (rv == BCM_E_NONE)
      {
        for (i = *group; i < (*group + sqsetInfo.status.slice_width_physical); i++)
        {
          groupPtr = &group_table[unit][entryPtr->policyStage][i];
    
          groupPtr->flags      = GROUP_USED;
          if (entryPtr->policyFlags & BROAD_POLICY_EGRESS_ON_INGRESS)
          {
            groupPtr->flags |= GROUP_EFP_ON_IFP;
          }
          groupPtr->gid        = gid;
          groupPtr->type       = entryPtr->policyType;
          groupPtr->sqset      = sqset;
          /* PTin added: FFP */
          #if 1
          groupPtr->count_rules = 0;
          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
              sysapiPrintf("Resetted count_rules to 0 (gid=%u)\n",groupPtr->gid);
          #endif
        }

        break;
      }
      else
      {
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
          sysapiPrintf("_policy_group_resource_check() returned %d\n", rv);
  
        /* This policy won't fit in the new group, so destroy the group. */
        (void)bcm_field_group_destroy(unit, gid);
      }
    }
  }

  return rv;
}

static int _policy_group_delete_group(int unit, BROAD_POLICY_STAGE_t policyStage, BROAD_GROUP_t group)
{
    group_table_t     *groupPtr;
    int                i;
    super_qset_entry_t sqsetInfo;

    groupPtr         = &group_table[unit][policyStage][group];

    _policy_sqset_get(unit, groupPtr->sqset, &sqsetInfo);

    for (i = group; i < (group + sqsetInfo.status.slice_width_physical); i++)
    {
      groupPtr         = &group_table[unit][policyStage][i];
      groupPtr->flags  = GROUP_NONE;
      groupPtr->gid    = BROAD_GROUP_INVALID;
      /* PTin added: FFP */
      #if 1
      groupPtr->count_rules = 0;
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
          sysapiPrintf("Resetted count_rules to 0 (gid=%u)\n",groupPtr->gid);
      #endif
    }

    return BCM_E_NONE;
}


/* Group Management Functions */
static void _policy_group_lookupstatus_convert(L7_ushort16 hapiStatus, L7_ushort16 *bcmStatus, L7_BOOL isMask)
{
  *bcmStatus = 0;
  if (hapiStatus & BROAD_LOOKUPSTATUS_DOS_ATTACK_PKT)
  {
    *bcmStatus |= BCM_FIELD_LOOKUP_DOS_ATTACK;
  }
  if (hapiStatus & BROAD_LOOKUPSTATUS_UNRESOLVED_SA)
  {
    *bcmStatus |= BCM_FIELD_LOOKUP_L2_MISS;
  }
  if (hapiStatus & BROAD_LOOKUPSTATUS_LPM_HIT)
  {
    *bcmStatus |= BCM_FIELD_LOOKUP_L3_LPM_HIT;
  }
  if (hapiStatus & BROAD_LOOKUPSTATUS_STARGV_HIT)
  {
    *bcmStatus |= BCM_FIELD_LOOKUP_L3MC_SGV_HIT;
  }
  if (hapiStatus & BROAD_LOOKUPSTATUS_L3_DST_HIT)
  {
    *bcmStatus |= BCM_FIELD_LOOKUP_L3UC_DA_HIT;
  }
  if (hapiStatus & BROAD_LOOKUPSTATUS_L3_UC_SRC_HIT)
  {
    *bcmStatus |= BCM_FIELD_LOOKUP_L3UC_SA_HIT;
  }
  if (hapiStatus & BROAD_LOOKUPSTATUS_L2_USER_ENTRY_HIT)
  {
    *bcmStatus |= BCM_FIELD_LOOKUP_L2_CACHE_HIT;
  }
  if (hapiStatus & BROAD_LOOKUPSTATUS_L2_TABLE_DST_L3)
  {
    *bcmStatus |= BCM_FIELD_LOOKUP_L3_ROUTABLE;
  }
  if (hapiStatus & BROAD_LOOKUPSTATUS_L2_DST_HIT)
  {
    *bcmStatus |= BCM_FIELD_LOOKUP_L2_DA_HIT;
  }
  if (hapiStatus & BROAD_LOOKUPSTATUS_L2_SRC_STATIC)
  {
    *bcmStatus |= BCM_FIELD_LOOKUP_L2_SA_STATIC;
  }
  if (hapiStatus & BROAD_LOOKUPSTATUS_L2_SRC_HIT)
  {
    *bcmStatus |= BCM_FIELD_LOOKUP_L2_SA_HIT;
  }
  if (isMask)
  {
    if (hapiStatus & BROAD_LOOKUPSTATUS_INGRESS_SPG_STATE_MASK)
    {
      *bcmStatus |= BCM_FIELD_LOOKUP_ING_STP_MASK;
    }
  }
  else
  {
    if ((hapiStatus & BROAD_LOOKUPSTATUS_INGRESS_SPG_STATE_MASK) == BROAD_LOOKUPSTATUS_INGRESS_SPG_STATE_DIS)
    {
      *bcmStatus |= BCM_FIELD_LOOKUP_ING_STP_DIS;
    }
    if ((hapiStatus & BROAD_LOOKUPSTATUS_INGRESS_SPG_STATE_MASK) == BROAD_LOOKUPSTATUS_INGRESS_SPG_STATE_BLK)
    {
      *bcmStatus |= BCM_FIELD_LOOKUP_ING_STP_BLK;
    }
    if ((hapiStatus & BROAD_LOOKUPSTATUS_INGRESS_SPG_STATE_MASK) == BROAD_LOOKUPSTATUS_INGRESS_SPG_STATE_LRN)
    {
      *bcmStatus |= BCM_FIELD_LOOKUP_ING_STP_LRN;
    }
    if ((hapiStatus & BROAD_LOOKUPSTATUS_INGRESS_SPG_STATE_MASK) == BROAD_LOOKUPSTATUS_INGRESS_SPG_STATE_FWD)
    {
      *bcmStatus |= BCM_FIELD_LOOKUP_ING_STP_FWD;
    }
  }
  if (hapiStatus & BROAD_LOOKUPSTATUS_FB_VLAN_ID_VALID)
  {
    *bcmStatus |= BCM_FIELD_LOOKUP_VLAN_VALID;
  }
  if (hapiStatus & BROAD_LOOKUPSTATUS_VXLT_HIT)
  {
    *bcmStatus |= BCM_FIELD_LOOKUP_VXLT_HIT;
  }
  if (hapiStatus & BROAD_LOOKUPSTATUS_TUNNEL_HIT)
  {
    *bcmStatus |= BCM_FIELD_LOOKUP_L3_TUN_HIT;
  }
}

static int _policy_group_add_std_field(int                   unit,
                                       BROAD_POLICY_STAGE_t  policyStage,
                                       bcm_field_entry_t     eid,
                                       BROAD_POLICY_FIELD_t  field,
                                       char                 *value,
                                       char                 *mask)
{
    int  rv = BCM_E_NONE;
    L7_ushort16 tempValue16      = 0;
    L7_ushort16 tempMask16       = 0;
    uint32 ipType = bcmFieldIpTypeAny;
    L7_ushort16 lookupStatus     = 0;
    L7_ushort16 lookupStatusMask = 0;
    L7_uint32   tempValue32      = 0;
    L7_uint32   tempMask32       = 0;
    bcm_vlan_t  tempVlan = 0, tempVlanMask = 0;
    bcm_field_snap_header_t snapValue, snapMask;

    switch (field)
    {
    case BROAD_FIELD_MACDA:
        rv = bcm_field_qualify_DstMac(unit, eid, *((bcm_mac_t *)value), *((bcm_mac_t *)mask));
        break;
    case BROAD_FIELD_MACSA:
        rv = bcm_field_qualify_SrcMac(unit, eid, *((bcm_mac_t *)value), *((bcm_mac_t *)mask));
        break;
    case BROAD_FIELD_ETHTYPE:
        /* ETHTYPE is a special case as it can be used to specify IPv4/6 or OTHER. */
        tempValue16 = 0;
        memcpy(&tempValue16, value, sizeof(L7_ushort16));
        rv = bcm_field_qualify_EtherType(unit, eid, tempValue16, 0xFFFF);
        /* ETHTYPE must not be part of the qset, try IpType */
        if((BCM_E_NOT_FOUND == rv) || (BCM_E_PARAM == rv))
        {
          switch (tempValue16)  
          {
             case 0x0800:
                rv = bcm_field_qualify_IpType(unit, eid, bcmFieldIpTypeIpv4Any);
                break;
             case 0x86DD:
                rv = bcm_field_qualify_IpType(unit, eid, bcmFieldIpTypeIpv6);
                break;
             default:
                /* field not specified or not part of qset -- let caller decide */
                rv = BCM_E_NOT_FOUND;
                break;
          }
        }
        break;

    case BROAD_FIELD_L2_FORMAT:
        switch(*value)
        {
          case BROAD_L2_FORMAT_ETHERII:
             rv = bcm_field_qualify_L2Format(unit, eid, bcmFieldL2FormatEthII);
             break;
          case BROAD_L2_FORMAT_SNAP:
             rv = bcm_field_qualify_L2Format(unit, eid, bcmFieldL2FormatSnap);
             break;
          case BROAD_L2_FORMAT_LLC:
             rv = bcm_field_qualify_L2Format(unit, eid, bcmFieldL2FormatLlc);
             break;
          case BROAD_L2_FORMAT_NONE:
             rv = bcm_field_qualify_L2Format(unit, eid, bcmFieldL2FormatAny);
             break;
          default:
             rv = BCM_E_NOT_FOUND;
             break;
        }
        break;

    case BROAD_FIELD_SNAP:
        memcpy(&snapValue, value, sizeof(snapValue));
        memcpy(&snapMask,  mask,  sizeof(snapMask));
        rv = bcm_field_qualify_Snap(unit, eid, snapValue, snapMask);
        break;

    case BROAD_FIELD_TCP_CONTROL:
        rv = bcm_field_qualify_TcpControl(unit, eid, *((uint8 *)value), *((uint8 *)mask));
        break;

    case BROAD_FIELD_OVID:
        {
          tempVlan = tempVlanMask = 0;
          memcpy(&tempVlan, value, sizeof(bcm_vlan_t));
          memcpy(&tempVlanMask, mask, sizeof(bcm_vlan_t));
          rv = bcm_field_qualify_OuterVlan(unit, eid, tempVlan, tempVlanMask);
        }
        break;
    case BROAD_FIELD_IVID:
        {
          tempVlan = tempVlanMask = 0;
          memcpy(&tempVlan, value, sizeof(bcm_vlan_t));
          memcpy(&tempVlanMask, mask, sizeof(bcm_vlan_t));
          if (policyStage == BROAD_POLICY_STAGE_EGRESS)
          {
            /* EFP doesn't support innerVlan but does support innerVlanId.*/
            rv = bcm_field_qualify_InnerVlanId(unit, eid, tempVlan, tempVlanMask);
          }
          else
          {
            rv = bcm_field_qualify_InnerVlan(unit, eid, tempVlan, tempVlanMask);
          }
          if ((BCM_E_NONE == rv) && ((tempVlan != 0) || (tempVlanMask != 0)))
          {
            rv = bcm_field_qualify_VlanFormat(unit, eid, BCM_FIELD_PKT_FMT_INNER_TAGGED, BCM_FIELD_PKT_FMT_INNER_TAGGED);
          }
        }
        break;
    case BROAD_FIELD_DSCP:
        rv = bcm_field_qualify_DSCP(unit, eid, *((uint8*)value), *((uint8*)mask));
        break;
    case BROAD_FIELD_PROTO:
        rv = bcm_field_qualify_IpProtocol(unit, eid, *((uint8*)value), *((uint8*)mask));
        break;
    case BROAD_FIELD_SIP:
        {
          bcm_ip_t  localIP,  localMask; 
          memcpy(&localIP,   value, sizeof(bcm_ip_t));
          memcpy(&localMask, mask, sizeof(bcm_ip_t));
          rv = bcm_field_qualify_SrcIp(unit, eid, localIP, localMask);
        }
        break;
    case BROAD_FIELD_DIP:
        {
          bcm_ip_t  localIP,  localMask; 
          memcpy(&localIP, value, sizeof(bcm_ip_t));
          memcpy(&localMask,mask, sizeof(bcm_ip_t));
          rv = bcm_field_qualify_DstIp(unit, eid, localIP, localMask );
        }
        break;
    case BROAD_FIELD_ICMP_MSG_TYPE:
      /* Use L4 src port for ICMP Msg Type. We can do this because
         XGS3 just treats the L4 src port field as the first two
         bytes following the IP header. */
        tempValue16 = *((uint8 *)value) << 8;
        tempMask16  = *((uint8 *)mask) << 8;
        rv = bcm_field_qualify_L4SrcPort(unit, eid, tempValue16, tempMask16);
        break;
    case BROAD_FIELD_SPORT:
        tempValue16 = tempMask16 = 0;
        memcpy(&tempValue16, value, sizeof(uint16));
        memcpy(&tempMask16, mask, sizeof(uint16));
        rv = bcm_field_qualify_L4SrcPort(unit, eid, tempValue16, tempMask16);
        break;
    case BROAD_FIELD_DPORT:
        tempValue16 = tempMask16 = 0;
        memcpy(&tempValue16, value, sizeof(uint16));
        memcpy(&tempMask16, mask, sizeof(uint16));
        rv = bcm_field_qualify_L4DstPort(unit, eid, tempValue16, tempMask16);
        break;
    case BROAD_FIELD_IP6_HOPLIMIT:
        rv = bcm_field_qualify_Ip6HopLimit(unit, eid, *((uint8*)value), 0xFF);
        break;
    case BROAD_FIELD_IP6_NEXTHEADER:
        rv = bcm_field_qualify_Ip6NextHeader(unit, eid, *((uint8*)value), *((uint8*)mask));
        break;
    case BROAD_FIELD_IP6_SRC:
        rv = bcm_field_qualify_SrcIp6(unit, eid, *((bcm_ip6_t*)value), *((bcm_ip6_t*)mask));
        break;
    case BROAD_FIELD_IP6_DST:
        rv = bcm_field_qualify_DstIp6(unit, eid, *((bcm_ip6_t*)value), *((bcm_ip6_t*)mask));
        break;
    case BROAD_FIELD_IP6_FLOWLABEL:
        {
          tempValue32 = tempMask32 = 0;
          memcpy(&tempValue32, value, sizeof(L7_uint32));
          memcpy(&tempMask32, mask, sizeof(L7_uint32));
          rv = bcm_field_qualify_Ip6FlowLabel(unit, eid, tempValue32, tempMask32);
        }
        break;
    case BROAD_FIELD_IP6_TRAFFIC_CLASS:
        if (policyStage == BROAD_POLICY_STAGE_EGRESS)
        {
          rv = bcm_field_qualify_Tos(unit, eid, *((uint8*)value), *((uint8*)mask));
        }
        else
        {
          rv = bcm_field_qualify_Ip6TrafficClass(unit, eid, *((uint8*)value), *((uint8*)mask));
        }
        break;
//  case BROAD_FIELD_CLASS_ID:
//      rv = bcm_field_qualify_LookupClass0(unit, eid, *((uint8*)value), 0xF);
//      break;
    case BROAD_FIELD_L2_CLASS_ID:
        rv = bcm_field_qualify_SrcMacGroup(unit, eid, *((uint8*)value), 0xF);
        break;
    case BROAD_FIELD_LOOKUP_STATUS:
        tempValue16 = value[0]<<8;
        tempValue16 |= value[1];
        _policy_group_lookupstatus_convert(tempValue16, &lookupStatus,     L7_FALSE);
        tempMask16 = mask[0]<<8;
        tempMask16 |= mask[1];
        _policy_group_lookupstatus_convert(tempMask16,  &lookupStatusMask, L7_TRUE);
        rv = _policy_group_lookupstatus_qualify(unit, eid, lookupStatus, lookupStatusMask);
        break;
    case BROAD_FIELD_VLAN_FORMAT:
        {
           uint32 vlanFormat=0;
           uint32 vlanFormatMask =0;

           if (*value & BROAD_VLAN_FORMAT_CTAG)
           {
              vlanFormat |= BCM_FIELD_VLAN_FORMAT_INNER_TAGGED;
              vlanFormatMask |= BCM_FIELD_VLAN_FORMAT_INNER_TAGGED;
           }

           if (*value & BROAD_VLAN_FORMAT_NOCTAG)
           {
              vlanFormat &= ~BCM_FIELD_VLAN_FORMAT_INNER_TAGGED;
              vlanFormatMask |= BCM_FIELD_VLAN_FORMAT_INNER_TAGGED;
           }

           if (*value & BROAD_VLAN_FORMAT_STAG)
           {
              vlanFormat |= BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED;
              vlanFormatMask |= BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED;
           }

           if (*value & BROAD_VLAN_FORMAT_NOSTAG)
           {
              vlanFormat &= ~BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED;
              vlanFormatMask |= BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED;
           }

           if (*value & BROAD_VLAN_FORMAT_UNTAG)
           {
              vlanFormat &= ~BCM_FIELD_VLAN_FORMAT_INNER_TAGGED;
              vlanFormat &= ~BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED;
              vlanFormatMask |= BCM_FIELD_VLAN_FORMAT_INNER_TAGGED;
              vlanFormatMask |= BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED;
           }

           if (*value & BROAD_VLAN_FORMAT_CTAG_ZERO)
           {
              vlanFormat |= BCM_FIELD_VLAN_FORMAT_INNER_TAGGED;
              vlanFormat |= BCM_FIELD_VLAN_FORMAT_INNER_TAGGED_VID_ZERO;
              vlanFormatMask |= BCM_FIELD_VLAN_FORMAT_INNER_TAGGED_VID_ZERO;
              vlanFormatMask |= BCM_FIELD_VLAN_FORMAT_INNER_TAGGED;
           }

           if (*value & BROAD_VLAN_FORMAT_STAG_ZERO)
           {
              vlanFormat |= BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED;
              vlanFormat |= BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED_VID_ZERO;
              vlanFormatMask |= BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED_VID_ZERO;
              vlanFormatMask |= BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED;
           }

           rv = bcm_field_qualify_VlanFormat(unit, eid, vlanFormat, vlanFormatMask);
        }
        break;
    case BROAD_FIELD_IP_TYPE:
        {
           if (*value & BROAD_IP_TYPE_IPV4)
           {
              ipType = bcmFieldIpTypeIpv4Any;
           }
           else if (*value & BROAD_IP_TYPE_IPV6)
           {
              ipType = bcmFieldIpTypeIpv6;
           }
           else if (*value & BROAD_IP_TYPE_NONIP)
           {
              ipType = bcmFieldIpTypeNonIp;
           }

           if (ipType != bcmFieldIpTypeAny)
           {
             rv = bcm_field_qualify_IpType(unit, eid, ipType);
           }
        }
        break;
    // PTin added: FP
    case BROAD_FIELD_INPORTS:
        rv = bcm_field_qualify_InPorts(unit, eid, *((bcm_pbmp_t *) value), *((bcm_pbmp_t *) mask));
        break;
    case BROAD_FIELD_OUTPORT:
        rv = bcm_field_qualify_OutPort(unit, eid, *((bcm_port_t *) value), *((bcm_port_t *) mask));
        break;
    case BROAD_FIELD_SRCTRUNK:
        rv = bcm_field_qualify_SrcTrunk(unit, eid, *((bcm_trunk_t*)value), *((bcm_trunk_t*)mask));
        break;
    case BROAD_FIELD_PORTCLASS:
        rv = bcm_field_qualify_PortClass(unit, eid, *((uint32*)value), *((uint32*)mask));
        break;
    case BROAD_FIELD_DROP:
        rv = bcm_field_qualify_Drop(unit,eid,*((uint8*)value),1);
        break;
//  case BROAD_FIELD_L2_STATION_MOVE:
//      rv = bcm_field_qualify_L2StationMove(unit,eid,*((uint8*)value),1);
//      break;
    // PTin end
    default:
        rv = BCM_E_PARAM;
        break;
    }

    return rv;
}
static int _policy_group_lookupstatus_qualify(int unit, bcm_field_entry_t entry, L7_uint32 data, L7_uint32 mask)
{
  uint32 value;

  if (mask & BCM_FIELD_LOOKUP_DOS_ATTACK)
  {
    value = (data & BCM_FIELD_LOOKUP_DOS_ATTACK) ? 0x1 : 0x0;
    BCM_IF_ERROR_RETURN(bcm_field_qualify_DosAttack(unit, entry, value, 0x1));
  }
  
  if (mask & BCM_FIELD_LOOKUP_L2_MISS) 
  {
    value = (data & BCM_FIELD_LOOKUP_L2_MISS) ? 0x1 : 0x0;
    BCM_IF_ERROR_RETURN
        (bcm_field_qualify_L2StationMove(unit, entry, value, 0x1));
  }
  
  if (mask & BCM_FIELD_LOOKUP_L3_LPM_HIT) 
  {
    value = (data & BCM_FIELD_LOOKUP_L3_LPM_HIT) ? 0x1 : 0x0;
    BCM_IF_ERROR_RETURN
        (bcm_field_qualify_L3DestRouteHit(unit, entry, value, 0x1));
  }
  
  if (mask & BCM_FIELD_LOOKUP_L3MC_SGV_HIT) 
  {
    value = (data & BCM_FIELD_LOOKUP_L3MC_SGV_HIT) ? 0x1 : 0x0;
    BCM_IF_ERROR_RETURN
        (bcm_field_qualify_IpmcStarGroupHit(unit, entry, value, 0x1));
  }

  if (mask & BCM_FIELD_LOOKUP_L3UC_DA_HIT) 
  {
    value = (data & BCM_FIELD_LOOKUP_L3UC_DA_HIT) ? 0x1 : 0x0;
    BCM_IF_ERROR_RETURN
        (bcm_field_qualify_L3DestHostHit(unit, entry, value, 0x1));
  }

  if (mask & BCM_FIELD_LOOKUP_L3UC_SA_HIT) 
  {
    value = (data & BCM_FIELD_LOOKUP_L3UC_SA_HIT) ? 0x1 : 0x0;
    BCM_IF_ERROR_RETURN
        (bcm_field_qualify_L3SrcHostHit(unit, entry, value, 0x1));
  }

  if (mask & BCM_FIELD_LOOKUP_L2_CACHE_HIT) 
  {
    value = (data & BCM_FIELD_LOOKUP_L2_CACHE_HIT) ? 0x1 : 0x0;
    BCM_IF_ERROR_RETURN
        (bcm_field_qualify_L2CacheHit(unit, entry, value, 0x1));
  }

  if (mask & BCM_FIELD_LOOKUP_L2_DA_HIT) 
  {
    value = (data & BCM_FIELD_LOOKUP_L2_DA_HIT) ? 0x1 : 0x0;
    BCM_IF_ERROR_RETURN
        (bcm_field_qualify_L2DestHit(unit, entry, value, 0x1));
  }

  if (mask & BCM_FIELD_LOOKUP_L2_SA_STATIC) 
  {
    value = (data & BCM_FIELD_LOOKUP_L2_SA_STATIC) ? 0x1 : 0x0;
    BCM_IF_ERROR_RETURN
        (bcm_field_qualify_L2SrcStatic(unit, entry, value, 0x1));
  }
  
  if (mask & BCM_FIELD_LOOKUP_L2_SA_HIT) 
  {
    value = (data & BCM_FIELD_LOOKUP_L2_SA_HIT) ? 0x1 : 0x0;
    BCM_IF_ERROR_RETURN
        (bcm_field_qualify_L2SrcHit(unit, entry, value, 0x1));
  }

  if (mask & BCM_FIELD_LOOKUP_ING_STP_MASK) 
  {
    switch (data & BCM_FIELD_LOOKUP_ING_STP_MASK) 
    {
      case BCM_FIELD_LOOKUP_ING_STP_DIS:
        value = BCM_STG_STP_DISABLE;
        break;
      case BCM_FIELD_LOOKUP_ING_STP_BLK:
        value = BCM_STG_STP_BLOCK;
        break;
      case BCM_FIELD_LOOKUP_ING_STP_LRN:
        value = BCM_STG_STP_LEARN;
        break;
      case BCM_FIELD_LOOKUP_ING_STP_FWD:
        value = BCM_STG_STP_FORWARD;
        break;
      default:
        return (BCM_E_PARAM);
    }
    BCM_IF_ERROR_RETURN
        (bcm_field_qualify_IngressStpState(unit, entry, value, 0x3));
  }

  if (mask & BCM_FIELD_LOOKUP_VLAN_VALID) 
  {
    value = (data & BCM_FIELD_LOOKUP_VLAN_VALID) ? 0x1 : 0x0;
    BCM_IF_ERROR_RETURN
        (bcm_field_qualify_ForwardingVlanValid(unit, entry, value, 0x1));
  }

  if (mask & BCM_FIELD_LOOKUP_VXLT_HIT)
  {
    value = (data & BCM_FIELD_LOOKUP_VXLT_HIT) ? 0x1 : 0x0;
    BCM_IF_ERROR_RETURN
        (bcm_field_qualify_VlanTranslationHit(unit, entry, value, 0x1));
  }

  if (mask & BCM_FIELD_LOOKUP_L3_TUN_HIT)
  {
    // PTin added: new switch => SOC_IS_VALKYRIE2
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || SOC_IS_ENDURO(unit) || SOC_IS_VALKYRIE2(unit))
    {
      /* TunnelTerminated not supported in IFP, use TunnelType instead. */
      BCM_IF_ERROR_RETURN
        (bcm_field_qualify_TunnelType(unit, entry, bcmFieldTunnelTypeIp));
    }
    else
    {
      value = (data & BCM_FIELD_LOOKUP_L3_TUN_HIT) ? 0x1 : 0x0;
      BCM_IF_ERROR_RETURN
        (bcm_field_qualify_TunnelTerminated(unit, entry, value, 0x1));
    }
  }

  if ((mask & BCM_FIELD_LOOKUP_L3_ROUTABLE)) 
  {
    value = (data & BCM_FIELD_LOOKUP_L3_ROUTABLE) ? 0x1 : 0x0;
    BCM_IF_ERROR_RETURN
      (bcm_field_qualify_L3Routable(unit, entry, value, 0x1));
  }

  return (BCM_E_NONE);
}
static L7_uchar8 udfL4SrcDestPortData[4];
static L7_uchar8 udfL4SrcDestPortMask[4];

static void _policy_group_rule_udf_fields_clear()
{
  memset(udfL4SrcDestPortData, 0, sizeof(udfL4SrcDestPortData));
  memset(udfL4SrcDestPortMask, 0, sizeof(udfL4SrcDestPortMask));
}

static int _policy_group_add_udf_field(int                   unit,
                                       BROAD_POLICY_STAGE_t  policyStage,
                                       bcm_field_entry_t     eid,
                                       uint32                udfId,
                                       BROAD_POLICY_TYPE_t   type,
                                       BROAD_POLICY_FIELD_t  field,
                                       char                 *value,
                                       char                 *mask)
{
    int   rv = BCM_E_NONE;
    uint8 userData[BCM_FIELD_USER_FIELD_SIZE];
    uint8 userMask[BCM_FIELD_USER_FIELD_SIZE];
    unsigned short temp16;
    unsigned long  temp32;

    memset(userData, MASK_ALL, sizeof(userData));
    memset(userMask, MASK_ALL, sizeof(userMask));

    switch (field)
    {
    case BROAD_FIELD_ETHTYPE:
    /* ETHTYPE is at offset 0 for 2 bytes */
        temp16 = osapiHtons(*((L7_ushort16 *)value));
    memcpy(&userData[0], &temp16, 2);
    userMask[0] = 0xFF;
    userMask[1] = 0xFF;
    rv = bcm_field_qualify_data(unit, eid, sys0_ethType[unit], userData, userMask, 2);
    break;

  case BROAD_FIELD_PROTO:
    /* PROTO is at offset 0 for 1 byte */
    userData[0] = *value;
    userMask[0] = *mask;
    rv = bcm_field_qualify_data(unit, eid, sys0_ipProto[unit], userData, userMask, 1);
        break;

    case BROAD_FIELD_SPORT:
        /* SPORT is at offset 0 for 2 bytes */
        temp16 = osapiHtons(*((L7_ushort16 *)value));
        memcpy(&udfL4SrcDestPortData[0], &temp16, 2);
        temp16 = osapiHtons(*((L7_ushort16 *)mask));
        memcpy(&udfL4SrcDestPortMask[0], &temp16, 2);
        if (udfId == BROAD_SYSTEM_UDF)
        {
      rv = bcm_field_qualify_data(unit, eid, sys0_l4DstPort[unit], udfL4SrcDestPortData, udfL4SrcDestPortMask, 4);
        }
        else if (udfId == BROAD_ISCSI_UDF)
        {
      rv = bcm_field_qualify_data(unit, eid, iscsi_l4SrcDstPort[unit], udfL4SrcDestPortData, udfL4SrcDestPortMask, 4);
        }
        break;

    case BROAD_FIELD_DPORT:
        /* DPORT is at offset 2 for 2 byte */
        temp16 = osapiHtons(*((L7_ushort16 *)value));
        memcpy(&udfL4SrcDestPortData[2], &temp16, 2);
        temp16 = osapiHtons(*((L7_ushort16 *)mask));
        memcpy(&udfL4SrcDestPortMask[2], &temp16, 2);
        if (udfId == BROAD_SYSTEM_UDF)
        {
      rv = bcm_field_qualify_data(unit, eid, sys0_l4DstPort[unit], udfL4SrcDestPortData, udfL4SrcDestPortMask, 4);
        }
        else if (udfId == BROAD_ISCSI_UDF)
        {
      rv = bcm_field_qualify_data(unit, eid, iscsi_l4SrcDstPort[unit], udfL4SrcDestPortData, udfL4SrcDestPortMask, 4);
        }
        break;

    case BROAD_FIELD_IP6_NEXTHEADER: /* Tunneled */
    /* NEXTHDR is at offset 0 for 1 byte */
    userData[0] = *value;
    userMask[0] = *mask;
    rv = bcm_field_qualify_data(unit, eid, sys0_tunIpv6NextHdr[unit], userData, userMask, 1);
        break;

    case BROAD_FIELD_DIP:
        /* Source IP address is at offset 0 for 4 bytes */
        temp32 = osapiHtonl(*((L7_uint32 *)value));
        memcpy(&userData[0], &temp32, 4);
    temp32 = osapiHtonl(*((L7_uint32 *)mask));
        memcpy(&userMask[0], &temp32, 4);
    rv = bcm_field_qualify_data(unit, eid, iscsi_dstIpAddr[unit], userData, userMask, 4);
        break;

    case BROAD_FIELD_ISCSI_OPCODE:
        /* iSCSI opCode is at offset 0 for 1 byte */
        userData[0] = *value;
        userMask[0] = *mask;
    rv = bcm_field_qualify_data(unit, eid, iscsi_opCode[unit], userData, userMask, 1);
        break;

    case BROAD_FIELD_ISCSI_OPCODE_TCP_OPTIONS:
        /* iSCSI opCode is at offset 0 for 1 byte */
        userData[0] = *value;
        userMask[0] = *mask;
    rv = bcm_field_qualify_data(unit, eid, iscsi_opCodeTcpOptions[unit], userData, userMask, 1);
        break;

    default:
        rv = BCM_E_PARAM;
        break;
    }

    return rv;
}

static int _policy_field_is_udf(int unit, group_table_t *groupPtr, BROAD_POLICY_FIELD_t field)
{
  /* If the field is defined in the SDK... */
  if (field_map[field] < bcmFieldQualifyCount)
  {
    return BCM_FIELD_QSET_TEST(super_qset_table[unit][groupPtr->sqset].qsetUdf, field_map[field]);
  }
  else
  {
    /* Else, it's a custom field. */
    return CUSTOM_FIELD_QSET_TEST(super_qset_table[unit][groupPtr->sqset].customQset, field_map[field]);
  }
}

static int _policy_group_add_field(int                   unit,
                                   BROAD_POLICY_STAGE_t  policyStage,
                                   group_table_t        *groupPtr,
                                   bcm_field_entry_t     eid,
                                   BROAD_POLICY_FIELD_t  field,
                                   char                 *value,
                                   char                 *mask)
{
    int  rv;

    /* user defined fields require special consideration */
    if (_policy_field_is_udf(unit, groupPtr, field))
    {
        rv = _policy_group_add_udf_field(unit, policyStage, eid, super_qset_table[unit][groupPtr->sqset].udfId, 
                                         groupPtr->type, field, value, mask);
    }
    else
    {
        
        rv = _policy_group_add_std_field(unit, policyStage, eid, field, value, mask);
    }

    return rv;
}

static int _policy_group_add_actions(int                   unit,
                                     BROAD_POLICY_STAGE_t  policyStage,
                                     bcm_field_entry_t     eid,
                                     BROAD_ACTION_ENTRY_t *actPtr)
{
  int                         rv = BCM_E_NONE;
  int                         i;
  int                         dscp; 
  bcm_field_action_t          bcm_action;
  action_map_entry_t         *action_map;
  BROAD_POLICY_ACTION_t       action;
  BROAD_POLICY_ACTION_SCOPE_t action_scope;
  L7_uint32                   param0, param1, param2;

  switch (policyStage)
  {
  case BROAD_POLICY_STAGE_LOOKUP:
    action_map = lookup_action_map;
    break;
  case BROAD_POLICY_STAGE_INGRESS:
    action_map = ingress_action_map;
    break;
  case BROAD_POLICY_STAGE_EGRESS:
    action_map = egress_action_map;
    break;
  default:
    return BCM_E_PARAM;
  }

  /* map BROAD_ACTION_t to corresponding BCM action(s) */
  for (action = 0; action < BROAD_ACTION_LAST; action++)
  {
    for (i = 0; i < ACTIONS_PER_MAP_ENTRY; i++)
    {
      for (action_scope = BROAD_POLICY_ACTION_CONFORMING; action_scope < BROAD_POLICY_ACTION_LAST; action_scope++)
      {
        if ((action_scope == BROAD_POLICY_ACTION_EXCEEDING) && BROAD_EXCEEDING_ACTION_IS_SPECIFIED(actPtr, action))
        {
          bcm_action = action_map[action].yellow[i];
        }
        else if ((action_scope == BROAD_POLICY_ACTION_NONCONFORMING) && BROAD_NONCONFORMING_ACTION_IS_SPECIFIED(actPtr, action))
        {
          bcm_action = action_map[action].red[i];
        }
        else if ((action_scope == BROAD_POLICY_ACTION_CONFORMING) && BROAD_CONFORMING_ACTION_IS_SPECIFIED(actPtr, action))
        {
          bcm_action = action_map[action].green[i];
        }
        else
        {
          /* Action is not specified. */
          continue;
        }

        /* handle case of redirect to invalid port, e.g. LAG with no members */
        if ((BROAD_ACTION_REDIRECT == action) || (BROAD_ACTION_MIRROR == action))
        {
          if (BCMX_LPORT_INVALID == actPtr->u.ifp_parms.modid)
          {
            /* Skip this action for now. 
               This policy will be updated later when a member is added to the LAG. */
            continue;
          }
        }

        if (BROAD_ACTION_TRAP_TO_CPU == action)
        {
          if (hpcBroadMasterCpuModPortGet(&param0, &param1) != BCM_E_NONE)
          {
            L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID, "Couldn't get CPU port info.");
            return BCM_E_FAIL;
          }
        }
        else
        {
          hapiBroadPolicyActionParmsGet(actPtr, policyStage, action, action_scope, &param0, &param1, &param2);
        }

        /* handle case of SET_TOS action. for non-conforming/exceeding traffic, there is 
         * no field in POLICY Table to set TOS. instead, we use bcmFieldActionRpDscpNew action,
         * and require to shift TOS value.
         */
        if ((action == BROAD_ACTION_SET_TOS) &&
            ((bcm_action == bcmFieldActionYpDscpNew) || (bcm_action == bcmFieldActionRpDscpNew) || 
             (bcm_action == bcmFieldActionGpDscpNew)))
        {
          dscp = param0 << 3; /* Treat TOS as a DSCP value */
          param0 = dscp;
        }

        if (PROFILE_ACTION_INVALID == bcm_action)
        {
          return BCM_E_CONFIG;
        }
        else if (bcmFieldActionGpDropPrecedence == bcm_action)
        {
          /* conforming color in sysparam0 */
          rv = bcm_field_action_add(unit, eid, bcm_action, color_map[param0], 0);
        }
        else if (bcmFieldActionYpDropPrecedence == bcm_action)
        {
          /* conforming color in sysparam1 */
          rv = bcm_field_action_add(unit, eid, bcm_action, color_map[param1], 0);
        }
        else if (bcmFieldActionRpDropPrecedence == bcm_action)
        {
          /* conforming color in sysparam2 */
          rv = bcm_field_action_add(unit, eid, bcm_action, color_map[param2], 0);
        }
        else if (PROFILE_ACTION_NONE != bcm_action)
        {
          /* set remaining actions */
          rv = bcm_field_action_add(unit, eid, bcm_action, param0, param1);
          if (BCM_E_NONE != rv)
          {
            return rv;
          }
        }

        /* For a redirect action, add an implicit lookup status qualifier to 
         * forward based on STP status - allow only if the state is set to "forwarding" */
        /* Allow redirecting to CPU regardless of STP state */
        if ((bcmFieldActionRedirect == bcm_action) && (param1 != CMIC_PORT(unit)))
        {
          L7_ushort16 val  = BROAD_LOOKUPSTATUS_INGRESS_SPG_STATE_FWD;
          L7_ushort16 mask = BROAD_LOOKUPSTATUS_INGRESS_SPG_STATE_MASK;

          /* Try and add a lookup status qualifier */
          rv = _policy_group_add_std_field(unit, policyStage, eid, BROAD_FIELD_LOOKUP_STATUS,
                                           (char *)&val, (char *)&mask);
          if (BCM_E_NONE != rv)
          {
            /* If error is BCM_E_NOT_FOUND, it means that the sqset for the group does not contain
               lookup status. This could happen only with a system policy.
               In this case, clear the error and return.
             */
            if((BCM_E_NOT_FOUND == rv) || 
               ((SOC_IS_HAWKEYE(unit)) && (BCM_E_PARAM == rv)))
            {
              rv = BCM_E_NONE;
            }
            /* Otherwise log the error */
            else if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
            {
              sysapiPrintf("Add lookup status to redirect/mirror action FAILED with ret val = %d\n", rv);
            }
          }
        }
      }
    }
  }
  return rv;
}

/* PTin added: policer */
#if 0
static int _policy_group_add_policer(int unit, BROAD_POLICY_STAGE_t stage, bcm_field_entry_t eid, BROAD_POLICY_RULE_ENTRY_t *rulePtr, L7_int *policer_id)
{
    int                  rv = BCM_E_NONE;
    bcm_policer_config_t policer_profile;
    BROAD_METER_ENTRY_t *policerPtr;
    L7_int pol_id;

    policerPtr = &rulePtr->u.meter.meterInfo;

    policer_profile.flags         = 0;
    policer_profile.mode          = (policerPtr->colorMode == BROAD_METER_COLOR_BLIND ?
                                     bcmPolicerModeCommitted :
                                     bcmPolicerModeTrTcm);
    policer_profile.ckbits_sec    = policerPtr->cir;
    policer_profile.ckbits_burst  = policerPtr->cbs*8;
    policer_profile.pkbits_sec    = policerPtr->pir;
    policer_profile.pkbits_burst  = policerPtr->pbs*8;

    /* Create policer */
    rv = bcm_policer_create(unit,&policer_profile,&pol_id);
    if (BCM_E_NONE != rv)
        return rv;

    rv = bcm_field_entry_policer_attach(unit,eid,0,pol_id);
    if (BCM_E_NONE != rv)
    {
        bcm_policer_destroy(unit,pol_id);
        return rv;
    }

    /* Save policer_id */
    if (policer_id!=L7_NULLPTR)  *policer_id = pol_id;

    return rv;
}
#endif
/* PTin end */

static int _policy_group_add_meter(int unit, BROAD_POLICY_STAGE_t stage, bcm_field_entry_t eid, BROAD_POLICY_RULE_ENTRY_t *rulePtr)
{
    int                  rv = BCM_E_NONE;
    BROAD_METER_ENTRY_t *meterPtr;
    uint32               counter_flags;
    uint64               zero64;

    meterPtr = &rulePtr->u.meter.meterInfo;

    if (rulePtr->ruleFlags & BROAD_METER_SHARED)
    {
        bcm_field_entry_t src_eid;

        src_eid = BROAD_ENTRY_TO_BCM_ENTRY(rulePtr->meterSrcEntry);

        rv = bcm_field_meter_share(unit, src_eid, eid);
        if (BCM_E_NONE != rv)
            return rv;
    }
    else
    {
        rv = bcm_field_meter_create(unit, eid);
        if (BCM_E_NONE != rv)
            return rv;

        /* all meters have cir/cbs and pir/pbs, so we always use trTCM */
        rv = bcm_field_meter_set(unit, eid, BCM_FIELD_METER_COMMITTED,
                                 meterPtr->cir, meterPtr->cbs * 8);
        if (BCM_E_NONE != rv)
            return rv;

        rv = bcm_field_meter_set(unit, eid, BCM_FIELD_METER_PEAK,
                                 meterPtr->pir, meterPtr->pbs * 8);
        if (BCM_E_NONE != rv)
            return rv;
    }

    rv = bcm_field_action_add(unit, eid, bcmFieldActionMeterConfig,
                              (meterPtr->colorMode == BROAD_METER_COLOR_BLIND ?
                               BCM_FIELD_METER_MODE_trTCM_COLOR_BLIND :
                               BCM_FIELD_METER_MODE_trTCM_COLOR_AWARE),
                              0);
    if (BCM_E_NONE != rv)
        return rv;

    /* add counter to support stats per entry */
    rv = bcm_field_counter_create(unit, eid);
    if (BCM_E_NONE != rv)
        return rv;

    /* zero values prior to first use */
    COMPILER_64_ZERO(zero64);
    (void)bcm_field_counter_set(unit, eid, 0, zero64);
    (void)bcm_field_counter_set(unit, eid, 1, zero64);

    counter_flags = BCM_FIELD_COUNTER_MODE_PACKETS;
    if (stage != BROAD_POLICY_STAGE_EGRESS)
    {
      counter_flags |= BCM_FIELD_COUNTER_MODE_GREEN_NOTGREEN;
    }
    else
    {
      counter_flags |= BCM_FIELD_COUNTER_MODE_YES_NO;
    }
    rv = bcm_field_action_add(unit, eid, bcmFieldActionUpdateCounter,
                  counter_flags, 0);
    if (BCM_E_NONE != rv)
        return rv;

    return rv;
}

static int _policy_group_add_counter(int unit, bcm_field_entry_t eid, BROAD_POLICY_RULE_ENTRY_t *rulePtr)
{
    int                    rv = BCM_E_NONE;
    uint32                 flags;
    uint64                 zero64;
    BROAD_COUNTER_ENTRY_t *counterPtr;

    counterPtr = &rulePtr->u.counter.counterInfo;

    if (rulePtr->ruleFlags & BROAD_COUNTER_SHARED)
    {
        bcm_field_entry_t src_eid;

        src_eid = BROAD_ENTRY_TO_BCM_ENTRY(rulePtr->meterSrcEntry);

        rv = bcm_field_counter_share(unit, src_eid, eid);
        if (BCM_E_NONE != rv)
            return rv;
    }
    else
    {
        /* add counter to support stats */
        rv = bcm_field_counter_create(unit, eid);
        if (BCM_E_NONE != rv)
            return rv;
    }

    /* zero values prior to first use */
    COMPILER_64_ZERO(zero64);
    (void)bcm_field_counter_set(unit, eid, 0, zero64);
    (void)bcm_field_counter_set(unit, eid, 1, zero64);

    /* update flags to indicate packet or byte counter */
    flags = BCM_FIELD_COUNTER_MODE_YES_NO;
    if (BROAD_COUNT_PACKETS == counterPtr->mode)
        flags |= BCM_FIELD_COUNTER_MODE_PACKETS;
    else
        flags |= BCM_FIELD_COUNTER_MODE_BYTES;

    rv = bcm_field_action_add(unit, eid, bcmFieldActionUpdateCounter, flags, 0);
    if (BCM_E_NONE != rv)
        return rv;

    return rv;
}

extern int hapiBroadBcmGroupRequired(int unit);
/* 
 * Returns the reserved group id if neccessary
 * returns -1, if there is no reserved group
 */
int hapiBroadPolicyBcmReservedGroupGet(int unit)
{
  int groupid_rev;

  if (hapiBroadBcmGroupRequired(unit))
  {
#ifdef L7_IPV6_PACKAGE
    /* System rules in slices 0 through 2 */
    groupid_rev = 3;
#else
    /* System rules in slices 0 through 1 */
    groupid_rev = 2;
#endif

    if (SOC_IS_HELIX1(unit))
    {
      /* For Helix, we need to ensure that the COS policies are kept in a separate
      (i.e. lower priority slice) than the other system policies. This isn't necessary
      for Scorpion since we can include OVID as a qualifier in all of the qsets used
      for system policies, meaning that the COS policies will go into the lowest slice,
      w/ the lowest rule priority. */
      groupid_rev++;
    }
  }
  else
    groupid_rev = -1;

  return groupid_rev;
}

/* 
 * Reserved Groups for XGS3 devices use the ACL/DS group block due to 
 * their not being uniformly applied on the entire stack.  This routine
 * determines the appropriate first group within the group.  Specifically,
 * we reserve a group for BCM if necessary
 */
int hapiBroadPolicyFirstAclDsGroupGet(int unit)
{
  int groupid_rev;

  /* Although Scorpion, TR2, and Apollo support intraslice doublewide mode,
     their system qsets end up using doublewide mode. */
  // PTin added: new switch => SOC_IS_VALKYRIE2
  if (_policy_supports_intraslice_doublewide_mode(unit) && 
      !SOC_IS_SCORPION(unit) && 
      !SOC_IS_TRIUMPH2(unit) && 
      !SOC_IS_APOLLO(unit) &&
      !SOC_IS_ENDURO(unit) &&
      !SOC_IS_VALKYRIE2(unit) &&
      !SOC_IS_TRIDENT(unit))        /* PTin added: new switch BCM56843 */
  {
    /* System rules in slice 0. */
    groupid_rev = 1;
  }
  else if (_policy_supports_wide_mode(unit) && !SOC_IS_SCORPION(unit))
  {
    /* System rules in slices 0 and 1 */
    groupid_rev = 2;
  }
  else
  {
#ifdef L7_IPV6_PACKAGE
    /* System rules in slices 0 through 2 */
    groupid_rev = 3;
#else
    if (SOC_IS_SCORPION(unit))
    {       
    /* For scorpion devices that don't include the IPv6 package,
       we still need to allocate 3 slices for system/COS policies.
       This is because of the order in which the system policies
       are installed. The L2 system policies are installed first, which
       puts them in the highest priority system slice. The L3 system
       policies are installed next, in a lower priority slice. If we
       don't reserve an even lower priority slice for the COS policies,
       then they will be placed w/ the L2 system policies, causing the
       L3 system policies to be trumped by the COS policies. Therefore
       we need to ensure that the COS policies are in a lower priority
       slice than the L3 system policies. */
    /* System rules in slices 0 through 2 */
     groupid_rev = 3;
    }
    else
    {
    /* System rules in slices 0 through 1 */
     groupid_rev = 2;
    }
#endif
  }

  if (hapiBroadBcmGroupRequired(unit))
  {
    groupid_rev++;
  }

  if (SOC_IS_HELIX1(unit))
  {
    /* For Helix, we need to ensure that the COS policies are kept in a separate
    (i.e. lower priority slice) than the other system policies. This isn't necessary
    for Scorpion since we can include OVID as a qualifier in all of the qsets used
    for system policies, meaning that the COS policies will go into the lowest slice,
    w/ the lowest rule priority. */
    groupid_rev++;
  }

  return groupid_rev;
}


static int _policy_group_alloc_init(int unit, BROAD_POLICY_STAGE_t policyStage, int groups)
{
    int lowPrioGroup;

    lowPrioGroup = hapiBroadPolicyFirstAclDsGroupGet(unit);

    switch (policyStage)
    {
    case BROAD_POLICY_STAGE_LOOKUP:
    case BROAD_POLICY_STAGE_EGRESS:
      /* low priority group starts at 0 and goes for 4 */
      group_alloc_table[unit][policyStage][ALLOC_BLOCK_LOW].lowPrio     = 0;
      group_alloc_table[unit][policyStage][ALLOC_BLOCK_LOW].highPrio    = groups - 1;

      group_alloc_table[unit][policyStage][ALLOC_BLOCK_MEDIUM].lowPrio  = 0;
      group_alloc_table[unit][policyStage][ALLOC_BLOCK_MEDIUM].highPrio = groups - 1;

      group_alloc_table[unit][policyStage][ALLOC_BLOCK_HIGH].lowPrio    = 0;
      group_alloc_table[unit][policyStage][ALLOC_BLOCK_HIGH].highPrio   = groups - 1;

      /* PTin added: policer */
      group_alloc_table[unit][policyStage][ALLOC_BLOCK_PTIN].lowPrio    = 0;
      group_alloc_table[unit][policyStage][ALLOC_BLOCK_PTIN].highPrio   = groups - 1;
      /* PTin end */

      /* PTin added: client stats: groups 0-2 [ 3 * 128/(4*2) = 48 clients ] */
      group_alloc_table[unit][policyStage][ALLOC_BLOCK_STATS_CLIENT].lowPrio  = 0;
      group_alloc_table[unit][policyStage][ALLOC_BLOCK_STATS_CLIENT].highPrio = groups-3;

      /* PTin added: EVC stats: groups 3 [ 1 * 128/(4*2) = 16 services/ports counters ] */
      group_alloc_table[unit][policyStage][ALLOC_BLOCK_STATS_EVC].lowPrio     = groups-2;
      group_alloc_table[unit][policyStage][ALLOC_BLOCK_STATS_EVC].highPrio    = groups-1;
      /* PTin end */

      break;
    case BROAD_POLICY_STAGE_INGRESS:
      if (SOC_IS_RAPTOR(unit) || SOC_IS_HAWKEYE(unit))
      { 
        int  lowPrio = 0;
        /* Assumption is the each group contains 128 rules */
 #ifdef L7_ROUTING_PACKAGE
      /* If it is routing package and silicon is Raptor */
        lowPrio = ((platRtrRouteMaxEntriesGet() + platRtrArpMaxEntriesGet())/ 127);
        lowPrio +=(((platRtrRouteMaxEntriesGet() + platRtrArpMaxEntriesGet())  % 127) > 0 ? 1 : 0);
#endif
        /* Raptor slice distribution:
           Slice 0 - n: Routing (only if routing is present)
           Slice n,n+1: System policies
           Slice n+2 - 7: User policies
        */

        group_alloc_table[unit][policyStage][ALLOC_BLOCK_LOW].lowPrio     = lowPrio;
        group_alloc_table[unit][policyStage][ALLOC_BLOCK_LOW].highPrio    = lowPrio + 1;

        group_alloc_table[unit][policyStage][ALLOC_BLOCK_MEDIUM].lowPrio  = lowPrio + 2;
        group_alloc_table[unit][policyStage][ALLOC_BLOCK_MEDIUM].highPrio = groups - 1;

        group_alloc_table[unit][policyStage][ALLOC_BLOCK_HIGH].lowPrio    = lowPrio + 2;
        group_alloc_table[unit][policyStage][ALLOC_BLOCK_HIGH].highPrio   = groups - 1;

        /* PTin added: policer */
        group_alloc_table[unit][policyStage][ALLOC_BLOCK_PTIN].lowPrio    = lowPrio + 2;
        group_alloc_table[unit][policyStage][ALLOC_BLOCK_PTIN].highPrio   = groups - 1;
        /* PTin end */

        /* PTin added: stats */
        group_alloc_table[unit][policyStage][ALLOC_BLOCK_STATS_CLIENT].lowPrio  = lowPrio;
        group_alloc_table[unit][policyStage][ALLOC_BLOCK_STATS_CLIENT].highPrio = groups - 3;

        group_alloc_table[unit][policyStage][ALLOC_BLOCK_STATS_EVC].lowPrio     = groups - 2;
        group_alloc_table[unit][policyStage][ALLOC_BLOCK_STATS_EVC].highPrio    = groups - 1;
        /* PTin end */
      }
      else
      {
        if (SOC_IS_TRIDENT(unit))
        {
          /* low priority group starts at 0 and goes for 1 or 2 */
          group_alloc_table[unit][policyStage][ALLOC_BLOCK_LOW].lowPrio     = 0;
          group_alloc_table[unit][policyStage][ALLOC_BLOCK_LOW].highPrio    = lowPrioGroup - 1;
          if (hapiBroadBcmGroupRequired(unit))
          {
            group_alloc_table[unit][policyStage][ALLOC_BLOCK_LOW].highPrio--;
          }

          group_alloc_table[unit][policyStage][ALLOC_BLOCK_MEDIUM].lowPrio  = lowPrioGroup;
          group_alloc_table[unit][policyStage][ALLOC_BLOCK_MEDIUM].highPrio = lowPrioGroup + 1;   /* PTin modified: policer */

          group_alloc_table[unit][policyStage][ALLOC_BLOCK_HIGH].lowPrio    = lowPrioGroup;
          group_alloc_table[unit][policyStage][ALLOC_BLOCK_HIGH].highPrio   = lowPrioGroup + 1;   /* PTin modified: policer */

          /* PTin added: policer */
          group_alloc_table[unit][policyStage][ALLOC_BLOCK_PTIN].lowPrio    = lowPrioGroup + 4;
          group_alloc_table[unit][policyStage][ALLOC_BLOCK_PTIN].highPrio   = groups - 1;
          /* PTin end */

          /* PTin added: client stats: groups 0-6 [ 7 * 256/(4*2) = 224 clients ] */
          group_alloc_table[unit][policyStage][ALLOC_BLOCK_STATS_CLIENT].lowPrio  = lowPrioGroup + 2;
          group_alloc_table[unit][policyStage][ALLOC_BLOCK_STATS_CLIENT].highPrio = lowPrioGroup + 4 - 1;

          /* PTin added: EVC stats: groups 7-7 [ 1 * 256/4 = 64 service/port counters ] */
          group_alloc_table[unit][policyStage][ALLOC_BLOCK_STATS_EVC].lowPrio     = lowPrioGroup + 4;
          group_alloc_table[unit][policyStage][ALLOC_BLOCK_STATS_EVC].highPrio    = groups - 1;
        }
        else
        {
          /* low priority group starts at 0 and goes for 1 or 2 */
          group_alloc_table[unit][policyStage][ALLOC_BLOCK_LOW].lowPrio     = 0;
          group_alloc_table[unit][policyStage][ALLOC_BLOCK_LOW].highPrio    = lowPrioGroup - 1;
          if (hapiBroadBcmGroupRequired(unit))
          {
            group_alloc_table[unit][policyStage][ALLOC_BLOCK_LOW].highPrio--;
          }

          group_alloc_table[unit][policyStage][ALLOC_BLOCK_MEDIUM].lowPrio  = lowPrioGroup;
          group_alloc_table[unit][policyStage][ALLOC_BLOCK_MEDIUM].highPrio = lowPrioGroup + 1; //groups/2 - 3;   /*groups - 1;*/     /* PTin modified: policer */

          group_alloc_table[unit][policyStage][ALLOC_BLOCK_HIGH].lowPrio    = lowPrioGroup; //groups/2 - 2;
          group_alloc_table[unit][policyStage][ALLOC_BLOCK_HIGH].highPrio   = lowPrioGroup + 1; //groups/2 - 1;   /*groups - 1;*/     /* PTin modified: policer */

          /* PTin added: policer */
          group_alloc_table[unit][policyStage][ALLOC_BLOCK_PTIN].lowPrio    = lowPrioGroup + 6;
          group_alloc_table[unit][policyStage][ALLOC_BLOCK_PTIN].highPrio   = groups - 1;
          /* PTin end */

          /* PTin added: client stats: groups 0-6 [ 7 * 256/(4*2) = 224 clients ] */
          group_alloc_table[unit][policyStage][ALLOC_BLOCK_STATS_CLIENT].lowPrio  = lowPrioGroup + 2;
          group_alloc_table[unit][policyStage][ALLOC_BLOCK_STATS_CLIENT].highPrio = lowPrioGroup + 4 - 1; //groups/2 - 3;

          /* PTin added: EVC stats: groups 7-7 [ 1 * 256/4 = 64 service/port counters ] */
          group_alloc_table[unit][policyStage][ALLOC_BLOCK_STATS_EVC].lowPrio     = lowPrioGroup + 4; //groups/2 - 2;
          group_alloc_table[unit][policyStage][ALLOC_BLOCK_STATS_EVC].highPrio    = lowPrioGroup + 6 - 1; //groups/2 - 1;
        }
        /* PTin end */

        LOG_INFO(LOG_CTX_STARTUP,"ALLOC_BLOCK_LOW   : Groups %u - %u",
                 group_alloc_table[unit][policyStage][ALLOC_BLOCK_LOW].lowPrio,
                 group_alloc_table[unit][policyStage][ALLOC_BLOCK_LOW].highPrio);
        LOG_INFO(LOG_CTX_STARTUP,"ALLOC_BLOCK_MEDIUM: Groups %u - %u",
                 group_alloc_table[unit][policyStage][ALLOC_BLOCK_MEDIUM].lowPrio,
                 group_alloc_table[unit][policyStage][ALLOC_BLOCK_MEDIUM].highPrio);
        LOG_INFO(LOG_CTX_STARTUP,"ALLOC_BLOCK_HIGH  : Groups %u - %u",
                 group_alloc_table[unit][policyStage][ALLOC_BLOCK_HIGH].lowPrio,
                 group_alloc_table[unit][policyStage][ALLOC_BLOCK_HIGH].highPrio);
        LOG_INFO(LOG_CTX_STARTUP,"ALLOC_BLOCK_STATS_CLIENT: Groups %u - %u",
                 group_alloc_table[unit][policyStage][ALLOC_BLOCK_STATS_CLIENT].lowPrio,
                 group_alloc_table[unit][policyStage][ALLOC_BLOCK_STATS_CLIENT].highPrio);
        LOG_INFO(LOG_CTX_STARTUP,"ALLOC_BLOCK_STATS_EVC   : Groups %u - %u",
                 group_alloc_table[unit][policyStage][ALLOC_BLOCK_STATS_EVC].lowPrio,
                 group_alloc_table[unit][policyStage][ALLOC_BLOCK_STATS_EVC].highPrio);
        LOG_INFO(LOG_CTX_STARTUP,"ALLOC_BLOCK_PTIN  : Groups %u - %u",
                 group_alloc_table[unit][policyStage][ALLOC_BLOCK_PTIN].lowPrio,
                 group_alloc_table[unit][policyStage][ALLOC_BLOCK_PTIN].highPrio);
      }
      break;
    default:
      break;
    }

    return BCM_E_NONE;
}

static int _policy_group_table_init(int unit, BROAD_POLICY_STAGE_t policyStage, int groups)
{
    int i;
    int group_table_bytes;

    if (NULL == group_table[unit][policyStage])
    {
        group_table_size[unit][policyStage] = groups;
        group_table_bytes                   = groups * sizeof(group_table_t);
        if (groups == 0)
        {
          group_table[unit][policyStage]    = L7_NULL;
        }
        else
        {
          group_table[unit][policyStage]    = (group_table_t *)osapiMalloc(L7_DRIVER_COMPONENT_ID, group_table_bytes);
          if (NULL == group_table[unit][policyStage])
          {
              LOG_ERROR(0);
              return BCM_E_MEMORY;
          }
        }
    }

    for (i = 0; i < group_table_size[unit][policyStage]; i++)
    {
        group_table[unit][policyStage][i].flags = GROUP_NONE;
        group_table[unit][policyStage][i].gid   = BROAD_GROUP_INVALID;
    }

    return BCM_E_NONE;
}

int _policy_group_total_slices(int unit, BROAD_POLICY_STAGE_t policyStage)
{
    int          total_slices = 0;

    switch (policyStage)
    {
    case BROAD_POLICY_STAGE_LOOKUP:
      // PTin added: new switch => SOC_IS_VALKYRIE2
      if (SOC_IS_FIREBOLT2(unit) ||
          SOC_IS_TR_VL(unit) ||
          SOC_IS_SCORPION(unit) || 
          SOC_IS_TRIUMPH2(unit) || 
          SOC_IS_APOLLO(unit)   ||
          SOC_IS_ENDURO(unit)   ||
          SOC_IS_VALKYRIE2(unit) ||
          SOC_IS_TRIDENT(unit))     /* PTin added: new switch BCM56843 */
        total_slices = 4;
      else  
        total_slices = 0;
      break;
    case BROAD_POLICY_STAGE_INGRESS:
      if (SOC_IS_ENDURO(unit))
      {
        total_slices = 8;
      }
      // PTin added: new switch => SOC_IS_VALKYRIE2
      else if ((SOC_IS_FIREBOLT(unit)  || 
          SOC_IS_FIREBOLT2(unit) ||
          SOC_IS_TR_VL(unit) ||
          SOC_IS_RAVEN(unit) || SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || SOC_IS_VALKYRIE2(unit))
           && !SOC_IS_TRIDENT(unit)) /* PTin added: new switch BCM56843 */
          total_slices = 16;
      else if (SOC_IS_RAPTOR(unit) || SOC_IS_HAWKEYE(unit)) 
      { 
        /* Note tcam_slices only counts internal slices. */
        if (soc_feature(unit, soc_feature_field_slices2)) {
           total_slices = 2;
        } else if (soc_feature(unit, soc_feature_field_slices4)) {
           total_slices = 4;
        } else {
          total_slices = soc_feature(unit, soc_feature_field_slices8) ? 8 : 16;
        }   
      }
      else if (SOC_IS_SCORPION(unit))
      {
        total_slices = 12;
      }
      /* PTin added: new switch BCM56843 */
      else if (SOC_IS_TRIDENT(unit))
      {
          total_slices = 10;
      }
      /* PTin end */
      else
      {
        total_slices = 8;
      }
      break;
    case BROAD_POLICY_STAGE_EGRESS:
      // PTin added: new switch => SOC_IS_VALKYRIE2
      if (SOC_IS_FIREBOLT2(unit) ||
          SOC_IS_TR_VL(unit)     ||
          SOC_IS_SCORPION(unit)  || 
          SOC_IS_TRIUMPH2(unit)  || 
          SOC_IS_APOLLO(unit)    ||
          SOC_IS_ENDURO(unit)    ||
          SOC_IS_VALKYRIE2(unit) ||
          SOC_IS_TRIDENT(unit))     /* PTin added: new switch BCM56843 */
        total_slices = 4;
      else  
        total_slices = 0;
      break;
    default:
      break;
    }
    return total_slices;
}

L7_BOOL policy_stage_supported(int unit, BROAD_POLICY_STAGE_t policyStage)
{
  L7_BOOL supported = L7_FALSE;

  /* Only FB2 supports the LOOKUP, INGRESS, and EGRESS stages */
  switch (policyStage)
  {
  case BROAD_POLICY_STAGE_LOOKUP:
    // PTin added: new switch => SOC_IS_VALKYRIE2
    if (SOC_IS_FIREBOLT2(unit) ||
        SOC_IS_TR_VL(unit)     ||
        SOC_IS_SCORPION(unit)  || 
        SOC_IS_TRIUMPH2(unit)  ||
        SOC_IS_APOLLO(unit)    ||
        SOC_IS_ENDURO(unit)    ||
        SOC_IS_VALKYRIE2(unit) ||
        SOC_IS_TRIDENT(unit))       /* PTin added: new switch BCM56843 */
    {
      supported = L7_TRUE;
    }
    break;

  case BROAD_POLICY_STAGE_INGRESS:
    supported = L7_TRUE;
    break;

  case BROAD_POLICY_STAGE_EGRESS:
    // PTin added: new switch => SOC_IS_VALKYRIE2
    if (SOC_IS_FIREBOLT2(unit) ||
        SOC_IS_TR_VL(unit)     ||
        SOC_IS_SCORPION(unit)  || 
        SOC_IS_TRIUMPH2(unit)  || 
        SOC_IS_APOLLO(unit)    ||
        SOC_IS_ENDURO(unit)    ||
        SOC_IS_VALKYRIE2(unit) ||
        SOC_IS_TRIDENT(unit))       /* PTin added: new switch BCM56843 */
    {
      supported = L7_TRUE;
    }
    break;

  default: 
    break;
  }

  return supported;
}

int policy_group_init(int unit)
{
    int                  rv;
    int                  total_slices;
    BROAD_POLICY_STAGE_t policyStage;

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("policy_group_init(%d)\n", unit);

    for (policyStage = BROAD_POLICY_STAGE_LOOKUP; policyStage < BROAD_POLICY_STAGE_COUNT; policyStage++)
    {
      total_slices = _policy_group_total_slices(unit, policyStage);

      rv = _policy_group_alloc_init(unit, policyStage, total_slices);
      if (BCM_E_NONE != rv)
      {
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
            sysapiPrintf("_policy_group_alloc_init failed (%d)\n", rv);
        return rv;
      }

      rv = _policy_group_table_init(unit, policyStage, total_slices);
      if (BCM_E_NONE != rv)
      {
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
            sysapiPrintf("_policy_group_table_init failed (%d)\n", rv);
        return rv;
      }
    }

    rv = _policy_udf_init(unit);
    if (BCM_E_NONE != rv)
    {
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          sysapiPrintf("_policy_udf_init failed (%d)\n", rv);
      return rv;
    }

    rv = _policy_super_qset_init(unit);
    if (BCM_E_NONE != rv)
    {
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          sysapiPrintf("_policy_super_qset_init failed (%d)\n", rv);
      return rv;
    }

    rv = _policy_action_map_init(unit);
    if (BCM_E_NONE != rv)
    {
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          sysapiPrintf("_policy_action_map_init failed (%d)\n", rv);
      return rv;
    }

    return rv;
}

int policy_group_create(int                             unit, 
                        BROAD_POLICY_ENTRY_t           *policyData, 
                        BROAD_GROUP_t                  *group,
                        policy_resource_requirements_t *resourceReq)
{
  int                  rv;
  L7_BOOL              reworkQset;
  bcm_field_qualify_t  reworkQualifier = bcmFieldQualifyIpType;

  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
    sysapiPrintf("- create policy\n");

  rv = _policy_group_find_group(unit, policyData, resourceReq, group);

  if (BCM_E_NONE != rv)
  {
    /* see if there is an alternative qset already created */
    reworkQset = L7_FALSE;
    if ((resourceReq->requiresEtype == 0)  && 
        BCM_FIELD_QSET_TEST(resourceReq->qsetAgg, bcmFieldQualifyIpType))
    {
      reworkQset      = L7_TRUE;
      reworkQualifier = bcmFieldQualifyIpType;
    }
    else if ((resourceReq->requiresEtype == 0)  && 
             BCM_FIELD_QSET_TEST(resourceReq->qsetAgg, bcmFieldQualifyIp4))
    {
      reworkQset      = L7_TRUE;
      reworkQualifier = bcmFieldQualifyIp4;
    }
    else if ((resourceReq->requiresEtype == 0)  && 
             BCM_FIELD_QSET_TEST(resourceReq->qsetAgg, bcmFieldQualifyIp6))
    {
      reworkQset      = L7_TRUE;
      reworkQualifier = bcmFieldQualifyIp6;
    }

    if (reworkQset == L7_TRUE)
    {
      BCM_FIELD_QSET_REMOVE(resourceReq->qsetAgg, reworkQualifier);
      BCM_FIELD_QSET_ADD(resourceReq->qsetAgg, bcmFieldQualifyEtherType);
      rv = _policy_group_find_group(unit, policyData, resourceReq , group);

      if (BCM_E_NONE != rv)
      {
        BCM_FIELD_QSET_ADD(resourceReq->qsetAgg, reworkQualifier);
        BCM_FIELD_QSET_REMOVE(resourceReq->qsetAgg, bcmFieldQualifyEtherType);
      }
    }
  }

  if (BCM_E_NONE != rv)
  {
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
      sysapiPrintf("- allocate new group\n");

    rv = _policy_group_alloc_group(unit, policyData, resourceReq, group);

    if (BCM_E_NONE != rv)
    {
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
        sysapiPrintf("- allocate new group FAILED\n");

      reworkQset = L7_FALSE;
      if ((resourceReq->requiresEtype == 0)  && 
          BCM_FIELD_QSET_TEST(resourceReq->qsetAgg, bcmFieldQualifyIpType))
      {
        reworkQset      = L7_TRUE;
        reworkQualifier = bcmFieldQualifyIpType;
      }
      else if ((resourceReq->requiresEtype == 0)  && 
               BCM_FIELD_QSET_TEST(resourceReq->qsetAgg, bcmFieldQualifyIp4))
      {
        reworkQset      = L7_TRUE;
        reworkQualifier = bcmFieldQualifyIp4;
      }
      else if ((resourceReq->requiresEtype == 0)  && 
               BCM_FIELD_QSET_TEST(resourceReq->qsetAgg, bcmFieldQualifyIp6))
      {
        reworkQset      = L7_TRUE;
        reworkQualifier = bcmFieldQualifyIp6;
      }

      if (reworkQset == L7_TRUE)
      {
        /*
         * attempt to rework the qset:
         * ethertype was not required due to an ethertype of 0800 or 86dd,
         * but let's try etherType in case there exists a group with this setting
         * remove the bcmFieldQualifyIpType from the qset and search again
         */
        BCM_FIELD_QSET_REMOVE(resourceReq->qsetAgg, reworkQualifier);
        BCM_FIELD_QSET_ADD(resourceReq->qsetAgg, bcmFieldQualifyEtherType);

        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
          sysapiPrintf("- allocate new group\n");

        rv = _policy_group_alloc_group(unit, policyData, resourceReq, group);

        if (BCM_E_NONE != rv)
        {
          BCM_FIELD_QSET_ADD(resourceReq->qsetAgg, reworkQualifier);
          BCM_FIELD_QSET_REMOVE(resourceReq->qsetAgg, bcmFieldQualifyEtherType);

          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
            sysapiPrintf("- allocate new group FAILED\n");
        }
      }
    }
  }

  if (rv == BCM_E_NONE)
  {
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
      sysapiPrintf("- using group %d\n", *group);
  }

  return rv;
}

int policy_group_add_rule(int                        unit,
                          BROAD_POLICY_STAGE_t       policyStage,
                          BROAD_POLICY_TYPE_t        policyType,
                          BROAD_GROUP_t              group,
                          BROAD_POLICY_RULE_ENTRY_t *rulePtr,
                          bcm_pbmp_t                 pbm,
                          int                        skip_actions,
                          BROAD_ENTRY_t             *entry,
                          L7_int                    *policer_id)      /* PTin modified: policer */
{
    BROAD_POLICY_FIELD_t f;
    group_table_t       *groupPtr;
    bcm_field_entry_t    eid;
    int                  rv = BCM_E_NONE;
    BROAD_ACTION_ENTRY_t *actionPtr;

    CHECK_GROUP(unit, policyStage, group);

    groupPtr = &group_table[unit][policyStage][group];

    if (!(groupPtr->flags & GROUP_USED))
        return BCM_E_NOT_FOUND;

    rv = bcm_field_entry_create(unit, groupPtr->gid, &eid);

    /* PTin added: FFP */
    #if 1
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
        sysapiPrintf("- bcm_field_entry_create rv = %d (entry=%d)\n", rv, eid);
    #endif

    if (BCM_E_NONE != rv)
    {
      /* PTin removed: FFP */
      #if 0
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
            sysapiPrintf("- bcm_field_entry_create rv = %d\n", rv);
      #endif
      return rv;
    }

    /* PTin added: FFP */
    #if 1
    groupPtr->count_rules++;
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
        sysapiPrintf("Incremented count_rules to %u (unit=%d, policyStage=%d, group=%d)\n",groupPtr->count_rules,unit,policyStage,group);
    #endif

    *entry = BCM_ENTRY_TO_BROAD_ENTRY(eid);

    /* Clear out the shared UDF fields for this rule (if applicable). */
    _policy_group_rule_udf_fields_clear();

    /* Add all the fields */
    for (f = 0; f < BROAD_FIELD_LAST; f++)
    {
        /* Only add specified fields with the exception of ETHTYPE
         * which is handled specially */
        if (hapiBroadPolicyFieldFlagsGet(&rulePtr->fieldInfo, f) == BROAD_FIELD_SPECIFIED)
        {
          rv = _policy_group_add_field(unit, policyStage, groupPtr, eid, f,
                                       (char*)hapiBroadPolicyFieldValuePtr(&rulePtr->fieldInfo, f),
                                       (char*)hapiBroadPolicyFieldMaskPtr(&rulePtr->fieldInfo, f));
          if (BCM_E_NONE != rv)     
          {
             if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
               sysapiPrintf("- _policy_group_add_field f=%d rv = %d\n", f, rv);
             return rv;
          }
        }
    }

    if (!skip_actions)
    {
      /* Add all the actions */
      actionPtr = &rulePtr->actionInfo;
      rv = _policy_group_add_actions(unit, policyStage, eid, actionPtr);
      if (BCM_E_NONE != rv)
          return rv;
  
      /* add meters or counters, if any, but not both */
      if (rulePtr->ruleFlags & BROAD_METER_SPECIFIED)
      {
          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
              sysapiPrintf("- adding a meter\n");
  
          /* Ptin modified: policer */
          rv = _policy_group_add_meter(unit, policyStage, eid, rulePtr);                    /* Original */
          //rv = _policy_group_add_policer(unit, policyStage, eid, rulePtr, policer_id);    /* New */
          if (BCM_E_NONE != rv)
              return rv;
      }
      else if (rulePtr->ruleFlags & BROAD_COUNTER_SPECIFIED)
      {
          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
              sysapiPrintf("- adding a counter\n");
  
          rv = _policy_group_add_counter(unit, eid, rulePtr);
          if (BCM_E_NONE != rv)
              return rv;
      }
    }

    /* Only install to HW if we expect any ports to match this rule. This is mostly because
       for LOOKUP and EGRESS policies, the portClass doesn't really provide a mechanism to
       never match rule (as a NULL pbmp would do for ingress) */
    if ((policyStage == BROAD_POLICY_STAGE_INGRESS) || (policyType == BROAD_POLICY_TYPE_VLAN) || (BCM_PBMP_NOT_NULL(pbm)))
    {
      rv = bcm_field_entry_install(unit, eid);

      /* PTin added: FFP */
      #if 1
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
          sysapiPrintf("- bcm_field_entry_install rv = %d (entry=%d)\n", rv, eid);
      #else
      if ( rv != BCM_E_NONE)
      {  
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
            sysapiPrintf("- bcm_field_entry_install rv = %d (entry=%d)\n", rv, eid);
      }
      #endif
    }

    return rv;
}

int policy_group_set_pbm(int                  unit,
                         BROAD_POLICY_STAGE_t policyStage,
                         BROAD_GROUP_t        group,
                         BROAD_ENTRY_t        entry,
                         bcm_pbmp_t           pbm)
{
    int               rv;
    group_table_t    *groupPtr;
    bcm_field_entry_t eid;
    bcm_pbmp_t        maskPbm;

    BCM_PBMP_CLEAR(maskPbm);

    CHECK_GROUP(unit,policyStage,group);

    groupPtr = &group_table[unit][policyStage][group];

    /* The following policy types are active on all ports and don't need to use the pbm. */
    if ((BROAD_POLICY_TYPE_VLAN == groupPtr->type) || (BROAD_POLICY_TYPE_ISCSI == groupPtr->type))
    {
        return BCM_E_NONE;
    }

    eid = BROAD_ENTRY_TO_BCM_ENTRY(entry);

    /* Reinstall only works for actions, so remove and install the entry explicitly. */
    rv = bcm_field_entry_remove(unit, eid);

    /* PTin added: FFP */
    #if 1
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
        sysapiPrintf("- bcm_field_entry_remove rv = %d (entry=%d)\n", rv, eid);
    #endif

    if (BCM_E_NONE != rv)
        return rv;

    BCM_PBMP_OR(maskPbm, PBMP_PORT_ALL(unit));
    BCM_PBMP_OR(maskPbm, PBMP_LB(unit));
    rv = bcm_field_qualify_InPorts(unit, eid, pbm, maskPbm);
    if (BCM_E_NONE != rv)
        return rv;

    rv = bcm_field_entry_install(unit, eid);

    /* PTin added: FFP */
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
        sysapiPrintf("- bcm_field_entry_install rv = %d (entry=%d)\n", rv, eid);

    if (BCM_E_NONE != rv)
        return rv;

    return rv;
}

int policy_group_set_portclass(int                  unit,
                               BROAD_POLICY_STAGE_t policyStage,
                               BROAD_GROUP_t        group,
                               BROAD_ENTRY_t        entry,
                               bcm_pbmp_t           pbm,
                               unsigned char        portClass)
{
    int               rv;
    group_table_t    *groupPtr;
    bcm_field_entry_t eid;
    unsigned int      portMask  = BCM_FIELD_EXACT_MATCH_MASK;
    unsigned int      portClassBmp;
    unsigned int      bcm_port;
    int               numPorts;

    CHECK_GROUP(unit,policyStage,group);

    groupPtr = &group_table[unit][policyStage][group];

    /* VLAN policies will always include all ports */
    if ((BROAD_POLICY_TYPE_VLAN == groupPtr->type) || (BROAD_POLICY_TYPE_ISCSI == groupPtr->type))
    {
        return BCM_E_NONE;
    }

    eid = BROAD_ENTRY_TO_BCM_ENTRY(entry);

    /* Reinstall only works for actions, so remove and install the entry explicitly. */
    rv = bcm_field_entry_remove(unit, eid);

    /* PTin added: FFP */
    #if 1
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
        sysapiPrintf("- bcm_field_entry_remove rv = %d (entry=%d)\n", rv, eid);
    #endif

    if (BCM_E_NONE != rv)
        return rv;

    /* Only install to HW if we expect any ports to match this rule. This is mostly because
       for LOOKUP and EGRESS policies, the portClass doesn't really provide a mechanism to
       never match rule (as a NULL pbmp would do for ingress) */
    if (BCM_PBMP_NOT_NULL(pbm))
    {
      /* Don't burn a portClassId if only one port uses this rule */
      BCM_PBMP_COUNT(pbm, numPorts);
      if (numPorts == 1)
      {
        BCM_PBMP_ITER(pbm, bcm_port)
        {
          if (policyStage == BROAD_POLICY_STAGE_LOOKUP)
          {
            rv = bcm_field_qualify_InPort(unit, eid, bcm_port, portMask);
            if (BCM_E_NONE != rv)
                return rv;
          }
          else if (policyStage == BROAD_POLICY_STAGE_EGRESS)
          {
            rv = bcm_field_qualify_OutPort(unit, eid, bcm_port, portMask);
            if (BCM_E_NONE != rv)
                return rv;
          }
          break;
        }

        rv = bcm_field_qualify_PortClass(unit, eid, 0, 0);
        if (BCM_E_NONE != rv)
            return rv;
      }
      else
      {
        if (policyStage == BROAD_POLICY_STAGE_LOOKUP)
        {
          rv = bcm_field_qualify_InPort(unit, eid, 0, 0);
          if (BCM_E_NONE != rv)
              return rv;
        }
        else if (policyStage == BROAD_POLICY_STAGE_EGRESS)
        {
          rv = bcm_field_qualify_OutPort(unit, eid, 0, 0);
          if (BCM_E_NONE != rv)
              return rv;
        }
        portClassBmp = 1 << portClass;
        rv = bcm_field_qualify_PortClass(unit, eid, portClassBmp, portClassBmp);
        if (BCM_E_NONE != rv)
            return rv;
      }

      rv = bcm_field_entry_install(unit, eid);

      /* PTin added: FFP */
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
        sysapiPrintf("- bcm_field_entry_install rv = %d (entry=%d)\n", rv, eid);

      if (BCM_E_NONE != rv)
          return rv;
    }

    return rv;
}

int policy_port_class_add_remove(int                  unit,
                                 bcm_port_t           port,
                                 BROAD_POLICY_STAGE_t policyStage,
                                 unsigned char        portClass,
                                 L7_BOOL              add)
{
    int               rv = BCM_E_NONE;
    L7_uint32         portClassBmp;
    bcm_port_class_t  portClassType;

    if (policyStage == BROAD_POLICY_STAGE_LOOKUP)
    {
      portClassType = bcmPortClassFieldLookup;
    }
    else if (policyStage == BROAD_POLICY_STAGE_EGRESS)
    {
      portClassType = bcmPortClassFieldEgress;
    }
    else
    {
      return BCM_E_PARAM;
    }

    if (portClass != BROAD_INVALID_PORT_CLASS)
    {
      rv = bcm_port_class_get(unit, port, portClassType, &portClassBmp);
      if (BCM_E_NONE != rv)
          return rv;
  
      if (add)
      {
        portClassBmp |= (1 << portClass);
      }
      else
      {
        portClassBmp &= ~(1 << portClass);
      }
  
      rv = bcm_port_class_set(unit, port, portClassType, portClassBmp);
    }

    return rv;
}
int policy_group_set_outervlan(int                  unit,
                               policy_map_table_t  *policyPtr,
                               L7_uint32            ruleIdx,
                               char                *value,
                               char                *mask)
{
    int               rv;
    group_table_t    *groupPtr;
    bcm_field_entry_t eid;
    bcm_vlan_t  tempVlan = 0, tempVlanMask = 0;
    BROAD_POLICY_STAGE_t policyStage;
    BROAD_GROUP_t        group;
    BROAD_ENTRY_t        entry;
    BROAD_POLICY_TYPE_t  policyType;

    policyStage = policyPtr->policyStage;
    group       = policyPtr->group;
    entry       = policyPtr->entry[ruleIdx];
    policyType  = policyPtr->policyType;

    CHECK_GROUP(unit,policyStage,group);

    groupPtr = &group_table[unit][policyStage][group];

    eid = BROAD_ENTRY_TO_BCM_ENTRY(entry);
    
    tempVlan = tempVlanMask = 0;
    memcpy(&tempVlan, value, sizeof(bcm_vlan_t));
    memcpy(&tempVlanMask, mask, sizeof(bcm_vlan_t));

    /* Reinstall only works for actions, so remove and install the entry explicitly. */
    rv = bcm_field_entry_remove(unit, eid);

    /* PTin added: FFP */
    #if 1
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
        sysapiPrintf("- bcm_field_entry_remove rv = %d (entry=%d)\n", rv, eid);
    #endif

    if (BCM_E_NONE != rv)
    {
        return rv;
    }

    rv = bcm_field_qualify_OuterVlan(unit, eid, tempVlan, tempVlanMask);
    if (BCM_E_NONE != rv)
    {
        return rv;
     }

    /* Only install to HW for VLAN policies or Ingress policies or we expect any ports 
       to match this rule. This is mostly because for LOOKUP and EGRESS policies,
       the portClass doesn't really provide a mechanism to never match rule 
       (as a NULL pbmp would do for ingress) */
    if ((policyType == BROAD_POLICY_TYPE_VLAN) ||
        (policyStage == BROAD_POLICY_STAGE_INGRESS) ||
        (((policyStage == BROAD_POLICY_STAGE_LOOKUP) || (policyStage == BROAD_POLICY_STAGE_EGRESS)) && BCM_PBMP_NOT_NULL(policyPtr->pbm)))
    {
      rv = bcm_field_entry_install(unit, eid);

      /* PTin added: FFP */
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
        sysapiPrintf("- bcm_field_entry_install rv = %d (entry=%d)\n", rv, eid);

      if (BCM_E_NONE != rv)
      {
          return rv;
      }
    }

    return rv;
}
int policy_port_class_pbmp_update(int                  unit,
                                  bcm_pbmp_t           pbm,
                                  BROAD_POLICY_STAGE_t policyStage,
                                  unsigned char        oldPortClass,
                                  unsigned char        newPortClass)
{
  int       rv = BCM_E_NONE;
  L7_uint32 port;

  BCM_PBMP_ITER(pbm, port)
  {
    /* Remove the old port class first. */
    rv = policy_port_class_add_remove(unit, port, policyStage, oldPortClass, L7_FALSE);
    if (BCM_E_NONE != rv)
    {
      break;
    }
    /* Set the new port class. */
    rv = policy_port_class_add_remove(unit, port, policyStage, newPortClass, L7_TRUE);
    if (BCM_E_NONE != rv)
    {
      break;
    }
  }

  return rv;
}

int policy_group_rule_priority_set(int                          unit,
                                   BROAD_POLICY_STAGE_t         policyStage,
                                   BROAD_GROUP_t                group,
                                   BROAD_ENTRY_t                entry,
                                   BROAD_POLICY_RULE_PRIORITY_t priority)
{
    int               rv;
    group_table_t    *groupPtr;
    bcm_field_entry_t eid;
    int prio;

    CHECK_GROUP(unit,policyStage,group);

    groupPtr = &group_table[unit][policyStage][group];

    if (!(groupPtr->flags & GROUP_USED))
        return BCM_E_NOT_FOUND;

    eid = BROAD_ENTRY_TO_BCM_ENTRY(entry);

    switch(priority)
    {
    case BROAD_POLICY_RULE_PRIORITY_LOWEST:   prio = BCM_FIELD_ENTRY_PRIO_LOWEST;
                                              break;
    case BROAD_POLICY_RULE_PRIORITY_HIGHEST:  prio = BCM_FIELD_ENTRY_PRIO_HIGHEST;
                                              break;
    default:                                  prio = ((BCM_FIELD_ENTRY_PRIO_HIGHEST - BCM_FIELD_ENTRY_PRIO_LOWEST)/2)+BCM_FIELD_ENTRY_PRIO_LOWEST;
    }

    rv = bcm_field_entry_prio_set(unit, eid, prio);

    if (rv != BCM_E_NONE)
    {
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          sysapiPrintf("- bcm_field_entry_prio_set rv = %d\n", rv);
    }
    return rv;
}

int policy_group_delete_rule(int                  unit,
                             BROAD_POLICY_STAGE_t policyStage,
                             BROAD_GROUP_t        group,
                             BROAD_ENTRY_t        entry,
                             L7_int               policer_id)       /* PTin modified: policer */
{
    int               rv;
    group_table_t    *groupPtr;
    bcm_field_entry_t eid;

    CHECK_GROUP(unit,policyStage,group);

    groupPtr = &group_table[unit][policyStage][group];

    if (!(groupPtr->flags & GROUP_USED))
        return BCM_E_NOT_FOUND;

    eid = BROAD_ENTRY_TO_BCM_ENTRY(entry);

    rv = bcm_field_entry_remove(unit, eid);

    /* PTin added: FFP */
    #if 1
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
        sysapiPrintf("- bcm_field_entry_remove rv = %d (entry=%d)\n", rv, eid);
    #endif

    /* If rv is BCM_E_UNAVAIL, it is possible that we are trying to remove
     * a rule that is not installed in the hardware. So, continue with the
     * destroy. */ 
    if((BCM_E_UNAVAIL != rv) && (BCM_E_NONE != rv))
        return rv;

    /* PTin added: policer */
    if (policer_id>0)
    {
      rv = bcm_field_entry_policer_detach(unit,eid,0);
      if (BCM_E_NONE != rv)
          return rv;
      rv = bcm_policer_destroy(unit,policer_id);
      if (BCM_E_NONE != rv)
          return rv;
    }
    /* PTin end */

    rv = bcm_field_entry_destroy(unit, eid);

    /* PTin added: FFP */
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
        sysapiPrintf("- bcm_field_entry_destroy rv = %d (entry=%d)\n", rv, eid);

    if (BCM_E_NONE != rv)
        return rv;

    /* PTin added: FFP */
    #if 1
    if (groupPtr->count_rules>0)  groupPtr->count_rules--;
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
        sysapiPrintf("Decremented count_rules to %u (unit=%d, policyStage=%d, group=%d)\n",groupPtr->count_rules,unit,policyStage,group);
    #endif

    return rv;
}

int policy_group_destroy(int unit, BROAD_POLICY_STAGE_t policyStage, BROAD_GROUP_t group)
{
    int                      rv;
    group_table_t           *groupPtr;

    CHECK_GROUP(unit,policyStage,group);

    groupPtr = &group_table[unit][policyStage][group];

    if (!(groupPtr->flags & GROUP_USED))
        return BCM_E_NOT_FOUND;

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
        sysapiPrintf("- destroy group %d, gid %d\n", group, groupPtr->gid);

    rv = bcm_field_group_destroy(unit, groupPtr->gid);
    if (BCM_E_NONE != rv)
        return rv;

    rv = _policy_group_delete_group(unit, policyStage, group);
    if (BCM_E_NONE != rv)
        return rv;

    return BCM_E_NONE;
}

int policy_group_get_stats(int                  unit,
                           BROAD_POLICY_STAGE_t policyStage,
                           BROAD_GROUP_t        group,
                           BROAD_ENTRY_t        entry,
                           uint64              *val1,
                           uint64              *val2)
{
    int                rv;
    group_table_t     *groupPtr;
    bcm_field_entry_t  eid;

    CHECK_GROUP(unit,policyStage,group);

    COMPILER_64_ZERO(*val1);
    COMPILER_64_ZERO(*val2);

    groupPtr = &group_table[unit][policyStage][group];

    if (!(groupPtr->flags & GROUP_USED))
        return BCM_E_NOT_FOUND;

    eid = BROAD_ENTRY_TO_BCM_ENTRY(entry);

    if (policyStage == BROAD_POLICY_STAGE_EGRESS)
    {
      rv = bcm_field_counter_get(unit, eid, 0, val2);
      if ((BCM_E_NONE != rv) && (BCM_E_EMPTY != rv))   /* empty means no counter */
          return rv;
    }
    else
    {
      rv = bcm_field_counter_get(unit, eid, 0, val1);
      if ((BCM_E_NONE != rv) && (BCM_E_EMPTY != rv))   /* empty means no counter */
          return rv;

      rv = bcm_field_counter_get(unit, eid, 1, val2);  /* empty means no counter */
      if ((BCM_E_NONE != rv) && (BCM_E_EMPTY != rv))
          return rv;
    }

    return BCM_E_NONE;
}

int policy_group_stats_clear(int                  unit,
                             BROAD_POLICY_STAGE_t policyStage,
                             BROAD_ENTRY_t        entry)
{
    int                rv;
    bcm_field_entry_t  eid;
    uint64             val1;

    COMPILER_64_ZERO(val1);

    eid = BROAD_ENTRY_TO_BCM_ENTRY(entry);

    rv = bcm_field_counter_set(unit, eid, 0, val1);
    if ((BCM_E_NONE != rv) && (BCM_E_EMPTY != rv))   /* empty means no counter */
        return rv;

    if (policyStage != BROAD_POLICY_STAGE_EGRESS)
    {
      rv = bcm_field_counter_set(unit, eid, 1, val1);  /* empty means no counter */
      if ((BCM_E_NONE != rv) && (BCM_E_EMPTY != rv))
          return rv;
    }

    return BCM_E_NONE;
}

void policy_group_dataplane_cleanup(int                  unit,
                                    BROAD_ENTRY_t        entry,
                                    L7_uint32            cpu_modid,
                                    L7_uint32            cpu_modport)
{
    int                rv;
    bcm_field_entry_t  eid;

    eid = BROAD_ENTRY_TO_BCM_ENTRY(entry);

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_NONE)
        sysapiPrintf("- new manager notify entry %d, modid %d, modport %d\n", eid, cpu_modid, cpu_modport);

    rv = bcm_field_entry_remove(unit, eid);

    /* PTin added: FFP */
    #if 1
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
        sysapiPrintf("- bcm_field_entry_remove rv = %d (entry=%d)\n", rv, eid);
    #endif

    if (BCM_E_NONE != rv)
    {
      /* PTin removed: FFP */
      #if 0
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_NONE)
          sysapiPrintf("- entry remove rv = %d\n", rv);
      #endif
      return;
    }

    rv = bcm_field_action_remove(unit, eid, bcmFieldActionRedirect);
    if (BCM_E_NONE != rv)
    {
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_NONE)
          sysapiPrintf("- action remove rv = %d\n", rv);
      return;
    }

    rv = bcm_field_action_add(unit, eid, bcmFieldActionRedirect, cpu_modid, cpu_modport);
    if (BCM_E_NONE != rv)
    {
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_NONE)
          sysapiPrintf("- action add rv = %d\n", rv);
      return;
    }

    rv = bcm_field_entry_install(unit, eid);

    /* PTin added: FFP */
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
      sysapiPrintf("- bcm_field_entry_install rv = %d (entry=%d)\n", rv, eid);

    if (BCM_E_NONE != rv)
    {
      /* PTin removed: FFP */
      #if 0
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_NONE)
          sysapiPrintf("- entry install rv = %d\n", rv);
      #endif
      return;
    }

    return;
}

int policy_group_set_epbm(int unit,
                          BROAD_GROUP_t group,
                          BROAD_ENTRY_t entry,
                          bcm_pbmp_t pbmp,
                          BROAD_POLICY_EPBM_CMD_t epbmCmd)
{
    int               rv;
    group_table_t    *groupPtr;
    bcm_field_entry_t eid;
    uint32            param0, param1;

    CHECK_GROUP(unit,BROAD_POLICY_STAGE_INGRESS,group);

    groupPtr = &group_table[unit][BROAD_POLICY_STAGE_INGRESS][group];

    eid = BROAD_ENTRY_TO_BCM_ENTRY(entry);

    /* Get the current epbm value */
    rv = bcm_field_action_get(unit, eid, bcmFieldActionEgressMask, &param0, &param1);
    if (rv == BCM_E_NONE)
    {
      rv = bcm_field_action_remove(unit, eid, bcmFieldActionEgressMask);  
      if (rv != BCM_E_NONE) 
      {
            return rv; 
      }
    }
    else if (rv == BCM_E_NOT_FOUND)
    {
      /* egress mask action hasn't been added yet. */
      param0 = 0;
    }
    else
    {
      return rv;
    }

    if (epbmCmd == BROAD_POLICY_EPBM_CMD_ASSIGN) 
    {
        param0 = SOC_PBMP_WORD_GET(pbmp, 0);
    }
    else if (epbmCmd == BROAD_POLICY_EPBM_CMD_ADD) 
    {
        param0 |= (SOC_PBMP_WORD_GET(pbmp, 0));
    }
    else if (epbmCmd == BROAD_POLICY_EPBM_CMD_REMOVE) 
    {
        param0 &= (~(SOC_PBMP_WORD_GET(pbmp, 0)));
    }
    else
    {
        return BCM_E_FAIL;
    }

    rv = bcm_field_action_add(unit, eid, bcmFieldActionEgressMask, param0, 0);
    if (rv != BCM_E_NONE) 
    {
        return rv; 
    }

    rv = bcm_field_entry_reinstall(unit, eid);

    return rv;
}

int policy_group_create_default_rule(int unit,
                                     BROAD_GROUP_t group,
                                     bcm_pbmp_t epbm,
                                     BROAD_ENTRY_t *entry
                                     )
{
    group_table_t       *groupPtr;
    bcm_field_entry_t    eid;
    int                  rv = BCM_E_NONE;

    CHECK_GROUP(unit,BROAD_POLICY_STAGE_INGRESS,group);

    groupPtr = &group_table[unit][BROAD_POLICY_STAGE_INGRESS][group];

    if (!(groupPtr->flags & GROUP_USED))
      return BCM_E_NOT_FOUND;

    rv = bcm_field_entry_create(unit, groupPtr->gid, &eid);

    /* PTin added: FFP */
    #if 1
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
        sysapiPrintf("- bcm_field_entry_create rv = %d (entry=%d)\n", rv, eid);
    #endif

    if (BCM_E_NONE != rv)
      return rv;

    /* PTin added: FFP */
    #if 1
    groupPtr->count_rules++;
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
        sysapiPrintf("Incremented count_rules to %u (unit=%d, policyStage=%d, group=%d)\n",groupPtr->count_rules,unit,BROAD_POLICY_STAGE_INGRESS,group);
    #endif

    *entry = BCM_ENTRY_TO_BROAD_ENTRY(eid);

    rv = bcm_field_qualify_PacketFormat(unit, eid, BCM_FIELD_PKT_FMT_IPV4,
                                        BCM_FIELD_PKT_FMT_IPV4);
    if (BCM_E_NONE != rv)
      return rv;

    rv = bcm_field_action_add(unit, eid, bcmFieldActionEgressMask, 
                              SOC_PBMP_WORD_GET(epbm, 0), 0
                              );
    if (BCM_E_NONE != rv)
      return rv;

    rv = bcm_field_entry_install(unit, eid);

    /* PTin added: FFP */
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
      sysapiPrintf("- bcm_field_entry_install rv = %d (entry=%d)\n", rv, eid);

    if (BCM_E_NONE != rv)
      return rv;

    rv = bcm_field_entry_prio_set(unit, eid, BCM_FIELD_ENTRY_PRIO_LOWEST);

    return rv;


}

int _policy_group_block_low_prio_get(int unit, BROAD_POLICY_STAGE_t policyStage, int block)
{
  return group_alloc_table[unit][policyStage][block].lowPrio;
}

int _policy_group_block_high_prio_get(int unit, BROAD_POLICY_STAGE_t policyStage, int block)
{
  return group_alloc_table[unit][policyStage][block].highPrio;
}

void _policy_sqset_get(int unit, int sqset, super_qset_entry_t *sqsetInfo)
{
  if ((unit < SOC_MAX_NUM_DEVICES) && (sqset < SUPER_QSET_TABLE_SIZE))
  {
    memcpy(sqsetInfo, &(super_qset_table[unit][sqset]), sizeof(*sqsetInfo));
  }
  else
  {
    memset(sqsetInfo, 0, sizeof(*sqsetInfo));
  }
}

void _policy_group_info_get(int unit, BROAD_POLICY_STAGE_t policyStage, BROAD_GROUP_t group, group_table_t *groupInfo)
{
  memset(groupInfo, 0, sizeof(*groupInfo));

  if ((unit < SOC_MAX_NUM_DEVICES) && (policyStage < BROAD_POLICY_STAGE_COUNT) && (group < group_table_size[unit][policyStage]))
  {
    memcpy(groupInfo, &group_table[unit][policyStage][group], sizeof(*groupInfo));
  }
}

int _policy_minimal_sqset_get(int unit, BROAD_POLICY_TYPE_t policyType, policy_resource_requirements_t *resourceReq, int *sqset)
{
  int          rv;
  sqsetWidth_t minQsetWidth;

  /* Find the minimal sqset for this policy. */
  for (minQsetWidth = sqsetWidthFirst; minQsetWidth < sqsetWidthLast; minQsetWidth++)
  {
    rv = _policy_super_qset_find_match(unit, 
                                       policyType, 
                                       minQsetWidth, 
                                       resourceReq->qsetAgg, 
                                       resourceReq->customQset, 
                                       sqset);
    if (rv == BCM_E_NONE)
    {
      /* We found a match. */
      break;
    }
  }

  return rv;
}

void _policy_group_status_to_sqset_width(bcm_field_group_status_t *status, sqsetWidth_t *sqsetWidth)
{
  if ((status->slice_width_physical == 1) && (status->intraslice_mode_enable == 0))
  {
    *sqsetWidth = sqsetWidthSingle;
  }
  else if ((status->slice_width_physical == 1) && (status->intraslice_mode_enable == 1))
  {
    *sqsetWidth = sqsetWidthDoubleIntraslice;
  }
  else if ((status->slice_width_physical == 2) && (status->intraslice_mode_enable == 0))
  {
    *sqsetWidth = sqsetWidthDouble;
  }
  else
  {
    *sqsetWidth = sqsetWidthQuad;
  }
}

/* Debug */
void debug_print_qset(bcm_field_qset_t *qset)
{
  /* Text names of the enumerated qualifier IDs. */
  char *qual_text[bcmFieldQualifyCount] = BCM_FIELD_QUALIFY_STRINGS;
  int   i;

  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
  {
    sysapiPrintf("- policy being created needs qset ");
    for (i = 0; i < bcmFieldQualifyCount; i++)
    {
      if (BCM_FIELD_QSET_TEST(*qset, i))
      {
        sysapiPrintf("%s, ", qual_text[i]);
      }
    }
    sysapiPrintf("\n");
  }

}

void debug_group_stats(int unit)
{
  int rv, i;
  bcm_field_group_status_t gstats;
  BROAD_POLICY_STAGE_t policyStage;

  sysapiPrintf("\nUnit %d Group Stats\n", unit);

  for (policyStage = BROAD_POLICY_STAGE_LOOKUP; policyStage < BROAD_POLICY_STAGE_COUNT; policyStage++)
  {
    if (policy_stage_supported(unit, policyStage) == L7_TRUE)
    {
      switch (policyStage)
      {
      case BROAD_POLICY_STAGE_LOOKUP:
        sysapiPrintf("Stage: Lookup\n");
        break;
      case BROAD_POLICY_STAGE_INGRESS:
        sysapiPrintf("Stage: Ingress\n");
        break;
      case BROAD_POLICY_STAGE_EGRESS:
        sysapiPrintf("Stage: Egress\n");
        break;
      default:
        sysapiPrintf("Stage: Unknown!\n");
        break;
      }
      sysapiPrintf("Group  GID     Entries(count/free/total)   Counters(c/f/t)   Meters(c/f/t)\n");
      for (i = 0; i < group_table_size[unit][policyStage]; i++)
      {
        sysapiPrintf("[%2d]   ", i);

        rv = BCM_E_NOT_FOUND;
        if (group_table[unit][policyStage][i].flags & GROUP_USED)
        {
          rv = bcm_field_group_status_get(unit, group_table[unit][policyStage][i].gid, &gstats);
          if (BCM_E_NONE == rv)
          {
            sysapiPrintf("%3d      %d/%d/%d    %d/%d/%d    %d/%d/%d",
                   group_table[unit][policyStage][i].gid,
                   gstats.entry_count,   gstats.entries_free,  gstats.entries_total,
                   gstats.counter_count, gstats.counters_free, gstats.counters_total,
                   gstats.meter_count,   gstats.meters_free,   gstats.meters_total);
          }
        }

        if (rv != BCM_E_NONE)
        {
          sysapiPrintf("unused");
        }
        sysapiPrintf("\n");
      }
    }
  }
}

void debug_group_table(int unit)
{
    int                  i;
    BROAD_POLICY_STAGE_t policyStage;

    sysapiPrintf("\nUnit %d\n", unit);

    for (policyStage = BROAD_POLICY_STAGE_LOOKUP; policyStage < BROAD_POLICY_STAGE_COUNT; policyStage++)
    {
      if (policy_stage_supported(unit, policyStage) == L7_TRUE)
      {
        switch (policyStage)
        {
        case BROAD_POLICY_STAGE_LOOKUP:
          sysapiPrintf("Stage: Lookup\n");
          break;
        case BROAD_POLICY_STAGE_INGRESS:
          sysapiPrintf("Stage: Ingress\n");
          break;
        case BROAD_POLICY_STAGE_EGRESS:
          sysapiPrintf("Stage: Egress\n");
          break;
        default:
          sysapiPrintf("Stage: Unknown!\n");
          break;
        }
        sysapiPrintf("Group   GID    Type          SQset   Width       RuleDepth\n");
        for (i = 0; i < group_table_size[unit][policyStage]; i++)
        {
            sysapiPrintf("[%2d]   ", i);

            if (group_table[unit][policyStage][i].flags & GROUP_USED)
            {
                sysapiPrintf("%4d    %11s    %2d      ", 
                       group_table[unit][policyStage][i].gid, 
                       hapiBroadPolicyTypeName(group_table[unit][policyStage][i].type),
                       group_table[unit][policyStage][i].sqset);
                sysapiPrintf("%d %s", 
                       super_qset_table[unit][group_table[unit][policyStage][i].sqset].status.slice_width_physical,
                       super_qset_table[unit][group_table[unit][policyStage][i].sqset].status.intraslice_mode_enable ? "Intraslice" : "          ");
                sysapiPrintf(" %d", super_qset_table[unit][group_table[unit][policyStage][i].sqset].status.natural_depth);
            }
            else
            {
                sysapiPrintf("unused");
            }
            sysapiPrintf("\n");
        }
      }
    }
}

void debug_sqset_table(int unit, int entry)
{
  /* Text names of the enumerated qualifier IDs. */
  char *qual_text[bcmFieldQualifyCount] = BCM_FIELD_QUALIFY_STRINGS;
  int i, j;
  int min_entry, max_entry;

  if (entry == -1)
  {
    /* All entries */
    min_entry = 0;
    max_entry = SUPER_QSET_TABLE_SIZE;
  }
  else
  {
    if (entry < SUPER_QSET_TABLE_SIZE)
    {
      min_entry = entry;
      max_entry = entry + 1;
    }
    else
    {
      sysapiPrintf("Entry must be less than %d\n", SUPER_QSET_TABLE_SIZE);
      return;
    }
  }

  for (i = min_entry; i < max_entry; i++)
  {
    sysapiPrintf("SQset %d", i);

    if (super_qset_table[unit][i].flags & SUPER_QSET_USED)
    {
      sysapiPrintf("\n- qsetAgg: ");
      for (j = 0; j < bcmFieldQualifyCount; j++)
      {
        if (BCM_FIELD_QSET_TEST(super_qset_table[unit][i].qsetAgg, j))
        {
          sysapiPrintf("%s, ", qual_text[j]);
        }
      }
      sysapiPrintf("\n- qsetUdf: ");
      for (j = 0; j < bcmFieldQualifyCount; j++)
      {
        if (BCM_FIELD_QSET_TEST(super_qset_table[unit][i].qsetUdf, j))
        {
          sysapiPrintf("%s, ", qual_text[j]);
        }
      }
      sysapiPrintf("\n- applicablePolicyTypes: ");
      for (j = 0; j < BROAD_POLICY_TYPE_LAST; j++)
      {
        if (super_qset_table[unit][i].applicablePolicyTypes & (1 << j))
        {
          sysapiPrintf("%s ", hapiBroadPolicyTypeName(j));
        }
      }
      sysapiPrintf("\n- sqsetWidth: ");
      switch (super_qset_table[unit][i].sqsetWidth)
      {
      case sqsetWidthSingle:
        sysapiPrintf("Single\n");
        break;
      case sqsetWidthDoubleIntraslice:
        sysapiPrintf("Double Intraslice\n");
        break;
      case sqsetWidthDouble:
        sysapiPrintf("Double\n");
        break;
      case sqsetWidthQuad:
        sysapiPrintf("Quad\n");
        break;
      default:
        break;
      }
      sysapiPrintf("- sliceWidthPhysical: %d\n", super_qset_table[unit][i].status.slice_width_physical);
      sysapiPrintf("- ruleDepth: %d\n", super_qset_table[unit][i].status.natural_depth);
      sysapiPrintf("- intrasliceModeEnabled: %d\n", super_qset_table[unit][i].status.intraslice_mode_enable);
    }
    else
    {
      sysapiPrintf(" unused");
    }
    sysapiPrintf("\n");
  }
}

void debug_entry_counter(int unit, bcm_field_entry_t eid)
{
    int    rv;
    uint64 val64;

    rv = bcm_field_counter_get(unit, eid, 0, &val64);
    if (BCM_E_NONE == rv)
        sysapiPrintf("Counter 0: %08x %08x\n", u64_H(val64), u64_L(val64));
    else
        sysapiPrintf("error code = %d\n", rv);

    rv = bcm_field_counter_get(unit, eid, 1, &val64);
    if (BCM_E_NONE == rv)
        sysapiPrintf("Counter 1: %08x %08x\n", u64_H(val64), u64_L(val64));
    else
        sysapiPrintf("error code = %d\n", rv);
}

/* PTin added: FFP */
#if 1
group_table_t *policy_group_count_rules(int unit, int group_idx, int stage)
{
    // Validate group_id
    if (unit!=0 ||
        stage>=BROAD_POLICY_STAGE_COUNT ||
        group_idx>=_policy_group_total_slices(0,stage) ||
        group_idx>=group_table_size[0][stage])
    {
      return L7_NULLPTR;
    }

    // Return available rules
    return &group_table[0][stage][group_idx];
}
#endif

