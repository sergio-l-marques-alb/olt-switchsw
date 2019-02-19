/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_filter_bcm.h
*
* @component hapi
*
* @create    4/18/2006
*
* @author    sdoke
*
* @end
*
**********************************************************************/

#ifndef BROAD_FILTER_BCM_H
#define BROAD_FILTER_BCM_H

#include "broad_policy_common.h"

int l7_bcm_policy_xgs2_init();

int l7_bcm_policy_xgs2_create(int unit, BROAD_POLICY_t policy, BROAD_POLICY_ENTRY_t *policyData);

int l7_bcm_policy_xgs2_destroy(int unit, BROAD_POLICY_t policy);

int l7_bcm_policy_xgs2_apply(int unit, BROAD_POLICY_t policy, bcm_port_t port);

int l7_bcm_policy_xgs2_apply_all(int unit, BROAD_POLICY_t policy);

int l7_bcm_policy_xgs2_remove(int unit, BROAD_POLICY_t policy, bcm_port_t port);

int l7_bcm_policy_xgs2_remove_all(int unit, BROAD_POLICY_t policy);

int l7_bcm_policy_xgs2_stats(int unit, BROAD_POLICY_t policy, uint32 *args);

int  l7_bcm_mirror_enable(int modid,int mod_port);

void l7_bcm_mirror_disable();

#endif /* BROAD_FILTER_BCM_H */

