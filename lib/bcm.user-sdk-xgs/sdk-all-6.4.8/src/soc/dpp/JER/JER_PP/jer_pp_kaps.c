/* $Id: jer_kaps.c, hagayco Exp $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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

#include <soc/mcm/memregs.h>

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_TCAM

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <soc/mem.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_kbp.h>

#include <shared/swstate/access/sw_state_access.h>
#include <shared/swstate/sw_state.h>

#include <soc/dpp/JER/JER_PP/jer_pp_kaps.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps_xpt.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps_entry_mgmt.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps_diag.h>

#ifdef USE_MODEL
#include <soc/kbp/alg_kbp/include/model.h>
#endif

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* 
 * Offset for table IDs.
 * Defined to differentiate from DB IDs which start from ID 0. */
#define TABLE_ID_OFFSET             10

/* 
 * Value to use in clone_table_id to indicate that
 * the table handle is the same as the DB handle.    */
#define JER_KAPS_TABLE_USE_DB_HANDLE      0xFF

#define JER_KAPS_RPB_CAM_BIST_CONTROL_REG_ADDR  0x2A
#define JER_KAPS_RPB_CAM_BIST_STATUS_REG_ADDR   0x2B

#define JER_KAPS_DB_IS_PRIVATE(db_id) ((db_id == JER_KAPS_IP_FWD_PRIVATE_DB_ID) || (db_id == JER_KAPS_IP_RPF_PRIVATE_DB_ID))

#define KAPS_ACCESS sw_state_access[unit].dpp.soc.arad.pp.kaps_db


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


static
    JER_KAPS_DB_CONFIG
        jer_kaps_db_config_info_static[JER_KAPS_IP_NOF_DB] = {

            /* DB 0 (private forwarding) */ 
            {   
                FALSE, /* Valid */
                0, /* DB Size */
                JER_KAPS_IP_NOF_DB, /* Clone DB ID */
                NULL, /* DB handle */
                NULL /* AD DB handle */
            },

            /* DB 1 (private RPF) */ 
            {   
                FALSE, /* Valid */
                0, /* DB Size */
                JER_KAPS_IP_FWD_PRIVATE_DB_ID, /* Clone DB ID */
                NULL, /* DB handle */
                NULL /* AD DB handle */
            }, 

            /* DB 2 (public forwarding) */ 
            {   
                FALSE, /* Valid */
                0, /* DB Size */
                JER_KAPS_IP_NOF_DB, /* Clone DB ID */
                NULL, /* DB handle */
                NULL /* AD DB handle */
            },

            /* DB 4 (public RPF) */ 
            {   
                FALSE, /* Valid */
                0, /* DB Size */
                JER_KAPS_IP_FWD_PUBLIC_DB_ID, /* Clone DB ID */
                NULL, /* DB handle */
                NULL /* AD DB handle */
            },
        };  



static
    JER_KAPS_TABLE_CONFIG
        jer_kaps_table_config_info_static[JER_KAPS_IP_NOF_TABLES] = {

            /* Table 0 (IPv4 UC private forwarding) + Non-IP */
            {   
                JER_KAPS_IP_FWD_PRIVATE_DB_ID, /* DB ID */ 
                JER_KAPS_TABLE_USE_DB_HANDLE, /* Clone table ID */
                {3, {{"TBL_ID_0", JER_KAPS_TABLE_PREFIX_LENGTH, KBP_KEY_FIELD_TABLE_ID}, {"IPV4: VRF_0 + DIP_0", 14 + 32 + (112 /*padding to 160b*/), KBP_KEY_FIELD_PREFIX}}}, /* key_fields */
                NULL /* table handle */
            },

            /* Table 1 (IPv4 UC private RPF)  + Non-IP */
            {   
                JER_KAPS_IP_RPF_PRIVATE_DB_ID, /* DB ID */ 
                JER_KAPS_TABLE_USE_DB_HANDLE, /* Clone table ID */
                {3, {{"TBL_ID_1", JER_KAPS_TABLE_PREFIX_LENGTH, KBP_KEY_FIELD_TABLE_ID}, {"IPV4: VRF_1 + SIP_1", 14 + 32 + (112 /*padding to 160b*/), KBP_KEY_FIELD_PREFIX}}}, /* key_fields */
                NULL /* table handle */
            }, 

            /* Table 2 (IPv6 UC private forwarding) */ 
            {   
                JER_KAPS_IP_FWD_PRIVATE_DB_ID, /* DB ID */ 
                JER_KAPS_IP_NOF_TABLES, /* Clone table ID */
                {3, {{"TBL_ID_2", JER_KAPS_TABLE_PREFIX_LENGTH, KBP_KEY_FIELD_TABLE_ID}, {"VRF_2", 14, KBP_KEY_FIELD_EM}, {"DIP_2", 128+ (16 /*padding to 160b*/), KBP_KEY_FIELD_PREFIX}}}, /* key_fields */
                NULL /* table handle */
            },

            /* Table 3 (IPv6 UC private RPF) */ 
            {   
                JER_KAPS_IP_RPF_PRIVATE_DB_ID, /* DB ID */ 
                JER_KAPS_IP_FWD_PRIVATE_IPV6_UC_TBL_ID, /* Clone table ID */
                {3, {{"TBL_ID_3", JER_KAPS_TABLE_PREFIX_LENGTH, KBP_KEY_FIELD_TABLE_ID}, {"VRF_3", 14, KBP_KEY_FIELD_EM}, {"SIP_3", 128+ (16 /*padding to 160b*/), KBP_KEY_FIELD_PREFIX}}}, /* key_fields */
                NULL /* table handle */
            }, 

            /* Table 4 (IPv4 MC private forwarding) */ 
            {   
                JER_KAPS_IP_FWD_PRIVATE_DB_ID, /* DB ID */ 
                JER_KAPS_IP_NOF_TABLES, /* Clone table ID                                                         MC-GROUP + SIP + IN-RIF */
                {3, {{"TBL_ID_4", JER_KAPS_TABLE_PREFIX_LENGTH, KBP_KEY_FIELD_TABLE_ID}, {"VRF_4", 14, KBP_KEY_FIELD_EM}, {"MC-G-SIP-INRIF_4", 28 + 32 + 15 + (69 /*padding to 160b*/), KBP_KEY_FIELD_PREFIX}}}, /* key_fields */
                NULL /* table handle */
            },

            /* Table 5 (IPv6 MC private forwarding) */ 
            {   
                JER_KAPS_IP_FWD_PRIVATE_DB_ID, /* DB ID */ 
                JER_KAPS_IP_NOF_TABLES, /* Clone table ID                                                                                      INRIF  */
                {4, {{"TBL_ID_5", JER_KAPS_TABLE_PREFIX_LENGTH, KBP_KEY_FIELD_TABLE_ID}, {"VRF_5", 14, KBP_KEY_FIELD_EM}, {"MC-GROUP_5", 128, KBP_KEY_FIELD_EM}, {"INRIF_5", 15 + (1 /*padding to 160b*/), KBP_KEY_FIELD_PREFIX}}}, /* key_fields */
                NULL /* table handle */
            },

            /* Table 6 (IPv4 UC public forwarding)  + Non-IP */
            {   
                JER_KAPS_IP_FWD_PUBLIC_DB_ID, /* DB ID */ 
                JER_KAPS_TABLE_USE_DB_HANDLE, /* Clone table ID */
                {3, {{"TBL_ID_6", JER_KAPS_TABLE_PREFIX_LENGTH, KBP_KEY_FIELD_TABLE_ID}, {"IPV4: VRF_6 + DIP_6", 14 + 32 + (112 /*padding to 160b*/), KBP_KEY_FIELD_PREFIX}}}, /* key_fields */
                NULL /* table handle */
            },

            /* Table 7 (IPv4 UC public RPF)  + Non-IP */
            {   
                JER_KAPS_IP_RPF_PUBLIC_DB_ID, /* DB ID */ 
                JER_KAPS_TABLE_USE_DB_HANDLE, /* Clone table ID */
                {3, {{"TBL_ID_7", JER_KAPS_TABLE_PREFIX_LENGTH, KBP_KEY_FIELD_TABLE_ID}, {"IPV4: VRF_7 + SIP_7", 14 + 32 + (112 /*padding to 160b*/), KBP_KEY_FIELD_PREFIX}}}, /* key_fields */
                NULL /* table handle */
            }, 

            /* Table 8 (IPv6 UC public forwarding) */ 
            {   
                JER_KAPS_IP_FWD_PUBLIC_DB_ID, /* DB ID */ 
                JER_KAPS_IP_NOF_TABLES, /* Clone table ID */
                {3, {{"TBL_ID_8", JER_KAPS_TABLE_PREFIX_LENGTH, KBP_KEY_FIELD_TABLE_ID}, {"VRF_8", 14, KBP_KEY_FIELD_EM}, {"DIP_8", 128 + (16 /*padding to 160b*/), KBP_KEY_FIELD_PREFIX}}}, /* key_fields */
                NULL /* table handle */
            },

            /* Table 9 (IPv6 UC public RPF) */ 
            {   
                JER_KAPS_IP_RPF_PUBLIC_DB_ID, /* DB ID */ 
                JER_KAPS_IP_FWD_PUBLIC_IPV6_UC_TBL_ID, /* Clone table ID */
                {3, {{"TBL_ID_9", JER_KAPS_TABLE_PREFIX_LENGTH, KBP_KEY_FIELD_TABLE_ID}, {"VRF_9", 14, KBP_KEY_FIELD_EM}, {"SIP_9", 128 + (16 /*padding to 160b*/), KBP_KEY_FIELD_PREFIX}}}, /* key_fields */
                NULL /* table handle */
            }, 

            /* Table 10 (IPv4 MC public forwarding) */ 
            {   
                JER_KAPS_IP_FWD_PUBLIC_DB_ID, /* DB ID */ 
                JER_KAPS_IP_NOF_TABLES, /* Clone table ID                                                           MC-GROUP + SIP + VRF */
                {3, {{"TBL_ID_10", JER_KAPS_TABLE_PREFIX_LENGTH, KBP_KEY_FIELD_TABLE_ID}, {"VRF_10", 14, KBP_KEY_FIELD_EM}, {"MC-G-SIP-INRIF_10", 28 + 32 + 15 + (69 /*padding to 160b*/), KBP_KEY_FIELD_PREFIX}}}, /* key_fields */
                NULL /* table handle */
            },

            /* Table 11 (IPv6 MC public forwarding) */ 
            {   
                JER_KAPS_IP_FWD_PUBLIC_DB_ID, /* DB ID */ 
                JER_KAPS_IP_NOF_TABLES, /* Clone table ID                                                                                          INRIF */
                {4, {{"TBL_ID_11", JER_KAPS_TABLE_PREFIX_LENGTH, KBP_KEY_FIELD_TABLE_ID}, {"VRF_11", 14, KBP_KEY_FIELD_EM}, {"MC-GROUP_11", 128, KBP_KEY_FIELD_EM}, {"INRIF_11", 15 + (1 /*padding to 160b*/), KBP_KEY_FIELD_PREFIX}}}, /* key_fields */
                NULL /* table handle */
            },
        };  

