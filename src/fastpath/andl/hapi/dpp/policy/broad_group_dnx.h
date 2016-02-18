/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_group_xgs3.h
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

#ifndef BROAD_GROUP_XGS3_H
#define BROAD_GROUP_XGS3_H

#include "broad_policy_common.h"
#include "broad_group_bcm.h"
#include "broad_group_sqset.h"
#include "bcm/field.h"

typedef enum 
{

  BROAD_POLICY_EPBM_CMD_ASSIGN = 0,
  BROAD_POLICY_EPBM_CMD_ADD ,
  BROAD_POLICY_EPBM_CMD_REMOVE
} BROAD_POLICY_EPBM_CMD_t;

#define BROAD_POLICY_MAP_INVALID (~0)
#define BROAD_GROUP_INVALID  (0xFF)
#define BROAD_ENTRY_INVALID  (~0)
#define BROAD_INVALID_PORT_CLASS (0xFF)


/* Super QSet Table */
/* Qsets may incorporate UDFs, which must be created in each unit, so the
 * Qsets must also be created per unit.
 */

#define SUPER_XSET_NONE          0
#define SUPER_XSET_USED          1

typedef struct
{
  uint32                   flags;      /* super qset flags */
  bcm_field_qset_t         qset1;      /* 1st composite qset */
  bcm_field_qset_t         qsetAgg;    /* aggregate qset supported by entry - std fields only */
  custom_field_qset_t      customQset; /* for custom UDF fields that don't map to bcm_field_qset_t */
  sqsetWidth_t             sqsetWidth; /* Indicates if sqset is single, double, or quadwide. */

  bcm_field_aset_t         aset;
  bcm_field_aset_t         asetAgg;
  sasetWidth_t             sasetWidth;

  uint32                   applicablePolicyTypes; /* Bitmap corresponding to applicable policy types. */
  bcm_field_group_status_t status;
}
super_xset_entry_t;


typedef enum
{
  ALLOC_BLOCK_LOW = 0,      /* blocks */                 
  ALLOC_BLOCK_MEDIUM,       /* used for VLAN policies */ 
  ALLOC_BLOCK_HIGH,         /* used for PORT policies */ 
  ALLOC_BLOCK_PTIN,         /* used for PTIN special polices */    /* PTin added: policer */
  ALLOC_BLOCK_STATS_EVC,    /* used for PTIN special polices */    /* PTin added: stats */
  ALLOC_BLOCK_STATS_CLIENT, /* used for PTIN special polices */    /* PTin added: stats */
  ALLOC_BLOCK_MAX
} group_alloc_block_t;

typedef enum
{
  ALLOC_HIGH_TO_LOW = 1,
  ALLOC_LOW_TO_HIGH
} group_alloc_dir_t;

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
    bcm_field_group_t   gid;
    BROAD_POLICY_TYPE_t type;
    int                 sqset;  /* super qset this group was created from */

    uint32              count_rules; /* PTin added: FFP */
}
group_table_t;

int policy_group_init(int unit);

int policy_group_create(int                             unit, 
                        BROAD_POLICY_ENTRY_t           *policyData, 
                        BROAD_GROUP_t                  *group,
                        policy_resource_requirements_t *resourceReq);

int policy_group_add_rule(int                        unit,
                          BROAD_POLICY_STAGE_t       policyStage,
                          BROAD_POLICY_TYPE_t        policyType,
                          BROAD_GROUP_t              group,
                          BROAD_POLICY_RULE_ENTRY_t *ruleInfo,
                          bcm_pbmp_t                 pbm,
                          int                        skip_actions,
                          BROAD_ENTRY_t             *entry,
                          int                       *policer_id,      /* PTin modified: SDK 6.3.0 */
                          int                       *counter_id);

int policy_group_delete_rule(int                  unit,
                             BROAD_POLICY_STAGE_t policyStage,
                             BROAD_GROUP_t        group,
                             BROAD_ENTRY_t        entry,
                             BROAD_ENTRY_t        srcEntry,           /* PTin added: Policer/Counter */
                             int                  policer_id,         /* PTin added: SDK 6.3.0 */
                             int                  counter_id);

int policy_group_rule_priority_set(int                          unit,
                                   BROAD_POLICY_STAGE_t         policyStage,
                                   BROAD_GROUP_t                group,
                                   BROAD_ENTRY_t                entry,
                                   BROAD_POLICY_RULE_PRIORITY_t priority);

int policy_group_set_pbm(int                  unit,
                         BROAD_POLICY_STAGE_t policyStage,
                         BROAD_GROUP_t        group,
                         BROAD_ENTRY_t        entry,
                         bcm_pbmp_t           pbm);

