/*******************************************************************************
 *
 * Copyright 2014-2019 Broadcom Corporation
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

#include "kaps_utility.h"
#include "kaps_device_internal.h"
#include "kaps_algo_hw.h"

/*
 * Writing to device is prevented if
 * XPT is null or ISSU is in progress
 */
#define KAPS_DM_KAPS_CHECK_XPT_WRITE(device, i)    \
      device->num_of_piowrs += i;                 \
      if ((device->xpt == NULL)                   \
         || (device->issu_in_progress == 1))      \
      {                                           \
          return KAPS_OK;                          \
      }                                           \

/*
 * Reading from device is always allowed, as long
 * as XPT is valid
 */
#define KAPS_DM_KAPS_CHECK_XPT_READ(device, i)     \
      device->num_of_piords += i;                 \
      if (device->xpt == NULL)                    \
          return KAPS_OK;                          \


kaps_status
kaps_dm_kaps_init(
    struct kaps_device *device)
{
    struct kaps_xpt *xpt;
    uint8_t reg_data[KAPS_REGISTER_WIDTH_8];

    if (!device)
        return KAPS_INVALID_ARGUMENT;

    xpt = device->xpt;

    if (!xpt)
    {
        device->id = KAPS_JERICHO_2_DEVICE_ID;
        device->silicon_sub_type = KAPS_JERICHO_2_SUB_TYPE_THREE_LEVEL;
        return KAPS_OK;
    }

    KAPS_STRY(xpt->kaps_register_read(xpt, KAPS_REVISION_REGISTER_ADDR, KAPS_REGISTER_WIDTH_8, reg_data));

    device->id = KapsReadBitsInArrray(reg_data, KAPS_REGISTER_WIDTH_8, 31, 16);


    device->silicon_sub_type = KapsReadBitsInArrray(reg_data, KAPS_REGISTER_WIDTH_8, 15, 0);

    return KAPS_OK;
}


kaps_status
kaps_dm_kaps_reg_write(
    struct kaps_device * device,
    uint32_t blk_num,
    uint32_t reg_num,
    uint32_t nbytes,
    uint8_t * data)
{
    struct kaps_xpt *xpt;

    if (!device || !data)
        return KAPS_INVALID_ARGUMENT;

    KAPS_DM_KAPS_CHECK_XPT_WRITE(device, 1);

    xpt = device->xpt;

    KAPS_STRY(xpt->kaps_command(xpt, KAPS_CMD_WRITE, KAPS_FUNC0, device->dba_offset + blk_num, reg_num, nbytes, data));

    return KAPS_OK;
}

kaps_status
kaps_dm_kaps_reg_read(
    struct kaps_device * device,
    uint32_t blk_num,
    uint32_t reg_num,
    uint32_t nbytes,
    uint8_t * data)
{
    struct kaps_xpt *xpt;

    if (!device || !data)
        return KAPS_INVALID_ARGUMENT;

    KAPS_DM_KAPS_CHECK_XPT_READ(device, 1);

    xpt = device->xpt;

    KAPS_STRY(xpt->kaps_command(xpt, KAPS_CMD_READ, KAPS_FUNC0, device->dba_offset + blk_num, reg_num, nbytes, data));

    return KAPS_OK;
}

kaps_status
kaps_dm_kaps_ad_write(
    struct kaps_device * device,
    uint32_t ab_num,
    uint32_t ad_addr,
    uint32_t nbytes,
    uint8_t * data)
{
    struct kaps_xpt *xpt;

    if (!device || !data)
        return KAPS_INVALID_ARGUMENT;

    KAPS_DM_KAPS_CHECK_XPT_WRITE(device, 1);

    xpt = device->xpt;

    KAPS_STRY(xpt->kaps_command(xpt, KAPS_CMD_WRITE, KAPS_FUNC4, device->dba_offset + ab_num, ad_addr, nbytes, data));

    return KAPS_OK;
}

