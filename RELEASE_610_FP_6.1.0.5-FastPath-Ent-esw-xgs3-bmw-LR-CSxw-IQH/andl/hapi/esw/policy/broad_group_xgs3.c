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
#include "ibde.h"
#include "sal/core/libc.h"
#include "osapi_support.h"

/* used for the Higig B0 workaround */
#include <soc/drv.h>
#include <bcm_int/control.h>

#define MASK_NONE    (~0)
#define MASK_ALL     (0)

#define L7_BCM_DOT1AD_INVALID_UNIT (~0)
#define L7_BCM_DOT1AD_INVALID_RULE (~0)
#define L7_BCM_DOT1AD_PORT_MASK 0x1f

void debug_sqset_table(int unit);

#define CHECK_UNIT(u)    {if (u >= SOC_MAX_NUM_DEVICES) LOG_ERROR(u);}

typedef enum custom_field_qualify_e 
{
    customFieldQualifyUdf0,                  /* User defined field */
    customFieldQualifyUdf1,                  /* User defined field */
    customFieldQualifyIscsiOpcode,           /*                     */
    customFieldQualifyIscsiOpcodeTcpOptions, /* Includes 12 bytes of TCP options (typical of Linux clients) */
    customFieldQualifyIcmpMsgType,           /* ICMP Message Type */
    customFieldQualifyLast                   /* Must be last */
} custom_field_qualify_t;

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
    bcmFieldQualifyLookupClass0,    /* Class ID from VFP, to be used in IFP */
    bcmFieldQualifySrcMacGroup,     /* Class ID from L2X, to be used in IFP */
    customFieldQualifyUdf1,         /* iSCSI PDU opCode field */
    customFieldQualifyUdf1,         /* iSCSI PDU opCode field, w/ TCP options */
    bcmFieldQualifyTcpControl,
    bcmFieldQualifyVlanFormat,     /* VLAN Format */
    bcmFieldQualifyIpType,         /* IP Type */
    bcmFieldQualifyInPorts,        /* InPorts, PTin added */
    bcmFieldQualifyDrop,           /* Drop PTin added */
    bcmFieldQualifyL2StationMove,  /* L2 Station move PTin added */
    bcmFieldQualifyL2SrcHit        /* L2 Source Hit PTin added */
};

typedef struct custom_field_qset_t {
    SHR_BITDCL w[_SHR_BITDCLSIZE(customFieldQualifyLast)]; /* custom field bitmap */
} custom_field_qset_t;

#define CUSTOM_FIELD_QSET_INIT(qset) \
    sal_memset(&(qset), 0, sizeof(custom_field_qset_t))

#define CUSTOM_FIELD_QSET_ADD(qset, q)    \
    if (q < customFieldQualifyLast)       \
        {SHR_BITSET(((qset).w), (q));}

#define CUSTOM_FIELD_QSET_REMOVE(qset, q) \
    if (q < customFieldQualifyLast)       \
        {SHR_BITCLR(((qset).w), (q));}