int policy_group_set_portclass(int                  unit,
                               BROAD_POLICY_STAGE_t policyStage,
                               BROAD_GROUP_t        group,
                               BROAD_ENTRY_t        entry,
                               bcm_pbmp_t           pbm,
                               unsigned char        portClass);

int policy_group_set_outervlan(int                  unit,
                               policy_map_table_t  *policyPtr,
                               L7_uint32            ruleIdx,
                               char                *value,
                               char                *mask);
int policy_port_class_add_remove(int                  unit,
                                 bcm_port_t           port,
                                 BROAD_POLICY_STAGE_t policyStage,
                                 unsigned char        portClass,
                                 L7_BOOL              add);

int policy_port_class_pbmp_update(int                  unit,
                                  bcm_pbmp_t           pbm,
                                  BROAD_POLICY_STAGE_t policyStage,
                                  unsigned char        oldPortClass,
                                  unsigned char        newPortClass);

int policy_group_destroy(int unit, BROAD_POLICY_STAGE_t policyStage, BROAD_GROUP_t group);

int policy_group_get_stats(int                  unit,
                           BROAD_POLICY_STAGE_t policyStage,
                           BROAD_GROUP_t        group,
                           BROAD_ENTRY_t        entry,
                           uint64              *val1,
                           uint64              *val2);

int policy_group_stats_clear(int                  unit,
                             BROAD_POLICY_STAGE_t policyStage,
                             BROAD_ENTRY_t        entry);

void policy_group_dataplane_cleanup(int                  unit,
                                    BROAD_ENTRY_t        entry,
                                    L7_uint32            cpu_modid,
                                    L7_uint32            cpu_modport);

L7_BOOL policy_stage_supported(int unit, BROAD_POLICY_STAGE_t policyStage);

int policy_group_set_epbm(int unit,
                          BROAD_GROUP_t group,
                          BROAD_ENTRY_t entry,
                          bcm_pbmp_t pbmp,
                          BROAD_POLICY_EPBM_CMD_t epbmCmd
                          );

int policy_group_create_default_rule(int unit,
                                     BROAD_GROUP_t group,
                                     bcm_pbmp_t epbm,
                                     BROAD_ENTRY_t *entry
                                     );

int _policy_group_total_slices(int unit, BROAD_POLICY_STAGE_t policyStage);
void _policy_group_alloc_type(BROAD_POLICY_TYPE_t type, group_alloc_block_t *block, group_alloc_dir_t *dir);

L7_BOOL _policy_group_types_compatible(int unit, BROAD_POLICY_TYPE_t group1_type, BROAD_POLICY_TYPE_t group2_type);
int _policy_group_block_low_prio_get(int unit, BROAD_POLICY_STAGE_t  policyStage, int block);
int _policy_group_block_high_prio_get(int unit, BROAD_POLICY_STAGE_t  policyStage, int block);
void _policy_qset_union(bcm_field_qset_t q1, bcm_field_qset_t *q2);
void _policy_aset_union(bcm_field_aset_t a1, bcm_field_aset_t *a2);
int _policy_set_subset(bcm_field_qset_t q1, custom_field_qset_t custom_q1, 
                       bcm_field_qset_t q2, custom_field_qset_t custom_q2);

int _policy_super_xset_find_match(int                  unit, 
                                  BROAD_POLICY_TYPE_t  type, 
                                  sqsetWidth_t         qsetWidth,
                                  bcm_field_qset_t     qset, 
                                  custom_field_qset_t  customQset, 
                                  int                 *idx);
void _policy_super_xset_get(int unit, int sqset, super_xset_entry_t *sqsetInfo);
void _policy_group_info_get(int unit, BROAD_POLICY_STAGE_t policyStage, BROAD_GROUP_t group, group_table_t *groupInfo);
int _policy_minimal_sxset_get(int unit, BROAD_POLICY_TYPE_t policyType, policy_resource_requirements_t *resourceReq, int *sqset);
void _policy_group_status_to_sxset_width(bcm_field_group_status_t *status, sqsetWidth_t *sqsetWidth);
int _policy_group_calc_xset(int                             unit,
                            BROAD_POLICY_ENTRY_t           *entryPtr, 
                            policy_resource_requirements_t *resourceReq);

void debug_print_xset(bcm_field_qset_t *qset);

/* PTin added: FFP */
#if 1
group_table_t *policy_group_count_rules(int unit, int group_idx, int stage);
#endif

#endif /* BROAD_GROUP_XGS3_H */
