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

#ifndef __KAPS_DEVICE_KAPS_ALG_H
#define __KAPS_DEVICE_KAPS_ALG_H

#include "kaps_device_internal.h"
#include "kaps_bitmap.h"

#define KAPS_BKT_WIDTH_1 (480)

#define KAPS_BKT_WIDTH_8 (60)

#define KAPS_BKT_MIN_NUM_ROWS (256)

#define KAPS_BKT_DEFAULT_NUM_ROWS (1024)

#define KAPS_BKT_MAX_NUM_ROWS (16 * 1024)

#define KAPS_ADS_WIDTH_1 (128)

#define KAPS_ADS_WIDTH_8 (16)

#define KAPS_ADS_MIN_NUW_ROWS (1 * 1024)

#define KAPS_ADS_DEFAULT_NUM_ROWS (2 * 1024)

#define KAPS_MIN_NUM_ADS_BLOCKS (2)

#define KAPS_DEFAULT_NUM_ADS_BLOCKS (4)

#define KAPS_MAX_NUM_ADS_BLOCKS (8)

#define KAPS_RPB_WIDTH_1 (160)

#define KAPS_RPB_WIDTH_8 (20)

#define KAPS_RPB_MIN_BLOCK_SIZE (512)

#define KAPS_RPB_DEFAULT_BLOCK_SIZE (2 * 1024)

#define KAPS_RPB_MAX_BLOCK_SIZE (8 * 1024)

#define KAPS_MIN_NUM_RPB_BLOCKS (2)

#define KAPS_DEFAULT_NUM_RPB_BLOCKS (4)

#define KAPS_NUM_FORMATS (16)

#define KAPS_MAX_NUM_BB (256)

#define KAPS_DEFAULT_MIN_NUM_BB (16)

#define KAPS_MAX_SEARCH_TAG (0x7F)

#define KAPS_MAX_PFX_PER_BKT_ARRAY_SIZE (16)

#define KAPS_MAX_NUM_GRANULARITIES (15)

#define KAPS_AD_ARRAY_START_BIT_POS (4)

#define KAPS_AD_ARRAY_END_BIT_POS (23)

#define KAPS_DMA_WIDTH_1 (128)

#define KAPS_DMA_NUM_ROWS (1024 * 4)

#define KAPS_DMA_TAG_MAX (255)

#define KAPS_RPB_BLOCK_START (1)

#define KAPS2_RPB_BLOCK_END  (2)

#define KAPS2_SMALL_BB_START (10)

#define KAPS2_SMALL_BB_END   (25)

#define KAPS2_ADS2_BLOCK_START (42)

#define KAPS2_ADS2_BLOCK_END    (43)

#define KAPS2_LARGE_BB_START (44)

#define KAPS2_LARGE_BB_END   (57)

#define KAPS2_NUM_LARGE_BB   (14)

#define KAPS_BB_GLOBAL_CONFIG_REG_ADDR (0x21)

#define KAPS2_BIG_CONFIG_REG_ADDR (0xF0)

#define KAPS2_BIG_BB_DEPTH_REG_ADDR (0xF1)

#define KAPS_ADS2_MAX_NUM_ROWS (32 * 1024)

#define KAPS_TOTAL_NUM_SMALL_BB (16)

#define KAPS_SMALL_BBS_PUBLIC_PRIVATE (16)

#define KAPS_BMP_IN_SMALL_BB_8 (2)

#define KAPS2_DEFAULT_NUM_RPB_BLOCKS (8)

#define KAPS2_DEFAULT_NUM_RPB_ROWS (1024)

#define KAPS2_DEFAULT_NUM_BB (14)

#define KAPS2_DEFAULT_NUM_BKT_ROWS  (16 * 1024)

#define KAPS2_BPM_ADS2_LOCATION_SIZE_1 (20)



enum KAPS_HW_BLOCK_TYPE
{
    KAPS_BLOCK_TYPE_RPB,
    KAPS_BLOCK_TYPE_ADS1,
    KAPS_BLOCK_TYPE_SMALL_BB,
    KAPS_BLOCK_TYPE_ADS2,
    KAPS_BLOCK_TYPE_LARGE_BB
};



/*For KAPS-2, there are 16 Small BBs.
Each brick in a small BB can have a maximum of 16 entries in it (assuming lowest granularity of 8b)
So the maximum virtual IT space needed for each RPT entry (that is same as virtual IT space
per AB) is = 16 small BBs * 16 entries per BB*/
#define KAPS_ADS2_VIRTUAL_IT_SIZE_PER_AB (16 * 16)



/*For KAPS-2, max slots in AB = 16 small BBs * 16 entries per BB*/
#define KAPS2_MAX_SLOTS_IN_AB   (16 * 16)

/**
 * @brief KAPS Associated Data Store
 */
