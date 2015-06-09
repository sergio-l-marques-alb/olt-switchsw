/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2010
*
**********************************************************************
*
* @filename  broad_group_shuffle.h
*
* @component hapi
*
* @create    1/27/2010
*
* @author    colinw
*
* @end
*
**********************************************************************/
#ifndef BROAD_GROUP_SHUFFLE_H
#define BROAD_GROUP_SHUFFLE_H

#include "datatypes.h"
#include "broad_policy_types.h"
#include "broad_group_xgs3.h"

int policy_group_shuffle(int unit, BROAD_POLICY_t newPolicyId, BROAD_POLICY_ENTRY_t *policyData);

#endif /* BROAD_GROUP_SHUFFLE_H */
