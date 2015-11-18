/* $Id: jer_pp_kaps_diag.c, hagayco Exp $
 * $Copyright: Copyright 2012 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 * 
*/

/*******************************************************************************
*
* Copyright 2011-2014 Broadcom Corporation
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
#include <soc/mcm/memregs.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_diag.h>

#define _ERR_MSG_MODULE_NAME BSL_SOC_TCAM

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)

#include <soc/dpp/JER/JER_PP/jer_pp_kaps_diag.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps.h>

#define PREFIX_LEN_8 (10)

#define GET_RAND_VAL(low, high)     (uint32_t) (low + ((float)(high - low  + 1) * (rand() /(RAND_MAX+1.0) )) )

static kbp_status write_bits_to_array(uint8_t* arr,
                                      uint32_t arr_sz,
                                      uint32_t end_pos,
                                      uint32_t st_pos,
                                      uint32_t value) {
    uint32_t startByteIdx;
    uint32_t endByteIdx;
    uint32_t byte;
    uint32_t len;
    uint8_t maskValue;

    if(end_pos < st_pos) {
        kbp_printf(" \n endbit cannot be less than startbit \n\n");
        exit(1);
    }
    if(end_pos >= (arr_sz << 3) ) {
        kbp_printf(" \n endBit exceeds the arr size \n\n");
        exit(1);
    }

    len = (end_pos - st_pos + 1);
    if(len > 32) {
        kbp_printf(" \n Can not write more than 32-bit at a time ! \n\n");
        exit(1);
    }

    /* Value is unsigned 32 bit variable, so it can not be greater than ~0.*/
    if(len != 32){
        if(value > ((uint32_t) (~(~0 << len))) )
            kbp_printf(" \n Value is to big to write in the bit field! \n\n");
    }

    startByteIdx = arr_sz - ((st_pos >> 3) + 1);
    endByteIdx = arr_sz- ((end_pos >> 3) + 1);

    if(startByteIdx == endByteIdx){
        maskValue = (uint8_t)(0xFE << ((end_pos  & 7)));
        maskValue |= ((1 << (st_pos  & 7)) - 1);
        arr[startByteIdx] &= maskValue;
        arr[startByteIdx] |= (uint8_t)((value << (st_pos  & 7)));
        return 0;
    }
    if(st_pos & 7) {
        maskValue = (uint8_t)((1 << (st_pos  & 7)) - 1);
        arr[startByteIdx] &= maskValue;
        arr[startByteIdx] |= (uint8_t)((value << (st_pos & 7)));
        startByteIdx--;
        value >>= (8 - (st_pos  & 7));
    }
    for(byte = startByteIdx; byte > endByteIdx; byte--){
        arr[byte] = (uint8_t)(value);
        value >>= 8;
    }
    maskValue = (uint8_t)(0xFE << ((end_pos & 7)));
    arr[byte] &= maskValue;
    arr[byte] |= value;

    return KBP_OK;
}


/* Convert the data and mask to XY format */
static void kaps_convert_dm_to_xy(uint8_t * data_d, uint8_t * data_m, uint8_t * data_x, uint8_t * data_y, uint32_t dataLenInBytes)
{
    uint32_t i;
    for (i = 0; i < dataLenInBytes; i++) {
        data_x[i] = data_d[i] & (~data_m[i]);
        data_y[i] = (~data_d[i]) & (~data_m[i]);
    }
}

/* Comparing the returned ads data with ads data which we written into ADS block */
static int kaps_compare_ads(struct kaps_ads w_ads, struct kaps_ads r_ads)
{

    if (w_ads.bpm_ad != r_ads.bpm_ad)
        return 1;

    if (w_ads.bpm_len != r_ads.bpm_len)
        return 1;

    if (w_ads.row_offset != r_ads.row_offset)
        return 1;

    if (w_ads.key_shift != r_ads.key_shift)
        return 1;

    if (w_ads.bkt_row != r_ads.bkt_row)
        return 1;

    if (w_ads.format_map_00 != r_ads.format_map_00)
        return 1;

    if (w_ads.format_map_03 != r_ads.format_map_03)
        return 1;

    if (w_ads.format_map_07 != r_ads.format_map_07)
        return 1;

    if (w_ads.format_map_11 != r_ads.format_map_11)
        return 1;

    if (w_ads.format_map_15 != r_ads.format_map_15)
        return 1;

    return 0;
}

static void kaps_construct_data(uint8_t * data, uint32_t leninbytes)
{
    uint32_t iter = 0;

    kbp_memset(data, 0, leninbytes);
    for (iter = 0; iter < leninbytes; iter++)
        data[iter] = GET_RAND_VAL(0, 256);
}

