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

#include "broad_group_dnx.h"
#include "bcm/field.h"
#include "bcm/policer.h"    /* PTin added: policer */
#include "ibde.h"
#include "sal/core/libc.h"
#include "osapi_support.h"
#include "broad_group_sqset.h"
#include "platform_config.h"

/* PTin added: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
#include "bcm_int/ea/tk371x/field.h"
#endif

/* used for the Higig B0 workaround */
#include <soc/drv.h>
#include <bcm_int/control.h>

#include "logger.h"

extern L7_int32 hpcBroadMasterCpuModPortGet(L7_int32 *modid, L7_int32 *cpuport);


#define MASK_NONE    (~0)
#define MASK_ALL     (0)

#define CHECK_UNIT(u)    {if (u >= SOC_MAX_NUM_DEVICES) L7_LOG_ERROR(u);}

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
    bcmFieldQualifySrcIp6,         /* Source IPv6 Address */
    bcmFieldQualifyDstIp6,         /* Destination IPv6 Address */
    bcmFieldQualifyIp6FlowLabel,   /* IPv6 Flow Label */
    bcmFieldQualifyIp6TrafficClass,/* IPv6 Traffic Class */
    //customFieldQualifyIcmpMsgType, /* ICMP Message Type   */
    bcmFieldQualifyDstClassField,  /* Class ID from VFP, to be used in IFP - PTin added: FP */
    bcmFieldQualifySrcClassField,  /* Class ID from VFP, to be used in IFP - PTin added: FP */
    bcmFieldQualifyTcpControl,
    bcmFieldQualifyVlanFormat,     /* VLAN Format */
    bcmFieldQualifyL2Format,       /* L2 Header Format */
    bcmFieldQualifyIpType,         /* IP Type */
    bcmFieldQualifyInPorts,        /* InPorts, PTin added: FP */
    bcmFieldQualifyOutPort,        /* OutPort, PTin added: FP */
    bcmFieldQualifyInterfaceClassPort,
    bcmFieldQualifyDrop,           /* Drop, PTin added: FP */
    bcmFieldQualifyL2SrcHit,       /* L2 Source hit, PTin added: FP */
    bcmFieldQualifyL2DestHit,      /* L2 Destination hit, PTin added: FP */
    bcmFieldQualifyIntPriority     /* Internal priority, PTin added: FP */
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
    bcm_field_action_t action[ACTIONS_PER_MAP_ENTRY];
}
action_map_entry_t;

static action_map_entry_t ingress_action_map[BROAD_ACTION_LAST] =
{
    /* SOFT_DROP - do not switch */
    {
        { bcmFieldActionDrop, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE}
    },
    /* HARD_DROP - override all other rules */
    {
        { bcmFieldActionDrop, bcmFieldActionCopyToCpuCancel, bcmFieldActionRedirectCancel, PROFILE_ACTION_NONE}
    },
    /* PERMIT - default behavior */
    {
        { bcmFieldActionDropCancel, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE}
    },
    /* REDIRECT */
    {
        { bcmFieldActionRedirect,   PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE},
    },
    /* MIRROR */
    {
        { bcmFieldActionMirrorIngress, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE}
    },
    /* TRAP_TO_CPU */
    {
        { bcmFieldActionRedirect, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE}
    },
    /* COPY_TO_CPU */
    {
        { bcmFieldActionCopyToCpu, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE}
    },
    /* TS_TO_CPU */
    {
        { bcmFieldActionTimeStampToCpu, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE}
    },
    /* SET_COSQ */
    {
        /* PTin modified: QOS */
        { bcmFieldActionPrioIntNew, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE}
    },
    /* SET_DSCP */
    {
        { bcmFieldActionDscpNew, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE}
    },
    /* SET_TOS */
    {
        { bcmFieldActionDscpNew, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE}
    },
    /* SET_USERPRIO */
    {
        { bcmFieldActionPrioPktNew, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE}
    },
    /* PTin added */
    /* SET_USERPRIO_INNERTAG */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_DROPPREC */
    {
        { bcmFieldActionDropPrecedence, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE}
    },
    /* SET_OUTER_VID */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_INNER_VID */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* ADD_OUTER_VID */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* ADD_INNER_VID */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* DO_NOT_LEARN */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_CLASS_ID */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* PTin added: FP */
    /* SET_SRC_CLASS_ID */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_REASON_CODE */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_USERPRIO_AS_INNER_DOT1P*/
    {
        { bcmFieldActionPrioPktCopy, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE}
    },
};