static 
    JER_KAPS_SEARCH_CONFIG
        jer_kaps_search_config_info_static[JER_KAPS_NOF_SEARCHES] = {

            /* Search 0: IPv4 UC */
            {
                FALSE, /* valid */
                {JER_KAPS_IP_RPF_PRIVATE_IPV4_UC_TBL_ID, JER_KAPS_IP_RPF_PUBLIC_IPV4_UC_TBL_ID, JER_KAPS_IP_FWD_PRIVATE_IPV4_UC_TBL_ID, JER_KAPS_IP_FWD_PUBLIC_IPV4_UC_TBL_ID}, /* tbl_id's*/

                NULL /* JER_KAPS_INSTRUCTION */
            }, 

            /* Search 1: IPv6 UC */
            {
                FALSE, /* valid */
                {JER_KAPS_IP_RPF_PRIVATE_IPV6_UC_TBL_ID, JER_KAPS_IP_RPF_PUBLIC_IPV6_UC_TBL_ID, JER_KAPS_IP_FWD_PRIVATE_IPV6_UC_TBL_ID, JER_KAPS_IP_FWD_PUBLIC_IPV6_UC_TBL_ID}, /* tbl_id's*/
                NULL /* JER_KAPS_INSTRUCTION */
            }, 

            /* Search 2: IPv4 MC */
            {
                FALSE, /* valid */
                {JER_KAPS_IP_RPF_PRIVATE_IPV4_UC_TBL_ID, JER_KAPS_IP_RPF_PUBLIC_IPV4_UC_TBL_ID, JER_KAPS_IP_FWD_PRIVATE_IPV4_MC_TBL_ID, JER_KAPS_IP_FWD_PUBLIC_IPV4_MC_TBL_ID}, /* tbl_id's*/
                NULL /* JER_KAPS_INSTRUCTION */
            },  

            /* Search 3: IPv6 MC */
            {
                FALSE, /* valid */
                {JER_KAPS_IP_RPF_PRIVATE_IPV6_UC_TBL_ID, JER_KAPS_IP_RPF_PUBLIC_IPV6_UC_TBL_ID, JER_KAPS_IP_FWD_PRIVATE_IPV6_MC_TBL_ID, JER_KAPS_IP_FWD_PUBLIC_IPV6_MC_TBL_ID}, /* tbl_id's*/
                NULL /* JER_KAPS_INSTRUCTION */
            }, 
        };

static
    JER_KAPS_DB_CONFIG
        jer_kaps_db_config_info[SOC_SAND_MAX_DEVICE][JER_KAPS_IP_NOF_DB];

static
    JER_KAPS_TABLE_CONFIG
        jer_kaps_table_config_info[SOC_SAND_MAX_DEVICE][JER_KAPS_IP_NOF_TABLES];

static 
    JER_KAPS_SEARCH_CONFIG
        jer_kaps_search_config_info[SOC_SAND_MAX_DEVICE][JER_KAPS_NOF_SEARCHES];
static 
    genericJerAppData 
        *JerAppData[SOC_SAND_MAX_DEVICE] = {NULL};

static
    JER_KAPS_DB_CONFIG
        jer_kaps_dma_db_config_info[SOC_SAND_MAX_DEVICE][JER_KAPS_NOF_MAX_DMA_DB];

static kbp_warmboot_t kaps_warmboot_data[SOC_SAND_MAX_DEVICE];

const char* JER_KAPS_TABLE_NAMES[] = {"FWD_PRIVATE_IPV4_UC",
                                      "RPF_PRIVATE_IPV4_UC",
                                      "FWD_PRIVATE_IPV6_UC",
                                      "RPF_PRIVATE_IPV6_UC",
                                      "FWD_PRIVATE_IPV4_MC",
                                      "FWD_PRIVATE_IPV6_MC",
                                      "FWD_PUBLIC_IPV4_UC ",
                                      "RPF_PUBLIC_IPV4_UC ",
                                      "FWD_PUBLIC_IPV6_UC ",
                                      "RPF_PUBLIC_IPV6_UC ",
                                      "FWD_PUBLIC_IPV4_MC ",
                                      "FWD_PUBLIC_IPV6_MC "};

const char* JER_KAPS_DB_NAMES[] = {"PRIVATE_FORWARDING", "PRIVATE_RPF", "PUBLIC_FORWARDING", "PUBLIC_RPF"};

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */


void jer_kaps_sw_init(int unit)
{
    uint32
        db_id,
        db_idx,
        tbl_id,
        tbl_idx,
        search_idx,
        db_size;
    uint8 
        valid;    
    soc_dpp_config_jer_pp_t *jer_pp_config = &(SOC_DPP_JER_CONFIG(unit)->pp);

    /* Initialize data structures with static configuration */
    sal_memcpy(&jer_kaps_db_config_info[unit][0], &jer_kaps_db_config_info_static[0], sizeof(JER_KAPS_DB_CONFIG)*JER_KAPS_IP_NOF_DB);
    sal_memcpy(&jer_kaps_table_config_info[unit][0], &jer_kaps_table_config_info_static[0], sizeof(JER_KAPS_TABLE_CONFIG)*JER_KAPS_IP_NOF_TABLES);
    sal_memcpy(&jer_kaps_search_config_info[unit][0], &jer_kaps_search_config_info_static[0], sizeof(JER_KAPS_SEARCH_CONFIG)*JER_KAPS_NOF_SEARCHES);
    sal_memset(&jer_kaps_dma_db_config_info[unit][0], 0x0, sizeof(JER_KAPS_DB_CONFIG)*JER_KAPS_NOF_MAX_DMA_DB);

    /* First set table sizes according to configuration */
    for ( db_idx = 0; 
          db_idx < JER_KAPS_IP_NOF_DB; 
          db_idx++ ) 
    {
        if (JER_KAPS_DB_IS_PRIVATE(db_idx))
        {
            db_size = jer_pp_config->kaps_private_ip_frwrd_table_size;
        }
        else
        {
            db_size = jer_pp_config->kaps_public_ip_frwrd_table_size;
        }

        if (db_size > 0) 
        {
            jer_kaps_db_config_info[unit][db_idx].valid = TRUE;
            jer_kaps_db_config_info[unit][db_idx].db_size = db_size;
        }   
    }

    /* Now verify all tables used by each search are valid */
    for(search_idx = 0; search_idx < JER_KAPS_NOF_SEARCHES; search_idx++) 
    {
        /* Verify instruction has tables configured */
        valid = TRUE;

        for ( tbl_idx = 0; tbl_idx < JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES; tbl_idx++) 
        {
            /* If one of the tables is not valid, then instruction is not valid */
            tbl_id = jer_kaps_search_config_info[unit][search_idx].tbl_id[tbl_idx];
            db_id = jer_kaps_table_config_info[unit][tbl_id].db_id;
            valid &= jer_kaps_db_config_info[unit][db_id].valid;
        }

        jer_kaps_search_config_info[unit][search_idx].valid = valid;
    }

    /* Set number of Direct access DB*/
    if (jer_pp_config->kaps_large_db_size) {
        for ( db_idx = 0; 
              db_idx <= ( jer_pp_config->kaps_large_db_size / JER_KAPS_DMA_DB_NOF_ENTRIES ); 
              db_idx++ ) 
        {
              jer_kaps_dma_db_config_info[unit][db_idx].valid = TRUE;
              jer_kaps_dma_db_config_info[unit][db_idx].db_size = JER_KAPS_DMA_DB_NOF_ENTRIES;

        }
    }
}

/* This function performs Device Inits */
STATIC int jer_kaps_init_device(int unit)
{
    uint32 flags;
    kbp_warmboot_t *warmboot_data;
    int32 res;
    uint8 is_allocated;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    JerAppData[unit]->kaps_xpt_p = NULL;
#ifdef USE_MODEL
    /* Initialize the C-Model */
    res = kbp_sw_model_init(JerAppData[unit]->dalloc_p, KBP_DEVICE_KAPS, KBP_DEVICE_DEFAULT,
                              NULL, &(JerAppData[unit]->kaps_xpt_p));
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "Error in %s(): kbp_sw_model_init with failed: %s!\n"), 
                             FUNCTION_NAME(),
                  kbp_get_status_string(res)));
    }
#else
#ifndef BLACKHOLE_MODE
    res = jer_pp_xpt_init(unit, &(JerAppData[unit]->kaps_xpt_p));
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
#endif
#endif

    warmboot_data = &kaps_warmboot_data[unit];

    flags = KBP_DEVICE_DEFAULT | KBP_DEVICE_ISSU;
    if (SOC_WARM_BOOT(unit)) {
        flags |= KBP_DEVICE_SKIP_INIT;
    }
    KBP_TRY(kbp_device_init(JerAppData[unit]->dalloc_p, 
                            KBP_DEVICE_KAPS, 
                            flags, 
                            (struct kaps_xpt*)JerAppData[unit]->kaps_xpt_p,
                            NULL,
                            &JerAppData[unit]->kaps_device_p));

    if (SOC_WARM_BOOT(unit)) {
       res = kbp_device_restore_state(JerAppData[unit]->kaps_device_p, warmboot_data->kbp_file_read , warmboot_data->kbp_file_write, warmboot_data->kbp_file_fp);
       if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
           LOG_ERROR(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "Error in %s(): failed with error: %s!\n"), 
                                 FUNCTION_NAME(),
                      kbp_get_status_string(res)));
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 90, exit);
       }
    }
    else {
        res = KAPS_ACCESS.is_allocated(unit, &is_allocated);
        SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

        if(!is_allocated) {
            res = KAPS_ACCESS.alloc(unit);
            SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
        }
    }

    ARAD_DO_NOTHING_AND_EXIT;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_kaps_init_nlm_dev_mgr()", 0, 0);
}


