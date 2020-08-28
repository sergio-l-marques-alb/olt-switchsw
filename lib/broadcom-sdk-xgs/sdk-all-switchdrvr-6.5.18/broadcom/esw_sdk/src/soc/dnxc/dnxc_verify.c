/** \file dnxc_verify.c
 *
 * Skip verify support.
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <soc/dnxc/dnxc_verify.h>
#include <soc/drv.h> /**soc_property_port_get_str*/
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/drv_dnxc_utils.h>

/*
 * Variable to store init_verify flag.
 */
int dnxc_verify_allowed[SOC_MAX_NUM_DEVICES] = { 0 };

void
dnxc_verify_allowed_init(
    int unit)
{
    int value = 1;

    /*
     * Set dnxc_verify_allowed value.
     */
    dnxc_verify_allowed_set(unit, value);

    return;
}

void
dnxc_verify_allowed_set(
    int unit,
    int value)
{
    dnxc_verify_allowed[unit] = value;
    return;
}

int
dnxc_verify_allowed_get(
    int unit)
{
    return dnxc_verify_allowed[unit];
}
