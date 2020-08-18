/** \file soc/dnx/intr/dnx_intr.h
 */

/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef SOC_DNX_DNX_INTR_H_INCLUDE
/*
 * { 
 */
#define SOC_DNX_DNX_INTR_H_INCLUDE

#if !defined(BCM_DNX_SUPPORT)
#error "This file is for use by DNX family only!"
#endif

/*
 * INCLUDE FILES:
 * {
 */
#include <sal/types.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/defs.h>
#include <soc/iproc.h>
/*
 * }
 */

/*
 * Structs and Enums:
 * {
 */

/*
 * }
 */
/*
 * MACROs:
 * {
 */
/*
 * }
 */

int soc_dnx_int_name_to_id(
    int unit,
    char *name);
int soc_dnx_interrupts_array_init(
    int unit);
int soc_dnx_interrupts_array_deinit(
    int unit);
int soc_dnx_interrupt_cb_init(
    int unit);
int soc_dnx_is_block_eci_intr_assert(
    int unit,
    int blk,
    soc_reg_above_64_val_t eci_interrupt);
int
soc_dnx_ser_init(int unit);


#endif /* SOC_DNX_DNX_INTR_H_INCLUDE */