#define CUSTOM_FIELD_QSET_TEST(qset, q)    SHR_BITGET(((qset).w), (q))

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
        { bcmFieldActionGpPrioIntNew, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
        { bcmFieldActionYpPrioIntNew, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
        { bcmFieldActionRpPrioIntNew, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
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
        { bcmFieldActionPrioPktAndIntNew, PROFILE_ACTION_NONE,       PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
        { PROFILE_ACTION_INVALID,         PROFILE_ACTION_INVALID,    PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID,         PROFILE_ACTION_INVALID,    PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
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
    /* SET_CLASS_ID */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_REASON_CODE */
    {
        { bcmFieldActionAddClassTag, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_USERPRIO_AS_INNER_DOT1P*/
    {
        { bcmFieldActionPrioPktCopy, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* PTin added: SET_COSQ_AS_PACKPRIO */
    {
        { bcmFieldActionPrioPktAndIntCopy, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* PTin added: REDIRECT_TRUNK */
    {
        { bcmFieldActionRedirectTrunk, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
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
        { bcmFieldActionVlanAdd,  PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
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
    /* PTin added: SET_COSQ_AS_PACKPRIO */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* PTin added: REDIRECT_TRUNK */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
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
    /* PTin added: SET_COSQ_AS_PACKPRIO */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* PTin added: REDIRECT_TRUNK */
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

/* User Defined Fields (UDF) */
/* One UDF is currently allocated to System Policies to allow a combination of
 * InPorts, PacketFormat and a combination of L2/3/4 packet header fields that
 * are not supported by standard FPF settings.
 */
#define BROAD_SYSTEM_UDF customFieldQualifyUdf0

/* One UDF is currently allocated to iSCSI Control Packet Policies to allow a combination of
 * InPorts, PacketFormat and a combination of L2/3/4 packet header fields that
 * are not supported by standard FPF settings.
 */
#define BROAD_ISCSI_UDF customFieldQualifyUdf1

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

static bcm_field_qualify_t l2SvtQset[] =   /* single VLAN tag */
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
static bcm_field_qualify_t l2SvtLookupStatusQset[] =   /* single VLAN tag */
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

static bcm_field_qualify_t l3l4Qset[] =    /* IPv4 six-tuple  */
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

static bcm_field_qualify_t l2l3SrcQset[] =    /* l2/3 src */
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

static bcm_field_qualify_t l2l3DstQset[] =    /* l2/3 dst */
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
static bcm_field_qualify_t l2l3l4Qset[] =    /* l2/l3/l4 */
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

static bcm_field_qualify_t l2l3l4SrcMacGroupQset[] =    /* l2/l3/l4 */
{
    bcmFieldQualifyInPorts,
    bcmFieldQualifySrcMac,
    bcmFieldQualifyDstMac,
    bcmFieldQualifySrcIp,
    bcmFieldQualifyDstIp,
    bcmFieldQualifyEtherType,
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifySrcMacGroup,
    bcmFieldQualifyL2Format,
    bcmFieldQualifyVlanFormat,
    bcmFieldQualifyIpType,
    bcmFieldQualifyIpProtocol,
    bcmFieldQualifyL4SrcPort,
    bcmFieldQualifyL4DstPort,
    bcmFieldQualifyDSCP,
    bcmFieldQualifyLookupStatus,
    bcmFieldQualifyStageIngress, 
    bcmFieldQualifyTcpControl
};

#define l2l3l4SrcMacGroupQsetSize (sizeof(l2l3l4SrcMacGroupQset) / sizeof(bcm_field_qualify_t))

/* The following sqset is used on FB2 to allow classifications on the 
   CLASS_ID determined via the LOOKUP engine. Compared to l2l3l4Qset,
   this qset requires removal of the InnerVlan. */
static bcm_field_qualify_t l2l3l4ClassIdQset[] =    /* l2/l3/l4 */
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
    bcmFieldQualifyLookupClass0,
    bcmFieldQualifySrcMacGroup,
    bcmFieldQualifyStageIngress
};

#define l2l3l4ClassIdQsetSize (sizeof(l2l3l4ClassIdQset) / sizeof(bcm_field_qualify_t))

/* The following sqset is used on Triumph to allow classifications on the
   CLASS_ID determined via the LOOKUP engine. Compared to l2l3l4ClassIdQset,
   this qset requires the InnerVlan. 
*/
static bcm_field_qualify_t l2l3l4Xgs4ClassIdQset[] =    /* l2/l3/l4 */
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
    bcmFieldQualifyLookupClass0,
    bcmFieldQualifySrcMacGroup,
    bcmFieldQualifyStageIngress
};

#define l2l3l4Xgs4ClassIdQsetSize (sizeof(l2l3l4Xgs4ClassIdQset) / sizeof(bcm_field_qualify_t))

/* VLAN + L3 super qset is used to support VLAN ACLs for L3/4 packets.
 * Note that it does not qualify on InPorts, so it will be applied to
 * all ports.
 */
static bcm_field_qualify_t vlanl3Qset[] =    /* vlan/l3 */
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
static bcm_field_qualify_t systemQsetDouble[] =  /* System requirement */
{
    bcmFieldQualifyInPorts,
    //bcmFieldQualifyOutPorts,        /* PTin added */
    bcmFieldQualifyDstMac,
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifyInnerVlan,         /* PTin added */
    bcmFieldQualifyL4SrcPort,
    bcmFieldQualifyL4DstPort,
    bcmFieldQualifyEtherType,
    bcmFieldQualifyIpProtocol,
    //bcmFieldQualifyTtl,                   /* PTin removed */
    bcmFieldQualifyL2SrcHit,                /* PTin added */
    bcmFieldQualifyL2StationMove,
    bcmFieldQualifyL3DestRouteHit,
    bcmFieldQualifyL3DestHostHit,
    //bcmFieldQualifyIngressStpState,     /* PTin removed */
    //bcmFieldQualifyTunnelTerminated,    /* PTin removed */
    bcmFieldQualifyIpType,
    //bcmFieldQualifyDstIp6,          /* PTin added */
//    bcmFieldQualifySrcIp6,          /* PTin added */
    bcmFieldQualifyDstIp,           /* PTin added */
    bcmFieldQualifyDrop,            /* PTin added */
    bcmFieldQualifyVlanFormat,     /* PTin added */
    bcmFieldQualifyStageIngress
};

#define systemQsetDoubleSize (sizeof(systemQsetDouble) / sizeof(bcm_field_qualify_t))

/* We use this qset on chips that support doublewide mode (e.g. Helix+, FB_B0, FB2, Triumph, etc.). */
static bcm_field_qualify_t systemQsetQuad[] =  /* System requirement */
{
    bcmFieldQualifyInPorts,
    //bcmFieldQualifyOutPorts,        /* PTin added */
    bcmFieldQualifyDstMac,
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifyInnerVlan,       /* PTin added */
    bcmFieldQualifyL4SrcPort,
    bcmFieldQualifyL4DstPort,
    bcmFieldQualifyEtherType,
    bcmFieldQualifyIpProtocol,
    //bcmFieldQualifyTtl,                 /* PTin removed */
    bcmFieldQualifyL2SrcHit,              /* PTin added */
    bcmFieldQualifyL2StationMove,
    bcmFieldQualifyL3DestRouteHit,
    bcmFieldQualifyL3DestHostHit,
    //bcmFieldQualifyIngressStpState,     /* PTin removed */
    //bcmFieldQualifyTunnelTerminated,    /* PTin removed */
    bcmFieldQualifyIpType,
    bcmFieldQualifyDstIp6,          /* PTin added */
    bcmFieldQualifySrcIp6,          /* PTin added */
    bcmFieldQualifyDstIp,           /* PTin added */
    bcmFieldQualifyDrop,            /* PTin added */
    bcmFieldQualifyVlanFormat,     /* PTin added */
    bcmFieldQualifyStageIngress
};

#define systemQsetQuadSize (sizeof(systemQsetQuad) / sizeof(bcm_field_qualify_t))

///* We use this qset on chips that support doublewide mode (e.g. Helix+, FB_B0, FB2, Triumph, etc.). */
//static bcm_field_qualify_t systemQsetDouble3[] =  /* System requirement */
//{
//    bcmFieldQualifyInPorts,
//    //bcmFieldQualifyOutPorts,        /* PTin added */
//    bcmFieldQualifyDstMac,
//    /*bcmFieldQualifyOuterVlan,*/
//    bcmFieldQualifyInnerVlan,       /* PTin added */
//    /*bcmFieldQualifyL4SrcPort,*/
//    /*bcmFieldQualifyL4DstPort,*/
//    bcmFieldQualifyEtherType,
//    /*bcmFieldQualifyIpProtocol,*/
//    /*bcmFieldQualifyTtl,*/         /* PTin removed */
//    /*bcmFieldQualifyL2StationMove,*/
//    /*bcmFieldQualifyL3DestRouteHit,*/
//    /*bcmFieldQualifyL3DestHostHit,*/
//    /*bcmFieldQualifyIngressStpState,*/     /* PTin removed */
//    /*bcmFieldQualifyTunnelTerminated,*/    /* PTin removed */
//    bcmFieldQualifyIpType,
//    bcmFieldQualifyDstIp6,          /* PTin added */
//    /*bcmFieldQualifySrcIp6,*/          /* PTin added */
//    /*bcmFieldQualifyDrop,*/            /* PTin added */
//    /*bcmFieldQualifyVlanFormat,*/     /* PTin added */
//    bcmFieldQualifyStageIngress
//};
//
//#define systemQsetDoubleSize3 (sizeof(systemQsetDouble3) / sizeof(bcm_field_qualify_t))


static bcm_field_qualify_t systemQset[] =  /* System requirement */
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
   bcmFieldQualifyTunnelTerminated compared to systemQsetDouble[]. */
static bcm_field_qualify_t systemQsetTriumph2[] =  /* System requirement */
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
  bcmFieldQualifyL3DestRouteHit,
  bcmFieldQualifyL3DestHostHit,
  bcmFieldQualifyIngressStpState,
  bcmFieldQualifyIpType,
  bcmFieldQualifyStageIngress
};
#define systemQsetTriumph2Size (sizeof(systemQsetTriumph2) / sizeof(bcm_field_qualify_t))

static custom_field_qualify_t systemCustomQset[] =  /* System requirement */
{
    BROAD_SYSTEM_UDF,        /* ETHTYPE, PROTO, L4DEST, Tunneled IPV6 Next Header */
};

#define systemCustomQsetSize (sizeof(systemCustomQset) / sizeof(custom_field_qualify_t))

static bcm_field_qualify_t iscsiQset[] =  /* iSCSI requirement */
{
    bcmFieldQualifyInPorts,
    bcmFieldQualifyL2StationMove,
    bcmFieldQualifyL3DestRouteHit,
    bcmFieldQualifyL3DestHostHit,
    bcmFieldQualifyIngressStpState,
    bcmFieldQualifyStageIngress
};

#define iscsiQsetSize (sizeof(iscsiQset) / sizeof(bcm_field_qualify_t))

static custom_field_qualify_t iscsiCustomQset[] =  /* iSCSI requirement */
{
    BROAD_ISCSI_UDF          /* DSTIP, L4SRC/DEST, iSCSI opCode, iSCSI opCode w/ TCP options */
};

#define iscsiCustomQsetSize (sizeof(iscsiCustomQset) / sizeof(custom_field_qualify_t))

static bcm_field_qualify_t ipv6NdQset[] =   /* req'd by system policies */
{
    bcmFieldQualifyInPorts,
    bcmFieldQualifyIp6HopLimit,    /* IP6_HOPLIMIT   */
    bcmFieldQualifyIp6NextHeader,  /* IP6_NEXTHEADER   */
    bcmFieldQualifyL2Format,
    bcmFieldQualifyVlanFormat,
    bcmFieldQualifyIpType,
    bcmFieldQualifyStageIngress
};

#define ipv6NdQsetSize (sizeof(ipv6NdQset) / sizeof(bcm_field_qualify_t))

static bcm_field_qualify_t ipv6L3L4Qset[] =  /* includes VLAN ID */
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
static bcm_field_qualify_t ipv6L3L4ClassIdQset[] =  /* includes VLAN ID */
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
    bcmFieldQualifyLookupClass0,
    bcmFieldQualifySrcMacGroup,
    bcmFieldQualifyStageIngress
};

#define ipv6L3L4ClassIdQsetSize (sizeof(ipv6L3L4ClassIdQset) / sizeof(bcm_field_qualify_t))

/* SQSet used for single wide mode policies */
static bcm_field_qualify_t l2QsetEgress[] =    /* l2 */
{
    bcmFieldQualifySrcMac,
    bcmFieldQualifyDstMac,
    bcmFieldQualifyEtherType,
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifyInnerVlanId,
    bcmFieldQualifyPortClass,
    bcmFieldQualifyOutPort,
    bcmFieldQualifyVlanFormat,
    bcmFieldQualifyStageEgress
};
#define l2QsetEgressSize (sizeof(l2QsetEgress) / sizeof(bcm_field_qualify_t))

static bcm_field_qualify_t l3l4QsetEgress[] =    /* l3/l4 */
{
    bcmFieldQualifySrcIp,
    bcmFieldQualifyDstIp,
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifyIpProtocol,
    bcmFieldQualifyL4SrcPort,
    bcmFieldQualifyL4DstPort,
    bcmFieldQualifyDSCP,
    bcmFieldQualifyPortClass,
    bcmFieldQualifyOutPort,
    bcmFieldQualifyIp4,
    bcmFieldQualifyVlanFormat,
    bcmFieldQualifyStageEgress
};
#define l3l4QsetEgressSize (sizeof(l3l4QsetEgress) / sizeof(bcm_field_qualify_t))

/* SQSet used for double wide mode policies */
static bcm_field_qualify_t ipv6L3L4QsetEgress[] =  /* includes VLAN ID */
{
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifySrcIp6,          
    bcmFieldQualifyDstIp6,          
    bcmFieldQualifyTos,          
    bcmFieldQualifyIp6NextHeader,          
    bcmFieldQualifyL4SrcPort,    /* also used for ICMP Msg Type */
    bcmFieldQualifyL4DstPort,
    bcmFieldQualifyPortClass,
    bcmFieldQualifyOutPort,
    bcmFieldQualifyIp6,
    bcmFieldQualifyStageEgress
};
#define ipv6L3L4QsetEgressSize (sizeof(ipv6L3L4QsetEgress) / sizeof(bcm_field_qualify_t))

/* SQSet used for double wide mode policies */
static bcm_field_qualify_t l2l3l4QsetLookup[] =    /* l2/l3 */
{
    bcmFieldQualifyInPort,
    bcmFieldQualifyPortClass,
    bcmFieldQualifySrcMac,
    bcmFieldQualifySrcIp,
    bcmFieldQualifyDstIp,
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifyIpProtocol,
    bcmFieldQualifyDSCP,
    bcmFieldQualifyL4DstPort,
    bcmFieldQualifyL2Format,
    bcmFieldQualifyVlanFormat,
    bcmFieldQualifyIpType,
    bcmFieldQualifyStageLookup
};
#define l2l3l4QsetLookupSize (sizeof(l2l3l4QsetLookup) / sizeof(bcm_field_qualify_t))

/******************************************************************************
 * DOT1AD specific QSET with vlan format and both VLANs                       *
 *****************************************************************************/
static bcm_field_qualify_t dot1adQsetLookup[] =    /* dot1ad specific qset */
{                                                  
    bcmFieldQualifyInPort,
    bcmFieldQualifyPortClass,
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifyInnerVlan,
    bcmFieldQualifyL2Format,
    bcmFieldQualifyVlanFormat,
    bcmFieldQualifyIpType,
    bcmFieldQualifyStageLookup
};

#define dot1adQsetLookupSize (sizeof(dot1adQsetLookup) / sizeof(bcm_field_qualify_t))

static bcm_field_qualify_t ipv6L3L4QsetLookup[] =  /* includes VLAN ID */
{
    bcmFieldQualifyInPort,
    bcmFieldQualifyPortClass,
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

/* Super QSet Table */
/* Qsets may incorporate UDFs, which must be created in each unit, so the
 * Qsets must also be created per unit.
 */

#define SUPER_QSET_NONE          0
#define SUPER_QSET_USED          1
#define SUPER_QSET_SINGLE_WIDE   2
#define SUPER_QSET_DOUBLE_WIDE   4
#define SUPER_QSET_TRIPLE_WIDE   8
#define SUPER_QSET_QUAD_WIDE     16

typedef struct
{
    uint32            flags;    /* super qset flags */
    bcm_field_qset_t  qset1;    /* 1st composite qset */
    bcm_field_qset_t  qsetAgg;  /* aggregate qset supported by entry - std fields only */
    bcm_field_qset_t  qsetUdf;  /* additional fields added by UDF - not part of aggreg */
    custom_field_qset_t customQset; /* for custom UDF fields that don't map to bcm_field_qset_t */
    uint32            slicesNeeded; /* indicates how many slices this sqset requires */
    uint32            rulesAvailable; /* indicates how many rules are available for this sqset */
    uint32            udfId;    /* identifier to differentiate which UDF this qset was created for */
    uint32            applicablePolicyTypes; /* Bitmap corresponding to applicable policy types. */
}
super_qset_entry_t;

#define SUPER_QSET_TABLE_SIZE  16    /* total number of super qsets */

static super_qset_entry_t super_qset_table[SOC_MAX_NUM_DEVICES][SUPER_QSET_TABLE_SIZE];

/* Group Definitions */
/* This structure mirrors the Broadcom group structure for each unit.
 * It should not contain policy or entry-specific information.
 */

#define GROUP_NONE               0
#define GROUP_USED               1
#define GROUP_EFP_ON_IFP         2

typedef struct
{
    uint32              flags;
    bcm_field_group_t   gid;    /* only valid in base group entry */
    BROAD_POLICY_TYPE_t type;
    int                 sqset;  /* super qset this group was created from */
    int                 base_group;  /* Identifies the base group in case this group is 
                                        part of a doublewide, triplewide, etc group. */
    uint32              availableRules;
}
group_table_t;

static group_table_t *group_table[SOC_MAX_NUM_DEVICES][BROAD_POLICY_STAGE_COUNT];
static int            group_table_size[SOC_MAX_NUM_DEVICES][BROAD_POLICY_STAGE_COUNT];

static int dot1ad_vfp_group_id[SOC_MAX_NUM_DEVICES];
int dot1ad_vfp_rule_id[SOC_MAX_NUM_DEVICES][L7_MAX_PHYSICAL_PORTS_PER_SLOT];
int traceDot1adVfpRule;


#define CHECK_GROUP(u,s,g)  {if ((u>=SOC_MAX_NUM_DEVICES)||(s>=BROAD_POLICY_STAGE_COUNT)||(g>=group_table_size[u][s])) LOG_ERROR(g);}

/* Mapping from BROAD_ENTRY_t to bcm_field_entry_t */
#define BROAD_ENTRY_TO_BCM_ENTRY(entry)   ((bcm_field_entry_t)entry)
#define BCM_ENTRY_TO_BROAD_ENTRY(eid)     ((BROAD_ENTRY_t)eid)

/* Group Allocation Definitions */

#define ALLOC_BLOCK_LOW         0    /* blocks */
#define ALLOC_BLOCK_MEDIUM      1    /* used for VLAN policies */
#define ALLOC_BLOCK_HIGH        2    /* used for PORT policies */
#define ALLOC_BLOCK_SYS_EXT     3    /* PTin added: Policies */

#define ALLOC_HIGH_TO_LOW  1    /* directions */
#define ALLOC_LOW_TO_HIGH  2

typedef struct
{
    int highPrio;     /* highest priority */
    int lowPrio;      /* lowest priority  */
}
group_alloc_table_t;

/* PTin modified: Policies */
static group_alloc_table_t group_alloc_table[SOC_MAX_NUM_DEVICES][BROAD_POLICY_STAGE_COUNT][/*ALLOC_BLOCK_HIGH*/ ALLOC_BLOCK_SYS_EXT+1];

static int _policy_group_lookupstatus_qualify(int unit, bcm_field_entry_t eid, L7_uint32 data, L7_uint32 mask);

int l7_bcm_dot1ad_vfp_group_alloc(int unit,
                                         BROAD_POLICY_STAGE_t  policyStage,
                                         BROAD_GROUP_t        *group);


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

static int _policy_set_subset(bcm_field_qset_t q1, custom_field_qset_t custom_q1, 
                              bcm_field_qset_t q2, custom_field_qset_t custom_q2)
{
    int  i;

    for (i = 0; i < bcmFieldQualifyCount; i++)
    {
        if (BCM_FIELD_QSET_TEST(q1,i) && !BCM_FIELD_QSET_TEST(q2,i))  {
            return BCM_E_FAIL;
        }
    }

    for (i = 0; i < customFieldQualifyLast; i++)
    {
        if (CUSTOM_FIELD_QSET_TEST(custom_q1,i) && !CUSTOM_FIELD_QSET_TEST(custom_q2,i))  {
            return BCM_E_FAIL;
        }
    }

    return BCM_E_NONE;
}

static void _policy_set_union(bcm_field_qset_t q1, bcm_field_qset_t *q2)
{
    int  i;

    for (i = 0; i < bcmFieldQualifyCount; i++)
    {
        if (BCM_FIELD_QSET_TEST(q1,i))  {
            BCM_FIELD_QSET_ADD(*q2,i);
        }
    }
}

/* Super Qset Functions */

static int _policy_super_qset_find_match(int                  unit, 
                                         BROAD_POLICY_STAGE_t policyStage,
                                         BROAD_POLICY_TYPE_t  type, 
                                         bcm_field_qset_t     qset, 
                                         custom_field_qset_t  customQset, 
                                         int                 *idx)
{
    int  i, rv;

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
    {
        printf("- searching for qset ");
        for (i = 0; i < bcmFieldQualifyCount; i++)
            printf ("%d", (BCM_FIELD_QSET_TEST(qset, i) ? 1 : 0));
        printf("\n");
    }

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

            /* consider the UDF when searching for a super qset */
            BCM_FIELD_QSET_INIT(qsetFull);
            _policy_set_union(super_qset_table[unit][i].qsetAgg, &qsetFull);
            _policy_set_union(super_qset_table[unit][i].qsetUdf, &qsetFull);

            rv = _policy_set_subset(qset, customQset, qsetFull, super_qset_table[unit][i].customQset);
            if (BCM_E_NONE == rv)
            {
                if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
                    printf("- using super qset %d\n", i);

                *idx = i;
                return BCM_E_NONE;
            }
            else
            {
              if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
                  printf("- _policy_set_subset failed rv %d\n", rv);
            }
        }
    }

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
    {
      printf("- failed to find qset \n");
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
    packet_format.vlan_tag        = BCM_FIELD_DATA_FORMAT_VLAN_DOUBLE_TAGGED; //BCM_FIELD_DATA_FORMAT_VLAN_NO_TAG;
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

static int _policy_super_qset_add(int                  unit,
                                  bcm_field_qualify_t *q1,
                                  int                  q1Size,
                                  custom_field_qualify_t *q2,
                                  int                  q2Size,
                                  int                  flags,
                                  int                  slicesNeeded,
                                  int                  rulesAvailable,
                                  L7_BOOL             *applicablePolicyTypes)
{
    int                 i;
    int                 rv = BCM_E_NONE;
    bcm_field_qset_t    qset1;
    super_qset_entry_t *qsetPtr;

    CHECK_UNIT(unit);

    rv = _policy_super_qset_find_free(unit, &i);
    if (BCM_E_NONE != rv)
    {
      LOG_ERROR(rv);  /* Need to increase SUPER_QSET_TABLE_SIZE. */
      return rv;
    }

    qsetPtr = &super_qset_table[unit][i];

    qsetPtr->flags = flags | SUPER_QSET_USED;
    qsetPtr->slicesNeeded = slicesNeeded;
    qsetPtr->rulesAvailable = rulesAvailable;

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
    for (i = 0; i < q1Size; i++)  {
        BCM_FIELD_QSET_ADD(qset1, q1[i]);
    }

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
            BCM_FIELD_QSET_ADD(qsetPtr->customQset, customFieldQualifyIscsiOpcode);
            BCM_FIELD_QSET_ADD(qsetPtr->customQset, customFieldQualifyIscsiOpcodeTcpOptions);

            qsetPtr->udfId = BROAD_ISCSI_UDF;
        }
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

static int _policy_default_rules_per_slice(int unit, BROAD_POLICY_STAGE_t stage)
{
  int rules_per_slice = 0;

  switch (stage)
  {
  case BROAD_POLICY_STAGE_LOOKUP:
    if (SOC_IS_FIREBOLT2(unit) ||
        SOC_IS_VALKYRIE(unit) ||
        SOC_IS_SCORPION(unit))
    {
      rules_per_slice = 256;
    }
    // PTin added: SOC_IS_VALKYRIE2(unit)
    else if (SOC_IS_TRIUMPH(unit) || 
             SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || SOC_IS_VALKYRIE2(unit))
    {
      rules_per_slice = 512;
    }
    else
    {
      rules_per_slice = 0;
    }
    break;

  case BROAD_POLICY_STAGE_INGRESS:
    if (SOC_IS_FIREBOLT2(unit) ||
        SOC_IS_VALKYRIE(unit))
    {
      rules_per_slice = 256;
    }
    // PTin added: SOC_IS_VALKYRIE2(unit)
    else if (SOC_IS_TRIUMPH(unit) || 
             SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || SOC_IS_VALKYRIE2(unit))
    {
      rules_per_slice = 512;
    }
    else
    {
      rules_per_slice = 128;
    }
    break;

  case BROAD_POLICY_STAGE_EGRESS:
    if (SOC_IS_FIREBOLT2(unit) ||
        SOC_IS_VALKYRIE(unit) ||
        SOC_IS_SCORPION(unit))
    {
      rules_per_slice = 128;
    }
    // PTin added: SOC_IS_VALKYRIE2(unit)
    else if (SOC_IS_TRIUMPH(unit) || 
             SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || SOC_IS_VALKYRIE2(unit))
    {
      rules_per_slice = 256;
    }
    else
      rules_per_slice = 0;
    break;

  default:
    break;
  }

  return rules_per_slice;
}

static int _policy_super_qset_init_vfp(int unit)
{
  int rules_per_slice;
  L7_BOOL applicable_policy_types[BROAD_POLICY_TYPE_LAST];

  rules_per_slice = _policy_default_rules_per_slice(unit, BROAD_POLICY_STAGE_LOOKUP);

  if(policy_stage_supported(unit, BROAD_POLICY_STAGE_LOOKUP))
  {
    memset(applicable_policy_types, 0, sizeof(applicable_policy_types));
    applicable_policy_types[BROAD_POLICY_TYPE_PORT]     = L7_TRUE;
    applicable_policy_types[BROAD_POLICY_TYPE_IPSG] = L7_TRUE;

      /* The following qset uses intra-slice doublewide mode, so the number of rules is cut in half. */
      _policy_super_qset_add(unit, l2l3l4QsetLookup, l2l3l4QsetLookupSize, NULL, 0, SUPER_QSET_DOUBLE_WIDE, 1, 
                           rules_per_slice / 2, applicable_policy_types);

    _policy_super_qset_add(unit, ipv6L3L4QsetLookup, ipv6L3L4QsetLookupSize, NULL, 0, SUPER_QSET_DOUBLE_WIDE, 1, 
                           rules_per_slice, applicable_policy_types);
  }

  return BCM_E_NONE;
}

static int _policy_super_qset_init_ifp(int unit)
{
  int rv;
  int rules_per_slice;
  L7_BOOL applicable_policy_types[BROAD_POLICY_TYPE_LAST];

  rules_per_slice = _policy_default_rules_per_slice(unit, BROAD_POLICY_STAGE_INGRESS);

  if (_policy_supports_wide_mode(unit))
  {
    memset(applicable_policy_types, 0, sizeof(applicable_policy_types));
    applicable_policy_types[BROAD_POLICY_TYPE_PORT] = L7_TRUE;
    applicable_policy_types[BROAD_POLICY_TYPE_VLAN] = L7_TRUE;

    if (policy_stage_supported(unit, BROAD_POLICY_STAGE_LOOKUP))
    {
       // PTin added: SOC_IS_VALKYRIE2(unit)
       if ( SOC_IS_TRIUMPH2(unit) ||
            SOC_IS_TRIUMPH(unit)  ||
            SOC_IS_APOLLO(unit)   ||
            SOC_IS_VALKYRIE2(unit)
          )
      {
        /* The following qset uses doublewide mode.
           In Triumph intra-slice doublewide mode is used.
           So, the number of required slices is 1.
        */
        _policy_super_qset_add(unit, 
                               l2l3l4Xgs4ClassIdQset,
                               l2l3l4Xgs4ClassIdQsetSize,
                               NULL,
                               0, SUPER_QSET_DOUBLE_WIDE, 1,
                              rules_per_slice/2, applicable_policy_types);
      }
      else if (SOC_IS_SCORPION(unit))
      {
        /* The following qset uses intra-slice doublewide mode, so the number of rules is cut in half. */
        _policy_super_qset_add(unit, l2l3l4ClassIdQset, l2l3l4ClassIdQsetSize, NULL,    0, SUPER_QSET_DOUBLE_WIDE, 1, 
                               rules_per_slice / 2, applicable_policy_types);
      }
      else
      {
        /* The following qset uses doublewide mode. */
        _policy_super_qset_add(unit, l2l3l4ClassIdQset, l2l3l4ClassIdQsetSize, NULL,    0, SUPER_QSET_DOUBLE_WIDE, 2, 
                               rules_per_slice, applicable_policy_types);
      }
        /* The following qset uses intra-slice doublewide mode, so the number of rules is cut in half. */
        _policy_super_qset_add(unit, ipv6L3L4ClassIdQset, ipv6L3L4ClassIdQsetSize, NULL,    0, SUPER_QSET_QUAD_WIDE, 2, 
                             (rules_per_slice / 2), applicable_policy_types);
      }
    else
    {
      if (soc_feature(unit, soc_feature_src_mac_group))
      {
        _policy_super_qset_add(unit, l2l3l4SrcMacGroupQset, l2l3l4SrcMacGroupQsetSize, NULL,    0, SUPER_QSET_DOUBLE_WIDE, 2, 
                               rules_per_slice, applicable_policy_types);
      }
      else
      {
        _policy_super_qset_add(unit, l2l3l4Qset, l2l3l4QsetSize, NULL,    0, SUPER_QSET_DOUBLE_WIDE, 2, 
                               rules_per_slice, applicable_policy_types);
      }
      _policy_super_qset_add(unit, ipv6L3L4Qset, ipv6L3L4QsetSize, NULL,    0, SUPER_QSET_TRIPLE_WIDE, 4, 
                             rules_per_slice, applicable_policy_types);
    }
  }
  if (SOC_IS_HAWKEYE(unit))
  {
    memset(applicable_policy_types, 0, sizeof(applicable_policy_types));
    applicable_policy_types[BROAD_POLICY_TYPE_PORT]        = L7_TRUE;
    applicable_policy_types[BROAD_POLICY_TYPE_VLAN]        = L7_TRUE;
    applicable_policy_types[BROAD_POLICY_TYPE_SYSTEM]      = L7_TRUE;
    applicable_policy_types[BROAD_POLICY_TYPE_SYSTEM_PORT] = L7_TRUE;
    applicable_policy_types[BROAD_POLICY_TYPE_COSQ]        = L7_TRUE;
    applicable_policy_types[BROAD_POLICY_TYPE_SYS_EXT]     = L7_TRUE;  /* PTin added: Policies */

      _policy_super_qset_add(unit, l2SvtLookupStatusQset,   l2SvtLookupStatusQsetSize,   NULL,    0, SUPER_QSET_SINGLE_WIDE, 1, 
                           rules_per_slice, applicable_policy_types);
  }
  memset(applicable_policy_types, 0, sizeof(applicable_policy_types));
  applicable_policy_types[BROAD_POLICY_TYPE_SYSTEM]      = L7_TRUE;
  applicable_policy_types[BROAD_POLICY_TYPE_SYSTEM_PORT] = L7_TRUE;
  applicable_policy_types[BROAD_POLICY_TYPE_COSQ]        = L7_TRUE;
  applicable_policy_types[BROAD_POLICY_TYPE_SYS_EXT]     = L7_TRUE;  /* PTin added: Policies */

  // PTin added: SOC_IS_VALKYRIE2(unit)
  if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || SOC_IS_VALKYRIE2(unit))
  {
    /* Intraslice doublewide mode. */
    _policy_super_qset_add(unit, systemQsetTriumph2,  systemQsetTriumph2Size,  NULL,  0, SUPER_QSET_DOUBLE_WIDE, 1, 
                           rules_per_slice / 2, applicable_policy_types);
  }
  else if (SOC_IS_SCORPION(unit))
  {
    _policy_super_qset_add(unit, systemQset, systemQsetSize,  systemCustomQset, systemCustomQsetSize, SUPER_QSET_SINGLE_WIDE, 1, 
                           rules_per_slice, applicable_policy_types);
    _policy_super_qset_add(unit, ipv6NdQset, ipv6NdQsetSize,  NULL,  0, SUPER_QSET_SINGLE_WIDE, 1, 
                           rules_per_slice, applicable_policy_types);
    _policy_super_qset_add(unit, l2SvtQset,   l2SvtQsetSize,   NULL,    0, SUPER_QSET_SINGLE_WIDE, 1, 
                           rules_per_slice, applicable_policy_types);
  }
  else if (_policy_supports_intraslice_doublewide_mode(unit))
  {
    /* Intraslice doublewide mode. */
    _policy_super_qset_add(unit, systemQsetDouble,  systemQsetDoubleSize,  NULL,  0, SUPER_QSET_DOUBLE_WIDE, 1, 
                           rules_per_slice / 2, applicable_policy_types);
  }
  else if (_policy_supports_wide_mode(unit))
  {
    /* Doublewide mode. */
    // PTin modified
    // This mode was blocking the group creation, because of the addition of a new field (SrcIp6)... changed from double to quad
    printf("%s(%d) rules_per_slice=%u\r\n",__FUNCTION__,__LINE__,rules_per_slice);
    applicable_policy_types[BROAD_POLICY_TYPE_SYSTEM]  = L7_TRUE;
    applicable_policy_types[BROAD_POLICY_TYPE_SYS_EXT] = L7_FALSE;
    rv = _policy_super_qset_add(unit, systemQsetDouble,  systemQsetDoubleSize,  NULL,  0, SUPER_QSET_DOUBLE_WIDE,  1, 
                                rules_per_slice, applicable_policy_types);
    applicable_policy_types[BROAD_POLICY_TYPE_SYSTEM]  = L7_FALSE;
    applicable_policy_types[BROAD_POLICY_TYPE_SYS_EXT] = L7_TRUE;
    rv = _policy_super_qset_add(unit, systemQsetQuad,  systemQsetQuadSize,  NULL,  0, SUPER_QSET_QUAD_WIDE,  1,
                                rules_per_slice, applicable_policy_types);
  }
  else
  {
    _policy_super_qset_add(unit, systemQset, systemQsetSize,  systemCustomQset, systemCustomQsetSize, SUPER_QSET_SINGLE_WIDE, 1, 
                           rules_per_slice, applicable_policy_types);
    _policy_super_qset_add(unit, ipv6NdQset, ipv6NdQsetSize,  NULL,  0, SUPER_QSET_SINGLE_WIDE, 1, 
                           rules_per_slice, applicable_policy_types);

    /* The following sqset is used for both system policies and user policies. */
    applicable_policy_types[BROAD_POLICY_TYPE_PORT]        = L7_TRUE;
    applicable_policy_types[BROAD_POLICY_TYPE_VLAN]        = L7_TRUE;

      _policy_super_qset_add(unit, l2SvtQset,   l2SvtQsetSize,   NULL,    0, SUPER_QSET_SINGLE_WIDE, 1, 
                           rules_per_slice, applicable_policy_types);

    /* The following sqsets are used only for user policies. */
    memset(applicable_policy_types, 0, sizeof(applicable_policy_types));
    applicable_policy_types[BROAD_POLICY_TYPE_PORT]        = L7_TRUE;
    applicable_policy_types[BROAD_POLICY_TYPE_VLAN]        = L7_TRUE;

      _policy_super_qset_add(unit, l2l3DstQset, l2l3DstQsetSize, NULL,    0, SUPER_QSET_SINGLE_WIDE, 1, 
                           rules_per_slice, applicable_policy_types);
      _policy_super_qset_add(unit, vlanl3Qset,  vlanl3QsetSize,  NULL,    0, SUPER_QSET_SINGLE_WIDE, 1, 
                           rules_per_slice, applicable_policy_types);
    }
  /* The following sqset is used for iSCSI control rules. */
  memset(applicable_policy_types, 0, sizeof(applicable_policy_types));
  applicable_policy_types[BROAD_POLICY_TYPE_ISCSI] = L7_TRUE;

    _policy_super_qset_add(unit, iscsiQset,  iscsiQsetSize,  iscsiCustomQset,  iscsiCustomQsetSize, SUPER_QSET_SINGLE_WIDE, 1, 
                         rules_per_slice, applicable_policy_types);

  /* The following sqset is used for iSCSI sessions but may also be used for user
     policies on devices that do not support doublewide mode. */
  memset(applicable_policy_types, 0, sizeof(applicable_policy_types));
  applicable_policy_types[BROAD_POLICY_TYPE_ISCSI] = L7_TRUE;
  if (!_policy_supports_wide_mode(unit))
  {
    applicable_policy_types[BROAD_POLICY_TYPE_PORT] = L7_TRUE;
    applicable_policy_types[BROAD_POLICY_TYPE_VLAN] = L7_TRUE;
  }
    _policy_super_qset_add(unit, l3l4Qset,    l3l4QsetSize,    NULL,    0, SUPER_QSET_SINGLE_WIDE, 1, 
                         rules_per_slice, applicable_policy_types);

  /* The following sqset is used for IPSG policies but may also be used for user
     policies on devices that do not support doublewide mode. */
  memset(applicable_policy_types, 0, sizeof(applicable_policy_types));
#if L7_FEAT_IPSG_ON_IFP
  applicable_policy_types[BROAD_POLICY_TYPE_IPSG] = L7_TRUE;
#endif
  if (!_policy_supports_wide_mode(unit))
  {
    applicable_policy_types[BROAD_POLICY_TYPE_PORT] = L7_TRUE;
    applicable_policy_types[BROAD_POLICY_TYPE_VLAN] = L7_TRUE;
  }
  _policy_super_qset_add(unit, l2l3SrcQset, l2l3SrcQsetSize, NULL, 0, SUPER_QSET_SINGLE_WIDE, 1, 
                         rules_per_slice, applicable_policy_types);

  return BCM_E_NONE;
}

static int _policy_super_qset_init_efp(int unit)
{
  int rules_per_slice;
  L7_BOOL applicable_policy_types[BROAD_POLICY_TYPE_LAST];

  rules_per_slice = _policy_default_rules_per_slice(unit, BROAD_POLICY_STAGE_EGRESS);

  if(policy_stage_supported(unit, BROAD_POLICY_STAGE_EGRESS))
  {
    memset(applicable_policy_types, 0, sizeof(applicable_policy_types));
    applicable_policy_types[BROAD_POLICY_TYPE_PORT] = L7_TRUE;
    applicable_policy_types[BROAD_POLICY_TYPE_VLAN] = L7_TRUE;

    _policy_super_qset_add(unit, l2QsetEgress, l2QsetEgressSize, NULL, 0, SUPER_QSET_SINGLE_WIDE, 1, 
                           rules_per_slice, applicable_policy_types);

    _policy_super_qset_add(unit, l3l4QsetEgress, l3l4QsetEgressSize, NULL, 0, SUPER_QSET_SINGLE_WIDE, 1, 
                           rules_per_slice, applicable_policy_types);

    _policy_super_qset_add(unit, ipv6L3L4QsetEgress, ipv6L3L4QsetEgressSize, NULL, 0, SUPER_QSET_DOUBLE_WIDE, 2, 
                           rules_per_slice, applicable_policy_types);
  }

  return BCM_E_NONE;
}

static int _policy_super_qset_init(int unit)
{
  int  i;

  for (i = 0; i < SUPER_QSET_TABLE_SIZE; i++)
  {
    super_qset_table[unit][i].flags = SUPER_QSET_NONE;
  }

  _policy_super_qset_init_vfp(unit);
  _policy_super_qset_init_ifp(unit);
  _policy_super_qset_init_efp(unit);

#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
  /*Reserve the VFP group for dot1ad application */
  if (policy_stage_supported(unit, BROAD_POLICY_STAGE_LOOKUP) == L7_TRUE)
  {
  dot1ad_vfp_group_id[unit] = L7_BCM_DOT1AD_INVALID_UNIT;
  l7_bcm_dot1ad_vfp_group_alloc(unit,
                                BROAD_POLICY_STAGE_LOOKUP,
                                &dot1ad_vfp_group_id[unit]
                               );

  }

  for (i=0;i<L7_MAX_PHYSICAL_PORTS_PER_SLOT;i++)
  {
    dot1ad_vfp_rule_id[unit][i] = L7_BCM_DOT1AD_INVALID_RULE;
  }
#endif

    return BCM_E_NONE;
}

static int _policy_action_map_init(int unit)
{
  // PTin added: SOC_IS_VALKYRIE2(unit)
  if ((SOC_IS_TR_VL(unit)) ||
      (SOC_IS_SCORPION(unit)) || (SOC_IS_TRIUMPH2(unit)) || (SOC_IS_APOLLO(unit)) || (SOC_IS_VALKYRIE2(unit)))
  {
    /* Modify action maps for certain actions. */
    memcpy(&ingress_action_map[BROAD_ACTION_SET_COSQ],     &xgs4_ingress_set_cosq_action_map,     sizeof(action_map_entry_t));
    memcpy(&ingress_action_map[BROAD_ACTION_SET_USERPRIO], &xgs4_ingress_set_userprio_action_map, sizeof(action_map_entry_t));

    memcpy(&ingress_action_map[BROAD_ACTION_SET_USERPRIO_AS_COS2], 
           &xgs4_ingress_set_userprio_as_cos2_action_map,
           sizeof(action_map_entry_t));
  }

  return BCM_E_NONE;
}

/* Group Allocation Functions */

static void _policy_group_alloc_type(BROAD_POLICY_TYPE_t type, int *block, int *dir)
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
        *block = ALLOC_BLOCK_MEDIUM;
        *dir   = ALLOC_LOW_TO_HIGH;
        break;
    case BROAD_POLICY_TYPE_PORT:
        *block = ALLOC_BLOCK_HIGH;
        *dir   = ALLOC_HIGH_TO_LOW;
        break;
    case BROAD_POLICY_TYPE_SYSTEM_PORT:
    case BROAD_POLICY_TYPE_SYSTEM:
        *block = ALLOC_BLOCK_LOW;
        *dir   = ALLOC_LOW_TO_HIGH; //ALLOC_HIGH_TO_LOW;
        break;
    /* PTin added: Policies */
    case BROAD_POLICY_TYPE_SYS_EXT:
        *block = ALLOC_BLOCK_SYS_EXT;
        *dir   = ALLOC_LOW_TO_HIGH; //ALLOC_HIGH_TO_LOW;
        break;
    case BROAD_POLICY_TYPE_COSQ:
        *block = ALLOC_BLOCK_LOW;
        *dir   = ALLOC_LOW_TO_HIGH;
        break;
    default:
        LOG_ERROR(type);
        break;
    }
}

static int _policy_sqset_slice_count(int unit, unsigned int sqset_idx)
{
  return super_qset_table[unit][sqset_idx].slicesNeeded;
}

static L7_BOOL _policy_group_types_compatible(BROAD_POLICY_TYPE_t group1_type, BROAD_POLICY_TYPE_t group2_type)
{
  L7_BOOL groupTypesCompatible = L7_TRUE;

  if (group1_type==BROAD_POLICY_TYPE_SYS_EXT)  group1_type = BROAD_POLICY_TYPE_SYSTEM;
  if (group2_type==BROAD_POLICY_TYPE_SYS_EXT)  group2_type = BROAD_POLICY_TYPE_SYSTEM;

  if (group1_type != group2_type)
  {
    /* There is a special case here... SYSTEM policies, SYSTEM_PORT, and COSQ
       policies may share a group. */
    if (group1_type == BROAD_POLICY_TYPE_SYSTEM_PORT)
    {
      if ((group2_type != BROAD_POLICY_TYPE_SYSTEM) &&
          (group2_type != BROAD_POLICY_TYPE_COSQ))
      {
        groupTypesCompatible = L7_FALSE;
      }
    }
    else if (group1_type == BROAD_POLICY_TYPE_SYSTEM)
    {
      if ((group2_type != BROAD_POLICY_TYPE_SYSTEM_PORT) &&
          (group2_type != BROAD_POLICY_TYPE_COSQ))
      {
        groupTypesCompatible = L7_FALSE;
      }
    }
    else if (group1_type == BROAD_POLICY_TYPE_COSQ)
    {
      if ((group2_type != BROAD_POLICY_TYPE_SYSTEM) &&
          (group2_type != BROAD_POLICY_TYPE_SYSTEM_PORT))
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
    int block, dir;
    int used_block, used_dir;
    group_table_t *groupPtr;

    /* PTin added */
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
        *group = groupPtr->base_group;
        groupPtr = &group_table[unit][policyStage][*group];

        /* make sure that the block that this group belongs to matches
           the block requested */
        if (_policy_group_types_compatible(type, groupPtr->type) == L7_FALSE)
        {
          return BCM_E_FAIL;
        }
        /* PTin removed */
        #if 1
        _policy_group_alloc_type(groupPtr->type, &used_block, &used_dir);
        if (block != used_block)
        {
          return BCM_E_FAIL;
        }
        #endif
    }

    return BCM_E_NONE;
}

static int _policy_group_decrement(int                   unit,
                                   BROAD_POLICY_STAGE_t  policyStage,
                                   BROAD_GROUP_t        *group, 
                                   int                   block, 
                                   int                   type)
{
    int used_block, used_dir;
    group_table_t *groupPtr;

    if ((*group - 1) >= group_alloc_table[unit][policyStage][block].lowPrio)
    {
        *group = *group - 1;

        groupPtr = &group_table[unit][policyStage][*group];
        if (groupPtr->flags & GROUP_USED)
        {
            *group = groupPtr->base_group;
            groupPtr = &group_table[unit][policyStage][*group];

            /* make sure that the block that this group belongs to matches
               the block requested */
            if (_policy_group_types_compatible(type, groupPtr->type) == L7_FALSE)
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
                                   int                   block, 
                                   int                   type)
{
    int used_block, used_dir;
    group_table_t *groupPtr;

    while ((*group + 1) <= group_alloc_table[unit][policyStage][block].highPrio)
    {
      *group = *group + 1;

      groupPtr = &group_table[unit][policyStage][*group];
      if (groupPtr->flags & GROUP_USED)
      {
          if (groupPtr->base_group != *group)
          {
            continue;
          }

          /* make sure that the block that this group belongs to matches
             the block requested */
          if (_policy_group_types_compatible(type, groupPtr->type) == L7_FALSE)
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
    int block, dir;
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

static int _policy_group_find_free_group(int                  unit, 
                                         BROAD_POLICY_STAGE_t policyStage,
                                         BROAD_POLICY_TYPE_t  type, 
                                         BROAD_GROUP_t       *group, 
                                         int                  slices_needed)
{
    int  rv;
    int block, dir;
    int i;
    int all_slices_available;

    _policy_group_alloc_type(type, &block, &dir);

    rv = _policy_group_find_first(unit, policyStage, type, group);
    while (BCM_E_NONE == rv)
    {
        /* find the base group based on the number of slices needed */
        *group = *group - (*group % slices_needed);

        if (!(group_table[unit][policyStage][*group].flags & GROUP_USED))
        {
            /* make sure that any other slices needed are also 
               available */
            all_slices_available = 1;
            for (i = *group; i < (*group + slices_needed); i++)
            {
              if (group_table[unit][policyStage][i].flags & GROUP_USED)
              {
                all_slices_available = 0;
                break;
              }
            }

            if (all_slices_available)
            {
              if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
                printf("- found free group %d\n", *group);

              return BCM_E_NONE;
            }
        }

        if (ALLOC_HIGH_TO_LOW == dir)
        {
          *group = *group - slices_needed + 1;
        }
        else
        {
          *group = *group + slices_needed - 1;
        }
        rv = _policy_group_find_next(unit, policyStage, type, group);
    }

    return BCM_E_FAIL;
}

static int _policy_field_to_bcm_field(BROAD_POLICY_FIELD_t  field,
                                      BROAD_POLICY_STAGE_t  policyStage,
                                      char                 *value,
                                      char                 *mask,
                                      bcm_field_qualify_t  *bcm_field,
                                      int                   req_ethtype)
{
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

static int _policy_group_calc_qset(int                   unit,
                                   BROAD_POLICY_STAGE_t  policyStage,
                                   BROAD_POLICY_ENTRY_t *entryPtr, 
                                   bcm_field_qset_t     *qset,
                                   custom_field_qset_t  *customQset,
                                   int                  *req_ethtype)
{
    int    r, f;
    int    rv;
    L7_BOOL allRulesQualifyEthertype         = L7_TRUE;
    L7_BOOL atLeastOneRuleQualifiesEthertype = L7_FALSE;

    BCM_FIELD_QSET_INIT(*qset);
    CUSTOM_FIELD_QSET_INIT(*customQset);

    /* Determine if ETHTYPE is required in this QSET, or if IpType can be used instead. */
    for (r = 0; r < entryPtr->ruleCount; r++)
    {
      if (hapiBroadPolicyFieldFlagsGet(&entryPtr->ruleInfo[r].fieldInfo, BROAD_FIELD_ETHTYPE) == BROAD_FIELD_SPECIFIED)
      {
        atLeastOneRuleQualifiesEthertype = L7_TRUE;

        uint16 ethtype = *((uint16*)hapiBroadPolicyFieldValuePtr(&entryPtr->ruleInfo[r].fieldInfo, BROAD_FIELD_ETHTYPE));
        if ((0x0800 != ethtype) && (0x86DD != ethtype))
        {
          *req_ethtype = 1;
        }
      }
      else
      {
        allRulesQualifyEthertype = L7_FALSE;
      }
    }

    if ((atLeastOneRuleQualifiesEthertype == L7_TRUE) && (allRulesQualifyEthertype == L7_FALSE))
    {
      *req_ethtype = 1;
    }

    /* Create a qset containing each field specified in each rule. */
    for (r = 0; r < entryPtr->ruleCount; r++)
    {
        for (f = 0; f < BROAD_FIELD_LAST; f++)
        {
            if (hapiBroadPolicyFieldFlagsGet(&entryPtr->ruleInfo[r].fieldInfo, f) == BROAD_FIELD_SPECIFIED)
            {
                char                *value;
                char                *mask;
                bcm_field_qualify_t  bcm_field = 0;
                L7_ushort16          temp16;

                value = (char*)hapiBroadPolicyFieldValuePtr(&entryPtr->ruleInfo[r].fieldInfo, f);
                mask  = (char*)hapiBroadPolicyFieldMaskPtr(&entryPtr->ruleInfo[r].fieldInfo, f);

                switch (f)
                {
                /* custom fields go here */
                case BROAD_FIELD_ICMP_MSG_TYPE:
                  /* Use L4 src port for ICMP Msg Type. We can do this because
                     XGS3 just treats the L4 src port field as the first two
                     bytes following the IP header. */
                  BCM_FIELD_QSET_ADD(*qset,bcmFieldQualifyL4SrcPort);
                  break;

                case BROAD_FIELD_ISCSI_OPCODE:
                  BCM_FIELD_QSET_ADD(*customQset, customFieldQualifyIscsiOpcode);
                  break;

                case BROAD_FIELD_ISCSI_OPCODE_TCP_OPTIONS:
                  BCM_FIELD_QSET_ADD(*customQset, customFieldQualifyIscsiOpcodeTcpOptions);
                  break;

                case BROAD_FIELD_LOOKUP_STATUS:
                  memcpy(&temp16, mask, sizeof(L7_ushort16));
                  if (temp16 & BROAD_LOOKUPSTATUS_DOS_ATTACK_PKT)  {
                    BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyDosAttack);
                  }

                  if (temp16 & BROAD_LOOKUPSTATUS_UNRESOLVED_SA)  {
                    BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyL2StationMove);
                  }

                  if (temp16 & BROAD_LOOKUPSTATUS_LPM_HIT)  {
                    BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyL3DestRouteHit);
                  }

                  if (temp16 & BROAD_LOOKUPSTATUS_STARGV_HIT)  {
                    BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyIpmcStarGroupHit);
                  }

                  if (temp16 & BROAD_LOOKUPSTATUS_L3_DST_HIT)  {
                    BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyL3DestHostHit);
                  }

                  if (temp16 & BROAD_LOOKUPSTATUS_L3_UC_SRC_HIT)  {
                    BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyL3SrcHostHit);
                  }

                  if (temp16 & BROAD_LOOKUPSTATUS_L2_USER_ENTRY_HIT)  {
                    BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyL2CacheHit);
                  }

                  if (temp16 & BROAD_LOOKUPSTATUS_L2_TABLE_DST_L3)  {
                    BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyL3Routable);
                  }

                  if (temp16 & BROAD_LOOKUPSTATUS_L2_DST_HIT)  {
                    BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyL2DestHit);
                  }

                  if (temp16 & BROAD_LOOKUPSTATUS_L2_SRC_STATIC)  {
                    BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyL2SrcStatic);
                  }

                  if (temp16 & BROAD_LOOKUPSTATUS_L2_SRC_HIT)  {
                    BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyL2SrcHit);
                  }

                  if (temp16 & BROAD_LOOKUPSTATUS_INGRESS_SPG_STATE_MASK)  {
                    BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyIngressStpState);
                  }

                  if (temp16 & BROAD_LOOKUPSTATUS_FB_VLAN_ID_VALID)  {
                    BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyForwardingVlanValid); /* ?? */
                  }

                  if (temp16 & BROAD_LOOKUPSTATUS_VXLT_HIT)  {
                    BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyVlanTranslationHit);
                  }

                  if (temp16 & BROAD_LOOKUPSTATUS_TUNNEL_HIT)  {
                    BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyTunnelTerminated);
                  }

                  break;

                default:
                  rv = _policy_field_to_bcm_field(f, policyStage, (char*)value, (char*)mask, &bcm_field, *req_ethtype);
                  if (BCM_E_NONE != rv)
                  {
                    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
                        printf("- _policy_field_to_bcm_field failed rv %d; field f %d; policyStage %d\n", rv, f, policyStage);

                    return rv;
                  }

                  BCM_FIELD_QSET_ADD(*qset,bcm_field);
                  break;
                }
            }
        }
    }

    switch (policyStage)
    {
    case BROAD_POLICY_STAGE_LOOKUP:
      BCM_FIELD_QSET_ADD(*qset,bcmFieldQualifyStageLookup);
      break;
    case BROAD_POLICY_STAGE_INGRESS:
      BCM_FIELD_QSET_ADD(*qset,bcmFieldQualifyStageIngress);
      break;
    case BROAD_POLICY_STAGE_EGRESS:
      BCM_FIELD_QSET_ADD(*qset,bcmFieldQualifyStageEgress);
      break;
    default:
      return BCM_E_CONFIG;
    }

    switch (entryPtr->policyType)
    {
    case BROAD_POLICY_TYPE_PORT:
    case BROAD_POLICY_TYPE_IPSG:
    case BROAD_POLICY_TYPE_SYSTEM_PORT:
    case BROAD_POLICY_TYPE_COSQ:
      if (policyStage == BROAD_POLICY_STAGE_LOOKUP)
      {
        BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyInPort);
        BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyPortClass);
      }
      else if (policyStage == BROAD_POLICY_STAGE_INGRESS)
      {
        BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyInPorts);
      }
      else if (policyStage == BROAD_POLICY_STAGE_EGRESS)
      {
        BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyOutPort);
        BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyPortClass);
      }
      break;

    default:
      break;
    }

    return BCM_E_NONE;
}

