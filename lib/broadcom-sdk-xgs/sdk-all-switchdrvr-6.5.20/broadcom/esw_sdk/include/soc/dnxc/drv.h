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
 * $Copyright: (c) 2020 Broadcom.
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
extern uint32 test_was_run_and_suceeded[(SOC_MAX_NUM_DEVICES + 31) / 32];
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

/**
 * \brief
 *   check if the device is marked as needing a system reset (performed by the board).
 *   Returns a non-zero value if the device needs a system reset
 */
int soc_dnxc_does_need_system_reset(
    int unit);

/**
 * \brief
 *   sets if the device is marked as needing a system reset (performed by the board).
 *   The device will be marked as needing a system reset if needs_system_reset is non-zero.
 */
void soc_dnxc_set_need_system_reset(
    int unit,
    int needs_system_reset);

/**
 * \brief
 *   check if performing a system reset for the device is supported.
 *   This requires that a system reset callback was registered by the application
 *   and that the callback reports actually supporting performing a system reset.
 *   Returns a non-zero value if supported.
 */
int soc_dnxc_does_support_system_reset(
    int unit);

/**
 * \brief
 *   Performs a system reset of the unit, using the callback registered for this purpose.
 *   The callback should also handle the needed PCIe reset.
 *   Returns a non-zero value if The system reset was performed and it was successful.
 */
int soc_dnxc_perform_system_reset(
    int unit);

/**
 * \brief
 *   Checks if a system reset is required and if so attempts to perform it
 *   returns SOC_E_NONE if a system reset is not required or if a system reset
 *   was performed successfully.
 */
int soc_dnxc_perform_system_reset_if_needed(
    int unit);
#endif /* SOC_DNXC_DRV_H_INCLUDE */
