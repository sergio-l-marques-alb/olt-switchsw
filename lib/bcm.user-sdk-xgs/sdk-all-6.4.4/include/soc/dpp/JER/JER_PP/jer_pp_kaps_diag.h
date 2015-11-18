/* $Id: jer_pp_kaps_diag.h, hagayco Exp $
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
*/

/*
 **************************************************************************************
 Copyright 2009-2014 Broadcom Corporation

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

#ifndef __KAPS_DIAG_H
#define __KAPS_DIAG_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <soc/kbp/alg_kbp/include/errors.h>
#include <soc/kbp/alg_kbp/include/kbp_portable.h>
#include <soc/kbp/alg_kbp/include/xpt_kaps.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/error.h>


#ifdef __cplusplus
extern "C" {

#endif

#define KAPS_BKT_WIDTH_1 (480)

#define KAPS_BKT_WIDTH_8 (60)

#define KAPS_BKT_NUM_ROWS (1024)


#define KAPS_ADS_WIDTH_1 (128)

#define KAPS_ADS_WIDTH_8 (16)

#define KAPS_ADS_NUW_ROWS (2 * 1024)

#define KAPS_NUM_ADS_BLOCKS (4)




#define KAPS_RPB_WIDTH_1 (160)

#define KAPS_RPB_WIDTH_8 (20)

#define KAPS_RPB_BLOCK_SIZE (2*1024)

#define KAPS_NUM_RPB_BLOCKS (4)


#define KAPS_MAX_NUM_BB (256)

#define KAPS_MIN_NUM_BB (16)


#define KAPS_MAX_SEARCH_TAG (0x7F)


#define KAPS_MAX_PFX_PER_BKT_ROW (16)

#define KAPS_NUM_GRANULARITIES (11)


#define KAPS_AD_ARRAY_START_BIT_POS (4)

#define KAPS_AD_ARRAY_END_BIT_POS (23)


#define KAPS_RPB_BLOCK_START (1)

#define KAPS_RPB_BLOCK_END (4)


#define KAPS_BB_START (5)

#define KAPS_BB_END (36)

/*************
 * MACROS    *
 *************/
/* { */

#define JER_PP_DIAG_FLD_READ(prm_fld, prm_blk, prm_addr_msb, prm_addr_lsb, prm_fld_msb, prm_fld_lsb, prm_err_num)  \
  ARAD_PP_DIAG_FLD_FILL(prm_fld, prm_addr_msb, prm_addr_lsb, prm_fld_msb, prm_fld_lsb);    \
  rv = arad_pp_diag_dbg_val_get_unsafe(      \
          unit,      \
          prm_blk,      \
          prm_fld,      \
          regs_val      \
        );              \
  SOCDNX_IF_ERR_EXIT(rv);;

/* } */


/**
 * @brief KAPS Associated Data Store
 */
struct kaps_ads
{
    __EO_23(uint32_t bpm_ad:20, /**< Associated data of the lmpsofar*/
        uint32_t bpm_len:8,     /**< Best Matching Prefix length*/
        uint32_t row_offset:4,    /**< Bucket block number corresponding to the first LPU of the LSN*/

        uint32_t format_map_00:4,  /**< Format map indicating the granularities of the buckets*/
        uint32_t format_map_01:4,
        uint32_t format_map_02:4,
        uint32_t format_map_03:4,
        uint32_t format_map_04:4,
        uint32_t format_map_05:4,
        uint32_t format_map_06:4,
        uint32_t format_map_07:4,

        uint32_t format_map_08:4,
        uint32_t format_map_09:4,
        uint32_t format_map_10:4,
        uint32_t format_map_11:4,
        uint32_t format_map_12:4,
        uint32_t format_map_13:4,
        uint32_t format_map_14:4,
        uint32_t format_map_15:4,

        uint32_t bkt_row:11,        /**< The row in the bucket block */
        uint32_t reserved:5,        /**< Reserved field */
        uint32_t key_shift:8,       /**< Number of bits by which the key should be shifted before sending to bucket block*/
        uint32_t ecc:8);            /**< Error correcting code, will be filled up by hardware */

};

struct kaps_write_rpb_rqt
{

    uint8_t key[KAPS_RPB_WIDTH_8];      /**< The key0/key1 value, already converted to X/Y format, to be written to the RPB. */
    __EO_3(uint8_t search_lsn:4, /**< The search lsn value to be matched with RPB_GLOBAL_CONFIG Register */
    uint8_t is_valid:2,   /**< If the entry is being added to the RPB, is_valid is set to 0x3 whereas if the entry is being deleted, is_valid is set to 0*/
    uint8_t reserved:2);   /**< Reserved bits */
};


struct kaps_bkt_rqt
{
    uint8_t data[KAPS_BKT_WIDTH_8];      /**< The data to be written to/read from the Bucket Block. */
};


struct kaps_read_rpb_resp
{
    uint8_t key[KAPS_RPB_WIDTH_8];      /**< The key0/key1 value, already converted to X/Y format, to be written to the RPB. */
    __EO_3(uint8_t search_lsn:4, /**< The search lsn value to be matched with RPB_GLOBAL_CONFIG Register */
    uint8_t is_valid:2,   /**< If the entry is being added to the RPB, is_valid is set to 0x3 whereas if the entry is being deleted, is_valid is set to 0*/
    uint8_t reserved:2);   /**< Reserved bits */
};


/* KAPS diag tool -1, where we write/read to various blocks */
kbp_status kaps_diag_01(struct kaps_xpt *xpt);

/* KAPS diag tool -2, where we excercise two searches (LSN, Lmp sofar) */
kbp_status kaps_diag_02(struct kaps_xpt *xpt);

/*prints the 4 KAPS search keys and their associated data*/
soc_error_t soc_jer_pp_diag_kaps_lkup_info_get(
   SOC_SAND_IN int      unit
   );

#ifdef __cplusplus
}
#endif
#endif
