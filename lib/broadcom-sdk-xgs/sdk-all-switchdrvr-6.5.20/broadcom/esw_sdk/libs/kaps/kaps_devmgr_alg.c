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

#include <stdint.h>

#include "kaps_utility.h"
#include "kaps_device_internal.h"
#include "kaps_device_alg.h"
#include "kaps_algo_hw.h"
#include "kaps_lpm_algo.h"

/*
 * Writing to device is prevented if
 * XPT is null or ISSU is in progress
 */

#define KAPS_DM_KAPS_CHECK_XPT_WRITE(device, i)  \
    device->num_of_piowrs += i;                 \
    if ((device->xpt == NULL)                    \
       || (device->issu_in_progress == 1))      \
    {                                           \
        return KAPS_OK;                          \
    }

/*
 * Reading from device is always allowed, as long
 * as XPT is valid
 */

#define KAPS_DM_KAPS_CHECK_XPT_READ(device, i)   \
        device->num_of_piords += i;             \
        if (device->xpt == NULL)                 \
        {                                       \
            return KAPS_OK;                      \
        }




kaps_status
kaps_dm_kaps_dma_bb_write(
    struct kaps_device *device,
    uint32_t bb_num,
    uint32_t offset,
    uint32_t length,
    uint8_t * data)
{
    struct kaps_xpt *xpt;

    if (!device || !data)
        return KAPS_INVALID_ARGUMENT;

    KAPS_DM_KAPS_CHECK_XPT_WRITE(device, 1);

    xpt = device->xpt;

    KAPS_STRY(xpt->kaps_command(xpt, KAPS_CMD_WRITE, KAPS_FUNC5, device->uda_offset + bb_num, offset, length, data));

    return KAPS_OK;
}

kaps_status
kaps_dm_kaps_dma_bb_read(
    struct kaps_device * device,
    uint32_t bb_num,
    uint32_t row_num,
    uint32_t length,
    uint8_t * o_data)
{
    struct kaps_xpt *xpt;

    if (!device || !o_data)
        return KAPS_INVALID_ARGUMENT;

    KAPS_DM_KAPS_CHECK_XPT_READ(device, 1);

    xpt = device->xpt;

    KAPS_STRY(xpt->kaps_command(xpt, KAPS_CMD_READ, KAPS_FUNC5, device->uda_offset + bb_num, row_num, length, o_data));

    return KAPS_OK;
}

kaps_status
kaps_dm_kaps_alg_reg_write(
    struct kaps_device * device,
    uint32_t bb_index,
    uint32_t reg_num,
    uint32_t length,
    uint8_t * register_data)
{
    struct kaps_xpt *xpt;
    uint32_t offset;

    if (!device || !register_data)
        return KAPS_INVALID_ARGUMENT;

    KAPS_DM_KAPS_CHECK_XPT_WRITE(device, 1);

    xpt = device->xpt;

    offset = device->uda_offset;
    if (device->id == KAPS_JERICHO_2_DEVICE_ID)
        offset = device->small_bb_offset;

    KAPS_STRY(xpt->kaps_command(xpt, KAPS_CMD_WRITE, KAPS_FUNC0, bb_index + offset, reg_num, length, register_data));

    return KAPS_OK;
}

kaps_status
kaps_dm_kaps_alg_reg_read(
    struct kaps_device * device,
    uint32_t bb_index,
    uint32_t reg_num,
    uint32_t length,
    uint8_t * o_data)
{
    struct kaps_xpt *xpt;
    uint32_t offset;

    if (!device || !o_data)
        return KAPS_INVALID_ARGUMENT;

    KAPS_DM_KAPS_CHECK_XPT_READ(device, 1);

    xpt = device->xpt;

    offset = device->uda_offset;
    if (device->id == KAPS_JERICHO_2_DEVICE_ID)
        offset = device->small_bb_offset;

    KAPS_STRY(xpt->kaps_command(xpt, KAPS_CMD_READ, KAPS_FUNC0, bb_index + offset,
                                reg_num, length, o_data));

    return KAPS_OK;
}