static int _policy_group_find_group(int                   unit,
                                    BROAD_POLICY_STAGE_t  policyStage,
                                    BROAD_POLICY_ENTRY_t *entryPtr,
                                    BROAD_GROUP_t        *group,
                                    bcm_field_qset_t      policyQset,
                                    custom_field_qset_t   customQset)
{
    int rv;
    int r, countersReq = 0, metersReq = 0;
    int groupEfpUsingIfp, policyEfpUsingIfp;

    /* Determine how many counters/meters are required for the policy. */
    for (r = 0; r < entryPtr->ruleCount; r++)
    {
        if (entryPtr->ruleInfo[r].ruleFlags & BROAD_METER_SPECIFIED)
        {
            /* meters use a counter as well */
            countersReq++;
            metersReq++;
        }
        else if (entryPtr->ruleInfo[r].ruleFlags & BROAD_COUNTER_SPECIFIED)
        {
            countersReq++;        
        }
    }

    /* Find an existing group that can satisfy the policy requirements. */
    rv = _policy_group_find_first(unit, policyStage, entryPtr->policyType, group);
    while (BCM_E_NONE == rv)
    {
        bcm_field_qset_t         qset;
        group_table_t           *groupPtr;

        groupPtr = &group_table[unit][policyStage][*group];

        if (groupPtr->flags & GROUP_USED)
        {
            if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
                printf("- examining group %d ", *group);

            /* Insure the group has a suitable qset and enough free entries, counters, et al. */
            BCM_FIELD_QSET_INIT(qset);
            _policy_set_union(super_qset_table[unit][groupPtr->sqset].qsetAgg, &qset);
            _policy_set_union(super_qset_table[unit][groupPtr->sqset].qsetUdf, &qset);

            rv = _policy_set_subset(policyQset, customQset, qset, super_qset_table[unit][groupPtr->sqset].customQset);

            // PTin modified: i cannot configure the priority of the last rule of each group!
            // As a temporary solution i reduced the total number of rules per group from 128 to 127 (groupPtr->availableRules-1)
            if ((BCM_E_NONE == rv) &&
                ((groupPtr->availableRules/*-1*/) >= entryPtr->ruleCount))
            {
                /* ensure that policies using IFP for EFP only go in groups using IFP for EFP */
                groupEfpUsingIfp  = (groupPtr->flags & GROUP_EFP_ON_IFP)                     ? L7_TRUE : L7_FALSE;
                policyEfpUsingIfp = (entryPtr->policyFlags & BROAD_POLICY_EGRESS_ON_INGRESS) ? L7_TRUE : L7_FALSE;

                if (groupEfpUsingIfp == policyEfpUsingIfp)
                {
                  /* reuse existing group */
                  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
                      printf("- reuse group %d\n", *group);

                  return BCM_E_NONE;
                }
            }
        }

        rv = _policy_group_find_next(unit, policyStage, entryPtr->policyType, group);
    }

    return rv;
}

