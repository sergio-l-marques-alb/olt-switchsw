/** \file sw_state_utils.c
 *
 * sw state functions definitions
 *
 */
/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/* 
 * Note! 
 * This include statement must be at the top of every sw state .c file 
 * It points to a set of in-code compilation flags that must be taken into 
 * account for every sw state componnent compilation 
 */
#include <soc/dnxc/swstate/sw_state_features.h>
/* ---------- */


#include <soc/dnxc/swstate/dnx_sw_state_utils.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <shared/bsl.h>

uint8 dnx_sw_state_alloc_during_test[SOC_MAX_NUM_DEVICES];

uint8
sw_state_is_flag_on(
    uint32 flags,
    uint32 flag)
{
    return ((flags & flag) ? 1 : 0);
}

uint8
dnx_sw_state_alloc_during_test_set(
    int unit,
    uint8 flag)
{
    dnx_sw_state_alloc_during_test[unit] = flag;
    return 0;
}

uint8
dnx_sw_state_alloc_during_test_get(
    int unit)
{
    return dnx_sw_state_alloc_during_test[unit];
}


uint8 is_prefix(const char *pre, const char *str)
{
    uint32 pre_len = sal_strlen(pre);
    uint32 str_len = sal_strlen(str);
    if (str_len < pre_len) {
        return 0;
    }
    return (sal_strncmp(pre, str, pre_len) == 0);
}

