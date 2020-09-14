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
        device->id = KAPS_DEFAULT_DEVICE_ID;
        return KAPS_OK;
    }

    KAPS_STRY(xpt->kaps_register_read(xpt, KAPS_REVISION_REGISTER_ADDR, KAPS_REGISTER_WIDTH_8, reg_data));

    device->id = KapsReadBitsInArrray(reg_data, KAPS_REGISTER_WIDTH_8, 31, 16);

#ifdef PRINT_DEV_INFO
    kaps_printf("\n   kaps_dm_kaps_init()");
    kaps_printf("\n   Rev Reg Data (31:0): %.2x%.2x_%.2x%.2x,   Device ID: %d (31:16)",
                reg_data[0], reg_data[1], reg_data[2], reg_data[3], device->id);
    switch (device->id)
    {
        case KAPS_DEFAULT_DEVICE_ID:
            kaps_printf(" KAPS_DEFAULT_DEVICE_ID \n\n");
            break;

        case KAPS_QUMRAN_DEVICE_ID:
            kaps_printf(" KAPS_QUMRAN_DEVICE_ID \n\n");
            break;

        case KAPS_JERICHO_PLUS_DEVICE_ID:
            kaps_printf(" KAPS_JERICHO_PLUS_DEVICE_ID \n\n");
            break;

        case KAPS_QUX_DEVICE_ID:
            kaps_printf(" KAPS_QUX_DEVICE_ID \n\n");
            break;

        case KAPS_JERICHO_2_DEVICE_ID:
            kaps_printf(" KAPS_JERICHO_2_DEVICE_ID \n\n");
            break;
        default:
            break;
    }
#endif

    if (device->id != KAPS_DEFAULT_DEVICE_ID && device->id != KAPS_QUMRAN_DEVICE_ID && device->id != KAPS_QUX_DEVICE_ID
        && device->id != KAPS_JERICHO_PLUS_DEVICE_ID && device->id != KAPS_JERICHO_2_DEVICE_ID)
        device->id = KAPS_DEFAULT_DEVICE_ID;

    device->silicon_sub_type = KapsReadBitsInArrray(reg_data, KAPS_REGISTER_WIDTH_8, 15, 0);

    return KAPS_OK;
}