static int _policy_group_alloc_group(int                   unit,
                                     BROAD_POLICY_STAGE_t  policyStage,
                                     BROAD_POLICY_ENTRY_t *entryPtr,
                                     BROAD_GROUP_t        *group,
                                     bcm_field_qset_t      policyQset,
                                     custom_field_qset_t   customQset)
{
    int  rv;
    int  sqset;
    int prio;
    group_table_t *groupPtr;
    group_table_t *tempGroupPtr;
    bcm_field_group_mode_t mode;
    int slices_needed;
    int i;

    /* make sure that only one group can do EFP on IFP */
    if (entryPtr->policyFlags & BROAD_POLICY_EGRESS_ON_INGRESS)
    {
      for (i = 0; i < group_table_size[unit][policyStage]; i++)
      {
        if ((group_table[unit][policyStage][i].flags & (GROUP_USED | GROUP_EFP_ON_IFP)) == (GROUP_USED | GROUP_EFP_ON_IFP))
        {
          return BCM_E_FAIL;
        }
      }
    }

//  printf("%s(%d) Requested:\r\n", __FUNCTION__, __LINE__);
//  int k;
//  for (k=0; k<_SHR_BITDCLSIZE(BCM_FIELD_QUALIFY_MAX); k++)
//  {
//      printf("%08X ",policyQset.w[k]);
//  }
//  printf("\r\n");
//  for (k=0; k<_SHR_BITDCLSIZE(BCM_FIELD_USER_NUM_UDFS); k++)
//  {
//      printf("%08X ",policyQset.udf_map[k]);
//  }
//  printf("\r\n");


    /* create all new groups based upon a super qset */
    rv = _policy_super_qset_find_match(unit, policyStage, entryPtr->policyType, policyQset, customQset, &sqset);
    if (BCM_E_NONE != rv)  {
        return rv;
    }

    slices_needed = _policy_sqset_slice_count(unit, sqset);

    /* allocate a new group of the appropriate type */
    rv = _policy_group_find_free_group(unit, policyStage, entryPtr->policyType, group, slices_needed);
    if (BCM_E_NONE != rv)  {
      return rv;
    }

    groupPtr = &group_table[unit][policyStage][*group];
    groupPtr->availableRules = super_qset_table[unit][sqset].rulesAvailable;

        /* populate group table */
    for (i = *group; i < (*group + slices_needed); i++)
    {
      tempGroupPtr = &group_table[unit][policyStage][i];

      tempGroupPtr->flags = GROUP_USED;
      if (entryPtr->policyFlags & BROAD_POLICY_EGRESS_ON_INGRESS)
      {
        tempGroupPtr->flags |= GROUP_EFP_ON_IFP;
      }
      tempGroupPtr->type  = entryPtr->policyType;
      tempGroupPtr->sqset = sqset;
      tempGroupPtr->base_group = *group;
    }

    prio = *group;   /* equate prio to group */

    if (super_qset_table[unit][sqset].flags & SUPER_QSET_SINGLE_WIDE)
    {
      mode = bcmFieldGroupModeSingle;
    }
    else if (super_qset_table[unit][sqset].flags & SUPER_QSET_DOUBLE_WIDE)
    {
      mode = bcmFieldGroupModeDouble;
    }
    else if (super_qset_table[unit][sqset].flags & SUPER_QSET_TRIPLE_WIDE)
    {
      mode = bcmFieldGroupModeTriple;
    }
    else
    {
      /* handles quadwide mode */
      mode = bcmFieldGroupModeAuto;
    }

//  printf("%s(%d) Contents:\r\n", __FUNCTION__, __LINE__);
//  //int k;
//  for (k=0; k<_SHR_BITDCLSIZE(BCM_FIELD_QUALIFY_MAX); k++)
//  {
//      printf("%08X ",super_qset_table[unit][sqset].qsetAgg.w[k]);
//  }
//  printf("\r\n");
//  for (k=0; k<_SHR_BITDCLSIZE(BCM_FIELD_USER_NUM_UDFS); k++)
//  {
//      printf("%08X ",super_qset_table[unit][sqset].qsetAgg.udf_map[k]);
//  }
//  printf("\r\n");

    rv = bcm_field_group_create_mode(unit, super_qset_table[unit][sqset].qsetAgg,
                                     BCM_FIELD_GROUP_PRIO_ANY /*prio*/, mode, &groupPtr->gid);
    if (rv != BCM_E_NONE)
    {
      for (i = *group; i < (*group + slices_needed); i++)
      {
        tempGroupPtr = &group_table[unit][policyStage][i];

        tempGroupPtr->flags      = GROUP_NONE;
        tempGroupPtr->gid        = BROAD_GROUP_INVALID;
        tempGroupPtr->base_group = BROAD_GROUP_INVALID;
      }
    }

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
        printf("bcm_field_group_create() returned %d\n", rv);

    return rv;
}

