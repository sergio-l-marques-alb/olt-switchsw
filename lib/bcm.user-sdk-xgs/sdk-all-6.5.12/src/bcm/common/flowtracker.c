/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#include <shared/bslenum.h>
#include <shared/bsl.h>

#include <soc/defs.h>
#include <soc/drv.h>

#include <bcm/error.h>
#include <bcm/flowtracker.h>

/* Initialize a flowtracker collector information structure. */
void bcm_flowtracker_collector_info_t_init(bcm_flowtracker_collector_info_t *collector_info)
{
    sal_memset(collector_info, 0, sizeof(bcm_flowtracker_collector_info_t));
}


/* Initialize a flowtracker flow group information structure. */
void bcm_flowtracker_group_info_t_init(
    bcm_flowtracker_group_info_t *flow_group_info)
{
   sal_memset(flow_group_info, 0, sizeof(bcm_flowtracker_group_info_t));
}

/* Initialize flowtracker template transmit configuration. */
void bcm_flowtracker_template_transmit_config_t_init(
                              bcm_flowtracker_template_transmit_config_t *config)
{
    sal_memset(config, 0, sizeof(bcm_flowtracker_template_transmit_config_t));
}

/* Initialize flow key. */
void bcm_flowtracker_flow_key_t_init(bcm_flowtracker_flow_key_t *flow_key)
{
    sal_memset(flow_key, 0, sizeof(bcm_flowtracker_flow_key_t));
}

/* Initialize a flowtracker group action info structure. */
void bcm_flowtracker_group_action_info_t_init(
                                bcm_flowtracker_group_action_info_t *action_info)
{
    sal_memset(action_info, 0, sizeof(bcm_flowtracker_group_action_info_t));
}

/* Initialize a flowtracker elephant profile info structure. */
void bcm_flowtracker_elephant_profile_info_t_init(
                               bcm_flowtracker_elephant_profile_info_t *profile)
{
    sal_memset(profile, 0, sizeof(bcm_flowtracker_elephant_profile_info_t));
}
