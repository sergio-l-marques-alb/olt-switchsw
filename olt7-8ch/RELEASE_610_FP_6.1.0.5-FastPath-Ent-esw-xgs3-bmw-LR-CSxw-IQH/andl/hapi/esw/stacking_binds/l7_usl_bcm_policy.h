
#ifndef BROAD_POLICY_USL_H
#define BROAD_POLICY_USL_H

#include "broad_policy_common.h"

void usl_policy_init();

int usl_policy_create(BROAD_POLICY_t policy, BROAD_POLICY_ENTRY_t *policyInfo);

int usl_policy_destroy(BROAD_POLICY_t policy);

int usl_policy_apply_all(BROAD_POLICY_t policy);

int usl_policy_remove_all(BROAD_POLICY_t policy);

#endif /* BROAD_POLICY_USL_H */
