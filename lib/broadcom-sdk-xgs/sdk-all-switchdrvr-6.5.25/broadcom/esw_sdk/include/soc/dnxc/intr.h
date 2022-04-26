/** \file soc/dnxc/intr.h
 * Slim SoC module to allow bcm actions.
 *
 * This file contains structure and routine declarations for the
 * Switch-on-a-Chip Driver.
 *
 * This file also includes the more common include files so the
 * individual driver files don't have to include as much.
 */

/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef SOC_DNXC_INTR_H_INCLUDE
/*
 * {
 */
#define SOC_DNXC_INTR_H_INCLUDE

#if !defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT)
#error "This file is for use by DNX (JR2) and DNXF (Ramon) family only!"
#endif

/*
 * INCLUDE FILES:
 * {
 */
#include <sal/types.h>
#include <shared/cyclic_buffer.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/scache.h>
#include <soc/mem.h>
#include <soc/defs.h>
#include <soc/iproc.h>
#include <soc/dnxc/dnxc_defs.h>
/*
 * }
 */

/*
 * Structs and Enums:
 * {
 */
typedef enum
{
    /**
     * Set default value none operation
     */
    dnxc_mem_mask_mode_none = 0,
    /**
     * Set field value to 0.
     */
    dnxc_mem_mask_mode_zero = 1,
    /**
     * Set field value to 1.
     */
    dnxc_mem_mask_mode_one = 2
} dnxc_mem_mask_mode_e;

/*
 * }
 */
/*
 * MACROs:
 * {
 */
#define INTR_ERROR_MAX_INTERRUPTS_SIZE    50
/*
 * }
 */

int soc_dnxc_intr_init(
    int unit);
int soc_dnxc_intr_deinit(
    int unit);
void soc_dnxc_intr_handler(
    int unit,
    void *data);
void soc_dnxc_ser_intr_handler(
    void *unit_vp,
    void *d1,
    void *d2,
    void *d3,
    void *d4);


#endif /* SOC_DNXC_INTR_H_INCLUDE */