STATIC uint32 jer_kaps_db_init(int unit, uint32 db_id)
{
    uint32 
        res = SOC_SAND_OK;

    JER_KAPS_DB_CONFIG
        *db_config_info = &(jer_kaps_db_config_info[unit][db_id]);

    JER_KAPS_DB_HANDLES
        db_handles_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Check if table was already created */
    if(NULL != db_config_info->db_p) {
        ARAD_DO_NOTHING_AND_EXIT;
    }

    if (!SOC_WARM_BOOT(unit)) {
       LOG_VERBOSE(BSL_LS_SOC_TCAM,
                   (BSL_META_U(unit,
                               "%s(): DB [%d] : create DB with db-size=%d.\n"),
                               FUNCTION_NAME(),
                    db_id,
                    db_config_info->db_size));

        if (db_config_info->clone_of_db_id == JER_KAPS_IP_NOF_DB) {
            /* Not a clone. Create the DB */
            res = kbp_db_init(
                    JerAppData[unit]->kaps_device_p, 
                    KBP_DB_LPM, 
                    db_id,
                    db_config_info->db_size, 
                    &db_config_info->db_p
                  );
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
               LOG_ERROR(BSL_LS_SOC_TCAM,
                         (BSL_META_U(unit,
                                     "Error in %s(): kbp_db_init failed with : %s!\n"), 
                                     FUNCTION_NAME(),
                          kbp_get_status_string(res)));

                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
            }

            /* Associate DB with AD table */
            LOG_VERBOSE(BSL_LS_SOC_TCAM,
                        (BSL_META_U(unit,
                                    "%s(): DB [%d] : create AD DB with asso-data=%d.\n"),
                                    FUNCTION_NAME(),
                         db_id,
                         JER_KAPS_AD_WIDTH_IN_BITS));

            KBP_TRY(kbp_ad_db_init(
                        JerAppData[unit]->kaps_device_p, 
                        db_id,
                        db_config_info->db_size, 
                        JER_KAPS_AD_WIDTH_IN_BITS,
                        &db_config_info->ad_db_p)
                    );

            KBP_TRY(kbp_db_set_ad(db_config_info->db_p,
                                  db_config_info->ad_db_p));

            /* save AD DB handle in case of not clone */
            db_handles_info.ad_db_p = db_config_info->ad_db_p;
        }
        else {
            KBP_TRY(kbp_db_clone(jer_kaps_db_config_info[unit][db_config_info->clone_of_db_id].db_p,
                                 db_id,
                                 &(db_config_info->db_p))
                    );
        }
        /* save DB handles */
        db_handles_info.db_p    = db_config_info->db_p;
        db_handles_info.is_valid = TRUE;
        res = KAPS_ACCESS.db_info.set(unit, db_id, db_handles_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        ARAD_DO_NOTHING_AND_EXIT;
    }
    else {
        res = KAPS_ACCESS.db_info.get(unit, db_id, &db_handles_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        KBP_TRY(kbp_db_refresh_handle(JerAppData[unit]->kaps_device_p, db_handles_info.db_p, &db_handles_info.db_p));
        KBP_TRY(kbp_ad_db_refresh_handle(JerAppData[unit]->kaps_device_p, db_handles_info.ad_db_p, &db_handles_info.ad_db_p));

        jer_kaps_db_config_info[unit][db_id].db_p = db_handles_info.db_p;
        jer_kaps_db_config_info[unit][db_id].ad_db_p = db_handles_info.ad_db_p;

        res = KAPS_ACCESS.db_info.set(unit, db_id, db_handles_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }
    

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_kaps_db_init()", db_id, 0);
}


STATIC uint32 jer_kaps_dma_db_init(int unit, uint32 db_id)
{
    uint32 
        res = SOC_SAND_OK;

    JER_KAPS_DB_CONFIG
        *db_config_info = &(jer_kaps_dma_db_config_info[unit][db_id]);


    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Check if table was already created */
    if(NULL != db_config_info->dma_db_p) {
        ARAD_DO_NOTHING_AND_EXIT;
    }

    res = kbp_dma_db_init(
            JerAppData[unit]->kaps_device_p, 
            db_id,
            db_config_info->db_size,
            JER_KAPS_DMA_DB_WIDTH, 
            &db_config_info->dma_db_p
          );
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "Error in %s(): kbp_db_init failed with : %s!\n"), 
                             FUNCTION_NAME(),
                  kbp_get_status_string(res)));

        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_kaps_db_init()", db_id, 0);
}


STATIC uint32 jer_kaps_table_init(int unit, uint32 tbl_id)
{
    uint32 
        key_ndx,
        db_id,
        res = SOC_SAND_OK;

    JER_KAPS_KEY *key;

    JER_KAPS_TABLE_CONFIG
        *tbl_config_info = &(jer_kaps_table_config_info[unit][tbl_id]);

    JER_KAPS_TABLE_HANDLES
        table_handle;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Check if table was already created */
    if(NULL != tbl_config_info->tbl_p) {
        ARAD_DO_NOTHING_AND_EXIT;
    }

    if (!SOC_WARM_BOOT(unit)) 
    {
        /* Call the Add Table API to create each of the tables */
       LOG_VERBOSE(BSL_LS_SOC_TCAM,
                   (BSL_META_U(unit,
                               "%s(): Table [%d] : create table.\n"),
                               FUNCTION_NAME(),
                    tbl_id));

        if (tbl_config_info->clone_of_tbl_id == JER_KAPS_TABLE_USE_DB_HANDLE) 
        {
            /* Use DB handle. Don't add a table in this case. */
            db_id = tbl_config_info->db_id;
            tbl_config_info->tbl_p = jer_kaps_db_config_info[unit][db_id].db_p;
        }
        else if (tbl_config_info->clone_of_tbl_id == JER_KAPS_IP_NOF_TABLES) 
        {
            /* Not a clone. Create the table */
            db_id = tbl_config_info->db_id;
            res = kbp_db_add_table(
                    jer_kaps_db_config_info[unit][db_id].db_p, 
                    tbl_id + TABLE_ID_OFFSET, 
                    &(tbl_config_info->tbl_p)
                  );
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
               LOG_ERROR(BSL_LS_SOC_TCAM,
                         (BSL_META_U(unit,
                                     "Error in %s(): kbp_db_add_table failed with : %s!\n"), 
                                     FUNCTION_NAME(),
                          kbp_get_status_string(res)));
           
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
            }
        }
        else
        {
            res = kbp_db_clone(jer_kaps_table_config_info[unit][tbl_config_info->clone_of_tbl_id].tbl_p,
                                 tbl_id + TABLE_ID_OFFSET,
                                 &(tbl_config_info->tbl_p));
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
               LOG_ERROR(BSL_LS_SOC_TCAM,
                         (BSL_META_U(unit,
                                     "Error in %s(): kbp_db_clone failed with : %s!\n"), 
                                     FUNCTION_NAME(),
                          kbp_get_status_string(res)));

                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
            }
                    
        }

        /* key creation */
        KBP_TRY(kbp_key_init(JerAppData[unit]->kaps_device_p, &key));

        /* Add fields to key */
        for(key_ndx = 0;
             key_ndx < tbl_config_info->key_fields.nof_key_segments;
             key_ndx++)
        {
            /*Create a unique field name*/
            if(tbl_config_info->key_fields.key_segment[key_ndx].nof_bits){

                res = kbp_key_add_field(
                        key, 
                        tbl_config_info->key_fields.key_segment[key_ndx].name,
                        tbl_config_info->key_fields.key_segment[key_ndx].nof_bits, 
                        tbl_config_info->key_fields.key_segment[key_ndx].type
                      );

                if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                    if(res != KBP_DUPLICATE_KEY_FIELD) {
                       LOG_ERROR(BSL_LS_SOC_TCAM,
                                 (BSL_META_U(unit,
                                             "Error in %s(): Key Resp Code = %d ,kbp_key_add_field failed: %s!\n"), 
                                             FUNCTION_NAME(),
                                  res,
                                  kbp_get_status_string(res)));

                        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 30, exit);
                    }
                }
            }
        }

        res = kbp_db_set_key(
                tbl_config_info->tbl_p,
                key
              );
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
           LOG_ERROR(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "Error in %s(): Table Key : kbp_db_set_key with failed: %s, table_id = %d!\n"), 
                                 FUNCTION_NAME(),
                      kbp_get_status_string(res), tbl_id));

            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
        }

        table_handle.tbl_p = tbl_config_info->tbl_p;
        res = KAPS_ACCESS.table_handles.set(unit, tbl_id, table_handle);
        SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

    } else {
        res = KAPS_ACCESS.table_handles.get(unit, tbl_id, &table_handle);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        KBP_TRY(kbp_db_refresh_handle(JerAppData[unit]->kaps_device_p, table_handle.tbl_p, &(table_handle.tbl_p)));

        tbl_config_info->tbl_p = table_handle.tbl_p;

        res = KAPS_ACCESS.table_handles.set(unit, tbl_id, table_handle);
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_kaps_table_init()", tbl_id, 0);
}