static int _policy_group_delete_group(int unit, BROAD_POLICY_STAGE_t policyStage, BROAD_GROUP_t group)
{
    group_table_t *groupPtr;
    group_table_t *tempGroupPtr;
    int            tempGroup;

    tempGroup = group;

    groupPtr     = &group_table[unit][policyStage][group];
    tempGroupPtr = &group_table[unit][policyStage][tempGroup];

    do
    {
      tempGroupPtr->flags      = GROUP_NONE;
      tempGroupPtr->gid        = BROAD_GROUP_INVALID;
      tempGroupPtr->base_group = BROAD_GROUP_INVALID;
      tempGroupPtr->availableRules = 0;

      tempGroup++;
      if (tempGroup >= group_table_size[unit][policyStage])
      {
        break;
      }
      tempGroupPtr = &group_table[unit][policyStage][tempGroup];

    } while (tempGroupPtr->base_group == group);


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
        if((BCM_E_NOT_FOUND==rv)||(BCM_E_PARAM==rv)||(SOC_IS_RAPTOR(unit)))
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
                rv = (((SOC_IS_RAPTOR(unit)||SOC_IS_RAVEN(unit)))? BCM_E_NONE : BCM_E_NOT_FOUND);
                break;
          }
        }
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
          rv = bcm_field_qualify_InnerVlan(unit, eid, tempVlan, tempVlanMask);
          // PTin removed