kbp_status kaps_diag_01(struct kaps_xpt *xpt)
{
    uint8_t data[KAPS_RPB_WIDTH_8];
    uint8_t mask[KAPS_RPB_WIDTH_8];
    uint8_t blk_nr, row_nr, nrows, iter, bkt_nr, i;
    kbp_status status = KBP_OK;
    uint32_t register_data;
    uint8_t func;

    kbp_printf("\n    -- KAPS DIAG TOOL --    \n\n");

    /* Reset the KAPS Blocks */
    xpt->kaps_command(xpt, KAPS_CMD_EXTENDED, KAPS_FUNC15, 0, 0, 0, NULL);

    /* Enumerate the KAPS Blocks */
    xpt->kaps_command(xpt, KAPS_CMD_EXTENDED, KAPS_FUNC14, 0, 0, 0, NULL);

    for (blk_nr = 0; blk_nr < KAPS_NUM_RPB_BLOCKS; blk_nr++) {

        KBP_STRY(xpt->kaps_command(xpt, KAPS_CMD_READ, KAPS_FUNC0, KAPS_RPB_BLOCK_START + blk_nr, 0x00000002, sizeof(register_data), (uint8_t *) &register_data ));
        write_bits_to_array((uint8_t *) &register_data, sizeof(register_data), 2, 2, 1);
        KBP_STRY(xpt->kaps_command(xpt, KAPS_CMD_WRITE, KAPS_FUNC0, KAPS_RPB_BLOCK_START + blk_nr, 0x00000002, sizeof(register_data), (uint8_t *) &register_data ));

        kbp_printf(" Write/Read to RPB Block #%d\n", blk_nr + 1);
        nrows = GET_RAND_VAL(0, KAPS_RPB_BLOCK_SIZE);
        for (iter = 0; iter < nrows; iter++) {

            struct kaps_write_rpb_rqt write_rpb_key0, write_rpb_key1;
            struct kaps_read_rpb_resp read_rpb_resp;
            uint8_t req_buffer[KAPS_RPB_WIDTH_8 + 1];
            uint8_t resp_buffer[KAPS_RPB_WIDTH_8 + 1];
            int32_t i;

            kbp_memset(mask, 0x00, KAPS_RPB_WIDTH_8);
            kbp_memset(data, 0x00, KAPS_RPB_WIDTH_8);
            row_nr = GET_RAND_VAL(0, KAPS_RPB_BLOCK_SIZE);
            kaps_construct_data(data, KAPS_RPB_WIDTH_8);

            /* Configure the write rpb request payload */
            func = KAPS_FUNC1;
            write_rpb_key0.is_valid = 0x3;
            write_rpb_key1.is_valid = 0x3;

            write_rpb_key0.search_lsn = 0;
            write_rpb_key1.search_lsn = 0;

            kaps_convert_dm_to_xy(data, mask, write_rpb_key0.key, write_rpb_key1.key, KAPS_RPB_WIDTH_8);

            for ( i = 0 ; i < KAPS_RPB_WIDTH_8; i++) {
                write_bits_to_array(req_buffer,KAPS_RPB_WIDTH_8 + 1, (i * 8) + 7, (i * 8), write_rpb_key1.key[i]);
            }
            write_bits_to_array(req_buffer,KAPS_RPB_WIDTH_8 + 1, 163, 160, write_rpb_key1.search_lsn);
            write_bits_to_array(req_buffer,KAPS_RPB_WIDTH_8 + 1, 165, 164, write_rpb_key1.is_valid);
            write_bits_to_array(req_buffer,KAPS_RPB_WIDTH_8 + 1, 167, 166, 0);
            KBP_STRY(xpt->kaps_command(xpt, KAPS_CMD_WRITE, func, KAPS_RPB_BLOCK_START + blk_nr, 2 * row_nr + 1, sizeof(write_rpb_key1), req_buffer));

            for ( i = 0 ; i < KAPS_RPB_WIDTH_8; i++) {
                write_bits_to_array(req_buffer,KAPS_RPB_WIDTH_8 + 1, (i * 8) + 7, (i * 8), write_rpb_key0.key[i]);
            }
            write_bits_to_array(req_buffer,KAPS_RPB_WIDTH_8 + 1, 163, 160, write_rpb_key0.search_lsn);
            write_bits_to_array(req_buffer,KAPS_RPB_WIDTH_8 + 1, 165, 164, write_rpb_key0.is_valid);
            write_bits_to_array(req_buffer,KAPS_RPB_WIDTH_8 + 1, 167, 166, 0);
            KBP_STRY(xpt->kaps_command(xpt, KAPS_CMD_WRITE, func, KAPS_RPB_BLOCK_START + blk_nr, 2 * row_nr, sizeof(write_rpb_key0), (uint8_t *) req_buffer));

            /* Configure the read rpb request payload */
            func = KAPS_FUNC1;

            KBP_STRY(xpt->kaps_command(xpt, KAPS_CMD_READ, func, KAPS_RPB_BLOCK_START + blk_nr, 2 * row_nr, sizeof(read_rpb_resp),
                                   (uint8_t *) &resp_buffer));
            if (kbp_memcmp(req_buffer, resp_buffer, KAPS_RPB_WIDTH_8) != 0) {
                kbp_printf("\tMis Match in data or valid bit is not set at Row Number: %d for RPB Blk #%d\n", row_nr,
                           blk_nr);
                return 1;
            }
        }
        kbp_printf("\tVerified #%d Rows\n", nrows);
    }
    kbp_printf(" RPB Block Testing is Passed \n\n");

    for (blk_nr = 0; blk_nr < KAPS_NUM_ADS_BLOCKS; blk_nr++) {

        kbp_printf(" Write/Read to ADS Block #%d\n", blk_nr + 1);
        nrows = GET_RAND_VAL(0, KAPS_ADS_NUW_ROWS);
        for (iter = 0; iter < nrows; iter++) {

            struct kaps_ads write_ads;
            struct kaps_ads read_ads_resp;
            uint8_t ad_value[KAPS_AD_WIDTH_8];
            row_nr = GET_RAND_VAL(0, KAPS_ADS_NUW_ROWS);

            /* Configure the write ads request payload */
            func = KAPS_FUNC4;
            write_ads.bkt_row = row_nr % KAPS_ADS_NUW_ROWS;
            write_ads.key_shift = 0;
            write_ads.row_offset = row_nr % KAPS_ADS_NUW_ROWS;
            write_ads.bpm_len = 0;
            write_ads.reserved = 0;
            write_ads.ecc = 0;

            kaps_construct_data(ad_value, KAPS_AD_WIDTH_8);
            for (i = 0; i < KAPS_AD_WIDTH_8; i++) {
                write_ads.bpm_ad = (write_ads.bpm_ad << 8) | ad_value[i];
            }

            write_ads.format_map_00 = GET_RAND_VAL(0, 12);
            write_ads.format_map_03 = GET_RAND_VAL(0, 12);
            write_ads.format_map_07 = GET_RAND_VAL(0, 12);
            write_ads.format_map_11 = GET_RAND_VAL(0, 12);
            write_ads.format_map_15 = GET_RAND_VAL(0, 12);

            KBP_STRY(xpt->kaps_command(xpt, KAPS_CMD_WRITE, func, KAPS_RPB_BLOCK_START + blk_nr, row_nr, sizeof(write_ads), (uint8_t *) &write_ads));

            /* Configure the read ads request payload */
            func = KAPS_FUNC4;

            KBP_STRY(xpt->kaps_command(xpt, KAPS_CMD_READ, func, KAPS_RPB_BLOCK_START + blk_nr, row_nr, sizeof(read_ads_resp),
                                   (uint8_t *) &read_ads_resp));
            if (kaps_compare_ads(write_ads, read_ads_resp) != 0) {
                kbp_printf("Mis Match in data at Row Number: %d for ADS Blk Number: %d\n", row_nr, blk_nr);
                return 1;
            }
        }
        kbp_printf("\tVerified #%d Rows\n", nrows);
    }
    kbp_printf(" ADS Block Testing is Passed \n\n");

    for (bkt_nr = 0; bkt_nr < KAPS_MIN_NUM_BB; bkt_nr++) {

        kbp_printf(" Write/Read to Bucket Block #%d\n", bkt_nr + 1);
        nrows = GET_RAND_VAL(0, KAPS_BKT_NUM_ROWS);
        for (iter = 0; iter < nrows; iter++) {

            struct kaps_bkt_rqt write_bkt;
            struct kaps_bkt_rqt read_bkt_resp;

            row_nr = GET_RAND_VAL(0, KAPS_BKT_NUM_ROWS);

            /* Configure the write bucket block request payload */
            func = KAPS_FUNC2;
            kaps_construct_data(write_bkt.data, KAPS_BKT_WIDTH_8);

            KBP_STRY(xpt->kaps_command(xpt, KAPS_CMD_WRITE, func, KAPS_BB_START + bkt_nr, row_nr, sizeof(write_bkt), (uint8_t *) &write_bkt));

            /* Configure the read bucket block request payload */
            func = KAPS_FUNC2;

            KBP_STRY(xpt->kaps_command(xpt, KAPS_CMD_READ, func, KAPS_BB_START + bkt_nr, row_nr, sizeof(read_bkt_resp),
                                   (uint8_t *) &read_bkt_resp));
            if (kbp_memcmp(write_bkt.data, read_bkt_resp.data, KAPS_BKT_WIDTH_8) != 0) {
                kbp_printf("Mis Match in data at Row Number: %d for BB Number: %d\n", row_nr, bkt_nr);
                return 1;
            }
        }
        kbp_printf("\tVerified #%d Rows\n", nrows);
    }
    kbp_printf(" Bucket Blocks Testing is Passed \n");
    return status;
}