static action_map_entry_t egress_action_map[BROAD_ACTION_LAST] =
{
    /* SOFT_DROP - do not switch */
    {
        { bcmFieldActionDrop, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE}
    },
    /* HARD_DROP - override all other rules */
    {
        { bcmFieldActionDrop, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE}
    },
    /* PERMIT - default behavior */
    {
        { bcmFieldActionDropCancel, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE}
    },
    /* REDIRECT */
    {
        { bcmFieldActionRedirect, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE}
    },
    /* MIRROR */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* TRAP_TO_CPU */
    {
        { bcmFieldActionRedirect, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE}
    },
    /* COPY_TO_CPU */
    {
        { PROFILE_ACTION_INVALID,  PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* TS_TO_CPU */
    {
        { PROFILE_ACTION_INVALID,  PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_COSQ */
    {
        { PROFILE_ACTION_INVALID,   PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_DSCP */
    {
        { bcmFieldActionDscpNew, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE}
    },
    /* SET_TOS */
    {
        { bcmFieldActionDscpNew, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE}
    },
    /* SET_USERPRIO */
    {
        { bcmFieldActionOuterVlanPrioNew, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE, PROFILE_ACTION_NONE}
    },
    /* PTin added */
    /* SET_USERPRIO_INNERTAG */
    {
        { bcmFieldActionInnerVlanPrioNew, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE}
    },
    /* SET_DROPPREC */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_OUTER_VID */
    {
        { bcmFieldActionOuterVlanNew, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE}
    },
    /* SET_INNER_VID */
    {
        { bcmFieldActionInnerVlanNew, PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE,    PROFILE_ACTION_NONE}
    },
    /* ADD_OUTER_VID */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* ADD_INNER_VID */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* DO_NOT_LEARN */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_CLASS_ID */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* PTin added: FP */
    /* SET_SRC_CLASS_ID */
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_REASON_CODE*/
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },
    /* SET_USERPRIO_AS_INNER_DOT1P*/
    {
        { PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID, PROFILE_ACTION_INVALID}
    },

};

/* Drop Precedence (Color) Map */
uint32 color_map[BROAD_COLOR_LAST] = 
{
    BCM_FIELD_COLOR_GREEN,
    BCM_FIELD_COLOR_YELLOW,
    BCM_FIELD_COLOR_RED
};

//static int policy_udf_id[SOC_MAX_NUM_DEVICES];

#define SUPER_XSET_TABLE_SIZE  32    /* total number of super qsets */

static super_xset_entry_t super_xset_table[SOC_MAX_NUM_DEVICES][SUPER_XSET_TABLE_SIZE];

static group_table_t *group_table[SOC_MAX_NUM_DEVICES][BROAD_POLICY_STAGE_COUNT];
static int            group_table_size[SOC_MAX_NUM_DEVICES][BROAD_POLICY_STAGE_COUNT];

#define CHECK_GROUP(u,s,g)  {if ((u>=SOC_MAX_NUM_DEVICES)||(s>=BROAD_POLICY_STAGE_COUNT)||(g>=group_table_size[u][s])) L7_LOG_ERROR(g);}

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

/* Utility Functions */

int _policy_set_subset(bcm_field_qset_t q1, custom_field_qset_t custom_q1, 
                       bcm_field_qset_t q2, custom_field_qset_t custom_q2)
{
    int  i;

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
    {
      sysapiPrintf("%s(%d) - Qset1:\r\n", __FUNCTION__,__LINE__);
      debug_print_xset(&q1);
      sysapiPrintf("%s(%d) - Qset2:\r\n", __FUNCTION__,__LINE__);
      debug_print_xset(&q2);
    }

    for (i = 0; i < bcmFieldQualifyCount; i++)
    {
        if (BCM_FIELD_QSET_TEST(q1,i) && !BCM_FIELD_QSET_TEST(q2,i))
        {
          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
            sysapiPrintf("%s(%d) - Qualifier %u failed\r\n", __FUNCTION__,__LINE__, i);
          return BCM_E_FAIL;
        }
    }

    for (i = customFieldQualifyFirst; i < customFieldQualifyLast; i++)
    {
        if (CUSTOM_FIELD_QSET_TEST(custom_q1,i) && !CUSTOM_FIELD_QSET_TEST(custom_q2,i))
        {
          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
            sysapiPrintf("%s(%d) - Qualifier %u failed\r\n", __FUNCTION__,__LINE__, i);
          return BCM_E_FAIL;
        }
    }

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
      sysapiPrintf("%s(%d) - Qsets match\r\n", __FUNCTION__,__LINE__);

    return BCM_E_NONE;
}

void _policy_qset_union(bcm_field_qset_t q1, bcm_field_qset_t *q2)
{
    int  i;

    for (i = 0; i < bcmFieldQualifyCount; i++)
    {
        if (BCM_FIELD_QSET_TEST(q1,i))
            BCM_FIELD_QSET_ADD(*q2,i);
    }
}

void _policy_aset_union(bcm_field_aset_t a1, bcm_field_aset_t *a2)
{
    int  i;

    for (i = 0; i < bcmFieldActionCount; i++)
    {
        if (BCM_FIELD_ASET_TEST(a1,i))
            BCM_FIELD_ASET_ADD(*a2,i);
    }
}

/* Super Qset Functions */

int _policy_super_xset_find_match(int                  unit, 
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

  for (i = 0; i < SUPER_XSET_TABLE_SIZE; i++)
  {
    if (super_xset_table[unit][i].flags & SUPER_XSET_USED)
    {
      bcm_field_qset_t qsetFull;

      if ((super_xset_table[unit][i].applicablePolicyTypes & (1 << type)) == 0)
      {
        /* If this sqset isn't applicable for this type of policy, continue. */
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
          sysapiPrintf("%s(%d) qualifier i=%d not selected: Continuing...\n", __FUNCTION__, __LINE__, i);
        continue;
      }

      if (super_xset_table[unit][i].sqsetWidth != qsetWidth)
      {
        /* If this sqset doesn't have the proper width for this policy, continue. */
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
          sysapiPrintf("%s(%d) Different width for i=%d (sqsetwidth=%u VS qsetwidth=%u): Continuing...\n", __FUNCTION__, __LINE__, i,
                       super_xset_table[unit][i].sqsetWidth, qsetWidth);
        continue;
      }

      BCM_FIELD_QSET_INIT(qsetFull);
      _policy_qset_union(super_xset_table[unit][i].qsetAgg, &qsetFull);

//    printf("qsetFull  : ");
//    for (j=0; j<_SHR_BITDCLSIZE(BCM_FIELD_QUALIFY_MAX); j++)
//    {
//      printf("%08X ",qsetFull.w[j]);
//    }
//    printf("\r\n");

      rv = _policy_set_subset(qset, customQset, qsetFull, super_xset_table[unit][i].customQset);
      if (BCM_E_NONE == rv)
      {
        *idx = i;
        /* If this sqset doesn't have the proper width for this policy, continue. */
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          sysapiPrintf("%s(%d) Selected table entry: i=%d\n", __FUNCTION__, __LINE__, i);
        return BCM_E_NONE;
      }
    }
  }

  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
    sysapiPrintf("%s(%d) failed!\n", __FUNCTION__, __LINE__);

  return BCM_E_FAIL;
}

static int _policy_super_xset_find_free(int unit, int *idx)
{
    int  i;

    for (i = 0; i < SUPER_XSET_TABLE_SIZE; i++)
    {
        if (!(super_xset_table[unit][i].flags & SUPER_XSET_USED))
        {
            *idx = i;
            return BCM_E_NONE;
        }
    }

    return BCM_E_FAIL;
}

static int _policy_super_xset_add(int                      unit,
                                  super_xset_definition_t *sxset_def,
                                  L7_BOOL                 *applicablePolicyTypes)
{
    int                      i;
    int                      rv = BCM_E_NONE;
    bcm_field_qset_t         qset1;
    bcm_field_aset_t         aset1;
    super_xset_entry_t      *xsetPtr;
    bcm_field_qualify_t     *q1;
    L7_uint32                q1Size;
    custom_field_qualify_t  *q2;
    L7_uint32                q2Size;
    bcm_field_action_t      *a1;
    L7_uint32                a1Size;
    L7_uint32                gid;
    bcm_field_group_config_t group_config;

    CHECK_UNIT(unit);

    q1     = sxset_def->standardQualifiers;
    q1Size = sxset_def->standardQualifiersCount;
    q2     = sxset_def->customQualifiers;
    q2Size = sxset_def->customQualifiersCount;
    a1     = sxset_def->standardActions;
    a1Size = sxset_def->standardActionsCount;

    rv = _policy_super_xset_find_free(unit, &i);
    if (BCM_E_NONE != rv)
    {
      L7_LOG_ERROR(rv);  /* Need to increase SUPER_QSET_TABLE_SIZE. */
      return rv;
    }

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
      sysapiPrintf("%s(%d) qset index=%d\n", __FUNCTION__, __LINE__, i);

    xsetPtr = &super_xset_table[unit][i];

    xsetPtr->flags = SUPER_XSET_USED;

    if(BROAD_POLICY_TYPE_LAST > 32)
    {
      /* We can only accommodate 32 bits in the applicablePolicyTypes mask... 
      need update the structure to have an additional mask. */
      L7_LOG_ERROR(0);
    }

    xsetPtr->applicablePolicyTypes = 0;
    for (i = 0; i < BROAD_POLICY_TYPE_LAST; i++)
    {
      if (applicablePolicyTypes[i] == L7_TRUE)
      {
        xsetPtr->applicablePolicyTypes |= (1 << i);
      }
    }

    BCM_FIELD_QSET_INIT(qset1);
    BCM_FIELD_ASET_INIT(aset1);

    /* initialize the 1st component qset */
    for (i = 0; i < q1Size; i++)
        BCM_FIELD_QSET_ADD(qset1, q1[i]);
    for (i = 0; i < a1Size; i++)
        BCM_FIELD_ASET_ADD(aset1, a1[i]);

    /* initialize the custom qset */
    CUSTOM_FIELD_QSET_INIT(xsetPtr->customQset);
    for (i = 0; i < q2Size; i++)
        CUSTOM_FIELD_QSET_ADD(xsetPtr->customQset, q2[i]);

    BCM_FIELD_QSET_INIT(xsetPtr->qset1);
    BCM_FIELD_QSET_INIT(xsetPtr->qsetAgg);
    _policy_qset_union(qset1, &xsetPtr->qset1);
    _policy_qset_union(qset1, &xsetPtr->qsetAgg);
    

    BCM_FIELD_ASET_INIT(xsetPtr->aset);
    BCM_FIELD_ASET_INIT(xsetPtr->asetAgg);
    _policy_aset_union(aset1, &xsetPtr->aset);
    _policy_aset_union(aset1, &xsetPtr->asetAgg);

    /* Temporarily create a group using this qset, then check w/ the SDK to determine how many slices it needs. */
    do
    {
      bcm_field_group_config_t_init(&group_config);
      group_config.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE |
                           /*BCM_FIELD_GROUP_CREATE_LARGE |*/
                           BCM_FIELD_GROUP_CREATE_WITH_ASET;
      group_config.qset  = xsetPtr->qsetAgg;
      group_config.mode  = bcmFieldGroupModeDefault;
      group_config.aset  = xsetPtr->asetAgg;

      rv = bcm_field_group_config_create(unit, &group_config);

      //rv = bcm_field_group_create_mode(unit, xsetPtr->qsetAgg, 0, bcmFieldGroupModeAuto, &gid);
      if (rv != BCM_E_NONE)
      {
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          sysapiPrintf("%s(%d) bcm_field_group_create_mode: rv=%d\n", __FUNCTION__, __LINE__, rv);
        break;
      }
      /* Extract gid */
      gid = group_config.group;

      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("%s(%d) gid=%d\n", __FUNCTION__, __LINE__, gid);

      rv = bcm_field_group_status_get(unit, gid, &xsetPtr->status);
      if (rv != BCM_E_NONE)
      {
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          sysapiPrintf("%s(%d) bcm_field_group_status_get: gid=%d, rv=%d\n", __FUNCTION__, __LINE__, gid, rv);
        break;
      }

      _policy_group_status_to_sxset_width(&xsetPtr->status, &xsetPtr->sqsetWidth);

      (void)bcm_field_group_destroy(unit, gid);

    } while (0);

    if (rv != BCM_E_NONE)
    {
      /* If there were any errors, clean up the sqset table entry. */
      memset(xsetPtr, 0, sizeof(*xsetPtr));
    }

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
      sysapiPrintf("%s(%d) qset_add: rv=%d\n", __FUNCTION__, __LINE__, rv);

    return rv;
}

static int _policy_super_xset_init_ifp(int unit)
{
  L7_BOOL applicable_policy_types[BROAD_POLICY_TYPE_LAST];

  memset(applicable_policy_types, 0, sizeof(applicable_policy_types));
  applicable_policy_types[BROAD_POLICY_TYPE_PORT] = L7_TRUE;
  applicable_policy_types[BROAD_POLICY_TYPE_VLAN] = L7_TRUE;

  _policy_super_xset_add(unit, &l2l3l4Xgs4ClassIdQsetDef, applicable_policy_types);
  _policy_super_xset_add(unit, &ipv6SrcL4QsetDef, applicable_policy_types);
  _policy_super_xset_add(unit, &ipv6DstL4QsetDef, applicable_policy_types);

  memset(applicable_policy_types, 0, sizeof(applicable_policy_types));
  applicable_policy_types[BROAD_POLICY_TYPE_SYSTEM]      = L7_TRUE;
#if 0
  applicable_policy_types[BROAD_POLICY_TYPE_SYSTEM_PORT] = L7_TRUE;
  applicable_policy_types[BROAD_POLICY_TYPE_COSQ]        = L7_TRUE;
#endif
  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
    sysapiPrintf("Adding qset systemQsetArad\r\n");

  /* Doublewide mode. */
  _policy_super_xset_add(unit, &systemXsetAradDef, applicable_policy_types);

  /* PTin added: ICAP */
  memset(applicable_policy_types, 0, sizeof(applicable_policy_types));
  applicable_policy_types[BROAD_POLICY_TYPE_PTIN]        = L7_TRUE;
  applicable_policy_types[BROAD_POLICY_TYPE_STAT_EVC]    = L7_TRUE;
  applicable_policy_types[BROAD_POLICY_TYPE_STAT_CLIENT] = L7_TRUE;

  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
    sysapiPrintf("Adding qset systemQsetPTin\r\n");

  /* Doublewide mode. */
  _policy_super_xset_add(unit, &systemQsetPTinDef, applicable_policy_types);
#if 0
  /* The following sqset is used for iSCSI control rules. */
  memset(applicable_policy_types, 0, sizeof(applicable_policy_types));
  applicable_policy_types[BROAD_POLICY_TYPE_ISCSI] = L7_TRUE;

  _policy_super_qset_add(unit, &iscsiQsetDef, applicable_policy_types);
#endif
  /* The following sqsets are used only for user policies. */
  memset(applicable_policy_types, 0, sizeof(applicable_policy_types));
  applicable_policy_types[BROAD_POLICY_TYPE_VLAN]        = L7_TRUE;
  _policy_super_xset_add(unit, &vlanl3QsetDef,      applicable_policy_types);

  /* The following sqset is used for IPSG policies but may also be used for user
     policies on devices that do not support doublewide mode. */
  memset(applicable_policy_types, 0, sizeof(applicable_policy_types));
  applicable_policy_types[BROAD_POLICY_TYPE_PORT] = L7_TRUE;
  applicable_policy_types[BROAD_POLICY_TYPE_VLAN] = L7_TRUE;
  _policy_super_xset_add(unit, &l2l3SrcQsetDef, applicable_policy_types);

  /* The following sqsets are used only for user policies. */
  memset(applicable_policy_types, 0, sizeof(applicable_policy_types));
  applicable_policy_types[BROAD_POLICY_TYPE_PORT]        = L7_TRUE;
  applicable_policy_types[BROAD_POLICY_TYPE_VLAN]        = L7_TRUE;

  _policy_super_xset_add(unit, &l2l3DstQsetDef,     applicable_policy_types);

  return BCM_E_NONE;
}

static int _policy_super_xset_init_efp(int unit)
{
  L7_BOOL applicable_policy_types[BROAD_POLICY_TYPE_LAST];

  if(policy_stage_supported(unit, BROAD_POLICY_STAGE_EGRESS))
  {
    memset(applicable_policy_types, 0, sizeof(applicable_policy_types));
    applicable_policy_types[BROAD_POLICY_TYPE_SYSTEM]      = L7_TRUE;
    applicable_policy_types[BROAD_POLICY_TYPE_PORT]        = L7_TRUE;
    applicable_policy_types[BROAD_POLICY_TYPE_VLAN]        = L7_TRUE;
    applicable_policy_types[BROAD_POLICY_TYPE_PTIN]        = L7_TRUE;   /* PTin added: policer */
    applicable_policy_types[BROAD_POLICY_TYPE_STAT_EVC]    = L7_TRUE;   /* PTin added: stats */
    applicable_policy_types[BROAD_POLICY_TYPE_STAT_CLIENT] = L7_TRUE;   /* PTin added: stats */

    _policy_super_xset_add(unit, &l2QsetEgressDef, applicable_policy_types);

    _policy_super_xset_add(unit, &l3l4QsetEgressDef, applicable_policy_types);

    //_policy_super_xset_add(unit, &ipv6L3L4QsetEgressDef, applicable_policy_types);
  }

  return BCM_E_NONE;
}

static int _policy_super_xset_init(int unit)
{
  int  i, j;
  char str[301], val[21];

  for (i = 0; i < SUPER_XSET_TABLE_SIZE; i++)
  {
    super_xset_table[unit][i].flags = SUPER_XSET_NONE;
  }

  _policy_super_xset_init_ifp(unit);
  _policy_super_xset_init_efp(unit);

  for (i = 0; i < SUPER_XSET_TABLE_SIZE; i++)
  {
    sprintf(str,"super_qset_table %-2u (width=%u, flags=0x%08x): ",i,super_xset_table[unit][i].sqsetWidth,super_xset_table[unit][i].flags);
    for (j = 0; j < _SHR_BITDCLSIZE(BCM_FIELD_QUALIFY_MAX); j++)
    {
      sprintf(val,"%08X ",super_xset_table[unit][i].qsetAgg.w[j]);
      strcat(str, val);
    }
    PT_LOG_TRACE(LOG_CTX_STARTUP, "%s", str);
  }

  return BCM_E_NONE;
}

static int _policy_action_map_init(int unit)
{
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
    case BROAD_POLICY_TYPE_IPSG:
    case BROAD_POLICY_TYPE_LLPF:
        *block = ALLOC_BLOCK_LOW;
        *dir   = ALLOC_LOW_TO_HIGH;
        break;
    case BROAD_POLICY_TYPE_VLAN:
    case BROAD_POLICY_TYPE_ISCSI:
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
        *block = ALLOC_BLOCK_MEDIUM;
        *dir   = ALLOC_LOW_TO_HIGH;
        break;
    default:
        L7_LOG_ERROR(type);
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
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          sysapiPrintf("%s(%d) Incompatible group types (type1=%u VS type2=%u)\r\n", __FUNCTION__,__LINE__, group1_type, group2_type);
      }
      else if ((group2_type == BROAD_POLICY_TYPE_COSQ) && SOC_IS_HELIX1(unit))
      {
        groupTypesCompatible = L7_FALSE;
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          sysapiPrintf("%s(%d) Incompatible group types (type1=%u VS type2=%u)\r\n", __FUNCTION__,__LINE__, group1_type, group2_type);
      }
#ifndef L7_IPV6_PACKAGE
      else if ((group2_type == BROAD_POLICY_TYPE_COSQ) && SOC_IS_SCORPION(unit))
      {
        groupTypesCompatible = L7_FALSE;
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          sysapiPrintf("%s(%d) Incompatible group types (type1=%u VS type2=%u)\r\n", __FUNCTION__,__LINE__, group1_type, group2_type);
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
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          sysapiPrintf("%s(%d) Incompatible group types (type1=%u VS type2=%u)\r\n", __FUNCTION__,__LINE__, group1_type, group2_type);
      }
      else if ((group2_type == BROAD_POLICY_TYPE_COSQ) && SOC_IS_HELIX1(unit))
      {
        groupTypesCompatible = L7_FALSE;
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          sysapiPrintf("%s(%d) Incompatible group types (type1=%u VS type2=%u)\r\n", __FUNCTION__,__LINE__, group1_type, group2_type);
      }
#ifndef L7_IPV6_PACKAGE
      else if ((group2_type == BROAD_POLICY_TYPE_COSQ) && SOC_IS_SCORPION(unit))
      {
        groupTypesCompatible = L7_FALSE;
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          sysapiPrintf("%s(%d) Incompatible group types (type1=%u VS type2=%u)\r\n", __FUNCTION__,__LINE__, group1_type, group2_type);
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
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          sysapiPrintf("%s(%d) Incompatible group types (type1=%u VS type2=%u)\r\n", __FUNCTION__,__LINE__, group1_type, group2_type);
      }
#ifndef L7_IPV6_PACKAGE
      else if (SOC_IS_SCORPION(unit))
      {
        groupTypesCompatible = L7_FALSE;
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          sysapiPrintf("%s(%d) Incompatible group types (type1=%u VS type2=%u)\r\n", __FUNCTION__,__LINE__, group1_type, group2_type);
      }
#endif
    }
    else if (group1_type == BROAD_POLICY_TYPE_IPSG)
    {
      if (group2_type != BROAD_POLICY_TYPE_LLPF)
      {
        groupTypesCompatible = L7_FALSE;
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          sysapiPrintf("%s(%d) Incompatible group types (type1=%u VS type2=%u)\r\n", __FUNCTION__,__LINE__, group1_type, group2_type);
      }
    }
    else if (group1_type == BROAD_POLICY_TYPE_LLPF)
    {
      if (group2_type != BROAD_POLICY_TYPE_IPSG)
      {
        groupTypesCompatible = L7_FALSE;
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          sysapiPrintf("%s(%d) Incompatible group types (type1=%u VS type2=%u)\r\n", __FUNCTION__,__LINE__, group1_type, group2_type);
      }
    }
    else
    {
      groupTypesCompatible = L7_FALSE;
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("%s(%d) Incompatible group types (type1=%u VS type2=%u)\r\n", __FUNCTION__,__LINE__, group1_type, group2_type);
    }
  }

  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
    sysapiPrintf("%s(%d) Compatible group types (type1=%u VS type2=%u)\r\n", __FUNCTION__,__LINE__, group1_type, group2_type);

  return groupTypesCompatible;
}

static int _policy_group_find_first(int                  unit, 
                                    BROAD_POLICY_STAGE_t policyStage,
                                    BROAD_POLICY_TYPE_t  type, 
                                    BROAD_GROUP_t       *group)
{
  group_alloc_block_t block/*, used_block*/;
  group_alloc_dir_t   dir/*,   used_dir*/;
  group_table_t      *groupPtr;
  super_xset_entry_t  sxsetInfo;

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
    _policy_super_xset_get(unit, groupPtr->sqset, &sxsetInfo);
    *group -= *group % sxsetInfo.status.slice_width_physical;
    groupPtr = &group_table[unit][policyStage][*group];

    /* make sure that the block that this group belongs to matches
       the block requested */
    if (_policy_group_types_compatible(unit, type, groupPtr->type) == L7_FALSE)
    {
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("- Incompatible group types (%u VS %u)\n", type, groupPtr->type);
      return BCM_E_FAIL;
    }

    /* PTin removed: excess validations */
    #if 0
    _policy_group_alloc_type(groupPtr->type, &used_block, &used_dir);
    if (block != used_block)
    {
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("- Different blocks (%u VS %u)\n", block, used_block);
      return BCM_E_FAIL;
    }
    #endif
  }

  return BCM_E_NONE;
}

static int _policy_group_decrement(int                   unit,
                                   BROAD_POLICY_STAGE_t  policyStage,
                                   BROAD_GROUP_t        *group, 
                                   group_alloc_block_t   block, 
                                   BROAD_POLICY_TYPE_t   type)
{
  //group_alloc_block_t used_block;
  //group_alloc_dir_t   used_dir;
  group_table_t      *groupPtr;
  super_xset_entry_t  sxsetInfo;

  /* If the group is within range of this block and it is in use
     and it is multi-slice, set the group to the first slice. */
  if ((*group >= group_alloc_table[unit][policyStage][block].lowPrio) &&
      (*group <= group_alloc_table[unit][policyStage][block].highPrio))
  {
    groupPtr = &group_table[unit][policyStage][*group];
    if (groupPtr->flags & GROUP_USED)
    {
      /* Set group to the first slice of the group. */
      _policy_super_xset_get(unit, groupPtr->sqset, &sxsetInfo);
      *group -= *group % sxsetInfo.status.slice_width_physical;
    }
  }

  if ((*group - 1) >= group_alloc_table[unit][policyStage][block].lowPrio)
  {
    *group = *group - 1;

    groupPtr = &group_table[unit][policyStage][*group];
    if (groupPtr->flags & GROUP_USED)
    {
      /* Set group to the first slice of the group. */
      _policy_super_xset_get(unit, groupPtr->sqset, &sxsetInfo);
      *group -= *group % sxsetInfo.status.slice_width_physical;
      groupPtr = &group_table[unit][policyStage][*group];

      /* make sure that the block that this group belongs to matches
         the block requested */
      if (_policy_group_types_compatible(unit, type, groupPtr->type) == L7_FALSE)
      {
        return BCM_E_FAIL;
      }
      /* PTin removed: excess validations */
      #if 0
      _policy_group_alloc_type(groupPtr->type, &used_block, &used_dir);
      if (block != used_block)
      {
        return BCM_E_FAIL;
      }
      #endif
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
  //group_alloc_block_t used_block;
  //group_alloc_dir_t   used_dir;
  group_table_t      *groupPtr;
  super_xset_entry_t  sxsetInfo;

  /* If the group is within range of this block and it is in use
     and it is multi-slice, set the group to the last slice. */
  if ((*group >= group_alloc_table[unit][policyStage][block].lowPrio) &&
      (*group <= group_alloc_table[unit][policyStage][block].highPrio))
  {
    groupPtr = &group_table[unit][policyStage][*group];
    if (groupPtr->flags & GROUP_USED)
    {
      /* Set group to the last slice of the group. */
      _policy_super_xset_get(unit, groupPtr->sqset, &sxsetInfo);
      *group += sxsetInfo.status.slice_width_physical - 1;
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
      /* PTin removed: excess validations */
      #if 0
      _policy_group_alloc_type(groupPtr->type, &used_block, &used_dir);
      if (block != used_block)
      {
        return BCM_E_FAIL;
      }
      #endif
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

int _policy_group_calc_xset(int                             unit,
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
        /* PTin removed: Stats */
        #if (SDK_VERSION_IS < SDK_VERSION(5,6,0,0))
        /* meters use a counter as well */
        (resourceReq->counterCount)++;
        #endif
        (resourceReq->meterCount)++;
      }
      /* PTin modified: Stats */
      #if (SDK_VERSION_IS < SDK_VERSION(5,6,0,0))
      else
      #endif
      if (rulePtr->ruleFlags & BROAD_COUNTER_SPECIFIED)
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

              value = (char*)hapiBroadPolicyFieldValuePtr(&(rulePtr->fieldInfo), f);
              mask  = (char*)hapiBroadPolicyFieldMaskPtr(&(rulePtr->fieldInfo), f);

              switch (f)
              {
              /* DNX: todo */
              #if 0
              /* custom fields go here */
              case BROAD_FIELD_ICMP_MSG_TYPE:
                /* Use L4 src port for ICMP Msg Type. We can do this because
                   XGS3 just treats the L4 src port field as the first two
                   bytes following the IP header. */
                BCM_FIELD_QSET_ADD(resourceReq->qsetAgg,bcmFieldQualifyL4SrcPort);
                break;
              #endif

              default:
                rv = _policy_field_to_bcm_field(f, entryPtr->policyStage, (char*)value, (char*)mask, &bcm_field, resourceReq->requiresEtype);
                if (BCM_E_NONE != rv)
                {
                  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
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
      if (entryPtr->policyStage == BROAD_POLICY_STAGE_INGRESS)
      {
        BCM_FIELD_QSET_ADD(resourceReq->qsetAgg, bcmFieldQualifyInPorts);
      }
      else if (entryPtr->policyStage == BROAD_POLICY_STAGE_EGRESS)
      {
        BCM_FIELD_QSET_ADD(resourceReq->qsetAgg, bcmFieldQualifyOutPort);
        /* PTin modified: SDK 6.3.0 */
        #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
        BCM_FIELD_QSET_ADD(resourceReq->qsetAgg, bcmFieldQualifyInterfaceClassPort);
        #else
        BCM_FIELD_QSET_ADD(resourceReq->qsetAgg, bcmFieldQualifyPortClass);
        #endif
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
  super_xset_entry_t         sxsetInfo;
  sqsetWidth_t               groupSqsetWidth;

  rv = _policy_minimal_sxset_get(unit, policyType, resourceReq, &sqset);
  if (BCM_E_NONE != rv)
    return rv;

  _policy_super_xset_get(unit, sqset, &sxsetInfo);

  rv = bcm_field_group_status_get(unit, gid, &stat);
  if (BCM_E_NONE != rv)
    return rv;

//sysapiPrintf("  GID=%d: Entries free/total %d/%d, counters free/total %d/%d, meters free/total %d/%d\n", gid,
//       stat.entries_free, stat.entries_total,
//       stat.counters_free, stat.counters_total,
//       stat.meters_free, stat.counters_total);

  _policy_group_status_to_sxset_width(&stat, &groupSqsetWidth);

  /* If this policy would require expansion of a group that is wider than the 
     policy needs, don't allow this to occur. The policy can be created w/ the
     minimal sqset later on. */
  if (sxsetInfo.sqsetWidth < groupSqsetWidth)
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

  /* TODO: SDK 6.3.0 */
  //return rv;
  return BCM_E_NONE;
}

static int _policy_group_find_group(int                             unit,
                                    BROAD_POLICY_ENTRY_t           *entryPtr,
                                    policy_resource_requirements_t *resourceReq,
                                    BROAD_GROUP_t                  *group)
{
  int          rv;
  sqsetWidth_t qsetWidth;
  int          groupEfpUsingIfp, policyEfpUsingIfp;

  debug_print_xset(&resourceReq->qsetAgg);

  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
    sysapiPrintf("_policy_group_find_group - stage=%d type=%d\n", entryPtr->policyStage, entryPtr->policyType);

  /* Find an existing group that can satisfy the policy requirements. */
  for (qsetWidth = sqsetWidthFirst; qsetWidth < sqsetWidthLast; qsetWidth++)
  {
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
      sysapiPrintf("_policy_group_find_group - qsetWidth reference %d\n", qsetWidth);

    rv = _policy_group_find_first(unit, entryPtr->policyStage, entryPtr->policyType, group);

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
      sysapiPrintf("_policy_group_find_group - First group is %d (rv=%d)\n", *group, rv);

    while (BCM_E_NONE == rv)
    {
      bcm_field_qset_t         qset;
      group_table_t           *groupPtr;

      groupPtr = &group_table[unit][entryPtr->policyStage][*group];

      if (groupPtr->flags & GROUP_USED)
      {
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
          sysapiPrintf("_policy_group_find_group - group %d in use: groupPtr->sqset=%u sqsetWidth=%u\n", *group, groupPtr->sqset, super_xset_table[unit][groupPtr->sqset].sqsetWidth);

        if (super_xset_table[unit][groupPtr->sqset].sqsetWidth == qsetWidth)
        {
          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
            sysapiPrintf("_policy_group_find_group - examining group %d\n", *group);

          /* Insure the group has a suitable qset and enough free entries, counters, et al. */
          BCM_FIELD_QSET_INIT(qset);
          _policy_qset_union(super_xset_table[unit][groupPtr->sqset].qsetAgg, &qset);

          rv = _policy_set_subset(resourceReq->qsetAgg, resourceReq->customQset, qset, super_xset_table[unit][groupPtr->sqset].customQset);

          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
            sysapiPrintf("_policy_group_find_group - rv = %d\n", rv);

          if (BCM_E_NONE == rv)
          {
            rv = _policy_group_resource_check(unit, entryPtr->policyType, resourceReq, groupPtr->gid);

            if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
              sysapiPrintf("_policy_group_find_group - rv = %d\n", rv);

            if (rv == BCM_E_NONE)
            {
              if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_MED)
                sysapiPrintf("_policy_group_find_group - Resources available\n");

              /* ensure that policies using IFP for EFP only go in groups using IFP for EFP */
              groupEfpUsingIfp  = (groupPtr->flags & GROUP_EFP_ON_IFP)                     ? L7_TRUE : L7_FALSE;
              policyEfpUsingIfp = (entryPtr->policyFlags & BROAD_POLICY_EGRESS_ON_INGRESS) ? L7_TRUE : L7_FALSE;

              if (groupEfpUsingIfp == policyEfpUsingIfp)
              {
                /* reuse existing group */
                if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
                  sysapiPrintf("_policy_group_find_group - reuse group %d\n", *group);

                return BCM_E_NONE;
              }
            }
          }
        }
      }

      rv = _policy_group_find_next(unit, entryPtr->policyStage, entryPtr->policyType, group);

      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("_policy_group_find_group - Next group is %d (rv=%d)\n", *group, rv);
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
  super_xset_entry_t     sxsetInfo;

  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
    sysapiPrintf("%s(%d) Going to allocate group...\n", __FUNCTION__, __LINE__);

  /* make sure that only one group can do EFP on IFP */
  if (entryPtr->policyFlags & BROAD_POLICY_EGRESS_ON_INGRESS)
  {
    for (i = 0; i < group_table_size[unit][entryPtr->policyStage]; i++)
    {
      if ((group_table[unit][entryPtr->policyStage][i].flags & (GROUP_USED | GROUP_EFP_ON_IFP)) == (GROUP_USED | GROUP_EFP_ON_IFP))
      {
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          sysapiPrintf("%s(%d) Failure\n", __FUNCTION__, __LINE__);
        return BCM_E_FAIL;
      }
    }
  }

  debug_print_xset(&resourceReq->qsetAgg);

  for (qsetWidth = sqsetWidthFirst; qsetWidth < sqsetWidthLast; qsetWidth++)
  {
    /* create all new groups based upon a super qset */
    rv = _policy_super_xset_find_match(unit, 
                                       entryPtr->policyType, 
                                       qsetWidth, 
                                       resourceReq->qsetAgg, 
                                       resourceReq->customQset, 
                                       &sqset);
    if (BCM_E_NONE != rv)
    {
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("%s(%d) Match not found for qsetWidth=%d: rv=%d\n", __FUNCTION__, __LINE__, qsetWidth, rv);
      continue;
    }
  
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
      sysapiPrintf("- using super qset %d\n", sqset);

    _policy_super_xset_get(unit, sqset, &sxsetInfo);

    /* Try to find a group priority that we can use to create this group. */
    rv = _policy_group_find_first(unit, entryPtr->policyStage, entryPtr->policyType, group);
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
      sysapiPrintf("First Group=%u - rv %d\n", *group, rv);

    while (BCM_E_NONE == rv)
    {
      /* Enforce physical boundary conditions. */
      if (*group % sxsetInfo.status.slice_width_physical == 0)
      {
        if (!(group_table[unit][entryPtr->policyStage][*group].flags & GROUP_USED))
        {
          rv = bcm_field_group_create_mode(unit, sxsetInfo.qsetAgg, *group, bcmFieldGroupModeAuto, &gid);

          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
            sysapiPrintf("bcm_field_group_create() (group=%u width=%u) returned %d\n", *group, sxsetInfo.status.slice_width_physical, rv);
    
          if (rv == BCM_E_NONE)
          {
            break;
          }
        }
      }
  
      rv = _policy_group_find_next(unit, entryPtr->policyStage, entryPtr->policyType, group);
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("Next Group=%u - rv %d\n", *group, rv);
    }
  
    if (rv == BCM_E_NONE)
    {
      rv = _policy_group_resource_check(unit, entryPtr->policyType, resourceReq, gid);
      if (rv == BCM_E_NONE)
      {
        for (i = *group; i < (*group + sxsetInfo.status.slice_width_physical); i++)
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
          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
              sysapiPrintf("Resetted count_rules to 0 (gid=%u)\n",groupPtr->gid);
          #endif
        }

        break;
      }
      else
      {
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
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
    super_xset_entry_t sxsetInfo;

    groupPtr         = &group_table[unit][policyStage][group];

    _policy_super_xset_get(unit, groupPtr->sqset, &sxsetInfo);

    for (i = group; i < (group + sxsetInfo.status.slice_width_physical); i++)
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
            /* PTin modified: SDK 6.3.0 */
            #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
            /* Packets comning from outside with onli the outer vlan (and afterwords the innervlan is added), looks to not be considered with this */
            //rv = bcm_field_qualify_VlanFormat(unit, eid, BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED | BCM_FIELD_VLAN_FORMAT_INNER_TAGGED, 0xff);
            #else
            rv = bcm_field_qualify_VlanFormat(unit, eid, BCM_FIELD_PKT_FMT_INNER_TAGGED, BCM_FIELD_PKT_FMT_INNER_TAGGED);
            #endif
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
    /* DNX: todo */
    #if 0
    case BROAD_FIELD_ICMP_MSG_TYPE:
      /* Use L4 src port for ICMP Msg Type. We can do this because
         XGS3 just treats the L4 src port field as the first two
         bytes following the IP header. */
        tempValue16 = *((uint8 *)value) << 8;
        tempMask16  = *((uint8 *)mask) << 8;
        rv = bcm_field_qualify_L4SrcPort(unit, eid, tempValue16, tempMask16);
        break;
    #endif
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
    case BROAD_FIELD_PORTCLASS:
        /* PTin modified: SDK 6.3.0 */
        #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
        rv = bcm_field_qualify_InterfaceClassPort(unit, eid, *((uint32*)value), *((uint32*)mask));
        #else
        rv = bcm_field_qualify_PortClass(unit, eid, *((uint32*)value), *((uint32*)mask));
        #endif
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
  bcm_field_action_t          bcm_action;
  action_map_entry_t         *action_map;
  BROAD_POLICY_ACTION_t       action;
  L7_uint32                   param0, param1, param2;

  switch (policyStage)
  {
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
      /* Get action value */
      bcm_action = action_map[action].action[i];

      /* handle case of redirect to invalid port, e.g. LAG with no members */
      if ((BROAD_ACTION_REDIRECT == action) || (BROAD_ACTION_MIRROR == action))
      {
        if (BCMX_LPORT_INVALID == actPtr->u.ifp_parms.modid)
        {
          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
            sysapiPrintf("%s(%d) Skipped action %u\n",__FUNCTION__,__LINE__,action);
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
        hapiBroadPolicyActionParmsGet(actPtr, policyStage, action, BROAD_POLICY_ACTION_CONFORMING, &param0, &param1, &param2);
      }

      if (PROFILE_ACTION_INVALID == bcm_action)
      {
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          sysapiPrintf("%s(%d) bcm_action=%d: rv = %d\n",__FUNCTION__,__LINE__, bcm_action, rv);
        return BCM_E_CONFIG;
      }
      else if (PROFILE_ACTION_NONE != bcm_action)
      {
        /* set remaining actions */
        rv = bcm_field_action_add(unit, eid, bcm_action, param0, param1);
        if (BCM_E_NONE != rv)
        {
          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
            sysapiPrintf("%s(%d) bcm_action=%d param0=%d param1=%d: rv = %d\n",__FUNCTION__,__LINE__, bcm_action, param0, param1, rv);

          return rv;
        }
      }

      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("%s(%d) Added bcm_action=%d (param0=%d param1=%d): rv = %d\n",__FUNCTION__,__LINE__, bcm_action, param0, param1, rv);

      /* DNX: todo */
      #if 0
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
          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
            sysapiPrintf("%s(%d) rv = %d\n",__FUNCTION__,__LINE__,rv);

          /* If error is BCM_E_NOT_FOUND, it means that the sqset for the group does not contain
             lookup status. This could happen only with a system policy.
             In this case, clear the error and return.
           */
          if((BCM_E_NOT_FOUND == rv) || (BCM_E_PARAM == rv))
          {
            rv = BCM_E_NONE;
          }
          /* Otherwise log the error */
          else if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          {
            sysapiPrintf("Add lookup status to redirect/mirror action FAILED with ret val = %d\n", rv);
          }
        }
      }
      #endif
    }
  }
  return rv;
}

/* Only use these routines for SDK version >= 5.6.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(5,6,0,0))

static int _policy_group_add_policer(int unit, BROAD_POLICY_STAGE_t stage, bcm_field_entry_t eid, bcm_field_group_t gid, BROAD_POLICY_RULE_ENTRY_t *rulePtr)
{
    int                   rv = BCM_E_NONE;
    BROAD_METER_ENTRY_t   *meterPtr;
    bcm_policer_t         policer_id;
    bcm_policer_config_t  policer_cfg;

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
        policer_id = rulePtr->src_policerId;

        rv = bcm_field_entry_policer_attach(unit, eid, 0, policer_id);

        if (BCM_E_NONE != rv)
        {
          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_NONE)
              sysapiPrintf("%s(%d) We have an error! policer_id=%d, rv=%d\r\n", __FUNCTION__, __LINE__, policer_id, rv);
          return rv;
        }

        rulePtr->policer.policer_id = policer_id;
    }
    else
    {
        /* Create policer */
        rv = bcm_policer_create(unit, &policer_cfg, &policer_id);
        if (BCM_E_NONE != rv)
        {
          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_NONE)
              sysapiPrintf("%s(%d) We have an error! rv=%d\r\n",__FUNCTION__,__LINE__,rv);
          return rv;
        }

        /* Attach policer to field entry */
        rv = bcm_field_entry_policer_attach(unit, eid, 0, policer_id);
        if (BCM_E_NONE != rv)
        {
          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_NONE)
              sysapiPrintf("%s(%d) We have an error! rv=%d",__FUNCTION__,__LINE__,rv);
          return rv;
        }

        rulePtr->policer.policer_id = policer_id;
        rulePtr->src_policerId = policer_id;
    }

    return rv;
}


static int _policy_group_add_stat(int unit, bcm_field_entry_t eid, bcm_field_group_t gid, BROAD_POLICY_RULE_ENTRY_t *rulePtr)
{
    int                    rv = BCM_E_NONE;
    uint64                 zero64;
    BROAD_COUNTER_ENTRY_t *counterPtr;
    bcm_field_stat_t       stat[2];
    int                    stat_id;

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_NONE)
        sysapiPrintf("%s(%d) gid=%u, eid=%u",__FUNCTION__,__LINE__,gid,eid);

    counterPtr = &rulePtr->counter.counterInfo;

    if (rulePtr->ruleFlags & BROAD_COUNTER_SHARED)
    {
      stat_id = rulePtr->src_counterId;

      rv = bcm_field_entry_stat_attach(unit, eid, stat_id);

      if (BCM_E_NONE != rv)
      {
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_NONE)
            sysapiPrintf("%s(%d) We have an error! rv=%d",__FUNCTION__,__LINE__,rv);
        return rv;
      }

      rulePtr->counter.counter_id = stat_id;
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
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_NONE)
            sysapiPrintf("%s(%d) We have an error! rv=%d",__FUNCTION__,__LINE__,rv);
        return rv;
      }

      rv = bcm_field_entry_stat_attach(unit, eid, stat_id);
      if (BCM_E_NONE != rv)
      {
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_NONE)
            sysapiPrintf("%s(%d) We have an error! rv=%d",__FUNCTION__,__LINE__,rv);
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
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_NONE)
          sysapiPrintf("%s(%d) We have an error! rv=%d",__FUNCTION__,__LINE__,rv);
      return rv;
    }

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_NONE)
        sysapiPrintf("%s(%d) Success! rv=%d",__FUNCTION__,__LINE__,rv);

    return rv;
}