kaps_status
kaps_dm_kaps_bb_write(
    struct kaps_device * device,
    struct kaps_db *db,
    uint32_t bb_num,
    uint32_t row_num,
    uint8_t is_final_level,
    uint32_t length,
    uint8_t * data)
{
    struct kaps_xpt *xpt;
    uint32_t offset;
    uint32_t hw_bb_num;
    uint32_t kaps_func;

    if (!device || !data)
        return KAPS_INVALID_ARGUMENT;

    KAPS_DM_KAPS_CHECK_XPT_WRITE(device, 1);

    xpt = device->xpt;

    offset = kaps_device_get_final_level_offset(device, db);
    
    hw_bb_num = bb_num;
    kaps_func = KAPS_FUNC2;

    if (db->num_algo_levels_in_db == 3)
    {
        if (!is_final_level)
            offset = device->small_bb_offset;
        else
        {
            hw_bb_num = bb_num / 2;

            if (bb_num % 2 == 1)
                kaps_func = KAPS_FUNC10;
        }
    }

    KAPS_STRY(xpt->kaps_command(xpt, KAPS_CMD_WRITE, kaps_func, offset + hw_bb_num, row_num, length, data));

    return KAPS_OK;
}

kaps_status
kaps_dm_kaps_bb_read(
    struct kaps_device * device,
    uint32_t bb_num,
    uint32_t row_num,
    uint32_t length,
    uint8_t * o_data)
{
    struct kaps_xpt *xpt;

    if (!device || !o_data)
        return KAPS_INVALID_ARGUMENT;

    KAPS_DM_KAPS_CHECK_XPT_READ(device, 1);

    xpt = device->xpt;

    KAPS_STRY(xpt->kaps_command(xpt, KAPS_CMD_READ, KAPS_FUNC2, device->uda_offset + bb_num, row_num, length, o_data));

    return KAPS_OK;
}

kaps_status
kaps_dm_kaps_kaps_rit_write(
    struct kaps_device * device,
    uint32_t blk_num,
    uint32_t rit_addr,
    uint32_t length,
    uint8_t * data)
{
    struct kaps_xpt *xpt;

    if (!device || !data)
        return KAPS_INVALID_ARGUMENT;

    KAPS_DM_KAPS_CHECK_XPT_WRITE(device, 1);

    if (device->kaps_shadow && device->kaps_shadow->ads_blks)
    {
        struct kaps_ads *write_ads = &device->kaps_shadow->ads_blks[blk_num].ads_rows[rit_addr];

        write_ads->bpm_ad = KapsReadBitsInArrray(data, 16, 19, 0);
        write_ads->bpm_len = KapsReadBitsInArrray(data, 16, 27, 20);
        write_ads->row_offset = KapsReadBitsInArrray(data, 16, 31, 28);

        write_ads->format_map_00 = KapsReadBitsInArrray(data, 16, 35, 32);
        write_ads->format_map_01 = KapsReadBitsInArrray(data, 16, 39, 36);
        write_ads->format_map_02 = KapsReadBitsInArrray(data, 16, 43, 40);
        write_ads->format_map_03 = KapsReadBitsInArrray(data, 16, 47, 44);
        write_ads->format_map_04 = KapsReadBitsInArrray(data, 16, 51, 48);
        write_ads->format_map_05 = KapsReadBitsInArrray(data, 16, 55, 52);
        write_ads->format_map_06 = KapsReadBitsInArrray(data, 16, 59, 56);
        write_ads->format_map_07 = KapsReadBitsInArrray(data, 16, 63, 60);
        write_ads->format_map_08 = KapsReadBitsInArrray(data, 16, 67, 64);
        write_ads->format_map_09 = KapsReadBitsInArrray(data, 16, 71, 68);
        write_ads->format_map_10 = KapsReadBitsInArrray(data, 16, 75, 72);
        write_ads->format_map_11 = KapsReadBitsInArrray(data, 16, 79, 76);
        write_ads->format_map_12 = KapsReadBitsInArrray(data, 16, 83, 80);
        write_ads->format_map_13 = KapsReadBitsInArrray(data, 16, 87, 84);
        write_ads->format_map_14 = KapsReadBitsInArrray(data, 16, 91, 88);
        write_ads->format_map_15 = KapsReadBitsInArrray(data, 16, 95, 92);

        write_ads->key_shift = KapsReadBitsInArrray(data, 16, 103, 96);
        write_ads->bkt_row = KapsReadBitsInArrray(data, 16, 112, 104);

    }

    xpt = device->xpt;

    KAPS_STRY(xpt->kaps_command(xpt, KAPS_CMD_WRITE, KAPS_FUNC4, device->dba_offset + blk_num, rit_addr, length, data));

    return KAPS_OK;
}