static void kaps_print_entry(uint8_t *data, uint32_t LenInBytes)
{
    int32_t i;

    for (i = 0;i < LenInBytes; i++)
        kbp_printf("%02x ",data[i]);
    kbp_printf("\n");

}

static kbp_status kaps_write_to_rpb(struct kaps_xpt *xpt, uint8_t *search_key, uint32_t blk_num, uint32_t row_num, uint32_t rpb_key_len_8)
{
    uint8_t rpb_data[KAPS_RPB_WIDTH_8] = { 0x00, };
    uint8_t mask[KAPS_RPB_WIDTH_8];
    uint8_t req_buffer[KAPS_RPB_WIDTH_8 + 1];
    uint8_t  i;
    struct kaps_write_rpb_rqt write_rpb_key0, write_rpb_key1;
    kbp_status status = KBP_OK;
    uint8_t func;

    for (i = 0; i < rpb_key_len_8; i++) {
        rpb_data[i] = search_key[i];
        mask[i] = 0x00;
    }

    for (i = rpb_key_len_8; i < KAPS_RPB_WIDTH_8; i++) {
        mask[i] = rpb_data[i] = 0xFF;
    }

    func = KAPS_FUNC1;
    write_rpb_key0.is_valid = 0x3;
    write_rpb_key1.is_valid = 0x3;

    write_rpb_key0.search_lsn = 0;
    write_rpb_key1.search_lsn = 0;

    kaps_convert_dm_to_xy(rpb_data, mask, write_rpb_key0.key, write_rpb_key1.key, KAPS_RPB_WIDTH_8);

    for ( i = 0 ; i < KAPS_RPB_WIDTH_8; i++) {
        write_bits_to_array(req_buffer,KAPS_RPB_WIDTH_8 + 1, ( (KAPS_RPB_WIDTH_8 - i) * 8) + 5, ((KAPS_RPB_WIDTH_8 - i) * 8) - 2, write_rpb_key1.key[i]);
    }
    write_bits_to_array(req_buffer,KAPS_RPB_WIDTH_8 + 1, 5, 2, write_rpb_key1.search_lsn);
    write_bits_to_array(req_buffer,KAPS_RPB_WIDTH_8 + 1, 1, 0, write_rpb_key1.is_valid);
    write_bits_to_array(req_buffer,KAPS_RPB_WIDTH_8 + 1, 167, 166, 0);

    KBP_STRY(xpt->kaps_command(xpt, KAPS_CMD_WRITE, func, KAPS_RPB_BLOCK_START + blk_num, 2 * row_num + 1, sizeof(write_rpb_key1), req_buffer));

    for ( i = 0 ; i < KAPS_RPB_WIDTH_8; i++) {
        write_bits_to_array(req_buffer,KAPS_RPB_WIDTH_8 + 1, ( (KAPS_RPB_WIDTH_8 - i) * 8) + 5, ((KAPS_RPB_WIDTH_8 - i) * 8) - 2, write_rpb_key0.key[i]);
    }
    write_bits_to_array(req_buffer,KAPS_RPB_WIDTH_8 + 1, 5, 2, write_rpb_key0.search_lsn);
    write_bits_to_array(req_buffer,KAPS_RPB_WIDTH_8 + 1, 1, 0, write_rpb_key0.is_valid);
    write_bits_to_array(req_buffer,KAPS_RPB_WIDTH_8 + 1, 167, 166, 0);
    KBP_STRY(xpt->kaps_command(xpt, KAPS_CMD_WRITE, func, KAPS_RPB_BLOCK_START + blk_num, 2 * row_num, sizeof(write_rpb_key0), req_buffer));
    return status;

}

