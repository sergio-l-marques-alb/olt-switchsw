/** \file soc/dnxc/drv.h
 * Slim SoC module to allow bcm actions.
 * 
 * This file contains structure and routine declarations for the
 * Switch-on-a-Chip Driver.
 *
 * This file also includes the more common include files so the
 * individual driver files don't have to include as much. 
 */

/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef SOC_DNXC_DRV_H_INCLUDE
/*
 * { 
 */
#define SOC_DNXC_DRV_H_INCLUDE

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
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/mem.h>
#include <soc/iproc.h>
#include <soc/dnxc/dnxc_defs.h>
#include <soc/dnxc/error.h>
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

/**
 * \brief
 *   if proper reset action is given, resets device and makes sure device is out of reset.
 *   The possible reset_action values are:
 *   SOC_DNXC_RESET_ACTION_IN_RESET, SOC_DNXC_RESET_ACTION_INOUT_RESET.
 */
int soc_dnxc_cmicx_device_hard_reset(
    int unit,
    int reset_action);

/**
 * \brief
 *   The function is reading the field of an ECI register that is giving the value of the AVS.
 *   The possible avs_val values are:
 *   Device dependant.
 */

int soc_dnxc_avs_value_get(
    int unit,
    uint32 *avs_val);

/**
 * \brief
 *   The function is to dump chip infomations.
 */
void soc_dnxc_chip_dump(
    int unit,
    soc_driver_t * d);

int soc_dnxc_nof_block_instances(
    int unit,
    soc_block_types_t block_types,
    int *nof_block_instances);

shr_error_e soc_dnxc_verify_device_init(
    int unit);

/**
 * \brief
 *  Override block instance validity from dnx/dnxf data.
 */
shr_error_e dnxc_block_instance_validity_override(
    int unit);

#endif /* SOC_DNXC_DRV_H_INCLUDE */
