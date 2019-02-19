/*
 **************************************************************************************
 Copyright 2009-2016 Broadcom Corporation

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

#ifndef __XPT_KAPS_H
#define __XPT_KAPS_H

/**
 * @file xpt_kaps.h
 *
 * This module contains the functions and data structures for the transport layer of the KAPS device.
 */

#include <stdint.h>
#include "errors.h"
#include "device_kaps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup KAPS_XPT_APIS
 * @{
 */

/**
 * KAPS transport-layer implementation structure expected to be passed into
 * the call to kbp_device_init().
 */

struct kaps_xpt
{
    /**
    * The device type. Should always be the first member of the structure.
    * Should be set to KBP_DEVICE_KAPS.
    */
    uint32_t device_type;


    /**
    * Search function.
    *
    * @param xpt pointer to the xpt that was passed to kbp_device_init() is passed back here.
    * @param key the data to be searched in the KAPS device. For example, if five bytes of data have to be searched, these
    * bytes will be stored in key[0] to key[4].
    * @param search_interface the search interface (s0a...s1b) to be used to search the key.
    * @param kaps_result the result of the KAPS search is returned here.
    *
    * @return KBP_OK on success or an error code otherwise.
    */
    kbp_status (*kaps_search) (void *xpt, uint8_t *key, enum kaps_search_interface search_interface, struct kaps_search_result *kaps_result);


    /**
    * Function to send various private commands to KAPS
    *
    * @param xpt pointer to the xpt that was passed to kbp_device_init() is passed back here.
    * @param cmd Command type
    * @param func Function to be selected for command
    * @param blk_nr Block Number to send the instruction
    * @param row_nr Row Number to send the instruction
    * @param nbytes the number of bytes in payload
    * @param bytes the payload
    *
    * @return KBP_OK if successfully dispatched to device
    */
    kbp_status (*kaps_command)(void *xpt, enum kaps_cmd cmd, enum kaps_func func, uint32_t blk_nr, uint32_t row_nr, uint32_t nbytes, uint8_t *bytes);

    /**
    * Function to send various register read commands to KAPS
    *
    * @param xpt pointer to the xpt that was passed to kbp_device_init() is passed back here.
    * @param offset Offset of the register to be read
    * @param nbytes the number of bytes in payload
    * @param bytes the payload
    *
    * @return KBP_OK if successfully dispatched to device
    */
    kbp_status (*kaps_register_read)(void *xpt, uint32_t offset, uint32_t nbytes, uint8_t *bytes);

    /**
    * Function to send various register write commands to KAPS
    *
    * @param xpt pointer to the xpt that was passed to kbp_device_init() is passed back here.
    * @param offset Offset of the register to be written
    * @param nbytes the number of bytes in payload
    * @param bytes the payload
    *
    * @return KBP_OK if successfully dispatched to device
    */
    kbp_status (*kaps_register_write)(void *xpt, uint32_t offset, uint32_t nbytes, uint8_t *bytes);


};


/**
 * @}
 */

#ifdef __cplusplus
}
#endif


#endif /* __XPT_KAPS_H */