//        if ((BCM_E_NONE == rv) && ((tempVlan != 0) || (tempVlanMask != 0)))
//        {
//          rv = bcm_field_qualify_VlanFormat(unit, eid, BCM_FIELD_PKT_FMT_INNER_TAGGED, BCM_FIELD_PKT_FMT_INNER_TAGGED);
//        }
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
    case BROAD_FIELD_CLASS_ID:
        rv = bcm_field_qualify_LookupClass0(unit, eid, *((uint8*)value), 0xF);
        break;
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
    // PTin added
    case BROAD_FIELD_INPORTS:
        rv = bcm_field_qualify_InPorts(unit, eid, *((bcm_pbmp_t *) value), *((bcm_pbmp_t *) mask));
        break;
    case BROAD_FIELD_DROP:
        rv = bcm_field_qualify_Drop(unit,eid,*((uint8*)value),1);
        break;
    case BROAD_FIELD_L2_STATION_MOVE:
        rv = bcm_field_qualify_L2StationMove(unit,eid,*((uint8*)value),1);
        break;
    case BROAD_FIELD_L2_SRC_HIT:
        rv = bcm_field_qualify_L2SrcHit(unit,eid,*((uint8*)value),1);
        break;
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

  if (SOC_IS_ENDURO(unit))
  {
    if (mask & BCM_FIELD_LOOKUP_L3_TUN_HIT)
    {
      value = (data & BCM_FIELD_LOOKUP_L3_TUN_HIT) ? 0x1 : 0x0;
      BCM_IF_ERROR_RETURN
          (bcm_field_qualify_MplsTerminated(unit, entry, value, 0x1));
    }
  }
  else 
  {
    if (mask & BCM_FIELD_LOOKUP_L3_TUN_HIT)
    {
      value = (data & BCM_FIELD_LOOKUP_L3_TUN_HIT) ? 0x1 : 0x0;
      BCM_IF_ERROR_RETURN
          (bcm_field_qualify_TunnelTerminated(unit, entry, value, 0x1));
    }
 
    if ((mask & BCM_FIELD_LOOKUP_L3_ROUTABLE)) 
    {
      value = (data & BCM_FIELD_LOOKUP_L3_ROUTABLE) ? 0x1 : 0x0;
      BCM_IF_ERROR_RETURN
          (bcm_field_qualify_L3Routable(unit, entry, value, 0x1));
    }
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
    /* determine if this field is part of the UDF portion of the qset */
    return (BCM_FIELD_QSET_TEST(super_qset_table[unit][groupPtr->sqset].qsetUdf, field_map[field]) ||
            BCM_FIELD_QSET_TEST(super_qset_table[unit][groupPtr->sqset].customQset, field_map[field]));
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

static void _policy_group_get_action_parms(BROAD_ACTION_ENTRY_t       *actionPtr, 
                                           BROAD_POLICY_ACTION_t       action, 
                                           BROAD_POLICY_ACTION_SCOPE_t action_scope,
                                           L7_uint32                  *param0, 
                                           L7_uint32                  *param1, 
                                           L7_uint32                  *param2)
{
  *param0 = 0;
  *param1 = 0;
  *param2 = 0;

  switch (action)
  {
  case BROAD_ACTION_REDIRECT:
  case BROAD_ACTION_MIRROR:
    *param0 = actionPtr->parms.modid;
    *param1 = actionPtr->parms.modport;
    break;

  /* PTin added */
  case BROAD_ACTION_REDIRECT_TRUNK:
    *param0 = actionPtr->parms.trunkid;
    break;

  case BROAD_ACTION_TRAP_TO_CPU:
  case BROAD_ACTION_COPY_TO_CPU:
    *param0 = actionPtr->parms.cpu_modid;
    *param1 = actionPtr->parms.cpu_modport;
    break;

  case BROAD_ACTION_SET_COSQ:
    *param0 = actionPtr->parms.set_cosq[action_scope];
    break;

  case BROAD_ACTION_SET_DSCP:
    *param0 = actionPtr->parms.set_dscp[action_scope];
    break;

  case BROAD_ACTION_SET_TOS:
    *param0 = actionPtr->parms.set_tos[action_scope];
    break;

  case BROAD_ACTION_SET_USERPRIO:
    *param0 = actionPtr->parms.set_userprio[action_scope];
    break;

  case BROAD_ACTION_SET_DROPPREC:
    *param0 = actionPtr->parms.set_dropprec.conforming;
    *param1 = actionPtr->parms.set_dropprec.exceeding;
    *param2 = actionPtr->parms.set_dropprec.nonconforming;
    break;

  case BROAD_ACTION_SET_OUTER_VID:
    *param0 = actionPtr->parms.set_ovid;
    break;

  case BROAD_ACTION_SET_INNER_VID:
    *param0 = actionPtr->parms.set_ivid;
    break;

  case BROAD_ACTION_ADD_OUTER_VID:
    *param0 = actionPtr->parms.add_ovid;
    break;

  case BROAD_ACTION_SET_CLASS_ID:
    *param0 = actionPtr->parms.set_class_id;
    break;

  /* PTin added */
  case BROAD_ACTION_SET_REASON_CODE:
    *param0 = actionPtr->parms.set_reason;
    break;

  case BROAD_ACTION_SET_COSQ_AS_PKTPRIO: /* PTin added */
  default:
    break;
  }
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
        if (BROAD_ACTION_REDIRECT == action)
        {
          if (BCMX_LPORT_INVALID == actPtr->parms.modid)
          {
            bcm_action = bcmFieldActionDrop;
          }
        }

        _policy_group_get_action_parms(actPtr, action, action_scope, &param0, &param1, &param2);

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
               ((SOC_IS_RAPTOR(unit)||SOC_IS_RAVEN(unit)|| SOC_IS_HAWKEYE(unit)) && (BCM_E_PARAM == rv)))
            {
              rv = BCM_E_NONE;
            }
            /* Otherwise log the error */
            else if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
            {
              printf("Add lookup status to redirect/mirror action FAILED with ret val = %d\n", rv);
            }
          }
        }
      }
    }
  }
  return rv;
}