kaps_status
kaps_dm_kaps_ad_read(
    struct kaps_device * device,
    uint32_t ab_num,
    uint32_t ad_addr,
    uint32_t nbytes,
    uint8_t * o_data)
{
    struct kaps_xpt *xpt;

    if (!device || !o_data)
        return KAPS_INVALID_ARGUMENT;

    KAPS_DM_KAPS_CHECK_XPT_READ(device, 1);

    xpt = device->xpt;

    KAPS_STRY(xpt->kaps_command(xpt, KAPS_CMD_READ, KAPS_FUNC4, device->dba_offset + ab_num, ad_addr, nbytes, o_data));

    return KAPS_OK;
}

kaps_status
kaps_dm_kaps_search(
    struct kaps_device * device,
    uint8_t * key,
    enum kaps_search_interface search_interface,
    struct kaps_search_result * kaps_result)
{
    struct kaps_xpt *xpt;

    if (!device || !key || !kaps_result)
        return KAPS_INVALID_ARGUMENT;

    if (device->xpt == NULL)
        return KAPS_OK;

    xpt = device->xpt;

    KAPS_STRY(xpt->kaps_search(xpt, key, search_interface, kaps_result));

    return KAPS_OK;
}

static kaps_status
get_hb_blk_info(
    struct kaps_device *device,
    struct kaps_db *db,
    uint32_t block_num,
    uint8_t * is_small_bb_hb,
    uint8_t * is_bb_hb,
    uint8_t * is_rpb_hb)
{
    uint32_t db_group_id = db->db_group_id;
        
    *is_small_bb_hb = 0;
    *is_bb_hb = 0;
    *is_rpb_hb = 0;

    if (device->hw_res->total_small_bb[db_group_id])
    {
        if (db->num_algo_levels_in_db == 2)
        {
            if (block_num >= device->small_bb_offset)
            {
                *is_small_bb_hb = 1;
            }
            else
            {
                *is_rpb_hb = 1;
            }

        }
        else
        {

            if (block_num >= device->uda_offset)
            {
                *is_bb_hb = 1;
            }
            else if (device->small_bb_offset <= block_num && block_num < device->uda_offset)
            {
                *is_small_bb_hb = 1;
            }
            else if (device->dba_offset <= block_num && block_num < (device->dba_offset + device->num_dba_sb))
            {
                *is_rpb_hb = 1;
            }
            else
            {
                kaps_assert(0, "Unable to determine the hit bit block \n");
            }

        }

    }
    else
    {
        if (block_num >= device->uda_offset)
        {
            *is_bb_hb = 1;
        }
        else
        {
            *is_rpb_hb = 1;
        }
    }

    return KAPS_OK;

}

kaps_status
kaps_dm_kaps_hb_read(
    struct kaps_device * device,
    struct kaps_db *db, 
    uint32_t block_num,
    uint32_t row_num,
    uint8_t * data)
{
    struct kaps_xpt *xpt;
    uint8_t is_small_bb_hb = 0, is_bb_hb = 0, is_rpb_hb = 0;

    if (!device || !data)
        return KAPS_INVALID_ARGUMENT;

    KAPS_DM_KAPS_CHECK_XPT_WRITE(device, 1);

    xpt = device->xpt;

    get_hb_blk_info(device, db, block_num, &is_small_bb_hb, &is_bb_hb, &is_rpb_hb);

    if (is_bb_hb)
    {
        KAPS_STRY(xpt->kaps_hb_read(xpt, block_num, row_num, data));

    }
    else if (is_small_bb_hb)
    {
        KAPS_STRY(xpt->kaps_hb_read(xpt, block_num, row_num, data));
    }
    else if (is_rpb_hb)
    {

        uint32_t hb_row = row_num / KAPS_HB_ROW_WIDTH_1;
        uint32_t bit_pos = row_num % KAPS_HB_ROW_WIDTH_1;

        KAPS_STRY(xpt->kaps_hb_read(xpt, block_num, hb_row, data));

        if (KapsReadBitsInArrray(data, KAPS_HB_ROW_WIDTH_8, bit_pos, bit_pos))
        {
            data[0] |= 1u << 7;
        }
        else
        {
            data[0] &= ~(1u << 7);
        }
    }
    else
    {
        kaps_assert(0, "Incorrect block specified during Hit bit read\n");
    }

    return KAPS_OK;
}