STATIC uint32 jer_kaps_search_init(int unit, uint32 search_id)
{
    uint32
        key_ndx,
        tbl_idx,
        tbl_id,
        res = SOC_SAND_OK;

    JER_KAPS_KEY *master_key;

    JER_KAPS_TABLE_CONFIG
        *tbl_config_info = NULL;

    JER_KAPS_SEARCH_CONFIG
        *search_config_info = &(jer_kaps_search_config_info[unit][search_id]);

    JER_KAPS_INSTRUCTION_HANDLES
        instruction_handles_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (!SOC_WARM_BOOT(unit)) {
        /* create a new instruction */
        LOG_VERBOSE(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                                "%s() : create a new instruction, search ID %d.\n"),
                                FUNCTION_NAME(),
                     search_id));

        KBP_TRY(kbp_instruction_init(
                    JerAppData[unit]->kaps_device_p, 
                    search_id,
                    0, 
                    &(search_config_info->inst_p))
                );

        /* Master key creation */
        KBP_TRY(kbp_key_init(JerAppData[unit]->kaps_device_p, &master_key));

        /* Add fields to the master key */
        for (tbl_idx = 0; tbl_idx < JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES; tbl_idx++)
        {
            tbl_id = search_config_info->tbl_id[tbl_idx];
            tbl_config_info = &(jer_kaps_table_config_info[unit][tbl_id]);

            /* Add fields to key */
            for(key_ndx = 0;
                 key_ndx < tbl_config_info->key_fields.nof_key_segments;
                 key_ndx++)
            {
                /*Create a unique field name*/
                if(tbl_config_info->key_fields.key_segment[key_ndx].nof_bits){

                    res = kbp_key_add_field(
                            master_key, 
                            tbl_config_info->key_fields.key_segment[key_ndx].name,
                            tbl_config_info->key_fields.key_segment[key_ndx].nof_bits, 
                            tbl_config_info->key_fields.key_segment[key_ndx].type
                          );

                    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                        if(res != KBP_DUPLICATE_KEY_FIELD) {
                           LOG_ERROR(BSL_LS_SOC_TCAM,
                                     (BSL_META_U(unit,
                                                 "Error in %s(): Key Resp Code = %d ,kbp_key_add_field failed: %s!\n"), 
                                                 FUNCTION_NAME(),
                                      res,
                                      kbp_get_status_string(res)));

                            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 30, exit);
                        }
                    }
                }
            }
        }

        KBP_TRY(kbp_instruction_set_key(search_config_info->inst_p, master_key));

        for (tbl_idx = 0; tbl_idx < JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES; tbl_idx++) 
        {
            tbl_id = search_config_info->tbl_id[tbl_idx];
            res = kbp_instruction_add_db(search_config_info->inst_p,
                                         jer_kaps_table_config_info[unit][tbl_id].tbl_p,
                                         tbl_idx);
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
               LOG_ERROR(BSL_LS_SOC_TCAM,
                         (BSL_META_U(unit,
                                     "Error in %s(): kbp_instruction_add_db failed with : %s, tbl_id = %d result = %d!\n"), 
                                     FUNCTION_NAME(),
                          kbp_get_status_string(res), tbl_id, tbl_idx));

                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
            }
        }

        res = kbp_instruction_install(search_config_info->inst_p);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
           LOG_ERROR(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "Error in %s(): kbp_instruction_install failed with : %s, search_id = %d!\n"), 
                                 FUNCTION_NAME(),
                      kbp_get_status_string(res), search_id));

            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
        }

        instruction_handles_info.inst_p = search_config_info->inst_p;

        res = KAPS_ACCESS.search_instruction_info.set(unit, search_id, instruction_handles_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    } else {
        res = KAPS_ACCESS.search_instruction_info.get(unit, search_id, &instruction_handles_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        KBP_TRY(kbp_instruction_refresh_handle(JerAppData[unit]->kaps_device_p, instruction_handles_info.inst_p, &instruction_handles_info.inst_p));

        search_config_info->inst_p = instruction_handles_info.inst_p;

        res = KAPS_ACCESS.search_instruction_info.set(unit, search_id, instruction_handles_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_kaps_search_init()", search_id, 0);
}

STATIC
    int jer_kaps_device_lock_config(
        SOC_SAND_IN  int unit
    )
{
    uint32 res = SOC_SAND_OK;
    res = kbp_device_lock(JerAppData[unit]->kaps_device_p);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "Error in %s(): kbp_device_lock with failed: %s!\n"), 
                             FUNCTION_NAME(),
                  kbp_get_status_string(res)));
    }

    return res;
}

uint32 jer_kaps_init_db_set(int unit)
{
    uint32
        tbl_idx,
        search_idx,
        db_idx,
        db_id,
        res;

    uint8 is_valid;

    JER_KAPS_DB_HANDLES
        db_handles_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Configure DBs */
    for (db_idx = 0; db_idx < JER_KAPS_IP_NOF_DB; db_idx++) 
    {
        if (SOC_WARM_BOOT(unit)) {
            res = KAPS_ACCESS.db_info.get(unit, db_idx, &db_handles_info);
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

            is_valid = db_handles_info.is_valid;
        } 
        else 
        {
            is_valid = jer_kaps_db_config_info[unit][db_idx].valid; 
        }

        if (is_valid) 
        {
            res = jer_kaps_db_init(
                    unit,  
                    db_idx
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
        }
    }

    /* Configure tables */
    for (tbl_idx = 0; tbl_idx < JER_KAPS_IP_NOF_TABLES; tbl_idx++) 
    {
        db_id = jer_kaps_table_config_info[unit][tbl_idx].db_id;
        is_valid = jer_kaps_db_config_info[unit][db_id].valid;
        if (is_valid)
        {
            res = jer_kaps_table_init(unit, tbl_idx);
            SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
        }
    }

    /* Configure search keys in KAPS */
    for(search_idx = 0; search_idx < JER_KAPS_NOF_SEARCHES; search_idx++) 
    {
        if (jer_kaps_search_config_info[unit][search_idx].valid) 
        {
            /* Configure Search */
            res = jer_kaps_search_init(
                    unit, 
                    search_idx
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);
        }
    }

    /* Configure direct Aceess DBs*/
    for (db_idx = 0; db_idx < JER_KAPS_NOF_MAX_DMA_DB; db_idx++) 
    {
         is_valid = jer_kaps_dma_db_config_info[unit][db_idx].valid; 

         if (is_valid) 
         {
            res = jer_kaps_dma_db_init(
                    unit,  
                    db_idx
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
         }

    }

    res = jer_kaps_device_lock_config(unit);
    if(res != 0) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
             (BSL_META_U(unit,
                         "Error in %s(): %s!\n"), 
                         FUNCTION_NAME(),
              kbp_get_status_string(res)));
    }
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_kaps_init_db_set()", 0, 0);
}

/* 
 * KAPS application init function. 
 * Called from PP Mgmt. 
 */
uint32 jer_kaps_init_app(int unit)
{
    uint32 res;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (JerAppData[unit] == NULL) {
        ARAD_ALLOC_ANY_SIZE(JerAppData[unit], genericJerAppData, 1,"JerAppData[unit]");
        sal_memset(JerAppData[unit], 0x0, sizeof(genericJerAppData));
    }
    else {
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "Error in %s(): kaps is already initialized.\n"), FUNCTION_NAME()));
        ARAD_DO_NOTHING_AND_EXIT;
    }

    /* Create the default allocator */
    KBP_TRY(default_allocator_create(&JerAppData[unit]->dalloc_p));

    /* Initialize Device now */
    res = jer_kaps_init_device(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    /* Create KAPS DB, config searches */
    res = jer_kaps_init_db_set(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_kaps_init_app()", 0, 0);
}

STATIC
uint32 jer_kaps_deinit_db_set(
    int unit)
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Set all configuration table to 0 */
    sal_memset(&jer_kaps_db_config_info[unit][0], 0, sizeof(JER_KAPS_DB_CONFIG)*JER_KAPS_IP_NOF_DB);
    sal_memset(&jer_kaps_table_config_info[unit][0], 0, sizeof(JER_KAPS_TABLE_CONFIG)*JER_KAPS_IP_NOF_TABLES);
    sal_memset(&jer_kaps_search_config_info[unit][0], 0, sizeof(JER_KAPS_SEARCH_CONFIG)*JER_KAPS_NOF_SEARCHES);

    jer_kaps_sw_init(unit);

    SOC_SAND_EXIT_AND_SEND_ERROR( "error in  jer_kaps_deinit_db_set()", 0, 0);

}

STATIC 
int
    jer_kaps_deinit_device(
       SOC_SAND_IN int unit
    )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    KBP_TRY(kbp_device_destroy(JerAppData[unit]->kaps_device_p));

#ifdef USE_MODEL
    /* Cleanup the model */
    KBP_TRY(kbp_sw_model_destroy(JerAppData[unit]->kaps_xpt_p));
#else
#ifndef BLACKHOLE_MODE
    jer_pp_xpt_deinit(unit, JerAppData[unit]->kaps_xpt_p);
#endif
#endif

    ARAD_DO_NOTHING_AND_EXIT;
           
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in jer_kaps_deinit_device()", 0, 0);
}

uint32 
    jer_kaps_deinit_app(
        int unit
    )
{
    uint32 res;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if(NULL == JerAppData[unit])
    {
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "Error in %s(): kaps not initialized.\n"), FUNCTION_NAME()));
        ARAD_DO_NOTHING_AND_EXIT;
    }

    res = jer_kaps_deinit_db_set(unit); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = jer_kaps_deinit_device(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    /* Free the default allocator */
    KBP_TRY(default_allocator_destroy(JerAppData[unit]->dalloc_p));

    ARAD_FREE(JerAppData[unit]);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_deinit_app()", unit, 0x0);
}

int
jer_kaps_autosync_set(int unit, int enable)
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    ARAD_DO_NOTHING_AND_EXIT;
           
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in kbp_sync()", 0, 0);
}

int
 jer_kaps_sync(int unit)
{
    kbp_warmboot_t *warmboot_data;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    warmboot_data = &kaps_warmboot_data[unit];

    KBP_TRY(kbp_device_save_state(JerAppData[unit]->kaps_device_p, warmboot_data->kbp_file_read , warmboot_data->kbp_file_write, warmboot_data->kbp_file_fp));
   
    ARAD_DO_NOTHING_AND_EXIT;
           
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in kbp_sync()", 0, 0);
}

