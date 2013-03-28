/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2008
*
**********************************************************************
*
* @filename  broad_cos_util.h
*
* @purpose   This file contains prototypes for utility functions to manage COS policies.
*
* @component hapi-broad
*
* @comments
*
* @create    12/04/08
*
* @author    colinw
*
* @end
*
**********************************************************************/
#include "datatypes.h"
#include "broad_policy_types.h"

#include "bcmx/types.h"

L7_RC_t hapiBroadCosPolicyUtilInit();

L7_BOOL hapiBroadCosPolicyUtilLookup(L7_uchar8 *data, L7_uint32 size, BROAD_POLICY_t *policy);

L7_RC_t hapiBroadCosPolicyUtilAdd(L7_uchar8 *data, L7_uint32 size, BROAD_POLICY_t policy);

L7_RC_t hapiBroadCosPolicyUtilApply(BROAD_POLICY_t policy, bcmx_lport_t lport);

L7_RC_t hapiBroadCosPolicyUtilRemove(BROAD_POLICY_t policy, bcmx_lport_t lport);

L7_RC_t hapiBroadCosPolicyUtilDelete(BROAD_POLICY_t policy);
