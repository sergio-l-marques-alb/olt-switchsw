/* $Id: jer_pp_kaps_entry_mgmt.c, hagayco Exp $
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
 * $
*/

#include <soc/mem.h>

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FORWARD
#include <soc/mem.h>

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_kbp.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps_entry_mgmt.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define UINT64_SIZE_IN_BITS 64
#define UINT32_SIZE_IN_BITS 32
#define UINT8_SIZE_IN_BITS 8

/* } */

/*************
 *  MACROS   *
 *************/
/* { */


/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */


  uint32
    jer_pp_kaps_entry_add_hw(
       SOC_SAND_IN  int                         unit,
       SOC_SAND_IN  uint32                      table_id,
       SOC_SAND_IN  uint32                      prefix_len,
       SOC_SAND_OUT  uint8                      *data,
       SOC_SAND_OUT  uint8                      *assoData,
       SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
    )
{
    int32
        res;
    struct kbp_db 
       *db_p = NULL;
    struct kbp_ad_db 
       *ad_db_p = NULL;
    struct kbp_entry 
       *db_entry = NULL;
    struct kbp_ad 
       *ad_entry = NULL;
    uint32
        kaps_cache_mode;
    int is_update = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *success = SOC_SAND_SUCCESS;
    
    jer_kaps_db_get(unit,
                    table_id, 
                    &db_p);

    SOC_SAND_CHECK_NULL_PTR(db_p, 30, exit);

    jer_kaps_ad_db_get(unit,
                       table_id, 
                       &ad_db_p
        );

    SOC_SAND_CHECK_NULL_PTR(ad_db_p, 30, exit);

    /* Check if the db_entry exists */
    kbp_db_get_prefix_handle(db_p, data, prefix_len, &db_entry);
    if (db_entry != NULL) {
        is_update = 1;
    }

    if (!is_update) {
        res = kbp_db_add_prefix(
                db_p, 
                data, 
                prefix_len,
                &db_entry);

        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            *success = SOC_SAND_FAILURE_INTERNAL_ERR;
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 40, exit);
        }

        res = kbp_ad_db_add_entry(
                ad_db_p,
                assoData,
                &ad_entry
              );

        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            *success = SOC_SAND_FAILURE_INTERNAL_ERR;
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 60, exit);
        }

        res = kbp_entry_add_ad(
                db_p,
                db_entry,
                ad_entry
              );

        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            *success = SOC_SAND_FAILURE_INTERNAL_ERR;
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 70, exit);
        }
    }
    else {
        res = kbp_entry_get_ad(db_p, db_entry, &ad_entry);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            *success = SOC_SAND_FAILURE_INTERNAL_ERR;
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 70, exit);
        }

        res = kbp_ad_db_update_entry(ad_db_p, ad_entry, assoData);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            *success = SOC_SAND_FAILURE_INTERNAL_ERR;
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 70, exit);
        }
    }

    res = arad_sw_db_frwrd_ip_kbp_cache_mode_get(unit, &kaps_cache_mode);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (kaps_cache_mode == FALSE) {
#ifdef JER_KAPS_TIME_MEASUREMENTS
        soc_sand_ll_timer_set("jer_pp_kaps_entry_add_hw", 6);
#endif 
        res = kbp_db_install(db_p);
#ifdef ARAD_PP_KBP_TIME_MEASUREMENTS
        soc_sand_ll_timer_stop(6);
#endif 
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
           LOG_ERROR(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "Error in %s(): Entry add : kbp_db_install with failed: %s!\n"), 
                                 FUNCTION_NAME(),
                      kbp_get_status_string(res)));
            *success = SOC_SAND_FAILURE_INTERNAL_ERR;
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 80, exit);
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_pp_kaps_entry_add()",0,0);
}

  uint32
    jer_pp_kaps_entry_remove_hw(
       SOC_SAND_IN  int                         unit,
       SOC_SAND_IN  uint32                      table_id,
       SOC_SAND_IN  uint32                      prefix_len,
       SOC_SAND_OUT uint8                       *data
    )
{
    uint32
        res, kaps_cache_mode;
    struct kbp_db 
        *db_p = NULL;
    struct kbp_ad_db 
        *ad_db_p = NULL;
    struct kbp_ad 
       *ad_entry = NULL;
    struct kbp_entry 
       *db_entry = NULL;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    jer_kaps_db_get(unit,
                    table_id, 
                    &db_p);

    SOC_SAND_CHECK_NULL_PTR(db_p, 30, exit);

    /* Retrieve the db_entry */
    res = kbp_db_get_prefix_handle(db_p, data, prefix_len, &db_entry);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 30, exit);
    }
    SOC_SAND_CHECK_NULL_PTR(db_entry, 40, exit);

    /* Retrieve the ad_entry */
    res = kbp_entry_get_ad(db_p, db_entry, &ad_entry);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 50, exit);
    }
    SOC_SAND_CHECK_NULL_PTR(ad_entry, 60, exit);

    res = kbp_db_delete_entry( 
            db_p,
            db_entry
          );
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 70, exit);
    }

    jer_kaps_ad_db_get(unit,
                       table_id, 
                       &ad_db_p
        );

    SOC_SAND_CHECK_NULL_PTR(ad_db_p, 30, exit);

    res = kbp_ad_db_delete_entry(
            ad_db_p, 
            ad_entry
          );
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "Error in %s(): kbp_ad_db_delete_entry failed with error: %s!\n"), 
                             FUNCTION_NAME(),
                  kbp_get_status_string(res)));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 90, exit);
    } 

    res = arad_sw_db_frwrd_ip_kbp_cache_mode_get(unit, &kaps_cache_mode);
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
    if (kaps_cache_mode == FALSE) {

#ifdef JER_KAPS_TIME_MEASUREMENTS
        soc_sand_ll_timer_set("jer_pp_kaps_entry_remove_hw", 4);
#endif 
        res = kbp_db_install(db_p);
#ifdef ARAD_PP_KBP_TIME_MEASUREMENTS
        soc_sand_ll_timer_stop(4);
#endif 
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 110, exit);
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_pp_kaps_entry_remove_hw()",0,0);
}

  uint32
    jer_pp_kaps_entry_get_hw(
       SOC_SAND_IN  int         unit,
       SOC_SAND_IN  uint8       table_id,
       SOC_SAND_IN  uint32      prefix_len,
       SOC_SAND_OUT uint8       *data,
       SOC_SAND_OUT uint8       *assoData,
       SOC_SAND_OUT uint8       *found
    )
{
    uint32
        res;
    struct kbp_db 
        *db_p = NULL;
    struct kbp_ad_db 
        *ad_db_p = NULL;
    struct kbp_ad 
        *ad_entry = NULL;
    struct kbp_entry
        *db_entry = NULL;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *found = FALSE;

    jer_kaps_db_get(unit,
                    table_id, 
                    &db_p);

    SOC_SAND_CHECK_NULL_PTR(db_p, 30, exit);

    /* Retrieve the db_entry */
    kbp_db_get_prefix_handle(db_p, data, prefix_len, &db_entry);
    if (!db_entry)
        goto exit;

    /* Retrieve the ad_entry */
    jer_kaps_ad_db_get(unit,
                       table_id, 
                       &ad_db_p
        );

    SOC_SAND_CHECK_NULL_PTR(ad_db_p, 30, exit);

    res = kbp_entry_get_ad(db_p, db_entry, &ad_entry);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 80, exit);
    }

    SOC_SAND_CHECK_NULL_PTR(ad_entry, 90, exit);

    sal_memset(assoData, 0x0, sizeof(uint8) * JER_KAPS_AD_BUFFER_NOF_BYTES);

    res = kbp_ad_db_get(ad_db_p, ad_entry, assoData);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
    }

    *found = TRUE;
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ip_tcam_kbp_lpm_route_get()",0,0);
}


