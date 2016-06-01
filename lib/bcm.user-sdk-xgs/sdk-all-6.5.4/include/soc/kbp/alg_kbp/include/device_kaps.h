/*******************************************************************************
 *
 * Copyright 2014-2016 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its
 * licensors, and may only be used, duplicated, modified or distributed pursuant
 * to the terms and conditions of a separate, written license agreement executed
 * between you and Broadcom (an "Authorized License").  Except as set forth in an
 * Authorized License, Broadcom grants no license (express or implied), right to
 * use, or waiver of any kind with respect to the Software, and Broadcom expressly
 * reserves all rights in and to the Software and all intellectual property rights
 * therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS
 * SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use all
 * reasonable efforts to protect the confidentiality thereof, and to use this
 * information only in connection with your use of Broadcom integrated circuit
 * products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
 * "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS
 * OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
 * TO THE SOFTWARE. BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM
 * OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
 * OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 *******************************************************************************/

#ifndef __DEVICE_KAPS_H
#define __DEVICE_KAPS_H

#include <stdint.h>
#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @cond INTERNAL
 *
 * @file device_kaps.h
 *
 * Defines depths, widths and the number of for all software-visible resources in KAPS
 */


#define KAPS_AD_WIDTH_1 (20)

#define KAPS_AD_WIDTH_8 (3)

#define KAPS_SEARCH_INTERFACE_WIDTH_1 (160)

#define KAPS_SEARCH_INTERFACE_WIDTH_8 (20)

#define KAPS_NUM_BB_SEARCH_PORTS (2)

#define KAPS_DBA_WIDTH_8 (20)

#define KAPS_DBA_WIDTH_1 (160)

#define KAPS_REGISTER_WIDTH_1 (32)

#define KAPS_REGISTER_WIDTH_8 (4)

#define KAPS_DEFAULT_DEVICE_ID (0x10001)

#define KAPS_QUMRAN_DEVICE_ID (0x20002)

#define KAPS_JERICHO_PLUS_DEVICE_ID (0x30003)

#define KAPS_JERICHO_2_DEVICE_ID      (0x50005)

#define KAPS_REVISION_REGISTER_ADDR (0x0)


/**
 *   @brief KAPS HW Commands
 */
enum kaps_cmd
{
    KAPS_CMD_READ,     /**< KAPS command to read through IBC */
    KAPS_CMD_WRITE,    /**< KAPS command to write through IBC */
    KAPS_CMD_EXTENDED, /**< KAPS command for extended functionality through IBC */
    KAPS_CMD_ACK       /**< KAPS IBC command acknowledgement*/
};



/**
 *   @brief KAPS HW Functions
 */
enum kaps_func
{
    KAPS_FUNC0,       /**< KAPS IBC function 0 **/
    KAPS_FUNC1,       /**< KAPS IBC function 1 **/
    KAPS_FUNC2,       /**< KAPS IBC function 2 **/
    KAPS_FUNC3,       /**< KAPS IBC function 3 **/
    KAPS_FUNC4,       /**< KAPS IBC function 4 **/
    KAPS_FUNC5,       /**< KAPS IBC function 5 **/
    KAPS_FUNC6,       /**< KAPS IBC function 6 **/
    KAPS_FUNC7,       /**< KAPS IBC function 7 **/
    KAPS_FUNC8,       /**< KAPS IBC function 8 **/
    KAPS_FUNC9,       /**< KAPS IBC function 9 **/
    KAPS_FUNC10,      /**< KAPS IBC function 10 **/
    KAPS_FUNC11,      /**< KAPS IBC function 11 **/
    KAPS_FUNC12,      /**< KAPS IBC function 12 **/
    KAPS_FUNC13,      /**< KAPS IBC function 13 **/
    KAPS_FUNC14,      /**< KAPS IBC function 14 **/
    KAPS_FUNC15       /**< KAPS IBC function 15 **/
};

/**
 *   @brief KAPS Search interface
 */

enum kaps_search_interface
{
    KAPS_SEARCH_0_A,    /**< s0a search interface*/
    KAPS_SEARCH_0_B,    /**< s0b search interface*/
    KAPS_SEARCH_1_A,    /**< s1a search interface*/
    KAPS_SEARCH_1_B     /**< s1b search interface*/
};

/**
 * @brief Search result from the KAPS device
 */

struct kaps_search_result
{
    uint8_t ad_value[KAPS_AD_WIDTH_8];  /**< The associated data value of the matching entry*/
    uint32_t match_len; /**< Length of the best match */
};

/**
 * @brief KAPS DBA entry
 */

struct kaps_dba_entry
{
    uint8_t key[KAPS_SEARCH_INTERFACE_WIDTH_8]; /**< 160b data or mask */
    __EO_3(uint32_t resv:4,       /**< Unused */
           uint32_t is_valid:2,   /**< Valid bit per 80b part of data/mask */
           uint32_t pad:2);       /**< SW pad */
};

/**
 * @brief KAPS AD info representation
 */
struct kaps_ad_entry
{