static kbp_status kaps_write_to_ads(struct kaps_xpt *xpt,struct kaps_ads ads, uint32_t blk_num, uint32_t row_num)
{

    kbp_status status = KBP_OK;
    uint8_t req_buffer[16];

    write_bits_to_array(req_buffer, 16, 19,0, ads.bpm_ad);
    write_bits_to_array(req_buffer, 16, 27, 20, ads.bpm_len);
    write_bits_to_array(req_buffer, 16, 31, 28, ads.row_offset);

    write_bits_to_array(req_buffer, 16, 35, 32, ads.format_map_00);
    write_bits_to_array(req_buffer, 16, 39, 36, ads.format_map_01);
    write_bits_to_array(req_buffer, 16, 43, 40, ads.format_map_02);
    write_bits_to_array(req_buffer, 16, 47, 44, ads.format_map_03);
    write_bits_to_array(req_buffer, 16, 51, 48, ads.format_map_04);
    write_bits_to_array(req_buffer, 16, 55, 52, ads.format_map_05);
    write_bits_to_array(req_buffer, 16, 59, 56, ads.format_map_06);
    write_bits_to_array(req_buffer, 16, 63, 60, ads.format_map_07);
    write_bits_to_array(req_buffer, 16, 67, 64, ads.format_map_08);
    write_bits_to_array(req_buffer, 16, 71, 68, ads.format_map_09);
    write_bits_to_array(req_buffer, 16, 75, 72, ads.format_map_10);
    write_bits_to_array(req_buffer, 16, 79, 76, ads.format_map_11);
    write_bits_to_array(req_buffer, 16, 83, 80, ads.format_map_12);
    write_bits_to_array(req_buffer, 16, 87, 84, ads.format_map_13);
    write_bits_to_array(req_buffer, 16, 91, 88, ads.format_map_14);
    write_bits_to_array(req_buffer, 16, 95, 92, ads.format_map_15);

    write_bits_to_array(req_buffer, 16, 106, 96, ads.bkt_row);
    write_bits_to_array(req_buffer, 16, 111, 107, ads.reserved);
    write_bits_to_array(req_buffer, 16, 119, 112, ads.key_shift);
    write_bits_to_array(req_buffer, 16, 127, 120, ads.ecc);

    status = xpt->kaps_command(xpt, KAPS_CMD_WRITE, KAPS_FUNC4, KAPS_RPB_BLOCK_START + blk_num, row_num, sizeof(ads), (uint8_t *) &req_buffer);
    return status;
}