kaps_status
kaps_dm_kaps_hb_write(
    struct kaps_device * device,
    struct kaps_db *db,
    uint32_t block_num,
    uint32_t row_num,
    uint8_t * data)
{
    struct kaps_xpt *xpt;
    uint8_t is_small_bb_hb = 0, is_bb_hb = 0, is_rpb_hb = 0;

    if (!device || !data)
        return KAPS_INVALID_ARGUMENT;

    KAPS_DM_KAPS_CHECK_XPT_WRITE(device, 1);

    xpt = device->xpt;

    get_hb_blk_info(device, db, block_num, &is_small_bb_hb, &is_bb_hb, &is_rpb_hb);

    if (is_bb_hb)
    {
        KAPS_STRY(xpt->kaps_hb_write(xpt, block_num, row_num, data));

    }
    else if (is_small_bb_hb)
    {
        KAPS_STRY(xpt->kaps_hb_write(xpt, block_num, row_num, data));
    }
    else if (is_rpb_hb)
    {
        uint32_t hb_row = row_num / KAPS_HB_ROW_WIDTH_1;
        uint32_t bit_pos = row_num % KAPS_HB_ROW_WIDTH_1;
        uint8_t buffer[KAPS_HB_ROW_WIDTH_8];

        KAPS_STRY(xpt->kaps_hb_read(xpt, block_num, hb_row, buffer));

        KapsWriteBitsInArray(buffer, KAPS_HB_ROW_WIDTH_8, bit_pos, bit_pos, data[0] >> 7);

        KAPS_STRY(xpt->kaps_hb_write(xpt, block_num, hb_row, buffer));

    }
    else
    {
        kaps_assert(0, "Incorrect block specified during Hit bit write\n");
    }

    return KAPS_OK;

}

kaps_status
kaps_dm_kaps_hb_dump(
    struct kaps_device * device,
    struct kaps_db *db,
    uint32_t start_blk_num,
    uint32_t start_row_num,
    uint32_t end_blk_num,
    uint32_t end_row_num,
    uint8_t clear_on_read,
    uint8_t * data)
{
    struct kaps_xpt *xpt;

    if (!device || !data)
        return KAPS_INVALID_ARGUMENT;

    KAPS_DM_KAPS_CHECK_XPT_WRITE(device, 1);

    xpt = device->xpt;

    KAPS_STRY(xpt->kaps_hb_dump(xpt, start_blk_num, start_row_num, end_blk_num, end_row_num, clear_on_read, data));

    return KAPS_OK;
}

kaps_status
kaps_dm_kaps_hb_copy(
    struct kaps_device * device,
    struct kaps_db *db,
    uint32_t src_blk_num,
    uint32_t src_row_num,
    uint32_t dest_blk_num,
    uint32_t dest_row_num,
    uint16_t source_mask,
    uint8_t rotate_right,
    uint8_t perform_clear)
{
    struct kaps_xpt *xpt;
    uint8_t is_small_bb_hb = 0, is_bb_hb = 0, is_rpb_hb = 0;

    if (!device)
        return KAPS_INVALID_ARGUMENT;

    KAPS_DM_KAPS_CHECK_XPT_WRITE(device, 1);

    xpt = device->xpt;

    get_hb_blk_info(device, db, src_blk_num, &is_small_bb_hb, &is_bb_hb, &is_rpb_hb);

    if (xpt->kaps_hb_copy)
    {
        KAPS_STRY(xpt->kaps_hb_copy
                  (xpt, src_blk_num, src_row_num, dest_blk_num, dest_row_num, source_mask, rotate_right,
                   perform_clear));

    }
    else
    {
        uint8_t data[KAPS_HB_ROW_WIDTH_8];

        KAPS_STRY(xpt->kaps_hb_read(xpt, src_blk_num, src_row_num, data));
        KAPS_STRY(xpt->kaps_hb_write(xpt, dest_blk_num, dest_row_num, data));

    }

    return KAPS_OK;
}
