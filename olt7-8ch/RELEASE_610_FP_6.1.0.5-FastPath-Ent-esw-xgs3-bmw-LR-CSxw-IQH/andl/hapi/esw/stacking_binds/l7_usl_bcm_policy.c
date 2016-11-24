
/* This file implements USL for system policies. */


#include "l7_usl_bcm_policy.h"
#include "broad_policy_bcm.h"
#include "broad_group_bcm.h"
#include "broad_cfp_bcm.h"
#include "broad_filter_bcm.h"
#include "ibde.h"

void usl_policy_init()
{
    custom_policy_init();  /* init custom bcmx layer */
}

int usl_policy_create(BROAD_POLICY_t policy, BROAD_POLICY_ENTRY_t *policyInfo)
{
    int tmprv, rv = BCM_E_NONE;
    int i;

#ifdef BCM_ROBO_SUPPORT
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
#else
    for (i = 0; i < bde->num_devices(BDE_ALL_DEVICES); i++)
#endif
    {
        if (!SOC_IS_XGS_FABRIC(i))
        {
            if (SOC_IS_XGS3_SWITCH(i))
            {
              tmprv = l7_bcm_policy_create(i, policy, policyInfo);
            }

            else if (SOC_IS_ROBO(i))
            {
              tmprv = l7_bcm_cfp_policy_create(i, policy, policyInfo);
            }

            else
            {
              tmprv = l7_bcm_policy_xgs2_create(i, policy, policyInfo);
            }

            if (tmprv < rv)
                rv = tmprv;
        }
    }

    return rv;
}

int usl_policy_destroy(BROAD_POLICY_t policy)
{
    int tmprv, rv = BCM_E_NONE;
    int i;

#ifdef BCM_ROBO_SUPPORT
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
#else
    for (i = 0; i < bde->num_devices(BDE_ALL_DEVICES); i++)
#endif
    {
        if (!SOC_IS_XGS_FABRIC(i))
        {
            if (SOC_IS_XGS3_SWITCH(i))
            {
               tmprv = l7_bcm_policy_destroy(i, policy);
            }
            else if (SOC_IS_ROBO(i))
            {
               tmprv = l7_bcm_cfp_policy_destroy(i, policy);
            }
            else
            {
               tmprv = l7_bcm_policy_xgs2_destroy(i, policy);
            }

            if (tmprv < rv)
                rv = tmprv;
        }
    }

    return rv;
}

int usl_policy_apply_all(BROAD_POLICY_t policy)
{
    int tmprv, rv = BCM_E_NONE;
    int i;

#ifdef BCM_ROBO_SUPPORT
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
#else
    for (i = 0; i < bde->num_devices(BDE_ALL_DEVICES); i++)
#endif
    {
        if (!SOC_IS_XGS_FABRIC(i))
        {
            if (SOC_IS_XGS3_SWITCH(i))
            {
               tmprv = l7_bcm_policy_apply_all(i, policy);
            }
            else if (SOC_IS_ROBO(i))
            {
               tmprv = l7_bcm_cfp_policy_apply_all(i, policy);
            }
            else
            {
               tmprv = l7_bcm_policy_xgs2_apply_all(i, policy);
            }

            if (tmprv < rv)
                rv = tmprv;
        }
    }

    return rv;
}

int usl_policy_remove_all(BROAD_POLICY_t policy)
{
    int tmprv, rv = BCM_E_NONE;
    int i;

#ifdef BCM_ROBO_SUPPORT
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
#else
    for (i = 0; i < bde->num_devices(BDE_ALL_DEVICES); i++)
#endif
    {
        if (!SOC_IS_XGS_FABRIC(i))
        {
            if (SOC_IS_XGS3_SWITCH(i))
            {
               tmprv = l7_bcm_policy_remove_all(i, policy);
            }
            else if (SOC_IS_ROBO(i))
            {
               tmprv = l7_bcm_cfp_policy_remove_all(i, policy);
            }

            else
            {
               tmprv = l7_bcm_policy_xgs2_remove_all(i, policy);
            }
            if (tmprv < rv)
                rv = tmprv;
        }
    }

    return rv;
}

