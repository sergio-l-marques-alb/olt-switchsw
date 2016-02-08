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

#ifndef BROAD_CFP_ROBO_H
#define BROAD_CFP_ROBO_H

#include "broad_policy_common.h"

typedef int BROAD_GROUP_t;
typedef int BROAD_ENTRY_t;

#define BROAD_POLICY_MAP_INVALID (~0)
#define BROAD_GROUP_INVALID  (~0)
#define BROAD_ENTRY_INVALID  (~0)

int policy_cfp_group_init(int unit);

int policy_cfp_group_create(int                   unit,
                        BROAD_POLICY_STAGE_t  policyStage,
                        BROAD_POLICY_ENTRY_t *policyData,
                        BROAD_GROUP_t        *group);

int policy_cfp_group_add_rule(int                        unit,
                          BROAD_POLICY_STAGE_t       policyStage,
                          BROAD_GROUP_t              group,
                          BROAD_POLICY_RULE_ENTRY_t *ruleInfo,
                          bcm_pbmp_t                 pbm,
                          BROAD_ENTRY_t             *entry);

int policy_cfp_group_delete_rule(int                  unit,
                             BROAD_POLICY_STAGE_t policyStage,
                             BROAD_GROUP_t        group,
                             BROAD_ENTRY_t        entry);

int policy_cfp_group_rule_priority_set(int                          unit,
                                   BROAD_POLICY_STAGE_t         policyStage,
                                   BROAD_GROUP_t                group,
                                   BROAD_ENTRY_t                entry,
                                   BROAD_POLICY_RULE_PRIORITY_t priority);

int policy_cfp_group_set_pbm(int                  unit,
                         BROAD_POLICY_STAGE_t policyStage,
                         BROAD_GROUP_t        group,
                         BROAD_ENTRY_t        entry,
                         bcm_pbmp_t           pbm,
                         bcm_pbmp_t           pbmMask);

int policy_cfp_group_set_portclass(int                  unit,
                               BROAD_POLICY_STAGE_t policyStage,
                               BROAD_GROUP_t        group,
                               BROAD_ENTRY_t        entry,
                               bcm_pbmp_t           pbm,
                               unsigned char        portClass);

int policy_cfp_port_class_set(int                  unit,
                          bcm_port_t           port,
                          BROAD_POLICY_STAGE_t policyStage,
                          unsigned char        portClass);

int policy_cfp_group_destroy(int unit, BROAD_POLICY_STAGE_t policyStage, BROAD_GROUP_t group);

int policy_cfp_group_get_stats(int                  unit,
                           BROAD_POLICY_STAGE_t policyStage,
                           BROAD_GROUP_t        group,
                           BROAD_ENTRY_t        entry,
                           uint64              *val1,
                           uint64              *val2);

L7_BOOL policy_cfp_stage_supported(int unit, BROAD_POLICY_STAGE_t policyStage);

#endif /* BROAD_GROUP_XGS3_H */
