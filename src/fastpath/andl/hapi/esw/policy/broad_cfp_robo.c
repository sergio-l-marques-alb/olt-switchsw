/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_cfp_robo.c
*
* This file implements XGS III specific policy code for the local unit.
*
* @component hapi
*
* @create    
*
* @author    
*
* @end
*
**********************************************************************/

#include "broad_cfp_robo.h"
#include "bcm/field.h"
#include "ibde.h"
#include "sal/core/libc.h"
#include "osapi_support.h"

/* PTin added: includes */
#if 1
#include "logger.h"
#endif

/* used for the Higig B0 workaround */
#include <soc/drv.h>
#include <bcm_int/control.h>

extern L7_int32 hpcBroadMasterCpuModPortGet(L7_int32 *modid, L7_int32 *cpuport);


#define MASK_NONE    (~0)
#define MASK_ALL     (0)

#define CHECK_UNIT(u)    {if (u >= SOC_MAX_NUM_DEVICES) LOG_ERROR(u);}

typedef enum custom_field_qualify_e 
{
    customFieldQualifyFirst = bcmFieldQualifyCount,
  /*customFieldQualifyUdf0,*/         /* User defined field */
    customFieldQualifyIcmpMsgType = customFieldQualifyFirst,    /* ICMP Message Type */
    customFieldQualifyLast            /* Must be last */
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
    bcmFieldQualifyLookupStatus,   /* LOOKUP_STATUS */ /*NOT SUPPORTED*/
    bcmFieldQualifySrcIp6,         /* Source IPv6 Address */
    bcmFieldQualifyDstIp6,         /* Destination IPv6 Address */ /*NOT SUPPORTED*/
    bcmFieldQualifyIp6FlowLabel,   /* IPv6 Flow Label */
    bcmFieldQualifyIp6TrafficClass,/* IPv6 Traffic Class */
    customFieldQualifyIcmpMsgType, /* ICMP Message Type   */
/* PTin modified: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
    bcmFieldQualifyDstClassField,  /* Class ID from VFP, to be used in IFP - PTin added: FP */
    bcmFieldQualifySrcClassField,  /* Class ID from VFP, to be used in IFP - PTin added: FP */
#else
    bcmFieldQualifyLookupClass0,   /* Class ID from VFP, to be used in IFP */
    bcmFieldQualifyLookupClass0,   /* PTin added: FP */
#endif
/* PTin modified: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
    bcmFieldQualifySrcClassL2,
#else
    bcmFieldQualifySrcMacGroup,     /* Class ID from L2X, to be used in IFP */
#endif
    0,                              /* iSCSI opcode: NOT SUPPORTED */
    0,                              /* iSCSI opcode, TCP options: NOT SUPPORTED */
    bcmFieldQualifyTcpControl,
    bcmFieldQualifyVlanFormat,     /* VLAN Format */
    bcmFieldQualifyIpType,         /* IP Type */
    bcmFieldQualifyInPorts,        /* InPorts, PTin added: FP */
    bcmFieldQualifyOutPorts,       /* OutPorts, PTin added: FP */
    bcmFieldQualifySrcTrunk,       /* SrcTrunk, PTin added: FP */
    bcmFieldQualifyDstTrunk,       /* DstTrunk, PTin added: FP */
    bcmFieldQualifyDrop,           /* Drop, PTin added: FP */
    bcmFieldQualifyL2SrcHit,       /* L2 Source hit, PTin added: FP */
    bcmFieldQualifyL2DestHit,      /* L2 Destination hit, PTin added: FP */
    bcmFieldQualifyIntPriority     /* Internal priority, PTin added: FP */
};

typedef struct custom_field_qset_t {
    SHR_BITDCL w[_SHR_BITDCLSIZE(customFieldQualifyLast-customFieldQualifyFirst)]; /* custom field bitmap */
} custom_field_qset_t;

#define CUSTOM_FIELD_QSET_INIT(qset) \
    sal_memset(&(qset), 0, sizeof(custom_field_qset_t))

#define CUSTOM_FIELD_QSET_ADD(qset, q)    \
    if ((q >= customFieldQualifyFirst) && (q < customFieldQualifyLast))       \
        {SHR_BITSET(((qset).w), (q - customFieldQualifyFirst));}

#define CUSTOM_FIELD_QSET_REMOVE(qset, q) \
    if ((q >= customFieldQualifyFirst) && (q < customFieldQualifyLast))       \
        {SHR_BITCLR(((qset).w), (q - customFieldQualifyFirst));}

#define CUSTOM_FIELD_QSET_TEST(qset, q)    (((q >= customFieldQualifyFirst) && (q < customFieldQualifyLast)) ? \
                                                  SHR_BITGET(((qset).w), (q - customFieldQualifyFirst)) : 0)

/* Action Map */
/* This table maps the BROAD_ACTION_t enum to BCM API types. Keep in mind that the same
 * params (0/1/2) are passed to all actions, so they must have compatible params or be
 * specified in separate actions.
 */

#define PROFILE_ACTION_INVALID    bcmFieldActionCount

#define ACTIONS_PER_MAP_ENTRY     4

typedef struct
{
    bcm_field_action_t green[ACTIONS_PER_MAP_ENTRY];
    bcm_field_action_t yellow[ACTIONS_PER_MAP_ENTRY];
    bcm_field_action_t red[ACTIONS_PER_MAP_ENTRY];
}
action_map_entry_t;