static kbp_status kaps_write_to_bb(struct kaps_xpt *xpt, uint8_t *bktdata,uint32_t bkt_num, uint32_t bkt_row_num)
{
    struct kaps_bkt_rqt write_bkt;
    kbp_status status = KBP_OK;
    uint8_t func;

    func = KAPS_FUNC2;

    kbp_memset(write_bkt.data, 0, KAPS_BKT_WIDTH_8);
    kbp_memcpy(write_bkt.data, bktdata, KAPS_BKT_WIDTH_8);

    status = xpt->kaps_command(xpt, KAPS_CMD_WRITE, func, KAPS_BB_START + bkt_num * 2, bkt_row_num, sizeof(write_bkt), (uint8_t *) &write_bkt);
    return status;

}

kbp_status kaps_diag_02(struct kaps_xpt *xpt)
{
    uint8_t srch_data[PREFIX_LEN_8] = { 0x00, };
 /*   uint8_t rpb_data[21] = { 0x00, };
    uint8_t rpb_data_1[21] = { 0x00, };*/
    uint8_t ad_value_1[KAPS_AD_WIDTH_8];
    uint8_t ad_value_2[KAPS_AD_WIDTH_8];
    uint8_t bktdata[KAPS_BKT_WIDTH_8] = {0x00, };
    uint32_t ad_data = 0, rpb_key_len_8 = 0, pfx_len_8 = 0;
    uint32_t bkt_len_8 = 0, i;
    uint32_t rpb_blk_num, rpb_row_num;
    uint32_t bkt_num, bkt_row_num;
    uint32_t register_data = 0;
    kbp_status status = KBP_OK;

    struct kaps_search_result kaps_result;
    struct kaps_ads ads1, ads2;

    /* Reset the KAPS Blocks */
    xpt->kaps_command(xpt, KAPS_CMD_EXTENDED, KAPS_FUNC15, 0, 0, 0, NULL);

    /* Enumerate the KAPS Blocks */
    xpt->kaps_command(xpt, KAPS_CMD_EXTENDED, KAPS_FUNC14, 0, 0, 0, NULL);

    pfx_len_8 = PREFIX_LEN_8;
    kbp_memset(&ads1 , 0, sizeof(struct kaps_ads));
    kbp_memset(&ads2 , 0, sizeof(struct kaps_ads));
    kaps_construct_data(srch_data, PREFIX_LEN_8);
    kaps_construct_data(ad_value_1, KAPS_AD_WIDTH_8);
    kaps_construct_data(ad_value_2, KAPS_AD_WIDTH_8);

    kbp_printf("\n -- Search - 1 -- \n\n");
    rpb_blk_num = 0;
    rpb_row_num = 1;
    bkt_num = 0;
    bkt_row_num = 2;
    rpb_key_len_8 = 7;
    bkt_len_8 = pfx_len_8 - rpb_key_len_8;

    kbp_printf(" RPB Block Number: %d RPB Row Number: %d\n", rpb_blk_num, rpb_row_num);
    kbp_printf(" Bucket Num: %d Bucket Row Num: %d\n", bkt_num, bkt_row_num);

    KBP_STRY(xpt->kaps_command(xpt, KAPS_CMD_READ, KAPS_FUNC0, KAPS_RPB_BLOCK_START + rpb_blk_num, 0x00000002, sizeof(register_data), (uint8_t *) &register_data ));
    write_bits_to_array((uint8_t *) &register_data, sizeof(register_data), 2, 2, 1);
    KBP_STRY(xpt->kaps_command(xpt, KAPS_CMD_WRITE, KAPS_FUNC0, KAPS_RPB_BLOCK_START + rpb_blk_num, 0x00000002, sizeof(register_data), (uint8_t *) &register_data ));

    /* write to RPB Block */
    kaps_write_to_rpb(xpt, srch_data, rpb_blk_num, rpb_row_num, rpb_key_len_8);

    /* Fill ADS */
    ads1.bkt_row = bkt_row_num;
    ads1.key_shift = rpb_key_len_8 * 8;
    ads1.row_offset = bkt_num;
    ads1.bpm_len = 0;
    ads1.bpm_ad = 0;

    ads1.format_map_00 = bkt_len_8 + 1;

    /* Write to ADS Block */
    kaps_write_to_ads(xpt, ads1,rpb_blk_num, rpb_row_num);

    register_data = 0;
    write_bits_to_array((uint8_t *) &register_data, sizeof(register_data), 1, 0, rpb_blk_num);
    xpt->kaps_command(xpt, KAPS_CMD_WRITE, KAPS_FUNC0, bkt_num + KAPS_BB_START, 0x00000021, sizeof(register_data), (uint8_t *)&register_data);

    /* construct Bucket Block data */
    for (i = 0; i < bkt_len_8; i++) {
        bktdata[i] = srch_data[rpb_key_len_8 + i];
    }

    /* Writing some part of key in LSN(Bucket Block)*/
    if (bkt_len_8) {
        bktdata[bkt_len_8] = 0x80;
        kbp_memcpy(&bktdata[KAPS_BKT_WIDTH_8 - KAPS_AD_WIDTH_8], ad_value_1, KAPS_AD_WIDTH_8);
    }

    /* Write to Bucket Block */
    kaps_write_to_bb(xpt, bktdata, bkt_num, bkt_row_num);

    for (i = 0;i < KAPS_AD_WIDTH_8; i++)
        ad_data = (ad_data << 8) | ad_value_1[i];

    ad_data = ad_data << 4;
    ad_value_1[0] = (ad_data & 0xFF0000) >> 16;
    ad_value_1[1] = (ad_data & 0x00FF00) >> 8;
    ad_value_1[2] = (ad_data & 0x0000FF);

    kbp_printf(" Search Data: ");
    kaps_print_entry(srch_data, PREFIX_LEN_8);
    KBP_STRY(xpt->kaps_search(xpt, srch_data, rpb_blk_num, &kaps_result));

    if (kaps_result.match_len != PREFIX_LEN_8 * 8) {
        kbp_printf("\n Mis Match in match len Exp: %d, Got: %d\n", PREFIX_LEN_8 * 8, kaps_result.match_len);
        return KBP_MISMATCH;
    } else {
        kbp_printf("\n Match len = %d\n", kaps_result.match_len);
    }

    if (kbp_memcmp(ad_value_1, kaps_result.ad_value, KAPS_AD_WIDTH_8) != 0) {
        kbp_printf("\n Mis Match in AD Value \n");
        kbp_printf("\n Exp =  ");
        kaps_print_entry(ad_value_1, KAPS_AD_WIDTH_8);
        kbp_printf("\n Got =  ");
        kaps_print_entry(kaps_result.ad_value, KAPS_AD_WIDTH_8);
        return KBP_MISMATCH;
    } else {
        kbp_printf("\n AD Value =  ");
        kaps_print_entry(ad_value_1, KAPS_AD_WIDTH_8);
    }

    /* Excercising Lmpsofar search, not storing any part of key in LSN(bucket block) */
    kbp_printf("\n -- Search - 2 -- \n\n");
    kaps_construct_data(&srch_data[PREFIX_LEN_8 / 2], PREFIX_LEN_8 / 2);
    ad_data = 0;
    rpb_blk_num = 0;
    rpb_row_num = 2;
    bkt_num = 0;
    bkt_row_num = 3;
    rpb_key_len_8 = 10;
    bkt_len_8 = pfx_len_8 - rpb_key_len_8;
    kbp_memset(&kaps_result, 0, sizeof(struct kaps_search_result));
    kbp_memset(bktdata, 0, KAPS_BKT_WIDTH_8);

    kbp_printf(" RPB Block Number: %d RPB Row Number: %d\n", rpb_blk_num, rpb_row_num);
    kbp_printf(" Bucket Num: %d Bucket Row Num: %d\n", bkt_num, bkt_row_num);

    /* write to RPB Block */
    kaps_write_to_rpb(xpt, srch_data, rpb_blk_num, rpb_row_num, rpb_key_len_8);

    /* Fill ADS */
    ads2.bkt_row = bkt_row_num;
    ads2.key_shift = rpb_key_len_8 * 8;
    ads2.row_offset = bkt_num;
    ads2.bpm_len = 0;
    ads2.bpm_ad = 0;

    for (i = 0; i < KAPS_AD_WIDTH_8; i++) {
        ads2.bpm_ad = (ads2.bpm_ad << 8) | ad_value_2[i] ;
    }
    ads2.bpm_len = pfx_len_8 * 8;
    ads2.format_map_00 = 0/*bkt_len_8 + 1*/;

    /* Write to ADS Block */
    kaps_write_to_ads(xpt, ads2,rpb_blk_num, rpb_row_num);

    register_data = 0;
    write_bits_to_array((uint8_t *) &register_data, sizeof(register_data), 1, 0, rpb_blk_num);
    xpt->kaps_command(xpt, KAPS_CMD_WRITE, KAPS_FUNC0, bkt_num + KAPS_BB_START, 0x00000021, sizeof(register_data), (uint8_t *)&register_data);


    /* Write to Bucket Block */
   /* kaps_write_to_bb(xpt, bktdata, bkt_num, bkt_row_num);*/

    for (i = 0;i < KAPS_AD_WIDTH_8; i++)
        ad_data = (ad_data << 8) | ad_value_2[i];

    ad_data = ad_data << 4;
    ad_value_2[0] = (ad_data & 0xFF0000) >> 16;
    ad_value_2[1] = (ad_data & 0x00FF00) >> 8;
    ad_value_2[2] = (ad_data & 0x0000FF);

    kbp_printf(" Search Data: ");
    kaps_print_entry(srch_data, PREFIX_LEN_8);
    KBP_STRY(xpt->kaps_search(xpt, srch_data, rpb_blk_num, &kaps_result));

    if (kaps_result.match_len != PREFIX_LEN_8 * 8) {
        kbp_printf("\n Mis Match in match len Exp: %d, Got: %d\n", PREFIX_LEN_8 * 8, kaps_result.match_len);
        return KBP_MISMATCH;
    } else {
        kbp_printf("\n Match len = %d\n", kaps_result.match_len);
    }

    if (kbp_memcmp(ad_value_2, kaps_result.ad_value, KAPS_AD_WIDTH_8) != 0) {
        kbp_printf("\n Mis Match in AD Value \n");
        kbp_printf("\n Exp =  ");
        kaps_print_entry(ad_value_2, KAPS_AD_WIDTH_8);
        kbp_printf("\n Got =  ");
        kaps_print_entry(kaps_result.ad_value, KAPS_AD_WIDTH_8);
        return KBP_MISMATCH;
    } else {
        kbp_printf("\n AD Value =  ");
        kaps_print_entry(ad_value_2, KAPS_AD_WIDTH_8);
    }

    kbp_memset(bktdata, 0, KAPS_BKT_WIDTH_8);
    kaps_write_to_bb(xpt, bktdata, bkt_num, bkt_row_num);
    return status;
}