kaps_status
kaps_dm_kaps_dba_write(
    struct kaps_device * device,
    uint32_t blk_num,
    uint32_t entry_nr,
    uint32_t nbytes,
    uint8_t * data,
    uint8_t * mask,
    uint8_t is_xy,
    uint8_t valid_bit)
{
    struct kaps_dba_entry data_x, data_y;
    struct kaps_xpt *xpt;
    uint32_t i;
    uint8_t write_buf[KAPS_DBA_WIDTH_8 + 1];

    if (!device || !data || !mask)
        return KAPS_INVALID_ARGUMENT;

    KAPS_DM_KAPS_CHECK_XPT_WRITE(device, 2);

    xpt = device->xpt;

    switch (valid_bit)
    {
        case 0:
            kaps_memset(write_buf, 0, sizeof(write_buf));

            if (device->id == KAPS_JERICHO_2_DEVICE_ID)
            {
                /*
                 * Write Enable is set to 0x3
                 */
                KapsWriteBitsInArray(write_buf, KAPS_DBA_WIDTH_8 + 1, 167, 166, 0x3);
            }

            KAPS_STRY(xpt->kaps_command(xpt, KAPS_CMD_WRITE, KAPS_FUNC1, device->dba_offset + blk_num, 2 * entry_nr,
                                        KAPS_DBA_WIDTH_8 + 1, write_buf));
            KAPS_STRY(xpt->kaps_command(xpt, KAPS_CMD_WRITE, KAPS_FUNC1, device->dba_offset + blk_num, 2 * entry_nr + 1,
                                        KAPS_DBA_WIDTH_8 + 1, write_buf));
            break;
        case 1:
            kaps_sassert(valid_bit);
            break;
        case 2:
            kaps_sassert(valid_bit);
            break;
        case 3:
            kaps_sassert(!is_xy);
            kaps_convert_dm_to_xy(data, mask, data_x.key, data_y.key, nbytes);
            data_x.is_valid = 0x3;
            data_y.is_valid = 0x3;
            data_x.resv = 0;
            data_y.resv = 0;
            kaps_memset(write_buf, 0, sizeof(write_buf));

            if (device->id == KAPS_JERICHO_2_DEVICE_ID)
            {
                for (i = 0; i < KAPS_DBA_WIDTH_8; i++)
                {
                    KapsWriteBitsInArray(write_buf, KAPS_DBA_WIDTH_8 + 1, ((KAPS_DBA_WIDTH_8 - i) * 8) + 3,
                                     ((KAPS_DBA_WIDTH_8 - i) * 8) - 4, data_y.key[i]);
                }

                KapsWriteBitsInArray(write_buf, KAPS_DBA_WIDTH_8 + 1, 3, 0, data_y.resv);
                KapsWriteBitsInArray(write_buf, KAPS_DBA_WIDTH_8 + 1, 165, 164, data_y.is_valid);
                KapsWriteBitsInArray(write_buf, KAPS_DBA_WIDTH_8 + 1, 167, 166, 0x3);       /* Write Enable is set to 0x3 */
            }
            else
            {
                for (i = 0; i < KAPS_DBA_WIDTH_8; i++)
                {
                    KapsWriteBitsInArray(write_buf, KAPS_DBA_WIDTH_8 + 1, ((KAPS_DBA_WIDTH_8 - i) * 8) + 5,
                                     ((KAPS_DBA_WIDTH_8 - i) * 8) - 2, data_y.key[i]);
                }
                KapsWriteBitsInArray(write_buf, KAPS_DBA_WIDTH_8 + 1, 5, 2, data_y.resv);
                KapsWriteBitsInArray(write_buf, KAPS_DBA_WIDTH_8 + 1, 1, 0, data_y.is_valid);
                KapsWriteBitsInArray(write_buf, KAPS_DBA_WIDTH_8 + 1, 167, 166, 0);
            }

            KAPS_STRY(xpt->kaps_command(xpt, KAPS_CMD_WRITE, KAPS_FUNC1, device->dba_offset + blk_num, 2 * entry_nr + 1,
                                        KAPS_DBA_WIDTH_8 + 1, write_buf));

            kaps_memset(write_buf, 0, sizeof(write_buf));

            if (device->id == KAPS_JERICHO_2_DEVICE_ID)
            {
                for (i = 0; i < KAPS_DBA_WIDTH_8; i++)
                {
                    KapsWriteBitsInArray(write_buf, KAPS_DBA_WIDTH_8 + 1, ((KAPS_DBA_WIDTH_8 - i) * 8) + 3,
                                     ((KAPS_DBA_WIDTH_8 - i) * 8) - 4, data_x.key[i]);
                }

                KapsWriteBitsInArray(write_buf, KAPS_DBA_WIDTH_8 + 1, 3, 0, data_x.resv);
                KapsWriteBitsInArray(write_buf, KAPS_DBA_WIDTH_8 + 1, 165, 164, data_x.is_valid);
                KapsWriteBitsInArray(write_buf, KAPS_DBA_WIDTH_8 + 1, 167, 166, 0x3);       /* Write Enable is set to 0x3 */

            }
            else
            {
                for (i = 0; i < KAPS_DBA_WIDTH_8; i++)
                {
                    KapsWriteBitsInArray(write_buf, KAPS_DBA_WIDTH_8 + 1, ((KAPS_DBA_WIDTH_8 - i) * 8) + 5,
                                     ((KAPS_DBA_WIDTH_8 - i) * 8) - 2, data_x.key[i]);
                }
                KapsWriteBitsInArray(write_buf, KAPS_DBA_WIDTH_8 + 1, 5, 2, data_x.resv);
                KapsWriteBitsInArray(write_buf, KAPS_DBA_WIDTH_8 + 1, 1, 0, data_x.is_valid);
                KapsWriteBitsInArray(write_buf, KAPS_DBA_WIDTH_8 + 1, 167, 166, 0);

                KAPS_STRY(xpt->kaps_command(xpt, KAPS_CMD_WRITE, KAPS_FUNC1, device->dba_offset + blk_num, 2 * entry_nr,
                                            KAPS_DBA_WIDTH_8 + 1, write_buf));
            }
            break;
        default:
            kaps_sassert(valid_bit);
            break;

    }
    return KAPS_OK;
}

kaps_status
kaps_dm_kaps_dba_read(
    struct kaps_device * device,
    uint32_t blk_num,
    uint32_t entry_nr,
    struct kaps_dba_entry * o_entry_x,
    struct kaps_dba_entry * o_entry_y)
{
    struct kaps_xpt *xpt;
    uint8_t read_buf[KAPS_DBA_WIDTH_8 + 1];
    uint32_t i;
    if (!device || !o_entry_x || !o_entry_y)
        return KAPS_INVALID_ARGUMENT;

    KAPS_DM_KAPS_CHECK_XPT_READ(device, 2);

    xpt = device->xpt;

    /*
     * Read y 
     */
    kaps_memset(read_buf, 0, sizeof(read_buf));
    KAPS_STRY(xpt->kaps_command(xpt, KAPS_CMD_READ, KAPS_FUNC1, device->dba_offset + blk_num, 2 * entry_nr + 1,
                                KAPS_DBA_WIDTH_8 + 1, read_buf));