void 
  jer_kaps_warmboot_register(int unit,
                             FILE *file_fp,
                             kbp_device_issu_read_fn read_fn, 
                             kbp_device_issu_write_fn write_fn)
{
    kaps_warmboot_data[unit].kbp_file_fp = file_fp;
    kaps_warmboot_data[unit].kbp_file_read = read_fn;
    kaps_warmboot_data[unit].kbp_file_write = write_fn;
}

/* 
 *  Table Get functions
 */
void jer_kaps_db_get(
   int unit,
   uint32 tbl_id,
   struct kbp_db **db_p)
{
    *db_p = jer_kaps_table_config_info[unit][tbl_id].tbl_p;
}

void jer_kaps_ad_db_get(
   int unit,
   uint32 tbl_id,
   struct kbp_ad_db **ad_db_p)
{
    uint32 db_id;

    db_id = jer_kaps_table_config_info[unit][tbl_id].db_id;
    *ad_db_p = jer_kaps_db_config_info[unit][db_id].ad_db_p;
}

void jer_kaps_dma_db_get(
   int unit,
   uint32 db_id,
   struct kbp_dma_db **db_p)
{

    *db_p = jer_kaps_dma_db_config_info[unit][db_id].dma_db_p;
}


void jer_kaps_search_config_get(
   int unit,
   uint32 search_id,
   JER_KAPS_SEARCH_CONFIG *search_cfg_p)
{
    *search_cfg_p = jer_kaps_search_config_info[unit][search_id];
}

void jer_kaps_table_config_get(
   int unit,
   uint32 tbl_id,
   JER_KAPS_TABLE_CONFIG *table_cfg_p)
{
    *table_cfg_p = jer_kaps_table_config_info[unit][tbl_id];
}

/* 
 *  Test functions
 */
int jer_kaps_search_generic(int unit, uint32 search_id, uint8 key[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES][JER_KAPS_KEY_BUFFER_NOF_BYTES],
                            uint32 *return_is_matched, uint32 *return_prefix_len, uint8  return_payload[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES][BITS2BYTES(JER_KAPS_AD_WIDTH_IN_BITS)],
                            struct kbp_search_result *return_cmp_rslt)
{
    uint32 rv;
    uint32 soc_sand_rv;
    struct kbp_search_result cmp_rslt, *cmp_rslt_p;
    int32 i, j=0, is_matched[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES], prefix_len[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES];
    uint8 payload[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES][BITS2BYTES(JER_KAPS_AD_WIDTH_IN_BITS)];

    JER_KAPS_IP_TBL_ID *table_id = jer_kaps_search_config_info[unit][search_id].tbl_id;
    uint8 master_key[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES * JER_KAPS_KEY_BUFFER_NOF_BYTES];

    memset(&cmp_rslt, 0, sizeof(cmp_rslt));

    for(i = 0; i<JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES; i++) {
		is_matched[i] = 0;
		prefix_len[i] = 0;
		/*copy the key*/
        for (j=0; j < JER_KAPS_KEY_BUFFER_NOF_BYTES; j++){
            master_key[i*JER_KAPS_KEY_BUFFER_NOF_BYTES + j] = key[i][j];
        }
		/*zero the payload*/
        for (j=0; j < BITS2BYTES(JER_KAPS_AD_WIDTH_IN_BITS); j++){
            payload[i][j] = 0;
        }
    }

    cli_out("\n\rSW search\n\r---------\n\r");

    for(i = 0; i<JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES; i++) {
        soc_sand_rv = jer_kaps_sw_search_test(unit, table_id[i], key[i], payload[i], &prefix_len[i], &is_matched[i]);
        rv = handle_sand_result(soc_sand_rv);
        if (BCM_FAILURE(rv)) {
            cli_out("Error: jer_kaps_sw_search_test(%d)\n", unit);
            return rv;
        }

        if (return_is_matched != NULL) {
            return_is_matched[i] = is_matched[i];
        }
        if (return_prefix_len != NULL) {
            return_prefix_len[i] = prefix_len[i];
        }
        if (return_payload != NULL) {
            for (j = 0; j < BITS2BYTES(JER_KAPS_AD_WIDTH_IN_BITS); j++) {
                return_payload[i][j] =  payload[i][j];
            }
        }

        if (is_matched[i]) {
            cli_out("SW search %d in %s: matched!, payload:0x", i, JER_KAPS_TABLE_NAMES[table_id[i]]);
            for (j = 0; j < BITS2BYTES(JER_KAPS_AD_WIDTH_IN_BITS-7); j++) {
                cli_out("%02x", payload[i][j]);
            }
            /*print the last 4 bits only*/
            cli_out("%x", payload[i][j] >> (BYTES2BITS(BITS2BYTES(JER_KAPS_AD_WIDTH_IN_BITS))-JER_KAPS_AD_WIDTH_IN_BITS));
            cli_out(", prefix_len:%d\n", prefix_len[i]);
        }
        else {
            cli_out("SW search %d in %s: no match\n", i, JER_KAPS_TABLE_NAMES[table_id[i]]);
        }
    }

    cli_out("\n\rHW search\n\r---------\n\r");

    /* If we return the hw compare result, use the passed struct instead */
    if (return_cmp_rslt != NULL) {
        cmp_rslt_p = return_cmp_rslt;
    } else {
        cmp_rslt_p = &cmp_rslt;
    }

    soc_sand_rv = jer_kaps_hw_search_test(unit, search_id, master_key, cmp_rslt_p);
    rv = handle_sand_result(soc_sand_rv);
    if (BCM_FAILURE(rv)) {
        cli_out("Error: jer_kaps_hw_search_test(%d)\n", unit);
        return rv;
    }



    for(i = 0; i<JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES; i++) {
        if (cmp_rslt_p->hit_or_miss[i]) {
            cli_out("HW search %d in %s: matched!, payload:0x", i, JER_KAPS_TABLE_NAMES[table_id[i]]);
            for (j = 0; j < BITS2BYTES(JER_KAPS_AD_WIDTH_IN_BITS-7); j++) {
                cli_out("%02x", cmp_rslt_p->assoc_data[i][j]);
            }
            /*print the last 4 bits only*/
            cli_out("%x", cmp_rslt_p->assoc_data[i][j] >> (BYTES2BITS(BITS2BYTES(JER_KAPS_AD_WIDTH_IN_BITS))-JER_KAPS_AD_WIDTH_IN_BITS));
			cli_out("\n");
        }
        else {
            cli_out("HW search %d in %s: no match\n", i, JER_KAPS_TABLE_NAMES[table_id[i]]);
        }
    }

    return rv;
}

int jer_kaps_sw_search_test(int unit, uint32 tbl_id, uint8 *key, uint8 *payload, int32 *prefix_len, int32 *is_matched)
{
    struct kbp_db *db_p;
    struct kbp_ad_db *ad_db_p;
    struct kbp_entry *entry_p = NULL;
    struct kbp_ad *ad_entry_p = NULL;
    int32 index, db_id, clone_db, res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    db_p = jer_kaps_table_config_info[unit][tbl_id].tbl_p; /* table/DB handle */
    db_id = jer_kaps_table_config_info[unit][tbl_id].db_id; /* table's DB */
    clone_db = jer_kaps_db_config_info[unit][db_id].clone_of_db_id;

    if (clone_db == JER_KAPS_IP_NOF_DB) {
        /* not a clone */
        ad_db_p = jer_kaps_db_config_info[unit][db_id].ad_db_p;
    }
    else {
        /* DB is a clone. Get AD DB of the original DB. */
        ad_db_p = jer_kaps_db_config_info[unit][clone_db].ad_db_p;
    }

    res = kbp_db_search(db_p, key, &entry_p, &index, prefix_len);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "Error in %s(): kbp_db_search failed with : %s!\n"), 
                             FUNCTION_NAME(),
                  kbp_get_status_string(res)));

        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
    }

    if (entry_p) {
        *is_matched = TRUE;
        kbp_entry_get_ad(db_p, entry_p, &ad_entry_p);
        kbp_ad_db_get(ad_db_p, ad_entry_p, payload);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in jer_kaps_search_test()", 0, 0);
}

int jer_kaps_hw_search_test(int unit, uint32 search_id, uint8 *master_key, struct kbp_search_result *cmp_rslt)
{
    int32 res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = kbp_instruction_search(jer_kaps_search_config_info[unit][search_id].inst_p, master_key, 0, cmp_rslt);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "Error in %s(): kbp_instruction_search failed with : %s!\n"), 
                             FUNCTION_NAME(),
                  kbp_get_status_string(res)));

        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in jer_kaps_search_test()", 0, 0);
}

