/*
 * $Id: gdpll.h,  Exp $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        gdpll_shared.h
 * Purpose:     
 */

#ifndef __BCM_INT_GDPLL_SHARED_H__
#define __BCM_INT_GDPLL_SHARED_H__

#if defined(INCLUDE_GDPLL)

#include <bcm/gdpll.h>

/* M7 TCM region references */

/* For any offset that gets added for versioning, etc */
#define DPLL_DEBUG_ENABLE_OFFSET        0   /* This is the M7 switch for debug push */
#define DPLL_PARAM_START_OFFSET         4
#define DPLL_PARAM_SIZE                 sizeof(dpll_param_t)
#define DPLL_CHAN_PARAM_ADDR(base, chan) \
    ((base + DPLL_PARAM_START_OFFSET) + (DPLL_PARAM_SIZE * chan))

/* Internal datastructure for dpll config */
typedef struct dpll_param_s {
    bcm_gdpll_chan_dpll_config_t    dpll_config;
    bcm_gdpll_chan_dpll_state_t     dpll_state;
    int debugMode;
}dpll_param_t;

#endif /* INCLUDE_GDPLL */
#endif /* __BCM_INT_GDPLL_SHARED_H__ */
