/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_group_bcm.h
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

#ifndef BROAD_GROUP_BCM_H
#define BROAD_GROUP_BCM_H

#include "broad_policy_common.h"
#include "bcm/field.h"

#if L7_FEAT_EGRESS_ACL_ON_IFP
/* Number of bytes in mask */
#define ENTRY_DENY_INDICES   ((BROAD_MAX_RULES_PER_POLICY - 1) / (sizeof(L7_uchar8) * 8) + 1)

/* Interface storage */
typedef struct
{
  L7_uchar8 value[ENTRY_DENY_INDICES];
} ENTRY_DENY_MASK_t;

/* SETMASKBIT turns on bit index # k in denyMask of policy_map_table_t entry 'j'. */
#define ENTRY_DENY_SETMASKBIT(j, k)                                    \
            ((j->denyMask).value[((k)/(8*sizeof(L7_uchar8)))] \
                         |= 1 << ((k) % (8*sizeof(L7_uchar8))))   

/* CLRMASK turns off all bits in denyMask of policy_map_table_t entry 'j'. */
#define ENTRY_DENY_CLRMASK(j)                                    \
           memset((j->denyMask).value, 0, ENTRY_DENY_INDICES)

/* ISMASKBITSET returns 0 if the bit k is not set in denyMask of policy_map_table_t entry 'j' */
#define ENTRY_DENY_ISMASKBITSET(j, k)                               \
        ((j->denyMask).value[((k)/(8*sizeof(L7_uchar8)))]  \
                         & ( 1 << ((k) % (8*sizeof(L7_char8)))) )
#else
/* SETMASKBIT turns on bit index # k in denyMask of policy_map_table_t entry 'j'. */
#define ENTRY_DENY_SETMASKBIT(j, k)

/* CLRMASK turns off all bits in denyMask of policy_map_table_t entry 'j'. */
#define ENTRY_DENY_CLRMASK(j)

/* ISMASKBITSET returns 0 if the bit k is not set in denyMask of policy_map_table_t entry 'j' */
#define ENTRY_DENY_ISMASKBITSET(j, k) 0
#endif

typedef unsigned char BROAD_GROUP_t;
typedef int           BROAD_ENTRY_t;

typedef enum custom_field_qualify_e 
{
    customFieldQualifyFirst = bcmFieldQualifyCount,    /* Not useable        */
    customFieldQualifyUdf0 = customFieldQualifyFirst, /* User defined field */
    customFieldQualifyUdf1,                           /* User defined field */
    customFieldQualifyIscsiOpcode,                    /*                     */
    customFieldQualifyIscsiOpcodeTcpOptions,          /* Includes 12 bytes of TCP options (typical of Linux clients) */
    customFieldQualifyIcmpMsgType,                    /* ICMP Message Type */
    customFieldQualifyLast                            /* Must be last */
} custom_field_qualify_t;

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

typedef struct
{
  unsigned short       ruleCount;
  unsigned short       counterCount;
  unsigned short       meterCount;
  bcm_field_qset_t     qsetAgg;    /* aggregate qset supported by entry - std fields only */
  custom_field_qset_t  customQset; /* for custom UDF fields that don't map to bcm_field_qset_t */
  L7_BOOL              requiresEtype; /* qset requires ethertype and can't be replaced w/ IpType. */
} policy_resource_requirements_t;

/* This table contains information used to map a policy to multiple entries
 * within a group. It also contains a bitmap of the ports to which this
 * policy applies.
 */

#define GROUP_MAP_NONE                   0
#define GROUP_MAP_USED                   1
#define GROUP_MAP_EFP_ON_IFP             2 /* Used to indicate that the policy is configured for egress filtering
                                              even though it is applied on ingress. */
#define GROUP_MAP_REQUIRES_IMPLICIT_DENY 4 /* This policy is configured for EFP on IFP and requires an implicit deny rule. */

typedef struct
{
    BROAD_GROUP_t                  group;
    BROAD_ENTRY_t                 *entry;
    BROAD_ENTRY_t                 *srcEntry;  /* PTin added: Policer/Counter */
    /* PTin added: SDK 6.3.0 */
    #if 1
    int                           *policer_id;
    int                           *counter_id;
    int                            general_policer_id;
    #endif
    bcm_pbmp_t                     pbm;
    unsigned char                  flags;
    BROAD_POLICY_TYPE_t            policyType;
    BROAD_POLICY_STAGE_t           policyStage;
    unsigned char                  portClass; /* Used for LOOKUP and EGRESS port classes */
    unsigned short                 entryCount;
#if L7_FEAT_EGRESS_ACL_ON_IFP
    ENTRY_DENY_MASK_t              denyMask;  /* Each bit indicates whether the corresponding policy rule has a 
                                                 deny action. Only used for EFP on IFP. */
#endif
    policy_resource_requirements_t resourceReq;
}
policy_map_table_t;

/* PTin added: policer */
/**
 * Get hardware information about fp rules
 * 
 * @param unit : unit
 * @param policy_id 
 * @param rule_id 
 * @param group_id 
 * @param entry_id 
 * @param policer_id 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t l7_bcm_policy_hwInfo_get(int unit, BROAD_POLICY_t policy_id, L7_uint rule_id,
                                 BROAD_GROUP_t *group_id, BROAD_ENTRY_t *entry_id,
                                 int *policer_id, int *counter_id);   /* PTin added: SDK 6.3.0 */
/* PTin end */

int l7_bcm_policy_init();

int l7_bcm_policy_create(int unit, BROAD_POLICY_t policy, BROAD_POLICY_ENTRY_t *policyData, L7_BOOL shuffleAllowed);

int l7_bcm_policy_destroy(int unit, BROAD_POLICY_t policy);

int l7_bcm_policy_apply(int unit, BROAD_POLICY_t policy, bcm_port_t port);

int l7_bcm_policy_apply_all(int unit, BROAD_POLICY_t policy);

int l7_bcm_policy_remove(int unit, BROAD_POLICY_t policy, bcm_port_t port);

int l7_bcm_policy_remove_all(int unit, BROAD_POLICY_t policy);

int l7_bcm_policy_stats(int unit, BROAD_POLICY_t policy, L7_uint32 ruleId, L7_uchar8 *buffer, L7_uint32 *buffer_size);

int l7_bcm_policy_counter_clear(int unit, BROAD_POLICY_t policy);

void l7_bcm_policy_dataplane_cleanup(L7_uint32 unit, L7_uint32 policy, L7_uint32 rule, L7_uint32 cpu_modid, L7_uint32 cpu_modport);
int l7_bcm_policy_rule_status_set(int unit, BROAD_POLICY_t policy, BROAD_POLICY_RULE_t rule, BROAD_RULE_STATUS_t status);
int _policy_map_get_info(int unit, int policy, policy_map_table_t *policyPtr);

#endif /* BROAD_GROUP_BCM_H */
