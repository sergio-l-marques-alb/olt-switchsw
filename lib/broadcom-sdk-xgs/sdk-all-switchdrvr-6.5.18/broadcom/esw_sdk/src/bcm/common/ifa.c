/*
 * $Id: $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * IFA - In-band Flow Analyzer Monitoring Embedded Application APP interface
 * Purpose: API to Initialize IFA embedded app Structures.
 *
 */
#include <shared/bslenum.h>
#include <shared/bsl.h>

#include <soc/defs.h>
#include <soc/drv.h>

#include <bcm/error.h>
#include <bcm/ifa.h>

/* Initialize a ifa collector information structure. */
void bcm_ifa_collector_info_t_init(bcm_ifa_collector_info_t *collector_info)
{
    sal_memset(collector_info, 0, sizeof(bcm_ifa_collector_info_t));
}

/* Initialize a ifa config information structure. */
void bcm_ifa_config_info_t_init(bcm_ifa_config_info_t *config_info)
{
    sal_memset(config_info, 0, sizeof(bcm_ifa_config_info_t));
}

/* Initialize a ifa leap config information structure. */
void bcm_ifa_leap_config_t_init(bcm_ifa_leap_config_t *config_info)
{
    sal_memset(config_info, 0, sizeof(bcm_ifa_leap_config_t));
}