struct kaps_ads
{
    uint32_t bpm_ad:20;     /**< Associated data of the lmpsofar*/

    uint32_t bpm_len:8;         /**< Best Matching Prefix length*/
    uint32_t row_offset:4;        /**< Bucket block number corresponding to the first LPU of the LSN*/

    uint32_t format_map_00:4;      /**< Format map indicating the granularities of the buckets*/
    uint32_t format_map_01:4;
    uint32_t format_map_02:4;
    uint32_t format_map_03:4;
    uint32_t format_map_04:4;
    uint32_t format_map_05:4;
    uint32_t format_map_06:4;
    uint32_t format_map_07:4;

    uint32_t format_map_08:4;
    uint32_t format_map_09:4;
    uint32_t format_map_10:4;
    uint32_t format_map_11:4;
    uint32_t format_map_12:4;
    uint32_t format_map_13:4;
    uint32_t format_map_14:4;
    uint32_t format_map_15:4;

    

    uint32_t bkt_row:14;            /**< The row in the bucket block */

    uint32_t reserved:2;            /**< Reserved field */
    uint32_t key_shift:8;           /**< Number of bits by which the key should be shifted before sending to bucket block*/
    uint32_t ecc:8;                /**< Error correcting code, will be filled up by hardware */
};

/**
* @brief KAPS Root Pivot Block entry
*/
struct kaps_rpb_entry
{
    uint8_t data[KAPS_RPB_WIDTH_8]; /**< The X portion of the RPB entry */
    uint8_t mask[KAPS_RPB_WIDTH_8]; /**< The Y portion of the RPB entry */
    uint8_t rpb_valid_data;         /**< indicates if data is valid*/
    uint8_t rpb_valid_mask;         /**< indicates if mask is valid*/
    uint8_t rpb_tbl_id;             /**< the table id corresponding to the RPB entry*/
};

/**
* @brief KAPS Shadow for RPB Blocks
*/
struct kaps_shadow_rpb
{
    struct kaps_rpb_entry *rpb_rows;  /**< dynamically created array that holds data, mask of each row. The size will be number of 160b rows*/
};

struct kaps_shadow_bkt_row
{
    uint8_t data[KAPS_BKT_WIDTH_8];
};

/**
* @brief KAPS Shadow for the Bucket Block
*/
struct kaps_shadow_bkt
{
    struct kaps_shadow_bkt_row *bkt_rows;  /**< Data stored in the shadow Bucket Blocks*/
};

/**
* @brief KAPS Shadow for ADS Blocks
*/
struct kaps_shadow_ads
{
    struct kaps_ads *ads_rows; /**< dynamically created array that holds ADS value of each row. The size will be number of 160b rows in rpb*/
};

/**
* @brief KAPS Shadow for ADS2 Blocks that are in between the Small BB and Large BB
*/
struct kaps_shadow_ads2
{
    struct kaps_ads *ads_rows;
};

/**
* @brief Additional information for mapping virtual IT space to real IT space
*/
struct kaps_shadow_ads2_overlay
{
    int32_t *x_table; /**< Given the virtual IT location, we get the corresponding real location*/
    struct kaps_fast_bitmap it_fbmp; /**< IT bitmap for the real IT space. It is smaller than the virtual IT space size*/
    uint32_t num_free_it_slots; /**< Total number of free slots in the real IT space*/
};

/**
* @brief One KAPS sub AB maps to a brick in the small BBs
*/
struct kaps_sub_ab
{
    int32_t lower_index; /**< Starting index in the AB where the Sub AB starts*/
    int32_t upper_index; /**< Ending index in the AB where the Sub AB ends */
    int32_t small_bb_num; /**< Small BB number (0-15) for this sub AB*/
    int32_t row_num; /**< Row number of the Small BB brick*/
    int32_t sub_ab_gran; /**< Granularity of the entries in a small BB brick*/
    uint8_t free_slot_bmp[KAPS_BMP_IN_SMALL_BB_8]; /**< bitmap which indicates the free slots within the current brick (Sub AB)*/
};

/**
* @brief Information for mapping an AB to a set of bricks in the small BBs
*/
struct kaps_shadow_ab_to_small_bb
{
    struct kaps_sub_ab sub_ab_bricks[KAPS_TOTAL_NUM_SMALL_BB];  /**< Info about the bricks for this AB*/
    uint32_t num_bricks; /**< Number of bricks in this AB*/
};




