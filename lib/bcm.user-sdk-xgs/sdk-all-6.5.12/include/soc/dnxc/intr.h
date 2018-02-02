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
 * $Copyright: (c) 2017 Broadcom.
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
#include <shared/utilex/utilex_hashtable.h>
#include <shared/utilex/utilex_multi_set.h>
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/mem.h>
#include <soc/iproc.h>
#include <soc/dnxc/legacy/dnxc_defs.h>
#include <soc/dnxc/legacy/error.h>
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

#endif /* SOC_DNXC_INTR_H_INCLUDE */