kaps_status
kaps_dm_kaps_kaps_iit_write(
    struct kaps_device * device,
    struct kaps_db *db,
    uint32_t blk_num,
    uint32_t iit_addr,
    uint32_t length,
    uint8_t * data)
{
    struct kaps_xpt *xpt;

    if (!device || !data)
        return KAPS_INVALID_ARGUMENT;

    KAPS_DM_KAPS_CHECK_XPT_WRITE(device, 1);

    if (device->kaps_shadow )
    {
        struct kaps_ads *write_ads;

        if (device->id == KAPS_JERICHO_2_DEVICE_ID)
        {
            if (db->num_algo_levels_in_db == 2)
            {
                write_ads = &device->kaps_shadow->ads_blks[blk_num].ads_rows[iit_addr];
            }
            else
            {
                write_ads = &device->kaps_shadow->ads2_blks[blk_num].ads_rows[iit_addr];
            }
        }
        else
        {
            write_ads = &device->kaps_shadow->ads_blks[blk_num].ads_rows[iit_addr];
        }

        write_ads->bpm_ad = KapsReadBitsInArrray(data, 16, 19, 0);
        write_ads->bpm_len = KapsReadBitsInArrray(data, 16, 27, 20);
        write_ads->row_offset = KapsReadBitsInArrray(data, 16, 31, 28);

        write_ads->format_map_00 = KapsReadBitsInArrray(data, 16, 35, 32);
        write_ads->format_map_01 = KapsReadBitsInArrray(data, 16, 39, 36);
        write_ads->format_map_02 = KapsReadBitsInArrray(data, 16, 43, 40);
        write_ads->format_map_03 = KapsReadBitsInArrray(data, 16, 47, 44);
        write_ads->format_map_04 = KapsReadBitsInArrray(data, 16, 51, 48);
        write_ads->format_map_05 = KapsReadBitsInArrray(data, 16, 55, 52);
        write_ads->format_map_06 = KapsReadBitsInArrray(data, 16, 59, 56);
        write_ads->format_map_07 = KapsReadBitsInArrray(data, 16, 63, 60);
        write_ads->format_map_08 = KapsReadBitsInArrray(data, 16, 67, 64);
        write_ads->format_map_09 = KapsReadBitsInArrray(data, 16, 71, 68);
        write_ads->format_map_10 = KapsReadBitsInArrray(data, 16, 75, 72);
        write_ads->format_map_11 = KapsReadBitsInArrray(data, 16, 79, 76);
        write_ads->format_map_12 = KapsReadBitsInArrray(data, 16, 83, 80);
        write_ads->format_map_13 = KapsReadBitsInArrray(data, 16, 87, 84);
        write_ads->format_map_14 = KapsReadBitsInArrray(data, 16, 91, 88);
        write_ads->format_map_15 = KapsReadBitsInArrray(data, 16, 95, 92);

        if (device->id == KAPS_JERICHO_2_DEVICE_ID)
        {
            write_ads->key_shift = KapsReadBitsInArrray(data, 16, 103, 96);

            if (db->num_algo_levels_in_db == 2)
            {
                write_ads->bkt_row = KapsReadBitsInArrray(data, 16, 112, 104);
            }
            else
            {
                write_ads->bkt_row = KapsReadBitsInArrray(data, 16, 117, 104);
            }
        }
        else
        {
            write_ads->bkt_row = KapsReadBitsInArrray(data, 16, 108, 96);
            write_ads->reserved = KapsReadBitsInArrray(data, 16, 111, 109);
            write_ads->key_shift = KapsReadBitsInArrray(data, 16, 119, 112);
            write_ads->ecc = KapsReadBitsInArrray(data, 16, 127, 120);
        }
    }

    xpt = device->xpt;

    if (db->num_algo_levels_in_db == 2)
    {
        KAPS_STRY(xpt->kaps_command(xpt, KAPS_CMD_WRITE, KAPS_FUNC4, device->dba_offset + blk_num, iit_addr,
                                    length, data));
    }
    else
    {
        KAPS_STRY(xpt->kaps_command(xpt, KAPS_CMD_WRITE, KAPS_FUNC16, KAPS2_ADS2_BLOCK_START + blk_num, iit_addr,
                                    length, data));
    }

    return KAPS_OK;
}