/* Prints the contents of the kaps table */
int jer_kaps_show_table(int unit, uint32 dbal_table_id) {
    struct kbp_entry_iter *iter;
    struct kbp_entry *kpb_e;
    struct kbp_entry_info kpb_e_info;

    SOC_DPP_DBAL_TABLE_INFO dbal_table;
    char qual_name[20];

    JER_KAPS_DB *tbl_p;
    uint32 i, j;
    uint32 res;

    uint8 db_id = 0;

    uint8 ad_8[BITS2BYTES(JER_KAPS_AD_WIDTH_IN_BITS)];

    uint8 mask_width_8;
    uint32 prio_len, temp_prio_len;
    uint32 qual_prio_len;
    uint32 qual_max_size; /* The maximal number of bits of a qual in a key */
    uint32 temp_zero_padding, zero_padding; /*used to calculate the number of bit padding added to key */
    uint32 qual_tbl_index, qual_type_tbl_index = 0;
    uint32 qual_print_len;
    uint32 tbl_idx;

    uint8 header_flag = 1;

    ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    DBAL_QUAL_VALS_CLEAR(qual_vals);

    res = sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, dbal_table_id, &dbal_table);
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

    if (dbal_table.physical_db_type != SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS) {
        cli_out("Provided DBAL_TBL_ID is not associated with KAPS.\n");
        ARAD_DO_NOTHING_AND_EXIT;
    }

    /*Print from both private and public tables*/
    for (j=0; j<2; j++) {
        res = jer_pp_kaps_dbal_table_id_translate(unit, dbal_table_id, NULL/*qual_vals*/, j/*private_table*/, &tbl_idx/*kaps*/);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        if ((tbl_idx < JER_KAPS_IP_NOF_TABLES) && (tbl_idx >= 0)) {
            tbl_p = jer_kaps_table_config_info[unit][tbl_idx].tbl_p;
            db_id = jer_kaps_table_config_info[unit][tbl_idx].db_id;
        } else {
            tbl_p = NULL;
        }

        if (tbl_idx < JER_KAPS_IP_PUBLIC_INDEX) {
            cli_out("\n\rDBAL Table ID %d: %s - Private DB"
                    "\n\r---------------------------------------------------------------"
                    "-------------------------------------------------------------------"
                    "--------------------------------------------------------------------\n\r",
                    dbal_table_id, dbal_table.table_name);
        } else {
            cli_out("\n\rDBAL Table ID %d: %s - Public DB"
                    "\n\r---------------------------------------------------------------"
                    "-------------------------------------------------------------------"
                    "--------------------------------------------------------------------\n\r",
                    dbal_table_id, dbal_table.table_name);
        }

        header_flag = 1;

        /*Only print if the table is allocated*/
        if ((tbl_p != NULL) && (dbal_table.is_table_initiated)) {
            /*initialize iterator*/
            KBP_TRY(kbp_db_entry_iter_init(tbl_p, &iter));

            /*traverse entries*/
            do {
                KBP_TRY(kbp_db_entry_iter_next(tbl_p, iter, &kpb_e));

                if (kpb_e == NULL)
                    break;

                KBP_TRY(kbp_entry_get_info(tbl_p, kpb_e, &kpb_e_info));

                /* Verify that the table prefix is correct - dynamic tables share the KAPS table but do not necessarily share the dbal prefix*/
                if (kpb_e_info.data[0] >> (SOC_SAND_NOF_BITS_IN_BYTE - dbal_table.db_prefix_len) != dbal_table.db_prefix) {
                    continue;
                }

                /*print entry*/
                mask_width_8 = kpb_e_info.width_8;
                prio_len = kpb_e_info.prio_len;
                res = arad_pp_dbal_kbp_buffer_to_entry_key(unit, &dbal_table, prio_len, kpb_e_info.data, qual_vals);
                SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

                /*cli_out(header_flag ? "|Prefix_len:%03d " : "|           %03d ", prio_len);*/
                cli_out(header_flag? "|Raw_Data: 0x" : "|          0x");
                for (i = 0; i < mask_width_8; i++) {
                    cli_out("%02x", kpb_e_info.data[i]);
                }
                cli_out("/%03d", prio_len);
                cli_out(header_flag ? " |TBL:0x%02x/%01d/%01d" : " |    0x%02x/%01d/%01d", dbal_table.db_prefix, dbal_table.db_prefix_len, dbal_table.db_prefix_len);

                prio_len -= dbal_table.db_prefix_len;
                /* Remove the zero padding from the prio_len */
                temp_prio_len = prio_len;
                zero_padding = 0;
                temp_zero_padding = 0;
                qual_tbl_index = dbal_table.nof_qualifiers - 1; /* quals are listed in the table qual_info from LSB to MSB*/
                while (qual_tbl_index != -1) {
                    if (dbal_table.qual_info[qual_tbl_index].qual_type == SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES) {
                         temp_zero_padding += dbal_table.qual_info[qual_tbl_index].qual_nof_bits;
                    } else { /* Only count the zeros within the key */
                        if (temp_prio_len > 0) {
                            zero_padding += temp_zero_padding;
                            temp_zero_padding = 0;
                        } else {
                            break;
                        }
                    }
                    temp_prio_len -= dbal_table.qual_info[qual_tbl_index].qual_nof_bits;

                    qual_tbl_index--;
                }
                prio_len -= zero_padding;

                for (i=0; i < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; i++) {
                    if (((qual_vals[i].type != SOC_PPC_NOF_FP_QUAL_TYPES) && (qual_vals[i].type != SOC_PPC_FP_QUAL_IRPP_ALL_ONES)
                        && (qual_vals[i].type != SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES)) && (qual_vals[i].type != BCM_FIELD_ENTRY_INVALID)) { /*Print all fields except ones/zeroes*/

                        if (qual_vals[i].type > SOC_PPC_NOF_FP_QUAL_TYPES) {
                            cli_out("\nUnexpected qual type!\n");
                            ARAD_DO_NOTHING_AND_EXIT;
                        }

                        switch (qual_vals[i].type) {/*Pretty print basic common qualifiers - shorter than ARAD_PP_FP_QUAL_TYPE_to_string*/
                        case SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP:
                            sal_strcpy(qual_name, "DIPv4");
                            qual_print_len = 8;
                            break;
                        case SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP:
                            sal_strcpy(qual_name, "SIPv4");
                            qual_print_len = 8;
                            break;
                        case SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW:
                            sal_strcpy(qual_name, "DIPv6_L");
                            qual_print_len = 8;
                            break;
                        case SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH:
                            sal_strcpy(qual_name, "DIPv6_H");
                            qual_print_len = 8;
                            break;
                        case SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_LOW:
                            sal_strcpy(qual_name, "SIPv6_L");
                            qual_print_len = 8;
                            break;
                        case SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH:
                            sal_strcpy(qual_name, "SIPv6_H");
                            qual_print_len = 8;
                            break;
                        case SOC_PPC_FP_QUAL_IRPP_VRF:
                            sal_strcpy(qual_name, "VRF");
                            qual_print_len = 4;
                            break;
                        case SOC_PPC_FP_QUAL_IRPP_IN_RIF:
                            sal_strcpy(qual_name, "INRIF");
                            qual_print_len = 4;
                            break;
                        default:
                            /* Attempt to retrieve the qual name string from function */
                            if (sal_strcmp(" Unknown", ARAD_PP_FP_QUAL_TYPE_to_string(qual_vals[i].type))) {
                                sal_strcpy(qual_name, ARAD_PP_FP_QUAL_TYPE_to_string(qual_vals[i].type));
                            } else {
                                sal_sprintf(qual_name, "Qual-%03d", qual_vals[i].type);
                            }
                            qual_print_len = 8;/*over 32 bits are printed as 64bit*/
                        }
                        header_flag ? cli_out(" |%s: 0x", qual_name) : cli_out(" |%*s  0x", strlen(qual_name), "");

                        qual_tbl_index = 0;
                        qual_max_size = 0;
                        while (qual_tbl_index != SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) {
                            if (dbal_table.qual_info[qual_tbl_index].qual_type == qual_vals[i].type) {
                                qual_max_size += dbal_table.qual_info[qual_tbl_index].qual_nof_bits;
                                qual_type_tbl_index = qual_tbl_index;
                            }
                            qual_tbl_index++;
                        }

                        /*In case this qualifier is longer than 32bits, print it all*/
                        if (dbal_table.qual_info[qual_type_tbl_index].qual_full_size > SOC_SAND_NOF_BITS_IN_UINT32) {
                            cli_out("%0*x", 8, qual_vals[i].val.arr[1]);
                        }

                        /* Calculate the qual prefix length */
                        qual_prio_len = prio_len > qual_max_size ? qual_max_size : prio_len;
                        prio_len -= qual_prio_len;
                        cli_out("%0*x/%02d/%02d", qual_print_len, qual_vals[i].val.arr[0], qual_prio_len, qual_max_size);
                    }
                }

                if ((jer_kaps_db_config_info[unit][db_id].ad_db_p != NULL) && (kpb_e_info.ad_handle != NULL)) {
                    cli_out(header_flag ? " |AD: 0x" : " |    0x");
                    KBP_TRY(kbp_ad_db_get(jer_kaps_db_config_info[unit][db_id].ad_db_p, kpb_e_info.ad_handle, ad_8));
                    for (i=0; i < BITS2BYTES(JER_KAPS_AD_WIDTH_IN_BITS - 7); i++ ) {
                        cli_out("%02x", ad_8[i]);
                    }
                    /*print the last 4 bits only*/
                    cli_out("%01x/%02d", ad_8[i] >> (BYTES2BITS(BITS2BYTES(JER_KAPS_AD_WIDTH_IN_BITS))-JER_KAPS_AD_WIDTH_IN_BITS), JER_KAPS_AD_WIDTH_IN_BITS);
                }
                cli_out("|\n\r");
                header_flag = 0;
            } while (1);

            /*reclaim iterator memory*/
            KBP_TRY(kbp_db_entry_iter_destroy(tbl_p, iter));
        } else {
            cli_out("\n\rTable %d not allocated\n\r----------------------------------------------------------------------------\n\r", tbl_idx);
        }
    }

    ARAD_DO_NOTHING_AND_EXIT;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in jer_kaps_show_table()", 0, 0);
}

/* kaps write/read test diagnostic tool */
int jer_pp_kaps_diag_01(int unit)
{
    int32 res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = kaps_diag_01(JerAppData[unit]->kaps_xpt_p);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "\n Overall Result: Failed, Reason Code: %s\n"),
                  kbp_get_status_string(res)));

       SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
    }
    else
    {
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "\n Overall Result: Passed\n")));
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in jer_pp_kaps_diag_01()", 0, 0);
}

/* kaps search test diagnostic tool */
int jer_pp_kaps_diag_02(int unit)
{
    int32 res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = kaps_diag_02(JerAppData[unit]->kaps_xpt_p);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "\n Overall Result: Failed, Reason Code: %s\n"),
                  kbp_get_status_string(res)));

       SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
    }
    else
    {
       LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "\n Overall Result: Passed\n")));
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in jer_pp_kaps_diag_02()", 0, 0);
}