static int _policy_group_add_meter(int unit, BROAD_POLICY_STAGE_t stage, bcm_field_entry_t eid, BROAD_POLICY_RULE_ENTRY_t *rulePtr)
{
    int                  rv = BCM_E_NONE;
    uint64               zero64;
    BROAD_METER_ENTRY_t *meterPtr;
    uint32               counter_flags;

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

  if (_policy_supports_intraslice_doublewide_mode(unit) && !SOC_IS_SCORPION(unit))
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
    /* System rules in slices 0 through 1 */
    groupid_rev = 2;
#endif
  }

  if (hapiBroadBcmGroupRequired(unit))
  {
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

      /* PTin added: Policies */
      group_alloc_table[unit][policyStage][ALLOC_BLOCK_SYS_EXT].lowPrio     = 0;
      group_alloc_table[unit][policyStage][ALLOC_BLOCK_SYS_EXT].highPrio    = groups - 1;

      break;
    case BROAD_POLICY_STAGE_INGRESS:
      if (SOC_IS_RAPTOR(unit) || SOC_IS_HAWKEYE(unit))
      { 
        int  lowPrio = 0;
        /* Assumption is the each group contains 128 rules */
 #ifdef L7_ROUTING_PACKAGE
      /* If it is routing package and silicon is Raptor */
        lowPrio = ((L7_L3_ROUTE_TBL_SIZE_TOTAL + L7_L3_ARP_CACHE_SIZE)/ 127);
        lowPrio +=(((L7_L3_ROUTE_TBL_SIZE_TOTAL+ L7_L3_ARP_CACHE_SIZE)  % 127) > 0 ? 1 : 0);
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

        /* PTin added: Policies */
        group_alloc_table[unit][policyStage][ALLOC_BLOCK_SYS_EXT].lowPrio  = lowPrio + 2;
        group_alloc_table[unit][policyStage][ALLOC_BLOCK_SYS_EXT].highPrio = groups - 1;
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
        printf("ALLOC_BLOCK_LOW    : Groups %u - %u\r\n",
               group_alloc_table[unit][policyStage][ALLOC_BLOCK_LOW].lowPrio,
               group_alloc_table[unit][policyStage][ALLOC_BLOCK_LOW].highPrio);

        group_alloc_table[unit][policyStage][ALLOC_BLOCK_MEDIUM].lowPrio  = lowPrioGroup;
        group_alloc_table[unit][policyStage][ALLOC_BLOCK_MEDIUM].highPrio = groups - 1;

        printf("ALLOC_BLOCK_MEDIUM : Groups %u - %u\r\n",
               group_alloc_table[unit][policyStage][ALLOC_BLOCK_MEDIUM].lowPrio,
               group_alloc_table[unit][policyStage][ALLOC_BLOCK_MEDIUM].highPrio);

        group_alloc_table[unit][policyStage][ALLOC_BLOCK_HIGH].lowPrio    = lowPrioGroup;
        group_alloc_table[unit][policyStage][ALLOC_BLOCK_HIGH].highPrio   = groups - 1;

        printf("ALLOC_BLOCK_HIGH   : Groups %u - %u\r\n",
               group_alloc_table[unit][policyStage][ALLOC_BLOCK_HIGH].lowPrio,
               group_alloc_table[unit][policyStage][ALLOC_BLOCK_HIGH].highPrio);

        /* PTin added: Policies */
        group_alloc_table[unit][policyStage][ALLOC_BLOCK_SYS_EXT].lowPrio  = lowPrioGroup;
        group_alloc_table[unit][policyStage][ALLOC_BLOCK_SYS_EXT].highPrio = groups - 1;

        printf("ALLOC_BLOCK_SYS_EXT: Groups %u - %u\r\n",
               group_alloc_table[unit][policyStage][ALLOC_BLOCK_SYS_EXT].lowPrio,
               group_alloc_table[unit][policyStage][ALLOC_BLOCK_SYS_EXT].highPrio);
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

static int _policy_group_total_slices(int unit, BROAD_POLICY_STAGE_t policyStage)
{
    int          total_slices = 0;

    switch (policyStage)
    {
    case BROAD_POLICY_STAGE_LOOKUP:
      // PTin added: SOC_IS_VALKYRIE2(unit)
      if (SOC_IS_FIREBOLT2(unit) ||
          SOC_IS_TR_VL(unit) ||
          SOC_IS_SCORPION(unit) || SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || SOC_IS_VALKYRIE2(unit)) 
        total_slices = 4;
      else  
        total_slices = 0;
      break;
    case BROAD_POLICY_STAGE_INGRESS:
      // PTin added: SOC_IS_VALKYRIE2(unit)
      if (SOC_IS_FIREBOLT(unit)  || 
          SOC_IS_FIREBOLT2(unit) ||
          SOC_IS_TR_VL(unit) ||
          SOC_IS_RAVEN(unit) || SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || SOC_IS_VALKYRIE2(unit)) 
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
      else
      {
        total_slices = 6;     // PTin modified : original value was 8!
      }
      break;
    case BROAD_POLICY_STAGE_EGRESS:
      // PTin added: SOC_IS_VALKYRIE2(unit)
      if (SOC_IS_FIREBOLT2(unit) ||
          SOC_IS_TR_VL(unit) ||
          SOC_IS_SCORPION(unit) || SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || SOC_IS_VALKYRIE2(unit)) 
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
    // PTin added: SOC_IS_VALKYRIE2(unit)
    if (SOC_IS_FIREBOLT2(unit) ||
        SOC_IS_TR_VL(unit) ||
        SOC_IS_SCORPION(unit) || SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || SOC_IS_VALKYRIE2(unit))
    {
      supported = L7_TRUE;
    }
    break;

  case BROAD_POLICY_STAGE_INGRESS:
    supported = L7_TRUE;
    break;

  case BROAD_POLICY_STAGE_EGRESS:
    // PTin added: SOC_IS_VALKYRIE2(unit)
    if (SOC_IS_FIREBOLT2(unit) ||
        SOC_IS_TR_VL(unit) ||
        SOC_IS_SCORPION(unit) || SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || SOC_IS_VALKYRIE2(unit))
    {
      supported = L7_TRUE;
    }
    break;

  default: 
    break;
  }

  return supported;
}

// PTin added: to get available rules
int policy_group_available_rules(int unit, int group_id)
{
    // Validate group_id
    if (unit!=0 || group_id>=_policy_group_total_slices(unit,BROAD_POLICY_STAGE_INGRESS))
        return 0;

    // Return available rules
    return group_table[unit][BROAD_POLICY_STAGE_INGRESS][group_id].availableRules;
}
// PTin end

int policy_group_init(int unit)
{
    int                  rv;
    int                  total_slices;
    BROAD_POLICY_STAGE_t policyStage;

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        printf("policy_group_init(%d)\n", unit);

    for (policyStage = BROAD_POLICY_STAGE_LOOKUP; policyStage < BROAD_POLICY_STAGE_COUNT; policyStage++)
    {
      total_slices = _policy_group_total_slices(unit, policyStage);

      rv = _policy_group_alloc_init(unit, policyStage, total_slices);
      if (BCM_E_NONE != rv)
      {
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
            printf("_policy_group_alloc_init failed (%d)\n", rv);
        return rv;
      }

      rv = _policy_group_table_init(unit, policyStage, total_slices);
      if (BCM_E_NONE != rv)
      {
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
            printf("_policy_group_table_init failed (%d)\n", rv);
        return rv;
      }
    }

    rv = _policy_udf_init(unit);
    if (BCM_E_NONE != rv)
    {
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          printf("_policy_udf_init failed (%d)\n", rv);
      return rv;
    }

    rv = _policy_super_qset_init(unit);
    if (BCM_E_NONE != rv)
    {
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          printf("_policy_super_qset_init failed (%d)\n", rv);
      return rv;
    }

    rv = _policy_action_map_init(unit);
    if (BCM_E_NONE != rv)
    {
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          printf("_policy_action_map_init failed (%d)\n", rv);
      return rv;
    }

    return rv;
}

int policy_group_create(int unit, BROAD_POLICY_STAGE_t policyStage, BROAD_POLICY_ENTRY_t *policyData, BROAD_GROUP_t *group)
{
    int                 rv;
    bcm_field_qset_t    policyQset;
    custom_field_qset_t customQset;
    int                 req_ethtype = 0;
    L7_BOOL             reworkQset;
    bcm_field_qualify_t reworkQualifier = bcmFieldQualifyIpType;

    if (policy_stage_supported(unit, policyStage) == L7_FALSE)
    {
      return BCM_E_UNAVAIL;
    }

    rv = _policy_group_calc_qset(unit, policyStage, policyData, &policyQset, &customQset, &req_ethtype);
    if (BCM_E_NONE != rv)  {
        return rv;
    }

    rv = _policy_group_find_group(unit, policyStage, policyData, group, policyQset, customQset);

    if (BCM_E_NONE != rv)
    {
      /* see if there is an alternative qset already created */
      reworkQset = L7_FALSE;
      if ((req_ethtype == 0)  && 
          BCM_FIELD_QSET_TEST(policyQset, bcmFieldQualifyIpType))
      {
        reworkQset      = L7_TRUE;
        reworkQualifier = bcmFieldQualifyIpType;
      }
      else if ((req_ethtype == 0)  && 
          BCM_FIELD_QSET_TEST(policyQset, bcmFieldQualifyIp4))
      {
        reworkQset      = L7_TRUE;
        reworkQualifier = bcmFieldQualifyIp4;
      }
      else if ((req_ethtype == 0)  && 
          BCM_FIELD_QSET_TEST(policyQset, bcmFieldQualifyIp6))
      {
        reworkQset      = L7_TRUE;
        reworkQualifier = bcmFieldQualifyIp6;
      }

      if (reworkQset == L7_TRUE)
      {
        BCM_FIELD_QSET_REMOVE(policyQset, reworkQualifier);
        BCM_FIELD_QSET_ADD(policyQset, bcmFieldQualifyEtherType);
        
        rv = _policy_group_find_group(unit, policyStage, policyData, group, policyQset, customQset);

        if (BCM_E_NONE != rv)
        {
          BCM_FIELD_QSET_ADD(policyQset, reworkQualifier);
          BCM_FIELD_QSET_REMOVE(policyQset, bcmFieldQualifyEtherType);
        }
      }
    }
    
    if (BCM_E_NONE != rv)
    {
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
            printf("- allocate new group\n");

        rv = _policy_group_alloc_group(unit, policyStage, policyData, group, policyQset, customQset);

        if (BCM_E_NONE != rv)
        {
            if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
                printf("- allocate new group FAILED\n");

            reworkQset = L7_FALSE;
            if ((req_ethtype == 0)  && 
                BCM_FIELD_QSET_TEST(policyQset, bcmFieldQualifyIpType))
            {
              reworkQset      = L7_TRUE;
              reworkQualifier = bcmFieldQualifyIpType;
            }
            else if ((req_ethtype == 0)  && 
                BCM_FIELD_QSET_TEST(policyQset, bcmFieldQualifyIp4))
            {
              reworkQset      = L7_TRUE;
              reworkQualifier = bcmFieldQualifyIp4;
            }
            else if ((req_ethtype == 0)  && 
                BCM_FIELD_QSET_TEST(policyQset, bcmFieldQualifyIp6))
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
              BCM_FIELD_QSET_REMOVE(policyQset, reworkQualifier);
              BCM_FIELD_QSET_ADD(policyQset, bcmFieldQualifyEtherType);

                  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
                    printf("- allocate new group\n");

                  rv = _policy_group_alloc_group(unit, policyStage, policyData, group, policyQset, customQset);

                  if (BCM_E_NONE != rv)
                  {
                    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
                      printf("- allocate new group FAILED\n");
                    return rv;
                  }
            }
            else
            {
              /* can't rework the qset, fail */
              return rv;
            }
        }
    }

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
        printf("- using group %d\n", *group);

    return rv;
}

int policy_group_add_rule(int                        unit,
                          BROAD_POLICY_STAGE_t       policyStage,
                          BROAD_POLICY_TYPE_t        policyType,
                          BROAD_GROUP_t              group,
                          BROAD_POLICY_RULE_ENTRY_t *rulePtr,
                          bcm_pbmp_t                 pbm,
                          int                        skip_actions,
                          BROAD_ENTRY_t             *entry)
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
    if (BCM_E_NONE != rv)
    {
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
            printf("- bcm_field_entry_create rv = %d\n", rv);
        return rv;
    }

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
               printf("- _policy_group_add_field f=%d rv = %d\n", f, rv);
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
              printf("- adding a meter\n");

          rv = _policy_group_add_meter(unit, policyStage, eid, rulePtr);
          if (BCM_E_NONE != rv)
              return rv;
      }
      else if (rulePtr->ruleFlags & BROAD_COUNTER_SPECIFIED)
      {
          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
              printf("- adding a counter\n");

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
      if ( rv != BCM_E_NONE)
      {  
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
            printf("- bcm_field_entry_install rv = %d\n", rv);
      }
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
        printf("%s: adding eid=%d   ", __FUNCTION__,eid);
    }

    if (rv == BCM_E_NONE)
    {
      groupPtr->availableRules--;
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

    CHECK_GROUP(unit,policyStage,group);

    groupPtr = &group_table[unit][policyStage][group];

    /* Without wide mode, VLAN ACLs don't support IPBM due to qset limitations, so quietly ignore */
    if (((BROAD_POLICY_TYPE_VLAN == groupPtr->type) || (BROAD_POLICY_TYPE_ISCSI == groupPtr->type)) && !_policy_supports_wide_mode(unit))
    {
        return BCM_E_NONE;
    }

    eid = BROAD_ENTRY_TO_BCM_ENTRY(entry);

    /* Reinstall only works for actions, so remove and install the entry explicitly. */
    rv = bcm_field_entry_remove(unit, eid);
    if (BCM_E_NONE != rv)
        return rv;

    rv = bcm_field_qualify_InPorts(unit, eid, pbm, PBMP_PORT_ALL(unit));
    if (BCM_E_NONE != rv)
        return rv;

    rv = bcm_field_entry_install(unit, eid);
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
    unsigned int      classMask = 0x0F;
    unsigned int      portMask  = 0x1F;
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
        rv = bcm_field_qualify_PortClass(unit, eid, portClass, classMask);
        if (BCM_E_NONE != rv)
            return rv;
      }

      rv = bcm_field_entry_install(unit, eid);
      if (BCM_E_NONE != rv)
          return rv;
    }

    return rv;
}

int policy_port_class_set(int                  unit,
                          bcm_port_t           port,
                          BROAD_POLICY_STAGE_t policyStage,
                          unsigned char        portClass)
{
    int               rv = BCM_E_NONE;

    if (policyStage == BROAD_POLICY_STAGE_LOOKUP)
    {
      rv = bcm_port_class_set(unit, port, bcmPortClassFieldLookup, portClass);
      if (BCM_E_NONE != rv)
          return rv;
    }
    else if (policyStage == BROAD_POLICY_STAGE_EGRESS)
    {
      rv = bcm_port_class_set(unit, port, bcmPortClassFieldEgress, portClass);
      if (BCM_E_NONE != rv)
          return rv;
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
    {
        return BCM_E_NOT_FOUND;
    }

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
    // PTin flag: there is a nasty bug when defining the priority of the last rule of each group!
    //printf("%s(%d) rv=%d, eid=%d, prio=%d\r\n",__FUNCTION__,__LINE__,rv,eid,prio);
    if (rv != BCM_E_NONE)
    {
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          printf("- bcm_field_entry_prio_set rv = %d\n", rv);
    }
    return rv;
}

int policy_group_delete_rule(int                  unit,
                             BROAD_POLICY_STAGE_t policyStage,
                             BROAD_GROUP_t        group,
                             BROAD_ENTRY_t        entry)
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
    /* If rv is BCM_E_UNAVAIL, it is possible that we are trying to remove
     * a rule that is not installed in the hardware. So, continue with the
     * destroy. */ 
    if((BCM_E_UNAVAIL != rv) && (BCM_E_NONE != rv))
        return rv;

    rv = bcm_field_entry_destroy(unit, eid);
    if (BCM_E_NONE != rv)
        return rv;
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
      printf("%s: removing eid=%d   ",__FUNCTION__,eid);

    groupPtr->availableRules++;

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
        printf("- destroy group %d, gid %d\n", group, groupPtr->gid);

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

    rv = bcm_field_counter_get(unit, eid, 0, val1);
    if ((BCM_E_NONE != rv) && (BCM_E_EMPTY != rv))   /* empty means no counter */
        return rv;

    if (policyStage != BROAD_POLICY_STAGE_EGRESS)
    {
      rv = bcm_field_counter_get(unit, eid, 1, val2);  /* empty means no counter */
      if ((BCM_E_NONE != rv) && (BCM_E_EMPTY != rv))
          return rv;
    }

    return BCM_E_NONE;
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
    if (BCM_E_NONE != rv)
      return rv;

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
    if (BCM_E_NONE != rv)
      return rv;

    groupPtr->availableRules--;

    rv = bcm_field_entry_prio_set(unit, eid, BCM_FIELD_ENTRY_PRIO_LOWEST);

    return rv;


}

/* Debug */

void debug_group_stats(int unit)
{
    int rv, i;
    bcm_field_status_t       stats;
    bcm_field_group_status_t gstats;

    printf("\nUnit %d Stats\n", unit);

    rv = bcm_field_status_get(unit, &stats);
    if (BCM_E_NONE == rv)
        printf("Groups free %d, total %d", stats.group_free, stats.group_total);
    else
    {
        printf("*** Error reading group stats ***");
        return;
    }

    printf("\n\n");

    printf("Group  Prio   Entries   Counters   Meters\n");
    for (i = 1; i <= stats.group_total; i++)
    {
        printf("[%2d]   ", i);

        rv = bcm_field_group_status_get(unit, i, &gstats);
        if (BCM_E_NONE == rv)
            printf("[%d..%d] %d/%d %d/%d %d/%d",
                   gstats.prio_min, gstats.prio_max,
                   gstats.entries_free, gstats.entries_total,
                   gstats.counters_free, gstats.counters_total,
                   gstats.meters_free, gstats.meters_total);
        else
            printf("unused (%d)", rv);
        printf("\n");
    }
}