STATIC
  uint32
    jer_pp_kaps_entry_table_clear_hw(
       SOC_SAND_IN  int         unit,
       SOC_SAND_IN  uint8       table_id
    )
{
    uint32
        kaps_cache_mode, res;
    struct kbp_db 
        *db_p = NULL;
    struct kbp_ad_db 
        *ad_db_p = NULL;
    struct kbp_ad 
        *ad_entry = NULL;
    struct kbp_entry
        *db_entry = NULL;
    struct kbp_entry_iter
        *iter_p = NULL;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    jer_kaps_db_get(unit,
                    table_id, 
                    &db_p);

    SOC_SAND_CHECK_NULL_PTR(db_p, 30, exit);

    /* Retrieve the ad_entry */
    jer_kaps_ad_db_get(unit,
                       table_id, 
                       &ad_db_p
        );

    SOC_SAND_CHECK_NULL_PTR(ad_db_p, 30, exit);

    res = kbp_db_entry_iter_init(db_p, &iter_p);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 80, exit);
    }
    SOC_SAND_CHECK_NULL_PTR(iter_p, 90, exit);

    res = kbp_db_entry_iter_next(db_p, iter_p, &db_entry);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 80, exit);
    }

    while (db_entry != NULL) {
        /* Retrieve the ad_entry */
        res = kbp_entry_get_ad(db_p, db_entry, &ad_entry);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 50, exit);
        }
        SOC_SAND_CHECK_NULL_PTR(ad_entry, 60, exit);

        res = kbp_db_delete_entry( 
                db_p,
                db_entry
              );
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 70, exit);
        }

        res = kbp_ad_db_delete_entry(
                ad_db_p, 
                ad_entry
              );
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
           LOG_ERROR(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "Error in %s(): kbp_ad_db_delete_entry failed with error: %s!\n"), 
                                 FUNCTION_NAME(),
                      kbp_get_status_string(res)));
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 90, exit);
        } 

        /* WA for iteration for delete. Wait for KBP to support */
        res = kbp_db_entry_iter_destroy(db_p, iter_p);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 80, exit);
        }

        res = kbp_db_entry_iter_init(db_p, &iter_p);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 80, exit);
        }
        SOC_SAND_CHECK_NULL_PTR(iter_p, 90, exit);

        res = kbp_db_entry_iter_next(db_p, iter_p, &db_entry);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 80, exit);
        }
    }

    res = kbp_db_entry_iter_destroy(db_p, iter_p);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 80, exit);
    }

    res = arad_sw_db_frwrd_ip_kbp_cache_mode_get(unit, &kaps_cache_mode);
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
    if (kaps_cache_mode == FALSE) {

#ifdef JER_KAPS_TIME_MEASUREMENTS
        soc_sand_ll_timer_set("jer_pp_kaps_entry_remove_hw", 4);
#endif 
        res = kbp_db_install(db_p);
#ifdef ARAD_PP_KBP_TIME_MEASUREMENTS
        soc_sand_ll_timer_stop(4);
#endif 
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 110, exit);
        }
    }
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_pp_kaps_entry_table_clear_hw()",0,0);
}


