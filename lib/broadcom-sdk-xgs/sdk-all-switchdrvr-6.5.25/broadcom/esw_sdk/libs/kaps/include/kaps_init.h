/*
 **************************************************************************************
 Copyright 2012-2019 Broadcom Corporation

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

#ifndef __KAPS_INIT_H
#define __KAPS_INIT_H

/**
 * @file init.h
 *
 * This module provides the SerDes and lane-initialization code.
 */

#include <stdint.h>
#include <stdio.h>

#include "kaps_errors.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DEVICE_CONFIG_APIS
 * @{
 */

/**
 * Type of device.
 */

enum kaps_device_type {
    KAPS_DEVICE_UNKNOWN,       /**< Invalid device */
    KAPS_DEVICE_KAPS = 4,      /**< KAPS class of devices. Keeping enum value consistent with KBP header file */
    KAPS_DEVICE_INVALID        /**< This should be the last entry */
};

/**
 * When initializing the device the following flags can be
 * OR'd together to specify the operating mode of the device.
 */

enum kaps_device_flags {
    KAPS_DEVICE_DEFAULT = 0,           /**< Single port, non-SMT, non-network byte order, no cascade mode. */
    KAPS_DEVICE_SMT = 1,               /**< SMT mode in device enabled. Use kaps_device_thread_init() to obtain the thread's device handle. */
    KAPS_DEVICE_DUAL_PORT = 2,         /**< Enable dual-port operation. */
    KAPS_DEVICE_NBO = 4,               /**< Enable network byte order. */
    KAPS_DEVICE_SUPPRESS_INDEX = 8,    /**< Suppress index generation in result when AD is present. */
    KAPS_DEVICE_ISSU = 16,             /**< In-service software upgrade enabled. */
    KAPS_DEVICE_CASCADE = 32,          /**< Cascading of KBPs is enabled */
    KAPS_DEVICE_SKIP_INIT = 64,        /**< Skip the device init. To be used before ISSU restore. */
    KAPS_DEVICE_SUPPRESS_PARITY = 128, /**< Disable Parity scan */
    KAPS_DEVICE_QUAD_PORT = 256,       /**< Up to four port support (Optimus Prime and beyond only) */
    KAPS_DEVICE_HST_DIG_LPBK = 512,    /**< hst digital loopback */
    KAPS_DEVICE_THREAD_SAFETY = 1024   /**< To enable thread-safety at transport layer */
};





/**
 * Device configuration structure used to initialize the device.
 */

struct kaps_device_config {

    /**
     * Total number of devices. Default is 1. When KAPS_DEVICE_CASCADE
     * is specified, the requested number of devices are brought up.
     * The devices are addressed logically from 0 - 3. The user must
     * translate it to actual MDIO MPID
     */

    uint32_t num_devices;


};














/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif /*__INIT_H */