/**
* @brief KAPS Shadow Device
*/
struct kaps_shadow_device
{
    struct kaps_shadow_rpb *rpb_blks; /**< Shadow RPB blocks */
    struct kaps_shadow_ads *ads_blks; /**< Shadow ADS blocks */
    struct kaps_shadow_bkt *bkt_blks; /**< Shadow Bucket Block*/
    struct kaps_shadow_bkt *small_bbs; /**< Small Bucket Blocks*/
    struct kaps_shadow_ads2 *ads2_blks; /**< Shadow ADS2 blocks that are between Small BB and Large BB*/
    struct kaps_shadow_ads2_overlay *ads2_overlay; /**< Table for converting virtual AB index to position in the ADS-2*/
    struct kaps_shadow_ab_to_small_bb *ab_to_small_bb; /**< AB to small BB mapping for 3 level KAPS*/

};




/**
* Function to write value to the specified offset in bucket block(bb_num) through dma.
*
* @param device Valid device handle.
* @param bb_num the Bucket block number (used by KAPS)
* @param offset offset in bucket block
* @param length the number of bytes to write
* @param data stream of data to write
*
* @return KAPS_OK on success or an error code otherwise.
*/
kaps_status kaps_dm_kaps_dma_bb_write(
    struct kaps_device *device,
    uint32_t bb_num,
    uint32_t offset,
    uint32_t length,
    uint8_t * data);

/**
* Function to read value of the specified offset in bucket block(bb_num) through dma.
*
* @param device Valid device handle.
* @param blk_num the block number (used by KAPS)
* @param offset offset in bucket block
* @param length the number of bytes to read
* @param o_data stream of read data
*
* @return KAPS_OK on success or an error code otherwise.
*/
kaps_status kaps_dm_kaps_dma_bb_read(
    struct kaps_device *device,
    uint32_t bb_num,
    uint32_t offset,
    uint32_t length,
    uint8_t * o_data);

/**
* Function to write value to the specified offset in bucket block(bb_num).
*
* @param device Valid device handle.
* @param bb_num the Bucket block number (used by KAPS)
* @param offset offset in bucket block
* @param length the number of bytes to write
* @param is_final_level indicates if we are writing to the last algorithmic level
* @param data stream of data to write
*
* @return KAPS_OK on success or an error code otherwise.
*/
kaps_status kaps_dm_kaps_bb_write(
    struct kaps_device *device,
    struct kaps_db *db,
    uint32_t bb_num,
    uint32_t offset,
    uint8_t is_final_level,
    uint32_t length,
    uint8_t * data);

/**
* Function to read value of the specified offset in bucket block(bb_num).
*
* @param device Valid device handle.
* @param bb_num the Bucket block number (used by KAPS)
* @param offset offset in bucket block
* @param length the number of bytes to read
* @param o_data stream of read data
*
* @return KAPS_OK on success or an error code otherwise.
*/
kaps_status kaps_dm_kaps_bb_read(
    struct kaps_device *device,
    uint32_t bb_num,
    uint32_t offset,
    uint32_t length,
    uint8_t * o_data);

/**
* Function to write value to the specified block(bb_index) at register(reg_num).
*
* @param device Valid device handle.
* @param bb_num the Bucket block number (used by KAPS)
* @param reg_num reg address where data has to be written
* @param length the number of bytes to write
* @param register_data stream of data to write
*
* @return KAPS_OK on success or an error code otherwise.
*/
kaps_status kaps_dm_kaps_alg_reg_write(
    struct kaps_device *device,
    uint32_t bb_index,
    uint32_t reg_num,
    uint32_t length,
    uint8_t * register_data);

/**
* Function to read value of the specified block(bb_index) at register(reg_num).
*
* @param device Valid device handle.
* @param bb_index the Bucket block number (used by KAPS)
* @param reg_num reg address to read the data
* @param length the number of bytes to read
* @param o_data stream of read register data
*
* @return KAPS_OK on success or an error code otherwise.
*/
kaps_status kaps_dm_kaps_alg_reg_read(
    struct kaps_device *device,
    uint32_t bb_index,
    uint32_t reg_num,
    uint32_t length,
    uint8_t * o_data);

kaps_status kaps_dm_kaps_kaps_rit_write(
    struct kaps_device *device,
    uint32_t blk_num,
    uint32_t rit_addr,
    uint32_t length,
    uint8_t * data);

/**
* Function to write value to the specified iit address(iit_addr).
*
* @param device Valid device handle.
* @param blk_num the block number (used by KAPS)
* @param iit_addr iit address where data has to be written
* @param length the number of bytes to write
* @param data stream of data to write
*
* @return KAPS_OK on success or an error code otherwise.
*/
kaps_status kaps_dm_kaps_kaps_iit_write(
    struct kaps_device *device,
    struct kaps_db *db,
    uint32_t blk_num,
    uint32_t iit_addr,
    uint32_t length,
    uint8_t * data);