/* KAPS RPB TCAM built in self test */
int jer_pp_kaps_tcam_bist(int unit)
{
    int32 res;
    uint32 blk_id, cam_index, nbytes = sizeof(uint32);
    uint8 data[4]={0}, res_data[4]={0}, expected_data[4]={0};

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* set the expected status */
    expected_data[0] = 0;
    expected_data[1] = 0;
    expected_data[2] = 0;
    expected_data[3] = 3;

    for (blk_id = JER_KAPS_RPB_BLOCK_INDEX_START; blk_id <= JER_KAPS_RPB_BLOCK_INDEX_END; blk_id++) {
        for(cam_index = 0; cam_index < 2; cam_index++) {
            sal_memset(data, 0, sizeof(uint32));
            sal_memset(res_data, 0, sizeof(uint32));
            data[3] = cam_index + 1;
            res = jer_pp_kaps_write_command(JerAppData[unit]->kaps_xpt_p,  
                                            blk_id, 
                                            KAPS_CMD_WRITE,
                                            KAPS_FUNC0,
                                            JER_KAPS_RPB_CAM_BIST_CONTROL_REG_ADDR,
                                            nbytes, 
                                            data);
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
               LOG_ERROR(BSL_LS_SOC_TCAM,
                         (BSL_META_U(unit,
                                     "\n jer_pp_kaps_write_command Failed, Reason Code: %s\n"),
                          kbp_get_status_string(res)));

               SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
            }

            res = jer_pp_kaps_read_command(JerAppData[unit]->kaps_xpt_p,  
                                           blk_id, 
                                           KAPS_CMD_READ,
                                           KAPS_FUNC0,
                                           JER_KAPS_RPB_CAM_BIST_STATUS_REG_ADDR,
                                           nbytes, 
                                           res_data);
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
               LOG_ERROR(BSL_LS_SOC_TCAM,
                         (BSL_META_U(unit,
                                     "\n jer_pp_kaps_read_command Failed, Reason Code: %s\n"),
                          kbp_get_status_string(res)));

               SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
            }

            if (sal_memcmp(res_data, expected_data, 4) != 0) {
               LOG_ERROR(BSL_LS_SOC_TCAM,
                         (BSL_META_U(unit,
                                     "\n unexpected regitser value \n")));

               SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
            }
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in jer_pp_kaps_tcam_bist()", 0, 0);
}

int jer_pp_kbp_sdk_ver_test(int unit)
{
    int32 res;
    uint32 regval, i, array_index = 0;
    /* The following three arrays hold the compatible kbp_sdk_ver and sw_sdk_ver.
     * The first sw_sdk_ver that supports the corresponding kbp_sdk_ver is listed in the same array index.
     * We need to maintain in parallel the compatible version for 6_4 and 6_5 branches.
     */
    uint32 number_of_elements;
    const char* kbp_sdk_ver_arr[] =       {"KBP SDK 1.3.11"};
    const uint32 sw_sdk_ver_arr_6_5[]  = {0x65100000};
    const uint32 sw_sdk_ver_arr_6_4[]  = {0x64700000};

    const uint32 *relevant_array;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = soc_reg32_get(unit, ECI_SW_VERSIONr, REG_PORT_ANY,  0, &regval);
    if (res != SOC_SAND_OK) {
        cli_out("Error: jer_pp_kbp_sdk_ver_test(%d), soc_reg32_get\n", unit);
        return res;
    }

    /* Find if we are on the 6_4 branch or 6_5 */
    if (regval < sw_sdk_ver_arr_6_5[0]) {
        relevant_array = sw_sdk_ver_arr_6_4;
    } else {
        relevant_array = sw_sdk_ver_arr_6_5;
    }

    /* Find the closest relevant sw_sdk_ver in the array */
    number_of_elements = sizeof(sw_sdk_ver_arr_6_5)/sizeof(sw_sdk_ver_arr_6_5[0]);
    for (i=0; i<number_of_elements; i++) {
        if (regval >= relevant_array[i]) {
            array_index = i;
        }
    }

    if (sal_strcmp(kbp_sdk_ver_arr[array_index], kbp_device_get_sdk_version())) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                 (BSL_META_U(unit,
                             "\n kbp_sdk_ver and sw_sdk_ver do not match. Expected: %s. Current: %s.\n"),
                  kbp_sdk_ver_arr[array_index], kbp_device_get_sdk_version()));

       SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in jer_pp_kbp_sdk_ver_test()", 0, 0);
}

STATIC
int jer_pp_kaps_verify_search_results(int unit, uint32 is_matched[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES], uint32 expected_is_matched[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES],
                                      uint32 search_payload_32[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES],
                                      uint32 expected_search_payload_32[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES],
                                      uint32 prefix_len_array[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES], uint32 expected_prefix_len_array[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES])
{
    uint32 i;

    SOCDNX_INIT_FUNC_DEFS;
    for (i=0; i<JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES; i++) {
        if (is_matched[i] == expected_is_matched[i]) {
            if (search_payload_32[i] == expected_search_payload_32[i]) {
                if ((prefix_len_array != NULL) && (prefix_len_array[i] != expected_prefix_len_array[i])) {
                    cli_out("search verification failed, wrong prefix length!\nDetails: Search id: %d Prefix_len: %d Expected_prefix_len: %d \n",
                            i, prefix_len_array[i], expected_prefix_len_array[i]);
                    SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_INTERNAL);
                } else{
                    cli_out("Search %d verified!\n", i);
                }
            } else {
                cli_out("Search verification failed, wrong payloads! \nDetails: Search id: %d Payload: 0x%x Expected_payload: 0x%x \n",
                            i, search_payload_32[i], expected_search_payload_32[i]);
                SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_INTERNAL);
            }
        } else {
            cli_out("Search verification failed, hit-match mismatch! \nDetails: Search id: %d Match: %d Expected_match: %d \n",
                        i, is_matched[i], expected_is_matched[i]);
            SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_INTERNAL);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/* KAPS RPB TCAM built in self test */
