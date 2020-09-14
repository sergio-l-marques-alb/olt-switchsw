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

#include <stdio.h>

#include "kaps_algo_hw.h"
#include "kaps_bitmap.h"
#include "kaps_utility.h"
#include "kaps_ab.h"

static kaps_status
kaps_kaps_rit_write(
    struct kaps_device *device,
    struct kaps_ads *write_ads,
    uint32_t blk_num,
    uint32_t it_addr)
{
    kaps_status status = KAPS_OK;
    uint8_t write_buf[16] = { 0 };
    uint32_t cur_bit_pos, num_bytes;

    num_bytes = sizeof(struct kaps_ads);

    KapsWriteBitsInArray(write_buf, num_bytes, 19, 0, write_ads->bpm_ad);
    KapsWriteBitsInArray(write_buf, num_bytes, 27, 20, write_ads->bpm_len);
    KapsWriteBitsInArray(write_buf, num_bytes, 31, 28, write_ads->row_offset);

    cur_bit_pos = 32;

    KapsWriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_00);
    cur_bit_pos += 4;

    KapsWriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_01);
    cur_bit_pos += 4;

    KapsWriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_02);
    cur_bit_pos += 4;

    KapsWriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_03);
    cur_bit_pos += 4;

    KapsWriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_04);
    cur_bit_pos += 4;

    KapsWriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_05);
    cur_bit_pos += 4;

    KapsWriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_06);
    cur_bit_pos += 4;

    KapsWriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_07);
    cur_bit_pos += 4;

    KapsWriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_08);
    cur_bit_pos += 4;

    KapsWriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_09);
    cur_bit_pos += 4;

    KapsWriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_10);
    cur_bit_pos += 4;

    KapsWriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_11);
    cur_bit_pos += 4;

    KapsWriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_12);
    cur_bit_pos += 4;

    KapsWriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_13);
    cur_bit_pos += 4;

    KapsWriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_14);
    cur_bit_pos += 4;

    KapsWriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_15);
    cur_bit_pos += 4;

    KapsWriteBitsInArray(write_buf, num_bytes, 103, 96, write_ads->key_shift);
    KapsWriteBitsInArray(write_buf, num_bytes, 112, 104, write_ads->bkt_row);

    status = kaps_dm_kaps_kaps_rit_write(device, blk_num, it_addr, num_bytes, write_buf);

    return status;
}

kaps_status
kaps_algo_hw_write_rpt_data(
    struct kaps_device * device,
    uint8_t rpt_num,
    uint8_t * data,
    uint8_t * mask,
    uint16_t entry_nr,
    uint8_t is_xy)
{
    kaps_sassert(is_xy);
    KAPS_STRY(kaps_dm_kaps_rpb_write(device, rpt_num, entry_nr, data, mask, 3, is_xy));
    return KAPS_OK;

}

kaps_status
kaps_algo_hw_delete_rpt_data(
    struct kaps_device * device,
    uint8_t rpt_num,
    uint16_t entry_nr)
{
    uint8_t invalid_data[KAPS_RPB_WIDTH_8];

    kaps_memset(invalid_data, 0x0, device->hw_res->pc_width_8);
    KAPS_STRY(kaps_dm_kaps_rpb_write(device, rpt_num, entry_nr, invalid_data, invalid_data, 0, 1));

    return KAPS_OK;
}

kaps_status
kaps_algo_hw_write_pct(
    struct kaps_device * device,
    struct kaps_pct * pct,
    uint8_t pct_num,
    uint16_t entry_nr)
{
    KAPS_STRY(kaps_kaps_rit_write(device, &pct->u.kaps, pct_num, entry_nr));
    return KAPS_OK;

}