void jer_pp_kaps_key_encode(SOC_SAND_IN    uint8 tbl_prefix,
                           SOC_SAND_IN    uint8 nof_quals,
                           SOC_SAND_IN    uint64 *qual_vals,
                           SOC_SAND_IN    uint8 *qual_nof_bits,
                           SOC_SAND_OUT   uint8 *data_bytes)
{
    int32 
        bit_offset,
        data_idx,
        qual_idx, i;
    uint64 val, data[3] = {0};

    /* add table prefix to key buffer */
    COMPILER_64_SET(data[0], 0, tbl_prefix);
    COMPILER_64_SHL(data[0], UINT64_SIZE_IN_BITS - JER_KAPS_TBL_PREFIX_NOF_BITS);

    /* add qualifiers to key buffer */
    bit_offset = UINT64_SIZE_IN_BITS - JER_KAPS_TBL_PREFIX_NOF_BITS;
    data_idx = 0;
    for (qual_idx = 0; qual_idx < nof_quals; qual_idx++)
    {  
        if (qual_nof_bits[qual_idx] > bit_offset)
        {
            /* qualifier length is bigger than what is left free in this uint64 */
            val = qual_vals[qual_idx];
            data[data_idx] |= COMPILER_64_SHR(val,(qual_nof_bits[qual_idx] - bit_offset)); /* write what you can in this uint64 */
            data_idx++;
            bit_offset += UINT64_SIZE_IN_BITS; /* update bit offset for next uint64 */
        }

        bit_offset -= qual_nof_bits[qual_idx]; /* update the bit offset in the uint64 */
        val = qual_vals[qual_idx];
        data[data_idx] |=  COMPILER_64_SHL(val, bit_offset); /* write the qual value in place */
    }

    /* copy to bytes buffer */
    for (i = 0; i < JER_KAPS_KEY_BUFFER_NOF_BYTES; i++) {
        val = data[i/UINT8_SIZE_IN_BITS];
        COMPILER_64_SHR(val, (UINT64_SIZE_IN_BITS - ((i%8)+1)*8));
        data_bytes[i] = val;
    }
}

