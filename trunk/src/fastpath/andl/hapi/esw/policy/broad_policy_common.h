/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_policy_common.h
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

#ifndef BROAD_POLICY_COMMON_H
#define BROAD_POLICY_COMMON_H

#include "broad_common.h"

int hapiBroadPolicyFieldSize(BROAD_POLICY_FIELD_t field);

char *hapiBroadPolicyFieldName(BROAD_POLICY_FIELD_t field);

void hapiBroadPolicyFieldFlagsSet(BROAD_FIELD_ENTRY_t *fieldInfo, BROAD_POLICY_FIELD_t field, L7_uchar8 value);
L7_uchar8 hapiBroadPolicyFieldFlagsGet(BROAD_FIELD_ENTRY_t *fieldInfo, BROAD_POLICY_FIELD_t field);
L7_uchar8 *hapiBroadPolicyFieldValuePtr(BROAD_FIELD_ENTRY_t *fieldInfo, BROAD_POLICY_FIELD_t field);
L7_uchar8 *hapiBroadPolicyFieldMaskPtr(BROAD_FIELD_ENTRY_t *fieldInfo, BROAD_POLICY_FIELD_t field);
void hapiBroadPolicyActionParmsGet(BROAD_ACTION_ENTRY_t       *actionPtr, 
                                   BROAD_POLICY_STAGE_t        policyStage,
                                   BROAD_POLICY_ACTION_t       action, 
                                   BROAD_POLICY_ACTION_SCOPE_t action_scope,
                                   L7_uint32                  *param0, 
                                   L7_uint32                  *param1, 
                                   L7_uint32                  *param2);

void hapiBroadPolicyRulesPurge(BROAD_POLICY_ENTRY_t *policyInfo);
int hapiBroadPolicyCopy(BROAD_POLICY_ENTRY_t *srcPolicyInfo, BROAD_POLICY_ENTRY_t *dstPolicyInfo);

char *hapiBroadPolicyActionName(BROAD_POLICY_ACTION_t action);

char *hapiBroadPolicyTypeName(BROAD_POLICY_TYPE_t type);

typedef enum
{
    POLICY_DEBUG_NONE,
    POLICY_DEBUG_LOW,
    POLICY_DEBUG_MED,
    POLICY_DEBUG_HIGH,
}
BROAD_POLICY_DEBUG_LEVEL_t;

BROAD_POLICY_DEBUG_LEVEL_t hapiBroadPolicyDebugLevel();
void hapiBroadDebugPolicyEnable(BROAD_POLICY_DEBUG_LEVEL_t val);  /* PTin added: for debug purposes */

#endif /* BROAD_POLICY_COMMON_H */