    if (device->id == KAPS_JERICHO_2_DEVICE_ID)
    {
        for (i = 0; i < KAPS_DBA_WIDTH_8; i++)
        {
            o_entry_y->key[i] = KapsReadBitsInArrray(read_buf, KAPS_DBA_WIDTH_8 + 1, ((KAPS_DBA_WIDTH_8 - i) * 8) + 3,
                                                 ((KAPS_DBA_WIDTH_8 - i) * 8) - 4);
        }
        o_entry_y->resv = KapsReadBitsInArrray(read_buf, KAPS_DBA_WIDTH_8 + 1, 3, 0);
        o_entry_y->is_valid = KapsReadBitsInArrray(read_buf, KAPS_DBA_WIDTH_8 + 1, 165, 164);

    }
    else
    {
        for (i = 0; i < KAPS_DBA_WIDTH_8; i++)
        {
            o_entry_y->key[i] = KapsReadBitsInArrray(read_buf, KAPS_DBA_WIDTH_8 + 1, ((KAPS_DBA_WIDTH_8 - i) * 8) + 5,
                                                 ((KAPS_DBA_WIDTH_8 - i) * 8) - 2);
        }
        o_entry_y->resv = KapsReadBitsInArrray(read_buf, KAPS_DBA_WIDTH_8 + 1, 5, 2);
        o_entry_y->is_valid = KapsReadBitsInArrray(read_buf, KAPS_DBA_WIDTH_8 + 1, 1, 0);
        o_entry_y->is_valid = KapsReadBitsInArrray(read_buf, KAPS_DBA_WIDTH_8 + 1, 167, 166);
    }

    /*
     * Read x 
     */
    kaps_memset(read_buf, 0, sizeof(read_buf));
    KAPS_STRY(xpt->kaps_command(xpt, KAPS_CMD_READ, KAPS_FUNC1, device->dba_offset + blk_num, 2 * entry_nr,
                                KAPS_DBA_WIDTH_8 + 1, read_buf));

    if (device->id == KAPS_JERICHO_2_DEVICE_ID)
    {
        for (i = 0; i < KAPS_DBA_WIDTH_8; i++)
        {
            o_entry_x->key[i] = KapsReadBitsInArrray(read_buf, KAPS_DBA_WIDTH_8 + 1, ((KAPS_DBA_WIDTH_8 - i) * 8) + 3,
                                                 ((KAPS_DBA_WIDTH_8 - i) * 8) - 4);
        }
        o_entry_x->resv = KapsReadBitsInArrray(read_buf, KAPS_DBA_WIDTH_8 + 1, 3, 0);
        o_entry_x->is_valid = KapsReadBitsInArrray(read_buf, KAPS_DBA_WIDTH_8 + 1, 165, 164);

    }
    else
    {
        for (i = 0; i < KAPS_DBA_WIDTH_8; i++)
        {
            o_entry_x->key[i] = KapsReadBitsInArrray(read_buf, KAPS_DBA_WIDTH_8 + 1, ((KAPS_DBA_WIDTH_8 - i) * 8) + 5,
                                                 ((KAPS_DBA_WIDTH_8 - i) * 8) - 2);
        }
        o_entry_x->resv = KapsReadBitsInArrray(read_buf, KAPS_DBA_WIDTH_8 + 1, 5, 2);
        o_entry_x->is_valid = KapsReadBitsInArrray(read_buf, KAPS_DBA_WIDTH_8 + 1, 1, 0);
        o_entry_x->is_valid = KapsReadBitsInArrray(read_buf, KAPS_DBA_WIDTH_8 + 1, 167, 166);
    }

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
    *is_small_bb_hb = 0;
    *is_bb_hb = 0;
    *is_rpb_hb = 0;

    if (device->id == KAPS_JERICHO_2_DEVICE_ID)
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
    uint8_t temp_data_1[KAPS_HB_ROW_WIDTH_8] = { 0 };
    uint8_t temp_data_2[KAPS_HB_ROW_WIDTH_8] = { 0 };
    uint32_t i;
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
        if (device->is_small_bb_on_second_half)
        {

            KAPS_STRY(xpt->kaps_hb_read(xpt, block_num, row_num, temp_data_1));
            KAPS_STRY(xpt->kaps_hb_read(xpt, block_num + 16, row_num, temp_data_2));

            for (i = 0; i < KAPS_HB_ROW_WIDTH_8; ++i)
            {
                data[i] = temp_data_1[i] | temp_data_2[i];
            }

        }
        else
        {
            KAPS_STRY(xpt->kaps_hb_read(xpt, block_num, row_num, data));
        }
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

        if (device->is_small_bb_on_second_half)
        {
            KAPS_STRY(xpt->kaps_hb_write(xpt, block_num + 16, row_num, data));
        }

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

        if (is_small_bb_hb && device->is_small_bb_on_second_half)
        {
            KAPS_STRY(xpt->kaps_hb_copy
                      (xpt, src_blk_num + 16, src_row_num, dest_blk_num + 16, dest_row_num, source_mask, rotate_right,
                       perform_clear));
        }

    }
    else
    {
        uint8_t data[KAPS_HB_ROW_WIDTH_8];

        KAPS_STRY(xpt->kaps_hb_read(xpt, src_blk_num, src_row_num, data));
        KAPS_STRY(xpt->kaps_hb_write(xpt, dest_blk_num, dest_row_num, data));

        if (is_small_bb_hb && device->is_small_bb_on_second_half)
        {
            KAPS_STRY(xpt->kaps_hb_read(xpt, src_blk_num + 16, src_row_num, data));
            KAPS_STRY(xpt->kaps_hb_write(xpt, dest_blk_num + 16, dest_row_num, data));
        }
    }

    return KAPS_OK;
}
