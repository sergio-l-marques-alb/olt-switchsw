/*
 * $Id: appl_dcmn_intr.h, v1 Broadcom SDK $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef _APPL_REF_INTR_H_
#define _APPL_REF_INTR_H_

/*************
 * INCLUDES  *
 *************/
#include <soc/defs.h>
#include <soc/types.h>
#include <bcm/error.h>

/*************
 * TYPE DEFS *
 *************/

/*************
 * STRUCTURES *
 *************/

/*
 * static data strucure which holds the info about:
 *             the function suppose to run for each interrupt
 *              info about storm threshold
 */
typedef struct interrupt_common_params_s {
    uint32* int_disable_on_init;
    uint32* int_active_on_init;
    uint32* int_disable_print_on_init;
    soc_mem_t* cached_mem;
} intr_common_params_t;

/*************
 * FUNCTIONS *
 *************/
int appl_dnxc_interrupt_init(int unit);
int appl_dnxc_interrupt_deinit(int unit);
int interrupt_appl_nocache_deinit(int unit);


#ifdef BCM_DNXF_SUPPORT
int ramon_interrupt_cmn_param_init(int unit, intr_common_params_t* interrupt_common_params);
#endif

uint32 *appl_dnxc_interrupt_active_on_intr_get(int unit);

#endif /*_DCMN_INTR_H_ */