uint32
  jer_pp_kaps_route_to_kaps_payload_buffer_encode(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                     *asso_data_buffer,
    SOC_SAND_OUT uint8                      *asso_data
  )
{
    uint32
        byte_ndx,
        byte_array_idx,
        fld_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(asso_data_buffer);
    SOC_SAND_CHECK_NULL_INPUT(asso_data);

    sal_memset(asso_data, 0x0, sizeof(uint8) * JER_KAPS_AD_BUFFER_NOF_BYTES);

    for (byte_ndx = 0; byte_ndx < JER_KAPS_AD_BUFFER_NOF_BYTES; ++byte_ndx)
    {
        byte_array_idx = JER_KAPS_AD_BUFFER_NOF_BYTES - byte_ndx - 1;
        /* Build data */
        fld_val = 0;
        SHR_BITCOPY_RANGE(&fld_val, 0, asso_data_buffer, (SOC_SAND_NOF_BITS_IN_BYTE * byte_ndx), SOC_SAND_NOF_BITS_IN_BYTE);
        asso_data[byte_array_idx] = (uint8) (fld_val & 0xFF);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_pp_kaps_route_to_kaps_payload_buffer_encode()",0,0);
}

uint32
  jer_pp_kaps_route_to_kaps_payload_buffer_decode(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint8                      *asso_data,
    SOC_SAND_OUT uint32                     *asso_data_buffer
  )
{
    uint32
        byte_ndx,
        byte_array_idx,
        fld_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(asso_data_buffer);
    SOC_SAND_CHECK_NULL_INPUT(asso_data);

    for (byte_ndx = 0; byte_ndx < JER_KAPS_AD_BUFFER_NOF_BYTES; ++byte_ndx)
    {
        byte_array_idx = JER_KAPS_AD_BUFFER_NOF_BYTES - byte_ndx - 1;
        fld_val = asso_data[byte_array_idx];
        SHR_BITCOPY_RANGE(asso_data_buffer, (SOC_SAND_NOF_BITS_IN_BYTE * byte_ndx), &fld_val, 0, SOC_SAND_NOF_BITS_IN_BYTE);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_pp_kaps_route_buffer_to_kaps_buffer_decode()",0,0);
}

/*if qual_vals==NULL then it uses private_table to determine whether it's private or public*/
uint32 
  jer_pp_kaps_dbal_table_id_translate(SOC_SAND_IN  int                       unit, 
                                      SOC_SAND_IN  SOC_DPP_DBAL_SW_TABLE_IDS table_id, 
                                      SOC_SAND_IN  ARAD_PP_FP_QUAL_VAL       qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX],
                                      SOC_SAND_IN  uint8                     private_table, 
                                      SOC_SAND_OUT JER_KAPS_IP_TBL_ID       *kaps_table_id)
{
    uint32 vrf;
    int i;

    SOCDNX_INIT_FUNC_DEFS;

    if (qual_vals == NULL) {
        /*if private_table == 0 then this is a public table*/
        vrf = private_table;
    } else {
        /*find the vrf in qual_vals to distinguish between public and private tables*/
        for (i=0; i < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; i++) {
            if (qual_vals[i].type == SOC_PPC_FP_QUAL_IRPP_VRF) {
                vrf = qual_vals[i].val.arr[0];
                break;
            }
        }
    }

    if (i == SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_flp_kaps_table_id_translate - no vrf field found")));
    }

    if (vrf == 0) {
        /*public tables*/
        switch (table_id) {
        case SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS:
            *kaps_table_id = JER_KAPS_IP_FWD_PUBLIC_IPV4_UC_TBL_ID;
            break;
        case SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_KAPS:
            *kaps_table_id = JER_KAPS_IP_FWD_PUBLIC_IPV4_UC_TBL_ID;
            break;
        case SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS:
            *kaps_table_id = JER_KAPS_IP_FWD_PUBLIC_IPV4_MC_TBL_ID;
            break;
        case SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_KAPS:
            *kaps_table_id = JER_KAPS_IP_FWD_PUBLIC_IPV6_UC_TBL_ID;
            break;
        case SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_RPF_KAPS:
            *kaps_table_id = JER_KAPS_IP_FWD_PUBLIC_IPV6_UC_TBL_ID;
            break;
        case SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_KAPS:
            *kaps_table_id = JER_KAPS_IP_FWD_PUBLIC_IPV6_MC_TBL_ID;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_flp_kaps_table_id_translate - invalid kaps table_id")));
        }
    } else {
        /*private tables*/
        switch (table_id) {
        case SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS:
            *kaps_table_id = JER_KAPS_IP_FWD_PRIVATE_IPV4_UC_TBL_ID;
            break;
        case SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_KAPS:
            *kaps_table_id = JER_KAPS_IP_FWD_PRIVATE_IPV4_UC_TBL_ID;
            break;
        case SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS:
            *kaps_table_id = JER_KAPS_IP_FWD_PRIVATE_IPV4_MC_TBL_ID;
            break;
        case SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_KAPS:
            *kaps_table_id = JER_KAPS_IP_FWD_PRIVATE_IPV6_UC_TBL_ID;
            break;
        case SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_RPF_KAPS:
            *kaps_table_id = JER_KAPS_IP_FWD_PRIVATE_IPV6_UC_TBL_ID;
            break;
        case SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_KAPS:
            *kaps_table_id = JER_KAPS_IP_FWD_PRIVATE_IPV6_MC_TBL_ID;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_flp_kaps_table_id_translate - invalid kaps table_id")));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32 jer_pp_kaps_entry_add(SOC_SAND_IN    int unit, 
                             SOC_SAND_IN    uint8 tbl_id, 
                             SOC_SAND_IN    uint8 tbl_prefix,
                             SOC_SAND_IN    uint8 nof_quals,
                             SOC_SAND_IN    uint64 *qual_vals,
                             SOC_SAND_IN    uint8 *qual_nof_bits,
                             SOC_SAND_IN    uint8 prefix_len,
                             SOC_SAND_IN    uint32 action,
                             SOC_SAND_OUT   SOC_SAND_SUCCESS_FAILURE    *success)
{
    int32 
        res,i,
        data_prefix_len;
    uint8 data_bytes[JER_KAPS_KEY_BUFFER_NOF_BYTES];
    uint8 action_bytes[JER_KAPS_AD_BUFFER_NOF_BYTES];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *success = SOC_SAND_SUCCESS;

    sal_memset(data_bytes, 0x0, sizeof(uint8) * JER_KAPS_KEY_BUFFER_NOF_BYTES);

    data_prefix_len = prefix_len + JER_KAPS_TBL_PREFIX_NOF_BITS; /* consider table prefix */

    jer_pp_kaps_key_encode(tbl_prefix,
                           nof_quals,
                           qual_vals,
                           qual_nof_bits,
                           data_bytes);

    /* copy action to bytes buffer */
    for (i = 0; i <JER_KAPS_AD_BUFFER_NOF_BYTES; i++) {
        action_bytes[i] = action >> (UINT32_SIZE_IN_BITS - (i+1)*UINT8_SIZE_IN_BITS);
    }

    res = jer_pp_kaps_entry_add_hw( unit,      
                                    tbl_id,  
                                    data_prefix_len,
                                    data_bytes,     
                                    action_bytes, 
                                    success  
                                  ); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 00, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_pp_kaps_entry_add_hw()",0,0);
}

uint32 jer_pp_kaps_entry_remove(SOC_SAND_IN    int unit, 
                                SOC_SAND_IN    uint8 tbl_id, 
                                SOC_SAND_IN    uint8 tbl_prefix,
                                SOC_SAND_IN    uint8 nof_quals,
                                SOC_SAND_IN    uint64 *qual_vals,
                                SOC_SAND_IN    uint8 *qual_nof_bits,
                                SOC_SAND_IN    uint8 prefix_len
                                )
{
    int32 
        res,
        data_prefix_len;
    uint8 data_bytes[JER_KAPS_KEY_BUFFER_NOF_BYTES];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    sal_memset(data_bytes, 0x0, sizeof(uint8) * JER_KAPS_KEY_BUFFER_NOF_BYTES);

    data_prefix_len = prefix_len + JER_KAPS_TBL_PREFIX_NOF_BITS; /* consider table prefix */

    jer_pp_kaps_key_encode(tbl_prefix,
                           nof_quals,
                           qual_vals,
                           qual_nof_bits,
                           data_bytes);

    res = jer_pp_kaps_entry_remove_hw( unit,      
                                      tbl_id,  
                                      data_prefix_len,
                                      data_bytes 
                                     ); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 00, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_pp_kaps_entry_remove()",0,0);
}

uint32 jer_pp_kaps_entry_get(SOC_SAND_IN    int unit, 
                             SOC_SAND_IN    uint8 tbl_id, 
                             SOC_SAND_IN    uint8 tbl_prefix,
                             SOC_SAND_IN    uint8 nof_quals,
                             SOC_SAND_IN    uint64 *qual_vals,
                             SOC_SAND_IN    uint8 *qual_nof_bits,
                             SOC_SAND_IN    uint8 prefix_len,
                             SOC_SAND_OUT   uint32 *action,
                             SOC_SAND_OUT   uint8  *found)
{
    int32 
        res,
        data_prefix_len, i;
    uint32 val = 0;
    uint8 data_bytes[JER_KAPS_KEY_BUFFER_NOF_BYTES];
    uint8 action_bytes[JER_KAPS_AD_BUFFER_NOF_BYTES];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    sal_memset(data_bytes, 0x0, sizeof(uint8) * JER_KAPS_KEY_BUFFER_NOF_BYTES);
    sal_memset(action_bytes, 0x0, sizeof(uint8) * JER_KAPS_AD_BUFFER_NOF_BYTES);

    data_prefix_len = prefix_len + JER_KAPS_TBL_PREFIX_NOF_BITS; /* consider table prefix */

    jer_pp_kaps_key_encode(tbl_prefix,
                           nof_quals,
                           qual_vals,
                           qual_nof_bits,
                           data_bytes);

    res = jer_pp_kaps_entry_get_hw( unit,      
                                    tbl_id,  
                                    data_prefix_len,
                                    data_bytes,     
                                    action_bytes, 
                                    found  
                                  ); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 00, exit);

    /* translate from bytes buffer to uint32 */
    for (i = 0; i <JER_KAPS_AD_BUFFER_NOF_BYTES; i++) {
        val |= action_bytes[i]<<(UINT32_SIZE_IN_BITS - (i+1)*UINT8_SIZE_IN_BITS); 
    }
    *action = val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_pp_kaps_entry_get()",0,0);
}

uint32 jer_pp_kaps_entry_table_clear(SOC_SAND_IN    int unit, 
                                     SOC_SAND_IN    uint8 tbl_id)
{
    int32 res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = jer_pp_kaps_entry_table_clear_hw(unit,      
                                           tbl_id    
                                          ); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 00, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_pp_kaps_entry_table_clear()",0,0);
}

uint32 jer_pp_kaps_db_enabled(SOC_SAND_IN    int              unit,
                              SOC_SAND_IN    SOC_PPC_VRF_ID   vrf_ndx)
{
    int32 res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (vrf_ndx == 0) {
        if (!JER_KAPS_ENABLE_PUBLIC_DB(unit)) {
            /* VRF 0 should be retrieved from the public DB */
            res = SOC_SAND_ERR;
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        }
    }
    else {
        if (!JER_KAPS_ENABLE_PRIVATE_DB(unit)) {
            /* VRF!=0 should be retrieved from the private DB */
            res = SOC_SAND_ERR;
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_pp_kaps_entry_table_clear()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif /* defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030) */