kaps_status
kaps_dm_kaps_iit_read(
    struct kaps_device * device,
    uint32_t blk_num,
    uint32_t iit_addr,
    uint32_t length,
    uint8_t * o_data)
{
    struct kaps_xpt *xpt;

    if (!device || !o_data)
        return KAPS_INVALID_ARGUMENT;

    KAPS_DM_KAPS_CHECK_XPT_READ(device, 1);

    if (device->kaps_shadow && device->nv_ptr)
    {
        struct kaps_ads *write_ads;
        write_ads = &device->kaps_shadow->ads_blks[blk_num].ads_rows[iit_addr];
        KapsWriteBitsInArray(o_data, 16, 19, 0, write_ads->bpm_ad);
        KapsWriteBitsInArray(o_data, 16, 27, 20, write_ads->bpm_len);
        KapsWriteBitsInArray(o_data, 16, 31, 28, write_ads->row_offset);

        KapsWriteBitsInArray(o_data, 16, 35, 32, write_ads->format_map_00);
        KapsWriteBitsInArray(o_data, 16, 39, 36, write_ads->format_map_01);
        KapsWriteBitsInArray(o_data, 16, 43, 40, write_ads->format_map_02);
        KapsWriteBitsInArray(o_data, 16, 47, 44, write_ads->format_map_03);
        KapsWriteBitsInArray(o_data, 16, 51, 48, write_ads->format_map_04);
        KapsWriteBitsInArray(o_data, 16, 55, 52, write_ads->format_map_05);
        KapsWriteBitsInArray(o_data, 16, 59, 56, write_ads->format_map_06);
        KapsWriteBitsInArray(o_data, 16, 63, 60, write_ads->format_map_07);
        KapsWriteBitsInArray(o_data, 16, 67, 64, write_ads->format_map_08);
        KapsWriteBitsInArray(o_data, 16, 71, 68, write_ads->format_map_09);
        KapsWriteBitsInArray(o_data, 16, 75, 72, write_ads->format_map_10);
        KapsWriteBitsInArray(o_data, 16, 79, 76, write_ads->format_map_11);
        KapsWriteBitsInArray(o_data, 16, 83, 80, write_ads->format_map_12);
        KapsWriteBitsInArray(o_data, 16, 87, 84, write_ads->format_map_13);
        KapsWriteBitsInArray(o_data, 16, 91, 88, write_ads->format_map_14);
        KapsWriteBitsInArray(o_data, 16, 95, 92, write_ads->format_map_15);

        KapsWriteBitsInArray(o_data, 16, 108, 96, write_ads->bkt_row);
        KapsWriteBitsInArray(o_data, 16, 111, 109, write_ads->reserved);
        KapsWriteBitsInArray(o_data, 16, 119, 112, write_ads->key_shift);
        KapsWriteBitsInArray(o_data, 16, 127, 120, write_ads->ecc);
        return KAPS_OK;
    }

    xpt = device->xpt;

    KAPS_STRY(xpt->kaps_command(xpt, KAPS_CMD_READ, KAPS_FUNC4, device->dba_offset + blk_num, iit_addr,
                                length, o_data));

    return KAPS_OK;
}

