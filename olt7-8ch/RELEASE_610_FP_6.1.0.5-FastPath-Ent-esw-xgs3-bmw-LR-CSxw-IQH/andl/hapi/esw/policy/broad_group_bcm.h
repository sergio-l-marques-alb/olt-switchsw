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

int l7_bcm_policy_init();

int l7_bcm_policy_create(int unit, BROAD_POLICY_t policy, BROAD_POLICY_ENTRY_t *policyData);

int l7_bcm_policy_destroy(int unit, BROAD_POLICY_t policy);

int l7_bcm_policy_check(int unit, BROAD_POLICY_t policy, bcm_port_t port);

int l7_bcm_policy_apply(int unit, BROAD_POLICY_t policy, bcm_port_t port);

int l7_bcm_policy_apply_all(int unit, BROAD_POLICY_t policy);

int l7_bcm_policy_remove(int unit, BROAD_POLICY_t policy, bcm_port_t port);

int l7_bcm_policy_remove_all(int unit, BROAD_POLICY_t policy);

int l7_bcm_policy_stats(int unit, BROAD_POLICY_t policy, uint32 *args);

/* Reserve port class 0 to be used as 'invalid'. That is, any rule that cares
   about which ports the rule should be applied to will use a class ID of 1 to
   BROAD_MAX_PORT_CLASS_ID. All ports that are not to be included as part of the rule
   will have their port class ID set to 0. */
#define BROAD_INVALID_PORT_CLASS    0
#define BROAD_MAX_PORT_CLASS_ID     16

#endif /* BROAD_GROUP_BCM_H */
