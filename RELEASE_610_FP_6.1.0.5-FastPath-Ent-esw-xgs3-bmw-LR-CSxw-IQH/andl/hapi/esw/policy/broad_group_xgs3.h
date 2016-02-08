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

typedef int BROAD_GROUP_t;
typedef int BROAD_ENTRY_t;

typedef enum 
{
  BROAD_POLICY_EPBM_CMD_ASSIGN = 0,
  BROAD_POLICY_EPBM_CMD_ADD ,
  BROAD_POLICY_EPBM_CMD_REMOVE
} BROAD_POLICY_EPBM_CMD_t;

#define BROAD_POLICY_MAP_INVALID (~0)
#define BROAD_GROUP_INVALID  (~0)
#define BROAD_ENTRY_INVALID  (~0)

// PTin added: to get available rules
int policy_group_available_rules(int unit, int group_id);

int policy_group_init(int unit);

int policy_group_create(int                   unit,
                        BROAD_POLICY_STAGE_t  policyStage,
                        BROAD_POLICY_ENTRY_t *policyData,
                        BROAD_GROUP_t        *group);

int policy_group_add_rule(int                        unit,
                          BROAD_POLICY_STAGE_t       policyStage,
                          BROAD_POLICY_TYPE_t        policyType,
                          BROAD_GROUP_t              group,
                          BROAD_POLICY_RULE_ENTRY_t *ruleInfo,
                          bcm_pbmp_t                 pbm,
                          int                        skip_actions,
                          BROAD_ENTRY_t             *entry);

int policy_group_delete_rule(int                  unit,
                             BROAD_POLICY_STAGE_t policyStage,
                             BROAD_GROUP_t        group,
                             BROAD_ENTRY_t        entry);

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

int policy_port_class_set(int                  unit,
                          bcm_port_t           port,
                          BROAD_POLICY_STAGE_t policyStage,
                          unsigned char        portClass);

int policy_group_destroy(int unit, BROAD_POLICY_STAGE_t policyStage, BROAD_GROUP_t group);

int policy_group_get_stats(int                  unit,
                           BROAD_POLICY_STAGE_t policyStage,
                           BROAD_GROUP_t        group,
                           BROAD_ENTRY_t        entry,
                           uint64              *val1,
                           uint64              *val2);

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
#endif /* BROAD_GROUP_XGS3_H */
