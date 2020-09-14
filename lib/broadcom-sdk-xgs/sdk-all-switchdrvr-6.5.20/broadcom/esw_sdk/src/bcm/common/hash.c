/*
 * $Id: hash.c,v 1.1 Broadcom SDK $
 *
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 * Hash structure initializers
 */


#include <sal/core/libc.h>
#include <bcm/hash.h>

/*
 * Function:
 *      bcm_hash_flex_bin_config_t_init
 * Purpose:
 *      Initialize hash flex bin config struct
 * Parameters:
 *      cfg - Pointer to the struct to be init'ed
 */
void
bcm_hash_flex_bin_config_t_init(bcm_hash_flex_bin_config_t *cfg)
{
    if (cfg != NULL) {
        sal_memset(cfg, 0, sizeof(*cfg));
    }
    return;
}