/**
* Function to read value of the specified iit address(iit_addr).
*
* @param device Valid device handle.
* @param blk_num the block number (used by KAPS)
* @param iit_addr iit address to read the data
* @param length the number of bytes to read
* @param o_data stream of read data
*
* @return KAPS_OK on success or an error code otherwise.
*/
kaps_status kaps_dm_kaps_iit_read(
    struct kaps_device *device,
    uint32_t blk_num,
    uint32_t iit_addr,
    uint32_t length,
    uint8_t * o_data);

/**
 * Writes data/mask of specified length(nbytes) to the specified row(row_num) and block(blk_num).
 * Data can be written in either DM or XY mode.
 * valid_bit is set 0 to invalidate, 3 to validate(1 and 2 are reserved) entries via this API
 *
 * @param device Valid device handle.
 * @param blk_num DBA block number
 * @param row_num Location of the entry in the DBA block
 * @param data Valid data pointer to write
 * @param mask Valid mask pointer to write
 * @param valid_bit 0 to invalidate the entry, 3 to validate (1 and 2 are reserved)
 *
 * @return KAPS_OK on success or an error code otherwise.
 */
kaps_status kaps_dm_kaps_rpb_write(
    struct kaps_device *device,
    uint32_t blk_num,
    uint32_t row_num,
    uint8_t * data,
    uint8_t * mask,
    uint8_t valid_bit);

/**
 * Reads data/mask of specified length(nbytes) from the specified row(row_num) and block(blk_num).
 *
 * @param device Valid device handle.
 * @param blk_num DBA block number
 * @param row_num Location of the entry in the DBA block
 * @param nbytes Number of bytes of data to read
 * @param o_entry_x entry structure to read x
 * @param o_entry_y entry structure to read y
 *
 * @return KAPS_OK on success or an error code otherwise.
 */
kaps_status kaps_dm_kaps_rpb_read(
    struct kaps_device *device,
    uint32_t blk_num,
    uint32_t row_num,
    struct kaps_dba_entry *o_entry_x,
    struct kaps_dba_entry *o_entry_y);

/**
* Function to reset KAPS blocks.
*
* @param device Valid device handle.
* @param blk_nr the block number (used by KAPS)
* @param row_num the row number of the block
* @param length the number of bytes
* @param data stream of data
*
* @return KAPS_OK on success or an error code otherwise.
*/
kaps_status kaps_dm_kaps_reset_blks(
    struct kaps_device *device,
    uint32_t blk_nr,
    uint32_t row_nr,
    uint32_t nbytes,
    uint8_t * data);

/**
* Function to enumerate KAPS blocks.
*
* @param device Valid device handle.
* @param blk_nr the block number (used by KAPS)
* @param row_num the row number of the block
* @param length the number of bytes
* @param data stream of data
*
* @return KAPS_OK on success or an error code otherwise.
*/
kaps_status kaps_dm_kaps_enumerate_blks(
    struct kaps_device *device,
    uint32_t blk_nr,
    uint32_t row_nr,
    uint32_t nbytes,
    uint8_t * data);

/**
* Function to read the possible width options for the Large Bucket Blocks in KAPS-2
*
* @param device Valid device handle.
* @param large_bb_nr the block number starting from 0
* @param options options are read from the device and returned
*
* @return KAPS_OK on success or an error code otherwise.
*/
kaps_status kaps_dm_kaps_read_bb_config_options(
    struct kaps_device *device,
    uint32_t large_bb_nr,
    uint32_t * options);

/**
* Function to write the width option for the Large Bucket Blocks in KAPS-2
*
* @param device Valid device handle.
* @param large_bb_nr the block number starting from 0
* @param option chosen width option to be written to device
*
* @return KAPS_OK on success or an error code otherwise.
*/
kaps_status kaps_dm_kaps_write_bb_config_option(
    struct kaps_device *device,
    uint32_t large_bb_nr,
    uint32_t option);

/**
* Function to read the number of rows present in the Large Bucket Block for KAPS-2. This API is called after the kaps_dm_kaps_write_bb_config_option
* function is called by the SDK. So we know in the final chosen configuration, how many rows are present
*
* @param device Valid device handle.
* @param large_bb_nr the block number starting from 0
* @param bb_num_rows the number of rows are read from the device and returned
*
* @return KAPS_OK on success or an error code otherwise.
*/
kaps_status kaps_dm_kaps_read_large_bb_num_rows(
    struct kaps_device *device,
    uint32_t large_bb_nr,
    uint32_t * bb_num_rows);

/**
* Function to read the ADS-2 depth in KAPS-2
*
* @param device Valid device handle.
* @param ads2_nr the second level ADS block number starting from 0
* @param depth the depth is read from the device and returned here
*
* @return KAPS_OK on success or an error code otherwise.

*/
kaps_status kaps_dm_kaps_read_ads_depth(
    struct kaps_device *device,
    uint32_t ads2_nr,
    uint32_t * depth);





#endif