kaps_status
kaps_dm_kaps_rpb_write(
    struct kaps_device * device,
    uint32_t blk_num,
    uint32_t row_num,
    uint8_t * data,
    uint8_t * mask,
    uint8_t valid_bit,
    uint8_t is_xy)
{
    struct kaps_xpt *xpt;
    struct kaps_dba_entry write_rpb_key0, write_rpb_key1;
    uint32_t i;
    uint8_t write_buf[2 * (KAPS_RPB_WIDTH_8 + 1)];
    uint32_t write_buf_len_8;

    if (!device || !data)
        return KAPS_INVALID_ARGUMENT;

    KAPS_DM_KAPS_CHECK_XPT_WRITE(device, 2);

    write_buf_len_8 = KAPS_RPB_WIDTH_8 + 1;
    if (device->id == KAPS_JERICHO_2_DEVICE_ID)
    {
        write_buf_len_8 = 2 * (KAPS_RPB_WIDTH_8 + 1);
    }

    if (device->kaps_shadow && device->kaps_shadow->rpb_blks)
    {
        /*Store the information in Data-Mask Format*/
        if (!is_xy)
        {
            kaps_memcpy(device->kaps_shadow->rpb_blks[blk_num].rpb_rows[row_num].data, data, KAPS_RPB_WIDTH_8);
            kaps_memcpy(device->kaps_shadow->rpb_blks[blk_num].rpb_rows[row_num].mask, mask, KAPS_RPB_WIDTH_8);
        }
        else
        {
            kaps_convert_xy_to_dm(data, mask, device->kaps_shadow->rpb_blks[blk_num].rpb_rows[row_num].data,
                             device->kaps_shadow->rpb_blks[blk_num].rpb_rows[row_num].mask, KAPS_RPB_WIDTH_8);
        }

        device->kaps_shadow->rpb_blks[blk_num].rpb_rows[row_num].rpb_valid_data = valid_bit;
        device->kaps_shadow->rpb_blks[blk_num].rpb_rows[row_num].rpb_valid_mask = valid_bit;
    }

    xpt = device->xpt;
    switch (valid_bit)
    {
        case 0:
            kaps_memset(write_buf, 0, sizeof(write_buf));

            if (device->id == KAPS_JERICHO_2_DEVICE_ID)
            {
                /*
                 * Write Enable is set to 0x3 for X and Y
                 */
                KapsWriteBitsInArray(write_buf, write_buf_len_8, 167, 166, 0x3);
                 /*X*/ KapsWriteBitsInArray(write_buf, write_buf_len_8, 168 + 167, 168 + 166, 0x3);
                 /*Y*/
                    KAPS_STRY(xpt->kaps_command
                              (xpt, KAPS_CMD_WRITE, KAPS_FUNC1, device->dba_offset + blk_num, 2 * row_num,
                               write_buf_len_8, write_buf));

            }
            else
            {

                KAPS_STRY(xpt->kaps_command(xpt, KAPS_CMD_WRITE, KAPS_FUNC1, device->dba_offset + blk_num, 2 * row_num,
                                            write_buf_len_8, write_buf));
                KAPS_STRY(xpt->kaps_command
                          (xpt, KAPS_CMD_WRITE, KAPS_FUNC1, device->dba_offset + blk_num, 2 * row_num + 1,
                           write_buf_len_8, write_buf));
            }
            break;
        case 1:
        case 2:
            kaps_sassert(valid_bit);
            break;
        case 3:
            if (!is_xy)
            {
                kaps_convert_dm_to_xy(data, mask, write_rpb_key0.key, write_rpb_key1.key, KAPS_RPB_WIDTH_8);
            }
            else
            {
                kaps_memcpy(write_rpb_key0.key, data, KAPS_RPB_WIDTH_8);
                kaps_memcpy(write_rpb_key1.key, mask, KAPS_RPB_WIDTH_8);
            }

            write_rpb_key0.is_valid = 0x3;
            write_rpb_key1.is_valid = 0x3;
            write_rpb_key0.resv = write_rpb_key0.pad = 0;
            write_rpb_key1.resv = write_rpb_key1.pad = 0;

            kaps_memset(write_buf, 0, sizeof(write_buf));

            if (device->id == KAPS_JERICHO_2_DEVICE_ID)
            {
                /*
                 * Copy X which goes to even row
                 */
                for (i = 0; i < KAPS_DBA_WIDTH_8; i++)
                {
                    KapsWriteBitsInArray(write_buf, write_buf_len_8, 168 + ((KAPS_RPB_WIDTH_8 - i) * 8) + 3,
                                     168 + ((KAPS_RPB_WIDTH_8 - i) * 8) - 4, write_rpb_key0.key[i]);
                }

                KapsWriteBitsInArray(write_buf, write_buf_len_8, 168 + 3, 168 + 0, write_rpb_key0.resv);
                KapsWriteBitsInArray(write_buf, write_buf_len_8, 168 + 165, 168 + 164, write_rpb_key0.is_valid);
                KapsWriteBitsInArray(write_buf, write_buf_len_8, 168 + 167, 168 + 166, 0x3);        /* Write Enable is set
                                                                                                 * to 0x3 */

                /*
                 * Copy Y which goes to odd row
                 */
                for (i = 0; i < KAPS_RPB_WIDTH_8; i++)
                {
                    KapsWriteBitsInArray(write_buf, write_buf_len_8, ((KAPS_RPB_WIDTH_8 - i) * 8) + 3,
                                     ((KAPS_RPB_WIDTH_8 - i) * 8) - 4, write_rpb_key1.key[i]);
                }

                KapsWriteBitsInArray(write_buf, write_buf_len_8, 3, 0, write_rpb_key1.resv);

                /*
                 * From Jericho2 onwards, X and Y share a common valid bit. So for coherency reasons, we set valid bit
                 * to 0 for the odd entry in the X-Y pair (the entry that we were writing first in the old code)
                 */
                KapsWriteBitsInArray(write_buf, write_buf_len_8, 165, 164, 0x0);
                KapsWriteBitsInArray(write_buf, write_buf_len_8, 167, 166, 0x3);    /* Write Enable is set to 0x3 */

                KAPS_STRY(xpt->kaps_command(xpt, KAPS_CMD_WRITE, KAPS_FUNC1, device->dba_offset + blk_num, 2 * row_num,
                                            write_buf_len_8, write_buf));

            }
            else
            {
                /*
                 * Older KAPS devices
                 */
                for (i = 0; i < KAPS_RPB_WIDTH_8; i++)
                {
                    KapsWriteBitsInArray(write_buf, KAPS_RPB_WIDTH_8 + 1, ((KAPS_RPB_WIDTH_8 - i) * 8) + 5,
                                     ((KAPS_RPB_WIDTH_8 - i) * 8) - 2, write_rpb_key1.key[i]);
                }
                KapsWriteBitsInArray(write_buf, KAPS_RPB_WIDTH_8 + 1, 5, 2, write_rpb_key1.resv);
                KapsWriteBitsInArray(write_buf, KAPS_RPB_WIDTH_8 + 1, 1, 0, write_rpb_key1.is_valid);
                KapsWriteBitsInArray(write_buf, KAPS_RPB_WIDTH_8 + 1, 167, 166, 0); /* as of now this is 0, check once it
                                                                                 * must same as bit 1:0 */

                KAPS_STRY(xpt->kaps_command
                          (xpt, KAPS_CMD_WRITE, KAPS_FUNC1, device->dba_offset + blk_num, 2 * row_num + 1,
                           KAPS_RPB_WIDTH_8 + 1, write_buf));

                kaps_memset(write_buf, 0, sizeof(write_buf));

                for (i = 0; i < KAPS_DBA_WIDTH_8; i++)
                {
                    KapsWriteBitsInArray(write_buf, KAPS_RPB_WIDTH_8 + 1, ((KAPS_RPB_WIDTH_8 - i) * 8) + 5,
                                     ((KAPS_RPB_WIDTH_8 - i) * 8) - 2, write_rpb_key0.key[i]);
                }
                KapsWriteBitsInArray(write_buf, KAPS_RPB_WIDTH_8 + 1, 5, 2, write_rpb_key0.resv);
                KapsWriteBitsInArray(write_buf, KAPS_RPB_WIDTH_8 + 1, 1, 0, write_rpb_key0.is_valid);
                KapsWriteBitsInArray(write_buf, KAPS_RPB_WIDTH_8 + 1, 167, 166, 0); /* as of now this is 0, check once it
                                                                                 * must same as bit 1:0 */

                KAPS_STRY(xpt->kaps_command(xpt, KAPS_CMD_WRITE, KAPS_FUNC1, device->dba_offset + blk_num, 2 * row_num,
                                            KAPS_RPB_WIDTH_8 + 1, write_buf));
            }

            break;
        default:
            kaps_sassert(valid_bit);
            break;
    }

    return KAPS_OK;
}