void debug_group_table(int unit)
{
    int                  i;
    BROAD_POLICY_STAGE_t policyStage;

    printf("\nUnit %d\n", unit);

    for (policyStage = BROAD_POLICY_STAGE_LOOKUP; policyStage < BROAD_POLICY_STAGE_COUNT; policyStage++)
    {
      if (policy_stage_supported(unit, policyStage) == L7_TRUE)
      {
        switch (policyStage)
        {
        case BROAD_POLICY_STAGE_LOOKUP:
          printf("Stage: Lookup\n");
          break;
        case BROAD_POLICY_STAGE_INGRESS:
          printf("Stage: Ingress\n");
          break;
        case BROAD_POLICY_STAGE_EGRESS:
          printf("Stage: Egress\n");
          break;
        default:
          printf("Stage: Unknown!\n");
          break;
        }
        printf("Group   GID   Type   SQset   BaseGroup   availableRules\n");
        for (i = 0; i < group_table_size[unit][policyStage]; i++)
        {
            printf("[%2d]   ", i);

            if (group_table[unit][policyStage][i].flags & GROUP_USED)
            {
                printf("%4d    %2d    %2d      %2d          %4d", 
                       group_table[unit][policyStage][i].gid, group_table[unit][policyStage][i].type,
                       group_table[unit][policyStage][i].sqset, group_table[unit][policyStage][i].base_group,
                       group_table[unit][policyStage][i].availableRules);
            }
            else
            {
                printf("unused");
            }
            printf("\n");
        }
      }
    }
}

void debug_sqset_table(int unit)
{
    int i, j;

    for (i = 0; i < SUPER_QSET_TABLE_SIZE; i++)
    {
        printf("SQset %d", i);

        if (super_qset_table[unit][i].flags & SUPER_QSET_USED)
        {
            printf("\n- qsetAgg: ");
            for (j = 0; j < bcmFieldQualifyCount; j++)
                printf ("%d", (BCM_FIELD_QSET_TEST(super_qset_table[unit][i].qsetAgg, j) ? 1 : 0));
            printf("\n- qsetUdf: ");
            for (j = 0; j < bcmFieldQualifyCount; j++)
                printf ("%d", (BCM_FIELD_QSET_TEST(super_qset_table[unit][i].qsetUdf, j) ? 1 : 0));
            printf("\n- applicablePolicyTypes: 0x%8.8x\n", super_qset_table[unit][i].applicablePolicyTypes);
        }
        else
        {
            printf("unused");
        }
        printf("\n");
    }
}

void debug_entry_counter(int unit, bcm_field_entry_t eid)
{
    int    rv;
    uint64 val64;

    rv = bcm_field_counter_get(unit, eid, 0, &val64);
    if (BCM_E_NONE == rv)
        printf("Counter 0: %08x %08x\n", u64_H(val64), u64_L(val64));
    else
        printf("error code = %d\n", rv);

    rv = bcm_field_counter_get(unit, eid, 1, &val64);
    if (BCM_E_NONE == rv)
        printf("Counter 1: %08x %08x\n", u64_H(val64), u64_L(val64));
    else
        printf("error code = %d\n", rv);
}

int l7_bcm_dot1ad_vfp_group_alloc(int unit,
                                         BROAD_POLICY_STAGE_t  policyStage,
                                         BROAD_GROUP_t        *group)
{

    int  rv;
    int prio;
    bcm_field_group_mode_t mode;
    int slices_needed;
    bcm_field_qset_t  qsetAgg;
    int tmp;

    slices_needed = 1;

    /* allocate a new group of the appropriate type */
    rv = _policy_group_find_free_group(unit, 
                                       BROAD_POLICY_STAGE_LOOKUP, 
                                       BROAD_POLICY_TYPE_PORT, 
                                       group, 
                                       slices_needed);
    if (BCM_E_NONE != rv)
      return rv;


    prio = *group;   /* equate prio to group */
    mode = bcmFieldGroupModeSingle;

    BCM_FIELD_QSET_INIT(qsetAgg);

    for (tmp = 0; tmp < dot1adQsetLookupSize; tmp++)
    {
      BCM_FIELD_QSET_ADD(qsetAgg,dot1adQsetLookup[tmp]);
    }

    rv = bcm_field_group_create_mode(unit, 
                                     qsetAgg,
                                     prio, 
                                     mode, 
                                     group);

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
        printf("bcm_field_group_create() returned %d\n", rv);

    return rv;
}

int l7_bcm_dot1ad_vfp_rule_delete(int unit,
                                  int portGroupId)
{
  int rv = BCM_E_NONE;
  int ruleId = (~L7_BCM_DOT1AD_INVALID_RULE);

  do
  {
    ruleId = dot1ad_vfp_rule_id[unit][portGroupId];

    if(ruleId != L7_BCM_DOT1AD_INVALID_RULE) 
    {
      if (traceDot1adVfpRule)
      {
        L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_DRIVER_COMPONENT_ID,
            "VFP entry delete: Rule id  %d",ruleId);
      }

      rv = bcm_field_entry_remove (unit, ruleId);
      if (rv != BCM_E_NONE)
      {
         L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID,
                "VFP entry delete: Failed to delete the subscription. Returning with value %d",rv);
        break;
      }
      rv = bcm_field_entry_destroy(unit, ruleId);
      if ( rv != BCM_E_NONE)
      {
         L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID,
                "VFP entry delete: Failed to delete the subscription. Returning with value %d",rv);
      }
      break;
    }
    else
    {
      rv = BCM_E_PARAM;
      L7_LOG(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID,
             "VFP entry delete: Invalid parameters are specified");
      break;
    }
  }while(0);

  if ( rv == BCM_E_NONE)
  {
    dot1ad_vfp_rule_id[unit][portGroupId] = L7_BCM_DOT1AD_INVALID_RULE;
  }

  return rv;
}

int l7_bcm_dot1ad_vfp_rule_add(int unit, bcm_port_t port,
                               bcm_dot1ad_rule_entry_t *dot1adRule)
{
  int rv = BCM_E_NONE;
  int gid= L7_BCM_DOT1AD_INVALID_RULE;

  if (dot1adRule == L7_NULLPTR)
  {
    L7_LOG(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID,
           "VFP entry add: Subscription pointer is null");
    return BCM_E_PARAM;
  }

  do
  {
    if (traceDot1adVfpRule)
    {
     L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_DRIVER_COMPONENT_ID,
            "VFP entry add: Add request on unit %d,port %d",unit,port);
    }

    gid = dot1ad_vfp_group_id[unit];
    rv = bcm_field_entry_create(unit, gid, &(dot1adRule->vfpRuleId));
    if (rv != BCM_E_NONE)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID,
             "VFP entry add: Failed to create subscription rule. Returning with value %d",rv);
      break;
    }
    if (traceDot1adVfpRule)
    {
     L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_DRIVER_COMPONENT_ID,
            "VFP entry add: Newly installed subscription's id is %d",dot1adRule->vfpRuleId);
    }
    /* Add the match fileds to the Rule */
    if (hapiBroadPolicyFieldFlagsGet(&dot1adRule->fieldInfo, BROAD_FIELD_VLAN_FORMAT) == BROAD_FIELD_SPECIFIED)
    {
       rv = _policy_group_add_std_field(unit, BROAD_POLICY_STAGE_LOOKUP,
                                        dot1adRule->vfpRuleId,
                                        BROAD_FIELD_VLAN_FORMAT,
                                        (char *)dot1adRule->fieldInfo.fieldVlanFormat.value,
                                        (char *)dot1adRule->fieldInfo.fieldVlanFormat.value);
      if (rv != BCM_E_NONE)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID,
             "VFP entry add: Failed to add vlan format as qualifier. Returning with value %d",rv);
        break;
      }
    }
    if (hapiBroadPolicyFieldFlagsGet(&dot1adRule->fieldInfo, BROAD_FIELD_IVID) == BROAD_FIELD_SPECIFIED)
    {
       rv = _policy_group_add_std_field(unit, BROAD_POLICY_STAGE_LOOKUP,
                                        dot1adRule->vfpRuleId,
                                        BROAD_FIELD_IVID,
                                        (char *)&dot1adRule->fieldInfo.fieldIvid.value,
                                        (char *)&dot1adRule->fieldInfo.fieldIvid.mask);
      if (rv != BCM_E_NONE)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID,
             "VFP entry add: Failed to add inner tag as qualifier. Returning with return value %d",rv);
        break;
      }
    }
    if (hapiBroadPolicyFieldFlagsGet(&dot1adRule->fieldInfo, BROAD_FIELD_OVID) == BROAD_FIELD_SPECIFIED)
    {
       rv = _policy_group_add_std_field(unit, BROAD_POLICY_STAGE_LOOKUP,
                                        dot1adRule->vfpRuleId,
                                        BROAD_FIELD_OVID,
                                        (char *)&dot1adRule->fieldInfo.fieldOvid.value,
                                        (char *)&dot1adRule->fieldInfo.fieldOvid.mask);
      if (rv != BCM_E_NONE)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID,
             "VFP entry add: Failed to add outer tag as qualifier. Returing with return value %d",rv);
        break;
      }
    }
    /* Add actions */
    if(dot1adRule->upStreamLearn == L7_TRUE)
    {
       rv = bcm_field_action_add(unit, dot1adRule->vfpRuleId, bcmFieldActionDoNotLearn, L7_NULL, L7_NULL);
       if ( rv != BCM_E_NONE)
       {
         L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID,
             "VFP entry add: Failed to set the MAC learn Flag. Returning with value %d",rv);
         return BCM_E_NONE;
       }
    }

    /* Add port as Qualifier */
    if (rv == BCM_E_NONE)
    {
      rv = bcm_field_qualify_InPort(unit,dot1adRule->vfpRuleId,port,L7_BCM_DOT1AD_PORT_MASK);
      if (rv != BCM_E_NONE)
      {
         L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID,
             "VFP entry add: Failed to add port as qualifier. Returing with value %d",rv);
        break;
      }
    }
    /* Qualify Port class */
    rv = bcm_field_qualify_PortClass(unit,dot1adRule->vfpRuleId, L7_NULL, L7_NULL);
    if (rv != BCM_E_NONE)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID,
            "VFP entry add: Failed to add class id as qualifier. Returning with value %d",rv);
      break;
    }

    if (dot1adRule->actionInfo.parms.set_ivid > L7_NULL)
    {
      rv = bcm_field_action_add(unit, dot1adRule->vfpRuleId,
                                bcmFieldActionInnerVlanAdd,
                                dot1adRule->actionInfo.parms.set_ivid, L7_NULL);
      if (rv != BCM_E_NONE)
      {
         L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID,
             "VFP entry add: Failed to set the inner tag marking action. Returning with  value %d",rv);
        break;
      }
    }

    if (dot1adRule->actionInfo.parms.set_ovid > L7_NULL)
    {
      rv = bcm_field_action_add(unit, dot1adRule->vfpRuleId,
                                bcmFieldActionOuterVlanAdd,
                                dot1adRule->actionInfo.parms.set_ovid, L7_NULL);
      if (rv != BCM_E_NONE)
      {
         L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID,
             "VFP entry add: Failed to set the outer tag marking action. Returning with value %d",rv);
        break;
      }
    }
    /* Install the rule in HW */
     rv = bcm_field_entry_install(unit, dot1adRule->vfpRuleId);
     if (rv != BCM_E_NONE)
     {
       L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID,
             "VFP entry add: Failed to install the subscription rule. Returing with value %d",rv);
       break;
     }
  }while(0);

  if ( (rv != BCM_E_NONE) &&
       (dot1adRule->vfpRuleId > L7_NULL)
     )
  {
    rv = bcm_field_entry_remove (unit, dot1adRule->vfpRuleId);
    if (rv != BCM_E_NONE)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID,
             "VFP entry delete: Failed to delete the subscription rule. Returing with value %d",rv);
      return rv;
    }
    rv = bcm_field_entry_destroy(unit, dot1adRule->vfpRuleId);
    if ( rv != BCM_E_NONE)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID,
             "VFP entry delete: Failed to delete the subscription rule. Returning with value %d",rv);
      return rv;
    }
  }
  else
  {
    /* Store the DOT1AD rule id here. 
       This information will be useful for removing the entries */
    dot1ad_vfp_rule_id[unit][port] = dot1adRule->vfpRuleId;
  }
  return rv;
}

/* Debug Routine */
void hapiBroadDebugDot1adVfpStatus(int unit)
{
  L7_int32 tmp;

  sysapiPrintf ("VFP Resereved Group id -%d\r\n", dot1ad_vfp_group_id[unit]);
  for (tmp = 0; tmp <L7_MAX_PHYSICAL_PORTS_PER_SLOT; tmp++)
  {
    if (dot1ad_vfp_rule_id[unit][tmp] != L7_BCM_DOT1AD_INVALID_RULE)
    {
      sysapiPrintf ("VFP Rule id -%d-%d\r\n", tmp,dot1ad_vfp_rule_id[tmp]);
    }
  }
}
void hapiBroadDebugDot1adVfpRule(L7_BOOL enable)
{
 traceDot1adVfpRule = enable;
}