#else

static int _policy_group_add_meter(int unit, BROAD_POLICY_STAGE_t stage, bcm_field_entry_t eid, BROAD_POLICY_RULE_ENTRY_t *rulePtr)
{
    int                  rv = BCM_E_NONE;
    BROAD_METER_ENTRY_t *meterPtr;
    uint32               counter_flags;
    uint64               zero64;

    meterPtr = &rulePtr->policer.policerInfo;   /* PTin modified: SDK 6.3.0 */

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

        rv = bcm_field_meter_set(unit, eid, BCM_FIELD_METER_PEAK,
                                 meterPtr->pir, meterPtr->pbs);
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

    rv = bcm_field_action_add(unit, eid, bcmFieldActionUpdateCounter, flags, 0);

    return rv;
}
#endif


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


static int _policy_group_alloc_init(int unit, BROAD_POLICY_STAGE_t policyStage, int groups)
{
    PT_LOG_INFO(LOG_CTX_STARTUP,"Stage=%u, groups=%u", policyStage, groups);

    switch (policyStage)
    {
    case BROAD_POLICY_STAGE_INGRESS:
      /* low priority group starts at 0 and goes for 1 or 2 */
      group_alloc_table[unit][policyStage][ALLOC_BLOCK_LOW].lowPrio     = 0;
      group_alloc_table[unit][policyStage][ALLOC_BLOCK_LOW].highPrio    = 0;

      group_alloc_table[unit][policyStage][ALLOC_BLOCK_MEDIUM].lowPrio  = 1;
      group_alloc_table[unit][policyStage][ALLOC_BLOCK_MEDIUM].highPrio = 2;

      group_alloc_table[unit][policyStage][ALLOC_BLOCK_HIGH].lowPrio    = 3;
      group_alloc_table[unit][policyStage][ALLOC_BLOCK_HIGH].highPrio   = 4;

      group_alloc_table[unit][policyStage][ALLOC_BLOCK_PTIN].lowPrio    = 5;
      group_alloc_table[unit][policyStage][ALLOC_BLOCK_PTIN].highPrio   = 5;

      group_alloc_table[unit][policyStage][ALLOC_BLOCK_STATS_CLIENT].lowPrio  = 6;
      group_alloc_table[unit][policyStage][ALLOC_BLOCK_STATS_CLIENT].highPrio = 6;

      group_alloc_table[unit][policyStage][ALLOC_BLOCK_STATS_EVC].lowPrio     = 7;
      group_alloc_table[unit][policyStage][ALLOC_BLOCK_STATS_EVC].highPrio    = 7;

      break;

    case BROAD_POLICY_STAGE_EGRESS:
      /* low priority group starts at 0 and goes for 4 */
      group_alloc_table[unit][policyStage][ALLOC_BLOCK_LOW].lowPrio     = 8;
      group_alloc_table[unit][policyStage][ALLOC_BLOCK_LOW].highPrio    = 8;

      group_alloc_table[unit][policyStage][ALLOC_BLOCK_MEDIUM].lowPrio  = 8;
      group_alloc_table[unit][policyStage][ALLOC_BLOCK_MEDIUM].highPrio = 8;

      group_alloc_table[unit][policyStage][ALLOC_BLOCK_HIGH].lowPrio    = 8;
      group_alloc_table[unit][policyStage][ALLOC_BLOCK_HIGH].highPrio   = 8;

      /* PTin added: policer */
      group_alloc_table[unit][policyStage][ALLOC_BLOCK_PTIN].lowPrio    = 9;
      group_alloc_table[unit][policyStage][ALLOC_BLOCK_PTIN].highPrio   = 9;

      /* PTin added: EVC stats: groups 3 [ 1 * 128/(4*2) = 16 services/ports counters ] */
      group_alloc_table[unit][policyStage][ALLOC_BLOCK_STATS_EVC].lowPrio     = 10;
      group_alloc_table[unit][policyStage][ALLOC_BLOCK_STATS_EVC].highPrio    = 10;

      /* PTin added: client stats: groups 0-2 [ 3 * 128/(4*2) = 48 clients ] */
      group_alloc_table[unit][policyStage][ALLOC_BLOCK_STATS_CLIENT].lowPrio  = 11;
      group_alloc_table[unit][policyStage][ALLOC_BLOCK_STATS_CLIENT].highPrio = 11;
      /* PTin end */

      break;
    default:
      break;
    }

    PT_LOG_INFO(LOG_CTX_STARTUP,"PolicyStage=%u => ALLOC_BLOCK_LOW   : Groups %u - %u", policyStage,
                group_alloc_table[unit][policyStage][ALLOC_BLOCK_LOW].lowPrio,
                group_alloc_table[unit][policyStage][ALLOC_BLOCK_LOW].highPrio);
    PT_LOG_INFO(LOG_CTX_STARTUP,"PolicyStage=%u => ALLOC_BLOCK_MEDIUM: Groups %u - %u", policyStage,
             group_alloc_table[unit][policyStage][ALLOC_BLOCK_MEDIUM].lowPrio,
             group_alloc_table[unit][policyStage][ALLOC_BLOCK_MEDIUM].highPrio);
    PT_LOG_INFO(LOG_CTX_STARTUP,"PolicyStage=%u => ALLOC_BLOCK_HIGH  : Groups %u - %u", policyStage,
             group_alloc_table[unit][policyStage][ALLOC_BLOCK_HIGH].lowPrio,
             group_alloc_table[unit][policyStage][ALLOC_BLOCK_HIGH].highPrio);
    PT_LOG_INFO(LOG_CTX_STARTUP,"PolicyStage=%u => ALLOC_BLOCK_PTIN  : Groups %u - %u", policyStage,
             group_alloc_table[unit][policyStage][ALLOC_BLOCK_PTIN].lowPrio,
             group_alloc_table[unit][policyStage][ALLOC_BLOCK_PTIN].highPrio);
    PT_LOG_INFO(LOG_CTX_STARTUP,"PolicyStage=%u => ALLOC_BLOCK_STATS_EVC   : Groups %u - %u", policyStage,
             group_alloc_table[unit][policyStage][ALLOC_BLOCK_STATS_EVC].lowPrio,
             group_alloc_table[unit][policyStage][ALLOC_BLOCK_STATS_EVC].highPrio);
    PT_LOG_INFO(LOG_CTX_STARTUP,"PolicyStage=%u => ALLOC_BLOCK_STATS_CLIENT: Groups %u - %u", policyStage,
             group_alloc_table[unit][policyStage][ALLOC_BLOCK_STATS_CLIENT].lowPrio,
             group_alloc_table[unit][policyStage][ALLOC_BLOCK_STATS_CLIENT].highPrio);

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
              L7_LOG_ERROR(0);
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
    int total_slices = 12;

    switch (policyStage)
    {
    case BROAD_POLICY_STAGE_INGRESS:
      total_slices = 8;

    case BROAD_POLICY_STAGE_EGRESS:
      total_slices = 4;
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
    supported = L7_FALSE;
    break;

  case BROAD_POLICY_STAGE_INGRESS:
    supported = L7_TRUE;
    break;

  case BROAD_POLICY_STAGE_EGRESS:
    supported = L7_TRUE;
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

    for (policyStage = BROAD_POLICY_STAGE_FIRST; policyStage < BROAD_POLICY_STAGE_COUNT; policyStage++)
    {
      if (policy_stage_supported(unit, policyStage) == L7_TRUE)
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
    }

    rv = _policy_super_xset_init(unit);
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

  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
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
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
      sysapiPrintf("- allocate new group\n");

    rv = _policy_group_alloc_group(unit, policyData, resourceReq, group);

    if (BCM_E_NONE != rv)
    {
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("- allocate new group FAILED (group=%d, rv=%d)\n", group, rv);

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

        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          sysapiPrintf("- allocate new group\n");

        rv = _policy_group_alloc_group(unit, policyData, resourceReq, group);

        if (BCM_E_NONE != rv)
        {
          BCM_FIELD_QSET_ADD(resourceReq->qsetAgg, reworkQualifier);
          BCM_FIELD_QSET_REMOVE(resourceReq->qsetAgg, bcmFieldQualifyEtherType);

          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
            sysapiPrintf("- allocate new group FAILED\n");
        }
      }
    }
  }

  if (rv == BCM_E_NONE)
  {
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
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
                          int                       *policer_id,      /* PTin added: SDK 6.3.0 */
                          int                       *counter_id)
{
    BROAD_POLICY_FIELD_t f;
    group_table_t       *groupPtr;
    bcm_field_entry_t    gid, eid;
    int                  rv = BCM_E_NONE;
    BROAD_ACTION_ENTRY_t *actionPtr;

    CHECK_GROUP(unit, policyStage, group);

    groupPtr = &group_table[unit][policyStage][group];

    if (!(groupPtr->flags & GROUP_USED))
        return BCM_E_NOT_FOUND;

    gid = groupPtr->gid;

    rv = bcm_field_entry_create(unit, groupPtr->gid, &eid);

    /* PTin added: FFP */
    #if 1
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("- bcm_field_entry_create rv = %d (entry=%d)\n", rv, eid);
    #endif

    if (BCM_E_NONE != rv)
    {
      /* PTin removed: FFP */
      #if 0
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
            sysapiPrintf("- bcm_field_entry_create rv = %d\n", rv);
      #endif
      return rv;
    }

    /* PTin added: FFP */
    #if 1
    groupPtr->count_rules++;
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("Incremented count_rules to %u (unit=%d, policyStage=%d, group=%d)\n",groupPtr->count_rules,unit,policyStage,group);
    #endif

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
             if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
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
      {
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          sysapiPrintf("%s(%d) rv = %d\n",__FUNCTION__,__LINE__,rv);
        return rv;
      }
  
      /* add meters or counters, if any, but not both */
      if (rulePtr->ruleFlags & BROAD_METER_SPECIFIED)
      {
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
            sysapiPrintf("- adding a meter\n");
  
        /* PTin modified: SDK 6.3.0 */
        #if (SDK_VERSION_IS >= SDK_VERSION(5,6,0,0))
        rv = _policy_group_add_policer(unit, policyStage, eid, gid, rulePtr);
        #else
        rv = _policy_group_add_meter(unit, policyStage, eid, rulePtr);
        #endif
        if (BCM_E_NONE != rv)
        {
          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
            sysapiPrintf("%s(%d) rv = %d\n",__FUNCTION__,__LINE__,rv);
          return rv;
        }

        /* Ptin added: SDK 6.3.0 */
        #if 1
        if (policer_id != L7_NULLPTR)   *policer_id = rulePtr->policer.policer_id;
        #endif
      }
      /* PTin added: global policer */
      else if (*policer_id > 0)
      {
        /* (Only) Attach policer to rule */
        rv = bcm_field_entry_policer_attach(unit, eid, 0, *policer_id);

        if (BCM_E_NONE != rv)
        {
          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_NONE)
            sysapiPrintf("%s(%d) Error attaching policer_id %d to eid %d: rv=%d\r\n", __FUNCTION__, __LINE__, *policer_id, eid, rv);
          return rv;
        }
        else
        {
          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_NONE)
            sysapiPrintf("%s(%d) Attached global policer_id %d to eid %d\r\n", __FUNCTION__, __LINE__, *policer_id, eid);
        }
      }
      /* PTin modified: Stats */
      #if (SDK_VERSION_IS < SDK_VERSION(5,6,0,0))
      else
      #endif
      if (rulePtr->ruleFlags & BROAD_COUNTER_SPECIFIED)
      {
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
            sysapiPrintf("- adding a counter\n");

        /* PTin modified: SDK 6.3.0 */
        #if (SDK_VERSION_IS >= SDK_VERSION(5,6,0,0))
        rv = _policy_group_add_stat(unit, eid, gid, rulePtr);
        #else
        rv = _policy_group_add_counter(unit, eid, rulePtr);
        #endif
        if (BCM_E_NONE != rv)
        {
          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
            sysapiPrintf("%s(%d) rv = %d\n",__FUNCTION__,__LINE__,rv);
          return rv;
        }

        /* Ptin added: SDK 6.3.0 */
        #if 1
        if (counter_id != L7_NULLPTR)   *counter_id = rulePtr->counter.counter_id;
        #endif
      }
    }

    /* Only install to HW if we expect any ports to match this rule. This is mostly because
       for LOOKUP and EGRESS policies, the portClass doesn't really provide a mechanism to
       never match rule (as a NULL pbmp would do for ingress) */
    if ((policyStage == BROAD_POLICY_STAGE_INGRESS) || (policyType == BROAD_POLICY_TYPE_VLAN) || (BCM_PBMP_NOT_NULL(pbm)))
    {
      rv = bcm_field_entry_install(unit, eid);

      /* PTin added: SDK 6.3.0 */
      #if 1
      if ( rv != BCM_E_NONE)
      {
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          sysapiPrintf("%s(%d) rv = %d\n",__FUNCTION__,__LINE__,rv);

        PT_LOG_ERR(LOG_CTX_HAPI, "Error commiting rule: unit=%d stage=%d gid=%d eid=%d (maxgroups=%d)",
                unit, policyStage, gid, eid, group_table_size[unit][policyStage]);

        /* Destroy rule */
        (void) policy_group_delete_rule(unit, policyStage, group,
                                        eid, rulePtr->meterSrcEntry,  /* PTin added: Policer/Counter */
                                        rulePtr->policer.policer_id, rulePtr->counter.counter_id);
      }
      #endif

      /* PTin added: FFP */
      #if 1
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          sysapiPrintf("- bcm_field_entry_install rv = %d (entry=%d)\n", rv, eid);
      #endif
    }

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
      sysapiPrintf("%s(%d) rv = %d\n",__FUNCTION__,__LINE__,rv);

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
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
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
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("- bcm_field_entry_install rv = %d (entry=%d)\n", rv, eid);

    if (BCM_E_NONE != rv)
        return rv;

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
      sysapiPrintf("- Applied new pbm: 0x%08x %08x %08x / 0x%08x %08x %08x (entry=%d)\n",
                   pbm.pbits[2], pbm.pbits[1], pbm.pbits[0], maskPbm.pbits[2], maskPbm.pbits[1], maskPbm.pbits[0], eid);

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
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
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
          if (policyStage == BROAD_POLICY_STAGE_EGRESS)
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
        if (policyStage == BROAD_POLICY_STAGE_EGRESS)
        {
          rv = bcm_field_qualify_OutPort(unit, eid, 0, 0);
          if (BCM_E_NONE != rv)
              return rv;
        }
        portClassBmp = 1 << portClass;

        /* PTin modified: SDK 6.3.0 */
        #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
        rv = bcm_field_qualify_InterfaceClassPort(unit, eid, portClassBmp, portClassBmp);
        #else
        rv = bcm_field_qualify_PortClass(unit, eid, portClassBmp, portClassBmp);
        #endif
        if (BCM_E_NONE != rv)
            return rv;
      }

      rv = bcm_field_entry_install(unit, eid);

      /* PTin added: FFP */
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
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

    if (policyStage == BROAD_POLICY_STAGE_EGRESS)
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
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
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
        (((policyStage == BROAD_POLICY_STAGE_EGRESS)) && BCM_PBMP_NOT_NULL(policyPtr->pbm)))
    {
      rv = bcm_field_entry_install(unit, eid);

      /* PTin added: FFP */
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
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
    case BROAD_POLICY_RULE_PRIORITY_HIGH:     prio = ((BCM_FIELD_ENTRY_PRIO_HIGHEST - BCM_FIELD_ENTRY_PRIO_LOWEST)/3)+BCM_FIELD_ENTRY_PRIO_LOWEST;
                                              break;
    case BROAD_POLICY_RULE_PRIORITY_HIGH2:    prio = ((BCM_FIELD_ENTRY_PRIO_HIGHEST - BCM_FIELD_ENTRY_PRIO_LOWEST)/2)+BCM_FIELD_ENTRY_PRIO_LOWEST;
                                              break;
    case BROAD_POLICY_RULE_PRIORITY_HIGHEST:  prio = BCM_FIELD_ENTRY_PRIO_HIGHEST;
                                              break;
    default:                                  prio = ((BCM_FIELD_ENTRY_PRIO_HIGHEST - BCM_FIELD_ENTRY_PRIO_LOWEST)/4)+BCM_FIELD_ENTRY_PRIO_LOWEST;
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
                             BROAD_ENTRY_t        srcEntry,     /* PTin added: Policer/Counter */
                             int                  policer_id,   /* PTin added: SDK 6.3.0 */
                             int                  counter_id)
{
    int               rv;
    group_table_t    *groupPtr;
    bcm_field_entry_t eid;
    bcm_field_entry_t src_eid;  /* Ptin added: Policer/Counter */

    CHECK_GROUP(unit,policyStage,group);

    groupPtr = &group_table[unit][policyStage][group];

    if (!(groupPtr->flags & GROUP_USED))
        return BCM_E_NOT_FOUND;

    eid = BROAD_ENTRY_TO_BCM_ENTRY(entry);
    src_eid = BROAD_ENTRY_TO_BCM_ENTRY(srcEntry);   /* PTin added: Policer/Counter */

    rv = bcm_field_entry_remove(unit, eid);

    /* PTin added: FFP */
    #if 1
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("- bcm_field_entry_remove rv = %d (entry=%d)\n", rv, eid);
    #endif

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
      if (eid == src_eid)   /* PTin added: Policer/Counter */
      {
        if (bcm_policer_destroy(unit, policer_id) != BCM_E_NONE)
        {
          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_NONE)
              sysapiPrintf("%s(%d) ERROR: Cannot destroy policer\r\n",__FUNCTION__,__LINE__);
        }
      }
    }
    /* Try to detach a global policer */
    else
    {
      (void) bcm_field_entry_policer_detach(unit, eid, 0);
    }

    if (counter_id>0)
    {
      rv = bcm_field_entry_stat_detach(unit, eid, counter_id);
      if (BCM_E_NONE != rv)
          return rv;
      if (eid == src_eid)   /* PTin added: Policer/Counter */
      {
        if (bcm_field_stat_destroy(unit, counter_id) != BCM_E_NONE)
        {
          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_NONE)
              sysapiPrintf("%s(%d) ERROR: Cannot destroy counter\r\n",__FUNCTION__,__LINE__);
        }
      }
    }
    #endif

    rv = bcm_field_entry_destroy(unit, eid);

    /* PTin added: FFP */
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("- bcm_field_entry_destroy rv = %d (entry=%d)\n", rv, eid);

    if (BCM_E_NONE != rv)
        return rv;

    /* PTin added: FFP */
    #if 1
    if (groupPtr->count_rules>0)  groupPtr->count_rules--;
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
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

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
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
    #endif

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

    /* PTin modified: SDK 6.3.0 */
    #if (SDK_VERSION_IS >= SDK_VERSION(5,6,0,0))
    int                stat_id;

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
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
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
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
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
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("- bcm_field_entry_create rv = %d (entry=%d)\n", rv, eid);
    #endif

    if (BCM_E_NONE != rv)
      return rv;

    /* PTin added: FFP */
    #if 1
    groupPtr->count_rules++;
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("Incremented count_rules to %u (unit=%d, policyStage=%d, group=%d)\n",groupPtr->count_rules,unit,BROAD_POLICY_STAGE_INGRESS,group);
    #endif

    *entry = BCM_ENTRY_TO_BROAD_ENTRY(eid);

    /* PTin modified: SDK 6.3.0 */
    #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
    rv = bcm_field_qualify_EtherType(unit, eid, 0x0800, 0xFFFF);
    if (BCM_E_NONE != rv)
      return rv;
    rv = bcm_field_qualify_IpType(unit, eid, bcmFieldIpTypeIpv4Any);
    if (BCM_E_NONE != rv)
      return rv;
    #else
    rv = bcm_field_qualify_PacketFormat(unit, eid, BCM_FIELD_PKT_FMT_IPV4,
                                        BCM_FIELD_PKT_FMT_IPV4);
    if (BCM_E_NONE != rv)
      return rv;
    #endif

    rv = bcm_field_action_add(unit, eid, bcmFieldActionEgressMask, 
                              SOC_PBMP_WORD_GET(epbm, 0), 0
                              );
    if (BCM_E_NONE != rv)
      return rv;

    rv = bcm_field_entry_install(unit, eid);

    /* PTin added: FFP */
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
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