kaps_status
kaps_dm_kaps_rpb_read(
    struct kaps_device * device,
    uint32_t blk_num,
    uint32_t row_num,
    struct kaps_dba_entry * o_entry_x,
    struct kaps_dba_entry * o_entry_y)
{
    struct kaps_xpt *xpt;
    uint8_t read_buf[KAPS_DBA_WIDTH_8 + 1];
    uint32_t i;

    if (!device || !o_entry_x || !o_entry_y)
        return KAPS_INVALID_ARGUMENT;

    KAPS_DM_KAPS_CHECK_XPT_READ(device, 2);

    if (device->kaps_shadow && device->nv_ptr)
    {
        kaps_memcpy(o_entry_x->key, device->kaps_shadow->rpb_blks[blk_num].rpb_rows[row_num].data, KAPS_RPB_WIDTH_8);
        kaps_memcpy(o_entry_y->key, device->kaps_shadow->rpb_blks[blk_num].rpb_rows[row_num].mask, KAPS_RPB_WIDTH_8);
        o_entry_x->is_valid = device->kaps_shadow->rpb_blks[blk_num].rpb_rows[row_num].rpb_valid_data;
        o_entry_y->is_valid = device->kaps_shadow->rpb_blks[blk_num].rpb_rows[row_num].rpb_valid_mask;
        return KAPS_OK;
    }

    

