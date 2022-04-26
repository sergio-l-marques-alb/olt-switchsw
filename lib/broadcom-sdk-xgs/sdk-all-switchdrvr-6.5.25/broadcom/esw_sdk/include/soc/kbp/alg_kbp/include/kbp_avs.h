/*
 **************************************************************************************
 Copyright 2012-2021 Broadcom Corporation

 This program is the proprietary software of Broadcom Corporation and/or its licensors,
 and may only be used, duplicated, modified or distributed pursuant to the terms and
 conditions of a separate, written license agreement executed between you and
 Broadcom (an "Authorized License").Except as set forth in an Authorized License,
 Broadcom grants no license (express or implied),right to use, or waiver of any kind
 with respect to the Software, and Broadcom expressly reserves all rights in and to
 the Software and all intellectual property rights therein.
 IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY
 WAY,AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization, constitutes the
    valuable trade secrets of Broadcom, and you shall use all reasonable efforts to
    protect the confidentiality thereof,and to use this information only in connection
    with your use of Broadcom integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" AND WITH
    ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER
    EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM
    SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
    NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS LICENSORS
    BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES
    WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY TO USE
    THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF
    OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
    ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
 **************************************************************************************
 */

#ifndef __KBP_AVS_H
#define __KBP_AVS_H

/**
 * @file kbp_avs.h
 *
 * This module is to find AVS convergence voltage.
 */

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Default VMAX, VMIN values
 */
#define DEFAULT_VMAX_AVS    (920)
#define DEFAULT_VMIN_AVS    (750)

/**
 * AVS configuration structure
 */

struct kbp_avs_config
{
    void *handle;                    /**< Opaque handle passed to MDIO reset routines as first argument. */

    /**
     * MDIO read function. Implemented by the user. Returns a 16b read value as part of the last argument
     *
     * @param handle Opaque user provided handle passed to the callback.
     * @param chip_no Virtual chip number to address the device on cascade. The user is responsible to translate to the real MPID select.
     * @param dev The MDIO device ID.
     * @param reg The MDIO register to be read.
     * @param value Returned by the call. Valid if function returns zero
     *
     * @retval 0 On success, the contents of value are valid
     * @retval non-zero On failure. The contents of value are ignored.
     */

    int32_t (*mdio_read)(void *handle, int32_t chip_no, uint8_t dev, uint16_t reg, uint16_t *value);

    /**
     * MDIO write function. Implemented by the user. Writes a 16b value to the specified MDIO register
     *
     * @param handle Opaque user provided handle passed to the callback.
     * @param chip_no Virtual chip number to address the device on cascade. The user is responsible to translate to the real MPID select.
     * @param dev The MDIO device ID.
     * @param reg The MDIO register to write to.
     * @param value The value to write to the register.
     *
     * @retval 0 On success, the value has been written.
     * @retval non-zero On failure. Value could not be written to the register.
     */

    int32_t (*mdio_write)(void *handle, int32_t chip_no, uint8_t dev, uint16_t reg, uint16_t value);

    int32_t max_avs_voltage; /**< Upper limit for die convergence voltage */
    int32_t min_avs_voltage; /**< Lower limit for die convergence voltage */
};

/**
 * Find out the AVS convergence voltage
 *
 * @param device Valid device handle.
 * @param vmax Maximum voltage limit
 * @param vmin Minimum voltage limit
 * @param fp file to log the dump
 * @return KBP_OK on success or an error code otherwise
 *
 */
kbp_status kbp_device_init_avs(struct kbp_device *device, int32_t vmax, int32_t vmin, FILE *fp);

/**
 * Find out the AVS convergence voltage
 * Expected S_RST and C_SRT are already deasserted or device interface init is called
 * before calling this API
 *
 * @param type The device type, currently only Optimus Prime is supported
 * @param config Interface configuration structure initialized correctly
 * @param fp file to log the dump
 *
 * @retval KBP_OK on success or an error code otherwise
 */
kbp_status kbp_device_interface_init_avs(enum kbp_device_type type, struct kbp_avs_config *config,
                                         FILE *fp);

/**
 * Find out if the AVS is enabled or not
 *
 * @param type The device type, currently only Optimus Prime is supported
 * @param config Interface configuration structure initialized correctly
 * @param is_enabled set to 1 if AVS is enabled else to 0
 *
 * @retval KBP_OK on success or an error code otherwise
 */
kbp_status kbp_device_interface_is_avs_enabled(enum kbp_device_type type, struct kbp_avs_config *config,
                                               uint32_t *is_enabled);

#ifdef __cplusplus
}
#endif
#endif /*__KBP_AVS_H */