void _policy_super_xset_get(int unit, int sqset, super_xset_entry_t *sqsetInfo)
{
  if ((unit < SOC_MAX_NUM_DEVICES) && (sqset < SUPER_XSET_TABLE_SIZE))
  {
    memcpy(sqsetInfo, &(super_xset_table[unit][sqset]), sizeof(*sqsetInfo));
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

int _policy_minimal_sxset_get(int unit, BROAD_POLICY_TYPE_t policyType, policy_resource_requirements_t *resourceReq, int *sqset)
{
  int          rv;
  sqsetWidth_t minQsetWidth;

  /* Find the minimal sqset for this policy. */
  for (minQsetWidth = sqsetWidthFirst; minQsetWidth < sqsetWidthLast; minQsetWidth++)
  {
    rv = _policy_super_xset_find_match(unit, 
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

void _policy_group_status_to_sxset_width(bcm_field_group_status_t *status, sqsetWidth_t *sqsetWidth)
{
  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
    sysapiPrintf("%s(%d) slice_width_physical=%u intraslice_mode_enable=%u\n", __FUNCTION__, __LINE__,
                 status->slice_width_physical, status->intraslice_mode_enable);

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
    *sqsetWidth = sqsetWidthSingle;
  }

  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
    sysapiPrintf("%s(%d) *sqsetWidth=%u\n", __FUNCTION__, __LINE__, *sqsetWidth);
}

/* Debug */
void debug_print_xset(bcm_field_qset_t *qset)
{
  /* Text names of the enumerated qualifier IDs. */
  char *qual_text[bcmFieldQualifyCount] = BCM_FIELD_QUALIFY_STRINGS;
  int   i;

  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
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

  for (policyStage = BROAD_POLICY_STAGE_FIRST; policyStage < BROAD_POLICY_STAGE_COUNT; policyStage++)
  {
    if (policy_stage_supported(unit, policyStage) == L7_TRUE)
    {
      switch (policyStage)
      {
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

    for (policyStage = BROAD_POLICY_STAGE_FIRST; policyStage < BROAD_POLICY_STAGE_COUNT; policyStage++)
    {
      if (policy_stage_supported(unit, policyStage) == L7_TRUE)
      {
        switch (policyStage)
        {
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
                       super_xset_table[unit][group_table[unit][policyStage][i].sqset].status.slice_width_physical,
                       super_xset_table[unit][group_table[unit][policyStage][i].sqset].status.intraslice_mode_enable ? "Intraslice" : "          ");
                sysapiPrintf(" %d", super_xset_table[unit][group_table[unit][policyStage][i].sqset].status.natural_depth);
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

void debug_super_xset_table(int unit, int entry)
{
  /* Text names of the enumerated qualifier IDs. */
  char *qual_text[bcmFieldQualifyCount] = BCM_FIELD_QUALIFY_STRINGS;
  int i, j;
  int min_entry, max_entry;

  if (entry == -1)
  {
    /* All entries */
    min_entry = 0;
    max_entry = SUPER_XSET_TABLE_SIZE;
  }
  else
  {
    if (entry < SUPER_XSET_TABLE_SIZE)
    {
      min_entry = entry;
      max_entry = entry + 1;
    }
    else
    {
      sysapiPrintf("Entry must be less than %d\n", SUPER_XSET_TABLE_SIZE);
      return;
    }
  }

  for (i = min_entry; i < max_entry; i++)
  {
    sysapiPrintf("SQset %d", i);

    if (super_xset_table[unit][i].flags & SUPER_XSET_USED)
    {
      sysapiPrintf("\n- qsetAgg: ");
      for (j = 0; j < bcmFieldQualifyCount; j++)
      {
        if (BCM_FIELD_QSET_TEST(super_xset_table[unit][i].qsetAgg, j))
        {
          sysapiPrintf("%s, ", qual_text[j]);
        }
      }
      sysapiPrintf("\n- applicablePolicyTypes: ");
      for (j = 0; j < BROAD_POLICY_TYPE_LAST; j++)
      {
        if (super_xset_table[unit][i].applicablePolicyTypes & (1 << j))
        {
          sysapiPrintf("%s ", hapiBroadPolicyTypeName(j));
        }
      }
      sysapiPrintf("\n- sqsetWidth: ");
      switch (super_xset_table[unit][i].sqsetWidth)
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
      sysapiPrintf("- sliceWidthPhysical: %d\n", super_xset_table[unit][i].status.slice_width_physical);
      sysapiPrintf("- ruleDepth: %d\n", super_xset_table[unit][i].status.natural_depth);
      sysapiPrintf("- intrasliceModeEnabled: %d\n", super_xset_table[unit][i].status.intraslice_mode_enable);
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