    xpt = device->xpt;

    /*
     * Read y
     */
    kaps_memset(read_buf, 0, sizeof(read_buf));
    KAPS_STRY(xpt->kaps_command(xpt, KAPS_CMD_READ, KAPS_FUNC1, device->dba_offset + blk_num, row_num + 1,
                                KAPS_RPB_WIDTH_8 + 1, read_buf));

    if (device->id == KAPS_JERICHO_2_DEVICE_ID)
    {
        for (i = 0; i < KAPS_DBA_WIDTH_8; i++)
        {
            o_entry_y->key[i] = KapsReadBitsInArrray(read_buf, KAPS_RPB_WIDTH_8 + 1, ((KAPS_RPB_WIDTH_8 - i) * 8) + 3,
                                                 ((KAPS_RPB_WIDTH_8 - i) * 8) - 4);
        }

        o_entry_y->resv = KapsReadBitsInArrray(read_buf, KAPS_RPB_WIDTH_8 + 1, 3, 0);
        o_entry_y->is_valid = KapsReadBitsInArrray(read_buf, KAPS_RPB_WIDTH_8 + 1, 165, 164);

    }
    else
    {
        for (i = 0; i < KAPS_DBA_WIDTH_8; i++)
        {
            o_entry_y->key[i] = KapsReadBitsInArrray(read_buf, KAPS_RPB_WIDTH_8 + 1, ((KAPS_RPB_WIDTH_8 - i) * 8) + 5,
                                                 ((KAPS_RPB_WIDTH_8 - i) * 8) - 2);
        }
        o_entry_y->resv = KapsReadBitsInArrray(read_buf, KAPS_RPB_WIDTH_8 + 1, 5, 2);
        o_entry_y->is_valid = KapsReadBitsInArrray(read_buf, KAPS_RPB_WIDTH_8 + 1, 1, 0);

    }

    /*
     * Read x
     */
    kaps_memset(read_buf, 0, sizeof(read_buf));
    KAPS_STRY(xpt->kaps_command(xpt, KAPS_CMD_READ, KAPS_FUNC1, device->dba_offset + blk_num, row_num,
                                KAPS_RPB_WIDTH_8 + 1, read_buf));

    if (device->id == KAPS_JERICHO_2_DEVICE_ID)
    {
        for (i = 0; i < KAPS_DBA_WIDTH_8; i++)
        {
            o_entry_x->key[i] = KapsReadBitsInArrray(read_buf, KAPS_RPB_WIDTH_8 + 1, ((KAPS_RPB_WIDTH_8 - i) * 8) + 3,
                                                 ((KAPS_RPB_WIDTH_8 - i) * 8) - 4);
        }

        o_entry_x->resv = KapsReadBitsInArrray(read_buf, KAPS_RPB_WIDTH_8 + 1, 3, 0);
        o_entry_x->is_valid = KapsReadBitsInArrray(read_buf, KAPS_RPB_WIDTH_8 + 1, 165, 164);

    }
    else
    {
        for (i = 0; i < KAPS_DBA_WIDTH_8; i++)
        {
            o_entry_x->key[i] = KapsReadBitsInArrray(read_buf, KAPS_RPB_WIDTH_8 + 1, ((KAPS_RPB_WIDTH_8 - i) * 8) + 5,
                                                 ((KAPS_RPB_WIDTH_8 - i) * 8) - 2);
        }
        o_entry_x->resv = KapsReadBitsInArrray(read_buf, KAPS_RPB_WIDTH_8 + 1, 5, 2);
        o_entry_x->is_valid = KapsReadBitsInArrray(read_buf, KAPS_RPB_WIDTH_8 + 1, 1, 0);

    }

    return KAPS_OK;
}

kaps_status
kaps_dm_kaps_reset_blks(
    struct kaps_device * device,
    uint32_t blk_nr,
    uint32_t row_nr,
    uint32_t nbytes,
    uint8_t * data)
{
    struct kaps_xpt *xpt;

    if (!device)
        return KAPS_INVALID_ARGUMENT;

    if (device->xpt == NULL)
        return KAPS_OK;

    KAPS_DM_KAPS_CHECK_XPT_WRITE(device, 1);

    xpt = device->xpt;

    KAPS_STRY(xpt->kaps_command(xpt, KAPS_CMD_EXTENDED, KAPS_FUNC15, blk_nr, row_nr, nbytes, data));

    return KAPS_OK;
}