int jer_pp_kaps_search_test(int unit, uint32 add_entries, uint32 search_entries, uint32 delete_entries)
{
    uint32 res;

    SOC_SAND_PP_IPV6_ADDRESS dip_v6_32,
           sip_v6_32,
        mc_v6_32;
    uint32 dip_v4 = 0, sip_v4 = 0, mc_v4 = 0, vrf = 0, inrif = 0;

    uint32 i,j,k;
    uint8 key[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES][JER_KAPS_KEY_BUFFER_NOF_BYTES];

    uint32 payload;

    JER_KAPS_SEARCH_CONFIG search_cfg;

    SOC_SAND_SUCCESS_FAILURE success;

    uint32  prefix_len;
    ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    uint32 mask[SOC_SAND_PP_IPV6_ADDRESS_NOF_UINT32S] = {SOC_SAND_U32_MAX, SOC_SAND_U32_MAX, SOC_SAND_U32_MAX, SOC_SAND_U32_MAX};
    uint32 partial_mask[SOC_SAND_PP_IPV6_ADDRESS_NOF_UINT32S] = {0, 0, SOC_SAND_U32_MAX, SOC_SAND_U32_MAX};
    SOC_DPP_DBAL_SW_TABLE_IDS table_id;
    SOC_DPP_DBAL_TABLE_INFO table;

    struct kbp_search_result cmp_rslt;
    uint32 is_matched[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES], prefix_len_array[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES];
    uint32 expected_is_matched[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES], expected_prefix_len_array[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES];
    uint8 search_payload[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES][BITS2BYTES(JER_KAPS_AD_WIDTH_IN_BITS)];
    uint32 expected_search_payload[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES];

    uint32 search_payload_32[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES];

    SOCDNX_INIT_FUNC_DEFS;

    dip_v6_32.address[0] = 0x12345678;
    dip_v6_32.address[1] = 0x9abcdef0;
    dip_v6_32.address[2] = 0xfedcba98;
    dip_v6_32.address[3] = 0x76543210;

    sip_v6_32.address[0] = 0x01020304;
    sip_v6_32.address[1] = 0x05060708;
    sip_v6_32.address[2] = 0x090a0b0c;
    sip_v6_32.address[3] = 0x0d0e0f00;

    mc_v6_32.address[0] = 0x01020304;
    mc_v6_32.address[1] = 0x05060708;
    mc_v6_32.address[2] = 0x090a0b0c;
    mc_v6_32.address[3] = 0x0d0e0f00;

    dip_v4 = 0xa1b2c3d4;
    sip_v4 = 0xf9e8d7c6;
    mc_v4 =  0xf13579bd;

    vrf = 5;
    inrif = 3;

    #define IPV4_DIP_PAYLOAD 0x12345
    #define IPV4_SIP_PAYLOAD 0x54321
    #define IPV4_MC_PAYLOAD  0xabcde

    #define IPV6_DIP_PAYLOAD 0x67890
    #define IPV6_SIP_PAYLOAD 0x98765
    #define IPV6_MC_PAYLOAD  0xfedcb

    /* Initialize qual_vals to include all the possible quals in the searches*/
    DBAL_QUAL_VALS_CLEAR(qual_vals);
    DBAL_QUAL_VAL_ENCODE_IPV4_SIP(&qual_vals[0], sip_v4, SOC_SAND_NOF_BITS_IN_UINT32);
    DBAL_QUAL_VAL_ENCODE_IPV6_SIP_LOW(&qual_vals[1], sip_v6_32.address, mask);
    DBAL_QUAL_VAL_ENCODE_IPV6_SIP_HIGH(&qual_vals[2], sip_v6_32.address, mask);
    DBAL_QUAL_VAL_ENCODE_IPV4_DIP(&qual_vals[3], dip_v4, SOC_SAND_NOF_BITS_IN_UINT32);
    DBAL_QUAL_VAL_ENCODE_IPV6_DIP_LOW(&qual_vals[4], dip_v6_32.address, mask);
    DBAL_QUAL_VAL_ENCODE_IPV6_DIP_HIGH(&qual_vals[5], dip_v6_32.address, mask);
    DBAL_QUAL_VAL_ENCODE_IN_RIF(&qual_vals[6], inrif, SOC_SAND_U32_MAX);
    DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[7], vrf);

    if (add_entries) {
        /* Add the ipv4 dip into the ipv4-uc table, fully masked, private entry */
        payload = IPV4_DIP_PAYLOAD;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS, qual_vals, 0/*priority*/,  &payload, &success));

        /* Add the ipv4 sip into the ipv4-uc table, partially masked, public entry */
        payload = IPV4_SIP_PAYLOAD;
        DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[7], 0);
        DBAL_QUAL_VAL_ENCODE_IPV4_DIP(&qual_vals[3], sip_v4, SOC_SAND_NOF_BITS_IN_UINT32/2);
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS, qual_vals, 0/*priority*/,  &payload, &success));

        /* Add the ipv4 mc into the ipv4-mc table, fully-masked, private entry */
        payload = IPV4_MC_PAYLOAD;
        DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[7], vrf);
        DBAL_QUAL_VAL_ENCODE_IPV4_DIP(&qual_vals[3], mc_v4, SOC_SAND_NOF_BITS_IN_UINT32);
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS, qual_vals, 0/*priority*/,  &payload, &success));

        /* Add the ipv6 dip into the ipv6-uc table, fully masked, public entry */
        payload = IPV6_DIP_PAYLOAD;
        DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[7], 0);
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_KAPS, qual_vals, 0/*priority*/,  &payload, &success));

        /* Add the ipv6 sip into the ipv6-uc table, partially masked, private entry */
        payload = IPV6_SIP_PAYLOAD;
        DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[7], vrf);
        DBAL_QUAL_VAL_ENCODE_IPV6_DIP_LOW(&qual_vals[4], sip_v6_32.address, partial_mask);
        DBAL_QUAL_VAL_ENCODE_IPV6_DIP_HIGH(&qual_vals[5], sip_v6_32.address, partial_mask);
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_KAPS, qual_vals, 0/*priority*/,  &payload, &success));

        /* Add the ipv6 mc into the ipv6-mc table, fully-masked, public entry */
        payload = IPV6_MC_PAYLOAD;
        DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[7], 0);
        DBAL_QUAL_VAL_ENCODE_IPV6_DIP_LOW(&qual_vals[4], mc_v6_32.address, mask);
        DBAL_QUAL_VAL_ENCODE_IPV6_DIP_HIGH(&qual_vals[5], mc_v6_32.address, mask);
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_KAPS, qual_vals, 0/*priority*/,  &payload, &success));
    }

    for (k=0; k < 2; k++) { /* Second pass runs with all expected zero after a table clear*/
        SOCDNX_IF_ERR_EXIT(jer_pp_xpt_wait_arm_done(unit));
        if (search_entries) {
            for (j=0; j < JER_KAPS_NOF_SEARCHES; j++) {
                jer_kaps_search_config_get(unit, j, &search_cfg);
                if (!search_cfg.valid) {
                    LOG_ERROR(BSL_LS_SOC_TCAM,
                         (BSL_META_U(unit,
                                     "\n Search config %d disabled.\n"), j));
                    SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_INTERNAL);
                }

                cli_out("\n-----------------------------\nTesting KAPS search config %d - Tables %s.\n-----------------------------\n", j, k ? "cleared" : "filled");

                for (i=0; i < JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES; i++) {
                    /*vrf!=0 only for private*/
                    if (search_cfg.tbl_id[i] < JER_KAPS_IP_PUBLIC_INDEX) {
                        DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[7], vrf);
                    } else{
                        DBAL_QUAL_VAL_ENCODE_VRF(&qual_vals[7], 0);
                    }

                    res = jer_pp_kaps_table_id_to_dbal_translate(unit, search_cfg.tbl_id[i], &table_id);
                    if (res != SOC_SAND_OK) {
                        cli_out("Error: jer_pp_kaps_search_test(%d), table translation failed\n", unit);
                        SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_INTERNAL);
                    }

                    res = sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table);
                    if (res != SOC_SAND_OK) {
                        cli_out("Error: jer_pp_kaps_search_test(%d), get dbal table failed\n", unit);
                        SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_INTERNAL);
                    }

                    /* MC entries utilize the dip qual, need to change it per ipv4/ipv6 table */
                    if (table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS) {
                        DBAL_QUAL_VAL_ENCODE_IPV4_DIP(&qual_vals[3], dip_v4, SOC_SAND_NOF_BITS_IN_UINT32);
                    } else if (table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS) {
                        DBAL_QUAL_VAL_ENCODE_IPV4_DIP(&qual_vals[3], mc_v4, SOC_SAND_NOF_BITS_IN_UINT32);
                    } else if (table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_KAPS) {
                        DBAL_QUAL_VAL_ENCODE_IPV6_DIP_LOW(&qual_vals[4], dip_v6_32.address, mask);
                        DBAL_QUAL_VAL_ENCODE_IPV6_DIP_HIGH(&qual_vals[5], dip_v6_32.address, mask);
                    } else if (table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_KAPS) {
                        DBAL_QUAL_VAL_ENCODE_IPV6_DIP_LOW(&qual_vals[4], mc_v6_32.address, mask);
                        DBAL_QUAL_VAL_ENCODE_IPV6_DIP_HIGH(&qual_vals[5], mc_v6_32.address, mask);
                    }

                    res = arad_pp_dbal_entry_key_to_kbp_buffer(unit, &table, JER_KAPS_KEY_BUFFER_NOF_BYTES, qual_vals, &prefix_len, key[i]);
                    if (res != SOC_SAND_OK) {
                        cli_out("Error: jer_pp_kaps_search_test(%d), key buffer construction failed\n", unit);
                        SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_INTERNAL);
                    }
                }

                res = jer_kaps_search_generic(unit, j, key, is_matched, prefix_len_array, search_payload, &cmp_rslt);
                if (res != SOC_SAND_OK) {
                    cli_out("Error: jer_kaps_search_generic(%d)\n", unit);
                    SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_INTERNAL);
                }

                /* Verify expected search results */
                if (k == 0) {
                    if (j == 0) {
                        expected_is_matched[0] = 0; expected_is_matched[1] = 1; expected_is_matched[2] = 1; expected_is_matched[3] = 0;
                        expected_search_payload[0] = 0; expected_search_payload[1] = IPV4_SIP_PAYLOAD; expected_search_payload[2] = IPV4_DIP_PAYLOAD; expected_search_payload[3] = 0;
                        expected_prefix_len_array[0] = 0; expected_prefix_len_array[1] = 32; expected_prefix_len_array[2] = 48; expected_prefix_len_array[3] = 0;
                    } else if (j == 1) {
                        expected_is_matched[0] = 1; expected_is_matched[1] = 0; expected_is_matched[2] = 0; expected_is_matched[3] = 1;
                        expected_search_payload[0] = IPV6_SIP_PAYLOAD; expected_search_payload[1] = 0; expected_search_payload[2] = 0; expected_search_payload[3] = IPV6_DIP_PAYLOAD;
                        expected_prefix_len_array[0] = 80; expected_prefix_len_array[1] = 0; expected_prefix_len_array[2] = 0; expected_prefix_len_array[3] = 144;
                    } else if (j == 2) {
                        expected_is_matched[0] = 0; expected_is_matched[1] = 1; expected_is_matched[2] = 1; expected_is_matched[3] = 0;
                        expected_search_payload[0] = 0; expected_search_payload[1] = IPV4_SIP_PAYLOAD; expected_search_payload[2] = IPV4_MC_PAYLOAD; expected_search_payload[3] = 0;
                        expected_prefix_len_array[0] = 0; expected_prefix_len_array[1] = 32; expected_prefix_len_array[2] = 92; expected_prefix_len_array[3] = 0;
                    } else if (j == 3) {
                        expected_is_matched[0] = 1; expected_is_matched[1] = 0; expected_is_matched[2] = 0; expected_is_matched[3] = 1;
                        expected_search_payload[0] = IPV6_SIP_PAYLOAD; expected_search_payload[1] = 0; expected_search_payload[2] = 0; expected_search_payload[3] = IPV6_MC_PAYLOAD;
                        expected_prefix_len_array[0] = 80; expected_prefix_len_array[1] = 0; expected_prefix_len_array[2] = 0; expected_prefix_len_array[3] = 159;
                    }
                } else { /* Second pass after the tables clear */
                    expected_is_matched[0] = 0; expected_is_matched[1] = 0; expected_is_matched[2] = 0; expected_is_matched[3] = 0;
                    expected_search_payload[0] = 0; expected_search_payload[1] = 0; expected_search_payload[2] = 0; expected_search_payload[3] = 0;
                    expected_prefix_len_array[0] = 0; expected_prefix_len_array[1] = 0; expected_prefix_len_array[2] = 0; expected_prefix_len_array[3] = 0;
                }

                cli_out("\nVerifying HW search config %d: \n", j);
                for (i=0; i < JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES; i++) {
                    SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_route_to_kaps_payload_buffer_decode(unit, cmp_rslt.assoc_data[i], &search_payload_32[i]));
                }
                SOCDNX_IF_ERR_EXIT(jer_pp_kaps_verify_search_results(unit, cmp_rslt.hit_or_miss, expected_is_matched, search_payload_32, expected_search_payload, NULL /*prefix*/, NULL));

                cli_out("\nVerifying SW search config %d: \n", j);
                for (i=0; i < JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES; i++) {
                    SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_route_to_kaps_payload_buffer_decode(unit, search_payload[i], &search_payload_32[i]));
                }
                SOCDNX_IF_ERR_EXIT(jer_pp_kaps_verify_search_results(unit, is_matched, expected_is_matched, search_payload_32, expected_search_payload, prefix_len_array, expected_prefix_len_array));
            }
        }

        if (delete_entries) {
            if (k == 0) {
                cli_out("\n-----------------------------\nClearing Tables.\n-----------------------------\n");
            }
            for (table_id = 0; table_id < SOC_DPP_DBAL_SW_NOF_TABLES; table_id++) {

                res = sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table);
                if (res != SOC_SAND_OK) {
                    cli_out("Error: jer_pp_kaps_search_test(%d), failed getting dbal table in clear\n", unit);
                    SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_INTERNAL);
                }

                if (table.physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS) {
                    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_clear(unit, table_id));
                }
            }
        } else {
            break;
        }
    }

exit:
  SOCDNX_FUNC_RETURN;
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif /* defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030) */