    __EO_5(uint32_t ad:20,   /**< Associated data */
           uint32_t resv:12, /**< unused */
           uint32_t resv1,/**< unused */
           uint32_t resv2,/**< unused */
           uint32_t resv3);/**< unused */
};


/**
 * Initializes the KAPS device handling by reading the KAPS Revision Register
 *
 * @param device Valid device handle.
 *
 * @return KBP_OK on success or an error code otherwise.
 */

kbp_status kbp_dm_kaps_init(struct kbp_device *device);


/**
 * Writes data/mask of specified length(nbytes) to the specified row(entry_nr) and block(blk_num).
 * Data can be written in either DM or XY mode.
 * valid_bit is set 0 to invalidate, 3 to validate(1 and 2 are reserved) entries via this API
 *
 * @param device Valid device handle.
 * @param blk_num DBA block number
 * @param entry_nr Location of the entry in the DBA block
 * @param nbytes Number of Bytes of data to be written
 * @param data Valid data pointer to write
 * @param mask Valid mask pointer to write
 * @param is_xy Whether data and mask are specified in XY format or not
 * @param valid_bit 0 to invalidate the entry, 3 to validate (1 and 2 are reserved)
 *
 * @return KBP_OK on success or an error code otherwise.
 */
kbp_status kbp_dm_kaps_dba_write(struct kbp_device *device, uint32_t blk_num, uint32_t entry_nr,
                                 uint32_t nbytes, uint8_t *data, uint8_t *mask, uint8_t is_xy,
                                 uint8_t valid_bit);

/**
 * Reads data/mask of specified length(nbytes) from the specified row(row_num) and block(blk_num).
 *
 * @param device Valid device handle.
 * @param blk_num DBA block number
 * @param row_num Location of the entry in the DBA block
 * @param nbytes Number of bytes of data to read
 * @param o_entry_x entry structure to read x
 * @param o_entry_y entry structure to ready
 *
 * @return KBP_OK on success or an error code otherwise.
 */
kbp_status kbp_dm_kaps_dba_read(struct kbp_device *device, uint32_t blk_num, uint32_t entry_nr,
                                struct kaps_dba_entry *o_entry_x, struct kaps_dba_entry *o_entry_y);

/**
* Function to write value to the specified register(reg_num)
*
* @param device Valid device handle.
* @param blk_num the block number (used by KAPS)
* @param reg_num valid reg_num to write
* @param nbytes the number of bytes to write
* @param data stream of data to write
*
* @return KBP_OK on success or an error code otherwise.
*/
kbp_status kbp_dm_kaps_reg_write(struct kbp_device *device, uint32_t blk_num, uint32_t reg_num,
                                 uint32_t nbytes, uint8_t *data);
/**
* Function to read value of the specified register(reg_num)
*
* @param device Valid device handle.
* @param blk_num the block number (used by KAPS)
* @param reg_num valid reg_num to read
* @param nbytes the number of bytes to be read
* @param o_data stream of read data
*
* @return KBP_OK on success or an error code otherwise.
*/
kbp_status kbp_dm_kaps_reg_read(struct kbp_device *device, uint32_t blk_num, uint32_t reg_num,
                                uint32_t nbytes, uint8_t *o_data);

 /**
 * Function to write ad to the specified UDA address(uda_addr)
 *
 * @param device Valid device handle.
 * @param ab_num the AB number (used by KAPS)
 * @param uda_addr the physical UDA address where the entry has to be written
 * @param nbytes the number of bytes to write
 * @param data stream of data to write
 *
 * @return KBP_OK on success or an error code otherwise.
 */
kbp_status kbp_dm_kaps_ad_write(struct kbp_device *device, uint32_t ab_num, uint32_t uda_addr,
                                uint32_t nbytes, uint8_t *data);

/**
* Function to read ad from the specified UDA address(uda_addr)
*
* @param device Valid device handle.
* @param ab_num the AB number (used by KAPS)
* @param uda_addr the physical UDA address where the entry is written to is returned
* @param nbytes the number of bytes to be read
* @param o_data stream of read data
*
* @return KBP_OK on success or an error code otherwise.
*/
kbp_status kbp_dm_kaps_ad_read(struct kbp_device *device, uint32_t ab_num, uint32_t uda_addr,
                               uint32_t nbytes, uint8_t *o_data);

/**
* Devmgr kaps Search function.
*
* @param device Valid device handle.
* @param key the data to be searched in the KAPS device. For example, if five bytes of data have to be searched,
*            these bytes will be stored in key[0] to key[4].
* @param search_interface the search interface (s0a...s1b) to be used to search the key.
* @param kaps_result the result of the KAPS search is returned here.
*
* @return KBP_OK on success or an error code otherwise.
*/
kbp_status kbp_dm_kaps_search(struct kbp_device * device, uint8_t *key, enum kaps_search_interface search_interface,
                              struct kaps_search_result *kaps_result);

/**
 * @endcond
 */

#ifdef __cplusplus
}
#endif

#endif /* __DEVICE_KAPS_H */
