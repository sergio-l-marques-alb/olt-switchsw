/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_policy_util.h
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

#ifndef BROAD_POLICY_UTIL_H
#define BROAD_POLICY_UTIL_H

/* This file provides utilities for reusing policies by applying them to
 * multiple interfaces.
 */
                           
#include "broad_policy_common.h"

L7_RC_t hapiBroadPolicyUtilInit();

L7_BOOL hapiBroadPolicyUtilLookup(L7_uchar8 *data, L7_uint32 size, BROAD_POLICY_t *policy);

L7_RC_t hapiBroadPolicyUtilAdd(L7_uchar8 *data, L7_uint32 size, BROAD_POLICY_t policy);

L7_RC_t hapiBroadPolicyUtilDelete(BROAD_POLICY_t policy);

L7_RC_t hapiBroadPolicyUtilApply(BROAD_POLICY_t policy, bcmx_lport_t lport);

L7_RC_t hapiBroadPolicyUtilRemove(BROAD_POLICY_t policy, bcmx_lport_t lport);

#endif /* BROAD_POLICY_UTIL_H */