static action_map_entry_t ingress_action_map[BROAD_ACTION_LAST] =
{
    /* SOFT_DROP - do not switch */
    {
        { bcmFieldActionGpDrop, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { bcmFieldActionRpDrop, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* HARD_DROP - override all other rules */
    {
        { bcmFieldActionGpDrop,   PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID , PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { bcmFieldActionRpDrop, PROFILE_ACTION_INVALID , PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* PERMIT - default behavior */
    /* DropCancel actions are not supported on Robo platforms. Leaving permit rule with no action will result
        in packet following normal ARL path (permit).*/
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* REDIRECT */
    {
        { bcmFieldActionGpRedirectPort, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { bcmFieldActionRpRedirectPort, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* MIRROR */
    {
        { bcmFieldActionMirrorIngress, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID,      PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID,      PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* TRAP_TO_CPU */
    {
        { bcmFieldActionRedirect, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* COPY_TO_CPU */
    {
        { bcmFieldActionCopyToCpu, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID,  PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID,  PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_COSQ */
    /* bcmFieldActionPrioIntNew is not supported on ROBO. 
        Assign-queue action (SET_COSQ) can be achieved by NewTc action.*/
    {
        { bcmFieldActionNewTc, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID,   PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID,   PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_DSCP */
    {
        { bcmFieldActionGpDscpNew,  PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { bcmFieldActionRpDscpNew,PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_TOS */
    {
        { bcmFieldActionGpDscpNew,  PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { bcmFieldActionRpDscpNew,PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_USERPRIO */
    {
        { bcmFieldActionNewTc, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID,   PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID,   PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* PTin added */
    /* SET_USERPRIO_INNERTAG */
    {
        { bcmFieldActionInnerVlanPrioNew, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
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
        { bcmFieldActionNewClassId, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* PTin added: FP */
    /* SET_SRC_CLASS_ID */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_REASON */
    {
        { bcmFieldActionNewReasonCode, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
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

static action_map_entry_t lookup_action_map[BROAD_ACTION_LAST] =
{
    /* SOFT_DROP - do not switch */
    {
        { bcmFieldActionDrop,     PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* HARD_DROP - override all other rules */
    {
        { bcmFieldActionDrop,     PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* PERMIT - default behavior */
    {
        { bcmFieldActionDropCancel, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
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
        { bcmFieldActionCopyToCpu, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
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
        { bcmFieldActionOuterVlanPrioNew, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID,         PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID,         PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* PTin added */
    /* SET_USERPRIO_INNERTAG */
    {
        { bcmFieldActionInnerVlanPrioNew, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
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
        { bcmFieldActionOuterVlanNew, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
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
        { bcmFieldActionVlanAdd,  PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
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
        { bcmFieldActionClassSet, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* PTin added: FP */
    /* SET_SRC_CLASS_ID */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_REASON*/
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

static action_map_entry_t egress_action_map[BROAD_ACTION_LAST] =
{
    /* SOFT_DROP - do not switch */
    {
        { bcmFieldActionGpDrop, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { bcmFieldActionYpDrop, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { bcmFieldActionRpDrop, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* HARD_DROP - override all other rules */
    {
        { bcmFieldActionGpDrop, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { bcmFieldActionYpDrop, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { bcmFieldActionRpDrop, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* PERMIT - default behavior */
    {
        { bcmFieldActionGpDropCancel, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { bcmFieldActionYpDropCancel, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { bcmFieldActionRpDropCancel, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
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
        { bcmFieldActionGpDscpNew, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { bcmFieldActionYpDscpNew, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { bcmFieldActionRpDscpNew, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_TOS */
    {
        { bcmFieldActionGpDscpNew, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { bcmFieldActionYpDscpNew, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { bcmFieldActionRpDscpNew, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_USERPRIO */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* PTin added */
    /* SET_USERPRIO_INNERTAG */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_DROPPREC */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_OUTER_VID */
    {
        { bcmFieldActionOuterVlanNew, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID,     PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID,     PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_INNER_VID */
    {
        { bcmFieldActionInnerVlanNew, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
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
    /* PTin added: FP */
    /* SET_SRC_CLASS_ID */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID},
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_REASON*/
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


static bcm_field_qualify_t l2InPortsQset[] =   /* single VLAN tag */
{
    bcmFieldQualifyInPorts,
    bcmFieldQualifyDstMac,
    bcmFieldQualifySrcMac,
    bcmFieldQualifyEtherType,
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifyInnerVlan,
    bcmFieldQualifyClassId,
    bcmFieldQualifyIpType,
    bcmFieldQualifyVlanFormat
};

#define l2InPortsQsetSize (sizeof(l2InPortsQset) / sizeof(bcm_field_qualify_t))

static bcm_field_qualify_t l2SrcInPortsQset[] =   /* single VLAN tag */
{
    bcmFieldQualifyInPorts,
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifyInnerVlan,
    bcmFieldQualifyIpProtocol,
    bcmFieldQualifyDSCP,
    bcmFieldQualifySrcIp,
    bcmFieldQualifyDstIp,
    bcmFieldQualifySrcMac,
    bcmFieldQualifyClassId,
    bcmFieldQualifyL4SrcPort,
    bcmFieldQualifyVlanFormat
};

#define l2SrcInPortsQsetSize (sizeof(l2SrcInPortsQset) / sizeof(bcm_field_qualify_t))

static bcm_field_qualify_t ipv6InPortsQset[] =   /* single VLAN tag */
{
    bcmFieldQualifyInPorts,
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifyInnerVlan,
    bcmFieldQualifyIp6NextHeader,
    bcmFieldQualifyIpFrag,
    bcmFieldQualifyIpAuth,
    bcmFieldQualifyIp6HopLimit,
    bcmFieldQualifyDstMac,
    bcmFieldQualifySrcMac,
    bcmFieldQualifyClassId,
    bcmFieldQualifyIpType,
    bcmFieldQualifyVlanFormat
};

#define ipv6InPortsQsetSize (sizeof(ipv6InPortsQset) / sizeof(bcm_field_qualify_t))


static bcm_field_qualify_t l2DstInPortsQset[] =   /* single VLAN tag */
{
    bcmFieldQualifyInPorts,
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifyInnerVlan,
    bcmFieldQualifyIpProtocol,
    bcmFieldQualifyDSCP,
    bcmFieldQualifySrcIp,
    bcmFieldQualifyDstIp,
    bcmFieldQualifyDstMac,
    bcmFieldQualifyClassId,
    bcmFieldQualifyL4DstPort,
    bcmFieldQualifyVlanFormat
};

#define l2DstInPortsQsetSize (sizeof(l2DstInPortsQset) / sizeof(bcm_field_qualify_t))

static bcm_field_qualify_t l3l4TcpInPortsQset[] =    /* IPv4 six-tuple  */
{
    bcmFieldQualifyInPorts,
    bcmFieldQualifyOuterVlan,
    bcmFieldQualifyInnerVlan,
    bcmFieldQualifyIpProtocol,
    bcmFieldQualifyDSCP,
    bcmFieldQualifySrcIp,
    bcmFieldQualifyDstIp,
    bcmFieldQualifyL4SrcPort,      /* TCP/UDP Source Port */
    bcmFieldQualifyL4DstPort,      /* TCP/UDP Destination Port */
    bcmFieldQualifyIpType,
    bcmFieldQualifyClassId,
    bcmFieldQualifyTcpControl,
    bcmFieldQualifyVlanFormat
};

#define l3l4TcpInPortsQsetSize (sizeof(l3l4TcpInPortsQset) / sizeof(bcm_field_qualify_t))


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
                                      /* NOT USED FOR ROBO, just left to maintain consistancy with XGS */
}
super_qset_entry_t;

#define SUPER_QSET_TABLE_SIZE  16    /* total number of super qsets */

static super_qset_entry_t super_qset_table[SOC_MAX_NUM_DEVICES][SUPER_QSET_TABLE_SIZE];

/* Group Definitions */
/* This structure mirrors the Broadcom group structure for each unit.
 * It should not contain policy or entry-specific information.
 */

#define GROUP_NONE         0
#define GROUP_USED         1

typedef struct
{
    uint32              flags;
    bcm_field_group_t   gid;    /* only valid in base group entry */
    BROAD_POLICY_TYPE_t type;
    int                 sqset;  /* super qset this group was created from */
    int                 base_group;  /* Identifies the base group in case this group is 
                                        part of a doublewide, triplewide, etc group. */
    uint32              availableRules; /* NOT USED FOR ROBO */
}
group_table_t;

static group_table_t *group_table[SOC_MAX_NUM_DEVICES][BROAD_POLICY_STAGE_COUNT];
static int            group_table_size[SOC_MAX_NUM_DEVICES][BROAD_POLICY_STAGE_COUNT];

#define CHECK_GROUP(u,s,g)  {if ((u>=SOC_MAX_NUM_DEVICES)||(s>=BROAD_POLICY_STAGE_COUNT)||(g>=group_table_size[u][s])) LOG_ERROR(g);}

/* Mapping from BROAD_ENTRY_t to bcm_field_entry_t */
#define BROAD_ENTRY_TO_BCM_ENTRY(entry)   ((bcm_field_entry_t)entry)
#define BCM_ENTRY_TO_BROAD_ENTRY(eid)     ((BROAD_ENTRY_t)eid)

/* Group Allocation Definitions */

#define ALLOC_BLOCK_LOW         0    /* blocks */
#define ALLOC_BLOCK_MEDIUM      1    /* used for VLAN policies */
#define ALLOC_BLOCK_HIGH        2    /* used for PORT policies */

#define ALLOC_HIGH_TO_LOW  1    /* directions */
#define ALLOC_LOW_TO_HIGH  2

typedef struct
{
    int highPrio;     /* highest priority */
    int lowPrio;      /* lowest priority  */
}
group_alloc_table_t;

static group_alloc_table_t group_alloc_table[SOC_MAX_NUM_DEVICES][BROAD_POLICY_STAGE_COUNT][ALLOC_BLOCK_HIGH+1];

/* Utility Functions */

/* Checks if wide mode policies are supported */
static L7_BOOL _policy_supports_wide_mode(int unit)
{
  if(SOC_IS_ROBO(unit))
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

static void _policy_set_union(bcm_field_qset_t q1, bcm_field_qset_t *q2)
{
    int  i;

    for (i = 0; i < bcmFieldQualifyCount; i++)
    {
        if (BCM_FIELD_QSET_TEST(q1,i))
            BCM_FIELD_QSET_ADD(*q2,i);
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
    int singleWideOnly = L7_FALSE;

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
    {
        sysapiPrintf("- searching for qset ");
        for (i = 0; i < bcmFieldQualifyCount; i++)
            sysapiPrintf ("%d", (BCM_FIELD_QSET_TEST(qset, i) ? 1 : 0));
        sysapiPrintf("\n");
    }

    if ((type == BROAD_POLICY_TYPE_SYSTEM) || (type == BROAD_POLICY_TYPE_SYSTEM_PORT) || (type == BROAD_POLICY_TYPE_COSQ))
    {
        singleWideOnly = L7_TRUE;
    }

    for (i = 0; i < SUPER_QSET_TABLE_SIZE; i++)
    {
        if (super_qset_table[unit][i].flags & SUPER_QSET_USED) 
        {
            bcm_field_qset_t qsetFull;

            /* If we are looking for a System/CoS Qset, then restrict the search to single wide qsets */
            if(singleWideOnly && !(super_qset_table[unit][i].flags & SUPER_QSET_SINGLE_WIDE)) {
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
                    sysapiPrintf("- using super qset %d\n", i);

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

static int _policy_super_qset_add(int                  unit,
                                  bcm_field_qualify_t *q1,
                                  int                  q1Size,
                                  custom_field_qualify_t *q2,
                                  int                  q2Size,
                                  int                  flags,
                                  int                  slicesNeeded,
                                  int                  rulesAvailable)
{
    int                 i;
    int                 rv = BCM_E_NONE;
    bcm_field_qset_t    qset1;
    super_qset_entry_t *qsetPtr;

    CHECK_UNIT(unit);

    rv = _policy_super_qset_find_free(unit, &i);
    if (BCM_E_NONE != rv)
        return rv;

    qsetPtr = &super_qset_table[unit][i];

    qsetPtr->flags = flags | SUPER_QSET_USED;
    qsetPtr->slicesNeeded = slicesNeeded;
    qsetPtr->rulesAvailable = rulesAvailable;

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

    return rv;
}

/* TODO: is not actively used but should be modified to make it useful  */
static int _policy_default_rules_per_slice(int unit, BROAD_POLICY_STAGE_t stage)
{
  int rules_per_slice = 0;

  switch (stage)
  {
  case BROAD_POLICY_STAGE_LOOKUP:
      rules_per_slice = 0;
    break;

  case BROAD_POLICY_STAGE_INGRESS:
      rules_per_slice = 128; /* not used fow now */
    break;

  case BROAD_POLICY_STAGE_EGRESS:
      rules_per_slice = 0;
    break;

  default:
    break;
  }

  return rules_per_slice;
}

static int _policy_super_qset_init(int unit)
{
    int  i;
    int rules_per_slice[BROAD_POLICY_STAGE_COUNT];

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("policy_super_qset_init(%d)\n", unit);

    for (i = BROAD_POLICY_STAGE_LOOKUP; i < BROAD_POLICY_STAGE_COUNT; i++)
    {
      rules_per_slice[i] = _policy_default_rules_per_slice(unit, i);
    }

    for (i = 0; i < SUPER_QSET_TABLE_SIZE; i++)
    {
        super_qset_table[unit][i].flags = SUPER_QSET_NONE;
    }

      _policy_super_qset_add(unit, l2InPortsQset,   l2InPortsQsetSize,   NULL,    0, SUPER_QSET_SINGLE_WIDE, 1, rules_per_slice[BROAD_POLICY_STAGE_INGRESS]);
      _policy_super_qset_add(unit, l3l4TcpInPortsQset,  l3l4TcpInPortsQsetSize,  NULL,    0, SUPER_QSET_SINGLE_WIDE, 1, rules_per_slice[BROAD_POLICY_STAGE_INGRESS]);
      _policy_super_qset_add(unit, ipv6InPortsQset,    ipv6InPortsQsetSize,    NULL,    0, SUPER_QSET_SINGLE_WIDE, 1, rules_per_slice[BROAD_POLICY_STAGE_INGRESS]);
      _policy_super_qset_add(unit, l2SrcInPortsQset,    l2SrcInPortsQsetSize,    NULL,    0, SUPER_QSET_SINGLE_WIDE, 1, rules_per_slice[BROAD_POLICY_STAGE_INGRESS]);
      _policy_super_qset_add(unit, l2DstInPortsQset, l2DstInPortsQsetSize, NULL,    0, SUPER_QSET_SINGLE_WIDE, 1, rules_per_slice[BROAD_POLICY_STAGE_INGRESS]);
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
    case BROAD_POLICY_TYPE_IPSG:
        *block = ALLOC_BLOCK_LOW;
        *dir   = ALLOC_LOW_TO_HIGH;
        break;
    case BROAD_POLICY_TYPE_VLAN:
    case BROAD_POLICY_TYPE_ISCSI:
        *block = ALLOC_BLOCK_MEDIUM;
        *dir   = ALLOC_LOW_TO_HIGH;
        break;
    case BROAD_POLICY_TYPE_PORT:
        *block = ALLOC_BLOCK_MEDIUM;
        *dir   = ALLOC_HIGH_TO_LOW;
        break;
    case BROAD_POLICY_TYPE_SYSTEM_PORT:
    case BROAD_POLICY_TYPE_SYSTEM:
        *block = ALLOC_BLOCK_LOW;
        *dir   = ALLOC_HIGH_TO_LOW;
        break;
    case BROAD_POLICY_TYPE_COSQ:
        *block = ALLOC_BLOCK_LOW;
        *dir   = ALLOC_LOW_TO_HIGH;
        break;
    case BROAD_POLICY_TYPE_DVLAN:
        *block = ALLOC_BLOCK_HIGH;
        *dir   = ALLOC_LOW_TO_HIGH;
        break;
    case BROAD_POLICY_TYPE_DOT1AD_SNOOP: /*protocol snooping policies*/
        *block = ALLOC_BLOCK_HIGH;
        *dir   = ALLOC_HIGH_TO_LOW;
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

static int _policy_group_find_first(int                  unit, 
                                    BROAD_POLICY_STAGE_t policyStage,
                                    BROAD_POLICY_TYPE_t  type, 
                                    BROAD_GROUP_t       *group)
{
    int block, dir;
    int used_block, used_dir;
    group_table_t *groupPtr;

    _policy_group_alloc_type(type, &block, &dir);

    if (ALLOC_HIGH_TO_LOW == dir)
    {
        *group = group_alloc_table[unit][policyStage][block].lowPrio;
    }
    else
    {
        *group = group_alloc_table[unit][policyStage][block].highPrio;
    }

    groupPtr = &group_table[unit][policyStage][*group];
    if (groupPtr->flags & GROUP_USED)
    {
        *group = groupPtr->base_group;
        groupPtr = &group_table[unit][policyStage][*group];

        /* make sure that the block that this group belongs to matches
           the block requested */
        if (type != groupPtr->type)
        {
          /* There is a special case here... SYSTEM policies and SYSTEM_PORT
             policies may share a group. */
          if (!(((type == BROAD_POLICY_TYPE_SYSTEM_PORT) && (groupPtr->type == BROAD_POLICY_TYPE_SYSTEM)) || 
                ((type == BROAD_POLICY_TYPE_SYSTEM)      && (groupPtr->type == BROAD_POLICY_TYPE_SYSTEM_PORT))))
          {
            return BCM_E_FAIL;
          }
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
            if (type != groupPtr->type)
            {
              /* There is a special case here... SYSTEM policies and SYSTEM_PORT
                 policies may share a group. */
              if (!(((type == BROAD_POLICY_TYPE_SYSTEM_PORT) && (groupPtr->type == BROAD_POLICY_TYPE_SYSTEM)) || 
                    ((type == BROAD_POLICY_TYPE_SYSTEM)      && (groupPtr->type == BROAD_POLICY_TYPE_SYSTEM_PORT))))
              {
                return BCM_E_FAIL;
              }
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
          if (type != groupPtr->type)
          {
            /* There is a special case here... SYSTEM policies and SYSTEM_PORT
               policies may share a group. */
            if (!(((type == BROAD_POLICY_TYPE_SYSTEM_PORT) && (groupPtr->type == BROAD_POLICY_TYPE_SYSTEM)) || 
                  ((type == BROAD_POLICY_TYPE_SYSTEM)      && (groupPtr->type == BROAD_POLICY_TYPE_SYSTEM_PORT))))
            {
              return BCM_E_FAIL;
            }
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
        retval = _policy_group_increment(unit, policyStage, group, block, type);
    }
    else
    {
        retval = _policy_group_decrement(unit, policyStage, group, block, type);
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
                sysapiPrintf("- found free group %d\n", *group);

              return BCM_E_NONE;
            }
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
    if (field < BROAD_FIELD_LAST)
    {
        /* map user-specified field to bcm field */
         *bcm_field = field_map[field];
              
    }
    else
        return BCM_E_PARAM;

    return BCM_E_NONE;
}

static int _policy_group_calc_qset(int                   unit,
                                   BROAD_POLICY_STAGE_t  policyStage,
                                   BROAD_POLICY_ENTRY_t *entryPtr, 
                                   bcm_field_qset_t     *qset,
                                   custom_field_qset_t  *customQset,
                                   int                  *req_ethtype)
{
    int    f;
    int    rv;
    BROAD_POLICY_RULE_ENTRY_t *rulePtr;

    BCM_FIELD_QSET_INIT(*qset);
    CUSTOM_FIELD_QSET_INIT(*customQset);

    /* Determine if ETHTYPE is required in this QSET, or if IpType can be used instead. */
    rulePtr = entryPtr->ruleInfo;
    while (rulePtr != L7_NULL)
    {
      if (hapiBroadPolicyFieldFlagsGet(&rulePtr->fieldInfo, BROAD_FIELD_ETHTYPE) == BROAD_FIELD_SPECIFIED)
      {
        uint16 ethtype = *((uint16*)hapiBroadPolicyFieldValuePtr(&rulePtr->fieldInfo, BROAD_FIELD_ETHTYPE));
        if ((0x0800 != ethtype) && (0x86DD != ethtype))
        {
          *req_ethtype = 1;
        }
      }

      rulePtr = rulePtr->next;
    }

    /* Create a qset containing each field specified in each rule. */
    rulePtr = entryPtr->ruleInfo;
    while (rulePtr != L7_NULL)
    {
        for (f = 0; f < BROAD_FIELD_LAST; f++)
        {
            if (hapiBroadPolicyFieldFlagsGet(&rulePtr->fieldInfo, f) == BROAD_FIELD_SPECIFIED)
            {
                char                *value;
                char                *mask;
                bcm_field_qualify_t  bcm_field;

                value = (char*)hapiBroadPolicyFieldValuePtr(&rulePtr->fieldInfo, f);
                mask  = (char*)hapiBroadPolicyFieldMaskPtr(&rulePtr->fieldInfo, f);

                switch (f)
                {
                /* custom fields go here */
                case BROAD_FIELD_ICMP_MSG_TYPE:
                  /* Use L4 src port for ICMP Msg Type. We can do this because
                     XGS3 just treats the L4 src port field as the first two
                     bytes following the IP header. */
                  BCM_FIELD_QSET_ADD(*qset,bcmFieldQualifyL4SrcPort);
                  break;

                case BROAD_FIELD_IP6_TRAFFIC_CLASS:
                  if (policyStage == BROAD_POLICY_STAGE_EGRESS)
                  {
                    /* For some reason the SDK requires us to use the TOS
                       qualifier on egress, instead of the Ip6TrafficClass. */
                    BCM_FIELD_QSET_ADD(*qset, bcmFieldQualifyTos);
                    break;
                  }
                  /* else fall through */

                default:
                  rv = _policy_field_to_bcm_field(f, policyStage, (char*)value, (char*)mask, &bcm_field, *req_ethtype);
                  if (BCM_E_NONE != rv)
                    return rv;

                  BCM_FIELD_QSET_ADD(*qset,bcm_field);
                  break;
                }
            }
        }

        rulePtr = rulePtr->next;
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
    int countersReq = 0, metersReq = 0;
    BROAD_POLICY_RULE_ENTRY_t *rulePtr;

    /* Determine how many counters/meters are required for the policy. */
    rulePtr = entryPtr->ruleInfo;
    while (rulePtr != L7_NULL)
    {
        if (rulePtr->ruleFlags & BROAD_METER_SPECIFIED)
        {
            #if (SDK_VERSION_IS < SDK_VERSION(5,6,0,0))
            /* meters use a counter as well */
            countersReq++;
            #endif
            metersReq++;
        }
        /* PTin modified: Stats */
        #if (SDK_VERSION_IS < SDK_VERSION(5,6,0,0))
        else
        #endif
        if (rulePtr->ruleFlags & BROAD_COUNTER_SPECIFIED)
        {
            countersReq++;        
        }

        rulePtr = rulePtr->next;
    }

    /* Find an existing group that can satisfy the policy requirements. */
    rv = _policy_group_find_first(unit, policyStage, entryPtr->policyType, group);
    while (BCM_E_NONE == rv)
    {
        bcm_field_qset_t         qset;
        bcm_field_group_status_t stat;
        group_table_t           *groupPtr;

        groupPtr = &group_table[unit][policyStage][*group];

        if (groupPtr->flags & GROUP_USED)
        {
            if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
                sysapiPrintf("- examining group %d ", *group);

            /* Insure the group has a suitable qset and enough free entries, counters, et al. */
            BCM_FIELD_QSET_INIT(qset);
            _policy_set_union(super_qset_table[unit][groupPtr->sqset].qsetAgg, &qset);
            _policy_set_union(super_qset_table[unit][groupPtr->sqset].qsetUdf, &qset);

            rv = bcm_field_group_status_get(unit, groupPtr->gid, &stat);
            if (BCM_E_NONE != rv)
                return rv;

            rv = _policy_set_subset(policyQset, customQset, qset, super_qset_table[unit][groupPtr->sqset].customQset);
            if ((BCM_E_NONE == rv) &&
                (stat.entries_free >= entryPtr->ruleCount) &&
                (stat.counters_free >= countersReq) &&
                (stat.meters_free >= metersReq))
            {
                /* reuse existing group */
                if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
                    sysapiPrintf("- reuse group %d\n", *group);

                return BCM_E_NONE;
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

    /* create all new groups based upon a super qset */
    rv = _policy_super_qset_find_match(unit, policyStage, entryPtr->policyType, policyQset, customQset, &sqset);
    if (BCM_E_NONE != rv)
        return rv;

    slices_needed = _policy_sqset_slice_count(unit, sqset);

        /* allocate a new group of the appropriate type */
    rv = _policy_group_find_free_group(unit, policyStage, entryPtr->policyType, group, slices_needed);
    if (BCM_E_NONE != rv)
      return rv;

    groupPtr = &group_table[unit][policyStage][*group];
    groupPtr->availableRules = super_qset_table[unit][sqset].rulesAvailable;

        /* populate group table */
    for (i = *group; i < (*group + slices_needed); i++)
    {
      tempGroupPtr = &group_table[unit][policyStage][i];

      tempGroupPtr->flags = GROUP_USED;
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
      mode = bcmFieldGroupModeAuto;
    }
    else if (super_qset_table[unit][sqset].flags & SUPER_QSET_TRIPLE_WIDE)
    {
      mode = bcmFieldGroupModeAuto;
    }
    else
    {
      /* handles quadwide mode */
      mode = bcmFieldGroupModeAuto;
    }

    rv = bcm_field_group_create_mode(unit, super_qset_table[unit][sqset].qsetAgg,
                                     prio, mode, &groupPtr->gid);
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
        sysapiPrintf("bcm_field_group_create() returned %d\n", rv);

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

  /* PTin modified: SDK 6.3.0 */
  #if 1
  if (hapiStatus & BROAD_LOOKUPSTATUS_DOS_ATTACK_PKT)
  {
    *bcmStatus |= BROAD_LOOKUPSTATUS_DOS_ATTACK_PKT;
  }
  if (hapiStatus & BROAD_LOOKUPSTATUS_UNRESOLVED_SA)
  {
    *bcmStatus |= BROAD_LOOKUPSTATUS_UNRESOLVED_SA;
  }
  if (hapiStatus & BROAD_LOOKUPSTATUS_LPM_HIT)
  {
    *bcmStatus |= BROAD_LOOKUPSTATUS_LPM_HIT;
  }
  if (hapiStatus & BROAD_LOOKUPSTATUS_STARGV_HIT)
  {
    *bcmStatus |= BROAD_LOOKUPSTATUS_STARGV_HIT;
  }
  if (hapiStatus & BROAD_LOOKUPSTATUS_L3_DST_HIT)
  {
    *bcmStatus |= BROAD_LOOKUPSTATUS_L3_DST_HIT;
  }
  if (hapiStatus & BROAD_LOOKUPSTATUS_L3_UC_SRC_HIT)
  {
    *bcmStatus |= BROAD_LOOKUPSTATUS_L3_UC_SRC_HIT;
  }
  if (hapiStatus & BROAD_LOOKUPSTATUS_L2_USER_ENTRY_HIT)
  {
    *bcmStatus |= BROAD_LOOKUPSTATUS_L2_USER_ENTRY_HIT;
  }
  if (hapiStatus & BROAD_LOOKUPSTATUS_L2_TABLE_DST_L3)
  {
    *bcmStatus |= BROAD_LOOKUPSTATUS_L2_TABLE_DST_L3;
  }
  if (hapiStatus & BROAD_LOOKUPSTATUS_L2_DST_HIT)
  {
    *bcmStatus |= BROAD_LOOKUPSTATUS_L2_DST_HIT;
  }
  if (hapiStatus & BROAD_LOOKUPSTATUS_L2_SRC_STATIC)
  {
    *bcmStatus |= BROAD_LOOKUPSTATUS_L2_SRC_STATIC;
  }
  if (hapiStatus & BROAD_LOOKUPSTATUS_L2_SRC_HIT)
  {
    *bcmStatus |= BROAD_LOOKUPSTATUS_L2_SRC_HIT;
  }
  if (isMask)
  {
    if (hapiStatus & BROAD_LOOKUPSTATUS_INGRESS_SPG_STATE_MASK)
    {
      *bcmStatus |= BROAD_LOOKUPSTATUS_INGRESS_SPG_STATE_MASK;
    }
  }
  else
  {
    if ((hapiStatus & BROAD_LOOKUPSTATUS_INGRESS_SPG_STATE_MASK) == BROAD_LOOKUPSTATUS_INGRESS_SPG_STATE_DIS)
    {
      *bcmStatus |= BROAD_LOOKUPSTATUS_INGRESS_SPG_STATE_DIS;
    }
    if ((hapiStatus & BROAD_LOOKUPSTATUS_INGRESS_SPG_STATE_MASK) == BROAD_LOOKUPSTATUS_INGRESS_SPG_STATE_BLK)
    {
      *bcmStatus |= BROAD_LOOKUPSTATUS_INGRESS_SPG_STATE_BLK;
    }
    if ((hapiStatus & BROAD_LOOKUPSTATUS_INGRESS_SPG_STATE_MASK) == BROAD_LOOKUPSTATUS_INGRESS_SPG_STATE_LRN)
    {
      *bcmStatus |= BROAD_LOOKUPSTATUS_INGRESS_SPG_STATE_LRN;
    }
    if ((hapiStatus & BROAD_LOOKUPSTATUS_INGRESS_SPG_STATE_MASK) == BROAD_LOOKUPSTATUS_INGRESS_SPG_STATE_FWD)
    {
      *bcmStatus |= BROAD_LOOKUPSTATUS_INGRESS_SPG_STATE_FWD;
    }
  }
  if (hapiStatus & BROAD_LOOKUPSTATUS_FB_VLAN_ID_VALID)
  {
    *bcmStatus |= BROAD_LOOKUPSTATUS_FB_VLAN_ID_VALID;
  }
  if (hapiStatus & BROAD_LOOKUPSTATUS_VXLT_HIT)
  {
    *bcmStatus |= BROAD_LOOKUPSTATUS_VXLT_HIT;
  }
  if (hapiStatus & BROAD_LOOKUPSTATUS_TUNNEL_HIT)
  {
    *bcmStatus |= BROAD_LOOKUPSTATUS_TUNNEL_HIT;
  }
  #endif
}

static int _policy_group_add_std_field(int                   unit,
                                       BROAD_POLICY_STAGE_t  policyStage,
                                       bcm_field_entry_t     eid,
                                       BROAD_POLICY_FIELD_t  field,
                                       char                 *value,
                                       char                 *mask)
{
    int  rv = BCM_E_NONE;
    L7_ushort16 value16, mask16;
    uint32 ipType = bcmFieldIpTypeAny;
    L7_ushort16 lookupStatus     = 0;
    L7_ushort16 lookupStatusMask = 0;

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
        rv = bcm_field_qualify_EtherType(unit, eid, *((uint16*)value), 0xFFFF);
        /* ETHTYPE must not be part of the qset, try IpType */
        if(BCM_E_NOT_FOUND==rv)
        {
          switch (*((uint16*)value))  
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
    case BROAD_FIELD_TCP_CONTROL:
        rv = bcm_field_qualify_TcpControl(unit, eid, *((uint8 *)value), *((uint8 *)mask));
        break;
    case BROAD_FIELD_OVID:
        rv = bcm_field_qualify_OuterVlan(unit, eid, *((bcm_vlan_t*)value), *((bcm_vlan_t*)mask));
        break;
    case BROAD_FIELD_IVID:
        rv = bcm_field_qualify_InnerVlan(unit, eid, *((bcm_vlan_t*)value), *((bcm_vlan_t*)mask));
        /* On bcm53115, untagged packets and single tagged packets are normalized to carry
           two tags before reaching CFP. So, there is no need to check for tagged status
           of the packet for IVID field.*/
        if(hapiBroadRoboVariantCheck() != __BROADCOM_53115_ID)
        {
          if ((BCM_E_NONE == rv) && ((*((bcm_vlan_t*)value) != 0) || (*((bcm_vlan_t*)mask) != 0)))
            /* PTin modified: SDK 6.3.0 */
            #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
            rv = bcm_field_qualify_VlanFormat(unit, eid, BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED | BCM_FIELD_VLAN_FORMAT_INNER_TAGGED, 0xff);
            #else
            rv = bcm_field_qualify_VlanFormat(unit, eid, BCM_FIELD_PKT_FMT_INNER_TAGGED, BCM_FIELD_PKT_FMT_INNER_TAGGED);
            #endif
        }
        break;
    case BROAD_FIELD_DSCP:
        rv = bcm_field_qualify_DSCP(unit, eid, *((uint8*)value), *((uint8*)mask));
        break;
    case BROAD_FIELD_PROTO:
        rv = bcm_field_qualify_IpProtocol(unit, eid, *((uint8*)value), *((uint8*)mask));
        break;
    case BROAD_FIELD_SIP:
        rv = bcm_field_qualify_SrcIp(unit, eid, *((bcm_ip_t*)value), *((bcm_ip_t*)mask));
        break;
    case BROAD_FIELD_DIP:
        rv = bcm_field_qualify_DstIp(unit, eid, *((bcm_ip_t*)value), *((bcm_ip_t*)mask));
        break;
    case BROAD_FIELD_ICMP_MSG_TYPE:
      /* Use L4 src port for ICMP Msg Type. We can do this because
         XGS3 just treats the L4 src port field as the first two
         bytes following the IP header. */
        value16 = *((uint8 *)value) << 8;
        mask16  = *((uint8 *)mask) << 8;
        rv = bcm_field_qualify_L4SrcPort(unit, eid, value16, mask16);
        break;
    case BROAD_FIELD_SPORT:
        rv = bcm_field_qualify_L4SrcPort(unit, eid, *((uint16*)value), *((uint16*)mask));
        break;
    case BROAD_FIELD_DPORT:
        rv = bcm_field_qualify_L4DstPort(unit, eid, *((uint16*)value), *((uint16*)mask));
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
        rv = bcm_field_qualify_Ip6FlowLabel(unit, eid, *((uint32*)value), *((uint32*)mask));
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
/* PTin added: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
    /* PTin added: FP */
    case BROAD_FIELD_CLASS_ID:
        rv = bcm_field_qualify_DstClassField(unit, eid, *((uint8*)value), *((uint8*)mask));
        break;
    case BROAD_FIELD_SRC_CLASS_ID:
        rv = bcm_field_qualify_SrcClassField(unit, eid, *((uint8*)value), *((uint8*)mask));
        break;
#else
    case BROAD_FIELD_CLASS_ID:
    case BROAD_FIELD_SRC_CLASS_ID:
        rv = bcm_field_qualify_LookupClass0(unit, eid, *((uint8*)value), 0xF);
        break;
#endif
    case BROAD_FIELD_L2_CLASS_ID:
        /* PTin modified: SDK 6.3.0 */
        #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
        rv = bcm_field_qualify_SrcClassL2(unit, eid, *((uint8*)value), 0xF);
        #else
        rv = bcm_field_qualify_SrcMacGroup(unit, eid, *((uint8*)value), 0xF);
        #endif
        break;
    case BROAD_FIELD_LOOKUP_STATUS:
        _policy_group_lookupstatus_convert(*((uint16*)value), &lookupStatus,     L7_FALSE);
        _policy_group_lookupstatus_convert(*((uint16*)mask),  &lookupStatusMask, L7_TRUE);
        /* TODO: SDK 6.3.0 */
        #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
        rv = BCM_E_UNAVAIL;
        PT_LOG_ERR(LOG_CTX_HAPI,"bcm_field_qualify_LookupStatus is not supported!");
        #else
        rv = bcm_field_qualify_LookupStatus(unit, eid, lookupStatus, lookupStatusMask);
        #endif
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
        rv = bcm_field_qualify_InPorts(unit, eid, *((bcm_pbmp_t*)value), *((bcm_pbmp_t*)mask));
        break;
    case BROAD_FIELD_OUTPORT:
        rv = bcm_field_qualify_OutPort(unit, eid, *((bcm_port_t*)value), *((bcm_port_t*)mask));
        break;
    case BROAD_FIELD_SRCTRUNK:
        rv = bcm_field_qualify_SrcTrunk(unit, eid, *((bcm_trunk_t*)value), *((bcm_trunk_t*)mask));
        break;
    case BROAD_FIELD_PORTCLASS:
        rv = bcm_field_qualify_DstTrunk(unit, eid, *((uint32*)value), *((uint32*)mask));
        break;
    case BROAD_FIELD_DROP:
        rv = bcm_field_qualify_Drop(unit,eid,*((uint8*)value), 1);
        break;
    case BROAD_FIELD_L2_SRCHIT:
        rv = bcm_field_qualify_L2SrcHit(unit,eid,*((uint8*)value), 1);
        break;
    case BROAD_FIELD_L2_DSTHIT:
        rv = bcm_field_qualify_L2DestHit(unit,eid,*((uint8*)value), 1);
        break;
    case BROAD_FIELD_INT_PRIO:
        rv = bcm_field_qualify_IntPriority(unit,eid,*((uint8*)value), *((uint8*)mask));
        break;
    // PTin end
    default:
        rv = BCM_E_PARAM;
        break;
    }

    return rv;
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

    rv = _policy_group_add_std_field(unit, policyStage, eid, field, value, mask);

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
        if (BROAD_ACTION_REDIRECT == action)
        {
          if (BCMX_LPORT_INVALID == actPtr->u.ifp_parms.modid)
          {
            bcm_action = bcmFieldActionDrop;
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

#ifdef BCM_ROBO_SUPPORT
        /* handle case of SET_TOS action. for non-conforming/exceeding traffic, if there is 
         * no field in POLICY Table to set TOS. instead, we use bcmFieldActionDscpNew action,
         * and require to shift TOS value.
         */
         /*On 53115, bcmFieldActionGpDscpNew, bcmFieldActionYpDscpNew actions are not supported.*/
        if(SOC_IS_ROBO53115(unit))
        {
          if ((action == BROAD_ACTION_SET_TOS) &&
              (bcm_action == bcmFieldActionDscpNew))      
          {
            dscp = param0 << 3; /* Treat TOS as a DSCP value */
            param0 = dscp;
          }
        }

       /*installing a rule without any meter configuration on bcm53115
          the packets will be treated as Out-Band */
        if(SOC_IS_ROBO53115(unit))
        {
          if (bcm_action == bcmFieldActionGpRedirectPort)
          {
            if(!BROAD_NONCONFORMING_ACTION_IS_SPECIFIED(actPtr, BROAD_ACTION_HARD_DROP))
            {
              rv = bcm_field_action_add(unit, eid, bcmFieldActionRpRedirectPort,
                                param0, param1);
              if (BCM_E_NONE != rv)
              {
               return rv;
              }
            }
          }
        } 
#endif

        if (bcmFieldActionDropPrecedence == bcm_action)
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
        else if (PROFILE_ACTION_INVALID != bcm_action)
        {
          /* set remaining actions */
          rv = bcm_field_action_add(unit, eid, bcm_action, param0, param1);
          if (BCM_E_NONE != rv)
            return rv;
        }

        /* For a redirect or mirror action, add an implicit lookup status qualifier to 
         * forward based on STP status - allow only if the state is set to "forwarding" */
        /* Allow redirecting to CPU regardless of STP state */
        if (((bcmFieldActionRedirect == bcm_action) && (param1 != CMIC_PORT(unit))) || 
            (bcmFieldActionMirrorIngress == bcm_action))
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
            if ((BCM_E_NOT_FOUND == rv) || (SOC_IS_ROBO(unit) && (BCM_E_UNAVAIL == rv)))
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

/* PTin modified: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(5,6,0,0))
static int _policy_group_add_policer(int unit, bcm_field_entry_t eid, bcm_field_group_t gid, BROAD_POLICY_RULE_ENTRY_t *rulePtr)
{
    int                   rv = BCM_E_NONE;
    BROAD_METER_ENTRY_t  *meterPtr;
    bcm_policer_t         src_policer_id, policer_id;
    bcm_policer_config_t  policer_cfg;

    //printf("%s(%d) I was here!",__FUNCTION__,__LINE__);

    meterPtr = &rulePtr->policer.policerInfo;

    bcm_policer_config_t_init(&policer_cfg);
    policer_cfg.flags = /*BCM_POLICER_REPLACE | BCM_POLICER_DROP_RED |*/ 0;
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
    //policer_cfg.flags |= BCM_POLICER_MODE_BYTES;
  #endif
    policer_cfg.mode  = bcmPolicerModeTrTcm;  /* RFC 2698 */
    policer_cfg.ckbits_sec    = meterPtr->cir;
    policer_cfg.ckbits_burst  = meterPtr->cbs;
    policer_cfg.pkbits_sec    = meterPtr->pir;
    policer_cfg.pkbits_burst  = meterPtr->pbs;
    policer_cfg.action_id     = bcmPolicerActionRpDrop;
    policer_cfg.sharing_mode  = 0;

    if (meterPtr->colorMode == BROAD_METER_COLOR_BLIND)
      policer_cfg.flags |= BCM_POLICER_COLOR_BLIND;

    if (rulePtr->ruleFlags & BROAD_METER_SHARED)
    {
        src_policer_id = rulePtr->src_policerId;

        rv = bcm_field_entry_policer_attach(unit, eid, 0, src_policer_id);

        if (BCM_E_NONE != rv)
        {
          printf("%s(%d) We have an error! rv=%d\r\n",__FUNCTION__,__LINE__,rv);
          return rv;
        }

        rulePtr->policer.policer_id = src_policer_id;
    }
    else
    {
        /* Create policer */
        rv = bcm_policer_create(unit, &policer_cfg, &policer_id);
        if (BCM_E_NONE != rv)
        {
          printf("%s(%d) We have an error! rv=%d\r\n",__FUNCTION__,__LINE__,rv);
            return rv;
        }

        /* Attach policer to field entry */
        rv = bcm_field_entry_policer_attach(unit, eid, 0, policer_id);
        if (BCM_E_NONE != rv)
        {
          printf("%s(%d) We have an error! rv=%d\r\n",__FUNCTION__,__LINE__,rv);
            return rv;
        }

        rulePtr->policer.policer_id = policer_id;
        rulePtr->src_policerId = policer_id;
    }

    printf("%s(%d) I was here! rv=%d\r\n",__FUNCTION__,__LINE__,rv);

    return rv;
}

static int _policy_group_add_stat(int unit, bcm_field_entry_t eid, bcm_field_group_t gid, BROAD_POLICY_RULE_ENTRY_t *rulePtr)
{
    int                    rv = BCM_E_NONE;
    uint64                 zero64;
    BROAD_COUNTER_ENTRY_t *counterPtr;
    bcm_field_stat_t       stat[2];
    int                    src_stat_id, stat_id;

    printf("%s(%d) gid=%u, eid=%u",__FUNCTION__,__LINE__,gid,eid);

    counterPtr = &rulePtr->counter.counterInfo;

    if (rulePtr->ruleFlags & BROAD_COUNTER_SHARED)
    {
      src_stat_id = rulePtr->src_counterId;

      rv = bcm_field_entry_stat_attach(unit, eid, src_stat_id);

      if (BCM_E_NONE != rv)
      {
        printf("%s(%d) We have an error! rv=%d",__FUNCTION__,__LINE__,rv);
          return rv;
      }

      rulePtr->counter.counter_id = src_stat_id;
    }
    else
    {
      /* add counter to support stats */
      if (BROAD_COUNT_PACKETS == counterPtr->mode)
      {
        stat[0] = bcmFieldStatPackets;
      }
      else
      {
        stat[0] = bcmFieldStatBytes;
      }

      rv = bcm_field_stat_create(unit, gid, 1, stat, &stat_id);
      if (BCM_E_NONE != rv)
      {
        printf("%s(%d) We have an error! rv=%d",__FUNCTION__,__LINE__,rv);
          return rv;
      }

      rv = bcm_field_entry_stat_attach(unit, eid, stat_id);
      if (BCM_E_NONE != rv)
      {
        printf("%s(%d) We have an error! rv=%d",__FUNCTION__,__LINE__,rv);
          return rv;
      }

      rulePtr->counter.counter_id = stat_id;
      rulePtr->src_counterId = stat_id;
    }

    /* zero values prior to first use */
    COMPILER_64_ZERO(zero64);
    rv = bcm_field_stat_all_set(unit, stat_id, zero64);
    if (BCM_E_NONE != rv)
    {
      printf("%s(%d) We have an error! rv=%d",__FUNCTION__,__LINE__,rv);
        return rv;
    }

    printf("%s(%d) Success! rv=%d",__FUNCTION__,__LINE__,rv);

    return rv;
}

#else

static int _policy_group_add_meter(int unit, bcm_field_entry_t eid, BROAD_POLICY_RULE_ENTRY_t *rulePtr)
{
    int                  rv = BCM_E_NONE;
    uint64               zero64;
    BROAD_METER_ENTRY_t *meterPtr;

    meterPtr = &rulePtr->policer.policerInfo;     /* PTin modified: SDK 6.3.0 */

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
                                 meterPtr->cir, meterPtr->cbs);
        if (BCM_E_NONE != rv)
            return rv;

    }

    /* SDK.5.5.5's robo part of action_add not handling COLOR_BLIND/COLOR_AWARE
       TODO: need to indentify correct parameters for color aware*/
    rv = bcm_field_action_add(unit, eid, bcmFieldActionMeterConfig,
                              BCM_FIELD_METER_MODE_DEFAULT,
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

    return rv;
}

static int _policy_group_add_counter(int unit, bcm_field_entry_t eid, BROAD_POLICY_RULE_ENTRY_t *rulePtr)
{
    int                    rv = BCM_E_NONE;
    uint32                 flags;
    uint64                 zero64;
    BROAD_COUNTER_ENTRY_t *counterPtr;

    counterPtr = &rulePtr->counter.counterInfo;   /* PTin modified: SDK 6.3.0 */

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

    return rv;
}
#endif

static int _policy_group_alloc_init(int unit, BROAD_POLICY_STAGE_t policyStage, int groups)
{

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("policy_group_alloc_init(%d)\n", unit);

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

      break;
    case BROAD_POLICY_STAGE_INGRESS:
      /* low priority group starts at 0 and goes for 4 */
      group_alloc_table[unit][policyStage][ALLOC_BLOCK_LOW].lowPrio     = 13;
      group_alloc_table[unit][policyStage][ALLOC_BLOCK_LOW].highPrio    = 15;

      group_alloc_table[unit][policyStage][ALLOC_BLOCK_MEDIUM].lowPrio  = 9;
      group_alloc_table[unit][policyStage][ALLOC_BLOCK_MEDIUM].highPrio = 12;

      group_alloc_table[unit][policyStage][ALLOC_BLOCK_HIGH].lowPrio    = 0;
      group_alloc_table[unit][policyStage][ALLOC_BLOCK_HIGH].highPrio   = 8;

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

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("policy_group_table_init(%d)\n", unit);


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

    /* TODO:
     * 5348 : 5
     * 5395 : 6
     * 53115 : 16
     */

    switch (policyStage)
    {
    case BROAD_POLICY_STAGE_LOOKUP:
       total_slices = 0;
      break;
    case BROAD_POLICY_STAGE_INGRESS:
     if (SOC_IS_ROBO(unit)) 
        total_slices = 16; /*53115 has 16 slices*/
     break;
    case BROAD_POLICY_STAGE_EGRESS:
       total_slices = 0;
      break;
    default:
      break;
    }

    return total_slices;
}

L7_BOOL policy_cfp_stage_supported(int unit, BROAD_POLICY_STAGE_t policyStage)
{
    if (policyStage != BROAD_POLICY_STAGE_INGRESS)
    {
      return L7_FALSE;
    }

  return L7_TRUE;
}

int policy_cfp_group_init(int unit)
{
    int                  rv;
    int                  total_slices;
    BROAD_POLICY_STAGE_t policyStage;

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("policy_cfp_group_init(%d) \n", unit);

    for (policyStage = BROAD_POLICY_STAGE_LOOKUP; policyStage < BROAD_POLICY_STAGE_COUNT; policyStage++)
    {
      total_slices = _policy_group_total_slices(unit, policyStage);

      rv = _policy_group_alloc_init(unit, policyStage, total_slices);
      if (BCM_E_NONE != rv)
          return rv;

      rv = _policy_group_table_init(unit, policyStage, total_slices);
      if (BCM_E_NONE != rv)
          return rv;
    }

    rv = _policy_super_qset_init(unit);
    if (BCM_E_NONE != rv)
        return rv;

    return rv;
}



int policy_cfp_group_create(int unit, BROAD_POLICY_STAGE_t policyStage, BROAD_POLICY_ENTRY_t *policyData, BROAD_GROUP_t *group)
{
    int                 rv;
    bcm_field_qset_t    policyQset;
    custom_field_qset_t customQset;
    int                 req_ethtype = 0;

    if (policy_cfp_stage_supported(unit, policyStage) == L7_FALSE)
    {
      return BCM_E_UNAVAIL;
    }

    rv = _policy_group_calc_qset(unit, policyStage, policyData, &policyQset, &customQset, &req_ethtype);
    if (BCM_E_NONE != rv)
        return rv;

    rv = _policy_group_find_group(unit, policyStage, policyData, group, policyQset, customQset);

    if (BCM_E_NONE != rv)
    {
#ifdef BCM_ROBO_SUPPORT
      if (!(SOC_IS_ROBO5395(unit) || SOC_IS_ROBO5348(unit)))
      {
        if ((req_ethtype == 0))
        {
          BCM_FIELD_QSET_ADD(policyQset,bcmFieldQualifyEtherType);
          rv = _policy_group_find_group(unit, policyStage, policyData, group, policyQset, customQset);
          if (BCM_E_NONE != rv)
          {  
            BCM_FIELD_QSET_REMOVE(policyQset,bcmFieldQualifyEtherType);
          }
        }
      }
#endif
    }

       
    
    if (BCM_E_NONE != rv)
    {
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
            sysapiPrintf("- allocate new group\n");

        rv = _policy_group_alloc_group(unit, policyStage, policyData, group, policyQset, customQset);

        if (BCM_E_NONE != rv)
        {
            if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
                sysapiPrintf("- allocate new group FAILED\n");
        }
    }

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
        sysapiPrintf("- using group %d\n", *group);

    return rv;
}
 
int policy_cfp_group_add_rule(int                        unit,
                              BROAD_POLICY_STAGE_t       policyStage,
                              BROAD_GROUP_t              group,
                              BROAD_POLICY_RULE_ENTRY_t *rulePtr,
                              bcm_pbmp_t                 pbm,
                              BROAD_ENTRY_t             *entry,
                              int                       *policer_id,      /* PTin added: SDK 6.3.0 */
                              int                       *counter_id)

{
    BROAD_POLICY_FIELD_t f;
    group_table_t       *groupPtr;
    bcm_field_entry_t    gid, eid;
    int                  rv = BCM_E_NONE;
    BROAD_ACTION_ENTRY_t *actionPtr;

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
                sysapiPrintf("- enter policy_cfp_group_add_rule \n");

    CHECK_GROUP(unit, policyStage, group);

    groupPtr = &group_table[unit][policyStage][group];

    if (!(groupPtr->flags & GROUP_USED))
        return BCM_E_NOT_FOUND;

    gid = groupPtr->gid;

    rv = bcm_field_entry_create(unit, groupPtr->gid, &eid);
    if (BCM_E_NONE != rv)
        return rv;

    *entry = BCM_ENTRY_TO_BROAD_ENTRY(eid);

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
            return rv;
          }
        }
    }

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

        /* PTin modified: SDK 6.3.0 */
        #if (SDK_VERSION_IS >= SDK_VERSION(5,6,0,0))
        rv = _policy_group_add_policer(unit, eid, gid, rulePtr);
        #else
        rv = _policy_group_add_meter(unit, eid, rulePtr);
        #endif
        if (BCM_E_NONE != rv)
            return rv;
    }
    /* PTin modified: Stats */
    #if (SDK_VERSION_IS < SDK_VERSION(5,6,0,0))
    else
    #endif
    if (rulePtr->ruleFlags & BROAD_COUNTER_SPECIFIED)
    {
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
            sysapiPrintf("- adding a counter\n");
        
        /* PTin modified: SDK 6.3.0 */
        #if (SDK_VERSION_IS >= SDK_VERSION(5,6,0,0))
        rv = _policy_group_add_stat(unit, eid, gid, rulePtr);
        #else
        rv = _policy_group_add_counter(unit, eid, rulePtr);
        #endif
        if (BCM_E_NONE != rv)
            return rv;
    }

    /* Ptin added: SDK 6.3.0 */
    #if 1
    if (policer_id!=L7_NULLPTR)   *policer_id = rulePtr->policer.policer_id;
    if (counter_id!=L7_NULLPTR)   *counter_id = rulePtr->counter.counter_id;
    #endif

    /* Only install to HW if we expect any ports to match this rule. This is mostly because
       for LOOKUP and EGRESS policies, the portClass doesn't really provide a mechanism to
       never match rule (as a NULL pbmp would do for ingress) */
    if ((policyStage == BROAD_POLICY_STAGE_INGRESS) || (BCM_PBMP_NOT_NULL(pbm)))
    {
      rv = bcm_field_entry_install(unit, eid);

      /* PTin added: SDK 6.3.0 */
      #if 1
      if ( rv != BCM_E_NONE)
      { 
        /* Destroy rule */
        (void) policy_cfp_group_delete_rule(unit, policyStage, gid, eid, rulePtr->policer.policer_id, rulePtr->counter.counter_id);
      }
      #endif
    }

    if (rv == BCM_E_NONE)
    {
      groupPtr->availableRules--;
    }

    return rv;
}

int policy_cfp_group_set_pbm(int                  unit,
                         BROAD_POLICY_STAGE_t policyStage,
                         BROAD_GROUP_t        group,
                         BROAD_ENTRY_t        entry,
                         bcm_pbmp_t           pbm,
                         bcm_pbmp_t           pbmMask)
{
    int               rv;
    group_table_t    *groupPtr;
    bcm_field_entry_t eid;
#ifdef BCM_ROBO_SUPPORT
    bcm_port_t port;
    bcm_port_t sport =0,smask=BCM_FIELD_EXACT_MATCH_MASK;
#endif

    CHECK_GROUP(unit,policyStage,group);

    groupPtr = &group_table[unit][policyStage][group];

    /* Without wide mode, VLAN ACLs don't support IPBM due to qset limitations, so quietly ignore */
    if (((BROAD_POLICY_TYPE_VLAN == groupPtr->type) || (BROAD_POLICY_TYPE_ISCSI == groupPtr->type)) && !_policy_supports_wide_mode(unit))
    {
        return BCM_E_NONE;
    }

    eid = BROAD_ENTRY_TO_BCM_ENTRY(entry);

    /* Reinstall only works for actions, so remove and install the entry explicitly. */
    rv = BCM_E_NONE;

#ifdef BCM_ROBO_SUPPORT    
    if(SOC_IS_ROBO5348( unit))
    {
    /*check of all ports set*/
      for(port = 0; port<53; port++)
      {
        if (PBMP_MEMBER((pbm), port))
        {
          continue;
        }
        break;
      }

      if(port == 53) 
      {
        sport  = 0;
        smask = 0;
      }
      else
      {
        for(port = 0; port<53; port++)
        {
          if (PBMP_MEMBER((pbm), port)) 
          {
            sport = port;
            smask = BCM_FIELD_EXACT_MATCH_MASK;
            break;
          }
        }
        if( port == 53 )
        {
          sport = 0;
          smask = 0;
        }
      }
      rv = bcm_field_qualify_SrcPort(unit, eid,0, BCM_FIELD_EXACT_MATCH_MASK,
          sport, smask);

    }
    else
#endif
    {
      pbmMask.pbits[0] =  ~pbmMask.pbits[0] ;
      rv = bcm_field_qualify_InPorts(unit, eid, pbm, pbmMask);
    }
    
    if (BCM_E_NONE != rv)
        return rv;

    rv = bcm_field_entry_install(unit, eid);
    if (BCM_E_NONE != rv)
        return rv;

    return rv;
}

int policy_cfp_group_set_portclass(int                  unit,
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

        /* PTin modified: SDK 6.3.0 */
        #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
        rv = bcm_field_qualify_InterfaceClassPort(unit, eid, 0, 0);
        #else
        rv = bcm_field_qualify_PortClass(unit, eid, 0, 0);
        #endif
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
        /* PTin modified: SDK 6.3.0 */
        #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
        rv = bcm_field_qualify_InterfaceClassPort(unit, eid, portClass, classMask);
        #else
        rv = bcm_field_qualify_PortClass(unit, eid, portClass, classMask);
        #endif
        if (BCM_E_NONE != rv)
            return rv;
      }

      rv = bcm_field_entry_install(unit, eid);
      if (BCM_E_NONE != rv)
          return rv;
    }

    return rv;
}

int policy_cfp_port_class_set(int                  unit,
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

int policy_cfp_group_rule_priority_set(int                          unit,
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
    case BROAD_POLICY_RULE_PRIORITY_DEFAULT:  prio = ((BCM_FIELD_ENTRY_PRIO_HIGHEST - BCM_FIELD_ENTRY_PRIO_LOWEST)/4)+BCM_FIELD_ENTRY_PRIO_LOWEST;
                                              break;
    case BROAD_POLICY_RULE_PRIORITY_HIGH:     prio = ((BCM_FIELD_ENTRY_PRIO_HIGHEST - BCM_FIELD_ENTRY_PRIO_LOWEST)/3)+BCM_FIELD_ENTRY_PRIO_LOWEST;
                                              break;
    case BROAD_POLICY_RULE_PRIORITY_HIGH2:    prio = ((BCM_FIELD_ENTRY_PRIO_HIGHEST - BCM_FIELD_ENTRY_PRIO_LOWEST)/2)+BCM_FIELD_ENTRY_PRIO_LOWEST;
                                              break;
    case BROAD_POLICY_RULE_PRIORITY_HIGHEST:  prio = BCM_FIELD_ENTRY_PRIO_HIGHEST;
                                              break;
    default:                                  prio = BCM_FIELD_ENTRY_PRIO_DONT_CARE;
    }

    rv = bcm_field_entry_prio_set(unit, eid, prio);

    return rv;
}

int policy_cfp_group_delete_rule(int                  unit,
                                 BROAD_POLICY_STAGE_t policyStage,
                                 BROAD_GROUP_t        group,
                                 BROAD_ENTRY_t        entry,
                                 int                  policer_id,   /* PTin added: SDK 6.3.0 */
                                 int                  counter_id)
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

    /* PTin added: SDK 6.3.0 */
    #if (SDK_VERSION_IS >= SDK_VERSION(5,6,0,0))
    if (policer_id>0)
    {
      rv = bcm_field_entry_policer_detach(unit, eid, 0);
      if (BCM_E_NONE != rv)
          return rv;
      rv = bcm_policer_destroy(unit, policer_id);
      if (BCM_E_NONE != rv)
          return rv;
    }
    if (counter_id>0)
    {
      rv = bcm_field_entry_stat_detach(unit, eid, counter_id);
      if (BCM_E_NONE != rv)
          return rv;
      rv = bcm_field_stat_destroy(unit, counter_id);
      if (BCM_E_NONE != rv)
          return rv;
    }
    #endif

    rv = bcm_field_entry_destroy(unit, eid);
    if (BCM_E_NONE != rv)
        return rv;

    groupPtr->availableRules++;

    return rv;
}

int policy_cfp_group_destroy(int unit, BROAD_POLICY_STAGE_t policyStage, BROAD_GROUP_t group)
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


int policy_cfp_group_get_stats(int                  unit,
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

    /* PTin modified: SDK 6.3.0 */
    #if 1
    if (val1!=NULL) COMPILER_64_ZERO(*val1);
    if (val2!=NULL) COMPILER_64_ZERO(*val2);
    #endif

    groupPtr = &group_table[unit][policyStage][group];

    if (!(groupPtr->flags & GROUP_USED))
        return BCM_E_NOT_FOUND;

    eid = BROAD_ENTRY_TO_BCM_ENTRY(entry);

    /* PTin added: SDK 6.3.0 */
    #if (SDK_VERSION_IS >= SDK_VERSION(5,6,0,0))
    int stat_size, stat_id;
    bcm_field_stat_t stat_type[2];
    uint64 values[2];

    /* Get stat id for this entry */
    rv = bcm_field_entry_stat_get(unit, entry, &stat_id);
    if ((BCM_E_NONE != rv) && (BCM_E_EMPTY != rv))
        return rv;

    /* Get number of counters */
    rv = bcm_field_stat_size(unit, stat_id, &stat_size);
    if ((BCM_E_NONE != rv) && (BCM_E_EMPTY != rv))
        return rv;
    /* Limit number of counters to 2 */
    if (stat_size>2)  stat_size = 2;

    if (stat_size==0)
      return BCM_E_EMPTY;

    /* Get collection of counters */
    rv = bcm_field_stat_config_get(unit, stat_id, stat_size, stat_type);
    if ((BCM_E_NONE != rv) && (BCM_E_EMPTY != rv))
        return rv;

    /* Get counters values */
    rv = bcm_field_stat_multi_get(unit, stat_id, stat_size, stat_type, values);
    if ((BCM_E_NONE != rv) && (BCM_E_EMPTY != rv))
        return rv;

    if (stat_size>=1 && val1!=NULL)  *val1 = values[0];
    if (stat_size>=2 && val2!=NULL)  *val2 = values[1];
    #else
    rv = bcm_field_counter_get(unit, eid, 0, val1);
    if ((BCM_E_NONE != rv) && (BCM_E_EMPTY != rv))   /* empty means no counter */
        return rv;

    if (policyStage != BROAD_POLICY_STAGE_EGRESS)
    {
      rv = bcm_field_counter_get(unit, eid, 1, val2);  /* empty means no counter */
      if ((BCM_E_NONE != rv) && (BCM_E_EMPTY != rv))
          return rv;
    }
    #endif

    return BCM_E_NONE;
}

int policy_cfp_group_stats_clear(int                  unit,
                                 BROAD_POLICY_STAGE_t policyStage,
                                 BROAD_ENTRY_t        entry)
{
    int                rv;
    bcm_field_entry_t  eid;
    uint64             val1;

    COMPILER_64_ZERO(val1);

    eid = BROAD_ENTRY_TO_BCM_ENTRY(entry);

     /* PTin modified: SDK 6.3.0 */
    #if (SDK_VERSION_IS >= SDK_VERSION(5,6,0,0))
    int                stat_id;   /* PTin added: SDK 6.3.0 */

    /* Get stat id for this entry */
    rv = bcm_field_entry_stat_get(unit, entry, &stat_id);
    if ((BCM_E_NONE != rv) && (BCM_E_EMPTY != rv))
        return rv;

    /* Reset counters */
    rv = bcm_field_stat_all_set(unit, stat_id, val1);
    if ((BCM_E_NONE != rv) && (BCM_E_EMPTY != rv))   /* empty means no counter */
        return rv;

    #else
    rv = bcm_field_counter_set(unit, eid, 0, val1);
    if ((BCM_E_NONE != rv) && (BCM_E_EMPTY != rv))   /* empty means no counter */
        return rv;

    if (policyStage != BROAD_POLICY_STAGE_EGRESS)
    {
      rv = bcm_field_counter_set(unit, eid, 1, val1);  /* empty means no counter */
      if ((BCM_E_NONE != rv) && (BCM_E_EMPTY != rv))
          return rv;
    }
    #endif

    return BCM_E_NONE;
}

void policy_cfp_group_dataplane_cleanup(int                  unit,
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
  if (BCM_E_NONE != rv)
  {
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_NONE)
        sysapiPrintf("- entry remove rv = %d\n", rv);
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
  if (BCM_E_NONE != rv)
  {
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_NONE)
        sysapiPrintf("- entry install rv = %d\n", rv);
    return;
  }

  return;
}

/* Debug */

void debug_cfp_group_stats(int unit)
{
    int groups_max;
    int rv, i;
    bcm_field_group_status_t gstats;

    #if 1
    sysapiPrintf("\nUnit %d Stats\n", unit);
    groups_max = _policy_group_total_slices(unit, BROAD_POLICY_STAGE_INGRESS);
    #else
    bcm_field_status_t       stats;

    sysapiPrintf("\nUnit %d Stats\n", unit);

    rv = bcm_field_status_get(unit, &stats);
    if (BCM_E_NONE == rv)
        sysapiPrintf("Groups free %d, total %d", stats.group_free, stats.group_total);
    else
    {
        sysapiPrintf("*** Error reading group stats ***");
        return;
    }

    sysapiPrintf("\n\n");

    groups_max = stats.group_total;
    #endif

    sysapiPrintf("Group  Prio   Entries   Counters   Meters\n");
    for (i = 1; i <= groups_max; i++)
    {
        sysapiPrintf("[%2d]   ", i);

        rv = bcm_field_group_status_get(unit, i, &gstats);
        if (BCM_E_NONE == rv)
            sysapiPrintf("[%d..%d] %d/%d %d/%d %d/%d",
                   gstats.prio_min, gstats.prio_max,
                   gstats.entries_free, gstats.entries_total,
                   gstats.counters_free, gstats.counters_total,
                   gstats.meters_free, gstats.meters_total);
        else
            sysapiPrintf("unused (%d)", rv);
        sysapiPrintf("\n");
    }
}

void debug_cfp_group_table(int unit)
{
    int                  i;
    BROAD_POLICY_STAGE_t policyStage;

    sysapiPrintf("\nUnit %d\n", unit);

    for (policyStage = BROAD_POLICY_STAGE_LOOKUP; policyStage < BROAD_POLICY_STAGE_COUNT; policyStage++)
    {
      if (policy_cfp_stage_supported(unit, policyStage) == L7_TRUE)
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
        sysapiPrintf("Group   GID   Type   SQset   BaseGroup   availableRules\n");
        for (i = 0; i < group_table_size[unit][policyStage]; i++)
        {
            sysapiPrintf("[%2d]   ", i);

            if (group_table[unit][policyStage][i].flags & GROUP_USED)
            {
                sysapiPrintf("%4d    %2d    %2d      %2d          %4d", 
                       group_table[unit][policyStage][i].gid, group_table[unit][policyStage][i].type,
                       group_table[unit][policyStage][i].sqset, group_table[unit][policyStage][i].base_group,
                       group_table[unit][policyStage][i].availableRules);
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

void debug_cfp_sqset_table(int unit)
{
    int i, j;

    for (i = 0; i < SUPER_QSET_TABLE_SIZE; i++)
    {
        sysapiPrintf("SQset %d", i);

        if (super_qset_table[unit][i].flags & SUPER_QSET_USED)
        {
            sysapiPrintf("\n- qsetAgg: ");
            for (j = 0; j < bcmFieldQualifyCount; j++)
                sysapiPrintf ("%d", (BCM_FIELD_QSET_TEST(super_qset_table[unit][i].qsetAgg, j) ? 1 : 0));
            sysapiPrintf("\n- qsetUdf: ");
            for (j = 0; j < bcmFieldQualifyCount; j++)
                sysapiPrintf ("%d", (BCM_FIELD_QSET_TEST(super_qset_table[unit][i].qsetUdf, j) ? 1 : 0));
        }
        else
        {
            sysapiPrintf("unused");
        }
        sysapiPrintf("\n");
    }
}

void debug_cfp_entry_counter(int unit, bcm_field_entry_t eid)
{
    int    rv;

    /* PTin modified: SDK 6.3.0 */
    #if (SDK_VERSION_IS >= SDK_VERSION(5,6,0,0))
    int    i;
    int    stat_id;
    int    stat_size;
    bcm_field_stat_t stat_type[4];
    uint64 values[4];

    /* Get stat id for this entry */
    rv = bcm_field_entry_stat_get(unit, eid, &stat_id);
    if ((BCM_E_NONE != rv) && (BCM_E_EMPTY != rv))
    {
      sysapiPrintf("Error with bcm_field_entry_stat_get\n");
      return;
    }

    /* Get number of counters */
    rv = bcm_field_stat_size(unit, stat_id, &stat_size);
    if ((BCM_E_NONE != rv) && (BCM_E_EMPTY != rv))
    {
      sysapiPrintf("Error with bcm_field_stat_size\n");
      return;
    }
    /* Limit number of counters to 2 */
    if (stat_size>4)  stat_size = 4;

    if (stat_size==0)
    {
      sysapiPrintf("No counters\n");
      return;
    }

    /* Get collection of counters */
    rv = bcm_field_stat_config_get(unit, stat_id, stat_size, stat_type);
    if ((BCM_E_NONE != rv) && (BCM_E_EMPTY != rv))
    {
      sysapiPrintf("Error with bcm_field_stat_config_get\n");
      return;
    }

    /* Get counters values */
    rv = bcm_field_stat_multi_get(unit, stat_id, stat_size, stat_type, values);
    if ((BCM_E_NONE != rv) && (BCM_E_EMPTY != rv))
    {
      sysapiPrintf("Error with bcm_field_stat_multi_get\n");
      return;
    }

    /* Print counters */
    for (i=0; i<stat_size; i++)
    {
      sysapiPrintf("Counter %d: %08x %08x\n", i, u64_H(values[i]), u64_L(values[i]));
    }
    #else
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
    #endif
}