kaps_status
kaps_dm_kaps_enumerate_blks(
    struct kaps_device * device,
    uint32_t blk_nr,
    uint32_t row_nr,
    uint32_t nbytes,
    uint8_t * data)
{
    struct kaps_xpt *xpt;

    if (!device)
        return KAPS_INVALID_ARGUMENT;

    if (device->xpt == NULL)
        return KAPS_OK;

    KAPS_DM_KAPS_CHECK_XPT_WRITE(device, 1);

    xpt = device->xpt;

    KAPS_STRY(xpt->kaps_command(device->xpt, KAPS_CMD_EXTENDED, KAPS_FUNC14, blk_nr, row_nr, nbytes, data));

    return KAPS_OK;
}

kaps_status
kaps_dm_kaps_read_bb_config_options(
    struct kaps_device * device,
    uint32_t large_bb_nr,
    uint32_t * options)
{
    struct kaps_xpt *xpt;
    uint8_t read_buf[KAPS_REGISTER_WIDTH_8] = { 0 };

    if (!device)
        return KAPS_INVALID_ARGUMENT;

    KAPS_DM_KAPS_CHECK_XPT_READ(device, 1);

    xpt = device->xpt;

    xpt->kaps_command(xpt, KAPS_CMD_READ, KAPS_FUNC0, device->uda_offset + large_bb_nr, KAPS2_BIG_CONFIG_REG_ADDR,
                      KAPS_REGISTER_WIDTH_8, read_buf);

    *options = KapsReadBitsInArrray(read_buf, sizeof(read_buf), 31, 0);

    return KAPS_OK;
}

kaps_status
kaps_dm_kaps_write_bb_config_option(
    struct kaps_device * device,
    uint32_t large_bb_nr,
    uint32_t option)
{
    struct kaps_xpt *xpt;
    uint8_t write_buf[KAPS_REGISTER_WIDTH_8] = { 0 };

    if (!device)
        return KAPS_INVALID_ARGUMENT;

    KAPS_DM_KAPS_CHECK_XPT_WRITE(device, 1);

    xpt = device->xpt;

    KapsWriteBitsInArray(write_buf, sizeof(write_buf), 31, 0, option);

    xpt->kaps_command(xpt, KAPS_CMD_WRITE, KAPS_FUNC0, device->uda_offset + large_bb_nr, KAPS2_BIG_CONFIG_REG_ADDR,
                      KAPS_REGISTER_WIDTH_8, write_buf);

    return KAPS_OK;
}

kaps_status
kaps_dm_kaps_read_large_bb_num_rows(
    struct kaps_device * device,
    uint32_t large_bb_nr,
    uint32_t * bb_num_rows)
{
    struct kaps_xpt *xpt;
    uint8_t read_buf[KAPS_REGISTER_WIDTH_8] = { 0 };

    if (!device)
        return KAPS_INVALID_ARGUMENT;

    KAPS_DM_KAPS_CHECK_XPT_READ(device, 1);

    xpt = device->xpt;

    xpt->kaps_command(xpt, KAPS_CMD_READ, KAPS_FUNC0, device->uda_offset + large_bb_nr, KAPS2_BIG_BB_DEPTH_REG_ADDR,
                      KAPS_REGISTER_WIDTH_8, read_buf);

    *bb_num_rows = KapsReadBitsInArrray(read_buf, sizeof(read_buf), 31, 0);

    return KAPS_OK;
}

kaps_status
kaps_dm_kaps_read_ads_depth(
    struct kaps_device * device,
    uint32_t ads2_nr,
    uint32_t * depth)
{
    struct kaps_xpt *xpt;
    uint8_t read_buf[KAPS_REGISTER_WIDTH_8] = { 0 };

    if (!device)
        return KAPS_INVALID_ARGUMENT;

    KAPS_DM_KAPS_CHECK_XPT_READ(device, 1);

    xpt = device->xpt;

    xpt->kaps_command(xpt, KAPS_CMD_READ, KAPS_FUNC0, KAPS2_ADS2_BLOCK_START + ads2_nr, KAPS2_BIG_CONFIG_REG_ADDR,
                      KAPS_REGISTER_WIDTH_8, read_buf);

    *depth = KapsReadBitsInArrray(read_buf, sizeof(read_buf), 31, 0);

    return KAPS_OK;
}
