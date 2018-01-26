/*
 * $Id: $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef _DNX_SW_STATE_UTILS_H
#define _DNX_SW_STATE_UTILS_H

#include <bcm/types.h>
#include <soc/types.h>

/**********************************************/
/***********      Functions     ***************/
/**********************************************/

uint8 sw_state_is_flag_on(
    uint32 flags,
    uint32 flag);

uint8 dnx_sw_state_alloc_during_test_set(
    int unit,
    uint8 flag);

uint8 dnx_sw_state_alloc_during_test_get(
    int unit);

#if defined(DNX_SW_STATE_DIAGNOSTIC)

uint8 is_prefix(
    const char *pre,
    const char *str);

#endif /* DNX_SW_STATE_DIAGNOSTIC */
#endif /* _DNX_SW_STATE_UTILS_H */