soc_error_t soc_jer_pp_diag_kaps_lkup_info_get(
   SOC_SAND_IN int unit
   )
{
    ARAD_PP_DIAG_REG_FIELD fld;
    uint32 regs_val[ARAD_PP_DIAG_DBG_VAL_LEN];
    uint32 rv = SOC_SAND_OK;
    /*0:RPF private, 1:RPF public, 2:FWD private, 3:FWD public */
    uint32 search_keys[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES][BYTES2WORDS(JER_KAPS_KEY_BUFFER_NOF_BYTES)];
    uint32 tmp_buffer[BYTES2WORDS(JER_KAPS_KEY_BUFFER_NOF_BYTES)];
    uint32 ad_array[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES];
    uint32 status[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES];

    uint32 i,j;

    char* search_names[] = {"first        ",
                            "first public ",
                            "second       ",
                            "second public"
                            };

    SOCDNX_INIT_FUNC_DEFS;

    for (i=0; i<JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES; i++) {
        for (j=0; j<BYTES2WORDS(JER_KAPS_KEY_BUFFER_NOF_BYTES); j++) {
            search_keys[i][j] = 0;
        }
        ad_array[i] = 0;
        status[i] = 0;
    }


    /*first : RPF, second : forwarding, 0 : private, 1 : public*/

    JER_PP_DIAG_FLD_READ(&fld,ARAD_IHP_ID,30,2,155,0,5);
    rv = soc_sand_bitstream_get_any_field(regs_val,0,159-4+1, tmp_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    SHR_BITCOPY_RANGE(search_keys[0], 4, tmp_buffer, 0, 159-4+1);

    JER_PP_DIAG_FLD_READ(&fld,ARAD_IHP_ID,30,1,255,252,5);
    rv = soc_sand_bitstream_get_any_field(regs_val, 0, 3, tmp_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    SHR_BITCOPY_RANGE(search_keys[0], 0, tmp_buffer, 0, 4);


    JER_PP_DIAG_FLD_READ(&fld,ARAD_IHP_ID,30,1,77,0,5);
    rv = soc_sand_bitstream_get_any_field(regs_val,0,159-82+1,tmp_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    SHR_BITCOPY_RANGE(search_keys[1], 82, tmp_buffer, 0, 159-82+1);

    JER_PP_DIAG_FLD_READ(&fld,ARAD_IHP_ID,30,0,255,174,5);
    rv = soc_sand_bitstream_get_any_field(regs_val,0,81,tmp_buffer);
    SOCDNX_SAND_IF_ERR_EXIT(rv);
    SHR_BITCOPY_RANGE(search_keys[1], 0, tmp_buffer, 0, 82);


    JER_PP_DIAG_FLD_READ(&fld,ARAD_IHP_ID,30,1,244,85,5);
    rv = soc_sand_bitstream_get_any_field(regs_val,0,BYTES2BITS(JER_KAPS_KEY_BUFFER_NOF_BYTES),search_keys[2]);
    SOCDNX_SAND_IF_ERR_EXIT(rv);


    JER_PP_DIAG_FLD_READ(&fld,ARAD_IHP_ID,30,0,166,7,5);
    rv = soc_sand_bitstream_get_any_field(regs_val,0,BYTES2BITS(JER_KAPS_KEY_BUFFER_NOF_BYTES),search_keys[3]);
    SOCDNX_SAND_IF_ERR_EXIT(rv);


    /*AD*/

    JER_PP_DIAG_FLD_READ(&fld,ARAD_IHP_ID,31,0,119,100,5);
    rv = soc_sand_bitstream_get_any_field(regs_val,0,JER_KAPS_AD_WIDTH_IN_BITS,&ad_array[0]);
    SOCDNX_SAND_IF_ERR_EXIT(rv);

    JER_PP_DIAG_FLD_READ(&fld,ARAD_IHP_ID,31,0,59,40,5);
    rv = soc_sand_bitstream_get_any_field(regs_val,0,JER_KAPS_AD_WIDTH_IN_BITS,&ad_array[1]);
    SOCDNX_SAND_IF_ERR_EXIT(rv);

    JER_PP_DIAG_FLD_READ(&fld,ARAD_IHP_ID,31,0,89,70,5);
    rv = soc_sand_bitstream_get_any_field(regs_val,0,JER_KAPS_AD_WIDTH_IN_BITS,&ad_array[2]);
    SOCDNX_SAND_IF_ERR_EXIT(rv);

    JER_PP_DIAG_FLD_READ(&fld,ARAD_IHP_ID,31,0,29,10,5);
    rv = soc_sand_bitstream_get_any_field(regs_val,0,JER_KAPS_AD_WIDTH_IN_BITS,&ad_array[3]);
    SOCDNX_SAND_IF_ERR_EXIT(rv);

    /*status*/

    JER_PP_DIAG_FLD_READ(&fld,ARAD_IHP_ID,31,0,99,98,5);
    rv = soc_sand_bitstream_get_any_field(regs_val,0,2,&status[0]);
    SOCDNX_SAND_IF_ERR_EXIT(rv);

    JER_PP_DIAG_FLD_READ(&fld,ARAD_IHP_ID,31,0,39,38,5);
    rv = soc_sand_bitstream_get_any_field(regs_val,0,2,&status[1]);
    SOCDNX_SAND_IF_ERR_EXIT(rv);

    JER_PP_DIAG_FLD_READ(&fld,ARAD_IHP_ID,31,0,69,68,5);
    rv = soc_sand_bitstream_get_any_field(regs_val,0,2,&status[2]);
    SOCDNX_SAND_IF_ERR_EXIT(rv);

    JER_PP_DIAG_FLD_READ(&fld,ARAD_IHP_ID,31,0,9,8,5);
    rv = soc_sand_bitstream_get_any_field(regs_val,0,2,&status[3]);
    SOCDNX_SAND_IF_ERR_EXIT(rv);

    /*Print all of the keys and AD*/
    for (i=0; i < JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES; i++) {
        cli_out("%s key: 0x", search_names[i]);
        for (j=0; j < BYTES2WORDS(JER_KAPS_KEY_BUFFER_NOF_BYTES); j++) {
            cli_out("%08x", search_keys[i][BYTES2WORDS(JER_KAPS_KEY_BUFFER_NOF_BYTES)-1-j]);
        }
        cli_out("   payload: 0x%05x", ad_array[i]);
        cli_out("   status: %01d\n", status[i]);
    }

exit:
  SOCDNX_FUNC_RETURN
}

#else /* defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030) */
soc_error_t soc_jer_pp_diag_kaps_lkup_info_get(
   SOC_SAND_IN int unit
   )
{
    SOCDNX_INIT_FUNC_DEFS;
    cli_out("KAPS unsupported\n");
    SOCDNX_FUNC_RETURN
}

#endif /* defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030) */
