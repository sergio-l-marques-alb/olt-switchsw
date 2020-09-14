/** \file dnxc_verify.c
 *
 * Skip verify support.
 *
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <soc/dnxc/dnxc_verify.h>
#include <soc/drv.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/drv_dnxc_utils.h>

/*
 * Variable to store init_verify flag.
 */
int dnxc_verify_allowed[SOC_MAX_NUM_DEVICES] = { 0 };
/*
 * This flag will overwrite the init_verify soc property
 */
int dnxc_verify_allowed_fast_init_flag = FALSE;
int dnxc_verify_allowed_verify_init_flag = FALSE;

void
dnxc_verify_allowed_init(
    int unit)
{
    int value = 1;

    if (dnxc_verify_allowed_fast_init_flag == TRUE)
    {
        dnxc_verify_allowed_set(unit, FALSE);
        return;
    }

    if (dnxc_verify_allowed_verify_init_flag == TRUE)
    {
        dnxc_verify_allowed_set(unit, TRUE);
        return;
    }


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

void
dnxc_verify_fast_init_enable(
    int enable)
{
    if (enable)
    {
        dnxc_verify_allowed_fast_init_flag = TRUE;
    }
    else
    {
        dnxc_verify_allowed_verify_init_flag = TRUE;
    }
}
